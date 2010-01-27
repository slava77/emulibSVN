mudcsGasFsmPanelsAndFsmAliasesSetMain(string system_to_look="")
{

  mudcsInit();
  mudcsGasInit();
  mudcsGasFsmPanelsAndFsmAliasesSet(system_to_look);  
// mudcsLvDebug("Done");
}
//=================================================================
//=================================================================
//=================================================================
//=================================================================
//=================================================================
//=================================================================

mudcsGasFsmPanelsAndFsmAliasesSet(string system_to_look){




  
  
string dir="CMS_CSCfw_GAS";
string dir_base="CMS_CSCfw_HV_CC";
string fwUiPnl;
//if(!mode)
fwUiPnl=dir_base+"/emufwUi.pnl";
fwUiPnl="fwFSMuser/fwUi.pnl";
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

 dpSet("fwOT_fwGasSystem_CSC_GAS.panel",   dir+"/emuDevfwGasSystem_CSC_GASOperation.pnl");
 dpSet("fwOT_fwCooling_CSC_COOLING.panel",   dir+"/emuDevfwCooling_CSC_COOLINGOperation.pnl");   
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


s="GAS_MONITOR";
 if((pos=strpos(o,s))>=0){
p="emuDev"+"fwGasSystem_CSC_GAS"+"Operation.pnl"; 
  dynAppend(panels,dir+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);

  label=o_cut;
 //   mudcsDebug(label);
  //label=substr(o_cut,strlen("CSC_"));

  
  dpSet(objs[i]+".ui.label:_original.._value",label);

 
  continue;
 }


 s="WATER_COOLING";
 if((pos=strpos(o,s))>=0){
p="emuDev"+"fwCooling_CSC_COOLING"+"Operation.pnl"; 
  dynAppend(panels,dir+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
//  mudcsDebug(o_cut);
 //\\ label=o_cut;
  label="WATER_and_AIR_COOLING";
  
  //label=substr(o_cut,strlen("CSC_"));

  
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

s="CSC_GAS";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuGASOperation.pnl";  // ****** S was added !!!!!!!
  dynAppend(panels,dir+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);
  continue;
 }

s="CSC_COOLING";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s))){
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuCOOLINGOperation.pnl";  // ****** S was added !!!!!!!
  dynAppend(panels,dir+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);
  continue;
 }



} //for

}
