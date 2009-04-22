#uses "CMS_CSCfw_Supervisor/emuDcsSuper.ctl"





//===============================================================

mudcsSuperCreateTree()
{
  
mudcsSuperInit();  
//mudcsDebug(dynlen(project_stations));
  
int cu_flag;  
string parent_domain;  

if(!dpExists("PROJECT_SYSTEM"))dpCreate("PROJECT_SYSTEM","MUDCS_STRING");
  
int i,j;
dyn_string disk_db, disk_db2, disk_db3;
dyn_string ds_split, ds_split2;
   
CSC_fwGSuper_g_EmuCmsGlobalNode=CSC_fwGSuper_g_csc_part;
CSC_fwGSuper_g_EmuCmsGlobalType= CSC_fwGSuper_g_MainLogicalFsmType;//"TEST_TYPE";
CSC_fwGSuper_g_EmuCmsGlobalCu="1";cu_flag = 1;
CSC_fwGSuper_g_EmuCmsGlobalParent="FSM";


// mudcsSuper_addLogical(false, "", CSC_fwGSuper_g_EmuCmsGlobalNode, CSC_fwGSuper_g_EmuCmsGlobalType,
// CSC_fwGSuper_g_Component+"/"+dir_config+"/emuEmuTemplate", CSC_fwGSuper_g_Component+"/emuEmuTemplate");

 mudcsSuper_addNode();

//================================================================== 
parent_domain=CSC_fwGSuper_g_csc_part;

string fsm_type; 

//------------------------------------
disk_db=dpNames("*:CSC_LV|CSC_LV","_FwFsmObject"); //
disk_db2=dpNames("*:CSC_HV|CSC_HV","_FwFsmObject"); //
dynAppend(disk_db,disk_db2);
disk_db2=dpNames("*:CSC_GAS|CSC_GAS","_FwFsmObject"); //
dynAppend(disk_db,disk_db2);
disk_db2=dpNames("*:CSC_COOLING|CSC_COOLING","_FwFsmObject"); //
dynAppend(disk_db,disk_db2);

 for(j=1;j<=dynlen(disk_db);j++){ 
   dpGet(disk_db[j]+".type",fsm_type); 
   ds_split=strsplit(disk_db[j],"|");

CSC_fwGSuper_g_EmuCmsGlobalNode=ds_split[1];// 
//CSC_fwGSuper_g_EmuCmsGlobalNode=ds_split[1]+"::"+ds_split[dynlen(ds_split)];// THAT ALSO WORKS BUT CU FLAG SHOULD BE 0, OTHERWISE NO CHILD NODES

CSC_fwGSuper_g_EmuCmsGlobalCu="1";cu_flag = 1; //  ??? poprobovat' po raznomu t.e. 1 i 0

CSC_fwGSuper_g_EmuCmsGlobalParent=parent_domain; // 
CSC_fwGSuper_g_EmuCmsGlobalType= fsm_type;//CSC_fwGSuper_g_TreeLogicalFsmType;

mudcsSuper_addNode();
 } // for 
 
 
//------------------------------------ 

DebugTN(CSC_fwGSuper_g_project_stations_super);
dyn_string project_stations_super=CSC_fwGSuper_g_project_stations_super;

for(i=1;i<=dynlen(project_stations_super);i++){
DebugTN("*:CSC_ME_"+project_stations_super[i]+"|CSC_ME_"+project_stations_super[i],"_FwFsmObject");
disk_db=dpNames("*:CSC_ME_"+project_stations_super[i]+"|CSC_ME_"+project_stations_super[i],"_FwFsmObject"); //
DebugTN(disk_db);
//disk_db2=dpNames("*:CSC_LV|CSC_LV","_FwFsmObject"); //
//disk_db3=dpNames("*:CSC_HV|CSC_HV","_FwFsmObject"); //
//dynAppend(disk_db,disk_db2);
//dynAppend(disk_db,disk_db3);

 for(j=1;j<=dynlen(disk_db);j++){
   dpGet(disk_db[j]+".type",fsm_type); 
   ds_split=strsplit(disk_db[j],"|");

CSC_fwGSuper_g_EmuCmsGlobalNode=ds_split[1];// 
//CSC_fwGSuper_g_EmuCmsGlobalNode=ds_split[1]+"::"+ds_split[dynlen(ds_split)];// THAT ALSO WORKS BUT CU FLAG SHOULD BE 0, OTHERWISE NO CHILD NODES

CSC_fwGSuper_g_EmuCmsGlobalCu="1";cu_flag = 1; //  ??? poprobovat' po raznomu t.e. 1 i 0

CSC_fwGSuper_g_EmuCmsGlobalParent=parent_domain; // 
CSC_fwGSuper_g_EmuCmsGlobalType= fsm_type;//CSC_fwGSuper_g_TreeLogicalFsmType;

mudcsSuper_addNode();
 } // for
 
} // for


 
//=======================================================================
fwFsmTree_generateAll();
}

