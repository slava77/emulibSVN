#uses "CMS_CSCfw_HV_CC/emuDcs.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs3.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs4.ctl"
#uses "CMS_CSCfw_HV_CC/emu_hvCommon.ctl"
#uses "CMS_CSC_common/emu_common.ctl"

string operating_system;


/* fsm : not used

mudcsCscSection_test()
{
int a;

//dpConnect("mudcsUpdate_test",CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.InRadDeviceList:_online.._value");
dpConnect("mudcsUpdate_test",CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.InRadDeviceList:_original.._value");

return;

}
mudcsUpdate_test(string dp, dyn_string value){

 mudcsDebug("Update");

}
*/

//=================================================================================================
mudcsProjectConnect(){
//mudcsDebug($project_system);
//if($project_system=="")mudcsInit(); // commented out as everything should be defined for when connection via PVSS00ui//  if the remote UI (PVSS00ui) is used (to define globals in particular)

string local_system=fwFsm_getSystemName();
if(local_system != CSC_fwG_g_SUPERVISOR_SYSTEM && (strpos(local_system,"cms_csc_dcs_")>=0 ) )return; // we do not have to close panels called from middleware project

string project_system_dp;  
//mudcsDebug(CSC_fwG_g_PROJECT_SYSTEM_DP);
if(globalExists("CSC_fwG_g_PROJECT_SYSTEM_DP") ){
  project_system_dp=CSC_fwG_g_PROJECT_SYSTEM_DP;
}
else{
  project_system_dp=CSC_fwG_g_SUPERVISOR_SYSTEM+":"+ "PROJECT_SYSTEM";
}
  
if(dpExists(project_system_dp) ){
dpSetWait(project_system_dp+".value",$project_system);  
  dpConnect("mudcsProjectMonitor",project_system_dp+".value");

}
else{
  mudcsDebug("RESTART SESSION (UI)"); 
}

}

//==================================================
mudcsProjectMonitor(string dp, string value)
{
  if(value != $project_system)ModuleOff(myModuleName()/*obj*/);
}

//=================================================================================================
mudcsDomainsStoring(){

dyn_string list, list_split;
int i;

dynClear(CSC_fwG_g_LIST_LOGICAL_DOMAINS);
dynClear(CSC_fwG_g_LIST_FSM_DOMAINS);

list=dpNames("*","FwNode");

 for(i=1;i<=dynlen(list);i++){
  if(strpos(list[i],CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_part)==0){
   dynAppend(CSC_fwG_g_LIST_LOGICAL_DOMAINS,list[i]);
   list_split=strsplit(list[i],"/");
   dynAppend(CSC_fwG_g_LIST_FSM_DOMAINS,list_split[dynlen(list_split)]);
  }
 }

dpSet(mudcsAddSystem("LIST_LOGICAL_DOMAINS."),CSC_fwG_g_LIST_LOGICAL_DOMAINS);
dpSet(mudcsAddSystem("LIST_FSM_DOMAINS."),CSC_fwG_g_LIST_FSM_DOMAINS);


}
//=================================================================================================
mudcsDomainsGetting(){

dyn_string list_l,list_f;
int i;

dynClear(CSC_fwG_g_LIST_LOGICAL_DOMAINS);
dynClear(CSC_fwG_g_LIST_FSM_DOMAINS);

dpGet(mudcsAddSystem("LIST_LOGICAL_DOMAINS."),list_l);
dpGet(mudcsAddSystem("LIST_FSM_DOMAINS."),list_f);

CSC_fwG_g_LIST_LOGICAL_DOMAINS=list_l;
CSC_fwG_g_LIST_FSM_DOMAINS=list_f;

}

//=================================================================================================
mudcsGetLogicalDomainByFsmDomain(string fsm, string &logical){

 int index=dynContains(CSC_fwG_g_LIST_FSM_DOMAINS,fsm);
 logical=CSC_fwG_g_LIST_LOGICAL_DOMAINS[index];

}

//======== templates: should be called in Visual Server for different station projects (components)======================
mudcsInit_m4m3(){
 dyn_int project_station_set=makeDynInt(1,2); mudcsInit("",project_station_set);
}
mudcsInit_m2(){
 dyn_int project_station_set=makeDynInt(3); mudcsInit("",project_station_set);
}
mudcsInit_m1(){
 dyn_int project_station_set=makeDynInt(4); mudcsInit("",project_station_set);
}
mudcsInit_p1(){
 dyn_int project_station_set=makeDynInt(5); mudcsInit("",project_station_set);
}
mudcsInit_p2(){
 dyn_int project_station_set=makeDynInt(6); mudcsInit("",project_station_set);
}
mudcsInit_p4p3(){
 dyn_int project_station_set=makeDynInt(7,8); mudcsInit("",project_station_set);
}

