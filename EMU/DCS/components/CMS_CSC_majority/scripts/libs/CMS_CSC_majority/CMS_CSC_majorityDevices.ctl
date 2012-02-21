#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_MiddleLayer/emu_x2p.ctl"
#uses "CMS_CSC_MiddleLayer/emu_hv.ctl"

/** values is .channel_states. States are ON, STANDBY, RAMPING and ERROR */
dyn_int emumaj_hvStateCounts(dyn_anytype values, int &weight, bool calcTotal, string node, string majType) {
  if (weight == 0) {
    return makeDynInt(0, 0, 0, 0);
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

/** values here are ".fsm_state", ".disabled_channels". States are ON, ERROR and NO_COMMUNICATION */
dyn_int emumaj_lvStateCounts(dyn_anytype values, int &weight, bool calcTotal, string node) {
  dyn_string disabledChannels = values[2];
  string fsmState = values[1];
  
  if (!calcTotal) {
    weight -= dynlen(disabledChannels);
  }
  
  int on = 0,
      error = 0,
      noCommunication = 0;
  
  if (fsmState == EMUHV_FSM_STATE_NO_COMM) { // no communication
    on = weight;
    noCommunication = weight;
    return makeDynInt(on, error, noCommunication);
  }
  if (fsmState == EMU_X2P_FSM_STATE_ON) { // everything is ok
    on = weight;
    return makeDynInt(on, error, noCommunication);
  }
  if (fsmState == EMU_X2P_FSM_STATE_OFF) { // chamber is off - so all 0 (not an error, not ok and communication is fine - because X2P is telling us that)
    return makeDynInt(on, error, noCommunication);
  }

  // everything that's not masked off is on - otherwise it would be in error or off state (in case everything is off).
  on = weight;
  // go through all the channels (except the masked ones) and check how many of them are in error
  string dataDp = node + "/Mon";

  // LVDB
  int v7AnalogAlert, v7DigitalAlert;
  dpGet(dataDp + ".lvdb.v7Analog:_alert_hdl.._act_state", v7AnalogAlert,
        dataDp + ".lvdb.v7Digital:_alert_hdl.._act_state", v7DigitalAlert);
  // if LVDB voltages are bad, just mark everything bad
  if (v7AnalogAlert > 0) { error = weight; return makeDynInt(on, error, noCommunication); }
  if (v7DigitalAlert > 0) { error = weight; return makeDynInt(on, error, noCommunication); }
  
  // check CFEB alerts
  for (int i=1; i <= 5; i++) {
    if (((strpos(node, "_M13_") >= 0) || (strpos(node, "_P13_") >= 0)) && (i == 5)) { continue; }  // CFEB5 doesn't exist in ME1/3 chambers so don't care about this one

    if (emumaj_lvCfebChannelAlertStatus(dataDp, i, "33", disabledChannels, !calcTotal)) { error++; }
    if (emumaj_lvCfebChannelAlertStatus(dataDp, i, "50", disabledChannels, !calcTotal)) { error++; }
    if (emumaj_lvCfebChannelAlertStatus(dataDp, i, "60", disabledChannels, !calcTotal)) { error++; }
  }
  // check ALCT alerts
  if (emumaj_lvAlctChannelAlertStatus(dataDp, "18", disabledChannels, !calcTotal)) { error++; }
  if (emumaj_lvAlctChannelAlertStatus(dataDp, "33", disabledChannels, !calcTotal)) { error++; }
  if (emumaj_lvAlctChannelAlertStatus(dataDp, "55", disabledChannels, !calcTotal)) { error++; }
  if (emumaj_lvAlctChannelAlertStatus(dataDp, "56", disabledChannels, !calcTotal)) { error++; }

  return makeDynInt(on, error, noCommunication);
}

/**
  * @return true if the given CFEB LV channel has an alert
  * @param ignoreDisabled if true then channels that are in disabledChannels param will be ignored
  */
bool emumaj_lvCfebChannelAlertStatus(string dataDp, int cfebNumber, string voltageLine, dyn_string disabledChannels, bool ignoreDisabled) {
  int voltageAlert = 0,
      currentAlert = 0;
  
  string voltageDp = dataDp + ".cfeb.v" + voltageLine + ".v" + cfebNumber,
         currentDp = dataDp + ".cfeb.c" + voltageLine + ".v" + cfebNumber;
  
  if (!ignoreDisabled || !dynContains(disabledChannels, voltageDp)) {
    dpGet(voltageDp + ":_alert_hdl.._act_state", voltageAlert);
  }
  if (!ignoreDisabled || !dynContains(disabledChannels, currentDp)) {
    dpGet(currentDp + ":_alert_hdl.._act_state", currentAlert);
  }
  
  return (voltageAlert + currentAlert > 0);
}
    
/**
  * @return true if the given ALCT LV channel has an alert
  * @param ignoreDisabled if true then channels that are in disabledChannels param will be ignored
  */
bool emumaj_lvAlctChannelAlertStatus(string dataDp, string voltageLine, dyn_string disabledChannels, bool ignoreDisabled) {
  int voltageAlert = 0,
      currentAlert = 0;

  string voltageDp = dataDp + ".alct.v" + voltageLine,
         currentDp = dataDp + ".alct.c" + voltageLine;

  if (!ignoreDisabled || !dynContains(disabledChannels, voltageDp)) {
    dpGet(voltageDp + ":_alert_hdl.._act_state", voltageAlert);
  }
  if (!ignoreDisabled || !dynContains(disabledChannels, currentDp)) {
    dpGet(currentDp + ":_alert_hdl.._act_state", currentAlert);
  }
  
  return (voltageAlert + currentAlert > 0);
}

/** values here are ".fsm_state", ".disabled_channels". States are OK, ALERT and NO_COMMUNICATION */
dyn_int emumaj_temperatureStateCounts(dyn_anytype values, int &weight, bool calcTotal, string node) {
  dyn_string disabledChannels = values[2];
  string fsmState = values[1];

  if (!calcTotal) {
    weight -= dynlen(disabledChannels);
  }
  
  int ok = 0,
      error = 0,
      noCommunication = 0;
  
if (fsmState == EMUHV_FSM_STATE_NO_COMM) { // no communication
    ok = weight;
    noCommunication = weight;
    return makeDynInt(ok, error, noCommunication);
  }
  if (fsmState == EMU_X2P_FSM_STATE_ON) { // everything is ok
    ok = weight;
    return makeDynInt(ok, error, noCommunication);
  }
  if (fsmState == EMU_X2P_FSM_STATE_OFF) { // chamber is off - so all 0 (not an error, not ok and communication is fine - because X2P is telling us that)
    return makeDynInt(ok, error, noCommunication);
  }
  
  
  // go through all the channels (except the masked ones) and collect their alert states
  ok = weight;
  string dataDp = node + "/Mon";

  // check CFEB alerts
  for (int i=1; i <= 5; i++) {
    if (((strpos(node, "_M13_") >= 0) || (strpos(node, "_P13_") >= 0)) && (i == 5)) { continue; }  // CFEB5 doesn't exist in ME1/3 chambers so don't care about this one

    string cfebDp = dataDp + ".cfeb.v" + i;
    if (!calcTotal && dynContains(disabledChannels, cfebDp)) { continue; } // skip if disabled
        
    int alert;
    dpGet(cfebDp + ":_alert_hdl.._act_state", alert);
    if (alert > 0) {
      error++;
      ok--;
    }
  }
  
  // check ALCT alert
  string alctDp = dataDp + "alct.v1";
  if (calcTotal || !dynContains(disabledChannels, alctDp)) {
    int alert;
    dpGet(alctDp + ":_alert_hdl.._act_state", alert);
    if (alert > 0) {
      error++;
      ok--;
    }
  }
  
  // check DMB alert
  string dmbDp = dataDp + "dmb.v1";
  if (calcTotal || !dynContains(disabledChannels, dmbDp)) {
    int alert;
    dpGet(dmbDp + ":_alert_hdl.._act_state", alert);
    if (alert > 0) {
      error++;
      ok--;
    }
  }
  
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
