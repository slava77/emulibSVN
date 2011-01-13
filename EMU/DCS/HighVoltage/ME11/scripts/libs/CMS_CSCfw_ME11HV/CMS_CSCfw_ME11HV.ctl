#uses "fwInstallation.ctl"
#uses "email.ctl"
/**@file
This library contains function associated with the CSC ME11 component:
 -- get system name where the CSC ME11 component has been installed
 -- set archiving config for HV datapoints  
 -- set Db connection
@par Creation Date
	03/2010  Xiaofeng Yang(PH-UCM)
@par Modification History                   
@par Constraints
	for CSC ME11 only,executing after component installed
@par Usage
	Private
        lib        : CMS_CSCfw_ME11HV.ctl
@par PVSS managers
	VISION, CTRL
@author 
	Xiaofeng Yang (PH-UCM)
*/
//define constant for archiving dp
//    const int       iTimeInt     = 7200;    //time interval for voltage/current
//    const float     fTolCur      = 0.5;     // tol for current
//    const float     fTolVolt     = 10;      // tol for voltage
//-----------------------------------------------------------------------------------------------------------------------------------------------
//this function to get system name where the component(CMS_CSCfw_ME11HV) installed 
//and add two global vars for panels and scripts
//of ME11
void emudcsME11_getSystemName()
{
    dyn_string dsCscME11HV;
  //   fwInstallation_getApplicationSystem("fwElmb",dsEndcapAlignment);
   fwInstallation_getApplicationSystem("CMS_CSCfw_ME11HV",dsCscME11HV);
    
    addGlobal("gCscME11HV", STRING_VAR);
    
    //change into global var using by all scripts
    gCscME11HV = dsCscME11HV[1];
}

// modify dp arch config
void emudcsME11_archivingConfigChange(float fTolCur,float fTolVolt,int iTimeInt)
{ 
   dyn_string dsChannelNames = dpNames("*","FwCaenChannel");
    for (int i=1;i<=dynlen(dsChannelNames);i++)
   {
      emudcsME11_setArchivingConfig(dsChannelNames[i]+".actual.iMon",fTolCur,iTimeInt);
      emudcsME11_setArchivingConfig(dsChannelNames[i]+".actual.vMon",fTolVolt,iTimeInt);
   }   
}
// set FSM archive
void emudcsME11_setFsmArchiving()
{    
 //---------get dp  for FSM device---------------------------
  dyn_string dsDpName_FSMdev = dpNames("*","_FwFsmDevice"); 
  //DebugN(dsDpName_FSMdev); 
  int iLen_FSMdev = dynlen(dsDpName_FSMdev);
   if(iLen_FSMdev == 0)
  {
     DebugTN("no FSM devive found, skip");
  }
  else    
  {  
    DebugTN("config archiving dps for FSM devices");  
     for (int i=1;i<=iLen_FSMdev;i++)
   {
    DebugN(dsDpName_FSMdev[i]);
    emudcsME11_setFsmDpConfig(dsDpName_FSMdev[i]);
   }  
  } 
  //---------get dp  for FSM object----------------------------    
  dyn_string dsDpName_FSMobj = dpNames("*","_FwFsmObject");
  //DebugN(dsDpName_FSMobj);
  int iLen_FSMobj = dynlen(dsDpName_FSMobj);
  if(iLen_FSMobj == 0)
  {
     DebugTN("no FSM object found, skip");
  }
  else
  {
    DebugTN("config archiving dps for FSM objects");
    for (int i=1;i<=iLen_FSMobj;i++)
    {
      DebugN(dsDpName_FSMobj[i]);
      emudcsME11_setFsmDpConfig(dsDpName_FSMobj[i]);
    }  
  }
}  
//--------------------------------------------------------------------------------------------
void emudcsME11_setArchivingConfig(string sDpName,float fTol,int iTime)
{
    dpSetWait (sDpName+ ":_archive.._type",45,
               sDpName+ ":_archive.._archive",1,
               sDpName+ ":_archive.1._type",3,
               sDpName+ ":_archive.1._class","_EVENT",
               sDpName+ ":_archive.1._interv",0,
               sDpName+ ":_archive.1._interv_type",0,
               sDpName+ ":_archive.1._std_type",2,        //time dep and value change
               sDpName+ ":_archive.1._std_tol",fTol,
               sDpName+ ":_archive.1._std_time",iTime);   // 7200 second
} 
//----FSM device dps configuration ---------
void emudcsME11_setFsmDpConfig(string sDpName)
{
  dyn_string ds = makeDynString( sDpName + ".fsm.currentState",
                                 sDpName + ".fsm.sendCommand",
                                 sDpName + ".fsm.executingAction",
                                 sDpName + ".fsm.currentParameters");
  for (int i=1; i<=dynlen(ds);i++)
  {
    emudcsME11_setFsmDpSmoothing(ds[i]);
  }
}
//-----create db config-----------------------------------------------------------------
void emudcsME11_createDbConfig()
{
   string accountName, databaseName,ps;
       accountName = "cms_me11_pvss_cond";
       databaseName = "cms_pvss_rdb";
       ps = "pvss_me11_dcs";
       
       fwRDBConfig_setUser(accountName);
       fwRDBConfig_setHost(databaseName);
       fwRDBConfig_setPassword(ps);
} 
void emudcsME11_setFsmDpSmoothing(string sDpName)
{
    dpSetWait (sDpName + ":_archive.._type",45,
               sDpName + ":_archive.._archive",1,
               sDpName + ":_archive.1._type",3,
               sDpName + ":_archive.1._class","_EVENT",
               sDpName + ":_archive.1._std_type",4);
}    

/** Sets the default HV settings like rampUp=7 V/s, i0=20 uA, etc. */
void emudcsME11_setDefaultHvSettings() {
  dyn_string dps;
  dps = dpNames(systemName + ":*HVME11*", "FwCaenChannel");
  
  for (int i=1; i <= dynlen(dps); i++) {
    dpSet(dps[i] + ".settings.onOff", false,
          dps[i] + ".settings.i0", 20,
          dps[i] + ".settings.rDwn", 100,
          dps[i] + ".settings.rUp", 7,
          dps[i] + ".settings.v0", 2900,
          dps[i] + ".settings.v1", 2300,
          dps[i] + ".settings.vMaxSoftValue", 3100);
  }
}
