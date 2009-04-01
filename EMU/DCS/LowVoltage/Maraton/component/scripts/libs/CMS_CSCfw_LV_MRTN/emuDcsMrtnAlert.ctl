

dyn_string dps_cr;
dyn_string dps_ch;


//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
mudcsMrtnCrateAlertSet(bool isCreateConfig, bool isAck) {


  dyn_string limits_s;
  string value;
  dyn_string dps;
  int i, j;


  limits_s = makeDynString();

// s1 is OK
// s2 is ALARM


  dps = dpNames("*", "FwWienerMarathon");
//==================================
  for (i = 1; i <= dynlen(dps); i++) {

    DebugTN("==============" + dps[i]);

    limits_s = makeDynString("1"); // that gives an alarm if the value >= 1

//value=mudcsLvRemoveSystem(dps[i]+".Status.OutputFailure"); // commented as OutputFailure is always TRUE while the maraton works OK
//mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

//value=mudcsLvRemoveSystem(dps[i]+".Status.CrateStatus");
//mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);


  } // for i

}
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
mudcsMrtnChannelAlertSet(bool isCreateConfig, bool isAck) {


  dyn_string limits_s;
  string value;
  dyn_string dps;
  int i, j;


  limits_s = makeDynString();

// s1 is OK
// s2 is ALARM


  dps = dpNames("*", "FwWienerMarathonChannel");
//==================================
  for (i = 1; i <= dynlen(dps); i++) {

    limits_s = makeDynString("1"); // that gives an alarm if the value >= 1



    value = mudcsLvRemoveSystem(dps[i] + ".Status.FailureMaxCurrent");
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    value = mudcsLvRemoveSystem(dps[i] + ".Status.FailureMaxSenseVoltage");
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    value = mudcsLvRemoveSystem(dps[i] + ".Status.FailureMaxTerminalVoltage");
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    /*
    value=mudcsLvRemoveSystem(dps[i]+".Status.FailureMinSenseVoltage");
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    value=mudcsLvRemoveSystem(dps[i]+".Status.FailureMaxPower");
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    value=mudcsLvRemoveSystem(dps[i]+".Status.FailureTimeout");
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);
    */

    value = mudcsLvRemoveSystem(dps[i] + ".Status.FailureMaxTemperature");
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

//value=mudcsLvRemoveSystem(dps[i]+".MeasurementCurrent:_original.._invalid");
//mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);
//dpGet(device+".MeasurementCurrent:_original.._invalid", quality);

  } // for i

}

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
mudcsMrtnSummaryAlertSet(bool isCreateConfig) {
//==================================
//=== SUMMARY ALERT ================
//==================================

  dyn_string dps, dps1, dps2, dps3;
  int i, j, i10;
  dyn_string exceptionInfo, s1;
  string coord;

  dps = dpNames("*", "FwWienerMarathon");

//-----------------------------------
  /*
  dps=dpNames("*","CRB_2");

   for(i=1;i<=dynlen(dps);i++){
    dynClear(dps2);
    dpGet(dps[i]+".coord",coord);

    dps1=dpNames("*ELMB_"+coord+"*","FwElmbAi");
    for(j=1;j<=dynlen(dps1);j++){
     dps2[j]=dps1[j]+".value";
    }
   //  DebugTN(dps2);
    dps1=dpNames("*ELMB_"+coord+"*","FwElmbNode");
    for(j=1;j<=dynlen(dps1);j++){
  //   dps3[2*j]=dps1[j]+".state.value";
  //   dps3[2*j+1]=dps1[j]+".error";
      dps3[j]=dps1[j]+".emergency.emergencyErrorCode";
    }

    s1=dps[i]+".error_status";
    dynAppend(dps2,dps3);
    dynAppend(dps2,s1);

    //DebugTN(dps2);
    */
//------------------------------------------------------------
  int type, active;


//==================================
  for (i = 1; i <= dynlen(dps); i++) {
    dynClear(dps2);
    //  DebugTN("+=++  "+dps[i]);
    dps1 = dpNames(dps[i] + ".Status.*Failure*");
    for (j = 1; j <= dynlen(dps1); j++) {
      dpGet(dps1[j] + ":_alert_hdl.._type", type);
      DebugTN(dps1[j] + ":_alert_hdl.._type" + ">>" + type);
      dpGet(dps1[j] + ":_alert_hdl.._active", active);
      if (type > 0 && active > 0)dynAppend(dps2, dps1[j]);
    } //j

    dps3 = dpNames("*", "FwWienerMarathonChannel");
//==================================
    for (i10 = 1; i10 <= dynlen(dps3); i10++) {
      if (strpos(dps3[i10], dps[i]) < 0)continue;
      dps1 = dpNames(dps3[i10] + ".Status.*Failure*");
      for (j = 1; j <= dynlen(dps1); j++) {
        dpGet(dps1[j] + ":_alert_hdl.._type", type);
        dpGet(dps1[j] + ":_alert_hdl.._active", active);
        if (type > 0 && active > 0)dynAppend(dps2, dps1[j]);
      } //j
    } //i

//-----------------------------------------------------------
    if (isCreateConfig) {
      fwAlertConfig_createSummary( mudcsLvRemoveSystem(dps[i]) + ".",
                                   makeDynString("", "") , dps2 , "", makeDynString(), "", exceptionInfo);
      dpSetWait(dps[i] + ".:_alert_hdl.._active", true);
    } else {
      fwAlertConfig_deleteSummary( mudcsLvRemoveSystem(dps[i]) + ".", exceptionInfo);
    }

    dyn_string s1;
//dpSetWait(mudcsLvRemoveSystem(dps[i])+"."+":_alert_hdl.._ack_state",DPATTR_ACKTYPE_SINGLE );//"_fwErrorAck");
    dpGet(mudcsLvRemoveSystem(dps[i]) + "." + ":_alert_hdl.._dp_list", s1);
    DebugTN(s1);


//  dpSet(mudcsLvRemoveSystem(dps[i])+"."+":_alert_hdl.._active", true);
  } // for i

// TestDP_1.element:_alert_hdl.1._class", alertclass[1],

}
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================











