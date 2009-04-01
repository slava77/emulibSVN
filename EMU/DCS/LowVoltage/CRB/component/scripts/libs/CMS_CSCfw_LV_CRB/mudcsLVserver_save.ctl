#uses "CMS_CSCfw_LV_CRB/emuDcsLv.ctl"
#uses "CMS_CSCfw_LV_CRB/emuDcsCrb.ctl"
//#uses "CMS_CSCfw_LV_CRB/emuDcsMrtn.ctl"
#uses "CMS_CSCfw_LV_CRB/emuDcsLvAlert.ctl"
#uses "CMS_CSCfw_LV_CRB/emuDcsCrbAlert.ctl"
//#uses "CMS_CSCfw_LV_CRB/emuDcsMrtnAlert.ctl"
#uses "CMS_CSCfw_LV_CRB/emufw.ctl"


dyn_string CSC_fwCAN1_g_ALERT_HANDLERS_FIRST;
int CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT;
dyn_string CSC_fwCAN1_g_STATUS_HANDLERS_FIRST;
dyn_string CSC_fwCAN1_g_FULL_DPLIST;

dyn_string psu_power_dps;
dyn_string psu_branch_dps;

bool crb_busy = false;

//dyn_int CSC_fwCAN1_g_nodeGuardInterval;
//dyn_string CSC_fwCAN1_g_CANBUSES;
//dyn_string CSC_fwCAN1_g_CRB2ELMB;

//========================================================
main() {

  mudcsCrbPnlInit();
  mudcsCrbInitDelays(0); // additional for the server
  mudcsCrbUpdate_BrokerList();
  mudcsCrbInitDelays(1); // additional for the server
  startThread("mudcsCrb_threadWatchForTimeouts");
//  while(1){delay(0,1000)};
}


