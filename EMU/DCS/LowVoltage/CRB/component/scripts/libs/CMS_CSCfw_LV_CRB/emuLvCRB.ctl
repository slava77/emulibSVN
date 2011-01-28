#uses "fwInstallation.ctl"
#uses "fwElmb/fwElmbUser.ctl"
#uses "fwElmb/fwElmbConstants.ctl"
#uses "fwDevice/fwDevice.ctl"
#uses "fwElmbPSU/fwElmbPSU.ctl"
#uses "fwElmbPSU/fwElmbPSUConstants.ctl"
#uses "CMS_CSCfw_LV_CRB/emuLvCRB_Constants.ctl"
#uses "fwFsmTreeDisplay.ctl"
#uses "fwConfigs/fwAlertConfig.ctl"
#uses "fwRDBArchiving/fwRDBConfig.ctl"

// patched

//----------------------------------------------------------------------------------
/**@file

This library contains LV_CRB functions for EMU DCS LV project.

@author Xiaofeng Yang (PH/UCM)
@date   Aug 2010
*/
      const bool      bDebug                = false; //debug on, else off
      const bool      bP5Project            = true; //for P5,else for 904      
//---------------------------------------------------------------------------------
/**
 * initialize generate parameters CRB,like system name where the CRB is installed
 * global variable gSystemNameCRB: the system name where CRB is installed
 */
void emuLvCRB_initalizeParam()
{
  //get system name where CRB component installation
  // dyn_string ds;
  // fwInstallation_getApplicationSystem("CMS_CSCfw_LV_CRB",ds);    
   addGlobal("gSystemNameCRB", STRING_VAR);
  // gSystemNameCRB = ds[1]; //with ":"
   gSystemNameCRB = "csc_dcs_lv1:";
}
/**
 * simple debug information on/off
 * bDebug=1, show debug information, else hide debug information
 */
void emuLvCRB_showDebug(bool bDebug, anytype aMessage)
{
  if (bDebug)
  {
    DebugTN(aMessage);
  }
  else
  return;  
} 
/**
 * config archiving db 
 */
void emuLvCRB_setDbConfig() 
{
  fwRDBConfig_setUser("cms_csc_pvss_cond");
  fwRDBConfig_setHost("cms_pvss_cond");  
}
/**
 * delete all dps,node,FSM tree,FSM scripts for CRB
 */
void emuLvCRB_deleteAll()
{
  DebugTN("delete all for CRB, waiting...");
  emuLvCRB_initalizeParam();
  //delete all dps for CRB and PSU
  dyn_string dsDpTypes,dsDpNames;
  int i,j;
  dsDpTypes = emuLvCRB_getDynString("DpType_Delete");  
  emuLvCRB_showDebug(bDebug,"dps are deleted in dpTypes:"+dsDpTypes);
  for(i=1;i<=dynlen(dsDpTypes);i++)
  {
    dsDpNames = dpNames("*",dsDpTypes[i]);
      if (dynlen(dsDpNames)>0)
       {  
          for(j=1;j<=dynlen(dsDpNames);j++)
          {
             dpDelete(dsDpNames[j]);
          }
       }
    if(strpos(dsDpTypes[i],"CSC")>0 || strpos(dsDpTypes[i],"PCMB")>0)
          {
            dpTypeDelete(dsDpTypes[i]);
            emuLvCRB_showDebug(bDebug,dsDpTypes[i]+" is deleted");
          } 
  }   
  
  //delete FSM tree
  fwFsmTree_removeNode("FSM","CMS_CSC_LV_CAN1",1);
  
  //remove dps from _FwTreeNode
  dyn_string dsSearchTypes = makeDynString("*CRB*","*CAN1*","*PSU*");
  for (i=1;i<=dynlen(dsSearchTypes);i++)
  {
    dsDpNames = dpNames(dsSearchTypes[i],"_FwTreeNode");
    for(j=1;j<=dynlen(dsDpNames);j++)
       {
         dpDelete(dsDpNames[j]);
       }
  }
  
  //remove dps from FwNode
   dsDpNames = dpNames("*CAN1*","FwNode");
    for(j=1;j<=dynlen(dsDpNames);j++)
       {
         dpDelete(dsDpNames[j]);
       }
    
  //clean dp EMUALIASES in dptype EMUDCS_DYN_STRING 
  if(dpExists(gSystemNameCRB+"EMUALIASES"))
    { 
     dpSet(gSystemNameCRB+"EMUALIASES.",makeDynString());
    }
  
  //delete all FSM install scripts
  string sPath;
  dyn_string dsConfPath,dsFileNames;
  int i,j; 
  fwInstallation_getProjPaths(dsConfPath);
  sPath = dsConfPath[1]+"/scripts/libs";
  emuLvCRB_showDebug(bDebug, sPath);
  dsFileNames = getFileNames(sPath,"*CAN1$install");
  emuLvCRB_showDebug(bDebug,dsFileNames); 
  if ( dynlen(dsFileNames)>0)
  {  
     for (int i=1;i<=dynlen(dsFileNames);i++)
      {  
       int j = remove(sPath+"/"+dsFileNames[i]);
        emuLvCRB_showDebug(bDebug,j);
      } 
   }
  delay(1);
  DebugTN("done for deleting"); 
}                            
/**
 * create dptype:
 *   -- "Db_PCMB" and datapoints
 *   -- fwCrb_CSC_LV and datapoints
 *   -- fwPsu_CSC_LV and datapoints
 *   -- fwBranch_CSC_LV and datapoints
 */
