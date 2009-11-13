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
    case "UFPNPI_HV":
      return emumaj_ufpnpiStateCounts(values, all, calcTotal, node);
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
  DebugTN("majStates: " + majStates);
  DebugTN("majPercentages: " + mapPercentages);
  // majStates and mapPercentages contain a map from device:state to the majority states or to the percentages
     if (    ( majStates["UFPNPI_HV:error"]   >= majority_ON  ) ) return "ERROR";
     else if ( majStates["UFPNPI_HV:on"]      >= majority_ON    ) return "ON";
     else if ( majStates["UFPNPI_HV:on"]      == majority_MIXED ) return "MIXED";
     else                                                       return "OFF";

     // example -> if more than the defined percentage of channels are in error then the state is ERROR
     //            otherwise if more than the defined percentage of channels is on then ON
     //            otherwise MIXED or OFF

     // for the meaning of majority_ON, majority_MIXED etc. see library majorityLib.ctl or http://lomasett.web.cern.ch/lomasett/treeCache_Majority/
}

/*
//     Examples of advanced configuration
//     ==================================

// Two additional user functions can be defined to modify the behaviour of the majority
// Normally you do not need to define them

      
// This function can be used to connect to the FSM state of the DUs (and not to the related data point)
// in this case you have to specify makeDynString(".fsm.currentState") in the list of the elements (during the configuration)
string majorityUser_nodeTranslation(string node) {
     return treeCache_getFsmInternalDp(node);
     // if you want to distinguish between different types you can get the FSM type with treeCache_getType(node);
     // if you want to get the name of the related data point (this is the default if this function is not defined) use
     // return treeCache_getFsmDevDp(node);
}



// This function allows you to decide to which data point element the majority should connect 
// (independently from the configuration) to compute the state of one device
// use it only if you are sure that you need it 

// @param dev device type (as in the majority config)
// @param node node name 
// @param use_it set it to true if you want to use the value that you return otherwise it will use the default
// @return list of data point elements to connect to compute the state of node (the length of the array should match the configuration of the device)
//         the elements can belong to different data points (this is a case when this function may be needed) but should all belong to the same PVSS system
//         (in principle not necessarily to the system where the majority script is running, even if this is reccomended)
dyn_string majorityUser_nodeTranslationToDpes(string dev, string node, bool& use_it) {
   use_it = false;
   if (dev == "...") {
      use_it = true;
      return makeDynString(....);             
   }
   return makeDynString(); // if use_it is false you can return any value, it will not be taken into account  
}



*/
