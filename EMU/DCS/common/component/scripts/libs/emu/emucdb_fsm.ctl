/**@file

This package contains EMU CDB functions to work with FSM configuration.

@author Evaldas Juska (PH/UCM)
@date   April 2009
*/

/** Updates FSM type definitions and device FSM types in conf DB. */
void emucdb_saveFsm(dyn_string &exceptionInfo) {
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
  
  //exit criteria
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


/** Loads the FSM tree from conf DB.
  @param useHardwareTreeDevices            If this is true then all DUs (devices) will be created using HW tree,
                                           otherwise Logical tree will be used (default).
*/
void emucdb_loadFsm(dyn_string &exceptionInfo, bool useHardwareTreeDevices = false) {
  string selectRootsSql = "select i.id, 'FSM' parent_name, i.name, i.dpname, ft.name fsm_type_name, ft.type, ft.is_cu " +
                         "from items i inner join emucdb_fsm_types ft on (i.fsm_type = ft.id) join items p on (p.id = i.parent) " +
                         "where i.parent not in (select id from items where fsm_type is not null)";
  time t0;
  emu_debugFuncStart("emucdb_loadFsm", t0);
  emu_info("Loading FSM tree from DB");
  
  dyn_dyn_mixed roots = emucdb_executeSql(selectRootsSql, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  for (int i=1; i <= dynlen(roots); i++) {
    _emucdb_createFsmRecursively(roots[i], exceptionInfo, useHardwareTreeDevices);
    if (emu_checkException(exceptionInfo)) { return; }
  }
  
  emu_info("[DONE] FSM tree loaded successfully");
  emu_info("Generating and refreshing FSM");
  fwFsmTree_generateAll();
  fwFsmTree_refreshTree();
  emu_info("DONE!");
  emu_debugFuncEnd("emucdb_loadFsm", t0);
}

/** Creates the node provided in nodeData parameter and then recurses down the tree.
  @param nodeData            information about the node: [1] nodeId, [2] parent name, [3] node name, [4] node dp name,
                                                         [5] fsm type name, [6] fsm type type (dev - device or obj - object),
                                                         [7] is CU (1 or 0)
*/
void _emucdb_createFsmRecursively(dyn_mixed nodeData, dyn_string &exceptionInfo, bool useHardwareTree = false) {
  string selectChildrenSql = "select i.id, p.name parent_name, i.name, i.dpname, ft.name fsm_type_name, ft.type, ft.is_cu " +
                             "from items i inner join emucdb_fsm_types ft on (i.fsm_type = ft.id) join items p on (p.id = i.parent) " +
                             "where i.parent = :parentId";
  string getFromHwSql = "select i.dpname " +
                        "from references r join items i on (i.id = r.ref_id) " +
                        "where r.valid_to is null and r.id = :nodeLogId";
  int id             = nodeData[1];
  string parentName  = nodeData[2];
  string name        = nodeData[3];
  string dpName      = nodeData[4];
  string fsmTypeName = nodeData[5];
  string fsmTypeType = nodeData[6];
  int isCu           = nodeData[7];
  time t0;
  emu_debugFuncStart("_emucdb_createFsmRecursively", t0);
  
  //create the parent node
  if (fsmTypeType == "dev") { // for devices replace the name with dpName
    if (useHardwareTree) { // get the device name from HW tree
      mapping bindVariables;
      bindVariables["nodeLogId"] = id;
      dyn_dyn_mixed hwDp = emucdb_executeSql(getFromHwSql, exceptionInfo, FALSE, bindVariables);
      if (emu_checkException(exceptionInfo)) { return; }
      if (dynlen(hwDp) < 1) {
        emu_addError("Reference to HW tree for \"" + dpName + "\" was not found", exceptionInfo);
        return;
      } else if (dynlen(hwDp) > 1) {
        emu_addError("More than one reference to HW tree for \"" + dpName + "\" were found", exceptionInfo);
        return;
      }
      name = hwDp[1][1];
    } else {               // replace the name with full DP name from Logical tree
      name = dpName;
    }
  }
  fwFsmTree_addNode(parentName, name, fsmTypeName, isCu);
  
  //get the children
  mapping bindVariables;
  bindVariables["parentId"] = id;
  dyn_dyn_mixed children = emucdb_executeSql(selectChildrenSql, exceptionInfo, FALSE, bindVariables);
  if (emu_checkException(exceptionInfo)) { return; }
  
  //create children
  for (int i=1; i <= dynlen(children); i++) {
    _emucdb_createFsmRecursively(children[i], exceptionInfo, useHardwareTree);
    if (emu_checkException(exceptionInfo)) { return; }
  }
  
  emu_debugFuncEnd("_emucdb_createFsmRecursively", t0);
}