//========================================================
mudcsCrb_threadWatchForTimeouts() {
  int i, j;

  dyn_time current_time;
  dyn_time current_time_save;
  int status, error, current_error_status, status_complex = 0;
  int current_elmb_state_update;
  dyn_int previous_elmb_state_update;

  int status_crb;
  bool status_psu;

  string fsm, fsm1, fsm2, sId, alertClass;
  dyn_string ds1, ds2, ds3;
  dyn_int ds_count, ds_count_1, ds_elmb_hang;
  int max_guard = 0;
  bool FIRST = true;
  for (i = 1; i <= dynlen(CSC_fwCAN1_g_CANBUSES); i++) {
    if (CSC_fwCAN1_g_nodeGuardInterval[i] > max_guard)max_guard = CSC_fwCAN1_g_nodeGuardInterval[i];
  }


  ds1 = dpNames("*", "FwElmbNode"); // fixed when ATLAS PSU is being develped


//=========== init of errors ================
  for (i = 1; i <= dynlen(ds1); i++) {
//     status_complex=0;

    dynAppend(ds_count, 0);
    dynAppend(ds_count_1, 0);

    ds2 = strsplit(ds1[i], "_");
    sId = ds2[dynlen(ds2)];

    fsm = mudcsCrbGetCrbByElmbId(sId);
    if (fsm != "")dpSet(fsm + ".error_status", 0);

  } // for i
  ds2 = dpNames("*", "fwPsu_CSC_LV");
  for (i = 1; i <= dynlen(ds2); i++) {
    dpSet(ds2[i] + ".error_status", 0);
  } // for i

//============================================================

  delay((max_guard / 1000) + 15); // max_guard is in milliseconds (+15 is just in case at the beginning)
//----------------------------
// ds1=dpNames("*","FwElmbNode"+max_guard);


  while (1) { // indefinite loop: never broken
    delay((max_guard / 1000));
    DebugTN("GUARDING WITH INTERVAL=" + max_guard);
    for (i = 1; i <= dynlen(ds1); i++) {


      ds2 = strsplit(ds1[i], "_");
      sId = ds2[dynlen(ds2)];
      fsm1 = mudcsCrbGetCrbByElmbId(sId);

//     status_complex=0;
      dpGet(ds1[i] + ".state.value", status);
      dpGet(ds1[i] + ".error", error);
      status_complex = 0;
      if (status != 5 && status != 133)status_complex |= 2048; // bit 1 is asserted

      if (FIRST) {
        FIRST = FIRST;
        current_time_save[i] = getCurrentTime();
      } // FIRST
      else {
        current_time[i] = getCurrentTime();
        int time_interval = current_time[i] - current_time_save[i];
        current_time_save[i] = getCurrentTime();
        DebugTN("Time Interval=" + time_interval);

        dpGet(ds1[i] + ".state.value:_online.._stime", current_elmb_state_update);
        DebugTN(current_elmb_state_update);
        DebugTN(previous_elmb_state_update[i]);
        fsm = mudcsCrbGetCrbByElmbId(sId);

        if (current_elmb_state_update - previous_elmb_state_update[i] < 2) { // elmb is frozen
          if (ds_count_1[i] >= 10)ds_count_1[i]++;
          else ds_count_1[i] = 10;
          if (ds_count_1[i] >= 11) {
            status_complex |= 4096; // bit 2 is asserted
            if (current_elmb_state_update != 0)DebugTN("Time-out Alert!!!!!  " + fsm + "   " + ds1[i] + " " + status_complex);
          }
        } // if
        /*
               if(current_elmb_state_update-previous_elmb_state_update[i] < 2) { // elmb is frozen
                 ds_elmb_hang[i]++;
                 if( (ds_elmb_hang[i] >1 && fsm != "") || (ds_elmb_hang[i] >4 && fsm == "") ){
                  status_complex |= 4096; // bit 2 is asserted
                  if(current_elmb_state_update !=0)DebugTN("Time-out Alert!!!!!  "+ds1[i]+" "+status_complex);
                 }
                 else if(current_elmb_state_update !=0)DebugTN("Time-out Warning!!!!!  "+ds1[i]+" "+status_complex);
               } // if
        */
        else { // this piece is to restore the statuses after power cycling (power cut) of ELMBS
          //fsm=mudcsCrbGetCrbByElmbId(sId);


          if (fsm == "") {
            for (j = 1; j <= dynlen(psu_power_dps); j++) {
              dpGet(psu_branch_dps[j] + ".:_alert_hdl.._act_state_color", alertClass);
              //if(alertClass=="FwAlarmErrorAck"){
              fsm2 = ds1[i];
              strreplace(fsm2, "/", "_");
              dpGet(fsm2 + ".error_status", current_error_status);
              if (current_error_status > 0)ds_count[i] = 3;
              if (ds_count[i] > 0) {
//     DebugTN(fsm2+" +++++++++++++++++++++++++ "+current_error_status+" "+ds_count[i]);
                dpGet(psu_power_dps[j], status_psu);
                dpSetWait(psu_power_dps[j], status_psu);

//     DebugTN(fsm2+" -------------------------- "+current_error_status+" "+ds_count[i]);
              }
              // }
            }
            ds_count[i]--;
            DebugTN(fsm2 + " --------------------------+ " + current_error_status + " " + ds_count[i]);
          } else {
            dpGet(fsm + ".:_alert_hdl.._act_state_color", alertClass);
            //  if(alertClass=="FwAlarmErrorAck"){
            dpGet(fsm1 + ".error_status", current_error_status);
            if (current_error_status > 0)ds_count[i] = 3;
            if (ds_count[i] > 0) {
              dpGet(fsm + ".status", status_crb);
              dpSetWait(fsm + ".status", status_crb);
            }
            ds_count[i]--;
            DebugTN(fsm + " -------------------------- " + current_error_status + " " + ds_count[i]);
            // }
          }
          ds_count_1[i] = 0;
        } // else
      } // else !FIRST
      dpGet(ds1[i] + ".state.value:_online.._stime", previous_elmb_state_update[i]);




      if (fsm1 == "") {
        fsm2 = ds1[i];
        strreplace(fsm2, "/", "_");
        dpSet(fsm2 + ".error_status", status_complex);// case of elmb of PSU (i.e. not elmb of CRB)

      } else dpSet(fsm1 + ".error_status", status_complex);

    } // for i
    if (FIRST) {
      FIRST = false;
    } // FIRST
  } // while(1)
}
//========================================================
string mudcsCrbGetCrbByElmbId(string sId) {

//  DebugTN("########"+sId);
  int i;
  dyn_string ds1;
  for (i = 1; i <= dynlen(CSC_fwCAN1_g_CRB2ELMB); i++) {
    ds1 = strsplit(CSC_fwCAN1_g_CRB2ELMB[i], ";");
    if (ds1[2] == sId)return ds1[1];
  }
  return ""; // case elmb of PSU (i.e. not elmb of CRB)
}
//========================================================

