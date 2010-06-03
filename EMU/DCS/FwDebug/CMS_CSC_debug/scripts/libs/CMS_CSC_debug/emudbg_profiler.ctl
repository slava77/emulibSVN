/**@file

This package contains a framework for performance studies and benchmarking.

@author Evaldas Juska (PH/UCM)
@date   June 2010
*/

private const int EMUPERF_FLUSH_INTERVAL_MS = 2000;

/** A map of "filename;funcName" -> total time spent. */
private mapping emuperf_funcTotalTimes;
private bool emuperf_isInitialized = false;
private string emuperf_dataDp;
private int emuperf_flushThreadId = 0;

/** Initializes EMU PERF framework. */
private void emuperf_init() {
  emuperf_isInitialized = true;
  
  emu_info("emuperf: initializing emuPerf framework");
  
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
  
  emuperf_dataDp = "emuPerf_data_" + managerId + "_" + manTypeStr + "_num" + (int)manNum;
  if (!dpExists(emuperf_dataDp)) {
    dpCreate(emuperf_dataDp, "emuPerf_data");
  }
  
  if (emuperf_flushThreadId == 0) {
    emuperf_flushThreadId = startThread("emuperf_flushThread");
  }
  
  emu_info("emuperf: emuPerf initialized");  
}

/**
  * Registers the time spent executing this function in perf package.
  * Should be called at the end of a function or before return.
  * @param filename name of the file where the function is located.
  * @param funcName name of the function
  * @param timeStart time at the begining of the function execution
  * @param timeEnd time at the end of the function execution.
  */
public void emuperf_funcEnd(string filename, string funcName, time timeStart, time timeEnd) {
  if (!emuperf_isInitialized) {
    emuperf_init();
  }
  
  string funcId = filename + ";" + funcName;
  time timeSpent = timeEnd - timeStart;
  
  synchronized(emuperf_funcTotalTimes) {
    if (!mappingHasKey(emuperf_funcTotalTimes, funcId)) {
      emuperf_funcTotalTimes[funcId] = timeSpent;
    } else {
      emuperf_funcTotalTimes[funcId] += timeSpent;
    }
  }
}

/** Flushes EMU PERF data to DP. */
private void emuperf_flush() {
  dpSet(emuperf_dataDp + ".timestamp", getCurrentTime());
  mapping totalTimes = emuperf_funcTotalTimes;
  dyn_string totalTimesDynStr = makeDynString();
  for (int i=1; i <= mappinglen(totalTimes); i++) {
    string func = mappingGetKey(totalTimes, i);
    time totalTime = mappingGetValue(totalTimes, i);
    string strValue = func + ";" + emuperf_getPeriodMillis(totalTime);
    dynAppend(totalTimesDynStr, strValue);
  }
  dpSet(emuperf_dataDp + ".totalTimes", totalTimesDynStr);
}

/** Calls emuperf_flush() in every EMUPERF_FLUSH_INTERVAL_MS milliseconds. */
private void emuperf_flushThread() {
  emu_info("emuperf: starting flush thread with interval: " + EMUPERF_FLUSH_INTERVAL_MS + "ms");
  while(true) {
    emuperf_flush();
    delay(0, EMUPERF_FLUSH_INTERVAL_MS);
  }
}

/** @return time period in milliseconds (period() only returns seconds). */
public int emuperf_getPeriodMillis(time periodTime) {
  return period(periodTime) * 1000 + milliSecond(periodTime);
}
