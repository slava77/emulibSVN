/*
dip/CMSX/DSS/AI_Flowmeter_EndCAp_Cooling_YE_Moims_1/DipData
dip/CMSX/DSS/AI_Flowmeter_EndCap_Cooling_YE_Plus_1/DipData

dip/CMSX/DSS/AI_Flowmeter_Rack_EndCAp_Cooling_Far_Side_YE_Moins_1/DipData
dip/CMSX/DSS/AI_Flowmeter_Rack_EndCAp_Cooling_Near_Side_YE_Moins_1/DipData
dip/CMSX/DSS/AI_Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Plus_1/DipData
dip/CMSX/DSS/AI_Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Plus_1/DipData
*/
//==========================================================================

mudcsGasInit(){
 
 string node_to_define_system_name="CSC_GAS";
 fwCU_getDp(node_to_define_system_name,node_to_define_system_name); 
 
 dyn_string split_ds; 
 addGlobal("CSC_fwGAS_g_SYSTEM_NAME",STRING_VAR); 
 dyn_string systemNameDps=dpNames("*:CMS_CSC_GAS");
// dyn_string systemNameDps=dpNames("*:"+node_to_define_system_name);
 if(dynlen(systemNameDps)<=0)CSC_fwGAS_g_SYSTEM_NAME="dcspcS2G19_06";
 else{
   split_ds=strsplit(systemNameDps[1],":");
   CSC_fwGAS_g_SYSTEM_NAME=split_ds[1];
 }



//=========
string os=getenv("OS");
addGlobal("CSC_fwGGas_g_OS",STRING_VAR);
CSC_fwGGas_g_OS=os;  
  
addGlobal("CSC_fwGGas_g_HOME",STRING_VAR);


if (os =="Linux")CSC_fwGGas_g_HOME = getenv("HOME");///"/nfshome0/cscdcsdev"; 
else CSC_fwGGas_g_HOME = "c:\\pvss_project_36";  
  
addGlobal("CSC_fwGGas_g_ProjectHome",STRING_VAR);  
//if (os =="Linux")CSC_fwGGas_g_ProjectHome = CSC_fwGGas_g_HOME+"/csc_station_p2";  
//else CSC_fwGGas_g_ProjectHome = CSC_fwGGas_g_HOME+"\\csc_super";
string project_path=getPath("");  // get absolute home path of the project 
project_path=substr(project_path,0,strlen(project_path)-1); // to get rid of the last "/"

if (os =="Linux"){

  CSC_fwGGas_g_ProjectHome = project_path;
  }
else {
  strreplace(project_path,"/","\\");  
  CSC_fwGGas_g_ProjectHome = project_path;

}


addGlobal("CSC_fwGGas_g_Component",STRING_VAR);
CSC_fwGGas_g_Component = "CMS_CSCfw_GAS"; // fsm: "mudcs" 

addGlobal("CSC_fwGGas_g_project_name_home",STRING_VAR);
CSC_fwGGas_g_project_name_home=CSC_fwGGas_g_ProjectHome; 
  
  
}
//==========================================================================

string mudcsGasAddSystem(string dp){
  
  if(strpos(dp,CSC_fwGAS_g_SYSTEM_NAME+":")>=0)return dp;
  dp=CSC_fwGAS_g_SYSTEM_NAME+":"+dp;
  return dp;
}
//==========================================================================




dyn_int GasSystem_State;
dyn_int GasSystem_Status;
dyn_string GasSystem_State_meaning;  // int
dyn_string GasSystem_Status_meaning;
//============================

dyn_int GasMixer_State;
dyn_bool GasMixer_Interlock;
dyn_string GasMixer_State_meaning;
dyn_string GasMixer_Interlock_meaning;

string GasMixer_LiquidLevel_units;
string GasMixer_EvaporatorTemperature_units;
string GasMixer_OutPressure_units;
string GasMixer_TotalFlow_units;
string GasMixer_LineRatio_units;
string GasMixer_LineInputPressure_units;
string GasMixer_LineLFmfcFeedback_units;
string GasMixer_LineHFmfcFeedback_units;
string GasMixer_LineWaterTankLevel_units;


//============================

dyn_int GasDistribution_State;
dyn_bool GasDistribution_BUSource;
dyn_string GasDistribution_State_meaning;
dyn_string GasDistribution_BUSource_meaning;

string GasDistribution_TotalInputFlow_units;

//============================



dyn_int GasDistributionRack_StateRack;
dyn_string GasDistributionRack_StateRack_meaning;
string GasDistributionRack_InPressure22_units;
string GasDistributionRack_ChPressure_units;
string GasDistributionRack_RegPressure_units;
string GasDistributionRack_InPressure24_units;
string GasDistributionRack_OutManifoldPressure_units;

//============================





string GasDistributionChannel_Pressure_units;
string GasDistributionChannel_InFlow_units;
string GasDistributionChannel_OutFlow_units;
string GasDistributionChannel_Isolation_Valve_units;




//============================




