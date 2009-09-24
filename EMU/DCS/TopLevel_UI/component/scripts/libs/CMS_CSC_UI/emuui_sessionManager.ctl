/**@file

This package contains functions for session management.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

global string emuui_g_sessionId;
global dyn_string emuui_g_activeTooltips;

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
  
  // go through all the session variables and if theres any that haven't got a value - apply the default
  emuui_applySessionDefaults(emuui_g_sessionId);
  
  emu_debugFuncEnd("emuui_initSession", t0);
}

/** @return ID of the current session. */
string emuui_getSessionId() {
  return emuui_g_sessionId;
}

/** @return a requested DPE of the current session. */
string emuui_getSessionDp(string dp) {
  return emuui_getSessionId() + "." + dp;
}

/** goes through all the session variables and if theres any that haven't got a value - applies the default
  
  */
void emuui_applySessionDefaults(string sessionDp, bool forceRewrite = false) {
  dyn_string ex;
  mapping sessDefaults = emuui_getMapping("sessionDefaults", ex);
  if (emu_checkException(ex)) { return; }
  
  for (int i=1; i <= mappinglen(sessDefaults); i++) {
    string key = mappingGetKey(sessDefaults, i);
    string setting = sessDefaults[key];
    string dpe = sessionDp + "." + key;
    
    string testValue;
    dpGet(dpe, testValue);
    if ((testValue == "") || forceRewrite){
      emu_info("applying default value to session variable '" + dpe + "': " + setting);
      dpSetWait(dpe, setting);
    }
  }
  
  dpSetWait(sessionDp + ".hostname", getHostname());
}

emuui_sessionRegisterActiveTooltip(string refName) {
  dynAppend(emuui_g_activeTooltips, refName);
}

emuui_sessionKillAllActiveTooltips() {
  dyn_string refNames;
  emu_dynAppend(refNames, emuui_g_activeTooltips);
  for (int i=1; i <= dynlen(refNames); i++) {
    int rc = removeSymbol(myModuleName(), myPanelName(), refNames[i]);
  }  
}

bool emuui_isTooltipActive(string refName) {
  return dynContains(emuui_g_activeTooltips, refName);
}

emuui_tooltipClosed(string refName) {
  int index = dynContains(emuui_g_activeTooltips, refName);
  if (index > 0) {
    dynRemove(emuui_g_activeTooltips, index);
  }
}
