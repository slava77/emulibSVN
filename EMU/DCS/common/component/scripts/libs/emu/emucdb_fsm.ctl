/**@file

This package contains EMU CDB functions to work with FSM configuration.

@author Evaldas Juska (PH/UCM)
@date   April 2009
*/

/** Updates FSM type definitions and saves FSM tree to conf DB. */
void emucdb_saveFsm(dyn_string &exceptionInfo) {
  time t0;  
  emu_debugFuncStart("emucdb_saveFsm", t0);
  
  emu_info("Saving FSM object and device types to DB");
  emucdb_beginTransaction(exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }

  //delete FSM configuration from DB
  emucdb_deleteFsmConf(exceptionInfo);
  if (emu_checkException(exceptionInfo)) { emucdb_rollback(); return; }
  
  //update type definitions
  _emucdb_updateFsmTypeDefinitions(exceptionInfo);
  if (emu_checkException(exceptionInfo)) { emucdb_rollback(); return; }
  
  //recurse down the tree and get the types of all FSM nodes
  dyn_dyn_string deviceTypes;
  _emucdb_getFsmDeviceTypesRecursively("FSM", deviceTypes, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { emucdb_rollback(); return; }
  
  //update ITEMS table
  _emucdb_updateDeviceFsmTypes(deviceTypes, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { emucdb_rollback(); return; }
  
  emucdb_commit(exceptionInfo);
  if (emu_checkException(exceptionInfo)) { emucdb_rollback(); return; }
  emu_info("[DONE] FSM objects and devices were saved successfully");
  
  emu_debugFuncEnd("emucdb_saveFsm", t0);
}

/** Reads FSM types and updates their definitions in DB (adds missing). */
void _emucdb_updateFsmTypeDefinitions(dyn_string exceptionInfo) {
  string existingTypesSql = "select name from emucdb_fsm_types";
  string insertSql = "insert into emucdb_fsm_types (name, type, is_cu) values (:insName, :insType, :isCu)";
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
      int isCu = 0;
      emu_info("TODO!!!!!! MOVE IS_CU FLAG TO EMUCDB_DEVICES FROM EMUCDB_FSM_TYPES!!!!!!!!!!!");
      dynAppend(insertBindVars, makeDynString(devTypes[i], "dev", isCu));
    }
  }
  for (int i=1; i <= dynlen(objTypes); i++) {
    if (!dynContains(data[1], objTypes[i])) {
      int isCu = 0;
      emu_info("TODO!!!!!! MOVE IS_CU FLAG TO EMUCDB_DEVICES FROM EMUCDB_FSM_TYPES!!!!!!!!!!!");
      dynAppend(insertBindVars, makeDynString(objTypes[i], "obj", isCu));
    }
  }
  
  emucdb_executeBulk(insertSql, insertBindVars, exceptionInfo);
  if (emu_checkException(exceptionInfo)) return;
  emu_info("[DONE] FSM types saved: " + dynlen(insertBindVars));
  
  emu_debugFuncEnd("emucdb_updateFsmTypeDefinitions", t0);
}


