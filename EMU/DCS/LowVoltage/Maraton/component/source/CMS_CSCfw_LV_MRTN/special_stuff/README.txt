

 03/25/2007

new component organization

 04/07/2007

also creation of mini component to be installed in
station project (mode=1)


 04/21/2007
 04/22/2007

delay(0,1000); in the main function of the mudcsLvServer: otherwise a lot of CPU time is consumed !!!!
 (may be delay(0,3600000)  -- because the handler only work!!!!)
equalizing crb and mrtn libraries emuLv.ctl and emuLvAlert.ctl
mudcsLvAddSystem and mudcsRemoveSystem correction
addition of alert handlers (dependence of fsm error states from alerting configuration: tested)
  mudcsLvServer -- while(CSC_fwCAN1_g_BLOCK_NEXT_DPCONNECT); -- for connection alert handlers

alert configuration: the summary alerting should be activated for all the time: not to deactivate for switching devices off 
  (within concept of the dependence of fsm error states from alerting configuration
   ro my note: the fsm error state is defined by FwWienerMarathonChannel_valueChanged
   and not totally equel to alerting )

emufw.ctl copy from pvss_fsm
 

//-----

 06/02/2007
 06/03/2007

 Addition of the PL500 in:

 1) creation HW tree
 2) DB creation 
 3) Logical & FSM trees creation
 4) LV server     (not tested)
 5) alert config  (not tested)
 6) set FSN panels  (not tested)
 7) create component (not tested)

//----

 also correction of Marathone part in alert functions

//=========

 09/25/2007

Correction of the FSM of the FwWienerMaratonChannel device type:
in the action section:

bool status;
   dpGet(device+".Status.On",status);
   dpSet(device+".Status.On",status);

It is prevent hanging of the FSM states of channels devices if 
   a) we switch maraton (and so channels)
   b) not communication with Maraton (PFS is off)

The idea is to call the status handler which otherwise is not called.

//===========

10/12/2007

MRTN project corrected to work within one project with CRB
a) config panels contain: 
      #uses "CMS_CSCfw_LV_MRTN/emuDcsLv.ctl" 
      .....
b) srever ctl contain 
      #uses "CMS_CSCfw_LV_MRTN/emuDcsLv.ctl" 
      .....

c) the config/config does not contain the MRTN libraries
   (and CMS_CSCfw_LV_MRTN_long.config accordingly does not contain too )
       
d) just comment: the FSM creation is not corrected the
   CRB and MRTN branches are created as root nodes.




//==================================

11/14/2007

the CRB and MRTN components should be installed very last
(especially after the RDBArchiving) because the wrong (long)
starting of RDBArchiving manager may affect the mudcsLvServer-s !!!!


//===================================


02/17/2008

The problem with FSM machne bypassed:
it is caused by the references of maratons in different folders
of the csc_station_p2 project. So the switching the top node
to the off state the FSM is confused (maratons can not come to 
a certain state). That was fixed by editing the action script
for fwwienermaraton device: (see // *** added)

      int Time = 30; 
     int m_state;// *** added
	if (command == "OFF")
	{
            dpGet(device+".Settings.OnOffCrate",m_state);  // *** added
            if(m_state==0){/*delay(2);*/ dpSetWait(device+".Status.On",FALSE);return;}// *** added
            fwDU_startTimeout(Time, domain, device, "ERROR","OFF"); 
		dpSetWait(device+".Settings.OnOffCrate",0);            
	}
	if (command == "ON")
	{
            dpGet(device+".Settings.OnOffCrate",m_state);// *** added
            if(m_state){/*delay(2);*/ dpSetWait(device+".Status.On",TRUE);return;}// *** added
            fwDU_startTimeout(Time, domain, device, "ERROR","ON");
            		dpSetWait(device+".Settings.OnOffCrate",1); 
	}








