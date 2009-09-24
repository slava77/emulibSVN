/**@file

This package contains the main top level functions of the UI.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

const string EMUUI_COMPONENT_NAME = "CMS_CSC_UI";
global string EMUUI_COMPONENT_VERSION;
global string EMUUI_COMPONENT_DATE;
                                    
global bool emuui_g_initialized = false;

void emuui_init() {
  if (emuui_g_initialized) {
    return;
  }
  
  emu_info("------========== EMU Top Level UI is initializing... ==========------");
  fwInstallation_getComponentInfo(EMUUI_COMPONENT_NAME, "componentVersionString", EMUUI_COMPONENT_VERSION);
  EMUUI_COMPONENT_VERSION = substr(EMUUI_COMPONENT_VERSION, 3);
  fwInstallation_getComponentInfo(EMUUI_COMPONENT_NAME, "date", EMUUI_COMPONENT_DATE);
  EMUUI_COMPONENT_DATE = substr(EMUUI_COMPONENT_DATE, 3);
  emu_info("Component version: " + EMUUI_COMPONENT_VERSION + ", date: " + EMUUI_COMPONENT_DATE);
           
  emuui_initSession();
  emu_info("Session ID: " + emuui_getSessionId());
  fwFsmUi_init("CMS_CSC", "CMS_CSC");
  fwCU_takeTree("CMS_CSC");
  
  emuui_g_initialized = true;
  emuuibc_startBackgroundChecks(); // in emuui_backgroundChecks.ctl
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

/** Converts a mapping to parameter string. */
string emuui_mappingToParameterString(mapping params) {
  string ret;
  
  for (int i=1; i <= mappinglen(params); i++) {
    string key = mappingGetKey(params, i);
    string value = params[key];
    
    if (i > 1) {
      ret += ";";
    }
    ret += key + "=" + value;
  }
  
  return ret;
}