void emuLvCRB_createLvType()
{  
  DebugTN("create dptypes and dps for CRB/PSU");
  emuLvCRB_createDpType("Db_PCMB"); //create type Db_PCMB  
  dpCreate("db_pcmb","Db_PCMB");
  dpCreate("pcmb_mrtn","Db_PCMB");
  dpCreate("db_pcmb_geographic","Db_PCMB");  
  dyn_string dsDb,dsDbGeog,dsPcmbMrtn;
  if(bP5Project)
   {
    dsDb = emuLvCRB_getDynString("Pcrate_Elmb_P5");
    dsDbGeog = emuLvCRB_getDynString("Pcrate_Geog_P5");
    dsPcmbMrtn = emuLvCRB_getDynString("Pcmb_Mrtn_P5");                              
   }
  else
   { 
    dsDb = emuLvCRB_getDynString("Pcrate_Elmb_B904");
    dsDbGeog = emuLvCRB_getDynString("Pcrate_Geog_B904");
    dsPcmbMrtn = emuLvCRB_getDynString("Pcmb_Mrtn_B904");
   }
  dpSetWait("db_pcmb.list",dsDb,
            "db_pcmb_geographic.list",dsDbGeog,
            "pcmb_mrtn.list",dsPcmbMrtn);
  emuLvCRB_showDebug(bDebug,"Db set"); 
  
  emuLvCRB_createDpType("fwCrb_CSC_LV");  //create type fwCrb_CSC_LV
  dpCreate("fwCrb_CSC_LVInfo","_FwDeviceDefinition");
  dpSetWait("fwCrb_CSC_LVInfo.type","CRB Device (2)",
            "fwCrb_CSC_LVInfo.dpType","fwCrb_CSC_LV");
  
  dpCreate("CRB","FwNode");
  dpSetWait("CRB.type","VENDOR",
            "CRB.dpTypes",makeDynString("fwCrb_CSC_LV"),
            "CRB.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"),
            "CRB.editorPanels",makeDynString("fwDevice/fwDeviceManage"),
            "CRB.model","CRB",
            "CRB.fsmState",0,
            "CRB.fsmCommand",0);
  string sDeviceType = "fwCrb_CSC_LV";
  dpSetWait(sDeviceType+"Info.panels.editor.hardware",   makeDynString("CMS_CSCfw_LV_CRB/emuDev"+sDeviceType+"Hardware"), 
            sDeviceType+"Info.panels.editor.logical",    makeDynString("CMS_CSCfw_LV_CRB/emuDev"+sDeviceType+"Logical"),
            sDeviceType+"Info.panels.navigator.hardware",makeDynString("CMS_CSCfw_LV_CRB/emuDev"+sDeviceType+"Hardware"),
            sDeviceType+"Info.panels.navigator.logical", makeDynString("CMS_CSCfw_LV_CRB/emuDev"+sDeviceType+"Logical"));  
  
  dpSetWait("FwElmbPSUBranchInfo.panels.navigator.logical",makeDynString("fwElmbPSU/fwElmbPSUbranchDisplay"));
  
  emuLvCRB_createDpType("fwPsu_CSC_LV");  //create type fwPsu_CSC_LV
  
  emuLvCRB_createDpType("fwBranch_CSC_LV");  //create type fwBranch_CSC_LV
  dpCreate("fwBranch_CSC_LVInfo","_FwDeviceDefinition");
  dpSetWait("fwBranch_CSC_LVInfo.type","EMU Branch Device",
            "fwBranch_CSC_LVInfo.dpType","fwBranch_CSC_LV");
  
  dpCreate("CPSU","FwNode");
  dpSetWait("CPSU.type","VENDOR",
            "CPSU.dpTypes",makeDynString("fwBranch_CSC_LV"),
            "CPSU.navigatorPanels",makeDynString("fwDevice/fwDeviceManage"),
            "CPSU.editorPanels",makeDynString("fwDevice/fwDeviceManage"),
            "CPSU.model","CPSU",
            "CPSU.fsmState",0,
            "CPSU.fsmCommand",0);
  
  if(!dpExists("EMUALIASES"))
   {
     dpCreate("EMUALIASES","MUDCS_DYN_STRING");    
   }
  DebugTN("done for creating CRB/PSU dptypes and dps ");
} 

/**
 * create dps for can-bus,elmb ai,do for CRB,
 * need make stop/start sim-driver 7 before running it
 */
emuLvCRB_createElmbCRB()
{
  string   sxE,sCard;
  dyn_string dsExceptionInfo,dsCanBusNames;
  dyn_int diAiChannels;
  dyn_dyn_int ddiElmbIDs;
  int iBus, iE, iA, iD;
  sCard = ELMB_CAN_CARD_KVASER;
  if(bP5Project)
   {  
      dsCanBusNames = emuLvCRB_getDynString("CanBus_Name_P5");
      ddiElmbIDs = emuLvCRB_getDynDynInt("ElmbID_P5");
   }
  else
   {
      dsCanBusNames = emuLvCRB_getDynString("CanBus_Name_B904");
      ddiElmbIDs = emuLvCRB_getDynDynInt("ElmbID_B904");
   } 
  diAiChannels = emuLvCRB_getDynInt("ElmbAi");
  DebugTN("create dps for can bus,elmb node,ai,do,waiting...");
  for(iBus=1;iBus<=dynlen(dsCanBusNames);iBus++)
   {
    fwElmbUser_createCANbus(dsCanBusNames[iBus],"",(iBus-1),sCard,125000,true,dsExceptionInfo); //create can bus
      for(iE=1;iE<=dynlen(ddiElmbIDs[iBus]);iE++)      
       {
          sprintf(sxE,"%02x",ddiElmbIDs[iBus][iE]);         
          fwElmbUser_createElmb("ELMB_"+sxE,"",gSystemNameCRB+"ELMB/"+dsCanBusNames[iBus],ddiElmbIDs[iBus][iE],true,dsExceptionInfo); //create ELMB
       
          for(iA=1;iA<=dynlen(diAiChannels);iA++)
           { 
            fwElmbUser_createSensor("ai_"+diAiChannels[iA], dsCanBusNames[iBus],"ELMB_"+sxE,"", makeDynString(diAiChannels[iA]),"Raw ADC Value",makeDynString(),true,dsExceptionInfo);	//create ai  
           }
          for(iD=0;iD<=7;iD++)
           {
            fwElmbUser_createDigital(dsCanBusNames[iBus],"ELMB_"+sxE,"","C",iD,false,true,dsExceptionInfo); //create DO for Port C         
           }
         for(iD=0;iD<=4;iD++)
           {
            fwElmbUser_createDigital(dsCanBusNames[iBus],"ELMB_"+sxE,"","A",iD,false,true,dsExceptionInfo); //create DO for Port A         
           }            
        }
    }
   delay(1);
   DebugTN("done for CRB elmb creating");
} 
/**
 * create dps for crate,elmb_3f,branch for PSU,start sim-driver 7 before running it
 */
void emuLvCRB_createElmbPSU()
{
  dyn_string ds1,dsExceptionInfo;
  string s1,sxBr,sCanBusName;
  int i,iBranches,iBr;
  if(bP5Project)
   { 
    sCanBusName = "LVCB_0";
    iBranches = 4;
   } 
  else
   { 
    sCanBusName = "LVCB_1";
    iBranches = 1;
   }
  DebugTN("create PSU,waiting...");
  //create elmb_3f for PSU control
  fwElmbUser_createElmb("ELMB_3f","",gSystemNameCRB+"ELMB/"+sCanBusName,63,true,dsExceptionInfo);
  //create Crate_00 for PSU
  fwElmbPSU_createCrate("EPSU/Crate00",gSystemNameCRB+"ELMB/"+sCanBusName+"/ELMB_3f","",dsExceptionInfo); 
  //create branch for PSU
  for(iBr=0;iBr<iBranches;iBr++)
   {
    sprintf(sxBr,"%02x",iBr);
    fwElmbPSU_createBranch("EPSU/Crate00/Branch"+sxBr,iBr,gSystemNameCRB+"ELMB/"+sCanBusName,gSystemNameCRB+"EPSU/Crate00","","",dsExceptionInfo);
   }
  delay(1); 
  //set .bus for PSU branch
  ds1=dpNames("*Branch*","FwElmbPSUBranch"); 
  for(i=1;i<=dynlen(ds1);i++)
   {   
      dpSetWait(ds1[i]+".bus",gSystemNameCRB+"ELMB/"+sCanBusName);
   }      
  DebugTN("done for PSU elmb creating");  
}  
/**
 * create dp type for CRB
 * @param: name of dp type
 */
