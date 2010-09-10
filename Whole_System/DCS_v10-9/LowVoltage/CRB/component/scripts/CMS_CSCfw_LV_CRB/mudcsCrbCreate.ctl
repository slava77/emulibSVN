#uses "CMS_CSCfw_LV_CRB/emuDcsLv.ctl"
#uses "CMS_CSCfw_LV_CRB/emuDcsCrb.ctl"
//#uses "CMS_CSCfw_LV_CRB/emuDcsMrtn.ctl"
#uses "CMS_CSCfw_LV_CRB/emuDcsLvAlert.ctl"
#uses "CMS_CSCfw_LV_CRB/emuDcsCrbAlert.ctl"
//#uses "CMS_CSCfw_LV_CRB/emuDcsMrtnAlert.ctl"
#uses "CMS_CSCfw_LV_CRB/emufw.ctl"

#uses "CMS_CSCfw_LV_CRB/mudcs0GlobalElmbCreate.ctl"
#uses "CMS_CSCfw_LV_CRB/mudcs11CrbDbCreate.ctl"
#uses "CMS_CSCfw_LV_CRB/mudcs1HwCrbCreate.ctl"
#uses "CMS_CSCfw_LV_CRB/mudcs2HwPsuCreate.ctl"
#uses "CMS_CSCfw_LV_CRB/mudcs3TypeCrbCreate.ctl"
#uses "CMS_CSCfw_LV_CRB/mudcs4FsmCrbCreate.ctl"
#uses "CMS_CSCfw_LV_CRB/mudcs5ConfCrb_2PsuNodes.ctl"
#uses "CMS_CSCfw_LV_CRB/mudcs6AlertCrb.ctl"
#uses "CMS_CSCfw_LV_CRB/mudcs7AlertPsu.ctl"
#uses "CMS_CSCfw_LV_CRB/mudcs8SetPanelsCrb.ctl"
#uses "CMS_CSCfw_LV_CRB/mudcs9ArchiveCrb.ctl"

#uses "CMS_CSCfw_LV_CRB/mudcs9XAlertReconfigAllSlowControls.ctl"

#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_alert.ctl"

#uses "aes.ctl"

/** if this flag is set to true the full component installation is performed, otherwise only patch() is called. */
global const bool EMU_CRB_INSTALLATION_FULL = true;

main(){
  
  if (!EMU_CRB_INSTALLATION_FULL) {
    patch();
    exit(0); 
  }

 int state, count=0;
 while(1){
 DebugTN("POSTINSTALLING IS RUNNING: please wait");
 delay(5);
 dpGet("_OPCCANopen.ServerState",state);
 if(state==1)break;
 
 count++;
 if(count>5){
  DebugTN("CSC POSTINSTALL:  OPC server is NOT running, probably SIM is running?, I am starting the POSTINSTALL ");
  break; 
 }
 }

mudcsLv_deleteAllHw();


delay(5);
//====

 mudcsCrbDbCreateMain();
 mudcsCrbHWcreateMain(); 
 mudcsPsuHWcreateMain();
 mudcsTypeCrbCreateMain(); 
 mudcsCrbFsmCreateMain();
 mudcsCrbNodesConfigAccording2DbMain();
 mudcsAlertCrbMain();
 mudcsAlertPsuMain();
 mudcsLvCrbFsmPanelsAndFsmAliasesSetMain(); 
// mudcsArchiveCrbMain(); 
 
  mudcsAlertReconfig("fwCrb_CSC_LV", ".off_channels", true);
  emu_info("Updating all alert classes");
  emuAlert_updateAllAlertClasses();
  emu_info("Done updating alert classes");
  
  mudcsCrbDistConfig();
    
 exit(0);
}

void patch() {
  emu_info("Updating all alert classes");
  emuAlert_updateAllAlertClasses();
  emu_info("Done updating alert classes");
}
