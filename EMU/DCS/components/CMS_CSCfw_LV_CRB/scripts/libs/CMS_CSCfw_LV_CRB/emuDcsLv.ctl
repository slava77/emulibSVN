#uses "CMS_CSCfw_LV_CRB/emuLvCRB.ctl"
//----------------------------------------------------------------------------------
/**@file

This library contains mudcsLv functions for EMU DCS LV project.
@data   Mar 2011 modified by X.Yang: 
               rewrite two functions: mudcsLvAddSystem and mudcsLvRemoveSystem
               commented out previous two 
*/
int mudcsLvProjectNumber(){
  // works only after a proper Init was called
  int i;
  
  if(globalExists("CSC_fwCAN1_g_SYSTEM_NAME"))i=1;
  else if(globalExists("CSC_fwCAN2_g_SYSTEM_NAME"))i=2;  
  
  return i;
  
}
//=======================================================================
string mudcsAliasNameGet(string fdpname){
 return fdpname;
}
//=======================================================================
/*
string mudcsLvAddSystem(string dp, string system_add=""){
// attention: the dp may contain e.g. :_alert..active !!!
// so the code below covers that 
  
  dyn_string s_split; 
  s_split=strsplit(dp,":");    
  
  string s1;  
  //--- this works only in the mrtn or crb projects !!!!----
  if(globalExists("CSC_fwCAN1_g_SYSTEM_NAME")){s1=CSC_fwCAN1_g_SYSTEM_NAME;}
  else if(globalExists("CSC_fwCAN2_g_SYSTEM_NAME"))s1=CSC_fwCAN2_g_SYSTEM_NAME;
  //--------------------------------------------------------
  
  //---- this works everywhere --------------
  if(system_add!="")s1=system_add; 
  //-----------------------------------------
  
  
//  return s1+":"+s_split[dynlen(s_split)];
//  else return system_add+":"+s_split[dynlen(s_split)];
  if(strpos(dp,s1+":")>=0)return dp;
  return s1+":"+dp;  
}
*/
//================created by X.Yang=======================================
string mudcsLvAddSystem(string dp,string system_add="")
{
  bool gExist = globalExists("gSystemNameCRB");
  if(!gExist){
      emuLvCRB_initalizeParam(); //get system name where the CRB component has been installed
    }    
  if(strpos(dp,gSystemNameCRB)>=0)return dp;
  else return gSystemNameCRB+dp;
}  
//=======================================================================
/*
string mudcsLvRemoveSystem(string dp, string system_rem=""){
  
// attention: the dp may contain e.g. :_alert..active !!!
// so the code below covers that 
    

  int pos;
  //--- this works only in the mrtn or crb projects !!!!----
  string s1;  
  if(globalExists("CSC_fwCAN1_g_SYSTEM_NAME"))s1=CSC_fwCAN1_g_SYSTEM_NAME;
  else if(globalExists("CSC_fwCAN2_g_SYSTEM_NAME"))s1=CSC_fwCAN2_g_SYSTEM_NAME;  
  //--------------------------------------------------------  
  
  //---- this works everywhere --------------
  if(system_rem!="")s1=system_rem; 
  //-----------------------------------------  
  DebugTN("s1 = "+s1);
  if((pos=strpos(dp,s1+":"))< 0){
    DebugTN("return dp 1= "+dp);
    return dp; 
  } 
  dp = substr(dp,strlen(s1+":"));
  DebugTN("return dp 2= "+dp);
  return dp;  
}
*/
//================created by X.Yang===========================================================
string mudcsLvRemoveSystem(string dp,string system_rem="")
{
  bool gExist = globalExists("gSystemNameCRB");
  if(!gExist){
      emuLvCRB_initalizeParam(); //get system name where the CRB component has been installed
    } 
  string sDpName;
  if(strpos(dp,gSystemNameCRB)>=0){
    sDpName = substr(dp,strlen(gSystemNameCRB));
  }
  else
    sDpName = dp;
  return sDpName;
}  
//============================================================================================

mudcsLvDebug(string string_to_debug){

dyn_string test_dyn_string;

 fwException_raise(test_dyn_string, "MYDEBUG", string_to_debug, "");
 fwExceptionHandling_display(test_dyn_string);


}
//============================================================================================
mudcsLvDebug2(string string_to_debug){

dyn_string test_dyn_string;

 fwException_raise(test_dyn_string, "MYDEBUG", string_to_debug, "");
 mudcsExceptionHandling_display(test_dyn_string);

}
//============================================================================================

//================================================

string mudcsGetPhysical(string logical){ // only used if fwFsm, fwFsmUtil.ctl modified


dyn_string emu_aliases;
dyn_string pair;
dpGet("EMUALIASES.",emu_aliases);
int i;
string ret="";
//return ret;

//if(dynlen(emu_aliases)==0)emu_aliases=CSC_fwG_g_EMU_ALIASES;

for(i=1;i<=dynlen(emu_aliases);i++){
///  DebugN("mudcsSetAliases: +++++++++++"+emu_aliases[i]);
  pair=strsplit(emu_aliases[i],"#");
  if(pair[2]==logical){DebugN("mudcsGetPhysical ++++++++++++++++++++++ "+pair[2]+" phys="+pair[1]);
  if(strpos(pair[1],".") >=0 )ret=substr(pair[1],0,strlen(pair[1])-1);
  return ret;}
}
DebugN("mudcsGetPhysical ++++++++++++++++++++++ "+logical+" phys="+"EMPTY");
return ret;

}

//================================================

string mudcsGetLogical(string phys){ // only used if fwFsm, fwFsmUtil.ctl modified


dyn_string emu_aliases;
dyn_string pair;
dpGet("EMUALIASES.",emu_aliases);
int i;
string ret="";
//return ret;

//if(dynlen(emu_aliases)==0)emu_aliases=CSC_fwG_g_EMU_ALIASES;

for(i=1;i<=dynlen(emu_aliases);i++){
if(strpos(phys,".") <0 )phys=phys+".";

  pair=strsplit(emu_aliases[i],"#");

  if(strpos(pair[1],phys)>=0){DebugN("mudcsGetLogical ++++++++++++++++++++++ "+pair[2]+" phys="+pair[1]);
  return pair[2];}
}
DebugN("mudcsGetLogical ++++++++++++++++++++++ "+phys+" logical="+"EMPTY");
return ret;

}

//==================================================
