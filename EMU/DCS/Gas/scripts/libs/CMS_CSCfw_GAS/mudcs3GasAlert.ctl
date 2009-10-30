
mudcsGasAlertCreateMain(bool isSet)
{
  
  mudcsInit();
  mudcsGasInit();
 
      
  GAS_alert_main();
 
}

//==========================================================

GAS_alert_main(bool isSet){



string device_type="fwGasSystem_CSC_GAS_d";
  GAS_alert_set(isSet,device_type);

  device_type="fwCooling_CSC_COOLING_d";
  COOLING_alert_set(isSet,device_type);

}
//==========================================================

//==========================================================
COOLING_alert_set(bool isSet, string device_type)
{

  
dyn_string limits_s;
string value;
dyn_string dps;
int i,j;
    

limits_s = makeDynString();

// s1 is OK
// s2 is ALARM
/*
Flowmeter_EndCAp_Cooling_YE_Moims_1
Flowmeter_EndCap_Cooling_YE_Plus_1
Flowmeter_Rack_EndCAp_Cooling_Far_Side_YE_Moins_1
Flowmeter_Rack_EndCAp_Cooling_Near_Side_YE_Moins_1
Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Plus_1
Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Plus_1
*/

dps=dpNames("*","device_type");
//==================================
for(i=1;i<=dynlen(dps);i++){

  
limits_s=makeDynString("0,100"); //
value=mudcsRemoveSystem(dps[i]+".Flowmeter_EndCAp_Cooling_YE_Moims_1");
mudcsGasAlertConfig(false, true, value, limits_s);

limits_s=makeDynString("0,100"); //
value=mudcsRemoveSystem(dps[i]+".Flowmeter_EndCap_Cooling_YE_Plus_1");
mudcsGasAlertConfig(false, true, value, limits_s);

limits_s=makeDynString("0,100"); //
value=mudcsRemoveSystem(dps[i]+".Flowmeter_Rack_EndCAp_Cooling_Far_Side_YE_Moins_1");
mudcsGasAlertConfig(false, true, value, limits_s);

limits_s=makeDynString("0,100"); //
value=mudcsRemoveSystem(dps[i]+".Flowmeter_Rack_EndCAp_Cooling_Near_Side_YE_Moins_1");
mudcsGasAlertConfig(false, true, value, limits_s);

limits_s=makeDynString("0,100"); //
value=mudcsRemoveSystem(dps[i]+".Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Plus_1");
mudcsGasAlertConfig(false, true, value, limits_s);

limits_s=makeDynString("0,100"); //
value=mudcsRemoveSystem(dps[i]+".Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Plus_1");
mudcsGasAlertConfig(false, true, value, limits_s);

} // for i  
  
  

}



//===================================================================================
//==========================================================
GAS_alert_set(bool isSet, string device_type)
{

  
dyn_string limits_s;
string value;
dyn_string dps;
int i,j;
    

limits_s = makeDynString();

// s1 is OK
// s2 is ALARM


dps=dpNames("*","device_type");
//==================================
for(i=1;i<=dynlen(dps);i++){

  
limits_s=makeDynString("0,4"); //
value=mudcsRemoveSystem(dps[i]+".state");
mudcsGasAlertConfig(false, true, value, limits_s);




} // for i  
  
  

}







//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

mudcsGasGroupAcknowlege(string groupDp){

dyn_string ds1;
int i;
dyn_string exceptionInfo;
dpGet(mudcsRemoveSystem(groupDp)+"."+":_alert_hdl.._dp_list",ds1);
//DebugTN(s1);
  for(i=1;i<=dynlen(ds1);i++){
   fwAlertConfig_acknowledge(ds1[i], exceptionInfo);
  }
}
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================


