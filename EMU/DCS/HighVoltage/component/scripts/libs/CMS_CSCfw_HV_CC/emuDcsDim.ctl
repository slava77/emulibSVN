

//=============================
mudcsDimConfigOneManagerNew(string type_par, string pc, string manager, dyn_string &exceptionInfo){

 int j;
 dyn_string BrokerList;
 dyn_string dps;
 dyn_string services=makeDynString();
 dyn_string commands=makeDynString();
 
 string service,update_fun,command;
 string test_string;
 int pos;
 dyn_string list;

 int i, i_ch;
 dyn_string test_dyn;
 string pc_id;
 string coord_1, coord_2;
 dyn_string coord;
 dyn_string s_split;


     int i_pc; // to configure primaries only for 1-st dim manager 
//mudcsDebug(pc);


 
 BrokerList=dpNames("*",type_par+"_d");
 
   for(j=1; j<= dynlen(BrokerList); j++){
    command="no_command";
 
    if(strpos(BrokerList[j],"Broker")>=0)service="no_service"; // because Brokers are configured via mudcsDimConfigOneManager()
//--------------------- start of master --------------------------------
    else if(strpos(BrokerList[j],"WienerSnmp")>=0){ 
      string prefix;
      s_split=strsplit(BrokerList[j],"/");
      coord_1=substr(s_split[1],strpos(s_split[1],"Crate")+strlen("Crate"));
      
//---
     pc_id="";
     for(i=1;i<=dynlen(g_HV_ID2PC_NAME);i++){
      test_dyn=strsplit(g_HV_ID2PC_NAME[i],";");
      if(test_dyn[2]==pc){pc_id=test_dyn[1];i_pc=i;break;} 
     }  
//---
//mudcsDebug(coord_1+">>"+pc_id);
      if(pc_id==""){service="no_service";command="no_command";}
      else if(pc_id != "all" && (strpos(pc_id,coord_1)<0)){service="no_service";command="no_command";}
      else {    
//---      
     if(i_pc==dynlen(g_HV_ID2PC_NAME) || i_pc==(dynlen(g_HV_ID2PC_NAME)-1)){     
      prefix="WIENER_PL512_HID"+coord_1;
      
      if(dynlen(s_split)==2){ // Channel 
       coord_2=substr(s_split[2],strpos(s_split[2],"Channel")+strlen("Channel"));
       i_ch=coord_2;
       i_ch--;
       sprintf(coord_2,"%02d",i_ch);
       service=prefix+"/CHAN"+coord_2+"/DATA";
       command=prefix+"/CHAN"+coord_2+"/CMD";      
      }
      else if (dynlen(s_split)==1){ // Crate 
        service=prefix+"/SYSDATA";
        command=prefix+"/SYSCMD";
      }
     } // if(i_pc==1)
     else service="";

     } // else
    }
//--------------------- start of master --------------------------------
    else if(strpos(BrokerList[j],"master")>=0){ 
      pos=strpos(BrokerList[j],"master");
      pos=pos+strlen("master");
      coord_1=substr(BrokerList[j],pos);
      coord=strsplit(coord_1,"_");
//---
     pc_id="";
     for(i=1;i<=dynlen(g_HV_ID2PC_NAME);i++){
      test_dyn=strsplit(g_HV_ID2PC_NAME[i],";");
      if(test_dyn[2]==pc){pc_id=test_dyn[1];i_pc=i;break;} 
     }  
//---
//mudcsDebug(coord_1+">>"+pc_id);
     if(pc_id==""){service="";}
     else if(pc_id != "all" && (strpos(pc_id,coord[1])<0)){service="";}
     else {
      if(i_pc==dynlen(g_HV_ID2PC_NAME) || i_pc==(dynlen(g_HV_ID2PC_NAME)-1))
      service="HV_1_"+ coord_1;
      else service="";
     }
    } 
//----------------------------- end of master ----------------------------
//--------------------- start of primary --------------------------------
    else if(strpos(BrokerList[j],"primary")>=0){ 
      pos=strpos(BrokerList[j],"primary");
      pos=pos+strlen("primary");
      coord_1=substr(BrokerList[j],pos);


      coord=strsplit(coord_1,"_");
//---

     pc_id="";
     for(i=1;i<=dynlen(g_HV_ID2PC_NAME);i++){
      test_dyn=strsplit(g_HV_ID2PC_NAME[i],";");
      if(test_dyn[2]==pc){pc_id=test_dyn[1];i_pc=i;break;} 
     }
//---
//mudcsDebug(coord_1+">>"+pc_id);
     if(pc_id==""){service="";}
     else if(pc_id != "all" && (strpos(pc_id,coord[1])<0)){service="";}
     else {
//       service="HV_PR_"+ coord_1;
      if(i_pc==dynlen(g_HV_ID2PC_NAME) || i_pc==(dynlen(g_HV_ID2PC_NAME)-1))
      service="HV_PR_"+ coord_1;
      else service="";
      
     }
    } 
//----------------------------- end of primary ----------------------------
//    else if(strpos(BrokerList[j],"Broker")>=0){
//     service="";
//    }
    else{

    /// DebugTN("++++++++++++++++++++++++++"+BrokerList[j]);
    mudcsDimGetService(type_par, pc, BrokerList[j],service);
    }


    if(service == "")service = "no_service";

    if(service != ""){
        dynAppend(services,service);
    ///    DebugTN("++++++++++++++++++++++++++2222"+service);
    }
    if(command != ""){
        dynAppend(commands,command);
    ///    DebugTN("++++++++++++++++++++++++++2222"+service);
    }    
    /// DebugTN("++++++++++++++++++++++++++1111"+service);

   } //for

/// return;

//---------------------------- SERVICES CONFIGURATION ---------------------------   
    for(j=1; j<= dynlen(BrokerList); j++){
//    if(strpos(BrokerList[j],"Broker")>=0)continue;
//    if(services[j]=="")continue;
    if(services[j]=="no_service")continue;

/////////       service=substr(BrokerList[j],0,strpos(BrokerList[j],"_DimBroker"));

//////////     mudcsDimConfig(true,BrokerList[j]+"_o,"+service+",,0,0", manager, exceptionInfo);
     mudcsDimConfig(true,BrokerList[j]+","+services[j]+",,0,1", manager, exceptionInfo);
	if(dynlen(exceptionInfo)>0){return;}

    }
    
//--------------------------- COMMAND CONFIGURATION ------------------------------
        
    for(j=1; j<= dynlen(BrokerList); j++){
//    if(strpos(BrokerList[j],"Broker")>=0)continue;
//    if(services[j]=="")continue;
    if(commands[j]=="no_command")continue;


    if(!dpExists(mudcsRemoveSystem(BrokerList[j]+"_COM")))dpCreate(mudcsRemoveSystem(BrokerList[j])+"_COM","MUDCS_DIMCOM");
      mudcsDimConfig(false,BrokerList[j]+"_COM.command"+","+commands[j], manager, exceptionInfo);
	if(dynlen(exceptionInfo)>0){return;}

    }    
//---------------------------------------------------------------------------------
        
/*
    list=makeDynString("RC2DCS_o,RC2DCS");
    dpSet(mudcsAddSystem(manager+".serverServices:_original.._value"),list);
    list=makeDynString("RC2DCS_COM.command,RC2DCS_COMMAND");
    dpSet(mudcsAddSystem(manager+".serverCommands:_original.._value"),list);
*/

}	
//=================================================================================================
mudcsDimGetService(string type_par, string pc, string broker,string &service){

string mudcs_alias;
int w_pos,radius,ich;
string type;
string setList;
string set;
int i,j;
string test;
int index;


string fsm;

dyn_string test_dyn;
string pc_id="";

//----

string spart;
int pos_part;

//---------
if(type_par=="WTH_SX5"){
       for(i=1;i<=dynlen(CSC_fwG_g_BROKER_DNS_MACHINE_NAMES);i++){
        test_dyn=strsplit(CSC_fwG_g_BROKER_DNS_MACHINE_NAMES[i],";");
        if(test_dyn[2]==pc){pc_id=test_dyn[1];break;} 
       }
       if(pc_id==""){service="";return;}
       else if(pc_id != "all" && strpos("WTH_SX5",pc_id) < 0){service="";return;}
 service="WTH_SX5";

 return;
}
//---------
else if(type_par=="PT100"){
       for(i=1;i<=dynlen(CSC_fwG_g_BROKER_DNS_MACHINE_NAMES);i++){
        test_dyn=strsplit(CSC_fwG_g_BROKER_DNS_MACHINE_NAMES[i],";");
        if(test_dyn[2]==pc){pc_id=test_dyn[1];break;} 
       }
       if(pc_id==""){service="";return;}
       else if(pc_id != "all" && strpos("PT100",pc_id) < 0){service="";return;}
 service="PT100";

 return;
}
//---------
else if(type_par=="LV_1" || type_par=="TEMP_1" || type_par=="CHIP_1"){
  setList="DynatemAndSetNumberList";
  for(i=1;i<=dynlen(CSC_fwG_g_PCRATE_ID2PC_NAME);i++){
   test_dyn=strsplit(CSC_fwG_g_PCRATE_ID2PC_NAME[i],";");
   if(test_dyn[2]==pc){pc_id=test_dyn[1];break;} 
  }
 }
 else if(type_par=="HV_1"){
  setList="HVsetList";
  for(i=1;i<=dynlen(g_HV_ID2PC_NAME);i++){
   test_dyn=strsplit(g_HV_ID2PC_NAME[i],";");
   if(test_dyn[2]==pc){pc_id=test_dyn[1];break;} 
  }  
 }
//-------
 else if(type_par=="CRB_1"){
  setList="DiskLevelDevicesCoordinates";
  for(i=1;i<=dynlen(CSC_fwG_g_STATIONS_DNS_MACHINE_NAMES);i++){
   test_dyn=strsplit(CSC_fwG_g_STATIONS_DNS_MACHINE_NAMES[i],";");
   if(test_dyn[2]==pc){pc_id=test_dyn[1];break;} 
  }  
 }
//-------
 else if(type_par=="MRTN_1"){
  setList="CscLevelDevicesCoordinates";
  for(i=1;i<=dynlen(CSC_fwG_g_BROKER_DNS_MACHINE_NAMES);i++){
   test_dyn=strsplit(CSC_fwG_g_BROKER_DNS_MACHINE_NAMES[i],";");
   if(test_dyn[2]==pc){pc_id=test_dyn[1];break;} 
  }  
 }
//-------
 else if(type_par=="WNR12_1"){
  setList="CscLevelDevicesCoordinates";
  for(i=1;i<=dynlen(CSC_fwG_g_BROKER_DNS_MACHINE_NAMES);i++){
   test_dyn=strsplit(CSC_fwG_g_BROKER_DNS_MACHINE_NAMES[i],";");
   if(test_dyn[2]==pc){pc_id=test_dyn[1];break;} 
  }  
 }
//-------
 else if(type_par=="ALNM_1"){
  setList="DiskLevelDevicesCoordinates";
  for(i=1;i<=dynlen(CSC_fwG_g_STATIONS_DNS_MACHINE_NAMES);i++){
   test_dyn=strsplit(CSC_fwG_g_STATIONS_DNS_MACHINE_NAMES[i],";");
   if(test_dyn[2]==pc){pc_id=test_dyn[1];break;} 
  }  
 }
//-------
 mudcs_alias=mudcsAliasNameGet(broker);
mudcsNameToPosConvert(mudcs_alias/*broker*/, w_pos, radius, ich, type);
///DebugTN("*** "+type_par+" " +broker+" " + setList+" " + w_pos+" " + radius+" " + ich+" " +type+" " +set);

retrieveCoordinateSet(type_par, broker, setList, w_pos, radius, ich, type, set);
//-------------------
/*
if(type_par=="CRB_1"||type_par=="MRTN_1"){
dyn_string dyn_debug1;
dpGet("dyn_debug1.",dyn_debug1);
dynAppend(dyn_debug1,type_par+"> "+ broker+">>"+mudcs_alias+"--"+ setList+ ">>set="+set+"w_pos="+w_pos);
dpSetWait("dyn_debug1.",dyn_debug1);
}
*/
//---------------------------------------------------------------------

///DebugTN("+++ "+type_par+" " +broker+" " + setList+" " + w_pos+" " + radius+" " + ich+" " +type+" " +set);
 if(set==""){
  service="";
  return;
 }


dyn_string coord=strsplit(set,";");
dyn_string d_ip,d_ip2 ;
//-------------------
/*
//dyn_string dyn_debug1;
dpGet("dyn_debug1.",dyn_debug1);
dynAppend(dyn_debug1,type_par+">>"+set);
dpSetWait("dyn_debug1.",dyn_debug1);
*/

//---------------------------------------------------------------------
if(pc_id==""){service="";return;}
else if(pc_id != "all" && (strpos(pc_id,coord[1]) < 0 /*&& strpos(coord[1], pc_id) < 0*/)  ){service="";return;}
//---------------------------------------------------------------------

if(type_par=="LV_1" || type_par=="TEMP_1" || type_par=="CHIP_1" || type_par=="CRB_1"
 || type_par=="MRTN_1" || type_par=="WNR12_1" || type_par=="ALNM_1"  ){

if(dynlen(coord)>=2)service=type_par + "_"+coord[1]+"_"+coord[2];
else service=type_par + "_"+coord[1];

strreplace(service,".","_");
strreplace(service,":","_");

DebugTN(service+"****************************************************************");

/*
d_ip=strsplit(coord[1],".");
service=type_par + "_";
for(i=1;i<=dynlen(d_ip);i++){
 d_ip2=strsplit(d_ip[i],":");
 for(j=1;j<=dynlen(d_ip2);j++){
  service=service+d_ip2[j]+"_";
 }
}
service=service+coord[2];
*/

/// service="LV_1_"+d_ip[1]+"_"+d_ip[2]+"_"+d_ip[3]+"_"+d_ip[4]+"_"+coord[2];

mudcsConvertData2FsmOne(type_par, broker, fsm);

}
//---------------------------------------------------------------------
else if(type_par=="HV_1"){
 
 
  
 pos_part=strpos(pc_id,"_part");
 if(pos_part>=0){
 spart=substr(pc_id,pos_part+5);
 if(spart=="1" && (strpos(broker,"ME_P1") <0 && strpos(broker,"ME_M1") <0) )service="";
 else if(spart=="2" && (strpos(broker,"ME_P2") <0 && strpos(broker,"ME_M2") <0) )service="";
 else if(spart=="3" && (strpos(broker,"ME_P3") <0 && strpos(broker,"ME_M3") <0) )service="";
 else if(spart=="4" && (strpos(broker,"ME_P4") <0 && strpos(broker,"ME_M4") <0) )service="";
 else service="HV_1_"+coord[1]+"_"+coord[2]+"_"+coord[3]; 
 }
 else 
 service="HV_1_"+coord[1]+"_"+coord[2]+"_"+coord[3];
 
  if(strpos(pc_id,coord[1])<0){service="";} // because there are two part4 pc (for HW tree project)
 
 mudcsConvertData2FsmOne("HV_1", broker, fsm);
 
 if(coord[4] != "0"){
   test=coord[1]+";"+coord[2]+";"+coord[3];
   index=dynContains(CSC_fwG_g_HV_36CHANNEL_BOARDS_LIST,test);


   if(index>0){
    CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[index][2] = fsm;
   }
   else {
    dynAppend(CSC_fwG_g_HV_36CHANNEL_BOARDS_LIST,test);
    CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[dynlen(CSC_fwG_g_HV_36CHANNEL_BOARDS_LIST)][1]=fsm;
   }
 }

}
//---------------------------------------------------------------------

dpSetWait(mudcsAddSystem(fsm+".coord"),set);

}

