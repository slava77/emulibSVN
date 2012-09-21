/**@file

This package contains everything for X2P data (LV, temperature, FED) processing (e.g. interpreting status flags, activating and deactivating alerts, seting FSM state DP, procedures during the project startup).

@author Evaldas Juska (FNAL)
@date   July 2010
*/

// EMU common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_alert.ctl"
#uses "CMS_CSC_common/emu_mapping.ctl"

// ============ status bit pattern ====================================================
// bit 0  (value   1): misc. errors
// bit 1  (value   2): chamber power off from Configuration DB
// bit 2  (value   4): data corrupted (in infospace or during transmission)
// bit 3  (value   8): VCC not accessible
// bit 4  (value  16): DMB reading error
// bit 5  (value  32): crate OFF
// bit 6  (value  64): this DMB module caused VCC reading trouble
// bit 7  (value 128): TMB reading error
// bit 8  (value 256): this TMB module caused VCC reading trouble
// bit 9  (value 512): chamber lost Analog power
// bit 10 (val  1024): chamber lost Digital power
// =====================================================================================
public const int EMU_X2P_STATUS_OTHER_PROBLEM = 0x1; // reserved for an undocumented problem
public const int EMU_X2P_STATUS_OFF = 0x2; // chamber power off from Configuration DB
public const int EMU_X2P_STATUS_CORRUPTED = 0x4; // data corrupted (in infospace or during transmission)
public const int EMU_X2P_STATUS_VCC_NOT_ACCESSIBLE = 0x8; // VME Crate Controller problem (not accessible)
public const int EMU_X2P_STATUS_DMB_READING_ERROR = 0x10; // DMB reading error
public const int EMU_X2P_STATUS_CRATE_OFF = 0x20; // the PCrate is OFF
public const int EMU_X2P_STATUS_DMB_CAUSED_VCC_ERROR = 0x40; // this DMB module caused VCC reading trouble
public const int EMU_X2P_STATUS_TMB_READING_ERROR = 0x80; // TMB reading error
public const int EMU_X2P_STATUS_TMB_CAUSED_VCC_ERROR = 0x100; // this TMB module caused VCC reading trouble
public const int EMU_X2P_STATUS_CHAMBER_HAS_NO_ANALOG_POWER = 0x200; // chamber lost Analog power
public const int EMU_X2P_STATUS_CHAMBER_HAS_NO_DIGITAL_POWER = 0x400; // chamber lost Digital power

// if any bit in this pattern is set then data should be ignored (and subject to timeout)
public const int EMU_X2P_STATUS_BAD_DATA = 0x9D; /* EMU_X2P_STATUS_OTHER_PROBLEM |
                                                    EMU_X2P_STATUS_CORRUPTED |
                                                    EMU_X2P_STATUS_VCC_NOT_ACCESSIBLE |
                                                    EMU_X2P_STATUS_DMB_READING_ERROR |
                                                    EMU_X2P_STATUS_TMB_READING_ERROR; */

public const int EMU_X2P_STATUS_ERROR_WITH_DATA = 0x600; /*EMU_X2P_STATUS_CHAMBER_HAS_NO_ANALOG_POWER |
                                                           EMU_X2P_STATUS_CHAMBER_HAS_NO_DIGITAL_POWER; */

// ============ status bit pattern ====================================================
// bit 0  (value   1):   ALCT not configured
// bit 1  (value   2):   TMB not configured
// bit 2  (value   4):   DMB / CFEB not configured (always 0 for ME1/3 chambers)
// bit 7  (value   128): CCB not configured
// =====================================================================================
public const int EMU_X2P_CCB_BITS_ALCT = 1; // ALCT not configured
public const int EMU_X2P_CCB_BITS_TMB  = 2; // TMB not configured
public const int EMU_X2P_CCB_BITS_DMB_CFEB = 4; // DMB / CFEB not configured
public const int EMU_X2P_CCB_BITS_CCB = 128; // CCB itself is not configured

public const string EMU_X2P_COMMAND_STOP_MONITORING = "STOP_SLOW_CONTROL";
public const string EMU_X2P_COMMAND_START_MONITORING = "RESUME_SLOW_CONTROL";
public const string EMU_X2P_COMMAND_PREPARE_POWER_UP = "PREPARE_POWER_UP";

public const string EMU_X2P_DEVICE_PREFIX_LV = "CscLowVoltage/";
public const string EMU_X2P_DEVICE_PREFIX_TEMP = "CscTemperature/";
public const string EMU_X2P_DEVICE_PREFIX_FED = "CscFed/";

public const string EMU_X2P_DEVICE_MON_DPT_LV = "CscLvChamberMon";
public const string EMU_X2P_DEVICE_MON_DPT_TEMP = "CscTempChamberMon";
public const string EMU_X2P_DEVICE_MON_DPT_FED = "CscFedDduMon";

public const string EMU_X2P_DEVICE_FSM_DPT_LV = "CscLvChamber";
public const string EMU_X2P_DEVICE_FSM_DPT_TEMP = "CscTempChamber";
public const string EMU_X2P_DEVICE_FSM_DPT_FED = "CscFedDdu";

