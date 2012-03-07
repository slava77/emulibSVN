/**@file

This manager looks at GAS data and determines gas FSM state

@author Evaldas Juska (FNAL)
@date   March 2012
*/

#uses "CMS_CSC_common/emu_common.ctl"

const string EMUGAS_DP_FSM_STATE = "CscGas/CSC_GAS_MONITOR.fsm_state";
const string EMUGAS_DP_MONITOR = "CSC_GAS_MONITOR";

const string EMUGAS_FSM_STATE_ON    = "ON";
const string EMUGAS_FSM_STATE_OFF   = "OFF";
const string EMUGAS_FSM_STATE_ERROR = "ERROR";

main() {
  emu_info("Gas monitor starting..");
  
  string statusDp = EMUGAS_DP_MONITOR + ".Status";
  string stateDp = EMUGAS_DP_MONITOR + ".State";
  dpConnect("emugas_statusUpdatedCB", true, statusDp);
  dpConnect("emugas_stateUpdatedCB", true, stateDp);
  
  emu_info("Gas monitor started.");
}

/**
  * Callback for Gas status update. Determines FSM state.
  */
void emugas_statusUpdatedCB(string dp, bool isOn) {
  emugas_updateFsmState();
}

/**
  * Callback for Gas state update. Determines FSM state.
  */
void emugas_stateUpdatedCB(string dp, int isOn) {
  emugas_updateFsmState();
}

/**
  * Determines FSM state.
  */
void emugas_updateFsmState() {
  int state;
  bool isOn;
  dpGet(EMUGAS_DP_MONITOR + ".State", state,
        EMUGAS_DP_MONITOR + ".Status", isOn);
  
  string fsm_state;
  if (state == 100) {
    fsm_state = EMUGAS_FSM_STATE_OFF;
  } else if (isOn) {
    fsm_state = EMUGAS_FSM_STATE_ON;
  } else {
    fsm_state = EMUGAS_FSM_STATE_ERROR;
  }
  
  dpSetWait(EMUGAS_DP_FSM_STATE, fsm_state);
}
