#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysUtil.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysInterface.ctl"

/**@file

Alert Fact handling

@author Evaldas Juska (PH/UCM)
@date   February 2011
*/

/**
  * Connects to all alerts and sends facts when they come and go
  */
public void exsys_startAlertFactDelivery(dyn_string &ex) {
// TODO find all alert configs and send their initial state (connect with true doesn't work because it starts discarding function calls due to big load)  
  dyn_errClass err;
  emu_info("Starting alert facts delivery service....");

  emu_info("Sending out initial states of all alerts....");
  exsys_sendAllAlertStates();
  
  int rc = alertConnect("exsys_updateAlertsCB", false, ":_alert_hdl.._alert_color");
  err = getLastError();
  if ((rc != 0) || (dynlen(err) > 0)) {
    emu_addError("alertConnect failed, return code = " + rc, ex);
    emu_addError(getErrorText(err), ex);
    return;
  }
  
  emu_info("Alert facts delivery service started.");
}

// private void exsys_sendAllAlertStates() {
//   dyn_string ex;    
//   dyn_dyn_anytype alerts;
// 
//   dpQuery("SELECT '_alert_hdl.._act_state', '_alert_hdl.._act_text', '_original.._value'"
//           + " FROM '*' " //+ " REMOTE '" + cscSystems[i] + ":'"
//           + " WHERE '_alert_hdl.._type' != 0", alerts);
// 
//   for(int i=1; i <= dynlen(alerts); i++) {
//     dyn_anytype alert = alerts[i];
//     exsys_sendAlertFactWithData(alert[1], alert[2], alert[3], alert[4]);
//   }
// }

private void exsys_sendAllAlertStates() {
  // this basically returns all DPEs in the system since everybody actually appear to have a default alert handler
  dyn_string allAlertTypeDpes = dpNames("*.**:_alert_hdl.._type");
  dyn_int alertTypes;
  dpGet(allAlertTypeDpes, alertTypes);
  dyn_string realAlertDpes;
  for (int i=1; i <= dynlen(allAlertTypeDpes); i++) {
    if (alertTypes[i] != 0) { // only those which have _alert_hdl.._type != 0 are real alert handlers - filter only those
      dynAppend(realAlertDpes, allAlertTypeDpes[i]);
    }
  }
  
  // send the alert state facts for all of the real alerts
  for (int i=1; i <= dynlen(realAlertDpes); i++) {
    exsys_sendAlertFact(realAlertDpes[i]);
  }
}

public void exsys_updateAlertsCB(time t, int count, string alert) {
  // this is to return quickly so that we don't loose the function calls during heavy load
  startThread("exsys_sendAlertFact", alert);
}

public void exsys_sendAlertFact(string alert) {
  string alertConf = dpSubStr(alert, DPSUB_SYS_DP_EL_CONF);

  // determine state and text  
  int actState;
  string actText;
  anytype value;
  dpGet(alertConf + ".._act_state", actState,
        alertConf + ".._act_text", actText);
  
  // determine value  
  string valueDpe = dpSubStr(alertConf, DPSUB_DP_EL) + ":_original.._value";
  if (dpExists(valueDpe)) {
    anytype value;
    dpGet(valueDpe, value);
    exsys_sendAlertFactWithData(dpSubStr(alertConf, DPSUB_DP_EL), actState, actText, value);
  } else {
    exsys_sendAlertFactWithData(dpSubStr(alertConf, DPSUB_DP_EL), actState, actText);
  }
}

public void exsys_sendAlertFactWithData(string dpe, int actState, string actText, anytype value = -9999999) {
  bool isOn = ((actState == 1) || (actState == 2)),
       isAcknowledged = ((actState == 0) || (actState == 2));
  
  dyn_string paramNames = makeDynString("isOn",
                                        "isAcknowledged",
                                        "alertText");
  dyn_anytype paramValues = makeDynAnytype(isOn,
                                           isAcknowledged,
                                           actText);

  if (value != -9999999) {
    int valueType = getType(value);
    // numeric value
    if ((valueType == FLOAT_VAR) || (valueType == INT_VAR) || (value == UINT_VAR)) {
      dynAppend(paramNames, "sourceNumericValue");
      dynAppend(paramValues, value);
    } else {
      dynAppend(paramNames, "sourceStringValue");
      dynAppend(paramValues, value);
    }
  }
  
  time t = getCurrentTime();
  dyn_string ex;
  exsys_sendFact("DcsAlertFact", t, dpSubStr(dpe, DPSUB_DP_EL), true, EXSYS_FACT_SEVERITY_WARN,
                 "DCS Alert", paramNames, paramValues, ex);
  if (emu_checkException(ex)) { return; }
}
