#uses "CMS_CSC_MiddleLayer/emu_hv.ctl"
#uses "CMS_CSC_common/emu_common.ctl"

string HVSIM_CMDTYPE_CMD = "HVCSCCMD";
string HVSIM_CMDTYPE_GETDATA = "HVCSCDATA";

int HVSIM_RAMP_MULTIPLIER = 20;
int HVSIM_DEFAULT_RAMP_UP_RATE = 10;
int HVSIM_DEFAULT_RAMP_DOWN_RATE = 100;

mapping channelIdToRampThread;
mapping chamberNameToFastMonDp;
mapping chamberNameToSlowMonDp;
mapping chamberNameToNumChans;

main()
{
  emu_info("HV simulator initializing");
  dyn_string ex;
  init(ex);
  if (emu_checkException(ex)) { return; }
  emu_info("HV simulator started");
}


void init(dyn_string &ex) {
  dyn_string hvDps = dpNames(EMUHV_DP_PREFIX + "*", "CscHvChamber");
  for (int i=1; i <= dynlen(hvDps); i++) {
    mapping chamber = emu_fsmNodeToDeviceParams(hvDps[i], ex);
    string chamberName = emu_getChamberName(chamber);
    if (emu_checkException(ex)) { return; }
    string slowMonDp = hvDps[i] + EMUHV_DP_POSTFIX_SLOW_MON;
    string fastMonDp = hvDps[i] + EMUHV_DP_POSTFIX_FAST_MON;
    chamberNameToFastMonDp[chamberName] = fastMonDp;
    chamberNameToSlowMonDp[chamberName] = slowMonDp;
    
    int numChans = ((chamber["ring"] == 2) && (chamber["station"] > 1)) ? 30 : 18;
    // some default settings
    dpSet(fastMonDp + ".num_chans", numChans,
          slowMonDp + ".num_chans", numChans,
          fastMonDp + ".interlock", 1,
          fastMonDp + ".pos5v", 3000,
          fastMonDp + ".neg5v", 3000);
    
    chamberNameToNumChans[chamberName] = numChans;
  }
  
  // start everything
  dyn_string commandDps = dpNames("*.command", "CscDimCommand");
  for (int i=1; i <= dynlen(commandDps); i++) {
    dpConnect("hvCommandCB", false, commandDps[i]);
  }
//  startThread("periodicUpdateThread");
}

void hvCommandCB(string dp, string hvCommandStr) {
  dyn_string dsTmp;

  string hostId, commandType;
  string chamberName;
  int channel, command, value;
  
  dsTmp = strsplit(hvCommandStr, "|");
  hostId = dsTmp[1];
  
  dyn_string params = strsplit(dsTmp[2], ";");
  
  commandType = params[1];
  chamberName = params[2];
  channel = params[3];
  command = params[4];
  value = params[5];
  
  emu_info("HVSIM: got command: " + hvCommandStr);
  
  if (commandType == HVSIM_CMDTYPE_CMD) {
    if (command == EMUHV_COMMAND_OFF) {
      cmdChannelOff(chamberName, channel);
    } else if (command == EMUHV_COMMAND_ON) {
      cmdChannelOn(chamberName, channel);
    } else if (command == EMUHV_COMMAND_SET_VSET) {
      cmdChannelVset(chamberName, channel, value);
    } else if (command == EMUHV_COMMAND_SET_IMAX) {
      cmdChannelImax(chamberName, channel, value);
    }
  } else if (commandType == HVSIM_CMDTYPE_GETDATA) {
    updateData(chamberName, channel);
  }
}

void cmdChannelOff(string chamberName, int channel) {
  dyn_int channels = getChannelNumbers(channel);
  emu_info("HVSIM: switching off channels " + channels + " on chamber " + chamberName);
  startNewRampThread(chamberName, channels, 0, 0);
}

void cmdChannelOn(string chamberName, int channel) {
  dyn_int channels = getChannelNumbers(channel);
  emu_info("HVSIM: switching on channels " + channels + " on chamber " + chamberName);
  startNewRampThread(chamberName, channels);
}

void cmdChannelVset(string chamberName, int channel, int targetVoltage) {
  dyn_int channels = getChannelNumbers(channel);
  emu_info("HVSIM: setting VSET=" + targetVoltage + " on channels " + channels + " on chamber " + chamberName);

  string slowMonDp = chamberNameToSlowMonDp[chamberName];
  for (int i=1; i <= dynlen(channels); i++) {
    int channelNum = channels[i];
    dpSet(slowMonDp + ".channels.ch" + channelNum + ".vset", targetVoltage);
  }
  dpSet(slowMonDp + ".num_chans", chamberNameToNumChans[chamberName]); // to trigger update in hvClient
  startThread("startNewRampThread", chamberName, channels);
}

