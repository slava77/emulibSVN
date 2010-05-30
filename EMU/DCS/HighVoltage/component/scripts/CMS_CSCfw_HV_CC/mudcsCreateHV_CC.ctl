#uses "CMS_CSCfw_HV_CC/emuDcs.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs2.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs3.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs4.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcsDim.ctl"
#uses "CMS_CSCfw_HV_CC/emuAlert.ctl"
#uses "CMS_CSCfw_HV_CC/emufw.ctl"


#uses "CMS_CSCfw_HV_CC/mudcs0CreateAllTypes.ctl"
#uses "CMS_CSCfw_HV_CC/mudcs1DbCreate.ctl"
#uses "CMS_CSCfw_HV_CC/mudcs2FsmCreate.ctl"
#uses "CMS_CSCfw_HV_CC/mudcs3AlertCreate.ctl"
#uses "CMS_CSCfw_HV_CC/mudcs4SetFsmPanels.ctl"
#uses "CMS_CSCfw_HV_CC/mudcs5Archive.ctl"

 #uses "CMS_CSCfw_LV_CRB/mudcs9XAlertReconfigAllSlowControls.ctl"
#uses "aes.ctl"

/** if this flag is set to true the full component installation is performed, otherwise only patch() is called. */
global const bool EMU_HVCC_INSTALLATION_FULL = false;

main(){
  mudcsInit();

  dpSetWait(CSC_fwG_g_SYSTEM_NAME+":MYWARNING.value","postinstall_start");
  
  if (!EMU_HVCC_INSTALLATION_FULL) {
    patch();
    dpSetWait(CSC_fwG_g_SYSTEM_NAME+":MYWARNING.value","postinstall_finish");
    exit(0); 
  }
  
//int is_exist=fwFsmTree_isNode(CSC_fwG_g_csc_part);
//if(is_exist)
mudcsDeleteAllTrees();

  mudcsCreateAllTypesMain();
  mudcsDbCreateMain(); 
  mudcsFsmCreateMain();
  mudcsAlertCreateMain(); 
  mudcsSetFsmPanelsMain();
// obsolete: mudcsArchiveMain(true);
//  setupArchiving(CSC_fwG_g_idisk_cross_numbers); // Xiaofeng+Evaldas Archiving

  dpSetWait(CSC_fwG_g_SYSTEM_NAME+":MYWARNING.value","postinstall_finish");
  exit(0); 
}

/**
  * This function is called when EMU_HVCC_INSTALLATION_FULL is FALSE. Put the desired patch actions here.
  */
void patch() {
  emuAlert_updateAllAlertClasses();
}