//=================================================================================================
mudcsInit(string project_station_fwUi="",dyn_int project_station_set_par=makeDynInt()){

  bool retieve_project_from_system_name=true;       // supposed to be true at P5 Windows XP, no matter at 904
  bool temporary_windows_pc_for_hv_cc_project=false;
  bool P5_SIM_AT_904=false;                          // no matter at P5, supposed to be true for P5 SIM mode at 904
  
  
addGlobal("CSC_fwG_g_P5_SIM_AT_904",STRING_VAR);  
CSC_fwG_g_P5_SIM_AT_904=P5_SIM_AT_904;
     
string os=getenv("OS");
addGlobal("CSC_fwG_g_OS",STRING_VAR);
CSC_fwG_g_OS=os;

addGlobal("CSC_fwG_g_PVSS_VERSION",STRING_VAR);  
CSC_fwG_g_PVSS_VERSION= mudcsGetPvssVersion();

//-------- instruction for installation of new project before the tree creation: -----
//--------- a) copy (if not present) the emuDcs2.ctl (with this function) to the project libs (no need if functions mudcsInit_m4m3 ... are used) 
//--------- b) change the following one variable
//--------- c) check  the line ended with: +++++++++++++++++++++++++++++++1
//--------- d) check  the line ended with: +++++++++++++++++++++++++++++++2  
string sTest;
dyn_string dsTest,dsTest2,dsTest3 ;  
string project_suffix="";
int project_station_from_host;

dyn_int project_station_set;
//-----
addGlobal("CSC_fwG_g_IS_IMAX_SET",BOOL_VAR);
CSC_fwG_g_IS_IMAX_SET=false;
//-----
addGlobal("CSC_fwG_g_904_MACHINE",STRING_VAR);
addGlobal("CSC_fwG_g_904_HV_MACHINE",STRING_VAR);
addGlobal("CSC_fwG_g_904",BOOL_VAR);
string emu904HostnamePrefix = "emu-dcs-dev";
string emu904MainMachine = "1";
//CSC_fwG_g_904_MACHINE = emu904HostnamePrefix + emu904MainMachine;
CSC_fwG_g_904_MACHINE = "emuslice12"; // location of the DIM tunnel to P5
CSC_fwG_g_904_HV_MACHINE="137.138.15.212";//"10.176.11.103";
sTest=getHostname();
dsTest=strsplit(sTest,"."); // just in case
if((strpos(dsTest[1], emu904HostnamePrefix) == 0) || (strpos(dsTest[1], "macfrank") >= 0)){
  if(!P5_SIM_AT_904)CSC_fwG_g_904=true;
  else CSC_fwG_g_904=false;
  retieve_project_from_system_name=true;
}
else CSC_fwG_g_904=false;
//dsTest=strsplit(dsTest[1],"-");

//------
////if(dynlen(project_station_set_par)==0)project_station_set=makeDynInt(5); // no matter: redefined below for call from  native project: once from VisualServer
////else project_station_set=project_station_set_par;

addGlobal("CSC_fwG_g_StationWithFsmHwTree",INT_VAR);
CSC_fwG_g_StationWithFsmHwTree = 5; // 6 is  P2 +++++++++++++++++++++++++++++++++++++++++++2

if(project_station_fwUi != ""){
 int project_station_fwUi_int=project_station_fwUi;
 if(project_station_fwUi_int > 100){
    project_station_fwUi=CSC_fwG_g_StationWithFsmHwTree;
    project_station_fwUi_int=project_station_fwUi;   
        
 } 
 project_station_set=makeDynInt(project_station_fwUi_int);
}

string project_station_with_hw="CMS_CSC_HV_CC"+"_"+CSC_fwG_g_StationWithFsmHwTree;
string project_station_with_hw_new="CSC_HV"; // to use with fwCU_getDp("CSC_HV",dp)
fwCU_getDp(project_station_with_hw_new,project_station_with_hw_new);

//- new organization: the below piece works for below only in local project! (from VisualServer) ----------------------------------------------
if(project_station_fwUi==""){ // case of call not from FSM panel, so definetly from native project: once from VisualServer


//-----
if(CSC_fwG_g_904 || retieve_project_from_system_name){ //retieve_project_from_system_name)
  if(CSC_fwG_g_904 && CSC_fwG_g_904_MACHINE=="dcspcS2G19-01"){
    project_station_from_host=5; //just as the test
  }
  else{
sTest=getSystemName();
strreplace(sTest,":","");
dsTest=strsplit(sTest,"_");
project_station_from_host=dsTest[dynlen(dsTest)];
//if(substr(dsTest[dynlen(dsTest)],0,2)=="m3")project_station_from_host=2;
//else if(substr(dsTest[dynlen(dsTest)],0,2)=="m2")project_station_from_host=3;
//else if(substr(dsTest[dynlen(dsTest)],0,2)=="m1")project_station_from_host=4;
//else if(substr(dsTest[dynlen(dsTest)],0,2)=="p1")project_station_from_host=5;
//else if(substr(dsTest[dynlen(dsTest)],0,2)=="p2")project_station_from_host=6;
//else if(substr(dsTest[dynlen(dsTest)],0,2)=="p3")project_station_from_host=7;
  }
} // if(CSC_fwG_g_904)
else{
sTest=getHostname();
dsTest=strsplit(sTest,"."); // just in case
dsTest=strsplit(dsTest[1],"-");
project_station_from_host=dsTest[dynlen(dsTest)];
}
//----
  
//project_station_from_host=dsTest[dynlen(dsTest)];
//if(project_station_from_host==1)project_station_set=makeDynInt(1,2);

if(project_station_from_host==0){project_station_set=makeDynInt(4);project_suffix="_m1";}  // temporal
else if(project_station_from_host==2){project_station_set=makeDynInt(1,2,3);project_suffix="_m3m4";}
else if(project_station_from_host==3){project_station_set=makeDynInt(1,2,3);project_suffix="_m2";}
else if(project_station_from_host==4){project_station_set=makeDynInt(4);project_suffix="_m1";}
else if(project_station_from_host==5){project_station_set=makeDynInt(5,6);project_suffix="_p1";}
else if(project_station_from_host==6){project_station_set=makeDynInt(6,7,8);project_suffix="_p2";}
else if(project_station_from_host==7){project_station_set=makeDynInt(6,7,8);project_suffix="_p3p4";}
else if(project_station_from_host==8){project_station_set=makeDynInt(5);project_suffix="_p3p4";}

if(temporary_windows_pc_for_hv_cc_project){project_station_set=makeDynInt(6,7);project_suffix="_p2";}

}
//mudcsDebug(project_station_from_host);
//mudcsDebug(project_suffix);
//------------------------------------------------

//------------------------------------------------

addGlobal("CSC_fwG_g_PCRATES_MAP",MAPPING_VAR);
mudcsPcCratesMapping();

addGlobal("CSC_fwG_g_HOME",STRING_VAR);
if (os =="Linux")CSC_fwG_g_HOME = getenv("HOME");//"/nfshome0/cscdcsdev"; 
else CSC_fwG_g_HOME = "c:\\pvss\\components";
//------------------------------------------------

int i; 
string project_station="CMS_CSC_HV_CC"+"_"+project_station_set[1];//="M2";  it works for below only in local project! (from VisualServer) 
string project_station_new; // to use with fwCU_getDp(project_station_new,dp)
dyn_string station_suffixes=makeDynString("M4","M3","M2","M1","P1","P2","P3","P4");
int project_station_fwUi_int;
    
dyn_string project_stations;
dyn_string ds_stations;
   dyn_string systemNamesDps;
   dyn_string s_split;

for(i=1;i<=dynlen(project_station_set);i++){ // it works for below only in local project! (from VisualServer) 
  dynAppend(project_stations,"CMS_CSC_HV_CC"+"_"+project_station_set[i]);
}
//------------------------------------------------------------------
if(project_station_fwUi != ""){ // case of call from FSM panel
 project_station="CMS_CSC_HV_CC"+"_"+project_station_fwUi;
 
 project_station_fwUi_int=project_station_fwUi;
 project_station_new="CSC_"+station_suffixes[project_station_fwUi_int];
 fwCU_getDp(project_station_new,project_station_new);
}
else if(project_station_fwUi==""){ // case of call not from FSM panel, so definetly from native project: once from VisualServer
  ds_stations=dpNames("CMS_CSC_HV_CC*","MUDCS_STRING"); 
  for(i=1;i<=dynlen(ds_stations);i++){
   s_split=strsplit(ds_stations[i],":");
   if(!dynContains(project_stations,s_split[dynlen(s_split)]) ){
    dpDelete(s_split[dynlen(s_split)]);  
   } 
  }
  for(i=1;i<=dynlen(project_stations);i++){    
   if(!dpExists(project_stations[i])) 
   dpCreate(project_stations[i],"MUDCS_STRING");
   dpSet(project_stations[i]+".value","valid");
  }
}

//-----------------------------------------------------------------------
int i;

addGlobal("CSC_fwG_g_Component",STRING_VAR);
CSC_fwG_g_Component = "CMS_CSCfw_HV_CC"; // fsm: "mudcs"

addGlobal("CSC_fwG_CSC_fwG_g_SYSTEM_NAME2",STRING_VAR);
CSC_fwG_CSC_fwG_g_SYSTEM_NAME2 = "CMS_CSC"; // fsm: "mudcs"


//------ the piece only really works if project_station_fwUi=="": because otherwise redefined in the emufwUi.pnl -------------------------
//---- actually it works here also for the project_station_fwUi !="" because the system name is defined only here -->
// --> the transmission of $project_system into panels is for two reasons: 
// --> a) to synchroniz. of projects b) advanced usage of project_name in panels (not done yet)

//CSC_fwG_CSC_fwG_g_SYSTEM_NAME2 = "CMS_CSC"; // fsm: "mudcs"

//------------    
   systemNamesDps=dpNames("*:"+project_station);
//   systemNamesDps=dpNames("*:"+project_station_new); 
//------------
   if(dynlen(systemNamesDps)==0)mudcsDebug("SYSTEM NAME NOT DEFINED !");
   else{
    s_split=strsplit(systemNamesDps[1],":"); 
    CSC_fwG_CSC_fwG_g_SYSTEM_NAME2=s_split[1];//dpSubStr(systemNamesDps[1], DPSUB_SYS);
    ///mudcsLvDebug(CSC_fwG_CSC_fwG_g_SYSTEM_NAME2);
   }
//--------------------------------------------------------------------------  
 //------------    
   systemNamesDps=dpNames("*:"+project_station_with_hw);
//   systemNamesDps=dpNames("*:"+project_station_with_hw_new);   
//------------
   addGlobal("CSC_fwG_g_HW_SYSTEM_NAME",STRING_VAR);
  
   if(dynlen(systemNamesDps)==0){
     DebugTN("HARDWARE FOLDERS SYSTEM NAME NOT DEFINED FOR THIS PROJECT!");
     CSC_fwG_g_HW_SYSTEM_NAME="NOT_DEFINED_FOR_THIS_PROJECT";
   }
   else{
    s_split=strsplit(systemNamesDps[1],":"); 
    CSC_fwG_g_HW_SYSTEM_NAME=s_split[1];//dpSubStr(systemNamesDps[1], DPSUB_SYS);
    ///mudcsLvDebug(CSC_fwG_CSC_fwG_g_SYSTEM_NAME2);
   }
     
      /*
 //------------    
   systemNamesDps=dpNames("*:"+project_station_with_hw);
//------------
   addGlobal("CSC_fwG_g_HW_SYSTEM_NAME",STRING_VAR);
   int wait_for_connection=0;
   if(dynContains(project_station_set,CSC_fwG_g_StationWithFsmHwTree)){
  while(1){
    if(dynlen(systemNamesDps)==0){
      wait_for_connection++; delay(3);
      if(wait_for_connection>20){
       mudcsDebug("HARDWARE FOLDERS SYSTEM NAME NOT DEFINED !");
       break;
      }
      else{DebugTN("Waiting for projects connection ........");}
    }
   else{
    s_split=strsplit(systemNamesDps[1],":"); 
    CSC_fwG_g_HW_SYSTEM_NAME=s_split[1];//dpSubStr(systemNamesDps[1], DPSUB_SYS);
    break;
    ///mudcsLvDebug(CSC_fwG_CSC_fwG_g_SYSTEM_NAME2);
   }
  } // while
   } // if
   else{
     CSC_fwG_g_HW_SYSTEM_NAME="any";
   }
   */
//--------------------------------------------------------------------------    
dyn_string super_systems=dpNames("*:CMS_CSC_Super");
dyn_string super_systems_split;


addGlobal("CSC_fwG_g_SUPERVISOR_SYSTEM",STRING_VAR);
if(dynlen(super_systems)>=1){
  super_systems_split=strsplit(super_systems[1],":");
CSC_fwG_g_SUPERVISOR_SYSTEM=super_systems_split[1]; //CSC_fwG_CSC_fwG_g_SYSTEM_NAME2;  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++1
}
else CSC_fwG_g_SUPERVISOR_SYSTEM="";

addGlobal("CSC_fwG_g_SYSTEM_NAME",STRING_VAR);
CSC_fwG_g_SYSTEM_NAME = CSC_fwG_CSC_fwG_g_SYSTEM_NAME2; // fsm: "mudcs"

addGlobal("CSC_fwG_g_idisk_cross_numbers",DYN_INT_VAR);
CSC_fwG_g_idisk_cross_numbers=project_station_set;//makeDynInt(2,3);
//---------------------
addGlobal("CSC_fwG_g_CRB_SYSTEM_NAME",STRING_VAR);
////CSC_fwG_g_CRB_SYSTEM_NAME="CMS_CSCfw_LV_CAN";

addGlobal("CSC_fwG_g_MRTN_SYSTEM_NAME_P",STRING_VAR);
addGlobal("CSC_fwG_g_MRTN_SYSTEM_NAME_M",STRING_VAR);
////CSC_fwG_g_MRTN_SYSTEM_NAME="CMS_CSCfw_LV_CAN";

addGlobal("CSC_fwG_g_Dubna_System_Name",STRING_VAR);

//--------------------------------------------------------------------------

   CSC_fwG_g_CRB_SYSTEM_NAME="";
   systemNamesDps=dpNames("*:CMS_CSC_LV_CRB");
   if(dynlen(systemNamesDps)==0)DebugTN("CRB SYSTEM NAME NOT DEFINED !");
   else{
    s_split=strsplit(systemNamesDps[1],":"); 
    CSC_fwG_g_CRB_SYSTEM_NAME=s_split[1];//dpSubStr(systemNamesDps[1], DPSUB_SYS);
   }
   CSC_fwG_g_MRTN_SYSTEM_NAME_P="";
   systemNamesDps=dpNames("*:CMS_CSC_LV_MRTN");
   if(dynlen(systemNamesDps)==0)DebugTN("MRTN_P SYSTEM NAME NOT DEFINED !");
   else{
    s_split=strsplit(systemNamesDps[1],":"); 
    CSC_fwG_g_MRTN_SYSTEM_NAME_P=s_split[1];//dpSubStr(systemNamesDps[1], DPSUB_SYS);
   }
   CSC_fwG_g_MRTN_SYSTEM_NAME_M="";
   systemNamesDps=dpNames("*:CMS_CSC_LV_MRTN");
   if(dynlen(systemNamesDps)==0)DebugTN("MRTN_M SYSTEM NAME NOT DEFINED !");
   else{
    s_split=strsplit(systemNamesDps[1],":"); 
    CSC_fwG_g_MRTN_SYSTEM_NAME_M=s_split[1];//dpSubStr(systemNamesDps[1], DPSUB_SYS);
   }

   CSC_fwG_g_Dubna_System_Name="";   
   systemNamesDps=dpNames("*:CSC_ME_11_HV|CSC_ME_11_HV");
   if(dynlen(systemNamesDps)==0)DebugTN("DUBNA ME11 SYSTEM NAME NOT DEFINED !");
   else{
    s_split=strsplit(systemNamesDps[1],":"); 
    CSC_fwG_g_Dubna_System_Name=s_split[1];//dpSubStr(systemNamesDps[1], DPSUB_SYS);
  
   }

//--------------------------------------------------------------------------    

//----

addGlobal("CSC_fwG_g_CRB_DOMAIN",STRING_VAR);
CSC_fwG_g_CRB_DOMAIN="CMS_CSC_LV_CAN1";

addGlobal("CSC_fwG_g_MRTN_DOMAIN_P",STRING_VAR);
CSC_fwG_g_MRTN_DOMAIN_P="CMS_CSC_LV_CAN2";

addGlobal("CSC_fwG_g_MRTN_DOMAIN_M",STRING_VAR);
CSC_fwG_g_MRTN_DOMAIN_M="CMS_CSC_LV_CAN2_M";

//--------------------------
addGlobal("CSC_fwG_g_CUSTOM_LV",BOOL_VAR);
CSC_fwG_g_CUSTOM_LV=false;

addGlobal("CSC_fwG_g_csc_part",STRING_VAR);

if(CSC_fwG_g_SYSTEM_NAME == "System1"){
 CSC_fwG_g_csc_part="CSC1";
}
else if(CSC_fwG_g_SYSTEM_NAME == CSC_fwG_CSC_fwG_g_SYSTEM_NAME2){
 CSC_fwG_g_csc_part=CSC_fwG_CSC_fwG_g_SYSTEM_NAME2;//"CMS_CSC";//"CSC2";
 ////CSC_fwG_g_csc_part="CMS_CSC1"; 
}
//========



//========
addGlobal("CSC_fwG_g_MainLogicalFsmType",STRING_VAR);
CSC_fwG_g_MainLogicalFsmType = "EMUNodes"; 

addGlobal("CSC_fwG_g_NodeLogicalFsmType",STRING_VAR);
CSC_fwG_g_NodeLogicalFsmType = "EMUTREENodes"; 

addGlobal("CSC_fwG_g_NodeStbyFsmType",STRING_VAR);
CSC_fwG_g_NodeStbyFsmType = "EMUSTBYNodes"; 
//========
addGlobal("CSC_fwG_g_FSM_V24R09", BOOL_VAR);
addGlobal("CSC_fwG_g_CSC_INTERLOCK_FLAG", BOOL_VAR);

int csc_interlock_flag;
dpGet(mudcsAddSystem("CSC_INTERLOCK_FLAG."),csc_interlock_flag);
CSC_fwG_g_CSC_INTERLOCK_FLAG=csc_interlock_flag;


addGlobal("CSC_fwG_g_LIST_LOGICAL_DOMAINS", DYN_STRING_VAR);
addGlobal("CSC_fwG_g_LIST_FSM_DOMAINS", DYN_STRING_VAR);

addGlobal("CSC_fwG_g_EMAILS_GENERAL", STRING_VAR);
addGlobal("CSC_fwG_g_EMAILS_HV", STRING_VAR);
addGlobal("CSC_fwG_g_EMAILS_GAS", STRING_VAR);
addGlobal("CSC_fwG_g_EMAILS_WTH", STRING_VAR);

CSC_fwG_g_EMAILS_GENERAL=" valeri.sytnik@cern.ch ";
CSC_fwG_g_EMAILS_HV=" valeri.sytnik@cern.ch ";
CSC_fwG_g_EMAILS_GAS=" valeri.sytnik@cern.ch vandreev@mail.cern.ch sytnik@physics.ucr.edu 41764874195@mail2sms.cern.ch";
CSC_fwG_g_EMAILS_WTH=" valeri.sytnik@cern.ch ";


addGlobal("CSC_fwG_g_IS_ADJUST_TO_CONTROL_SIDE", BOOL_VAR); 
CSC_fwG_g_IS_ADJUST_TO_CONTROL_SIDE=false;


addGlobal("CSC_fwG_g_IS_TEMPORAL_50V",BOOL_VAR);
CSC_fwG_g_IS_TEMPORAL_50V=true;

addGlobal("CSC_fwG_g_HV_36CHANNEL_BOARDS_LIST", DYN_STRING_VAR);
addGlobal("CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST", DYN_DYN_STRING_VAR);

CSC_fwG_g_HV_36CHANNEL_BOARDS_LIST=makeDynString();
CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST=makeDynString();

addGlobal("CSC_fwG_g_FULL_DPLIST", DYN_STRING_VAR); 


addGlobal("CSC_fwG_g_BROKER_HANDLERS_FIRST", DYN_STRING_VAR); 
addGlobal("CSC_fwG_g_STATUS_HANDLERS_FIRST", DYN_STRING_VAR);

addGlobal("CSC_fwG_g_BLOCK_NEXT_DPCONNECT", INT_VAR);


addGlobal("CSC_fwG_g_wheel_labels", DYN_STRING_VAR);
addGlobal("CSC_fwG_g_radius_labels", DYN_STRING_VAR);
addGlobal("CSC_fwG_g_project_name_home", STRING_VAR);
addGlobal("CSC_fwG_g_jcop_dim_home", STRING_VAR);

addGlobal("CSC_fwG_CSC_fwG_g_radius_labels_short", STRING_VAR);
addGlobal("CSC_fwG_g_WHEELS",DYN_STRING_VAR);
addGlobal("CSC_fwG_g_RADIUSES",DYN_STRING_VAR);

addGlobal("CSC_fwG_g_IS_CALL_SELECT_HANDLER", BOOL_VAR);

addGlobal("CSC_fwG_g_alct_variables",DYN_STRING_VAR);
addGlobal("CSC_fwG_g_cfeb_variables",DYN_STRING_VAR);

addGlobal("CSC_fwG_g_csc_prefix",DYN_STRING_VAR);
CSC_fwG_g_csc_prefix = "CSC";

addGlobal("CSC_fwG_g_DEFINE_ELMB",INT_VAR);

addGlobal("CSC_fwG_g_DISPLAY_SAVED", STRING_VAR);
addGlobal("CSC_fwG_g_GAS_ERROR_FLAG",INT_VAR);
addGlobal("CSC_fwG_g_GAS_WARNING_FLAG",INT_VAR);
addGlobal("CSC_fwG_g_CopyDimBrokerList",DYN_STRING_VAR);

addGlobal("CSC_fwG_g_EMU_ALIASES", DYN_STRING_VAR);
addGlobal("CSC_fwG_g_EMU_NAME_ALIASES", DYN_STRING_VAR);

addGlobal("CSC_fwG_g_HV_STATUS", DYN_STRING_VAR);
addGlobal("CSC_fwG_g_CURRENT_MASTER_INDEX",INT_VAR);

addGlobal("CSC_fwG_g_SLICE_DPOINTS",DYN_STRING_VAR);
addGlobal("CSC_fwG_g_HV_LONG_TERM_DPOINTS",DYN_STRING_VAR);

addGlobal("CSC_fwG_g_all_masters",DYN_STRING_VAR);
addGlobal("CSC_fwG_g_all_1masters",DYN_STRING_VAR);
addGlobal("CSC_fwG_g_all_2masters",DYN_STRING_VAR);

addGlobal("CSC_fwG_g_host", INT_VAR);
addGlobal("CSC_fwG_g_1host", INT_VAR);
addGlobal("CSC_fwG_g_2host", INT_VAR);

addGlobal("CSC_fwG_EmuCmsSpecialMode",INT_VAR);
addGlobal("EmuCmsGlobalNode",STRING_VAR);
addGlobal("CSC_fwG_EmuCmsGlobalType",STRING_VAR);
addGlobal("CSC_fwG_EmuCmsGlobalCu",STRING_VAR);
addGlobal("CSC_fwG_EmuCmsGlobalParent",STRING_VAR);

addGlobal("CSC_fwG_g_all_primary",DYN_STRING_VAR);
addGlobal("CSC_fwG_g_all_hosts",DYN_INT_VAR);

addGlobal("CSC_fwG_g_all_maratons",DYN_STRING_VAR);
addGlobal("CSC_fwG_g_all_wieners12",DYN_STRING_VAR);

addGlobal("CSC_fwG_g_FsmPanelMaxSizeX",INT_VAR);
addGlobal("CSC_fwG_g_FsmPanelMaxSizeY",INT_VAR);

addGlobal("g_HV_ID2PC_NAME",DYN_STRING_VAR);
addGlobal("CSC_fwG_g_PCRATE_ID2PC_NAME",DYN_STRING_VAR); //
addGlobal("CSC_fwG_g_BROKER_DNS_MACHINE_NAMES",DYN_STRING_VAR);
addGlobal("CSC_fwG_g_STATIONS_DNS_MACHINE_NAMES",DYN_STRING_VAR);

addGlobal("CSC_fwG_g_dyn_debug1",DYN_STRING_VAR);
addGlobal("CSC_fwG_g_dyn_debug2",DYN_STRING_VAR);

addGlobal("CSC_fwG_g_master2chamber",DYN_STRING_VAR);

addGlobal("CSC_fwG_g_buffer_mrtn_commands",DYN_INT_VAR);
addGlobal("CSC_fwG_g_mrtn_threadID_enable",INT_VAR);
addGlobal("CSC_fwG_g_mrtn_threadID_disable",INT_VAR);

CSC_fwG_g_mrtn_threadID_enable=-1;
CSC_fwG_g_mrtn_threadID_disable=-1;

CSC_fwG_g_FSM_V24R09=true;
if(CSC_fwG_g_OS != "Linux")CSC_fwG_g_FSM_V24R09=false;
    
CSC_fwG_g_dyn_debug1=makeDynString();
CSC_fwG_g_dyn_debug2=makeDynString();
CSC_fwG_g_master2chamber=makeDynString();




if(CSC_fwG_g_904){
g_HV_ID2PC_NAME=makeDynString("800_part1;"+CSC_fwG_g_904_HV_MACHINE+"_part1",
                              "800_part2;"+CSC_fwG_g_904_HV_MACHINE+"_part2",
                              "800_part3;"+CSC_fwG_g_904_HV_MACHINE+"_part3",
                              "800_part4;"+CSC_fwG_g_904_HV_MACHINE+"_part4",
                              "400;fake"); // ufcmshv2 & ufcmshv1 (for primaries)
}
else{
if(CSC_fwG_g_idisk_cross_numbers[1]>=5 && CSC_fwG_g_idisk_cross_numbers[1]<=8){
g_HV_ID2PC_NAME=makeDynString("600_part1;10.176.11.103_part1","600_part2;10.176.11.103_part2",
                              "600_part3;10.176.11.103_part3","600_part4;10.176.11.103_part4",
                              "500_part4;10.176.11.67_part4"); // ufcmshv2 & ufcmshv1 (for primaries)
}
else{
g_HV_ID2PC_NAME=makeDynString("500_part1;10.176.11.67_part1","500_part2;10.176.11.67_part2",
                              "500_part3;10.176.11.67_part3","500_part4;10.176.11.67_part4",
                              "400;fake"); // ufcmshv2  
}
}
//CSC_fwG_g_PCRATE_ID2PC_NAME=makeDynString("02:00:00:00:00:05 02:00:00:00:00:03 02:00:00:00:00:0f 02:00:00:00:00:1a;dcspcs2g19-06");

if(CSC_fwG_g_904)
CSC_fwG_g_PCRATE_ID2PC_NAME=makeDynString("all;"+CSC_fwG_g_904_MACHINE);
else  
CSC_fwG_g_PCRATE_ID2PC_NAME=makeDynString("all;csc-dcs-pc1","all;csc-dcs-pc2");

// the CSC_fwG_g_BROKER_DNS_MACHINE_NAMES is used in:
//    to configure services for broker dps and commands (function: mudcsDimConfigOneManager)
//    to configure general dps (WTH, PT100 etc) (function mudcsDimConfigOneManagerNew)
if(CSC_fwG_g_904)
  CSC_fwG_g_BROKER_DNS_MACHINE_NAMES=makeDynString
    ("all;"+CSC_fwG_g_904_MACHINE,
    "LV_1;"+CSC_fwG_g_904_MACHINE,
    "HV_PR;"+CSC_fwG_g_904_HV_MACHINE+"_part4", // ufcmshv2: for primary commands only  +Z
    "HV_1;"+CSC_fwG_g_904_HV_MACHINE+"_part4"   // ufcmshv2: for primary commands only +Z
     ); 
else
CSC_fwG_g_BROKER_DNS_MACHINE_NAMES=makeDynString
    ("all;csc-dcs-pc1",
    "LV_1;csc-dcs-pc1",
    "LV_1;csc-dcs-pc2",
    "LV_SX5;csc-dcs-pc1",
    "HV_PR;10.176.11.103_part4", // ufcmshv2: for primary commands only  +Z
    "HV_PR;10.176.11.67_part4", // ufcmshv1: for primary commands only   -Z   
    "HV_1;10.176.11.103_part4",   // ufcmshv2: for primary commands only +Z
    "HV_1;10.176.11.67_part4"   // ufcmshv1: for primary commands only   -Z
//    "HV_PR;ufcmspc02",
//    "HV_1;ufcmspc02"
     ); // HV_1 is needed for HV_1_COMMAND so that they are configured on all machines (not only on dcspcs2g19-06)

if(CSC_fwG_g_904)
CSC_fwG_g_STATIONS_DNS_MACHINE_NAMES=makeDynString("all;"+CSC_fwG_g_904_MACHINE);
else
CSC_fwG_g_STATIONS_DNS_MACHINE_NAMES=makeDynString("all;csc-dcs-pc1");


CSC_fwG_g_FsmPanelMaxSizeX=1200;
CSC_fwG_g_FsmPanelMaxSizeY=760;

CSC_fwG_EmuCmsSpecialMode=0;

addGlobal("CSC_fwG_g_last_time_wth_alarm", INT_VAR);
CSC_fwG_g_last_time_wth_alarm=getCurrentTime();
addGlobal("CSC_fwG_g_last_time_pt100_alarm", INT_VAR);
CSC_fwG_g_last_time_pt100_alarm=getCurrentTime();
if(project_station_fwUi==""){
addGlobal("CSC_fwG_g_watch_mask",DYN_STRING_VAR); // is defined and used only in the mudcsSErver.ctl
}


//--------------------------------------------------
if(CSC_fwG_g_SYSTEM_NAME == "System1"){
CSC_fwG_g_host=500;

CSC_fwG_g_all_primary=makeDynString("primary500");

CSC_fwG_g_all_masters = makeDynString(CSC_fwG_g_SYSTEM_NAME+":"+"master500_1_1",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master500_1_15");

CSC_fwG_g_SLICE_DPOINTS=makeDynString(	
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp2r1c16",
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp3r1c16",
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp2r2c31",
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp2r2c32",
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp3r2c31",
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp3r2c32");

CSC_fwG_g_HV_LONG_TERM_DPOINTS=makeDynString(

CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp2r1c11",
CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp2r1c12",
CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp2r1c14",

CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp3r1c11",
CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp3r1c12",
CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp3r1c13",
CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp3r1c14",
CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp3r1c15",

CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp2r2c30",
CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"dp3r2c30");

}
//--------------------------------------------------
else if(CSC_fwG_g_SYSTEM_NAME == CSC_fwG_CSC_fwG_g_SYSTEM_NAME2){
// new piece for moving to tree close to slice test
  // attention !: the CSC_fwG_g_SLICE_DPOINTS and CSC_fwG_g_HV_LONG_TERM_DPOINTS
  // should be corrected too: (but not for tree creation)
/* not used
CSC_fwG_g_host=500;
CSC_fwG_g_1host=500;
CSC_fwG_g_2host=500; 
*/

//============= DB ===========================
 int i_db;
 dyn_string emu_db;
 dyn_string hosts_db; 
 dyn_string s_split, s_split2;
 dyn_string CscLevelDevices;
 dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);
 for(i_db=1;i_db<=dynlen(CscLevelDevices);i_db++){
   if(strpos(CscLevelDevices[i_db],"HV_PRIMARY")>=0 ){
     s_split=strsplit(CscLevelDevices[i_db]," ");
     dynAppend(emu_db,substr(s_split[1], strlen("HighVoltage/HV_PR_")  )) ;
     s_split2=strsplit(s_split[2],";");
     if( !dynContains(hosts_db,s_split2[2]) )dynAppend(hosts_db,s_split2[2]);
   } 
 }
     //CSC_fwG_g_all_maratons=emu_db;
     CSC_fwG_g_all_primary=emu_db;
     CSC_fwG_g_all_hosts=hosts_db;
//============================================

//CSC_fwG_g_all_hosts = makeDynInt(500);
//CSC_fwG_g_all_primary=makeDynString("primary500_1_1","primary500_1_2");


/////CSC_fwG_g_all_maratons=makeDynString("maraton01","maraton02");
/////CSC_fwG_g_all_wieners12=makeDynString("wiener1201","wiener1202","wiener1203");
int idisk_cross,idisk_cross_i;

dyn_string disk_db,DiskLevelDevices;

for(idisk_cross_i=1;idisk_cross_i<=dynlen(CSC_fwG_g_idisk_cross_numbers);idisk_cross_i++){
      idisk_cross=CSC_fwG_g_idisk_cross_numbers[idisk_cross_i];
  if((idisk_cross==4 || idisk_cross==5)){ // i.e. ME 1/1
    dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Ring_DUBNA"+"."+"DiskLevelDevicesCoordinates"),DiskLevelDevices);
     for(i_db=1;i_db<=dynlen(DiskLevelDevices);i_db++){
      if(idisk_cross==4 && i_db <= 36){
       if(strpos(DiskLevelDevices[i_db],"master") >=0)
        {if(!dynContains(disk_db,mudcsAddSystem(DiskLevelDevices[i_db])))dynAppend(disk_db,mudcsAddSystem(DiskLevelDevices[i_db]) );}
      }
      else if(idisk_cross==5 && i_db >=37){
       if(strpos(DiskLevelDevices[i_db],"master") >=0)
        {if(!dynContains(disk_db,mudcsAddSystem(DiskLevelDevices[i_db])))dynAppend(disk_db,mudcsAddSystem(DiskLevelDevices[i_db]) );}        
      }
     } // for 
  }
  //else{
   dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel"+idisk_cross+"."+"DiskLevelDevicesCoordinates"),DiskLevelDevices);
   for(i_db=1;i_db<=dynlen(DiskLevelDevices);i_db++){
   
        if(strpos(DiskLevelDevices[i_db],"master") >=0)
        { 
          if(!dynContains(disk_db,mudcsAddSystem(DiskLevelDevices[i_db])) )dynAppend(disk_db,mudcsAddSystem(DiskLevelDevices[i_db]) );
        }
   }
 //} // else     
} // for idisk_cross

CSC_fwG_g_all_masters = disk_db;
//DebugTN("MASTERS============================================");
//DebugTN(CSC_fwG_g_all_masters);

//CSC_fwG_g_all_masters = dpNames("master*","HV_1_d");




/*
CSC_fwG_g_all_masters = makeDynString(CSC_fwG_g_SYSTEM_NAME+":"+"master500_3_0",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master500_3_1",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master500_3_2",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master500_3_3",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master500_3_4",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master500_3_5"
);

CSC_fwG_g_all_1masters = makeDynString(CSC_fwG_g_SYSTEM_NAME+":"+"master500_3_0",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master500_3_1",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master500_3_2",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master500_3_3",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master500_3_4",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master500_3_5"
);



CSC_fwG_g_all_2masters = makeDynString();
*/



/* was up to 03/03/2006
CSC_fwG_g_host=200;
CSC_fwG_g_1host=500;
CSC_fwG_g_2host=200; 

CSC_fwG_g_all_primary=makeDynString("primary500","primary200");

CSC_fwG_g_all_maratons=makeDynString("maraton01","maraton02");
CSC_fwG_g_all_wieners12=makeDynString("wiener1201","wiener1202","wiener1203");

CSC_fwG_g_all_masters = makeDynString(CSC_fwG_g_SYSTEM_NAME+":"+"master500_1_1",//s1
                              CSC_fwG_g_SYSTEM_NAME+":"+"master500_1_15",//s1
                              CSC_fwG_g_SYSTEM_NAME+":"+"master200_0_0",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master200_0_1",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master200_0_2",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master200_0_3",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master200_0_4",
                              CSC_fwG_g_SYSTEM_NAME+":"+"master200_0_5"
);
CSC_fwG_g_all_1masters = makeDynString(CSC_fwG_g_SYSTEM_NAME+":"+"master500_1_1",//s1
                               CSC_fwG_g_SYSTEM_NAME+":"+"master500_1_15"//s1

);

CSC_fwG_g_all_2masters = makeDynString(CSC_fwG_g_SYSTEM_NAME+":"+"master200_0_0",
                               CSC_fwG_g_SYSTEM_NAME+":"+"master200_0_1",
                               CSC_fwG_g_SYSTEM_NAME+":"+"master200_0_2",
                               CSC_fwG_g_SYSTEM_NAME+":"+"master200_0_3",
                               CSC_fwG_g_SYSTEM_NAME+":"+"master200_0_4",
                               CSC_fwG_g_SYSTEM_NAME+":"+"master200_0_5"
);
*/

CSC_fwG_g_SLICE_DPOINTS=makeDynString(	
/////

       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm2r1c08",  //n
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm2r1c09",  //n
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm2r1c10",

       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm3r1c08", //n
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm3r1c09", //n
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm3r1c10",

       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm2r2c15", //n
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm2r2c16", //n
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm2r2c17", //n
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm2r2c18", //n
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm2r2c19",
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm2r2c20",

       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm3r2c15", //n
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm3r2c16", //n
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm3r2c17", //n
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm3r2c18", //n
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm3r2c19",
       CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sm3r2c20");

CSC_fwG_g_HV_LONG_TERM_DPOINTS=makeDynString(

CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r1c11",  //n
CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r1c12",  //n

CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r1c11", //n
CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r1c12", //n
CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r1c13", //n



 // ---------------------------------------------
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r1c01",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r1c02",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r1c03",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r1c06",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r1c07",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r1c17",
//  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r1c18", // commenetd because +3/1/07 uses the same master channel

  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c01",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c02",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c03",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c04",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c05",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c06",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c07",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c08",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c09",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c10",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c11",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c12",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c13",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c33",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c34",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c35",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp2r2c36",

CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r1c01",
//CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r1c02",
//CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r1c07",
//CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r1c04",
//CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r1c06",
CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r1c17",

//CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r1c18" // commenetd because +3/1/02 uses the same master channel

  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c01",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c02",
/*
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c03",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c04",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c05",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c06",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c07",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c08",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c09",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c10",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c11",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c12",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c13",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c14",
*/
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c33",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c34",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c35",
  CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_prefix+"sp3r2c36"

);

}
//--------------------------------------------------
if(project_station_fwUi==""){ // case of mudcsServer or VisualServer(CSC_fwG_g_watch_mask is not used in VisualServer)

CSC_fwG_g_watch_mask=makeDynString("fwGasSystem_CSC_GAS_DimBroker",
                                   "fwCooling_CSC_COOLING_DimBroker",
                "LV_SX5_DimBroker",
//               "LV_1_DimBroker_MINUS","LV_1_DimBroker_PLUS",
               "GAS_SX5_DimBroker","WTH_SX5_DimBroker","PT100_DimBroker", 
               "CRB_1_DimBroker", "MRTN_1_DimBroker", "WNR12_1_DimBroker", "ALNM_1_DimBroker",
               "fwWnrCr_CSC_LV_DimBroker","fwWnrCh_CSC_LV_DimBroker");
////dynAppend(CSC_fwG_g_watch_mask,"primary500"); // should be one entry per each machine: otherwise semaphore problem with watch_for_alive 
for(i=1;i<=dynlen(CSC_fwG_g_all_hosts);i++)dynAppend(CSC_fwG_g_watch_mask,"primary"+CSC_fwG_g_all_hosts[i]);// should be one entry per each machine: otherwise semaphore problem with watch_for_alive 

 dyn_string dyn_debug22;
 dpSetWait("dyn_debug1.",CSC_fwG_g_watch_mask);
 for(i=1; i<= dynlen(CSC_fwG_g_watch_mask); i++){
   if(!dpExists("WATCH_TIME"+i)){
      dpCreate("WATCH_TIME"+i,"MUDCS_TIME");
      
      dpGet("dyn_debug2.",dyn_debug22);
      dynAppend(dyn_debug22,"WATCH_TIME"+i);
      dpSetWait("dyn_debug2.",dyn_debug22);      
   }
 }
 
}

CSC_fwG_g_GAS_ERROR_FLAG=1;
CSC_fwG_g_GAS_WARNING_FLAG=1;

CSC_fwG_g_DISPLAY_SAVED="";

CSC_fwG_g_CURRENT_MASTER_INDEX=1;
CSC_fwG_g_HV_STATUS=makeDynString(	
        "OFF",
	"ON",
	"Ramp_Up",
	"Ramp_Dn",
	"DEAD",
	"ILOCK",
	"Int_Trip",
	"Ch_Trip",
	"oV_trip"  );

CSC_fwG_g_EMU_ALIASES=makeDynString();
CSC_fwG_g_EMU_NAME_ALIASES=makeDynString();

CSC_fwG_g_DEFINE_ELMB=1;

CSC_fwG_g_wheel_labels = makeDynString("sm4","sm3","sm2","sm1","sp1","sp2","sp3","sp4");
CSC_fwG_g_radius_labels = makeDynString("r1","r2","r3"); // fsm: ("inn","out");

//if (os =="Linux")CSC_fwG_g_project_name_home = CSC_fwG_g_HOME+"/csc_station"+project_suffix; // fsm: "/home/fast/pvss_dcs8"; 
//else CSC_fwG_g_project_name_home = CSC_fwG_g_HOME+"\\csc_station"+project_suffix;
//--------------

dpGet("fwInstallation_fwDIM.installationDirectory",CSC_fwG_g_jcop_dim_home);

//--------------
string project_path=getPath("");  // get absolute home path of the project 
project_path=substr(project_path,0,strlen(project_path)-1); // to get rid of the last "/"

if (os =="Linux"){

  CSC_fwG_g_project_name_home = project_path;
  }
else {
  strreplace(project_path,"/","\\");  
  CSC_fwG_g_project_name_home = project_path;

}

CSC_fwG_CSC_fwG_g_radius_labels_short = makeDynString("r1","r2","r3"); // fsm: makeDynString("In","Out");
CSC_fwG_g_WHEELS=makeDynString("ME-4","ME-3","ME-2","ME-1","ME+1","ME+2","ME+3","ME+4");
CSC_fwG_g_RADIUSES=makeDynString("/1/","/2/","/3/"); // fsm: + "/3/"

CSC_fwG_g_IS_CALL_SELECT_HANDLER = true;

CSC_fwG_g_alct_variables= makeDynString(
"v18","v33","v55","v56","c18","c33","c55","c56");
CSC_fwG_g_cfeb_variables= makeDynString(
"v33","v50","v60","c33","c50","c60");


addGlobal("CSC_fwG_g_DEVICE_LIST", DYN_STRING_VAR);

addGlobal("CSC_fwG_g_ACTIVE_DISKS_AND_CHAMBERS", DYN_STRING_VAR);

mudcsGetCscDeviceList(CSC_fwG_g_DEVICE_LIST);

}
//=================================================================================================
//=================================================================================================

mudcsPutCscDeviceList(string device_type){

dyn_string test_list;

 dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.CscDeviceList"), test_list);
 if(dynContains(test_list,device_type) <=0 ){
  dynAppend(test_list,device_type);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.CscDeviceList"), test_list);
 }

}

//=================================================================================================

mudcsGetCscDeviceList(dyn_string &device_list){

 dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.CscDeviceList"), device_list);

}


//=================================================================================================

//  fsm +
// usage: panels/mudcsArchiveConfig.pnl

mudcsGetDeviceList(dyn_string &device_list){

dyn_string BrokerList;
int i, pos;

 dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimBrokerList"), BrokerList);

 dynClear(device_list);

 for(i=1; i<=dynlen(BrokerList); i++){

  pos=strpos(BrokerList[i],"_DimBroker");
  dynAppend(device_list,substr(BrokerList[i],0,pos));

 }

}


//=================================================================================================

// fsm : used +
mudcsDimConfigOneManager(string manager, string pc, dyn_string &exceptionInfo){
//
// configuration of one Dim manager (e.g. _DimClient_1):
// for each item from Db_o.DimBrokerList (e.g. LV_1_DImBroker)
//  1) associate service (LV_1) with dp: LV_1_DImBroker_o
//  2) associate command (LV_1_COMMAND) with dp: LV_1_DImBroker_o.command
// 

  
 int j,i;
 dyn_string BrokerList;
 string service,update_fun;
 string test_string;
 dyn_string test_dyn;
 string pc_id="";


		dpSetWait(mudcsAddSystem(manager+".clientServices:_original.._value"),makeDynString());
		dpSetWait(mudcsAddSystem(manager+".clientCommands:_original.._value"),makeDynString());



 dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimBrokerList"), BrokerList);
 

//-----------------------------------------------------------------------------
    mudcsDimConfig(true,"dyn_debug1"+","+"dummy_service"+
    ",,0,0", manager, exceptionInfo); 
    //if(dynlen(exceptionInfo)>0){return;}
  
    for(j=1; j<= dynlen(BrokerList); j++){
      if(strpos(BrokerList[j],"fwWnrC")>=0)continue; 
      pc_id="";


//-----------------------------------------------------------------------------


//----- dim config for different machine --------------------------------------
       for(i=1;i<=dynlen(CSC_fwG_g_BROKER_DNS_MACHINE_NAMES);i++){
        test_dyn=strsplit(CSC_fwG_g_BROKER_DNS_MACHINE_NAMES[i],";");
        if(test_dyn[2]==pc && (strpos(BrokerList[j],test_dyn[1]) >=0 ||  test_dyn[1] == "all")){pc_id=test_dyn[1]; break;}
       } // for

       if(pc_id==""){continue;}
       if(pc_id != "all" && strpos(BrokerList[j],pc_id) < 0)continue;
//------------------------------------------------------------------------------

       service=substr(BrokerList[j],0,strpos(BrokerList[j],"_DimBroker"));

//------- from now on: Commenting out DimBroker_o datapoints DIM configuration -----------------------       
if(strpos(BrokerList[j],"WTH_SX5") < 0 && strpos(BrokerList[j],"GAS_SX5") >= 0){
     mudcsDimConfig(true,BrokerList[j]+"_o,"+service+
       ",,0,0", manager, exceptionInfo);
	if(dynlen(exceptionInfo)>0){return;}
}

//---------------------------------------------------------------------------------------

///     mudcsDimConfig(false,BrokerList[j]+"_o.command,"+service+"_COMMAND", manager, exceptionInfo);

    if (EMU_G_TEST_DIM_SERVICE) { // no commands on a test run please
      return;
    }

test_string=service+"_COM";

     if(!(dpExists(test_string))){
         dpCreate(test_string,"MUDCS_DIMCOM");
         while(!(dpExists(test_string))){delay(0,50);}
     }

test_string=service+"_COM_CONF";

     if(!(dpExists(test_string))){
         dpCreate(test_string,"MUDCS_DIMCOM");
         while(!(dpExists(test_string))){delay(0,50);}
     }
      if(strpos(manager,"_DimClient_4")<0 || service!="LV_1"){
     mudcsDimConfig(false,service+"_COM"+".command,"+service+"_COMMAND", manager, exceptionInfo);
     mudcsDimConfig(false,service+"_COM_CONF"+".command,"+service+"_COMMAND_CONFIRMATION", manager, exceptionInfo);
      }
	if(dynlen(exceptionInfo)>0){ return;}

    } // for over BrokerList)



}	
//=================================================================================================
// fsm:
// usage: panels/mudcsEmuConfig.pnl
// usage: panels/mudcsEmuOperation.pnl

mudcsDimConfigAllManagers(dyn_string &exceptionInfo){
//
// lools at how many manages we have ( dpPointList=dpNames(CSC_fwG_g_SYSTEM_NAME+":_DimClient_*"); )
// and configures them in loop
//

 dyn_string dpPointList;
 int i;


dyn_string pcs;
string pc;
dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.PCToManID"),pcs);

 dpPointList=dpNames(CSC_fwG_g_SYSTEM_NAME+":_DimClient_*");

//-------

  for(i=1; i <= dynlen(dpPointList); i++ ){
    dpSet(mudcsAddSystem(dpPointList[i]+".clientServices"),makeDynString());
    dpSet(mudcsAddSystem(dpPointList[i]+".clientCommands"),makeDynString());    
    
   pc=pcs[i+1];
   if(pc=="" || i > (dynlen(pcs)-1)) continue;
   mudcsDimConfigOneManager(dpPointList[i],pc,exceptionInfo);
   mudcsDimConfigOneManagerNew("FED_1",pc,dpPointList[i],exceptionInfo);
   mudcsDimConfigOneManagerNew("fwWnrCr_CSC_LV",pc,dpPointList[i],exceptionInfo);
   mudcsDimConfigOneManagerNew("fwWnrCh_CSC_LV",pc,dpPointList[i],exceptionInfo);   
   mudcsDimConfigOneManagerNew("HV_1",pc,dpPointList[i],exceptionInfo);
   mudcsDimConfigOneManagerNew("LV_1",pc,dpPointList[i],exceptionInfo);
   mudcsDimConfigOneManagerNew("TEMP_1",pc,dpPointList[i],exceptionInfo);
   mudcsDimConfigOneManagerNew("CHIP_1",pc,dpPointList[i],exceptionInfo);
   mudcsDimConfigOneManagerNew("WTH_SX5",pc,dpPointList[i],exceptionInfo);
   mudcsDimConfigOneManagerNew("PT100",pc,dpPointList[i],exceptionInfo);
   mudcsDimConfigOneManagerNew("CRB_1",pc,dpPointList[i],exceptionInfo);
   mudcsDimConfigOneManagerNew("MRTN_1",pc,dpPointList[i],exceptionInfo);
   mudcsDimConfigOneManagerNew("WNR12_1",pc,dpPointList[i],exceptionInfo);
   mudcsDimConfigOneManagerNew("HV_PR",pc,dpPointList[i],exceptionInfo);
   mudcsDimConfigOneManagerNew("ALNM_1",pc,dpPointList[i],exceptionInfo);
  }


  dyn_string test;
  for(i=1;i<=dynlen(CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST);i++){
   if(dynlen(CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[i]) == 2)
    dynAppend(test,CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[i][1]+"|"+CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[i][2]);
   else 
    dynAppend(test,CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[i][1]);
  }
  
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"HV_36CHANNEL_BOARDS_CHAMBER_LIST."),test); 
  


}


