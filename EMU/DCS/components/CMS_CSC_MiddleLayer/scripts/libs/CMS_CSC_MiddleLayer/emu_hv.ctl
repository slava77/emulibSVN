/**@file

This package contains common functions for HV.

@author Evaldas Juska (FNAL)
@date   June 2011
*/

#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_deviceInfo.ctl"

public const string EMUHV_DP_PREFIX = "CscHighVoltage/";

public const string EMUHV_DPT_HV_FSM = "CscHvChamber";
public const string EMUHV_DPT_HV_FAST_MON = "CscHvChamberFastMon";
public const string EMUHV_DPT_HV_SLOW_MON = "CscHvChamberFastMon";
public const string EMUHV_DPT_HV_SETTINGS = "CscHvChamberSettings";
public const string EMUHV_DPT_PRIMARY_FSM = "CscHvPrimary";
public const string EMUHV_DPT_PRIMARY_MON = "CscHvPrimaryMon";

public const string EMUHV_DP_POSTFIX_FSM = "";
public const string EMUHV_DP_POSTFIX_SLOW_MON = "/SlowMon";
public const string EMUHV_DP_POSTFIX_FAST_MON = "/FastMon";
public const string EMUHV_DP_POSTFIX_SETTINGS = "/Settings";
public const string EMUHV_DP_POSTFIX_PRIMARY_MON = "/Mon";

public const int EMUHV_STATE_OFF           = 0;  // Channel or module is turned Off
public const int EMUHV_STATE_ON            = 1;  // Channel or module is turned On
public const int EMUHV_STATE_RAMP_UP       = 2;  // Channel is ramping Up
public const int EMUHV_STATE_RAMP_DOWN     = 3;  // Channel is ramping Down
public const int EMUHV_STATE_DEAD          = 4;  // Channel is Dead
public const int EMUHV_STATE_INTERLOCK     = 5;  // Module's Interlock 
public const int EMUHV_STATE_INTERNAL_TRIP = 6;  // Module detects Trip condition in one of channels
public const int EMUHV_STATE_CHANNEL_TRIP  = 7;  // Trip condition in channel
public const int EMUHV_STATE_OV_TRIP       = 8;  // Overvoltage Trip condition

public const int EMUHV_COMMAND_OFF           = 0;  // Turn Off module or channel
public const int EMUHV_COMMAND_ON            = 1;  // Turn On module or channel
public const int EMUHV_COMMAND_SET_RAMP_UP   = 3;  // Set Ramp Up rate
public const int EMUHV_COMMAND_SET_RAMP_DOWN = 4;  // Set Ramp Down rate
public const int EMUHV_COMMAND_SET_VMAX      = 5;  // Set maximum voltage output value
public const int EMUHV_COMMAND_SET_IMAX      = 6;  // Set maximum current limit (trip level)
public const int EMUHV_COMMAND_SET_IMAX_RAMP = 63; // Set trip level for ramp state
public const int EMUHV_COMMAND_SET_VSET      = 7;  // Set output voltage
public const int EMUHV_COMMAND_RESET_TRIP    = 37; // Reset trip state
public const int EMUHV_COMMAND_SET_MOD_STATE = 30; // Set module state
public const int EMUHV_COMMAND_SET_STATE     = 8;  // Set state

public const int EMUHV_VOLTAGE_NOMINAL = 3600;
public const int EMUHV_VOLTAGE_STANDBY = 3000;
public const int EMUHV_VOLTAGE_PRIMARY_STANDBY = 3200;

public const int EMUHV_DEFAULT_RAMP_UP = 10;
public const int EMUHV_DEFAULT_RAMP_DOWN = 100;
public const int EMUHV_DEFAULT_IMAX = 5;

private const int EMUHV_HOST_ID_PLUS  = 600;
private const int EMUHV_HOST_ID_MINUS = 500;

private const string EMUHV_COMMAND_DP_PREFIX = "CscDimCommand/HV_";

private global int emuhv_command_lock; // semaphore for doing dpSetWait on the DIM command DPE

