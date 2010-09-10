mudcsGasFsmDelete(){
DebugTN("mudcsGasFsmDelete() started");

  int i;
  int cu_flag;
    
mudcsInit(); // especially needed for addLogical
mudcsGasInit();

mudcs_deleteHardwareDevices("fwGasSystem_CSC_GAS");
mudcs_deleteHardwareDevices("fwCooling_CSC_COOLING");
/*
EmuCmsGlobalNode="CSC_MONITOR";
CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1;
CSC_fwG_EmuCmsGlobalParent="FSM";
mudcs_removeNode();
*/

EmuCmsGlobalNode="CSC_GAS";
CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1;
CSC_fwG_EmuCmsGlobalParent=CSC_fwG_g_csc_part;
mudcs_removeNode();
//fwFsmTree_removeNode(CSC_fwG_EmuCmsGlobalParent,EmuCmsGlobalNode);

EmuCmsGlobalNode="CSC_COOLING";
CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1;
CSC_fwG_EmuCmsGlobalParent=CSC_fwG_g_csc_part;
mudcs_removeNode();
//fwFsmTree_removeNode(CSC_fwG_EmuCmsGlobalParent,EmuCmsGlobalNode);




dyn_string dpsToDelete=dpNames("*GAS*","_FwTreeNode");
for(i=1;i<=dynlen(dpsToDelete);i++){
  if(strpos(dpsToDelete[i],"TrendTree")>=0)continue;
  if(strpos(dpsToDelete[i],"FSM")>=0)continue;  
  DebugTN("Deleting _FwTreeNode DP: " + dpsToDelete[i]);
  dpDelete(dpsToDelete[i]);
}       

dpsToDelete=dpNames("*COOLING*","_FwTreeNode");
for(i=1;i<=dynlen(dpsToDelete);i++){
  if(strpos(dpsToDelete[i],"TrendTree")>=0)continue;
  if(strpos(dpsToDelete[i],"FSM")>=0)continue;  
  DebugTN("Deleting _FwTreeNode DP: " + dpsToDelete[i]);
  dpDelete(dpsToDelete[i]);
}       

dpsToDelete=dpNames("*GAS*","_FwFsmDevice");
for(i=1;i<=dynlen(dpsToDelete);i++){
  if(strpos(dpsToDelete[i],"TrendTree")>=0)continue;
  if(strpos(dpsToDelete[i],"FSM")>=0)continue;  
  DebugTN("Deleting _FwFsmDevice DP: " + dpsToDelete[i]);
  dpDelete(dpsToDelete[i]);
}       

dpsToDelete=dpNames("*COOLING*","_FwFsmDevice");
for(i=1;i<=dynlen(dpsToDelete);i++){
  if(strpos(dpsToDelete[i],"TrendTree")>=0)continue;
  if(strpos(dpsToDelete[i],"FSM")>=0)continue;  
  DebugTN("Deleting _FwFsmDevice DP: " + dpsToDelete[i]);
  dpDelete(dpsToDelete[i]);
}       

dpsToDelete=dpNames("*GAS*","_FwFsmObject");
for(i=1;i<=dynlen(dpsToDelete);i++){
  if(strpos(dpsToDelete[i],"TrendTree")>=0)continue;
  if(strpos(dpsToDelete[i],"FSM")>=0)continue;  
  DebugTN("Deleting _FwFsmObject DP: " + dpsToDelete[i]);
  dpDelete(dpsToDelete[i]);
}       

dpsToDelete=dpNames("*COOLING*","_FwFsmObject");
for(i=1;i<=dynlen(dpsToDelete);i++){
  if(strpos(dpsToDelete[i],"TrendTree")>=0)continue;
  if(strpos(dpsToDelete[i],"FSM")>=0)continue;  
  DebugTN("Deleting _FwFsmObject DP: " + dpsToDelete[i]);
  dpDelete(dpsToDelete[i]);
}       
   

DebugTN("mudcsGasFsmDelete() finished");


}