dyn_int GasPreDistribution_State;
dyn_string GasPreDistribution_State_meaning;

string GasPreDistribution_InPressure;
string GasPreDistribution_OutPressure;





//============================




dyn_int GasPump_State;
dyn_string GasPump_State_meaning;

string GasPump_InPressure_units;
string GasPump_PressureSetpoint_units;
string GasPump_OutPressure_units;
string GasPump_PumpInput_Flow_units;
string GasPump_PumpCurrent_Speed_units;
string GasPump_PumpCurrent_Load_units;
string GasPump_PumpCurrent_Speed2_units;
string GasPump_PumpCurrent_Load2_units;
string GasPump_PumpUpperTemperature_units;
string GasPump_PumpLowerTemperature_units;
string GasPump_PumpOutTemperature_units;
string GasPump_PumpEngineTemperature_units;



//============================

dyn_int GasExhaust_State;
dyn_string GasExhaust_State_meaning;

string GasExhaust_OutFlow_units;
string GasExhaust_BufferPressure_units;
string GasExhaust_CirculationPressure_units;
string GasExhaust_CirculationFlow_units;




//============================



dyn_int GasPurifier_State;
dyn_int GasPurifier_ColState;
dyn_string GasPurifier_State_meaning;
dyn_string GasPurifier_ColState_meaning;

string GasPurifier_ColRunVolume_units;
string GasPurifier_ColInternal_TopTemp_units;
string GasPurifier_ColInternalMiddle_Temp_units;
string GasPurifier_ColExternalTemp_units;
string GasPurifier_ColInFlow_units;
string GasPurifier_ColOutPressure_units;




