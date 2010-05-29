
  
int new_lv_power_up_sequence=true;
int g_attempts=false;

bool new_lv_power_up_sequence_2=true;
bool new_lv_power_up_sequence_test=false;

bool automatic_stop_pcrate_slow_control=true;
bool two_way_communications_x2p=true;//false;//true;//false;//true;//false;

bool is_off_disabled_channels=false;

mudcsCrbDistConfig(){
  
string ProjectHome=getPath("");//"C:\\PVSS_PROJECT_36\\csc_lv";//+key;
strreplace(ProjectHome,"/","\\");
ProjectHome=substr(ProjectHome,0,strlen(ProjectHome)-1);  

string config_file=ProjectHome+"\\config\\config";
string dist_config;

if(CSC_fwCAN1_g_904_MACHINE)dist_config="dist_config_904.txt";
else dist_config="dist_config_p5.txt";

DebugTN("cmd /c cat "+ProjectHome+"\\source\\CMS_CSCfw_LV_CRB\\special_stuff\\"+dist_config+" >> "+config_file);
system("cmd /c cat "+ProjectHome+"\\source\\CMS_CSCfw_LV_CRB\\special_stuff\\"+dist_config+" >> "+config_file );  
  
}


mudcsCrbInitDelays(int mode){ // used as additional init for server
 int i,j;
 dyn_string dpp=dpNames(CSC_fwCAN1_g_SYSTEM_NAME+":*ME_P*","fwCrb_CSC_LV");
 dyn_string dpm=dpNames(CSC_fwCAN1_g_SYSTEM_NAME+":*ME_M*","fwCrb_CSC_LV");
 dyn_string dpall=dpp;
     
 dynAppend(dpall,dpm);  
 
 if(CSC_fwCAN1_g_DEV_MODE){ // corr++
  dpall= dpNames(CSC_fwCAN1_g_SYSTEM_NAME+":*","fwCrb_CSC_LV");
 }
 
 
 addGlobal("CSC_fwCAN1_g_CRB_DPS", DYN_STRING_VAR);  
 CSC_fwCAN1_g_CRB_DPS=dpall;
 
 addGlobal("CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY", DYN_INT_VAR);
 j=0;
 for(i=1;i<=dynlen(CSC_fwCAN1_g_CRB_DPS);i++){
  if(strpos(CSC_fwCAN1_g_CRB_DPS[i],"ME_M")>=0)j++;
  
  if(mode==0)CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=0; // in milliseconds
  else  {
    if(new_lv_power_up_sequence){
     if(!new_lv_power_up_sequence_2){ 
      if(j>0)CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=1000+(j-1)*1300; // in milliseconds
      else CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=1000+(i-1)*1300; // in milliseconds
     }
     else{
      if(!new_lv_power_up_sequence_test){
       if(j>0)CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=5000+(j-1)*1450; // in milliseconds
       else CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=5000+(i-1)*1450; // in milliseconds 
      }
      else{
       if(j>0)CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=1000+(j-1)*42000; // in milliseconds
       else CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=1000+(i-1)*42000; // in milliseconds         
      }
     }
     
    }
    else 
      CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=(i-1)*40; // in milliseconds
  }  
 }
 
 
  
}


//=====================================================================

