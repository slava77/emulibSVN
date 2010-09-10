

 03/28/2007 

copule of days ago:
 new HV_PR protocol
 new file structure
 creating component script very modified
 some corrections

04/2/2007

this is the combined version of the:
 a) custom tree || integrated LV projects
 b) wiener snmp

Data2Fsm converion routines: bug fised : test1[dynlen(test1)]
there was like this: test1[dynlen(test)]

Also: there is a temporal comment out of 
WienerSnmp dim commands configuration: emuDcsDim.ctl : line 163

06/16/2007

FSM changed (STANDBY was added more in EMUNodes, EMUTREENodes and 
                STANDBY was in devices)

06/17/2007

FSM changed:

1) EMUNodes and EMUTREESTBYNodes  are the same (EMUNodes type is used for main node)
    if(any children in stby  --> state is stby) (on conditions: there is no error)    
     examples: TOP node
               ME_P1_TS5 (sector folder) (currently it is EMUTREENodes as there is no child in stby possible)

2') EMUSTBYNodes = EMUTREENodes

2) EMUSTBYNodes1
    if(there is a child in ON and there is achild in OFF --> state is stby) 
    if(any children in stby  --> state is stby) (on conditions: there is no error)      
     examples: CSC_LV



3) EMUTREENodes
    can not be in stby state but transfer the command stby firther

4) currently the PCMB and LV_1 and TEMP_1 devices are stay ON at stby command
   probably they should be OFF at stby command: if so they can be included in FSM
   (PCMB and LV_1 and TEMP_1 devices are excluded from FSM as they give ERROR at MRTN off)

//=====

 06/25/2007


panel is created(more exactly added): FsmTreeCreate.pnl 
which creates everything: types, trees, config alerst and  set fsm panels and create components

 06/26/2007
function:
mudcsFsmErrorSet(string subtype, string test_string, string test_string_d, bool isSetOn=true)
was corrected in two ways:
1) test on  _alert_hdl.._active is replaced by test on _alert_hdl.._dp_list
2) added case of not alert configured devices (e.g. fsWnrCh)

//=====

 07/08/2007

 1) added mudcsCreateAtlasPsu (to integrate atlas psu): NOT TESTED
 2) copied emufw.ctl from crb project to equalize with LV components: NOT TESTED

//================================================================================

 07/21/2007

ln -s /usr/bin/X11/nedit $HOME/bin/xemacs
is needed to use FSM editors in editor&navigator 


 07/22/2007

1) timeout for watch dog now is 180 seconds (in not standalone mode)
2) FSM schema are changed (was strange ON command in OFF state for STNBY types)
3) panels changed for not standalone mode

//=================================================================================

 09/25/2007

FSM changed:

1) Winer PL512 crates and channels devices are configured not to be turned off an command OFF
2) The same shoudl be done for PSU.

//==============================

10/11/2007

unix2dos is not needed: the windows PVSS understands the unix format
(tested !!!)
the special_stuff/unix2dos is remained just in case

//===============================


11/14/2007

the HV_CC component should be installed very last
(especially after the RDBArchiving) because the wrong (long)
starting of RDBArchiving manager may affect the mudcsServer !!!!

//===============================

12/04/2007

in status handler of LV and TEMP:

 if(value==-2){
  mudcsConvertFsm2DataOne(dpName, data);
  mudcsAlertSetActive(data,true);
 }

because:

a) the LV and TEMP cann be alert off after turning off
and on turning all stuff on, the LV and TEMP devices are not 
turned on for they are in the error state (-2 state).
No action ON for LV and TEMP when in the ERROR state.


//===============================

01/11/2008

/etc/hosts

10.176.11.96    pcmslvux01.cern.ch      pcmslvux01--cms pcmslvux01
10.176.11.103 ufcmshv2--cms.cms ufcmshv2--cms
10.176.3.81 cms-cent-dcs-01 cms-cent-dcs-01.cms cms-cent-dcs-01.cern.ch

in particular, the DIM_DNS_NODE=cms-cent-dcs-01.cms
does not work anymore (that is trying to look for cms-cent-dcs-01.cern.ch)
probably because of change dim version on cms-cent-dcs-01 machine

//================================


 02/24/2008

FSM modification:
LV_1 is set to 2 on OFF command (not to turn off it) at all.
Ass the OFF command was allowed from ERROR (-2) that led to
pseudo turning on the LV_1.
(set to 0(as it was previously) for -2 state returns state to -2 in mudcsServer(LV_1))
So OFF command is removed for ERROR state.


//======================================


 04/13/2008

move to new watch timeouts
see: dp WATCH_TIME1, WATCH_TIME2,...  
of type MUDCS_TIME

//======================================

05/05/2008

WATCH_TIME topic:

mudcsInit() (i.e. with no parameter!!!)
was commnted out in:

////emuEmuAdditionalOperation.pnl (returned init)
emuEmuPCs.pnl
emuDcs2.ctl :: mudcsProjectConnect(){
//mudcsDebug($project_system);
//if($project_system=="")mudcsInit(); // commented out as everything should be defined (globals) for when connection via PVSS00ui 

The problem was caused by mudcsInit() call in the 
emuDcs2.ctl:: mudcsProjectConnect() when $project_system is not defined
(case of the upper HV_CC node panel) -> the mudcsInit() was called simultaneously
in a few panels (HVcontrol(that contained primary panel and master panels), additionalOperation)
that led to the wrong creation of the WATCH_TIME datapoints in the mudcsInit()

//=========================================

05/09/2008

HV alert configuration from now:
if channel status >= 4 alert is issued (starting from status DEAD)
Before there was >=5 (starting from status Interlock) 

//========================================

08/04/2008



commention out at HV_1_new:

///dpSetWait(test_string+".data.module_state",status_source);
///dpSetWait(test_string+".data.chamber_complex_status",chamber_complex_status);
which led to:

switching on-ramp-on-ramp when click refresh for
small !!!! chambers !!!!

(trick: the handler HV_1_new can work almost in parallel for the same chamber 
when we click refresh that conflict thw dpSet with new module_state and chamber_complex_status
just comming with data (like 1001 comming and we set just 1000 or 1))
