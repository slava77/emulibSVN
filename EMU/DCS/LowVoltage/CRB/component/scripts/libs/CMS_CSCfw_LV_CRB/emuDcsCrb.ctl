#uses "CMS_CSCfw_LV_CRB/emuLvCRB.ctl" 
#uses "CMS_CSCfw_LV_CRB/emuDcsLv.ctl"  
int new_lv_power_up_sequence=true;
int g_attempts=false;
int interval_set = 150;  //change back to 150 ms
bool new_lv_power_up_sequence_2=true;
bool new_lv_power_up_sequence_test=false;
bool automatic_stop_pcrate_slow_control=true;
bool two_way_communications_x2p=true;
bool is_off_disabled_channels=false;

// patched

//=================================================================================
void mudcsCrbDistConfig()
{  
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
//============set a global var "CSC_fwCAN1_g_CRB_DPS" as dpNames of Crbs==================
//=====set a global var ""CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY" as general switch delays=====
//=====================used as additional init for Vserver================================
//=================================modified by X.Yang=====================================
void mudcsCrbInitDelays(int mode)
{   
 int i,j=0;
 dyn_string dpp,dpm,dpall;
//----get crb dpnames for P5 project--------------------------------- 
 dpp=dpNames(CSC_fwCAN1_g_SYSTEM_NAME+":*ME_P*","fwCrb_CSC_LV");
 dpm=dpNames(CSC_fwCAN1_g_SYSTEM_NAME+":*ME_M*","fwCrb_CSC_LV");
 dpall=dpp;     
 dynAppend(dpall,dpm); //plus side first 
//----get crb dpnames for B904 project------------------------------ 
 if(CSC_fwCAN1_g_DEV_MODE)
  { // corr++ for B904
   dpall= dpNames(CSC_fwCAN1_g_SYSTEM_NAME+":*","fwCrb_CSC_LV");
  } 
 addGlobal("CSC_fwCAN1_g_CRB_DPS", DYN_STRING_VAR);  
 CSC_fwCAN1_g_CRB_DPS=dpall;
//----set delay time for Crb---------------------------------------- 
 addGlobal("CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY", DYN_INT_VAR);
 for(i=1;i<=dynlen(CSC_fwCAN1_g_CRB_DPS);i++)
  {
   if(strpos(CSC_fwCAN1_g_CRB_DPS[i],"ME_M")>=0)
     {j++;}  
   if(mode==0)
     {  
      CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=0; //set delay to 0
     } 
   else 
     {        
       if(!new_lv_power_up_sequence_test) //new_lv_power_up_sequence_test is false
             {
               //if(j>0)CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=5000+(j-1)*1450; // in milliseconds,for crate at minus side,5s delay for first crate 
               //else CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=5000+(i-1)*1450; // in milliseconds, for crate at plus side,5s delay for first crate
               if(j>0)CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=5000+(j-1)*9000; // in milliseconds,for crate at minus side,5s delay for first crate 
               else CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=5000+(i-1)*9000; // in milliseconds, for crate at plus side,5s delay for first crate
             }
       else
             {
               if(j>0)CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=5000+(j-1)*42000; // in milliseconds
               else CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]=5000+(i-1)*42000; // in milliseconds         
             }         
     }     
  } //for i  
}
//================create global vars for CRB project===============================
void mudcsCrbPnlInit(int level=1)
{
  int i,n;
  dyn_string ds1,ds2,ds3;
  string coord;
 
  addGlobal("CSC_fwCAN1_g_NEW_ELMB_SWITCH", BOOL_VAR);
  CSC_fwCAN1_g_NEW_ELMB_SWITCH=true;
  
  addGlobal("CSC_fwCAN1_g_DB_PCMB", DYN_STRING_VAR);
  dpGet("db_pcmb.list",CSC_fwCAN1_g_DB_PCMB); 
  
  addGlobal("CSC_fwCAN1_g_DB_PCMB_GEOGRAPHIC", DYN_STRING_VAR);
  dpGet("db_pcmb_geographic.list",CSC_fwCAN1_g_DB_PCMB_GEOGRAPHIC);
  
  addGlobal("CSC_fwCAN1_g_P5_SIM", BOOL_VAR); // corr++
  CSC_fwCAN1_g_P5_SIM=false;  // to set P5 simulation mode
     
  addGlobal("CSC_fwCAN1_g_DEV_MODE", BOOL_VAR); // corr++
  CSC_fwCAN1_g_DEV_MODE=false; // to set 904 only real nodes mode

  addGlobal("CSC_fwCAN1_g_904_MACHINE", BOOL_VAR); // corr++
  CSC_fwCAN1_g_904_MACHINE=false;

  string hostname1=getHostname();
  if((strpos(hostname1, "EMU-DCS-DEV") >= 0) || (strpos(hostname1, "emu-dcs-dev") >= 0))
    { //  DEV machine  // corr++
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
  CSC_fwCAN1_g_ISACK=false;
  
  addGlobal("CSC_fwCAN1_g_CANBUSES", DYN_STRING_VAR); 
  
  addGlobal("CSC_fwCAN1_g_csc_part", STRING_VAR);
  CSC_fwCAN1_g_csc_part="CMS_CSC_LV_CAN1";  

  string node_to_define_system_name=CSC_fwCAN1_g_csc_part;
  fwCU_getDp(node_to_define_system_name,node_to_define_system_name);
  
  dyn_string systemNamesDps,s_split;
  systemNamesDps=dpNames("*:CMS_CSC_LV_CRB");  
  if(dynlen(systemNamesDps)==0)mudcsLvDebug("SYSTEM NAME NOT DEFINED !");
  else
    {
      s_split=strsplit(systemNamesDps[1],":"); 
      CSC_fwCAN1_g_SYSTEM_NAME=s_split[1];//system name without ":"
    }
  if(automatic_stop_pcrate_slow_control)  //automatic_stop_pcrate_slow_control=true
    {  
     //------x2p------------
      if(two_way_communications_x2p)
        { 
         addGlobal("CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER", DYN_STRING_VAR); // x2p
         dpGet(mudcsLvAddSystem("PREPARE_POWER_UP_BUFFER."),CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER); //x2p 
        }    
      addGlobal("CSC_fwCAN1_g_MINUS_SYSTEM_NAME", STRING_VAR);
      addGlobal("CSC_fwCAN1_g_PLUS_SYSTEM_NAME", STRING_VAR);  

      int m_max=0;
      int p_min=10;
      string valid;    
      CSC_fwCAN1_g_PLUS_SYSTEM_NAME="";
      CSC_fwCAN1_g_MINUS_SYSTEM_NAME="";
      systemNamesDps=dpNames("*:CMS_CSC_HV_CC_*","MUDCS_STRING"); //looking for dps at middle layer projects
      emuLvCRB_showDebug(bDebug,"systemNamesDps="+systemNamesDps);
  
      for(i=1;i<=dynlen(systemNamesDps);i++)
        {
          dpGet(systemNamesDps[i]+".value",valid);
          if(valid !="valid")continue;    
          ds2=strsplit(systemNamesDps[i],":");
          ds3=strsplit(ds2[dynlen(ds2)],"_");
          n=ds3[dynlen(ds3)];
          if(n<=4 && n > m_max){CSC_fwCAN1_g_MINUS_SYSTEM_NAME=ds2[1];m_max=n;}
          else if(n>=5 && n < p_min){CSC_fwCAN1_g_PLUS_SYSTEM_NAME=ds2[1];p_min=n;}
        }   
    }   
//------- elmb ai channel--> voltage of module in crate------     
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
 
  if(level==0)return;      
//----- INIT PART------------------------------------
  CSC_fwCAN1_g_CANBUSES=dpNames(CSC_fwCAN1_g_SYSTEM_NAME+":*","FwElmbCANbus");
  for(i=1;i<=dynlen(CSC_fwCAN1_g_CANBUSES);i++)
   {
    dpGet(CSC_fwCAN1_g_CANBUSES[i]+".nodeGuardInterval",CSC_fwCAN1_g_nodeGuardInterval[i]);
   }  
  ds1=dpNames(CSC_fwCAN1_g_SYSTEM_NAME+":*","fwCrb_CSC_LV");  
  dynClear(CSC_fwCAN1_g_CRB2ELMB);
  for(i=1;i<=dynlen(ds1);i++)
   {  
    dpGet(ds1[i]+".coord",coord);
    dynAppend(CSC_fwCAN1_g_CRB2ELMB,ds1[i]+";"+coord);
   }  
  return;  
}
//========================================================================
int mudcsCrbGetChannelNumberByName(string dp)
{
  int iN; 
  dyn_string ds1,ds2;
  ds1=strsplit(dp,"/");
  ds2=strsplit(ds1[dynlen(ds1)],"_");
  iN=ds2[dynlen(ds2)];
  return iN;  
}
//==========switch on/off Crb with delay======================================================
//================modified by X.Yang==========================================================
void mudcsCrbCommandCrb(string fsm, bool isOn)
{  
  string sElmb, sBit, sBit_id,sTest;
  dyn_string dsPairs, dsRest,dsOffChannels,switch_list,argdsExceptionInfo;
  dyn_int off_channels;
  int factor_switch=1;
  int i,j,delay_all,delay_s,delay_ms,interval;
  
  if(new_lv_power_up_sequence)
   {
    if(!isOn)
     {factor_switch=10;}  //change back to 10 
   }
  dsOffChannels=makeDynString();
//--set "CSC_fwCAN1_g_SINGLE_CRB_SWITCH" to 0 by function "mudcsCrbUpdate_single_crb_switch"---
  bool single_crb_switch=false;
  if(CSC_fwCAN1_g_SINGLE_CRB_SWITCH==1)
    {
     single_crb_switch=true;
     dpSetWait("SINGLE_CRB_SWITCH.",0);
    }
//--------set delay time if not single switch--------------------------------------
  i=dynContains(CSC_fwCAN1_g_CRB_DPS, fsm);
  if(single_crb_switch)
    {
     DebugTN("NO DELAY: SINGLE_CRB_SWITCH = TRUE");
    }
  else if(i>=1)
    {
     if(CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i] !=0 )
      {
       delay_all=CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]/factor_switch;
       delay_s=delay_all/1000;
       delay_ms=delay_all%1000;
       delay(delay_s,delay_ms);
      }
     DebugTN("PUT DELAY");
     DebugTN(fsm +" delay="+CSC_fwCAN1_g_CRB_DPS_SWITCH_DELAY[i]+" "+delay_s+" "+delay_ms);       
    }
  else
    {
     DebugTN("PUT DELAY NOT FOUND");
    }
