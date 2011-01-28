#uses "fwInstallation.ctl"
#uses "fwWiener/fwWiener.ctl"
#uses "CMS_CSCfw_LV_MRTN/emuLvMRTN_Constants.ctl"
#uses "fwDevice/fwDevice.ctl"
#uses "fwDeviceEditorNavigator/fwDeviceEditorNavigator.ctl"
#uses "fwConfigs/fwAlertConfig.ctl"
#uses "fwFsmTreeDisplay.ctl"
//----------------------------------------------------------------------------------
/**@file

This library contains LV_MRTN functions for EMU DCS LV project.

@author Xiaofeng Yang (PH/UCM)
@date   Aug 2010
*/
      const bool      bDebug                = false; //debug on, else off
      const bool      bP5Project            = true; //for P5,else for 904      
//---------------------------------------------------------------------------------
/**
 * initialize generate parameters MRTN,like system name where the MRTN is installed
 * global variable gSystemNameMRTN: the system name where MRTN is installed
 */
void emuLvMRTN_initalizeParam()
{
  addGlobal("gSystemNameMRTN", STRING_VAR);
  //get system name where MRTN component installation
  /*
   dyn_string ds;
   fwInstallation_getApplicationSystem("CMS_CSCfw_LV_MRTN",ds);    
   gSystemNameMRTN = ds[1]; //with ":"  
  */
  //get system name with function getSystemName()
   gSystemNameMRTN = getSystemName(); 
}
/**
 * simple debug information on/off
 * bDebug=1, show debug information, else hide debug information
 */
void emuLvMRTN_showDebug(bool bDebug, anytype aMessage)
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
void emuLvMRTN_setDbConfig() 
{
  fwRDBConfig_setUser("cms_csc_pvss_cond");
  fwRDBConfig_setHost("cms_pvss_cond");  
}

/**
 * delete all dps,node,FSM tree,FSM scripts for MRTN
 */
void emuLvMRTN_deleteAll()
{
  DebugTN("delete all for MRTN, waiting...");
  //delete all dps for MRTN
  dyn_string dsDpTypes,dsDpNames;
  int i,j;
  dsDpTypes = emuLvMRTN_getDynString("DpType_Delete");  
  emuLvMRTN_showDebug(bDebug,dsDpTypes);
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
    if(strpos(dsDpTypes[i],"MRTN")>0 ||strpos(dsDpTypes[i],"_DATA")>0 )
         {dpTypeDelete(dsDpTypes[i]);}      
  }     
  //delete FSM tree
  fwFsmTree_removeNode("FSM","CMS_CSC_LV_CAN2",1);
  
  //remove dps from _FwTreeNode
  dyn_string dsSearchTypes = makeDynString("*MRTN*","*CAN2*","*Wiener*");
  for (i=1;i<=dynlen(dsSearchTypes);i++)
  {
    dsDpNames = dpNames(gSystemNameMRTN+dsSearchTypes[i],"_FwTreeNode");
    for(j=1;j<=dynlen(dsDpNames);j++)
       {
         dpDelete(dsDpNames[j]);
       }
  }  
  //remove dps from FwNode
   dsDpNames = dpNames(gSystemNameMRTN+"*CAN2*","FwNode");
    for(j=1;j<=dynlen(dsDpNames);j++)
       {
         dpDelete(dsDpNames[j]);
       }
    
  //clean dp EMUALIASES in dptype EMUDCS_DYN_STRING 
  if(dpExists(gSystemNameMRTN+"EMUALIASES"))
    { 
     dpSet(gSystemNameMRTN+"EMUALIASES.",makeDynString());
    }
  
  //delete all FSM install scripts
  string sPath;
  dyn_string dsConfPath,dsFileNames;
  int i,j; 
  fwInstallation_getProjPaths(dsConfPath);
  sPath = dsConfPath[1]+"/scripts/libs";
  emuLvMRTN_showDebug(bDebug, sPath);
  dsFileNames = getFileNames(sPath,"*CAN2$install");
  emuLvMRTN_showDebug(bDebug,dsFileNames); 
  if ( dynlen(dsFileNames)>0)
  {  
     for (int i=1;i<=dynlen(dsFileNames);i++)
      {  
       int j = remove(sPath+"/"+dsFileNames[i]);
        emuLvMRTN_showDebug(bDebug,j);
      } 
   }
  delay(1);
  DebugTN("done for Lv MRTN deleting"); 
}
/**
 * create all dps for MRTN
 */
