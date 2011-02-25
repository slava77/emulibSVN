#uses "fwRDBArchiving/fwRDBConfig.ctl"

void setupArchiving (dyn_int projectStations, bool deleteFirst = true) {
  DebugTN("Post install: configuring RDB");
  if (deleteFirst) {    
    deleteArchiving();
  }
  
  string accountName, databaseName;
  accountName = "cms_csc_pvss_cond";
  databaseName = "cms_pvss_cond";

  fwRDBConfig_setUser(accountName);
  fwRDBConfig_setHost(databaseName);
  DebugTN("RDB configured, ***restart needed***");
  
  DebugTN("Post install: creating archiving DPs... This can take awhile, please wait for a message saying that it's completed");
  createArchivingDPs(projectStations);
  DebugTN("Post install: DONE creating archiving DPs");
  DebugTN("Post install: please restart the project when ready");
}

void deleteArchiving() {
  DebugTN("Deleting existing archiving setup");
  dyn_string archivingDpTypes = makeDynString("CSC_HV_I_DATA", "CSC_HV_V_DATA", "CSC_HV_M_DATA", "CSC_LV_I_DATA", "CSC_LV_V_DATA", "CSC_TEMP_DATA");
  for (int i=1; i <= dynlen(archivingDpTypes); i++) {
    dyn_string dps = dpNames(getSystemName() + "*", archivingDpTypes[i]);
    for (int j=1; j <= dynlen(dps); j++) {
      dpDelete(dps[j]);
    }
    dpTypeDelete(archivingDpTypes[i]);
  }
  DebugTN("Archiving setup deleted");
}

