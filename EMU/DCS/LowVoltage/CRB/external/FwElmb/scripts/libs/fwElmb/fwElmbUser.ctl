

//  @Author: Fernando Varela Rodriguez
//  @Date: 18th of March, 2004
//  @Description:
//
//  This library provides functions that may be used in user panels and scripts
//  to create CAN buses, ELMBs and inputs and outputs. Some utility functions
//  are also provided.
//
//  @Limitations: None
//  @Bugs and fixes: Fernando.Varela.Rodriguez@cern.ch.
//
// User functions:  fwElmbUser_checkDefaultDriver
//                  fwElmbUser_checkStateOperational
//                  fwElmbUser_createCANbus
//                  fwElmbUser_createElmb
//                  fwElmbUser_createSensor
//                  fwElmbUser_createAnalogOutput
//                  fwElmbUser_createDigital
//                  fwElmbUser_createSPI
//                  fwElmbUser_createOPCFile
//                  fwElmbUser_readSPIValue
//                  fwElmbUser_monitorAll
//                  fwElmbUser_monitorElmb
//                  fwElmbUser_monitorOpc
//                  fwElmbUser_setDoBit
//                  fwElmbUser_setDoBits
//                  fwElmbUser_setDoByte
//                  fwElmbUser_setDoBytes
//                  fwElmbUser_getDoByte
//                  fwElmbUser_getDoBytes
//                  fwElmbUser_synchronizeDoBytes
//                  fwElmbUser_updateFirmwareInfo

//*****************************************************************************
// @name Function: fwElmbUser_checkDefaultDriver
//
// Checks whether the driver (OPC Client) or simulator with the correct number
// is running for the ELMB component
//
// @param argiDriverNumber:   The default driver number for the ELMB is returned
//                            here
// @param argdsExceptionInfo: Exception handler information returned here
// @param argsDpTypeName:     A datapoint type name may be given here, otherwise
//                            the CANbus type is used by default
// @param argsSystemName:     Optional system name if checking a remote system
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
bool fwElmbUser_checkDefaultDriver(int &argiDriverNumber,
                                   dyn_string &argdsExceptionInfo,
                                   string argsDpTypeName = ELMB_CAN_BUS_TYPE_NAME,
                                   string argsSystemName = "")
{
// Local Variables
// ---------------
  bool bIsRunning = false;
  
  dyn_string dsAddressParameters;

// Executable Code
// ---------------
  // Just to make sure that everything is properly 
  // initialized even if the function is not called from the FW
  fwDevice_initialize();
  
  // Initialise driver number return value
  argiDriverNumber = 0;
  
  // Check is DPT given (this allows the default driver number to be found)
  if (strlen(argsDpTypeName) == 0)
    argsDpTypeName = ELMB_CAN_BUS_TYPE_NAME;
  
  // Check if a system name is given, if not, default to local system
  if (strlen(argsSystemName) == 0)
    argsSystemName = getSystemName();
    
  // Get address parameters to obtain driver number used
  fwDevice_getAddressDefaultParamsOPC(argsDpTypeName,
                                      dsAddressParameters,
                                      argdsExceptionInfo);

  if (dynlen(argdsExceptionInfo) > 0) {
    fwException_raise(argdsExceptionInfo,
                      "ERROR",
                      "fwElmbUser_checkDefaultDriver(): Problem reading the\ndefault OPC address parameters. Aborting action...",
                      "");
  } else {
    // Set driver number return value
    argiDriverNumber = dsAddressParameters[fwDevice_ADDRESS_DRIVER_NUMBER];
  
    // Check driver is running
    fwPeriphAddress_checkIsDriverRunning(argiDriverNumber,
                                         bIsRunning,
                                         argdsExceptionInfo,
                                         argsSystemName);
  }

  // Return to calling routine
  return (bIsRunning);
}

//*****************************************************************************
// @name Function: fwElmbUser_checkStateOperational
//
// Checks whether the ELMB specified is in the operational state or not.
//
// @param argsElmb: DP name of ELMB to check
// @param argdsExceptionInfo: Exception handler information returned here
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
bool fwElmbUser_checkStateOperational(string argsElmb,
                                      dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  bool bIsOperational = false;
  
  unsigned uState;
  
//  string sNgDpe;
  
//  dyn_string dsTemp;

// Executable Code
// ---------------
  // Ensure system name given
  argsElmb = dpSubStr(argsElmb, DPSUB_SYS_DP);
  
  // Get the datapoint name of the associated bus
  /* JRC not implemented yet. This should check that last update was within nodeguardinterval
  dsTemp = strsplit(argsElmb, fwDevice_HIERARCHY_SEPARATOR);
  if (dynlen(dsTemp) == 3)
    sNgDpe = dsTemp[1] + fwDevice_HIERARCHY_SEPARATOR + dsTemp[2] + ".nodeGuardInterval";
    */
  
  // Get required values
  dpGet(argsElmb + ".state.value", uState);
  if ((uState & 0x7f) == 5)
    bIsOperational = true;
  
  // Return to calling routine
  return (bIsOperational);
}

