/**@file

This package contains general purpose functions for handling alerts.

@author Evaldas Juska (PH/UCM)
@date   May 2010
*/

/**
 * Get alert class descriptor DP for a given DP type. This descriptor DP carries information like alert class, alert texts, etc.
 * @param dpType datapoint type that you want to get the alert class descriptor for (if the class in concern is not for a summary alert, then you can specify dpe like that: <dpType>[.dpe])
 */
string emuAlert_getAlertClassDescriptorDp(string dpType) {
  strreplace(dpType, ".", "_dot_");
  string alertClassDp = "CscAlertClass_" + dpType;
  if (!dpExists(alertClassDp)) {
    return "";
  }
  return alertClassDp;
}

/** 
 * Get alert class for a given datapoint type.
 * @param dpType datapoint type that you want to get the alert class for (if the class in concern is not for a summary alert, then you can specify dpe like that: <dpType>[.dpe])
 * @return alert class for a given datapoint type.
 */
string emuAlert_getClass(string dpType) {
  string alertClass;
  string alertClassDescDp = emuAlert_getAlertClassDescriptorDp(dpType);
  dpGet(alertClassDescDp + ".class", alertClass);
  return alertClass;
}

/** 
 * Get alert texts for a given datapoint type.
 * @param dpType datapoint type that you want to get the alert texts for (if the class in concern is not for a summary alert, then you can specify dpe like that: <dpType>[.dpe])
 * @return alert texts for a given datapoint type.
 */
dyn_string emuAlert_getTexts(string dpType) {
  string alertClassDescDp = emuAlert_getAlertClassDescriptorDp(dpType);
  dyn_string texts;
  dpGet(alertClassDescDp + ".alertTexts", texts);
  return texts;
}

/**
 * Sets the appropriate alert class, texts and other class related configuration (e.g. notification) for the given DPE.
 * @param dpe datapoint element whose alarm class should be set.
 * @param dpType (optional) type of the datapoint element passed - this one is only for performance reasons - if it is passed then DP type lookup is not performed.
 * @throws IllegalArgumentException if the DPE doesn't exits.
 */
void emuAlert_updateAlertClass(string dpe, string dpType = "") {
  if (!dpExists(dpe)) {
    emu_error("IllegalArgumentException: Non-existing DP element passed to emuAlert_updateAlertClass: " + dpe);
    return;
  }
  if (dpType == "") {
    dpType = dpTypeName(dpe);
  }
  
  // set class
  string alertClass = emuAlert_getClass(dpType);
  if (strpos(dpe, ".") < 0) { dpe += "."; }
  emu_debug("Setting alert class of " + dpe + " to " + alertClass, emu_DEBUG_DETAIL);
  dpSet(dpe + ":_alert_hdl.._class", alertClass);
  
  // set texts
  int type;
  dpGet(dpe + ":_alert_hdl.._type", type);
  if (type == DPCONFIG_SUM_ALERT) {
    dyn_string alertTexts = emuAlert_getTexts(dpType);
    if (dynlen(alertTexts) < 2) {
      emu_error("Alert texts (at least 2) must be provided for alert class descriptor");
      return;
    }
    bool isActive;
    dpGet(dpe + ":_alert_hdl.._active", isActive);
    if (isActive) {
      dyn_string ex;
      fwAlertConfig_deactivate(dpe, ex);
      if (emu_checkException(ex)) { return; }
    }
    dpSet(dpe + ":_alert_hdl.._text0", alertTexts[1]);
    dpSet(dpe + ":_alert_hdl.._text1", alertTexts[2]);
    if (isActive) {
      dyn_string ex;
      fwAlertConfig_activate(dpe, ex);
      if (emu_checkException(ex)) { return; }
    }
  } else {
    emu_debug("Setting alert texts is only supported for summary alerts - skipping texts setting for " + dpe);
  }
}

/**
 * Updates alert classes, texts and other alert class related configuration (e.g. notification) for all DPs of the given type.
 * @param dpType datapoint type that you want to update the alert class for (if the class in concern is not for a summary alert, then you can specify dpe like that: <dpType>[.dpe])
 * @param onlyInLocalSystem (optional, default = true) if this flag is false then this will be done for all connected systems as well. If you want to use that - think about what you're doing first ;)
 */
void emuAlert_updateAllAlertClassesForType(string dpType, bool onlyInLocalSystem = true) {
  dyn_string dps;
  string type = strsplit(dpType, ".")[1];
  if (onlyInLocalSystem) {
    dps = dpNames("*", type);
  } else {
    dps = dpNames("*:*", type);
  }
  for (int i=1; i <= dynlen(dps); i++) {
    emuAlert_updateAlertClass(dps[i], dpType);
  }
}

/**
 * Updates alert classes, texts and other class related configuration (e.g. notification) for all DPs of all known types (see DPs of type CscConf_alertClassDescriptors).
 * @param onlyInLocalSystem (optional, default = true) if this flag is false then this will be done for all connected systems as well. Classes configs are taken only from the local system though. If you want to use that - think about what you're doing first ;)
 */
void emuAlert_updateAllAlertClasses(bool onlyInLocalSystem = true) {
  dyn_string alertClassConfigs = dpNames("*", "CscConf_alertClassDescriptors");
  for (int i=1; i <= dynlen(alertClassConfigs); i++) {
    alertClassConfigs[i] = dpSubStr(alertClassConfigs[i], DPSUB_DP);
    strreplace(alertClassConfigs[i], "CscAlertClass_", "");
    emuAlert_updateAllAlertClassesForType(alertClassConfigs[i], onlyInLocalSystem);
  }
}


void emuAlert_createSummary(string dpe,
							dyn_string alertTexts,
							dyn_string dpelementList,      
							string alertPanel,
							dyn_string alertPanelParameters,
							string alertHelp,
							dyn_string &exceptionInfo)
{
	fwAlertConfig_setSummary(dpe, alertTexts, dpelementList,
								alertPanel, alertPanelParameters, alertHelp, exceptionInfo, FALSE);
  emuAlert_updateAlertClass(dpe);
}