//Create special data points for csc project using for RDB archiving
//mapping original data point elements by dp_fct
//done by Xiaofeng Yang Dec,2008
//modify by Xiaofeng Yang Sept,2009
// sProjectName should be one of following format:  
//     " ME+1"   --- for porject P1
//     " ME+2/3/4"   --- for project P2,P3,P4 
//     " ME-1"   --- for porject M1
//     " ME-2/3/4"   --- for project M2,M3,M4 
createArchivingDPs(dyn_int stations)
{ 
  dyn_string validProjectNames = makeDynString(" ME+1", " ME+2/3/4", " ME-1", " ME-2/3/4");
  
   string sProjectName;
   // extract the station numbers
   dynSortAsc(stations); // sort ascending
   if ((dynlen(stations) > 0) && (stations[dynlen(stations)] < 5)) { // if minus side - sort desc.
     // sorting desc. - stupid PVSS doesn't have a function for this...
     dyn_string sortedDesc;
     for (int i = dynlen(stations); i > 0; i--) {
       dynAppend(sortedDesc, stations[i]);
     }
     stations = sortedDesc;
   }
   for (int i=1; i <= dynlen(stations); i++) {
     if (i > 1) { sProjectName += "/"; }
     
     if (stations[i] > 4) { // plus side
       if (i == 1) { sProjectName += " ME+"; }
       sProjectName += (stations[i] - 4);
     } else { // minus side - inverse
       if (i == 1) { sProjectName += " ME-"; }
       sProjectName += (5 - stations[i]);
     }
   }  
   
   
   // -----------------------------
   DebugTN("Creating archiving for " + sProjectName); 
   if ((strlen(sProjectName) == 0) || !dynContains(validProjectNames, sProjectName))
   { 
     DebugTN("Invalid project name: " + sProjectName + ". Parameters for stations from main script: " + stations);
     return; 
   }   
   else
   {           
//Create_archiving_dp_for_CSC(string sProjectName)  
//Create DP type  
  int n;
  dyn_dyn_string dsHV_Vdpes,dsHV_Idpes,dsHV_Mdpes,dsLV_Vdpes,dsLV_Idpes,dsTEMPdpes;
  dyn_dyn_int diHV_Vdpes,diHV_Idpes,diHV_Mdpes,diLV_Vdpes,diLV_Idpes,diTEMPdpes;
  // define HV_V data type
       dsHV_Vdpes[1] = makeDynString ("CSC_HV_V_DATA","");
       dsHV_Vdpes[2] = makeDynString ("","value");

       diHV_Vdpes[1] = makeDynInt (DPEL_STRUCT);
       diHV_Vdpes[2] = makeDynInt (0,DPEL_FLOAT);
  // define HV_I data type
       dsHV_Idpes[1] = makeDynString ("CSC_HV_I_DATA","");
       dsHV_Idpes[2] = makeDynString ("","value");

       diHV_Idpes[1] = makeDynInt (DPEL_STRUCT);
       diHV_Idpes[2] = makeDynInt (0,DPEL_FLOAT);  
  // define HV_M data type
       dsHV_Mdpes[1] = makeDynString ("CSC_HV_M_DATA","");
       dsHV_Mdpes[2] = makeDynString ("","value");

       diHV_Mdpes[1] = makeDynInt (DPEL_STRUCT);
       diHV_Mdpes[2] = makeDynInt (0,DPEL_FLOAT);         
  // define LV_V data type
       dsLV_Vdpes[1] = makeDynString ("CSC_LV_V_DATA","");
       dsLV_Vdpes[2] = makeDynString ("","value");

       diLV_Vdpes[1] = makeDynInt (DPEL_STRUCT);
       diLV_Vdpes[2] = makeDynInt (0,DPEL_FLOAT);
  // define LV_I data type
       dsLV_Idpes[1] = makeDynString ("CSC_LV_I_DATA","");
       dsLV_Idpes[2] = makeDynString ("","value");

       diLV_Idpes[1] = makeDynInt (DPEL_STRUCT);
       diLV_Idpes[2] = makeDynInt (0,DPEL_FLOAT);     
  // define Temp data type
       dsTEMPdpes[1] = makeDynString ("CSC_TEMP_DATA","");
       dsTEMPdpes[2] = makeDynString ("","value");

       diTEMPdpes[1] = makeDynInt (DPEL_STRUCT);
       diTEMPdpes[2] = makeDynInt (0,DPEL_FLOAT);
  // Create HV_V datapoint type
       n = dpTypeCreate(dsHV_Vdpes,diHV_Vdpes);
       DebugN ("CSC_HV_V_DATA type Created, result: ",n);
  // Create HV_I datapoint type
       n = dpTypeCreate(dsHV_Idpes,diHV_Idpes);
       DebugN ("CSC_HV_I_DATA type Created, result: ",n);  
  // Create HV_M datapoint type
       n = dpTypeCreate(dsHV_Mdpes,diHV_Mdpes);
       DebugN ("CSC_HV_M_DATA type Created, result: ",n);                
  // Create LV_V datapoint type
       n = dpTypeCreate(dsLV_Vdpes,diLV_Vdpes);
       DebugN ("CSC_LV_V_DATA type Created, result: ",n); 
 // Create LV_I datapoint type
       n = dpTypeCreate(dsLV_Idpes,diLV_Idpes);
       DebugN ("CSC_LV_I_DATA type Created, result: ",n);     
  // Create TEMP datapoint type
       n = dpTypeCreate(dsTEMPdpes,diTEMPdpes);
       DebugN ("CSC_TEMP_DATA type Created, result: ",n);   
   }
   if ((sProjectName == " ME+1") || (sProjectName == " ME-1"))
  {
    dyn_dyn_string dsFED_Vdpes,dsFED_Tdpes;
    dyn_dyn_int diFED_Vdpes,diFED_Tdpes;
   // define FED_V data type
       dsFED_Vdpes[1] = makeDynString ("CSC_FED_V_DATA","");
       dsFED_Vdpes[2] = makeDynString ("","value");

       diFED_Vdpes[1] = makeDynInt (DPEL_STRUCT);
       diFED_Vdpes[2] = makeDynInt (0,DPEL_FLOAT);  
       
   // define FED_T data type
       dsFED_Tdpes[1] = makeDynString ("CSC_FED_T_DATA","");
       dsFED_Tdpes[2] = makeDynString ("","value");

       diFED_Tdpes[1] = makeDynInt (DPEL_STRUCT);
       diFED_Tdpes[2] = makeDynInt (0,DPEL_FLOAT);     
       
   // Create FED_V datapoint type
       n = dpTypeCreate(dsFED_Vdpes,diFED_Vdpes);
       DebugN ("CSC_FED_V_DATA type created, result: ",n); 
       
   // Create FED_T datapoint type
       n = dpTypeCreate(dsFED_Tdpes,diFED_Tdpes);
       DebugN ("CSC_FED_T_DATA type created, result: ",n);
   }        
   
   // create archiving dp and fct  
   if (sProjectName == " ME+1")
   { 
     //Create dp with archiving config
     //for ME+1
     Create_archiving_dp_HV("CSC_ME_P12",36,18);
     Create_archiving_dp_HV("CSC_ME_P13",36,18);
     
     Create_archiving_dp_LV_TEMP("CSC_ME_P11",36,5);
     Create_archiving_dp_LV_TEMP("CSC_ME_P12",36,5);
     Create_archiving_dp_LV_TEMP("CSC_ME_P13",36,4); 
     //for HV master
     Create_archiving_dp_HV_Master("ME+1");
     //for FED_DDU
     Create_archiving_dp_FED("CSC_FED");
     
     // Create dp_fct
     Create_dp_fct_HV("CSC_ME_P12","P12",2,18);  //HV for ME_P12
     Create_dp_fct_HV("CSC_ME_P13","P13",2,18);  //HV for ME_P13
     Create_dp_fct_LV_TEMP("CSC_ME_P11",36,5); //LV and TEMP for ME_P11
     Create_dp_fct_LV_TEMP("CSC_ME_P12",36,5); //LV and TEMP for ME_P12
     Create_dp_fct_LV_TEMP("CSC_ME_P13",36,4); //LV and TEMP for ME_P13
     
     Create_dp_fct_HV_Master("ME+1"); 
     //for FED_DDU
     Create_archiving_dp_fct_FED("CSC_FED");   
     return;
    } 
   else if (sProjectName == " ME+2/3/4")
   { 
     //for ME+2
     Create_archiving_dp_HV("CSC_ME_P21",18,18);
     Create_archiving_dp_HV("CSC_ME_P22",36,30); 
     
     Create_archiving_dp_LV_TEMP("CSC_ME_P21",18,5);
     Create_archiving_dp_LV_TEMP("CSC_ME_P22",36,5);
        
     //for ME+3
     Create_archiving_dp_HV("CSC_ME_P31",18,18);
     Create_archiving_dp_HV("CSC_ME_P32",36,30); 
          
     Create_archiving_dp_LV_TEMP("CSC_ME_P31",18,5);
     Create_archiving_dp_LV_TEMP("CSC_ME_P32",36,5);
     
    //for ME+4
     Create_archiving_dp_HV("CSC_ME_P41",18,18);  
     Create_archiving_dp_HV("CSC_ME_P42",5,30); 
     
     Create_archiving_dp_LV_TEMP("CSC_ME_P41",18,5);  
     Create_archiving_dp_LV_TEMP("CSC_ME_P42",5,5); 
    //for HV master      
     Create_archiving_dp_HV_Master("ME+2/3/4"); 
     
     //for ME+2
     Create_dp_fct_HV("CSC_ME_P21","P21",2,18);
     Create_dp_fct_HV("CSC_ME_P22","P22",1,30);
     Create_dp_fct_LV_TEMP("CSC_ME_P21",18,5);
     Create_dp_fct_LV_TEMP("CSC_ME_P22",36,5);
     
     //for ME+3     
     Create_dp_fct_HV("CSC_ME_P31","P31",2,18); 
     Create_dp_fct_HV("CSC_ME_P32","P32",1,30);
     Create_dp_fct_LV_TEMP("CSC_ME_P31",18,5);
     Create_dp_fct_LV_TEMP("CSC_ME_P32",36,5);
     
     //for ME+4
     Create_dp_fct_HV("CSC_ME_P41","P41",2,18);    
     Create_dp_fct_HV("CSC_ME_P42","P42",1,30);
     
     Create_dp_fct_LV_TEMP("CSC_ME_P41",18,5);
     Create_dp_fct_LV_TEMP("CSC_ME_P42",5,5);
     
     Create_dp_fct_HV_Master("ME+2/3/4"); 
     return;    
         
   } 
   
   else if (sProjectName == " ME-1")
   {  
    //for ME-1 
     Create_archiving_dp_HV("CSC_ME_M12",36,18);
     Create_archiving_dp_HV("CSC_ME_M13",36,18);
     
     Create_archiving_dp_LV_TEMP("CSC_ME_M11",36,5);
     Create_archiving_dp_LV_TEMP("CSC_ME_M12",36,5);
     Create_archiving_dp_LV_TEMP("CSC_ME_M13",36,4);
     //for HV master
     Create_archiving_dp_HV_Master("ME-1"); 
     
      //for ME-1
     Create_dp_fct_HV("CSC_ME_M12","M12",2,18); 
     Create_dp_fct_HV("CSC_ME_M13","M13",2,18);
     Create_dp_fct_LV_TEMP("CSC_ME_M11",36,5);
     Create_dp_fct_LV_TEMP("CSC_ME_M12",36,5);
     Create_dp_fct_LV_TEMP("CSC_ME_M13",36,4);
     
     Create_dp_fct_HV_Master("ME-1");
     return;
         
   } 
   else if (sProjectName == " ME-2/3/4")
   {  
     //for ME-2
     Create_archiving_dp_HV("CSC_ME_M21",18,18);
     Create_archiving_dp_HV("CSC_ME_M22",36,30); 
     
     Create_archiving_dp_LV_TEMP("CSC_ME_M21",18,5);
     Create_archiving_dp_LV_TEMP("CSC_ME_M22",36,5); 
      
     //for ME-3  
     Create_archiving_dp_HV("CSC_ME_M31",18,18);
     Create_archiving_dp_HV("CSC_ME_M32",36,30); 
     
     Create_archiving_dp_LV_TEMP("CSC_ME_M31",18,5);
     Create_archiving_dp_LV_TEMP("CSC_ME_M32",36,5);  
     
     //for ME-4
     Create_archiving_dp_HV("CSC_ME_M41",18,18);     
     Create_archiving_dp_LV_TEMP("CSC_ME_M41",18,5);  
     // for HV master
     Create_archiving_dp_HV_Master("ME-2/3/4");
     
     //for ME-2 
     Create_dp_fct_HV("CSC_ME_M21","M21",2,18);
     Create_dp_fct_HV("CSC_ME_M22","M22",1,30);
     Create_dp_fct_LV_TEMP("CSC_ME_M21",18,5);
     Create_dp_fct_LV_TEMP("CSC_ME_M22",36,5);
    
     //for ME-3 
     Create_dp_fct_HV("CSC_ME_M31","M31",2,18);
     Create_dp_fct_HV("CSC_ME_M32","M32",1,30);
     Create_dp_fct_LV_TEMP("CSC_ME_M31",18,5);
     Create_dp_fct_LV_TEMP("CSC_ME_M32",36,5);  
     //for ME-4
     Create_dp_fct_HV("CSC_ME_M41","M41",2,18);  
     Create_dp_fct_LV_TEMP("CSC_ME_M41",18,5);
     
     Create_dp_fct_HV_Master("ME-2/3/4");
     return;       
   }
   else 
   return;            
} 
//----HV archiving dp create----- 
// sCSC_Station: station name like CSC_ME_M22
// iChamberNumbers: chamber number on that station,like 36
// iChannelNumbers: HV channel number on chamber of that station,like 30
void Create_archiving_dp_HV(string sCSC_Station,int iChamberNumbers,int iChannelNumbers)
{   
   //DebugN(sCSC_Station);
   //DebugN(iChamberNumbers);
   //DebugN(iChannelNumbers);
    dyn_string dCSC_Chambers;
    if (iChamberNumbers == 18) 
      //for 18 chambers
       dCSC_Chambers = makeDynString(sCSC_Station+"_C01",sCSC_Station+"_C02",sCSC_Station+"_C03",
                                     sCSC_Station+"_C04",sCSC_Station+"_C05",sCSC_Station+"_C06",
                                     sCSC_Station+"_C07",sCSC_Station+"_C08",sCSC_Station+"_C09",
                                     sCSC_Station+"_C10",sCSC_Station+"_C11",sCSC_Station+"_C12",
                                     sCSC_Station+"_C13",sCSC_Station+"_C14",sCSC_Station+"_C15",
                                     sCSC_Station+"_C16",sCSC_Station+"_C17",sCSC_Station+"_C18");
        
    
    else if (iChamberNumbers == 36)
      //36 chambers
       dCSC_Chambers = makeDynString(sCSC_Station+"_C01",sCSC_Station+"_C02",sCSC_Station+"_C03",
                                     sCSC_Station+"_C04",sCSC_Station+"_C05",sCSC_Station+"_C06",
                                     sCSC_Station+"_C07",sCSC_Station+"_C08",sCSC_Station+"_C09",
                                     sCSC_Station+"_C10",sCSC_Station+"_C11",sCSC_Station+"_C12",
                                     sCSC_Station+"_C13",sCSC_Station+"_C14",sCSC_Station+"_C15",
                                     sCSC_Station+"_C16",sCSC_Station+"_C17",sCSC_Station+"_C18",
                                     sCSC_Station+"_C19",sCSC_Station+"_C20",sCSC_Station+"_C21",
                                     sCSC_Station+"_C22",sCSC_Station+"_C23",sCSC_Station+"_C24",
                                     sCSC_Station+"_C25",sCSC_Station+"_C26",sCSC_Station+"_C27",
                                     sCSC_Station+"_C28",sCSC_Station+"_C29",sCSC_Station+"_C30",
                                     sCSC_Station+"_C31",sCSC_Station+"_C32",sCSC_Station+"_C33",
                                     sCSC_Station+"_C34",sCSC_Station+"_C35",sCSC_Station+"_C36"); 
    
    else if (iChamberNumbers == 5)
      //5 chambers for ME+4/2
       dCSC_Chambers = makeDynString(sCSC_Station+"_C09",sCSC_Station+"_C10",sCSC_Station+"_C11",
                                     sCSC_Station+"_C12",sCSC_Station+"_C13");                              
     else DebugN("error");
     
       //DebugN(dCSC_Chambers[1]);     
       int k;
       for (k=1;k<=dynlen(dCSC_Chambers);k++)
       { 
         DebugTN("Creating HV archiving DP for chamber " + dCSC_Chambers[k] + ". number of channels: " + iChannelNumbers);
         int i;
         for (i=1;i<=iChannelNumbers;i++)  //create dp for HV 
         {
           Chamber_HV_dp_setup(dCSC_Chambers[k],i);   
         } 
       }
}

