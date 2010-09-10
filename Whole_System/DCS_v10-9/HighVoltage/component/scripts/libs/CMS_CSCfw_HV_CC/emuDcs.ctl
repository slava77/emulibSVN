//=========================================
string mudcsAddHwSystem(string dp){ 

//  dp=fwFsm_extractSystem(dp);


   int pos1=strpos(dp,CSC_fwG_g_HW_SYSTEM_NAME+":");
   int pos2=strpos(dp,CSC_fwG_g_SYSTEM_NAME+":");
   int pos=pos1;
   if(pos2>pos)pos=pos2;

   if(pos>=0 && pos==pos1)pos=pos1+strlen(CSC_fwG_g_HW_SYSTEM_NAME+":");
   else if(pos>=0 && pos==pos2)pos=pos2+strlen(CSC_fwG_g_SYSTEM_NAME+":") ;    
    
   if(pos>=0)dp=substr(dp,pos);
   dp=CSC_fwG_g_HW_SYSTEM_NAME+":"+dp;


  return dp;

}
//=========================================
string mudcsAddSystem(string dp, string system_add=""){ 

//  dp=fwFsm_extractSystem(dp);

  if(system_add==""){  
   int pos=strpos(dp,CSC_fwG_g_SYSTEM_NAME+":");
   if(pos>=0)dp=substr(dp,pos+strlen(CSC_fwG_g_SYSTEM_NAME+":"));
   dp=CSC_fwG_g_SYSTEM_NAME+":"+dp;
  }
  else{
   int pos=strpos(dp,system_add+":");
   if(pos>=0)dp=substr(dp,pos+strlen(system_add+":"));
   dp=system_add+":"+dp;
  }

  return dp;

}
//=========================================
string mudcsRemoveSystem(string dp, string system_remove=""){ 

  
  int pos;
  if(system_remove==""){   
   if((pos=strpos(dp,CSC_fwG_g_SYSTEM_NAME+":"))< 0)return dp;  
   dp = substr(dp,strlen(CSC_fwG_g_SYSTEM_NAME+":"));

  }
  else{
   if((pos=strpos(dp,system_remove+":"))< 0)return dp;  
   dp = substr(dp,strlen(system_remove+":"));    
    
  }
   return dp;
}
//=======================================================================================
dyn_string mudcsNamesByAliasesGet(dyn_string dyn_alias){

int i,j;
dyn_string dyn_name=makeDynString();
int pos;
string name, alias;

  dyn_string emu_name_aliases;
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"EMUNAMEALIASES."),emu_name_aliases);
  dyn_string dps;

  for(j=1;j<=dynlen(dyn_alias);j++){

dps=strsplit(dyn_alias[j],":");
dps=strsplit(dps[dynlen(dps)],"/");
alias=dps[dynlen(dps)];
/*
   dyn_string d_s2;
    dpGet(mudcsAddSystem("dyn_debug2."),d_s2);
    dynAppend(d_s2,dyn_alias[j]+">>>"+alias);
    dpSetWait(mudcsAddSystem("dyn_debug2."),d_s2);
*/

  for(i=1;i<=dynlen(emu_name_aliases);i++){
   pos=strpos(emu_name_aliases[i],"#"+alias);
   if(pos>=0){
/*
    dynAppend(d_s2,pos+">>>>>>"+strlen(emu_name_aliases[i])+">>>>"+strlen("#"+alias));
    dpSetWait(mudcsAddSystem("dyn_debug2."),d_s2);
*/
    if((strlen(emu_name_aliases[i]) - pos) == strlen("#"+alias)){
     name=substr(emu_name_aliases[i],0,pos);
     if(dynlen(dps)==2)name=dps[1]+"/"+name;
     name=CSC_fwG_g_SYSTEM_NAME+":"+name;
     dynAppend(dyn_name,name);
     break;
    }
   }
  }


  } //j

 return dyn_name;

}
//=======================================================================================
string mudcsNameByAliasGet(string alias){

int i;
string name="";
int pos;

dyn_string dps=strsplit(alias,":");
dps=strsplit(dps[dynlen(dps)],"/");
alias=dps[dynlen(dps)];

  dyn_string emu_name_aliases;
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"EMUNAMEALIASES."),emu_name_aliases);
  //-------------------------------------
  
  if(dynlen(emu_name_aliases)==0){
    dyn_string dyn_debug5;
    dynAppend(dyn_debug5,"mudcsNameByAliasGet: "+CSC_fwG_g_SYSTEM_NAME+":"+"EMUNAMEALIASES.");
    dynAppend(dyn_debug5,"mudcsNameByAliasGet: "+alias);    
    mudcsDebug_dyn5(dyn_debug5); 
  }
  
  //--------------------------------------  
  
  
  for(i=1;i<=dynlen(emu_name_aliases);i++){
   pos=strpos(emu_name_aliases[i],"#"+alias);
   if(pos>=0){
    if((strlen(emu_name_aliases[i]) - pos) == strlen("#"+alias)){
     name=substr(emu_name_aliases[i],0,pos);
     if(dynlen(dps)==2)name=dps[1]+"/"+name;
     name=CSC_fwG_g_SYSTEM_NAME+":"+name;
     break;
    }
   }
  }

 return name;

}
//=======================================================================================
string mudcsAliasNameGet(string fdpname){

int i;
string alias="";
//DebugTN("+++"+fdpname);
dyn_string dps=strsplit(fdpname,".");

dps=strsplit(dps[1],":");
//string dpname=dps[dynlen(dps)];

dps=strsplit(dps[dynlen(dps)],"::");

dps=strsplit(dps[dynlen(dps)],"/");


string dpname=dps[dynlen(dps)];
//DebugTN("+++"+dpname);
bool found=false;

  dyn_string emu_name_aliases;
  dpGet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"EMUNAMEALIASES."),emu_name_aliases);
  //-------------------------------------
  
  if(dynlen(emu_name_aliases)==0){
    dyn_string dyn_debug5;
    dynAppend(dyn_debug5,"mudcsAliasNameGet: "+CSC_fwG_g_SYSTEM_NAME+":"+"EMUNAMEALIASES.");
    dynAppend(dyn_debug5,"mudcsAliasNameGet: "+dpname);    
    mudcsDebug_dyn5(dyn_debug5); 
  }
  
  //--------------------------------------
//  DebugTN("+++"+mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"EMUNAMEALIASES."));
//  DebugTN("+++"+emu_name_aliases[1]);
  for(i=1;i<=dynlen(emu_name_aliases);i++){
   if(strpos(emu_name_aliases[i],dpname+"#")>=0){
     found=true;
     alias=substr(emu_name_aliases[i],strpos(emu_name_aliases[i],"#")+1);
     break;
   }
  }   

 if(found)strreplace(fdpname,dpname,alias);
 alias=fdpname; // returns the same if there is no alias (FED)
 return alias;

}
//=======================================================================================

mudcsNameCompose(string type, string station_label, string emu_system_side, int idisk, string disk_rad, string chamber, string &name){
//mudcsNameCompose(CSC_fwG_EmuCmsGlobalType, station_label"", emu_system_side"", idisk, disk_rad"", chamber"", EmuCmsGlobalNode);
//mudcsNameCompose("", station_label"", emu_system_side"", idisk, disk_rad"", chamber"", EmuCmsGlobalNode);


  string alias;
  string type_cut;
  type_cut=substr(type,0,strpos(type,"_"));
  string CHAMBER=chamber;
  string DISK_RAD=disk_rad;
  strreplace(CHAMBER,"c","C");
  strreplace(DISK_RAD,"r","");

 if(type==""){ // LU
  name="CSC";
  if(emu_system_side=="m")name+="_ME_M"+idisk;
  else name+="_ME_P"+idisk;
  alias="CSC"+station_label+emu_system_side+idisk;
  
  if(strpos(disk_rad,"r") >=0){
   name+=DISK_RAD+"_"+CHAMBER;
   alias+=disk_rad+chamber;
  }
  else if(disk_rad != ""){ // trigger secor
   name+="_TS_"+disk_rad;
   alias+="t"+disk_rad;     
  }

 }
 else if(type=="LV_CRB" ){ //LU
  name="CSC";
  if(emu_system_side=="m")name+="_ME_M"+idisk+"_"+type;
  else name+="_ME_P"+idisk+"_"+type;
  alias="CSC"+station_label+emu_system_side+idisk+"PC";
 }
 else if(type=="_LV_CRB" ){ //LU
  name="CSC";
  if(emu_system_side=="m")name+="_ME_M"+idisk+"_"+type;
  else name+="_ME_P"+idisk+"_"+type;
  alias="_CSC"+station_label+emu_system_side+idisk+"PC";
 } 
/*
 else if(type=="HV_PRIMARIES"){ //LU
  name="CSC";
  name+="_HV_PRIMARIES"+idisk; // idisk stands for  host ID
  alias="CSCHV_PR"+idisk; // idisk stands for  host ID
 }
*/ 
 else if(type=="WIENER_SNMP_HOST"){ // folder for LVPS for HV
  name="CSC_";
  name+=station_label;
  alias=name;
 }  
 else if(type=="LV_HV"){ // general folder for LVPS for HV
  name="CSC_";
  name+=type;
  alias=name;
 } 
 else if(type=="HV_HOSTS"){ //LU
  name="CSC";
  name+="_HV_HOSTS";
  alias="CSCHV_HOSTS";
 }
  else if(type=="HV_ME11"){ //LU
  name="CSC";
  name+="_HV_ME11";
  alias="CSCHV_ME11";
 }
 else if(type=="HV_HOST"){ //LU
  name="CSC";
  name+="_HV_HOST_"+idisk; // idisk stands for  host ID;
  alias="CSCHV_HOST_"+idisk; // idisk stands for  host ID;
 }
 else if(type=="LV_MRTN"){ //LU  // outdated 09/05/2008
  name="CSC";
  name+="_LV_MRTN";
  alias="CSCLV_MRTN";
 } 
 else if(type=="LV_MRTN_P"){ //LU
  name="CSC";
  name+="_LV_MRTN_P";
  alias="CSCLV_MRTN_P";
 }
 else if(type=="LV_MRTN_M"){ //LU
  name="CSC";
  name+="_LV_MRTN_M";
  alias="CSCLV_MRTN_M";
 } 
 else if(type=="LV_PSU"){ //LU
  name="CSC_";
  name+=type;
  alias=name;
 } 
 else if(type=="_LV_MRTN"){ //LU
 // type="LV_MRTN";
  name="CSC";
  if(emu_system_side=="m")name+="_ME_M"+idisk+"_"+type;
  else name+="_ME_P"+idisk+"_"+type;
  alias="_CSC"+station_label+emu_system_side+idisk+"MRTN";
 } 
 else if(type=="LV_WIENER12"){ //LU
  name="CSC";
  name+="_LV_WIENER12";
  alias="CSCLV_WIENER12";
 }//-------------------------------------------------------
 else if(type=="LV_1" || type=="TEMP_1" || type=="CHIP_1" || type=="HV_1"){ //DU
  name="CSC";
  if(emu_system_side=="m")name+="_ME_M"+idisk+DISK_RAD+"_"+CHAMBER+"_"+type_cut;
  else name+="_ME_P"+idisk+DISK_RAD+"_"+CHAMBER+"_"+type_cut;
  alias="CSC"+station_label+emu_system_side+idisk+disk_rad+chamber+type;
 }
 else if(type=="CRB_1"){ //DU
  name="CSC";
  if(emu_system_side=="m")name+="_ME_M"+idisk+"_PC"+CHAMBER+"_"+"LV_CRB";
  else name+="_ME_P"+idisk+"_PC"+CHAMBER+"_"+"LV_CRB";
  alias="CSC"+station_label+emu_system_side+idisk+"p"+chamber+type;
 }
 else if(type=="ALNM_1"){ //DU
  name="CSC";
  if(emu_system_side=="m")name+="_ME_M"+idisk+"_"+type_cut;
  else name+="_ME_P"+idisk+"_"+type_cut;
  alias="CSC"+station_label+emu_system_side+idisk+type;
 } //--------------------------------------------------------
 else if(type=="fwWnrCr_CSC_LV"){ //DU
  name=station_label;
  alias=name;
 } 
 else if(type=="fwWnrCh_CSC_LV"){ //DU
  name=station_label;
  alias=name;
 }  
 else if(type=="HV_PR"){ //DU
  name=type+"_";
  name+=station_label;
  alias="CSC"+name;
 }
 else if(type=="MRTN_1"){ //DU
  name=type_cut+"_";
  name+=station_label;
  alias="CSC"+station_label+type;
 }
 else if(type=="WNR12_1"){ //DU
  name=type_cut+"_";
  name+=station_label;
  alias="CSC"+station_label+type;
 }
 else if(type=="GAS_SX5"){ //DU
  name=type_cut+"_MIXERS";
  alias="CSC"+type;
 }
 else if(type=="PT100"){ //DU
  name="GAS_PT100";
  alias="CSC"+type;
 }
 else if(type=="WTH_SX5"){ //DU
  name="WEATHER_SX5";
  alias="CSC"+type;
 } //--------------------------------------------------------


  dyn_string emu_name_aliases;
  dpGet(mudcsAddSystem("EMUNAMEALIASES."),emu_name_aliases);

//-------------------------------------
  
  if(dynlen(emu_name_aliases)==0){
    dyn_string dyn_debug5;
    dynAppend(dyn_debug5,"mudcsNameCompose: "+CSC_fwG_g_SYSTEM_NAME+":"+"EMUNAMEALIASES.");
    dynAppend(dyn_debug5,"mudcsNameCompose: "+name);    
    mudcsDebug_dyn5(dyn_debug5); 
  }
  
//--------------------------------------  
  
  
  if(!dynContains(emu_name_aliases,name +"#"+ alias))dynAppend(emu_name_aliases, name +"#"+ alias);
  dpSetWait(mudcsAddSystem("EMUNAMEALIASES."),emu_name_aliases);


}

//=======================================================================================

