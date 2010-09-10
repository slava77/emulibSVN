

mudcsGasArchiveMain(bool isArchiving)
{
  
  mudcsInit();
  mudcsGasInit();
  int i,j, i10;
dyn_string exceptionInfo;
string dp;

 string device_type;
 dyn_string dp_names, device_types;
 dyn_string dp_elements;
 
dynClear(device_types);
/*
dynAppend(device_types,"fwCooling_CSC_COOLING_d");

dynAppend(device_types,"fwGasSystem_CSC_GAS_d");
dynAppend(device_types,"fwGasMixer_CSC_GAS_data");
dynAppend(device_types,"fwGasDistribution_CSC_GAS_data");
dynAppend(device_types,"fwGasDistributionRack_CSC_GAS_data");
dynAppend(device_types,"fwGasDistributionChannel_CSC_GAS_data");
dynAppend(device_types,"fwGasPreDistribution_CSC_GAS_data");
dynAppend(device_types,"fwGasPump_CSC_GAS_data");
dynAppend(device_types,"fwGasExhaust_CSC_GAS_data");
dynAppend(device_types,"fwGasPurifier_CSC_GAS_data");
*/

dynAppend(device_types,"fwCooling_CSC_TURBINES_data");

//------------------------------------------------------


 

dyn_string value_seq_dyn, delta_v;
float v;
int time_interval;


DebugTN(device_types);
for(i10=1;i10<=dynlen(device_types);i10++){
  device_type=device_types[i10];


  if(device_types[i10]=="fwCooling_CSC_TURBINES_data"){
    dp_names=dpNames("Cooling*",device_types[i10]);
   // DebugTN(dp_names);
  }
v=1;
time_interval=7200;
for(i=1;i<=dynlen(dp_names);i++){
 // DebugTN(dp_names[i]);
 /*dynAppend(value_seq_dyn,   "."+dp_names[i]);*/dynAppend(delta_v, v);
// DebugTN(value_seq_dyn[i]);
}




//------------------------------------------------------

  
for(i=1;i<=dynlen(dp_names);i++){

  if(!isArchiving){
     fwArchive_deleteMultiple(makeDynString(dp_names[i]), exceptionInfo);
   }
  else{
    dp=dp_names[i]+".";
     dpSetWait(dp+":_archive.._type",45, 
          dp+":_archive.._archive", 1,
          dp+":_archive.1._type", 3,                 //here you say you are using smoothing
          dp+":_archive.1._class", "_EVENT",  
          dp+":_archive.1._interv", 0,              
          dp+":_archive.1._interv_type", 0,
          dp+":_archive.1._std_type", 2,        //type of standard smoothing procedure (value/time/comb ...)
          dp+":_archive.1._std_tol", delta_v[i],       //here I select the value deadband 
          dp+":_archive.1._std_time", time_interval       //here the time deadband
    );
  
   } //else  
 
} // for(i=1;i<=dynlen(dp_names);i++)

//---------------

//---------------
	if(dynlen(exceptionInfo)>0)
		fwExceptionHandling_display(exceptionInfo);
        
      } // for i10
}

//======================================================
