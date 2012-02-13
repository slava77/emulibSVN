/**@file

This package contains general purpose utility functions.

@author Evaldas Juska (PH/UCM)
@date   March 2009
*/

const int emu_ERROR_REPORT_DELAY = 300; //ms

const int emu_DEBUG_GENERAL = 1;
const int emu_DEBUG_FUNC_START_STOP = 2;
const int emu_DEBUG_DETAIL = 4;

/** Useful sometimes, when you want to return an empty mapping. */
const mapping EMU_DUMMY_MAPPING;

/** Debug level (bitmask)
  0 - nothing
  1 - general debug messages
  2 - function start/stop messages
  4 - detailed debug messgaes
  
  combos:
  3 - general + function start/stop messages
  5 - general + detailed messages
  7 - general + function start/stop + detailed messages
*/
global int g_emu_Debug = 0;

global dyn_string g_emu_debugBacktrace;
global dyn_int g_emu_reportingThreads;
global dyn_dyn_string g_emu_exceptionsToReport;

/** Cache of DPE values used by emu_dpGetCached() */
global mapping g_emu_dpCache;

/** Register an EMU error
  @param msg         Error message. 
*/
void emu_errorSingle(string msg) {
  emu_error(makeDynString(msg));
}

/** Throw NotImplemented exception.
    @param functionName name of the function that this exception is refering to.
    @param exceptionInfo the exception will be added here
*/
void emu_notImplementedError(string functionName) {
  emu_errorSingle("Function \"" + functionName + "\" is not implemented yet");
}

/** Register an EMU error. 
    Appends EMU initials, skips identical consecutive errors (happens often when the same exception travels through different procedures), 
      appends a backtrace (if available) and reports to the user. Now reports only the most informative message.
  @param error         JCOP framework error object.
*/
void emu_error(dyn_string error, bool pvssReport = false, bool terminateManager = false) {
  // error is already on the queue to be reported - kill the thread and post the new one
  int index = emu_dynDynContains(g_emu_exceptionsToReport, error);
  if (index > 0) {
    emu_debug("EMU - skipping the same error report", emu_DEBUG_DETAIL);
    stopThread(g_emu_reportingThreads[index]);
    dynRemove(g_emu_reportingThreads, index);
    dynRemove(g_emu_exceptionsToReport, index);
  } 
  //if this isn't the same error that's already waiting then maybe it's just a more detailed one - then do the same because we want this one to be displayed
  if (index <= 0) {  
    index = _emu_findSimilarInMap(g_emu_exceptionsToReport, error);
    if (index > 0) {
      emu_debug("EMU - skipping a less detailed error report", emu_DEBUG_DETAIL);
      stopThread(g_emu_reportingThreads[index]);
      dynRemove(g_emu_reportingThreads, index);
      dynRemove(g_emu_exceptionsToReport, index);
    }
  }
  
  dynAppend(g_emu_exceptionsToReport, error);
  int id = startThread("_emu_delayedReport", error, emu_ERROR_REPORT_DELAY, pvssReport, terminateManager);
  dynAppend(g_emu_reportingThreads, id);
}

/** Reports the error. */
void _emu_reportError(dyn_string error, bool pvssReport = false, bool terminateManager = false) {
  dyn_string exInfoForGraphics;
  DebugTN("EMU ERROR:");
  for (int i=1; i <= dynlen(error); i++) {
    dynAppend(exInfoForGraphics, "EMU DCS Exception");
    dynAppend(exInfoForGraphics, error[i]);
    dynAppend(exInfoForGraphics, "");
    DebugTN("error [" + i + "]: " + error[i]);
    if (pvssReport) {
      throwError(makeError("", PRIO_SEVERE, ERR_IMPL, -1, "error [" + i + "]: " + error[i])); //this one works very slow
    }
  }
  
  DebugTN("EMU EXCEPTION BACKTRACE:");
  dynAppend(exInfoForGraphics, "EMU EXCEPTION BACKTRACE:");
  string backtrace;
  for (int i=1; i <= dynlen(g_emu_debugBacktrace); i++) {
    backtrace += "trace [" + i + "]: " + g_emu_debugBacktrace[i] + '\n';
    DebugTN("trace [" + i + "]: " + g_emu_debugBacktrace[i]);
  }
  //TODO figure out somehow if it's running on a CTRL manager or a UI manager (if it's not on UI - do not show the dialog box).
  dynAppend(exInfoForGraphics, backtrace);
  dynAppend(exInfoForGraphics, "");
  if (isFunctionDefined("fwExceptionHandling_display")) {
    fwExceptionHandling_display(exInfoForGraphics);
  }
  if (terminateManager) {
    throwError(makeError("", PRIO_FATAL, ERR_IMPL, 0, error));
  }
  //clear backtrace
  dynClear(g_emu_debugBacktrace);
}

