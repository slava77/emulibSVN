
//===================================================================================
//===================================================================================
//===================================================================================
mudcsCrbAiAlertSet(bool isCreateConfig, bool isAck){
  
  
dyn_string limits_s;
string value;
dyn_string dps;
int i,j;
dyn_string exceptionInfo;
string coord;
int iN;
    

limits_s = makeDynString();

// s1 is OK
// s2 is ALARM

string s1,s2;

string s1_33="2.9";
string s2_33="3.7";

string s1_50="4.5";
string s2_50="5.5";

string s1_15="1.2";
string s2_15="1.8";
/*
dyn_int CSC_fwCAN1_g_LIST_5V;
dyn_int CSC_fwCAN1_g_LIST_3V1;
dyn_int CSC_fwCAN1_g_LIST_3V2; 
dyn_int CSC_fwCAN1_g_LIST_15V; // 1.5, 1.5 
dyn_int CSC_fwCAN1_g_LIST_CC;  // 3.3 ,5 
dyn_int CSC_fwCAN1_g_LIST_MPC; // 3.3 , 5
*/

dps=dpNames("*","FwElmbAi");
//==================================
for(i=1;i<=dynlen(dps);i++){  

if(strpos(dps[i],"/ai_")<0)continue; // to skip the atlas psu 
  
s1="";
s2="";
    
iN=mudcsCrbGetChannelNumberByName(dps[i]);
if(dynContains(CSC_fwCAN1_g_LIST_5V,iN) || iN==CSC_fwCAN1_g_LIST_CC[2] || iN==CSC_fwCAN1_g_LIST_MPC[2])
{s1=s1_50; s2=s2_50;}
else if(dynContains(CSC_fwCAN1_g_LIST_3V1,iN) || dynContains(CSC_fwCAN1_g_LIST_3V2,iN)
                                        || iN==CSC_fwCAN1_g_LIST_CC[1] || iN==CSC_fwCAN1_g_LIST_MPC[1])
{s1=s1_33; s2=s2_33;}
else if(dynContains(CSC_fwCAN1_g_LIST_15V,iN))
{s1=s1_15; s2=s2_15;}
      
value=mudcsLvRemoveSystem(dps[i]+".value");
limits_s=makeDynString(s1,s2);

//crb_set(value,limits_s);  

if(s1=="")continue;

mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);
//dpSetWait(value+":_alert_hdl.._active", true);
} // for i
  
  
}
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
mudcsCrbElmbAlertSet(bool isCreateConfig, bool isAck){
  
  
dyn_string limits_s;
string value;
dyn_string dps;
int i,j;
    

limits_s = makeDynString();

// s1 is OK
// s2 is ALARM


dps=dpNames("*","FwElmbNode");
//==================================
for(i=1;i<=dynlen(dps);i++){

  /*
//limits_s=makeDynString("5","6","133","134");
limits_s=makeDynString("5","6");
  value=mudcsLvRemoveSystem(dps[i]+".state.value");
mudcsLvAlertConfig(false, true, value, limits_s);


//limits_s=makeDynString("7","8"); // 8 correcponds to disconnected
*/
  
limits_s=makeDynString("2047"); // that gives an alarm if the value >= 2047
value=mudcsLvRemoveSystem(dps[i]+".error");
mudcsLvAlertConfig(false, true, value, limits_s);


 
  limits_s=makeDynString("1"); // that gives an alarm if the value >= 1
value=mudcsLvRemoveSystem(dps[i]+".emergency.emergencyErrorCode");
mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);


} // for i

}

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
mudcsCrbErrorStatusAlertSet(bool isCreateConfig,bool isAck){
  
  
dyn_string limits_s;
string value;
dyn_string dps;
int i,j;
    

limits_s = makeDynString();

// s1 is OK
// s2 is ALARM


dps=dpNames("*","fwCrb_CSC_LV");
//==================================
for(i=1;i<=dynlen(dps);i++){

  
  limits_s=makeDynString("1"); // that gives an alarm if the value >= 1
value=mudcsLvRemoveSystem(dps[i]+".error_status");
mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);


} // for i

}  
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
mudcsCrbSummaryAlertSet(bool isCreateConfig){
//==================================
//=== SUMMARY ALERT ================
//==================================

dyn_string dps, dps1, dps2, dps3;
int i,j;
dyn_string exceptionInfo, s1;  
string coord;
  
dps=dpNames("*","fwCrb_CSC_LV");

 for(i=1;i<=dynlen(dps);i++){
  dynClear(dps2); 
  dpGet(dps[i]+".coord",coord);
  
  dps1=dpNames("*ELMB_"+coord+"*","FwElmbAi");
  for(j=1;j<=dynlen(dps1);j++){
   dps2[j]=dps1[j]+".value";  
  }
 //  DebugTN(dps2);
  dps1=dpNames("*ELMB_"+coord+"*","FwElmbNode");  
  for(j=1;j<=dynlen(dps1);j++){
//   dps3[2*j]=dps1[j]+".state.value";
//   dps3[2*j+1]=dps1[j]+".error";   
////// temp    dps3[j]=dps1[j]+".emergency.emergencyErrorCode";
  }
  
  s1=dps[i]+".error_status";
  dynAppend(dps2,dps3);
  dynAppend(dps2,s1);  
  
  //DebugTN(dps2);
  
  if(isCreateConfig){
   fwAlertConfig_createSummary( mudcsLvRemoveSystem(dps[i])+".",
   makeDynString("","") , dps2 ,"", makeDynString(),"",exceptionInfo);
  }
  else{
   fwAlertConfig_deleteSummary( mudcsLvRemoveSystem(dps[i])+".",exceptionInfo);        
  }

dyn_string s1;
//dpSetWait(mudcsLvRemoveSystem(dps[i])+"."+":_alert_hdl.._ack_state",DPATTR_ACKTYPE_SINGLE );//"_fwErrorAck");
dpGet(mudcsLvRemoveSystem(dps[i])+"."+":_alert_hdl.._dp_list",s1);
DebugTN(s1);    


//  dpSet(mudcsLvRemoveSystem(dps[i])+"."+":_alert_hdl.._active", true);
 } // for i    

// TestDP_1.element:_alert_hdl.1._class", alertclass[1],   

}
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

