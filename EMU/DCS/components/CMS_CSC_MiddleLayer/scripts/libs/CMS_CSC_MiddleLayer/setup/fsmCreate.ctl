/**@file

This package contains functions responsible for creating the FSM tree.

@author Evaldas Juska (FNAL)
@date   January 2012
*/

#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_fsm.ctl"
#uses "CMS_CSC_common/emu_deviceInfo.ctl"
#uses "CMS_CSC_MiddleLayer/emu_hv.ctl"
#uses "CMS_CSC_MiddleLayer/emu_x2p.ctl"

// ************* FSM constants ****************************************
const string EMU_FSM_TYPE_SYSTEM = "EMUGrouping";
const string EMU_FSM_TYPE_TOP = "EMUGrouping";
const string EMU_FSM_TYPE_STATION = "EMUGrouping";
const string EMU_FSM_TYPE_TRIGGER_SECTOR = "EMUGrouping";
const string EMU_FSM_TYPE_CHAMBER_INNER = "EMUChamberInner";
const string EMU_FSM_TYPE_CHAMBER_OUTER = "EMUChamberOuter";
const string EMU_FSM_TYPE_FED = "EMUFed";
const string EMU_FSM_TYPE_HV = "EMUGrouping";
const string EMU_FSM_TYPE_HV_LV = "EMU_LV";

const string EMU_FSM_DOMAIN_ME11_HV = "CSC_ME_11_HV";
const string EMU_FSM_NODE_ME11_HV_PLUS = "CSC_ME_P11_HV";
const string EMU_FSM_NODE_ME11_HV_MINUS = "CSC_ME_N11_HV";

const string EMU_FSM_DOMAIN_LV_MRTN = "CMS_CSC_LV_CAN2";
const string EMU_FSM_DOMAIN_LV_CRB = "CMS_CSC_LV_CAN1";

const string EMU_FSM_NODE_ELMB_PSU_MINUS = "CSC_ME_LV_CR1_MINUS_PSU";
const string EMU_FSM_NODE_ELMB_PSU_PLUS = "CSC_ME_LV_CR1_PLUS_PSU";

/**
  * Creates Middle Layer FSM.
  */
void emuFsm_createMiddleLayerFsm(dyn_string &ex) {
  emu_info("Creating FSM");

  // ****************** create the top system node ******************
  string systemNode = getSystemName();
  strreplace(systemNode, ":", "");
  emuFsm_createFsmNode("FSM", systemNode, EMU_FSM_TYPE_SYSTEM, true);

  
  // ****************** Create stations FSM ******************

  // find out chambers that are present on this system
  dyn_mapping chambers;
  dyn_string lvDps = dpNames("*CSC_ME*", "CscLvChamber");
  for (int i=1; i <= dynlen(lvDps); i++) {
    dynAppend(chambers, emu_fsmNodeToDeviceParams(lvDps[i], ex));
    if (emu_checkException(ex)) { return; }
  }
  
  // create the stations FSM
  emuFsm_createStationsFsm(systemNode, chambers, ex);
  if (emu_checkException(ex)) { return; }
  
  
  // ****************** Create HV FSM ******************
  
  // find out HV primary PSUs and HV LV PSUs that are present on this system
  dyn_string primaryDpsPlus = dpNames("*Primary_600*", "CscHvPrimary");
  dyn_string primaryDpsMinus = dpNames("*Primary_500*", "CscHvPrimary");
  dyn_string hvLvDpsPlus = dpNames("*HV_LV_P*", "FwWienerMarathon");
  dyn_string hvLvDpsMinus = dpNames("*HV_LV_M*", "FwWienerMarathon");
  
  // create the HV FSM
  if ((dynlen(primaryDpsPlus) > 0) || (dynlen(hvLvDpsPlus) > 0)) {
    emuFsm_createHvFsm(systemNode, "P", primaryDpsPlus, hvLvDpsPlus, ex);
    if (emu_checkException(ex)) { return; }
  }
  if ((dynlen(primaryDpsMinus) > 0) || (dynlen(hvLvDpsMinus) > 0)) {
    emuFsm_createHvFsm(systemNode, "M", primaryDpsMinus, hvLvDpsMinus, ex);
    if (emu_checkException(ex)) { return; }
  }


  // ****************** Create FED FSM ******************
  dyn_string dduDps = dpNames("CscFed/DDU*", "CscFedDdu");
  emuFsm_createFedFsm(systemNode, dduDps);
  
  fwFsmTree_generateAll();
  fwFsmTree_refreshTree();
  
  emu_info("Done creating FSM");
}

/**
  * Creates FSM subtrees for each station.
  */
