ELMB FW v3.5.3
--------------

** VERY IMPORTANT **
====================
If you use the digital outputs of the ELMB, it is possible that you may need to change scipts/libs/panels
as the individual bits are no longer addressed. Functions have replaced any calls to dpSet on any digital
output bit. Please test all your code to check whether you need to change anything. All panels supplied
with the component work by using these functions, so if you use the standard panels, the changes will be
transparent.

IMPORTANT: This version of the ELMB framework can only be used with ELMB128. If you are still using the
old version of the ELMB (Atmega 103) you should get in touch with the ATLAS DCS group
(James.Cook@cern.ch, Helfried.Buckhart@cern.ch).

Change Log
==========
v3.0.1 27/09/2004 - Change log introduced.
       Bug Fixes: Custom sensor types can now be created successfully
                  Warnings for 'Constants already defined' shown in log viewer no longer exist
                  User entered prefix for sensor name (at creation time) now taken into account properly

       Additions: Custom sensor types can now be edited and/or deleted
                  New 'Standard' sensor types included for use with the Rack Control
                  'Standard' sensor types cannot be edited or deleted

v3.0.2 03/11/2004
       Bug Fixes: When configuring the ELMB ADC settings, the ELMB was not correctly set back into
                  the operational state. This has been rectified.
                  The formulas for the standard sensors for the rack control project have been corrected.

v3.0.2 03/11/2004
       Internal release only. Never released to the public

v3.0.4 15/11/2004
       Additions: Various changes have been made for use with the ELMB PSU component that is under development

v3.0.5 19/11/2004
       Internal release only. Never released to the public

v3.0.6 08/02/2005
       Additions: First prototype of ELMB monitoring included

v3.0.7 15/03/2005
       Additions: Online documentation added for most panels
       Bug Fixes: Errors in device definition causing problems creating devices
                  Errors in FwCore also created problems, so this release coincided with a FwCore release

v3.0.8 27/06/2005
       Additions: User functions added in FwElmbUser.ctl
                  Improved functionality when creating multiple ELMBs
       Bug Fixes: Minor bugs in prototype of ELMB monitoring fixed

v3.0.9 23/08/2005
       Bug Fixes: Minor bugs in FwElmbUser.ctl for user functions fixed.
                  Sensor parameter descriptions did not have any spaces (and so were difficult to read)
                  Creation of OPC configuration file bug fixed when 'e' notation used (e.g. 1e-006)

v3.0.10 01/09/2005
       Bug Fixes: Definition of a constant had been overlooked in previous versions

v3.1.0 16/01/2006
       Additions: On installation of the ELMB component, a Simulation Manager and an OPC Client
                  with the correct number is now entered into the list of managers in the console.
                  Updates to Device Definitions for all ELMB related datapoints to allow the
                  Configuration Database tool to be used properly with the ELMB.
                  When creating an analog input, a choice can be made for whether the channel
                  number is appended to the sensor name or not.

       Bug Fixes: Small bugs fixed in the library FwElmbUser.ctl causing an error when certain
                  functions returned after an exception was caught.
                  State Timer for OPC Server is now set to 10 seconds to remove a warning message
                  from the log viewer when the OPC Client is started.

v3.2.0 07.02.2006
       Additions: Possibility to define custom PDOs to be used for analog inputs.
                  Possibility to define a sensor as a 'raw value' sensor.
                  Hidden buttons for the ELMB monitoring until more development done (more a deletion!)

       Bug Fixes: Names in device definition for some DPTs where more than 32 characters, meaning
                  that they could not be stored correctly with the configuration database tool. This
                  has been rectified.

v3.2.1 16.02.2006
       Additions: Update to user function for creating a sensor so that the parameters for the function
                  can be taken from the definition of the sensor (rather than being supplied by user).
                  Increased use of constants defined for the ELMB component to aid in consistency.

v3.2.2 08.03.2006
       Additions: Minor updates to configuration panels for AI, DI and DO for consistency.
                  Addition of help to DI and DO configuration panels.

       Bug Fixes: Fixed bug in digital input and output creation panels which did not allow the selection
                  of port A unless all bits of port F (for DI) or port C (for DO) were already created.
                  Installation procedure improved to handle already existing custom sensors.
                  Removed references to libraries for other framework components in configuration file
                  entries, which caused problems when upgrading from older versions of the framework.

v3.3.0 30.06.2006
	Additions:	Inclusion of SPI into ELMB component
			Confirmation questions are now given before deleting any device in the ELMB component
			All scripts and libraries are now in subfolders called 'fwElmb' (you should manually
			  delete any old scripts and/or libraries starting with the prefix 'fwElmb').

	Bug Fixes:	Fixed bug that occurred if a user defined sensor did not contain any 'variable constants'

v3.3.1 05.09.2006
	Additions:	Comment for devices is now displayed in panels
			Valid, Invalid or Alert is now displayed in panels for analog/digital inputs/outputs
			Can now choose node IDs greater than 63 (for custom ELMB firmware)
			Buses that do not have any elements addressed are not output to OPC configuration
			  file, nor any devices lower in the hierarchy

	Bug Fixes:	Custom PDOs did not have COB-ID calculated correctly when more than one node (using
			  the same custom PDO) was declared

