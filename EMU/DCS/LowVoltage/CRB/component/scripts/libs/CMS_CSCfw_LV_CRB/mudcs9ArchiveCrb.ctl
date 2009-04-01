


mudcsArchiveCrbMain(bool isArchiving = true) {

  string device_type;
  dyn_string device_types;
  dyn_string dp_names;
  dyn_string dp_elements;

  device_type = "Elmb";
  dynClear(device_types);
  dynAppend(device_types, "FwElmbAi");


  int i, j, i10;
  dyn_string exceptionInfo, dps;
  string dp;

//------------------------------------------------------


//	g_busyThread = -1;
//	fwBusyBarButton_std_startBusy(exceptionInfo);


  dyn_string value_seq_dyn, delta_v;
  float v;
  int time_interval;

// commented out : only general configuration is possible
///if(strpos($sDpe,"LV_1") >=0 || device_type == "LV_1"){
///device_type= "LV_1";
///}

//--------------- special align-----------------------------------------------

  DebugTN(device_types);
  for (i10 = 1; i10 <= dynlen(device_types); i10++) {

    dynClear(value_seq_dyn);
//-----------------------------------------------------------------------------
    if (device_types[i10] == "FwWienerMarathonChannel") {

      v = 1;
      time_interval = 7200;
      dp_names = dpNames("*", device_types[i10]);
      dynAppend(value_seq_dyn,   ".MeasurementSenseVoltage");
      dynAppend(delta_v, v);
      dynAppend(value_seq_dyn,  ".MeasurementCurrent");
      dynAppend(delta_v, v);
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    else if (device_types[i10] == "FwWienerMarathon") {

      v = 1;
      time_interval = 7200;
      dp_names = dpNames("*", device_types[i10]);
      dynAppend(value_seq_dyn,   ".Status.CrateStatus");
      dynAppend(delta_v, v);

    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

    else if (device_types[i10] == "FwElmbAi") {

      v = 1;
      time_interval = 7200;
      dp_names = dpNames("*", device_types[i10]);
      dynAppend(value_seq_dyn,   ".value");
      dynAppend(delta_v, v);

    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------



//------------------------------------------------------
// DebugTN(device_type+"===="+dynlen(dp_names));

//  getValue("isArchiving","state",0,isArchiving);



    for (i = 1; i <= dynlen(dp_names); i++) {
      if (strpos(dp_names[i], "DimBroker") >= 0 )continue;

      for (j = 1; j <= dynlen(value_seq_dyn); j++) {
        dp = dp_names[i] + value_seq_dyn[j];
        //DebugTN("=="+dp);
        if (!isArchiving) {
          fwArchive_deleteMultiple(makeDynString(dp), exceptionInfo);
        } else {

          DebugTN(dp);
          dpSetWait(dp + ":_archive.._type", 45,
                    dp + ":_archive.._archive", 1,
                    dp + ":_archive.1._type", 3,               //here you say you are using smoothing
                    dp + ":_archive.1._class", "_EVENT",
                    dp + ":_archive.1._interv", 0,
                    dp + ":_archive.1._interv_type", 0,
                    dp + ":_archive.1._std_type", 2,      //type of standard smoothing procedure (value/time/comb ...)
                    dp + ":_archive.1._std_tol", delta_v[j],     //here I select the value deadband
                    dp + ":_archive.1._std_time", time_interval     //here the time deadband
                   );

        } //else
      }
    } // for(i=1;i<=dynlen(dp_names);i++)

//---------------
//fwBusyBarButton_std_stopBusy(exceptionInfo);
//---------------
    if (dynlen(exceptionInfo) > 0)
      fwExceptionHandling_display(exceptionInfo);

  } // for i10

}
