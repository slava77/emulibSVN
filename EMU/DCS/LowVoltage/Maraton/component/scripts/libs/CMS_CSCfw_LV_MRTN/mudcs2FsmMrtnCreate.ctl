
mudcsMrtnFsmCreateMain() {
  int i;
  int iStationCross, iStation;
  int cu_flag;
  string emu_side;
  dyn_string ds1;
//================================================================

  int first_station, last_station;

  emu_side = mudcsMrtnPnlInit();
  if (emu_side == "_P") {
    first_station = 5;
    last_station = 8;

  } else if (emu_side == "_M") {
    first_station = 1;
    last_station = 4;
  } else if (emu_side == "") {
    first_station = 1;
    last_station = 8;
  }

// ds1=dpNames("CMS_CSC_LV_MRTN*","MUDCS_STRING");
// for(i=1;i<=dynlen(ds1);i++)dpDelete(ds1[i]);

// dpCreate("CMS_CSC_LV_MRTN"+emu_side);

//================================================================

  if (!dpExists("EMUALIASES.")) {
    dpCreate("EMUALIASES", "MUDCS_DYN_STRING");
  }


//======== main node creation (CMS_CSC_LV_CAN1) ==================

  CSC_fwCAN1_g_EmuCmsGlobalNode = CSC_fwCAN2_g_csc_part;
  CSC_fwCAN1_g_EmuCmsGlobalType = CSC_fwCAN1_g_MainLogicalFsmType;//"TEST_TYPE";
  CSC_fwCAN1_g_EmuCmsGlobalCu = "1";
  cu_flag = 1;
  CSC_fwCAN1_g_EmuCmsGlobalParent = "FSM";

  mudcsLv_addLogical(false, "", CSC_fwCAN1_g_EmuCmsGlobalNode, CSC_fwCAN1_g_EmuCmsGlobalType,
                     "CMS_CSCfw_LV_MRTN/config/emuEmptyConfig", "CMS_CSCfw_LV_MRTN/emuEmptyOperation");

  mudcsLv_addNode();
//=====================================================================
//===========================================
//// mudcsMrtnCreatePL500(CSC_fwCAN2_g_csc_part);

  for (iStationCross = first_station; iStationCross <= last_station; iStationCross++) {
    mudcsMrtnCreateMRTN(iStationCross, CSC_fwCAN2_g_csc_part);
  } // for iStationCross
//=====================================


  dpSet("EMUALIASES.", CSC_fwCAN2_g_EMU_ALIASES);

  fwFsmTree_generateAll();
  fwFsmTree_refreshTree();
//  mudcsLvDebug("Done");

  DebugTN("TREE IS DONE");
  DebugTN("TREE IS DONE");
  DebugTN("TREE IS DONE");
  DebugTN("TREE IS DONE");
  DebugTN("TREE IS DONE");
  DebugTN("TREE IS DONE");
  DebugTN("TREE IS DONE");
//======================================================================
}
//====================================================================
//mudcsMrtnCreateMRTN(int CreateLevel, int isAppendToFilledStation, int idisk, int idisk_cross,
//              string emu_system_side, string station_parent_node)
mudcsMrtnCreateMRTN(int iStationCross, string system_parent_node) {
  int iStation;
  string emu_system_side;
  dyn_string list, ds1, ds2, ds3 , ds4, ds5;
  int i, j;
  string sTemplate, sTemplate2;
  string dpN;
//---------------------------------------
  string parent_domain;
  string Node_save;
  int cu_flag;
  string parent_node, parent_node_2;

  dpGet(mudcsLvAddSystem("db_mrtn.list"), list);

//---------------------
  parent_domain = system_parent_node;

  CSC_fwCAN1_g_EmuCmsGlobalType = CSC_fwCAN1_g_NodeLogicalFsmType;
//mudcsMrtnNameCompose("LV_CRB", station_label, emu_system_side, idisk, "", "", CSC_fwCAN1_g_EmuCmsGlobalNode);
  mudcsMrtnGetStationInforByCrossNumber(iStationCross, iStation, emu_system_side);
  CSC_fwCAN1_g_EmuCmsGlobalNode = "CSC_ME_" + emu_system_side + iStation + "_LV_MRTN";

  parent_node = CSC_fwCAN1_g_EmuCmsGlobalNode;
//CSC_fwCAN1_g_EmuCmsGlobalNode="CSC"+station_label+emu_system_side+idisk+"PC";
  CSC_fwCAN1_g_EmuCmsGlobalCu = "0";
  cu_flag = 0; // logical unit
  CSC_fwCAN1_g_EmuCmsGlobalParent = parent_domain;

  mudcsLv_addLogical(false, CSC_fwCAN2_g_csc_part,
                     CSC_fwCAN1_g_EmuCmsGlobalNode, CSC_fwCAN1_g_EmuCmsGlobalType,
                     "CMS_CSCfw_LV_MRTN/config/emuEmptyConfig", "CMS_CSCfw_LV_MRTN/emuEmptyOperation");

  mudcsLv_addNode();

  DebugTN(CSC_fwCAN1_g_EmuCmsGlobalNode + " " + CSC_fwCAN1_g_EmuCmsGlobalParent);

//----------------------


  sTemplate = emu_system_side + iStation + "_";


  for (i = 1; i <= dynlen(list); i++) {
//DebugTN("====="+list[i]+" "+sTemplate);

    if (strpos(list[i], sTemplate) < 0)continue;
    ds1 = strsplit(list[i], ";");
    if (dynlen(ds1) < 2 || ds1[2] == "")continue;

    parent_domain = parent_node;

    CSC_fwCAN1_g_EmuCmsGlobalType = CSC_fwCAN1_g_NodeLogicalFsmType;
    CSC_fwCAN1_g_EmuCmsGlobalNode = "CSC_ME_" + ds1[1] + "_LV_MRTN";

    parent_node_2 = CSC_fwCAN1_g_EmuCmsGlobalNode;
// mudcsMrtnNameCompose(CSC_fwCAN1_g_EmuCmsGlobalType, station_label, emu_system_side, idisk, "", i, CSC_fwCAN1_g_EmuCmsGlobalNode);
// CSC_fwCAN1_g_EmuCmsGlobalNode= "CSC"+station_label+emu_system_side+idisk+"p"+i+"CRB_1";

    CSC_fwCAN1_g_EmuCmsGlobalCu = "0";
    cu_flag = 0;
    CSC_fwCAN1_g_EmuCmsGlobalParent = parent_domain;

//mudcsLv_addHardwareDevice(CSC_fwCAN1_g_EmuCmsGlobalNode,CSC_fwCAN1_g_EmuCmsGlobalType,"",dpN);
//CSC_fwCAN1_g_EmuCmsGlobalNode=dpN;

    mudcsLv_addLogical(false, system_parent_node + "/" + parent_domain,
                       CSC_fwCAN1_g_EmuCmsGlobalNode, CSC_fwCAN1_g_EmuCmsGlobalType,
                       "CMS_CSCfw_LV_MRTN/config/emuEmptyConfig", "CMS_CSCfw_LV_MRTN/emuEmptyOperation");

    mudcsLv_addNode();

    DebugTN("=====" + CSC_fwCAN1_g_EmuCmsGlobalNode + " " + CSC_fwCAN1_g_EmuCmsGlobalParent);


//continue; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//===========ADD DEVICES=============

    ds2 = dpNames("*Crate" + ds1[2], "FwWienerMarathon");

//----------------------

    parent_domain = parent_node_2;

    for (j = 1; j <= dynlen(ds2); j++) {

      ds4 = strsplit(ds2[j], "/");
      sTemplate2 = ds4[dynlen(ds4)];

      CSC_fwCAN1_g_EmuCmsGlobalType = "FwWienerMarathon";
// mudcsMrtnNameCompose(CSC_fwCAN1_g_EmuCmsGlobalType, station_label, emu_system_side, idisk, "", i, CSC_fwCAN1_g_EmuCmsGlobalNode);
// CSC_fwCAN1_g_EmuCmsGlobalNode= "CSC"+station_label+emu_system_side+idisk+"p"+i+"CRB_1";
      Node_save = CSC_fwCAN1_g_EmuCmsGlobalNode;
      CSC_fwCAN1_g_EmuCmsGlobalCu = "0";
      cu_flag = 0;
      CSC_fwCAN1_g_EmuCmsGlobalParent = parent_domain;

      CSC_fwCAN1_g_EmuCmsGlobalNode = ds2[j];

      mudcsLv_addLogical(true, system_parent_node + "/" + parent_node + "/" + parent_node_2,
                         /*Node_save*/ds2[j], CSC_fwCAN1_g_EmuCmsGlobalType,
                         "CMS_CSCfw_LV_MRTN/config/emuDevMRTNConfig", "fwWiener/fwWienerMarathonOperation.pnl");

      mudcsLv_addNode();
    } // for j
//-------------------

    ds2 = dpNames("*Crate" + ds1[2] + "/Channel*", "FwWienerMarathonChannel");

    for (j = 1; j <= dynlen(ds2); j++) {

      ds4 = strsplit(ds2[j], "/");
      sTemplate2 = ds4[dynlen(ds4)];

      CSC_fwCAN1_g_EmuCmsGlobalType = "FwWienerMarathonChannel";
// mudcsMrtnNameCompose(CSC_fwCAN1_g_EmuCmsGlobalType, station_label, emu_system_side, idisk, "", i, CSC_fwCAN1_g_EmuCmsGlobalNode);
// CSC_fwCAN1_g_EmuCmsGlobalNode= "CSC"+station_label+emu_system_side+idisk+"p"+i+"CRB_1";
      Node_save = CSC_fwCAN1_g_EmuCmsGlobalNode;
      CSC_fwCAN1_g_EmuCmsGlobalCu = "0";
      cu_flag = 0;
      CSC_fwCAN1_g_EmuCmsGlobalParent = parent_domain;

      CSC_fwCAN1_g_EmuCmsGlobalNode = ds2[j];

      mudcsLv_addLogical(true, system_parent_node + "/" + parent_node + "/" + parent_node_2,
                         /*Node_save*/ds2[j], CSC_fwCAN1_g_EmuCmsGlobalType,
                         "CMS_CSCfw_LV_MRTN/config/emuDevMRTNCHANConfig", "fwWiener/fwWienerMarathonRCMChannelOperation.pnl");

      mudcsLv_addNode();
    } // for j


//===================================
  } // for i
//--------------------


}

