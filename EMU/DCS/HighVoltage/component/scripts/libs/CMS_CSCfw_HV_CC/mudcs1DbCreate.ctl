bool slice_test=false;
bool all_chambers=true;

  dyn_dyn_string CC_IN; 
  dyn_dyn_string CC_OUT;
  dyn_dyn_string HV_IN;  
  dyn_dyn_string HV_OUT;   
  dyn_dyn_string DISK_LEVEL;  
  
//  dyn_dyn_string InRadDeviceList;
//  dyn_dyn_string OutRadDeviceList;
//  dyn_dyn_string InConfigPanels;
//  dyn_dyn_string InOperationPanels;
//  dyn_dyn_string OutConfigPanels;
//  dyn_dyn_string OutOperationPanels;
  
  //=== DUBNA =====
  dyn_string CC_DUBNA;
  dyn_string HV_DUBNA;//600;;;"; that is dummy: used for the FSM tree creation  
  dyn_string DISK_DUBNA;  
  
//  dyn_string DeviceList_DUBNA;
//  dyn_string ConfigPanels_DUBNA;
//  dyn_string OperationPanels_DUBNA;
  //=================
    
  dyn_string EMU_LEVEL;
  dyn_string PCToManID;
  dyn_string DimServerComputerList;

mudcsDbCreateMain()
{

  mudcsInit();
  
int i,j;  
  
for(i=1;i<=8;i++)dynClear(DISK_LEVEL[i]);
dynClear(EMU_LEVEL);    

db_set_cc();
db_set_cc_new(); // postpone
db_set_disk_level();
db_set_emu_level();  

if (CSC_fwG_g_904) {
  db_set_hv(); // not used at P5 anymore - only imported through DP list (which is generated using HV map import utility
}



    int i10_i=1;
    int i10_o=1;
    int j10_i=0;
    int j10_o=6;
    int ipart;

for(i=1;i<=8;i++){
 
  if(CSC_fwG_g_904 && i==6){
       
     
    
     for(j=1;j<=dynlen(HV_IN[i]);j++){
      if(j<2 || j>7)continue; // two sectors       
//     strreplace(HV_IN[i][j],"600;","800;");
       if(j%2)ipart=1;
       else ipart=2;
       HV_IN[i][j]="800;"+i10_i+";"+j10_i+";"+ipart; 
      
      if(ipart==2)j10_i++;
      if(j10_i>5){i10_i++;j10_i=0;}
    }
  

   
   
   if(i!=5){     
    for(j=1;j<=dynlen(HV_OUT[i]);j++){
      if(j<3 || j>20)continue; // two sectors
    // strreplace(HV_OUT[i][j],"600;","800;");
 
       HV_OUT[i][j]="800;"+i10_o+";"+j10_o+";0"; 
      
      j10_o++;
      if(j10_o>15){i10_o++;j10_o=6;}   
   
    }
   } // i!=5
   else{
     for(j=1;j<=dynlen(HV_OUT[i]);j++){
//     strreplace(HV_IN[i][j],"600;","800;");
       if(j%2)ipart=1;
       else ipart=2;
       HV_OUT[i][j]="800;"+i10_i+";"+j10_i+";"+ipart; 
      
      if(ipart==2)j10_i++;
      if(j10_i>5){i10_i++;j10_i=0;}
    }    
     
     
   }
     
     
     
   
   
   
  }
    
  
  dpSet("Db_o.Wheels_o.Wheel"+i+".InDynatemAndSetNumberList",CC_IN[i]); 
  /*if(i!=1 & i!=8)*/dpSet("Db_o.Wheels_o.Wheel"+i+".OutDynatemAndSetNumberList",CC_OUT[i]);

if (CSC_fwG_g_904) { // not used at P5 anymore - only imported through DP list (which is generated using HV map import utility               
  dpSet("Db_o.Wheels_o.Wheel"+i+".InHVsetList",HV_IN[i]); 
  /*if(i!=1 & i!=8)*/dpSet("Db_o.Wheels_o.Wheel"+i+".OutHVsetList",HV_OUT[i]);   
  dpSet("Db_o.Wheels_o.Ring_DUBNA.HVsetList",HV_DUBNA);  
}
     
  dpSet("Db_o.Wheels_o.Wheel"+i+".DiskLevelDevicesCoordinates",DISK_LEVEL[i]); 
} // for

  dpSet("Db_o.Wheels_o.Ring_DUBNA.DynatemAndSetNumberList",CC_DUBNA);
  dpSet("Db_o.Wheels_o.Ring_DUBNA.DiskLevelDevicesCoordinates",DISK_DUBNA); 

  dpSet("Db_o.CscLevelDevicesCoordinates",EMU_LEVEL);    
  dpSet("Db_o.PCToManID",PCToManID); 
  dpSet("Db_o.DimServerComputerList",DimServerComputerList);   

 ///mudcsDebug("done");
  
}


//============================================  
//======= CRATE CONTROLLERS SECTION ==========
//============================================  
  