//=================================================================================================

//fsm: +
// usage: panels/mudcsEmuConfig.pnl
// usage: panels/mudcsEmuOperation.pnl

mudcsAllDimManagersStart(bool restore=false){

//
// starts all managers from CSC_fwG_CSC_fwG_g_SYSTEM_NAME2:Db_o.PCToManID list by 
// /bin/PVSS00dim -num [manager number] ....
// (an item in CSC_fwG_CSC_fwG_g_SYSTEM_NAME2:Db_o.PCToManID list should not be "" to be started)
//

 int i,pos,manId, newId;
 dyn_string dim_names,test_dyn_string,test_dyn_string2,  exceptionInfo;   
 dyn_int connectedApiIds;
 int valueToWrite;
 string test_string, test_string2;
 bool found;
 string info;
 
 dyn_int dim_csc_connections=makeDynInt();
//----

  test_string="~/bin/delete_processes_by_name bin/PVSS00dim";
operating_system=getenv("OS");
  if(operating_system=="Linux")system(test_string);

//  test_string=CSC_fwG_g_project_name_home + "/bin/delete_processes_by_name home/fast/pvss_fsm/bin/LVSS00testdim";
//  system(test_string);
//-----
  info="mudcsDimStart:";

/////////////////////////////////////////////////////////// mudcsInit();

   dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.PCToManID"),test_dyn_string2);
/////   test_dyn_string2[i]=test_dyn_string[i];
/////   dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.PCToManID"),test_dyn_string2);

  for(i=1; i <= dynlen(test_dyn_string2); i++){
   if(test_dyn_string2[i] != "" && test_dyn_string2[i] != "smi"){

	dpGet(mudcsAddSystem("_Connections.Device.ManNums:_original.._value"),connectedApiIds);

	if(dynContains(connectedApiIds, i)<=0)
	{
        
     int pos_part;
     if((pos_part=strpos(test_dyn_string2[i],"_part"))>=0)
       test_dyn_string2[i]=substr(test_dyn_string2[i],0,pos_part);
      
     test_string="_DimClient_"+i;
         test_string2=CSC_fwG_g_jcop_dim_home/*CSC_fwG_g_project_name_home*/ + "/bin/PVSS00dim"+CSC_fwG_g_PVSS_VERSION+" -num "+i+" -dim_dp_config "+test_string+
         " -dim_dns_node "+ test_dyn_string2[i]; 
         if(CSC_fwG_g_FSM_V24R09)test_string2 = "export DIM_DNS_NODE="+test_dyn_string2[i]+";"+test_string2;
operating_system=getenv("OS");

//-------
      bool isManagerEmpty=false;
      dyn_string clientServices, clientCommands;
      dpGet(test_string+".clientServices",clientServices);
      dpGet(test_string+".clientCommands",clientCommands);
      if(dynlen(clientServices) <= 1 /*(dummy service)*/ && dynlen(clientCommands) ==0)isManagerEmpty=true;
//-------  
      if(!isManagerEmpty){
         if(operating_system=="Linux")system(test_string2+" &");
         else system("start /b "+test_string2);
         dynAppend(dim_csc_connections,i);
         mudcsDebug(test_string2);         
      }
 info +=  test_string2 + "\n";
 //mudcsDebug(test_string2);
//// mudcsDebug(""+connectedApiIds[i]+ " "+i);
	}

//////	valueToWrite = convManIdToInt(API_MAN,i);
////	dpSet(mudcsAddSystem("_Managers.Exit:_original.._value"),valueToWrite);


   } // if(test_dyn_string2[i] == "")


  } // for
  //if(!restore)dpSetWait("DIM_CSC_CONNECTIONS.",dim_csc_connections);
//////// mudcsDebug(info);

}