//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================

mudcsConvertFsm2Data_type(string type_par, dyn_string fsm, dyn_string &data){

 int i;
 mudcsConvertFsm2Data(fsm, data);
 for(i=1;i<=dynlen(data);i++){
    if(type_par !="" && strpos(data[i],type_par)<0){
     data[i] += type_par;
    }
 }

}

//=================================================================================================

mudcsConvertFsm2Data(dyn_string fsm, dyn_string &data){

int i;
dyn_string test;
dyn_string test1;


int new_system=0;

string mudcs_alias=mudcsAliasNameGet(fsm[1]);
string fsm1=mudcs_alias;


 if(strpos(fsm1,"HV_1")>=0 || strpos(fsm1,"LV_1")>=0 || strpos(fsm1,"TEMP_1")>=0 || strpos(fsm1,"CHIP_1")>=0
    ||  strpos(fsm1,"WTH_SX5")>=0 ||  strpos(fsm1,"HV_PR" )>=0 ||  strpos(fsm1,"PT100")>=0 ||  strpos(fsm1,"CRB_1")>=0
    ||  strpos(fsm1,"MRTN_1")>=0 ||  strpos(fsm1,"WNR12_1")>=0 ||  strpos(fsm1,"ALNM_1")>=0 
       ||  strpos(fsm1,"WienerSnmpCrate" )>=0 ){
  new_system=1;
 }

 if(strpos(fsm,"CoolingCMS")>=0 || strpos(fsm,"GasCMS")>=0){
  new_system=1;
 }
 
 if(new_system==0){
   data=fsm;
   return;
 }

 
 for(i=1;i<=dynlen(fsm);i++){
  test1=strsplit(fsm[i],":");
  test=strsplit(test1[dynlen(test1)],"/");

   data[i]=CSC_fwG_g_SYSTEM_NAME+":"+test[dynlen(test)];
   
   if(strpos(fsm1,"WienerSnmpCrate" )>=0 && strpos(fsm1,"Channel" )>=0){
    data[i]=CSC_fwG_g_SYSTEM_NAME+":"+test[dynlen(test)-1]+"/"+test[dynlen(test)];
   }
   
 } // for

}

