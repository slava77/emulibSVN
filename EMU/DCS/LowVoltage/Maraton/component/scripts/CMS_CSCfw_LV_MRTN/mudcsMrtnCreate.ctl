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


#uses "CMS_CSCfw_LV_MRTN/mudcs0GlobalMrtnCreate.ctl"
#uses "CMS_CSCfw_LV_MRTN/mudcs1HwMrtnCreate.ctl"
#uses "CMS_CSCfw_LV_MRTN/mudcs22MrtnDbCreate.ctl"
#uses "CMS_CSCfw_LV_MRTN/mudcs2FsmMrtnCreate.ctl"
#uses "CMS_CSCfw_LV_MRTN/mudcs3AlertMrtn.ctl"
#uses "CMS_CSCfw_LV_MRTN/mudcs4SetPanelsMrtn.ctl"
#uses "CMS_CSCfw_LV_MRTN/mudcs5ArchiveMrtn.ctl"
#uses "fwDeviceEditorNavigator/fwDeviceEditorNavigator.ctl"
#uses "fwDeviceEditorNavigator/fwDeviceEditorNavigatorClipboard.ctl"

#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_alert.ctl"

#uses "aes.ctl"

/** if this flag is set to true the full component installation is performed, otherwise only patch() is called. */
global const bool EMU_MRTN_INSTALLATION_FULL = false;


main(){
  
  if (!EMU_MRTN_INSTALLATION_FULL) {
    patch();
    exit(0); 
  }
  
  
 int state, state1, count=0;
 while(1){
 
 DebugTN("POSTINSTALLING IS RUNNING: please wait");
 delay(5);
 dpGet("_WienerOPCServer.ServerState",state);
 dpGet("_WienerMarathonOPCServer.ServerState",state1);
 if(state==1 || state1==1)break;
 count++;
 if(count>5){
  DebugTN("CSC POSTINSTALL:  OPC server is NOT running, probably SIM is running?, I am starting the POSTINSTALL ");
  break;
  }
 }  
  
 //return;
mudcsLv_deleteAllHw();

delay(5);
//==== 

  mudcsMrtnHWcreateMain();
  mudcsMrtnDbCreateMain();
  mudcsMrtnFsmCreateMain(); 
  mudcsAlertMrtnMain();
  mudcsLvMrtnFsmPanelsAndFsmAliasesSetMain();  
//  mudcsArchiveMrtnMain();   
  exit(0);
}

void patch() {
  emu_info("Updating all alert classes");
  emuAlert_updateAllAlertClasses();
  emu_info("Done updating alert classes");
}
