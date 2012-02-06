//#uses "fwRDBArchiving/fwRDBConfig.ctl"

/**@file
This library contains function associated with the CSC datapoints archive config:
 -- create special dps used by RDB archiving for all csc projects
 -- set archiving config for FSM, FwElmbAi, Cooling  
 -- CMS online Oracle database connection
@par Creation Date
	12/2008  Xiaofeng Yang(PH-UCM)

@par Modification History
	09/2009  Xiaofeng Yang(PH-UCM) 
				
	12/2009  Xiaofeng Yang(PH-UCM)
                 -- remove all hard-code, fit for all projects automaticlally
                 -- set archiving for FSM, FwElmb, Cooling dps automatically 	
  05/2010  Xiaofeng Yang(PH-UCM)
                 -- add archiving for HV primary power supply  
  07/2010  Xiaofeng Yang(PH-UCM)
                 -- add archiving for maraton status  
  09/2010  Xiaofeng Yang(PH-UCM)
                 -- add archiving for TMB LV status  
  03/2011  Xiaofeng Yang(PH-UCM)
                 -- add archiving for LVDB status/CCB_bits/FPGA_bits 
  02/2012  Xiaofeng Yang(PH-UCM)
                 -- modify for new middle layer project  
@par Constraints
	for CSC archiving set only,executing after CSC component installed
@par Usage
	Private
        lib        : CMS_CSCfw_RdbArchiving.ctl
        main script: emudcsArchive_configAllArchivingDps()
@par PVSS managers
	VISION, CTRL
@author 
	Xiaofeng Yang (PH-UCM)
*/
//-----define constant for archiving dp-----------------------------------
   const float     fTolHVV     = 20;      //deadband for HV_V
   const float     fTolHVI     = 1;       //deadband for HV_I
   const float     fTolHVMV    = 20;      //deadband for HV_MV
   const float     fTolHVMI    = 10;      //deadband for HV_MI
   const float     fTolHVPV    = 20;      //deadband for HV_PV
   const float     fTolHVPI    = 30;      //deadband for HV_PI
   const float     fTolLV      = 0.01;    //deadband for LV_V/I
   const float     fTolLvdb    = 0.1;     //deadband for LV_Lvdb
   const float     fTolTEMP    = 0.1;     //deadband for TEMP
   const float     fTolFEDV    = 0.01;    //deadband for FED_V
   const float     fTolFEDT    = 0.1;     //deadband for FED_T
   const float     fTolMA      = 1;       //deadband for MARATON channel
   const float     fTolElmbAi  = 1;       //deadband for ElmbAi 
   const float     fTolCooling = 0.5;     //deadband for Cooling
 
   const int       iTimeHV     = 7200;    //time interval for HV
   const int       iTimeHVM    = 7200;    //time interval for HVM
   const int       iTimeHVP    = 7200;    //time interval for HVP
   const int       iTimeLV     = 28800;   //time interval for LV
   const int       iTimeTEMP   = 28800;   //time interval for TEMP
   const int       iTimeFED    = 28800;   //time interval for FED
   const int       iTimeMA     = 7200;    //time interval for MARATON channel
   const int       iTimeElmbAi = 7200;    //time interval for ElmbAi
   const int       iTimeCooling= 7200;    //time interval for Cooling
   
   const string    sDpTypeHVV  = "CSC_HV_V_DATA";  //channel HV voltage
   const string    sDpTypeHVI  = "CSC_HV_I_DATA";  //channel HV current
   const string    sDpTypeHVM  = "CSC_HV_M_DATA";  //master HV voltage/current
   const string    sDpTypeHVP  = "CSC_HV_P_DATA";  //primary HV voltage/current
   const string    sDpTypeLVV  = "CSC_LV_V_DATA";
   const string    sDpTypeLVI  = "CSC_LV_I_DATA";
   const string    sDpTypeTEMP = "CSC_TEMP_DATA";
   const string    sDpTypeFEDV = "CSC_FED_V_DATA";
   const string    sDpTypeFEDT = "CSC_FED_T_DATA";
   const string    sDpTypeMAV = "CSC_MARATON_V_DATA";
   const string    sDpTypeMAI = "CSC_MARATON_I_DATA";
   const string    sDpTypeMAT = "CSC_MARATON_T_DATA";
   const string    sDpTypeTMB = "CSC_TMB_DATA";       //new data from TMB
   const string    sDpTypeLVDB = "CSC_LVDB_DATA";     // new data from LVDB
   const string    sDpTypeBITS = "CSC_BITS_DATA";     // new data for CCB_bits and FPGA_bits
   const bool      bDebug     = true;                 // true  --- enable debug info
                                                      // false --- disable debug info