//---LV and Temp archiving dp create -----
// iCfebNumbers: cfeb numbers on chamber, like 4 for ME1/3 and 5 for all other stations
void Create_archiving_dp_LV_TEMP(string sCSC_Station,int iChamberNumbers,int iCfebNumbers)
{ 
  dyn_string dCSC_Chambers;
  if (iChamberNumbers == 18)  
      //for 18 chambers
       dCSC_Chambers = makeDynString(sCSC_Station+"_C01",sCSC_Station+"_C02",sCSC_Station+"_C03",
                                     sCSC_Station+"_C04",sCSC_Station+"_C05",sCSC_Station+"_C06",
                                     sCSC_Station+"_C07",sCSC_Station+"_C08",sCSC_Station+"_C09",
                                     sCSC_Station+"_C10",sCSC_Station+"_C11",sCSC_Station+"_C12",
                                     sCSC_Station+"_C13",sCSC_Station+"_C14",sCSC_Station+"_C15",
                                     sCSC_Station+"_C16",sCSC_Station+"_C17",sCSC_Station+"_C18");
    
    else if (iChamberNumbers == 36)
      //36 chambers
       dCSC_Chambers = makeDynString(sCSC_Station+"_C01",sCSC_Station+"_C02",sCSC_Station+"_C03",
                                     sCSC_Station+"_C04",sCSC_Station+"_C05",sCSC_Station+"_C06",
                                     sCSC_Station+"_C07",sCSC_Station+"_C08",sCSC_Station+"_C09",
                                     sCSC_Station+"_C10",sCSC_Station+"_C11",sCSC_Station+"_C12",
                                     sCSC_Station+"_C13",sCSC_Station+"_C14",sCSC_Station+"_C15",
                                     sCSC_Station+"_C16",sCSC_Station+"_C17",sCSC_Station+"_C18",
                                     sCSC_Station+"_C19",sCSC_Station+"_C20",sCSC_Station+"_C21",
                                     sCSC_Station+"_C22",sCSC_Station+"_C23",sCSC_Station+"_C24",
                                     sCSC_Station+"_C25",sCSC_Station+"_C26",sCSC_Station+"_C27",
                                     sCSC_Station+"_C28",sCSC_Station+"_C29",sCSC_Station+"_C30",
                                     sCSC_Station+"_C31",sCSC_Station+"_C32",sCSC_Station+"_C33",
                                     sCSC_Station+"_C34",sCSC_Station+"_C35",sCSC_Station+"_C36"); 
    
     else if (iChamberNumbers == 5)
      //5 chambers for ME+4/2
       dCSC_Chambers = makeDynString(sCSC_Station+"_C09",sCSC_Station+"_C10",sCSC_Station+"_C11",
                                     sCSC_Station+"_C12",sCSC_Station+"_C13");
      else DebugN("error");
    
   int k;
    for (k=1;k<=dynlen(dCSC_Chambers);k++)
    {
      DebugTN("Creating LV & TEMP archiving DPs for chamber " + dCSC_Chambers[k] + ". number of CFEBs: " + iCfebNumbers);
      int i;
       for (i=1;i<=iCfebNumbers;i++)  
        {
            Chamber_LV_Cfeb_dp_setup(dCSC_Chambers[k],i);   //create LV dp for cfeb
            Chamber_TEMP_Cfeb_dp_setup(dCSC_Chambers[k],i);  //create TEMP dp for cfeb
        } 
            Chamber_LV_Alct_dp_setup(dCSC_Chambers[k]);    //create LV dp for alct
            Chamber_TEMP_AD_dp_setup(dCSC_Chambers[k]);    //create TEMP dp for alct/dmb
    } 
}
//----HV for Master with 8 HV channel for each----- 
void Create_archiving_dp_HV_Master(string sCSC_Station)
{ 
    dyn_string dHV_Master;
    if      (sCSC_Station == "ME+1")
    dHV_Master = makeDynString("master600_00_0","master600_00_1","master600_00_2",
                               "master600_00_3","master600_00_4","master600_00_5");
    else if (sCSC_Station == "ME+2/3/4")
    dHV_Master = makeDynString("master600_04_0","master600_04_1","master600_04_2",
                               "master600_04_3","master600_04_4","master600_04_5",
                               "master600_08_0","master600_08_1","master600_08_2",
                               "master600_08_3","master600_08_4","master600_08_5",
                               "master600_12_0","master600_12_1");
    
    else if (sCSC_Station == "ME-1")
    dHV_Master = makeDynString("master500_00_0","master500_00_1","master500_00_2",
                               "master500_00_3","master500_00_4","master500_00_5");
    else if (sCSC_Station == "ME-2/3/4")
    dHV_Master = makeDynString("master500_04_0","master500_04_1","master500_04_2",
                               "master500_04_3","master500_04_4","master500_04_5",
                               "master500_08_0","master500_08_1","master500_08_2",
                               "master500_08_3","master500_08_4","master500_08_5",
                               "master500_12_0","master500_12_1");
    
       int k;
       for (k=1;k<=dynlen(dHV_Master);k++)
       { 
         DebugTN("Creating HV Master archiving DP for master " + dHV_Master[k]);
         int i;
         for (i=1;i<=8;i++)  //create dp for HV master channel
         {
           HV_master_dp_config_setup(dHV_Master[k],i);   
         } 
       }
} 
//create archiving dp for FED
void Create_archiving_dp_FED(string sCSC_FED)
{   
   //DebugN(sCSC_Station);
   
    dyn_string dsFED_DDUs;
    
          dsFED_DDUs = makeDynString(sCSC_FED+"_DDU01",sCSC_FED+"_DDU02",sCSC_FED+"_DDU03",
                                     sCSC_FED+"_DDU04",sCSC_FED+"_DDU05",sCSC_FED+"_DDU06",
                                     sCSC_FED+"_DDU07",sCSC_FED+"_DDU08",sCSC_FED+"_DDU09",
                                     sCSC_FED+"_DDU10",sCSC_FED+"_DDU11",sCSC_FED+"_DDU12",
                                     sCSC_FED+"_DDU13",sCSC_FED+"_DDU14",sCSC_FED+"_DDU15",
                                     sCSC_FED+"_DDU16",sCSC_FED+"_DDU17",sCSC_FED+"_DDU18",
                                     sCSC_FED+"_DDU19",sCSC_FED+"_DDU20",sCSC_FED+"_DDU21",
                                     sCSC_FED+"_DDU22",sCSC_FED+"_DDU23",sCSC_FED+"_DDU24",
                                     sCSC_FED+"_DDU25",sCSC_FED+"_DDU26",sCSC_FED+"_DDU27",
                                     sCSC_FED+"_DDU28",sCSC_FED+"_DDU29",sCSC_FED+"_DDU30",
                                     sCSC_FED+"_DDU31",sCSC_FED+"_DDU32",sCSC_FED+"_DDU33",
                                     sCSC_FED+"_DDU34",sCSC_FED+"_DDU35",sCSC_FED+"_DDU36");
    
    
     
       //DebugN(dsFED_DDUs[1]);     
       int k;
       for (k=1;k<=dynlen(dsFED_DDUs);k++)
       { 
           DebugTN("Creating FED archiving DP for DDU " + dsFED_DDUs[k]);
           FED_DDU_dp_setup(dsFED_DDUs[k]);    //create FED DDU dp
       }
}
//------Create dp_fct for HV----------------------------------
//sCSC_Station: station name, like CSC_ME_M12
//sStation_Type: short station name,like M12
//iChamberType: 1 -- only HV type 1, start from ch1, 2 -- have type 1 and type 2 start from ch19
//iChannelNumbers: HV channel numbers 18 or 30
void Create_dp_fct_HV(string sCSC_Station,string sStation_Type,int iChamberType,int iChannelNumbers) 
{        
  dyn_string dChamber_type1,dChamber_type2;
  if ( sStation_Type == "M12" || sStation_Type == "M13" || sStation_Type == "P12" || sStation_Type == "P13")
    {//for ME+1/2&3 and ME-1/2&3 
    //for chamber with HV chamnnel 1-18
   dChamber_type1 = makeDynString(sCSC_Station+"_C02",sCSC_Station+"_C04",sCSC_Station+"_C06",
                                  sCSC_Station+"_C08",sCSC_Station+"_C10",sCSC_Station+"_C12",
                                  sCSC_Station+"_C14",sCSC_Station+"_C16",sCSC_Station+"_C18",
                                  sCSC_Station+"_C20",sCSC_Station+"_C22",sCSC_Station+"_C24",
                                  sCSC_Station+"_C26",sCSC_Station+"_C28",sCSC_Station+"_C30",
                                  sCSC_Station+"_C32",sCSC_Station+"_C34",sCSC_Station+"_C36");
    
    //for chamber with HV chamnnel 19-36
   dChamber_type2 = makeDynString(sCSC_Station+"_C01",sCSC_Station+"_C03",sCSC_Station+"_C05",
                                  sCSC_Station+"_C07",sCSC_Station+"_C09",sCSC_Station+"_C11",
                                  sCSC_Station+"_C13",sCSC_Station+"_C15",sCSC_Station+"_C17",
                                  sCSC_Station+"_C19",sCSC_Station+"_C21",sCSC_Station+"_C23",
                                  sCSC_Station+"_C25",sCSC_Station+"_C27",sCSC_Station+"_C29",
                                  sCSC_Station+"_C31",sCSC_Station+"_C33",sCSC_Station+"_C35");
   }
  if (sStation_Type == "M22" || sStation_Type == "M32" || sStation_Type == "P22" || sStation_Type == "P32")
  {//for ME+2/2 ME+3/3, ME-2/2 ME-3/2
    dChamber_type1 = makeDynString(sCSC_Station+"_C01",sCSC_Station+"_C02",sCSC_Station+"_C03",
                                   sCSC_Station+"_C04",sCSC_Station+"_C05",sCSC_Station+"_C06",
                                   sCSC_Station+"_C07",sCSC_Station+"_C08",sCSC_Station+"_C09",
                                   sCSC_Station+"_C10",sCSC_Station+"_C11",sCSC_Station+"_C12",
                                   sCSC_Station+"_C13",sCSC_Station+"_C14",sCSC_Station+"_C15",
                                   sCSC_Station+"_C16",sCSC_Station+"_C17",sCSC_Station+"_C18",
                                   sCSC_Station+"_C19",sCSC_Station+"_C20",sCSC_Station+"_C21",
                                   sCSC_Station+"_C22",sCSC_Station+"_C23",sCSC_Station+"_C24",
                                   sCSC_Station+"_C25",sCSC_Station+"_C26",sCSC_Station+"_C27",
                                   sCSC_Station+"_C28",sCSC_Station+"_C29",sCSC_Station+"_C30",
                                   sCSC_Station+"_C31",sCSC_Station+"_C32",sCSC_Station+"_C33",
                                   sCSC_Station+"_C34",sCSC_Station+"_C35",sCSC_Station+"_C36");
  }  
  if (sStation_Type == "M21")
  {//for ME-2/1
    //for chamber with HV chamnnel 1-18
   dChamber_type1 = makeDynString(sCSC_Station+"_C01",sCSC_Station+"_C02",sCSC_Station+"_C03",
                                  sCSC_Station+"_C04",sCSC_Station+"_C05",sCSC_Station+"_C06",
                                  sCSC_Station+"_C07",sCSC_Station+"_C09",sCSC_Station+"_C11",
                                  sCSC_Station+"_C13",sCSC_Station+"_C14",sCSC_Station+"_C17",
                                  sCSC_Station+"_C18");
   //for chamber with HV chamnnel 19-36
   dChamber_type2 = makeDynString(sCSC_Station+"_C08",sCSC_Station+"_C10",sCSC_Station+"_C12",
                                  sCSC_Station+"_C15",sCSC_Station+"_C16");
  }
  if (sStation_Type == "M31")
  {//for ME-3/1
    //for chamber with HV chamnnel 19-36
   dChamber_type2 = makeDynString(sCSC_Station+"_C01",sCSC_Station+"_C02",sCSC_Station+"_C03",
                                  sCSC_Station+"_C04",sCSC_Station+"_C05",sCSC_Station+"_C06",
                                  sCSC_Station+"_C07",sCSC_Station+"_C09",sCSC_Station+"_C11",
                                  sCSC_Station+"_C13",sCSC_Station+"_C15",sCSC_Station+"_C17",
                                  sCSC_Station+"_C18");
   //for chamber with HV chamnnel 1-18
   dChamber_type1 = makeDynString(sCSC_Station+"_C08",sCSC_Station+"_C10",sCSC_Station+"_C12",
                                  sCSC_Station+"_C14",sCSC_Station+"_C16");
  }
  if (sStation_Type == "P21")
  {//for ME+2/1
    //for chamber with HV chamnnel 1-18
   dChamber_type1 = makeDynString(sCSC_Station+"_C02",sCSC_Station+"_C04",sCSC_Station+"_C06",
                                  sCSC_Station+"_C08",sCSC_Station+"_C09",sCSC_Station+"_C10",
                                  sCSC_Station+"_C11",sCSC_Station+"_C12",sCSC_Station+"_C13",
                                  sCSC_Station+"_C14",sCSC_Station+"_C15",sCSC_Station+"_C16",
                                  sCSC_Station+"_C18");
   //for chamber with HV chamnnel 19-36
   dChamber_type2 = makeDynString(sCSC_Station+"_C01",sCSC_Station+"_C03",sCSC_Station+"_C05",
                                  sCSC_Station+"_C07",sCSC_Station+"_C17");
  }
  if (sStation_Type == "P31")
  {//for ME+3/1
    //for chamber with HV chamnnel 19-36
   dChamber_type2 = makeDynString(sCSC_Station+"_C02",sCSC_Station+"_C04",sCSC_Station+"_C06",
                                  sCSC_Station+"_C08",sCSC_Station+"_C09",sCSC_Station+"_C10",
                                  sCSC_Station+"_C11",sCSC_Station+"_C12",sCSC_Station+"_C13",
                                  sCSC_Station+"_C14",sCSC_Station+"_C15",sCSC_Station+"_C16",
                                  sCSC_Station+"_C18");
   //for chamber with HV chamnnel 1-18
   dChamber_type1 = makeDynString(sCSC_Station+"_C01",sCSC_Station+"_C03",sCSC_Station+"_C05",
                                  sCSC_Station+"_C07",sCSC_Station+"_C17");
  }
  if (sStation_Type == "P42")
  {//for ME+4/1
    //for 5 chambers with HV chamnnel 1-30
   dChamber_type1 = makeDynString(sCSC_Station+"_C09",sCSC_Station+"_C10",sCSC_Station+"_C11",
                                  sCSC_Station+"_C12",sCSC_Station+"_C13");
  } 
  
  //for stations only have type 1 chambers
  if ( iChamberType == 1)
  { 
   int k;
     for (k=1;k<=dynlen(dChamber_type1);(k++))
      {
        int i;
        for (i=1;i<=iChannelNumbers;i++)
          {
            HV_dp_fct_setup(dChamber_type1[k],"type1",i);
          }
      }
   } 
  //for stations have type 1 and 2 chambers
  else if ( iChamberType == 2)
   {   
    int k;
      for (k=1;k<=dynlen(dChamber_type1);(k++))
      {
        int i;
        for (i=1;i<=iChannelNumbers;i++)
        {
          HV_dp_fct_setup(dChamber_type1[k],"type1",i);
        }
      } 
     for (k=1;k<=dynlen(dChamber_type2);(k++))
      {
        int i;
        for (i=1;i<=iChannelNumbers;i++)
          {
           HV_dp_fct_setup(dChamber_type2[k],"type2",i);
          }
      }
   }
}       

