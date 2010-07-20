#uses "CMS_CSC_common/emu_common.ctl"

mudcsFsmCreateMain(){
  mudcsFsmDeleteAll();
  mudcsInit();
  mudcsCreateAllTrees(0);
dpSet(CSC_fwG_g_SYSTEM_NAME+":"+"EMUALIASES.",CSC_fwG_g_EMU_ALIASES);
 fwFsmTree_refreshTree(); 
}

mudcsFsmDeleteAll() {
  emu_info("Deleting all FSM DPs...");
  
  dyn_string dpsToDelete;
  emu_dynAppend(dpsToDelete, dpNames("*", "_FwCtrlUnit"));
  emu_dynAppend(dpsToDelete, dpNames("*", "_FwFsmDevice"));
  emu_dynAppend(dpsToDelete, dpNames("*", "_FwFsmObject"));
  emu_dynAppend(dpsToDelete, dpNames("*", "_FwTreeNode"));
  
  emu_info("Deleting " + dynlen(dpsToDelete) + " DPs");
  
  for (int i=1; i <= dynlen(dpsToDelete); i++) {
    string dp = dpsToDelete[i];
    if ((strpos(dp, "Clipboard") >= 0) || (strpos(dp, "TrendTree") >= 0) || (strpos(dp, "FSM") >= 0)) { continue; } // don't delete framework DPs
    dpDelete(dp);
  }
  
  emu_info("Done deleting FSM DPs...");
}
