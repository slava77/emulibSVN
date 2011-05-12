#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_alert.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"
#uses "CMS_CSC_ExSys_Interface/facts/alertFact.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysUtil.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysInterface.ctl"
#uses "CMS_CSC_ExSys_Interface/pingPong.ctl"

main()
{
  dyn_string ex;
  exsys_startAlertFactDelivery(ex);
  if (emu_checkException(ex)) { return; }
 
  // connect to internal commands
  dpConnect("exsys_alertSenderInternalCommandUpdatedCB", false, EXSYS_INTERNAL_CMD_DP);
}

/**
  * Executes exsys internal commands
  */
void exsys_alertSenderInternalCommandUpdatedCB(string dp, string command) {
  emu_info("Exsys Alert Sender: got '" + command + "' internal command");
  
  if (command == EXSYS_INTERNAL_CMD_NO_EXSYS_MODE) {
    dpSetWait("CscAlertClass_HV_1_d.class", "_fwErrorNack_70.");
    emuAlert_updateAllAlertClassesForType("HV_1_d");
    dpSetWait("CscAlertClass_FwCaenChannel.class", "_fwErrorNack_70.");
    emuAlert_updateAllAlertClassesForType("FwCaenChannel");
  } else if (command == EXSYS_INTERNAL_CMD_EXSYS_MODE) {
    dpSetWait("CscAlertClass_HV_1_d.class", "_fwWarningNack_41.");
    emuAlert_updateAllAlertClassesForType("HV_1_d");
    dpSetWait("CscAlertClass_FwCaenChannel.class", "_fwWarningNack_41.");
    emuAlert_updateAllAlertClassesForType("FwCaenChannel");
    exit(); // restart so that a snapshot of current alerts is sent over to exsys
  }
}
