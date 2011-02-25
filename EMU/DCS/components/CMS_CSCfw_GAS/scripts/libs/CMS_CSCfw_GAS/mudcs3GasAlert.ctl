//bool isStandalone=true;

mudcsGasAlertCreateMain(bool isSet)
{
//======  
 
 // if(isStandalone){
 // addGlobal("CSC_fwG_g_SYSTEM_NAME",STRING_VAR);
// CSC_fwG_g_SYSTEM_NAME = "CSC_MONITOR"; // fsm: "mudcs"
 
 // }
 // else 
    mudcsInit();
//=====
  mudcsGasInit();
 
      
  GAS_alert_main(isSet);
 
}

//==========================================================

GAS_alert_main(bool isSet){



string device_type="fwGasSystem_CSC_GAS_d";
  GAS_alert_set(isSet,device_type);

  Cooling_alert_main();
}
//==========================================================

//===================================================================================
Cooling_alert_main(){



 bool isAck=false;
 mudcsTurbinesInit();
 mudcsCoolingAlertSet(true, isAck);     
 mudcsCoolingTurbinesAlertSet(true,isAck) ;
 mudcsCoolingSummaryAlertSet(true);
 mudcsCoolingRacksSummary(true);       


}
//===================================================================================
mudcsCoolingAlertSet(bool isCreateConfig, bool isAck){
  
dyn_float limits_s;
string value;
dyn_string dps;
int i,j;

    
dyn_string cool_er_dpe=makeDynString(
//"mainOn",
//"mainInhibit",
//"localControlOnly",
"Flowmeter_EndCap_Cooling_YE_Minus_1",
"Flowmeter_EndCap_Cooling_YE_Plus_1",
"Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Minus_1",
"Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Minus_1",
"Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Plus_1",
"Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Plus_1");

limits_s = makeDynFloat();

// s1 is OK
// s2 is ALARM


dps=dpNames("*","fwCooling_CSC_COOLING_d");
//==================================
for(i=1;i<=dynlen(dps);i++){

  if(strpos(dps[i],"DimBroker")>=0)continue;  
DebugTN("=============="+dps[i]);
    
 DebugTN(">>>>> alerting ....."+dps[i]+" "+dynlen(cool_er_dpe));
  
for(j=1;j<=dynlen(cool_er_dpe);j++){  
  
  if(strpos(cool_er_dpe[j],"Side") <0 )limits_s=makeDynFloat(200,100000);// that gives an alarm if the value < 200 and >= 100000 
  else limits_s=makeDynFloat(20,100000); //// that gives an alarm if the value < 20 and >= 100000    
  
value=mudcsRemoveSystem(dps[i]+"."+cool_er_dpe[j]); 
DebugTN("alerting ....."+value);
mudcsAlertConfig(false, isCreateConfig, value, limits_s, isAck);  
  
} // j

//value=mudcsRemoveSystem(dps[i]+".Status.OutputFailure"); // commented as OutputFailure is always TRUE while the maraton works OK
//mudcsAlertConfig(false, isCreateConfig, value, limits_s, isAck);
//value=mudcsRemoveSystem(dps[i]+".Status.CrateStatus");
//mudcsAlertConfig(false, isCreateConfig, value, limits_s, isAck);


} // for i

}
//===================================================================================
//===================================================================================
//===================================================================================
mudcsCoolingTurbinesAlertSet(bool isCreateConfig, bool isAck){
  
dyn_float limits_s;
string value;
dyn_string dps;
int i,j;


limits_s = makeDynFloat();

// s1 is OK
// s2 is ALARM


dps=dpNames("*","fwCooling_CSC_TURBINES_data");
//==================================
for(i=1;i<=dynlen(dps);i++){

  if(strpos(dps[i],"DimBroker")>=0)continue;  
DebugTN("=============="+dps[i]);

  if(strpos(dps[i],"Current") <0 && strpos(dps[i],"Temperature") <0)continue; // TMP items not calibrated yet
    
  if(strpos(dps[i],"Current") <0)limits_s=makeDynFloat(10,32);// that gives an alarm if the value < 200 and >= 100000 
  else limits_s=makeDynFloat(0.5,1.4); //// that gives an alarm if the value < 20 and >= 100000     
  
//  if(strpos(dps[i],"Temperature") <0 && strpos(dps[i],"Temerature") <0)limits_s=makeDynFloat(0.5,1.4);// that gives an alarm if the value < 200 and >= 100000 
//  else limits_s=makeDynFloat(10,32); //// that gives an alarm if the value < 20 and >= 100000    
  
value=mudcsRemoveSystem(dps[i]+"."); 
DebugTN("alerting ....."+value);
mudcsAlertConfig(false, isCreateConfig, value, limits_s, isAck);  
  

/*
for(j=1;j<=dynlen(cool_er_dpe);j++){  
  
  if(strpos(cool_er_dpe[j],"Temperature") <0 )limits_s=makeDynFloat(0.5,1.4);// that gives an alarm if the value < 200 and >= 100000 
  else limits_s=makeDynFloat(10,32); //// that gives an alarm if the value < 20 and >= 100000    
  
value=mudcsRemoveSystem(dps[i]+"."+cool_er_dpe[j]); 
DebugTN("alerting ....."+value);
mudcsAlertConfig(false, isCreateConfig, value, limits_s, isAck);  
  
} // j
*/

//value=mudcsRemoveSystem(dps[i]+".Status.OutputFailure"); // commented as OutputFailure is always TRUE while the maraton works OK
//mudcsAlertConfig(false, isCreateConfig, value, limits_s, isAck);
//value=mudcsRemoveSystem(dps[i]+".Status.CrateStatus");
//mudcsAlertConfig(false, isCreateConfig, value, limits_s, isAck);


} // for i

}
//===================================================================================
//===================================================================================

