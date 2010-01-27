


//=============================
GasSystem_main(){
int i;

create_MUDCS_STRING(); // just in case
dpCreate("CMS_CSC_GAS","MUDCS_STRING");

DeviceType_create("fwCooling_CSC_COOLING");
CscCooling_create_data(); // water cooling
CscCooling_turbines_create_data(); // turbines cooling
CscCooling_turbines_create_summary();

DeviceType_create("fwGasSystem_CSC_GAS");
GasSystem_create_data();
GasMixer_create_data();
GasDistribution_create_data();
GasDistributionRack_create_data();
GasDistributionChannel_create_data();
GasPreDistribution_create_data();
GasPump_create_data();
GasExhaust_create_data();
GasPurifier_create_data();

DebugTN("types: fwGasSystem_CSC_GAS created");
//=========================================
//---------
dpCreate("fwCooling_CSC_COOLING_DimBroker_o","fwCooling_CSC_COOLING_d");
DebugTN("dp: fwCooling_CSC_COOLING_DimBroker_o created");
//---------
dpCreate("fwCooling_CSC_COOLINGInfo","_FwDeviceDefinition");
dpSet("fwCooling_CSC_COOLINGInfo.type","CoolingCMS Device (1)");
dpSet("fwCooling_CSC_COOLINGInfo.dpType","fwCooling_CSC_COOLING");
DebugTN("fwCooling_CSC_COOLINGInfo created");
//----------
dpCreate("CoolingCMS","FwNode");
dpSet("CoolingCMS.type","VENDOR");
dpSet("CoolingCMS.dpTypes",makeDynString("fwCooling_CSC_COOLING"));
dpSet("CoolingCMS.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("CoolingCMS.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("CoolingCMS.model","CoolingCMS");
dpSet("CoolingCMS.fsmState",0);
dpSet("CoolingCMS.fsmCommand",0);
DebugTN("types: CoolingCMS of FwNode created");


//===========================================
//---------
dpCreate("fwGasSystem_CSC_GAS_DimBroker_o","fwGasSystem_CSC_GAS_d");
DebugTN("dp: GasSystem_DimBroker_o created");
//---------
dpCreate("fwGasSystem_CSC_GASInfo","_FwDeviceDefinition");
dpSet("fwGasSystem_CSC_GASInfo.type","GasCMS Device (1)");
dpSet("fwGasSystem_CSC_GASInfo.dpType","fwGasSystem_CSC_GAS");
DebugTN("fwGasSystem_CSC_GASInfo created");
//----------
dpCreate("GasCMS","FwNode");
dpSet("GasCMS.type","VENDOR");
dpSet("GasCMS.dpTypes",makeDynString("fwGasSystem_CSC_GAS"));
dpSet("GasCMS.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("GasCMS.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("GasCMS.model","GasCMS");
dpSet("GasCMS.fsmState",0);
dpSet("GasCMS.fsmCommand",0);
DebugTN("types: GasCMS of FwNode created");

//if(isStandalone)return;
mudcsInit();
DebugTN("mudcsInit()");
//-----------
if(!(dpExists("Db_o")) )return;
mudcs_setBrokerList();
DebugTN("mudcs_setBrokerList()");
//------------
//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
dyn_string device_list;
string device_info;

mudcsGetDeviceList(device_list);
for(int i=1;i<=dynlen(device_list);i++){

  device_info= device_list[i]+"Info.panels.editor.hardware";
  if(dpExists(device_list[i]+"Info"))dpSet(device_info,CSC_fwG_g_Component+"/config/emuDev" + device_list[i]+"Config");

  device_info= device_list[i]+"Info.panels.navigator.hardware";
  if(dpExists(device_list[i]+"Info"))dpSet(device_info,CSC_fwG_g_Component+"/emuDev" + device_list[i]+"Operation");
    
}
DebugTN("panels set");
//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF

//mudcsDebug("done");
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++fwGasSystem_CSC_GAS done");
}
//============================

create_MUDCS_STRING(){
  
 dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString ("MUDCS_STRING","");
xxdepes[2] = makeDynString ("","value");


xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_STRING);


dpTypeCreate(xxdepes,xxdepei);
 
}


//=============================
//============================
DeviceType_create(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","chamber_state");
xxdepes[3] = makeDynString ("","coord");
xxdepes[4] = makeDynString ("","status");
xxdepes[5] = makeDynString ("","off_channels");

//xxdepes[6] = makeDynString ("","dynatemTcpIp");
//xxdepes[7] = makeDynString ("","update_value");

xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_DYN_INT);

//xxdepei[6]=makeDynInt (0,DPEL_STRING);
//xxdepei[7]=makeDynInt (0,DPEL_INT);

dpTypeCreate(xxdepes,xxdepei);

}
//=============================
CscCooling_turbines_create_data()
{


string dtype= "fwCooling_CSC_TURBINES_data";
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");
//xxdepes[2] = makeDynString ("","value");
//xxdepes[3] = makeDynString ("","Temperature1");
//xxdepes[4] = makeDynString ("","TurbineCurrent1");
//xxdepes[5] = makeDynString ("","TurbineCurrent2");
//xxdepes[6] = makeDynString ("","off_channels");
//--------------

xxdepei[1]=makeDynInt (DPEL_FLOAT);
//xxdepei[1]=makeDynInt (DPEL_STRUCT);
//xxdepei[2]=makeDynInt (0,DPEL_FLOAT);
//xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
//xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
//xxdepei[5]=makeDynInt (0,DPEL_FLOAT);
//xxdepei[6]=makeDynInt (0,DPEL_DYN_INT);

dpTypeCreate(xxdepes,xxdepei);

}
//=============================

