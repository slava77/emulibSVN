#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"
#uses "CMS_CSC_ExSys_Interface/facts/alertFact.ctl"

main()
{
  dyn_string ex;
  exsys_startAlertFactDelivery(ex);
  if (emu_checkException(ex)) { return; }
}