void emuFsm_createStationsFsm(string systemNode, dyn_mapping chambers, dyn_string &ex) {
  for (int i=1; i <= dynlen(chambers); i++) {
    mapping chamber = chambers[i];

    //create station
    string stationNode = "CSC_ME_" + chamber["side"] + chamber["station"];
    string stationName = "CSC ME" + 
                         ((chamber["side"] == "M") ? "-" : "+") + // translate P/M to +/-
                         chamber["station"];
    emuFsm_createFsmNode(systemNode, stationNode, EMU_FSM_TYPE_STATION, true);    
    fwFsmTree_setNodeLabel(stationNode, stationName);
    
    // import the Maraton and CRB nodes
    string crbNode = stationNode + "_LV_CRB";
    string maratonNode = stationNode + "_LV_MRTN";
    if (chamber["station"] == 3) { // special case for station 3 - use Maraton node for station 2 - it powers both ME2 and ME3 (on the same disk). I know.. doesn't look very nice.. mais c'est la vie..
      maratonNode = "CSC_ME_" + chamber["side"] + "2" + "_LV_MRTN";
    }

    emuFsm_createRefNode(maratonNode, stationNode, EMU_FSM_DOMAIN_LV_MRTN, false, ex);
    if (emu_checkException(ex)) { return; }
    emuFsm_createRefNode(crbNode, stationNode, EMU_FSM_DOMAIN_LV_CRB, false, ex);
    if (emu_checkException(ex)) { return; }
    
    // create trigger sector
    bool isSmallRing = ((chamber["station"] > 1) && (chamber["ring"] == 1));
    int trgSector;
    if (!isSmallRing) {
      trgSector = floor(((float)chamber["chamberNumber"] - 3) / 6.0) + 1;
    } else {
      trgSector = floor(((float)chamber["chamberNumber"] * 2 - 3) / 6.0) + 1;
    }
    if (trgSector == 0) { trgSector = 6; }
    string trgSectorNode = stationNode + "_TS_" + trgSector;
    string trgSectorName = "ME" + 
                         ((chamber["side"] == "M") ? "-" : "+") + // translate P/M to +/-
                         chamber["station"] +
                         " trigger sector " + trgSector;
    emuFsm_createFsmNode(stationNode, trgSectorNode, EMU_FSM_TYPE_TRIGGER_SECTOR, false);
    fwFsmTree_setNodeLabel(trgSectorNode, trgSectorName);
    
    // create chamber
    string chamberType = (chamber["ring"] == 1) ? EMU_FSM_TYPE_CHAMBER_INNER : EMU_FSM_TYPE_CHAMBER_OUTER;
    string chamberNode = "CSC_ME_" + chamber["side"] + chamber["station"] + chamber["ring"] + "_C" + chamber["chamberNumber"];
    string chamberName = emu_getChamberName(chamber);
    emuFsm_createFsmNode(trgSectorNode, chamberNode, chamberType, false);
    fwFsmTree_setNodeLabel(chamberNode, chamberName);
    
    // create chamber HV DU
    if (!emuDev_isMe11Chamber(chamber)) { // UF/PNPI HV system
      string hvDp = emuhv_getDp(chamber, "", ex);
      if (emu_checkException(ex)) { return; }
      emuFsm_createFsmNode(chamberNode, hvDp, EMUHV_DPT_HV_FSM, false);
      fwFsmTree_setNodeLabel(hvDp, chamberName + " HV");
    } else {  // CAEN HV system
      string me11HvNode = "CSC_ME_" +
                          ((chamber["side"] == "M") ? "N" : "P") + // translate M (minus) to ME1/1 notation i.e. N (negative)
                          chamber["station"] +
                          chamber["ring"] +
                          "_C" + chamber["chamberNumber"] +
                          "_HV";
      
      emuFsm_createRefNode(me11HvNode, chamberNode, EMU_FSM_DOMAIN_ME11_HV, false, ex);
      if (emu_checkException(ex)) { return; }
    }
    
    // create chamber LV DU
    string lvDp = emux2p_getLvDpName(chamber, ex);
    if (emu_checkException(ex)) { return; }
    emuFsm_createFsmNode(chamberNode, lvDp, EMU_X2P_DEVICE_FSM_DPT_LV, false);
    fwFsmTree_setNodeLabel(lvDp,  chamberName + " LV");

    // create chamber Temperature DU
    string tempDp = emux2p_getTempDpName(chamber, ex);
    if (emu_checkException(ex)) { return; }
    emuFsm_createFsmNode(chamberNode, tempDp, EMU_X2P_DEVICE_FSM_DPT_TEMP, false);
    fwFsmTree_setNodeLabel(tempDp,  chamberName + " Temperature");
  }
}