mudcsPosToNameConvert(int w_pos, int radius, int ich, string device_type, string &name){

 int pos;
 int len;
 string test;
 string w,r;
 int i;
 string prefix;


// !!!!!!!!  this function should be called only for devices located on chambers

 if(strpos(device_type,"LV")>=0)prefix="LowVoltage/";
 else if(strpos(device_type,"HV")>=0)prefix="HighVoltage/";
 else if(strpos(device_type,"TEMP")>=0)prefix="Temperature/";
 else if(strpos(device_type,"CHIP")>=0)prefix="ChipLoading/";

// !!!!!!!!  this function should be called only for devices located on chambers
 else if(strpos(device_type,"GAS")>=0){
  prefix="GasMonitoring/";
   if(strpos(device_type,"GAS_SX5")>=0)
   sprintf(name,"%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + device_type);

  return;
 }
 else if(strpos(device_type,"WTH")>=0){
  prefix="WeatherStation/";
   if(strpos(device_type,"WTH_SX5")>=0)
   sprintf(name,"%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + device_type);

  return;
 }
 else if(strpos(device_type,"PT100")>=0){
  prefix="GasMonitoring/";
   if(strpos(device_type,"PT100")>=0)
   sprintf(name,"%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + device_type);

  return;
 }
 else if(strpos(device_type,"CRB")>=0){  // should be corrected by calculating pcrate position by chamber number!!!!!!!
  prefix="CRB/";
   if(strpos(device_type,"CRB_1")>=0)
   sprintf(name,"%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + CSC_fwG_g_wheel_labels[w_pos]+ device_type);

  return;
 }
 else if(strpos(device_type,"MRTN")>=0){  // should be corrected by calculating pcrate position by chamber number!!!!!!!
  prefix="MRTN/";
   if(strpos(device_type,"MRTN_1")>=0)
   sprintf(name,"%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + CSC_fwG_g_wheel_labels[w_pos]+ device_type);

  return;
 }
 else if(strpos(device_type,"WNR12")>=0){  // should be corrected by calculating pcrate position by chamber number!!!!!!!
  prefix="WNR12/";
   if(strpos(device_type,"WNR12_1")>=0)
   sprintf(name,"%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + CSC_fwG_g_wheel_labels[w_pos]+ device_type);

  return;
 }
 else if(strpos(device_type,"ALNM")>=0){  // 
  prefix="ALNM/";
   if(strpos(device_type,"ALNM_1")>=0)
   sprintf(name,"%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + CSC_fwG_g_wheel_labels[w_pos]+ device_type);

  return;
 }
 sprintf(name,"%sc%02d%s",CSC_fwG_g_SYSTEM_NAME+":"+prefix+CSC_fwG_g_csc_prefix + CSC_fwG_g_wheel_labels[w_pos] + CSC_fwG_g_radius_labels[radius], ich, device_type);

}

//==========================================================================================
mudcsNameToPosConvert(string double_name, int &disk_pos, int &radius, int &ich, string &type){

 int pos, pos2, pos3;
 int len;
 string name,test,test1;
 int disk_dir;
 dyn_string name_tree, dyn_test;
 

///// mudcsDebug2("emudcsNameToPosConvert");

pos=strpos(double_name,"::");

if(pos>=0)name=substr(double_name,pos+2);
else name=double_name;

 name_tree=strsplit(name,"/");
 name=name_tree[dynlen(name_tree)];

/*
if((dynlen(name_tree))==0){
dyn_string d_s2;
    dpGet(mudcsAddSystem("dyn_debug2."),d_s2);
    dynAppend(d_s2,double_name+">>"+dynlen(name_tree));
    dpSetWait(mudcsAddSystem("dyn_debug2."),d_s2);
}
*/
//--
pos=strpos(name,CSC_fwG_g_SYSTEM_NAME+":");
if(pos>=0)name=substr(name,pos+strlen(CSC_fwG_g_SYSTEM_NAME+":"));
//else name=name;

//--
//DebugN("mudcsNameToPosConvert::==============================================================="+name+" "+name_tree);
   string csc_prefix=CSC_fwG_g_csc_prefix;
   pos2=strpos(name,csc_prefix);
   test=substr(name,pos2+3);

////mudcsDebug2(double_name);
////mudcsDebug2(name);

   disk_pos=substr(test,2,1);
   if(substr(test,1,1)=="m")disk_dir=-1;
   else disk_dir=+1;
   if(disk_dir == -1 )disk_pos=5-disk_pos;
   else disk_pos= 4+disk_pos;

  type="n_a"; // important to set for any case as n/a

 if(strlen(test)==3){
  
   radius=-1;
   ich=-1;   
   type="n_a";


 }
 else if(strlen(test)==8){ // chambers

//   disk_pos=substr(test,2,1);
   radius=substr(test,4,1);
   ich=substr(test,6,2);
   type="n_a";

/////mudcsDebug2("CH");
 }
 else if(strpos(test,"GAS_SX5")>=0){ // GAS_SX5 : if csc level
  radius=-1;
  ich=-1;   
  disk_pos=-1;
  type="GAS_SX5";
 }
 else if(strpos(test,"PT100")>=0){ // PT100 : if csc level
  radius=-1;
  ich=-1;   
  disk_pos=-1;
  type="PT100";

 }
 else if(strpos(test,"MRTN")>=0){ // maraton : if csc level

  type="MRTN_1";
  radius=-1;
  ich=-1;   
  disk_pos=-1;
  ich=substr(test,strpos(test,"maraton")+7,2);

 }
 else if(strpos(test,"WNR12")>=0){ // wiener : if csc level

  type="WNR12_1";
  radius=-1;
  ich=-1;   
  disk_pos=-1;
  ich=substr(test,strpos(test,"wiener12")+8,2);
 }
 else if(strpos(test,"HV_PR")>=0){ // maraton : if csc level

  type="HV_PR";

  test=substr(test,strpos(test,"primary")+7);
  dyn_test=strsplit(test,"_");
  disk_pos=dyn_test[1];
  radius=dyn_test[2];   
  ich=dyn_test[3];
 
// disk_pos=;
//  radius=;   
//  ich=;  

 }
 else if(strpos(test,"CRATE_SNMP")>=0){ // 

  type="fwWnrCr_CSC_LV";

  test=substr(test,strpos(test,"CRATE_SNMP_")+11); 
  ich=test;
  radius=-1;
  disk_pos=-1;
// disk_pos=;
//  radius=;   
//  ich=;  

 } 
 else if(strpos(test,"ALNM")>=0){ // 
   type="ALNM_1";
   disk_pos=disk_pos; // just formal indication that I did that
 }
 else if(strlen(test) > 8 && strpos(test,"c")>=0){ // chamber devices

//   disk_pos=substr(test,2,1);
   radius=substr(test,4,1);
   ich=substr(test,6,2);
   if(strpos(test,".")>=0)
    type=substr(test,8,strpos(test,".")-8);
   else type=substr(test,8);
 }
 else if(strlen(test) > 8){ // station level devices
   radius=-1;
   if((pos3=strpos(test,"CRB_1"))<0 )pos3=5;
   ich=substr(test,4,pos3-4); // actually e.g crate(CRB) number
   //else ich=substr(test,4,1);
   if(strpos(test,".")>=0)
    type=substr(test,pos3,strpos(test,".")-pos3);
   else type=substr(test,pos3); 
 }


///////mudcsDebug2(" "+disk_pos+" "+radius+" "+ich+" "+type);


}
//=====================================================================================================================
mudcsPayAttention(){
DebugN("MMMMMMMMMMMMMMMMMMMMMMM&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&MMMMMMMMMMMMMMMMMMMMMMM");
DebugN("MMMMMMMMMMMMMMMMMMMMMMM&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&MMMMMMMMMMMMMMMMMMMMMMM");
DebugN("MMMMMMMMMMMMMMMMMMMMMMM&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&MMMMMMMMMMMMMMMMMMMMMMM");
DebugN("MMMMMMMMMMMMMMMMMMMMMMM&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&MMMMMMMMMMMMMMMMMMMMMMM");
DebugN("MMMMMMMMMMMMMMMMMMMMMMM&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&MMMMMMMMMMMMMMMMMMMMMMM");
DebugN("MMMMMMMMMMMMMMMMMMMMMMM&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&MMMMMMMMMMMMMMMMMMMMMMM");
DebugN("MMMMMMMMMMMMMMMMMMMMMMM&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&MMMMMMMMMMMMMMMMMMMMMMM");
DebugN("MMMMMMMMMMMMMMMMMMMMMMM&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&MMMMMMMMMMMMMMMMMMMMMMM");

}
//=====================================================================================================================
mudcsPayAttention2(){
DebugN("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");
DebugN("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");
DebugN("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");
DebugN("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");
DebugN("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");
DebugN("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");
DebugN("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");
DebugN("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");


}
//=====================================================================================================================

mudcsSetAliases(){



dyn_string emu_aliases;
dyn_string pair;
dpGet("EMUALIASES.",emu_aliases);
int i;

for(i=1;i<=dynlen(emu_aliases);i++){
  DebugN("mudcsSetAliases: +++++++++++"+emu_aliases[i]);
  pair=strsplit(emu_aliases[i],"#");
  dpSetAlias(pair[1],pair[2]);

 //  string devdp = fwDU_getPhysicalName(pair[2]);
 //   DebugN("mudcsSetAliases: "+devdp);
}



}

//================================================

string mudcsGetPhysical(string logical){


dyn_string emu_aliases;
dyn_string pair;
dpGet("EMUALIASES.",emu_aliases);
int i;
string ret="";
//return ret;

//if(dynlen(emu_aliases)==0)emu_aliases=CSC_fwG_g_EMU_ALIASES;

for(i=1;i<=dynlen(emu_aliases);i++){
///  DebugN("mudcsSetAliases: +++++++++++"+emu_aliases[i]);
  pair=strsplit(emu_aliases[i],"#");
  if(pair[2]==logical){DebugN("mudcsGetPhysical ++++++++++++++++++++++ "+pair[2]+" phys="+pair[1]);
  if(strpos(pair[1],".") >=0 )ret=substr(pair[1],0,strlen(pair[1])-1);
  return ret;}
}
DebugN("mudcsGetPhysical ++++++++++++++++++++++ "+logical+" phys="+"EMPTY");
return ret;

}

//================================================

string mudcsGetLogical(string phys){


dyn_string emu_aliases;
dyn_string pair;
dpGet("EMUALIASES.",emu_aliases);
int i;
string ret="";
//return ret;

//if(dynlen(emu_aliases)==0)emu_aliases=CSC_fwG_g_EMU_ALIASES;

for(i=1;i<=dynlen(emu_aliases);i++){
if(strpos(phys,".") <0 )phys=phys+".";

  pair=strsplit(emu_aliases[i],"#");

  if(strpos(pair[1],phys)>=0){DebugN("mudcsGetLogical ++++++++++++++++++++++ "+pair[2]+" phys="+pair[1]);
  return pair[2];}
}
DebugN("mudcsGetLogical ++++++++++++++++++++++ "+phys+" logical="+"EMPTY");
return ret;

}

//==================================================
//===================================================

emudcs_interlock_run_control(dyn_string test_dyn){


if(test_dyn[2] != "0"){
 dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":CSC_INTERLOCK_FLAG."),1);
}
else if(test_dyn[2] == "0"){
 dpSet(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":CSC_INTERLOCK_FLAG."),0);
}
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"CONFIRMATION"); // service update !!!

}


//===================================================
//==================================================

emudcs_hv_run_control(dyn_string test_dyn){
// +2/1/16 +3/1/16  +2/2/31,32   +3/2/31,32


int i;
string mudcs_alias;
string prefix="CSC";
string suffix="HV_1";
dyn_string dps;
dyn_string data;
string set;
dyn_string coords;

int disk_number; 
int ring_number; 
int chamber_number;
string device_type;
string  chn;
string  host;
string  subcommand;
string  voltage=test_dyn[3];
int voltage_int=voltage;
int united_status=1;


if(test_dyn[4]=="all" || test_dyn[4]=="ALL"){
 for(i=1;i<=dynlen(CSC_fwG_g_SLICE_DPOINTS);i++){
  data[i]=CSC_fwG_g_SLICE_DPOINTS[i]+suffix;
//  data[i]=mudcsNameByAliasGet(data[i]);
 }
 data=mudcsNamesByAliasesGet(data);
}
else{
   if(dpExists(mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[4]+suffix)))
 dynAppend(data,mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[4]+suffix));
}

 mudcsConvertData2Fsm(suffix, data, dps);

//if(test_dyn[2] == "0" && voltage_int >=0){
if(voltage_int >=0){
 for(i=1;i<=dynlen(dps);i++){

 mudcs_alias=mudcsAliasNameGet(dps[i]);
  mudcsNameToPosConvert
  (mudcs_alias/*dps[i]*/, disk_number, ring_number, chamber_number, device_type);

  retrieveCoordinateSet
  ("HV", dps[i], "HVsetList", disk_number, ring_number, chamber_number, device_type, set);
  coords=strsplit(set,";");

  if(dynlen(coords)!=4)return;

  chn="255";
  if(coords[4]=="1")chn="253";
  if(coords[4]=="2")chn="254";

  host=coords[1];
  subcommand="HVCMD;"+coords[2]+";"+coords[3]+";"+chn+";"+"7"+";"+voltage+";"+"-1";

  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

 } // for

} // if(test_dyn[2] != "0")
//---------

if(voltage_int <0){

//----------
int current_status;
if(test_dyn[2] != "0"){
 for(i=1;i<=dynlen(dps);i++){  
  if(strpos(dps[i],"Broker")>=0)continue;
  dpGet(mudcsAddSystem(dps[i]+".status"),current_status);
  if(current_status == -2 && united_status > 0){ 
      united_status *=-1;
//---
/*
  dyn_string dyn_debug;
    dpGet("dyn_debug2.",dyn_debug);
    dynAppend(dyn_debug,dps[i]+" "+mudcsAddSystem(dps[i]+".status"));
    dpSetWait("dyn_debug2.",dyn_debug);
*/
//----
  }
  else united_status *=2;
  if(test_dyn[4] !="all" && test_dyn[4] !="ALL" && current_status != -2)dpSet(mudcsAddSystem(dps[i]+".status"),2);
 }
 if(test_dyn[4]=="all" || test_dyn[4]=="ALL")emudcs_hv_slice_test_on_new(3600);
}
else if(test_dyn[2] == "0"){
 for(i=1;i<=dynlen(dps);i++){  
  if(strpos(dps[i],"Broker")>=0)continue;
  dpGet(mudcsAddSystem(dps[i]+".status"),current_status);
  if(current_status == -2 && united_status > 0)united_status *=-1;
  else united_status *=2;
  if(test_dyn[4] !="all" && test_dyn[4] !="ALL" && current_status != -2)dpSet(mudcsAddSystem(dps[i]+".status"),0);
 }
 if(test_dyn[4]=="all" || test_dyn[4]=="ALL")emudcs_hv_slice_test_off_new(0);
}

}
else united_status=2;


if(united_status == 1)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"NOSUCHDEVICES"); // service update !!!
else if(united_status == -1)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"ERROR"); // service update !!!
else if(united_status <= -2)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"SOMEDEVICEERROR"); // service update !!!
else 
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"CONFIRMATION"); // service update !!!

//-----------

}

//===================================================

emudcs_lv_run_control(dyn_string test_dyn){
// +2/1/16 +3/1/16  +2/2/31,32   +3/2/31,32

int i;
string prefix="CSC";
string suffix="LV_1";
dyn_string dps;
dyn_string data;
int united_status=1;


if(test_dyn[4]=="all" || test_dyn[4]=="ALL"){
 for(i=1;i<=dynlen(CSC_fwG_g_SLICE_DPOINTS);i++){
  data[i]=CSC_fwG_g_SLICE_DPOINTS[i]+suffix;
//  data[i]=mudcsNameByAliasGet(data[i]);
 }
 data=mudcsNamesByAliasesGet(data);
}
else{
//   if(dpExists(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[4]+suffix))
   if(dpExists(mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[4]+suffix)))
 dynAppend(data,mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[4]+suffix));
}

 mudcsConvertData2Fsm(suffix, data, dps);

int current_status;
if(test_dyn[2] != "0"){
 for(i=1;i<=dynlen(dps);i++){  
  if(strpos(dps[i],"Broker")>=0)continue;
  dpGet(mudcsAddSystem(dps[i]+".status"),current_status);
  if(current_status != -2){dpSet(mudcsAddSystem(dps[i]+".status"),2); united_status *=2;}
  else if(united_status > 0)united_status *=-1;
 }
}
else if(test_dyn[2] == "0"){
 for(i=1;i<=dynlen(dps);i++){
  if(strpos(dps[i],"Broker")>=0)continue;
  dpGet(mudcsAddSystem(dps[i]+".status"),current_status);
  if(current_status != -2){dpSet(mudcsAddSystem(dps[i]+".status"),0);united_status *=2;}
  else if(united_status > 0)united_status *=-1;
 }
}


