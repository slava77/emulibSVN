/**@file

This package contains common functions for HV.

@author Evaldas Juska (PH/UCM)
@date   July 2010
*/

public const int EMUHV_COMMAND_OFF = 0;
public const int EMUHV_COMMAND_ON = 1;
//public const int EMUHV_COMMAND_STANDBY = 1;

/** 
  * Returns DP of the requested HV channel. Useful because it does the conversion between channel number on a chamber (that's what you provide) and channel
  *   number on the board (that's what you need in order to get the actual DP.
  * @param channelDeviceParams device params of the channel you're looking for - must include side, station, ring, chamber, channelNumber.
  */
public string emuhv_getHvChannelDp(mapping channelDeviceParams, dyn_string &exceptionInfo) {
  // get the what channel number on the hv module corresponds to the given chamber hv channel number
  int moduleChannelNum = _emuhv_getModuleHvChannelNum(channelDeviceParams, channelDeviceParams["channelNumber"], exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return ""; }
  channelDeviceParams["moduleChannelNumber"] = moduleChannelNum;
  string channelDp = emuui_getDpName("HV_channel", channelDeviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return ""; }
  return channelDp;
}

/** Converts chamber hv channel number to the channel number in the module (for small chambers this may be different - two chambers are connected to the same module) */
private int _emuhv_getModuleHvChannelNum(mapping chamberDeviceParams, int chamberHvChannelNum, dyn_string &exceptionInfo) {
  int channelsOffset = 0;
  
  string coordDp = emuui_getDpName("HV_coord", deviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return -1; }

  string coord;
  dpGet(coordDp, coord);
  int part = strsplit(coord, ";")[4];
  if (part == 2) {
    channelsOffset = 18;
  }

  return channelsOffset + chamberHvChannelNum;
}

/**
  * Sends an HV command to all channels of a given chamber
  * @param chamberDeviceParams device params of the chamber - has to include side, station, ring, chamberNumber
  * @param command command - use EMUHV_COMMAND_* constants
  */
void emuhv_sendAllChannelsCommand(mapping chamberDeviceParams, int command, dyn_string &exceptionInfo) {
  emuhv_sendChannelCommand(chamberDeviceParams, command, exceptionInfo, true);
}

/**
  * Sends an HV command to a given channel
  * @param channelDeviceParams device params of a chamber hv channel - has to include side, station, ring, chamberNumber, channelNumber (and optionally moduleChannelNumber - if that's provided, then it's faster)
  * @param command command - use EMUHV_COMMAND_* constants
  * @param broadcastToAllChannels if this is true then channelDeviceParams["channelNumber"] is ignored and command is sent to all channels on that chamber
  */
