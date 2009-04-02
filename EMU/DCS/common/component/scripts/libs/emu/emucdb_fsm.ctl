/**@file

This package contains EMU CDB functions to work with FSM configuration.

@author Evaldas Juska (PH/UCM)
@date   April 2009
*/

/** Reads FSM types and updates their definitions in DB (adds missing). */
void emucdb_updateFsmTypes(dyn_string &exceptionInfo) {
  string existingTypesSql = "select name from emucdb_fsm_types";
  string insertSql = "insert into emucdb_fsm_types (name, type) values (:insName, :insType)";
  dyn_string devTypes, objTypes;
  time t0;
  
  emu_debugFuncStart("emucdb_updateFsmTypes", t0);
  
  fwFsm_initialize();
  devTypes = fwFsm_getDeviceTypes();
  objTypes = fwFsm_getObjectTypes();
  
  emu_info("FSM device types:");
  emu_info(devTypes);
  emu_info("FSM object types:");
  emu_info(objTypes);
  
  emu_info("Saving FSM object and device types to DB");
  //select existing types
  dyn_dyn_mixed data = emucdb_executeSql(existingTypesSql, exceptionInfo, TRUE);
  if (emu_checkException(exceptionInfo)) return;
  if (dynlen(data) < 1) { // in case the table is completely empty
    data[1] = makeDynString();
  }
  
  //filter out those dev and obj types which do not yet exist in the DB
  dyn_dyn_mixed insertBindVars;
  for (int i=1; i <= dynlen(devTypes); i++) {
    if (!dynContains(data[1], devTypes[i])) {
      dynAppend(insertBindVars, makeDynString(devTypes[i], "dev"));
    }
  }
  for (int i=1; i <= dynlen(objTypes); i++) {
    if (!dynContains(data[1], objTypes[i])) {
      dynAppend(insertBindVars, makeDynString(objTypes[i], "obj"));
    }
  }
  
  emu_executeBulk(insertSql, insertBindVars, exceptionInfo);
  if (emu_checkException(exceptionInfo)) return;

  emu_info("[DONE] FSM objects and device saved: " + dynlen(insertBindVars));
  
  emu_debugFuncEnd("emucdb_updateFsmTypes", t0);
}
