#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_MiddleLayer/emu_hv.ctl"
#uses "majority_treeCache/treeCache.ctl"
#uses "majority_treeCache/majorityLib.ctl"
#uses "CMS_CSC_majority/CMS_CSC_majorityDevices.ctl"

/*
  @device  the device type as it was defined in the configuration
  @values  the values of the datapoint elements you defined to be connected to for this device type
  @all this is the total count that may be modified in this function (to take into account partial excluding of complex device units)
  @calcTotal tells if the function is called to compute the total (regardless the inclusion state) or the included devices 
  @node this is the node name in the FSM that might be used to retrieve some additional info
  @return array of integers of state counts of the defined states for this device type
 */

dyn_int majorityUser_stateCounts(string device, dyn_anytype values, // information needed to calculate the state counts
				 int&all, // all may be modified in this function
				 bool calcTotal, // is count for total statistics being calculated? (if not: included nodes statistics)
				 string node) { // node name: might me useful. all tree information can be retreived using node and treeCache lib.
     
  // values are returned in the same order you defined in the majority_addDevice function. 

  switch (device) {
    case "HV_OUTER":
      return emumaj_hvStateCounts(values, all, calcTotal, node, device);
    case "HV_INNER":
      return emumaj_hvStateCounts(values, all, calcTotal, node, device);
    case "HV_ME11":
      return emumaj_onOffStandbyErrorFsmStateCounts(values, all, calcTotal, node, device);
    case "HV_Primary":
      return emumaj_onOffStandbyErrorFsmStateCounts(values, all, calcTotal, node, device);
    case "LV":
      return emumaj_lvStateCounts(values, all, calcTotal, node);
    case "TEMP":
      return emumaj_temperatureStateCounts(values, all, calcTotal, node);
    case "CRB":
      return emumaj_onOffErrorFsmStateCounts(values, all, calcTotal, node);
    case "MrtnChannel":
      return emumaj_onOffErrorFsmStateCounts(values, all, calcTotal, node);
    case "MrtnCrate":
      return emumaj_onOffErrorFsmStateCounts(values, all, calcTotal, node);
    case "AtlasPSU_Branch":
      return emumaj_onOffErrorFsmStateCounts(values, all, calcTotal, node);
    case "DDU":
      return emumaj_onOffErrorFsmStateCounts(values, all, calcTotal, node);
    case "Gas":
      return emumaj_onOffErrorFsmStateCounts(values, all, calcTotal, node);
    case "Cooling":
      return emumaj_onOffErrorFsmStateCounts(values, all, calcTotal, node);
    default:
      break;
  }
}


// translates fsm device dp to the dp, where the configured dpes are located.
// if your dpes are part of the fw fsm dev dps (normal case), leave this function unchanged
string majorityUser_dpTranslation(string fsmDevDp) {
     return fsmDevDp;
}

