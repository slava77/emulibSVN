// EMU common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs2.ctl"
#uses "CMS_CSCfw_HV_CC/emu_hvCommon.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcsDim.ctl"
#uses "CMS_CSCfw_HV_CC/emuAlert.ctl"
#uses "CMS_CSCfw_HV_CC/emufw.ctl"


// x2p
// _smiDeviceDummy
// mudcsFsmErrorSet: { and } in commented piece (do not care) 
// is_newPRFSM
// is_GasMatrix
// is_CoolingMatrix (should not use -- alert config. should be used instead)
 
bool is_x2p=true;
bool is_newPRFSM=true;
bool is_GasMatrix=true;
bool is_CoolingMatrix=false; // (should not use -- alert config. should be used instead)
    
int CSC_g_BLOCK_WATCHDOG_THREAD=0;
int watch_dog_delay;

bool no_auto_restor=TRUE;
bool no_auto_restor_add=TRUE;//FALSE;
bool no_auto_restor_add2=TRUE;//

threadWatchForWatch(){
  
  time last_set, current_time;
  int last_set_i, current_time_i, time_difference;
  int watch_dog_delay2=watch_dog_delay*2;
    delay(watch_dog_delay2);
  while(1){ 
   delay(watch_dog_delay);
   current_time=getCurrentTime();
   current_time_i=current_time;
   dpGet("WATCH_FOR_TIMEOUTS.:_online.._stime",last_set);
   last_set_i=last_set;
  time_difference=current_time_i-last_set_i;
  //mudcsDebug(time_difference);
  if(time_difference > watch_dog_delay2){
   startThread("threadWatchForTimeouts");
   dpSetWait("THREAD_WATCHDOG_RESTART.",current_time);
   delay(watch_dog_delay2);
  }
 } // while
}

threadWatchForTimeouts(){

 dyn_string BrokerList;
 dyn_int watch_array;
 int j;
//// int watch_dog_delay;
// if(CSC_fwG_g_CUSTOM_LV)watch_dog_delay=3600;
// else watch_dog_delay=240;//180;

 time current_time;
 dyn_time watch_times;
    for(j=1; j<= dynlen(CSC_fwG_g_watch_mask); j++){
      watch_array[j]=0;
      watch_times[j]=0;
      dpSetWait(CSC_fwG_g_SYSTEM_NAME+":WATCH_TIME"+j+".",watch_times[j]);   
    }
  
//-------------
//dyn_string CSC_fwG_g_watch_mask;//=makeDynString("LV_SX5_DimBroker","LV_1_DimBroker","GAS_SX5_DimBroker",
                        //            "WTH_SX5_DimBroker","PT100_DimBroker");
                        //for(j=1;j<=dynlen(CSC_fwG_g_all_primary);j++)dynAppend(CSC_fwG_g_watch_mask,CSC_fwG_g_all_primary[j]);
//-------------

file femessage;
string emessage="To: valerii.sytnik@cern.ch \nSubject: EMU-DCS: SUBSYSTEM ALIVE WARNING  \n \n \n";
string eemessage;
 
emu_info("Starting watchdog, watch mask = " + CSC_fwG_g_watch_mask);

//------------------
 while(1){
//   delay(1800);

 current_time=getCurrentTime(); 
 delay(watch_dog_delay); //3600 is 60 minutes (the watch dog time)
 CSC_g_BLOCK_WATCHDOG_THREAD=1; // to decouple threads (main and watchdog thread) I
 delay(10); // to decouple threads (main and watchdog thread) II
 
 /////// dpGet(CSC_fwG_g_SYSTEM_NAME+":WATCH_FOR_TIMEOUTS.",watch_array);
 /////dpGet(CSC_fwG_g_SYSTEM_NAME+":WATCH_TIMES.",watch_times);
    femessage=0;
//    femessage=fopen(CSC_fwG_g_project_name_home+"/source/"+CSC_fwG_g_Component+"/text/alive_emessage","w");
//    if(femessage==0)continue;
    int was=0;
        emu_debug("ALIVETHREAD "+dynlen(CSC_fwG_g_watch_mask), emu_DEBUG_DETAIL);
    

    for(j=1; j<= dynlen(CSC_fwG_g_watch_mask); j++){
      
     dpGet(CSC_fwG_g_SYSTEM_NAME+":WATCH_TIME"+j+".",watch_times[j]);   
     if(current_time>watch_times[j]){
/////     if(watch_array[j] == 0)
       watch_array[j]--; // not to set alarm again 

       emu_info("WATCH FOR SUBSYSTEM ALIVE: TIMEOUT IN: "+CSC_fwG_g_watch_mask[j]);
       if(!was){
        if(watch_array[j]==0){        
//        fputs(emessage+"WATCH FOR SUBSYSTEM ALIVE: TIMEOUT IN: "+CSC_fwG_g_watch_mask[j]+"-->"+CSC_fwG_g_SYSTEM_NAME+"\n",femessage);
//        eemessage += emessage+"WATCH FOR SUBSYSTEM ALIVE: TIMEOUT IN: "+CSC_fwG_g_watch_mask[j]+"-->"+CSC_fwG_g_SYSTEM_NAME+"\n";
setDevicesNotConnected(CSC_fwG_g_watch_mask[j]);
        }
       }
       else{ 
        if(watch_array[j]==0){
//        fputs("WATCH FOR SUBSYSTEM ALIVE: TIMEOUT IN: "+CSC_fwG_g_watch_mask[j]+"-->"+CSC_fwG_g_SYSTEM_NAME+"\n",femessage);
//        eemessage += "WATCH FOR SUBSYSTEM ALIVE: TIMEOUT IN: "+CSC_fwG_g_watch_mask[j]+"-->"+CSC_fwG_g_SYSTEM_NAME+"\n";
setDevicesNotConnected(CSC_fwG_g_watch_mask[j]);
        }
       }
//       fflush(femessage); // write buffer immediately
      if(watch_array[j]==0)was=1; // to format email 
//       watch_array[j]--; // not to set alarm again 
     }
     else {
       emu_info("WATCH FOR SUBSYSTEM ALIVE: THE SUBSYSTEM "+CSC_fwG_g_watch_mask[j]+" IS ALIVE");  
       watch_array[j]=1;   
     }



    } // for

//       fclose(femessage);
      //if(was)system("mail "+CSC_fwG_g_EMAILS_GENERAL+" < "+CSC_fwG_g_project_name_home+"/source/"+CSC_fwG_g_Component+"/text/alive_emessage");


  dpSet(CSC_fwG_g_SYSTEM_NAME+":WATCH_FOR_TIMEOUTS.",watch_array);    
  CSC_g_BLOCK_WATCHDOG_THREAD=0;
 } // while(1)
//------------------

}
//==============================
threadSynchronizeMRTN2LV_1_enable(){
 threadSynchronizeMRTN2LV_1(1);
}
//==============================
threadSynchronizeMRTN2LV_1_disable(){
 threadSynchronizeMRTN2LV_1(0);
}
//==============================
threadSynchronizeMRTN2LV_1(int mode)
{
// mode 1 : eanble, switch on
// mode 0 : disable



dyn_string flags;
dyn_string chs;
int i;

if(mode)delay(300);// 5 minute


//chs=fwFsmUi_getAllChildren("CSC_ME_P4","CSC_ME_P4",flags);
//for(i=1;i<dynlen(chs);i++){
// mudcsDebug(chs[i]);
//}


//string node="CMS_CSC";
//chs=fwFsmUi_getAllChildren(node,node,flags);

chs=makeDynString("CMS_CSC");
///////getch(2, chs[1], chs, makeDynInt(1));
mudcs_getchSynchronizeMRTN2LV_1(mode, chs[1], chs, makeDynInt(1));

///fwFsmUi_setCUModeByName("CMS_CSC","CSC_ME_P4::CSC_ME_P4","Exclude");

////mudcsDebug("done");

}
//===========================================================
mudcs_getchSynchronizeMRTN2LV_1(int mode, string domain, dyn_string chs, dyn_int flags){

dyn_string chs1;
dyn_string split;
int status;
dyn_int flags1;
int i,j;
int count;
string mode_en;

//if(domain=="CSC_ME_P12_C27")mudcsDebug(domain);

 for(i=1;i<=dynlen(chs);i++){
  if(flags[i]==2){
   continue;


  }
  else{
   if(flags[i]==0){
    split[1]=domain;
    chs1=fwFsmUi_getAllChildren(split[1],chs[i],flags1);
   }
   else{ 
    split=strsplit(chs[i],":");
    chs1=fwFsmUi_getAllChildren(split[1],split[1],flags1);
   }

   if(dynlen(chs1)>=1)mudcs_getchSynchronizeMRTN2LV_1(mode,split[1],chs1,flags1);
    count=0;
    for(j=1;j<=dynlen(chs1);j++){



//if(domain=="CSC_ME_P2")mudcsDebug(chs1[j]+" "+flags1[j]);
     if(flags1[j]==2){

//mudcsDebug2(split[1]+" "+chs1[j]);
      dpGet(chs1[j]+".status",status);
//if(domain=="CSC_ME_P2")mudcsDebug2(split[1]+" "+chs1[j]);
          count++;

         mode_en=fwFsmUi_getDUMode(split[1],chs1[j]);
         if(mode==1 && mode_en=="Disabled"){
          if(strpos(chs1[j],"LowVoltage/")>=0 && strpos(chs1[j],"_LV")>=0){
       //     mudcsDebug(split[1]+" "+chs1[j]);
            fwFsmUi_setDUModeByName(split[1],chs1[j],"Enable");
            dpSet(chs1[j]+".status",2);
          }
         }
         else if(mode==0 && mode_en=="Enabled"){
          if(strpos(chs1[j],"LowVoltage/")>=0 && strpos(chs1[j],"_LV")>=0){
       //     mudcsDebug(split[1]+" "+chs1[j]);
            fwFsmUi_setDUModeByName(split[1],chs1[j],"Disable");
          }
         }


//        if(domain=="CSC_ME_P2")mudcsDebug2(split[1]+" "+chs1[j]);

     }
    } // for j

  } // else if(flags[i]==2)
 } // for i


}





//==============================
setDevicesNotConnected(string sample){

int j;
string type, s1,s2;
int pos, pos1,pos2;
dyn_string dps, dps1, dsTemp;
int status_get;

  pos=strpos(sample,"_DimBroker");
  if(pos >= 0){

   type=substr(sample,0,pos);
/*
   pos1=strpos(sample,"_MINUS");
   pos2=strpos(sample,"_PLUS");
   
   if(pos1>=0){
    dps=dpNames("*CSC_ME_M*",type);
    if(type=="LV_1")dps1=dpNames("*CSC_ME_M*","TEMP_1");
    dynAppend(dps,dps1);
   }
   else if(pos2>=0){
    dps=dpNames("*CSC_ME_P*",type);
    if(type=="LV_1")dps1=dpNames("*CSC_ME_P*","TEMP_1");
    dynAppend(dps,dps1);
   }
*/
   dps=dpNames("*",type);
   
   for(j=1;j<=dynlen(dps);j++){
    dpGet(dps[j]+".status",status_get);  
    if(status_get!=-2)dpSet(dps[j]+".status",-2);
    if(!(dynContains(CSC_fwG_g_FULL_DPLIST, dps[j])))dynAppend(CSC_fwG_g_FULL_DPLIST,dps[j]);
    if(dpExists(dps[j]+".module_state"))dpSetWait(dps[j]+".module_state",-2);// this all is to allow to switch off from error state and differ -2 and -1
    else dpSetWait(dps[j]+".chamber_state",-2);// this all is to allow to switch off from error state and differ -2 and -1
   }
  }
  else if(strpos(sample,"primary")>=0){ // case of primary
   dps=dpNames("*"+sample+"*","HV_PR");
   for(j=1;j<=dynlen(dps);j++){
//    pos=strpos(dps[j],sample);
//     if(pos >= 0){
      dpGet(dps[j]+".status",status_get);  
      if(status_get!=-2)dpSet(dps[j]+".status",-2);
      if(!(dynContains(CSC_fwG_g_FULL_DPLIST, dps[j])))dynAppend(CSC_fwG_g_FULL_DPLIST,dps[j]);
      if(dpExists(dps[j]+".module_state"))dpSetWait(dps[j]+".module_state",-2);// this all is to allow to switch off from error state and differ -2 and -1
      else dpSetWait(dps[j]+".chamber_state",-2);// this all is to allow to switch off from error state and differ -2 and -1
//     }  
   }
  }
  else {  // LV_1, TEMP_1, FED_1

    if(strpos(sample,"LowVoltage")>=0){
     dsTemp=strsplit(sample,"/");    
     dynAppend(dps,"LowVoltage/"+dsTemp[dynlen(dsTemp)]+"_LV");
     dynAppend(dps,"Temperature/"+dsTemp[dynlen(dsTemp)]+"_TEMP");
    }
    else if(strpos(sample,"Fed")>=0){
     dsTemp=strsplit(sample,"/");    
     dynAppend(dps,"Fed/"+dsTemp[dynlen(dsTemp)]+"_FED");
    }
    for(j=1;j<=dynlen(dps);j++){
     dps[j]=mudcsAddSystem(dps[j]);
     dpGet(dps[j]+".status",status_get);  
     if(status_get!=-2)dpSet(dps[j]+".status",-2);
     if(!(dynContains(CSC_fwG_g_FULL_DPLIST, dps[j])))dynAppend(CSC_fwG_g_FULL_DPLIST,dps[j]);
     if(dpExists(dps[j]+".module_state"))dpSetWait(dps[j]+".module_state",-2);// this all is to allow to switch off from error state and differ -2 and -1
     else dpSetWait(dps[j]+".chamber_state",-2);// this all is to allow to switch off from error state and differ -2 and -1
    }
  }


setDpointsNotControlled("");
}
//==============================

threadForELMBTest(){

string device_type="_LV";
dyn_string selected_list;
int i;
string current_status;

mudcsScanTreeForDevices(device_type, CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_part+"/CSCdm2", selected_list); // _LV

 while(1){
 delay(1200); // 20 minutes

   for(i=1;i<=dynlen(selected_list);i++){
    dpGet(selected_list[i]+".status",current_status);
    if(current_status != -2)
    dpSet(selected_list[i]+".status",0);
   }
   delay(60);
   for(i=1;i<=dynlen(selected_list);i++){
    dpGet(selected_list[i]+".status",current_status);
    if(current_status != -2)
    dpSet(selected_list[i]+".status",2);
   }

 } // while(1)

}




//=====================

main(){

int a, i;
dyn_string test_dyn_string;
dyn_string test_dyn_string1; 

mudcsInit_new();
mudcsInitServer();
dpConnect("mudcsserver_update_mywarning",TRUE,CSC_fwG_g_SYSTEM_NAME+":MYWARNING.value");

//===================================================================================================


//dpConnect("mudcsUpdate_test",CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.InRadDeviceList:_online.._value");
//dpConnect("mudcsUpdate_test1",CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.InRadDeviceList");

emu_info("delay started to let dim services start up (configured)");
waiting_for_dim_configured();
// delay(60*4); // to let pvss-dim servers to start up (the dim servers are launched by the operation tab of Editor-Navigator)
// DebugTN("delay ended");

//delay(15); // to let pvss-dim servers start up (the dim servers are launched by the operation tab of Editor-Navigator)



addGlobal("g_FIRST", BOOL_VAR);
//mudcsInit_new();
//-------------
  dpGet(CSC_fwG_g_SYSTEM_NAME+":"+"HV_36CHANNEL_BOARDS_CHAMBER_LIST.",test_dyn_string);
  for(i=1;i<=dynlen(test_dyn_string);i++){
   test_dyn_string1=strsplit(test_dyn_string[i],"|");
   if(dynlen(test_dyn_string1) == 2){
     CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[i][1]=test_dyn_string1[1];
     CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[i][2]=test_dyn_string1[2];
   }
   else{
     CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[i][1]=test_dyn_string1[1];
   }
  }
//-------------

 dpSet(CSC_fwG_g_SYSTEM_NAME+":DPOINTS_NOT_CONTROLLED.value","INITIAL");

//DebugN(")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))0");
//DebugN(")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))0");
//DebugN(")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))0");

g_FIRST=true;
mudcsUpdate_BrokerList("",test_dyn_string);
////dpConnect("mudcsUpdate_BrokerList",CSC_fwG_g_SYSTEM_NAME+":Db_o.DimBrokerList");

 delay(60);
//DebugN(")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))");
//DebugN(")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))");
//DebugN(")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))");

setDpointsNotControlled("DPOINTS_NOT_CONTROLLED_CAN_BE_SET");

//DebugN(")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))1");
//DebugN(")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))1");
//DebugN(")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))1");

if(CSC_fwG_g_DEFINE_ELMB){
//////////  startThread("threadForELMBTest");
}

//DebugN(")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))2");
//DebugN(")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))2");
//DebugN(")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))2");

string warning_value;
dpGet(CSC_fwG_g_SYSTEM_NAME+":MYWARNING.value",warning_value);
if(strpos(warning_value,"postinstall_start")>=0){//should never come here
  
}
else if(strpos(warning_value,"postinstall_finish")>=0){
 dpSet(CSC_fwG_g_SYSTEM_NAME+":"+"MYWARNING.value","warning_value_not_set"); //should never come here
}
else dpSet(CSC_fwG_g_SYSTEM_NAME+":"+"MYWARNING.value","init_finish");
/*
if(strpos(warning_value,"postinstall")<0)
dpSet(CSC_fwG_g_SYSTEM_NAME+":"+"MYWARNING.value","init_finish");
else dpSet(CSC_fwG_g_SYSTEM_NAME+":"+"MYWARNING.value","warning_value_not_set");
*/

//=============================================
// if(CSC_fwG_g_CUSTOM_LV)watch_dog_delay=3600;
// else watch_dog_delay=240;//180;
 
startThread("threadWatchForTimeouts");
startThread("threadWatchForWatch");
//===========================================

}



//=================================================================================================