if(united_status == 1)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"NOSUCHDEVICES"); // service update !!!
else if(united_status == -1)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"ERROR"); // service update !!!
else if(united_status <= -2)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"SOMEDEVICEERROR"); // service update !!!
else 
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"CONFIRMATION"); // service update !!!


}
//===================================================

emudcs_mrtn_run_control(dyn_string test_dyn){

int i;
string prefix="CSC";
string suffix="MRTN_1";

dyn_string data;
dyn_string fsm;

int united_status=1;

if(test_dyn[3]=="all" || test_dyn[3]=="ALL"){
  data=dpNames("*","MRTN_1_d");
}
else{
//   if(dpExists(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[3]+suffix))
   if(dpExists(mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[3]+suffix)))
 dynAppend(data,mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[3]+suffix));
}

mudcsConvertData2Fsm("MRTN_1", data, fsm);

//-----

int current_status;
if(test_dyn[2] != "0"){
 for(i=1;i<=dynlen(fsm);i++){  
  if(strpos(fsm[i],"Broker")>=0)continue;
  dpGet(mudcsAddSystem(fsm[i]+".status"),current_status);
  if(current_status != -2){dpSet(mudcsAddSystem(fsm[i]+".status"),2);united_status *=2;}
  else if(united_status > 0)united_status *=-1;
 }
}
else if(test_dyn[2] == "0"){
 for(i=1;i<=dynlen(fsm);i++){
  if(strpos(fsm[i],"Broker")>=0)continue;
  dpGet(mudcsAddSystem(fsm[i]+".status"),current_status);
  if(current_status != -2){dpSet(mudcsAddSystem(fsm[i]+".status"),0);united_status *=2;}
  else if(united_status > 0)united_status *=-1;
 }
}


if(united_status == 1)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"NOSUCHDEVICES"); // service update !!!
else if(united_status == -1)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"ERROR"); // service update !!!
else if(united_status <= -2)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"SOMEDEVICEERROR"); // service update !!!
else 
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"CONFIRMATION"); // service update !!!

}
//===================================================

emudcs_crb_run_control(dyn_string test_dyn){

int i;
string prefix="CSC";
string suffix="CRB_1";

dyn_string data;
dyn_string fsm;

int united_status=1;

if(test_dyn[3]=="all" || test_dyn[3]=="ALL"){
  data=dpNames("*","CRB_1_d");
}
else{
//   if(dpExists(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[3]+suffix))
   if(dpExists(mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[3]+suffix)))
 dynAppend(data,mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[3]+suffix));
}

mudcsConvertData2Fsm("CRB_1", data, fsm);
//------

int current_status;
if(test_dyn[2] != "0"){
 for(i=1;i<=dynlen(fsm);i++){  
  if(strpos(fsm[i],"Broker")>=0)continue;
  dpGet(mudcsAddSystem(fsm[i]+".status"),current_status);
  if(current_status != -2){dpSet(mudcsAddSystem(fsm[i]+".status"),2);united_status *=2;delay(10);}
  else if(united_status > 0)united_status *=-1;
 }
}
else if(test_dyn[2] == "0"){
 for(i=1;i<=dynlen(fsm);i++){
  if(strpos(fsm[i],"Broker")>=0)continue;
  dpGet(mudcsAddSystem(fsm[i]+".status"),current_status);
  if(current_status != -2){dpSet(mudcsAddSystem(fsm[i]+".status"),0);united_status *=2;}
  else if(united_status > 0)united_status *=-1;
 }
}

if(united_status == 1)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"NOSUCHDEVICES"); // service update !!!
else if(united_status == -1)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"ERROR"); // service update !!!
else if(united_status <= -2)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"SOMEDEVICEERROR"); // service update !!!
else 
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"CONFIRMATION"); // service update !!!



}


//===================================================

emudcs_chip_run_control(dyn_string test_dyn){

int i;
string prefix="CSC";
string suffix="CHIP_1";

dyn_string data;
dyn_string fsm;

int united_status=1;

if(test_dyn[3]=="all" || test_dyn[3]=="ALL"){
  data=dpNames("*","CHIP_1_d");
}
else{
//   if(dpExists(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[3]+suffix))
   if(dpExists(mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[3]+suffix)))
 dynAppend(data,mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[3]+suffix));
}

mudcsConvertData2Fsm(suffix, data, fsm);
//------
int current_status;

 for(i=1;i<=dynlen(fsm);i++){ 
  if(strpos(fsm[i],"Broker")>=0)continue;
  dpGet(mudcsAddSystem(fsm[i]+".status"),current_status);
  if(current_status != -2){united_status *=2;}
  else if(united_status > 0)united_status *=-1;

  if(test_dyn[2] == "0") mudcsCommand("LV",data[i],"hard_reset_all");
  else if(test_dyn[2] == "1") mudcsCommand("LV",data[i],"program_alct");  
  else if(test_dyn[2] == "2") mudcsCommand("LV",data[i],"program_daqmb"); 
  else if(test_dyn[2] == "3") mudcsCommand("LV",data[i],"program_tmb"); 
  else if(test_dyn[2] == "4") mudcsCommand("LV",data[i],"program_all_cfebs"); 
  else if(test_dyn[2] == "5") mudcsCommand("LV",data[i],"program_mpc"); 

 }


if(united_status == 1)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"NOSUCHDEVICES"); // service update !!!
else if(united_status == -1)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"ERROR"); // service update !!!
else if(united_status <= -2)
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"SOMEDEVICEERROR"); // service update !!!
else 
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"CONFIRMATION"); // service update !!!


}

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================


emudcs_mrtn_rc_read(dyn_string test_dyn){

int i,j;
string prefix="CSC";
string suffix="MRTN_1";

dyn_string data;
dyn_string fsm;
string service_data;
float value;
string s_value;
int pos;
int fsm_status;
string coord;
time last_set_status;
string last_set_status_string;

if(test_dyn[2]=="all" || test_dyn[2]=="ALL"){
  data=dpNames("*","MRTN_1_d");
}
else{
 dynAppend(data,mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[2]+suffix));
 if(!dpExists(mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[2]+suffix)))test_dyn[2] =" ITEM DOES NOT EXIST !!!!!";
}

mudcsConvertData2Fsm("MRTN_1", data, fsm);
//--------------------------------------------------
 if(test_dyn[1]=="MRTNPREREAD"){
  for(i=1;i<=dynlen(fsm);i++){  
    if(strpos(data[i],"Broker")>=0)continue; 
   mudcsCommand("MRTN",fsm[i], "get_data");
  }
  dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"CONFIRMATION"); // service update !!!
 }
//--------------------------------------------------
 else{
  for(i=1;i<=dynlen(fsm);i++){  
    if(strpos(data[i],"Broker")>=0)continue;

    string data_alias=mudcsAliasNameGet(data[i]);
    if(strpos(test_dyn[2],"DOES NOT EXIST")<0)test_dyn[2]=substr(data_alias,strpos(data_alias,"CSC")+3,strpos(data_alias,suffix)-strpos(data_alias,"CSC")-3);
//    if(strpos(test_dyn[2],"DOES NOT EXIST")<0)test_dyn[2]=substr(data[i],strpos(data[i],"CSC")+3,strpos(data[i],suffix)-strpos(data[i],"CSC")-3);
    strreplace(test_dyn[2],"d","s"); 

    dpGet(mudcsAddSystem(fsm[i]+".status"),fsm_status);
    dpGet(mudcsAddSystem(data[i]+".dynatemTcpIp"),coord);
    dpGet(mudcsAddSystem(data[i]+".dynatemTcpIp:_online.._stime"),last_set_status);
    last_set_status_string=last_set_status;

    if(strpos(coord,"WIENER") < 0)fsm_status=-2;
    if((getCurrentTime()-last_set_status) > 60*60)fsm_status=-2; // 60*60 seconds

   service_data += "service=MRTN ;";
   service_data += "item="+test_dyn[2]+";\n";

    if(fsm_status==-2)service_data += "status="+"ERROR: NO COMMUNICATION TO CONTROL SERVER"+";\n";
    else if(fsm_status==-1)service_data += "status="+"ALERT: THE DEVICE IS IN ALERT STATUS!!!"+";\n";
    else if(fsm_status==0)service_data += "status="+"THE DEVICE IS OFF"+";\n";
    else if(fsm_status>0)service_data += "status="+"THE DEVICE IS ON"+";\n";
    else service_data += "status="+"UNKNOWN: "+fsm_status+";\n";

    if(fsm_status==-2)last_set_status_string="NOT APPLICABLE";
    service_data += "time_stamp="+last_set_status_string+";\n";

    dpGet(mudcsAddSystem(data[i]+".data.general_status"),value);
    service_data += "state="+value+";\n";

   for(j=1;j<=6;j++){ 
    dpGet(mudcsAddSystem(data[i]+".data.channel"+j+".item1"),value);
//---
    s_value=value;
    pos=strpos(s_value,".");
    if(pos >=0 && (strlen(s_value)-1-pos)>3){
     s_value=substr(s_value,0,pos+4);
    }
//---   
    service_data += "v"+j+"="+s_value+" ;";
    if(j==6)service_data += "\n";
   }
   for(j=1;j<=6;j++){ 
    dpGet(mudcsAddSystem(data[i]+".data.channel"+j+".item2"),value);
//
    s_value=value;
    pos=strpos(s_value,".");
    if(pos >=0 && (strlen(s_value)-1-pos)>3){
     s_value=substr(s_value,0,pos+4);
    }
//--- 
    service_data += "i"+j+"="+s_value+" ;";
    if(j==6)service_data += "\n";
   }


  } // for over dpoints
   dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),service_data); // service update !!!    
 } //else
//--------------------------------------------------
}

//===================================================================================
//===================================================================================

emudcs_crb_rc_read(dyn_string test_dyn){

int i,j,jj;
string prefix="CSC";
string suffix="CRB_1";

dyn_string data;
dyn_string fsm;
string service_data;
float value;
string s_value;
int pos, pos1;
int fsm_status;
string coord;
time last_set_status;
string last_set_status_string;
dyn_string voltage_pair_items=makeDynString("v33","v34","v50");
dyn_string voltage_items=makeDynString("vcc_33","vcc_50","vccb_33","vccb_50","v15_1","v15_2");

if(test_dyn[2]=="all" || test_dyn[2]=="ALL"){
  data=dpNames("*","CRB_1_d");
}
else{
 dynAppend(data,mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[2]+suffix));
if(!dpExists(mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[2]+suffix)))test_dyn[2]=" ITEM DOES NOT EXIST !!!!!";
}

mudcsConvertData2Fsm("CRB_1", data, fsm);
//--------------------------------------------------
 if(test_dyn[1]=="CRBPREREAD"){
  for(i=1;i<=dynlen(fsm);i++){
    if(strpos(data[i],"Broker")>=0)continue;   
   mudcsCommand("CRB",fsm[i], "get_data");
  }
  dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"CONFIRMATION"); // service update !!!
 }
//--------------------------------------------------
 else{
  for(i=1;i<=dynlen(fsm);i++){ 
    if(strpos(data[i],"Broker")>=0)continue;

    string data_alias=mudcsAliasNameGet(data[i]);
    if(strpos(test_dyn[2],"DOES NOT EXIST")<0)test_dyn[2]=substr(data_alias,strpos(data_alias,"CSC")+3,strpos(data_alias,suffix)-strpos(data_alias,"CSC")-3);
//    if(strpos(test_dyn[2],"DOES NOT EXIST")<0)test_dyn[2]=substr(data[i],strpos(data[i],"CSC")+3,strpos(data[i],suffix)-strpos(data[i],"CSC")-3);
    strreplace(test_dyn[2],"d","s"); 

    dpGet(mudcsAddSystem(fsm[i]+".status"),fsm_status);
    dpGet(mudcsAddSystem(data[i]+".dynatemTcpIp"),coord);
    dpGet(mudcsAddSystem(data[i]+".dynatemTcpIp:_online.._stime"),last_set_status);
    last_set_status_string=last_set_status;

    if(strpos(coord,"LVCB") < 0)fsm_status=-2;
    if((getCurrentTime()-last_set_status) > 60*60)fsm_status=-2; // 60*60 seconds

   service_data += "service=CRB ;";
   service_data += "item="+test_dyn[2]+";\n";

    if(fsm_status==-2)service_data += "status="+"ERROR: NO COMMUNICATION TO CONTROL SERVER"+";\n";
    else if(fsm_status==-1)service_data += "status="+"ALERT: THE DEVICE IS IN ALERT STATUS!!!"+";\n";
    else if(fsm_status==0)service_data += "status="+"THE DEVICE IS OFF"+";\n";
    else if(fsm_status>0)service_data += "status="+"THE DEVICE IS ON"+";\n";
    else service_data += "status="+"UNKNOWN: "+fsm_status+";\n";

    if(fsm_status==-2)last_set_status_string="NOT APPLICABLE";
    service_data += "time_stamp="+last_set_status_string+";\n";

    dpGet(mudcsAddSystem(data[i]+".data.status"),value);
    service_data += "state="+value+";\n";
//---
   for(jj=1;jj<=6;jj++){
    dpGet(mudcsAddSystem(data[i]+".data."+voltage_items[jj]),value); 
     s_value=value;
     pos=strpos(s_value,".");
     if(pos >=0 && (strlen(s_value)-1-pos)>3){
      s_value=substr(s_value,0,pos+4);
     }
     service_data += voltage_items[jj]+"="+s_value+" ;";
   } // for over jj
    service_data += "\n";
//---
   for(j=1;j<=9;j++){ 
    for(jj=1;jj<=3;jj++){ 
     dpGet(mudcsAddSystem(data[i]+".data.pair"+j+"."+voltage_pair_items[jj]),value);

     s_value=value;
     pos=strpos(s_value,".");
     if(pos >=0 && (strlen(s_value)-1-pos)>3){
      s_value=substr(s_value,0,pos+4);
     }
     service_data += voltage_pair_items[jj]+"="+s_value+" ;";
    } // for over jj
    service_data += "\n";
   }
//---
 
  } // for over dpoints
   dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),service_data); // service update !!!   
 } //else
//--------------------------------------------------
}
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

emudcs_lv_rc_read(dyn_string test_dyn){

int i,ii,j,jj;
string prefix="CSC";
string suffix="LV_1";
string type="LV";
int items_number=5;

dyn_string data;
dyn_string fsm;
string service_data;
float value;
string s_value;
int pos;
int fsm_status;
string coord;
time last_set_status;
string last_set_status_string;
dyn_dyn_string cfeb_items;
dyn_dyn_string alct_items;

cfeb_items[1]=makeDynString("v33","c33");
cfeb_items[2]=makeDynString("v50","c50");
cfeb_items[2]=makeDynString("v60","c60");

alct_items[1]=makeDynString("v18","v33","v55","v56");
alct_items[2]=makeDynString("c18","c33","c55","c56");

if(test_dyn[2]=="all" || test_dyn[2]=="ALL"){
 for(i=1;i<=dynlen(CSC_fwG_g_SLICE_DPOINTS);i++){
  data[i]=CSC_fwG_g_SLICE_DPOINTS[i]+suffix;
//  data[i]=mudcsNameByAliasGet(data[i]);

////DebugTN("=============================================>>>> "+data[i]);

 }
 data=mudcsNamesByAliasesGet(data);
}
else{
 dynAppend(data,mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[2]+suffix));
if(!dpExists(mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[2]+suffix)))test_dyn[2]=" ITEM DOES NOT EXIST !!!!!";
}