mudcsCrbUpdate_BrokerList() {

  int j;
  dyn_string d_s1, d_s2;
  string update_fun_alert, update_fun_status;

  mudcsCrbCanControl(1); // running all ELMB-s (just in case)

//dpSet("dyn_debug1.",makeDynString());
//dpSet("dyn_debug2.",makeDynString());


  DebugN(" INFO, mudcsCrbServer: init start***************************");

  dyn_string BrokerList, BrokerList_old;
  string service, update_fun_alert, update_fun_status;
  dyn_string datapoints;


//-------------CRB PART----------------------------------------------------------
  BrokerList = makeDynString("fwCrb_CSC_LV"); // add

  for (j = 1; j <= dynlen(BrokerList); j++) {

    service = BrokerList[j];// add

    update_fun_alert = "mudcsCrbUpdateMonitor_" + service + "_alert";
    update_fun_status = "mudcsCrbUpdateMonitor_" + service + "_status";
    datapoints = dpNames("*", service);

    for (int i = 1; i <= dynlen(datapoints); i++) { // corr

      dynAppend(CSC_fwCAN1_g_ALERT_HANDLERS_FIRST, datapoints[i]);

      CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT = 1;
      dpConnect(update_fun_alert, datapoints[i] + ".:_alert_hdl.._act_state_color");  //:_alert_hdl.._active
      while (CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT);

    } //i

    for (int i = 1; i <= dynlen(datapoints); i++) {
      dynAppend(CSC_fwCAN1_g_STATUS_HANDLERS_FIRST, datapoints[i]);
      dynAppend(CSC_fwCAN1_g_FULL_DPLIST, datapoints[i]);


      CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT = 1;
      dpConnect(update_fun_status, datapoints[i] + ".status");
      while (CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT);

    } //i

//-----

  } //j
//--------------- END CRB PART -----------------------------------------------------
//dynClear(CSC_fwCAN1_g_ALERT_HANDLERS_FIRST,datapoints);
//dynClear(CSC_fwCAN1_g_STATUS_HANDLERS_FIRST,datapoints);
//-------------ATLAS PSU PART----------------------------------------------------------
  BrokerList = makeDynString("FwElmbPSUBranch"); // add

  for (j = 1; j <= dynlen(BrokerList); j++) {

    service = BrokerList[j];// add

    update_fun_alert = "mudcsPsuUpdateMonitor_" + service + "_alert";
    update_fun_status = "mudcsPsuUpdateMonitor_" + service + "_status";
    datapoints = dpNames("*", service);

    for (int i = 1; i <= dynlen(datapoints); i++) { // corr

      dynAppend(CSC_fwCAN1_g_ALERT_HANDLERS_FIRST, datapoints[i]);

      CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT = 1;
      dpConnect(update_fun_alert, datapoints[i] + ".:_alert_hdl.._act_state_color");  //:_alert_hdl.._active
      while (CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT);

    } //i

    for (int i = 1; i <= dynlen(datapoints); i++) {
      string sPowerDpe, sPowerDpeShort;
      dpGet(datapoints[i] + ".power", sPowerDpe);
      sPowerDpeShort = substr(sPowerDpe, 0, strpos(sPowerDpe, "."));
      dynAppend(CSC_fwCAN1_g_STATUS_HANDLERS_FIRST, mudcsLvAddSystem(sPowerDpeShort));
      dynAppend(CSC_fwCAN1_g_FULL_DPLIST, datapoints[i]);

      CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT = 1;
//        dpConnect(update_fun_status,datapoints[i]+".bus");
      dynAppend(psu_power_dps, mudcsLvAddSystem(sPowerDpe));
      dynAppend(psu_branch_dps, mudcsLvAddSystem(datapoints[i]));
      dpConnect(update_fun_status, sPowerDpe);
      while (CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT);

    } //i

//-----

  } //j
//--------------- END ATLAS PSU PART -----------------------------------------------------

//-------------------------------------------
  DebugN("mudcsCrbServer: init end***************************");

}
//============================================================================================