db_set_cc(){

int i;  
  
//======== ME-4 =============================  
  i=1;
  
  CC_IN[i][1]="dummy";
  CC_IN[i][2]="dummy";
  CC_IN[i][3]="dummy";  
  CC_IN[i][4]="dummy";
  CC_IN[i][5]="dummy";
  CC_IN[i][6]="dummy";
  CC_IN[i][7]="dummy";  
  CC_IN[i][8]="dummy";
  CC_IN[i][9]="dummy";  
  CC_IN[i][10]="dummy";
  CC_IN[i][11]="dummy";
  CC_IN[i][12]="dummy";
  CC_IN[i][13]="dummy";   
  CC_IN[i][14]="dummy";
  CC_IN[i][15]="dummy";  
  CC_IN[i][16]="dummy";
  CC_IN[i][17]="dummy";
  CC_IN[i][18]="dummy";  


    
//======== ME-3 =============================  
  i=2;
  
  CC_IN[i][1]="dummy";
  CC_IN[i][2]="dummy";
  CC_IN[i][3]="dummy";  
  CC_IN[i][4]="dummy";
  CC_IN[i][5]="dummy";
  CC_IN[i][6]="dummy";
  CC_IN[i][7]="dummy";  
  CC_IN[i][8]="02:00:00:00:00;47;3";
  CC_IN[i][9]="02:00:00:00:00;47;5";  
  CC_IN[i][10]="02:00:00:00:00;47;7";
  CC_IN[i][11]="dummy";
  CC_IN[i][12]="dummy";
  CC_IN[i][13]="dummy";   
  CC_IN[i][14]="dummy";
  CC_IN[i][15]="dummy";  
  CC_IN[i][16]="dummy";
  CC_IN[i][17]="dummy";
  CC_IN[i][18]="dummy";    
  
  CC_OUT[i][1]="dummy";
  CC_OUT[i][2]="dummy";
  CC_OUT[i][3]="dummy";  
  CC_OUT[i][4]="dummy";
  CC_OUT[i][5]="dummy";
  CC_OUT[i][6]="dummy";
  CC_OUT[i][7]="dummy";  
  CC_OUT[i][8]="dummy";
  CC_OUT[i][9]="dummy";  
  CC_OUT[i][10]="dummy";
  CC_OUT[i][11]="dummy";
  CC_OUT[i][12]="dummy";
  CC_OUT[i][13]="dummy";   
  CC_OUT[i][14]="dummy";
  CC_OUT[i][15]="02:00:00:00:00;47;9";  
  CC_OUT[i][16]="02:00:00:00:00;47;11";
  CC_OUT[i][17]="02:00:00:00:00;47;15";
  CC_OUT[i][18]="02:00:00:00:00;47;17";     
  CC_OUT[i][19]="02:00:00:00:00;47;19";
  CC_OUT[i][20]="02:00:00:00:00;47;21";
  CC_OUT[i][21]="dummy";  
  CC_OUT[i][22]="dummy";
  CC_OUT[i][23]="dummy";
  CC_OUT[i][24]="dummy";
  CC_OUT[i][25]="dummy";  
  CC_OUT[i][26]="dummy";
  CC_OUT[i][27]="dummy";  
  CC_OUT[i][28]="dummy";
  CC_OUT[i][29]="dummy";
  CC_OUT[i][30]="dummy";
  CC_OUT[i][31]="dummy";   
  CC_OUT[i][32]="dummy";
  CC_OUT[i][33]="dummy";  
  CC_OUT[i][34]="dummy";
  CC_OUT[i][35]="dummy";
  CC_OUT[i][36]="dummy";     

//======== ME-2 =============================  
  i=3;
  
  CC_IN[i][1]="dummy";
  CC_IN[i][2]="dummy";
  CC_IN[i][3]="dummy";  
  CC_IN[i][4]="dummy";
  CC_IN[i][5]="dummy";
  CC_IN[i][6]="dummy";
  CC_IN[i][7]="dummy";  
  CC_IN[i][8]="02:00:00:00:00;4a;3";
  CC_IN[i][9]="02:00:00:00:00;4a;5";  
  CC_IN[i][10]="02:00:00:00:00;4a;7";
  CC_IN[i][11]="dummy";
  CC_IN[i][12]="dummy";
  CC_IN[i][13]="dummy";   
  CC_IN[i][14]="dummy";
  CC_IN[i][15]="dummy";  
  CC_IN[i][16]="dummy";
  CC_IN[i][17]="dummy";
  CC_IN[i][18]="dummy";    
  
  CC_OUT[i][1]="dummy";
  CC_OUT[i][2]="dummy";
  CC_OUT[i][3]="dummy";  
  CC_OUT[i][4]="dummy";
  CC_OUT[i][5]="dummy";
  CC_OUT[i][6]="dummy";
  CC_OUT[i][7]="dummy";  
  CC_OUT[i][8]="dummy";
  CC_OUT[i][9]="dummy";  
  CC_OUT[i][10]="dummy";
  CC_OUT[i][11]="dummy";
  CC_OUT[i][12]="dummy";
  CC_OUT[i][13]="dummy";   
  CC_OUT[i][14]="dummy";
  CC_OUT[i][15]="02:00:00:00:00;4a;9";  
  CC_OUT[i][16]="02:00:00:00:00;4a;11";
  CC_OUT[i][17]="02:00:00:00:00;4a;15";
  CC_OUT[i][18]="02:00:00:00:00;4a;17";     
  CC_OUT[i][19]="02:00:00:00:00;4a;19";
  CC_OUT[i][20]="02:00:00:00:00;4a;21";
  CC_OUT[i][21]="dummy";  
  CC_OUT[i][22]="dummy";
  CC_OUT[i][23]="dummy";
  CC_OUT[i][24]="dummy";
  CC_OUT[i][25]="dummy";  
  CC_OUT[i][26]="dummy";
  CC_OUT[i][27]="dummy";  
  CC_OUT[i][28]="dummy";
  CC_OUT[i][29]="dummy";
  CC_OUT[i][30]="dummy";
  CC_OUT[i][31]="dummy";   
  CC_OUT[i][32]="dummy";
  CC_OUT[i][33]="dummy";  
  CC_OUT[i][34]="dummy";
  CC_OUT[i][35]="dummy";
  CC_OUT[i][36]="dummy";     

//======== ME-1 =============================  
  i=4;
  
  CC_IN[i][1]="dummy";
  CC_IN[i][2]="dummy";
  CC_IN[i][3]="dummy";  
  CC_IN[i][4]="dummy";
  CC_IN[i][5]="dummy";
  CC_IN[i][6]="dummy";
  CC_IN[i][7]="dummy";  
  CC_IN[i][8]="dummy";
  CC_IN[i][9]="dummy";  
  CC_IN[i][10]="dummy";
  CC_IN[i][11]="dummy";
  CC_IN[i][12]="dummy";
  CC_IN[i][13]="dummy";   
  CC_IN[i][14]="dummy";
  CC_IN[i][15]="dummy";  
  CC_IN[i][16]="dummy";
  CC_IN[i][17]="dummy";
  CC_IN[i][18]="dummy";
  CC_IN[i][19]="dummy";
  CC_IN[i][20]="dummy";
  CC_IN[i][21]="dummy";
  CC_IN[i][22]="dummy";    
  CC_IN[i][23]="dummy";
  CC_IN[i][24]="dummy";
  CC_IN[i][25]="dummy";
  CC_IN[i][26]="dummy";
  CC_IN[i][27]="dummy";
  CC_IN[i][28]="dummy";    
  CC_IN[i][29]="dummy";
  CC_IN[i][30]="dummy";
  CC_IN[i][31]="dummy";
  CC_IN[i][32]="dummy";
  CC_IN[i][33]="dummy";
  CC_IN[i][34]="dummy";    
  CC_IN[i][35]="dummy";
  CC_IN[i][36]="dummy";      
  
  CC_OUT[i][1]="dummy";
  CC_OUT[i][2]="dummy";
  CC_OUT[i][3]="dummy";  
  CC_OUT[i][4]="dummy";
  CC_OUT[i][5]="dummy";
  CC_OUT[i][6]="dummy";
  CC_OUT[i][7]="dummy";  
  CC_OUT[i][8]="dummy";
  CC_OUT[i][9]="dummy";  
  CC_OUT[i][10]="dummy";
  CC_OUT[i][11]="dummy";
  CC_OUT[i][12]="dummy";
  CC_OUT[i][13]="dummy";   
  CC_OUT[i][14]="dummy";
  CC_OUT[i][15]="dummy";  
  CC_OUT[i][16]="dummy";
  CC_OUT[i][17]="dummy";
  CC_OUT[i][18]="dummy";     
  CC_OUT[i][19]="dummy";
  CC_OUT[i][20]="dummy";
  CC_OUT[i][21]="dummy";  
  CC_OUT[i][22]="dummy";
  CC_OUT[i][23]="dummy";
  CC_OUT[i][24]="dummy";
  CC_OUT[i][25]="dummy";  
  CC_OUT[i][26]="dummy";
  CC_OUT[i][27]="dummy";  
  CC_OUT[i][28]="dummy";
  CC_OUT[i][29]="dummy";
  CC_OUT[i][30]="dummy";
  CC_OUT[i][31]="dummy";   
  CC_OUT[i][32]="dummy";
  CC_OUT[i][33]="dummy";  
  CC_OUT[i][34]="dummy";
  CC_OUT[i][35]="dummy";
  CC_OUT[i][36]="dummy"; 
    
//======== ME+1 =============================  
  i=5;
  
  CC_IN[i][1]="dummy"; //sector6
  CC_IN[i][2]="dummy";//sector6
  CC_IN[i][3]="dummy"; //sector 1
  CC_IN[i][4]="dummy";//sector1
  CC_IN[i][5]="dummy";//sector1
  CC_IN[i][6]="dummy";//sector1
  CC_IN[i][7]="dummy"; //sector 1
  CC_IN[i][8]="dummy";//sector1
  CC_IN[i][9]="02:00:00:00:00:20;9";  //sector2
  CC_IN[i][10]="02:00:00:00:00:20;11";//sector2
  CC_IN[i][11]="02:00:00:00:00:20;15";//sector2
  CC_IN[i][12]="02:00:00:00:00:24;9";//sector2
  CC_IN[i][13]="02:00:00:00:00:24;11";  //sector 2
  CC_IN[i][14]="02:00:00:00:00:24;15";//sector2
  CC_IN[i][15]="02:00:00:00:00:34;9"; //sector 3
  CC_IN[i][16]="02:00:00:00:00:34;11";//sector3
  CC_IN[i][17]="02:00:00:00:00:34;15";//sector3
  CC_IN[i][18]="02:00:00:00:00:35;9"; //sector 3  
  CC_IN[i][19]="02:00:00:00:00:35;11";//sector3
  CC_IN[i][20]="02:00:00:00:00:35;15";//sector3
  CC_IN[i][21]="02:00:00:00:00:40;9";//sector4
  CC_IN[i][22]="02:00:00:00:00:40;11"; //sector 4  
  CC_IN[i][23]="02:00:00:00:00:40;15";//sector4
  CC_IN[i][24]="02:00:00:00:00:41;9";//sector4
  CC_IN[i][25]="02:00:00:00:00:41;11";//sector4
  CC_IN[i][26]="02:00:00:00:00:41;15";//sector4
  CC_IN[i][27]="dummy";//sector5
  CC_IN[i][28]="dummy"; //sector 5  
  CC_IN[i][29]="dummy";//sector5
  CC_IN[i][30]="dummy";//sector5
  CC_IN[i][31]="dummy";//sector5
  CC_IN[i][32]="dummy";//sector5
  CC_IN[i][33]="dummy";//sector6
  CC_IN[i][34]="dummy";  //sector 6 
  CC_IN[i][35]="dummy";//sector6
  CC_IN[i][36]="dummy";//sector6
              
  CC_OUT[i][1]="dummy";//sector6
  CC_OUT[i][2]="dummy";//sector6
  CC_OUT[i][3]="dummy";  //sector1
  CC_OUT[i][4]="dummy";//sector1
  CC_OUT[i][5]="dummy";//sector1
  CC_OUT[i][6]="dummy";//sector1
  CC_OUT[i][7]="dummy"; //sector 1
  CC_OUT[i][8]="dummy";//sector1
  CC_OUT[i][9]="02:00:00:00:00:20;17"; //sector 2
  CC_OUT[i][10]="02:00:00:00:00:20;19";//sector2
  CC_OUT[i][11]="02:00:00:00:00:20;21";//sector2
  CC_OUT[i][12]="02:00:00:00:00:24;17";//sector2
  CC_OUT[i][13]="02:00:00:00:00:24;19";  //sector 2
  CC_OUT[i][14]="02:00:00:00:00:24;21";//sector2
  CC_OUT[i][15]="02:00:00:00:00:34;17"; //sector 3
  CC_OUT[i][16]="02:00:00:00:00:34;19";//sector3
  CC_OUT[i][17]="02:00:00:00:00:34;21";//sector3
  CC_OUT[i][18]="02:00:00:00:00:35;17";  //sector 3  
  CC_OUT[i][19]="02:00:00:00:00:35;19";//sector3
  CC_OUT[i][20]="02:00:00:00:00:35;21";//sector3
  CC_OUT[i][21]="02:00:00:00:00:40;17";  //sector4
  CC_OUT[i][22]="02:00:00:00:00:40;19";//sector4
  CC_OUT[i][23]="02:00:00:00:00:40;21";//sector4
  CC_OUT[i][24]="02:00:00:00:00:41;17";//sector4
  CC_OUT[i][25]="02:00:00:00:00:41;19";  //sector4
  CC_OUT[i][26]="02:00:00:00:00:41;21";//sector4
  CC_OUT[i][27]="dummy";  //sector5
  CC_OUT[i][28]="dummy";//sector5
  CC_OUT[i][29]="dummy";//sector5
  CC_OUT[i][30]="dummy";//sector5
  CC_OUT[i][31]="dummy";  //sector 5
  CC_OUT[i][32]="dummy";//sector5
  CC_OUT[i][33]="dummy";  //sector6
  CC_OUT[i][34]="dummy";//sector6
  CC_OUT[i][35]="dummy";//sector6
  CC_OUT[i][36]="dummy";  //sector 6  

//======== ME+2 =============================  
  i=6;
  
  CC_IN[i][1]="02:00:00:00:00:0e;7";//sector 6
  CC_IN[i][2]="02:00:00:00:00:12;3";//sector 1
  CC_IN[i][3]="02:00:00:00:00:12;5";//sector 1
  CC_IN[i][4]="02:00:00:00:00:12;7";//sector 1
  CC_IN[i][5]="dummy";//"02:00:00:00:00:;3";//sector  2
  CC_IN[i][6]="dummy";//"02:00:00:00:00:;5";//sector  2
  CC_IN[i][7]="dummy";//"02:00:00:00:00:;7";//sector  2  
  CC_IN[i][8]="dummy";//"02:00:00:00:00:;3";//sector  3
  CC_IN[i][9]="dummy";//"02:00:00:00:00:;5";//sector  3  
  CC_IN[i][10]="dummy";//"02:00:00:00:00:;7";//sector  3
  CC_IN[i][11]="dummy";//"02:00:00:00:00:;3";//sector  4
  CC_IN[i][12]="dummy";//"02:00:00:00:00:;5";//sector  4
  CC_IN[i][13]="dummy";//"02:00:00:00:00:;7";//sector  4   
  CC_IN[i][14]="dummy";//"02:00:00:00:00:;3";//sector  5
  CC_IN[i][15]="dummy";//"02:00:00:00:00:;5";//sector  5  
  CC_IN[i][16]="dummy";//"02:00:00:00:00:;7";//sector  5
  CC_IN[i][17]="02:00:00:00:00:0e;3"; //sector 6
  CC_IN[i][18]="02:00:00:00:00:0e;5"; //sector 6   
  
  CC_OUT[i][1]="02:00:00:00:00:0e;19"; //sector 6 
  CC_OUT[i][2]="02:00:00:00:00:0e;21"; //sector 6 
  CC_OUT[i][3]="02:00:00:00:00:12;9"; //sector 1 
  CC_OUT[i][4]="02:00:00:00:00:12;11";//sector 1
  CC_OUT[i][5]="02:00:00:00:00:12;15";//sector 1
  CC_OUT[i][6]="02:00:00:00:00:12;17";//sector 1
  CC_OUT[i][7]="02:00:00:00:00:12;19";//sector 1
  CC_OUT[i][8]="02:00:00:00:00:12;21";//sector 1
  CC_OUT[i][9]="dummy"; //"02:00:00:00:00:;9";//sector  2
  CC_OUT[i][10]="dummy";//"02:00:00:00:00:;11";//sector  2
  CC_OUT[i][11]="dummy";//"02:00:00:00:00:;15";//sector  2
  CC_OUT[i][12]="dummy";//"02:00:00:00:00:;17";//sector  2
  CC_OUT[i][13]="dummy";//"02:00:00:00:00:;19";//sector  2  
  CC_OUT[i][14]="dummy";//"02:00:00:00:00:;21";//sector  2
  CC_OUT[i][15]="dummy";//"02:00:00:00:00:;9";//sector  3
  CC_OUT[i][16]="dummy";//"02:00:00:00:00:;11";//sector  3
  CC_OUT[i][17]="dummy";//"02:00:00:00:00:;15";//sector  3
  CC_OUT[i][18]="dummy";//"02:00:00:00:00:;17";//sector  3     
  CC_OUT[i][19]="dummy";//"02:00:00:00:00:;19";//sector  3
  CC_OUT[i][20]="dummy";//"02:00:00:00:00:;21";//sector  3
  CC_OUT[i][21]="dummy";//"02:00:00:00:00:;9";//sector  4  
  CC_OUT[i][22]="dummy";//"02:00:00:00:00:;11";//sector  4 
  CC_OUT[i][23]="dummy";//"02:00:00:00:00:;15";//sector  4 
  CC_OUT[i][24]="dummy";//"02:00:00:00:00:;17";//sector  4 
  CC_OUT[i][25]="dummy";//"02:00:00:00:00:;19";//sector  4   
  CC_OUT[i][26]="dummy";//"02:00:00:00:00:;21";//sector  4 
  CC_OUT[i][27]="dummy";//"02:00:00:00:00:;9";//sector  5  
  CC_OUT[i][28]="dummy";//"02:00:00:00:00:;11";//sector  5
  CC_OUT[i][29]="dummy";//"02:00:00:00:00:;15";//sector  5
  CC_OUT[i][30]="dummy";//"02:00:00:00:00:;17";//sector  5
  CC_OUT[i][31]="dummy";//"02:00:00:00:00:;19";//sector  5   
  CC_OUT[i][32]="dummy";//"02:00:00:00:00:;21";//sector  5
  CC_OUT[i][33]="02:00:00:00:00:0e;9"; //sector 6   
  CC_OUT[i][34]="02:00:00:00:00:0e;11"; //sector 6 
  CC_OUT[i][35]="02:00:00:00:00:0e;15"; //sector 6 
  CC_OUT[i][36]="02:00:00:00:00:0e;17"; //sector 6     
  
//======== ME+3 =============================  
  i=7;

  CC_IN[i][1]="02:00:00:00:00:33;7";//sector 6
  CC_IN[i][2]="02:00:00:00:00:0d;3";//sector 1
  CC_IN[i][3]="02:00:00:00:00:0d;5";//sector 1
  CC_IN[i][4]="02:00:00:00:00:0d;7";//sector 1
  CC_IN[i][5]="dummy";//"02:00:00:00:00:;3";//sector  2
  CC_IN[i][6]="dummy";//"02:00:00:00:00:;5";//sector  2
  CC_IN[i][7]="dummy";//"02:00:00:00:00:;7";//sector  2  
  CC_IN[i][8]="dummy";//"02:00:00:00:00:;3";//sector  3
  CC_IN[i][9]="dummy";//"02:00:00:00:00:;5";//sector  3  
  CC_IN[i][10]="dummy";//"02:00:00:00:00:;7";//sector  3
  CC_IN[i][11]="dummy";//"02:00:00:00:00:;3";//sector  4
  CC_IN[i][12]="dummy";//"02:00:00:00:00:;5";//sector  4
  CC_IN[i][13]="dummy";//"02:00:00:00:00:;7";//sector  4   
  CC_IN[i][14]="dummy";//"02:00:00:00:00:;3";//sector  5
  CC_IN[i][15]="dummy";//"02:00:00:00:00:;5";//sector  5  
  CC_IN[i][16]="dummy";//"02:00:00:00:00:;7";//sector  5
  CC_IN[i][17]="02:00:00:00:00:33;3"; //sector 6
  CC_IN[i][18]="02:00:00:00:00:33;5"; //sector 6   
  
  CC_OUT[i][1]="02:00:00:00:00:;19"; //sector 6 
  CC_OUT[i][2]="02:00:00:00:00:;21"; //sector 6 
  CC_OUT[i][3]="02:00:00:00:00:0d;9"; //sector 1 
  CC_OUT[i][4]="02:00:00:00:00:0d;11";//sector 1
  CC_OUT[i][5]="02:00:00:00:00:0d;15";//sector 1
  CC_OUT[i][6]="02:00:00:00:00:0d;17";//sector 1
  CC_OUT[i][7]="02:00:00:00:00:0d;19";//sector 1
  CC_OUT[i][8]="02:00:00:00:00:0d;21";//sector 1
  CC_OUT[i][9]="dummy"; //"02:00:00:00:00:;9";//sector  2
  CC_OUT[i][10]="dummy";//"02:00:00:00:00:;11";//sector  2
  CC_OUT[i][11]="dummy";//"02:00:00:00:00:;15";//sector  2
  CC_OUT[i][12]="dummy";//"02:00:00:00:00:;17";//sector  2
  CC_OUT[i][13]="dummy";//"02:00:00:00:00:;19";//sector  2  
  CC_OUT[i][14]="dummy";//"02:00:00:00:00:;21";//sector  2
  CC_OUT[i][15]="dummy";//"02:00:00:00:00:33;9";//sector  3
  CC_OUT[i][16]="dummy";//"02:00:00:00:00:33;11";//sector  3
  CC_OUT[i][17]="dummy";//"02:00:00:00:00:33;15";//sector  3
  CC_OUT[i][18]="dummy";//"02:00:00:00:00:33;17";//sector  3     
  CC_OUT[i][19]="dummy";//"02:00:00:00:00:33;19";//sector  3
  CC_OUT[i][20]="dummy";//"02:00:00:00:00:33;21";//sector  3
  CC_OUT[i][21]="dummy";//"02:00:00:00:00:;9";//sector  4  
  CC_OUT[i][22]="dummy";//"02:00:00:00:00:;11";//sector  4 
  CC_OUT[i][23]="dummy";//"02:00:00:00:00:;15";//sector  4 
  CC_OUT[i][24]="dummy";//"02:00:00:00:00:;17";//sector  4 
  CC_OUT[i][25]="dummy";//"02:00:00:00:00:;19";//sector  4   
  CC_OUT[i][26]="dummy";//"02:00:00:00:00:;21";//sector  4 
  CC_OUT[i][27]="dummy";//"02:00:00:00:00:;9";//sector  5  
  CC_OUT[i][28]="dummy";//"02:00:00:00:00:;11";//sector  5
  CC_OUT[i][29]="dummy";//"02:00:00:00:00:;15";//sector  5
  CC_OUT[i][30]="dummy";//"02:00:00:00:00:;17";//sector  5
  CC_OUT[i][31]="dummy";//"02:00:00:00:00:;19";//sector  5   
  CC_OUT[i][32]="dummy";//"02:00:00:00:00:;21";//sector  5
  CC_OUT[i][33]="02:00:00:00:00:;9"; //sector 6   
  CC_OUT[i][34]="02:00:00:00:00:;11"; //sector 6 
  CC_OUT[i][35]="02:00:00:00:00:;15"; //sector 6 
  CC_OUT[i][36]="02:00:00:00:00:;17"; //sector 6    
 

//======== ME+4 =============================  
  i=8;
  
  CC_IN[i][1]="dummy";
  CC_IN[i][2]="dummy";
  CC_IN[i][3]="dummy";  
  CC_IN[i][4]="dummy";
  CC_IN[i][5]="dummy";
  CC_IN[i][6]="dummy";
  CC_IN[i][7]="dummy";  
  CC_IN[i][8]="dummy";
  CC_IN[i][9]="dummy";  
  CC_IN[i][10]="dummy";
  CC_IN[i][11]="dummy";
  CC_IN[i][12]="dummy";
  CC_IN[i][13]="dummy";   
  CC_IN[i][14]="dummy";
  CC_IN[i][15]="dummy";  
  CC_IN[i][16]="dummy";
  CC_IN[i][17]="dummy";
  CC_IN[i][18]="dummy";                    
  
  CC_OUT[i][1]=""; //sector 6 
  CC_OUT[i][2]=""; //sector 6 
  CC_OUT[i][3]=""; //sector 1 
  CC_OUT[i][4]="";//sector 1
  CC_OUT[i][5]="";//sector 1
  CC_OUT[i][6]="";//sector 1
  CC_OUT[i][7]="";//sector 1
  CC_OUT[i][8]="";//sector 1
  CC_OUT[i][9]="dummy"; ////sector  2
  CC_OUT[i][10]="dummy";////sector  2
  CC_OUT[i][11]="dummy";///sector  2
  CC_OUT[i][12]="dummy";////sector  2
  CC_OUT[i][13]="dummy";////sector  2  
  CC_OUT[i][14]="";////sector  2
  CC_OUT[i][15]="";////sector  3
  CC_OUT[i][16]="";////sector  3
  CC_OUT[i][17]="";////sector  3
  CC_OUT[i][18]="";////sector  3     
  CC_OUT[i][19]="";////sector  3
  CC_OUT[i][20]="";////sector  3
  CC_OUT[i][21]="";////sector  4  
  CC_OUT[i][22]="";////sector  4 
  CC_OUT[i][23]="";////sector  4 
  CC_OUT[i][24]="";////sector  4 
  CC_OUT[i][25]="";////sector  4   
  CC_OUT[i][26]="";////sector  4 
  CC_OUT[i][27]="";///sector  5  
  CC_OUT[i][28]="";////sector  5
  CC_OUT[i][29]="";////sector  5
  CC_OUT[i][30]="";////sector  5
  CC_OUT[i][31]="";////sector  5   
  CC_OUT[i][32]="";////sector  5
  CC_OUT[i][33]=""; //sector 6   
  CC_OUT[i][34]=""; //sector 6 
  CC_OUT[i][35]=""; //sector 6 
  CC_OUT[i][36]=""; //sector 6   


//======== ME-1 (DUBNA) =============================  
  
  
  CC_DUBNA[1]="dummy"; //
  CC_DUBNA[2]="dummy";//
  CC_DUBNA[3]="dummy";  //
  CC_DUBNA[4]="dummy";//
  CC_DUBNA[5]="dummy";//
  CC_DUBNA[6]="dummy";//
  CC_DUBNA[7]="dummy";  //
  CC_DUBNA[8]="dummy";//
  CC_DUBNA[9]="dummy";  //
  CC_DUBNA[10]="dummy";//
  CC_DUBNA[11]="dummy";//
  CC_DUBNA[12]="dummy";//
  CC_DUBNA[13]="dummy";  // 
  CC_DUBNA[14]="dummy";//
  CC_DUBNA[15]="dummy";  //
  CC_DUBNA[16]="dummy";//
  CC_DUBNA[17]="dummy";//
  CC_DUBNA[18]="dummy";  //   
  CC_DUBNA[19]="dummy";//
  CC_DUBNA[20]="dummy";//
  CC_DUBNA[21]="dummy";  //
  CC_DUBNA[22]="dummy";//
  CC_DUBNA[23]="dummy";//
  CC_DUBNA[24]="dummy";//
  CC_DUBNA[25]="dummy";  //
  CC_DUBNA[26]="dummy";//
  CC_DUBNA[27]="dummy";  //
  CC_DUBNA[28]="dummy";//
  CC_DUBNA[29]="dummy";//
  CC_DUBNA[30]="dummy";//
  CC_DUBNA[31]="dummy";  // 
  CC_DUBNA[32]="dummy";//
  CC_DUBNA[33]="dummy";  //
  CC_DUBNA[34]="dummy";//
  CC_DUBNA[35]="dummy";//
  CC_DUBNA[36]="dummy";  //   

//======== ME+1 (DUBNA) =============================  
  
  
  CC_DUBNA[37]="dummy";//6
  CC_DUBNA[38]="dummy";//6
  CC_DUBNA[39]="dummy";  //1
  CC_DUBNA[40]="dummy";//1
  CC_DUBNA[41]="dummy";//1
  CC_DUBNA[42]="dummy";//1
  CC_DUBNA[43]="dummy";  //1
  CC_DUBNA[44]="dummy";//1
  CC_DUBNA[45]="02:00:00:00:00:20;3";//2  
  CC_DUBNA[46]="02:00:00:00:00:20;5";//2
  CC_DUBNA[47]="02:00:00:00:00:20;7";//2
  CC_DUBNA[48]="02:00:00:00:00:24;3";//2
  CC_DUBNA[49]="02:00:00:00:00:24;5";  //2 
  CC_DUBNA[50]="02:00:00:00:00:24;7";//2
  CC_DUBNA[51]="02:00:00:00:00:34;3";  //3
  CC_DUBNA[52]="02:00:00:00:00:34;5";//3
  CC_DUBNA[53]="02:00:00:00:00:34;7";//3
  CC_DUBNA[54]="02:00:00:00:00:35;3";  //3   
  CC_DUBNA[55]="02:00:00:00:00:35;5";//3
  CC_DUBNA[56]="02:00:00:00:00:35;7";//3
  CC_DUBNA[57]="02:00:00:00:00:40;3";  //4
  CC_DUBNA[58]="02:00:00:00:00:40;5";//4
  CC_DUBNA[59]="02:00:00:00:00:40;7";//4
  CC_DUBNA[60]="02:00:00:00:00:41;3";//4
  CC_DUBNA[61]="02:00:00:00:00:41;5";  //4
  CC_DUBNA[62]="02:00:00:00:00:41;7";//4
  CC_DUBNA[63]="dummy";  //5
  CC_DUBNA[64]="dummy";//5
  CC_DUBNA[65]="dummy";//5
  CC_DUBNA[66]="dummy";//5
  CC_DUBNA[67]="dummy";  //5 
  CC_DUBNA[68]="dummy";//5
  CC_DUBNA[69]="dummy";  //6
  CC_DUBNA[70]="dummy";//6
  CC_DUBNA[71]="dummy";//6
  CC_DUBNA[72]="dummy";  //6   
  
  
}
//============================================  
//======= HV SECTION ==========
//============================================  
  