CscCooling_turbines_create_summary()
{


string dtype= "fwCooling_CSC_TURBINES_summary";
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","value");
//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_FLOAT);

dpTypeCreate(xxdepes,xxdepei);

dpCreate("mrtn_racks_summary","fwCooling_CSC_TURBINES_summary");
dpCreate("hv_racks_summary","fwCooling_CSC_TURBINES_summary");
dpCreate("pc_racks_summary","fwCooling_CSC_TURBINES_summary");
dpCreate("align_racks_summary","fwCooling_CSC_TURBINES_summary");
dpCreate("water_summary","fwCooling_CSC_TURBINES_summary");
}

//=============================
//=============================
/*
CscCooling_turbines_create_data()
{


string dtype= "fwCooling_CSC_TURBINES_data";
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","Temperature2");
xxdepes[3] = makeDynString ("","Temperature1");
xxdepes[4] = makeDynString ("","TurbineCurrent1");
xxdepes[5] = makeDynString ("","TurbineCurrent2");
xxdepes[6] = makeDynString ("","off_channels");
//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6]=makeDynInt (0,DPEL_DYN_INT);

dpTypeCreate(xxdepes,xxdepei);

}
*/
//=============================

CscCooling_create_data()
{


string dtype= "fwCooling_CSC_COOLING_d";
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","Flowmeter_EndCap_Cooling_YE_Minus_1");
xxdepes[3] = makeDynString ("","Flowmeter_EndCap_Cooling_YE_Plus_1");
xxdepes[4] = makeDynString ("","Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Minus_1");
xxdepes[5] = makeDynString ("","Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Minus_1");
xxdepes[6] = makeDynString ("","Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Plus_1");
xxdepes[7] = makeDynString ("","Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Plus_1");
xxdepes[8] = makeDynString ("","off_channels");
//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6]=makeDynInt (0,DPEL_FLOAT);
xxdepei[7]=makeDynInt (0,DPEL_FLOAT);
xxdepei[8]=makeDynInt (0,DPEL_DYN_STRING);

dpTypeCreate(xxdepes,xxdepei);

}
//=============================
//=============================
GasSystem_create_data()
{


string dtype= "fwGasSystem_CSC_GAS_d";
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","State");
xxdepes[3] = makeDynString ("","Status");
xxdepes[4] = makeDynString ("","off_channels");
//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_BOOL);
xxdepei[4]=makeDynInt (0,DPEL_DYN_INT);

dpTypeCreate(xxdepes,xxdepei);

}
//=============================
GasMixer_create_data(){ 
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,j;
string dtype= "fwGasMixer_CSC_GAS_data"; 
 
xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","State");
xxdepes[3] = makeDynString ("","Interlock");
xxdepes[4]=makeDynString ("","LiquidLevel");
xxdepes[5]=makeDynString ("","EvaporatorTemperature");
xxdepes[6]=makeDynString ("","OutPressure");
xxdepes[7]=makeDynString ("","TotalFlow");
xxdepes[8] = makeDynString ("","off_channels");
for(i=1;i<=3;i++){
xxdepes[8+(i-1)*5+1]=makeDynString ("","Line"+i+"Ratio");
xxdepes[8+(i-1)*5+2]=makeDynString ("","Line"+i+"InputPressure");
xxdepes[8+(i-1)*5+3]=makeDynString ("","Line"+i+"LFmfcFeedback");
xxdepes[8+(i-1)*5+4]=makeDynString ("","Line"+i+"HFmfcFeedback");
xxdepes[8+(i-1)*5+5]=makeDynString ("","Line"+i+"WaterTankLevel");
}

//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_BOOL);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6]=makeDynInt (0,DPEL_FLOAT);
xxdepei[7]=makeDynInt (0,DPEL_FLOAT);
xxdepei[8]=makeDynInt (0,DPEL_DYN_INT);
for(i=1;i<=3;i++){
xxdepei[8+(i-1)*5+1]=makeDynInt (0,DPEL_FLOAT);
xxdepei[8+(i-1)*5+2]=makeDynInt (0,DPEL_FLOAT);
xxdepei[8+(i-1)*5+3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[8+(i-1)*5+4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[8+(i-1)*5+5]=makeDynInt (0,DPEL_FLOAT);
}

dpTypeCreate(xxdepes,xxdepei); 
} 
//=============================
GasDistribution_create_data(){ 
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,j;
string dtype= "fwGasDistribution_CSC_GAS_data"; 
 xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","State");
xxdepes[3] = makeDynString ("","BUSource");
xxdepes[4]=makeDynString ("","TotalInputFlow");
xxdepes[5] = makeDynString ("","off_channels");
//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_BOOL);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_DYN_INT);
 
dpTypeCreate(xxdepes,xxdepei);   
  
}
//=============================
GasDistributionRack_create_data(){ 
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,j;
string dtype= "fwGasDistributionRack_CSC_GAS_data"; 
 
 xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","InPressure22");
xxdepes[3] = makeDynString ("","ChPressure");
xxdepes[4]=makeDynString ("","RegPressure");
xxdepes[5]=makeDynString ("","StateRack");
xxdepes[6]=makeDynString ("","InPressure24");
xxdepes[7]=makeDynString ("","OutManifoldPressure");
xxdepes[8] = makeDynString ("","off_channels");
//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_INT);
xxdepei[6]=makeDynInt (0,DPEL_FLOAT);
xxdepei[7]=makeDynInt (0,DPEL_FLOAT);
xxdepei[8]=makeDynInt (0,DPEL_DYN_INT);

dpTypeCreate(xxdepes,xxdepei);   
  
}
//=============================
GasDistributionChannel_create_data(){ 
 dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,j;
string dtype= "fwGasDistributionChannel_CSC_GAS_data"; 
 
 xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","Pressure");
xxdepes[3] = makeDynString ("","InFlow");
xxdepes[4]=makeDynString ("","OutFlow");
xxdepes[5]=makeDynString ("","Isolation-Valve");
xxdepes[6] = makeDynString ("","off_channels");
//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6]=makeDynInt (0,DPEL_DYN_INT);

dpTypeCreate(xxdepes,xxdepei);  
  
  
}
//=============================
GasPreDistribution_create_data(){ 
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,j;
string dtype= "fwGasPreDistribution_CSC_GAS_data"; 
 xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","State");
xxdepes[3] = makeDynString ("","InPressure");
xxdepes[4]=makeDynString ("","OutPressure");
xxdepes[5] = makeDynString ("","off_channels");
//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_DYN_INT);
 
dpTypeCreate(xxdepes,xxdepei);   
  
  
}
//=============================
GasPump_create_data(){ 
 dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,j;
string dtype= "fwGasPump_CSC_GAS_data"; 
xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","State");
xxdepes[3] = makeDynString ("","InPressure");
xxdepes[4]=makeDynString ("","PressureSetpoint");
xxdepes[5]=makeDynString ("","OutPressure");
xxdepes[6] = makeDynString ("","off_channels");
for(i=1;i<=2;i++){
xxdepes[6+(i-1)*9+1]=makeDynString ("","Pump"+i+"Input-Flow");
xxdepes[6+(i-1)*9+2]=makeDynString ("","Pump"+i+"Current-Speed");
xxdepes[6+(i-1)*9+3]=makeDynString ("","Pump"+i+"Current-Load");
xxdepes[6+(i-1)*9+4]=makeDynString ("","Pump"+i+"Current-Speed2");
xxdepes[6+(i-1)*9+5]=makeDynString ("","Pump"+i+"Current-Load2");
xxdepes[6+(i-1)*9+6]=makeDynString ("","Pump"+i+"UpperTemperature");
xxdepes[6+(i-1)*9+7]=makeDynString ("","Pump"+i+"LowerTemperature");
xxdepes[6+(i-1)*9+8]=makeDynString ("","Pump"+i+"OutTemperature");
xxdepes[6+(i-1)*9+9]=makeDynString ("","Pump"+i+"EngineTemperature");
}

//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6]=makeDynInt (0,DPEL_DYN_INT);
for(i=1;i<=2;i++){
xxdepei[6+(i-1)*9+1]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6+(i-1)*9+2]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6+(i-1)*9+3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6+(i-1)*9+4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6+(i-1)*9+5]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6+(i-1)*9+6]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6+(i-1)*9+7]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6+(i-1)*9+8]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6+(i-1)*9+9]=makeDynInt (0,DPEL_FLOAT);
}

 
dpTypeCreate(xxdepes,xxdepei);  
  
  
}
//=============================
GasExhaust_create_data(){ 
 dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,j;
string dtype= "fwGasExhaust_CSC_GAS_data"; 
 
 xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","State");
xxdepes[3] = makeDynString ("","OutFlow");
xxdepes[4]=makeDynString ("","BufferPressure");
xxdepes[5]=makeDynString ("","CirculationPressure");
xxdepes[6]=makeDynString ("","CirculationFlow");
xxdepes[7] = makeDynString ("","off_channels");

//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6]=makeDynInt (0,DPEL_FLOAT);
xxdepei[7]=makeDynInt (0,DPEL_DYN_INT);

dpTypeCreate(xxdepes,xxdepei);  
  
  
  
}
//=============================
GasPurifier_create_data(){ 
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,j;
string AorB;
string dtype= "fwGasPurifier_CSC_GAS_data"; 
 xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","State");
xxdepes[3] = makeDynString ("","off_channels");
for(i=1;i<=2;i++){
  if(i==1)AorB="A"; 
  else AorB="B";
xxdepes[3+(i-1)*7+1] = makeDynString ("","Col"+AorB+"State");
xxdepes[3+(i-1)*7+2]=makeDynString ("","Col"+AorB+"RunVolume");
xxdepes[3+(i-1)*7+3]=makeDynString ("","Col"+AorB+"Internal-TopTemp");
xxdepes[3+(i-1)*7+4]=makeDynString ("","Col"+AorB+"InternalMiddle-Temp");
xxdepes[3+(i-1)*7+5]=makeDynString ("","Col"+AorB+"ExternalTemp");
xxdepes[3+(i-1)*7+6]=makeDynString ("","Col"+AorB+"InFlow");
xxdepes[3+(i-1)*7+7]=makeDynString ("","Col"+AorB+"OutPressure");
}

//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_DYN_INT);
for(i=1;i<=2;i++){
  if(i==1)AorB="A"; 
  else AorB="B";
xxdepei[3+(i-1)*7+1]=makeDynInt (0,DPEL_INT);
xxdepei[3+(i-1)*7+2]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3+(i-1)*7+3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3+(i-1)*7+4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3+(i-1)*7+5]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3+(i-1)*7+6]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3+(i-1)*7+7]=makeDynInt (0,DPEL_FLOAT);
}

 
dpTypeCreate(xxdepes,xxdepei);   
  
  
}

