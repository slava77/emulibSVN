/**@name LIBRARY: fwElmb.ctl
  
This library contains function associated with the ELMB component.
Functions are provided for creating and deleting DPs of this type.
Also, templates for functions to configure this device are supplied.

Creation Date: 18.06.2003

Modification History: 14.06.2004 fwElmb_create renamed to fwElmb_createNode
                                  for easier searching
                      06.02.2006 Addition of fwElmb_aiChannelFilter dedicated
                                  to handle the analog input channels available
                                  due to the process being more complicated

External function list: fwElmb_channelFilter
                        fwElmb_aiChannelFilter
                        fwElmb_getFwName
                        fwElmb_getChannelName
                        fwElmb_checkSensorPrefix
                        fwElmb_getRawSensors
                        fwElmb_getUserDefinedFct
                        fwElmb_getAvailableNodeIdList
                        fwElmb_getAvailableBitList
                        fwElmb_setValuesToOPCGroup
                        fwElmb_setValuesToCANbus
                        fwElmb_setValuesToElmb
                        fwElmb_createCANbus
                        fwElmb_createNode (Check usage)
                        fwElmb_createConfig
                        fwElmb_createChannel
                        fwElmb_elementSQ
                        fwElmb_monitorElmbStateCbk
                        fwElmb_monitorElmbEMCbk
                        fwElmb_monitorOpcCbk
                        fwElmb_activatePortAMasks
      
Constraints: None

Usage: Public

PVSS manager usage: VISION, CTRL

@author Fernando Varela (EP-ATI)
*/

