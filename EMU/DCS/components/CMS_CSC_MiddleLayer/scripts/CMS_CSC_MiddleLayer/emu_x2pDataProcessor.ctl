/**@file

This script processes X2P data (LV, temperature, FED) and assigns FSM state

@author Evaldas Juska (FNAL)
@date   July 2011
*/

// EMU common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_alert.ctl"
#uses "CMS_CSC_common/emu_mapping.ctl"
#uses "CMS_CSC_MiddleLayer/emu_x2p.ctl"

void main() {
  emux2p_init();
}

/** Called during the project initialization. Puts all devices into some initial state and starts the monitoring. */
public void emux2p_init() {
  emu_info("Initializing X2P data monitoring/processing...");
  dyn_string x2pDataDps = emux2p_getAllDataDps();
  
  for (int i=1; i <= dynlen(x2pDataDps); i++) {
    dpConnect("_emux2p_monitorDataStatusCB", true, x2pDataDps[i] + ".status");
  }
  emu_info("Done initializing X2P data monitoring/processing");
}

/** Processes data status updates. */
private void _emux2p_monitorDataStatusCB(string dp, int status) {
//  emu_debug("got status update for " + dp + " = " + status);
  
  dyn_string ex;
  string dataDp = dpSubStr(dp, DPSUB_DP);
  
  string fsmDp = emux2p_getFsmDpFromDataDp(dataDp, ex);
  if (emu_checkException(ex)) { return; }
  
  // status is a bitmask of different problems, 0 means data is OK.
  if ((status == 0) || (status & EMU_X2P_STATUS_ERROR_WITH_DATA)) { // good data
    // check if there is an alert - if so, set FSM DP status to -1 (error), if not then 2 (on)
    int alertState;
    dpGet(dataDp + ".:_alert_hdl.._act_state", alertState);
    if (alertState != 0) {
      dpSet(fsmDp + ".fsm_state", "ERROR"); // always set this one to let majority recalculate the number of channels in error
    } else {
      emu_dpSetWaitIfDifferent(fsmDp + ".fsm_state", "ON"); // only set it if it's different right now - will ease the load on majority which checks this DP
    }
  } else if ((status & EMU_X2P_STATUS_OFF) || (status & EMU_X2P_STATUS_CRATE_OFF)) { // chamber is OFF
    emu_dpSetWaitIfDifferent(fsmDp + ".fsm_state", "OFF");
  } else { // treating this as bad data, so the same as if there was no data at all (subject to timeout and be flaged with NO_COMMUNICATION, a.k.a. question mark)
    // if FSM DP status is ERROR or OFF, then set to ON, otherwise don't touch it
    // communication timeout procedure should check this status and if it falls into this category, then it should be treated as "no update"
    int fsmState;
    dpGet(fsmDp + ".fsm_state", fsmState);
    if ((fsmState == "ERROR") || (fsmState == "OFF")) { // because this is treated as bad communication, we shouldn't use that data to set chamber into error (or off) state
      dpSet(fsmDp + ".fsm_state", "ON");
    }
  }
}
