#uses "CMS_CSCfw_GAS/emuGas.ctl"
#uses "CMS_CSCfw_GAS/mudcs0GasCreateAllTypes.ctl"
#uses "CMS_CSCfw_GAS/mudcs1GasDipConfig.ctl"

string Component;
string ProjectHome;
string oper_system, add_to_system_command;
int mode=2;
string dir_oper;

main(){
 mudcsGasComponentCreate(1); 
 mudcsGasComponentCreate(2);  
}


mudcsGasComponentCreate(int mode_par)
{

  mode=mode_par;
  
mudcsGasInit();

string oper_system=getenv("OS");

add_to_system_command="";
if ( oper_system !="Linux")add_to_system_command="cmd /c ";
  
Component=CSC_fwGGas_g_Component;
ProjectHome=CSC_fwGGas_g_ProjectHome;


dir_oper=Component;
string dir_config=Component+"/config";
string dir_init=Component+"/init";

string key_add="";
if(mode==1)key_add="_MINI";

Component+=key_add;

//---forming package: EMU DCS components------------
mudcsSynchSystemPoratbleOS(oper_system,"rm -rf "+CSC_fwGGas_g_HOME+"/"+Component);
DebugTN(oper_system+"  >> "+"rm -rf "+CSC_fwGGas_g_HOME+"/"+Component);

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGGas_g_HOME+"/"+Component);
DebugTN(oper_system+"  >> "+"mkdir "+CSC_fwGGas_g_HOME+"/"+Component);
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGGas_g_HOME+"/"+Component+"/scripts");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGGas_g_HOME+"/"+Component+"/scripts/"+dir_oper);
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGGas_g_HOME+"/"+Component+"/scripts/libs");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGGas_g_HOME+"/"+Component+"/scripts/libs/"+dir_oper);

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGGas_g_HOME+"/"+Component+"/panels");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGGas_g_HOME+"/"+Component+"/panels/"+dir_oper);


mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGGas_g_HOME+"/"+Component+"/config/");

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGGas_g_HOME+"/"+Component+"/dplist/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGGas_g_HOME+"/"+Component+"/dplist/"+dir_oper+"/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGGas_g_HOME+"/"+Component+"/images/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGGas_g_HOME+"/"+Component+"/images/"+dir_oper+"/");

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGGas_g_HOME+"/"+Component+"/source/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGGas_g_HOME+"/"+Component+"/source/"+dir_oper+"/");


//======== COPYING EVERYTHING FOR THE WHOLE PROJECT===============================================

mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/scripts/libs/"+dir_oper+"/*mu*.ctl"+" "+CSC_fwGGas_g_HOME+"/"+Component+"/scripts/libs/"+dir_oper+"/.");
if(mode==2){
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/scripts/"+dir_oper+"/*.ctl"+" "+CSC_fwGGas_g_HOME+"/"+Component+"/scripts/"+dir_oper+"/.");  
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/scripts/"+dir_oper+"/"+dir_oper+".postInstall"+" "+CSC_fwGGas_g_HOME+"/"+Component+"/scripts/"+dir_oper+"/."); 

}
  



mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/panels/"+dir_oper+"/*.pnl"+" "+CSC_fwGGas_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

if(mode==2){
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/panels/"+dir_config+" "+CSC_fwGGas_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/panels/"+dir_init+" "+CSC_fwGGas_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
}


//mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/panles/"+dir_dubna+"/"+dir_init+" "+CSC_fwGGas_g_HOME+"/"+Component+"/panels/"+Component+"/.");

//================================================================================================


////////#########mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/CSCfw/scripts/libs/* "+" "+CSC_fwGGas_g_HOME+"/"+Component+"/scripts/libs/CMS_CSCfw/.");
///////##########mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/CSCfw/panels/* "+" "+CSC_fwGGas_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");




mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/source/"+dir_oper+"/config/"+dir_oper+".config "+
                                  +CSC_fwGGas_g_HOME+"/"+Component+"/config/.");

if(mode==2){
mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/source/"+dir_oper+"/config/"+dir_oper+".xml "+CSC_fwGGas_g_HOME+"/"+Component+"/"+dir_oper+".xml");
mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/source/"+dir_oper+"/config/"+dir_oper+"_short.xml "+CSC_fwGGas_g_HOME+"/"+Component+"/"+dir_oper+"_MINI.xml");
 
}
else if(mode==1)
mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/source/"+dir_oper+"/config/"+dir_oper+"_short_mode1.xml "+CSC_fwGGas_g_HOME+"/"+Component+"/"+dir_oper+".xml");
  
  
if(mode==2)dplist();
////////dplist2();

//=================================================================================================

mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/source/"+dir_oper+"/config"+" "+CSC_fwGGas_g_HOME+"/"+Component+"/source/"+dir_oper+"/.");

//mudcsGasDebug("done");


}
//======================================

dplist(){

dyn_string all_dps, dps;
int i;

dynAppend(all_dps,"fwOT_EMUTREENodes");
dps=dpNames("fwOT_EMUTREENodes.*");
dynAppend(all_dps,dps);

if(mode==2){
dynAppend(all_dps,"fwOT_fwGasSystem_CSC_GAS");
dps=dpNames("fwOT_fwGasSystem_CSC_GAS.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_fwCooling_CSC_COOLING");
dps=dpNames("fwOT_fwCooling_CSC_COOLING.*");
dynAppend(all_dps,dps);
}

//all_dps=makeDynString();

//=================

//string command="PVSS00ascii -out dplist/migr_to_new_project.dpl ";
string command="PVSS00ascii -out "+CSC_fwGGas_g_HOME+"/"+Component+"/dplist/"+dir_oper+"/"+dir_oper+".dpl -filterDpType _FwFsmObjectType ";

 for(i=1;i<=dynlen(all_dps);i++){


   command += " -filterDp "+all_dps[i];

 }



DebugTN(command);


DebugTN("-------");
system(command);
DebugTN("-------");

}
//========================================================================



//========================================================================
