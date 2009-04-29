int id;
string station_label="s";
int mode_tree_creation=1;
string Component="CMS_CSCfw_HV_CC";
string dir_config="config";
bool custom_lv;
bool isWholeCrbFolderAdd=true;
bool isWholeMrtnFolderAdd=false;
dyn_string project_stations=makeDynString("M4","M3","M2","M1","P1","P2","P3","P4"); 
//
mudcsPostCreateConfig(){
 
  int i;
  dyn_string dps_hv=dpNames("*","HV_1"); 
  for(i=1;i<=dynlen(dps_hv);i++){
    dpSet(mudcsAddSystem(dps_hv[i]+".last_vset"),3600);
  }
  
  dps_hv=dpNames("*","HV_PR"); 
  for(i=1;i<=dynlen(dps_hv);i++){
    dpSet(mudcsAddSystem(dps_hv[i]+".last_vset"),3700);
  }  
  
  dpSet(mudcsAddSystem("vset_primary_default."),3700);
  dpSet(mudcsAddSystem("vset_all_default."),3600);
    
  dyn_string dps_wnr=dpNames("*","fwWnrCr_CSC_LV");
  for(i=1;i<=dynlen(dps_wnr);i++){
    dpSet(mudcsAddSystem(dps_wnr[i]+".status"),3); 
  //comment:  it may be made -2 if we create tree before the "You may operate"
  // netherless the -2 is converted to +2 (now same as +3) as the alert is active
  }    
  dyn_string dps_wnr=dpNames("*","fwWnrCh_CSC_LV");
  for(i=1;i<=dynlen(dps_wnr);i++){
    dpSet(mudcsAddSystem(dps_wnr[i]+".status"),2);   
  }
  
  dyn_string dps_lv=dpNames("*","LV_1");
  for(i=1;i<=dynlen(dps_lv);i++){
    dpSet(mudcsAddSystem(dps_lv[i]+".status"),2);   
  }  
 dps_lv=dpNames("*","TEMP_1");
  for(i=1;i<=dynlen(dps_lv);i++){
    dpSet(mudcsAddSystem(dps_lv[i]+".status"),2);   
  }   
}

//======================================================================================
mudcsDeleteAllTrees()
{

int i;
int cu_flag;
string operating_system;
operating_system=getenv("OS");

//system("rm "+CSC_fwG_g_project_name_home+"/scripts/libs/*'$install'"); // with $ not correctly interpreted by PVSS
if(operating_system=="Linux") 
system("rm "+CSC_fwG_g_project_name_home+"/scripts/libs/CSC_*install");
else system("cmd /c rm "+CSC_fwG_g_project_name_home+"\\scripts\\libs/CSC_*install");

dpSetWait(mudcsAddSystem("EMUNAMEALIASES."),makeDynString());

// delete_database();

//--
  dpDelete(CSC_fwG_g_SYSTEM_NAME+":"+"vset_master_100_20_15_default");
  dpDelete(CSC_fwG_g_SYSTEM_NAME+":"+"vset_primary_default");
  dpDelete(CSC_fwG_g_SYSTEM_NAME+":"+"vset_all_default");

 dyn_string dps_to_delete=dpNames("*","HV_PR_d");
 for(i=1;i<=dynlen(dps_to_delete);i++){
  if(strpos(dps_to_delete[i],"DimBroker") >=0 )continue;
  dpDelete(dps_to_delete[i]);
 }
 dyn_string dps_to_delete=dpNames("*","HV_1_d");
 for(i=1;i<=dynlen(dps_to_delete);i++){
  if(strpos(dps_to_delete[i],"DimBroker") >=0 )continue;
  dpDelete(dps_to_delete[i]);
 }
 dps_to_delete=dpNames("*","LV_1_d");
 for(i=1;i<=dynlen(dps_to_delete);i++){
  if(strpos(dps_to_delete[i],"DimBroker") >=0 )continue;
  dpDelete(dps_to_delete[i]);
 }
 dps_to_delete=dpNames("*","TEMP_1_d");
 for(i=1;i<=dynlen(dps_to_delete);i++){
  if(strpos(dps_to_delete[i],"DimBroker") >=0 )continue;
  dpDelete(dps_to_delete[i]);
 }
 dps_to_delete=dpNames("*","CHIP_1_d");
 for(i=1;i<=dynlen(dps_to_delete);i++){
  if(strpos(dps_to_delete[i],"DimBroker") >=0 )continue;
  dpDelete(dps_to_delete[i]);
 }
 dps_to_delete=dpNames("*","WTH_SX5_d");
 for(i=1;i<=dynlen(dps_to_delete);i++){
  if(strpos(dps_to_delete[i],"DimBroker") >=0 )continue;
  dpDelete(dps_to_delete[i]);
 }
 dps_to_delete=dpNames("*","PT100_d");
 for(i=1;i<=dynlen(dps_to_delete);i++){
  if(strpos(dps_to_delete[i],"DimBroker") >=0 )continue;
  dpDelete(dps_to_delete[i]);
 }
 dps_to_delete=dpNames("*","CRB_1_d");
 for(i=1;i<=dynlen(dps_to_delete);i++){
  if(strpos(dps_to_delete[i],"DimBroker") >=0 )continue;
  dpDelete(dps_to_delete[i]);
 }
 dps_to_delete=dpNames("*","MRTN_1_d");
 for(i=1;i<=dynlen(dps_to_delete);i++){
  if(strpos(dps_to_delete[i],"DimBroker") >=0 )continue;
  dpDelete(dps_to_delete[i]);
 }
 dps_to_delete=dpNames("*","WNR12_1_d");
 for(i=1;i<=dynlen(dps_to_delete);i++){
  if(strpos(dps_to_delete[i],"DimBroker") >=0 )continue;
  dpDelete(dps_to_delete[i]);
 }
 dps_to_delete=dpNames("*","ALNM_1_d");
 for(i=1;i<=dynlen(dps_to_delete);i++){
  if(strpos(dps_to_delete[i],"DimBroker") >=0 )continue;
  dpDelete(dps_to_delete[i]);
 }
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

//--

// dps_to_delete=dpNames("*","HV_PR");
// for(i=1;i<=dynlen(dps_to_delete);i++){
//  dpDelete(dps_to_delete[i]);
// }



CSC_fwG_EmuCmsSpecialMode=1;

EmuCmsGlobalNode=CSC_fwG_g_csc_part;
CSC_fwG_EmuCmsGlobalType="MainNode";
CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1;
CSC_fwG_EmuCmsGlobalParent="FSM";

onSelectSimulation(1,1,EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,cu_flag);

////////mudcsDebug2(""+CSC_fwG_EmuCmsSpecialMode);

mudcs_removeNode();
/*
mudcs_deleteHardwareDevices("LowVoltage");
mudcs_deleteHardwareDevices("Temperature");
mudcs_deleteHardwareDevices("ChipLoading");
mudcs_deleteHardwareDevices("GasMonitoring");
mudcs_deleteHardwareDevices("HighVoltage");
mudcs_deleteHardwareDevices("WeatherStation");
mudcs_deleteHardwareDevices("CRB");
mudcs_deleteHardwareDevices("MRTN");
mudcs_deleteHardwareDevices("WNR12");
mudcs_deleteHardwareDevices("ALNM");
*/
mudcs_deleteHardwareDevices("LV_1");
mudcs_deleteHardwareDevices("TEMP_1");
mudcs_deleteHardwareDevices("CHIP_1");
mudcs_deleteHardwareDevices("GAS_SX5");
mudcs_deleteHardwareDevices("HV_1");
mudcs_deleteHardwareDevices("HV_PR");
mudcs_deleteHardwareDevices("WTH_SX5");
mudcs_deleteHardwareDevices("CRB_1");
mudcs_deleteHardwareDevices("MRTN_1");
mudcs_deleteHardwareDevices("fwWnrCr_CSC_LV");
mudcs_deleteHardwareDevices("fwWnrCh_CSC_LV");
mudcs_deleteHardwareDevices("fwGasSystem_CSC_GAS");
mudcs_deleteHardwareDevices("fwCooling_CSC_COOLING");
    
//-------  deleting logical tree ----------------------------------

string dpAlias=CSC_fwG_g_csc_part;
int pos=2;
int parentPos=1;

	dyn_string exceptionInfo;
// attention !!! to apply this the modification in fwDevice/fwDevice.ctl should be done
// beside I am not sure the aliases for devives will be deleted ! so it is better way to delete the log tree manualy !!! 
// fwDevice_deleteLogical(makeDynString("", "", "", "", dpAlias), "", exceptionInfo);

//---  detetion of the logical tree ---------
///int j;
///dyn_string log_points_to_delete=dpNames(CSC_fwG_g_csc_part+"*","FwNode"); 
///for(j=1;j<=dynlen(log_points_to_delete);j++){
/// dpDelete(log_points_to_delete[j]);// this is because fwDevice_deleteLogical does not work from 06/06/2006 : CMS_CSC/type = ROOT DELETE NOD
///}
//---------------------------------------------

///////////////////////////fwTreeView_getParentNodePosition(pos, parentPos, "");
//DebugN("Parent position is: " + parentPos + " " + pos);				
//////////fwDeviceEditorNavigator_expandTree(parentPos, "");
//////////fwTreeView_draw("logicalTree.");
////mudcsDebug(" "+dpAlias+" "+pos+" "+parentPos);

//-----------------------------------------------------------------
// 10/01/2004: see doc: this is because some dpoint may remain in the tree that prevents the following tree creation

dyn_string fwTN_names=dpNames("*","_FwTreeNode");
for(i=1;i<=dynlen(fwTN_names);i++){
  if(strpos(fwTN_names[i],"TrendTree")>=0)continue;
  if(strpos(fwTN_names[i],"FSM")>=0)continue;  
dpDelete(fwTN_names[i]);
}       
        
//----------------------------------------------------------------


dpSet(mudcsAddSystem("EMUALIASES."),makeDynString()); // deleting dp wher aliase are stored

CSC_fwG_EmuCmsSpecialMode=0;

}

//=========================================================================

mudcs_removeNode(){


if(mode_tree_creation){
 fwFsmTree_removeNode(CSC_fwG_EmuCmsGlobalParent,EmuCmsGlobalNode,1);
 return;
}


int redo;
dyn_string redo_nodes;//236
string new_node;
int i;


//230
string redo_node, redo_node_parent;
	redo_node_parent = "";

// 230
///int wait=0; 
///string tree;
dyn_string exInfo; //236

//236

		new_node = fwFsmTree_askRemoveNode(CSC_fwG_EmuCmsGlobalParent, EmuCmsGlobalNode,redo);
		dynAppend(redo_nodes,new_node);
/*
// 230
fwFsmTree_askRemoveNode(CSC_fwG_EmuCmsGlobalParent, EmuCmsGlobalNode, redo);
redo_node = CSC_fwG_EmuCmsGlobalParent;
*/

/* 204
_fwTree_askRemoveNode(CSC_fwG_EmuCmsGlobalParent, EmuCmsGlobalNode, redo);

	//	_fwTree_askAddNode(node, redo, new_node);

		if(CSC_fwG_EmuCmsGlobalParent != "")
			redo_nodes[1] = CSC_fwG_EmuCmsGlobalParent;
*/

////mudcsDebug2("1");



// 236

	if(redo)
	{
//DebugN("redo",redo, redo_node_parent, redo_nodes);

		if(redo_node_parent != "")
		{
			if(!fwFsm_isDomain(redo_node_parent))
			{
				fwTree_getCUName(redo_node_parent, redo_node_parent, exInfo);
				if(redo_node_parent == ""){
					redo_node_parent = "FSM";
                                }
			}
//DebugN("node_parent", redo_node_parent);
			if((redo_node_parent != "FSM") && (redo_node_parent != fwFsm_clipboardNodeName))
				fwFsmTree_generateTreeNode(redo_node_parent, 0);
			fwTreeDisplay_setRedoTreeNode(FwActiveTrees[CurrTreeIndex], redo_node_parent);
		}
		for(i = 1; i <= dynlen(redo_nodes); i++)
		{
			if(!fwFsm_isDomain(redo_nodes[i]))
			{
				fwTree_getCUName(redo_nodes[i], redo_nodes[i], exInfo);
				if(redo_nodes[i] == ""){
					redo_nodes[i] = "FSM";
                                }
			}
//DebugN("nodes", i, redo_nodes[i]);
			if(redo_nodes[i] != redo_node_parent)
			{
				if((redo_nodes[i] != "FSM") && (redo_nodes[i] != fwFsm_clipboardNodeName))
					fwFsmTree_generateTreeNode(redo_nodes[i], 0);
				fwTreeDisplay_setRedoTreeNode(FwActiveTrees[CurrTreeIndex], redo_nodes[i]);
			}
		}
       } // if redo

/*
// 230:
	if(redo)
	{

		if(redo_node_parent != "")
			fwTreeDisplay_setRedoTreeNode(FwActiveTrees[CurrTreeIndex], redo_node_parent);
		else
			fwTreeDisplay_setRedoTreeNode(FwActiveTrees[CurrTreeIndex], redo_node);
		if((redo_node != "FSM") && (redo_node != fwFsm_clipboardNodeName))
		{
//			_fwTree_redoTreeNode(redo_node);
			if((redo_node_parent == "FSM") || (redo_node_parent == fwFsm_clipboardNodeName))
				redo_node_parent = "";
			fwFsmTree_generateTreeNode(redo_node, 0, redo_node_parent);
		}

        }
*/
/* 204
	if(redo)
	{


		RedoTree2 = 1;
		_fwTree_redoTree();
		for(i = 1; i <= dynlen(redo_nodes); i++)
		{
			_fwTree_generateTreeNode(redo_nodes[i]);
		}

	}
*/

// 230:
	fwUi_uninformUserProgress(1);

}
//=========================================================================