mudcsserver_update_mywarning(string dpName, string value){

   if(value == "postinstall_start" || value == "postinstall_finish"){
    exit(0);
   }


}
//=================================================================================================
mudcsInit_new(){
  
 mudcsInit();

}
//=================================================================================================
//=================================================================================================
/* fsm
mudcsUpdate_test1(string dp, dyn_string value){

 dpSet(CSC_fwG_g_SYSTEM_NAME+":treeBrowser_control_resp.","nmnmnm");

}

*/
//=================================================================================================
mudcsUpdate_BrokerList(string dp, dyn_string value){

 int j;
    dyn_string d_s1, d_s2;


//=============================================
 if(CSC_fwG_g_CUSTOM_LV)watch_dog_delay=3600;
 else watch_dog_delay=240;//180;    
//==============================
 
 dpSet(CSC_fwG_g_SYSTEM_NAME+":HV_MASTER2CHAMBER.",makeDynString());

 dyn_int watch_array;
for(j=1; j<= dynlen(CSC_fwG_g_watch_mask); j++){
 dynAppend(watch_array,0);
}
  dpSet(CSC_fwG_g_SYSTEM_NAME+":WATCH_FOR_TIMEOUTS.",watch_array);


//===============================


//dpSet("dyn_debug1.",makeDynString());
//dpSet("dyn_debug2.",makeDynString());


emu_info(" INFO, mudcsServer: init start***************************");

 dyn_string BrokerList,BrokerList_old;
 string service,update_fun,update_fun_status;
// dyn_int watch_array;
//// dpSet(CSC_fwG_g_SYSTEM_NAME+":treeBrowser_control_resp.","nmnmnm1");

//  get_primary_data();
//  get_master_data();

 dpGet(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimBrokerList", BrokerList);
 dpGet(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimBrokerList_old", BrokerList_old);

////  if(g_FIRST) dpSet(CSC_fwG_g_SYSTEM_NAME+":treeBrowser_control_resp.","first");
////  if(!g_FIRST) dpSet(CSC_fwG_g_SYSTEM_NAME+":treeBrowser_control_resp.","second");

    CSC_fwG_g_CopyDimBrokerList=BrokerList;
    for(j=1; j<= dynlen(BrokerList); j++){


      // -----========= REMOVING ALL X2P DATA HANDLING (it's now handled in emu_x2pDataProcessing.ctl)===========----
      if ((strpos(BrokerList[j], "LV_1") >= 0) ||
          (strpos(BrokerList[j], "TEMP_1") >= 0) ||
          (strpos(BrokerList[j], "FED_1") >= 0)) {
        continue;
      }
      // ------------------====================================================---------------------------------------
      
      
      
     if(!g_FIRST && BrokerList[j] == BrokerList_old[j])continue;
       service=substr(BrokerList[j],0,strpos(BrokerList[j],"_DimBroker"));
       update_fun="mudcsUpdateMonitor_"+service+"_new";
 

 if(service == "GAS_SX5"){        
       dynAppend(CSC_fwG_g_BROKER_HANDLERS_FIRST,CSC_fwG_g_SYSTEM_NAME+":"+BrokerList[j]+"_o");
       dpConnect(update_fun,CSC_fwG_g_SYSTEM_NAME+":"+BrokerList[j]+"_o.update_value");
 }




emu_debug("Connecting "+update_fun + " to "+ CSC_fwG_g_SYSTEM_NAME+":"+BrokerList[j]+"_o.update_value");

//////dpSet(CSC_fwG_g_SYSTEM_NAME+":treeBrowser_control_resp.",update_fun+"___"+CSC_fwG_g_SYSTEM_NAME+":"+BrokerList[j]+"_o.update_value");

///DebugN("mudcsServer: init start 1***************************");

dyn_string datapoints;
dyn_string datapoints_d;

       update_fun_status="mudcsUpdateMonitor_"+service+"_status";
       datapoints = dpNames("*",service);
       datapoints_d = dpNames("*",service+"_d");



       for(int i=1;i<=dynlen(datapoints_d);i++){

       if(strpos(datapoints_d[i],"Broker")>=0 || strpos(datapoints_d[i],"master")>=0)continue;
       dynAppend(CSC_fwG_g_BROKER_HANDLERS_FIRST,datapoints_d[i]);
///// DebugN("-----------"+  datapoints_d[i]  +" ------------");
/////        dpSetWait(datapoints_d[i]+".update_value",999);
        if(service=="fwGasSystem_CSC_GAS")
           dpConnect(update_fun,FALSE,"GasPlcCounter.");
        else if(service=="fwCooling_CSC_COOLING")
           dpConnect(update_fun,FALSE,datapoints_d[i]+".Flowmeter_EndCap_Cooling_YE_Plus_1");
        else dpConnect(update_fun,datapoints_d[i]+".update_value");
        
/*
    dynAppend(d_s1,update_fun+">>>>>>>"+datapoints_d[i]+".update_value");
    dpSetWait("dyn_debug1.",d_s1);
*/

       } //i


// ================== below is to get master data so to link masters and chambers (see mudcsGetMasterForChamber)===
// ======== ths is needed if a dim server is restarted : maybe we should insert here also getting chambers data ===
if(service == "HV_1")get_master_data();  // 
//=================================================================================================================

//-----
//-----

       for(int i=1;i<=dynlen(datapoints);i++){
       if(strpos(datapoints[i],"_smiDevice")>=0)continue;   
       dynAppend(CSC_fwG_g_STATUS_HANDLERS_FIRST,datapoints[i]);
       dynAppend(CSC_fwG_g_FULL_DPLIST,datapoints[i]);
///// DebugN("-----------"+  datapoints[i]  +" ------------");

        CSC_fwG_g_BLOCK_NEXT_DPCONNECT=1;
        
        int current_status;
        string data;
        if(no_auto_restor_add && (service == "HV_1" || service == "HV_PR")){
         dpGet(datapoints[i]+".status",current_status);
         if(current_status!=-2)
          dpSetWait(datapoints[i]+".status",0);
         else{
          mudcsConvertFsm2DataOne(datapoints[i],data);
          dpSetWait(data+".:_alert_hdl.._active",0);           
         } 
         //if(service == "HV_PR")dpSetWait(datapoints[i]+".chamber_state",0);
         //else dpSetWait(datapoints[i]+".module_state",0);
        }
          dpConnect(update_fun_status,TRUE,datapoints[i]+".status");
        
/*
    dynAppend(d_s1,datapoints[i]);
    dpSetWait("dyn_debug1.",d_s1);
*/

        while(CSC_fwG_g_BLOCK_NEXT_DPCONNECT);
//        delay(1);
       } //i

//-----

    } //j

//  get_master_data();
//  get_primary_data();

 g_FIRST=false;
 dpSet(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimBrokerList_old", BrokerList);

/////// dpSet(CSC_fwG_g_SYSTEM_NAME+":treeBrowser_control_resp.","nmnmnm2");

//  dpSet(CSC_fwG_g_SYSTEM_NAME+":WATCH_FOR_TIMEOUTS.",watch_array);
//----------
/*
  dynAppend(CSC_fwG_g_STATUS_HANDLERS_FIRST,CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_COM"); //
  dpConnect("mudcs_update_run_control_command",CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_COM.command"); // slice test 
*/
//----------


  get_primary_data();
//  get_master_data();
/*
    dynAppend(CSC_fwG_g_dyn_debug1,"dp");
    dyn_string d_s=CSC_fwG_g_dyn_debug1;
    dpSetWait("dyn_debug1.",d_s);
*/
    dpSetWait(CSC_fwG_g_SYSTEM_NAME+":HV_MASTER2CHAMBER.",CSC_fwG_g_master2chamber);

//-------------------------------------------
  dpSet(CSC_fwG_g_SYSTEM_NAME+":RUNCONTROL_FLAG.",1);
//-------------------------------------------
emu_info("mudcsServer: init end***************************");

}	
//============================================================================================
//============================================================================================
//============================================================================================
mudcs_update_run_control_command(string dpName, string value){
//dpSet("h3.",CSC_fwG_g_STATUS_HANDLERS_FIRST);
int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
//dpSet("h2.",CSC_fwG_g_STATUS_HANDLERS_FIRST);
  emu_info("mudcs_update_run_control_command:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER RUN_CONTROL");
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
 //dpSet("h2.",CSC_fwG_g_STATUS_HANDLERS_FIRST);
  return;

 }

 
emu_info("SHOULD NOT SEE THIS (shouldn't be called)  YOOOU mudcs_update_run_control_command");
 
// dpSet("h1.",CSC_fwG_g_STATUS_HANDLERS_FIRST);

if(strpos(value,";dp")>=0)strreplace(value,";dp",";sp");


int runcontrol_flag;
dpGet(CSC_fwG_g_SYSTEM_NAME+":RUNCONTROL_FLAG.",runcontrol_flag);
if(!runcontrol_flag){
 dpSetWait(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command","BLOCKED"); // service update !!!
 return; 
}

dyn_string test_dyn=strsplit(value,";");


if(test_dyn[1] == "HV")emudcs_hv_run_control(test_dyn);
else if(test_dyn[1] == "DCSINTERLOCK")emudcs_interlock_run_control(test_dyn); 
else if(test_dyn[1] == "LV")emudcs_lv_run_control(test_dyn); 
else if(test_dyn[1] == "CRB")emudcs_crb_run_control(test_dyn); 
else if(test_dyn[1] == "MRTN")emudcs_mrtn_run_control(test_dyn); 
else if(test_dyn[1] == "CHIP")emudcs_chip_run_control(test_dyn);

else if(test_dyn[1] == "HVPREREAD" || test_dyn[1] == "HVREAD")emudcs_hv_rc_read(test_dyn);
else if(test_dyn[1] == "LVPREREAD" || test_dyn[1] == "LVREAD")emudcs_lv_rc_read(test_dyn);
else if(test_dyn[1] == "CRBPREREAD" || test_dyn[1] == "CRBREAD")emudcs_crb_rc_read(test_dyn);
else if(test_dyn[1] == "MRTNPREREAD" || test_dyn[1] == "MRTNREAD")emudcs_mrtn_rc_read(test_dyn);

else if(test_dyn[1] == "WTHPREREAD" || test_dyn[1] == "WTHREAD")emudcs_wth_rc_read(test_dyn);
else if(test_dyn[1] == "PT100PREREAD" || test_dyn[1] == "PT100READ")emudcs_pt100_rc_read(test_dyn);
else if(test_dyn[1] == "GASPREREAD" || test_dyn[1] == "GASREAD")emudcs_gas_rc_read(test_dyn);
else if(test_dyn[1] == "TEMPPREREAD" || test_dyn[1] == "TEMPREAD")emudcs_temp_rc_read(test_dyn);

}
//============================================================================================
//============================================================================================
//============================================================================================
mudcsUpdateMonitor_HV_PR_status(string dpName, int value){

int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
//  return;
  } else {
    emu_info("Status update HV Primary: " + dpName + "=" + value);
  }
// else{if(CSC_fwG_g_CSC_INTERLOCK_FLAG)return;}

int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".chamber_state",state_save);
//if(value==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".chamber_state",-2);

 bool state_destin;
 dpGet(substr(dpName,0,strpos(dpName,"."))+".:_alert_hdl.._active",state_destin);// !!!! error here (wrong dp): anyway the mudcsFsmErrorSet does this function


if(value == 2 || (value==-1 && index >=1) /*|| (value==-2 && state_destin==true && index >=1)*/){  // -1, -2  is to initialize the state at start  
 mudcsCommandCscLevel(dpName,"power_on","HV_1");
 if(CSC_fwG_g_IS_IMAX_SET)mudcsSetRampUpRateForAllChambers(substr(dpName,0,strpos(dpName,".")), 10); //imax
}
else if(value == 1 || (value==-1 && index >=1) /*|| (value==-2 && state_destin==true && index >=1)*/){  // -1, -2  is to initialize the state at start  
 mudcsCommandCscLevel(dpName,"standby","HV_1");
 if(CSC_fwG_g_IS_IMAX_SET)mudcsSetRampUpRateForAllChambers(substr(dpName,0,strpos(dpName,".")), 10); //imax
}
else if(value == 0 /*|| (value==-2 && state_destin==false && index >=1)*/){ //  -2  is to initialize the state at start  
 if(state_save ==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".status",-2);
 else mudcsCommandCscLevel(dpName,"power_off","HV_1");
}

if(value == 0 || value == 2 || ((value==-1 || value==-2) && index >=1) ){ 
 mudcsCommandCscLevel(dpName,"get_data","HV_1");
}

/////////// oshibka: ne nujno dlya HV_PR  !!! mudcsUpdateStatusForDevicesWithoutNonDeFactoStatus(dpName, value);

CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;
}
//============================================================================================
mudcsUpdateMonitor_HV_1_status(string dpName, int value){

string mudcs_alias;

/*
  dyn_string dyn_debug;

    dpGet("dyn_debug2.",dyn_debug);
    dynAppend(dyn_debug,dpName);
    dpSetWait("dyn_debug2.",dyn_debug);

  dynAppend(CSC_fwG_g_dyn_debug1,dpName);
*/


string test_string;

int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
//  return;
  } else {
    emu_info("Status update HV: " + dpName + "=" + value);
  }
// else{if(CSC_fwG_g_CSC_INTERLOCK_FLAG)return;}

//----
/*
 if(index >=1){
  dyn_string dyn_debug;

    dpGet("dyn_debug2.",dyn_debug);
    dynAppend(dyn_debug,dpName);
    dpSetWait("dyn_debug2.",dyn_debug);

  dynAppend(CSC_fwG_g_dyn_debug1,dpName);
 }
*/
//----


/////DebugN("mudcsServer:mudcsUpdateMonitor_HV_1_status-> ***************************"+substr(dpName,0,strpos(dpName,".")));

// !!!! COMMENT: MULTY_MODE_DONE.value is set true when MUDCS_DOMAIN.value is defined
// !!!! it is needed to call all;all commands only once.
// !!!! (multy-mode is not used now: see below).




string domain;
string MULTY_MODE_DONE;

bool MULTY_MODE=false;


dpGet(CSC_fwG_g_SYSTEM_NAME+":MULTY_MODE_DONE.value",MULTY_MODE_DONE);
dpGet(CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value",domain);

int w_pos,radius,ich;
string type;

if(index >=1){ // first call
  MULTY_MODE=false;

}
else{

mudcs_alias=mudcsAliasNameGet(domain);
 mudcsNameToPosConvert(mudcs_alias/*domain*/, w_pos, radius, ich, type);
 if(type == "n_a"){
// !!!! 08/10/2004: Now the routine works only in the  MULTY_MODE=false mode:
// !!!! this is because we have e.g. the following problem:
// !!!! if the chamber "sends" the command, that results in "all;all" to all 
// !!!! devices of this type (devices on other chmbers)    
// !!! In further the following algorithm can be used to let usage of the
// !!! all;all commands:
// !!! the ne dp should be introduced, which has to be set to node name (CSC, disk, or chamber)
// !!! at the toem when  MUDCS_DOMAIN.value is set TRUE.
// !!! Then the node shoule be alylized here to determine its coordinates. 
// !!! If the node is CSC the above is not needed: we can send all;all
// !!! If the node is disk we need to look at which broker (correconding to PC)
// !!! is corresponded to this disk and send only the command for thsi broker
// !!! If the node is chamber: no way to use MULTY_MODE


///////// !!!!!!! temporal comment out  MULTY_MODE=true;
 }

}



//////DebugN("mudcsUpdateMonitor_HV_1_status:%%%%%%%%%%%%%%%% " +type + " %%%%%%%%%%%%%%%%% " + ich + " "+ MULTY_MODE + " "+domain + " %%%%%%%%%%%%%%%%% ");
//////DebugN("mudcsUpdateMonitor_HV_1_status:%%%%%%%%%%%%%%%% " +CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value" + " %%%%%%%%%%%%%%%%%11 ");

mudcs_alias=mudcsAliasNameGet(dpName);
mudcsNameToPosConvert(mudcs_alias/*dpName*/, w_pos, radius, ich, type); // to determine ich

 bool state_destin;
 dpGet(substr(dpName,0,strpos(dpName,"."))+".:_alert_hdl.._active",state_destin); // !!!! error here (wrong dp): anyway the mudcsFsmErrorSet does this function

int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".module_state",state_save);
//if(value==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".module_state",-2);

/*
//-------
  dyn_string dyn_debug;

    dpGet("dyn_debug2.",dyn_debug);
    dynAppend(dyn_debug, substr(dpName,0,strpos(dpName,"."))+".:_alert_hdl.._active");
    dynAppend(dyn_debug, "value"+value);
    dynAppend(dyn_debug, "state_destin"+state_destin);
    dpSetWait("dyn_debug2.",dyn_debug);

//------
*/
//------- 03/21/2005 -------------------------------------
if(value == 3 && index >=1){ // works only the the start
  test_string=substr(dpName,0,strpos(dpName,"."));
  dpSetWait(test_string+".status",2);
  return;
}
//--------------------------------------------------------

if(value == 2 || (value==-1 && index >=1) /*|| (value==-2 && state_destin==true && index >=1)*/){  // -1, -2  is to initialize the state at start
 
 if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("HV","all;all","power_on");
 else if(!MULTY_MODE)mudcsCommand("HV",dpName,"power_on");
 if(CSC_fwG_g_IS_IMAX_SET)mudcsHVCommand(substr(dpName,0,strpos(dpName,".")), 6, 10); // set 10mkA for Imax
//// moved: mudcsAlertSetActive(dpName,true); 

  emu_info("mudcsServer::mudcsUpdateMonitor_HV_1_status, sending ON command to HV: " + dpName + "=" + value);
}
else if(value == 0 /*|| (value==-2 && state_destin==false && index >=1)*/){ //  -2  is to initialize the state at start  

//// moved:  mudcsAlertSetActive(dpName,false);

//// if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("HV","all;all","power_off");
 if(state_save ==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".status",-2);// this all is to allow to switch off from error state and differ -2 and -1
 else if(!MULTY_MODE)mudcsCommand("HV",dpName,"power_off");

  emu_info("mudcsServer::mudcsUpdateMonitor_HV_1_status, sending OFF command to HV: " + dpName + "=" + value);
}


if(value == 0 || value == 2 || ((value==-1 || value==-2) && index >=1) ){ // comment !!! without "index >=1" give the indefinit loop 10/04/2004
 if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("HV","all;all","get_data");
 else if(!MULTY_MODE)mudcsCommand("HV",dpName,"get_data");
}


 MULTY_MODE_DONE = "TRUE";



CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;
}











//============================================================================================
//============================================================================================
mudcsUpdateMonitor_LV_1_status(string dpName, int value){
return;

string mudcs_alias;
string data;

// attention :: the order of power/on/off and setting alarm is essential !!! 
DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_LV_1_status:&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");

int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_LV_1_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER LV"+" "+dpName);
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
  

/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
/////  return;
 }
// else{if(CSC_fwG_g_CSC_INTERLOCK_FLAG)return;}

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_LV_1_status ***************************"+dpName+" = "+value);
/////DebugN("mudcsServer:mudcsUpdateMonitor_LV_1_status-> ***************************"+substr(dpName,0,strpos(dpName,".")));

// !!!! COMMENT: MULTY_MODE_DONE.value is set true when MUDCS_DOMAIN.value is defined
// !!!! it is needed to call all;all commands only once.
// !!!! (multy-mode is not used now: see below).




string domain;
string MULTY_MODE_DONE;

bool MULTY_MODE=false;


dpGet(CSC_fwG_g_SYSTEM_NAME+":MULTY_MODE_DONE.value",MULTY_MODE_DONE);
dpGet(CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value",domain);

int w_pos,radius,ich;
string type;

if(index >=1){ // first call
  MULTY_MODE=false;

}
else{

mudcs_alias=mudcsAliasNameGet(domain);
 mudcsNameToPosConvert(mudcs_alias/*domain*/, w_pos, radius, ich, type);
 if(type == "n_a"){
// !!!! 08/10/2004: Now the routine works only in the  MULTY_MODE=false mode:
// !!!! this is because we have e.g. the following problem:
// !!!! if the chamber "sends" the command, that results in "all;all" to all 
// !!!! devices of this type (devices on other chmbers)    
// !!! In further the following algorithm can be used to let usage of the
// !!! all;all commands:
// !!! the ne dp should be introduced, which has to be set to node name (CSC, disk, or chamber)
// !!! at the toem when  MUDCS_DOMAIN.value is set TRUE.
// !!! Then the node shoule be alylized here to determine its coordinates. 
// !!! If the node is CSC the above is not needed: we can send all;all
// !!! If the node is disk we need to look at which broker (correconding to PC)
// !!! is corresponded to this disk and send only the command for thsi broker
// !!! If the node is chamber: no way to use MULTY_MODE


///////// !!!!!!! temporal comment out  MULTY_MODE=true;
 }

}



////DebugN("mudcsUpdateMonitor_LV_1_status:%%%%%%%%%%%%%%%% " +type + " %%%%%%%%%%%%%%%%% " + ich + " "+ MULTY_MODE + " "+domain + " %%%%%%%%%%%%%%%%% ");
////DebugN("mudcsUpdateMonitor_LV_1_status:%%%%%%%%%%%%%%%% " +CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value" + " %%%%%%%%%%%%%%%%%11 ");

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_LV_1_status:%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " +dpName);


mudcs_alias=mudcsAliasNameGet(dpName);
mudcsNameToPosConvert(mudcs_alias/*dpName*/, w_pos, radius, ich, type); // to determine ich

 bool state_destin;
 dpGet(substr(dpName,0,strpos(dpName,"."))+".:_alert_hdl.._active",state_destin);// !!!! error here (wrong dp): anyway the mudcsFsmErrorSet does this function

 if(value==-2){
  mudcsConvertFsm2DataOne(dpName, data);
  mudcsAlertSetActive(data,true);
 }
 
int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".chamber_state",state_save);
//if(value==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".chamber_state",-2);

