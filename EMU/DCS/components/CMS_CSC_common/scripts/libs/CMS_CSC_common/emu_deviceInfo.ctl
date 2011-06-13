/**@file

This package contains functions to lookup parameters of various devices.

@author Evaldas Juska (PH/UCM)
@date   June 2009
*/

// Common libs
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_math.ctl"
#uses "CMS_CSC_common/emu_util.ctl"
#uses "CMS_CSC_common/emu_mapping.ctl"

/**
  * Returns chambers connected to the given Peripheral Crate.
  * @param pcrate peripheral crate device parameters (must have these parameters: side, station, crateNum)
  */
dyn_mapping emuDev_getPCrateChambers(mapping pcrate, dyn_string &exceptionInfo) {
  emuDev_checkDeviceParams("emuDev_getPCrateChambers", "PCrate", pcrate, makeDynString("side", "station", "crateNum"), exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return; }
  
  dyn_mapping ret;
  
  string side = pcrate["side"];
  int station = pcrate["station"];
  int crateNum = pcrate["crateNum"];
  
  // total number of crates on a station
  int totalCrates = 6;
  if (station == 1) {
    totalCrates = 12;
  }
  
  if (crateNum > totalCrates) {
    emu_addError("emuDev_getPCrateChambers(): invalid crateNum supplied. Supplied PCrate parameters: side=" + side + ", station=" + station + ", crateNum=" + crateNum, exceptionInfo);
    return;
  }
  
  int ringsCount = 2;
  if (station == 1) {
    ringsCount = 3;
  } else if (station == 4) {
    ringsCount = 1;
  }
  
  for (int ring=1; ring <= ringsCount; ring++) {
    int chamberNumShift;
    int countChambersToInclude;
    if (station == 1) { // on station 1, chamber numbers connected to the first PCrate start from 36, then go 1, 2
      chamberNumShift = -1;
      countChambersToInclude = 3;
    } else if (ring == 1) { // on all other stations station ring 1, chamber numbers connected to the first PCrate start from 2, then go 3, 4
      chamberNumShift = 1;
      countChambersToInclude = 3;
    } else if (ring == 2) { // on all other stations station ring 2, chamber numbers connected to the first PCrate start from 3, then go 4, 5, 6, 7, 8
      chamberNumShift = 2;
      countChambersToInclude = 6;
    }
    
    int startChamberNum = (1 + chamberNumShift) + ((crateNum - 1) * countChambersToInclude);
    int endChamberNum = (1 + chamberNumShift) + ((crateNum) * countChambersToInclude) - 1;
    int totalChambers = totalCrates * countChambersToInclude;
    
    for (int i = startChamberNum; i <= endChamberNum; i++) {
      int chamberNum = i;
      if (chamberNum <= 0) { chamberNum += totalChambers; } // wrap around the total number of available chambers
      if (chamberNum > totalChambers) { chamberNum -= totalChambers; } // wrap around the total number of available chambers
      
      mapping chamber;
      chamber["side"] = side;
      chamber["station"] = station;
      chamber["ring"] = ring;
      string chamberNumStr = (string) chamberNum;
      if (strlen(chamberNumStr) == 1) { chamberNumStr = "0" + chamberNumStr; }
      chamber["chamberNumber"] = chamberNumStr;
      
      dynAppend(ret, chamber);
    }
  }
  
  if ((side == "P") && (station == 4) && (crateNum == 2)) { // special case for the crate connected to the few ME+4/2 chambers
    for (int chamberNum=9; chamberNum <= 13; chamberNum++) {
      mapping chamber;
      chamber["side"] = side;
      chamber["station"] = station;
      chamber["ring"] = 2;
      string chamberNumStr = (string) chamberNum;
      if (strlen(chamberNumStr) == 1) { chamberNumStr = "0" + chamberNumStr; }
      chamber["chamberNumber"] = chamberNumStr;
      dynAppend(ret, chamber);
    }
  }
  
  return ret;
}

/**
  * Checks if needed device parameters are provided. If some of them are not provided, an exception is thrown.
  * @param funcName name of the function that got the device parameters that it wants to check.
  * @param deviceName name of the device that the parameters are for.
  * @param deviceParams device parameters that you want to check.
  * @param requiredParams list of required parameters.
  */
void emuDev_checkDeviceParams(string funcName, string deviceName, mapping deviceParams, dyn_string requiredParams, dyn_string &exceptionInfo) {
  for (int i=1; i <= dynlen(requiredParams); i++) {
    if (!mappingHasKey(deviceParams, requiredParams[i])) {
      emu_addError("Incorrect device parameters for " + deviceName + " passed to " + funcName + ". Required parameters are: " + requiredParams, exceptionInfo);
      return;
    }
  }
}

/**
  * @return all PCrates in the system.
  */
dyn_mapping emu_getAllPCrates() {
  dyn_mapping allCrates;
  
  for (int sideInt=0; sideInt <= 1; sideInt++) {
    string side = "P";
    if (sideInt == 1) { side = "M"; }
    
    for (int station=1; station <= 4; station++) {
      int totalCrates = 6;
      if (station == 1) { totalCrates = 12; }
      
      for (int crateNum=1; crateNum <= totalCrates; crateNum++) {
        mapping pcrate;
        pcrate["side"] = side;
        pcrate["station"] = station;
        pcrate["crateNum"] = crateNum;
        dynAppend(allCrates, pcrate);
      }
    }
  }
  
  return allCrates;
}

/**
  * @param side endcap - "M" or "P"
  * @param station station number
  * @return deviceParams of all chambers in the given station
  */
dyn_mapping emuDev_getAllChambersForStation(string side, int station) {
  dyn_mapping ret;
  int ringCount = (station == 1) ? 3 : 2;

  for (int ring=1; ring <= ringCount; ring++) {
    int chamberStartIdx = 1, 
        chamberEndIdx = 36;
    
    // determine the number of chambers in this ring
    if ((station != 1) && (ring == 1)) { // inner, non ME1/1 rings - 18 chambers
      chamberEndIdx = 18;
    } else if ((station == 4) && (ring == 2)) { // ME4/2
      if (side == "P") {      // ME+4/2 <-- chambers 9 through 13
        chamberStartIdx = 9;
        chamberEndIdx = 13;
      } else {                // ME-4/2 <-- no chambers here
        chamberEndIdx = 0;
      }
    }
    
    for (int chamberNum=chamberStartIdx; chamberNum <= chamberEndIdx; chamberNum++) {
      mapping deviceParams;
      
      deviceParams["side"] = side;
      deviceParams["station"] = station;
      deviceParams["ring"] = ring;
      string chamberNumStr;
      sprintf(chamberNumStr, "%.2d", chamberNum);
      deviceParams["chamberNumber"] = chamberNumStr;
      
      dynAppend(ret, deviceParams);
    }
  }
  
  return ret;
}
