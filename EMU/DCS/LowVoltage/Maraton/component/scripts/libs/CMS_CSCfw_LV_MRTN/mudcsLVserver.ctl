//#uses "CMS_CSCfw_LV_CRB/emuDcsLv.ctl"
#uses "CMS_CSCfw_LV_MRTN/emuDcsLv.ctl"
//#uses "CMS_CSCfw_LV_CRB/emuDcsCrb.ctl"
#uses "CMS_CSCfw_LV_MRTN/emuDcsMrtn.ctl"
//#uses "CMS_CSCfw_LV_CRB/emuDcsLvAlert.ctl"
#uses "CMS_CSCfw_LV_MRTN/emuDcsLvAlert.ctl"
//#uses "CMS_CSCfw_LV_CRB/emuDcsCrbAlert.ctl"
#uses "CMS_CSCfw_LV_MRTN/emuDcsMrtnAlert.ctl"
//#uses "CMS_CSCfw_LV_CRB/emufw.ctl"
#uses "CMS_CSCfw_LV_MRTN/emufw.ctl"

#uses "CMS_CSCfw_LV_MRTN/mudcs4SetPanelsMrtn.ctl"

dyn_string CSC_fwCAN2_g_ALERT_HANDLERS_FIRST;
int CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT;
dyn_string CSC_fwCAN2_g_STATUS_HANDLERS_FIRST;
dyn_string CSC_fwCAN2_g_FULL_DPLIST;

int FIRST_FSM_PANELS = 0;

//dyn_int CSC_fwCAN2_g_nodeGuardInterval;
//dyn_string CSC_fwCAN2_g_CANBUSES;
//dyn_string CSC_fwCAN2_g_CRB2ELMB;

//========================================================
main() {

  mudcsMrtnPnlInit();
  mudcsMrtnServerInit();
  mudcsMrtnUpdate_BrokerList();

  mudcsFsmPanelsCheck();
  startThread("mudcsMrtn_threadWatchForTimeouts");
/// !!!!!!! needed if no call to startThread("mudcsCrb_threadWatchForTimeouts");  while(1){delay(0,1000);};
}


//========================================================
mudcsMrtn_threadWatchForTimeouts() {
  dyn_string ch_dps;
  int i;
  int quality;
  int update_status;

  ch_dps = dpNames("*", "FwWienerMarathonChannel");


  while (1) {
    delay(120);

// DebugTN(ch_dps);
    DebugTN("mudcsMrtn_threadWatchForTimeouts()+++++++++++++++++++++++++++++++++++++");
// this is to call  _valueChanged function of channels
    for (i = 1; i <= dynlen(ch_dps); i++) {
      dpGet(ch_dps[i] + ".Status.On:_original.._invalid", quality);
      if (quality) { // i.e. bad quality
        dpGet(ch_dps[i] + ".Status.On", update_status);
        dpSetWait(ch_dps[i] + ".Status.On", update_status);
      }
    }    //for


  } // while


}
//========================================================

