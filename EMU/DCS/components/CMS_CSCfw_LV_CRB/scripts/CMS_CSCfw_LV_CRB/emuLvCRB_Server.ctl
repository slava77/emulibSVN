#uses "CMS_CSCfw_LV_CRB/emuDcsCrb.ctl"
#uses "CMS_CSCfw_LV_CRB/emuLvCRB.ctl" 
//=========constant======================================================================== 
bool automatic_stop_pcrate_slow_control=true;
bool two_way_communications_x2p=true;
bool connected_minus_side=false;
bool connected_plus_side=false;
int  FIRST_FSM_PANELS=0;
int  CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT;
dyn_string CSC_fwCAN1_g_ALERT_HANDLERS_FIRST;
dyn_string CSC_fwCAN1_g_STATUS_HANDLERS_FIRST;
dyn_string CSC_fwCAN1_g_FULL_DPLIST;
dyn_string psu_power_dps,psu_branch_dps,all_crb_dps;
dyn_bool   crb_busy;
dyn_string dsCommandBuffers;  //for new crb power up 
bool bBufferEmpty;
//==========================================================================================
main()
{
  DebugTN("CRB monitor script starts"); 
  mudcsCrbPnlInit(); 
  emuLvCRB_initalizeParam();//get global system name for CRB: gSystemNameCRB with ":"
  emuLvCRB_initalizeCrbBuffers(); //create dps for new CRB power up 
  mudcsCrbInitDelays(0); // set all CRBs delay to 0 
//------------ reset crb_busy----------------------------
  all_crb_dps = dpNames(gSystemNameCRB+"*CRB*","fwCrb_CSC_LV");   
  for(int i=1;i<=dynlen(all_crb_dps);i++)
   {  
    crb_busy[i]=false;
   }
//-------- set delay time for crb -----------------------  
  mudcsCrbInitDelays(1);     
//-------- set dp connection for crb and psu ------------
  mudcsCrbUpdate_BrokerList();  

  dpConnect("dist_connections_detect","_DistConnections.Dist.ManNums");
  
  string sValue;
  dpGet(gSystemNameCRB+"ALERT_CONFIG_CHANGED_DP.",sValue);
  if(sValue != "")
   {
     dpConnect("alert_config_changed_dp","ALERT_CONFIG_CHANGED_DP.");  
   }
  
  startThread("mudcsCrb_threadWatchForTimeouts");
  
//+++++++++watdog for new CRB power up++++++++++++++++++++++++++++  
  startThread("emuLvCRB_monitorCrbBuffers");  
//+++++++++for boards control+++++++++++++++++++++++++++++++++++++
  dpConnect("emuLvCRB_boardControl",gSystemNameCRB+"CRB_Boards_Control."); 
}
//========================================================
void dist_connections_detect(string dpName, dyn_int dists)
{
  DebugTN("_DistConnections.Dist.ManNums = "+dists);                          
  delay(15);
  mudcsCrbPnlInit();
  emuLvCRB_showDebug(bDebug,"_CSC_fwCAN1_g_PLUS_SYSTEM_NAME="+CSC_fwCAN1_g_PLUS_SYSTEM_NAME); 
  emuLvCRB_showDebug(bDebug,"_CSC_fwCAN1_g_MINUS_SYSTEM_NAME="+CSC_fwCAN1_g_MINUS_SYSTEM_NAME);
  if(CSC_fwCAN1_g_PLUS_SYSTEM_NAME!="" && !connected_plus_side)
   {      
     dpConnect("update_confirmation",FALSE,CSC_fwCAN1_g_PLUS_SYSTEM_NAME+":LV_CONFIRMATION_SERVICE.value");
     update_confirmation(CSC_fwCAN1_g_PLUS_SYSTEM_NAME+":LV_CONFIRMATION_SERVICE.value", "SOFT_START");
     connected_plus_side=true;
   }
  if(CSC_fwCAN1_g_MINUS_SYSTEM_NAME!="" && !connected_minus_side)
   {
     dpConnect("update_confirmation",FALSE,CSC_fwCAN1_g_MINUS_SYSTEM_NAME+":LV_CONFIRMATION_SERVICE.value");
     update_confirmation(CSC_fwCAN1_g_MINUS_SYSTEM_NAME+":LV_CONFIRMATION_SERVICE.value", "SOFT_START");
     connected_minus_side=true;
   }    
}
//=============================================================================================================
void alert_config_changed_dp(string dpName, string value)
{  
  dpConnect("emuLvCRB_monitorCrbStatus",value+".:_alert_hdl.._act_state_color");  
}
//==============================================================================================================
void mudcsCrb_threadWatchForTimeouts()
{  
  dyn_time current_time,current_time_save;
  dyn_string ds1,ds2,ds3,ELMB_ERRORS;
  dyn_int previous_elmb_state_update,ds_elmb_hang;  
  dyn_int ds_count,ds_count_1,ds_count_error1,ds_count_error2,ds_count_error3;
  bool status_psu,FIRST=true;     
  string fsm, fsm1, fsm2, sId, alertClass; 
  int i,j,status_crb,ServerState,status,error,current_error_status,current_elmb_state_update;
  int max_guard=0;
  int general_count=0;
  int count_opc_on=0;
  int status_complex=0;
  
  DebugTN("start watchdog for emu dcs CRB"); 
  for(i=1;i<=dynlen(CSC_fwCAN1_g_CANBUSES);i++)
   {
    if(CSC_fwCAN1_g_nodeGuardInterval[i]>max_guard)     
     { 
      max_guard=CSC_fwCAN1_g_nodeGuardInterval[i];
     }      
   } 
  if(max_guard==0)  
   { 
     max_guard=1000;
   }  
//------------ init of errors --------------------------------------------
  ds1=dpNames(gSystemNameCRB+"*ELMB*","FwElmbNode");    
  for(i=1;i<=dynlen(ds1);i++)
   {
     dynAppend(ds_count,0);
     dynAppend(ds_count_1,0);
     dynAppend(ds_count_error1,0);
     dynAppend(ds_count_error2,0);
     dynAppend(ds_count_error3,0);
                             
     ds2=strsplit(ds1[i],"_");
     sId=ds2[dynlen(ds2)];              
     fsm=mudcsCrbGetCrbByElmbId(sId); //crb,include elmb for PSU
     if(fsm!="")
      {
        dpSet(fsm+".error_status",0);
      } 
   }  
  ds2=dpNames(gSystemNameCRB+"*ELMB*","fwPsu_CSC_LV");
  for(i=1;i<=dynlen(ds2);i++)
   {
     dpSet(ds2[i]+".error_status",0);       
   }       
//------------indefinite loop: never broken ------------------------------------------------------- 
  delay((max_guard/1000)+15); // max_guard is in milliseconds (+15 is just in case at the beginning) 
  int iCountTimeOut = 0;
  while(1)
   { 
     iCountTimeOut ++;
     delay((max_guard/1000));
     emuLvCRB_showDebug(bDebug,"mudcsCrb_threadWatchForTimeouts"); 
     if(iCountTimeOut >= 20){
       DebugTN("watch for time out...");
       iCountTimeOut = 0;
     }  
//------------------ for x2p ---------------------------------------------------------------------
     if(automatic_stop_pcrate_slow_control)  
      {
        mudcsCrb_resume_slow_control();
      }      
//------------------ check opc and elmb node status ----------------------------------------------  
     dpGet("_OPCCANopen.ServerState",ServerState);
     general_count++;
     if(ServerState==1)
      {count_opc_on++;}      
     emuLvCRB_showDebug(bDebug,"GUARDING WITH INTERVAL="+max_guard+" general_count="+general_count);
     for(i=1;i<=dynlen(ds1);i++)
      {     
       ds2=strsplit(ds1[i],"_");
       sId=ds2[dynlen(ds2)];
       fsm1=mudcsCrbGetCrbByElmbId(sId); //crb,=fsm                
       dpGet(ds1[i]+".state.value",status);
       dpGet(ds1[i]+".error",error);     
       status_complex=0;
       if(status != 5 && status != 133)
        { 
         if(ServerState==1)
           {
             ds_count_error2[i]++;  
             status_complex |= 2048;
           } // bit 1 is asserted 
        }
       if(FIRST)
        {
           FIRST=FIRST;
           current_time_save[i]=getCurrentTime();
        } // FIRST
       else
        {
           current_time[i]=getCurrentTime();
           int time_interval= current_time[i]-current_time_save[i];
           current_time_save[i]=getCurrentTime();  
           emuLvCRB_showDebug(bDebug,"Time Interval="+time_interval);
           dpGet(ds1[i]+".state.value:_online.._stime",current_elmb_state_update); 
           emuLvCRB_showDebug(bDebug,"current status updated at "+current_elmb_state_update);
           emuLvCRB_showDebug(bDebug,"previous_status updated at "+previous_elmb_state_update[i]);    
           fsm=mudcsCrbGetCrbByElmbId(sId);
           if(current_elmb_state_update-previous_elmb_state_update[i] < 2)
              { // elmb is frozen
                 if(ServerState==1) 
                  {ds_count_error1[i]++;}                   
                 if(ds_count_1[i]>=10)
                  {ds_count_1[i]++;}                   
                 else                 
                  {ds_count_1[i]=10;}                  
                 if(ds_count_1[i]>=14)
                  {
                   ds_count_error3[i]++;
                   status_complex |= 4096; // bit 2 is asserted
                   if(current_elmb_state_update !=0)
                      {DebugTN("Time-out Alert at CRB "+fsm);}
                  }
              } // if       
           else                         
              {
                 dyn_string ds_alerts, chambers_fsms; 
                 dyn_int alert_channels;            
                 mudcsCrbGetDmbTempAlerts(fsm, ds_alerts, alert_channels,chambers_fsms);
                 if(dynlen(alert_channels)>0)
                    {status_complex |= 8192;}
                 ds_count_1[i]=0;          
              }
        }// else !FIRST          
       dpGet(ds1[i]+".state.value:_online.._stime",previous_elmb_state_update[i]);
       if(fsm1=="")
        {
          fsm2=ds1[i];
          strreplace(fsm2,"/","_");
          dpSet(fsm2+".error_status",status_complex);// case of elmb of PSU (i.e. not elmb of CRB)          
        }   
       else 
         {dpSet(fsm1+".error_status",status_complex);}
           
       ELMB_ERRORS[i]=ds_count_error1[i]+";"+ds_count_error2[i]+";"+ds_count_error3[i]+";"+fsm+";"+sId;
      } // for i
   if(FIRST)
    {
      FIRST=false;
    } // FIRST    
   emuLvCRB_showDebug(bDebug,"status_complex = "+status_complex);   
   dpSetWait(gSystemNameCRB+"ELMB_ERRORS.",ELMB_ERRORS);
   dpSetWait(gSystemNameCRB+"GENERAL_COUNT.",general_count);
   dpSetWait(gSystemNameCRB+"COUNT_OPC_ON.",count_opc_on);      
  } // while(1)
}
//===================================================================================
/**
 * get Crb dpName from Elmb ID
 * @param string sID: id of Elmb
 * @return string: Crb dpName corresponding to Elmb of crb,or "" for Elmb of Psu
 */
