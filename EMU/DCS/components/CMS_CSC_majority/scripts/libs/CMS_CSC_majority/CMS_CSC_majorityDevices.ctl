#uses "CMS_CSC_common/emu_common.ctl"

global int EMUMAJ_HV_STATE_ON_VMON_ACCURACY = 50;
global int EMUMAJ_HV_STATE_STANDBY_VMON_ACCURACY = 60;
global int EMUMAJ_HV_STANDBY_VOLTAGE = 3000;
global mapping hvParts;
global mapping hvOnChannelVsets;

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
  
//  dyn_int onChVsets = values[4];
  
  int vset = values[3];
  int on = 0,
      standby = 0,
      error = 0;
  
  // determine the channel offset
  int channelsOffset = 0;
  int part = getHvPart(node);
  if (part < 0) {
    error = weight;
    return makeDynInt(on, standby, error);
  }
  if (part == 2) {
    channelsOffset = 18;
  }

  bool checkChannelAlarms = false;
  if (status < 0) {
    checkChannelAlarms = true;
  }

  dyn_int onChVsets;
  if (!mappingHasKey(hvOnChannelVsets, node)) {
    dpGet(node + ".on_ch_vsets", onChVsets);
    hvOnChannelVsets[node] = onChVsets;
    dpConnect("emumaj_hvChannelOnVoltageSettingChangedCB", false, node + ".on_ch_vsets");
  } else {
    onChVsets = hvOnChannelVsets[node];
  }
   
  // go through all the channels (except the masked ones) and collect their states
  string dataDp = node;
  strreplace(dataDp, "HighVoltage/", "");
  for (int i = 1 + channelsOffset; i <= channelsOffset + channelCount; i++) {
    if (dynContains(excludedChannels, i) && !calcTotal) { continue; }
    int chVset = vset;
    
     if (dynlen(onChVsets) < channelCount) {
       emu_info("onChVsets length is only " + dynlen(onChVsets) + " for " + node);
     } else {
       // for a chamber-wide setting of less than 3400V - most likely standby - apply the chamber-wide vset
       // for a chamber-wide setting of more than 3400V - most likely ON - apply the individual channel vset
       if (vset >= 3400) {
         chVset = onChVsets[i - channelsOffset];
       }
     }
    dyn_int chStates = emumaj_hvChannelStates(dataDp + ".data.v" + i, 
                                                  chVset, EMUMAJ_HV_STANDBY_VOLTAGE,
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

void emumaj_hvChannelOnVoltageSettingChangedCB(string dp, dyn_int onChVsets) {
  string node = dpSubStr(dp, DPSUB_DP);
  hvOnChannelVsets[node] = onChVsets;
}

dyn_int emumaj_hvChannelStates(string dp, int vset, int standbyVoltage, bool checkForAlerts = true) {
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

  if (vset > standbyVoltage + EMUMAJ_HV_STATE_ON_VMON_ACCURACY) { // going to ON
    if (vmon > vset - EMUMAJ_HV_STATE_ON_VMON_ACCURACY) { // it's ON
      on = 1;
    } else if (vmon > standbyVoltage - EMUMAJ_HV_STATE_ON_VMON_ACCURACY) { // it's passed standby voltage - keep in standby
      standby = 1;
    } // else it's off
  } else {
    // it's in standby
    if ((vmon <= standbyVoltage + EMUMAJ_HV_STATE_STANDBY_VMON_ACCURACY) && 
        (vmon >= standbyVoltage - EMUMAJ_HV_STATE_STANDBY_VMON_ACCURACY)){
      standby = 1;
    } else if (vmon > standbyVoltage + EMUMAJ_HV_STATE_ON_VMON_ACCURACY) {
      on = 1;
    }
  }
  
  return makeDynInt(on, standby, error);
}

/** values here are ".status", ".chamber_state", ".noalert_channels". States are ON, ERROR and NO_COMMUNICATION */
dyn_int emumaj_lvStateCounts(dyn_anytype values, int &weight, bool calcTotal, string node) {
  mapping deviceParams = emumaj_getChamberDeviceParams(node);
  dyn_int excludedChannels = values[2];
  int status = values[1];
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
  
  if (status == -2) { // no communication
    on = weight;
    noCommunication = weight;
    return makeDynInt(on, error, noCommunication);
  }
  if (status == 2) { // everything is ok
    on = weight;
    return makeDynInt(on, error, noCommunication);
  }
  if (status == 0) { // chamber is off - so all 0 (not an error, not ok and communication is fine - because X2P is telling us that)
    return makeDynInt(on, error, noCommunication);
  }

  // everything that's not masked off is on - otherwise it would be in error or off state (in case everything is off).
  on = weight;
  // go through all the channels (except the masked ones) and check how many of them are in error
  string dataDp = node;
  strreplace(dataDp, "LowVoltage/", "");

  // LVDB
  int v7AnalogAlert, v7DigitalAlert;
  dpGet(dataDp + ".data.Lvdb_o.v7Analog:_alert_hdl.._act_state", v7AnalogAlert,
        dataDp + ".data.Lvdb_o.v7Digital:_alert_hdl.._act_state", v7DigitalAlert);
  // if LVDB voltages are bad, just mark everything bad
  if (v7AnalogAlert > 0) { error = weight; return makeDynInt(on, error, noCommunication); }
  if (v7DigitalAlert > 0) { error = weight; return makeDynInt(on, error, noCommunication); }
  
  for (int i=1; i <= 6; i++) {
    if (dynContains(excludedChannels, i) && !calcTotal) { continue; }
    // CFEB5 doesn't exist in ME1/3 chambers so don't care about this one
    if ((deviceParams["station"] == 1) && (deviceParams["ring"] == 3) && (i == 5)) { continue; }
    
    if (i < 6) { //CFEB
      if (emumaj_lvCfebChannelAlertStatus(dataDp, i, "33")) { error++; }
      if (emumaj_lvCfebChannelAlertStatus(dataDp, i, "50")) { error++; }
      if (emumaj_lvCfebChannelAlertStatus(dataDp, i, "60")) { error++; }
    } else { // ALCT
      if (emumaj_lvAlctChannelAlertStatus(dataDp, "18")) { error++; }
      if (emumaj_lvAlctChannelAlertStatus(dataDp, "33")) { error++; }
      if (emumaj_lvAlctChannelAlertStatus(dataDp, "55")) { error++; }
      if (emumaj_lvAlctChannelAlertStatus(dataDp, "56")) { error++; }
    }
  }
  
  /** if fsm is in ERROR, but channels don't have any alarms - it means that there is a more general problem 
      (perhaps a master channel trip), in which case all channels should be marked with error. */
  
  return makeDynInt(on, error, noCommunication);
}

bool emumaj_lvCfebChannelAlertStatus(string dataDp, int cfebNumber, string voltageLine) {
  int voltageAlert, currentAlert;

  dpGet(dataDp + ".data.Cfeb_o.v" + voltageLine + ".v" + cfebNumber + ":_alert_hdl.._act_state", voltageAlert);
  dpGet(dataDp + ".data.Cfeb_o.c" + voltageLine + ".v" + cfebNumber + ":_alert_hdl.._act_state", currentAlert);
  
  return (voltageAlert + currentAlert > 0);
}
    
bool emumaj_lvAlctChannelAlertStatus(string dataDp, string voltageLine) {
  int voltageAlert, currentAlert;

  dpGet(dataDp + ".data.Alct_o.v" + voltageLine + ":_alert_hdl.._act_state", voltageAlert);
  dpGet(dataDp + ".data.Alct_o.c" + voltageLine + ":_alert_hdl.._act_state", currentAlert);
  
  return (voltageAlert + currentAlert > 0);
}

/** values here are ".status", ".chamber_state", ".off_channels". States are OK, ALERT and NO_COMMUNICATION */
dyn_int emumaj_temperatureStateCounts(dyn_anytype values, int &weight, bool calcTotal, string node) {
  mapping deviceParams = emumaj_getChamberDeviceParams(node);
  dyn_int excludedChannels = values[2];
  int status = values[1];
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
  
  if (status == -2) { // no communication
    ok = weight;
    noCommunication = weight;
    return makeDynInt(ok, error, noCommunication);
  }
  if (status == 2) { // everything is ok
    ok = weight;
    return makeDynInt(ok, error, noCommunication);
  }
  if (status == 0) { // chamber is off - so all 0 (not an error, not ok and communication is fine - because X2P is telling us that)
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
dyn_int emumaj_onOffErrorStatusDpCounts(dyn_anytype values, int &weight, bool calcTotal, string node, bool noCommMeansOn, bool noCommMeansError = true) {
  int status = values[1];
  int on = 0,
      error = 0;
  
  if (status > 1) {
    on = 1;
  } else if (status < 0) {
    if (status == -2) { // means no communication
      if (noCommMeansOn) {
        on = 1;
      }
      if (noCommMeansError) {
        error = 1;
      }
    } else {
      error = 1;
    }
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

int getHvPart(string node) {
  if (!mappingHasKey(hvParts, node)) {
    string coord;
    dpGet(node + ".coord", coord);
    dyn_string coordSplit = strsplit(coord, ";");
    if (dynlen(coordSplit) < 4) {
      return -1;
    }
    hvParts[node] = strsplit(coord, ";")[4];
  }
  
  return hvParts[node];
}
