// Constant Definitions for emu dcs LV_CRB
//---------------------------------------------------------------------------------------------------------------------------------------------------
const string TopFsmObjType     = "CSC_LV_NODES";
const string StationFsmObjType = "CSC_LV_TREE_NODES";
const string CrbFsmDevType     = "fwCrb_CSC_LV";
const string PsuFsmDevType     = "fwBranch_CSC_LV";
const int    iTimeElmbAi       = 7200;    //time interval for ElmbAi
const float  fTolElmbAi        = 1;       //deadband for ElmbAi 
//---------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * return a dyn_string 
 */ 
dyn_string emuLvCRB_getDynString(string sConstantName)
{
  dyn_string dsConstant;
  if(sConstantName == "Pcrate_Elmb_P5") 
   { 
              dsConstant = makeDynString("CR1_PSU;00;3f","M4_PC1;37","M4_PC2;38","M4_PC3;39","M4_PC4;3a","M4_PC5;3b","M4_PC6;3c",      
                                                      "M3_PC1;31","M3_PC2;32","M3_PC3;33","M3_PC4;34","M3_PC5;35","M3_PC6;36",
                                                      "M2_PC1;2b","M2_PC2;2c","M2_PC3;2d","M2_PC4;2e","M2_PC5;2f","M2_PC6;30",
                                                "M1_PC1A;20","M1_PC1B;21","M1_PC2A;23","M1_PC2B;22","M1_PC3A;25","M1_PC3B;24",
                                                "M1_PC4A;27","M1_PC4B;26","M1_PC5A;28","M1_PC5B;29","M1_PC6A;2a","M1_PC6B;1f",
                                                "P1_PC1A;02","P1_PC1B;03","P1_PC2A;05","P1_PC2B;04","P1_PC3A;07","P1_PC3B;06",
                                                "P1_PC4A;09","P1_PC4B;08","P1_PC5A;0a","P1_PC5B;0b","P1_PC6A;0c","P1_PC6B;01",
                                                      "P2_PC1;0d","P2_PC2;0e","P2_PC3;0f","P2_PC4;10","P2_PC5;11","P2_PC6;12",
                                                      "P3_PC1;13","P3_PC2;14","P3_PC3;15","P3_PC4;16","P3_PC5;17","P3_PC6;18",
                                                      "P4_PC1;19","P4_PC2;1a","P4_PC3;1b","P4_PC4;1c","P4_PC5;1d","P4_PC6;1e");
   }           
// M-4 37 38 39 3a 3b 3c
// M-3 31 32 33 34 35 36
// M-2 2b 2c 2d 2e 2f 30
// M-1 20 21 | 23 22 | 25 24 | 27 26 | 28 29 | 2a 1f
// P+1 2 3 | 5 4 | 7 6 | 9 8 | 0a 0b | 0c 1
// P+2 0d 0e 0f 10 11 12
// P+3 13 14 15 16 17 18
// P+4 19 1a 1b 1c 1d 1e                                                
  if(sConstantName == "Pcrate_Geog_P5")
  {  
                       dsConstant = makeDynString("n.a.","X5E51/bot;1","X5L51/bot;2","X3S51/top;3","X1L51/top;4","X1E51/bot;5","X3V51/bot;6",  //ME-4
                                                         "X5E41/bot;1","X5L41/bot;2","X3S41/top;3","X1L41/top;4","X1E41/bot;5","X3V41/bot;6",  //ME-3
                                                         "X5E41/top;1","X5L41/top;2","X3S41/bot;3","X1L41/bot;4","X1E41/top;5","X3V41/top;6",  //ME-2
                                                         "X5E31/bot;2","X5E31/top;3","X5L31/bot;5","X5L31/top;4","X3S31/bot;7","X3S31/top;6",  //ME-1
                                                      "X1L31/bot;9","X1L31/top;8","X1E31/bot;10","X1E31/top;11","X3V31/bot;12","X3V31/top;1",  //ME-1
                                                         "X5U31/bot;2","X5U31/top;3","X5R31/bot;5","X5R31/top;4","X3A31/bot;7","X3A31/top;6",  //ME+1
                                                      "X1R31/bot;9","X1R31/top;8","X1U31/bot;10","X1U31/top;11","X3J31/bot;12","X3J31/top;1",  //ME+1
                                                         "X5U41/top;1","X5R41/top;2","X3A41/top;3","X1R41/bot;4","X1U41/top;5","X3J41/bot;6",  //ME+2
                                                         "X5U41/bot;1","X5R41/top;2","X3A41/top;3","X1R41/bot;4","X1U41/top;5","X3J41/bot;6",  //ME+3
                                                         "X5U51/bot;1","X5R51/bot;2","X3A51/top;3","X1R51/top;4","X1U51/bot;5","X3J51/bot;6"); //ME+4
  }
  if(sConstantName == "Pcmb_Mrtn_P5")
  {  
                     dsConstant = makeDynString("55;38","56;37","57;37","58;37","59;38","60;38",                                                 //ME-4
                                                "49;29","50;32","51;34","52;34","53;35","54;29",                                                 //ME-3
                                                "43;30","44;31","45;33","46;34","47;36","48;29",                                                 //ME-2
                                                "32;21","33;22","35;24","34;23","37;26","36;25","39;26","38;26","40;27","41;28","42;21","31;21", //ME-1
                                                "2;1",  "3;2",  "5;4",  "4;3",  "7;6",  "6;5",  "9;6",  "8;6", "10;7", "11;8", "12;1",  "1;1",   //ME+1
                                                "13;10","14;11","15;13","16;14","17;16", "18;9",                                                 //ME+2
                                                "19;9","20;12","21;14","22;14","23;15", "24;9",                                                  //ME+3
                                                "25;18","26;19","27;17","28;17","29;18","30;18");                                                //ME+4  
  }
  if(sConstantName == "Pcrate_Elmb_B904")
  {  
        dsConstant = makeDynString("CR1_PSU;00;3f","DEV_PC1;13","DEV_PC2;02");
  }      
  if(sConstantName == "Pcrate_Geog_B904")
  {  
        dsConstant = makeDynString("n.a.","DEV Bld","DEV Bld");
  }
  if(sConstantName == "Pcrate_Mrtn_B904")
  { 
       dsConstant = makeDynString("");
  }
  if(sConstantName == "DpType_Delete")
  {  
       dsConstant = makeDynString("fwCrb_CSC_LV","fwPsu_CSC_LV","fwBranch_CSC_LV","Db_PCMB",
                                  "FwElmbAi","FwElmbAiConfig",
                                  "FwElmbDo","FwElmbDoBytes","FwElmbDoConfig",
                                  "FwElmbNode","FwElmbCANbus",
                                  "FwElmbPSUBranch","FwElmbPSUCrate");
  } 
  if(sConstantName == "CRB_Station_Nodes")
  { 
       dsConstant = makeDynString("CSC_ME_M1_LV_CRB","CSC_ME_M2_LV_CRB","CSC_ME_M3_LV_CRB","CSC_ME_M4_LV_CRB",
                                  "CSC_ME_P1_LV_CRB","CSC_ME_P2_LV_CRB","CSC_ME_P3_LV_CRB","CSC_ME_P4_LV_CRB");
  }
  if(sConstantName == "CSC_ME_M1_LV_CRB")
  {
       dsConstant = makeDynString("CSC_ME_M1_PC1A_LV_CRB","CSC_ME_M1_PC1B_LV_CRB","CSC_ME_M1_PC2A_LV_CRB","CSC_ME_M1_PC2B_LV_CRB",
                                  "CSC_ME_M1_PC3A_LV_CRB","CSC_ME_M1_PC3B_LV_CRB","CSC_ME_M1_PC4A_LV_CRB","CSC_ME_M1_PC4B_LV_CRB",
                                  "CSC_ME_M1_PC5A_LV_CRB","CSC_ME_M1_PC5B_LV_CRB","CSC_ME_M1_PC6A_LV_CRB","CSC_ME_M1_PC6B_LV_CRB");
  } 
  if(sConstantName == "CSC_ME_M2_LV_CRB")
  {
       dsConstant = makeDynString("CSC_ME_M2_PC1_LV_CRB","CSC_ME_M2_PC2_LV_CRB","CSC_ME_M2_PC3_LV_CRB",
                                  "CSC_ME_M2_PC4_LV_CRB","CSC_ME_M2_PC5_LV_CRB","CSC_ME_M2_PC6_LV_CRB");                                  
  } 
  if(sConstantName == "CSC_ME_M3_LV_CRB")
  {
       dsConstant = makeDynString("CSC_ME_M3_PC1_LV_CRB","CSC_ME_M3_PC2_LV_CRB","CSC_ME_M3_PC3_LV_CRB",
                                  "CSC_ME_M3_PC4_LV_CRB","CSC_ME_M3_PC5_LV_CRB","CSC_ME_M3_PC6_LV_CRB");                                  
  }
  if(sConstantName == "CSC_ME_M4_LV_CRB")
  {
       dsConstant = makeDynString("CSC_ME_M4_PC1_LV_CRB","CSC_ME_M4_PC2_LV_CRB","CSC_ME_M4_PC3_LV_CRB",
                                  "CSC_ME_M4_PC4_LV_CRB","CSC_ME_M4_PC5_LV_CRB","CSC_ME_M4_PC6_LV_CRB");                                  
  } 
  if(sConstantName == "CSC_ME_P1_LV_CRB")
  {
       dsConstant = makeDynString("CSC_ME_P1_PC1A_LV_CRB","CSC_ME_P1_PC1B_LV_CRB","CSC_ME_P1_PC2A_LV_CRB","CSC_ME_P1_PC2B_LV_CRB",
                                  "CSC_ME_P1_PC3A_LV_CRB","CSC_ME_P1_PC3B_LV_CRB","CSC_ME_P1_PC4A_LV_CRB","CSC_ME_P1_PC4B_LV_CRB",
                                  "CSC_ME_P1_PC5A_LV_CRB","CSC_ME_P1_PC5B_LV_CRB","CSC_ME_P1_PC6A_LV_CRB","CSC_ME_P1_PC6B_LV_CRB");
  }
  if(sConstantName == "CSC_ME_P2_LV_CRB")
  {
       dsConstant = makeDynString("CSC_ME_P2_PC1_LV_CRB","CSC_ME_P2_PC2_LV_CRB","CSC_ME_P2_PC3_LV_CRB",
                                  "CSC_ME_P2_PC4_LV_CRB","CSC_ME_P2_PC5_LV_CRB","CSC_ME_P2_PC6_LV_CRB");                                  
  } 
  if(sConstantName == "CSC_ME_P3_LV_CRB")
  {
       dsConstant = makeDynString("CSC_ME_P3_PC1_LV_CRB","CSC_ME_P3_PC2_LV_CRB","CSC_ME_P3_PC3_LV_CRB",
                                  "CSC_ME_P3_PC4_LV_CRB","CSC_ME_P3_PC5_LV_CRB","CSC_ME_P3_PC6_LV_CRB");                                  
  }
  if(sConstantName == "CSC_ME_P4_LV_CRB")
  {
       dsConstant = makeDynString("CSC_ME_P4_PC1_LV_CRB","CSC_ME_P4_PC2_LV_CRB","CSC_ME_P4_PC3_LV_CRB",
                                  "CSC_ME_P4_PC4_LV_CRB","CSC_ME_P4_PC5_LV_CRB","CSC_ME_P4_PC6_LV_CRB");                                  
  }  
 if(sConstantName == "PSU_Branch_Nodes")
  { 
       dsConstant = makeDynString("CSC_ME_LV_CR1_PLUS_PSU","CSC_ME_LV_CR1_MINUS_PSU");
  } 
 if(sConstantName == "CSC_ME_LV_CR1_PLUS_PSU")
  { 
       dsConstant = makeDynString("Branch00","Branch01");
  } 
 if(sConstantName == "CSC_ME_LV_CR1_MINUS_PSU")
  { 
       dsConstant = makeDynString("Branch02","Branch03");
  } 
 if(sConstantName == "CanBus_Name_P5")
  { 
       dsConstant = makeDynString("LVCB_0","LVCB_1","LVCB_2","LVCB_3");
  }
 if(sConstantName == "CanBus_Name_B904")
  { 
       dsConstant = makeDynString("LVCB_1");
  }
 if(sConstantName == "Switch_List_DTmbs")
  { 
       dsConstant = makeDynString("C;0","C;1","C;2","C;3","C;4","C;5","C;6","C;7","A;0");
  }
 if(sConstantName == "Switch_List_Others")
  { 
       dsConstant = makeDynString("A;1","A;2","A;3","A;4");
  }
  return dsConstant;
}  
/**
 * return a dyn_int 
 */ 
