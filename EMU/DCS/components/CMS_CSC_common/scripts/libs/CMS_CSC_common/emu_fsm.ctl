/**@file

This package contains helper functions related to FSM.

@author Evaldas Juska (FNAL)
@date   February 2012
*/

#uses "CMS_CSC_common/emu_common.ctl"

/**
  * Creates a reference to remote FSM node. Checks if the node exists remotely and doesn't exist locally.
  */
void emuFsm_createRefNode(string nodeName, string parentNode, string remoteDomain, int cuFlag, dyn_string &ex) {
  if (fwFsmTree_isNode(nodeName)) { // is the node reachable?
    if (!fwFsmTree_isNode(parentNode + "::" + nodeName)) { // if already exists locally - skip
      string type;
      fwCU_getType(nodeName, type);  // get the type
      if (strlen(type) == 0) {
        emu_addError("ERROR: Cannot determine type of node " + nodeName + ". Node is not created.", ex);
        return;
      }
      
      if (remoteDomain != "") {
        nodeName = remoteDomain + "::" + nodeName;
      }
      emuFsm_createFsmNode(parentNode, nodeName, type, cuFlag); // create the node
    }
  } else {
    emu_addError("Remote FSM node could not be found: " + nodeName, ex);
    return;
  }
}

/**
  * Creates an FSM node - same as fwFsmTree_addNode, but also has some logging.
  */
void emuFsm_createFsmNode(string parent, string nodeName, string type, int cuFlag) {
  emu_info("===== creating " + nodeName + " of type " + type + " to " + parent);
  fwFsmTree_addNode(parent, nodeName, type, cuFlag);
}

/** Deletes fwTreeNodes having parents passed with the parameter (or name is like one of the parents)
    This is sometimes useful to clean up orphaned fwTreeNodes. */
void emuFsm_deleteFwTreeNodes(dyn_string parents) {
  dyn_string dps = dpNames("*", "_FwTreeNode");
  for (int i=1; i <= dynlen(dps); i++) {
    string parent;
    dpGet(dps[i] + ".parent", parent);
    // check if parent is in the array and if so - delete the node
    for (int j=1; j <= dynlen(parents); j++) {
      if ((strpos(dpSubStr(dps[i], DPSUB_DP), parents[j]) >= 0) || (strpos(parent, parents[j]) >= 0)) {
        emu_info("deleting " + dps[i]);
        dpDelete(dps[i]);
        break;
      } 
    }
  }
}
