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

main(){
  mudcsInit();
  dpSetWait(CSC_fwG_g_SYSTEM_NAME+":MYWARNING.value","postinstall_start");  
  mudcsCreateAllTypesMain();
  mudcsDbCreateMain(); 
  mudcsFsmCreateMain();
  mudcsAlertCreateMain(); 
  mudcsSetFsmPanelsMain();
/////  mudcsArchiveMain();
  dpSetWait(CSC_fwG_g_SYSTEM_NAME+":MYWARNING.value","postinstall_finish");
  exit(0); 
}
