mudcsCrbNodesConfigAccording2DbMain()
{
  mudcsCrbPnlInit();
  mudcsCrbNodesConfigAccording2Db();
//  mudcsLvDebug("Done");
}

mudcsCrbNodesConfigAccording2Db() {
  dyn_string list, ds1, ds2, ds3 , ds4, ds5;
  int i, j;
  string fsm, sId, s1;

  dpGet(mudcsLvAddSystem("db_pcmb.list"), list);

  for (i = 1; i <= dynlen(list); i++) {
//DebugTN("====="+list[i]+" "+sTemplate);
    if (strpos(list[i], "PSU") >= 0)continue;


    //-----------------------------------------------

    ds1 = strsplit(list[i], ";");
    if (dynlen(ds1) < 2 || ds1[2] == "")continue;

    CSC_fwCAN1_g_EmuCmsGlobalNode = "CRB/CSC_ME_" + ds1[1] + "_LV_CRB";

    dpSetWait(CSC_fwCAN1_g_EmuCmsGlobalNode + ".coord", ds1[2]);
    /*
     ds2=dpNames("*ELMB_"+ds1[2]+"*", "FwElmbAi");

     for(j=1;j<=dynlen(ds2);j++){

     ds4=strsplit(ds2[j],"/");
     sTemplate2=ds4[dynlen(ds4)];

     } // for j
    //-------------------
    */

    ds2 = dpNames("*ELMB_" + ds1[2] + "*", "FwElmbDo");
    ds3 = makeDynString();

    for (j = 1; j <= dynlen(ds2); j++) {
      dynAppend(ds3, ds2[j]);
    } // for j

    dpSetWait(CSC_fwCAN1_g_EmuCmsGlobalNode + ".switch_list", ds3);
//===================================
// else i.e. ELMB od crb

  } // for i
//--------------------

// ------------- PSU part --------------

  mudcsCrbPnlInit(); // call again  tp get CRB2ELMB global

  DebugTN(CSC_fwCAN1_g_CRB2ELMB);

  ds1 = dpNames("*", "FwElmbNode");
  for (i = 1; i <= dynlen(ds1); i++) {
    ds2 = strsplit(ds1[i], "_");
    sId = ds2[dynlen(ds2)];

    fsm = mudcsCrbGetCrbByElmbId(sId);
    if (fsm == "") {// case of elmb of PSU (i.e. not elmb of CRB)
      fsm = ds1[i];
      strreplace(fsm, "/", "_");
      dpCreate(mudcsLvRemoveSystem(fsm), "fwPsu_CSC_LV");
      dpSetWait(fsm + ".coord", ds1[i]);
    }


  } // for

//---
  ds1 = dpNames("*", "FwElmbPSUBranch");
  for (i = 1; i <= dynlen(ds1); i++) {
    dpGet(ds1[i] + ".power", s1);
    dpSet(ds1[i] + ".power", mudcsLvAddSystem(s1));
    dpGet(ds1[i] + ".CAN.current", s1);
    dpSet(ds1[i] + ".CAN.current", mudcsLvAddSystem(s1));
    dpGet(ds1[i] + ".CAN.volts", s1);
    dpSet(ds1[i] + ".CAN.volts", mudcsLvAddSystem(s1));
    dpGet(ds1[i] + ".AD.current", s1);
    dpSet(ds1[i] + ".AD.current", mudcsLvAddSystem(s1));
    dpGet(ds1[i] + ".AD.volts", s1);
    dpSet(ds1[i] + ".AD.volts", mudcsLvAddSystem(s1));

  }





}


//========================================================
string mudcsCrbGetCrbByElmbId(string sId) {

//  DebugTN("########"+sId);
  int i;
  dyn_string ds1;
  for (i = 1; i <= dynlen(CSC_fwCAN1_g_CRB2ELMB); i++) {
    ds1 = strsplit(CSC_fwCAN1_g_CRB2ELMB[i], ";");
    if (ds1[2] == sId)return ds1[1];
  }
  return ""; // case elmb of PSU (i.e. not elmb of CRB)
}
//========================================================