if(value == 2 || (value==-1 && index >=1) /*|| (value==-2 && state_destin==true && index >=1)*/){  // -1, -2  is to initialize the state at start  
 
 if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("LV","all;all","power_on");
 else if(!MULTY_MODE)mudcsCommand("LV",dpName,"power_on");

//// moved: mudcsAlertSetActive(dpName,true); 

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_LV_1_status == 2++***************************"+dpName+value);
}
else if(value == 0 /*|| (value==-2 && state_destin==false && index >=1)*/){ //  -2  is to initialize the state at start  

//// moved:  mudcsAlertSetActive(dpName,false);

////// if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("LV","all;all","power_off");
 if(state_save ==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".status",-2);// this all is to allow to switch off from error state and differ -2 and -1
 else if(!MULTY_MODE)mudcsCommand("LV",dpName,"power_off");

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_LV_1_status == 0++***************************"+dpName+value);
}


if(value == 0 || value == 2 || ((value==-1 || value==-2) && index >=1) ){ // comment !!! without "index >=1" give the indefinit loop 10/04/2004
 if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("LV","all;all","get_data");
 else if(!MULTY_MODE)mudcsCommand("LV",dpName,"get_data");
}


 MULTY_MODE_DONE = "TRUE";

CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;

}







//============================================================================================


//============================================================================================
//============================================================================================
mudcsUpdateMonitor_CRB_1_status(string dpName, int value){

string mudcs_alias;

// attention :: the order of power/on/off and setting alarm is essential !!! 
DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_CRB_1_status:&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");

int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_CRB_1_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER CRB_1"+" "+dpName);
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
  

/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
/////  return;
 }
// else{if(CSC_fwG_g_CSC_INTERLOCK_FLAG)return;}

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_CRB_1_status ***************************"+dpName+" = "+value);
/////DebugN("mudcsServer:mudcsUpdateMonitor_CRB_1_status-> ***************************"+substr(dpName,0,strpos(dpName,".")));

// !!!! COMMENT: MULTY_MODE_DONE.value is set true when MUDCS_DOMAIN.value is defined
// !!!! it is needed to call all;all commands only once.
// !!!! (multy-mode is not used now: see below).




string domain;
string MULTY_MODE_DONE;

bool MULTY_MODE=false;


dpGet(CSC_fwG_g_SYSTEM_NAME+":MULTY_MODE_DONE.value",MULTY_MODE_DONE);
dpGet(CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value",domain);

int w_pos,radius,ich;
string type;

if(index >=1){ // first call
  MULTY_MODE=false;

}
else{

mudcs_alias=mudcsAliasNameGet(domain);
 mudcsNameToPosConvert(mudcs_alias/*domain*/, w_pos, radius, ich, type);
 if(type == "n_a"){


 }

}

////DebugN("mudcsUpdateMonitor_CRB_1_status:%%%%%%%%%%%%%%%% " +type + " %%%%%%%%%%%%%%%%% " + ich + " "+ MULTY_MODE + " "+domain + " %%%%%%%%%%%%%%%%% ");
////DebugN("mudcsUpdateMonitor_CRB_1_status:%%%%%%%%%%%%%%%% " +CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value" + " %%%%%%%%%%%%%%%%%11 ");

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_CRB_1_status:%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " +dpName);


mudcs_alias=mudcsAliasNameGet(dpName);
mudcsNameToPosConvert(mudcs_alias/*dpName*/, w_pos, radius, ich, type); // to determine ich

 bool state_destin;
 dpGet(substr(dpName,0,strpos(dpName,"."))+".:_alert_hdl.._active",state_destin);// !!!! error here (wrong dp): anyway the mudcsFsmErrorSet does this function

int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".module_state",state_save);
//if(value==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".module_state",-2);

/*
dyn_string d_s2;
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,dpName);
    dpSetWait("dyn_debug2.",d_s2);
*/

if(value == 2 || (value==-1 && index >=1) /*|| (value==-2 && state_destin==true && index >=1)*/){  // -1, -2  is to initialize the state at start  
 
///    dynAppend(d_s2,"11111"+dpName);
///    dpSetWait("dyn_debug2.",d_s2);

 if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("CRB","all;all","power_on");
 else if(!MULTY_MODE){ mudcsCommand("CRB",dpName,"power_on");}

//// moved: mudcsAlertSetActive(dpName,true); 

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_CRB_1_status == 2++***************************"+dpName+value);
}
else if(value == 0 /*|| (value==-2 && state_destin==false && index >=1)*/){ //  -2  is to initialize the state at start  

//// moved:  mudcsAlertSetActive(dpName,false);

//// if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("CRB","all;all","power_off");
 if(state_save ==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".status",-2);// this all is to allow to switch off from error state and differ -2 and -1
 else if(!MULTY_MODE)mudcsCommand("CRB",dpName,"power_off");

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_CRB_1_status == 0++***************************"+dpName+value);
}


if(value == 0 || value == 2 || ((value==-1 || value==-2) && index >=1) ){ // comment !!! without "index >=1" give the indefinit loop 10/04/2004
 if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("CRB","all;all","get_data");
 else if(!MULTY_MODE)mudcsCommand("CRB",dpName,"get_data");
}


 MULTY_MODE_DONE = "TRUE";

CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;

}


//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================


mudcsUpdateMonitor_fwWnrCh_CSC_LV_status(string dpName, int value){




/*
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,dpName);
    dpSetWait("dyn_debug2.",d_s2);
*/

string mudcs_alias;

// attention :: the order of power/on/off and setting alarm is essential !!! 
emu_info("mudcsUpdateMonitor_fwWnrCh_CSC_LV_status: status update: " + dpName + "=" + value);

int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
  

/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
/////  return;
  }


/*
   dyn_string d_s2;
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,dpName+">>>"+mudcs_alias);
    dpSetWait("dyn_debug2.",d_s2);
*/

//return;

int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".module_state",state_save);
//if(value==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".module_state",-2);

if(value == 2 || (value==-1 && index >=1) /*|| (value==-2 && state_destin==true && index >=1)*/){  // -1, -2  is to initialize the state at start  
 
 // mudcsSnmpCommand("fwWnrCh_CSC_LV",dpName,"power_on");

emu_info("mudcsServer::mudcsUpdateMonitor_fwWnrCh_CSC_LV_status command: " + dpName + "=" + value);
}
else if(value == 0 /*|| (value==-2 && state_destin==false && index >=1)*/){ //  -2  is to initialize the state at start  

 if(state_save ==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".status",-2);// this all is to allow to switch off from error state and differ -2 and -1
 else {/*mudcsSnmpCommand("fwWnrCh_CSC_LV",dpName,"power_off");*/}

emu_info("mudcsServer::mudcsUpdateMonitor_fwWnrCh_CSC_LV_status command: " + dpName + "=" + value);
}


if(value == 0 || value == 2 || ((value==-1 || value==-2) && index >=1) ){ // comment !!! without "index >=1" give the indefinit loop 10/04/2004
 mudcsSnmpCommand("fwWnrCh_CSC_LV",dpName,"get_data");
}


CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;

}
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================


mudcsUpdateMonitor_fwWnrCr_CSC_LV_status(string dpName, int value){




/*
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,dpName);
    dpSetWait("dyn_debug2.",d_s2);
*/

string mudcs_alias;

// attention :: the order of power/on/off and setting alarm is essential !!! 
emu_info("mudcsUpdateMonitor_fwWnrCr_CSC_LV_status: status update: " + dpName + "=" + value);

int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
  

/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
/////  return;
 }


/*
   dyn_string d_s2;
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,dpName+">>>"+mudcs_alias);
    dpSetWait("dyn_debug2.",d_s2);
*/


//return;

int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".module_state",state_save);
//if(value==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".module_state",-2);


if(value == 2 || (value==-1 && index >=1) /*|| (value==-2 && state_destin==true && index >=1)*/){  // -1, -2  is to initialize the state at start  
 
 mudcsSnmpCommand("fwWnrCr_CSC_LV",dpName,"power_on");

emu_info("mudcsServer::mudcsUpdateMonitor_fwWnrCr_CSC_LV_status command ON: " + dpName + "=" + value);
}
else if(value == 3 || (value==-1 && index >=1) /*|| (value==-2 && state_destin==true && index >=1)*/){  // -1, -2  is to initialize the state at start  
 
 mudcsSnmpCommand("fwWnrCr_CSC_LV",dpName,"power_on_all");

emu_info("mudcsServer::mudcsUpdateMonitor_fwWnrCr_CSC_LV_status command ALL ON: " + dpName + "=" + value);
}
else if(value == 0 /*|| (value==-2 && state_destin==false && index >=1)*/){ //  -2  is to initialize the state at start  

 if(state_save ==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".status",-2);// this all is to allow to switch off from error state and differ -2 and -1
 else mudcsSnmpCommand("fwWnrCr_CSC_LV",dpName,"power_off");

emu_info("mudcsServer::mudcsUpdateMonitor_fwWnrCr_CSC_LV_status command OFF: " + dpName + "=" + value);
}


if(value == 0 || value >= 2 || ((value==-1 || value==-2) && index >=1) ){ // comment !!! without "index >=1" give the indefinit loop 10/04/2004
 mudcsSnmpCommand("fwWnrCr_CSC_LV",dpName,"get_data");
}


CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;

}

//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================


mudcsUpdateMonitor_MRTN_1_status(string dpName, int value){




/*
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,dpName);
    dpSetWait("dyn_debug2.",d_s2);
*/

string mudcs_alias;

// attention :: the order of power/on/off and setting alarm is essential !!! 
DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_MRTN_1_status:&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");

int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_MRTN_1_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER MRTN_1"+" "+dpName);
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
  

/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
/////  return;
 }
// else{if(CSC_fwG_g_CSC_INTERLOCK_FLAG)return;}

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_MRTN_1_status ***************************"+dpName+" = "+value);
/////DebugN("mudcsServer:mudcsUpdateMonitor_MRTN_1_status-> ***************************"+substr(dpName,0,strpos(dpName,".")));

// !!!! COMMENT: MULTY_MODE_DONE.value is set true when MUDCS_DOMAIN.value is defined
// !!!! it is needed to call all;all commands only once.
// !!!! (multy-mode is not used now: see below).




string domain;
string MULTY_MODE_DONE;

bool MULTY_MODE=false;


dpGet(CSC_fwG_g_SYSTEM_NAME+":MULTY_MODE_DONE.value",MULTY_MODE_DONE);
dpGet(CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value",domain);

int w_pos,radius,ich;
string type;

if(index >=1){ // first call
  MULTY_MODE=false;

}
else{

mudcs_alias=mudcsAliasNameGet(domain);
 mudcsNameToPosConvert(mudcs_alias/*domain*/, w_pos, radius, ich, type);
 if(type == "n_a"){


 }

}

////DebugN("mudcsUpdateMonitor_MRTN_1_status:%%%%%%%% " +type + " %%%%%% " + ich + " "+ MULTY_MODE + " "+domain + " %%%%");
////DebugN("mudcsUpdateMonitor_MRTN_1_status:%%%%%%%% " +CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value" + " %%%%%%%%%%%11 ");

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_MRTN_1_status:%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " +dpName);



mudcs_alias=mudcsAliasNameGet(dpName);
mudcsNameToPosConvert(mudcs_alias/*dpName*/, w_pos, radius, ich, type); // to determine ich

/*
   dyn_string d_s2;
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,dpName+">>>"+mudcs_alias);
    dpSetWait("dyn_debug2.",d_s2);
*/

 bool state_destin;
 dpGet(substr(dpName,0,strpos(dpName,"."))+".:_alert_hdl.._active",state_destin);// !!!! error here (wrong dp): anyway the mudcsFsmErrorSet does this function

int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".module_state",state_save);
//if(value==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".module_state",-2);

if(value == 2 || (value==-1 && index >=1) /*|| (value==-2 && state_destin==true && index >=1)*/){  // -1, -2  is to initialize the state at start  
 
 if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("MRTN","all;all","power_on");
 else if(!MULTY_MODE)mudcsCommand("MRTN",dpName,"power_on");

//// moved: mudcsAlertSetActive(dpName,true); 

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_MRTN_1_status == 2++***************************"+dpName+value);
}
else if(value == 0 /*|| (value==-2 && state_destin==false && index >=1)*/){ //  -2  is to initialize the state at start  

//// moved:  mudcsAlertSetActive(dpName,false);

///// if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("MRTN","all;all","power_off");

 if(state_save ==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".status",-2);// this all is to allow to switch off from error state and differ -2 and -1
 else if(!MULTY_MODE)mudcsCommand("MRTN",dpName,"power_off");

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_MRTN_1_status == 0++***************************"+dpName+value);
}


if(value == 0 || value == 2 || ((value==-1 || value==-2) && index >=1) ){ // comment !!! without "index >=1" give the indefinit loop 10/04/2004
 if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("MRTN","all;all","get_data");
 else if(!MULTY_MODE)mudcsCommand("MRTN",dpName,"get_data");
}


 MULTY_MODE_DONE = "TRUE";

CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;

}

//============================================================================================
//============================================================================================

mudcsUpdateMonitor_WNR12_1_status(string dpName, int value){

string mudcs_alias;

// attention :: the order of power/on/off and setting alarm is essential !!! 
DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_WNR12_1_status:&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");

int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
  

/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
/////  return;
 }
// else{if(CSC_fwG_g_CSC_INTERLOCK_FLAG)return;}

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_WNR12_1_status ***************************"+dpName+" = "+value);
/////DebugN("mudcsServer:mudcsUpdateMonitor_WNR12_1_status-> ***************************"+substr(dpName,0,strpos(dpName,".")));

// !!!! COMMENT: MULTY_MODE_DONE.value is set true when MUDCS_DOMAIN.value is defined
// !!!! it is needed to call all;all commands only once.
// !!!! (multy-mode is not used now: see below).




string domain;
string MULTY_MODE_DONE;

bool MULTY_MODE=false;


dpGet(CSC_fwG_g_SYSTEM_NAME+":MULTY_MODE_DONE.value",MULTY_MODE_DONE);
dpGet(CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value",domain);

int w_pos,radius,ich;
string type;

if(index >=1){ // first call
  MULTY_MODE=false;

}
else{

mudcs_alias=mudcsAliasNameGet(domain);
 mudcsNameToPosConvert(mudcs_alias/*domain*/, w_pos, radius, ich, type);
 if(type == "n_a"){


 }

}

////DebugN("mudcsUpdateMonitor_WNR12_1_status:%%%%%%%% " +type + " %%%%%% " + ich + " "+ MULTY_MODE + " "+domain + " %%%%");
////DebugN("mudcsUpdateMonitor_WNR12_1_status:%%%%%%%% " +CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value" + " %%%%%%%%%%%11 ");

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_WNR12_1_status:%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " +dpName);


mudcs_alias=mudcsAliasNameGet(dpName);
mudcsNameToPosConvert(mudcs_alias/*dpName*/, w_pos, radius, ich, type); // to determine ich

 bool state_destin;
 dpGet(substr(dpName,0,strpos(dpName,"."))+".:_alert_hdl.._active",state_destin);// !!!! error here (wrong dp): anyway the mudcsFsmErrorSet does this function

int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".module_state",state_save);
//if(value==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".module_state",-2);

if(value == 2 || (value==-1 && index >=1) /*|| (value==-2 && state_destin==true && index >=1)*/){  // -1, -2  is to initialize the state at start  
 
 if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("WNR12","all;all","power_on");
 else if(!MULTY_MODE)mudcsCommand("WNR12",dpName,"power_on");

//// moved: mudcsAlertSetActive(dpName,true); 

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_WNR12_1_status == 2++***************************"+dpName+value);
}
else if(value == 0 /*|| (value==-2 && state_destin==false && index >=1)*/){ //  -2  is to initialize the state at start  

//// moved:  mudcsAlertSetActive(dpName,false);

///// if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("WNR12","all;all","power_off");
 if(state_save ==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".status",-2);// this all is to allow to switch off from error state and differ -2 and -1
 else if(!MULTY_MODE)mudcsCommand("WNR12",dpName,"power_off");

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_WNR12_1_status == 0++***************************"+dpName+value);
}


if(value == 0 || value == 2 || ((value==-1 || value==-2) && index >=1) ){ // comment !!! without "index >=1" give the indefinit loop 10/04/2004
 if(MULTY_MODE && MULTY_MODE_DONE == "FALSE")mudcsCommand("WNR12","all;all","get_data");
 else if(!MULTY_MODE)mudcsCommand("WNR12",dpName,"get_data");
}


 MULTY_MODE_DONE = "TRUE";

CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;

}

//============================================================================================
mudcsUpdateMonitor_fwGasSystem_CSC_GAS_status(string dpName, int value){


int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
//  return;
  } else {
    emu_info("mudcsUpdateMonitor_fwGasSystem_CSC_GAS_status: status update: " + dpName + "=" + value);
  }

int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".chamber_state",state_save);

if(value == 0 || value == 2 || ((value==-1 || value==-2) && index >=1) ){ 
mudcsGasCoolingCommand("GASSYSTEM",dpName,"get_data");
}
/*
   dyn_string d_s2;
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,dpName);
    dpSetWait("dyn_debug2.",d_s2);
*/
mudcsUpdateStatusForDevicesWithoutNonDeFactoStatus(dpName, value, state_save);
CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;
}
//============================================================================================
//============================================================================================
mudcsUpdateMonitor_fwCooling_CSC_COOLING_status(string dpName, int value){


int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
//  return;
  } else {
    emu_info("mudcsUpdateMonitor_fwCooling_CSC_COOLING_status: status update: " + dpName + "=" + value);
  }

int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".chamber_state",state_save);

if(value == 0 || value == 2 || ((value==-1 || value==-2) && index >=1) ){ 
mudcsGasCoolingCommand("COOLING",dpName,"get_data");
}
/*
   dyn_string d_s2;
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,dpName);
    dpSetWait("dyn_debug2.",d_s2);
*/
mudcsUpdateStatusForDevicesWithoutNonDeFactoStatus(dpName, value, state_save);
CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;
}
//============================================================================================
//============================================================================================
mudcsUpdateMonitor_ALNM_1_status(string dpName, int value){


int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_ALNM_1_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER ALNM_1");
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
//  return;
 }

int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".module_state",state_save);
//if(value==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".module_state",-2);

if(value == 0 || value == 2 || ((value==-1 || value==-2) && index >=1) ){ 
mudcsCommand("ALNM",dpName,"get_data");
}
/*
   dyn_string d_s2;
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,dpName);
    dpSetWait("dyn_debug2.",d_s2);
*/
mudcsUpdateStatusForDevicesWithoutNonDeFactoStatus(dpName, value, state_save);
CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;
}
//============================================================================================
//============================================================================================

mudcsUpdateMonitor_FED_1_status(string dpName, int value){

string data;
  
int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_FED_1_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER FED");
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
//  return;
 }

 if(value==-2){
  mudcsConvertFsm2DataOne(dpName, data);
  mudcsAlertSetActive(data,true);
 }
 