string mudcsCrbGetCrbByElmbId(string sId)
{
  dyn_string ds1;
  for(int i=1;i<=dynlen(CSC_fwCAN1_g_CRB2ELMB);i++)
   {
    ds1=strsplit(CSC_fwCAN1_g_CRB2ELMB[i],";"); 
    if(ds1[2]==sId)
      {return ds1[1];}
   }
  return ""; // case elmb of PSU 
}
//==================set dp connection for Crb and Psu==================================
void mudcsCrbUpdate_BrokerList()
{
  int i,j;
  dyn_string datapoints; 
//-------------set all elmbs to operating mode---------------------------
  emuLvCRB_sendCanBusCommand(1); 
  DebugTN("init start...");
//---------------------CRB PART------------------------------------------
  datapoints = dpNames(gSystemNameCRB+"*CRB*","fwCrb_CSC_LV");
  for(i=1;i<=dynlen(datapoints);i++)
    { 
       dynAppend(CSC_fwCAN1_g_ALERT_HANDLERS_FIRST,datapoints[i]);        
       CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT=1;
       dpConnect("emuLvCRB_monitorCrbStatus",datapoints[i]+".:_alert_hdl.._act_state_color");
       while(CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT);      
    }
//---------------------for deactive alert handle of CRB--------------------
  for(i=1;i<=dynlen(datapoints);i++)
    {
       dpConnect("emuLvCRB_deactiveAlertHandle",datapoints[i]+".command"); 
    }
// ------------------previous CRB power up--------------------------------------
/*  
  for(i=1;i<=dynlen(datapoints);i++)
    {
       dynAppend(CSC_fwCAN1_g_STATUS_HANDLERS_FIRST,datapoints[i]);
       dynAppend(CSC_fwCAN1_g_FULL_DPLIST,datapoints[i]);
       CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT=1;
       dpConnect("emuLvCRB_monitorCrbCommand",datapoints[i]+".command");
       while(CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT); 
    }
*/
//--------------------------------------------------------------------------------
  
//+++++++++++++++++++++++++for new CRB power up+++++++++++++++++++++++++++++++++++++++++  
  dyn_string dsDpNames_Plus = dpNames(gSystemNameCRB+"CRB/CSC_ME_P*","fwCrb_CSC_LV");
  dyn_string dsDpNames_Minus = dpNames(gSystemNameCRB+"CRB/CSC_ME_M*","fwCrb_CSC_LV");
 
  for (i=1; i<= dynlen(dsDpNames_Plus);i++) {
    dpConnect("emuLvCRB_addCrbBufferPlus",false,dsDpNames_Plus[i]+".command");
  }
  for (j=1; j<= dynlen(dsDpNames_Minus);j++) {
    dpConnect("emuLvCRB_addCrbBufferMinus",false,dsDpNames_Minus[j]+".command");
  }  
  dpConnect("emuLvCRB_powerCrbBuffers",gSystemNameCRB+"CRB_Plus.");
  dpConnect("emuLvCRB_powerCrbBuffers",gSystemNameCRB+"CRB_Minus.");
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
  dpConnect("mudcsCrbUpdate_single_crb_switch","SINGLE_CRB_SWITCH."); 
  
//------------ATLAS PSU PART with new type fwBranch_CSC_LV------------------
  dyn_string dsBranchName = dpNames(gSystemNameCRB+"*EPSU*","FwElmbPSUBranch");     
  for(i=1; i<= dynlen(dsBranchName); i++)
    {
      dynAppend(CSC_fwCAN1_g_ALERT_HANDLERS_FIRST,dsBranchName[i]);        
      CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT=1;
      string sDpName_CANV;
      dpGet(dsBranchName[i]+".CAN.volts",sDpName_CANV);
      dpConnect("emuLvCRB_monitorPsuStatus",dsBranchName[i]+".:_alert_hdl.._act_state_color",
                                            gSystemNameCRB+sDpName_CANV+":_alert_hdl.._active");
      while(CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT);       
    }  
  dyn_string dsBranchNameCSC = dpNames(gSystemNameCRB+"*CPSU*","fwBranch_CSC_LV");
  for(i=1; i<= dynlen(dsBranchNameCSC); i++)
    {
     dynAppend(CSC_fwCAN1_g_STATUS_HANDLERS_FIRST,dsBranchNameCSC[i]);        
     CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT=1;
     dpConnect("emuLvCRB_monitorPsuCommand",dsBranchNameCSC[i]+".command");
     while(CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT);       
    }
  DebugTN("init end");  
} 
//============================================================================================
//-----global var "CSC_fwCAN1_g_SINGLE_CRB_SWITCH" based on dp "SINGLE_CRB_SWITCH."----------
//-----CSC_fwCAN1_g_SINGLE_CRB_SWITCH=0 set by mudcsCrbPnlInit in emuDcsCrb.ctl--------------
void mudcsCrbUpdate_single_crb_switch(string dpName, int value)
{ 
  if(value==1)
   {
    CSC_fwCAN1_g_SINGLE_CRB_SWITCH=1;    
   }
  else
   { 
    CSC_fwCAN1_g_SINGLE_CRB_SWITCH=0;
   }  
}
//============================================================================================
void emuLvCRB_monitorCrbCommand(string dpName, int value)
{
  int i,index,iStatus; 
  bool bIsActive; 
  string fsm = substr(dpName,0,strpos(dpName,"."));//remove .command
  dpGet(fsm+".status",iStatus);
  int index_busy=dynContains(all_crb_dps,fsm);  
  if(index_busy<=0)
   { 
    for(i=1;i<=3;i++)  
     {
      DebugTN("ERROR: IMPOSSIBLE SITUATION IN "+fsm);
     }
    emuLvCRB_showDebug(bDebug,"all_crb_dps: "+all_crb_dps);
    return;
   }
  while(crb_busy[index_busy]);
  DebugTN("Checking Crb power status...");
  index=dynContains(CSC_fwCAN1_g_STATUS_HANDLERS_FIRST, fsm);
  if(index >=1 )
   { // first call of handler
     emuLvCRB_showDebug(bDebug,"First handle for CRB:"+fsm);
     dynRemove(CSC_fwCAN1_g_STATUS_HANDLERS_FIRST,index);  
   } 
//-------switch on/off based the .command value,no active in first handle----------
  if(value == 2 && index <1)
   {  
     startThread("mudcsCrb_threadAlertActiveSet",fsm,true,index_busy); 
   }
  else if(value == 0 && index <1)
   {  
     dpGet(fsm+".:_alert_hdl.._active",bIsActive); 
     if(bIsActive)
      {
       if(CSC_fwCAN1_g_ISACK)
        {mudcsLvGroupAcknowlege(fsm);}
      }  
     dpSetWait(fsm+".:_alert_hdl.._active", false); //de-active alert_hdl
     dpGet(fsm+".:_alert_hdl.._active",bIsActive);
     emuLvCRB_showDebug(bDebug,"active(before switch Off) = "+bIsActive);
     startThread("mudcsCrb_threadCommandCrb", fsm, false);
   }
  DebugTN("Crb "+dpName+" command = "+value+", index = "+index);
  CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT=0;
}
//============================================================================================
void emuLvCRB_monitorCrbStatus(string dpName, string value)
{          
  string fsm;  
  int current_status,index; 
  bool bIsActive;
  fsm= substr(dpName,0,strpos(dpName,"."));
  dpGet(fsm+".:_alert_hdl.._active",bIsActive);
  DebugTN("Checking status for CRB: "+fsm);
  
  index=dynContains(CSC_fwCAN1_g_ALERT_HANDLERS_FIRST,fsm);
  if(index >=1 )
   { // first call of handler
     emuLvCRB_showDebug(bDebug,"First handle for CRB: "+dpName);
     dynRemove(CSC_fwCAN1_g_ALERT_HANDLERS_FIRST,index);  
   }
  
  emuLvCRB_showDebug(bDebug,"CRB:"+fsm+".:_alert_hdl.._active is "+bIsActive); 
  emuLvCRB_showDebug(bDebug,"CRB:"+fsm+" alert value is "+value); 
  if(bIsActive)
   {
      if(value == "FwAlarmErrorAck")
         {
             dpSetWait(fsm+".status",-1);  //-1 is error state of crb 
         }
      else 
         {
             dpSetWait(fsm+".status",2); //2 is on state of crb
         }  
    } // if(bIsActive)
  dpGet(fsm+".status",current_status);
  DebugTN("CRB:"+fsm+".status = "+current_status);
  
  CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT=0;
}
//============================================================================================
/**
 * send management command to can bus
 * @param int iAction: 1--start, 2--stop, 129--reset, effect to all elmbs in can-buses
 */