//=======================
mudcsDeleteGas_dps( bool isDelete_types=false){
  
  
  int i; 
  dyn_string ds;
 //====================== COOLING ===================================================  
  
  ds=dpNames("*","fwCooling_CSC_COOLING_d"); for(i=1;i<=dynlen(ds);i++)dpDelete(ds[i]);
  ds=dpNames("*","fwCooling_CSC_TURBINES_data"); for(i=1;i<=dynlen(ds);i++)dpDelete(ds[i]);
  ds=dpNames("*","fwCooling_CSC_TURBINES_summary"); for(i=1;i<=dynlen(ds);i++)dpDelete(ds[i]);  
  //dpTypeDelete("fwCooling_CSC_COOLING");  
  
  if(isDelete_types){
  dpTypeDelete("fwCooling_CSC_COOLING_d");
  dpTypeDelete("fwCooling_CSC_TURBINES_data");  
  dpTypeDelete("fwCooling_CSC_TURBINES_summary");    
  }
//====================== GAS =========================================================  
  
  ds=dpNames("*","fwGasSystem_CSC_GAS_d"); for(i=1;i<=dynlen(ds);i++)dpDelete(ds[i]);
 
  ds=dpNames("*","fwGasMixer_CSC_GAS_data"); for(i=1;i<=dynlen(ds);i++)dpDelete(ds[i]);
  ds=dpNames("*","fwGasDistribution_CSC_GAS_data");for(i=1;i<=dynlen(ds);i++)dpDelete(ds[i]);
  ds=dpNames("*","fwGasDistributionRack_CSC_GAS_data");for(i=1;i<=dynlen(ds);i++)dpDelete(ds[i]);
  ds=dpNames("*","fwGasDistributionChannel_CSC_GAS_data");for(i=1;i<=dynlen(ds);i++)dpDelete(ds[i]);
  ds=dpNames("*","fwGasPreDistribution_CSC_GAS_data");for(i=1;i<=dynlen(ds);i++)dpDelete(ds[i]);
  ds=dpNames("*","fwGasPump_CSC_GAS_data");for(i=1;i<=dynlen(ds);i++)dpDelete(ds[i]);
  ds=dpNames("*","fwGasExhaust_CSC_GAS_data");for(i=1;i<=dynlen(ds);i++)dpDelete(ds[i]);
  ds=dpNames("*","fwGasPurifier_CSC_GAS_data");  for(i=1;i<=dynlen(ds);i++)dpDelete(ds[i]); 
  
    if(isDelete_types){
  //dpTypeDelete("fwGasSystem_CSC_GAS");
  dpTypeDelete("fwGasSystem_CSC_GAS_d");      
  dpTypeDelete("fwGasMixer_CSC_GAS_data"); 
  dpTypeDelete("fwGasDistribution_CSC_GAS_data");
  dpTypeDelete("fwGasDistributionRack_CSC_GAS_data");
  dpTypeDelete("fwGasDistributionChannel_CSC_GAS_data");
  dpTypeDelete("fwGasPreDistribution_CSC_GAS_data");
  dpTypeDelete("fwGasPump_CSC_GAS_data");
  dpTypeDelete("fwGasExhaust_CSC_GAS_data");
  dpTypeDelete("fwGasPurifier_CSC_GAS_data");
  }
}