/**
  * Creates HV FSM subtree.
  * @param systemNode the system node
  * @param side endcap - can be either "M" or "P"
  * @param primaryDps list of HV Primary PSU DPs (for the given side only!)
  * @param hvLvDps list of HV LV PSU DPs (for the given side only!)
  */
void emuFsm_createHvFsm(string systemNode, string side, dyn_string primaryDps, dyn_string hvLvDps, dyn_string &ex) {
  string sideFull = (side == "M") ? "MINUS" : "PLUS"; // translate P/M to PLUS/MINUS
    
  // main HV node

  string hvNode = "CSC_HV_" + side;
  string hvName = "CSC HV " + sideFull;
    
  emuFsm_createFsmNode(systemNode, hvNode, EMU_FSM_TYPE_HV, true);
  fwFsmTree_setNodeLabel(hvNode, hvName);

  // UF/PNPI HV primary PSUs group node
    
  string hvPrimariesNode = hvNode + "_PRIMARIES";
  emuFsm_createFsmNode(hvNode, hvPrimariesNode, EMU_FSM_TYPE_HV, true);
  fwFsmTree_setNodeLabel(hvPrimariesNode, hvName + " Primaries");
    
  // UF/PNPI HV primary PSUs
    
  for (int i=1; i <= dynlen(primaryDps); i++) {
    string hvPrimaryDp = primaryDps[i];
    string nodeName = hvPrimaryDp;
    strreplace(nodeName, "CscHighVoltage/", "");
    emuFsm_createFsmNode(hvPrimariesNode, hvPrimaryDp, EMUHV_DPT_PRIMARY_FSM, false);
    fwFsmTree_setNodeLabel(hvPrimaryDp, nodeName);
  }
    
  // HV LV PSU
    
  for (int i=1; i <= dynlen(hvLvDps); i++) {
    string wienerDp = hvLvDps[i];
    if (dpExists(wienerDp)) {
      // create LV HV logical node
      string hvLvNode = "CSC_HV_LV_" + side;
      string hvLvName = "LV for HV " + sideFull;
      emuFsm_createFsmNode(hvNode, hvLvNode, EMU_FSM_TYPE_HV_LV, true);
      fwFsmTree_setNodeLabel(hvLvNode, hvLvName);
      
      // add the devices
      string wienerName = "Wiener PSU for HV " + sideFull;
      emuFsm_createFsmNode(hvLvNode, wienerDp, "FwWienerMarathon", false);
      fwFsmTree_setNodeLabel(wienerDp, wienerName);
      
      dyn_string channelDps = dpNames(wienerDp + "/*", "FwWienerMarathonChannel");
      for (int i=1; i <= dynlen(channelDps); i++) {
        string channelDp = channelDps[i];
        dyn_string channelDpSplit = strsplit(channelDp, "/");
        string channelName = channelDpSplit[3];
        strreplace(channelName, "Channel", "Channel ");
        
        emuFsm_createFsmNode(hvLvNode, channelDp, "FwWienerMarathonChannel", false);
        fwFsmTree_setNodeLabel(channelDp, channelName);
      }
    }      
  }
    
  // ME1/1 HV

  string me11HvNode = (side == "M") ? EMU_FSM_NODE_ME11_HV_MINUS : EMU_FSM_NODE_ME11_HV_PLUS;
        
  emuFsm_createRefNode(me11HvNode, hvNode, EMU_FSM_DOMAIN_ME11_HV, false, ex);
  if (emu_checkException(ex)) { return; }
}

/**
  * Creates FED FSM subtree.
  */
void emuFsm_createFedFsm(string systemNode, dyn_string dduDps) {
  
  for (int i=1; i <= dynlen(dduDps); i++) {
    // extract the ID and determine the endcap
    string dduDp = dduDps[i];
    int dduId = (int) substr(dduDp, strlen(dduDp) - 2, 2);    
    string side = (dduId <= 18) ? "P" : "M"; // plus side if DDU ID <= 18, otherwise it's minus side

    // create the FED node
    string fedNode = "CSC_FED_" + side;
    string fedName = "CSC FED " +
                     ((side == "M") ? "MINUS" : "PLUS"); // translate P/M to PLUS/MINUS
    emuFsm_createFsmNode(systemNode, fedNode, EMU_FSM_TYPE_FED, true);
    fwFsmTree_setNodeLabel(fedNode, fedName);

    // create the DDU node
    string dduLabel;
    sprintf(dduLabel, "DDU%.2d", dduId);
    emuFsm_createFsmNode(fedNode, dduDp, EMU_X2P_DEVICE_FSM_DPT_FED, false);
    fwFsmTree_setNodeLabel(dduDp, dduLabel);
  }
  
}
