string configDp;//="DIPConfig_CMS_CSCfw_GAS"; <- this is being set in mudcsDipConfig()

mudcsTurbineConfig(dyn_string crates,bool issubscribe){

string colling_crate_dp;
string area;

int i,j;
dyn_string dpes; 
dyn_string tags; 

//DebugTN(crates);

for(i=1;i<=dynlen(crates);i++){
//  colling_crate_dp="CoolingTurbinesRack"+crates[i]+"_o";

  area=substr(crates[i],0,2);  
  
string item="dip/CMS/RACK/"+area+"/"+crates[i]+"/Sensors";    

//=======================
dyn_string childName;
dyn_int childType;
dyn_string fieldName;
dyn_int fieldType;
dyn_string exceptionInfo;

  int ret=fwDIP_DipQuery(configDp,item,childName, childType,fieldName, fieldType,exceptionInfo);  
//========================

 // fieldName=makeDynString("Temperature1","Temerature2","TurbineCurrent","TurbineCurrent2");
  
dynClear(dpes);
dynClear(tags);

for(j=1;j<=dynlen(fieldName);j++){
  
  DebugTN("CoolingTurbinesRack/"+crates[i]+"/"+fieldName[j]);
  
colling_crate_dp="CoolingTurbinesRack/"+crates[i]+"/"+fieldName[j];
strreplace(colling_crate_dp,"Temerature","Temperature");


if(strpos(colling_crate_dp,"Temperature")<0 && strpos(colling_crate_dp,"Current")<0)colling_crate_dp="x"+colling_crate_dp;
dpCreate(colling_crate_dp,"fwCooling_CSC_TURBINES_data");

dynAppend(dpes,colling_crate_dp+".");  //float
dynAppend(tags,fieldName[j]); 

} //for j

mudcs_dip_config(issubscribe,item, dpes, tags);

} // for i

}
//===================================================================

