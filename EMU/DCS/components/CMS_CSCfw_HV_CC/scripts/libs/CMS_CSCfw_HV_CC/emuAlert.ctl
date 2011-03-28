 


int mudcsDpNameCompare(string dp1,string dp2){

// ************************************************************************************
// **** this is to compare dpnames with and without system prefix (like System1:) *****
//*************************************************************************************

dyn_string dps1=strsplit(dp1,":");
dyn_string dps2=strsplit(dp2,":");

string dp11=dps1[dynlen(dps1)];
string dp22=dps2[dynlen(dps2)];

 if(strpos(dp11,dp22) < 0 && strpos(dp22,dp11) <0) return -1;
 
 return 0;


}

//======================================================================

int getActiveDisksAndChambers(dyn_string selected_list){

int i;
dyn_string test_dyn_string,dpoints_not_controlled_dyn;
string test_string, dpoints_not_controlled;


dynClear(CSC_fwG_g_ACTIVE_DISKS_AND_CHAMBERS);

dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":DPOINTS_NOT_CONTROLLED.value"),dpoints_not_controlled);

dpoints_not_controlled_dyn=strsplit(dpoints_not_controlled,"\n");

 for(i=1;i<=dynlen(selected_list);i++){

  test_dyn_string=strsplit(selected_list[i],"/");

////mudcsDebug(test_dyn_string[dynlen(test_dyn_string)]);



  if(dynContains(dpoints_not_controlled_dyn,selected_list[i]))continue;

  test_string=substr(test_dyn_string[dynlen(test_dyn_string)],0,strpos(test_dyn_string[dynlen(test_dyn_string)],"d")+2+1); // disk
  if(!dynContains(CSC_fwG_g_ACTIVE_DISKS_AND_CHAMBERS,test_string)){
   dynAppend(CSC_fwG_g_ACTIVE_DISKS_AND_CHAMBERS,test_string);
  }

  test_string=substr(test_dyn_string[dynlen(test_dyn_string)],0,strpos(test_dyn_string[dynlen(test_dyn_string)],"c")+2+1); // chmaber
  if(!dynContains(CSC_fwG_g_ACTIVE_DISKS_AND_CHAMBERS,test_string)){
   dynAppend(CSC_fwG_g_ACTIVE_DISKS_AND_CHAMBERS,test_string);
  }

 } // for


return 0;

}



//======================================================================


mudcsScanTreeForDevices(string device_type, string start_dp_name, dyn_string &selected_list){


string dpName_start, child_s, child_s2,child_s3 ,child_s4 ,child_s5 ;
dyn_string childList1,childList2,childList3,childList4,childList5,  exceptionInfo;
dyn_string itext, itext2, itext3, itext4, itext5;
dyn_string ilist, ilist2, ilist3, ilist4, ilist5;
int i1,i2,i3,i4,i5,i6,     i10,i11;
int pos, pos1, length1, type1;
dyn_float sublimits;
dyn_string limits;
bool isGeneralSetting;

dyn_string CscLevelDevicesCoordinates;
int idev;
dyn_string splittedDeviceName;

//---+++++

 string device,hierarchyType;
 dyn_string children;
 hierarchyType = "LOGICAL";
 bool BROKER_FIRST=true;

 dynClear(selected_list);


//---+++++
// !!! COMMENT: isGeneralSetting is set true ONLY when we use the Summary tab in the alertin configuration !!!!!!
isGeneralSetting = true;
if(device_type != "GENERAL" )isGeneralSetting = false;


 dpName_start = CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_part;

 fwDevice_getChildren(dpName_start, hierarchyType,childList1 , exceptionInfo);

DebugN("mudcsScanTreeForDevices:MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"+" "+dynlen(childList1)+" "+dpName_start);

for(i1=1;i1 <= dynlen(childList1); i1++){ // loop over disks 

if(start_dp_name != "" && mudcsDpNameCompare(start_dp_name,childList1[i1]) < 0) continue;

child_s = childList1[i1];

fwDevice_getChildren(child_s, hierarchyType, childList2 , exceptionInfo);


DebugN("mudcsScanTreeForDevices:MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM2222"+" "+dynlen(childList2)+" "+child_s+" "+i1);

  for(i2=1;i2<= dynlen(childList2); i2++){// loop over chambers

child_s2 = childList2[i2];

fwDevice_getChildren(child_s2, hierarchyType, childList3 , exceptionInfo);

DebugN("mudcsScanTreeForDevices:MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM3333333333"+" "+dynlen(childList3));



 for(i10=1;i10<=dynlen(childList3);i10++){
   
  DebugN("mudcsScanTreeForDevices:MMMMMMMMMMMMMMMMMMMMMMMMMM"+childList3[i10]);

  
 fwDevice_getChildren(childList3[i10], hierarchyType, childList4 , exceptionInfo); 
  
  for(i11=1;i11<=dynlen(childList4);i11++){
 
  dyn_string childList333=strsplit(childList4[i11],"/");

 if(device_type != "GENERAL" && 
   ((pos=strpos(childList333[dynlen(childList333)],device_type))<0 || strlen(childList333[dynlen(childList333)])-pos != strlen(device_type)))continue;

///mudcsDebug2(childList333[dynlen(childList333)]+" "+device_type+" "+strlen(childList333[dynlen(childList333)])+" "+pos+" "+strlen(device_type));


//  DebugN("mudcsScanTreeForDevices:MMMMMMMMMMMMMMMMMMMMMMMMMM----->"+childList3[i10]+">> "+childList333[dynlen(childList333)]+"  "+device_type); 

 childList4[i11]=dpAliasToName(childList4[i11]); // getting full name ,i.e. with prefix LowVoltage e.g.
 child_s3 = childList4[i11];
 child_s3 = substr(child_s3,0,strpos(child_s3,"."));



  DebugN("mudcsScanTreeForDevices:MMMMMMMMMMMMMMMMMMMMMMMMMM----->"+child_s3); 

  dynAppend(selected_list,child_s3);

} // for over devices
 } // for over chambers

 } // loop over sectors
} // loop over disks

}


//======================================================================
//===================================================================================














//======================================================================


