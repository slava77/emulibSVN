// Common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_math.ctl"

// UI libs
#uses "CMS_CSC_UI/emuui_main.ctl"
#uses "CMS_CSC_UI/emuui_configuration.ctl"
#uses "CMS_CSC_UI/emuui_sessionManager.ctl"
#uses "CMS_CSC_UI/emuui_utils.ctl"
#uses "CMS_CSC_UI/emuui_deviceInfo.ctl"

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

/** A recursive function which tells if the given FSM node is disabled 
    (i.e. this node or any of its parents are either disabled or "excluded and lockedOut", any other state is considered as enabled). */
bool emuui_isFsmNodeEnabled(string fsmNode) {
  emu_info("IS FSM NODE ENABLED: " + fsmNode);
  
  // if at any level of the hierarchy we get operation flag == 1 - break recursing and return ENABLED
  int operationFlag = fwCU_getOperationMode(fsmNode);
  if (operationFlag == 1) {
    return true;
  }
  // at this point (we have operation flag == 0) it's possible that our device or it's parent is disabled
  string domain = fsmNode;
  string object = _fwCU_getNodeObj(domain);

  if (fwFsm_isCU(domain, object)) {
    string cuMode = fwFsmUi_getCUMode(domain, object);
    emu_info("IS FSM NODE ENABLED: " + fsmNode + " is CU and it's mode is " + cuMode);
    if (cuMode == "Disabled") {
      return false;
    } else {
      int type;
      string parent = fwCU_getParent(type, fsmNode);
      if (parent == "") { // top level node reached
        return true;
      } else {
        return emuui_isFsmNodeEnabled(parent);
      }
    }
  } else {
    string duMode = fwFsmUi_getDUMode(domain, object);
    emu_info("IS FSM NODE ENABLED: " + fsmNode + " is DU and it's mode is " + duMode);
    if (duMode == "Disabled") {
      return false;
    } else {
      int type;
      string parent = fwCU_getParent(type, fsmNode);
      if (parent == "") { // top level node reached
        return true;
      } else {
        return emuui_isFsmNodeEnabled(parent);
      }
    }
  }
}

/** Returns Maraton device parameters given an ID. */
mapping emuui_getMaratonDeviceParams(int maratonId, dyn_string &exceptionInfo) {
  mapping maratonDbSwapped = emuui_getMaratonDbSwapped(exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  string maratonStr = maratonDbSwapped[(string)maratonId];
  mapping deviceParams;
  
  dyn_string split = strsplit(maratonStr, "_"); // e.g M4_CR2
  deviceParams["side"] = split[1][0]; // first part e.g. is M4 and second is CR2
  int station = (string) split[1][1];
  deviceParams["station"] = station;
  int disk = station;
  if (disk > 2) { disk--; } // Valeri named them after stations ME 1, 2, 4 (I like disks YE 1, 2, 3).
  deviceParams["disk"] = disk;
  strreplace(split[2], "CR", "");
  deviceParams["crateNum"] = split[2];
  
  return deviceParams;
}

/** Given the ME11 (dubna) chamber deviceParams mapping this function adds neccessary parameters for ME11 HV channels. */
mapping emuui_getME11HVChannelsDeviceParams(mapping deviceParams, dyn_string &exceptionInfo) {
  dyn_string channelFsmNodes = emuui_getME11HVChannelFsmNodes(deviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  string boardAndChannelParam = "{";
  for (int i=1; i <= dynlen(channelFsmNodes); i++) {
    mapping channelDeviceParams = emuui_parseME11channelName(channelFsmNodes[i], exceptionInfo);
    if (emu_checkException(exceptionInfo)) { return; }
    if (i > 1) {
      boardAndChannelParam += ",";
    }
    boardAndChannelParam += "board" + channelDeviceParams["boardNumber"] + 
                            "/channel" + channelDeviceParams["channelNumber"];
  }
  boardAndChannelParam += "}";
  deviceParams["boardAndChannel"] = boardAndChannelParam;
  return deviceParams;
}

/** Given the ME11 (dubna) chamber deviceParams this function returns FSM nodes for HV channels. */
dyn_string emuui_getME11HVChannelFsmNodes(mapping deviceParams, dyn_string &exceptionInfo) {
  if ((deviceParams["station"] != 1) || deviceParams["ring"] != 1) {
    return makeDynString();
  }
  
  // ----------========== GET THE MAIN HV FSM NODE FOR THIS CHAMBER ==========----------
  string fsmNode = emuui_getFsmNode("chamber_me11_high_voltage", deviceParams, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  // ----------========== GET CHANNEL FSM NODES ==========----------
  dyn_int channelTypes;
  dyn_string channelFsmNodes = fwCU_getChildren(channelTypes, fsmNode);
  
  return channelFsmNodes;
}

/** Returns Peripheral Crate name in standard format (VME+-station/pcrateNum). */
string emuui_getPCrateName(string side, string station, string pcrateNum) {
  string sideSign = "+";
  if (side == "M") { sideSign = "-"; }
  
  return "VME" + sideSign + station + "/" + pcrateNum;
}

/** Returns Peripheral Crate name in standard format (VME+-station/pcrateNum) given the pcrate ID. */
string emuui_getPCrateNameId(string elmbId, dyn_string &exceptionInfo) {
  dyn_string info = emuui_getPcmbInfoById(elmbId, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  return emuui_getPCrateName(info[1], info[2], info[6]);
}

/** Returns chamber name (in a standard form e.g. ME+2/2/02) given deviceParams (it should contain at least "side", "station", "ring" and "chamberNumber" params). */
string emuui_getChamberName(mapping deviceParams) {
  string strSide = "+";
  if (deviceParams["side"] == "M") { strSide = "-"; }
  
  return "ME" + strSide + 
         deviceParams["station"] + "/" + deviceParams["ring"] + "/" +
         deviceParams["chamberNumber"];
}
