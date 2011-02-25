

mudcsSuperSetPanels(string system_to_look="")
{


mudcsSuperInit();
string dir_oper=CSC_fwGSuper_g_Component;



//=================================
string fwUiPnl;
//if(!mode)
// fwUiPnl=dir_oper+"/emufwUi.pnl";
 fwUiPnl=dir_oper+"/fwUi_noclose.pnl";
//else 
// fwUiPnl="fwFSMuser/fwUi.pnl";


//=================================================================

dyn_string os;
string o,s, s1;
int i, pos;
dyn_string panels;
string p;

  dyn_string d_string1, d_string2;
  string label, o_cut;
//----------------------------------------------------------------- 
//==================================================================

dyn_string objs=dpNames(system_to_look+"*","_FwFsmObject");
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


DebugTN("test");

s="_CSC";
s1="_CSC1";
 if((pos=strpos(o,s))>=0 && pos==(strlen(o)-strlen(s)) ||
  (pos=strpos(o,s1))>=0 && pos==(strlen(o)-strlen(s1)) ){
   
   DebugTN("test1");
//panels=makeDynString("fwFSMuser/fwUi.pnl");
p="emuEmuTemplate.pnl";  // ****** S was added !!!!!!!
  dynAppend(panels,dir_oper+"/"+p);
//  mudcsDebug(o+" "+panels[dynlen(panels)]);
  dpSet(objs[i]+".ui.panels:_original.._value",panels);
  label=o_cut;
  dpSet(objs[i]+".ui.label:_original.._value",label);
  continue;
 }

/*
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
*/

} //for








DebugTN("SETPANAMES ==========================1");
//mudcsSuperDebug("done");
DebugTN("SETPANAMES ==========================2");
if(system_to_look=="")exit(0);

}

//========================================================================
