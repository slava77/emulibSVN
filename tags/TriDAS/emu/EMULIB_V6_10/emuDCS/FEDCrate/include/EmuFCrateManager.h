/*****************************************************************************\
* $Id: EmuFCrateManager.h,v 1.18 2009/01/29 15:31:22 paste Exp $
*
* $Log: EmuFCrateManager.h,v $
* Revision 1.18  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 1.17  2008/10/09 11:21:19  paste
* Attempt to fix DCC MPROM load.  Added debugging for "Global SOAP death" bug.  Changed the debugging interpretation of certain DCC registers.  Added inline SVG to EmuFCrateManager page for future GUI use.
*
* Revision 1.16  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 1.15  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 1.13  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __EMUFCRATEMANAGER_H__
#define __EMUFCRATEMANAGER_H__

#include <string>

#include "Application.h"

namespace emu {
	namespace fed {
		/** @class EmuFCrateManager An XDAq application class that relays commands from the CSCSupervisor class
		*	to the individual EmuFCrate classes.
		**/
		class EmuFCrateManager: public emu::fed::Application
		{

		public:
			XDAQ_INSTANTIATOR();

			/** Default constructor **/
			EmuFCrateManager(xdaq::ApplicationStub *stub);

			// HyperDAQ pages
			/** Default HyperDAQ page **/
			void webDefault(xgi::Input *in, xgi::Output *out);

			// FSM transition call-back functions
			/** Send the 'Configure' command to the EmuFCrate applications **/
			void configureAction(toolbox::Event::Reference event)
			throw (toolbox::fsm::exception::Exception);

			/** Send the 'Enable' command to the EmuFCrate applications **/
			void enableAction(toolbox::Event::Reference event)
			throw (toolbox::fsm::exception::Exception);

			/** Send the 'Disable' command to the EmuFCrate applications **/
			void disableAction(toolbox::Event::Reference event)
			throw (toolbox::fsm::exception::Exception);

			/** Send the 'Halt' command to the EmuFCrate applications **/
			void haltAction(toolbox::Event::Reference event)
			throw (toolbox::fsm::exception::Exception);

			/** Transition into an unknown FSM state because the underlying EmuFCrate applications' FSM states do not agree **/
			void unknownAction(toolbox::Event::Reference event);

			// FSM state change call-back function
			/** Decault FSM state change call-back function **/
			void inline stateChanged(toolbox::fsm::FiniteStateMachine &fsm) { return emu::fed::Application::stateChanged(fsm); }

			// SOAP call-back functions that send FSM transitions
			/** Start the FSM 'Configure' transition **/
			DEFINE_DEFAULT_SOAP2FSM_ACTION(Configure);

			/** Start the FSM 'Enable' transition **/
			DEFINE_DEFAULT_SOAP2FSM_ACTION(Enable);

			/** Start the FSM 'Disable' transition **/
			DEFINE_DEFAULT_SOAP2FSM_ACTION(Disable);

			/** Start the FSM 'Halt' transition **/
			DEFINE_DEFAULT_SOAP2FSM_ACTION(Halt);

			// Other SOAP call-back functions
			/** Set the TTS bits on the underlying EmuFCrate applications as defined by members ttsID_ and ttsBits_ **/
			xoap::MessageReference onSetTTSBits(xoap::MessageReference message);

		private:

			/** Get the combined FSM states of the underlying EmuFCrate applications.
			*
			*	@returns a string containing the state of the EmuFCrates if they are consistant, "Unknown" if they are inconsistant, and "Failed" if at least one is in a "Failed" state.
			**/
			std::string getUnderlyingStates();

			/// The TTS ID with which to communicate (sent from above) for FMM tests.
			xdata::Integer ttsID_;

			/// The target TTS bits to set for FMM tests.
			xdata::Integer ttsBits_;

			/// Whether or not the application has been configured via SOAP or via a web action.
			bool soapConfigured_;

		};
	}
}

#endif