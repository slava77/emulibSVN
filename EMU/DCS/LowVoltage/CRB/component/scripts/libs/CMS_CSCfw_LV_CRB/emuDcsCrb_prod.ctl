int new_lv_power_up_sequence = true;
int g_attempts = false;

mudcsCrbInitDelays(int mode) { // used as additional init for server
  int i, j;
  dyn_string dpp = dpNames(CSC_fwCAN1_g_SYSTEM_NAME + ":*ME_P*", "fwCrb_CSC_LV");
  dyn_string dpm = dpNames(CSC_fwCAN1_g_SYSTEM_NAME + ":*ME_M*", "fwCrb_CSC_LV");
  dyn_string dpall = dpp;

  dynAppend(dpall, dpm);

  addGlobal("CSC_fwCAN1_g_CRB_DPS", DYN_STRING_VAR);
  CSC_fwCAN1_g_CRB_DPS = dpall;

  addGlobal("CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY", DYN_INT_VAR);
  j = 0;
  for (i = 1; i <= dynlen(CSC_fwCAN1_g_CRB_DPS); i++) {
    if (strpos(CSC_fwCAN1_g_CRB_DPS[i], "ME_M") >= 0)j++;

    if (mode == 0)CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i] = 0; // in milliseconds
    else  {
      if (new_lv_power_up_sequence) {
        if (j > 0)CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i] = 1000 + (j - 1) * 1300; // in milliseconds
        else CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i] = 1000 + (i - 1) * 1300; // in milliseconds
      } else
        CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i] = (i - 1) * 40; // in milliseconds
    }
  }



}


//=====================================================================

mudcsCrbPnlInit(int level = 1) {

  int i;
  dyn_string ds1;
  string coord;

  addGlobal("CSC_fwCAN1_g_IS_NEW_ATLAS_PSU", BOOL_VAR);
  CSC_fwCAN1_g_IS_NEW_ATLAS_PSU = true;

  addGlobal("CSC_fwCAN1_g_SYSTEM_NAME", STRING_VAR);
  addGlobal("CSC_fwCAN1_g_EMU_ALIASES", DYN_STRING_VAR);
  addGlobal("CSC_fwCAN1_g_ISACK", BOOL_VAR);
  addGlobal("CSC_fwCAN1_g_CANBUSES", DYN_STRING_VAR);
  addGlobal("CSC_fwCAN1_g_csc_part", STRING_VAR);
  CSC_fwCAN1_g_csc_part = "CMS_CSC_LV_CAN1";
//--------------------------------------------------------------------------
  // CSC_fwCAN1_g_SYSTEM_NAME= "CMS_CSC_LV_CAN1";  //fwFsm_getSystemName();
//-------------
  string node_to_define_system_name = CSC_fwCAN1_g_csc_part;
  fwCU_getDp(node_to_define_system_name, node_to_define_system_name);
//-------------
  dyn_string systemNamesDps;
  dyn_string s_split;
  systemNamesDps = dpNames("*:CMS_CSC_LV_CRB");
//   systemNamesDps=dpNames("*:"+node_to_define_system_name);
  if (dynlen(systemNamesDps) == 0)mudcsLvDebug("SYSTEM NAME NOT DEFINED !");
  else {
    s_split = strsplit(systemNamesDps[1], ":");
    CSC_fwCAN1_g_SYSTEM_NAME = s_split[1];//dpSubStr(systemNamesDps[1], DPSUB_SYS);
    ///mudcsLvDebug(CSC_fwCAN1_g_SYSTEM_NAME);
  }
//--------------------------------------------------------------------------
  CSC_fwCAN1_g_ISACK = false;
//------------------------------------------------------



  addGlobal("CSC_fwCAN1_g_LIST_5V", DYN_INT_VAR);
  addGlobal("CSC_fwCAN1_g_LIST_3V1", DYN_INT_VAR);
  addGlobal("CSC_fwCAN1_g_LIST_3V2", DYN_INT_VAR);
  addGlobal("CSC_fwCAN1_g_LIST_15V", DYN_INT_VAR);
  addGlobal("CSC_fwCAN1_g_LIST_CC", DYN_INT_VAR);
  addGlobal("CSC_fwCAN1_g_LIST_MPC", DYN_INT_VAR);
  addGlobal("CSC_fwCAN1_g_LIST_MAIN", DYN_INT_VAR);

  addGlobal("CSC_fwCAN1_g_nodeGuardInterval", DYN_INT_VAR);

  addGlobal("CSC_fwCAN1_g_CRB2ELMB", DYN_STRING_VAR);





  CSC_fwCAN1_g_LIST_5V = makeDynInt(56, 55, 54, 53, 52, 51, 50, 49, 48);
  CSC_fwCAN1_g_LIST_3V1 = makeDynInt(0, 2, 4, 6, 8, 10, 12, 14, 32);
  CSC_fwCAN1_g_LIST_3V2 = makeDynInt(1, 3, 5, 7, 9, 11, 13, 15, 33);
  CSC_fwCAN1_g_LIST_15V = makeDynInt(20, 21);
  CSC_fwCAN1_g_LIST_CC = makeDynInt(35, 19);
  CSC_fwCAN1_g_LIST_MPC = makeDynInt(34, 57);
  CSC_fwCAN1_g_LIST_MAIN = makeDynInt(16, 17, 18);

//---- should be uncomented only for the old PSU version --------
  if (!CSC_fwCAN1_g_IS_NEW_ATLAS_PSU) {
    addGlobal("EPSU_POWER_ON_VALUE", BOOL_VAR);
    EPSU_POWER_ON_VALUE = false;
  }
//---------------------------------------------------------------

  if (level == 0)return;

//----- INIT PART------------------------------------
  CSC_fwCAN1_g_CANBUSES = dpNames(CSC_fwCAN1_g_SYSTEM_NAME + ":*", "FwElmbCANbus");
  for (i = 1; i <= dynlen(CSC_fwCAN1_g_CANBUSES); i++) {
    dpGet(CSC_fwCAN1_g_CANBUSES[i] + ".nodeGuardInterval", CSC_fwCAN1_g_nodeGuardInterval[i]);
  }

  ds1 = dpNames(CSC_fwCAN1_g_SYSTEM_NAME + ":*", "fwCrb_CSC_LV");
  dynClear(CSC_fwCAN1_g_CRB2ELMB);
  for (i = 1; i <= dynlen(ds1); i++) {
    dpGet(ds1[i] + ".coord", coord);
    dynAppend(CSC_fwCAN1_g_CRB2ELMB, ds1[i] + ";" + coord);
  }

//  DebugTN(CSC_fwCAN1_g_CRB2ELMB);

//---------------------------------------------------



  return;

}

