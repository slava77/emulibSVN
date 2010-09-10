

 03/25/2007

new component organization


 04/07/2007

also creation of mini component to be installed in
station project (mode=1)

 
 04/21/2007
 04/22/2007

equalizing crb and mrtn libraries emuLv.ctl and emuLvAlert.ctl
mudcsLvAddSystem and mudcsRemoveSystem correction
mudcsLvServer -- while(CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT); -- for connection alert handlers
emufw.ctl copy from pvss_fsm



 06/03/2007

 some changes (shoudl not affect crb) 
 to equilize with mrtn project:
  a) alert config (mudcsLvAlert.ctl)
  b) component create (COMMON.ctl)
  c) emufw.ctl


07/06/2007

 adding the PSU component (for old ATLAS PSU and old PSU component fwElmbPSU_v1.0.6.zop)
   
---------

 moving PSU to the new ATLAS psu hardware/software:
   the difference in old and new csc_crb is just in emuDcsCrb.ctl:
 
CSC_fwCAN1_g_IS_NEW_ATLAS_PSU=false;
 

 08/10/2007

just emergency.emergencyErrorCode is included in summary alert of the PSU
(also the line around is moved upward)


//==================================================================

10/12/1007


CRB project corrected to work within one project with CRB
a) config panels contain: 
      #uses "CMS_CSCfw_LV_CRB/emuDcsLv.ctl" 
      .....
b) srever ctl contain 
      #uses "CMS_CSCfw_LV_CRB/emuDcsLv.ctl" 
      .....

c) the config/config does not contain the CRB libraries
   (and CMS_CSCfw_LV_CRB_long.config accordingly does not contain too )
       
d) just comment: the FSM creation is not corrected the
   CRB and MRTN branches are created as root nodes.

//======================================================================

10/14/2007

The FSM schema correction:
now not possible to ON from ERROR state:
The reason is : when we ON from ERROR state the alert_color stays in the same
value so the alert handler is not called, so the device stays in the state ON.
Comment1: firts I tried to do like this:
          dpGet(fsm+".:_alert_hdl.._act_state_color", alert_state);
          dpSetWait(fsm+".:_alert_hdl.._act_state_color", alert_state);
Hpwever the above method dose not work: the dpSetWait dows not cause the
alert handler update (for unknown reason)

Attention : if the STANDBY command is the same as ON command it also should be
forbidden from the ERROR state !!!

//==================================

11/14/2007

the CRB and MRTN components should be installed very last
(especially after the RDBArchiving) because the wrong (long)
starting of RDBArchiving manager may affect the mudcsLvServer-s !!!!

//==================================

04/25/2008

significantly improved the switching CRB

Problem of previous version: 

1) only one crb_busy variable was used for all CRB
2) imagine sequence:
   a) switch on the P2
   b) switch on the P1
   c) switch off the P1

- after (a) we will have 8 seconds while(crb_busy)
  for both "switch on P1" and "switch off P1"
  i.e. we will have a number of functions 
  (number of P1 CRBs (ON) and number of P1 CRBs (OFF))
   waiting on that while(crb_busy).
  Note: the FSM status while we wait at while(crb_busy)
  is OFF (the last switch) 

- so the switching OFF can be ahead of switching ON
  for some CRB
  when the crb_busy comes FALSE (released).
  so the CRB will be first switch OFF then ON.
  
One more observation:
If we do only say  "switch on the P2"
and even we have the common variable for all CRB-s 
(crb_busy) there will not be the waitings at the
entry of command handler because
the crb_busy set TRUE in the mudcsCrbCommandCrb function
so all P1 CRBs have a time to test "crb_busy"
as FALSE before the first CRB comes into mudcsCrbCommandCrb 























