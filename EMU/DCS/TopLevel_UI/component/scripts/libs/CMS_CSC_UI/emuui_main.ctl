/**@file

This package contains the main top level functions of the UI.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

#uses "CMS_CSC_UI/look_and_feel/default.ctl"

global bool emuui_g_initialized = false;

void emuui_init() {
  if (emuui_g_initialized) {
    return;
  }
  
  emu_info("------========== EMU Top Level UI is initializing... ==========------");
  emuui_initSession();
  
  emuui_g_initialized = true;
  emu_info("------========== EMU Top Level UI initialization DONE! ==========------");
}

bool emuui_isInitialized() {
  return emuui_g_initialized;
}

/** Parses the "parameters string" and returns a mapping.
  String syntax is: key1=value1;key2=value2;.... */
mapping emuui_parseParameterString(string strParams, dyn_string &exceptionInfo) {
  dyn_string entries;
  mapping ret;
  
  entries = strsplit(strParams, ";");
  for (int i=1; i <= dynlen(entries); i++) {
    dyn_string entry = strsplit(entries[i], "=");
    if (dynlen(entry) != 2) {
      emu_addError("Could not parse the parameterString: " + strParams, exceptionInfo);
      return;
    }
    ret[entry[1]] = entry[2];
  }
  
  return ret;
}