//=================================================================================================
mudcsConvertFsm2DataOne_type(string type_par, string fsm, string &data){ // is not in use 04/27/2006

 mudcsConvertFsm2DataOne(fsm, data);

    if(type_par !="" && strpos(data,type_par)<0){
     data += type_par;
    }


}
//=================================================================================================
mudcsConvertFsm2DataOne(string fsm, string &data){

dyn_string test;
dyn_string test2;

test2=strsplit(fsm,".");  // to cut an element
if(dynlen(test2)<=0){DebugTN(fsm+" mudcsConvertFsm2DataOne::test2 is OF ZERO LENGTH");return;} // pvss36
fsm=test2[1];

string mudcs_alias=mudcsAliasNameGet(fsm);
string fsm1=mudcs_alias;

int new_system=0;


 if(strpos(fsm1,"HV_1")>=0 || strpos(fsm1,"LV_1")>=0 || strpos(fsm1,"TEMP_1")>=0 || strpos(fsm1,"CHIP_1")>=0
    ||  strpos(fsm1,"WTH_SX5" )>=0 ||  strpos(fsm1,"HV_PR" )>=0 ||  strpos(fsm1,"PT100" )>=0 ||  strpos(fsm1,"CRB_1" )>=0 
    ||  strpos(fsm1,"MRTN_1" )>=0 ||  strpos(fsm1,"WNR12_1" )>=0 ||  strpos(fsm1,"ALNM_1" )>=0  
       ||  strpos(fsm1,"WienerSnmpCrate" )>=0 ){
  new_system=1;
 }

 if(strpos(fsm,"CoolingCMS")>=0 || strpos(fsm,"GasCMS")>=0){
  new_system=1;
 }
 
 if(new_system==0){
   data=fsm;
   return;
 }

  test=strsplit(fsm,"/");
   if(dynlen(test)>1)
    data=CSC_fwG_g_SYSTEM_NAME+":"+test[dynlen(test)];
   else if(dynlen(test)==1)
    data=test[dynlen(test)];

   if(strpos(fsm1,"WienerSnmpCrate" )>=0 && strpos(fsm1,"Channel" )>=0){
    data=CSC_fwG_g_SYSTEM_NAME+":"+test[dynlen(test)-1]+"/"+test[dynlen(test)];
   }   
   
}
//=================================================================================================
//=================================================================================================
//================================================================================================
//=================================================================================================