/**
  * @return DP of a given type for the given device (chamber or channel).
  * @param deviceParams device parameters describing either a chamber or a channel (must include "side", "station", "ring" and "chamberNumber" params and optionally "channelNumber")
  * @param dpPostfix (optional) defines the type of the DP to be returned. Possible values: EMUHV_DP_POSTFIX_FSM, EMUHV_DP_POSTFIX_SLOW_MON, EMUHV_DP_POSTFIX_FAST_MON and EMUHV_DP_POSTFIX_SETTINGS
  */
public string emuhv_getDp(mapping deviceParams, string dpPostfix, dyn_string &exceptionInfo, bool checkIfExists = true) {
  string dp = EMUHV_DP_PREFIX + "CSC_ME_" + 
              deviceParams["side"] + 
              deviceParams["station"] +
              deviceParams["ring"] +
              "_C" + deviceParams["chamberNumber"] +
              dpPostfix;
  
  if (mappingHasKey(deviceParams, "channelNumber") && dpPostfix != EMUHV_DP_POSTFIX_FSM) {
    dp += ".channels.ch" + deviceParams["channelNumber"];
  }

  if (checkIfExists && !dpExists(dp)) {
    dyn_string dps = dpNames("*:" + dp);
    if (dynlen(dps) == 0) {
      emu_addError("HV: couldn't find HV DP for device parameters: " + deviceParams + " and dpPostfix=" + dpPostfix, exceptionInfo);
      return "";
    } else if (dynlen(dps) > 1) {
      emu_info("HV warning: more than one match found for HV DP with device parameters: " + deviceParams + " and dpPostfix=" + dpPostfix + ": " + dps + ". returning the first one.");
    }
    return dps[1];
  }
  
  return dp;
}

/**
  * @return DP of a primary PSU
  * @param deviceParams device parameters the primary PSU (must include "hostId", "port", "address")
  * @param dpPostfix (optional) defines the type of the DP to be returned. Possible values: EMUHV_DP_POSTFIX_FSM, EMUHV_DP_POSTFIX_PRIMARY_MON
  */
public string emuhv_getPrimaryDp(mapping deviceParams, string dpPostfix, dyn_string &exceptionInfo, bool checkIfExists = true) {
  string dp = EMUHV_DP_PREFIX + "Primary_" + 
              deviceParams["hostId"] + "_" +
              deviceParams["port"] + "_" +
              deviceParams["address"] +
              dpPostfix;
  
  if (checkIfExists && !dpExists(dp)) {
    dyn_string dps = dpNames("*:" + dp);
    if (dynlen(dps) == 0) {
      emu_addError("HV: couldn't find HV DP for device parameters: " + deviceParams + " and dpPostfix=" + dpPostfix, exceptionInfo);
      return "";
    } else if (dynlen(dps) > 1) {
      emu_info("HV warning: more than one match found for HV DP with device parameters: " + deviceParams + " and dpPostfix=" + dpPostfix + ": " + dps + ". returning the first one.");
    }
    return dps[1];
  }

  return dp;
}

/**
  * @return primary PSU device params ("hostId", "port" and "address")
  * @param dp DP of the primary PSU (either the FSM or Mon, doesn't matter)
  */
public mapping emuhv_getPrimaryDeviceParams(string dp, dyn_string &exceptionInfo) {
  dp = dpSubStr(dp, DPSUB_DP);
  string paramsStr = substr(dp, strpos(dp, "Primary_") + strlen("Primary_"), 7); // selects something like 600_1_3 (hostId_port_address)
  dyn_string params = strsplit(paramsStr, "_");
  if (dynlen(params) != 3) {
    emu_addError("Could not determine Primary PSU device params from dp: " + dp, exceptionInfo);
  }
  
  mapping deviceParams;
  deviceParams["hostId"] = params[1];
  deviceParams["port"] = params[2];
  deviceParams["address"] = params[3];
  
  return deviceParams;
}

/**
  * @return DPs of all channels of a given chamber.
  * @param deviceParams device parameters describing a chamber (must include "side", "station", "ring" and "chamberNumber" params)
  * @param dpPostfix defines the type of the DPs to be returned. Possible values: EMUHV_DP_POSTFIX_SLOW_MON, EMUHV_DP_POSTFIX_FAST_MON and EMUHV_DP_POSTFIX_SETTINGS
  */