mudcsLookAndAlertArchiveConfig (bool isAlert, bool isAlertChangeOnly, bool isAlertActive,
string device_type, string value, dyn_string limits_s, string current_dpName,
string archiveClass, int smoothType, int smoothProcedure,float deadband,float newTime){


string dpName_start, child_s, child_s2,child_s3 ,child_s4 ,child_s5 ;
dyn_string childList1,childList2,childList3,childList4,childList5,  exceptionInfo;
dyn_string itext, itext2, itext3, itext4, itext5;
dyn_string ilist, ilist2, ilist3, ilist4, ilist5;
int i1,i2,i3,i4,i5,i6,     i10;
int pos1, length1, type1;
dyn_float sublimits;
dyn_string limits;
bool isGeneralSetting;

dyn_string CscLevelDevicesCoordinates;
int idev;
dyn_string splittedDeviceName;

//---+++++

 string device,hierarchyType;
 dyn_string children;
 hierarchyType = "LOGICAL";
 bool BROKER_FIRST=true;

// !!!! COMMENT: THE SENCE OF THE BROKER_FIRST:
// THE BROKER_FIRST ID NEEDED TO ALERT_CONFIGURE BROKERS ONLY AT THE FIRST
// PASSAGE THROUGH THE DEVICE OF THIS DEVICE TYPE.
// FOR EXAMPLE: THE ALERT_CONFIGURE OF BROKERS IS NOT NEEDED FOR SECOND PASS (SECOND CHAMBER, ETC)
//


//---+++++
// !!! COMMENT: isGeneralSetting is set true ONLY when we use the Summary tab in the alertin configuration !!!!!!
isGeneralSetting = true;
if(device_type != "GENERAL" )isGeneralSetting = false;
//////if(strpos(current_dpName,device_type) >= 0 )isGeneralSetting = false;

 dpName_start = CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_part;

///////++++++ fwGenericDpFunctions_getChildren(dpName_start, childList1, exceptionInfo);
 fwDevice_getChildren(dpName_start, hierarchyType,childList1 , exceptionInfo);


  dynClear(childList3); // for CSC level devices

DebugN("mudcsLookAndAlertArchiveConfig:MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"+" "+dynlen(childList1)+" "+dpName_start);

for(i1=1;i1 <= dynlen(childList1); i1++){ // loop over wheels 

// **********************************************************************
// ************ GAS **AND OTHER Csc Level Devices ***********************
// **********************************************************************
// **********************************************************************
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.CscLevelDevicesCoordinates"), CscLevelDevicesCoordinates);


  //mudcsDebug(childList1[i1]);

  splittedDeviceName=strsplit(childList1[i1],"/");
//mudcsDebug2(splittedDeviceName[dynlen(splittedDeviceName)]);
    pos1=-1;
  for(idev=1;idev<=dynlen(CscLevelDevicesCoordinates);idev++){
//mudcsDebug2(CscLevelDevicesCoordinates[idev]);
   pos1=strpos(CscLevelDevicesCoordinates[idev],splittedDeviceName[dynlen(splittedDeviceName)]);
   if(pos1 >=0 )break;
  }

   /////// new naming schema: if(pos1 >=0 && strpos(childList1[i1],"CSCHV_PR")<0){
   if(pos1 >= 0){   /////// new naming schema: 
  dynAppend(childList3,childList1[i1]);

   // mudcsDebug(childList1[i1]);

  DeviceAlertSetting(BROKER_FIRST, childList3, isAlert, isAlertChangeOnly, isAlertActive,
  device_type, value, limits_s, current_dpName,
  archiveClass, smoothType, smoothProcedure, deadband, newTime, ilist3); // here ilist3 is not needed (cleared in the following call to DeviceAlertSetting)

//// mudcsDebug2(splittedDeviceName[dynlen(splittedDeviceName)]);

 childList1[i1]=dpAliasToName(childList1[i1]); // getting full name ,i.e. with prefix LowVoltage e.g.
 child_s = childList1[i1];

// mudcsDebug2(child_s);
 dynAppend(ilist,child_s);

  continue;
  }
// **********************************************************************
// **********************************************************************
// **********************************************************************
// **********************************************************************

////// pos1 = strpos(childList1[i1],"|");
////// length1 = strlen(childList1[i1])-pos1; 
////// child_s = substr(childList1[i1], pos1+1, length1+1);
child_s = childList1[i1];



/////// fwGenericDpFunctions_getChildren(child_s, childList2, exceptionInfo);
fwDevice_getChildren(child_s, hierarchyType, childList2 , exceptionInfo);

DebugN("mudcsLookAndAlertArchiveConfig:MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM2222"+" "+dynlen(childList2)+" "+child_s+" "+i1);


  for(i2=1;i2<= dynlen(childList2); i2++){// loop over sectors or primaries folders

// **********************************************************************
// ************ primaries **AND OTHER System Level Devices or chamber level devices***********************
// **********************************************************************
// **********************************************************************
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.CscLevelDevicesCoordinates"), CscLevelDevicesCoordinates);



  splittedDeviceName=strsplit(childList2[i2],"/");
//mudcsDebug2(splittedDeviceName[dynlen(splittedDeviceName)]);
    pos1=-1;

////if(strpos(childList2[i2],"HV_PR")>=0)
////mudcsDebug("---->"+splittedDeviceName[dynlen(splittedDeviceName)]);

  for(idev=1;idev<=dynlen(CscLevelDevicesCoordinates);idev++){
//mudcsDebug2(CscLevelDevicesCoordinates[idev]);
   pos1=strpos(CscLevelDevicesCoordinates[idev],splittedDeviceName[dynlen(splittedDeviceName)]);
   if(pos1 >=0 )break;
  }
  if(pos1 >=0 ){
  dynAppend(childList3,childList2[i2]);

/*
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
DebugTN(childList3);
mudcsDebug("");
*/

  DeviceAlertSetting(BROKER_FIRST, childList3, isAlert, isAlertChangeOnly, isAlertActive,
  device_type, value, limits_s, current_dpName,
  archiveClass, smoothType, smoothProcedure, deadband, newTime, ilist3); // here ilist3 is not needed (cleared in the following call to DeviceAlertSetting)

//// mudcsDebug2(splittedDeviceName[dynlen(splittedDeviceName)]);

 childList2[i2]=dpAliasToName(childList2[i2]); // getting full name ,i.e. with prefix LowVoltage e.g.
 child_s = childList2[i2];

// mudcsDebug2(child_s);
 dynAppend(ilist,child_s);

  continue;
  } // if(pos1 >=0 )
// **********************************************************************
// **********************************************************************
// **********************************************************************
// **********************************************************************


//DebugN("1++++++++++++++++++++++++++++++++++");

//////  pos1 = strpos(childList2[i2],"|");
//////  length1 = strlen(childList2[i2])-pos1; 
//////  child_s2 = substr(childList2[i2], pos1+1, length1+1);
child_s2 = childList2[i2];


///////  fwGenericDpFunctions_getChildren(child_s2, childList3, exceptionInfo);
fwDevice_getChildren(child_s2, hierarchyType, childList3 , exceptionInfo);


DebugN("mudcsLookAndAlertArchiveConfig:MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM3333333333"+" "+dynlen(childList3));

//-------------------------------
if(dynlen(childList3) == 0){ // means that the child_s2 is device

DebugN("mudcsLookAndAlertArchiveConfig:MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM3333333333"+" "+dynlen(childList3) +" "+childList2[i2]+" is added");

  dynAppend(childList3,childList2[i2]);

  DeviceAlertSetting(BROKER_FIRST, childList3, isAlert, isAlertChangeOnly, isAlertActive,
  device_type, value, limits_s, current_dpName,
  archiveClass, smoothType, smoothProcedure, deadband, newTime, ilist3); // here ilist3 is not needed (cleared in the following call to DeviceAlertSetting)

//// mudcsDebug2(splittedDeviceName[dynlen(splittedDeviceName)]);

 childList2[i2]=dpAliasToName(childList2[i2]); // getting full name ,i.e. with prefix LowVoltage e.g.
 child_s = childList2[i2];

// mudcsDebug2(child_s);
 dynAppend(ilist,child_s);

  continue;

}
//--------------------------------

 for(i10=1;i10<=dynlen(childList3);i10++){
  DebugN("mudcsLookAndAlertArchiveConfig:MMMMMMMMMMMMMMMMMMMMMMMMMM"+childList3[i10]);

  fwDevice_getChildren(childList3[i10], hierarchyType, childList4 , exceptionInfo);
  
//-------------------------------
if(dynlen(childList4) == 0){ // means that the childList3[i10] is device

DebugN("mudcsLookAndAlertArchiveConfig:MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM3333333333"+" "+dynlen(childList3) +" "+childList2[i2]+" is added");

  dynAppend(childList4,childList3[i10]);

  DeviceAlertSetting(BROKER_FIRST, childList4, isAlert, isAlertChangeOnly, isAlertActive,
  device_type, value, limits_s, current_dpName,
  archiveClass, smoothType, smoothProcedure, deadband, newTime, ilist3); // here ilist3 is not needed (cleared in the following call to DeviceAlertSetting)

//// mudcsDebug2(splittedDeviceName[dynlen(splittedDeviceName)]);

 childList3[i10]=dpAliasToName(childList3[i10]); // getting full name ,i.e. with prefix LowVoltage e.g.
 child_s = childList3[i10];

// mudcsDebug2(child_s);
 dynAppend(ilist,child_s);

  continue;

}
//--------------------------------

 
 
 
  DeviceAlertSetting(BROKER_FIRST, childList4, isAlert, isAlertChangeOnly, isAlertActive,
  device_type, value, limits_s, current_dpName,
  archiveClass, smoothType, smoothProcedure, deadband, newTime, ilist3);

  
  


// DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES
// DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES-DEVICES
 if(!isGeneralSetting)continue;
    
       if(isAlert){
           if(isAlertActive){
        if(!isAlertChangeOnly){
          fwAlertConfig_createSummary( child_s2+".",makeDynString("","") , ilist3,"", makeDynString(),"",exceptionInfo);
          dpSetWait(mudcsAddSystem(child_s2 +"."+ ":_alert_hdl.._active"), true);
        }
           }
           // TAK NE DOLZHNO BYT' !!!! else dpSetWait(mudcsAddSystem(child_s2 +"."+ ":_alert_hdl.._active"), false);//
           else fwAlertConfig_deleteSummary( child_s2+".",exceptionInfo);
       }


       dynClear(ilist3);

        dynAppend(ilist2,child_s2+".");
  } // for chambers    
  } //for sectors  
// CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS
// CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS-CHAMBERS
 if(!isGeneralSetting)continue;

      if(isAlert){
           if(isAlertActive){
        if(!isAlertChangeOnly){
          fwAlertConfig_createSummary( child_s+".",makeDynString("","") , ilist2,"", makeDynString(),"",exceptionInfo);
          dpSetWait(mudcsAddSystem(child_s +"."+ ":_alert_hdl.._active"), true);
        }
           }
           // TAK NE DOLZHNO BYT' !!!! else dpSetWait(mudcsAddSystem(child_s +"."+ ":_alert_hdl.._active"), false);//
           else fwAlertConfig_deleteSummary( child_s+".",exceptionInfo);
       }
       dynClear(ilist2);

        dynAppend(ilist,child_s+".");
 } //for wheels 
// WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS
// WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS-WHEELS




// !!! COMMENT: isGeneralSetting is set true ONLY when we use the Summary tab in the alertin configuration !!!!!!
 if(isGeneralSetting){



       if(isAlert){
           if(isAlertActive){
        if(!isAlertChangeOnly){

//  mudcsDebug2(ilist[1]);
//mudcsDebug2(ilist[2]);

         fwAlertConfig_createSummary( dpName_start+".",makeDynString("","") , ilist,"", makeDynString(),"",exceptionInfo);
         dpSetWait(mudcsAddSystem( dpName_start+"." + ":_alert_hdl.._active"), true);
        }
           }
           // TAK NE DOLZHNO BYT' !!!! else dpSetWait(mudcsAddSystem( dpName_start+"." + ":_alert_hdl.._active"), false);//
          else fwAlertConfig_deleteSummary(dpName_start+".",exceptionInfo);
       }
       dynClear(ilist);
 } // if(isGeneralSetting){


//mudcsDebug("2----------------------->"+child_s3);
}

