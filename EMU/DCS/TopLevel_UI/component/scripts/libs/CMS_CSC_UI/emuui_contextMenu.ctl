// Common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_math.ctl"

// UI libs
#uses "CMS_CSC_UI/emuui_main.ctl"
#uses "CMS_CSC_UI/emuui_configuration.ctl"
#uses "CMS_CSC_UI/emuui_sessionManager.ctl"
#uses "CMS_CSC_UI/emuui_utils.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"

/**@file

This package contains functions to construct context menus and issue commands using them.

@author Evaldas Juska (PH/UCM)
@date   June 2009
*/

#uses "CMS_CSC_UI/contextMenus/emuui_chamberContextMenu.ctl"
#uses "CMS_CSC_UI/contextMenus/emuui_maratonContextMenu.ctl"

/** map[deviceType] -> map[contextMenuItemId] -> function to call. */
global mapping emuuiContextMenuActionMap;

void emuui_initContextMenus() {
  emuuiContextMenuActionMap["chamber"] = emuuicm_initChamberContextMenus();
  emuuiContextMenuActionMap["maraton"] = emuuicm_initMaratonContextMenus();
}

/** Opens and processes a context menu.
  @param deviceType     device type for which the menu is displayed.
  @param deviceParams   device parameters.
*/
void emuui_openContextMenu(string deviceType, mapping deviceParams, dyn_string &exceptionInfo) {
  int answer;
  dyn_string menu;
  
  switch (deviceType) {
     case "chamber":
       menu = emuuicm_getChamberContextMenu(deviceParams, exceptionInfo);
       break;
    case "maraton":
      menu = emuuicm_getMaratonContextMenu(deviceParams, exceptionInfo);
      break;
    default:
//      emu_addError("No context menu defined for this type of device, please inform DCS developers about this message.", exceptionInfo);
      return;
  }
  
  if (emu_checkException(exceptionInfo)) { return; }
  popupMenu(menu, answer);
  if (answer == 0) { // no item selected
    return;
  }
  
  emuui_executeContextMenuAction(deviceType, answer, deviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
}

emuui_executeContextMenuAction(string deviceType, int answer, mapping deviceParams, dyn_string &exceptionInfo) {
  if (!mappingHasKey(emuuiContextMenuActionMap, deviceType)) {
    emu_addError("Device type '" + deviceType + "' was not found in context menu action matrix!", exceptionInfo);
    return;
  }
  if (!mappingHasKey(emuuiContextMenuActionMap[deviceType], "action_" + answer)) {
    emu_addError("No function defined for '" + deviceType + "' context menu item with ID=" + answer, exceptionInfo);
    return;
  }
  
  string function = emuuiContextMenuActionMap[deviceType]["action_" + answer][1];
  dyn_anytype params;
  emu_dynAppend(params, emuuiContextMenuActionMap[deviceType]["action_" + answer]);
  dynRemove(params, 1);

  emu_info("CSC UI: Context menu - executing action " + answer + " for device '" + deviceParams + "' (device type is " + deviceType + "), this action maps to function " + function + ". Params: " + params);

  if (dynlen(params) > 0) {
    startThread(function, deviceParams, params);
  } else {
    startThread(function, deviceParams);
  }
}
