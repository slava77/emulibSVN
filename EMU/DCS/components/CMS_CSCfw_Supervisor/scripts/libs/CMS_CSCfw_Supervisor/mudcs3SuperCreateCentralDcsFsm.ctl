#uses "CMS_CSCfw_Supervisor/emuDcsSuper.ctl"
#uses "CMS_CSCfw_Supervisor/mudcs0SuperCreateFsm.ctl"

mudcsSuper_createFsmNew() {
  const string topDomain = "CMS_CSC";
  const string plusDomain = "CMS_CSC_PLUS";
  const string minusDomain = "CMS_CSC_MINUS";
  const string dubnaDomain = "CSC_ME_11_HV";
  const string gasNode = "CSC_GAS";
  const string coolingNode = "CSC_COOLING";
  const string plusFedNode = "CSC_FED_P";
  const string minusFedNode = "CSC_FED_M";
  const string plusHvNode = "CSC_HV_P";
  const string minusHvNode = "CSC_HV_M";
  const string plusLvNode = "CSC_LV_P";
  const string minusLvNode = "CSC_LV_M";
  const string mainHvNode = "CSC_HV";
  const string mainLvNode = "CSC_LV";
  const string plusUfHvNode= "CSC_HV::CSC_HV_HOST_600";
  const string minusUfHvNode = "CSC_HV::CSC_HV_HOST_500";
  const string plusUfHvLvNode = "CSC_LV::CSC_WIENER_CRATE_SNMP_600";
  const string minusUfHvLvNode = "CSC_LV::CSC_WIENER_CRATE_SNMP_500";
  const string plusDubnaHvNode = "CSC_ME_11_HV::CSC_ME_P11_HV";
  const string minusDubnaHvNode = "CSC_ME_11_HV::CSC_ME_N11_HV";
  const string plusMrtnNode= "CSC_LV::CSC_LV_MRTN_P";
  const string minusMrtnNode = "CSC_LV::CSC_LV_MRTN_M";
  const string plusUfHvNodeLabel = "UFPNPI_HV_P";
  const string minusUfHvNodeLabel = "UFPNPI_HV_M";
  const string plusUfHvLvNodeLabel = "UFPNPI_LV_FOR_HV_P";
  const string minusUfHvLvNodeLabel = "UFPNPI_LV_FOR_HV_M";
  const string plusDubnaHvNodeLabel = "ME11_HV_P";
  const string minusDubnaHvNodeLabel = "ME11_HV_M";

  mudcsSuperInit();  

  DebugTN("Creating central DCS nodes");
  
// ====== CLEANUP ======  
  if (fwFsmTree_isNode(topDomain)) {
    DebugTN("Deleting " + topDomain);
    fwFsmTree_removeNode("FSM", topDomain);
    deleteFwTreeNodes(makeDynString("fwTN_CMS_CSC"));
  }
  if (fwFsmTree_isNode(plusDomain)) {
    DebugTN("Deleting " + plusDomain);
    fwFsmTree_removeNode("FSM", plusDomain);
    deleteFwTreeNodes(makeDynString("fwTN_CMS_CSC_PLUS", "fwTN_CSC_HV_PLUS"));
  }
  if (fwFsmTree_isNode(minusDomain)) {
    DebugTN("Deleting " + minusDomain);
    fwFsmTree_removeNode("FSM", minusDomain);
    deleteFwTreeNodes(makeDynString("fwTN_CMS_CSC_MINUS", "fwTN_CSC_HV_MINUS"));
  }
  dyn_string ctrlUnits = dpNames("*", "_FwCtrlUnit");
  for (int i=1; i <= dynlen(ctrlUnits); i++) {
    dpDelete(ctrlUnits[i]);
  }
// ====== start creating the tree ======  
  
//top nodes  
  mudcsSuper_createFsmNode("FSM", topDomain, CSC_fwGSuper_g_MainLogicalFsmType, 1);  
  mudcsSuper_createFsmNode(topDomain, plusDomain, CSC_fwGSuper_g_MainLogicalFsmType, 1);  
  mudcsSuper_createFsmNode(topDomain, minusDomain, CSC_fwGSuper_g_MainLogicalFsmType, 1);

//HV nodes
  mudcsSuper_createFsmReferenceNode(plusDomain, plusHvNode, 1);
  mudcsSuper_createFsmReferenceNode(minusDomain, minusHvNode, 1);
      
//LV nodes
  mudcsSuper_createFsmReferenceNode(plusDomain, plusLvNode, 1);
  mudcsSuper_createFsmReferenceNode(minusDomain, minusLvNode, 1);

// GAS and COOLING
  mudcsSuper_createFsmReferenceNode(plusDomain, gasNode, 1);
  mudcsSuper_createFsmReferenceNode(minusDomain, gasNode, 1);
  mudcsSuper_createFsmReferenceNode(plusDomain, coolingNode, 1);
  mudcsSuper_createFsmReferenceNode(minusDomain, coolingNode, 1);
  
// FED nodes  
  mudcsSuper_createFsmReferenceNode(plusDomain, plusFedNode, 1);
  mudcsSuper_createFsmReferenceNode(minusDomain, minusFedNode, 1);  
  
//stations
  dyn_string stations = CSC_fwGSuper_g_project_stations_super;
  for (int i=1; i <= dynlen(stations); i++) {
    string node = "CSC_ME_" + stations[i];
    string parent = minusDomain;
    if (strpos(stations[i], "P") >= 0) {
      parent = plusDomain;
    }
    mudcsSuper_createFsmReferenceNode(parent, node, 1);
  }
  
  fwFsmTree_generateAll();
  fwFsmTree_refreshTree();
  
//  mudcsSuper_changeFsmNodeLabel(plusHvNode + "|" + mainHvNode, mudcsSuper_getShortFsmNodeName(plusUfHvNode), plusUfHvNodeLabel);
//  mudcsSuper_changeFsmNodeLabel(minusHvNode + "|" + mainHvNode, mudcsSuper_getShortFsmNodeName(minusUfHvNode), minusUfHvNodeLabel);
//  mudcsSuper_changeFsmNodeLabel(plusLvNode + "|" + mainLvNode, mudcsSuper_getShortFsmNodeName(plusUfHvLvNode), plusUfHvLvNodeLabel);
//  mudcsSuper_changeFsmNodeLabel(minusLvNode + "|" + mainLvNode, mudcsSuper_getShortFsmNodeName(minusUfHvLvNode), minusUfHvLvNodeLabel);
//  mudcsSuper_changeFsmNodeLabel(plusHvNode + "|" + dubnaDomain, mudcsSuper_getShortFsmNodeName(plusDubnaHvNode), plusDubnaHvNodeLabel);
//  mudcsSuper_changeFsmNodeLabel(minusHvNode + "|" + dubnaDomain, mudcsSuper_getShortFsmNodeName(minusDubnaHvNode), minusDubnaHvNodeLabel);
  
  DebugTN("DONE creating central DCS nodes");
}

