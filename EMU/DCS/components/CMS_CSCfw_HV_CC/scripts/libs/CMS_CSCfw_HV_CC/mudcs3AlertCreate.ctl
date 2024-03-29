#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_alert.ctl"
#uses "CMS_CSCfw_HV_CC/emuAlert.ctl"

const float MUDCS_G_ALERT_DEFAULT_TEMPERATURE = 25;

string DpNameStartForAlertConfig;
mudcsAlertCreateMain()
{
  
  mudcsInit();
  
  DpNameStartForAlertConfig=CSC_fwG_g_csc_part;

  FED_1_alert_main(true);    
  HV_1_alert_main();
  CRB_1_alert_main();
  MRTN_1_alert_main();
  LV_1_alert_main();
  TEMP_1_alert_main();  
  Snmp_alert_main();
 // should be called in GAS/Cooling component: Cooling_alert_main();  
  
    mudcsAlertReconfig("TEMP_1", ".off_channels", true);

    mudcsAlertReconfig("HV_1", ".off_channels", true);

    mudcsAlertReconfig("LV_1", ".noalert_channels", true);  
  mudcsAlertReconfig("LV_1;.data.Cfeb_o.v33", ".noalert_channels", true);
  mudcsAlertReconfig("LV_1;.data.Cfeb_o.v50", ".noalert_channels", true); 
  mudcsAlertReconfig("LV_1;.data.Cfeb_o.v60", ".noalert_channels", true); 
  mudcsAlertReconfig("LV_1;.data.Cfeb_o.c33", ".noalert_channels", true);
  mudcsAlertReconfig("LV_1;.data.Cfeb_o.c50", ".noalert_channels", true); 
  mudcsAlertReconfig("LV_1;.data.Cfeb_o.c60", ".noalert_channels", true);
 
  fwLvCr_CSC_TEMP_alert_main(true);  
  
  emu_info("Updating all alert classes");
  emuAlert_updateAllAlertClasses();
  emu_info("Done updating alert classes");
}

//CSC_fwG_g_csc_part


float v18,v33,v55,v56;
float c18,c33,c55,c56;
float v33_c, v50_c, v60_c;
float c33_c, c50_c, c60_c;
float rel_delta_volt, rel_delta_curr;
float rel_delta_volt_c, rel_delta_curr_c;

//-------------------------
float dmb_min, dmb_max;
float alct_min, alct_max;
float cfebs_min, cfebs_max;
//-------------------------
//===================================================================================

fwLvCr_CSC_TEMP_alert_main(bool isSet){



 bool isAck=false;
 mudcsfwLvCr_CSC_TEMPSummaryAlertSet(isSet);
       


}

