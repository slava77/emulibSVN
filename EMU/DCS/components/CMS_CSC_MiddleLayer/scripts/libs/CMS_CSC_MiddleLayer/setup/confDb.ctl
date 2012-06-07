/**@file

This package contains functions for dealing with configuration DB (e.g. saving devices)

@author Evaldas Juska (FNAL)
@date   June 2012
*/

#uses "fwConfigurationDB/fwConfigurationDB.ctl"
#uses "fwConfigurationDB/fwConfigurationDB_DeviceConfiguration.ctl"
#uses "CMS_CSC_common/emu_common.ctl"

const string EMUCONFDB_PREFIX = "MiddleLayer/";

/**
  * Saves all HV devices that are on local system to conf DB.
  * @param version configuration version number (currently using 2 number format that looks like 1.0, 2.0, 11.5, etc.)
  * @param justPrint (optional, default = FALSE) if true then all this procedure does is print what it would be saving to DB if this parameter was = false.
  * @param saveOptions (optional, default = 0) if provided can be used to save specific device properties only (e.g. use fwConfigurationDB_deviceConfig_VALUE to save only the values). By default saves all device properties.
  */
void emuconfdb_saveHvDevices(string version, dyn_string &ex, bool justPrint = FALSE, int saveOptions = 0) {
  fwConfigurationDB_checkInit(ex);
  if (emu_checkException(ex)) { return; }
  
  dyn_string devices;

  // *** HV ***
  emu_dynAppend(devices, emuconfdb_getDevicesFromHardwareHierarchy(getSystemName() + "CscHighVoltage", ex));
  if (emu_checkException(ex)) { return; }
  emu_dynAppend(devices, emuconfdb_getDevicesFromHardwareHierarchy(getSystemName() + "DimConfig", ex, "*HV*"));
  if (emu_checkException(ex)) { return; }
  emu_dynAppend(devices, dpNames("*HV*", "CscDimCommand"));
  emu_dynAppend(devices, dpNames("HvServerStatus", "CscCommonString"));
  
  emuconfdb_saveConfiguration(devices, "HV", version, ex, justPrint, saveOptions);
  if (emu_checkException(ex)) { return; }

  // *** HV LV ***
  dynClear(devices);
  emu_dynAppend(devices, emuconfdb_getDevicesFromHardwareHierarchy(getSystemName() + "Wiener", ex, "*HV_LV_*"));
  if (emu_checkException(ex)) { return; }

  emuconfdb_saveConfiguration(devices, "HV_LV", version, ex, justPrint, saveOptions);
  if (emu_checkException(ex)) { return; }
}

/**
  * Saves all X2P devices that are on local system to conf DB.
  * @param version configuration version number (currently using 2 number format that looks like 1.0, 2.0, 11.5, etc.)
  * @param justPrint (optional, default = FALSE) if true then all this procedure does is print what it would be saving to DB if this parameter was = false.
  * @param saveOptions (optional, default = 0) if provided can be used to save specific device properties only (e.g. use fwConfigurationDB_deviceConfig_VALUE to save only the values). By default saves all device properties.
  */
void emuconfdb_saveX2pDevices(string version, dyn_string &ex, bool justPrint = FALSE, int saveOptions = 0) {
  fwConfigurationDB_checkInit(ex);
  if (emu_checkException(ex)) { return; }
  
  dyn_string devices;

  // *** X2P ***
  dynClear(devices);
  emu_dynAppend(devices, emuconfdb_getDevicesFromHardwareHierarchy(getSystemName() + "CscLowVoltage", ex));
  if (emu_checkException(ex)) { return; }
  emu_dynAppend(devices, emuconfdb_getDevicesFromHardwareHierarchy(getSystemName() + "CscTemperature", ex));
  if (emu_checkException(ex)) { return; }
  emu_dynAppend(devices, emuconfdb_getDevicesFromHardwareHierarchy(getSystemName() + "DimConfig", ex, "*X2P_[PM]*"));
  if (emu_checkException(ex)) { return; }
  emu_dynAppend(devices, emuconfdb_getDevicesFromHardwareHierarchy(getSystemName() + "DimConfig", ex, "*X2P_COMMAND_[PM]*"));
  if (emu_checkException(ex)) { return; }
  emu_dynAppend(devices, emuconfdb_getDevicesFromHardwareHierarchy(getSystemName() + "DimConfig", ex, "*X2P_REPLY_[PM]*"));
  if (emu_checkException(ex)) { return; }
  emu_dynAppend(devices, dpNames("*X2P*", "CscDimCommand"));
  emu_dynAppend(devices, dpNames("X2P_REPLY_*", "CscCommonString"));
  
  emuconfdb_saveConfiguration(devices, "X2P", version, ex, justPrint, saveOptions);
  if (emu_checkException(ex)) { return; }
  
  // *** X2P FED ***
  dynClear(devices);
  emu_dynAppend(devices, emuconfdb_getDevicesFromHardwareHierarchy(getSystemName() + "CscFed", ex));
  if (emu_checkException(ex)) { return; }
  emu_dynAppend(devices, emuconfdb_getDevicesFromHardwareHierarchy(getSystemName() + "DimConfig", ex, "*X2P_FED*"));
  if (emu_checkException(ex)) { return; }
  
  emuconfdb_saveConfiguration(devices, "X2P_FED", version, ex, justPrint, saveOptions);
  if (emu_checkException(ex)) { return; }
}

/**
  * Saves given devices to configuration DB (if justPrint=true, it won't actually save the devices, but just print the list instead)
  */
void emuconfdb_saveConfiguration(dyn_string devices, string name, string version, dyn_string &exceptionInfo, bool justPrint = FALSE, int saveOptions = 0) {
  fwConfigurationDB_checkInit(exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }

  string confName = EMUCONFDB_PREFIX + name + "_v" + version;
  emu_info("Saving these devices to conf DB with name " + confName + " (with saveOptions = " + saveOptions + ")");
  emu_info(devices);
  if (!justPrint) {
    fwConfigurationDB_saveDeviceConfiguration(devices, confName, fwDevice_HARDWARE, exceptionInfo, saveOptions);
    if (emu_checkException(exceptionInfo)) { return; }
  }
}

/**
  * @return devices from the specified node of the hardware hierarchy (also optionally matching a given pattern)
  */
dyn_string emuconfdb_getDevicesFromHardwareHierarchy(string rootNode, dyn_string &ex, string pattern = "") {
  dyn_string deviceList;
  fwConfigurationDB_getHierarchyFromPVSS(rootNode, fwDevice_HARDWARE, deviceList, ex);
  if (emu_checkException(ex)) { return; }

  dyn_string ret;
  if (pattern != "") {
    for (int i=1; i <= dynlen(deviceList); i++) {
      string device = deviceList[i];
      if (patternMatch(pattern, device)) {
        dynAppend(ret, device);
      }
    }
  } else {
    ret = deviceList;
  }
  
  return ret;
}
