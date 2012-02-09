/**@file

This manager determines HV states from the incoming data and executes HV FSM commands
by hooking up to HV command DPEs and translating the arriving FSM commands to 
HV commands and sending them over to HV server over DIM.

@author Evaldas Juska (FNAL)
@date   June 2011
*/

#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_MiddleLayer/emu_hv.ctl"

const int EMUHV_STATE_OFF_HIGHEST_VMON = 80;
const int EMUHV_STATE_ON_VMON_ACCURACY = 50;
const int EMUHV_STATE_STANDBY_VMON_ACCURACY = 60;

main() {
  emu_info("HV Client: starting up...");
  
  // run integrity check
  emu_info("HV Client: running integrity checks...");
  dyn_string ex;
  emuhv_checkIntegrity(ex);
  if (!emu_checkException(ex)) {
    emu_info("HV Client: no critical integrity errors found");
  }
  
  // hook up to monitoring DPEs
  dyn_string monDps = dpNames(EMUHV_DP_PREFIX + "*.num_chans", "CscHvChamberFastMon");
  for (int i=1; i <= dynlen(monDps); i++) {
    dpConnect("emuhv_dataUpdatedCB", true, monDps[i]);
  }
  // hook up to primary monitoring DPEs
  dyn_string primaryMonDps = dpNames(EMUHV_DP_PREFIX + "*.status", "CscHvPrimaryMon");
  for (int i=1; i <= dynlen(primaryMonDps); i++) {
    dpConnect("emuhv_primaryDataUpdatedCB", true, primaryMonDps[i]);
  }

  
  // hook up to FSM command DPEs
  dyn_string dps = dpNames(EMUHV_DP_PREFIX + "*.fsm_command", "CscHvChamber");
  for (int i=1; i <= dynlen(dps); i++) {
    dpConnect("emuhv_commandReceivedCB", false, dps[i]);
  }
  // hook up to primary FSM command DPEs
  dyn_string primaryCommandDps = dpNames(EMUHV_DP_PREFIX + "*.fsm_command", "CscHvPrimary");
  for (int i=1; i <= dynlen(primaryCommandDps); i++) {
    dpConnect("emuhv_primaryCommandReceivedCB", false, primaryCommandDps[i]);
  }
  
  emu_info("HV Client: started up successfully");
}

/**
  * Callback that's hooked up to each chamber (just one DPE per chamber).
  * It then delegates the call to emuhv_updateState which then recomputes the HV state for that chamber and all its channels.
  */
void emuhv_dataUpdatedCB(string dp, string value) {
  string hvDp = substr(dpSubStr(dp, DPSUB_DP), 0, strlen(EMUHV_DP_PREFIX + "CSC_ME_M##_C##"));
  emuhv_updateChamberState(hvDp);
}

/**
  * Callback that's hooked up to each primary PSU Mon DP (just one DPE per PSU).
  * It looks at the monitoring data and updates the FSM state of the corresponding primary PSU.
  */
void emuhv_primaryDataUpdatedCB(string dp, string value) {
  string monDp = dpSubStr(dp, DPSUB_DP);
  string fsmDp = monDp;
  strreplace(fsmDp, "/Mon", "");
  int hvState = value;
  
  int alert = 0;
  dpGet(fsmDp + ".:_alert_hdl.._act_state", alert);

  string fsmState;
  if (alert) {
    fsmState = EMUHV_FSM_STATE_ERROR;
  } else {
    if (hvState == EMUHV_STATE_OFF) {
      fsmState = EMUHV_FSM_STATE_OFF;
    } else if ((hvState == EMUHV_STATE_RAMP_UP) || (hvState == EMUHV_STATE_RAMP_DOWN)) {
      fsmState = EMUHV_FSM_STATE_RAMPING;
    } else if (hvState == EMUHV_STATE_ON) {
      int vmon;
      dpGet(monDp + ".vmon", vmon);
      if (vmon > EMUHV_VOLTAGE_NOMINAL - EMUHV_STATE_ON_VMON_ACCURACY) {
        fsmState = EMUHV_FSM_STATE_ON;
      } else if (vmon > EMUHV_VOLTAGE_STANDBY - EMUHV_STATE_STANDBY_VMON_ACCURACY) {
        fsmState = EMUHV_FSM_STATE_STANDBY;
      }
    }
  }
  
  if (fsmState == "") {
    fsmState = EMUHV_FSM_STATE_NOT_READY;
  }
  
  dpSet(fsmDp + ".fsm_state", fsmState);
}

/**
  * Updates FSM states of the given HV DP (for the chamber as well as all the channels).
  */
