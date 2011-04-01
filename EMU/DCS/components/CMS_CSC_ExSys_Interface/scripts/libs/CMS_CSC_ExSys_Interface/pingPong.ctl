#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysUtil.ctl"
#uses "CMS_CSC_ExSys_Interface/exsysInterface.ctl"

/**@file

DCS <-> Expert System Ping-Pong (a.k.a. heartbeat) stuff

@author Evaldas Juska (PH/UCM)
@date   March 2011
*/


private const int EXSYS_PING_PONG_INTERVAL = 60; // 1 minute = 60s
private const int EXSYS_PING_PONG_TIMEOUT = 300; // 5 minutes = 300s
public const string EXSYS_PING_PONG_ALIVE_DP = "ExsysAlert_isAlive.value";

private bool exsys_wasAlive;

/**
  * Starts the pinging and pong monitoring service.
  */
public void exsys_startPinging() {
  emu_info("Starting pinging service");
  dpGet("ExsysAlert_isAlive.value", exsys_wasAlive);
  while(true) {
    // send out the ping
    exsys_sendPingFact();
    delay(EXSYS_PING_PONG_INTERVAL);
    
    // check timestamp
    time heartbeatTimestamp;
    dpGet(EXSYS_PING_PONG_ALIVE_DP + ":_original.._stime", heartbeatTimestamp);
    
    bool isAlive;
    dpGet("ExsysAlert_isAlive.value", isAlive);
    if (heartbeatTimestamp + EXSYS_PING_PONG_TIMEOUT < getCurrentTime()) {
      isAlive = false;
    }
    
    if (!isAlive) { // timeout
      emu_info("Exsys is not responding!");
      dpSetWait("ExsysAlert_isAlive.value", false);
      if (exsys_wasAlive) {
        exsys_sendInternalCommand(EXSYS_INTERNAL_CMD_NO_EXSYS_MODE);
      }
    } else if (!exsys_wasAlive) { // is alive
      exsys_sendInternalCommand(EXSYS_INTERNAL_CMD_EXSYS_MODE);
    }
    
    exsys_wasAlive = isAlive;
  }
}

/**
  * Sends out a single ping fact.
  */
public void exsys_sendPingFact() {
  dyn_string paramNames = makeDynString("isTransient");
  dyn_anytype paramValues = makeDynAnytype(true);
  time t = getCurrentTime();
  dyn_string ex;
  exsys_sendFact("DcsPingFact", t, "ME", false, EXSYS_FACT_SEVERITY_DEBUG,
                 "DCS Ping", paramNames, paramValues, ex);
  if (emu_checkException(ex)) { return; }
}

/** Executes exsys "pong command" - basically updates the "heartbeat" timestamp. */
public void exsysCmd_pong(dyn_string args) {
  if (dpExists(EXSYS_PING_PONG_ALIVE_DP)) {
    dpSetWait(EXSYS_PING_PONG_ALIVE_DP, true);
  }
}
