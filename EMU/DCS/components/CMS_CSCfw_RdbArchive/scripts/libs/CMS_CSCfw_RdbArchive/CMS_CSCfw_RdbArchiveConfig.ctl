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
   const string    sDpTypeBits = "CSC_BITS_DATA";     // new data for CCB_bits and FPGA_bits
   const bool      bDebug     = true;                 // true  --- enable debug info
                                                      // false --- disable debug info
//-------config all archiving dps ---------------------------------------------------  
void emudcsArchive_configAllArchivingDps()  
{  
  return;
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
  dyn_string dsDpNameBits = dpNames("*","CSC_BITS_DATA");
  for (int k=1; k <= dynlen(dsDpNameBits); k++)
    {
      dpDelete(dsDpNameBits[k]);
    }
  dpTypeDelete("CSC_BITS_DATA"); 
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
  //define a dp type
       dsDpes[1] = makeDynString (dsArchiveDpTypes[i],"");
       dsDpes[2] = makeDynString ("","value");

       diDpes[1] = makeDynInt (DPEL_STRUCT);
       diDpes[2] = makeDynInt (0,DPEL_FLOAT);
  // Create a dp type
       n = dpTypeCreate(dsDpes,diDpes);
       DebugN (dsArchiveDpTypes[i]+" type Created, result: ",n); 
     }    
  // Create an int dp type for CSC_BITS_DATA      
     dsDpes[1] = makeDynString (sDpTypeBits,"");
     dsDpes[2] = makeDynString ("","value");

     diDpes[1] = makeDynInt (DPEL_STRUCT);
     diDpes[2] = makeDynInt (0,DPEL_INT);
     n = dpTypeCreate(dsDpes,diDpes);
     DebugN (sDpTypeBits+" type Created, result: ",n);           
}  
//----------  set configuration for all archiving dp-------------------------------------------------
void emudcsArchive_setAllDpConfigs()
{
   dyn_string dsDpName_HV,dsDpName_LV,dsDpName_TEMP,dsDpName_HVM,dsDpName_FED,dsDpName_MA,dsDpName_HVP; 
   int i, iLen_HV,iLen_LV,iLen_TEMP,iLen_HVM,iLen_FED,iLen_MA,iLen_HVP;
  
//------- get dp name for HV channels-------------------------------------     
   dsDpName_HV = emudcsArchive_getDpNames("*HV","HV_1_d");   
   iLen_HV = dynlen(dsDpName_HV);
   //DebugN(dsDpName_HV);
   if (iLen_HV == 0)
   {
     DebugTN("no dp at HV_1_d,skip");
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
   dsDpName_LV = emudcsArchive_getDpNames("*LV","LV_1_d");
   iLen_LV = dynlen(dsDpName_LV);
   //DebugN(dsDpName_LV);
   if (iLen_LV == 0)
   {
     DebugTN("no dp at LV_1_d, skip");
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
   dsDpName_TEMP = emudcsArchive_getDpNames("*TEMP","TEMP_1_d");
   iLen_TEMP = dynlen(dsDpName_TEMP);
   //DebugN(dsDpName_TEMP); 
   if (iLen_TEMP == 0)
   {
     DebugTN("no dp at TEMP_1_d, skip");
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
 
//--------------- get dp for HV primary ------------------------------------------  
   dsDpName_HVP = emudcsArchive_getDpNames("HV_PR_primary*","HV_PR_d");
   iLen_HVP = dynlen(dsDpName_HVP);
   //DebugN(dsDpName_HVP);
   if (iLen_HVP == 0)
   {
     DebugTN("no primary dp at HV_PR_d, skip");
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
   dsDpName_FED = emudcsArchive_getDpNames("*FED","FED_1_d");
   iLen_FED = dynlen(dsDpName_FED);
   //DebugN(dsDpName_FED); 
   if (iLen_FED == 0)
   {
     DebugTN("no DDU dp at FED_1_d, skip");
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
//----------------HV archiving dp configuration--------------------------------- 
void emudcsArchive_setHvDpConfig(string sDpName)
{  
// find chamber type   
    dyn_string ds1,ds2,ds3,ds4;
    ds1 = strsplit(sDpName,":");
    ds2 = strsplit(ds1[2],"_");
    string sChamberCoord;
    dpGet (ds1[1]+":"+"HighVoltage/"+ds1[2]+".coord",sChamberCoord);
//    DebugN(sChamberCoord);    
    ds4 = strsplit(sChamberCoord,";");

//  create HV archiving dp and fct    
    int iChannelNumbers;
    string sCscChamber,sChamberType;
    sCscChamber = "CSC_ME_"+ds2[3]+"_"+ds2[4];  //like "CSC_ME_P12_C01"
    
    if (ds4[4]== "0")
      {  
         iChannelNumbers = 30;
         sChamberType = "type1";
      }
    else if (ds4[4] == "1")
      {  
         iChannelNumbers = 18;
         sChamberType = "type1";
      } 
    else if (ds4[4] == "2")
      {  
         iChannelNumbers = 18;
         sChamberType = "type2";
      } 
    else
     {
        DebugN("missing chamber type"); 
     } 
    //DebugTN("CSC chamber: "+sCscChamber);
    //DebugN(ds4[4]);
    //DebugN(sChamberType); 
    //DebugTN("Number of HV channels: "+iChannelNumbers);
         
    for (int i=1;i<=iChannelNumbers;i++)   
         {
           emudcsArchive_createHvDps(sCscChamber,i);                //create dp for HV
           emudcsArchive_setHvDpFunction(sCscChamber,sChamberType,i); //create dp fct for HV
         }     
}

//---LV archiving dp configuration -----
void emudcsArchive_setLvDpConfig(string sDpName)
{ 
  // find chamber type   
    dyn_string ds1,ds2,ds3,ds4;
    ds1 = strsplit(sDpName,":");
    ds2 = strsplit(ds1[2],"_");
    string sCscStation = ds2[3];
    int iCfebNumbers;
    if( sCscStation == "P13" || sCscStation == "M13")
    {
       iCfebNumbers = 4;
    }    
    else
    {
       iCfebNumbers = 5;
    }
     
    string sCscChamber = "CSC_ME_"+ds2[3]+"_"+ds2[4];  //like "CSC_ME_P12_C01"   
    //DebugTN("CSC chamber: "+sCscChamber);
    //DebugTN("Cfeb numbers: "+iCfebNumbers); 
         for (int i=1;i<=iCfebNumbers;i++)  
         {
            emudcsArchive_createLvCfebDps(sCscChamber,i);      //create LV dp for cfeb
            emudcsArchive_setLvCfebDpFunction(sCscChamber,i);  //create LV dp fct for cfeb
         } 
            emudcsArchive_createLvAlctDps(sCscChamber);       //create LV dp for alct
            emudcsArchive_setLvAlctDpFunction(sCscChamber);   //create LV dp fct for alct
//new data from Tmb            
            emudcsArchive_createLvTmbDps(sCscChamber);       //create LV dp for Tmb
            emudcsArchive_setLvTmbDpFunction(sCscChamber);   //create LV dp fct for Tmb
            emudcsArchive_setDpSmoothingBool(sCscChamber+"_LV.status"); //set archiving for status at LV_1_d
//new data from LVDB
            emudcsArchive_createLvLvdbDps(sCscChamber);       //create LV dp for Lvdb
            emudcsArchive_setLvLvdbDpFunction(sCscChamber);   //create LV dp fct for Lvdb
//new data from CCB/FPFA bits  
            emudcsArchive_createLvBitsDps(sCscChamber);       //create LV dp for Bits 
            emudcsArchive_setLvBitsDpFunction(sCscChamber);   //create LV dp fct for Bits         
}
//---Temp archiving dp configuration-----
void emudcsArchive_setTempDpConfig(string sDpName)
{ 
  // find chamber type   
    dyn_string ds1,ds2,ds3,ds4;
    ds1 = strsplit(sDpName,":");
    ds2 = strsplit(ds1[2],"_");
    string sCscStation = ds2[3];
    int iCfebNumbers;
    if( sCscStation == "P13" || sCscStation == "M13")
    {
       iCfebNumbers = 4;
    }    
    else
    {
       iCfebNumbers = 5;
    }  
    string sCscChamber = "CSC_ME_"+ds2[3]+"_"+ds2[4];  //like "CSC_ME_P12_C01"   
    //DebugTN("CSC chamber "+sCscChamber);
    //DebugTN("Cfeb numbers "+iCfebNumbers);    
       for (int i=1;i<=iCfebNumbers;i++)  
         {
            emudcsArchive_createTempCfebDps(sCscChamber,i);   //create TEMP dp for cfeb
            emudcsArchive_setTempCfebDpFunction(sCscChamber,i); //create TEMP dp fct for cfeb
         } 
            emudcsArchive_createTempAlctDmbDps(sCscChamber);     //create TEMP dp for alct/dmb
            emudcsArchive_setTempAlctDmbDpFunction(sCscChamber); //create TEMP dp fct for alct/dmb
}            
//----------HV Master archiving dp configuration (8 HV channels for each)----- 
void emudcsArchive_setHvmDpConfig(string sDpName)
{     
    dyn_string ds1,ds2;
    string sHVMaster,sHVMasterNew;
    ds1 = strsplit(sDpName,":");
    sHVMaster = ds1[2];
    ds2 = strsplit(ds1[2],"_");
    sHVMasterNew = ds2[1]+"_0"+ds2[2]+"_"+ds2[3];  //like: master600_00_0 
    //DebugTN("HV master: "+sHVMaster); 
    //DebugTN(sHVMasterNew);         
        for (int i=1;i<=8;i++)  //create dp for HV master channel
         {
           emudcsArchive_createHvmDps(sHVMasterNew,i); 
           emudcsArchive_setHvmDpFunction(sHVMaster,sHVMasterNew,i);  
         }       
} 
//---------HV primary archiving dp configuration ------------------------------ 
void emudcsArchive_setHvpDpConfig(string sDpName)
{     
    dyn_string ds1,ds2;
    string sHVPrimary,sHVPrimaryNew;
    ds1 = strsplit(sDpName,":");
    sHVPrimary = ds1[2];
    ds2 = strsplit(ds1[2],"_");
    sHVPrimaryNew = ds2[3]+"_"+ds2[4]+"_"+ds2[5];  //like: primary500_1_1 
    //DebugTN("HV primary: "+sHVPrimary); 
    //DebugTN(sHVPrimaryNew);         
         
           emudcsArchive_createHvpDps(sHVPrimaryNew); 
           emudcsArchive_setHvpDpFunction(sHVPrimary,sHVPrimaryNew);  
                
} 
//----FED archiving dp configuration-------
void emudcsArchive_setFedDpConfig(string sDpName)
{   
   dyn_string ds1,ds2;
   ds1 = strsplit(sDpName,":");
   ds2 = strsplit(ds1[2],"_");
   string sDpNameFed = "CSC_FED_"+ds2[1];  //like: CSC_FED_DDU01
   //DebugTN("FED DDU: "+sDpName);
   //DebugTN(sDpNameFed);     
       emudcsArchive_createFedDps(sDpNameFed);    //create FED DDU dp
       emudcsArchive_setFedDpFunction(sDpNameFed);  // create FED DDU fct  
}
//----Maraton archiving dp configuration ---------
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
//----FSM device dps configuration ---------
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
//----HV archiving dp create------------------------
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
//---------------HV master archiving dp create ------------------------
void emudcsArchive_createHvmDps(string sHVMasterNew,int i)
{                  
         
         string sDpNameHV_MV = sHVMasterNew+"_V"+i+"_VMON";
         string sDpNameHV_MI = sHVMasterNew+"_V"+i+"_IMON";
         
         emudcsArchive_createDp(sDpNameHV_MV,"HV_M");
         emudcsArchive_setDpSmoothing(sDpNameHV_MV,fTolHVMV,iTimeHVM);
        
         emudcsArchive_createDp(sDpNameHV_MI,"HV_M");
         emudcsArchive_setDpSmoothing(sDpNameHV_MI,fTolHVMI,iTimeHVM);
}
//---------------HV primary archiving dp create ------------------------
void emudcsArchive_createHvpDps(string sHVPrimaryNew)
{                  
         
         string sDpNameHV_PV = sHVPrimaryNew+"_VMON";
         string sDpNameHV_PI = sHVPrimaryNew+"_IMON";
         
         emudcsArchive_createDp(sDpNameHV_PV,"HV_P");
         emudcsArchive_setDpSmoothing(sDpNameHV_PV,fTolHVPV,iTimeHVP);
        
         emudcsArchive_createDp(sDpNameHV_PI,"HV_P");
         emudcsArchive_setDpSmoothing(sDpNameHV_PI,fTolHVPI,iTimeHVP);
}                   
//--- LV cfeb archiving dp create------------------------
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
//--- LV alct archiving dp create------------------
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
//--- LV lvdb archiving dp create--------------------------
void emudcsArchive_createLvLvdbDps(string sCscChamber)
{
     dyn_string dsV = makeDynString ("_LV_Lvdb_v7Analog","_LV_Lvdb_v7Digital");
     for (int i=1;i<=dynlen(dsV);i++)
      {
        emudcsArchive_createDp(sCscChamber+dsV[i],"LVDB");
        emudcsArchive_setDpSmoothing(sCscChamber+dsV[i],fTolLvdb,iTimeLV);
      }   
} 
//--- LV Bits archiving dp create--------------------------
void emudcsArchive_createLvBitsDps(string sCscChamber)
{
     dyn_string dsV = makeDynString ("_LV_CCB_bits","_LV_FPGA_bits");
     for (int i=1;i<=dynlen(dsV);i++)
      {
        emudcsArchive_createDp(sCscChamber+dsV[i],"BITS");
        emudcsArchive_setDpSmoothingBool(sCscChamber+dsV[i]+".value");
      }   
}  
//--- LV Tmb archiving dp create------------------
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
//--temp Alct and Dmb archiving dp create---
void emudcsArchive_createTempAlctDmbDps(string sCscChamber)
{     
      string sDpNameTEMP_Alct = sCscChamber+"_TEMP_Alct_T1";
      string sDpNameTEMP_Dmb  = sCscChamber+"_TEMP_Dmb__T1"; 
      emudcsArchive_createDp(sDpNameTEMP_Alct,"TEMP");
      emudcsArchive_setDpSmoothing(sDpNameTEMP_Alct,fTolTEMP,iTimeTEMP);
      
      emudcsArchive_createDp(sDpNameTEMP_Dmb,"TEMP");
      emudcsArchive_setDpSmoothing(sDpNameTEMP_Dmb,fTolTEMP,iTimeTEMP);
}

//--temp Cfeb archiving dp create---  
void emudcsArchive_createTempCfebDps(string sCscChamber,int i)
{              
        string sDpNameTEMP_Cfeb = sCscChamber+"_TEMP_Cfeb_T"+i;
        emudcsArchive_createDp(sDpNameTEMP_Cfeb,"TEMP");
        emudcsArchive_setDpSmoothing(sDpNameTEMP_Cfeb,fTolTEMP,iTimeTEMP);
}        

//---FED_DDU archiving dp create------------------
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
//---create maraton dps-------------------------------------------
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
//---set archiving dp smoothing config-----------------------------  
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
//------HV dp_fct setup---------------
void emudcsArchive_setHvDpFunction(string sCscChamber,string sType,int i)
{  
   int j; 
   if (sType == "type1")
       j=0;
   else if (sType == "type2")
       j=18;
   int n = j+i;   
   string sChannel;
   if (i < 10)
          sChannel = "_V0";
   else if (i > 9)
          sChannel = "_V";   
   string sDpName,sDpNameImon,sDpNameVmon,p1;  
   sDpName = sCscChamber+"_HV";
   p1=sDpName+".data.v"+n+".imon:_original.._value";
   sDpNameImon = sDpName+sChannel+i+"_IMON";
    emudcsArchive_setDpFunction(sDpNameImon,p1);
       
   p1=sDpName+".data.v"+n+".vmon:_original.._value";
   sDpNameVmon = sDpName+sChannel+i+"_VMON";
    emudcsArchive_setDpFunction(sDpNameVmon,p1);   
}
//-----------------HV Master dp_fct setup ------------------------------
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
//-----------------HV primary dp_fct setup ------------------------------
void emudcsArchive_setHvpDpFunction(string sHVPrimary,string sHVPrimaryNew)
{  
   string sDpNameNew,sDpNameLink;
   sDpNameNew = sHVPrimaryNew+"_IMON";
   sDpNameLink =sHVPrimary+".data.imon:_original.._value";
    emudcsArchive_setDpFunction(sDpNameNew,sDpNameLink);
    
   sDpNameNew = sHVPrimaryNew+"_VMON";
   sDpNameLink = sHVPrimary+".data.vmon:_original.._value";
    emudcsArchive_setDpFunction(sDpNameNew,sDpNameLink);   
 }
//------LV cfeb dp_fct setup---------------
void emudcsArchive_setLvCfebDpFunction(string sCscChamber,int i)
{
  // string sDpName,p1; 
   string sDpName = sCscChamber+"_LV";
   dyn_string dsNew = makeDynString(sDpName+"_Cfeb33_C"+i,
                                    sDpName+"_Cfeb33_V"+i,
                                    sDpName+"_Cfeb50_C"+i,
                                    sDpName+"_Cfeb50_V"+i,
                                    sDpName+"_Cfeb60_C"+i,
                                    sDpName+"_Cfeb60_V"+i);
   
   dyn_string dsOld = makeDynString(sDpName+".data.Cfeb_o.c33.v"+i+":_original.._value",
                                    sDpName+".data.Cfeb_o.v33.v"+i+":_original.._value",
                                    sDpName+".data.Cfeb_o.c50.v"+i+":_original.._value",
                                    sDpName+".data.Cfeb_o.v50.v"+i+":_original.._value",
                                    sDpName+".data.Cfeb_o.c60.v"+i+":_original.._value",
                                    sDpName+".data.Cfeb_o.v60.v"+i+":_original.._value");
   for(int k=1;k<=dynlen(dsNew);k++)
   {
     emudcsArchive_setDpFunction(dsNew[k],dsOld[k]);
   }  
}
//----- LV lvdb dp_fct setup--------------
void emudcsArchive_setLvLvdbDpFunction(string sCscChamber)
{
   string sDpName = sCscChamber+"_LV";
   dyn_string dsNew = makeDynString(sDpName+"_Lvdb_v7Analog",
                                    sDpName+"_Lvdb_v7Digital");
   dyn_string dsOld = makeDynString(sDpName+".data.Lvdb_o.v7Analog:_original.._value",
                                    sDpName+".data.Lvdb_o.v7Digital:_original.._value");
   for(int k=1;k<=dynlen(dsNew);k++)
   {
     emudcsArchive_setDpFunction(dsNew[k],dsOld[k]);
   }  
}
//----- LV bits dp_fct setup--------------
void emudcsArchive_setLvBitsDpFunction(string sCscChamber)
{
   string sDpName = sCscChamber+"_LV";
   dyn_string dsNew = makeDynString(sDpName+"_CCB_bits",
                                    sDpName+"_FPGA_bits");
   dyn_string dsOld = makeDynString(sDpName+".data.CCB_bits:_original.._value",
                                    sDpName+".data.FPGA_bits:_original.._value");
   for(int k=1;k<=dynlen(dsNew);k++)
   {
     emudcsArchive_setDpFunction(dsNew[k],dsOld[k]);
   }  
}  
//------LV Tmb dp_fct setup---------------
void emudcsArchive_setLvTmbDpFunction(string sCscChamber)
{ 
   string sDpName = sCscChamber+"_LV";
   dyn_string dsNew = makeDynString(sDpName+"_Tmb_v50",
                                    sDpName+"_Tmb_v33",
                                    sDpName+"_Tmb_v15C",
                                    sDpName+"_Tmb_v15T",
                                    sDpName+"_Tmb_v10T",
                                    sDpName+"_Tmb_c50",
                                    sDpName+"_Tmb_c33",
                                    sDpName+"_Tmb_c15C",
                                    sDpName+"_Tmb_c15T",
                                    sDpName+"_Tmb_cRAT",
                                    sDpName+"_Tmb_vRAT",
                                    sDpName+"_Tmb_vREF",
                                    sDpName+"_Tmb_vGND",
                                    sDpName+"_Tmb_vMAX");
   
   dyn_string dsOld = makeDynString(sDpName+".data.Tmb_o.v50:_original.._value",
                                    sDpName+".data.Tmb_o.v33:_original.._value",
                                    sDpName+".data.Tmb_o.v15C:_original.._value",
                                    sDpName+".data.Tmb_o.v15T:_original.._value",
                                    sDpName+".data.Tmb_o.v10T:_original.._value",
                                    sDpName+".data.Tmb_o.c50:_original.._value",
                                    sDpName+".data.Tmb_o.c33:_original.._value",
                                    sDpName+".data.Tmb_o.c15C:_original.._value",
                                    sDpName+".data.Tmb_o.c15T:_original.._value",
                                    sDpName+".data.Tmb_o.cRAT:_original.._value",
                                    sDpName+".data.Tmb_o.vRAT:_original.._value",
                                    sDpName+".data.Tmb_o.vREF:_original.._value",
                                    sDpName+".data.Tmb_o.vGND:_original.._value",
                                    sDpName+".data.Tmb_o.vMAX:_original.._value");
   for(int k=1;k<=dynlen(dsNew);k++)
   {
     emudcsArchive_setDpFunction(dsNew[k],dsOld[k]);
   }  
   
   
}  
//------LV Alct dp_fct setup---------------
void emudcsArchive_setLvAlctDpFunction(string sCscChamber)
{ 
   string sDpName = sCscChamber+"_LV";
   dyn_string dsNew = makeDynString(sDpName+"_Alct18_C1",
                                    sDpName+"_Alct18_V1",
                                    sDpName+"_Alct33_C1",
                                    sDpName+"_Alct33_V1",
                                    sDpName+"_Alct55_C1",
                                    sDpName+"_Alct55_V1",
                                    sDpName+"_Alct56_C1",
                                    sDpName+"_Alct56_V1");
   
   dyn_string dsOld = makeDynString(sDpName+".data.Alct_o.c18:_original.._value",
                                    sDpName+".data.Alct_o.v18:_original.._value",
                                    sDpName+".data.Alct_o.c33:_original.._value",
                                    sDpName+".data.Alct_o.v33:_original.._value",
                                    sDpName+".data.Alct_o.c55:_original.._value",
                                    sDpName+".data.Alct_o.v55:_original.._value",
                                    sDpName+".data.Alct_o.c56:_original.._value",
                                    sDpName+".data.Alct_o.v56:_original.._value");
   for(int k=1;k<=dynlen(dsNew);k++)
   {
     emudcsArchive_setDpFunction(dsNew[k],dsOld[k]);
   }  
   
   
}  
//------Temp cfeb dp_fct setup---------------
void emudcsArchive_setTempCfebDpFunction(string sCscChamber,int i)
{
   string sDpName,sDpNameCfeb,sDpNameLink; 
   sDpName = sCscChamber+"_TEMP";
   sDpNameLink =sDpName+".data.t_cfeb.v"+i+":_original.._value";
   sDpNameCfeb = sDpName+"_Cfeb_T"+i;
    emudcsArchive_setDpFunction(sDpNameCfeb,sDpNameLink); //dpfct for cfeb
 }
//------Temp alct and dmb dp_fct setup---------------  
void emudcsArchive_setTempAlctDmbDpFunction(string sCscChamber) 
{   
   string sDpName,sDpNameAlct,sDpNameDmb,sDpNameLink; 
   sDpName = sCscChamber+"_TEMP"; 
   sDpNameLink =sDpName+".data.t_alct.v1:_original.._value";
   sDpNameAlct = sDpName+"_Alct_T1";
     emudcsArchive_setDpFunction(sDpNameAlct,sDpNameLink);  //dpfct for alct
      
   sDpNameLink = sDpName+".data.t_dmb.v1:_original.._value";
   sDpNameDmb = sDpName+"_Dmb__T1";
     emudcsArchive_setDpFunction(sDpNameDmb,sDpNameLink);  //dpfct for dmb
        
}      
//------FED DDU dp_fct setup---------------
void emudcsArchive_setFedDpFunction(string sDDU)
{
  
  // string sDpName,sDpNameNew,p1; 
   dyn_string ds,dsNew,dsOld; 
   ds = strsplit(sDDU,"_");   //split CSC_FED_DDU01
   string sDpName = ds[3]+"_FED";    // get DDU01_FED  
   dsOld = makeDynString( sDpName+".data.v15:_original.._value",
                          sDpName+".data.v25a:_original.._value",
                          sDpName+".data.v25b:_original.._value",
                          sDpName+".data.v33:_original.._value",
                          sDpName+".data.td1:_original.._value",
                          sDpName+".data.td2:_original.._value",
                          sDpName+".data.td3:_original.._value",
                          sDpName+".data.td4:_original.._value");
   dsNew = makeDynString( sDDU+"_V15A",sDDU+"_V25A",sDDU+"_V25B",sDDU+"_V33A",
                          sDDU+"_TD1",  sDDU+"_TD2", sDDU+"_TD3", sDDU+"_TD4");
   for(int i=1;i<=dynlen(dsNew);i++)
   {
     emudcsArchive_setDpFunction(dsNew[i],dsOld[i]);
   }                             
}
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
  
void emudcsArchive_setDpFunction(string sDpName,string sDpNameLink)
{ 
   string p1 =  sDpNameLink;
   dyn_string ds=makeDynString(p1);   
   dpSetWait(sDpName+".value:_dp_fct.._type", 60, 
             sDpName+".value:_dp_fct.._param", ds, 
             sDpName+".value:_dp_fct.._fct", "p1"); 
} 

void emudcsArchive_createDp(string sDpName,string sType)
{
  string sDpType = "CSC_"+sType+"_DATA";
  dpCreate(sDpName,sDpType);
} 
       
dyn_string emudcsArchive_defineDpTypes(dyn_string &dsArchiveDpTypes)
{
   //not include sDpTypeBITS
   dyn_string dsArchiveDpType = makeDynString(sDpTypeHVV, sDpTypeHVI, sDpTypeLVV, sDpTypeLVI,
                                              sDpTypeHVM,sDpTypeTEMP,sDpTypeFEDV,sDpTypeFEDT,
                                              sDpTypeMAV,sDpTypeMAI,sDpTypeMAT,sDpTypeHVP,sDpTypeTMB,
                                              sDpTypeLVDB);
   return dsArchiveDpType;
   
}       
// old/new comparison
void emudcsArchive_setDpSmoothingBool(string sDpName)
{
    dpSetWait (sDpName + ":_archive.._type",45,
               sDpName + ":_archive.._archive",1,
               sDpName + ":_archive.1._type",3,
               sDpName + ":_archive.1._class","_EVENT",
               sDpName + ":_archive.1._std_type",4); //  old/new comparison
}

//-----------new function for recreating Fct after HV_CC component reinstallation and setting archiving for FSM...------
void emudcsArchive_recreateAllDpFct_Arch()
{
   DebugTN("recreate dp_fct/arch...");
   dyn_string dsDpName_HV,dsDpName_LV,dsDpName_TEMP,dsDpName_HVM,dsDpName_FED,dsDpName_MA,dsDpName_HVP; 
   int i, iLen_HV,iLen_LV,iLen_TEMP,iLen_HVM,iLen_FED,iLen_MA,iLen_HVP;
  
// get dp name for HV      
   dsDpName_HV = emudcsArchive_getDpNames("*HV","HV_1_d");   
   iLen_HV = dynlen(dsDpName_HV);
   //DebugN(dsDpName_HV);
   if (iLen_HV == 0)
   {
     DebugTN("no dp at HV_1_d,skip");
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
   dsDpName_LV = emudcsArchive_getDpNames("*LV","LV_1_d");
   iLen_LV = dynlen(dsDpName_LV);
   //DebugN(dsDpName_LV);
   if (iLen_LV == 0)
   {
     DebugTN("no dp at LV_1_d, skip");
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
   dsDpName_TEMP = emudcsArchive_getDpNames("*TEMP","TEMP_1_d");
   iLen_TEMP = dynlen(dsDpName_TEMP);
   //DebugN(dsDpName_TEMP); 
   if (iLen_TEMP == 0)
   {
     DebugTN("no dp at TEMP_1_d, skip");
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
   dsDpName_HVM = emudcsArchive_getDpNames("master*","HV_1_d");
   iLen_HVM = dynlen(dsDpName_HVM);
   //DebugN(dsDpName_HVM);
   if (iLen_HVM == 0)
   {
     DebugTN("no master dp at HV_1_d, skip");
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
  
// get dp for HV primary   
   dsDpName_HVP = emudcsArchive_getDpNames("HV_PR_primary*","HV_PR_d");
   iLen_HVP = dynlen(dsDpName_HVP);
   //DebugN(dsDpName_HVP);
   if (iLen_HVP == 0)
   {
     DebugTN("no primary dp at HV_PR_d, skip");
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
//--------------------------------------------------------------------------------------------
void emudcsArchive_recreateHvDpFct(string sDpName)
{
  // find chamber type   
    dyn_string ds1,ds2,ds3,ds4;
    ds1 = strsplit(sDpName,":");
    ds2 = strsplit(ds1[2],"_");
    string sChamberCoord;
    dpGet (ds1[1]+":"+"HighVoltage/"+ds1[2]+".coord",sChamberCoord);
//    DebugN(sChamberCoord);    
    ds4 = strsplit(sChamberCoord,";");

//  recreate HV dp fct    
    int iChannelNumbers;
    string sCscChamber,sChamberType;
    sCscChamber = "CSC_ME_"+ds2[3]+"_"+ds2[4];  //like "CSC_ME_P12_C01"
    
    if (ds4[4]== "0")
      {  
         iChannelNumbers = 30;
         sChamberType = "type1";
      }
    else if (ds4[4] == "1")
      {  
         iChannelNumbers = 18;
         sChamberType = "type1";
      } 
    else if (ds4[4] == "2")
      {  
         iChannelNumbers = 18;
         sChamberType = "type2";
      } 
    else
     {
        DebugN("missing chamber type"); 
     } 
    //DebugTN("CSC chamber: "+sCscChamber);
    //DebugN(ds4[4]);
    //DebugN(sChamberType); 
    //DebugTN("Number of HV channels: "+iChannelNumbers);
         
    for (int i=1;i<=iChannelNumbers;i++)   
         {
           emudcsArchive_setHvDpFunction(sCscChamber,sChamberType,i); //recreate dp fct for HV
         }     
}
//---------------------------------------------------------------------------------------------
void emudcsArchive_recreateLvDpFct(string sDpName)
{
  // find chamber type   
    dyn_string ds1,ds2,ds3,ds4;
    ds1 = strsplit(sDpName,":");
    ds2 = strsplit(ds1[2],"_");
    string sCscStation = ds2[3];
    int iCfebNumbers;
    if( sCscStation == "P13" || sCscStation == "M13")
    {
       iCfebNumbers = 4;
    }    
    else
    {
       iCfebNumbers = 5;
    }
     
    string sCscChamber = "CSC_ME_"+ds2[3]+"_"+ds2[4];  //like "CSC_ME_P12_C01"   
    //DebugTN("CSC chamber: "+sCscChamber);
    //DebugTN("Cfeb numbers: "+iCfebNumbers); 
         for (int i=1;i<=iCfebNumbers;i++)  
         {
            emudcsArchive_setLvCfebDpFunction(sCscChamber,i);  //recreate LV dp fct for cfeb
         } 
            emudcsArchive_setLvAlctDpFunction(sCscChamber);   //recreate LV dp fct for alct
            
            emudcsArchive_setLvTmbDpFunction(sCscChamber);     //recreate LV dp fct for Tmb
            
            emudcsArchive_setLvLvdbDpFunction(sCscChamber);     //recreate LV dp fct for Lvdb
            
            emudcsArchive_setLvBitsDpFunction(sCscChamber);     //recreate LV dp fct for Bits
}
//----------------------------------------------------------------------------------------------
void emudcsArchive_recreateTempDpFct(string sDpName)
{
  // find chamber type   
    dyn_string ds1,ds2,ds3,ds4;
    ds1 = strsplit(sDpName,":");
    ds2 = strsplit(ds1[2],"_");
    string sCscStation = ds2[3];
    int iCfebNumbers;
    if( sCscStation == "P13" || sCscStation == "M13")
    {
       iCfebNumbers = 4;
    }    
    else
    {
       iCfebNumbers = 5;
    }  
    string sCscChamber = "CSC_ME_"+ds2[3]+"_"+ds2[4];  //like "CSC_ME_P12_C01"   
    //DebugTN("CSC chamber "+sCscChamber);
    //DebugTN("Cfeb numbers "+iCfebNumbers);    
       for (int i=1;i<=iCfebNumbers;i++)  
         {
            emudcsArchive_setTempCfebDpFunction(sCscChamber,i); //recreate TEMP dp fct for cfeb
         } 
            emudcsArchive_setTempAlctDmbDpFunction(sCscChamber); //recreate TEMP dp fct for alct/dmb
}
//---------------------------------------------------------------------------------------------
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
         { 
           emudcsArchive_setHvmDpFunction(sHVMaster,sHVMasterNew,i); //recreate dp_fct for HV master channel
         }       
}

//---------------------------------------------------------------------------------------------
void emudcsArchive_recreateHvpDpFct(string sDpName)
{
  dyn_string ds1,ds2;
    string sHVPrimary,sHVPrimaryNew;
    ds1 = strsplit(sDpName,":");
    sHVPrimary = ds1[2];
    ds2 = strsplit(ds1[2],"_");
    sHVPrimaryNew = ds2[3]+"_"+ds2[4]+"_"+ds2[5];  //like: primary500_1_1 
    //DebugTN("HV primary: "+sHVPrimary); 
    //DebugTN(sHVPrimaryNew);  
    
        emudcsArchive_setHvpDpFunction(sHVPrimary,sHVPrimaryNew); //recreate dp_fct for HV primary        
}
//---------------------------------------------------------------------------------------------
void emudcsArchive_recreateFedDpFct(string sDpName)
{
 dyn_string ds1,ds2;
   ds1 = strsplit(sDpName,":");
   ds2 = strsplit(ds1[2],"_");
   string sDpNameFed = "CSC_FED_"+ds2[1];  //like: CSC_FED_DDU01
   //DebugTN("FED DDU: "+sDpName);
   //DebugTN(sDpNameFed);     
       emudcsArchive_setFedDpFunction(sDpNameFed);  // recreate FED DDU fct  
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
void emudcsArchive_removeDpSmoothing(string sDpName)
{
    dpSetWait (sDpName + ":_archive.._type",DPCONFIG_NONE);       
}
void emudcsArchive_showDebugInfo(bool bDebug, anytype aMessage)
{
  if(bDebug)
  {
    DebugTN(aMessage);
  }
  else
   return;
}  
    
