mudcsPsuHWcreateMain()
{
  mudcsCrbPnlInit(0);
  mudcsPsuHWcreate();
//mudcsLvDebug("Done");
}


mudcsPsuHWcreate() {
  string   sUserName, sComment, sPort, sCard, sDpName, sNodeID;
  string   sSensorName, sBusName, sElmbName, sSensorType;
  string   sBus, sElmb;

  dyn_string dsExceptionInfo, dsIds, dsDefaultParameters;
  bool bDefaultAddressing;
  int iBusSpeed, iBit;

  dyn_string argdsExceptionInfo;



  int iBus, iE, iA, iD , i;
  int iCr, iBr;
  string sxE, sdE, siA, siD;
  string sxCr, sxBr;

  string sTemplate = "PSU";

//==== DEFINITIONS================================================

  int iNumberElmb = 127; // always: should be enouth big number


  dyn_string dsCanBusDef = makeDynString("LVCB_0");


  dyn_dyn_int ddsElmbIDs;
  dyn_dyn_int ddsCrateIDs;
//ddsElmbIDs[1];//=makeDynInt(63); // decimal IDs as they will be transfered to HEX below

  dyn_int BranchesPerCrate = makeDynInt(4); // bracnches per crate (i.e. per elmb)

//==================================
  dyn_string db_list;
  dyn_string ds1;
  dpGet(mudcsLvAddSystem("db_pcmb.list"), db_list);
  sTemplate = "PSU";

  for (i = 1; i <= dynlen(db_list); i++) {

    if (strpos(db_list[i], sTemplate) < 0)continue;
    ds1 = strsplit(db_list[i], ";");
    if (dynlen(ds1) < 2 || ds1[2] == "")continue;
    sscanf("0x" + ds1[3], "%x", iE);
    iCr = ds1[2];
    dynAppend(ddsElmbIDs[1], iE);
    dynAppend(ddsCrateIDs[1], iCr);
  }

//DebugTN(iE+" "+ds1[3]);
// DebugTN("0x"+ds1[3]);
// return;


//ddsElmbIDs[1]=makeDynInt(17,18); // decimal IDs as they will be transfered to HEX below

//ddsElmbIDs[2]=makeDynInt(15,21); // decimal IDs as they will be transfered to HEX below
//ddsElmbIDs[3]=makeDynInt(15,21); // decimal IDs as they will be transfered to HEX below
//ddsElmbIDs[4]=makeDynInt(15,21); // decimal IDs as they will be transfered to HEX below


  for (iBus = 1; iBus <= dynlen(dsCanBusDef); iBus++) {
//======================================================
    sUserName = dsCanBusDef[iBus];
    sComment = "";
    sPort = "";
    iBusSpeed = 125000;
    bDefaultAddressing = true;

    if (!dpExists(CSC_fwCAN1_g_SYSTEM_NAME + ":ELMB/" + dsCanBusDef[iBus]) )
      fwElmbUser_createCANbus(sUserName, sComment, sPort, sCard, iBusSpeed,
                              bDefaultAddressing, dsExceptionInfo);

    int iCr = 0;
//==============================
    for (iE = 1; iE <= iNumberElmb; iE++) {
//iE=18;
      if (!dynContains(ddsElmbIDs[iBus], iE))continue;
      iCr++;
//sprintf(sxCr,"%02x",iCr);
// sxCr=ddsCrateIDs[iBus][iCr];
      sprintf(sxCr, "%02d", ddsCrateIDs[iBus][iCr]);
      sprintf(sxE, "%02x", iE);

      sUserName = "ELMB_" + sxE;
      sComment = "";
      sDpName = CSC_fwCAN1_g_SYSTEM_NAME + ":ELMB/" + dsCanBusDef[iBus];  // ?????????????????
      sNodeID = iE;
      bDefaultAddressing = true;

      DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++ " + "ELMB_" + sxE);
      DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++ " + CSC_fwCAN1_g_SYSTEM_NAME + ":ELMB/" + dsCanBusDef[iBus]);
      DebugTN("++++++++++++++++++++++++++++++++++++++++++++++++++++++++ " + sNodeID);

      fwElmbUser_createElmb(sUserName, sComment, sDpName, sNodeID,
                            bDefaultAddressing, dsExceptionInfo);


//=====================================
//=====================================
//=====================================
//=====================================
//=====================================
//=====================================
// adding crate (one elmb <--> one crate)
      string argsCrateDp = "EPSU/Crate" + sxCr;
      string argsElmbDp = "ELMB/" + dsCanBusDef[iBus] + "/" + sUserName;
      string argsComment;

      DebugTN("----argsCrateDp " + argsCrateDp);
      DebugTN("----argsElmbDp " + argsElmbDp);
      DebugTN("----argsComment " + argsComment);

      fwElmbPSU_createCrate(
        argsCrateDp,
        argsElmbDp,
        argsComment,
        argdsExceptionInfo);

//PVSS00ui1:2007.07.02 17:27:05.902["----argsCrateDp EPSU/Crate0"]
//PVSS00ui1:2007.07.02 17:27:05.902["----argsElmbDp ELMB/LVCB_0/ELMB_3f"]
//PVSS00ui1:2007.07.02 17:27:05.902["----argsComment "]




// adding branches ==============================
      for (iBr = 0; iBr < BranchesPerCrate[iCr]; iBr++) {

        sprintf(sxBr, "%02x", iBr);

        string argsBranchDp = mudcsLvAddSystem(argsCrateDp + "/Branch" + sxBr);
        int argiBranchNumber = iBr;
        string argsBusDp;
        argsCrateDp = mudcsLvAddSystem(argsCrateDp);
        string argsComment;
        string argsLabel;

        DebugTN("argsBranchDp " + argsBranchDp);
        DebugTN("argiBranchNumber " + argiBranchNumber);
        DebugTN("argsBusDp " + argsBusDp);
        DebugTN("argsCrateDp " + argsCrateDp);
        DebugTN("argsComment " + argsComment);

        if (CSC_fwCAN1_g_IS_NEW_ATLAS_PSU) {
          fwElmbPSU_createBranch(
            argsBranchDp,
            argiBranchNumber,
            argsBusDp,
            argsCrateDp,
            argsComment,
            argsLabel,
            argdsExceptionInfo);
        } else {
          fwElmbPSU_createBranch(
            argsBranchDp,
            argiBranchNumber,
            argsBusDp,
            argsCrateDp,
            argsComment,
            argdsExceptionInfo);

        }

//PVSS00ui1:2007.07.02 17:27:41.166["argsBranchDp csc_atlas:EPSU/Crate0/Branch00"]
//PVSS00ui1:2007.07.02 17:27:41.166["argiBranchNumber 0"]
//PVSS00ui1:2007.07.02 17:27:41.166["argsBusDp "]
//PVSS00ui1:2007.07.02 17:27:41.166["argsCrateDp csc_atlas:EPSU/Crate0"]
//PVSS00ui1:2007.07.02 17:27:41.166["argsComment "]


      }
//===============================================

    } // for(iE=1;iE<=60;iE++)
//===============================================


  } // for iBus















}

//=====================================
/*
Start of the can bus

	string sDpe = ".management";

// Executable Code
// ---------------
	// Action the command
	dpSet($sDpName + sDpe, iAction);

*/
