/**@file

This package contains functions to construct context menus and issue commands using them.

@author Evaldas Juska (PH/UCM)
@date   June 2009
*/

#uses "CMS_CSC_UI/contextMenus/emuui_chamberContextMenu.ctl"

/** Opens and processes a context menu.
  @param deviceType     device type for which the menu is displayed.
  @param deviceParams   device parameters.
*/
void emuui_openContextMenu(string deviceType, mapping deviceParams, dyn_string &exceptionInfo) {
  int answer;
  dyn_string menu;
  
  switch (deviceType) {
    case "chamber":
      menu = emuui_getChamberContextMenu(deviceParams, exceptionInfo);
      break;
    default:
      emu_addError("No context menu defined for this type of device, please inform DCS developers about this message.", exceptionInfo);
      return;
  }
  
  if (emu_checkException(exceptionInfo)) { return; }
  
  popupMenu(menu, answer);
  
  
}
