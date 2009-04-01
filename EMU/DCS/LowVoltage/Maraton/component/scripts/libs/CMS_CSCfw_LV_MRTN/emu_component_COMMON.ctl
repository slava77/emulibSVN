//#uses "CMS_CSCfw_LV_CRB/emuDcsLv.ctl"
#uses "CMS_CSCfw_LV_MRTN/emuDcsLv.ctl"
//#uses "CMS_CSCfw_LV_CRB/emuDcsCrb.ctl"
#uses "CMS_CSCfw_LV_MRTN/emuDcsMrtn.ctl"
//#uses "CMS_CSCfw_LV_CRB/emuDcsLvAlert.ctl"
#uses "CMS_CSCfw_LV_MRTN/emuDcsLvAlert.ctl"
//#uses "CMS_CSCfw_LV_CRB/emuDcsCrbAlert.ctl"
#uses "CMS_CSCfw_LV_MRTN/emuDcsMrtnAlert.ctl"
//#uses "CMS_CSCfw_LV_CRB/emufw.ctl"
#uses "CMS_CSCfw_LV_MRTN/emufw.ctl"

//main(){
// system("cmd \\c dir");
//}
string key = "MRTN";

main() {
  mudcsLVCreateComponent(1);
  mudcsLVCreateComponent(2);
  DebugTN("done");

}


mudcsLVCreateComponent(int mode) {
//MRTN/CRB specific
  string OPC_SERVER_DIR;

  if (key == "MRTN") {
    mudcsMrtnPnlInit();
    OPC_SERVER_DIR = "C:\\WienerOPC";
  } else if (key == "CRB") {
    mudcsCrbPnlInit();
    OPC_SERVER_DIR = "C:\\CANopenOPC";
  }

//int mode=1; // mode =2 is browsing with all CSC panels: need for separate project installed on windows
  // mode=0 only FSM panel browsing // not needed : saved for historical reasons
  // mode=1  : is browsing with all CSC panels besides some: need for installation in CSC station project

  string key_add = "";
  if (mode == 1)key_add = "_MINI";

  string ProjectHome = "C:\\PVSS_PROJECT_36\\csc_lv";//+key;
  string LV_PROJECT = "CMS_CSCfw_LV_" + key + key_add;

//mudcsLvDebug("CYGWIN INSTALLED IS NEEDED!");


  string dir = "CMS_CSCfw_LV_" + key; // operation panel directory
  string dir_config = "CMS_CSCfw_LV_" + key + "\\config"; // configuration panel directory
//string ProjectHome="\\cygdrive\\c\\PVSS_PROJECT_36\\test_36";



//DebugTN("rm -r "+ProjectHome+"//CMS_CSCfw_LV");
  system("cmd /c rm -rf " + ProjectHome + "\\" + LV_PROJECT);


//return;

//DebugTN("cmd /c mkdir "+ProjectHome+"\\CMS_CSCfw_LV");
//system("cmd \\c which mkdir");
//system("cmd \\c mkdir "+ProjectHome+"\\CMS_CSCfw_LV");
  system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT);
//DebugTN("mkdir "+ProjectHome+"\\CMS_CSCfw_LV================");
//while(1){}

//return;

  system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\panels");
  system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\panels\\" + dir);
  if (mode == 2) {
    system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\panels\\" + dir_config);
  }

  system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\scripts");
  system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\scripts\\" + dir);
  system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\scripts\\libs");
  system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\scripts\\libs\\" + dir);

  system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\images");
  system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\images\\" + dir);

  system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\dplist");
  system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\dplist\\" + dir);

  system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\config");

  if (mode == 2) {
    system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\source");
    system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\source\\" + dir);
    system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\source\\" + dir + "\\special_stuff");
    system("cmd /c mkdir " + ProjectHome + "\\" + LV_PROJECT + "\\source\\" + dir + "\\config");
  }
//system("cmd /c mkdir "+ProjectHome+"\\"+LV_PROJECT+"\\panels\\CMS_CSCfw_LV");
//----------------------------------------------------------------

