// Constant Definitions for emu dcs LV_MRTN
//---------------------------------------------------------------------------------------------------------------------------------------------------
const string TopFsmObjType = "CSC_LV_NODES";
const string StationFsmObjType = "CSC_LV_TREE_NODES";
const string CrateFsmObjType = "CSC_LV_TREE_NODES";
const string MRTNCrateFsmDevType = "FwWienerMarathon";
const string MRTNChannelFsmDevType = "FwWienerMarathonChannel";
const int    iTimeMarathon = 7200;    //time interval for Marathon
const float  fTolMarathon  = 1;       //deadband for Marathon
//---------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * return a dyn_string 
 */ 
dyn_string emuLvMRTN_getDynString(string sConstantName)
{
  dyn_string dsConstant;
  if(sConstantName == "MRTN_CanBus_P5") 
   { 
        dsConstant = makeDynString("CAN4","CAN5","CAN6","CAN7");
   }                                               
  if(sConstantName == "MRTN_CanBus_B904")
  {  
        dsConstant = makeDynString("CAN0");
  }
  if(sConstantName == "MRTN_Channels")
  {  
       dsConstant = makeDynString("0","1","3","4","5","7");                                            
  }
  if(sConstantName == "Db_MRTN_B904")
  {  
        dsConstant = makeDynString("DEV_CR1;10");
  }      
  if(sConstantName == "Db_MRTN_Geog_B904")
  {  
        dsConstant = makeDynString("DEV Bld");
  }
  if(sConstantName == "Db_MRTN_P5")
  { 
       dsConstant = makeDynString("M1_CR1;31",  "M1_CR2;32",  "M1_CR3;33",   //X4 near
                                  "M1_CR4;34",  "M1_CR5;35",  "M1_CR6;36",   //X4 far
                                  "M1_CR7;37",  "M1_CR8;38",  "M1_CR9;39",   //X2 far
                                  "M1_CR10;40", "M1_CR11;41", "M1_CR12;42", //X2 near
                                  "M2_CR1;43",  "M2_CR2;44",  //X4 near
                                  "M2_CR3;45",  "M2_CR4;46",  //X4 far
                                  "M2_CR5;47",  "M2_CR6;48",  //X2 far
                                  "M2_CR7;49",  "M2_CR8;50",  //X2 near
                                  "M4_CR1;51",   //X4 near
                                  "M4_CR2;52",   //X4 far
                                  "M4_CR3;53",   //X2 far
                                  "M4_CR4;54",   //X2 near
                                  "M4_CR5;",  "M4_CR6;",  "M4_CR7;",  "M4_CR8;",
                                    
                                  "P1_CR1;1",   "P1_CR2;2",   "P1_CR3;3",   //X4 near
                                  "P1_CR4;4",   "P1_CR5;5",   "P1_CR6;6",   //X4 far
                                  "P1_CR7;7",   "P1_CR8;8",   "P1_CR9;9",   //X2 far
                                  "P1_CR10;10", "P1_CR11;11", "P1_CR12;12", //X2 near
                                  "P2_CR1;13",  "P2_CR2;14",  //X4 near
                                  "P2_CR3;15",  "P2_CR4;16",  //X4 far
                                  "P2_CR5;17",  "P2_CR6;18",  //X2 far
                                  "P2_CR7;19",  "P2_CR8;20",  //X2 near
                                  "P4_CR1;21",   //X4 near
                                  "P4_CR2;22",   //X4 far
                                  "P4_CR3;23",   //X2 far
                                  "P4_CR4;24",   //X2 near
                                  "P4_CR5;",  "P4_CR6;",  "P4_CR7;",  "P4_CR8;");
  }
  if(sConstantName == "Db_MRTN_Geog_P5")
  {                               // !!! MINUS SIDE RACKS ARE NOT CORRECT (just a copy of plus side for now)
    dsConstant = makeDynString("X4J31_L","X4J31_M","X4J31_U",  //X4 near
                               "X4A31_U","X4A31_M","X4A31_L",  //X4 far
                               "X2A33_U","X2A33_M","X2A33_L",  //X2 far
                               "X2J31_L","X2J31_M","X2J31_U",  //X2 near
                               "X4J41_L","X4J41_U",  //X4 near
                               "X4A41_U","X4A41_L",  //X4 far
                               "X2A41_U","X2A41_L",  //X2 far
                               "X2J42_L","X2J42_U",  //X2 near
                               "X4J51",  //X4 near
                               "X4A51",  //X4 far
                               "X2A52",  //X2 far
                               "X2J52",  //X2 near
                               "","","","",
                               "X4J31_L","X4J31_M","X4J31_U",  //X4 near
                               "X4A31_U","X4A31_M","X4A31_L",  //X4 far
                               "X2A33_U","X2A33_M","X2A33_L",  //X2 far
                               "X2J31_L","X2J31_M","X2J31_U",  //X2 near
                               "X4J41_L","X4J41_U",  //X4 near
                               "X4A41_U","X4A41_L",  //X4 far
                               "X2A41_U","X2A41_L",  //X2 far
                               "X2J42_L","X2J42_U",  //X2 near
                               "X4J51",  //X4 near
                               "X4A51",  //X4 far
                               "X2A52",  //X2 far
                               "X2J52",  //X2 near
                               "","","","");
  }  
  if(sConstantName == "DpType_Delete")
  {  
    dsConstant = makeDynString("Db_MRTN","CSC_MARATON_V_DATA","CSC_MARATON_I_DATA","CSC_MARATON_T_DATA",
                               "FwWienerMarathonChannel","FwWienerMarathon",
                               "FwWienerChannel","FwWienerPS","FwWienerCanBus");
  } 

  if(sConstantName == "MRTN_Station_Nodes")
  { 
    dsConstant = makeDynString("CSC_ME_M1_LV_MRTN","CSC_ME_M2_LV_MRTN","CSC_ME_M3_LV_MRTN","CSC_ME_M4_LV_MRTN",
                               "CSC_ME_P1_LV_MRTN","CSC_ME_P2_LV_MRTN","CSC_ME_P3_LV_MRTN","CSC_ME_P4_LV_MRTN");
  }
  if(sConstantName == "CSC_ME_M1_LV_MRTN")
  {
    dsConstant = makeDynString("CSC_ME_M1_CR1_LV_MRTN", "CSC_ME_M1_CR2_LV_MRTN", "CSC_ME_M1_CR3_LV_MRTN",
                               "CSC_ME_M1_CR4_LV_MRTN", "CSC_ME_M1_CR5_LV_MRTN", "CSC_ME_M1_CR6_LV_MRTN",
                               "CSC_ME_M1_CR7_LV_MRTN", "CSC_ME_M1_CR8_LV_MRTN", "CSC_ME_M1_CR9_LV_MRTN",
                               "CSC_ME_M1_CR10_LV_MRTN","CSC_ME_M1_CR11_LV_MRTN","CSC_ME_M1_CR12_LV_MRTN");
  } 
  if(sConstantName == "CSC_ME_M2_LV_MRTN")
  {
    dsConstant = makeDynString("CSC_ME_M2_CR1_LV_MRTN","CSC_ME_M2_CR2_LV_MRTN","CSC_ME_M2_CR3_LV_MRTN","CSC_ME_M2_CR4_LV_MRTN",
                               "CSC_ME_M2_CR5_LV_MRTN","CSC_ME_M2_CR6_LV_MRTN","CSC_ME_M2_CR7_LV_MRTN","CSC_ME_M2_CR8_LV_MRTN");                                  
  } 
  if(sConstantName == "CSC_ME_M3_LV_MRTN")
  {
    dsConstant = makeDynString();                                  
  }
  if(sConstantName == "CSC_ME_M4_LV_MRTN")
  {
    dsConstant = makeDynString("CSC_ME_M4_CR1_LV_MRTN","CSC_ME_M4_CR2_LV_MRTN", "CSC_ME_M4_CR3_LV_MRTN","CSC_ME_M4_CR4_LV_MRTN");                                  
  } 
  if(sConstantName == "CSC_ME_P1_LV_MRTN")
  {
    dsConstant = makeDynString("CSC_ME_P1_CR1_LV_MRTN", "CSC_ME_P1_CR2_LV_MRTN", "CSC_ME_P1_CR3_LV_MRTN",
                               "CSC_ME_P1_CR4_LV_MRTN", "CSC_ME_P1_CR5_LV_MRTN", "CSC_ME_P1_CR6_LV_MRTN",
                               "CSC_ME_P1_CR7_LV_MRTN", "CSC_ME_P1_CR8_LV_MRTN", "CSC_ME_P1_CR9_LV_MRTN",
                               "CSC_ME_P1_CR10_LV_MRTN","CSC_ME_P1_CR11_LV_MRTN","CSC_ME_P1_CR12_LV_MRTN");
  }
  if(sConstantName == "CSC_ME_P2_LV_MRTN")
  {
    dsConstant = makeDynString("CSC_ME_P2_CR1_LV_MRTN","CSC_ME_P2_CR2_LV_MRTN","CSC_ME_P2_CR3_LV_MRTN","CSC_ME_P2_CR4_LV_MRTN",
                               "CSC_ME_P2_CR5_LV_MRTN","CSC_ME_P2_CR6_LV_MRTN","CSC_ME_P2_CR7_LV_MRTN","CSC_ME_P2_CR8_LV_MRTN");                              
  } 
  if(sConstantName == "CSC_ME_P3_LV_MRTN")
  {
    dsConstant = makeDynString();                                  
  }
  if(sConstantName == "CSC_ME_P4_LV_MRTN")
  {
    dsConstant = makeDynString("CSC_ME_P4_CR1_LV_MRTN","CSC_ME_P4_CR2_LV_MRTN","CSC_ME_P4_CR3_LV_MRTN","CSC_ME_P4_CR4_LV_MRTN");                                
  } 
  if(sConstantName == "MRTN_Channel_Alert_Status")
  {
    dsConstant = makeDynString("FailureMaxTemperature",        "FailureMaxCurrent",
                               "FailureMaxSenseVoltage","FailureMaxTerminalVoltage");                                
  } 
  if(sConstantName == "MRTN_Crate_Status_Alerts")
  {
    dsConstant = makeDynString("OutputFailure", "InputFailure",
                               "SystemFailure", "SensorFailure");                                
  } 
  if(sConstantName == "MRTN_Channel_Archiving_Status")
  {
    dsConstant = makeDynString("On","FailureMaxTemperature","FailureMaxCurrent",
                               "FailureMaxSenseVoltage","FailureMaxTerminalVoltage",
                               "FailureMinSenseVoltage");                                
  }
  if(sConstantName == "MRTN_Crate_Archiving_Status")
  {
    dsConstant = makeDynString("On","OutputFailure","InputFailure",
                               "SystemFailure","SensorFailure","Inhibit",
                               "WriteProtect");                                
  }  
  if(sConstantName == "MRTN_Channel_Alert_Texts")
  {
    dsConstant = makeDynString("OK","STATUS ERROR");                                
  } 
  if(sConstantName == "MRTN_Crate_Alert_Texts")
  {
    dsConstant = makeDynString("Maraton PSU OK","Maraton PSU Error");                                
  } 
  return dsConstant;
}  
/**
 * return a dyn_int 
 */ 