//======================================================================
//===================================================================================




















DeviceAlertSetting(bool &BROKER_FIRST, dyn_string &childList3, bool isAlert, 
bool isAlertChangeOnly, bool isAlertActive,
string device_type, string value, dyn_string limits_s, string current_dpName,
string archiveClass, int smoothType, int smoothProcedure,float deadband,float newTime, dyn_string &ilist3){

int pos;


string device_type_alias;
string mudcs_alias;

        if(device_type == "LV_1")device_type_alias="_LV";
        else if(device_type == "TEMP_1")device_type_alias="_TEMP";
        else if(device_type == "CHIP_1")device_type_alias="_CHIP";
        else if(device_type == "GAS_SX5")device_type_alias="GAS_MIXERS"; 
        else if(device_type == "HV_1")device_type_alias="_HV";
        else if(device_type == "WTH_SX5")device_type_alias="WEATHER_SX5";
        else if(device_type == "PT100")device_type_alias="GAS_PT100";
        else if(device_type == "HV_PR")device_type_alias="HV_PR";
        else if(device_type == "CRB_1")device_type_alias="_CRB";
        else if(device_type == "MRTN_1")device_type_alias="MRTN_"; 
        else if(device_type == "WNR12_1")device_type_alias="WNR12_"; 
        else if(device_type == "ALNM_1")device_type_alias="_ALNM";


string mudcs_alias;
dyn_string dps_to_archive=makeDynString(

  "CSCsp2r1c14",
  "CSCsp2r1c15",
  "CSCsp2r1c16",
  "CSCsp2r2c27",
  "CSCsp2r2c28",
  "CSCsp2r2c29",
  "CSCsp2r2c30",
  "CSCsp2r2c31",
  "CSCsp2r2c32",
  "CSCsp3r1c14",
  "CSCsp3r1c15",
  "CSCsp3r1c16",
  "CSCsp3r2c27",
  "CSCsp3r2c28",
  "CSCsp3r2c29",
  "CSCsp3r2c30",
  "CSCsp3r2c31",
  "CSCsp3r2c32",



  "CSCsp1r2c27",
  "CSCsp1r2c28",
  "CSCsp1r2c29",
  "CSCsp1r2c30",
  "CSCsp1r2c31",
  "CSCsp1r2c32",
  "CSCsp1r3c27",
  "CSCsp1r3c28",
  "CSCsp1r3c29",
  "CSCsp1r3c30",
  "CSCsp1r3c31",
  "CSCsp1r3c32",

  "CSCmaraton01MRTN_1",
  "CSCmaraton02MRTN_1"
//  "CSCmaraton03MRTN_1"






);

dps_to_archive=mudcsNamesByAliasesGet(dps_to_archive);

//mudcsDebug("");

//----
/*
dyn_string dyn_debug;
int i_ar1;
   for(i_ar1=1;i_ar1<=dynlen(dps_to_archive);i_ar1++){

dpGet("dyn_debug2.",dyn_debug);
dynAppend(dyn_debug,dps_to_archive[i_ar1]); 
dpSet("dyn_debug2.",dyn_debug);

   }
*/
//-----

//---------
bool a_configExists;
string a_archiveClass;
int a_archiveType;
int a_smoothProcedure;
float a_deadband;
float a_timeInterval;
bool a_isActive;
dyn_string a_exceptionInfo;
//---------


bool isDeviceTypeMet=false;;
dyn_string brokers;
int i_br;
string test_string;



string child_s3 ,child_s4 ,child_s5 ;

dyn_string childList3_d;

/////////////////dyn_string childList3;

dyn_string alert_dp_list;
dyn_string childList4,childList5,  exceptionInfo;
dyn_string ilist4, ilist5;
int i3,i4,i5,i6;
int pos1, length1, type1;
dyn_float sublimits;
dyn_string limits;
bool isGeneralSetting;
//---+++++

 string device,hierarchyType;
 dyn_string children;
 hierarchyType = "LOGICAL";



////DebugN("___________))))))))))))))))))))))))))))))))))))))))))))");

dynClear(ilist3);


// !!! COMMENT: isGeneralSetting is set true ONLY when we use the Summary tab in the alertin configuration !!!!!!
//---+++++
isGeneralSetting = true;
if(device_type != "GENERAL" )isGeneralSetting = false;



//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// the piece to 1) alert-config masters (which are not in the childList3) 
//              2) to add masters to the summary alert of distributions (chambers)


 dyn_string master_channels;
 int number_of_devices_before_masters=dynlen(childList3);


/// dyn_string dyn_debug;
///dpGet(mudcsAddSystem("dyn_debug2."),dyn_debug);

if(device_type=="HV_1"  && isAlert && !isAlertChangeOnly){ 
 int master_id, master_chan, master_index; 
 dyn_string coord_master;



 for(i3=1;i3<= dynlen(childList3); i3++){// 

  mudcs_alias=mudcsAliasNameGet(childList3[i3]);
//mudcsDebug2(mudcs_alias+">>"+childList3[i3]);
  if(strpos(mudcs_alias,"HV_1") < 0 || strpos(childList3[i3],"Broker")>=0)continue;


   child_s3 = dpAliasToName(childList3[i3]); // getting full name ,i.e. with prefix LowVoltage e.g.
   child_s3 = substr(child_s3,0,strpos(child_s3,"."));
   mudcsGetMasterForChamber(child_s3, master_id, master_chan, master_index, coord_master);
////mudcsDebug2(child_s3+">>"+master_id+">>"+master_chan); 
 if(master_index >= 1){ // for those HV devices which are connected to masters
   master_chan++;
   master_channels[i3]= CSC_fwG_g_all_masters[master_index]+".data.v"+master_chan+".status";
   if(dynContains(childList3,CSC_fwG_g_all_masters[master_index]) <=0)
   {
        if(!dynContains(childList3,CSC_fwG_g_all_masters[master_index]))dynAppend(childList3,CSC_fwG_g_all_masters[master_index]);
//////        if(!dynContains(dyn_debug,CSC_fwG_g_all_masters[master_index]))dynAppend(dyn_debug,CSC_fwG_g_all_masters[master_index]); 
   }
  }
  else {master_channels[i3]="";}
 }

}


//dyn_string dyn_debug;
//dpGet(mudcsAddSystem("dyn_debug2."),dyn_debug);
//   dynAppend(dyn_debug,child_s3+" "+1); 
///dpSet(mudcsAddSystem("dyn_debug2."),dyn_debug);


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

   for(i3=1;i3<= dynlen(childList3); i3++){// loop over devices


  mudcs_alias=mudcsAliasNameGet(childList3[i3]);
//  if(strpos(childList3[i3],device_type_alias) < 0 && (strpos(child_s3,"master") < 0 && device_type == "HV_1"))continue;
//----------- this is the second piece to add master status  to the summary alert of distributions (chambers) --------------------------
       if(device_type=="HV_1" && strpos(mudcs_alias,"HV_1")>=0 && isAlert && !isAlertChangeOnly && master_channels[i3] != ""
          && i3<= number_of_devices_before_masters){
           dynAppend(ilist4,master_channels[i3]);
           
       }
//-------------------------------------------------------------------------------------------------------------------------------------

//////DebugN("2 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.."+childList3[i3]);

//////   pos1 = strpos(childList3[i3],"|");
//////   length1 = strlen(childList3[i3])-pos1; 
//////   child_s3 = substr(childList3[i3], pos1+1, length1+1);
//////   mudcsDebug(childList3[i3]);


/////string n_save=childList3[i3];

if(strpos(childList3[i3],"master") >= 0 && device_type == "HV_1"){
 child_s3 = childList3[i3];
}
else if(isAlertChangeOnly){
 child_s3 = childList3[i3];
}
else{
 childList3[i3]=dpAliasToName(childList3[i3]); // getting full name ,i.e. with prefix LowVoltage e.g.
 child_s3 = childList3[i3];
 child_s3 = substr(child_s3,0,strpos(child_s3,"."));
}
/* removing brokers
else if(i3 < (dynlen(childList3)-dynlen(brokers)+1) || !BROKER_FIRST){ // NOT FOR BROKER
 childList3[i3]=dpAliasToName(childList3[i3]); // getting full name ,i.e. with prefix LowVoltage e.g.
 child_s3 = childList3[i3];
 child_s3 = substr(child_s3,0,strpos(child_s3,"."));
}
else{
 child_s3 = childList3[i3];
}
*/

////mudcsDebug2(n_save+">>"+childList3[i3]+">>"+child_s3);


int i_test;
//mudcsDebug(child_s3);
//if(strpos(childList3[i3],"master")>0)mudcsDebug(child_s3);
mudcsConvertFsm2DataOne(child_s3,child_s3);
//if(strpos(childList3[i3],"master")>0)mudcsDebug("master:\n"+child_s3);
//mudcsDebug(child_s3);}
//else return;

//if(strpos(child_s3,"primary")>=0)mudcsDebug("0--->"+child_s3);
//if(strpos(child_s3,"primary")>=0)mudcsDebug("0--->"+value);

/////////mudcsDebug("2->"+child_s3+">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.");

//////////////////    if(!isGeneralSetting && child_s3 != current_dpName)
/////////////////      {
/////////////////       continue; // ******************************************
////////////////      }

// !!! COMMENT: isGeneralSetting is set true ONLY when we use the Summary tab in the alertin configuration !!!!!!
 if(isGeneralSetting){
 // removing brokers: if(i3 < (dynlen(childList3)-dynlen(brokers)+1) || !BROKER_FIRST) // NOT FOR BROKER 
 // {

   //   test_string=  dpTypeName(child_s3);
   //        if(test_string=="LV_1" || test_string=="TEMP_1" ||test_string=="GAS_SX5" )
           dynAppend(ilist3,child_s3+".");

 // }
  continue;  // to -> for devices
 }




//if(strpos(child_s3,"primary")>=0)mudcsDebug("1--->"+child_s3);
//if(strpos(child_s3,"primary")>=0)mudcsDebug("1--->"+value);

//////DebugN("2->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"+child_s3+" "+device_type);
/*
if(strpos(device_type,"CRB")>=0){
dyn_string dyn_debug;
dpGet(mudcsAddSystem("dyn_debug2."),dyn_debug);
   dynAppend(dyn_debug,child_s3+" "+1); 
dpSet(mudcsAddSystem("dyn_debug2."),dyn_debug);
}
*/

// mudcs_alias=mudcsAliasNameGet(child_s3);
// mudcsDebug2(mudcs_alias+">>"+child_s3);

 if(device_type_alias != "_LV" || strpos(child_s3,"_CRB") ==-1) //pvss36
     if((strpos(child_s3,device_type_alias) !=-1) || (strpos(child_s3,"master") !=-1 && device_type == "HV_1")
       /*removing brokers:  || i3 >= (dynlen(childList3)-dynlen(brokers)+1)*/  ){
/*
if(strpos(device_type,"CRB")>=0){
dyn_string dyn_debug;
dpGet(mudcsAddSystem("dyn_debug2."),dyn_debug);
   dynAppend(dyn_debug,child_s3+" "+2); 
dpSet(mudcsAddSystem("dyn_debug2."),dyn_debug);
}
*/

     childList4 = strsplit(value, "|");

//------------------------




string setList;
int  w_pos_return, radius_return, ich_return;
string type_return,set_return ;

if(device_type=="LV_1" || device_type=="TEMP_1" || device_type=="CHIP_1")setList="DynatemAndSetNumberList";
else if(device_type=="HV_1")setList="HVsetList";
else setList="";

if(setList != ""){
 mudcs_alias=mudcsAliasNameGet(child_s3);
 if( strpos(child_s3,"master")>=0 ){
  mudcsGetCoordForDp("any", child_s3, set_return); 
 }
 else{
  mudcsNameToPosConvert(mudcs_alias/*child_s3*/, w_pos_return, radius_return, ich_return, type_return);
  retrieveCoordinateSet("", child_s3, setList , w_pos_return, radius_return, ich_return, type_return, set_return);
 }
}



//if(strpos(child_s3,"primary")>=0)mudcsDebug("2--->"+child_s3);
//if(strpos(child_s3,"primary")>=0)mudcsDebug("2--->"+value);

if(setList != "" && (set_return=="" || set_return=="0") &&  archiveClass != "delete" && !isAlert)continue;

//----------------------------------------------



dynClear(alert_dp_list);
   dpGet(mudcsAddSystem(child_s3+".:_alert_hdl.._dp_list"),alert_dp_list);

////if(strpos(child_s3,"master")>=0)DebugTN("????????????????????????????????????????????alert_dp_list  "+alert_dp_list);

//////////   if(isAlert && !isAlertChangeOnly && dynlen(alert_dp_list) > 2)continue; // it is not to configure (alert) if already alert-configured
  


//----------------------------------------

//dyn_string dyn_debug;



   int i_ar;
   bool found_ar=false;
   for(i_ar=1;i_ar<=dynlen(dps_to_archive);i_ar++){
/*
dpGet("dyn_debug2.",dyn_debug);
dynAppend(dyn_debug,mudcsAddSystem(child_s3)+">>>>>"+dps_to_archive[i_ar]); 
dpSet("dyn_debug2.",dyn_debug);
*/
    if(strpos(mudcsAddSystem(child_s3),dps_to_archive[i_ar])>=0){found_ar=true; ;break;}
   }

   if(!found_ar && !isAlert)continue;


//-----------------------------------------

if(dynlen(childList4) > 0 && device_type=="HV_1" /* removing brokers && i3 != (dynlen(childList3)-dynlen(brokers)+1)*/){ // NOT FOR BROKER 



 int number_per_HV_channel, ch_first, ch_last, i_ch;
// int  w_pos_return, radius_return, ich_return;
// string type_return,set_return ;
 dyn_string coords, childList4_new;
// mudcsNameToPosConvert(child_s3, w_pos_return, radius_return, ich_return, type_return);
// retrieveCoordinateSet("", child_s3, "HVsetList", w_pos_return, radius_return, ich_return, type_return, set_return);
 coords=strsplit(set_return,";");   
 



  number_per_HV_channel=dynlen(childList4)/36; // it is supposed that number of module-level dpoins (.module_status e.g. < 36)
  dynClear(childList4_new);

 int temp_i=36*number_per_HV_channel;

  if(dynlen(coords)<4){
   if(strpos(child_s3,"master") < 0){
    ch_first=0; 
    ch_last=0;
   }
   else{
    ch_first=1; 
    ch_last=8*number_per_HV_channel;
   }
  }
  else if(coords[4] ==0){
   ch_first=1; 
   ch_last=30*number_per_HV_channel;
  }
  else if(coords[4] ==1){
   ch_first=1; 
   ch_last=18*number_per_HV_channel;
   if(archiveClass == "delete" && !isAlert){
    ch_first=1; 
    ch_last=36*number_per_HV_channel;
   }

  }
  else if(coords[4] ==2){
   ch_first=18*number_per_HV_channel+1;
   ch_last=36*number_per_HV_channel;
   if(archiveClass == "delete" && !isAlert){
    ch_first=1; 
    ch_last=36*number_per_HV_channel;
   }
  }
  for(i_ch=1;i_ch<=36*number_per_HV_channel;i_ch++){
 //if(i_ch < ch_first || i_ch > ch_last)dynRemove(childList4,i_ch);
    if(i_ch >= ch_first && i_ch <= ch_last)dynAppend(childList4_new,childList4[i_ch]);
  }

  for(i_ch=36*number_per_HV_channel+1;i_ch<=dynlen(childList4);i_ch++){ // for module-level dpoints
    dynAppend(childList4_new,childList4[i_ch]);
    limits_s[dynlen(childList4_new)]=limits_s[i_ch];
  }

  childList4=childList4_new;




  DebugTN("DeviceAlertSetting >>>>>>>>>>>>>>>>>>>>>>> ch_first="+ch_first+" ch_last="+ch_last+" dynlen(childList4)="+dynlen(childList4)+" number_per_HV_channel="+number_per_HV_channel);

} // if(device_type=="HV_1")



//--------------------------------------------------------------------------------------

//----
a_configExists=false;

//----

//if(strpos(child_s3,"primary")>=0)mudcsDebug("--->"+child_s3);
//if(strpos(child_s3,"primary")>=0)mudcsDebug("--->"+value);

     for(i4=1;i4<= dynlen(childList4); i4++){// loop over device values(maybe arrays)


//if(strpos(child_s3,"CRB")>0)mudcsDebug(child_s4[i4]);


if(a_configExists  &&  archiveClass != "delete" && !isAlert)break;

      if(isAlert && !isAlertChangeOnly){  // && !isAlertChangeOnly to_correct (09/20/2004) not to have warnings: check a: on/off b: alert config

       limits = strsplit(limits_s[i4], "|");
       
      }


/////DebugN("3 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.");
      sublimits=limits;
      child_s4=child_s3 + childList4[i4];
///mudcsDebug(child_s4);
      type1=dpElementType(child_s4);
      if(type1 == DPEL_FLOAT || type1 == DPEL_INT){
        if(device_type == "LV_1" || device_type == "TEMP_1" || device_type == "CHIP_1" ||
        device_type == "GAS_SX5" || device_type == "HV_1" || device_type == "WTH_SX5" 
        || device_type == "PT100" || device_type == "HV_PR" 
        || device_type == "CRB_1"|| device_type == "MRTN_1" || device_type == "WNR12_1" || device_type == "ALNM_1"){
///// DebugN("3 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>..........."+isAlert+isAlertChangeOnly+" "+child_s4);
          if(isAlert && !isAlertChangeOnly)mudcsLV_1_alert_set(isAlertChangeOnly,isAlertActive, child_s4, sublimits);
          else if(!isAlert){ 
           if(archiveClass != "delete"){
//---------
fwArchive_get(child_s4, a_configExists, a_archiveClass, a_archiveType, 
                        a_smoothProcedure, a_deadband, a_timeInterval, a_isActive, a_exceptionInfo);
if(a_configExists &&  archiveClass != "delete")break;
//---------


           fwArchive_setMultiple(makeDynString(child_s4), archiveClass, smoothType, smoothProcedure,	deadband, newTime, exceptionInfo);
DebugN("-----------------------------------------------------4>>>>>>>>>>>>>>>>>"+child_s4+"   "+archiveClass);
if(dynlen(exceptionInfo)>=1)DebugN(exceptionInfo);
DebugN("-----------------------------------------------------4>>>>>>>>>>>>>>>>>");
          // DebugN(child_s4);
           // mudcsDebug(archiveClass);
           }
           else
           fwArchive_deleteMultiple(makeDynString(child_s4), exceptionInfo);
          }
        } //LV_1; TEMP_1

      }
      else if(type1 == DPEL_STRUCT){
        childList5=dpNames(child_s4+".*");
        if(device_type == "LV_1"){ 
        for(i5=1;i5<= dynlen(childList5); i5++){// loop over values
//------------------------------------------------------------------------------------
          if(isAlert  && !isAlertChangeOnly)mudcsLV_1_alert_set(isAlertChangeOnly, isAlertActive, childList5[i5], sublimits);
          else if(!isAlert){
           if(archiveClass != "delete"){
DebugN("-----------------------------------------------------5>>>>>>>>>>>>>>>>>"+childList5[i5]);
//---------
fwArchive_get(childList5[i5], a_configExists, a_archiveClass, a_archiveType, 
                        a_smoothProcedure, a_deadband, a_timeInterval, a_isActive, a_exceptionInfo);
if(a_configExists &&  archiveClass != "delete")break;
//---------
fwArchive_setMultiple(makeDynString(childList5[i5]), archiveClass, smoothType, smoothProcedure,	deadband, newTime, exceptionInfo);
           }
           else 
fwArchive_deleteMultiple(makeDynString(childList5[i5]), exceptionInfo);

//DebugN("4++++++++++++++++++++++++++++++++++");
          }
          pos=strpos(childList5[i5],".v5");
          if(pos>=0 && pos!=(strlen(childList5[i5])-strlen(".v5")) )
            pos=strpos(substr(childList5[i5],pos+2),".v5");
//------------------------------------------------------------------------------------
          if(device_type!="LV_1" || strpos(child_s3,"13_C")<0 || 
             pos<0 )dynAppend(ilist5,childList5[i5]);
        } // for loop over values

         if(isAlert){
           if(isAlertActive){
            if(!isAlertChangeOnly){ 
               fwAlertConfig_createSummary( child_s4,makeDynString("","") , ilist5,"", makeDynString(),"",exceptionInfo);
               dpSetWait(mudcsAddSystem(child_s4 + ":_alert_hdl.._active"), true);
            }
           }
           // TAK NE DOLZHNO BYT' !!!! else dpSetWait(mudcsAddSystem(child_s4 + ":_alert_hdl.._active"), false); //
           else fwAlertConfig_deleteSummary( child_s4,exceptionInfo);
         }
          dynClear(ilist5);
         
        
        } // if(device_type == "LV_1")

      } // else if(type1 == DPEL_STRUCT)
        //pos=strpos(child_s4,".v5");
          pos=strpos(child_s4,".v5");
          if(pos>=0 && pos!=(strlen(child_s4)-strlen(".v5")) )
          pos=strpos(substr(child_s4,pos+2),".v5");
           
        if(device_type!="TEMP_1" || strpos(child_s3,"13_C")<0 || 
            pos<0 )dynAppend(ilist4,child_s4);
     } // loop over device values(maybe arrays)
//---------------------------------------------------------------------------------------
//////// DebugN("5>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"+isAlert+isAlertActive+isAlertChangeOnly+BROKER_FIRST+child_s3 +"."+ ":_alert_hdl.._active");
       if(isAlert){
         if(isAlertActive){ // different createSummary as compared to others 
          if(!isAlertChangeOnly){
//mudcsDebug(child_s3);
DebugTN("ITEMSTOSUMMARY_ITEMSTOSUMMARY_ITEMSTOSUMMARY_ITEMSTOSUMMARY_ITEMSTOSUMMARY_ITEMSTOSUMMARY_");
DebugTN(ilist4);
DebugTN("END_ITEMSTOSUMMARY_END_ITEMSTOSUMMARY_END_ITEMSTOSUMMARY_END_ITEMSTOSUMMARY_END_ITEMSTOSUMMARY_END_ITEMSTOSUMMARY_");

            fwAlertConfig_createSummary( child_s3+".",makeDynString("","") , ilist4 ,"", makeDynString(),"",exceptionInfo);


/*

int i_test;
dpGet(mudcsAddSystem("dyn_debug2."),dyn_debug);
  for(i_test=1;i_test<=dynlen(ilist4);i_test++){
   dynAppend(dyn_debug,ilist4[i_test]);
  }
dpSet(mudcsAddSystem("dyn_debug2."),dyn_debug);

*/





            // removing brokers: if(i3 >= (dynlen(childList3)-dynlen(brokers)+1) && BROKER_FIRST)dpSetWait(mudcsAddSystem(child_s3 +"."+ ":_alert_hdl.._active"), true); // FOR BROKER
          }
          else dpSetWait(mudcsAddSystem(child_s3 +"."+ ":_alert_hdl.._active"), true);
         }
         else{
           // removing brokers: if(i3 < (dynlen(childList3)-dynlen(brokers)+1) || !BROKER_FIRST) // NOT FOR BROKER 
           dpSetWait(mudcsAddSystem(child_s3 +"."+ ":_alert_hdl.._active"), false);//fwAlertConfig_deleteSummary( child_s3+".",exceptionInfo);          
         }

       }
       dynClear(ilist4);
     
    isDeviceTypeMet=true;

     } // if(strpos(child_s3,device_type_alias) !=-1 )


 /////////////       if(i3 != dynlen(childList3))dynAppend(ilist3,child_s3);// NOT FOR BROKER 

//mudcsDebug("2----------------------->"+child_s3);

   } //for devices 
  if(isDeviceTypeMet)BROKER_FIRST=false; // so that CSCLevel devices (like gas does not set it if the function is called for the LV e.g.)





}


