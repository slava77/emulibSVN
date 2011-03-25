#uses "CMS_CSCfw_HV_CC/emu_hvCommon.ctl"
#uses "CMS_CSC_common/emu_common.ctl"

string HVSIM_CMDTYPE_CMD = "HVCMD";
string HVSIM_CMDTYPE_GETDATA = "HVDATA";

int HVSIM_RAMP_MULTIPLIER = 300;//5;

mapping moduleIdToDps;

mapping channelIdToRampThread;


main()
{
  emu_info("HV simulator initializing");
  init();
  emu_info("HV simulator started");
}


void init() {
  dyn_string coordsDps = dpNames("HighVoltage/CSC_ME_*_HV.coord", "HV_1");
  for (int i=1; i <= dynlen(coordsDps); i++) {
    string coords;
    dpGet(coordsDps[i], coords);
    string hostSlotAddress = substr(coords, 0, strlen(coords) - 2);
    string fsmDp = dpSubStr(coordsDps[i], DPSUB_DP);
    string dp = substr(fsmDp, strpos(fsmDp, "CSC_ME"));

    if (!mappingHasKey(moduleIdToDps, hostSlotAddress)) {
      moduleIdToDps[hostSlotAddress] = makeDynString();
    }
    
    int part = strsplit(coords, ";")[4];
    if (part == 0) { part++; }
    dynInsertAt(moduleIdToDps[hostSlotAddress], dp, part);
  }

  // start everything  
  dpConnect("hvCommandCB", false, "HV_1_COM.command");
  startThread("periodicUpdateThread");
}

void hvCommandCB(string dp, string hvCommandStr) {
  dyn_string dsTmp;

  string hostId, commandType;
  int slot, address, channel, command, value;
  
  dsTmp = strsplit(hvCommandStr, "|");
  hostId = dsTmp[1];
  
  dyn_string params = strsplit(dsTmp[2], ";");
  
  commandType = params[1];
  slot = params[2];
  address = params[3];
  channel = params[4];
  command = params[5];
  value = params[6];
  
  string moduleId = hostId + ";" + slot + ";" + address;
  
  emu_info("HVSIM: got command: " + hvCommandStr);
  
  if (commandType == HVSIM_CMDTYPE_CMD) {
    if (command == EMUHV_COMMAND_OFF) {
      cmdChannelOff(moduleId, channel);
    } else if (command == EMUHV_COMMAND_ON) {
      cmdChannelOn(moduleId, channel);
    } else if (command == EMUHV_COMMAND_VSET) {
      cmdChannelVset(moduleId, channel, value);
    }
  } else if (commandType == HVSIM_CMDTYPE_GETDATA) {
    updateData(moduleId, channel);
  }
}

void cmdChannelOff(string moduleId, int channel) {
  dyn_int channels = getChannelNumbers(channel);
  emu_info("HVSIM: switching off channels " + channels + " on module " + moduleId);
  startNewRampThread(moduleId, channels, 0, 0);
}

void cmdChannelOn(string moduleId, int channel) {
  dyn_int channels = getChannelNumbers(channel);
  emu_info("HVSIM: switching on channels " + channels + " on module " + moduleId);
  startNewRampThread(moduleId, channels);
}

void cmdChannelVset(string moduleId, int channel, int targetVoltage) {
  dyn_int channels = getChannelNumbers(channel);
  emu_info("HVSIM: setting VSET=" + targetVoltage + " on channels " + channels + " on module " + moduleId);

  dyn_string chamberDps;
  emu_dynAppend(chamberDps, moduleIdToDps[moduleId]);
  for (int i=1; i <= dynlen(chamberDps); i++) {
    string chamberDp = chamberDps[i];
    
    for (int i=1; i <= dynlen(channels); i++) {
      int channelNum = channels[i];
      
      setPackedData(chamberDp, channelNum, "vset", targetVoltage);
    }
  }
  
  startNewRampThread(moduleId, channels);
}

void updateData(string moduleId, int channel) {
  dyn_string chamberDps;
  emu_dynAppend(chamberDps, moduleIdToDps[moduleId]);

  emu_info("HVSIM: updating data on module " + moduleId);
  
  for (int i=1; i <= dynlen(chamberDps); i++) {
    dpSet(chamberDps[i] + ".update_value", 1);
  }
}

dyn_int getChannelNumbers(int channel) {
  dyn_int channels;
  if (channel == 253) {
    for (int i=1; i <= 18; i++) {
      dynAppend(channels, i);
    }
  } else if (channel == 254) {
    for (int i=19; i <= 36; i++) {
      dynAppend(channels, i);
    }
  } else if (channel == 255) {
    for (int i=1; i <= 36; i++) {
      dynAppend(channels, i);
    }
  } else {
    dynAppend(channels, channel + 1);
  }
  
  return channels;
}

