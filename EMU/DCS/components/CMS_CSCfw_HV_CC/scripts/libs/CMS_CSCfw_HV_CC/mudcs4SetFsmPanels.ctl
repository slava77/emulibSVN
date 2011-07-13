



mudcsSetFsmPanelsMain(string system_to_look="")
{

DebugTN("1");  




mudcsSetPanels(system_to_look);



//mudcsDebug("done");
}

mudcsSetPanels(string system_to_look){  

  dyn_string db_list;
  dyn_string db_split;
  int db_coord;
 
  
  
bool custom_lv;
string dir_oper;
string Component;//="CMS_CSCfw_HV_CC";
  
Component="CMS_CSCfw_HV_CC";    
mudcsInit();

 dpGet(CSC_fwG_g_MRTN_SYSTEM_NAME_P+":db_mrtn.list",db_list);

DebugTN("2");  
custom_lv=CSC_fwG_g_CUSTOM_LV; 
dir_oper=Component;


if(dpExists("fwOT_WTH_SX5"))dpSet("fwOT_WTH_SX5.panel", dir_oper+"/emuDevWTH_SX5Operation.pnl");
if(dpExists("fwOT_ALNM_1"))dpSet("fwOT_ALNM_1.panel",  dir_oper+"/emuDevALNM_1Operation.pnl");
if(dpExists("fwOT_CHIP_1"))dpSet("fwOT_CHIP_1.panel",  dir_oper+"/emuDevCHIP_1Operation.pnl");
if(dpExists("fwOT_CRB_1"))dpSet("fwOT_CRB_1.panel",   dir_oper+"/emuDevCRB_1Operation.pnl");
if(dpExists("fwOT_GAS_SX5"))dpSet("fwOT_GAS_SX5.panel", dir_oper+"/emuDevGAS_SX5Operation.pnl");
if(dpExists("fwOT_TEMP_1"))dpSet("fwOT_TEMP_1.panel",  dir_oper+"/emuDevTEMP_1Operation.pnl");
if(dpExists("fwOT_WNR12_1"))dpSet("fwOT_WNR12_1.panel", dir_oper+"/emuDevWNR12_1Operation.pnl");
if(dpExists("fwOT_HV_1"))dpSet("fwOT_HV_1.panel",    dir_oper+"/emuDevHV_1Operation.pnl");
if(dpExists("fwOT_HV_PR"))dpSet("fwOT_HV_PR.panel",   dir_oper+"/emuDevHV_PROperation.pnl");
if(dpExists("fwOT_LV_1"))dpSet("fwOT_LV_1.panel",    dir_oper+"/emuDevLV_1Operation.pnl");
if(dpExists("fwOT_MRTN_1"))dpSet("fwOT_MRTN_1.panel",  dir_oper+"/emuDevMRTN_1Operation.pnl");
if(dpExists("fwOT_PT100"))dpSet("fwOT_PT100.panel",   dir_oper+"/emuDevPT100Operation.pnl");
if(dpExists("fwOT_fwWnrCr_CSC_LV"))dpSet("fwOT_fwWnrCr_CSC_LV.panel",   dir_oper+"/emuDevfwWnrCr_CSC_LVOperation.pnl");



//=================================
string fwUiPnl;
//if(!mode)
 fwUiPnl=dir_oper+"/emufwUi.pnl";
//else 
// fwUiPnl="fwFSMuser/fwUi.pnl";

string standardFwUiPnl = "fwFSMuser/fwUi.pnl";
 
//=================================================================

dyn_string os;
string o,s, s1, s2, s3;
int i,j, pos;
dyn_string panels;
string p;

  dyn_string d_string1, d_string2, split;
  string label, o_cut;
//----------------------------------------------------------------- 
 
dyn_string objs=dpNames(system_to_look+"*","_FwFsmDevice");



for(i=1;i<=dynlen(objs);i++){

d_string1=strsplit(objs[i],"|");
d_string2=strsplit(d_string1[dynlen(d_string1)],"/");
o_cut=d_string2[dynlen(d_string2)];    
  
panels=makeDynString(fwUiPnl);

//////DebugTN("OOOOOOOOOOOOO1 "+objs[i]);

//mudcsDebug(objs[i]);
os=strsplit(objs[i],"|");
o=os[dynlen(os)];

/////DebugTN("OOOOOOOOOOOOO2 "+o);
o=mudcsAliasNameGet(o);
//////DebugTN("OOOOOOOOOOOOO3 "+o);
///////////DebugTN(mudcsAliasNameGet(o));

s="GAS_SX5";
 if((pos=strpos(o,s))>=0){
p="emuDev"+s+"Operation.pnl"; 
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

s="PT100";
 if((pos=strpos(o,s))>=0){
p="emuDev"+s+"Operation.pnl"; 
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

s="HV_PR";
 if((pos=strpos(o,s))>=0){
p="emuDev"+s+"Operation.pnl"; 
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=substr(o_cut,strlen("HV_PR_"));
  dpSet(objs[i]+".ui.label:_original.._value",label);
  continue;
 }

s="WTH_SX5";
 if((pos=strpos(o,s))>=0){
p="emuDev"+s+"Operation.pnl"; 
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

s="MRTN_1";
 if((pos=strpos(o,s))>=0){
p="emuDev"+s+"Operation.pnl"; 
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
   label=substr(o_cut,strlen("MRTN_"));
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

s="WienerSnmp";
s1="Channel";
 if((pos=strpos(o,s))>=0 && (pos=strpos(o,s1))<0){
p="emuDev"+"fwWnrCr_CSC_LV"+"Operation.pnl"; 
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=substr(o_cut,strpos(o_cut,"Crate"));
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }
 
s="WienerSnmp";
s1="Channel";
 if((pos=strpos(o,s))>=0 && (pos=strpos(o,s1))>=0){
p="emuDev"+"fwWnrCh_CSC_LV"+"Operation.pnl"; 
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
//  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=substr(o_cut,strpos(o_cut,"Channel"));
//  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

 
 
s="ALNM_1";
 if((pos=strpos(o,s))>=0){
p="emuDev"+s+"Operation.pnl"; 
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

s="CRB_1";
 if((pos=strpos(o,s))>=0){
p="emuDev"+s+"Operation.pnl"; 
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=substr(o_cut,strlen("CSC_")+6);
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

s="TEMP_1";
 if((pos=strpos(o,s))>=0){
p="emuDev"+s+"Operation.pnl"; 
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
    label=substr(o_cut,strlen("CSC_"));
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }
 
s="_FED";
s1="FED_1";
 if((pos=strpos(o,s))>=0){
p="emuDev"+s1+"Operation.pnl"; 
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
    label=substr(o_cut,0);
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

s="CHIP_1";
 if((pos=strpos(o,s))>=0){
p="emuDev"+s+"Operation.pnl"; 
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=substr(o_cut,strlen("CSC_"));
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

s="LV_1";
 if((pos=strpos(o,s))>=0){
p="emuDev"+s+"Operation.pnl"; 
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=substr(o_cut,strlen("CSC_"));
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

s="HV_1";
 if((pos=strpos(o,s))>=0){
p="emuDev"+s+"Operation.pnl"; 
  panels = makeDynString(standardFwUiPnl); //override this s**t..
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=substr(o_cut,strlen("CSC_"));
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }




} // for

//==================================================================

objs=dpNames(system_to_look+"*","_FwFsmObject");
//dyn_string os;
//string o,s;
//int i, pos;
//dyn_string panels;
//string p;



for(i=1;i<=dynlen(objs);i++){

d_string1=strsplit(objs[i],"|");
d_string2=strsplit(d_string1[dynlen(d_string1)],"/");
o_cut=d_string2[dynlen(d_string2)];
    
  panels=makeDynString(fwUiPnl);


//mudcsDebug(objs[i]);
os=strsplit(objs[i],"|");
o=os[dynlen(os)];

////////DebugTN(o);



s="csc";
s1="emu";
s2="dcspcS2G19";
s3="CMS_CSC";
// if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
  if((pos=strpos(o,s) )>=0 || (pos=strpos(o,s1) )>=0 || (pos=strpos(o,s2) )>=0 ||(pos=strpos(o,s3) )>=0  ){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
    
p="emuEmuOperationS.pnl";  // ****** S was added !!!!!!!
if((dpExists("CSC_ME_M1|CSC_ME_M1") && dpExists("CSC_ME_M2|CSC_ME_M2")
 && dpExists("CSC_ME_M3|CSC_ME_M3") && dpExists("CSC_ME_M4|CSC_ME_M4")) 
  ||
  (dpExists("CSC_ME_P1|CSC_ME_P1") && dpExists("CSC_ME_P2|CSC_ME_P2")
&& dpExists("CSC_ME_P3|CSC_ME_P3") && dpExists("CSC_ME_P4|CSC_ME_P4"))     )
p="emuEmuOperationST.pnl";

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);
  continue;
 }


//return;
  s="FED_M";
  s1="FED_P";
  s2="CSC_FED";
 if( ((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s)))
  || ((pos=strpos(o,s1))>=0 && pos==(strlen(o)-strlen(s1)))
   || ((pos=strpos(o,s2))>=0 && pos==(strlen(o)-strlen(s2))) ){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
//p="emuLVSubSystemOperation.pnl";
 p="emuEmptyOperation.pnl";  

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
    label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);
  continue;
 }  
  
s="_PSU";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
//p="emuLVSubSystemOperation.pnl";
 p="emuEmptyOperation.pnl";  

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
    label=substr(o_cut,strlen("CSC_ME_"));
  dpSet(objs[i]+".ui.label:_original.._value",label);
  continue;
 }  
  
s="_LV_HV";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
//p="emuLVSubSystemOperation.pnl";
 p="emuLV_MRTNOperation.pnl";  

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
    label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);
  continue;
 }
  
s="_HV";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuHVSubSystemOperation.pnl";

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);
  continue;
 }
s="_LV";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuLVSubSystemOperation.pnl";

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
    label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);
  continue;
 }
 
s="_LV_MINUS"; // temporal
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
 panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuLVSubSystemOperation.pnl";

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
    label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);
  continue;
 }

 