//=================================================================================================

//=================================================================================================

//fsm: +
// usage: panels/mudcsEmuOperation.pnl

mudcsAllDimManagersStop(){

dyn_string test_dyn_string2,exceptionInfo;
int valueToWrite;

   dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.PCToManID"),test_dyn_string2);

  for(int i=1; i <= dynlen(test_dyn_string2); i++){

   if(test_dyn_string2[i] != "" && test_dyn_string2[i] != "smi"){

	valueToWrite = convManIdToInt(DEVICE_MAN,i);
	dpSetWait(mudcsAddSystem("_Managers.Exit:_original.._value"),valueToWrite);
   }

  }

///  mudcsDebug("All DIM managers have been stopped");

}

//=================================================================================================

// fsm: +
// usage: panels/mudcsCscSectionPCs.pnl:     mudcsDimAddRemovePC(true,sPC,exceptionInfo);
// usage: panels/mudcsCscSectionPCs.pnl: mudcsDimAddRemovePC(false,sPC,exceptionInfo);



mudcsDimAddRemovePC(bool isAdd, string PCName, dyn_string &exceptionInfo){

// 
// adds new item (delete item) in the Db_o.PCToManID list.
// To add new item the function at firts looks for "empty"
// element in the list (early deleted PCname). If "" item is found
// so it is filled by PC name and this PC name is associated to
// Dim manager with number of that item.
// If item "" is not found, so the Db_o.PCToManID list is expanded
// and new manager is configured (mudcsDimConfigOneManager) and 
// lanched (/bin/PVSS00dim -num ..... )
//

 int i,pos,manId, newId;
 dyn_string dim_names,test_dyn_string,test_dyn_string2; // fsm: ,  exceptionInfo;   
 dyn_int connectedApiIds;
 int valueToWrite;
 string test_string, test_string2;
 bool found;

///////////////////////////////////////////////////////////////// mudcsInit();
////// mudcsDebug("DIM debug 0"); 


dyn_string pcs;
string pc;
dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.PCToManID"),pcs);

 if(isAdd){

 found = false;


   dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.PCToManID"),test_dyn_string2);
/////   test_dyn_string2[i]=test_dyn_string[i];
/////   dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.PCToManID"),test_dyn_string2);

  for(i=1; i <= dynlen(test_dyn_string2); i++){
   if(test_dyn_string2[i] == ""){

	dpGet(mudcsAddSystem("_Connections.Device.ManNums:_original.._value"),connectedApiIds);

	if(dynContains(connectedApiIds, i)>0)
	{
  if(isFunctionDefined("fwExceptionHandling_display")){  
		fwException_raise(exceptionInfo, "INFO", "Manager ID "+i+" is connected","");
		fwExceptionHandling_display(exceptionInfo);
  }
  else mudcsEmuMessages("Manager ID "+i+" is connected");
		return;
	}

//////	valueToWrite = convManIdToInt(API_MAN,i);
////	dpSet(mudcsAddSystem("_Managers.Exit:_original.._value"),valueToWrite);

     pc=pcs[i];

     test_string="_DimClient_"+i;
     mudcsDimConfigOneManager(test_string,pc,exceptionInfo);
    mudcsDimConfigOneManagerNew("FED_1",pc,test_string,exceptionInfo); 
   mudcsDimConfigOneManagerNew("fwWnrCr_CSC_LV",pc,test_string,exceptionInfo);
   mudcsDimConfigOneManagerNew("fwWnrCh_CSC_LV",pc,test_string,exceptionInfo);     
     mudcsDimConfigOneManagerNew("HV_1",pc,test_string,exceptionInfo);
     mudcsDimConfigOneManagerNew("LV_1",pc,test_string,exceptionInfo);
     mudcsDimConfigOneManagerNew("TEMP_1",pc,test_string,exceptionInfo);
     mudcsDimConfigOneManagerNew("CHIP_1",pc,test_string,exceptionInfo);
     mudcsDimConfigOneManagerNew("WTH_SX5",pc,test_string,exceptionInfo);
     mudcsDimConfigOneManagerNew("PT100",pc,test_string,exceptionInfo);
     mudcsDimConfigOneManagerNew("CRB_1",pc,test_string,exceptionInfo);
     mudcsDimConfigOneManagerNew("MRTN_1",pc,test_string,exceptionInfo);
     mudcsDimConfigOneManagerNew("WNR12_1",pc,test_string,exceptionInfo);
     mudcsDimConfigOneManagerNew("HV_PR",pc,test_string,exceptionInfo);
     mudcsDimConfigOneManagerNew("ALNM_1",pc,test_string,exceptionInfo);

     int pos_part;
     if((pos_part=strpos(PCName,"_part"))>=0)
       PCName=substr(PCName,0,pos_part);
       
         test_string2=CSC_fwG_g_jcop_dim_home/*CSC_fwG_g_project_name_home*/ + "/bin/PVSS00dim"+CSC_fwG_g_PVSS_VERSION+" -num "+i+" -dim_dp_config "+test_string+
         " -dim_dns_node "+ PCName; 
         if(CSC_fwG_g_FSM_V24R09)test_string2 = "export DIM_DNS_NODE="+PCName+";"+test_string2;
operating_system=getenv("OS");
//-------
      bool isManagerEmpty=false;
      dyn_string clientServices, clientCommands;
      dpGet(test_string+".clientServices",clientServices);
      dpGet(test_string+".clientCommands",clientCommands);
      if(dynlen(clientServices) <= 1 /*(dummy service)*/ && dynlen(clientCommands) ==0)isManagerEmpty=true;
//-------  
      if(!isManagerEmpty){
         if(operating_system=="Linux")system(test_string2+" &");
         else system("start /b "+test_string2); 
 mudcsDebug(test_string2);
      }

   test_dyn_string2[i]=PCName;
   dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.PCToManID"),test_dyn_string2);
   return;



   } // if(test_dyn_string2[i] == "")


  } // for








      newId=dynlen(test_dyn_string2)+1;
        test_string="_DimClient_"+newId;
     if(!(dpExists(CSC_fwG_g_SYSTEM_NAME+":"+test_string))){
         dpCreate(test_string,"_FwDimConfig");
         while(!(dpExists(CSC_fwG_g_SYSTEM_NAME+":"+test_string))){delay(0,50);}
     }


     pc=pcs[newId];

     mudcsDimConfigOneManager(test_string,pc,exceptionInfo);
   mudcsDimConfigOneManagerNew("FED_1",pc,test_string,exceptionInfo);     
   mudcsDimConfigOneManagerNew("fwWnrCr_CSC_LV",pc,test_string,exceptionInfo);
   mudcsDimConfigOneManagerNew("fwWnrCh_CSC_LV",pc,test_string,exceptionInfo);          
    mudcsDimConfigOneManagerNew("HV_1",pc,test_string,exceptionInfo);
    mudcsDimConfigOneManagerNew("LV_1",pc,test_string,exceptionInfo);
   mudcsDimConfigOneManagerNew("TEMP_1",pc,test_string,exceptionInfo);
   mudcsDimConfigOneManagerNew("CHIP_1",pc,test_string,exceptionInfo);
   mudcsDimConfigOneManagerNew("WTH_SX5",pc,test_string,exceptionInfo);
   mudcsDimConfigOneManagerNew("PT100",pc,test_string,exceptionInfo);
   mudcsDimConfigOneManagerNew("CRB_1",pc,test_string,exceptionInfo);
   mudcsDimConfigOneManagerNew("MRTN_1",pc,test_string,exceptionInfo);
   mudcsDimConfigOneManagerNew("WNR12_1",pc,test_string,exceptionInfo);
   mudcsDimConfigOneManagerNew("HV_PR",pc,test_string,exceptionInfo);
   mudcsDimConfigOneManagerNew("ALNM_1",pc,test_string,exceptionInfo);

	dpGet(mudcsAddSystem("_Connections.Device.ManNums:_original.._value"),connectedApiIds);

	if(dynContains(connectedApiIds, newId)>0)
	{
  if(isFunctionDefined("fwExceptionHandling_display")){ 
		fwException_raise(exceptionInfo, "INFO", "Manager ID "+newId+" is connected","");
		fwExceptionHandling_display(exceptionInfo);
  }
  else mudcsEmuMessages("Manager ID "+newId+" is connected");
		return;
  }

       int pos_part;
       if((pos_part=strpos(PCName,"_part"))>=0)
       PCName=substr(PCName,0,pos_part);
             
         test_string2=CSC_fwG_g_jcop_dim_home/*CSC_fwG_g_project_name_home*/ + "/bin/PVSS00dim"+CSC_fwG_g_PVSS_VERSION+" -num "+newId+" -dim_dp_config "+test_string+
         " -dim_dns_node "+PCName; 
         if(CSC_fwG_g_FSM_V24R09)test_string2 = "export DIM_DNS_NODE="+PCName+";"+test_string2;
operating_system=getenv("OS");
//-------
      bool isManagerEmpty=false;
      dyn_string clientServices, clientCommands;
      dpGet(test_string+".clientServices",clientServices);
      dpGet(test_string+".clientCommands",clientCommands);
      if(dynlen(clientServices) <= 1 /*(dummy service)*/ && dynlen(clientCommands) ==0)isManagerEmpty=true;
//-------  
      if(!isManagerEmpty){
         if(operating_system=="Linux")system(test_string2+" &");
         else system("start /b "+test_string2); 
 mudcsDebug(test_string2);
      }
   test_dyn_string2[newId]=PCName;
   dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.PCToManID"),test_dyn_string2);	

 mudcsDebug("DIM debug add 2");

 } // if(isAdd)
 else{ //-------------------------------------------------- i.e. delete


   dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.PCToManID"),test_dyn_string2);
/////   test_dyn_string2[i]=test_dyn_string[i];
/////   dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.PCToManID"),test_dyn_string2);
/////mudcsDebug("STOP1 "+dynlen(test_dyn_string2)+" "+PCName);

  for(i=1; i <= dynlen(test_dyn_string2); i++){
////mudcsDebug("STOP2 "+test_dyn_string2[i]);
   if(test_dyn_string2[i] == PCName){

////mudcsDebug("STOP2 ");
	dpGet(mudcsAddSystem("_Connections.Device.ManNums:_original.._value"),connectedApiIds);
	if(dynContains(connectedApiIds, i)<=0)
	{
  if(isFunctionDefined("fwExceptionHandling_display")){  
		fwException_raise(exceptionInfo, "INFO", "Manager ID "+i+" is not connected","");
		fwExceptionHandling_display(exceptionInfo);
  }
  else mudcsEmuMessages("Manager ID "+i+" is not connected");
//		return;
	}
        else {

	valueToWrite = convManIdToInt(DEVICE_MAN,i);
/////mudcsDebug("DEVICE_MAN1 "+i);
	dpSetWait(mudcsAddSystem("_Managers.Exit:_original.._value"),valueToWrite);
/////mudcsDebug("DEVICE_MAN2 "+DEVICE_MAN);
/////mudcsDebug("DEVICE_MAN3 "+valueToWrite);
        }

   test_dyn_string2[i]="";
   dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.PCToManID"),test_dyn_string2);


   } // if(test_dyn_string2[i] == PCName)

  } // for







 } // else if(isAdd)



} // end of mudcsDim

