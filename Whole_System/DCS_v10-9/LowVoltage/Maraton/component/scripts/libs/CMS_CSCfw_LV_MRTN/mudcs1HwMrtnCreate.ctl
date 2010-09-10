mudcsMrtnHWcreateMain()
{
 string emu_side=mudcsMrtnPnlInit(); 
 mudcsMrtnHWcreate(emu_side);
 // mudcsLvDebug("Done");
}
//====================================================================
mudcsMrtnHWcreate(string emu_side){
_fwDeviceEditorNavigator_init(); // needed otherwise the configureDevice function does not work 
/*
  fwDevice_ADDRESS_TYPE = 1;
  fwDevice_ADDRESS_DRIVER_NUMBER=2; 
  fwDevice_ADDRESS_ROOT_NAME=3; 
  fwDevice_ADDRESS_OPC_SERVER_NAME=13; 
  fwDevice_ADDRESS_OPC_GROUP_IN=14; 
  fwDevice_ADDRESS_OPC_GROUP_OUT=15; 
*/    
dyn_string device;
dyn_string parentDevice; 
dyn_string exceptionInfo;
dyn_bool defaults=makeDynBool(TRUE,TRUE);
bool isOk;

int iBus, iCr, iCh, i, j;
dyn_dyn_string dsCratesIDs;
dyn_dyn_int dsCratesIDsInt;//=makeDynString();
 

//================================================================
//================================================================  

dyn_string dsCanBuses;

//------ DEFINITIONS ------------------------------------------------------------------------
  if(emu_side=="_P"){
dsCanBuses=makeDynString("CAN4","CAN5"); // CAN1 means the second socket 
dsCratesIDsInt[1]=makeDynString(1,2,7,8,9,10,15,16,18);
dsCratesIDsInt[2]=makeDynString(3,4,5,6,11,12,13,14,17,19);
//dsCratesIDsInt[3]=makeDynString(21,22,27,28,29,30,35,36,38);
//dsCratesIDsInt[4]=makeDynString(23,24,25,26,31,32,33,34,37);
  }
  else if(emu_side=="_M"){
dsCanBuses=makeDynString("CAN6","CAN7"); // CAN1 means the second socket 
//dsCratesIDsInt[1]=makeDynString(1,2,7,8,9,10,15,16,18);
//dsCratesIDsInt[2]=makeDynString(3,4,5,6,11,12,13,14,17);
dsCratesIDsInt[1]=makeDynString(21,22,27,28,29,30,35,36,38);
dsCratesIDsInt[2]=makeDynString(23,24,25,26,31,32,33,34,37);
  }
  else if(emu_side=="_DEV"){
dsCanBuses=makeDynString("CAN0");//,"CAN1"); // CAN1 means the second socket 
//dsCratesIDsInt[1]=makeDynString(1,2,7,8,9,10,15,16,18);
//dsCratesIDsInt[2]=makeDynString(3,4,5,6,11,12,13,14,17);
dsCratesIDsInt[1]=makeDynString(10);
//dsCratesIDsInt[2]=makeDynString(23,24,25,26,31,32,33,34,37);
  }  
  else if(emu_side==""){
dsCanBuses=makeDynString("CAN4","CAN5","CAN6","CAN7"); // CAN1 means the second socket 
dsCratesIDsInt[1]=makeDynString(1,2,7,8,9,10,15,16,18);
dsCratesIDsInt[2]=makeDynString(3,4,5,6,11,12,13,14,17);
dsCratesIDsInt[3]=makeDynString(21,22,27,28,29,30,35,36,38);
dsCratesIDsInt[4]=makeDynString(23,24,25,26,31,32,33,34,37);

if(CSC_fwCAN2_g_904_MACHINE && !CSC_fwCAN2_g_P5_SIM){
dsCanBuses=makeDynString("CAN0","CAN5","CAN6","CAN7"); // CAN1 means the second socket 
dsCratesIDsInt[1]=makeDynString(1,2,7,8,9,10,15,16,18);
dsCratesIDsInt[2]=makeDynString(3,4,5,6,11,12,13,14,17);
dsCratesIDsInt[3]=makeDynString(21,22,27,28,29,30,35,36,38);
dsCratesIDsInt[4]=makeDynString(23,24,25,26,31,32,33,34,37);  
  
}

  }  
  
//dsCratesIDsInt[1]=makeDynString(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20);
//dsCratesIDsInt[2]=makeDynString(21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40);
dyn_string dsChannels=makeDynString("0","1","3","4","5","7"); // CHECK THIS !!!!!!!!!!!!!
//-------------------------------------------------------------------------------------------

//dsCratesInt[3]=makeDynString();
//dsCratesInt[4]=makeDynString();
//--------------------------------------------
for(int i=1;i<=dynlen(dsCratesIDsInt);i++){
  for(int j=1;j<=dynlen(dsCratesIDsInt[i]);j++){
    dsCratesIDs[i][j]="Crate"+dsCratesIDsInt[i][j];
  }
}

for(iBus=1;iBus<=dynlen(dsCanBuses);iBus++){
//-------------------------------------------- 
//======== CAN BUS CREATION ==========================
/*
PVSS00ui1:2007.02.20 15:30:52.623["i=CAN2end"]
PVSS00ui1:2007.02.20 15:30:52.623["i=FwWienerCanBusend"]
PVSS00ui1:2007.02.20 15:30:52.623["i=end"]
PVSS00ui1:2007.02.20 15:30:52.623["i=Wiener CAN Busend"]

PVSS00ui1:2007.02.20 15:30:52.623["i=CMS_CSC_LV_CAN1:Wienerend"]
PVSS00ui1:2007.02.20 15:30:52.623["i=end"]
PVSS00ui1:2007.02.20 15:30:52.623["i=end"]
PVSS00ui1:2007.02.20 15:30:52.623["i=end"]
*/
if(CSC_fwCAN2_g_ISNEWOPCSERVER)
device=makeDynString(dsCanBuses[iBus],"FwWienerCanBus","","CAN Bus");//"Wiener CAN Bus");
else
device=makeDynString(dsCanBuses[iBus],"FwWienerCanBus","","Wiener CAN Bus");
  
parentDevice=makeDynString(CSC_fwCAN2_g_SYSTEM_NAME+":"+"Wiener","","",""); 
  
//device=makeDynString("","","","");
//parentDevice=makeDynString("","","","");

        DebugTN("+++++++++++++device "+device);
                DebugTN("+++++++++++++parentDevice "+parentDevice);
        for(i=1;i<=dynlen(device);i++){DebugTN("i="+device[i]+"end");}
        for(i=1;i<=dynlen(parentDevice);i++){DebugTN("i="+parentDevice[i]+"end");}        

fwDevice_checkAddress(device, makeDynString(fwDevice_ADDRESS_DEFAULT), isOk, exceptionInfo);

if(!isOk)DebugTN("FAULT !!!!!!!!!!!!!!!!!");
///else DebugTN("NOT FAULT !!!!!!!!!!!!!!!!!");
fwDevice_create(device, parentDevice, exceptionInfo);
device[1]=parentDevice[1]+"/"+device[1];
configureDevice(device, defaults, exceptionInfo);
//continue;

 for(iCr=1;iCr<=dynlen(dsCratesIDs[iBus]);iCr++){
//======== CRATE CREATION ============================
/*
PVSS00ui1:2007.02.20 15:33:26.622["i=Crate1end"]
PVSS00ui1:2007.02.20 15:33:26.622["i=FwWienerMarathonend"]
PVSS00ui1:2007.02.20 15:33:26.622["i=end"]
PVSS00ui1:2007.02.20 15:33:26.622["i=Wiener Marathon (CAN)end"]

PVSS00ui1:2007.02.20 15:33:26.622["i=CMS_CSC_LV_CAN1:Wiener/CAN2end"]
PVSS00ui1:2007.02.20 15:33:26.622["i=end"]
PVSS00ui1:2007.02.20 15:33:26.622["i=end"]
PVSS00ui1:2007.02.20 15:33:26.622["i=end"]
*/
 
if(CSC_fwCAN2_g_ISNEWOPCSERVER)   
device=makeDynString(dsCratesIDs[iBus][iCr],"FwWienerMarathon","","PL508 Crate (CAN)");//"PL508-DO Crate (CAN)");//"Wiener Marathon (CAN)");
else
device=makeDynString(dsCratesIDs[iBus][iCr],"FwWienerMarathon","","Wiener Marathon (CAN)");

parentDevice=makeDynString(CSC_fwCAN2_g_SYSTEM_NAME+":"+"Wiener/"+dsCanBuses[iBus],"","","");

        DebugTN("+++++++++++++device "+device);
                DebugTN("+++++++++++++parentDevice "+parentDevice);
        for(i=1;i<=dynlen(device);i++){DebugTN("i="+device[i]+"end");}
        for(i=1;i<=dynlen(parentDevice);i++){DebugTN("i="+parentDevice[i]+"end");}  

fwDevice_create(device, parentDevice, exceptionInfo);
device[1]=parentDevice[1]+"/"+device[1];
configureDevice(device, defaults, exceptionInfo);
//continue;
  for(iCh=1;iCh<=dynlen(dsChannels);iCh++){
//======== CHANNELS CREATION =========================  
 // if(!dynContains(dsChannels,(iCh-1)))continue;

/*
PVSS00ui1:2007.02.20 15:35:54.510["i=Channel2end"]
PVSS00ui1:2007.02.20 15:35:54.510["i=FwWienerMarathonChannelend"]
PVSS00ui1:2007.02.20 15:35:54.510["i=end"]
PVSS00ui1:2007.02.20 15:35:54.510["i=Marathon Channel (CAN)end"]

PVSS00ui1:2007.02.20 15:35:54.510["i=CMS_CSC_LV_CAN1:Wiener/CAN2/Crate1end"]
PVSS00ui1:2007.02.20 15:35:54.510["i=end"]
PVSS00ui1:2007.02.20 15:35:54.510["i=end"]
PVSS00ui1:2007.02.20 15:35:54.510["i=end"]
*/

if(CSC_fwCAN2_g_ISNEWOPCSERVER)    
device=makeDynString("Channel"+dsChannels[iCh],"FwWienerMarathonChannel","","PL508 Channel (CAN)");//"Marathon Channel (CAN)");
else
device=makeDynString("Channel"+dsChannels[iCh],"FwWienerMarathonChannel","","Marathon Channel (CAN)");
  
parentDevice=makeDynString(CSC_fwCAN2_g_SYSTEM_NAME+":"+"Wiener/"+dsCanBuses[iBus]+"/"+dsCratesIDs[iBus][iCr],"","","");
  

        DebugTN("+++++++++++++device "+device);
                DebugTN("+++++++++++++parentDevice "+parentDevice);
        for(i=1;i<=dynlen(device);i++){DebugTN("i="+device[i]+"end");}
        for(i=1;i<=dynlen(parentDevice);i++){DebugTN("i="+parentDevice[i]+"end");}  
        
fwDevice_create(device, parentDevice, exceptionInfo);
device[1]=parentDevice[1]+"/"+device[1];
configureDevice(device, defaults, exceptionInfo);
//====================================================
  } // for iCh
 } // for iCr
} // for iBus
}

