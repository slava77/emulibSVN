/**@file

This package contains functions to construct context and process menus for "chamber" device type.

@author Evaldas Juska (PH/UCM)
@date   June 2009
*/

#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_alert.ctl"
#uses "CMS_CSC_MiddleLayer/emu_hv.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"
#uses "CMS_CSC_common/emu_accessControl.ctl"

/** Initializes the actionMap which is [menu_answer]->[function] mapping. */
mapping emuuicm_initChamberContextMenus() {
  mapping actionMap;

  // recover HV
  actionMap["action_100"] = makeDynString("emuuicm_recoverAllTrippedHvChannels");
  for (int i=1; i <= 30; i++) {
    int actionIdx = 100 + i;
    actionMap["action_" + actionIdx] = makeDynString("emuuicm_recoverHvChannel", i);
  }

  // disable HV
  actionMap["action_200"] = makeDynString("emuuicm_disableAllTrippedHvChannels");
  for (int i=1; i <= 30; i++) {
    int actionIdx = 200 + i;
    actionMap["action_" + actionIdx] = makeDynString("emuuicm_disableHvChannel", i);
  }
  
  // enable HV
  actionMap["action_300"] = makeDynString("emuuicm_enableAllHvChannels");
  for (int i=1; i <= 30; i++) {
    int actionIdx = 300 + i;
    actionMap["action_" + actionIdx] = makeDynString("emuuicm_enableHvChannel", i);
  }

  // open HV panel
  actionMap["action_400"] = makeDynString("emuuicm_openHvPanel");
  
  return actionMap;
}