void emuhv_updateChamberState(string hvDp) {
  string fastMonDp = hvDp + EMUHV_DP_POSTFIX_FAST_MON;
  string slowMonDp = hvDp + EMUHV_DP_POSTFIX_SLOW_MON;
  string settingsMonDp = hvDp + EMUHV_DP_POSTFIX_SETTINGS;
  
  int alert = 0;
  dpGet(hvDp + ".:_alert_hdl.._act_state", alert);
  
  int numChannels = emu_dpGetCached(slowMonDp + ".num_chans");
  
  dyn_string channelStates = makeDynString();
  for(int i=1; i <= numChannels; i++) {
    string states = emuhv_getChannelStates(hvDp, i, alert);
    dynAppend(channelStates, states);
  }
  
  // compare with previous states
  dyn_string prevChannelStates = emu_dpGetCached(hvDp + ".channel_states");
  string prevFsmState = emu_dpGetCached(hvDp + ".fsm_state");
  if (compareArrays(prevChannelStates, channelStates) && !alert && (prevFsmState != EMUHV_FSM_STATE_ERROR)) {
    // if they're the same, then nothing to do
    // (though if there's an alert, still redo the state calc because there might be alerts coming not from channels)
    return;
  }

  // it's different from before
  dpSet(hvDp + ".channel_states", channelStates);
  
  // compute the overal chamber HV state
  string fsmState;
  if (alert) {
    fsmState = EMUHV_FSM_STATE_ERROR;
  } else {
    for (int i=1; i <= dynlen(channelStates); i++) {
      string chState = channelStates[i];
      if (strpos(chState, EMUHV_FSM_STATE_DISABLED) >= 0) {
        continue;
      }
      if (strpos(chState, EMUHV_FSM_STATE_ERROR) >= 0) {
        fsmState = EMUHV_FSM_STATE_ERROR;
        break;
      }
      if (strpos(chState, EMUHV_FSM_STATE_RAMPING) >= 0) {
        fsmState = EMUHV_FSM_STATE_RAMPING;
        break;
      }
    
      // only ON, STANDBY or OFF left - just use the same for chamber if ALL channels are the same, if not all are the same, then chamber HV is NOT-READY
      if (fsmState == "") {
        fsmState = chState;
      }
      if (fsmState != chState) {
        fsmState = EMUHV_FSM_STATE_NOT_READY;
        break;
      }
    }
    if (fsmState == "") { // all channels disabled??? :)
      fsmState = EMUHV_FSM_STATE_NOT_READY;
    }
  }
  strreplace(fsmState, ";", "");
  emu_dpSetIfDifferent(hvDp + ".fsm_state", fsmState);
}

/**
  * Get states of the given HV channel.
  * @note a channel can be in more than one state at the same time e.g. ON and ERROR
  * @return states of the given HV channel.
  */
string emuhv_getChannelStates(string hvDp, int channelNum, bool checkForAlerts = true) {
  string settingsDp = hvDp + EMUHV_DP_POSTFIX_SETTINGS + ".channels.ch" + channelNum;
  string fastMonDp = hvDp + EMUHV_DP_POSTFIX_FAST_MON + ".channels.ch" + channelNum;
  
  string ret;
  
  // check if it's disabled
  bool isDisabled = emu_dpGetCached(settingsDp + ".disabled");
  if (isDisabled) {
    ret += EMUHV_FSM_STATE_DISABLED + ";";
  }
  
  // check if it has alerts
  if (checkForAlerts || isDisabled) { // always check for alerts on disabled channels, because they don't propagate to summary alerts
    int alert = 0;
    dpGet(fastMonDp + ".status:_alert_hdl.._act_state", alert);
    if (alert > 0) {
      ret += EMUHV_FSM_STATE_ERROR + ";";
    }
  }
  
  int onVset = emu_dpGetCached(settingsDp + ".on_vset");
  int vmon, status;
  dpGet(fastMonDp + ".vmon", vmon,
        fastMonDp + ".status", status);
  
  if (status == EMUHV_STATE_ON) {
    if ((vmon >= onVset - EMUHV_STATE_ON_VMON_ACCURACY) &&
        (vmon <= onVset + EMUHV_STATE_ON_VMON_ACCURACY)) {
      ret += EMUHV_FSM_STATE_ON + ";";
    } else if ((vmon >= EMUHV_VOLTAGE_STANDBY - EMUHV_STATE_STANDBY_VMON_ACCURACY) &&
               (vmon <= EMUHV_VOLTAGE_STANDBY + EMUHV_STATE_STANDBY_VMON_ACCURACY)) {
      ret += EMUHV_FSM_STATE_STANDBY + ";";
    } else if (vmon < EMUHV_STATE_OFF_HIGHEST_VMON) {
      ret += EMUHV_FSM_STATE_OFF + ";";
    } else { // this shouldn't happen, but if it does, let's say it's ramping
      ret += EMUHV_FSM_STATE_RAMPING + ";";
    }
  } else if (status == EMUHV_STATE_OFF) {
    ret += EMUHV_FSM_STATE_OFF + ";";
  } else if ((status == EMUHV_STATE_RAMP_UP) || (status == EMUHV_STATE_RAMP_DOWN)) {
    ret += EMUHV_FSM_STATE_RAMPING + ";";
  }
  
  return ret;
}

