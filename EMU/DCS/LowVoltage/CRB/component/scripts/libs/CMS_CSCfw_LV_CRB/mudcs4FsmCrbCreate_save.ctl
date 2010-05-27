mudcsCrbFsmCreateMain()
{
  int iStationCross, iStation;
 int cu_flag;  
  mudcsCrbPnlInit(0);

//================================================================  

  if(!dpExists("EMUALIASES.")){
   dpCreate("EMUALIASES","MUDCS_DYN_STRING");     
  }

   
//======== main node creation (CMS_CSC_LV_CAN1) ==================  

CSC_fwCAN1_g_EmuCmsGlobalNode=CSC_fwCAN1_g_csc_part;
CSC_fwCAN1_g_EmuCmsGlobalType= CSC_fwCAN1_g_MainLogicalFsmType;//"TEST_TYPE";
CSC_fwCAN1_g_EmuCmsGlobalCu="1";cu_flag = 1;
CSC_fwCAN1_g_EmuCmsGlobalParent="FSM";
 
 mudcsLv_addLogical(false, "", CSC_fwCAN1_g_EmuCmsGlobalNode, CSC_fwCAN1_g_EmuCmsGlobalType,
 "CMS_CSCfw_LV_CRB/config/emuEmptyConfig", "CMS_CSCfw_LV_CRB/emuEmptyOperation"); 
 mudcsLv_addNode();
//=====================================================================
//===========================================
 
 for(iStationCross=1;iStationCross<=8;iStationCross++){ 
  mudcsCrbFsmCreatePC(iStationCross, CSC_fwCAN1_g_csc_part);
 } // for iStationCross
  mudcsPsuFsmCreate(CSC_fwCAN1_g_csc_part);
  mudcsPsuPostConfig(); 
 //=====================================
 
 dpSet("EMUALIASES.", CSC_fwCAN1_g_EMU_ALIASES);
 
 
 fwFsmTree_generateAll(); 
 fwFsmTree_refreshTree();
  
 // mudcsLvDebug("Done");
  
 DebugTN("TREE IS DONE");
  DebugTN("TREE IS DONE");
   DebugTN("TREE IS DONE");
    DebugTN("TREE IS DONE");
     DebugTN("TREE IS DONE");
      DebugTN("TREE IS DONE");
       DebugTN("TREE IS DONE"); 
//====================================================================== 
}
//====================================================================
//mudcsCrbCreatePC(int CreateLevel, int isAppendToFilledStation, int idisk, int idisk_cross, 
//              string emu_system_side, string station_parent_node)
mudcsCrbFsmCreatePC(int iStationCross, string system_parent_node)
{
  int iStation;
string emu_system_side;  
dyn_string list, ds1, ds2, ds3 ,ds4, ds5;
int i,j;
string sTemplate, sTemplate2;
string dpN;
//---------------------------------------
string parent_domain;
string Node_save;
int cu_flag;
string parent_node, parent_node_2;

  dpGet(mudcsLvAddSystem("db_pcmb.list"),list);

//---------------------
parent_domain=system_parent_node;

CSC_fwCAN1_g_EmuCmsGlobalType=CSC_fwCAN1_g_NodeLogicalFsmType;
//mudcsCrbNameCompose("LV_CRB", station_label, emu_system_side, idisk, "", "", CSC_fwCAN1_g_EmuCmsGlobalNode);
mudcsCrbGetStationInforByCrossNumber(iStationCross, iStation, emu_system_side);
CSC_fwCAN1_g_EmuCmsGlobalNode="CSC_ME_"+emu_system_side+iStation+"_LV_CRB";

 parent_node=CSC_fwCAN1_g_EmuCmsGlobalNode;
//CSC_fwCAN1_g_EmuCmsGlobalNode="CSC"+station_label+emu_system_side+idisk+"PC";
CSC_fwCAN1_g_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwCAN1_g_EmuCmsGlobalParent=parent_domain;

 mudcsLv_addLogical(false, CSC_fwCAN1_g_csc_part, 
 CSC_fwCAN1_g_EmuCmsGlobalNode, CSC_fwCAN1_g_EmuCmsGlobalType,
 "CMS_CSCfw_LV_CRB/config/emuEmptyConfig", "CMS_CSCfw_LV_CRB/emuEmptyOperation"); 
 
mudcsLv_addNode();

DebugTN(CSC_fwCAN1_g_EmuCmsGlobalNode+" "+CSC_fwCAN1_g_EmuCmsGlobalParent);

//----------------------
parent_domain=parent_node;  

sTemplate=emu_system_side+iStation+"_";


 for(i=1;i<=dynlen(list);i++){
//DebugTN("====="+list[i]+" "+sTemplate);

  if(strpos(list[i],sTemplate) < 0)continue; 
  ds1=strsplit(list[i],";"); 
  if(dynlen(ds1) < 2 || ds1[2] == "")continue;
  
 CSC_fwCAN1_g_EmuCmsGlobalType="fwCrb_CSC_LV";//CSC_fwCAN1_g_NodeLogicalFsmType;
 CSC_fwCAN1_g_EmuCmsGlobalNode="CSC_ME_"+ds1[1]+"_LV_CRB";

 parent_node_2=CSC_fwCAN1_g_EmuCmsGlobalNode;
// mudcsCrbNameCompose(CSC_fwCAN1_g_EmuCmsGlobalType, station_label, emu_system_side, idisk, "", i, CSC_fwCAN1_g_EmuCmsGlobalNode);
// CSC_fwCAN1_g_EmuCmsGlobalNode= "CSC"+station_label+emu_system_side+idisk+"p"+i+"CRB_1"; 

 CSC_fwCAN1_g_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwCAN1_g_EmuCmsGlobalParent=parent_domain;

 mudcsLv_addHardwareDevice(CSC_fwCAN1_g_EmuCmsGlobalNode,CSC_fwCAN1_g_EmuCmsGlobalType,"",dpN);
 CSC_fwCAN1_g_EmuCmsGlobalNode=dpN;
 
 mudcsLv_addLogical(true, system_parent_node+"/"+parent_domain, 
 CSC_fwCAN1_g_EmuCmsGlobalNode, CSC_fwCAN1_g_EmuCmsGlobalType,
 "CMS_CSCfw_LV_CRB/config/emuDevfwCrb_CSC_LVConfig", "CMS_CSCfw_LV_CRB/emuDevfwCrb_CSC_LVOperation");

DebugTN("======================================"); 
DebugTN(CSC_fwCAN1_g_EmuCmsGlobalType); 
DebugTN(CSC_fwCAN1_g_EmuCmsGlobalNode); 
DebugTN(CSC_fwCAN1_g_EmuCmsGlobalParent); 
DebugTN(""); 
DebugTN("========================================"); 

 mudcsLv_addNode();

DebugTN("====="+CSC_fwCAN1_g_EmuCmsGlobalNode+" "+CSC_fwCAN1_g_EmuCmsGlobalParent);
  

continue; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//===========ADD DEVICES=============
 
 ds2=dpNames("*ELMB_"+ds1[2]+"*", "FwElmbAi");

//----------------------

 parent_domain=parent_node_2; 
 
 for(j=1;j<=dynlen(ds2);j++){

 ds4=strsplit(ds2[j],"/");
 sTemplate2=ds4[dynlen(ds4)];
       
 CSC_fwCAN1_g_EmuCmsGlobalType="FwElmbAi";
// mudcsCrbNameCompose(CSC_fwCAN1_g_EmuCmsGlobalType, station_label, emu_system_side, idisk, "", i, CSC_fwCAN1_g_EmuCmsGlobalNode);
// CSC_fwCAN1_g_EmuCmsGlobalNode= "CSC"+station_label+emu_system_side+idisk+"p"+i+"CRB_1"; 
 Node_save=CSC_fwCAN1_g_EmuCmsGlobalNode;
 CSC_fwCAN1_g_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwCAN1_g_EmuCmsGlobalParent=parent_domain;

 CSC_fwCAN1_g_EmuCmsGlobalNode=ds2[j];

 mudcsLv_addLogical(true, system_parent_node+"/"+parent_node+"/"+parent_node_2, 
 /*Node_save*/ds2[j], CSC_fwCAN1_g_EmuCmsGlobalType,
 "emu_config/emuDevCRB_1Config", "emu/emuDevCRB_1Operation");

 mudcsLv_addNode();
} // for j
//-------------------

 ds2=dpNames("*ELMB_"+ds1[2]+"*", "FwElmbDo");
    
 for(j=1;j<=dynlen(ds2);j++){

 ds4=strsplit(ds2[j],"/");
 sTemplate2=ds4[dynlen(ds4)];
       
 CSC_fwCAN1_g_EmuCmsGlobalType="FwElmbDo";
// mudcsCrbNameCompose(CSC_fwCAN1_g_EmuCmsGlobalType, station_label, emu_system_side, idisk, "", i, CSC_fwCAN1_g_EmuCmsGlobalNode);
// CSC_fwCAN1_g_EmuCmsGlobalNode= "CSC"+station_label+emu_system_side+idisk+"p"+i+"CRB_1"; 
 Node_save=CSC_fwCAN1_g_EmuCmsGlobalNode;
 CSC_fwCAN1_g_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwCAN1_g_EmuCmsGlobalParent=parent_domain;

 CSC_fwCAN1_g_EmuCmsGlobalNode=ds2[j];

 mudcsLv_addLogical(true, system_parent_node+"/"+parent_node+"/"+parent_node_2, 
 /*Node_save*/ds2[j], CSC_fwCAN1_g_EmuCmsGlobalType,
 "emu_config/emuDevCRB_1Config", "emu/emuDevCRB_1Operation");

 mudcsLv_addNode();
} // for j
  
 
//===================================
 } // for i
//--------------------


}

