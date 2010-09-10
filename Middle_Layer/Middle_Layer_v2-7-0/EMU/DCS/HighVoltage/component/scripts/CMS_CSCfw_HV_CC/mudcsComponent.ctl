
int mode;//=1; // mode =2 is browsing with all CSC panels
            // mode=0 only FSM panel browsing
            // mode=1  : is browsing with all CSC panels besides some

string Component;//="CMS_CSCfw_HV_CC";
string dir_oper;
bool custom_lv;
string oper_system, add_to_system_command;

main()
{

DebugTN("1");  

oper_system=getenv("OS");
add_to_system_command="";
if (oper_system !="Linux")add_to_system_command="cmd /c ";

DebugTN(oper_system);

mudcsCreateComponent(1);
mudcsCreateComponent(2);
mudcsDebug("done");
}

mudcsCreateComponent(int mode_par){  

  
Component="CMS_CSCfw_HV_CC";
mode=mode_par;    
mudcsInit();

DebugTN("2");  
custom_lv=CSC_fwG_g_CUSTOM_LV; 



dir_oper=Component;

string key_add="";
if(mode==1)key_add="_MINI";

Component+=key_add;

string dir_config=dir_oper+"/config";
string dir_init=dir_oper+"/init";
if(oper_system!="Linux"){
 dir_config=dir_oper+"\\config";
 dir_init=dir_oper+"\\init"; 
}

string dir_dubna="CMS_ME11";

DebugTN(oper_system+">>>");
//---forming package: EMU DCS components------------
mudcsSynchSystemPoratbleOS(oper_system,"rm -rf "+CSC_fwG_g_HOME+"/"+Component);
DebugTN("3");
if(oper_system!="Linux")mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME);
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component);
DebugTN(add_to_system_command+"mkdir "+CSC_fwG_g_HOME+"/"+Component);
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/scripts");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/scripts/"+dir_oper);
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/scripts/libs");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/scripts/libs/"+dir_oper);

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/panels");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper);
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_config);
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_init);

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/config/");

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/dplist/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/dplist/"+dir_oper+"/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/images/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/images/"+dir_oper+"/");

mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/source/");
mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/source/"+dir_oper+"/");
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/source/"+Component+"/special_stuff");
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/source/"+Component+"/unix2dos");
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/source/"+Component+"/delete_processes_by_name");
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/source/"+Component+"/config");
//mudcsSynchSystemPoratbleOS(oper_system,"mkdir "+CSC_fwG_g_HOME+"/"+Component+"/source/"+Component+"/text");

/////////////###########mudcsSynchSystemPoratbleOS(oper_system,"rm -f "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/*");
/////////////#############mudcsSynchSystemPoratbleOS(oper_system,"rm -f "+CSC_fwG_g_project_name_home+"/CSCfw/scripts/libs/*");





if(!mode){
 DebugTN("cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emuDevWTH_SX5Operation.pnl");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emuDevWTH_SX5Operation.pnl");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emuDevALNM_1Operation.pnl");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emuDevCHIP_1Operation.pnl");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emuDevCRB_1Operation.pnl");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emuDevGAS_SX5Operation.pnl");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emuDevTEMP_1Operation.pnl");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emuDevWNR12_1Operation.pnl");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emuDevHV_1Operation.pnl");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emuDevHV_PROperation.pnl");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emuDevLV_1Operation.pnl");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emuDevMRTN_1Operation.pnl");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emuDevPT100Operation.pnl");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emuDevfwWnrCr_CSC_LVOperation.pnl");
}
else{
 DebugTN("------>>>  cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuDevWTH_SX5Operation.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/."); 
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuDevWTH_SX5Operation.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuDevALNM_1Operation.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuDevCHIP_1Operation.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuDevCRB_1Operation.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuDevGAS_SX5Operation.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuDevTEMP_1Operation.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuDevWNR12_1Operation.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuDevHV_1Operation.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuDevHV_PROperation.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuDevLV_1Operation.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuDevMRTN_1Operation.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuDevPT100Operation.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuDevfwWnrCr_CSC_LVOperation.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
}
//=================================