/** Returns context menu for chamber (as popupMenu structure - search PVSS help for popupMenu() for more information).*/
dyn_string emuuicm_getChamberContextMenu(mapping deviceParams, dyn_string &exceptionInfo) {
  dyn_string menu;

//~~~~~~~~!!!!!!!!!! checking CMS_CSC::CMS_CSC is wrong - think about cDCS shifter :) aaaaa if (!emu_hasControl() || !emu_hasFsmControl("CMS_CSC::CMS_CSC")) {
//    dynAppend(menu, "PUSH_BUTTON, You don't have control privileges.., 999, 0");
//    return menu;
//  }

  
  // ------====== HIGH VOLTAGE ======------
  dyn_int trippedHvChannels, interlockedHvChannels, deadHvChannels, disabledHvChannels;
  emuui_chamberHvGetProblematicChannels(deviceParams, 
                                        trippedHvChannels, 
                                        interlockedHvChannels, 
                                        deadHvChannels, 
                                        disabledHvChannels, 
                                        exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  int enableRecoverMenu = dynlen(trippedHvChannels) + dynlen(interlockedHvChannels),
      enableDisableMenu = dynlen(trippedHvChannels) + dynlen(interlockedHvChannels) + dynlen(deadHvChannels),
      enableEnableMenu = dynlen(disabledHvChannels);
  
  dynAppend(menu, "CASCADE_BUTTON, Recover High Voltage, " + enableRecoverMenu);
  dynAppend(menu, "CASCADE_BUTTON, Disable High Voltage Channels, " + enableDisableMenu);
  dynAppend(menu, "CASCADE_BUTTON, Enable High Voltage Channels, " + enableEnableMenu);
  dynAppend(menu, "PUSH_BUTTON, Open High Voltage Panel, 400, 1");
  
  
  //recover submenu
  dynAppend(menu, "Recover High Voltage");
  dynAppend(menu, "PUSH_BUTTON, Recover All Tripped Channels, 100, 1");
  dynAppend(menu, "CASCADE_BUTTON, Recover One Tripped Channel, 1");
  dynAppend(menu, "Recover One Tripped Channel");
  //generate a submenu for recovering individual channels (add only tripped and interlocked channels)
  dyn_int canRecover;
  emu_dynAppend(canRecover, trippedHvChannels);
  emu_dynAppend(canRecover, interlockedHvChannels);
  // add the channels from canRecover array to the menu
  for (int i=1; i <= dynlen(canRecover); i++) {
    // if the current channel has already been put into the menu - skip it
    if (dynContains(canRecover, canRecover[i]) != i) {
      continue;
    }
    int id = 100 + canRecover[i];
    dynAppend(menu, "PUSH_BUTTON, Recover #" + canRecover[i] + ", " + id + ", 1");
  }

  
  
  //disable submenu
  dynAppend(menu, "Disable High Voltage Channels");
  dynAppend(menu, "PUSH_BUTTON, Disable All Tripped Channels, 200, 1");
  dynAppend(menu, "CASCADE_BUTTON, Disable One Channel, 1");
  dynAppend(menu, "Disable One Channel");
  //generate a submenu for disabling individual channels (add only tripped, interlocked or dead channels which are not already disabled)
  dyn_int canDisable;
  emu_dynAppend(canDisable, trippedHvChannels);
  emu_dynAppend(canDisable, interlockedHvChannels);
  emu_dynAppend(canDisable, deadHvChannels);
  // add the channels from canDisable array to the menu but still check if they're not already disabled
  for (int i=1; i <= dynlen(canDisable); i++) {
    // if the current channel has already been disabled or put into the menu - skip it
    if (dynContains(disabledHvChannels, canDisable[i]) || (dynContains(canDisable, canDisable[i]) != i)) {
      continue;
    }
    int id = 200 + canDisable[i];
    dynAppend(menu, "PUSH_BUTTON, Disable #" + canDisable[i] + ", " + id + ", 1");
  }
    
  
  
  //enable submenu
  dynAppend(menu, "Enable High Voltage Channels");
  dynAppend(menu, "PUSH_BUTTON, Enable All Disabled Channels, 300, 1");
  dynAppend(menu, "CASCADE_BUTTON, Enable One Channel, 1");
  dynAppend(menu, "Enable One Channel");
  //generate items for enable submenu (add all disabled channels)
  for (int i=1; i <= dynlen(disabledHvChannels); i++) {
    int id = 300 + disabledHvChannels[i];
    dynAppend(menu, "PUSH_BUTTON, Enable #" + disabledHvChannels[i] + ", " + id + ", 1");
  }

  return menu;
}

// ================ ACTION FUNCTIONS (note all those functions must accept either only one parameter - mapping deviceParams, or two parameters - mapping deviceParams, dyn_anytype params) ================


// recover HV
void emuuicm_recoverAllTrippedHvChannels(mapping deviceParams) {
  dyn_string ex;
  dyn_int trippedHvChannels, interlockedHvChannels, deadHvChannels, disabledHvChannels;
  emuui_chamberHvGetProblematicChannels(deviceParams, 
                                        trippedHvChannels, 
                                        interlockedHvChannels, 
                                        deadHvChannels, 
                                        disabledHvChannels, 
                                        ex);
  if (emu_checkException(ex)) { return; }
  dyn_int canRecover;
  emu_dynAppend(canRecover, trippedHvChannels);
  emu_dynAppend(canRecover, interlockedHvChannels);  
  
  emuuicm_recoverHvChannel(deviceParams, canRecover);
}

void emuuicm_recoverHvChannel(mapping deviceParams, dyn_int channelNumbers) {
  for (int i=1; i <= dynlen(channelNumbers); i++) {
    int chNum = channelNumbers[i];
    
    dyn_string ex;
    mapping channelDeviceParams = deviceParams;
    channelDeviceParams["channelNumber"] = chNum;
  
    string channelDp = emuhv_getHvChannelDp(channelDeviceParams, ex);
    if (emu_checkException(ex)) { return; }
  
    int state, status;
    dpGet(channelDp + ".state", state,
          channelDp + ".status", status);
  
    // switch it off if it's tripped
    if ((state == 0) && (status >= 4)) {
      emuhv_sendChannelCommand(channelDeviceParams, EMUHV_COMMAND_OFF, ex);
      if (emu_checkException(ex)) { return; }
    }
  
    // turn it back on
    emuhv_sendChannelCommand(channelDeviceParams, EMUHV_COMMAND_ON, ex);
  
    // refresh the data now  
    delay(1, 0);
    emuhv_requestData(deviceParams, ex);
    if (emu_checkException(ex)) { return; }
  }
}

// disable HV
void emuuicm_disableAllTrippedHvChannels(mapping deviceParams) {
  dyn_string ex;
  dyn_int trippedHvChannels, interlockedHvChannels, deadHvChannels, disabledHvChannels;
  emuui_chamberHvGetProblematicChannels(deviceParams, 
                                        trippedHvChannels, 
                                        interlockedHvChannels, 
                                        deadHvChannels, 
                                        disabledHvChannels, 
                                        ex);
  if (emu_checkException(ex)) { return; }
  dyn_int canDisable;
  emu_dynAppend(canDisable, trippedHvChannels);
  emu_dynAppend(canDisable, interlockedHvChannels);
  emu_dynAppend(canDisable, deadHvChannels); 

  emuuicm_disableHvChannel(deviceParams, canDisable);
}

void emuuicm_disableHvChannel(mapping deviceParams, dyn_int channelNumbers) {
  for (int i=1; i <= dynlen(channelNumbers); i++) {
    int chNum = channelNumbers[i];
    
    dyn_string ex;
    mapping channelDeviceParams = deviceParams;
    channelDeviceParams["channelNumber"] = chNum;
  
    emuhv_enableDisableChannel(channelDeviceParams, false, ex);
    if (emu_checkException(ex)) { return; }

    // refresh the data now  
    delay(1, 0);
    emuhv_requestData(deviceParams, ex);
    if (emu_checkException(ex)) { return; }
  }
}

//enable HV
void emuuicm_enableAllHvChannels(mapping deviceParams) {
  dyn_string ex;
  dyn_int trippedHvChannels, interlockedHvChannels, deadHvChannels, disabledHvChannels;
  emuui_chamberHvGetProblematicChannels(deviceParams, 
                                        trippedHvChannels, 
                                        interlockedHvChannels, 
                                        deadHvChannels, 
                                        disabledHvChannels, 
                                        ex);
  if (emu_checkException(ex)) { return; }
  
  emuuicm_enableHvChannel(deviceParams, disabledHvChannels);
}

void emuuicm_enableHvChannel(mapping deviceParams, dyn_int channelNumbers) {
  for (int i=1; i <= dynlen(channelNumbers); i++) {
    int chNum = channelNumbers[i];
    
    dyn_string ex;
    mapping channelDeviceParams = deviceParams;
    channelDeviceParams["channelNumber"] = chNum;
  
    emuhv_enableDisableChannel(channelDeviceParams, true, ex);
    if (emu_checkException(ex)) { return; }

    // refresh the data now  
    delay(1, 0);
    emuhv_requestData(deviceParams, ex);
    if (emu_checkException(ex)) { return; }
  }
}

// open HV FSM panel
void emuuicm_openHvPanel(mapping deviceParams) {
  dyn_string ex;
  
  // get the domain and object
  string fsmNode = emuui_getFsmNode("chamber_high_voltage", deviceParams, ex);
  if (emu_checkException(ex)) { return; }
  string domain = fsmNode;
  string object = _fwCU_getNodeObj(domain);

  // open the panel
  fwFsmUi_ObjButtonDoubleClick(domain, object, domain);   
}
