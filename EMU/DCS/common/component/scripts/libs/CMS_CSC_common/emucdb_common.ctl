/**@file

This package contains general purpose utility functions to access DB.

@author Evaldas Juska (PH/UCM)
@date   March 2009
*/

const mapping emucdb_dummyMapping;
const string emucdb_CONNECTION_FILENAME = "emucdb_DBConnection";
const float emucdb_SCHEMA_VERSION = 1.0;

global string emucdb_g_connName = "emucdb_DB_connection";
global dbConnection emucdb_g_dbConn;

/** Opens connection to the EMU confDB and initializes the fwConfigurationDB. */
void emucdb_initialize() {
  dyn_string exceptionInfo;
  time t0;

  emu_debugFuncStart("emucdb_initialize", t0);

  //initialize fwConfigurationDB connection - always using default setup
  fwConfigurationDB_checkInit(exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  DebugTN("Connection to the fwConfigurationDB database OK");
  
  //initialize EMU CDB connection
  dyn_string connections;
  bool connectRequired = false;
  rdbGetConnectionNames(connections);
  if (!dynContains(connections, emucdb_g_connName)) {
    connectRequired = true;
  } else {
    dbConnection dbConn;
    int rc = rdbGetConnection(emucdb_g_connName, dbConn);
    if (rc != 0) {
      connectRequired = true;
    } else {
      rc = rdbIsConnectionValid(dbConn);
      if (rc != 0) {
        rdbCloseConnection(emucdb_g_connName);
        connectRequired = true;
      }
    }
  }
  if (connectRequired) {
    string connFile=getPath(CONFIG_REL_PATH, emucdb_CONNECTION_FILENAME);
    string connString;
    string err;
    dbConnection newConn;
    emucdb_g_dbConn = newConn;
    
    bool ok=fileToString(connFile, connString);
    if (!ok) {
      emu_error("EMU CDB connection file cannot be open");
      return;
    }
    emu_info("EMU CDB connection file opened successfully, connecting to DB...");
    rdbOpenConnection(connString, emucdb_g_dbConn, emucdb_g_connName);
    if (rdbCheckError(err, emucdb_g_dbConn)){ emu_error("Error while connecting to EMU CDB database: " + err); return;};
    emu_info("Connection to the EMU CDB database opened OK");
  }

  emu_info("Checking DB schema");  
  emucdb_checkSchema(exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  emu_info("DB Schema OK");
  
  emu_debugFuncEnd("emucdb_initialize", t0);
}

void emucdb_checkSchema(dyn_string &exceptionInfo) {
  time t0;
  emu_debugFuncStart("emucdb_checkSchema", t0);
  
  //check fwConfigurationDB schema
  float fwConfDbVersion = fwConfigurationDB_verifyDBSchema(g_fwConfigurationDB_DBConnection, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  if (fwConfDbVersion <= 0.0) {
    emu_error("fwConfigurationDB component not installed or there's a DB connection problem");
  }
  
  //check EMU CDB schema
  string err;
  string selectVersionSql = "select value from emucdb_parameters where name = :paramName";
  mapping bindVars;
  dbCommand cmd;
  bool isError;
  dyn_dyn_mixed sqlret;
  
  // we have to do all this stuff to execute sql manually because emucdb_executeSql or emucdb_querySingle* will report any errors and we want to keep it quite and handle the error
  bindVars["paramName"] = "emucdb_version";
  rdbStartCommand(emucdb_g_connName, selectVersionSql, cmd);
  if (rdbCheckError(err,cmd)) { isError = true; }
  rdbBindParams(cmd, bindVars);
  if (rdbCheckError(err,cmd)) { isError = true; }
  rdbExecuteCommand(cmd);
  if (rdbCheckError(err,cmd)) { isError = true; } 
  rdbGetData(cmd, sqlret, false, 0, makeDynInt(FLOAT_VAR));
  if (rdbCheckError(err,cmd)) { isError = true; } 
  rdbFinishCommand(cmd);
  if (rdbCheckError(err)) { isError = true; } 
    
  if (isError || (dynlen(sqlret) < 1) || (dynlen(sqlret[1]) < 1)) { //if there's an error - try to create it.
    emu_info("An error occured while retrieving EMU CDB version from DB. Trying to create schema...");
    emucdb_createDBSchema(exceptionInfo, false);
    if (emu_checkException(exceptionInfo)) { return; }
  } else if (sqlret[1][1] < emucdb_SCHEMA_VERSION) { // old schema - drop and create
    emu_addError("Old EMUCDB schema found (version " + schemaVersion + 
             "), this code is working with schema version " + emucdb_SCHEMA_VERSION + " or greater. " + 
             "Please drop the current schema and with the next execution EMUCDB will create the new one", 
             exceptionInfo);
    return;
  }

  emu_debugFuncEnd("emucdb_checkSchema", t0);
}

/** Creates EMU CDB DB schema. Stops on error. */
void emucdb_createDBSchema(dyn_string &exceptionInfo,bool dropExistingSchema=FALSE) {
  time t0;
  emu_debugFuncStart("emucdb_createDBSchema", t0);

  if (dropExistingSchema) {
    emu_info("Dropping existing schema");
    emucdb_dropDBSchema(exceptionInfo);
  }
  emu_info("Creating EMUCDB schema..");
  _fwConfigurationDB_executeSqlFromFile(emucdb_g_connName,
                                        "emucdb_createSchema.sql",
                                        "CREATE DB SCHEMA", exceptionInfo,TRUE);
  if (emu_checkException(exceptionInfo)) { return; }

  emu_info("[DONE] EMUCDB database schema succesfully created");
  emu_debugFuncEnd("emucdb_createDBSchema", t0);
}

/** Drops EMU CDB schema. Doesn't stop on error. */
void emucdb_dropDBSchema(dyn_string &exceptionInfo) {
  _fwConfigurationDB_executeSqlFromFile(emucdb_g_connName,
                                        "emucdb_dropSchema.sql",
                                        "DROP DB SCHEMA", exceptionInfo,FALSE);
}

/** Executes the sql statement and transposes the returned dataset (i.e. the resulting 2D array is [column][row]).
  @param sql                sql statement to be executed.
  @param exceptionInfo      any exceptions are reported here.
  @param transposeResults   If this is false (default) then the returned result dataset is row-wise (i.e. 2D array [row][column]).
                            Otherwise the returned result dataset is transposed - column-wise (i.e. 2D array [column][row]).
  @param bindVariables      bind variables. This is a "mapping" e.g. bindVariables[var_name] = var_value.
  @param datatypes          datatypes of the columns (e.g. makeDynInt(INT_VAR, INT_VAR, STRING_VAR)). 
                            If it's blank then all the values are converted to string (default).
*/
dyn_dyn_mixed emucdb_executeSql(string sql, dyn_string &exceptionInfo, 
                                bool transposeResults = false,
                                mapping bindVariables = emucdb_dummyMapping,
                                dyn_int datatypes = makeDynInt()) {
  dbCommand cmd;
  dyn_dyn_mixed data;
  time t0;
  emu_debugFuncStart("emucdb_executeSql", t0);
  
  cmd = _emucdb_prepareStatement(sql, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  data = _emucdb_executeCommand(cmd, exceptionInfo, true, transposeResults, bindVariables, datatypes);
  if (dynlen(exceptionInfo)) { emu_addError("Error when executing sql statement: " + sql, exceptionInfo); }
  if (emu_checkException(exceptionInfo)) { return; }
  
  emu_debugFuncEnd("emucdb_executeSql", t0);
  return data;
}

/** Executes a bulk command. Sql must contain at least one bound variable and the values of those should be given in data param.*/
void emucdb_executeBulk(string sql, dyn_dyn_mixed data, dyn_string &exceptionInfo){
  dbCommand cmd;
  string err;
  time t0;
  emu_debugFuncStart("emu_executeBulk", t0);
  
  cmd = _emucdb_prepareStatement(sql, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  rdbBindAndExecute(cmd, data);
  if (rdbCheckError(err)) {
    dynAppend(exceptionInfo, "DB ERROR: " + err);
    emu_addError("Error when executing bulk operation using this sql statement: " + sql, exceptionInfo);
    return;
  }
  rdbFinishCommand(cmd);
  if (rdbCheckError(err)){dynAppend(exceptionInfo, "DB ERROR: " + err); emu_error(exceptionInfo); return;};
  
  emu_debugFuncEnd("emu_executeBulk", t0);
}

/** Very simple but yet quite often useful (because of error handling) wrapper function to query just a single field of information from DB. 
    For parameters see emucdb_executeSql(...). */
mixed emucdb_querySingleField(string sql, dyn_string &exceptionInfo, 
                              mapping bindVariables = emucdb_dummyMapping, int datatype = STRING_VAR) {
  dyn_mixed data;
  time t0;
  emu_debugFuncStart("emucdb_querySingleField", t0);
  
  data = emucdb_querySingleRow(sql, exceptionInfo, bindVariables, makeDynInt(datatype));
  if (emu_checkException(exceptionInfo)) { return; }

  if (dynlen(data) != 1) {
    emu_addError("Expected one result column on query, but got: " + dynlen(data[1]));
    emu_addError("Error when executing sql statement: " + sql, exceptionInfo);
    return;
  }
  
  emu_debugFuncEnd("emucdb_querySingleField", t0);
  return data[1];
}

/** Very simple but yet quite often useful (because of error handling) wrapper function to query just a single row of information from DB. 
    If the number of returned rows is not 1 then error message will be issued.
    For parameters see emucdb_executeSql(...). */
dyn_mixed emucdb_querySingleRow(string sql, dyn_string &exceptionInfo, 
                              mapping bindVariables = emucdb_dummyMapping, dyn_int datatypes = makeDynInt()) {
  dyn_dyn_mixed data;
  time t0;
  emu_debugFuncStart("emucdb_querySingleRow", t0);
  
  data = emucdb_executeSql(sql, exceptionInfo, FALSE, bindVariables, datatypes);
  if (emu_checkException(exceptionInfo)) { return; }
  if (dynlen(data) != 1) {
    emu_addError("Expected one result row on query, but got: " + dynlen(data), exceptionInfo);
    emu_addError("Error when executing sql statement: " + sql, exceptionInfo);
    return;
  }
  
  emu_debugFuncEnd("emucdb_querySingleRow", t0);
  return data[1];
}

/** Begins a transaction. */
void emucdb_beginTransaction(dyn_string &exceptionInfo) {
  string err;
  time t0;
  emu_debugFuncStart("emucdb_beginTransaction", t0);
  
  rdbBeginTransaction(emucdb_g_connName);
  if (rdbCheckError(err, emucdb_g_dbConn)){dynAppend(exceptionInfo, "DB TRANSACTION ERROR: " + err); emu_error(exceptionInfo); return;};
      
  emu_debugFuncEnd("emucdb_beginTransaction", t0);
}

/** Commits the current transaction. */
void emucdb_commit(dyn_string &exceptionInfo) {
  string err;
  time t0;
  emu_debugFuncStart("emucdb_commit", t0);
  
  rdbCommitTransaction(emucdb_g_connName);
  if (rdbCheckError(err, emucdb_g_dbConn)){dynAppend(exceptionInfo, "DB TRANSACTION ERROR: " + err); emu_error(exceptionInfo); return;};
      
  emu_debugFuncEnd("emucdb_commit", t0);
}

/** Rolls back the current transaction. */
void emucdb_rollback() {
  time t0;
  emu_debugFuncStart("emucdb_rollback", t0);
  
  rdbRollbackTransaction(emucdb_g_connName);
      
  emu_debugFuncEnd("emucdb_rollback", t0);
}

/** Prepares an sql statement and returns dbCommand object which can be executed multiple times.
  @param sql            sql statement.
  @param exceptionInfo  any exceptions are reported here.
*/
dbCommand _emucdb_prepareStatement(string sql, dyn_string &exceptionInfo) {
  dbCommand cmd;
  string err;

  rdbStartCommand(emucdb_g_connName, sql, cmd);
  if (rdbCheckError(err,cmd)){dynAppend(exceptionInfo, "DB ERROR: " + err); emu_error(exceptionInfo); return;};

  return cmd;
}

/** Executes a given dbCommand object and returns the data (a 2D array [row][column]).
  @param cmd                a dbCommand object to be executed.
  @param exceptionInfo      any exceptions are reported here.
  @param releaseStatement   if this flag is true then all resources regarding the given prepared statement are released when execution is done 
                            and the dbCommand object cannot be used again. (default - true).
  @param transposeResults   If this is false (default) then the returned result dataset is row-wise (i.e. 2D array [row][column]).
                            Otherwise the returned result dataset is transposed - column-wise (i.e. 2D array [column][row]).
  @param bindVariables  bind variables. This is a "mapping" e.g. bindVariables[var_name] = var_value.
  @param datatypes          datatypes of the columns (e.g. makeDynInt(INT_VAR, INT_VAR, STRING_VAR)). 
                            If it's blank then all the values are converted to string (default).
*/
dyn_dyn_mixed _emucdb_executeCommand(dbCommand cmd, dyn_string &exceptionInfo, 
                                     bool releaseStatement = true, 
                                     bool transposeResults = false,
                                     mapping bindVariables = emucdb_dummyMapping,
                                     dyn_int datatypes = makeDynInt()) {
  dyn_dyn_mixed data;
  string err;
  
  rdbBindParams(cmd, bindVariables);
  if (rdbCheckError(err,cmd)){dynAppend(exceptionInfo, "DB ERROR: " + err); emu_error(exceptionInfo); return;};

  rdbExecuteCommand(cmd);
  if (rdbCheckError(err,cmd)){dynAppend(exceptionInfo, "DB ERROR: " + err); emu_error(exceptionInfo); return;};
  
  rdbGetData(cmd, data, transposeResults, 0, datatypes);
  if (rdbCheckError(err,cmd)){dynAppend(exceptionInfo, "DB ERROR: " + err); emu_error(exceptionInfo); return;};
  
  if (releaseStatement) {
    rdbFinishCommand(cmd);
    if (rdbCheckError(err)){dynAppend(exceptionInfo, "DB ERROR: " + err); emu_error(exceptionInfo); return;};
  }
  
  return data;
}

/** Executes a given dbCommand object and transposes the returned dataset (i.e. the resulting 2D array is [column][row]).
  @param cmd                a dbCommand object to be executed.
  @param datatypes      datatypes of the columns (e.g. makeDynInt(INT_VAR, INT_VAR, STRING_VAR)).
  @param exceptionInfo      any exceptions are reported here.
  @param releaseStatement   if this flag is true then all resources regarding the given prepared statement are released when execution is done 
                            and the dbCommand object cannot be used again. (default - true).
  @param bindVariables  bind variables. This is a "mapping" e.g. bindVariables[var_name] = var_value.
*/
dyn_dyn_mixed _emucdb_executeCommandTranspose(dbCommand cmd, dyn_int datatypes, 
                                              dyn_string &exceptionInfo, bool releaseStatement = true, 
                                              mapping bindVariables = emucdb_dummyMapping) {
  dyn_dyn_mixed data;
  string err;
  
  rdbBindParams(cmd, bindVariables);
  if (rdbCheckError(err,cmd)){dynAppend(exceptionInfo, "DB ERROR: " + err); emu_error(exceptionInfo); return;};

  rdbExecuteCommand(cmd);
  if (rdbCheckError(err,cmd)){dynAppend(exceptionInfo, "DB ERROR: " + err); emu_error(exceptionInfo); return;};
  
  rdbGetData(cmd, data, TRUE, 0, datatypes);
  if (rdbCheckError(err,cmd)){dynAppend(exceptionInfo, "DB ERROR: " + err); emu_error(exceptionInfo); return;};
  
  if (releaseStatement) {
    rdbFinishCommand(cmd);
    if (rdbCheckError(err)){dynAppend(exceptionInfo, "DB ERROR: " + err); emu_error(exceptionInfo); return;};
  }
  
  return data;
}