void emuLvCRB_createDpType(string sDpType)
{
  dyn_dyn_string ddsdepes;
  dyn_dyn_int    ddidepei;  
   if(sDpType=="Db_PCMB")
    {
      ddsdepes[1] = makeDynString ("Db_PCMB","");
      ddsdepes[2] = makeDynString ("","list");
      
      ddidepei[1] = makeDynInt (DPEL_STRUCT);
      ddidepei[2] = makeDynInt (0,DPEL_DYN_STRING);
    }
   else if(sDpType=="fwCrb_CSC_LV")
    {
      ddsdepes[1] = makeDynString ("fwCrb_CSC_LV","");
      ddsdepes[2] = makeDynString ("","coord"); // ELMB_ID to get ai_ dps to use for aler configuration etc
      ddsdepes[3] = makeDynString ("","switch_list"); // list of cout dps to use in status handler
      ddsdepes[4] = makeDynString ("","status");
      ddsdepes[5] = makeDynString ("","command");
      ddsdepes[6] = makeDynString ("","off_channels");
      ddsdepes[7] = makeDynString ("","error_status"); // combination of wrong state: state not 5 or 133--> (1) and timeout--> (10)

      ddidepei[1]= makeDynInt (DPEL_STRUCT);
      ddidepei[2]= makeDynInt (0,DPEL_STRING);
      ddidepei[3]= makeDynInt (0,DPEL_DYN_STRING);
      ddidepei[4]= makeDynInt (0,DPEL_INT);
      ddidepei[5]= makeDynInt (0,DPEL_INT);
      ddidepei[6]= makeDynInt (0,DPEL_DYN_INT);
      ddidepei[7]= makeDynInt (0,DPEL_INT);
    }
   else if(sDpType=="fwPsu_CSC_LV")
    {
      ddsdepes[1] = makeDynString ("fwPsu_CSC_LV","");
      ddsdepes[2] = makeDynString ("","coord"); // ELMB_ID to get ai_ dps to use for aler configuration etc
      ddsdepes[3] = makeDynString ("","switch_list"); // list of cout dps to use in status handler
      ddsdepes[4] = makeDynString ("","status");
      ddsdepes[5] = makeDynString ("","off_channels");
      ddsdepes[6] = makeDynString ("","error_status"); // combination of wrong state: state not 5 or 133--> (1) and timeout--> (10)
      ddsdepes[7] = makeDynString ("","bit_0");
      ddsdepes[8] = makeDynString ("","bit_1");
      ddsdepes[9] = makeDynString ("","bit_2");
      ddsdepes[10]= makeDynString ("","bit_3");

      ddidepei[1] = makeDynInt (DPEL_STRUCT);
      ddidepei[2] = makeDynInt (0,DPEL_STRING);
      ddidepei[3] = makeDynInt (0,DPEL_DYN_STRING);
      ddidepei[4] = makeDynInt (0,DPEL_INT);
      ddidepei[5] = makeDynInt (0,DPEL_DYN_INT);
      ddidepei[6] = makeDynInt (0,DPEL_INT);
      ddidepei[7] = makeDynInt (0,DPEL_BOOL);
      ddidepei[8] = makeDynInt (0,DPEL_BOOL);
      ddidepei[9] = makeDynInt (0,DPEL_BOOL);
      ddidepei[10]= makeDynInt (0,DPEL_BOOL);
    }
   else if(sDpType=="fwBranch_CSC_LV")
    {
      ddsdepes[1] = makeDynString ("fwBranch_CSC_LV","");
      ddsdepes[2] = makeDynString ("","coord");       // branch ID
      ddsdepes[3] = makeDynString ("","elmb_id");     //elmb for PSU
      ddsdepes[4] = makeDynString ("","switch_bit");  // port;bit
      ddsdepes[5] = makeDynString ("","status");      //on/off/error in FSM
      ddsdepes[6] = makeDynString ("","command");     //on/off       in FSM
      
      ddidepei[1] = makeDynInt (DPEL_STRUCT);
      ddidepei[2] = makeDynInt (0,DPEL_STRING);
      ddidepei[3] = makeDynInt (0,DPEL_STRING);
      ddidepei[4] = makeDynInt (0,DPEL_STRING);
      ddidepei[5] = makeDynInt (0,DPEL_INT);
      ddidepei[6] = makeDynInt (0,DPEL_INT);      
    }
  if(dynlen(dpTypes("*"+sDpType+"*")))
    { 
      DebugTN("DpType "+sDpType+" already exist");
    }
  else
    {  
      dpTypeCreate(ddsdepes,ddidepei);   
      DebugTN("DpType "+sDpType+" is created"); 
    } 
}  
/**
 * create Tree for CRB
 */
void emuLvCRB_createCrbTree()
{
  dyn_string dsCRBStationNodes,dsCRBDeviceNodes;
  int i,j; 
  //create top node for logical and FSM tree 
  DebugTN("create tree for CRB, waiting...");
  emuLvCRB_createLogicalNode("","CMS_CSC_LV_CAN1","",""); 
  emuLvCRB_createFsmNode("FSM","CMS_CSC_LV_CAN1",TopFsmObjType,1); 
  //create station node
  dsCRBStationNodes = emuLvCRB_getDynString("CRB_Station_Nodes");
  for (i=1;i<=dynlen(dsCRBStationNodes);i++)
   {  
    emuLvCRB_createLogicalNode("CMS_CSC_LV_CAN1",dsCRBStationNodes[i],"",""); 
    emuLvCRB_createFsmNode("CMS_CSC_LV_CAN1",dsCRBStationNodes[i],StationFsmObjType,0);
    dsCRBDeviceNodes = emuLvCRB_getDynString(dsCRBStationNodes[i]);
    for(j=1;j<=dynlen(dsCRBDeviceNodes);j++)
     {      
      emuLvCRB_createLogicalNode("CMS_CSC_LV_CAN1/"+dsCRBStationNodes[i],dsCRBDeviceNodes[j],"", 
                                 "CMS_CSCfw_LV_CRB/emuDevfwCrb_CSC_LVLogical");
      emuLvCRB_createFsmNode(dsCRBStationNodes[i],"CRB/"+dsCRBDeviceNodes[j],CrbFsmDevType,0);
     }    
   } 
  DebugTN("done for CRB tree");
}  
/**
 * create Tree for PSU
 */