//------Create dp_fct for LV Temp------------
// iCfebNumbers: cfeb numbers on chamber, like 4 for ME1/3 and 5 for all other stations
void Create_dp_fct_LV_TEMP(string sCSC_Station, int iChamberNumbers, int iCfebNumbers) 
{        
  dyn_string dCSC_Chambers;
   if (iChamberNumbers == 18)
    dCSC_Chambers = makeDynString(sCSC_Station+"_C01",sCSC_Station+"_C02",sCSC_Station+"_C03",
                                  sCSC_Station+"_C04",sCSC_Station+"_C05",sCSC_Station+"_C06",
                                  sCSC_Station+"_C07",sCSC_Station+"_C08",sCSC_Station+"_C09",
                                  sCSC_Station+"_C10",sCSC_Station+"_C11",sCSC_Station+"_C12",
                                  sCSC_Station+"_C13",sCSC_Station+"_C14",sCSC_Station+"_C15",
                                  sCSC_Station+"_C16",sCSC_Station+"_C17",sCSC_Station+"_C18");
   else if (iChamberNumbers == 36)
     dCSC_Chambers = makeDynString(sCSC_Station+"_C01",sCSC_Station+"_C02",sCSC_Station+"_C03",
                                  sCSC_Station+"_C04",sCSC_Station+"_C05",sCSC_Station+"_C06",
                                  sCSC_Station+"_C07",sCSC_Station+"_C08",sCSC_Station+"_C09",
                                  sCSC_Station+"_C10",sCSC_Station+"_C11",sCSC_Station+"_C12",
                                  sCSC_Station+"_C13",sCSC_Station+"_C14",sCSC_Station+"_C15",
                                  sCSC_Station+"_C16",sCSC_Station+"_C17",sCSC_Station+"_C18",
                                  sCSC_Station+"_C19",sCSC_Station+"_C20",sCSC_Station+"_C21",
                                  sCSC_Station+"_C22",sCSC_Station+"_C23",sCSC_Station+"_C24",
                                  sCSC_Station+"_C25",sCSC_Station+"_C26",sCSC_Station+"_C27",
                                  sCSC_Station+"_C28",sCSC_Station+"_C29",sCSC_Station+"_C30",
                                  sCSC_Station+"_C31",sCSC_Station+"_C32",sCSC_Station+"_C33",
                                  sCSC_Station+"_C34",sCSC_Station+"_C35",sCSC_Station+"_C36");
   else if (iChamberNumbers == 5)
    dCSC_Chambers = makeDynString(sCSC_Station+"_C09",sCSC_Station+"_C10",sCSC_Station+"_C11",
                                  sCSC_Station+"_C12",sCSC_Station+"_C13"); 


   int k;
    for (k=1;k<=dynlen(dCSC_Chambers);k++)
   {
      int i;
       for (i=1;i<=iCfebNumbers;i++)
        {
//for cfeb        
          LV_cfeb_dp_fct_setup(dCSC_Chambers[k],i);  
          TEMP_cfeb_dp_fct_setup(dCSC_Chambers[k],i);

        }
//for alct  and dmb   
          LV_alct_dp_fct_setup(dCSC_Chambers[k]);
          TEMP_alct_dmb_dp_fct_setup(dCSC_Chambers[k]);
    }     
 } 