mudcsConvertData2Fsm(string type_par, dyn_string list, dyn_string &list_fsm){

int i,j;
dyn_string test;
dyn_string test1;
dyn_string test2;
string type;
string type_short=type_par;

 if(strpos(type_par,"HV_1")>=0){
  type="HighVoltage/";
type_short="HV";
 }
 else if(strpos(type_par,"HV_PR")>=0){
  type="HighVoltage/";
 }
 else if(strpos(type_par,"LV_1")>=0){
  type="LowVoltage/";
type_short="LV";
 }
 else if(strpos(type_par,"TEMP_1")>=0){
  type="Temperature/";
type_short="TEMP";
 }
 else if(strpos(type_par,"CHIP_1")>=0){
  type="ChipLoading/";
type_short="CHIP";
 }
 else if(strpos(type_par,"WTH_SX5")>=0){
  type="WeatherStation/";
type_short="WEATHER";
 }
 else if(strpos(type_par,"PT100")>=0){
  type="GasMonitoring/";
 }
 else if(strpos(type_par,"CRB")>=0){
  type="CRB/";
type_short="CRB";
 } 
 else if(strpos(type_par,"MRTN")>=0){
  type="MRTN/";
type_short="MRTN";
 } 
 else if(strpos(type_par,"WNR12")>=0){
  type="WNR12/";
type_short="WNR12";
 } 
 else if(strpos(type_par,"ALNM")>=0){
  type="ALNM/";
type_short="ALNM";
 } 
 else if(strpos(type_par,"fwWnrCr_CSC_LV")>=0){
  type="WienerSnmp/";
type_short="WienerSnmp";
 }  
 else if(strpos(type_par,"fwWnrCh_CSC_LV")>=0){
  type="WienerSnmp/";
type_short="WienerSnmp";
 }
 else if(strpos(type_par,"fwGasSystem_CSC_GAS")>=0){
  type="GasCMS/";
type_short="GasCMS";
 }   
 else if(strpos(type_par,"fwCooling_CSC_COOLING")>=0){
  type="CoolingCMS/";
type_short="CoolingCMS";
 }   

 for(i=1;i<=dynlen(list);i++){

  test2=strsplit(list[i],".");   // to cut an element
  test1=strsplit(test2[1],":");
  test=strsplit(test1[dynlen(test1)],"/");
  
    if(type==test[1]+"/")
    list_fsm[i]=CSC_fwG_g_SYSTEM_NAME+":"+test1[dynlen(test1)];
    else{
     if(strpos(test[dynlen(test)],"Channel")<0) // case of snmp channel device (WienerSnmp/WienerSnmpCrate600/Channel00)
     list_fsm[i]=CSC_fwG_g_SYSTEM_NAME+":"+type+test[dynlen(test)];
     else list_fsm[i]=CSC_fwG_g_SYSTEM_NAME+":"+type+test[dynlen(test)-1]+"/"+test[dynlen(test)];
    }
////  test=strsplit(list[i],":");
/////   list_fsm[i]=CSC_fwG_g_SYSTEM_NAME+":"+type+test[dynlen(test)];

    if(type_par !="" && strpos(list_fsm[i],type_short)<0){
     list_fsm[i] += "_"+type_short;
    }
 }

}