/** This is used in a thread to report an exception after certain delay 
  (if another exception which is just an extended version of this arrives then this thread is killed and a new one is started).
  @param error         error to be reported
  @param delayMs       delay in milli seconds
*/
void _emu_delayedReport(dyn_string error, int delayMs, bool pvssReport = false, bool terminateManager = false) {
  delay(0, delayMs);
  _emu_reportError(error, pvssReport, terminateManager);
  int index = dynContains(g_emu_reportingThreads, getThreadId());
  dynRemove(g_emu_reportingThreads, index);
  dynRemove(g_emu_exceptionsToReport, index);
}

/** Searches for a same or a subset of the given array in the given map.
    If it's found then index in map is returned, otherwise -1 is returned.
*/
int _emu_findSimilarInMap(dyn_dyn_anytype map, dyn_anytype array) {
  for (int i=1; i <= dynlen(map); i++) {
    if (dynlen(map[i]) > dynlen(array)) { //it's not a subset (it has more elements)
      continue;
    }
    string arrayS = array;
    string mapS = map[i];
    
    if (strpos(arrayS, mapS) >= 0) {
      return i;
    }    
  }
  return -1;
}

/** Adds an error to exceptionInfo and reports it through emu_error(...). */
void emu_addError(string errMsg, dyn_string &exceptionInfo) {
  dynInsertAt(exceptionInfo, errMsg, 1);
  emu_error(exceptionInfo);
}

/** Used when an exception has been handled - cancels the error report.
  @param exceptionInfo - exception which was handled and so to be canceled to report
  @param funcName - name of the function which handled the exception (needed to fix the backtrace).
                    This is not mandatory - if the name is not provided then all the backtrace is cleared out.
                    If the function doesnt use emu_debugFuncStart(...) or emu_debugFuncEnd(...) then a parent function can be used.
*/
void emu_errorHandled(dyn_string &exceptionInfo, string funcName = "") {
  // if the exception is there then cancel the report
  int index = emu_dynDynContains(g_emu_exceptionsToReport, exceptionInfo);
  if (index > 0) {
    emu_debug("EMU - exception has been handled - canceling the report", emu_DEBUG_DETAIL);
    stopThread(g_emu_reportingThreads[index]);
    dynRemove(g_emu_reportingThreads, index);
    dynRemove(g_emu_exceptionsToReport, index);
    dynClear(exceptionInfo);
  } else { // if it's not there then just clear the exception and return - we probably missed it and it's been reported already
    dynClear(exceptionInfo);
    return;
  }
  
  // if we canceled the report - go ahead and fix the debug backtrace
  if (funcName == "") {
    dynClear(g_emu_debugBacktrace);
  } else {
    int index = dynContains(g_emu_debugBacktrace, funcName);
    if (index > 0) {
      dyn_string tmpBacktrace;
      for (int i=1; i <= index; i++) {
        tmpBacktrace[i] = g_emu_debugBacktrace[i];
      }
      g_emu_debugBacktrace = tmpBacktrace;
    }
  }
}

/** Register an info message.
  @param msg           Info message.
*/
void emu_info(anytype msg) {
  _emu_printWithPrefix(msg, "EMU INFO: ");
}

/** Prints any type of value (including arrays and 2D arrays). Adds a prefix if provided. */
dyn_string _emu_printWithPrefix(anytype value, string prefix = "") {
  dyn_string ret;
  if (emu_isTypeDyn(getType(value))) {  // it's a 1D array
    for (int i=1; i <= dynlen(value); i++) {
      DebugTN(prefix + "    [" + i + "]: " + value[i]);
    }
  } else if (emu_isTypeDynDyn(getType(value))) { // it's a 2D array
    for (int i=1; i <= dynlen(value); i++) {
      string line;
      int length = dynlen(value[i]);
      for (int j=1; j <= length; j++) {
        line += "[" + i + "][" + j + "]: " + value[i][j];
        if (j != length) {
          line += ", ";
        }
      }
      DebugTN(prefix + "    " + line);
    }
  } else { // it's a simple value
    DebugTN(prefix + value);
  }
}

