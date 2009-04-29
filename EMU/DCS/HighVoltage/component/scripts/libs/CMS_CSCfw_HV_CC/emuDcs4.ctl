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
