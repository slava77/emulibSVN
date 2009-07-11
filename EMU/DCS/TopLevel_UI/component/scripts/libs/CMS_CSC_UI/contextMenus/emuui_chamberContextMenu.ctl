/**@file

This package contains functions to construct context and process menus for "chamber" device type.

@author Evaldas Juska (PH/UCM)
@date   June 2009
*/

/** Returns context menu for chamber (as popupMenu structure - search PVSS help for popupMenu() for more information).*/
dyn_string emuui_getChamberContextMenu(mapping deviceParams, dyn_string &exceptionInfo) {
  dyn_string menu;
  
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
