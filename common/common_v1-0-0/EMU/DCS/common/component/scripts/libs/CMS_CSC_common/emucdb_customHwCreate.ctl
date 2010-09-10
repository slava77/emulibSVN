/**@file

This package contains general purpose utility functions to access DB.

@author Evaldas Juska (PH/UCM)
@date   March 2009
*/

/** Creates the hardware which needs custom actions and is not created correctly by fwConfDB. */
void emucdb_createCustomHwMain(dyn_string &ex) {
  dyn_dyn_mixed data;
  string itemsSql = "select n.dpname dp, p.dpname parent_dp, n.name, n.description " +
                    "from items n join items p on (p.id = n.parent) " +
                    "where n.type = :type and n.dpname like :sysName || '%' order by length(dp) asc";
  mapping bindVariables;
  time t0;
  emu_debugFuncStart("emucdb_createCustomHwMain", t0);
  
  emu_info("Creating custom devices");
  
  bindVariables["sysName"] = getSystemName();

  //create CANbusses
  bindVariables["type"] = "FwElmbCANbus";
  data = emucdb_executeSql(itemsSql, ex, false, bindVariables);
  if (emu_checkException(ex)) { return; }
  for (int i=1; i <= dynlen(data); i++) {
    if (dpExists(data[i][1])) { continue; } // if this device already exists - skip it
    emucdb_createElmbCANbus(data[i], ex);
    if (emu_checkException(ex)) { return; }
  }

  //create Elmb nodes
  bindVariables["type"] = "FwElmbNode";
  data = emucdb_executeSql(itemsSql, ex, false, bindVariables);
  if (emu_checkException(ex)) { return; }
  for (int i=1; i <= dynlen(data); i++) {
    if (dpExists(data[i][1])) { continue; } // if this device already exists - skip it
    emucdb_createElmbNode(data[i], ex);
    if (emu_checkException(ex)) { return; }
  }

  //create Elmb analog input
  bindVariables["type"] = "FwElmbAi";
  data = emucdb_executeSql(itemsSql, ex, false, bindVariables);
  if (emu_checkException(ex)) { return; }
  for (int i=1; i <= dynlen(data); i++) {
    if (dpExists(data[i][1])) { continue; } // if this device already exists - skip it
    emucdb_createElmbAi(data[i], ex);
    if (emu_checkException(ex)) { return; }
  }
  
  //create Elmb analog input
  bindVariables["type"] = "FwElmbDo";
  data = emucdb_executeSql(itemsSql, ex, false, bindVariables);
  if (emu_checkException(ex)) { return; }
  for (int i=1; i <= dynlen(data); i++) {
    if (dpExists(data[i][1])) { continue; } // if this device already exists - skip it
    emucdb_createElmbDo(data[i], ex);
    if (emu_checkException(ex)) { return; }
  }

  emu_debugFuncEnd("emucdb_createCustomHwMain", t0);
}

/** Creates Elmb CANbus using the fwConfigurationDB data (see parameter data). 
    Note: It is presumed that the card type is Kvaser and bus speed is 125 Kb/s.
  @param data         an array of parameters: dp (not used), parent_dp (not used), name, comment. 
                      Here name must conform this simple convention: <whateverName>_<bus port>
  @param ex           Any exceptions are reported here
*/
void emucdb_createElmbCANbus(dyn_mixed data, dyn_string &ex) {
  string name = data[3];
  string comment = data[4];
  int port;
  time t0;
  emu_debugFuncStart("emucdb_createElmbCANbus", t0);

  //parse the name and register any errors
  port = _emucdb_getLastNumber(name, "_", false, ex);
  if (emu_checkException(ex)) { return; }
    
  //create the device
  fwElmbUser_createCANbus(name, comment, port, "Kvaser", 125000, true, ex);
  if (emu_checkException(ex)) { return; }
  emu_info("Elmb CANbus \"" + name + "\" was created OK");
  
  emu_debugFuncEnd("emucdb_createElmbCANbus", t0);
}

/** Creates Elmb node using the fwConfigurationDB data (see parameter data). 
  @param data         an array of parameters: dp (not used), parent_dp, name, comment. 
                      Here name must conform this simple convention: <whateverName>_<node_id_hex>
  @param ex           Any exceptions are reported here
*/
void emucdb_createElmbNode(dyn_mixed data, dyn_string &ex) {
  string name = data[3];
  string comment = data[4];
  string busName = data[2];
  int id;
  time t0;
  emu_debugFuncStart("emucdb_createElmbNode", t0);

  //parse the name and register any errors
  id = _emucdb_getLastNumber(name, "_", true, ex);
  if (emu_checkException(ex)) { return; }
    
  //create the device
  fwElmbUser_createElmb(name, comment, busName, id, true, ex);
  if (emu_checkException(ex)) { return; }
  emu_info("Elmb node \"" + name + "\" was created OK");
  
  emu_debugFuncEnd("emucdb_createElmbNode", t0);
}

