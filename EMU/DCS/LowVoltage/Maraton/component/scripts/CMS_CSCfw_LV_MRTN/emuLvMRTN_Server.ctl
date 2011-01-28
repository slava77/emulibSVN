#uses "CMS_CSCfw_LV_MRTN/emuLvMRTN.ctl"
#uses "CMS_CSCfw_LV_MRTN/emuDcsMrtn.ctl"

dyn_string CSC_fwCAN2_g_ALERT_HANDLERS_FIRST;
int CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT;
dyn_string CSC_fwCAN2_g_STATUS_HANDLERS_FIRST;
dyn_string CSC_fwCAN2_g_FULL_DPLIST;
int FIRST_FSM_PANELS=0;
//========================================================
main()
{
  emuLvMRTN_initalizeParam();
  mudcsMrtnPnlInit();
  mudcsMrtnServerInit();
  mudcsMrtnUpdate_BrokerList();
}
//========================================================
void mudcsMrtnUpdate_BrokerList()
{
  DebugTN("init start...");
//-------------- MRTN crate----------------------------------------------
  dyn_string datapoints = dpNames(gSystemNameMRTN+"*Wiener*","FwWienerMarathon");       
   for(int i=1;i<=dynlen(datapoints);i++)
     { // corr
        dynAppend(CSC_fwCAN2_g_ALERT_HANDLERS_FIRST,datapoints[i]);        
        CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT=1;
        dpConnect("emuLvMRTN_updateAlert",datapoints[i]+".:_alert_hdl.._act_state_color");         
        while(CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT);          
     } //i
   for(int i=1;i<=dynlen(datapoints);i++)
     {
        if(strpos(datapoints[i],"Dummy")>=0)continue;  
        dynAppend(CSC_fwCAN2_g_STATUS_HANDLERS_FIRST,datapoints[i]);
        dynAppend(CSC_fwCAN2_g_FULL_DPLIST,datapoints[i]);       
        CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT=1;
        dpConnect("emuLvMRTN_updateStatus",datapoints[i]+".Status.On");
        while(CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT);
     } //i
   delay(1);
   DebugTN("init end");
}	
//========ROLE OF THE FUNCTION: TO RESTORE THE STATUSES AFTER PVSS PROJECT RESTART====================
void emuLvMRTN_updateStatus(string dpName,bool value)
{     
   string fsm= substr(dpName,0,strpos(dpName,"."));
   emuLvMRTN_showDebug(bDebug,"Update status for marathon crate: "+fsm);
   int index=dynContains(CSC_fwCAN2_g_STATUS_HANDLERS_FIRST, fsm); // to eliminate .status
   if(index >=1 )
     { // first call of handler
       emuLvMRTN_showDebug(bDebug,"First status handle of marathon crate: "+fsm);
       dynRemove(CSC_fwCAN2_g_STATUS_HANDLERS_FIRST,index);
       dpSetWait(fsm+".Settings.OnOffCrate", value); 
       DebugTN("Marathon crate power status: "+dpName+" = "+value+" index = "+index);
     }  
   CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT=0;
}
//============ROLE OF FUNCTION: to call _valueChanged function ========================
void emuLvMRTN_updateAlert(string dpName,string value)
{   
   int i, current_status;
   dyn_string ch_dps; 
   bool update_status;
   string fsm= substr(dpName,0,strpos(dpName,"."));
   emuLvMRTN_showDebug(bDebug,"Update alert for marathon crate: "+fsm);
   int index=dynContains(CSC_fwCAN2_g_ALERT_HANDLERS_FIRST, fsm);
   if(index >=1 )
    { // first call of handler
      emuLvMRTN_showDebug(bDebug,"First alert handle of marathon crate: "+fsm);
      dynRemove(CSC_fwCAN2_g_ALERT_HANDLERS_FIRST,index);
    } 
//------ this is to call  _valueChanged function of channels in FSM---------   
   ch_dps=dpNames(fsm+"/*", "FwWienerMarathonChannel");  
   for(i=1;i<=dynlen(ch_dps);i++)
    {
     dpGet(ch_dps[i]+".Status.On",update_status);    
     dpSetWait(ch_dps[i]+".Status.On",update_status); 
    }
//------ this is to call  _valueChanged function of crate in FSM------------
   dpGet(fsm+".Status.On",update_status);    
   dpSetWait(fsm+".Status.On",update_status); 
   DebugTN("Marathon crate alert: "+dpName+" = "+value); 
   CSC_fwCAN2_g_BLOCK_NEXT_DPCONNECT=0;
}