mudcsMrtnUpdate_BrokerList() {

  int j;
  dyn_string d_s1, d_s2, ch_dps;
  string update_fun_alert, update_fun_status;


//dpSet("dyn_debug1.",makeDynString());
//dpSet("dyn_debug2.",makeDynString());

  DebugN(" INFO, mudcsMrtnServer: init start***************************");

  dyn_string BrokerList, BrokerList_old;
  string service, update_fun_alert, update_fun_status;
  dyn_string datapoints;

//-------------- MRTN ----------------------------------------------
  BrokerList = makeDynString("FwWienerMarathon"); // add

  for (j = 1; j <= dynlen(BrokerList); j++) {

    service = BrokerList[j];// add

    update_fun_alert = "mudcsMrtnUpdateMonitor_" + "MRTN_2" + "_alert";
    update_fun_status = "mudcsMrtnUpdateMonitor_" + "MRTN_2" + "_status";
    datapoints = dpNames("*", service);


    for (int i = 1; i <= dynlen(datapoints); i++) { // corr

      dynAppend(CSC_fwCAN2_g_ALERT_HANDLERS_FIRST, datapoints[i]);

      CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT = 1;
      dpConnect(update_fun_alert, datapoints[i] + ".:_alert_hdl.._act_state_color");  //:_alert_hdl.._active

      while (CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT);


//dpSetWait(fsm+".:_alert_hdl.._active", false);
    } //i

    for (int i = 1; i <= dynlen(datapoints); i++) {
      if (strpos(datapoints[i], "Dummy") >= 0)continue;
      dynAppend(CSC_fwCAN2_g_STATUS_HANDLERS_FIRST, datapoints[i]);
      dynAppend(CSC_fwCAN2_g_FULL_DPLIST, datapoints[i]);

      CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT = 1;
      dpConnect(update_fun_status, datapoints[i] + ".Settings.OnOffCrate");


      while (CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT);

    } //i



  } //j
//---------  END MRTN ------------------------------------------------------
//-------------- PL500 ----------------------------------------------
  BrokerList = makeDynString("FwWienerPS"); // add

  for (j = 1; j <= dynlen(BrokerList); j++) {
    service = BrokerList[j];// add

    update_fun_alert = "mudcsMrtnUpdateMonitor_" + "PL500_2" + "_alert";
    update_fun_status = "mudcsMrtnUpdateMonitor_" + "PL500_2" + "_status";
    datapoints = dpNames("*", service);


    for (int i = 1; i <= dynlen(datapoints); i++) { // corr

      dynAppend(CSC_fwCAN2_g_ALERT_HANDLERS_FIRST, datapoints[i]);

      CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT = 1;
      dpConnect(update_fun_alert, datapoints[i] + ".:_alert_hdl.._act_state_color");  //:_alert_hdl.._active

      while (CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT);


//dpSetWait(fsm+".:_alert_hdl.._active", false);
    } //i

    for (int i = 1; i <= dynlen(datapoints); i++) {
      if (strpos(datapoints[i], "Dummy") >= 0)continue;
      dynAppend(CSC_fwCAN2_g_STATUS_HANDLERS_FIRST, datapoints[i]);
      dynAppend(CSC_fwCAN2_g_FULL_DPLIST, datapoints[i]);

      CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT = 1;
      dpConnect(update_fun_status, datapoints[i] + ".General.Commands.OnOffCrate");

      while (CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT);

    } //i
  } //j
//---------  END PL500 ------------------------------------------------------




//-------------------------------------------
  DebugN("mudcsMrtnServer: init end***************************");

}
//============================================================================================