void emuLvCRB_createPsuTree()
{
  dyn_string dsPSUBranchNodes,dsPSUDeviceNodes;
  int i,j; 
  //create PSU crate node under CMS_CSC_LV_CAN1
  if(!fwFsmTree_isNode("CMS_CSC_LV_CAN1"))
   {
    DebugTN("missing top node CMS_CSC_LV_CAN1 ");
   } 
  else
   {  
    //create PSU branch node  
    DebugTN("create tree for PSU, waiting..."); 
    emuLvCRB_createLogicalNode("CMS_CSC_LV_CAN1","CSC_ME_LV_PSU","",""); 
    emuLvCRB_createFsmNode("CMS_CSC_LV_CAN1","CSC_ME_LV_PSU",StationFsmObjType,0);
    dsPSUBranchNodes = emuLvCRB_getDynString("PSU_Branch_Nodes"); 
    for (i=1;i<=dynlen(dsPSUBranchNodes);i++)
     {  
      emuLvCRB_createLogicalNode("CMS_CSC_LV_CAN1/CSC_ME_LV_PSU",dsPSUBranchNodes[i],"",""); 
      emuLvCRB_createFsmNode("CSC_ME_LV_PSU",dsPSUBranchNodes[i],StationFsmObjType,0);
      dsPSUDeviceNodes = emuLvCRB_getDynString(dsPSUBranchNodes[i]);
      for(j=1;j<=dynlen(dsPSUDeviceNodes);j++)
       { 
        // for new device type: fwBranch_CSC_LV
        dpSetAlias("CPSU/EPSU_Crate00_"+dsPSUDeviceNodes[j]+".",
                   "CMS_CSC_LV_CAN1/CSC_ME_LV_PSU/"+dsPSUBranchNodes[i]+"/"+dsPSUDeviceNodes[j]);
        emuLvCRB_createFsmNode(dsPSUBranchNodes[i],"CPSU/EPSU_Crate00_"+dsPSUDeviceNodes[j],PsuFsmDevType,0); 
       }    
     } 
   }
  DebugTN("done for PSU tree");
} 
/**
 * create logical node for CRB
 */

void emuLvCRB_createLogicalNode(string sParentNode,string sNode,string sConfigPanel,string sOpPanel)
{    
	string sName, sType, sAlias;
  dyn_string dsExceptionInfo;
	dyn_errClass deErr;		
						
	if (sParentNode == "")
	{
		sName = sNode;
		sAlias = sNode;
		sType = fwNode_TYPE_LOGICAL_ROOT;
	}
	else
	{
		sName = sParentNode + fwDevice_HIERARCHY_SEPARATOR + sNode;
		sAlias = dpGetAlias(sParentNode + ".") + fwDevice_HIERARCHY_SEPARATOR + sNode;
		sType = fwNode_TYPE_LOGICAL;	
	}
	fwDevice_create(makeDynString(sNode, "FwNode","",""),
					        makeDynString(sParentNode,""),dsExceptionInfo);			
  if(dynlen(dsExceptionInfo) > 0) {return;}
	
	dpSetWait(gSystemNameCRB + sName + ".type", sType,
	          gSystemNameCRB + sName + ".dpTypes", makeDynString(),
	          gSystemNameCRB + sName + ".navigatorPanels", makeDynString(sOpPanel),
	          gSystemNameCRB + sName + ".editorPanels", makeDynString(sConfigPanel));
          	
	dpSetAlias(gSystemNameCRB + sName + ".", sAlias);
 
 	//test whether there were errors
	deErr = getLastError(); 
 	if(dynlen(deErr) > 0)
 	{
 		dpDelete(sName);
 		fwException_raise(exceptionInfo,"ERROR","Could not create the logical node","");
		return;
 	}
}
/**
 * create FSM node for CRB
 */
void emuLvCRB_createFsmNode(string sParentNode,string sNode,string sType,int iCu)
{
  if(fwFsmTree_isNode(sNode))
  {
    DebugTN("FSM node exists already");
    return;
  }
  else 
  {
    fwFsmTree_addNode(sParentNode,sNode,sType,iCu);
    DebugTN("FSM node created");
  }
}  
/**
 * create hardware node for CRB
 */
void emuLvCRB_createHardwareNode(string sNode,string sType)
{ 
  string sGeneralType;
  dyn_string dsExceptionInfo;
  if (sType == "fwCrb_CSC_LV")
  { 
    sGeneralType = "CRB";
  }
  fwDevice_create(makeDynString(sNode, sType, "", ""), 
					makeDynString(sGeneralType, "", "", ""),dsExceptionInfo);      
}  
/**
 * create dp for fwCrb_CSC_LV from Db_PCMB:
 *                 -- set  crb .status to 0  (OFF state)
 *                 -- set  crb .command to 0 ( switch OFF)
 * create dp for fwPsu_CSC_LV
 * create dp for fwBranch_CSC_LV
 */
void emuLvCRB_createFwCrbPsu()
{
   DebugTN("create dps for CRB/PSU, waiting...");
   dyn_string dsList,ds1,ds2,ds3,ds4,ds5,ds6,ds7,ds8;
   string sPcrate,sElmbID,sPsus,sPsu;
   int i,j;
   dpGet(gSystemNameCRB+"db_pcmb.list",dsList);
   for(i=2;i<=dynlen(dsList);i++)
   {
     //get Pcrate name and position, elmbID from dp_pcmb.list for .coord
     ds1 = strsplit(dsList[i],";");
     sPcrate = "CRB/CSC_ME_"+ds1[1]+"_LV_CRB";
     sElmbID = ds1[2];
     //get digit output from elmbDo for switch_list
     ds2 = dpNames("*ELMB_"+sElmbID+"*","FwElmbDo");
     ds3 = makeDynString();
       for (j=1;j<=dynlen(ds2);j++)
        {
         dynAppend(ds3,ds2[j]);
        }        
     dpCreate(sPcrate,"fwCrb_CSC_LV");
     dpSetWait(sPcrate+".coord",sElmbID,
               sPcrate+".switch_list",ds3,
               sPcrate+".status",0,
               sPcrate+".command",0);
   } 
     //for fwPsu_CSC_LV
     emuLvCRB_showDebug(bDebug,"create dp at fwPsu_CSC_LV, waiting...");
     ds4 = strsplit(dsList[1],";");                 
     sPsus = dpNames("*ELMB_"+ds4[3],"FwElmbNode"); //with system name systemname:ELMB/LV...
     ds5 = strsplit(sPsus,":");                     
     sPsu = ds5[2];                                 //without system name, ELMB/LV...
     strreplace(sPsu,"/","_");                      // ELMB_LV...
     dpCreate(sPsu,"fwPsu_CSC_LV");
     dpSetWait(gSystemNameCRB+sPsu+".coord",sPsus);   //systemname:ELMB_LV..... 
     //for fwBranch_CSC_LV
     emuLvCRB_showDebug(bDebug,"create dp at fwBranch_CSC_LV, waiting...");
     ds6 = dpNames("*EPSU*","FwElmbPSUBranch");
     for (i=1;i<=dynlen(ds6);i++)
      {
        string sElmb,sElmbDo;
        dpGet(ds6[i]+".power",sElmbDo);
        sElmbID=substr(sElmbDo,0,strpos(sElmbDo,"/DO/"));
        string sBranchName = dpSubStr(ds6[i], DPSUB_DP);
        string sBranchNameNew = sBranchName;                     
        strreplace(sBranchNameNew,"/","_");  
        dpCreate("CPSU/"+sBranchNameNew,"fwBranch_CSC_LV");
        dpSetWait(gSystemNameCRB+"CPSU/"+sBranchNameNew+".coord",sBranchName,
                  gSystemNameCRB+"CPSU/"+sBranchNameNew+".switch_bit", "C;"+(i-1),
                  gSystemNameCRB+"CPSU/"+sBranchNameNew+".elmb_id",sElmbID,
                  gSystemNameCRB+"CPSU/"+sBranchNameNew+".command",2);            
      }  
     DebugTN("done for CRB/PSU dps crating");
}  
/**
 * set alert handle for Elmb Ai
 */ 
