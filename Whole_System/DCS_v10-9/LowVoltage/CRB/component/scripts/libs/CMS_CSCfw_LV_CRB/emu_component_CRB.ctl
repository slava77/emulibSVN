//main(){
// system("cmd \\c dir"); 
//}

main()
{

  
mudcsCrbPnlInit();
string ProjectHome="C:\\PVSS_PROJECT_36\\csc_crb";
string LV_PROJECT="CMS_CSCfw_LV_CRB";


//mudcsLvDebug("CYGWIN INSTALLED IS NEEDED!");

int mode=2; // mode =2 is browsing with all CSC panels
            // mode=0 only FSM panel browsing
            // mode=1  : is browsing with all CSC panels besides some

string dir="CMS_CSCfw_LV_CRB"; // operation panel directory 
string dir_config="CMS_CSCfw_LV_CRB\\config"; // configuration panel directory
//string ProjectHome="\\cygdrive\\c\\PVSS_PROJECT_36\\test_36";



//DebugTN("rm -r "+ProjectHome+"//CMS_CSCfw_LV");
system("cmd /c rm -rf "+ProjectHome+"\\"+LV_PROJECT);


//return;

//DebugTN("cmd /c mkdir "+ProjectHome+"\\CMS_CSCfw_LV");
//system("cmd \\c which mkdir");
//system("cmd \\c mkdir "+ProjectHome+"\\CMS_CSCfw_LV");
system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT);
//DebugTN("mkdir "+ProjectHome+"\\CMS_CSCfw_LV================");
//while(1){}

//return;

system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\panels");
system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\"+dir);
system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\"+dir_config);

system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\scripts");
system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\scripts\\"+dir);
system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\scripts\\libs");
system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\scripts\\libs\\"+dir);

system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\images");
system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\images\\"+dir);

system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\dplist");
system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\dplist\\"+dir);

system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\config");

system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\source");
system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\source\\"+dir);
system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\source\\"+dir+"\\special_stuff");
system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\source\\"+dir+"\\config");
//system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\CMS_CSCfw_LV");
//----------------------------------------------------------------

//system("rm -f "+ProjectHome+"\\"+LV_PROJECT+"\\panels/*");
//system("rm -f "+ProjectHome+"\\"+LV_PROJECT+"\\scripts\\libs/*");


