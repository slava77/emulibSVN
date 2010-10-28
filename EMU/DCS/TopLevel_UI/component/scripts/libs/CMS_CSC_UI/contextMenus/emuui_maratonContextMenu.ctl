/**@file

This package contains functions to construct context and process menus for "chamber" device type.

@author Evaldas Juska (PH/UCM)
@date   October 2009
*/

#uses "CMS_CSC_common/emu_accessControl.ctl"

/** Initializes the actionMap which is [menu_answer]->[function] mapping. */
mapping emuuicm_initMaratonContextMenus() {
  mapping actionMap;
  
  actionMap["action_100"] = makeDynAnytype("emuuicm_openChannelControlPanel");
  
  return actionMap;
}

/** Returns context menu for maraton (as popupMenu structure - search PVSS help for popupMenu() for more information).*/
dyn_string emuuicm_getMaratonContextMenu(mapping deviceParams, dyn_string &exceptionInfo) {
  dyn_string menu;

  if (!emu_hasExpertControl() || !emu_hasFsmControl("CMS_CSC::CMS_CSC")) {
    return menu;
  }
  
  dynAppend(menu, "PUSH_BUTTON, Control Individual Channels (!experts only!), 100, 1");
  
  return menu;
}

// ACTION FUNCTIONS (note all those functions must accept either only one parameter - mapping deviceParams, or two parameters - mapping deviceParams, dyn_anytype params)
void emuuicm_openChannelControlPanel(mapping deviceParams) {
  dyn_string ex;
  string mrtnDp = emuui_getDpName("maraton_upTime", deviceParams, ex);
  if (emu_checkException(ex)) { return; }
  mrtnDp = dpSubStr(mrtnDp, DPSUB_SYS_DP);
  string deviceParamsStr = emuui_deviceParamsToString(deviceParams);
  
  ChildPanelOnCentral(EMUUI_OBJECTS_DIR + "/refMrtnControl.pnl", 
                      "MaratonChannelControlPanel" + deviceParams,
                      makeDynString("$mrtnDp:" + mrtnDp, "$deviceParams:" + deviceParamsStr));
}