void emuLvCRB_setAlertElmbAi()
{
  dyn_string dsCanBus,dsAlertTexts,dsAlertClasses;
  dyn_float  dfLimits;
  dyn_int diAiChannels;
  dyn_dyn_int ddiElmbIDs;
  string sxID,sDpName;
  int i,j,k;
  
  if(bP5Project)
  { 
     dsCanBus = emuLvCRB_getDynString("CanBus_Name_P5");
     ddiElmbIDs = emuLvCRB_getDynDynInt("ElmbID_P5");
  }
  else
  { 
     dsCanBus = emuLvCRB_getDynString("CanBus_Name_B904");
     ddiElmbIDs = emuLvCRB_getDynDynInt("ElmbID_B904");
  }
  DebugTN("ElmbAi alert config...");
  for (i=1;i<=dynlen(dsCanBus);i++)
    {
       for(j=1;j<=dynlen(ddiElmbIDs[i]);j++)
        {
          sprintf(sxID,"%02x",ddiElmbIDs[i][j]);
          //for channel with 3.3V
          diAiChannels = emuLvCRB_getDynInt("ElmbAi_3.3V");
          for (k=1;k<=dynlen(diAiChannels);k++)
          {
            sDpName = "ELMB/"+dsCanBus[i]+"/ELMB_"+sxID+"/AI/ai_"+diAiChannels[k]+".value";
            dfLimits = makeDynFloat(2.9,3.7);
            dsAlertTexts = makeDynString("error");
            dsAlertClasses = makeDynString("_fwErrorNack.");
            emuLvCRB_configAlert(sDpName,dfLimits,dsAlertTexts,dsAlertClasses);
          }
          //for channel with 5V
          diAiChannels = emuLvCRB_getDynInt("ElmbAi_5V");
          for (k=1;k<=dynlen(diAiChannels);k++)
          {
            sDpName = "ELMB/"+dsCanBus[i]+"/ELMB_"+sxID+"/AI/ai_"+diAiChannels[k]+".value";
            dfLimits = makeDynFloat(4.5,5.5);
            dsAlertTexts = makeDynString("error");
            dsAlertClasses = makeDynString("_fwErrorNack.");
            emuLvCRB_configAlert(sDpName,dfLimits,dsAlertTexts,dsAlertClasses);
          }
          //for channel with 1.5V
          diAiChannels = emuLvCRB_getDynInt("ElmbAi_1.5V");
          for (k=1;k<=dynlen(diAiChannels);k++)
          {
            sDpName = "ELMB/"+dsCanBus[i]+"/ELMB_"+sxID+"/AI/ai_"+diAiChannels[k]+".value";
            dfLimits = makeDynFloat(1.2,1.8);
            dsAlertTexts = makeDynString("error");
            dsAlertClasses = makeDynString("_fwErrorNack.");
            emuLvCRB_configAlert(sDpName,dfLimits,dsAlertTexts,dsAlertClasses);
          }      
         
        }             
    }
   DebugTN("done for elmbai alert config");  
}
/**
 * set alert handle for Elmb node 
 */
void emuLvCRB_setAlertElmbNode()
{
  dyn_string dsElmbNames,dsAlertTexts,dsAlertClasses; 
  dyn_float  dfLimit1,dfLimit2;  
  string sDpName1,sDpName2;
  dsElmbNames = dpNames("*Elmb*","FwElmbNode");
  dsAlertTexts = makeDynString("error");
  dsAlertClasses = makeDynString("_fwErrorNack.");
  dfLimit1 = makeDynFloat(1);
  dfLimit2 = makeDynFloat(2047);
  DebugTN("set alert for elmb node...");
  for (int i=1;i<=dynlen(dsElmbNames);i++)
  {
    sDpName1 = dsElmbNames[i]+".emergency.emergencyErrorCode";
    emuLvCRB_configAlert(sDpName1,dfLimit1,dsAlertTexts,dsAlertClasses);
    sDpName2 = dsElmbNames[i]+".error";
    emuLvCRB_configAlert(sDpName2,dfLimit2,dsAlertTexts,dsAlertClasses);
  } 
  DebugTN("done for elmb node alert config"); 
}
/**
 * set alert handle for crb,include summary alert setup 
 * set alert handle active to false 
 */
void emuLvCRB_setAlertCrb()
{
  dyn_string dsCrbNames,dsAlertTexts,dsAlertClasses,dsDpLists,dsExceptionInfo; 
  dyn_float  dfLimits;  
  string sDpName,sCoord;
  dsCrbNames = dpNames("*CRB*","fwCrb_CSC_LV");
  dsAlertTexts = makeDynString("error");
  dsAlertClasses = makeDynString("_fwErrorNack.");
  dfLimits = makeDynFloat(1);
  DebugTN("set alert for CRB...");
  for (int i=1;i<=dynlen(dsCrbNames);i++)
  {
    //set alert for Crb error_status
    sDpName = dsCrbNames[i]+".error_status";
    emuLvCRB_configAlert(sDpName,dfLimits,dsAlertTexts,dsAlertClasses);
    //set summary alert for Crb
    dpGet(dsCrbNames[i]+".coord",sCoord);
    dsDpLists = dpNames("*ELMB_"+sCoord+"*.value","FwElmbAi");
    dynAppend(dsDpLists,sDpName);
    fwAlertConfig_createSummary(dsCrbNames[i]+".", makeDynString("PCrate OK","PCrate ERROR"),
                                dsDpLists,"", makeDynString(),"",dsExceptionInfo);  
    dpSetWait (dsCrbNames[i]+".:_alert_hdl.._active",false); 
  }
  DebugTN("done for CRB alert config");  
}
/**
 * set alert handle for PSU:
 *     -- set alert for ADV,CANV in FwElmbA1
 *     -- set alert for error status in fwPsu_CSC_LV
 *     -- set summary alert in FwElmbBranch and set active to true 
 */