//-----------check slow control-----------------------------------------
  if(automatic_stop_pcrate_slow_control)
    {
     if(isOn)
       {mudcsCrb_stop_slow_control(fsm);}
     if(!isOn)
       {
        mudcsCrb_remove_id(fsm);
        mudcsCrb_sendToX2P("CRATE_POWER_OFF", fsm);
       }
    }    
//-------- isOn --> from ture to false or false to ture ---- 
  if(isOn)
     {isOn=false;}
  else 
     {isOn=true;}
//------------------------------------------------------
  dpGet(fsm+".switch_list",switch_list);
  dpGet(fsm+".off_channels",off_channels);

  if(is_off_disabled_channels)
     {
       for(i=1;i<=dynlen(off_channels);i++)
          {dsOffChannels[i]=off_channels[i];}
     }
  sTest=substr(switch_list[1],0,strpos(switch_list[1],"/do_"));    
  sElmb=substr(switch_list[1],0,strpos(switch_list[1],"/DO/"));  

  bool inv;
  dpGet(sElmb+"/AI/ai_0.value:_online.._invalid",inv);
  if(inv)
     {
       DebugTN(sElmb+"/AI/ai_0.value "+"inv=true");
       return;
     }
  else 
     {DebugTN("inv=false");}    
  
  dynClear(switch_list);  