// ******************* COMMAND HANDLING *********************

/**
  * Callback function which actually executes FSM commands.
  */
void emuhv_commandReceivedCB(string dpe, string command) {
  emu_info("HV Client: executing command " + command + " on " + dpe);
  
  dyn_string ex;
  mapping chamber = emu_fsmNodeToDeviceParams(dpe, ex);
  if (emu_checkException(ex)) { return; }

  // turn on and make sure all channels have correct settings
  if ((command == EMUHV_FSM_COMMAND_ON) || (command == EMUHV_FSM_COMMAND_STANDBY)) {
    string fastMonDp = emuhv_getDp(chamber, EMUHV_DP_POSTFIX_FAST_MON, ex);
    if (emu_checkException(ex)) { return; }
    // turn module on if it's off
//    int moduleState;
//    dpGet(fastMonDp + ".module_status", moduleState);
//    if (moduleState == EMUHV_STATE_OFF) {
      emuhv_sendCommand(chamber, EMUHV_COMMAND_SET_MOD_STATE, ex, EMUHV_STATE_ON);
      if (emu_checkException(ex)) { return; }
//    }
    
    dyn_mapping channels = emuhv_getAllChannelDevices(chamber, ex);
    dyn_string channelSettingsDps = emuhv_getAllChannelDps(chamber, EMUHV_DP_POSTFIX_SETTINGS, ex);
    dyn_string channelSlowMonDps = emuhv_getAllChannelDps(chamber, EMUHV_DP_POSTFIX_SLOW_MON, ex);
    dyn_int disabledChannels;
    if (emu_checkException(ex)) { return; }
    
    // go through all channels and ensure they have the correct settings set
    for (int i=1; i <= dynlen(channelSettingsDps); i++) {
      bool isDisabled = emu_dpGetCached(channelSettingsDps[i] + ".disabled");
      if (isDisabled) {
        dynAppend(disabledChannels, i);
      }
      
      int imax = emu_dpGetCached(channelSettingsDps[i] + ".imax");
      int imaxActual = emu_dpGetCached(channelSlowMonDps[i] + ".imax");
      if (imax != imaxActual) {
        emuhv_sendCommand(channels[i], EMUHV_COMMAND_SET_IMAX, ex, imax);
        if (emu_checkException(ex)) { return; }
      }
      
      int vset;
      if (command == EMUHV_FSM_COMMAND_STANDBY) {
        vset = EMUHV_VOLTAGE_STANDBY;
      } else {
        vset = emu_dpGetCached(channelSettingsDps[i] + ".on_vset");
      }
      emuhv_sendCommand(channels[i], EMUHV_COMMAND_SET_VSET, ex, vset);
    }
    
    // switch on all channels
    emuhv_sendCommand(chamber, EMUHV_COMMAND_SET_STATE, ex, EMUHV_STATE_ON);
      
    // if there are disabled channels - switch them off
    for(int i=1; i <= dynlen(disabledChannels); i++) {
      emuhv_sendCommand(channels[disabledChannels[i]], EMUHV_COMMAND_SET_STATE, ex, EMUHV_STATE_OFF);
    }

  } else if (command == EMUHV_FSM_COMMAND_OFF) {
    emuhv_sendCommand(chamber, EMUHV_COMMAND_SET_STATE, ex, EMUHV_STATE_OFF);
    if (emu_checkException(ex)) { return; }
  } 
  
}

/**
  * Callback function which actually executes primary PSU FSM commands.
  */
void emuhv_primaryCommandReceivedCB(string dpe, string command) {
  dyn_string ex;
  mapping primary = emuhv_getPrimaryDeviceParams(dpe, ex);
  if (emu_checkException(ex)) { return; }
  
  if (command == EMUHV_FSM_COMMAND_OFF) { // switch off
    emuhv_sendPrimaryCommand(primary, EMUHV_COMMAND_OFF, ex);
    if (emu_checkException(ex)) { return; }
    delay(0, 100);
    emuhv_requestPrimaryData(primary, ex);
    if (emu_checkException(ex)) { return; }
  } else if (command == EMUHV_FSM_COMMAND_STANDBY) { // lower the voltage
    emuhv_sendPrimaryCommand(primary, EMUHV_COMMAND_SET_VSET, ex, EMUHV_VOLTAGE_PRIMARY_STANDBY);
    if (emu_checkException(ex)) { return; }
    delay(0, 100);
    emuhv_requestPrimaryData(primary, ex);
    if (emu_checkException(ex)) { return; }
  } else {
    emu_info("HV Client: ignoring primary PSU command " + command + " on " + dpe + " (it will be handled automatically by the HV server when it gets a command for a chamber channel, only OFF and STANDBY commands are executed by DCS for primary PSUs)");
  }
}