mudcsGasAlertConfig(bool isAlertChangeOnly, bool isAlertActive, string dpe, dyn_float limits, bool isAck=false)
{
// !!!!! 08/10/2004 Attention: this function is never called now with isAlertChangeOnly=true;
// !!!!! the alerts set active in the DeviceAlertSetting

int j;
string test_string;
string error_class;

if(isAck)
 error_class="_fwErrorAck";
else 
 error_class="_fwErrorNack";

////////// DebugN("mudcsCrbLV_1_alert_set:++++++++++++++++++++++++++++++++++"+" "+isAlertActive);
    bool alertRequest;
    int i, requestedRanges;
    int alertType, configType;  // +++++++++++++++++++++++

////////////    string dpe;
////////////    dyn_float limits = makeDynFloat();
    dyn_string alertClass, alertText, exceptionInfo;
//-------------------------
    dyn_string dpes, DPE_LIST;
    string DPE_TYPE;

//-------------------------
    // get state of checkbox
////////////////    alertRequest = alertOnOff.state(0);
      
   alertRequest = isAlertActive; // later:  to add one more parameter to routine: alertRequest

///////////////    Button1.enabled = FALSE;
    //Close.enabled = FALSE;
 
    // Convert list of dpes passed as a string to a dyn_string
///////////////    dpe = $sDpe;
    strreplace(dpe, " ", "");
    dpes = strsplit(dpe, "|");

//----------------------------------------------
   DPE_LIST=dpes;
         
// !!!!! 08/10/2004 Attention: this function is never called now with isAlertChangeOnly=true;
// !!!!! the alerts set active in the DeviceAlertSetting

   if(isAlertChangeOnly){

	for(i = 1; i <= dynlen(dpes); i++)
	{ 
          if(isAlertActive)
          dpSetWait(mudcsAddSystem( dpes[i] + ":_alert_hdl.._active"), true);
          else
          dpSetWait(mudcsAddSystem( dpes[i] + ":_alert_hdl.._active"), false);
            
	}
    return;

   } // if(isAlertChangeOnly)

//---------------------------------------------------


//++++++++++++++++++
	if(alertRequest)
	{
//---------------------------
            DPE_TYPE=dpElementType(dpes[1]); // instead of DPES_OK = _fwConfigs_checkDpeList(DPE_LIST, fwConfigs_PVSS_ALERT_HDL, DPE_TYPE, exceptionInfo);
//----------------------------
DebugTN("DPE_TYPE="+DPE_TYPE+" "+dpes[1]);
	    // read input data from the panel depending on the dpe type
		_fwConfigs_getConfigOptionsForDpeType(DPE_TYPE, fwConfigs_PVSS_ALERT_HDL, configType, exceptionInfo);
DebugTN("config_type="+configType+" "+fwConfigs_ANALOG_OPTIONS);                
		switch(configType)
		{
			case fwConfigs_ANALOG_OPTIONS:
				alertType = DPCONFIG_ALERT_NONBINARYSIGNAL;
/////////				requestedRanges = rangeCombo.text;
requestedRanges = dynlen(limits)+1;
	
				for (i = 1; i <= requestedRanges; i++)
				{ 
////////				getValue("alText" + i, "text", alertText[i]);
if(requestedRanges >=3 ){
 if(i%2)alertText[i]="error";
 else alertText[i]="";;
}
else if(requestedRanges ==2 ){
 if(i==2)alertText[i]="error";
 else alertText[i]="";;
}

///////					getValue("alClass" + i, "text", alertClass[i]);
//if(i!=2)alertClass[i]="_fwErrorNack";
if(requestedRanges >=3){
 if(i%2)alertClass[i]= error_class;//"_fwErrorNack";
 else alertClass[i]="";
}
else if(requestedRanges ==2 ){
 if(i==2)alertClass[i]= error_class;//"_fwErrorNack";
 else alertClass[i]="";
}
					if (alertClass[i] != "")
						alertClass[i] = alertClass[i] + ".";
				}
DebugTN("DPE_TYPE1122="+DPE_TYPE);     							
				// put the limits in a dyn_float 
///////				for (i = 1; i < requestedRanges; i++)
//////					getValue("limNum" + i, "text", limits[i]);
				break;
				
			case fwConfigs_BINARY_OPTIONS:	
				alertType = DPCONFIG_ALERT_BINARYSIGNAL;
                           if(limits[1]==1){   
                             alertText = makeDynString("OK", "STATUS ERROR");
//////				alertText = makeDynString(alText7.text, alText6.text);
//////	  	    
//////	   			// create alarm class list with valid range
//////				if (radio1.number == 1)
                             alertClass = makeDynString("", error_class + ".");
                           }
                           else{
                             alertText = makeDynString( "STATUS ERROR","OK");
                             alertClass = makeDynString(error_class + ".","");                             
                             
                           }    
//////					alertClass = makeDynString("", alClass3.text + ".");
//////				else if (radio1.number == 0)
//////					alertClass = makeDynString(alClass3.text + ".", ""); 
				break;
			default:
				break;
		}

///// DebugN(DPE_LIST[1]+".......................");
DebugTN("DPE_TYPE1133="+DPE_TYPE);     		
fwAlertConfig_setMultiple(DPE_LIST, alertType, alertText, limits, alertClass,
makeDynString(), "", makeDynString(),  "", exceptionInfo);
/*
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++11111");
DebugTN(DPE_LIST);
DebugTN("alertType="+alertType);
DebugTN(alertText);
DebugTN(limits);
DebugTN(alertClass);
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++11111");
*/

//for(j=1;j<=dynlen(DPE_LIST);j++){
//mudcsLvDebug2(DPE_LIST[j]);
//}

//for(j=1;j<=dynlen(limits);j++){
//test_string=limits[j];
//mudcsLvDebug2(test_string);
//}

//-----------------------------------------------------------------------------------------
	         for(i = 1; i <= dynlen(DPE_LIST); i++){
                  dpSetWait(mudcsAddSystem( DPE_LIST[i] + ":_alert_hdl.._active"), true);
	         }
//-----------------------------------------------------------------------------------------

	}
	else
	{
	 	fwAlertConfig_deleteMultiple(DPE_LIST, exceptionInfo);
	}
	
	if(dynlen(exceptionInfo) > 0)
		fwExceptionHandling_display(exceptionInfo);
//+++++++++++++++++++++++++++++++++++++


}