dyn_int emuLvCRB_getDynInt(string sConstantName)
{    
    dyn_int diConstant;
    if(sConstantName == "ElmbAi")
     { 
       diConstant = makeDynInt(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
                                 20,21,32,33,34,35,48,49,50,51,52,53,54,55,56,57);
     } 
    if(sConstantName == "ElmbAi_3.3V")
     { 
       diConstant = makeDynInt(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,32,33,34,35);
     } 
    if(sConstantName == "ElmbAi_5V")
     { 
       diConstant = makeDynInt(19,48,49,50,51,52,53,54,55,56,57);
     } 
    if(sConstantName == "ElmbAi_1.5V")
     { 
       diConstant = makeDynInt(20,21);
     } 
  return diConstant;
}    
/**
 * return a dyn_dyn_int 
 */ 
dyn_dyn_int emuLvCRB_getDynDynInt(string sConstantName)
{
  dyn_dyn_int ddiConstant;
  if(sConstantName == "ElmbID_P5")
  { 
       ddiConstant[1] = makeDynInt(1,2,3,10,11,12,13,17,18,19,23,24,25,29,30);      // ELMB at LVCB_0
       ddiConstant[2] = makeDynInt(4,5,6,7,8,9,14,15,16,20,21,22,26,27,28);         // ELMB at LVCB_1
       ddiConstant[3] = makeDynInt(31,32,33,40,41,42,43,47,48,49,53,54,55,59,60);   // ELMB at LVCB_2
       ddiConstant[4] = makeDynInt(34,35,36,37,38,39,44,45,46,50,51,52,56,57,58);   // ELMB at LVCB_3
  } 
  if(sConstantName == "ElmbID_B904") 
  { 
       ddiConstant[1] = makeDynInt(1,2);
  } 
  return ddiConstant; 
}
/**
 * return a mapping constant
 */