/** Register a debug message.
  @param msg           Debug message.
  @param level         Level of the debug message. See g_emu_Debug.
*/
void emu_debug(anytype msg, int level = 1 /*emu_DEBUG_GENERAL*/) {
  if ((g_emu_Debug & level) > 0) {
    _emu_printWithPrefix(msg, "EMU DEBUG: ");
  }
}

/** Checks the exceptionInfo parameter and if it contains any exception - it's registered and reported and true is returned.
  @param exceptionInfo   standard framework "exception object".
  @returns               true if there was any error, false if not.
*/
bool emu_checkException(dyn_string &exceptionInfo, string higherLevelMsg = "") {
  if (dynlen(exceptionInfo)) {
    if (strlen(higherLevelMsg) > 0) { // add and report
      emu_addError(higherLevelMsg, exceptionInfo);
    } else {  // just report
      emu_error(exceptionInfo);
    }
    return true;
  }
  return false;
}

/** Providing that the appropriate DEBUG level is set this function 
    logs that a certain function has been started (and updates the t0 with the current time).
  @param funcName      Function name.
  @param t0            Time when the function has been started (it's updated with the current time).
  
  @note                DO NOT USE WHEN MULTI-THREADING!
*/
void emu_debugFuncStart(string funcName, time &t0) {
  t0=getCurrentTime();
  emu_debug("-- "+funcName+" started --", emu_DEBUG_FUNC_START_STOP);
  dynAppend(g_emu_debugBacktrace, funcName);
}


/** Providing that the appropriate DEBUG level is set this function 
    logs that a certain function has finished and prints out how much time did it take to execute).
  @param funcName      Function name.
  @param t0            Time when the function has been started.
  
  @note                DO NOT USE WHEN MULTI-THREADING!
*/
void emu_debugFuncEnd(string funcName, time t0) {
  time t1=getCurrentTime();
  time dt=t1-t0;
  emu_debug("## "+funcName+" ended ## finished in "+minute(dt)+":"+second(dt)+"."+milliSecond(dt),
            emu_DEBUG_FUNC_START_STOP);
  if (g_emu_debugBacktrace[dynlen(g_emu_debugBacktrace)] != funcName) {
    emu_errorSingle("Error in the backtrace - the ended function is not at the end of the stack. " +
                    "Please check the emu_debugFuncStart() and emu_debugFuncEnd() calls, " +
                    "also refer to the printed backtrace.");
  } else {
    dynRemove(g_emu_debugBacktrace, dynlen(g_emu_debugBacktrace));
  }
}

/** Gets requested parameter from the database and returns it. */
mixed emu_getParameter(string paramName, dyn_string &exceptionInfo, int datatype = STRING_VAR) {
  string sql = "select value from emucdb_parameters where name = :paramName";
  time t0;
  emu_debugFuncStart("emu_getParameter", t0);
  
  mapping bindVariables;
  bindVariables["paramName"] = paramName;
  mixed ret = emucdb_querySingleField(sql, exceptionInfo, bindVariables, datatype);
  if (emu_checkException(exceptionInfo)) { return; }
  
  emu_debugFuncEnd("emu_getParameter", t0);
  return ret;
}

/** If a given map contains a given array - returns the index at which it was found, if not -1 is returned. */
int emu_dynDynContains(dyn_dyn_anytype map, dyn_anytype array) {
  for (int i=1; i <= dynlen(map); i++) {
    if ((bool) (map[i] == array)) {
      return i;
    }
  }
  return -1;
}

/** If the given datapoint element type (acquired by dpElementType(...)) is an array - returns true, otherwise - false. */
bool emu_isDpTypeDyn(int type) {
  return type == DPEL_DYN_STRING ||
         type == DPEL_DYN_INT ||
         type == DPEL_DYN_FLOAT ||
         type == DPEL_DYN_TIME ||
         type == DPEL_DYN_CHAR ||
         type == DPEL_DYN_BOOL ||
         type == DPEL_DYN_BIT32 ||
         type == DPEL_DYN_BIT32_STRUCT ||
         type == DPEL_DYN_BLOB ||
         type == DPEL_DYN_BLOB_STRUCT ||
         type == DPEL_DYN_BOOL_STRUCT ||
         type == DPEL_DYN_CHAR_STRUCT ||
         type == DPEL_DYN_DPID ||
         type == DPEL_DYN_DPID_STRUCT ||
         type == DPEL_DYN_FLOAT_STRUCT ||
         type == DPEL_DYN_INT_STRUCT ||
         type == DPEL_DYN_LANGSTRING ||
         type == DPEL_DYN_LANGSTRING_STRUCT ||
         type == DPEL_DYN_STRING_STRUCT ||
         type == DPEL_DYN_TIME_STRUCT ||
         type == DPEL_DYN_UINT ||
         type == DPEL_DYN_UINT_STRUCT;
}