int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".chamber_state",state_save);
//if(value==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".chamber_state",-2);
//DebugTN("mudcsUpdateMonitor_FED_1_status:++++++++++++ "+data+ " "+ dpName+" "+value);
mudcsUpdateStatusForDevicesWithoutNonDeFactoStatus(dpName, value, state_save);

CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;
}
//============================================================================================

mudcsUpdateMonitor_TEMP_1_status(string dpName, int value){

string data;
  
int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_TEMP_1_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER TEMP");
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
//  return;
 }

 if(value==-2){
  mudcsConvertFsm2DataOne(dpName, data);
  mudcsAlertSetActive(data,true);
 }
 
int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".chamber_state",state_save);
//if(value==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".chamber_state",-2);

mudcsUpdateStatusForDevicesWithoutNonDeFactoStatus(dpName, value, state_save);

CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;
}
//============================================================================================
mudcsUpdateMonitor_CHIP_1_status(string dpName, int value){

int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_CHIP_1_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER CHIP");
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
//  return;
 }

int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".chamber_state",state_save);
//if(value==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".chamber_state",-2);

mudcsUpdateStatusForDevicesWithoutNonDeFactoStatus(dpName, value, state_save);
CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;
}

//============================================================================================
mudcsUpdateMonitor_GAS_SX5_status(string dpName, int value){


int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_GAS_SX5_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER GAS_SX5");
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
//  return;
 }



if(value == 0 || value == 2 || ((value==-1 || value==-2) && index >=1) ){ 
mudcsCommandCscLevel(dpName,"get_data","GAS_SX5");
}

mudcsUpdateStatusForDevicesWithoutNonDeFactoStatus(dpName, value, 0);
CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;
}
//============================================================================================
mudcsUpdateMonitor_WTH_SX5_status(string dpName, int value){

//////DebugN("............mudcsUpdateMonitor_WTH_SX5_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER WTH_SX5");

int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_WTH_SX5_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER WTH_SX5");
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
//  return;
 }

int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".module_state",state_save);
//if(value==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".module_state",-2);

if(value == 0 || value == 2 || ((value==-1 || value==-2) && index >=1) ){ 
mudcsCommandCscLevel(dpName,"get_data","WTH_SX5");
}

mudcsUpdateStatusForDevicesWithoutNonDeFactoStatus(dpName, value, state_save);
CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;
}
//============================================================================================
mudcsUpdateMonitor_PT100_status(string dpName, int value){

//////DebugN("............mudcsUpdateMonitor_PT100_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER PT100");

int index=dynContains(CSC_fwG_g_STATUS_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_PT100_status:+++++++++++++++++++++++++++++++++++++++++++++++++++++++FIRST CALL OF HANDLER PT100");
  dynRemove(CSC_fwG_g_STATUS_HANDLERS_FIRST,index);
/// !!!! the return below is commented out because the last status (remained at server interruption) SHOULD BE "EXECUTED" 
// to restore the previous system state JUST IN CASE
//  return;
 }

int state_save;
dpGet(substr(dpName,0,strpos(dpName,"."))+".module_state",state_save);
//if(value==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".module_state",-2);

if(value == 0 || value == 2 || ((value==-1 || value==-2) && index >=1) ){ 
mudcsCommandCscLevel(dpName,"get_data","PT100");
}

mudcsUpdateStatusForDevicesWithoutNonDeFactoStatus(dpName, value, state_save);
CSC_fwG_g_BLOCK_NEXT_DPCONNECT=0;
}

//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================

mudcsUpdateMonitor_HV_PR_new(string dpName, string value){

//DebugN("------------");
while(CSC_g_BLOCK_WATCHDOG_THREAD){}
  
int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
 }

emu_info("mudcsUpdateMonitor_HV_PR_new: status update: " + dpName + "=" + value);

// ---------------
//DebugN("******************** HV_PR PVSS CONFIRMATION******************");
//dpSet(CSC_fwG_g_SYSTEM_NAME+":HV_1_COM.command","CONFIRMATION");
//DebugN("******************** HV_PR PVSS CONFIRMATION******************");
//// return; // !!!!!!!!!
// ---------------

HV_primary_data_handler(dpName);

}
//============================================================================================
//============================================================================================
//============================================================================================

//============================================================================================
mudcsUpdateMonitor_HV_1_new(string dpName, string value){
 mudcsUpdateMonitor_HV_N_new("HV_1", dpName, value, "");

//watch_for_alive("HV_1_DimBroker");

}
//============================================================================================
mudcsUpdateMonitor_HV_2_new(string dpName, string value){
 mudcsUpdateMonitor_HV_N_new("HV_2", dpName, value, "");

watch_for_alive(dpName);

}
//============================================================================================
mudcsUpdateMonitor_HV_3_new(string dpName, string value){
 mudcsUpdateMonitor_HV_N_new("HV_3", dpName, value, "");

watch_for_alive(dpName);

}
//============================================================================================

mudcsUpdateMonitor_HV_N_new(string subtype, string dpName, string value, string isconf){

 while(CSC_g_BLOCK_WATCHDOG_THREAD){} 
  
int status_source, status_destin;
int chamber_complex_status;
bool isDpNameActive;
int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
 }
emu_info("mudcsUpdateMonitor_HV_N_new: status update: " + dpName + "=" + value + ", subtype=" + subtype + ", isconf=" + isconf);


int pos;
int w_pos,radius;
string label;

int ret,i, i_module_part;
dyn_string test_dyn_string;
string test_string, test_string1, test_string2, t_string;
string test_string_status;
int test_int;
bool found;
int index;

label="HV_1";

int host_hostid,host_slot,module_number,module_type;


//-------

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];
 if((pos=strpos(dpName,":"))>=0){

  test_string_status=CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/"+substr(d_test[1],pos+1);

 }

 string coord;
 dyn_string d_coord;
 dpGet(test_string_status+".coord",coord);
 d_coord=strsplit(coord,";");
 i_module_part = d_coord[4];

//-------

  dpGet(test_string+".data.host_hostid", test_int);
  host_hostid=test_int;
test_string1=test_string1+test_int;
  dpGet(test_string+".data.host_slot", test_int);
  host_slot=test_int;
test_string1=test_string1+";"+test_int;
  dpGet(test_string+".data.module_number", test_int);
  module_number=test_int;
test_string1=test_string1+";"+test_int;

  dpGet(test_string+".data.module_type", test_int);
  module_type=test_int;

 emu_debug("mudcsUpdateMonitor_HV_N_new: MODULE TYPE "+module_type, emu_DEBUG_DETAIL);
  if(module_type==1){

    return; // if master we do not go further 
  }

  if( host_hostid <=0 )return; // not to receive a corrupted data !!!!!!!

////// for(i_module_part=1;i_module_part<=2;i_module_part++){

 if(module_type == 3)test_string2=test_string1+";"+i_module_part;
 else if(module_type == 2){test_string2=test_string1+";"+"0";i_module_part=2;}

//---
//---
/////DebugTN("mudcsServer::mudcsUpdateMonitor_HV_N_new-->test_string_status "+test_string_status);

 int current_status;
 int update_mode;  // 2 - on a change; 1 - regular (once per 2 hours) 
 dpGet(test_string_status+".status",current_status);
 dpGet(test_string+".update_value",update_mode);
 if(update_mode !=1 && update_mode !=2)update_mode=2; // very temporal (server is not restarted after change) 
 if(dpExists(test_string_status+".reserve1") /*&& update_mode ==2*/)dpSet(test_string_status+".reserve1", update_mode);
 
 
 
 int status_save;
 dpGet(test_string_status+".module_state",status_save);
 if(status_save==-2)dpSetWait(test_string_status+".module_state",-22); // this all is to allow to switch off from error state and differ -2 and -1

// ======== 10/16/2004 ====== it is needed when the pvss runs, control app. runs then quits, the starts up again  
dpGet(test_string+".data.module_state",status_source);
//------------------------------------------------------------------------------------------------
 if(module_type == 3){
  if(i_module_part==1)status_source=status_source%1000;
  else status_source=status_source/1000;
 }
 else if(module_type == 2){
  status_source=status_source;
 }
 else if(module_type < 1 || module_type > 3){ // case of the LV of HV is off
   status_source=1; // to cock the alert active
 }

 dpGet(test_string_status+".module_state",status_destin);       // attention: different from other types (without de-facto status)
//---------------------------------------------------------------------------------------------
//==========================
 //int master_channel_status=mudcsGetMasterChannelStatus(test_string); 
 //if(status_source==1 && master_channel_status==0){
//   mudcsHVMasterChannelSwitch(test_string_status+".module_state", true);
   
 //}
//==========================
//////DebugTN("mudcsServer::mudcsUpdateMonitor_HV_N_new-->test_string_status 1*");

// ======== 03/21/2005 ====== it is needed to indicate that ramp up is finished and we can switch to FSM ON state ===
dpGet(test_string+".data.chamber_complex_status",chamber_complex_status);
//------------------------------------------------------------------------------------------------
 if(module_type == 3){
  if(i_module_part==1)chamber_complex_status=chamber_complex_status%1000;
  else chamber_complex_status=chamber_complex_status/1000;
 }
 else if(module_type == 2){
  chamber_complex_status=chamber_complex_status;
 }
//---------------------------------
 string primary_status;
 time last_set_primary_status;
 int chamber_index;
  mudcsGetPrimaryFsmForChamberFsm(test_string_status, primary_status, last_set_primary_status);
  chamber_index=dynContains(CSC_fwG_so_chambers_fsm,test_string_status);
 /*
  if(strpos(test_string_status,"P12_C03")>=0){ 
  dyn_string dyn_debug3;
  dyn_debug3[1]=test_string_status;
    dyn_debug3[2]=CSC_fwG_so_chambers_fsm[1];
         dyn_debug3[3]=   chamber_index;
        dyn_debug3[4]=last_set_primary_status;
          if(chamber_index>=1)dyn_debug3[5]=  CSC_fwG_so_get_chambers_time[chamber_index];  
   dpSet("dyn_debug3.",dyn_debug3);
  
  }
  */
  
  if(chamber_index>=1  && current_status==3 && update_mode ==2
    /* &&  last_set_primary_status > (CSC_fwG_so_get_chambers_time[chamber_index]-200 )*/ ){
    if(CSC_fwG_g_IS_IMAX_SET)if(primary_status<2)mudcsHVCommand(test_string_status, 6, 1); // imax
   //\\ else mudcsHVCommand(test_string_status, 6, 10);
   }

//--------------------- cause simetimes the status ON comes form server before the ramping !!!!! -------------
 time last_set_status;
 dpGet(test_string_status+".status:_online.._stime",last_set_status);

   if(current_status == 2 && chamber_complex_status == 1){
     if((getCurrentTime()-last_set_status) > 30){
       if(CSC_fwG_g_IS_IMAX_SET)if(primary_status<2)mudcsHVCommand(test_string_status, 6, 1); // set 1mkA for Imax 
      if(no_auto_restor_add2 && status_source==5){}
      else dpSetWait(test_string_status+".status",3);

     }
     else mudcsCommand("HV",test_string_status,"get_data");
   }
   else if(current_status == 3 && chamber_complex_status != 1){
      if(no_auto_restor_add2 && status_source==5){}      
      else dpSetWait(test_string_status+".status",2); // sometime the first data received after switching ON contains status ON in all channels -- then channels go to the RAMP_UP

   }

//------------------------------------------------------------------------------------------------------------

//////// else if(current_status == 3 && chamber_complex_status != 1)dpSetWait(test_string_status+".status",2); // cause simetimes the status ON comes form server before the ramping !!!!!
//---------------------------------------------------------------------------------------------
//====================================================================================================================

///////DebugTN("mudcsServer::mudcsUpdateMonitor_HV_N_new-->test_string_status 2*");

// ==========================

 if(current_status != -2){ // this is not to take data (and especially to change alert.active before setting initial state of the device) 

//------------------------------------------------------------------------------------------------
 ///// dpGet(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_DimBroker_o.module_state",status_source); // attention: different from other types (without de-facto status)
////// dpGet(test_string+".module_state",status_destin);                           // attention: different from other types (without de-facto status)

// -------------  attention: different from other types (without de-facto status) -------------------------

if(status_source==5)adjust_to_control_part(test_string_status, current_status, status_source);
else if(status_source > 0 && status_source != status_destin){
                                          if(current_status != 0){ // case of sombody switch the electronics by standalone or other tool
                                                             mudcsAlertSetActive(test_string,true);
                                                         dpSetWait(test_string_status+".module_state",status_source);
                                          }
                                                         adjust_to_control_part(test_string_status, current_status, status_source);
 }
 else  if(status_source == 0 && status_source != status_destin){
                                           if(current_status == 0){ // case of sombody switch the electronics by standalone or other tool
                                                             mudcsAlertSetActive(test_string,false);
                                                                dpSetWait(test_string_status+".module_state",status_source);
                                           }
                                                         adjust_to_control_part(test_string_status, current_status, status_source);
 }
// ----------------------------------------------------------------------------------------------------


/////DebugTN("mudcsServer::mudcsUpdateMonitor_HV_N_new-->test_string_status 3*");

int copy_error;

/////dpCopyOriginal(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_DimBroker_o.data",test_string+".data",copy_error);
/////if(copy_error !=0 )DebugTN("copy error: "+subtype+"_DimBroker_o.data");

///dpSetWait(test_string+".data.module_state",status_source);
///dpSetWait(test_string+".data.chamber_complex_status",chamber_complex_status);
//////dpSetWait(test_string+".update_value",10);



       

} //  if(current_status != -2) 
else if(status_source==5 && no_auto_restor)adjust_to_control_part(test_string_status, current_status, status_source);
//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(test_string_status); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet(subtype, test_string_status, test_string); // moved up (before CONFIRMATION) 10/05/2004

int previous_status=current_status;
dpGet(test_string_status+".status",current_status); // to check if status set to -1
if(previous_status >=0 && current_status == -1){
  ////system("mail "+CSC_fwG_g_EMAILS_HV+" < "+CSC_fwG_g_project_name_home+"/source/"+CSC_fwG_g_Component+"/text/HV_mail_trip.txt &");
}
//-----------------------------------------------------------------------------------------------------

//////// if(i_module_part==2)dpSetWait(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_COM"+isconf+".command","CONFIRMATION");


//////} // for(i_module_part=1;i_module_part<=2;i_module_part++)

CSC_fwG_so_get_chambers_time[chamber_index]=getCurrentTime();



}

//============================================================================================
//============================================================================================

HV_primary_data_handler(string dpName){

string dp;
int hostid;
char port, addr;
int i;
int pos;
string t_string1, t_string2;

int status_source, status_destin;
string test_string, fsm;

string status_real_save;
int status_real, status_real_save_i; 
//    is_newPRFSM
    
/////  dyn_string dyn_debug, dyn_debug1;

//================================================================
 string subtype,label;
 int test_int;
 string test_string2;

 
 subtype=label="HV_PR";

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];
 mudcsConvertData2FsmOne(subtype, d_test[1], fsm);

//  dpGet(test_string+".dynatemTcpIp", test_string2);
//  dpGet(test_string+".setNumber", test_int);


//  test_string2=test_string2+";"+test_int;
//  if(strpos(test_string2,"HV_PRIMARY") <0 )return; // not to receive a corrupted data !!!!!!!
//================================================================

  ///////////////////////// dpGet(CSC_fwG_g_SYSTEM_NAME+":"+"HV_PR_DimBroker_o.data.hostid", hostid);
////////////////////// string label="HV_PR";



 ////////////////////// test_string=CSC_fwG_g_SYSTEM_NAME+":CSCHV_PR_primary"+hostid;
 ////////////////////// mudcsConvertData2FsmOne("HV_PR", test_string, fsm);

//////mudcsDebug(test_string2);



bool found=false;
dyn_string test_dyn_string;
dpGet(test_string+".data.hostid", hostid);
string test_string2="HV_PRIMARY"+";"+hostid;

//-------------------------------------------------
watch_for_alive(test_string);
//-------------------------------------------------

emu_debug("HV_primary_data_handler: test_string2 ="+test_string2, emu_DEBUG_DETAIL);
emu_debug("HV_primary_data_handler: test_string ="+test_string, emu_DEBUG_DETAIL);
emu_debug("HV_primary_data_handler: fsm ="+fsm, emu_DEBUG_DETAIL);

string t_string=CSC_fwG_g_SYSTEM_NAME+":Db_o.CscLevelDevicesCoordinates";
  dpGet(t_string,test_dyn_string);

 for(i=1; i<=dynlen(test_dyn_string); i++){

           pos=strpos(test_dyn_string[i]," ");           
           if(pos<0)continue;
           t_string1 = substr(test_dyn_string[i],0,pos);
           t_string2 = substr(test_dyn_string[i],pos+1);
   if(strpos(t_string2,test_string2)>=0){
    found=true;
    break;
   }

 } // for

 if(!found)return;
emu_debug("HV_primary_data_handler: found", emu_DEBUG_DETAIL);


//-------------------------------------------------
////////////////   dp=CSC_fwG_g_SYSTEM_NAME+":"+"CSCHV_PR_primary"+hostid;
//////////////// int copy_error;
//////////////// dpCopyOriginal(CSC_fwG_g_SYSTEM_NAME+":"+"HV_PR_DimBroker_o",dp,copy_error);
//////////////// if(copy_error !=0 )DebugTN("copy error: "+"HV_PR_DimBroker_o");
//////////////// dpSetWait(CSC_fwG_g_SYSTEM_NAME+":"+"HV_1"+"_COM"+""+".command","CONFIRMATION");
//------------------------------------------------------------------------------------------------
 dpGet(test_string+".data.output",status_source);    //
 dpGet(fsm+".chamber_state",status_destin); // 
//-----------------------------------------------------------------------------------------------
  dpGet(test_string+".status",status_real);
if(is_newPRFSM){
 
  dpGet(fsm+".coord",status_real_save);
  status_real_save_i=status_real_save;
  if(status_real_save_i != status_real
     || (status_real == 2 || status_real == 3))dpSetWait(fsm+".chamber_state",status_destin);
    dpSet(fsm+".coord",status_real);//
  
  emu_debug("HV_primary_data_handler: ----------->newPRFSM<---------------", emu_DEBUG_DETAIL);
}

//------------------------------------------------------------------------------------------------
 int current_status;
 dpGet(fsm+".status",current_status);

 int status_save;
 dpGet(fsm+".chamber_state",status_save);
 if(status_save==-2)dpSetWait(fsm+".chamber_state",-22);// this all is to allow to switch off from error state and differ -2 and -1

//--- step0:it is to switch on alert if the primary is switched on but sends the 0 as output -------------
////////////////////   int isPrSwitchedTo0=0;
//---------------------------------------------------------------------------------------------------------
// ======== 10/16/2004 ====== it is needed when the pvss runs, control app. runs then quits, the starts up again  

 if(current_status != -2){ // this is not to take data (and especially to change alert.active before setting initial state of the device) 

//--- step3:it is to switch on alert if the primary is switched on but sends the 0 as output -------------
//////////////////// if((current_status == 2 || current_status == -1) && status_source == 0 && (status_destin == 38 || status_destin == 39)){
////////////////////  status_source=39;
//////////////////// }
//--------------------------------------------------------------------------------------------------

// -------------  attention: different from other types (without de-facto status) -------------------------
 if((status_source > 0 && status_source != status_destin) || status_real==5){ // 5 is intrlock 

                                          if(current_status != 0){ // case of sombody switch the electronics by standalone or other tool
                                                         mudcsAlertSetActive(test_string,true);
                                                           if(status_real==5)dpSetWait(fsm+".chamber_state",1);
                                                           else dpSetWait(fsm+".chamber_state",status_source);
                                          }
                                                         adjust_to_control_part(fsm, current_status, status_source);
 }
 else  if(status_source == 0 && status_source != status_destin){

//--- step1:it is to switch on alert if the primary is switched on but sends the 0 as output -------------
////////////////////   if(current_status == 2 || current_status == -1)isPrSwitchedTo0=1;
//---------------------------------------------------------------------------------------------------------

                                           if(current_status == 0){ // case of sombody switch the electronics by standalone or other tool
                                                               mudcsAlertSetActive(test_string,false);
                                                               dpSetWait(fsm+".chamber_state",status_source);
                                           }
                                                         adjust_to_control_part(fsm, current_status, status_source);
 }

 } //  if(current_status != -2)
 else if(status_source==5 && no_auto_restor)adjust_to_control_part(fsm, current_status, status_source);
 