mudcsCrbPnlInit(int level=1){

  int i,n;
  dyn_string ds1,ds2,ds3;
  string coord;

float componentVersion;  
addGlobal("CSC_fwCAN1_g_NEW_ELMB_SWITCH", BOOL_VAR);
dpGet("fwInstallation_fwElmb.componentVersion",componentVersion);
if(componentVersion>3.3)CSC_fwCAN1_g_NEW_ELMB_SWITCH=true;
else CSC_fwCAN1_g_NEW_ELMB_SWITCH=false;


//dpGet("TWO_WAY_X2P.",two_way_communications_x2p);  
  
addGlobal("CSC_fwCAN1_g_DB_PCMB", DYN_STRING_VAR);   
addGlobal("CSC_fwCAN1_g_DB_PCMB_GEOGRAPHIC", DYN_STRING_VAR);
dpGet("db_pcmb.list",CSC_fwCAN1_g_DB_PCMB);
dpGet("db_pcmb_geographic.list",CSC_fwCAN1_g_DB_PCMB_GEOGRAPHIC);
  
addGlobal("CSC_fwCAN1_g_P5_SIM", BOOL_VAR); // corr++
CSC_fwCAN1_g_P5_SIM=false;  // to set P5 simulation mode
     
addGlobal("CSC_fwCAN1_g_DEV_MODE", BOOL_VAR); // corr++

CSC_fwCAN1_g_DEV_MODE=false; // to set 904 only real nodes mode

addGlobal("CSC_fwCAN1_g_904_MACHINE", BOOL_VAR); // corr++

CSC_fwCAN1_g_904_MACHINE=false;

 string hostname1=getHostname();
  if((strpos(hostname1, "EMU-DCS-DEV") >= 0) || (strpos(hostname1, "emu-dcs-dev") >= 0)){ //  DEV machine  // corr++
   CSC_fwCAN1_g_904_MACHINE=true;  
  }
  else CSC_fwCAN1_g_DEV_MODE=false;
  
  if(CSC_fwCAN1_g_P5_SIM)CSC_fwCAN1_g_DEV_MODE=false;
  
addGlobal("CSC_fwCAN1_g_SINGLE_CRB_SWITCH", INT_VAR);
CSC_fwCAN1_g_SINGLE_CRB_SWITCH=0;
      
addGlobal("CSC_fwCAN1_g_IS_NEW_ATLAS_PSU", BOOL_VAR);
  CSC_fwCAN1_g_IS_NEW_ATLAS_PSU=true;
   
  addGlobal("CSC_fwCAN1_g_SYSTEM_NAME", STRING_VAR); 
  addGlobal("CSC_fwCAN1_g_EMU_ALIASES", DYN_STRING_VAR); 
  addGlobal("CSC_fwCAN1_g_ISACK", BOOL_VAR);
  addGlobal("CSC_fwCAN1_g_CANBUSES", DYN_STRING_VAR);  
  addGlobal("CSC_fwCAN1_g_csc_part", STRING_VAR);
      CSC_fwCAN1_g_csc_part="CMS_CSC_LV_CAN1";  
//--------------------------------------------------------------------------
   // CSC_fwCAN1_g_SYSTEM_NAME= "CMS_CSC_LV_CAN1";  //fwFsm_getSystemName();  
//-------------
      string node_to_define_system_name=CSC_fwCAN1_g_csc_part;
      fwCU_getDp(node_to_define_system_name,node_to_define_system_name);
//-------------            
   dyn_string systemNamesDps;
   dyn_string s_split;
   systemNamesDps=dpNames("*:CMS_CSC_LV_CRB");
//   systemNamesDps=dpNames("*:"+node_to_define_system_name);   
   if(dynlen(systemNamesDps)==0)mudcsLvDebug("SYSTEM NAME NOT DEFINED !");
   else{
    s_split=strsplit(systemNamesDps[1],":"); 
    CSC_fwCAN1_g_SYSTEM_NAME=s_split[1];//dpSubStr(systemNamesDps[1], DPSUB_SYS);
    ///mudcsLvDebug(CSC_fwCAN1_g_SYSTEM_NAME);
   }
if(automatic_stop_pcrate_slow_control){
  
//==x2p==
 if(two_way_communications_x2p){ 
  addGlobal("CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER", DYN_STRING_VAR); // x2p
  dpGet(mudcsLvAddSystem("PREPARE_POWER_UP_BUFFER."),CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER); //x2p 
 }
//========     
  addGlobal("CSC_fwCAN1_g_MINUS_SYSTEM_NAME", STRING_VAR);
  addGlobal("CSC_fwCAN1_g_PLUS_SYSTEM_NAME", STRING_VAR);  

  int m_max=0;
  int p_min=10;
  string valid;
    
  CSC_fwCAN1_g_PLUS_SYSTEM_NAME="";
  CSC_fwCAN1_g_MINUS_SYSTEM_NAME="";

  systemNamesDps=dpNames("*:CMS_CSC_HV_CC_*","MUDCS_STRING");
  DebugTN("systemNamesDps");
  DebugTN(systemNamesDps);
  for(i=1;i<=dynlen(systemNamesDps);i++){
    dpGet(systemNamesDps[i]+".value",valid);
    if(valid !="valid")continue;    
    ds2=strsplit(systemNamesDps[i],":");
    ds3=strsplit(ds2[dynlen(ds2)],"_");
    n=ds3[dynlen(ds3)];
    if(n<=4 && n > m_max){CSC_fwCAN1_g_MINUS_SYSTEM_NAME=ds2[1];m_max=n;}
    else if(n>=5 && n < p_min){CSC_fwCAN1_g_PLUS_SYSTEM_NAME=ds2[1];p_min=n;}
  }
  
    
}
   
//--------------------------------------------------------------------------     
    CSC_fwCAN1_g_ISACK=false;  
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
    

   


  CSC_fwCAN1_g_LIST_5V=makeDynInt(56,55,54,53,52,51,50,49,48);
  CSC_fwCAN1_g_LIST_3V1=makeDynInt(0,2,4,6,8,10,12,14,32);
  CSC_fwCAN1_g_LIST_3V2=makeDynInt(1,3,5,7,9,11,13,15,33);
  CSC_fwCAN1_g_LIST_15V=makeDynInt(20,21);
  CSC_fwCAN1_g_LIST_CC=makeDynInt(35,19);
  CSC_fwCAN1_g_LIST_MPC=makeDynInt(34,57);
  CSC_fwCAN1_g_LIST_MAIN=makeDynInt(16,17,18);

//---- should be uncomented only for the old PSU version --------
  if(!CSC_fwCAN1_g_IS_NEW_ATLAS_PSU){
   addGlobal("EPSU_POWER_ON_VALUE", BOOL_VAR);
   EPSU_POWER_ON_VALUE=false; 
  }
//---------------------------------------------------------------
 
if(level==0)return;    
  
//----- INIT PART------------------------------------
  CSC_fwCAN1_g_CANBUSES=dpNames(CSC_fwCAN1_g_SYSTEM_NAME+":*","FwElmbCANbus");
  for(i=1;i<=dynlen(CSC_fwCAN1_g_CANBUSES);i++){
   dpGet(CSC_fwCAN1_g_CANBUSES[i]+".nodeGuardInterval",CSC_fwCAN1_g_nodeGuardInterval[i]);
  }
  
  ds1=dpNames(CSC_fwCAN1_g_SYSTEM_NAME+":*","fwCrb_CSC_LV");  
  dynClear(CSC_fwCAN1_g_CRB2ELMB);
  for(i=1;i<=dynlen(ds1);i++){  
   dpGet(ds1[i]+".coord",coord);
   dynAppend(CSC_fwCAN1_g_CRB2ELMB,ds1[i]+";"+coord);
  }
  
//  DebugTN(CSC_fwCAN1_g_CRB2ELMB);
  
//---------------------------------------------------  
  
  
  
  return;
  
}