//======================================================================
//===================================================================================

mudcsLV_1_alert_set(bool isAlertChangeOnly, bool isAlertActive, string dpe, dyn_float limits, bool isAck=false)
{
// !!!!! 08/10/2004 Attention: this function is never called now with isAlertChangeOnly=true;
// !!!!! the alerts set active in the DeviceAlertSetting

int j;
string test_string;
//=== new
string error_class, fatal_class;

if(isAck){
 error_class="_fwErrorAck";
 fatal_class="_fwFatalAck";
 }
else {
 error_class="_fwErrorNack";
 fatal_class="_fwFatalNack";
 }
//=== new
////////// DebugN("mudcsLV_1_alert_set:++++++++++++++++++++++++++++++++++"+" "+isAlertActive);
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

	    // read input data from the panel depending on the dpe type
		_fwConfigs_getConfigOptionsForDpeType(DPE_TYPE, fwConfigs_PVSS_ALERT_HDL, configType, exceptionInfo);
		switch(configType)
		{
			case fwConfigs_ANALOG_OPTIONS:
				alertType = DPCONFIG_ALERT_NONBINARYSIGNAL;
/////////				requestedRanges = rangeCombo.text;
requestedRanges = 3;
requestedRanges = dynlen(limits)+1; // new
	
				for (i = 1; i <= requestedRanges; i++)
				{ 
////////				getValue("alText" + i, "text", alertText[i]);
alertText[i]="";
///////					getValue("alClass" + i, "text", alertClass[i]);


if(i!=2)alertClass[i]="_fwErrorNack";
else alertClass[i]="";

// ================== new
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
if(requestedRanges == 4){
 if(i%2)alertClass[i]= error_class;//"_fwErrorNack";
 else alertClass[i]="";
 if(i==4)alertClass[i]=fatal_class;
}
else if(requestedRanges ==3){
 if(i%2)alertClass[i]= error_class;//"_fwErrorNack";
 else alertClass[i]="";
}
else if(requestedRanges ==2 ){
 if(i==2)alertClass[i]= error_class;//"_fwErrorNack";
 else alertClass[i]="";
}
//================ new

					if (alertClass[i] != "")
						alertClass[i] = alertClass[i] + ".";
				}
							
				// put the limits in a dyn_float 
///////				for (i = 1; i < requestedRanges; i++)
//////					getValue("limNum" + i, "text", limits[i]);
				break;
				
			case fwConfigs_BINARY_OPTIONS:	
//////				alertType = DPCONFIG_ALERT_BINARYSIGNAL;
//////				alertText = makeDynString(alText7.text, alText6.text);
//////	  	    
//////	   			// create alarm class list with valid range
//////				if (radio1.number == 1)
//////					alertClass = makeDynString("", alClass3.text + ".");
//////				else if (radio1.number == 0)
//////					alertClass = makeDynString(alClass3.text + ".", ""); 
				break;
			default:
				break;
		}

///// DebugN(DPE_LIST[1]+".......................");
		
fwAlertConfig_setMultiple(DPE_LIST, alertType, alertText, limits, alertClass,
makeDynString(), "", makeDynString(),  "", exceptionInfo);

if(dynlen(exceptionInfo)>=1)DebugN(DPE_LIST[1]+"..........................................................................................."+exceptionInfo[1]);

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
//mudcsDebug2(DPE_LIST[j]);
//}

//for(j=1;j<=dynlen(limits);j++){
//test_string=limits[j];
//mudcsDebug2(test_string);
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

//===================================================================================
//===================================================================================


//===================================================================================
mudcsAlertGroupAcknowlege(string groupDp){

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


mudcsAlertConfig(bool isAlertChangeOnly, bool isAlertActive, string dpe, dyn_float limits, bool isAck=false)
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
                                alertText = makeDynString("OK", "STATUS ERROR");
//////				alertText = makeDynString(alText7.text, alText6.text);
//////	  	    
//////	   			// create alarm class list with valid range
//////				if (radio1.number == 1)
                     alertClass = makeDynString("", error_class + ".");           
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
//mudcsDebug2(DPE_LIST[j]);
//}

//for(j=1;j<=dynlen(limits);j++){
//test_string=limits[j];
//mudcsDebug2(test_string);
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

//===================================================================================

/*
PVSS00ui1:2007.02.12 16:52:46.827["++++++++++++++++++++++++++++++++++++++++++++++++++++++11111"]
PVSS00ui1:2007.02.12 16:52:46.827[dyn_string 1 items
PVSS00ui1:     1:"CMS_CSC:CSC_ME_M3_PC1_LV_CRB.data.v15_2"
PVSS00ui1:]
PVSS00ui1:2007.02.12 16:52:46.827["alertType=13"]
PVSS00ui1:2007.02.12 16:52:46.827[dyn_string 3 items
PVSS00ui1:     1:""
PVSS00ui1:     2:""
PVSS00ui1:     3:""
PVSS00ui1:]
PVSS00ui1:2007.02.12 16:52:46.827[dyn_float 2 items
PVSS00ui1:     1:1.2
PVSS00ui1:     2:1.8
PVSS00ui1:]
PVSS00ui1:2007.02.12 16:52:46.828[dyn_string 3 items
PVSS00ui1:     1:"_fwErrorNack."
PVSS00ui1:     2:""
PVSS00ui1:     3:"_fwErrorNack."
PVSS00ui1:]
PVSS00ui1:2007.02.12 16:52:46.828["++++++++++++++++++++++++++++++++++++++++++++++++++++++11111"]
*/

//===================================================================================




mudcsAlertSetActive(string DpName1, bool isAlertActive)
{

 DebugN("___________))))))))))))))))))))))))))))))))))))))))))))1111111111111111111111111111111111111111111111111111111111111111111111111111111111");


string mudcs_alias;
string device_type, value;
dyn_string childList, childList2, childList3;
int i, i2, i3 , pos;
string test_string;
int w_pos, radius, ich;



dyn_string dummy_list;
dyn_string  dummy_list2= makeDynString(DpName1);
 mudcs_alias=mudcsAliasNameGet(DpName1);
mudcsNameToPosConvert(mudcs_alias/*DpName1*/, w_pos, radius, ich, device_type);
/*
if(strpos(device_type,"CRB")>=0){
dyn_string dyn_debug;
dpGet(mudcsAddSystem("dyn_debug2."),dyn_debug);
   dynAppend(dyn_debug,device_type); 
dpSet(mudcsAddSystem("dyn_debug2."),dyn_debug);
}
*/
bool F_B=false;
DeviceAlertSetting(F_B, dummy_list2, true, true,  isAlertActive, device_type, value, makeDynString() , DpName1,
"",0,0,0.,0., dummy_list);
return;

/*
//==============================================================
////device_type="LV_1";
////mudcsDebug("1");

dyn_string dpsplit;
dpsplit=strsplit(DpName1,"."); 

if(dynlen(dpsplit)>=2)
DpName1=dpsplit[1];
else 
DpName1=DpName1;

//DpName1=substr(0,strpos(DpName1,":"));
//DebugN("***==="+DpName1);

///mudcsNameToPosConvert(DpName1, w_pos, radius, ich, device_type);

//DebugN("***====="+device_type);

 childList=dpNames(DpName1+".data.*");
 for(i=1;i<= dynlen(childList); i++){// loop over values

  childList2=dpNames(childList[i]+".*");

  value="";
  for(i2=1;i2<= dynlen(childList2); i2++){// loop over values2  

    pos = strpos(childList2[i2],".data.");
    test_string = substr(childList2[i2],pos);
     
    if(device_type == "LV_1") {
         if(strpos(test_string,"_o.c") >=0)continue;
         if(strpos(test_string,"_o.status") >=0)continue;
    }
    value= value + test_string;
    if(i2 != dynlen(childList2))value=value+"|";

  }  //loop over values2  

///////mudcsDebug(device_type + "\n" + value);

//DebugN("******************8"+isAlertActive+" "+device_type+" "+value+" "+DpName1+" ");
//mudcsLookAndAlertArchiveConfig(true, true, isAlertActive, device_type, value, makeDynString() , DpName1,
//"",0,0,0.,0.);


//DeviceAlertSetting(bool &BROKER_FIRST, dyn_string &childList3, bool isAlert, 
//bool isAlertChangeOnly, bool isAlertActive,
//string device_type, string value, dyn_string limits_s, string current_dpName,
//string archiveClass, int smoothType, int smoothProcedure,float deadband,float newTime, dyn_string &ilist3)

dyn_string dummy_list;
//dyn_string dummy_list1=strsplit(DpName1,"/");
//dyn_string  dummy_list2= makeDynString(dummy_list1[dynlen(dummy_list1)]);
dyn_string  dummy_list2= makeDynString(DpName1);
bool F_B=false;

 DebugN("___________))))))))))))))))))))))))))))))))))))))))))))1");
DeviceAlertSetting(F_B, dummy_list2, true, true,  isAlertActive, device_type, value, makeDynString() , DpName1,
"",0,0,0.,0., dummy_list);



 DebugN("___________))))))))))))))))))))))))))))))))))))))))))))2");

/////mudcsLV_1_alert_set(true, isAlertActive, DpName1, makeDynString());

 } //loop over values



///value=".data.Alct_o.v18|"
///         +".data.Alct_o.v33|"
///         +".data.Alct_o.v55|"
///         +".data.Alct_o.v56";
///

*/


}

//====================================================================================================
mudcsGetAlctDefaults(float &v18, float &v33, float &v55, float &v56){

///string CSC_fwG_g_SYSTEM_NAME;
///CSC_fwG_g_SYSTEM_NAME="mudcs";

 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o")){


  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v18"), v18);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v33"), v33);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v55"), v55);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v56"), v56);

  

 }


}
//====================================================================================================
mudcsGetAlctDeltas(float &v18, float &v33, float &v55, float &v56){

///string CSC_fwG_g_SYSTEM_NAME;
///CSC_fwG_g_SYSTEM_NAME="mudcs";

 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o")){


  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_delta.Alct_o.v18"), v18);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_delta.Alct_o.v33"), v33);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_delta.Alct_o.v55"), v55);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_delta.Alct_o.v56"), v56);


 }


}