//=========================================================================================

mudcsConvertData2FsmOne(string type_par, string data, string &fsm){

dyn_string test;
dyn_string test1;
dyn_string test2;
string type_short=type_par;

string type;

 if(strpos(type_par,"HV_1")>=0){
  type="HighVoltage/";
type_short="HV";
 }
 else if(strpos(type_par,"HV_PR")>=0){
  type="HighVoltage/";
 }
 else if(strpos(type_par,"LV_1")>=0){
  type="LowVoltage/";
type_short="LV";
 }
 else if(strpos(type_par,"TEMP_1")>=0){
  type="Temperature/";
type_short="TEMP";
 }
 else if(strpos(type_par,"CHIP_1")>=0){
  type="ChipLoading/";
type_short="CHIP";
 }
 else if(strpos(type_par,"WTH_SX5")>=0){
  type="WeatherStation/";
type_short="WEATHER";
 }
 else if(strpos(type_par,"PT100")>=0){
  type="GasMonitoring/";
 }
 else if(strpos(type_par,"CRB")>=0){
  type="CRB/";
type_short="CRB";
 }
 else if(strpos(type_par,"MRTN")>=0){
  type="MRTN/";
type_short="MRTN";
 }
 else if(strpos(type_par,"WNR12")>=0){
  type="WNR12/";
type_short="WNR12";
 } 
 else if(strpos(type_par,"ALNM")>=0){
  type="ALNM/";
type_short="ALNM";
 }
 else if(strpos(type_par,"fwWnrCr_CSC_LV")>=0){
  type="WienerSnmp/";
type_short="WienerSnmp";
 }  
 else if(strpos(type_par,"fwWnrCh_CSC_LV")>=0){
  type="WienerSnmp/";
type_short="WienerSnmp";
 }   
 else if(strpos(type_par,"fwGasSystem_CSC_GAS")>=0){
  type="GasCMS/";
type_short="GasCMS";
 }   
 else if(strpos(type_par,"fwCooling_CSC_COOLING")>=0){
  type="CoolingCMS/";
type_short="CoolingCMS";
 }   
 
  test2=strsplit(data,".");   // to cut an element
  test1=strsplit(test2[1],":");
  test=strsplit(test1[dynlen(test1)],"/");

    if(type==test[1]+"/")
    fsm=CSC_fwG_g_SYSTEM_NAME+":"+test1[dynlen(test1)];
    else{
    if(strpos(test[dynlen(test)],"Channel")<0) // case of snmp channel device (WienerSnmp/WienerSnmpCrate600/Channel00) 
    fsm=CSC_fwG_g_SYSTEM_NAME+":"+type+test[dynlen(test)];
    else fsm=CSC_fwG_g_SYSTEM_NAME+":"+type+test[dynlen(test)-1]+"/"+test[dynlen(test)];  
    }

      
//    fsm=CSC_fwG_g_SYSTEM_NAME+":"+type+test[dynlen(test)];
 
    if(type_par !="" && strpos(fsm,type_short)<0){
     fsm += "_"+type_short;
    }
}
//=================================================================================================

