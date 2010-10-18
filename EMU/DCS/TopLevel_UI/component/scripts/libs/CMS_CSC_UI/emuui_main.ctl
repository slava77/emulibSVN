// Common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_math.ctl"

// UI libs
#uses "CMS_CSC_UI/emuui_configuration.ctl"
#uses "CMS_CSC_UI/emuui_sessionManager.ctl"
#uses "CMS_CSC_UI/background_checks/emuui_backgroundChecksMain.ctl"
#uses "CMS_CSC_UI/emuui_contextMenu.ctl"
#uses "CMS_CSC_UI/emuui_utils.ctl"

/**@file

This package contains the main top level functions of the UI.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

const string EMUUI_COMPONENT_NAME = "CMS_CSC_UI";
const string EMUUI_OBJECTS_DIR = "CMS_CSC_UI/objects";
private global string EMUUI_COMPONENT_VERSION;
private global string EMUUI_COMPONENT_DATE;
private global string EMUUI_SYSTEM; // system where CMS_CSC_UI component is installed

global bool emuui_g_initialized = false;

void emuui_init() {
  if (emuui_g_initialized) {
    return;
  }

  emu_info("------========== EMU Top Level UI is initializing... ==========------");
  
  dyn_string sysNames;
  fwInstallation_getApplicationSystem("CMS_CSC_UI", sysNames);
  if (dynlen(sysNames) == 0) {
    emu_errorSingle("UI FATAL: Cannot find system where CMS_CSC_UI component is installed! UI behavior is undefined in this condition.");
    return;
  }
  if (dynlen(sysNames) > 0) {
    if (dynlen(sysNames) > 1) {
      emu_info("WARNING: more than one system foun with CMS_CSC_UI installed, choosing the first one: " + sysNames[1] + " (found: " + sysNames + ")");
    }
    EMUUI_SYSTEM = sysNames[1];
  }
  emu_info("System: " + emuui_getSystem());
      
  fwInstallation_getComponentInfo(EMUUI_COMPONENT_NAME, "componentVersionString", EMUUI_COMPONENT_VERSION);
  EMUUI_COMPONENT_VERSION = substr(EMUUI_COMPONENT_VERSION, 3);
  fwInstallation_getComponentInfo(EMUUI_COMPONENT_NAME, "date", EMUUI_COMPONENT_DATE);
  EMUUI_COMPONENT_DATE = substr(EMUUI_COMPONENT_DATE, 3);
  emu_info("Component version: " + EMUUI_COMPONENT_VERSION + ", date: " + EMUUI_COMPONENT_DATE);
           
  emuui_initSession();
  emu_info("Session ID: " + emuui_getSessionId());
  fwFsmUi_init("CMS_CSC", "CMS_CSC");
  //fwCU_takeTree("CMS_CSC");
  
  emuui_g_initialized = true;
  emuuibc_startBackgroundChecks(); // in emuui_backgroundChecks.ctl
  emuui_initContextMenus();
  emu_info("Context menus initialized");
  emu_info("------========== EMU Top Level UI initialization DONE! ==========------");
  
}

string emuui_getSystem() {
  return EMUUI_SYSTEM;
}

string emuui_getComponentVersion() {
  return EMUUI_COMPONENT_VERSION;
}

string emuui_getComponentDate() {
  return EMUUI_COMPONENT_DATE;
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

string emuui_deviceParamsToString(mapping deviceParams) {
  string ret;
  for (int i=1; i <= mappinglen(deviceParams); i++) {
    if (i > 1) { ret += ";"; }
    string key = mappingGetKey(deviceParams, i);
    ret += key + "=" + deviceParams[key];
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

void fwFsmUser_nodeDoubleClicked(string domain, string obj, string parent = "") {
  if ((domain == "CMS_CSC") && (obj == "CMS_CSC")) {
    emuui_showInfoMessage("'Old' UI is no longer supported." +
                          "\nIf you want e.g. to disable stations - double click on the station label and the station panel will open which will allow you to disable it.",
                          "Info - old UI is no longer available");
    return;
  }
  
  // FROM ORIGINAL fwFSM  
  int openSingle;
  int cursor;
  dpGet("ToDo.openOnSingleClick", openSingle);
  if(!openSingle)
  {
    cursor = this.cursor;
    this.cursor = CURSOR_WAIT;
    fwUi_showFsmObject(domain, obj, parent); 
    this.cursor = cursor;
  }
}
