#uses "CMS_CSCfw_HV_CC/emuDcs.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs2.ctl"

 #uses "CMS_CSCfw_LV_CRB/mudcs9XAlertReconfigAllSlowControls.ctl"

#uses "CMS_CSCfw_GAS/emuGas.ctl"
#uses "CMS_CSCfw_GAS/mudcs0GasCreateAllTypes.ctl"
#uses "CMS_CSCfw_GAS/mudcs1GasDipConfig.ctl"
#uses "CMS_CSCfw_GAS/mudcs2GasFsmCreate.ctl"
#uses "CMS_CSCfw_GAS/mudcs3GasAlert.ctl"
#uses "CMS_CSCfw_GAS/mudcs4GasArchive.ctl"
#uses "CMS_CSCfw_GAS/mudcs5GasSetPanels.ctl"

#uses "aes.ctl"


main(){
  
  if (!dpExists("MYWARNING_GAS")) {
    dpCreate("MYWARNING_GAS", "MUDCS_STRING");
  }

  dpSet("MYWARNING_GAS.value", "postinstall started");
  
  //=== deletion =============
  mudcsGasFsmDelete();
   mudcsDipConfig(false); 
  mudcsDeleteGas_dps(true);
 
  //=== creation ==============
  
  GasSystem_main();
  mudcsDipConfig(true); 
  mudcsGasCreateFsmMain();
  mudcsGasAlertCreateMain(true);
  mudcsGasArchiveMain(true);
  mudcsGasFsmPanelsAndFsmAliasesSetMain();  
 
  dpSet("MYWARNING_GAS.value", "postinstall finished");

  exit(0);
   
}

