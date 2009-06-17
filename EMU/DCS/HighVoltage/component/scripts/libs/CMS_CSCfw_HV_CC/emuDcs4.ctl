//------------------------------------
mudcsCheckDistConnections(){

dyn_int dist_connections;
int i,n, found;

while(1){
 dpGet("_DistConnections.Dist.ManNums",dist_connections);
 n=dynlen(dist_connections);

 delay(3);
 if(n >= 1){
 found=0;
  for(i=1;i <= n ;i++){
   if(dist_connections[i]==161 || dist_connections[i]==162 || dist_connections[i]==163 )
   found++;  // the LV project should be connected
  }
  if(found>=1){
   delay(15);
   break;
  }
 }
 DebugTN("NOT ENOUGH DISTANT CONNECTIONS: CHECK THE CHILD PROJECTS ARE RUNNING  ========================== "+ dist_connections);   
  
} // while

}
//-------------------------------------

mudcsSynchSystemPoratbleOS(string os, string command){

dyn_string ds1, ds2;
string s1, s2 ; 
int pos;
  if(os!="Linux"){
   strreplace(command,"/","\\"); 
   if( (pos=strpos(command,"\\."))>=0){
     
    ds1=strsplit(command," ");
    if(dynlen(ds1)>=3 && strpos(command,"cp ")>=0){
     if(dynlen(ds1)==3)ds2=strsplit(ds1[2],"\\");
     else ds2=strsplit(ds1[3],"\\");
     s2=ds2[dynlen(ds2)];
     if(strpos(s2,"*")>=0){
       strreplace(command,"\\"+s2,"/"+s2);       
       //strreplace(command,"\\.","\\");
     }
     else strreplace(command,"\\.","\\"+s2);     
    }
   }
        system("cmd /c "+command);
   //DebugTN(command);
  }
  else{
   system(command);    
  }
}


