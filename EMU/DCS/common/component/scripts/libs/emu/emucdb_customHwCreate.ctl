/**@file

This package contains general purpose utility functions to access DB.

@author Evaldas Juska (PH/UCM)
@date   March 2009
*/

/** Checks if fwConfigurationDB table "ITEMS" is present in the EMU conf db. 
    If it doesn't exits - it returns an exception in the ex. */
void emucdb_checkConfDB(dyn_string &ex) {
  dyn_dyn_mixed data;
  time t0;   
  emu_debugFuncStart("emucdb_checkConfDB", t0);

  data = emucdb_executeSql("select * from user_tables where table_name = 'ITEMS'", ex);
  if (dynlen(data) < 1) {
    emu_addError("fwConfigurationDB table 'ITEMS' does not exits. Check if fwConfigurationDB is installed and the version is correct.", ex);
    return;
  }
  
  emu_debugFuncEnd("emucdb_checkConfDB", t0);
}

/** Creates the hardware which needs custom actions and is not created correctly by fwConfDB. */
void emucdb_createCustomHwMain() {
  dyn_dyn_mixed data;
  string itemsSql = "select n.dpname dp, p.dpname parent_dp, gp.dpname grand_paret_dp, n.name, n.description " +
                    "from items n join items p on (p.id = n.parent) left outer join items gp on (gp.id = p.parent) " +
                    "where n.type = :type and n.dpname like :sysName || '%' order by length(dp) asc";
  mapping bindVariables;
  dyn_string ex;
  time t0;
  emu_debugFuncStart("emucdb_createCustomHwMain", t0);
  
  emu_info("Creating custom devices");
  emucdb_checkConfDB(ex);
  if (emu_checkException(ex)) { return; }
  
  bindVariables["sysName"] = getSystemName();

  //create CANbusses
  bindVariables["type"] = "FwElmbCANbus";
  data = emucdb_executeSql(itemsSql, ex, false, bindVariables);
  if (emu_checkException(ex)) { return; }
  for (int i=1; i <= dynlen(data); i++) {
    emucdb_createElmbCANbus(data[i], ex);
    if (emu_checkException(ex)) { return; }
  }

  //create Elmb nodes
  bindVariables["type"] = "FwElmbNode";
  data = emucdb_executeSql(itemsSql, ex, false, bindVariables);
  if (emu_checkException(ex)) { return; }
  for (int i=1; i <= dynlen(data); i++) {
    emucdb_createElmbNode(data[i], ex);
    if (emu_checkException(ex)) { return; }
  }

  emu_debugFuncEnd("emucdb_createCustomHwMain", t0);
}

/** Creates Elmb CANbus using the fwConfigurationDB data (see parameter data). 
    Note: It is presumed that the card type is Kvaser and bus speed is 125 Kb/s.
  @param data         an array of parameters: dp (not used), parent_dp (not used), grand_parent_dp (not used), name, comment. 
                      Here name must conform this simple convention: <whateverName>_<bus port>
  @param ex           Any exceptions are reported here
*/
void emucdb_createElmbCANbus(dyn_mixed data, dyn_string &ex) {
  string name = data[4];
  string comment = data[5];
  int port;
  time t0;
  emu_debugFuncStart("emucdb_createElmbCANbus", t0);

  //parse the name and register any errors
  port = _emucdb_getLastHexNumber(name, "_", ex);
  if (emu_checkException(ex)) { return; }
    
  //create the device
  fwElmbUser_createCANbus(name, comment, port, "Kvaser", 125000, true, ex);
  if (emu_checkException(ex)) { return; }
  emu_info("Elmb CANbus \"" + name + "\" was created OK");
  
  emu_debugFuncEnd("emucdb_createElmbCANbus", t0);
}

/** Creates Elmb node using the fwConfigurationDB data (see parameter data). 
  @param data         an array of parameters: dp (not used), parent_dp, grand_parent_dp (not used), name, comment. 
                      Here name must conform this simple convention: <whateverName>_<node_id_hex>
  @param ex           Any exceptions are reported here
*/
void emucdb_createElmbNode(dyn_mixed data, dyn_string &ex) {
  string name = data[4];
  string comment = data[5];
  string busName = data[2];
  int id;
  time t0;
  emu_debugFuncStart("emucdb_createElmbNode", t0);

  //parse the name and register any errors
  id = _emucdb_getLastHexNumber(name, "_", ex);
  if (emu_checkException(ex)) { return; }
    
  //create the device
  fwElmbUser_createElmb(name, comment, busName, id, true, ex);
  if (emu_checkException(ex)) { return; }
  emu_info("Elmb node \"" + name + "\" was created OK");
  
  emu_debugFuncEnd("emucdb_createElmbNode", t0);
}

/** Parses a device name, gets a hexadecimal number at the end of it (after the last separator symbol) and returns it as decimal. */
int _emucdb_getLastHexNumber(string deviceName, string separator, dyn_string &ex) {
  dyn_string tmp;
  string hex;
  int ret;
  time t0;
  emu_debugFuncStart("_emucdb_getLastHexNumber", t0);
  
  tmp = strsplit(deviceName, separator);
  if (dynlen(tmp) < 2) {
    emu_addError("Invalid name \"" + deviceName + "\". Convention: <whateverName>" + separator + "<hex_number>", ex);
    return;
  }
  hex = tmp[dynlen(tmp)];
  if (strlen(hex) == 0) {
    emu_addError("Invalid name \"" + deviceName + "\". Convention: <whateverName>" + separator + "<hex_number>", ex);
    return;
  }
  
  while (strlen(hex) < 2) { //fill with zeros in the front
    hex = '0' + hex;
  }
  blob tmpBlob = hex;
  int rc = blobGetValue(tmpBlob, 0, ret, 1, true);
  if (rc == -1) {
    emu_addError("Invalid name \"" + deviceName + "\". Convention: <whateverName>" + separator + "<hex_number>", ex);
    return;
  }
  
  emu_debugFuncEnd("_emucdb_getLastHexNumber", t0);
  return ret;
}