//====================================================================
mudcsCrbGetStationInforByCrossNumber(int iStationCross, int &iStation, string &emu_system_side){

if(iStationCross<=4){iStation=5-iStationCross;emu_system_side="M";}
else {iStation=iStationCross-4;emu_system_side="P";}

}

//=================================================================================================

//====================================================================
mudcsPsuFsmCreate(string system_parent_node)
{
  int iStation;
string emu_system_side;  
dyn_string list, ds1, ds2, ds3 ,ds4, ds5;
int i,j;
string sTemplate, sTemplate2;
string dpN;
//---------------------------------------
string parent_domain;
string Node_save;
int cu_flag;
string parent_node, parent_node_2;

//  dpGet(mudcsLvAddSystem("db_mrtn.list"),list);
  dpGet(mudcsLvAddSystem("db_pcmb.list"),list);
  
//---------------------
parent_domain=system_parent_node;

CSC_fwCAN1_g_EmuCmsGlobalType=CSC_fwCAN1_g_NodeLogicalFsmType;
//mudcsMrtnNameCompose("LV_CRB", station_label, emu_system_side, idisk, "", "", CSC_fwCAN1_g_EmuCmsGlobalNode);
CSC_fwCAN1_g_EmuCmsGlobalNode="CSC_ME_LV_PSU";

 parent_node=CSC_fwCAN1_g_EmuCmsGlobalNode;
//CSC_fwCAN1_g_EmuCmsGlobalNode="CSC"+station_label+emu_system_side+idisk+"PC";
CSC_fwCAN1_g_EmuCmsGlobalCu="0";cu_flag = 0; // logical unit
CSC_fwCAN1_g_EmuCmsGlobalParent=parent_domain;

 mudcsLv_addLogical(false, CSC_fwCAN1_g_csc_part, 
 CSC_fwCAN1_g_EmuCmsGlobalNode, CSC_fwCAN1_g_EmuCmsGlobalType,
 "CMS_CSCfw_LV_CRB/config/emuEmptyConfig", "CMS_CSCfw_LV_CRB/emuEmptyOperation");
 
mudcsLv_addNode();

DebugTN(CSC_fwCAN1_g_EmuCmsGlobalNode+" "+CSC_fwCAN1_g_EmuCmsGlobalParent);

//----------------------
parent_domain=parent_node;  

sTemplate="PSU";

 for(i=1;i<=dynlen(list);i++){
//DebugTN("====="+list[i]+" "+sTemplate);

  if(strpos(list[i],sTemplate) < 0)continue; 
  ds1=strsplit(list[i],";"); 
  if(dynlen(ds1) < 2 || ds1[2] == "")continue;
  
 CSC_fwCAN1_g_EmuCmsGlobalType=CSC_fwCAN1_g_NodeLogicalFsmType;
 CSC_fwCAN1_g_EmuCmsGlobalNode="CSC_ME_LV_"+ds1[1];

 parent_node_2=CSC_fwCAN1_g_EmuCmsGlobalNode;
// mudcsMrtnNameCompose(CSC_fwCAN1_g_EmuCmsGlobalType, station_label, emu_system_side, idisk, "", i, CSC_fwCAN1_g_EmuCmsGlobalNode);
// CSC_fwCAN1_g_EmuCmsGlobalNode= "CSC"+station_label+emu_system_side+idisk+"p"+i+"CRB_1"; 

 CSC_fwCAN1_g_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwCAN1_g_EmuCmsGlobalParent=parent_domain;

 //mudcsLv_addHardwareDevice(CSC_fwCAN1_g_EmuCmsGlobalNode,CSC_fwCAN1_g_EmuCmsGlobalType,"",dpN);
 //CSC_fwCAN1_g_EmuCmsGlobalNode=dpN;
 
 mudcsLv_addLogical(false, system_parent_node+"/"+parent_domain, 
 CSC_fwCAN1_g_EmuCmsGlobalNode, CSC_fwCAN1_g_EmuCmsGlobalType,
 "CMS_CSCfw_LV_CRB/config/emuEmptyConfig", "CMS_CSCfw_LV_CRB/emuEmptyOperation");

 mudcsLv_addNode();

DebugTN("====="+CSC_fwCAN1_g_EmuCmsGlobalNode+" "+CSC_fwCAN1_g_EmuCmsGlobalParent);
  

//continue; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//===========ADD DEVICES=============
 
 ds2=dpNames("*Crate"+ds1[2], "FwElmBPSUCrate");

//----------------------

 parent_domain=parent_node_2; 
 /*
 for(j=1;j<=dynlen(ds2);j++){

 ds4=strsplit(ds2[j],"/");
 sTemplate2=ds4[dynlen(ds4)];
       
 CSC_fwCAN1_g_EmuCmsGlobalType="FwElmbPSUCrate";
// mudcsMrtnNameCompose(CSC_fwCAN1_g_EmuCmsGlobalType, station_label, emu_system_side, idisk, "", i, CSC_fwCAN1_g_EmuCmsGlobalNode);
// CSC_fwCAN1_g_EmuCmsGlobalNode= "CSC"+station_label+emu_system_side+idisk+"p"+i+"CRB_1"; 
 Node_save=CSC_fwCAN1_g_EmuCmsGlobalNode;
 CSC_fwCAN1_g_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwCAN1_g_EmuCmsGlobalParent=parent_domain;

 CSC_fwCAN1_g_EmuCmsGlobalNode=ds2[j];

 mudcsLv_addLogical(true, system_parent_node+"/"+parent_node+"/"+parent_node_2, 
 ds2[j], CSC_fwCAN1_g_EmuCmsGlobalType,
 "CMS_CSCfw_LV_CRB/config/emuDevMRTNConfig", "fwWiener/fwWienerMarathonOperation.pnl");

 mudcsLv_addNode();
} // for j
 */
//-------------------
 
 ds2=dpNames("*Crate"+ds1[2]+"/Branch*", "FwElmbPSUBranch");
    
 for(j=1;j<=dynlen(ds2);j++){

 ds4=strsplit(ds2[j],"/");
 sTemplate2=ds4[dynlen(ds4)];
       
 CSC_fwCAN1_g_EmuCmsGlobalType="FwElmbPSUBranch";
// mudcsMrtnNameCompose(CSC_fwCAN1_g_EmuCmsGlobalType, station_label, emu_system_side, idisk, "", i, CSC_fwCAN1_g_EmuCmsGlobalNode);
// CSC_fwCAN1_g_EmuCmsGlobalNode= "CSC"+station_label+emu_system_side+idisk+"p"+i+"CRB_1"; 
 Node_save=CSC_fwCAN1_g_EmuCmsGlobalNode;
 CSC_fwCAN1_g_EmuCmsGlobalCu="0";cu_flag = 0;
 CSC_fwCAN1_g_EmuCmsGlobalParent=parent_domain;

 CSC_fwCAN1_g_EmuCmsGlobalNode=ds2[j];

 mudcsLv_addLogical(true, system_parent_node+"/"+parent_node+"/"+parent_node_2, 
 /*Node_save*/ds2[j], CSC_fwCAN1_g_EmuCmsGlobalType,
 "CMS_CSCfw_LV_CRB/config/emuEmptyConfig", "fwElmbPSU/fwElmbPSUBranchDisplay");

 mudcsLv_addNode();
} // for j
  
 
//===================================
 } // for i
//--------------------


}

//====================================================================
mudcsPsuPostConfig()
{
dyn_string ds_psu=dpNames("*","FwElmbPSUBranch");
string sPowerDpe;
int i;

 for(i=1;i<dynlen(ds_psu);i++){
  dpGet(ds_psu[i] + ".power", sPowerDpe);
  
  dpSet(sPowerDpe, EPSU_POWER_ON_VALUE);
  DebugTN(sPowerDpe);
 }
  
}
//=================================================================================================