public const string EMU_X2P_FSM_STATE_ON        = "ON";
public const string EMU_X2P_FSM_STATE_OFF       = "OFF";
public const string EMU_X2P_FSM_STATE_ERROR     = "ERROR";
public const string EMU_X2P_FSM_STATE_NOT_READY = "NOT-READY";
public const string EMU_X2P_FSM_STATE_DISABLED  = "DISABLED";

/**
  * @return all X2P data DPs (LV, temperature and FED)
  */
public dyn_string emux2p_getAllDataDps() {
  dyn_string x2pDataDps = makeDynString();
  emu_dynAppend(x2pDataDps, emux2p_getLvDataDps());
  emu_dynAppend(x2pDataDps, emux2p_getTemperatureDataDps());
  emu_dynAppend(x2pDataDps, emux2p_getFedDataDps());
  
  return x2pDataDps;
}

/**
  * @return LV data DPs (DPT: CscLvChamberMon)
  */
public dyn_string emux2p_getLvDataDps() {
  return dpNames("*CSC_ME*", EMU_X2P_DEVICE_MON_DPT_LV);
}

/**
  * @return On-Chamber electronics temperature data DPs (DPT: CscTempChamberMon)
  */
public dyn_string emux2p_getTemperatureDataDps() {
  return dpNames("*CSC_ME*", EMU_X2P_DEVICE_MON_DPT_TEMP);
}

/**
  * @return FED (DDU) data DPs (DPT: CscFedDduMon)
  */
public dyn_string emux2p_getFedDataDps() {
  return dpNames("*DDU*", EMU_X2P_DEVICE_MON_DPT_FED);
}

/**
  * @return FSM dp given the data DP (e.g. converts from CscLvChamberMon to CscLvChamber).
  */
public string emux2p_getFsmDpFromDataDp(string dp, dyn_string &ex) {
  if (strpos(dp, "/Mon") < 0) {
    emu_addError("Invalid data DP: " + dp, ex);
    return "";
  }
  
  dp = dpSubStr(dp, DPSUB_SYS_DP);
  strreplace(dp, "/Mon", "");
  return dp;
}

/**
  * @return Low Voltage FSM dp, given chamber device parameters (must inclue "side", "station", "ring" and "chamberNumber" parameters)
  */
string emux2p_getLvDpName(mapping chamber, dyn_string &exceptionInfo) {
  string ret = EMU_X2P_DEVICE_PREFIX_LV + emu_chamberDeviceParamsToFsmName(chamber, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return ""; }
  return ret;
}

/**
  * @return Temperature FSM dp, given chamber device parameters (must inclue "side", "station", "ring" and "chamberNumber" parameters)
  */
string emux2p_getTempDpName(mapping chamber, dyn_string &exceptionInfo) {
  string ret = EMU_X2P_DEVICE_PREFIX_TEMP + emu_chamberDeviceParamsToFsmName(chamber, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return ""; }
  return ret;
}

/**
  * @return DDU FSM dp, given DDU device parameters (must inclue "id" parameter)
  */
string emux2p_getDduDpName(mapping ddu, dyn_string &exceptionInfo) {
  emuDev_checkDeviceParams("emux2p_getDduDpName", "DDU", ddu, makeDynString("id"), exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return ""; }
  
  string dduDp;
  sprintf(dduDp, EMU_X2P_DEVICE_PREFIX_FED + "DDU%.2d", ddu["id"]);
  return dduDp;
}

/** 
  * Enables / disables a given channel.
  * @param channelDeviceParams device params of a chamber hv channel - has to include side, station, ring, chamberNumber, channelNumber
  * @param isEnable if this is true then the channel is included, if false excluded
  */
public void emux2p_enableDisableChannel(string dp, bool isEnable, dyn_string &exceptionInfo) {
  if (!dpExists(dp)) {
    emu_addError("Not existing DP passed to emux2p_enableDisableChannel()", exceptionInfo);
  }

  string fsmDp = emux2p_getFsmDpFromDataDp(dp, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  dyn_string disabledDps;
  dpGet(fsmDp + ".disabled_channels", disabledDps);
  
  int idx = dynContains(disabledDps, dpSubStr(dp, DPSUB_SYS_DP_EL));
  if (isEnable && (idx > 0)) {
    emu_info("Enabling " + dpSubStr(dp, DPSUB_SYS_DP_EL));
    fwAlertConfig_activate(dp, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
    dynRemove(disabledDps, idx);
    dpSet(fsmDp + ".disabled_channels", disabledDps);
  } else if (!isEnable && (idx <= 0)) {
    emu_info("Disabling " + dpSubStr(dp, DPSUB_SYS_DP_EL));
    fwAlertConfig_deactivate(dp, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
    dynAppend(disabledDps, dpSubStr(dp, DPSUB_SYS_DP_EL));
    dpSet(fsmDp + ".disabled_channels", disabledDps);
  }
}