/** Creates Elmb analog input using the fwConfigurationDB data (see parameter data).
  @param data         an array of parameters: dp (not used), parent_dp, name, comment. 
                      Here name must conform this simple convention: <whateverName>_<channel_num>
  @param ex           Any exceptions are reported here
*/
void emucdb_createElmbAi(dyn_mixed data, dyn_string &ex) {
  string name = data[3];
  string comment = data[4];
  string parentDp = data[2];
  string busName, elmbName;
  int chNum;
  time t0;
  emu_debugFuncStart("emucdb_createElmbAi", t0);

  //parse the name and register any errors
  chNum = _emucdb_getLastNumber(name, "_", false, ex);
  if (emu_checkException(ex)) { return; }
  elmbName = _emucdb_dropLastDevice(parentDp);
  busName = _emucdb_dropLastDevice(elmbName);
  
  //create the device
  fwElmbUser_createSensor(name, busName, elmbName, comment, chNum, "Raw ADC Value", makeDynFloat(), true, ex);
  if (emu_checkException(ex)) { return; }
  emu_info("Elmb AI \"" + name + "\" was created OK");
  
  emu_debugFuncEnd("emucdb_createElmbAi", t0);
}

/** Creates Elmb digital output using the fwConfigurationDB data (see parameter data).
  @param data         an array of parameters: dp (not used), parent_dp, name, comment. 
                      Here name must conform this simple convention: <whateverName>_<port>_<bit_num>
  @param ex           Any exceptions are reported here
*/
void emucdb_createElmbDo(dyn_mixed data, dyn_string &ex) {
  string name = data[3];
  string comment = data[4];
  string parentDp = data[2];
  string busName, elmbName, port;
  int bit;
  time t0;
  emu_debugFuncStart("emucdb_createElmbDo", t0);

  //parse the name and register any errors
  bit = _emucdb_getLastNumber(name, "_", false, ex);
  if (emu_checkException(ex)) { return; }
  elmbName = _emucdb_dropLastDevice(parentDp);
  busName = _emucdb_dropLastDevice(elmbName);
  dyn_string split = strsplit(name, "_");
  if (dynlen(split) < 3) {
    emu_addError("Invalid name \"" + deviceName + "\". Convention: <whateverName>_<port>_<bit_number>", ex);
    return;
  }
  port = split[2];
    
  //create the device
  fwElmbUser_createDigital(busName, elmbName, comment, port, bit, false, true, ex);
  if (emu_checkException(ex)) { return; }
  emu_info("Elmb DO \"" + name + "\" was created OK");
  
  emu_debugFuncEnd("emucdb_createElmbDo", t0);
}

/** Parses a device name, gets a hexadecimal number at the end of it (after the last separator symbol) and returns it as decimal. */
int _emucdb_getLastNumber(string deviceName, string separator, bool isHex, dyn_string &ex) {
  dyn_string tmp;
  string number;
  int ret;
  time t0;
  emu_debugFuncStart("_emucdb_getLastNumber", t0);
  
  tmp = strsplit(deviceName, separator);
  if (dynlen(tmp) < 2) {
    emu_addError("Invalid name \"" + deviceName + "\". Convention: <whateverName>" + separator + "<number> (the number can be either hexadecimal or decimal)", ex);
    return;
  }
  number = tmp[dynlen(tmp)];
  if (strlen(number) == 0) {
    emu_addError("Invalid name \"" + deviceName + "\". Convention: <whateverName>" + separator + "<number> (the number can be either hexadecimal or decimal)", ex);
    return;
  }
  
  //it's a hexadecimal number
  if (isHex) {
    while (strlen(number) < 2) { //fill with zeros in the front
      number = '0' + number;
    }
    blob tmpBlob = number;
    int rc = blobGetValue(tmpBlob, 0, ret, 1, true);
    if (rc == -1) {
      emu_addError("Invalid name \"" + deviceName + "\". Convention: <whateverName>" + separator + "<number> (the number can be either hexadecimal or decimal)", ex);
      return;
    }
  } else { // it's a decimal number
    int rc = sscanf(number, "%d", ret);
    if (rc == EOF) {
      emu_addError("Invalid name \"" + deviceName + "\". Convention: <whateverName>" + separator + "<number> (the number can be either hexadecimal or decimal)", ex);
    }
  }
  emu_debugFuncEnd("_emucdb_getLastNumber", t0);
  return ret;
}

/** drops the last node in a device name (e.g. ELMB/LVCB_1/ELMB_3F   ---> ELMB/LVCB_1) */
string _emucdb_dropLastDevice(string deviceName) {
  dyn_string split;
  string ret = "";
  int length;
  
  split = strsplit(deviceName, fwDevice_HIERARCHY_SEPARATOR);
  length = dynlen(split);
  for (int i=1; i < length; i++) {
    ret += split[i];
    if (i < length - 1) {
      ret += fwDevice_HIERARCHY_SEPARATOR;
    }
  }
  return ret;
}