//============================================================================================
// fsm: +
// usage in : mudcsDimConfigOneManager()

mudcsDimConfig(bool isService, string srvCmd, string managerName, dyn_string &exceptionInfo)
{

// -- the function is called in mudcsDimConfigOneManager
// -- the srvCmd parameter is line like:  LV_1_DImBroker_o,LV_1,,0,0
//
// This function does the following:
//
// configuration of one service or command of the
// one Dim manager with name managerName (e.g. _DimClient_1):
// by
//  1) association service (LV_1) with dp: LV_1_DImBroker_o  (for service)
//  2) association command (LV_1_COMMAND) with dp: LV_1_DImBroker_o.command (for command)
//  


/////	string srvCmd, managerName;
	dyn_string list;/////, exceptionInfo;
	int pos;
        int list_len;

        if(isService)list_len=5;
        else list_len=2;
	
////	srvCmd = serviceCommand.text();
////	if(srvCmd == "")
////		return;
	
////	managerName = dimConfig.text();
////	if(!dpExists(managerName)) {
////		result.text()="DP not existing";
////		return;
////	}

	pos = strpos(srvCmd, " ");
	
	if(pos >=0) {
		fwException_raise(exceptionInfo, "ERROR", "fwModifyDpClientConfig.pnl: the DIM name must not have ' ' character", "");
                return;
	}
	list = strsplit(srvCmd,",");
	if(dynlen(list) == list_len) {
		if((list[1] == "") || (list[2] == "")){
			fwException_raise(exceptionInfo, "ERROR", "fwModifyDpClientConfig.pnl: wrong dp or service or command", "");
                        return;
                }
	}
	else{
		fwException_raise(exceptionInfo, "ERROR", "fwModifyDpClientConfig.pnl: the DIM format is wrong", "");
                return;
        }

//////////	if(dynlen(exceptionInfo) != 0) {
//////////		fwExceptionHandling_display(exceptionInfo);
/////////		return;
/////////	}
/////	this.enabled = false;
	if(managerName != "") {
//////fwExceptionHandling_display(list);

             if(isService)
		dpGet(mudcsAddSystem(managerName+".clientServices:_online.._value"),list);
             else
		dpGet(mudcsAddSystem(managerName+".clientCommands:_online.._value"),list);

//             if(strpos(srvCmd,"CSC_ME_P")<0 && strpos(srvCmd,"master")<0 ) 
                if(dynContains(list,srvCmd) < 1)dynAppend(list,srvCmd);             
             
//DebugN(managerName,list);
             if(isService)
		dpSet(mudcsAddSystem(managerName+".clientServices:_original.._value"),list);
             else
              {dpSet(mudcsAddSystem(managerName+".clientCommands:_original.._value"),list);}

/////		serviceList.items() = list;
/////		resultAdd.text()="Service added";
	}
/////	this.enabled = true;



}
//============================================================================================
//============================================================================================

mudcsDpSetFLoat(string dp, float value){

string error_message;

 if(!(dpExists(dp))){
      error_message = "The dpoint: "+dp+ " does not exist";
      mudcsDebug(error_message);
 }
 else{
  dpSet(mudcsAddSystem(dp),value);
 }

}

//============================================================================================
mudcsDpSetInt(string dp, int value){

string error_message;

 if(!(dpExists(dp))){
      error_message = "The dpoint: "+dp+ " does not exist";
      mudcsDebug(error_message);
 }
 else{
  dpSet(mudcsAddSystem(dp),value);
 }

}

//============================================================================================

mudcsDpSetString(string dp, string value){

string error_message;

 if(!(dpExists(dp))){
      error_message = "The dpoint: "+dp+ " does not exist";
      mudcsDebug(error_message);
 }
 else{
  dpSet(mudcsAddSystem(dp),value);
 }

}

//============================================================================================
//============================================================================================
mudcsDpGetFloat(string dp, float &value){

string error_message;

 if(!(dpExists(dp))){
      error_message = "The dpoint: "+dp+ " does not exist";
      mudcsDebug(error_message);
 }
 else{
  dpGet(mudcsAddSystem(dp),value);
 }

}

//============================================================================================

mudcsDpGetInt(string dp, int &value){

string error_message;

 if(!(dpExists(dp))){
      error_message = "The dpoint: "+dp+ " does not exist";
      mudcsDebug(error_message);
 }
 else{
  dpGet(mudcsAddSystem(dp),value);
 }

}

//============================================================================================

mudcsDpGetString(string dp, string &value){

string error_message;

 if(!(dpExists(dp))){
      error_message = "The dpoint: "+dp+ " does not exist";
      mudcsDebug(error_message);
 }
 else{
  dpGet(mudcsAddSystem(dp),value);
 }

}

//============================================================================================
//============================================================================================

mudcsDebug(string string_to_debug){
  
dyn_string test_dyn_string;

if(isFunctionDefined("fwExceptionHandling_display")){
  DebugTN("defined!");
 fwException_raise(test_dyn_string, "MYDEBUG", string_to_debug, "");
 fwExceptionHandling_display(test_dyn_string);
}
else mudcsEmuMessages(string_to_debug);

}
//============================================================================================
mudcsDebug2(string string_to_debug){

dyn_string test_dyn_string;

if(isFunctionDefined("fwExceptionHandling_display")){
 fwException_raise(test_dyn_string, "MYDEBUG", string_to_debug, "");
 mudcsExceptionHandling_display(test_dyn_string);
}
else mudcsEmuMessages(string_to_debug);

}
//============================================================================================
// this function is actually copied from fwExceptionHandling.ctl fwExceptionHandling_display : panel called is different: mudcsExceptionDisplay.pnl
mudcsExceptionHandling_display(dyn_string &exceptionInfo)
{
	int number = 1;

	if(dynlen(exceptionInfo) <= 0)
		return;

	while(isPanelOpen("Exception Details #" + number))
	{
		//DebugN("fwExceptionHandling_display(): Number used " + number);
		number++;
	}

	/*ModuleOn("Exception Details #" + number, 100, 100, 100, 100, 1, 1, 1, "");

	RootPanelOnModule("fwExceptionDisplay.pnl",
					  "",
					  "Exception Details #" + number,
					  makeDynString("$asExceptionInfo:" + exceptionInfo, 
					  				"$iListIndex:1"));*/
					  				
	ChildPanelOnCentralModal(	CSC_fwG_g_Component+"/mudcsExceptionDisplay.pnl", 
								"Exception Details #" + number,
								makeDynString(	"$asExceptionInfo:" + exceptionInfo, 
								  				"$iListIndex:1"));

	exceptionInfo = makeDynString();
}

//============================================================================================
/*
mudcsUpdateMonitor_HV_1(string dpName, string value){


HV_1_DimBroker value;
int ret;

ret=dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":HV_1_DimBroker_o"), value);




}
*/

//============================================================================================

mudcsCopy(string dpNameSource, string dpNameDestin){

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




  elemListS=dpNames(dpNameSource+".*");
  elemListD=dpNames(dpNameDestin+".*");

/////////////////mudcsDebug("Copy-----"+dpNameSource);

  for(i=1;i<=dynlen(elemListS);i++){
     typ=dpElementType(elemListS[i]);
     switch(typ){
      case DPEL_BOOL:
       dpGet(mudcsAddSystem(elemListS[i]),bool_el);
       dpSet(mudcsAddSystem(elemListD[i]),bool_el);
      break;
      case DPEL_INT:
       dpGet(mudcsAddSystem(elemListS[i]),int_el);
       dpSet(mudcsAddSystem(elemListD[i]),int_el);
      break;
      case DPEL_FLOAT:
       dpGet(mudcsAddSystem(elemListS[i]),float_el);
    //temporal !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
////mudcsDebug(elemListS[i]);
////mudcsDebug(elemListD[i]);
////       float_el=10;
       dpSet(mudcsAddSystem(elemListD[i]),float_el);
      break;
      case DPEL_STRING:
       dpGet(mudcsAddSystem(elemListS[i]),string_el);
       dpSet(mudcsAddSystem(elemListD[i]),string_el);
      break;

      case DPEL_DYN_BOOL:
       dpGet(mudcsAddSystem(elemListS[i]),bool_ar);
       dpSet(mudcsAddSystem(elemListD[i]),bool_ar);
      break;
      case DPEL_DYN_INT:
       dpGet(mudcsAddSystem(elemListS[i]),int_ar);
       dpSet(mudcsAddSystem(elemListD[i]),int_ar);
      break;
      case DPEL_DYN_FLOAT:
       dpGet(mudcsAddSystem(elemListS[i]),float_ar);
    //temporal !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///       float_ar[1]=10;
///       float_ar[2]=11;
       dpSet(mudcsAddSystem(elemListD[i]),float_ar);
      break;
      case DPEL_DYN_STRING:
       dpGet(mudcsAddSystem(elemListS[i]),string_ar);
       dpSet(mudcsAddSystem(elemListD[i]),string_ar);
      break;



     }// switch

  } //for


}

//============================================================================================
mudcsTest(){


}
//============================================================================================
//============================================================================================
//============================================================================================
// fsm: is it needed now?

mudcsPosToLabelConvert(int w_pos, int radius, int ich, string device_type, string &label){


string before_ich;

if(ich<=9)  before_ich="0";
else before_ich="";

 label = "Disk"+ CSC_fwG_g_wheel_labels[w_pos]+ CSC_fwG_g_radius_labels[radius] + before_ich + ich + device_type+ "|";


}




















//============================================================================================
// 10/17/2004


mudcsGetCoordinatesByDpName(string dp_name,string coordinates){

int i,j, pos;

string mudcs_alias;
int w_pos,radius,ich;
string type;
dyn_string list;


coordinates="";

 if(dp_name != "all;all"){

 mudcs_alias=mudcsAliasNameGet(dp_name);
mudcsNameToPosConvert(mudcs_alias/*dp_name*/, w_pos, radius, ich, type);

  if(dynContains(CSC_fwG_g_DEVICE_LIST,type)){

 if((w_pos==4 || w_pos==5) && radius == 1){ // i.e. ME 1/1
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Ring_DUBNA.DynatemAndSetNumberList"),list);
 }
 else{
    if(((w_pos==4 || w_pos==5) && radius==2 ) || radius==1)
      dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel"+w_pos+".InDynatemAndSetNumberList"),list);
    else 
      dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel"+w_pos+".OutDynatemAndSetNumberList"),list);

 } // else
 

//--------
 if((w_pos==4 || w_pos==5) && radius == 1){ // i.e. ME 1/1
    if((w_pos-4)*36+ich  > dynlen(list) ||  list[(w_pos-4)*36+ich] == "") return;
      coordinates=list[(w_pos-4)*36+ich];
 }
 else{
    if(ich  > dynlen(list) ||  list[ich] == "") return;
      coordinates=list[ich];
 }
//--------

  } //if(dynContains(CSC_fwG_g_DEVICE_LIST,type))

  }// not all;all


}
//============================================================================================
mudcsGasCoolingCommand(string type_par, string dp_name,string subcommand){
  if(type_par=="GASSYSTEM"){
    
  }
  else if(type_par=="COOLING"){
    
  }
  
}
//============================================================================================
mudcsSnmpCommand(string type_par, string dp_name,string subcommand){

  int i;
  string data, data_save;
  
  mudcsConvertFsm2DataOne(dp_name, data);
  data_save=data;
  data=mudcsAddSystem(data)+"_COM.command";
  
  
//------------------------------------------------------    
  if(type_par=="fwWnrCr_CSC_LV"){ // crate
    
//--------------------------------------------------------
dyn_string ch_dps,ch_dps_fsm;
    
dyn_string s_split=strsplit(data_save,":");
dyn_string s_split2=strsplit(s_split[dynlen(s_split)],"/");
ch_dps=dpNames("*"+s_split2[dynlen(s_split2)]+"*","fwWnrCh_CSC_LV_d");
//DebugTN(ch_dps);
mudcsConvertData2Fsm("fwWnrCh_CSC_LV",ch_dps,ch_dps_fsm);

//--------------------------------------------------------    
    
    //if(subcommand=="power_on"){
    //dpSetWait(data,"SET_MAIN_ON");
    //delay(3);
    //dpSetWait(data,"SET_GROUP_SWITCH_ON=3"); 
    //}
    if(subcommand=="power_on" || subcommand=="power_on_all"){
    dpSetWait(data,"SET_MAIN_ON");
    delay(3);
    dpSetWait(data,"SET_ALL_CHANS_ON"); 
    delay(3);
    for(i=1;i<=dynlen(ch_dps);i++)dpSet(ch_dps_fsm[i]+".status",2);
    delay(3);     
    }    
    else if(subcommand=="power_off"){    
//    dpSetWait(data,"SET_GROUP_SWITCH_OFF=3"); 
    dpSetWait(data,"SET_ALL_CHANS_OFF");
    delay(3);
    dpSetWait(data,"SET_MAIN_OFF");
    delay(3);    
    for(i=1;i<=dynlen(ch_dps);i++)dpSet(ch_dps_fsm[i]+".status",0);
    delay(3);  
    }
    else if(subcommand=="get_data"){
    dpSetWait(data,"GET_DATA");   
    }    
    else{
     dpSetWait(data,subcommand); 
     // where subcommand: SET_CHAN_OUTPUT_VOLT=x.x
     //"SET_ALL_CHANS_ON" 
     //?SET_ALL_CHANS_OFF? 
     //?SET_GROUP_SWITCH_ON=x? 
     //?SET_GROUP_SWITCH_OFF=x? 
     //

    }    

  } //--------------------------------------------------
  else if(type_par=="fwWnrCh_CSC_LV"){ // channel
    if(subcommand=="power_on"){
  /////////  dpSetWait(data,"SET_CHAN_ON"); 
    }
    else if(subcommand=="power_off"){
  /////////  dpSetWait(data,"SET_CHAN_OFF");       
    }
    else if(subcommand=="get_data"){
    dpSetWait(data,"GET_DATA");   
    }     
    else{
     dpSetWait(data,subcommand); 
     // where subcommand: SET_CHAN_OUTPUT_VOLT=x.x
     //"SET_CHAN_CURRENT_LIMIT=x.x" 
     //"SET_CHAN_MIN_SENSE_VOLT=x.x" 
     //"SET_CHAN_MAX_SENSE_VOLT=x.x" 
     //"SET_CHAN_MAX_TERMINAL_VOLT=x.x" 
     //"SET_CHAN_MAX_CURRENT=x.x" 
     //"SET_CHAN_MAX_POWER=x.x" 
     //"SET_CHAN_GROUP=x" 
     //

    }
  }
  

//------------------------------------------------------  
}
//============================================================================================
// fsm:
// usage: panels/mudcsCscCHIP_1OpBrowser.pnl
// usage: panels/mudcsCscLV_1OpBrowser.pnl