//========================================================================
int mudcsCrbGetChannelNumberByName(string dp){
  int iN; 
  dyn_string ds1,ds2;
  ds1=strsplit(dp,"/");
  ds2=strsplit(ds1[dynlen(ds1)],"_");
  iN=ds2[dynlen(ds2)];
  return iN;
  
}
//========================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
mudcs_fwElmbUser_setDoBit(string sElmb,string s,bool isOn,dyn_string argdsExceptionInfo){
  string sPortAInvalid;
  string sPortCInvalid;
  
  fwElmbUser_setDoBit(sElmb,s,isOn,argdsExceptionInfo);
    
 
  //:_online.._invalid
 /* 
   sPortAInvalid = sElmb + fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_CONFIG_NAME +
                  fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_PREFIX + "Bytes." +
                  ELMB_DO_PREFIX + "A" + "_read:_original.._aut_inv";
   sPortCInvalid = sElmb + fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_CONFIG_NAME +
                  fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_PREFIX + "Bytes." +
                  ELMB_DO_PREFIX + "C" + "_read:_original.._aut_inv";
   dpSetWait(sPortAInvalid, false, sPortCInvalid, false);
   
   sPortAInvalid = sElmb + fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_CONFIG_NAME +
                  fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_PREFIX + "Bytes." +
                  ELMB_DO_PREFIX + "A" + "_read:_original.._invalid";
   sPortCInvalid = sElmb + fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_CONFIG_NAME +
                  fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_PREFIX + "Bytes." +
                  ELMB_DO_PREFIX + "C" + "_read:_original.._invalid";
   dpSetWait(sPortAInvalid, false, sPortCInvalid, false);   
 */
 dyn_string ds=strsplit(s,";");  
  int bit=ds[2];
  string port=ds[1];
  int current;
  dpGet(sElmb+"/DO/do_Bytes.do_"+port+"_read",current);
  current = current & (1 << bit);
  dpSetWait(sElmb+"/DO/do_Bytes.do_"+port+"_read",current);
   
   
}


//============================================================================================

mudcsCrbCommandCrb(string fsm, bool isOn){
  
string  sTest;

string sElmb, sBit, sBit_id;
dyn_string argdsExceptionInfo;

dyn_string dsPairs, dsRest, dsOffChannels,switch_list;
dyn_int off_channels;

int factor_switch=1;
if(new_lv_power_up_sequence){
 if(!isOn)factor_switch=10;
}
int i,j;

int delay_all, delay_s, delay_ms;
 
dsOffChannels=makeDynString();


//==========================================
bool single_crb_switch=false;
if(CSC_fwCAN1_g_SINGLE_CRB_SWITCH==1){
  single_crb_switch=true;
  dpSetWait("SINGLE_CRB_SWITCH.",0); // return to 0
}
//==========================================

  
i=dynContains(CSC_fwCAN1_g_CRB_DPS, fsm);
if(single_crb_switch){
  DebugTN("NO DELAY: SINGLE_CRB_SWITCH = TRUE");
}
else if(i>=1){
  if(CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i] !=0 ){
    delay_all=CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]/factor_switch;
    delay_s=delay_all/1000;
    delay_ms=delay_all%1000;
//    delay(0,(CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]/factor_switch));
    delay(delay_s,delay_ms);
  }
         DebugTN("PUT DELAY");
         DebugTN(fsm +" delay="+CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]+" "+delay_s+" "+delay_ms);
       
         }
else{
  DebugTN("PUT DELAY NOT FOUND");
}
//========================================
if(automatic_stop_pcrate_slow_control){
  if(isOn)mudcsCrb_stop_slow_control(fsm);
  if(!isOn) {
    mudcsCrb_remove_id(fsm);
    mudcsCrb_sendToX2P("CRATEOFF", fsm);
  }
}  
//=========================================
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
if(isOn)isOn=false;
else isOn=true;

dpGet(fsm+".switch_list",switch_list);
dpGet(fsm+".off_channels",off_channels);

if(is_off_disabled_channels){
for(i=1;i<=dynlen(off_channels);i++)dsOffChannels[i]=off_channels[i];
}
sTest=substr(switch_list[1],0,strpos(switch_list[1],"/do_"));    
sElmb=substr(switch_list[1],0,strpos(switch_list[1],"/DO/"));  

bool inv;
//dpGet(sElmb+"/DO/do_Bytes.do_C_read:_online.._invalid",inv);
dpGet(sElmb+"/AI/ai_0.value:_online.._invalid",inv);
if(inv){DebugTN(sElmb+"/AI/ai_0.value "+"inv=true");return;}
else DebugTN("inv=false");
dynClear(switch_list);

//CSC_fwCAN1_g_NEW_ELMB_SWITCH

sBit=sTest+"/do_C_0";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit); 
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_C_1";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_C_2";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_C_3";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_C_4";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_C_5";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_C_6";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_C_7";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_A_0";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);

sBit=sTest+"/do_A_1";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsRest,sBit);
sBit=sTest+"/do_A_2";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsRest,sBit);
sBit=sTest+"/do_A_3";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsRest,sBit);
sBit=sTest+"/do_A_4";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsRest,sBit);
//-----------------------------------------------
//==== reserved: to be commented out old power up sequence ====
if(!new_lv_power_up_sequence){
for(j=1;j<=4;j++){

for(i=1;i<=dynlen(dsPairs);i++){  
  if(dynContains(dsOffChannels,i))continue;

  if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsPairs[i],isOn,argdsExceptionInfo); 
  else dpSet(dsPairs[i]+".value",isOn);
  
}
//----------------------------------------
for(i=1;i<=3;i++){  
  if(dynContains(dsOffChannels,dynlen(dsPairs)+i))continue;
  
  if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[i],isOn,argdsExceptionInfo); 
  else dpSet(dsRest[i]+".value",isOn); 
}
//---------------------------------------
  if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[4],false,argdsExceptionInfo); 
  else dpSet(dsRest[4]+".value",false); // clock 
  