//=========================================================================
//========================================================================
//======================================================================================
mudcsSuperDeleteTree()
{

int i;
int cu_flag;

system("rm "+CSC_fwGSuper_g_project_name_home+"/scripts/libs/*'$install'");
/*
 dps_to_delete=dpNames("*","fwWnrCr_CSC_LV_d");
 for(i=1;i<=dynlen(dps_to_delete);i++){
  if(strpos(dps_to_delete[i],"DimBroker") >=0 )continue;
  dpDelete(dps_to_delete[i]);
 }
 dps_to_delete=dpNames("*","fwWnrCh_CSC_LV_d");
 for(i=1;i<=dynlen(dps_to_delete);i++){
  if(strpos(dps_to_delete[i],"DimBroker") >=0 )continue;
  dpDelete(dps_to_delete[i]);
 } 
*/

//CSC_fwGSuper_EmuCmsSpecialMode=1;

CSC_fwGSuper_g_EmuCmsGlobalNode=CSC_fwGSuper_g_csc_part;
CSC_fwGSuper_g_EmuCmsGlobalType="MainNode";
CSC_fwGSuper_g_EmuCmsGlobalCu="1";cu_flag = 1;
CSC_fwGSuper_g_EmuCmsGlobalParent="FSM";

mudcsSuper_removeNode();


//mudcsSuper_deleteHardwareDevices("fwWnrCr_CSC_LV");
//mudcsSuper_deleteHardwareDevices("fwWnrCh_CSC_LV");

//-------  deleting logical tree ----------------------------------
/*
string dpAlias=CSC_fwGSuper_g_csc_part;
int pos=2;
int parentPos=1;

	dyn_string exceptionInfo;
*/
// attention !!! to apply this the modification in fwDevice/fwDevice.ctl should be done
// beside I am not sure the aliases for devives will be deleted ! so it is better way to delete the log tree manualy !!! 
// fwDevice_deleteLogical(makeDynString("", "", "", "", dpAlias), "", exceptionInfo);

//---  detetion of the logical tree ---------
///int j;
///dyn_string log_points_to_delete=dpNames(CSC_fwGSuper_g_csc_part+"*","FwNode"); 
///for(j=1;j<=dynlen(log_points_to_delete);j++){
/// dpDelete(log_points_to_delete[j]);// this is because fwDevice_deleteLogical does not work from 06/06/2006 : CMS_CSC/type = ROOT DELETE NOD
///}
//---------------------------------------------


//-----------------------------------------------------------------
// 10/01/2004: see doc: this is because some dpoint may remain in the tree that prevents the following tree creation

dyn_string fwTN_names=dpNames("*","_FwTreeNode");
for(i=1;i<=dynlen(fwTN_names);i++){
  if(strpos(fwTN_names[i],"TrendTree")>=0)continue;
  if(strpos(fwTN_names[i],"FSM")>=0)continue;  
dpDelete(fwTN_names[i]);
}

//----------------------------------------------------------------

//CSC_fwGSuper_EmuCmsSpecialMode=0;

}

//=========================================================================

mudcsSuper_removeNode(){


 fwFsmTree_removeNode(CSC_fwGSuper_g_EmuCmsGlobalParent,CSC_fwGSuper_g_EmuCmsGlobalNode,1);
 return;


}
//=========================================================================

mudcsSuper_addNode(){


  
 int cu_flag;
 if(CSC_fwGSuper_g_EmuCmsGlobalCu=="0")cu_flag=0;
 else cu_flag=1;
 fwFsmTree_addNode(CSC_fwGSuper_g_EmuCmsGlobalParent,CSC_fwGSuper_g_EmuCmsGlobalNode,CSC_fwGSuper_g_EmuCmsGlobalType,cu_flag);
////// fwFsmTree_setNodeLabel(CSC_fwGSuper_g_EmuCmsGlobalNode,CSC_fwGSuper_g_EmuCmsGlobalNode+"111");
///// fwFsmTree_generateTreeNode(CSC_fwGSuper_g_EmuCmsGlobalNode);
 return;

}

