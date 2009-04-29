

mudcsArchiveMain(bool isArchiving)
{
  
  mudcsInit();
  
  int i,j, i10;
dyn_string exceptionInfo;
string dp;
int number_per_HV_channel;
int ch_first, ch_last;
 string device_type;
 dyn_string dp_names, device_types;
 dyn_string dp_elements;
 
dynClear(device_types);
dynAppend(device_types,"fwWnrCh_CSC_LV");
dynAppend(device_types,"HV_1");
dynAppend(device_types,"LV_1");
dynAppend(device_types,"TEMP_1");
//dynAppend(device_types,"");
//dynAppend(device_types,"");
//dynAppend(device_types,"");


dyn_string cr_dpe=makeDynString();
dyn_string ch_dpe=makeDynString("meas_volt","meas_cur");



//------------------------------------------------------


 

dyn_string value_seq_dyn, delta_v;
float v;
int time_interval;

// commented out : only general configuration is possible
///if(strpos($sDpe,"LV_1") >=0 || device_type == "LV_1"){
///device_type= "LV_1";
///}

DebugTN(device_types);
for(i10=1;i10<=dynlen(device_types);i10++){
  device_type=device_types[i10];
  
//--------------- special align-----------------------------------------------
if(device_type == "ALIGN_1"){

dyn_string al_dpnames=dpNames("*_ALNM.data.item*");
dyn_string al_items;

v=1;
time_interval=7200;

for(i=1;i<dynlen(al_dpnames);i++){
///fwArchive_setMultiple(makeDynString(dpnames[i]),"RDB-99) EVENT",15,0,0,0,exceptionInfo);
  al_items=strsplit(al_dpnames[i],".");
  dynAppend(value_seq_dyn, al_items[dynlen(al_items)]);dynAppend(delta_v, v);
}

}
//-----------------------------------------------------------------------------
else if(device_type == "fwWnrCr_CSC_LV"){


}
//-----------------------------------------------------------------------------
/*
else if(device_type == "fwWnrCr_CSC_LV"){

v=1;
time_interval=7200;
for(i=1;i<=dynlen(cr_dpe);i++){
 dynAppend(value_seq_dyn,   "."+cr_dpe[i]);dynAppend(delta_v, v); 
}

}
*/
//-----------------------------------------------------------------------------
else if(device_type == "fwWnrCh_CSC_LV"){

  dp_names=dpNames("*",device_types[i10]);
v=1;
time_interval=7200;
for(i=1;i<=dynlen(ch_dpe);i++){
 dynAppend(value_seq_dyn,   "."+ch_dpe[i]);dynAppend(delta_v, v);
 DebugTN(value_seq_dyn[i]);
}

}
//-----------------------------------------------------------------------------

else if(device_type == "HV_1"){


  dp_names=dpNames("*",device_types[i10]);
time_interval=7200;
  
 for(i=1;i<=36;i++){
  v=1; 
     dynAppend(value_seq_dyn, ".data.v"+i+".imon");dynAppend(delta_v, v);
  v=20;   
     dynAppend(value_seq_dyn, ".data.v"+i+".vmon");dynAppend(delta_v, v);     
 } // for
//======================= 


 
number_per_HV_channel=2;
}
else if(device_type == "LV_1"){

  dp_names=dpNames("*",device_types[i10]);
v=0.1;
time_interval=7200;

                 dynAppend(value_seq_dyn,  ".data.Alct_o.v18");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Alct_o.v33");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Alct_o.v55");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Alct_o.v56");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Alct_o.c18");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Alct_o.c33");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Alct_o.c55");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Alct_o.c56");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Alct_o.status");dynAppend(delta_v, v);

                 dynAppend(value_seq_dyn,  ".data.Cfeb_o.v33");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Cfeb_o.v50");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Cfeb_o.v60");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Cfeb_o.c33");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Cfeb_o.c50");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Cfeb_o.c60");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Cfeb_o.status");dynAppend(delta_v, v);

}
else if(device_type == "TEMP_1"){

  dp_names=dpNames("*",device_types[i10]);
v=1;
time_interval=7200;
  
                 dynAppend(value_seq_dyn,   ".data.t_dmb.v1");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.t_alct.v1");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.t_cfeb.v1");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.t_cfeb.v2");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.t_cfeb.v3");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.t_cfeb.v4");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.t_cfeb.v5");dynAppend(delta_v, v);
}
else if(device_type == "GAS_SX5"){

  dp_names=dpNames("*",device_types[i10]);
v=1;
time_interval=7200;  
             //    ".data.Mixer1_o.ar"
             //    ".data.Mixer1_o.co2"
                 dynAppend(value_seq_dyn,  ".data.Mixer2_o.ar");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Mixer2_o.co2");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.Mixer2_o.cf4");dynAppend(delta_v, v);
}
else if(device_type == "MRTN_1"){

  dp_names=dpNames("*",device_types[i10]);
v=0.1;
time_interval=7200;  
                 dynAppend(value_seq_dyn,  ".data.channel1.item1");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.channel1.item2");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.channel2.item1");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.channel2.item2");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.channel3.item1");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.channel3.item2");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.channel4.item1");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.channel4.item2");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.channel5.item1");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.channel5.item2");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.channel6.item1");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.channel6.item2");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.crate.item1");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.crate.item2");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.crate.item3");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.crate.item4");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.crate.item5");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.crate.item6");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.crate.item7");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.crate.item8");dynAppend(delta_v, v);
                 dynAppend(value_seq_dyn,  ".data.crate.item9");dynAppend(delta_v, v);