void emuLvCRB_setAlertPsu()
{
  dyn_string dsADVs,dsCANVs,dsPsus,dsBranches,dsAlertTexts,dsAlertClasses,dsDpLists,dsExceptionInfo;
  dyn_float  dfLimits;
  string sDpName,sBranchCanV,sBranchAdV;
  int i,j,k;
  //modify alert class for ADV/CANV
  dsADVs = dpNames("*ADV_*","FwElmbAi");
  dsCANVs = dpNames("*CANV_*","FwElmbAi");
  dsPsus = dpNames("*ELMB*","fwPsu_CSC_LV");
  dsBranches = dpNames("*Branch*","FwElmbPSUBranch");
  DebugTN("set alert for Psu...");
  for (i=1;i<=dynlen(dsADVs);i++)
    {
      emuLvCRB_modifyAlertClassPsu(dsADVs[i]+".value");
      emuLvCRB_modifyAlertClassPsu(dsCANVs[i]+".value");
    } 
  //set alert for fwPsu_CSC_LV
  for (j=1;j<=dynlen(dsPsus);j++)
    {
     sDpName = dsPsus[j]+".error_status";
     dsAlertTexts = makeDynString("error");
     dsAlertClasses = makeDynString("_fwErrorNack.");
     dfLimits = makeDynFloat(1);  
     emuLvCRB_configAlert(sDpName,dfLimits,dsAlertTexts,dsAlertClasses);     
  //set summary alert for PSU Branches
      for(k=1;k<=dynlen(dsBranches);k++)
       { 
         dpGet(dsBranches[k]+".CAN.volts",sBranchCanV);
         dpGet(dsBranches[k]+".AD.volts",sBranchAdV);    
         dsDpLists = makeDynString(sBranchCanV,sBranchAdV,sDpName);
         fwAlertConfig_createSummary(dsBranches[k]+".",
                                     makeDynString("ELMB Power Supply OK","ELMB Power Supply ERROR"),
                                     dsDpLists,"", makeDynString(),"",dsExceptionInfo);  
         dpSetWait (dsBranches[k]+".:_alert_hdl.._active",true);          
       }
    } 
   DebugTN("done for PSU alert config"); 
}  
    
/**
 *  general alert handle config
 */
void emuLvCRB_configAlert(string sDpName,dyn_float dfLimits,dyn_string dsAlertTexts,dyn_string dsAlertClasses)
{
    dyn_bool dbLimitsincl = makeDynBool(true);
    if(dynlen(dfLimits)>1)
    {  
      dpSetWait(sDpName+ ":_alert_hdl.._type", 13, 
                sDpName+ ":_alert_hdl.1._type", 4, // Min-Max value range 
                sDpName+ ":_alert_hdl.2._type", 4, 
                sDpName+ ":_alert_hdl.3._type", 4, 
                sDpName+ ":_alert_hdl.1._u_limit", dfLimits[1], // Upper limit 
                sDpName+ ":_alert_hdl.2._u_limit", dfLimits[2], 
                sDpName+ ":_alert_hdl.2._l_limit", dfLimits[1], // Lower limit  
                sDpName+ ":_alert_hdl.3._l_limit", dfLimits[2], 
                sDpName+ ":_alert_hdl.1._u_incl", !dbLimitsincl[1], // Upper limit not belonging to the range   
                sDpName+ ":_alert_hdl.2._u_incl", !dbLimitsincl[1], 
                sDpName+ ":_alert_hdl.2._l_incl",  dbLimitsincl[1],  
                sDpName+ ":_alert_hdl.3._l_incl",  dbLimitsincl[1], 
                sDpName+ ":_alert_hdl.1._text",    dsAlertTexts[1], // Alert text of range 1  
                sDpName+ ":_alert_hdl.3._text",    dsAlertTexts[1], 
                sDpName+ ":_alert_hdl.1._class",   dsAlertClasses[1], // Alert class of range 1 
                sDpName+ ":_alert_hdl.3._class",   dsAlertClasses[1], 
                sDpName+ ":_alert_hdl.._orig_hdl", TRUE, // Working with the original value  
                sDpName+ ":_alert_hdl.._active", TRUE); // Activates the alert handling  
    }
   else
    {
      dpSetWait(sDpName+ ":_alert_hdl.._type", 13,
                sDpName+ ":_alert_hdl.1._type", 4,
                sDpName+ ":_alert_hdl.2._type", 4,
                sDpName+ ":_alert_hdl.1._u_limit", dfLimits[1],
                sDpName+ ":_alert_hdl.2._l_limit", dfLimits[1],
                sDpName+ ":_alert_hdl.1._u_incl",  !dbLimitsincl[1],
                sDpName+ ":_alert_hdl.2._l_incl",  dbLimitsincl[1],
                sDpName+ ":_alert_hdl.2._text",  dsAlertTexts[1],
                sDpName+ ":_alert_hdl.2._class", dsAlertClasses[1],
                sDpName+ ":_alert_hdl.._orig_hdl", TRUE,             
                sDpName+ ":_alert_hdl.._active",TRUE);   
    }    
}  
/**
 *  alert class modify for PSU ADV/CANV
 */
void emuLvCRB_modifyAlertClassPsu(string sDpName)
{
  dpSetWait(sDpName+ ":_alert_hdl.._active",false);
  dpSetWait(sDpName+ ":_alert_hdl.1._class","_fwErrorNack.",
            sDpName+ ":_alert_hdl.2._class","_fwWarningNack.",
            sDpName+ ":_alert_hdl.4._class","_fwWarningNack.",
            sDpName+ ":_alert_hdl.5._class","_fwErrorNack.",
            sDpName+ ":_alert_hdl.._active",true);  
}  
/**
 * set label/panel for FSM with ui.label/panels in _FwFsmDevice/_FwFsmObject
 * but those setup will get lost after regenerating the FSM
 */
