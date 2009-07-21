/**@file

This package contains functions for session management.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

global string emuui_g_sessionId;

/** Initializes a session (creates a new DP or registers an already existing one. */
void emuui_initSession() {
  time t0;
  emu_debugFuncStart("emuui_initSession", t0);
  
  if (strlen(emuui_g_sessionId) > 0) {
    emu_debugFuncEnd("emuui_initSession", t0);
    return;
  }
  
  string user;
  fwAccessControl_getUserName(user);
  emuui_g_sessionId = "session_" + getHostname() + "_" + user;
  strreplace(emuui_g_sessionId, " ", "_");
  
  dyn_string sessionDps = dpNames(emuui_g_sessionId, "CSC_UI_sessionState");
  // should never happen, but just in case
  if (dynlen(sessionDps) > 1) {
    emu_info("More than one saved session found for this user at this computer - cleaning up (deleting all of them and starting new)");
    for (int i=1; i <= dynlen(sessionDps); i++) {
      dpDelete(sessionDps[i]);
    }
    sessionDps = dpNames("emu_g_sessionId", "CSC_UI_sessionState");
  }
  
  // if it doesn't yet exist - create one
  if (dynlen(sessionDps) == 0) {
    dpCreate(emuui_g_sessionId, "CSC_UI_sessionState");
    emuui_applySessionDefaults(emuui_g_sessionId);
  }
  
  string test;
  dpGet(emuui_g_sessionId + ".mainChamberView.connectionType", test);
  if (test == "") {
    emuui_applySessionDefaults(emuui_g_sessionId);
  }
  
  emu_debugFuncEnd("emuui_initSession", t0);
}

/** @return a requested DPE of the current session. */
string emuui_getSessionDp(string dp) {
  return emuui_g_sessionId + "." + dp;
}

void emuui_applySessionDefaults(string sessionDp) {
  dyn_string ex;
  mapping sessDefaults = emuui_getMapping("sessionDefaults", ex);
  if (emu_checkException(ex)) { return; }
  
  for (int i=1; i <= mappinglen(sessDefaults); i++) {
    string key = mappingGetKey(sessDefaults, i);
    string setting = sessDefaults[key];
    string dpe = sessionDp + "." + key;
    
    dpSetWait(dpe, setting);
  }
  
  dpSetWait(sessionDp + ".hostname", getHostname());
}