mudcsGasCreateFsmMain()
{



mudcsInit(); // especially needed for addLogical
mudcsGasInit();

string Component="CMS_CSCfw_GAS";
string dir_config="config";

string parent_domain;
string Node_save;
string dpN;
int cu_flag;

int i,j, pos;
dyn_string ds;

 
 //=============================================================================

 parent_domain=CSC_fwG_g_csc_part;

CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
EmuCmsGlobalNode="CSC_GAS";

CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1; // control unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addLogical(false, CSC_fwG_g_csc_part, 
 EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuGASConfig", Component+"/emuGASOperation");

 DebugTN(CSC_fwG_EmuCmsGlobalParent);
  DebugTN(EmuCmsGlobalNode);
  DebugTN(CSC_fwG_EmuCmsGlobalType);
   
mudcs_addNode();

//------- add devices -----------


//mudcsDebug(parent_node);

parent_domain="CSC_GAS";

 CSC_fwG_EmuCmsGlobalType="fwGasSystem_CSC_GAS";
 EmuCmsGlobalNode="CSC_GAS_MONITOR";
 //EmuCmsGlobalNode= "CSC"+"HV_PR_"+CSC_fwG_g_all_primary[i]; 
 Node_save=EmuCmsGlobalNode;
 CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
 EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+"CSC_GAS", 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevfwGasSystem_CSC_GASConfig", Component+"/emuDevfwGasSystem_CSC_GASOperation");

 mudcs_addNode();
//========================================================================== 
//==============================Cooling=====================================
//==========================================================================
 
Component="CMS_CSCfw_COOLING";
dir_config="config";
 
 //=============================================================================

 parent_domain=CSC_fwG_g_csc_part;

CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
EmuCmsGlobalNode="CSC_COOLING";

CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1; // control unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addLogical(false, CSC_fwG_g_csc_part, 
 EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuCOOLINGConfig", Component+"/emuCOOLINGOperation");
 
mudcs_addNode();

//------- add devices -----------


//mudcsDebug(parent_node);

parent_domain="CSC_COOLING";

 CSC_fwG_EmuCmsGlobalType="fwCooling_CSC_COOLING";
 EmuCmsGlobalNode="CSC_WATER_COOLING";
 //EmuCmsGlobalNode= "CSC"+"HV_PR_"+CSC_fwG_g_all_primary[i]; 
 Node_save=EmuCmsGlobalNode;
 CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
 EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+"CSC_COOLING", 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevfwCooling_CSC_COOLINGConfig", Component+"/emuDevfwCooling_CSC_COOLINGOperation");

 mudcs_addNode();
 
 
  //=============================================================================
/*
 parent_domain="CSC_COOLING";

CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
EmuCmsGlobalNode="CSC_TURBINE_COOLING";

CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addLogical(false, CSC_fwG_g_csc_part+"/"+"CSC_COOLING", 
 EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuCOOLINGConfig", Component+"/emuCOOLINGOperation");
 
mudcs_addNode();

//------- add devices -----------
dyn_string  maraton_racks;

dyn_string  maraton_racks_obsolete=makeDynString(
   
"X4J31",  "X4A31","X2A33","X2J31","X4J41",
"X4A41","X2A43","X2J41","X4A51","X2J51",
 
"X4V31","X4S31","X2S33","X2V31","X4V41",
"X4S41","X2S43","X2V41","X4S51","X2V51");

dyn_string  maraton_rack_new=makeDynString(
   
"X4J31",  "X4A31","X2A33","X2J31","X4J41",
"X4A41","X2A41","X2J42","X4A51","X2J52",

"X4V31","X4S31","X2S33","X2V31","X4V41",
"X4S41","X2S41","X2V42","X4S51","X2V52");
 
maraton_racks=maraton_rack_new;

for(i=1;i<=dynlen(maraton_racks);i++){
 ds=dpNames("cms_rc_dcs_03:*"+maraton_racks[i]+"*","FwElmbAi");
 //DebugTN(ds1); 
 for(j=1;j<=dynlen(ds);j++){
   
 parent_domain="CSC_TURBINE_COOLING";

 CSC_fwG_EmuCmsGlobalType="fwElmbAi";
 pos=strpos(ds[j],maraton_racks[i]); 
 EmuCmsGlobalNode=substr(ds[j],pos);
 EmuCmsGlobalNode=strreplace(EmuCmsGlobalNode,"/","_");
 //EmuCmsGlobalNode= "CSC"+"HV_PR_"+CSC_fwG_g_all_primary[i]; 
 Node_save=EmuCmsGlobalNode;
 CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwG_EmuCmsGlobalParent=parent_domain;


 EmuCmsGlobalNode=ds[j];

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+"CSC_COOLING"+"/"+"CSC_TURBINE_COOLING", 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevfwElmbAiConfig", Component+"/emuDevfwElmbAiOperation");

 mudcs_addNode();
    
   
   
 }
    
 
}
 
*/
 
//========================================================================== 
 
fwFsmTree_generateAll();


}