db_set_hv(){

int i;  
//======== ME-4 =============================  
  i=1;
  
  HV_IN[i][1]="500;13;2;2";
  HV_IN[i][2]="500;13;2;1";
  HV_IN[i][3]="500;13;3;2";
  HV_IN[i][4]="500;13;3;1";
  HV_IN[i][5]="500;13;4;2";
  HV_IN[i][6]="500;13;4;1";
  HV_IN[i][7]="500;13;5;2";
  HV_IN[i][8]="500;13;5;1";
  HV_IN[i][9]="500;13;6;2";
  HV_IN[i][10]="500;13;6;1";
  HV_IN[i][11]="500;13;7;2";
  HV_IN[i][12]="500;13;7;1";
  HV_IN[i][13]="500;13;8;2"; 
  HV_IN[i][14]="500;13;8;1";
  HV_IN[i][15]="500;13;9;2";
  HV_IN[i][16]="500;13;9;1";
  HV_IN[i][17]="500;13;1;2";   // 
  HV_IN[i][18]="500;13;1;1";   // 
  
//======== ME-3 =============================  
  i=2;
  
  HV_IN[i][1]="500;7;2;2";
  HV_IN[i][2]="500;7;3;2";
  HV_IN[i][3]="500;7;4;2";
  HV_IN[i][4]="500;7;5;2";
  HV_IN[i][5]="500;7;6;2";
  HV_IN[i][6]="500;7;7;2";
  HV_IN[i][7]="500;7;8;2";
  HV_IN[i][8]="500;11;4;2"; //11.4.1
  HV_IN[i][9]="500;11;5;1"; 
  HV_IN[i][10]="500;11;5;2";
  HV_IN[i][11]="500;11;6;1";
  HV_IN[i][12]="500;11;6;2";
  HV_IN[i][13]="500;11;7;1"; 
  HV_IN[i][14]="500;11;7;2";
  HV_IN[i][15]="500;11;8;1";
  HV_IN[i][16]="500;11;8;2";
  HV_IN[i][17]="500;7;15;2";
  HV_IN[i][18]="500;7;1;2";  
  
  HV_OUT[i][1]="500;6;10;0";
  HV_OUT[i][2]="500;6;9;0";
  HV_OUT[i][3]="500;7;14;0";
  HV_OUT[i][4]="500;7;13;0";
  HV_OUT[i][5]="500;7;12;0";
  HV_OUT[i][6]="500;5;14;0";
  HV_OUT[i][7]="500;5;13;0";
  HV_OUT[i][8]="500;5;12;0";
  HV_OUT[i][9]="500;5;11;0";
  HV_OUT[i][10]="500;5;10;0";
  HV_OUT[i][11]="500;5;9;0";
  HV_OUT[i][12]="500;7;9;0";
  HV_OUT[i][13]="500;7;10;0";
  HV_OUT[i][14]="500;7;11;0";
  HV_OUT[i][15]="500;10;14;0"; 
  HV_OUT[i][16]="500;10;13;0";
  HV_OUT[i][17]="500;10;12;0";
  HV_OUT[i][18]="500;10;11;0";    
  HV_OUT[i][19]="500;10;10;0";
  HV_OUT[i][20]="500;10;9;0";
  HV_OUT[i][21]="500;10;8;0";
  HV_OUT[i][22]="500;11;10;0";
  HV_OUT[i][23]="500;11;11;0";
  HV_OUT[i][24]="500;9;14;0";
  HV_OUT[i][25]="500;9;13;0";
  HV_OUT[i][26]="500;9;12;0";
  HV_OUT[i][27]="500;9;11;0";
  HV_OUT[i][28]="500;9;10;0";
  HV_OUT[i][29]="500;9;9;0";
  HV_OUT[i][30]="500;9;8;0";
  HV_OUT[i][31]="500;11;13;0"; 
  HV_OUT[i][32]="500;11;14;0";
  HV_OUT[i][33]="500;6;14;0";
  HV_OUT[i][34]="500;6;13;0";
  HV_OUT[i][35]="500;6;12;0";
  HV_OUT[i][36]="500;6;11;0";   

//======== ME-2 =============================  
  i=3;
  
  HV_IN[i][1]="500;7;2;1";
  HV_IN[i][2]="500;7;3;1";
  HV_IN[i][3]="500;7;4;1";
  HV_IN[i][4]="500;7;5;1";
  HV_IN[i][5]="500;7;6;1";
  HV_IN[i][6]="500;7;7;1";
  HV_IN[i][7]="500;7;8;1";
  HV_IN[i][8]="500;11;15;1";
  HV_IN[i][9]="500;11;15;2";
  HV_IN[i][10]="500;11;1;1";
  HV_IN[i][11]="500;11;1;2";
  HV_IN[i][12]="500;11;2;1";
  HV_IN[i][13]="500;11;2;2"; 
  HV_IN[i][14]="500;11;3;1";
  HV_IN[i][15]="500;11;3;2";
  HV_IN[i][16]="500;11;4;1"; // 11,4,2
  HV_IN[i][17]="500;7;15;1";
  HV_IN[i][18]="500;7;1;1";  
  
  HV_OUT[i][1]="500;6;4;0";
  HV_OUT[i][2]="500;6;3;0";
  HV_OUT[i][3]="500;6;2;0";
  HV_OUT[i][4]="500;6;1;0";
  HV_OUT[i][5]="500;6;15;0";
  HV_OUT[i][6]="500;5;8;0";
  HV_OUT[i][7]="500;5;7;0";
  HV_OUT[i][8]="500;5;6;0";
  HV_OUT[i][9]="500;5;5;0";
  HV_OUT[i][10]="500;5;4;0";
  HV_OUT[i][11]="500;5;3;0";
  HV_OUT[i][12]="500;5;2;0";
  HV_OUT[i][13]="500;5;1;0"; 
  HV_OUT[i][14]="500;5;15;0";
  HV_OUT[i][15]="500;10;7;0";  
  HV_OUT[i][16]="500;10;6;0";
  HV_OUT[i][17]="500;10;5;0";
  HV_OUT[i][18]="500;10;4;0";   
  HV_OUT[i][19]="500;10;3;0";
  HV_OUT[i][20]="500;10;2;0";
  HV_OUT[i][21]="500;10;1;0";
  HV_OUT[i][22]="500;10;15;0";
  HV_OUT[i][23]="500;11;9;0";
  HV_OUT[i][24]="500;9;7;0";
  HV_OUT[i][25]="500;9;6;0";
  HV_OUT[i][26]="500;9;5;0";
  HV_OUT[i][27]="500;9;4;0";
  HV_OUT[i][28]="500;9;3;0";
  HV_OUT[i][29]="500;9;2;0";
  HV_OUT[i][30]="500;9;1;0";
  HV_OUT[i][31]="500;9;15;0"; 
  HV_OUT[i][32]="500;11;12;0";
  HV_OUT[i][33]="500;6;8;0";
  HV_OUT[i][34]="500;6;7;0";
  HV_OUT[i][35]="500;6;6;0";
  HV_OUT[i][36]="500;6;5;0";   

//======== ME-1 =============================  
  i=4;
  
  HV_IN[i][1]="500;2;1;2";
  HV_IN[i][2]="500;2;1;1";
  HV_IN[i][3]="500;3;9;2";
  HV_IN[i][4]="500;3;9;1";
  HV_IN[i][5]="500;3;8;2";
  HV_IN[i][6]="500;3;8;1";
  HV_IN[i][7]="500;3;7;2";
  HV_IN[i][8]="500;3;7;1";
  HV_IN[i][9]="500;3;6;2";
  HV_IN[i][10]="500;3;6;1";
  HV_IN[i][11]="500;3;5;2";
  HV_IN[i][12]="500;3;5;1";
  HV_IN[i][13]="500;3;4;2"; 
  HV_IN[i][14]="500;3;4;1";
  HV_IN[i][15]="500;3;3;2";
  HV_IN[i][16]="500;3;3;1";
  HV_IN[i][17]="500;3;2;2";
  HV_IN[i][18]="500;3;2;1";  
  HV_IN[i][19]="500;3;1;2";
  HV_IN[i][20]="500;3;1;1";
  HV_IN[i][21]="500;2;9;2";
  HV_IN[i][22]="500;2;9;1";
  HV_IN[i][23]="500;2;8;2";
  HV_IN[i][24]="500;2;8;1";
  HV_IN[i][25]="500;2;7;2";
  HV_IN[i][26]="500;2;7;1";
  HV_IN[i][27]="500;2;6;2";
  HV_IN[i][28]="500;2;6;1";
  HV_IN[i][29]="500;2;5;2";
  HV_IN[i][30]="500;2;5;1";
  HV_IN[i][31]="500;2;4;2"; 
  HV_IN[i][32]="500;2;4;1";
  HV_IN[i][33]="500;2;3;2";
  HV_IN[i][34]="500;2;3;1";
  HV_IN[i][35]="500;2;2;2";
  HV_IN[i][36]="500;2;2;1";  
  
    
  HV_OUT[i][1]="500;1;6;2";
  HV_OUT[i][2]="500;1;6;1";
  HV_OUT[i][3]="500;3;15;2";
  HV_OUT[i][4]="500;3;15;1";
  HV_OUT[i][5]="500;3;14;2";
  HV_OUT[i][6]="500;3;14;1";
  HV_OUT[i][7]="500;3;13;2";
  HV_OUT[i][8]="500;3;13;1";
  HV_OUT[i][9]="500;3;12;2";
  HV_OUT[i][10]="500;3;12;1";
  HV_OUT[i][11]="500;3;11;2";  // 
  HV_OUT[i][12]="500;3;11;1";  // 
  HV_OUT[i][13]="500;3;10;2"; 
  HV_OUT[i][14]="500;3;10;1";
  HV_OUT[i][15]="500;1;1;2";
  HV_OUT[i][16]="500;1;1;1";
  HV_OUT[i][17]="500;1;2;2";
  HV_OUT[i][18]="500;1;2;1";   
  HV_OUT[i][19]="500;1;3;2";
  HV_OUT[i][20]="500;1;3;1";
  HV_OUT[i][21]="500;2;15;2";
  HV_OUT[i][22]="500;2;15;1";
  HV_OUT[i][23]="500;2;14;2";
  HV_OUT[i][24]="500;2;14;1";
  HV_OUT[i][25]="500;2;13;2";
  HV_OUT[i][26]="500;2;13;1";
  HV_OUT[i][27]="500;2;12;2";
  HV_OUT[i][28]="500;2;12;1";
  HV_OUT[i][29]="500;2;11;2";
  HV_OUT[i][30]="500;2;11;1";
  HV_OUT[i][31]="500;2;10;2"; 
  HV_OUT[i][32]="500;2;10;1";
  HV_OUT[i][33]="500;1;4;2";
  HV_OUT[i][34]="500;1;4;1";
  HV_OUT[i][35]="500;1;5;2";
  HV_OUT[i][36]="500;1;5;1";   

    
//======== ME+1 =============================  
  i=5;
     if(slice_test || all_chambers){  
  HV_IN[i][1]="600;3;15;2";
  HV_IN[i][2]="600;3;15;1";
  HV_IN[i][3]="600;2;8;2";
  HV_IN[i][4]="600;2;8;1";
  HV_IN[i][5]="600;2;7;2";
  HV_IN[i][6]="600;2;7;1";
  HV_IN[i][7]="600;2;6;2";
  HV_IN[i][8]="600;2;6;1";
  HV_IN[i][9]="600;2;5;2";
  HV_IN[i][10]="600;2;5;1";
  HV_IN[i][11]="600;2;4;2";
  HV_IN[i][12]="600;2;4;1";
  HV_IN[i][13]="600;2;3;2"; 
  HV_IN[i][14]="600;2;3;1";
  HV_IN[i][15]="600;2;2;2";
  HV_IN[i][16]="600;2;2;1";
  HV_IN[i][17]="600;2;1;2";
  HV_IN[i][18]="600;2;1;1"; 
  HV_IN[i][19]="600;2;15;2";
  HV_IN[i][20]="600;2;15;1";
  HV_IN[i][21]="600;3;8;2";
  HV_IN[i][22]="600;3;8;1";
  HV_IN[i][23]="600;3;7;2";
  HV_IN[i][24]="600;3;7;1";
  HV_IN[i][25]="600;3;6;2";
  HV_IN[i][26]="600;3;6;1";
  HV_IN[i][27]="600;3;5;2";//  
  HV_IN[i][28]="600;3;5;1";//
  HV_IN[i][29]="600;3;4;2";//
  HV_IN[i][30]="600;3;4;1";//
  HV_IN[i][31]="600;3;3;2";// 
  HV_IN[i][32]="600;3;3;1";//
  HV_IN[i][33]="600;3;2;2";
  HV_IN[i][34]="600;3;2;1";
  HV_IN[i][35]="600;3;1;2";
  HV_IN[i][36]="600;3;1;1";    
  
  HV_OUT[i][1]="600;1;6;2";
  HV_OUT[i][2]="600;1;6;1";
  HV_OUT[i][3]="600;2;14;2";
  HV_OUT[i][4]="600;2;14;1";
  HV_OUT[i][5]="600;2;13;2";
  HV_OUT[i][6]="600;2;13;1";
  HV_OUT[i][7]="600;2;12;2";
  HV_OUT[i][8]="600;2;12;1";
  HV_OUT[i][9]="600;2;11;2";
  HV_OUT[i][10]="600;2;11;1";
  HV_OUT[i][11]="600;2;10;2";
  HV_OUT[i][12]="600;2;10;1";
  HV_OUT[i][13]="600;2;9;2"; 
  HV_OUT[i][14]="600;2;9;1";
  HV_OUT[i][15]="600;1;1;2";
  HV_OUT[i][16]="600;1;1;1";
  HV_OUT[i][17]="600;1;2;2";
  HV_OUT[i][18]="600;1;2;1";   
  HV_OUT[i][19]="600;1;3;2";
  HV_OUT[i][20]="600;1;3;1";
  HV_OUT[i][21]="600;3;14;2";
  HV_OUT[i][22]="600;3;14;1";
  HV_OUT[i][23]="600;3;13;2";
  HV_OUT[i][24]="600;3;13;1";
  HV_OUT[i][25]="600;3;12;2";
  HV_OUT[i][26]="600;3;12;1";
  HV_OUT[i][27]="600;3;11;2";//  
  HV_OUT[i][28]="600;3;11;1";//
  HV_OUT[i][29]="600;3;10;2";//
  HV_OUT[i][30]="600;3;10;1";//
  HV_OUT[i][31]="600;3;9;2";//
  HV_OUT[i][32]="600;3;9;1";//
  HV_OUT[i][33]="600;1;4;2";
  HV_OUT[i][34]="600;1;4;1";
  HV_OUT[i][35]="600;1;5;2";
  HV_OUT[i][36]="600;1;5;1"; 
}
//======== ME+2 =============================  
  i=6;
  
  HV_IN[i][1]="600;5;6;2"; //sector 6
  if(CSC_fwG_g_904)HV_IN[i][2]="800;0;4;1"; //sector 1  
  else HV_IN[i][2]="600;5;3;1";//sector 1
  HV_IN[i][3]="600;5;4;2";//sector 1
  HV_IN[i][4]="600;5;5;1";//sector 1
  HV_IN[i][5]="600;5;2;2";
  HV_IN[i][6]="600;5;7;1";
  HV_IN[i][7]="600;5;8;2";
  HV_IN[i][8]="600;9;1;1";
  HV_IN[i][9]="600;9;2;1";
  HV_IN[i][10]="600;9;3;1";
  HV_IN[i][11]="600;9;4;1";
  HV_IN[i][12]="600;9;5;1";
  HV_IN[i][13]="600;9;6;1"; 
  HV_IN[i][14]="600;9;7;1";
  HV_IN[i][15]="600;9;8;1";
  HV_IN[i][16]="600;9;9;1";
  HV_IN[i][17]="600;5;15;2"; //sector 6
  HV_IN[i][18]="600;5;1;1";   //sector 6
  
  HV_OUT[i][1]="600;6;5;0"; //sector 6
  HV_OUT[i][2]="600;6;4;0"; //sector 6
  HV_OUT[i][3]="600;6;3;0";//sector 1
  HV_OUT[i][4]="600;6;2;0";//sector 1
  HV_OUT[i][5]="600;6;1;0";//sector 1
  HV_OUT[i][6]="600;7;9;0";//sector 1
  HV_OUT[i][7]="600;7;8;0";//sector 1
  HV_OUT[i][8]="600;7;7;0";//sector 1
  HV_OUT[i][9]="600;7;6;0";
  HV_OUT[i][10]="600;7;5;0";
  HV_OUT[i][11]="600;7;4;0";
  HV_OUT[i][12]="600;7;3;0";
  HV_OUT[i][13]="600;7;2;0"; 
  HV_OUT[i][14]="600;7;1;0";
  HV_OUT[i][15]="600;10;9;0";
  HV_OUT[i][16]="600;10;8;0";
  HV_OUT[i][17]="600;10;7;0";
  HV_OUT[i][18]="600;10;6;0";   
  HV_OUT[i][19]="600;10;5;0";
  HV_OUT[i][20]="600;10;4;0";
  HV_OUT[i][21]="600;10;3;0";
  HV_OUT[i][22]="600;10;2;0";
  HV_OUT[i][23]="600;10;1;0";
  HV_OUT[i][24]="600;11;15;0";
  HV_OUT[i][25]="600;11;14;0";
  HV_OUT[i][26]="600;11;13;0";
  HV_OUT[i][27]="600;11;12;0";
  HV_OUT[i][28]="600;11;11;0";
  HV_OUT[i][29]="600;11;10;0";
  HV_OUT[i][30]="600;9;13;0";
  HV_OUT[i][31]="600;9;14;0"; 
  HV_OUT[i][32]="600;9;15;0";
  HV_OUT[i][33]="600;6;9;0"; //sector 6
  HV_OUT[i][34]="600;6;8;0"; //sector 6
  HV_OUT[i][35]="600;6;7;0"; //sector 6
  HV_OUT[i][36]="600;6;6;0"; //sector 6 

    
//======== ME+3 =============================  
  i=7;

  HV_IN[i][1]="600;5;2;1"; //sector 6
  HV_IN[i][2]="600;5;3;2";//sector 1
  HV_IN[i][3]="600;5;4;1";//sector 1
  HV_IN[i][4]="600;5;5;2";//sector 1
  HV_IN[i][5]="600;5;6;1";
  HV_IN[i][6]="600;5;7;2";
  HV_IN[i][7]="600;5;8;1";
  HV_IN[i][8]="600;9;1;2";
  HV_IN[i][9]="600;9;2;2";
  HV_IN[i][10]="600;9;3;2";
  HV_IN[i][11]="600;9;4;2";
  HV_IN[i][12]="600;9;5;2";
  HV_IN[i][13]="600;9;6;2"; 
  HV_IN[i][14]="600;9;7;2";
  HV_IN[i][15]="600;9;8;2";
  HV_IN[i][16]="600;9;9;2";
  HV_IN[i][17]="600;5;15;1"; //sector 6
  HV_IN[i][18]="600;5;1;2";  //sector 6 
  
  HV_OUT[i][1]="600;6;14;0"; //sector 6
  HV_OUT[i][2]="600;6;15;0"; //sector 6
  HV_OUT[i][3]="600;5;12;0";//sector 1
  HV_OUT[i][4]="600;5;13;0";//sector 1
  HV_OUT[i][5]="600;5;14;0";//sector 1
  HV_OUT[i][6]="600;7;10;0";//sector 1
  HV_OUT[i][7]="600;7;11;0";//sector 1
  HV_OUT[i][8]="600;7;12;0";//sector 1
  HV_OUT[i][9]="600;7;13;0";
  HV_OUT[i][10]="600;7;14;0";
  HV_OUT[i][11]="600;7;15;0";
  HV_OUT[i][12]="600;5;9;0";
  HV_OUT[i][13]="600;5;10;0"; 
  HV_OUT[i][14]="600;5;11;0";
  HV_OUT[i][15]="600;10;15;0";
  HV_OUT[i][16]="600;10;14;0";
  HV_OUT[i][17]="600;10;13;0";
  HV_OUT[i][18]="600;10;12;0";   
  HV_OUT[i][19]="600;10;11;0";
  HV_OUT[i][20]="600;10;10;0";
  HV_OUT[i][21]="600;9;10;0";
  HV_OUT[i][22]="600;9;11;0";
  HV_OUT[i][23]="600;9;12;0";
  HV_OUT[i][24]="600;11;9;0";
  HV_OUT[i][25]="600;11;8;0";
  HV_OUT[i][26]="600;11;7;0";
  HV_OUT[i][27]="600;11;6;0";
  HV_OUT[i][28]="600;11;5;0";
  HV_OUT[i][29]="600;11;4;0";
  HV_OUT[i][30]="600;11;3;0";
  HV_OUT[i][31]="600;11;2;0"; 
  HV_OUT[i][32]="600;11;1;0";
  HV_OUT[i][33]="600;6;10;0"; //sector 6
  HV_OUT[i][34]="600;6;11;0"; //sector 6
  HV_OUT[i][35]="600;6;12;0"; //sector 6
  HV_OUT[i][36]="600;6;13;0"; //sector 6  

//======== ME+4 =============================  
  i=8;

  HV_IN[i][1]="600;13;9;2";//7;2";
  HV_IN[i][2]="600;13;9;1";//7;1";
  HV_IN[i][3]="600;13;1;2";
  HV_IN[i][4]="600;13;1;1";
  HV_IN[i][5]="600;13;2;2";
  HV_IN[i][6]="600;13;2;1";
  HV_IN[i][7]="600;13;3;2";
  HV_IN[i][8]="600;13;3;1";
  HV_IN[i][9]="600;13;4;2";
  HV_IN[i][10]="600;13;4;1";
  HV_IN[i][11]="600;13;5;2";
  HV_IN[i][12]="600;13;5;1";
  HV_IN[i][13]="600;13;6;2"; 
  HV_IN[i][14]="600;13;6;1";
  HV_IN[i][15]="600;13;7;2";
  HV_IN[i][16]="600;13;7;1";
  HV_IN[i][17]="600;13;8;2";
  HV_IN[i][18]="600;13;8;1";
//  HV_IN[i][19]="";

  HV_OUT[i][1]="";//"600;6;14;0"; //sector 6
  HV_OUT[i][2]="";//"600;6;15;0"; //sector 6
  HV_OUT[i][3]="";//"600;5;12;0";//sector 1
  HV_OUT[i][4]="";//"600;5;13;0";//sector 1
  HV_OUT[i][5]="";//"600;5;14;0";//sector 1
  HV_OUT[i][6]="";//"600;7;10;0";//sector 1
  HV_OUT[i][7]="";//"600;7;11;0";//sector 1
  HV_OUT[i][8]="";//"600;7;12;0";//sector 1
  
  HV_OUT[i][9]="600;13;14;0";
  HV_OUT[i][10]="600;13;11;0";
  HV_OUT[i][11]="600;13;12;0";
  HV_OUT[i][12]="600;13;13;0";
  HV_OUT[i][13]="600;13;10;0"; 
  
  HV_OUT[i][14]="";//"600;5;11;0";
  HV_OUT[i][15]="";//"600;10;15;0";
  HV_OUT[i][16]="";//"600;10;14;0";
  HV_OUT[i][17]="";//"600;10;13;0";
  HV_OUT[i][18]="";//"600;10;12;0";   
  HV_OUT[i][19]="";//"600;10;11;0";
  HV_OUT[i][20]="";//"600;10;10;0";
  HV_OUT[i][21]="";//"600;9;10;0";
  HV_OUT[i][22]="";//"600;9;11;0";
  HV_OUT[i][23]="";//"600;9;12;0";
  HV_OUT[i][24]="";//"600;11;9;0";
  HV_OUT[i][25]="";//"600;11;8;0";
  HV_OUT[i][26]="";//"600;11;7;0";
  HV_OUT[i][27]="";//"600;11;6;0";
  HV_OUT[i][28]="";//"600;11;5;0";
  HV_OUT[i][29]="";//"600;11;4;0";
  HV_OUT[i][30]="";//"600;11;3;0";
  HV_OUT[i][31]="";//"600;11;2;0"; 
  HV_OUT[i][32]="";//"600;11;1;0";
  HV_OUT[i][33]="";//"600;6;10;0"; //sector 6
  HV_OUT[i][34]="";//"600;6;11;0"; //sector 6
  HV_OUT[i][35]="";//"600;6;12;0"; //sector 6
  HV_OUT[i][36]="";//"600;6;13;0"; //sector 6    
  
//======== ME-1 (DUBNA) =============================  // that is dummy: used for the FSM tree creation
  
  
  HV_DUBNA[1]="dummy";
  HV_DUBNA[2]="dummy";
  HV_DUBNA[3]="dummy";  
  HV_DUBNA[4]="dummy";
  HV_DUBNA[5]="dummy";
  HV_DUBNA[6]="dummy";
  HV_DUBNA[7]="dummy";  
  HV_DUBNA[8]="dummy";
  HV_DUBNA[9]="dummy";  
  HV_DUBNA[10]="dummy";
  HV_DUBNA[11]="dummy";
  HV_DUBNA[12]="dummy";
  HV_DUBNA[13]="dummy";   
  HV_DUBNA[14]="dummy";
  HV_DUBNA[15]="dummy";  
  HV_DUBNA[16]="dummy";
  HV_DUBNA[17]="dummy";
  HV_DUBNA[18]="dummy";     
  HV_DUBNA[19]="dummy";
  HV_DUBNA[20]="dummy";
  HV_DUBNA[21]="dummy";  
  HV_DUBNA[22]="dummy";
  HV_DUBNA[23]="dummy";
  HV_DUBNA[24]="dummy";
  HV_DUBNA[25]="dummy";  
  HV_DUBNA[26]="dummy";
  HV_DUBNA[27]="dummy";  
  HV_DUBNA[28]="dummy";
  HV_DUBNA[29]="dummy";
  HV_DUBNA[30]="dummy";
  HV_DUBNA[31]="dummy";   
  HV_DUBNA[32]="dummy";
  HV_DUBNA[33]="dummy";  
  HV_DUBNA[34]="dummy";
  HV_DUBNA[35]="dummy";
  HV_DUBNA[36]="dummy";     

//======== ME+1 (DUBNA) =============================  // that is dummy: used for the FSM tree creation
  
  
  HV_DUBNA[37]="dummy";
  HV_DUBNA[38]="dummy";
  HV_DUBNA[39]="dummy";  
  HV_DUBNA[40]="dummy";
  HV_DUBNA[41]="dummy";
  HV_DUBNA[42]="dummy";
  HV_DUBNA[43]="dummy";  
  HV_DUBNA[44]="dummy";
  HV_DUBNA[45]="dummy";  
  HV_DUBNA[46]="dummy";
  HV_DUBNA[47]="dummy";
  HV_DUBNA[48]="dummy";
  HV_DUBNA[49]="dummy";   
  HV_DUBNA[50]="dummy";
  HV_DUBNA[51]="dummy";  
  HV_DUBNA[52]="dummy";
  HV_DUBNA[53]="dummy";
  HV_DUBNA[54]="dummy";     
  HV_DUBNA[55]="dummy";
  HV_DUBNA[56]="dummy";
  HV_DUBNA[57]="dummy";  
  HV_DUBNA[58]="dummy";
  HV_DUBNA[59]="dummy";
  HV_DUBNA[60]="dummy";
  HV_DUBNA[61]="dummy";  
  HV_DUBNA[62]="dummy";
  HV_DUBNA[63]="dummy";  
  HV_DUBNA[64]="dummy";
  HV_DUBNA[65]="dummy";
  HV_DUBNA[66]="dummy";
  HV_DUBNA[67]="dummy";   
  HV_DUBNA[68]="dummy";
  HV_DUBNA[69]="dummy";  
  HV_DUBNA[70]="dummy";
  HV_DUBNA[71]="dummy";
  HV_DUBNA[72]="dummy";     
    

      
}
//============================================  
//======= DISK LEVEL SECTION ==========
//============================================  

