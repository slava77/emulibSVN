// Common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_math.ctl"

// UI libs
#uses "CMS_CSC_UI/background_checks/emuui_connectionMonitor.ctl"
#uses "CMS_CSC_UI/background_checks/emuui_pcrateConfigurationMonitor.ctl"

/**@file

This package is the entry point to background checks package e.g. it has a main function to start all the background checks

All functions have prefix emuuibc_ where bc stands for Background Checks.

@author Evaldas Juska (PH/UCM)
@date   September 2009
*/

/** Starts the background checks - basically makes the initial dpConnects. */
void emuuibc_startBackgroundChecks() {
  // get the necessary datapoints
  dyn_string ex;
  dyn_string disconnectedDevDPs = emuui_getDpNames("disconnected_devices", emuui_dummyMapping, ex);
  if (emu_checkException(ex)) { return; }
  dyn_string xmasControlDPs = emuui_getDpNames("xmas_control", emuui_dummyMapping, ex);
  if (emu_checkException(ex)) { return; }
  emu_info("Background checks service: starting monitoring for disconnected devices (devices that loose communication)");
  emu_debug("Background checks service: disconnected devices dps: " + disconnectedDevDPs, emu_DEBUG_DETAIL);
  emu_debug("Background checks service: xmas control dps: " + xmasControlDPs, emu_DEBUG_DETAIL);
  
  // connect the DPs to the work functions
  for (int i=1; i <= dynlen(xmasControlDPs); i++) {
    dpConnect("emuuibc_xmasControlUpdatedCB", true, xmasControlDPs[i]);
  }
  for (int i=1; i <= dynlen(disconnectedDevDPs); i++) {
    dpConnect("emuuibc_disconnectedDevicesUpdatedCB", true, disconnectedDevDPs[i]);
  }
  
  // connect to a list of remote PVSS projects (from distribution manager)
  dpConnect("emuuibc_checkComputersConnectionStatusCB", true, "_DistManager.State.SystemNums");
  
  // connect to the "UPS on battery" signal
  string upsOnBatteryDp = emuui_getDpName("UPS_on_battery_status", emuui_dummyMapping, ex);
  if (emu_checkException(ex)) { return; }
  dpConnect("emuuibc_upsMonitorCB", true, upsOnBatteryDp);
  emu_debug("Background checks service: connected to 'UPS on battery' signal - " + upsOnBatteryDp, emu_DEBUG_DETAIL);
}

/** This function is called when "UPS on battery" state has changed. */
void emuuibc_upsMonitorCB(string dp, bool onBattery) {
  lblUpsOnBattery.visible = onBattery;
  upsOnBatteryBack.visible = onBattery;
}
