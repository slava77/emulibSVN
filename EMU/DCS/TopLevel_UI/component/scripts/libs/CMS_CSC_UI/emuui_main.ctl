/**@file

This package contains the main top level functions of the UI.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

#uses "CMS_CSC_UI/look_and_feel/default.ctl"

void emuui_init() {
  emuui_initSession();
}

/** Parses the "device parameters string" and returns a mapping.
  String syntax is: key1=value1;key2=value2;.... */
mapping emuui_parseDeviceParameters(string strParams, dyn_string &exceptionInfo) {
  dyn_string entries;
  mapping ret;
  
  entries = strsplit(strParams, ";");
  for (int i=1; i <= dynlen(entries); i++) {
    dyn_string entry = strsplit(entries[i], "=");
    if (dynlen(entry) != 2) {
      emu_addError("Could not parse the deviceParameters string: " + strParams, exceptionInfo);
      return;
    }
    ret[entry[1]] = entry[2];
  }
  
  return ret;
}