/** If the given type (acquired by getType(...)) is an array - returns true, otherwise - false. */
bool emu_isTypeDyn(int type) {
  return type == DYN_STRING_VAR ||
         type == DYN_INT_VAR ||
		 type == DYN_UINT_VAR ||
         type == DYN_FLOAT_VAR ||
         type == DYN_TIME_VAR ||
         type == DYN_CHAR_VAR ||
         type == DYN_BOOL_VAR ||
         type == DYN_BIT32_VAR ||
         type == DYN_ERRCLASS_VAR ||
         type == DYN_ANYTYPE_VAR ||
         type == DYN_LANGSTRING_VAR ||
         type == DYN_DPIDENTIFIER_VAR ||
		 type == DYN_BLOB_VAR ||
		 type == DYN_MIXED_VAR;
}

/** If the given type (acquired by getType(...)) is of type dyn_dyn_* - returns true, otherwise - false. */
bool emu_isTypeDynDyn(int type) {
  return type == DYN_DYN_STRING_VAR ||
         type == DYN_DYN_INT_VAR ||
		 type == DYN_DYN_UINT_VAR ||
         type == DYN_DYN_FLOAT_VAR ||
         type == DYN_DYN_TIME_VAR ||
         type == DYN_DYN_CHAR_VAR ||
         type == DYN_DYN_BOOL_VAR ||
         type == DYN_DYN_BIT32_VAR ||
         type == DYN_DYN_ERRCLASS_VAR ||
         type == DYN_DYN_ANYTYPE_VAR ||
         type == DYN_DYN_LANGSTRING_VAR ||
		 type == DYN_DYN_BLOB_VAR ||
		 type == DYN_DYN_MAPPING_VAR ||
		 type == DYN_DYN_MIXED_VAR;
}

/** Compares two arrays.
  @return true if they are identical, false if not.
*/
bool emu_compareArrays(dyn_anytype array1, dyn_anytype array2) {
  int length1, length2;
  length1 = dynlen(array1);
  length2 = dynlen(array2);
  
  if (length1 != length2) {
    return FALSE;
  }
  
  for (int i=1; i <= length1; i++) {
    if (array1[i] != array2[i]) {
      return FALSE;
    }
  }
  
  return TRUE;
}

/** Converts anytype value to string. If maxChars is given then if resulting string is longer than maxChars - it's cut off and "..." is added. */
string emu_convertToString(anytype value, int maxChars = 0) {
  string strValue = (string)value;
  if ((maxChars > 0) &&(strlen(strValue) > maxChars)) {
    strValue = substr(strValue, 0, maxChars - 3);
    strValue += "...";
  }
  
  return strValue;
}

// ---=== SOME USEFUL UTILITY FUNCTIONS ===---

void emu_printMap(dyn_dyn_anytype map) {
  emu_info("Printing map:");
  for (int i=1; i <= dynlen(map); i++) {
    string line = "";
    for (int j=1; j <= dynlen(map[i]); j++) {
      line += "\t" + map[i][j] + "\t|";
    }
    emu_info(line);
  }
}

/* prints a mapping to a string. */
string emu_mappingToString(mapping map) {
  string ret = "[";
  for (int i=1; i <= mappinglen(map); i++) {
    string key = mappingGetKey(map, i);
    string value = map[key];
    ret += key + "=" + value;
    if (i != mappinglen(map)) { // if it's not the last element then append a semicolon
      ret += ";";
    }
  }
  ret += "]";
  return ret;
}

/** Prints the given array to a string using the provided separator string (default = ";") */
string emu_arrayToString(dyn_anytype array, string separator = ";") {
  string ret;
  for (int i=1; i <= dynlen(array); i++) {
    if (i > 1) {
      ret += separator;
    }
    ret += array[i];
  }
  
  return ret;
}

