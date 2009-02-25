mudcsLvMrtnFsmPanelsAndFsmAliasesSetMain(string system_to_look="")
{

  mudcsMrtnPnlInit();
  mudcsLvMrtnFsmPanelsAndFsmAliasesSet(system_to_look);  
 //mudcsLvDebug("Done");
}
//=================================================================
//=================================================================
//=================================================================
//=================================================================
//=================================================================
//=================================================================

mudcsLvMrtnFsmPanelsAndFsmAliasesSet(string system_to_look){

  dyn_string db_list;
  dyn_string db_split;
  int db_coord;
  dpGet(system_to_look+"db_mrtn.list",db_list);

  
  
string dir="CMS_CSCfw_LV_MRTN";
string fwUiPnl;
//if(!mode)
fwUiPnl=dir+"/emufwUi.pnl";
//else 
// fwUiPnl="fwFSMuser/fwUi.pnl";




//=================================================================

dyn_string os;
string o,s, s1;
int i,j, pos;
dyn_string panels;
string p;

  dyn_string d_string1, d_string2;
  string label, o_cut;
//----------------------------------------------------------------- 

// dpSet("fwOT_CRB_2.panel",   dir+"/emuDevCRB_2Operation.pnl");   
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
//o=mudcsAliasNameGet(o);
//////DebugTN("OOOOOOOOOOOOO3 "+o);
///////////DebugTN(mudcsAliasNameGet(o));

/*
s="CRB";
 if((pos=strpos(o,s))>=0){
p="emuDev"+"CRB_2"+"Operation.pnl"; 
  dynAppend(panels,dir+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=substr(o_cut,strlen("CSC_")+6);
  dpSet(objs[i]+".ui.label:_original.._value",label);
 DebugTN(d_string1[dynlen(d_string1)]+" >> "+dir+"/"+p);   
// fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );
 
  continue;
 }
 
*/

// FwWienerMarathon 
 if(dpTypeName(system_to_look+d_string1[dynlen(d_string1)]) == "FwWienerMarathon"){// if((pos=strpos(o,s))>=0)
p="fwWienerMarathonOperationCsc.pnl"; 
//  dynAppend(panels,"fwWiener"+"/"+p);
  dynAppend(panels,dir+"/"+p);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
//  label=substr(o_cut,strlen("CSC_")+6);
//  dpSet(objs[i]+".ui.label:_original.._value",label);
 DebugTN(d_string1[dynlen(d_string1)]+" >> "+dir+"/"+p);   
// fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );
 
  continue;
 }
 
  if(dpTypeName(system_to_look+d_string1[dynlen(d_string1)]) == "FwWienerMarathonChannel"){// if((pos=strpos(o,s))>=0)
p="fwWienerMarathonRCMChannelOperationCsc.pnl"; 
  dynAppend(panels,dir+"/"+p);
//  dynAppend(panels,"fwWiener"+"/"+p);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
//  label=substr(o_cut,strlen("CSC_")+6);
//  dpSet(objs[i]+".ui.label:_original.._value",label);
 DebugTN(d_string1[dynlen(d_string1)]+" >> "+dir+"/"+p);   
// fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );
 
  continue;
 }
// dpTypeName(dsFsm[dynlen(dsFsm)
 

 // FwWienerMarathon 
 if(dpTypeName(system_to_look+d_string1[dynlen(d_string1)]) == "FwWienerPS"){// if((pos=strpos(o,s))>=0)
p="fwWienerPSOperation.pnl"; 
//  dynAppend(panels,"fwWiener"+"/"+p);
  dynAppend(panels,"fwWiener/"+p);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
//  label=substr(o_cut,strlen("CSC_")+6);
//  dpSet(objs[i]+".ui.label:_original.._value",label);
 DebugTN(d_string1[dynlen(d_string1)]+" >> "+dir+"/"+p);   
// fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );
 
  continue;
 }
 
 if(dpTypeName(system_to_look+d_string1[dynlen(d_string1)]) == "FwWienerChannel"){// if((pos=strpos(o,s))>=0)
p="fwWienerChannelOperation.pnl"; 
  dynAppend(panels,"fwWiener/"+p);
//  dynAppend(panels,"fwWiener"+"/"+p);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
//  label=substr(o_cut,strlen("CSC_")+6);
//  dpSet(objs[i]+".ui.label:_original.._value",label);
 DebugTN(d_string1[dynlen(d_string1)]+" >> "+dir+"/"+p);   
// fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );
 
  continue;
 }
// dpTypeName(dsFsm[dynlen(dsFsm)
 
 
 
 


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

//DebugTN("+++++++++++"+o);

s="_CAN2";
if((pos=strpos(o,s))>=0){//&& pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuMRTNOperation.pnl";  // ****** S was added !!!!!!!
  dynAppend(panels,dir+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);
  continue;
 }
/*
s="LV_CRB";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuPCOperation.pnl";
  dynAppend(panels,dir+"/"+p);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
   label=substr(o_cut,strlen("CSC_"));
  dpSet(objs[i]+".ui.label:_original.._value",label);  
// fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );  
  continue;
 }
*/
 
  
s="LV_MRTN";
s1="_CR";
if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))) { 
  if(pos=strpos(o,s1)>=0){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
//p="emuMRTNOperation.pnl";
  p="fwWienerMarathonOperation.pnl"; // just to define the size here for fwUi.pnl : 
  dynAppend(panels,"fwWiener"+"/"+p);
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
  p="emuMRTNOperation.pnl";  // ****** S was added !!!!!!!
     label=substr(o_cut,strlen("CSC_"));
  dynAppend(panels,dir+"/"+p);    
  }
  
  dpSet(objs[i]+".ui.panels:_original.._value",panels);

  dpSet(objs[i]+".ui.label:_original.._value",label);  
// fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );  
  continue;
 }



s="_PL500";
s1="_CR";
if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))) { 

  if(pos=strpos(o,s1)>=0){

    
//panels=makeDynString("fwFSMuser/fwUi.pnl");
//p="emuMRTNOperation.pnl";
//  mudcsLvDebug("");  
    
  p="fwWienerPSOperation.pnl"; // just to define the size here for fwUi.pnl: 
  dynAppend(panels,"fwWiener"+"/"+p);
     label=substr(o_cut,strlen("CSC_"));
  }
  else{
  p="emuMRTNOperation.pnl";  // ****** S was added !!!!!!!
     label=substr(o_cut,strlen("CSC_"));
  dynAppend(panels,dir+"/"+p);    
  }
  
  dpSet(objs[i]+".ui.panels:_original.._value",panels);

  dpSet(objs[i]+".ui.label:_original.._value",label);  
// fwFsmTree_setNodePanel(d_string1[dynlen(d_string1)],dir+"/"+p );  
  continue;
 }


} //for

}