mudcsGetAllCoordForOneType(int mode, string type_par, dyn_string &sets){

 int j;
 string mudcs_alias;
 int w_pos,radius,ich;
 string type;
 string setList;
 string set;
 int i;

 dyn_string dps; 
 string broker;
 dynClear(sets);

 if(type_par=="LV_1" || type_par=="TEMP_1" || type_par=="CHIP_1")setList="DynatemAndSetNumberList";
 else if(type_par=="HV_1")setList="HVsetList";
 else if(type_par=="CRB_1")setList="DiskLevelDevicesCoordinates";
 else if(type_par=="MRTN_1")setList="CscLevelDevicesCoordinates";
 else if(type_par=="WNR12_1")setList="CscLevelDevicesCoordinates";
 else if(type_par=="ALNM_1")setList="DiskLevelDevicesCoordinates";

 if(mode==0){
  dps=dpNames("*",type_par+"_d");
 } 
 else if(mode==1){
  mudcsConvertData2Fsm(type_par, mudcsNamesByAliasesGet(CSC_fwG_g_SLICE_DPOINTS), dps);
  mudcsConvertFsm2Data(dps, dps);  

 }
 else if(mode==2){
  mudcsConvertData2Fsm(type_par, mudcsNamesByAliasesGet(CSC_fwG_g_HV_LONG_TERM_DPOINTS), dps);
  mudcsConvertFsm2Data(dps, dps);  
 }

  for(j=1; j<= dynlen(dps); j++){
   broker=dps[j]; 
   if(strpos(broker,"master")>=0 || strpos(broker,"Broker")>=0)continue;
//   DebugTN("---------------------------"+broker);
 mudcs_alias=mudcsAliasNameGet(broker);
   mudcsNameToPosConvert(mudcs_alias/*broker*/, w_pos, radius, ich, type);
   ///DebugTN("*** "+type_par+" " +broker+" " + setList+" " + w_pos+" " + radius+" " + ich+" " +type+" " +set);
   retrieveCoordinateSet(type_par, broker, setList, w_pos, radius, ich, type, set);
   ///DebugTN("+++ "+type_par+" " +broker+" " + setList+" " + w_pos+" " + radius+" " + ich+" " +type+" " +set);
    if(set!=""){
     dynAppend(sets,set);
    }
 } // for

}
//=================================================================================================