void mudcsSuper_createFsmNode(string parent, string nodeName, string type, int cuFlag) {
  DebugTN("===== creating " + nodeName + " of type " + type + " to " + parent);
  fwFsmTree_addNode(parent, nodeName, type, cuFlag);
}

void mudcsSuper_createFsmReferenceNode(string parent, string nodeName, int cuFlag) {
  string type;
  fwCU_getType(nodeName, type);
  if (strlen(type) == 0) {
    DebugTN("ERROR: Cannot determine type of node " + nodeName + ". Node is not created.");
    return;
  }
  mudcsSuper_createFsmNode(parent, nodeName, type, cuFlag);
}

void mudcsSuper_changeFsmNodeLabel(string domain, string object, string label) {
   string panel;
   panel = fwFsm_getDefaultPanel(domain, object);
   fwFsm_setObjectLabelPanel(domain, object, label, panel, true, true);
}

string mudcsSuper_getShortFsmNodeName(string nodeName) {
  string shortName = nodeName;
  if (strpos(nodeName, "::") >= 0) {
    shortName = substr(nodeName, strpos(nodeName, "::") + 2);
  }
  return shortName;
}

/** Deletes fwTreeNodes having parents passed with the parameter (or name is like one of the parents)
    This is sometimes useful to clean up orphaned fwTreeNodes. */
void deleteFwTreeNodes(dyn_string parents) {
  dyn_string dps = dpNames("*", "_FwTreeNode");
  for (int i=1; i <= dynlen(dps); i++) {
    string parent;
    dpGet(dps[i] + ".parent", parent);
    // check if parent is in the array and if so - delete the node
    for (int j=1; j <= dynlen(parents); j++) {
      if ((strpos(dps[i], parents[j]) >= 0) || (strpos(parent, parents[j]) >= 0)) {
        dpDelete(dps[i]);
        break;
      } 
    }
  }
}