mudcsConvertData2Fsm(suffix, data, fsm);
//--------------------------------------------------
 if(test_dyn[1]==type+"PREREAD"){
  for(i=1;i<=dynlen(fsm);i++){
    if(strpos(data[i],"Broker")>=0)continue;   
   mudcsCommand(type,fsm[i], "get_data");
  }
  dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"CONFIRMATION"); // service update !!!
 }
//--------------------------------------------------
 else{
  for(i=1;i<=dynlen(fsm);i++){
/////DebugTN("=============================================>>>>==================== "+fsm[i]+" "+dynlen(fsm)+" "+dynlen(data));
    if(strpos(data[i],"Broker")>=0)continue;

    string data_alias=mudcsAliasNameGet(data[i]);
    if(strpos(test_dyn[2],"DOES NOT EXIST")<0)test_dyn[2]=substr(data_alias,strpos(data_alias,"CSC")+3,strpos(data_alias,suffix)-strpos(data_alias,"CSC")-3);
//    if(strpos(test_dyn[2],"DOES NOT EXIST")<0)test_dyn[2]=substr(data[i],strpos(data[i],"CSC")+3,strpos(data[i],suffix)-strpos(data[i],"CSC")-3);
    strreplace(test_dyn[2],"d","s"); 

    dpGet(mudcsAddSystem(fsm[i]+".status"),fsm_status);
    dpGet(mudcsAddSystem(data[i]+".dynatemTcpIp"),coord);
    dpGet(mudcsAddSystem(data[i]+".dynatemTcpIp:_online.._stime"),last_set_status);
    last_set_status_string=last_set_status;

    if(strpos(coord,".0.0.") < 0 && strpos(coord,":00:00:") < 0 && strpos(coord,"LVCB") < 0 )fsm_status=-2;
    if((getCurrentTime()-last_set_status) > 60*60)fsm_status=-2; // 60*60 seconds

   service_data += "service="+type+" ;";
   service_data += "item="+test_dyn[2]+";\n";

    if(fsm_status==-2)service_data += "status="+"ERROR: NO COMMUNICATION TO CONTROL SERVER"+";\n";
    else if(fsm_status==-1)service_data += "status="+"ALERT: THE DEVICE IS IN ALERT STATUS!!!"+";\n";
    else if(fsm_status==0)service_data += "status="+"THE DEVICE IS OFF"+";\n";
    else if(fsm_status>0)service_data += "status="+"THE DEVICE IS ON"+";\n";
    else service_data += "status="+"UNKNOWN: "+fsm_status+";\n";

    if(fsm_status==-2)last_set_status_string="NOT APPLICABLE";
    service_data += "time_stamp="+last_set_status_string+";\n";

//    dpGet(mudcsAddSystem(data[i]+".data.status"),value);
//    service_data += "state="+value+";\n";
//---
  for(ii=1;ii<=dynlen(alct_items);ii++){
   for(jj=1;jj<=dynlen(alct_items[ii]);jj++){ 
    dpGet(mudcsAddSystem(data[i]+".data.Alct_o."+alct_items[ii][jj]),value); 
     s_value=value;
     pos=strpos(s_value,".");
     if(pos >=0 && (strlen(s_value)-1-pos)>3){
      s_value=substr(s_value,0,pos+4);
     }
     service_data += alct_items[ii][jj]+"="+s_value+" ;";
   } // for over jj
    service_data += "\n";
  } // for ii
//---
  for(ii=1;ii<=dynlen(cfeb_items);ii++){
   for(jj=1;jj<=dynlen(cfeb_items[ii]);jj++){
    for(j=1;j<=items_number;j++){ 
     dpGet(mudcsAddSystem(data[i]+".data.Cfeb_o."+cfeb_items[ii][jj]+".v"+j),value);

     s_value=value;
     pos=strpos(s_value,".");
     if(pos >=0 && (strlen(s_value)-1-pos)>3){
      s_value=substr(s_value,0,pos+4);
     }
     service_data += cfeb_items[ii][jj]+"="+s_value+" ;";
    } // for over jj
    service_data += "\n";
   } // for over jj
  } // for over ii
//---
//   dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),service_data); // service update !!!    

  } // for over fsm-s

//DebugTN("=====>"+service_data);
//service_data="test1;test2;test3";
dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),service_data); // service update !!!    

 } //else
//--------------------------------------------------
}
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

emudcs_temp_rc_read(dyn_string test_dyn){

int i,ii,j,jj;
string prefix="CSC";
string suffix="TEMP_1";
string type="TEMP";
//////////int items_number=5;

dyn_string data;
dyn_string fsm;
string service_data;
float value;
string s_value;
int pos;
int fsm_status;
string coord;
time last_set_status;
string last_set_status_string;

dyn_dyn_string temp_items;

temp_items[1]=makeDynString("t_dmb");
temp_items[2]=makeDynString("t_alct");
temp_items[3]=makeDynString("t_cfeb","t_cfeb","t_cfeb","t_cfeb","t_cfeb");


if(test_dyn[2]=="all" || test_dyn[2]=="ALL"){
 for(i=1;i<=dynlen(CSC_fwG_g_SLICE_DPOINTS);i++){
  data[i]=CSC_fwG_g_SLICE_DPOINTS[i]+suffix;
//  data[i]=mudcsNameByAliasGet(data[i]);
 }
 data=mudcsNamesByAliasesGet(data);
}
else{
 dynAppend(data,mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[2]+suffix));
if(!dpExists(mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[2]+suffix)))test_dyn[2]=" ITEM DOES NOT EXIST !!!!!";
}

mudcsConvertData2Fsm(suffix, data, fsm);
//--------------------------------------------------
 if(test_dyn[1]==type+"PREREAD"){
  for(i=1;i<=dynlen(fsm);i++){
    if(strpos(data[i],"Broker")>=0)continue;   
   mudcsCommand("LV",fsm[i], "get_data");
  }
  dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"CONFIRMATION"); // service update !!!
 }
//--------------------------------------------------
 else{
  for(i=1;i<=dynlen(fsm);i++){  
    if(strpos(data[i],"Broker")>=0)continue;

    string data_alias=mudcsAliasNameGet(data[i]);
    if(strpos(test_dyn[2],"DOES NOT EXIST")<0)test_dyn[2]=substr(data_alias,strpos(data_alias,"CSC")+3,strpos(data_alias,suffix)-strpos(data_alias,"CSC")-3);
//    if(strpos(test_dyn[2],"DOES NOT EXIST")<0)test_dyn[2]=substr(data[i],strpos(data[i],"CSC")+3,strpos(data[i],suffix)-strpos(data[i],"CSC")-3);
    strreplace(test_dyn[2],"d","s"); 

    dpGet(mudcsAddSystem(fsm[i]+".status"),fsm_status);
    dpGet(mudcsAddSystem(data[i]+".dynatemTcpIp"),coord);
    dpGet(mudcsAddSystem(data[i]+".dynatemTcpIp:_online.._stime"),last_set_status);
    last_set_status_string=last_set_status;

    if(strpos(coord,".0.0.") < 0 && strpos(coord,":00:00:") < 0 && strpos(coord,"LVCB") < 0 )fsm_status=-2;
    if((getCurrentTime()-last_set_status) > 60*60)fsm_status=-2; // 60*60 seconds

   service_data += "service="+type+" ;";
   service_data += "item="+test_dyn[2]+";\n";

    if(fsm_status==-2)service_data += "status="+"ERROR: NO COMMUNICATION TO CONTROL SERVER"+";\n";
    else if(fsm_status==-1)service_data += "status="+"ALERT: THE DEVICE IS IN ALERT STATUS!!!"+";\n";
    else if(fsm_status==0)service_data += "status="+"THE DEVICE IS OFF"+";\n";
    else if(fsm_status>0)service_data += "status="+"THE DEVICE IS ON"+";\n";
    else service_data += "status="+"UNKNOWN: "+fsm_status+";\n";

    if(fsm_status==-2)last_set_status_string="NOT APPLICABLE";
    service_data += "time_stamp="+last_set_status_string+";\n";

//    dpGet(mudcsAddSystem(data[i]+".data.status"),value);
//    service_data += "state="+value+";\n";
//---
  for(ii=1;ii<=dynlen(temp_items);ii++){
   for(jj=1;jj<=dynlen(temp_items[ii]);jj++){ 
    dpGet(mudcsAddSystem(data[i]+".data."+temp_items[ii][jj]+".v"+jj),value); 
     s_value=value;
     pos=strpos(s_value,".");
     if(pos >=0 && (strlen(s_value)-1-pos)>3){
      s_value=substr(s_value,0,pos+4);
     }
     service_data += temp_items[ii][jj]+jj+"="+s_value+" ;";
   } // for over jj
    service_data += "\n";
  } // for ii

  
  } // for ober fsm-s
   dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),service_data); // service update !!!  
 } //else
//--------------------------------------------------
}

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

emudcs_hv_rc_read(dyn_string test_dyn){

int i,ii,j,jj;
string ii_s;
string prefix="CSC";
string suffix="HV_1";
string type="HV";
int items_number;
int first_channel;
int host_id;

dyn_string data;
dyn_string fsm;
string service_data;
int value;
float f_value;
string s_value;
int pos;
int fsm_status;
string coord;


dyn_string dyn_coord;
time last_set_status;
string last_set_status_string;

dyn_string hv_items;

hv_items=makeDynString("vmon","imon");


if(test_dyn[2]=="all" || test_dyn[2]=="ALL"){
 for(i=1;i<=dynlen(CSC_fwG_g_SLICE_DPOINTS);i++){
  data[i]=CSC_fwG_g_SLICE_DPOINTS[i]+suffix;
//  data[i]=mudcsNameByAliasGet(data[i]);
 }
 data=mudcsNamesByAliasesGet(data);
}
else{
  dynAppend(data,mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[2]+suffix));
if(!dpExists(mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+test_dyn[2]+suffix)))test_dyn[2]=" ITEM DOES NOT EXIST !!!!!";

}

mudcsConvertData2Fsm(suffix, data, fsm);
//--------------------------------------------------
 if(test_dyn[1]==type+"PREREAD"){
  for(i=1;i<=dynlen(fsm);i++){  
    if(strpos(data[i],"Broker")>=0)continue; 
   mudcsCommand(type,fsm[i], "get_data");
  }
  dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"CONFIRMATION"); // service update !!!
 }
//--------------------------------------------------
 else{
  for(i=1;i<=dynlen(fsm);i++){ 
    if(strpos(data[i],"Broker")>=0)continue;

    string data_alias=mudcsAliasNameGet(data[i]);
    if(strpos(test_dyn[2],"DOES NOT EXIST")<0)test_dyn[2]=substr(data_alias,strpos(data_alias,"CSC")+3,strpos(data_alias,suffix)-strpos(data_alias,"CSC")-3);
//    if(strpos(test_dyn[2],"DOES NOT EXIST")<0)test_dyn[2]=substr(data[i],strpos(data[i],"CSC")+3,strpos(data[i],suffix)-strpos(data[i],"CSC")-3);
    strreplace(test_dyn[2],"d","s"); 

    dpGet(mudcsAddSystem(fsm[i]+".status"),fsm_status);
    dpGet(mudcsAddSystem(fsm[i]+".coord"),coord);
    dpGet(mudcsAddSystem(data[i]+".data.host_hostid"),host_id);
    dpGet(mudcsAddSystem(data[i]+".data.host_hostid:_online.._stime"),last_set_status);
    last_set_status_string=last_set_status;

    dyn_coord=strsplit(coord,";");
    if(dyn_coord[4]=="0"){items_number=30;first_channel=1;}
    else if(dyn_coord[4]=="1"){items_number=18;first_channel=1;}
    else if(dyn_coord[4]=="2"){items_number=18;first_channel=19;}

    if(host_id <=0 )fsm_status=-2;
    if((getCurrentTime()-last_set_status) > 60*60)fsm_status=-2; // 60*60 seconds

   service_data += "service="+type+" ;";
   service_data += "item="+test_dyn[2]+";\n";

    if(fsm_status==-2)service_data += "status="+"ERROR: NO COMMUNICATION TO CONTROL SERVER"+";\n";
    else if(fsm_status==-1)service_data += "status="+"ALERT: THE DEVICE IS IN ALERT STATUS!!!"+";\n";
    else if(fsm_status==0)service_data += "status="+"THE DEVICE IS OFF"+";\n";
    else if(fsm_status>0)service_data += "status="+"THE DEVICE IS ON"+";\n";
    else service_data += "status="+"UNKNOWN: "+fsm_status+";\n";

    if(fsm_status==-2)last_set_status_string="NOT APPLICABLE";
    service_data += "time_stamp="+last_set_status_string+";\n";

 /////   dpGet(mudcsAddSystem(data[i]+".data.status"),value);
 /////   service_data += "state="+value+";\n";
//---
//---
   for(jj=1;jj<=dynlen(hv_items);jj++){
    for(j=first_channel;j<=first_channel+items_number-1;j++){ 
     dpGet(mudcsAddSystem(data[i]+".data"+".v"+j+"."+hv_items[jj]),value);
     ii=j-first_channel+1;
     if(ii<=9)ii_s="0"+ii;
     else ii_s=ii;
     f_value=value;
     if(hv_items[jj]=="imon")f_value /= 11.;
     s_value=f_value;
     pos=strpos(s_value,".");
     if(pos >=0 && (strlen(s_value)-1-pos)>3){
      s_value=substr(s_value,0,pos+4);
     }
     service_data += substr(hv_items[jj],0,1)+"_ch"+ii_s+"="+s_value+" ;";
     if(items_number == 30 && !(ii%10))service_data += "\n";
     else if(items_number == 18 && !(ii%9))service_data += "\n";

    } // for over jj
///////    service_data += "\n";
   } // for over jj
//---
  
  } // for ober fsm-s
   dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),service_data); // service update !!!  
 } //else
//--------------------------------------------------
}

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

emudcs_wth_rc_read(dyn_string test_dyn){

int i,ii,j,jj;
string prefix="CSC";
string suffix="WTH_SX5";
string type="WTH";
//////////int items_number=5;

dyn_string data;
dyn_string fsm;
string service_data;
float value;
string s_value;
int pos;
int fsm_status;
string coord;

time last_set_status;
string last_set_status_string;

dyn_dyn_string temp_items;

temp_items[1]=makeDynString("TEMPERATURE","TEMPERATURE","TEMPERATURE","TEMPERATURE");
temp_items[2]=makeDynString("HUMIDITY","HUMIDITY","HUMIDITY");
temp_items[3]=makeDynString("PRESSURE");

 dynAppend(data,mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+suffix));

mudcsConvertData2Fsm(suffix, data, fsm);
//--------------------------------------------------
 if(test_dyn[1]==type+"PREREAD"){
  for(i=1;i<=dynlen(fsm);i++){
    if(strpos(data[i],"Broker")>=0)continue;   
   mudcsCommandCscLevel(fsm[i]+".status", "get_data","WTH_SX5");
  }
  dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"CONFIRMATION"); // service update !!!
 }