//===================================================================================

mudcsCoolingSummaryAlertSet(bool isCreateConfig){
//==================================
//=== SUMMARY ALERT ================
//==================================

  
  
dyn_string dps, dps1, dps2,dps3,dps_t;
int i,j,i10;
dyn_string exceptionInfo, s1;  
string coord;

dyn_string cool_er_dpe=makeDynString(
//"mainOn",
//"mainInhibit",
//"localControlOnly",
"Flowmeter_EndCap_Cooling_YE_Minus_1",
"Flowmeter_EndCap_Cooling_YE_Plus_1",
"Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Minus_1",
"Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Minus_1",
"Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Plus_1",
"Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Plus_1");
 
dyn_string cool_er_dpe_t=makeDynString(
"Temperature2",
"Temperature1",
"TurbineCurrent1",
"TurbineCurrent2");

dps=dpNames("*","fwCooling_CSC_COOLING_d");
dps_t=dpNames("*","fwCooling_CSC_TURBINES_data");
//-----------------------------------
//------------------------------------------------------------  
int type;
  

//==================================
for(i=1;i<=dynlen(dps);i++){
  
 if(strpos(dps[i],"DimBroker")>=0)continue; 
  //  DebugTN("+=++  "+dps[i]);
//dps1=dpNames(dps[i]+".Status.*Failure*");  
 for(j=1;j<=dynlen(cool_er_dpe);j++){
  dpGet(dps[i]+"."+cool_er_dpe[j]+":_alert_hdl.._type",type);
  DebugTN(dps[i]+"."+cool_er_dpe[j]+":_alert_hdl.._type"+">>"+type);
  if(type > 0)dynAppend(dps2,dps[i]+"."+cool_er_dpe[j]);
 } //j

   for(i10=1;i10<=dynlen(dps_t);i10++){
    //for(j=1;j<=dynlen(cool_er_dpe_t);j++){
     dpGet(dps_t[i10]+"."+":_alert_hdl.._type",type);
     DebugTN(dps_t[i10]+"."+":_alert_hdl.._type"+">>"+type);
     if(type > 0)dynAppend(dps2,dps_t[i10]+".");
    //} //j
   }

//----------------------------------------------------------- 
  if(isCreateConfig){
   fwAlertConfig_createSummary( mudcsRemoveSystem(dps[i])+".",
   makeDynString("","") , dps2 ,"", makeDynString(),"",exceptionInfo);
   dpSet(mudcsRemoveSystem(dps[i])+"."+":_alert_hdl.._active", true);
  }
  else{
   fwAlertConfig_deleteSummary( mudcsRemoveSystem(dps[i])+".",exceptionInfo);        
  }

dyn_string s1;
//dpSetWait(mudcsRemoveSystem(dps[i])+"."+":_alert_hdl.._ack_state",DPATTR_ACKTYPE_SINGLE );//"_fwErrorAck");
dpGet(mudcsRemoveSystem(dps[i])+"."+":_alert_hdl.._dp_list",s1);
DebugTN(s1);    


//  dpSet(mudcsRemoveSystem(dps[i])+"."+":_alert_hdl.._active", true);
 } // for i    

// TestDP_1.element:_alert_hdl.1._class", alertclass[1],   

}
//===================================================================================
mudcsCoolingRacksSummary(bool isCreateConfig){
//==================================
//=== SUMMARY ALERT ================
//==================================
  
dyn_string dps, dps1, dps2,dps3,dps_t, dps_w;
int i,j,i10;
dyn_string exceptionInfo, s1;  
string coord;

dyn_string cool_er_dpe=makeDynString(
//"mainOn",
//"mainInhibit",
//"localControlOnly",
"Flowmeter_EndCap_Cooling_YE_Minus_1",
"Flowmeter_EndCap_Cooling_YE_Plus_1",
"Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Minus_1",
"Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Minus_1",
"Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Plus_1",
"Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Plus_1");


dps_w=dpNames("*","fwCooling_CSC_COOLING_d");
dps=dpNames("*","fwCooling_CSC_TURBINES_summary");
dps_t=dpNames("*","fwCooling_CSC_TURBINES_data");
//-----------------------------------
//------------------------------------------------------------  
int type;
dyn_string racks;
    
//==================================
for(i=1;i<=dynlen(dps);i++){

   // mudcsDebug(dps[i]);      
  dynClear(dps2);
  dynClear(racks);  
 if(strpos(dps[i],"DimBroker")>=0)continue;
 if(strpos(dps[i],"mrtn_racks")>=0)racks=turbine_crates; 
 else  if(strpos(dps[i],"hv_racks")>=0)racks=hv_racks;
 else  if(strpos(dps[i],"pc_racks")>=0)racks=pc_racks;
 else  if(strpos(dps[i],"align_racks")>=0)racks=align_racks; 

  
 
 if(dynlen(racks)>=1){ 
  for(j=1;j<=dynlen(racks);j++){
   for(i10=1;i10<=dynlen(dps_t);i10++){    
    if(strpos(dps_t[i10],racks[j])>=0)dynAppend(dps2,dps_t[i10]+".");
  //   DebugTN("++++++++++++"+dps_t[i10]); 
   } // i10
  }  // j
 
 }
 else if(strpos(dps[i],"water")>=0){ 
   // water
  for(i10=1;i10<=dynlen(dps_w);i10++){ 
   if(strpos(dps_w[i10],"DimBroker")>=0)continue;       
  for(j=1;j<=dynlen(cool_er_dpe);j++){
   dpGet(dps_w[i10]+"."+cool_er_dpe[j]+":_alert_hdl.._type",type);
   DebugTN(dps_w[i10]+"."+cool_er_dpe[j]+":_alert_hdl.._type"+">>"+type);
   if(type > 0)dynAppend(dps2,dps_w[i10]+"."+cool_er_dpe[j]);
  } //j   
  } // i10
 } // else
 
  if(isCreateConfig){
  DebugTN(dps2);   
  DebugTN(dps[i]);
  if(dynlen(dps2)<=0)continue;
  
   fwAlertConfig_createSummary( mudcsRemoveSystem(dps[i])+".",
   makeDynString("","") , dps2 ,"", makeDynString(),"",exceptionInfo);
   dpSet(mudcsRemoveSystem(dps[i])+"."+":_alert_hdl.._active", true);
  }
  else{
   fwAlertConfig_deleteSummary( mudcsRemoveSystem(dps[i])+".",exceptionInfo);        
  }

dyn_string s1;
//dpSetWait(mudcsRemoveSystem(dps[i])+"."+":_alert_hdl.._ack_state",DPATTR_ACKTYPE_SINGLE );//"_fwErrorAck");
dpGet(mudcsRemoveSystem(dps[i])+"."+":_alert_hdl.._dp_list",s1);
DebugTN(s1);      
  
  
} // i

//----------------------------------------------------------- 



//  dpSet(mudcsRemoveSystem(dps[i])+"."+":_alert_hdl.._active", true);
 } // for i    