db_set_disk_level(){
int i;  

//======= ME-4 ===============================  
  i=1;
  if(!CSC_fwG_g_904){
  dynAppend(DISK_LEVEL[i],"master500_12_0");
  dynAppend(DISK_LEVEL[i],"master500_12_1");
  }
  else{
  dynAppend(DISK_LEVEL[i],"master0_0_0");    
  }
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");      
//======= ME-3 ===============================  
  i=2;
 // dynAppend(DISK_LEVEL[i],"CRB/CSC_ME_M3_PC3_LV_CRB LVCB_1.ELMB_15;0");//600;;;"; PC1 is Periph crate 1

  if(!CSC_fwG_g_904){  
  dynAppend(DISK_LEVEL[i],"master500_4_0");
  dynAppend(DISK_LEVEL[i],"master500_4_1");
  dynAppend(DISK_LEVEL[i],"master500_4_2");
  dynAppend(DISK_LEVEL[i],"master500_4_3");
  dynAppend(DISK_LEVEL[i],"master500_4_4");
  dynAppend(DISK_LEVEL[i],"master500_4_5");
  dynAppend(DISK_LEVEL[i],"master500_8_0");
  dynAppend(DISK_LEVEL[i],"master500_8_1");
  dynAppend(DISK_LEVEL[i],"master500_8_2");
  dynAppend(DISK_LEVEL[i],"master500_8_3");
  dynAppend(DISK_LEVEL[i],"master500_8_4");
  dynAppend(DISK_LEVEL[i],"master500_8_5");
  }
  else{
  dynAppend(DISK_LEVEL[i],"master0_0_0");    
  }  
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],""); 
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],""); 
  dynAppend(DISK_LEVEL[i],"");         
