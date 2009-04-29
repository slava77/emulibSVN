mudcsFsmCreateMain(){
  
  mudcsInit();
  mudcsCreateAllTrees(0);
dpSet(CSC_fwG_g_SYSTEM_NAME+":"+"EMUALIASES.",CSC_fwG_g_EMU_ALIASES);
 fwFsmTree_refreshTree(); 
}