//----HV for Master with 8 HV channel for each----- 
void Create_dp_fct_HV_Master(string sCSC_Station)
{ 
    dyn_string dHV_Master;
    if      (sCSC_Station == "ME+1")
    dHV_Master = makeDynString("master600_00_0","master600_00_1","master600_00_2",
                               "master600_00_3","master600_00_4","master600_00_5");
    else if (sCSC_Station == "ME+2/3/4")
    dHV_Master = makeDynString("master600_04_0","master600_04_1","master600_04_2",
                               "master600_04_3","master600_04_4","master600_04_5",
                               "master600_08_0","master600_08_1","master600_08_2",
                               "master600_08_3","master600_08_4","master600_08_5",
                               "master600_12_0","master600_12_1");
    else if (sCSC_Station == "ME-1")
    dHV_Master = makeDynString("master500_00_0","master500_00_1","master500_00_2",
                               "master500_00_3","master500_00_4","master500_00_5");
    else if (sCSC_Station == "ME-2/3/4")
    dHV_Master = makeDynString("master500_04_0","master500_04_1","master500_04_2",
                               "master500_04_3","master500_04_4","master500_04_5",
                               "master500_08_0","master500_08_1","master500_08_2",
                               "master500_08_3","master500_08_4","master500_08_5",
                               "master500_12_0","master500_12_1");
    
       int k;
       for (k=1;k<=dynlen(dHV_Master);k++)
       { 
         int i;
         for (i=1;i<=8;i++)  //Create dp fct for HV master channel
         {
           HV_master_dp_fct_setup(dHV_Master[k],i);   
         } 
       }
} 
//----FED archiving dp fct create----- 
void Create_archiving_dp_fct_FED(string sCSC_FED)
{   
   
    dyn_string dsFED_DDUs;
          dsFED_DDUs = makeDynString(sCSC_FED+"_DDU01",sCSC_FED+"_DDU02",sCSC_FED+"_DDU03",
                                     sCSC_FED+"_DDU04",sCSC_FED+"_DDU05",sCSC_FED+"_DDU06",
                                     sCSC_FED+"_DDU07",sCSC_FED+"_DDU08",sCSC_FED+"_DDU09",
                                     sCSC_FED+"_DDU10",sCSC_FED+"_DDU11",sCSC_FED+"_DDU12",
                                     sCSC_FED+"_DDU13",sCSC_FED+"_DDU14",sCSC_FED+"_DDU15",
                                     sCSC_FED+"_DDU16",sCSC_FED+"_DDU17",sCSC_FED+"_DDU18",
                                     sCSC_FED+"_DDU19",sCSC_FED+"_DDU20",sCSC_FED+"_DDU21",
                                     sCSC_FED+"_DDU22",sCSC_FED+"_DDU23",sCSC_FED+"_DDU24",
                                     sCSC_FED+"_DDU25",sCSC_FED+"_DDU26",sCSC_FED+"_DDU27",
                                     sCSC_FED+"_DDU28",sCSC_FED+"_DDU29",sCSC_FED+"_DDU30",
                                     sCSC_FED+"_DDU31",sCSC_FED+"_DDU32",sCSC_FED+"_DDU33",
                                     sCSC_FED+"_DDU34",sCSC_FED+"_DDU35",sCSC_FED+"_DDU36");
    
 /*   
          dsFED_DDUs = makeDynString("DDU01","DDU02","DDU03","DDU04","DDU05","DDU06",
                                     "DDU07","DDU08","DDU09","DDU10","DDU11","DDU12",
                                     "DDU13","DDU14","DDU15","DDU16","DDU17","DDU18",
                                     "DDU19","DDU20","DDU21","DDU22","DDU23","DDU24",
                                     "DDU25","DDU26","DDU27","DDU28","DDU29","DDU30",
                                     "DDU31","DDU32","DDU33","DDU34","DDU35","DDU36");
*/                                     
       //DebugN(dsFED_DDUs[1]);     
       int k;
       for (k=1;k<=dynlen(dsFED_DDUs);k++)
       { 
           FED_DDU_dp_fct_setup(dsFED_DDUs[k]);    //create FED DDU dp
       }
}   
//----for chamber HV dp setup------------------------
void Chamber_HV_dp_setup(string sCSC_Chamber,int i)
{         
         string channel;
         if (i < 10)
           channel = "_V0";
         else if (i > 9)
           channel = "_V"; 
         string sdpnameHV_I,sdpnameHV_V,sdptypeHV_I,sdptypeHV_V;
         sdpnameHV_V = sCSC_Chamber+"_HV"+channel+i+"_VMON";
         sdpnameHV_I = sCSC_Chamber+"_HV"+channel+i+"_IMON";
         sdptypeHV_V = "CSC_HV_V_DATA";
         sdptypeHV_I = "CSC_HV_I_DATA";
         dpCreate(sdpnameHV_V,sdptypeHV_V);
         HV_V_dp_config_setup(sdpnameHV_V);
         dpCreate(sdpnameHV_I,sdptypeHV_I);
         HV_I_dp_config_setup(sdpnameHV_I);
} 
//----for HV master dp setup ------------------------
void HV_master_dp_config_setup(string sHV_Master,int i)
{         
         
         string sdpnameHV_MV,sdpnameHV_MI,sdptypeHV_M;
         sdpnameHV_MV = sHV_Master+"_V"+i+"_VMON";
         sdpnameHV_MI = sHV_Master+"_V"+i+"_IMON";
         sdptypeHV_M = "CSC_HV_M_DATA";
         dpCreate(sdpnameHV_MV,sdptypeHV_M);
         HV_V_dp_config_setup(sdpnameHV_MV);
         dpCreate(sdpnameHV_MI,sdptypeHV_M);
         HV_I_dp_config_setup(sdpnameHV_MI);
}                   
//---chamber LV cfeb dp setup------------------------
void Chamber_LV_Cfeb_dp_setup(string sCSC_Chamber,int i)    
{
         string sdpnameLV_c_C33,sdpnameLV_c_V33;
         string sdpnameLV_c_C50,sdpnameLV_c_V50;
         string sdpnameLV_c_C60,sdpnameLV_c_V60;
         string sdptypeLV_I="CSC_LV_I_DATA";
         string sdptypeLV_V="CSC_LV_V_DATA";
//for Cfeb v33
        sdpnameLV_c_V33 = sCSC_Chamber+"_LV_Cfeb33_V"+i;
        sdpnameLV_c_C33 = sCSC_Chamber+"_LV_Cfeb33_C"+i;
        dpCreate(sdpnameLV_c_V33,sdptypeLV_V);
        LV_dp_config_setup(sdpnameLV_c_V33);        
        dpCreate(sdpnameLV_c_C33,sdptypeLV_I);
        LV_dp_config_setup(sdpnameLV_c_C33);
        
  //for Cfeb v50
      sdpnameLV_c_V50 = sCSC_Chamber+"_LV_Cfeb50_V"+i;
      sdpnameLV_c_C50 = sCSC_Chamber+"_LV_Cfeb50_C"+i;
      dpCreate(sdpnameLV_c_V50,sdptypeLV_V);
      LV_dp_config_setup(sdpnameLV_c_V50);
      dpCreate(sdpnameLV_c_C50,sdptypeLV_I);
      LV_dp_config_setup(sdpnameLV_c_C50);
  //for Cfeb v60
      sdpnameLV_c_V60 = sCSC_Chamber+"_LV_Cfeb60_V"+i;
      sdpnameLV_c_C60 = sCSC_Chamber+"_LV_Cfeb60_C"+i;
      dpCreate(sdpnameLV_c_V60,sdptypeLV_V);
      LV_dp_config_setup(sdpnameLV_c_V60);
      dpCreate(sdpnameLV_c_C60,sdptypeLV_I);
      LV_dp_config_setup(sdpnameLV_c_C60);
} 