//======= ME-2 ===============================  
  i=3;
//  dynAppend(DISK_LEVEL[i],"CRB/CSC_ME_M2_PC3_LV_CRB LVCB_1.ELMB_0f;0");//600;;;"; PC1 is Periph crate 1
  if(!CSC_fwG_g_904){
  dynAppend(DISK_LEVEL[i],"master500_4_0");
  dynAppend(DISK_LEVEL[i],"master500_4_1");
  dynAppend(DISK_LEVEL[i],"master500_4_2");
  dynAppend(DISK_LEVEL[i],"master500_4_3");
  dynAppend(DISK_LEVEL[i],"master500_4_4");
  dynAppend(DISK_LEVEL[i],"master500_4_5");
  dynAppend(DISK_LEVEL[i],"master500_8_0");
  dynAppend(DISK_LEVEL[i],"master500_8_2");
  dynAppend(DISK_LEVEL[i],"master500_8_4");
  dynAppend(DISK_LEVEL[i],"master500_8_5");
  }
  else{
  dynAppend(DISK_LEVEL[i],"master0_0_0");    
  }  
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");    
  dynAppend(DISK_LEVEL[i],"");       
//======= ME-1 ===============================  
  i=4;
  if(!CSC_fwG_g_904){
  dynAppend(DISK_LEVEL[i],"master500_0_0");
  dynAppend(DISK_LEVEL[i],"master500_0_1");
  dynAppend(DISK_LEVEL[i],"master500_0_2");
  dynAppend(DISK_LEVEL[i],"master500_0_3");
  dynAppend(DISK_LEVEL[i],"master500_0_4");
  dynAppend(DISK_LEVEL[i],"master500_0_5");
  }
  else{
  dynAppend(DISK_LEVEL[i],"master0_0_0");    
  }  
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");      
//======= ME+1 ===============================  
  i=5;
  if(!CSC_fwG_g_904){  
  dynAppend(DISK_LEVEL[i],"master600_0_0");
  dynAppend(DISK_LEVEL[i],"master600_0_1");
  dynAppend(DISK_LEVEL[i],"master600_0_2");
  dynAppend(DISK_LEVEL[i],"master600_0_3");
  dynAppend(DISK_LEVEL[i],"master600_0_4");
  dynAppend(DISK_LEVEL[i],"master600_0_5");
  dynAppend(DISK_LEVEL[i],"CRB/CSC_ME_P1_PC9_LV_CRB LVCB_1.ELMB_28;0");
  dynAppend(DISK_LEVEL[i],"CRB/CSC_ME_P1_PC10_LV_CRB LVCB_1.ELMB_29;0");
  }
  else{
  dynAppend(DISK_LEVEL[i],"master0_0_0");    
  }  
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");      
//======= ME+2 ===============================  
  i=6;
  if(!CSC_fwG_g_904){  
  dynAppend(DISK_LEVEL[i],"master600_4_0");
  dynAppend(DISK_LEVEL[i],"master600_4_1");
  dynAppend(DISK_LEVEL[i],"master600_4_2");
  dynAppend(DISK_LEVEL[i],"master600_4_3");
  dynAppend(DISK_LEVEL[i],"master600_4_4");
  dynAppend(DISK_LEVEL[i],"master600_4_5");
  dynAppend(DISK_LEVEL[i],"master600_8_0");
  dynAppend(DISK_LEVEL[i],"master600_8_1");
  dynAppend(DISK_LEVEL[i],"master600_8_2");
  dynAppend(DISK_LEVEL[i],"master600_8_3");
  dynAppend(DISK_LEVEL[i],"master600_8_4");
  dynAppend(DISK_LEVEL[i],"master600_8_5");
  }
  else{
  dynAppend(DISK_LEVEL[i],"master0_0_0");    
  }    
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");      
//======= ME+3 ===============================  
  i=7;
  if(!CSC_fwG_g_904){  
  dynAppend(DISK_LEVEL[i],"master600_4_0");
  dynAppend(DISK_LEVEL[i],"master600_4_1");
  dynAppend(DISK_LEVEL[i],"master600_4_2");
  dynAppend(DISK_LEVEL[i],"master600_4_3");
  dynAppend(DISK_LEVEL[i],"master600_4_4");
  dynAppend(DISK_LEVEL[i],"master600_4_5");
  dynAppend(DISK_LEVEL[i],"master600_8_0");
  dynAppend(DISK_LEVEL[i],"master600_8_1");
  dynAppend(DISK_LEVEL[i],"master600_8_2");
  dynAppend(DISK_LEVEL[i],"master600_8_3");
  dynAppend(DISK_LEVEL[i],"master600_8_4");
  dynAppend(DISK_LEVEL[i],"master600_8_5");
  }
  else{
  dynAppend(DISK_LEVEL[i],"master0_0_0");    
  }  
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");      
//======= ME+4 ===============================  
  i=8;
  if(!CSC_fwG_g_904){  
  dynAppend(DISK_LEVEL[i],"master600_12_0");
  dynAppend(DISK_LEVEL[i],"master600_12_1");
  }
  else{
  dynAppend(DISK_LEVEL[i],"master0_0_0");    
  }  
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
  dynAppend(DISK_LEVEL[i],"");
                
  