// TestDP_1.element:_alert_hdl.1._class", alertclass[1],   


//===================================================================================


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


//===================
mudcsCoolingChannelExclude(string box_name, bool isEnabled){
  
  
     mudcsGasInit();
   mudcsTurbinesInit();  

dyn_string off_channels;
int pos;
bool isexclude =! isEnabled;
dyn_string split1=strsplit(m_exclude[box_name],"/"); 
string rack=split1[dynlen(split1)-1];

dpGet(DpName+".off_channels",off_channels);

 if(isexclude){
  if(dynContains(off_channels,m_exclude[box_name])==0)dynAppend(off_channels,m_exclude[box_name]);
  //DebugTN(off_channels);
 }
 else{
  pos=dynContains(off_channels,m_exclude[box_name]);
  if(pos)dynRemove(off_channels,pos);

 }
 dpSetWait(DpName+".off_channels",off_channels);
 mudcsCoolingChannelExclude_alert_reconfig(m_exclude[box_name], isexclude); 
 /*
  mudcsCoolingChannelExclude_alert_reconfig(DpName,m_exclude[box_name], isexclude);   


  string dpname;
  if(dynContains(align_racks,rack)>0)dpname=mudcsGasAddSystem("align_racks_summary");
  else if(dynContains(pc_racks,rack)>0)dpname=mudcsGasAddSystem("pc_racks_summary");
  else if(dynContains(turbine_crates,rack)>0)dpname=mudcsGasAddSystem("mrtn_racks_summary");
  else if(dynContains(hv_racks,rack)>0)dpname=mudcsGasAddSystem("hv_racks_summary");
 // else if(dynContains(,m_exclude[box_name])>=0)dpname=mudcsGasAddSystem("water_summary");
  

  
  mudcsCoolingChannelExclude_alert_reconfig(dpname,m_exclude[box_name], isexclude);    
  
  dpSetWait(mudcsGasAddSystem("cooling_alert_update.value"),"update");
  */
 
}

