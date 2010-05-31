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
  dpSet("MYWARNING_GAS.value", "starting mudcsGasFsmDelete()");
  mudcsGasFsmDelete();
  dpSet("MYWARNING_GAS.value", "starting mudcsDipConfig(false)");
   mudcsDipConfig(false); 
  dpSet("MYWARNING_GAS.value", "starting mudcsDeleteGas_dps(true)");
  mudcsDeleteGas_dps(true);
 
  //=== creation ==============
  
  dpSet("MYWARNING_GAS.value", "starting GasSystem_main()");
  GasSystem_main();
  dpSet("MYWARNING_GAS.value", "starting mudcsDipConfig(true)");
  mudcsDipConfig(true); 
  dpSet("MYWARNING_GAS.value", "starting mudcsGasCreateFsmMain()");
  mudcsGasCreateFsmMain();
  dpSet("MYWARNING_GAS.value", "starting mudcsGasAlertCreateMain(true)");
  mudcsGasAlertCreateMain(true);
  dpSet("MYWARNING_GAS.value", "starting mudcsGasArchiveMain(true)");
  mudcsGasArchiveMain(true);
  dpSet("MYWARNING_GAS.value", "starting mudcsGasFsmPanelsAndFsmAliasesSetMain()");
  mudcsGasFsmPanelsAndFsmAliasesSetMain();  
 
  dpSet("MYWARNING_GAS.value", "postinstall finished");

  exit(0);
   
}