void cmdChannelImax(string chamberName, int channel, int imax) {
  dyn_int channels = getChannelNumbers(channel);
  emu_info("HVSIM: setting IMAX=" + imax + " on channels " + channels + " on chamber " + chamberName + " (NOT IMPLEMENTED)");  
}

void updateData(string chamberName, int channel) {
  string fastMonDp = chamberNameToFastMonDp[chamberName];
  string slowMonDp = chamberNameToSlowMonDp[chamberName];

  emu_info("HVSIM: updating data on chamber " + chamberName);
  
  dpSet(fastMonDp + ".num_chans", emu_dpGetCached(fastMonDp + ".num_chans"));
  dpSet(slowMonDp + ".num_chans", emu_dpGetCached(slowMonDp + ".num_chans"));
}

dyn_int getChannelNumbers(int channel) {
  dyn_int channels;
  if (channel == 255) {
    for (int i=1; i <= 30; i++) {
      dynAppend(channels, i);
    }
  } else {
    dynAppend(channels, channel + 1);
  }
  
  return channels;
}

void startNewRampThread(string chamberName, dyn_int channels, int targetVoltage = -1, int statusAfterDone = 1) synchronized(channelIdToRampThread) {
  for (int i=1; i <= dynlen(channels); i++) {    
    string channelId = chamberName + ";ch" + channels[i];
    if (mappingHasKey(channelIdToRampThread, channelId) && (channelIdToRampThread[channelId] != -1)) {
      //emu_info("trying to stop thread with id = " + channelIdToRampThread[channelId]);
      stopThread(channelIdToRampThread[channelId]);
      channelIdToRampThread[channelId] = -1;
      delay(0, 50);
      if (channelIdToRampThread[channelId] != -1) {
        return;
      }
    }
  
    channelIdToRampThread[channelId] = startThread("rampThread", chamberName, channels[i], targetVoltage, statusAfterDone);
  }
}

void rampThread(string chamberName, int channelNum, int targetVoltage = -1, int statusAfterDone = 1) {
  emu_info("HVSIM: ramping channel " + channelNum + " on chamber " + chamberName + " to " + targetVoltage + ".");
  
  string fastMonDp = chamberNameToFastMonDp[chamberName];
  string slowMonDp = chamberNameToSlowMonDp[chamberName];
  string chFastMonDp = fastMonDp + ".channels.ch" + channelNum;
  string chSlowMonDp = slowMonDp + ".channels.ch" + channelNum;
  
  if (targetVoltage == -1) {
    targetVoltage = emu_dpGetCached(chSlowMonDp + ".vset");
  }
  
  // get ramping parameters
  int currentVoltage;
  dpGet(chFastMonDp + ".vmon", currentVoltage);
  int rampRate;
  if (currentVoltage > targetVoltage) { // ramp down
    dpSet(chFastMonDp + ".status", 3);
    rampRate = emu_dpGetCached(chSlowMonDp + ".ramp_down") * -1;
    if (rampRate == 0) {
      dpSet(chSlowMonDp + ".ramp_down", HVSIM_DEFAULT_RAMP_DOWN_RATE);
      rampRate = HVSIM_DEFAULT_RAMP_DOWN_RATE * -1;
    }
  } else {
    dpSet(chFastMonDp + ".status", 2);
    rampRate = emu_dpGetCached(chSlowMonDp + ".ramp_up");
    if (rampRate == 0) {
      dpSet(chSlowMonDp + ".ramp_up", HVSIM_DEFAULT_RAMP_UP_RATE);
      rampRate = HVSIM_DEFAULT_RAMP_UP_RATE;
    }
  }
  dpSet(chSlowMonDp + ".is_on", statusAfterDone);
  dpSet(slowMonDp + ".num_chans", chamberNameToNumChans[chamberName]); // to trigger update in hvClient  
  // ramp
  while((rampRate < 0) && (currentVoltage > targetVoltage) || (rampRate > 0) && (currentVoltage < targetVoltage)) {
    dpSet(chFastMonDp + ".vmon", currentVoltage);
    dpSet(fastMonDp + ".num_chans", chamberNameToNumChans[chamberName]); // to trigger update in hvClient
    currentVoltage += rampRate * HVSIM_RAMP_MULTIPLIER + (rand() % (2 * HVSIM_RAMP_MULTIPLIER));
    delay(1);
  }
  
  // finalize ramping
  dpSet(chFastMonDp + ".vmon", targetVoltage);
  dpSet(chFastMonDp + ".status", statusAfterDone);
  dpSet(fastMonDp + ".num_chans", chamberNameToNumChans[chamberName]); // to trigger update in hvClient

  string channelId = chamberName + ";ch" + channelNum;
  channelIdToRampThread[channelId] = -1;
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