//--------------------------------------------------
 else{
  for(i=1;i<=dynlen(fsm);i++){  
    if(strpos(data[i],"Broker")>=0)continue; 
    dpGet(mudcsAddSystem(fsm[i]+".status"),fsm_status);
    dpGet(mudcsAddSystem(data[i]+".dynatemTcpIp"),coord);
    dpGet(mudcsAddSystem(data[i]+".dynatemTcpIp:_online.._stime"),last_set_status);
    last_set_status_string=last_set_status;

    if(strpos(coord,"WEATHERSTATION") < 0)fsm_status=-2;
    if((getCurrentTime()-last_set_status) > 60*60)fsm_status=-2; // 60*60 seconds

   service_data += type+";";
   service_data += "WEATHERSTATION;";

    if(fsm_status==-2)service_data += "status="+"ERROR: NO COMMUNICATION TO CONTROL SERVER"+";\n";
    else if(fsm_status==-1)service_data += "status="+"ALERT: THE DEVICE IS IN ALERT STATUS!!!"+";\n";
    else if(fsm_status==0)service_data += "status="+"THE DEVICE IS OFF"+";\n";
    else if(fsm_status>0)service_data += "status="+"THE DEVICE IS ON"+";\n";
    else service_data += "status="+"UNKNOWN: "+fsm_status+";\n";

    if(fsm_status==-2)last_set_status_string="NOT APPLICABLE";
    service_data += "time_stamp="+last_set_status_string+";\n";

////    dpGet(mudcsAddSystem(data[i]+".data.status"),value);
////    service_data += "state="+value+";\n";
//---
  for(ii=1;ii<=dynlen(temp_items);ii++){
   for(jj=1;jj<=dynlen(temp_items[ii]);jj++){ 
    dpGet(mudcsAddSystem(data[i]+".data."+temp_items[ii][jj]+".sensor"+jj),value); 
     s_value=value;
     pos=strpos(s_value,".");
     if(pos >=0 && (strlen(s_value)-1-pos)>3){
      s_value=substr(s_value,0,pos+4);
     }
     service_data += temp_items[ii][jj]+"_sensor"+jj+"="+s_value+" ;";
   } // for over jj
    service_data += "\n";
  } // for ii

  
  } // for ober fsm-s
   dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),service_data); // service update !!!  
 } //else
//--------------------------------------------------
}
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

emudcs_pt100_rc_read(dyn_string test_dyn){

int i,ii,j,jj;
string prefix="CSC";
string suffix="PT100";
string type="PT100";
//////////int items_number=5;

dyn_string data;
dyn_string fsm;
string service_data;
float value;
string s_value;
int pos;
int fsm_status;
string coord;

time last_set_status;
string last_set_status_string;

dyn_dyn_string temp_items;

temp_items[1]=makeDynString("TEMPERATURE","TEMPERATURE","TEMPERATURE","TEMPERATURE");
temp_items[2]=makeDynString("HUMIDITY","HUMIDITY","HUMIDITY");
temp_items[3]=makeDynString("PRESSURE");

 dynAppend(data,mudcsNameByAliasGet(CSC_fwG_g_SYSTEM_NAME+":"+prefix+suffix));

mudcsConvertData2Fsm(suffix, data, fsm);
//--------------------------------------------------
 if(test_dyn[1]==type+"PREREAD"){
  for(i=1;i<=dynlen(fsm);i++){
    if(strpos(data[i],"Broker")>=0)continue;   
   mudcsCommandCscLevel(fsm[i]+".status", "get_data",type);
  }
  dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"CONFIRMATION"); // service update !!!
 }
//--------------------------------------------------
 else{
  for(i=1;i<=dynlen(fsm);i++){  
    if(strpos(data[i],"Broker")>=0)continue; 
    dpGet(mudcsAddSystem(fsm[i]+".status"),fsm_status);
    dpGet(mudcsAddSystem(data[i]+".dynatemTcpIp"),coord);
    dpGet(mudcsAddSystem(data[i]+".dynatemTcpIp:_online.._stime"),last_set_status);
    last_set_status_string=last_set_status;

    if(strpos(coord,"PT100") < 0)fsm_status=-2;
    if((getCurrentTime()-last_set_status) > 60*60)fsm_status=-2; // 60*60 seconds

   service_data += type+";";
   service_data += "PT100;";

    if(fsm_status==-2)service_data += "status="+"ERROR: NO COMMUNICATION TO CONTROL SERVER"+";\n";
    else if(fsm_status==-1)service_data += "status="+"ALERT: THE DEVICE IS IN ALERT STATUS!!!"+";\n";
    else if(fsm_status==0)service_data += "status="+"THE DEVICE IS OFF"+";\n";
    else if(fsm_status>0)service_data += "status="+"THE DEVICE IS ON"+";\n";
    else service_data += "status="+"UNKNOWN: "+fsm_status+";\n";

    if(fsm_status==-2)last_set_status_string="NOT APPLICABLE";
    service_data += "time_stamp="+last_set_status_string+";\n";

//    dpGet(mudcsAddSystem(data[i]+".data.status"),value);
//    service_data += "state="+value+";\n";
//---

 
  } // for ober fsm-s
   dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),service_data); // service update !!!   
 } //else
//--------------------------------------------------
}

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

emudcs_gas_rc_read(dyn_string test_dyn){

int i,ii,j,jj;
////string prefix="GasMonitoring/CSC";
string suffix="GAS_SX5";
string type="GAS";
//////////int items_number=5;

dyn_string data;
dyn_string fsm;
string service_data;
float value;
string s_value;
int pos;
int fsm_status;
string coord;

time last_set_status;
string last_set_status_string;

dyn_dyn_string temp_items;

temp_items[1]=makeDynString("Mixer1_o.ar","Mixer1_o.co2");
temp_items[2]=makeDynString("Mixer2_o.ar","Mixer2_o.co2","Mixer2_o.cf4");

 
 dynAppend(data,CSC_fwG_g_SYSTEM_NAME+":"+"GAS_SX5_DimBroker_o");
 dynAppend(fsm,CSC_fwG_g_SYSTEM_NAME+":"+"GasMonitoring/GAS_MIXERS");

//--------------------------------------------------
 if(test_dyn[1]==type+"PREREAD"){
  for(i=1;i<=dynlen(fsm);i++){
    if(strpos(data[i],"Broker")>=0)continue;   
   mudcsCommandCscLevel(fsm[i]+".status", "get_data","GAS_SX5");
  }
  dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),"CONFIRMATION"); // service update !!!
 }
//--------------------------------------------------
 else{
  for(i=1;i<=dynlen(fsm);i++){  
///// !!! comment for gas only !!! if(strpos(data[i],"Broker")>=0)continue; 
    dpGet(mudcsAddSystem(fsm[i]+".status"),fsm_status);
    dpGet(mudcsAddSystem(data[i]+".dynatemTcpIp"),coord);
    dpGet(mudcsAddSystem(data[i]+".dynatemTcpIp:_online.._stime"),last_set_status);
    last_set_status_string=last_set_status;

    if(strpos(coord,"LVCB") < 0)fsm_status=-2;
    if((getCurrentTime()-last_set_status) > 60*60)fsm_status=-2; // 60*60 seconds

   service_data += type+";";
   service_data += "GAS_SX5;";

    if(fsm_status==-2)service_data += "status="+"ERROR: NO COMMUNICATION TO CONTROL SERVER"+";\n";
    else if(fsm_status==-1)service_data += "status="+"ALERT: THE DEVICE IS IN ALERT STATUS!!!"+";\n";
    else if(fsm_status==0)service_data += "status="+"THE DEVICE IS OFF"+";\n";
    else if(fsm_status>0)service_data += "status="+"THE DEVICE IS ON"+";\n";
    else service_data += "status="+"UNKNOWN: "+fsm_status+";\n";

    if(fsm_status==-2)last_set_status_string="NOT APPLICABLE";
    service_data += "time_stamp="+last_set_status_string+";\n";

////    dpGet(mudcsAddSystem(data[i]+".data.status"),value);
////    service_data += "state="+value+";\n";
//---
  for(ii=1;ii<=dynlen(temp_items);ii++){
   for(jj=1;jj<=dynlen(temp_items[ii]);jj++){ 
    dpGet(mudcsAddSystem(data[i]+".data."+temp_items[ii][jj]),value); 
     s_value=value;
     pos=strpos(s_value,".");
     if(pos >=0 && (strlen(s_value)-1-pos)>3){
      s_value=substr(s_value,0,pos+4);
     }
     strreplace(temp_items[ii][jj],"_o.","_");
     service_data += temp_items[ii][jj]+"="+s_value+" ;";
   } // for over jj
    service_data += "\n";
  } // for ii

  
  } // for ober fsm-s
   dpSetWait(mudcsAddSystem(CSC_fwG_g_SYSTEM_NAME+":"+"RC2DCS_o.command"),service_data); // service update !!!  
 } //else
//--------------------------------------------------
}




//===================================================================================
//===================================================================================
get_primary_data(){

string subcommand,set;
dyn_string coord_primary;
int i;

if(CSC_fwG_g_SYSTEM_NAME=="System1"){  // not used
 subcommand="HVPRDATA;1;1;255;0;0;-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),CSC_fwG_g_host+"|"+subcommand);
}
else if(CSC_fwG_g_SYSTEM_NAME==CSC_fwG_CSC_fwG_g_SYSTEM_NAME2){


 for(i=1;i<=dynlen(CSC_fwG_g_all_primary);i++){

  mudcsGetCoordForDp("HV_1",CSC_fwG_g_all_primary[i], set);
  coord_primary=strsplit(set,";");
  subcommand="HVPRDATA;"+coord_primary[2]+";"+coord_primary[3]+";255;0;0;-1";
  dpSetWait(mudcsAddSystem(mudcsAddSystem("HV_1_COM")+".command"),coord_primary[1]+"|"+subcommand);

 }

/*
 subcommand="HVPRDATA;1;1;255;0;0;-1";
 dpSetWait(mudcsAddSystem(mudcsAddSystem("HV_1_COM")+".command"),CSC_fwG_g_1host+"|"+subcommand);
 subcommand="HVPRDATA;1;2;255;0;0;-1";
 dpSetWait(mudcsAddSystem(mudcsAddSystem("HV_1_COM")+".command"),CSC_fwG_g_2host+"|"+subcommand);
*/

}

}

//========================================================

get_master_data(){

string subcommand,set;
dyn_string coord_master;
int i;

if(CSC_fwG_g_SYSTEM_NAME=="System1"){  // not used
 subcommand="HVDATA;1;1;255;0;0;-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),CSC_fwG_g_host+"|"+subcommand);
 subcommand="HVDATA;1;15;255;0;0;-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),CSC_fwG_g_host+"|"+subcommand);

}
else if(CSC_fwG_g_SYSTEM_NAME==CSC_fwG_CSC_fwG_g_SYSTEM_NAME2){
/*
 subcommand="HVDATA;4;0;255;0;0;-1";
 dpSetWait(mudcsAddSystem(mudcsAddSystem("HV_1_COM")+".command"),CSC_fwG_g_1host+"|"+subcommand);
 subcommand="HVDATA;4;1;255;0;0;-1";
 dpSetWait(mudcsAddSystem(mudcsAddSystem("HV_1_COM")+".command"),CSC_fwG_g_1host+"|"+subcommand);
 subcommand="HVDATA;4;2;255;0;0;-1";
 dpSetWait(mudcsAddSystem(mudcsAddSystem("HV_1_COM")+".command"),CSC_fwG_g_1host+"|"+subcommand);

 subcommand="HVDATA;3;2;255;0;0;-1";
 dpSetWait(mudcsAddSystem(mudcsAddSystem("HV_1_COM")+".command"),CSC_fwG_g_1host+"|"+subcommand);
 subcommand="HVDATA;3;3;255;0;0;-1";
 dpSetWait(mudcsAddSystem(mudcsAddSystem("HV_1_COM")+".command"),CSC_fwG_g_1host+"|"+subcommand);
 subcommand="HVDATA;3;4;255;0;0;-1";
 dpSetWait(mudcsAddSystem(mudcsAddSystem("HV_1_COM")+".command"),CSC_fwG_g_1host+"|"+subcommand);
*/




 for(i=1;i<=dynlen(CSC_fwG_g_all_masters);i++){
  mudcsGetCoordForDp("HV_1",CSC_fwG_g_all_masters[i], set);
  coord_master=strsplit(set,";");
  subcommand="HVDATA;"+coord_master[2]+";"+coord_master[3]+";255;0;0;-1";
  dpSetWait(mudcsAddSystem(mudcsAddSystem("HV_1_COM")+".command"),coord_master[1]+"|"+subcommand);
 }

////DebugTN("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM "+mudcsAddSystem(mudcsAddSystem("HV_1_COM")+".command"),coord_master[1]+"|"+subcommand);

}


}
//=========================================================

mudcs_master_check_voltage(string dpName){ // not used

int i;
int master_id;
int master_module_id;
float vset;
float vset_master;
string subcommand;
string host;
int master_chan;
int master_chan_p1;

string dp=substr(dpName,0,strpos(dpName,"."));
 dpGet(mudcsAddSystem(dp+".master_id"),master_id);

DebugTN(">>>>>>>>>>>>>>>>>>>>>>>>>>>>"+dpName);

dyn_string master_names=dpNames("master*","HV_1");
for(i=1;i<=dynlen(master_names);i++){
 dpGet(mudcsAddSystem(master_names[i]+".module_ID"),master_module_id);
 if(master_id == master_module_id){
   dpGet(mudcsAddSystem(dp+".master_chan"),master_chan);
   master_chan_p1=master_chan+1;
   dpGet(mudcsAddSystem(master_names[i]+".v"+master_chan_p1+".vset"),vset_master);
   dpGet(mudcsAddSystem(dp+".v1.vset"),vset);


  if(vset_master > vset && vset <3300){
    host=CSC_fwG_g_host;
    if(master_names[i] == CSC_fwG_g_SYSTEM_NAME+":"+"master500_1_1"){
    subcommand="HVCMD;2;15;"+master_chan+";7;"+vset+";"+"-1";
    dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
    }
    else if(master_names[i] == CSC_fwG_g_SYSTEM_NAME+":"+"master500_1_15"){
    subcommand="HVCMD;2;14;"+master_chan+";7;"+vset+";"+"-1";
    dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
    }
  
  }

 break;
 }
}


}

//=========================================================