//system("rm -f "+ProjectHome+"\\"+LV_PROJECT+"\\panels/*");
//system("rm -f "+ProjectHome+"\\"+LV_PROJECT+"\\scripts\\libs/*");


  if (!mode) {
//MRTN/CRB specific
    if (key == "CRB")
      system("cmd /c cp " + ProjectHome + "\\panels\\" + dir + "\\emuPanelComponentTemplate.pnl" +
             " " + ProjectHome + "\\" + LV_PROJECT + "\\panels\\" + dir + "\\" + "emuDevfwCrb_CSC_LVOperation.pnl");

//MRTN/CRB specific
    if (key == "MRTN")
      system("cmd /c cp " + ProjectHome + "\\panels\\" + dir + "\\emuPanelComponentTemplate.pnl" +
             " " + ProjectHome + "\\" + LV_PROJECT + "\\panels\\" + dir + "\\" + "emuMRTNOperation.pnl");
    else if (key == "CRB")
      system("cmd /c cp " + ProjectHome + "\\panels\\" + dir + "\\emuPanelComponentTemplate.pnl" +
             " " + ProjectHome + "\\" + LV_PROJECT + "\\panels\\" + dir + "\\" + "emuPCOperation.pnl");

    system("cmd /c cp " + ProjectHome + "\\panels\\" + dir + "\\emufwUi.pnl" +
           " " + ProjectHome + "\\" + LV_PROJECT + "\\panels\\" + dir + "\\" + "emufwUi.pnl");
  } else {

    if (mode == 1) {


      system("cmd /c cp " + ProjectHome + "\\panels\\" + dir + "/*.pnl" +
             " " + ProjectHome + "\\" + LV_PROJECT + "\\panels\\" + dir + "\\" + ".");
      system("cmd /c cp " + ProjectHome + "\\panels\\" + dir + "\\emufwUi.pnl" +
             " " + ProjectHome + "\\" + LV_PROJECT + "\\panels\\" + dir + "\\" + "emufwUi.pnl");

    }

    else if (mode == 2) {
      system("cmd /c cp " + ProjectHome + "\\panels\\" + dir + "/*.pnl" +
             " " + ProjectHome + "\\" + LV_PROJECT + "\\panels\\" + dir + "\\" + ".");
      system("cmd /c cp " + ProjectHome + "\\panels\\" + dir + "\\emufwUi.pnl" +
             " " + ProjectHome + "\\" + LV_PROJECT + "\\panels\\" + dir + "\\" + "emufwUi.pnl");

      system("cmd /c cp " + ProjectHome + "\\panels\\" + dir_config + "/*.pnl" +
             " " + ProjectHome + "\\" + LV_PROJECT + "\\panels\\" + dir_config + "\\" + ".");
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

  if (mode) {
    system("cmd /c cp " + ProjectHome + "\\scripts\\libs\\" + dir + "/emu*.ctl " +
           ProjectHome + "\\" + LV_PROJECT + "\\scripts\\libs\\" + dir + "\\.");
    ///if(mode==2)
    system("cmd /c cp " + ProjectHome + "\\scripts\\libs\\" + dir + "/mudcs*.ctl " +
           ProjectHome + "\\" + LV_PROJECT + "\\scripts\\libs\\" + dir + "\\.");
  } else system("cmd /c cp " + ProjectHome + "\\scripts\\libs\\" + dir + "/emufw.ctl " +
                  ProjectHome + "\\" + LV_PROJECT + "\\scripts\\libs\\" + dir + "\\.");


//system("cmd /c cp "+ProjectHome+"\\"+LV_PROJECT+"\\panels/* "+" \\home\\fast\\"+LV_PROJECT+"\\panels\\"+dir+"\\.");
//=========== CONFIG ===============================================================================================

  if (!mode || mode == 1)system("cmd /c cp " + ProjectHome + "\\source\\" + dir + "\\config\\" + dir + "_short.config " +
                                  ProjectHome + "\\" + LV_PROJECT + "\\config\\" + dir + ".config");
  else system("cmd /c cp " + ProjectHome + "\\source\\" + dir + "\\config\\" + dir + "_long.config " +
                ProjectHome + "\\" + LV_PROJECT + "\\config\\" + dir + ".config");

  if (mode == 2) {
    system("cmd /c cp " + ProjectHome + "\\source\\" + dir + "\\config/" + dir + "*.config " +
           ProjectHome + "\\" + LV_PROJECT + "\\source\\" + dir + "\\config/.");

//  system("cmd /c cp "+ProjectHome+"\\scripts\\"+dir+"/*.postInstall "+
//                ProjectHome+"\\"+LV_PROJECT+"\\scripts\\"+dir+"/"+dir+".postInstall");
    system("cmd /c cp " + ProjectHome + "\\scripts\\" + dir + "/* " +
           ProjectHome + "\\" + LV_PROJECT + "\\scripts\\" + dir + "/.");
//system("cmd /c cp "+ProjectHome+"\\source\\"+dir+"\\special_stuff/*cfg* "+
//       OPC_SERVER_DIR+"/.");
//DebugTN("cmd /c cp "+ProjectHome+"\\source\\"+dir+"\\special_stuff/*cfg* "+
//       OPC_SERVER_DIR+"/.");
  }

//====  special stuff ======================================================================================
//+"\\source\\"+dir+"\\special_stuff
  if (mode == 2) {
    system("cmd /c cp " + ProjectHome + "\\source\\" + dir + "\\special_stuff/* " + ProjectHome + "\\" + LV_PROJECT + "\\source\\" + dir + "\\special_stuff/.");
  }
//===================================================================================================================
//==================================================================================================================
//============= XML ================================================================================================
  if (mode == 2) {
    system("cmd /c cp " + ProjectHome + "\\source\\" + dir + "\\config\\" + dir + "_long.xml " + ProjectHome + "\\" + LV_PROJECT + "\\" + dir + ".xml");
  } else system("cmd /c cp " + ProjectHome + "\\source\\" + dir + "\\config\\" + dir + "_short.xml " + ProjectHome + "\\" + LV_PROJECT + "\\" + dir + ".xml");

  if (mode == 2) {
    system("cmd /c cp " + ProjectHome + "\\source\\" + dir + "\\config/*.xml " + ProjectHome + "\\" + LV_PROJECT + "\\source\\" + dir + "\\config/.");
  }
//===================================================================================================================
  dplist(mode, ProjectHome, LV_PROJECT, dir);
//DebugTN("done");
}

//======================================

dplist(int mode, string ProjectHome, string LV_PROJECT, string dir) {


  dyn_string all_dps, dps;
  int i;

//MRTN/CRB specific
  dynAppend(all_dps, "fwOT_CSC_LV_NODES");
  dps = dpNames("fwOT_CSC_LV_NODES.*");
  dynAppend(all_dps, dps);

  if (mode) {

    dynAppend(all_dps, "fwOT_CSC_LV_TREE_NODES");
    dps = dpNames("fwOT_CSC_LV_TREE_NODES.*");
    dynAppend(all_dps, dps);

    if (key == "CRB") {
      if (mode == 2) {
        dynAppend(all_dps, "fwOT_fwCrb_CSC_LV");
        dps = dpNames("fwOT_fwCrb_CSC_LV.*");
        dynAppend(all_dps, dps);

        dynAppend(all_dps, "fwOT_FwElmbPSUBranch");
        dps = dpNames("fwOT_FwElmbPSUBranch.*");
        dynAppend(all_dps, dps);
      }
    } else if (key == "MRTN") {
      if (mode == 2) {
        dynAppend(all_dps, "fwOT_FwWienerMarathonChannel");
        dps = dpNames("fwOT_FwWienerMarathonChannel.*");
        dynAppend(all_dps, dps);

        dynAppend(all_dps, "fwOT_FwWienerMarathon");
        dps = dpNames("fwOT_FwWienerMarathon.*");
        dynAppend(all_dps, dps);

        dynAppend(all_dps, "fwOT_FwWienerChannel");
        dps = dpNames("fwOT_FwWienerChannel.*");
        dynAppend(all_dps, dps);

        dynAppend(all_dps, "fwOT_FwWienerPS");
        dps = dpNames("fwOT_FwWienerPS.*");
        dynAppend(all_dps, dps);

      } // if(mode==2)
    }

  }


//all_dps=makeDynString();

//=================

//string command="PVSS00ascii -out dplist\\migr_to_new_project.dpl ";
  string command = "cmd /c PVSS00ascii -out " + ProjectHome + "\\" + LV_PROJECT + "\\dplist\\" + dir + "\\" + dir + ".dpl -filterDpType _FwFsmObjectType ";

  for (i = 1; i <= dynlen(all_dps); i++) {
    command += " -filterDp " + all_dps[i];
  }


  if (mode == 2) {
    dps = dpNames("*", "MUDCS_DYN_STRING");

    command += " -filterDpType MUDCS_DYN_STRING";
    for (i = 1; i <= dynlen(dps); i++)command += " -filterDp " + dps[i];

    dps = dpNames("*", "MUDCS_STRING");

    command += " -filterDpType MUDCS_STRING";
    for (i = 1; i <= dynlen(dps); i++)command += " -filterDp " + dps[i];

  }
  DebugTN(command);


  DebugTN("-------");
  system(command);
  DebugTN("-------");

//system("cmd /c cp "+ProjectHome+"\\"+LV_PROJECT+"\\dplist\\CMS_CSCfw_LV.dpl "+
//       ProjectHome+"\\"+LV_PROJECT+"\\dplist\\"+LV_PROJECT+"\\CMS_CSCfw_LV.dpl");

////// system("cd "+ProjectHome+"\\unix2dos; .\\macro_unix2dos");
}