mudcsGetCoordForDp(string type_par, string dp, string &set){

 int j; 
 int i;
 string mudcs_alias;
 int w_pos,radius,ich;
 string type;
 string setList;

 string broker;
 dyn_string coord;
 dyn_string coord2;

 if(type_par=="LV_1" || type_par=="TEMP_1" || type_par=="CHIP_1")setList="DynatemAndSetNumberList";
 else if(type_par=="HV_1")setList="HVsetList";
 else if(type_par=="CRB_1")setList="DiskLevelDevicesCoordinates"; 
 else if(type_par=="MRTN_1")setList="CscLevelDevicesCoordinates";
 else if(type_par=="WNR12_1")setList="CscLevelDevicesCoordinates";
 else if(type_par=="ALNM_1")setList="DiskLevelDevicesCoordinates"; 

   if(strpos(dp,"master")>=0){
    ////// broker=dp;
 //   DebugTN("++++++++++++++++++__________________"+dp); 
    broker=fwFsm_extractSystem(dp);
    coord=strsplit(broker,"_");
    coord2=strsplit(coord[1],"r");
    set=coord2[2]+";"+coord[2]+";"+coord[3];
   }
   else if(strpos(dp,"primary")>=0){
    ////// broker=dp;
    broker=fwFsm_extractSystem(dp);
    coord=strsplit(broker,"_");
    coord2=strsplit(coord[1],"y");
    set=coord2[2]+";"+coord[2]+";"+coord[3];
   }
   else{
    mudcsConvertFsm2DataOne(dp, broker);
 mudcs_alias=mudcsAliasNameGet(broker);

/*
   dyn_string d_s2;
    dpGet(mudcsAddSystem("dyn_debug2."),d_s2);
    dynAppend(d_s2,dp+">>>"+broker+">>>"+mudcs_alias);
    dpSetWait(mudcsAddSystem("dyn_debug2."),d_s2);
*/
    mudcsNameToPosConvert(mudcs_alias/*broker*/, w_pos, radius, ich, type);
    ///DebugTN("*** "+type_par+" " +broker+" " + setList+" " + w_pos+" " + radius+" " + ich+" " +type+" " +set);
    retrieveCoordinateSet(type_par, broker, setList, w_pos, radius, ich, type, set);
    ///DebugTN("+++ "+type_par+" " +broker+" " + setList+" " + w_pos+" " + radius+" " + ich+" " +type+" " +set);
   }
}
//================================================================================================
mudcsGetAllCoordForHvMasters(string type_par, dyn_string &sets){

 int j;
 string set;
 int i;

 dyn_string dps; 
 string broker;
 dynClear(sets);
 dyn_string coord;
 dyn_string coord2;

 dps=dpNames("master*",type_par+"_d");
 
  for(j=1; j<= dynlen(dps); j++){
   broker=dps[j]; 
   coord=strsplit(broker,"_");
   coord2=strsplit(coord[1],"r");
     dynAppend(sets,coord2[2]+";"+coord[2]+";"+coord[3]);
  } // for
}
//================================================================================================
mudcsGetMasterForChamber(string fsm, int &master_id, int &master_chan, int &master_index, dyn_string &coord_master){


    dyn_string dyn_debug;

int status;
int i;
string data;

  master_id=-2;
  master_chan=-2;
  master_index=-2;


 dpGet(mudcsAddSystem(fsm+".status"),status);

 if(status==-2){
  return;
 }
//--- 

 string set;
 dyn_string coord;
 int master_module_id;

 mudcsConvertFsm2DataOne(fsm, data);

// dpGet(mudcsAddSystem(fsm+".coord"),set); commented out as fsm.coord might not be set 
// coord=strsplit(set,";");
 
 dpGet(mudcsAddSystem(data+".data.master_chan"),master_chan);
 dpGet(mudcsAddSystem(data+".data.master_id"),master_id);

////// dynAppend(CSC_fwG_g_dyn_debug1,fsm+" "+master_id);

/*
    dpGet(mudcsAddSystem("dyn_debug2."),dyn_debug);
    dynAppend(dyn_debug,data+ " master_chan master_id "+master_chan+" "+master_id);
    dpSetWait(mudcsAddSystem("dyn_debug2."),dyn_debug);
*/

//---
 for(i=1;i<=dynlen(CSC_fwG_g_all_masters);i++){
  dpGet(mudcsAddSystem(CSC_fwG_g_all_masters[i]+".data.module_ID"),master_module_id);
  mudcsGetCoordForDp("HV_1",CSC_fwG_g_all_masters[i], set);
  coord_master=strsplit(set,";");
//   dynAppend(master2chamber,data+";"+master_chan+"___"+master_id +";"+ master_module_id+ "|");

/*
    dpGet(mudcsAddSystem("dyn_debug2."),dyn_debug);
    dynAppend(dyn_debug," master_module_id master_id "+master_module_id+" "+master_id);
    dpSetWait(mudcsAddSystem("dyn_debug2."),dyn_debug);
*/

  if(/*coord_master[1]==coord[1] &&*/ master_module_id==master_id && master_id !=0 ){ /*pvss36*/
  
   master_index=i;
////   if(dynlen(CSC_fwG_g_master2chamber)>=master_index)   // pvss36
////   if(strpos(CSC_fwG_g_master2chamber[master_index],data) < 0) // pvss36
   CSC_fwG_g_master2chamber[master_index]=CSC_fwG_g_master2chamber[master_index]+data+";"+master_chan+ "|";


/* 
    dpGet(mudcsAddSystem("dyn_debug2."),dyn_debug);
    dynAppend(dyn_debug," ========== "+CSC_fwG_g_master2chamber[master_index]+" "+master_index);
    dpSetWait(mudcsAddSystem("dyn_debug2."),dyn_debug);
*/

//   dynAppend(CSC_fwG_g_dyn_debug1,fsm+master_chan+"___"+master_id +";"+ master_module_id+ "|");

   return;
  }
 }




//   dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":HV_MASTER2CHAMBER."),master2chamber);

}
//================================================================================================