mudcsGasSystemMeanings(){


//=============================
//============================

/*dyn_int*/ GasSystem_State=makeDynInt(100,26,27,28,29,30,31,32,33,34
                                     ,16,17,18,19,20,21,22,23,24,25  
                                     ,1,2,3,4,5,6,7,8,9,10 
                                     ,11,12,13,14,15);  // int
/*dyn_int*/ GasSystem_Status=makeDynBool(FALSE,TRUE);  // bool
/*dyn_string*/ GasSystem_State_meaning=makeDynString("Stop","Fill Start-Up/Fill1","Fill Start-Up/Fill2","Fill Start-Up/Fill3",
                                     "Fill Start-Up/Fill4","Fill Start-Up/Fill5","Fill Start-Up/Fill6","Fill Start-Up/Fill7",
                                     "Filling/Filling","Filling/FillCompletion",
                                     "Run Start-Up/Run1","Run Start-Up/Run2","Run Start-Up/Run3","Run Start-Up/Run4","Run Start-Up/Run5",
                                     "Run Start-Up/Run6","Run Start-Up/Run61","Run Start-Up/Run62","Run Start-Up/Run7",  
                                     "RunReady/Run",
                                     "Purge Start-Up/Purge1","Purge Start-Up/Purge2","Purge Start-Up/Purge3",
                                     "Purge Start-Up/Purge4","Purge Start-Up/Purge5","Purging/Purging",
                                     "Recuperation Start-Up/Recupe1","Recuperation Start-Up/Recupe2","Recuperation Start-Up/Recupe3", 
                                     "Recuperation Start-Up/Recupe4","Recuperation Start-Up/Recupe5","Recuperation Start-Up/Recupe51",
                                     "Recuperation Start-Up/Recupe52","Recuperation Start-Up/Recupe6","Recuperating/Recuperate");   // int
/*dyn_string*/ GasSystem_Status_meaning=makeDynString("NOT READY","NOMINAL");  // bool
//============================

/*dyn_int*/ GasMixer_State=makeDynInt(1,4,8,12,2,6,10,3,7,
                                     11,5,9,13);          // int 
/*dyn_bool*/ GasMixer_Interlock=makeDynBool(FALSE,TRUE);     // bool
/*dyn_string*/ GasMixer_State_meaning=makeDynString("Stop","Run Entry","Run Unstable","Run Stable",
                                     "Fill Entry","Fill Unstable","Fill Stable",
                                     "Purge Entry","Purge Unstable","Purge Stable",
                                     "Direct Entry","Direct Unstable","Direct Stable");
/*dyn_string*/ GasMixer_Interlock_meaning=makeDynString("Problem","OK");

/*string*/ GasMixer_LiquidLevel_units=" %";
/*string*/ GasMixer_EvaporatorTemperature_units=" C";
/*string*/ GasMixer_OutPressure_units=" Bar";
/*string*/ GasMixer_TotalFlow_units=" l/h";
/*string*/ GasMixer_LineRatio_units=" %";
/*string*/ GasMixer_LineInputPressure_units=" Bar";
/*string*/ GasMixer_LineLFmfcFeedback_units=" l/h";
/*string*/ GasMixer_LineHFmfcFeedback_units=" l/h";
/*string*/ GasMixer_LineWaterTankLevel_units=" %";


//============================

/*dyn_int*/ GasDistribution_State=makeDynInt(1,25,30,35,15,20);     // int 
/*dyn_bool*/ GasDistribution_BUSource=makeDynBool(FALSE,TRUE);   // bool
/*dyn_string*/ GasDistribution_State_meaning=makeDynString("Stop","Starting Run","Run Not Ready",
                                                            "Run Ready","Start Com Purge","Com Purge Ready");     // int 
/*dyn_string*/ GasDistribution_BUSource_meaning=makeDynString("Line","Bottle");   // bool

/*string*/ GasDistribution_TotalInputFlow_units=" l/h";

//============================



/*dyn_int*/ GasDistributionRack_StateRack=makeDynInt(1,25,27,30,35,5,7,10,15,
                                     17,20); // int
/*dyn_string*/ GasDistributionRack_StateRack_meaning=makeDynString("Stop","Starting Run","Starting Run2","Run Not Ready",
                                     "Run Ready","Starting Individual Purge","Starting Individual Purge2","Individual Purge Ready",
                                     "Start Common Purge","Start Common Purge2","Common Purge Ready"); // int

/*string*/ GasDistributionRack_InPressure22_units=" mBar";
/*string*/ GasDistributionRack_ChPressure_units=" mBar";
/*string*/ GasDistributionRack_RegPressure_units=" mBar";
/*string*/ GasDistributionRack_InPressure24_units=" mBar";
/*string*/ GasDistributionRack_OutManifoldPressure_units=" mBar";

//============================





/*string*/ GasDistributionChannel_Pressure_units=" mBar";
/*string*/ GasDistributionChannel_InFlow_units=" l/h";
/*string*/ GasDistributionChannel_OutFlow_units=" l/h";
/*string*/ GasDistributionChannel_Isolation_Valve_units="";




//============================




/*dyn_int*/ GasPreDistribution_State=makeDynInt(1,2,3,4,5);  // int
/*dyn_string*/ GasPreDistribution_State_meaning=makeDynString("Stop","Forward Nominal","Backward Nominal","Forward Exhaust","Backward Exhaust");  // int

/*string*/ GasPreDistribution_InPressure=" mBar";
/*string*/ GasPreDistribution_OutPressure=" mBar";





//============================




/*dyn_int*/ GasPump_State=makeDynInt(1,2,3);  // int
/*dyn_string*/ GasPump_State_meaning=makeDynString("Stop","Starting","Run");  // int

/*string*/ GasPump_InPressure_units=" mBar";
/*string*/ GasPump_PressureSetpoint_units=" mBar";
/*string*/ GasPump_OutPressure_units=" Bar";
/*string*/ GasPump_PumpInput_Flow_units=" m3/h";
/*string*/ GasPump_PumpCurrent_Speed_units=" Hz";
/*string*/ GasPump_PumpCurrent_Load_units=" A";
/*string*/ GasPump_PumpCurrent_Speed2_units=" Hz";
/*string*/ GasPump_PumpCurrent_Load2_units=" A";
/*string*/ GasPump_PumpUpperTemperature_units=" C";
/*string*/ GasPump_PumpLowerTemperature_units=" C";
/*string*/ GasPump_PumpOutTemperature_units=" C";
/*string*/ GasPump_PumpEngineTemperature_units=" C";



//============================

/*dyn_int*/ GasExhaust_State=makeDynInt(1,2,3);   // int
/*dyn_string*/ GasExhaust_State_meaning=makeDynString("Stop","Recirculating","Venting");   // int

/*string*/ GasExhaust_OutFlow_units=" l/h";
/*string*/ GasExhaust_BufferPressure_units=" Bar";
/*string*/ GasExhaust_CirculationPressure_units=" Bar";
/*string*/ GasExhaust_CirculationFlow_units=" l/h";




//============================



/*dyn_int*/ GasPurifier_State=makeDynInt(100,2,40,20,30,50);   // int
/*dyn_int*/ GasPurifier_ColState=makeDynInt(100,1,6,7,2,8,3,9,10,
                                     4,11,5); // int
/*dyn_string*/ GasPurifier_State_meaning=makeDynString("Stop","Nominal Run","Double Column",
                                     "Single Column A","Single Column B","Free Run");   // int
/*dyn_string*/ GasPurifier_ColState_meaning=makeDynString("Stop","Stand By","Prepare for Run Ph1","Prepare for Run Ph2",
                                     "Run/Nominal Run","Run/Pre-Saturated","Purge Process Gas/Regen.Purge","Heat Up",
                                     "Regeneration",
                                     "Cool Down","Prepare for Stand-By","Column Purge"); // int

/*string*/ GasPurifier_ColRunVolume_units=" m3";
/*string*/ GasPurifier_ColInternal_TopTemp_units=" C";
/*string*/ GasPurifier_ColInternalMiddle_Temp_units=" C";
/*string*/ GasPurifier_ColExternalTemp_units=" C";
/*string*/ GasPurifier_ColInFlow_units=" l/h";
/*string*/ GasPurifier_ColOutPressure_units=" Bar";







  
}