delay(0,100); DebugTN(" "+fsm+" Clock1 "+100+" attempt "+j);

  if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[4],true,argdsExceptionInfo); 
  else dpSet(dsRest[4]+".value",true); // clock 
} // j
} // if(!new_lv_power_up_sequence)
//==== end of reserved: to be commented out old power up sequence ====

//===== reserved new power up sequence ========
//====== test 33 ms first (as a clock) to use many attempts 
if(new_lv_power_up_sequence){
int n_att=1;
if(g_attempts)n_att=3;
    
if(isOn==true){ // switch OFF

for(j=1;j<=4;j++){

for(i=1;i<=dynlen(dsPairs);i++){  
  if(dynContains(dsOffChannels,i))continue;
  
  if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsPairs[i],isOn,argdsExceptionInfo);  
  else dpSet(dsPairs[i]+".value",isOn); 
}
//----------------------------------------
for(i=1;i<=3;i++){  
  if(dynContains(dsOffChannels,dynlen(dsPairs)+i))continue;
  
  if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[i],isOn,argdsExceptionInfo);   
  else dpSet(dsRest[i]+".value",isOn); 
}
//---------------------------------------
 if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[4],false,argdsExceptionInfo);
 else dpSet(dsRest[4]+".value",false); // clock 
delay(0,100); DebugTN(" "+fsm+" Clock2 "+100+" attempt "+j);
 if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[4],true,argdsExceptionInfo);
 else dpSet(dsRest[4]+".value",true); // clock 
} // j

} // 
else if(isOn==false){ // switch ON

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

//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================

int nloop1, nloop2;
if(!new_lv_power_up_sequence_2){
 nloop1=2;
 nloop2=1; 
}
else{
 nloop1=1;
 nloop2=2;  
}

int jj;
for(j=1;j<=n_att;j++){
for(i=1;i<=nloop1;i++){
 
if(!new_lv_power_up_sequence_2){   
  if(i==1)jj=3; // 1.5
  else if(i==2)jj=1; // MPC/CCB
  //else if(i==3)jj=2; // VCC
}
else{
   if(i==1)jj=3; // 1.5  
}
    
  if(dynContains(dsOffChannels,dynlen(dsPairs)+jj))continue;

  if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[jj],isOn,argdsExceptionInfo);  
  else dpSet(dsRest[jj]+".value",isOn);

if(!new_lv_power_up_sequence_2){  
 if(i==2)interval=150/n_att; // 1 is right! because this is clock (not just delay)
 else interval=100/n_att;
}
else{
  if(!new_lv_power_up_sequence_test){  
   interval=100/n_att;
  }
  else{
   interval=3000/n_att;  
    
  }
}

 if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[4],false,argdsExceptionInfo);
 else dpSet(dsRest[4]+".value",false); // clock 
 
 delay(0,interval); DebugTN(" "+fsm+" Clock3 "+interval+" board_cc "+jj);
 
 if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[4],true,argdsExceptionInfo); 
 else dpSet(dsRest[4]+".value",true); // clock 

} // i
} // j
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================

interval=100;
interval=interval/n_att;

for(j=1;j<=n_att;j++){ 
for(i=1;i<=dynlen(dsPairs);i++){
  
  if(dynContains(dsOffChannels,i))continue;
  
  if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsPairs[i],isOn,argdsExceptionInfo);  
  else dpSet(dsPairs[i]+".value",isOn); 
  
if(!new_lv_power_up_sequence_2){  
 interval=100/n_att;  
}
else{
 if(!new_lv_power_up_sequence_test){  
  if(i==1)interval=150/n_att;
/////  else if(i==dynlen(dsPairs))interval=250/n_att;
  else interval=100/n_att;
 }
 else{
   interval=3000/n_att;     
   
 }
}  

 if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[4],false,argdsExceptionInfo);
 else dpSet(dsRest[4]+".value",false); // clock 
 
 delay(0,interval); DebugTN(" "+fsm+" Clock4 "+interval+" board "+i);

 if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[4],true,argdsExceptionInfo); 
 else dpSet(dsRest[4]+".value",true); // clock

 
 
}
} // j 
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
for(j=1;j<=n_att;j++){
for(i=1;i<=nloop2;i++){
  
  //if(i==1)jj=3; // 1.5
  //else if(i==2)jj=1; // MPC/CCB
  //else if(i==3)jj=2; // VCC
if(!new_lv_power_up_sequence_2){  
  if(i==1)jj=2; // VCC
}
else{
  if(i==1)jj=2; // VCC
  else if(i==2)jj=1; // MPC/CCB
}
    
  if(dynContains(dsOffChannels,dynlen(dsPairs)+jj))continue;
  
  if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[jj],isOn,argdsExceptionInfo);  
  else dpSet(dsRest[jj]+".value",isOn);
  
if(!new_lv_power_up_sequence_2){
 interval=100/n_att;
}
else{
 if(!new_lv_power_up_sequence_test){
  if(i==1)interval=250/n_att;
  else interval=100/n_att;
 }
 else{
   interval=3000/n_att;
 }
} 
 
//if(jj==2)interval=150/n_att; // 2 is right! because this is clock (not just delay)
//else interval=100/n_att;

 if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[4],false,argdsExceptionInfo);
 else dpSet(dsRest[4]+".value",false); // clock 
 
 delay(0,interval); DebugTN(" "+fsm+" Clock5 "+interval+" board_cc "+jj);
 
 if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[4],true,argdsExceptionInfo); 
 else dpSet(dsRest[4]+".value",true); // clock 

} // i
} // j
//===================
interval=100;


}//if(isOn==false)

        dpSetWait("LV_START_UP_CHAMBER.",fsm+" is finished");
} //if(new_lv_power_up_sequence)
//==============================================================
//==============================================================
//==============================================================
DebugTN(" "+fsm+" Time Measurement Crb");
}

