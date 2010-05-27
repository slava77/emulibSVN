int mode=2; // mode =2 is browsing with all CSC panels
            // mode=0 only FSM panel browsing
            // mode=1  : is browsing with all CSC panels besides some

string Component;
string ProjectHome;
string oper_system, add_to_system_command;

main()
{

  

mudcsSuperInit();

string oper_system=getenv("OS");

add_to_system_command="";
if ( oper_system !="Linux")add_to_system_command="cmd /c ";
  
Component=CSC_fwGSuper_g_Component;
ProjectHome=CSC_fwGSuper_g_ProjectHome;


string dir_oper=Component;
string dir_config=Component+"/config";
string dir_init=Component+"/init";


string dir_dubna="CMS_ME11";


//---forming package: EMU DCS components------------
mudcsSynchSystemPoratbleOS(oper_system,"rm -rf "+CSC_fwGSuper_g_HOME+"/"+Component);
DebugTN(oper_system+"  >> "+"rm -rf "+CSC_fwGSuper_g_HOME+"/"+Component);

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component);
DebugTN(oper_system+"  >> "+"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component);
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts/"+Component);
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts/libs");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts/libs/"+Component);

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+dir_oper);
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+dir_config);
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+dir_init);

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/config/");

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/dplist/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/dplist/"+Component+"/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/images/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/images/"+Component+"/");

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/source/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/source/"+Component+"/");
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/source/"+Component+"/special_stuff");
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/source/"+Component+"/unix2dos");
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/source/"+Component+"/delete_processes_by_name");
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/source/"+Component+"/config");
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwGSuper_g_HOME+"/"+Component+"/source/"+Component+"/text");

/////////////###########mudcsSynchSystemPoratbleOS(oper_system,"rm -f "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+dir_oper+"/*");
/////////////#############mudcsSynchSystemPoratbleOS(oper_system,"rm -f $ProjectHome/CSCfw/scripts/libs/*");




//======== COPYING EVERYTHING FOR THE WHOLE PROJECT===============================================

mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/scripts/libs/"+Component+"/*mu*.ctl"+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts/libs/"+Component+"/.");
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/scripts/"+Component+"/*.ctl"+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts/"+Component+"/.");  
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/scripts/"+Component+"/"+Component+".postInstall"+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts/"+Component+"/."); 


mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/panels/"+dir_oper+"/*.pnl"+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/panels/"+dir_config+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+Component+"/.");
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/panels/"+dir_init+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+Component+"/.");



//mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/panles/"+dir_dubna+"/"+dir_init+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+Component+"/.");

//================================================================================================


////////#########mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/CSCfw/scripts/libs/* "+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/scripts/libs/CMS_CSCfw/.");
///////##########mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/CSCfw/panels/* "+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");




mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/source/"+Component+"/config/"+Component+".config "+
                                  +CSC_fwGSuper_g_HOME+"/"+Component+"/config/.");

mudcsSynchSystemPoratbleOS(oper_system,"cp "+ProjectHome+"/source/"+Component+"/config/"+Component+".xml "+CSC_fwGSuper_g_HOME+"/"+Component+"/"+".");


dplist();
dplist2();

// unix2dos not needed: the windows PVSS understands the unix format
//mudcsSynchSystemPoratbleOS(oper_system,"cd "+ProjectHome+"/source/"+Component+"/unix2dos; ./macro_unix2dos "+Component);
//=================================================================================================
//mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/source/"+Component+"/unix2dos"+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/source/"+Component+"/.");
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+ProjectHome+"/source/"+Component+"/config"+" "+CSC_fwGSuper_g_HOME+"/"+Component+"/source/"+Component+"/.");

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
string command="PVSS00ascii -out "+CSC_fwGSuper_g_HOME+"/"+Component+"/dplist/"+Component+"/"+Component+".dpl -filterDpType _FwFsmObjectType ";

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
 command="PVSS00ascii -out "+CSC_fwGSuper_g_HOME+"/"+Component+"/dplist/"+Component+"/"+Component+"2.dpl ";
 
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