void emuLvCRB_setFsmUiInfo()
{
  dyn_string dsObjects,dsDevices,dsSplit1,dsSplit2,dsSplit3,dsLast;
  string sDpName,sLastName,sCrate,sKey,sLabel,sPanel;
  int i;
  DebugTN("set label/panel for CRB/PSU FSM tree");
  //set label for Crb device,no need set panel for Crb device,the reason is:
  //use standard fwUi.pnl, and operating panel is defined in fwCrb_CSC_LV FSM type already 
  mapping mMarCrbID = emuLvCRB_getMapping("Marthon_Crb_ID_P5");
  dsDevices = dpNames("*_CRB","_FwFsmDevice");
  for(i=1;i<=dynlen(dsDevices);i++)
  {
     sDpName = dpSubStr(dsDevices[i], DPSUB_DP);
     dsSplit1 = strsplit(sDpName,"|");
     dsSplit2 = strsplit(dsSplit1[dynlen(dsSplit1)],"/");
     sLastName = dsSplit2[dynlen(dsSplit2)];
     dsLast = strsplit(sLastName,"_");
     sCrate = dsLast[2]+"_"+dsLast[3]+"_"+dsLast[4]+"_"+dsLast[6];
     sKey = dsLast[3]+"_"+dsLast[4];
     sLabel = sCrate+"/"+mMarCrbID[sKey];
     dpSetWait(dsDevices[i]+".ui.label",sLabel);
  }  
  //set label/panel for Psu device
  dsDevices = dpNames("*Branch*","_FwFsmDevice");
  for(i=1;i<=dynlen(dsDevices);i++)
  {
     sDpName = dpSubStr(dsDevices[i], DPSUB_DP);
     dsSplit1 = strsplit(sDpName,"|");
     dsSplit2 = strsplit(dsSplit1[dynlen(dsSplit1)],"/");
     dsSplit3 = strsplit(dsSplit2[dynlen(dsSplit2)],"_");
     sLabel = dsSplit3[dynlen(dsSplit3)];
     dpSetWait(dsDevices[i]+".ui.label",sLabel);                                                 
  }
  //set label/panel for Crb objects
  dsObjects = dpNames("*LV_CRB","_FwFsmObject");
  for(i=1;i<=dynlen(dsObjects);i++)
  { 
    sDpName = dpSubStr(dsObjects[i], DPSUB_DP);
    dsSplit1 = strsplit(sDpName,"|");
    sLastName = (dsSplit1[dynlen(dsSplit1)]);
    sLabel = substr(sLastName, strlen("CSC_"));
    dpSetWait(dsObjects[i]+".ui.label",sLabel,
              dsObjects[i]+".ui.panels",makeDynString("fwFSMuser/fwUi.pnl",
                                                      "CMS_CSCfw_LV_CRB/emuStationCrbOperation.pnl"));      
  } 
  //set label for Psu objects, no panel set needed
  dsObjects = dpNames("*PSU","_FwFsmObject");
  for(i=1;i<=dynlen(dsObjects);i++)
  { 
    sDpName = dpSubStr(dsObjects[i], DPSUB_DP);
    dsSplit1 = strsplit(sDpName,"|");
    sLastName = (dsSplit1[dynlen(dsSplit1)]);
    sLabel = substr(sLastName, strlen("CSC_ME_"));
    dpSetWait(dsObjects[i]+".ui.label",sLabel);      
  } 
  DebugTN("done for CRB/PSU FSM UI info setting ");
}
/**
 * set label/panel for FSM with .userdata in _FwTreeNode
 * 
 */
void emuLvCRB_setFsmUserData()
{
  dyn_string dsObjects,dsDevices,dsSplit1,dsSplit2,dsLast;
  string sDpName,sLastName,sCrate,sKey,sLabel,sPanel;
  int i;
  mapping mMarCrbID = emuLvCRB_getMapping("Marthon_Crb_ID_P5");
  DebugTN("set userdata for Crb/PSU FSM tree node");
  //set label for Crb device in _FwTreeNode,no need for panel assign
  dsDevices = dpNames("*TN_CRB*LV_CRB","_FwTreeNode");
  for(i=1;i<=dynlen(dsDevices);i++)
  {
     sDpName   = dpSubStr(dsDevices[i], DPSUB_DP);
     dsSplit1  = strsplit(sDpName,"/");
     sLastName = dsSplit1[dynlen(dsSplit1)];
     dsLast = strsplit(sLastName,"_");
     sCrate = dsLast[2]+"_"+dsLast[3]+"_"+dsLast[4]+"_"+dsLast[6];
     sKey = dsLast[3]+"_"+dsLast[4];
     sLabel = sCrate+"/"+mMarCrbID[sKey];
     dpSetWait(dsDevices[i]+".userdata",makeDynString(1,1,sLabel,""));
  }  
  //set label for PSU branch with .userdata in _FwTreeNode
  dsDevices = dpNames("*Branch*","_FwTreeNode");
  for(i=1;i<=dynlen(dsDevices);i++)
  {
     sDpName  = dpSubStr(dsDevices[i], DPSUB_DP);
     dsSplit1 = strsplit(sDpName,"/");
     dsSplit2 = strsplit(dsSplit1[dynlen(dsSplit1)],"_");
     sLabel = dsSplit2[dynlen(dsSplit2)];
     dpSetWait(dsDevices[i]+".userdata",makeDynString(1,1,sLabel,""));
  }
  //set label/panel for Crb objects with .userdata in _FwTreeNode
  dsObjects = dpNames("*TN_CSC*LV_CRB","_FwTreeNode");
  for(i=1;i<=dynlen(dsObjects);i++)
  { 
    sDpName = dpSubStr(dsObjects[i], DPSUB_DP);
    sLabel = substr(sDpName, strlen("fwTN_CSC_"));
    dpSetWait(dsObjects[i]+".userdata",makeDynString(1,1,sLabel,
                                      "CMS_CSCfw_LV_CRB/emuStationCrbOperation.pnl"));    
  }
   //set label for Psu objects with .userdata in _FwTreeNode 
  dsObjects = dpNames("*TN_CSC*PSU","_FwTreeNode");
   for(i=1;i<=dynlen(dsObjects);i++)
  { 
    sDpName = dpSubStr(dsObjects[i], DPSUB_DP);
    sLabel = substr(sDpName, strlen("fwTN_CSC_ME_"));
    dpSetWait(dsObjects[i]+".userdata",makeDynString(1,1,sLabel,""));      
  } 
  DebugTN("done for CRB/PSU FSM userdata setting");
}
/**
 * set dp archiving for elmb ai, FSM
 */
