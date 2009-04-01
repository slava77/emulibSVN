

mudcsLvGroupAcknowlege(string groupDp) {

  dyn_string ds1;
  int i;
  dyn_string exceptionInfo;
  dpGet(mudcsLvRemoveSystem(groupDp) + "." + ":_alert_hdl.._dp_list", ds1);
//DebugTN(s1);
  for (i = 1; i <= dynlen(ds1); i++) {
    fwAlertConfig_acknowledge(ds1[i], exceptionInfo);
  }
}
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================


mudcsLvAlertConfig(bool isAlertChangeOnly, bool isAlertActive, string dpe, dyn_float limits, bool isAck = false) {
// !!!!! 08/10/2004 Attention: this function is never called now with isAlertChangeOnly=true;
// !!!!! the alerts set active in the DeviceAlertSetting

  int j;
  string test_string;
  string error_class;

  if (isAck)
    error_class = "_fwErrorAck";
  else
    error_class = "_fwErrorNack";

////////// DebugN("mudcsCrbLV_1_alert_set:++++++++++++++++++++++++++++++++++"+" "+isAlertActive);
  bool alertRequest;
  int i, requestedRanges;
  int alertType, configType;  // +++++++++++++++++++++++

////////////    string dpe;
////////////    dyn_float limits = makeDynFloat();
  dyn_string alertClass, alertText, exceptionInfo;
//-------------------------
  dyn_string dpes, DPE_LIST;
  string DPE_TYPE;

//-------------------------
  // get state of checkbox
////////////////    alertRequest = alertOnOff.state(0);

  alertRequest = isAlertActive; // later:  to add one more parameter to routine: alertRequest

///////////////    Button1.enabled = FALSE;
  //Close.enabled = FALSE;

  // Convert list of dpes passed as a string to a dyn_string
///////////////    dpe = $sDpe;
  strreplace(dpe, " ", "");
  dpes = strsplit(dpe, "|");

//----------------------------------------------
  DPE_LIST = dpes;

// !!!!! 08/10/2004 Attention: this function is never called now with isAlertChangeOnly=true;
// !!!!! the alerts set active in the DeviceAlertSetting

  if (isAlertChangeOnly) {

    for (i = 1; i <= dynlen(dpes); i++) {
      if (isAlertActive)
        dpSetWait(mudcsLvAddSystem( dpes[i] + ":_alert_hdl.._active"), true);
      else
        dpSetWait(mudcsLvAddSystem( dpes[i] + ":_alert_hdl.._active"), false);

    }
    return;

  } // if(isAlertChangeOnly)

//---------------------------------------------------


//++++++++++++++++++
  if (alertRequest) {
//---------------------------
    DPE_TYPE = dpElementType(dpes[1]); // instead of DPES_OK = _fwConfigs_checkDpeList(DPE_LIST, fwConfigs_PVSS_ALERT_HDL, DPE_TYPE, exceptionInfo);
//----------------------------
    DebugTN("DPE_TYPE=" + DPE_TYPE + " " + dpes[1]);
    // read input data from the panel depending on the dpe type
    _fwConfigs_getConfigOptionsForDpeType(DPE_TYPE, fwConfigs_PVSS_ALERT_HDL, configType, exceptionInfo);
    DebugTN("config_type=" + configType + " " + fwConfigs_ANALOG_OPTIONS);
    switch (configType) {
    case fwConfigs_ANALOG_OPTIONS:
      alertType = DPCONFIG_ALERT_NONBINARYSIGNAL;
/////////				requestedRanges = rangeCombo.text;
      requestedRanges = dynlen(limits) + 1;

      for (i = 1; i <= requestedRanges; i++) {
////////				getValue("alText" + i, "text", alertText[i]);
        if (requestedRanges >= 3 ) {
          if (i % 2)alertText[i] = "error";
          else alertText[i] = "";;
        } else if (requestedRanges == 2 ) {
          if (i == 2)alertText[i] = "error";
          else alertText[i] = "";;
        }

///////					getValue("alClass" + i, "text", alertClass[i]);
//if(i!=2)alertClass[i]="_fwErrorNack";
        if (requestedRanges >= 3) {
          if (i % 2)alertClass[i] = error_class;//"_fwErrorNack";
          else alertClass[i] = "";
        } else if (requestedRanges == 2 ) {
          if (i == 2)alertClass[i] = error_class;//"_fwErrorNack";
          else alertClass[i] = "";
        }
        if (alertClass[i] != "")
          alertClass[i] = alertClass[i] + ".";
      }
      DebugTN("DPE_TYPE1122=" + DPE_TYPE);
      // put the limits in a dyn_float
///////				for (i = 1; i < requestedRanges; i++)
//////					getValue("limNum" + i, "text", limits[i]);
      break;

    case fwConfigs_BINARY_OPTIONS:
      alertType = DPCONFIG_ALERT_BINARYSIGNAL;
      if (limits[1] == 1) {
        alertText = makeDynString("OK", "STATUS ERROR");
//////				alertText = makeDynString(alText7.text, alText6.text);
//////
//////	   			// create alarm class list with valid range
//////				if (radio1.number == 1)
        alertClass = makeDynString("", error_class + ".");
      } else {
        alertText = makeDynString( "STATUS ERROR", "OK");
        alertClass = makeDynString(error_class + ".", "");

      }
//////					alertClass = makeDynString("", alClass3.text + ".");
//////				else if (radio1.number == 0)
//////					alertClass = makeDynString(alClass3.text + ".", "");
      break;
    default:
      break;
    }

///// DebugN(DPE_LIST[1]+".......................");
    DebugTN("DPE_TYPE1133=" + DPE_TYPE);
    fwAlertConfig_setMultiple(DPE_LIST, alertType, alertText, limits, alertClass,
                              makeDynString(), "", makeDynString(),  "", exceptionInfo);
    /*
    DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++11111");
    DebugTN(DPE_LIST);
    DebugTN("alertType="+alertType);
    DebugTN(alertText);
    DebugTN(limits);
    DebugTN(alertClass);
    DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++11111");
    */

//for(j=1;j<=dynlen(DPE_LIST);j++){
//mudcsLvDebug2(DPE_LIST[j]);
//}

//for(j=1;j<=dynlen(limits);j++){
//test_string=limits[j];
//mudcsLvDebug2(test_string);
//}

//-----------------------------------------------------------------------------------------
    for (i = 1; i <= dynlen(DPE_LIST); i++) {
      dpSetWait(mudcsLvAddSystem( DPE_LIST[i] + ":_alert_hdl.._active"), true);
    }
//-----------------------------------------------------------------------------------------

  } else {
    fwAlertConfig_deleteMultiple(DPE_LIST, exceptionInfo);
  }

  if (dynlen(exceptionInfo) > 0)
    fwExceptionHandling_display(exceptionInfo);
//+++++++++++++++++++++++++++++++++++++


}

