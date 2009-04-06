/**@file

This package contains general purpose utility functions to access DB.

@author Evaldas Juska (PH/UCM)
@date   March 2009
*/

const mapping emucdb_dummyMapping;
const string emucdb_CONNECTION_FILENAME = "emucdb_DBConnection";

global string emucdb_g_connName = "emucdb_DB_connection";

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
    dbConnection dbConn;
    string err;
    
    bool ok=fileToString(connFile, connString);
    if (!ok) {
      emu_error("EMU CDB connection file cannot be open");
      return;
    }
    emu_info("EMU CDB connection file opened successfully, connecting to DB...");
    rdbOpenConnection(connString, dbConn, emucdb_g_connName);
    if (rdbCheckError(err,dbConn)){ emu_error("Error while connecting to EMU CDB database: " + err); return;};
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
  dyn_dyn_mixed data = emucdb_executeSql("select value from emucdb_parameters where name = 'emucdb_version'", exceptionInfo);
  if (dynlen(exceptionInfo) || dynlen(data) == 0) { //if there's an error - try to create it.
    emu_info("An error occured while retrieving EMU CDB version from DB. Trying to create schema...");
    emucdb_createDBSchema(exceptionInfo, false);
    if (emu_checkException(exceptionInfo)) { return; }
  }

  emu_debugFuncEnd("emucdb_checkSchema", t0);
}

/** Creates EMU CDB DB schema. Stops on error. */
void emucdb_createDBSchema(dyn_string &exceptionInfo,bool dropExistingSchema=FALSE) {
  if (dropExistingSchema) {
    emu_info("Dropping existing schema");
    emucdb_dropDBSchema(exceptionInfo);
  }
  
  _fwConfigurationDB_executeSqlFromFile(emucdb_g_connName,
                                        "emucdb_createSchema.sql",
                                        "CREATE DB SCHEMA", exceptionInfo,TRUE);
  if (emu_checkException(exceptionInfo)) { return; }

  emu_info("Database schema succesfully created");
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
  if (emu_checkException(exceptionInfo)) { return; }
  
  emu_debugFuncEnd("emucdb_executeSql", t0);
  return data;
}

/** Executes a bulk command. Sql must contain at least one bound variable and the values of those should be given in data param.*/
void emucdb_executeBulk(string sql, dyn_dyn_mixed data, dyn_string exceptionInfo){
  dbCommand cmd;
  string err;
  time t0;
  emu_debugFuncStart("emu_executeBulk", t0);
  
  cmd = _emucdb_prepareStatement(sql, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  rdbBindAndExecute(cmd, data);
  if (rdbCheckError(err)){dynAppend(exceptionInfo, "DB ERROR: " + err); emu_error(exceptionInfo); return;};
  rdbFinishCommand(cmd);
  if (rdbCheckError(err)){dynAppend(exceptionInfo, "DB ERROR: " + err); emu_error(exceptionInfo); return;};
  
  emu_debugFuncEnd("emu_executeBulk", t0);
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
