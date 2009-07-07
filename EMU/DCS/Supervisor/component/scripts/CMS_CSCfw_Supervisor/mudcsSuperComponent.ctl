int mode=2; // mode =2 is browsing with all CSC panels
            // mode=0 only FSM panel browsing
            // mode=1  : is browsing with all CSC panels besides some
            // mode=3 easy mode

string Component;
string ProjectHome;
string oper_system, add_to_system_command;
string dir_oper;
string dir_config;
string dir_init;

main()
{

  
DebugTN("Super Component Creation");
mudcsSuperInit();
DebugTN("Super Component Creation 2");
mudcsSupeComponentrCreate(1);
mudcsSupeComponentrCreate(2);
mudcsSupeComponentrCreate(3);

}

mudcsSupeComponentrCreate(int mode_par){

mode=mode_par;
DebugTN("Super Component Creation "+mode);


string oper_system=getenv("OS");

add_to_system_command="";
if ( oper_system !="Linux")add_to_system_command="cmd /c ";
  
Component=CSC_fwGSuper_g_Component;
ProjectHome=CSC_fwGSuper_g_ProjectHome;


dir_oper=Component;
dir_config=Component+"/config";
dir_init=Component+"/init";


string dir_dubna="CMS_ME11";

string key_add;
if(mode==2)key_add="";
else if(mode==1)key_add="_MINI";
else if(mode==3)key_add="_EASY";

Component+=key_add;

DebugTN("Super Component Creation "+mode);

//---forming package: EMU DCS components------------
mudcsSynchSystemPoratbleOS(oper_system,"rm -rf "+CSC_fwGSuper_g_HOME+"/"+Component);
DebugTN(oper_system+"  >> "+"rm -rf "+CSC_fwGSuper_g_HOME+"/"+Component);

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component);
DebugTN(oper_system+"  >> "+"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component);
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts/"+dir_oper);
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts/libs");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts/libs/"+dir_oper);

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+dir_oper);
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+dir_config);
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+dir_init);

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/config/");

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/dplist/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/dplist/"+dir_oper+"/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/images/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/images/"+dir_oper+"/");

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/source/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/source/"+dir_oper+"/");




//======== COPYING EVERYTHING FOR THE WHOLE PROJECT===============================================

mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/scripts/libs/"+dir_oper+"/*mu*.ctl"+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts/libs/"+dir_oper+"/.");
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/scripts/"+dir_oper+"/*.ctl"+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts/"+dir_oper+"/.");  

//if(mode==2)
  mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/scripts/"+dir_oper+"/*.postInstall"+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts/"+dir_oper+"/."); 


mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/panels/"+dir_oper+"/*.pnl"+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/panels/"+dir_config+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/panels/"+dir_init+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");




//================================================================================================




mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/source/"+dir_oper+"/config/"+dir_oper+".config "+
                                  +CSC_fwGSuper_g_HOME+"/"+Component+"/config/.");

if(mode==2)
mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/source/"+dir_oper+"/config/"+dir_oper+".xml "+CSC_fwGSuper_g_HOME+"/"+Component+"/"+dir_oper+".xml");
else if(mode==1)
mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/source/"+dir_oper+"/config/"+dir_oper+"_short.xml "+CSC_fwGSuper_g_HOME+"/"+Component+"/"+dir_oper+".xml");  
else if(mode==3)
mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/source/"+dir_oper+"/config/"+dir_oper+"_easy.xml "+CSC_fwGSuper_g_HOME+"/"+Component+"/"+dir_oper+".xml");  


if(mode!=3){
dplist();
dplist2();
}

mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/source/"+dir_oper+"/config"+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/source/"+dir_oper+"/.");

//mudcsSuperDebug("done");


}
//======================================

dplist(){

dyn_string all_dps, dps;
int i;

dynAppend(all_dps,"fwOT_EMUNodes");
dps=dpNames("fwOT_EMUNodes.*");
dynAppend(all_dps,dps);

//all_dps=makeDynString();

//=================

//string command="PVSS00ascii -out dplist/migr_to_new_project.dpl ";
string command="PVSS00ascii -out "+CSC_fwGSuper_g_HOME+"/"+Component+"/dplist/"+dir_oper+"/"+dir_oper+".dpl -filterDpType _FwFsmObjectType ";

 for(i=1;i<=dynlen(all_dps);i++){


   command += " -filterDp "+all_dps[i];

 }



DebugTN(command);


DebugTN("-------");
system(command);
DebugTN("-------");

////////////////#####################mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/CSCfw/dplist/CMS_CSCfw.dpl "+" /home/fast/CMS_CSCfw/dplist/CMS_CSCfw/.");


}
//========================================================================
dplist2()
{

string command;


dyn_string all_types, dps;
dyn_dyn_string all_dps;
int i,j;

all_types=dpTypes("*MUDCS_*");


//dynAppend(all_types,"_FwDimConfig");
//dynAppend(all_types,"Db");



////////dynAppend(all_types,"_FwFsmObjectType");

/////////////dynAppend(all_types,"");
////////dynAppend(all_types,"");



 for(i=1;i<=dynlen(all_types);i++){
 /* not necessary as all data to any dp level (*.**) are stored in dpl: if only -filterDpType is idicated (without -filterDp)
      
   if(strpos(all_types[i],"Db")>=0 ){
    dps=dpNames("*.**",all_types[i]);
    dynAppend(all_dps[i],dps);    
   }
 */
   all_dps[i]=makeDynString(); 
  DebugTN(all_types[i]);
 }

//=================

//string command="PVSS00ascii -out dplist/migr_to_new_project.dpl ";
// command="PVSS00ascii -out /home/fast/pvss_export/migr_to_new_project.dpl ";
 command="PVSS00ascii -out "+CSC_fwGSuper_g_HOME+"/"+Component+"/dplist/"+dir_oper+"/"+dir_oper+"2.dpl ";
 
 for(i=1;i<=dynlen(all_types);i++){

  command += " -filterDpType "+all_types[i];
  for(j=1;j<=dynlen(all_dps[i]);j++){


   command += " -filterDp "+all_dps[i][j];


  } // j
 } // i



DebugTN(command);


DebugTN("-------");
system(command);
DebugTN("-------");

}


//========================================================================