void startNewRampThread(string moduleId, dyn_int channels, int targetVoltage = -1, int statusAfterDone = 1) {
  for (int i=1; i <= dynlen(channels); i++) {    
    string channelId = moduleId + ";ch" + channels[i];
    if (mappingHasKey(channelIdToRampThread, channelId) && (channelIdToRampThread[channelId] != -1)) {
      stopThread(channelIdToRampThread[channelId]);
      channelIdToRampThread[channelId] = -1;
    }
  
    channelIdToRampThread[channelId] = startThread("rampThread", moduleId, channels[i], targetVoltage, statusAfterDone);
  }
}

void rampThread(string moduleId, int channelNum, int targetVoltage = -1, int statusAfterDone = 1) {
  dyn_string chamberDps;
  emu_dynAppend(chamberDps, moduleIdToDps[moduleId]);

  emu_info("HVSIM: ramping channel " + channelNum + " on module " + moduleId + " to " + targetVoltage + ". (chamberDps = " + chamberDps + ")");
  
  if (targetVoltage == -1) {
    targetVoltage = getPackedData(chamberDps[1], channelNum, "vset");
  }
  
  // get ramping parameters
  int currentVoltage;
  dpGet(chamberDps[1] + ".data.v" + channelNum + ".vmon", currentVoltage);
  int rampRate;
  if (currentVoltage > targetVoltage) { // ramp down
    dpSetForAllChambers(chamberDps, ".data.v" + channelNum + ".state", statusAfterDone);
    dpSetForAllChambers(chamberDps, ".data.v" + channelNum + ".status", 3);
    rampRate = getPackedData(chamberDps[1], channelNum, "ramp_down") * -1;
  } else {
    dpSetForAllChambers(chamberDps, ".data.v" + channelNum + ".state", statusAfterDone);
    dpSetForAllChambers(chamberDps, ".data.v" + channelNum + ".status", 2);
    rampRate = getPackedData(chamberDps[1], channelNum, "ramp_up");
  }
  
  // ramp
  while((rampRate < 0) && (currentVoltage > targetVoltage) || (rampRate > 0) && (currentVoltage < targetVoltage)) {
    dpSetForAllChambers(chamberDps, ".data.v" + channelNum + ".vmon", currentVoltage);
    currentVoltage += rampRate * HVSIM_RAMP_MULTIPLIER + (rand() % (2 * HVSIM_RAMP_MULTIPLIER));
    delay(1);
  }
  
  // finalize ramping
  dpSetForAllChambers(chamberDps, ".data.v" + channelNum + ".vmon", targetVoltage);
      
  dpSetForAllChambers(chamberDps, ".data.v" + channelNum + ".state", statusAfterDone);
  dpSetForAllChambers(chamberDps, ".data.v" + channelNum + ".status", statusAfterDone);
    
//  dpSetForAllChambers(chamberDps, ".update_value", 2);
}

private void dpSetForAllChambers(dyn_string chamberDps, string dpe, anytype value) {
  for (int i=1; i <= dynlen(chamberDps); i++) {
    dpSet(chamberDps[i] + dpe, value);
  }
}

int getPackedData(string chamberDp, int channelNum, string dpe) {
  int packedDataNum = ceil((float)channelNum / 2);
  int data;
  dpGet(chamberDp + ".data.pack_data.v" + packedDataNum + "." + dpe, data);
  int data1 = data & 0x0FFFF;
  int data2 = (data >> 16) & 0x0FFFF;
  if (((float) channelNum / 2) < packedDataNum) {
    return data1;
  } else {
    return data2;
  }
}

void setPackedData(string chamberDp, int channelNum, string dpe, int value) {
  int packedDataNum = ceil((float)channelNum / 2);
  int data;
  dpGet(chamberDp + ".data.pack_data.v" + packedDataNum + "." + dpe, data);
  int data1 = data & 0x0FFFF;
  int data2 = (data >> 16) & 0x0FFFF;
  if (((float) channelNum / 2) < packedDataNum) {
    data1 = value;
  } else {
    data2 = value;
  }
  data = data1 + (data2 << 16);
  dpSetWait(chamberDp + ".data.pack_data.v" + packedDataNum + "." + dpe, data);
}

void periodicUpdateThread() {
  dyn_string chamberDps = dpNames("CSC_ME_*_HV", "HV_1_d");
    
  while(true) {
    emu_info("HVSIM: doing periodic update");
    for (int i=1; i <= dynlen(chamberDps); i++) {
      dpSet(chamberDps[i] + ".update_value", 1);
    }
    delay(600); // every 10 minutes
  }
}