void emuLvCRB_sendCanBusCommand(int iAction)
{  
  dyn_string dsBuses=dpNames(gSystemNameCRB+"*ELMB*","FwElmbCANbus");
  for(int i=1;i<=dynlen(dsBuses);i++)
   { 
	  dpSet(dsBuses[i]+".management", iAction);
   }        
}        
//=============================================================================================
void mudcsCrb_threadCommandCrb(string fsm, bool isOn)
{
  mudcsCrbCommandCrb(fsm,isOn);  //in library emuDcsCrb.ctl
}
//=============================================================================================
//====isActive works as two param:  set power on(isOn), set alert_hdl active(isActive)=========
void mudcsCrb_threadAlertActiveSet(string dpe, bool isActive, int index_busy)
{
  string bus;  
  if(index_busy != 0)
   {
     crb_busy[index_busy]=true; // to block the next switching !!!
     mudcsCrbCommandCrb(dpe,isActive);//in library emuDcsCrb.ctl
   }
  delay(10);//10 s delay for activing alert  
  dpSetWait(dpe+".:_alert_hdl.._active", isActive);
  delay(0,500);
  dpGet(dpe+".:_alert_hdl.._active", isActive);  
  emuLvCRB_showDebug(bDebug,"iActive(after switch on) = "+isActive);
  if(index_busy != 0)
    {crb_busy[index_busy]=false;}
}
//=================== PSU status handle ==================
void emuLvCRB_monitorPsuStatus(string dpName, string value,
                               string dpNameCANV, bool bActive)
{ 
  string fsm;  
  int current_status,index;  
  bool bIsActive;
  fsm= substr(dpName,0,strpos(dpName,"."));
  DebugTN("Checking status for PSU Branch: "+fsm);
  index=dynContains(CSC_fwCAN1_g_ALERT_HANDLERS_FIRST, fsm);
  if(index >=1 )
   { // first call of handler
    emuLvCRB_showDebug(bDebug,"First handle for PSU Branch: "+dpName);
    dynRemove(CSC_fwCAN1_g_ALERT_HANDLERS_FIRST,index);
   } 
  dpGet(fsm+".:_alert_hdl.._active",bIsActive);
  emuLvCRB_showDebug(bDebug,"PSU Branch:"+fsm+".:_alert_hdl.._active is "+bIsActive); 
  emuLvCRB_showDebug(bDebug,"PSU Branch:"+fsm+" alert value = "+value);
  //---check .status at dptype fwBranh_CSC_LV,set it based on dpname/value in dpconnect-- 
  dyn_string ds = strsplit(fsm, ":");
  strreplace(ds[2],"/","_");
  string sDpNameBranchCSC = ds[1]+":CPSU/"+ds[2]; 
  dpGet(sDpNameBranchCSC+".status",current_status);
  emuLvCRB_showDebug(bDebug,"Previous status:"+sDpNameBranchCSC+".status = "+current_status);
  if(bIsActive)
   { 
     if(bActive)
      { 
       if(value == "FwAlarmErrorAck")      
           {
             dpSetWait(sDpNameBranchCSC+".status",-1);  //-1 is error state of PSU Branch 
           }
       else
           {
            dpSetWait(sDpNameBranchCSC+".status",2); // 2 is normal state of psu 
           }
      }
     else
      {
          delay(5);
          //get ai dp name for branch
          string sDpNameADV,sDpNameCANV;
          dpGet(fsm+".AD.volts",sDpNameADV);
          dpGet(fsm+".CAN.volts",sDpNameCANV);
          //get voltage value for branch
          float fVoltageAD,fVoltageCAN;
          dpGet(ds[1]+":"+sDpNameADV,fVoltageAD);
          dpGet(ds[1]+":"+sDpNameCANV,fVoltageCAN);
          if(fVoltageAD < 1 && fVoltageCAN < 1)
             dpSetWait(sDpNameBranchCSC+".status",0);  // 0 is off state of psu  
      }
    }  
  dpGet(sDpNameBranchCSC+".status",current_status);
  DebugTN("PSU:"+sDpNameBranchCSC+".status = "+current_status);  
  CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT=0;
}  
//=================== PSU command handle ==================
void emuLvCRB_monitorPsuCommand(string dpName, int value)
{ 
  string fsm;  
  int iStatus,index;  
  bool bIsActive;
  dyn_string argdsExceptionInfo;
  fsm= substr(dpName,0,strpos(dpName,"."));
  DebugTN("Checking power status for PSU Branch: "+fsm);
  index=dynContains(CSC_fwCAN1_g_STATUS_HANDLERS_FIRST, fsm);
  if(index >=1 )
   { // first call of handler
    emuLvCRB_showDebug(bDebug,"First handle for PSU Branch: "+dpName);
    dynRemove(CSC_fwCAN1_g_STATUS_HANDLERS_FIRST,index);
   } 
  //---get current status of PSU-----------------------------------------
  dpGet(fsm+".status",iStatus);
  emuLvCRB_showDebug(bDebug,"Current power status for PSU Branch: "+fsm+" is "+iStatus);
  DebugTN("PSU Branch:"+fsm+".command = "+value);
  //---send command based on .command value-----------------------------
  if(value == 2 || (iStatus == -1 && index>=1))
   { 
     string sElmb,sPsuSwitchBit,sPsuName;
     dpGet(fsm+".switch_bit",sPsuSwitchBit);
     dpGet(fsm+".elmb_id",sElmb);
     dpGet(fsm+".coord",sPsuName);
     emuLvCRB_setDoBitSync(sElmb,sPsuSwitchBit,true,argdsExceptionInfo); 
     dyn_string ds = strsplit(fsm,":");
     //---toggle do_C_read to active PSU display panel-----
     unsigned unValue;
     dpGet(ds[1]+":"+sElmb+"/DO/do_Bytes.do_C_read",unValue);
     dpSetWait(ds[1]+":"+sElmb+"/DO/do_Bytes.do_C_read",unValue);
     //set alert hdl for dps at FwElmbPSUBranch-------    
     delay(1);
     string sDpNameCANV,sDpNameADV;
     dpGet(ds[1]+":"+sPsuName+".CAN.volts",sDpNameCANV);
     dpGet(ds[1]+":"+sPsuName+".AD.volts",sDpNameADV);
     dpSetWait(ds[1]+":"+sDpNameCANV+":_alert_hdl.._active",true);
     dpSetWait(ds[1]+":"+sDpNameADV+":_alert_hdl.._active",true);
     
     dpGet(ds[1]+":"+sPsuName+".:_alert_hdl.._active",bIsActive);
     emuLvCRB_showDebug(bDebug,sPsuName+".:_alert_hdl.._active is "+bIsActive);                    
   } 
  CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT=0;
}
//==========acknowlege all alerts ========================================
void mudcsLvGroupAcknowlege(string sDpName)
{
   dyn_string ds1;
   int i;
   dyn_string exceptionInfo;
   dpGet(sDpName+"."+":_alert_hdl.._dp_list",ds1);
   for(i=1;i<=dynlen(ds1);i++)
    {
      fwAlertConfig_acknowledge(ds1[i], exceptionInfo);
    }
}
//******************************************************* **********************
//******************script for new CRB power up procedure***********************
//******************************************************************************
void emuLvCRB_addCrbBufferPlus(string sDpName)
{
  emuLvCRB_addCrbBuffers(sDpName,"Plus");
}
void emuLvCRB_addCrbBufferMinus(string sDpName)
{
  emuLvCRB_addCrbBuffers(sDpName,"Minus");
} 
void emuLvCRB_addCrbBuffers(string sDpName,string sSide)synchronized(dsCommandBuffers)
{
   //remove .command
   dyn_string ds = strsplit(sDpName,".");
   dpGet(gSystemNameCRB+"CRB_Buffer_"+sSide+".",dsCommandBuffers);
   dynAppend(dsCommandBuffers,ds[1]);  
   dpSetWait(gSystemNameCRB+"CRB_Buffer_"+sSide+".",dsCommandBuffers);
}