void emuhv_sendChannelCommand(mapping channelDeviceParams, int command, dyn_string &exceptionInfo, bool broadcastToAllChannels = false) {
  if ((command != EMUHV_COMMAND_OFF) && (command != EMUHV_COMMAND_ON)) {
    emu_addError("Unsupported command: " + command, exceptionInfo);
    return;
  }
  int commandValue = 0;
  
  int channelNumber;
  
  if (broadcastToAllChannels) { // BROADCAST - all channels on the chamber
    int channelOffset = emuui_chamberGetHvChannelsOffset(chamberDeviceParams, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }

    int channelCount = emuui_chamberGetHvChannelCount(chamberDeviceParams, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
    
    if (channelCount < 30) { // small chamber - so only half of the channels on the board
      if (channelOffset == 0) {
        channelNumber = 253; // all channels in the first part of the distribution board
      } else {
        channelNumber = 254; // all channels in the second part of the distribution board
      }
    } else {                 // big chamber - so all channels on the board
      channelNumber = 255;
    }
  } else {  // SINGLE CHANNEL
    if (mappingHasKey(channelDeviceParams, "moduleChannelNumber")) {
      channelNumber = channelDeviceParams["moduleChannelNumber"];
    } else {
      channelNumber = _emuhv_getModuleHvChannelNum(channelDeviceParams, channelDeviceParams["channelNumber"], exceptionInfo);
      if (emu_checkException(exceptionInfo)) { return; }
    }
    channelNumber--; // we need 0-based
  }

  string coordsDp = emuui_getDpName("HV_coord", channelDeviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  string coordsStr;
  dpGet(coordsDp, coordsStr);
  dyn_string coords = strsplit(coordsStr, ";");

  // HERE WE GO
  
  string host = coords[1];
  string subcommand = "HVCMD;" + coords[2] + ";" + coords[3] + ";" + channelNumber + ";" + command + ";" + commandValue + ";" + "-1"; //e.g.: 500|HVCMD;6;3;29;1;0;-1 or 500|HVCMD;5;8;10;0;0;-1

  dpSetWait(dpSubStr(coordsDp, DPSUB_SYS) + "HV_1_COM.command", host + "|" + subcommand);
//  dpSetWait(mudcsAddSystem("HV_1_COM"+".command",project_system),host+"|"+subcommand);

  
  
  
  
  
  
return;  
  
  if(command == 7){
    dpSetWait(DpNameFsm+".last_vset",value); 
  
  int master_id, master_chan, master_index;
  dyn_string coord_master;

  // master channel vset is set
   dpSetWait(DpNameFsm+".status",2);
   mudcsGetMasterForChamber(mudcsAddSystem(DpNameFsm), master_id, 
                           master_chan, master_index, coord_master);
      subcommand="HVCMD;"+coord_master[2]+";"+coord_master[3]+";"+master_chan+";"+7+";"+value+";"+"-1";
      dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord_master[1]+"|"+subcommand);
 
     
  }
  //mudcsDebug(project_system);  
  COMMAND_FLAG=1;

  //....
}

/** 
  * Enables / disables a given HV channel.
  * @param channelDeviceParams device params of a chamber hv channel - has to include side, station, ring, chamberNumber, channelNumber
  * @param isEnable if this is true then the channel is included, if false excluded
  */
public void emuhv_enableDisableChannel(mapping channelDeviceParams, bool isEnable, dyn_string &exceptionInfo) {
  // get the what channel number on the hv module corresponds to the given chamber hv channel number
  int moduleChannelNum = _emuhv_getModuleHvChannelNum(channelDeviceParams, channelDeviceParams["channelNumber"], exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  channelDeviceParams["moduleChannelNumber"] = moduleChannelNum;
  
  // get the necessary DPs  
  
  string offChannelsDp = emu_getDpName("HV_off_channels", channelDeviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  dyn_int offChannels;
  dpGet(offChannelsDp, offChannels);
  
  string moduleDp = emu_getDpName("HV_module", channelDeviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  string channelDp = emu_getDpName("HV_channel", channelDeviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }

  // showtime
  
  if (isEnable) {                               // ------====== INCLUDE ======------
    // turn the channel off
    emuhv_sendChannelCommand(channelDeviceParams, EMUHV_COMMAND_ON, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }

    // pull it out of excluded channels list
    int idx = dynContains(offChannels, moduleChannelNum);
    if (idx) {
      dynRemove(offChannels, idx);
    }
    dpSet(offChannelsDp, offChannels);

    // add it to the summary alert
    fwAlertConfig_addDpInAlertSummary(moduleDp + ".", channelDp + ".status", exceptionInfo, false);
    if (emu_checkException(exceptionInfo)) { return; }
  } else {                                       // ------====== EXCLUDE ======------
    // turn the channel on
    emuhv_sendChannelCommand(channelDeviceParams, EMUHV_COMMAND_OFF, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }

    // put it into excluded channels list
    if (!dynContains(offChannels, moduleChannelNum)) {
      dynAppend(offChannels, moduleChannelNum);
    }
    dpSet(offChannelsDp, offChannels);

    // take it out of the summary alert
    fwAlertConfig_deleteDpFromAlertSummary(moduleDp + ".", channelDp + ".status", exceptionInfo, false);
    if (emu_checkException(exceptionInfo)) { return; }
  }
}


/** Returns datapoints holding high voltage channel detailed information (like vset, imax, etc..) that are used by the given chamber. 
    Also returns channel number offset from #1. */
void emuhv_chamberGetHvChannelDetailsDps(mapping deviceParams, dyn_string &channelDps, int &channelsOffset, dyn_string &exceptionInfo) {
  dyn_string dps = emuui_getDpNames("HV_channelDetails", deviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  channelsOffset = 0;
  int numChannels = emuui_chamberGetHvChannelCount(deviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  if (numChannels == 18) { // not a big chamber i.e. a small one
    channelsOffset = emuui_chamberGetHvChannelsOffset(deviceParams, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
  }
  
  // in array dps there are all 36 channels of the distribution board (packed into 18 DPs actually). Now we filter out only those used by our chamber
  // divide by 2 for DP offset and count because every details DP holds data for two channels
  for (int i = (channelsOffset / 2) + 1; i <= (channelsOffset / 2) + (numChannels / 2); i++) {
    // the dps array is very badly sorted so we need this ugly search procedure
    string channelNumPart = ".v" + i;
    int index;
    for (int j=1; j <= dynlen(dps); j++) {
      if (strpos(dps[j], channelNumPart) >= 0) {
        index = j;
        break;
      }
    }
    dynAppend(channelDps, dps[index]);
  }
}
