#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSC_common/emu_fsm.ctl"

const string EMU_FSM_DOMAIN_TOP = "CMS_CSC";
const string EMU_FSM_DOMAIN_PLUS = "CMS_CSC_PLUS";
const string EMU_FSM_DOMAIN_MINUS = "CMS_CSC_MINUS";
const string EMU_FSM_DOMAIN_DUBNA = "CSC_ME_11_HV";
const string EMU_FSM_NODE_FED_PLUS = "CSC_FED_P";
const string EMU_FSM_NODE_FED_MINUS = "CSC_FED_M";
const string EMU_FSM_NODE_HV_PLUS = "CSC_HV_P";
const string EMU_FSM_NODE_HV_MINUS = "CSC_HV_M";

emuSuper_createFsmNew() {

  emu_info("Creating central DCS nodes");
  
// ====== CLEANUP ======  
  if (fwFsmTree_isNode(EMU_FSM_DOMAIN_TOP)) {
    emu_info("Deleting " + EMU_FSM_DOMAIN_TOP);
    fwFsmTree_removeNode("FSM", EMU_FSM_DOMAIN_TOP);
    emuFsm_deleteFwTreeNodes(makeDynString("fwTN_CMS_CSC"));
  }
  if (fwFsmTree_isNode(EMU_FSM_DOMAIN_PLUS)) {
    emu_info("Deleting " + EMU_FSM_DOMAIN_PLUS);
    fwFsmTree_removeNode("FSM", EMU_FSM_DOMAIN_PLUS);
    emuFsm_deleteFwTreeNodes(makeDynString("fwTN_CMS_CSC_PLUS", "fwTN_CSC_HV_PLUS"));
  }
  if (fwFsmTree_isNode(EMU_FSM_DOMAIN_MINUS)) {
    emu_info("Deleting " + EMU_FSM_DOMAIN_MINUS);
    fwFsmTree_removeNode("FSM", EMU_FSM_DOMAIN_MINUS);
    emuFsm_deleteFwTreeNodes(makeDynString("fwTN_CMS_CSC_MINUS", "fwTN_CSC_HV_MINUS"));
  }
  dyn_string ctrlUnits = dpNames("*", "_FwCtrlUnit");
  for (int i=1; i <= dynlen(ctrlUnits); i++) {
    dpDelete(ctrlUnits[i]);
  }

  // ====== start creating the tree ======  
  
  dyn_string ex;
  
//top nodes  
  emuFsm_createFsmNode("FSM", EMU_FSM_DOMAIN_TOP, CSC_fwGSuper_g_MainLogicalFsmType, 1);  
  emuFsm_createFsmNode(EMU_FSM_DOMAIN_TOP, EMU_FSM_DOMAIN_PLUS, CSC_fwGSuper_g_MainLogicalFsmType, 1);  
  emuFsm_createFsmNode(EMU_FSM_DOMAIN_TOP, EMU_FSM_DOMAIN_MINUS, CSC_fwGSuper_g_MainLogicalFsmType, 1);

//HV nodes
  emuFsm_createRefNode(EMU_FSM_NODE_HV_PLUS, EMU_FSM_DOMAIN_PLUS, "", true, ex);
  if (emu_checkException(ex)) { return; }
  emuFsm_createRefNode(EMU_FSM_NODE_HV_MINUS, EMU_FSM_DOMAIN_MINUS, "", true, ex);
  if (emu_checkException(ex)) { return; }
      
// FED nodes  
  emuFsm_createRefNode(EMU_FSM_NODE_FED_PLUS, EMU_FSM_DOMAIN_PLUS, "", true, ex);
  if (emu_checkException(ex)) { return; }  
  emuFsm_createRefNode(EMU_FSM_NODE_FED_MINUS, EMU_FSM_DOMAIN_MINUS, "", true, ex);
  if (emu_checkException(ex)) { return; }
  
//stations
  for (int stationNum = -4; stationNum <= 4; stationNum++) {
    if (stationNum == 0) { continue; }
    string stationStr, parent;
    if (stationNum < 0) {
      stationStr = "M" + (stationNum * -1);
      parent = EMU_FSM_DOMAIN_MINUS;
    } else {
      stationStr = "P" + stationNum;
      parent = EMU_FSM_DOMAIN_PLUS;
    }
   
    string node = "CSC_ME_" + stationStr;
    emuFsm_createRefNode(node, parent, "", true, ex);
    if (emu_checkException(ex)) { return; }
  }
  
  fwFsmTree_generateAll();
  fwFsmTree_refreshTree();
  
  DebugTN("DONE creating central DCS nodes");
}