/** Does exactly what dynAppend does provided y as an array. But doesn't do any nonsence emptying array Y stuff like dynAppend does :| */
int emu_dynAppend(dyn_anytype &x, dyn_anytype y) {
  bool errors;
  for (int i=1; i <= dynlen(y); i++) {
    int rc = dynAppend(x, y[i]);
    if (rc == -1) {
      errors = true;
    }
  }
  if (errors) { return -1; }
  return dynlen(x);
}

/** Removes all elements from x which are also found in y. */
void emu_dynRemove(dyn_anytype &x, dyn_anytype y) {
  for (int i=1; i <= dynlen(y); i++) {
    int idx = dynContains(x, y[i]);
    if (idx > 0) {
      dynRemove(x, idx);
    }
  }
}

/** Does a dpGet, then checks if the value is different from the one provided, if so then does a dpSetWait with the value provided. */
void emu_dpSetWaitIfDifferent(string dp, anytype value) {
  anytype currentValue;
  if (_emu_isInCache(dp)) {
    currentValue = emu_dpGetCached(dp);
  } else {
    dpGet(dp, currentValue);
  }
  if (currentValue != value) {
    dpSetWait(dp, value);
  }
}

/** Does a dpGet, then checks if the value is different from the one provided, if so then does a dpSet with the value provided. */
void emu_dpSetIfDifferent(string dp, anytype value) {
  anytype currentValue;
  if (_emu_isInCache(dp)) {
    currentValue = emu_dpGetCached(dp);
  } else {
    dpGet(dp, currentValue);
  }
  if (currentValue != value) {
    dpSet(dp, value);
  }
}

/** If this DPT exists already then does dpTypeChange(), if not then dpTypeCreate(). */
void emu_dpTypeCreateOrChange(dyn_dyn_string elements, dyn_dyn_int types) {
  if (dynlen(dpTypes(elements[1][1])) > 0) {
    dpTypeChange(elements, types);
  } else {
    dpTypeCreate(elements, types);
  }
}

/** Given an FSM node, this function returns a mapping of device params */
mapping emu_fsmNodeToDeviceParams(string fsmNode, dyn_string &exceptionInfo) {
  string dp = dpSubStr(fsmNode, DPSUB_DP);
  if (dp != "") {
    fsmNode = dp;
  }
  
  if (strpos(fsmNode, "CSC_ME_") < 0) {
    emu_addError("Invalid fsmNode provided to emu_fsmNodeToDeviceParams - this node doesn't comform with the convention (*CSC_ME_*): " + fsmNode, exceptionInfo);
    return EMU_DUMMY_MAPPING;
  }
  
  fsmNode = substr(fsmNode, strpos(fsmNode, "CSC_ME_"));
  dyn_string fsmNodeSplit = strsplit(fsmNode, "_");
  mapping ret;
  ret["side"] = substr(fsmNodeSplit[3], 0, 1);
  ret["station"] = substr(fsmNodeSplit[3], 1, 1);
  ret["ring"] = substr(fsmNodeSplit[3], 2, 1);
  strreplace(fsmNodeSplit[4], "C", "");
  ret["chamberNumber"] = substr(fsmNodeSplit[4], 0, 2);
  
  return ret;
}

/**
  * Given chamber device params (must include "side", "station", "ring" and "chamberNumber" parameters) returns
  * standard chamber name in FSM notation e.g. CSC_ME_P12_C03
  */
string emu_chamberDeviceParamsToFsmName(mapping chamberDeviceParams, dyn_string &exceptionInfo) {
  emuDev_checkDeviceParams("emu_chamberDeviceParamsToFsmName", "chamber", chamberDeviceParams,
                           makeDynString("side", "station", "ring", "chamberNumber"), exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return ""; }
                           
  return "CSC_ME_" +
       chamberDeviceParams["side"] + 
       chamberDeviceParams["station"] +
       chamberDeviceParams["ring"] +
       "_C" + chamberDeviceParams["chamberNumber"];
}

/** Given a standard chamber name (e.g. ME+1/2/03), this function returns a mapping of device params */
mapping emu_chamberNameToDeviceParams(string chamberName, dyn_string &exceptionInfo) {
  if (strpos(chamberName, "ME") < 0) {
    emu_addError("Invalid chamber name provided to emu_chamberNameToDeviceParams - this name doesn't comform with the convention (ME+|-/#/#/##): " + chamberName, exceptionInfo);
    return EMU_DUMMY_MAPPING;
  }
  
  chamberName = substr(chamberName, strpos(chamberName, "ME") + 2);
  dyn_string nameSplit = strsplit(chamberName, "/");
  mapping ret;
  string sideSign = substr(nameSplit[1], 0, 1);
  if (sideSign == "+") {
    ret["side"] = "P";
  } else if (sideSign == "-") {
    ret["side"] = "M";
  }      
  ret["station"] = substr(nameSplit[1], 1, 1);
  ret["ring"] = nameSplit[2];
  ret["chamberNumber"] = nameSplit[3];
  
  return ret;
}

