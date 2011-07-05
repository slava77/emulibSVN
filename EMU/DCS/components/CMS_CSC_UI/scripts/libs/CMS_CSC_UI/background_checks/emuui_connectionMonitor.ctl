/**@file

This package contains functions which monitor various connection status in background and show or hide appropriate icons in the UI.

All functions have prefix emuuibc_ where bc stands for Background Checks.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

// Common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_math.ctl"

// UI libs
#uses "CMS_CSC_UI/emuui_utils.ctl"

const string EMUUIBC_NO_COMMUNICATION_ICON_NAME = "imgNoCommunication";
const int EMUUIBC_MAX_XMAS_ERROR_COUNT = 9;

global mapping emuuibc_g_chamberRefNameToFsmNode; // map of chamber reference name to its' fsm node
global mapping emuuibc_g_disconnectedDevices; // map of disconnected devices dp name to its' value
global mapping emuuibc_g_lastXmasCommand; // map of detector side -> last XMas command (side is M or P)

/** Register a chamber for background checks. */
void emuuibc_registerChamberViewFsmNode(string refName, string fsmNodeName, mapping deviceParams) {
  emuuibc_g_chamberRefNameToFsmNode[refName] = fsmNodeName;
  emuuibc_checkForNoCommunication(refName, fsmNodeName);
}

/** Chamber view has bee reset - clear everything. Not mandatory but still nicer if you call it. */
void emuuibc_chamberViewReset() {
  // hide all icons
  for (int i=1; i <= mappinglen(emuuibc_g_chamberRefNameToFsmNode); i++) {
    setValue(refName + "." + EMUUIBC_NO_COMMUNICATION_ICON_NAME, "visible", false);
  }
  // clear out all subscribers
  mappingClear(emuuibc_g_chamberRefNameToFsmNode);
}

/** This function is called when a DP of disconnected devices list is updated. */
void emuuibc_disconnectedDevicesUpdatedCB(string dp, string disconnectedDevices) {
  // if something has changed here
  if (!mappingHasKey(emuuibc_g_disconnectedDevices, dp) || 
      (emuuibc_g_disconnectedDevices[dp] != disconnectedDevices)) {
    emuuibc_g_disconnectedDevices[dp] = disconnectedDevices;
    startThread("emuuibc_checkAllForNoCommunication");
  }
}

/** This function is called when a DP of disconnected devices list is updated. */
void emuuibc_xmasControlUpdatedCB(string dp, string command) {
  dyn_string ex;
  string side = emuui_getMappingValue("sysNameToSide", emuui_getSystemName(dp), ex);
  if (emu_checkException(ex)) { return; }
  emuuibc_g_lastXmasCommand[side] = command;
  emuuibc_checkXmasConnectionState(side);
}

/** Checks if a given chamber data is up to date and hides or shows a warning icon. 
  @param chamberRefName reference name of the chamber reference panel.
  @param chamberFsmNode fsm node for LV, TEMP or HV for that chamber. 
*/
bool emuuibc_checkForNoCommunication(string chamberRefName, string chamberFsmNode) {
  bool isDisconnected = false;
  for (int i=1; i <= mappinglen(emuuibc_g_disconnectedDevices); i++) {
    string disconnectedDevices = mappingGetValue(emuuibc_g_disconnectedDevices, i);
    if (strpos(disconnectedDevices, chamberFsmNode) >= 0) {
      isDisconnected = true;
      break;
    }
  }
  setValue(chamberRefName + "." + EMUUIBC_NO_COMMUNICATION_ICON_NAME, "visible", isDisconnected);
  
  return isDisconnected;
}

