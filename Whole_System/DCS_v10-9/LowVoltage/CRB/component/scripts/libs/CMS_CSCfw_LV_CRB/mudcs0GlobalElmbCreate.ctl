string CSC_fwCAN1_g_EmuCmsGlobalNode, CSC_fwCAN1_g_EmuCmsGlobalType;
string CSC_fwCAN1_g_EmuCmsGlobalCu, CSC_fwCAN1_g_EmuCmsGlobalParent;
string CSC_fwCAN1_g_MainLogicalFsmType="CSC_LV_NODES";
string CSC_fwCAN1_g_NodeLogicalFsmType="CSC_LV_TREE_NODES";



//============================================================================
mudcsLv_addNode(){
  int pos_doublecolon=strpos(CSC_fwCAN1_g_EmuCmsGlobalNode,"::");
  int pos_colon=strpos(CSC_fwCAN1_g_EmuCmsGlobalNode,":");
  int is_exists;

  string just_node=CSC_fwCAN1_g_EmuCmsGlobalNode;
  if(pos_doublecolon>=0)just_node=substr(CSC_fwCAN1_g_EmuCmsGlobalNode,pos_doublecolon+2);
  
  is_exists= fwFsmTree_isNode(just_node);
  if(is_exists)return;
  if(pos_colon >=0 && pos_colon==(strlen(CSC_fwCAN1_g_EmuCmsGlobalNode)-1))return;
  
 int cu_flag;
 if(CSC_fwCAN1_g_EmuCmsGlobalCu=="0")cu_flag=0;
 else cu_flag=1;
 fwFsmTree_addNode(CSC_fwCAN1_g_EmuCmsGlobalParent,CSC_fwCAN1_g_EmuCmsGlobalNode,CSC_fwCAN1_g_EmuCmsGlobalType,cu_flag);
// DebugTN("+++"+CSC_fwCAN1_g_EmuCmsGlobalParent+">>>"+CSC_fwCAN1_g_EmuCmsGlobalNode+">>>"+CSC_fwCAN1_g_EmuCmsGlobalType+">>"+cu_flag);
 
// fwFsmTree_generateTreeNode(CSC_fwCAN1_g_EmuCmsGlobalNode);
 return;
}

//====================================================
mudcsLv_addLogical(bool isDevice,  string parent, string node, string deviceType,
string config_panel, string op_panel){

dyn_string exceptionInfo;
string generalDeviceType;

 if(!isDevice){
	mudcsLvCreateNode(parent,node,config_panel,op_panel,exceptionInfo);
	if(dynlen(exceptionInfo) > 0)
		fwExceptionHandling_display(exceptionInfo);
 }
 else{

  dyn_string deviceObject,ds, ds1;
  string dpAlias, node_leaf;
  dpAlias=parent;

node=mudcsLvRemoveSystem(node);  


ds1=strsplit(node,fwDevice_HIERARCHY_SEPARATOR);  
node_leaf=ds1[dynlen(ds1)];

ds=makeDynString(mudcsLvAddSystem(node)+" | | | | | "+node);


  for(int i = 1; i <= dynlen(ds); i++){
  	fwGeneral_stringToDynString(ds[i], deviceObject);
//dpSetAlias(deviceObject[fwDevice_DP_NAME] + ".", dpAlias + fwDevice_HIERARCHY_SEPARATOR + deviceObject[fwDevice_ALIAS]);
dpSetAlias(deviceObject[fwDevice_DP_NAME] + ".", dpAlias + fwDevice_HIERARCHY_SEPARATOR + node_leaf);
  DebugTN(deviceObject[fwDevice_DP_NAME] + "."+"==========="+ dpAlias + fwDevice_HIERARCHY_SEPARATOR + node_leaf);    
  
/*        
        dynAppend(CSC_fwCAN1_g_EMU_ALIASES, deviceObject[fwDevice_DP_NAME] + "."
             +"#"+
              dpAlias + fwDevice_HIERARCHY_SEPARATOR + deviceObject[fwDevice_ALIAS]);  
*/
  
   if(globalExists("CSC_fwCAN1_g_EMU_ALIASES")) 
        dynAppend(CSC_fwCAN1_g_EMU_ALIASES, deviceObject[fwDevice_DP_NAME] + "."
             +"#"+
              dpAlias + fwDevice_HIERARCHY_SEPARATOR + node_leaf);  
  else if(globalExists("CSC_fwCAN2_g_EMU_ALIASES"))     
        dynAppend(CSC_fwCAN2_g_EMU_ALIASES, deviceObject[fwDevice_DP_NAME] + "."
             +"#"+
              dpAlias + fwDevice_HIERARCHY_SEPARATOR + node_leaf);            
             
  } // for i
 } // else


}
//====================================================
//=============================================================================================================