// majStates: contains mapping with fulfill status of devices:states
// mapPercentages: contains mapping with exact percentages (should not be needed normally)
string majorityUser_calcFsmState(mapping majStates,mapping mapPercentages,string node) {

  // OFF state
  if (emumaj_allOFF(majStates, true,
                    makeDynString("HV_OUTER:on",
                                  "HV_OUTER:standby",
                                  "HV_OUTER:error",
                                  "HV_INNER:on",
                                  "HV_INNER:standby",
                                  "HV_INNER:error",
                                  "HV_ME11:on",
                                  "HV_ME11:standby",
                                  "HV_ME11:error",
                                  "HV_Primary:on",
                                  "HV_Primary:standby",
                                  "HV_Primary:error",
                                  "CRB:on",
                                  "CRB:error",
                                  "MrtnChannel:on",
                                  "MrtnChannel:error",
                                  "MrtnCrate:on",
                                  "MrtnCrate:error",
                                  "LV:error",
                                  "TEMP:alert"),
                    true) || // at least one of those must exist (but don't care about the devices listed below), if none of those exist - see below.
      (!emumaj_anyExist(majStates,
                    makeDynString("HV_OUTER:on",
                                  "HV_INNER:on",
                                  "HV_ME11:on",
                                  "HV_Primary:on",
                                  "CRB:on",
                                  "MrtnChannel:on",
                                  "MrtnCrate:on")) &&
       emumaj_allOFF(majStates, true,
                    makeDynString("LV:on",
                                  "DDU:on",
                                  "DDU:error",
                                  "AtlasPSU_Branch:on",
                                  "AtlasPSU_Branch:error",
                                  "Cooling:on",
                                  "Cooling:error",                         
                                  "Gas:on",
                                  "Gas:error")))) {
    return "OFF";
    
  // ERROR state
  } else if (emumaj_anyON(majStates,
                   makeDynString("HV_OUTER:error",
                                 "HV_INNER:error",
                                 "HV_ME11:error",
                                 "HV_Primary:error",
                                 "LV:error",
                                 "TEMP:alert",
                                 "CRB:error",
                                 "MrtnChannel:error",
                                 "MrtnCrate:error",
                                 "DDU:error",
                                 "AtlasPSU_Branch:error",
                                 "Cooling:error",
                                 "Gas:error"))) {
    return "ERROR";
    
  // ON state
  } else if (emumaj_allON(majStates, true,
                   makeDynString("HV_OUTER:on",
                                 "HV_INNER:on",
                                 "HV_ME11:on",
                                 "HV_Primary:on",
                                 "LV:on",
                                 "TEMP:ok",
                                 "CRB:on",
                                 "MrtnChannel:on",
                                 "MrtnCrate:on",
                                 "DDU:on",
                                 "AtlasPSU_Branch:on",
                                 "Cooling:on",
                                 "Gas:on"))) {
    return "ON";

  // OUTER_ON state
  } else if ((majStates["HV_OUTER:on"] >= majority_ON) && // HV_OUTER must exist and be ON
             ((majStates["HV_INNER:on"] != majority_TOTALZERO) || (majStates["HV_ME11:on"] != majority_TOTALZERO)) && // and at least one of either HV_INNER or HV_ME11 must exist
             emumaj_allOFF(majStates, true, // but in any case if they exist (HV_INNER or HV_ME11), they must NOT be ON
                   makeDynString("HV_INNER:on",
                                 "HV_ME11:on")) &&
             emumaj_allON(majStates, true,
                   makeDynString("HV_Primary:on",
                                 "LV:on",
                                 "TEMP:ok",
                                 "CRB:on",
                                 "MrtnChannel:on",
                                 "MrtnCrate:on",
                                 "DDU:on",
                                 "AtlasPSU_Branch:on",
                                 "Cooling:on",
                                 "Gas:on"))){
    return "OUTER_ON";

  // STANDBY state    
  } else if (emumaj_allON(majStates, true,
                   makeDynString("HV_OUTER:standby",
                                 "HV_INNER:standby",
                                 "HV_ME11:standby",
                                 "HV_Primary:standby",
                                 "LV:on",
                                 "TEMP:ok",
                                 "CRB:on",
                                 "MrtnChannel:on",
                                 "MrtnCrate:on",
                                 "DDU:on",
                                 "AtlasPSU_Branch:on",
                                 "Cooling:on",
                                 "Gas:on")) &&
             emumaj_allOFF(majStates, true,
                   makeDynString("HV_OUTER:on",
                                 "HV_INNER:on",
                                 "HV_ME11:on",
                                 "HV_Primary:on"))) {
    return "STANDBY";
    
  // NOT-READY state
  } else {
    return "NOT-READY";
  }
}


//     Examples of advanced configuration
//     ==================================

// Two additional user functions can be defined to modify the behaviour of the majority
// Normally you do not need to define them

// This function can be used to connect to the FSM state of the DUs (and not to the related data point)
// in this case you have to specify makeDynString(".fsm.currentState") in the list of the elements (during the configuration)
string majorityUser_nodeTranslation(string node) {
  string type = treeCache_getType(node);
  if ((type == "fwCrb_CSC_LV") || 
      (type == "FwWienerMarathonChannel") || 
      (type == "FwWienerMarathon") || 
      (type == "fwBranch_CSC_LV") ||
      (type == "CscFedDdu") ||
      (type == "CscHvPrimary") ||
      (type == "FwCaenChannel") ||
      (type == "CscGasSystem_CSC_GAS") ||
      (type == "CscCooling")) {
    return treeCache_getFsmInternalDp(node);
  } else {
    return treeCache_getFsmDevDp(node);
  }
     
     // if you want to distinguish between different types you can get the FSM type with treeCache_getType(node);
     // if you want to get the name of the related data point (this is the default if this function is not defined) use
     // return treeCache_getFsmDevDp(node);
}

/*
// This function allows you to decide to which data point element the majority should connect 
// (independently from the configuration) to compute the state of one device
// use it only if you are sure that you need it 

// @param dev device type (as in the majority config)
// @param node node name 
// @param use_it set it to true if you want to use the value that you return otherwise it will use the default
// @return list of data point elements to connect to compute the state of node (the length of the array should match the configuration of the device)
//         the elements can belong to different data points (this is a case when this function may be needed) but should all belong to the same PVSS system
//         (in principle not necessarily to the system where the majority script is running, even if this is reccomended)
  */
dyn_string majorityUser_nodeTranslationToDpes(string dev, string node, bool& use_it) {
   use_it = false;
   return makeDynString(); // if use_it is false you can return any value, it will not be taken into account  
}

/**
  * @return weight of devices which have a variable weight so have been set a weight of -1 during majority configuration (e.g. HV)
  */