//============================================================================================
mudcsCrbCommandCrbGroup(string fsm, bool isOn, dyn_int boards){
  
string  sTest;


string sElmb, sBit, sBit_id;
dyn_string argdsExceptionInfo;

dyn_string dsPairs, dsRest, dsOffChannels,switch_list;
dyn_int off_channels;

int i,j;
    
if(isOn)isOn=false;
else isOn=true;

dsOffChannels=makeDynString();
//========================================
if(automatic_stop_pcrate_slow_control){
  if(isOn)mudcsCrb_stop_slow_control(fsm);
  delay(5);
}  
//========================================

dpGet(fsm+".switch_list",switch_list);
dpGet(fsm+".off_channels",off_channels);

if(is_off_disabled_channels){
for(i=1;i<=dynlen(off_channels);i++)dsOffChannels[i]=off_channels[i];
}

sTest=substr(switch_list[1],0,strpos(switch_list[1],"/do_"));    
sElmb=substr(switch_list[1],0,strpos(switch_list[1],"/DO/"));  

dynClear(switch_list);

//CSC_fwCAN1_g_NEW_ELMB_SWITCH

sBit=sTest+"/do_C_0";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit); 
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_C_1";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_C_2";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_C_3";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_C_4";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_C_5";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_C_6";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_C_7";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);
sBit=sTest+"/do_A_0";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsPairs,sBit);

sBit=sTest+"/do_A_1";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsRest,sBit);
sBit=sTest+"/do_A_2";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsRest,sBit);
sBit=sTest+"/do_A_3";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsRest,sBit);
sBit=sTest+"/do_A_4";
if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)dpGet(sBit+".id",sBit);
dynAppend(dsRest,sBit);
//-----------------------------------------------  
//-----------------------------------------------

for(j=1;j<=4;j++){

for(i=1;i<=dynlen(dsPairs);i++){  
  if(dynContains(dsOffChannels,i)>=1)continue;
  if(dynContains(boards,i)>=1){
    
    if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsPairs[i],isOn,argdsExceptionInfo);    
    else dpSet(dsPairs[i]+".value",isOn); 
    
  }
}
//----------------------------------------
for(i=1;i<=3;i++){  
  if(dynContains(dsOffChannels,dynlen(dsPairs)+i)>=1)continue;
  if(dynContains(boards,dynlen(dsPairs)+i)>=1){
    
    if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[i],isOn,argdsExceptionInfo);    
    else dpSet(dsRest[i]+".value",isOn); 
    
  }
}
//---------------------------------------
 if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[4],false,argdsExceptionInfo);
 else dpSet(dsRest[4]+".value",false); // clock 
 
 delay(0,100);
 
 if(CSC_fwCAN1_g_NEW_ELMB_SWITCH)mudcs_fwElmbUser_setDoBit(sElmb,dsRest[4],true,argdsExceptionInfo);  
 else dpSet(dsRest[4]+".value",true); // clock 

} //j
 
}

//============================================================================================
update_confirmation(string dpName, string value){ // x2p
  
  int i;
  string remote_system;
  dyn_string ds1,ds2;
  ds1=strsplit(dpName,":");
  remote_system=ds1[1];
  ds1=strsplit(value,";");
  string coord, last_command, id_s;
  int coord_int, coord_int_receive;
  int minus_len=0, plus_len=0;
string special_stop_value;
  
  dpGet(remote_system+":STOP_SLOW_CONTROL.value",last_command);
  dpGet(remote_system+":SPECIAL_STOP_SC.value",special_stop_value);//   
  //====
  DebugTN(value);
  DebugTN(remote_system+" "+dpName);
  DebugTN(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);
  //DebugTN();
  //=====
 if((dynlen(ds1)==2 && ds1[1]=="INIT_IS_DONE")){
    if(strpos(ds1[2],"VME")>=0)id_s=mudcsCrb_getIdByVme(ds1[2] );
    else id_s=ds1[2];
    coord_int_receive=id_s;//ds1[2];
    dynUnique(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);
    
  int ind=dynContains(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER,id_s);//ds1[2]);
  if(ind>0){
  dynRemove(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER, ind);
  for(i=1;i<=dynlen(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);i++){
    coord_int=CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i];
    if(coord_int<=30)plus_len++;
    else minus_len++;
  }
 if(special_stop_value != "STOP"){ 
  if(coord_int_receive <=30 && remote_system==CSC_fwCAN1_g_PLUS_SYSTEM_NAME && plus_len==0 && last_command !="STOP_SLOW_CONTROL"){
     dpSetWait(remote_system+":STOP_SLOW_CONTROL.value","RESUME_SLOW_CONTROL");
     dpSetWait(remote_system+":LV_1_COM.command","RESUME_SLOW_CONTROL");     
  }
  if(coord_int_receive > 30 && remote_system==CSC_fwCAN1_g_MINUS_SYSTEM_NAME && minus_len==0 && last_command !="STOP_SLOW_CONTROL"){
     dpSetWait(remote_system+ ":STOP_SLOW_CONTROL.value","RESUME_SLOW_CONTROL");
     dpSetWait(remote_system+":LV_1_COM.command","RESUME_SLOW_CONTROL");     
  }
 }//  if(special_stop_value != "STOP")
   
  ///if(dynlen(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER)==0)dynAppend(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER,"done");  
  dpSetWait(mudcsLvAddSystem("PREPARE_POWER_UP_BUFFER."),CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER); 
  
  }
 }
 else{
   // heart beat command || start command
  if(strpos(value,"START")>=0){   

   if(last_command=="RESUME_SLOW_CONTROL"){
    dpSetWait(remote_system+":LV_1_COM.command","RESUME_SLOW_CONTROL");
   }
   else if(last_command != "STOP_SLOW_CONTROL"){
     
     if(value=="SOFT_START"){  
    for(i=1;i<=dynlen(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);i++){
     ds2=strsplit(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i],";");
     //if(dynlen(ds2)<2)continue;
     coord_int=CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i];//ds2[2];
     coord=CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i];//ds2[2];
     string vme=mudcsCrb_getVmeById(coord);
     if(coord_int <=30 && remote_system==CSC_fwCAN1_g_PLUS_SYSTEM_NAME){ // <=1f
      dpSetWait(remote_system+":STOP_SLOW_CONTROL.value","PREPARE_POWER_UP;"+vme);
      dpSetWait(remote_system+":LV_1_COM.command","PREPARE_POWER_UP;"+vme);
     }
     else if(coord_int >30 && remote_system==CSC_fwCAN1_g_MINUS_SYSTEM_NAME){
      dpSetWait(remote_system+":STOP_SLOW_CONTROL.value","PREPARE_POWER_UP;"+vme);
      dpSetWait(remote_system+":LV_1_COM.command","PREPARE_POWER_UP;"+vme);       
     }
    } //for 
     }
     else if(value=="HARD_START"){
       mudcsCrb_lv_buffer_power_cycle();
     }
   } // else
   else if(last_command=="STOP_SLOW_CONTROL"){
      dpSetWait(remote_system+":STOP_SLOW_CONTROL.value","STOP_SLOW_CONTROL");
      dpSetWait(remote_system+":LV_1_COM.command","STOP_SLOW_CONTROL");     
   }
  } // if "START_"
 } //else
  
}

