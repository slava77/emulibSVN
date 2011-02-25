/**@file

This package contains general purpose functions for handling alerts.

@author Evaldas Juska (PH/UCM)
@date   May 2010
*/

#uses "CMS_CSC_common/emu_common.ctl"

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
  alertClass = dpSubStr(alertClass, DPSUB_DP_EL);
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
  string dpe = "";
  if (strpos(dpType, ".") >= 0) {
    dpe = substr(dpType, strpos(dpType, "."));
  }
  if (onlyInLocalSystem) {
    dps = dpNames("*" + dpe, type);
  } else {
    dps = dpNames("*:*" + dpe, type);
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
    strreplace(alertClassConfigs[i], "_dot_", ".");
    emuAlert_updateAllAlertClassesForType(alertClassConfigs[i], onlyInLocalSystem);
  }
}

/**
  * Activates or deactivates the given alert. If it's a summary alert, then it calls the same function recursively to all of the registered DPEs in the summary alert.
  * @param activate if true, then the alert(s) is(are) activated, if false - deactivated.
  * @param dpe dpe to act on (or begin with).
  * @param ignorePatterns (optional, default empty) at any level of recursion, if the DPE to be activated/deactivated contains any of the strings inside this array, it's then ignored - not activated/deactivated and the recursion exists.
  */
void emuAlert_activateDeactivate(bool activate, string dpe, dyn_string &exceptionInfo, dyn_string ignorePatterns = makeDynString()) {
  bool configExists;
  int alertConfigType;
  dyn_string alertTexts;
  dyn_float alertLimits;
  dyn_string alertClasses;
  dyn_string summaryDpeList;
  string alertPanel;
  dyn_string alertPanelParameters;
  string alertHelp;
  bool isActive;

  for (int i=1; i <= dynlen(ignorePatterns); i++) {
    if (strpos(dpe, ignorePatterns[i]) >= 0) {
      return;
    }
  }
  
  fwAlertConfig_get(dpe,
                    configExists,
                    alertConfigType,
                    alertTexts,
                    alertLimits,
                    alertClasses,
                    summaryDpeList,
                    alertPanel,
                    alertPanelParameters,
                    alertHelp,
                    isActive,
                    exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }

  if (dynlen(summaryDpeList) > 0) {
    for (int i=1; i <= dynlen(summaryDpeList); i++) {
      emuAlert_activateDeactivate(activate, summaryDpeList[i], exceptionInfo, ignorePatterns);
      if (emu_checkException(exceptionInfo)) { return; }
    }
  }
  
  if (configExists) {
    if (activate && !isActive) {
      emu_info("emu_alert: activating alert on " + dpe);
      fwAlertConfig_activate(dpe, exceptionInfo);
      if (emu_checkException(exceptionInfo)) { return; }
    } else if (!activate && isActive) {
      emu_info("emu_alert: deactivating alert on " + dpe);
      fwAlertConfig_deactivate(dpe, exceptionInfo);
      if (emu_checkException(exceptionInfo)) { return; }
    }
  }
}

/**
  * Activates summary alerts on all DPs of the given type. In addition it activates all alerts that the summary alerts are refering to (recursively).
  * @param activate if true, then the alerts are activated, if false - deactivated.
  * @param dpType datapoint type that you want this function to work on.
  * @param ignorePatterns (optional, default empty) at any level of recursion, if the DPE to be activated/deactivated contains any of the strings inside this array, it's then ignored - not activated/deactivated and the recursion exists.
  * @param onlyInLocalSystem (optional, default = true) if this flag is false then this will be done for all connected systems as well. If you want to use that - think about what you're doing first ;)
  */
void emuAlert_activateDeactivateSummaryAlertsForType(bool activate, string dpType, dyn_string &exceptionInfo, 
                                                     dyn_string ignorePatterns = makeDynString(), bool onlyInLocalSystem = true) {
//   if (emuAlert_getAlertClassDescriptorDp(dpType) == "") {
//     return;
//   }
   
  dyn_string dps;
  string type = strsplit(dpType, ".")[1];
  if (onlyInLocalSystem) {
    dps = dpNames("*", type);
  } else {
    dps = dpNames("*:*", type);
  }
  for (int i=1; i <= dynlen(dps); i++) {
    emuAlert_activateDeactivate(activate, dps[i] + ".", exceptionInfo, ignorePatterns);
    if (emu_checkException(exceptionInfo)) { return; }
  }
}

/**
  * Activates/Deactivates (recursively digging into summary alerts) all alerts that have a CscConf_alertClassDescriptors DP associated.
  * @param activate if true, then the alerts are activated, if false - deactivated.
  * @param onlyInLocalSystem (optional, default = true) if this flag is false then this will be done for all connected systems as well. If you want to use that - think about what you're doing first ;)
  * @param ignorePatterns (optional, default empty) at any level of recursion, if the DPE to be activated/deactivated contains any of the strings inside this array, it's then ignored - not activated/deactivated and the recursion exists.
  */
void emuAlert_activateDeactivateSummaryAlertsWithDescriptors(bool activate, dyn_string &exceptionInfo, 
                                                             dyn_string ignorePatterns = makeDynString(), bool onlyInLocalSystem = true) {
  dyn_string alertClassConfigs = dpNames("*", "CscConf_alertClassDescriptors");
  for (int i=1; i <= dynlen(alertClassConfigs); i++) {
    alertClassConfigs[i] = dpSubStr(alertClassConfigs[i], DPSUB_DP);
    string dpType = alertClassConfigs[i];
    strreplace(dpType, "CscAlertClass_", "");
    emuAlert_activateDeactivateSummaryAlertsForType(activate, dpType, exceptionInfo, ignorePatterns, onlyInLocalSystem);
    if (emu_checkException(exceptionInfo)) { return; }
  }
}

/** Creates a summary alert and sets the right alert class. */
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
