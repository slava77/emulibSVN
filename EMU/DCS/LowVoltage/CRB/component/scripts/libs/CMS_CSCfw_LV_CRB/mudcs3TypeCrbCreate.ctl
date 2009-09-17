mudcsTypeCrbCreateMain(){
int i;

mudcsCrbPnlInit(0);

string device_type="fwCrb_CSC_LV";

mudcsTypeCrbCreate("fwCrb_CSC_LV");
mudcsTypePsuCreate("fwPsu_CSC_LV");

DebugTN("types: CRB_* created");
//---------
//---------
dpCreate("fwCrb_CSC_LVInfo","_FwDeviceDefinition");
dpSet("fwCrb_CSC_LVInfo.type","CRB Device (2)");
dpSet("fwCrb_CSC_LVInfo.dpType","fwCrb_CSC_LV"); //?
DebugTN("fwCrb_CSC_LVInfo created");
//----------
//----------
dpCreate("CRB","FwNode");


dpSet("CRB.type","VENDOR");
dpSet("CRB.dpTypes",makeDynString("fwCrb_CSC_LV"));
dpSet("CRB.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("CRB.editorPanels",makeDynString("fwDevice/fwDeviceManage"));
dpSet("CRB.model","CRB");
dpSet("CRB.fsmState",0);
dpSet("CRB.fsmCommand",0);
DebugTN("types: CRB of FwNode created");


DebugTN("types: CRB of FwNode created");
//mudcsCrbInit();
//-----------
//mudcsCrb_setBrokerList();
//DebugTN("mudcsCrb_setBrokerList()");
//------------
//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
string device_info;


  device_info= device_type+"Info.panels.editor.hardware";
  dpSet(device_info,"CMS_CSCfw_LV_CONF/emuDev" + device_type+"Config");

  device_info= device_type+"Info.panels.navigator.hardware";
  dpSet(device_info,"CMS_CSCfw_LV/emuDev" + device_type+"Operation");

DebugTN("panels set");
//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF

 //mudcsLvDebug("Done");
}
//============================
//============================

//============================
mudcsTypeCrbCreate(string dtype)
{

dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","coord"); // ELMB_ID to get ai_ dps to use for aler configuration etc
xxdepes[3] = makeDynString ("","switch_list"); // list of cout dps to use in status handler
xxdepes[4] = makeDynString ("","status");
xxdepes[5] = makeDynString ("","off_channels");
xxdepes[6] = makeDynString ("","error_status"); // combination of wrong state: state not 5 or 133--> (1) and timeout--> (10)

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_STRING);
xxdepei[3]=makeDynInt (0,DPEL_DYN_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_DYN_INT);
xxdepei[6]=makeDynInt (0,DPEL_INT);

dpTypeCreate(xxdepes,xxdepei);

}

//=============================
//============================
mudcsTypePsuCreate(string dtype)
{

  
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","coord"); // ELMB_ID to get ai_ dps to use for aler configuration etc
xxdepes[3] = makeDynString ("","switch_list"); // list of cout dps to use in status handler
xxdepes[4] = makeDynString ("","status");
xxdepes[5] = makeDynString ("","off_channels");
xxdepes[6] = makeDynString ("","error_status"); // combination of wrong state: state not 5 or 133--> (1) and timeout--> (10)

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_STRING);
xxdepei[3]=makeDynInt (0,DPEL_DYN_STRING);
xxdepei[4]=makeDynInt (0,DPEL_INT);
xxdepei[5]=makeDynInt (0,DPEL_DYN_INT);
xxdepei[6]=makeDynInt (0,DPEL_INT);

dpTypeCreate(xxdepes,xxdepei);

}

//=============================