v3.3.2 03.11.2006
	Bug Fixes:	OPC Configuration file can now be created in other drives (e.g. D:\)
			Fixed issues with Digital Input and Output creation for PVSS 3.6

v3.3.3 05.12.2006
	Additions:	Simple monitoring of ELMB now included. Sets analog input channels to invalid
			  if state pre-operational or stopped, or if OPC connection lost, or if emergency
			  message for analog input timeout received.
			All emergency messages received are now entered into a string DPE within the 
			  FwElmbNode DPT. This contains emergency error code, all fields and a timestamp.

	Bug Fixes:	Emergency message now shown correctly in Node Operation panel

v3.3.4 15.03.2007
	Additions:  Input channels which are invalid are shown as bold/italic in the panels. This has
                    been added as the framework colour for came/went unacknowleged is the same as
                    when the value is invalid. Therefore, it was not easy to tell whether a value is
                    invalid, or whether the alert had just been unacknowledged.
                  A new item has been added which 'removes' the toggle bit from the state.value. This
                    allows alert handling to be added for a state which is not Operational. A default
                    alert handling for this item (state.noToggle) has also been added.
                  The timeout for reading the ADC configuration settings has been increased and the
                    mechanism for checking whether the reading has been successful has been improved.
                    This has been done as when there are many ELMBs on a bus, the previous timeout was
                    found to sometimes be too small.

v3.3.5 16.05.2007
      Additions:  May now format values shown in the table on the ELMB Node Operation Panel.
                  May select whether to filter out analog input channels that have already been assigned
                    with input sensors or not. This is for specific and advanced use-cases (only for experts).
                  Additions to handle better the sign within the sensor formula when the constants are
                    negative (or different than the sign in the formula itself).

	Bug Fixes:	Corrected the OPC Group configuration for Analog and Digital Inputs and Outputs to read
                    from the cache (rather than device).

v3.4.0 10.07.2007
      NOTE:       When updating to this version from an older version, when the post installation script
      -----         is run, error messages may be shown in the log viewer. This is only the case if the 
                    Control Manager with option '-f fwScripts.lst' starts before the Simulation Driver
                    with option '-num 7'. The exceptions seen will all be regarding functions with the
                    prefix 'fwPeriphAddress_' and can be ignored. The addressing is set as required even
                    though these errors are displayed.

                  If you wish to avoid these errors, ensure the the Simulation Driver with option '-num 7'
                    starts before the post installation scripts.

      Additions:  Digital outputs are now handled completely differently. The only addressed item is for
                    digital output bytes and the individual bits do not have addressing. Setting a value
                    into a datapoint element of a digital output bit will have no effect. Functions have
                    been created which should be used instead (available in fwElmbbUser.ctl).
                  Digital input configuration information has been extended to allow for Port A input enable
                    mask and the interrupt enable masks for both Ports A and F to be modified.
                  Minor version of the ELMB firmware can now be read out.
                  Output ports A and C may now be read (used to keep consistency between hardware and
                    software).
                  
      Bug Fixes:  The state.noToggle datapoint function is now added to each node (which was only done when
                    the ELMB was created previously).
                  The first time the OPC Server configuration file is created, it could only be saved to the
                    root C:\ drive. It may now be created in any folder.
                  Corrections made to OPC Server configuration file for when Port A has any bits defined as
                    inputs.

v3.4.1 06.08.2007

      Bug Fixes:  Fixed bug with digital outputs that made it seem like setting a digital output on one ELMB
                    also changed digital outputs on other ELMBs (though this was not actually the case)

v3.5.0 25.01.2008

      Additions:  Analog outputs have been added to the ELMB framework component.
                  New entry added to CANbus operation panel to manage ALL buses on the system.
                  Addition of panel (fwElmb\objects\fwElmbManageOpcGroups.pnl) to aid the management of unused
                    OPC groups (possibly due to a CANbus being removed from the system).

      Bug Fixes:  Fixed bug for querying SDO item values from a remote system.

v3.5.1 29.04.2008

      Additions:  Analog inputs which use SDOs for the values have now been included. This is mainly for
                    use by the radiation monitoring sensors, and probably is not much use to other applications.
                  OPC groups are now created 'on demand', that is, the group used by analog inputs is
                    created when the first analog input is created.
                  Digital input lines on ports D and E may now be created. Note that the usage of these ports
                    requires custom firmware, which is not officially supported.

      Bug Fixes:  Checking whether the OPC client is running now checks remote system if necessary.
                  Fixed (random) bug seen during installation on some systems, where the FwElmbNode DPT was
                    not correctly updated.

v3.5.2 24.06.2008

      Additions:  New datapoint element added to FwElmbNode to indicate whether ANY analog input channel is
                    marked as invalid. This boolean value may then be used to show an alert. The new DPE is
                    updated only if the script 'fwElmb/fwElmbCheckInvalid.ctl' is running.

v3.5.2 24.06.2008

      Bug Fixes:  If the event timer was used (instead of SYNC Interval), the requested value for the timer
                    interval was set as milliseconds (instead of seconds) and therefore was 1000 times to big.