//============================================================================================

//===================================================
mudcsCrb_remove_id(string fsm){ // x2p
  

  
  if(two_way_communications_x2p){
   string coord;
   int coord_int;  

  dpGet(fsm+".coord",coord); 
  sscanf(coord,"%x",coord_int);
 // dynUnique(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER); 
 // int ind=dynContains(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER,coord_int);
 // if(ind>=1)dynRemove(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER, ind);
 // dpSetWait(mudcsLvAddSystem("PREPARE_POWER_UP_BUFFER."),CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);
  
  string remote_system;
  if(coord_int <=30) remote_system=CSC_fwCAN1_g_PLUS_SYSTEM_NAME;
  else remote_system=CSC_fwCAN1_g_MINUS_SYSTEM_NAME;
  update_confirmation(remote_system+":any", "INIT_IS_DONE;"+coord_int);
  }
  
}
//========================================

mudcsCrb_stop_slow_control(string fsm){ // x2p
  
  string coord, previous_command; 
 
  
  if(two_way_communications_x2p){
    int coord_int;
         
  DebugTN("debug_stop_slow_control "+fsm);
  DebugTN("debug_stop_slow_control "+CSC_fwCAN1_g_PLUS_SYSTEM_NAME);
  string stop_value;
  string remote_system;
  if(strpos(fsm,"ME_M")>=0)remote_system=CSC_fwCAN1_g_MINUS_SYSTEM_NAME;
  else if(strpos(fsm,"ME_P")>=0)remote_system=CSC_fwCAN1_g_PLUS_SYSTEM_NAME;

  dpGet(fsm+".coord",coord); 
  sscanf(coord,"%x",coord_int);

  dpSetWait(remote_system+":STOP_SLOW_CONTROL.value","PREPARE_POWER_UP;"+coord_int);
//  dpSetWait(remote_system+":LV_1_COM.command","PREPARE_POWER_UP;"+coord_int); 
  dynAppend(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER,coord_int);
  dynUnique(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);   
//  dpSetWait(mudcsLvAddSystem("PREPARE_POWER_UP_BUFFER."),CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);  
  
  } 
  else{
    
  DebugTN("debug_stop_slow_control "+fsm);
  DebugTN("debug_stop_slow_control "+CSC_fwCAN1_g_PLUS_SYSTEM_NAME);
  string stop_value;
  string remote_system;
  if(strpos(fsm,"ME_M")>=0)remote_system=CSC_fwCAN1_g_MINUS_SYSTEM_NAME;
  else if(strpos(fsm,"ME_P")>=0)remote_system=CSC_fwCAN1_g_PLUS_SYSTEM_NAME;
    
  dpSetWait(remote_system+":STOP_SLOW_CONTROL.value","STOP_SLOW_CONTROL_LV");
  dpSetWait(remote_system+":LV_1_COM.command","STOP_SLOW_CONTROL");     
    
  }
}
//===========================================
string mudcsCrb_getVmeById(string id){
int i;
dyn_string dps, dps1;
int coord_int, pos;
string coord_s;
for(i=1;i<=dynlen(CSC_fwCAN1_g_DB_PCMB);i++){
 dps=strsplit(CSC_fwCAN1_g_DB_PCMB[i],";");
 sscanf(dps[2],"%x",coord_int);
 coord_s=coord_int;
 if(id==coord_int){
  dps1=strsplit(CSC_fwCAN1_g_DB_PCMB_GEOGRAPHIC[i],";");
  return "VME"+strtolower(substr(dps[1],0,2))+"_"+dps1[2];
 }
}  // i 

 return "";
}

