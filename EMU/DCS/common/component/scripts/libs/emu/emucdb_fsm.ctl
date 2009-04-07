/**@file

This package contains EMU CDB functions to work with FSM configuration.

@author Evaldas Juska (PH/UCM)
@date   April 2009
*/

/** Updates FSM type definitions and device FSM types in conf DB. */
void emucdb_updateFsmTypes(dyn_string &exceptionInfo) {
  time t0;  
  emu_debugFuncStart("emucdb_updateFsmTypes", t0);
  
  emu_info("Saving FSM object and device types to DB");
  //update type definitions
  _emucdb_updateFsmTypeDefinitions(exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  //recurse down the tree and get the types of all FSM nodes
  dyn_dyn_string deviceTypes;
  _emucdb_getFsmDeviceTypesRecursively("FSM", deviceTypes, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  //update ITEMS table
  _emucdb_updateDeviceFsmTypes(deviceTypes, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  emu_info("[DONE] FSM objects and devices were saved successfully");
  
  emu_debugFuncEnd("emucdb_updateFsmTypes", t0);
}

/** Reads FSM types and updates their definitions in DB (adds missing). */
void _emucdb_updateFsmTypeDefinitions(dyn_string exceptionInfo) {
  string existingTypesSql = "select name from emucdb_fsm_types";
  string insertSql = "insert into emucdb_fsm_types (name, type) values (:insName, :insType)";
  dyn_string devTypes, objTypes;
  time t0;  
  emu_debugFuncStart("emucdb_updateFsmTypeDefinitions", t0);
  
  fwFsm_initialize();
  devTypes = fwFsm_getDeviceTypes();
  objTypes = fwFsm_getObjectTypes();
  
  emu_info("FSM device types:");
  emu_info(devTypes);
  emu_info("FSM object types:");
  emu_info(objTypes);
  
  emu_info("Updating FSM type definitions in DB");
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
  
  emucdb_executeBulk(insertSql, insertBindVars, exceptionInfo);
  if (emu_checkException(exceptionInfo)) return;
  emu_info("[DONE] FSM types saved: " + dynlen(insertBindVars));
  
  emu_debugFuncEnd("emucdb_updateFsmTypeDefinitions", t0);
}


/** Saves the FSM device types to the DB.
  @param deviceTypes          2D array [device] -> [logical_alias][fsm_type]
*/
void _emucdb_updateDeviceFsmTypes(dyn_dyn_string deviceTypes, dyn_string &exceptionInfo) {
  string typesSql = "SELECT ID, NAME FROM EMUCDB_FSM_TYPES";
  string updateSql = "UPDATE ITEMS SET FSM_TYPE = :id WHERE DPNAME = :dpName";
  dyn_dyn_mixed data; // the bind variables passed to the update command
  time t0;
  emu_debugFuncStart("emucdb_updateDeviceFsmTypes", t0);

  emu_info("Updating FSM object and device types in DB");
  // get a fsm types id "look-up table"
  dyn_dyn_mixed types = emucdb_executeSql(typesSql, exceptionInfo, true, emucdb_dummyMapping, makeDynInt(INT_VAR, STRING_VAR));
  if (emu_checkException(exceptionInfo)) { return; }
  
  //fill the "data" array with all the necessary info
  for (int i=1; i <= dynlen(deviceTypes); i++) {
    string logName = deviceTypes[i][1];
    string typeName = deviceTypes[i][2];
    int typeId;
    
    if (dpExists(logName)) { // it's not a logical name, it's a device name - we change it to the logical
      logName = dpGetAlias(logName + ".");
    }
    data[i][2] = logName;
    
    //find out type ID
    int index = dynContains(types[2], typeName);
    if (index <= 0) {
      emu_addError("FSM type \"" + typeName + "\" doesn't exist in the confDB, please update the FSM_TYPES table.", exceptionInfo);
      return;
    }
    data[i][1] = types[1][index];
  }
  //update the DB
  emucdb_executeBulk(updateSql, data, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  emu_info("[DONE] FSM object and device types updated: " + dynlen(data));

  emu_debugFuncEnd("emucdb_updateDeviceFsmTypes", t0);
}

/** Recurse down the FSM tree and get types of all the nodes 
    and return them all in deviceTypes parameter (2D array [device] -> [logical_alias][fsm_type]). */
void _emucdb_getFsmDeviceTypesRecursively(string parent, dyn_dyn_string &deviceTypes, dyn_string &exceptionInfo, string parentLogicalName = "") {
  dyn_string children;
  time t0;
  emu_debugFuncStart("emucdb_getFsmDeviceTypesRecursively", t0);
  
  fwTree_getChildren(parent, children, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  //process current node (parent)
  if (parent != "FSM") {
    //find out the type and put it into deviceTypes
    string device, type;
    fwTree_getNodeDevice(parent, device, type, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
    dynAppend(deviceTypes, makeDynString(parentLogicalName, type));
  }
  
  //exit condition
  if (dynlen(children) == 0) {
    emu_debugFuncEnd("emucdb_getFsmDeviceTypesRecursively", t0);
    return;
  }
  
  //recurse down
  for (int i=1; i <= dynlen(children); i++) {
    // if this is a device, so do not prepend any parent stuff to the logical name
    if (dpExists(children[i]) || (strlen(dpAliasToName(children[i])) > 0)) {
      parentLogicalName = children[i];
    } else { // this is an FSM object - that means we prepend parent's name to make a valid name in the logical tree
      if (strlen(parentLogicalName) > 0) {
        parentLogicalName += fwDevice_HIERARCHY_SEPARATOR;
      }
      parentLogicalName += children[i];
    }
    _emucdb_getFsmDeviceTypesRecursively(children[i], deviceTypes, exceptionInfo, parentLogicalName);
    if (emu_checkException(exceptionInfo)) { return; }
  }
  
  emu_debugFuncEnd("emucdb_getFsmDeviceTypesRecursively", t0);
}
