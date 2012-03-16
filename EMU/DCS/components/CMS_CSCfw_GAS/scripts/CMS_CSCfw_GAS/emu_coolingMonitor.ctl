/**@file

This manager looks at cooling data and determines cooling FSM states

@author Evaldas Juska (FNAL)
@date   March 2012
*/

#uses "CMS_CSC_common/emu_common.ctl"

const string EMUCOOL_FSM_STATE_ON = "ON";
const string EMUCOOL_FSM_STATE_ERROR = "ERROR";

main() {
  emu_info("Cooling monitor starting..");
  
  dyn_string dps = dpNames("*", "CscCooling");
  for (int i=1; i <= dynlen(dps); i++) {
    dpConnect("emucool_alertUpdatedCB", true, dps[i] + ".:_alert_hdl.._act_state");
  }
  
  emu_info("Cooling monitor started.");
}

void emucool_alertUpdatedCB(string dp, int alert) {
  string state = (alert) ? EMUCOOL_FSM_STATE_ERROR : EMUCOOL_FSM_STATE_ON;
  dpSetWait(dpSubStr(dp, DPSUB_SYS_DP) + ".fsm_state", state);
}