//====================================================================================================
mudcsSetAlctDefaults(float v18, float v33, float v55, float v56){

///string CSC_fwG_g_SYSTEM_NAME;
///CSC_fwG_g_SYSTEM_NAME="mudcs";

 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o")){


  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v18"), v18);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v33"), v33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v55"), v55);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v56"), v56);

  

 }


}
//====================================================================================================
mudcsSetAlctDeltas(float v18, float v33, float v55, float v56){

///string CSC_fwG_g_SYSTEM_NAME;
///CSC_fwG_g_SYSTEM_NAME="mudcs";

 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o")){


  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_delta.Alct_o.v18"), v18);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_delta.Alct_o.v33"), v33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_delta.Alct_o.v55"), v55);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_delta.Alct_o.v56"), v56);


 }


}


//====================================================================================================


//====================================================================================================
mudcsGetCfebDefaults(float &v33, float &v50, float &v60){

///string CSC_fwG_g_SYSTEM_NAME;
///CSC_fwG_g_SYSTEM_NAME="mudcs";


 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o")){

  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v33.v1"), v33);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v50.v1"), v50);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v60.v1"), v60);

 }




}
//====================================================================================================
mudcsGetCfebDeltas(float &v33, float &v50, float &v60){

///string CSC_fwG_g_SYSTEM_NAME;
///CSC_fwG_g_SYSTEM_NAME="mudcs";

 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o")){

  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_delta.Cfeb_o.v33.v1"), v33);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_delta.Cfeb_o.v50.v1"), v50);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_delta.Cfeb_o.v60.v1"), v60);

 }


}
//====================================================================================================
//====================================================================================================
mudcsSetCfebDefaults(float v33, float v50, float v60){

///string CSC_fwG_g_SYSTEM_NAME;
///CSC_fwG_g_SYSTEM_NAME="mudcs";



 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o")){

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v33.v1"), v33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v50.v1"), v50);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v60.v1"), v60);

 }




}
//====================================================================================================
mudcsSetCfebDeltas(float v33, float v50, float v60){

///string CSC_fwG_g_SYSTEM_NAME;
///CSC_fwG_g_SYSTEM_NAME="mudcs";
//////  mudcsDebug(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.");
 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o")){

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_delta.Cfeb_o.v33.v1"), v33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_delta.Cfeb_o.v50.v1"), v50);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_delta.Cfeb_o.v60.v1"), v60);


 }


}
//====================================================================================================
//====================================================================================================
//====================================================================================================
//====================================================================================================
//====================================================================================================
mudcsSetAlertTemperatures(float dmb_min, float alct_min, float cfebs_min, float dmb_max, float alct_max, float cfebs_max ){


 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_TEMP_1_alert_o")){

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_TEMP_1_alert_o.min.t_dmb"), dmb_min);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_TEMP_1_alert_o.min.t_alct"), alct_min);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_TEMP_1_alert_o.min.t_cfebs"), cfebs_min);

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_TEMP_1_alert_o.max.t_dmb"), dmb_max);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_TEMP_1_alert_o.max.t_alct"), alct_max);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_TEMP_1_alert_o.max.t_cfebs"), cfebs_max);

 }


}
//====================================================================================================
mudcsGetAlertTemperatures(float &dmb_min, float &alct_min, float &cfebs_min, float &dmb_max, float &alct_max, float &cfebs_max ){


 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_TEMP_1_alert_o")){
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_TEMP_1_alert_o.min.t_dmb"), dmb_min);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_TEMP_1_alert_o.min.t_alct"), alct_min);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_TEMP_1_alert_o.min.t_cfebs"), cfebs_min);

  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_TEMP_1_alert_o.max.t_dmb"), dmb_max);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_TEMP_1_alert_o.max.t_alct"), alct_max);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_TEMP_1_alert_o.max.t_cfebs"), cfebs_max);

 }


}