s="_WIENER_CRATE_SNMP";
 if((pos=strpos(o,s))>=0 ){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
 p="emuWienerOperation.pnl";
 //p="emuPanelComponentTemplate.pnl";  

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
    label=substr(o_cut,strpos(o_cut,"CRATE"));
    if(strpos(label,"500")>=0)label="LV_HV_CRATE_MINUS";
    else if(strpos(label,"600")>=0)label="LV_HV_CRATE_PLUS";   
  dpSet(objs[i]+".ui.label:_original.._value",label);
  continue;
 }
  
 
 
s="_GAS";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuGASSubSystemOperation.pnl";

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
   label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label); 
  continue;
 }

s="_WTH";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuWTHSubSystemOperation.pnl";

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

s="HV_HOSTS";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuHV_HOSTSOperation.pnl";

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }
 s="HV_HOST_";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s)-3)){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuHV_HOSTSOperation.pnl";

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=substr(o_cut,strpos(o_cut,"HOST_"));
    if(strpos(label,"500")>=0)label="HOST_MINUS";
    else if(strpos(label,"600")>=0)label="HOST_PLUS";     
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }
  
s="LV_MRTN";
s1="_CR";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
   if(strpos(o,s1)>=0){
    p="emuMRTNOperation.pnl";
    dynAppend(panels,"CMS_CSCfw_LV_MRTN"+"/"+p);
    label=substr(o_cut,strlen("CSC_"));
  strreplace(label,"_LV_","_");
  for(j=1;j<=dynlen(db_list);j++){
   db_split=strsplit(db_list[j],";");
   if(strpos(label,db_split[1])>=0){
     if(dynlen(db_split)>=2){
       db_coord=db_split[2];
//        sscanf(db_split[2],"%x",db_coord);
        //sprintf(s1,"%d",i1);
        //DebugTN(s1);
       label=label+"/Id "+db_coord; 
       
     }
   }
  }      
     
   }
   else{  
//panels=makeDynString("fwFSMuser/fwUi.pnl");
     split=strsplit(o,"_");
    if(CSC_fwG_g_CUSTOM_LV)p="emuLV_MRTNOperation.pnl";
    else p="emuEmptyOperation.pnl";
    dynAppend(panels,dir_oper+"/"+p);
    if(strpos(split[3],"2")>=0)split[3]=split[3]+"/3";
    label="LV_MARATONS"+"_"+split[3];

   }
   


//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

s="LV_CRB";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuPCOperation.pnl";

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  
  label=s;
  if((pos=strpos(o,"_PC"))>=0) label=substr(o_cut,strlen("CSC_")+6); // case of reference from CRB project
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

s="ME_P"; s1="ME_M"; 
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-5) ||
    (pos=strpos(o,s1))>=0 && pos==(strlen(o)-5))  {
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuDiskOperationS.pnl"; // ****** S was added !!!!!!!

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }


s="_TS_"; 
 if((pos=strpos(o,s))>=0)  {
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuSectorOperation.pnl";

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=substr(o_cut,strlen("CSC_"));
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

s="ME_P"; s1="ME_M"; 
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-10) ||
    (pos=strpos(o,s1))>=0 && pos==(strlen(o)-10))  {
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuChamberOperation.pnl";

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=substr(o_cut,strlen("CSC_"));
  dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }

s="HV_ME11";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuME11Operation.pnl";

  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  
 // label=s;
 // if((pos=strpos(o,"_PC"))>=0) label=substr(o_cut,strlen("CSC_")+6); // case of reference from CRB project
 // dpSet(objs[i]+".ui.label:_original.._value",label);  
  continue;
 }




} //for




//mudcsDebug("done");

}

//======================================



//========================================================================
