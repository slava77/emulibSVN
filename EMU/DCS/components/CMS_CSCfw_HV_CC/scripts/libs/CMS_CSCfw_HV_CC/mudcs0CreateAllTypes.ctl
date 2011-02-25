mudcsCreateAllTypesMain()
{
  
// !!!!!! ATTENTION : the script must not contain mudcsDebug() in the middle of running: otherwise it is interrupts there   
// !!!!!!
     mudcsInit();
  FED_1_main(); 
  fwLvCr_CSC_TEMP();
     
  LV_1_main();
  TEMP_1_main();
  CHIP_1_main();
  GAS_SX5_main();
  HV_PR_main(); // the HV_PR should go firts before HV_1 to have HV_PR the first in the Db.oDimBrokerList;
  HV_1_main();
  MRTN_1_main();
  CRB_1_main();
  WTH_SX5_main();
  Snmp_main();
//  main();
//  main();
  
// !!!!!! ATTENTION : the script must not contain mudcsDebug() in the middle of running: otherwise it is interrupts there   
// !!!!!!               
       
}
fwLvCr_CSC_TEMP(){
  
string dtype="fwLvCr_CSC_TEMP";
      
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","reserve1");
xxdepes[3] = makeDynString ("","reserve2");
xxdepes[4] = makeDynString ("","reserve3");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_STRING);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_STRING);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);
  
  
}
//=============================
FED_1_main(){
int i;


FED_1_create_data("FED_1_data");
FED_1_create_d("FED_1_d");
FED_1_create("FED_1");
DebugTN("types: FED_1 created");
//---------
dpCreate("FED_1_DimBroker_o","FED_1_d");
DebugTN("dp: FED_1_DimBroker_o created");
//---------
dpCreate("FED_1Info","_FwDeviceDefinition");
dpSet("FED_1Info.type","Fed Device (1)");
dpSet("FED_1Info.dpType","FED_1");
DebugTN("FED_1Info created");
//----------
dpCreate("Fed","FwNode");
dpSet("Fed.type","VENDOR");
dpSet("Fed.dpTypes",makeDynString("FED_1"));
dpSet("Fed.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("Fed.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("Fed.model","Fed");
dpSet("Fed.fsmState",0);
dpSet("Fed.fsmCommand",0);
DebugTN("types: Fed of FwNode created");

mudcsInit();

//-----------
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
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++TEMP done");
}
//============================



//============================
FED_1_create(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","chamber_state");
xxdepes[3] = makeDynString ("","coord");
xxdepes[4] = makeDynString ("","status");
xxdepes[5] = makeDynString ("","off_channels");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_DYN_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
//============================
FED_1_create_d(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","data","FED_1_data");
xxdepes[3] = makeDynString ("","update_value");
xxdepes[4] = makeDynString ("","status");

xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
FED_1_create_data(string dtype)
{


char a;
a=50;

int b=a;

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","v15");
xxdepes[3] = makeDynString ("","v25a");
xxdepes[4] = makeDynString ("","v25b");
xxdepes[5] = makeDynString ("","v33");

xxdepes[6] = makeDynString ("","td1");
xxdepes[7] = makeDynString ("","td2");
xxdepes[8] = makeDynString ("","td3");
xxdepes[9] = makeDynString ("","td4");
//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);

xxdepei[6]=makeDynInt (0,DPEL_FLOAT);
xxdepei[7]=makeDynInt (0,DPEL_FLOAT);
xxdepei[8]=makeDynInt (0,DPEL_FLOAT);
xxdepei[9]=makeDynInt (0,DPEL_FLOAT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);


}
//============================

LV_1_main(){
int i;

//---------
dpCreate("LV_1_DimBroker_o","LV_1_d");
DebugTN("dp: LV_1_DimBroker_o created");
//---------
dpCreate("LV_1Info","_FwDeviceDefinition");
dpSet("LV_1Info.type","Low Voltage Device (1)");
dpSet("LV_1Info.dpType","LV_1");
DebugTN("LV_1Info created");
//----------
dpCreate("LowVoltage","FwNode");
dpSet("LowVoltage.type","VENDOR");
dpSet("LowVoltage.dpTypes",makeDynString("LV_1"));
dpSet("LowVoltage.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("LowVoltage.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("LowVoltage.model","LowVoltage");
dpSet("LowVoltage.fsmState",0);
dpSet("LowVoltage.fsmCommand",0);
DebugTN("types: LowVoltage of FwNode created");

mudcsInit();

//-----------
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
float v18=1.8,v33=3.3,v55=5.5,v56=5.5;
float c18=2.1,c33=2.5,c55=2.1,c56=2.1;
float v33_c=3.3, v50_c=5.0, v60_c=6.0;
float c33_c=0.5, c50_c=1.0, c60_c=3.0;
float rel_delta_volt=0.2, rel_delta_curr=1.0;
float rel_delta_volt_c=0.2, rel_delta_curr_c=1.0;

LV_1_db_lv_1_alert("Db_LV_1_alert");
dpCreate("Db_LV_1_alert_o","Db_LV_1_alert");


mudcsSetAlctAlert(v18,v33,v55,v56,rel_delta_volt,c18,c33,c55,c56,rel_delta_curr);
mudcsSetCfebAlert(v33_c,v50_c,v60_c,rel_delta_volt_c,c33_c,c50_c,c60_c,rel_delta_curr_c);

DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ LV done");
}

//============================
LV_1_db_lv_1_alert(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","LV_1_default","LV_1_data");
xxdepes[3] = makeDynString ("","LV_1_delta","LV_1_data");

xxdepes[4] = makeDynString ("","rel_delta_volt_alct");
xxdepes[5] = makeDynString ("","rel_delta_curr_alct");
xxdepes[6] = makeDynString ("","rel_delta_volt_cfeb");
xxdepes[7] = makeDynString ("","rel_delta_curr_cfeb");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_TYPEREF);

xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6]=makeDynInt (0,DPEL_FLOAT);
xxdepei[7]=makeDynInt (0,DPEL_FLOAT);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
TEMP_1_main(){
int i;


TEMP_1_create_data("TEMP_1_data");
TEMP_1_create_d("TEMP_1_d");
TEMP_1_create("TEMP_1");
DebugTN("types: TEMP_1 created");
//---------
dpCreate("TEMP_1_DimBroker_o","TEMP_1_d");
DebugTN("dp: TEMP_1_DimBroker_o created");
//---------
dpCreate("TEMP_1Info","_FwDeviceDefinition");
dpSet("TEMP_1Info.type","Temperature Device (1)");
dpSet("TEMP_1Info.dpType","TEMP_1");
DebugTN("TEMP_1Info created");
//----------
dpCreate("Temperature","FwNode");
dpSet("Temperature.type","VENDOR");
dpSet("Temperature.dpTypes",makeDynString("TEMP_1"));
dpSet("Temperature.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("Temperature.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("Temperature.model","Temperature");
dpSet("Temperature.fsmState",0);
dpSet("Temperature.fsmCommand",0);
DebugTN("types: Temperature of FwNode created");

mudcsInit();

//-----------
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
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++TEMP done");
}
//============================



//============================
TEMP_1_create(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","chamber_state");
xxdepes[3] = makeDynString ("","coord");
xxdepes[4] = makeDynString ("","status");
xxdepes[5] = makeDynString ("","off_channels");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_DYN_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
//============================
TEMP_1_create_d(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","data","TEMP_1_data");
xxdepes[3] = makeDynString ("","update_value");
xxdepes[4] = makeDynString ("","status");


xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
TEMP_1_create_data(string dtype)
{


char a;
a=50;

int b=a;

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","t_dmb","FLOAT_ONE");
xxdepes[3] = makeDynString ("","t_alct","FLOAT_ONE");
xxdepes[4] = makeDynString ("","t_cfeb","FLOAT_FIVE");

//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[4]=makeDynInt (0,DPEL_TYPEREF);




emu_dpTypeCreateOrChange(xxdepes,xxdepei);


}
CHIP_1_main(){
int i;


CHIP_1_create_data("CHIP_1_data");
CHIP_1_create_d("CHIP_1_d");
CHIP_1_create("CHIP_1");
DebugTN("types: CHIP_1 created");
//---------
dpCreate("CHIP_1_DimBroker_o","CHIP_1_d");
DebugTN("dp: CHIP_1_DimBroker_o created");
//---------
dpCreate("CHIP_1Info","_FwDeviceDefinition");
dpSet("CHIP_1Info.type","Chip Loading Device (1)");
dpSet("CHIP_1Info.dpType","CHIP_1");
DebugTN("CHIP_1Info created");
//----------
dpCreate("ChipLoading","FwNode");
dpSet("ChipLoading.type","VENDOR");
dpSet("ChipLoading.dpTypes",makeDynString("CHIP_1"));
dpSet("ChipLoading.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("ChipLoading.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("ChipLoading.model","ChipLoading");
dpSet("ChipLoading.fsmState",0);
dpSet("ChipLoading.fsmCommand",0);
DebugTN("types: ChipLoading of FwNode created");

mudcsInit();

//-----------
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
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++CHIP done");

}
//============================



//============================
CHIP_1_create(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","chamber_state");
xxdepes[3] = makeDynString ("","coord");
xxdepes[4] = makeDynString ("","status");
xxdepes[5] = makeDynString ("","off_channels");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_DYN_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
//============================
CHIP_1_create_d(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","data","CHIP_1_data");
xxdepes[3] = makeDynString ("","update_value");
xxdepes[4] = makeDynString ("","setNumber");
xxdepes[5] = makeDynString ("","dynatemTcpIp");


xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_STRING);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
CHIP_1_create_data(string dtype)
{


char a;
a=50;

int b=a;

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","operation_result");

//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);



emu_dpTypeCreateOrChange(xxdepes,xxdepei);


}

GAS_SX5_main(){
int i;


GAS_SX5_create_components();
GAS_SX5_create_data("GAS_SX5_data");
GAS_SX5_create_d("GAS_SX5_DimBroker");
GAS_SX5_create("GAS_SX5");
DebugTN("types: GAS_SX5 created");
//---------
dpCreate("GAS_SX5_DimBroker_o","GAS_SX5_DimBroker");
DebugTN("dp: GAS_SX5_DimBroker_o created");
//---------
dpCreate("GAS_SX5Info","_FwDeviceDefinition");
dpSet("GAS_SX5Info.type","Gas Monitoring Device (SX5)");
dpSet("GAS_SX5Info.dpType","GAS_SX5");
DebugTN("GAS_SX5Info created");
//----------
dpCreate("GasMonitoring","FwNode");
dpSet("GasMonitoring.type","VENDOR");
dpSet("GasMonitoring.dpTypes",makeDynString("GAS_SX5"));
dpSet("GasMonitoring.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("GasMonitoring.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("GasMonitoring.model","GasMonitoring");
dpSet("GasMonitoring.fsmState",0);
dpSet("GasMonitoring.fsmCommand",0);
DebugTN("types: GasMonitoring of FwNode created");

mudcsInit();

//-----------
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
float m1_ar_mean=1, m1_co2_mean=1;
float m2_ar_mean=1, m2_co2_mean=1, m2_cf4_mean=1;

float m1_ar_delta=100, m1_co2_delta=100;
float m2_ar_delta=100, m2_co2_delta, m2_cf4_delta=100;

GAS_SX5_db_gas_sx5_alert("Db_GAS_SX5_alert");
dpCreate("Db_GAS_SX5_alert_o","Db_GAS_SX5_alert");

mudcsSetAlertGasMeans(m1_ar_mean, m1_co2_mean, m2_ar_mean, m2_co2_mean, m2_cf4_mean);
mudcsSetAlertGasDeltas(m1_ar_delta, m1_co2_delta, m2_ar_delta, m2_co2_delta, m2_cf4_delta);

//mudcsDebug("done");
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++GAS_SX5 done");
}
//============================
GAS_SX5_create_components()
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
string dtype;

//========
dtype="MIXER1";

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","ar");
xxdepes[3] = makeDynString ("","co2");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);
//========
//========
dyn_dyn_string xxdepes2;
dyn_dyn_int xxdepei2;

dtype="MIXER2";

xxdepes2[1] = makeDynString (dtype,"");
xxdepes2[2] = makeDynString ("","ar");
xxdepes2[3] = makeDynString ("","co2");
xxdepes2[4] = makeDynString ("","cf4");

xxdepei2[1]=makeDynInt (DPEL_STRUCT);
xxdepei2[2]=makeDynInt (0,DPEL_FLOAT);
xxdepei2[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei2[4]=makeDynInt (0,DPEL_FLOAT);


emu_dpTypeCreateOrChange(xxdepes2,xxdepei2);
//========
//========

dyn_dyn_string xxdepes3;
dyn_dyn_int xxdepei3;

dtype="Alct";

xxdepes3[1] = makeDynString (dtype,"");
xxdepes3[2] = makeDynString ("","v18");
xxdepes3[3] = makeDynString ("","v33");
xxdepes3[4] = makeDynString ("","v55");
xxdepes3[5] = makeDynString ("","v56");
xxdepes3[6] = makeDynString ("","c18");
xxdepes3[7] = makeDynString ("","c33");
xxdepes3[8] = makeDynString ("","c55");
xxdepes3[9] = makeDynString ("","c56");
//xxdepes3[] = makeDynString ("","");

xxdepei3[1]=makeDynInt (DPEL_STRUCT);
xxdepei3[2]=makeDynInt (0,DPEL_FLOAT);
xxdepei3[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei3[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei3[5]=makeDynInt (0,DPEL_FLOAT);
xxdepei3[6]=makeDynInt (0,DPEL_FLOAT);
xxdepei3[7]=makeDynInt (0,DPEL_FLOAT);
xxdepei3[8]=makeDynInt (0,DPEL_FLOAT);
xxdepei3[9]=makeDynInt (0,DPEL_FLOAT);
//xxdepei3[]=makeDynInt (0,DPEL_FLOAT);

emu_dpTypeCreateOrChange(xxdepes3,xxdepei3);

//========
//========

dyn_dyn_string xxdepes4;
dyn_dyn_int xxdepei4;

dtype="Cfeb";

xxdepes4[1] = makeDynString (dtype,"");
xxdepes4[2] = makeDynString ("","v33","FLOAT_FIVE");
xxdepes4[3] = makeDynString ("","v50","FLOAT_FIVE");
xxdepes4[4] = makeDynString ("","v60","FLOAT_FIVE");
xxdepes4[5] = makeDynString ("","c33","FLOAT_FIVE");
xxdepes4[6] = makeDynString ("","c50","FLOAT_FIVE");
xxdepes4[7] = makeDynString ("","c60","FLOAT_FIVE");

xxdepei4[1]=makeDynInt (DPEL_STRUCT);
xxdepei4[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei4[3]=makeDynInt (0,DPEL_TYPEREF);
xxdepei4[4]=makeDynInt (0,DPEL_TYPEREF);
xxdepei4[5]=makeDynInt (0,DPEL_TYPEREF);
xxdepei4[6]=makeDynInt (0,DPEL_TYPEREF);
xxdepei4[7]=makeDynInt (0,DPEL_TYPEREF);

emu_dpTypeCreateOrChange(xxdepes4,xxdepei4);


}

//============================
GAS_SX5_create(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","data","GAS_SX5_data");
xxdepes[3] = makeDynString ("","status");


xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);


}

//=============================
//============================
GAS_SX5_create_d(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","data","GAS_SX5_data");
xxdepes[3] = makeDynString ("","update_value");
xxdepes[4] = makeDynString ("","setNumber");
xxdepes[5] = makeDynString ("","dynatemTcpIp");


xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_STRING);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
GAS_SX5_create_data(string dtype)
{


char a;
a=50;

int b=a;

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","Mixer1_o","MIXER1");
xxdepes[3] = makeDynString ("","Mixer2_o","MIXER2");

//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_TYPEREF);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);


}

//============================
GAS_SX5_db_gas_sx5_alert(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","mean","GAS_SX5_data");
xxdepes[3] = makeDynString ("","delta","GAS_SX5_data");


xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_TYPEREF);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
HV_PR_main(){


HV_PR_create_data("HV_PR_data");
HV_PR_create("HV_PR");
HV_PR_create_broker("HV_PR_d");


dpCreate("HV_PR_DimBroker_o","HV_PR_d");



// all other datapoints are created via the mudcsCreateHV_PR() (emuDcs3.ctl)


//---------
dpCreate("HV_PRInfo","_FwDeviceDefinition");
dpSet("HV_PRInfo.type","High Voltage Device (PR)");
dpSet("HV_PRInfo.dpType","HV_PR");
DebugTN("HV_PRInfo created");
//----------
//----------
dpCreate("HighVoltage","FwNode");
dpSet("HighVoltage.type","VENDOR");
dpSet("HighVoltage.dpTypes",makeDynString("HV_1","HV_PR"));
dpSet("HighVoltage.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("HighVoltage.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("HighVoltage.model","HighVoltage");
dpSet("HighVoltage.fsmState",0);
dpSet("HighVoltage.fsmCommand",0);
DebugTN("types: HighVoltage of FwNode created");


DebugTN("types: HighVoltage of FwNode created");



//----------

mudcsInit();


//-----------
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
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++HV_PR done");

}

//================================================================
HV_PR_create_broker(string dtype)
{


dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;



xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","data","HV_PR_data");

xxdepes[3] = makeDynString ("","update_value");
xxdepes[4] = makeDynString ("","status");


//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);

xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);


}

//====================================================================

HV_PR_create_data(string dtype)
{


dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;



xxdepes[1] = makeDynString (dtype,"");



xxdepes[2] = makeDynString ("","hostid");

xxdepes[3] = makeDynString ("","port");
xxdepes[4] = makeDynString ("","addr");

xxdepes[5] = makeDynString ("","polarity");

xxdepes[6] = makeDynString ("","outset");
xxdepes[7] = makeDynString ("","output");
xxdepes[8] = makeDynString ("","control");

xxdepes[9] = makeDynString ("","imax");

xxdepes[10] = makeDynString ("","vset");
xxdepes[11] = makeDynString ("","imon");
xxdepes[12] = makeDynString ("","vmon");
xxdepes[13] = makeDynString ("","vcur");
xxdepes[14] = makeDynString ("","rampup");
xxdepes[15] = makeDynString ("","rampdown");

////xxdepes[16] = makeDynString ("","update_value");
////xxdepes[17] = makeDynString ("","status");


//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);


xxdepei[2]=makeDynInt (0,DPEL_INT);

xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);

xxdepei[5]=makeDynInt (0,DPEL_INT);
xxdepei[6]=makeDynInt (0,DPEL_INT);
xxdepei[7]=makeDynInt (0,DPEL_INT);
xxdepei[8]=makeDynInt (0,DPEL_INT);

xxdepei[9]=makeDynInt (0,DPEL_INT);  // FLOAT

xxdepei[10]=makeDynInt (0,DPEL_INT);

xxdepei[11]=makeDynInt (0,DPEL_INT); // FLOAT
xxdepei[12]=makeDynInt (0,DPEL_INT); // FLOAT

xxdepei[13]=makeDynInt (0,DPEL_INT);
xxdepei[14]=makeDynInt (0,DPEL_INT);
xxdepei[15]=makeDynInt (0,DPEL_INT);
////xxdepei[16]=makeDynInt (0,DPEL_INT);
////xxdepei[17]=makeDynInt (0,DPEL_INT);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);





//dpSet("HVcmdBroker1.data",a);
//dpSet("HVcmdBroker1.size",1);
//dpSet(parent_domain+"|"+dpN+".ui.panels:_original.._value",panels);
}

//====================================================================

HV_PR_create(string dtype)
{


dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;

xxdepes[1] = makeDynString (dtype,"");
////xxdepes[2] = makeDynString ("","data","HV_PR_data");

xxdepes[2] = makeDynString ("","chamber_state");
xxdepes[3] = makeDynString ("","coord");
xxdepes[4] = makeDynString ("","last_vset");
xxdepes[5] = makeDynString ("","status");

//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
/////xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);

xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_INT);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);


}
HV_1_main()
{

HV_1_main_create_HV_channel_pack();
HV_1_main_create_the_rest();  
//mudcsDebug("done");
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++HV_1 done");
}

//=========================

HV_1_main_create_HV_channel_pack()
{

HV_1_create_ch("HVchannel");
HV_1_create_ch_pack("HVchannel_pack");
HV_1_create_data("HVchannel_pack_data");

}


//============================
HV_1_create_ch(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","imon");
xxdepes[3] = makeDynString ("","vmon");
xxdepes[4] = makeDynString ("","state");
xxdepes[5] = makeDynString ("","status");

//-----------------------------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_INT);



emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
//============================
HV_1_create_ch_pack(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","ramp_up");
xxdepes[3] = makeDynString ("","ramp_down");
xxdepes[4] = makeDynString ("","imax");
xxdepes[5] = makeDynString ("","trip_cnt");
xxdepes[6] = makeDynString ("","trip_delay");
xxdepes[7] = makeDynString ("","trip_reset_delay");
xxdepes[8] = makeDynString ("","trip_reset_delay_cnt");
xxdepes[9] = makeDynString ("","trip_max_resets");
xxdepes[10] = makeDynString ("","trip_resets_cnt");
xxdepes[11] = makeDynString ("","itrip");
xxdepes[12] = makeDynString ("","vtrip");
xxdepes[13] = makeDynString ("","vmax");
xxdepes[14] = makeDynString ("","vset");
xxdepes[15] = makeDynString ("","vcur");
xxdepes[16] = makeDynString ("","relay");
xxdepes[17] = makeDynString ("","fuse");

//-----------------------------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_INT);
xxdepei[6]=makeDynInt (0,DPEL_INT);
xxdepei[7]=makeDynInt (0,DPEL_INT);
xxdepei[8]=makeDynInt (0,DPEL_INT);
xxdepei[9]=makeDynInt (0,DPEL_INT);
xxdepei[10]=makeDynInt (0,DPEL_INT);
xxdepei[11]=makeDynInt (0,DPEL_INT);
xxdepei[12]=makeDynInt (0,DPEL_INT);
xxdepei[13]=makeDynInt (0,DPEL_INT);
xxdepei[14]=makeDynInt (0,DPEL_INT);
xxdepei[15]=makeDynInt (0,DPEL_INT);
xxdepei[16]=makeDynInt (0,DPEL_INT);
xxdepei[17]=makeDynInt (0,DPEL_INT);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
HV_1_create_data(string dtype)
{




dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");


for(i=2;i<=19;i++){
i1=i-1;
xxdepes[i] = makeDynString ("","v"+i1,"HVchannel_pack");
}


//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);


for(i=2;i<=19;i++){
xxdepei[i]=makeDynInt (0,DPEL_TYPEREF);
}



emu_dpTypeCreateOrChange(xxdepes,xxdepei);



}
//==================================================================
//==================================================================
//==================================================================
//==================================================================
//==================================================================
//==================================================================
//==================================================================



HV_1_main_create_the_rest(){


////create("HV_1_DimBroker");
HV_1_create_data_rest("HV_1_data");
HV_1_create_d("HV_1_d");
//HV_1_create("HV_1");
DebugTN("HV_1 created");


//---------
dpCreate("HV_1_DimBroker_o","HV_1_d");
DebugTN("dp: HV_1_DimBroker_o created");
//---------
dpCreate("HV_1Info","_FwDeviceDefinition");
dpSet("HV_1Info.type","High Voltage Device (1)");
dpSet("HV_1Info.dpType","HV_1");
DebugTN("HV_1Info created");
//----------
dpCreate("HighVoltage","FwNode");
dpSet("HighVoltage.type","VENDOR");
dpSet("HighVoltage.dpTypes",makeDynString("HV_1","HV_PR"));
dpSet("HighVoltage.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("HighVoltage.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("HighVoltage.model","HighVoltage");
dpSet("HighVoltage.fsmState",0);
dpSet("HighVoltage.fsmCommand",0);
DebugTN("types: HighVoltage of FwNode created");

mudcsInit();

//-----------
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


}
//============================
HV_1_create(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","module_state");
xxdepes[3] = makeDynString ("","coord");
xxdepes[4] = makeDynString ("","last_vset");
xxdepes[5] = makeDynString ("","status");
xxdepes[6] = makeDynString ("","off_channels");
xxdepes[7] = makeDynString ("","reserve1");
xxdepes[8] = makeDynString ("","reserve2");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_INT);
xxdepei[6]=makeDynInt (0,DPEL_DYN_INT);
xxdepei[7]=makeDynInt (0,DPEL_INT);
xxdepei[8]=makeDynInt (0,DPEL_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
//============================
HV_1_create_d(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","data","HV_1_data");
xxdepes[3] = makeDynString ("","update_value");

xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
HV_1_create_data_rest(string dtype)
{


char a;
a=50;

int b=a;



dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");



xxdepes[2] = makeDynString ("","request_card");
xxdepes[3] = makeDynString ("","request_module");
xxdepes[4] = makeDynString ("","request_chan");
xxdepes[5] = makeDynString ("","request_cmd");

xxdepes[6] = makeDynString ("","host_hostid");
xxdepes[7] = makeDynString ("","host_bus");
xxdepes[8] = makeDynString ("","host_slot");

xxdepes[9] = makeDynString ("","module_number");

xxdepes[10] = makeDynString ("","module_type");
xxdepes[11] = makeDynString ("","module_ID");
xxdepes[12] = makeDynString ("","module_busaddr");
xxdepes[13] = makeDynString ("","module_interlock");
xxdepes[14] = makeDynString ("","module_pos5v");
xxdepes[15] = makeDynString ("","module_neg5v");
xxdepes[16] = makeDynString ("","module_num_chans");
xxdepes[17] = makeDynString ("","module_state");
xxdepes[18] = makeDynString ("","module_status");

for(i=19;i<=54;i++){
i1=i-18;
xxdepes[i] = makeDynString ("","v"+i1,"HVchannel");
}

xxdepes[55] = makeDynString ("","master_id"); // factory: can be seen at autodetect
xxdepes[56] = makeDynString ("","master_hostid"); // 100 e.g.
xxdepes[57] = makeDynString ("","master_hvcard"); // parameter n
xxdepes[58] = makeDynString ("","master_busaddr"); // parameter a 
xxdepes[59] = makeDynString ("","master_chan"); // channel in master module
xxdepes[60] = makeDynString ("","master_trip_delay"); // new delay
xxdepes[61] = makeDynString ("","chamber_complex_status"); // new delay

xxdepes[62] = makeDynString ("","pack_data","HVchannel_pack_data");


//xxdepes[61] = makeDynString ("","update_value");
//xxdepes[62] = makeDynString ("","status");


//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);


xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_INT);

xxdepei[6]=makeDynInt (0,DPEL_INT);
xxdepei[7]=makeDynInt (0,DPEL_INT);
xxdepei[8]=makeDynInt (0,DPEL_INT);

xxdepei[9]=makeDynInt (0,DPEL_INT);

xxdepei[10]=makeDynInt (0,DPEL_INT);
xxdepei[11]=makeDynInt (0,DPEL_INT);
xxdepei[12]=makeDynInt (0,DPEL_INT);
xxdepei[13]=makeDynInt (0,DPEL_INT);
xxdepei[14]=makeDynInt (0,DPEL_INT);
xxdepei[15]=makeDynInt (0,DPEL_INT);
xxdepei[16]=makeDynInt (0,DPEL_INT);
xxdepei[17]=makeDynInt (0,DPEL_INT);
xxdepei[18]=makeDynInt (0,DPEL_INT);

for(i=19;i<=54;i++){
xxdepei[i]=makeDynInt (0,DPEL_TYPEREF);
}

xxdepei[55]=makeDynInt (0,DPEL_INT);
xxdepei[56]=makeDynInt (0,DPEL_INT);
xxdepei[57]=makeDynInt (0,DPEL_INT);
xxdepei[58]=makeDynInt (0,DPEL_INT);
xxdepei[59]=makeDynInt (0,DPEL_INT);
xxdepei[60]=makeDynInt (0,DPEL_INT);
xxdepei[61]=makeDynInt (0,DPEL_INT);

xxdepei[62]=makeDynInt (0,DPEL_TYPEREF);



emu_dpTypeCreateOrChange(xxdepes,xxdepei);





//dpSet("HVcmdBroker1.data",a);
//dpSet("HVcmdBroker1.size",1);
//dpSet(parent_domain+"|"+dpN+".ui.panels:_original.._value",panels);
}
MRTN_1_main(){
int i;


MRTN_1_create_components();
MRTN_1_create_data("MRTN_1_data");
MRTN_1_create_d("MRTN_1_d");
MRTN_1_create("MRTN_1");
DebugTN("types: MRTN_* created");
//---------
dpCreate("MRTN_1_DimBroker_o","MRTN_1_d");
DebugTN("dp: MRTN_1_DimBroker_o created");
//---------
dpCreate("MRTN_1Info","_FwDeviceDefinition");
dpSet("MRTN_1Info.type","MRTN Device (1)");
dpSet("MRTN_1Info.dpType","MRTN_1"); //?
DebugTN("MRTN_1Info created");
//----------
//----------
dpCreate("MRTN","FwNode");
dpSet("MRTN.type","VENDOR");
dpSet("MRTN.dpTypes",makeDynString("MRTN_1"));
dpSet("MRTN.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("MRTN.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("MRTN.model","MRTN");
dpSet("MRTN.fsmState",0);
dpSet("MRTN.fsmCommand",0);
DebugTN("types: MRTN of FwNode created");



DebugTN("types: MRTN of FwNode created");
mudcsInit();

//-----------
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
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++MRTN_1 done");

}
//============================
//============================
MRTN_1_create_components()
{

string dtype;

//========
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

dtype="MRTN_Channel";

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","item1");
xxdepes[3] = makeDynString ("","item2");
xxdepes[4] = makeDynString ("","item3");
xxdepes[5] = makeDynString ("","item4");
xxdepes[6] = makeDynString ("","item5");
xxdepes[7] = makeDynString ("","item6");
xxdepes[8] = makeDynString ("","item7");
xxdepes[9] = makeDynString ("","item8");
xxdepes[10] = makeDynString ("","item9");
xxdepes[11] = makeDynString ("","item10");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6]=makeDynInt (0,DPEL_FLOAT);
xxdepei[7]=makeDynInt (0,DPEL_FLOAT);
xxdepei[8]=makeDynInt (0,DPEL_FLOAT);
xxdepei[9]=makeDynInt (0,DPEL_FLOAT);
xxdepei[10]=makeDynInt (0,DPEL_FLOAT);
xxdepei[11]=makeDynInt (0,DPEL_FLOAT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);
//========



dyn_dyn_string xxdepes2;
dyn_dyn_int xxdepei2;

dtype="MRTN_Crate";

xxdepes2[1] = makeDynString (dtype,"");
xxdepes2[2] = makeDynString ("","item1");
xxdepes2[3] = makeDynString ("","item2");
xxdepes2[4] = makeDynString ("","item3");
xxdepes2[5] = makeDynString ("","item4");
xxdepes2[6] = makeDynString ("","item5");
xxdepes2[7] = makeDynString ("","item6");
xxdepes2[8] = makeDynString ("","item7");
xxdepes2[9] = makeDynString ("","item8");
xxdepes2[10] = makeDynString ("","item9");


xxdepei2[1]=makeDynInt (DPEL_STRUCT);
xxdepei2[2]=makeDynInt (0,DPEL_INT);
xxdepei2[3]=makeDynInt (0,DPEL_INT);
xxdepei2[4]=makeDynInt (0,DPEL_INT);
xxdepei2[5]=makeDynInt (0,DPEL_INT);
xxdepei2[6]=makeDynInt (0,DPEL_INT);
xxdepei2[7]=makeDynInt (0,DPEL_INT);
xxdepei2[8]=makeDynInt (0,DPEL_INT);
xxdepei2[9]=makeDynInt (0,DPEL_INT);
xxdepei2[10]=makeDynInt (0,DPEL_INT);


emu_dpTypeCreateOrChange(xxdepes2,xxdepei2);

}

//============================
MRTN_1_create(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","module_state");
xxdepes[3] = makeDynString ("","coord");
xxdepes[4] = makeDynString ("","status");
xxdepes[5] = makeDynString ("","off_channels");
xxdepes[6] = makeDynString ("","off_channels_volt");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_DYN_INT);
xxdepei[6]=makeDynInt (0,DPEL_DYN_FLOAT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
//============================
MRTN_1_create_d(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","data","MRTN_1_data");
xxdepes[3] = makeDynString ("","update_value");
xxdepes[4] = makeDynString ("","setNumber");
xxdepes[5] = makeDynString ("","dynatemTcpIp");


xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_STRING);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
MRTN_1_create_data(string dtype)
{


char a;
a=50;

int b=a;

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");

for(i=2;i<=7;i++){
i1=i-2+1;
xxdepes[i] = makeDynString ("","channel"+i1,"MRTN_Channel");
}
xxdepes[8] = makeDynString ("","crate","MRTN_Crate");

for(i=9;i<=14;i++){
i1=i-9+1;
xxdepes[i] = makeDynString ("","status"+i1);
}
xxdepes[15] = makeDynString ("","general_status");
//=====================================

xxdepei[1]=makeDynInt (DPEL_STRUCT);

for(i=2;i<=7;i++){
xxdepei[i]=makeDynInt (0,DPEL_TYPEREF);
}
xxdepei[8]=makeDynInt (0,DPEL_TYPEREF);

for(i=9;i<=14;i++){
xxdepei[i]=makeDynInt (0,DPEL_INT);
}

xxdepei[15]=makeDynInt (0,DPEL_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}
CRB_1_main(){
int i;


CRB_1_create_components();
CRB_1_create_data("CRB_1_data");
CRB_1_create_d("CRB_1_d");
CRB_1_create("CRB_1");
DebugTN("types: CRB_* created");
//---------
dpCreate("CRB_1_DimBroker_o","CRB_1_d");
DebugTN("dp: CRB_1_DimBroker_o created");
//---------
dpCreate("CRB_1Info","_FwDeviceDefinition");
dpSet("CRB_1Info.type","CRB Device (1)");
dpSet("CRB_1Info.dpType","CRB_1"); //?
DebugTN("CRB_1Info created");
//----------
//----------
dpCreate("CRB","FwNode");
dpSet("CRB.type","VENDOR");
dpSet("CRB.dpTypes",makeDynString("CRB_1"));
dpSet("CRB.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("CRB.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("CRB.model","CRB");
dpSet("CRB.fsmState",0);
dpSet("CRB.fsmCommand",0);
DebugTN("types: CRB of FwNode created");



DebugTN("types: CRB of FwNode created");
mudcsInit();

//-----------
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
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++CRB_1 done");

}
//============================
//============================
CRB_1_create_components()
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
string dtype;

//========
dtype="CRB_pair";

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","v33");
xxdepes[3] = makeDynString ("","v34");
xxdepes[4] = makeDynString ("","v50");
xxdepes[5] = makeDynString ("","status");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);
//========
}

//============================
CRB_1_create(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","module_state");
xxdepes[3] = makeDynString ("","coord");
xxdepes[4] = makeDynString ("","status");
xxdepes[5] = makeDynString ("","off_channels");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_DYN_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
//============================
CRB_1_create_d(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","data","CRB_1_data");
xxdepes[3] = makeDynString ("","update_value");
xxdepes[4] = makeDynString ("","setNumber");
xxdepes[5] = makeDynString ("","dynatemTcpIp");


xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_STRING);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
CRB_1_create_data(string dtype)
{


char a;
a=50;

int b=a;

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","vin_45");
xxdepes[3] = makeDynString ("","vin_65");
xxdepes[4] = makeDynString ("","vin_5STBY");
xxdepes[5] = makeDynString ("","vcc_33");
xxdepes[6] = makeDynString ("","vcc_50");
xxdepes[7] = makeDynString ("","vccb_33");
xxdepes[8] = makeDynString ("","vccb_50");


for(i=9;i<=17;i++){
i1=i-9+1;
xxdepes[i] = makeDynString ("","pair"+i1,"CRB_pair");
}

xxdepes[18] = makeDynString ("","v15_1");
xxdepes[19] = makeDynString ("","v15_2");
xxdepes[20] = makeDynString ("","status");

//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6]=makeDynInt (0,DPEL_FLOAT);
xxdepei[7]=makeDynInt (0,DPEL_FLOAT);
xxdepei[8]=makeDynInt (0,DPEL_FLOAT);

for(i=9;i<=17;i++){
xxdepei[i]=makeDynInt (0,DPEL_TYPEREF);
}

xxdepei[18]=makeDynInt (0,DPEL_FLOAT);
xxdepei[19]=makeDynInt (0,DPEL_FLOAT);
xxdepei[20]=makeDynInt (0,DPEL_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);


}
WNR12_1_main(){
int i;


WNR12_1_create_components();
WNR12_1_create_data("WNR12_1_data");
WNR12_1_create_d("WNR12_1_d");
WNR12_1_create("WNR12_1");
DebugTN("types: WNR12_* created");
//---------
dpCreate("WNR12_1_DimBroker_o","WNR12_1_d");
DebugTN("dp: WNR12_1_DimBroker_o created");
//---------
dpCreate("WNR12_1Info","_FwDeviceDefinition");
dpSet("WNR12_1Info.type","WNR12 Device (1)");
dpSet("WNR12_1Info.dpType","WNR12_1"); //?
DebugTN("WNR12_1Info created");
//----------
//----------
dpCreate("WNR12","FwNode");
dpSet("WNR12.type","VENDOR");
dpSet("WNR12.dpTypes",makeDynString("WNR12_1"));
dpSet("WNR12.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("WNR12.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("WNR12.model","WNR12");
dpSet("WNR12.fsmState",0);
dpSet("WNR12.fsmCommand",0);
DebugTN("types: WNR12 of FwNode created");

DebugTN("types: WNR12 of FwNode created");
mudcsInit();

//-----------
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
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++WNR12_1 done");
}
//============================
//============================
WNR12_1_create_components()
{

string dtype;

//========
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

dtype="WNR12_Channel";

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","item1");
xxdepes[3] = makeDynString ("","item2");
xxdepes[4] = makeDynString ("","item3");
xxdepes[5] = makeDynString ("","item4");
xxdepes[6] = makeDynString ("","item5");
xxdepes[7] = makeDynString ("","item6");
xxdepes[8] = makeDynString ("","item7");
xxdepes[9] = makeDynString ("","item8");
xxdepes[10] = makeDynString ("","item9");
xxdepes[11] = makeDynString ("","item10");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6]=makeDynInt (0,DPEL_FLOAT);
xxdepei[7]=makeDynInt (0,DPEL_FLOAT);
xxdepei[8]=makeDynInt (0,DPEL_FLOAT);
xxdepei[9]=makeDynInt (0,DPEL_FLOAT);
xxdepei[10]=makeDynInt (0,DPEL_FLOAT);
xxdepei[11]=makeDynInt (0,DPEL_FLOAT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);
//========



dyn_dyn_string xxdepes2;
dyn_dyn_int xxdepei2;

dtype="WNR12_Crate";

xxdepes2[1] = makeDynString (dtype,"");
xxdepes2[2] = makeDynString ("","item1");
xxdepes2[3] = makeDynString ("","item2");
xxdepes2[4] = makeDynString ("","item3");
xxdepes2[5] = makeDynString ("","item4");
xxdepes2[6] = makeDynString ("","item5");
xxdepes2[7] = makeDynString ("","item6");
xxdepes2[8] = makeDynString ("","item7");
xxdepes2[9] = makeDynString ("","item8");
xxdepes2[10] = makeDynString ("","item9");


xxdepei2[1]=makeDynInt (DPEL_STRUCT);
xxdepei2[2]=makeDynInt (0,DPEL_INT);
xxdepei2[3]=makeDynInt (0,DPEL_INT);
xxdepei2[4]=makeDynInt (0,DPEL_INT);
xxdepei2[5]=makeDynInt (0,DPEL_INT);
xxdepei2[6]=makeDynInt (0,DPEL_INT);
xxdepei2[7]=makeDynInt (0,DPEL_INT);
xxdepei2[8]=makeDynInt (0,DPEL_INT);
xxdepei2[9]=makeDynInt (0,DPEL_INT);
xxdepei2[10]=makeDynInt (0,DPEL_INT);


emu_dpTypeCreateOrChange(xxdepes2,xxdepei2);

}

//============================
WNR12_1_create(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","module_state");
xxdepes[3] = makeDynString ("","coord");
xxdepes[4] = makeDynString ("","status");
xxdepes[5] = makeDynString ("","off_channels");
xxdepes[6] = makeDynString ("","off_channels_volt");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_DYN_INT);
xxdepei[6]=makeDynInt (0,DPEL_DYN_FLOAT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
//============================
WNR12_1_create_d(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","data","WNR12_1_data");
xxdepes[3] = makeDynString ("","update_value");
xxdepes[4] = makeDynString ("","setNumber");
xxdepes[5] = makeDynString ("","dynatemTcpIp");


xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_STRING);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
WNR12_1_create_data(string dtype)
{


char a;
a=50;

int b=a;

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");

for(i=2;i<=13;i++){
i1=i-2+1;
xxdepes[i] = makeDynString ("","channel"+i1,"WNR12_Channel");
}
xxdepes[14] = makeDynString ("","crate","WNR12_Crate");

for(i=15;i<=26;i++){
i1=i-15+1;
xxdepes[i] = makeDynString ("","status"+i1);
}
xxdepes[27] = makeDynString ("","general_status");
//=====================================

xxdepei[1]=makeDynInt (DPEL_STRUCT);

for(i=2;i<=13;i++){
xxdepei[i]=makeDynInt (0,DPEL_TYPEREF);
}
xxdepei[14]=makeDynInt (0,DPEL_TYPEREF);

for(i=15;i<=26;i++){
xxdepei[i]=makeDynInt (0,DPEL_INT);
}

xxdepei[27]=makeDynInt (0,DPEL_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}
WTH_SX5_main(){
int i;


WTH_SX5_create_components();
WTH_SX5_create_data("WTH_SX5_data");
WTH_SX5_create_d("WTH_SX5_d");
WTH_SX5_create("WTH_SX5");
DebugTN("types: WTH_SX5 created");
//---------
dpCreate("WTH_SX5_DimBroker_o","WTH_SX5_d");
DebugTN("dp: WTH_SX5_DimBroker_o created");
//---------
dpCreate("WTH_SX5Info","_FwDeviceDefinition");
dpSet("WTH_SX5Info.type","Weather Station Device (SX5)");
dpSet("WTH_SX5Info.dpType","WTH_SX5");
DebugTN("WTH_SX5Info created");
//----------
dpCreate("WeatherStation","FwNode");
dpSet("WeatherStation.type","VENDOR");
dpSet("WeatherStation.dpTypes",makeDynString("WTH_SX5"));
dpSet("WeatherStation.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("WeatherStation.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("WeatherStation.model","WeatherStation");
dpSet("WeatherStation.fsmState",0);
dpSet("WeatherStation.fsmCommand",0);
DebugTN("types: WeatherStation of FwNode created");








mudcsInit();


//-----------
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
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++WTH_SX5 done");
}
//============================
WTH_SX5_create_components()
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
string dtype;

//========
dtype="TEMPERATURE_SX5";

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","sensor1");
xxdepes[3] = makeDynString ("","sensor2");
xxdepes[4] = makeDynString ("","sensor3");
xxdepes[5] = makeDynString ("","sensor4");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_FLOAT);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);
//========


dyn_dyn_string xxdepes2;
dyn_dyn_int xxdepei2;

dtype="HUMIDITY_SX5";

xxdepes2[1] = makeDynString (dtype,"");
xxdepes2[2] = makeDynString ("","sensor1");
xxdepes2[3] = makeDynString ("","sensor2");
xxdepes2[4] = makeDynString ("","sensor3");

xxdepei2[1]=makeDynInt (DPEL_STRUCT);
xxdepei2[2]=makeDynInt (0,DPEL_FLOAT);
xxdepei2[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei2[4]=makeDynInt (0,DPEL_FLOAT);

emu_dpTypeCreateOrChange(xxdepes2,xxdepei2);

//========

dyn_dyn_string xxdepes3;
dyn_dyn_int xxdepei3;

dtype="PRESSURE_SX5";

xxdepes3[1] = makeDynString (dtype,"");
xxdepes3[2] = makeDynString ("","sensor1");

xxdepei3[1]=makeDynInt (DPEL_STRUCT);
xxdepei3[2]=makeDynInt (0,DPEL_FLOAT);

emu_dpTypeCreateOrChange(xxdepes3,xxdepei3);


}

//============================
WTH_SX5_create(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","module_state");
xxdepes[3] = makeDynString ("","coord");
xxdepes[4] = makeDynString ("","status");
xxdepes[5] = makeDynString ("","off_channels");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_DYN_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
//============================
WTH_SX5_create_d(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","data","WTH_SX5_data");
xxdepes[3] = makeDynString ("","update_value");
xxdepes[4] = makeDynString ("","setNumber");
xxdepes[5] = makeDynString ("","dynatemTcpIp");

xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_STRING);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
WTH_SX5_create_data(string dtype)
{


char a;
a=50;

int b=a;

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","TEMPERATURE","TEMPERATURE_SX5");
xxdepes[3] = makeDynString ("","HUMIDITY","HUMIDITY_SX5");
xxdepes[4] = makeDynString ("","PRESSURE","PRESSURE_SX5");

//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[4]=makeDynInt (0,DPEL_TYPEREF);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);


}
PT100_main(){
int i;


PT100_create_data("PT100_data");
PT100_create_d("PT100_d");
PT100_create("PT100");
DebugTN("types: PT100 created");
//---------
dpCreate("PT100_DimBroker_o","PT100_d");
DebugTN("dp: PT100_DimBroker_o created");
//---------
dpCreate("PT100Info","_FwDeviceDefinition");
dpSet("PT100Info.type","PT100 Device");
dpSet("PT100Info.dpType","PT100");
DebugTN("PT100Info created");
//----------

dpSet("GasMonitoring.dpTypes",makeDynString("GAS_SX5","PT100"));

DebugTN("types: GasMonitoring of FwNode created");

mudcsInit();

//-----------
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
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++PT100 done");
}
//============================
//============================
PT100_create(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","module_state");
xxdepes[3] = makeDynString ("","coord");
xxdepes[4] = makeDynString ("","status");
xxdepes[5] = makeDynString ("","off_channels");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_DYN_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
//============================
PT100_create_d(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","data","PT100_data");
xxdepes[3] = makeDynString ("","update_value");
xxdepes[4] = makeDynString ("","setNumber");
xxdepes[5] = makeDynString ("","dynatemTcpIp");

xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_STRING);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
PT100_create_data(string dtype)
{


char a;
a=50;

int b=a;

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","error_status");

//--------------

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);


}
int STATION_ITEMS_NUMBER=168; // 84+84 
int LASERS_NUMBER=6;


ALNM_1_main(){
int i;


ALNM_1_create_data("ALNM_1_data");
ALNM_1_create_d("ALNM_1_d");
ALNM_1_create("ALNM_1");
DebugTN("types: ALNM_* created");
//---------
dpCreate("ALNM_1_DimBroker_o","ALNM_1_d");
DebugTN("dp: ALNM_1_DimBroker_o created");
//---------
dpCreate("ALNM_1Info","_FwDeviceDefinition");
dpSet("ALNM_1Info.type","ALNM Device (1)");
dpSet("ALNM_1Info.dpType","ALNM_1"); //?
DebugTN("ALNM_1Info created");
//----------
//----------
dpCreate("ALNM","FwNode");
dpSet("ALNM.type","VENDOR");
dpSet("ALNM.dpTypes",makeDynString("ALNM_1"));
dpSet("ALNM.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("ALNM.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("ALNM.model","ALNM");
dpSet("ALNM.fsmState",0);
dpSet("ALNM.fsmCommand",0);
DebugTN("types: ALNM of FwNode created");



DebugTN("types: ALNM of FwNode created");
mudcsInit();

//-----------
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
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ALNM_1 done");

}
//============================
//============================
//============================
ALNM_1_create(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","module_state");
xxdepes[3] = makeDynString ("","coord");
xxdepes[4] = makeDynString ("","status");
xxdepes[5] = makeDynString ("","off_channels");
xxdepes[6] = makeDynString ("","off_channels_volt");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_DYN_INT);
xxdepei[6]=makeDynInt (0,DPEL_DYN_FLOAT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
//============================
ALNM_1_create_d(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","data","ALNM_1_data");
xxdepes[3] = makeDynString ("","update_value");
xxdepes[4] = makeDynString ("","setNumber");
xxdepes[5] = makeDynString ("","dynatemTcpIp");


xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_TYPEREF);
xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_STRING);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
ALNM_1_create_data(string dtype)
{


char a;
a=50;

int b=a;

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;
int i,i1;


xxdepes[1] = makeDynString (dtype,"");

for(i=2;i<=(STATION_ITEMS_NUMBER+1);i++){
i1=i-2+1;
xxdepes[i] = makeDynString ("","item"+i1);
}
//////xxdepes[8] = makeDynString ("","crate","ALNM_Crate");

for(i=(STATION_ITEMS_NUMBER+2);i<=(STATION_ITEMS_NUMBER+LASERS_NUMBER+1);i++){
i1=i-STATION_ITEMS_NUMBER-2+1;
xxdepes[i] = makeDynString ("","status"+i1);
}
xxdepes[STATION_ITEMS_NUMBER+LASERS_NUMBER+2] = makeDynString ("","general_status");
//=====================================

xxdepei[1]=makeDynInt (DPEL_STRUCT);

for(i=2;i<=(STATION_ITEMS_NUMBER+2);i++){
xxdepei[i]=makeDynInt (0,DPEL_FLOAT);
}
//////xxdepei[8]=makeDynInt (0,DPEL_TYPEREF);

for(i=(STATION_ITEMS_NUMBER+2);i<=(STATION_ITEMS_NUMBER+LASERS_NUMBER+1);i++){
xxdepei[i]=makeDynInt (0,DPEL_INT);
}

xxdepei[STATION_ITEMS_NUMBER+LASERS_NUMBER+2]=makeDynInt (0,DPEL_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}Snmp_main(){
int i;


Snmp_create_cr_d(/*"fwWnrCr_CSC_LV_d"*/);
Snmp_create_cr(/*"fwWnrCr_CSC_LV"*/);

Snmp_create_ch_d(/*"fwWnrCh_CSC_LV_d"*/);
Snmp_create_ch(/*"fwWnrCh_CSC_LV"*/);

DebugTN("types: fwWnr_* created");
//---------
dpCreate("fwWnrCr_CSC_LV_DimBroker_o","fwWnrCr_CSC_LV_d");
dpCreate("fwWnrCh_CSC_LV_DimBroker_o","fwWnrCh_CSC_LV_d");
DebugTN("dp: *_1_DimBroker_o created");
//---------
dpCreate("fwWnrCr_CSC_LVInfo","_FwDeviceDefinition");
dpSet("fwWnrCr_CSC_LVInfo.type","fwWnrCr_CSC_LV Device");
dpSet("fwWnrCr_CSC_LVInfo.dpType","fwWnrCr_CSC_LV"); //?
DebugTN("fwWnrCr_CSC_LVInfo created");
//----------
//---------
dpCreate("fwWnrCh_CSC_LVInfo","_FwDeviceDefinition");
dpSet("fwWnrCh_CSC_LVInfo.type","fwWnrCh_CSC_LV Device");
dpSet("fwWnrCh_CSC_LVInfo.dpType","fwWnrCh_CSC_LV"); //?
DebugTN("fwWnrCh_CSC_LVInfo created");
//----------
//----------
dpCreate("WienerSnmp","FwNode");
dpSet("WienerSnmp.type","VENDOR");
dpSet("WienerSnmp.dpTypes",makeDynString("fwWnrCr_CSC_LV","fwWnrCh_CSC_LV"));
dpSet("WienerSnmp.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("WienerSnmp.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("WienerSnmp.model","WienerSnmp");
dpSet("WienerSnmp.fsmState",0);
dpSet("WienerSnmp.fsmCommand",0);
DebugTN("types: WienerSnmp of FwNode created");

mudcsInit();

//-----------
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
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Snmp done");

}
//============================
//============================


//============================
Snmp_create_cr()
{

string dtype="fwWnrCr_CSC_LV";   
  
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","module_state");
xxdepes[3] = makeDynString ("","coord");
xxdepes[4] = makeDynString ("","status");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
//============================
Snmp_create_ch()
{

string dtype="fwWnrCh_CSC_LV";   
  
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","module_state");
xxdepes[3] = makeDynString ("","coord");
xxdepes[4] = makeDynString ("","status");
xxdepes[5] = makeDynString ("","last_vset");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_INT);
xxdepei[3]=makeDynInt (0,DPEL_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//============================
//============================

Snmp_create_cr_d()
{

string dtype="fwWnrCr_CSC_LV_d";  
  
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","sysStatus");
xxdepes[3] = makeDynString ("","sysMainSwitch");
xxdepes[4] = makeDynString ("","psOperatingTime");
xxdepes[5] = makeDynString ("","sysUpTime");
xxdepes[6] = makeDynString ("","nodeID");
xxdepes[7] = makeDynString ("","sysDescr");
xxdepes[8] = makeDynString ("","psSerialNumber");

xxdepes[9] = makeDynString ("","mainOn");
xxdepes[10] = makeDynString ("","mainInhibit");
xxdepes[11] = makeDynString ("","localControlOnly");
xxdepes[12] = makeDynString ("","inputFailure");
xxdepes[13] = makeDynString ("","outputFailure");
xxdepes[14] = makeDynString ("","fantrayFailure");
xxdepes[15] = makeDynString ("","sensorFailure");
xxdepes[16] = makeDynString ("","VmeSysfail");
//xxdepes[17] = makeDynString ("","");

xxdepes[17] = makeDynString ("","update_value");

//-----------

xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_UINT);
xxdepei[3]=makeDynInt (0,DPEL_INT);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6]=makeDynInt (0,DPEL_STRING);
xxdepei[7]=makeDynInt (0,DPEL_STRING);
xxdepei[8]=makeDynInt (0,DPEL_STRING);

xxdepei[9]=makeDynInt (0,DPEL_INT);
xxdepei[10]=makeDynInt (0,DPEL_INT);
xxdepei[11]=makeDynInt (0,DPEL_INT);
xxdepei[12]=makeDynInt (0,DPEL_INT);
xxdepei[13]=makeDynInt (0,DPEL_INT);
xxdepei[14]=makeDynInt (0,DPEL_INT);
xxdepei[15]=makeDynInt (0,DPEL_INT);
xxdepei[16]=makeDynInt (0,DPEL_INT);
//xxdepei[17]=makeDynInt (0,DPEL_INT);

xxdepei[17]=makeDynInt (0,DPEL_INT);


emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}

//=============================
//============================
Snmp_create_ch_d()
{

string dtype="fwWnrCh_CSC_LV_d";  
  
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");

xxdepes[2] = makeDynString ("","name");
xxdepes[3] = makeDynString ("","meas_volt");
xxdepes[4] = makeDynString ("","meas_cur");
xxdepes[5] = makeDynString ("","meas_term_volt");
xxdepes[6] = makeDynString ("","out_volt");
xxdepes[7] = makeDynString ("","adj_volt");
xxdepes[8] = makeDynString ("","cur_limit");
xxdepes[9] = makeDynString ("","min_sense_volt");
xxdepes[10] = makeDynString ("","max_sense_volt");
xxdepes[11] = makeDynString ("","max_term_volt");
xxdepes[12] = makeDynString ("","max_cur");
xxdepes[13] = makeDynString ("","cfg_max_sense_volt");
xxdepes[14] = makeDynString ("","cfg_max_term_volt");
xxdepes[15] = makeDynString ("","cfg_max_cur");
xxdepes[16] = makeDynString ("","max_power");
xxdepes[17] = makeDynString ("","group");
xxdepes[18] = makeDynString ("","out_switch");
xxdepes[19] = makeDynString ("","status");

xxdepes[20] = makeDynString ("","outputOn");
xxdepes[21] = makeDynString ("","outputInhibit");
xxdepes[22] = makeDynString ("","outputFailureMinSenseVoltage");
xxdepes[23] = makeDynString ("","outputFailureMaxSenseVoltage");
xxdepes[24] = makeDynString ("","outputFailureMaxTerminalVoltage");
xxdepes[25] = makeDynString ("","outputFailureMaxCurrent");
xxdepes[26] = makeDynString ("","outputFailureMaxTemperature");
xxdepes[27] = makeDynString ("","outputFailureMaxPower");

xxdepes[28] = makeDynString ("","update_value");

xxdepei[1]=makeDynInt (DPEL_STRUCT);

xxdepei[2]=makeDynInt (0,DPEL_STRING);
xxdepei[3]=makeDynInt (0,DPEL_FLOAT);
xxdepei[4]=makeDynInt (0,DPEL_FLOAT);
xxdepei[5]=makeDynInt (0,DPEL_FLOAT);
xxdepei[6]=makeDynInt (0,DPEL_FLOAT);
xxdepei[7]=makeDynInt (0,DPEL_INT);
xxdepei[8]=makeDynInt (0,DPEL_FLOAT);
xxdepei[9]=makeDynInt (0,DPEL_FLOAT);
xxdepei[10]=makeDynInt (0,DPEL_FLOAT);
xxdepei[11]=makeDynInt (0,DPEL_FLOAT);
xxdepei[12]=makeDynInt (0,DPEL_FLOAT);
xxdepei[13]=makeDynInt (0,DPEL_FLOAT);
xxdepei[14]=makeDynInt (0,DPEL_FLOAT);
xxdepei[15]=makeDynInt (0,DPEL_FLOAT);
xxdepei[16]=makeDynInt (0,DPEL_FLOAT);
xxdepei[17]=makeDynInt (0,DPEL_INT);
xxdepei[18]=makeDynInt (0,DPEL_UINT);
xxdepei[19]=makeDynInt (0,DPEL_INT);

xxdepei[20]=makeDynInt (0,DPEL_INT);
xxdepei[21]=makeDynInt (0,DPEL_INT);
xxdepei[22]=makeDynInt (0,DPEL_INT);
xxdepei[23]=makeDynInt (0,DPEL_INT);
xxdepei[24]=makeDynInt (0,DPEL_INT);
xxdepei[25]=makeDynInt (0,DPEL_INT);
xxdepei[26]=makeDynInt (0,DPEL_INT);
xxdepei[27]=makeDynInt (0,DPEL_INT);

xxdepei[28]=makeDynInt (0,DPEL_INT);

emu_dpTypeCreateOrChange(xxdepes,xxdepei);

}