/** Saves the FSM device types to the DB.
  @param deviceTypes          2D array [device] -> [logical_alias][fsm_type][is_root]
*/
void _emucdb_updateDeviceFsmTypes(dyn_dyn_string deviceTypes, dyn_string &exceptionInfo) {
  string typesSql = "select id, name, default_ui from emucdb_fsm_types";
  string selectIdSql = "select i.id, emu.id from items i left outer join emucdb_devices emu on (emu.id = i.id) where i.dpname = :dpName";
  string insertSql = "insert into emucdb_devices (id, fsm_type, fsm_ui, is_root) values (:id, :fsmType, :fsmUi, :isRoot)";
  string updateSql = "update emucdb_devices set fsm_type = :fsmType, fsm_ui = :fsmUi, is_root = :isRoot where id = :id";
  dyn_dyn_mixed updateData; // the bind variables passed to the bulk update command
  dyn_dyn_mixed insertData; // the bind variables passed to the bulk insert command
  mapping selectIdMapping;
  time t0;
  emu_debugFuncStart("emucdb_updateDeviceFsmTypes", t0);

  emu_info("Updating FSM object and device types in DB");
  // get a fsm types id "look-up table"
  dyn_dyn_mixed types = emucdb_executeSql(typesSql, exceptionInfo, true, emucdb_dummyMapping, makeDynInt(INT_VAR, STRING_VAR, STRING_VAR));
  if (emu_checkException(exceptionInfo)) { return; }
  
  //fill the "data" arrays with all the necessary info
  for (int i=1; i <= dynlen(deviceTypes); i++) {
    string logName = deviceTypes[i][1];
    string typeName = deviceTypes[i][2];
    int isRoot = deviceTypes[i][3];
    int typeId;
    int deviceId;
    string defaultUi;
    
    if (dpExists(logName)) { // it's not a logical name, it's a device name - we change it to the logical
      logName = dpGetAlias(logName + ".");
    }
    
    //find out type ID
    int index = dynContains(types[2], typeName);
    if (index <= 0) {
      emu_addError("FSM type \"" + typeName + "\" doesn't exist in the confDB, please update the FSM_TYPES table.", exceptionInfo);
      return;
    }
    typeId = types[1][index];
    defaultUi = types[3][index];
    
    //find out device ID and if it's already in emucdb table or not
    selectIdMapping["dpName"] = logName;
    emu_debug("--== _emucdb_updateDeviceFsmTypes ==-- Checking Id of device: " + logName, emu_DEBUG_DETAIL);
    dyn_mixed ids = emucdb_querySingleRow(selectIdSql, exceptionInfo, selectIdMapping, makeDynInt(INT_VAR, INT_VAR));
    if (emu_checkException(exceptionInfo)) { return; }
    deviceId = ids[1];
    if (ids[2] == "") { // device does not exist in emucdb table - insert operation
      int index = dynlen(insertData) + 1;
      insertData[index][1] = deviceId;
      insertData[index][2] = typeId;
      insertData[index][3] = defaultUi;
      insertData[index][4] = isRoot;
    } else {            // device already exists in emucdb table - update operation
      int index = dynlen(updateData) + 1;
      updateData[index][1] = typeId;
      updateData[index][2] = defaultUi;
      updateData[index][3] = isRoot;
      updateData[index][4] = deviceId;
    }
  }
  //update the DB - perform insert
  emu_debug("--== _emucdb_updateDeviceFsmTypes ==-- Executing bulk insert with data: " + insertData, emu_DEBUG_DETAIL);
  emucdb_executeBulk(insertSql, insertData, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  //update the DB - perform update
  emu_debug("--== _emucdb_updateDeviceFsmTypes ==-- Executing bulk update with data: " + updateData, emu_DEBUG_DETAIL);
  emucdb_executeBulk(updateSql, updateData, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  emu_info("[DONE] FSM object and device types inserted: " + dynlen(insertData) + ", updated: " + dynlen(updateData));

  emu_debugFuncEnd("emucdb_updateDeviceFsmTypes", t0);
}

/** Recurse down the FSM tree and get types of all the nodes 
    and return them all in deviceTypes parameter (2D array [device] -> [logical_alias][fsm_type][is_root]). */
void _emucdb_getFsmDeviceTypesRecursively(string parent, dyn_dyn_string &deviceTypes, dyn_string &exceptionInfo, string parentLogicalName = "") {
  dyn_string children;
  time t0;
  emu_debugFuncStart("emucdb_getFsmDeviceTypesRecursively", t0);
  
  fwTree_getChildren(parent, children, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  emu_debug("--== _emucdb_getFsmDeviceTypesRecursively ==-- parent: " + parent, emu_DEBUG_DETAIL);
  emu_debug("--== _emucdb_getFsmDeviceTypesRecursively ==-- parent logical name: " + parentLogicalName, emu_DEBUG_DETAIL);
  emu_debug("--== _emucdb_getFsmDeviceTypesRecursively ==-- children: " + children, emu_DEBUG_DETAIL);
  
  //process current node (parent)
  if (parent != "FSM") {
    //find out the type and put it into deviceTypes
    string device, type;
    fwTree_getNodeDevice(parent, device, type, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
    int isRoot = 0;
    if (strpos(parentLogicalName, fwDevice_HIERARCHY_SEPARATOR) < 0) { // if there's no separator in the logical name - then it's root
      isRoot = 1;
    }
    dynAppend(deviceTypes, makeDynString(parentLogicalName, type, isRoot));
  }
  
  //exit criteria
  if (dynlen(children) == 0) {
    emu_debugFuncEnd("emucdb_getFsmDeviceTypesRecursively", t0);
    return;
  }
  
  //recurse down
  for (int i=1; i <= dynlen(children); i++) {
    string logName = parentLogicalName;
    // if this is a device, so do not prepend any parent stuff to the logical name
    if (dpExists(children[i]) || (strlen(dpAliasToName(children[i])) > 0)) {
      logName = children[i];
    } else { // this is an FSM object - that means we prepend parent's name to make a valid name in the logical tree
      if (strlen(logName) > 0) {
        logName += fwDevice_HIERARCHY_SEPARATOR;
      }
      logName += children[i];
    }
    _emucdb_getFsmDeviceTypesRecursively(children[i], deviceTypes, exceptionInfo, logName);
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
                         "from items i inner join emucdb_devices emud on (emud.id = i.id) " +
                                      "inner join emucdb_fsm_types ft on (emud.fsm_type = ft.id) " +
                         "where emud.is_root = 1";
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
                             "from items i inner join emucdb_devices emud on (emud.id = i.id) " +
                                          "inner join emucdb_fsm_types ft on (emud.fsm_type = ft.id) " +
                                          "join items p on (p.id = i.parent) " +
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

/** Generates FSM tree out of the Logical Tree in confDB. */
void emucdb_generateFSM(dyn_string &exceptionInfo) {
  time t0;
  emu_debugFuncStart("emucdb_generateFSM", t0);
  
  emu_addError("emucdb_generateFSM is not yet implemented", exceptionInfo);
  return;
  // Update EMUCDB_DEVICES table
  _emucdb_setDefaultFsmTypes(exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  emu_debugFuncEnd("emucdb_generateFSM", t0);
}

/** Set default FSM types to logical tree items in conf DB.
    This enables to generate FSM tree out of Logical tree using emucdb_loadFsm(...).
    Note: All nodes that have references to real devices are assigned an FSM type which name is equal to the device type in HW tree (this type must exist, otherwise an error will be generated).
  @param defTopNodeType            FSM type to be assigned to the top node.
  @param defObjType                FSM type to be assigned to all intermediate tree nodes (all nodes except tops and leafs).
*/
void _emucdb_setDefaultFsmTypes(dyn_string &exceptionInfo, string defTopNodeType = "CSC_LV_NODES", 
                                                          string defObjType = "CSC_LV_TREE_NODES") {
  string updateRootsSql = "update items set fsm_type = :fsmTypeId " +
                          "where fsm_type is null and " +
                                "hver in (select hver from hierarchies where htype = 'LOGICAL') and " +
                                "parent in (select id from items where type = 'SYSTEM')";
  string updateTreeNodesSql = "update items set fsm_type = :fsmTypeId " +
                              "where fsm_type is null and " +
                              "hver in (select hver from hierarchies where htype = 'LOGICAL') and " +
                              "id not in (select id from references where valid_to is null) and " +
                              "type <> 'SYSTEM'";
  string selectDevicesSql = "select fsmt.id, i.id " +
                            "from items i left outer join emucdb_fsm_types fsmt on (fsmt.name = i.type) "
                            "where i.fsm_type is null and " +
                            "i.hver in (select hver from hierarchies where htype = 'LOGICAL') and " +
                            "i.id in (select id from references where valid_to is null)";
  string updateIndividualSql = "update items set fsm_type = :fsmTypeId where id = :itemId";
  string selectFsmTypeIdSql = "select id from emucdb_fsm_types where name = :fsmTypeName";
  int defTopTypeId, defObjTypeId;
  time t0;
  emu_debugFuncStart("emucdb_setDefaultFsmTypes", t0);
  emu_info("Applying default FSM types for logical tree nodes in DB");
  
  mapping fsmTypeNameMapping;
  fsmTypeNameMapping["fsmTypeName"] = defTopNodeType;
  defTopTypeId = emucdb_querySingleField(selectFsmTypeIdSql, exceptionInfo, fsmTypeNameMapping, INT_VAR);
  if (dynlen(exceptionInfo)) { dynInsertAt(exceptionInfo, "Problem getting the default FSM type for top nodes: " + defTopNodeType, 1); }
  if (emu_checkException(exceptionInfo)) { return; }
  fsmTypeNameMapping["fsmTypeName"] = defObjType;
  defObjTypeId = emucdb_querySingleField(selectFsmTypeIdSql, exceptionInfo, fsmTypeNameMapping, INT_VAR);
  if (dynlen(exceptionInfo)) { dynInsertAt(exceptionInfo, "Problem getting the default FSM type for object nodes: " + defObjType, 1); }
  if (emu_checkException(exceptionInfo)) { return; }
  
  mapping fsmTypeIdMapping;
  fsmTypeIdMapping["fsmTypeId"] = defTopTypeId;
  emucdb_executeSql(updateRootsSql, exceptionInfo, FALSE, fsmTypeIdMapping);
  if (emu_checkException(exceptionInfo)) { return; }
  fsmTypeIdMapping["fsmTypeId"] = defObjTypeId;
  emucdb_executeSql(updateTreeNodesSql, exceptionInfo, FALSE, fsmTypeIdMapping);
  if (emu_checkException(exceptionInfo)) { return; }
  
  dyn_dyn_mixed devices = emucdb_executeSql(selectDevicesSql, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  //check if there are no null values for fsmTypeId, if there are - report an error
  for (int i=1; i <= dynlen(devices); i++) {
    if (devices[i][2] == "") {
      emu_addError("Some device types haven't got their equivalent in FSM types. e.g. device id in DB: " + devices[i][1], exceptionInfo);
      return;
    }
  }
  emucdb_executeBulk(updateIndividualSql, devices, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  emu_info("[DONE] Default FSM types applied successfully");
  emu_debugFuncEnd("emucdb_setDefaultFsmTypes", t0);
}

/** Deletes FSM tree configuration from DB. Optionally (if deleteTypes flag is set) also FSM type definitions are deleted from DB. */
void emucdb_deleteFsmConf(dyn_string &exceptionInfo, bool deleteTypes = false) {
  string deleteSql = "update emucdb_devices set fsm_type = null, fsm_ui = null";
  string deleteTypesSql = "delete from emucdb_fsm_types";
  time t0;
  emu_debugFuncStart("emucdb_deleteFsmConf", t0);
  emu_info("Deleting FSM configuration from DB");
  
  emucdb_executeSql(deleteSql, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  if (deleteTypes) {
    emucdb_executeSql(deleteTypesSql, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
  }
  
  emu_info("[DONE] FSM configuration deleted successfully");
  emu_debugFuncEnd("emucdb_deleteFsmConf", t0);
}