void emuLvCRB_readCrbBuffers(string sSide)synchronized(dsCommandBuffers)
{
   dpGet(gSystemNameCRB+"CRB_Buffer_"+sSide+".",dsCommandBuffers);   
   string sFirstCRB = "";
   int iContain;
   if (dynlen(dsCommandBuffers)<1){
     emuLvCRB_showDebug(bDebug,"nothing in buffer "+sSide);
     bBufferEmpty = true;
     //dpSetWait(gSystemNameCRB+"CRB_Buffer_Empty.",true);
     emuLvCRB_showDebug(bDebug,"CRB buffer empty:"+bBufferEmpty);
   }
   else { 
       do {   
       sFirstCRB = dsCommandBuffers[1];
       dynRemove(dsCommandBuffers,1);
       iContain = dynContains(dsCommandBuffers,sFirstCRB);
       emuLvCRB_showDebug(bDebug,"iContain = "+iContain);
       }
       while(iContain != 0);
       emuLvCRB_showDebug(bDebug,"switch on/off "+sFirstCRB);
       dpSetWait(gSystemNameCRB+"CRB_Buffer_"+sSide+".",dsCommandBuffers);
       dpSetWait(gSystemNameCRB+"CRB_"+sSide+".",sFirstCRB);     
   }
}

