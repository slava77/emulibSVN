// former initialize handler of fwFsmOperation.onl

int FIRST_CONTROLLED=0; // my_corr++
int FIRST_NEW_DP_CONNECTED=0; // my_corr++
int FIRST_MYWARNING=0; // my_corr++
main() // former initialize handler of fwFsmOperation.onl
{
//-------------------------------// my_corr++

time start_time=getCurrentTime();
dpSetWait("START_TIME.",start_time);


dyn_string exceptionInfo;



dyn_string dns_list;
int i;
string dns_command;

mudcsInit(); 

//system(CSC_fwG_g_project_name_home+"/bin/dns &");
system("dns &"); // is called form fwComponents/bin

dpGet("Db_o.DimServerComputerList",dns_list);
 for(i=1;i<=dynlen(dns_list);i++){
  if(strpos(dns_list[i],"smi") < 0 && strpos(dns_list[i],"uf") < 0){
   dns_command=  "rsh "+dns_list[i]+" -l fast '. .bash_profile; echo $DIM_DNS_NODE; dns &'";
   dns_command= dns_command+" &"; 
   system(dns_command);
  }
  
 }

dpSet(CSC_fwG_g_SYSTEM_NAME+":RUNCONTROL_FLAG.",0); // not allow RUN CONTROL OVER STARTING


mudcsDomainsStoring();

mudcsDimConfigAllManagers(exceptionInfo);
if(dynlen(exceptionInfo)>0){mudcsDebug(exceptionInfo);return;}

delay(5);
mudcsAllDimManagersStart();

dpConnect("update_csc_current_state",CSC_fwG_g_SYSTEM_NAME+":"+CSC_fwG_g_csc_part+"|"+CSC_fwG_g_csc_part+".fsm.currentState");
dpConnect("update_dpoints_not_controlled",CSC_fwG_g_SYSTEM_NAME+":DPOINTS_NOT_CONTROLLED.value");
///////dpConnect("update_new_dp_connected",CSC_fwG_g_SYSTEM_NAME+":NEW_DP_CONNECTED.value");
dpConnect("update_mywarning",CSC_fwG_g_SYSTEM_NAME+":MYWARNING.value");


delay(2);
system("PVSS00ctrlWEB -num 12 rs_http.ctl &");
DebugTN("----------------- START SERVERS -------------------");
DebugTN("----------------- START SERVERS -------------------");
DebugTN("----------------- START SERVERS -------------------");
DebugTN("----------------- START SERVERS -------------------");
system("start_servers &");


//------------------------------------------------------------
}
// ================== mycorr++ ==============================
update_mywarning(string dpName, string value){

 if(FIRST_MYWARNING>=1){
   if(value == "init_finish")
    mudcsDebug("START IS COMPLETE \n YOU CAN OPERATE");
 }


 FIRST_MYWARNING++;

}

// ================== mycorr++ ==============================
update_new_dp_connected(string dpName, string value){

 if(FIRST_NEW_DP_CONNECTED>=1){
    mudcsDebug(value);
 }
 FIRST_NEW_DP_CONNECTED++;

}

//============================================================  
update_dpoints_not_controlled(string dpName, string value){

dyn_string selected_list;

 if(FIRST_CONTROLLED>=2){
////////    mudcsDebug(value);
    mudcsScanTreeForDevices("GENERAL", CSC_fwG_g_csc_part, selected_list);
    getActiveDisksAndChambers(selected_list);


 }
 FIRST_CONTROLLED++;


}

//============================================================

update_csc_current_state(string dpName, string value){

int i;
int status;
dyn_string selected_list;

 if(value=="ERROR"){

   mudcsScanTreeForDevices("GENERAL", "", selected_list);
   for(i=1;i<=dynlen(selected_list);i++){
    dpGet(selected_list[i]+".status",status);
     if(status == -1)mudcsDebug("ERROR in \n"+ selected_list[i]);

   } // for   
  

 } // if

}