mudcsCommand(string type_par, string dp_name,string subcommand){
//---------------------------

string mudcs_alias;
dyn_string subcommand_split;
subcommand_split=strsplit(subcommand,"|");
subcommand=subcommand_split[1]; // mostly case of set_data

//----------------------------
    dyn_string dyn_debug;

int i,j, pos;

int last_vset;
string DpNameFsm;

int w_pos,radius,ich;
string type;
string address,comp_subcommand;
dyn_string list;
dyn_string test_list, command_list;

string ip_address;

string setList;
string set, set_save;
if(type_par=="LV")setList="DynatemAndSetNumberList";
else if(type_par=="HV")setList="HVsetList";
else if(type_par=="CRB")setList="DiskLevelDevicesCoordinates";
else if(type_par=="MRTN")setList="CscLevelDevicesCoordinates";
else if(type_par=="WNR12")setList="CscLevelDevicesCoordinates";
else if(type_par=="ALNM")setList="DiskLevelDevicesCoordinates";
else if(type_par=="FED")setList="CoordFromDpName";

 dynClear(command_list);

////////////// test_list=dpNames(type_par+"*","MUDCS_DIMCOM");
test_list=dpNames(CSC_fwG_g_SYSTEM_NAME+":"+type_par+"*_COM");

 for(i=1;i<=dynlen(test_list);i++){
   pos=strpos(test_list[i],"_CONF");
   if(pos < 0){
      dynAppend(command_list, test_list[i]);
   }
 }

 if(dp_name != "all;all"){

 mudcs_alias=mudcsAliasNameGet(dp_name);
mudcsNameToPosConvert(mudcs_alias/*dp_name*/, w_pos, radius, ich, type);

/*
   dyn_string d_s2;
if(strpos(dp_name,"ALNM")>=0){

    dpGet(mudcsAddSystem("dyn_debug2."),d_s2);
    dynAppend(d_s2,dp_name+">>>"+mudcs_alias+">>>"+type);
    dpSetWait(mudcsAddSystem("dyn_debug2."),d_s2);
}
*/

for(int i=1;i<=dynlen(CSC_fwG_g_DEVICE_LIST);i++){
//  DebugN("mudcsCommand: "+CSC_fwG_g_DEVICE_LIST[i]+">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>11");
  if(strpos(dp_name,CSC_fwG_g_DEVICE_LIST[i])>=0 && strpos(CSC_fwG_g_DEVICE_LIST[i],type_par)>=0)type=CSC_fwG_g_DEVICE_LIST[i];
}
DebugN("mudcsCommand:"+type+">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>1 "+dp_name +" "+subcommand);
//DebugN(type+">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>2 ");

//DebugN(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>??????????????????????????????????????????????????????1234"+type_par);
  if(dynContains(CSC_fwG_g_DEVICE_LIST,type)){

retrieveCoordinateSet(type_par, dp_name, setList, w_pos, radius, ich, type, set);

/*
if(strpos(dp_name,"ALNM")>=0){
    dpGet(mudcsAddSystem("dyn_debug2."),d_s2);
    dynAppend(d_s2,dp_name+"111>>>"+set);
    dpSetWait(mudcsAddSystem("dyn_debug2."),d_s2);
}
*/

//DebugN(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>??????????????????????????????????????????????????????1"+type_par+">>"+setList+">>"+set+">>"
//  +w_pos+">>"+radius+">>"+ich+">>"+type);
if(set=="")return;
//DebugN(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>??????????????????????????????????????????????????????2"+type_par+" "+setList+" "+set);
//--------

   string host,inhostcoordinates,cmd,cmd_data, chamber_depend_all_channels;
   dyn_int off_channels;
   int ichan; 
  
   dyn_string coords=strsplit(set,";");   

    if(strpos(dp_name,".status") >=0 )DpNameFsm=substr(dp_name,0,strpos(dp_name,".status"));
    if(strpos(dp_name,".") >=0 )DpNameFsm=substr(dp_name,0,strpos(dp_name,"."));
    else DpNameFsm=substr(dp_name,0);

//---------------------------------------------------------------------------------------------------------------------------
   if(type_par=="HV"){
//    DebugN(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>??????????????????????????????????????????????????????  "+set +" "+dp_name);

    set=coords[1];
    if(coords[4]=="0")chamber_depend_all_channels="255;";
    else if(coords[4]=="1")chamber_depend_all_channels="253;";
    else if(coords[4]=="2")chamber_depend_all_channels="254;";


    if(subcommand=="power_on"){ subcommand=      "HVCMD;"+coords[2]+";"+coords[3]+";"+"0;"+"30;"+"1;"+"-1";
                                //////mudcs_master_check_voltage(dp_name);



    dpGet(mudcsAddSystem(DpNameFsm+".off_channels"),off_channels);

                                subcommand=subcommand  +"|"+   "HVCMD;"+coords[2]+";"+coords[3]+";"+chamber_depend_all_channels+"1;"+"0;"+"-1";
                                // DpNameFsm=substr(dp_name,0,strpos(dp_name,".status"));
                                
                                
                                dpGet(mudcsAddSystem(DpNameFsm+".last_vset"),last_vset);
//                            subcommand=subcommand  +"|"+   "HVCMD;"+coords[2]+";"+coords[3]+";"+chamber_depend_all_channels+"7;"+last_vset+";"+"-1";// vset
                            
                                int chCount = 18;
                                if (coords[4] == "0") {
                                  chCount = 30;
                                }
                                int chIdxOffset = 0;
                                if (coords[4] == "2") {
                                  chIdxOffset = 18;
                                }

                                dyn_int vsets;
                                dpGet(mudcsAddSystem(DpNameFsm+".on_ch_vsets"), vsets);
                                if (dynlen(vsets) == 0) { // if vsets have not been initialized yet, set everybody to 3600
                                  for (int i=1; i <= chCount; i++) {
                                    dynAppend(vsets, 3600);
                                    dpSet(mudcsAddSystem(DpNameFsm+".on_ch_vsets"), vsets);
                                  }
                                }
                                
                                for (int i=0; i < chCount; i++) {
                                  int vset = vsets[i + 1];
                                  if (last_vset < 3400) { // if chamber-wide vset is not nominal - lower than 3400V - (e.g. during standby), set it to the chamber-wide vset
                                    vset = last_vset;
                                  }
                                  subcommand=subcommand  +"|"+   "HVCMD;"+coords[2]+";"+coords[3]+";"+ (i + chIdxOffset) +";7;"+ vset +";"+"-1";// vset
                                }                            
                            
                            subcommand=subcommand  +"|"+   "HVCMD;"+coords[2]+";"+coords[3]+";"+chamber_depend_all_channels+"3;"+"11"+";"+"-1"; // ramp_up
                            /*if(CSC_fwG_g_IS_IMAX_SET)*/ subcommand=subcommand  +"|"+   "HVCMD;"+coords[2]+";"+coords[3]+";"+chamber_depend_all_channels+"6;"+"5"+";"+"-1"; //imax
                            subcommand=subcommand  +"|"+   "HVCMD;"+coords[2]+";"+coords[3]+";"+chamber_depend_all_channels+"38;"+"500"+";"+"-1"; //trip_dl
                            mudcsHVMasterChannelSwitch(dp_name, true);

                            for(i=1;i<=dynlen(off_channels);i++){ // switching of the channels to be excluded according to db
                             ichan=off_channels[i]-1;                              
                             subcommand=subcommand  +"|"+   "HVCMD;"+coords[2]+";"+coords[3]+";"+ichan+";0;"+"0;"+"-1";
                            }
                            
//                            mapping chamber = emu_fsmNodeToDeviceParams(DpNameFsm, exceptionInfo);
//                            emu_checkException(exceptionInfo);
//                            emuhv_checkChamberSpecialChannelsVset(chamber, exceptionInfo);
//                            emu_checkException(exceptionInfo);

/*
    dpGet(mudcsAddSystem("dyn_debug2."),dyn_debug);
    dynAppend(dyn_debug,dp_name+" "+subcommand);
    dpSetWait(mudcsAddSystem("dyn_debug2."),dyn_debug);
*/




    }
    else if(subcommand=="power_off"){
                                    if(chamber_depend_all_channels == "255;"){
                                    subcommand=      "HVCMD;"+coords[2]+";"+coords[3]+";"+"0;"+"30;"+"0;"+"-1";
                                    subcommand=subcommand   +"|";
                                    }else subcommand="";
                                    
                                    subcommand=subcommand   +  "HVCMD;"+coords[2]+";"+coords[3]+";"+chamber_depend_all_channels+"0;"+"0;"+"-1";
                                    mudcsHVMasterChannelSwitch(dp_name, false);

/*
    dpGet(mudcsAddSystem("dyn_debug2."),dyn_debug);
    dynAppend(dyn_debug,dp_name+" "+subcommand);
    dpSetWait(mudcsAddSystem("dyn_debug2."),dyn_debug);
*/

    }
    else if(subcommand=="get_data")subcommand= "HVDATA;"+coords[2]+";"+coords[3]+";"+"255;"+"0;"+"0;"+"-1";
   }
//---------------------------------------------------------------------------------------------------------------------------
   else{ // i.e not the HV

    set_save=set;
    //----------------
    if(subcommand=="power_on"){
     dpGet(mudcsAddSystem(DpNameFsm+".off_channels"),off_channels);
     if(type == "ALNM_1"){ 
      set=set+";"+subcommand_split[2];
     }
     else if(type != "LV_1"){
      if(dynlen(subcommand_split)>=2){ // special case: switch a single channel
       set=set  +";"+subcommand_split[2]; // where set is like ELMB;3f and subcommand_split[2] is channel 
      }
      else{  
       for(i=1;i<=dynlen(off_channels);i++){ // switching off the channels to be excluded according to db
         ichan=off_channels[i]-1;
         set=set  +"|"+  set_save+";"+ichan;
         subcommand=subcommand  +"|"+"power_off";
       } //for
      } // else
     }
     else{ // == LV_1
      int ichan_lv=0;
      for(i=1;i<=6;i++){ // switching off the channels to be excluded according to db
         if(!dynContains(off_channels,i)){
           ichan=i-1;
           ichan_lv += (1 << ichan);
         }
      }
      set=set +";"+ichan_lv;
     } // LV_1
    }
    //----------------
    else  if(subcommand=="power_off"){
     if(type == "ALNM_1"){ 
      set=set+";"+subcommand_split[2];
     }
     else if(type != "LV_1"){
       if(dynlen(subcommand_split)>=2){ // special case: switch a single channel
        set=set  +";"+subcommand_split[2]; // where set is like ELMB;3f and subcommand_split[2] is channel 
       } 
       // if subcommand_split < 2 -- so not define anything : just go to JUST_SINGLE_SET below
     }      
    } 
    //----------------
    else if(subcommand=="set_data"){
     set=set+";"+subcommand_split[2];
    } // else
   }
//---------------------------------------------------------------------------------------------------------------------------
//======================================================================================================
//============= THE REAL EXECUTION OF COMMANDS IS BELOW:
//============= BEFORE: THERE WAS PREPARATION OF SUBCOMMANDS AND SETS 
//======================================================================================================
   
dyn_string subcommands;
dyn_string sets;

  
///DebugN(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>??????????????????????????????????????????????????????3"+type_par);
   for(i=1;i<=dynlen(command_list);i++){

///DebugN(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>??????????????????????????????????????????????????????4"+type_par);
    if(type_par=="HV" && strpos(command_list[i],"HV_1") <0)continue;
    else if(type_par=="LV" && (strpos(command_list[i],"LV_1") <0 && strpos(command_list[i],"LV_SX5") <0))continue;
    else if(type_par=="CRB" && (strpos(command_list[i],"CRB_1") <0))continue;
    else if(type_par=="ALNM" && (strpos(command_list[i],"ALNM_1") <0))continue;
 
//if(ich  <= dynlen(list) &&  list[ich] != "")
//DebugN("mudcsCommand:>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> "+command_list[i]+".command"+list[ich]+"|"+subcommand);
 
   if(strpos(subcommand,"|") >=0 && strpos(set,"|") < 0){
    subcommands=strsplit(subcommand,"|");
     for(j=1;j<=dynlen(subcommands);j++){

/*
    dpGet(mudcsAddSystem("dyn_debug1."),dyn_debug);
    dynAppend(dyn_debug,"3333"+mudcsAddSystem(command_list[i]+".command")+set+"|"+subcommands[j]);
    dpSet(mudcsAddSystem("dyn_debug1."),dyn_debug);
*/

      dpSetWait(mudcsAddSystem(command_list[i]+".command"),set+"|"+subcommands[j]);
      //dpSetWait(mudcsAddSystem(command_list[i]+".command"),set+"|"+subcommands[2]);

/*
if(strpos(dp_name,"dp2r1c01HV_1") >=0 ){
    dyn_string dyn_debug;
    dpGet(mudcsAddSystem("dyn_debug2."),dyn_debug);
    dynAppend(dyn_debug,subcommands[j]);
    dpSet(mudcsAddSystem("dyn_debug2."),dyn_debug);
        dynAppend(dyn_debug,"_____"+dp_name+"____"+DpNameFsm);
}
*/
     }

/*
    dyn_string dyn_debug;
    dpGet(mudcsAddSystem("dyn_debug1."),dyn_debug);
    dynAppend(dyn_debug,subcommand);
    dpSet(mudcsAddSystem("dyn_debug1."),dyn_debug);
*/

   }
   else if(strpos(subcommand,"|") >=0 &&  strpos(set,"|") >=0){
     sets=strsplit(set,"|");
     subcommands=strsplit(subcommand,"|");
     for(j=1;j<=dynlen(sets);j++){

/*
    dpGet(mudcsAddSystem("dyn_debug1."),dyn_debug);
    dynAppend(dyn_debug,"2222"+mudcsAddSystem(command_list[i]+".command")+sets[j]+"|"+subcommands[j]);
    dpSet(mudcsAddSystem("dyn_debug1."),dyn_debug);
*/
      dpSetWait(mudcsAddSystem(command_list[i]+".command"),sets[j]+"|"+subcommands[j]);
     }
   } 
   else { // JUST_SINGLE_SET

/* 
    dpGet(mudcsAddSystem("dyn_debug1."),dyn_debug);
    dynAppend(dyn_debug,"1111"+mudcsAddSystem(command_list[i]+".command")+set+"|"+subcommand);
    dpSet(mudcsAddSystem("dyn_debug1."),dyn_debug);
*/

      dpSetWait(mudcsAddSystem(command_list[i]+".command"),set+"|"+subcommand);
   }

   } // for

  } //if(dynContains(CSC_fwG_g_DEVICE_LIST,type))

  }// not all;all
  else {
   for(i=1;i<=dynlen(command_list);i++){  
    dpSetWait(mudcsAddSystem(command_list[i]+".command"),"all;all"+"|"+subcommand);
   }
  }
}
//============================================================================================



mudcsHVMasterChannelSwitch(string DpNameFsmStatus, bool isOn)
{

int current_status;
int status2;
int i,j,index1, index2;
string dp2;

int master_id, master_chan, master_index;
dyn_string coord_master;

string DpNameFsm=substr(DpNameFsmStatus,0,strpos(DpNameFsmStatus,".status"));


 mudcsGetMasterForChamber(DpNameFsm, master_id, 
                         master_chan, master_index, coord_master);

DebugTN("coord_master");
DebugTN(coord_master);

if(master_id==0)return;

 dpGet(mudcsAddSystem(DpNameFsm+".status"),current_status);

if(current_status != -2){

  if(isOn){ // switch on
    mudcsMasterChannelSwitch(1, coord_master, master_chan, DpNameFsm);
  } else {
  
    string fellowChamberDp = emuhv_getFellowChamber(DpNameFsm);
    int fellowChamberStatus = -99; // not 0 or any other valid status
    if (fellowChamberDp != "") {
      dpGet(mudcsAddSystem(fellowChamberDp + ".status"), fellowChamberStatus);
    }
    
    // switch off the master chanel only if the chamber doesn't have a fellow (another chamber hooked up to the same distrib. board) or the fellow chamber is off
    if ((fellowChamberDp == "") || (fellowChamberStatus == 0)) {
      mudcsMasterChannelSwitch(0, coord_master, master_chan, DpNameFsm);
    }
    
 } // else
} // if


}

//============================================================================================

