/**@file

This package contains procedures to monitor communication for timeouts.

@author Evaldas Juska (FNAL)
@date   July 2010
*/

// EMU common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_util.ctl"

// HV_CC common libs
#uses "CMS_CSC_MiddleLayer/emu_x2p.ctl"

/** 
  * DP to last good timestamp mapping (key can be original value e.g. CSC_ME_P11_C01_LV.update_value or it can be a PVSS timestamp config e.g. CSC_ME_P12_C01_HV.update_value:_online.._stime)
  * initially all timestamps are set to current time
  */
private mapping emu_comm_lastGoodTimestamps;
private dyn_string emu_comm_timedOutDevices;

/** Time interval (in seconds) that the monitoring loop sleeps after checking X2P timestamps */
private const int EMU_COMM_X2P_MONITOR_INTERVAL = 60; // 1 minute = 60s
private const int EMU_COMM_X2P_TIMEOUT = 600; // 10 minutes = 600s

private const int EMU_COMM_HV_PR_MONITOR_INTERVAL = 60; // 1 minute = 60s
private const int EMU_COMM_HV_PR_TIMEOUT = 300; // 5 minutes = 300s

void main() {
  emu_commMonitorInit();
}

/** 
  * Initializes the communication timeout monitoring data and starts the monitoring thread. 
  */
public void emu_commMonitorInit() {
  emu_info("EMU communication monitor: Initializing communication monitoring for timeouts...");

  dpSetWait("COMM_TIMEOUTS.value", "");

  // X2P
  dyn_string x2pTimestampDps = emux2p_getAllDataDps();
  for (int i=1; i <= dynlen(x2pTimestampDps); i++) {
    x2pTimestampDps[i] += ".timestamp";
  }
  emu_info("EMU communication monitor: starting X2P communication timeout monitoring loop.");
  startThread("_emu_commMonitorThread", x2pTimestampDps, EMU_COMM_X2P_MONITOR_INTERVAL, EMU_COMM_X2P_TIMEOUT, true);
  
  // HV Primaries
  dyn_string primaryDataDps = dpNames("*.status", "CscHvPrimaryMon");
  emu_info("EMU communication monitor: starting HV Primary PSU communication timeout monitoring loop.");
  startThread("_emu_commMonitorThread", primaryDataDps, EMU_COMM_HV_PR_MONITOR_INTERVAL, EMU_COMM_HV_PR_TIMEOUT, false, true);
  
  emu_info("EMU communication monitor: Done initializing communication monitoring for timeouts.");  
}

/** 
  * Main monitoring loop - every "interval" seconds checks all timestamps provided in "timestampDpList" and if it finds any that are older than the timeout threshold, then flags those devices with a communication problem.
  * @param timestampDpList timestamp DPs that this procedure should be checking (can be original value e.g. CSC_ME_P11_C01.update_value or it can be a PVSS timestamp config e.g. CSC_ME_P12_C01.update_value:_online.._stime).
  * @param interval time interval (in seconds) that the monitoring loop sleeps after checking all timestamps.
  * @param timeout timeout threshold (maximum allowed "age" of a timestamp).
  * @param isX2pData (optional, default=false) if this flag is true, then data status DPE is checked and data is used only if that status says that data is ok (X2P specific).
  * @param useDpStime (optional, default=false) if this flag is true, then the DP:_online.._stime config is used to determine last update time and if this flag is false then the DP value is used to determine the time of last update
  */
private void _emu_commMonitorThread(dyn_string timestampDpList, int interval, int timeout, bool isX2pData = false, bool useDpStime = false) {

  dyn_string fsmDpList;
  // determine the FSM DPs for each of the timestamp DPs.
  for (int i=1; i <= dynlen(timestampDpList); i++) {
    string fsmDp = dpSubStr(timestampDpList[i], DPSUB_DP);
    strreplace(fsmDp, "/FastMon", "");
    strreplace(fsmDp, "/SlowMon", "");
    strreplace(fsmDp, "/Mon", "");
    dynAppend(fsmDpList, fsmDp);
  }
  
  dyn_string ex;
  bool firstRound = true;
  while(true) {
    emu_debug("EMU communication monitor: checking timestamps with interval=" + interval + ", timeout=" + timeout, emu_DEBUG_DETAIL);
    time currentTime = getCurrentTime();
    
    bool listHasChanged = false;
    for (int i=1; i <= dynlen(timestampDpList); i++) {
      string timestampDp = timestampDpList[i];
      string fsmDp = fsmDpList[i];
      time timestamp;
      if (!useDpStime) {
        dpGet(timestampDp, timestamp);
      } else {
        timestamp = emu_getLastUpdateTimeAsTime(timestampDp);
      }
      
      if (!firstRound) {
        time lastTimestamp = emu_comm_lastGoodTimestamps[timestampDp]; // get the last good timestamp
        if (lastTimestamp > timestamp) {                               // if the timestamp is older than the last good one, we want to use the last good one (this may happen e.g. when this procedure is started and data is not yet flowing (project startup) - we don't want to time it out immediately)
          timestamp = lastTimestamp;
        }
        
        if (isX2pData) { // special additional handling for X2P data. X2P provides status bit which can tell if data is ok to use or not - if not, then just do timestamp = lastTimestamp
          string statusDp = dpSubStr(timestampDp, DPSUB_SYS_DP) + ".status";
          int status;
          dpGet(statusDp, status);
          if (status & EMU_X2P_STATUS_BAD_DATA) { // it's bad data - ignore it
            timestamp = lastTimestamp;
          }
        }
        
        
        if (timestamp + timeout < currentTime) {                        // TIMEOUT!
          emu_info("EMU communication monitor: Communication timeout for " + fsmDp);
          synchronized(emu_comm_timedOutDevices) {
            if (!dynContains(emu_comm_timedOutDevices, fsmDp)) {
              dynAppend(emu_comm_timedOutDevices, fsmDp);
              listHasChanged = true;
              dpSet(fsmDp + ".fsm_state", "NO_COMMUNICATION");
            }
          }
        } else {                                                       // came back from the dead :)
          synchronized(emu_comm_timedOutDevices) {
            int index = dynContains(emu_comm_timedOutDevices, fsmDp);
            if (index > 0) {
              dynRemove(emu_comm_timedOutDevices, index);
              listHasChanged = true;
              emu_info("EMU communication monitor: Communication is now OK with " + fsmDp);
            }
          }
        }
        
        if (timestamp != lastTimestamp) {                              // if it's different (newer) then save that to the mapping
          emu_comm_lastGoodTimestamps[timestampDp] = timestamp;
        }
      } else { // on the first round we just put them in emu_comm_lastGoodTimestamps with the current time (if they don't exists there yet)
        if (!mappingHasKey(emu_comm_lastGoodTimestamps, timestampDp)) {
          emu_comm_lastGoodTimestamps[timestampDp] = currentTime;
        }
      }
    }

    // convert emu_comm_timedOutDevices to string and set the COMM_TIMEOUTS DP
    if (listHasChanged) { // write this DP only if the list has changed (that will ease the load on the UI who has a dpConnect to it
      synchronized(emu_comm_timedOutDevices) {
        string timedOutDevicesString = emu_arrayToString(emu_comm_timedOutDevices, " ");
        dpSetWait("COMM_TIMEOUTS.value", timedOutDevicesString);
      }
    }

    // sleep the "interval"
    if (firstRound) { firstRound = false; }
    emu_debug("EMU communication monitor: done checking timestamps, sleeping for " + interval + "s", emu_DEBUG_DETAIL);
    delay(interval);
  }
}