mudcsDipConfig(bool issubscribe)
{
 
  configDp="DIPConfig_CMS_CSCfw_GAS";  
  
/*
  
  item="test.pub2"; // name of DIP publication (complex)
  string dpe1="dip_test_structure.field1"; // name of dp element to be subscibed to this publication tag
  string dpe2="dip_test_structure.field2";  // name of dp element to be subscibed to this publication tag
  string tag1="field1"; // tag (of int type) in the  DIP publication "test.item2"
  string tag2="field2";  // tag (of int type) in the  DIP publication "test.item2"
  configDp="DIPConfig_CMS_CSCfw_GAS"; // DIP config to be saved for this configuration (see type _FwDipConfig)
  overwrite=FALSE;
  dyn_string dpes= makeDynString(dpe1,dpe2);
  dyn_string tags=makeDynString(tag1,tag2);
  
  fwDIP_subscribeStructure(item, dpes, 
            tags, configDp, exceptionInfo, true);// this inserts the ->
  // -> dcspcS2G19_06:dip_test_structure.field1
  //->      dcspcS2G19_06:dip_test_structure.field2 into the dyn_string "DIPConfig_CMS_CSCfw_GAS.clientConfig"

*/
  
dpSetWait(configDp + ".clientConfig",makeDynString());  
  
  int i,j,xx,yy,jj;
dyn_int RackChannels;  
dyn_string item;
dyn_string dpes; 
dyn_string tags; 



//===================================================================
mudcsTurbinesInit();
    
mudcsTurbineConfig(turbine_crates,issubscribe);
mudcsTurbineConfig(hv_racks,issubscribe);
mudcsTurbineConfig(pc_racks,issubscribe);
//mudcsTurbineConfig(align_racks,issubscribe);

//===================================================================

/*
dip/CMSX/DSS/AI_Flowmeter_EndCap_Cooling_YE_Minus_1/DipData
dip/CMSX/DSS/AI_Flowmeter_EndCap_Cooling_YE_Plus_1/DipData

dip/CMSX/DSS/AI_Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Minus_1/DipData
dip/CMSX/DSS/AI_Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Minus_1/DipData
dip/CMSX/DSS/AI_Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Plus_1/DipData
dip/CMSX/DSS/AI_Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Plus_1/DipData
*/  
//===================================================================
dpCreate("CSC_WATER_COOLING","fwCooling_CSC_COOLING_d");
item="dip/CMSX/DSS/AI_Flowmeter_EndCap_Cooling_YE_Minus";
dynClear(dpes);
dynClear(tags);

dynAppend(dpes,"CSC_WATER_COOLING.Flowmeter_EndCap_Cooling_YE_Minus_1");  //float

dynAppend(tags,"__DIP_DEFAULT__");
mudcs_dip_config(issubscribe,item, dpes, tags);
//==================================
item="dip/CMSX/DSS/AI_Flowmeter_EndCap_Cooling_YE_Plus";
dynClear(dpes);
dynClear(tags);

dynAppend(dpes,"CSC_WATER_COOLING.Flowmeter_EndCap_Cooling_YE_Plus_1");  //float

dynAppend(tags,"__DIP_DEFAULT__");
mudcs_dip_config(issubscribe,item, dpes, tags);
//==================================
item="dip/CMSX/DSS/AI_Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Minus_1";
dynClear(dpes);
dynClear(tags);

dynAppend(dpes,"CSC_WATER_COOLING.Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Minus_1");  //float

dynAppend(tags,"__DIP_DEFAULT__");
mudcs_dip_config(issubscribe,item, dpes, tags);
//==================================
item="dip/CMSX/DSS/AI_Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Minus_1";
dynClear(dpes);
dynClear(tags);

dynAppend(dpes,"CSC_WATER_COOLING.Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Minus_1");  //float

dynAppend(tags,"__DIP_DEFAULT__");
mudcs_dip_config(issubscribe,item, dpes, tags);
//==================================
item="dip/CMSX/DSS/AI_Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Plus_1";
dynClear(dpes);
dynClear(tags);

dynAppend(dpes,"CSC_WATER_COOLING.Flowmeter_Rack_EndCap_Cooling_Far_Side_YE_Plus_1");  //float

dynAppend(tags,"__DIP_DEFAULT__");
mudcs_dip_config(issubscribe,item, dpes, tags);
//==================================
item="dip/CMSX/DSS/AI_Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Plus_1";
dynClear(dpes);
dynClear(tags);

dynAppend(dpes,"CSC_WATER_COOLING.Flowmeter_Rack_EndCap_Cooling_Near_Side_YE_Plus_1");  //float

dynAppend(tags,"__DIP_DEFAULT__");
mudcs_dip_config(issubscribe,item, dpes, tags);
//==================================

//===================================================================
dpCreate("CSC_GAS_MONITOR","fwGasSystem_CSC_GAS_d");
item="dip/CMS/GCS/CMSCSC/GasSystem";
dynClear(dpes);
dynClear(tags);

dynAppend(dpes,"CSC_GAS_MONITOR.State");  //int
dynAppend(dpes,"CSC_GAS_MONITOR.Status");  //bool|c:1

dynAppend(tags,"State(GsStepWS)");
dynAppend(tags,"Status(GSY0505)");
mudcs_dip_config(issubscribe,item, dpes, tags);


//===================================================================
dpCreate("GasMixer_o","fwGasMixer_CSC_GAS_data");
item="dip/CMS/GCS/CMSCSC/Mixer";
dynClear(dpes);
dynClear(tags);
dynAppend(dpes,"GasMixer_o.State");//int
//dynAppend(dpes,"GasMixer_o.Interlock");//bool
//dynAppend(dpes,"GasMixer_o.LiquidLevel");//float
//dynAppend(dpes,"GasMixer_o.EvaporatorTemperature");//float
dynAppend(dpes,"GasMixer_o.OutPressure");//float
dynAppend(dpes,"GasMixer_o.TotalFlow");//float
/*
for(i=1;i<=3;i++){
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
}
*/
for(i=1;i<=3;i++){
  dynAppend(dpes,"GasMixer_o.Line"+i+"Ratio");//float
  dynAppend(dpes,"GasMixer_o.Line"+i+"InputPressure");//float
//  dynAppend(dpes,"GasMixer_o.Line"+i+"LFmfcFeedback");//float
//  dynAppend(dpes,"GasMixer_o.Line"+i+"HFmfcFeedback");//float
//  dynAppend(dpes,"GasMixer_o.Line"+i+"WaterTankLevel");//float
}
dynAppend(tags,"State(StepperWS)"); 
//dynAppend(tags,"Interlock(ZSH9119)"); 
//dynAppend(tags,"LiquidLevel(LSH1015)"); 
//dynAppend(tags,"EvaporatorTemperature");  
dynAppend(tags,"OutPressure(PT1009)"); 
dynAppend(tags,"TotalFlow(TotalFlowAS)"); 

for(i=1;i<=3;i++){
  dynAppend(tags,"Line"+i+"Ratio(L"+i+"CompRatioAS)");
  dynAppend(tags,"Line"+i+"InputPressure(PT1"+i+"03)");
//  dynAppend(tags,"Line"+i+"LFmfcFeedback(XMFC1"+i+"06F)");
//  dynAppend(tags,"Line"+i+"HFmfcFeedback(XMFC1"+i+"09F)");
//  dynAppend(tags,"Line"+i+"WaterTankLevel(L"+i+"WaterLevAS)");
}
mudcs_dip_config(issubscribe,item, dpes, tags);

//===================================================================
dpCreate("GasDistribution_o","fwGasDistribution_CSC_GAS_data");
item="dip/CMS/GCS/CMSCSC/Distribution";
dynClear(dpes);
dynClear(tags);

dynAppend(dpes,"GasDistribution_o.State"); // int
//dynAppend(dpes,"GasDistribution_o.BUSource"); //bool
//dynAppend(dpes,"GasDistribution_o.TotalInputFlow");//float

dynAppend(tags,"State(ModStepWS)");
//dynAppend(tags,"BUSource(PSL5508)");
//dynAppend(tags,"TotalInputFlow(Tot-FlowAS)");
mudcs_dip_config(issubscribe,item, dpes, tags);
//===================================================================

for(i=1;i<=10;i++){
  xx=60+i;
dpCreate("GasDistributionRack"+xx+"_o","fwGasDistributionRack_CSC_GAS_data");
item="dip/CMS/GCS/CMSCSC/Distribution/Rack"+xx+"";

dynClear(dpes);
dynClear(tags);

dynAppend(dpes,"GasDistributionRack"+xx+"_o.InPressure22");  //float
//dynAppend(dpes,"GasDistributionRack"+xx+"_o.ChPressure");//float
dynAppend(dpes,"GasDistributionRack"+xx+"_o.RegPressure");//float
dynAppend(dpes,"GasDistributionRack"+xx+"_o.StateRack"); //int
dynAppend(dpes,"GasDistributionRack"+xx+"_o.InPressure24");//float
//dynAppend(dpes,"GasDistributionRack"+xx+"_o.OutManifoldPressure");//float

dynAppend(tags,"InPressure(PT"+xx+"22)");
//dynAppend(tags,"ChPressure(PT"+xx+"04)");
dynAppend(tags,"RegPressure(PT"+xx+"26)");
dynAppend(tags,"State(Rack"+xx+"StepWS)");
dynAppend(tags,"InPressure(PT"+xx+"24)");
//dynAppend(tags,"OutManifoldPressure(PDT"+xx+"25)");
mudcs_dip_config(issubscribe,item, dpes, tags);
}


//===================================================================
for(i=1;i<=10;i++){
  xx=60+i;
  if(xx==61 || xx== 66){
   dynClear(RackChannels);
   for(jj=1;jj<=9;jj++)dynAppend(RackChannels,jj); 
  }
  if(xx== 62|| xx==63 || xx==64 || xx==67 || xx==68 || xx==69 ){
   dynClear(RackChannels);
   for(jj=1;jj<=18;jj++)dynAppend(RackChannels,jj); 
  }
  if(xx== 65|| xx==70){  
   dynClear(RackChannels);
   RackChannels=makeDynInt(1,4,7,10,13,16); 
  } 
  for(j=1;j<=dynlen(RackChannels);j++){
  yy=RackChannels[j]; 
dpCreate("GasDistributionRack"+xx+"Channel"+yy+"_o","fwGasDistributionChannel_CSC_GAS_data");
item="dip/CMS/GCS/CMSCSC/Distribution/Rack"+xx+"/Channel"+yy+"";
dynClear(dpes);
dynClear(tags);

//dynAppend(dpes,"GasDistributionRack"+xx+"Channel"+yy+"_o.Pressure");
dynAppend(dpes,"GasDistributionRack"+xx+"Channel"+yy+"_o.InFlow");
dynAppend(dpes,"GasDistributionRack"+xx+"Channel"+yy+"_o.OutFlow");
//dynAppend(dpes,"GasDistributionRack"+xx+"Channel"+yy+"_o.Isolation-Valve");

//dynAppend(tags,"Pressure(PT"+xx+"04Ch"+yy+")"); //float
dynAppend(tags,"InFlow(FE"+xx+"02Ch"+yy+")"); //float
dynAppend(tags,"OutFlow(FE"+xx+"06Ch"+yy+")");//float
//dynAppend(tags,"Isolation-Valve(YY"+xx+"03Ch"+yy+")");//bool
mudcs_dip_config(issubscribe,item, dpes, tags);
} //j
} //i

//===================================================================
dpCreate("GasPreDistribution_o","fwGasPreDistribution_CSC_GAS_data");
item="dip/CMS/GCS/CMSCSC/PreDistribution";
dynClear(dpes);
dynClear(tags);

dynAppend(dpes,"GasPreDistribution_o.State");  //int
dynAppend(dpes,"GasPreDistribution_o.InPressure"); //float
dynAppend(dpes,"GasPreDistribution_o.OutPressure"); //float

dynAppend(tags,"State(StepperWS)");
dynAppend(tags,"InPressure(PT6119)");
dynAppend(tags,"OutPressure(PT6139)");
mudcs_dip_config(issubscribe,item, dpes, tags);
//===================================================================
dpCreate("GasPump_o","fwGasPump_CSC_GAS_data");
item="dip/CMS/GCS/CMSCSC/Pump";
dynClear(dpes);
dynClear(tags);

dynAppend(dpes,"GasPump_o.State");  //int
dynAppend(dpes,"GasPump_o.InPressure"); //float
dynAppend(dpes,"GasPump_o.PressureSetpoint");//float
dynAppend(dpes,"GasPump_o.OutPressure");//float

for(i=1;i<=2;i++){

//  dynAppend(dpes,"GasPump_o.Pump"+i+"Input-Flow");//float
//  dynAppend(dpes,"GasPump_o.Pump"+i+"Current-Speed");//float
//  dynAppend(dpes,"GasPump_o.Pump"+i+"Current-Load");//float
//  dynAppend(dpes,"GasPump_o.Pump"+i+"Current-Speed2");//float
//  dynAppend(dpes,"GasPump_o.Pump"+i+"Current-Load2");//float
  dynAppend(dpes,"GasPump_o.Pump"+i+"UpperTemperature");//float
  dynAppend(dpes,"GasPump_o.Pump"+i+"LowerTemperature");//float
//  dynAppend(dpes,"GasPump_o.Pump"+i+"OutTemperature");//float
//  dynAppend(dpes,"GasPump_o.Pump"+i+"EngineTemperature");    //float
}
dynAppend(tags,"State(StepperWS)");
dynAppend(tags,"InPressure(PT4004)");
dynAppend(tags,"PressureSetpoint(RnDfltPSp)");
dynAppend(tags,"OutPressure(PT4006)");

for(i=1;i<=2;i++){
//  dynAppend(tags,"Pump"+i+"Input-Flow(FT4"+i+"01)");
//  dynAppend(tags,"Pump"+i+"Current-Speed(ST4"+i+"01)");
//  dynAppend(tags,"Pump"+i+"Current-Load(IT4"+i+"01)");
//  dynAppend(tags,"Pump"+i+"Current-Speed(PUMP4"+i+"01FrgF)");
//  dynAppend(tags,"Pump"+i+"Current-Load(PUMP4"+i+"01CurF)");
  dynAppend(tags,"Pump"+i+"UpperTemperature(TE4"+i+"01_1)");
  dynAppend(tags,"Pump"+i+"LowerTemperature(TE4"+i+"01_2)");
//  dynAppend(tags,"Pump"+i+"OutTemperature(TE4"+i+"01_3)");
//  dynAppend(tags,"Pump"+i+"EngineTemperature(TE4"+i+"01_4)");  
}
mudcs_dip_config(issubscribe,item, dpes, tags);
//===================================================================
dpCreate("GasExhaust_o","fwGasExhaust_CSC_GAS_data");
item="dip/CMS/GCS/CMSCSC/Exhaust";
dynClear(dpes);
dynClear(tags);

dynAppend(dpes,"GasExhaust_o.State");  //int
//dynAppend(dpes,"GasExhaust_o.OutFlow");  //float
dynAppend(dpes,"GasExhaust_o.BufferPressure");//float
dynAppend(dpes,"GasExhaust_o.CirculationPressure");//float
dynAppend(dpes,"GasExhaust_o.CirculationFlow");//float

dynAppend(tags,"State(StepperWS)");
//dynAppend(tags,"OutFlow(XMFC5002FIF)");
dynAppend(tags,"BufferPressure(PT5005)");
dynAppend(tags,"CirculationPressure(PT5011)");
dynAppend(tags,"CirculationFlow(FIT5010)");

mudcs_dip_config(issubscribe,item, dpes, tags);
//===================================================================

dpCreate("GasPurifier_o","fwGasPurifier_CSC_GAS_data");
string AorB;
string s1or2;
item="dip/CMS/GCS/CMSCSC/Purifiers/Purifier1";
dynClear(dpes);
dynClear(tags);

dynAppend(dpes,"GasPurifier_o.State"); //int

for(i=1;i<=2;i++){
  if(i==1)AorB="A"; 
  else AorB="B"; 
  dynAppend(dpes,"GasPurifier_o.Col"+AorB+"State"); //int
  dynAppend(dpes,"GasPurifier_o.Col"+AorB+"RunVolume");//float
  dynAppend(dpes,"GasPurifier_o.Col"+AorB+"Internal-TopTemp");//float
  dynAppend(dpes,"GasPurifier_o.Col"+AorB+"InternalMiddle-Temp");//float
  dynAppend(dpes,"GasPurifier_o.Col"+AorB+"ExternalTemp");//float
  dynAppend(dpes,"GasPurifier_o.Col"+AorB+"InFlow");//float
  dynAppend(dpes,"GasPurifier_o.Col"+AorB+"OutPressure");//float

}
dynAppend(tags,"State(PuStepWS)");


for(i=1;i<=2;i++){ 
  if(i==1){AorB="A";s1or2="1"; }
  else {AorB="B";s1or2="2";}
  dynAppend(tags,"Col"+AorB+"State(PuC"+AorB+"StepWS)");
  dynAppend(tags,"Col"+AorB+"RunVolume(C"+AorB+"RunVol)");
  dynAppend(tags,"Col"+AorB+"InternalTopTemp(TE2"+s1or2+"01_1)");
  dynAppend(tags,"Col"+AorB+"InternalMiddleTemp(TE2"+s1or2+"01_3)");
  dynAppend(tags,"Col"+AorB+"ExternalTemp(TE2"+s1or2+"01_2)");
  dynAppend(tags,"Col"+AorB+"InFlow(FIT2"+s1or2+"09)");
  dynAppend(tags,"Col"+AorB+"OutPressure(PT2"+s1or2+"08)");

}
 mudcs_dip_config(issubscribe,item, dpes, tags);
//===================================================================
/*
//===================================================================
dpCreate("","_CSC_GAS_data");
item="";
dynClear(dpes);
dynClear(tags);

dynAppend(dpes,"");
dynAppend(dpes,"");
dynAppend(dpes,"");
dynAppend(dpes,"");
dynAppend(dpes,"");
dynAppend(dpes,"");
dynAppend(dpes,"");
dynAppend(dpes,"");
for(i=1;i<=3;i++){
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
}
dynAppend(tags,"");
dynAppend(tags,"");
dynAppend(tags,"");
dynAppend(tags,"");
dynAppend(tags,"");
dynAppend(tags,"");
dynAppend(tags,"");
dynAppend(tags,"");
for(i=1;i<=3;i++){
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");
  dynAppend(dpes,"");  
}
//===================================================================
dpCreate("","_CSC_GAS_data");
item="";
dynClear(dpes);
dynClear(tags);
dynAppend(dpes,"");    
dynAppend(tags,"");
//===================================================================


*/

//mudcsDebug("DONE");

}  
//====================================================================================



//====================================================================================
mudcs_dip_config(bool subscribe, string item, dyn_string dpes, dyn_string tags){

//bool subscribe;  
//    getValue("issubscribe","state",0,subscribe);
//    subscribe=!subscribe;
    
dyn_string exceptionInfo;

DebugTN(subscribe);

if(subscribe){
  
 // DebugTN(item);
  // DebugTN(dpes);
// DebugTN(tags);
  // DebugTN("====");
  fwDIP_subscribeStructure(item, dpes, 
            tags, configDp, exceptionInfo, true);

  if(dynlen(exceptionInfo)>0)DebugTN(exceptionInfo);
  
  }// this inserts the ->
  // -> dcspcS2G19_06:dip_test_structure.field1
  //->      dcspcS2G19_06:dip_test_structure.field2 into the dyn_string "DIPConfig_CMS_CSCfw_GAS.clientConfig" 
else 
  fwDIP_unsubscribeMany(dpes,exceptionInfo);
  
  
  
  
}