//======== ME-1 (DUBNA) =============================  
 // !!!! in principle in new organization (windows pvss projects) there is no dubna disk level devices in db 
 DISK_DUBNA[1]=""; 
 
//======== ME+1 (DUBNA) =============================  
 // !!!! in principle in new organization (windows pvss projects) there is no dubna disk level devices in db 
 DISK_DUBNA[37]=""; 

}
//============================================  
//======= EMU LEVEL SECTION ==========
//============================================  

db_set_emu_level(){

 if(CSC_fwG_g_904){
  dynAppend(EMU_LEVEL,"HighVoltage/HV_PR_primary800_0_1 HV_PRIMARY;800;0;1");   
 }
 else{     
  dynAppend(EMU_LEVEL,"GasMonitoring/GAS_MIXERS LVCB_1.ELMB_3F;0");
  dynAppend(EMU_LEVEL,"WeatherStation/WEATHER_SX5 WEATHERSTATION;0");
  dynAppend(EMU_LEVEL,"HighVoltage/HV_PR_primary600_1_1 HV_PRIMARY;600;1;1");
  dynAppend(EMU_LEVEL,"HighVoltage/HV_PR_primary600_1_2 HV_PRIMARY;600;1;2");
  dynAppend(EMU_LEVEL,"HighVoltage/HV_PR_primary600_1_3 HV_PRIMARY;600;1;3");
  dynAppend(EMU_LEVEL,"HighVoltage/HV_PR_primary600_1_4 HV_PRIMARY;600;1;4");
  dynAppend(EMU_LEVEL,"HighVoltage/HV_PR_primary500_1_1 HV_PRIMARY;500;1;1");
  dynAppend(EMU_LEVEL,"HighVoltage/HV_PR_primary500_1_2 HV_PRIMARY;500;1;2");
  dynAppend(EMU_LEVEL,"HighVoltage/HV_PR_primary500_1_3 HV_PRIMARY;500;1;3");
  dynAppend(EMU_LEVEL,"HighVoltage/HV_PR_primary500_1_4 HV_PRIMARY;500;1;4");  
  dynAppend(EMU_LEVEL,"MRTN/MRTN_maraton01 WIENER.CAN0.Crate1;0");
  dynAppend(EMU_LEVEL,"MRTN/MRTN_maraton02 WIENER.CAN0.Crate2;0"); 
  dynAppend(EMU_LEVEL,"WIENER_CRATE_SNMP_600 SNMP;600");//600;;;"; not coordinate is needed
  dynAppend(EMU_LEVEL,"WIENER_CRATE_SNMP_500 SNMP;500");//600;;;"; not coordinate is needed 
 } 
  dynAppend(EMU_LEVEL,"");
  dynAppend(EMU_LEVEL,"");
  dynAppend(EMU_LEVEL,"");
  dynAppend(EMU_LEVEL,"");
  dynAppend(EMU_LEVEL,""); 
  dynAppend(EMU_LEVEL,"");
  dynAppend(EMU_LEVEL,"");
  dynAppend(EMU_LEVEL,"");
  dynAppend(EMU_LEVEL,"");
  dynAppend(EMU_LEVEL,"");
  dynAppend(EMU_LEVEL,""); 
  dynAppend(EMU_LEVEL,"");
  dynAppend(EMU_LEVEL,"");
  dynAppend(EMU_LEVEL,"");
  dynAppend(EMU_LEVEL,"");
  dynAppend(EMU_LEVEL,"");
  dynAppend(EMU_LEVEL,"");                
  
  
  // supposedly the PCToManID and DimServerComputerList should be the same in indexes



  
  DimServerComputerList[1]="smi";
  if(CSC_fwG_g_idisk_cross_numbers[1]>=5 && CSC_fwG_g_idisk_cross_numbers[1]<=8){
    if(CSC_fwG_g_904) DimServerComputerList[2]=CSC_fwG_g_904_MACHINE;
    else DimServerComputerList[2]="csc-dcs-pc1";
  }
  else {
   if(CSC_fwG_g_904)DimServerComputerList[2]=CSC_fwG_g_904_MACHINE;    
   else DimServerComputerList[2]="csc-dcs-pc2";    
  }
  
if(CSC_fwG_g_904){
  DimServerComputerList[3]= CSC_fwG_g_904_HV_MACHINE+"_part1";
  DimServerComputerList[4]= CSC_fwG_g_904_HV_MACHINE+"_part2";
  DimServerComputerList[5]= CSC_fwG_g_904_HV_MACHINE+"_part3";
  DimServerComputerList[6]= CSC_fwG_g_904_HV_MACHINE+"_part4";
  DimServerComputerList[7]="";    
}
else{    
  if(CSC_fwG_g_idisk_cross_numbers[1]>=5 && CSC_fwG_g_idisk_cross_numbers[1]<=8){
  DimServerComputerList[3]="10.176.11.103_part1"; // ufcmshv2--cms
  DimServerComputerList[4]="10.176.11.103_part2";
  DimServerComputerList[5]="10.176.11.103_part3";
  DimServerComputerList[6]="10.176.11.103_part4";
  if(CSC_fwG_g_idisk_cross_numbers[1]==CSC_fwG_g_StationWithFsmHwTree)DimServerComputerList[4]="csc-dcs-pc2"; 
  }
  else{
  DimServerComputerList[3]="10.176.11.67_part1"; // ufcmshv1--cms
  DimServerComputerList[4]="10.176.11.67_part2";
  DimServerComputerList[5]="10.176.11.67_part3";
  DimServerComputerList[6]="10.176.11.67_part4"; 
 if(CSC_fwG_g_idisk_cross_numbers[1]==CSC_fwG_g_StationWithFsmHwTree)DimServerComputerList[4]="csc-dcs-pc1";  
  }
  if(dynContains(CSC_fwG_g_idisk_cross_numbers,CSC_fwG_g_StationWithFsmHwTree)){  
  DimServerComputerList[7]="10.176.11.67_part4";  
  }
  else DimServerComputerList[7]="";
}
  
  DimServerComputerList[8]="";
  DimServerComputerList[9]="";      
  DimServerComputerList[10]="";      
  DimServerComputerList[11]="";      
      
  PCToManID=DimServerComputerList;
  /*
  PCToManID[1]="smi";  
  PCToManID[2]="ufcmshv1";
  PCToManID[3]="emudcs";
  PCToManID[4]="";  /plus/etc/passwd.users
  PCToManID[5]="";
  PCToManID[6]="";
  PCToManID[7]="";  
  PCToManID[8]="";
  PCToManID[9]="";  
  */
}