void emuLvCRB_monitorCrbBuffers()
{ 
  int iCount = 0; 
  while(1){
    delay(2);
    iCount++;
    if(iCount >= 10){
      DebugTN("checking CRB buffer...");
      iCount = 0;
    }        
    emuLvCRB_readCrbBuffers("Plus");
    emuLvCRB_readCrbBuffers("Minus");
  }
}
void emuLvCRB_initalizeCrbBuffers()
{
  DebugTN("initalize CRB buffers");
  if(!(dpExists(gSystemNameCRB+"CRB_Plus"))){
       dpCreate("CRB_Plus","MUDCS_STRING");
     }
  if(!(dpExists(gSystemNameCRB+"CRB_Minus"))){
      dpCreate("CRB_Minus","MUDCS_STRING");
     }
  if(!(dpExists(gSystemNameCRB+"CRB_Buffer_Minus"))){
     dpCreate("CRB_Buffer_Minus","MUDCS_DYN_STRING");
     }
  if(!(dpExists(gSystemNameCRB+"CRB_Buffer_Plus"))){
     dpCreate("CRB_Buffer_Plus","MUDCS_DYN_STRING"); 
    }
  //for board control
  if(!(dpExists(gSystemNameCRB+"CRB_"))){
       dpCreate("CRB_Boards_Control","MUDCS_DYN_STRING");
     } 
  dpSetWait(gSystemNameCRB+"CRB_Buffer_Minus.",makeDynString());
  dpSetWait(gSystemNameCRB+"CRB_Buffer_Plus.",makeDynString());
  dpSetWait(gSystemNameCRB+"CRB_Boards_Control.",makeDynString());//for board control
  dpSetWait(gSystemNameCRB+"CRB_Plus.","");
  dpSetWait(gSystemNameCRB+"CRB_Minus.","");
  DebugTN("done");  
}