//--- step2:it is to switch on alert if the primary is switched on but sends the 0 as output -------------
//////////////////// if(isPrSwitchedTo0 && (current_status == 2 || current_status == -1) && status_source == 0){
////////////////////   mudcsAlertSetActive(test_string,true);
////////////////////   dpSetWait(fsm+".chamber_state",38);
////////////////////   mudcsCommandCscLevel(fsm+".status","get_data","HV_1");
//////////////////// }
//--------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(fsm); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet("HV_PR", fsm, test_string); // moved up (before CONFIRMATION) 10/05/2004
//-----------------------------------------------------------------------------------------------------

}

//============================================================================================
//============================================================================================
//============================================================================================

mudcsUpdateMonitor_CRB_1_new(string dpName, string value){
while(CSC_g_BLOCK_WATCHDOG_THREAD){} 
int status_source, status_destin;

int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_CRB_1_new: ------------------------------------------------FIRST CALL OF HANDLER CRB"+" "+dpName);
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
 }

int w_pos,radius;
string subtype, label;

int ret,i;
dyn_string test_dyn_string;
string test_string, t_string, test_string2;

string fsm; // ***

int test_int;
bool found;
int index;

subtype=label="CRB_1";

//------- 

int pos;

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];
/////////// if((pos=strpos(dpName,":"))>=0){
 mudcsConvertData2FsmOne(subtype, d_test[1], fsm);
/////////  fsm=CSC_fwG_g_SYSTEM_NAME+":"+"CRB/"+substr(d_test[1],pos+1);
///////// }

 string coord;
 dyn_string d_coord;
 dpGet(fsm+".coord",coord);

//-------

  dpGet(test_string+".dynatemTcpIp", test_string2);
  dpGet(test_string+".setNumber", test_int);

test_string2=test_string2+";"+test_int;
  if(strpos(test_string2,"LVCB") <0 )return; // not to receive a corrupted data !!!!!!!

DebugTN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_CRB_1_new 22222222222222222222222222222222222222222222"+ test_int+" "+test_string2);

//////mudcsDebug(test_string);

//------------------------------------------------------------------------------------------------
 dpGet(test_string+".data.status",status_source);    //
 dpGet(fsm+".module_state",status_destin); // 
//------------------------------------------------------------------------------------------------
 int current_status;
 dpGet(fsm+".status",current_status);

 int status_save;
 dpGet(fsm+".module_state",status_save);
 if(status_save==-2)dpSetWait(fsm+".module_state",-22);// this all is to allow to switch off from error state and differ -2 and -1

// ======== 10/16/2004 ====== it is needed when the pvss runs, control app. runs then quits, the starts up again  

 if(current_status != -2){ // this is not to take data (and especially to change alert.active before setting initial state of the device) 

// -------------  attention: different from other types (without de-facto status) -------------------------
 if(status_source > 0 && status_source != status_destin){
                                         if(current_status != 0){ // case of sombody switch the electronics by standalone or other tool
                                                         mudcsAlertSetActive(test_string,true);
                                                         dpSetWait(fsm+".module_state",status_source);
                                         }
                                                         adjust_to_control_part(fsm, current_status, status_source);


 }
 else  if(status_source == 0 && status_source != status_destin){
                                           if(current_status == 0){ // case of sombody switch the electronics by standalone or other tool 
                                                         mudcsAlertSetActive(test_string,false);
                                                         dpSetWait(fsm+".module_state",status_source);
                                           }
                                                         adjust_to_control_part(fsm, current_status, status_source);

 }
 else if(status_source == -1){
  adjust_to_control_part(fsm, current_status, status_source);
  return;
 }


// ----------------------------------------------------------------------------------------------------


 } //  if(current_status != -2)



//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(fsm); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet(subtype, fsm, test_string); // moved up (before CONFIRMATION) 10/05/2004
//-----------------------------------------------------------------------------------------------------

///////// dpSetWait(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_COM"+isconf+".command","CONFIRMATION");


watch_for_alive("CRB_1_DimBroker");
DebugTN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_CRB_1_new BROKER ********************************"+"CRB_1"+" "+subtype+ test_string);



}
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================

mudcsUpdateMonitor_fwWnrCh_CSC_LV_new(string dpName, string value){
while(CSC_g_BLOCK_WATCHDOG_THREAD){}
int status_source, status_destin;

int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
} else {
  emu_info("mudcsUpdateMonitor_fwWnrCh_CSC_LV_new: status update: " + dpName + "=" + value);
}


int w_pos,radius;
string subtype, label;

int ret,i,max_sense_volt;
dyn_string test_dyn_string;
string test_string, t_string, test_string2;

string fsm; // ***

int test_int;
bool found;
int index;

subtype=label="fwWnrCh_CSC_LV_new";

//------- 

int pos;

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];

 mudcsConvertData2FsmOne(subtype, d_test[1], fsm);


 string coord;
 dyn_string d_coord;
 dpGet(fsm+".coord",coord);

//-------

  dpGet(test_string+".max_sense_volt", max_sense_volt);


  if(max_sense_volt <=0 || max_sense_volt > 20)return; // not to receive a corrupted data !!!!!!!


//////mudcsDebug(test_string);

//------------------------------------------------------------------------------------------------
 dpGet(test_string+".outputOn",status_source);    //
 dpGet(fsm+".module_state",status_destin); // 
//------------------------------------------------------------------------------------------------
 int current_status;
 dpGet(fsm+".status",current_status);


int status_save;
 dpGet(fsm+".module_state",status_save);
 if(status_save==-2)dpSetWait(fsm+".module_state",-22);// this all is to allow to switch off from error state and differ -2 and -1

// ======== 10/16/2004 ====== it is needed when the pvss runs, control app. runs then quits, the starts up again  

 if(current_status != -2){ // this is not to take data (and especially to change alert.active before setting initial state of the device) 

// -------------  attention: different from other types (without de-facto status) -------------------------
 if(status_source > 0 && status_source != status_destin){
                                          if(current_status != 0){ // case of sombody switch the electronics by standalone or other tool 
                                                         //mudcsAlertSetActive(test_string,true);
                                                         dpSetWait(fsm+".module_state",status_source);
                                          }
                                                         adjust_to_control_part(fsm, current_status, status_source);


 }
 else  if(status_source == 0 && status_source != status_destin){
                                            if(current_status == 0){ // case of sombody switch the electronics by standalone or other tool 
                                                         //mudcsAlertSetActive(test_string,false);
                                                         dpSetWait(fsm+".module_state",status_source);
                                            }
                                                         adjust_to_control_part(fsm, current_status, status_source);


 }
 else if(status_source == -1){
  adjust_to_control_part(fsm, current_status, status_source);
  return;
 }

// ----------------------------------------------------------------------------------------------------


 } //  if(current_status != -2)



//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(fsm); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet(subtype, fsm, test_string); // moved up (before CONFIRMATION) 10/05/2004
//-----------------------------------------------------------------------------------------------------

///////// dpSetWait(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_COM"+isconf+".command","CONFIRMATION");


watch_for_alive("fwWnrCh_CSC_LV_DimBroker");


//--------------

}
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================

mudcsUpdateMonitor_fwWnrCr_CSC_LV_new(string dpName, string value){
while(CSC_g_BLOCK_WATCHDOG_THREAD){}
int status_source, status_destin;

int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
 } else {
  emu_info("mudcsUpdateMonitor_fwWnrCr_CSC_LV_new: status update: " + dpName + "=" + value);
 }


int w_pos,radius;
string subtype, label;

int ret,i, nodeID;
dyn_string test_dyn_string;
string test_string, t_string, test_string2, sysDescr;

string fsm; // ***

int test_int;
bool found;
int index;

subtype=label="fwWnrCr_CSC_LV_new";

//------- 

int pos;

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];

 mudcsConvertData2FsmOne(subtype, d_test[1], fsm);


 string coord;
 dyn_string d_coord;
 dpGet(fsm+".coord",coord);
 int nodeIDcoord=coord;
//-------

  dpGet(test_string+".nodeID", nodeID);
  dpGet(test_string+".sysDescr", sysDescr);

  if(strpos(sysDescr,"WIENER")<0)return; // not to receive a corrupted data !!!!!!!


//////mudcsDebug(test_string);

//------------------------------------------------------------------------------------------------
 dpGet(test_string+".mainOn",status_source);    //
 dpGet(fsm+".module_state",status_destin); // 
//------------------------------------------------------------------------------------------------
 int current_status;
 dpGet(fsm+".status",current_status);


int status_save;
 dpGet(fsm+".module_state",status_save);
 if(status_save==-2)dpSetWait(fsm+".module_state",-22);// this all is to allow to switch off from error state and differ -2 and -1

// ======== 10/16/2004 ====== it is needed when the pvss runs, control app. runs then quits, the starts up again  

 if(current_status != -2){ // this is not to take data (and especially to change alert.active before setting initial state of the device) 

// -------------  attention: different from other types (without de-facto status) -------------------------
 if(status_source > 0 && status_source != status_destin){
                                          if(current_status != 0){ // case of sombody switch the electronics by standalone or other tool 
                                                         mudcsAlertSetActive(test_string,true);
                                                         dpSetWait(fsm+".module_state",status_source);
                                          }
                                                         adjust_to_control_part(fsm, current_status, status_source);


 }
 else  if(status_source == 0 && status_source != status_destin){
                                            if(current_status == 0){ // case of sombody switch the electronics by standalone or other tool 
                                                         mudcsAlertSetActive(test_string,false);
                                                         dpSetWait(fsm+".module_state",status_source);
                                            }
                                                         adjust_to_control_part(fsm, current_status, status_source);


 }
 else if(status_source == -1){
  adjust_to_control_part(fsm, current_status, status_source);
  return;
 }

// ----------------------------------------------------------------------------------------------------


 } //  if(current_status != -2)



//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(fsm); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet(subtype, fsm, test_string); // moved up (before CONFIRMATION) 10/05/2004
//-----------------------------------------------------------------------------------------------------

///////// dpSetWait(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_COM"+isconf+".command","CONFIRMATION");


watch_for_alive("fwWnrCr_CSC_LV_DimBroker");


//--------------

}
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================

mudcsUpdateMonitor_MRTN_1_new(string dpName, string value){
while(CSC_g_BLOCK_WATCHDOG_THREAD){}
int status_source, status_destin;

int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_MRTN_1_new: ------------------------------------------------FIRST CALL OF HANDLER MRTN"+" "+dpName);
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
 }

int csc_maraton_is_auto;
dpGet("CSC_MARATON_IS_AUTO.",csc_maraton_is_auto);

int w_pos,radius;
string subtype, label;

int ret,i;
dyn_string test_dyn_string;
string test_string, t_string, test_string2;

string fsm; // ***

int test_int;
bool found;
int index;

subtype=label="MRTN_1";

//------- 

int pos;

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];

 mudcsConvertData2FsmOne(subtype, d_test[1], fsm);


 string coord;
 dyn_string d_coord;
 dpGet(fsm+".coord",coord);

//-------

  dpGet(test_string+".dynatemTcpIp", test_string2);
  dpGet(test_string+".setNumber", test_int);

test_string2=test_string2+";"+test_int;

  if(strpos(test_string2,"WIENER") <0 )return; // not to receive a corrupted data !!!!!!!

DebugTN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_MRTN_1_new 22222222222222222222222222222222222222222222"+ test_int+" "+test_string2);

//////mudcsDebug(test_string);

//------------------------------------------------------------------------------------------------
 dpGet(test_string+".data.general_status",status_source);    //
 dpGet(fsm+".module_state",status_destin); // 
//------------------------------------------------------------------------------------------------
 int current_status;
 dpGet(fsm+".status",current_status);


int status_save;
 dpGet(fsm+".module_state",status_save);
 if(status_save==-2)dpSetWait(fsm+".module_state",-22);// this all is to allow to switch off from error state and differ -2 and -1

// ======== 10/16/2004 ====== it is needed when the pvss runs, control app. runs then quits, the starts up again  

 if(current_status != -2){ // this is not to take data (and especially to change alert.active before setting initial state of the device) 

// -------------  attention: different from other types (without de-facto status) -------------------------
 if(status_source > 0 && status_source != status_destin){
                                          if(current_status != 0){ // case of sombody switch the electronics by standalone or other tool 
                                                         mudcsAlertSetActive(test_string,true);
                                                         dpSetWait(fsm+".module_state",status_source);
                                                         if(csc_maraton_is_auto){
                                                          stopThread(CSC_fwG_g_mrtn_threadID_disable);
                                                          CSC_fwG_g_mrtn_threadID_enable=startThread("threadSynchronizeMRTN2LV_1_enable");
                                                         }
                                          }
                                                         adjust_to_control_part(fsm, current_status, status_source);


 }
 else  if(status_source == 0 && status_source != status_destin){
                                            if(current_status == 0){ // case of sombody switch the electronics by standalone or other tool 
                                                         mudcsAlertSetActive(test_string,false);
                                                         dpSetWait(fsm+".module_state",status_source);
                                                         if(csc_maraton_is_auto){
                                                          stopThread(CSC_fwG_g_mrtn_threadID_enable);
                                                          CSC_fwG_g_mrtn_threadID_disable=startThread("threadSynchronizeMRTN2LV_1_disable");
                                                         }
                                            }
                                                         adjust_to_control_part(fsm, current_status, status_source);


 }
 else if(status_source == -1){
  adjust_to_control_part(fsm, current_status, status_source);
  return;
 }

// ----------------------------------------------------------------------------------------------------


 } //  if(current_status != -2)



//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(fsm); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet(subtype, fsm, test_string); // moved up (before CONFIRMATION) 10/05/2004
//-----------------------------------------------------------------------------------------------------

///////// dpSetWait(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_COM"+isconf+".command","CONFIRMATION");


watch_for_alive("MRTN_1_DimBroker");
DebugTN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_MRTN_1_new BROKER ********************************"+"MRTN_1"+" "+subtype+ test_string);


//--------------



if(csc_maraton_is_auto){

int current_status_save=current_status;
dpGet(fsm+".status",current_status); // as status may change in mudcsFsmErrorSet 

 if(current_status==-1 && current_status_save != current_status){
  stopThread(CSC_fwG_g_mrtn_threadID_enable);
  CSC_fwG_g_mrtn_threadID_disable=startThread("threadSynchronizeMRTN2LV_1_disable");
 }
}

//--------------
//-- maraton automatic curing: temporal ------------------------------
/*
int csc_maraton_is_auto;
dpGet("CSC_MARATON_IS_AUTO.",csc_maraton_is_auto);
if(csc_maraton_is_auto){
 dpGet(fsm+".status",current_status); // as status may change in mudcsFsmErrorSet 
if(current_status==-1){
 CSC_fwG_g_buffer_mrtn_commands=makeDynInt();
 dynAppend(CSC_fwG_g_buffer_mrtn_commands,0);
 dynAppend(CSC_fwG_g_buffer_mrtn_commands,2);
}
if(dynlen(CSC_fwG_g_buffer_mrtn_commands)>=1){
  dpSetWait(fsm+".status",CSC_fwG_g_buffer_mrtn_commands[1]);
  /////dynRemove(CSC_fwG_g_buffer_mrtn_commands,1);
}
} // if(csc_maraton_is_auto)
*/
//---------------------------------------------------------------------

}

//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================

mudcsUpdateMonitor_WNR12_1_new(string dpName, string value){
while(CSC_g_BLOCK_WATCHDOG_THREAD){}
int status_source, status_destin;

int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_WNR12_1_new: ------------------------------------------------FIRST CALL OF HANDLER WNR12"+" "+dpName);
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
 }

int w_pos,radius;
string subtype, label;

int ret,i;
dyn_string test_dyn_string;
string test_string, t_string, test_string2;

string fsm; // ***

int test_int;
bool found;
int index;

subtype=label="WNR12_1";

//------- 

int pos;

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];

 mudcsConvertData2FsmOne(subtype, d_test[1], fsm);


 string coord;
 dyn_string d_coord;
 dpGet(fsm+".coord",coord);

//-------

  dpGet(test_string+".dynatemTcpIp", test_string2);
  dpGet(test_string+".setNumber", test_int);

test_string2=test_string2+";"+test_int;

  if(strpos(test_string2,"WIENER") <0 )return; // not to receive a corrupted data !!!!!!!

DebugTN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_WNR12_1_new 22222222222222222222222222222222222222222222"+ test_int+" "+test_string2);

//////mudcsDebug(test_string);

//------------------------------------------------------------------------------------------------
 dpGet(test_string+".data.general_status",status_source);    //
 dpGet(fsm+".module_state",status_destin); // 
//------------------------------------------------------------------------------------------------
 int current_status;
 dpGet(fsm+".status",current_status);


int status_save;
 dpGet(fsm+".module_state",status_save);
 if(status_save==-2)dpSetWait(fsm+".module_state",-22);// this all is to allow to switch off from error state and differ -2 and -1

// ======== 10/16/2004 ====== it is needed when the pvss runs, control app. runs then quits, the starts up again  

 if(current_status != -2){ // this is not to take data (and especially to change alert.active before setting initial state of the device) 

// -------------  attention: different from other types (without de-facto status) -------------------------
 if(status_source > 0 && status_source != status_destin){
                                          if(current_status != 0){ // case of sombody switch the electronics by standalone or other tool 
                                                         mudcsAlertSetActive(test_string,true);
                                                         dpSetWait(fsm+".module_state",status_source);
                                          }
                                                         adjust_to_control_part(fsm, current_status, status_source);


 }
 else  if(status_source == 0 && status_source != status_destin){
                                            if(current_status == 0){ // case of sombody switch the electronics by standalone or other tool 
                                                         mudcsAlertSetActive(test_string,false);
                                                         dpSetWait(fsm+".module_state",status_source);
                                            }
                                                         adjust_to_control_part(fsm, current_status, status_source);


 }
 else if(status_source == -1){
  adjust_to_control_part(fsm, current_status, status_source);
  return;
 }

// ----------------------------------------------------------------------------------------------------


 } //  if(current_status != -2)



//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(fsm); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet(subtype, fsm, test_string); // moved up (before CONFIRMATION) 10/05/2004
//-----------------------------------------------------------------------------------------------------

///////// dpSetWait(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_COM"+isconf+".command","CONFIRMATION");


watch_for_alive("WNR12_1_DimBroker");
DebugTN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_WNR12_1_new BROKER ********************************"+"WNR12_1"+" "+subtype+ test_string);



}




//============================================================================================

