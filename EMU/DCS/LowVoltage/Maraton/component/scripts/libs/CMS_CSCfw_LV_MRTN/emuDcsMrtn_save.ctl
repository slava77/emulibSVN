string mudcsMrtnGetSide(string node){

dyn_string dsTest;
string sTest;

if(node ==""){ // case of call not from FSM panel, so definetly from native project: once from VisualServer

//-----
 
sTest=getHostname();
dsTest=strsplit(sTest,"."); // just in case
dsTest=strsplit(dsTest[1],"-");
if(CSC_fwCAN2_g_ISSINGLEMACHINE)return "";
else if(strpos(dsTest[dynlen(dsTest)],"1")>=0)return "_P";
else if(strpos(dsTest[dynlen(dsTest)],"2")>=0)return "_M";

}
else{
dsTest=strsplit(node,"_");
if(dsTest[dynlen(dsTest)] != "P" && dsTest[dynlen(dsTest)] != "M")return ""; // "CAN2" is returned
return "_"+dsTest[dynlen(dsTest)];

}
//----


}




mudcsMrtnServerInit(){

//if(level==0)return;
    
//----- INIT PART------------------------------------
  CSC_fwCAN2_g_CANBUSES=dpNames(CSC_fwCAN2_g_SYSTEM_NAME+":*","FwWienerCanBus");
 /* 
  for(i=1;i<=dynlen(CSC_fwCAN1_g_CANBUSES);i++){
   dpGet(CSC_fwCAN1_g_CANBUSES[i]+".nodeGuardInterval",CSC_fwCAN1_g_nodeGuardInterval[i]);
  }
  
  ds1=dpNames("*","MRTN_2");  
  for(i=1;i<=dynlen(ds1);i++){  
   dpGet(ds1[i]+".coord",coord);
   dynAppend(CSC_fwCAN1_g_MRTN2ELMB,ds1[i]+";"+coord);
  }
  
//  DebugTN(CSC_fwCAN1_g_CRB2ELMB);
  
//---------------------------------------------------  
  
  */
  
}

//=====================================================================

string mudcsMrtnPnlInit(string node=""){


  int i;
  dyn_string ds1;
  string coord;
  string emu_side;

  addGlobal("CSC_fwCAN2_g_ISSINGLEMACHINE", BOOL_VAR);
  CSC_fwCAN2_g_ISSINGLEMACHINE=false;
  
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
//  addGlobal("CSC_fwCAN2_g_MRTN2ADDR", DYN_STRING_VAR);
    

//--------------------------------------------------------------------------
//  CSC_fwCAN2_g_SYSTEM_NAME="CMS_CSC_LV_CAN2";//fwFsm_getSystemName(); 
//-------------
      string node_to_define_system_name=CSC_fwCAN2_g_csc_part;
      fwCU_getDp(node_to_define_system_name,node_to_define_system_name);
//-------------         
   dyn_string systemNamesDps;
   dyn_string s_split;
   systemNamesDps=dpNames("*:CMS_CSC_LV_MRTN"+emu_side);
//   systemNamesDps=dpNames("*:"+node_to_define_system_name);   
   if(dynlen(systemNamesDps)==0)DebugTN("SYSTEM NAME NOT DEFINED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
   else{
    s_split=strsplit(systemNamesDps[1],":"); 
    CSC_fwCAN2_g_SYSTEM_NAME=s_split[1];//dpSubStr(systemNamesDps[1], DPSUB_SYS);
   ////// mudcsLvDebug(CSC_fwCAN2_g_SYSTEM_NAME);
   }
//--------------------------------------------------------------------------       
  

  CSC_fwCAN2_g_ISACK=false;  
  CSC_fwCAN2_g_actual_channels=makeDynString("0","1","3","4","5","7");
    

  return emu_side;
  
}
//=======================================================================
//===================================================
