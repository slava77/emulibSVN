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
       dsConstant = makeDynString("M4_CR1;37","M4_CR2;38",  "M4_CR3;",  "M4_CR4;",
                                    "M4_CR5;",  "M4_CR6;",  "M4_CR7;",  "M4_CR8;",
                                  "M2_CR1;29","M2_CR2;30","M2_CR3;32","M2_CR4;31",
                                  "M2_CR5;34","M2_CR6;33","M2_CR7;35","M2_CR8;36",
                                  "M1_CR1;21","M1_CR2;22","M1_CR3;24","M1_CR4;23",
                                  "M1_CR5;26","M1_CR6;25","M1_CR7;27","M1_CR8;28",
                                  "P1_CR1;1",  "P1_CR2;2", "P1_CR3;4", "P1_CR4;3",
                                  "P1_CR5;6",  "P1_CR6;5", "P1_CR7;7", "P1_CR8;8",
                                  "P2_CR1;9", "P2_CR2;10","P2_CR3;12","P2_CR4;11",
                                  "P2_CR5;14","P2_CR6;13","P2_CR7;15","P2_CR8;16",
                                  "P4_CR1;17","P4_CR2;18","P4_CR3;19",  "P4_CR4;",
                                    "P4_CR5;",  "P4_CR6;",  "P4_CR7;",  "P4_CR8;");       
  }
  if(sConstantName == "Db_MRTN_Geog_P5")
  { 
    dsConstant = makeDynString("X4S51","X2V52","","","","","","",
                               "X4V41_L","X4V41_U","X4S41_L","X4S41_U","X2S41_L","X2S41_U","X2V42_L","X2V42_U",
                               "X4V31_L","X4V31_U","X4S31_L","X4S31_U","X2S33_L","X2S33_U","X2V31_L","X2V31_U",
                               "X4J31_L","X4J31_U","X4A31_L","X4A31_U","X2A33_L","X2A33_U","X2J31_L","X2J31_U",
                               "X4J41_L","X4J41_U","X4A41_L","X4A41_U","X2A41_L","X2A41_U","X2J42_L","X2J42_U",
                               "X4A51","X2J52","X4A51","","","","","");
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
    dsConstant = makeDynString("CSC_ME_M1_CR1_LV_MRTN","CSC_ME_M1_CR2_LV_MRTN","CSC_ME_M1_CR3_LV_MRTN","CSC_ME_M1_CR4_LV_MRTN",
                               "CSC_ME_M1_CR5_LV_MRTN","CSC_ME_M1_CR6_LV_MRTN","CSC_ME_M1_CR7_LV_MRTN","CSC_ME_M1_CR8_LV_MRTN");
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
    dsConstant = makeDynString("CSC_ME_M4_CR1_LV_MRTN","CSC_ME_M4_CR2_LV_MRTN");                                  
  } 
  if(sConstantName == "CSC_ME_P1_LV_MRTN")
  {
    dsConstant = makeDynString("CSC_ME_P1_CR1_LV_MRTN","CSC_ME_P1_CR2_LV_MRTN","CSC_ME_P1_CR3_LV_MRTN","CSC_ME_P1_CR4_LV_MRTN",
                               "CSC_ME_P1_CR5_LV_MRTN","CSC_ME_P1_CR6_LV_MRTN","CSC_ME_P1_CR7_LV_MRTN","CSC_ME_P1_CR8_LV_MRTN");
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
    dsConstant = makeDynString("CSC_ME_P4_CR1_LV_MRTN","CSC_ME_P4_CR2_LV_MRTN","CSC_ME_P4_CR3_LV_MRTN");                                
  } 
  if(sConstantName == "MRTN_Channel_Alert_Status")
  {
    dsConstant = makeDynString("FailureMaxTemperature",        "FailureMaxCurrent",
                               "FailureMaxSenseVoltage","FailureMaxTerminalVoltage");                                
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
       ddiConstant[1] = makeDynInt( 1, 2, 7, 8, 9,10,15,16,18);    // MRTN at CAN4
       ddiConstant[2] = makeDynInt( 3, 4, 5, 6,11,12,13,14,17,19); // MRTN at CAN5
       ddiConstant[3] = makeDynInt(21,22,27,28,29,30,35,36,38);    // MRTN at CAN6
       ddiConstant[4] = makeDynInt(23,24,25,26,31,32,33,34,37);    // MRTN at CAN7
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
              mConstant["CSC_ME_M4_CR1_LV_MRTN"] = "ME_M4_CR1_LV_MRTN/Id 37";
              mConstant["CSC_ME_M4_CR2_LV_MRTN"] = "ME_M4_CR2_LV_MRTN/Id 38";              
              
              mConstant["CSC_ME_M2_CR1_LV_MRTN"] = "ME_M2_CR1_LV_MRTN/Id 29";
              mConstant["CSC_ME_M2_CR2_LV_MRTN"] = "ME_M2_CR2_LV_MRTN/Id 30";
              mConstant["CSC_ME_M2_CR3_LV_MRTN"] = "ME_M2_CR3_LV_MRTN/Id 32";
              mConstant["CSC_ME_M2_CR4_LV_MRTN"] = "ME_M2_CR4_LV_MRTN/Id 31";
              mConstant["CSC_ME_M2_CR5_LV_MRTN"] = "ME_M2_CR5_LV_MRTN/Id 34";
              mConstant["CSC_ME_M2_CR6_LV_MRTN"] = "ME_M2_CR6_LV_MRTN/Id 33";
              mConstant["CSC_ME_M2_CR7_LV_MRTN"] = "ME_M2_CR7_LV_MRTN/Id 35";
              mConstant["CSC_ME_M2_CR8_LV_MRTN"] = "ME_M2_CR8_LV_MRTN/Id 36";
                            
              mConstant["CSC_ME_M1_CR1_LV_MRTN"] = "ME_M1_CR1_LV_MRTN/Id 21";
              mConstant["CSC_ME_M1_CR2_LV_MRTN"] = "ME_M1_CR2_LV_MRTN/Id 22";
              mConstant["CSC_ME_M1_CR3_LV_MRTN"] = "ME_M1_CR3_LV_MRTN/Id 24";
              mConstant["CSC_ME_M1_CR4_LV_MRTN"] = "ME_M1_CR4_LV_MRTN/Id 23";
              mConstant["CSC_ME_M1_CR5_LV_MRTN"] = "ME_M1_CR5_LV_MRTN/Id 26";
              mConstant["CSC_ME_M1_CR6_LV_MRTN"] = "ME_M1_CR6_LV_MRTN/Id 25";
              mConstant["CSC_ME_M1_CR7_LV_MRTN"] = "ME_M1_CR7_LV_MRTN/Id 27";
              mConstant["CSC_ME_M1_CR8_LV_MRTN"] = "ME_M1_CR8_LV_MRTN/Id 28";
              
              mConstant["CSC_ME_P4_CR1_LV_MRTN"] = "ME_P4_CR1_LV_MRTN/Id 17";
              mConstant["CSC_ME_P4_CR2_LV_MRTN"] = "ME_P4_CR2_LV_MRTN/Id 18";
              mConstant["CSC_ME_P4_CR3_LV_MRTN"] = "ME_P4_CR3_LV_MRTN/Id 19";       
              
              mConstant["CSC_ME_P2_CR1_LV_MRTN"] = "ME_P2_CR1_LV_MRTN/Id 9";
              mConstant["CSC_ME_P2_CR2_LV_MRTN"] = "ME_P2_CR2_LV_MRTN/Id 10";
              mConstant["CSC_ME_P2_CR3_LV_MRTN"] = "ME_P2_CR3_LV_MRTN/Id 12";
              mConstant["CSC_ME_P2_CR4_LV_MRTN"] = "ME_P2_CR4_LV_MRTN/Id 11";
              mConstant["CSC_ME_P2_CR5_LV_MRTN"] = "ME_P2_CR5_LV_MRTN/Id 14";
              mConstant["CSC_ME_P2_CR6_LV_MRTN"] = "ME_P2_CR6_LV_MRTN/Id 13";
              mConstant["CSC_ME_P2_CR7_LV_MRTN"] = "ME_P2_CR7_LV_MRTN/Id 15";
              mConstant["CSC_ME_P2_CR8_LV_MRTN"] = "ME_P2_CR8_LV_MRTN/Id 16";
              
              mConstant["CSC_ME_P1_CR1_LV_MRTN"] = "ME_P1_CR1_LV_MRTN/Id 1";
              mConstant["CSC_ME_P1_CR2_LV_MRTN"] = "ME_P1_CR2_LV_MRTN/Id 2";
              mConstant["CSC_ME_P1_CR3_LV_MRTN"] = "ME_P1_CR3_LV_MRTN/Id 4";
              mConstant["CSC_ME_P1_CR4_LV_MRTN"] = "ME_P1_CR4_LV_MRTN/Id 3";
              mConstant["CSC_ME_P1_CR5_LV_MRTN"] = "ME_P1_CR5_LV_MRTN/Id 6";
              mConstant["CSC_ME_P1_CR6_LV_MRTN"] = "ME_P1_CR6_LV_MRTN/Id 5";
              mConstant["CSC_ME_P1_CR7_LV_MRTN"] = "ME_P1_CR7_LV_MRTN/Id 7";
              mConstant["CSC_ME_P1_CR8_LV_MRTN"] = "ME_P1_CR8_LV_MRTN/Id 8";              
  }
  if(sConstantName == "MRTN_Crate_Node_Name_P5")
  {
              mConstant["CSC_ME_M4_CR1_LV_MRTN"] = "Wiener/CAN7/Crate37";
              mConstant["CSC_ME_M4_CR2_LV_MRTN"] = "Wiener/CAN6/Crate38";              
              
              mConstant["CSC_ME_M2_CR1_LV_MRTN"] = "Wiener/CAN6/Crate29";
              mConstant["CSC_ME_M2_CR2_LV_MRTN"] = "Wiener/CAN6/Crate30";
              mConstant["CSC_ME_M2_CR3_LV_MRTN"] = "Wiener/CAN7/Crate32";
              mConstant["CSC_ME_M2_CR4_LV_MRTN"] = "Wiener/CAN7/Crate31";
              mConstant["CSC_ME_M2_CR5_LV_MRTN"] = "Wiener/CAN7/Crate34";
              mConstant["CSC_ME_M2_CR6_LV_MRTN"] = "Wiener/CAN7/Crate33";
              mConstant["CSC_ME_M2_CR7_LV_MRTN"] = "Wiener/CAN6/Crate35";
              mConstant["CSC_ME_M2_CR8_LV_MRTN"] = "Wiener/CAN6/Crate36";
                            
              mConstant["CSC_ME_M1_CR1_LV_MRTN"] = "Wiener/CAN6/Crate21";
              mConstant["CSC_ME_M1_CR2_LV_MRTN"] = "Wiener/CAN6/Crate22";
              mConstant["CSC_ME_M1_CR3_LV_MRTN"] = "Wiener/CAN7/Crate24";
              mConstant["CSC_ME_M1_CR4_LV_MRTN"] = "Wiener/CAN7/Crate23";
              mConstant["CSC_ME_M1_CR5_LV_MRTN"] = "Wiener/CAN7/Crate26";
              mConstant["CSC_ME_M1_CR6_LV_MRTN"] = "Wiener/CAN7/Crate25";
              mConstant["CSC_ME_M1_CR7_LV_MRTN"] = "Wiener/CAN6/Crate27";
              mConstant["CSC_ME_M1_CR8_LV_MRTN"] = "Wiener/CAN6/Crate28";
              
              mConstant["CSC_ME_P4_CR1_LV_MRTN"] = "Wiener/CAN5/Crate17";
              mConstant["CSC_ME_P4_CR2_LV_MRTN"] = "Wiener/CAN4/Crate18";
              mConstant["CSC_ME_P4_CR3_LV_MRTN"] = "Wiener/CAN5/Crate19";       
              
              mConstant["CSC_ME_P2_CR1_LV_MRTN"] = "Wiener/CAN4/Crate9";
              mConstant["CSC_ME_P2_CR2_LV_MRTN"] = "Wiener/CAN4/Crate10";
              mConstant["CSC_ME_P2_CR3_LV_MRTN"] = "Wiener/CAN5/Crate12";
              mConstant["CSC_ME_P2_CR4_LV_MRTN"] = "Wiener/CAN5/Crate11";
              mConstant["CSC_ME_P2_CR5_LV_MRTN"] = "Wiener/CAN5/Crate14";
              mConstant["CSC_ME_P2_CR6_LV_MRTN"] = "Wiener/CAN5/Crate13";
              mConstant["CSC_ME_P2_CR7_LV_MRTN"] = "Wiener/CAN4/Crate15";
              mConstant["CSC_ME_P2_CR8_LV_MRTN"] = "Wiener/CAN4/Crate16";
              
              mConstant["CSC_ME_P1_CR1_LV_MRTN"] = "Wiener/CAN4/Crate1";
              mConstant["CSC_ME_P1_CR2_LV_MRTN"] = "Wiener/CAN4/Crate2";
              mConstant["CSC_ME_P1_CR3_LV_MRTN"] = "Wiener/CAN5/Crate4";
              mConstant["CSC_ME_P1_CR4_LV_MRTN"] = "Wiener/CAN5/Crate3";
              mConstant["CSC_ME_P1_CR5_LV_MRTN"] = "Wiener/CAN5/Crate6";
              mConstant["CSC_ME_P1_CR6_LV_MRTN"] = "Wiener/CAN5/Crate5";
              mConstant["CSC_ME_P1_CR7_LV_MRTN"] = "Wiener/CAN4/Crate7";
              mConstant["CSC_ME_P1_CR8_LV_MRTN"] = "Wiener/CAN4/Crate8";              
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
    ddsConstant[1] = makeDynString( "Crate1", "Crate2", "Crate7", "Crate8", "Crate9","Crate10","Crate15","Crate16","Crate18");           // MRTN at CAN4
    ddsConstant[2] = makeDynString( "Crate3", "Crate4", "Crate5", "Crate6","Crate11","Crate12","Crate13","Crate14","Crate17","Crate19"); // MRTN at CAN5
    ddsConstant[3] = makeDynString("Crate21","Crate22","Crate27","Crate28","Crate29","Crate30","Crate35","Crate36","Crate38");           // MRTN at CAN6
    ddsConstant[4] = makeDynString("Crate23","Crate24","Crate25","Crate26","Crate31","Crate32","Crate33","Crate34","Crate37");           // MRTN at CAN7
  } 
 if(sConstantName == "MRTN_Crate_B904") 
  { 
       ddsConstant[1] = makeDynString("Crate10");
  } 
 return ddsConstant;   
}  
 
