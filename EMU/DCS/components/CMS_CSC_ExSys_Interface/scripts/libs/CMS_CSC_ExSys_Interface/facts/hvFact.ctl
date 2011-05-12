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
  
  dyn_string disabledChannelsDps = dpNames("*:HighVoltage/CSC_ME_*_HV.off_channels", "HV_1");
  dyn_string channelVsetDps = dpNames("*:HighVoltage/CSC_ME_*_HV.on_ch_vsets", "HV_1");
  dyn_string me11ChannelVsetDps = dpNames("*:CAEN/HVME11*/board*/channel*.userDefined", "FwCaenChannel");
  
  // send out initial values
  emu_info("sending out initial HV facts...");
  for (int i=1; i <= dynlen(disabledChannelsDps); i++) {
    exsys_sendDisabledChannelsFacts(disabledChannelsDps[i]);
  }
  for (int i=1; i <= dynlen(channelVsetDps); i++) {
    exsys_sendChannelVsetFacts(channelVsetDps[i]);
  }
  for (int i=1; i <= dynlen(me11ChannelVsetDps); i++) {
    string voltageStr;
    dpGet(me11ChannelVsetDps[i], voltageStr);
    int voltage = voltageStr;
    exsys_sendMe11ChannelVsetFact(me11ChannelVsetDps[i], voltage);
  }
  for (int i=1; i <= dynlen(me11ChannelVsetDps); i++) {
    string channelDp = dpSubStr(me11ChannelVsetDps[i], DPSUB_DP);
    int enabled;
    fwUi_getEnabled("CSC_ME_11_HV", channelDp, enabled);
    exsys_updateMe11HVChannelEnabledCB(dpSubStr(me11ChannelVsetDps[i], DPSUB_SYS_DP), enabled);
  }
  
  // connect
  for (int i=1; i <= dynlen(disabledChannelsDps); i++) {
    dpConnect("exsys_updateDisabledChannelsCB", false, disabledChannelsDps[i]);
  }
  for (int i=1; i <= dynlen(channelVsetDps); i++) {
    dpConnect("exsys_updateHVChannelVsetsCB", false, channelVsetDps[i]);
  }
  for (int i=1; i <= dynlen(me11ChannelVsetDps); i++) {
    dpConnect("exsys_updateMe11HVChannelVsetCB", false, me11ChannelVsetDps[i]);
  }
  for (int i=1; i <= dynlen(me11ChannelVsetDps); i++) {
    string channelDp = dpSubStr(me11ChannelVsetDps[i], DPSUB_DP);
    fwUi_connectEnabled("exsys_updateMe11HVChannelEnabledCB", "CSC_ME_11_HV", channelDp);
  }
  
  emu_info("HV facts delivery service started up successfully!");
}

public void exsys_updateDisabledChannelsCB(string dp, dyn_string value) {
  exsys_sendDisabledChannelsFacts(dp);
}

public void exsys_updateHVChannelVsetsCB(string dp, dyn_string value) {
  exsys_sendChannelVsetFacts(dp);
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

/** Given a HighVoltage/CSC_ME_*_HV.off_channels DP, this function sends out facts about all disabled and
  * not disabled channels on the chamber that this DP belongs to.
  */
public void exsys_sendDisabledChannelsFacts(string offChannelsDp) {
  dyn_int offChannels;
  dpGet(offChannelsDp, offChannels);
  
  dyn_string ex;
  mapping chamber = emu_fsmNodeToDeviceParams(offChannelsDp, ex);
  if (emu_checkException(ex)) { return; }
  
  dyn_string channelDps;
  int channelOffset;
  emuui_chamberGetHvChannelDps(chamber, channelDps, channelOffset, ex);

  time t = getCurrentTime();  
  for (int i=1; i <= dynlen(channelDps); i++) {
    bool isDisabled = dynContains(offChannels, i + channelOffset);
    dyn_string paramNames = makeDynString("isDisabled");
    dyn_anytype paramValues = makeDynAnytype(isDisabled);
    exsys_sendFact("DcsDisableEnableFact", t, channelDps[i], true, EXSYS_FACT_SEVERITY_WARN,
                   "HV channel disabled/enabled fact", paramNames, paramValues, ex);
    emu_checkException(ex);
  }
}

/** Given a HighVoltage/CSC_ME_*_HV.ch_on_vsets DP, this function sends out facts about all individual channel Vsets */
public void exsys_sendChannelVsetFacts(string onChVsetsDp) {
  dyn_int onChVsets;
  dpGet(onChVsetsDp, onChVsets);
  
  dyn_string ex;
  mapping chamber = emu_fsmNodeToDeviceParams(onChVsetsDp, ex);
  if (emu_checkException(ex)) { return; }
  
  dyn_string channelDps;
  int channelOffset;
  emuui_chamberGetHvChannelDps(chamber, channelDps, channelOffset, ex);

  time t = getCurrentTime();
  for (int i=1; i <= dynlen(channelDps); i++) {
    dyn_string paramNames = makeDynString("voltageSetting");
    dyn_anytype paramValues = makeDynAnytype(onChVsets[i]);
    exsys_sendFact("DcsHVOnVoltageFact", t, channelDps[i], true, EXSYS_FACT_SEVERITY_WARN,
                   "HV channel VSet fact", paramNames, paramValues, ex);
    emu_checkException(ex);
  }
}

public void exsys_sendMe11ChannelVsetFact(string onChVsetDp, int voltage) {
  dyn_string ex;
  time t = getCurrentTime();
  dyn_string paramNames = makeDynString("voltageSetting");
  dyn_anytype paramValues = makeDynAnytype(voltage);
  exsys_sendFact("DcsHVOnVoltageFact", t, onChVsetDp, true, EXSYS_FACT_SEVERITY_WARN,
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
  exsys_sendFact("DcsDisableEnableFact", t, channelDp, true, EXSYS_FACT_SEVERITY_WARN,
                 "HV channel disabled/enabled fact", paramNames, paramValues, ex);
  emu_checkException(ex);
}