//========================================================================
int mudcsCrbGetChannelNumberByName(string dp) {
  int iN;
  dyn_string ds1, ds2;
  ds1 = strsplit(dp, "/");
  ds2 = strsplit(ds1[dynlen(ds1)], "_");
  iN = ds2[dynlen(ds2)];
  return iN;

}
//========================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

//============================================================================================

mudcsCrbCommandCrb(string fsm, bool isOn) {

  string  sTest;
  dyn_string dsPairs, dsRest, dsOffChannels, switch_list;
  dyn_int off_channels;

  int factor_switch = 1;
  if (new_lv_power_up_sequence) {
    if (!isOn)factor_switch = 10;
  }
  int i, j;

  int delay_all, delay_s, delay_ms;

  i = dynContains(CSC_fwCAN1_g_CRB_DPS, fsm);
  if (i >= 1) {
    if (CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i] != 0 ) {
      delay_all = CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i] / factor_switch;
      delay_s = delay_all / 1000;
      delay_ms = delay_all % 1000;
//    delay(0,(CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]/factor_switch));
      delay(delay_s, delay_ms);
    }
    DebugTN("PUT DELAY");
    DebugTN(fsm + " delay=" + CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i] + " " + delay_s + " " + delay_ms);

  } else {
    DebugTN("PUT DELAY NOT FOUND");
  }
//===== reserved new power up sequence ========
  /*
  if(new_lv_power_up_sequence){
   if(isOn){
   string LvPowerUpModule="LV_START_UP_WARNING";

  	if(!isModuleOpen(LvPowerUpModule) ){
  		ModuleOn(LvPowerUpModule,1,1,0,0,1,1,1,"");
  	}
  /////        else PanelOffModule("",MRTNTrendingModule);
  	RootPanelOnModule("CMS_CSCFw_LV_CRB/LV_START_UP_WARNING.pnl", "",LvPowerUpModule,makeDynString());

          dpSetWait("LV_START_UP_CHAMBER.",fsm);
   } //if(isOn)
  } // if(new_lv_power_up_sequence)
  */