if(!mode)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emufwUi.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emufwUi.pnl");
else {
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/*.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
 if(mode==1)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emufwUi.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emufwUi.pnl");
 else if(mode==2)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emufwUi.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/emufwUi.pnl");

}


//=================================
string fwUiPnl;
//if(!mode)
 fwUiPnl=dir_oper+"/emufwUi.pnl";
//else 
// fwUiPnl="fwFSMuser/fwUi.pnl";


//=================================================================

dyn_string os;
string o,s, s1, s2, s3;
int i, pos;
dyn_string panels;
string p;

  dyn_string d_string1, d_string2;
  string label, o_cut;
//----------------------------------------------------------------- 
 
dyn_string objs;

//==================================================================

objs=dpNames("*","_FwFsmObject");
//dyn_string os;
//string o,s;
//int i, pos;
//dyn_string panels;
//string p;



for(i=1;i<=dynlen(objs);i++){

d_string1=strsplit(objs[i],"|");
d_string2=strsplit(d_string1[dynlen(d_string1)],"/");
o_cut=d_string2[dynlen(d_string2)];
    
  panels=makeDynString(fwUiPnl);


//mudcsDebug(objs[i]);
os=strsplit(objs[i],"|");
o=os[dynlen(os)];

////////DebugTN(o);



s="csc";
s1="emu";
s2="dcspcS2G19";
s3="CMS_CSC";
// if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
  if((pos=strpos(o,s) )>=0 || (pos=strpos(o,s1) )>=0 || (pos=strpos(o,s2) )>=0 ||(pos=strpos(o,s3) )>=0  ){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuEmuOperationS.pnl";  // ****** S was added !!!!!!!
mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
if(mode==2)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

  continue;
 }


//return;
  
s="_PSU";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
//p="emuLVSubSystemOperation.pnl";
 p="emuEmptyOperation.pnl";  
//mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
//if(mode==2)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

  continue;
 }  
  
s="_LV_HV";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
//p="emuLVSubSystemOperation.pnl";
 p="emuLV_MRTNOperation.pnl";  
//mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
//if(mode==2)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

  continue;
 }
  
s="_HV";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuHVSubSystemOperation.pnl";
mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
if(mode)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

  continue;
 }
s="_LV";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuLVSubSystemOperation.pnl";
mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
if(mode)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

  continue;
 }

 
s="_WIENER_CRATE_SNMP";
 if((pos=strpos(o,s))>=0 ){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
 p="emuWienerOperation.pnl";
 //p="emuPanelComponentTemplate.pnl";  
mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
if(mode)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

  continue;
 }
  
 
 
s="_GAS";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuGASSubSystemOperation.pnl";
mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
if(mode)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

  continue;
 }

s="_WTH";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuWTHSubSystemOperation.pnl";
mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
if(mode)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

  continue;
 }

s="HV_HOSTS";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuHV_HOSTSOperation.pnl";
mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
if(mode)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

  continue;
 }
 s="HV_HOST_";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuHV_HOSTSOperation.pnl";
mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
if(mode)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

  continue;
 }
 
s="LV_MRTN";
s1="_CR";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
   if(strpos(o,s1)>=0){
    p="emuMRTNOperation.pnl";

   }
   else{  
//panels=makeDynString("fwFSMuser/fwUi.pnl");
    if(CSC_fwG_g_CUSTOM_LV)p="emuLV_MRTNOperation.pnl";
    else p="emuEmptyOperation.pnl";

mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
if(mode)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");    
   }
   


  continue;
 }

s="LV_CRB";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuPCOperation.pnl";
mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
if(mode)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

  continue;
 }

s="ME_P"; s1="ME_M"; 
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-5) ||
    (pos=strpos(o,s1))>=0 && pos==(strlen(o)-5))  {
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuDiskOperationS.pnl"; // ****** S was added !!!!!!!
mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
////if(mode==2)
if(mode)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

  continue;
 }


s="_TS_"; 
 if((pos=strpos(o,s))>=0)  {
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuSectorOperation.pnl";
mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
if(mode)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

  continue;
 }

s="ME_P"; s1="ME_M"; 
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-10) ||
    (pos=strpos(o,s1))>=0 && pos==(strlen(o)-10))  {
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuChamberOperation.pnl";
mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/emuPanelComponentTemplate.pnl"+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/"+p);
if(mode)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/panels/"+dir_oper+"/"+p+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

  continue;
 }






} //for






if(mode)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/scripts/libs/"+dir_oper+"/emu*.ctl"+" "+CSC_fwG_g_HOME+"/"+Component+"/scripts/libs/"+dir_oper+"/.");
///if(mode==2)
mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/scripts/libs/"+dir_oper+"/mudcs*.ctl"+" "+CSC_fwG_g_HOME+"/"+Component+"/scripts/libs/"+dir_oper+"/.");
////else 
mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/scripts/libs/"+dir_oper+"/emufw.ctl"+" "+CSC_fwG_g_HOME+"/"+Component+"/scripts/libs/"+dir_oper+"/.");



//======== COPYING EVERYTHING FOR THE WHOLE PROJECT===============================================
if(mode==2){

mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+CSC_fwG_g_project_name_home+"/scripts/"+dir_oper+"/*.ctl"+" "+CSC_fwG_g_HOME+"/"+Component+"/scripts/"+dir_oper+"/.");  
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+CSC_fwG_g_project_name_home+"/scripts/"+dir_oper+"/"+dir_oper+".postInstall"+" "+CSC_fwG_g_HOME+"/"+Component+"/scripts/"+dir_oper+"/."); 

mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+CSC_fwG_g_project_name_home+"/panels/"+dir_config+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+CSC_fwG_g_project_name_home+"/panels/"+dir_init+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+CSC_fwG_g_project_name_home+"/panels/"+dir_dubna+             " "+CSC_fwG_g_HOME+"/"+Component+"/panels/.");



//mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+CSC_fwG_g_project_name_home+"/panles/"+dir_dubna+"/"+dir_init+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+Component+"/.");

}
//================================================================================================


////////#########mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/CSCfw/scripts/libs/* "+" "+CSC_fwG_g_HOME+"/"+Component+"/scripts/libs/CMS_CSCfw/.");
///////##########mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/CSCfw/panels/* "+" "+CSC_fwG_g_HOME+"/"+Component+"/panels/"+dir_oper+"/.");

if(mode<=1)mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/source/"+dir_oper+"/config/"+dir_oper+"_short.config "+
                                  " "+CSC_fwG_g_HOME+"/"+Component+"/config/"+dir_oper+".config");
else {
  mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/source/"+dir_oper+"/config/"+dir_oper+"_long.config "+
                                  " "+CSC_fwG_g_HOME+"/"+Component+"/config/"+dir_oper+".config");
  mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/source/"+dir_oper+"/config/"+dir_oper+"_short.config "+
                                  " "+CSC_fwG_g_HOME+"/"+Component+"/config/"+dir_oper+"_MINI.config");
}

if(mode==2){
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/source/"+dir_oper+"/config/"+dir_oper+"_long.xml "+" "+CSC_fwG_g_HOME+"/"+Component+"/"+dir_oper+".xml");
 mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/source/"+dir_oper+"/config/"+dir_oper+"_short.xml "+" "+CSC_fwG_g_HOME+"/"+Component+"/"+dir_oper+"_MINI.xml");
}
else mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/source/"+dir_oper+"/config/"+dir_oper+"_short_mode1.xml "+" "+CSC_fwG_g_HOME+"/"+Component+"/"+dir_oper+".xml");


if(mode==2){
  dplist(1);
  dplist(2);
  dplist2();
  mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/dplist/"+dir_oper+"/"+dir_oper+"_ARCHIVE.dpl "+
                                  " "+CSC_fwG_g_HOME+"/"+Component+"/dplist/"+dir_oper+"/"+dir_oper+"_ARCHIVE.dpl");  
}
else{
  dplist(mode);
}

if(oper_system=="Linux")mudcsSynchSystemPoratbleOS(oper_system,"cd "+CSC_fwG_g_project_name_home+"/source/"+dir_oper+"/unix2dos; ./macro_unix2dos "+Component);
//=================================================================================================
if(mode==2){
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+CSC_fwG_g_project_name_home+"/source/"+dir_oper+"/delete_processes_by_name"+" "+CSC_fwG_g_HOME+"/"+Component+"/source/"+dir_oper+"/.");
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+CSC_fwG_g_project_name_home+"/source/"+dir_oper+"/unix2dos"+" "+CSC_fwG_g_HOME+"/"+Component+"/source/"+dir_oper+"/.");
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+CSC_fwG_g_project_name_home+"/source/"+dir_oper+"/special_stuff"+" "+CSC_fwG_g_HOME+"/"+Component+"/source/"+dir_oper+"/.");
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+CSC_fwG_g_project_name_home+"/source/"+dir_oper+"/config"+" "+CSC_fwG_g_HOME+"/"+Component+"/source/"+dir_oper+"/.");
mudcsSynchSystemPoratbleOS(oper_system,"cp -rp "+CSC_fwG_g_project_name_home+"/source/"+dir_oper+"/text"+" "+CSC_fwG_g_HOME+"/"+Component+"/source/"+dir_oper+"/.");
}
//mudcsDebug("done");
}

//======================================

dplist(int mode){

dyn_string all_dps, dps;
int i;

dynAppend(all_dps,"fwOT_EMUNodes");
dps=dpNames("fwOT_EMUNodes.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_CMS_CSC_ME11_HV_Type"); // dubna ME11 type
dps=dpNames("fwOT_CMS_CSC_ME11_HV_Type.*");// dubna ME11 type
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_EMUTREENodes");
dps=dpNames("fwOT_EMUTREENodes.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_EMUSTBYNodes");
dps=dpNames("fwOT_EMUSTBYNodes.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_EMUTREESTBYNodes");
dps=dpNames("fwOT_EMUTREESTBYNodes.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_CSC_LV_HV");
dps=dpNames("fwOT_CSC_LV_HV.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_EMUTREENodes_MAJOR");
dps=dpNames("fwOT_EMUTREENodes_MAJOR.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_EMUTREENodes_MAJOR_CMS");
dps=dpNames("fwOT_EMUTREENodes_MAJOR_CMS.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_EMUFEDNodes");
dps=dpNames("fwOT_EMUFEDNodes.*");
dynAppend(all_dps,dps);
//--
dynAppend(all_dps,"fwOT_EMUTREENodes_S"); // sector 
dps=dpNames("fwOT_EMUTREENodes_S.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_EMUTREENodes_S_save"); // sector 
dps=dpNames("fwOT_EMUTREENodes_S_save.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_EMUTREENodes_I");  // inner chambers
dps=dpNames("fwOT_EMUTREENodes_I.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_EMUTREENodes_O");  // outer chambers
dps=dpNames("fwOT_EMUTREENodes_O.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_HV_1");
dps=dpNames("fwOT_HV_1.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_HV_PR");
dps=dpNames("fwOT_HV_PR.*");
dynAppend(all_dps,dps);

if(mode==2){
/*
dynAppend(all_dps,"fwOT_ALNM_1");
dps=dpNames("fwOT_ALNM_1.*");
dynAppend(all_dps,dps);
*/

dynAppend(all_dps,"fwOT_MRTN_1");
dps=dpNames("fwOT_MRTN_1.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_fwWnrCr_CSC_LV");
dps=dpNames("fwOT_fwWnrCr_CSC_LV.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_fwWnrCh_CSC_LV");
dps=dpNames("fwOT_fwWnrCh_CSC_LV.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_CRB_1");
dps=dpNames("fwOT_CRB_1.*");
dynAppend(all_dps,dps);
/*
dynAppend(all_dps,"fwOT_WNR12_1");
dps=dpNames("fwOT_WNR12_1.*");
dynAppend(all_dps,dps);
*/
//dynAppend(all_dps,"fwOT_HV_PR");
//dps=dpNames("fwOT_HV_PR.*");
//dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_WTH_SX5");
dps=dpNames("fwOT_WTH_SX5.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_GAS_SX5");
dps=dpNames("fwOT_GAS_SX5.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_PT100");
dps=dpNames("fwOT_PT100.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_TEMP_1");
dps=dpNames("fwOT_TEMP_1.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_CHIP_1");
dps=dpNames("fwOT_CHIP_1.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_LV_1");
dps=dpNames("fwOT_LV_1.*");
dynAppend(all_dps,dps);

//dynAppend(all_dps,"fwOT_HV_1");
//dps=dpNames("fwOT_HV_1.*");
//dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_FED_1");
dps=dpNames("fwOT_FED_1.*");
dynAppend(all_dps,dps);
}
//all_dps=makeDynString();