//===========================================
string mudcsCrb_getIdByVme(string vme){
  
int i;
dyn_string dps, dps1;
int coord_int, pos;
string coord_s, vme_s;
for(i=1;i<=dynlen(CSC_fwCAN1_g_DB_PCMB);i++){
 dps=strsplit(CSC_fwCAN1_g_DB_PCMB[i],";");
 dps1=strsplit(CSC_fwCAN1_g_DB_PCMB_GEOGRAPHIC[i],";");
 if(dynlen(dps)<2 || dynlen(dps1)<2)continue;
 vme_s="VME"+strtolower(substr(dps[1],0,2))+"_"+dps1[2];
 
 if(vme==vme_s){
  sscanf(dps[2],"%x",coord_int);
  coord_s=coord_int;   
  return coord_s;
 }
}  // i   
  
 return "";
}
//==========================================
mudcsCrb_power_up_packet(string remote_system){
  int i,status, coord_int;
  string coord;
  dyn_string dps;
  dyn_string error_buffer, final_buffer;
  dps=dpNames("*","fwCrb_CSC_LV");

  for(i=1;i<=dynlen(dps);i++){
   dpGet(dps[i]+".status",status);
   DebugTN(dps[i]+" "+status);
   if(status<=0){ // including OFF state
    dpGet(dps[i]+".coord",coord); 
    sscanf(coord,"%x",coord_int);
    dynAppend(error_buffer,coord_int);
   }
  } // for
  //-----
  for(i=1;i<=dynlen(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);i++){
    
    coord_int=CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i];    
    if(coord_int<=30 && remote_system==CSC_fwCAN1_g_MINUS_SYSTEM_NAME)continue;
    if(coord_int>30 && remote_system==CSC_fwCAN1_g_PLUS_SYSTEM_NAME)continue;
    
  //\\if(!dynContains(error_buffer,CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i])){ 
    dynAppend(final_buffer,CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i]);

    string vme=mudcsCrb_getVmeById(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i] );
    dpSetWait(remote_system+":LV_1_COM.command","PREPARE_POWER_UP;"+vme);
    DebugTN(remote_system+":LV_1_COM.command  "+"PREPARE_POWER_UP;"+vme);
  //\\ } // if(!dynContains)
  } // for
  //CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER=final_buffer;
  DebugTN(remote_system);
  DebugTN(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);
  dpSetWait(mudcsLvAddSystem("PREPARE_POWER_UP_BUFFER."),CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);
  
}
//========================================

mudcsCrb_resume_slow_control(){ //x2p
  
  int i;
  string stop_value, special_stop_value;
  time stop_value_time, current_time;
  string remote_system;
 
  if(two_way_communications_x2p){  
  
  current_time=getCurrentTime();
     DebugTN(CSC_fwCAN1_g_MINUS_SYSTEM_NAME+","+CSC_fwCAN1_g_PLUS_SYSTEM_NAME);
  for(i=1;i<=2;i++){
   if(i==1)remote_system=CSC_fwCAN1_g_MINUS_SYSTEM_NAME;
   else if(i==2)remote_system=CSC_fwCAN1_g_PLUS_SYSTEM_NAME;
   dpGet(remote_system+":STOP_SLOW_CONTROL.value",stop_value);//"STOP_SLOW_CONTROL_LV"
   dpGet(remote_system+":STOP_SLOW_CONTROL.value:_online.._stime",stop_value_time);
   if(strpos(stop_value,"PREPARE_POWER_UP")>=0){
    if((current_time-stop_value_time) > 15){
     mudcsCrb_power_up_packet(remote_system);
     dpSet(remote_system+":STOP_SLOW_CONTROL.value","EXECUTE_POWER_UP");
     dpSet(remote_system+":LV_1_COM.command","EXECUTE_POWER_UP");       
    }
   }//if
  } // for
  
  }
  else{
     
  current_time=getCurrentTime();
    
  for(i=1;i<=2;i++){
   if(i==1)remote_system=CSC_fwCAN1_g_MINUS_SYSTEM_NAME;
   else if(i==2)remote_system=CSC_fwCAN1_g_PLUS_SYSTEM_NAME;
   dpGet(remote_system+":STOP_SLOW_CONTROL.value",stop_value);//"STOP_SLOW_CONTROL_LV"
   dpGet(remote_system+":SPECIAL_STOP_SC.value",special_stop_value);//   
   dpGet(remote_system+":STOP_SLOW_CONTROL.value:_online.._stime",stop_value_time);
   if(stop_value == "STOP_SLOW_CONTROL_LV" && special_stop_value != "STOP"){
    if((current_time-stop_value_time) > 3){
     dpSet(remote_system+":STOP_SLOW_CONTROL.value","RESUME_SLOW_CONTROL");
     dpSet(remote_system+":LV_1_COM.command","RESUME_SLOW_CONTROL");       
    }
   }//if
  } // for    
  
    
  }
}
//==========================================
//========================================

mudcsCrb_lv_buffer_power_cycle(){ //x2p
  
  //===========================================================

  
 
//int mode_action;
//int mode;
  
int i;  
dyn_string ds, ds1, ds2, split, prepare_power_up_buffer;
int coord_int, ind;
string coord_s;

dpGet("db_pcmb.list",ds);
//dpGet("PREPARE_POWER_UP_BUFFER.",prepare_power_up_buffer);
prepare_power_up_buffer=CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER;

//ds=dpNames("*","fwCrb_CSC_LV");
 for(i=1;i<=dynlen(ds);i++){
    
    split=strsplit(ds[i],";");
    sscanf(split[2],"%x",coord_int);
    coord_s=coord_int;
    ind=dynContains(prepare_power_up_buffer,coord_s);
    DebugTN(ind);
    if(ind>=1){
      ds1=dpNames("*"+split[1]+"*","fwCrb_CSC_LV");
      dynAppend(ds2,ds1[1]);
      
    }
      
 }
 DebugTN(ds2);
 
 for(i=1;i<=dynlen(ds2);i++){
   dpSet(ds2[i]+".status",0);   
 } 
 delay(10);
 for(i=1;i<=dynlen(ds2);i++){
   dpSet(ds2[i]+".status",2);   
 }  
 
  
}