//===================================================================================
//===================================================================================
//===================================================================================
mudcsPsuAiAlertSet(bool isCreateConfig, bool isAck){
  
string dpe;
bool configExists;
int  alertConfigType;
dyn_string alertTexts;
dyn_float alertLimits;
dyn_string alertClasses;
dyn_string summaryDpeList;
string alertPanel;
dyn_string alertPanelParameters;
string alertHelp;
bool isActive;
dyn_string exceptionInfo;
    
int i,j,jj;
dyn_string dps;
dyn_string dpsa;
string s1;
dps=dpNames("*","FwElmbPSUBranch");

for(i=1;i<=dynlen(dps);i++){
  dynClear(dpsa);
//  dpGet(dps[i]+".CAN.current",s1);
//  dynAppend(dpsa,s1);
  dpGet(dps[i]+".CAN.volts",s1);
  dynAppend(dpsa,s1);
//  dpGet(dps[i]+".AD.current",s1);
//  dynAppend(dpsa,s1);
  dpGet(dps[i]+".AD.volts",s1);
  dynAppend(dpsa,s1);      
  for(j=1;j<=dynlen(dpsa);j++){
   fwAlertConfig_get(dpsa[j],
   configExists,alertConfigType,alertTexts,alertLimits,alertClasses,summaryDpeList,
   alertPanel,alertPanelParameters,alertHelp,isActive,exceptionInfo);    
   
   DebugTN(dps[i]+"  "+dpsa[j]);
   DebugTN(alertClasses);
   DebugTN("---");
   for(jj=1;jj<=dynlen(alertClasses);jj++){
//    if(strpos(alertClasses[jj],"_fwFatalAck")>=0 && !isAck)alertClasses[jj]="_fwErrorNack"; 
//    if(strpos(alertClasses[jj],"_fwWarningNack")>=0)alertClasses[jj]=alertClasses[jj]; 
    if((strpos(alertClasses[jj],"_fwFatal")>=0 || strpos(alertClasses[jj],"_fwError")>=0 )&& !isAck)alertClasses[jj]="_fwErrorNack"; 
    if(strpos(alertClasses[jj],"_fwWarning")>=0)alertClasses[jj]=alertClasses[jj];     
   }
    
   
   fwAlertConfig_set(dpsa[j], alertConfigType,alertTexts,alertLimits,alertClasses,
   summaryDpeList,alertPanel,alertPanelParameters,alertHelp, exceptionInfo);  
   dpSetWait(mudcsLvAddSystem( dpsa[j] + ":_alert_hdl.._active"), true);
  int isActive;
   while(1){
   dpGet(mudcsLvAddSystem( dpsa[j] + ":_alert_hdl.._active"), isActive);
   if(!isActive)dpSetWait(mudcsLvAddSystem( dpsa[j] + ":_alert_hdl.._active"), true);
   else break;
   } // while
  }
   
}

/*
fwAlertConfig_get(dpe,
configExists,alertConfigType,alertTexts,alertLimits,alertClasses,summaryDpeList,
alertPanel,alertPanelParameters,alertHelp,isActive,exceptionInfo);
 
fwAlertConfig_setMultiple(DPE_LIST, alertType, alertText, limits, alertClass,
makeDynString(), "", makeDynString(),  "", exceptionInfo);

//fwAlertConfig_set(string dpe,int alertConfigType,dyn_string alertTexts,
//dyn_float alertLimits,dyn_string alertClasses,dyn_string summaryDpeList,
//string alertPanel,dyn_string alertPanelParameters,string alertHelp,
//dyn_string &exceptionInfo,bool modifyOnly = FALSE,bool fallBackToSet = FALSE,
//string addDpeInSummary = "",bool storeInParamHistory = TRUE)
*/  



}
//===================================================================================
mudcsPsuFsmAlertSet(bool isCreateConfig, bool isAck){
  
 
  dyn_string limits_s;
  string value;
  dyn_string dps;
  int i,j;
  
   dps=dpNames("*","fwPsu_CSC_LV");
   for(i=1;i<=dynlen(dps);i++){
     
    limits_s=makeDynString("1"); // that gives an alarm if the value >= 1
    value=mudcsLvRemoveSystem(dps[i]+".error_status");
    mudcsLvAlertConfig(false, isCreateConfig, value, limits_s, isAck);

   } // for i    
  
  
}