//====================================================================================================

mudcsSetAlctAlert(float v18,float v33,float v55,float v56,float rel_delta_volt,
float c18,float c33,float c55,float c56,float rel_delta_curr){

 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o")){

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v18"), v18);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v33"), v33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v55"), v55);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v56"), v56);

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.c18"), c18);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.c33"), c33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.c55"), c55);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.c56"), c56);
  
   dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.rel_delta_volt_alct"), rel_delta_volt); 
   dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.rel_delta_curr_alct"), rel_delta_curr); 

 }



}
//====================================================================================================

mudcsGetAlctAlert(float &v18,float &v33,float &v55,float &v56,float &rel_delta_volt,
float &c18,float &c33,float &c55,float &c56,float &rel_delta_curr){

 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o")){

  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v18"), v18);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v33"), v33);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v55"), v55);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.v56"), v56);

  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.c18"), c18);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.c33"), c33);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.c55"), c55);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Alct_o.c56"), c56);
  
   dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.rel_delta_volt_alct"), rel_delta_volt); 
   dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.rel_delta_curr_alct"), rel_delta_curr); 

 }



}
//====================================================================================================

mudcsSetCfebAlert(float v33,float v50,float v60, float rel_delta_volt,
float c33,float c50,float c60, float rel_delta_curr){

 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o")){

   //voltage
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v33.v1"), v33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v50.v1"), v50);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v60.v1"), v60);

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v33.v2"), v33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v50.v2"), v50);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v60.v2"), v60);

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v33.v3"), v33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v50.v3"), v50);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v60.v3"), v60);

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v33.v4"), v33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v50.v4"), v50);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v60.v4"), v60);

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v33.v5"), v33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v50.v5"), v50);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v60.v5"), v60);

  //current
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c33.v1"), c33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c50.v1"), c50);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c60.v1"), c60);

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c33.v2"), c33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c50.v2"), c50);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c60.v2"), c60);

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c33.v3"), c33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c50.v3"), c50);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c60.v3"), c60);

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c33.v4"), c33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c50.v4"), c50);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c60.v4"), c60);

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c33.v5"), c33);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c50.v5"), c50);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c60.v5"), c60);

  //deltas
   dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.rel_delta_volt_cfeb"), rel_delta_volt); 
   dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.rel_delta_curr_cfeb"), rel_delta_curr); 

 }



}
//====================================================================================================