//============================================================================================
mudcsMrtnUpdateMonitor_MRTN_2_status(string dpName, bool value) {

// ROLE OF THE FUNCTION: TO RESTORE THE STATUSES AFTER PVSS PROJECT RESTART

//string mudcs_alias;

  string fsm;

// attention :: the order of power/on/off and setting alarm is essential !!!
  DebugN("mudcsMrtnUpdateMonitor_MRTN_2_status:&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");

  fsm = substr(dpName, 0, strpos(dpName, "."));
  int index = dynContains(CSC_fwCAN2_g_STATUS_HANDLERS_FIRST, fsm); // to eliminate .status
  if (index >= 1 ) { // first call of handler
    DebugN("mudcsMrtnUpdateMonitor_MRTN_2_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER MRTN_2" + " " + dpName);
    dynRemove(CSC_fwCAN2_g_STATUS_HANDLERS_FIRST, index);


/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED"
// to restore the previous system state JUST IN CASE
/////  return;
  }


  DebugN("mudcsMrtnServer::mudcsMrtnUpdateMonitor_MRTN_2_status ***************************" + dpName + " = " + value);


// !!!! COMMENT: MULTY_MODE_DONE.value is set true when MUDCS_DOMAIN.value is defined
// !!!! it is needed to call all;all commands only once.
// !!!! (multy-mode is not used now: see below).


  DebugN("mudcsMrtnUpdateMonitor_MRTN_2_status:%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " + dpName);


  if ( (value == true && index >= 1) ) {  // -1, -2  is to initialize the state at start

///    dynAppend(d_s2,"11111"+dpName);
///    dpSetWait("dyn_debug2.",d_s2);

///mudcsMrtnCommandCrb(fsm,true);
    ///    startThread("mudcsMrtn_threadAlertActiveSet",fsm+".:_alert_hdl.._active",true);
    //dpSetWait(fsm+".:_alert_hdl.._active",true);
    dpSetWait(fsm + ".Settings.OnOffCrate", true);


    DebugN("mudcsMrtnServer::mudcsMrtnUpdateMonitor_MRTN_2_status == 2++***************************" + dpName + value);
  } else if (value == false  && index >= 1  /*|| (value==-2 && state_destin==false && index >=1)*/) { //  -2  is to initialize the state at start

/////////////////// if(state_save ==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".status",-2);// this all is to allow to switch off from error state and differ -2 and -1

    dpSetWait(fsm + ".Settings.OnOffCrate", false);
    /*
    bool bIsActive;
    dpGet(fsm+".:_alert_hdl.._active",bIsActive);
    if(bIsActive){
    if(CSC_fwCAN2_g_ISACK)mudcsLvGroupAcknowlege(fsm);
    dpSetWait(fsm+".:_alert_hdl.._active", false);

    } // if(bIsActive)
    */
//// mudcsMrtnCommandCrb(fsm,false);
///  if(CSC_fwCAN2_g_ISACK)mudcsLvGroupAcknowlege(fsm);
//       startThread("mudcsMrtn_threadAlertActiveSet",fsm+".:_alert_hdl.._active",false);

//       dpSetWait(fsm+".:_alert_hdl.._active", false);

    DebugN("mudcsMrtnServer::mudcsMrtnUpdateMonitor_MRTN_2_status == 0++***************************" + dpName + value);
  }


  CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT = 0;

}


//============================================================================================
//============================================================================================

mudcsMrtnUpdateMonitor_MRTN_2_alert(string dpName, string value) {

// ROLE OF FUNCTION: to call _valueChanged function

  string fsm;
  int current_status;
  dyn_string ch_dps;
  int i;
  bool update_status;

// attention :: the order of power/on/off and setting alarm is essential !!!

// THIS MOSTLY RELATES TO THE CRB:
// TO MY NOTICE: ATTENTION!: IF THE DATA VALUE CHANGES WHEN VALUE IS IN THE ERROR INTERVAL
// (THE _alert_hdl.._act_state_color STAYS "FwAlarmErrorAck")
// NEVERTHELESS THE VALUE IN THIS HANDLER (_alert_hdl.._act_state_color) IS UPDATED !!!!
// I.E. THIS HANDLER IS INVOKED
// THIS IS NOT THE CASE IF DATA VALUE CHANGES WHEN VALUE IS IN THE VALID INTERVAL (HANDLER IS NOT INVOKED)
//


  fsm = substr(dpName, 0, strpos(dpName, "."));
  DebugN("mudcsMrtnUpdateMonitor_fwMrtn_CSC_LV_alert:&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ", fsm);
  int index = dynContains(CSC_fwCAN2_g_ALERT_HANDLERS_FIRST, fsm); // to eiminate ...
  if (index >= 1 ) { // first call of handler
    DebugN("mudcsMrtnUpdateMonitor_fwMrtn_CSC_LV_alert:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER fwMrtn_CSC_LV" + " " + dpName);
    dynRemove(CSC_fwCAN2_g_ALERT_HANDLERS_FIRST, index);

///////  return;
  }

  ch_dps = dpNames(fsm + "/*", "FwWienerMarathonChannel");

// DebugTN(ch_dps);

// this is to call  _valueChanged function of channels
  for (i = 1; i <= dynlen(ch_dps); i++) {
    dpGet(ch_dps[i] + ".Status.On", update_status);
    dpSetWait(ch_dps[i] + ".Status.On", update_status);
  }
// this is to call  _valueChanged function of crate
  dpGet(fsm + ".Status.On", update_status);
  dpSetWait(fsm + ".Status.On", update_status);

  /*
  dyn_string d_s2;
      dpGet("dyn_debug2.",d_s2);
      dynAppend(d_s2,dpName);
      dpSetWait("dyn_debug2.",d_s2);
  */


  /*
  bool bIsActive;
  dpGet(fsm+".:_alert_hdl.._active",bIsActive);

  // I understand that change of ._alert_hdl.._act_state_color can be only if bIsActive is true :: it is not so !!!
   DebugTN("##########################====== "+fsm+".:_alert_hdl.._active"+" "+value+" "+bIsActive);

  if(bIsActive){
   dpGet(fsm+".status",current_status);
    DebugTN("########################## "+value+" "+bIsActive);
  if(value == "FwAlarmErrorAck"){
   if(current_status != -1)dpSet(fsm+".status",-1);
  }
  else {
    if(current_status != 2)dpSet(fsm+".status",2);
  }
  } // if(bIsActive)
  else{ // !bIsActive

  }
  */

  CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT = 0;

}