/**
  * @return chamber name (in a standard form e.g. ME+2/2/02) given the deviceParams (it should contain at least "side", "station", "ring" and "chamberNumber" params).
  * @param zeroPadded (optional, default=true) if set to false, will produce a non zero padded chamber name e.g. ME+1/1/1 (note that the use of this option is strongly discouraged in order to preserve a uniform format in the whole system, however it is sometimes needed e.g. when communicating with X2P).
  */
string emu_getChamberName(mapping deviceParams, bool zeroPadded = true) {
  string strSide = "+";
  if (deviceParams["side"] == "M") { strSide = "-"; }

  string ret;
  if (zeroPadded) {  
    ret = "ME" + strSide + 
          deviceParams["station"] + "/" + deviceParams["ring"] + "/" +
          deviceParams["chamberNumber"];
  } else {
    ret = "ME" + strSide + 
          deviceParams["station"] + "/" + deviceParams["ring"] + "/" +
          (int) deviceParams["chamberNumber"];
  }
  
  return ret;
}

/**
  * This function works as dpGet, but it caches the result of the DPE and next time
  * uses memory to answer the query.
  * Also the first time a particular DPE is requested, this function does a dpConnect
  * to it so that the cached value would be kept up to date.
  * @note This function is useful for rarely changing but frequently needed DPEs (like
  * number of channels of an HV board).
  * @note DO NOT USE THIS FUNCTION ON FREQUENTLY CHANGING DPEs.
  * Rule of thumb here would be this: if you think the DPE will change less frequently
  * than you're going to query it's value - use this function, otherwise don't.
  * @param keepUpToDate (optional, default = true) if this parameter is set to false (careful with that!)
  *    then the cached value of this DPE won't be updated, meaning that you will always
  *    get the same value with a call to this function with the given DPE as you got from the 
  *    first call. This removes any overhead when the DPE is updated. But this should only be 
  *    used with DPEs whose value is considered completely static (or updates are irrelevant).
  *    e.g. it would make sense for HV number of channels DPE since the total number of channels 
  *    for a given chamber type never changes, but still this DPE is inside DP which is 
  *    updated by DIM client every once in awhile.
  */
anytype emu_dpGetCached(string dpe, bool keepUpToDate = TRUE) {
  anytype ret;
  dpGet(dpe, ret); // dummy implementation for now
  return ret;

  dpe = dpSubStr(dpe, DPSUB_SYS_DP_EL_CONF_DET_ATT);
  strreplace(dpe, ":_online.._value", "");
  if (mappingHasKey(g_emu_dpCache, dpe)) {
    return g_emu_dpCache[dpe];
  } else {
    anytype ret;
    dpGet(dpe, ret);
    g_emu_dpCache[dpe] = ret;
    if (keepUpToDate) {
      dpConnect("_emu_cachedDpUpdatedCB", false, dpe);
    }
    return ret;
  }  
}

void _emu_cachedDpUpdatedCB(string dpe, anytype value) {
  strreplace(dpe, ":_online.._value", "");
  if (!mappingHasKey(g_emu_dpCache, dpe)) {
    emu_errorSingle("cache update came for a DPE that couldn't be found in the cache!! DPE = " + dpe + ". To ensure the integrity, destroying the whole cache!");
    emu_destroyCache();
    return;
  }
  g_emu_dpCache[dpe] = value;
}

/**
  * Disconnects from all the DPEs in the cache and cleans the cache.
  */
void emu_destroyCache() {
  for (int i=1; i <= mappinglen(g_emu_dpCache); i++) {
    dpDisconnect("_emu_cachedDpUpdatedCB", mappingGetKey(g_emu_dpCache, i));
  }
  mappingClear(g_emu_dpCache);
}

bool _emu_isInCache(string dpe) {
  dpe = dpSubStr(dpe, DPSUB_SYS_DP_EL_CONF_DET_ATT);
  strreplace(dpe, ":_online.._value", "");
  return mappingHasKey(g_emu_dpCache, dpe);
}