mudcsGetCfebAlert(float &v33,float &v50,float &v60, float &rel_delta_volt,
float &c33,float &c50,float &c60, float &rel_delta_curr){

 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o")){

  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v33.v1"), v33);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v50.v1"), v50);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.v60.v1"), v60);

  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c33.v1"), c33);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c50.v1"), c50);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.LV_1_default.Cfeb_o.c60.v1"), c60);
  
   dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.rel_delta_volt_cfeb"), rel_delta_volt); 
   dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_LV_1_alert_o.rel_delta_curr_cfeb"), rel_delta_curr); 

 }



}


//====================================================================================================
//====================================================================================================
//====================================================================================================
//====================================================================================================
//====================================================================================================
mudcsSetAlertGasMeans(float m1_ar, float m1_co2, 
float m2_ar, float m2_co2, float m2_cf4  ){


 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o")){


  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.mean.Mixer1_o.ar"), m1_ar);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.mean.Mixer1_o.co2"), m1_co2);

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.mean.Mixer2_o.ar"), m2_ar);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.mean.Mixer2_o.co2"), m2_co2);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.mean.Mixer2_o.cf4"), m2_cf4);


 }


}
//====================================================================================================
mudcsGetAlertGasMeans(float &m1_ar, float &m1_co2, 
float &m2_ar, float &m2_co2, float &m2_cf4  ){


 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o")){


  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.mean.Mixer1_o.ar"), m1_ar);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.mean.Mixer1_o.co2"), m1_co2);

  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.mean.Mixer2_o.ar"), m2_ar);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.mean.Mixer2_o.co2"), m2_co2);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.mean.Mixer2_o.cf4"), m2_cf4);


 }


}
//====================================================================================================
mudcsSetAlertGasDeltas(float m1_ar, float m1_co2, 
float m2_ar, float m2_co2, float m2_cf4  ){


 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o")){


  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.delta.Mixer1_o.ar"), m1_ar);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.delta.Mixer1_o.co2"), m1_co2);

  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.delta.Mixer2_o.ar"), m2_ar);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.delta.Mixer2_o.co2"), m2_co2);
  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.delta.Mixer2_o.cf4"), m2_cf4);


 }


}
//====================================================================================================
mudcsGetAlertGasDeltas(float &m1_ar, float &m1_co2, 
float &m2_ar, float &m2_co2, float &m2_cf4  ){


 if(dpExists(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o")){


  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.delta.Mixer1_o.ar"), m1_ar);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.delta.Mixer1_o.co2"), m1_co2);

  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.delta.Mixer2_o.ar"), m2_ar);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.delta.Mixer2_o.co2"), m2_co2);
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_GAS_SX5_alert_o.delta.Mixer2_o.cf4"), m2_cf4);


 }


}
//====================================================================================================


//============================================================
int percentage_calculation(float ar_f, float co2_f, float cf4_f){
 
float ar, co2, cf4;

float sum= ar_f+co2_f+cf4_f;
ar=ar_f/sum;
co2=co2_f/sum;
cf4=cf4_f/sum;

/////mudcsDebug(ar+" "+co2+" "+cf4+" ");

if(fabs(ar-0.4)>(0.4*0.12))return 1;
if(fabs(co2-0.5)>(0.5*0.12))return 1;
if(fabs(cf4-0.1)>(0.1*0.12))return 1;

return 0;


}
//============================================================
//============================================================
hv_master_off()
{
string host, subcommand;
int i;


if(CSC_fwG_g_SYSTEM_NAME=="System1"){

 host="100";

 subcommand="HVCMD;2;15;0;30;0;-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

 subcommand="HVCMD;2;15;255;0;0;-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
 //-------
 subcommand="HVCMD;2;14;0;30;0;-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

 subcommand="HVCMD;2;14;255;0;0;-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

}
else if(CSC_fwG_g_SYSTEM_NAME==CSC_fwG_CSC_fwG_g_SYSTEM_NAME2){


 host="100";

 subcommand="HVCMD;2;15;0;30;0;-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

 subcommand="HVCMD;2;15;255;0;0;-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
 //-------
 subcommand="HVCMD;2;14;0;30;0;-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

 subcommand="HVCMD;2;14;255;0;0;-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);



 host="200";

 for(i=0;i<dynlen(CSC_fwG_g_all_masters);i++){
  subcommand="HVCMD;0;"+i+";0;30;0;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

  subcommand="HVCMD;0;"+i+";255;0;0;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
 }
}



}

//============================================================
//============================================================

// *************** ALERT RECONFIG FUNCTIONS ***************************

 //=========================
dyn_string mudcsTempAlertReconfig(dyn_int &off_channels, dyn_string &ilist_new){

dyn_string ilist, f_inds;;

  string  s_j;
  int i, j, pos, ind, f_ind;

 
   for(j=1;j<=dynlen(off_channels);j++){
     s_j=off_channels[j];
     f_ind=0;
 //-------------------------    
     if(off_channels[j] <=5){
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         if(strpos(ilist_new[ind],"cfeb.v"+s_j)>=0){f_ind=ind;break;}
       } // for ind
     }
     else if(off_channels[j] ==6){
      for(ind=1;ind<=dynlen(ilist_new);ind++){
         if(strpos(ilist_new[ind],"alct.")>=0){f_ind=ind;break;}
       } // for ind      
     }
     else if(off_channels[j] ==7){
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         if(strpos(ilist_new[ind],"tmb.")>=0){f_ind=ind;break;}
       } // for ind   
     }     
