main() {
  DebugTN("UPS monitoring started");
  dpConnect("emulv_upsStatusChanged", true, "CMS_CSC_UPS_LV_On_Battery.");
}

void emulv_upsStatusChanged(string dp, bool onBattery) {
  DebugTN("UPS status changed. OnBattery=" + onBattery + " (dp=" + dp + ")");
  if (onBattery) {
    DebugTN("Got UPS on battery signal - sending OFF command to CMS_CSC");
    fwCU_sendCommand("CMS_CSC", "OFF");
    delay(10);
    DebugTN("Got UPS on battery signal - sending OFF command to CMS_CSC_LV_CAN1");
    fwCU_sendCommand("CMS_CSC_LV_CAN1", "OFF");
    delay(10);
    DebugTN("Got UPS on battery signal - sending OFF command to CMS_CSC_LV_CAN2");
    fwCU_sendCommand("CMS_CSC_LV_CAN2", "OFF");
  }
}
