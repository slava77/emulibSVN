global int EMUMAJ_HV_STATE_ON_VMON_ACCURACY = 50;
global int EMUMAJ_HV_STANDBY_VOLTAGE = 3000;

/** values here are ".status",".off_channels", ".last_vset". States are ON, STANDBY and ERROR */
dyn_int emumaj_hvStateCounts(dyn_anytype values, int &weight, bool calcTotal, string node, string majType) {
  mapping deviceParams = emumaj_getChamberDeviceParams(node);

  // check if the real type (outer or inner) of the given device is equal to the expected majority type
  // if not - return total = 0. This way we differenciate inner and outer chambers without two different DU types
  string type = "HV_OUTER";
  if (deviceParams["ring"] == 1) {
    type = "HV_INNER";
  }
  if (type != majType) {
    weight = 0;
    return makeDynInt(0, 0, 0);
  }
  //----------------------------------------------------------------------------------------------------
  
  dyn_int excludedChannels = values[2];
  int status = values[1];
  int channelCount;
  if ((deviceParams["station"] == 1) || (deviceParams["ring"] == 1)) {
    channelCount = 18;
  } else {
    channelCount = 30;
  }
  if (calcTotal) {
    weight = channelCount;
  } else {
    weight = channelCount - dynlen(excludedChannels);
  }
  
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
  bool checkChannelAlarms = false;
  if ((fsmState == "DEAD") || (status < 0)) {
    checkChannelAlarms = true;
  }
  
  // go through all the channels (except the masked ones) and collect their states
  string dataDp = node;
  strreplace(dataDp, "HighVoltage/", "");
  for (int i = 1 + channelsOffset; i <= channelsOffset + channelCount; i++) {
    if (dynContains(excludedChannels, i) && !calcTotal) { continue; }
    dyn_int chStates = emumaj_hvChannelStates(dataDp + ".data.v" + i, 
                                                  vset, EMUMAJ_HV_STANDBY_VOLTAGE,
                                                  true);
    on += chStates[1];
    standby += chStates[2];
    error += chStates[3];
  }
  
  /** if fsm is in ERROR, but channels don't have any alarms - it means that there is a more general problem 
      (perhaps a master channel trip), in which case all channels should be marked with error. */
//  if (((fsmState == "ERROR") && (error == 0)) || (status == -2)) {
  if ((status == -2) || ((status < 0) && (error == 0))){
    error = weight;
  }
  
  return makeDynInt(on, standby, error);
}

dyn_int emumaj_hvChannelStates(string dp, int onVoltage, int standbyVoltage, bool checkForAlerts = true) {
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

  if ((vmon > standbyVoltage - EMUMAJ_HV_STATE_ON_VMON_ACCURACY) && 
            (vmon < standbyVoltage + EMUMAJ_HV_STATE_ON_VMON_ACCURACY)){
    standby = 1;
  } else if ((vmon > onVoltage - EMUMAJ_HV_STATE_ON_VMON_ACCURACY) && 
      (vmon < onVoltage + EMUMAJ_HV_STATE_ON_VMON_ACCURACY)) {
    on = 1;
  }
  
  return makeDynInt(on, standby, error);
}

