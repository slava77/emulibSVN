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
  mapping channelDeviceParams = emuhv_getHvChannel(channelDp, ex);
  if (emu_checkException(ex)) { return; }
  
  emu_info("ExSys command: Forcing on HV channel " + channelDp);
  
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
  mapping channelDeviceParams = emuhv_getHvChannel(channelDp, ex);
  if (emu_checkException(ex)) { return; }

  emu_info("ExSys command: Changing HV channel " + channelDp + " VSET to " + vset);
  
  int state, status;
  dpGet(channelDp + ".state", state,
        channelDp + ".status", status);
  
  emuhv_changeChannelVset(channelDeviceParams, vset, ex);
  if (emu_checkException(ex)) { return; }

  // refresh the data now  
  delay(1, 0);
  emuhv_requestData(channelDeviceParams, ex);
  if (emu_checkException(ex)) { return; }
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
  mapping channelDeviceParams = emuhv_getHvChannel(channelDp, ex);
  if (emu_checkException(ex)) { return; }

  emu_info("ExSys command: Disabling HV channel " + channelDp);
  
  emuhv_enableDisableChannel(channelDeviceParams, false, ex);
  if (emu_checkException(ex)) { return; }

  // refresh the data now  
  delay(1, 0);
  emuhv_requestData(channelDeviceParams, ex);
  if (emu_checkException(ex)) { return; }
}
