/**@file

LV Fact handling

@author Evaldas Juska (PH/UCM)
@date   September 2010
*/

#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysUtil.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysInterface.ctl"

private global dyn_string lvDps;

/**
  * Collects all CFEB LV data, forms the DcsCfebLvFacts and sends them to the Expert System.
  */
public void exsys_sendAllLvFacts(dyn_string &exceptionInfo) {
  
  dyn_string dps = exsys_getLvDps(exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  dyn_anytype values = exsys_massDpeGet(dps, makeDynString(".update_value",
                                                           ".status",
                                                           ".data.Cfeb_o.c50.v1",
                                                           ".data.Cfeb_o.c50.v2",
                                                           ".data.Cfeb_o.c50.v3",
                                                           ".data.Cfeb_o.c50.v4",
                                                           ".data.Cfeb_o.c50.v5",
                                                           ".data.Alct_o.c18"));
  dyn_string cfebParamNames = makeDynString("status",
                                            "current_5v");
  dyn_string alctParamNames = makeDynString("status",
                                            "current_1v8");
  

  for (int dpIdx=1; dpIdx <= dynlen(dps); dpIdx++) {
    mapping chamberDeviceParams = emu_fsmNodeToDeviceParams(dps[dpIdx], exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
    string chamberName = emuui_getChamberName(chamberDeviceParams);
    
    int firstValueIdx = (dpIdx * 8) - 7;
    time timestamp = values[firstValueIdx];
    int status = values[firstValueIdx + 1];
    // send CFEB LV facts
    for (int cfeb=0; cfeb <= 4; cfeb++) {
      if ((chamberDeviceParams["station"] == 1) && (chamberDeviceParams["ring"] == 3) && (cfeb == 4)) { // ME1/3 chambers only have 4 CFEBs
        continue;
      }
      string component = chamberName + "/CFEB" + cfeb;
      float current = values[firstValueIdx + 2 + cfeb];
      dyn_anytype paramValues = makeDynString(status, current);
      exsys_sendFact("DcsCfebLvFact", timestamp, component, EXSYS_FACT_SEVERITY_INFO,
                     "CFEB LV", cfebParamNames, paramValues, exceptionInfo);
      if (emu_checkException(exceptionInfo)) { return; }
    }
    // send ALCT LV facts
    string component = chamberName + "/ALCT";
    float current = values[firstValueIdx + 7];
    dyn_anytype paramValues = makeDynString(status, current);
    exsys_sendFact("DcsAlctLvFact", timestamp, component, EXSYS_FACT_SEVERITY_INFO,
                   "ALCT LV", alctParamNames, paramValues, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
  }
  
}

/**
  * @return all LV DPs (also caches them after first request).
  */
public dyn_string exsys_getLvDps(dyn_string &exceptionInfo) {
  if (dynlen(lvDps) == 0) {
    lvDps = dpNames("*:CSC_ME_*_LV", "LV_1_d");
//    lvDps = dpNames("*:CSC_ME_P11_C01_LV", "LV_1_d");
  }
  if (dynlen(lvDps) == 0) {
    emu_addError("ExSys::exsys_getLvDps(): No LV DPs were found", exceptionInfo);
  }
  return lvDps;
}

/**
  * Connects to one of the LV DPs .update_value DPE to the exsys_lvFactSendTriggerCB callback, which sends LV facts about all system.
  */
public void exsys_startLvFactDelivery() {
  dyn_string ex;
  dyn_string lvDps = exsys_getLvDps(ex);
  if (emu_checkException(ex)) { return; }
  dpConnect("exsys_lvFactSendTriggerCB", true, lvDps[1] + ".update_value");
}

/**
  * Callback function which triggers exsys_sendAllLvFacts function, which sends LV facts about all system.
  */
public void exsys_lvFactSendTriggerCB(string dp, int value) {
  dyn_string ex;
  exsys_sendAllLvFacts(ex);
  if (emu_checkException(ex)) { return; }
}