//---chamber LV Alct dp setup------------------
void Chamber_LV_Alct_dp_setup(string sCSC_Chamber) 
{
      string sdpnameLV_a_C18,sdpnameLV_a_V18;
      string sdpnameLV_a_C33,sdpnameLV_a_V33;
      string sdpnameLV_a_C55,sdpnameLV_a_V55;
      string sdpnameLV_a_C56,sdpnameLV_a_V56;
      string sdptypeLV_I="CSC_LV_I_DATA";
      string sdptypeLV_V="CSC_LV_V_DATA";
      sdpnameLV_a_V18 = sCSC_Chamber+"_LV_Alct18_V1";
      sdpnameLV_a_C18 = sCSC_Chamber+"_LV_Alct18_C1"; 
      sdpnameLV_a_V33 = sCSC_Chamber+"_LV_Alct33_V1";
      sdpnameLV_a_C33 = sCSC_Chamber+"_LV_Alct33_C1";
      sdpnameLV_a_V55 = sCSC_Chamber+"_LV_Alct55_V1";
      sdpnameLV_a_C55 = sCSC_Chamber+"_LV_Alct55_C1"; 
      sdpnameLV_a_V56 = sCSC_Chamber+"_LV_Alct56_V1";
      sdpnameLV_a_C56 = sCSC_Chamber+"_LV_Alct56_C1";                               
      dpCreate(sdpnameLV_a_V18,sdptypeLV_V);
      LV_dp_config_setup(sdpnameLV_a_V18);
      dpCreate(sdpnameLV_a_C18,sdptypeLV_I);
      LV_dp_config_setup(sdpnameLV_a_C18);
      dpCreate(sdpnameLV_a_V33,sdptypeLV_V);
      LV_dp_config_setup(sdpnameLV_a_V33);
      dpCreate(sdpnameLV_a_C33,sdptypeLV_I);
      LV_dp_config_setup(sdpnameLV_a_C33);
      dpCreate(sdpnameLV_a_V55,sdptypeLV_V);
      LV_dp_config_setup(sdpnameLV_a_V55);
      dpCreate(sdpnameLV_a_C55,sdptypeLV_I);
      LV_dp_config_setup(sdpnameLV_a_C55);
      dpCreate(sdpnameLV_a_V56,sdptypeLV_V);
      LV_dp_config_setup(sdpnameLV_a_V56);
      dpCreate(sdpnameLV_a_C56,sdptypeLV_I);
      LV_dp_config_setup(sdpnameLV_a_C56);  
} 
//--chamber Temp alct and dmb dp setup---
void Chamber_TEMP_AD_dp_setup(string sCSC_Chamber)
{
      string sdpnameTEMP_Alct,sdpnameTEMP_Dmb; 
      string sdptypeTEMP;
      sdpnameTEMP_Alct = sCSC_Chamber+"_TEMP_Alct_T1";
      sdpnameTEMP_Dmb = sCSC_Chamber+"_TEMP_Dmb__T1"; 
      sdptypeTEMP = "CSC_TEMP_DATA";
      dpCreate(sdpnameTEMP_Alct,sdptypeTEMP);
      TEMP_dp_config_setup(sdpnameTEMP_Alct);
      dpCreate(sdpnameTEMP_Dmb,sdptypeTEMP);
      TEMP_dp_config_setup(sdpnameTEMP_Dmb);
}

//--chamber Temp Cfeb dp setup---  
void Chamber_TEMP_Cfeb_dp_setup(string sCSC_Chamber,int i)
{    
        string  sdpnameTEMP_Cfeb,sdptypeTEMP;  
        sdpnameTEMP_Cfeb = sCSC_Chamber+"_TEMP_Cfeb_T"+i;
        sdptypeTEMP = "CSC_TEMP_DATA";
        dpCreate(sdpnameTEMP_Cfeb,sdptypeTEMP);
        TEMP_dp_config_setup(sdpnameTEMP_Cfeb);
}        