/** values here are ".status", ".chamber_state", ".noalert_channels". States are ON, ERROR and NO_COMMUNICATION */
dyn_int emumaj_lvStateCounts(dyn_anytype values, int &weight, bool calcTotal, string node) {
  mapping deviceParams = emumaj_getChamberDeviceParams(node);
  dyn_int excludedChannels = values[3];
  int status = values[1];
  int chamberState = values[2];
  int channelCount;
  if ((deviceParams["station"] == 1) && (deviceParams["ring"] == 3)) {
    channelCount = 16; // ME1/3 chambers have 4 CFEBs, all other chambers have 5
  } else {
    channelCount = 19;
  }
  int excludedChannelCount = 0;
  for (int i=1; i <= dynlen(excludedChannels); i++) {
    int channelNum = excludedChannels[i];
    // CFEB5 doesn't exist in ME1/3 chambers so don't care about this one
    if ((deviceParams["station"] == 1) && (deviceParams["ring"] == 3) && (channelNum == 5)) {
      continue;
    }
    // channel 6 is ALCT which has 4 voltage lines, anything less than 6 is a CFEB which has 3 voltage lines
    if (channelNum < 6) {
      excludedChannelCount += 3;
    } else {
      excludedChannelCount += 4;
    }
  }
  if (calcTotal) {
    weight = channelCount;
  } else {
    weight = channelCount - excludedChannelCount;
  }
  
  int on = 0,
      error = 0,
      noCommunication = 0;
  
  if ((chamberState == -2) || (status == -2)) {
    noCommunication = weight;
    return makeDynInt(on, error, noCommunication);
  }
  
  // go through all the channels (except the masked ones) and collect their states
  string dataDp = node;
  strreplace(dataDp, "LowVoltage/", "");
  for (int i=1; i <= 6; i++) {
    if (dynContains(excludedChannels, i) && !calcTotal) { continue; }
    // CFEB5 doesn't exist in ME1/3 chambers so don't care about this one
    if ((deviceParams["station"] == 1) && (deviceParams["ring"] == 3) && (i == 5)) { continue; }
    
    if (i < 6) { //CFEB
      dyn_int v33States = emumaj_lvCfebChannelStates(dataDp, i, "33");
      dyn_int v50States = emumaj_lvCfebChannelStates(dataDp, i, "50");
      dyn_int v60States = emumaj_lvCfebChannelStates(dataDp, i, "60");
      
      on += v33States[1] + v50States[1] + v60States[1];
      error += v33States[2] + v50States[2] + v60States[2];
    } else { // ALCT
      dyn_int v18States = emumaj_lvAlctChannelStates(dataDp, "18");
      dyn_int v33States = emumaj_lvAlctChannelStates(dataDp, "33");
      dyn_int v55States = emumaj_lvAlctChannelStates(dataDp, "55");
      dyn_int v56States = emumaj_lvAlctChannelStates(dataDp, "56");

      on += v18States[1] + v33States[1] + v55States[1] + v56States[1];
      error += v18States[2] + v33States[2] + v55States[2] + v56States[2];
    }
  }
  
  /** if fsm is in ERROR, but channels don't have any alarms - it means that there is a more general problem 
      (perhaps a master channel trip), in which case all channels should be marked with error. */
  
  return makeDynInt(on, error, noCommunication);
}

dyn_int emumaj_lvCfebChannelStates(string dataDp, int cfebNumber, string voltageLine) {
  int voltageAlert, currentAlert;
  float voltage;
  dpGet(dataDp + ".data.Cfeb_o.v" + voltageLine + ".v" + cfebNumber, voltage);
  dpGet(dataDp + ".data.Cfeb_o.v" + voltageLine + ".v" + cfebNumber + ":_alert_hdl.._act_state", voltageAlert);
  dpGet(dataDp + ".data.Cfeb_o.c" + voltageLine + ".v" + cfebNumber + ":_alert_hdl.._act_state", currentAlert);
  
  int on = 0,
      error = 0;
  if (voltage > 0.1) { on = 1; }
  if (voltageAlert + currentAlert > 0) {
    error = 1;
  }
  
  return makeDynInt(on, error);
}
    
dyn_int emumaj_lvAlctChannelStates(string dataDp, string voltageLine) {
  int voltageAlert, currentAlert;
  float voltage;
  dpGet(dataDp + ".data.Alct_o.v" + voltageLine, voltage);
  dpGet(dataDp + ".data.Alct_o.v" + voltageLine + ":_alert_hdl.._act_state", voltageAlert);
  dpGet(dataDp + ".data.Alct_o.c" + voltageLine + ":_alert_hdl.._act_state", currentAlert);
  
  int on = 0,
      error = 0;
  if (voltage > 0.1) { on = 1; }
  if (voltageAlert + currentAlert > 0) {
    error = 1;
  }
  
  return makeDynInt(on, error);
}