public dyn_string emuhv_getAllChannelDps(mapping deviceParams, string dpPostfix, dyn_string &exceptionInfo) {
  string baseDp = dpSubStr(emuhv_getDp(deviceParams, dpPostfix, exceptionInfo), DPSUB_SYS_DP);
  if (emu_checkException(exceptionInfo)) { return makeDynString(); }
  int channelCount = emu_dpGetCached(emuhv_getDp(deviceParams, EMUHV_DP_POSTFIX_SLOW_MON, exceptionInfo) + ".num_chans");
  if (emu_checkException(exceptionInfo)) { return makeDynString(); }

  dyn_string ret;
  for (int i=1; i <= channelCount; i++) {
    dynAppend(ret, baseDp + ".channels.ch" + i);
  }
  
  return ret;
}

/**
  * @return devive params of all channels of a given chamber.
  * @param deviceParams device parameters describing a chamber (must include "side", "station", "ring" and "chamberNumber" params)
  */
public dyn_mapping emuhv_getAllChannelDevices(mapping deviceParams, dyn_string &exceptionInfo) {
  int channelCount = emu_dpGetCached(emuhv_getDp(deviceParams, EMUHV_DP_POSTFIX_SLOW_MON, exceptionInfo) + ".num_chans");
  if (emu_checkException(exceptionInfo)) { return; }
  
  dyn_mapping ret;
  for (int i=1; i <= channelCount; i++) {
    mapping ch = deviceParams;
    ch["channelNumber"] = i;
    dynAppend(ret, ch);
  }
  
  return ret;
}

/**
  * Executes a command on a given chamber or channel.
  * @param deviceParam params of the device on which the command should act - may be a chamber or a single channel (must include "side", "station", "ring" and "chamberNumber" params and optionally "channelNumber")
  * @param command command number (please use the EMUHV_COMMAND_* constants)
  * @param commandParamValue (optional) parameter for the command e.g. target voltage for the VSET command.
  */
public void emuhv_sendCommand(mapping deviceParams, int command, dyn_string &exceptionInfo, int commandParamValue = minINT()) {
  // some error checking and preparation
  if ((command != EMUHV_COMMAND_OFF) && 
      (command != EMUHV_COMMAND_ON) && 
      (command != EMUHV_COMMAND_SET_RAMP_UP) && 
      (command != EMUHV_COMMAND_SET_RAMP_DOWN) && 
      (command != EMUHV_COMMAND_SET_VMAX) && 
      (command != EMUHV_COMMAND_SET_IMAX) && 
      (command != EMUHV_COMMAND_SET_IMAX_RAMP) && 
      (command != EMUHV_COMMAND_SET_VSET) && 
      (command != EMUHV_COMMAND_RESET_TRIP) &&
      (command != EMUHV_COMMAND_SET_MOD_STATE) &&
      (command != EMUHV_COMMAND_SET_STATE)) {
    
    emu_addError("Unsupported command: " + command, exceptionInfo);
    return;
  }
  
  if ((commandParamValue == minINT()) &&
      (
          (command == EMUHV_COMMAND_SET_RAMP_UP) ||
          (command == EMUHV_COMMAND_SET_RAMP_DOWN) ||
          (command == EMUHV_COMMAND_SET_VMAX) ||
          (command == EMUHV_COMMAND_SET_IMAX) ||
          (command == EMUHV_COMMAND_SET_IMAX_RAMP) ||
          (command == EMUHV_COMMAND_SET_VSET) ||
          (command == EMUHV_COMMAND_SET_MOD_STATE) ||
          (command == EMUHV_COMMAND_SET_STATE))
     ) {
    
    emu_addError("Unsupported command: No value supplied for command " + command, exceptionInfo);
    return;
  }
  
  int commandValue = 0;
  
  if (commandParamValue != minINT()) {
    commandValue = commandParamValue;
  }

  // glue the command
  string cmdStr = emuhv_getHostId(deviceParams["side"]) + "|";
  
  int channelNumber = 255; // all channels of the chamber
  if (mappingHasKey(deviceParams, "channelNumber")) {
    channelNumber = deviceParams["channelNumber"] - 1;
  }
  
  cmdStr += "HVCSCCMD;" + 
             emu_getChamberName(deviceParams) + ";" +  // chamber name
             channelNumber + ";" +                     // channel number (255 refers to all channels)
             command + ";" +                           // command code
             commandValue + ";" +                      // optional command value
             "-1";                                     // NOT USED. data size (?)

  emu_info("HV: sending command (" + command + ") to HV server for " + emu_getChamberName(deviceParams) + ", channel number: " + channelNumber + ":" + cmdStr);
  emuhv_sendDimCommand(deviceParams["side"], cmdStr, exceptionInfo);
}