//============================================================================================
//============================================================================================
mudcsMrtnUpdateMonitor_PL500_2_status(string dpName, bool value) {

// ROLE OF THE FUNCTION: TO RESTORE THE STATUSES AFTER PVSS PROJECT RESTART
//string mudcs_alias;

  string fsm;

// attention :: the order of power/on/off and setting alarm is essential !!!
  DebugN("mudcsMrtnUpdateMonitor_PL500_2_status:&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");

  fsm = substr(dpName, 0, strpos(dpName, "."));
  int index = dynContains(CSC_fwCAN2_g_STATUS_HANDLERS_FIRST, fsm); // to eliminate .status
  if (index >= 1 ) { // first call of handler
    DebugN("mudcsMrtnUpdateMonitor_PL500_2_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER PL500_2" + " " + dpName);
    dynRemove(CSC_fwCAN2_g_STATUS_HANDLERS_FIRST, index);


/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED"
// to restore the previous system state JUST IN CASE
/////  return;
  }


  DebugN("mudcsMrtnServer::mudcsMrtnUpdateMonitor_PL500_2_status ***************************" + dpName + " = " + value);



  DebugN("mudcsMrtnUpdateMonitor_PL500_2_status:%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " + dpName);


  if ( (value == true && index >= 1) ) {  // -1, -2  is to initialize the state at start

    dpSetWait(fsm + ".General.Commands.OnOffCrate", true);


    DebugN("mudcsMrtnServer::mudcsMrtnUpdateMonitor_PL500_2_status == 2++***************************" + dpName + value);
  } else if (value == false  && index >= 1  /*|| (value==-2 && state_destin==false && index >=1)*/) { //  -2  is to initialize the state at start

/////////////////// if(state_save ==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".status",-2);// this all is to allow to switch off from error state and differ -2 and -1

    dpSetWait(fsm + ".General.Commands.OnOffCrate", false);


    DebugN("mudcsMrtnServer::mudcsMrtnUpdateMonitor_PL500_2_status == 0++***************************" + dpName + value);
  }


  CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT = 0;

}


//============================================================================================
//============================================================================================

