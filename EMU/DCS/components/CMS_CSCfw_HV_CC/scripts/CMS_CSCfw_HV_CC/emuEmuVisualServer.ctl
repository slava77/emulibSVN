#uses "CMS_CSCfw_HV_CC/emuDcs.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs2.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs3.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs4.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcsDim.ctl"
#uses "CMS_CSCfw_HV_CC/emuAlert.ctl"
#uses "CMS_CSCfw_HV_CC/emufw.ctl"


#uses "CMS_CSCfw_HV_CC/mudcs0CreateAllTypes.ctl"
#uses "CMS_CSCfw_HV_CC/mudcs1DbCreate.ctl"
#uses "CMS_CSCfw_HV_CC/mudcs2FsmCreate.ctl"
#uses "CMS_CSCfw_HV_CC/mudcs3AlertCreate.ctl"
#uses "CMS_CSCfw_HV_CC/mudcs4SetFsmPanels.ctl"
#uses "CMS_CSCfw_HV_CC/mudcs5Archive.ctl"

#uses "CMS_CSCfw_GAS/emuGas.ctl"
#uses "CMS_CSCfw_GAS/mudcs5GasSetPanels.ctl"

 #uses "CMS_CSCfw_LV_CRB/mudcs9XAlertReconfigAllSlowControls.ctl"
#uses "aes.ctl"

// former initialize handler of fwFsmOperation.onl

int FIRST_CONTROLLED=0; // my_corr++
int FIRST_NEW_DP_CONNECTED=0; // my_corr++
int FIRST_MYWARNING=0; // my_corr++
int FIRST_FSM_PANELS=0;

string os;

