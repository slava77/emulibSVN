mudcsLvCrbFsmPanelsAndFsmAliasesSetMain(string system_to_look = "")
{
  mudcsCrbPnlInit();
  mudcsLvCrbFsmPanelsAndFsmAliasesSet(system_to_look);
// mudcsLvDebug("Done");
}
//=================================================================
//=================================================================
//=================================================================
//=================================================================
//=================================================================
//=================================================================

mudcsLvCrbFsmPanelsAndFsmAliasesSet(string system_to_look) {



  dyn_string db_list, pcmb_mrtn;
  dyn_string db_split, pcmb_mrtn_split ;
  int db_coord;
  dpGet(system_to_look + "db_pcmb.list", db_list);
  dpGet(system_to_look + "pcmb_mrtn.list", pcmb_mrtn);


  string dir = "CMS_CSCfw_LV_CRB";
  string fwUiPnl;
//if(!mode)
  fwUiPnl = dir + "/emufwUi.pnl";
//else
// fwUiPnl="fwFSMuser/fwUi.pnl";




//=================================================================

  dyn_string os;
  string o, s, s1;
  int i, j, pos;
  dyn_string panels;
  string p;


  dyn_string d_string1, d_string2;
  string label, o_cut;
//-----------------------------------------------------------------

  dpSet(system_to_look + "fwOT_fwCrb_CSC_LV.panel",   dir + "/emuDevfwCrb_CSC_LVOperation.pnl");
//-----------------------------------------------------------------

  dyn_string objs = dpNames(system_to_look + "*", "_FwFsmDevice");

  for (i = 1; i <= dynlen(objs); i++) {

    d_string1 = strsplit(objs[i], "|");
    d_string2 = strsplit(d_string1[dynlen(d_string1)], "/");
    o_cut = d_string2[dynlen(d_string2)];

    panels = makeDynString(fwUiPnl);

//////DebugTN("OOOOOOOOOOOOO1 "+objs[i]);

//mudcsDebug(objs[i]);
    os = strsplit(objs[i], "|");
    o = os[dynlen(os)];

/////DebugTN("OOOOOOOOOOOOO2 "+o);
//o=mudcsAliasNameGet(o);
//////DebugTN("OOOOOOOOOOOOO3 "+o);
///////////DebugTN(mudcsAliasNameGet(o));


    s = "CRB";
    if ((pos = strpos(o, s)) >= 0) {
      p = "emuDev" + "fwCrb_CSC_LV" + "Operation.pnl";
      dynAppend(panels, dir + "/" + p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
      dpSet(objs[i] + ".ui.panels:_original.._value", panels);
      label = substr(o_cut, strlen("CSC_"));
      strreplace(label, "_LV_", "_");
      for (j = 1; j <= dynlen(db_list); j++) {
        db_split = strsplit(db_list[j], ";");
        if (strpos(label, db_split[1]) >= 0) {
          if (dynlen(db_split) >= 2) {

            pcmb_mrtn_split = strsplit(pcmb_mrtn[j-1], ";");

            sscanf(db_split[2], "%x", db_coord);
            //sprintf(s1,"%d",i1);
            //DebugTN(s1);
            label = label + "/MId " + pcmb_mrtn_split[2] + "/Id " + db_coord;

          }
        }
      }

      dpSet(objs[i] + ".ui.label:_original.._value", label);
      DebugTN(d_string1[dynlen(d_string1)] + " >> " + dir + "/" + p);
// fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );

      continue;
    }
// PSUBranch
    if (dpTypeName(system_to_look + d_string1[dynlen(d_string1)]) == "FwElmbPSUBranch") {// if((pos=strpos(o,s))>=0)
      p = "fwElmbPSUBranchDisplay.pnl";
      dynAppend(panels, "fwElmbPSU" + "/" + p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
      dpSet(objs[i] + ".ui.panels:_original.._value", panels);
//  label=substr(o_cut,strlen("CSC_")+6);
//  dpSet(objs[i]+".ui.label:_original.._value",label);
      DebugTN(d_string1[dynlen(d_string1)] + " >> " + dir + "/" + p);
// fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );

      continue;
    }

    /*
    // FwWienerMarathon
     if(dpTypeName(system_to_look+d_string1[dynlen(d_string1)]) == "FwWienerMarathon"){// if((pos=strpos(o,s))>=0)
    p="fwWienerMarathonOperation.pnl";
      dynAppend(panels,"fwWiener"+"/"+p);
    //  mudcsDebug(o+" "+panels[dynlen(panels)]);
      dpSet(objs[i]+".ui.panels:_original.._value",panels);
    //  label=substr(o_cut,strlen("CSC_")+6);
    //  dpSet(objs[i]+".ui.label:_original.._value",label);
     DebugTN(d_string1[dynlen(d_string1)]+" >> "+dir+"/"+p);
    // fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );

      continue;
     }

      if(dpTypeName(system_to_look+d_string1[dynlen(d_string1)]) == "FwWienerMarathonChannel"){// if((pos=strpos(o,s))>=0)
    p="fwWienerMarathonRCMChannelOperation.pnl";
      dynAppend(panels,"fwWiener"+"/"+p);
    //  mudcsDebug(o+" "+panels[dynlen(panels)]);
      dpSet(objs[i]+".ui.panels:_original.._value",panels);
    //  label=substr(o_cut,strlen("CSC_")+6);
    //  dpSet(objs[i]+".ui.label:_original.._value",label);
     DebugTN(d_string1[dynlen(d_string1)]+" >> "+dir+"/"+p);
    // fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );

      continue;
     }
    // dpTypeName(dsFsm[dynlen(dsFsm)
     */





  } // for

//==================================================================

  objs = dpNames(system_to_look + "*", "_FwFsmObject");
//dyn_string os;
//string o,s;
//int i, pos;
//dyn_string panels;
//string p;



  for (i = 1; i <= dynlen(objs); i++) {

    d_string1 = strsplit(objs[i], "|");
    d_string2 = strsplit(d_string1[dynlen(d_string1)], "/");
    o_cut = d_string2[dynlen(d_string2)];

    panels = makeDynString(fwUiPnl);


//mudcsDebug(objs[i]);
    os = strsplit(objs[i], "|");
    o = os[dynlen(os)];

////////DebugTN(o);

    s = "_CAN1";
    if ((pos = strpos(o, s)) >= 0 && pos == (strlen(o) - strlen(s))) {
//panels=makeDynString("fwFSMuser/fwUi.pnl");
      p = "emuEmptyOperation.pnl";  // ****** S was added !!!!!!!
      dynAppend(panels, dir + "/" + p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
      dpSet(objs[i] + ".ui.panels:_original.._value", panels);
      label = o_cut;
      dpSet(objs[i] + ".ui.label:_original.._value", label);
      continue;
    }

    s = "LV_CRB";
    if ((pos = strpos(o, s)) >= 0 && pos == (strlen(o) - strlen(s))) {
//panels=makeDynString("fwFSMuser/fwUi.pnl");
      p = "emuStationCrbOperation.pnl";
      dynAppend(panels, dir + "/" + p);
      dpSet(objs[i] + ".ui.panels:_original.._value", panels);
      label = substr(o_cut, strlen("CSC_"));
      dpSet(objs[i] + ".ui.label:_original.._value", label);
// fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );
      continue;
    }


    /*
    s="LV_MRTN";
    s1="_CR";
    if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))) {
      if(pos=strpos(o,s1)>=0){
    //panels=makeDynString("fwFSMuser/fwUi.pnl");
    //p="emuMRTNOperation.pnl";
      p="fwWienerMarathonOperation.pnl"; // just to define the sixe for fwUi.pnl
      dynAppend(panels,"fwWiener"+"/"+p);
      }
      else{
      p="emuMRTNOperation.pnl";  // ****** S was added !!!!!!!
      dynAppend(panels,dir+"/"+p);
      }

      dpSet(objs[i]+".ui.panels:_original.._value",panels);
       label=substr(o_cut,strlen("CSC_"));
      dpSet(objs[i]+".ui.label:_original.._value",label);
    // fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );
      continue;
     }
    */

    s = "_PSU";
    s1 = "_CR";
    if ((pos = strpos(o, s)) >= 0 && pos == (strlen(o) - strlen(s))) {
      if (pos = strpos(o, s1) >= 0) {
//panels=makeDynString("fwFSMuser/fwUi.pnl");
//p="emuMRTNOperation.pnl";
        p = "fwElmbPSUCrateDisplayS.pnl"; //
        dynAppend(panels, dir + "/" + p);
      } else {
        p = "emuEmptyOperation.pnl";  // ****** S was added !!!!!!!
        dynAppend(panels, dir + "/" + p);
      }

      dpSet(objs[i] + ".ui.panels:_original.._value", panels);
      label = substr(o_cut, strlen("CSC_"));
      dpSet(objs[i] + ".ui.label:_original.._value", label);
// fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );
      continue;
    }


  } //for

}