mudcsUpdateMonitor_fwGasSystem_CSC_GAS_new(string dpName, string value){

while(CSC_g_BLOCK_WATCHDOG_THREAD){}
string subtype;

int i,pos,status_save;
string test_string;
string test_string_status; // ***
string fsm;
 
subtype="fwGasSystem_CSC_GAS";

string dpName_save=dpName;
dpName="CSC_GAS_MONITOR";
//-------

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];

  mudcsConvertData2FsmOne("fwGasSystem_CSC_GAS",test_string , fsm);
  test_string_status=fsm;
//========== action matrix ==============  
  if(is_GasMatrix){
   time current_time=getCurrentTime();
 // dyn_string d_test1=strsplit(dpName_save,":");
 // string test_string1;
 // if(dynlen(d_test1)==3)test_string1=d_test1[2];
 // else if(dynlen(d_test1)==2)test_string1=d_test1[1];
    
 // time last_set_state1;
  time last_set_state;
  
    
 // dpGet(test_string1+".state:_online.._stime",last_set_state1);
  /////if(current_time-last_set_state > 60){
  
  int state;
  int current_status;
  int time_interval=24*3600;
  dpGet(test_string+".State", state);
    dpGet(fsm+".status", current_status);
  if(state==100){
   dpGet(test_string+".State:_online.._stime",last_set_state);
 
   if(current_time-last_set_state> time_interval){
    if(current_status != -1)dpSetWait(fsm+".status",-1); 
   }
   else{
    if(current_status != 0)dpSetWait(fsm+".status",0);     
   }
  }
  else{
   if(current_status != 2)dpSetWait(fsm+".status",2); 
    
  }
 /* 
  dpGet(fsm+".status", current_status);
     dpGet(test_string+".State:_online.._stime",last_set_state);
  int t_diff=current_time-last_set_state;
  int current_time_i=current_time;
  int last_set_state_i=last_set_state;
    int t_diff_i=current_time_i-last_set_state_i;
  DebugTN("mudcsServer::mudcsUpdateMonitor_fwGasSystem_CSC_GAS_new 1 "+t_diff+" "+state+"  "+"  "+current_time+"  "+last_set_state);
  DebugTN("mudcsServer::mudcsUpdateMonitor_fwGasSystem_CSC_GAS_new 2 "+t_diff_i+" "+"  "+"  "+current_time_i+"  "+last_set_state_i);  
  */
  /////} // > 60
   } // is_GasMatrix
 
//===========================  
// if((pos=strpos(dpName,":"))>=0){
//  test_string_status=CSC_fwG_g_SYSTEM_NAME+":"+"GasCMS/"+substr(d_test[1],pos+1);
// }


//DebugTN("mudcsServer::mudcsUpdateMonitor_fwGasSystem_CSC_GAS_new 22222222222222222222222222222222222222222222"+dpName+">>>>>>>>>>"+test_string+" "+test_string_status+" "+d_test[1]);

//-------

// mudcsConvertData2FsmOne("fwGasSystem_CSC_GAS",test_string , fsm);

 dpGet(fsm+".chamber_state",status_save);
 if(status_save==-2)dpSetWait(fsm+".chamber_state",-22);// this all is to allow to switch off from error state and differ -2 and -1

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(test_string_status); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet(subtype, test_string_status, test_string); // moved up (before CONFIRMATION) 10/05/2004
//-----------------------------------------------------------------------------------------------------

watch_for_alive("fwGasSystem_CSC_GAS_DimBroker");

}



//============================================================================================
//============================================================================================

mudcsUpdateMonitor_fwCooling_CSC_COOLING_new(string dpName, string value){

while(CSC_g_BLOCK_WATCHDOG_THREAD){}
string subtype;

int i,pos,status_save;
string test_string;
string test_string_status; // ***
string fsm;
 
subtype="fwCooling_CSC_COOLING";

//-------

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];
 if((pos=strpos(dpName,":"))>=0){
  test_string_status=CSC_fwG_g_SYSTEM_NAME+":"+"CoolingCMS/"+substr(d_test[1],pos+1);
 }

//DebugTN("mudcsServer::mudcsUpdateMonitor_fwCooling_CSC_COOLING_new 22222222222222222222222222222222222222222222"+dpName+">>>>>>>>>>"+test_string+" "+test_string_status+" "+d_test[1]);

//-------

 mudcsConvertData2FsmOne("fwCooling_CSC_COOLING",test_string , fsm);
//========== action matrix ==============  

/*
//xxdepes[2] = makeDynString ("","Flowmeter_EndCap_Cooling_YE_Minus_1");
//xxdepes[3] = makeDynString ("","Flowmeter_EndCap_Cooling_YE_Plus_1");
//xxdepes[4] = makeDynString ("","Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Minus_1");
//xxdepes[5] = makeDynString ("","Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Minus_1");
//xxdepes[6] = makeDynString ("","Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Plus_1");
//xxdepes[7] = makeDynString ("","Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Plus_1");
 
  if(is_CoolingMatrix){
         
  float Flowmeter_EndCap_Cooling_YE_Minus_1;  
  float Flowmeter_EndCap_Cooling_YE_Plus_1;
      
  int current_status;
  
  dpGet(test_string+".Flowmeter_EndCap_Cooling_YE_Minus_1", Flowmeter_EndCap_Cooling_YE_Minus_1);
  dpGet(test_string+".Flowmeter_EndCap_Cooling_YE_Plus_1", Flowmeter_EndCap_Cooling_YE_Plus_1);
   
  if(Flowmeter_EndCap_Cooling_YE_Minus_1 < 200 || Flowmeter_EndCap_Cooling_YE_Plus_1 < 200){
   dpGet(fsm+".status", current_status);
   if(current_status != -1)dpSetWait(fsm+".status",-1); 
  }

  } // is_CoolingMatrix
*/ 
//===========================  
 dpGet(fsm+".chamber_state",status_save);
 if(status_save==-2)dpSetWait(fsm+".chamber_state",-22);// this all is to allow to switch off from error state and differ -2 and -1

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(test_string_status); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet(subtype, test_string_status, test_string); // moved up (before CONFIRMATION) 10/05/2004
//-----------------------------------------------------------------------------------------------------

watch_for_alive("fwCooling_CSC_COOLING_DimBroker");

}



//============================================================================================

//============================================================================================
//============================================================================================

mudcsUpdateMonitor_ALNM_1_new(string dpName, string value){
while(CSC_g_BLOCK_WATCHDOG_THREAD){}
int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_ALNM_1_new:------------------------------------------------FIRST CALL OF HANDLER ALNM");
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
 }


int w_pos,radius;
string label, subtype;

int ret,i;
dyn_string test_dyn_string;
string test_string, t_string, test_string2;

string test_string_status; // ***

int test_int;
bool found;
int index;


subtype=label="ALNM_1";

//------- // ***

int pos;

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];
 if((pos=strpos(dpName,":"))>=0){
  test_string_status=CSC_fwG_g_SYSTEM_NAME+":"+"ALNM/"+substr(d_test[1],pos+1);
 }

DebugTN("SHOULD NOT SEE THIS (shouldn't be called) " + dpName+">>>>>>>>>>"+test_string+" "+test_string2+" "+test_string_status+" "+d_test[1]);

 string coord;
 dyn_string d_coord;
 dpGet(test_string_status+".coord",coord);

//-------

  dpGet(test_string+".dynatemTcpIp", test_string2);
  dpGet(test_string+".setNumber", test_int);

 string fsm;
 mudcsConvertData2FsmOne("ALNM",test_string , fsm);
 int status_save;
 dpGet(fsm+".module_state",status_save);
 if(status_save==-2)dpSetWait(fsm+".module_state",-22);// this all is to allow to switch off from error state and differ -2 and -1



test_string2=test_string2+";"+test_int;
if(strpos(test_string2,"me") <0)return; // not to receive a corrupted data !!!!!!!
DebugTN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_ALNM_1_new 22222222222222222222222222222222222222222222"+ test_int+" "+test_string2);


//////mudcsDebug(test_string);


//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(test_string_status); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet(subtype, test_string_status, test_string); // moved up (before CONFIRMATION) 10/05/2004
//-----------------------------------------------------------------------------------------------------


DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_ALNM_1_new ********************************ALNM_1"+ test_string);

watch_for_alive("ALNM_1_DimBroker");

}



//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
mudcsUpdateMonitor_LV_SX5_new(string dpName, string value){
 mudcsUpdateMonitor_LV_N_new("LV_SX5", dpName, value, "_CONF");
 watch_for_alive(dpName);
}

//============================================================================================
mudcsUpdateMonitor_LV_1_new(string dpName, string value){
  return;
 mudcsUpdateMonitor_LV_N_new("LV_1", dpName, value, "");
/*
   int pos1=strpos(dpName,"CSC_ME_M");
   int pos2=strpos(dpName,"CSC_ME_P");
 
 string coord;
 string fsm;
 dyn_string d_coord;
 mudcsConvertData2FsmOne("LV_1", dpName, fsm);
 
 dpGet(fsm+".coord",coord);
  if(coord != ""){
   d_coord=strsplit(coord,";");
   if(strpos(d_coord[1],"ELMB")>=0)watch_for_alive("LV_SX5_DimBroker");
   else if(pos1>=0)watch_for_alive("LV_1_DimBroker_MINUS");
   else if(pos2>=0)watch_for_alive("LV_1_DimBroker_PLUS");   
  }
  */
}

//============================================================================================
//============================================================================================
mudcsUpdateMonitor_TYPE_1_new_alive(string dpName, string type, string type1){

//   int pos1=strpos(dpName,"CSC_ME_M");
//   int pos2=strpos(dpName,"CSC_ME_P");
 
 string coord;
 string fsm, sTest,data;
 dyn_string d_coord, dsTest;
 mudcsConvertData2FsmOne(type, dpName, fsm);
 sTest=substr(fsm,0,strpos(fsm,type1));
  dsTest=strsplit(sTest,":");
  sTest=dsTest[dynlen(dsTest)];

if(!is_x2p)watch_for_alive(sTest);
else{ 
  mudcsConvertFsm2DataOne(fsm, data);
  watch_for_alive(sTest,data);
 
}
/*
 dpGet(fsm+".coord",coord);
  if(coord != ""){
   d_coord=strsplit(coord,";");
   if(strpos(d_coord[1],"ELMB")>=0)watch_for_alive("LV_SX5_DimBroker");
   else if(pos1>=0)watch_for_alive("LV_1_DimBroker_MINUS");
   else if(pos2>=0)watch_for_alive("LV_1_DimBroker_PLUS");   
  }
*/

}

//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================



//============================================================================================
//============================================================================================
//============================================================================================

mudcsUpdateMonitor_LV_N_new(string subtype, string dpName, string value, string isconf){
while(CSC_g_BLOCK_WATCHDOG_THREAD){}
int status_source, status_destin;

int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_LV_N_new: ------------------------------------------------FIRST CALL OF HANDLER LV"+" "+dpName);
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
 }

int w_pos,radius;
string label;

int ret,i;
dyn_string test_dyn_string;
string test_string, t_string, test_string2;

string test_string_status; // ***

int test_int;
bool found;
int index;

label="LV_1";

//------- // ***

int pos;

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];
 if((pos=strpos(dpName,":"))>=0){

  test_string_status=CSC_fwG_g_SYSTEM_NAME+":"+"LowVoltage/"+substr(d_test[1],pos+1);

 }

 string coord;
 dyn_string d_coord;
 dpGet(test_string_status+".coord",coord);

//-------

mudcsUpdateMonitor_TYPE_1_new_alive(dpName,"LV_1","_LV");

//=====================================================
if(is_x2p){
    time current_time=getCurrentTime();
    int time_stamp_i;
    dpGet(test_string+".update_value",time_stamp_i);
    time time_stamp_t=time_stamp_i;
    int time_dif=current_time-time_stamp_t;
      
  if(strpos(test_string,"ME_P12_C02")>=0){    
    dyn_string dyn_debug2;
 
    dynAppend(dyn_debug2,"2 "+test_string);
    dynAppend(dyn_debug2,"3 "+time_stamp_t);
     dynAppend(dyn_debug2,">> "+watch_dog_delay);   
     dynAppend(dyn_debug2,time_dif);                
     if(time_dif>watch_dog_delay)dpSetWait("dyn_debug2.",dyn_debug2);
  }

    if(time_dif>watch_dog_delay)return;
}
//=====================================================
DebugTN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_LV_N_new 22222222222222222222222222222222222222222222"+ test_string);


//////mudcsDebug(test_string);


//------------------------------------------------------------------------------------------------
 dpGet(test_string+".data.Alct_o.status",status_source);    //
 dpGet(test_string_status+".chamber_state",status_destin); // 
//------------------------------------------------------------------------------------------------
 int current_status;
 dpGet(test_string_status+".status",current_status);

 int status_save;
 dpGet(test_string_status+".chamber_state",status_save);
 if(status_save==-2)dpSetWait(test_string_status+".chamber_state",-22);// this all is to allow to switch off from error state and differ -2 and -1



// ======== 10/16/2004 ====== it is needed when the pvss runs, control app. runs then quits, the starts up again  

 if(current_status != -2){ // this is not to take data (and especially to change alert.active before setting initial state of the device) 

// -------------  attention: different from other types (without de-facto status) -------------------------
 if(status_source > 0 && status_source != status_destin){
                                           if(current_status != 0){ // case of sombody switch the electronics by standalone or other tool
                                                         mudcsAlertSetActive(test_string,true);
                                                         dpSetWait(test_string_status+".chamber_state",status_source);
                                           }
                                                         adjust_to_control_part(test_string_status, current_status, status_source);

 }
 else  if(status_source == 0 && status_source != status_destin){
                                           if(current_status == 0){ // case of sombody switch the electronics by standalone or other tool
                                                         mudcsAlertSetActive(test_string,false);
                                                         dpSetWait(test_string_status+".chamber_state",status_source);
                                           }
                                                         adjust_to_control_part(test_string_status, current_status, status_source);
 }
 else if(status_source == -1){
  adjust_to_control_part(test_string_status, current_status, status_source);
  return;
 }



// ----------------------------------------------------------------------------------------------------

// mudcsCopy_new(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_DimBroker_o.data.Cfeb_o.v33",test_string+".data.Cfeb_o.v33");
// mudcsCopy_new(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_DimBroker_o.data.Cfeb_o.v50",test_string+".data.Cfeb_o.v50");
// mudcsCopy_new(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_DimBroker_o.data.Cfeb_o.v60",test_string+".data.Cfeb_o.v60");
// mudcsCopy_new(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_DimBroker_o.data.Cfeb_o.c33",test_string+".data.Cfeb_o.c33");
// mudcsCopy_new(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_DimBroker_o.data.Cfeb_o.c50",test_string+".data.Cfeb_o.c50");
// mudcsCopy_new(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_DimBroker_o.data.Cfeb_o.c60",test_string+".data.Cfeb_o.c60");
// mudcsCopy_new(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_DimBroker_o.data.Cfeb_o.c60",test_string+".data.Cfeb_o.status");
// mudcsCopy_new(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_DimBroker_o.data.Alct_o",test_string+".data.Alct_o");

 } //  if(current_status != -2)



//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(test_string_status); // must be before the mudcsFsmErrorSet
/*
  dyn_string dyn_debug;

    dpGet("dyn_debug2.",dyn_debug);
    dynAppend(dyn_debug,dpName);
    dpSetWait("dyn_debug2.",dyn_debug);
*/
mudcsFsmErrorSet(subtype, test_string_status, test_string); // moved up (before CONFIRMATION) 10/05/2004
//-----------------------------------------------------------------------------------------------------

///////// dpSetWait(CSC_fwG_g_SYSTEM_NAME+":"+subtype+"_COM"+isconf+".command","CONFIRMATION");


DebugTN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_LV_N_new BROKER ********************************"+"LV_N"+" "+subtype+ test_string);



}


//============================================================================================
mudcsUpdateMonitor_CHIP_1_new(string dpName, string value){
 mudcsUpdateMonitor_CHIP_N_new("CHIP_1", dpName, value, "LV_1_COM");
}

//============================================================================================

//============================================================================================
//============================================================================================

mudcsUpdateMonitor_CHIP_N_new(string subtype, string dpName, string value, string com_confirm){

while(CSC_g_BLOCK_WATCHDOG_THREAD){}


int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_CHIP_N_new:------------------------------------------------FIRST CALL OF HANDLER CHIP");
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
 }


int w_pos,radius;
string label;

int ret,i;
dyn_string test_dyn_string;
string test_string, t_string, test_string2;

string test_string_status; // ***

int test_int;
bool found;
int index;


label="CHIP_1";

//------- // ***

int pos;

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];
 if((pos=strpos(dpName,":"))>=0){

  test_string_status=CSC_fwG_g_SYSTEM_NAME+":"+"ChipLoading/"+substr(d_test[1],pos+1);

 }

 string coord;
 dyn_string d_coord;
 dpGet(test_string_status+".coord",coord);

//-------

  dpGet(test_string+".dynatemTcpIp", test_string2);
  dpGet(test_string+".setNumber", test_int);

test_string2=test_string2+";"+test_int;
if(strpos(test_string2,"00:00:00:00") <0 
&& strpos(test_string2,"10.0.0.") <0)return; // not to receive a corrupted data !!!!!!!

DebugTN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_CHIP_N_new 22222222222222222222222222222222222222222222"+ test_int+" "+test_string2);


//////mudcsDebug(test_string);


//------------------------------------------------------------------------------------------------
/// dpGet(test_string+".data.Alct_o.status",status_source);    //
/// dpGet(test_string_status+".chamber_state",status_destin); // 
//------------------------------------------------------------------------------------------------
/// int current_status;
/// dpGet(test_string_status+".status",current_status);


 int status_save;
 dpGet(test_string_status+".chamber_state",status_save);
 if(status_save==-2)dpSetWait(test_string_status+".chamber_state",-22);// this all is to allow to switch off from error state and differ -2 and -1

// ======== 10/16/2004 ====== it is needed when the pvss runs, control app. runs then quits, the starts up again  


//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(test_string_status); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet(subtype, test_string_status, test_string); // moved up (before CONFIRMATION) 10/05/2004
//-----------------------------------------------------------------------------------------------------


 DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_CHIP_N_new ********************************CHIP_1"+ test_string);


}

//============================================================================================
//============================================================================================



//============================================================================================
mudcsUpdateMonitor_TEMP_1_new(string dpName, string value){
 mudcsUpdateMonitor_TEMP_N_new("TEMP_1", dpName, value, "LV_1_COM");
}

//============================================================================================

//============================================================================================
//============================================================================================

mudcsUpdateMonitor_TEMP_N_new(string subtype, string dpName, string value, string com_confirm){
while(CSC_g_BLOCK_WATCHDOG_THREAD){}
int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_TEMP_N_new:------------------------------------------------FIRST CALL OF HANDLER TEMP");
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
 }


int w_pos,radius;
string label;

int ret,i;
dyn_string test_dyn_string;
string test_string, t_string, test_string2;

string test_string_status; // ***

int test_int;
bool found;
int index;


label="TEMP_1";

//------- // ***

int pos;

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];
 if((pos=strpos(dpName,":"))>=0){

  test_string_status=CSC_fwG_g_SYSTEM_NAME+":"+"Temperature/"+substr(d_test[1],pos+1);

 }

 string coord;
 dyn_string d_coord;
 dpGet(test_string_status+".coord",coord);

//-------

DebugTN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_TEMP_N_new 22222222222222222222222222222222222222222222"+ test_string);