void emuLvCRB_powerCrbBuffers(string sBuffer,string sCRB)
{  
  //DebugTN("power_CRB:" +sCRB);
  if (sCRB == ""){
    return;
  } 
  else{ 
       int iCommand,iStatus;
       dpGet(sCRB+".command",iCommand);
       dpGet(sCRB+".status",iStatus);
       DebugTN(sCRB+" Command = "+iCommand);
       DebugTN(sCRB+" Status = "+iStatus);
       if(iCommand == iStatus){
         DebugTN("CRB already in the status required");
         dpSetWait(sCRB+".status",iStatus); //to reflash the FSM status
         }
       else{
         if(iCommand == 2){
             emuLvCRB_showDebug(bDebug,"switch on CRB:"+sCRB);
             emuLvCRB_powerOnCRB(sCRB); //switch on CRB
             }  
         else{
            emuLvCRB_showDebug(bDebug,"switch off CRB:"+sCRB);
            emuLvCRB_powerOffCRB(sCRB); //switch off CRB
            }
         bBufferEmpty = false;
         //dpSetWait(gSystemNameCRB+"CRB_Buffer_Empty.",false);
         emuLvCRB_showDebug(bDebug,"CRB buffer empty:"+bBufferEmpty);
         }
    }   
}  
void emuLvCRB_powerOnCRB(string sCRB)
{
  int i,j,interval;
  dyn_string dsSwitchList,dsPairs,dsRest,argdsExceptionInfo;
  dyn_int diOffChannels;
  dyn_string dsOffChannels = makeDynString();
  bool bPowerOn = false;
  time startT = getCurrentTime();
  if (bBufferEmpty == true){
      delay(5); //set delay for first CRB to 5s
  }    
  mudcsCrb_stop_slow_control(sCRB); //for x2p
  dpGet(sCRB+".switch_list",dsSwitchList);
  dpGet(sCRB+".off_channels",diOffChannels);
  string sElmb=substr(dsSwitchList[1],0,strpos(dsSwitchList[1],"/DO/"));
  if(is_off_disabled_channels)
     {
       for(i=1;i<=dynlen(diOffChannels);i++)
          {dsOffChannels[i]=diOffChannels[i];}
     }
//check the highest 3 bits in portA, if not zero then set to zreo
  unsigned unValuePortA = fwElmbUser_getDoByte(sElmb,"A",argdsExceptionInfo);
  if (dynlen(argdsExceptionInfo) > 0) { return; }
  bit32 b32ValuePortA = unValuePortA;
  bit32    b32Thres = 31;
  if((b32ValuePortA>>5)){
      bit32   b32ValueSet = (b32ValuePortA & b32Thres);
      fwElmbUser_setDoByte(sElmb,"A",b32ValueSet,argdsExceptionInfo);
  }
    
//for  new_lv_power_up_sequence: switch on order: 1. 1.5v  2. 9 pairs of 3.3v-1,3.3v_2,5v
// 3. VCC  4. MPC/CCB after each switch on, make a power cycle for dsRest(4) as toggle  
    dsPairs = emuLvCRB_getDynString("Switch_List_DTmbs");
    dsRest  = emuLvCRB_getDynString("Switch_List_Others");
    
    DebugTN("switching on "+sCRB);
    for(j=1;j<=1;j++)
         {         
          //---switch on 1.5v-----------------------
          if(dynContains(dsOffChannels,dynlen(dsPairs)+3))continue;
          else emuLvCRB_setDoBitSync(sElmb,dsRest[3],bPowerOn,argdsExceptionInfo);
          interval = interval_set;
          emuLvCRB_generateToggleA4(sElmb,interval);    
          delay(0,interval);     
          emuLvCRB_showDebug(bDebug,"switch on "+sCRB+" board_cc 3 with "+interval+"ms clock");       
          //---switch on board 1-9-------------------
          for(i=1;i<=dynlen(dsPairs);i++)
                {  
                  if(dynContains(dsOffChannels,i))continue; 
                  else emuLvCRB_setDoBitSync(sElmb,dsPairs[i],bPowerOn,argdsExceptionInfo); 
                  emuLvCRB_generateToggleA4(sElmb,interval);
                  delay(0,interval);                   
                  emuLvCRB_showDebug(bDebug,"switch on "+sCRB+" board "+i+" with "+interval+"ms clock"); 
                }
          //----switch on vcc--------------------------
          if(dynContains(dsOffChannels,dynlen(dsPairs)+2))continue;  
          else emuLvCRB_setDoBitSync(sElmb,dsRest[2],bPowerOn,argdsExceptionInfo); 
          emuLvCRB_generateToggleA4(sElmb,interval);
          delay(0,interval);           
          emuLvCRB_showDebug(bDebug,"switch on "+sCRB+" board_cc 2 with "+interval+"ms clock");
          //----switch on ccb/mpc--------------------------
          if(dynContains(dsOffChannels,dynlen(dsPairs)+1))continue;  
          else emuLvCRB_setDoBitSync(sElmb,dsRest[1],bPowerOn,argdsExceptionInfo);   
          emuLvCRB_generateToggleA4(sElmb,interval);
          delay(0,interval);           
          emuLvCRB_showDebug(bDebug,"switch on "+sCRB+" board_cc 1 with "+interval+"ms clock");
          dpSetWait(sCRB+".status",2); //set FSM state to On or standby
          DebugTN("done for power on of Crb:"+sCRB);
          //---set alert handle to active-----------------------------------------
          startThread("emuLvCRB_setAlertHandle",sCRB,true);
        } 
    time endT = getCurrentTime();
    time dt = endT - startT;
    emuLvCRB_showDebug(bDebug,"Switch on took " + minute(dt) +":"+ second(dt) +"."+ milliSecond(dt));
    //-----end of Power on-----------------------------
    dpSetWait("LV_START_UP_CHAMBER.",sCRB+" is finished"); 
}
void emuLvCRB_powerOffCRB(string sCRB)
{
  time startT = getCurrentTime();
  emuLvCRB_setAlertHandle(sCRB,false);  
  delay(0,500); //delay 500ms
  //----for x2p-----------------------------------------
  mudcsCrb_remove_id(sCRB); 
  mudcsCrb_sendToX2P("CRATE_POWER_OFF", sCRB); 
  
  dyn_string dsSwitchList,argdsExceptionInfo;
  dpGet(sCRB+".switch_list",dsSwitchList);
  string sElmb=substr(dsSwitchList[1],0,strpos(dsSwitchList[1],"/DO/"));
  int interval = interval_set;
  fwElmbUser_setDoBytes(sElmb,4095,argdsExceptionInfo);    
  emuLvCRB_generateToggleA4(sElmb,interval);            
  emuLvCRB_showDebug(bDebug,"switch off "+sCRB+" with "+interval+"ms Clock");           
  dpSetWait(sCRB+".status",0); //set FSM state to OFF
  delay(0,100);
  int iStatus;
  dpGet(sCRB+".status",iStatus);
  DebugTN("CRB:"+sCRB+".status = "+iStatus+" (after power off)");
  time endT = getCurrentTime();
  time dt = endT - startT;
  emuLvCRB_showDebug(bDebug,"Switch on took " + minute(dt) +":"+ second(dt) +"."+ milliSecond(dt));
  DebugTN("done for power off of Crb:"+sCRB);
}
void emuLvCRB_setAlertHandle(string sCRB,bool bIsActive)
{
  if (bIsActive == true){
    delay(12); //change to 12s from 10s
    string iCommand;
    dpGet(sCRB+".command",iCommand);
    if(iCommand == 2){
          dpSetWait(sCRB+".:_alert_hdl.._active",true); 
       }
    else  
       dpSetWait(sCRB+".:_alert_hdl.._active",false);   
    delay(0,500);
    dpGet(sCRB+".:_alert_hdl.._active", bIsActive);  
    emuLvCRB_showDebug(bDebug,"alert handle(after action) = "+bIsActive);    
  }
  else {
    dpGet(sCRB+".:_alert_hdl.._active",bIsActive); 
    emuLvCRB_showDebug(bDebug,"alert handle(before switch Off) = "+bIsActive);
    if(bIsActive)
       {
        if(CSC_fwCAN1_g_ISACK)
         {mudcsLvGroupAcknowlege(sCRB);}
       }  
     dpSetWait(sCRB+".:_alert_hdl.._active", false); //de-active alert_hdl 
     delay(0,500);
     dpGet(sCRB+".:_alert_hdl.._active", bIsActive);  
     emuLvCRB_showDebug(bDebug,"alert handle(after action) = "+bIsActive);      
  }     
}  
//====for board control================================================
void emuLvCRB_boardControl(string sDpName,dyn_string dsValue)
{
  if(dynlen(dsValue)<3)
   { 
    DebugTN("no action on board");
   } 
  else
    {    
      bool bIsOn = dsValue[2];
      dyn_int diBoard = makeDynInt(dsValue[3]);
      mudcsCrbCommandCrbGroup(dsValue[1],bIsOn,diBoard);
    }  
}
//=====for fast deactive CRB alert handle==============================
void emuLvCRB_deactiveAlertHandle(string sDpName,int iCommand)
{
  string sCRB = substr(sDpName,0,strpos(sDpName,"."));//remove .command
  if(iCommand == 0)
   {
     emuLvCRB_setAlertHandle(sCRB,false);
     DebugTN("alert handle for "+sCRB+" deactived");
   }
  else 
   return;  
}  
