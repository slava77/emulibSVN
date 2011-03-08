#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysUtil.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysInterface.ctl"
#uses "CMS_CSC_ExSys_Interface/facts/lvFact.ctl"
#uses "CMS_CSC_ExSys_Interface/facts/alertFact.ctl"
#uses "CMS_CSC_ExSys_Interface/facts/hvFact.ctl"

main()
{
  dyn_string ex;
  exsys_startLvFactDelivery();
//  exsys_startAlertFactDelivery(ex);
//  if (emu_checkException(ex)) { return; }
  exsys_startHvFactDelivery(ex);
  if (emu_checkException(ex)) { return; }
}