//===================================================================================
//===================================================================================
mudcsfwLvCr_CSC_TEMPSummaryAlertSet(bool isCreateConfig){
//==================================
//=== SUMMARY ALERT ================
//==================================


dyn_string dps, dps1, dps2, dps3, side_stations;
int i,j,i10 , p1,p2;
dyn_string exceptionInfo, s1;  
string coord, side_station;

//dyn_string fed_er_dpe=makeDynString(
//".data.v15",".data.v25a",".data.v25b",".data.v33",
//".data.td1",".data.td2",".data.td3",".data.td4"
//);

dyn_string temp_er_dpe=makeDynString(
"data.t_dmb.v1"
);

dps1=dpNames("*","TEMP_1_d");

for(i=1;i<=dynlen(dps1);i++){
   p1=strpos(dps1[i],"ME_");
   p2=strpos(dps1[i],"_C");
  if(p1>=0 && p2>=0){
   side_station= substr(dps1[i],p1+3,p2-1-(p1+3) );
    if(!dynContains(side_stations,side_station))dynAppend(side_stations,side_station);
  } 
} // i


dps=dpNames("*","fwLvCr_CSC_TEMP");
for(i=1;i<=dynlen(dps);i++){
  dpDelete(dps[i]);
}

for(i=1;i<=dynlen(side_stations);i++){
 
    for(j=1;j<=6;j++){ 
       if(strpos(side_stations[i],"1")>=0){
     dpCreate(side_stations[i]+"_PC"+j+"A","fwLvCr_CSC_TEMP"); 
     dpCreate(side_stations[i]+"_PC"+j+"B","fwLvCr_CSC_TEMP");
       }
      else{
     dpCreate(side_stations[i]+"_PC"+j,"fwLvCr_CSC_TEMP"); 
      }        
    }    
  

} // i

dps=dpNames("*","fwLvCr_CSC_TEMP");

//-----------------------------------
//------------------------------------------------------------  
int type;

//==================================
for(i=1;i<=dynlen(dps);i++){
  dynClear(dps2);
  
 dps3=CSC_fwG_g_PCRATES_MAP[mudcsRemoveSystem(dps[i])];
 if(strpos(dps[i],"DimBroker")>=0)continue; 
  //  DebugTN("+=++  "+dps[i]);
//dps1=dpNames(dps[i]+".Status.*Failure*");  
 for(i10=1;i10<=dynlen(dps3);i10++){ 
 for(j=1;j<=dynlen(temp_er_dpe);j++){
  dpGet(dps3[i10]+"."+temp_er_dpe[j]+":_alert_hdl.._type",type);
  DebugTN(dps3[i10]+"."+temp_er_dpe[j]+":_alert_hdl.._type"+">>"+type);
  if(type > 0)dynAppend(dps2,dps3[i10]+"."+temp_er_dpe[j]);
 } //j
 } // i10
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
//===================================================================================
//===================================================================================

FED_1_alert_main(bool isSet){



 bool isAck=false;
 mudcsFedAlertSet(isSet, isAck);     
 mudcsFedSummaryAlertSet(isSet);
       


}
//===================================================================================
mudcsFedAlertSet(bool isCreateConfig, bool isAck){
  
  

  
  
dyn_float limits_s;
string value;
dyn_string dps;
int i,j;

    
dyn_string fed_er_dpe=makeDynString(
"data.v15","data.v25a","data.v25b","data.v33",
"data.td1","data.td2","data.td3","data.td4"
);

limits_s = makeDynFloat();

// s1 is OK
// s2 is ALARM


dps=dpNames("*","FED_1_d");
//==================================
for(i=1;i<=dynlen(dps);i++){

  if(strpos(dps[i],"DimBroker")>=0)continue;  
DebugTN("=============="+dps[i]);
    
 DebugTN(">>>>> alerting ....."+dps[i]+" "+dynlen(fed_er_dpe));
  
for(j=1;j<=dynlen(fed_er_dpe);j++){  
  
  if(strpos(fed_er_dpe[j],"v")>=0){
   string volt=substr(fed_er_dpe[j],strpos(fed_er_dpe[j],"v")+1,2);
   float volt_f=volt;
   volt_f=volt_f/10.;
  limits_s=makeDynFloat(volt_f-volt_f*0.2,volt_f+volt_f*0.2);// that gives an alarm if the value < v1 and >= v2    
  }
  else if(strpos(fed_er_dpe[j],"t")>=0){
  limits_s=makeDynFloat(0,35);// that gives an alarm if the value < v1 and >= v2    
  }
  
value=mudcsRemoveSystem(dps[i]+"."+fed_er_dpe[j]); 
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
//===================================================================================
mudcsFedSummaryAlertSet(bool isCreateConfig){
//==================================
//=== SUMMARY ALERT ================
//==================================

  
  
dyn_string dps, dps1, dps2, dps3;
int i,j,i10;
dyn_string exceptionInfo, s1;  
string coord;

//dyn_string fed_er_dpe=makeDynString(
//".data.v15",".data.v25a",".data.v25b",".data.v33",
//".data.td1",".data.td2",".data.td3",".data.td4"
//);

dyn_string fed_er_dpe=makeDynString(
"data.v15","data.v25a","data.v25b","data.v33",
"data.td1","data.td2","data.td3","data.td4"
);
dps=dpNames("*","FED_1_d");

//-----------------------------------
//------------------------------------------------------------  
int type;
  

//==================================
for(i=1;i<=dynlen(dps);i++){
  
 dynClear(dps2); 
 if(strpos(dps[i],"DimBroker")>=0)continue; 
  //  DebugTN("+=++  "+dps[i]);
//dps1=dpNames(dps[i]+".Status.*Failure*");  
 for(j=1;j<=dynlen(fed_er_dpe);j++){
  dpGet(dps[i]+"."+fed_er_dpe[j]+":_alert_hdl.._type",type);
  DebugTN(dps[i]+"."+fed_er_dpe[j]+":_alert_hdl.._type"+">>"+type);
  if(type > 0)dynAppend(dps2,dps[i]+"."+fed_er_dpe[j]);
 } //j



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
//===================================================================================
//===============================================================================
LV_1_alert_main()
{



mudcsGetAlctAlert(v18,v33,v55,v56,rel_delta_volt,c18,c33,c55,c56,rel_delta_curr);

mudcsGetCfebAlert(v33_c,v50_c,v60_c,rel_delta_volt_c,c33_c,c50_c,c60_c,rel_delta_curr_c);

lv_set(true);
lv_setValidRandgeAndDefaultValue();

}
//=======================================

void lv_setValidRandgeAndDefaultValue() {
  dyn_string dpes = dpNames("Db_LV_1_alert_o.LV_1_default.**:_original.._value", "Db_LV_1_alert");
  emu_info(dpes);
  
  for (int i=1; i <= dynlen(dpes); i++) {
    float defaultValue;
    dpGet(dpes[i], defaultValue);
    if (defaultValue == 0) {
      continue;
    }
    dpes[i] = dpSubStr(dpes[i], DPSUB_SYS_DP_EL);
    strreplace(dpes[i], "Db_LV_1_alert_o.LV_1_default", "*.data");
    dyn_string dataDpes = dpNames(dpes[i], "LV_1_d");
    
    for (int j=1; j <= dynlen(dataDpes); j++) {
       dpSetWait(dataDpes[j] + ":_pv_range.._type", DPCONFIG_MINMAX_PVSS_RANGECHECK,
                 dataDpes[j] + ":_pv_range.._min", -1,
                 dataDpes[j] + ":_pv_range.._max", maxFLOAT(),
                 dataDpes[j] + ":_default.._type", DPCONFIG_DEFAULTVALUE,
                 dataDpes[j] + ":_default.._value", defaultValue,
                 dataDpes[j] + ":_default.._set_pvrange", true);                 
    }
  }
}

//=======================================

lv_set(bool isSet)
{

dyn_string limits_s;
string device_type, value;
string s1,s2;
//bool isGeneralSetting;

	dyn_string exceptionInfo; 

device_type="LV_1";

//isGeneralSetting = true;
//if(strpos(DpNameStartForAlertConfig,device_type) >= 0 )isGeneralSetting = false;


limits_s = makeDynString();



value=    ".data.Alct_o.v18|"
         +".data.Alct_o.v33|"
         +".data.Alct_o.v55|"
         +".data.Alct_o.v56|"
         +".data.Alct_o.c18|"
         +".data.Alct_o.c33|"
         +".data.Alct_o.c55|"
         +".data.Alct_o.c56|"

         +".data.Cfeb_o.v33|"
         +".data.Cfeb_o.v50|"
         +".data.Cfeb_o.v60|"        
         +".data.Cfeb_o.c33|"
         +".data.Cfeb_o.c50|"
         +".data.Cfeb_o.c60|"
         +".data.Lvdb_o.v7Analog|"
         +".data.Lvdb_o.v7Digital";

if(rel_delta_volt == 0 || rel_delta_curr == 0){
  mudcsDebug("Interval should be > 0");
  return;

}

if(rel_delta_volt_c == 0 || rel_delta_curr_c == 0){
  mudcsDebug("Interval should be > 0");
  return;
}


s1=v18-v18*rel_delta_volt;
s2=v18+v18*rel_delta_volt;
dynAppend(limits_s,s1+"|"+s2);
s1=v33-v33*rel_delta_volt;
s2=v33+v33*rel_delta_volt;
dynAppend(limits_s,s1+"|"+s2);
s1=v55-v55*rel_delta_volt;
s2=v55+v55*rel_delta_volt;
dynAppend(limits_s,s1+"|"+s2);
s1=v56-v56*rel_delta_volt;
s2=v56+v56*rel_delta_volt;
dynAppend(limits_s,s1+"|"+s2);

float min_range=0.6;

if(c18*rel_delta_curr > min_range){
s1=c18-c18*rel_delta_curr; 
s2=c18+c18*rel_delta_curr;
}
else {
if(c18-min_range < 0)s1=0.075; else s1= c18-min_range;
s2=c18+min_range; 
}
dynAppend(limits_s,s1+"|"+s2);

if(c33*rel_delta_curr > min_range){
s1=c33-c33*rel_delta_curr;
s2=c33+c33*rel_delta_curr;
}
else {
if(c33-min_range < 0)s1=0; else s1= c33-min_range;
s2=c33+min_range; 
}

dynAppend(limits_s,s1+"|"+s2);
s1=c55-c55*rel_delta_curr;
s2=c55+c55*rel_delta_curr;
dynAppend(limits_s,s1+"|"+s2);
s1=c56-c56*rel_delta_curr;
s2=c56+c56*rel_delta_curr;
dynAppend(limits_s,s1+"|"+s2);

//----------

s1=v33_c-v33_c*rel_delta_volt_c;
s2=v33_c+v33_c*rel_delta_volt_c;
dynAppend(limits_s,s1+"|"+s2);
s1=v50_c-v50_c*rel_delta_volt_c;
s2=v50_c+v50_c*rel_delta_volt_c;
dynAppend(limits_s,s1+"|"+s2);
s1=v60_c-v60_c*rel_delta_volt_c;
s2=v60_c+v60_c*rel_delta_volt_c;
dynAppend(limits_s,s1+"|"+s2);

if(c33_c*rel_delta_curr_c > min_range){
s1=c33_c-c33_c*rel_delta_curr_c;
s2=c33_c+c33_c*rel_delta_curr_c;
}
else {
if(c33_c-min_range < 0)s1=0.075; else s1= c33_c-min_range;
s2=c33_c+min_range; 
}
dynAppend(limits_s,s1+"|"+s2);

s1=c50_c-c50_c*rel_delta_curr_c;
s2=c50_c+c50_c*rel_delta_curr_c;
dynAppend(limits_s,s1+"|"+s2);
s1=c60_c-c60_c*rel_delta_curr_c;
s2=c60_c+c60_c*rel_delta_curr_c;
dynAppend(limits_s,s1+"|"+s2);

// LVDB limits
  float v7AnalogDelta, v7DigitalDelta;
  dpGet("Db_LV_1_alert_o.LV_1_delta.Lvdb_o.v7Analog", v7AnalogDelta,
        "Db_LV_1_alert_o.LV_1_delta.Lvdb_o.v7Digital", v7DigitalDelta);
  s1 = 7 - v7AnalogDelta;
  s2 = 7 + v7AnalogDelta;
  dynAppend(limits_s,s1+"|"+s2);
  s1 = 7 - v7DigitalDelta;
  s2 = 7 + v7DigitalDelta;
  dynAppend(limits_s,s1+"|"+s2);

////mudcsDebug(DpNameStartForAlertConfig);
////mudcsDebug(device_type);

mudcsLookAndAlertArchiveConfig(true, false, isSet, device_type, value, limits_s, DpNameStartForAlertConfig,
"",0,0,0.,0.);

mudcsSetAlctAlert(v18,v33,v55,v56,rel_delta_volt,c18,c33,c55,c56,rel_delta_curr);
mudcsSetCfebAlert(v33_c,v50_c,v60_c,rel_delta_volt_c,c33_c,c50_c,c60_c,rel_delta_curr_c);

//if(isGeneralSetting){
/////mudcsSetAlctDefaults(v18,v33,v55,v56);
/////mudcsSetAlctDeltas(v18_d,v33_d,v55_d,v56_d);
//}


}

//==========================================================
//==========================================================


TEMP_1_alert_main()
{


mudcsGetAlertTemperatures(dmb_min, alct_min, cfebs_min, dmb_max, alct_max, cfebs_max);

temp_set(true,"TEMP_1");
temp_setValidRandgeAndDefaultValue();

}
//==========================================================

void temp_setValidRandgeAndDefaultValue() {
  dyn_string tempDpes = dpNames("CSC_ME_*.data.**:_original.._value", "TEMP_1_d");
  for (int j=1; j <= dynlen(tempDpes); j++) {
     tempDpes[j] = dpSubStr(tempDpes[j], DPSUB_SYS_DP_EL);
     dpSetWait(tempDpes[j] + ":_pv_range.._type", DPCONFIG_MINMAX_PVSS_RANGECHECK,
               tempDpes[j] + ":_pv_range.._min", -1,
               tempDpes[j] + ":_pv_range.._max", maxFLOAT(),
               tempDpes[j] + ":_default.._type", DPCONFIG_DEFAULTVALUE,
               tempDpes[j] + ":_default.._value", MUDCS_G_ALERT_DEFAULT_TEMPERATURE,
               tempDpes[j] + ":_default.._set_pvrange", true);                 
  }
}

//=================================================================
temp_set(bool isSet, string device_type)
{

dyn_string limits_s;
string value;
string s1,s2;
//bool isGeneralSetting;



//isGeneralSetting = true;
//if(strpos(DpNameStartForAlertConfig,device_type) >= 0 )isGeneralSetting = false;


limits_s = makeDynString();



value=".data.t_dmb.v1|"
         +".data.t_alct.v1|"
         +".data.t_cfeb.v1|"
         +".data.t_cfeb.v2|"
         +".data.t_cfeb.v3|"
         +".data.t_cfeb.v4|"
         +".data.t_cfeb.v5";

//if(v18_d == 0 || v33_d == 0 || v55_d == 0 || v56_d == 0){
//  mudcsDebug("Interval should be > 0");
//  return;
//}

s1=dmb_min;
s2=dmb_max;
dynAppend(limits_s,s1+"|"+s2);
s1=alct_min;
s2=alct_max;
dynAppend(limits_s,s1+"|"+s2);
 for(int i=1;i<=5;i++){
  s1=cfebs_min;
  s2=cfebs_max;
  dynAppend(limits_s,s1+"|"+s2);
 }


mudcsLookAndAlertArchiveConfig(true, 
false, isSet, device_type, value, limits_s, DpNameStartForAlertConfig, "",0,0,0.,0.);

mudcsSetAlertTemperatures(dmb_min, alct_min, cfebs_min, dmb_max, alct_max, cfebs_max);

}

//==========================================================

HV_1_alert_main(){



string device_type="HV_1";
  hv_set_d(true,device_type);
string device_type1="HV_PR";
  hv_set_pr(true,device_type1);


}
//==========================================================



hv_set_d(bool isSet, string device_type)
{

int i;

dyn_string limits_s;
string value;
string s1,s2;

limits_s = makeDynString();

// s1 is OK
// s2 is ALARM

s1="0";
s2="4";

//	HV_STATE_ILOCK		= 5,
//	HV_STATE_INT_TRIP	= 6,
//	HV_STATE_CHAN_TRIP	= 7,

for(i=1;i<=36;i++){

value=value+".data.v"+i+".status|";
//// if(i<36)value=value+".data.v"+i+".status|";
//// else value=value+".data.v"+i+".status"; 

 dynAppend(limits_s,s1+"|"+s2);

//-----------------------------------------------
/*
 // not necessary: if we switch on the channel it bacomes DEAD, also the master channel is DEAD 
 
//if(i>=19)continue; // this is pack data (two channels in one value) 
 value+=".data.pack_data.v"+i+".vset|"; // to set alarm if somebody switchs off (or power cycle) the LVPS (Wiener LVPC)

s1="1";   //  ok
s2="5000";   // alarm 

 dynAppend(limits_s,s1+"|"+s2);

*/ 
//------------------------------------------------  
 
}


value=value+".data.module_status|"; 
dynAppend(limits_s,"-2|5");  // -2 is OK
                            // 5 is alarm


value=value+".data.module_type"; 
dynAppend(limits_s,"1|4");  // 1 is OK
                            // 4 is alarm

mudcsLookAndAlertArchiveConfig(true, 
false, isSet, device_type, value, limits_s, DpNameStartForAlertConfig, "",0,0,0.,0.);




}

//==========================================================
//==========================================================
hv_set_pr(bool isSet, string device_type)
{

int i;

dyn_string limits_s;
string value;
string s1,s2;

limits_s = makeDynString();

//-------
s1="-1";   //  ok
s2="100000";   // alarm 


value=value+".data.imon|"; 


 dynAppend(limits_s,s1+"|"+s2);
//------
/*
value+=".data.output|";

s1="1";   //  ok
s2="100000";   // alarm 

 dynAppend(limits_s,s1+"|"+s2);
*/
//-----
 value+=".data.vset|"; // to set alarm if somebody switchs off (or power cycle) the LVPS (Wiener LVPC)

s1="1";   //  ok
s2="5000";   // alarm 

 dynAppend(limits_s,s1+"|"+s2);
 
value+=".status|";

s1="0";   //  ok
s2="4";   // alarm 

 dynAppend(limits_s,s1+"|"+s2);

mudcsLookAndAlertArchiveConfig(true, 
false, isSet, device_type, value, limits_s, DpNameStartForAlertConfig, "",0,0,0.,0.);




}

//=========================================================
//==========================================================
MRTN_1_alert_main(){


string device_type="MRTN_1";
  mrtn_set_d(true,device_type);



}

//=========================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
Cooling_alert_main(){



 bool isAck=false;
 mudcsCoolingAlertSet(true, isAck);     
 mudcsCoolingSummaryAlertSet(true);
       


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
//===================================================================================

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

//===================================================================================
//===================================================================================
//===================================================================================

mudcsCoolingSummaryAlertSet(bool isCreateConfig){
//==================================
//=== SUMMARY ALERT ================
//==================================

  
  
dyn_string dps, dps1, dps2, dps3;
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
 


dps=dpNames("*","fwCooling_CSC_COOLING_d");

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
//===================================================================================
//===================================================================================
//===================================================================================
Snmp_alert_main(){



 bool isAck=false;
 mudcsWnrCrateAlertSet(true, isAck);
 mudcsWnrChannelAlertSet(true, isAck);     
 mudcsWnrSummaryAlertSet(true);
       


}
//==========================================================




//==========================================================
//==========================================================
//==========================================================
mrtn_set_d(bool isSet, string device_type)
{

int i;

dyn_string limits_s;
string value;
//string s1,s2;

limits_s = makeDynString();

// s1 is OK
// s2 is ALARM

dyn_string s1, s2;

s1[1]="1";
s2[1]="2";

s1[2]="0";
s2[2]="1";

s1[3]="1";
s2[3]="2";

s1[4]="0";
s2[4]="1";

s1[5]="0";
s2[5]="1";

s1[6]="0";
s2[6]="1";

///s1[7]="1";
///s2[7]="2";

for(i=1;i<=6;i++){

value=value+".data.crate.item"+i+"|";
 
 dynAppend(limits_s,s1[i]+"|"+s2[i]);

}




mudcsLookAndAlertArchiveConfig(true, 
false, isSet, device_type, value, limits_s, DpNameStartForAlertConfig, "",0,0,0.,0.);




}

//==========================================================
//==========================================================
//==========================================================


dyn_string cr_er_dpe;
                             
                             
dyn_string ch_er_dpe;





//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
mudcsWnrCrateAlertSet(bool isCreateConfig, bool isAck){
  
  

  
  
dyn_string limits_s;
string value;
dyn_string dps;
int i,j;

cr_er_dpe=makeDynString(
//"mainOn",
//"mainInhibit",
//"localControlOnly",
"inputFailure",
"outputFailure",
"fantrayFailure",
"sensorFailure",
"VmeSysfail");
    

limits_s = makeDynString();

// s1 is OK
// s2 is ALARM


dps=dpNames("*","fwWnrCr_CSC_LV_d");
//==================================
for(i=1;i<=dynlen(dps);i++){

  if(strpos(dps[i],"DimBroker")>=0)continue;  
DebugTN("=============="+dps[i]);
    
 DebugTN(">>>>> alerting ....."+dps[i]+" "+dynlen(cr_er_dpe));
  
for(j=1;j<=dynlen(cr_er_dpe);j++){  
  
  if(cr_er_dpe[j]=="mainOn")limits_s=makeDynString("1,100");// that gives an alarm if the value < 1 and >= 100 
  else limits_s=makeDynString("1"); // that gives an alarm if the value >= 1  
  
value=mudcsRemoveSystem(dps[i]+"."+cr_er_dpe[j]); 
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
//===================================================================================
mudcsWnrChannelAlertSet(bool isCreateConfig, bool isAck){
  
  
dyn_string limits_s;
string value;
dyn_string dps;
int i,j;
    
ch_er_dpe=makeDynString(
//"out_switch",
//"outputOn",
//"outputInhibit",
"outputFailureMinSenseVoltage",
"outputFailureMaxSenseVoltage",
"outputFailureMaxTerminalVoltage",
"outputFailureMaxCurrent",
"outputFailureMaxTemperature",
"outputFailureMaxPower");

limits_s = makeDynString();

// s1 is OK
// s2 is ALARM


dps=dpNames("*","fwWnrCh_CSC_LV_d");
//==================================
for(i=1;i<=dynlen(dps);i++){
   if(strpos(dps[i],"DimBroker")>=0)continue; 
     
for(j=1;j<=dynlen(ch_er_dpe);j++){

  if(ch_er_dpe[j]=="out_switch") limits_s=makeDynString("1,100"); // that gives an alarm if the value < 1 and >= 100 
  else limits_s=makeDynString("1"); // that gives an alarm if the value >= 1
value=mudcsRemoveSystem(dps[i]+"."+ch_er_dpe[j]);
DebugTN("alerting ....."+value); 
mudcsAlertConfig(false, isCreateConfig, value, limits_s, isAck);  
  
} // j
   


} // for i

}

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
mudcsWnrSummaryAlertSet(bool isCreateConfig){
//==================================
//=== SUMMARY ALERT ================
//==================================

  
  
dyn_string dps, dps1, dps2, dps3;
int i,j,i10;
dyn_string exceptionInfo, s1;  
string coord;

cr_er_dpe=makeDynString(
//"mainOn",
//"mainInhibit",
//"localControlOnly",
"inputFailure",
"outputFailure",
"fantrayFailure",
"sensorFailure",
"VmeSysfail");
 
ch_er_dpe=makeDynString(
//"out_switch",
//"outputOn",
//"outputInhibit",
"outputFailureMinSenseVoltage",
"outputFailureMaxSenseVoltage",
"outputFailureMaxTerminalVoltage",
"outputFailureMaxCurrent",
"outputFailureMaxTemperature",
"outputFailureMaxPower");

dps=dpNames("*","fwWnrCr_CSC_LV_d");

//-----------------------------------
//------------------------------------------------------------  
int type;
  

//==================================
for(i=1;i<=dynlen(dps);i++){
  
 if(strpos(dps[i],"DimBroker")>=0)continue; 
  //  DebugTN("+=++  "+dps[i]);
//dps1=dpNames(dps[i]+".Status.*Failure*");  
 for(j=1;j<=dynlen(cr_er_dpe);j++){
  dpGet(dps[i]+"."+cr_er_dpe[j]+":_alert_hdl.._type",type);
  DebugTN(dps[i]+"."+cr_er_dpe[j]+":_alert_hdl.._type"+">>"+type);
  if(type > 0)dynAppend(dps2,dps[i]+"."+cr_er_dpe[j]);
 } //j

dps3=dpNames("*","fwWnrCh_CSC_LV_d");
//==================================

for(i10=1;i10<=dynlen(dps3);i10++){
  if(strpos(dps3[i10],mudcsRemoveSystem(dps[i]))<0)continue;
//dps1=dpNames(dps3[i10]+".Status.*Failure*");  
 for(j=1;j<=dynlen(ch_er_dpe);j++){
  dpGet(dps3[i10]+"."+ch_er_dpe[j]+":_alert_hdl.._type",type);
  if(type > 0)dynAppend(dps2,dps3[i10]+"."+ch_er_dpe[j]);
 } //j
} //i10

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
//===================================================================================
//===================================================================================
//===================================================================================



CRB_1_alert_main(){

	//dyn_string exceptionInfo; 
	//g_busyThread = -1; 
	//fwBusyBarButton_std_startBusy(exceptionInfo); 


string device_type="CRB_1";
  crb_set_d(true,device_type);

	//	fwBusyBarButton_std_stopBusy(exceptionInfo);

}
//============================================================================
crb_set_d(bool isSet, string device_type)
{

int i;

dyn_string limits_s;
string value;
string s1,s2;

limits_s = makeDynString();

// s1 is OK
// s2 is ALARM

string s1_33="2.9";
string s2_33="3.7";

string s1_50="4.5";
string s2_50="5.5";

string s1_15="1.2";
string s2_15="1.8";



//	HV_STATE_ILOCK		= 5,
//	HV_STATE_INT_TRIP	= 6,
//	HV_STATE_CHAN_TRIP	= 7,



for(i=1;i<=9;i++){

value=value+".data.pair"+i+".v33|.data.pair"+i+".v34|.data.pair"+i+".v50|";
//// if(i<36)value=value+".data.v"+i+".status|";
//// else value=value+".data.v"+i+".status"; 
 
 dynAppend(limits_s,s1_33+"|"+s2_33);
 dynAppend(limits_s,s1_33+"|"+s2_33);
 dynAppend(limits_s,s1_50+"|"+s2_50);
}


value=value+".data.vcc_33|.data.vcc_50|";
 dynAppend(limits_s,s1_33+"|"+s2_33);
 dynAppend(limits_s,s1_50+"|"+s2_50);
value=value+".data.vccb_33|.data.vccb_50|";
 dynAppend(limits_s,s1_33+"|"+s2_33);
 dynAppend(limits_s,s1_50+"|"+s2_50);
value=value+".data.v15_1|.data.v15_2|";
 dynAppend(limits_s,s1_15+"|"+s2_15);
 dynAppend(limits_s,s1_15+"|"+s2_15);
  
  


//value=value+".data.module_status"; 
//dynAppend(limits_s,"0|5");  // 0 is OK
//                            // 5 is alarm

mudcsLookAndAlertArchiveConfig(true, 
false, isSet, device_type, value, limits_s, DpNameStartForAlertConfig, "",0,0,0.,0.);




}

//==========================================================
//==========================================================
//==========================================================