//============================================================================================
mudcsCrbUpdateMonitor_fwCrb_CSC_LV_status(string dpName, int value) {

//string mudcs_alias;

  string fsm;

  while (crb_busy);

// attention :: the order of power/on/off and setting alarm is essential !!!
  DebugN("mudcsCrbUpdateMonitor_fwCrb_CSC_LV_status:&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");

  fsm = substr(dpName, 0, strpos(dpName, "."));
  int index = dynContains(CSC_fwCAN1_g_STATUS_HANDLERS_FIRST, fsm); // to eliminate .status
  if (index >= 1 ) { // first call of handler
    DebugN("mudcsCrbUpdateMonitor_fwCrb_CSC_LV_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER fwCrb_CSC_LV" + " " + dpName);
    dynRemove(CSC_fwCAN1_g_STATUS_HANDLERS_FIRST, index);


/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED"
// to restore the previous system state JUST IN CASE
/////  return;
  }


  DebugN("mudcsCrbServer::mudcsCrbUpdateMonitor_fwCrb_CSC_LV_status ***************************" + dpName + " = " + value);


// !!!! COMMENT: MULTY_MODE_DONE.value is set true when MUDCS_DOMAIN.value is defined
// !!!! it is needed to call all;all commands only once.
// !!!! (multy-mode is not used now: see below).


  DebugN("mudcsCrbUpdateMonitor_fwCrb_CSC_LV_status:%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " + dpName);


  if (value == 2 || (value == -1 && index >= 1) ) {  // -1, -2  is to initialize the state at start

///    dynAppend(d_s2,"11111"+dpName);
///    dpSetWait("dyn_debug2.",d_s2);

    mudcsCrbCommandCrb(fsm, true);
    startThread("mudcsCrb_threadAlertActiveSet", fsm + ".:_alert_hdl.._active", true);
    //dpSetWait(fsm+".:_alert_hdl.._active",true);

    DebugN("mudcsCrbServer::mudcsCrbUpdateMonitor_fwCrb_CSC_LV_status == 2++***************************" + dpName + value);
  } else if (value == 0 /*|| (value==-2 && state_destin==false && index >=1)*/) { //  -2  is to initialize the state at start

/////////////////// if(state_save ==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".status",-2);// this all is to allow to switch off from error state and differ -2 and -1
    bool bIsActive;
    dpGet(fsm + ".:_alert_hdl.._active", bIsActive);
    if (bIsActive) {
      if (CSC_fwCAN1_g_ISACK)mudcsLvGroupAcknowlege(fsm);
      dpSetWait(fsm + ".:_alert_hdl.._active", false);
      /*else*/
    } // if(bIsActive)
    mudcsCrbCommandCrb(fsm, false);
///  if(CSC_fwCAN1_g_ISACK)mudcsLvGroupAcknowlege(fsm);
//       startThread("mudcsCrb_threadAlertActiveSet",fsm+".:_alert_hdl.._active",false);

//       dpSetWait(fsm+".:_alert_hdl.._active", false);

    DebugN("mudcsCrbServer::mudcsCrbUpdateMonitor_fwCrb_CSC_LV_status == 0++***************************" + dpName + value);
  }


  CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT = 0;

}


//============================================================================================











// insert globals ??????



//============================================================================================
mudcsCrbUpdateMonitor_fwCrb_CSC_LV_alert(string dpName, string value) {

//
// TO MY NOTICE: ATTENTION!: IF THE DATA VALUE CHANGES IN THE ERROR INTERVAL
// (THE _alert_hdl.._act_state_color STAYS "FwAlarmErrorAck")
// NEVERTHELESS THE VALUE IN THIS HANDLER (_alert_hdl.._act_state_color) IS UPDATED !!!!
// I.E. THIS HANDLER IS INVOKED
// THIS IS NOT THE CASE IF DATA VALUE CHANGES IN THE VALID INTERVAL (HANDLER IS NOT INVOKED)
//

  string fsm;
  int current_status;

// attention :: the order of power/on/off and setting alarm is essential !!!


  fsm = substr(dpName, 0, strpos(dpName, "."));
  DebugN("mudcsCrbUpdateMonitor_fwCrb_CSC_LV_alert:&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ", fsm);
  int index = dynContains(CSC_fwCAN1_g_ALERT_HANDLERS_FIRST, fsm); // to eiminate ...
  if (index >= 1 ) { // first call of handler
    DebugN("mudcsCrbUpdateMonitor_fwCrb_CSC_LV_alert:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER fwCrb_CSC_LV" + " " + dpName);
    dynRemove(CSC_fwCAN1_g_ALERT_HANDLERS_FIRST, index);

///////  return;
  }

  /*
  dyn_string d_s2;
      dpGet("dyn_debug2.",d_s2);
      dynAppend(d_s2,dpName);
      dpSetWait("dyn_debug2.",d_s2);
  */


  bool bIsActive;
  dpGet(fsm + ".:_alert_hdl.._active", bIsActive);

// I understand that change of ._alert_hdl.._act_state_color can be only if bIsActive is true :: it is not so !!!
  DebugTN("##########################====== " + dpName);
  DebugTN("##########################====== " + fsm + ".:_alert_hdl.._active" + " " + value + " " + bIsActive);

  if (bIsActive) {
    dpGet(fsm + ".status", current_status);
    DebugTN("########################## " + value + " " + bIsActive);
    if (value == "FwAlarmErrorAck") {
      if (current_status != -1)dpSet(fsm + ".status", -1);
    } else {
      if (current_status != 2)dpSet(fsm + ".status", 2);
    }
  } // if(bIsActive)
  else { // !bIsActive

  }


  CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT = 0;

}