//*****************************************************************************
// @name Function: fwElmb_channelFilter
//
// Returns a dynamic array of channels still available for the Elmb Node given.
// At present, this only works correctly if ALL channels assigned are of the
// Elmb specific Analog/Digital input/output type.
//
// @param argsDpName: DP name of Elmb to get list of non-allocated channels
//                    for.
// @param argsDpType: DP type name for channel/port type to filter for
// @param argdsAvailableChannels: List of available ports/channels are
//                                returned here
// @param argdsExceptionInfo: Details of any exceptions are returned here
// @param argsPDOdp: Optional argument giving the PDO DP to check for. If
//                   not given, the default PDO is assumed. This value is
//                   (currently) only checked for analog inputs
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Fernando Varela
//*****************************************************************************
void fwElmb_channelFilter(string argsDpName,
                          string argsDpType,
                          dyn_string &argdsAvailableChannels,
                          dyn_string &argdsExceptionInfo,
                          string argsPDOdp = "",
                          bool argbFilter = true)
{
// Local Variables
// ---------------
  int i;
  
  string sElement;
  
  dyn_string dsChildren;
  dyn_string dsChannels;
  dyn_string dsAllocatedChannelIDs;
  dyn_string dsTypes;
  dyn_string dsTemp;

  const dyn_string ELMB_DI_CHANNELS = makeDynString("F;0", "F;1", "F;2", "F;3", "F;4", "F;5", "F;6", "F;7",
                                                    "A;0", "A;1", "A;2", "A;3", "A;4", "A;5", "A;6", "A;7",
                                                    "D;0", "D;1", "D;2", "D;3", "D;4", "D;5", "D;6", "D;7",
                                                    "E;0", "E;1", "E;2", "E;3", "E;4", "E;5", "E;6", "E;7");
  const dyn_string ELMB_DO_CHANNELS = makeDynString("C;0", "C;1", "C;2", "C;3", "C;4", "C;5", "C;6", "C;7",
                                                    "A;0", "A;1", "A;2", "A;3", "A;4", "A;5", "A;6", "A;7");
  const dyn_string ELMB_AI_SDO_CHANNELS = makeDynString("0","1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16",
                                                        "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32",
                                                        "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48",
                                                        "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "63");
  const dyn_string ELMB_AO_CHANNELS = makeDynString("0","1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16",
                                                    "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32",
                                                    "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48",
                                                    "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "63");

// Executable Code
// ---------------
  // Set some base information
  if (argsDpType == ELMB_AI_TYPE_NAME) {
    // Easier to handle these with dedicated function due to complications
    fwElmb_aiChannelFilter(argsDpName, argdsAvailableChannels, argdsExceptionInfo, argsPDOdp, argbFilter);
    return;
  } else if (argsDpType == ELMB_AI_SDO_TYPE_NAME) {
    sElement = ".channel";
    argdsAvailableChannels = ELMB_AI_SDO_CHANNELS;
    dsTypes = makeDynString(ELMB_AI_SDO_TYPE_NAME);
  } else if (argsDpType == ELMB_AO_TYPE_NAME) {
    sElement = ".channel";
    argdsAvailableChannels = ELMB_AO_CHANNELS;
    dsTypes = makeDynString(ELMB_AO_TYPE_NAME);
  } else if ((argsDpType == ELMB_DI_TYPE_NAME) || (argsDpType == ELMB_DO_TYPE_NAME)) {
    sElement = ".id";
    dsTypes = makeDynString(ELMB_DI_TYPE_NAME, ELMB_DO_TYPE_NAME);
    if (argsDpType == ELMB_DI_TYPE_NAME)
      argdsAvailableChannels = ELMB_DI_CHANNELS;
    else
      argdsAvailableChannels = ELMB_DO_CHANNELS;
  } else {
    fwException_raise(argdsExceptionInfo,
                      "ERROR",
                      "fwElmb_channelFilter(): Must filter using Elmb specific channel type",
                      "");
    return;
  }
  
  // Obtain all children of the given Elmb Config(s)
  dynClear(dsChildren);
  for (i = 1; i <= dynlen(dsTypes); i++) {
    dsTemp = dpNames(argsDpName + "*", dsTypes[i]);  
    dynAppend(dsChildren, dsTemp);
  }

  // Loop through all children, finding the allocated IDs for the correct PDO type
  for (i = 1; i <= dynlen(dsChildren); i++) {
    dpGet(dsChildren[i] + sElement, dsChannels);
    dynAppend(dsAllocatedChannelIDs, dsChannels);
  }
  
  // Loop through all allocated channels, removing them from the list
  for (i = 1; i <= dynlen(dsAllocatedChannelIDs); i++)
    dynRemove(argdsAvailableChannels, dynContains(argdsAvailableChannels, dsAllocatedChannelIDs[i]));
    
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_aiChannelFilter
//
// Returns a dynamic array of analog input channels still available for the
// Elmb Node given.
//
// @param argsDpName: DP name of Elmb to get list of non-allocated channels
//                    for.
// @param argdsAvailableChannels: List of available channels are returned here
// @param argdsExceptionInfo: Details of any exceptions are returned here
// @param argsPDOdp: Optional argument giving the PDO DP to check for. If
//                   not given, the default PDO is assumed.
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Jim Cook
//*****************************************************************************
void fwElmb_aiChannelFilter(string argsDpName,
                            dyn_string &argdsAvailableChannels,
                            dyn_string &argdsExceptionInfo,
                            string argsPDOdp = "",
                            bool argbFilter = true)
{
// Local Variables
// ---------------
  bool bMuxAll;
  
  int i;
  int iMaxChannels;
  int iChannelNumber;
  
  string sProfile;
  string sTemp;
  
  dyn_string dsChildren;
  dyn_string dsChannels;
  dyn_string dsAllocatedChannelIDs;
  dyn_string dsTypes;
  dyn_string dsPDOdps;
  dyn_string dsTemp;

  const dyn_string ELMB_AI_CHANNELS = makeDynString("0","1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16",
                                                    "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32",
                                                    "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48",
                                                    "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63");

// Executable Code
// ---------------
  // Check which PDO DP to check for
  if ((argsPDOdp == "") || (argsPDOdp == ELMB_NO_INFO)) {
    argsPDOdp = ELMB_NO_INFO;
    argdsAvailableChannels = ELMB_AI_CHANNELS;
  } else {
    // Get required information for this PDO
    dpGet(argsPDOdp + ".profile", sProfile,
          argsPDOdp + ".mux.timesMuxed", iMaxChannels,
          argsPDOdp + ".mux.all", bMuxAll,
          argsPDOdp + ".mux.number", iChannelNumber);
    if ((sProfile == "404") || (sProfile == "LMB")) {
      if (bMuxAll) {
        for (i = 0; i < iMaxChannels; i++)
          dynAppend(argdsAvailableChannels, i);
      } else {
        // Only a single channel is available
        argdsAvailableChannels = makeDynString(iChannelNumber);
      }
    } else {
      // Channel number does not apply to profile 401
      argdsAvailableChannels = makeDynString(ELMB_CHANNEL_NA);
    }
  }
  
  if (argbFilter) {
    // Get all sensors using this PDO
    dpGet(ELMB_SENSOR_INFO_NAME + ".types", dsTemp,
          ELMB_SENSOR_INFO_NAME + ".pdoDp", dsPDOdps);
    for (i = 1; i <= dynlen(dsPDOdps); i++) {
      if (dsPDOdps[i] == argsPDOdp)
        dynAppend(dsTypes, dsTemp[i]);
  }
  
  // Obtain all children of the given Elmb Config(s) using any of the relevant sensors
  dynClear(dsChildren);
  dsTemp = dpNames(argsDpName + "*", ELMB_AI_TYPE_NAME);
  for (i = 1; i <= dynlen(dsTemp); i++) {
    dpGet(dsTemp[i] + ".type", sTemp);
    if (dynContains(dsTypes, sTemp) > 0)
      dynAppend(dsChildren, dsTemp[i]);
  }

  // Loop through all children, finding the allocated IDs for the correct PDO type
  for (i = 1; i <= dynlen(dsChildren); i++) {
    dpGet(dsChildren[i] + ".channel", dsChannels);
    dynAppend(dsAllocatedChannelIDs, dsChannels);
  }
  
  // Loop through all allocated channels, removing them from the list
  for (i = 1; i <= dynlen(dsAllocatedChannelIDs); i++)
    dynRemove(argdsAvailableChannels, dynContains(argdsAvailableChannels, dsAllocatedChannelIDs[i]));
  }
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_getFwName
//
// Returns a name which takes into account the FW naming convention for the HW
// devices
//
// @param argsCANbusName: To be done...
//
// Modification History: None
//
// Constraints: None
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Fernando Varela
//*****************************************************************************
void fwElmb_getFwName(string argsUserName,
                      string argsDpParentName,
                      string &argsDpName)
{
// Local Variables
// ---------------
// None
  
// Executable Code
// ---------------
  // Create framework name
  argsDpName = argsDpParentName + fwDevice_HIERARCHY_SEPARATOR + argsUserName;
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_getChannelName
//
// Returns the fully qualified framework path name for an Elmb channel. The
// channel can be analog or digital, input or output.
//
// @param argsUserName: Lowest level part of name (string) for the channel. This
//                      must not include the framework 'path'
// @param argsParentName: Name of datapoint that the channel will be created
//                        under. This datapoint must be of either Elmb node type
//                        or the relevant configuration type (i.e. A/D i/p)
// @param argsChannelType: Datapoint type name of channel to get full name for
// @param argsChannelName: The fully qualified framework path name for the
//                          channel requested
// @param argdsExceptionInfo: Details of any exceptions are returned here
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Fernando Varela
//*****************************************************************************
void fwElmb_getChannelName(string argsUserName,
                           string argsParentName,
                           string argsChannelType,
                           string &argsChannelName,
                           dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  string sConfigSeparator;
  string sParentType;
  
  dyn_string dsTmp;
    
// Executable Code
// ---------------
  // Get parent type
  sParentType = dpTypeName(argsParentName);

  // Check which channel type is being created
  if (argsChannelType == ELMB_AI_TYPE_NAME) {
    sConfigSeparator = ELMB_AI_CONFIG_NAME;
  } else if (argsChannelType == ELMB_AI_SDO_TYPE_NAME) {
    sConfigSeparator = ELMB_AI_CONFIG_NAME;
  } else if (argsChannelType == ELMB_AO_TYPE_NAME) {
    sConfigSeparator = ELMB_AO_CONFIG_NAME;
  } else if (argsChannelType == ELMB_DI_TYPE_NAME) {
    sConfigSeparator = ELMB_DI_CONFIG_NAME;
  } else if (argsChannelType == ELMB_DO_TYPE_NAME) {
    sConfigSeparator = ELMB_DO_CONFIG_NAME;
  } else {
    fwException_raise(argdsExceptionInfo,
          "ERROR",
          "fwElmb_getChannelName(): Unknown channel type requested, \"" + argsChannelType,
          "");
    return;
  }
  
  if (ELMB_TYPE_NAME == sParentType) {
    argsChannelName = argsParentName + fwDevice_HIERARCHY_SEPARATOR +
                      sConfigSeparator + fwDevice_HIERARCHY_SEPARATOR +
                      argsUserName;
  } else if ((ELMB_AI_CONFIG_TYPE_NAME == sParentType) ||
             (ELMB_AO_CONFIG_TYPE_NAME == sParentType) ||
             (ELMB_DI_CONFIG_TYPE_NAME == sParentType) ||
             (ELMB_DO_CONFIG_TYPE_NAME == sParentType)) {
    argsChannelName = argsParentName + fwDevice_HIERARCHY_SEPARATOR +
                      argsUserName;
  } else {
    fwException_raise(argdsExceptionInfo,
          "ERROR",
          "fwElmb_getChannelName(): Parent given is invalid for this function, \"" + argsParentName,
          "");
  }

  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_checkSensorPrefix
//
// Returns whether the sensor prefix given is valid or not, taking into account
// whether the text given has already been defined for a raw sensor (which
// means that an OPC item will be defined with that name, and therefore a
// conflict would occur).
//
// @param sSensorName: Prefix of sensor to check
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Jim Cook
//*****************************************************************************
bool fwElmb_checkSensorPrefix(string sSensorName)
{
// Local Variables
// ---------------
  bool bValid = false;
  
  dyn_string dsPDOdps;
  dyn_string dsPDONames;
  
// Executable Code
// ---------------
  // Get all raw value sensor PDOs
  fwElmb_getRawSensors(dsPDONames, dsPDOdps);
  
  // Check for reserved name
  if (dynContains(dsPDONames, sSensorName) == 0)
    bValid = true;
  
  // Return to calling routine
  return (bValid);
}

//*****************************************************************************
// @name Function: fwElmb_getRawSensors
//
// Returns the names and corresponding informational DPs for any raw sensors
// defined in the system. The raw sensors are basically custom defined PDOs.
//
// @param dsRawSensorNames: The names of the raw sensors are returned here
// @param dsPDOInfoDps: The Info DPs for the raw sensors are returned here
// @param sDirection: (Optional) Can be "IN", "OUT" or "BOTH". If not "BOTH"
//                    then only raw sensors for the given direction are returned
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Jim Cook
//*****************************************************************************
void fwElmb_getRawSensors(dyn_string &dsRawSensorNames,
                          dyn_string &dsPDOInfoDps,
                          string sDirection = "BOTH")
{
// Local Variables
// ---------------
  int i;
  
  string sName;
  string sDir;
  
  dyn_string dsInPDOs;
  dyn_string dsOutPDOs;
  dyn_string dsInNames;
  dyn_string dsOutNames;
  dyn_string dsPDOdps;
  
// Executable Code
// ---------------
  // Get all raw value sensor PDOs
  dsPDOdps = dpNames("*", ELMB_PDO_INFO_TYPE_NAME);
    
  // Get the variables for return
  dynClear(dsRawSensorNames);
  dynClear(dsPDOInfoDps);
  
  // Loop through all raw sensors defined
  for (i = 1; i <= dynlen(dsPDOdps); i++) {
    dpGet(dsPDOdps[i] + ".name", sName,
          dsPDOdps[i] + ".direction", sDir);
    if (sDir == "IN") {
      dynAppend(dsInNames, sName);
      dynAppend(dsInPDOs, dsPDOdps[i]);
    } else if (sDir == "OUT") {
      dynAppend(dsOutNames, sName);
      dynAppend(dsOutPDOs, dsPDOdps[i]);
    }
  }
  
  // Depending on requested operation, setup return values
  if (sDirection != "OUT") {
    // Assume either "IN" or "BOTH" requested, so add inputs with default at start
    dynAppend(dsRawSensorNames, ELMB_AI_PREFIX);
    dynAppend(dsPDOInfoDps, ELMB_NO_INFO);  // There isn't a DP for the default
    dynAppend(dsRawSensorNames, dsInNames);
    dynAppend(dsPDOInfoDps, dsInPDOs);
  }
  
  if (sDirection != "IN") {
    // Assume either "OUT" or "BOTH" requested, so add outputs with default at start
    dynAppend(dsRawSensorNames, ELMB_AO_PREFIX);
    dynAppend(dsPDOInfoDps, ELMB_NO_INFO);  // There isn't a DP for the default
    dynAppend(dsRawSensorNames, dsOutNames);
    dynAppend(dsPDOInfoDps, dsOutPDOs);
  }
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_getUserDefinedFct
//
// Returns the function for an ELMB sensor from the function template, the list
// of ELMB channels and the parameters
//
// @param argsFctTemplate: Function template
// @param argdsChannelNumber: List of ELMB channels used in the function template
// @param argdfParameters: list of parameters used in the function template
// @param argsFct: Returned function
// @param argdsExceptionInfo: Exception handler
// @param argbOnlyChannels: Flag to indicate whether to only substitute channels
//                          and not parameters (Optional, default is false)
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Fernando Varela
//*****************************************************************************
void fwElmb_getUserDefinedFct(  string argsFctTemplate, 
                               dyn_string argdsChannelNumber, 
                               dyn_float argdfParameters, 
                               string &argsFct, 
                               dyn_string &argdsExceptionInfo,
                               bool argbOnlyChannels = false)
{
// Local Variables
// ---------------
  int i;
  
  string sChannelToReplace;
  string sParameterToReplace;
  string sParameterFormatted;
  
  dyn_string dsTemp;

// Executable Code
// ---------------
  argsFct = argsFctTemplate;
  
  // Find out how many channels were are using.
  for (i = 1; i <= dynlen(argdsChannelNumber); i++) {
    sChannelToReplace = "%c" + i;
  
    if (!patternMatch("*" + sChannelToReplace + "*", argsFctTemplate)) {
      fwException_raise(argdsExceptionInfo,
                        "ERROR",
                        "Inconsistency: The number of channels given do\nnot match the formula template. Aborting action...",
                        "");
      return;
    }
  
    // Enter the actual channel values into the function
    strreplace(argsFct, sChannelToReplace, argdsChannelNumber[i]);
  }

  if (!argbOnlyChannels) {
    for(i = 1; i <= dynlen(argdfParameters); i++) {
      sParameterToReplace = "%x" + i;
    
      if (!patternMatch("*" + sParameterToReplace + "*", argsFctTemplate)) {
        fwException_raise(argdsExceptionInfo,
                          "ERROR",
                          "Inconsistency: The number of parameters given do\nnot match the formula template. Aborting action...",
                          "");
        return;
      }
      sParameterFormatted = argdfParameters[i];
      
      // Check for 'e' notation
      dsTemp = strsplit(sParameterFormatted, "e");
      if (dynlen(dsTemp) < 2)
        dsTemp = strsplit(sParameterFormatted, "E");
        
      if (dynlen(dsTemp) > 1) {
        // Check for decimal point
        if (strpos(dsTemp[1], ".") < 0)
          sParameterFormatted = dsTemp[1] + ".e" + dsTemp[2];
      }
    
      // Enter the actual parameter values into the function
      strreplace(argsFct, sParameterToReplace, sParameterFormatted);
    }
    
    // Remove any and all spaces
    strreplace(argsFct, " ", "");
    
    // Replace any double minus with plus
    strreplace(argsFct, "--", "+");
    
    // Replace any double plus with plus
    strreplace(argsFct, "++", "+");
    
    // Replace any plus/minus with minus
    strreplace(argsFct, "+-", "-");
    
    // Replace any minus/plus with minus
    strreplace(argsFct, "-+", "-");
  }
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_getAvailableNodeIdList
//
// Returns available node IDs for a given bus in last argument.
//
// @param argsCANbusDpName: DP name of CAN bus datapoint, string
// @param argdsAvailableNodeIds: Node IDs available are returned here, dyn_string
// @param argbStandardElmb: (Optional) States whether to limit node IDs to <= 63
//                          or to allow max for 7-bit CANopen (default is to limit to 63)
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Fernando Varela
//*****************************************************************************
void fwElmb_getAvailableNodeIdList(  string argsCANbusDpName,
                                    dyn_string &argdsAvailableNodeIds,
                                    bool argbStandardElmb = true)
{
// Local Variables
// ---------------
  int i;
  int iMaxNodeId = 63;
  
  string sElement = ".id";
  string sTemp;
  
  dyn_string dsChildren;
  dyn_string dsAllocatedNodeIds;
                                          
// Executable Code
// ---------------
  // Clear any existing information
  dynClear(argdsAvailableNodeIds);
  
  // Get all children
  dsChildren = dpNames($sDpName + "*" + sElement , ELMB_TYPE_NAME);
  
  // Check if any children exist
  if (dynlen(dsChildren) > 0) {
    // Get all allocated node IDs
    dpGet(dsChildren, dsAllocatedNodeIds);
  }
  
  // Check if standard ELMB node IDs are to be used
  if (!argbStandardElmb)
    iMaxNodeId = 127;  // Otherwise use max allowed for 7-bit CANopen
  
  // Create array of remaining node IDs
  for (i = 1; i <= iMaxNodeId; i++) {
    sprintf(sTemp, "%d", i);
    if (dynContains(dsAllocatedNodeIds, sTemp) == 0)
      dynAppend(argdsAvailableNodeIds, sTemp);
  }
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_getAvailableBitList
//
// Gets the list of all available bits in a given port for a particular ELMB
// in a bus
//
// @param argsCANbusDpName: ...to be done
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Fernando Varela
//*****************************************************************************
fwElmb_getAvailableBitList(  string argsElmbName,
                            string argsChannelType,
                            string argsDigitalPort,
                            dyn_string &argdsBitList,
                            dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  int i;

  dyn_string dsChannelList;
  dyn_string dsExceptionInfo;
  dyn_string dsTmp;

// Executable Code
// ---------------
  // Initialization of arrays:
  dynClear(argdsBitList);

  // Obtain all available children of the parent datapoint, i.e. all ELMB channel configs
  fwElmb_channelFilter(  argsElmbName,
                        argsChannelType,
                        dsChannelList,
                        dsExceptionInfo);
               
  for (i = 1; i <= dynlen(dsChannelList); i++) {
    
    // We assume that the ids are of the type "port;bit", e.g. A;3
     // First we check that we are dealing with the right ELMB port
    if (patternMatch(argsDigitalPort + ";*", dsChannelList[i])) {
    
      // If it is the good port, we split the id to extract the bit number
      dsTmp = strsplit(dsChannelList[i], ";");
      
      // Add this bit number to the available bit number list
      dynAppend(argdsBitList, dsTmp[2]);
    }
  } //End of loop for i over the channel list

  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_createOPCGroup
//
//  
//*****************************************************************************
void fwElmb_createOPCGroup(string argsForType,
                           string argsBus,
                           dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  dyn_bool dbOPCGroupDevice;
  
  dyn_string dsOPCGroups;

// Executable Code
// ---------------
  // Get which type is given, and select the correct OPC groups to create
  if (argsForType == ELMB_CAN_BUS_TYPE_NAME) {
    dsOPCGroups = makeDynString("_Cmd_" + argsBus, "_Value_" + argsBus);
    dbOPCGroupDevice = makeDynBool(true, true);
  } else if (argsForType == ELMB_AI_TYPE_NAME ) {
    dsOPCGroups = makeDynString("_Ai_" + argsBus);
    dbOPCGroupDevice = makeDynBool(false);
  } else if (argsForType == ELMB_AI_SDO_TYPE_NAME) {
    dsOPCGroups = makeDynString("_AiSdo_" + argsBus);
    dbOPCGroupDevice = makeDynBool(true);
  } else if (argsForType == ELMB_AO_TYPE_NAME) {
    dsOPCGroups = makeDynString("_Ao_" + argsBus);
    dbOPCGroupDevice = makeDynBool(true);
  } else if (argsForType == ELMB_DI_TYPE_NAME) {
    dsOPCGroups = makeDynString("_Di_" + argsBus);
    dbOPCGroupDevice = makeDynBool(false);
  } else if (argsForType == ELMB_DO_TYPE_NAME) {
    dsOPCGroups = makeDynString("_Do_" + argsBus);
    dbOPCGroupDevice = makeDynBool(true);
  }
  
  if (dynlen(dsOPCGroups) > 0) {
    for (int i = 1; i <= dynlen(dsOPCGroups); i++) {
      if (!dpExists(dsOPCGroups[i])) {
        dpCreate(dsOPCGroups[i], "_OPCGroup");
        fwElmb_setValuesToOPCGroup(dsOPCGroups[i], dbOPCGroupDevice[i], argdsExceptionInfo);
      }
    }
  }

  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_setValuesToOPCGroup
//
//  
//*****************************************************************************
void fwElmb_setValuesToOPCGroup(string argsGroup,
                                bool argbDevice,
                                dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  int iOPCUpdateRate = 1000;
  int iErr;

  float fDeadBand = 0.0;

// Executable Code
// ---------------
  // Set default values to the OPC group given
  iErr = dpSet(argsGroup + ".Active", true,
               argsGroup + ".DeathBand", fDeadBand,
               argsGroup + ".DataSourceDevice", argbDevice,
               argsGroup + ".EnableCallback", true,
               argsGroup + ".UpdateRateReq", iOPCUpdateRate);
  
  // Check for success
  if (iErr == -1) {
    fwException_raise(argdsExceptionInfo,
                      "ERROR",
                      "_fwElmb_setValuesToOPCGroup(): Problem setting values to the OPC group: " + argsGroup,
                      "");
  }

  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_setValuesToCANbus
//
// Sets initial values to the data points for a given CANbus
//
// @param argsCANbusName: Name (string) of the ELMB CAN bus to be configured
// @param argsComment: A comment/description (string) for the CAN bus
// @param argsInterfacePort: Name (string) of the interface port used
// @param argsInterfaceCard: Name (string) of the interface card type
// @param argiBusSpeed: Bus Speed in bits/s (int)
// @param argiManagement: Value (int) for bus management (NMT)
// @param argiSyncInterval: Value (int) of SYNC Interval
// @param argiNodeGuardInterval: Value (int) of Node Guard Interval
// @param argdsExceptionInfo: Dynamic string containing any and all exceptions
//                            that may have occurred during execution
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Fernando Varela
//*****************************************************************************
void fwElmb_setValuesToCANbus(string argsCANbusName,
                              string argsComment,
                              string argsInterfacePort,
                              string argsInterfaceCard,
                              int argiBusSpeed,
                              int argiManagement,
                              int argiSyncInterval,
                              int argiNodeGuardInterval,
                              dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  string sDpeInterfaceCard = ".id";
  string sDpeInterfacePort = ".interfacePort";
  string sDpeBusSpeed = ".busSpeed";
  string sDpeManagement = ".management";
  string sDpeSyncInterval = ".syncInterval";
  string sDpeNodeGuardInterval = ".nodeGuardInterval";

// Executable Code
// ---------------
  // Check if dp does not exist
  if (!dpExists(argsCANbusName)) {
    fwException_raise(argdsExceptionInfo,
              "ERROR", "fwElmb_setValuesToCANbus(): The data point \"" + argsCANbusName + "\" does not exist",
              "");
    return;
  }
  
  // If no problem, set values to dpes
  dpSet(argsCANbusName + sDpeInterfacePort, argsInterfacePort,
        argsCANbusName + sDpeInterfaceCard, argsInterfaceCard,
        argsCANbusName + sDpeBusSpeed, argiBusSpeed,
        argsCANbusName + sDpeManagement, argiManagement,
        argsCANbusName + sDpeSyncInterval, argiSyncInterval,
        argsCANbusName + sDpeNodeGuardInterval, argiNodeGuardInterval);

  // Check for correct dpname
  if (!patternMatch("*.*", argsCANbusName) || !patternMatch("*.", argsCANbusName))
    argsCANbusName = argsCANbusName + ".";

  // Set the comment
  dpSetComment(argsCANbusName, argsComment);

  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_setValuesToElmb
//
// Sets initial values to the data points for a given ELMB
// NOTE:  Only the node Id and the management dpe are set. It does not set
//        configuration for the ADC, DAC or any other
//
// @param argsElmbName: Name (string) of the ELMB to be configured
// @param argsComment: A comment/description (string) for the ELMB
// @param argsNodeId: Value (string) containing the nodeId in decimal
// @param argdsExceptionInfo: Dynamic string containing all exceptions occurred during execution
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Fernando Varela
//*****************************************************************************
void fwElmb_setValuesToElmb(string argsElmbName,
                            string argsComment,
                            string argsNodeId,
                            dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
// None
  
// Executable Code
// ---------------
  // Check if dp does not exist
  if (!dpExists(argsElmbName)) {
    fwException_raise(argdsExceptionInfo,
              "ERROR", "fwElmb_setValuesToElmb(): The data point \"" + argsElmbName + "\" does not exist",
              "");
    return;
  }
  
  // If no problem, set values to dpes
  dpSet(argsElmbName + ".id", argsNodeId);
        
  // Check for correct dp name
  if (!patternMatch("*.*", argsElmbName) || !patternMatch("*.", argsElmbName))
    argsElmbName = argsElmbName + ".";

  // Set the comment
  dpSetComment(argsElmbName, argsComment);

  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_createCANbus
//
// Creates a DP for an Elmb/CAN Bus
//
// @param argsDpName: A name for the new data point. System name not required
// @param argdsExceptionInfo: Details of any exceptions are returned here
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Fernando Varela
//*****************************************************************************
void fwElmb_createCANbus(string argsDpName,
                         dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  string sSystemName;
  string sDpName;
  
  dyn_string dsTemp;

// Executable Code
// ---------------
  // Check new dp name is not empty
  if (argsDpName == "") {
    fwException_raise(argdsExceptionInfo,
          "ERROR",
          "fwElmb_createCANBus(): The data point name must not be empty",
          "");
    return;
  }

  // Check dp does not already exist
  if (dpExists(argsDpName)) {
    fwException_raise(argdsExceptionInfo,
          "ERROR",
          "fwElmb_createCANBus(): The data point \"" + argsDpName + "\" already exists",
          "");
    return;
  }
  
  // Check if a system name has been given
  if (strpos(argsDpName, ":") >= 0) {
    // System name has been included, so split it out
    dsTemp = strsplit(argsDpName, ":");
    
    // System name may be used in future to allow Dps to be created in other systems
    sSystemName = dsTemp[1];
    sDpName = dsTemp[2];
  } else {
    sDpName = argsDpName;
  }

  // Create dp
  dpCreate(sDpName, ELMB_CAN_BUS_TYPE_NAME);
  
  // Check dp was created successfully
  if (!dpExists(argsDpName)) {
    fwException_raise(argdsExceptionInfo,
          "ERROR",
          "fwElmb_createCANBus(): The data point \"" + argsDpName + "\" was not\nsuccessfully created",
          "");
    return;
  }
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_createNode
//
// Creates a dp of type Elmb.
//
// @param argsDpName: A name for the new data point. System name not required
// @param argdsExceptionInfo: Details of any exceptions are returned here
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Fernando Varela
//*****************************************************************************
void fwElmb_createNode(string argsDpName,
                       dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  string sSystemName;
  string sDpName;
  
  dyn_string dsTemp;

// Executable Code
// ---------------
  // Check argument given is valid
  if (strlen(argsDpName) == 0) {
    fwException_raise(argdsExceptionInfo,
          "ERROR",
          "fwElmb_createNode(): The data point name must not be empty",
          "");
    return;
  }

  // Check dp does not already exist
  if (dpExists(argsDpName)) {
    fwException_raise(argdsExceptionInfo,
          "ERROR",
          "fwElmb_createNode(): The data point\n\"" + argsDpName + "\"\nalready exists",
          "");
    return;
  }
  
  // Check if a system name has been given
  if (strpos(argsDpName, ":") >= 0) {
    // System name has been included, so split it out
    dsTemp = strsplit(argsDpName, ":");
    
    // System name may be used in future to allow Dps to be created in other systems
    sSystemName = dsTemp[1];
    sDpName = dsTemp[2];
  } else {
    sDpName = argsDpName;
  }

  // Create dp
  dpCreate(sDpName, ELMB_TYPE_NAME);
  
  // Check dp was created successfully
  if (dpExists(argsDpName)) {
    // Set default DP functions required
    fwDevice_setDpFunction(argsDpName, fwDevice_DPFUNCTION_SET, argdsExceptionInfo);
  } else {
    fwException_raise(argdsExceptionInfo,
          "ERROR",
          "fwElmb_createNode():  The data point\n\"" + argsDpName + "\"\nwas not successfully created",
          "");
    return;
  }

  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_createConfig
//
// Creates a dp for configuration of Elmb settings.
//
// @param argsDpName: A name for the new data point. System name not required
// @param argsDpType: Exact DP type to create. Different types exist for
//                    Analog/Digital input/output
// @param argdsExceptionInfo: Details of any exceptions are returned here
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Fernando Varela
//*****************************************************************************
void fwElmb_createConfig(  string argsDpName,
                          string argsDpType,
                          dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
// None
    
// Executable Code
// ---------------
  // Check new dp name is not empty
  if (argsDpName == "") {
    fwException_raise(argdsExceptionInfo,
          "ERROR",
          "fwElmb_createConfig(): The data point name must not be empty",
          "");
    return;
  }

  // Check dp does not already exist
  if (dpExists(argsDpName)) {
    fwException_raise(argdsExceptionInfo,
          "ERROR",
          "fwElmb_createConfig(): The data point \"" + argsDpName + "\" already exists",
          "");
    return;
  }

  // Create dp
  dpCreate(argsDpName, argsDpType);
  
  // Check dp was created successfully
  if (dpExists(argsDpName)) {
    // Set default DP functions, only for analog input config
    if (argsDpType == ELMB_AI_CONFIG_TYPE_NAME)
      fwDevice_setDpFunction(argsDpName, fwDevice_DPFUNCTION_SET, argdsExceptionInfo);
  } else {
    fwException_raise(argdsExceptionInfo,
          "ERROR",
          "fwElmb_createConfig():  The data point \"" + argsDpName + "\" was not\nsuccessfully created",
          "");
    return;
  }
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_createChannel
//
// Creates a DP for an Elmb channel
//
// @param argsDpName: A name for the new data point. System name not required
// @param argsComment: A comment (string) that will be applied to the DP and
//                      the 'value' DPE
// @param argsDpType: Exact DP type to create (string). Different types exist
//                    for Analog/Digital input/output
// @param argsSensorType: A value (string) that is the sensor type for this
//                        channel
// @param argdsChannelID: The channel(s) (dyn_string) that this sensor
//                        uses
// @param argsFct: The function (string) which will be used in the OPC Server
//                  configuration file in the [ITEM] section
// @param argdsExceptionInfo: Details of any exceptions are returned here
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Fernando Varela
//*****************************************************************************
void fwElmb_createChannel(string argsDpName,
                          string argsComment,
                          string argsDpType,
                          string argsSensorType,
                          dyn_string argdsChannelID,
                          string argsFct,
                          dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
// None

// Executable Code
// ---------------
  // Check new dp name is not empty
  if (argsDpName == "") {
    fwException_raise(argdsExceptionInfo,
          "ERROR",
          "fwElmb_createChannel(): The data point name must not be empty",
          "");
    return;
  }

  // Check dp does not already exist
  if (dpExists(argsDpName)) {
    fwException_raise(argdsExceptionInfo,
          "ERROR",
          "fwElmb_createChannel(): The data point \"" + argsDpName + "\" already exists",
          "");
    return;
  }

  // Create dp
  dpCreate(argsDpName, argsDpType);
  
  // Check dp was created successfully
  if (!dpExists(argsDpName)) {
    fwException_raise(argdsExceptionInfo,
          "ERROR",
          "fwElmb_createChannel():  The data point \"" + argsDpName + "\" was not\nsuccessfully created",
          "");
    return;
  }
  
  // If everything ok, we set dp Comment:
  if (!patternMatch("*.*", argsDpName) || !patternMatch("*.", argsDpName))
    argsDpName = argsDpName + ".";
  dpSetComment(argsDpName, argsComment);

  // Check if type is Elmb specific
  if (argsDpType == ELMB_AI_TYPE_NAME) {
    // Set Analog input channel ID and other information
    dpSet(argsDpName + "channel", argdsChannelID,
          argsDpName + "function", argsFct,
          argsDpName + "type", argsSensorType);
          
    // Set comment on value DPE also
    dpSetComment(argsDpName + "value", argsComment);
    
  } else if (argsDpType == ELMB_AI_SDO_TYPE_NAME) {
    // Set SDO Analog input channel ID
    dpSet(argsDpName + "channel", argdsChannelID[1]);
          
    // Set comment on value DPE also
    dpSetComment(argsDpName + "value", argsComment);
  
    // Set up the DP functions required (firstly removing any trailing '.')
    argsDpName = dpSubStr(argsDpName, DPSUB_DP);
    fwDevice_setDpFunction(argsDpName, fwDevice_DPFUNCTION_SET, argdsExceptionInfo);
    
  } else if (argsDpType == ELMB_AO_TYPE_NAME) {
    // Set Analog output channel ID
    dpSet(argsDpName + "channel", argdsChannelID[1]);
          
    // Set comment on value DPE also
    dpSetComment(argsDpName + "value", argsComment);
    
  } else if ((argsDpType == ELMB_DI_TYPE_NAME) ||
             (argsDpType == ELMB_DO_TYPE_NAME)) {
    // Set Digital ID
    dpSet(argsDpName + "id", argdsChannelID[1]);
    
    // Set comment on value DPE also
    dpSetComment(argsDpName + "value", argsComment);
  }
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_createDigitalBytes
//
// Creates a DP for Elmb digital output bytes
//
// @param argsElmb: Datapoint name of ELMB to create digital output bytes for
// @param argbOPCAddressing: Flag to indicate whether to address the datapoint
//                           created or not
// @param argdsExceptionInfo: Details of any exceptions are returned here
//
// Modification History: None
//
// Constraints:
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Jim Cook
//*****************************************************************************
void fwElmb_createDigitalBytes(string argsElmb,
                               bool argbOPCAddressing,
                               dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  string sChannelName;
  string sOPCGroup;
  string sBus;
  
  dyn_string dsTemp;
  
// Executable Code
// ---------------
  // Digital output sensors require some extra datapoints for hardware/software consistency
  sChannelName = dpSubStr(argsElmb, DPSUB_DP) + fwDevice_HIERARCHY_SEPARATOR +
                 ELMB_DO_CONFIG_NAME + fwDevice_HIERARCHY_SEPARATOR + 
                 ELMB_DO_PREFIX + "Bytes";

  if (!dpExists(sChannelName + ".")) {
    dpCreate(sChannelName, ELMB_DO_BYTES_TYPE_NAME);
  
    // Check dp was created successfully
    if (!dpExists(sChannelName)) {
      fwException_raise(argdsExceptionInfo,
                        "ERROR",
                        "fwElmb_createDigital():  The data point \"" + sChannelName + "\" was not\nsuccessfully created",
                        "");
      return;
    }
  }
      
  // OPC addressing code
  if (argbOPCAddressing) {
    dsTemp = strsplit(argsElmb, fwDevice_HIERARCHY_SEPARATOR);
    sBus = dsTemp[2];
    sOPCGroup = "_Do_Read_" + sBus;
    if (!dpExists(sOPCGroup)) {
      dpCreate(sOPCGroup, "_OPCGroup");
      fwElmb_setValuesToOPCGroup(sOPCGroup, true, argdsExceptionInfo);
    }
    fwDevice_setAddress(sChannelName, makeDynString(fwDevice_ADDRESS_DEFAULT), argdsExceptionInfo);
  }
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_checkPortAInputs
//
// Function used to check whether there are any digital inputs defined for an
// ELMB.
//
// @param argsElmb: DP name of ELMB to (de)activate masks for
//
// Modification History: None
//
// Constraints: None
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Jim Cook
//*****************************************************************************
bool fwElmb_checkPortAInputs(string argsElmb)
{
// Local Variables
// ---------------
  bool bReturn = false;
  
  dyn_string dsDIs;
  dyn_string dsIds;
  dyn_string dsPortADIs;

// Executable Code
// ---------------
  // Check for any digital inputs
  dsDIs = dpNames(argsElmb + fwDevice_HIERARCHY_SEPARATOR + ".id", ELMB_DI_TYPE_NAME);
  if (dynlen(dsDIs) > 0) {
          
    // Get the IDs of the digital inputs
    dpGet(dsDIs, dsIds);

    // Find only those on Port A
    dsPortADIs = dynPatternMatch("A;*", dsIds);
          
    // Activate enable masks for port A
    if (dynlen(dsPortADIs) > 0)
      bReturn = true;
  }
  
  // Return to calling routine
  return (bReturn);
}

//*****************************************************************************
// @name Function: fwElmb_activatePortAMasks
//
// Function used to activate (or deactivate) the address config for Port A
// digital input interrupt enable and input enable masks. This is required as
// these masks are only entered into the OPC Server configuration file if needed,
// though the addressing is always present (deactivating the address stops
// any error messages)
//
// @param argsElmb: DP name of ELMB to (de)activate masks for
// @param argdsExceptionInfo: Any exceptions are returned here
// @param argbActivate: Optional argument for whether to activate or deactivate
//                      the address config. Default is true (activate)
//
// Modification History: None
//
// Constraints: None
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Jim Cook
//*****************************************************************************
void fwElmb_activatePortAMasks(string argsElmb,
                               dyn_string &argdsExceptionInfo,
                               bool argbActivate = true)
{
// Local Variables
// ---------------
  int i;
  int iType;
  
  dyn_string dsMaskDpes;

// Executable Code
// ---------------
  // Ensure ELMB DP name has system name
  argsElmb = dpSubStr(argsElmb, DPSUB_SYS_DP);
  
  // Create the DPE names required
  dsMaskDpes = makeDynString(argsElmb + fwDevice_HIERARCHY_SEPARATOR + ELMB_DI_CONFIG_NAME + ".portAInterruptMask.read",
                             argsElmb + fwDevice_HIERARCHY_SEPARATOR + ELMB_DI_CONFIG_NAME + ".portAInterruptMask.write",
                             argsElmb + fwDevice_HIERARCHY_SEPARATOR + ELMB_DI_CONFIG_NAME + ".portAInEnMask.read",
                             argsElmb + fwDevice_HIERARCHY_SEPARATOR + ELMB_DI_CONFIG_NAME + ".portAInEnMask.write");
  
  // Loop through the DPEs and do the necessary actions
  for (i = 1; i <= dynlen(dsMaskDpes); i++) {
    dpGet(dsMaskDpes[i] + ":_address.._type", iType);
    if (iType != DPCONFIG_NONE)
      dpSet(dsMaskDpes[i] + ":_address.._active", argbActivate);
  }
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_elementSQ
//
// Function used to single query a given DPE. There is no check that the driver
// is running.
//
// @param argsDPE: Full name of DPE to Single Query
// @param argtTimeout: Value of timeout to wait for expected response
// @param argaReadValue: The read value is returned here
// @param argdsExceptionInfo: Any exceptions are returned here
//
// Modification History: None
//
// Constraints: None
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Jim Cook
//*****************************************************************************
void fwElmb_elementSQ(string argsDPE,
                      time argtTimeout,
                      anytype &argaReadValue,
                      dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  bool bTimeout;
  bool bIsRunning;
  bool bRemote = false;
  
  int iDriverNumber;
  
  time tTimeNow;
  
  string sSystem;
  
  dyn_string dsDpNamesToWaitFor;
  dyn_string dsDpNamesReturn;
  
  dyn_anytype daConditions;
  dyn_anytype daReturnedValues;
  
// Executable Code
// ---------------
  // Ensure the DPE does not contain any config information
  argsDPE = dpSubStr(argsDPE, DPSUB_SYS_DP_EL);
  
  // Get the system name only and check for remote system
  sSystem = dpSubStr(argsDPE, DPSUB_SYS);
  if (sSystem != getSystemName())
    bRemote = true;
  
  // Get the driver number
  dpGet(argsDPE + ":_distrib.._driver", iDriverNumber);
  
  // Check if driver is running
  fwPeriphAddress_checkIsDriverRunning(iDriverNumber,
                                       bIsRunning,
                                       argdsExceptionInfo,
                                       sSystem);
  if (dynlen(argdsExceptionInfo) > 0) {
    return;
  } else if (!bIsRunning) {
    // Raise an exception
    fwException_raise(argdsExceptionInfo,
                      "ERROR",
                      "Driver with '-num " + iDriverNumber + "' is not running" + (bRemote ? " on remote system" : ""),
                      "");
    return;
  }
  
  // Set up parameters to wait for the value to be updated  
  dsDpNamesToWaitFor = makeDynString(argsDPE + ":_online.._value",
                                     argsDPE + ":_online.._stime");
//                                     argsDPE + ":_online.._invalid");
                                     
  dsDpNamesReturn = dsDpNamesToWaitFor;
      
  // Get time now
  tTimeNow = getCurrentTime();
  
  // For scattered systems, timing of PCs may be slightly out, therefore this is a fix
  // to try and compensate
  // tTimeNow = tTimeNow - 1;//
      
  // Request the required data
  dpSet(sSystem + "_Driver" + iDriverNumber + ".SQ", argsDPE);
    
  // Wait for up to the timeout for a value to be updated
  dpWaitForValue(dsDpNamesToWaitFor,
                 daConditions,
                 dsDpNamesReturn,
                 daReturnedValues,
                 argtTimeout,
                 bTimeout);
                 
  // if the value and the timestamp didn't timeout, wait for the invalid flag to become false 
  // (_online.._invalid doesn't seem to work with dpWaitForValue(...))
  if (!bTimeout) {
    while(getCurrentTime() - tTimeNow < argtTimeout) {
      bool invalidFlag;
      dpGet(argsDPE + ":_online.._invalid", invalidFlag);
      daReturnedValues[3] = invalidFlag;
      if (invalidFlag == false) { 
        break;
      }
      delay(0, 10);
    }    
  }
  
  // Check whether timeout has expired or value updated
  if (bTimeout) {
    fwException_raise(argdsExceptionInfo,
                      "ERROR",
                      "Value for '" + argsDPE + "', was not updated due to timeout",
                      "");
  } else {
//    if (dynlen(daReturnedValues) > 1) {
//      if (daReturnedValues[2] >= tTimeNow) {
        argaReadValue = daReturnedValues[1];
        if (daReturnedValues[3])
          fwException_raise(argdsExceptionInfo,
                            "ERROR",
                            "Value for '" + argsDPE + "', is invalid",
                            "");
//      } // else {//
        // fwException_raise(argdsExceptionInfo,//
        //                  "ERROR",//
        //                  "Value for '" + argsDPE + "', was not updated",//
        //                  "");//
     // }//
//    }
  }
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_monitorElmbStateCbk
//
// Callback used to check for certain states and then set analog input channels
// invalid if required
//
// Modification History: None
//
// Constraints: None
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Jim Cook
//*****************************************************************************
void fwElmb_monitorElmbStateCbk(string sDpState, unsigned uValueState)
{
// Local Variables
// ---------------
  bool bInvalid;
  
  int i;

  string sDpName;
  
  dyn_string dsAIs;
  
// Executable Code
// ---------------
  // Check for state which would not give analog input channel updates
  uValueState &= 0x7f;
  if ((uValueState == 0x04) || (uValueState == 0x7f)) {
      
    // Get the name of the DP from the DPE name
    sDpName = dpSubStr(sDpState, DPSUB_SYS_DP);
    
    // Get all analog inputs for this ELMB
    dsAIs = dpNames(sDpName + fwDevice_HIERARCHY_SEPARATOR + ELMB_AI_CONFIG_NAME + fwDevice_HIERARCHY_SEPARATOR + "*.value:_original.._aut_inv", ELMB_AI_TYPE_NAME);
    dpGet(dsAIs[1], bInvalid);
    if (!bInvalid) {
      for (i = 1; i <= dynlen(dsAIs); i++)
        dpSet(dsAIs[i], true);
    }
  }
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_monitorElmbEMCbk
//
// Callback used to check for certain emergency messages and then set analog\
// input channels invalid if required
//
// Modification History: None
//
// Constraints: None
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Jim Cook
//*****************************************************************************
void fwElmb_monitorElmbEMCbk(  string sDpErrorCode, int iValueErrorCode,
                              string sDpErrorByte1, char cValueErrorByte1,
                              string sDpErrorByte2, char cValueErrorByte2,
                              string sDpErrorByte3, char cValueErrorByte3,
                              string sDpErrorByte4, char cValueErrorByte4,
                              string sDpErrorByte5, char cValueErrorByte5,
                              string sTimetamp, time tTime)
{
// Local Variables
// ---------------
  bool bInvalid;
  
  int i;
  int iChannel;
  
  string sDpName;
  string sEmergencyMsg;
  
  dyn_string dsAIs;
  dyn_string dsChannels;
  
// Executable Code
// ---------------
  // Get the name of the DP from the DPE name
  sDpName = dpSubStr(sDpErrorCode, DPSUB_SYS_DP);
  
  // Check for emergency message 0x5000 0x01
  if ((iValueErrorCode == 0x5000) && (cValueErrorByte1 == 1)) {
    
    // Get all analog inputs for this ELMB
    dsAIs = dpNames(sDpName + fwDevice_HIERARCHY_SEPARATOR + ELMB_AI_CONFIG_NAME + fwDevice_HIERARCHY_SEPARATOR + "*", ELMB_AI_TYPE_NAME);
    dpGet(dsAIs[1], bInvalid);
    if (!bInvalid) {
      // Set all necessary channels invalid (simulating 'invalid by driver',
      // just as OPC server would do)
      for (i = 1; i <= dynlen(dsAIs); i++) {
        dpGet(dsAIs[i] + ".channel", dsChannels);
        iChannel = dsChannels[1];
        if (iChannel >= cValueErrorByte2)
          dpSet(dsAIs[i] + ".value:_original.._aut_inv", true);
      }
    }
    
/*
    // Cannot set 'online_value', so remove it (then dpSet uses default of 'original_value')
    strreplace(sDpErrorCode, ":_online.._value", "");
    strreplace(sDpErrorByte1, ":_online.._value", "");
    strreplace(sDpErrorByte2, ":_online.._value", "");
    strreplace(sDpErrorByte3, ":_online.._value", "");
    strreplace(sDpErrorByte4, ":_online.._value", "");
    strreplace(sDpErrorByte5, ":_online.._value", "");
    
    // Zero these parts of the emergenecy message
    // These parts of the emergency message are zeroed so that if another emergency message 
    // with the same values is received, we still capture it
    dpSet(sDpErrorCode, 0,
          sDpErrorByte1, 0,
          sDpErrorByte2, 0,
          sDpErrorByte3, 0,
          sDpErrorByte4, 0,
          sDpErrorByte5, 0);
*/
  }
  sprintf(sEmergencyMsg, "0x%04x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x @ %s",
          iValueErrorCode & 0xffff,
          cValueErrorByte1,
          cValueErrorByte2,
          cValueErrorByte3,
          cValueErrorByte4,
          cValueErrorByte5,
          formatTime("%d.%m.%Y %H:%M:%S", tTime, ".%03d"));
  dpSet(sDpName + ".monitoring.emergencyMessage.fullText", sEmergencyMsg);
          
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_monitorOpcCbk
//
// Callback used to check for certain states and then set analog input channels
// invalid if required
//
// @param argsElmb: DP name of ELMB to monitor
//
// Modification History: None
//
// Constraints: None
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Jim Cook
//*****************************************************************************
void fwElmb_monitorOpcCbk(string sDpServerConnected, bool bServerConnected)
{
// Local Variables
// ---------------
  bool bInvalid;
  
  int i, j;
  
  dyn_string dsAIs;
  dyn_string dsElmbs;
  
// Executable Code
// ---------------
  if (!bServerConnected) {
      
    // Get all ELMBs in the system
    dsElmbs = dpNames(getSystemName() + "*", ELMB_TYPE_NAME);
    
    // Loop through the ELMBs found
    for (i = 1; i <= dynlen(dsElmbs); i++) {
    
      // Get all analog inputs for this ELMB and set them invalid
      dsAIs = dpNames(dsElmbs[i] + fwDevice_HIERARCHY_SEPARATOR + ELMB_AI_CONFIG_NAME + fwDevice_HIERARCHY_SEPARATOR + "*.value:_original.._aut_inv", ELMB_AI_TYPE_NAME);
      dpGet(dsAIs[1], bInvalid);
      for (j = 1; j <= dynlen(dsAIs); j++)
        dpSet(dsAIs[j], true);
    }
  }
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmb_monitorInvalid
//
// Callback used to check for any channel being invalid. A DPE is set to true
// if any channel is found to be invalid to allow for an alert to be signalled.
//
// @param dsValidList: List of channels
// @param dbIsInvalid: boolean values indicating whether channel is invalid or not
//
// Modification History: None
//
// Constraints: None
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Jim Cook
//*****************************************************************************
void fwElmb_cbkMonitorInvalid(dyn_string dsValidList,
                              dyn_bool dbIsInvalid)
{
// Local Variables
// ---------------
  string sNode;
  
  dyn_string dsTemp;
    
// Executable Code
// ---------------
  // Split the name of the first element and get the node name from this
  dsTemp = strsplit(dsValidList[1], fwDevice_HIERARCHY_SEPARATOR);
  sNode = dsTemp[1] + fwDevice_HIERARCHY_SEPARATOR +
          dsTemp[2] + fwDevice_HIERARCHY_SEPARATOR +
          dsTemp[3] + ".channelInvalid";
  
  // Check for any channel invalid and set flag as necessary
  if (dynContains(dbIsInvalid, true) > 0) {
    dpSet(sNode, true);
  } else {
    dpSet(sNode, false);
  }
  
  // Return to calling routine
  return;
}


//*****************************************************************************
// @name Function: fwElmb_cbkDOinitialisation
//
// Callback used to check if any ELMB that has DO defined became operationnal
// In this case, fwElmbUser_synchronizeDoBytes() executed.
//
// @param dsValidList: sDp -> .state.value
//
// Modification History: None
//
// Constraints: None
//
// Usage: Public
//
// PVSS manager usage: VISION, CTRL
//
// @author Olivier Gutzwiller
//*****************************************************************************
fwElmb_cbkDOinitialisation(string sDp, int iNewValue)
{
// Local Variables
// ---------------
string sElmb = strsplit(sDp ,".")[1]; 
int iOldValue;
dpGet(sElmb+"/DO/do_Bytes.ElmbState", iOldValue); 
dyn_string argdsExceptionInfo; 
if ((iNewValue == 0x05 || iNewValue == 0x85) && (iOldValue != 0x05 && iOldValue != 0x85)) {
fwElmbUser_synchronizeDoBytes(sElmb,
argdsExceptionInfo);
// Check for errors
if (dynlen(argdsExceptionInfo) > 0)
return;
}
dpSet(sElmb+"/DO/do_Bytes.ElmbOldState", iOldValue); 
dpSet(sElmb+"/DO/do_Bytes.ElmbState", iNewValue);
// Return to calling routine
return;
}


//*********************************************************************************
// Correction of constant parameters for temperature calculation of NTC sensors
// Target component : FwElmb (any version)
// Date : 14/12/09
// @author Sebastien Franz, Olivier Gutzwiller
//*********************************************************************************

fwElmb_updateNTC()
{
  //temperature calculation formula (Steinhart and Hart):
  // T = 1/(a + b*Ln(R) + c*(Ln(R))^3)
  string a = "1.129241e-3";
  string b = "2.341077e-4";
  string c = "8.775468e-8";
  
  string dpType,fct,sensorStr,aiStr,channel;
  dyn_string dplist;
  
  //Get all NTC DP of type FwElmbAi in the system
  dyn_string dpELMBai = dpNames(getSystemName() + "*","FwElmbAi");
  for(int i = 1; i <= dynlen(dpELMBai); i++)
  {
    dpGet(dpELMBai[i] + ".type",dpType);
    if(dpType == "NTC 10 kOhm")
      dynAppend(dplist,dpELMBai[i]);
  }
  DebugTN("Found " + dynlen(dplist) + " NTC in system " + getSystemName());
  
  //Replace formula with new constant parameters  
  for(int i = 1; i <= dynlen(dplist); i++)
  {
    dpGet(dplist[i] + ".channel",channel);
    //DebugN("channel " + channel);
    sensorStr = strsplit(dplist[i],"/")[2] + "." + 
                strsplit(dplist[i],"/")[3] + "." +
                strsplit(dplist[i],"/")[5];
    aiStr = strsplit(dplist[i],"/")[2] + "." + 
            strsplit(dplist[i],"/")[3] + "." +
            "ai_" + channel;
    fct = sensorStr + " = 1/(" + a + " + " + b +
          "*log(1.e-6*"+ aiStr + "*1000000/(2.5-1.e-6*" + 
          aiStr + "))+" + c + "*pow(log(1000000*1.e-6*" +
          aiStr + "/(2.5-1.e-6*" + aiStr + ")),3))-273.15";
    dpSet(dplist[i] + ".function",fct);
    DebugTN("NTC " + dplist[i] + " updated!");
  }
  DebugN("NTC update done!");
}


///////////////////////////////////////////////////////////////////////////////
// Mutex implementation functions
// usage:
//   - first init mutex with name: fwElmb_initLock(name)
//   - then lock or unlock mutex with fwElmb_lock(name)
//     or fwElmb_unlock(name)
// Date : 20/10/10
// @author Stefan Schlenker, Olivier Gutzwiller
///////////////////////////////////////////////////////////////////////////////

synchronized bool fwElmb_initLock(string lockName)
{
  if (!globalExists("gElmbList")) {
    addGlobal("gElmbList", DYN_STRING_VAR);
    addGlobal("gElmbLocked", DYN_BOOL_VAR);
  }
  if (dynlen(gElmbList) == 12) {
    error("Cannot create more mutex locks, maximum reached (12).");
    return false;
  }

  if (dynContains(gElmbList, lockName)==0) {
    dynAppend(gElmbList, lockName);
    dynAppend(gElmbLocked, false);
  }
  return true;
}

fwElmb_lock(string lockName)
{
//   DebugTN("fwElmb_lock: lock "+lockName);
  int index = dynContains(gElmbList, lockName);
//   DebugTN("fwElmb_lock: lock "+lockName+" has index "+index);
  switch (index) {
  case 1: _fwElmb_lock1();break;
  case 2: _fwElmb_lock2();break;
  case 3: _fwElmb_lock3();break;
  case 4: _fwElmb_lock4();break;
  case 5: _fwElmb_lock5();break;
  case 6: _fwElmb_lock6();break;
  case 7: _fwElmb_lock7();break;
  case 8: _fwElmb_lock8();break;
  case 9: _fwElmb_lock9();break;
  case 10: _fwElmb_lock10();break;
  case 11: _fwElmb_lock11();break;
  case 12: _fwElmb_lock12();break;    
  }
}

synchronized fwElmb_unlock(string lockName)
{
  int index = dynContains(gElmbList, lockName);
  gElmbLocked[index] = false;
}

_fwElmb_lock(int lockNum)
{
//   DebugTN("_lock: lock "+lockNum+gElmbLocked);
  while (gElmbLocked[lockNum]) {
    //    DebugTN("_lock: waiting for getting lock "+lockNum);
    delay(0, 50);
  }
  gElmbLocked[lockNum] = true;
}

//
// need one lock function per mutex (max = 12) to protect lock list
//
synchronized _fwElmb_lock1() {_fwElmb_lock(1);}
synchronized _fwElmb_lock2() {_fwElmb_lock(2);}
synchronized _fwElmb_lock3() {_fwElmb_lock(3);}
synchronized _fwElmb_lock4() {_fwElmb_lock(4);}
synchronized _fwElmb_lock5() {_fwElmb_lock(5);}
synchronized _fwElmb_lock6() {_fwElmb_lock(6);}
synchronized _fwElmb_lock7() {_fwElmb_lock(7);}
synchronized _fwElmb_lock8() {_fwElmb_lock(8);}
synchronized _fwElmb_lock9() {_fwElmb_lock(9);}
synchronized _fwElmb_lock10() {_fwElmb_lock(10);}
synchronized _fwElmb_lock11() {_fwElmb_lock(11);}
synchronized _fwElmb_lock12() {_fwElmb_lock(12);}


///////////////////////////////////////////////////////////////////////////////