//====================================
mudcsCoolingChannelExclude_alert_reconfig(string dpname, int isexclude){
 if(!isexclude){
   dpSetWait(mudcsGasAddSystem( dpname + ":_alert_hdl.._active"), true);
 }
 else{
   dpSetWait(mudcsGasAddSystem( dpname + ":_alert_hdl.._active"),false);
   
 } 
}

//=====================================
mudcsCoolingChannelExclude_alert_reconfig_old(string dpname, string dp_frag, int isexclude){

bool configExists;
dyn_string alertTexts;
dyn_string ilist;
string alertPanel;
dyn_string alertPanelParameters;
string alertHelp;
bool isActive;
dyn_string exceptionInfo;
int pos;

int i,j;

fwAlertConfig_getSummary(dpname+".",configExists,alertTexts,ilist,
	alertPanel,alertPanelParameters,alertHelp,isActive,exceptionInfo);

//DebugTN(dp_frag+"=========");

if(!isexclude){
  if(dynContains(ilist,dp_frag) == 0)dynAppend(ilist,dp_frag);
 
// DebugTN(ilist);
// mudcsDebug(dp_frag);
}
else{
 if((pos=dynContains(ilist,dp_frag)) > 0)dynRemove(ilist,pos);
//DebugTN(ilist);
 
}

//=====
							
 fwAlertConfig_createSummary(dpname+".",makeDynString("","") , ilist ,"", 
                              makeDynString(),"",exceptionInfo);

 /* 
fwAlertConfig_getSummary(dpname+".",configExists,alertTexts,ilist,
	alertPanel,alertPanelParameters,alertHelp,isActive,exceptionInfo);

 if((pos=dynContains(ilist,dp_frag)) > 0)mudcsDebug("V");
 */
//======

}
//====================================