//=====================================================
if(is_x2p){
    time current_time=getCurrentTime();
    int time_stamp_i;
    dpGet(test_string+".update_value",time_stamp_i);
    time time_stamp_t=time_stamp_i;
    int time_dif=current_time-time_stamp_t;
/*      
  if(strpos(test_string,"ME_P12_C02")>=0){    
    dyn_string dyn_debug2;
 
    dynAppend(dyn_debug2,"2 "+test_string);
    dynAppend(dyn_debug2,"3 "+time_stamp_t);
     dynAppend(dyn_debug2,">> "+watch_dog_delay);   
     dynAppend(dyn_debug2,time_dif);                
     if(time_dif>watch_dog_delay)dpSetWait("dyn_debug2.",dyn_debug2);
  }
*/
    if(time_dif>watch_dog_delay)return;
}
//=====================================================
//////mudcsDebug(test_string);


//------------------------------------------------------------------------------------------------
/// dpGet(test_string+".data.Alct_o.status",status_source);    //
//// dpGet(test_string_status+".chamber_state",status_destin); // 
//------------------------------------------------------------------------------------------------
/// int current_status;
/// dpGet(test_string_status+".status",current_status);



int status_save;
 dpGet(test_string_status+".chamber_state",status_save);
 if(status_save==-2)dpSetWait(test_string_status+".chamber_state",-22);// this all is to allow to switch off from error state and differ -2 and -1

// ======== 10/16/2004 ====== it is needed when the pvss runs, control app. runs then quits, the starts up again  


//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(test_string_status); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet(subtype, test_string_status, test_string); // moved up (before CONFIRMATION) 10/05/2004
//-----------------------------------------------------------------------------------------------------


DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_TEMP_N_new ********************************TEMP_1"+ test_string);

 
}



//============================================================================================
//============================================================================================

//============================================================================================
//============================================================================================

mudcsUpdateMonitor_FED_1_new(string dpName, string value){
while(CSC_g_BLOCK_WATCHDOG_THREAD){}
int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_FED_N_new:------------------------------------------------FIRST CALL OF HANDLER FED");
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
 }


int w_pos,radius;
string label,subtype="FED_1";

int ret,i;
dyn_string test_dyn_string;
string test_string, t_string, test_string2;

string test_string_status; // ***

int test_int;
bool found;
int index;


//------- // ***

int pos;

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];
 if((pos=strpos(dpName,":"))>=0){

  test_string_status=CSC_fwG_g_SYSTEM_NAME+":"+"Fed/"+substr(d_test[1],pos+1);

 }

 string coord;
 dyn_string d_coord;
 dpGet(test_string_status+".coord",coord);

//-------

DebugTN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_FED_1_new 22222222222222222222222222222222222222222222"+ test_string_status);

mudcsUpdateMonitor_TYPE_1_new_alive(dpName,"FED_1","_FED");
//=====================================================
if(is_x2p){
    time current_time=getCurrentTime();
    int time_stamp_i;
    dpGet(test_string+".update_value",time_stamp_i);
    time time_stamp_t=time_stamp_i;
    int time_dif=current_time-time_stamp_t;

    if(time_dif>watch_dog_delay)return;
}
//=====================================================
//////mudcsDebug(test_string);

int status_save;
 dpGet(test_string_status+".chamber_state",status_save);
 if(status_save==-2)dpSetWait(test_string_status+".chamber_state",-22);// this all is to allow to switch off from error state and differ -2 and -1

// ======== 10/16/2004 ====== it is needed when the pvss runs, control app. runs then quits, the starts up again  


//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(test_string_status); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet(subtype, test_string_status, test_string); // moved up (before CONFIRMATION) 10/05/2004
//-----------------------------------------------------------------------------------------------------


DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_FED_1_new ********************************FED_1"+ test_string);

 
}


//============================================================================================
//============================================================================================
mudcsUpdateMonitor_GAS_SX5_new(string dpName, string value){
while(CSC_g_BLOCK_WATCHDOG_THREAD){}
int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_GAS_SX5_new:------------------------------------------------FIRST CALL OF HANDLER GAS_SX5");
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
 }



int w_pos,radius;
string label;

int ret,i;
dyn_string test_dyn_string;
string test_string, t_string, t_string1, t_string2;
int test_int;
bool found;
int index;
int pos;

///// dpSet(CSC_fwG_g_SYSTEM_NAME+":treeBrowser_control_resp.","nmnmnm4");


label="GAS_SX5";

  dpGet(CSC_fwG_g_SYSTEM_NAME+":GAS_SX5_DimBroker_o.dynatemTcpIp", test_string);
  dpGet(CSC_fwG_g_SYSTEM_NAME+":GAS_SX5_DimBroker_o.setNumber", test_int);

test_string=test_string+";"+test_int;
//////mudcsDebug(test_string);

/*
//found=false;
//
//  t_string=CSC_fwG_g_SYSTEM_NAME+":Db_o.CscLevelDevicesCoordinates";
//  dpGet(t_string,test_dyn_string);
//
//
// for(i=1; i<=dynlen(test_dyn_string); i++){
//
//           pos=strpos(test_dyn_string[i]," ");           
//           if(pos<0)continue;
//           t_string1 = substr(test_dyn_string[i],0,pos);
//           t_string2 = substr(test_dyn_string[i],pos+1);
//   if(t_string1 == "GasMonitoring/CSCGAS_SX5" && t_string2 == test_string){
//    found=true;
//    break;
//   }
//
// } // for
*/

DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_GAS_SX5_new ********************************GAS_SX5::"+ t_string1);
//DebugN("mudcsServer: ********************************GAS_SX5::"+ t_string2);
//DebugN("mudcsServer: ********************************GAS_SX5::"+ test_string);

//// if(!found){
 if(strpos(test_string,"LVCB") <0 ){ // not to receive a corrupted data !!!!!!!
/////////////    mudcsDebug("mudcsUpdateMonitor_"+label+": \n dynatemTcpId;setNumber is not in the database");
DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_GAS_SX5_new ********************************GAS_SX5 11::");
dpSet(CSC_fwG_g_SYSTEM_NAME+":GAS_SX5_COM_CONF.command","CONFIRMATION");
    return;
 }
 else{

//DebugN("mudcsServer: ********************************GAS_SX5 2::");
   mudcsPosToNameConvert_new(w_pos, radius, index, label, test_string);  // !!!!!!!!!!!!!!
/*
   dyn_string d_s2;
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,"gas1:::"+test_string);
    dpSetWait("dyn_debug2.",d_s2);
*/
   test_string=mudcsNameByAliasGet(test_string);
/*
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,"gas2:::"+test_string);
    dpSetWait("dyn_debug2.",d_s2);
*/

////////// fsm   sprintf(test_string,"%s%02d%s",CSC_fwG_g_SYSTEM_NAME+":mudcsCsc_o_" + CSC_fwG_g_wheel_labels[w_pos] + CSC_fwG_g_radius_labels[radius], index,label);


////   dpSet(CSC_fwG_g_SYSTEM_NAME+":LV_1_DimBroker_o.command","ffff");
////////   mudcsDebug(test_string);
 }

//DebugN("mudcsServer: ********************************GAS_SX5 3::"+test_string);

 if(!dpExists(test_string)){
  dpSet(CSC_fwG_g_SYSTEM_NAME+":GAS_SX5_COM_CONF.command","CONFIRMATION");
  return;
 }

 mudcsCopy_new(CSC_fwG_g_SYSTEM_NAME+":GAS_SX5_DimBroker_o.data.Mixer1_o",test_string+".data.Mixer1_o");
 mudcsCopy_new(CSC_fwG_g_SYSTEM_NAME+":GAS_SX5_DimBroker_o.data.Mixer2_o",test_string+".data.Mixer2_o");

//------------------------------------------

float ar_f,co2_f,cf4_f;

dpGet(CSC_fwG_g_SYSTEM_NAME+":GAS_SX5_DimBroker_o.data.Mixer2_o.ar",ar_f);
dpGet(CSC_fwG_g_SYSTEM_NAME+":GAS_SX5_DimBroker_o.data.Mixer2_o.co2",co2_f);
dpGet(CSC_fwG_g_SYSTEM_NAME+":GAS_SX5_DimBroker_o.data.Mixer2_o.cf4",cf4_f);

DebugTN("SHOULD NOT SEE THIS (shouldn't be called) GAS:b "+ret);

ret=percentage_calculation(ar_f,co2_f,cf4_f);

DebugTN("SHOULD NOT SEE THIS (shouldn't be called) GAS:a "+ret);
 if(ret){
///   DebugTN("GAS ALARM");
  if(ar_f > 0.7 || co2_f > 0.7 || cf4_f > 0.5){
   DebugTN("SHOULD NOT SEE THIS (shouldn't be called) GAS ALARM: RED LEVEL");
   DebugTN("SHOULD NOT SEE THIS (shouldn't be called) HV should be switched?");
    if(CSC_fwG_g_GAS_ERROR_FLAG)system("mail "+CSC_fwG_g_EMAILS_GAS+" < "+CSC_fwG_g_project_name_home+"/source/"+CSC_fwG_g_Component+"/text/HV_mail_gas_error.txt &");
    CSC_fwG_g_GAS_ERROR_FLAG=0;
    CSC_fwG_g_GAS_WARNING_FLAG=1;
//////// hv_master_off();
  }
  else{
    DebugTN("SHOULD NOT SEE THIS (shouldn't be called) GAS ALARM: YELLOW LEVEL");
    if(CSC_fwG_g_GAS_WARNING_FLAG)system("mail "+CSC_fwG_g_EMAILS_GAS+" < "+CSC_fwG_g_project_name_home+"/source/"+CSC_fwG_g_Component+"/text/HV_mail_gas_warning.txt &");
    CSC_fwG_g_GAS_WARNING_FLAG=0;
    CSC_fwG_g_GAS_ERROR_FLAG=1;
  }
////////  send mail  
 } // if ret
 else{
  CSC_fwG_g_GAS_ERROR_FLAG=1;
  CSC_fwG_g_GAS_WARNING_FLAG=1;
 }

//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(test_string);
/*
   dyn_string d_s2;
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,"gas1:::"+test_string);
    dpSetWait("dyn_debug2.",d_s2);
*/
mudcsFsmErrorSet("GAS_SX5", test_string, test_string); // moved up (before CONFIRMATION) 10/05/2004
//-----------------------------------------------------------------------------------------------------

 dpSet(CSC_fwG_g_SYSTEM_NAME+":GAS_SX5_COM_CONF.command","CONFIRMATION");
 



watch_for_alive("GAS_SX5_DimBroker");
DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_GAS_SX5_new ********************************GAS_SX5"+ test_string);



}


//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================

mudcsUpdateMonitor_WTH_SX5_new(string dpName, string value){
while(CSC_g_BLOCK_WATCHDOG_THREAD){}
int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_WTH_SX5_new:------------------------------------------------FIRST CALL OF HANDLER WTH_SX5");
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
 }


int w_pos,radius;
string label;

int ret,i;
dyn_string test_dyn_string;
string test_string, t_string, test_string2, t_string1, t_string2;

string test_string_status; // ***

int test_int;
bool found;
int index;
int pos;



label="WTH_SX5";

//------- // ***

int pos;

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];
 if((pos=strpos(dpName,":"))>=0){

  test_string_status=CSC_fwG_g_SYSTEM_NAME+":"+"WeatherStation/"+substr(d_test[1],pos+1);

 }

 string coord;
 dyn_string d_coord;
 dpGet(test_string_status+".coord",coord);

//-------

  dpGet(test_string+".dynatemTcpIp", test_string2);
  dpGet(test_string+".setNumber", test_int);



test_string2=test_string2+";"+test_int;
DebugTN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_WTH_SX5_new 22222222222222222222222222222222222222222222"+ test_int+" "+test_string2);

//---
/*
//found=false;
//
//  t_string=CSC_fwG_g_SYSTEM_NAME+":Db_o.CscLevelDevicesCoordinates";
//  dpGet(t_string,test_dyn_string);
//
//
// for(i=1; i<=dynlen(test_dyn_string); i++){
//
//           pos=strpos(test_dyn_string[i]," ");           
//           if(pos<0)continue;
//           t_string1 = substr(test_dyn_string[i],0,pos);
//           t_string2 = substr(test_dyn_string[i],pos+1);
//   if(t_string1 == "WeatherStation/CSCWTH_SX5" && t_string2 == test_string2){ // important: not to generate error on corrupted data 
//    found=true;
//    break;
//   }
//
// } // for
//if(!found)return;
*/
//----


  if(strpos(test_string2,"WEATHER") <0 )return; // not to receive a corrupted data !!!!!!!



//////mudcsDebug(test_string);


//------------------------------------------------------------------------------------------------
/// dpGet(test_string+".data.Alct_o.status",status_source);    //
//// dpGet(test_string_status+".chamber_state",status_destin); // 
//------------------------------------------------------------------------------------------------
 int current_status;
 dpGet(test_string_status+".status",current_status);

 int status_save;
 dpGet(test_string_status+".module_state",status_save);
 if(status_save==-2)dpSetWait(test_string_status+".module_state",-22);// this all is to allow to switch off from error state and differ -2 and -1

//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(test_string_status); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet("WTH_SX5", test_string_status, test_string); // moved up (before CONFIRMATION) 10/05/2004

int current_time=getCurrentTime();
int previous_status=current_status;
dpGet(test_string_status+".status",current_status); // to check if status set to -1
if( current_status == -1 && (previous_status >=0 || (current_time-CSC_fwG_g_last_time_wth_alarm) > 86400) ){
  CSC_fwG_g_last_time_wth_alarm=current_time;
  system("mail "+CSC_fwG_g_EMAILS_WTH+" < "+CSC_fwG_g_project_name_home+"/source/"+CSC_fwG_g_Component+"/text/WTH_mail_alarm.txt &");
}
else if(current_status == 0){
   system("mail "+CSC_fwG_g_EMAILS_GENERAL+" < "+CSC_fwG_g_project_name_home+"/source/"+CSC_fwG_g_Component+"/text/ON_mail_warning.txt &");

}
//-----------------------------------------------------------------------------------------------------


watch_for_alive("WTH_SX5_DimBroker");
DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_WTH_SX5_new ********************************WTH_SX5"+ test_string);

 
}


//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================

mudcsUpdateMonitor_PT100_new(string dpName, string value){
while(CSC_g_BLOCK_WATCHDOG_THREAD){}
int index=dynContains(CSC_fwG_g_BROKER_HANDLERS_FIRST, substr(dpName,0,strpos(dpName,".")));
 if(index >=1 ){ // first call of handler
  DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsUpdateMonitor_PT100_new:------------------------------------------------FIRST CALL OF HANDLER PT100");
  dynRemove(CSC_fwG_g_BROKER_HANDLERS_FIRST,index);
  return;
 }


int w_pos,radius;
string label;

int ret,i;
dyn_string test_dyn_string;
string test_string, t_string, test_string2, t_string1, t_string2;

string test_string_status; // ***

int test_int;
bool found;
int index;
int pos;



label="PT100";

//------- // ***

int pos;

 dyn_string d_test=strsplit(dpName,".");
 test_string=d_test[1];
 if((pos=strpos(dpName,":"))>=0){

  test_string_status=CSC_fwG_g_SYSTEM_NAME+":"+"GasMonitoring/"+substr(d_test[1],pos+1);

 }

 string coord;
 dyn_string d_coord;
 dpGet(test_string_status+".coord",coord);

//-------

  dpGet(test_string+".dynatemTcpIp", test_string2);
  dpGet(test_string+".setNumber", test_int);



test_string2=test_string2+";"+test_int;
DebugTN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_PT100_new 22222222222222222222222222222222222222222222"+ test_int+" "+test_string2);

//---
/*
//found=false;
//
//  t_string=CSC_fwG_g_SYSTEM_NAME+":Db_o.CscLevelDevicesCoordinates";
//  dpGet(t_string,test_dyn_string);
//
// for(i=1; i<=dynlen(test_dyn_string); i++){
//
//           pos=strpos(test_dyn_string[i]," ");           
//           if(pos<0)continue;
//           t_string1 = substr(test_dyn_string[i],0,pos);
//           t_string2 = substr(test_dyn_string[i],pos+1);
//   if(t_string1 == "GasMonitoring/CSCPT100" && t_string2 == test_string2){ // important: not to generate error on corrupted data 
//    found=true;
//    break;
//   }
//
// } // for

// if(!found)return;
*/
//----




  if(strpos(test_string2,"PT100") <0 )return; // not to receive a corrupted data !!!!!!!

//////mudcsDebug(test_string);


//------------------------------------------------------------------------------------------------
/// dpGet(test_string+".data.Alct_o.status",status_source);    //
//// dpGet(test_string_status+".chamber_state",status_destin); // 
//------------------------------------------------------------------------------------------------
 int current_status;
 dpGet(test_string_status+".status",current_status);

 int status_save;
 dpGet(test_string_status+".module_state",status_save);
 if(status_save==-2)dpSetWait(test_string_status+".module_state",-22);// this all is to allow to switch off from error state and differ -2 and -1

//------------------------------------------------------------------------------------------------
checkOfLinkWithControlProgram(test_string_status); // must be before the mudcsFsmErrorSet
mudcsFsmErrorSet("PT100", test_string_status, test_string); // moved up (before CONFIRMATION) 10/05/2004


int current_time=getCurrentTime();
int previous_status=current_status;
dpGet(test_string_status+".status",current_status); // to check if status set to -1
if( current_status == -1 && (previous_status >=0 || (current_time-CSC_fwG_g_last_time_pt100_alarm) > 86400) ){
  CSC_fwG_g_last_time_pt100_alarm=current_time;
  system("mail "+CSC_fwG_g_EMAILS_GAS+" < "+CSC_fwG_g_project_name_home+"/source/"+CSC_fwG_g_Component+"/text/PT100_mail_alarm.txt &");
}
else if(current_status == 0){
   system("mail "+CSC_fwG_g_EMAILS_GENERAL+" < "+CSC_fwG_g_project_name_home+"/source/"+CSC_fwG_g_Component+"/text/ON_mail_warning.txt &");

}
//-----------------------------------------------------------------------------------------------------


watch_for_alive("PT100_DimBroker");
DebugN("SHOULD NOT SEE THIS (shouldn't be called) mudcsServer::mudcsUpdateMonitor_PT100_new ********************************PT100"+ test_string);

 
}



//============================================================================================
//============================================================================================


mudcsCopy_new(string dpNameSource, string dpNameDestin){

dyn_int int_ar;
dyn_float  float_ar;
dyn_string  string_ar; 
dyn_bool  bool_ar;
//dyn_dpid  dpid_ar;
//dyn_time  time_ar;

int int_el;
float float_el;
string string_el;
bool bool_el;
//dpid dpid_el;
//time time_el;
 
dyn_string elemListS, elemListD;
int i, typ;


if(dpElementType(dpNameSource)==DPEL_STRUCT){

  elemListS=dpNames(dpNameSource+".*");
  elemListD=dpNames(dpNameDestin+".*");

}
else {
elemListS=makeDynString(dpNameSource);
elemListD=makeDynString(dpNameDestin);
}


/////////////////mudcsDebug("Copy-----"+dpNameSource);

  for(i=1;i<=dynlen(elemListS);i++){
     typ=dpElementType(elemListS[i]);
     switch(typ){
      case DPEL_BOOL:
       dpGet(elemListS[i],bool_el);
       dpSetWait(elemListD[i],bool_el);
      break;
      case DPEL_INT:
       dpGet(elemListS[i],int_el);
       dpSetWait(elemListD[i],int_el);
      break;
      case DPEL_FLOAT:
       dpGet(elemListS[i],float_el);
    //temporal !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
////mudcsDebug(elemListS[i]);
////mudcsDebug(elemListD[i]);
////       float_el=10;
       dpSetWait(elemListD[i],float_el);
      break;
      case DPEL_STRING:
       dpGet(elemListS[i],string_el);
       dpSetWait(elemListD[i],string_el);
      break;

      case DPEL_DYN_BOOL:
       dpGet(elemListS[i],bool_ar);
       dpSetWait(elemListD[i],bool_ar);
      break;
      case DPEL_DYN_INT:
       dpGet(elemListS[i],int_ar);
       dpSetWait(elemListD[i],int_ar);
      break;
      case DPEL_DYN_FLOAT:
       dpGet(elemListS[i],float_ar);
    //temporal !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///       float_ar[1]=10;
///       float_ar[2]=11;
       dpSetWait(elemListD[i],float_ar);
      break;
      case DPEL_DYN_STRING:
       dpGet(elemListS[i],string_ar);
       dpSetWait(elemListD[i],string_ar);
      break;



     }// switch

  } //for


}