//============================================================================================



mudcsCrbCanControl(int iAction) {

  dyn_string dsBuses;
  int i;

// 1 -start
// 2 - stop
// 129 - reset
  string sDpe;

  dsBuses = dpNames("*", "FwElmbCANbus");

  for (i = 1; i <= dynlen(dsBuses); i++) {

    sDpe = dsBuses[i] + ".management";

// Executable Code
// ---------------
    // Action the command
    dpSet(sDpe, iAction);
  } // for i


}
//=============================================================================================

mudcsCrb_threadAlertActiveSet(string dpe, bool isActive) {
  int i;
  crb_busy = true;
  for (i = 0; i < 200; i++)
    delay(0, 40);// delay is needed not to generate alert if the device is being switched ON (and not zero data is not come yet)
  //delay(4); // delay is needed not to generate alert if the device is being switched ON (and not zero data is not come yet)
  dpSetWait(dpe, isActive);
  string bus, alert_state;
  string fsm = substr(dpe, 0, strpos(dpe, ".:_alert_hdl.._active"));
  if (strpos(fsm, "PSU") >= 0) {
    dpGet(fsm + ".bus", bus);
    dpSetWait(fsm + ".bus", bus);
  } else { // .i.e CRBs
    // this is to fix situations when the switching ON is made from error atate
    // (so the alert handler is not called automatically (alert state does not change))
    // comment: the below update does not work: the handler is not invoked !
    //if(isActive){
    // DebugTN(fsm+".:_alert_hdl.._act_state_color"+" ????????????????????????????????????????????=======================================================================");
    // dpGet(fsm+".:_alert_hdl.._act_state_color", alert_state);
    // dpSetWait(fsm+".:_alert_hdl.._act_state_color", alert_state);
    //}
  }
  crb_busy = false;
}
//=============================================================================================
//=============================================================================================
//=============================================================================================
//=============================================================================================
//=============================================================================================
//=============================================================================================
//============================= PSU HANDLERS    ===============================================
//=============================================================================================
//=============================================================================================
//=============================================================================================
//=============================================================================================
//=============================================================================================
//=============================================================================================
//=============================================================================================

