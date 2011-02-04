/**@file

This package contains everything for X2P data (LV, temperature, FED) processing (e.g. interpreting status flags, activating and deactivating alerts, seting FSM state DP, procedures during the project startup).
Everything that concerns X2P data is here and nowhere else.

@author Evaldas Juska (PH/UCM)
@date   July 2010
*/

// EMU common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_alert.ctl"
#uses "CMS_CSC_common/emu_mapping.ctl"

// HV_CC common libs
#uses "CMS_CSCfw_HV_CC/emuDcsDim.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs2.ctl"

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
public int EMU_X2P_STATUS_BAD_DATA = EMU_X2P_STATUS_OTHER_PROBLEM |
                                     EMU_X2P_STATUS_CORRUPTED |
                                     EMU_X2P_STATUS_VCC_NOT_ACCESSIBLE |
                                     EMU_X2P_STATUS_DMB_READING_ERROR |
                                     EMU_X2P_STATUS_TMB_READING_ERROR;

public int EMU_X2P_STATUS_ERROR_WITH_DATA = EMU_X2P_STATUS_CHAMBER_HAS_NO_ANALOG_POWER |
                                            EMU_X2P_STATUS_CHAMBER_HAS_NO_DIGITAL_POWER;

void main() {
  mudcsInit();
  emux2p_init();
}

/** Called during the project initialization. Puts all devices into some initial state and starts the monitoring. */
public void emux2p_init() {
  emu_info("Initializing X2P data monitoring/processing...");
  dyn_string x2pDataDps = emux2p_getAllDataDps();
  
  for (int i=1; i <= dynlen(x2pDataDps); i++) {
    dpConnect("_emux2p_monitorDataStatusCB", false, x2pDataDps[i] + ".status");
  }
  emu_info("Done initializing X2P data monitoring/processing");
}

/**
  * @return all X2P data DPs (LV_1_d, TEMP_1_d, FED_1_d)
  */
public dyn_string emux2p_getAllDataDps() {
  dyn_string x2pDataDps = makeDynString();
  emu_dynAppend(x2pDataDps, emux2p_getLvDataDps());
  emu_dynAppend(x2pDataDps, emux2p_getTemperatureDataDps());
  emu_dynAppend(x2pDataDps, emux2p_getFedDataDps());
  
  return x2pDataDps;
}

/**
  * @return LV data DPs (DPT: LV_1_d)
  */
public dyn_string emux2p_getLvDataDps() {
  return dpNames("CSC_ME*", "LV_1_d");
}

/**
  * @return On-Chamber electronics temperature data DPs (DPT: TEMP_1_d)
  */
public dyn_string emux2p_getTemperatureDataDps() {
  return dpNames("CSC_ME*", "TEMP_1_d");
}

/**
  * @return FED (DDU) data DPs (DPT: FED_1_d)
  */
public dyn_string emux2p_getFedDataDps() {
  return dpNames("DDU*", "FED_1_d");
}

/** Processes data status updates. */
private void _emux2p_monitorDataStatusCB(string dp, int status) {
//  emu_debug("got status update for " + dp + " = " + status);
  
  dyn_string ex;
  string dataDp = dpSubStr(dp, DPSUB_DP);
  string fsmDp = mudcsConvertData2FsmOneNoType(dataDp, ex);
  if (emu_checkException(ex)) { return; }
  
  // status is a bitmask of different problems, 0 means data is OK.
  if ((status == 0) || (status & EMU_X2P_STATUS_ERROR_WITH_DATA)) { // good data
    // check if there is an alert - if so, set FSM DP status to -1 (error), if not then 2 (on)
    int alertState;
    dpGet(dataDp + ".:_alert_hdl.._act_state", alertState);
    if (alertState != 0) {
      dpSet(fsmDp + ".status", -1); // always set this one to let majority recalculate the number of channels in error
    } else {
      emu_dpSetWaitIfDifferent(fsmDp + ".status", 2); // only set it if it's different right now - will ease the load on majority which checks this DP
    }
  } else if ((status & EMU_X2P_STATUS_OFF) || (status & EMU_X2P_STATUS_CRATE_OFF)) { // chamber is OFF
    // set FSM status = 0 (off)
    emu_dpSetWaitIfDifferent(fsmDp + ".status", 0);
  } else { // treating this as bad data, so the same as if there was no data at all (subject to timeout and be flaged with NO_COMMUNICATION, a.k.a. question mark)
    // if FSM DP status is -1 (error) or 0 (off), then set to 2 (on), otherwise don't touch it
    // new communication timeout procedure should check this status and if it falls into this category, then it should be treated as "no update"
    int fsmStatus;
    dpGet(fsmDp + ".status", fsmStatus);
    if ((fsmStatus == -1) || (fsmStatus == 0)) { // because this is treated as bad communication - we shouldn't use that data to set chamber into error (or off) state
      dpSet(fsmDp + ".status", 2);
    }
  }
}