//---FED_DDU dp setup------------------
void FED_DDU_dp_setup(string sDDU) 
{
  //FED DDU Voltage
      string sdpnameDDU_V15A;
      string sdpnameDDU_V25A;
      string sdpnameDDU_V25B;
      string sdpnameDDU_V33A;
      string sdptypeFED_V="CSC_FED_V_DATA";
     
      sdpnameDDU_V15A = sDDU+"_V15A";
      sdpnameDDU_V25A = sDDU+"_V25A";
      sdpnameDDU_V25B = sDDU+"_V25B";
      sdpnameDDU_V33A = sDDU+"_V33A";
                                
      dpCreate(sdpnameDDU_V15A,sdptypeFED_V);
      FED_DDU_V_dp_config_setup(sdpnameDDU_V15A);
      dpCreate(sdpnameDDU_V25A,sdptypeFED_V);
      FED_DDU_V_dp_config_setup(sdpnameDDU_V25A);
      dpCreate(sdpnameDDU_V25B,sdptypeFED_V);
      FED_DDU_V_dp_config_setup(sdpnameDDU_V25B);
      dpCreate(sdpnameDDU_V33A,sdptypeFED_V);
      FED_DDU_V_dp_config_setup(sdpnameDDU_V33A);
      
      //FED DDU temp
      string sdpnameDDU_TD1;
      string sdpnameDDU_TD2;
      string sdpnameDDU_TD3;
      string sdpnameDDU_TD4;
      string sdptypeFED_T="CSC_FED_T_DATA";
     
      sdpnameDDU_TD1 = sDDU+"_TD1";
      sdpnameDDU_TD2 = sDDU+"_TD2";
      sdpnameDDU_TD3 = sDDU+"_TD3";
      sdpnameDDU_TD4 = sDDU+"_TD4";
                                
      dpCreate(sdpnameDDU_TD1,sdptypeFED_T);
      FED_DDU_T_dp_config_setup(sdpnameDDU_TD1);
      dpCreate(sdpnameDDU_TD2,sdptypeFED_T);
      FED_DDU_T_dp_config_setup(sdpnameDDU_TD2);
      dpCreate(sdpnameDDU_TD3,sdptypeFED_T);
      FED_DDU_T_dp_config_setup(sdpnameDDU_TD3);
      dpCreate(sdpnameDDU_TD4,sdptypeFED_T);
      FED_DDU_T_dp_config_setup(sdpnameDDU_TD4);
            
}    
//---set dp config for HV_V-----------------------------  
void HV_V_dp_config_setup(string sDPname)
{
    dpSetWait (sDPname + ".value:_archive.._type",45,
               sDPname + ".value:_archive.._archive",1,
               sDPname + ".value:_archive.1._type",3,
               sDPname + ".value:_archive.1._class","_EVENT",
               sDPname + ".value:_archive.1._interv",0,
               sDPname + ".value:_archive.1._interv_type",0,
               sDPname + ".value:_archive.1._std_type",2,
               sDPname + ".value:_archive.1._std_tol",20,
               sDPname + ".value:_archive.1._std_time",7200);
       
}
//--- set dp config for HV_I-------------------------------
void HV_I_dp_config_setup(string sDPname)
{
    dpSetWait (sDPname + ".value:_archive.._type",45,
               sDPname + ".value:_archive.._archive",1,
               sDPname + ".value:_archive.1._type",3,
               sDPname + ".value:_archive.1._class","_EVENT",
               sDPname + ".value:_archive.1._interv",0,
               sDPname + ".value:_archive.1._interv_type",0,
               sDPname + ".value:_archive.1._std_type",2,
               sDPname + ".value:_archive.1._std_tol",1,
               sDPname + ".value:_archive.1._std_time",7200);
       
}            
//--- set dp config for LV---------------------------------
void LV_dp_config_setup(string sDPname)
{
    dpSetWait (sDPname + ".value:_archive.._type",45,
               sDPname + ".value:_archive.._archive",1,
               sDPname + ".value:_archive.1._type",3,
               sDPname + ".value:_archive.1._class","_EVENT",
               sDPname + ".value:_archive.1._interv",0,
               sDPname + ".value:_archive.1._interv_type",0,
               sDPname + ".value:_archive.1._std_type",2,
               sDPname + ".value:_archive.1._std_tol",0.01,
               sDPname + ".value:_archive.1._std_time",28800);
       
}            
//--- set dp config for Temp---------------------------------
void TEMP_dp_config_setup(string sDPname)
{
    dpSetWait (sDPname + ".value:_archive.._type",45,
               sDPname + ".value:_archive.._archive",1,
               sDPname + ".value:_archive.1._type",3,
               sDPname + ".value:_archive.1._class","_EVENT",
               sDPname + ".value:_archive.1._interv",0,
               sDPname + ".value:_archive.1._interv_type",0,
               sDPname + ".value:_archive.1._std_type",2,
               sDPname + ".value:_archive.1._std_tol",0.1,
               sDPname + ".value:_archive.1._std_time",28800);
       
}
//--- set dp config for FED DDU voltage---------------------------------
void FED_DDU_V_dp_config_setup(string sDPname)
{
    dpSetWait (sDPname + ".value:_archive.._type",45,
               sDPname + ".value:_archive.._archive",1,
               sDPname + ".value:_archive.1._type",3,
               sDPname + ".value:_archive.1._class","_EVENT",
               sDPname + ".value:_archive.1._interv",0,
               sDPname + ".value:_archive.1._interv_type",0,
               sDPname + ".value:_archive.1._std_type",2,
               sDPname + ".value:_archive.1._std_tol",0.01,
               sDPname + ".value:_archive.1._std_time",28800);
       
}   

//--- set dp config for FED DDU temp---------------------------------
void FED_DDU_T_dp_config_setup(string sDPname)
{
    dpSetWait (sDPname + ".value:_archive.._type",45,
               sDPname + ".value:_archive.._archive",1,
               sDPname + ".value:_archive.1._type",3,
               sDPname + ".value:_archive.1._class","_EVENT",
               sDPname + ".value:_archive.1._interv",0,
               sDPname + ".value:_archive.1._interv_type",0,
               sDPname + ".value:_archive.1._std_type",2,
               sDPname + ".value:_archive.1._std_tol",0.1,
               sDPname + ".value:_archive.1._std_time",28800);
       
}
  
//------HV dp_fct setup---------------
void HV_dp_fct_setup(string sdpname,string stype,int i)
{  
   int j; 
   if (stype == "type1")
       j=0;
   else if (stype == "type2")
       j=18;
   int n = j+i;   
   string channel;
   if (i < 10)
          channel = "_V0";
   else if (i > 9)
          channel = "_V";   
   string sdpnameNew,p1; 
   dyn_string ds; 
   sdpnameNew = sdpname+"_HV";
   p1=sdpnameNew+".data.v"+n+".imon:_original.._value";
   ds=makeDynString(p1); 
   
   dpSetWait(sdpnameNew+channel+i+"_IMON.value:_dp_fct.._type", 60, 
             sdpnameNew+channel+i+"_IMON.value:_dp_fct.._param", ds, 
             sdpnameNew+channel+i+"_IMON.value:_dp_fct.._fct", "p1"); 
   
   p1=sdpnameNew+".data.v"+n+".vmon:_original.._value";
   ds=makeDynString(p1);
   
   dpSetWait(sdpnameNew+channel+i+"_VMON.value:_dp_fct.._type", 60, 
             sdpnameNew+channel+i+"_VMON.value:_dp_fct.._param", ds, 
             sdpnameNew+channel+i+"_VMON.value:_dp_fct.._fct", "p1");   
      
 }
//------HV Master dp_fct setup ------------------------------
void HV_master_dp_fct_setup(string sdpname,int i)
{  
   dyn_string dsNew;
   string sNew,sOld;
   int iNew;
   dsNew = strsplit(sdpname,"_");
   sNew  = dsNew[2];
   iNew  = sNew;   //remove 0 from 02
   sOld  = iNew;
   
   string sdpnameNew,sdpnameOld,p1;
   dyn_string ds; 
   sdpnameNew = sdpname+"_V";
   sdpnameOld = dsNew[1]+"_"+sOld+"_"+dsNew[3];
   p1=sdpnameOld+".data.v"+i+".imon:_original.._value";
   ds=makeDynString(p1); 
   
   dpSetWait(sdpnameNew+i+"_IMON.value:_dp_fct.._type", 60, 
             sdpnameNew+i+"_IMON.value:_dp_fct.._param", ds, 
             sdpnameNew+i+"_IMON.value:_dp_fct.._fct", "p1"); 
   
   p1=sdpnameOld+".data.v"+i+".vmon:_original.._value";
   ds=makeDynString(p1);
   
   dpSetWait(sdpnameNew+i+"_VMON.value:_dp_fct.._type", 60, 
             sdpnameNew+i+"_VMON.value:_dp_fct.._param", ds, 
             sdpnameNew+i+"_VMON.value:_dp_fct.._fct", "p1");   
      
 }