mudcsPsuUpdateMonitor_FwElmbPSUBranch_status(string dpName, int value) {

// ROLE OF THE FUNCTION: TO RESTORE THE STATUSES AFTER PVSS PROJECT RESTART

  string fsm;
  int pos;
  string sPowerDpe;
// attention :: the order of power/on/off and setting alarm is essential !!!
  DebugN("mudcsCrbUpdateMonitor_fwElmbPSUBranch_status:&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");



  fsm = substr(dpName, 0, strpos(dpName, "."));
  sPowerDpe = fsm + ".value";
//DebugTN(fsm);
//DebugTN(CSC_fwCAN1_g_STATUS_HANDLERS_FIRST);
  int index = dynContains(CSC_fwCAN1_g_STATUS_HANDLERS_FIRST, fsm); // to eliminate .status
  if (index >= 1 ) { // first call of handler
    DebugTN("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ " + index);
    DebugN("mudcsCrbUpdateMonitor_fwElmbPSUBranch_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER fwElmbPSUBranch" + " " + dpName);
    dynRemove(CSC_fwCAN1_g_STATUS_HANDLERS_FIRST, index);


/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED"
// to restore the previous system state JUST IN CASE
/////  return;
  }

  fsm = mudcsLvAddSystem(fsm);
//DebugTN(psu_power_dps+" ====================================================== "+fsm);
  pos = dynContains(psu_power_dps, fsm + ".value");
  fsm = psu_branch_dps[pos];

  DebugN("mudcsCrbServer::mudcsCrbUpdateMonitor_fwElmbPSUBranch_status ***************************" + dpName + " = " + value);

  DebugN("mudcsCrbUpdateMonitor_fwElmbPSUBranch_status:%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " + dpName);


  if (value == (EPSU_POWER_ON_VALUE)) {  //

///    dynAppend(d_s2,"11111"+dpName);
///    dpSetWait("dyn_debug2.",d_s2);

    if (index >= 1 ) {
      DebugTN(sPowerDpe + "||||||||||||||||||||||||||");
      dpSet(sPowerDpe, EPSU_POWER_ON_VALUE);
    }
// commented otherwise indefinit loop       dpSetWait(DpName,FALSE); // inversed and is used only at the project start
    startThread("mudcsCrb_threadAlertActiveSet", fsm + ".:_alert_hdl.._active", true);

//------------------- to call FSM state handler of the fwElmbPSUBranch FSM device type-------------

//------------------------------------
    DebugN("mudcsCrbServer::mudcsCrbUpdateMonitor_fwElmbPSUBranch_status == 2++***************************" + dpName + value);
  } else if (value == (!EPSU_POWER_ON_VALUE)) { //

    if (index >= 1 ) {

      dpSet(sPowerDpe, (!EPSU_POWER_ON_VALUE));
    }

    bool bIsActive;
    dpGet(fsm + ".:_alert_hdl.._active", bIsActive);
    DebugTN(fsm + ".:_alert_hdl.._active=========================== " + bIsActive);
    if (bIsActive) {
      if (CSC_fwCAN1_g_ISACK)mudcsLvGroupAcknowlege(fsm);
//  DebugTN(fsm+".:_alert_hdl.._active>>>>>>>>>>>>>>>>>>>>>>>>============== "+bIsActive);
      dpSetWait(fsm + ".:_alert_hdl.._active", false);
//dpGet(fsm+".:_alert_hdl.._active",bIsActive);
//DebugTN(fsm+".:_alert_hdl.._active>>>>>>>>>>>>>>>>>>>>>>>>============== "+bIsActive);
    } // if(bIsActive)
//  commented otherwise indefinit loop    dpSetWait(DpName,TRUE); // inversed and is used only at the project start

//------------------- to call FSM state handler of the fwElmbPSUBranch FSM device type-------------
    string bus;
    dpGet(fsm + ".bus", bus);
    dpSetWait(fsm + ".bus", bus);
//------------------------------------
    DebugN("mudcsCrbServer::mudcsCrbUpdateMonitor_fwElmbPSUBranch_status == 0++***************************" + dpName + value);
  }

  CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT = 0;

}


//============================================================================================


//============================================================================================
mudcsPsuUpdateMonitor_FwElmbPSUBranch_alert(string dpName, string value) {

//
// TO MY NOTICE: ATTENTION!: IF THE DATA VALUE CHANGES IN THE ERROR INTERVAL
// (THE _alert_hdl.._act_state_color STAYS "FwAlarmErrorAck")
// NEVERTHELESS THE VALUE IN THIS HANDLER (_alert_hdl.._act_state_color) IS UPDATED !!!!
// I.E. THIS HANDLER IS INVOKED
// THIS IS NOT THE CASE IF DATA VALUE CHANGES IN THE VALID INTERVAL (HANDLER IS NOT INVOKED)
//




  string fsm;
  int current_status;

// attention :: the order of power/on/off and setting alarm is essential !!!



  fsm = substr(dpName, 0, strpos(dpName, "."));
  DebugN("mudcsCrbUpdateMonitor_fwElmbPSUBranch_alert:&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ", fsm);
  int index = dynContains(CSC_fwCAN1_g_ALERT_HANDLERS_FIRST, fsm); // to eiminate ...
  if (index >= 1 ) { // first call of handler
    DebugN("mudcsCrbUpdateMonitor_fwElmbPSUBranch_alert:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER fwElmbPSUBranch" + " " + dpName);
    dynRemove(CSC_fwCAN1_g_ALERT_HANDLERS_FIRST, index);

///////  return;
  }

  /*
  dyn_string d_s2;
      dpGet("dyn_debug2.",d_s2);
      dynAppend(d_s2,dpName);
      dpSetWait("dyn_debug2.",d_s2);
  */



//------------------- to call FSM state handler of the fwElmbPSUBranch FSM device type-------------
  string bus;
  dpGet(fsm + ".bus", bus);
  dpSetWait(fsm + ".bus", bus);
//------------------------------------




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

  CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT = 0;

}


//============================================================================================
