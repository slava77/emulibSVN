string mudcsMrtnGetSide(string node)
{
 // corr++ datapoint CMS_CSC_LV_MRTN_DEV should be created 
 dyn_string dsTest, system_dps;
 string sTest;
 int i;

 if(node =="")
  { // case of call not from FSM panel, so definetly from native project: once from VisualServer
   system_dps=dpNames("CMS_CSC_LV_MRTN*","MUDCS_STRING");
   sTest=getHostname();
// mudcsLvDebug(sTest);
   if((strpos(sTest, "EMU-DCS-DEV") >= 0) || (strpos(sTest, "emu-dcs-dev") >= 0))
    { //  DEV machine  // corr++
     CSC_fwCAN2_g_904_MACHINE=true;   
     if(CSC_fwCAN2_g_DEV_MODE)
      {
        return "_DEV";  
      }   
     else if(CSC_fwCAN2_g_ISSINGLEMACHINE)
      {
       return ""; 
      }  
     else if(CSC_fwCAN2_g_P5_SIM_MINUS)
      {
       return "_M";   
      }
     else if(CSC_fwCAN2_g_P5_SIM_PLUS)
      {
       return "_P";   
      }
    }       
   else
    {
     dsTest=strsplit(sTest,"."); // just in case
     dsTest=strsplit(dsTest[1],"-");
     if(CSC_fwCAN2_g_ISSINGLEMACHINE)
      {
       return "";    
      }
     else if(strpos(dsTest[dynlen(dsTest)],"1")>=0)
      {
       return "_P";
      }
     else if(strpos(dsTest[dynlen(dsTest)],"2")>=0)
      {       
       return "_M";
      }
    }
  }
 else
  {
   dsTest=strsplit(node,"_");
   if(dsTest[dynlen(dsTest)] == "DEV")return "_DEV"; // corr++ fsm node(domain) should be ended with "_DEV" 
   if(CSC_fwCAN2_g_ISSINGLEMACHINE)return ""; 
   if(dsTest[dynlen(dsTest)] != "P" && dsTest[dynlen(dsTest)] != "M")return "_P"; // corr++ 
   return "_"+dsTest[dynlen(dsTest)];
  }
}
//=====================================================================
void mudcsMrtnServerInit()
{    
//----- INIT PART------------------------------------
  CSC_fwCAN2_g_CANBUSES=dpNames(CSC_fwCAN2_g_SYSTEM_NAME+":*","FwWienerCanBus");  
}
//=====================================================================
string mudcsMrtnPnlInit(string node="")
{
  int i;
  dyn_string ds1;
  string coord,emu_side;

  addGlobal("CSC_fwCAN2_g_P5_SIM", BOOL_VAR); // to set P5 simulation mode
  CSC_fwCAN2_g_P5_SIM=false;        
  
  addGlobal("CSC_fwCAN2_g_DEV_MODE", BOOL_VAR); // to set only real 904 nodes
  CSC_fwCAN2_g_DEV_MODE=false;  
    
  if(CSC_fwCAN2_g_P5_SIM)CSC_fwCAN2_g_DEV_MODE=false; // CSC_fwCAN2_g_P5_SIM overrides CSC_fwCAN2_g_DEV_MODE
    
  addGlobal("CSC_fwCAN2_g_ISNEWOPCSERVER", BOOL_VAR);
  CSC_fwCAN2_g_ISNEWOPCSERVER=true;  

  addGlobal("CSC_fwCAN2_g_ISSINGLEMACHINE", BOOL_VAR);
  CSC_fwCAN2_g_ISSINGLEMACHINE=true;

  addGlobal("CSC_fwCAN2_g_904_MACHINE", BOOL_VAR);
  CSC_fwCAN2_g_904_MACHINE=false;  
  
  addGlobal("CSC_fwCAN2_g_P5_SIM_MINUS", BOOL_VAR);
  addGlobal("CSC_fwCAN2_g_P5_SIM_PLUS", BOOL_VAR);
  CSC_fwCAN2_g_P5_SIM_MINUS=false;
  CSC_fwCAN2_g_P5_SIM_PLUS=false;
//========================================
  emu_side=mudcsMrtnGetSide(node);
//======================================== 
  addGlobal("CSC_fwCAN2_g_SYSTEM_NAME", STRING_VAR);   
  addGlobal("CSC_fwCAN2_g_EMU_ALIASES", DYN_STRING_VAR); 
  addGlobal("CSC_fwCAN2_g_CANBUSES", DYN_STRING_VAR);
  addGlobal("CSC_fwCAN2_g_csc_part", STRING_VAR);
  addGlobal("CSC_fwCAN2_g_ISACK", BOOL_VAR);
  addGlobal("CSC_fwCAN2_g_actual_channels", DYN_STRING_VAR);

  if(emu_side=="_M")CSC_fwCAN2_g_csc_part="CMS_CSC_LV_CAN2"+emu_side;
  else if(emu_side=="_P")CSC_fwCAN2_g_csc_part="CMS_CSC_LV_CAN2";
  else if(emu_side=="_DEV")CSC_fwCAN2_g_csc_part="CMS_CSC_LV_CAN2"+emu_side; // corr++
  else if(emu_side=="")CSC_fwCAN2_g_csc_part="CMS_CSC_LV_CAN2"+emu_side; // corr++  
//--------------------------------------------------------------------------
  string node_to_define_system_name=CSC_fwCAN2_g_csc_part;
  fwCU_getDp(node_to_define_system_name,node_to_define_system_name);
//-------------         
  dyn_string systemNamesDps;
  dyn_string s_split;
  systemNamesDps=dpNames("*:CMS_CSC_LV_MRTN"+emu_side);

  if(dynlen(systemNamesDps)==0)DebugTN("SYSTEM NAME NOT DEFINED !!");
  else
    {
     s_split=strsplit(systemNamesDps[1],":"); 
     CSC_fwCAN2_g_SYSTEM_NAME=s_split[1];//dpSubStr(systemNamesDps[1], DPSUB_SYS);
    }
//--------------------------------------------------------------------------       
  CSC_fwCAN2_g_ISACK=false;  
  CSC_fwCAN2_g_actual_channels=makeDynString("0","1","3","4","5","7");
  return emu_side;  
}
//=======================================================================
