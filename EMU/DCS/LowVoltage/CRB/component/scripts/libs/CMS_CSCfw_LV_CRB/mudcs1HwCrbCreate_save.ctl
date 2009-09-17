
mudcsCrbHWcreateMain()
{
 mudcsCrbPnlInit(0); 
 mudcsCrbHWcreate();
 //mudcsLvDebug("Done");
}

mudcsCrbHWcreate(){
string   sUserName,sComment,sPort,sCard, sDpName,sNodeID;
string   sSensorName,sBusName,sElmbName,sSensorType;
string   sBus,sElmb;

dyn_string dsExceptionInfo, dsIds, dsDefaultParameters;
bool bDefaultAddressing;
int iBusSpeed,iBit;

   

int iBus, iE, iA, iD ,i;
string sxE, siA, siD;
//==== DEFINITIONS================================================

int iNumberElmb=127; // always: should be enouth big number
int iNumberAi=64; // 64 always
int iNumberDCo=8; // 8 always
int iNumberDAo=5;  // 2 always

dyn_string dsCanBusDef=makeDynString("LVCB_1","LVCB_0","LVCB_2","LVCB_3");

dyn_dyn_int ddsElmbIDs;

// M-4 37 38 39 3a 3b 3c
// M-3 31 32 33 34 35 36
// M-2 2b 2c 2d 2e 2f 30
// M-1 20 21 | 23 22 | 25 24 | 27 26 | 28 29 | 2a 1f
// P+1 2 3 | 5 4 | 7 6 | 9 8 | 0a 0b | 0c 1
// P+2 0d 0e 0f 10 11 12
// P+3 13 14 15 16 17 18
// P+4 19 1a 1b 1c 1d 1e

ddsElmbIDs[1]=makeDynInt(4,5,6,7,8,9,14,15,16,20,21,22,26,27,28); // decimal IDs as they will be transfered to HEX below
ddsElmbIDs[2]=makeDynInt(1,2,3,10,11,12,13,17,18,19,23,24,25,29,30);
ddsElmbIDs[3]=makeDynInt(31,32,33,40,41,42,43,47,48,49,53,54,55,59,60);
ddsElmbIDs[4]=makeDynInt(34,35,36,37,38,39,44,45,46,50,51,52,56,57,58);  
//ddsElmbIDs[1]=makeDynInt(17,18); // decimal IDs as they will be transfered to HEX below

//ddsElmbIDs[2]=makeDynInt(15,21); // decimal IDs as they will be transfered to HEX below
//ddsElmbIDs[3]=makeDynInt(15,21); // decimal IDs as they will be transfered to HEX below
//ddsElmbIDs[4]=makeDynInt(15,21); // decimal IDs as they will be transfered to HEX below

dyn_string dsPcmbValidChannels=makeDynString();
for(i=0;i<=21;i++)dynAppend(dsPcmbValidChannels,i);
for(i=48;i<=57;i++)dynAppend(dsPcmbValidChannels,i);
for(i=32;i<=35;i++)dynAppend(dsPcmbValidChannels,i);
//for(i=;i<=;i++)dynAppend(dsPcmbValidChannels,i);
//for(i=;i<=;i++)dynAppend(dsPcmbValidChannels,i);

for(iBus=1;iBus<=dynlen(dsCanBusDef);iBus++){
//======================================================
sUserName=dsCanBusDef[iBus];
sComment="";
sPort="";
iBusSpeed=125000;
bDefaultAddressing=true;

if(!dpExists(CSC_fwCAN1_g_SYSTEM_NAME+":ELMB/"+dsCanBusDef[iBus]) )     
  	fwElmbUser_createCANbus(sUserName, sComment, sPort, sCard, iBusSpeed,
			        bDefaultAddressing, dsExceptionInfo);
        

//==============================
  for(iE=1;iE<=iNumberElmb;iE++){
//iE=18;
 if(!dynContains(ddsElmbIDs[iBus],iE))continue;
           
sprintf(sxE,"%02x",iE);
                    
sUserName="ELMB_"+sxE;  
sComment="";
sDpName=CSC_fwCAN1_g_SYSTEM_NAME+":ELMB/"+dsCanBusDef[iBus];  // ?????????????????
sNodeID=iE;
bDefaultAddressing=true;
 DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++ "+"ELMB_"+sxE); 
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++ "+CSC_fwCAN1_g_SYSTEM_NAME+":ELMB/"+dsCanBusDef[iBus]); 
DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++ "+sNodeID); 

	fwElmbUser_createElmb(sUserName, sComment, sDpName, sNodeID, 
	                      bDefaultAddressing, dsExceptionInfo);

//=====================================
//==================================

        for(iA=0;iA<iNumberAi;iA++){     

if(dynContains(dsPcmbValidChannels,iA) <=0 )continue;

                         
 sprintf(siA,"%d",iA);           
sSensorName="ai_"+siA; 
sBusName=dsCanBusDef[iBus];
sElmbName="ELMB_"+sxE;
sComment="";
dsIds=makeDynString(siA);
sSensorType="Raw ADC Value"; // ?????????????????
dsDefaultParameters==makeDynString();
bDefaultAddressing=true;

DebugTN("iA="+sSensorName);
                
fwElmbUser_createSensor(sSensorName, sBusName, sElmbName, sComment, dsIds, sSensorType,
			dsDefaultParameters, bDefaultAddressing, dsExceptionInfo);
														  
			// Check for any errors
        } // for iA
//===============================================
        
         for(iD=0;iD<iNumberDCo;iD++){           
         sprintf(siD,"%d",iD); 
        
sBus=dsCanBusDef[iBus];
sElmb="ELMB_"+sxE;
sComment="";
sPort="C";
iBit=iD;
bDefaultAddressing=true;

DebugTN(sBus);
DebugTN(sElmb);
DebugTN(sComment);
DebugTN(sPort);
DebugTN(iBit);
DebugTN(bDefaultAddressing);


	fwElmbUser_createDigital(sBus, sElmb, sComment, sPort, iBit,
			          false, bDefaultAddressing, dsExceptionInfo); 
         } // for iD
//==========================================
                  
         for(iD=0;iD<iNumberDAo;iD++){           
         sprintf(siD,"%d",iD); 
        
sBus=dsCanBusDef[iBus];
sElmb="ELMB_"+sxE;
sComment="";
sPort="A";
iBit=iD;
bDefaultAddressing=true;

	fwElmbUser_createDigital(sBus, sElmb, sComment, sPort, iBit,
			          false, bDefaultAddressing, dsExceptionInfo); 
         } // for iD         
         
         
   } // for(iE=1;iE<=60;iE++)
//===============================================
       
} // for iBus 



}


/*
Start of the can bus

	string sDpe = ".management";

// Executable Code
// ---------------
	// Action the command
	dpSet($sDpName + sDpe, iAction);
  
*/