retrieveCoordinateSet(string type_par, string dp_name, string setList, int w_pos, int radius, int ich, string type, string &set){

dyn_string list;
int i;
  bool found;
  string fsm, t_string1;
  int pos,pos1, pos2;

//DebugN("retrieveCoordinateSet:>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> "+ type+" sent "+ dp_name);
//-----------------------------------------------------------------------------------
if(setList == "CoordFromDpName"){  
  if(type_par=="FED_1"){
   pos=strpos(dp_name,"_FED");
   pos1=strpos(dp_name,"DDU0");
   pos2=strpos(dp_name,"DDU");
   if(pos1>=0)set=substr(dp_name,pos2,3)+substr(dp_name,pos2+4,1);
   else set=substr(dp_name,pos2,5);
   //set=substr(dp_name, pos1,pos-(pos1));
  } 
  return;
}  
//-----------------------------------------------------------------------------------
 if(setList == "CscLevelDevicesCoordinates"){  
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o."+setList),list);
  found=false;
  mudcsConvertData2FsmOne(type_par, dp_name, fsm);

  for(i=1; i<=dynlen(list); i++){



           pos=strpos(list[i]," ");           
           if(pos<0)continue;
           t_string1 = CSC_fwG_g_SYSTEM_NAME+":"+substr(list[i],0,pos);
           set = substr(list[i],pos+1);
/*
if(strpos(type_par,"MRTN")>=0 && strpos(list[i],"MRTN")>=0){
dyn_string dyn_debug;
dpGet(mudcsAddSystem("dyn_debug2."),dyn_debug);
 //  dynAppend(dyn_debug,dp_name+">>"+fsm+">>"+list[i]+">>"+t_string1+">>"+set); 
   dynAppend(dyn_debug,fsm+">>"+t_string1); 
dpSet(mudcsAddSystem("dyn_debug2."),dyn_debug);
}
*/
   if(t_string1 == fsm){
    found=true;


    break;
   }
  } // for  
 if(!found)set ="";
 return;
 }
//-----------------------------------------------------------------------------------
 if((w_pos==4 || w_pos==5) && radius == 1){ // i.e. ME 1/1
  if(setList != "DiskLevelDevicesCoordinates")
   dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Ring_DUBNA."+setList),list);
  else 
   dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Ring_DUBNA."+setList),list);

 }
 else{
  if(setList != "DiskLevelDevicesCoordinates"){  
    if(((w_pos==4 || w_pos==5) && radius==2 ) || radius==1)
      dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel"+w_pos+".In"+setList),list);
    else 
      dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel"+w_pos+".Out"+setList),list);
  }
  else{
      dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel"+w_pos+"."+setList),list);
  }

 } // else



//---------------
 if(setList != "DiskLevelDevicesCoordinates"){  
  if((w_pos==4 || w_pos==5) && radius == 1){ // i.e. ME 1/1
     if((w_pos-4)*36+ich  > dynlen(list) ||  list[(w_pos-4)*36+ich] == "") {set=""; return;}
       set=list[(w_pos-4)*36+ich];
  }
  else{ 
 
    if(ich  > dynlen(list) || ich  <=0 ){set=""; return;}
    //    if(ich  > dynlen(list) ||  list[ich] == ""){set=""; return;}
    //DebugTN(dp_name+">>>>>>>>>>>>>>>>>>>>>>>>"+ich);
       set=list[ich];
  }
 }
//------------------------------------
 else{
  
  found=false;

  
  mudcsConvertData2FsmOne(type_par, dp_name, fsm);

//-------------------
/*
if(type_par=="CRB_1"||type_par=="MRTN_1"){
dyn_string dyn_debug1;
dpGet("dyn_debug1.",dyn_debug1);
dynAppend(dyn_debug1,type_par+">========= "+ dp_name+">>"+fsm+"--"+ setList+ ">>set="+set+"w_pos="+w_pos);
dpSetWait("dyn_debug1.",dyn_debug1);
}
*/
//---------------------------------------------------------------------


///DebugTN(fsm+">>"+list[1]+">>"+dynlen(list)+">>");

  for(i=1; i<=dynlen(list); i++){

           pos=strpos(list[i]," ");           
           if(pos<0)continue;
           
           t_string1 = mudcsAddSystem(substr(list[i],0,pos));
           set = substr(list[i],pos+1);

//-------------------
if(type_par=="CRB_1"){
dyn_string dyn_debug1;
dpGet("dyn_debug1.",dyn_debug1);
dynAppend(dyn_debug1,type_par+">========= "+ t_string1+">>"+fsm+"--"+ setList+ ">>set="+set+"w_pos="+w_pos);
dpSetWait("dyn_debug1.",dyn_debug1);
}
//---------------------------------------------------------------------


   if(t_string1 == fsm){
    found=true;
    break;
   }
 } // for
  if(!found)set="";

 } // esle
//--------
}



//============================================================================================
// 10/13/2004

mudcsCommandCscLevel(string dp_name,string subcommand, string device_type){
////DebugN("................mudcsCommandCscLevel:>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");


int i, pos;
int j;

string DpNameFsm;
int last_vset;

int w_pos,radius,ich;
string type;
string address,comp_subcommand;
dyn_string list;
dyn_string test_list, command_list;


 dynClear(command_list);

////// test_list=dpNames(device_type+"*","MUDCS_DIMCOM");
test_list=dpNames(CSC_fwG_g_SYSTEM_NAME+":"+device_type+"*_COM");

 for(i=1;i<=dynlen(test_list);i++){
   pos=strpos(test_list[i],"_CONF");
   if(pos < 0){
      dynAppend(command_list, test_list[i]);
   }
 }

 if(dp_name != "all;all"){


//for(int i=1;i<=dynlen(CSC_fwG_g_DEVICE_LIST);i++)DebugN("mudcsCommandCscLevel: "+CSC_fwG_g_DEVICE_LIST[i]+">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>11");
DebugN("mudcsCommandCscLevel:"+type+">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>1 "+dp_name +" "+subcommand);

  if(dynContains(CSC_fwG_g_DEVICE_LIST,device_type)){

DebugN("mudcsCommandCscLevel:>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> "+ device_type+" sent "+ dp_name);

  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.CscLevelDevicesCoordinates"),list);

 int found=0;
 dyn_string dyn_list, dyn_list2;
 
    for(j=1;j<=dynlen(list);j++){ 
     dyn_list= strsplit(list[j]," "); 
     if(strpos(dp_name,dyn_list[1]) >=0 ){      
       found=j;
       break;
     } // if(strpos(list[i],device_type) >=0 )
    } // for(j=1;j<=dynlen(list);j++)




    if(found){ 
      for(i=1;i<=dynlen(command_list);i++){
//
// !!!! Attention: the command_list does not make much sense here as an only one computer will supposedly 
// !!!! run the all devices of the given csclevel device_type
// !!!!
//-------------------------------------
       if(device_type == "HV_1"){
        dyn_list2= strsplit(dyn_list[2],";");

         if(subcommand == "power_on"){
          subcommand="HVPRCMD;"+dyn_list2[3]+";"+dyn_list2[4]+";255;1;0;-1";
          DpNameFsm=substr(dp_name,0,strpos(dp_name,".status"));
          dpGet(mudcsAddSystem(DpNameFsm+".last_vset"),last_vset);          

          subcommand=subcommand  +"|"+ "HVPRCMD;"+dyn_list2[3]+";"+dyn_list2[4]+";255;7;"+last_vset+";-1";
         }
         else if(subcommand == "standby"){
          subcommand="HVPRCMD;"+dyn_list2[3]+";"+dyn_list2[4]+";255;1;0;-1";
          DpNameFsm=substr(dp_name,0,strpos(dp_name,".status"));       

          subcommand=subcommand  +"|"+ "HVPRCMD;"+dyn_list2[3]+";"+dyn_list2[4]+";255;7;"+"3000"+";-1";
         }         
         else if(subcommand == "power_off"){
          subcommand="HVPRCMD;"+dyn_list2[3]+";"+dyn_list2[4]+";255;0;0;-1";
         }
         else if(subcommand == "get_data"){
          subcommand="HVPRDATA;"+dyn_list2[3]+";"+dyn_list2[4]+";255;0;0;-1";
         }


     dyn_string subcommands=strsplit(subcommand,"|");
     for(j=1;j<=dynlen(subcommands);j++){
      dpSetWait(mudcsAddSystem(command_list[i]+".command"),dyn_list2[2]+"|"+subcommands[j]);
     }
           


DebugTN(">>>>>>>>>>>>"+command_list[i]+".command",dyn_list2[2]+"|"+subcommand);
/*
    dyn_string dyn_debug;
    dpGet(mudcsAddSystem("dyn_debug2."),dyn_debug);
    dynAppend(dyn_debug,command_list[i]+".command"+">>>"+dyn_list2[2]+"|"+subcommand);
    dpSet(mudcsAddSystem("dyn_debug2."),dyn_debug);
*/
  
       } // if(device_type == "HV_1")
//-------------------------------------
       else{
       dpSetWait(mudcsAddSystem(command_list[i]+".command"),dyn_list[2]+"|"+subcommand);
DebugN("mudcsCommandCscLevel:>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> "+command_list[i]+".command" +" "+ dyn_list[2]+"|"+subcommand );

       }

      } // for
    } // if(found)
  } // if(dynContains(CSC_fwG_g_DEVICE_LIST,type))

  }// not all;all  
  else {
   for(i=1;i<=dynlen(command_list);i++){  
    dpSetWait(mudcsAddSystem(command_list[i]+".command"),"all;all"+"|"+subcommand);
   }
  }
}
//============================================================================================

mudcsHVCommand(string fsm, int com, int value){
  
  
 string coord;
 dyn_string d_coord;
 dpGet(fsm+".coord",coord);
 d_coord=strsplit(coord,";");

 
int chn=255;
if(d_coord[4]=="1")chn=253;
else if(d_coord[4]=="2")chn=254;

string subcommand="HVCMD;"+d_coord[2]+";"+d_coord[3]+";"+chn+";"+com+";"+value+";"+"-1";

dpSetWait("HV_1_COM"+".command",d_coord[1]+"|"+subcommand);



}







//============================================================================================

//============================================================================================


//-----------------------------------------------------------------------------------------------------------------------

mudcs_setBrokerList(){

int i;
int pos;
dyn_string dpTypeList, BrokerList, BrokerList_old;

while(!(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_o"))){
  
 delay(0,100);

}
// delay(1,100);

dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimBrokerList"), BrokerList);
/////dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimBrokerList_old"), BrokerList_old);

  dpTypeList = dpNames("*DimBroker_o");
  for(i=1; i <= dynlen(dpTypeList); i++ ){




   pos = strpos(dpTypeList[i],":");
   if(pos >=0)
   dpTypeList[i] = substr(dpTypeList[i],pos+1,strlen(dpTypeList[i])-2-pos-1);
   else 
   dpTypeList[i] = substr(dpTypeList[i],0,strlen(dpTypeList[i])-2);

    if(dynContains(BrokerList,dpTypeList[i]) < 1){
     dynAppend(BrokerList,dpTypeList[i]);
//      if(!(dpExists(CSC_fwG_g_SYSTEM_NAME+":"+dpTypeList[i]+"_o"))){

//	dpCreate(dpTypeList[i]+"_o", dpTypeList[i]);
//         while(!(dpExists(CSC_fwG_g_SYSTEM_NAME+":"+dpTypeList[i]+"_o"))){
//          delay(0,50);
//         }

//      }//if

    }
  }

dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimBrokerList"), BrokerList);
dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimBrokerList_old"), BrokerList); // the old and new are the same here 

//-----------------------------------------------------------------------------------------------------------------------

}

//============================================================================================
//============================================================================================
// fsm: to correct


//mudcsCscSection_create(/*fsm: string dpName, string label, string parentDpName, string parentLabel,*/
//								dyn_string &exceptionInfo)
mudcsCscSection_create(dyn_string &exceptionInfo)
{
	int pos;
	string operationPanel, configPanel, containerDpName, containerLabel;

int i, j;
dyn_string add_labels;
string dpName_d;
string newDpLabel_d;
string parentDp_d;
string currentDpLabel_d;
dyn_string exceptionInfo_d;

int w_num;

dyn_string dpTypeList, BrokerList, BrokerList_old;
dyn_string dpPointList;
string update_fun, service;
dyn_string test_dyn_string,test_dyn_string2;

//dyn_string test_dyn_string;
string test_string, test_string2;
int test_int1, test_int2,test_int3;

dyn_int connectedApiIds;

///////////////////////////////////////////////////////////////////////mudcsInit();

//mudcsDim(true,"fast_final2",exceptionInfo);

/////mudcsDimStart();
//mudcsDebug("1");

//////return;




/////////return;


//treeBrowser.selectedPos(2);

//dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":treeBrowser_control."),makeDynString("selectedPos","3"));
//return;

////// database filling




        if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_o"))dpDelete(CSC_fwG_g_SYSTEM_NAME+":Db_o");
//-----------------------------------------------------------------------------------------------------------------------
	dpCreate("Db_o", "Db");
//-----------------------------------------------------------------------------------------------------------------------


while(!(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_o"))){
  
 delay(0,100);

}

mudcs_setBrokerList();

/* removed to mudcs_setBrokerList()
// delay(1,100);

dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimBrokerList"), BrokerList);
/////dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimBrokerList_old"), BrokerList_old);

  dpTypeList = dpNames("*DimBroker_o");
  for(i=1; i <= dynlen(dpTypeList); i++ ){




   pos = strpos(dpTypeList[i],":");
   if(pos >=0)
   dpTypeList[i] = substr(dpTypeList[i],pos+1,strlen(dpTypeList[i])-2-pos-1);
   else 
   dpTypeList[i] = substr(dpTypeList[i],0,strlen(dpTypeList[i])-2);

    if(dynContains(BrokerList,dpTypeList[i]) < 1){
     dynAppend(BrokerList,dpTypeList[i]);
//      if(!(dpExists(CSC_fwG_g_SYSTEM_NAME+":"+dpTypeList[i]+"_o"))){

//	dpCreate(dpTypeList[i]+"_o", dpTypeList[i]);
//         while(!(dpExists(CSC_fwG_g_SYSTEM_NAME+":"+dpTypeList[i]+"_o"))){
//          delay(0,50);
//         }

//      }//if

    }
  }

dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimBrokerList"), BrokerList);
dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimBrokerList_old"), BrokerList); // the old and new are the same here 

*/
//-----------------------------------------------------------------------------------------------------------------------

// temporal !!!!!!!!!! should be done in config panel !!!!


   test_dyn_string=makeDynString("smi","ufcmspc01");
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimServerComputerList"),test_dyn_string);
   test_int1 = dynlen(test_dyn_string);

  for(i=1; i <= test_int1; i++ ){
   if(test_dyn_string[i] != "smi"){

     test_string="_DimClient_"+i;

     if(!(dpExists(CSC_fwG_g_SYSTEM_NAME+":"+test_string))){
         dpCreate(test_string,"_FwDimConfig");
         while(!(dpExists(CSC_fwG_g_SYSTEM_NAME+":"+test_string))){delay(0,50);}
     }

 

	dpGet(mudcsAddSystem("_Connections.Device.ManNums:_original.._value"),connectedApiIds);

	if(dynContains(connectedApiIds, i)<=0)
	{
     int pos_part;
     if((pos_part=strpos(test_dyn_string[i],"_part"))>=0)
     test_dyn_string[i]=substr(test_dyn_string[i],0,pos_part);
               
         test_string2=CSC_fwG_g_jcop_dim_home/*CSC_fwG_g_project_name_home*/ + "/bin/PVSS00dim"+CSC_fwG_g_PVSS_VERSION+" -num "+i+" -dim_dp_config "+test_string+
         " -dim_dns_node "+test_dyn_string[i]; 
         if(CSC_fwG_g_FSM_V24R09)test_string2 = "export DIM_DNS_NODE="+test_dyn_string[i]+";"+test_string2;
operating_system=getenv("OS");
//-------
      bool isManagerEmpty=false;
      dyn_string clientServices, clientCommands;
      dpGet(test_string+".clientServices",clientServices);
      dpGet(test_string+".clientCommands",clientCommands);
      if(dynlen(clientServices) <= 1 /*(dummy service)*/ && dynlen(clientCommands) ==0)isManagerEmpty=true;
//-------  
      if(!isManagerEmpty){         
         if(operating_system=="Linux")system(test_string2+" &");
         else system("start /b "+test_string2); 

 mudcsDebug(test_string2);
      }
	}

   } //  if(test_dyn_string[i] != "smi"){
 
        dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.PCToManID"),test_dyn_string2);
        test_dyn_string2[i]=test_dyn_string[i];
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.PCToManID"),test_dyn_string2);

     
  }


//-----------------------------------------------------
//-----------------------------------------------------

////////////// dpPointList=dpNames(CSC_fwG_g_SYSTEM_NAME+":_DimClient_*");



//-------

/*
  for(i=1; i <= dynlen(dpPointList); i++ ){
    for(j=1; j<= dynlen(BrokerList); j++){


       service=substr(BrokerList[j],0,strpos(BrokerList[j],"_DimBroker"));
       update_fun="mudcsUpdateMonitor_"+service;

    if(i==1){dpConnect(update_fun,CSC_fwG_g_SYSTEM_NAME+":"+BrokerList[j]+"_o.update_value");
     //  mudcsDebug(update_fun);
     //  mudcsDebug(CSC_fwG_g_SYSTEM_NAME+":"+BrokerList[j]+"_o.update_value");

    }
     mudcsDimConfig(true,BrokerList[j]+"_o,"+service+
       ",,0,0", dpPointList[i], exceptionInfo);

	if(dynlen(exceptionInfo)>0){return;}

     mudcsDimConfig(false,BrokerList[j]+"_o.command,"+service+"_COMMAND", dpPointList[i], exceptionInfo);

	if(dynlen(exceptionInfo)>0){ return;}

    }
  }
*/


//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
dyn_string device_list;
string device_info;

mudcsGetDeviceList(device_list);
for(int i=1;i<=device_list[i];i++){

  device_info= device_list[i]+"Info.panels.editor.hardware";
  dpSet(mudcsAddSystem(device_info),"emu_config/emuDev" + device_list[i]+"Config");

  device_info= device_list[i]+"Info.panels.navigator.hardware";
  dpSet(mudcsAddSystem(device_info),"emu/emuDev" + device_list[i]+"Operation");

}
//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF

////////////////mudcsDimConfigCommon(exceptionInfo);
///////////////if(dynlen(exceptionInfo)>0){ return;}

// temporal !!!!!!!!!!
////dpConnect("test_fun",CSC_fwG_g_SYSTEM_NAME+":LV_1_TEST_o.v9");//:_online.._value");
//test_fun("","");


//---------------------------------------------------

// temporal !!!!!!!!!!
///////    dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":LV_1_DimBroker_o.dynatemTcpIp"), "dynatem2");
///////    dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":LV_1_DimBroker_o.setNumber"), 3);
// temporal !!!!!!!!!!
    dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":HV_1_DimBroker_o.controllerNumber"), 1);
    dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":HV_1_DimBroker_o.boardNumber"), 2);
    dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":HV_1_DimBroker_o.channelNumber"), 3);

