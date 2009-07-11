/**@file

This package contains functions to query status of various parameters of various devices.

@author Evaldas Juska (PH/UCM)
@date   June 2009
*/

/** Returns datapoints representing high voltage channels that are used by the given chamber. 
    Also returns channel number offset from #1. */
void emuui_chamberGetHvChannelDps(mapping deviceParams, dyn_string &channelDps, int &channelsOffset, dyn_string &exceptionInfo) {
  dyn_string dps = emuui_getDpNames("HV_channel", deviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  channelsOffset = 0;
  int numChannels = emuui_chamberGetHvChannelCount(deviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  if (numChannels == 18) { // not a big chamber i.e. a small one
    channelsOffset = emuui_chamberGetHvChannelsOffset(deviceParams, exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
  }
  
  // in array dps there are all 36 channels of the distribution board. Now we filter out only those used by our chamber
  for (int i = channelsOffset + 1; i <= channelsOffset + numChannels; i++) {
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

/** Returns the offset of the high voltage channels in the distribution board that are use by a given chamber (normally it's 0 or 18). */
int emuui_chamberGetHvChannelsOffset(mapping deviceParams, dyn_string &exceptionInfo) {
  int channelsOffset = 0;
  
  string coordDp = emuui_getDpName("HV_coord", deviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }

  string coord;
  dpGet(coordDp, coord);
  int part = strsplit(coord, ";")[4];
  if (part == 2) {
    channelsOffset = 18;
  }
  
  return channelsOffset;
}

/** Returns the number of high voltage channels used by a given chamber. */
int emuui_chamberGetHvChannelCount(mapping deviceParams, dyn_string &exceptionInfo) {
  int numChannels;
  
  string numChannelsDp = emuui_getDpName("HV_numChannels", deviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  dpGet(numChannelsDp, numChannels);
  if (numChannels == 36) { numChannels = 18; } // if module uses all 36 channels - that means it has two chambers hooked up - so our chamber is a small one - 18 channels.

  return numChannels;
}

/** Returns problematic HV channels of the given chamber.
  @param trippedChannels        HV channels that are tripped,
  @param interlockedChannels    HV channels that are interlocked,
  @param deadChannels           HV channels that are dead,
  @param disabledChannels       HV channels that are disabled.
*/
void emuui_chamberHvGetProblematicChannels(mapping deviceParams, 
                                    dyn_int &trippedChannels, 
                                    dyn_int &interlockedChannels, 
                                    dyn_int &deadChannels, 
                                    dyn_int &disabledChannels, 
                                    dyn_string &exceptionInfo) {
  // get channel dps
  dyn_string channelDps;
  int channelsOffset;
  emuui_chamberGetHvChannelDps(deviceParams, channelDps, channelsOffset, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  // make them status dps
  for (int i=1; i <= dynlen(channelDps); i++) {
    channelDps[i] += ".status";
  }
  
  dyn_int channelStatuses;
  dpGet(channelDps, channelStatuses);
  for (int i=1; i <= dynlen(channelStatuses); i++) {
    if (channelStatuses[i] <= 3) { // if status is less than or equal to 3 it's fine - skip it.
      continue;
    }
    //problems
    if (channelStatuses[i] == 4) { // state = 4 means DEAD
      dynAppend(deadChannels, channelsOffset + i);
    } else if (channelStatuses[i] == 5) { // state = 5 means interlocked
      dynAppend(interlockedChannels, channelsOffset + i);
    } else if (channelStatuses[i] > 5) { // if status is more than 5 then it's a trip
      dynAppend(trippedChannels, channelsOffset + i);
    }
  }
  string offChannelsDp = emuui_getDpName("HV_off_channels", deviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  dpGet(offChannelsDp, disabledChannels);
  for (int i=1; i <= dynlen(disabledChannels); i++) {
    disabledChannels[i] -= channelsOffset;
  }
}

/** Returns deviceParams of ME11 high voltage channel, given its' name. */
mapping emuui_parseME11channelName(string channelName, dyn_string &exceptionInfo) {
  mapping ret;
  int sideIdx, boardIdx, channelIdx;
  
  sideIdx = strpos(channelName, "/HVME11") + strlen("/HVME11");
  boardIdx = strpos(channelName, "/board") + strlen("/board");
  channelIdx = strpos(channelName, "/channel") + strlen("/channel");
  
  if ((sideIdx < 0) || (boardIdx < 0) || (channelIdx < 0)) {
    emu_addError("One or more parameters missing in the ME11 channel name: " + channelName, exceptionInfo);
    return emuui_dummyMapping;
  }
  
  ret["side_ME11style"] = substr(channelName, sideIdx, 1);
  ret["boardNumber"] = substr(channelName, boardIdx, 2);
  ret["channelNumber"] = substr(channelName, channelIdx, 3);
  
  return ret;
}