//mudcsDebug("m");

 //                +".data.Mixer2_o.cf4";
}


//------------------------------------------------------
// DebugTN(device_type+"===="+dynlen(dp_names));
 
 // getValue("isArchiving","state",0,isArchiving);
 
 
 
for(i=1;i<=dynlen(dp_names);i++){
 if(strpos(dp_names[i],"DimBroker") >=0 )continue;
     if(device_type == "HV_1"){
      mudcsArchive_get_channels_in_module(dp_names[i], number_per_HV_channel, ch_first, ch_last);
        DebugTN("=="+dp_names[i]);
        DebugTN("first "+ch_first);
        DebugTN("last "+ch_last);
     }     
 for(j=1;j<=dynlen(value_seq_dyn);j++){
  dp=dp_names[i]+value_seq_dyn[j];
  //DebugTN("=="+dp);
  if(!isArchiving){
     fwArchive_deleteMultiple(makeDynString(dp), exceptionInfo);
   }
  else{
    if(device_type == "HV_1"){
      if((j) < ch_first || (j) > ch_last)continue;
    }  
    
     DebugTN(dp); 
    
     dpSetWait(dp+":_archive.._type",45, 
          dp+":_archive.._archive", 1,
          dp+":_archive.1._type", 3,                 //here you say you are using smoothing
          dp+":_archive.1._class", "_EVENT",  
          dp+":_archive.1._interv", 0,              
          dp+":_archive.1._interv_type", 0,
          dp+":_archive.1._std_type", 2,        //type of standard smoothing procedure (value/time/comb ...)
          dp+":_archive.1._std_tol", delta_v[j],       //here I select the value deadband 
          dp+":_archive.1._std_time", time_interval       //here the time deadband
    );
  
   } //else  
 }
} // for(i=1;i<=dynlen(dp_names);i++)

//---------------

//---------------
	if(dynlen(exceptionInfo)>0)
		fwExceptionHandling_display(exceptionInfo);
        
      } // for i10
}

//======================================================

mudcsArchive_get_channels_in_module(string &dp, int number_per_HV_channel, int &ch_first, int &ch_last){

string setList;
int  w_pos_return, radius_return, ich_return;
string type_return,set_return, mudcs_alias ;
dyn_string coords;

setList="HVsetList";

 mudcs_alias=mudcsAliasNameGet(dp);
 mudcsNameToPosConvert(mudcs_alias, w_pos_return, radius_return, ich_return, type_return);
 retrieveCoordinateSet("", dp, setList , w_pos_return, radius_return, ich_return, type_return, set_return);

 coords=strsplit(set_return,";");   
 
  if(dynlen(coords)<4){
   if(strpos(dp,"master") < 0){
    ch_first=0; 
    ch_last=0;
   }
   else{
    ch_first=1; 
    ch_last=8*number_per_HV_channel;
   }
  }
  else if(coords[4] ==0){
   ch_first=1; 
   ch_last=30*number_per_HV_channel;
  }
  else if(coords[4] ==1){
   ch_first=1; 
   ch_last=18*number_per_HV_channel;


  }
  else if(coords[4] ==2){
   ch_first=18*number_per_HV_channel+1;
   ch_last=36*number_per_HV_channel;

  }

}