//-------config all archiving dps ---------------------------------------------------  
void emudcsArchive_configAllArchivingDps()  
{  
   DebugTN("start archiving dp configuration..."); 
   DebugTN("please ignore any waining message");                   
//Clean all previous archiving dps and dptypes  
     emudcsArchive_cleanAllDps();
     delay(1);
//Create DpTypes   
     emudcsArchive_createDpTypes();
//Create all archiving dps
     emudcsArchive_setAllDpConfigs();   
   delay(1);
   DebugTN("done");           
}
//---------------clean all archiving dps and dptype if exist------------------------------ 
void emudcsArchive_cleanAllDps()
{
  DebugTN("Deleting existing archiving dps and dptype");
  dyn_string dsArchiveDpTypes = emudcsArchive_defineDpTypes(dsArchiveDpTypes);
  for (int i=1; i <= dynlen(dsArchiveDpTypes); i++)
   {
     dyn_string dsDps = dpNames("*", dsArchiveDpTypes[i]);
        for (int j=1; j <= dynlen(dsDps); j++)
      {
        dpDelete(dsDps[j]);
      }
      dpTypeDelete(dsArchiveDpTypes[i]);
    }
  DebugTN("Archiving dps and dptype deleted");
}  
//-------------- create dptype for archiving dp-------------------------------------------------
void emudcsArchive_createDpTypes()
{
  DebugTN("create archiving dptype"); 
  dyn_string dsArchiveDpTypes = emudcsArchive_defineDpTypes(dsArchiveDpTypes);
  emudcsArchive_showDebugInfo(bDebug,dsArchiveDpTypes);
  dyn_dyn_string dsDpes;
  dyn_dyn_int diDpes;
  int n;  
  for (int i=1;i<= dynlen(dsArchiveDpTypes);i++)
   {  
     if (dsArchiveDpTypes[i] != "CSC_BITS_DATA")     
       { //define a dp type
           dsDpes[1] = makeDynString (dsArchiveDpTypes[i],"");
           dsDpes[2] = makeDynString ("","value");

           diDpes[1] = makeDynInt (DPEL_STRUCT);
           diDpes[2] = makeDynInt (0,DPEL_FLOAT);
        // Create a dp type
           n = dpTypeCreate(dsDpes,diDpes);
           DebugN (dsArchiveDpTypes[i]+" type Created, result: ",n); 
       }
     else
       {   // Create an int dp type for CSC_BITS_DATA      
           dsDpes[1] = makeDynString (sDpTypeBITS,"");
           dsDpes[2] = makeDynString ("","value");

           diDpes[1] = makeDynInt (DPEL_STRUCT);
           diDpes[2] = makeDynInt (0,DPEL_INT);
           n = dpTypeCreate(dsDpes,diDpes);
           DebugN (sDpTypeBITS+" type Created, result: ",n);        
       }
   }     
}  
//------set configuration for all archiving dp,modify for new middle layer project------------------
void emudcsArchive_setAllDpConfigs()
{
   dyn_string dsDpName_HV,dsDpName_LV,dsDpName_TEMP,dsDpName_HVM,dsDpName_FED,dsDpName_MA,dsDpName_HVP; 
   int i, iLen_HV,iLen_LV,iLen_TEMP,iLen_HVM,iLen_FED,iLen_MA,iLen_HVP;
  
//------- get dp name for HV channels-------------------------------------   
   dsDpName_HV = emudcsArchive_getDpNames("*HighVoltage*","CscHvChamberFastMon");   
   iLen_HV = dynlen(dsDpName_HV);
   //DebugN(dsDpName_HV);
   if (iLen_HV == 0)
   {
     DebugTN("no dp at CscHvChamberFastMon,skip");
   }
   else
   {
     DebugTN("create archiving dps for HV");
     for (i=1;i<=iLen_HV;i++)
     {  
       emudcsArchive_showDebugInfo(bDebug,dsDpName_HV[i]);
       emudcsArchive_setHvDpConfig(dsDpName_HV[i]);  //set config for HV dp
     }
   }  
//--------- get dp name for LV-------------------------------------------------   
   dsDpName_LV = emudcsArchive_getDpNames("*LowVoltage*","CscLvChamberMon");
   iLen_LV = dynlen(dsDpName_LV);
   //DebugN(dsDpName_LV);
   if (iLen_LV == 0)
   {
     DebugTN("no dp at CscLvChamberMon, skip");
   }
   else
   {
     DebugTN("create archiving dps for LV");
     for (i=1;i<=iLen_LV;i++)
     {  
       emudcsArchive_showDebugInfo(bDebug,dsDpName_LV[i]);
       emudcsArchive_setLvDpConfig(dsDpName_LV[i]);  //set config for LV dp
     }
   }   
//--------- get dp for TEMP-------------------------------------------------------   
   dsDpName_TEMP = emudcsArchive_getDpNames("*Temperature*","CscTempChamberMon");
   iLen_TEMP = dynlen(dsDpName_TEMP);
   //DebugN(dsDpName_TEMP); 
   if (iLen_TEMP == 0)
   {
     DebugTN("no dp at CscTempChamberMon, skip");
   }
   else
   {
     DebugTN("create archiving dps for TEMP");
     for (i=1;i<=iLen_TEMP;i++)
     { 
       emudcsArchive_showDebugInfo(bDebug,dsDpName_TEMP[i]); 
       emudcsArchive_setTempDpConfig(dsDpName_TEMP[i]);  //set config for TEMP dp
     }
   }
        
//----------- get dp for HV master  -----------------------------------------------
//-----------no master dp in new middle layer--------------------------------------
/*   
   dsDpName_HVM = emudcsArchive_getDpNames("master*","HV_1_d");
   iLen_HVM = dynlen(dsDpName_HVM);
   //DebugN(dsDpName_HVM);
   if (iLen_HVM == 0)
   {
     DebugTN("no master dp at HV_1_d, skip");
   }
   else
   {
     DebugTN("create archiving dps for HV master");
     for (i=1;i<=iLen_HVM;i++)
     {  
       emudcsArchive_showDebugInfo(bDebug,dsDpName_HVM[i]);
       emudcsArchive_setHvmDpConfig(dsDpName_HVM[i]);  //set config for HVM dp
     }
   }   
*/ 
//--------------- get dp for HV primary ------------------------------------------  
   dsDpName_HVP = emudcsArchive_getDpNames("*Primary*","CscHvPrimaryMon");
   iLen_HVP = dynlen(dsDpName_HVP);
   //DebugN(dsDpName_HVP);
   if (iLen_HVP == 0)
   {
     DebugTN("no primary dp at CscHvPrimaryMon, skip");
   }
   else
   {
     DebugTN("create archiving dps for HV primary");
     for (i=1;i<=iLen_HVP;i++)
     {  
       emudcsArchive_showDebugInfo(bDebug,dsDpName_HVP[i]);
       emudcsArchive_setHvpDpConfig(dsDpName_HVP[i]);  //set config for HVP dp
     }
   }      
//------------ get dp for FED DDU ---------------------------------------------  
   dsDpName_FED = emudcsArchive_getDpNames("*DDU*","CscFedDduMon");
   iLen_FED = dynlen(dsDpName_FED);
   //DebugN(dsDpName_FED); 
   if (iLen_FED == 0)
   {
     DebugTN("no DDU dp at CscFedDduMon, skip");
   }
   else
   {
     DebugTN("create archiving dps for FED DDU");
     for (i=1;i<=iLen_FED;i++)
     { 
       emudcsArchive_showDebugInfo(bDebug,dsDpName_FED[i]); 
       emudcsArchive_setFedDpConfig(dsDpName_FED[i]);  //set config for FED DDU dp
     }
   }   
   
   //--------------- get dp for Wiener Maraton channel -----------------------------------  
   dsDpName_MA = emudcsArchive_getDpNames("*","FwWienerMarathonChannel");
   iLen_MA = dynlen(dsDpName_MA);
   //DebugN(dsDpName_MA); 
   if (iLen_MA == 0)
   {
     DebugN("no Maraton dp at FwWienerMarathonChannel, skip");
   }
   else
   {
     DebugTN("create archiving dps for Maraton channel");
     for (i=1;i<=iLen_MA;i++)
     {  
       emudcsArchive_showDebugInfo(bDebug,dsDpName_MA[i]);
       emudcsArchive_setMaDpConfig(dsDpName_MA[i]);  //set config for Maraton dp
       emudcsArchive_setMaStatusConfig(dsDpName_MA[i]); //set config for Maraton status
     }
   }   
   
   //---------------get dp  for FSM--------------------------------------------------
  dyn_string dsDpName_FSMdev = emudcsArchive_getDpNames("*","_FwFsmDevice"); 
  //DebugN(dsDpName_FSMdev); 
  int iLen_FSMdev = dynlen(dsDpName_FSMdev);
   if(iLen_FSMdev == 0)
  {
     DebugTN("no FSM devive found, skip");
  }
  else    
  {  
    DebugTN("config archiving dps for FSM devices");  
     for (i=1;i<=iLen_FSMdev;i++)
   {
    emudcsArchive_showDebugInfo(bDebug,dsDpName_FSMdev[i]);
    emudcsArchive_setFsmDpConfig(dsDpName_FSMdev[i]);
   }  
  } 
      
  dyn_string dsDpName_FSMobj = emudcsArchive_getDpNames("*","_FwFsmObject");
  //DebugN(dsDpName_FSMobj);
  int iLen_FSMobj = dynlen(dsDpName_FSMobj);
  if(iLen_FSMobj == 0)
  {
     DebugTN("no FSM object found, skip");
  }
  else
  {
    DebugTN("config archiving dps for FSM objects");
    for (i=1;i<=iLen_FSMobj;i++)
    {
      emudcsArchive_showDebugInfo(bDebug,dsDpName_FSMobj[i]);
      emudcsArchive_setFsmDpConfig(dsDpName_FSMobj[i]);
    }  
  }
  
  //--------------get dp for elmb ---------------------------------------------
  dyn_string dsDpName_FwElmbAi = emudcsArchive_getDpNames("*","FwElmbAi"); 
  //DebugN(dsDpName_FwElmbAi); 
  int iLen_FwElmbAi = dynlen(dsDpName_FwElmbAi);
  if(iLen_FwElmbAi == 0)
  {
     DebugTN("no FwElmbA1 found, skip");
  } 
  else
  {
    DebugTN("config archiving dps for FwElmbAi");
    for(i=1;i<=iLen_FwElmbAi;i++)
    {
      emudcsArchive_showDebugInfo(bDebug,dsDpName_FwElmbAi[i]);
      emudcsArchive_setDpSmoothing(dsDpName_FwElmbAi[i],fTolElmbAi,iTimeElmbAi);
    }    
  }       

   //---------------get dp for cooling ---------------------------------------------------
  dyn_string dsDpName_Cooling = emudcsArchive_getDpNames("*","fwCooling_CSC_TURBINES_data"); 
  //DebugN(dsDpName_Cooling); 
  int iLen_Cooling = dynlen(dsDpName_Cooling);
  if(iLen_Cooling == 0)
  {
     DebugTN("no Cooling dp found, skip");
  } 
  else
  {
    DebugTN("config archiving dps for CSC Colling");
    for(i=1;i<=iLen_Cooling;i++)
    {
      emudcsArchive_showDebugInfo(bDebug,dsDpName_Cooling[i]);
      emudcsArchive_setDpSmoothing(dsDpName_Cooling[i],fTolCooling,iTimeCooling);
    }    
  }          
   DebugTN("all archiving dps configed");   
}   
//-------------- get dp name-----------------------------------------------------
dyn_string emudcsArchive_getDpNames(string sDpPatten,string sDpType)
{
   dyn_string dsDpNames; 
   dsDpNames = dpNames(sDpPatten,sDpType);
   return dsDpNames;
}          
//--------HV archiving dp configuration,modify for new middle layer project---------- 
void emudcsArchive_setHvDpConfig(string sDpName)
{  
// create HV archiving dp and fct 
   string s = dpSubStr(sDpName,DPSUB_DP); 
   dyn_string ds = strsplit(s,"/");
   string sCscChamber = ds[2];  //like "CSC_ME_P12_C01"
//get HV channel numbers   
   int iChannelNumbers = emudcsArchive_getHvChannelNumbers(sCscChamber);
   for (int i=1;i<=iChannelNumbers;i++)   
         {
           emudcsArchive_createHvDps(sCscChamber,i);               //create dp for HV
           emudcsArchive_setHvDpFunction(sDpName,sCscChamber,i);   //create dp fct for HV
         }
// create chamber master channel dps and fct
           emudcsArchive_createHvMaChDps(sCscChamber);  
           emudcsArchive_setHvMaChDpFunction(sDpName,sCscChamber); 
}
//---LV archiving dp configuration,modify for new middle layer project -----------------
void emudcsArchive_setLvDpConfig(string sDpName)
{ 
    dyn_string ds1 = strsplit(sDpName,":");
    dyn_string ds2 = strsplit(ds1[2],"/");
    string sCscChamber = ds2[2];  //like "CSC_ME_P12_C01" 
    int iCfebNumbers = emudcsArchive_getCfebNumbers(sCscChamber);  
       for (int i=1;i<=iCfebNumbers;i++)  
         {
            emudcsArchive_createLvCfebDps(sCscChamber,i);              //create LV dp for cfeb
            emudcsArchive_setLvCfebDpFunction(sDpName,sCscChamber,i);  //create LV dp fct for cfeb
         } 
            emudcsArchive_createLvAlctDps(sCscChamber);                //create LV dp for alct
            emudcsArchive_setLvAlctDpFunction(sDpName,sCscChamber);    //create LV dp fct for alct
//new data from Tmb            
            emudcsArchive_createLvTmbDps(sCscChamber);                 //create LV dp for Tmb
            emudcsArchive_setLvTmbDpFunction(sDpName,sCscChamber);     //create LV dp fct for Tmb
//new data from LVDB
            emudcsArchive_createLvLvdbDps(sCscChamber);                //create LV dp for Lvdb
            emudcsArchive_setLvLvdbDpFunction(sDpName,sCscChamber);    //create LV dp fct for Lvdb
//new data from CCB/FPFA bits  
            emudcsArchive_createLvBitsDps(sCscChamber);                //create LV dp for Bits 
            emudcsArchive_setLvBitsDpFunction(sDpName,sCscChamber);    //create LV dp fct for Bits   
//monitor LV status      
            emudcsArchive_setDpSmoothingBool(sDpName+".status");       //set archiving for status at LV mon      
}
//---Temp archiving dp configuration,modify for new middle layer project-------------------------------
void emudcsArchive_setTempDpConfig(string sDpName)
{ 
    dyn_string ds1 = strsplit(sDpName,":");
    dyn_string ds2 = strsplit(ds1[2],"/");
    string sCscChamber = ds2[2];  //like "CSC_ME_P12_C01" 
    int iCfebNumbers = emudcsArchive_getCfebNumbers(sCscChamber);  
       for (int i=1;i<=iCfebNumbers;i++)  
         {
            emudcsArchive_createTempCfebDps(sCscChamber,i);             //create TEMP dp for cfeb
            emudcsArchive_setTempCfebDpFunction(sDpName,sCscChamber,i); //create TEMP dp fct for cfeb
         } 
            emudcsArchive_createTempAlctDmbDps(sCscChamber);             //create TEMP dp for alct/dmb
            emudcsArchive_setTempAlctDmbDpFunction(sDpName,sCscChamber); //create TEMP dp fct for alct/dmb
}            
//---HV Master archiving dp configuration (8 HV channels for each),no need--------- 
void emudcsArchive_setHvmDpConfig(string sDpName)
{     
    dyn_string ds1,ds2;
    string sHVMaster,sHVMasterNew;
    ds1 = strsplit(sDpName,":");
    sHVMaster = ds1[2];
    ds2 = strsplit(ds1[2],"_");
    sHVMasterNew = ds2[1]+"_0"+ds2[2]+"_"+ds2[3];  //like: master600_00_0 
        for (int i=1;i<=8;i++)  //create dp for HV master channel
         {
           emudcsArchive_createHvmDps(sHVMasterNew,i); 
           emudcsArchive_setHvmDpFunction(sHVMaster,sHVMasterNew,i);  
         }       
} 
//----HV primary archiving dp configuration,modify for new middle layer project --- 
void emudcsArchive_setHvpDpConfig(string sDpName)
{     
    dyn_string ds1,ds2;
    ds1 = strsplit(sDpName,":");
    ds2 = strsplit(ds1[2],"/");
    string sHVPrimary = ds2[2];  //like: primary500_1_1 
    //DebugTN("HV primary: "+sHVPrimary);                
           emudcsArchive_createHvpDps(sHVPrimary); 
           emudcsArchive_setHvpDpFunction(sDpName,sHVPrimary);                  
} 
//---------FED archiving dp configuration,modify for new middle layer project-----
void emudcsArchive_setFedDpConfig(string sDpName)
{   
   dyn_string ds1,ds2;
   ds1 = strsplit(sDpName,":");
   ds2 = strsplit(ds1[2],"/");
   string sDpNameFed = "CSC_FED_"+ds2[2];  //like: CSC_FED_DDU01
   //DebugTN("FED DDU: "+sDpName);
   //DebugTN(sDpNameFed);     
       emudcsArchive_createFedDps(sDpNameFed);              //create FED DDU dp
       emudcsArchive_setFedDpFunction(sDpName,sDpNameFed);  // create FED DDU fct  
}
//-----------------Maraton archiving dp configuration ----------------------------
void emudcsArchive_setMaDpConfig(string sChannel)
{      
        dyn_string ds1 = strsplit(sChannel,":");
        dyn_string ds2 = strsplit(ds1[2],"/");
        int iCrateNumber = strltrim(ds2[3],"Crate");
        string sCrateNew,sChannelNew,sChannelOld;
         if ( iCrateNumber < 10)
           sCrateNew = "Crate0"+iCrateNumber;
         else if (iCrateNumber > 9)
           sCrateNew = "Crate"+iCrateNumber; 
           sChannelNew = ds2[1]+"_"+sCrateNew+"_"+ds2[4];
           sChannelOld = ds1[2];
           emudcsArchive_createMaDps(sChannelNew,sChannelOld);
} 
//---------------FSM device dps configuration ------------------------------------
void emudcsArchive_setFsmDpConfig(string sDpName)
{
  dyn_string ds = makeDynString( sDpName + ".fsm.currentState",
                                 sDpName + ".fsm.sendCommand",
                                 sDpName + ".fsm.executingAction",
                                 sDpName + ".fsm.currentParameters");
  for (int i=1; i<=dynlen(ds);i++)
  {
    emudcsArchive_setDpSmoothingBool(ds[i]);
  }
}
//-----------HV archiving dp create------------------------------------------------
void emudcsArchive_createHvDps(string sCscChamber,int i)
{         
         string sChannel;
         if (i < 10)
           sChannel = "_V0";
         else if (i > 9)
           sChannel = "_V"; 
         string sDpNameHV_V = sCscChamber+"_HV"+sChannel+i+"_VMON";
           emudcsArchive_createDp(sDpNameHV_V,"HV_V");
           emudcsArchive_setDpSmoothing(sDpNameHV_V,fTolHVV,iTimeHV);         
         string sDpNameHV_I = sCscChamber+"_HV"+sChannel+i+"_IMON";
           emudcsArchive_createDp(sDpNameHV_I,"HV_I");
           emudcsArchive_setDpSmoothing(sDpNameHV_I,fTolHVI,iTimeHV);           
} 
//----HV mastr channel archiving dp create,add for new middle layer project----------
void emudcsArchive_createHvMaChDps(string sCscChamber)
{         
         string sDpNameHV_MaChV = sCscChamber+"_HV_Mch_VMON";
           emudcsArchive_createDp(sDpNameHV_MaChV,"HV_V");
           emudcsArchive_setDpSmoothing(sDpNameHV_MaChV,fTolHVV,iTimeHV);         
         string sDpNameHV_MaChI = sCscChamber+"_HV_Mch_IMON";
           emudcsArchive_createDp(sDpNameHV_MaChI,"HV_I");
           emudcsArchive_setDpSmoothing(sDpNameHV_MaChI,fTolHVI,iTimeHV);           
} 
//--------HV master archiving dp create,not exist at new middle layer project -------
void emudcsArchive_createHvmDps(string sHVMasterNew,int i)
{                          
         string sDpNameHV_MV = sHVMasterNew+"_V"+i+"_VMON";
         string sDpNameHV_MI = sHVMasterNew+"_V"+i+"_IMON";         
         emudcsArchive_createDp(sDpNameHV_MV,"HV_M");
         emudcsArchive_setDpSmoothing(sDpNameHV_MV,fTolHVMV,iTimeHVM);        
         emudcsArchive_createDp(sDpNameHV_MI,"HV_M");
         emudcsArchive_setDpSmoothing(sDpNameHV_MI,fTolHVMI,iTimeHVM);
}
//---------HV primary archiving dp create,modify for new middle layer project -------
void emudcsArchive_createHvpDps(string sHVPrimary)
{                         
         string sDpNameHV_PV = sHVPrimary+"_VMON";
         string sDpNameHV_PI = sHVPrimary+"_IMON";         
         emudcsArchive_createDp(sDpNameHV_PV,"HV_P");
         emudcsArchive_setDpSmoothing(sDpNameHV_PV,fTolHVPV,iTimeHVP);        
         emudcsArchive_createDp(sDpNameHV_PI,"HV_P");
         emudcsArchive_setDpSmoothing(sDpNameHV_PI,fTolHVPI,iTimeHVP);
}                   
//-------------- LV cfeb archiving dp create------------------------------------
void emudcsArchive_createLvCfebDps(string sCscChamber,int i)
{      
      dyn_string dsV = makeDynString ("_LV_Cfeb33_V","_LV_Cfeb50_V","_LV_Cfeb60_V");
      dyn_string dsI = makeDynString ("_LV_Cfeb33_C","_LV_Cfeb50_C","_LV_Cfeb60_C");
      for (int k=1;k<=dynlen(dsV);k++)
      {
        emudcsArchive_createDp(sCscChamber+dsV[k]+i,"LV_V");
        emudcsArchive_setDpSmoothing(sCscChamber+dsV[k]+i,fTolLV,iTimeLV);
      }   
      for (int k=1;k<=dynlen(dsI);k++)
      {
        emudcsArchive_createDp(sCscChamber+dsI[k]+i,"LV_I");
        emudcsArchive_setDpSmoothing(sCscChamber+dsI[k]+i,fTolLV,iTimeLV);
      }       
}    
//--------------- LV alct archiving dp create---------------------------------
void emudcsArchive_createLvAlctDps(string sCscChamber) 
{
      dyn_string dsV = makeDynString ("_LV_Alct18_V1","_LV_Alct33_V1","_LV_Alct55_V1","_LV_Alct56_V1");
      dyn_string dsI = makeDynString ("_LV_Alct18_C1","_LV_Alct33_C1","_LV_Alct55_C1","_LV_Alct56_C1");
      for (int i=1;i<=dynlen(dsV);i++)
      {
        emudcsArchive_createDp(sCscChamber+dsV[i],"LV_V");
        emudcsArchive_setDpSmoothing(sCscChamber+dsV[i],fTolLV,iTimeLV);
      }   
      for (int i=1;i<=dynlen(dsI);i++)
      {
        emudcsArchive_createDp(sCscChamber+dsI[i],"LV_I");
        emudcsArchive_setDpSmoothing(sCscChamber+dsI[i],fTolLV,iTimeLV);
      }           
} 
//---------------- LV lvdb archiving dp create----------------------------------
void emudcsArchive_createLvLvdbDps(string sCscChamber)
{
     dyn_string dsV = makeDynString ("_LV_Lvdb_v7Analog","_LV_Lvdb_v7Digital");
     for (int i=1;i<=dynlen(dsV);i++)
      {
        emudcsArchive_createDp(sCscChamber+dsV[i],"LVDB");
        emudcsArchive_setDpSmoothing(sCscChamber+dsV[i],fTolLvdb,iTimeLV);
      }   
} 
//------------------- LV Bits archiving dp create--------------------------------
void emudcsArchive_createLvBitsDps(string sCscChamber)
{
     dyn_string dsV = makeDynString ("_LV_CCB_bits","_LV_FPGA_bits");
     for (int i=1;i<=dynlen(dsV);i++)
      {
        emudcsArchive_createDp(sCscChamber+dsV[i],"BITS");
        emudcsArchive_setDpSmoothingBool(sCscChamber+dsV[i]+".value");
      }   
}  
//-------------- LV Tmb archiving dp create---------------------------------------
void emudcsArchive_createLvTmbDps(string sCscChamber) 
{

      dyn_string dsTmb = makeDynString ("_LV_Tmb_v50","_LV_Tmb_v33","_LV_Tmb_v15C","_LV_Tmb_v15T","_LV_Tmb_v10T",
                                        "_LV_Tmb_c50","_LV_Tmb_c33","_LV_Tmb_c15C","_LV_Tmb_c15T","_LV_Tmb_cRAT",
                                        "_LV_Tmb_vRAT","_LV_Tmb_vREF","_LV_Tmb_vGND","_LV_Tmb_vMAX");
      for (int i=1;i<=dynlen(dsTmb);i++)
      {
        emudcsArchive_createDp(sCscChamber+dsTmb[i],"TMB");
        emudcsArchive_setDpSmoothing(sCscChamber+dsTmb[i],fTolLV,iTimeLV);
      }            
} 
//-----------------temp Alct and Dmb archiving dp create--------------------------
void emudcsArchive_createTempAlctDmbDps(string sCscChamber)
{     
      string sDpNameTEMP_Alct = sCscChamber+"_TEMP_Alct_T1";
      string sDpNameTEMP_Dmb  = sCscChamber+"_TEMP_Dmb__T1"; 
      emudcsArchive_createDp(sDpNameTEMP_Alct,"TEMP");
      emudcsArchive_setDpSmoothing(sDpNameTEMP_Alct,fTolTEMP,iTimeTEMP);      
      emudcsArchive_createDp(sDpNameTEMP_Dmb,"TEMP");
      emudcsArchive_setDpSmoothing(sDpNameTEMP_Dmb,fTolTEMP,iTimeTEMP);
}
//------------------temp Cfeb archiving dp create--------------------------------  
void emudcsArchive_createTempCfebDps(string sCscChamber,int i)
{              
        string sDpNameTEMP_Cfeb = sCscChamber+"_TEMP_Cfeb_T"+i;
        emudcsArchive_createDp(sDpNameTEMP_Cfeb,"TEMP");
        emudcsArchive_setDpSmoothing(sDpNameTEMP_Cfeb,fTolTEMP,iTimeTEMP);
}        
//-----------------------FED_DDU archiving dp create-----------------------------
void emudcsArchive_createFedDps(string sDDU) 
{
      dyn_string dsV = makeDynString ("_V15A","_V25A","_V25B","_V33A");
      dyn_string dsT = makeDynString ("_TD1","_TD2","_TD3","_TD4");
      for (int i=1;i<=dynlen(dsV);i++)
      {
        emudcsArchive_createDp(sDDU+dsV[i],"FED_V");
        emudcsArchive_setDpSmoothing(sDDU+dsV[i],fTolFEDV,iTimeFED);
        
        emudcsArchive_createDp(sDDU+dsT[i],"FED_T");
        emudcsArchive_setDpSmoothing(sDDU+dsT[i],fTolFEDT,iTimeFED);
      }              
}   
//-------------------create maraton dps-------------------------------------------
void emudcsArchive_createMaDps(string sChannelNew,string sChannelOld)
{
        dyn_string ds1 = makeDynString("Voltage","Current","Temperature");
        dyn_string ds2 = makeDynString("V","I","T");
        for(int i=1;i<=dynlen(ds1);i++)
        {
          string sChannel = sChannelNew+"_"+ds1[i];
          emudcsArchive_showDebugInfo(bDebug,sChannel);
          emudcsArchive_createDp(sChannel,"MARATON_"+ds2[i]);
          emudcsArchive_setDpSmoothing(sChannel,fTolMA,iTimeMA);
          emudcsArchive_setMaDpFunction(sChannel,sChannelOld,ds1[i]);
        }  
}      
//-----------set archiving dp smoothing config(float)-----------------------------  
void emudcsArchive_setDpSmoothing(string sDpName,float fTol,int iTime)
{
    dpSetWait (sDpName + ".value:_archive.._type",45,
               sDpName + ".value:_archive.._archive",1,
               sDpName + ".value:_archive.1._type",3,
               sDpName + ".value:_archive.1._class","_EVENT",
               sDpName + ".value:_archive.1._interv",0,
               sDpName + ".value:_archive.1._interv_type",0,
               sDpName + ".value:_archive.1._std_type",2,
               sDpName + ".value:_archive.1._std_tol",fTol,
               sDpName + ".value:_archive.1._std_time",iTime);       
}
//----------set smoothing for int------------------------------------------------
void emudcsArchive_setDpSmoothingInt(string sDpName,int iTol,int iTime)
{
    dpSetWait (sDpName + ":_archive.._type",45,
               sDpName + ":_archive.._archive",1,
               sDpName + ":_archive.1._type",3,
               sDpName + ":_archive.1._class","_EVENT",
               sDpName + ":_archive.1._interv",0,
               sDpName + ":_archive.1._interv_type",0,
               sDpName + ":_archive.1._std_type",2,
               sDpName + ":_archive.1._std_tol",iTol,
               sDpName + ":_archive.1._std_time",iTime);       
} 
//------HV dp_fct setup,modify for new middle layer project-----------------------
void emudcsArchive_setHvDpFunction(string sDpName,string sCscChamber,int i)
{  
   string sChannel;
   if (i < 10)
          sChannel = "_V0";
   else if (i > 9)
          sChannel = "_V";   
   string sDpNameImon,sDpNameVmon,p1;  
   p1=sDpName+".channels.ch"+i+".imon:_original.._value";
   sDpNameImon = sCscChamber+"_HV"+sChannel+i+"_IMON";
    emudcsArchive_setDpFunction(sDpNameImon,p1);
       
   p1=sDpName+".channels.ch"+i+".vmon:_original.._value";
   sDpNameVmon = sCscChamber+"_HV"+sChannel+i+"_VMON";
    emudcsArchive_setDpFunction(sDpNameVmon,p1);   
}
//------HV master channel dp_fct setup,add for new middle layer project----------
void emudcsArchive_setHvMaChDpFunction(string sDpName,string sCscChamber)
{  
   string sDpNameImon,sDpNameVmon,p1;  
   p1=sDpName+".master_channel.imon:_original.._value";
   sDpNameImon = sCscChamber+"_HV_Mch_IMON";
    emudcsArchive_setDpFunction(sDpNameImon,p1);
       
   p1=sDpName+".master_channel.vmon:_original.._value";
   sDpNameVmon = sCscChamber+"_HV_Mch_VMON";
    emudcsArchive_setDpFunction(sDpNameVmon,p1);   
}
//---------HV Master dp_fct setup,no exist for new middle layer project ----------
void emudcsArchive_setHvmDpFunction(string sHVMaster,string sHVMasterNew,int i)
{  
   string sDpNameNew,sDpNameLink;
   sDpNameNew = sHVMasterNew+"_V"+i+"_IMON";
   sDpNameLink =sHVMaster+".data.v"+i+".imon:_original.._value";
    emudcsArchive_setDpFunction(sDpNameNew,sDpNameLink);
    
   sDpNameNew = sHVMasterNew+"_V"+i+"_VMON";
   sDpNameLink = sHVMaster+".data.v"+i+".vmon:_original.._value";
    emudcsArchive_setDpFunction(sDpNameNew,sDpNameLink);   
 }