//====================================================================



/*
Start of the can bus

	string sDpe = ".management";

// Executable Code
// ---------------
	// Action the command
	dpSet($sDpName + sDpe, iAction);
  
*/


//==============================================================================

configureDevice(dyn_string device, dyn_bool defaults, dyn_string &exceptionInfo)
{
  
DebugTN("device: "+device);
DebugTN("defaults: "+defaults);  
	// Set the addresses for the device	
	if(defaults[1])
	{
//		DebugN("Setting addresses for " + dpName);
//////////////		fwShowProgressBar("Setting hardware connection...");	
		fwDevice_setAddress(device[fwDevice_DP_NAME], 
							makeDynString(fwDevice_ADDRESS_DEFAULT), 
							exceptionInfo);
	}

	// Set the alerts for the device
	if(defaults[2])
	{
		//DebugN("Setting alarms for " + dpName);
///////////////	fwShowProgressBar("Setting alarms for...");
		fwDevice_setAlert(device[fwDevice_DP_NAME], fwDevice_ALERT_SET, exceptionInfo);
	}
	
/*		Default archiving disabled for now
		// Set the archiving for the device
		if(defaults[SELECTION_ARCHIVE])
		{
//			DebugN("Setting archiving for " + dpName);
			fwShowProgressBar("Setting archiving for...", 75);
			fwDevice_setArchive(device[fwDevice_DP_NAME], "", fwDevice_ARCHIVE_SET, exceptionInfo);
		}
*/
}