mudcsPanelsChamberInit(){
 

}
mudcsPanelsOutChamberInit(){


}
mudcsHVMasterAlertConfig()
{
 string operating_system; 
  operating_system=getenv("OS");
// the function is called in visual server to to alert config
// the masters in the chambers summary alerts:
// the script   mudcsCreateAlertHV.ctl is only called
// if a) not found a master in the chamber summary alert
//    b) and the chamber is assigned with some master (according to the HV_MASTER2CHAMBER)
  
  
  //mudcsInit();
  
  int i,j;
  dyn_string ds1=dpNames("*","HV_1_d");
  dyn_string ds2;
 
  string s1;
  int ms_found, found;
  dpGet("HV_MASTER2CHAMBER.",ds2);
   DebugTN(ds2);   
for(j=1;j<=dynlen(ds1);j++){
  
    dyn_string dps;  
  dpGet(ds1[j]+".:_alert_hdl.._dp_list",dps);
  if(dynlen(dps) <= 0){
    int a;

  }
  else{
 
    int ms_found=0;
    for(i=1;i<=dynlen(dps);i++){

      if((strpos(dps[i],"master")) >= 0){

        ms_found=1;
        break;
      } 
      
    } // for
    if(!ms_found){ // !
     found=0;
     for(i=1;i<=dynlen(ds2);i++){   
       if(strpos(ds2[i],mudcsRemoveSystem(ds1[j]))>=0){found=1;break;}
       //if(dynContains(ds2[i],mudcsRemoveSystem(ds1[j])) ){found=1;break;}
     } //for 
    
     if(found){
       if(operating_system=="Linux")
      system("PVSS00ctrl -num 15 CMS_CSCfw_HV_CC/mudcsCreateAlertHV.ctl &");
       else system("start /b PVSS00ctrl -num 15 CMS_CSCfw_HV_CC/mudcsCreateAlertHV.ctl");
      break;
//      DebugTN("1"); 
     }
     else{
//      DebugTN("2");       
     }
    } // if(!ms_found
  } //else
} // for j


//  return;
}
//==============================================================
int  mudcsGetMasterChannelStatus(string DpName){
  
 int i; 
 string master_channel;
 int ms_status;
    int ms_found=0; 
 
 dyn_string dps;  
  dpGet(DpName+".:_alert_hdl.._dp_list",dps);
  if(dynlen(dps) > 0){
  
    for(i=1;i<=dynlen(dps);i++){
      if((strpos(dps[i],"master")) >= 0){
        master_channel=dps[i];
        dpGet(mudcsAddSystem(master_channel),ms_status);

        ms_found=1;
      }
     
    } // for
   if(ms_found)return ms_status;
   else return -1;
    
  }
}
//===================================================================
mudcsInitServer(){
//  mudcsGetMasterForChamber(string fsm, int &master_id, int &master_chan, int &master_index, dyn_string &coord_master);
 
    addGlobal("CSC_fwG_so_primary_chambers",DYN_DYN_STRING_VAR);
    addGlobal("CSC_fwG_so_primary_fsm",DYN_STRING_VAR);
    addGlobal("CSC_fwG_so_get_chambers_time",DYN_TIME_VAR);
    addGlobal("CSC_fwG_so_chambers_fsm",DYN_STRING_VAR);    
  int i,j;
  
  string system_master;
  
  
  CSC_fwG_so_primary_fsm= dpNames("*:*","HV_PR");
  
   for(j=1;j<=dynlen(CSC_fwG_so_primary_fsm);j++){
      CSC_fwG_so_primary_chambers[j]=makeDynString();
   }   

  
  int master_id; int master_chan; int master_index; dyn_string coord_master;
  string master, primary;
  dyn_int coord_primary;
  
  dyn_string chambers_fsm= dpNames("*","HV_1");
  CSC_fwG_so_chambers_fsm=chambers_fsm;
    for(i=1;i<=dynlen(chambers_fsm);i++){

      
  mudcsGetMasterForChamber(chambers_fsm[i], master_id, master_chan, master_index, coord_master);
  system_master=substr(chambers_fsm[i],0,strpos(chambers_fsm[i],":" ));
  //DebugTN(chambers_fsm[i]);
 // DebugTN(master_id);
  //  DebugTN(master_chan);
   //   DebugTN(master_index);
    //    DebugTN(coord_master);
  master=system_master+":master"+coord_master[1]+"_"+coord_master[2]+"_"+coord_master[3];
   dpGet(master+".data.master_hostid",coord_primary[1]); 
  dpGet(master+".data.master_busaddr",coord_primary[2]);
  dpGet(master+".data.master_hvcard",coord_primary[3]);
  primary="HV_PR_primary"+coord_primary[1]+"_"+coord_primary[2]+"_"+coord_primary[3];
  
 // DebugTN(primary);
   for(j=1;j<=dynlen(CSC_fwG_so_primary_fsm);j++){
     if(strpos(CSC_fwG_so_primary_fsm[j], primary)>=0){
       dynAppend(CSC_fwG_so_primary_chambers[j],chambers_fsm[i]);
   // DebugTN("fsm:"+CSC_fwG_so_primary_fsm[j]);
  //      DebugTN("data:"+substr(primary,strpos(primary,":")+1));
      }
   }  //for
  
   
  } //for

    
}

//====================================================================
string mudcsGetPrimaryFsmForChamberFsm(string &fsm, int &primary_status, time &last_set_primary_status){
  int i,j;
  string data;
  
     for(j=1;j<=dynlen(CSC_fwG_so_primary_fsm);j++){
            for(i=1;i<=dynlen(CSC_fwG_so_primary_chambers[j]);i++){
       if(strpos(CSC_fwG_so_primary_chambers[j][i],fsm)>=0){
         data=substr(CSC_fwG_so_primary_fsm[j],0,strpos(CSC_fwG_so_primary_fsm[j],":")+1)+
                  substr(CSC_fwG_so_primary_fsm[j],strpos(CSC_fwG_so_primary_fsm[j],"/")+1) ;
        // mudcsDebug(data);
          dpGet(data+".status",primary_status);
          dpGet(data+".status:_online.._stime",last_set_primary_status);

          
          
         return  CSC_fwG_so_primary_fsm[j];
       } 
            }
     }
  
}
//====================================================================
dyn_string mudcsGetChambersFsmForPrimaryFsm(string &fsm){
    int i,j;
     for(j=1;j<=dynlen(CSC_fwG_so_primary_fsm);j++){
       if(strpos(CSC_fwG_so_primary_fsm[j],fsm)>=0){
         return  CSC_fwG_so_primary_chambers[j];
       } 
       
     }  
  
}
//=====================================================================
mudcsSetRampUpRateForAllChambers(string primary_fsm, int rate){
   int i;
   dyn_string ds1= mudcsGetChambersFsmForPrimaryFsm(primary_fsm);   
    DebugTN(ds1);
    for(i=1;i<=dynlen(ds1);i++){
       mudcsHVCommand(ds1[i], 6, rate);
     }      
}
     