//====================================================================
mudcsMrtnGetStationInforByCrossNumber(int iStationCross, int &iStation, string &emu_system_side) {

  if (iStationCross <= 4) {
    iStation = 5 - iStationCross;
    emu_system_side = "M";
  } else {
    iStation = iStationCross - 4;
    emu_system_side = "P";
  }

}

//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================

mudcsMrtnCreatePL500(string system_parent_node) {
////  int iStation;
//// string emu_system_side;
  dyn_string list, ds1, ds2, ds3 , ds4, ds5;
  int i, j;
  string sTemplate, sTemplate2;
  string dpN;
//---------------------------------------
  string parent_domain;
  string Node_save;
  int cu_flag;
  string parent_node, parent_node_2;

  dpGet(mudcsLvAddSystem("db_mrtn.list"), list);

//---------------------
  parent_domain = system_parent_node;

  CSC_fwCAN1_g_EmuCmsGlobalType = CSC_fwCAN1_g_NodeLogicalFsmType;
//mudcsMrtnNameCompose("LV_CRB", station_label, emu_system_side, idisk, "", "", CSC_fwCAN1_g_EmuCmsGlobalNode);
//mudcsMrtnGetStationInforByCrossNumber(iStationCross, iStation, emu_system_side);
  CSC_fwCAN1_g_EmuCmsGlobalNode = "CSC_ME_LV_PL500";

  parent_node = CSC_fwCAN1_g_EmuCmsGlobalNode;
//CSC_fwCAN1_g_EmuCmsGlobalNode="CSC"+station_label+emu_system_side+idisk+"PC";
  CSC_fwCAN1_g_EmuCmsGlobalCu = "0";
  cu_flag = 0; // logical unit
  CSC_fwCAN1_g_EmuCmsGlobalParent = parent_domain;

  mudcsLv_addLogical(false, CSC_fwCAN2_g_csc_part,
                     CSC_fwCAN1_g_EmuCmsGlobalNode, CSC_fwCAN1_g_EmuCmsGlobalType,
                     "CMS_CSCfw_LV_MRTN/config/emuEmptyConfig", "CMS_CSCfw_LV_MRTN/emuEmptyOperation");

  mudcsLv_addNode();

  DebugTN(CSC_fwCAN1_g_EmuCmsGlobalNode + " " + CSC_fwCAN1_g_EmuCmsGlobalParent);

//----------------------
  parent_domain = parent_node;

  sTemplate = "PL500";


  for (i = 1; i <= dynlen(list); i++) {
//DebugTN("====="+list[i]+" "+sTemplate);

    if (strpos(list[i], sTemplate) < 0)break;
    ds1 = strsplit(list[i], ";");
    if (dynlen(ds1) < 2 || ds1[2] == "")continue;

    CSC_fwCAN1_g_EmuCmsGlobalType = CSC_fwCAN1_g_NodeLogicalFsmType;
    CSC_fwCAN1_g_EmuCmsGlobalNode = "CSC_ME_LV_" + ds1[1];

    parent_node_2 = CSC_fwCAN1_g_EmuCmsGlobalNode;
// mudcsMrtnNameCompose(CSC_fwCAN1_g_EmuCmsGlobalType, station_label, emu_system_side, idisk, "", i, CSC_fwCAN1_g_EmuCmsGlobalNode);
// CSC_fwCAN1_g_EmuCmsGlobalNode= "CSC"+station_label+emu_system_side+idisk+"p"+i+"CRB_1";

    CSC_fwCAN1_g_EmuCmsGlobalCu = "0";
    cu_flag = 0;
    CSC_fwCAN1_g_EmuCmsGlobalParent = parent_domain;

//mudcsLv_addHardwareDevice(CSC_fwCAN1_g_EmuCmsGlobalNode,CSC_fwCAN1_g_EmuCmsGlobalType,"",dpN);
//CSC_fwCAN1_g_EmuCmsGlobalNode=dpN;

    mudcsLv_addLogical(false, system_parent_node + "/" + parent_domain,
                       CSC_fwCAN1_g_EmuCmsGlobalNode, CSC_fwCAN1_g_EmuCmsGlobalType,
                       "CMS_CSCfw_LV_MRTN/config/emuEmptyConfig", "CMS_CSCfw_LV_MRTN/emuEmptyOperation");

    mudcsLv_addNode();

    DebugTN("=====" + CSC_fwCAN1_g_EmuCmsGlobalNode + " " + CSC_fwCAN1_g_EmuCmsGlobalParent);


//continue; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//===========ADD DEVICES=============

    ds2 = dpNames("*Crate" + ds1[2], "FwWienerPS");

//----------------------

    parent_domain = parent_node_2;

    for (j = 1; j <= dynlen(ds2); j++) {

      ds4 = strsplit(ds2[j], "/");
      sTemplate2 = ds4[dynlen(ds4)];

      CSC_fwCAN1_g_EmuCmsGlobalType = "FwWienerPS";
// mudcsMrtnNameCompose(CSC_fwCAN1_g_EmuCmsGlobalType, station_label, emu_system_side, idisk, "", i, CSC_fwCAN1_g_EmuCmsGlobalNode);
// CSC_fwCAN1_g_EmuCmsGlobalNode= "CSC"+station_label+emu_system_side+idisk+"p"+i+"CRB_1";
      Node_save = CSC_fwCAN1_g_EmuCmsGlobalNode;
      CSC_fwCAN1_g_EmuCmsGlobalCu = "0";
      cu_flag = 0;
      CSC_fwCAN1_g_EmuCmsGlobalParent = parent_domain;

      CSC_fwCAN1_g_EmuCmsGlobalNode = ds2[j];

      mudcsLv_addLogical(true, system_parent_node + "/" + parent_node + "/" + parent_node_2,
                         /*Node_save*/ds2[j], CSC_fwCAN1_g_EmuCmsGlobalType,
                         "CMS_CSCfw_LV_MRTN/config/emuDevMRTNConfig", "fwWiener/fwWienerPSOperation.pnl");

      mudcsLv_addNode();
    } // for j
//-------------------

    ds2 = dpNames("*Crate" + ds1[2] + "/Channel*", "FwWienerChannel");

    for (j = 1; j <= dynlen(ds2); j++) {

      ds4 = strsplit(ds2[j], "/");
      sTemplate2 = ds4[dynlen(ds4)];

      CSC_fwCAN1_g_EmuCmsGlobalType = "FwWienerChannel";
// mudcsMrtnNameCompose(CSC_fwCAN1_g_EmuCmsGlobalType, station_label, emu_system_side, idisk, "", i, CSC_fwCAN1_g_EmuCmsGlobalNode);
// CSC_fwCAN1_g_EmuCmsGlobalNode= "CSC"+station_label+emu_system_side+idisk+"p"+i+"CRB_1";
      Node_save = CSC_fwCAN1_g_EmuCmsGlobalNode;
      CSC_fwCAN1_g_EmuCmsGlobalCu = "0";
      cu_flag = 0;
      CSC_fwCAN1_g_EmuCmsGlobalParent = parent_domain;

      CSC_fwCAN1_g_EmuCmsGlobalNode = ds2[j];

      mudcsLv_addLogical(true, system_parent_node + "/" + parent_node + "/" + parent_node_2,
                         /*Node_save*/ds2[j], CSC_fwCAN1_g_EmuCmsGlobalType,
                         "CMS_CSCfw_LV_MRTN/config/emuDevMRTNCHANConfig", "fwWiener/fwWienerChannelOperation.pnl");

      mudcsLv_addNode();
    } // for j


//===================================
  } // for i
//--------------------


}