//---------HV primary dp_fct setup,modify for new middle layer project ----------
void emudcsArchive_setHvpDpFunction(string sDpName,string sHVPrimary)
{  
   string sDpNameNew,sDpNameLink;
   sDpNameNew = sHVPrimary+"_IMON";
   sDpNameLink =sDpName+".imon:_original.._value";
    emudcsArchive_setDpFunction(sDpNameNew,sDpNameLink);    
   sDpNameNew = sHVPrimary+"_VMON";
   sDpNameLink = sDpName+".vmon:_original.._value";
    emudcsArchive_setDpFunction(sDpNameNew,sDpNameLink);   
 }
//------LV cfeb dp_fct setup,modify for new middle layer project-----------------
void emudcsArchive_setLvCfebDpFunction(string sDpName,string sCscChamber,int i)
{
   dyn_string dsNew = makeDynString(sCscChamber+"_LV_Cfeb33_C"+i,
                                    sCscChamber+"_LV_Cfeb33_V"+i,
                                    sCscChamber+"_LV_Cfeb50_C"+i,
                                    sCscChamber+"_LV_Cfeb50_V"+i,
                                    sCscChamber+"_LV_Cfeb60_C"+i,
                                    sCscChamber+"_LV_Cfeb60_V"+i);
   
   dyn_string dsOld = makeDynString(sDpName+".cfeb.c33.v"+i+":_original.._value",
                                    sDpName+".cfeb.v33.v"+i+":_original.._value",
                                    sDpName+".cfeb.c50.v"+i+":_original.._value",
                                    sDpName+".cfeb.v50.v"+i+":_original.._value",
                                    sDpName+".cfeb.c60.v"+i+":_original.._value",
                                    sDpName+".cfeb.v60.v"+i+":_original.._value");
   for(int k=1;k<=dynlen(dsNew);k++)
   {
     emudcsArchive_setDpFunction(dsNew[k],dsOld[k]);
   }  
}
//----- LV lvdb dp_fct setup,modify for new middle layer project--------------------
void emudcsArchive_setLvLvdbDpFunction(string sDpName,string sCscChamber)
{
   dyn_string dsNew = makeDynString(sCscChamber+"_LV_Lvdb_v7Analog",
                                    sCscChamber+"_LV_Lvdb_v7Digital");
   dyn_string dsOld = makeDynString(sDpName+".lvdb.v7Analog:_original.._value",
                                    sDpName+".lvdb.v7Digital:_original.._value");
   for(int k=1;k<=dynlen(dsNew);k++)
   {
     emudcsArchive_setDpFunction(dsNew[k],dsOld[k]);
   }  
}
//----- LV bits dp_fct setup,modify for new middle layer project--------------------
void emudcsArchive_setLvBitsDpFunction(string sDpName,string sCscChamber)
{
   dyn_string dsNew = makeDynString(sCscChamber+"_LV_CCB_bits",
                                    sCscChamber+"_LV_FPGA_bits");
   dyn_string dsOld = makeDynString(sDpName+".ccb_bits:_original.._value",
                                    sDpName+".fpga_bits:_original.._value");
   for(int k=1;k<=dynlen(dsNew);k++)
   {
     emudcsArchive_setDpFunction(dsNew[k],dsOld[k]);
   }  
}  
//------LV Tmb dp_fct setup,modify for new middle layer project----------------------
void emudcsArchive_setLvTmbDpFunction(string sDpName,string sCscChamber)
{ 
   dyn_string dsNew = makeDynString(sCscChamber+"_LV_Tmb_v50",
                                    sCscChamber+"_LV_Tmb_v33",
                                    sCscChamber+"_LV_Tmb_v15C",
                                    sCscChamber+"_LV_Tmb_v15T",
                                    sCscChamber+"_LV_Tmb_v10T",
                                    sCscChamber+"_LV_Tmb_c50",
                                    sCscChamber+"_LV_Tmb_c33",
                                    sCscChamber+"_LV_Tmb_c15C",
                                    sCscChamber+"_LV_Tmb_c15T",
                                    sCscChamber+"_LV_Tmb_cRAT",
                                    sCscChamber+"_LV_Tmb_vRAT",
                                    sCscChamber+"_LV_Tmb_vREF",
                                    sCscChamber+"_LV_Tmb_vGND",
                                    sCscChamber+"_LV_Tmb_vMAX");
   
   dyn_string dsOld = makeDynString(sDpName+".tmb.v50:_original.._value",
                                    sDpName+".tmb.v33:_original.._value",
                                    sDpName+".tmb.v15C:_original.._value",
                                    sDpName+".tmb.v15T:_original.._value",
                                    sDpName+".tmb.v10T:_original.._value",
                                    sDpName+".tmb.c50:_original.._value",
                                    sDpName+".tmb.c33:_original.._value",
                                    sDpName+".tmb.c15C:_original.._value",
                                    sDpName+".tmb.c15T:_original.._value",
                                    sDpName+".tmb.cRAT:_original.._value",
                                    sDpName+".tmb.vRAT:_original.._value",
                                    sDpName+".tmb.vREF:_original.._value",
                                    sDpName+".tmb.vGND:_original.._value",
                                    sDpName+".tmb.vMAX:_original.._value");
   for(int k=1;k<=dynlen(dsNew);k++)
   {
     emudcsArchive_setDpFunction(dsNew[k],dsOld[k]);
   }     
}  
//------LV Alct dp_fct setup,modify for new middle layer project----------------------------
void emudcsArchive_setLvAlctDpFunction(string sDpName,string sCscChamber)
{ 
   dyn_string dsNew = makeDynString(sCscChamber+"_LV_Alct18_C1",
                                    sCscChamber+"_LV_Alct18_V1",
                                    sCscChamber+"_LV_Alct33_C1",
                                    sCscChamber+"_LV_Alct33_V1",
                                    sCscChamber+"_LV_Alct55_C1",
                                    sCscChamber+"_LV_Alct55_V1",
                                    sCscChamber+"_LV_Alct56_C1",
                                    sCscChamber+"_LV_Alct56_V1");
   
   dyn_string dsOld = makeDynString(sDpName+".alct.c18:_original.._value",
                                    sDpName+".alct.v18:_original.._value",
                                    sDpName+".alct.c33:_original.._value",
                                    sDpName+".alct.v33:_original.._value",
                                    sDpName+".alct.c55:_original.._value",
                                    sDpName+".alct.v55:_original.._value",
                                    sDpName+".alct.c56:_original.._value",
                                    sDpName+".alct.v56:_original.._value");
   for(int k=1;k<=dynlen(dsNew);k++)
   {
     emudcsArchive_setDpFunction(dsNew[k],dsOld[k]);
   }     
}  
//------Temp cfeb dp_fct setup,modify for new middle layer project----------------------
void emudcsArchive_setTempCfebDpFunction(string sDpName,string sCscChamber,int i)
{
   string sDpNameLink =sDpName+".cfeb.v"+i+":_original.._value";
   string sDpNameCfeb = sCscChamber+"_TEMP_Cfeb_T"+i;
    emudcsArchive_setDpFunction(sDpNameCfeb,sDpNameLink); //dpfct for cfeb
}
//------Temp alct and dmb dp_fct setup,modify for new middle layer project----------------  
void emudcsArchive_setTempAlctDmbDpFunction(string sDpName,string sCscChamber) 
{   
   string sDpNameAlct,sDpNameDmb,sDpNameLink;  
   sDpNameLink =sDpName+".alct.v1:_original.._value";
   sDpNameAlct = sCscChamber+"_TEMP_Alct_T1";
     emudcsArchive_setDpFunction(sDpNameAlct,sDpNameLink);  //dpfct for alct      
   sDpNameLink = sDpName+".dmb.v1:_original.._value";
   sDpNameDmb = sCscChamber+"_TEMP_Dmb__T1";
     emudcsArchive_setDpFunction(sDpNameDmb,sDpNameLink);  //dpfct for dmb        
}      
//------FED DDU dp_fct setup,modify for new middle layer project---------------------------
void emudcsArchive_setFedDpFunction(string sDpName,string sDDU)
{   
   dyn_string dsNew,dsOld; 
   dsOld = makeDynString( sDpName+".lv.v15:_original.._value",
                          sDpName+".lv.v25a:_original.._value",
                          sDpName+".lv.v25b:_original.._value",
                          sDpName+".lv.v33:_original.._value",
                          sDpName+".temp.v1:_original.._value",
                          sDpName+".temp.v2:_original.._value",
                          sDpName+".temp.v3:_original.._value",
                          sDpName+".temp.v4:_original.._value"); //td1-4 change to v1-4
   dsNew = makeDynString( sDDU+"_V15A",sDDU+"_V25A",sDDU+"_V25B",sDDU+"_V33A",
                          sDDU+"_TD1",  sDDU+"_TD2", sDDU+"_TD3", sDDU+"_TD4");
   for(int i=1;i<=dynlen(dsNew);i++)
   {
     emudcsArchive_setDpFunction(dsNew[i],dsOld[i]);
   }                             
}
//-------set dpfct for marathon-------------------------------------------------------------
void emudcsArchive_setMaDpFunction(string sDpNameNew,string sDpName,string sType)
{
   string sDpNameLink; 
   if (sType == "Voltage")
       {     
          sDpNameLink = sDpName+".MeasurementSenseVoltage:_original.._value";
       }
   if (sType == "Current")
       {     
          sDpNameLink = sDpName+".MeasurementCurrent:_original.._value";
       } 
   if (sType == "Temperature")
       {     
          sDpNameLink = sDpName+".MeasurementTemperature:_original.._value";
       }      
   emudcsArchive_setDpFunction(sDpNameNew,sDpNameLink);   
}
//----------set marathon status archiving-----------------------------------------------------
void emudcsArchive_setMaStatusConfig(string sDpName)
{
  dyn_string ds = makeDynString( sDpName + ".Status.FailureMaxTemperature",
                                 sDpName + ".Status.FailureMaxCurrent",
                                 sDpName + ".Status.FailureMaxSenseVoltage",
                                 sDpName + ".Status.FailureMaxTerminalVoltage",
                                 sDpName + ".Status.FailureMinSenseVoltage");
  for (int i=1; i<=dynlen(ds);i++)
  {
    emudcsArchive_setDpSmoothingBool(ds[i]);
  }
}
//-------------set dpfct-----------------------------------------------------------------------  
void emudcsArchive_setDpFunction(string sDpName,string sDpNameLink)
{ 
   string p1 =  sDpNameLink;
   dyn_string ds=makeDynString(p1);   
   dpSetWait(sDpName+".value:_dp_fct.._type", 60, 
             sDpName+".value:_dp_fct.._param", ds, 
             sDpName+".value:_dp_fct.._fct", "p1"); 
} 
//-------------create dps-----------------------------------------------------------------------
void emudcsArchive_createDp(string sDpName,string sType)
{
  string sDpType = "CSC_"+sType+"_DATA";
  dpCreate(sDpName,sDpType);
} 
//-------------dptypes for archiving------------------------------------------------------------      
dyn_string emudcsArchive_defineDpTypes(dyn_string &dsArchiveDpTypes)
{
   //not include sDpTypeBITS
   dyn_string dsArchiveDpType = makeDynString(sDpTypeHVV, sDpTypeHVI, sDpTypeLVV, sDpTypeLVI,
                                              sDpTypeHVM,sDpTypeTEMP,sDpTypeFEDV,sDpTypeFEDT,
                                              sDpTypeMAV,sDpTypeMAI,sDpTypeMAT,sDpTypeHVP,sDpTypeTMB,
                                              sDpTypeLVDB,sDpTypeBITS);
   return dsArchiveDpType;   
}       
//----- old/new comparison----------------------------------------------------------------------
void emudcsArchive_setDpSmoothingBool(string sDpName)
{
    dpSetWait (sDpName + ":_archive.._type",45,
               sDpName + ":_archive.._archive",1,
               sDpName + ":_archive.1._type",3,
               sDpName + ":_archive.1._class","_EVENT",
               sDpName + ":_archive.1._std_type",4); //  old/new comparison
}
//---recreating dpfct and setting archiving for FSM...,modify for new middle layer project------
void emudcsArchive_recreateAllDpFct_Arch()
{
   DebugTN("recreate dp_fct/arch...");
   dyn_string dsDpName_HV,dsDpName_LV,dsDpName_TEMP,dsDpName_HVM,dsDpName_FED,dsDpName_MA,dsDpName_HVP; 
   int i, iLen_HV,iLen_LV,iLen_TEMP,iLen_HVM,iLen_FED,iLen_MA,iLen_HVP;
  
// get dp name for HV      
   dsDpName_HV = emudcsArchive_getDpNames("*HighVoltage*","CscHvChamberFastMon");   
   iLen_HV = dynlen(dsDpName_HV);
   //DebugN(dsDpName_HV);
   if (iLen_HV == 0)
   {
     DebugTN("no dp at CscHvChamberFastMon ,skip");
   }
   else
   {
     DebugTN("recreate dp_fct for HV");
     for (i=1;i<=iLen_HV;i++)
     {  
       emudcsArchive_showDebugInfo(bDebug,dsDpName_HV[i]);
       emudcsArchive_recreateHvDpFct(dsDpName_HV[i]);  //recreate HV dp_fct
     }
   } 
  
// get dp name for LV   
   dsDpName_LV = emudcsArchive_getDpNames("*LowVoltage*","CscLvChamberMon");
   iLen_LV = dynlen(dsDpName_LV);
   //DebugN(dsDpName_LV);
   if (iLen_LV == 0)
   {
     DebugTN("no dp at CscLvChamberMon, skip");
   }
   else
   {
     DebugTN("recreate dp_fct for LV");
     for (i=1;i<=iLen_LV;i++)
     {  
       emudcsArchive_showDebugInfo(bDebug,dsDpName_LV[i]);
       emudcsArchive_recreateLvDpFct(dsDpName_LV[i]);  //recreate LV dp_fct
     }
   } 
   
// get dp for TEMP   
   dsDpName_TEMP = emudcsArchive_getDpNames("*Temperature*","CscTempChamberMon");
   iLen_TEMP = dynlen(dsDpName_TEMP);
   //DebugN(dsDpName_TEMP); 
   if (iLen_TEMP == 0)
   {
     DebugTN("no dp at CscTempChamberMon, skip");
   }
   else
   {
     DebugTN("recreate dp_fct for TEMP");
     for (i=1;i<=iLen_TEMP;i++)
     { 
       emudcsArchive_showDebugInfo(bDebug,dsDpName_TEMP[i]); 
       emudcsArchive_recreateTempDpFct(dsDpName_TEMP[i]);  //recreate Temp dp_fct
     }
   }
        
// get dp for HV master
/*   
   dsDpName_HVM = emudcsArchive_getDpNames("*","????");
   iLen_HVM = dynlen(dsDpName_HVM);
   //DebugN(dsDpName_HVM);
   if (iLen_HVM == 0)
   {
     DebugTN("no master dp at ????, skip");//??????
   }
   else
   {
     DebugTN("recreate dp_fct for HV master");
     for (i=1;i<=iLen_HVM;i++)
     {  
       emudcsArchive_showDebugInfo(bDebug,dsDpName_HVM[i]);
       emudcsArchive_recreateHvmDpFct(dsDpName_HVM[i]);  //recreate HVM dp_fct
     }
   }  
*/ 
// get dp for HV primary   
   dsDpName_HVP = emudcsArchive_getDpNames("*Primary*","CscHvPrimaryMon");
   iLen_HVP = dynlen(dsDpName_HVP);
   //DebugN(dsDpName_HVP);
   if (iLen_HVP == 0)
   {
     DebugTN("no primary dp at CscHvPrimaryMon, skip");
   }
   else
   {
     DebugTN("create archiving dps for HV primary");
     for (i=1;i<=iLen_HVP;i++)
     {  
       emudcsArchive_showDebugInfo(bDebug,dsDpName_HVP[i]);
       emudcsArchive_recreateHvpDpFct(dsDpName_HVP[i]);  //set config for HVP dp
     }
   }         
           
// get dp for FED DDU   
   dsDpName_FED = emudcsArchive_getDpNames("*FED","FED_1_d");
   iLen_FED = dynlen(dsDpName_FED);
   //DebugN(dsDpName_FED); 
   if (iLen_FED == 0)
   {
     DebugTN("no DDU dp at FED_1_d, skip");
   }
   else
   {
     DebugTN("recreate dp_fct for FED DDU");
     for (i=1;i<=iLen_FED;i++)
     { 
       emudcsArchive_showDebugInfo(bDebug,dsDpName_FED[i]); 
       emudcsArchive_recreateFedDpFct(dsDpName_FED[i]);  //recreate DDU dp_fct
     }
   }   
   
   // get dp for Wiener Maraton channel   
   dsDpName_MA = emudcsArchive_getDpNames("*","FwWienerMarathonChannel");
   iLen_MA = dynlen(dsDpName_MA);
   //DebugN(dsDpName_MA); 
   if (iLen_MA == 0)
   {
     DebugN("no Maraton dp at FwWienerMarathonChannel, skip");
   }
   else
   {
     DebugTN("recreate dp_fct for Maraton channel");
     for (i=1;i<=iLen_MA;i++)
     {  
       emudcsArchive_showDebugInfo(bDebug,dsDpName_MA[i]);
       emudcsArchive_recreateMaDpFct(dsDpName_MA[i]);  //recreate dp_fct for Maraton dp
       emudcsArchive_setMaStatusConfig(dsDpName_MA[i]); //set config for Maraton status
     }
   }   
   
   //get dp  for FSM
  dyn_string dsDpName_FSMdev = emudcsArchive_getDpNames("*","_FwFsmDevice"); 
  //DebugN(dsDpName_FSMdev); 
  int iLen_FSMdev = dynlen(dsDpName_FSMdev);
   if(iLen_FSMdev == 0)
  {
     DebugTN("no FSM devive found, skip");
  }
  else    
  {  
    DebugTN("reconfig archiving dps for FSM devices");  
     for (i=1;i<=iLen_FSMdev;i++)
   {
    emudcsArchive_showDebugInfo(bDebug,dsDpName_FSMdev[i]);
    emudcsArchive_setFsmDpConfig(dsDpName_FSMdev[i]);
   }  
  } 
      
  dyn_string dsDpName_FSMobj = emudcsArchive_getDpNames("*","_FwFsmObject");
  //DebugN(dsDpName_FSMobj);
  int iLen_FSMobj = dynlen(dsDpName_FSMobj);
  if(iLen_FSMobj == 0)
  {
     DebugTN("no FSM object found, skip");
  }
  else
  {
    DebugTN("reconfig archiving dps for FSM objects");
    for (i=1;i<=iLen_FSMobj;i++)
    {
      emudcsArchive_showDebugInfo(bDebug,dsDpName_FSMobj[i]);
      emudcsArchive_setFsmDpConfig(dsDpName_FSMobj[i]);
    }  
  }
  
  //get dp for elmb 
  dyn_string dsDpName_FwElmbAi = emudcsArchive_getDpNames("*","FwElmbAi"); 
  //DebugN(dsDpName_FwElmbAi); 
  int iLen_FwElmbAi = dynlen(dsDpName_FwElmbAi);
  if(iLen_FwElmbAi == 0)
  {
     DebugTN("no FwElmbA1 found, skip");
  } 
  else
  {
    DebugTN("reconfig archiving dps for FwElmbAi");
    for(i=1;i<=iLen_FwElmbAi;i++)
    {
      emudcsArchive_showDebugInfo(bDebug,dsDpName_FwElmbAi[i]);
      emudcsArchive_setDpSmoothing(dsDpName_FwElmbAi[i],fTolElmbAi,iTimeElmbAi);
    }    
  }       

   //get dp for cooling 
  dyn_string dsDpName_Cooling = emudcsArchive_getDpNames("*","fwCooling_CSC_TURBINES_data"); 
  //DebugN(dsDpName_Cooling); 
  int iLen_Cooling = dynlen(dsDpName_Cooling);
  if(iLen_Cooling == 0)
  {
     DebugTN("no Cooling dp found, skip");
  } 
  else
  {
    DebugTN("reconfig archiving dps for CSC Colling");
    for(i=1;i<=iLen_Cooling;i++)
    {
      emudcsArchive_showDebugInfo(bDebug,dsDpName_Cooling[i]);
      emudcsArchive_setDpSmoothing(dsDpName_Cooling[i],fTolCooling,iTimeCooling);
    }    
  }          
   DebugTN("dp_fct/arch created");   
}
//--------recreate HV dpfct,modify for new middle layer project ------------------------------------
void emudcsArchive_recreateHvDpFct(string sDpName)
{
   string s = dpSubStr(sDpName,DPSUB_DP); 
   dyn_string ds = strsplit(s,"/");
   string sCscChamber = ds[2];  //like "CSC_ME_P12_C01"
//get HV channel numbers   
   int iChannelNumbers = emudcsArchive_getHvChannelNumbers(sCscChamber);
   for (int i=1;i<=iChannelNumbers;i++)   
         { emudcsArchive_setHvDpFunction(sDpName,sCscChamber,i); }  //create dpfct for HV channel        
// create chamber master channel dpfct
           emudcsArchive_setHvMaChDpFunction(sDpName,sCscChamber); 
}
//--------recreate Lv dpfct,modify for new middle layer project----------------------------------
void emudcsArchive_recreateLvDpFct(string sDpName)
{
    dyn_string ds1 = strsplit(sDpName,":");
    dyn_string ds2 = strsplit(ds1[2],"/");
    string sCscChamber = ds2[2];  //like "CSC_ME_P12_C01" 
    int iCfebNumbers = emudcsArchive_getCfebNumbers(sCscChamber);  
         for (int i=1;i<=iCfebNumbers;i++)  
         {
            emudcsArchive_setLvCfebDpFunction(sDpName,sCscChamber,i); //recreate LV dp fct for cfeb
         } 
            emudcsArchive_setLvAlctDpFunction(sDpName,sCscChamber);   //recreate LV dp fct for alct           
            emudcsArchive_setLvTmbDpFunction(sDpName,sCscChamber);    //recreate LV dp fct for Tmb            
            emudcsArchive_setLvLvdbDpFunction(sDpName,sCscChamber);   //recreate LV dp fct for Lvdb            
            emudcsArchive_setLvBitsDpFunction(sDpName,sCscChamber);   //recreate LV dp fct for Bits
}
//--------recreate Temp dpfct,modify for new middle layer project----------------------------------
void emudcsArchive_recreateTempDpFct(string sDpName)
{
    dyn_string ds1 = strsplit(sDpName,":");
    dyn_string ds2 = strsplit(ds1[2],"/");
    string sCscChamber = ds2[2];  //like "CSC_ME_P12_C01" 
    int iCfebNumbers = emudcsArchive_getCfebNumbers(sCscChamber);  
       for (int i=1;i<=iCfebNumbers;i++)  
         {
            emudcsArchive_setTempCfebDpFunction(sDpName,sCscChamber,i); //recreate TEMP dp fct for cfeb
         } 
            emudcsArchive_setTempAlctDmbDpFunction(sDpName,sCscChamber); //recreate TEMP dp fct for alct/dmb
}
//-----------------not exist for new middle layer project--------------------------------------------
void emudcsArchive_recreateHvmDpFct(string sDpName)
{
  dyn_string ds1,ds2;
    string sHVMaster,sHVMasterNew;
    ds1 = strsplit(sDpName,":");
    sHVMaster = ds1[2];
    ds2 = strsplit(ds1[2],"_");
    sHVMasterNew = ds2[1]+"_0"+ds2[2]+"_"+ds2[3];  //like: master600_00_0 
    //DebugTN("HV master: "+sHVMaster); 
    //DebugTN(sHVMasterNew);         
        for (int i=1;i<=8;i++)  
          { emudcsArchive_setHvmDpFunction(sHVMaster,sHVMasterNew,i);} //recreate dp_fct for HV master channel       
}
//--------recreate Hvp dpfct,modify for new middle layer project----------------------------------
void emudcsArchive_recreateHvpDpFct(string sDpName)
{
  dyn_string ds1,ds2;
    ds1 = strsplit(sDpName,":");
    ds2 = strsplit(ds1[2],"/");
    string sHVPrimary = ds2[2];  //like: primary500_1_1 
    //DebugTN("HV primary: "+sHVPrimary);                
        emudcsArchive_setHvpDpFunction(sDpName,sHVPrimary);         
}
//--------recreate Fed dpfct,modify for new middle layer project----------------------------------
void emudcsArchive_recreateFedDpFct(string sDpName)
{
   dyn_string ds1,ds2;
   ds1 = strsplit(sDpName,":");
   ds2 = strsplit(ds1[2],"/");
   string sDpNameFed = "CSC_FED_"+ds2[2];  //like: CSC_FED_DDU01
       emudcsArchive_setFedDpFunction(sDpName,sDpNameFed);  // create FED DDU fct  
}
//-----------------------------------------------------------------------------------------------
void emudcsArchive_recreateMaDpFct(string sDpName)
{
        dyn_string ds1 = strsplit(sChannel,":");
        dyn_string ds2 = strsplit(ds1[2],"/");
        int iCrateNumber = strltrim(ds2[3],"Crate");
        string sCrateNew,sChannelNew,sChannelOld;
         if ( iCrateNumber < 10)
           sCrateNew = "Crate0"+iCrateNumber;
         else if (iCrateNumber > 9)
           sCrateNew = "Crate"+iCrateNumber; 
           sChannelNew = ds2[1]+"_"+sCrateNew+"_"+ds2[4];
           sChannelOld = ds1[2];
        dyn_string ds3 = makeDynString("Voltage","Current","Temperature");
        for(int i=1;i<=dynlen(ds3);i++)
        {
          string sChannel = sChannelNew+"_"+ds3[i];
          emudcsArchive_showDebugInfo(bDebug,sChannel);
          emudcsArchive_setMaDpFunction(sChannel,sChannelOld,ds3[i]);
        } 
}
//----------------------------------------------------------------------------------------------
void emudcsArchive_removeDpSmoothing(string sDpName)
{
    dpSetWait (sDpName + ":_archive.._type",DPCONFIG_NONE);       
}
//----------------------------------------------------------------------------------------------
void emudcsArchive_showDebugInfo(bool bDebug, anytype aMessage)
{
  if(bDebug)
  {
    DebugTN(aMessage);
  }
  else return;
} 
//-----------set hv channel number for chamber type,add for new middle layer-------------------
void emudcsArchive_setHvChannelNumber()
{    
// get dp name for HV      
   dyn_string dsDpName_HV = emudcsArchive_getDpNames("*HighVoltage*","CscHvChamberFastMon");   
   //DebugN(dsDpName_HV);   
   for (int i=1;i<=dynlen(dsDpName_HV);i++)
   {
     string sDpName = dpSubStr(dsDpName_HV[i],DPSUB_DP);
     //DebugTN(sDpName);
     dyn_string ds = strsplit(sDpName,"/");     
     //DebugTN(ds[2]);     
     dyn_string ds1 = strsplit(ds[2],"_");
     //DebugTN(ds1[3]);
     string sChamberType = ds1[3];
     int iChannelNumbers;
     if(sChamberType == "M22" || sChamberType == "M32" || sChamberType == "M42" ||
        sChamberType == "P22" || sChamberType == "P32" || sChamberType == "P42")
        {iChannelNumbers = 30;}
     else
        {iChannelNumbers = 18;}    
     dpSetWait(dsDpName_HV[i]+".num_chans", iChannelNumbers);
   }        
}
//-----------get hv channel number for chamber type----------------------------------------
//-----------add for new middle layer------------------------------------------------------
int emudcsArchive_getHvChannelNumbers(string sCscChamber)
{    
     dyn_string ds = strsplit(sCscChamber,"_");
     //DebugTN(ds[3]);
     string sChamberType = ds[3];
     int iChannelNumbers;
     if(sChamberType == "M22" || sChamberType == "M32" || sChamberType == "M42" ||
        sChamberType == "P22" || sChamberType == "P32" || sChamberType == "P42")
        {iChannelNumbers = 30;}
     else
        {iChannelNumbers = 18;}    
     return iChannelNumbers;        
}
//--------get cfeb numbers----------------------------------------------------------------
//--------add for new middle layer--------------------------------------------------------
int emudcsArchive_getCfebNumbers(string sCscChamber)
{
  // find chamber type   
    dyn_string ds = strsplit(sCscChamber,"_");
    string sCscStation = ds[3];
    int iCfebNumbers;
    if( sCscStation == "P13" || sCscStation == "M13")
    {
       iCfebNumbers = 4;
    }    
    else
    {
       iCfebNumbers = 5;
    }
    return iCfebNumbers;    
}  
