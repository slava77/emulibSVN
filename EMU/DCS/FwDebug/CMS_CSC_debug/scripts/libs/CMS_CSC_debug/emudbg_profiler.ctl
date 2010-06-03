/**@file

This package contains a debugging framework component called profiler - it can be used for performance studies and benchmarking.

@author Evaldas Juska (CMS/CSC, Fermilab)
@date   June 2010
*/

private const int EMUPROF_FLUSH_INTERVAL_MS = 2000;

/** A map of "filename;funcName" -> total time spent. */
private mapping emuprof_funcTotalTimes;
private bool emuprof_isInitialized = false;
private string emuprof_dataDp;
private int emuprof_flushThreadId = 0;

/** Initializes EMU PERF framework. */
private void emuprof_init() {
  emuprof_isInitialized = true;
  
  emu_info("emuprof: initializing emu profiler");
  
  int managerId = myManId();
  char manType, manNum;
  getManIdFromInt(managerId, manType, manNum);
  string manTypeStr;
  switch (manType) {
    case CTRL_MAN:
      manTypeStr = "CTRL";
      break;
    case UI_MAN:
      manTypeStr = "UI";
      break;
    default:
      manTypeStr = "OTHER";
  }
  
  emuprof_dataDp = "emuprof_data_" + managerId + "_" + manTypeStr + "_num" + (int)manNum;
  if (!dpExists(emuprof_dataDp)) {
    dpCreate(emuprof_dataDp, "emuDbg_profiler_data");
  }
  
  if (emuprof_flushThreadId == 0) {
    emuprof_flushThreadId = startThread("emuprof_flushThread");
  }
  
  emu_info("emuprof: emu profiler initialized");  
}

/**
  * Registers the time spent executing this function in perf package.
  * Should be called at the end of a function or before return.
  * @param filename name of the file where the function is located.
  * @param funcName name of the function
  * @param timeStart time at the begining of the function execution
  * @param timeEnd time at the end of the function execution.
  */
public void emuprof_funcEnd(string filename, string funcName, time timeStart, time timeEnd) {
  if (!emuprof_isInitialized) {
    emuprof_init();
  }
  
  string funcId = filename + ";" + funcName;
  time timeSpent = timeEnd - timeStart;
  
  synchronized(emuprof_funcTotalTimes) {
    if (!mappingHasKey(emuprof_funcTotalTimes, funcId)) {
      emuprof_funcTotalTimes[funcId] = timeSpent;
    } else {
      emuprof_funcTotalTimes[funcId] += timeSpent;
    }
  }
}

/** Flushes EMU PERF data to DP. */
private void emuprof_flush() {
  dpSet(emuprof_dataDp + ".timestamp", getCurrentTime());
  mapping totalTimes = emuprof_funcTotalTimes;
  dyn_string totalTimesDynStr = makeDynString();
  for (int i=1; i <= mappinglen(totalTimes); i++) {
    string func = mappingGetKey(totalTimes, i);
    time totalTime = mappingGetValue(totalTimes, i);
    string strValue = func + ";" + emuprof_getPeriodMillis(totalTime);
    dynAppend(totalTimesDynStr, strValue);
  }
  dpSet(emuprof_dataDp + ".totalTimes", totalTimesDynStr);
}

/** Calls emuprof_flush() in every EMUPROF_FLUSH_INTERVAL_MS milliseconds. */
private void emuprof_flushThread() {
  emu_info("emuprof: starting flush thread with interval: " + EMUPROF_FLUSH_INTERVAL_MS + "ms");
  while(true) {
    emuprof_flush();
    delay(0, EMUPROF_FLUSH_INTERVAL_MS);
  }
}

/** @return time period in milliseconds (period() only returns seconds). */
public int emuprof_getPeriodMillis(time periodTime) {
  return period(periodTime) * 1000 + milliSecond(periodTime);
}