//============================================================================
//============================================================================
//============================================================================

//=======================================================================

mudcs_addHardwareDevice_gas(string deviceName,string deviceType,string deviceComment, string &dpN)
{
/////	string 	deviceName = "CSCdm4r2c01LV_1",
/////			deviceType = "Low Voltage Device (1)",
//////	string		deviceComment, 
string			dpType;//////////, dpName;
        string generalDeviceType;

	dyn_string exceptionInfo;

mudcsPutCscDeviceList(deviceType);

//"fwWnrCh_CSC_LV"
if(strpos(deviceType,"fwGasSystem_CSC_GAS")>=0){
  
//    dpCreate(deviceName,"fwGasSystem_CSC_GAS_d");       // !!!!!!!!!!!!!!!!!!!!!
 
  generalDeviceType = "GasCMS";
  deviceType = "GasCMS Device (1)";
}
else if(strpos(deviceType,"fwCooling_CSC_COOLING")>=0){
  
//    dpCreate(deviceName,"fwCooling_CSC_COOLING_d");       // !!!!!!!!!!!!!!!!!!!!!
 
  generalDeviceType = "CoolingCMS";
  deviceType = "CoolingCMS Device (1)";
}
else if(strpos(deviceType,"fwWnrCr_CSC_LV")>=0){
  
    dpCreate(deviceName,"fwWnrCr_CSC_LV_d");       // !!!!!!!!!!!!!!!!!!!!!
 
  generalDeviceType = "WienerSnmp";
  deviceType = "fwWnrCr_CSC_LV Device";
}
else if(strpos(deviceType,"fwWnrCh_CSC_LV")>=0){
  
    dpCreate(deviceName,"fwWnrCh_CSC_LV_d");       // !!!!!!!!!!!!!!!!!!!!!
 
  generalDeviceType = "WienerSnmp";
  deviceType = "fwWnrCh_CSC_LV Device";
}
else if(strpos(deviceType,"LV")>=0){
  if(strpos(deviceType,"LV_1")>=0){
    dpCreate(deviceName,"LV_1_d");       // !!!!!!!!!!!!!!!!!!!!!
  }

  generalDeviceType = "LowVoltage";
  if(strpos(deviceType,"LV_1")>=0)deviceType = "Low Voltage Device (1)";
}
else if(strpos(deviceType,"HV")>=0){
  if(strpos(deviceType,"HV_1")>=0){
    dpCreate(deviceName,"HV_1_d");       // !!!!!!!!!!!!!!!!!!!!!
  }
  else if(strpos(deviceType,"HV_PR")>=0){
    dpCreate(deviceName,"HV_PR_d");       // !!!!!!!!!!!!!!!!!!!!!
  }
  generalDeviceType = "HighVoltage";
  if(strpos(deviceType,"HV_1")>=0)deviceType = "High Voltage Device (1)";
  else if(strpos(deviceType,"HV_PR")>=0)deviceType = "High Voltage Device (PR)";

}
else if(strpos(deviceType,"TEMP")>=0){
  if(strpos(deviceType,"TEMP_1")>=0){
    dpCreate(deviceName,"TEMP_1_d");       // !!!!!!!!!!!!!!!!!!!!!
  }

  generalDeviceType = "Temperature";
  if(strpos(deviceType,"TEMP_1")>=0)deviceType = "Temperature Device (1)";

}
else if(strpos(deviceType,"CHIP")>=0){
  if(strpos(deviceType,"CHIP_1")>=0){
    dpCreate(deviceName,"CHIP_1_d");       // !!!!!!!!!!!!!!!!!!!!!
  }

  generalDeviceType = "ChipLoading";
  if(strpos(deviceType,"CHIP_1")>=0)deviceType = "Chip Loading Device (1)";
}
else if(strpos(deviceType,"GAS")>=0){
  generalDeviceType = "GasMonitoring";
  if(strpos(deviceType,"GAS_SX5")>=0)deviceType = "Gas Monitoring Device (SX5)";
}
else if(strpos(deviceType,"WTH")>=0){
  if(strpos(deviceType,"WTH_SX5")>=0){
    dpCreate(deviceName,"WTH_SX5_d");       // !!!!!!!!!!!!!!!!!!!!!
  }
  generalDeviceType = "WeatherStation";
  if(strpos(deviceType,"WTH_SX5")>=0)deviceType = "Weather Station Device (SX5)";
}
else if(strpos(deviceType,"PT100")>=0){
  if(strpos(deviceType,"PT100")>=0){
    dpCreate(deviceName,"PT100_d");       // !!!!!!!!!!!!!!!!!!!!!
  }
  generalDeviceType = "GasMonitoring";
  if(strpos(deviceType,"PT100")>=0)deviceType = "PT100 Device";
}
else if(strpos(deviceType,"CRB")>=0){
  if(strpos(deviceType,"CRB_1")>=0){
    dpCreate(deviceName,"CRB_1_d");       // !!!!!!!!!!!!!!!!!!!!!
  }
  generalDeviceType = "CRB";
  if(strpos(deviceType,"CRB_1")>=0)deviceType = "CRB Device (1)";
}
else if(strpos(deviceType,"MRTN")>=0){
  if(strpos(deviceType,"MRTN_1")>=0){
    dpCreate(deviceName,"MRTN_1_d");       // !!!!!!!!!!!!!!!!!!!!!
  }
  generalDeviceType = "MRTN";
  if(strpos(deviceType,"MRTN_1")>=0)deviceType = "MRTN Device (1)";
}
else if(strpos(deviceType,"WNR12")>=0){
  if(strpos(deviceType,"WNR12_1")>=0){
    dpCreate(deviceName,"WNR12_1_d");       // !!!!!!!!!!!!!!!!!!!!!
  }
  generalDeviceType = "WNR12";
  if(strpos(deviceType,"WNR12_1")>=0)deviceType = "WNR12 Device (1)";
}
else if(strpos(deviceType,"ALNM")>=0){
  if(strpos(deviceType,"ALNM_1")>=0){
    dpCreate(deviceName,"ALNM_1_d");       // !!!!!!!!!!!!!!!!!!!!!
  }
  generalDeviceType = "ALNM";
  if(strpos(deviceType,"ALNM_1")>=0)deviceType = "ALNM Device (1)";
}

	
	fwDevice_getDpType(deviceType, dpType, exceptionInfo);
		
	dpN = generalDeviceType + fwDevice_HIERARCHY_SEPARATOR + deviceName;

DebugTN(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>1");   
DebugTN(deviceName +" >>>>>>>>>>> "+ dpType+" >>>>>>>>>>>>>>>>> "+generalDeviceType);
DebugTN(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>2");        
	fwDevice_create(makeDynString(deviceName, dpType, deviceComment, ""/*deviceModelText.text*/), 
					makeDynString(generalDeviceType, "", "", ""), 
					exceptionInfo);

 

 

}





//====================================================
//====================================================
//====================================================

mudcs_addLogical_gas(bool isDevice,  string parent, string node, string deviceType,
string config_panel, string op_panel){

dyn_string exceptionInfo;
string generalDeviceType;
int j;

 if(!isDevice){
	createNode(parent,node,config_panel,op_panel,exceptionInfo);
	if(dynlen(exceptionInfo) > 0)
		fwExceptionHandling_display(exceptionInfo);
 }
 else{

  dyn_string deviceObject,ds;
  string dpAlias;
  dpAlias=parent;
if(strpos(deviceType,"fwCooling_CSC_COOLING")>=0){
  
  generalDeviceType = "CoolingCMS";
  deviceType = "CoolingCMS Device (1)";
} 
else if(strpos(deviceType,"fwGasSystem_CSC_GAS")>=0){
  
  generalDeviceType = "GasCMS";
  deviceType = "GasCMS Device (1)";
}
else if(strpos(deviceType,"fwWnrCr_CSC_LV")>=0){
  
  generalDeviceType = "WienerSnmp";
  deviceType = "fwWnrCr_CSC_LV Device";
}
else if(strpos(deviceType,"fwWnrCh_CSC_LV")>=0){
  
  generalDeviceType = "WienerSnmp";
  deviceType = "fwWnrCh_CSC_LV Device";
}
else if(strpos(deviceType,"LV")>=0){
  generalDeviceType = "LowVoltage";
  if(strpos(deviceType,"LV_1")>=0)deviceType = "Low Voltage Device (1)";
}
else if(strpos(deviceType,"HV")>=0){
  generalDeviceType = "HighVoltage";
  if(strpos(deviceType,"HV_1")>=0)deviceType = "High Voltage Device (1)";
  else if(strpos(deviceType,"HV_PR")>=0)deviceType = "High Voltage Device (PR)";

}
else if(strpos(deviceType,"TEMP")>=0){
  generalDeviceType = "Temperature";
  if(strpos(deviceType,"TEMP_1")>=0)deviceType = "Temperature Device (1)";

}
else if(strpos(deviceType,"CHIP")>=0){
  generalDeviceType = "ChipLoading";
  if(strpos(deviceType,"CHIP_1")>=0)deviceType = "Chip Loading Device (1)";
}
else if(strpos(deviceType,"GAS")>=0){
  generalDeviceType = "GasMonitoring";
  if(strpos(deviceType,"GAS_SX5")>=0)deviceType = "Gas Monitoring Device (SX5)";
}
else if(strpos(deviceType,"WTH")>=0){
  generalDeviceType = "WeatherStation";
  if(strpos(deviceType,"WTH_SX5")>=0)deviceType = "Weather Station Device (SX5)";
}
else if(strpos(deviceType,"PT100")>=0){
  generalDeviceType = "GasMonitoring";
  if(strpos(deviceType,"PT100")>=0)deviceType = "PT100 Device";
}
else if(strpos(deviceType,"CRB")>=0){
  generalDeviceType = "CRB";
  if(strpos(deviceType,"CRB_1")>=0)deviceType = "CRB Device (1)";
}
else if(strpos(deviceType,"MRTN")>=0){
  generalDeviceType = "MRTN";
  if(strpos(deviceType,"MRTN_1")>=0)deviceType = "MRTN Device (1)";
}
else if(strpos(deviceType,"WNR12")>=0){
  generalDeviceType = "WNR12";
  if(strpos(deviceType,"WNR12_1")>=0)deviceType = "WNR12 Device (1)";
}
else if(strpos(deviceType,"ALNM")>=0){
  generalDeviceType = "ALNM";
  if(strpos(deviceType,"ALNM_1")>=0)deviceType = "ALNM Device (1)";
}

//   ds=makeDynString(CSC_fwG_g_SYSTEM_NAME+":"+"LowVoltage/CSCdm4r2c01LV_1 | | | | | CSCdm4r2c01LV_1");//,"");

//DebugN("+++++++++++++++++++++++++++++++++++++++++++++++"+CSC_fwG_g_SYSTEM_NAME+":"+
//        generalDeviceType +"/"+ node+" | | | | | "+node);

ds=makeDynString(CSC_fwG_g_SYSTEM_NAME+":"+
        generalDeviceType +"/"+ node+" | | | | | "+node);

  for(int i = 1; i <= dynlen(ds); i++){
  	fwGeneral_stringToDynString(ds[i], deviceObject);
        
        if(dpExists(deviceObject[fwDevice_DP_NAME] + "."))DebugTN("EXISTS");
        else{for(j=1;j<=10;j++){
          delay(0,20);
          DebugTN("Not Exist");
          if((dpExists(deviceObject[fwDevice_DP_NAME] + "."))){DebugTN("EXISTS");break;}
          
        }
        }
        DebugTN(deviceObject[fwDevice_DP_NAME] + "."+" ALIAS2");
        DebugTN(dpAlias + fwDevice_HIERARCHY_SEPARATOR + deviceObject[fwDevice_ALIAS]+" ALIAS2");
        
        
  	dpSetAlias(deviceObject[fwDevice_DP_NAME] + ".", dpAlias + fwDevice_HIERARCHY_SEPARATOR + deviceObject[fwDevice_ALIAS]);

        dynAppend(CSC_fwG_g_EMU_ALIASES, deviceObject[fwDevice_DP_NAME] + "."
             +"#"+
              dpAlias + fwDevice_HIERARCHY_SEPARATOR + deviceObject[fwDevice_ALIAS]);

  }


 } // else

}
//====================================================