//====================================================
mudcsCrbGetDmbTempAlerts(string DpName, dyn_string &ds_alerts, dyn_int &alert_channels, dyn_string &fsms){
 
   int i,p1,p2, t_status;
  string dp_template, test_string, remote_system, fsm ;
  dyn_string dps;
  dyn_float temps;
  string alertClass;
  dyn_string ds_alerts_notordered, split1, split2;
  dyn_int off_channels;
  
 dynClear(alert_channels);  
  
 p1=strpos(DpName,"ME_");
 p2=strpos(DpName,"_LV");
 if(p1>=0 && p2>=0){
   dp_template=substr(DpName,p1+3,p2-(p1+3));  
 }
 dps=dpNames("*:"+dp_template,"fwLvCr_CSC_TEMP");
 if(dynlen(dps)<=0)return;
 dpGet(dps[1]+"."+":_alert_hdl.._dp_list",ds_alerts_notordered);
 /////dpGet(ds_alerts, temps);
 
 for(i=1;i<=dynlen(ds_alerts_notordered);i++){
  if(strpos(dp_template,"PC6")>=0 && strlen(dp_template)==6){
   if(i==1)ds_alerts[3]=ds_alerts_notordered[i]; 
   else if(i==2)ds_alerts[1]=ds_alerts_notordered[i];
   else if(i==3)ds_alerts[2]=ds_alerts_notordered[i];
   else if(i==4)ds_alerts[8]=ds_alerts_notordered[i];
   else if(i==5)ds_alerts[9]=ds_alerts_notordered[i];
   else if(i==6)ds_alerts[4]=ds_alerts_notordered[i]; 
   else if(i==7)ds_alerts[5]=ds_alerts_notordered[i];  
   else if(i==8)ds_alerts[6]=ds_alerts_notordered[i];  
   else if(i==9)ds_alerts[7]=ds_alerts_notordered[i];                       
  }  
  else if(strpos(dp_template,"PC6B")>=0){
   if(i==1)ds_alerts[2]=ds_alerts_notordered[i]; 
   else if(i==2)ds_alerts[3]=ds_alerts_notordered[i];
   else if(i==3)ds_alerts[1]=ds_alerts_notordered[i];
   else if(i==4)ds_alerts[5]=ds_alerts_notordered[i];
   else if(i==5)ds_alerts[6]=ds_alerts_notordered[i];
   else if(i==6)ds_alerts[4]=ds_alerts_notordered[i]; 
   else if(i==7)ds_alerts[8]=ds_alerts_notordered[i];  
   else if(i==8)ds_alerts[9]=ds_alerts_notordered[i];  
   else if(i==9)ds_alerts[7]=ds_alerts_notordered[i]; 
  }
  else{
   ds_alerts[i]=ds_alerts_notordered[i];  
  }
 } //i
 

 
 for(i=1;i<=dynlen(ds_alerts);i++){ 
   split1=strsplit(ds_alerts[i],":");
   remote_system=split1[1];
   split2=strsplit(split1[2],".");
   fsm=remote_system+":"+"Temperature/"+split2[1];
   dynAppend(fsms,fsm);
   dpGet(fsm+".status",t_status);
   if(t_status==-2)continue;
  dpGet( fsm+".off_channels",off_channels);
  if(dynContains(off_channels,7))continue;
  dpGet(ds_alerts[i]+":_alert_hdl.._act_state_color",alertClass);
  if(alertClass == "FwAlarmErrorAck")dynAppend(alert_channels,i);
 }   
  
//  DebugTN(DpName+" "+dp_template);
//  DebugTN(ds_alerts_notordered);
//   DebugTN(ds_alerts);
//   DebugTN(alert_channels);
 
}
//MAPPING_VAR

// ----==== Evaldas code ====----

/**
  * Sends the given command for the given crate to X2P.
  * @param command command you wish to send.
  * @param crateFsmDp FSM DP of the crate for which the command is dedicated.
  */
void mudcsCrb_sendToX2P(string command, string crateFsmDp) {
  if(two_way_communications_x2p){
    int crateId = mudcsCrb_getCrateId(crateFsmDp);
    string midLayerSystem = mudcsCrb_getMidLayerSystemForCrateId(crateId);
    string crateVmeId = mudcsCrb_getVmeById(crateId);
    dpSetWait(midLayerSystem + ":LV_1_COM.command", command + ";" + crateVmeId);
    DebugTN("Sent command to X2P: " + command + ";" + crateVmeId);
  }
}

/**
  * Get numeric crate ID.
  * @param crateFsmDp FSM DP of the crate for which you want to obtain the ID.
  * @return numeric crate ID.
  */
int mudcsCrb_getCrateId(string crateFsmDp) {
  string coord;
  int coord_int;  

  dpGet(crateFsmDp + ".coord", coord); 
  sscanf(coord, "%x", coord_int);
  
  return coord_int;
}

/**
  * Get the corresponding middle layer system name for a given crate (identified by numeric ID).
  * @param numeric ID of the crate for which you want to look up middle layer sys. name.
  * @return the corresponding middle layer system name for the given crate.
  */
string mudcsCrb_getMidLayerSystemForCrateId(int crateId) {
  if(crateId <=30) {
    return CSC_fwCAN1_g_PLUS_SYSTEM_NAME;
  } else {
    return CSC_fwCAN1_g_MINUS_SYSTEM_NAME;
  }
}

/**
  * Get the corresponding middle layer system name for a given crate (identified by FSM DP).
  * @param FSM DP of the crate for which you want to look up middle layer sys. name.
  * @return the corresponding middle layer system name for the given crate.
  */
string mudcsCrb_getMidLayerSystemForCrateFsmDp(int crateFsmDp) {
  return mudcsCrb_getMidLayerSystemForCrateId(mudcsCrb_getCrateId(crateFsmDp));
}