dyn_int emuLvMRTN_getDynInt(string sConstantName)
{    
  dyn_int diConstant; 
  return diConstant;
}    
/**
 * return a dyn_dyn_int 
 */ 
dyn_dyn_int emuLvMRTN_getDynDynInt(string sConstantName)
{
  dyn_dyn_int ddiConstant;
  if(sConstantName == "MRTN_CreateID_P5")
  { 
       ddiConstant[1] = makeDynInt(1, 2, 3, 10, 11, 12, 13, 14, 19, 20, 21, 24); // MRTN at CAN4
       ddiConstant[2] = makeDynInt(4, 5, 6, 7,  8,  9,  15, 16, 17, 18, 22, 23); // MRTN at CAN5
       ddiConstant[3] = makeDynInt(31, 32, 33, 40, 41, 42, 43, 44, 49, 50, 51, 54);    // MRTN at CAN6
       ddiConstant[4] = makeDynInt(34, 35, 36, 37, 38, 39, 45, 46, 47, 48, 52, 53);    // MRTN at CAN7
  } 
 if(sConstantName == "MRTN_CrateID_B904") 
  { 
       ddiConstant[1] = makeDynInt(10);
  } 
 return ddiConstant; 
}
mapping emuLvMRTN_getMapping(string sConstantName)
{
  mapping mConstant;
  if(sConstantName == "MRTN_Crate_Node_Label_P5")
  {
              mConstant["CSC_ME_M4_CR1_LV_MRTN"] = "ME_M4_CR1_LV_MRTN/Id 51";
              mConstant["CSC_ME_M4_CR2_LV_MRTN"] = "ME_M4_CR2_LV_MRTN/Id 52";
              mConstant["CSC_ME_M4_CR3_LV_MRTN"] = "ME_M4_CR3_LV_MRTN/Id 53";       
              mConstant["CSC_ME_M4_CR4_LV_MRTN"] = "ME_M4_CR4_LV_MRTN/Id 54";       
              
              mConstant["CSC_ME_M2_CR1_LV_MRTN"] = "ME_M2_CR1_LV_MRTN/Id 43";
              mConstant["CSC_ME_M2_CR2_LV_MRTN"] = "ME_M2_CR2_LV_MRTN/Id 44";
              mConstant["CSC_ME_M2_CR3_LV_MRTN"] = "ME_M2_CR3_LV_MRTN/Id 45";
              mConstant["CSC_ME_M2_CR4_LV_MRTN"] = "ME_M2_CR4_LV_MRTN/Id 46";
              mConstant["CSC_ME_M2_CR5_LV_MRTN"] = "ME_M2_CR5_LV_MRTN/Id 47";
              mConstant["CSC_ME_M2_CR6_LV_MRTN"] = "ME_M2_CR6_LV_MRTN/Id 48";
              mConstant["CSC_ME_M2_CR7_LV_MRTN"] = "ME_M2_CR7_LV_MRTN/Id 49";
              mConstant["CSC_ME_M2_CR8_LV_MRTN"] = "ME_M2_CR8_LV_MRTN/Id 50";
              
              mConstant["CSC_ME_M1_CR1_LV_MRTN"] = "ME_M1_CR1_LV_MRTN/Id 31";
              mConstant["CSC_ME_M1_CR2_LV_MRTN"] = "ME_M1_CR2_LV_MRTN/Id 32";
              mConstant["CSC_ME_M1_CR3_LV_MRTN"] = "ME_M1_CR3_LV_MRTN/Id 33";
              mConstant["CSC_ME_M1_CR4_LV_MRTN"] = "ME_M1_CR4_LV_MRTN/Id 34";
              mConstant["CSC_ME_M1_CR5_LV_MRTN"] = "ME_M1_CR5_LV_MRTN/Id 35";
              mConstant["CSC_ME_M1_CR6_LV_MRTN"] = "ME_M1_CR6_LV_MRTN/Id 36";
              mConstant["CSC_ME_M1_CR7_LV_MRTN"] = "ME_M1_CR7_LV_MRTN/Id 37";
              mConstant["CSC_ME_M1_CR8_LV_MRTN"] = "ME_M1_CR8_LV_MRTN/Id 38";              
              mConstant["CSC_ME_M1_CR9_LV_MRTN"] = "ME_M1_CR9_LV_MRTN/Id 39";              
              mConstant["CSC_ME_M1_CR10_LV_MRTN"] = "ME_M1_CR10_LV_MRTN/Id 40";              
              mConstant["CSC_ME_M1_CR11_LV_MRTN"] = "ME_M1_CR11_LV_MRTN/Id 41";              
              mConstant["CSC_ME_M1_CR12_LV_MRTN"] = "ME_M1_CR12_LV_MRTN/Id 42";                 
              
              
              mConstant["CSC_ME_P4_CR1_LV_MRTN"] = "ME_P4_CR1_LV_MRTN/Id 21";
              mConstant["CSC_ME_P4_CR2_LV_MRTN"] = "ME_P4_CR2_LV_MRTN/Id 22";
              mConstant["CSC_ME_P4_CR3_LV_MRTN"] = "ME_P4_CR3_LV_MRTN/Id 23";       
              mConstant["CSC_ME_P4_CR4_LV_MRTN"] = "ME_P4_CR4_LV_MRTN/Id 24";       
              
              mConstant["CSC_ME_P2_CR1_LV_MRTN"] = "ME_P2_CR1_LV_MRTN/Id 13";
              mConstant["CSC_ME_P2_CR2_LV_MRTN"] = "ME_P2_CR2_LV_MRTN/Id 14";
              mConstant["CSC_ME_P2_CR3_LV_MRTN"] = "ME_P2_CR3_LV_MRTN/Id 15";
              mConstant["CSC_ME_P2_CR4_LV_MRTN"] = "ME_P2_CR4_LV_MRTN/Id 16";
              mConstant["CSC_ME_P2_CR5_LV_MRTN"] = "ME_P2_CR5_LV_MRTN/Id 17";
              mConstant["CSC_ME_P2_CR6_LV_MRTN"] = "ME_P2_CR6_LV_MRTN/Id 18";
              mConstant["CSC_ME_P2_CR7_LV_MRTN"] = "ME_P2_CR7_LV_MRTN/Id 19";
              mConstant["CSC_ME_P2_CR8_LV_MRTN"] = "ME_P2_CR8_LV_MRTN/Id 20";
              
              mConstant["CSC_ME_P1_CR1_LV_MRTN"] = "ME_P1_CR1_LV_MRTN/Id 1";
              mConstant["CSC_ME_P1_CR2_LV_MRTN"] = "ME_P1_CR2_LV_MRTN/Id 2";
              mConstant["CSC_ME_P1_CR3_LV_MRTN"] = "ME_P1_CR3_LV_MRTN/Id 3";
              mConstant["CSC_ME_P1_CR4_LV_MRTN"] = "ME_P1_CR4_LV_MRTN/Id 4";
              mConstant["CSC_ME_P1_CR5_LV_MRTN"] = "ME_P1_CR5_LV_MRTN/Id 5";
              mConstant["CSC_ME_P1_CR6_LV_MRTN"] = "ME_P1_CR6_LV_MRTN/Id 6";
              mConstant["CSC_ME_P1_CR7_LV_MRTN"] = "ME_P1_CR7_LV_MRTN/Id 7";
              mConstant["CSC_ME_P1_CR8_LV_MRTN"] = "ME_P1_CR8_LV_MRTN/Id 8";              
              mConstant["CSC_ME_P1_CR9_LV_MRTN"] = "ME_P1_CR9_LV_MRTN/Id 9";              
              mConstant["CSC_ME_P1_CR10_LV_MRTN"] = "ME_P1_CR10_LV_MRTN/Id 10";              
              mConstant["CSC_ME_P1_CR11_LV_MRTN"] = "ME_P1_CR11_LV_MRTN/Id 11";              
              mConstant["CSC_ME_P1_CR12_LV_MRTN"] = "ME_P1_CR12_LV_MRTN/Id 12";              
  }
  if(sConstantName == "MRTN_Crate_Node_Name_P5")
  {
              mConstant["CSC_ME_M4_CR1_LV_MRTN"] = "Wiener/CAN6/Crate51";
              mConstant["CSC_ME_M4_CR2_LV_MRTN"] = "Wiener/CAN7/Crate52";
              mConstant["CSC_ME_M4_CR3_LV_MRTN"] = "Wiener/CAN7/Crate53";       
              mConstant["CSC_ME_M4_CR4_LV_MRTN"] = "Wiener/CAN6/Crate54";
              
              mConstant["CSC_ME_M2_CR1_LV_MRTN"] = "Wiener/CAN6/Crate43";
              mConstant["CSC_ME_M2_CR2_LV_MRTN"] = "Wiener/CAN6/Crate44";
              mConstant["CSC_ME_M2_CR3_LV_MRTN"] = "Wiener/CAN7/Crate45";
              mConstant["CSC_ME_M2_CR4_LV_MRTN"] = "Wiener/CAN7/Crate46";
              mConstant["CSC_ME_M2_CR5_LV_MRTN"] = "Wiener/CAN7/Crate47";
              mConstant["CSC_ME_M2_CR6_LV_MRTN"] = "Wiener/CAN7/Crate48";
              mConstant["CSC_ME_M2_CR7_LV_MRTN"] = "Wiener/CAN6/Crate49";
              mConstant["CSC_ME_M2_CR8_LV_MRTN"] = "Wiener/CAN6/Crate50";
              
              mConstant["CSC_ME_M1_CR1_LV_MRTN"] = "Wiener/CAN6/Crate31";
              mConstant["CSC_ME_M1_CR2_LV_MRTN"] = "Wiener/CAN6/Crate32";
              mConstant["CSC_ME_M1_CR3_LV_MRTN"] = "Wiener/CAN6/Crate33";
              mConstant["CSC_ME_M1_CR4_LV_MRTN"] = "Wiener/CAN7/Crate34";
              mConstant["CSC_ME_M1_CR5_LV_MRTN"] = "Wiener/CAN7/Crate35";
              mConstant["CSC_ME_M1_CR6_LV_MRTN"] = "Wiener/CAN7/Crate36";
              mConstant["CSC_ME_M1_CR7_LV_MRTN"] = "Wiener/CAN7/Crate37";
              mConstant["CSC_ME_M1_CR8_LV_MRTN"] = "Wiener/CAN7/Crate38";              
              mConstant["CSC_ME_M1_CR9_LV_MRTN"] = "Wiener/CAN7/Crate39";              
              mConstant["CSC_ME_M1_CR10_LV_MRTN"] = "Wiener/CAN6/Crate40";              
              mConstant["CSC_ME_M1_CR11_LV_MRTN"] = "Wiener/CAN6/Crate41";              
              mConstant["CSC_ME_M1_CR12_LV_MRTN"] = "Wiener/CAN6/Crate42";              

              
              mConstant["CSC_ME_P4_CR1_LV_MRTN"] = "Wiener/CAN4/Crate21";
              mConstant["CSC_ME_P4_CR2_LV_MRTN"] = "Wiener/CAN5/Crate22";
              mConstant["CSC_ME_P4_CR3_LV_MRTN"] = "Wiener/CAN5/Crate23";       
              mConstant["CSC_ME_P4_CR4_LV_MRTN"] = "Wiener/CAN4/Crate24";
              
              mConstant["CSC_ME_P2_CR1_LV_MRTN"] = "Wiener/CAN4/Crate13";
              mConstant["CSC_ME_P2_CR2_LV_MRTN"] = "Wiener/CAN4/Crate14";
              mConstant["CSC_ME_P2_CR3_LV_MRTN"] = "Wiener/CAN5/Crate15";
              mConstant["CSC_ME_P2_CR4_LV_MRTN"] = "Wiener/CAN5/Crate16";
              mConstant["CSC_ME_P2_CR5_LV_MRTN"] = "Wiener/CAN5/Crate17";
              mConstant["CSC_ME_P2_CR6_LV_MRTN"] = "Wiener/CAN5/Crate18";
              mConstant["CSC_ME_P2_CR7_LV_MRTN"] = "Wiener/CAN4/Crate19";
              mConstant["CSC_ME_P2_CR8_LV_MRTN"] = "Wiener/CAN4/Crate20";
              
              mConstant["CSC_ME_P1_CR1_LV_MRTN"] = "Wiener/CAN4/Crate1";
              mConstant["CSC_ME_P1_CR2_LV_MRTN"] = "Wiener/CAN4/Crate2";
              mConstant["CSC_ME_P1_CR3_LV_MRTN"] = "Wiener/CAN4/Crate3";
              mConstant["CSC_ME_P1_CR4_LV_MRTN"] = "Wiener/CAN5/Crate4";
              mConstant["CSC_ME_P1_CR5_LV_MRTN"] = "Wiener/CAN5/Crate5";
              mConstant["CSC_ME_P1_CR6_LV_MRTN"] = "Wiener/CAN5/Crate6";
              mConstant["CSC_ME_P1_CR7_LV_MRTN"] = "Wiener/CAN5/Crate7";
              mConstant["CSC_ME_P1_CR8_LV_MRTN"] = "Wiener/CAN5/Crate8";              
              mConstant["CSC_ME_P1_CR9_LV_MRTN"] = "Wiener/CAN5/Crate9";              
              mConstant["CSC_ME_P1_CR10_LV_MRTN"] = "Wiener/CAN4/Crate10";              
              mConstant["CSC_ME_P1_CR11_LV_MRTN"] = "Wiener/CAN4/Crate11";              
              mConstant["CSC_ME_P1_CR12_LV_MRTN"] = "Wiener/CAN4/Crate12";              
  }    
  return mConstant;
}
/**
 * return a dyn_dyn_string 
 */ 