//===================================================================================
mudcsPsuSummaryAlertSet(bool isCreateConfig){
//==================================
//=== SUMMARY ALERT ================
//==================================

int i,j,jj;
dyn_string dps;
dyn_string dpsa;
string s1,node;
dyn_string exceptionInfo;
dyn_string crates;
int pos;

dps=dpNames("*","FwElmbPSUBranch");

 for(i=1;i<=dynlen(dps);i++){
   dynClear(dpsa); 
   dynClear(crates);    
//  dpGet(dps[i]+".CAN.current",s1);
//  dynAppend(dpsa,s1);
  dpGet(dps[i]+".CAN.volts",s1);
  dynAppend(dpsa,s1);
//  dpGet(dps[i]+".AD.current",s1);
//  dynAppend(dpsa,s1);
  dpGet(dps[i]+".AD.volts",s1);
  dynAppend(dpsa,s1);      
  
  pos=strpos(dps[i],"/Branch");
  s1=substr(dps[i],0,pos); // getting crate 
  if(!dynContains(crates,s1)){
    dynAppend(crates,s1);
    dpGet(s1+".node",node);
    ///////////dynAppend(dpsa,node+".emergency.emergencyErrorCode");    
    strreplace(node,"/","_");
    dynAppend(dpsa,node+".error_status");  
  }
  
  
  if(isCreateConfig){
   fwAlertConfig_createSummary( mudcsLvRemoveSystem(dps[i])+".",
   makeDynString("","") , dpsa ,"", makeDynString(),"",exceptionInfo);
  }
  else{
   fwAlertConfig_deleteSummary( mudcsLvRemoveSystem(dps[i])+".",exceptionInfo);        
  }
    

 } //for
}


//===================================================================================

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