//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
mudcsPL500CrateAlertSet(bool isCreateConfig, bool isAck) {


  dyn_string limits_s;
  string value;
  dyn_string dps;
  int i, j;
  string element;

  limits_s = makeDynString();

// s1 is OK
// s2 is ALARM


  dps = dpNames("*", "FwWienerPS");
//==================================
  for (i = 1; i <= dynlen(dps); i++) {

    DebugTN("==============" + dps[i]);

    limits_s = makeDynString("0"); // that gives an alarm if the value >= 1

    element = ".General.Status.GetACInLimit";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".General.Status.GetNoErrors";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Fans.Status.GetFansOK";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);


    limits_s = makeDynString("1"); ////////////////////////////////////

    element = ".General.Status.GetEEPROMError";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);


    element = ".Temperatures.Status.ExtTempErrorFlag1";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.ExtTempErrorFlag2";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.ExtTempErrorFlag3";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.ExtTempErrorFlag4";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.ExtTempErrorFlag5";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.ExtTempErrorFlag6";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.ExtTempErrorFlag7";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.ExtTempErrorFlag8";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.TempErrorFlag1";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.TempErrorFlag2";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.TempErrorFlag3";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.TempErrorFlag4";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.TempErrorFlag5";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.TempErrorFlag6";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.TempErrorFlag7";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Temperatures.Status.TempErrorFlag8";
    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    dynAppend(dps_cr, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);


  } // for i

}
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
mudcsPL500ChannelAlertSet(bool isCreateConfig, bool isAck) {


  dyn_string limits_s;
  string value;
  dyn_string dps;
  int i, j;
  string element;

  limits_s = makeDynString();

// s1 is OK
// s2 is ALARM


  dps = dpNames("*", "FwWienerChannel");
//==================================
  for (i = 1; i <= dynlen(dps); i++) {

    limits_s = makeDynString("1"); // that gives an alarm if the value >= 1


    element = ".Actual.CurrentFlag";
    dynAppend(dps_ch, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Actual.OverVoltFlag";
    dynAppend(dps_ch, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Actual.OverVoltProtFlag";
    dynAppend(dps_ch, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

    element = ".Actual.UnderVoltFlag";
    dynAppend(dps_ch, element);
    value = mudcsLvRemoveSystem(dps[i] + element);
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);



  } // for i

}

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
mudcsPL500SummaryAlertSet(bool isCreateConfig) {
//==================================
//=== SUMMARY ALERT ================
//==================================

  dyn_string dps, dps1, dps2, dps3;
  int i, j, i10;
  dyn_string exceptionInfo, s1;
  string coord;

  dps = dpNames("*", "FwWienerPS");

//-----------------------------------
  /*
  dps=dpNames("*","CRB_2");

   for(i=1;i<=dynlen(dps);i++){
    dynClear(dps2);
    dpGet(dps[i]+".coord",coord);

    dps1=dpNames("*ELMB_"+coord+"*","FwElmbAi");
    for(j=1;j<=dynlen(dps1);j++){
     dps2[j]=dps1[j]+".value";
    }
   //  DebugTN(dps2);
    dps1=dpNames("*ELMB_"+coord+"*","FwElmbNode");
    for(j=1;j<=dynlen(dps1);j++){
  //   dps3[2*j]=dps1[j]+".state.value";
  //   dps3[2*j+1]=dps1[j]+".error";
      dps3[j]=dps1[j]+".emergency.emergencyErrorCode";
    }

    s1=dps[i]+".error_status";
    dynAppend(dps2,dps3);
    dynAppend(dps2,s1);

    //DebugTN(dps2);
    */
//------------------------------------------------------------
  int type, active;


//==================================
  for (i = 1; i <= dynlen(dps); i++) {
    //  DebugTN("+=++  "+dps[i]);
// dps1=dpNames(dps[i]);
// for(j=1;j<=dynlen(dps1);j++){
    for (j = 1; j <= dynlen(dps_cr); j++) {

      dpGet(dps[i] + dps_cr[j] + ":_alert_hdl.._type", type);
      DebugTN(dps[i] + dps_cr[j] + ":_alert_hdl.._type" + ">>" + type);
      dpGet(dps[i] + dps_cr[j] + ":_alert_hdl.._active", active);
      if (type > 0 && active > 0)dynAppend(dps2, dps[i] + dps_cr[j]);
    }
// } //j

    dps3 = dpNames("*", "FwWienerChannel");
//==================================
    for (i10 = 1; i10 <= dynlen(dps3); i10++) {
      if (strpos(dps3[i10], dps[i]) < 0)continue;
// dps1=dpNames(dps3[i10]);
      for (j = 1; j <= dynlen(dps_ch); j++) {
        dpGet(dps3[i10] + dps_ch[j] + ":_alert_hdl.._type", type);
        dpGet(dps3[i10] + dps_ch[j] + ":_alert_hdl.._active", active);
        if (type > 0 && active > 0)dynAppend(dps2, dps3[i10] + dps_ch[j]);
      } //j
    } //i10

//-----------------------------------------------------------
    if (isCreateConfig) {
      fwAlertConfig_createSummary( mudcsLvRemoveSystem(dps[i]) + ".",
                                   makeDynString("", "") , dps2 , "", makeDynString(), "", exceptionInfo);
      dpSetWait(dps[i] + ".:_alert_hdl.._active", true);
    } else {
      fwAlertConfig_deleteSummary( mudcsLvRemoveSystem(dps[i]) + ".", exceptionInfo);
    }

    dyn_string s1;
//dpSetWait(mudcsLvRemoveSystem(dps[i])+"."+":_alert_hdl.._ack_state",DPATTR_ACKTYPE_SINGLE );//"_fwErrorAck");
    dpGet(mudcsLvRemoveSystem(dps[i]) + "." + ":_alert_hdl.._dp_list", s1);
    DebugTN(s1);


//  dpSet(mudcsLvRemoveSystem(dps[i])+"."+":_alert_hdl.._active", true);
  } // for i

// TestDP_1.element:_alert_hdl.1._class", alertclass[1],

}
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

