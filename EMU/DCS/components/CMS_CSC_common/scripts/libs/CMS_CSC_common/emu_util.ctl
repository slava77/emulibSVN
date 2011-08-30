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
string emu_getLastUpdateTimeAsTime(string dp) {
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
