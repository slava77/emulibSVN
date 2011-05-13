#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysUtil.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysInterface.ctl"
#uses "CMS_CSC_ExSys_Interface/pingPong.ctl"
#uses "CMS_CSC_ExSys_Interface/facts/lvFact.ctl"
#uses "CMS_CSC_ExSys_Interface/facts/alertFact.ctl"
#uses "CMS_CSC_ExSys_Interface/facts/hvFact.ctl"

main()
{
  dyn_string ex;
  startThread("exsys_startPinging");
  exsys_startLvFactDelivery();
//  exsys_startAlertFactDelivery(ex);
//  if (emu_checkException(ex)) { return; }
  exsys_startHvFactDelivery(ex);
  if (emu_checkException(ex)) { return; }
  
  // connect to internal commands
  dpConnect("exsys_factFinderInternalCommandUpdatedCB", false, EXSYS_INTERNAL_CMD_DP);  
}

/**
  * Executes exsys internal commands
  */
void exsys_factFinderInternalCommandUpdatedCB(string dp, string command) {
  emu_info("Exsys Fact Finder: got '" + command + "' internal command");
  
  if (command == EXSYS_INTERNAL_CMD_EXSYS_MODE) {
    delay(10);
    exit(); // restart so that a snapshot of current alerts is sent over to exsys
  }
}
