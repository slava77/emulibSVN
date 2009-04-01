/**@file

This package contains general purpose utility functions to access DB.

@author Evaldas Juska (PH/UCM)
@date   March 2009
*/

const mapping emucdb_dummyMapping;

string g_emucdb_dbConnectionString =                                                                 "Driver=QOCI8;Database=devdb10;User=CMS_MUON_ENDCAP_EVKA;Password=Ventos6385!";
global string g_emucdb_dbConnName = "emucdb_conn";

/** Opens connection to the EMU confDB and initializes the fwConfigurationDB. */
void emucdb_initialize() {
  string err;
  dbConnection dbConn;
  dyn_string exceptionInfo;
  dyn_string connections;
  time t0;

  emu_debugFuncStart("emucdb_initialize", t0);

  fwConfigurationDB_initialize("", exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  DebugTN("Connection to the fwConfigurationDB database opened OK");
  
  rdbGetConnectionNames(connections);
  if (dynContains(connections, g_emucdb_dbConnName) > 0) { return; }
  // reset the connection manager - terminate all connections that are still present
  rdbOption("Reset",0);  
  rdbOpenConnection(g_emucdb_dbConnectionString, dbConn, g_emucdb_dbConnName);
  if (rdbCheckError(err, dbConn)){emu_error(makeDynString("ERROR WHILE CONNECTING TO THE EMU CONFIGURATION DATABASE",err));return;};

  DebugTN("Connection to the EMU configuration DB opened OK");
    
  emu_debugFuncEnd("emucdb_initialize", t0);
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

/** Prepares an sql statement and returns dbCommand object which can be executed multiple times.
  @param sql            sql statement.
  @param exceptionInfo  any exceptions are reported here.
*/
dbCommand _emucdb_prepareStatement(string sql, dyn_string &exceptionInfo) {
  dbCommand cmd;
  string err;

  rdbStartCommand(g_emucdb_dbConnName, sql, cmd);
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