/** values here are ".status", ".chamber_state", ".off_channels". States are OK, ALERT and NO_COMMUNICATION */
dyn_int emumaj_temperatureStateCounts(dyn_anytype values, int &weight, bool calcTotal, string node) {
  mapping deviceParams = emumaj_getChamberDeviceParams(node);
  dyn_int excludedChannels = values[3];
  int status = values[1];
  int chamberState = values[2];
  int channelCount;
  if ((deviceParams["station"] == 1) && (deviceParams["ring"] == 3)) {
    channelCount = 6; // ME1/3 chambers have 4 CFEBs, all other chambers have 5
    int idx = dynContains(excludedChannels, 5);
    if (idx > 0) {
      dynRemove(excludedChannels, idx);
    }
  } else {
    channelCount = 7;
  }

  if (calcTotal) {
    weight = channelCount;
  } else {
    weight = channelCount - dynlen(excludedChannels);
  }
  
  int ok = 0,
      error = 0,
      noCommunication = 0;
  
  if ((chamberState == -2) || (status == -2)) {
    noCommunication = weight;
    return makeDynInt(ok, error, noCommunication);
  }
  
  // go through all the channels (except the masked ones) and collect their states
  string dataDp = node;
  strreplace(dataDp, "Temperature/", "");
  for (int i=1; i <= 7; i++) {
    if (dynContains(excludedChannels, i) && !calcTotal) { continue; }
    // CFEB5 doesn't exist in ME1/3 chambers so don't care about this one
    if ((deviceParams["station"] == 1) && (deviceParams["ring"] == 3) && (i == 5)) { continue; }
    
    if (i < 6) { //CFEB
      int alert;
      dpGet(dataDp + ".data.t_cfeb.v" + i + ":_alert_hdl.._act_state", alert);
      if ((alert > 0) && (status < 0)) {
        error++;
      } else {
        ok++;
      }
    } else if (i == 6) { // ALCT
      int alert;
      dpGet(dataDp + ".data.t_alct.v1:_alert_hdl.._act_state", alert);
      if ((alert > 0) && (status < 0)) {
        error++;
      } else {
        ok++;
      }
    } else if (i == 7) { // DMB
      int alert;
      dpGet(dataDp + ".data.t_dmb.v1:_alert_hdl.._act_state", alert);
      if ((alert > 0) && (status < 0)) {
        error++;
      } else {
        ok++;
      }
    }
  }
  
  /** if fsm is in ERROR, but channels don't have any alarms - it means that there is a more general problem 
      (perhaps a master channel trip), in which case all channels should be marked with error. */
  
  return makeDynInt(ok, error, noCommunication);
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

/** standard .status DP. > 1 means ON, < 0 means ERROR, -2 means NO_COMMUNICATION, else means OFF.
  values here are ".status". States are ON, ERROR and NO_COMMUNICATION */
dyn_int emumaj_onOffErrorNoCommStatusDpCounts(dyn_anytype values, int &weight, bool calcTotal, string node, bool noCommMeansOn) {
  int status = values[1];
  int on = 0,
      error = 0,
      noCommunication = 0;
  
  if (status > 1) {
    on = 1;
  } else if (status == -2) {
    if (noCommMeansOn) {
      on = 1;
    }
    noCommunication = 1;
  } else if (status < 0) {
    on = 1;
    error = 1;
  }
  
  return makeDynInt(on, error, noCommunication);
}

/** standard .status DP. > 1 means ON, < 0 means ERROR, else means OFF.
  values here are ".status". States are ON, ERROR and NO_COMMUNICATION */
dyn_int emumaj_onOffErrorStatusDpCounts(dyn_anytype values, int &weight, bool calcTotal, string node, bool noCommMeansOn) {
  int status = values[1];
  int on = 0,
      error = 0;
  
  if (status > 1) {
    on = 1;
  } else if (status < 0) {
    if (noCommMeansOn && (status == -2)) {
      on = 1;
    }
    error = 1;
  }
  
  return makeDynInt(on, error);
}

/** value here is ".fsm.currentState" (internal FSM DPE). States are ON and ERROR */
dyn_int emumaj_onOffErrorFsmStateCounts(dyn_anytype values, int &weight, bool calcTotal, string node) {
  string state = values[1];
  int on = 0,
      error = 0;
  
  if (state == "ON") {
    on = 1;
  } else if (state == "OFF") {
  } else if ((state == "ERROR") || (state == "DEAD")) {
    error = 1;
  } else {
    error = 1;
  }
  
  return makeDynInt(on, error);
}

/** value here is ".fsm.currentState" (internal FSM DPE). States are ON, STANDBY and ERROR */
dyn_int emumaj_onOffStandbyErrorFsmStateCounts(dyn_anytype values, int &weight, bool calcTotal, string node) {
  string state = values[1];
  int on = 0,
      standby = 0,
      error = 0;
  
  if (state == "ON") {
    on = 1;
  } else if (state == "STANDBY") {
    standby = 1;
  } else if (state == "OFF") {
  } else if ((state == "ERROR") || (state == "DEAD")) {
    error = 1;
  } else {
    error = 1;
  }
  
  return makeDynInt(on, standby, error);
}
