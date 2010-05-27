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

#uses "aes.ctl"


main(){
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
