global const int EMUMAJ_UFPNPI_STATE_ON_VMON_ACCURACY = 50;
global const int EMUMAJ_UFPNPI_STANDBY_VOLTAGE = 3000;

/** values here are ".status",".off_channels", ".last_vset". */
dyn_int emumaj_ufpnpiStateCounts(dyn_anytype values, int &weight, bool calcTotal, string node) {
  mapping deviceParams = emumaj_getChamberDeviceParams(node);
  dyn_int excludedChannels = values[2];
  int channelCount;
  if ((deviceParams["station"] == 1) || (deviceParams["ring"] == 1)) {
    channelCount = 18;
  } else {
    channelCount = 30;
  }
  weight = channelCount - dynlen(excludedChannels);
  
  int vset = values[3];
  int on = 0,
      standby = 0,
      error = 0;
  
  // determine the channel offset
  int channelsOffset = 0;
  string coord;
  dpGet(node + ".coord", coord);
  dyn_string coordSplit = strsplit(coord, ";");
  if (dynlen(coordSplit) < 4) {
    error = weight;
    return makeDynInt(on, standby, error);
  }
  int part = strsplit(coord, ";")[4];
  if (part == 2) {
    channelsOffset = 18;
  }

  //check the FSM state - if it's ERROR or DEAD then check the channel alert statuses, if not then do not
  string fsmDp = treeCache_getFsmInternalDp(node);
  string fsmState;
  dpGet(fsmDp + ".fsm.currentState", fsmState);
  DebugTN("fsm state of " + deviceParams["side"] + deviceParams["station"] + deviceParams["ring"] + "_" + deviceParams["chamberNumber"] + ": " + fsmState);
  bool checkChannelAlarms = false;
  if ((fsmState == "ERROR") || (fsmState == "DEAD")) {
    checkChannelAlarms = true;
  }
  
  // go through all the channels (except the masked ones) and collect their states
  string dataDp = node;
  strreplace(dataDp, "HighVoltage/", "");
  for (int i = 1 + channelsOffset; i <= channelsOffset + channelCount; i++) {
    if (dynContains(excludedChannels, i)) { continue; }
    dyn_int chStates = emumaj_ufpnpiChannelStates(dataDp + ".data.v" + i, 
                                                  vset, EMUMAJ_UFPNPI_STANDBY_VOLTAGE,
                                                  checkChannelAlarms);
    on += chStates[1];
    standby += chStates[2];
    error += chStates[3];
  }
  
  /** if fsm is in ERROR, but channels don't have any alarms - it means that there is a more general problem 
      (perhaps a master channel trip), in which case all channels should be marked with error. */
  if ((fsmState == "ERROR") && (error == 0)) {
    error = weight;
  }
  
  DebugTN("majority states for " + deviceParams["side"] + deviceParams["station"] + deviceParams["ring"] + "_" + deviceParams["chamberNumber"] + ": on=" + on + ", standby=" + standby + ", error=" + error);
  return makeDynInt(on, standby, error);
}

dyn_int emumaj_ufpnpiChannelStates(string dp, int onVoltage, int standbyVoltage, bool checkForAlerts = true) {
  float vmon;
  int alert = 0;
  dpGet(dp + ".vmon", vmon);
  if (checkForAlerts) {
    dpGet(dp + ".status:_alert_hdl.._act_state", alert);
  }
  
  int on = 0,
      standby = 0,
      error = 0;

  if (alert > 0) {
    error = 1;
  }  
  
  if ((vmon > onVoltage - EMUMAJ_UFPNPI_STATE_ON_VMON_ACCURACY) && 
      (vmon < onVoltage + EMUMAJ_UFPNPI_STATE_ON_VMON_ACCURACY)) {
    on = 1;
  } else if ((vmon > standbyVoltage - EMUMAJ_UFPNPI_STATE_ON_VMON_ACCURACY) && 
            (vmon < standbyVoltage + EMUMAJ_UFPNPI_STATE_ON_VMON_ACCURACY)){
    standby = 1;
  }
  
  return makeDynInt(on, standby, error);
}

/** Takes any kind of chamber device (HV, LV, TEMP) and returns a mapping with elements:
                                                   dataDp, side, station, ring, chamberNumber.
*/
mapping emumaj_getChamberDeviceParams(string nodeName) {
  dyn_string split = strsplit(nodeName, "/");
  string dataDp = nodeName;
  if (dynlen(split) > 1) {
    dataDp = split[2];
  }
  
  string tmpStr = dataDp;
  strreplace(tmpStr, "CSC_ME_", "");
  split = strsplit(tmpStr, "_");
  strreplace(split[2], "C", "");

  mapping deviceParams;
  deviceParams["dataDp"] = dataDp;
  deviceParams["side"] = (string) split[1][0];
  deviceParams["station"] = (int)(string) split[1][1];
  deviceParams["ring"] = (int)(string) split[1][2];
  deviceParams["chamberNumber"] = (int) split[2];
  
  return deviceParams;
}
