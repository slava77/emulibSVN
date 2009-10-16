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

main(){
  
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
  
 mudcsCrbDbCreateMain();
 mudcsCrbHWcreateMain(); 
 mudcsPsuHWcreateMain();
 mudcsTypeCrbCreateMain(); 
 mudcsCrbFsmCreateMain();
 mudcsCrbNodesConfigAccording2DbMain();
 mudcsAlertCrbMain();
 mudcsAlertPsuMain();
 mudcsLvCrbFsmPanelsAndFsmAliasesSetMain(); 
 mudcsArchiveCrbMain(); 
 
  mudcsAlertReconfig("fwCrb_CSC_LV", ".off_channels", true);

  mudcsCrbDistConfig();
    
 exit(0);
}