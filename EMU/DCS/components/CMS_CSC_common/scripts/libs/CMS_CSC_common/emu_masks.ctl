/**@file

This package contains functions to read all hardware masks (HV, LV, temperature, DDU and FSM) and save them to a DP of a given system 
and also same thing with loading the masks back to the system.

@author Evaldas Juska (PH/UCM)
@date   July 2010
*/

#uses "CMS_CSC_common/emu_common.ctl"

/** gets all masks from the given source systems and saves them to the given target system. */
void emumask_getAndSaveAllMasks(dyn_string sourceSystems = makeDynString(), string targetSystem = "") {
  emu_info("collecting and saving masks...");

  string sourceSysPattern = "";
  if (dynlen(sourceSystems) > 0) {
    sourceSysPattern = "{";
    for (int i=1; i <= dynlen(sourceSystems); i++) {
      if (i > 1) {
        sourceSysPattern += ",";
      }
      sourceSysPattern += sourceSystems[i];
    }
    sourceSysPattern += "}";
  }
  
  emumask_getAndSaveChannelMasks(sourceSysPattern, targetSystem);
  emumask_getAndSaveFsmMasks(sourceSysPattern, targetSystem);
      
  emu_info("done saving masks");
}

/** loads all masks from the given source system and saves them to any system indicated in a mask, provided that this sys is reachable. */ 
void emumask_loadAndApplyAllMasks(string sourceSystem = "") {
  emu_info("loading and applying masks");
  
  emumask_loadAndApplyChannelMasks(sourceSystem);
  emumask_loadAndApplyFsmMasks(sourceSystem);
      
  emu_info("done loading and applying masks");
}

/** gets all channel masks from the given source systems and saves them to the given target system. */
void emumask_getAndSaveChannelMasks(string sourceSysPattern = "", string targetSystem = "") {
  dyn_string channelMasksDynStr;
  mapping lvMasks = emumask_getChannelMasks("LV_1", sourceSysPattern);
  mapping tempMasks = emumask_getChannelMasks("TEMP_1", sourceSysPattern);
  mapping fedMasks = emumask_getChannelMasks("FED_1", sourceSysPattern);
  mapping hvMasks = emumask_getChannelMasks("HV_1", sourceSysPattern);
  
  emu_dynAppend(channelMasksDynStr, emumask_maskMappingToDynString(lvMasks));
  emu_dynAppend(channelMasksDynStr, emumask_maskMappingToDynString(tempMasks));
  emu_dynAppend(channelMasksDynStr, emumask_maskMappingToDynString(fedMasks));
  emu_dynAppend(channelMasksDynStr, emumask_maskMappingToDynString(hvMasks));
  
  dpSetWait(targetSystem + "emu_masks.channelMasks", channelMasksDynStr);
}

/** gets all FSM masks from the given source systems and saves them to the given target system. */
void emumask_getAndSaveFsmMasks(string sourceSysPattern = "", string targetSystem = "") {
  dyn_string fsmMasks;
  dyn_string fsmMaskDps;
  dyn_string fsmObjectMaskDps = dpNames(sourceSysPattern + "*.mode.enabled", "_FwFsmObject");
  dyn_string fsmDeviceMaskDps = dpNames(sourceSysPattern + "*.mode.enabled", "_FwFsmDevice");
  emu_dynAppend(fsmMaskDps, fsmObjectMaskDps);
  emu_dynAppend(fsmMaskDps, fsmDeviceMaskDps);
  for (int i=1; i <= dynlen(fsmMaskDps); i++) {
    int isEnabled;
    dpGet(fsmMaskDps[i], isEnabled);
    if (isEnabled == 0) {
      dynAppend(fsmMasks, fsmMaskDps[i]);
    }
  }
  dpSetWait(targetSystem + "emu_masks.fsmMasks", fsmMasks);
}

/** loads all channel masks from the given source system and saves them to any system indicated in a mask, provided that this sys is reachable. */ 
void emumask_loadAndApplyChannelMasks(string sourceSystem = "") {
  dyn_string channelMasksDynStr;
  dpGet(sourceSystem + "emu_masks.channelMasks", channelMasksDynStr);
  mapping masks = emumask_maskDynStringToMapping(channelMasksDynStr);
  for (int i=1; i <= mappinglen(masks); i++) {
    string dp = mappingGetKey(masks, i);
    if (dpExists(dp)) {
      dpSetWait(dp, masks[dp]);
    } else {
      emu_info("couldn't find DP (perhaps the system is not connected): " + dp);
    }
  }
}

/** loads all FSM masks from the given source system and saves them to any system indicated in a mask, provided that this sys is reachable. */ 
void emumask_loadAndApplyFsmMasks(string sourceSystem = "") {
  dyn_string fsmMasks;
  dpGet(sourceSystem + "emu_masks.fsmMasks", fsmMasks);
  for (int i=1; i <= dynlen(fsmMasks); i++) {
    dpSetWait(fsmMasks[i], 0);
  }
}
  
/** Returns the masks of the given dpType (currently supported are LV_1, TEMP_1, FED_1, HV_1). Return is a map of dp -> array of masked channel numbers */
mapping emumask_getChannelMasks(string dpType, string sysPattern="") {
  mapping ret;
  dyn_string dps = dpNames(sysPattern + "*", dpType);
  for (int i=1; i <= dynlen(dps); i++) {
    string dp = dps[i];
    if (dpType == "LV_1") { 
      dp += ".noalert_channels"; // hmm :)
    } else {
      dp += ".off_channels";
    }
    dyn_int maskedChannels;
    dpGet(dp, maskedChannels);
    if (dynlen(maskedChannels) > 0) {
      ret[dp] = maskedChannels;
    }
  }
  return ret;
}

/** Converts a "DP -> array of masked channel numbers" to a string array like this: DP;ch1,ch2,ch3.. */
dyn_string emumask_maskMappingToDynString(mapping masks) {
  dyn_string ret;
  for (int i=1; i <= mappinglen(masks); i++) {
    string dp = mappingGetKey(masks, i);
    dyn_string channels = mappingGetValue(masks, i);
    string maskStr = dp + ";";
    for (int j=1; j <= dynlen(channels); j++) {
      if (j > 1) { maskStr += ","; }
      maskStr += channels[j];
    }
    dynAppend(ret, maskStr);
  }
  return ret;
}

/** Converts a string array like this: DP;ch1,ch2,ch3.." to a mapping DP -> array of masked channel numbers" */
mapping emumask_maskDynStringToMapping(dyn_string maskDynStr) {
  mapping ret;
  for (int i=1; i <= dynlen(maskDynStr); i++) {
    dyn_string tmpSplit = strsplit(maskDynStr[i], ";");
    string dp = tmpSplit[1];
    dyn_int channels = strsplit(tmpSplit[2], ",");
    ret[dp] = channels;
  }
  return ret;
}