void emuLvMRTN_createHardware()
{
  DebugTN("create hardware for Lv MRTN, waiting...");
  // _fwDeviceEditorNavigator_init(); // needed otherwise the configureDevice function does not work 
  int iBus, iCr, iCh, i, j;
  dyn_string dsCanBuses,dsChannels,dsDevice,dsParentDevice,dsExceptionInfo;
  dyn_dyn_string ddsCratesIDs;
  //----create Can-bus for MRTN---------
  dsCanBuses = emuLvMRTN_getDynString("MRTN_CanBus_P5");
  for(iBus=1;iBus<=dynlen(dsCanBuses);iBus++)
    {
      dsDevice=makeDynString(dsCanBuses[iBus],"FwWienerCanBus","","CAN Bus");
      dsParentDevice=makeDynString(gSystemNameMRTN+"Wiener","","","");
      fwDevice_create(dsDevice, dsParentDevice, dsExceptionInfo);
      emuLvMRTN_configHardware(dsDevice,dsParentDevice,dsExceptionInfo);
  //----create Crate for MRTN--------- 
      ddsCratesIDs = emuLvMRTN_getDynDynString("MRTN_Crates_P5");   
      for(iCr=1;iCr<=dynlen(ddsCratesIDs[iBus]);iCr++)
       {
         dsDevice=makeDynString(ddsCratesIDs[iBus][iCr],"FwWienerMarathon","","PL508 Crate (CAN)");
         dsParentDevice=makeDynString(gSystemNameMRTN+"Wiener/"+dsCanBuses[iBus],"","","");
         fwDevice_create(dsDevice, dsParentDevice, dsExceptionInfo);
         emuLvMRTN_configHardware(dsDevice,dsParentDevice,dsExceptionInfo);
   //----create Channel for MRTN---------  
         dsChannels = emuLvMRTN_getDynString("MRTN_Channels");     
         for(iCh=1;iCh<=dynlen(dsChannels);iCh++)
          {
            dsDevice=makeDynString("Channel"+dsChannels[iCh],"FwWienerMarathonChannel","","PL508 Channel (CAN)");
            dsParentDevice=makeDynString(gSystemNameMRTN+"Wiener/"+dsCanBuses[iBus]+"/"+ddsCratesIDs[iBus][iCr],"","","");
            fwDevice_create(dsDevice, dsParentDevice, dsExceptionInfo);
            emuLvMRTN_configHardware(dsDevice,dsParentDevice,dsExceptionInfo);
          }  //for ch
       } //for crate
    } //for can bus
  DebugTN("done for creating hardware of MRTN");
}
/**
 * config for all MRTN dps
 * @param:dyn_string dsDevice        --> Device information
 * @param:dyn_string dsParentDevice  --> Parent Device information
 * @param:dyn_string dsExceptionInfo --> Exception information
 */
void emuLvMRTN_configHardware(dyn_string dsDevice,dyn_string dsParentDevice,dyn_string &dsExceptionInfo)
{
   _fwDeviceEditorNavigator_init(); // needed otherwise the configureDevice function does not work 
   dsDevice[1]=dsParentDevice[1]+"/"+dsDevice[1];
   fwDevice_setAddress(dsDevice[fwDevice_DP_NAME],makeDynString(fwDevice_ADDRESS_DEFAULT),dsExceptionInfo);
   fwDevice_setAlert  (dsDevice[fwDevice_DP_NAME],fwDevice_ALERT_SET, dsExceptionInfo);   
} 
/**
 * create dptype "Db_MRTN" and datapoints for MRTN
 */