//===== end reserved new power up sequence ========
  if (isOn)isOn = false;
  else isOn = true;

  dpGet(fsm + ".switch_list", switch_list);
  dpGet(fsm + ".off_channels", off_channels);

  for (i = 1; i <= dynlen(off_channels); i++)dsOffChannels[i] = off_channels[i];

  sTest = substr(switch_list[1], 0, strpos(switch_list[1], "/do_"));

  dynClear(switch_list);
  dynAppend(dsPairs, sTest + "/do_C_0");
  dynAppend(dsPairs, sTest + "/do_C_1");
  dynAppend(dsPairs, sTest + "/do_C_2");
  dynAppend(dsPairs, sTest + "/do_C_3");
  dynAppend(dsPairs, sTest + "/do_C_4");
  dynAppend(dsPairs, sTest + "/do_C_5");
  dynAppend(dsPairs, sTest + "/do_C_6");
  dynAppend(dsPairs, sTest + "/do_C_7");
  dynAppend(dsPairs, sTest + "/do_A_0");

  dynAppend(dsRest, sTest + "/do_A_1");
  dynAppend(dsRest, sTest + "/do_A_2");
  dynAppend(dsRest, sTest + "/do_A_3");
  dynAppend(dsRest, sTest + "/do_A_4");
//-----------------------------------------------
//==== reserved: to be commented out old power up sequence ====
  if (!new_lv_power_up_sequence) {
    for (j = 1; j <= 4; j++) {

      for (i = 1; i <= dynlen(dsPairs); i++) {
        if (dynContains(dsOffChannels, i))continue;
        dpSet(dsPairs[i] + ".value", isOn);
      }
//----------------------------------------
      for (i = 1; i <= 3; i++) {
        if (dynContains(dsOffChannels, dynlen(dsPairs) + i))continue;
        dpSet(dsRest[i] + ".value", isOn);
      }
//---------------------------------------
      dpSet(dsRest[4] + ".value", false); // clock
      delay(0, 100);
      DebugTN(" " + fsm + " Clock " + 100 + " attempt " + j);
      dpSet(dsRest[4] + ".value", true); // clock
    } // j
  } // if(!new_lv_power_up_sequence)
//==== end of reserved: to be commented out old power up sequence ====

//===== reserved new power up sequence ========
//====== test 33 ms first (as a clock) to use many attempts
  if (new_lv_power_up_sequence) {
    int n_att = 1;
    if (g_attempts)n_att = 3;

    if (isOn == true) { // switch OFF

      for (j = 1; j <= 4; j++) {

        for (i = 1; i <= dynlen(dsPairs); i++) {
          if (dynContains(dsOffChannels, i))continue;
          dpSet(dsPairs[i] + ".value", isOn);
        }
//----------------------------------------
        for (i = 1; i <= 3; i++) {
          if (dynContains(dsOffChannels, dynlen(dsPairs) + i))continue;
          dpSet(dsRest[i] + ".value", isOn);
        }
//---------------------------------------
        dpSet(dsRest[4] + ".value", false); // clock
        delay(0, 100);
        DebugTN(" " + fsm + " Clock " + 100 + " attempt " + j);
        dpSet(dsRest[4] + ".value", true); // clock
      } // j

    } //
    else if (isOn == false) { // switch ON

      int interval;
      /*
      for(i=1;i<=dynlen(dsPairs);i++){
        if(dynContains(dsOffChannels,i))continue;
        dpSet(dsPairs[i]+".value",true); // set to off all the bits at the beginning: just in case
      }
      //----------------------------------------
      for(i=1;i<=3;i++){
        if(dynContains(dsOffChannels,dynlen(dsPairs)+i))continue;
        dpSet(dsRest[i]+".value",true); // set to off all the bits at the beginning: just in case
      }
      */
//----------------------------------------
      int jj;
      for (j = 1; j <= n_att; j++) {
        for (i = 1; i <= 2; i++) {

          if (i == 1)jj = 3; // 1.5
          else if (i == 2)jj = 1; // MPC/CCB
          //else if(i==3)jj=2; // VCC

          if (dynContains(dsOffChannels, dynlen(dsPairs) + jj))continue;
          dpSet(dsRest[jj] + ".value", isOn);


          if (jj == 1)interval = 150 / n_att; // 1 is right! because this is clock (not just delay)
          else interval = 100 / n_att;

          dpSet(dsRest[4] + ".value", false); // clock
          delay(0, interval);
          DebugTN(" " + fsm + " Clock " + interval + " board_cc " + jj);
          dpSet(dsRest[4] + ".value", true); // clock

        } // i
      } // j
//----
      interval = 100;
      interval = interval / n_att;
      for (j = 1; j <= n_att; j++) {
        for (i = 1; i <= dynlen(dsPairs); i++) {
          if (dynContains(dsOffChannels, i))continue;
          dpSet(dsPairs[i] + ".value", isOn);

          dpSet(dsRest[4] + ".value", false); // clock
          delay(0, interval);
          DebugTN(" " + fsm + " Clock " + interval + " board " + i);
          dpSet(dsRest[4] + ".value", true); // clock



        }
      } // j

//=====================
      for (j = 1; j <= n_att; j++) {
        for (i = 1; i <= 1; i++) {

          //if(i==1)jj=2; // CCB
          //else if(i==2)jj=3; // CC
          //else if(i==3)jj=1; // 1.5
          if (i == 1)jj = 2; // CCB


          if (dynContains(dsOffChannels, dynlen(dsPairs) + jj))continue;
          dpSet(dsRest[jj] + ".value", isOn);

          interval = 100 / n_att;
//if(jj==2)interval=150/n_att; // 2 is right! because this is clock (not just delay)
//else interval=100/n_att;

          dpSet(dsRest[4] + ".value", false); // clock
          delay(0, interval);
          DebugTN(" " + fsm + " Clock " + interval + " board_cc " + jj);
          dpSet(dsRest[4] + ".value", true); // clock

        } // i
      } // j
//===================
      interval = 100;


    }//if(isOn==false)

    dpSetWait("LV_START_UP_CHAMBER.", fsm + " is finished");
  } //if(new_lv_power_up_sequence)