mudcsLvCreateNode(string sDpName, string nodeNameText, string config_panel, string op_panel, dyn_string exceptionInfo)
{ 



// my_corr:
//  new params of mudcsLvCreateNode:sDpName, nodeNameText, op_panel, config_panel
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

//////mudcsLvDebug(name);
	
	if(dynlen(exceptionInfo) > 0)
		return;
	 

	
	dpSet(mudcsLvAddSystem(name + ".type"), type);
	dpSet(mudcsLvAddSystem(name + ".dpTypes"), makeDynString());
	dpSet(mudcsLvAddSystem(name + ".navigatorPanels"), makeDynString(op_panel));
	dpSet(mudcsLvAddSystem(name + ".editorPanels"), makeDynString(config_panel));
	
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


//========== ?????????????????????????????
// to define globals, to create dp for mudcsLvalias (just in case)
//===========

//=======================================================================

mudcsLv_addHardwareDevice(string deviceName,string deviceType,string deviceComment, string &dpN)
{
/////	string 	deviceName = "CSCdm4r2c01LV_1",
/////			deviceType = "Low Voltage Device (1)",
//////	string		deviceComment, 
string			dpType;//////////, dpName;
        string generalDeviceType;

	dyn_string exceptionInfo;
//DebugTN("======================================================###################################");
        
//mudcsLvPutCscDeviceList(deviceType);

if(strpos(deviceType,"fwCrb")>=0){
  generalDeviceType = "CRB";
  if(strpos(deviceType,"fwCrb_CSC_LV")>=0)deviceType = "CRB Device (2)";
}
//else if(strpos(deviceType,"MRTN")>=0){
//  generalDeviceType = "MRTN";
//  if(strpos(deviceType,"MRTN_2")>=0)deviceType = "MRTN Device (2)";
//}
//DebugTN(deviceName+">>"+deviceType+">>"+generalDeviceType); 
	fwDevice_getDpType(deviceType, dpType, exceptionInfo);		
	dpN = generalDeviceType + fwDevice_HIERARCHY_SEPARATOR + deviceName;
//DebugTN(deviceName+">"+dpType+">"+generalDeviceType);        
	fwDevice_create(makeDynString(deviceName, dpType, deviceComment, ""/*deviceModelText.text*/), 
					makeDynString(generalDeviceType, "", "", ""), 
					exceptionInfo);
}

//====================================================
//============================
mudcsLvDeleteAllTrees(string type)
{

int i;
//int cu_flag;

system("rm scripts/libs/*'$install'");

//CSC_fwCAN1_g_EmuCmsSpecialMode=1;

CSC_fwCAN1_g_EmuCmsGlobalNode=CSC_fwCAN1_g_csc_part;
//CSC_fwCAN1_g_EmuCmsGlobalType="MainNode";
//CSC_fwCAN1_g_EmuCmsGlobalCu="1";cu_flag = 1;
CSC_fwCAN1_g_EmuCmsGlobalParent="FSM";

//onSelectSimulation(1,1,CSC_fwCAN1_g_EmuCmsGlobalNode,CSC_fwCAN1_g_EmuCmsGlobalType,cu_flag);

// mudcsLvDebug(CSC_fwCAN1_g_EmuCmsGlobalParent);
// mudcsLvDebug(CSC_fwCAN1_g_EmuCmsGlobalNode);

mudcsLv_removeNode();

//-----------------------------------------------------------------
// 10/01/2004: see doc: this is because some dpoint may remain in the tree that prevents the following tree creation
dyn_string ds,ds2, fwTN_names;


if(type=="CRB"){
 ds=dpNames("*CRB*","_FwTreeNode");
 dynAppend(ds2,ds);
 ds=dpNames("*CAN1*","_FwTreeNode"); 
 dynAppend(ds2,ds);
 ds=dpNames("*PSU*","_FwTreeNode"); 
 dynAppend(ds2,ds);
 fwTN_names=ds2;
  
}
else if(type=="MRTN"){
 ds=dpNames("*MRTN*","_FwTreeNode");
 dynAppend(ds2,ds);
 ds=dpNames("*CAN2*","_FwTreeNode"); 
 dynAppend(ds2,ds);
 ds=dpNames("*Wiener*","_FwTreeNode"); 
 dynAppend(ds2,ds);
 fwTN_names=ds2; 
  
}

//dyn_string fwTN_names=dpNames("*","_FwTreeNode");


for(i=1;i<=dynlen(fwTN_names);i++){
  if(strpos(fwTN_names[i],"TrendTree")>=0)continue;
  if(strpos(fwTN_names[i],"FSM")>=0)continue;  
dpDelete(fwTN_names[i]);
}
//----------------------------------------------------------------
//--- DELETE HARDWARE TREE (EXCEPT STANDARD ELMB-s-------------

mudcsLv_deleteHardwareDevices("fwCrb_CSC_LV");
//mudcsLv_deleteHardwareDevices("FwWienerMarathonChannel");
//mudcsLv_deleteHardwareDevices("FwWienerMarathon");

//-------  deleting logical tree ----------------------------------

//----------------------------------------------------------------


dpSet(mudcsLvAddSystem("EMUALIASES."),makeDynString()); // deleting dp wher aliase are stored

//CSC_fwCAN1_g_EmuCmsSpecialMode=0;

}

//=========================================================================


//=========================================================================

mudcsLv_removeNode(){


 fwFsmTree_removeNode(CSC_fwCAN1_g_EmuCmsGlobalParent,CSC_fwCAN1_g_EmuCmsGlobalNode,1);
 return;

}
//=========================================================================
//===========================================================================

mudcsLv_deleteHardwareDevices(string generalDeviceType){

dyn_string exceptionInfo;
dyn_string dps;
int i;


//dps=dpNames(generalDeviceType + fwDevice_HIERARCHY_SEPARATOR + "*" );

dps=dpNames("*", generalDeviceType);


for(i=1;i<=dynlen(dps);i++){
DebugTN("delete "+mudcsLvAddSystem(dps[i]));
dpDelete(mudcsLvAddSystem(dps[i]));
//fwDevice_delete(mudcsLvAddSystem(dps[i]), exceptionInfo);
}

//mudcsDebug2(""+dynlen(dps));

}

//====================================================================

mudcsLv_deleteAllHw(){
  

dyn_string ds;
int i;

//===========
    
 mudcsCrbPnlInit(0);
//=================

 
mudcsLvDeleteAllTrees("CRB");// fsm 

//==================== // CRB nodes 
 
ds=dpNames("*","fwCrb_CSC_LV");

for(i=1;i<=dynlen(ds);i++){
 DebugTN("delete "+mudcsLvAddSystem(ds[i]));
 dpDelete(mudcsLvAddSystem(ds[i]));
}  
dpTypeDelete("fwCrb_CSC_LV");

//================ HW

  
//dyn_string ds;
//int i;
// mudcsCrbPnlInit(0); 

ds=dpNames("*","FwElmbAi");

for(i=1;i<=dynlen(ds);i++){
 DebugTN("delete "+mudcsLvAddSystem(ds[i]));
 dpDelete(mudcsLvAddSystem(ds[i]));
}  

    
ds=dpNames("*","FwElmbDo");

for(i=1;i<=dynlen(ds);i++){
 DebugTN("delete "+mudcsLvAddSystem(ds[i]));
 dpDelete(mudcsLvAddSystem(ds[i]));
}  


    
ds=dpNames("*","FwElmbNode");

for(i=1;i<=dynlen(ds);i++){
 DebugTN("delete "+mudcsLvAddSystem(ds[i]));
 dpDelete(mudcsLvAddSystem(ds[i]));
}  

ds=dpNames("*","FwElmbCANbus");

for(i=1;i<=dynlen(ds);i++){
 DebugTN("delete "+mudcsLvAddSystem(ds[i]));
 dpDelete(mudcsLvAddSystem(ds[i]));
}  

ds=dpNames("*","FwElmbPSUBranch");

for(i=1;i<=dynlen(ds);i++){
 DebugTN("delete "+mudcsLvAddSystem(ds[i]));
 dpDelete(mudcsLvAddSystem(ds[i]));
}  

ds=dpNames("*","FwElmbPSUCrate");

for(i=1;i<=dynlen(ds);i++){
 DebugTN("delete "+mudcsLvAddSystem(ds[i]));
 dpDelete(mudcsLvAddSystem(ds[i]));
}  

//=================  // db
 
dpDelete("db_pcmb");
dpTypeDelete("Db_PCMB"); 

  
}