main() // former initialize handler of fwFsmOperation.onl
{
//-------------------------------// my_corr++

dpSetWait("EMU_MESSAGES.",makeDynString());

mudcsCheckDistConnections();

dyn_string exceptionInfo;





mudcsInit(); 
os=getenv("OS");
int i;

//==================================== DIM ==================================================
dyn_string dns_list;
string dns_command;
time start_time=getCurrentTime();

//system(CSC_fwG_g_project_name_home+"/bin/dns &");
if (os =="Linux")system("dns &"); // to call from fwComponents/bin
////else system("start /b dns");


if (os =="Linux"){
dpGet(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimServerComputerList",dns_list);
 for(i=1;i<=dynlen(dns_list);i++){
  if(strpos(dns_list[i],"smi") < 0 && strpos(dns_list[i],"uf") < 0){
   dns_command=  "rsh "+dns_list[i]+" -l fast '. .bash_profile; echo $DIM_DNS_NODE; dns &'";
   dns_command= dns_command+" &"; 
   system(dns_command);
  }
  
 }
}

dpSetWait(mudcsAddSystem("START_TIME."),start_time);
//=============================================================================================


//return;//////


dyn_string ctrl_mans;
dpGet(CSC_fwG_g_SYSTEM_NAME+":_Connections.Ctrl.ManNums",ctrl_mans);
 if(dynContains(ctrl_mans,12)){ // 12 is mudcsServer number
   
 mudcsEmuMessages("vs:RESTART: you can operate"); // case when X-server of remote machine falls
}
else{
dpSet(CSC_fwG_g_SYSTEM_NAME+":RUNCONTROL_FLAG.",0); // not allow RUN CONTROL OVER STARTING
}

//dpSet(CSC_fwG_g_SYSTEM_NAME+":RUNCONTROL_FLAG.",0); // not allow RUN CONTROL OVER STARTING


mudcsDomainsStoring();

mudcsAllDimManagersStop();
mudcsDimConfigAllManagers(exceptionInfo);
if(dynlen(exceptionInfo)>0){mudcsEmuMessages("vs:"+exceptionInfo[1]);return;}

delay(5);
if(!CSC_fwG_g_P5_SIM_AT_904)mudcsAllDimManagersStart();


mudcsFsmPanelsCheck();
///// remporat comment: should be set the top node for each station project: dpConnect("update_csc_current_state",CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_part+"|"+CSC_fwG_g_csc_part+".fsm.currentState");
dpConnect("update_dpoints_not_controlled",CSC_fwG_g_SYSTEM_NAME+":DPOINTS_NOT_CONTROLLED.value");
///////dpConnect("update_new_dp_connected",CSC_fwG_g_SYSTEM_NAME+":NEW_DP_CONNECTED.value");
dpConnect("update_mywarning",CSC_fwG_g_SYSTEM_NAME+":MYWARNING.value");


delay(2);
if (os =="Linux")system("PVSS00ctrlWEB -num 12 rs_http.ctl &");
DebugTN("----------------- START SERVERS -------------------");
DebugTN("----------------- START SERVERS -------------------");
DebugTN("----------------- START SERVERS -------------------");
DebugTN("----------------- START SERVERS -------------------");

string postinstall_state;
dpGet(CSC_fwG_g_SYSTEM_NAME+":MYWARNING.value",postinstall_state);
if(postinstall_state!="postinstall_start"){

// if (os =="Linux")system(CSC_fwG_g_project_name_home+"/bin/start_servers "+CSC_fwG_g_project_name_home+" &"); // moved to postinstall (or to progs)
 if (os =="Linux") system("PVSS00ctrl -num 12 CMS_CSCfw_HV_CC/mudcsServer.ctl &");
 else system("start /b PVSS00ctrl -num 12 CMS_CSCfw_HV_CC/mudcsServer.ctl");

}
//PVSS00ctrl -num 11 CMS_CSCfw_HV_CC/delete_old_lib_versions.ctl &
//PVSS00ctrl -num 12 CMS_CSCfw_HV_CC/mudcsServer.ctl &


//startEditorNavigator();

mudcsServerCheck();

//------------------------------------------------------------
}
//========================================================================================
mudcsServerCheck(){
 delay(10);
 dpConnect("mudcsServerCheckIsRun",true,CSC_fwG_g_SYSTEM_NAME+":_Connections.Ctrl.ManNums");
 dpConnect("mudcsDimCheckIsRun",true,CSC_fwG_g_SYSTEM_NAME+":_Connections.Device.ManNums"); 
  
  
}
//========================================================================================
mudcsDimCheckIsRun(string dpName, dyn_int ctrl_mans){

delay(5);
if(!CSC_fwG_g_P5_SIM_AT_904)mudcsAllDimManagersStart(true);

  
}

//========================================================================================
//========================================================================================
mudcsServerCheckIsRun(string dpName, dyn_string ctrl_mans){

 // dyn_int drivers;
 // dpGet(CSC_fwG_g_SYSTEM_NAME+":_Connections.Driver.ManNums",drivers);
 // if(!dynContains(drivers,1))return;
  
string postinstall_state;
dpGet(CSC_fwG_g_SYSTEM_NAME+":MYWARNING.value",postinstall_state);
if(postinstall_state=="postinstall_start")return;
  
 dyn_int dists;
 dpGet(CSC_fwG_g_SYSTEM_NAME+":_Connections.Dist.ManNums",dists);
 //\\if(dynlen(dists)==0)return;  
  
//dpGet(CSC_fwG_g_SYSTEM_NAME+":_Connections.Ctrl.ManNums",ctrl_mans);
 if(!dynContains(ctrl_mans,12)){ // 12 is mudcsServer number
   delay(30);//delay(20);
 
 // dpGet(CSC_fwG_g_SYSTEM_NAME+":_Connections.Driver.ManNums",drivers);
 // if(!dynContains(drivers,1))return;
   
 dpGet(CSC_fwG_g_SYSTEM_NAME+":_Connections.Dist.ManNums",dists);
//\\ if(dynlen(dists)==0)return;  
     
 dpGet(CSC_fwG_g_SYSTEM_NAME+":_Connections.Ctrl.ManNums",ctrl_mans);
 if(dynContains(ctrl_mans,12))return;     
 
  if (os =="Linux")system(CSC_fwG_g_project_name_home+"/bin/start_servers "+CSC_fwG_g_project_name_home+" &"); // moved to postinstall (or to progs)
  else system("start /b PVSS00ctrl -num 12 CMS_CSCfw_HV_CC/mudcsServer.ctl");  
  
 }
  
}

//========================================================================================

// ================== mycorr++ ==============================
update_mywarning(string dpName, string value){
  
 if(FIRST_MYWARNING>=1){
   if(value == "init_finish"){
    mudcsEmuMessages("vs:"+"START IS COMPLETE \n YOU CAN OPERATE");
    fwFsm_restartAllDomains();
    mudcsHVMasterAlertConfig();
   }
 }
   if(value == "postinstall_finish"){
    mudcsEmuMessages("vs:"+"POSTINSTALL IS COMPLETE \n PLEASE RESTART THE PROJECT AGAIN");
    dpSet(CSC_fwG_g_SYSTEM_NAME+":"+"MYWARNING.value","warning_value_not_set");
   }
   else if(value == "postinstall_start"){
    mudcsEmuMessages("vs:"+"POSTINSTALL IS STARTED \n WAIT FOR COMPLETION MESSAGE");
   }   
 


 FIRST_MYWARNING++;
  
}

// ================== mycorr++ ==============================
update_new_dp_connected(string dpName, string value){

 if(FIRST_NEW_DP_CONNECTED>=1){
    mudcsEmuMessages("vs:"+value);
 }
 FIRST_NEW_DP_CONNECTED++;

}

//============================================================  
update_dpoints_not_controlled(string dpName, string value){

dyn_string selected_list;

 if(FIRST_CONTROLLED>=2){
/* commented out as:
  
    a) too much calls in setDevicesNotConnected >> setDpointsNotControlled
       (too many CSC_fwG_g_watch_mask items for LV_1)
    b) the ACTIVE_DISK_AND_CHAMBERS defined in getActiveDisksAndChambers
       is not used
       
    mudcsScanTreeForDevices("GENERAL", CSC_fwG_g_csc_part, selected_list);
    getActiveDisksAndChambers(selected_list);

*/

 }
 FIRST_CONTROLLED++;


}

//============================================================

update_csc_current_state(string dpName, string value){

int i;
int status;
dyn_string selected_list;

 if(value=="ERROR"){

/*
   mudcsScanTreeForDevices("GENERAL", "", selected_list);
   for(i=1;i<=dynlen(selected_list);i++){
    dpGet(mudcsAddSystem(selected_list[i])+".status",status);
     if(status == -1)mudcsDebug("ERROR in \n"+ selected_list[i]);

   } // for   
*/
get_error_devices();  

 } // if

}

//=============================================================
startEditorNavigator(){

 
string Module="EditorNavigator";

	if(!isModuleOpen(Module) ){ 
	int xPos, yPos, xSize, ySize;
	
	panelPosition(myModuleName(), "", xPos, yPos);
	panelSize("", xSize, ySize);
	ModuleOn(Module, xPos, yPos + 100, 10, 10, 1, 1, "Scale");
//	ModuleOn(Module, xPos + xSize, yPos, 0, 0, 1, 1, "");
//		ModuleOn(LVModule,1,1,0,0,1,1,1,""); 
	} 

	RootPanelOnModule("fwDeviceEditorNavigator/fwDeviceEditorNavigator.pnl", 
				"",Module, 
	makeDynString());



}

//======
//=======
//=======

get_error_devices()
{

dyn_string flags;
dyn_string chs;
int i;

//chs=fwFsmUi_getAllChildren("CSC_ME_P4","CSC_ME_P4",flags);
//for(i=1;i<dynlen(chs);i++){
// mudcsDebug(chs[i]);
//}


//string node="CMS_CSC";
//chs=fwFsmUi_getAllChildren(node,node,flags);

chs=makeDynString("CMS_CSC");
getch(1, chs[1], chs, makeDynInt(1));
/////getch(2, chs[1], chs, makeDynInt(1));

///fwFsmUi_setCUModeByName("CMS_CSC","CSC_ME_P4::CSC_ME_P4","Exclude");

///mudcsDebug("done");

}
//===========================================================
getch(int mode, string domain, dyn_string chs, dyn_int flags){

dyn_string chs1;
dyn_string split;
int status;
dyn_int flags1;
int i,j;
int count;
string mode_en, mode_dom_en;


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

   if(dynlen(chs1)>=1)getch(mode,split[1],chs1,flags1);


//---------------------------
 if(flags[i]==0){
      mode_dom_en=fwFsmUi_getDUMode(domain,chs[i]);
      if(mode_dom_en=="Enabled")mode_dom_en="Enabled";
      else mode_dom_en="Disabled";
 }
 else{
      mode_dom_en=fwFsmUi_getCUMode(domain,chs[i]);
      if(mode_dom_en=="Included")mode_dom_en="Enabled";
      else mode_dom_en="Disabled";
 } //else

//----------------------------
    count=0;
    for(j=1;j<=dynlen(chs1);j++){



     if(flags1[j]==2){

      dpGet(chs1[j]+".status",status);
      if(status==-1){
          count++;


         mode_en=fwFsmUi_getDUMode(split[1],chs1[j]);
         
///// temporal comment out:  if(mode_en=="Enabled" && mode_dom_en=="Enabled")mudcsDebug("ERROR in: \n "+chs1[j]);

      }
     }
    } // for j

  } // else
 } // for i


}