//============================================================================
//========================================================================
//============================================================================


//===========================================================================

mudcsSuper_deleteHardwareDevices(string generalDeviceType){

dyn_string exceptionInfo;
dyn_string dps;
int i;


//dps=dpNames(generalDeviceType + fwDevice_HIERARCHY_SEPARATOR + "*" );

dps=dpNames("*", generalDeviceType);

for(i=1;i<=dynlen(dps);i++){
DebugTN("delete "+mudcsSuperAddSystem(dps[i]));
dpDelete(mudcsSuperAddSystem(dps[i]));
//fwDevice_delete(mudcsSuperAddSystem(dps[i]), exceptionInfo);
}
/*
if(dynlen(exceptionInfo)>=1){
  DebugTN("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
    DebugTN("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
      DebugTN("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
        DebugTN("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
        
  DebugTN(exceptionInfo);
}
*/
//mudcsSuperDebug2(""+dynlen(dps));

}

//====================================================================


//====================================================
/*
mudcsSuper_addLogical(bool isDevice,  string parent, string node, string deviceType,
string config_panel, string op_panel){

dyn_string exceptionInfo;
string generalDeviceType;

 if(!isDevice){
	mudcsSuperCreateNode(parent,node,config_panel,op_panel,exceptionInfo);
	if(dynlen(exceptionInfo) > 0)
		fwExceptionHandling_display(exceptionInfo);
 }
 else{

  dyn_string deviceObject,ds;
  string dpAlias;
  dpAlias=parent;

if(strpos(deviceType,"fwWnrCr_CSC_LV")>=0){
  
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
  	dpSetAlias(deviceObject[fwDevice_DP_NAME] + ".", dpAlias + fwDevice_HIERARCHY_SEPARATOR + deviceObject[fwDevice_ALIAS]);

        dynAppend(CSC_fwG_g_EMU_ALIASES, deviceObject[fwDevice_DP_NAME] + "."
             +"#"+
              dpAlias + fwDevice_HIERARCHY_SEPARATOR + deviceObject[fwDevice_ALIAS]);

  }


 } // else

}
//====================================================
*/
//=============================================================================================================

mudcsSuperCreateNode(string sDpName, string nodeNameText, string config_panel, string op_panel, dyn_string exceptionInfo)
{ 



// my_corr:
//  new params of createNode:sDpName, nodeNameText, op_panel, config_panel
//  $sDpName --> sDpName
//  see my_corr above and below  
//  nodeNameText.text() --> nodeNameText
// navigatorPanelText.text() --> op_panel 
// editotPanelText.text()  --> config_panel

	string name, type, alias;
//	dyn_string exceptionInfo;
	dyn_errClass err;		
						
	if (sDpName == "")
	{
		name = nodeNameText;
		alias = nodeNameText;
		type = fwNode_TYPE_LOGICAL_ROOT;
	}
	else
	{
		name = sDpName + fwDevice_HIERARCHY_SEPARATOR + nodeNameText;
		alias = dpGetAlias(sDpName + ".") + fwDevice_HIERARCHY_SEPARATOR + nodeNameText;
		type = fwNode_TYPE_LOGICAL;	
	}

	fwDevice_create(makeDynString(nodeNameText, "FwNode", "", ""),
					makeDynString(sDpName, ""),
					exceptionInfo);		

//////mudcsSuperDebug(name);
	
	if(dynlen(exceptionInfo) > 0)
		return;
	 

	
	dpSet(mudcsSuperAddSystem(name + ".type"), type);
	dpSet(mudcsSuperAddSystem(name + ".dpTypes"), makeDynString());
	dpSet(mudcsSuperAddSystem(name + ".navigatorPanels"), makeDynString(op_panel));
	dpSet(mudcsSuperAddSystem(name + ".editorPanels"), makeDynString(config_panel));
	
	dpSetAlias(name + ".", alias);
 
 	//test whether there were errors
	err = getLastError(); 
 	if(dynlen(err) > 0)
 	{
 		dpDelete(name);
 		fwException_raise(	exceptionInfo,
 							"ERROR",
 							"Could not create the logical node",
 							"");
		return;
 	}


}

//=============================