//------LV cfeb dp_fct setup---------------
void LV_cfeb_dp_fct_setup(string sdpname,int i)
{
   string sdpnameNew,p1; 
   dyn_string ds; 
   sdpnameNew = sdpname+"_LV";
   //for v33
   p1=sdpnameNew+".data.Cfeb_o.c33.v"+i+":_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Cfeb33_C"+i+".value:_dp_fct.._type", 60, 
             sdpnameNew+"_Cfeb33_C"+i+".value:_dp_fct.._param", ds, 
             sdpnameNew+"_Cfeb33_C"+i+".value:_dp_fct.._fct", "p1"); 
   
   p1=sdpnameNew+".data.Cfeb_o.v33.v"+i+":_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Cfeb33_V"+i+".value:_dp_fct.._type", 60, 
             sdpnameNew+"_Cfeb33_V"+i+".value:_dp_fct.._param", ds, 
             sdpnameNew+"_Cfeb33_V"+i+".value:_dp_fct.._fct", "p1");
   //for v50
   p1=sdpnameNew+".data.Cfeb_o.c50.v"+i+":_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Cfeb50_C"+i+".value:_dp_fct.._type", 60, 
             sdpnameNew+"_Cfeb50_C"+i+".value:_dp_fct.._param", ds, 
             sdpnameNew+"_Cfeb50_C"+i+".value:_dp_fct.._fct", "p1"); 
   
   p1=sdpnameNew+".data.Cfeb_o.v50.v"+i+":_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Cfeb50_V"+i+".value:_dp_fct.._type", 60, 
             sdpnameNew+"_Cfeb50_V"+i+".value:_dp_fct.._param", ds, 
             sdpnameNew+"_Cfeb50_V"+i+".value:_dp_fct.._fct", "p1");
   //for v60
   p1=sdpnameNew+".data.Cfeb_o.c60.v"+i+":_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Cfeb60_C"+i+".value:_dp_fct.._type", 60, 
             sdpnameNew+"_Cfeb60_C"+i+".value:_dp_fct.._param", ds, 
             sdpnameNew+"_Cfeb60_C"+i+".value:_dp_fct.._fct", "p1"); 
   
   p1=sdpnameNew+".data.Cfeb_o.v60.v"+i+":_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Cfeb60_V"+i+".value:_dp_fct.._type", 60, 
             sdpnameNew+"_Cfeb60_V"+i+".value:_dp_fct.._param", ds, 
             sdpnameNew+"_Cfeb60_V"+i+".value:_dp_fct.._fct", "p1");
} 

//------LV alct dp_fct setup---------------
void LV_alct_dp_fct_setup(string sdpname)
{
  //for alct
   string sdpnameNew,p1; 
   dyn_string ds; 
   sdpnameNew = sdpname+"_LV";
   p1=sdpnameNew+".data.Alct_o.c18:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Alct18_C1.value:_dp_fct.._type", 60, 
             sdpnameNew+"_Alct18_C1.value:_dp_fct.._param", ds, 
             sdpnameNew+"_Alct18_C1.value:_dp_fct.._fct", "p1"); 
   
   p1=sdpnameNew+".data.Alct_o.v18:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Alct18_V1.value:_dp_fct.._type", 60, 
             sdpnameNew+"_Alct18_V1.value:_dp_fct.._param", ds, 
             sdpnameNew+"_Alct18_V1.value:_dp_fct.._fct", "p1");
   
   p1=sdpnameNew+".data.Alct_o.c33:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Alct33_C1.value:_dp_fct.._type", 60, 
             sdpnameNew+"_Alct33_C1.value:_dp_fct.._param", ds, 
             sdpnameNew+"_Alct33_C1.value:_dp_fct.._fct", "p1"); 
   
   p1=sdpnameNew+".data.Alct_o.v33:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Alct33_V1.value:_dp_fct.._type", 60, 
             sdpnameNew+"_Alct33_V1.value:_dp_fct.._param", ds, 
             sdpnameNew+"_Alct33_V1.value:_dp_fct.._fct", "p1");
   
   p1=sdpnameNew+".data.Alct_o.c55:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Alct55_C1.value:_dp_fct.._type", 60, 
             sdpnameNew+"_Alct55_C1.value:_dp_fct.._param", ds, 
             sdpnameNew+"_Alct55_C1.value:_dp_fct.._fct", "p1"); 
   
   p1=sdpnameNew+".data.Alct_o.v55:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Alct55_V1.value:_dp_fct.._type", 60, 
             sdpnameNew+"_Alct55_V1.value:_dp_fct.._param", ds, 
             sdpnameNew+"_Alct55_V1.value:_dp_fct.._fct", "p1");
   
   p1=sdpnameNew+".data.Alct_o.c56:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Alct56_C1.value:_dp_fct.._type", 60, 
             sdpnameNew+"_Alct56_C1.value:_dp_fct.._param", ds, 
             sdpnameNew+"_Alct56_C1.value:_dp_fct.._fct", "p1"); 
   
   p1=sdpnameNew+".data.Alct_o.v56:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Alct56_V1.value:_dp_fct.._type", 60, 
             sdpnameNew+"_Alct56_V1.value:_dp_fct.._param", ds, 
             sdpnameNew+"_Alct56_V1.value:_dp_fct.._fct", "p1"); 
}  

//------Temp cfeb dp_fct setup---------------
void TEMP_cfeb_dp_fct_setup(string sdpname,int i)
{
  //for cfeb
   string sdpnameNew,p1; 
   dyn_string ds; 
   sdpnameNew = sdpname+"_TEMP";
   p1=sdpnameNew+".data.t_cfeb.v"+i+":_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Cfeb_T"+i+".value:_dp_fct.._type", 60, 
             sdpnameNew+"_Cfeb_T"+i+".value:_dp_fct.._param", ds, 
             sdpnameNew+"_Cfeb_T"+i+".value:_dp_fct.._fct", "p1"); 
 }
//------Temp alct and dmb dp_fct setup---------------  
void TEMP_alct_dmb_dp_fct_setup(string sdpname) 
{   
   string sdpnameNew,p1; 
   dyn_string ds; 
   sdpnameNew = sdpname+"_TEMP"; 
   p1=sdpnameNew+".data.t_alct.v1:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Alct_T1.value:_dp_fct.._type", 60, 
             sdpnameNew+"_Alct_T1.value:_dp_fct.._param", ds, 
             sdpnameNew+"_Alct_T1.value:_dp_fct.._fct", "p1");
   
   p1=sdpnameNew+".data.t_dmb.v1:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sdpnameNew+"_Dmb__T1.value:_dp_fct.._type", 60, 
             sdpnameNew+"_Dmb__T1.value:_dp_fct.._param", ds, 
             sdpnameNew+"_Dmb__T1.value:_dp_fct.._fct", "p1"); 
} 
//------FED DDU dp_fct setup---------------
void FED_DDU_dp_fct_setup(string sDDU)
{
  
   string sdpnameNew,p1; 
   dyn_string ds_dpname,ds; 
   ds_dpname = strsplit(sDDU,"_");   //split CSC_FED_DDU01
   sdpnameNew = ds_dpname[3]+"_FED";    // get DDU01_FED
   
   p1=sdpnameNew+".data.v15:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sDDU+"_V15A.value:_dp_fct.._type", 60, 
             sDDU+"_V15A.value:_dp_fct.._param", ds, 
             sDDU+"_V15A.value:_dp_fct.._fct", "p1"); 
   
   p1=sdpnameNew+".data.v25a:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sDDU+"_V25A.value:_dp_fct.._type", 60, 
             sDDU+"_V25A.value:_dp_fct.._param", ds, 
             sDDU+"_V25A.value:_dp_fct.._fct", "p1"); 
   
   p1=sdpnameNew+".data.v25b:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sDDU+"_V25B.value:_dp_fct.._type", 60, 
             sDDU+"_V25B.value:_dp_fct.._param", ds, 
             sDDU+"_V25B.value:_dp_fct.._fct", "p1");
   
   p1=sdpnameNew+".data.v33:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sDDU+"_V33A.value:_dp_fct.._type", 60, 
             sDDU+"_V33A.value:_dp_fct.._param", ds, 
             sDDU+"_V33A.value:_dp_fct.._fct", "p1");
   
   p1=sdpnameNew+".data.td1:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sDDU+"_TD1.value:_dp_fct.._type", 60, 
             sDDU+"_TD1.value:_dp_fct.._param", ds, 
             sDDU+"_TD1.value:_dp_fct.._fct", "p1");
   
   p1=sdpnameNew+".data.td2:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sDDU+"_TD2.value:_dp_fct.._type", 60, 
             sDDU+"_TD2.value:_dp_fct.._param", ds, 
             sDDU+"_TD2.value:_dp_fct.._fct", "p1");
   
   p1=sdpnameNew+".data.td3:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sDDU+"_TD3.value:_dp_fct.._type", 60, 
             sDDU+"_TD3.value:_dp_fct.._param", ds, 
             sDDU+"_TD3.value:_dp_fct.._fct", "p1");
   
   p1=sdpnameNew+".data.td4:_original.._value";
   ds=makeDynString(p1);   
   dpSetWait(sDDU+"_TD4.value:_dp_fct.._type", 60, 
             sDDU+"_TD4.value:_dp_fct.._param", ds, 
             sDDU+"_TD4.value:_dp_fct.._fct", "p1");
   
}
