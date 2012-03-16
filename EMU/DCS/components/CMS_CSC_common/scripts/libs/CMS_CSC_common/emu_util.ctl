/**@file

This package contains miscellaneous utility functions.

@author Evaldas Juska (PH/UCM)
@date   May 2009
*/

// Common libraries
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_math.ctl"
#uses "CMS_CSC_common/emu_mapping.ctl"


/** returns stime of a given datapoint (as type time). */
time emu_getLastUpdateTimeAsTime(string dp) {
  time stime;
  string dpSTime = dpSubStr(dp, DPSUB_SYS_DP_EL) + ":_online.._stime";
  if (!dpExists(dpSTime)) {
    return "";
  }
  dpGet(dpSTime, stime);

  return stime;
}

/** returns stime of a given datapoint in string. So this is the only place if you need to change the date format everywhere. */
string emu_getLastUpdateTime(string dp) {
  time stime = emu_getLastUpdateTimeAsTime(dp);
  string timeStr = emu_formatTime(stime);
  return (string) timeStr;
}

/* Format time as a string. */
string emu_formatTime(time t) {
  return formatTime("%Y.%m.%d %H:%M", t);
}

/** returns dp which has been updated most recently in the dp List. */
string emu_getDpWithLastUpdateTime(dyn_string dpList) {
  int retIndex = 1;
  time lastUpdate;
  for (int i=1; i <= dynlen(dpList); i++) {
    time stime;
    string dpSTime = dpSubStr(dpList[i], DPSUB_SYS_DP_EL) + ":_online.._stime";
    dpGet(dpSTime, stime);
    if ((i == 1) || (stime > lastUpdate)) {
      lastUpdate = stime;
      retIndex = i;
    }
  }

  return dpList[retIndex];
}

/** returns stime of a given datapoint in string. So this is the only place if you need to change the date format everywhere. */
string emu_getLastUpdateTimeOfDpList(dyn_string dpList) {
  time lastUpdate;
  for (int i=1; i <= dynlen(dpList); i++) {
    time stime;
    string dpSTime = dpSubStr(dpList[i], DPSUB_SYS_DP_EL) + ":_online.._stime";
    dpGet(dpSTime, stime);
    if ((i == 1) || (stime > lastUpdate)) {
      lastUpdate = stime;
    }
  }

  return emu_formatTime(lastUpdate);
}

/** Returns system name of the given DP without the colon at the end. */
string emu_getSystemName(string dp) {
  string sysName = dpSubStr(dp, DPSUB_SYS);
  strreplace(sysName, ":", "");
  return sysName;
}

/**
  * Creates a given device if it doesn't already exist.
  * @param type device type
  * @param name device name
  * @param parentDevice parent device object, format as required by JCOP fwDevice (see fwDevice_createObject)
  * @return device object that was created
  */
dyn_string emu_createDevice(string type, string name, dyn_string &ex, dyn_string parentDevice = makeDynString()) {
  dyn_string device;
  
  fwDevice_createObject(device, ex);
  if (emu_checkException(ex)) { return makeDynString(); }

  if (dynlen(parentDevice) == 0) {
    fwDevice_createObject(parentDevice, ex);
    if (emu_checkException(ex)) { return makeDynString(); }
  }
  
  device[fwDevice_DP_TYPE] = type;
  device[fwDevice_DP_NAME] = name;
  if (!emu_deviceExists(device, parentDevice)) {
    fwDevice_create(device, parentDevice, ex);
    if (emu_checkException(ex)) { return makeDynString(); }
  }
  
  return device;
}

/**
  * @param device device object, format as required by JCOP fwDevice (see fwDevice_createObject)
  * @param parent parent device object, format as required by JCOP fwDevice (see fwDevice_createObject)
  * @return true if given device already exists in the local system.
  */
bool emu_deviceExists(dyn_string device, dyn_string parent) {
  string dp = "";
  if (parent[fwDevice_DP_NAME] != "") {
    dp += parent[fwDevice_DP_NAME] + fwDevice_HIERARCHY_SEPARATOR;
  }
  dp += device[fwDevice_DP_NAME];
  return dpExists(dp);
}