void emuLvMRTN_createLvType()
{  
  DebugTN("create dptype and dps for Lv MRTN");
  emuLvMRTN_createDpType("Db_MRTN"); //create type Db_MRTN  
  dpCreate("db_mrtn","Db_MRTN");
  dpCreate("db_mrtn_geographic","Db_MRTN");  
  dyn_string dsDb,dsDbGeog;
  if(bP5Project)
   {
    dsDb = emuLvMRTN_getDynString("Db_MRTN_P5");
    dsDbGeog = emuLvMRTN_getDynString("Db_MRTN_Geog_P5");                             
   }
  else
   { 
    dsDb = emuLvMRTN_getDynString("Db_MRTN_B904");
    dsDbGeog = emuLvMRTN_getDynString("Db_MRTN_Geog_B904");
   }
  dpSetWait("db_mrtn.list",dsDb,
            "db_mrtn_geographic.list",dsDbGeog);
  emuLvMRTN_showDebug(bDebug,"MRTN Db set"); 
  //---create archiving dptype and dp,dp function
  emuLvMRTN_createDpType("CSC_MARATON_V_DATA");
  emuLvMRTN_createDpType("CSC_MARATON_I_DATA");
  emuLvMRTN_createDpType("CSC_MARATON_T_DATA");  
  if(!dpExists("EMUALIASES"))
   {
     dpCreate("EMUALIASES","MUDCS_DYN_STRING");    
   }
  if(!dpExists("CMS_CSC_LV_MRTN"))
    {
     dpCreate("CMS_CSC_LV_MRTN","MUDCS_STRING");
    } 
  DebugTN("done for creating Lv MRTN dptype and dps");
}
/**
 * define dptype "Db_MRTN" for MRTN
 * @param:string sDpType --> dptype need tb crated
 */
