#uses "CMS_CSCfw_HV_CC/emu_hvCommon.ctl"

/**@file

HV Commands

@author Evaldas Juska (PH/UCM)
@date   March 2011
*/

/**
  * Forces on an HV channel
  * @param args[1] - HV channel DP
  */
public void exsysCmd_hvChannelForceOn(dyn_string args) {
  dyn_string ex;
  string channelDp = args[1];
  if (!dpExists(channelDp)) {
    return;
  }
  
  emu_info("ExSys command: Forcing on HV channel " + channelDp);
    
  if (!_exsysCmd_isMe11(channelDp)) { // non-ME1/1 channels
    mapping channelDeviceParams = emuhv_getHvChannel(channelDp, ex);
    if (emu_checkException(ex)) { return; }
  
    int state, status;
    dpGet(channelDp + ".state", state,
          channelDp + ".status", status);
  
    // switch it off if it's tripped
    if ((state == 0) && (status >= 4)) {
      emuhv_sendChannelCommand(channelDeviceParams, EMUHV_COMMAND_OFF, ex);
      if (emu_checkException(ex)) { return; }
    }
  
    // turn it back on
    emuhv_sendChannelCommand(channelDeviceParams, EMUHV_COMMAND_ON, ex);

    // refresh the data now  
    delay(1, 0);
    emuhv_requestData(channelDeviceParams, ex);
    if (emu_checkException(ex)) { return; }  
  } else { // ME1/1 channels
    dpSetWait(channelDp + ".settings.onOff", false);
    delay(3);
    dpSetWait(channelDp + ".settings.onOff", true);
  }
}

/**
  * Forces on an HV channel
  * @param args[1] - HV channel DP
  * @param args[2] - target voltage
  */
public void exsysCmd_hvChannelVset(dyn_string args) {
  dyn_string ex;
  string channelDp = args[1];
  if (!dpExists(channelDp)) {
    return;
  }
  int vset = args[2];

  emu_info("ExSys command: Changing HV channel " + channelDp + " VSET to " + vset);
  
  if (!_exsysCmd_isMe11(channelDp)) { // non-ME1/1 channels  
    mapping channelDeviceParams = emuhv_getHvChannel(channelDp, ex);
    if (emu_checkException(ex)) { return; }

  
    int state, status;
    dpGet(channelDp + ".state", state,
          channelDp + ".status", status);
  
    emuhv_changeChannelVset(channelDeviceParams, vset, ex);
    if (emu_checkException(ex)) { return; }

    // refresh the data now  
    delay(1, 0);
    emuhv_requestData(channelDeviceParams, ex);
    if (emu_checkException(ex)) { return; }
  } else { // ME1/1 channels
    if (vset > 2950) {
      emu_errorSingle("Got a vset command for ME1/1 for " + vset + "V. No way I'm going to do that, the upper voltage limit for ME1/1 is 2950V");
      return;
    }
    dpSetWait(channelDp + ".userDefined", vset);
    int v0;
    dpGet(channelDp + ".readBackSettings.v0", v0);
    if (v0 != vset) {
      dpSetWait(channelDp + ".settings.v0", vset);
    }
  }
}

/**
  * Disables an HV channel
  * @param args[1] - HV channel DP
  */
public void exsysCmd_hvChannelDisable(dyn_string args) {
  dyn_string ex;
  string channelDp = args[1];
  if (!dpExists(channelDp)) {
    return;
  }
  
  emu_info("ExSys command: Disabling HV channel " + channelDp);
  
  if (!_exsysCmd_isMe11(channelDp)) { // non-ME1/1 channels    
    mapping channelDeviceParams = emuhv_getHvChannel(channelDp, ex);
    if (emu_checkException(ex)) { return; }

    emuhv_enableDisableChannel(channelDeviceParams, false, ex);
    if (emu_checkException(ex)) { return; }

    // refresh the data now  
    delay(1, 0);
    emuhv_requestData(channelDeviceParams, ex);
    if (emu_checkException(ex)) { return; }
  } else { //ME1/1 channels
    dpSetWait(channelDp + ".settings.onOff", false);
    string node = strsplit(dpNameToAlias(channelDp + "."), "/")[3];
    fwCU_disableObj(node, channelDp);
  }
}

private bool _exsysCmd_isMe11(string dp) {
  return (strpos(dp, "CAEN") >= 0);
}
