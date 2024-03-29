#uses "aes.ctl"
#uses "CMS_CSC_common/emu_alert.ctl"
#uses "CMS_CSC_common/emu_common.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs2.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs3.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcs4.ctl"
#uses "CMS_CSCfw_HV_CC/emuDcsDim.ctl"
#uses "CMS_CSCfw_HV_CC/emuAlert.ctl"
#uses "CMS_CSCfw_HV_CC/emufw.ctl"





string DpName;

main()
{
  
  mudcsInit();
  
  DpName=CSC_fwG_g_csc_part;
      
  HV_1_alert_main();
     mudcsAlertReconfig("HV_1", ".off_channels", true);
  emu_info("Updating all alert classes");
  emuAlert_updateAllAlertClasses();
  emu_info("Done updating alert classes");     
}










//==========================================================

HV_1_alert_main(){



string device_type="HV_1";
  hv_set_d(true,device_type);
//string device_type1="HV_PR";
//  hv_set_pr(true,device_type1);


}
//==========================================================



hv_set_d(bool isSet, string device_type)
{

int i;

dyn_string limits_s;
string value;
string s1,s2;

limits_s = makeDynString();

// s1 is OK
// s2 is ALARM

s1="0";
s2="4";

//	HV_STATE_ILOCK		= 5,
//	HV_STATE_INT_TRIP	= 6,
//	HV_STATE_CHAN_TRIP	= 7,

for(i=1;i<=36;i++){

value=value+".data.v"+i+".status|";
//// if(i<36)value=value+".data.v"+i+".status|";
//// else value=value+".data.v"+i+".status"; 

 dynAppend(limits_s,s1+"|"+s2);

//-----------------------------------------------
/*
 // not necessary: if we switch on the channel it bacomes DEAD, also the master channel is DEAD 
 
//if(i>=19)continue; // this is pack data (two channels in one value) 
 value+=".data.pack_data.v"+i+".vset|"; // to set alarm if somebody switchs off (or power cycle) the LVPS (Wiener LVPC)

s1="1";   //  ok
s2="5000";   // alarm 

 dynAppend(limits_s,s1+"|"+s2);

*/ 
//------------------------------------------------  
 
}

value=value+".data.module_status|"; 
dynAppend(limits_s,"-2|5");  // -2 is OK
                            // 5 is alarm

value=value+".data.module_type"; 
dynAppend(limits_s,"1|4");  // 1 is OK
                            // 4 is alarm

mudcsLookAndAlertArchiveConfig(true, 
false, isSet, device_type, value, limits_s, DpName, "",0,0,0.,0.);




}

//==========================================================

//=========================================================


//==========================================================
//==========================================================
//==========================================================