// temporal !!!!!!!!!!
//////        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.InDynatemAndSetNumberList"),makeDynString( "dynatem2;13","dynatem_fast_final;13")); // m4
//////        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.OutDynatemAndSetNumberList"),makeDynString("dynatem2;13","dynatem_fast_final;13")); // m4
//dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.OutDynatemAndSetNumberList"),makeDynString("128.146.39.89;13","LVCB_1.ELMB_3E;0")); // m4

//dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.CscLevelDevicesCoordinates"),"GasMonitoring/CSCGAS_SX5 LVCB_1.ELMB_3F;0");

// ch -3/2/01 LVCB_1.ELMB_3F;0
// ch -3/2/31 LVCB_1.ELMB_3E;0
// ch -3/2/33 LVCB_1.ELMB_3E;1
// ch -3/2/35 LVCB_1.ELMB_3F;1

// ch +2/1/11 100;18;3;2
// ch +2/1/12 100;18;4;1
// ch +2/1/14 100;18;4;2
// ch +2/1/16 100;18;1;2

// ch +3/1/11 100;18;3;1
// ch +3/1/12 100;18;2;2
// ch +3/1/13 100;18;2;1
// ch +3/1/14 100;18;1;1
// ch +3/1/15 100;18;0;2
// ch +3/1/16 100;18;0;1



//////// ME+2/2/27    100;16;10;0 (272)
///////// ME+2/2/29   100;16;8;0 (267)
// ch +2/2/30         100;16;7;0 (276)
////////// ME+2/2/31  100;16;9;0  (271)
// ch +2/2/32         100;16;6;0 (268)


//////////// ME+3/2/27  100;16;13;0 (273)
////////// ME+3/2/30    100;16;11;0 (270)
///////// // ME+3/2/31  100;16;12;0 (281)
// ch +3/2/32           100;16;5;0 (269)


// temporal !!!!!!!!!!
/////////////        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.InHVsetList"),makeDynString("2;3;4","1;2;3")); // m4
/////////////        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.OutHVsetList"),makeDynString("2;3;4","1;2;3")); // m4
// temporal !!!!!!!!!!
///mudcsCommand(CSC_fwG_g_SYSTEM_NAME+":mudcsCsc_o_m4inn02LV_1","ffffff1");
///return;
//-----------------------------------------------------------------------------------------------------------------------
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.InRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1")); // m4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.OutRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1")); 
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel8.InRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1")); // p4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel8.OutRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1"));
//=
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.InConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); //m4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.OutConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); 
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel8.InConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); //p4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel8.OutConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); 
//=
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.InOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); //m4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel1.OutOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); 
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel8.InOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); //p4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel8.OutOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); 
//=====
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel2.InRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1")); // m4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel2.OutRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1")); 
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel3.InRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1")); // p4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel3.OutRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1"));
//=
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel2.InConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); //m4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel2.OutConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); 
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel3.InConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); //p4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel3.OutConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); 
//=
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel2.InOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); //m4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel2.OutOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); 
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel3.InOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); //p4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel3.OutOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); 



        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel4.InRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1")); // m4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel4.OutRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1")); 
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel5.InRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1")); // p4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel5.OutRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1"));
//=
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel4.InConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); //m4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel4.OutConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); 
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel5.InConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); //p4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel5.OutConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); 
//=
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel4.InOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); //m4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel4.OutOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); 
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel5.InOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); //p4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel5.OutOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); 



        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel6.InRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1")); // m4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel6.OutRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1")); 
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel7.InRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1")); // p4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel7.OutRadDeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1"));
//=
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel6.InConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); //m4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel6.OutConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); 
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel7.InConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); //p4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel7.OutConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); 
//=
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel6.InOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); //m4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel6.OutOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); 
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel7.InOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); //p4
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel7.OutOperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); 
//---

        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Ring_DUBNA.DeviceList"),makeDynString("HV_1","LV_1"
        ,"CHIP_1","TEMP_1")); // m4

//=
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Ring_DUBNA.ConfigPanels"),makeDynString("emu_config/emuDevHV_1Config","emu_config/emuDevLV_1Config"
        ,"emu_config/emuDevCHIP_1Config","emu_config/emuDevTEMP_1Config")); //m4

//=
        dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Ring_DUBNA.OperationPanels"),makeDynString("emu/emuDevHV_1Operation","emu/emuDevLV_1Operation"
        ,"emu/emuDevCHIP_1Operation","emu/emuDevTEMP_1Operation")); //m4




//-----------------------------------------------------------------------------------------------------------------------

 //// fsm: not used ??       dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.DimConfigNumber"),2); /// temporary !!!!!!!!!!!!!!!!!! 




//-----------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------

////        if(dpExists(CSC_fwG_g_SYSTEM_NAME+":HV_1_DimBroker_o"))dpDelete(CSC_fwG_g_SYSTEM_NAME+":HV_1_DimBroker_o");
////        if(dpExists(CSC_fwG_g_SYSTEM_NAME+":LV_1_DimBroker_o"))dpDelete(CSC_fwG_g_SYSTEM_NAME+":LV_1_DimBroker_o");
////	dpCreate("HV_1_DimBroker_o", "HV_1_DimBroker");
////        dpCreate("LV_1_DimBroker_o", "LV_1_DimBroker");


//-----------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------

//==============================================
/* fsm


	//remove system name if it given
	dpName = substr(dpName, strpos(dpName, ":")+1);

	//get dp name and label of default container
///////	mudcsCscSection_locateContainer(containerDpName, containerLabel, exceptionInfo);
      	containerDpName = parentDpName;
        containerLabel = parentLabel;	

	//if problems with container, return
	if(dynlen(exceptionInfo)>0){return;}
		
	//if additional parent was given, check its details
	if(parentDpName != "")
	{
		//check given parent dp exists and is of valid type and dpname
		if(!fwGenericDpFunctions_checkIsDpOfValidType(parentDpName, exceptionInfo))
		{
			fwException_raise(exceptionInfo, "ERROR", "mudcsCscSection_create(): The given parent dp is not valid", "");
			return;
		}

		//check system name of parent was given
		pos = strpos(parentDpName,":");
		if(pos<0)
		{
			fwException_raise(exceptionInfo, "WARNING", "mudcsCscSection_create(): The system name of the parent dp must be given", "");
			return;
		}
		// check if the parentLabel exists for the parent dp
		pos = strpos(parentLabel,";");
		if(pos<0)
			parentLabel += (";"+parentDpName);

		fwGenericDpFunctions_checkLabelExists(parentDpName, parentLabel, exceptionInfo);
		if(dynlen(exceptionInfo)>0){return;}
	}

	//if no problems, create dp in default container
	_mudcsCscSection_create(dpName, label, containerDpName, containerLabel, exceptionInfo);

	//if problems with create, return
	if(dynlen(exceptionInfo)>0){return;}

	//if additional parent name was given and is not equal to the default contain,
	//add the new device to the second parent
	if((parentDpName != "")&&(parentDpName != containerDpName))
	{
		//add system name to dp name for hierarchy information
		dpName = getSystemName() + dpName;
	
		//generate the information needed in order to add dp to hierarchy
		operationPanel = "mudcsCscSectionOperation";
		configPanel = "mudcsCscSectionConfig";
	
		//reduce label to user label only
		parentLabel = substr(parentLabel, 0, strpos(parentLabel, ";"));

		//add new device in the second position in the hierarchy
		fwGenericDpFunctions_addChildToParent(dpName, label, parentDpName, parentLabel,
												makeDynString(configPanel),
												makeDynString(operationPanel), exceptionInfo);
	}

parentDp_d = CSC_fwG_g_SYSTEM_NAME+":Csc_o";
currentDpLabel_d = "Csc1";




// add_labels = makeDynString("m4","m3","m2","m1","p1","p2","p3","p4");


 for(i=1; i <= 8; i++ ){

   dpName_d = CSC_fwG_g_SYSTEM_NAME+":mudcsCsc_o_"+CSC_fwG_g_wheel_labels[i];
   newDpLabel_d = "Disk"+CSC_fwG_g_wheel_labels[i];
   mudcsCscWheel_create(i, dpName_d, newDpLabel_d, parentDp_d, currentDpLabel_d, exceptionInfo);
	if(dynlen(exceptionInfo)>0){return;}

 


 }


// temporal !!!!!!!!!!
/////// mudcsUpdateMonitor_LV_1("","");
// temporal !!!!!!!!!!
//// mudcsUpdateMonitor_HV_1("","");

*/


}
mudcsPcCratesMapping(){
    
dyn_string ds, ds2;
  
  //    bool is=patternMatch("*_C0[34]_TEMP","_C05_TEMP");
  //    ds=dpNames("*_C0[3]_TEMP","TEMP_1");
  
//  ds2=dynPatternMatch("*_C0[345]_TEMP",ds);

dyn_string sides=makeDynString("P","M");
dyn_string stations=makeDynString("2","3","4"); 
string side;
string station;

//side="P";
//  ds=dpNames("*_"+side+"*_C*_TEMP","TEMP_1_d");
//  CSC_fwG_g_PCRATES_MAP[side+"1_PC1A"]=dynPatternMatch("*_C0[345]_TEMP",ds);  
// DebugTN(CSC_fwG_g_PCRATES_MAP[side+"1_PC1A"]); 
 //  DebugTN(ds); 
//return;
    
 int i,j;   
for(i=1;i<=2;i++){
 side=sides[i];
      
  station="1";
            
 ds=dpNames("*ME_"+side+"*_C*_TEMP","TEMP_1_d");
  CSC_fwG_g_PCRATES_MAP[side+"1_PC1A"]=dynPatternMatch("*"+side+"1?_C0[345]_TEMP",ds); 
  CSC_fwG_g_PCRATES_MAP[side+"1_PC1B"]=dynPatternMatch("*"+side+"1?_C0[678]_TEMP",ds);
  CSC_fwG_g_PCRATES_MAP[side+"1_PC2A"]=dynPatternMatch("*"+side+"1?_C1[234]_TEMP",ds);
  
  ds2=dynPatternMatch("*"+side+"1?_C09_TEMP",ds); dynAppend(ds2,dynPatternMatch("*"+side+"1?_C1[01]_TEMP",ds));
  dynSortAsc(ds2);
  CSC_fwG_g_PCRATES_MAP[side+"1_PC2B"]=ds2;
  
//  CSC_fwG_g_PCRATES_MAP[side+"1_PC3A"]=dynPatternMatch("*"+side+"1?_C1[89]_TEMP",ds)+dynPatternMatch("*"+side+"1?_C20_TEMP",ds);
  ds2=dynPatternMatch("*"+side+"1?_C1[89]_TEMP",ds);dynAppend(ds2,dynPatternMatch("*"+side+"1?_C20_TEMP",ds));
  dynSortAsc(ds2);
  CSC_fwG_g_PCRATES_MAP[side+"1_PC3A"]=ds2;
      
  CSC_fwG_g_PCRATES_MAP[side+"1_PC3B"]=dynPatternMatch("*"+side+"1?_C1[567]_TEMP",ds);
  CSC_fwG_g_PCRATES_MAP[side+"1_PC4A"]=dynPatternMatch("*"+side+"1?_C2[456]_TEMP",ds);    
  CSC_fwG_g_PCRATES_MAP[side+"1_PC4B"]=dynPatternMatch("*"+side+"1?_C2[123]_TEMP",ds); 
  CSC_fwG_g_PCRATES_MAP[side+"1_PC5A"]=dynPatternMatch("*"+side+"1?_C2[789]_TEMP",ds);     
  CSC_fwG_g_PCRATES_MAP[side+"1_PC5B"]=dynPatternMatch("*"+side+"1?_C3[012]_TEMP",ds); 
  CSC_fwG_g_PCRATES_MAP[side+"1_PC6A"]=dynPatternMatch("*"+side+"1?_C3[345]_TEMP",ds);     
  
//  CSC_fwG_g_PCRATES_MAP[side+"1_PC6B"]=dynPatternMatch("*"+side+"1?_C36TEMP",ds)+dynPatternMatch("*"+side+"1?_C0[12]TEMP",ds);
  ds2=dynPatternMatch("*11_C36_TEMP",ds);dynAppend(ds2,dynPatternMatch("*11_C0[12]_TEMP",ds));
  dynAppend(ds2,dynPatternMatch("*12_C36_TEMP",ds));dynAppend(ds2,dynPatternMatch("*12_C0[12]_TEMP",ds));
  dynAppend(ds2,dynPatternMatch("*13_C36_TEMP",ds));dynAppend(ds2,dynPatternMatch("*13_C0[12]_TEMP",ds));
    
//  dynSortAsc(ds2);  
  CSC_fwG_g_PCRATES_MAP[side+"1_PC6B"]=ds2;
  
  for(j=1;j<=3;j++){  
  station=stations[j];
  
  //CSC_fwG_g_PCRATES_MAP[side+station+"_PC1"]=dynPatternMatch("*_?"+station+"1_C0[234]_TEMP",ds)+dynPatternMatch("*_?"+station+"2_C0[345678]_TEMP",ds); 
  ds2=dynPatternMatch("*_?"+station+"1_C0[234]_TEMP",ds);
  dynAppend(ds2,dynPatternMatch("*_?"+station+"2_C0[345678]_TEMP",ds));
  CSC_fwG_g_PCRATES_MAP[side+station+"_PC1"]=ds2;
    
  //CSC_fwG_g_PCRATES_MAP[side+station+"_PC2"]=dynPatternMatch("*_?"+station+"1_C0[567]_TEMP",ds)+
  //                    dynPatternMatch("*_?"+station+"2_C09_TEMP",ds)+dynPatternMatch("*_?"+station+"2_C1[01234]_TEMP",ds);      
  ds2=dynPatternMatch("*_?"+station+"1_C0[567]_TEMP",ds);
  dynAppend(ds2,dynPatternMatch("*_?"+station+"2_C09_TEMP",ds));
  dynAppend(ds2,dynPatternMatch("*_?"+station+"2_C1[01234]_TEMP",ds)); 
 CSC_fwG_g_PCRATES_MAP[side+station+"_PC2"]=ds2;
   
 // CSC_fwG_g_PCRATES_MAP[side+station+"_PC3"]=dynPatternMatch("*_?"+station+"1_C0[89]_TEMP",ds)+   dynPatternMatch("*_?"+station+"1_C10_TEMP",ds)+
 //                     dynPatternMatch("*_?"+station+"2_C1[56789]_TEMP",ds)+dynPatternMatch("*_?"+station+"2_C20_TEMP",ds);                
  ds2=dynPatternMatch("*_?"+station+"1_C0[89]_TEMP",ds);
  dynAppend(ds2,dynPatternMatch("*_?"+station+"1_C10_TEMP",ds));
  dynAppend(ds2,dynPatternMatch("*_?"+station+"2_C1[56789]_TEMP",ds)); 
  dynAppend(ds2,dynPatternMatch("*_?"+station+"2_C20_TEMP",ds));
  CSC_fwG_g_PCRATES_MAP[side+station+"_PC3"]= ds2;
  
 // CSC_fwG_g_PCRATES_MAP[side+station+"_PC4"]=dynPatternMatch("*_?"+station+"1_C1[123]_TEMP",ds)+dynPatternMatch("*_?"+station+"2_C2[123456]_TEMP",ds);                
   ds2=dynPatternMatch("*_?"+station+"1_C1[123]_TEMP",ds);
  dynAppend(ds2,dynPatternMatch("*_?"+station+"2_C2[123456]_TEMP",ds));
 CSC_fwG_g_PCRATES_MAP[side+station+"_PC4"]=ds2;
  
 // CSC_fwG_g_PCRATES_MAP[side+station+"_PC5"]=dynPatternMatch("*_?"+station+"1_C1[456]_TEMP",ds)+
 //                     dynPatternMatch("*_?"+station+"2_C2[789]_TEMP",ds)+dynPatternMatch("*_?"+station+"2_C3[012]_TEMP",ds);        
   ds2=dynPatternMatch("*_?"+station+"1_C1[456]_TEMP",ds);
  dynAppend(ds2,dynPatternMatch("*_?"+station+"2_C2[789]_TEMP",ds));
  dynAppend(ds2,dynPatternMatch("*_?"+station+"2_C3[012]_TEMP",ds)); 
CSC_fwG_g_PCRATES_MAP[side+station+"_PC5"]=ds2;
    
 // CSC_fwG_g_PCRATES_MAP[side+station+"_PC6"]=dynPatternMatch("*_?"+station+"1_C1[78]_TEMP",ds)+   dynPatternMatch("*_?"+station+"1_C01_TEMP",ds)+
 //                     dynPatternMatch("*_?"+station+"2_C3[3456]_TEMP",ds)+dynPatternMatch("*_?"+station+"2_C0[12]_TEMP",ds); 
  ds2=dynPatternMatch("*_?"+station+"1_C1[78]_TEMP",ds);
  dynAppend(ds2,dynPatternMatch("*_?"+station+"1_C01_TEMP",ds));
  dynAppend(ds2,dynPatternMatch("*_?"+station+"2_C3[3456]_TEMP",ds)); 
  dynAppend(ds2,dynPatternMatch("*_?"+station+"2_C0[12]_TEMP",ds));
 CSC_fwG_g_PCRATES_MAP[side+station+"_PC6"]= ds2;
} // j  
  
}
  
}