//===================================================================================

/*
PVSS00ui1:2007.02.12 16:52:46.827["++++++++++++++++++++++++++++++++++++++++++++++++++++++11111"]
PVSS00ui1:2007.02.12 16:52:46.827[dyn_string 1 items
PVSS00ui1:     1:"CMS_CSC:CSC_ME_M3_PC1_LV_CRB.data.v15_2"
PVSS00ui1:]
PVSS00ui1:2007.02.12 16:52:46.827["alertType=13"]
PVSS00ui1:2007.02.12 16:52:46.827[dyn_string 3 items
PVSS00ui1:     1:""
PVSS00ui1:     2:""
PVSS00ui1:     3:""
PVSS00ui1:]
PVSS00ui1:2007.02.12 16:52:46.827[dyn_float 2 items
PVSS00ui1:     1:1.2
PVSS00ui1:     2:1.8
PVSS00ui1:]
PVSS00ui1:2007.02.12 16:52:46.828[dyn_string 3 items
PVSS00ui1:     1:"_fwErrorNack."
PVSS00ui1:     2:""
PVSS00ui1:     3:"_fwErrorNack."
PVSS00ui1:]
PVSS00ui1:2007.02.12 16:52:46.828["++++++++++++++++++++++++++++++++++++++++++++++++++++++11111"]
*/

//===================================================================================
