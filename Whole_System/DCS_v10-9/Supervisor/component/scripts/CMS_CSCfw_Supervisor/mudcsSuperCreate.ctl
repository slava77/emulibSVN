#uses "CMS_CSCfw_Supervisor/emuDcsSuper.ctl"
#uses "CMS_CSCfw_Supervisor/mudcs0SuperCreateFsm.ctl"
#uses "CMS_CSCfw_Supervisor/mudcs1SuperSetPanels.ctl"
#uses "CMS_CSCfw_Supervisor/mudcs3SuperCreateCentralDcsFsm.ctl"


main(){
  
//  mudcsSuperCreateTree();
  
  DebugTN("END OF FSM CREATE =======================================================1");
// mudcsSuperSetPanels(); 
 mudcsSuper_createFsmNew();
   DebugTN("END OF FSM CREATE =======================================================2");
 exit(0); 
   DebugTN("END OF FSM CREATE =======================================================3");
  
}