void emuLvCRB_setArchiving()
{
  dyn_string dsAiNames,dsFsmDevs,dsFsmObjs;
  int i;
  DebugTN("set archiving config for CRB...");
  dsAiNames = dpNames("*ai*","FwElmbAi");
  if(dynlen(dsAiNames))
  { 
    for(i=1;i<=dynlen(dsAiNames);i++)
    {
      dpSetWait (dsAiNames[i] + ".value:_archive.._type",45,
                 dsAiNames[i] + ".value:_archive.._archive",1,
                 dsAiNames[i] + ".value:_archive.1._type",3,
                 dsAiNames[i] + ".value:_archive.1._class","_EVENT",
                 dsAiNames[i] + ".value:_archive.1._interv",0,
                 dsAiNames[i] + ".value:_archive.1._interv_type",0,
                 dsAiNames[i] + ".value:_archive.1._std_type",2,
                 dsAiNames[i] + ".value:_archive.1._std_tol",fTolElmbAi,
                 dsAiNames[i] + ".value:_archive.1._std_time",iTimeElmbAi);
    }  
  } 
  dsFsmDevs = dpNames("*","_FwFsmDevice");
  if(dynlen(dsFsmDevs))
  {
    for(i=1;i<=dynlen(dsFsmDevs);i++)
    {
      emuLvCRB_setFsmDpArchiving(dsFsmDevs[i]);
    }  
  }
  dsFsmObjs = dpNames("*","_FwFsmObject");
  if(dynlen(dsFsmObjs))
  {
    for(i=1;i<=dynlen(dsFsmObjs);i++)
    {
      emuLvCRB_setFsmDpArchiving(dsFsmObjs[i]);
    }  
  }
  DebugTN("done for CRB archiving config");    
}  
/**
 * set archiving for FSM dps
 */
void emuLvCRB_setFsmDpArchiving(string sDpName)
{
  dyn_string ds = makeDynString( sDpName + ".fsm.currentState",
                                 sDpName + ".fsm.sendCommand",
                                 sDpName + ".fsm.executingAction",
                                 sDpName + ".fsm.currentParameters");
  for (int i=1; i<=dynlen(ds);i++)
  {
    dpSetWait (ds[i] + ":_archive.._type",45,
               ds[i] + ":_archive.._archive",1,
               ds[i] + ":_archive.1._type",3,
               ds[i] + ":_archive.1._class","_EVENT",
               ds[i] + ":_archive.1._std_type",4); //  old/new comparison
  }
}
/**
 * generate a toggle at digital output A:4 with a time interval
 * @param sElmbName -- which elmb of A:4
 * @param iInterval -- time interval for toggle
 */
void emuLvCRB_generateToggleA4(string sElmbName,int iInterval)
{
  dyn_string dsExceptionInfo;
  //iInterval = 300;
  delay(0,iInterval/3);
  emuLvCRB_setDoBitSync(sElmbName,"A;4",false,dsExceptionInfo);
  delay(0,iInterval/3); 
  emuLvCRB_setDoBitSync(sElmbName,"A;4",true,dsExceptionInfo); 
  delay(0,iInterval/3);
}
/**
 * adaptor to fwElmbUser_setDoBitsSynchronized
 * @param string sElmbName     -- Elmb name
 * @param string sBitId        --"port;bit"
 * @param bool   bValue        -- true/false
 * @param dyn_string dsExceptionInfo
 */
void emuLvCRB_setDoBitSync(string sElmbName,string sBitId,bool bValue,dyn_string &dsExceptionInfo)
{
  emuLvCRB_initalizeParam();
  float fElmbVersion;
  dpGet(gSystemNameCRB+"fwInstallation_fwElmb.componentVersion",fElmbVersion);
  emuLvCRB_showDebug(bDebug,"current elmb version is "+fElmbVersion);
  dyn_string dsBitIds = makeDynString(sBitId);
  dyn_bool dbValues = makeDynBool(bValue);  
  if(fElmbVersion>=4.2)
   {
     fwElmbUser_setDoBitsSynchronized(sElmbName,dsBitIds,dbValues,dsExceptionInfo); 
//     fwElmbUser_setDoBits(sElmbName,dsBitIds,dbValues,dsExceptionInfo); 
     //fwElmbUser_setDoBit(sElmbName,sBitId,bValue,dsExceptionInfo); 
     //DebugTN("dsExceptionInfo:"+dsExceptionInfo);
     if(dynlen(dsExceptionInfo)>0)
        {
          //DebugTN(dsExceptionInfo);
          delay(0,100);
          fwElmbUser_setDoBitsSynchronized(sElmbName,dsBitIds,dbValues,dsExceptionInfo);
//          fwElmbUser_setDoBits(sElmbName,dsBitIds,dbValues,dsExceptionInfo);
         // fwElmbUser_setDoBit(sElmbName,sBitId,bValue,dsExceptionInfo,false);
        }
     
     // readback the bit and check if it's indeed got set... try 3 times..
     int resendingTryCount = 0;
     bool valueGood = false;
     while ((resendingTryCount < 3) && !valueGood) {
        unsigned value = fwElmbUser_getDoByte(sElmbName, strsplit(sBitId, ";")[1], dsExceptionInfo);
        if (dynlen(dsExceptionInfo) > 0) { return; }
        bit32 bit32value = value;
        int bitValue = getBit(bit32value, strsplit(sBitId, ";")[2]);
        bool bbitValue = bitValue;
        if (bbitValue != bValue) {
          DebugTN("DO readback value of bit " + sElmbName + ", " + sBitId + " is not as it's supposed to be after setting. Setting was " + bValue + ", but readback " + bbitValue + ". Trying to set again (attempt #" + resendingTryCount + ")");
          delay(0, 100);
          fwElmbUser_setDoBitsSynchronized(sElmbName,dsBitIds,dbValues,dsExceptionInfo);
//          fwElmbUser_setDoBits(sElmbName,dsBitIds,dbValues,dsExceptionInfo);
        } else {
          valueGood = true;
        }
        resendingTryCount++;
      }
  
   }
  else
   {
     fwElmbUser_setDoBit(sElmbName,sBitId,bValue,dsExceptionInfo);
   }        
}
/**
 * Create all for LV CRB
 */
void emuLvCRB_createLvCRBAll()
{
  DebugTN("starting create all for CSC LV CRB"); 
  emuLvCRB_initalizeParam();
  delay(0.5);
  emuLvCRB_createLvType();
  delay(0.5);
  emuLvCRB_createElmbCRB();
  delay(0.5);
  emuLvCRB_createElmbPSU();
  delay(0.5);
  emuLvCRB_createFwCrbPsu();
  delay(0.5);
  emuLvCRB_setAlertElmbAi();
  delay(0.5);
  emuLvCRB_setAlertElmbNode();
  delay(0.5);
  emuLvCRB_setAlertCrb();
  delay(0.5);
  emuLvCRB_setAlertPsu();
  delay(0.5);
  emuLvCRB_createCrbTree();
  delay(0.5);
  emuLvCRB_createPsuTree();
  delay(0.5);
  fwFsmTree_generateAll(); 
  fwFsmTree_refreshTree();
  fwFsm_stopAllDomains();
  delay(0.5);
  emuLvCRB_setFsmUiInfo();
  delay(0.5);
  emuLvCRB_setFsmUserData();
  delay(0.5);
  emuLvCRB_setArchiving();
  DebugTN("done for CSC LV CRB");
}  

  
  
  
  
  
  