//---get switch bit ID when CSC_fwCAN1_g_NEW_ELMB_SWITCH=ture----------  
//for  new_lv_power_up_sequence: switch on order: 1. 1.5v  2. 9 pairs of 3.3v-1,3.3v_2,5v
// 3. VCC  4. MPC/CCB after each switch on, make a power cycle for dsRest(4) as toggle  
    dsPairs = emuLvCRB_getDynString("Switch_List_DTmbs");
    dsRest  = emuLvCRB_getDynString("Switch_List_Others");
//---Power on/off crb (once only)---------------------------------------------  
  if(isOn==true)
        {  //---- actually switch OFF ---------------------------------
          interval = interval_set;
     /*
          for(j=1;j<=1;j++)
            {
              //---switch off 9 pair 3.3v_1/3.3v_2/5v------------
              for(i=1;i<=dynlen(dsPairs);i++)
                {  
                  if(dynContains(dsOffChannels,i))continue;  
                 
                  else emuLvCRB_setDoBitSync(sElmb,dsPairs[i],isOn,argdsExceptionInfo);  
                //  emuLvCRB_generateToggleA4(sElmb,interval);          
                }
               //---switch off 1.5v/ccb_mpc/vcc------------------
              for(i=1;i<=3;i++)
                {                  
                  if(dynContains(dsOffChannels,dynlen(dsPairs)+i))continue;  
                  else emuLvCRB_setDoBitSync(sElmb,dsRest[i],isOn,argdsExceptionInfo);
               //   emuLvCRB_generateToggleA4(sElmb,interval);
                } 
             }   
       */
          fwElmbUser_setDoBytes(sElmb,4095,argdsExceptionInfo);    
          emuLvCRB_generateToggleA4(sElmb,interval);            
          emuLvCRB_showDebug(bDebug,"switch off "+fsm+" with "+interval+"ms Clock");
           
          dpSetWait(fsm+".status",0); //set FSM state to OFF 
          DebugTN("Power off for Crb:"+fsm);
        } //----end switch OFF------------------------------------------------------- 
   else if(isOn==false)
        { //---- actually switch ON ------------------------------------------------- 
     time startT = getCurrentTime();
     DebugTN("switching on");
         for(int j=1;j<=1;j++)
         {         
          //---switch on 1.5v-----------------------
          if(dynContains(dsOffChannels,dynlen(dsPairs)+3))continue;
          else emuLvCRB_setDoBitSync(sElmb,dsRest[3],isOn,argdsExceptionInfo);
          interval = interval_set;
          emuLvCRB_generateToggleA4(sElmb,interval);         
          emuLvCRB_showDebug(bDebug,"switch on "+fsm+" board_cc 3 with "+interval+"ms clock");       
          //---switch on board 1-9-------------------
          for(i=1;i<=dynlen(dsPairs);i++)
                {  
                  if(dynContains(dsOffChannels,i))continue; 
                  else emuLvCRB_setDoBitSync(sElmb,dsPairs[i],isOn,argdsExceptionInfo); 
                  if(i==1) interval = 1.5*interval_set;
                  else     interval = interval_set;
                  emuLvCRB_generateToggleA4(sElmb,interval);                  
                  emuLvCRB_showDebug(bDebug,"switch on "+fsm+" board "+i+" with "+interval+"ms clock"); 
                }
          //----switch on vcc--------------------------
          if(dynContains(dsOffChannels,dynlen(dsPairs)+2))continue;  
          else emuLvCRB_setDoBitSync(sElmb,dsRest[2],isOn,argdsExceptionInfo); 
          interval = 1.5*interval_set;  
          emuLvCRB_generateToggleA4(sElmb,interval);          
          emuLvCRB_showDebug(bDebug,"switch on "+fsm+" board_cc 2 with "+interval+"ms clock");
          //----switch on ccb/mpc--------------------------
          if(dynContains(dsOffChannels,dynlen(dsPairs)+1))continue;  
          else emuLvCRB_setDoBitSync(sElmb,dsRest[1],isOn,argdsExceptionInfo); 
          interval = interval_set;  
          emuLvCRB_generateToggleA4(sElmb,interval);          
          emuLvCRB_showDebug(bDebug,"switch on "+fsm+" board_cc 1 with "+interval+"ms clock");
          dpSetWait(fsm+".status",2); //set FSM state to On or standby
          DebugTN("Power on for Crb:"+fsm);
        } 
         time endT = getCurrentTime();
         time dt = endT - startT;
         DebugTN("Switch on took " + minute(dt) +":"+ second(dt) +"."+ milliSecond(dt));
      } //end of Power on
    dpSetWait("LV_START_UP_CHAMBER.",fsm+" is finished"); 
}    
//============================================================================================
void mudcsCrbCommandCrbGroup(string fsm, bool isOn, dyn_int boards)
{  
  string sElmb,sBit,sBit_id,sTest,switch_status;
  dyn_string dsPairs, dsRest, dsOffChannels,switch_list,argdsExceptionInfo;
  dyn_int off_channels;
  int i,j;
//------------------for x2p---------------------------------
  if(automatic_stop_pcrate_slow_control)
   {
    if(isOn)mudcsCrb_stop_slow_control(fsm);
    delay(5);
   }    
//------low level is on, high level is off for CRB---------
  if(isOn)
   {
    isOn=false;
    switch_status = "switch on";
   } 
  else
   { 
     isOn=true;
     switch_status = "switch off"; 
   }  
  dsOffChannels=makeDynString();
  dpGet(fsm+".switch_list",switch_list);
  dpGet(fsm+".off_channels",off_channels);
  if(is_off_disabled_channels)
   {
     for(i=1;i<=dynlen(off_channels);i++)dsOffChannels[i]=off_channels[i];
   }
  sTest=substr(switch_list[1],0,strpos(switch_list[1],"/do_"));    
  sElmb=substr(switch_list[1],0,strpos(switch_list[1],"/DO/"));  
  dynClear(switch_list);
//-------------CSC_fwCAN1_g_NEW_ELMB_SWITCH-------------------
  dsPairs = emuLvCRB_getDynString("Switch_List_DTmbs");
  dsRest  = emuLvCRB_getDynString("Switch_List_Others");
//------------------for Tmb/Dmb boards (once only)--------------------------  
  for(j=1;j<=1;j++)
   {
     for(i=1;i<=dynlen(dsPairs);i++)
       {  
         if(dynContains(dsOffChannels,i)>=1)continue;
         if(dynContains(boards,i)>=1)
             {    
               emuLvCRB_setDoBitSync(sElmb,dsPairs[i],isOn,argdsExceptionInfo);    
             }
       }
//------------------for others-----------------------------------
     for(i=1;i<=3;i++)
       {  
         if(dynContains(dsOffChannels,dynlen(dsPairs)+i)>=1)continue;
         if(dynContains(boards,dynlen(dsPairs)+i)>=1)
          {    
            emuLvCRB_setDoBitSync(sElmb,dsRest[i],isOn,argdsExceptionInfo);         
          }
       }
//------------------set toggle to A;4---------------------------------
     emuLvCRB_generateToggleA4(sElmb,100);      
   }
  DebugTN("Crb:"+sElmb+" board "+boards+" "+switch_status); 
}  
//============================================================================================
void update_confirmation(string dpName, string value)
{ // x2p  
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
  //--------------------------
  //DebugTN(value);
  DebugTN(remote_system+" "+dpName);
  //DebugTN(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);
  //---------------------------
  if((dynlen(ds1)==2 && ds1[1]=="INIT_IS_DONE"))
    {
      if(strpos(ds1[2],"VME")>=0)id_s=mudcsCrb_getIdByVme(ds1[2] );
      else id_s=ds1[2];
      coord_int_receive=id_s;//ds1[2];
      dynUnique(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);    
      int ind=dynContains(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER,id_s);//ds1[2]);
      if(ind>0)
        {
         dynRemove(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER, ind);
         for(i=1;i<=dynlen(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);i++)
          {
            coord_int=CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i];
            if(coord_int<=30)plus_len++;
            else minus_len++;
          }
         if(special_stop_value != "STOP")
          { 
            if(coord_int_receive <=30 && remote_system==CSC_fwCAN1_g_PLUS_SYSTEM_NAME && plus_len==0 && last_command !="STOP_SLOW_CONTROL")
             {
              dpSetWait(remote_system+":STOP_SLOW_CONTROL.value","RESUME_SLOW_CONTROL");
              dpSetWait(remote_system+":LV_1_COM.command","RESUME_SLOW_CONTROL");     
             }
            if(coord_int_receive > 30 && remote_system==CSC_fwCAN1_g_MINUS_SYSTEM_NAME && minus_len==0 && last_command !="STOP_SLOW_CONTROL")
             {
              dpSetWait(remote_system+ ":STOP_SLOW_CONTROL.value","RESUME_SLOW_CONTROL");
              dpSetWait(remote_system+":LV_1_COM.command","RESUME_SLOW_CONTROL");     
             }
          }//  if(special_stop_value != "STOP")  
         dpSetWait(mudcsLvAddSystem("PREPARE_POWER_UP_BUFFER."),CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);   
        }
     }
  else
     {
   // heart beat command || start command
       if(strpos(value,"START")>=0)
        {   
          if(last_command=="RESUME_SLOW_CONTROL")
            {
              dpSetWait(remote_system+":LV_1_COM.command","RESUME_SLOW_CONTROL");
            }
          else if(last_command != "STOP_SLOW_CONTROL")
            {    
              if(value=="SOFT_START")
               {  
                 for(i=1;i<=dynlen(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);i++)
                  {
                    ds2=strsplit(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i],";");
                    coord_int=CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i];//ds2[2];
                    coord=CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i];//ds2[2];
                    string vme=mudcsCrb_getVmeById(coord);
                    if(coord_int <=30 && remote_system==CSC_fwCAN1_g_PLUS_SYSTEM_NAME)
                     { // <=1f
                      dpSetWait(remote_system+":STOP_SLOW_CONTROL.value","PREPARE_POWER_UP;"+vme);
                      dpSetWait(remote_system+":LV_1_COM.command","PREPARE_POWER_UP;"+vme);
                     }
                   else if(coord_int >30 && remote_system==CSC_fwCAN1_g_MINUS_SYSTEM_NAME)
                     {
                      dpSetWait(remote_system+":STOP_SLOW_CONTROL.value","PREPARE_POWER_UP;"+vme);
                      dpSetWait(remote_system+":LV_1_COM.command","PREPARE_POWER_UP;"+vme);       
                     }
                 } //for 
              }
             else if(value=="HARD_START")
              {
                mudcsCrb_lv_buffer_power_cycle();
              }
           } // else
          else if(last_command=="STOP_SLOW_CONTROL")
           {
             dpSetWait(remote_system+":STOP_SLOW_CONTROL.value","STOP_SLOW_CONTROL");
             dpSetWait(remote_system+":LV_1_COM.command","STOP_SLOW_CONTROL");     
           }
        } // if "START_"
    } //else  
}
//==============================for X2P ======================================
void mudcsCrb_remove_id(string fsm)
{ // x2p  
  if(two_way_communications_x2p)
     {
       string coord,remote_system;
       int coord_int;  
       dpGet(fsm+".coord",coord); 
       sscanf(coord,"%x",coord_int);  
       if(coord_int <=30)
          {remote_system=CSC_fwCAN1_g_PLUS_SYSTEM_NAME;}
       else
          {remote_system=CSC_fwCAN1_g_MINUS_SYSTEM_NAME;}
       update_confirmation(remote_system+":any", "INIT_IS_DONE;"+coord_int);
     }  
}
//===============================for X2P=========================================
void mudcsCrb_stop_slow_control(string fsm)
{  
  string coord, previous_command;   
  if(two_way_communications_x2p)
   {
     int coord_int;
     DebugTN("debug_stop_slow_control "+fsm);
     DebugTN("debug_stop_slow_control "+CSC_fwCAN1_g_PLUS_SYSTEM_NAME);
     string stop_value,remote_system;
     if(strpos(fsm,"ME_M")>=0)
        {remote_system=CSC_fwCAN1_g_MINUS_SYSTEM_NAME;}
     else if(strpos(fsm,"ME_P")>=0)
        {remote_system=CSC_fwCAN1_g_PLUS_SYSTEM_NAME;} 
     dpGet(fsm+".coord",coord); 
     sscanf(coord,"%x",coord_int);
     dpSetWait(remote_system+":STOP_SLOW_CONTROL.value","PREPARE_POWER_UP;"+coord_int);
     dynAppend(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER,coord_int);
     dynUnique(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);      
   } 
  else
   {    
     DebugTN("debug_stop_slow_control "+fsm);
     DebugTN("debug_stop_slow_control "+CSC_fwCAN1_g_PLUS_SYSTEM_NAME);
     string stop_value,remote_system;
     if(strpos(fsm,"ME_M")>=0)
        {remote_system=CSC_fwCAN1_g_MINUS_SYSTEM_NAME;}
     else if(strpos(fsm,"ME_P")>=0)
        {remote_system=CSC_fwCAN1_g_PLUS_SYSTEM_NAME;}    
     dpSetWait(remote_system+":STOP_SLOW_CONTROL.value","STOP_SLOW_CONTROL_LV");
     dpSetWait(remote_system+":LV_1_COM.command","STOP_SLOW_CONTROL");         
   }
}
//===========================================
string mudcsCrb_getVmeById(string id)
{
  dyn_string dps, dps1;
  int i, coord_int, pos;
  string coord_s;
  for(i=1;i<=dynlen(CSC_fwCAN1_g_DB_PCMB);i++)
   {
    dps=strsplit(CSC_fwCAN1_g_DB_PCMB[i],";");
    sscanf(dps[2],"%x",coord_int);
    coord_s=coord_int;
    if(id==coord_int)
     {
      dps1=strsplit(CSC_fwCAN1_g_DB_PCMB_GEOGRAPHIC[i],";");
      return "VME"+strtolower(substr(dps[1],0,2))+"_"+dps1[2];
     }
   }  // i 
  return "";
}
//===========================================
string mudcsCrb_getIdByVme(string vme)
{
  dyn_string dps, dps1;
  int i,coord_int, pos;
  string coord_s, vme_s;
  for(i=1;i<=dynlen(CSC_fwCAN1_g_DB_PCMB);i++)
   {
     dps=strsplit(CSC_fwCAN1_g_DB_PCMB[i],";");
     dps1=strsplit(CSC_fwCAN1_g_DB_PCMB_GEOGRAPHIC[i],";");
     if(dynlen(dps)<2 || dynlen(dps1)<2)continue;
     vme_s="VME"+strtolower(substr(dps[1],0,2))+"_"+dps1[2]; 
     if(vme==vme_s)
      {
        sscanf(dps[2],"%x",coord_int);
        coord_s=coord_int;   
        return coord_s;
      }
   }  // i     
  return "";
}
//==========================================
void mudcsCrb_power_up_packet(string remote_system)
{
  int i,status, coord_int;
  string coord;
  dyn_string dps, error_buffer, final_buffer;
  dps=dpNames("*","fwCrb_CSC_LV");
  for(i=1;i<=dynlen(dps);i++)
   {
     dpGet(dps[i]+".status",status);
     DebugTN(dps[i]+" "+status);
     if(status<=0)
      { // including OFF state
       dpGet(dps[i]+".coord",coord); 
       sscanf(coord,"%x",coord_int);
       dynAppend(error_buffer,coord_int);
      }
   } // for
  //-----
  for(i=1;i<=dynlen(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);i++)
   {    
    coord_int=CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i];    
    if(coord_int<=30 && remote_system==CSC_fwCAN1_g_MINUS_SYSTEM_NAME)continue;
    if(coord_int>30 && remote_system==CSC_fwCAN1_g_PLUS_SYSTEM_NAME)continue;
    dynAppend(final_buffer,CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i]);
    string vme=mudcsCrb_getVmeById(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER[i] );
    dpSetWait(remote_system+":LV_1_COM.command","PREPARE_POWER_UP;"+vme);
    DebugTN(remote_system+":LV_1_COM.command  "+"PREPARE_POWER_UP;"+vme);
   } // for
  DebugTN(remote_system);
  DebugTN(CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);
  dpSetWait(mudcsLvAddSystem("PREPARE_POWER_UP_BUFFER."),CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER);  
}
//========================================
void mudcsCrb_resume_slow_control()
{ //x2p  
  int i;
  string stop_value, special_stop_value;
  time stop_value_time, current_time;
  string remote_system; 
  if(two_way_communications_x2p)
    {    
     current_time=getCurrentTime();
     DebugTN(CSC_fwCAN1_g_MINUS_SYSTEM_NAME+","+CSC_fwCAN1_g_PLUS_SYSTEM_NAME);
     for(i=1;i<=2;i++)
      {
        if(i==1)remote_system=CSC_fwCAN1_g_MINUS_SYSTEM_NAME;
        else if(i==2)remote_system=CSC_fwCAN1_g_PLUS_SYSTEM_NAME;
        dpGet(remote_system+":STOP_SLOW_CONTROL.value",stop_value);//"STOP_SLOW_CONTROL_LV"
        dpGet(remote_system+":STOP_SLOW_CONTROL.value:_online.._stime",stop_value_time);
        if(strpos(stop_value,"PREPARE_POWER_UP")>=0)
         {
          string side = "M";
          if (i==2) { side = "P"; }
          dyn_string crbDps = dpNames("CRB/CSC_ME_" + side + "*", "fwCrb_CSC_LV");
          bool powerupFinished = true;
          for (int j=1; j <= dynlen(crbDps); j++) {
            int command, status;
            dpGet(crbDps[j] + ".command", command,
                  crbDps[j] + ".status", status);
            if ((command == 2) && (status != command)) {
              powerupFinished = false;
              break;
            }
          }
          //if((current_time-stop_value_time) > 15)
          if (powerupFinished)
           {
             mudcsCrb_power_up_packet(remote_system);
             dpSet(remote_system+":STOP_SLOW_CONTROL.value","EXECUTE_POWER_UP");
             dpSet(remote_system+":LV_1_COM.command","EXECUTE_POWER_UP");       
           }
         }//if
       } // for  
    }
  else
    {     
      current_time=getCurrentTime();    
      for(i=1;i<=2;i++)
       {
         if(i==1)remote_system=CSC_fwCAN1_g_MINUS_SYSTEM_NAME;
         else if(i==2)remote_system=CSC_fwCAN1_g_PLUS_SYSTEM_NAME;
         dpGet(remote_system+":STOP_SLOW_CONTROL.value",stop_value);//"STOP_SLOW_CONTROL_LV"
         dpGet(remote_system+":SPECIAL_STOP_SC.value",special_stop_value);//   
         dpGet(remote_system+":STOP_SLOW_CONTROL.value:_online.._stime",stop_value_time);
         if(stop_value == "STOP_SLOW_CONTROL_LV" && special_stop_value != "STOP")
          {
            if((current_time-stop_value_time) > 3)
             {
               dpSet(remote_system+":STOP_SLOW_CONTROL.value","RESUME_SLOW_CONTROL");
               dpSet(remote_system+":LV_1_COM.command","RESUME_SLOW_CONTROL");       
             }
          }//if
       } // for        
    }
}
//================== switch .status to .command --X.Yang ======================
void mudcsCrb_lv_buffer_power_cycle()
{ //x2p 
   dyn_string ds, ds1, ds2, split, prepare_power_up_buffer;
   int i,coord_int, ind;
   string coord_s;
   dpGet("db_pcmb.list",ds);
   prepare_power_up_buffer=CSC_fwCAN1_g_PREPARE_POWER_UP_BUFFER;
   for(i=1;i<=dynlen(ds);i++)
     {    
       split=strsplit(ds[i],";");
       sscanf(split[2],"%x",coord_int);
       coord_s=coord_int;
       ind=dynContains(prepare_power_up_buffer,coord_s);
       //DebugTN(ind);
       if(ind>=1)
        {
          ds1=dpNames("*"+split[1]+"*","fwCrb_CSC_LV");
          dynAppend(ds2,ds1[1]);      
        }     
     }
   //DebugTN(ds2); 
   for(i=1;i<=dynlen(ds2);i++)
    {
    // dpSet(ds2[i]+".status",0); 
       dpSet(ds2[i]+".command",0);  
    } 
   delay(1);
   for(i=1;i<=dynlen(ds2);i++)
    {
   //  dpSet(ds2[i]+".status",2); 
      dpSet(ds2[i]+".command",2); 
    }     
}
//====================================================
void mudcsCrbGetDmbTempAlerts(string DpName, dyn_string &ds_alerts, dyn_int &alert_channels, dyn_string &fsms)
{
 
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
   if(p1>=0 && p2>=0)
    {
     dp_template=substr(DpName,p1+3,p2-(p1+3));  
    }
   if (dp_template == "") { return; }
   dps=dpNames("cms_csc_dcs_*:"+dp_template,"fwLvCr_CSC_TEMP");
   if(dynlen(dps)<=0)return;
   DebugTN("dp_template = "+dp_template);
   DebugTN("dps = "+dps); 
   dpGet(dps[1]+"."+":_alert_hdl.._dp_list",ds_alerts_notordered); 
   for(i=1;i<=dynlen(ds_alerts_notordered);i++)
    {
      if(strpos(dp_template,"PC6")>=0 && strlen(dp_template)==6)
       {
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
      else if(strpos(dp_template,"PC6B")>=0)
       {
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
      else
       {
         ds_alerts[i]=ds_alerts_notordered[i];  
       }
    } //i
   for(i=1;i<=dynlen(ds_alerts);i++)
    { 
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
}
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
