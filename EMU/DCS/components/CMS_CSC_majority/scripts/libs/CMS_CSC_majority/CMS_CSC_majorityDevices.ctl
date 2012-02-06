#uses "CMS_CSC_common/emu_common.ctl"

/** values is .channel_states. States are ON, STANDBY, RAMPING and ERROR */
dyn_int emumaj_hvStateCounts(dyn_anytype values, int &weight, bool calcTotal, string node, string majType) {
  if (weight == 0) {
    return;
  }
  
  dyn_string channelStates = values[1];
  
  int on = 0,
      standby = 0,
      ramping = 0,
      error = 0,
      disabled = 0;
  
  for (int i=1; i <= dynlen(channelStates); i++) {
    string chState = channelStates[i];
    
    if (strpos(chState, EMUHV_FSM_STATE_DISABLED) >= 0) {
      disabled++;
      continue;
    }
    if (strpos(chState, EMUHV_FSM_STATE_ON) >= 0) {
      on++;
    }
    if (strpos(chState, EMUHV_FSM_STATE_STANDBY) >= 0) {
      standby++;
    }
    if (strpos(chState, EMUHV_FSM_STATE_RAMPING) >= 0) {
      ramping++;
    }
    if (strpos(chState, EMUHV_FSM_STATE_ERROR) >= 0) {
      error++;
    }
    
  }
  
  if (!calcTotal) {
    weight -= disabled;
  }
  
  return makeDynInt(on, standby, ramping, error);
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
