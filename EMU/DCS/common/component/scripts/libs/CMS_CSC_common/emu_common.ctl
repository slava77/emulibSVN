/**@file

This package contains general purpose utility functions.

@author Evaldas Juska (PH/UCM)
@date   March 2009
*/

const int emu_ERROR_REPORT_DELAY = 150; //ms

const int emu_DEBUG_GENERAL = 1;
const int emu_DEBUG_FUNC_START_STOP = 2;
const int emu_DEBUG_DETAIL = 4;

/** Debug level (bitmask)
  1 - general debug messages
  2 - function start/stop messages
*/
global int g_emu_Debug = 5;//emu_DEBUG_GENERAL | emu_DEBUG_FUNC_START_STOP;

global dyn_string g_emu_debugBacktrace;
global dyn_int g_emu_reportingThreads;
global dyn_dyn_string g_emu_exceptionsToReport;

/** Register an EMU error
  @param msg         Error message. 
*/
void emu_errorSingle(string msg) {
  emu_error(makeDynString(msg));
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
    DebugTN("EMU - skipping the same error report");
    stopThread(g_emu_reportingThreads[index]);
    dynRemove(g_emu_reportingThreads, index);
    dynRemove(g_emu_exceptionsToReport, index);
  } 
  //if this isn't the same error that's already waiting then maybe it's just a more detailed one - then do the same because we want this one to be displayed
  if (index <= 0) {  
    index = _emu_findSimilarInMap(g_emu_exceptionsToReport, error);
    if (index > 0) {
      DebugTN("EMU - skipping a less detailed error report");
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
  fwExceptionHandling_display(exInfoForGraphics);
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

/** Register an info message.
  @param msg           Info message.
*/
void emu_info(string msg) {
  DebugTN("EMU INFO: " + msg);
}

/** Register a debug message.
  @param msg           Debug message.
  @param level         Level of the debug message. See g_emu_Debug.
*/
void emu_debug(string msg, int level = 1 /*emu_DEBUG_GENERAL*/) {
  if ((g_emu_Debug & level) > 0) {
    DebugTN("EMU DEBUG: " + msg);
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

int emu_dynDynContains(dyn_dyn_anytype map, dyn_anytype array) {
  for (int i=1; i <= dynlen(map); i++) {
    if ((bool) (map[i] == array)) {
      return i;
    }
  }
  return -1;
}

//utility functions
string emu_decToHex(int value, int byteCount = 2) {
  blob tmpBlob;
  blobAppendValue(tmpBlob, 255, byteCount, true);
  string ret = tmpBlob;
  
  return tmpBlob;
}