/**
  * Executes a primary PSU command.
  * @param deviceParam params of the primary PSU device on which the command should act (must include "hostId", "port", "address")
  * @param command command number (please use the EMUHV_COMMAND_* constants)
  * @param commandParamValue (optional) parameter for the command e.g. target voltage for the VSET command.
  */
public void emuhv_sendPrimaryCommand(mapping deviceParams, int command, dyn_string &exceptionInfo, int commandParamValue = minINT()) {
  // some error checking and preparation
  if ((command != EMUHV_COMMAND_OFF) &&
      (command != EMUHV_COMMAND_SET_VSET)) { // only off and standby commands are supported for primary PSUs, everything else is handled automagically by the HV server when it gets commands for chamber channels
    
    emu_addError("Unsupported command: " + command, exceptionInfo);
    return;
  }

  if ((commandParamValue == minINT()) &&
      (
          (command == EMUHV_COMMAND_SET_RAMP_UP) ||
          (command == EMUHV_COMMAND_SET_RAMP_DOWN) ||
          (command == EMUHV_COMMAND_SET_VMAX) ||
          (command == EMUHV_COMMAND_SET_IMAX) ||
          (command == EMUHV_COMMAND_SET_IMAX_RAMP) ||
          (command == EMUHV_COMMAND_SET_VSET) ||
          (command == EMUHV_COMMAND_SET_MOD_STATE) ||
          (command == EMUHV_COMMAND_SET_STATE))
     ) {
    
    emu_addError("Unsupported command: No value supplied for command " + command, exceptionInfo);
    return;
  }

  int commandValue = 0;
  
  if (commandParamValue != minINT()) {
    commandValue = commandParamValue;
  }

  string cmdStr;  
  cmdStr = deviceParams["hostId"] + "|";
  cmdStr += "HVPRCMD;" + 
            deviceParams["port"] + ";" +      // port
            deviceParams["address"] + ";" +   // address
            "255;" +                          // channels
            command + ";" +                   // command code
            commandValue + ";" +              // optional command value
            "-1";                             // NOT USED. data size (?)

  emu_info("HV: sending primary PSU command (" + command + ") to HV server for " + deviceParams + ":" + cmdStr);
  
  string side = emuhv_getSideFromHostId(deviceParams["hostId"], exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  emuhv_sendDimCommand(side, cmdStr, exceptionInfo);
}

private string emuhv_getSideFromHostId(int hostId, dyn_string &exceptionInfo) {
  string side;
  if (hostId == EMUHV_HOST_ID_MINUS) {
    side = "M";
  } else if (hostId == EMUHV_HOST_ID_PLUS) {
    side = "P";
  } else {
    emu_addError("emuhv_getSideFromHostId could not determine side from hostId = " + hostId, exceptionInfo);
    return;
  }
  
  return side;
}

/** Given a side ("P" or "M"), this function returns the host ID that can be used in an HV command. */
private int emuhv_getHostId(string side) {
  if (side == "M") {
    return EMUHV_HOST_ID_MINUS;
  } else if (side == "P") {
    return EMUHV_HOST_ID_PLUS;
  }
  return -1;
}

/**
  * Sends out the given command string to the HV server
  * @param side CSC endcap (or side) that this command is meant to
  * @param commandStr command string to be sent out
  */
private void emuhv_sendDimCommand(string side, string commandStr, dyn_string &exceptionInfo) synchronized (emuhv_command_lock) {
  string commandDp = EMUHV_COMMAND_DP_PREFIX + side;
  if (!dpExists(commandDp)) {
    dyn_string dps = dpNames("*:" + commandDp, "CscDimCommand");
    if (dynlen(dps) == 0) {
      emu_addError("Couldn't find any HV DIM command DP for this side: " + side, exceptionInfo);
      return;
    }
    commandDp = dps[1];
  }
  dpSetWait(commandDp + ".command", commandStr);
}

/**
  * Requests data from HV server for a given chamber.
  * @param chamberDeviceParams device params of a chamber that you want to request data for - has to include side, station, ring and chamberNumber
  */
public void emuhv_requestData(mapping chamberDeviceParams, dyn_string &exceptionInfo) {
  string chamberName = emu_getChamberName(chamberDeviceParams);
  string cmdStr = "HVCSCDATA|" + chamberName;
  emu_info("HV: Requesting data for " + chamberName);
  emuhv_sendDimCommand(chamberDeviceParams["side"], cmdStr, exceptionInfo);
}

/**
  * Requests primary PSU data from HV server.
  * @param primaryDeviceParams device params of the primary PSU (must include "hostId", "port", "address")
  */
public void emuhv_requestPrimaryData(mapping primaryDeviceParams, dyn_string &exceptionInfo) {
  string cmdStr = primaryDeviceParams["hostId"] + "|" +
                    "HVPRDATA;" + 
                    primaryDeviceParams["port"] + ";" + 
                    primaryDeviceParams["address"] + 
                    ";255;0;0;-1";
  emu_info("HV: Requesting data for primary PSU: " + primaryDeviceParams);
  
  string side = emuhv_getSideFromHostId(primaryDeviceParams["hostId"], exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  emuhv_sendDimCommand(side, cmdStr, exceptionInfo);
}

/** 
  * Enables / disables a given HV channel.
  * @param channelDeviceParams device params of a chamber hv channel - has to include side, station, ring, chamberNumber, channelNumber
  * @param isEnable if this is true then the channel is included, if false excluded
  */
public void emuhv_enableDisableChannel(mapping channelDeviceParams, bool isEnable, dyn_string &exceptionInfo) {
  string settingsDp = emuhv_getDp(channelDeviceParams, EMUHV_DP_POSTFIX_SETTINGS, exceptionInfo);
  string fastMonDp = emuhv_getDp(channelDeviceParams, EMUHV_DP_POSTFIX_FAST_MON, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }

  if (isEnable) {                    // ------====== ENABLE ======------
    emu_info("HV: Enabling channel #" + channelDeviceParams["channelNumber"] + " on chamber " + emu_getChamberName(channelDeviceParams));
    
    // turn the channel on if the chamber isn't off
    string fsmDp = emuhv_getDp(channelDeviceParams, EMUHV_DP_POSTFIX_FSM, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
    string fsmState;
    dpGet(fsmDp + ".fsm_state", fsmState);
    if (fsmState != "OFF") {
      emuhv_sendCommand(channelDeviceParams, EMUHV_COMMAND_ON, exceptionInfo);
      if (emu_checkException(exceptionInfo)) { return; }
    }
    
    // add the channel to summary alert
    string chamberFastMonDp = dpSubStr(fastMonDp, DPSUB_SYS_DP);
    fwAlertConfig_addDpInAlertSummary(chamberFastMonDp + ".", fastMonDp + ".status", exceptionInfo, false);
    if (emu_checkException(exceptionInfo)) { return; }
    fwAlertConfig_activate(chamberFastMonDp + ".", exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
  } else {                          // ------====== DISABLE ======------
    emu_info("HV: Disabling channel #" + channelDeviceParams["channelNumber"] + " on chamber " + emu_getChamberName(channelDeviceParams));
    
    // turn the sucker off
    emuhv_sendCommand(channelDeviceParams, EMUHV_COMMAND_OFF, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
    
    // take it out of the summary alert
    string chamberFastMonDp = dpSubStr(fastMonDp, DPSUB_SYS_DP);
    fwAlertConfig_deleteDpFromAlertSummary(chamberFastMonDp + ".", fastMonDp + ".status", exceptionInfo, false);
    if (emu_checkException(exceptionInfo)) { return; }
    fwAlertConfig_activate(chamberFastMonDp + ".", exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
  }
  
  // set disabled flag in settings
  dpSetWait(settingsDp + ".disabled", !isEnable);
}

/**
  * Changes channel nominal vset. It changes it in the settings dp and sends a command to HV server as well.
  */
public void emuhv_setChannelOnVset(mapping channelDeviceParams, int vset, dyn_string &exceptionInfo) {
  string settingsDp = emuhv_getDp(channelDeviceParams, EMUHV_DP_POSTFIX_SETTINGS, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  dpSetWait(settingsDp + ".on_vset", vset);
  emuhv_sendCommand(channelDeviceParams, EMUHV_COMMAND_SET_VSET, exceptionInfo, vset);
  if (emu_checkException(exceptionInfo)) { return; }
}

/**
  * Checks integrity of the system and fixes any problems that are fixable and reports back those which are not fixable.
  * e.g. it checks that all the necessary alerts are enabled and that all channels that are not disabled are in the summary alert configurations.
  */
public void emuhv_checkIntegrity(dyn_string &exceptionInfo) {
  emu_info("HV: Running integrity check");
  
  dyn_string dps = dpNames(EMUHV_DP_PREFIX + "*", "CscHvChamber");
  for (int i=1; i <= dynlen(dps); i++) {
    emu_debug("HV: Checking " + dps[i]);

    string numChannelsDp = dps[i] + EMUHV_DP_POSTFIX_SLOW_MON + ".num_chans";
    int numChannels = emu_dpGetCached(numChannelsDp);
    if (numChannels <= 0) {
      emu_addError("Invalid number of HV channels! " +
                   numChannelsDp + " = " + numChannels, exceptionInfo);
      continue;
    }
    for (int ch=1; ch <= numChannels; ch++) {
      // make sure that disabled channels are not in summary alerts and enabled ones are in summary alerts
      string channelSettingsDp = dps[i] + EMUHV_DP_POSTFIX_SETTINGS + ".channels.ch" + ch;
      bool isDisabled = emu_dpGetCached(channelSettingsDp + ".disabled");
      string channelStatusDpe = dps[i] + EMUHV_DP_POSTFIX_FAST_MON + ".channels.ch" + ch + ".status";
      if (isDisabled) {
        fwAlertConfig_deleteDpFromAlertSummary(dps[i] + EMUHV_DP_POSTFIX_FAST_MON + ".",
                                               channelStatusDpe,
                                               exceptionInfo,
                                               false);
      } else {
        fwAlertConfig_addDpInAlertSummary(dps[i] + EMUHV_DP_POSTFIX_FAST_MON + ".",
                                          channelStatusDpe,
                                          exceptionInfo,
                                          false);
      }
      if (emu_checkException(exceptionInfo)) { return; }
      
      // make sure the channel status alert is active
      fwAlertConfig_activate(channelStatusDpe, exceptionInfo);
      if (emu_checkException(exceptionInfo)) { return; }
      
      int onVset = emu_dpGetCached(channelSettingsDp + ".on_vset");
      if (onVset == 0) {
        dpSet(channelSettingsDp + ".on_vset", EMUHV_VOLTAGE_NOMINAL);
      }
      int imax = emu_dpGetCached(channelSettingsDp + ".imax");
      if (imax == 0) {
        dpSet(channelSettingsDp + ".imax", EMUHV_DEFAULT_IMAX);
      }
    }
    
    // make sure all the summary alerts are active
    fwAlertConfig_activate(dps[i] + ".", exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
    fwAlertConfig_activate(dps[i] + EMUHV_DP_POSTFIX_SLOW_MON + ".", exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
    fwAlertConfig_activate(dps[i] + EMUHV_DP_POSTFIX_FAST_MON + ".", exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
  }
  
  emu_info("HV: Integrity check finished");
}

public void emuhv_reconfigure() {
  // set default settings to all channels
}
