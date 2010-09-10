//global int genSignal
global mapping useV1;
global dyn_string channelsDps;

main() {
  useV1["P"] = false;
  useV1["M"] = false;
  
  DebugTN("Connecting to crate DPs...");
  dyn_string crateDps = dpNames("*", "FwCaenCrateSY1527");
  for (int i=1; i <= dynlen(crateDps); i++) {
    dpConnect("crateGenSignalChangedCB", true, crateDps[i] + ".Information.GenSignCfg");
  }
  DebugTN("DONE connecting to crate DPs");
  DebugTN("Connecting to channel DPs...");
  channelsDps = dpNames("*", "FwCaenChannel");
  for (int i=1; i <= dynlen(channelsDps); i++) {
    dpConnect("channelOnOffSettingChangedCB", true, channelsDps[i] + ".settings.onOff");
    dpConnect("channelVoltageSettingChangedCB", true, channelsDps[i] + ".settings.v0", 
                                                      channelsDps[i] + ".settings.v1");
  }
  DebugTN("DONE connecting to channel DPs");
}

void channelOnOffSettingChangedCB(string chOnOffDpName, bool onOff) {
  string chDpName = dpSubStr(chOnOffDpName, DPSUB_SYS_DP);
  dpSet(chDpName + ".readBackSettings.onOff", onOff,
        chDpName + ".actual.status", onOff);
  
  float voltage = 0;
  if (onOff) {
    string side = "P";
    if (strpos(chDpName, "HVME11N") >= 0) { side = "N"; }
    if (useV1[side]) {
      dpGet(chDpName + ".readBackSettings.v1", voltage);
    } else {
      dpGet(chDpName + ".readBackSettings.v0", voltage);
    }
  }
  dpSet(chDpName + ".actual.vMon", voltage);
}

void channelVoltageSettingChangedCB(dyn_string v0v1Dps, dyn_float v0v1) {
  string chDpName = dpSubStr(v0v1Dps[1], DPSUB_SYS_DP);
  dpSet(chDpName + ".readBackSettings.v0", v0v1[1],
        chDpName + ".readBackSettings.v1", v0v1[2]);
  
  // trigger onOff listener to set the right voltage to actual vMon
  bool onOff;
  dpGet(chDpName + ".settings.onOff", onOff);
  dpSet(chDpName + ".settings.onOff", onOff);
}

void crateGenSignalChangedCB(string dp, int genOutput) {
  string side = "P";
  if (strpos(dp, "HVME11N") >= 0) { side = "N"; }
  if (genOutput == 771) {
    useV1[side] = true;
    touchChannelsOnOff(side);
  } else if (genOutput == 1) {
    useV1[side] = false;
    touchChannelsOnOff(side);
  }
}

/** calls channelOnOffSettingChangedCB with the same onOff value as found in ".actual.status" just to trigger a status update (e.g. when crate generator output has changed). */
void touchChannelsOnOff(string side) {
  for (int i=1; i <= dynlen(channelsDps); i++) {
    if (strpos(channelsDps[i], "HVME11" + side) >= 0) {
      bool onOff;
      dpGet(channelsDps[i] + ".actual.status", onOff);
      channelOnOffSettingChangedCB(channelsDps[i] + ".settings.onOff", onOff);
    }
  }
}