dyn_dyn_string emuLvMRTN_getDynDynString(string sConstantName)
{ 
  dyn_dyn_string ddsConstant;
  if(sConstantName == "MRTN_Crates_P5")
  { 
    ddsConstant[1] = makeDynString("Crate1", "Crate2", "Crate3", "Crate10", "Crate11", "Crate12", "Crate13", "Crate14", "Crate19", "Crate20", "Crate21", "Crate24"); // MRTN at CAN4
    ddsConstant[2] = makeDynString("Crate4", "Crate5", "Crate6", "Crate7",  "Crate8",  "Crate9",  "Crate15", "Crate16", "Crate17", "Crate18", "Crate22", "Crate23"); // MRTN at CAN5
    ddsConstant[3] = makeDynString("Crate31", "Crate32", "Crate33", "Crate40", "Crate41", "Crate42", "Crate43", "Crate44", "Crate49", "Crate50", "Crate51", "Crate54"); // MRTN at CAN6
    ddsConstant[4] = makeDynString("Crate34", "Crate35", "Crate36", "Crate37",  "Crate38",  "Crate39",  "Crate45", "Crate46", "Crate47", "Crate48", "Crate52", "Crate53"); // MRTN at CAN7
  } 
 if(sConstantName == "MRTN_Crate_B904") 
  { 
       ddsConstant[1] = makeDynString("Crate10");
  } 
 return ddsConstant;   
}  
 