mudcs_addNode(){


if(mode_tree_creation){
 int cu_flag;
 if(CSC_fwG_EmuCmsGlobalCu=="0")cu_flag=0;
 else cu_flag=1;
 fwFsmTree_addNode(CSC_fwG_EmuCmsGlobalParent,EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,cu_flag);
////// fwFsmTree_setNodeLabel(EmuCmsGlobalNode,EmuCmsGlobalNode+"111");
///// fwFsmTree_generateTreeNode(EmuCmsGlobalNode);
 return;
}


int redo;
dyn_string redo_nodes, new_nodes; // 236
string new_node;
int i;

// 230
string redo_node, redo_node_parent;
	redo_node_parent = "";
////int wait=0; 
////string tree;
dyn_string exInfo; //236

//236
if(CSC_fwG_EmuCmsGlobalParent == "FSM"){

		fwFsmTree_askAddNode("FSM", redo, new_nodes, "Object");
		dynAppend(redo_nodes, new_nodes);
}
else{
		fwFsmTree_askAddNode(CSC_fwG_EmuCmsGlobalParent, redo, new_nodes, "Object");
		dynAppend(redo_nodes, new_nodes);
		if(CSC_fwG_EmuCmsGlobalParent != "FSM")
			redo_node_parent = CSC_fwG_EmuCmsGlobalParent;
}

/*
// 230:		
if(CSC_fwG_EmuCmsGlobalParent == "FSM"){
		fwFsmTree_askAddNode("FSM", redo, new_node, "Object");
		redo_node = new_node;
}
else{
		fwFsmTree_askAddNode(CSC_fwG_EmuCmsGlobalParent, redo, new_node, "Object");
		redo_node = new_node;
		if(CSC_fwG_EmuCmsGlobalParent != "FSM")
			redo_node_parent = CSC_fwG_EmuCmsGlobalParent;
}
*/

/* 204
_fwTree_askAddNode(CSC_fwG_EmuCmsGlobalParent, redo, new_node, EmuCmsGlobalNode);

	//	_fwTree_askAddNode(node, redo, new_node);
		if(CSC_fwG_EmuCmsGlobalParent != "")
			redo_nodes[1] = CSC_fwG_EmuCmsGlobalParent;
		else
			redo_nodes[1] = new_node;
////mudcsDebug2("1");
*/
// 236

	if(redo)
	{
//DebugN("redo",redo, redo_node_parent, redo_nodes);

		if(redo_node_parent != "")
		{
			if(!fwFsm_isDomain(redo_node_parent))
			{
				fwTree_getCUName(redo_node_parent, redo_node_parent, exInfo);
				if(redo_node_parent == ""){
					redo_node_parent = "FSM";
                                }
			}
//DebugN("node_parent", redo_node_parent);
			if((redo_node_parent != "FSM") && (redo_node_parent != fwFsm_clipboardNodeName))
				fwFsmTree_generateTreeNode(redo_node_parent, 0);
			fwTreeDisplay_setRedoTreeNode(FwActiveTrees[CurrTreeIndex], redo_node_parent);
		}
		for(i = 1; i <= dynlen(redo_nodes); i++)
		{
			if(!fwFsm_isDomain(redo_nodes[i]))
			{
				fwTree_getCUName(redo_nodes[i], redo_nodes[i], exInfo);
				if(redo_nodes[i] == ""){
					redo_nodes[i] = "FSM";
                                }
			}
//DebugN("nodes", i, redo_nodes[i]);
			if(redo_nodes[i] != redo_node_parent)
			{
				if((redo_nodes[i] != "FSM") && (redo_nodes[i] != fwFsm_clipboardNodeName))
					fwFsmTree_generateTreeNode(redo_nodes[i], 0);
				fwTreeDisplay_setRedoTreeNode(FwActiveTrees[CurrTreeIndex], redo_nodes[i]);
			}
		}
       } // if redo

/*
// 230:
	if(redo)
	{

		if(redo_node_parent != "")
			fwTreeDisplay_setRedoTreeNode(FwActiveTrees[CurrTreeIndex], redo_node_parent);
		else
			fwTreeDisplay_setRedoTreeNode(FwActiveTrees[CurrTreeIndex], redo_node);
		if((redo_node != "FSM") && (redo_node != fwFsm_clipboardNodeName))
		{
//			_fwTree_redoTreeNode(redo_node);
			if((redo_node_parent == "FSM") || (redo_node_parent == fwFsm_clipboardNodeName))
				redo_node_parent = "";
			fwFsmTree_generateTreeNode(redo_node, 0, redo_node_parent);
		}

        }
*/

/* 204
	if(redo)
	{
		RedoTree2 = 1;
		_fwTree_redoTree();
		for(i = 1; i <= dynlen(redo_nodes); i++)
		{
			_fwTree_generateTreeNode(redo_nodes[i]);
		}
	}	

*/
// 230:
	fwUi_uninformUserProgress(1);

}

//============================================================================
//========================================================================
//============================================================================
//========================================================================
onSelectSimulation(int level, int isTree, string domain,string type,int cu_flag){


if(mode_tree_creation){
 return;
}

//	int id;
	unsigned pos; //position in the printed list
	unsigned index; //index in the tree array

string add_item;
//string sys;

if(level==0)add_item="";
else if(level==1)add_item="    ";
else if(level==2)add_item="        ";
else if(level==3)add_item="            ";
else if(level==4)add_item="                ";
else if(level==5)add_item="                    ";
else if(level==6)add_item="                        ";
else if(level==7)add_item="                            ";

if(isTree)add_item=add_item+"-";
else add_item=add_item+" ";

//sys = _fwTree_getNodeSys(domain);
//sys="nnnn";

/////DebugN("44444"+add_item+domain);
setValue("list", "selectedText", add_item+domain);

//setValue("object", "text", domain);
//setValue("type", "text", type);
//setValue("CU_flag", "state",0, cu_flag);

getValue("list", "selectedPos", pos);
index  = fwTreeView_view2TreeIndex(pos);

//a=1;

// 204 id=startThread("_fwTree_showNode", index);
// 230:
id=startThread("fwTreeDisplay_selectNode", index);

while(id!=-1) delay(0,100);

////setValue("object", "text", domain);
/////a=1;

}
//===========================================================================
/*
mudcs_deleteHardwareDevices(string generalDeviceType){

dyn_string exceptionInfo;
dyn_string dps;
int i;


dps=dpNames(generalDeviceType + fwDevice_HIERARCHY_SEPARATOR + "*" );

for(i=1;i<=dynlen(dps);i++){
//mudcsDebug2(dps[i]);
fwDevice_delete(dps[i], exceptionInfo);
}

//mudcsDebug2(""+dynlen(dps));

}
*/
//====================================================================
//===========================================================================

mudcs_deleteHardwareDevices(string generalDeviceType){

dyn_string exceptionInfo;
dyn_string dps;
int i;


//dps=dpNames(generalDeviceType + fwDevice_HIERARCHY_SEPARATOR + "*" );

dps=dpNames("*", generalDeviceType);

for(i=1;i<=dynlen(dps);i++){
DebugTN("delete "+mudcsAddSystem(dps[i]));
dpDelete(mudcsAddSystem(dps[i]));
//fwDevice_delete(mudcsAddSystem(dps[i]), exceptionInfo);
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
//mudcsDebug2(""+dynlen(dps));

}

//====================================================================
delete_database(){

dyn_string name_list;

name_list=makeDynString(".InDynatemAndSetNumberList",".OutDynatemAndSetNumberList");

int w,r;

for(w=1;w<=8;w++){
for(r=1;r<=2;r++){
dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel"+w+name_list[r]),makeDynString());
}
}

name_list=makeDynString(".InHVsetList",".OutHVsetList");

for(w=1;w<=8;w++){
for(r=1;r<=2;r++){
dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel"+w+name_list[r]),makeDynString());
}
}

// for ME 1/1
dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Ring_DUBNA.DynatemAndSetNumberList"),makeDynString());
dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Ring_DUBNA.HVsetList"),makeDynString());

}

//====================================================================
//====================================================================
//====================================================================
//====================================================================
mudcsCreatePT100(string parent_node_test)
{

dyn_string CscLevelDevices;
string parent_domain;
string Node_save;
string dpN;
int cu_flag;
dyn_string panels;

CSC_fwG_EmuCmsSpecialMode=1;
panels=makeDynString("fwFSMuser/fwUi.pnl");

 dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);

//------- add PT100 -----------

// Note:
//   Probably INSEAD OF to deal with setting panels for the fsm nodes in the 
//   mudcsCreateAllTrees():
//   dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);
//   it is possible to set that panel in two ways:
//      -- editor/fsm tab/ 
//               edit LV_1 type e.g by setting panel 
//     -- correct the _FwFsmObjectType/fwOT_LV_1.panel 
//

mudcs_selectParent(2, 1, parent_node_test,"DCSNodes",1, parent_domain);

 CSC_fwG_EmuCmsGlobalType="PT100";
 mudcsNameCompose(CSC_fwG_EmuCmsGlobalType, "", "", 0, "", "", EmuCmsGlobalNode);
 //EmuCmsGlobalNode= "CSC"+"PT100"; 
 Node_save=EmuCmsGlobalNode;
 CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
 EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+parent_node_test, 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevPT100Config", Component+"/emuDevPT100Operation");

 mudcs_addNode();

 dynRemove(panels,2); 
 dynAppend(panels,Component+"/emuDevPT100Operation.pnl"); 

dpN=fwFsm_getLogicalDeviceName(EmuCmsGlobalNode);
////////dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);

//----array of coordinates of devices: cell_format: "device_name coordinates"------ 
/// if(dynContains(CscLevelDevices,EmuCmsGlobalNode+" PT100;0") == 0  )
/// dynAppend(CscLevelDevices,EmuCmsGlobalNode+" PT100;0"); 
//---------------------------------------------------------------------------------

///// dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);

}
//====================================================================
//====================================================================
//====================================================================
//====================================================================

mudcsCreateWTH_SX5(string parent_node_test)
{

dyn_string CscLevelDevices;
string parent_domain;
string Node_save;
string dpN;
int cu_flag;
dyn_string panels;

CSC_fwG_EmuCmsSpecialMode=1;
panels=makeDynString("fwFSMuser/fwUi.pnl");

 dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);

//------- add WTH_SX5 -----------

// Note:
//   Probably INSEAD OF to deal with setting panels for the fsm nodes in the 
//   mudcsCreateAllTrees():
//   dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);
//   it is possible to set that panel in two ways:
//      -- editor/fsm tab/ 
//               edit LV_1 type e.g by setting panel 
//     -- correct the _FwFsmObjectType/fwOT_LV_1.panel 
//

mudcs_selectParent(2, 0, parent_node_test,"DCSNodes",1, parent_domain);

 CSC_fwG_EmuCmsGlobalType="WTH_SX5";
 mudcsNameCompose(CSC_fwG_EmuCmsGlobalType, "", "", 0, "", "", EmuCmsGlobalNode);
 //EmuCmsGlobalNode= "CSC"+"WTH_SX5"; 
 Node_save=EmuCmsGlobalNode;
 CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
 EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+parent_node_test, 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevWTH_SX5Config", Component+"/emuDevWTH_SX5Operation");

 mudcs_addNode();

 dynRemove(panels,2); 
 dynAppend(panels,Component+"/emuDevWTH_SX5Operation.pnl"); 

dpN=fwFsm_getLogicalDeviceName(EmuCmsGlobalNode);
////////dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);

//----array of coordinates of devices: cell_format: "device_name coordinates"------ 
 /////if(dynContains(CscLevelDevices,EmuCmsGlobalNode+" WEATHERSTATION;0") == 0  )
 /////dynAppend(CscLevelDevices,EmuCmsGlobalNode+" WEATHERSTATION;0"); 
//---------------------------------------------------------------------------------

///// dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);

}
//====================================================================
//====================================================================
//====================================================================
//====================================================================


mudcsCreateHV_PR(string  parent_node_test)
{

dyn_string CscLevelDevices;
string parent_domain;
string Node_save;
string dpN;
int cu_flag;
dyn_string panels;
string parent_node;

CSC_fwG_EmuCmsSpecialMode=1;
panels=makeDynString("fwFSMuser/fwUi.pnl");

int i, i_host;

//============= DB ===========================

 int i_db;
 dyn_string emu_db;
 dyn_string hosts_db; 
 dyn_string s_split, s_split2;
 dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);
 for(i_db=1;i_db<=dynlen(CscLevelDevices);i_db++){
   if(strpos(CscLevelDevices[i_db],"HV_PRIMARY")>=0 ){
     s_split=strsplit(CscLevelDevices[i_db]," ");
     dynAppend(emu_db,substr(s_split[1], strlen("HighVoltage/HV_PR_")  )) ;
     s_split2=strsplit(s_split[2],";");
     if( !dynContains(hosts_db,s_split2[2]) )dynAppend(hosts_db,s_split2[2]);
   } 
 }
     //CSC_fwG_g_all_maratons=emu_db;
     CSC_fwG_g_all_primary=emu_db;
     CSC_fwG_g_all_hosts=hosts_db;
//============================================

mudcs_selectParent(2, 0, parent_node_test, "DCSNodes",1, parent_domain);

CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
mudcsNameCompose("HV_HOSTS", "", "", 0, "", "", EmuCmsGlobalNode);
parent_node=EmuCmsGlobalNode;
//  parent_node=EmuCmsGlobalNode;
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addLogical(false, CSC_fwG_g_csc_part+"/"+parent_node_test, 
 EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuHV_HOSTSConfig", Component+"/emuHV_HOSTSOperation");
 
mudcs_addNode();



/// dynRemove(panels,2);
/// dynAppend(panels,Component+"/emuHV_PROperation.pnl");
/// dpSet(mudcsAddSystem(EmuCmsGlobalNode+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);


//-----------------------

// dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);

//-----------------------

//dyn_strnig primaries_for_host;

string host_s;
string parent_node_test_2;

for(i_host=1;i_host<=dynlen(CSC_fwG_g_all_hosts);i_host++){


mudcs_selectParent(-1, -1, parent_node, "",-1, parent_domain);

CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
mudcsNameCompose("HV_HOST", "", "", CSC_fwG_g_all_hosts[i_host], "", "", EmuCmsGlobalNode);
parent_node_test_2=EmuCmsGlobalNode;

CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addLogical(false, CSC_fwG_g_csc_part+"/"+parent_node_test+"/"+parent_node, 
 EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuHV_HOSTConfig", Component+"/emuHV_HOSTOperation");
 
mudcs_addNode();

//------- add devices -----------

host_s=CSC_fwG_g_all_hosts[i_host];

 for(i=1;i<=dynlen(CSC_fwG_g_all_primary);i++){

if(strpos(CSC_fwG_g_all_primary[i],host_s) < 0)continue;

//mudcsDebug(parent_node);

mudcs_selectParent(-1, -1, parent_node_test_2,"",-1, parent_domain);

 CSC_fwG_EmuCmsGlobalType="HV_PR";
 mudcsNameCompose(CSC_fwG_EmuCmsGlobalType,CSC_fwG_g_all_primary[i],"",0,"","", EmuCmsGlobalNode);
 //EmuCmsGlobalNode= "CSC"+"HV_PR_"+CSC_fwG_g_all_primary[i]; 
 Node_save=EmuCmsGlobalNode;
 CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
 EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+parent_node_test+"/"+parent_node+"/"+parent_node_test_2, 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevHV_PRConfig", Component+"/emuDevHV_PROperation");

 mudcs_addNode();

//// dynRemove(panels,2); 
//// dynAppend(panels,Component+"/emuDevHV_PROperation.pnl"); 

dpN=fwFsm_getLogicalDeviceName(EmuCmsGlobalNode);
////////dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);

//----array of coordinates of devices: cell_format: "device_name coordinates"------ 
///// if(i==1 && dynContains(CscLevelDevices,EmuCmsGlobalNode+" HV_PRIMARY;500;1;1") == 0  )
///// dynAppend(CscLevelDevices,EmuCmsGlobalNode+" HV_PRIMARY;500;1;1");

//// else if(i==2 && dynContains(CscLevelDevices,EmuCmsGlobalNode+" HV_PRIMARY;500;1;2") == 0  )
//// dynAppend(CscLevelDevices,EmuCmsGlobalNode+" HV_PRIMARY;500;1;2");
//---------------------------------------------------------------------------------

 } // for over primaries

////// dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);

} // loop ove i_host

}



//====================================================================
//====================================================================
//====================================================================
//====================================================================


mudcsCreateWNR_SNMP(string parent_node_test)
{

dyn_string CscLevelDevices;
string parent_domain;
string Node_save;
string dpN;
int cu_flag;
dyn_string panels;
string parent_node, parent_node_host;

CSC_fwG_EmuCmsSpecialMode=1;
panels=makeDynString("fwFSMuser/fwUi.pnl");

int i,j;

//================= DB ============================================================

 int i_db;
 dyn_string emu_db;
 dyn_string s_split, s_split2;
 dyn_string hosts;
 dyn_string channels;
 string s_test;
 
 dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);
  for(i_db=1;i_db<=dynlen(CscLevelDevices);i_db++){
   if(strpos(CscLevelDevices[i_db],"SNMP")>=0 ){
     s_split=strsplit(CscLevelDevices[i_db]," ");
     dynAppend(emu_db,s_split[1]);
     s_split2=strsplit(s_split[2],";");
     dynAppend(hosts,s_split2[2]) ;     
   } 
  } // for
 
     CSC_fwG_g_all_maratons=emu_db;
//==================================================================================

     
if(dynlen(emu_db)==0)return;     


//---------------------
mudcs_selectParent(-1, -1, parent_node_test, "",-1, parent_domain);

CSC_fwG_EmuCmsGlobalType="CSC_LV_HV";//CSC_fwG_g_NodeLogicalFsmType;
mudcsNameCompose("LV_HV", "", "", 0, "", "", EmuCmsGlobalNode);
 parent_node=EmuCmsGlobalNode;
//EmuCmsGlobalNode="CSC"+"LV_MRTN"; //r2
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addLogical(false, CSC_fwG_g_csc_part+"/"+parent_node_test, 
 EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuPanelComponentTemplate", Component+"/emuLV_MRTNOperation");
 
mudcs_addNode();
//------------------------
for(i=1;i<=dynlen(emu_db);i++){ // over hosts
//---------------------
mudcs_selectParent(-1, -1, parent_node, "",-1, parent_domain);

CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
mudcsNameCompose("WIENER_SNMP_HOST", emu_db[i], "", 0, "", "", EmuCmsGlobalNode);
 parent_node_host=EmuCmsGlobalNode;
//EmuCmsGlobalNode="CSC"+"LV_MRTN"; //r2
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addLogical(false, CSC_fwG_g_csc_part+"/"+parent_node_test+"/"+parent_node, 
 EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuPanelComponentTemplate", Component+"/emuLvSnmpOperation");
 
mudcs_addNode();
//------------------------


 mudcs_selectParent(-1, -1,parent_node_host ,"", -1, parent_domain);

 CSC_fwG_EmuCmsGlobalType="fwWnrCr_CSC_LV";
 s_test="WienerSnmpCrate"+hosts[i];
 mudcsNameCompose(CSC_fwG_EmuCmsGlobalType, s_test , "", 0, "", "", EmuCmsGlobalNode);
 // EmuCmsGlobalNode= "CSC"+CSC_fwG_g_all_maratons[i]+"MRTN_1"; 
 Node_save=EmuCmsGlobalNode;
 CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
 EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+parent_node_test+"/"+parent_node+"/"+parent_node_host, 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevWnrCr_CSC_LVConfig", Component+"/emuDevWnrCr_CSC_LVOperation");

 mudcs_addNode();

 dpSetWait(dpN+".coord",hosts[i]);
  
dpN=fwFsm_getLogicalDeviceName(EmuCmsGlobalNode);

string node_parent=Node_save;
    
//------------------------

 for(j=1;j<=12;j++){ // loop over 12 channels

// mudcs_selectParent(-1, -1,parent_node_host ,"", -1, parent_domain);
 sprintf(s_test,"%02d",j);
 channels[j]= node_parent +"/"+ "Channel"+s_test;
 CSC_fwG_EmuCmsGlobalType="fwWnrCh_CSC_LV";
 mudcsNameCompose(CSC_fwG_EmuCmsGlobalType,channels[j] , "", 0, "", "", EmuCmsGlobalNode);
 // EmuCmsGlobalNode= "CSC"+CSC_fwG_g_all_maratons[i]+"MRTN_1";
 Node_save=EmuCmsGlobalNode;
 CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwG_EmuCmsGlobalParent=parent_domain;

// DebugTN(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> "+EmuCmsGlobalNode);
 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
 EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+parent_node_test+"/"+parent_node+"/"+parent_node_host, 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevWnrCh_CSC_LVConfig", Component+"/emuDevWnrCh_CSC_LVOperation");

 mudcs_addNode();

dpSetWait(dpN+".coord",hosts[i]+";"+s_test);

dpN=fwFsm_getLogicalDeviceName(EmuCmsGlobalNode);

//---------------------------------------------------------------------------------------------

 } // for over crates

} // over hosts 
 
}

//====================================================================
//====================================================================
//====================================================================
//====================================================================

//====================================================================
//====================================================================
//====================================================================
//====================================================================


mudcsCreateMRTN(int idisk, int idisk_cross, string emu_system_side, string parent_node_test)
{

dyn_string CscLevelDevices;
string parent_domain;
string Node_save;
string dpN;
int cu_flag;
dyn_string panels;
string parent_node;

CSC_fwG_EmuCmsSpecialMode=1;
panels=makeDynString("fwFSMuser/fwUi.pnl");

int i;
string MRTN_SYSTEM_NAME,MRTN_DOMAIN;



 string EMU_SYSTEM_SIDE="P";
 MRTN_SYSTEM_NAME=CSC_fwG_g_MRTN_SYSTEM_NAME_P;
 MRTN_DOMAIN=CSC_fwG_g_MRTN_DOMAIN_P; 

if(emu_system_side=="m"){
 EMU_SYSTEM_SIDE="M";
 MRTN_SYSTEM_NAME=CSC_fwG_g_MRTN_SYSTEM_NAME_M;
 MRTN_DOMAIN=CSC_fwG_g_MRTN_DOMAIN_M;  
}

//================= DB ============================================================

 int i_db;
 dyn_string emu_db;
 dyn_string s_split;
 
 if(custom_lv){
   if(idisk==-1){  
 dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);
  for(i_db=1;i_db<=dynlen(CscLevelDevices);i_db++){
   if(strpos(CscLevelDevices[i_db],"WIENER")>=0 && strpos(CscLevelDevices[i_db],"MRTN")>=0){
     s_split=strsplit(CscLevelDevices[i_db]," ");
     dynAppend(emu_db,substr(s_split[1], strlen("MRTN/MRTN_")  )) ;
   } 
  } // for
   } // if disk==-1
 } // if custom lv
 else{
 
   if(idisk==-1){
     if(CSC_fwG_g_MRTN_SYSTEM_NAME_P!="")
       emu_db=dpNames(CSC_fwG_g_MRTN_SYSTEM_NAME_P+":*ME_P*_CR*_LV_MRTN","_FwFsmObject");      
   }
   else if(idisk==-2){
     if(CSC_fwG_g_MRTN_SYSTEM_NAME_M!="")
       emu_db=dpNames(CSC_fwG_g_MRTN_SYSTEM_NAME_M+":*ME_M*_CR*_LV_MRTN","_FwFsmObject");      
   }   
   else{ 
    if(isWholeMrtnFolderAdd){
      if(idisk==3) // for stations 2 and 3 are the same and are contained only in station 2 folder 
      emu_db=dpNames(MRTN_SYSTEM_NAME+":*"+EMU_SYSTEM_SIDE+"2_LV_MRTN","_FwFsmObject"); // whole folder with maratons on this disk!!!
      else emu_db=dpNames(MRTN_SYSTEM_NAME+":*"+EMU_SYSTEM_SIDE+idisk+"_LV_MRTN","_FwFsmObject"); // whole folder with maratons on this disk!!! 
    }
    else {
      if(idisk==3) // for stations 2 and 3 are the same and are contained only in station 2 folder 
      emu_db=dpNames(MRTN_SYSTEM_NAME+":*"+EMU_SYSTEM_SIDE+"2_CR*_LV_MRTN","_FwFsmObject");
      else emu_db=dpNames(MRTN_SYSTEM_NAME+":*"+EMU_SYSTEM_SIDE+idisk+"_CR*_LV_MRTN","_FwFsmObject");
    } 
   }
  /*
  for(i_db=1;i_db<=dynlen(emu_db);i_db++){
   s_split=strsplit(emu_db[i_db],"|");
   emu_db[i_db]=MRTN_SYSTEM_NAME+":"+MRTN_DOMAIN+"::"+s_split[dynlen(s_split)];
  } // for
  */
 }

     CSC_fwG_g_all_maratons=emu_db;
//==================================================================================

     
if(dynlen(emu_db)==0)return;     


if(idisk == -1 || idisk == -2){ // i.e. hardware tree folder: CSC_LV
//---------------------
mudcs_selectParent(2, -1, parent_node_test, "DCSNodes",1, parent_domain);

CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
if(idisk == -1)mudcsNameCompose("LV_MRTN_P", "", "", 0, "", "", EmuCmsGlobalNode);
else if(idisk == -2)mudcsNameCompose("LV_MRTN_M", "", "", 0, "", "", EmuCmsGlobalNode);
 parent_node=EmuCmsGlobalNode;
//EmuCmsGlobalNode="CSC"+"LV_MRTN"; //r2
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addLogical(false, CSC_fwG_g_csc_part+"/"+parent_node_test, 
 EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuLV_MRTNConfig", Component+"/emuLV_MRTNOperation");
 
mudcs_addNode();
//------------------------
} // if idisk==-1 hardware tree folder: CSC_LV
else if(!custom_lv && !isWholeMrtnFolderAdd){

mudcs_selectParent(2, -1, parent_node_test,"DCSNodes",1, parent_domain);

CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
mudcsNameCompose("_LV_MRTN", station_label, emu_system_side, idisk, "", "", EmuCmsGlobalNode);
 parent_node=EmuCmsGlobalNode;
//EmuCmsGlobalNode="CSC"+station_label+emu_system_side+idisk+"PC";
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addLogical(false, CSC_fwG_g_csc_part+"/"+parent_node_test, 
 EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuLV_MRTNConfig", Component+"/emuLV_MRTNOperation");
 
mudcs_addNode();  

} // else if idisk==-1 hardware tree folder: CSC_LV
//------------------------


// dynRemove(panels,2);
// dynAppend(panels,Component+"/emuLV_MRTNOperation.pnl");
// dpSet(mudcsAddSystem(EmuCmsGlobalNode+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);




if(custom_lv){     
 for(i=1;i<=dynlen(CSC_fwG_g_all_maratons);i++){

 mudcs_selectParent(3, i-1,parent_node ,"DCSNodes",1, parent_domain);

 CSC_fwG_EmuCmsGlobalType="MRTN_1";
 mudcsNameCompose(CSC_fwG_EmuCmsGlobalType,CSC_fwG_g_all_maratons[i] , "", 0, "", "", EmuCmsGlobalNode);
 // EmuCmsGlobalNode= "CSC"+CSC_fwG_g_all_maratons[i]+"MRTN_1"; 
 Node_save=EmuCmsGlobalNode;
 CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
 EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+parent_node_test+"/"+parent_node, 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevMRTN_1Config", Component+"/emuDevMRTN_1Operation");

 mudcs_addNode();

 dynRemove(panels,2); 
 dynAppend(panels,Component+"/emuDevMRTN_1Operation.pnl"); 

dpN=fwFsm_getLogicalDeviceName(EmuCmsGlobalNode);
////////dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);

//----array of coordinates of devices: cell_format: "device_name coordinates"------------------ 
/// if(i==1 && dynContains(CscLevelDevices,EmuCmsGlobalNode+" WIENER.CAN1.Crate1;0") == 0  )
/// dynAppend(CscLevelDevices,EmuCmsGlobalNode+" WIENER.CAN1.Crate1;0");

/// else if(i==2 && dynContains(CscLevelDevices,EmuCmsGlobalNode+" WIENER.CAN1.Crate2;0") == 0  )
/// dynAppend(CscLevelDevices,EmuCmsGlobalNode+" WIENER.CAN1.Crate2;0");

/// else if(i==3 && dynContains(CscLevelDevices,EmuCmsGlobalNode+" WIENER.CAN1.Crate3;0") == 0  )
/// dynAppend(CscLevelDevices,EmuCmsGlobalNode+" WIENER.CAN1.Crate3;0");

/// else if(i==3 && dynContains(CscLevelDevices,EmuCmsGlobalNode+" WIENER.CAN1.Crate4;0") == 0  )
/// dynAppend(CscLevelDevices,EmuCmsGlobalNode+" WIENER.CAN1.Crate4;0");

//---------------------------------------------------------------------------------------------

 } // for over maratons
} // if custom_lv
else{
 for(i=1;i<=dynlen(CSC_fwG_g_all_maratons);i++){ // if idisk!=-1 - only one item : the whole folder with maratons on this disk !!!   
   if(idisk==-1 || idisk==-2 || !isWholeMrtnFolderAdd)mudcs_selectParent(-1, -1, parent_node,"",-1, parent_domain);
   else mudcs_selectParent(-1, -1, parent_node_test,"",-1, parent_domain); // not a single maraton and a whole folder with maratons on this disk !!!! 
//   

CSC_fwG_EmuCmsGlobalType="CSC_LV_TREE_NODES";

//string EMU_SYSTEM_SIDE="P";
//if(emu_system_side=="n")EMU_SYSTEM_SIDE="N";
//EmuCmsGlobalNode=CSC_fwG_g_Dubna_System_Name+":CMS_CSC_LV_CAN1::"+"CSC_ME_"+EMU_SYSTEM_SIDE+idisk+"CR";
EmuCmsGlobalNode=CSC_fwG_g_all_maratons[i];//
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; 
CSC_fwG_EmuCmsGlobalParent=parent_domain;
mudcs_addNode();    
    
 } // for over maratons

  
}



 /////dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);

}

//====================================================================
//====================================================================
//====================================================================
//====================================================================


mudcsCreateWNR12(string parent_node_test)
{

dyn_string CscLevelDevices;
string parent_domain;
string Node_save;
string dpN;
int cu_flag;
dyn_string panels;
string parent_node;

CSC_fwG_EmuCmsSpecialMode=1;
panels=makeDynString("fwFSMuser/fwUi.pnl");

int i;

//---------------------

mudcs_selectParent(2, 1, parent_node_test, "DCSNodes",1, parent_domain);


CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
mudcsNameCompose("LV_WIENER12", "", "", 0, "", "", EmuCmsGlobalNode);
 parent_node=EmuCmsGlobalNode;
//EmuCmsGlobalNode="CSC"+"LV_WIENER12"; //r2
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addLogical(false, CSC_fwG_g_csc_part+"/"+parent_node_test, 
 EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuLV_WNR12Config", Component+"/emuLV_WNR12Operation");
 
mudcs_addNode();

 dynRemove(panels,2);
 dynAppend(panels,Component+"/emuLV_WNR12Operation.pnl");
 dpSet(mudcsAddSystem(EmuCmsGlobalNode+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);


//-----------------------

 dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);

//==================================================================================

 for(i=1;i<=dynlen(CSC_fwG_g_all_wieners12);i++){

mudcs_selectParent(3, i-1, parent_node,"DCSNodes",1, parent_domain);

 CSC_fwG_EmuCmsGlobalType="WNR12_1";
 mudcsNameCompose(CSC_fwG_EmuCmsGlobalType,CSC_fwG_g_all_wieners12[i] , "", 0, "", "", EmuCmsGlobalNode);
// EmuCmsGlobalNode= "CSC"+CSC_fwG_g_all_wieners12[i]+"WNR12_1"; 
 Node_save=EmuCmsGlobalNode;
 CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
 EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+parent_node_test+"/"+parent_node, 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevWNR12_1Config", Component+"/emuDevWNR12_1Operation");

 mudcs_addNode();

 dynRemove(panels,2); 
 dynAppend(panels,Component+"/emuDevWNR12_1Operation.pnl"); 

dpN=fwFsm_getLogicalDeviceName(EmuCmsGlobalNode);
////////dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);

//----array of coordinates of devices: cell_format: "device_name coordinates"------ 
/// if(i==1 && dynContains(CscLevelDevices,EmuCmsGlobalNode+" WIENER.CAN0.Crate11;0") == 0  )
/// dynAppend(CscLevelDevices,EmuCmsGlobalNode+" WIENER.CAN0.Crate11;0");

/// else if(i==2 && dynContains(CscLevelDevices,EmuCmsGlobalNode+" WIENER.CAN0.Crate12;0") == 0  )
/// dynAppend(CscLevelDevices,EmuCmsGlobalNode+" WIENER.CAN0.Crate12;0");

/// else if(i==3 && dynContains(CscLevelDevices,EmuCmsGlobalNode+" WIENER.CAN0.Crate13;0") == 0  )
/// dynAppend(CscLevelDevices,EmuCmsGlobalNode+" WIENER.CAN0.Crate13;0");

//---------------------------------------------------------------------------------

 } // for over primaries

////// dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);

}

//====================================================================
//====================================================================
//====================================================================
//====================================================================


mudcsCreateAtlasPsu(string parent_node_test)
{

dyn_string CscLevelDevices;
string parent_domain;
string Node_save;
string dpN;
int cu_flag;
dyn_string panels;
string parent_node;

CSC_fwG_EmuCmsSpecialMode=1;

int i;


//================= DB ============================================================

 int i_db;
 dyn_string emu_db;
 dyn_string s_split;


     emu_db=dpNames(CSC_fwG_g_CRB_SYSTEM_NAME+":*_LV_CR*_PSU","_FwFsmObject");      
 
  for(i_db=1;i_db<=dynlen(emu_db);i_db++){
   s_split=strsplit(emu_db[i_db],"|");
   emu_db[i_db]=CSC_fwG_g_CRB_SYSTEM_NAME+":"+CSC_fwG_g_CRB_DOMAIN+"::"+s_split[dynlen(s_split)];
  } // for
   
//==================================================================================

     
if(dynlen(emu_db)==0)return;     


///if(idisk == -1){ // i.e. hardware tree folder: CSC_LV
//---------------------
mudcs_selectParent(-1, -1, parent_node_test, "",-1, parent_domain);

CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
mudcsNameCompose("LV_PSU", "", "", 0, "", "", EmuCmsGlobalNode);
 parent_node=EmuCmsGlobalNode;
//EmuCmsGlobalNode="CSC"+"LV_MRTN"; //r2
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addLogical(false, CSC_fwG_g_csc_part+"/"+parent_node_test, 
 EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuEmptyConfig", Component+"/emuEmptyOperation");
 
mudcs_addNode();
//------------------------


 for(i=1;i<=dynlen(emu_db);i++){ //   
   mudcs_selectParent(-1, -1, parent_node,"",-1, parent_domain);   

CSC_fwG_EmuCmsGlobalType="CSC_LV_TREE_NODES";

EmuCmsGlobalNode=emu_db[i];//
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; 
CSC_fwG_EmuCmsGlobalParent=parent_domain;
mudcs_addNode();    
    
 } // for over atlas psu-s

  
//}



 /////dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);

}

//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
mudcsCreatePC(int idisk, int idisk_cross, string emu_system_side, string station_parent_node)
{

dyn_string list;  
dyn_string disk_db;
int i_db;
 dyn_string s_split;

string EMU_SYSTEM_SIDE="P";
if(emu_system_side=="m")EMU_SYSTEM_SIDE="M";
 
if(custom_lv){     
  if((idisk_cross==4 || idisk_cross==5)){ // i.e. ME 1/1
    dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Ring_DUBNA"+"."+"DiskLevelDevicesCoordinates"),list);
     for(i_db=1;i_db<=dynlen(list);i_db++){
      if(idisk_cross==4){
       if(strpos(list[i_db],"CSC_ME_M1")>=0 && strpos(list[i_db],"_CRB") >=0 && list[i_db]!="")
        {dynAppend(disk_db,substr(list[i_db],0,strpos(list[i_db]," ") ) );}
      }
      else if(idisk_cross==5){
       if(strpos(list[i_db],"CSC_ME_P1")>=0 && strpos(list[i_db],"_CRB") >=0 && list[i_db]!="")
        {dynAppend(disk_db,substr(list[i_db],0,strpos(list[i_db]," ") ) );}         
         
      }
     } // for 
  }
//  else{
   dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel"+idisk_cross+"."+"DiskLevelDevicesCoordinates"),list);
   for(i_db=1;i_db<=dynlen(list);i_db++){
    if(list[i_db]!="" && strpos(list[i_db],"_CRB") >=0 ){dynAppend(disk_db,substr(list[i_db],0,strpos(list[i_db]," ") ) );}
   }
//  }
} // if custom_lv
else{

 if(isWholeCrbFolderAdd)disk_db=dpNames(CSC_fwG_g_CRB_SYSTEM_NAME+":*"+EMU_SYSTEM_SIDE+idisk+"_LV_CRB","_FwFsmObject"); // 
 else disk_db=dpNames(CSC_fwG_g_CRB_SYSTEM_NAME+":*"+EMU_SYSTEM_SIDE+idisk+"_PC*_LV_CRB","_FwFsmDevice");//"fwCrb_CSC_LV"); // 
 
 for(i_db=1;i_db<=dynlen(disk_db);i_db++){
//   s_split=strsplit(disk_db[i_db],":");
   s_split=strsplit(disk_db[i_db],"|");
   disk_db[i_db]=CSC_fwG_g_CRB_SYSTEM_NAME+":"+CSC_fwG_g_CRB_DOMAIN+"::"+s_split[dynlen(s_split)];
   
 } // for
} // else if custom_lv

  
/*  
//------- add devices -----------

int ipcB=0;
int ipcL=-1;
//------------- the following table is endcap side regardless ---------------- 
if(emu_system_side =="p" && idisk==1){
      ipcB=1;
      ipcL=2;     
}
else if(emu_system_side =="p" && idisk==2){
      ipcB=1;
      ipcL=1; 
}
else if(emu_system_side =="p" && idisk==3){
      ipcB=1;
      ipcL=1; 
}
else if(emu_system_side =="p" && idisk==4){
      ipcB=1;
      ipcL=1; 
}

else if(emu_system_side =="m" && idisk==2){
      ipcB=1;
      ipcL=1; 
}
else if(emu_system_side =="m" && idisk==3){
      ipcB=1;
      ipcL=1; 
}
*/
//----------------------------------------------------------------------------

//---------------------------------------


/////if(ipcL < ipcB)return;
if(dynlen(disk_db)==0)return;

//---------------------------------------
dyn_string CscLevelDevices;
string parent_domain;
string Node_save;
string dpN;
int cu_flag;
dyn_string panels;
string parent_node;

//int w_pos,radius,ich;
//string type;

CSC_fwG_EmuCmsSpecialMode=1;
panels=makeDynString("fwFSMuser/fwUi.pnl");

int i;

if(custom_lv || !(isWholeCrbFolderAdd)){ // add folder only if custom_lv mode !!!!!!
//---------------------
mudcs_selectParent(2, -1, station_parent_node,"DCSNodes",1, parent_domain);

DebugTN("//////////////////////////  "+"CSC"+station_label+" "+emu_system_side+" "+idisk);


CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
if(isWholeCrbFolderAdd)mudcsNameCompose("LV_CRB", station_label, emu_system_side, idisk, "", "", EmuCmsGlobalNode);
else mudcsNameCompose("_LV_CRB", station_label, emu_system_side, idisk, "", "", EmuCmsGlobalNode);
 parent_node=EmuCmsGlobalNode;
//EmuCmsGlobalNode="CSC"+station_label+emu_system_side+idisk+"PC";
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addLogical(false, CSC_fwG_g_csc_part+"/"+station_parent_node, 
 EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuPCConfig", Component+"/emuPCOperation");
 
mudcs_addNode();

 dynRemove(panels,2);
 dynAppend(panels,Component+"/emuPCOperation.pnl");
// dpSet(mudcsAddSystem(EmuCmsGlobalNode+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);
}
 
 
//----------------------
if(custom_lv){
 for(i_db=1;i_db<=dynlen(disk_db);i_db++){

//  if((idisk_cross==4 || idisk_cross==5))i=substr(disk_db[i_db],strpos(disk_db[i_db],"_PC")+3,2);
  i=substr(disk_db[i_db],strpos(disk_db[i_db],"_PC")+3,strpos(disk_db[i_db],"_LV_CRB")-strpos(disk_db[i_db],"_PC")-3);
  
mudcs_selectParent(3, i-1, parent_node,"DCSNodes",1, parent_domain);

 CSC_fwG_EmuCmsGlobalType="CRB_1";
 mudcsNameCompose(CSC_fwG_EmuCmsGlobalType, station_label, emu_system_side, idisk, "", i, EmuCmsGlobalNode);
// EmuCmsGlobalNode= "CSC"+station_label+emu_system_side+idisk+"p"+i+"CRB_1"; 
 Node_save=EmuCmsGlobalNode;
 CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
 EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+station_parent_node+"/"+parent_node, 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevCRB_1Config", Component+"/emuDevCRB_1Operation");

 mudcs_addNode();

 dynRemove(panels,2); 
 dynAppend(panels,Component+"/emuDevCRB_1Operation.pnl"); 

dpN=fwFsm_getLogicalDeviceName(EmuCmsGlobalNode);
////////dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);
//--------------------


string coord_toappend;
/*
if(idisk_cross==2 && i == 1 ){
 coord_toappend=CSC_fwG_g_SYSTEM_NAME+":CRB/"+Node_save+" LVCB_1.ELMB_0F;0";
 if(dynContains(list,coord_toappend))
 dynAppend(list,coord_toappend);
}
else if(idisk_cross==3 && i == 1){
 coord_toappend=CSC_fwG_g_SYSTEM_NAME+":CRB/"+Node_save+" LVCB_1.ELMB_15;0";
 if(dynContains(list,coord_toappend))
 dynAppend(list,coord_toappend);
}
else if(idisk_cross==6 && i == 1){
 coord_toappend=CSC_fwG_g_SYSTEM_NAME+":CRB/"+Node_save+" LVCB_1.ELMB_3C;0";
 if(dynContains(list,coord_toappend))
 dynAppend(list,coord_toappend);
}
else if(idisk_cross==7 && i == 1){
 coord_toappend=CSC_fwG_g_SYSTEM_NAME+":CRB/"+Node_save+" LVCB_1.ELMB_3E;0";
 if(dynContains(list,coord_toappend))
 dynAppend(list,coord_toappend);
}
else if(idisk_cross==8 && i == 1){
 coord_toappend=CSC_fwG_g_SYSTEM_NAME+":CRB/"+Node_save+" LVCB_1.ELMB_3B;0";
 if(dynContains(list,coord_toappend))
 dynAppend(list,coord_toappend);
}
*/
//mudcsNameToPosConvert(Node_save, w_pos, radius, ich, type);
//retrieveCoordinateSet("CRB_1", Node_save, setList, w_pos, radius, ich, type, set);
// dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel"+w_pos+"."+setList),list);


//--------------------

 } // for over peripheral crates (crb-s)
} // if custom_lv
else{
//----------
for(i_db=1;i_db<=dynlen(disk_db);i_db++){ // actually -- only one item: the folder with all crb-s for the given disk id isWholeCrbFolderAdd=true !!!!

 if(isWholeCrbFolderAdd)mudcs_selectParent(-1, -1, station_parent_node,"",-1, parent_domain); // add right to the disk folder 
 else mudcs_selectParent(-1, -1, parent_node,"",-1, parent_domain);
   
if(isWholeCrbFolderAdd)CSC_fwG_EmuCmsGlobalType="CSC_LV_TREE_NODES"; ///
else CSC_fwG_EmuCmsGlobalType="fwCrb_CSC_LV";


DebugTN(parent_domain+" >>>> "+disk_db[i_db]);

//string EMU_SYSTEM_SIDE="P";
//if(emu_system_side=="n")EMU_SYSTEM_SIDE="N";
//EmuCmsGlobalNode=CSC_fwG_g_Dubna_System_Name+":CMS_CSC_LV_CAN1::"+"CSC_ME_"+EMU_SYSTEM_SIDE+idisk+"CR";
EmuCmsGlobalNode=disk_db[i_db];// 
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; 
CSC_fwG_EmuCmsGlobalParent=parent_domain;
mudcs_addNode();
}  // for
//----------   
  
}





// if((idisk_cross==4 || idisk_cross==5) && radius == 1){ // i.e. ME 1/1
//  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Ring_DUBNA"+"."+"DiskLevelDevicesCoordinates"),list);
// }
// else{
  ///////dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel"+idisk_cross+"."+"DiskLevelDevicesCoordinates"),list);
// }
}

//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
mudcsCreateALNM(int CreateLevel, int isAppendToFilledStation, int idisk, int idisk_cross, string emu_system_side, string parent_node)
{


//---------------------------------------
dyn_string CscLevelDevices;
string parent_domain;
string Node_save;
string dpN;
int cu_flag;
dyn_string panels;


//int w_pos,radius,ich;
//string type;
dyn_string list;



// if((idisk_cross==4 || idisk_cross==5) && radius == 1){ // i.e. ME 1/1
//  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Ring_DUBNA"+"."+"DiskLevelDevicesCoordinates"),list);
// }
// else{
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel"+idisk_cross+"."+"DiskLevelDevicesCoordinates"),list);
// }

CSC_fwG_EmuCmsSpecialMode=1;
panels=makeDynString("fwFSMuser/fwUi.pnl");

int i;

//---------------------
//----------------------


mudcs_selectParent(2, isAppendToFilledStation,  parent_node,"DCSNodes",1, parent_domain);

 CSC_fwG_EmuCmsGlobalType="ALNM_1";
 mudcsNameCompose(CSC_fwG_EmuCmsGlobalType, station_label, emu_system_side, idisk, "", "", EmuCmsGlobalNode);
// EmuCmsGlobalNode= "CSC"+station_label+emu_system_side+idisk+"ALNM_1";
 Node_save=EmuCmsGlobalNode;
 CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
 EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+parent_node, 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevALNM_1Config", Component+"/emuDevALNM_1Operation");

 mudcs_addNode();

 dynRemove(panels,2); 
 dynAppend(panels,Component+"/emuDevALNM_1Operation.pnl"); 

dpN=fwFsm_getLogicalDeviceName(EmuCmsGlobalNode);
////////dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);
//--------------------


////string string_coord=" me"+emu_system_side+idisk;
///string_coord=string_coord+";0";
////string coord_toappend=CSC_fwG_g_SYSTEM_NAME+":ALNM/"+Node_save+string_coord;
////if(!dynContains(list,coord_toappend))dynAppend(list,coord_toappend);

//--------------------


// if((idisk_cross==4 || idisk_cross==5) && radius == 1){ // i.e. ME 1/1
//  dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Ring_DUBNA"+"."+"DiskLevelDevicesCoordinates"),list);
// }
// else{
 //////// dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel"+idisk_cross+"."+"DiskLevelDevicesCoordinates"),list);
// }
}

//====================================================================
//====================================================================
//====================================================================
//====================================================================

mudcsCreateGas(int isGas, string parent_node_test)
{

//if(isGas) {

dyn_string CscLevelDevices;
string parent_domain;
string Node_save;
string dpN;
int cu_flag;
dyn_string panels;

CSC_fwG_EmuCmsSpecialMode=1;
panels=makeDynString("fwFSMuser/fwUi.pnl");


//================= DB ============================================================

 dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);

//==================================================================================


//------- add GAS_SX5 -----------

// Note:
//   Probably INSEAD OF to deal with setting panels for the fsm nodes in the 
//   mudcsCreateAllTrees():
//   dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);
//   it is possible to set that panel in two ways:
//      -- editor/fsm tab/ 
//               edit LV_1 type e.g by setting panel 
//     -- correct the _FwFsmObjectType/fwOT_LV_1.panel 
//

mudcs_selectParent(2, 0, parent_node_test,"DCSNodes",1, parent_domain);

 CSC_fwG_EmuCmsGlobalType="GAS_SX5";
 mudcsNameCompose(CSC_fwG_EmuCmsGlobalType, "", "", 0, "", "", EmuCmsGlobalNode);
// EmuCmsGlobalNode= "CSC"+"GAS_SX5"; 
 Node_save=EmuCmsGlobalNode;
 CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
 EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+parent_node_test,
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevGAS_SX5Config", Component+"/emuDevGAS_SX5Operation");

//mudcsDebug2(parent_node_test+">>"+CSC_fwG_EmuCmsGlobalParent);

 mudcs_addNode();
////fwFsmTree_addNode(parent_node_test,EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,cu_flag);

 dynRemove(panels,2); 
 dynAppend(panels,Component+"/emuDevGAS_SX5Operation.pnl"); 

dpN=fwFsm_getLogicalDeviceName(EmuCmsGlobalNode);
////////dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);
// dpSet(mudcsAddSystem(parent_domain+"|"+CSC_fwG_g_csc_part+"/"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);
//dpSet(mudcsAddSystem(parent_domain+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);

 //----array of coordinates of devices: cell_format: "device_name coordinates"------ 
//// if(dynContains(CscLevelDevices,EmuCmsGlobalNode+" LVCB_1.ELMB_3F;0") == 0  )
//// dynAppend(CscLevelDevices,EmuCmsGlobalNode+" LVCB_1.ELMB_3F;0"); 
 //---------------------------------------------------------------------------------

/////// dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);


//} // if(isGas)

}

//====================================================================
//====================================================================
//====================================================================
//====================================================================


mudcsCreateHardwareBranch(int isFilled, string subsystem, string &parent_node){


dyn_string CscLevelDevices;
string parent_domain;
string Node_save;
string dpN;
int cu_flag;
dyn_string panels;
//string parent_node;

mudcs_selectParent(1, isFilled, CSC_fwG_g_csc_part,"DCSNodes",1, parent_domain);

///////////domain = "DISK"+disk_numbers[i];
///////////type =CSC_fwG_g_NodeLogicalFsmType;
cu_flag = 1;
if(subsystem=="LV")CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeStbyFsmType;
else CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
//mudcsNameCompose("", station_label, emu_system_side, idisk, "", "", EmuCmsGlobalNode);
EmuCmsGlobalNode="CSC_"+subsystem;
 parent_node=EmuCmsGlobalNode;
CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1;
CSC_fwG_EmuCmsGlobalParent=parent_domain;


//mudcsDebug(EmuCmsGlobalNode);
//mudcsDebug(CSC_fwG_EmuCmsGlobalParent);

//////////if(CreateLevel <= 4){
 mudcs_addLogical(false, CSC_fwG_g_csc_part, EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emu"+subsystem+"SubSystemConfig", Component+"/emu"+subsystem+"SubSystemOperation");
 
mudcs_addNode();
///////fwFsmTree_addNode(CSC_fwG_EmuCmsGlobalParent,EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,cu_flag);

 dynRemove(panels,2);
 dynAppend(panels,Component+"/emu"+subsystem+"SubSystemOperation.pnl");
// dpSet(mudcsAddSystem(EmuCmsGlobalNode+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);


/////////////}

}
//====================================================================
mudcsSupervisorRole(){
  
string parent_domain=CSC_fwG_g_csc_part;
int i,j;
dyn_string disk_db, ds_split;
int cu_flag;
string fsm_type; 
string sys_name=getSystemName();

for(i=1;i<=dynlen(project_stations);i++){

disk_db=dpNames("*:CSC_ME_"+project_stations[i]+"|CSC_ME_"+project_stations[i],"_FwFsmObject"); //
//disk_db2=dpNames("*:CSC_LV|CSC_LV","_FwFsmObject"); //
//disk_db3=dpNames("*:CSC_HV|CSC_HV","_FwFsmObject"); //
//dynAppend(disk_db,disk_db2);
//dynAppend(disk_db,disk_db3);

 for(j=1;j<=dynlen(disk_db);j++){
   if(strpos(disk_db,sys_name)>=0)continue;
   dpGet(disk_db[j]+".type",fsm_type); 
   ds_split=strsplit(disk_db[j],"|");

EmuCmsGlobalNode=ds_split[1];// 
//EmuCmsGlobalNode=ds_split[1]+"::"+ds_split[dynlen(ds_split)];// THAT ALSO WORKS BUT CU FLAG SHOULD BE 0, OTHERWISE NO CHILD NODES

CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1; //  ??? poprobovat' po raznomu t.e. 1 i 0

CSC_fwG_EmuCmsGlobalParent=parent_domain; // 
CSC_fwG_EmuCmsGlobalType= fsm_type;//CSC_fwGSuper_g_TreeLogicalFsmType;

mudcs_addNode();
 } // for
 
} // for

  
}
//====================================================================
//====================================================================
//====================================================================
//====================================================================



//////////////int id;

mudcsCreateAllTrees(int CreateLevel)
{

 if(fwFsmTree_isNode(CSC_fwG_g_csc_part)>0){
   DebugTN("++++++++++ TREE ALREADY EXISTS ++++++++++++++++");
   return; 
 } 
  
    
custom_lv=CSC_fwG_g_CUSTOM_LV;
  
dyn_dyn_int sector_16;
  
sector_16[1]=makeDynString(2,3,4);
sector_16[2]=makeDynString(5,6,7);  
sector_16[3]=makeDynString(8,9,10);  
sector_16[4]=makeDynString(11,12,13);  
sector_16[5]=makeDynString(14,15,16);  
sector_16[6]=makeDynString(17,18,1);  

dyn_dyn_int sector_32;

sector_32[1]=makeDynString(3,4,5,6,7,8);  
sector_32[2]=makeDynString(9,10,11,12,13,14);  
sector_32[3]=makeDynString(15,16,17,18,19,20);  
sector_32[4]=makeDynString(21,22,23,24,25,26);  
sector_32[5]=makeDynString(27,28,29,30,31,32);  
sector_32[6]=makeDynString(33,34,35,36,1,2);  
  

dpSetWait(mudcsAddSystem("EMUNAMEALIASES."),makeDynString());

string test_string, test_string2;
int i,i10,i20,i30, iring, iringB, iringL, i100;
int idisk;
string parent_node;

	string domain, type;
	int cu_flag;
             string parent_domain, parent;
dyn_string panels;
string actionscript;
string dpN;
string Node_save;

dyn_string CscLevelDevices;
dyn_string DiskLevelDevices;

/////mudcsDebug2(actionscript);

mudcsInit(); // especially needed for addLogical

if(CreateLevel==0){
//------

// importing Dubna type !!!
///// tempor fwFsmTree_importType("CMS_CSC_ME11_HV_Type",CSC_fwG_g_Dubna_System_Name);


  dpCreate("vset_master_100_20_15_default","ExampleDP_Int");
  dpCreate("vset_primary_default","ExampleDP_Int");
  dpCreate("vset_all_default","ExampleDP_Int");

 dpCreate("HV_1_DimBroker_o","HV_1_d");
 dpCreate("HV_PR_DimBroker_o","HV_PR_d");
 
if(CSC_fwG_g_SYSTEM_NAME == "System1"){

  dpCreate("master500_1_1","HV_1_d");
  dpCreate("master500_1_15","HV_1_d");
}
else if(CSC_fwG_g_SYSTEM_NAME == CSC_fwG_CSC_fwG_g_SYSTEM_NAME2){



for(i=1;i<=dynlen(CSC_fwG_g_all_masters);i++){
dyn_string split_masters=strsplit(CSC_fwG_g_all_masters[i],":");
 dpCreate(split_masters[2],"HV_1_d");
}



//  dpCreate("master500_1_1","HV_1_d");
//  dpCreate("master500_1_15","HV_1_d");
// for(i=0;i<dynlen(CSC_fwG_g_all_2masters);i++){
//  test_string="master"+CSC_fwG_g_2host+"_0_"+i;
//  dpCreate(test_string,"HV_1_d");
// }

}
} // if(CreateLevel==0)


//------

//-----------------------------------------------------
 int isGas;

 if(CSC_fwG_g_SYSTEM_NAME=="System1"){
  isGas=1;
 }
 else if(CSC_fwG_g_SYSTEM_NAME==CSC_fwG_CSC_fwG_g_SYSTEM_NAME2){
  isGas=1;
 }
//-----------------------------------------------------


/////------------------------------------------------------
/////mudcs_selectParent(1, 1, "CMS","MainNode",1, parent_domain);
/////return;
/////------------------------------------------------------------------


  
panels=makeDynString("fwFSMuser/fwUi.pnl");
CSC_fwG_EmuCmsSpecialMode=1;

EmuCmsGlobalNode=CSC_fwG_g_csc_part;
CSC_fwG_EmuCmsGlobalType= CSC_fwG_g_MainLogicalFsmType;//"TEST_TYPE";
CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1;
CSC_fwG_EmuCmsGlobalParent="FSM";

if(CreateLevel==0){

 mudcs_addLogical(false, "", EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuEmuConfig", Component+"/emuEmuOperation");

 mudcs_addNode();

 dynRemove(panels,2);
 dynAppend(panels,Component+"/emuEmuOperation.pnl");
// dpSet(mudcsAddSystem(EmuCmsGlobalNode+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);


}

/*

//------------------------------------------------------

/////////mudcs_selectParent(1, 0, "CSC","MaiNode",1, parent_domain);
//------------------------------------------------------------------
/////////EmuCmsGlobalNode="DT";
/////////CSC_fwG_EmuCmsGlobalType="DCSNodes";//"TEST_CH";
/////////CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1;
/////////CSC_fwG_EmuCmsGlobalParent="CMS";
/////////
/////////if(CreateLevel == 0){
///////// mudcs_addNode();
/////////
///////// dynRemove(panels,2);
///////// dynAppend(panels,"test.pnl");
///////// dpSet(mudcsAddSystem(EmuCmsGlobalNode+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);
/////////
///////// mudcs_addLogical(false, "CMS", EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
///////// "fwDevice/fwDeviceManage", "fwDevice/fwDeviceManage");
/////////}
//------------------------------------------------------
//------------------------------------------------------
/////////
/////////mudcs_selectParent(2, 0, "DT","DCSNodes",1, parent_domain);
/////////
/////////EmuCmsGlobalNode="testlv38";
/////////Node_save=EmuCmsGlobalNode;
/////////CSC_fwG_EmuCmsGlobalType="LV_1";
/////////CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
/////////CSC_fwG_EmuCmsGlobalParent=parent_domain;
/////////
/////////if(CreateLevel == 0){
///////// mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
///////// EmuCmsGlobalNode=dpN;
/////////
///////// mudcs_addNode();
/////////
///////// dynRemove(panels,2);
///////// dynAppend(panels,Component+"/emuDevLV_1Operation.pnl");
///////// dpSet(mudcsAddSystem(parent_domain+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);
/////////
///////// mudcs_addLogical(true, "CMS/DT", Node_save, CSC_fwG_EmuCmsGlobalType,
///////// Component+"/"+dir_config+"/emuDevLV_1Config", Component+"/emuDevLV_1Operation");
/////////}
/////////
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
/////////
/////////mudcs_selectParent(1, 1, "CMS","MainNode",1, parent_domain);  // corre 00
/////////
//------------------------------------------------------
/////////
/////////EmuCmsGlobalNode="CSC";
/////////CSC_fwG_EmuCmsGlobalType="DCSNodes";//"TEST_CH";
/////////CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1;
/////////CSC_fwG_EmuCmsGlobalParent="CMS";
/////////
/////////if(CreateLevel <= 1){
///////// mudcs_addNode();
/////////
///////// dynRemove(panels,2);
///////// dynAppend(panels,Component+"/emuEmuOperation.pnl");
///////// dpSet(mudcsAddSystem(EmuCmsGlobalNode+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);
/////////
///////// mudcs_addLogical(false, "CMS", EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
///////// Component+"/"+dir_config+"/emuEmuConfig", Component+"/emuEmuOperation");
/////////}


*/

///return;

//------------------------------------------------------
//----------------------------------------------------
// ----------------------------------------------------------------------------

//-----------------------------------------------------
dyn_int disk_numbers;
dynClear(disk_numbers);

for(i=1;i<=dynlen(CSC_fwG_g_idisk_cross_numbers);i++){
  dynAppend(disk_numbers,CSC_fwG_g_idisk_cross_numbers[i]);
}
  
//------------------------------------------------------

string parent_node_test;
if(CreateLevel <= 1){
 if(dynContains(disk_numbers,CSC_fwG_g_StationWithFsmHwTree)){  // i.e. csc level devices (sm Hw tree) are only for that station
/////  mudcsCreateHardwareBranch(0, "GAS", parent_node_test);
/////  mudcsCreateGas(0,parent_node_test);
/////////////return;
/////////////////////////mudcsCreatePT100(parent_node_test);
mudcsCreateHardwareBranch(1, "LV", parent_node_test);
mudcsCreateMRTN(-1,-1,"",parent_node_test);
mudcsCreateMRTN(-2,-2,"",parent_node_test);

mudcsCreateWNR_SNMP(parent_node_test);
mudcsCreateAtlasPsu(parent_node_test);

mudcsCreateHardwareBranch(1, "HV", parent_node_test);
mudcsCreateHV_PR(parent_node_test);
mudcsSupervisorRole();
//return;
////  mudcsCreateHardwareBranch(1, "WTH", parent_node_test);
////  mudcsCreateWTH_SX5(parent_node_test);
 } // if(dynContains(disk_numbers,3))
}

//-----------------------------------------------------

if(CreateLevel <= 2){


/// mudcs_addLogical(true, CSC_fwG_g_csc_part, 
/// Node_save, CSC_fwG_EmuCmsGlobalType,
/// Component+"/"+dir_config+"/emuDevGAS_SX5Config", Component+"/emuDevGAS_SX5Operation");

 // -- bla-bla-bla for adding other csc level devices --
 //
/// dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o"+".CscLevelDevicesCoordinates"), CscLevelDevices);


//mudcsCreateGas(0);
//mudcsCreatePT100();
//mudcsCreateHV_PR();
//mudcsCreateWTH_SX5();
//mudcsCreateMRTN();
/////mudcsCreateWNR12();

} // if(CreateLevel <= 2)

//  dynAppend(disk_numbers,5);
//  dynAppend(disk_numbers,6);
//  dynAppend(disk_numbers,7);
//  dynAppend(disk_numbers,8);


string emu_system_side;
//-----------------------------------------------------
 int first_station, last_station;

 if(CSC_fwG_g_SYSTEM_NAME=="System1"){
 first_station=1; // 1
 last_station=3;  // 3
 }
 else if(CSC_fwG_g_SYSTEM_NAME==CSC_fwG_CSC_fwG_g_SYSTEM_NAME2){
 first_station=1; // 1
 last_station=dynlen(disk_numbers);  // 4
 }

//-----------------------------------------------------
for (i=first_station;i<=last_station;i++){ // ------- add disks ----------

dynClear(DiskLevelDevices);

if(disk_numbers[i]<=4){idisk=5-disk_numbers[i];emu_system_side="m";}
else {idisk=disk_numbers[i]-4;emu_system_side="p";}
// ATTENTION !!!!: i-1+1 : =1 is because of GAS_SX5 before
mudcs_selectParent(1, isGas, CSC_fwG_g_csc_part,"DCSNodes",1, parent_domain);
isGas=1;

domain = "DISK"+disk_numbers[i];
type =CSC_fwG_g_NodeLogicalFsmType;
cu_flag = 1;
CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
mudcsNameCompose("", station_label, emu_system_side, idisk, "", "", EmuCmsGlobalNode);
 parent_node=EmuCmsGlobalNode;
//EmuCmsGlobalNode="CSC"+station_label+emu_system_side+idisk;
CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1;
CSC_fwG_EmuCmsGlobalParent=parent_domain;


//mudcsDebug(EmuCmsGlobalNode);
//mudcsDebug(CSC_fwG_EmuCmsGlobalParent);

if(CreateLevel <= 4){
 mudcs_addLogical(false, CSC_fwG_g_csc_part, EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDiskConfig", Component+"/emuDiskOperation");
 
mudcs_addNode();

 dynRemove(panels,2);
 dynAppend(panels,Component+"/emuDiskOperation.pnl");
// dpSet(mudcsAddSystem(EmuCmsGlobalNode+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);


}

//TTTTTTT template for device on the disk level TTTTTTTTT 
// ---here is some addition hardware device ----
//
//----array of coordinates of devices: cell_format: "device_name coordinates"------ 
// template: dynAppend(DiskLevelDevices,EmuCmsGlobalNode); 
//---------------------------------------------------------------------------------
//
// -- here is some addition logical ------------
//
// -- bla-bla-bla for adding other disk level devices --
//
// dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":Db_o.Wheels_o.Wheel"+i+".DiskLevelDevicesCoordinates"), DiskLevelDevices);
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//      iringB=2; // default
//      iringL=-2; // default


      iringB=1; // default
      iringL=2; // default
      
      
if(disk_numbers[i]==4 || disk_numbers[i]==5){
      iringB=1;
      iringL=3; 
}
      
/*      
if(disk_numbers[i]==2){
      iringB=1;
      iringL=2; 
}
if(disk_numbers[i]==3){
      iringB=1;
      iringL=2; 
}



if(disk_numbers[i]==6){
      iringB=1;
      iringL=2; 
}
if(disk_numbers[i]==7){
      iringB=1;
      iringL=2; 
}
if(disk_numbers[i]==8){
      iringB=1;
      iringL=1; 
}
*/

//-------------------------

string c0;
dyn_int ch_numbers;
dyn_int ch_devices_number;
int i_db;
dyn_string hv_db;
//dynClear(ch_numbers);

//-----------------------------------------------------
 int isAppendToFilledStation;
//-----------------------------------------------------
if(CreateLevel == 50 || CreateLevel == 0){ // 50 is to create only PCs
 if(CreateLevel == 50)isAppendToFilledStation=1;
 else isAppendToFilledStation=0;

  mudcsCreatePC(idisk, disk_numbers[i], emu_system_side, parent_node);
  mudcsCreateMRTN(idisk, disk_numbers[i], emu_system_side, parent_node);
 
}
//-----------------------------------------------------
///////////if(CreateLevel == 51 || CreateLevel == 0){ // 51 is to create only ALNM devices
/////////// isAppendToFilledStation=1;
/////////// mudcsCreateALNM(CreateLevel, isAppendToFilledStation, idisk, disk_numbers[i], emu_system_side, parent_node);
///////////}
//-----------------------------------------------------
string station_parent_node=parent_node;
//-----------------------------------------------------
for(iring=iringB;iring<=iringL;iring++){

dynClear(ch_numbers);
dynClear(ch_devices_number);

///int ch_number_on_ring;
//// int ichB;

if(CSC_fwG_g_SYSTEM_NAME=="System1"){
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
}
else if(CSC_fwG_g_SYSTEM_NAME==CSC_fwG_CSC_fwG_g_SYSTEM_NAME2){
//-----------------------------------------------------------------------------------------------------------
  
if(disk_numbers[i]==4 || disk_numbers[i]==5){
 if(iring == 2){
   dpGet(mudcsAddSystem("Db_o.Wheels_o.Wheel"+disk_numbers[i]+".InHVsetList"),hv_db);
   if(CreateLevel == 0){
     for(i_db=1;i_db<=dynlen(hv_db);i_db++){
       if(hv_db[i_db]!=""){dynAppend(ch_numbers,i_db); dynAppend(ch_devices_number,3);} // ch_devices_number-not used! 
     }
   }
 }
 else if(iring == 3){
   dpGet(mudcsAddSystem("Db_o.Wheels_o.Wheel"+disk_numbers[i]+".OutHVsetList"),hv_db);
   if(CreateLevel == 0){
     for(i_db=1;i_db<=dynlen(hv_db);i_db++){
       if(hv_db[i_db]!=""){dynAppend(ch_numbers,i_db); dynAppend(ch_devices_number,3);}// ch_devices_number-not used!
     }
   }    
 }
 else if(iring == 1){
   dpGet(mudcsAddSystem("Db_o.Wheels_o.Ring_DUBNA.HVsetList"),hv_db);
   if(CreateLevel == 0){
     int i_db_1;
     for(i_db=1;i_db<=dynlen(hv_db);i_db++){
       if(hv_db[i_db]!="" &&  disk_numbers[i]==4 && i_db <= 36){dynAppend(ch_numbers,i_db); dynAppend(ch_devices_number,2);} // ch_devices_number-not used!
       else if(hv_db[i_db]!="" &&  disk_numbers[i]==5 && i_db > 36){i_db_1=i_db-36;dynAppend(ch_numbers,i_db_1); dynAppend(ch_devices_number,2);} // ch_devices_number-not used!
     }
   }   
 }
} // if(disk_numbers[i]==4 || disk_numbers[i]==5)
else{ // i.e. disk not ME-1 or ME+1
 if(iring == 1){
   dpGet(mudcsAddSystem("Db_o.Wheels_o.Wheel"+disk_numbers[i]+".InHVsetList"),hv_db);
   if(CreateLevel == 0){
     for(i_db=1;i_db<=dynlen(hv_db);i_db++){
       if(hv_db[i_db]!=""){dynAppend(ch_numbers,i_db); dynAppend(ch_devices_number,3);} // ch_devices_number-not used!
     }
   }
 }
 else if(iring == 2){
   dpGet(mudcsAddSystem("Db_o.Wheels_o.Wheel"+disk_numbers[i]+".OutHVsetList"),hv_db);
   if(CreateLevel == 0){
     for(i_db=1;i_db<=dynlen(hv_db);i_db++){
       if(hv_db[i_db]!=""){dynAppend(ch_numbers,i_db); dynAppend(ch_devices_number,3);}// ch_devices_number-not used!
     }
   }    
 }  
}// else  i.e. disk not ME-1 or ME+1
  
  


//--------------------------------------
//----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
}

string disk_rad;
////// ch_number_on_ring=1;
disk_rad="r"+iring;
////////string station_parent_node=parent_node;

int i_sec;
int i_ch;
int found_sec;
string s_sec;
string sector_parent_node;

for(i_sec=1;i_sec <=6;i_sec++){ // SECTORS=SECTORS=SECTORS=SECTORS=SECTORS=SECTORS=SECTORS=SECTORS=SECTORS=SECTORS=SECTORS=SECTORS

  found_sec=0;;

  s_sec=i_sec;
  
  if(disk_numbers[i] == 4|| disk_numbers[i] == 5){ // ME+1/ME-1 stations
    for(i_ch=1;i_ch<=dynlen(ch_numbers);i_ch++){
     if(dynContains(sector_32[i_sec],ch_numbers[i_ch])>=1 )found_sec=1;
    }
  }  // if 
  else{
    if(iring==1){
     for(i_ch=1;i_ch<=dynlen(ch_numbers);i_ch++){
      if(dynContains(sector_16[i_sec],ch_numbers[i_ch])>=1 )found_sec=1;
     }           
    } // if
    else{
     for(i_ch=1;i_ch<=dynlen(ch_numbers);i_ch++){
      if(dynContains(sector_32[i_sec],ch_numbers[i_ch])>=1 )found_sec=1;
     }       
    } // else
  } // else 
  
if(!found_sec)continue;  

mudcs_selectParent(2, i10-1+(iring-iringB), station_parent_node,"DCSNodes",1, parent_domain);

CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
mudcsNameCompose("", station_label, emu_system_side, idisk, s_sec, "", EmuCmsGlobalNode);
 sector_parent_node=EmuCmsGlobalNode;
//EmuCmsGlobalNode="CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100; //r2
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

 mudcs_addLogical(false, CSC_fwG_g_csc_part+"/"+station_parent_node, 
 EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuSectorConfig", Component+"/emuSectorOperation");
 
mudcs_addNode();

 // continue;
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
 
  
  
for (i10=1;i10<=dynlen(ch_numbers);i10++){

  
  found_sec=0;
  
  if(disk_numbers[i] == 4|| disk_numbers[i] == 5){ // ME+1/ME-1 stations
     if(dynContains(sector_32[i_sec],ch_numbers[i10])>=1 )found_sec=1;
  }  // if 
  else{
    if(iring==1){
      if(dynContains(sector_16[i_sec],ch_numbers[i10])>=1 )found_sec=1;
    } // if
    else{
      if(dynContains(sector_32[i_sec],ch_numbers[i10])>=1 )found_sec=1;
    } // else
  } // else 
  
if(!found_sec)continue;  
  
  
  
  
mudcs_selectParent(2, i10-1+(iring-iringB), sector_parent_node,"DCSNodes",1, parent_domain);

i100=ch_numbers[i10];
if(i100<=9)c0="c0";
else c0="c";

CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
mudcsNameCompose("", station_label, emu_system_side, idisk, disk_rad, c0+i100, EmuCmsGlobalNode);
 parent_node=EmuCmsGlobalNode;
//EmuCmsGlobalNode="CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100; //r2
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

if(CreateLevel <= 5){
 mudcs_addLogical(false, CSC_fwG_g_csc_part+"/"+station_parent_node+"/"+sector_parent_node, 
 EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuChamberConfig", Component+"/emuChamberOperation");
 
mudcs_addNode();

///// dynRemove(panels,2);
////// dynAppend(panels,Component+"/emuChamberOperation.pnl");
///////// Oshibka : no more CU dpSet(mudcsAddSystem(EmuCmsGlobalNode+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);


}

// ----------------------------------------------------------------------------
for(i20=1;i20<=3;i20++){ // NUMBER OF DEVICES

///////////EmuCmsGlobalNode="CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100; //r2
//////////CSC_fwG_EmuCmsGlobalType=CSC_fwG_g_NodeLogicalFsmType;
//////////CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1;
///////////CSC_fwG_EmuCmsGlobalParent=parent_domain;

mudcs_selectParent(3, i20-1, parent_node,"DCSNodes",1, parent_domain); //r2
//------------------------------------------------------------------------------

if(i20==1){

CSC_fwG_EmuCmsGlobalType="LV_1";
mudcsNameCompose(CSC_fwG_EmuCmsGlobalType, station_label, emu_system_side, idisk, disk_rad, c0+i100, EmuCmsGlobalNode);
//EmuCmsGlobalNode=domain = "CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100+"LV_1"; // r2
Node_save=EmuCmsGlobalNode;
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
CSC_fwG_EmuCmsGlobalParent=parent_domain;

if(CreateLevel <= 6){
 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+station_parent_node+"/"+sector_parent_node+"/"+parent_node, 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevLV_1Config", Component+"/emuDevLV_1Operation");

 mudcs_addNode();

 dynRemove(panels,2);
 dynAppend(panels,Component+"/emuDevLV_1Operation.pnl");

dpN=fwFsm_getLogicalDeviceName(EmuCmsGlobalNode);
////////dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);
// dpSet(mudcsAddSystem(parent_domain+"|"+CSC_fwG_g_csc_part+"/"+"CSC"+station_label+emu_system_side+idisk+"/"+"CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100+"/"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);
// dpSet(mudcsAddSystem(parent_domain+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);

/// mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+"CSC"+station_label+emu_system_side+idisk+"/"+"CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100, 
/// Node_save, CSC_fwG_EmuCmsGlobalType,
/// Component+"/"+dir_config+"/emuDevLV_1Config", Component+"/emuDevLV_1Operation");
}

//getActionScript(actionscript);
///////////////getActionScriptFromFile(actionscript);
///////////////fwFsm_writeDeviceActionScript("DemoDevice4",actionscript);

}
/*

if(i20==2){

CSC_fwG_EmuCmsGlobalType="CHIP_1";
mudcsNameCompose(CSC_fwG_EmuCmsGlobalType, station_label, emu_system_side, idisk, disk_rad, c0+i100, EmuCmsGlobalNode);
//EmuCmsGlobalNode=domain = "CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100+"CHIP_1"; //r2
Node_save=EmuCmsGlobalNode;
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
CSC_fwG_EmuCmsGlobalParent=parent_domain;

if(CreateLevel <= 6){
 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+station_parent_node+"/"+sector_parent_node+"/"+parent_node, 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevCHIP_1Config", Component+"/emuDevCHIP_1Operation");

 mudcs_addNode();

 dynRemove(panels,2); 
 dynAppend(panels,Component+"/emuDevCHIP_1Operation.pnl"); 

dpN=fwFsm_getLogicalDeviceName(EmuCmsGlobalNode);
////////dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);
// dpSet(mudcsAddSystem(parent_domain+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);
// dpSet(mudcsAddSystem(parent_domain+"|"+CSC_fwG_g_csc_part+"/"+"CSC"+station_label+emu_system_side+idisk+"/"+"CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100+"/"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);

/// mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+"CSC"+station_label+emu_system_side+idisk+"/"+"CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100, 
/// Node_save, CSC_fwG_EmuCmsGlobalType,
/// Component+"/"+dir_config+"/emuDevCHIP_1Config", Component+"/emuDevCHIP_1Operation");
}

}
*/


if(i20==2){


CSC_fwG_EmuCmsGlobalType="TEMP_1";
mudcsNameCompose(CSC_fwG_EmuCmsGlobalType, station_label, emu_system_side, idisk, disk_rad, c0+i100, EmuCmsGlobalNode);
//EmuCmsGlobalNode=domain = "CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100+"TEMP_1"; //r2
Node_save=EmuCmsGlobalNode;
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
CSC_fwG_EmuCmsGlobalParent=parent_domain;

if(CreateLevel <= 6){
 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+station_parent_node+"/"+sector_parent_node+"/"+parent_node, 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevTEMP_1Config", Component+"/emuDevTEMP_1Operation");

 mudcs_addNode();

 dynRemove(panels,2); 
 dynAppend(panels,Component+"/emuDevTEMP_1Operation.pnl"); 

dpN=fwFsm_getLogicalDeviceName(EmuCmsGlobalNode);
////////dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);
// dpSet(mudcsAddSystem(parent_domain+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);
// dpSet(mudcsAddSystem(parent_domain+"|"+CSC_fwG_g_csc_part+"/"+"CSC"+station_label+emu_system_side+idisk+"/"+"CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100+"/"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);

/// mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+"CSC"+station_label+emu_system_side+idisk+"/"+"CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100, 
/// Node_save, CSC_fwG_EmuCmsGlobalType,
/// Component+"/"+dir_config+"/emuDevTEMP_1Config", Component+"/emuDevTEMP_1Operation");
}

}

if(i20==3){
if((disk_numbers[i] != 4 && disk_numbers[i] != 5) || (iring != 1)) {


CSC_fwG_EmuCmsGlobalType="HV_1";
mudcsNameCompose(CSC_fwG_EmuCmsGlobalType, station_label, emu_system_side, idisk, disk_rad, c0+i100, EmuCmsGlobalNode);
//EmuCmsGlobalNode=domain = "CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100+"HV_1"; //r2
Node_save=EmuCmsGlobalNode;
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
CSC_fwG_EmuCmsGlobalParent=parent_domain;

if(CreateLevel <= 6){
 mudcs_addHardwareDevice(EmuCmsGlobalNode,CSC_fwG_EmuCmsGlobalType,"",dpN);
EmuCmsGlobalNode=dpN;

 mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+station_parent_node+"/"+sector_parent_node+"/"+parent_node, 
 Node_save, CSC_fwG_EmuCmsGlobalType,
 Component+"/"+dir_config+"/emuDevHV_1Config", Component+"/emuDevHV_1Operation");

 mudcs_addNode();

 dynRemove(panels,2); 
 dynAppend(panels,Component+"/emuDevHV_1Operation.pnl"); 

dpN=fwFsm_getLogicalDeviceName(EmuCmsGlobalNode);
////////dpSet(mudcsAddSystem(parent_domain+"|"+dpN+".ui.panels:_original.._value"),panels);
// dpSet(mudcsAddSystem(parent_domain+"|"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);
// dpSet(mudcsAddSystem(parent_domain+"|"+CSC_fwG_g_csc_part+"/"+"CSC"+station_label+emu_system_side+idisk+"/"+"CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100+"/"+EmuCmsGlobalNode+".ui.panels:_original.._value"),panels);

/// mudcs_addLogical(true, CSC_fwG_g_csc_part+"/"+"CSC"+station_label+emu_system_side+idisk+"/"+"CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100, 
/// Node_save, CSC_fwG_EmuCmsGlobalType,
/// Component+"/"+dir_config+"/emuDevHV_1Config", Component+"/emuDevHV_1Operation");
}


} // 

else {// i.e. if((disk_numbers[i] == 4 || disk_numbers[i] == 5) && (iring == 1))

//----------
CSC_fwG_EmuCmsGlobalType="CMS_CSC_ME11_HV_Type";
////====mudcsNameCompose("", station_label, emu_system_side, idisk, disk_rad, c0+i100, EmuCmsGlobalNode);
//// ===== parent_node=EmuCmsGlobalNode;
//EmuCmsGlobalNode="CSC"+station_label+emu_system_side+idisk+disk_rad+c0+i100; //r2

string EMU_SYSTEM_SIDE="P";
if(emu_system_side=="m")EMU_SYSTEM_SIDE="N";
string C0_DUBNA;
if(i100<=9)C0_DUBNA="C0";
else C0_DUBNA="C";

//EmuCmsGlobalNode=CSC_fwG_g_Dubna_System_Name+":CMS_ME11P_HV::"+"ME11"+EMU_SYSTEM_SIDE+i100+"HV";
//EmuCmsGlobalNode=CSC_fwG_g_Dubna_System_Name+":CMS_CSC_ME11P_HV::"+"ME11"+EMU_SYSTEM_SIDE+i100+"HV";
EmuCmsGlobalNode=CSC_fwG_g_Dubna_System_Name+":CSC_ME_11_HV::"+"CSC_ME_"+EMU_SYSTEM_SIDE+"11_"+C0_DUBNA+i100+"_HV";

CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwG_EmuCmsGlobalParent=parent_domain;

//////==== mudcs_addLogical(false, CSC_fwG_g_csc_part+"/"+station_parent_node, 
//////==== EmuCmsGlobalNode, CSC_fwG_EmuCmsGlobalType,
/////==== Component+"/"+dir_config+"/emuChamberConfig", Component+"/emuChamberOperation");
 
 mudcs_addNode();

//----------
} // 
} // if(i20==4)

}
// ----------------------------------------------------------------------------
} // loop on chmbers
} // i_sec
} // for(iring=iringB;iring<=iringL;iring++)
// ----------------------------------------------------------------------------
} // loop on disks

mudcsPostCreateConfig();
fwFsmTree_generateAll();
DebugTN("+++++++++++++++++++++++++++++++++++++ FSM tree creation is done ++++++++++++++++++++++++++++++++++++++++");

/*

// ----------------------------------------------------------------------------
//------------------------------------------------------------------



mudcs_selectParent(1, 1, "CMS","MainNode",1, parent_domain);

EmuCmsGlobalNode="ECAL";
CSC_fwG_EmuCmsGlobalType="DCSNodes";//"TEST_CH";
CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1;
CSC_fwG_EmuCmsGlobalParent="CMS";

mudcs_addNode();
//------------------------------------------------------

mudcs_selectParent(2, 1, "ECAL","DCSNodes",1, parent_domain);

EmuCmsGlobalNode="testlv4";
CSC_fwG_EmuCmsGlobalType="LV_1";
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
CSC_fwG_EmuCmsGlobalParent=parent_domain;

mudcs_addNode();

// ----------------------------------------------------------------------------
//------------------------------------------------------------------
mudcs_selectParent(1, 1, "CMS","MainNode",1, parent_domain);

EmuCmsGlobalNode="HCAL";
CSC_fwG_EmuCmsGlobalType="DCSNodes";//"TEST_CH";
CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1;
CSC_fwG_EmuCmsGlobalParent="CMS";

mudcs_addNode();
//------------------------------------------------------

mudcs_selectParent(2, 1, "HCAL","DCSNodes",1, parent_domain);

EmuCmsGlobalNode="testlv5";
CSC_fwG_EmuCmsGlobalType="LV_1";
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
CSC_fwG_EmuCmsGlobalParent=parent_domain;

mudcs_addNode();

// ----------------------------------------------------------------------------
//------------------------------------------------------------------
mudcs_selectParent(1, 1, "CMS","MainNode",1, parent_domain);

EmuCmsGlobalNode="Tracker";
CSC_fwG_EmuCmsGlobalType="DCSNodes";//"TEST_CH";
CSC_fwG_EmuCmsGlobalCu="1";cu_flag = 1;
CSC_fwG_EmuCmsGlobalParent="CMS";

mudcs_addNode();
//------------------------------------------------------

mudcs_selectParent(2, 1, "Tracker","DCSNodes",1, parent_domain);

EmuCmsGlobalNode="testlv6";
CSC_fwG_EmuCmsGlobalType="LV_1";
CSC_fwG_EmuCmsGlobalCu="0";cu_flag = 0;
CSC_fwG_EmuCmsGlobalParent=parent_domain;

mudcs_addNode();

//------------------------------------------------------
//------------------------------------------------------

*/

//// crteation logical tree
////mudcs_logicalTreeCreation();

CSC_fwG_EmuCmsSpecialMode=0;
}

//=========================================================================
mudcs_selectParent(int level, int isTree, string domain,string type,int cu_flag, string &parent_domain){

if(mode_tree_creation){
 parent_domain=domain;
 return;
}

if(isTree >1)isTree=1;

if(domain==CSC_fwG_g_csc_part)type=CSC_fwG_g_MainLogicalFsmType;
else type=CSC_fwG_g_NodeLogicalFsmType;

onSelectSimulation(level,isTree,domain,type,cu_flag);
parent_domain=domain;

}

//========================================================================

//=======================================================================

mudcs_addHardwareDevice(string deviceName,string deviceType,string deviceComment, string &dpN)
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
  
///    dpCreate(deviceName,"fwGasSystem_CSC_GAS_d");       // !!!!!!!!!!!!!!!!!!!!!
 
  generalDeviceType = "GasCMS";
  deviceType = "GasCMS Device (1)";
}
else if(strpos(deviceType,"fwCooling_CSC_COOLING")>=0){
  
///    dpCreate(deviceName,"fwCooling_CSC_COOLING_d");       // !!!!!!!!!!!!!!!!!!!!!
 
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
mudcs_addLogical(bool isDevice,  string parent, string node, string deviceType,
string config_panel, string op_panel){

dyn_string exceptionInfo;
string generalDeviceType;

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
  	dpSetAlias(deviceObject[fwDevice_DP_NAME] + ".", dpAlias + fwDevice_HIERARCHY_SEPARATOR + deviceObject[fwDevice_ALIAS]);

        dynAppend(CSC_fwG_g_EMU_ALIASES, deviceObject[fwDevice_DP_NAME] + "."
             +"#"+
              dpAlias + fwDevice_HIERARCHY_SEPARATOR + deviceObject[fwDevice_ALIAS]);

  }


 } // else

}
//====================================================
//=============================================================================================================

createNode(string sDpName, string nodeNameText, string config_panel, string op_panel, dyn_string exceptionInfo)
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

//////mudcsDebug(name);
	
	if(dynlen(exceptionInfo) > 0)
		return;
	 

	
	dpSet(mudcsAddSystem(name + ".type"), type);
	dpSet(mudcsAddSystem(name + ".dpTypes"), makeDynString());
	dpSet(mudcsAddSystem(name + ".navigatorPanels"), makeDynString(op_panel));
	dpSet(mudcsAddSystem(name + ".editorPanels"), makeDynString(config_panel));
	
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
