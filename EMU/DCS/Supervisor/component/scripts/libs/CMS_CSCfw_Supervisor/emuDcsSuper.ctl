
mudcsSuperInit(){

 
string host_name=getHostname();

  
  
 //========
addGlobal("CSC_fwGSuper_g_EmuCmsGlobalNode",STRING_VAR);
addGlobal("CSC_fwGSuper_g_EmuCmsGlobalType",STRING_VAR);
addGlobal("CSC_fwGSuper_g_EmuCmsGlobalCu",STRING_VAR);
addGlobal("CSC_fwGSuper_g_EmuCmsGlobalParent",STRING_VAR);


addGlobal("CSC_fwGSuper_g_project_stations_super",DYN_STRING_VAR); 

 CSC_fwGSuper_g_project_stations_super=makeDynString("M4","M3","M2","M1","P1","P2","P3","P4"); 
 //=========
string os=getenv("OS");
addGlobal("CSC_fwGSuper_g_OS",STRING_VAR);
CSC_fwGSuper_g_OS=os;  
  
addGlobal("CSC_fwGSuper_g_HOME",STRING_VAR);

if (os =="Linux")CSC_fwGSuper_g_HOME = "/home/cscdcs"; // "/nfshome0/cscdcsdev"; 
else CSC_fwGSuper_g_HOME = "c:\\pvss_project_36";  
  
addGlobal("CSC_fwGSuper_g_ProjectHome",STRING_VAR);  
if (os =="Linux")CSC_fwGSuper_g_ProjectHome = CSC_fwGSuper_g_HOME+"/CSC_MONITOR";///"/csc_super";  
else CSC_fwGSuper_g_ProjectHome = CSC_fwGSuper_g_HOME+"\\csc_super";



addGlobal("CSC_fwGSuper_g_Component",STRING_VAR);
CSC_fwGSuper_g_Component = "CMS_CSCfw_Supervisor"; // fsm: "mudcs" 

addGlobal("CSC_fwGSuper_g_project_name_home",STRING_VAR);
CSC_fwGSuper_g_project_name_home=CSC_fwGSuper_g_ProjectHome; 

addGlobal("CSC_fwGSuper_g_SYSTEM_NAME",STRING_VAR);
CSC_fwGSuper_g_SYSTEM_NAME="CMS_CSC";


//------------    
   dyn_string systemNamesDps=dpNames("*:CMS_CSC_Super");
   dyn_string s_split;
//------------
   if(dynlen(systemNamesDps)==0)DebugTN("SYSTEM NAME NOT DEFINED !");
   else{
    s_split=strsplit(systemNamesDps[1],":"); 
    CSC_fwGSuper_g_SYSTEM_NAME=s_split[1];//dpSubStr(systemNamesDps[1], DPSUB_SYS);
    ///mudcsLvDebug(CSC_fwG_CSC_fwG_g_SYSTEM_NAME2);
   }


addGlobal("CSC_fwGSuper_g_csc_part",STRING_VAR);
if(strpos(host_name,"-09")>=0)CSC_fwGSuper_g_csc_part="CMS_CSC";
else CSC_fwGSuper_g_csc_part="CMS_CSC1";

addGlobal("CSC_fwGSuper_g_MainLogicalFsmType",STRING_VAR);
CSC_fwGSuper_g_MainLogicalFsmType="EMUNodes";

addGlobal("CSC_fwGSuper_g_TreeLogicalFsmType",STRING_VAR);
CSC_fwGSuper_g_TreeLogicalFsmType="EMUTREENodes";


// CSC_fwGSuper_g_Station_System_Names 
  }
//============================================================================================

mudcsSuperDebug(string string_to_debug){

dyn_string test_dyn_string;

 fwException_raise(test_dyn_string, "MYDEBUG", string_to_debug, "");
 fwExceptionHandling_display(test_dyn_string);


}
//=============================================================================================
//=========================================
string mudcsSuperAddSystem(string dp, string system_add=""){ 

//  dp=fwFsm_extractSystem(dp);

  if(system_add==""){  
   int pos=strpos(dp,CSC_fwGSuper_g_SYSTEM_NAME+":");
   if(pos>=0)dp=substr(dp,pos+strlen(CSC_fwGSuper_g_SYSTEM_NAME+":"));
   dp=CSC_fwGSuper_g_SYSTEM_NAME+":"+dp;
  }
  else{
   int pos=strpos(dp,system_add+":");
   if(pos>=0)dp=substr(dp,pos+strlen(system_add+":"));
   dp=system_add+":"+dp;
  }

  return dp;

}
//=========================================
string mudcsSuperRemoveSystem(string dp, string system_remove=""){ 

  
  int pos;
  if(system_remove==""){   
   if((pos=strpos(dp,CSC_fwGSuper_g_SYSTEM_NAME+":"))< 0)return dp;  
   dp = substr(dp,strlen(CSC_fwGSuper_g_SYSTEM_NAME+":"));

  }
  else{
   if((pos=strpos(dp,system_remove+":"))< 0)return dp;  
   dp = substr(dp,strlen(system_remove+":"));    
    
  }
   return dp;
}
//=======================================================================================
//============================================================================================