//=========================================================

db_set_cc_new(){

  int i,j, j1;
  for(i=1;i<=dynlen(CC_IN);i++){

    for(j=1;j<=dynlen(CC_IN[i]);j++){
     if(CC_IN[i][j] !=""){
      if(i!=4 && i!=5)CC_IN[i][j]=CSC_fwG_g_WHEELS[i]+"/1/"+j;
      else CC_IN[i][j]=CSC_fwG_g_WHEELS[i]+"/2/"+j;
      //DebugTN("IN  "+CC_IN[i][j]);
     }
    }//for 
  }
  for(i=1;i<=dynlen(CC_OUT);i++){  
    for(j=1;j<=dynlen(CC_OUT[i]);j++){
     if(CC_OUT[i][j] !=""){
      if(i!=4 && i!=5)CC_OUT[i][j]=CSC_fwG_g_WHEELS[i]+"/2/"+j;
      else CC_OUT[i][j]=CSC_fwG_g_WHEELS[i]+"/3/"+j;
      //DebugTN("OUT "+CC_OUT[i][j]);
     }
    }//for     


  }//for    
//--------
    for(j=1;j<=dynlen(CC_DUBNA);j++){
      j1=j-36;
     if(CC_DUBNA[j] !=""){
      if(j<=36)CC_DUBNA[j]=CSC_fwG_g_WHEELS[4]+"/1/"+j;
      else CC_DUBNA[j]=CSC_fwG_g_WHEELS[5]+"/1/"+j1;
      //DebugTN("DUBNA "+CC_DUBNA[j]);
     }
    }//for     

}

//==========================================================