//---------------------------     
     if(f_ind!=0)dynAppend(f_inds,f_ind);
     
   } // for j
   
       for(ind=1;ind<=dynlen(ilist_new);ind++){
             if(!dynContains(f_inds,ind))dynAppend(ilist,ilist_new[ind]);
            // else mudcsDebug(""); 
       } // for ind          
   
  return ilist;
  
}
//========================

dyn_string mudcsLvCfebAlertReconfig(dyn_int &off_channels, dyn_string &ilist_new, string dpe){
  
 
dyn_string ilist, f_inds;;

  string  s_j;
  int i, j, pos, ind, f_ind;

 
   for(j=1;j<=dynlen(off_channels);j++){
     s_j=off_channels[j];
     f_ind=0;
 //-------------------------    
     if(off_channels[j] <=5){
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         if(strpos(ilist_new[ind],dpe+".v"+s_j)>=0){dynAppend(f_inds,ind);}         
       } // for ind
     }

      
 
//---------------------------     
 //    if(f_ind!=0)dynAppend(f_inds,f_ind);
     
   } // for j
   
       for(ind=1;ind<=dynlen(ilist_new);ind++){
             if(!dynContains(f_inds,ind))dynAppend(ilist,ilist_new[ind]);
            // else mudcsDebug(""); 
       } // for ind          
   
  return ilist;
   
  
}
//========================
//========================

dyn_string mudcsLvAlctAlertReconfig(dyn_int &off_channels, dyn_string &ilist_new){
  
 
dyn_string ilist, f_inds;;

  string  s_j;
  int i, j, pos, ind, f_ind;

 
   for(j=1;j<=dynlen(off_channels);j++){
     s_j=off_channels[j];
     f_ind=0;
 //-------------------------    
 
     if(off_channels[j] ==6){
      for(ind=1;ind<=dynlen(ilist_new);ind++){
         if(strpos(ilist_new[ind],"Alct_o.")>=0){dynAppend(f_inds,ind);}                                      
       } // for ind      
     }
 
//---------------------------     
 //    if(f_ind!=0)dynAppend(f_inds,f_ind);
     
   } // for j
   
       for(ind=1;ind<=dynlen(ilist_new);ind++){
             if(!dynContains(f_inds,ind))dynAppend(ilist,ilist_new[ind]);
            // else mudcsDebug(""); 
       } // for ind          
   
  return ilist;
   
  
}
//========================
dyn_string mudcsHvAlertReconfig(dyn_int &off_channels, dyn_string &ilist_new){
  

dyn_string ilist, f_inds;;

  string  s_j;
  int i, j, pos, ind, f_ind;

 
   for(j=1;j<=dynlen(off_channels);j++){
     s_j=off_channels[j];
     f_ind=0;
 //-------------------------    
     
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         if(strpos(ilist_new[ind],".v"+s_j)>=0 && strpos(ilist_new[ind],"master")<0){f_ind=ind;break;}
       } // for ind
     
 
//---------------------------     
     if(f_ind!=0)dynAppend(f_inds,f_ind);
     
   } // for j
   
       for(ind=1;ind<=dynlen(ilist_new);ind++){
             if(!dynContains(f_inds,ind))dynAppend(ilist,ilist_new[ind]);
            // else mudcsDebug(""); 
       } // for ind          
   
  return ilist;
    
  
}//========================
dyn_string mudcsCrbAlertReconfig(dyn_int &off_channels, dyn_string &ilist_new){

//-------------------------  
  mudcsCrbPnlInit();  
//-------------------------
dyn_string ilist, f_inds, dp_frag;

  string  s_j;
  int i, j, pos, ind, f_ind, i10, chn;

 
   for(j=1;j<=dynlen(off_channels);j++){
     chn=off_channels[j];
     
     s_j=off_channels[j];
     f_ind=0;
 //-------------------------    
     if(off_channels[j] <=9){
   //   mudcsDebug(chn+" ; "+off_channels[j] );
      dp_frag=makeDynString("ai_"+CSC_fwCAN1_g_LIST_5V[chn],
                      "ai_"+CSC_fwCAN1_g_LIST_3V1[chn],"ai_"+CSC_fwCAN1_g_LIST_3V2[chn]);       
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         for(i10=1;i10<=dynlen(dp_frag);i10++){
          if(strpos(ilist_new[ind],dp_frag[i10])>=0){dynAppend(f_inds,ind);}
         }
       } // for ind
     }
     else if(off_channels[j] ==10){
      dp_frag=makeDynString("ai_"+CSC_fwCAN1_g_LIST_MPC[1],"ai_"+CSC_fwCAN1_g_LIST_MPC[2]);  
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         for(i10=1;i10<=dynlen(dp_frag);i10++){
          if(strpos(ilist_new[ind],dp_frag[i10])>=0){dynAppend(f_inds,ind);}
         }
       } // for ind
     }
     else if(off_channels[j] ==11){
      dp_frag=makeDynString("ai_"+CSC_fwCAN1_g_LIST_CC[1],"ai_"+CSC_fwCAN1_g_LIST_CC[2]);
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         for(i10=1;i10<=dynlen(dp_frag);i10++){
           if(strpos(ilist_new[ind],dp_frag[i10])>=0){dynAppend(f_inds,ind);}
         }
       } // for ind
     }
     else if(off_channels[j] ==12){
      dp_frag=makeDynString("ai_"+CSC_fwCAN1_g_LIST_15V[1],"ai_"+CSC_fwCAN1_g_LIST_15V[2]);   
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         for(i10=1;i10<=dynlen(dp_frag);i10++){
          if(strpos(ilist_new[ind],dp_frag[i10])>=0){dynAppend(f_inds,ind);}
         }
       } // for ind
     }

//---------------------------     
//     if(f_ind!=0)dynAppend(f_inds,f_ind);
     
   } // for j
   
       for(ind=1;ind<=dynlen(ilist_new);ind++){
             if(!dynContains(f_inds,ind))dynAppend(ilist,ilist_new[ind]);
            // else mudcsDebug(""); 
       } // for ind          
   
  return ilist;
    
  
}
//========================

//=========================
mudcsAlertReconfig(string type, string dpe, bool isLocal=false){
  
 
  
  string data, system_name, s_j, dpe2="";
  int i, j, pos, ind, f_ind;
  dyn_string dps, split;
  dyn_int off_channels;
  
bool configExists;
dyn_string alertTexts;
dyn_string ilist, ilist_new;
string alertPanel;
dyn_string alertPanelParameters;
string alertHelp;
bool isActive;
dyn_string exceptionInfo;

if(strpos(type,";")>=0){
  split=strsplit(type,";");
  type=split[1];
  dpe2=split[2]; 
}
  if(isLocal)dps=dpNames("*",type);
  else dps=dpNames("*:*",type); 
  

    
  for(i=1;i<=dynlen(dps);i++){
  //  DebugTN("aler:     "+dps[i]);
   pos=strpos(dps[i],":");
   if(pos>0)system_name=substr(dps[i],0,pos+1); // with ":"
   else system_name="";
   if(type=="fwCrb_CSC_LV")data=dps[i]+".";
   else if(dpe2!="")data = system_name+substr(dps[i],strpos(dps[i],"/")+1)+dpe2;
   else data=system_name+substr(dps[i],strpos(dps[i],"/")+1)+".";
       
   dpGet(dps[i]+dpe,off_channels);
   
  fwAlertConfig_getSummary(data,configExists,alertTexts,ilist,
  alertPanel,alertPanelParameters,alertHelp,isActive,exceptionInfo);
  
  if(dynlen(off_channels)>0){
   //DebugTN(ilist); 
  }
  
   ilist_new=ilist;
   dynClear(ilist);
   //========================================
   
  if(type=="TEMP_1")ilist=mudcsTempAlertReconfig(off_channels, ilist_new);
  else if(type=="LV_1"&& dpe2=="")ilist=mudcsLvAlctAlertReconfig(off_channels, ilist_new);
  else if(type=="LV_1"&& dpe2!="")ilist=mudcsLvCfebAlertReconfig(off_channels, ilist_new, dpe2);  
  else if(type=="HV_1")ilist=mudcsHvAlertReconfig(off_channels, ilist_new);
  else if(type=="fwCrb_CSC_LV")ilist=mudcsCrbAlertReconfig(off_channels, ilist_new);
        
  
 //========================================  
   if(dynlen(off_channels)>0){
 //    DebugTN("alert");
 //    DebugTN(ilist);
     if(ilist!=ilist_new){
     DebugTN(ilist);         
  //     mudcsDebug(dps[i]);
   fwAlertConfig_createSummary(data,makeDynString("","") , ilist ,"", 
                              makeDynString(),"",exceptionInfo);  
     } // if(ilist!=ilist_new)
   }
  } // for i
  
}
//========================