//=================

//string command="PVSS00ascii -out dplist/migr_to_new_project.dpl ";
string command;
    
if(mode==2)
command="PVSS00ascii -out "+CSC_fwG_g_HOME+"/"+Component+"/dplist/"+dir_oper+"/"+dir_oper+".dpl -filterDpType _FwFsmObjectType ";
else
command="PVSS00ascii -out "+CSC_fwG_g_HOME+"/"+Component+"/dplist/"+dir_oper+"/"+dir_oper+"_MINI.dpl -filterDpType _FwFsmObjectType ";

 for(i=1;i<=dynlen(all_dps);i++){


   command += " -filterDp "+all_dps[i];

 }



DebugTN(command);


DebugTN("-------");
system(add_to_system_command+command);
DebugTN("-------");

////////////////#####################mudcsSynchSystemPoratbleOS(oper_system,"cp "+CSC_fwG_g_project_name_home+"/CSCfw/dplist/CMS_CSCfw.dpl "+" /home/fast/CMS_CSCfw/dplist/CMS_CSCfw/.");


}
//========================================================================
dplist2()
{

string command;


dyn_string all_types, dps;
dyn_dyn_string all_dps;
int i,j;

all_types=dpTypes("*MUDCS_*");

dynAppend(all_types,"_FwDimConfig");

dynAppend(all_types,"Db");
//dynAppend(all_types,"Db_GAS_SX5_alert");
//dynAppend(all_types,"Db_LV_1_alert");

dynAppend(all_types,"Db_TEMP1_alert");

/*
dynAppend(all_types,"ExampleDP_Int");
dynAppend(all_types,"int20");
*/
dynAppend(all_types,"FLOAT_FIVE");
dynAppend(all_types,"FLOAT_ONE");
dynAppend(all_types,"Wheel");
dynAppend(all_types,"Wheels");
dynAppend(all_types,"WheelRing"); /// ?????


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
 command="PVSS00ascii -out "+CSC_fwG_g_HOME+"/"+Component+"/dplist/"+dir_oper+"/"+dir_oper+"2.dpl ";
 
 for(i=1;i<=dynlen(all_types);i++){

  command += " -filterDpType "+all_types[i];
  for(j=1;j<=dynlen(all_dps[i]);j++){


   command += " -filterDp "+all_dps[i][j];


  } // j
 } // i



DebugTN(command);


DebugTN("-------");
system(add_to_system_command+command);
DebugTN("-------");

}


//========================================================================