//================================================================================================
mudcsFsmPanelsCheck(){




//=================================================================

dyn_string os;
string o,s, s1, s2, s3;
int i,j, pos;
///dyn_string panels;
string p;

  dyn_string d_string1, d_string2;
  string label, o_cut;
//----------------------------------------------------------------- 
 
dyn_string objs;
string obj_found;
//==================================================================

objs=dpNames("*","_FwFsmObject");


for(i=1;i<=dynlen(objs);i++){

d_string1=strsplit(objs[i],"|");
d_string2=strsplit(d_string1[dynlen(d_string1)],"/");
o_cut=d_string2[dynlen(d_string2)];
    
///  panels=makeDynString(fwUiPnl);


//mudcsDebug(objs[i]);
os=strsplit(objs[i],"|");
o=os[dynlen(os)];

////////DebugTN(o);



 
s="ME_P"; s1="ME_M"; 
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-5) ||
    (pos=strpos(o,s1))>=0 && pos==(strlen(o)-5))  {
   obj_found=objs[i];
   break;

 } 



} //for

if(obj_found!=""){
  
  dpConnect("check_fsm_generated",true,obj_found+".ui.label");
////  mudcsDebug2(obj_found+".ui.label");
  
}
}

//=================================================================
check_fsm_generated(string dpName, string value){


    
dyn_string panels;
string dir_oper;
string Component;
Component="CMS_CSCfw_HV_CC"; 
dir_oper=Component;   
//=================================
string fwUiPnl;
 fwUiPnl=dir_oper+"/emufwUi.pnl";


dyn_string dps=strsplit(dpName,".");
string obj_found=dps[1];
///mudcsDebug("updated");
 dpGet(obj_found+".ui.panels:_original.._value",panels);
 if(!dynContains(panels,fwUiPnl) || !FIRST_FSM_PANELS){
   if(FIRST_FSM_PANELS)delay(80); // wait until fsm generation is over
   FIRST_FSM_PANELS=1;   
   mudcsSetFsmPanelsMain();
    if(isFunctionDefined("mudcsGasFsmPanelsAndFsmAliasesSetMain"))mudcsGasFsmPanelsAndFsmAliasesSetMain() ; 
 }
 

    
}
