mudcsMrtnUpdateMonitor_PL500_2_alert(string dpName, string value) {

// ROLE OF FUNCTION: to call _valueChanged function

  string fsm;
  int current_status;
  dyn_string ch_dps;
  int i;
  bool update_status;

// attention :: the order of power/on/off and setting alarm is essential !!!

// THIS MOSTLY RELATES TO THE CRB:
// TO MY NOTICE: ATTENTION!: IF THE DATA VALUE CHANGES WHEN VALUE IS IN THE ERROR INTERVAL
// (THE _alert_hdl.._act_state_color STAYS "FwAlarmErrorAck")
// NEVERTHELESS THE VALUE IN THIS HANDLER (_alert_hdl.._act_state_color) IS UPDATED !!!!
// I.E. THIS HANDLER IS INVOKED
// THIS IS NOT THE CASE IF DATA VALUE CHANGES WHEN VALUE IS IN THE VALID INTERVAL (HANDLER IS NOT INVOKED)
//


  fsm = substr(dpName, 0, strpos(dpName, "."));
  DebugN("mudcsMrtnUpdateMonitor_PL500_2_alert:&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ", fsm);
  int index = dynContains(CSC_fwCAN2_g_ALERT_HANDLERS_FIRST, fsm); // to eiminate ...
  if (index >= 1 ) { // first call of handler
    DebugN("mudcsMrtnUpdateMonitor_PL500_2_alert:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER PL500_2" + " " + dpName);
    dynRemove(CSC_fwCAN2_g_ALERT_HANDLERS_FIRST, index);

///////  return;
  }

  ch_dps = dpNames(fsm + "/*", "FwWienerChannel");

// DebugTN(ch_dps);

// this is to call  _valueChanged function of channels
  for (i = 1; i <= dynlen(ch_dps); i++) {
    dpGet(ch_dps[i] + ".Actual.CurrentFlag", update_status);
    dpSetWait(ch_dps[i] + ".Actual.CurrentFlag", update_status);
  }

// this is to call  _valueChanged function of crate
  dpGet(fsm + ".General.Status.GetCurrentFlags", update_status);
  dpSetWait(fsm + ".General.Status.GetCurrentFlags", update_status);

  CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT = 0;

}


//============================================================================================


mudcsFsmPanelsCheck() {


//=================================================================

  dyn_string os;
  string o, s, s1, s2, s3;
  int i, j, pos;
///dyn_string panels;
  string p;

  dyn_string d_string1, d_string2;
  string label, o_cut;
//-----------------------------------------------------------------

  dyn_string objs;
  string obj_found;
//==================================================================

  objs = dpNames("*", "_FwFsmObject");


  for (i = 1; i <= dynlen(objs); i++) {

    d_string1 = strsplit(objs[i], "|");
    d_string2 = strsplit(d_string1[dynlen(d_string1)], "/");
    o_cut = d_string2[dynlen(d_string2)];

///  panels=makeDynString(fwUiPnl);


//mudcsDebug(objs[i]);
    os = strsplit(objs[i], "|");
    o = os[dynlen(os)];

////////DebugTN(o);





    s = "_CAN2";
    if ((pos = strpos(o, s)) >= 0 && pos == (strlen(o) - strlen(s))) {

      obj_found = objs[i];
      break;

    }



  } //for

  if (obj_found != "") {

    dpConnect("check_fsm_generated", true, obj_found + ".ui.label");
////  mudcsDebug2(obj_found+".ui.label");

  }
}

//=================================================================
check_fsm_generated(string dpName, string value) {

  dyn_string panels;
  string dir_oper;
  string Component;
  Component = "CMS_CSCfw_LV_MRTN";
  dir_oper = Component;
//=================================
  string fwUiPnl;
  fwUiPnl = dir_oper + "/emufwUi.pnl";


  dyn_string dps = strsplit(dpName, ".");
  string obj_found = dps[1];
///mudcsDebug("updated");
  dpGet(obj_found + ".ui.panels:_original.._value", panels);

  if (!dynContains(panels, fwUiPnl) || !FIRST_FSM_PANELS) {
    if (FIRST_FSM_PANELS)delay(80);
    DebugTN("FSM PANELS ARE SET");
    mudcsLvMrtnFsmPanelsAndFsmAliasesSet();
  }

  FIRST_FSM_PANELS = 1;

}