//==============================================================
//==============================================================
//==============================================================
  DebugTN(" " + fsm + " Time Measurement Crb");
}

//============================================================================================
mudcsCrbCommandCrbGroup(string fsm, bool isOn, dyn_int boards) {

  string  sTest;
  dyn_string dsPairs, dsRest, dsOffChannels, switch_list;
  dyn_int off_channels;

  int i, j;

  if (isOn)isOn = false;
  else isOn = true;

  dpGet(fsm + ".switch_list", switch_list);
  dpGet(fsm + ".off_channels", off_channels);

  for (i = 1; i <= dynlen(off_channels); i++)dsOffChannels[i] = off_channels[i];

  sTest = substr(switch_list[1], 0, strpos(switch_list[1], "/do_"));

  dynClear(switch_list);
  dynAppend(dsPairs, sTest + "/do_C_0");
  dynAppend(dsPairs, sTest + "/do_C_1");
  dynAppend(dsPairs, sTest + "/do_C_2");
  dynAppend(dsPairs, sTest + "/do_C_3");
  dynAppend(dsPairs, sTest + "/do_C_4");
  dynAppend(dsPairs, sTest + "/do_C_5");
  dynAppend(dsPairs, sTest + "/do_C_6");
  dynAppend(dsPairs, sTest + "/do_C_7");
  dynAppend(dsPairs, sTest + "/do_A_0");

  dynAppend(dsRest, sTest + "/do_A_1");
  dynAppend(dsRest, sTest + "/do_A_2");
  dynAppend(dsRest, sTest + "/do_A_3");
  dynAppend(dsRest, sTest + "/do_A_4");
//-----------------------------------------------

  for (j = 1; j <= 4; j++) {

    for (i = 1; i <= dynlen(dsPairs); i++) {
      if (dynContains(dsOffChannels, i) >= 1)continue;
      if (dynContains(boards, i) >= 1)dpSet(dsPairs[i] + ".value", isOn);
    }
//----------------------------------------
    for (i = 1; i <= 3; i++) {
      if (dynContains(dsOffChannels, dynlen(dsPairs) + i) >= 1)continue;
      if (dynContains(boards, dynlen(dsPairs) + i) >= 1)dpSet(dsRest[i] + ".value", isOn);
    }
//---------------------------------------
    dpSet(dsRest[4] + ".value", false); // clock
    delay(0, 100);
    dpSet(dsRest[4] + ".value", true); // clock

  } //j

}

//============================================================================================
//===================================================