if(!mode){
 system("cmd /c cp "+ProjectHome+"\\panels\\"+dir+"\\emuPanelComponentTemplate.pnl"+
          " "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\"+dir+"\\"+"emuDevfwCrb_CSC_LVOperation.pnl");

 system("cmd /c cp "+ProjectHome+"\\panels\\"+dir+"\\emuPanelComponentTemplate.pnl"+
          " "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\"+dir+"\\"+"emuPCOperation.pnl");
 
 system("cmd /c cp "+ProjectHome+"\\panels\\"+dir+"\\emufwUi_short.pnl"+
          " "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\"+dir+"\\"+"emufwUi.pnl");   
}
else{
  
  if(mode==1){   
// system("cmd /c cp "+ProjectHome+"\\panels\\"+dir+"\\emuDevfwCrb_CSC_LVOperation.pnl"+
//          " "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\.");
// system("cmd /c cp "+ProjectHome+"\\panels\\"+dir+"\\emuPCOperation.pnl"+
//          " "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\."); 
 
 system("cmd /c cp "+ProjectHome+"\\panels\\"+dir+"/*.pnl"+
          " "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\"+dir+"\\"+"."); 
 system("cmd /c cp "+ProjectHome+"\\panels\\"+dir+"\\emufwUi_long.pnl"+
          " "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\"+dir+"\\"+"emufwUi.pnl"); 
 
  }
  
  else if(mode==2){
 system("cmd /c cp "+ProjectHome+"\\panels\\"+dir+"/*.pnl"+
          " "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\"+dir+"\\"+"."); 
 system("cmd /c cp "+ProjectHome+"\\panels\\"+dir+"\\emufwUi_long.pnl"+
          " "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\"+dir+"\\"+"emufwUi.pnl");

 system("cmd /c cp "+ProjectHome+"\\panels\\"+dir_config+"/*.pnl"+
          " "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\"+dir_config+"\\"+".");      
  }
 
}
//=================================
/*
if(!mode)system("cmd /c cp "+ProjectHome+"\\panels\\"+dir+"\\emufwUi_short.pnl"+" "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\emufwUi.pnl");
else {
 system("cmd /c cp "+ProjectHome+"\\panels\\"+dir+"/*.pnl"+" "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\.");
 if(mode==1)system("cmd /c cp "+ProjectHome+"\\panels\\"+dir+"\\emufwUi_medium.pnl"+" "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\emufwUi.pnl");
 else if(mode==2)system("cmd /c cp "+ProjectHome+"\\panels\\"+dir+"\\emufwUi_long.pnl"+" "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\emufwUi.pnl");

}
*/


//if(mode)system("cmd /c cp "+ProjectHome+"\\scripts\\libs\\"+LV_PROJECT+"\\emu*.ctl"+" "+ProjectHome+"\\"+LV_PROJECT+"\\scripts\\libs\\.");
//else system("cmd /c cp "+ProjectHome+"\\scripts\\libs\\"+LV_PROJECT+"\\emufw.ctl"+" "+ProjectHome+"\\"+LV_PROJECT+"\\scripts\\libs\\.");


//---forming package: EMU DCS components------------
/*
system("rm \\home\\fast\\"+LV_PROJECT+"\\scripts\\libs\\CMS_CSCfw_LV/*");
system("rm \\home\\fast\\"+LV_PROJECT+"\\panels\\"+dir+"/*");
system("rm \\home\\fast\\"+LV_PROJECT+"\\panels\\"+dir_config+"/*");
system("rm \\home\\fast\\"+LV_PROJECT+"\\dplist\\CMS_CSCfw_LV/*");
system("rm \\home\\fast\\"+LV_PROJECT+"\\images\\CMS_CSCfw_LV/*");
system("rm \\home\\fast\\"+LV_PROJECT+"\\config/*");
*/

if(mode){
  system("cmd /c cp "+ProjectHome+"\\scripts\\libs\\CMS_CSCfw_LV_CRB/emu*.ctl "+
                          ProjectHome+"\\"+LV_PROJECT+"\\scripts\\libs\\"+dir+"\\.");
  if(mode==2)system("cmd /c cp "+ProjectHome+"\\scripts\\libs\\CMS_CSCfw_LV_CRB/mudcs*.ctl "+
                          ProjectHome+"\\"+LV_PROJECT+"\\scripts\\libs\\"+dir+"\\.");
}
else system("cmd /c cp "+ProjectHome+"\\scripts\\libs\\CMS_CSCfw_LV_CRB/emufw.ctl "+
                          ProjectHome+"\\"+LV_PROJECT+"\\scripts\\libs\\"+dir+"\\.");


//system("cmd /c cp "+ProjectHome+"\\"+LV_PROJECT+"\\panels/* "+" \\home\\fast\\"+LV_PROJECT+"\\panels\\"+dir+"\\.");
//=========== CONFIG ===============================================================================================

if(!mode)system("cmd /c cp "+ProjectHome+"\\source\\"+dir+"\\config\\CMS_CSCfw_LV_CRB_short.config "+
                ProjectHome+"\\"+LV_PROJECT+"\\config\\CMS_CSCfw_LV_CRB.config");
else system("cmd /c cp "+ProjectHome+"\\source\\"+dir+"\\config\\CMS_CSCfw_LV_CRB_long.config "+
                ProjectHome+"\\"+LV_PROJECT+"\\config\\CMS_CSCfw_LV_CRB.config");

if(mode==2){
  system("cmd /c cp "+ProjectHome+"\\source\\"+dir+"\\config/CMS_CSCfw_LV_CRB*.config "+
                ProjectHome+"\\"+LV_PROJECT+"\\source\\"+dir+"\\config/.");  

//  system("cmd /c cp "+ProjectHome+"\\scripts\\"+dir+"/*.postInstall "+
//                ProjectHome+"\\"+LV_PROJECT+"\\scripts\\"+dir+"/CMS_CSCfw_LV_CRB.postInstall");
 system("cmd /c cp "+ProjectHome+"\\scripts\\"+dir+"/* "+
                ProjectHome+"\\"+LV_PROJECT+"\\scripts\\"+dir+"/.");          
}

//====  special stuff ======================================================================================
//+"\\source\\"+dir+"\\special_stuff
system("cmd /c cp "+ProjectHome+"\\source\\"+dir+"\\special_stuff/* "+ProjectHome+"\\"+LV_PROJECT+"\\source\\"+dir+"\\special_stuff/.");
//===================================================================================================================
//==================================================================================================================
//============= XML ================================================================================================
if(mode==2){
system("cmd /c cp "+ProjectHome+"\\source\\"+dir+"\\config\\CMS_CSCfw_LV_CRB_long.xml "+ProjectHome+"\\"+LV_PROJECT+"\\CMS_CSCfw_LV_CRB.xml");
}
else system("cmd /c cp "+ProjectHome+"\\source\\"+dir+"\\config\\CMS_CSCfw_LV_CRB_short.xml "+ProjectHome+"\\"+LV_PROJECT+"\\CMS_CSCfw_LV_CRB.xml");

if(mode==2){
system("cmd /c cp "+ProjectHome+"\\source\\"+dir+"\\config/*.xml "+ProjectHome+"\\"+LV_PROJECT+"\\source\\"+dir+"\\config/.");
}
//===================================================================================================================
dplist(mode, ProjectHome, LV_PROJECT, dir);
DebugTN("done");
}

//======================================

dplist(int mode, string ProjectHome, string LV_PROJECT, string dir){

dyn_string all_dps, dps;
int i;

dynAppend(all_dps,"fwOT_CSC_LV_NODES");
dps=dpNames("fwOT_CSC_LV_NODES.*");
dynAppend(all_dps,dps);

if(mode==2){
dynAppend(all_dps,"fwOT_CSC_LV_TREE_NODES");
dps=dpNames("fwOT_CSC_LV_TREE_NODES.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_fwCrb_CSC_LV");
dps=dpNames("fwOT_fwCrb_CSC_LV.*");
dynAppend(all_dps,dps);
}

/*
dynAppend(all_dps,"fwOT_ALNM_1");
dps=dpNames("fwOT_ALNM_1.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_MRTN_1");
dps=dpNames("fwOT_MRTN_1.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_CRB_1");
dps=dpNames("fwOT_CRB_1.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_WNR12_1");
dps=dpNames("fwOT_WNR12_1.*");
dynAppend(all_dps,dps);

dynAppend(all_dps,"fwOT_HV_PR");
dps=dpNames("fwOT_HV_PR.*");
dynAppend(all_dps,dps);

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

dynAppend(all_dps,"fwOT_HV_1");
dps=dpNames("fwOT_HV_1.*");
dynAppend(all_dps,dps);

*/
//all_dps=makeDynString();

//=================

//string command="PVSS00ascii -out dplist\\migr_to_new_project.dpl ";
string command="cmd /c PVSS00ascii -out "+ProjectHome+"\\"+LV_PROJECT+"\\dplist\\"+dir+"\\CMS_CSCfw_LV_CRB.dpl -filterDpType _FwFsmObjectType ";

 for(i=1;i<=dynlen(all_dps);i++){
   command += " -filterDp "+all_dps[i];
 }
   command += " -filterDpType MUDCS_DYN_STRING";


DebugTN(command);


DebugTN("-------");
system(command);
DebugTN("-------");

//system("cmd /c cp "+ProjectHome+"\\"+LV_PROJECT+"\\dplist\\CMS_CSCfw_LV.dpl "+
//       ProjectHome+"\\"+LV_PROJECT+"\\dplist\\"+LV_PROJECT+"\\CMS_CSCfw_LV.dpl");

////// system("cd "+ProjectHome+"\\unix2dos; .\\macro_unix2dos");
}