//*****************************************************************************
// @name Function: fwElmbUser_createCANbus
//
// Creates a CANbus datapoint ands sets various parts of the required
// information to the datapoint elements
//
// @param argsUserName: Name (string) of bus to create, not including system
//                      name or any framework hierarchy information
// @param argsComment: Any comment to be applied to the datapoint
// @param argsPort: Port identifier (string). Must include 'can' prefix for
//                  National Intruments ports.
// @param argsCard: Name of card interface (string). There are constants
//                  defined in the file fwElmbConstants.ctl for:
//                    Kvaser card - ELMB_CAN_CARD_KVASER
//                    NICAN card - ELMB_CAN_CARD_NICAN
// @param argiBusSpeed: Bus speed (integer) in bits/s
// @param argbDefaultAddressing: True is default OPC addressing should be applied
// @param argdsExceptionInfo: Exception handler information returned here
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
fwElmbUser_createCANbus(string argsUserName, 
                        string argsComment, 
                        string argsPort, 
                        string argsCard, 
                        int argiBusSpeed, 
                        bool argbDefaultAddressing,
                        dyn_string &argdsExceptionInfo)
{

//To be done:  1- Check if card info is correct, i.e. if the can card and port are free

// Local Variables
// ---------------
  int i;
  int iManagement = 0;
  int iSyncInterval = 0;
  int iNodeGuardInterval = 0;
  int isRunning;
    
  string sDpName;

  dyn_string dsAvailablePortListKVASER, dsAvailablePortListNICAN;
  dyn_string dsAddressParameters;  

// Executable Code
// ---------------
  // Just to make sure that everything is properly 
  // initialized even if the function is not called from the FW
  fwDevice_initialize();

   // Warning for empty user name field
   if ((argsUserName == "")) {
    fwException_raise(argdsExceptionInfo,
                      "ERROR",
                      "Empty or invalid CANbus name. Aborting action...",
                      "");
    return;
  }
  
  // Get a FW complient DP name from the User Name  
  fwElmb_getFwName(argsUserName, VENDOR_NAME, sDpName);
                
  // Create CAN bus 
  fwElmb_createCANbus(sDpName,
                      argdsExceptionInfo);
  if (dynlen(argdsExceptionInfo) > 0) 
    return;

  // If we have arrived to this point, it means no error => Right moment to set 
  // the values to the dpe in the CANbus structure
  fwElmb_setValuesToCANbus(sDpName, 
                           argsComment, 
                           argsPort, 
                           argsCard,
                           argiBusSpeed, 
                           iManagement, 
                           iSyncInterval, 
                           iNodeGuardInterval, 
                           argdsExceptionInfo);

  // Check if exception has been raised:
  if (dynlen(argdsExceptionInfo) > 0) 
    return;
  
  // Now we want to create the dps for the OPC groups that are required
  fwElmb_createOPCGroup(ELMB_CAN_BUS_TYPE_NAME, argsUserName, argdsExceptionInfo);
  if (dynlen(argdsExceptionInfo) > 0) 
    return;

  // If default OPC addressing selected -> address items
  if (argbDefaultAddressing)
    fwDevice_setAddress(sDpName, makeDynString(fwDevice_ADDRESS_DEFAULT), argdsExceptionInfo);
  
  // Initialise the monitoring information
  fwElmbMonitorLib_setupMonitor(sDpName);

  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmbUser_createElmb
//
// Creates a ELMB node data-point, from the information provided in the
// arguments
//
// @param argsUserName: Name (string) of ELMB node to create, not including
//                      system name or any framework hierarchy information
// @param argsComment: Any comment to be applied to the datapoint
// @param argsBusName: Name of the CANbus this ELMB is connected to. System
//                      name and framework hierarchy information is optional
// @param argsNodeID:  Value (string) containing the nodeId in decimal
// @param argbDefaultAddressing: True is default OPC addressing should be applied
// @param argdsExceptionInfo: Exception handler information returned here
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
fwElmbUser_createElmb(string argsUserName, 
                      string argsComment, 
                      string argsBusName, 
                      string argsNodeID, 
                      bool argbDefaultAddressing, 
                      dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  bool isRunning;
  
  int iManagement;
  
  string sDpName;
  
  dyn_string dsAddressParameters;

// Executable Code
// ---------------
  // Makes sure function works if it is not called from the FwDEN
  fwDevice_initialize();
  
  if ((argsUserName == "") || (argsNodeID == ""))
    return;
    
  // Check if bus name contains any hierarchy information
  if (patternMatch("*" + VENDOR_NAME + fwDevice_HIERARCHY_SEPARATOR + "*", argsBusName)) {
    // Framework hierarchy information is included, remove system name if given
    argsBusName = dpSubStr(argsBusName, DPSUB_DP);
  } else {
    // Framework hierarchy information NOT included (therefore system name cannot be there)
    argsBusName = VENDOR_NAME + fwDevice_HIERARCHY_SEPARATOR + argsBusName;
  }
  
  // Get a FW complient DP name from the User Name  
  fwElmb_getFwName(argsUserName, argsBusName, sDpName);
  
  // Check DPT is of correct type and create DP
  fwElmb_createNode(sDpName, argdsExceptionInfo);

  // Check for any errors
  if (dynlen(argdsExceptionInfo) > 0) 
    return;
  
  //We have arrived to this point? Right moment to set values to the ELMB dpes
  fwElmb_setValuesToElmb(sDpName,
                         argsComment,
                         argsNodeID,
                         argdsExceptionInfo);
  // Check for any errors
  if (dynlen(argdsExceptionInfo) > 0) 
    return;

  // new OPC addressing code
  if (argbDefaultAddressing)
    fwDevice_setAddress(sDpName, makeDynString(fwDevice_ADDRESS_DEFAULT), argdsExceptionInfo);
  
  // Initialise the monitoring information
  fwElmbMonitorLib_setupMonitor(sDpName);
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmbUser_createSensor
//
// Creates an analog input sensor data-point from the information provided in
// the arguments. The actual name of the datapoint created is given as a return
// to the function if successful.
//
// @param argsUserName: Name (string) of sensor to create, not including
//                      system name or any framework hierarchy information
// @param argsBusName: Name (string) of CANbus the ELMB to which the sensor
//                      should be created is connected to. System name and
//                      framework hierarchy information is optional
// @param argsElmbName: Name of the ELMB this sensor will be connected to.
//                      System name and framework hierarchy information is
//                      optional
// @param argsComment: Comment for the sensor
// @param argdsChannelNumber: List of channel numbers used by this sensor
// @param argssSensorType: Name of the sensor type as set in the Sensor Info
//                          datapoint
// @param argdfParameters: List of parameter values (i.e. these will replace
//                          the markers %x1, ..., %xn that are defined in the
//                          formula for the sensor type). If no parameters are
//                          given, but the formula needs them, the default values
//                          defined for the sensor are used.
// @param argbDefaultAddressing: True is default OPC addressing is to be applied
// @param argdsExceptionInfo: Exception handler information returned here
// @param argsFctTemplate: [Optional] Sensor function template (e.g. %c1+3.5/%x1).
//                          Only required if formula defined in Sensor Info
//                          datapoint for this type is to be over-ridden
// @param argsOpcItem: [Optional] Name of the OPC item corresponding to this
//                      sensor. Only needed if default OPC item should be
//                      over-ridden.
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
string fwElmbUser_createSensor(string argsUserName,
                               string argsBusName,
                               string argsElmbName,
                               string argsComment,
                               dyn_string argdsChannelNumber,
                               string argsSensorType,
                               dyn_float argdfParameters,
                               bool argbDefaultAddressing,
                               dyn_string &argdsExceptionInfo,
                               string argsFctTemplate = "",
                               string argsOpcItem = "")
{
// Local Variables
// ---------------
  int i, j;
  int iPos;
  
  string sElmbName;
  string sSensorName = "";
  string sConfigName;
  string sOpcItemName;
  string sFct;
  string sFctTemplate;
  string sSensorPrefix;
  string sProfile;
  string sParameters;
  
  dyn_bool dbIsRaw;

  dyn_string dsChannelOpcItems;
  dyn_string dsAddressParameters;
  dyn_string dsSensorTypes;
  dyn_string dsFunctionTemplate;
  dyn_string dsPDOdps;
  dyn_string dsTemp;

// Executable Code
// ---------------  
  // Makes sure it works even if not called from the FwDEN
  fwDevice_initialize();  
    
  // Remove any system name and/or framework hierarchy information from the bus
  dsTemp = strsplit(argsBusName, fwDevice_HIERARCHY_SEPARATOR);
  argsBusName = dsTemp[dynlen(dsTemp)];
    
  // Remove any system name and/or framework hierarchy information from the ELMB
  dsTemp = strsplit(argsElmbName, fwDevice_HIERARCHY_SEPARATOR);
  argsElmbName = dsTemp[dynlen(dsTemp)];

  // Build up ELMB name
  sElmbName = VENDOR_NAME + fwDevice_HIERARCHY_SEPARATOR +
              argsBusName + fwDevice_HIERARCHY_SEPARATOR +
              argsElmbName;
  
  // Create OPC item name, first checking if item has been given
  if (argsOpcItem == "") {
    // Use same name as user name for item
    sOpcItemName = argsBusName + OPC_DELIMETER + argsElmbName + OPC_DELIMETER + argsUserName;
  } else {
    // Use item name given
    sOpcItemName = argsBusName + OPC_DELIMETER + argsElmbName + OPC_DELIMETER + argsOpcItem;
  }

  // Check for a raw value sensor
  dpGet(ELMB_SENSOR_INFO_NAME + ".types", dsSensorTypes,
        ELMB_SENSOR_INFO_NAME + ".isRaw", dbIsRaw,
        ELMB_SENSOR_INFO_NAME + ".pdoDp", dsPDOdps);
        
  // Find the index of this sensor type
  iPos = dynContains(dsSensorTypes, argsSensorType);
  if (iPos <= 0)
    return ("");
  
  if (dbIsRaw[iPos]) {
    sFct = ELMB_NO_INFO;
  } else {
    // Check whether sensor uses custom PDO and get prefix if so
    if (dsPDOdps[iPos] == ELMB_NO_INFO) {
      sSensorPrefix = ELMB_AI_PREFIX;
      sProfile = "404";
    } else {
      dpGet(dsPDOdps[iPos] + ".name", sSensorPrefix,
            dsPDOdps[iPos] + ".profile", sProfile);
    }
    
    // Set the input channels
    if ((sProfile == "404") || (sProfile == "LMB")) {
      for(i = 1; i <= dynlen(argdsChannelNumber); i++) {
        dsChannelOpcItems[i] =   argsBusName + OPC_DELIMETER + 
                                argsElmbName + OPC_DELIMETER + 
                                sSensorPrefix + argdsChannelNumber[i];
      }
    } else {
      // Channel number does not apply
      dsChannelOpcItems = makeDynString(argsBusName + OPC_DELIMETER +
                                        argsElmbName + OPC_DELIMETER +
                                        sSensorPrefix);
    }

    // Check if a function template has been given
    if (argsFctTemplate == "") {
      // Get information from internal DP
      dpGet(ELMB_SENSOR_INFO_NAME + ".functions", dsFunctionTemplate);
        
      // Set the template to use
      sFctTemplate = dsFunctionTemplate[iPos];
    } else {
      sFctTemplate = argsFctTemplate;
    }
    
    // Check if parameters have been given, and if not, get defaults from
    // sensor definition
    if (dynlen(argdfParameters) == 0) {
      dpGet(ELMB_SENSOR_INFO_NAME + ".parameters", dsTemp);
      if (dsTemp[iPos] == ELMB_NO_INFO)
        dynClear(argdfParameters);
      else
        fwGeneral_stringToDynString(dsTemp[iPos], argdfParameters);
    }

    // Create the required function  
    fwElmb_getUserDefinedFct(sFctTemplate, 
                             dsChannelOpcItems, 
                             argdfParameters, 
                             sFct, 
                             argdsExceptionInfo);

    if (dynlen(argdsExceptionInfo) > 0)
      return ("");
                           
    sFct = sOpcItemName + " = " + sFct;
  }
    
  //Check if the config for that channel already exists.
  sConfigName = sElmbName + fwDevice_HIERARCHY_SEPARATOR + ELMB_AI_CONFIG_NAME;
  if (!dpExists(sConfigName + ".")) {
    fwElmb_createConfig(sConfigName,
                        ELMB_AI_CONFIG_TYPE_NAME,
                        argdsExceptionInfo);
  
    if (dynlen(argdsExceptionInfo) > 0)
      return ("");

    // Set OPC address
    fwDevice_setAddress(sConfigName, makeDynString(fwDevice_ADDRESS_DEFAULT), argdsExceptionInfo);
  }
    
  //Build up the sensor name from the config name
  sSensorName = sConfigName + fwDevice_HIERARCHY_SEPARATOR + argsUserName;
    
  fwElmb_createChannel(sSensorName, 
                       argsComment, 
                       ELMB_AI_TYPE_NAME, 
                       argsSensorType, 
                       argdsChannelNumber, 
                       sFct, 
                       argdsExceptionInfo);
                         
  // Check for errors
  if (dynlen(argdsExceptionInfo) > 0)
    return ("");
  
  // Create any OPC group(s) necessary
  fwElmb_createOPCGroup(ELMB_AI_TYPE_NAME, argsBusName, argdsExceptionInfo);
    
  if (argbDefaultAddressing)
    fwDevice_setAddress(sSensorName, makeDynString(fwDevice_ADDRESS_DEFAULT), argdsExceptionInfo);
  
  // Return to calling routine
  return (sSensorName);
}

//*****************************************************************************
// @name Function: fwElmbUser_createAnalogInputSDO
//
// Creates an analog input for SDO for an existing Elmb. The actual name of
// the datapoint created is given as a return to the function if successful.
//
// @param argsBusName: Name of bus (string) that Elmb is connected to. System
//                      name and framework hierarchy information is optional
// @param argsElmbName: Name of Elmb (string) to create analog input on. System
//                      name and framework hierarchy information is optional
// @param argsComment: Comment (string) for the analog input
// @param argiChannel: Channel number (int) for the analog input
// @param argbOPCAddressing: Should default OPC addressing be configured
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
string fwElmbUser_createAnalogInputSDO(string argsBusName,
                                       string argsElmbName,
                                       string argsComment,
                                       int argiChannel,
                                       bool argbOPCAddressing,
                                       dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  string sElmbName;
  string sChannelName;
  string sConfigDpName;
  
  dyn_string dsIds;
  dyn_string dsTemp;

// Executable Code
// ---------------
  // Makes sure it works even if not called from the FwDEN
  fwDevice_initialize();
    
  // Remove any system name and/or framework hierarchy information from the bus
  dsTemp = strsplit(argsBusName, fwDevice_HIERARCHY_SEPARATOR);
  argsBusName = dsTemp[dynlen(dsTemp)];
    
  // Remove any system name and/or framework hierarchy information from the ELMB
  dsTemp = strsplit(argsElmbName, fwDevice_HIERARCHY_SEPARATOR);
  argsElmbName = dsTemp[dynlen(dsTemp)];
    
  // Check channel value is valid
  if ((argiChannel < 0) || (argiChannel >= ELMB_MAX_CHANNEL)) {
    // Raise an exception
    fwException_raise(argdsExceptionInfo,
                      "ERROR",
                      "fwElmbUser_createAnalogInputSDO(): Channel \"" + argiChannel + "\" is an invalid value",
                      "");
    return ("");
  }

  // Build up ELMB name
  sElmbName = VENDOR_NAME + fwDevice_HIERARCHY_SEPARATOR +
              argsBusName + fwDevice_HIERARCHY_SEPARATOR +
              argsElmbName;

  // Set the ID
  dsIds = makeDynString(argiChannel);

  fwElmb_getChannelName(ELMB_AI_SDO_PREFIX + argiChannel,
                        sElmbName,
                        ELMB_AI_SDO_TYPE_NAME,
                        sChannelName,
                        argdsExceptionInfo);
                        
  // Check for success
  if (dynlen(argdsExceptionInfo) > 0)
    return ("");

  // Before creating the sensor we have to look if the config for that channel already exists.
  sConfigDpName = sElmbName + fwDevice_HIERARCHY_SEPARATOR + ELMB_AI_CONFIG_NAME;
  if (!dpExists(sConfigDpName + ".")) {
    fwElmb_createConfig(sConfigDpName,
                        ELMB_AI_CONFIG_TYPE_NAME,
                        argdsExceptionInfo);
    if (dynlen(argdsExceptionInfo) > 0)
      return ("");
    
    // OPC addressing code
    if (argbOPCAddressing)
      fwDevice_setAddress(sConfigDpName, makeDynString(fwDevice_ADDRESS_DEFAULT), argdsExceptionInfo);
  }

  // Create the digital line    
  fwElmb_createChannel(sChannelName,
                       argsComment,
                       ELMB_AI_SDO_TYPE_NAME,
                       "",
                       dsIds,
                       "",
                       argdsExceptionInfo);

  // Check for errors
  if (dynlen(argdsExceptionInfo) > 0)
    return ("");
  
  // Create any OPC group(s) necessary
  fwElmb_createOPCGroup(ELMB_AI_SDO_TYPE_NAME, argsBusName, argdsExceptionInfo);

  // Check for whether to apply OPC addressing
  if (argbOPCAddressing)
    fwDevice_setAddress(sChannelName, makeDynString(fwDevice_ADDRESS_DEFAULT), argdsExceptionInfo);
  
  // Return to calling routine
  return (sChannelName);
}

//*****************************************************************************
// @name Function: fwElmbUser_createDigital
//
// Creates a digital input or output for an existing Elmb. The actual name of
// the datapoint created is given as a return to the function if successful.
//
// @param argsBusName: Name of bus (string) that Elmb is connected to. System
//                      name and framework hierarchy information is optional
// @param argsElmbName: Name of Elmb (string) to create digital line on. System
//                      name and framework hierarchy information is optional
// @param argsComment: Comment (string) for the digital line
// @param argsPort: Port letter (string) for the digital line
// @param argiBit: Bit number (int) in the digital byte (port)
// @param argbDigitalInput: True of digital input to be created,
//                          False to create digital output
// @param argbOPCAddressing: Should default OPC addressing be configured
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
string fwElmbUser_createDigital(string argsBusName,
                                string argsElmbName,
                                string argsComment,
                                string argsPort,
                                int argiBit,
                                bool argbDigitalInput,
                                bool argbOPCAddressing,
                                dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  bool bStatus = false;
  bool bActivate;
  
  string sDigitalTypeName;
  string sDigitalConfigName;
  string sDigitalConfigTypeName;
  string sElmbName;
  string sSensorName;
  string sChannelName;
  string sConfigDpName;
  
  dyn_string dsIds;
  dyn_string dsAddressParameters;
  dyn_string dsTemp;

// Executable Code
// ---------------
  // Makes sure it works even if not called from the FwDEN
  fwDevice_initialize();
    
  // Remove any system name and/or framework hierarchy information from the bus
  dsTemp = strsplit(argsBusName, fwDevice_HIERARCHY_SEPARATOR);
  argsBusName = dsTemp[dynlen(dsTemp)];
    
  // Remove any system name and/or framework hierarchy information from the ELMB
  dsTemp = strsplit(argsElmbName, fwDevice_HIERARCHY_SEPARATOR);
  argsElmbName = dsTemp[dynlen(dsTemp)];

  // Check for digital input or output
  if (argbDigitalInput) {
    sDigitalTypeName = ELMB_DI_TYPE_NAME;
    sDigitalConfigName = ELMB_DI_CONFIG_NAME;
    sDigitalConfigTypeName = ELMB_DI_CONFIG_TYPE_NAME;
    
    // Check the port is correct for input
    if ((argsPort != "F") && (argsPort != "A") && (argsPort != "D") && (argsPort != "E")) {
      // Raise an exception
      fwException_raise(argdsExceptionInfo,
                        "ERROR",
                        "fwElmbUser_createDigital(): Port \"" + argsPort + "\" is not an input port",
                        "");
    } else {
      // Create sensor name    
      sSensorName = ELMB_DI_PREFIX + argsPort + "_" + argiBit;
    }
  } else {
    sDigitalTypeName = ELMB_DO_TYPE_NAME;
    sDigitalConfigName = ELMB_DO_CONFIG_NAME;
    sDigitalConfigTypeName = ELMB_DO_CONFIG_TYPE_NAME;
    
    // Check the port is correct for output
    if ((argsPort != "C") && (argsPort != "A")) {
      // Raise an exception
      fwException_raise(argdsExceptionInfo,
                        "ERROR",
                        "fwElmbUser_createDigital(): Port \"" + argsPort + "\" is not an output port",
                        "");
    } else {
      // Create sensor name    
      sSensorName = ELMB_DO_PREFIX + argsPort + "_" + argiBit;
    }
  }
  
  // Check for exceptions
  if (dynlen(argdsExceptionInfo) > 0)
    return ("");
    
  // Check bit value is valid
  if ((argiBit < 0) || (argiBit > 7)) {
    // Raise an exception
    fwException_raise(argdsExceptionInfo,
                      "ERROR",
                      "fwElmbUser_createDigital(): Bit \"" + argiBit + "\" is an invalid value",
                      "");
    return ("");
  }

  // Build up ELMB name
  sElmbName = VENDOR_NAME + fwDevice_HIERARCHY_SEPARATOR +
              argsBusName + fwDevice_HIERARCHY_SEPARATOR +
              argsElmbName;

  // Set the ID
  dsIds = makeDynString(argsPort + ";" + argiBit);

  fwElmb_getChannelName(sSensorName,
                        sElmbName,
                        sDigitalTypeName,
                        sChannelName,
                        argdsExceptionInfo);
                        
  // Check for success
  if (dynlen(argdsExceptionInfo) > 0)
    return ("");

  // Before creating the sensor we have to look if the config for that channel already exists.
  sConfigDpName = sElmbName + fwDevice_HIERARCHY_SEPARATOR + sDigitalConfigName;
  if (!dpExists(sConfigDpName + ".")) {
    fwElmb_createConfig(sConfigDpName,
                        sDigitalConfigTypeName,
                        argdsExceptionInfo);
    if (dynlen(argdsExceptionInfo) > 0)
      return ("");
    
    // OPC addressing code
    if (argbOPCAddressing)
      fwDevice_setAddress(sConfigDpName, makeDynString(fwDevice_ADDRESS_DEFAULT), argdsExceptionInfo);
  }

  // Create the digital line    
  fwElmb_createChannel(sChannelName,
                       argsComment,
                       sDigitalTypeName,
                       "",
                       dsIds,
                       "",
                       argdsExceptionInfo);

  // Check for errors
  if (dynlen(argdsExceptionInfo) > 0)
    return ("");
  
  // Create any OPC group(s) necessary
  fwElmb_createOPCGroup(sDigitalTypeName, argsBusName, argdsExceptionInfo);

  // Digital outputs need some special handling to be able to read the digital outputs
  // for hardware/software consistency  
  if (sDigitalTypeName == ELMB_DO_TYPE_NAME) {

    fwElmb_createDigitalBytes(sElmbName, argbOPCAddressing, argdsExceptionInfo);
    
  } else if (argbOPCAddressing) {
    fwDevice_setAddress(sChannelName, makeDynString(fwDevice_ADDRESS_DEFAULT), argdsExceptionInfo);
  }
  
  if (sDigitalTypeName == ELMB_DI_TYPE_NAME) {
    bActivate = fwElmb_checkPortAInputs(sElmbName);
    fwElmb_activatePortAMasks(sElmbName, argdsExceptionInfo, bActivate);
  }
  
  // Return to calling routine
  return (sChannelName);
}

//*****************************************************************************
// @name Function: fwElmbUser_createAnalogOutput
//
// Creates an analog output for an existing Elmb. The actual name of
// the datapoint created is given as a return to the function if successful.
//
// @param argsBusName: Name of bus (string) that Elmb is connected to. System
//                      name and framework hierarchy information is optional
// @param argsElmbName: Name of Elmb (string) to create analog output on. System
//                      name and framework hierarchy information is optional
// @param argsComment: Comment (string) for the analog output
// @param argiChannel: Channel number (int) for the analog output
// @param argbOPCAddressing: Should default OPC addressing be configured
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
string fwElmbUser_createAnalogOutput(string argsBusName,
                                     string argsElmbName,
                                     string argsComment,
                                     int argiChannel,
                                     bool argbOPCAddressing,
                                     dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  string sElmbName;
  string sChannelName;
  string sConfigDpName;
  
  dyn_string dsIds;
  dyn_string dsTemp;

// Executable Code
// ---------------
  // Makes sure it works even if not called from the FwDEN
  fwDevice_initialize();
    
  // Remove any system name and/or framework hierarchy information from the bus
  dsTemp = strsplit(argsBusName, fwDevice_HIERARCHY_SEPARATOR);
  argsBusName = dsTemp[dynlen(dsTemp)];
    
  // Remove any system name and/or framework hierarchy information from the ELMB
  dsTemp = strsplit(argsElmbName, fwDevice_HIERARCHY_SEPARATOR);
  argsElmbName = dsTemp[dynlen(dsTemp)];
    
  // Check channel value is valid
  if ((argiChannel < 0) || (argiChannel >= ELMB_MAX_CHANNEL)) {
    // Raise an exception
    fwException_raise(argdsExceptionInfo,
                      "ERROR",
                      "fwElmbUser_createAnalogOutput(): Channel \"" + argiChannel + "\" is an invalid value",
                      "");
    return ("");
  }

  // Build up ELMB name
  sElmbName = VENDOR_NAME + fwDevice_HIERARCHY_SEPARATOR +
              argsBusName + fwDevice_HIERARCHY_SEPARATOR +
              argsElmbName;

  // Set the ID
  dsIds = makeDynString(argiChannel);

  fwElmb_getChannelName(ELMB_AO_PREFIX + argiChannel,
                        sElmbName,
                        ELMB_AO_TYPE_NAME,
                        sChannelName,
                        argdsExceptionInfo);
                        
  // Check for success
  if (dynlen(argdsExceptionInfo) > 0)
    return ("");

  // Before creating the sensor we have to look if the config for that channel already exists.
  sConfigDpName = sElmbName + fwDevice_HIERARCHY_SEPARATOR + ELMB_AO_CONFIG_NAME;
  if (!dpExists(sConfigDpName + ".")) {
    fwElmb_createConfig(sConfigDpName,
                        ELMB_AO_CONFIG_TYPE_NAME,
                        argdsExceptionInfo);
    if (dynlen(argdsExceptionInfo) > 0)
      return ("");
    
    // OPC addressing code
    if (argbOPCAddressing)
      fwDevice_setAddress(sConfigDpName, makeDynString(fwDevice_ADDRESS_DEFAULT), argdsExceptionInfo);
  }

  // Create the digital line    
  fwElmb_createChannel(sChannelName,
                       argsComment,
                       ELMB_AO_TYPE_NAME,
                       "",
                       dsIds,
                       "",
                       argdsExceptionInfo);

  // Check for errors
  if (dynlen(argdsExceptionInfo) > 0)
    return ("");
  
  // Create any OPC group(s) necessary
  fwElmb_createOPCGroup(ELMB_AO_TYPE_NAME, argsBusName, argdsExceptionInfo);

  // Check for whether to apply OPC addressing
  if (argbOPCAddressing)
    fwDevice_setAddress(sChannelName, makeDynString(fwDevice_ADDRESS_DEFAULT), argdsExceptionInfo);
  
  // Return to calling routine
  return (sChannelName);
}

//*****************************************************************************
// @name Function: fwElmbUser_createSPI
//
// Creates SPI comunication information for an existing Elmb. The actual name of
// the datapoint created is given as a return to the function if successful.
//
// @param argsBusName: Name of bus (string) that Elmb is connected to. System
//                      name and framework hierarchy information is optional
// @param argsElmbName: Name of Elmb (string) to create digital line on. System
//                      name and framework hierarchy information is optional
// @param argsComment: Comment (string) for the digital line
// @param argbOPCAddressing: Should default OPC addressing be configured
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
string fwElmbUser_createSPI(string argsBusName,
                            string argsElmbName,
                            string argsComment,
                            bool argbOPCAddressing,
                            dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  bool bStatus = false;
  
  string sDpName;
  
  dyn_string dsAddressParameters;
  dyn_string dsTemp;

// Executable Code
// ---------------
  // Makes sure it works even if not called from the FwDEN
  fwDevice_initialize();
    
  // Remove any system name and/or framework hierarchy information from the bus
  dsTemp = strsplit(argsBusName, fwDevice_HIERARCHY_SEPARATOR);
  argsBusName = dsTemp[dynlen(dsTemp)];
    
  // Remove any system name and/or framework hierarchy information from the ELMB
  dsTemp = strsplit(argsElmbName, fwDevice_HIERARCHY_SEPARATOR);
  argsElmbName = dsTemp[dynlen(dsTemp)];

  // Build up DP name
  sDpName = VENDOR_NAME + fwDevice_HIERARCHY_SEPARATOR +
            argsBusName + fwDevice_HIERARCHY_SEPARATOR +
            argsElmbName + fwDevice_HIERARCHY_SEPARATOR +
            ELMB_SPI_CONFIG_NAME;

  // Create the SPI configuration/operation DP
  dpCreate(sDpName, ELMB_SPI_TYPE_NAME);
      
  // OPC addressing code
  if (argbOPCAddressing)
    fwDevice_setAddress(sDpName, makeDynString(fwDevice_ADDRESS_DEFAULT), argdsExceptionInfo);

  if (!patternMatch("*.*", sDpName) || !patternMatch("*.", sDpName))
    dpSetComment(sDpName + ".", argsComment);
  else
    dpSetComment(sDpName, argsComment);
  
  // Return to calling routine
  return (sDpName);
}

//*****************************************************************************
// @name Function: fwElmbUser_createOPCFile
//
// Creates an OPC configuration file for the list of buses given
//
// @param argdsBuses: DP names of buses (dyn_string) that should have
//                    configuration information in file
// @param argsFilename: Name of file (string) including path, to create
// @param argbUseNodeGuarding: Should node guard flags be used
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
void fwElmbUser_createOPCFile(dyn_string argdsBuses,
                              string argsFilename,
                              bool argbUseNodeGuarding,
                              dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  bool bValid;
  bool bPDOSection = false;
  bool bPDOItemSection = false;
  bool bSDOSection = false;
  bool bItemSection = false;
  bool bInitSection = false;
  bool bMuxAll;
  
  int i, j, k, l, m;
  int iChannel;
  int iBusSpeed;
  int iNodeId;
  int iError;
  int iPos;
  int iCobID;
  int iCobIdTemp;
  int iDataByte;
  int iMuxByte;
  int iMuxNumber;
  int iTimesMuxed;
  int iSyncIntervalAddr;
  int iNodeGuardAddr;
  int iManagementAddr;
  int iResult;
  
  unsigned uSyncInterval;
  unsigned uNodeGuard;
  unsigned uManagement;

  string sBusName;
  string sInterfaceCard;
  string sInterfacePort;
  string sCANServer;
  string sBusOPCName;
  string sBusSection = "[CANBUS]\n";
  string sElmbSection = "\n[ELMB]\n";
  string sSDOItemSection = "\n[SDOItem]\n";
  string sPDOSection = "\n[PDO]\n";
  string sPDOItemSection = "\n[PDOItem]\n";
  string sItemSection = "\n[ITEM]\n";
  string sInitSection = "\n[INIT]\n";
  string sElmbName;
  string sNodeId;
  string sElmbOPCName;
  string sFunction;
  string sType;
  string sTemp;
  string sPDOName;
  string sCobID;
  string sDataType;
  string sProfile;
  string sDirection;
  string sPDOdp;
  string sSubIndex;

  dyn_float df;
  
  dyn_string dsElmbs;
  dyn_string dsChannels;
  dyn_string ds;
  dyn_string dsTemp;
  dyn_string dsCustomSensors;
  dyn_string dsPDOdps;
  dyn_string dsInfo;
  dyn_string dsSPI;
  dyn_string dsSPInodes;
  dyn_string dsAOs;
  dyn_string dsDIs;
  dyn_string dsDOs;
  dyn_string dsIDs;
  dyn_string dsPortADIs;
  dyn_string dsPortDDIs;
  dyn_string dsPortEDIs;
  
  file fFile;
  
  mapping mPDOs;
  
// Executable Code
// ---------------
  // Check if information is valid
  if (dynlen(argdsBuses) == 0) {  
    fwException_raise(argdsExceptionInfo,
                      "ERROR",
                      "fwElmbUser_createOPCFile(): No buses given",
                      "");
    return;
  } else if (strlen(argsFilename) == 0) {
    fwException_raise(argdsExceptionInfo,
                      "ERROR",
                      "fwElmbUser_createOPCFile(): Filename given is invalid",
                      "");
    return;
  }
  
  // Get any sensors that use a custom PDO
  dynClear(dsCustomSensors);
  dynClear(dsPDOdps);
  dpGet(ELMB_SENSOR_INFO_NAME + ".types", ds,
        ELMB_SENSOR_INFO_NAME + ".pdoDp", dsTemp);
  for (i = 1; i <= dynlen(ds); i++) {
    if (dsTemp [i] != ELMB_NO_INFO) {
      dynAppend(dsCustomSensors, ds[i]);
      dynAppend(dsPDOdps, dsTemp[i]);
    }
  }
  dynClear(ds);
  dynClear(dsTemp);
    
  // Loop through all buses
  for (i = 1; i <= dynlen(argdsBuses); i++) {

    //Bus Level:
    // 0- we get rid of the system name from the bus name.
    // *** Note that for future releases of this script, the system name could
    //     be needed when working with distributed systems. *** 
    sBusName = argdsBuses[i];  
  
    // Get bus speed and interface type, port and bus speed
    dpGet(sBusName + ".id", sInterfaceCard,
          sBusName + ".interfacePort", sInterfacePort,
          sBusName + ".busSpeed", iBusSpeed,
          sBusName + ".syncInterval", uSyncInterval,
          sBusName + ".syncInterval:_address.._type", iSyncIntervalAddr,
          sBusName + ".nodeGuardInterval", uNodeGuard,
          sBusName + ".nodeGuardInterval:_address.._type", iNodeGuardAddr,
          sBusName + ".management", uManagement,
          sBusName + ".management:_address.._type", iManagementAddr);
              
    // Check if addressing has been set (if not, no point adding it to config file)
    if ((iSyncIntervalAddr == DPCONFIG_NONE) && (iNodeGuardAddr == DPCONFIG_NONE) && (iManagementAddr == DPCONFIG_NONE)) {
      bValid = false;
    } else {
      // Check interface card
      if (sInterfaceCard == ELMB_CAN_CARD_NICAN) {
        sCANServer = "NICANServer+";
        bValid = true;
      } else if (sInterfaceCard == ELMB_CAN_CARD_KVASER) {
        sCANServer = "KVCANServer+";
        bValid = true;
      } else {
        fwException_raise(argdsExceptionInfo,
                          "ERROR",
                          "fwElmbUser_createOPCFile(): Unknown interface card type found\n" + sInterfaceCard + "\nBus " + sBusName + " not written to file",
                          "");
        bValid = false;
      }
    }
    
    if (bValid) {
      // Get OPC name to use for this bus
      dsTemp = strsplit(sBusName, fwDevice_HIERARCHY_SEPARATOR);
      sBusOPCName = dsTemp[dynlen(dsTemp)];
    
      // Add information to bus section
      sBusSection += sBusOPCName + " = " + sCANServer + " " + sInterfacePort + " " + iBusSpeed + "\n";
    
      // Add any required information to the INIT section
      if (uNodeGuard > 0) {
        bInitSection = true;
        sInitSection += sBusOPCName + ".NodeGuardInterval = " + uNodeGuard + "\n";
      }
      if (uManagement > 0) {
        bInitSection = true;
        sInitSection += sBusOPCName + ".NMT = " + uManagement + "\n";
      }
      if (uSyncInterval > 0) {
        bInitSection = true;
        sInitSection += sBusOPCName + ".SyncInterval = " + uSyncInterval + "\n";
      }
      
      // Get all ELMBs on this bus
      dsElmbs = dpNames(sBusName + fwDevice_HIERARCHY_SEPARATOR + "*", ELMB_TYPE_NAME);

      // Loop through all these ELMBs  
      for (j = 1; j <= dynlen(dsElmbs); j++) {
      
        // Get the node ID and format it to hexadecimal
        sElmbName = dsElmbs[j];
        dpGet(sElmbName + ".id", iNodeId);
        sprintf(sNodeId, "%X", iNodeId);
    
        // Get the OPC name to use for this ELMB
        dsTemp = strsplit(sElmbName, fwDevice_HIERARCHY_SEPARATOR);
        sElmbOPCName = dsTemp[dynlen(dsTemp)];
    
        // Add information to ELMB section
        if (argbUseNodeGuarding)
          sElmbSection += sElmbOPCName + " = " + sBusOPCName + " " + sNodeId + " NG\n";
        else    
          sElmbSection += sElmbOPCName + " = " + sBusOPCName + " " + sNodeId + "\n";

        // Get all standard analog input channels
        dsChannels = dpNames(sElmbName + fwDevice_HIERARCHY_SEPARATOR + "*", ELMB_AI_TYPE_NAME);
        for (k = 1; k <= dynlen(dsChannels); k++) {
        
          // Get the function required
          dpGet(dsChannels[k] + ".function", sFunction,
                dsChannels[k] + ".type", sType);
          
          // Check if function required (and only add to 'item' section if it is)
          if (sFunction != ELMB_NO_INFO) {
            bItemSection = true;
            sItemSection += sFunction + "\n";
          }
            
          // Check for sensor using custom PDO
          iPos = dynContains(dsCustomSensors, sType);
          if (iPos > 0) {
            sPDOdp = dsPDOdps[iPos];
            sTemp = sBusOPCName + ";" + sBusOPCName + "." + sElmbOPCName + ";" + iNodeId;
            if (mappingHasKey(mPDOs, sPDOdp)) {
              dsTemp = mPDOs[sPDOdp];
              dynAppend(dsTemp, sTemp);
              mPDOs[sPDOdp] = dsTemp;
            } else {
              dsTemp = makeDynString(sTemp);
              mPDOs[sPDOdp] = dsTemp;
            }
          }
        }  // End of loop on k: Standard analog input channels

        // Get all SDO analog input channels
        dsChannels = dpNames(sElmbName + fwDevice_HIERARCHY_SEPARATOR + "*", ELMB_AI_SDO_TYPE_NAME);
        if (dynlen(dsChannels) > 0) {
          bSDOSection = true;
          for (k = 1; k <= dynlen(dsChannels); k++) {
        
            // Get the function required
            dpGet(dsChannels[k] + ".channel", iChannel);
            sprintf(sSubIndex, "%X", (iChannel + 1));
            sSDOItemSection += ELMB_AI_SDO_PREFIX + iChannel + " = " + sBusOPCName + "." + sElmbOPCName + " 2404 " + sSubIndex + " IN VT_UI4\n";
          }  // End of loop on k: SDO analog input channels
        }

        // Check for any analog outputs
        dsAOs = dpNames(sElmbName + fwDevice_HIERARCHY_SEPARATOR + "*.channel", ELMB_AO_TYPE_NAME);
        if (dynlen(dsAOs) > 0) {
  
          // Add any SDO and PDO Item information for digital outputs
          bSDOSection = true;
          bPDOSection = true;
          bPDOItemSection = true;
          sSDOItemSection += "aoChannelMax = " + sBusOPCName + "." + sElmbOPCName + " 2500 1 IN VT_UI1\n";
          sSDOItemSection += "dacType = " + sBusOPCName + "." + sElmbOPCName + " 2500 2 IO VT_BOOL\n";
          sSDOItemSection += "aoSpiSclkHigh = " + sBusOPCName + "." + sElmbOPCName + " 2500 3 IO VT_UI1\n";
          
          iCobID = 0x300 + iNodeId;
          sprintf(sCobID, "%X", iCobID);
          sPDOSection += sCobID + " = " + sBusOPCName + " 404 OUT 0 64\n";
          sPDOItemSection += "ao_ = " + sBusOPCName + "." + sElmbOPCName + " " + sCobID + " VT_UI2 ALL 1\n";
        }

        // Check for any digital inputs on Port A, D or E
        dsDIs = dpNames(sElmbName + fwDevice_HIERARCHY_SEPARATOR + "*.id", ELMB_DI_TYPE_NAME);
        if (dynlen(dsDIs) > 0) {
          // As there are some digital inputs, add an entry for enabling digital input interrupts
          bSDOSection = true;
          sSDOItemSection += "diInterruptMaskF = " + sBusOPCName + "." + sElmbOPCName + " 6006 1 IO VT_UI1\n";
          
          // Calculate COB-ID
          iCobID = 0x180 + iNodeId;
          sprintf(sCobID, "%X", iCobID);
          
          // Get the IDs of the digital inputs
          dpGet(dsDIs, dsIDs);

          // Find only those on Port A
          dsPortADIs = dynPatternMatch("A;*", dsIDs);
          
          // Check if any digital inputs have been defined for port A on this ELMB
          if (dynlen(dsPortADIs) > 0) {

            sSDOItemSection += "diInterruptMaskA = " + sBusOPCName + "." + sElmbOPCName + " 6006 2 IO VT_UI1\n";
            sSDOItemSection += "dioOutputMaskA = " + sBusOPCName + "." + sElmbOPCName + " 6208 2 IO VT_UI1\n";

            for (int k = 1; k <= dynlen(dsPortADIs); k++) {
              dsTemp = strsplit(dsPortADIs[k], ";");
              sTemp = ELMB_DI_PREFIX + dsTemp[1] + "_" + dsTemp[2];

              sPDOItemSection += sTemp + " = " + sBusOPCName + "." + sElmbOPCName + " " + sCobID + " VT_BOOL 1 " + dsTemp[2] + "\n";
              bPDOItemSection = true;
            } // End of loop on k: Digital Inputs on Port A
          }
          
          // Find any on Port D
          dsPortDDIs = dynPatternMatch("D;*", dsIDs);
          for (int k = 1; k <= dynlen(dsPortDDIs); k++) {
            dsTemp = strsplit(dsPortDDIs[k], ";");
            sTemp = ELMB_DI_PREFIX + dsTemp[1] + "_" + dsTemp[2];

            sPDOItemSection += sTemp + " = " + sBusOPCName + "." + sElmbOPCName + " " + sCobID + " VT_BOOL 2 " + dsTemp[2] + "\n";
            bPDOItemSection = true;
          } // End of loop on k: Digital Inputs on Port D
          
          // Find any on Port E
          dsPortEDIs = dynPatternMatch("E;*", dsIDs);
          for (int k = 1; k <= dynlen(dsPortEDIs); k++) {
            dsTemp = strsplit(dsPortEDIs[k], ";");
            sTemp = ELMB_DI_PREFIX + dsTemp[1] + "_" + dsTemp[2];

            sPDOItemSection += sTemp + " = " + sBusOPCName + "." + sElmbOPCName + " " + sCobID + " VT_BOOL 3 " + dsTemp[2] + "\n";
            bPDOItemSection = true;
          } // End of loop on k: Digital Inputs on Port E
        }
        
        // Check for any digital outputs
        dsDOs = dpNames(sElmbName + fwDevice_HIERARCHY_SEPARATOR + "*.id", ELMB_DO_TYPE_NAME);
        if (dynlen(dsDOs) > 0) {
  
          // Add any SDO and PDO Item information for digital outputs
          bSDOSection = true;
          bPDOItemSection = true;
          iCobID = 0x200 + iNodeId;
          sprintf(sCobID, "%X", iCobID);
          sSDOItemSection += "do_C_read = " + sBusOPCName + "." + sElmbOPCName + " 6200 1 IN VT_UI1\n";
          sSDOItemSection += "do_A_read = " + sBusOPCName + "." + sElmbOPCName + " 6200 2 IN VT_UI1\n";
          sPDOItemSection += "do_write = " + sBusOPCName + "." + sElmbOPCName + " " + sCobID + " VT_UI2 0\n";
        }

        // Get whether SPI is setup for this ELMB
        dsSPI = dpNames(sElmbName + fwDevice_HIERARCHY_SEPARATOR + "*", ELMB_SPI_TYPE_NAME);
        if (dynlen(dsSPI) > 0) {
          // Add any SDO Item information for SPI communication
          bSDOSection = true;
          sSDOItemSection += "oneByteData = " + sBusOPCName + "." + sElmbOPCName + " 2600 1 IO VT_I4\n";
          sSDOItemSection += "twoByteData = " + sBusOPCName + "." + sElmbOPCName + " 2600 2 IO VT_I4\n";
          sSDOItemSection += "threeByteData = " + sBusOPCName + "." + sElmbOPCName + " 2600 3 IO VT_I4\n";
          sSDOItemSection += "fourByteData = " + sBusOPCName + "." + sElmbOPCName + " 2600 4 IO VT_I4\n";
          sSDOItemSection += "chipSelect = " + sBusOPCName + "." + sElmbOPCName + " 2601 0 IO VT_UI1\n";
          sSDOItemSection += "sclkHighPeriod = " + sBusOPCName + "." + sElmbOPCName + " 2602 1 IO VT_UI1\n";
          sSDOItemSection += "sclkRisingEdge = " + sBusOPCName + "." + sElmbOPCName + " 2602 2 IO VT_UI1\n";
        }
      } // End of loop on j: ELMBs
    } // End of check for valid information
  } // End of loop on i: Buses
        
  // Add any PDO Item information for ELMBs
  if (mappinglen(mPDOs) > 0) {
          
    // Set flag to ensure PDO information goes into file
    bPDOSection = true;
    bPDOItemSection = true;
          
    // Loop through each PDO in turn
    for (i = 1; i <= mappinglen(mPDOs); i++) {
      sPDOdp = mappingGetKey(mPDOs, i);
      dpGet(sPDOdp + ".name", sPDOName,
            sPDOdp + ".cobid", sCobID,
            sPDOdp + ".dataType", sDataType,
            sPDOdp + ".profile", sProfile,
            sPDOdp + ".direction", sDirection,
            sPDOdp + ".dataByte", iDataByte,
            sPDOdp + ".mux.all", bMuxAll,
            sPDOdp + ".mux.number", iMuxNumber,
            sPDOdp + ".mux.muxByte", iMuxByte,
            sPDOdp + ".mux.timesMuxed", iTimesMuxed);

      // Get decimal value of 'start' COB-ID
      sscanf(sCobID, "%x", iCobID);
                  
      // Get which ELMBs use this PDO
      dsInfo = mPDOs[sPDOdp];
      dynUnique(dsInfo);
            
      // Loop through all ELMBs with custom PDOs of this type
      for (j = 1; j <= dynlen(dsInfo); j++) {
        dsTemp = strsplit(dsInfo[j], ";");
      
        // Calculate correct Cob-ID for this ELMB
        iNodeId = dsTemp[3];
        iCobIdTemp = iCobID + iNodeId;
        sprintf(sCobID, "%X", iCobIdTemp);

        // Add information known so far            
        sPDOItemSection += sPDOName + " = " + dsTemp[2] + " " + sCobID + " " + sDataType;
        sPDOSection += sCobID + " = " + dsTemp[1] + " " + sProfile + " " + sDirection;
            
        // Check for multiplexed PDO
        if ((sProfile == "404") || (sProfile == "LMB")) {
          sPDOSection += " " + iMuxByte + " " + iTimesMuxed;
          if (bMuxAll)
            sPDOItemSection += " ALL";
          else
            sPDOItemSection += " " + iMuxNumber;
        }
            
        // Add the data start byte
        sPDOItemSection += " " + iDataByte;
        
        // Add end of line to any new lines
        sPDOSection += "\n";
        sPDOItemSection += "\n";
              
      } // End of loop on j: ELMBs with same custom PDO
    } // End of loop on i: Custom PDOs
  } // End of check for custom PDOs

  // We now have all information required. Open the file for writing
  fFile = fopen(argsFilename, "w");

  // Check if file was opened succcessfully
  if (fFile == 0) {
    iError = ferror(fFile);
    fwException_raise(argdsExceptionInfo,
                      "ERROR",
                      "fwElmbUser_createOPCFile(): Unable to open file,\n" + argsFilename,
                      iError);
    return;
  }

  // Write the CANBUS section to the file    
  fputs(sBusSection, fFile);
    
  // Write the ELMB section to the file    
  fputs(sElmbSection, fFile);
  
  // Write any SDO sections to file
  if (bSDOSection)
    fputs(sSDOItemSection, fFile);
  
  // Write any PDO sections to file
  if (bPDOSection)
    fputs(sPDOSection, fFile);
  if (bPDOItemSection)
    fputs(sPDOItemSection, fFile);

  // Write any ITEM section to the file
  if (bItemSection)
    fputs(sItemSection, fFile);
      
  // Write the INIT section to the file, if required
  if (bInitSection)
    fputs(sInitSection, fFile);
    
  // Close the file
  fclose(fFile);
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmbUser_readSPIValue
//
// Reads a value from the SPI bus of a value of the current SPI configuration
// and returns the value.
//
// @param argsElmb: DP name of ELMB or of SPI configuration DP (string) to
//                  read from. Hierarchy information is required, but system
//                  name is optional
// @param argsValueToRead: Name of element to read (string)
// @param argdsExceptionInfo: Any exceptions are returned here
// @param argtTimeout: Optional argument for timeout (default is 2 seconds)
// @param argbSetPreop: Optional argument for whether to set ELMB to
//                      pre-operational before any SDOs sent (default is true)
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
int fwElmbUser_readSPIValue(string argsElmb,
                            string argsValueToRead,
                            dyn_string &argdsExceptionInfo,
                            time argtTimeout = 2,
                            bool argbSetPreop = true)
{
// Local Variables
// ---------------
  bool bIsRunning;
  bool bTimeout;
  bool bRemote = false;
  
  int iDriverNumber;
  int iResult = 0;
  int i;
  
  unsigned uState;
  
  string sDpe;
  string sType;
  string sElmbDpName;
  string sSystem;
  
  anytype aValue;
  
  dyn_string dsDpNamesToWaitFor;
  dyn_string dsDpNamesReturn;
  dyn_string dsTemp;
  
  dyn_anytype daConditions;
  dyn_anytype daReturnedValues;
  
  time tTimeNow;
  
// Executable Code
// ---------------
  // Check for remote system
  sSystem = dpSubStr(argsElmb, DPSUB_SYS);
  if (sSystem != getSystemName())
    bRemote = true;
  
  // Check if driver is running and get driver number
  bIsRunning = fwElmbUser_checkDefaultDriver(iDriverNumber, argdsExceptionInfo, ELMB_CAN_BUS_TYPE_NAME, sSystem);
  if (dynlen(argdsExceptionInfo) == 0) {
    if (!bIsRunning) {
      // Raise an exception
      fwException_raise(argdsExceptionInfo,
                        "ERROR",
                        "OPC Client with '-num " + iDriverNumber + " is not running" + (bRemote ? " on remote system" : ""),
                        "");
    } else {
      // Check DPT of argument given
      sType = dpTypeName(argsElmb);
      if (sType == ELMB_SPI_TYPE_NAME) {
        sDpe = argsElmb + "." + argsValueToRead;
        dsTemp = strsplit(argsElmb, fwDevice_HIERARCHY_SEPARATOR);
        sElmbDpName = dsTemp[1];
        for (i = 2; i < dynlen(dsTemp); i++)
          sElmbDpName += fwDevice_HIERARCHY_SEPARATOR + dsTemp[i];
      } else if (sType == ELMB_TYPE_NAME) {
        sElmbDpName = argsElmb;
        sDpe = argsElmb + fwDevice_HIERARCHY_SEPARATOR + ELMB_SPI_CONFIG_NAME + "." + argsValueToRead;
      } else {
        fwException_raise(argdsExceptionInfo,
                          "ERROR",
                          "fwElmbUser_readSPIValue: Wrong device type given - " + sType,
                          "");
        return (0);
      }
      
      if (argbSetPreop) {
        // Get current state of ELMB
        dpGet(sElmbDpName + ".state.value", uState);
      
        // Set ELMB to preoperational
        dpSet(sElmbDpName + ".management", 128);
      }
      
      // Action a Single Query on the required DPE to obtain the value
      fwElmb_elementSQ(sDpe,
                       argtTimeout,
                       aValue,
                       argdsExceptionInfo);
      
      if (dynlen(argdsExceptionInfo) == 0)
        iResult = aValue;
  
      // Check whether state had been changed (and therefore needs to be set back)
      if (argbSetPreop) {
        // Set the ELMB back to it's original state, but only if it was stopped
        // or operational. Otherwise, leave it as preoperational
        if ((uState == 0x84) || (uState == 0x04))
          dpSet(sElmbDpName + ".management", 2);
        else if ((uState  == 0x85) || (uState == 0x05))
          dpSet(sElmbDpName + ".management", 1);
      }
    }
  }    

  // Return to calling routine
  return (iResult);
}

//*****************************************************************************
// @name Function: fwElmbUser_setDoBit
//
// Sets the given digital output while ensuring consistency between hardware
// and software information.
//*****************************************************************************
void fwElmbUser_setDoBit(string argsElmb,
                         string argsBitId,
                         bool argbValue,
                         dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  dyn_bool dbValues;
  
  dyn_string dsBitIds;
  
// Executable Code
// ---------------
  // Set up as if a list and call fwElmbUser_setDoBits function
  dbValues = makeDynBool(argbValue);
  dsBitIds = makeDynString(argsBitId);
  fwElmbUser_setDoBits(argsElmb, dsBitIds, dbValues, argdsExceptionInfo);
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmbUser_setDoBits
//
// Sets the given digital output bits while ensuring consistency between
// hardware and software information.
//*****************************************************************************
void fwElmbUser_setDoBits(string argsElmb,
                          dyn_string argdsBitIds,
                          dyn_bool argdbValues,
                          dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  int i;
  int iBit;
  
  unsigned uValueToSet;
  unsigned uMask;
  
  string sPort;
  
  dyn_string dsTemp;
  
// Executable Code
// ---------------
  // Ensure system name is given
  argsElmb = dpSubStr(argsElmb, DPSUB_SYS_DP);

  // Get current value
  uValueToSet = fwElmbUser_getDoBytes(argsElmb, argdsExceptionInfo);
  
  // Check for errors
  if (dynlen(argdsExceptionInfo) == 0) {
    for (i = 1; i <= dynlen(argdsBitIds); i++) {
      dsTemp = strsplit(argdsBitIds[i], ";");
      sPort = dsTemp[1];
      iBit = dsTemp[2];
      if (sPort == "A")
        uMask = 1 << (iBit + 8);
      else if (sPort == "C")
        uMask = 1 << iBit;
      if (argdbValues[i])
        uValueToSet |= uMask;
      else
        uValueToSet &= ~uMask;
    }
  
    // Set all relevant output bits/bytes
    fwElmbUser_setDoBytes(argsElmb, uValueToSet, argdsExceptionInfo);
  }
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmbUser_setDoByte
//
// Sets the given digital output byte while ensuring consistency between
// hardware and software information.
//*****************************************************************************
void fwElmbUser_setDoByte(string argsElmb,
                          string argsPort,
                          unsigned arguValue,
                          dyn_string &argdsExceptionInfo,
                          time argtTimeout = 2)
{
// Local Variables
// ---------------
  unsigned uPortA;
  unsigned uPortC;
  unsigned uValueToSet;
  
// Executable Code
// ---------------
  // Check for valid port
  if ((argsPort == "A") || (argsPort == "C")) {
    // Ensure system name is given
    argsElmb = dpSubStr(argsElmb, DPSUB_SYS_DP);
  
    if (argsPort == "A") {
      // Get current settings for Port C and what needs to be set to Port A
      uPortC = fwElmbUser_getDoByte(argsElmb, "C", argdsExceptionInfo, argtTimeout, false);
      uPortA = arguValue;
    } else {
      // Get what needs to be set to Port C and current settings for Port A
      uPortC = arguValue;
      uPortA = fwElmbUser_getDoByte(argsElmb, "A", argdsExceptionInfo, argtTimeout, false);
    }
  
    // Check for errors
    if (dynlen(argdsExceptionInfo) > 0)
      return;
    
    // Create actual value to write
    uValueToSet = (uPortA << 8) | uPortC;
  
    // Set all relevant output bits/bytes
    fwElmbUser_setDoBytes(argsElmb, uValueToSet, argdsExceptionInfo);
  } else {
    fwException_raise(argdsExceptionInfo,
                      "ERROR",
                      "fwElmbUser_setDoByte: Port '" + argsPort + "' is not a valid output port",
                      "");
    return;
  }
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmbUser_setDoBytes
//
// Sets the 16-bit digital output while ensuring consistency between hardware
// and software information. Only the least significant 16 bits are used, and
// this is made up of (PortA << 8) && PortC
//*****************************************************************************
void fwElmbUser_setDoBytes(string argsElmb,
                           unsigned arguValue,
                           dyn_string &argdsExceptionInfo)
{
// Local Variables
// ---------------
  int i;
  int iBit;
  
  string sDpe;
  string sPort;
  string sPortAInvalid;
  string sPortCInvalid;
  
  dyn_bool dbValues;
  
  dyn_string dsIdDpes;
  dyn_string dsIds;
  dyn_string dsValueDpes;
  dyn_string dsTemp;
  
// Executable Code
// ---------------
  // Ensure system name is given
  argsElmb = dpSubStr(argsElmb, DPSUB_SYS_DP);
  
  // Ensure only lowest (least significant) 16 bits are used
  arguValue &= 0xffff;
  
  // Set all existing outputs for consistency, starting with the only addressed element
  sDpe = argsElmb + fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_CONFIG_NAME +
         fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_PREFIX + "Bytes." +
         ELMB_DO_PREFIX + "write";
  dpSet(sDpe, arguValue);
  
  // Get any individual output bit datapoints
  dsIdDpes = dpNames(argsElmb + fwDevice_HIERARCHY_SEPARATOR + "*.id", ELMB_DO_TYPE_NAME);
  dsValueDpes = dpNames(argsElmb + fwDevice_HIERARCHY_SEPARATOR + "*.value", ELMB_DO_TYPE_NAME);
  
  // Ensure the two lists are in the same order
  dynSortAsc(dsIdDpes);
  dynSortAsc(dsValueDpes);

  // Get all the IDs so that the correct values can be set
  dpGet(dsIdDpes, dsIds);
  for (i = 1; i <= dynlen(dsIds); i++) {
    dsTemp = strsplit(dsIds[i], ";");
    sPort = dsTemp[1];
    iBit = dsTemp[2];
    if (sPort == "A") {
      dynAppend(dbValues, ((1 << (iBit + 8)) & arguValue) > 0 ? true : false);
    } else if (sPort == "C") {
      dynAppend(dbValues, ((1 << iBit) & arguValue) > 0 ? true : false);
    }
  }
  dpSet(dsValueDpes, dbValues);
  
  // The 'read' values are now possibly different to the actual values set,
  // so mark them as invalid
  sPortAInvalid = argsElmb + fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_CONFIG_NAME +
                  fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_PREFIX + "Bytes." +
                  ELMB_DO_PREFIX + "A" + "_read:_original.._aut_inv";
  sPortCInvalid = argsElmb + fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_CONFIG_NAME +
                  fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_PREFIX + "Bytes." +
                  ELMB_DO_PREFIX + "C" + "_read:_original.._aut_inv";
  dpSet(sPortAInvalid, true,
        sPortCInvalid, true);
  
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmbUser_getDoByte
//
// Reads value for the digtal byte of the port specified and returns the value.
//
// @param argsElmb: DP name of ELMB (string) to read from.
//                  Hierarchy information is required, but system name is optional
// @param argsPort: Port to read (string). Must be either 'A' or 'C'
// @param argdsExceptionInfo: Any exceptions are returned here
// @param argtTimeout: Optional argument for timeout (default is 2 seconds)
// @param argbSetPreop: Optional argument for whether to set ELMB to
//                      pre-operational before any SDOs sent (default is false)
//*****************************************************************************
unsigned fwElmbUser_getDoByte(string argsElmb,
                              string argsPort,
                              dyn_string &argdsExceptionInfo,
                              time argtTimeout = 2,
                              bool argbSetPreop = false)
{
// Local Variables
// ---------------
  bool bIsRunning;
  bool bTimeout;
  bool bRemote = false;
  
  int iDriverNumber;
  
  unsigned uState;
  unsigned uResult = 0;
  
  string sDpe;
  string sType;
  string sSystem;
  
  anytype aValue;
  
// Executable Code
// ---------------
  // Ensure system name is given
  argsElmb = dpSubStr(argsElmb, DPSUB_SYS_DP);
  
  // Check for remote system
  sSystem = dpSubStr(argsElmb, DPSUB_SYS);
  if (sSystem != getSystemName())
    bRemote = true;
  
  // Check if driver is running and get driver number
  bIsRunning = fwElmbUser_checkDefaultDriver(iDriverNumber, argdsExceptionInfo, ELMB_CAN_BUS_TYPE_NAME, sSystem);
  if (dynlen(argdsExceptionInfo) == 0) {
    if (!bIsRunning) {
      // Raise an exception
      fwException_raise(argdsExceptionInfo,
                        "ERROR",
                        "OPC Client with '-num " + iDriverNumber + " is not running" + (bRemote ? " on remote system" : ""),
                        "");
    } else {
      // Check DPT of argument given
      sType = dpTypeName(argsElmb);
      if (sType == ELMB_TYPE_NAME) {
        // Check port requested is valid
        if ((argsPort == "A") || (argsPort == "C")) {
          sDpe = argsElmb + fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_CONFIG_NAME +
                 fwDevice_HIERARCHY_SEPARATOR + ELMB_DO_PREFIX + "Bytes." +
                 ELMB_DO_PREFIX + argsPort + "_read";
          // Check if DPE exists
          if (!dpExists(sDpe)) {
            fwException_raise(argdsExceptionInfo,
                              "ERROR",
                              "fwElmbUser_getDoByte: Port '" + argsPort + "' does not exist on the specified ELMB",
                              "");
            return (0);
          }
        } else {
          fwException_raise(argdsExceptionInfo,
                            "ERROR",
                            "fwElmbUser_getDoByte: Port '" + argsPort + "' is not a valid output port",
                            "");
          return (0);
        }
      } else {
        fwException_raise(argdsExceptionInfo,
                          "ERROR",
                          "fwElmbUser_getDoByte: Wrong device type given - " + sType,
                          "");
        return (0);
      }
      
      if (argbSetPreop) {
        // Get current state of ELMB
        dpGet(argsElmb + ".state.value", uState);
      
        // Set ELMB to preoperational
        dpSet(argsElmb + ".management", 128);
      }
      
      // Action a Single Query on the required DPE to obtain the value
      fwElmb_elementSQ(sDpe,
                       argtTimeout,
                       aValue,
                       argdsExceptionInfo);

      if (dynlen(argdsExceptionInfo) == 0)
        uResult = aValue;
  
      // Check whether state had been changed (and therefore needs to be set back)
      if (argbSetPreop) {
        // Set the ELMB back to it's original state, but only if it was stopped
        // or operational. Otherwise, leave it as preoperational
        if ((uState == 0x84) || (uState == 0x04))
          dpSet(argsElmb + ".management", 2);
        else if ((uState  == 0x85) || (uState == 0x05))
          dpSet(argsElmb + ".management", 1);
      }
    }
  }
  
  // Ensure that any value returned is one byte max
  uResult &= 0xff;

  // Return to calling routine
  return (uResult);
}

//*****************************************************************************
// @name Function: fwElmbUser_getDoBytes
//
// Reads values of the digtal output bytes of the ELMB specified and returns
// the value. This value is returned as a two byte value and is made up of
// (PortA << 8) | PortC.
//
// @param argsElmb: DP name of ELMB (string) to read from.
//                  Hierarchy information is required, but system name is optional
// @param argdsExceptionInfo: Any exceptions are returned here
// @param argtTimeout: Optional argument for timeout (default is 2 seconds)
// @param argbSetPreop: Optional argument for whether to set ELMB to
//                      pre-operational before any SDOs sent (default is false)
//*****************************************************************************
unsigned fwElmbUser_getDoBytes(string argsElmb,
                               dyn_string &argdsExceptionInfo,
                               time argtTimeout = 2,
                               bool argbSetPreop = false)
{
// Local Variables
// ---------------
  unsigned uResult = 0;
  unsigned uPortA;
  unsigned uPortC;
  
  dyn_string dsExceptionInfo;
  
// Executable Code
// ---------------
  // Ensure system name is given
  argsElmb = dpSubStr(argsElmb, DPSUB_SYS_DP);
  
  // Call required functions to obtain the values
  uPortA = fwElmbUser_getDoByte(argsElmb, "A", dsExceptionInfo, argtTimeout, argbSetPreop);
  if (dynlen(dsExceptionInfo) > 0)
    dynAppend(argdsExceptionInfo, dsExceptionInfo);
  uPortC = fwElmbUser_getDoByte(argsElmb, "C", dsExceptionInfo, argtTimeout, argbSetPreop);
  if (dynlen(dsExceptionInfo) > 0)
    dynAppend(argdsExceptionInfo, dsExceptionInfo);
  
  // Create full value for return
  uResult = (uPortA << 8) | uPortC;
  
  // Return to calling routine
  return (uResult);
}

//*****************************************************************************
// @name Function: fwElmbUser_synchronizeDoBytes
//
// Reads values of the digtal output bytes of the ELMB specified and ensures
// that all 'set' values correspond correctly.
//
// @param argsElmb: DP name of ELMB (string) to synchronise.
//                  Hierarchy information is required, but system name is optional
// @param argdsExceptionInfo: Any exceptions are returned here
// @param argtTimeout: Optional argument for timeout (default is 2 seconds)
// @param argbSetPreop: Optional argument for whether to set ELMB to
//                      pre-operational before any SDOs sent (default is false)
//*****************************************************************************
void fwElmbUser_synchronizeDoBytes(string argsElmb,
                                   dyn_string &argdsExceptionInfo,
                                   time argtTimeout = 2,
                                   bool argbSetPreop = false)
{
// Local Variables
// ---------------
  unsigned uValue;
  
// Executable Code
// ---------------
  // Ensure system name is given
  argsElmb = dpSubStr(argsElmb, DPSUB_SYS_DP);
  
  // Get current settings
  uValue = fwElmbUser_getDoBytes(argsElmb, argdsExceptionInfo, argtTimeout, argbSetPreop);
  
  // Check for errors
  if (dynlen(argdsExceptionInfo) > 0)
    return;
  
  // Set all relevant output bits/bytes
  fwElmbUser_setDoBytes(argsElmb, uValue, argdsExceptionInfo);
  
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmbUser_updateFirmwareInfo
//
// Reads values of the firmware information for the ELMB specified. Updates the
// datapoint elements for the firmware version, software major and minor version
// and the serial number. Function starts a new thread so that it will not impact
// on the running of any other code.
//
// @param argsElmb: DP name of ELMB (string) to read.
//*****************************************************************************
void fwElmbUser_updateFirmwareInfo(string argsElmb)
{
  startThread("fwElmbUser_updateFirmwareThread", argsElmb);
}

// Actual thread function to update the firmware information
void fwElmbUser_updateFirmwareThread(string argsElmb)
{
  anytype aValue;
  dyn_string dsExceptionInfo;
  fwElmb_elementSQ(argsElmb + ".serialNumber", 2, aValue, dsExceptionInfo);
  fwElmb_elementSQ(argsElmb + ".hwVersion", 2, aValue, dsExceptionInfo);
  fwElmb_elementSQ(argsElmb + ".swVersion", 2, aValue, dsExceptionInfo);
  fwElmb_elementSQ(argsElmb + ".swMinorVersion", 2, aValue, dsExceptionInfo);
  return;
}

// ELMB Monitoring Functions
// =========================
//*****************************************************************************
// @name Function: fwElmbUser_monitorAll
//
// Connects to ALL required information in order to check for channel values which
// should be marked as invalid for all ELMBs and for OPC communication
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
void fwElmbUser_monitorAll()
{
// Local Variables
// ---------------
  int i;
  
  dyn_string dsElmbs;
  
// Executable Code
// ---------------
  // Call function to check OPC communication
  fwElmbUser_monitorOpc();
  
  // Get all ELMBs in the system
  dsElmbs = dpNames(getSystemName() + "*", ELMB_TYPE_NAME);
  
  // Loop through all ELMBs, connecting to required information
  for (i = 1; i <= dynlen(dsElmbs); i++)
    fwElmbUser_monitorElmb(dsElmbs[i]);
            
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmbUser_monitorElmb
//
// Connects to required information in order to check for channel values which
// should be marked as invalid
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
void fwElmbUser_monitorElmb(string argsElmb)
{
// Local Variables
// ---------------
// None
  
// Executable Code
// ---------------
  // Connect to required elements of the emergency message for each ELMB
  dpConnect("fwElmb_monitorElmbEMCbk",
            argsElmb + ".emergency.emergencyErrorCode",
            argsElmb + ".emergency.errorCodeByte1",
            argsElmb + ".emergency.errorCodeByte2",
            argsElmb + ".emergency.errorCodeByte3",
            argsElmb + ".emergency.errorCodeByte4",
            argsElmb + ".emergency.errorCodeByte5",
          argsElmb + ".emergency:_online.._stime");
  dpConnect("fwElmb_monitorElmbStateCbk",
            argsElmb + ".state.value");
            
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmbUser_monitorOpc
//
// Connects to required information in order to check for OPC connection for
// ELMB and marks relevant channel values as invalid if necessary
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
void fwElmbUser_monitorOpc()
{
// Local Variables
// ---------------
// None
  
// Executable Code
// ---------------
  // Connect to required elements for the OPC client/server
  dpConnect("fwElmb_monitorOpcCbk",
            ELMB_OPC_SERVER + ".Connected");
            
  // Return to calling routine
  return;
}

//*****************************************************************************
// @name Function: fwElmbUser_monitorInvalid
//
// Connects to required information in order to check for any channel being
// marked as invalid and sets a DPE for the respective node to allow for an
// alert to be activated.
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
void fwElmbUser_monitorInvalid()
{
// Local Variables
// ---------------
  dyn_string dsElmbs;
  dyn_string dsValidList;
  
// Executable Code
// ---------------
  // Get all ELMBs in this system
  dsElmbs = dpNames(getSystemName() + "*", ELMB_TYPE_NAME);
  
  // Get all analog input channels for each node in turn and set up the
  // connection to the invalidity flags
  for (int i = 1; i <= dynlen(dsElmbs); i++) {
    dsValidList = dpNames(dsElmbs[i] + "*.value:_online.._invalid", ELMB_AI_TYPE_NAME);
    if (dynlen(dsValidList) > 0)
      dpConnect("fwElmb_cbkMonitorInvalid", dsValidList);
  }
  
  // Return to calling routine
  return;
}
