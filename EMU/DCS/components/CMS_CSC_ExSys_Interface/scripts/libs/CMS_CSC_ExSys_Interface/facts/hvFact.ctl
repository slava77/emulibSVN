#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysUtil.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysInterface.ctl"

/**@file

HV Fact handling

@author Evaldas Juska (PH/UCM)
@date   February 2011
*/

/**
  * Starts sending HV facts
  */
public void exsys_startHvFactDelivery(dyn_string &ex) {
  emu_info("starting HV facts delivery service...");
  
  dyn_string channelDisabledDps = dpNames("*:CscHighVoltage/CSC_ME_*/Settings.channels.ch*.disabled", "CscHvChamberSettings");
  dyn_string channelOnVsetDps = dpNames("*:CscHighVoltage/CSC_ME_*/Settings.channels.ch*.on_vset", "CscHvChamberSettings");
  dyn_string me11ChannelOnVsetDps = dpNames("*:CAEN/HVME11*/board*/channel*.userDefined", "FwCaenChannel");
  
  dyn_bool channelDisabledValues = emu_dpGetMany(channelDisabledDps);
  dyn_int channelOnVsetValues = emu_dpGetMany(channelOnVsetDps);
  
  // send out initial values
  emu_info("sending out initial HV facts...");
  for (int i=1; i <= dynlen(channelDisabledDps); i++) {
    exsys_sendDisabledChannelsFacts(channelDisabledDps[i], channelDisabledValues[i]);
  }
  for (int i=1; i <= dynlen(channelOnVsetDps); i++) {
    exsys_sendChannelVsetFacts(channelOnVsetDps[i], channelOnVsetValues[i]);
  }
  for (int i=1; i <= dynlen(me11ChannelOnVsetDps); i++) {
    string voltageStr;
    dpGet(me11ChannelOnVsetDps[i], voltageStr);
    int voltage = voltageStr;
    exsys_sendMe11ChannelVsetFact(me11ChannelOnVsetDps[i], voltage);
  }
  for (int i=1; i <= dynlen(me11ChannelOnVsetDps); i++) {
    string channelDp = dpSubStr(me11ChannelOnVsetDps[i], DPSUB_DP);
    int enabled;
    fwUi_getEnabled("CSC_ME_11_HV", channelDp, enabled);
    exsys_updateMe11HVChannelEnabledCB(dpSubStr(me11ChannelOnVsetDps[i], DPSUB_SYS_DP), enabled);
  }
  
  // connect
  for (int i=1; i <= dynlen(channelDisabledDps); i++) {
    dpConnect("exsys_updateDisabledChannelsCB", false, channelDisabledDps[i]);
  }
  for (int i=1; i <= dynlen(channelOnVsetDps); i++) {
    dpConnect("exsys_updateHVChannelVsetsCB", false, channelOnVsetDps[i]);
  }
  for (int i=1; i <= dynlen(me11ChannelOnVsetDps); i++) {
    dpConnect("exsys_updateMe11HVChannelVsetCB", false, me11ChannelOnVsetDps[i]);
  }
  for (int i=1; i <= dynlen(me11ChannelOnVsetDps); i++) {
    string channelDp = dpSubStr(me11ChannelOnVsetDps[i], DPSUB_DP);
    if (fwFsm_isDU("CSC_ME_11_HV", channelDp)) {  // make sure the node exists (we do have spare channels which are not in FSM!!!)
      fwUi_connectEnabled("exsys_updateMe11HVChannelEnabledCB", "CSC_ME_11_HV", channelDp);
    }
  }
  
  emu_info("HV facts delivery service started up successfully!");
}

public void exsys_updateDisabledChannelsCB(string dp, bool isDisabled) {
  exsys_sendDisabledChannelsFacts(dp, isDisabled);
}

public void exsys_updateHVChannelVsetsCB(string dp, int voltage) {
  exsys_sendChannelVsetFacts(dp, voltage);
}

public void exsys_updateMe11HVChannelVsetCB(string dp, string value) {
  int voltage = value;
  exsys_sendMe11ChannelVsetFact(dp, voltage);
}