void emuLvMRTN_createDpType(string sDpType)
{
  dyn_dyn_string ddsdepes;
  dyn_dyn_int    ddidepei;  
   if(sDpType=="Db_MRTN")
    {
      ddsdepes[1] = makeDynString ("Db_MRTN","");
      ddsdepes[2] = makeDynString ("","list");
      
      ddidepei[1] = makeDynInt (DPEL_STRUCT);
      ddidepei[2] = makeDynInt (0,DPEL_DYN_STRING);
    }
   else if(sDpType=="CSC_MARATON_V_DATA"||
           sDpType=="CSC_MARATON_I_DATA"||
           sDpType=="CSC_MARATON_T_DATA")
    {
      ddsdepes[1] = makeDynString (sDpType,"");
      ddsdepes[2] = makeDynString ("","value");
      
      ddidepei[1] = makeDynInt (DPEL_STRUCT);
      ddidepei[2] = makeDynInt (0,DPEL_FLOAT);
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
 * create logical node for MRTN
 * @param:string sParentNode  --> parent node name
 * @param:string sNode        --> node name
 * @param:string sConfigPanel --> panel name for config
 * @param:string sConfigPanel --> panel name for operate
 */
void emuLvMRTN_createLogicalNode(string sParentNode,string sNode,string sConfigPanel,string sOpPanel)
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
	fwDevice_create(makeDynString(sNode, "FwNode", "", ""),
					makeDynString(sParentNode, ""),
					dsExceptionInfo);			
 if(dynlen(dsExceptionInfo) > 0) {return;}
	
	dpSetWait(gSystemNameMRTN + sName + ".type", sType,
	          gSystemNameMRTN + sName + ".dpTypes", makeDynString(),
	          gSystemNameMRTN + sName + ".navigatorPanels", makeDynString(sOpPanel),
	          gSystemNameMRTN + sName + ".editorPanels", makeDynString(sConfigPanel));
          	
	dpSetAlias(gSystemNameMRTN + sName + ".", sAlias);
 
 	//test whether there were errors
	deErr = getLastError(); 
 	if(dynlen(deErr) > 0)
 	{
 		dpDelete(sName);
 		fwException_raise(	exceptionInfo,
 							"ERROR",
 							"Could not create the logical node",
 							"");
		return;
 	}
}
/**
 * create FSM node for MRTN
 * @param:string sParentNode  --> parent node name
 * @param:string sNode        --> node name
 * @param:string sType        --> Fsm Type
 * @param:int   iCu           --> is Cu or Du
 */
void emuLvMRTN_createFsmNode(string sParentNode,string sNode,string sType,int iCu)
{
  if(fwFsmTree_isNode(sNode))
  {
    emuLvMRTN_showDebug(bDebug,"FSM node exists already");
    return;
  }
  else 
  {
    fwFsmTree_addNode(sParentNode,sNode,sType,iCu);
    emuLvMRTN_showDebug(bDebug,"FSM node created");
  }
}  
/**
 * create Tree for MRTN
 */
void emuLvMRTN_createFsmTree()
{
  dyn_string dsCrates,dsChannels,dsMRTNStationNodes,dsMRTNCrateNodes,dsMRTNDeviceNodes;
  int i,j,k; 
  mapping mCrateNames = emuLvMRTN_getMapping("MRTN_Crate_Node_Name_P5");
 //----------create top node for logical and FSM tree--------------------
  DebugTN("create tree for Lv MRTN, waiting...");
  emuLvMRTN_createLogicalNode("","CMS_CSC_LV_CAN2","",""); 
  emuLvMRTN_createFsmNode("FSM","CMS_CSC_LV_CAN2",TopFsmObjType,1);
 //-----------create station node----------------------------------------
  dsMRTNStationNodes = emuLvMRTN_getDynString("MRTN_Station_Nodes");
  for(i=1;i<=dynlen(dsMRTNStationNodes);i++)
   {
     emuLvMRTN_createLogicalNode("CMS_CSC_LV_CAN2",dsMRTNStationNodes[i],"",""); 
     emuLvMRTN_createFsmNode("CMS_CSC_LV_CAN2",dsMRTNStationNodes[i],StationFsmObjType,0);
 //-----------create crate node------------------------------------------ 
     dsMRTNCrateNodes = emuLvMRTN_getDynString(dsMRTNStationNodes[i]);
     for(j=1;j<=dynlen(dsMRTNCrateNodes);j++)
      { 
       emuLvMRTN_createLogicalNode("CMS_CSC_LV_CAN2/"+dsMRTNStationNodes[i],dsMRTNCrateNodes[j],"","");
       emuLvMRTN_createFsmNode(dsMRTNStationNodes[i],dsMRTNCrateNodes[j],CrateFsmObjType,0);
 //-----------create crate device node-----------------------------------       
      
       dsCrates = strsplit(mCrateNames[dsMRTNCrateNodes[j]],"/");
       dpSetAlias(mCrateNames[dsMRTNCrateNodes[j]]+".",
                  "CMS_CSC_LV_CAN2/"+dsMRTNStationNodes[i]+"/"+dsMRTNCrateNodes[j]+"/"+dsCrates[dynlen(dsCrates)]);
       emuLvMRTN_createFsmNode(dsMRTNCrateNodes[j],mCrateNames[dsMRTNCrateNodes[j]],MRTNCrateFsmDevType,0);
 //----------create channel node----------------------------------------- 
       dsChannels = emuLvMRTN_getDynString("MRTN_Channels");
       for (k=1;k<=dynlen(dsChannels);k++)
         {
           dpSetAlias(mCrateNames[dsMRTNCrateNodes[j]]+"/Channel"+dsChannels[k]+".",
                      "CMS_CSC_LV_CAN2/"+dsMRTNStationNodes[i]+"/"+dsMRTNCrateNodes[j]+"/Channel"+dsChannels[k]);
           emuLvMRTN_createFsmNode(dsMRTNCrateNodes[j],mCrateNames[dsMRTNCrateNodes[j]]+"/Channel"+dsChannels[k],
                                   MRTNChannelFsmDevType,0);
         }
      } 
   }    
  DebugTN("done for Lv MRTN tree");
}  
/**
 * set alert config for MRTN dps,basic alert set has been done in emuLvMRTN_configHardware
 */
void emuLvMRTN_setAlertConfig()
{
  DebugTN("set alert config for Lv MRTN, waiting...");  
  dyn_string dsItemLists,dsExceptionInfo;
  dyn_string dsStatusAlertItems  = emuLvMRTN_getDynString("MRTN_Channel_Alert_Status");
  dyn_string dsChannelAlertTexts = emuLvMRTN_getDynString("MRTN_Channel_Alert_Texts");
  dyn_string dsCrateAlertTexts   = emuLvMRTN_getDynString("MRTN_Crate_Alert_Texts");
  dyn_string dsChannelNumbers    = emuLvMRTN_getDynString("MRTN_Channels");
  dyn_string dsCrates   = dpNames(gSystemNameMRTN+"*Crate*","FwWienerMarathon");
  dyn_string dsChannels = dpNames(gSystemNameMRTN+"*Channel*","FwWienerMarathonChannel");
  //---change alert text0 and text1--------------------------
  for(int i=1;i<=dynlen(dsChannels);i++)    
     {
       for(int j=1;j<=dynlen(dsStatusAlertItems);j++)
         {            
            dpSetWait(dsChannels[i]+".Status."+dsStatusAlertItems[j]+":_alert_hdl.._active",false);
            delay(0,10);               
            dpSetWait(dsChannels[i]+".Status."+dsStatusAlertItems[j]+":_alert_hdl.._text0",dsChannelAlertTexts[1],
                      dsChannels[i]+".Status."+dsStatusAlertItems[j]+":_alert_hdl.._text1",dsChannelAlertTexts[2]);
            dpSetWait(dsChannels[i]+".Status."+dsStatusAlertItems[j]+":_alert_hdl.._active",true);
         }
     }      
  //---get alert dplists for crate from channel--------------
  for(int k=1;k<=dynlen(dsCrates);k++)
     { 
       dynClear(dsItemLists);
       for(int l=1;l<=dynlen(dsChannelNumbers);l++)
         {
           for(int m=1;m<=dynlen(dsStatusAlertItems);m++)
            {
             dynAppend(dsItemLists,dsCrates[k]+"/Channel"+dsChannelNumbers[l]+".Status."+dsStatusAlertItems[m]);
            }
         }             
  //----set summary alert for crate--------------------------------        
       dpSetWait(dsCrates[k]+".:_alert_hdl.._active",false);
       delay(0,10);           
       fwAlertConfig_createSummary(dsCrates[k]+".",makeDynString(dsCrateAlertTexts[1],dsCrateAlertTexts[2]),
                                   dsItemLists,"", makeDynString(),"",dsExceptionInfo);
       dpSetWait(dsCrates[k]+".:_alert_hdl.._active",true);   
     } 
  DebugTN("done for Lv MRTN alert config"); 
}
/**
 * set label/panel for MRTN FSM with ui.label/panels in _FwFsmDevice/_FwFsmObject
 * no need to set label/panel for MRTN device
 * but those setup will get lost after regenerating the FSM
 */
void emuLvMRTN_setFsmUiInfo()
{
  dyn_string dsObjects,dsDevices,dsSplit1,dsSplit2;
  string sDpName,sLabel;
  int i;
  DebugTN("set label/panel for Lv MRTN FSM tree");
  mapping mMRTNCrateNodeLabel = emuLvMRTN_getMapping("MRTN_Crate_Node_Label_P5");  
  //---set label/panel for MRTN objects------
  dsObjects = dpNames(gSystemNameMRTN+"*_LV_MRTN","_FwFsmObject");
  for(i=1;i<=dynlen(dsObjects);i++)
  { 
    sDpName = dpSubStr(dsObjects[i], DPSUB_DP);
    dsSplit1 = strsplit(sDpName,"|");
    sLabel = dsSplit1[dynlen(dsSplit1)];
    if(strpos(sLabel,"CR")>0)
     {
       //---for MRTN crate obj node-----
       dpSetWait(dsObjects[i]+".ui.label",mMRTNCrateNodeLabel[sLabel],
                 dsObjects[i]+".ui.panels",makeDynString("fwFSMuser/fwUi.pnl",
                                                         "CMS_CSCfw_LV_MRTN/emuDevMRTN_CNodeOperation.pnl")); 
     } 
    else
     { 
       //---for MRTN station obj node----
       sLabel = substr(sLabel, strlen("CSC_"));   
       dpSetWait(dsObjects[i]+".ui.label",sLabel);
     }
  }  
  DebugTN("done for setting label/panel for Lv MRTN FSM tree");
}
/**
 * set label/panel for MRTN FSM with .userdata in _FwTreeNode
 * 
 */
void emuLvMRTN_setFsmUserData()
{
  dyn_string dsObjects,dsDevices,dsSplit1,dsSplit2;
  string sDpName,sLabel;
  int i;  
  DebugTN("set userdata for Lv MRTN FSM tree node");
  mapping mMRTNCrateNodeLabel = emuLvMRTN_getMapping("MRTN_Crate_Node_Label_P5"); 
  //set label/panel for MRTN objects with .userdata in _FwTreeNode
  dsObjects = dpNames(gSystemNameMRTN+"*TN_CSC*LV_MRTN","_FwTreeNode");
  for(i=1;i<=dynlen(dsObjects);i++)
  { 
    sDpName = dpSubStr(dsObjects[i], DPSUB_DP);
    sLabel = substr(sDpName, strlen("fwTN_CSC_"));
    if(strpos(sLabel,"CR")>0)
     {
      dpSetWait(dsObjects[i]+".userdata",makeDynString(1,1,mMRTNCrateNodeLabel["CSC_"+sLabel],
                                                       "CMS_CSCfw_LV_MRTN/emuDevMRTN_CNodeOperation.pnl"));  
     } 
    else
     { 
      dpSetWait(dsObjects[i]+".userdata",makeDynString(1,1,sLabel,""));
     } 
  }
  DebugTN("done for setting userdata for Lv MRTN FSM tree node");
}
/**
 * set archiving for FSM dps
 * @param:string sDpName  --> dpname
 */
void emuLvMRTN_setFsmDpArchiving(string sDpName)
{
  dyn_string ds = makeDynString( sDpName + ".fsm.currentState",
                                 sDpName + ".fsm.sendCommand",
                                 sDpName + ".fsm.executingAction",
                                 sDpName + ".fsm.currentParameters");
  for (int i=1; i<=dynlen(ds);i++)
  {
    emuLvMRTN_setDpSmoothingBool(ds[i]);    
  }
}
/**
 * set dp archiving for Marathon,Marathon channel, FSM
 */
void emuLvMRTN_setArchiving()
{
  dyn_string dsChannelNames,dsCrateNames,dsFsmDevs,dsFsmObjs;
  int i;
  DebugTN("set archiving...");
  //---config archiving for channels---------------------
  dsChannelNames = dpNames(gSystemNameMRTN+"*Wiener*","FwWienerMarathonChannel");
  for(i=1;i<=dynlen(dsChannelNames);i++)
    {  
   //----set config for new archiving dp-------------------
       emuLvMRTN_setArchivingDpConfig(dsChannelNames[i]); 
   //----set config for channel status --------------------  
       dyn_string dsChannelStatus = emuLvMRTN_getDynString("MRTN_Channel_Archiving_Status"); 
       for(int j=1;j<=dynlen(dsChannelStatus);j++)
        {
          emuLvMRTN_setDpSmoothingBool(dsChannelNames[i]+".Status."+dsChannelStatus[j]);     
        }
    } 
  //---config archiving for crate status--------------------
  dsCrateNames = dpNames(gSystemNameMRTN+"*Wiener*","FwWienerMarathon");  
  for(int k=1;k<=dynlen(dsCrateNames);k++)
    {
       dyn_string dsCrateStatus = emuLvMRTN_getDynString("MRTN_Crate_Archiving_Status"); 
       for(int l=1;l<=dynlen(dsCrateStatus);l++)
         { 
           emuLvMRTN_setDpSmoothingBool(dsCrateNames[k]+".Status."+dsCrateStatus[l]);
         } 
    }     
  //---config archiving for FSM------------------------- 
  dsFsmDevs = dpNames(gSystemNameMRTN+"*","_FwFsmDevice");
  if(dynlen(dsFsmDevs))
  {
    for(i=1;i<=dynlen(dsFsmDevs);i++)
    {
      emuLvMRTN_setFsmDpArchiving(dsFsmDevs[i]);
    }  
  }
  dsFsmObjs = dpNames(gSystemNameMRTN+"*","_FwFsmObject");
  if(dynlen(dsFsmObjs))
  {
    for(i=1;i<=dynlen(dsFsmObjs);i++)
    {
      emuLvMRTN_setFsmDpArchiving(dsFsmObjs[i]);
    }  
  }
  DebugTN("done for Lv MRTN archiving config");    
}
/**
* archiving smoothing:old/new comparison
* @param:string sDpName --> dpname to be configured
*/  
void emuLvMRTN_setDpSmoothingBool(string sDpName)
{
    dpSetWait (sDpName + ":_archive.._type",45,
               sDpName + ":_archive.._archive",1,
               sDpName + ":_archive.1._type",3,
               sDpName + ":_archive.1._class","_EVENT",
               sDpName + ":_archive.1._std_type",4); //  old/new comparison
}
/**
* create new channel name,mainly set two digit numbers for crate: crate1--> crate01
* @param:string sChannel --> marathon channel name
*/
void emuLvMRTN_setArchivingDpConfig(string sChannel)
{      
        dyn_string ds1 = strsplit(sChannel,":");
        dyn_string ds2 = strsplit(ds1[2],"/");
        int iCrateNumber = strltrim(ds2[3],"Crate");
        string sCrateNew,sChannelNew,sChannelOld;
        if ( iCrateNumber < 10)
         {
           sCrateNew = "Crate0"+iCrateNumber;
         }  
        else if (iCrateNumber > 9)
         { 
           sCrateNew = "Crate"+iCrateNumber; 
         }  
        sChannelNew = ds2[1]+"_"+sCrateNew+"_"+ds2[4];
        sChannelOld = ds1[2];
        dyn_string ds3 = makeDynString("Voltage","Current","Temperature");
        dyn_string ds4 = makeDynString("V","I","T");
        for(int i=1;i<=dynlen(ds3);i++)
         {
          string sChannel = sChannelNew+"_"+ds3[i];
       //---create new archiving dps-----------------------
          dpCreate(sChannel,"CSC_MARATON_"+ds4[i]+"_DATA");
       //---set dp smoothing-------------------------------
          dpSetWait (sChannel + ".value:_archive.._type",45,
                     sChannel + ".value:_archive.._archive",1,
                     sChannel + ".value:_archive.1._type",3,
                     sChannel + ".value:_archive.1._class","_EVENT",
                     sChannel + ".value:_archive.1._interv",0,
                     sChannel + ".value:_archive.1._interv_type",0,
                     sChannel + ".value:_archive.1._std_type",2,
                     sChannel + ".value:_archive.1._std_tol",fTolMarathon,
                     sChannel + ".value:_archive.1._std_time",iTimeMarathon);          
       //--set dp function-------------------------------
          string sDpNameLink; 
          if (ds3[i] == "Voltage")
            {     
              sDpNameLink = sChannelOld+".MeasurementSenseVoltage:_original.._value";
            }
          if (ds3[i] == "Current")
            {     
              sDpNameLink = sChannelOld+".MeasurementCurrent:_original.._value";
            } 
          if (ds3[i] == "Temperature")
            {     
              sDpNameLink = sChannelOld+".MeasurementTemperature:_original.._value";
            }        
          string p1 =  sDpNameLink;
          dyn_string ds=makeDynString(p1);   
          dpSetWait(sChannel+".value:_dp_fct.._type", 60, 
                    sChannel+".value:_dp_fct.._param", ds, 
                    sChannel+".value:_dp_fct.._fct", "p1"); 
        }          
} 
/**
 * Create all for LV MRTN
 */
void emuLvMRTN_createLvMRTNAll()
{
DebugTN("starting create all for CSC LV MRTN"); 
  emuLvMRTN_initalizeParam();
  delay(0.5);
  emuLvMRTN_createLvType();
  delay(0.5);
  emuLvMRTN_createHardware();
  delay(0.5);
  emuLvMRTN_createFsmTree();
  delay(0.5);
  fwFsmTree_generateAll(); 
  fwFsmTree_refreshTree();
  fwFsm_stopAllDomains();  
  delay(0.5);
  emuLvMRTN_setAlertConfig();
  delay(0.5);
  emuLvMRTN_setFsmUiInfo();
  delay(0.5);
  emuLvMRTN_setFsmUserData();
  delay(0.5);
  emuLvMRTN_setArchiving();
  DebugTN("done for CSC LV MRTN");
}  