emudcs_hv_slice_test_on(int vset){


dyn_int master_1_channels;
dyn_int master_2_channels;

dyn_int card_1_channels;
dyn_int card_2_channels;

dyn_string slice_test_chambers;


int voltage_primary=4000;
int voltage_master_1=4000;
int voltage_master_2=4000;
int voltage_small=vset;
int voltage_big=vset;

master_1_channels = makeDynString("5","4");// should be by-1 less !!!!!
master_2_channels = makeDynString("4","0","3","1"); // should be by-1 less !!!!! 

card_1_channels = makeDynString("1","0");           //  card slot 18
card_2_channels = makeDynString("9","6","12","5");  // card slot 16

slice_test_chambers=makeDynString(
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c16HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c31HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c32HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c16HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c31HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c32HV_1"
);



int value;
string subcommand;
string host="100";
string im;
int i;

// === vset for PRIMARY

value=voltage_primary;
//////setValue("pr_vset","text",value);
//////dpSetWait(mudcsAddSystem("vset_primary_default."),value);
//////primary_box_command("pr_vset", 7);

subcommand="HVPRCMD;0;1;255;7"+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

////int vset_primary_default;
////getValue("pr_vset","text",vset_primary_default);


// === primary ON ======

/////////requested_primary_state=1;
/////////primary_status=0;

subcommand="HVPRCMD;0;1;255;1;0;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

delay(2);
//+++++++++++++++++++++++++++++++++++++++++++

// === vset for MASTER ===

for(i=1;i<=dynlen(master_1_channels);i++){
im=master_1_channels[i];

value=voltage_master_1;
subcommand="HVCMD;20;15;"+im+";"+7+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
dpSet(mudcsAddSystem("vset_master_100_20_15_default."),value);

}
for(i=1;i<=dynlen(master_2_channels);i++){
im=master_2_channels[i];

value=voltage_master_2;
subcommand="HVCMD;20;14;"+im+";"+7+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
dpSet(mudcsAddSystem("vset_master_100_20_15_default."),value);

}

// ==== rampup for MASTER ===
for(i=1;i<=dynlen(master_1_channels);i++){
im=master_1_channels[i];

value=15;
subcommand="HVCMD;20;15;"+im+";"+3+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

}
for(i=1;i<=dynlen(master_2_channels);i++){
im=master_2_channels[i];
value=15;
subcommand="HVCMD;20;14;"+im+";"+3+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
// ====== imax for MASTER ====

for(i=1;i<=dynlen(master_1_channels);i++){
im=master_1_channels[i];
value=1500;
subcommand="HVCMD;20;15;"+im+";"+6+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
for(i=1;i<=dynlen(master_2_channels);i++){
im=master_2_channels[i];
value=1500;
subcommand="HVCMD;20;14;"+im+";"+6+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
// ===== TrigDelay for MASTER =====

for(i=1;i<=dynlen(master_1_channels);i++){
im=master_1_channels[i];
value=100;
subcommand="HVCMD;20;15;"+im+";"+38+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
for(i=1;i<=dynlen(master_2_channels);i++){
im=master_2_channels[i];
value=100;
subcommand="HVCMD;20;14;"+im+";"+38+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
delay(2);
// ===== MASTER ON =====

subcommand="HVCMD;20;15;0;30;1;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

subcommand="HVCMD;20;14;0;30;1;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

for(i=1;i<=dynlen(master_1_channels);i++){
im=master_1_channels[i];
subcommand="HVCMD;20;15;"+im+";1;0;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
for(i=1;i<=dynlen(master_2_channels);i++){
im=master_2_channels[i];
subcommand="HVCMD;20;14;"+im+";1;0;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
delay(3);

//+++++++++++++++++++++++++++++++++++++++++++
// === vset for all ch ====

value=voltage_small;
for(i=1;i<=dynlen(card_1_channels);i++){ // !!!!! currently 5 modules !!!!!! 
 im=card_1_channels[i];
 subcommand="HVCMD;18;"+im+";255;7;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}

value=voltage_big;

for(i=1;i<=dynlen(card_2_channels);i++){ // !!!!! currently 9 modules !!!!!!
 im=card_2_channels[i];
 subcommand="HVCMD;16;"+im+";255;7;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
// === rampup for all ch===

value=15;
for(i=1;i<=dynlen(card_1_channels);i++){ // !!!!! currently 5 modules !!!!!! 
 im=card_1_channels[i];
 subcommand="HVCMD;18;"+im+";255;3;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
for(i=1;i<=dynlen(card_2_channels);i++){ // !!!!! currently 9 modules !!!!!!
 im=card_2_channels[i];
 subcommand="HVCMD;16;"+im+";255;3;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
// ==== imax for all ch ====

value=10;
for(i=1;i<=dynlen(card_1_channels);i++){ // !!!!! currently 5 modules !!!!!! 
 im=card_1_channels[i];
 subcommand="HVCMD;18;"+im+";255;6;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
for(i=1;i<=dynlen(card_2_channels);i++){ // !!!!! currently 9 modules !!!!!!
 im=card_2_channels[i];
 subcommand="HVCMD;16;"+im+";255;6;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
// ==== trigDelay for all ch ====

value=100;
for(i=1;i<=dynlen(card_1_channels);i++){ // !!!!! currently 5 modules !!!!!! 
 im=card_1_channels[i];
 subcommand="HVCMD;18;"+im+";255;38;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
for(i=1;i<=dynlen(card_2_channels);i++){ // !!!!! currently 9 modules !!!!!!
 im=card_2_channels[i];
 subcommand="HVCMD;16;"+im+";255;38;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}

delay(3);
// ====== CHAMBERS ON =======

int current_status;
for(i=1;i<=dynlen(slice_test_chambers);i++){
 dpGet(mudcsAddSystem(slice_test_chambers[i]+".status"),current_status);
 if(current_status != -2)
 dpSet(mudcsAddSystem(slice_test_chambers[i]+".status"),2);
}




}
//=========================================================
//=========================================================
//=========================================================


emudcs_hv_slice_test_off(int vset){



dyn_int master_1_channels;
dyn_int master_2_channels;

dyn_int card_1_channels;
dyn_int card_2_channels;

dyn_string slice_test_chambers;

int voltage_primary=0;
int voltage_master_1=0;
int voltage_master_2=0;
int voltage_small=vset;
int voltage_big=vset;

master_1_channels = makeDynString("5","4");// should be by-1 less !!!!!
master_2_channels = makeDynString("4","0","3","1"); // should be by-1 less !!!!! 

card_1_channels = makeDynString("1","0");           //  card slot 18
card_2_channels = makeDynString("9","6","12","5");  // card slot 16

slice_test_chambers=makeDynString(
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c16HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c31HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c32HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c16HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c31HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c32HV_1"
);


int value;
string subcommand;
string host="100";
string im;
int i;

// === vset for PRIMARY

////int vset_primary_default;
////getValue("pr_vset","text",vset_primary_default);

value=voltage_primary;
//////setValue("pr_vset","text",value);
//////dpSetWait(mudcsAddSystem("vset_primary_default."),value);
//////primary_box_command("pr_vset", 7);

subcommand="HVPRCMD;0;1;255;7"+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);


// === primary ON ======

/////////requested_primary_state=1;
/////////primary_status=0;

subcommand="HVPRCMD;0;1;255;1;0;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

delay(2);
//+++++++++++++++++++++++++++++++++++++++++++

// === vset for MASTER ===

for(i=1;i<=dynlen(master_1_channels);i++){
im=master_1_channels[i];

value=voltage_master_1;
subcommand="HVCMD;20;15;"+im+";"+7+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
dpSet(mudcsAddSystem("vset_master_100_20_15_default."),value);

}
for(i=1;i<=dynlen(master_2_channels);i++){
im=master_2_channels[i];

value=voltage_master_2;
subcommand="HVCMD;20;14;"+im+";"+7+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
dpSet(mudcsAddSystem("vset_master_100_20_15_default."),value);

}

// ==== rampup for MASTER ===
for(i=1;i<=dynlen(master_1_channels);i++){
im=master_1_channels[i];

value=15;
subcommand="HVCMD;20;15;"+im+";"+3+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

}
for(i=1;i<=dynlen(master_2_channels);i++){
im=master_2_channels[i];
value=15;
subcommand="HVCMD;20;14;"+im+";"+3+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
// ====== imax for MASTER ====

for(i=1;i<=dynlen(master_1_channels);i++){
im=master_1_channels[i];
value=1500;
subcommand="HVCMD;20;15;"+im+";"+6+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
for(i=1;i<=dynlen(master_2_channels);i++){
im=master_2_channels[i];
value=1500;
subcommand="HVCMD;20;14;"+im+";"+6+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
// ===== TrigDelay for MASTER =====

for(i=1;i<=dynlen(master_1_channels);i++){
im=master_1_channels[i];
value=100;
subcommand="HVCMD;20;15;"+im+";"+38+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
for(i=1;i<=dynlen(master_2_channels);i++){
im=master_2_channels[i];
value=100;
subcommand="HVCMD;20;14;"+im+";"+38+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
delay(2);
// ===== MASTER ON =====

subcommand="HVCMD;20;15;0;30;1;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

subcommand="HVCMD;20;14;0;30;1;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);

for(i=1;i<=dynlen(master_1_channels);i++){
im=master_1_channels[i];
subcommand="HVCMD;20;15;"+im+";0;0;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
for(i=1;i<=dynlen(master_2_channels);i++){
im=master_2_channels[i];
subcommand="HVCMD;20;14;"+im+";0;0;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
delay(3);


//+++++++++++++++++++++++++++++++++++++++++++
// === vset for all ch ====

value=voltage_small;
for(i=1;i<=dynlen(card_1_channels);i++){ // !!!!! currently 5 modules !!!!!! 
 im=card_1_channels[i];
 subcommand="HVCMD;18;"+im+";255;7;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}

value=voltage_big;

for(i=1;i<=dynlen(card_2_channels);i++){ // !!!!! currently 9 modules !!!!!!
 im=card_2_channels[i];
 subcommand="HVCMD;16;"+im+";255;7;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
// === rampup for all ch===

value=15;
for(i=1;i<=dynlen(card_1_channels);i++){ // !!!!! currently 5 modules !!!!!! 
 im=card_1_channels[i];
 subcommand="HVCMD;18;"+im+";255;3;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
for(i=1;i<=dynlen(card_2_channels);i++){ // !!!!! currently 9 modules !!!!!!
 im=card_2_channels[i];
 subcommand="HVCMD;16;"+im+";255;3;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
// ==== imax for all ch ====

value=10;
for(i=1;i<=dynlen(card_1_channels);i++){ // !!!!! currently 5 modules !!!!!! 
 im=card_1_channels[i];
 subcommand="HVCMD;18;"+im+";255;6;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
for(i=1;i<=dynlen(card_2_channels);i++){ // !!!!! currently 9 modules !!!!!!
 im=card_2_channels[i];
 subcommand="HVCMD;16;"+im+";255;6;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
// ==== trigDelay for all ch ====

value=100;
for(i=1;i<=dynlen(card_1_channels);i++){ // !!!!! currently 5 modules !!!!!! 
 im=card_1_channels[i];
 subcommand="HVCMD;18;"+im+";255;38;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}
for(i=1;i<=dynlen(card_2_channels);i++){ // !!!!! currently 9 modules !!!!!!
 im=card_2_channels[i];
 subcommand="HVCMD;16;"+im+";255;38;"+value+";-1";
 dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host+"|"+subcommand);
}

delay(3);
// ====== CHAMBERS ON =======

int current_status;
for(i=1;i<=dynlen(slice_test_chambers);i++){
 dpGet(mudcsAddSystem(slice_test_chambers[i]+".status"),current_status);
 if(current_status != -2) 
 dpSet(mudcsAddSystem(slice_test_chambers[i]+".status"),0);
}


}


//=========================================================
//=========================================================


emudcs_hv_slice_test_on_new(int vset){


dyn_int masterss; //// &&&&&&&&&&&&&&&&&&&&
int im_int; //// &&&&&&&&&&&&&&&&&&&&

string all_ch;

string master, chamber;
string set;
dyn_string coord;
dyn_string master_sets;
dyn_string chamber_sets;

dyn_dyn_int master_channels;
dyn_dyn_int card_channels;

dyn_string slice_test_chambers;
dyn_string slice_test_chambers_data;

int voltage_primary=150;//3800;
dyn_int voltage_master=makeDynInt(4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000);

dyn_int voltage_chambers=makeDynInt(vset,vset); // for small and big chambers (different cards)

if(CSC_fwG_g_SYSTEM_NAME=="System1"){

//master_channels[1] = makeDynString("5","4");// should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[1]
//master_channels[2] = makeDynString("4","0","3","1"); // should be by-1 less !!!!!  // for CSC_fwG_g_all_masters[2]

////card_channels[1] = makeDynString("1","0");           //  card slot 18
////card_channels[2] = makeDynString("9","6","12","5");  // card slot 16

/*
slice_test_chambers=makeDynString(
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c16HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c31HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c32HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c16HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c31HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c32HV_1"
);
*/

}
else if(CSC_fwG_g_SYSTEM_NAME==CSC_fwG_CSC_fwG_g_SYSTEM_NAME2){

//master_channels[1] = makeDynString("5","4");// should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[1]
//master_channels[2] = makeDynString("4","0","3","1"); // should be by-1 less !!!!!  // for CSC_fwG_g_all_masters[2]

/*
slice_test_chambers=makeDynString(
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c16HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c31HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c32HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c16HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c31HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c32HV_1"
);
*/

}

///mudcsDebug(">>"+CSC_fwG_g_SLICE_DPOINTS[1]);
slice_test_chambers_data=mudcsNamesByAliasesGet(CSC_fwG_g_SLICE_DPOINTS);
////mudcsDebug(">>>"+slice_test_chambers_data[1]);
mudcsConvertData2Fsm("HV_1", /*CSC_fwG_g_SLICE_DPOINTS*/slice_test_chambers_data, slice_test_chambers);
////mudcsDebug(">>>>"+slice_test_chambers[1]);
mudcsConvertFsm2Data(slice_test_chambers, slice_test_chambers_data);



int value;
string subcommand;

string host1=CSC_fwG_g_all_primary[1];//CSC_fwG_g_1host;
string host2=CSC_fwG_g_all_primary[2];//CSC_fwG_g_2host;

string im;
int i, j;


//---
int master_id, master_chan, master_index;
dyn_string coord_master;
 for(i=1;i<=dynlen(slice_test_chambers);i++){
  mudcsGetMasterForChamber(slice_test_chambers[i], master_id, master_chan, master_index, coord_master);
  if(master_index > 0 ){
   dynAppend(master_channels[master_index],master_chan);
   dynAppend(masterss,master_index); //// &&&&&&&&&&&&&&&&&&&&
  }
 }
//---



// === vset for PRIMARY

if(dynlen(master_channels)>0){
value=voltage_primary;

//////subcommand="HVPRCMD;1;1;255;7"+";"+value+";"+"-1";
//////dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host1+"|"+subcommand);
subcommand="HVPRCMD;1;2;255;7"+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host2+"|"+subcommand);

// === primary ON ======


string dp_primary;

////dp_primary=CSC_fwG_g_SYSTEM_NAME+":HighVoltage/CSCHV_PR_"+CSC_fwG_g_all_primary[1];
////dp_primary=mudcsNameByAliasGet(dp_primary);
////dpSet(mudcsAddSystem(dp_primary+".last_vset"),value);
/////dpSet(mudcsAddSystem(dp_primary+".status"),2); // primary ON

dp_primary=CSC_fwG_g_SYSTEM_NAME+":HighVoltage/CSCHV_PR_"+CSC_fwG_g_all_primary[2];
dp_primary=mudcsNameByAliasGet(dp_primary);
dpSet(mudcsAddSystem(dp_primary+".last_vset"),value);
dpSet(mudcsAddSystem(dp_primary+".status"),2); // primary ON

delay(2);

}
//+++++++++++++++++++++++++++++++++++++++++++

// === vset for MASTER ===

for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue; //// &&&&&&&&&&&&&&&&&&&& 

  master=CSC_fwG_g_all_masters[j];
  mudcsGetCoordForDp("HV_1", master, set);
  //// dynAppend(master_sets,set);
  master_sets[j]=set; //// &&&&&&&&&&&&&&&&&&&&

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=voltage_master[j];
  coord=strsplit(set,";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+7+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
  dpSet(mudcsAddSystem("vset_master_100_20_15_default."),value);
 }
} // for j 

// ==== rampup for MASTER ===

for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue; //// &&&&&&&&&&&&&&&&&&&& 

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=15;
////  mudcsDebug(master_sets[j]);
  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+3+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

// ====== imax for MASTER ====
for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue; //// &&&&&&&&&&&&&&&&&&&& 

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=1500;
  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+6+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

// ===== TrigDelay for MASTER =====
for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue; //// &&&&&&&&&&&&&&&&&&&& 

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=100;
  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+38+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

delay(2);
// ===== MASTER ON =====
/*
for(j=1;j<=dynlen(master_channels);j++){

  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";0;30;1;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 
} // for j 

for(j=1;j<=dynlen(master_channels);j++){
 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";1;0;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

delay(3);
*/

//==============================================================================================================
//==============================================================================================================
// ===== MASTER ON ===== TEMPORAL TO SET 50 VOLTS ON ALL CHANNELS !!!!!!!!!!!!!!!! =============================
//==============================================================================================================
//==============================================================================================================

if(CSC_fwG_g_IS_TEMPORAL_50V){

for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue;

  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";0;30;1;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 
} // for j 

//mudcsDebug("number "+dynlen(master_channels));
for(j=1;j<=dynlen(master_channels);j++){

//mudcsDebug("before "+j);
  if(!dynContains(masterss,j))continue;
//mudcsDebug("++++ "+j);

 for(i=1;i<=8;i++){
  im_int=i-1;
  im=im_int;

   if(dynContains(master_channels[j],im_int))continue;

  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";1;0;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";7;50;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

 }
} // for j 

delay(3);

}

//==============================================================================================================
//==============================================================================================================
//========================== end of temporal piece =============================================================
//==============================================================================================================

//+++++++++++++++++++++++++++++++++++++++++++
// === vset for all ch ====


for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=voltage_chambers[1];

  mudcsGetCoordForDp("HV_1", slice_test_chambers[j], set);

//mudcsDebug2(slice_test_chambers[j]+">>"+set);

  dynAppend(chamber_sets,set);
  coord=strsplit(set,";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";7;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
  dpSetWait(mudcsAddSystem(slice_test_chambers[j]+".last_vset"),value); // ********** switch ON only ******************

} // for j

// === rampup for all ch===

for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=15;
  coord=strsplit(chamber_sets[j],";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";3;"+value+";"+"-1";
//mudcsDebug(subcommand);
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

} // for j

// ==== imax for all ch ====

for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=1;
  coord=strsplit(chamber_sets[j],";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";6;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

} // for j

// ==== trigDelay for all ch ====

for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=500;
  coord=strsplit(chamber_sets[j],";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";38;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

} // for j

delay(3);
// ====== CHAMBERS ON =======

int current_status;
for(i=1;i<=dynlen(slice_test_chambers);i++){
 dpGet(mudcsAddSystem(slice_test_chambers[i]+".status"),current_status);
 if(current_status != -2)
 dpSet(mudcsAddSystem(slice_test_chambers[i]+".status"),2);
}




}
//=========================================================
//=========================================================
//=========================================================
//=========================================================
//=========================================================


emudcs_hv_slice_test_off_new(int vset){


dyn_int masterss; //// &&&&&&&&&&&&&&&&&&&&
int im_int; //// &&&&&&&&&&&&&&&&&&&&

string all_ch;

string master, chamber;
string set;
dyn_string coord;
dyn_string master_sets;
dyn_string chamber_sets;

dyn_dyn_int master_channels;
dyn_dyn_int card_channels;

dyn_string slice_test_chambers;
dyn_string slice_test_chambers_data;

int voltage_primary=0;
dyn_int voltage_master=makeDynInt(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

dyn_int voltage_chambers=makeDynInt(vset,vset); // for small and big chambers (different cards)

if(CSC_fwG_g_SYSTEM_NAME=="System1"){

//master_channels[1] = makeDynString("5","4");// should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[1]
//master_channels[2] = makeDynString("4","0","3","1"); // should be by-1 less !!!!!  // for CSC_fwG_g_all_masters[2]

////card_channels[1] = makeDynString("1","0");           //  card slot 18
////card_channels[2] = makeDynString("9","6","12","5");  // card slot 16

/*
slice_test_chambers=makeDynString(
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c16HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c31HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c32HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c16HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c31HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c32HV_1"
);
*/



}
else if(CSC_fwG_g_SYSTEM_NAME==CSC_fwG_CSC_fwG_g_SYSTEM_NAME2){

//master_channels[1] = makeDynString("5","4");// should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[1]
//master_channels[2] = makeDynString("4","0","3","1"); // should be by-1 less !!!!!  // for CSC_fwG_g_all_masters[2]

/*
slice_test_chambers=makeDynString(
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c16HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c31HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c32HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c16HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c31HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c32HV_1"
);
*/


}

slice_test_chambers_data=mudcsNamesByAliasesGet(CSC_fwG_g_SLICE_DPOINTS);
mudcsConvertData2Fsm("HV_1", /*CSC_fwG_g_SLICE_DPOINTS*/slice_test_chambers_data, slice_test_chambers);
mudcsConvertFsm2Data(slice_test_chambers, slice_test_chambers_data);

int value;
string subcommand;
string host1=CSC_fwG_g_all_primary[1];//CSC_fwG_g_1host;
string host2=CSC_fwG_g_all_primary[2];//CSC_fwG_g_2host;
string im;
int i, j;


//---
int master_id, master_chan, master_index;
dyn_string coord_master;
 for(i=1;i<=dynlen(slice_test_chambers);i++){
  mudcsGetMasterForChamber(slice_test_chambers[i], master_id, master_chan, master_index, coord_master);
  if(master_index > 0 ){
//-----
  int index1, index2;
  string dp2;
  int status2;
  int found=false;
  for(j=1;j<=dynlen(CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST);j++){
   if(dynlen(CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[j])<2)continue;
   index1=dynContains(CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[j],slice_test_chambers[i]);
   if(index1 < 1)continue;
   else if(index1==1)index2=2;
   else if(index1==2)index2=1;
   if(dynlen(CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[j]) < 2)break; // i.e. found will be false
   dp2=CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[j][index2];
   if(dynContains(slice_test_chambers,dp2))break; // i.e. found will be false
   dpGet(mudcsAddSystem(dp2+".status"),status2);
   if(status2 == 0){
     dynAppend(master_channels[master_index],master_chan);   
     dynAppend(masterss,master_index); //// &&&&&&&&&&&&&&&&&&&& 
   }
   else {
       mudcsDebug("master channel remains ON \n as another chamber is connected");
   ////    mudcsDebug(dp2);
   }
   found=true;
   break;
  } //for

  if(!found){ // i.e. 30 channel board or there is one chamber connected to 36 ch board 
       dynAppend(master_channels[master_index],master_chan);
       dynAppend(masterss,master_index); //// &&&&&&&&&&&&&&&&&&&&
  }
//-----
//   dynAppend(master_channels[master_index],master_chan);
  }
 }
//---


// === vset for PRIMARY

if(dynlen(master_channels)>0){

value=voltage_primary;
/*
subcommand="HVPRCMD;0;1;255;7"+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host1+"|"+subcommand);
subcommand="HVPRCMD;0;1;255;7"+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host2+"|"+subcommand);


// === primary ON ======

subcommand="HVPRCMD;0;1;255;1;0;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host1+"|"+subcommand);
subcommand="HVPRCMD;0;1;255;1;0;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host2+"|"+subcommand);
*/

delay(2);
}

//+++++++++++++++++++++++++++++++++++++++++++

// === vset for MASTER ===

for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue; //// &&&&&&&&&&&&&&&&&&&& 

  master=CSC_fwG_g_all_masters[j];
  mudcsGetCoordForDp("HV_1", master, set);
  ///// dynAppend(master_sets,set);
  master_sets[j]=set; //// &&&&&&&&&&&&&&&&&&&&

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=voltage_master[j];
  coord=strsplit(set,";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+7+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
  dpSet(mudcsAddSystem("vset_master_100_20_15_default."),value);
 }
} // for j 

// ==== rampup for MASTER ===

for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue; //// &&&&&&&&&&&&&&&&&&&& 

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=15;
///  mudcsDebug(master_sets[j]);
  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+3+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

// ====== imax for MASTER ====
for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue; //// &&&&&&&&&&&&&&&&&&&& 

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=1500;
  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+6+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

// ===== TrigDelay for MASTER =====
for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue; //// &&&&&&&&&&&&&&&&&&&& 

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=100;
  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+38+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

delay(2);
// ===== MASTER ON =====
/*
for(j=1;j<=dynlen(master_channels);j++){

  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";0;30;1;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 
} // for j 

for(j=1;j<=dynlen(master_channels);j++){
 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";0;0;-1";
/////mudcsDebug(subcommand);
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

delay(3);
*/
//+++++++++++++++++++++++++++++++++++++++++++
// === vset for all ch ====


for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=voltage_chambers[1];

  mudcsGetCoordForDp("HV_1", slice_test_chambers[j], set);
  dynAppend(chamber_sets,set);
  coord=strsplit(set,";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";7;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

} // for j

// === rampup for all ch===

for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=15;
  coord=strsplit(chamber_sets[j],";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";3;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

} // for j

// ==== imax for all ch ====

for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=10;
  coord=strsplit(chamber_sets[j],";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";6;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

} // for j

// ==== trigDelay for all ch ====

for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=500;
  coord=strsplit(chamber_sets[j],";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";38;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

} // for j

delay(3);
// ====== CHAMBERS ON =======

int current_status;
for(i=1;i<=dynlen(slice_test_chambers);i++){
 dpGet(mudcsAddSystem(slice_test_chambers[i]+".status"),current_status);
 if(current_status != -2)
 dpSet(mudcsAddSystem(slice_test_chambers[i]+".status"),0);
}




}
//=========================================================
//=========================================================
//=========================================================


//=========================================================


emudcs_hv_long_term_on_new(int vset){


dyn_int masterss; //// &&&&&&&&&&&&&&&&&&&&
int im_int; //// &&&&&&&&&&&&&&&&&&&&

string all_ch;

string master, chamber;
string set;
dyn_string coord;
dyn_string master_sets;
dyn_string chamber_sets;

dyn_dyn_int master_channels;
dyn_dyn_int card_channels;

dyn_string slice_test_chambers;
dyn_string slice_test_chambers_data;

int voltage_primary=3800;

dyn_int voltage_master=makeDynInt(4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000);

dyn_int voltage_chambers=makeDynInt(vset,vset); // for small and big chambers (different cards)

if(CSC_fwG_g_SYSTEM_NAME=="System1"){

//master_channels[1] = makeDynString("1","2","3");// should be by-1 less !!!!! // for CSC_fwG_g_all_masters[1]
//master_channels[2] = makeDynString("5","6"); // should be by-1 less !!!!! // for CSC_fwG_g_all_masters[2]

/*
slice_test_chambers=makeDynString(
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c11HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c12HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c14HV_1",

CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c11HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c12HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c13HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c14HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c15HV_1",


CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c30HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c30HV_1");
*/



}
else if(CSC_fwG_g_SYSTEM_NAME==CSC_fwG_CSC_fwG_g_SYSTEM_NAME2){

//master_channels[1] = makeDynString("1","2","3");// should be by-1 less !!!!! // for CSC_fwG_g_all_masters[1]
//master_channels[2] = makeDynString("5","6"); // should be by-1 less !!!!! // for CSC_fwG_g_all_masters[2]

// order as in the CSC_fwG_g_all_masters
//ME+2/2/05 ME+2/2/08 ( ME+3/1/04 ME+3/1/06 )
//master_channels[3] = makeDynString("1","4","6");//2 5 7  should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[1] // id=6
//ME+2/2/01 // ME+2/2/03 ME+2/2/12  ME+2/2/10  (ME+2/1/06 ME+2/1/07)
//master_channels[4] = makeDynString("6","1","2","5","4");// 7 2 3 6 5     should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[2] // id=14
// ME+2/2/35 ME+2/2/33 ME+2/2/06
//master_channels[5] = makeDynString("5","3","1");//6 4 2    should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[3] // id=11
// ME+2/2/02 ME+2/2/36 ME+2/2/07 ME+2/2/09 (ME+2/1/17 ME+2/1/01) (ME+3/1/18 ME+3/1/02)
//master_channels[6] = makeDynString("6","1","4","5","2","7");//7 2 5 6 3 8  should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[4] // id=17
// ME+2/2/04 ME+2/2/13 (ME+2/1/03 ME+2/1/02) (ME+3/1/07 ME+2/1/18)
//master_channels[7] = makeDynString("6","7","2","5");//7 8 3 6   should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[5] // id=7
// ME+2/2/34 ME+2/2/11 (ME+3/1/01 ME+3/1/17)
//master_channels[8] = makeDynString("5","2","7");//6 3 8  should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[6] // id=16

// 

/*
slice_test_chambers=makeDynString(
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c11HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c12HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c14HV_1",

CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c11HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c12HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c13HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c14HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c15HV_1",

CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c30HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c30HV_1",
//-----
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c01HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c02HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c03HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c06HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c07HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c17HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c18HV_1",

CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c01HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c02HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c03HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c04HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c05HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c06HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c07HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c08HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c09HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c10HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c11HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c12HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c13HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c33HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c34HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c35HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c36HV_1",


CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c01HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c02HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c07HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c04HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c06HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c17HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c18HV_1");
*/



}

slice_test_chambers_data=mudcsNamesByAliasesGet(CSC_fwG_g_HV_LONG_TERM_DPOINTS);
mudcsConvertData2Fsm("HV_1", /*CSC_fwG_g_HV_LONG_TERM_DPOINTS*/slice_test_chambers_data, slice_test_chambers);
mudcsConvertFsm2Data(slice_test_chambers, slice_test_chambers_data);

int value;
string subcommand;
string host1=CSC_fwG_g_all_primary[1];//CSC_fwG_g_1host;
string host2=CSC_fwG_g_all_primary[2];//CSC_fwG_g_2host;
string im;
int i, j;

//---
int master_id, master_chan, master_index;
dyn_string coord_master;
 for(i=1;i<=dynlen(slice_test_chambers);i++){
  mudcsGetMasterForChamber(slice_test_chambers[i], master_id, master_chan, master_index, coord_master);
  if(master_index > 0 ){
   dynAppend(master_channels[master_index],master_chan);
   dynAppend(masterss,master_index); //// &&&&&&&&&&&&&&&&&&&&
  }
 }
//---

// === vset for PRIMARY

////mudcsDebug(dynlen(master_channels));

if(dynlen(master_channels)>0){

value=voltage_primary;
/////mudcsDebug(dynlen(master_channels)+".......");

subcommand="HVPRCMD;0;1;255;7"+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host1+"|"+subcommand);
subcommand="HVPRCMD;0;1;255;7"+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host2+"|"+subcommand);


string dp_primary;
dp_primary=CSC_fwG_g_SYSTEM_NAME+":HighVoltage/CSCHV_PR_"+CSC_fwG_g_all_primary[1];
dp_primary=mudcsNameByAliasGet(dp_primary);
dpSet(mudcsAddSystem(dp_primary+".last_vset"),value);
dpSet(mudcsAddSystem(dp_primary+".status"),2); // primary ON
dp_primary=CSC_fwG_g_SYSTEM_NAME+":HighVoltage/CSCHV_PR_"+CSC_fwG_g_all_primary[2];
dp_primary=mudcsNameByAliasGet(dp_primary);
dpSet(mudcsAddSystem(dp_primary+".last_vset"),value);
dpSet(mudcsAddSystem(dp_primary+".status"),2); // primary ON

// === primary ON ======
/*
subcommand="HVPRCMD;0;1;255;1;0;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host1+"|"+subcommand);
subcommand="HVPRCMD;0;1;255;1;0;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host2+"|"+subcommand);
*/

delay(2);

}
//+++++++++++++++++++++++++++++++++++++++++++

// === vset for MASTER ===

for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue; //// &&&&&&&&&&&&&&&&&&&& 

  master=CSC_fwG_g_all_masters[j];
  mudcsGetCoordForDp("HV_1", master, set);

  //// &&&&&&&&&&&&&&&&&&&& dynAppend(master_sets,set);
  master_sets[j]=set; //// &&&&&&&&&&&&&&&&&&&&

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=voltage_master[j];
  coord=strsplit(set,";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+7+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
  dpSet(mudcsAddSystem("vset_master_100_20_15_default."),value);
 }
} // for j 

// ==== rampup for MASTER ===

for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue;

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=15;
////  mudcsDebug(master_sets[j]);
  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+3+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

// ====== imax for MASTER ====

for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue;

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=1500;
  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+6+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

// ===== TrigDelay for MASTER =====
for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue;

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=100;
  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+38+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

delay(2);
// ===== MASTER ON =====
/*
for(j=1;j<=dynlen(master_channels);j++){

  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";0;30;1;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 
} // for j 

for(j=1;j<=dynlen(master_channels);j++){
 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";1;0;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

delay(3);
*/

//==============================================================================================================
//==============================================================================================================
// ===== MASTER ON ===== TEMPORAL TO SET 50 VOLTS ON ALL CHANNELS !!!!!!!!!!!!!!!! =============================
//==============================================================================================================
//==============================================================================================================

if(CSC_fwG_g_IS_TEMPORAL_50V){

for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue;

  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";0;30;1;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 
} // for j 

//mudcsDebug("number "+dynlen(master_channels));
for(j=1;j<=dynlen(master_channels);j++){

//mudcsDebug("before "+j);
  if(!dynContains(masterss,j))continue;
//mudcsDebug("++++ "+j);

 for(i=1;i<=8;i++){
  im_int=i-1;
  im=im_int;

   if(dynContains(master_channels[j],im_int))continue;

  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";1;0;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";7;50;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

 }
} // for j 

delay(3);

}

//==============================================================================================================
//==============================================================================================================
//========================== end of temporal piece =============================================================
//==============================================================================================================

//+++++++++++++++++++++++++++++++++++++++++++
// === vset for all ch ====


for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=voltage_chambers[1];

  mudcsGetCoordForDp("HV_1", slice_test_chambers[j], set);
//  DebugTN(slice_test_chambers[j]+"____"+set);
  dynAppend(chamber_sets,set);
  coord=strsplit(set,";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";7;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
  dpSetWait(mudcsAddSystem(slice_test_chambers[j]+".last_vset"),value); // ********** switch ON only ******************

} // for j

// === rampup for all ch===

for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=15;
  coord=strsplit(chamber_sets[j],";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";3;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

} // for j

// ==== imax for all ch ====

for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=10;
  coord=strsplit(chamber_sets[j],";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";6;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

} // for j

// ==== trigDelay for all ch ====

for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=500;
  coord=strsplit(chamber_sets[j],";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";38;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

} // for j

delay(3);
// ====== CHAMBERS ON =======

int current_status;
for(i=1;i<=dynlen(slice_test_chambers);i++){
 dpGet(mudcsAddSystem(slice_test_chambers[i]+".status"),current_status);
 if(current_status != -2)
 dpSet(mudcsAddSystem(slice_test_chambers[i]+".status"),2);
}




}
//=========================================================
//=========================================================
//=========================================================
//=========================================================
//=========================================================


emudcs_hv_long_term_off_new(int vset){

dyn_int masterss; //// &&&&&&&&&&&&&&&&&&&&
int im_int; //// &&&&&&&&&&&&&&&&&&&&

string all_ch;

string master, chamber;
string set;
dyn_string coord;
dyn_string master_sets;
dyn_string chamber_sets;

dyn_dyn_int master_channels;
dyn_dyn_int card_channels;

dyn_string slice_test_chambers;
dyn_string slice_test_chambers_data;

int voltage_primary=0;

dyn_int voltage_master=makeDynInt(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

dyn_int voltage_chambers=makeDynInt(vset,vset); // for small and big chambers (different cards)

if(CSC_fwG_g_SYSTEM_NAME=="System1"){

//master_channels[1] = makeDynString("1","2","3");// should be by-1 less !!!!! // for CSC_fwG_g_all_masters[1]
//master_channels[2] = makeDynString("5","6"); // should be by-1 less !!!!! // for CSC_fwG_g_all_masters[2]

/*
slice_test_chambers=makeDynString(
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c11HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c12HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c14HV_1",

CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c11HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c12HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c13HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c14HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c15HV_1",


CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c30HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c30HV_1");
*/




}
else if(CSC_fwG_g_SYSTEM_NAME==CSC_fwG_CSC_fwG_g_SYSTEM_NAME2){

//master_channels[1] = makeDynString("1","2","3");// should be by-1 less !!!!! // for CSC_fwG_g_all_masters[1]
//master_channels[2] = makeDynString("5","6"); // should be by-1 less !!!!! // for CSC_fwG_g_all_masters[2]

// order as in the CSC_fwG_g_all_masters
//ME+2/2/05 ME+2/2/08 ( ME+3/1/04 ME+3/1/06 )
//master_channels[3] = makeDynString("1","4","6");//2 5 7  should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[1] // id=6
//ME+2/2/01 // ME+2/2/03 ME+2/2/12  ME+2/2/10  (ME+2/1/06 ME+2/1/07)
//master_channels[4] = makeDynString("6","1","2","5","4");// 7 2 3 6 5     should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[2] // id=14
// ME+2/2/35 ME+2/2/33 ME+2/2/06
//master_channels[5] = makeDynString("5","3","1");//6 4 2    should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[3] // id=11
// ME+2/2/02 ME+2/2/36 ME+2/2/07 ME+2/2/09 (ME+2/1/17 ME+2/1/01) (ME+3/1/18 ME+3/1/02)
//master_channels[6] = makeDynString("6","1","4","5","2","7");//7 2 5 6 3 8  should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[4] // id=17
// ME+2/2/04 ME+2/2/13 (ME+2/1/03 ME+2/1/02) (ME+3/1/07 ME+2/1/18)
//master_channels[7] = makeDynString("6","7","2","5");//7 8 3 6   should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[5] // id=7
// ME+2/2/34 ME+2/2/11 (ME+3/1/01 ME+3/1/17)
//master_channels[8] = makeDynString("5","2","7");//6 3 8  should be by-1 less !!!!!   // for CSC_fwG_g_all_masters[6] // id=16

// 

/*
slice_test_chambers=makeDynString(
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c11HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c12HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c14HV_1",

CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c11HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c12HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c13HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c14HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c15HV_1",

CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c30HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r2c30HV_1",
//---------------------------------------------
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c01HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c02HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c03HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c06HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c07HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c17HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r1c18HV_1",

CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c01HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c02HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c03HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c04HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c05HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c06HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c07HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c08HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c09HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c10HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c11HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c12HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c13HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c33HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c34HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c35HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp2r2c36HV_1",


CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c01HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c02HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c07HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c04HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c06HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c17HV_1",
CSC_fwG_g_SYSTEM_NAME+":"+"HighVoltage/CSCdp3r1c18HV_1");
*/



}

slice_test_chambers_data=mudcsNamesByAliasesGet(CSC_fwG_g_HV_LONG_TERM_DPOINTS);
mudcsConvertData2Fsm("HV_1", /*CSC_fwG_g_HV_LONG_TERM_DPOINTS*/slice_test_chambers_data, slice_test_chambers);
mudcsConvertFsm2Data(slice_test_chambers, slice_test_chambers_data);

int value;
string subcommand;
string host1=CSC_fwG_g_all_primary[1];//CSC_fwG_g_1host;
string host2=CSC_fwG_g_all_primary[2];//CSC_fwG_g_2host;
string im;
int i, j;


//---
int master_id, master_chan, master_index;
dyn_string coord_master;
 for(i=1;i<=dynlen(slice_test_chambers);i++){
  mudcsGetMasterForChamber(slice_test_chambers[i], master_id, master_chan, master_index, coord_master);
  if(master_index > 0 ){
//-----
  int index1, index2;
  string dp2;
  int status2;
  int found=false;
  for(j=1;j<=dynlen(CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST);j++){
   if(dynlen(CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[j])<2)continue;
   index1=dynContains(CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[j],slice_test_chambers[i]);
   if(index1 < 1)continue;
   else if(index1==1)index2=2;
   else if(index1==2)index2=1;
   if(dynlen(CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[j]) < 2)break; // i.e. found will be false
   dp2=CSC_fwG_g_HV_36CHANNEL_BOARDS_CHAMBER_LIST[j][index2];
   if(dynContains(slice_test_chambers,dp2))break; // i.e. found will be false
   dpGet(mudcsAddSystem(dp2+".status"),status2);
   if(status2 == 0){
     dynAppend(master_channels[master_index],master_chan);
     dynAppend(masterss,master_index); //// &&&&&&&&&&&&&&&&&&&&
   }
   else {
       mudcsDebug("master channel remains ON \n as another chamber is connected");
   ////    mudcsDebug(dp2);
   }
   found=true;
   break;
  } //for

  if(!found){ // i.e. 30 channel board or there is one chamber connected to 36 ch board 
       dynAppend(master_channels[master_index],master_chan);
       dynAppend(masterss,master_index); //// &&&&&&&&&&&&&&&&&&&&
  }
//-----

////   dynAppend(master_channels[master_index],master_chan);
  }
 }
//---



// === vset for PRIMARY

if(dynlen(master_channels)>0){

value=voltage_primary;
/*
subcommand="HVPRCMD;0;1;255;7"+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host1+"|"+subcommand);
subcommand="HVPRCMD;0;1;255;7"+";"+value+";"+"-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host2+"|"+subcommand);


// === primary ON ======

subcommand="HVPRCMD;0;1;255;1;0;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host1+"|"+subcommand);
subcommand="HVPRCMD;0;1;255;1;0;-1";
dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),host2+"|"+subcommand);
*/
delay(2);
}

//+++++++++++++++++++++++++++++++++++++++++++

// === vset for MASTER ===

for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue; //// &&&&&&&&&&&&&&&&&&&& 

  master=CSC_fwG_g_all_masters[j];
  mudcsGetCoordForDp("HV_1", master, set);
  ///// dynAppend(master_sets,set);
  master_sets[j]=set; //// &&&&&&&&&&&&&&&&&&&&

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=voltage_master[j];
  coord=strsplit(set,";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+7+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
  dpSet(mudcsAddSystem("vset_master_100_20_15_default."),value);
 }
} // for j 

// ==== rampup for MASTER ===

for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue; //// &&&&&&&&&&&&&&&&&&&& 

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=15;
///  mudcsDebug(master_sets[j]);
  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+3+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

// ====== imax for MASTER ====
for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue; //// &&&&&&&&&&&&&&&&&&&& 

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=1500;
  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+6+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

// ===== TrigDelay for MASTER =====
for(j=1;j<=dynlen(master_channels);j++){

  if(!dynContains(masterss,j))continue; //// &&&&&&&&&&&&&&&&&&&& 

 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  value=100;
  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";"+38+";"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

delay(2);
// ===== MASTER ON =====
/*
for(j=1;j<=dynlen(master_channels);j++){

  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";0;30;1;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 
} // for j 

for(j=1;j<=dynlen(master_channels);j++){
 for(i=1;i<=dynlen(master_channels[j]);i++){
  im=master_channels[j][i];

  coord=strsplit(master_sets[j],";");
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+im+";0;0;-1";
/////mudcsDebug(subcommand);
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
 }
} // for j 

delay(3);
*/
//+++++++++++++++++++++++++++++++++++++++++++
// === vset for all ch ====


for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=voltage_chambers[1];

  mudcsGetCoordForDp("HV_1", slice_test_chambers[j], set);
  dynAppend(chamber_sets,set);
  coord=strsplit(set,";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";7;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

} // for j

// === rampup for all ch===

for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=15;
  coord=strsplit(chamber_sets[j],";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";3;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

} // for j

// ==== imax for all ch ====

for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=10;
  coord=strsplit(chamber_sets[j],";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";6;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

} // for j

// ==== trigDelay for all ch ====

for(j=1;j<=dynlen(slice_test_chambers);j++){

  value=500;
  coord=strsplit(chamber_sets[j],";");
    if(coord[4]=="0")     all_ch="255";
    else if(coord[4]=="1")all_ch="253";
    else if(coord[4]=="2")all_ch="254";
  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+all_ch+";38;"+value+";"+"-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

} // for j

delay(3);
// ====== CHAMBERS ON =======

int current_status;
for(i=1;i<=dynlen(slice_test_chambers);i++){
 dpGet(mudcsAddSystem(slice_test_chambers[i]+".status"),current_status);
 if(current_status != -2)
 dpSet(mudcsAddSystem(slice_test_chambers[i]+".status"),0);
}




}
//=========================================================
//=========================================================
//=========================================================

mudcsMasterChannelSwitch(int isOn, dyn_string coord, int channel, string fsm){

  string subcommand;
  string isOn_s=isOn;
  string channel_s=channel;
  string data;

  int voltage;
  string voltage_s;
  dyn_string coord_ch_dyn;
  string coord_ch;


  if(isOn){

/*
    dpGet(mudcsAddSystem(fsm+".coord"),coord_ch);
    coord_ch_dyn=strsplit(coord_ch,";");
    mudcsConvertFsm2DataOne(fsm,data);

     if(coord_ch_dyn[4] == "0" || coord_ch_dyn[4] == "1")dpGet(mudcsAddSystem(data+".data.v1.vset"),voltage);
     else dpGet(mudcsAddSystem(data+".data.v19.vset"),voltage);
*/
    dpGet(mudcsAddSystem(fsm+".last_vset"),voltage);

    if (voltage > 3000) { // nominal voltage
      voltage_s="4000";
    } else {              // standby voltage
      // check if a fellow chamber (if one exists) doesn't have a higher voltage setting
      string fellowChamberDp = emuhv_getFellowChamber(fsm);
      if (fellowChamberDp != "") {
        int fellowChamberVoltage;
        dpGet(mudcsAddSystem(fellowChamberDp + ".last_vset"), fellowChamberVoltage);
        if (fellowChamberVoltage > 3000) {
          voltage_s = 4000;
        } else if (fellowChamberVoltage > voltage) {
          voltage_s = fellowChamberVoltage;
        }
      } else {
        voltage_s=voltage;
      }
    }

    subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+channel_s+";"+7+";"+voltage_s+";"+"-1";
    dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
    
    subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+channel_s+";"+3+";12;"+"-1"; //ramp_up for masters
    dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);    
    
  }

  subcommand="HVCMD;"+coord[2]+";"+coord[3]+";0;30;1;-1";
  dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);

  if(isOn || !CSC_fwG_g_IS_TEMPORAL_50V){ // should work finally for both isOn and !isOn

   subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+channel_s+";"+isOn_s+";0;-1";
   dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
  }
  else{ // temporal !!!!!!!!!! to stay master channels ON at 50 VOLTS
   subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+channel_s+";1;0;-1";
   dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
   subcommand="HVCMD;"+coord[2]+";"+coord[3]+";"+channel_s+";7;50;-1";
   dpSetWait(mudcsAddSystem("HV_1_COM"+".command"),coord[1]+"|"+subcommand);
  }

}