mapping emuLvCRB_getMapping(string sConstantName)
{
  mapping mConstant;
  if(sConstantName == "Marthon_Crb_ID_P5")
  {
              mConstant["M4_PC1"] = "MId 38/Id 55";
              mConstant["M4_PC2"] = "MId 37/Id 56";
              mConstant["M4_PC3"] = "MId 37/Id 57";
              mConstant["M4_PC4"] = "MId 37/Id 58";
              mConstant["M4_PC5"] = "MId 38/Id 59";
              mConstant["M4_PC6"] = "MId 38/Id 60";
              
              mConstant["M3_PC1"] = "MId 29/Id 49";
              mConstant["M3_PC2"] = "MId 32/Id 50";
              mConstant["M3_PC3"] = "MId 34/Id 51";
              mConstant["M3_PC4"] = "MId 34/Id 52";
              mConstant["M3_PC5"] = "MId 35/Id 53";
              mConstant["M3_PC6"] = "MId 29/Id 54";
              
              mConstant["M2_PC1"] = "MId 30/Id 43";
              mConstant["M2_PC2"] = "MId 31/Id 44";
              mConstant["M2_PC3"] = "MId 33/Id 45";
              mConstant["M2_PC4"] = "MId 34/Id 46";
              mConstant["M2_PC5"] = "MId 36/Id 47";
              mConstant["M2_PC6"] = "MId 29/Id 48";
              
              mConstant["M1_PC1A"] = "MId 21/Id 32";
              mConstant["M1_PC1B"] = "MId 22/Id 33";
              mConstant["M1_PC2A"] = "MId 24/Id 35";
              mConstant["M1_PC2B"] = "MId 23/Id 34";
              mConstant["M1_PC3A"] = "MId 26/Id 37";
              mConstant["M1_PC3B"] = "MId 25/Id 36";
              mConstant["M1_PC4A"] = "MId 26/Id 39";
              mConstant["M1_PC4B"] = "MId 26/Id 38";
              mConstant["M1_PC5A"] = "MId 27/Id 40";
              mConstant["M1_PC5B"] = "MId 28/Id 41";
              mConstant["M1_PC6A"] = "MId 21/Id 42";
              mConstant["M1_PC6B"] = "MId 21/Id 31";
              
              mConstant["P4_PC1"] = "MId 18/Id 25";
              mConstant["P4_PC2"] = "MId 19/Id 26";
              mConstant["P4_PC3"] = "MId 17/Id 27";
              mConstant["P4_PC4"] = "MId 17/Id 28";
              mConstant["P4_PC5"] = "MId 18/Id 29";
              mConstant["P4_PC6"] = "MId 18/Id 30";
              
              mConstant["P3_PC1"] = "MId 9/Id 19";
              mConstant["P3_PC2"] = "MId 12/Id 20";
              mConstant["P3_PC3"] = "MId 14/Id 21";
              mConstant["P3_PC4"] = "MId 14/Id 22";
              mConstant["P3_PC5"] = "MId 15/Id 23";
              mConstant["P3_PC6"] = "MId 9/Id 24";
              
              mConstant["P2_PC1"] = "MId 10/Id 13";
              mConstant["P2_PC2"] = "MId 11/Id 14";
              mConstant["P2_PC3"] = "MId 13/Id 15";
              mConstant["P2_PC4"] = "MId 14/Id 16";
              mConstant["P2_PC5"] = "MId 16/Id 17";
              mConstant["P2_PC6"] = "MId 9/Id 18";
              
              mConstant["P1_PC1A"] = "MId 1/Id 2";
              mConstant["P1_PC1B"] = "MId 2/Id 3";
              mConstant["P1_PC2A"] = "MId 4/Id 5";
              mConstant["P1_PC2B"] = "MId 3/Id 4";
              mConstant["P1_PC3A"] = "MId 6/Id 7";
              mConstant["P1_PC3B"] = "MId 5/Id 6";
              mConstant["P1_PC4A"] = "MId 6/Id 9";
              mConstant["P1_PC4B"] = "MId 6/Id 8";
              mConstant["P1_PC5A"] = "MId 7/Id 10";
              mConstant["P1_PC5B"] = "MId 8/Id 11";
              mConstant["P1_PC6A"] = "MId 1/Id 12";
              mConstant["P1_PC6B"] = "MId 1/Id 1";
              
  }  
  return mConstant;
}  
  
