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

public const int EMU_X2P_STATUS_OFF = 0x2; // chamber is OFF
public const int EMU_X2P_STATUS_CORRUPTED = 0x4; // corrupted data (probably while sending from Xmas to X2P)
public const int EMU_X2P_STATUS_VCC_PROBLEM = 0x8; // VME Crate Controller problem
public const int EMU_X2P_STATUS_READOUT_PROBLEM = 0x10; // board readout problem
public const int EMU_X2P_STATUS_CRATE_OFF = 0x20; // the PCrate is OFF
public const int EMU_X2P_STATUS_OTHER_PROBLEM = 0x1; // reserved for an undocumented problem

// if any bit in this pattern is set then data should be ignored (and subject to timeout)
public int EMU_X2P_STATUS_BAD_DATA = EMU_X2P_STATUS_CORRUPTED | EMU_X2P_STATUS_VCC_PROBLEM | EMU_X2P_STATUS_READOUT_PROBLEM | EMU_X2P_STATUS_OTHER_PROBLEM;

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
  if (status == 0) { // good data
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