/** Calls emuuibc_checkForNoCommunication(...) for all registered chambers. */
void emuuibc_checkAllForNoCommunication() {
  mapping currentSnapshot = emuuibc_g_disconnectedDevices;
  delay(0, 2000);
  //check if the current version is consistent with the global one
  //if not - that means it has been updated once more, so this thread can exit
  //and let it be handled by the next thread
  for (int i=1; i <= mappinglen(currentSnapshot); i++) {
    string key = mappingGetKey(currentSnapshot, i);
    if (currentSnapshot[key] != emuuibc_g_disconnectedDevices[key]) {
      return;
    }
  }
  
  for (int i=1; i <= mappinglen(emuuibc_g_chamberRefNameToFsmNode); i++) {
    string refName = mappingGetKey(emuuibc_g_chamberRefNameToFsmNode, i);
    string fsmNode = emuuibc_g_chamberRefNameToFsmNode[refName];
    bool noComm = emuuibc_checkForNoCommunication(refName, fsmNode);
  }
  
  emuuibc_checkXmasConnectionState("P");
  emuuibc_checkXmasConnectionState("M");

  // check connection status of HV, LV for HV, Gas and Cooling  
  int hvErrorCount = 0,
      lvForHvErrorCount = 0,
      gasErrorCount = 0,
      coolingErrorCount = 0;
  for (int i=1; i <= mappinglen(emuuibc_g_disconnectedDevices); i++) {
    string disconnectedDpsStr = mappingGetValue(emuuibc_g_disconnectedDevices, i);
    hvErrorCount += strreplace(disconnectedDpsStr, "HighVoltage/", "HighVoltage/");
    lvForHvErrorCount += strreplace(disconnectedDpsStr, "WienerSnmp/", "WienerSnmp/");
    gasErrorCount += strreplace(disconnectedDpsStr, "GasCMS/", "GasCMS/");
    coolingErrorCount += strreplace(disconnectedDpsStr, "CoolingCMS/", "CoolingCMS/");
  }

  string hvState = "OK",
         lvForHvState = "OK",
         gasState = "OK",
         coolingState = "OK";
  
  if (hvErrorCount > 0) { hvState = "ERROR"; }
  if (lvForHvErrorCount > 0) { lvForHvState = "ERROR"; }
  if (gasErrorCount > 0) { gasState = "ERROR"; }
  if (coolingErrorCount > 0) { coolingState = "ERROR"; }
  
  // get the DP names for HV, LV for HV, gas and cooling status and write the status there
  dyn_string ex;
  mapping dpParams;
  dpParams["sessionId"] = emuui_getSessionId();
  string hvStateDp = emuui_getDpName("connection_hv_general", dpParams, ex);
  if (emu_checkException(ex)) { return; }
  string lvForHvStateDp = emuui_getDpName("connection_lvForHv_general", dpParams, ex);
  if (emu_checkException(ex)) { return; }
  string gasStateDp = emuui_getDpName("connection_gas_general", dpParams, ex);
  if (emu_checkException(ex)) { return; }
  string coolingStateDp = emuui_getDpName("connection_cooling_general", dpParams, ex);
  if (emu_checkException(ex)) { return; }
  
  dpSetWait(hvStateDp, hvState);
  dpSetWait(lvForHvStateDp, lvForHvState);
  dpSetWait(gasStateDp, gasState);
  dpSetWait(coolingStateDp, coolingState);
}

/** Checks the Xmas status of one side.
  @param side has to be 'P' for plus or 'M' for minus. */
void emuuibc_checkXmasConnectionState(string side) {
  // get the state DP to which we have to write
  mapping params;
  params["sessionId"] = emuui_getSessionId();
  params["side"] = side;
  dyn_string ex;
  string stateDp = emuui_getDpName("connection_xmas_general", params, ex);
  if (emu_checkException(ex)) { return; }
  
  // assume the state is OK
  string state = "OK";
  // if last command for this side contained word "STOP" - then state is STOPPED - in that case write and return
  if (mappingHasKey(emuuibc_g_lastXmasCommand, side)) {
    if (strpos(emuuibc_g_lastXmasCommand[side], "STOP") >= 0) {
      state = "STOPPED";
      dpSetWait(stateDp, state);
      return;
    }
  }

  // if it's not stopped, then we check for errors  
  int errorCount;
  for (int i=1; i <= mappinglen(emuuibc_g_disconnectedDevices); i++) {
    string dpName = mappingGetKey(emuuibc_g_disconnectedDevices, i);
    string dpSide = emuui_getMappingValue("sysNameToSide", emuui_getSystemName(dpName), ex);
    if (emu_checkException(ex)) { return; }
    if (dpSide != side) { continue; }
    
    string disconnectedDpsStr = emuuibc_g_disconnectedDevices[dpName];
    errorCount += strreplace(disconnectedDpsStr, "LowVoltage/CSC", "LowVoltage/CSC");
    errorCount += strreplace(disconnectedDpsStr, "Temperature/CSC", "Temperature/CSC");
  }
  if (errorCount > EMUUIBC_MAX_XMAS_ERROR_COUNT) {
    state = "ERROR";
  }
  
  // write the state
  dpSetWait(stateDp, state);
}

/**
  * Callback that updates DCS computers connection status in the session DP
  */
void emuuibc_checkComputersConnectionStatusCB(string dp, dyn_string systemNums) {
  dyn_string ex;
  mapping requiredSystems = emuui_getMapping("dcsProjectSystemNames", ex);
  if (emu_checkException(ex)) { return; }
  
  int missingCount = 0;
  for (int i=1; i <= mappinglen(requiredSystems); i++) {
    string reqSysNum = mappingGetKey(requiredSystems, i);
    if (!dynContains(systemNums, reqSysNum) && (reqSysNum != getSystemId())) {
      missingCount++;
    }
  }
  
  string state = "ALL OK";
  if (missingCount > 0) {
    state = "ERROR";
//    state = "ERROR (" + 
//            (mappinglen(requiredSystems) - missingCount) + "/" 
//            + mappinglen(requiredSystems) + 
//            ")";
  }
  
  mapping params;
  params["sessionId"] = emuui_getSessionId();
  string dpName = emuui_getDpName("connection_dcsPCs_general", params, ex);
  if (emu_checkException(ex)) { return; }
  dpSet(dpName, state);
}

/**
  * Callback that updates expert system connection status in the session DP
  */
void emuuibc_checkExsysConnectionStatusCB(string dp, bool isAlive) {
  dyn_string ex;
  string state = isAlive ? "OK" : "ERROR";
  mapping params;
  params["sessionId"] = emuui_getSessionId();
  string dpName = emuui_getDpName("connection_exsys_general", params, ex);
  if (emu_checkException(ex)) { return; }
  dpSet(dpName, state);
}
