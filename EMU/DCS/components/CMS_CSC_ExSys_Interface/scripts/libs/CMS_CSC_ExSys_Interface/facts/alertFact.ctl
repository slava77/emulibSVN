/**
  * Connects to all alerts and sends facts when they come and go
  */
public void exsys_startAlertFactDelivery(dyn_string &ex) {
// TODO find all alert configs and send their initial state (connect with true doesn't work because it starts discarding function calls due to big load)  
  dyn_errClass err;
  emu_info("Starting alert facts delivery service....");

  int rc = alertConnect("exsys_updateAlertsCB", true, ":_alert_hdl.._alert_color");
  err = getLastError();
  if ((rc != 0) || (dynlen(err) > 0)) {
    emu_addError("alertConnect failed, return code = " + rc, ex);
    emu_addError(getErrorText(err), ex);
    return;
  }
  
  emu_info("Alert facts delivery service started.");
}

public void exsys_updateAlertsCB(time t, int count, string alert) {
  // this is to return quickly so that we don't loose the function calls during 
  // startup when there's a huge number of alert states being sent out
  startThread("exsys_sendAlertFact", alert);
}

private void exsys_sendAlertFact(string alert) {
  string alertConf = dpSubStr(alert, DPSUB_SYS_DP_EL_CONF);

  // determine state and text  
  int actState;
  string actText;
  anytype value;
  dpGet(alertConf + ".._act_state", actState,
        alertConf + ".._act_text", actText);
  
  bool isOn = ((actState == 1) || (actState == 2)),
       isAcknowledged = ((actState == 0) || (actState == 2));
  
  dyn_string paramNames = makeDynString("isOn",
                                        "isAcknowledged",
                                        "alertText");
  dyn_anytype paramValues = makeDynAnytype(isOn,
                                           isAcknowledged,
                                           actText);
  // determine value  
  string valueDpe = dpSubStr(alertConf, DPSUB_DP_EL) + ":_original.._value";
  if (dpExists(valueDpe)) {
    anytype value;
    dpGet(valueDpe, value);
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
  exsys_sendFact("DcsAlertFact", t, dpSubStr(alertConf, DPSUB_DP_EL), true, EXSYS_FACT_SEVERITY_WARN,
                 "DCS Alert", paramNames, paramValues, ex);
  if (emu_checkException(ex)) { return; }
}