public void exsys_updateMe11HVChannelEnabledCB(string dp, int enabled) {
  bool isDisabled = (enabled < 1);
  dyn_string split = strsplit(dpSubStr(dp, DPSUB_DP), "|");
  string channelDp = dpSubStr(dp, DPSUB_SYS) + split[dynlen(split)];
  exsys_sendMe11ChannelDisabledFact(channelDp, isDisabled);
}

/**
  * Given a CscHvChamberSettings.channels.ch*.disabled DPE and it's value, this function sends out facts about 
  * that channel being disabled or not.
  */
public void exsys_sendDisabledChannelsFacts(string dpe, bool isDisabled) {
  dyn_string ex;
  time t = getCurrentTime();
  strreplace(dpe, "/Settings", "/FastMon"); // expert system only knows about fast mon DPs
  strreplace(dpe, ".disabled", ""); // leave only the channel part, no DPE (it doesn't exist in fastMon)
  dyn_string paramNames = makeDynString("isDisabled");
  dyn_anytype paramValues = makeDynAnytype(isDisabled);
  string severity = EXSYS_FACT_SEVERITY_INFO;
  if (isDisabled) {
    severity = EXSYS_FACT_SEVERITY_WARN;
  }
  exsys_sendFact("DcsDisableEnableFact", t, dpe, true, severity,
                 "HV channel disabled/enabled fact", paramNames, paramValues, ex);
  emu_checkException(ex);
}

/**
  * Given a CscHvChamberSettings.channels.ch*.on_vset DPE and it's value, this function sends out facts about nominal voltage of that channel.
  */
public void exsys_sendChannelVsetFacts(string onChVsetDpe, int voltage) {
  dyn_string ex;
  time t = getCurrentTime();
  strreplace(onChVsetDpe, "/Settings", "/FastMon"); // expert system only knows about fast mon DPs
  strreplace(onChVsetDpe, ".on_vset", ""); // leave only the channel part, no DPE (it doesn't exist in fastMon)
  dyn_string paramNames = makeDynString("voltageSetting");
  dyn_anytype paramValues = makeDynAnytype(voltage);
  string severity = EXSYS_FACT_SEVERITY_INFO;
  if (voltage < 3600) {
    severity = EXSYS_FACT_SEVERITY_WARN;
  }
  exsys_sendFact("DcsHVOnVoltageFact", t, onChVsetDpe, true, severity,
                 "HV channel VSet fact", paramNames, paramValues, ex);
  emu_checkException(ex);
}

public void exsys_sendMe11ChannelVsetFact(string onChVsetDp, int voltage) {
  dyn_string ex;
  time t = getCurrentTime();
  dyn_string paramNames = makeDynString("voltageSetting");
  dyn_anytype paramValues = makeDynAnytype(voltage);
  string severity = EXSYS_FACT_SEVERITY_INFO;
  if (voltage < 2900) {
    severity = EXSYS_FACT_SEVERITY_WARN;
  }
  exsys_sendFact("DcsHVOnVoltageFact", t, onChVsetDp, true, severity,
                 "HV channel VSet fact", paramNames, paramValues, ex);
  emu_checkException(ex);
}

/** 
  * Given a channel DP and a flag saying if it's disabled or not, this function sends out a DcsDisableEnableFact
  */
public void exsys_sendMe11ChannelDisabledFact(string channelDp, bool isDisabled) {
  dyn_string ex;
  time t = getCurrentTime();
  dyn_string paramNames = makeDynString("isDisabled");
  dyn_anytype paramValues = makeDynAnytype(isDisabled);
  string severity = EXSYS_FACT_SEVERITY_INFO;
  if (isDisabled) {
    severity = EXSYS_FACT_SEVERITY_WARN;
  }
  exsys_sendFact("DcsDisableEnableFact", t, channelDp, true, severity,
                 "HV channel disabled/enabled fact", paramNames, paramValues, ex);
  emu_checkException(ex);
}