//===================================================================
/* fsm

mudcsPosToNameConvert_new(int w_pos, int radius, int ich, string device_type, string &name){

 int pos;
 int len;
 string test;
 string w,r;
 int i;

 sprintf(name,"%s%02d%s",CSC_fwG_g_SYSTEM_NAME+":mudcsCsc_o_" + CSC_fwG_g_wheel_labels[w_pos] + CSC_fwG_g_radius_labels[radius], ich, device_type);

}

*/
//============================================================================================
mudcsPosToNameConvert_new(int w_pos, int radius, int ich, string device_type, string &name){

 int pos;
 int len;
 string test;
 string w,r;
 int i;
 string prefix;

// !!!!!!!!  this function should be called only for devices located on chambers

 if(strpos(device_type,"LV")>=0)prefix="LowVoltage/";
 else if(strpos(device_type,"HV")>=0)prefix="HighVoltage/";
 else if(strpos(device_type,"TEMP")>=0)prefix="Temperature/";
 else if(strpos(device_type,"CHIP")>=0)prefix="ChipLoading/";


// !!!!!!!!  this function should be called only for devices located on chambers
 else if(strpos(device_type,"GAS")>=0){
  prefix="GasMonitoring/";
   if(strpos(device_type,"GAS_SX5")>=0)
   sprintf(name,"%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + device_type);

  return;
 }
 else if(strpos(device_type,"WTH")>=0){
  prefix="WeatherStation/";
   if(strpos(device_type,"WTH_SX5")>=0)
   sprintf(name,"%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + device_type);

  return;
 }
 else if(strpos(device_type,"PT100")>=0){
  prefix="GasMonitoring/";
   if(strpos(device_type,"PT100")>=0)
   sprintf(name,"%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + device_type);

  return;
 }
 else if(strpos(device_type,"CRB")>=0){ // should be corrected by calculating pcrate position by chamber number!!!!!!!
  prefix="CRB/";
   if(strpos(device_type,"CRB_1")>=0)
   sprintf(name,"%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + CSC_fwG_g_wheel_labels[w_pos]+ device_type);

  return;
 }
 else if(strpos(device_type,"MRTN")>=0){ // should be corrected by calculating pcrate position by chamber number!!!!!!!
  prefix="MRTN/";
   if(strpos(device_type,"MRTN_1")>=0)
   sprintf(name,"%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + device_type);

  return;
 }
 else if(strpos(device_type,"WNR12")>=0){ // should be corrected by calculating pcrate position by chamber number!!!!!!!
  prefix="WNR12/";
   if(strpos(device_type,"WNR12_1")>=0)
   sprintf(name,"%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + device_type);

  return;
 }
 else if(strpos(device_type,"ALNM")>=0){ // should be corrected by calculating pcrate position by chamber number!!!!!!!
  prefix="ALNM/";
   if(strpos(device_type,"ALNM_1")>=0)
   sprintf(name,"%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + CSC_fwG_g_wheel_labels[w_pos]+device_type);

  return;
 }

// sprintf(name,"%sc%02d%s",CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix + CSC_fwG_g_wheel_labels[w_pos] + CSC_fwG_g_radius_labels[radius], ich, device_type);
 sprintf(name,"%sc%02d%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + CSC_fwG_g_wheel_labels[w_pos] + CSC_fwG_g_radius_labels[radius], ich, device_type);

}

//==========================================================================================

mudcsFsmErrorSet(string subtype, string test_string, string test_string_d, bool isSetOn=true){
  
  int current_status;
   
  dyn_string dps;  
  dpGet(test_string_d+".:_alert_hdl.._dp_list",dps);
  if(dynlen(dps) > 0)
  {
  // mudcsDebug(DpName+".:_alert_hdl.._active" + "is not configured");  
 // 
    //if(dpExists(test_string_d+".:_alert_hdl.._active")) // "." is important here !!!! 
// 

 string error_class_source;
 string error_class_destin;
 bool state_destin;

 dpGet(test_string+".:_alert_hdl.._act_state_color",error_class_source); //  01/23/2005
 if(test_string_d != "")dpGet(test_string_d+".:_alert_hdl.._act_state_color",error_class_destin);
 else error_class_destin=error_class_source;

 dpGet(test_string_d+".:_alert_hdl.._active",state_destin);

// 10/04/2004
 dpGet(test_string+".status",current_status);

 

// === the following because: the status can be set from -1 to 0 (e.g.) but the _alert_hdl.._active is not still set to 0
// ===  when data comes from the server ============= 02/10/2005
 if(current_status == 0 && state_destin != 0)return;
 if(current_status == 2 && state_destin != 1)return;
// ======

 emu_info("mudcsFsmErrorSet:?????????????????????????????????????"+error_class_source+" "+error_class_destin+" "+state_destin);
/*
   dyn_string d_s2;
  if(strpos(test_string,"GAS_MIXERS")>=0){
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,"gas3:::"+state_destin+" gas4 "+current_status);
    dpSetWait("dyn_debug2.",d_s2);
  }
*/
  if((error_class_source == "FwAlarmErrorAck" || error_class_destin == "FwAlarmErrorAck") &&
   state_destin == true && current_status != -1 && current_status != -2){ // 10/05/2004
  // dpSet( CSC_fwG_g_SYSTEM_NAME+":LV_N_DimBroker_o."+":_alert_hdl.._ack", true); // no way to acklowledge the summary(group) alert
   dpSetWait(CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value",test_string);
   dpSetWait(test_string+".status",-1);
   ///// dpSet(test_string+".:_alert_hdl.._act_state_color","FwAlarmErrorUnack"); // nypassing because the test_string is created with group  _alert_hdl
/*
   DebugTN("*");
   DebugTN("*");
   DebugTN("*");
   DebugTN("*");
   DebugTN("*");
   DebugTN("*");
   DebugTN("*");
   DebugTN("*");
   DebugTN("*");
   DebugTN("*");
   DebugTN("*");
   DebugTN("*");
   DebugTN("*");
   DebugTN("*");
   DebugTN("*");
*/
   emu_info("mudcsServer::mudcsFsmErrorSet ?????????????????????????????????????????????????????????????? "+subtype+ test_string);
  }
  else  if(current_status==-1 && error_class_source != "FwAlarmErrorAck" && error_class_destin != "FwAlarmErrorAck" &&  
                                                           state_destin == true){ // 10/04/2004 
    // it is to recover from error state if alarm goes away
 
   dpSetWait(CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value",test_string);
   dpSet(test_string+".status",2); // 2 because  state_destin == true 
  }
  else  if(current_status==-2 &&  state_destin == true){ 
// 10/10/2004 case if device was connected (control app. starts up): mudcsUpdateMonitor_LV_1_status is called this way: init of device state

////// mudcsPayAttention();

   dpSetWait(CSC_fwG_g_SYSTEM_NAME+":NEW_DP_CONNECTED.value",test_string);
   dpSetWait(CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value",test_string);
   dpSet(test_string+".status",2); // 2 because  state_destin == true 

   setDpointsNotControlled("");
  }
  else  if(current_status==-2 &&  state_destin == false){ 
// 10/10/2004 case if device was connected (control app starts up): mudcsUpdateMonitor_LV_1_status is called this way: init of device state
 
////// mudcsPayAttention();

   dpSetWait(CSC_fwG_g_SYSTEM_NAME+":NEW_DP_CONNECTED.value",test_string);
   dpSetWait(CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value",test_string);
   dpSet(test_string+".status",0); // 0 because  state_destin == false 

   setDpointsNotControlled("");

  }

 } // if(dpExists(test_string_d+".:_alert_hdl.._active"))
 else{ // case of e.g. Snmp Channel (fwWnrCh) (as it is not alert configured)
     dpGet(test_string+".status",current_status);
     if(current_status==-2 && isSetOn){ 

   dpSetWait(CSC_fwG_g_SYSTEM_NAME+":NEW_DP_CONNECTED.value",test_string);
   dpSetWait(CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value",test_string);
   dpSet(test_string+".status",2); // 2 because  state_destin == true 

   setDpointsNotControlled("");
     }    // 
    else  if(current_status==-2 &&  !isSetOn){ 
      
   dpSetWait(CSC_fwG_g_SYSTEM_NAME+":NEW_DP_CONNECTED.value",test_string);
   dpSetWait(CSC_fwG_g_SYSTEM_NAME+":MUDCS_DOMAIN.value",test_string);
   dpSet(test_string+".status",0); // 0 because  state_destin == false 

   setDpointsNotControlled("");

    } // else if    
 } // else
  
}

//=====================================================================================================================

mudcsUpdateStatusForDevicesWithoutNonDeFactoStatus(string dpName, int value, int state_save){

  string data;
  mudcsConvertFsm2DataOne(dpName, data);
       
 if(value == 2){
   if(strpos(dpName,"CoolingCMS")>=0 || strpos(dpName,"GasCMS")>=0 ) {
     dpSetWait(mudcsAddSystem(data +"."+ ":_alert_hdl.._active"), true);
   }
   else{
    mudcsAlertSetActive(dpName,true); 
   }
 }
 else if(value == 0){
  if(state_save ==-2)dpSetWait(substr(dpName,0,strpos(dpName,"."))+".status",-2);// this all is to allow to switch off from error state and differ -2 and -1
  else {
    if(strpos(dpName,"CoolingCMS")>=0 || strpos(dpName,"GasCMS")>=0 ) {
      dpSetWait(mudcsAddSystem(data +"."+ ":_alert_hdl.._active"), false);
    } 
    else{   
      mudcsAlertSetActive(dpName,false);
    }
  }
 } // else if(value == 0)

}
//=====================================================================================================================


checkOfLinkWithControlProgram(string &dpName){
/*
   dyn_string d_s2;
  if(strpos(dpName,"ALNM")>=0 || strpos(dpName,"PT100")>=0){
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,"gas5:"+dpName);
    dpSetWait("dyn_debug2.",d_s2);
  }
*/

 int index1=dynContains(CSC_fwG_g_FULL_DPLIST, dpName);
 
 if(index1 >=1 ){
  dynRemove(CSC_fwG_g_FULL_DPLIST,index1);

/*
  if(strpos(dpName,"ALNM")>=0 || strpos(dpName,"PT100")>=0){
    dpGet("dyn_debug2.",d_s2);
    dynAppend(d_s2,"gas6:"+index1);
    dpSetWait("dyn_debug2.",d_s2);
  }
*/

 }
//----- 
 else{
 
  string dps_not_controlled;
  dpGet(CSC_fwG_g_SYSTEM_NAME+":DPOINTS_NOT_CONTROLLED.value",dps_not_controlled);
  if(strpos(dps_not_controlled,dpName)>=0)setDpointsNotControlled("");
  
 }
 //----- 
}

//=======================================================================================

setDpointsNotControlled(string display){

/////mudcsPayAttention2();

 if(display=="DPOINTS_NOT_CONTROLLED_CAN_BE_SET" || CSC_fwG_g_DISPLAY_SAVED == "DPOINTS_NOT_CONTROLLED_CAN_BE_SET"){
 if(display=="DPOINTS_NOT_CONTROLLED_CAN_BE_SET")CSC_fwG_g_DISPLAY_SAVED=display;


  int i;
  int status_get;
  dyn_string FULL_DPLIST;
  
  string dpoints_not_controlled;
  FULL_DPLIST=CSC_fwG_g_FULL_DPLIST;
  
  for(i=1;i <=dynlen(FULL_DPLIST);i++ ){
 
  dpoints_not_controlled += FULL_DPLIST[i]+"\n";
  dpGet(FULL_DPLIST[i]+".status",status_get);
  if(status_get !=-2 )dpSet(FULL_DPLIST[i]+".status",-2);

    if(dpExists(FULL_DPLIST[i]+".module_state"))dpSetWait(FULL_DPLIST[i]+".module_state",-2);
    else dpSetWait(FULL_DPLIST[i]+".chamber_state",-2); 

////  mudcsDebug(FULL_DPLIST[i]);  
////////////DebugN(")))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))"+FULL_DPLIST[i]);  

  } // for

///mudcsPayAttention();

  dpSetWait(CSC_fwG_g_SYSTEM_NAME+":DPOINTS_NOT_CONTROLLED.value",dpoints_not_controlled);

 } // if(display=="DPOINTS_NOT_CONTROLLED_CAN_BE_SET")

}

//=======================================================================================

watch_for_alive(string dpName, string data=""){

  dyn_int watch_array;
 
  dyn_time watch_times;
  int i;
  string watch_dp;
/////  dpGet(CSC_fwG_g_SYSTEM_NAME+":WATCH_FOR_TIMEOUTS.",watch_array);
////     watch_times[dynlen(CSC_fwG_g_watch_mask)+1]=0;
  for(i=1;i<=dynlen(CSC_fwG_g_watch_mask);i++){
 
   watch_dp=CSC_fwG_g_watch_mask[i];
   if(strpos(dpName,watch_dp)>=0){
    if(data=="")watch_times[i]=getCurrentTime();
    else{
      time current_time=getCurrentTime();
      int time_stamp_i;
      dpGet(data+".update_value",time_stamp_i);
      if(current_time - time_stamp_i < watch_dog_delay)watch_times[i]=getCurrentTime();
      else watch_times[i]=time_stamp_i;
/*      
  if(strpos(data,"ME_P12_C04")>=0){    
    dyn_string dyn_debug2;
    dynAppend(dyn_debug2,"2 "+data);
    dynAppend(dyn_debug2,"3 "+time_stamp_i);
     dynAppend(dyn_debug2,">> "+i);   
     dynAppend(dyn_debug2,watch_times[i]);                
     dpSetWait("dyn_debug2.",dyn_debug2);
  }
*/        
    }
    
     //dpGet(+".status:_online.._stime",last_set_status);
   //// if(watch_array[i] != -1){watch_array[i]++; if(watch_array[i]>65000)watch_array[i]=1;}
   //// else watch_array[i]=1;
    dpSet(CSC_fwG_g_SYSTEM_NAME+":WATCH_TIME"+i+".",watch_times[i]);
    break;
   }
  } // for
  ///// dpSet(CSC_fwG_g_SYSTEM_NAME+":WATCH_FOR_TIMEOUTS.",watch_array);  
///DebugTN("WATCH_WATCH_WATCH_");

 
/*
dyn_string d_s;
dpGet("dyn_debug1.",d_s);
dynAppend(d_s,">>>"+dpName);
dpSet("dyn_debug1.",d_s);
*/

}

//=======================================================================================

adjust_to_control_part(string fsm, int current_status, int status_source){
// attention: this fuction may not do adjusting if
// 1) the control (setting of fsm.status goes from both sides at the same time: control and pvss )
// 2) a control from control side (e.g. power on/off) happens more often than "interval" (in seconds) 
/*
 if(strpos(fsm,"21_C02_HV")>=0 && status_source == 5){
   dyn_string dyn_debug4;
    dpGet("dyn_debug4.",dyn_debug4); 
    dynAppend(dyn_debug4,fsm+" "+status_source);
    dpSetWait("dyn_debug4.",dyn_debug4);
  
 } 
*/
int interval=30;
string data;

//---------------------------------------------------------------
if(status_source == -1){ // the case is control server is started
    if(current_status != 0)dpSetWait(fsm+".status",2);
    else if(current_status == 0)dpSetWait(fsm+".status",0);
}
else if(status_source == 5 && strpos(fsm,"HV")>=0){ // the case is control server is started: for HV as two ccsc is decoded inone module
  if(no_auto_restor){

   if(current_status!=-2){
//    if(strpos(fsm,"primary")>=0)dpSetWait(fsm+".chamber_state",status_source);
//    else dpSetWait(fsm+".module_state",status_source);
    dpSetWait(fsm+".status",0);
   }
   else{
    mudcsConvertFsm2DataOne(fsm,data);
    dpSetWait(data+".:_alert_hdl.._active",0);         
   }
  }
  else{
   if(current_status != 0)dpSetWait(fsm+".status",2);
   else if(current_status == 0)dpSetWait(fsm+".status",0);
  }
}
else if(current_status != status_source && CSC_fwG_g_IS_ADJUST_TO_CONTROL_SIDE){ // case of control on the control side

 time last_set_status;
 dpGet(fsm+".status:_online.._stime",last_set_status);

  if((getCurrentTime()-last_set_status) > interval){
    if(status_source != 0)dpSetWait(fsm+".status",2);
    else if(status_source == 0)dpSetWait(fsm+".status",0);
  } 
}
//---------------------------------------------------------------

}

//=======================================================================================

waiting_for_dim_configured(){
// delay(60*4); // to let pvss-dim servers to start up (the dim servers are launched by the operation tab of Editor-Navigator)

int i,j;
dyn_string dims;
dyn_string list;
dyn_string split_list;
dyn_string dps;
time last_set_time;
time start_time;
int count;

 dpGet("START_TIME.",start_time);
/*
//-------------------------------------------
 dims= dpNames("_DimClient*","_FwDimConfig");
 dynClear(dps);
// DebugTN(dps);
 for(i=1;i<=dynlen(dims);i++){
//  DebugTN(dims[i]+".clientServices"+">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
   dpGet(dims[i]+".clientServices",list);
if(dynlen(list) <= 0)continue;
//  DebugTN(list+">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>----");
split_list=strsplit(list[dynlen(list)],",");
   dynAppend(dps,split_list[1]);
// DebugTN(split_list[1]+"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
 } // for
//------------------------------------------- 
 */
//for(j=1;j<=dynlen(dps);j++){
// DebugTN(dps[j]+"======++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
//}
 
for(j=1;j<=10;j++){ // 30*10=5 minutes
delay(30); 
//-------------------------------------------
 dims= dpNames("_DimClient*","_FwDimConfig");
 dynClear(dps);
// DebugTN(dps);
 for(i=1;i<=dynlen(dims);i++){
//  DebugTN(dims[i]+".clientServices"+">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
   dpGet(dims[i]+".clientServices",list);
if(dynlen(list) <= 0)continue;
//  DebugTN(list+">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>----");
split_list=strsplit(list[dynlen(list)],",");
   dynAppend(dps,split_list[1]);
// DebugTN(split_list[1]+"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
 } // for
//------------------------------------------- 
count=0;
  for(i=1;i<=dynlen(dps);i++){
int skip=0;    
  emu_debug("waiting_for_dim_configured: " + dynlen(dps)+"----->>>>"+dps[i]);
   dpGet(dps[i]+".update_value:_online.._stime",last_set_time); // pvss6
   if(strpos(dps[i],"dyn_debug")>=0)skip=1;
   if((last_set_time < start_time) && !skip){
string t1=start_time;
string t2=last_set_time;
 emu_debug("waiting_for_dim_configured: " + t1+"==="+t2);
     break;
   }
   count++;
  }
 if(count == dynlen(dps)){delay(10);break;}
  
}

 for(i=1;i<=10;i++){
  emu_info("==================== DIM CONFIG ended ===================================");
 }

}