int majorityUser_getDeviceWeight(string node, string device) {
  emu_debug("got majorityUser_getDeviceWeight() call for device = " + device + ", node = " + node);
  if (!dpExists(node)) {
    dyn_string nodeDps = dpNames("*:" + node);
    if (dynlen(nodeDps) == 0) {
      emu_errorSingle("got majorityUser_getDeviceWeight() call for device = " + device + ", node = " + node + ", but couldn't find a dp for the given node");
      exit(-1);
    }
    node = nodeDps[1];
  }
  
  dyn_string ex;
  mapping deviceParams = emu_fsmNodeToDeviceParams(node, ex);
  if (emu_checkException(ex)) { return 0; }
  
  if ((device == "HV_OUTER") || (device == "HV_INNER")) {               // *********** High Voltage ************
    string hvType = (deviceParams["ring"] == 1) ? "HV_INNER" : "HV_OUTER";
    if (device != hvType) {
      return 0;
    }
    int numChannels;
    dpGet(dpSubStr(node, DPSUB_SYS_DP) + EMUHV_DP_POSTFIX_SLOW_MON + ".num_chans", numChannels);
    if (numChannels == 0) { // special case (when HV distribution board is not connected or something): set it to default number just to be able to report error
      numChannels = ((deviceParams["station"] > 1) && (deviceParams["ring"] == 2)) ? 30 : 18;
    }
    return numChannels;
  } else if (device == "LV") {                                         // *********** Low Voltage ************
    if ((deviceParams["station"] == 1) && (deviceParams["ring"] == 3)) {
      return 16; // ME1/3 chambers have 4 CFEBs, all other chambers have 5
    } else {
      return 19;
    }
  } else if (device == "TEMP") {                                      // *********** Temperature ************
    if ((deviceParams["station"] == 1) && (deviceParams["ring"] == 3)) {
      return 6; // ME1/3 chambers have 4 CFEBs, all other chambers have 5
    } else {
      return 7;
    }
  }
  
  return 0;
}

/**/

/** 
  Parameter majStates is a mapping of "device:state" to majority range (you get this mapping in function majorityUser_calcFsmState).
  Parameter checkStates is a list of majStates keys to be checked i.e. "deviceName:stateName".
  This function checks if ALL of the device states in checkStates are 0% - returns true if it's true and false if it's false.
  If parameter okIfDoesntExist is set to true then each of the device states in checkStates can either not exist or be 0%.
  If parameter atLeastOneMustExist is set to true then it is additionally required that at least one of the device states must exist for fuction to return true.
  atLeastOneMustExist parameter can only be used with okIfDoesntExist = true.
*/
bool emumaj_allOFF(mapping majStates, bool okIfDoesntExist, dyn_string checkStates, bool atLeastOneMustExist = false) {
  int notExistsCounter = 0;
  for (int i=1; i <= dynlen(checkStates); i++) {
    string deviceState = checkStates[i];
    if (majStates[deviceState] != majority_OFF) {
      if (!okIfDoesntExist) {
        return false;
      } else {
        if (majStates[deviceState] != majority_TOTALZERO) {
          return false;
        } else if (atLeastOneMustExist) {
          notExistsCounter++;
        }
      }
    }
  }

  if (okIfDoesntExist && atLeastOneMustExist && (notExistsCounter >= dynlen(checkStates))) {
    return false;
  }  
  
  return true;
}

/** 
  Parameter majStates is a mapping of "device:state" to majority range (you get this mapping in function majorityUser_calcFsmState).
  Parameter checkStates is a list of majStates keys to be checked i.e. "deviceName:stateName".
  This function checks if ALL of the device states in checkStates are more than threshold% - returns true if it's true and false if it's false.
  If parameter okIfDoesntExist is set to true then each of the device states in checkStates can either not exist or be more than threshold%.
*/
bool emumaj_allON(mapping majStates, bool okIfDoesntExist, dyn_string checkStates) {
  for (int i=1; i <= dynlen(checkStates); i++) {
    string deviceState = checkStates[i];
    if (majStates[deviceState] < majority_ON) {
      if (!okIfDoesntExist) {
        return false;
      } else {
        if (majStates[deviceState] != majority_TOTALZERO) {
          return false;
        }
      }
    }
  }
  
  return true;
}

/** 
  Parameter majStates is a mapping of "device:state" to majority range (you get this mapping in function majorityUser_calcFsmState).
  Parameter checkStates is a list of majStates keys to be checked i.e. "deviceName:stateName".
  This function checks if ANY of the device states in checkStates exist i.e. != majority_TOTALZERO.
*/
bool emumaj_anyExist(mapping majStates, dyn_string checkStates) {
  for (int i=1; i <= dynlen(checkStates); i++) {
    string deviceState = checkStates[i];
    if (majStates[deviceState] != majority_TOTALZERO) {
      return true;
    }
  }
  
  return false;
}

/** 
  Parameter majStates is a mapping of "device:state" to majority range (you get this mapping in function majorityUser_calcFsmState).
  Parameter checkStates is a list of majStates keys to be checked i.e. "deviceName:stateName".
  This function checks if ANY of the device states in checkStates are more than threshold% - returns true if it's true and false if it's false.
*/
bool emumaj_anyON(mapping majStates, dyn_string checkStates) {
  for (int i=1; i <= dynlen(checkStates); i++) {
    string deviceState = checkStates[i];
    if (majStates[deviceState] >= majority_ON) {
      return true;
    }
  }
  
  return false;
}
