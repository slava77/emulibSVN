/*****************************************************************************\
* $Id: EmuFCrate.cc,v 3.55 2009/01/30 19:14:16 paste Exp $
*
* $Log: EmuFCrate.cc,v $
* Revision 3.55  2009/01/30 19:14:16  paste
* New emu::base namespace and emu::base::Supervised inheritance added.
*
* Revision 3.54  2009/01/29 15:31:24  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.53  2008/11/14 09:34:31  paste
* Updated IRQ thread handling to fix and abstract FMM enabling and disabling.
*
* Revision 3.52  2008/10/30 12:56:11  paste
* Fixing more map-related bugs in IRQData
* Changing IRQ FMM threshold to > 8 chambers (from > 1 chamber)
*
* Revision 3.51  2008/10/29 16:01:44  paste
* Updated interoperability with primative DCC commands, added new xdata variables for future use.
*
* Revision 3.50  2008/10/22 20:23:58  paste
* Fixes for random FED software crashes attempted.  DCC communication and display reverted to ancient (pointer-based communication) version at the request of Jianhui.
*
* Revision 3.49  2008/10/15 00:46:56  paste
* Attempt to solve certain crashes on Enable/Disable commands.
*
* Revision 3.48  2008/10/13 11:56:40  paste
* Cleaned up some of the XML config files and scripts, added more SVG, changed the DataTable object to inherit from instead of contain stdlib objects (experimental)
*
* Revision 3.47  2008/10/04 18:44:05  paste
* Fixed bugs in DCC firmware loading, altered locations of files and updated javascript/css to conform to WC3 XHTML standards.
*
* Revision 3.46  2008/09/30 08:32:40  paste
* Updated IRQ Threads so that the endcap name is mentioned in the log filename
*
* Revision 3.45  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 3.44  2008/09/22 14:31:54  paste
* /tmp/cvsY7EjxV
*
* Revision 3.43  2008/09/19 23:13:59  paste
* Fixed a small bug in disabling of error reporting, added missing file.
*
* Revision 3.42  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.41  2008/08/31 21:18:27  paste
* Moved buffers from VMEController class to VMEModule class for more rebust communication.
*
* Revision 3.40  2008/08/26 13:09:02  paste
* Documentation update.
*
* Revision 3.39  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.38  2008/08/18 08:30:15  paste
* Update to fix error propagation from IRQ threads to EmuFCrateManager.
*
* Revision 3.35  2008/08/15 16:14:51  paste
* Fixed threads (hopefully).
*
* Revision 3.34  2008/08/15 09:59:22  paste
* Fixed bug where transitions to Halted state while threads were not active caused a crash.
*
* Revision 3.33  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "EmuFCrate.h"

#include <sstream>

#include "xgi/Method.h"
#include "cgicc/HTMLClasses.h"
#include "FEDCrate.h"
#include "DDU.h"
#include "Chamber.h"
#include "DCC.h"
#include "IRQData.h"
#include "VMEController.h"
#include "FEDCrateParser.h"

XDAQ_INSTANTIATOR_IMPL(emu::fed::EmuFCrate);

emu::fed::EmuFCrate::EmuFCrate(xdaq::ApplicationStub *stub):
Application(stub),
ttsCrate_(0),
ttsSlot_(0),
ttsBits_(0),
soapConfigured_(false)
{

	// Variables that are to be made available to other applications
	getApplicationInfoSpace()->fireItemAvailable("xmlFileName", &xmlFile_);
	getApplicationInfoSpace()->fireItemAvailable("ttsCrate", &ttsCrate_);
	getApplicationInfoSpace()->fireItemAvailable("ttsSlot",  &ttsSlot_);
	getApplicationInfoSpace()->fireItemAvailable("ttsBits",  &ttsBits_);

	// HyperDAQ pages
	xgi::bind(this, &emu::fed::EmuFCrate::webDefault, "Default");

	// SOAP call-back functions which fire the transitions to the FSM
	BIND_DEFAULT_SOAP2FSM_ACTION(EmuFCrate, Configure);
	BIND_DEFAULT_SOAP2FSM_ACTION(EmuFCrate, Enable);
	BIND_DEFAULT_SOAP2FSM_ACTION(EmuFCrate, Disable);
	BIND_DEFAULT_SOAP2FSM_ACTION(EmuFCrate, Halt);

	// Other SOAP call-back functions
	xoap::bind(this, &emu::fed::EmuFCrate::onSetTTSBits, "SetTTSBits", XDAQ_NS_URI);
	//xoap::bind(this, &emu::fed::EmuFCrate::onGetParameters, "GetParameters", XDAQ_NS_URI);

	// FSM state definitions and state-change call-back functions
	fsm_.addState('H', "Halted", this, &emu::fed::EmuFCrate::stateChanged);
	fsm_.addState('C', "Configured", this, &emu::fed::EmuFCrate::stateChanged);
	fsm_.addState('E', "Enabled", this, &emu::fed::EmuFCrate::stateChanged);

	// FSM transition definitions
	fsm_.addStateTransition('H', 'C', "Configure", this, &emu::fed::EmuFCrate::configureAction); // valid
	fsm_.addStateTransition('C', 'C', "Configure", this, &emu::fed::EmuFCrate::configureAction); // valid
	fsm_.addStateTransition('E', 'C', "Configure", this, &emu::fed::EmuFCrate::configureAction); // invalid
	fsm_.addStateTransition('F', 'C', "Configure", this, &emu::fed::EmuFCrate::configureAction); // invalid
	
	fsm_.addStateTransition('H', 'C', "Disable", this, &emu::fed::EmuFCrate::disableAction); // invalid
	fsm_.addStateTransition('C', 'C', "Disable", this, &emu::fed::EmuFCrate::disableAction); // invalid
	fsm_.addStateTransition('E', 'C', "Disable", this, &emu::fed::EmuFCrate::disableAction); // valid
	fsm_.addStateTransition('F', 'C', "Disable", this, &emu::fed::EmuFCrate::disableAction); // invalid
	
	fsm_.addStateTransition('H', 'E', "Enable", this, &emu::fed::EmuFCrate::enableAction); // invalid
	fsm_.addStateTransition('C', 'E', "Enable", this, &emu::fed::EmuFCrate::enableAction); // valid
	fsm_.addStateTransition('E', 'E', "Enable", this, &emu::fed::EmuFCrate::enableAction); // invalid
	fsm_.addStateTransition('F', 'E', "Enable", this, &emu::fed::EmuFCrate::enableAction); // invalid
	
	fsm_.addStateTransition('H', 'H', "Halt", this, &emu::fed::EmuFCrate::haltAction); // valid
	fsm_.addStateTransition('C', 'H', "Halt", this, &emu::fed::EmuFCrate::haltAction); // valid
	fsm_.addStateTransition('E', 'H', "Halt", this, &emu::fed::EmuFCrate::haltAction); // vlad
	fsm_.addStateTransition('F', 'H', "Halt", this, &emu::fed::EmuFCrate::haltAction); // valium

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	// Other initializations
	TM_ = new IRQThreadManager(endcap_);

}



xoap::MessageReference emu::fed::EmuFCrate::onSetTTSBits(xoap::MessageReference message)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Remote SOAP command: SetTTSBits");
	
	// Check to see if this instance is in command of the given crate number and slot
	bool found = false;
	for (std::vector<FEDCrate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		if ((*iCrate)->number() != ttsCrate_) continue;
		
		for (std::vector<DDU *>::iterator iDDU = (*iCrate)->getDDUs().begin(); iDDU != (*iCrate)->getDDUs().end(); iDDU++) {
			if ((*iDDU)->slot() == ttsSlot_) {
				found = true;
				break;
			}
		}
		if (found) break;
		for (std::vector<DCC *>::iterator iDCC = (*iCrate)->getDCCs().begin(); iDCC != (*iCrate)->getDCCs().end(); iDCC++) {
			if ((*iDCC)->slot() == ttsSlot_) {
				found = true;
				break;
			}
		}
	}
	
	if (!found) {
		LOG4CPLUS_INFO(getApplicationLogger(), "ttsCrate_=" << ttsCrate_.toString() << ", ttsSlot_=" << ttsSlot_.toString() << " is not commanded by this application");
		return createReply(message);
	} else {
		LOG4CPLUS_INFO(getApplicationLogger(), "Writing ttsCrate_=" << ttsCrate_.toString() << " ttsSlot_=" << ttsSlot_.toString() << " ttsBits_=" << ttsBits_.toString());
	}

	// cache TTS bits
	xdata::Integer cachedBits = ttsBits_;
	try {
		// set TTS bits
		writeTTSBits(ttsCrate_, ttsSlot_, ttsBits_);
		// read back TTS bits
		ttsBits_ = readTTSBits(ttsCrate_, ttsSlot_);
	} catch (emu::fed::TTSException &e) {
		std::ostringstream error;
		error << "Set TTS bits in crate " << ttsCrate_.toString() << ", slot " << ttsSlot_.toString() << " has failed";
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::TTSException, e2, error.str(), e);
		notifyQualified("ERROR", e2);
		return createReply(message);
	}

	LOG4CPLUS_DEBUG(getApplicationLogger(), "Read back ttsBits_=" << ttsBits_.toString());
	
	if (ttsBits_ != cachedBits) {
		std::ostringstream error;
		error << "Read back ttsBits_=" << ttsBits_.toString() << " from ttsCrate_=" << ttsCrate_.toString() << ", ttsSlot_=" << ttsSlot_.toString() << ", should have been " << cachedBits.toString();
		LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
		XCEPT_DECLARE(emu::fed::TTSException, e, error.str());
		notifyQualified("ERROR", e);
	}

	// PGK Remember:  you can always steal the TTSBits status via SOAP if you really, really want it.
	return createReply(message);
}



void emu::fed::EmuFCrate::configureAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{

	LOG4CPLUS_DEBUG(getApplicationLogger(), "Entering EmuFCrate::configureAction");

	// Determine whether this is a local configure or a configure from SOAP
	if (soapLocal_) {
		soapLocal_ = false;
		soapConfigured_ = false;
	} else {
		soapConfigured_ = true;
	}

	// JRG: note that the HardReset & Resync should already be done by this point!

	// PGK Easier parsing.  Less confusing.
	LOG4CPLUS_DEBUG(getApplicationLogger(), "EmuFCrate::configureAction using XML file " << xmlFile_.toString());
	try {
		FEDCrateParser *parser = new FEDCrateParser(xmlFile_.toString().c_str());
		// From the parser, set the crates.
		crateVector_ = parser->getCrates();
		// Get the name of this endcap from the parser, too.  This is specified in the XML
		// for convenience.
		endcap_ = parser->getName();
	} catch (emu::fed::ParseException &e) {
		std::ostringstream error;
		error << "Unable to create FED objects by parsing";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::ParseException, e2, error.str(), e);
		notifyQualified("FATAL", e2);
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
	} catch (emu::fed::FileException &e) {
		std::ostringstream error;
		error << "Unable to create FED objects due to file exception";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::ParseException, e2, error.str(), e);
		notifyQualified("FATAL", e2);
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
	}


	
	// First, we must make a system to parse out strings.  String-fu!
	// Strings looke like this:  "Crate# BHandle# Crate# BHandle# Crate# BHandle#..."
	/*
	std::map< int, int > BHandles;
	
	LOG4CPLUS_DEBUG(getApplicationLogger(),"Got old handles: " << BHandles_.toString());

	// Parse out the string into handles (if they have already been opened by HyperDAQ)
	std::stringstream sHandles(BHandles_.toString());
	int buffer;
	while (sHandles >> buffer) {
		int crateNumber = buffer;
		sHandles >> buffer;
		if (!sHandles.good()) {
			std::ostringstream error;
			error << "Unable to properly parse BHandle data (" << BHandles_.toString() << ")";
			XCEPT_DECLARE(emu::fed::ParseException, e, error.str());
			notifyQualified("WARN", e);
			LOG4CPLUS_WARN(getApplicationLogger(), error.str());
		}
		int BHandle = buffer;
		
		BHandles[crateNumber] = BHandle;
	}

	// Now we have to see if we need new handles from the crate vector.
	std::ostringstream newHandles;

	for (std::vector< FEDCrate * >::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		// The controller knows its handle.
		VMEController *myController = (*iCrate)->getController();

		// A handle of -1 means that opening of the handle failed, so someone has
		// already opened it (HyperDAQ)
		if (myController->getBHandle() == -1) {
			LOG4CPLUS_DEBUG(getApplicationLogger(),"Controller in crate " << (*iCrate)->number() << " has already been opened by someone else.  Looking for the BHandle...");

			// If that is the case, we can look up the handle.  Handles are stored by
			// create NUMBER (not index), so we need to check against our crate number.
			for (std::map<int,int>::iterator iHandle = BHandles.begin(); iHandle != BHandles.end(); iHandle++) {
				
				if (iHandle->first != (*iCrate)->number()) continue;
				LOG4CPLUS_DEBUG(getApplicationLogger(),"Found handle " << iHandle->second);
				(*iCrate)->setBHandle(iHandle->second);

				// In this case, just copy the old handle information to our new string.
				newHandles << iHandle->first << " " << iHandle->second << " ";
				
				break;
			}
		} else {

			// If the handle is a reasonable number, we assume that we created it.  Use it.
			LOG4CPLUS_DEBUG(getApplicationLogger(),"Controller in crate " << (*iCrate)->number() << " has been first opened by this application.  Saving the BHandle...");
			
			bool replaced = false;

			// It may be the case that the handle was opened at some point in the past,
			// then closed.  If that is the case, we need to reset the handle in the string.
			for (std::map<int,int>::iterator iHandle = BHandles.begin(); iHandle != BHandles.end(); iHandle++) {
				
				if (iHandle->first != (*iCrate)->number()) continue;
				LOG4CPLUS_DEBUG(getApplicationLogger(),"Resetting handle (was " << iHandle->second << ")");
				replaced = true;
				newHandles << (*iCrate)->number() << " " << myController->getBHandle() << " ";
			}

			// If nothing was replaced (we didn't find the crate number in the previous
			// string), then we write a new one.
			if (!replaced) newHandles << (*iCrate)->number() << " " << myController->getBHandle() << " ";
			
		}

	}
	
	LOG4CPLUS_DEBUG(getApplicationLogger(),"Saving new handles: " << newHandles.str());
	
	BHandles_ = newHandles.str();
	*/


	// PGK No hard reset or sync reset is coming any time soon, so we should
	//  do it ourselves.
	for (std::vector<FEDCrate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {

		// Only reset if we have a DCC in the crate.
		std::vector<DCC *> dccs = (*iCrate)->getDCCs();

		// Don't reset crate 5 (TF)
		if (dccs.size() > 0 && (*iCrate)->number() <= 4) {
			LOG4CPLUS_DEBUG(getApplicationLogger(), "HARD RESET THROUGH DCC!");
			try {
				dccs[0]->crateHardReset();
			} catch (emu::fed::Exception &e) {
				std::ostringstream error;
				error << "Hard reset through DCC in crate " << (*iCrate)->number() << " slot " << dccs[0]->slot() << " has failed";
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				XCEPT_DECLARE_NESTED(emu::fed::Exception, e2, error.str(), e);
				notifyQualified("FATAL", e2);
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
			}
		}

		// Now we do the configure.  This is big.
		LOG4CPLUS_DEBUG(getApplicationLogger(), "Configuring crate " << (*iCrate)->number());
		try {
			(*iCrate)->configure();
		} catch (emu::fed::ConfigurationException &e) {
			std::ostringstream error;
			error << "Configuration of crate " << (*iCrate)->number() << " has failed";
			LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
			XCEPT_DECLARE_NESTED(emu::fed::Exception, e2, error.str(), e);
			notifyQualified("FATAL", e2);
			XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e2);
		}
	}

// JRG, add loop over all DDUs in the FED Crates
//   1) verify the L1A scalers at the DDU FPGA == 0, use  DDU::ddu_rdscaler()
//   2) get the Live Fibers "Link Active" from the InFPGAs and check these
//	  against the Flash settings (or DB settings or the XML?)
//	   -use  DDU::infpga_CheckFiber(enum DEVTYPE dv)
//	   -use "Kill Channel Mask" from  DDU::read_page1()
//   3) check that the DDU "CSC Error Status" is Zero (like top page).
//         -Read "CSC status summary for FMM" from  DDU::vmepara_CSCstat()
//	   -Verify the 32-bit status from DDUFPGA == 0?
//              use  DDU::ddu_fpgastat()&0xdecfffff   <<- note the mask
//	   -Verify the 32-bit status from InFPGAs == 0?
//              use  DDU::infpgastat(enum DEVTYPE dv)&0xf7eedfff   <<- note the mask
//	 -->> definitely need to ignore some bits though!  see the masks


	for (std::vector<FEDCrate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		
		// Set FMM error reporting disable.  Not on TF, though
		if ((*iCrate)->number() < 5) {
			(*iCrate)->getBroadcastDDU()->disableFMM();
		}

		std::vector<DDU *> myDDUs = (*iCrate)->getDDUs();
		for (std::vector<DDU *>::iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++) {
			
			try {

				LOG4CPLUS_DEBUG(getApplicationLogger(), "Checking XML, FPGA, and flash values for DDU in crate " << (*iCrate)->number() << ", slot " << (*iDDU)->slot());

				uint32_t flashKillFiber = (*iDDU)->readFlashKillFiber();
				uint32_t fpgaKillFiber = (*iDDU)->readKillFiber();
				uint32_t xmlKillFiber = (*iDDU)->getKillFiber();

				LOG4CPLUS_DEBUG(getApplicationLogger(), "killFiber for DDU in crate " << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << ": XML(" << std::hex << xmlKillFiber << std::dec << ") FPGA(" << std::hex << fpgaKillFiber << std::dec << ") flash(" << std::hex << flashKillFiber << std::dec << ")");

				if ((flashKillFiber & 0x7fff) != (xmlKillFiber & 0x7fff)) {
					LOG4CPLUS_INFO(getApplicationLogger(),"Flash and XML killFiber for DDU in crate " << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << " disagree:  reloading flash");
					(*iDDU)->writeFlashKillFiber(xmlKillFiber & 0x7fff);
					
					// Check again.
					uint32_t newFlashKillFiber = (*iDDU)->readFlashKillFiber();
					if ((newFlashKillFiber & 0x7fff) != (xmlKillFiber & 0x7fff)) {
						std::ostringstream error;
						error << "Flash (" << std::hex << newFlashKillFiber << ") and XML (" << xmlKillFiber << ") killFiber for DDU in crate " << std::dec << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << " disagree after an attempt was made to reload the flash.";
						LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
						XCEPT_DECLARE(emu::fed::ConfigurationException, e, error.str());
						notifyQualified("ERROR", e);
					}
				}
				
				if (fpgaKillFiber != xmlKillFiber) {
					LOG4CPLUS_INFO(getApplicationLogger(),"FPGA and XML killFiber for DDU in crate " << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << " disagree:  reloading FPGA");
					(*iDDU)->writeKillFiber(xmlKillFiber);
					
					// Check again.
					uint32_t newKillFiber = (*iDDU)->readKillFiber();
					if ((newKillFiber & 0x7fff) != (xmlKillFiber & 0x7fff)) {
						std::ostringstream error;
						error << "FPGA (" << std::hex << newKillFiber << ") and XML (" << xmlKillFiber << ") killFiber for DDU in crate " << std::dec << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << " disagree after an attempt was made to reload the FPGA.";
						LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
						XCEPT_DECLARE(emu::fed::ConfigurationException, e, error.str());
						notifyQualified("ERROR", e);
					}
				}

				uint16_t fpgaGbEPrescale = (*iDDU)->readGbEPrescale();
				uint16_t xmlGbEPrescale = (*iDDU)->getGbEPrescale();
				
				LOG4CPLUS_DEBUG(getApplicationLogger(), "GbE_Prescale for DDU in crate " << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << ": XML(" << std::hex << xmlGbEPrescale << std::dec << ") FPGA(" << std::hex << fpgaGbEPrescale << std::dec << ")");
				
				if ((fpgaGbEPrescale & 0xf) != xmlGbEPrescale) {
					LOG4CPLUS_INFO(getApplicationLogger(),"FPGA and XML GbEPrescale for DDU in crate " << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << " disagree:  reloading FPGA");
					(*iDDU)->writeGbEPrescale(xmlGbEPrescale);
					
					// Check again.
					uint16_t newGbEPrescale = (*iDDU)->readGbEPrescale();
					if ((newGbEPrescale & 0xf) != (xmlGbEPrescale & 0xf)) {
						std::ostringstream error;
						error << "FPGA (" << std::hex << newGbEPrescale << ") and XML (" << xmlGbEPrescale << ") GbEPrescale for DDU in crate " << std::dec << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << " disagree after an attempt was made to reload the FPGA.";
						LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
						XCEPT_DECLARE(emu::fed::ConfigurationException, e, error.str());
						notifyQualified("ERROR", e);
					}
				}

				// Now we should check if the RUI matches the flash value and
				//  update it as needed.
				uint16_t flashRUI = (*iDDU)->readFlashRUI();
				uint16_t targetRUI = (*iCrate)->getRUI((*iDDU)->slot());

				LOG4CPLUS_DEBUG(getApplicationLogger(),"RUI: flash(" << flashRUI << ") calculated(" << targetRUI << ")");
				
				if (flashRUI != targetRUI) {
					LOG4CPLUS_INFO(getApplicationLogger(),"Flash and calculated RUI for DDU in crate " << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << " disagree:  reloading flash");
					(*iDDU)->writeFlashRUI(targetRUI);
					
					// Check again.
					uint16_t newRUI = (*iDDU)->readFlashRUI();
					if (newRUI != targetRUI) {
						std::ostringstream error;
						error << "Flash (" << std::hex << newRUI << ") and calculated (" << targetRUI << ") RUI for DDU in crate " << std::dec << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << " disagree after an attempt was made to reload the flash.";
						LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
						XCEPT_DECLARE(emu::fed::ConfigurationException, e, error.str());
						notifyQualified("ERROR", e);
					}
				}
				
				// Now check the status registers to see if everything has been configured properly
				LOG4CPLUS_DEBUG(getApplicationLogger(), "Checking configuration for DDU in crate " << (*iCrate)->number() << ", slot " << (*iDDU)->slot());
				
				uint16_t fmmReg = (*iDDU)->readFMM();
				if (fmmReg != (0xFED0)) {
					std::ostringstream error;
					error << "FMM register is wrong.  Got " << std::hex << fmmReg << ", shoud be FED0 for DDU in crate " << std::dec << (*iCrate)->number() << ", slot " << (*iDDU)->slot();
					LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
					XCEPT_DECLARE(emu::fed::ConfigurationException, e, error.str());
					notifyQualified("ERROR", e);
				}
				
				uint32_t CSCStat = (*iDDU)->readCSCStatus();
				uint32_t dduFPGAStat = (*iDDU)->readFPGAStatus(DDUFPGA) & 0xdecfffff;  // <<- note the mask
				uint32_t inFPGA0Stat = (*iDDU)->readFPGAStatus(INFPGA0) & 0xf7eedfff;  // <<- note the mask
				uint32_t inFPGA1Stat = (*iDDU)->readFPGAStatus(INFPGA1) & 0xf7eedfff;  // <<- note the mask
				
				LOG4CPLUS_DEBUG(getApplicationLogger(), "DDU Status for crate " << (*iCrate)->number() << ", slot " << std::dec << (*iDDU)->slot() << ": CSC: " << std::hex << CSCStat << ", DDUFPGA: " << dduFPGAStat << ", INFPGA0: " << inFPGA0Stat << ", INFPGA1:" << inFPGA1Stat << std::dec);
				
				uint16_t liveFibers = ((*iDDU)->readFiberStatus(INFPGA0)&0x000000ff) | (((*iDDU)->readFiberStatus(INFPGA1)&0x000000ff)<<8);
				uint16_t killFiber = ((*iDDU)->readKillFiber() & 0x7fff);
				
				LOG4CPLUS_INFO(getApplicationLogger(), "liveFibers/killFibers for crate " << (*iCrate)->number() << ", slot " << std::dec << (*iDDU)->slot() << ": " << std::hex << liveFibers << "/" << killFiber << std::dec);
				
				uint32_t thisL1A = (*iDDU)->readL1Scaler(DDUFPGA);
				LOG4CPLUS_DEBUG(getApplicationLogger(), "L1A Scalar for crate " << std::dec << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << ": " << thisL1A);
				
				if (inFPGA0Stat) {
					std::ostringstream error;
					error << "Configuration failure for DDU in crate " << std::dec << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << ": INFPGA0 status register (" << std::hex << inFPGA0Stat << std::dec << ")";
					LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
					XCEPT_DECLARE(emu::fed::ConfigurationException, e, error.str());
					notifyQualified("ERROR", e);
				}
				if (inFPGA1Stat) {
					std::ostringstream error;
					error << "Configuration failure for DDU in crate " << std::dec << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << ": INFPGA1 status register (" << std::hex << inFPGA1Stat << std::dec << ")";
					LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
					XCEPT_DECLARE(emu::fed::ConfigurationException, e, error.str());
					notifyQualified("ERROR", e);
				}
				if (dduFPGAStat) {
					std::ostringstream error;
					error << "Configuration failure for DDU in crate " << std::dec << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << ": DDUFPGA status register (" << std::hex << dduFPGAStat << std::dec << ")";
					LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
					XCEPT_DECLARE(emu::fed::ConfigurationException, e, error.str());
					notifyQualified("ERROR", e);
				}
				if (CSCStat) {
					std::ostringstream error;
					error << "Configuration failure for DDU in crate " << std::dec << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << ": CSC status register (" << std::hex << CSCStat << std::dec << ")";
					LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
					XCEPT_DECLARE(emu::fed::ConfigurationException, e, error.str());
					notifyQualified("ERROR", e);
				}
				if (thisL1A) {
					std::ostringstream error;
					error << "Configuration problem for DDU in crate " << std::dec << (*iCrate)->number() << ", slot " << (*iDDU)->slot() << ": L1A register (" << thisL1A << ") not reset";
					LOG4CPLUS_WARN(getApplicationLogger(), error.str());
					XCEPT_DECLARE(emu::fed::ConfigurationException, e, error.str());
					notifyQualified("WARN", e);
				}
				
			} catch (emu::fed::Exception &e) {
				std::ostringstream error;
				error << "Exception in communicating to DDU in crate " << (*iCrate)->number() << ", slot " << (*iDDU)->slot();
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				XCEPT_DECLARE_NESTED(emu::fed::Exception, e2, error.str(), e);
				notifyQualified("FATAL", e2);
				XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e2);
			}
		}
		
		std::vector<DCC *> myDCCs = (*iCrate)->getDCCs();
		for (std::vector<DCC *>::iterator iDCC = myDCCs.begin(); iDCC != myDCCs.end(); iDCC++) {
			
			try {
				uint16_t fpgaFIFOInUse = (*iDCC)->readFIFOInUse();
				uint16_t xmlFIFOInUse = (*iDCC)->getFIFOInUse();
				
				LOG4CPLUS_DEBUG(getApplicationLogger(), "FIFOInUse for DCC in crate " << (*iCrate)->number() << ", slot " << (*iDCC)->slot() << ": XML(" << std::hex << xmlFIFOInUse << std::dec << ") FPGA(" << std::hex << fpgaFIFOInUse << std::dec << ")");
				
				if ((fpgaFIFOInUse & 0x3ff) != xmlFIFOInUse) {
					LOG4CPLUS_INFO(getApplicationLogger(),"FPGA and XML FIFOInUse for DCC in crate " << (*iCrate)->number() << ", slot " << (*iDCC)->slot() << " disagree:  reloading FPGA");
					(*iDCC)->writeFIFOInUse(xmlFIFOInUse);
					
					// Check again.
					uint16_t newFIFOInUse = (*iDCC)->readFIFOInUse();
					if ((newFIFOInUse & 0x3ff) != xmlFIFOInUse) {
						std::ostringstream error;
						error << "FPGA (" << std::hex << newFIFOInUse << ") and XML (" << xmlFIFOInUse << ") FIFOInUse for DCC in crate " << std::dec << (*iCrate)->number() << ", slot " << (*iDCC)->slot() << " disagree after an attempt was made to reload the FPGA.";
						LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
						XCEPT_DECLARE(emu::fed::ConfigurationException, e, error.str());
						notifyQualified("ERROR", e);
					}
				}

			} catch (emu::fed::Exception &e) {
				std::ostringstream error;
				error << "Exception in communicating to DCC in crate " << (*iCrate)->number() << ", slot " << (*iDCC)->slot();
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				XCEPT_DECLARE_NESTED(emu::fed::Exception, e2, error.str(), e);
				notifyQualified("FATAL", e2);
				XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e2);
			}
		}
	}
}



void emu::fed::EmuFCrate::enableAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Received SOAP message: Enable");
	soapLocal_ = false;

	LOG4CPLUS_DEBUG(getApplicationLogger(), "The run number is " << runNumber_.toString());

	// PGK No hard reset or sync reset is coming any time soon, so we should
	//  do it ourselves.
	for (std::vector<FEDCrate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		std::vector<DCC *> dccs = (*iCrate)->getDCCs();
		if (dccs.size() > 0 && (*iCrate)->number() <= 4) {
			LOG4CPLUS_DEBUG(getApplicationLogger(), "RESYNC THROUGH DCC!");
			try {
				dccs[0]->crateResync();
			} catch (emu::fed::Exception &e) {
				std::ostringstream error;
				error << "Resync through DCC in crate " << (*iCrate)->number() << " slot " << dccs[0]->slot() << " has failed";
				LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
				XCEPT_DECLARE_NESTED(emu::fed::Exception, e2, error.str(), e);
				notifyQualified("FATAL", e2);
				XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e2);
			}
		}
	}

	// PGK You have to wipe the thread manager and start over.
	delete TM_;
	TM_ = new IRQThreadManager(endcap_);
	for (unsigned int i=0; i<crateVector_.size(); i++) {
		if (crateVector_[i]->number() > 4) continue;
		TM_->attachCrate(crateVector_[i]);
	}
	// PGK We now have the run number from CSCSV
	try {
		TM_->startThreads(runNumber_);
	} catch (emu::fed::FMMThreadException &e) {
		std::ostringstream error;
		error << "FMM monitoring threads not started";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::Exception, e2, error.str(), e);
		notifyQualified("FATAL", e2);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e2);
	}
}



void emu::fed::EmuFCrate::disableAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Received SOAP message: Disable");
	soapLocal_ = false;

	try {
		TM_->endThreads();
	} catch (emu::fed::FMMThreadException &e) {
		std::ostringstream error;
		error << "Error in stopping FMM monitoring threads";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::Exception, e2, error.str(), e);
		notifyQualified("FATAL", e2);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e2);
	}

}



void emu::fed::EmuFCrate::haltAction(toolbox::Event::Reference event)
throw (toolbox::fsm::exception::Exception)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Received SOAP message: Halt");
	soapConfigured_ = false;
	soapLocal_ = false;

	try {
		TM_->endThreads();
	} catch (emu::fed::FMMThreadException &e) {
		std::ostringstream error;
		error << "Error in stopping FMM monitoring threads";
		LOG4CPLUS_FATAL(getApplicationLogger(), error.str());
		XCEPT_DECLARE_NESTED(emu::fed::Exception, e2, error.str(), e);
		notifyQualified("FATAL", e2);
		XCEPT_RETHROW(toolbox::fsm::exception::Exception, error.str(), e2);
	}

}



// HyperDAQ pages
void emu::fed::EmuFCrate::webDefault(xgi::Input *in, xgi::Output *out)
{

	std::stringstream sTitle;
	sTitle << "EmuFCrate(" << getApplicationDescriptor()->getInstance() << ") " << endcap_.toString();
	std::vector<std::string> jsFileNames;
	jsFileNames.push_back("reload.js");
	jsFileNames.push_back("errorFlasher.js");
	*out << Header(sTitle.str(), jsFileNames);

	// Manual state changing
	*out << cgicc::fieldset()
		.set("class","fieldset") << std::endl;
	*out << cgicc::div("Manual state changes")
		.set("class","legend") << std::endl;

	*out << cgicc::div();
	*out << "Present state: ";
	*out << cgicc::span(state_.toString())
		.set("class",state_.toString()) << std::endl;
	*out << cgicc::div();

	*out << cgicc::div() << std::endl;
	if (!soapConfigured_) {
		*out << cgicc::form()
			.set("style","display: inline;")
			.set("action","/" + getApplicationDescriptor()->getURN() + "/Fire")
			.set("method","GET") << std::endl;
		if (state_.toString() == "Halted" || state_.toString() == "Configured") {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Configure") << std::endl;
		}
		if (state_.toString() == "Configured") {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Enable") << std::endl;
		}
		if (state_.toString() == "Enabled") {
			*out << cgicc::input()
				.set("name","action")
				.set("type","submit")
				.set("value","Disable") << std::endl;
		}
		*out << cgicc::input()
			.set("name","action")
			.set("type","submit")
			.set("value","Halt") << std::endl;
		*out << cgicc::form() << std::endl;

	} else {
		*out << "EmuFCrate has been configured through SOAP." << std::endl;
		*out << cgicc::br() << "Send the Halt signal through SOAP to manually change states." << std::endl;
	}
	*out << cgicc::div() << std::endl;
	*out << cgicc::span("Configuration located at " + xmlFile_.toString())
		.set("style","color: #A00; font-size: 10pt;") << std::endl;

	*out << cgicc::fieldset() << std::endl;

	// HyperDAQ?
	std::set<xdaq::ApplicationDescriptor * > hddescriptors =
		getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuFCrateHyperDAQ");

	if (hddescriptors.size()) {

		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;
		*out << cgicc::div("EmuFCrateHyperDAQ")
			.set("class","legend") << std::endl;

		std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
		for ( itDescriptor = hddescriptors.begin(); itDescriptor != hddescriptors.end(); itDescriptor++ ) {
			if ((*itDescriptor)->getInstance() != getApplicationDescriptor()->getInstance()) continue;
			std::stringstream className;
			className << (*itDescriptor)->getClassName() << "(" << (*itDescriptor)->getInstance() << ")";
			std::stringstream url;
			url << (*itDescriptor)->getContextDescriptor()->getURL() << "/" << (*itDescriptor)->getURN();

			*out << cgicc::a(className.str())
				.set("href",url.str()) << std::endl;

		}

		*out << cgicc::fieldset() << std::endl;
	}

	// IRQ Monitoring
	// Hide the TrackFinder...
	/*
	if (endcap_.toString() != "TrackFinder") {
		*out << cgicc::fieldset()
			.set("class","fieldset") << std::endl;

		if (state_.toString() == "Enabled") {
			*out << cgicc::div("IRQ Monitoring Enabled")
				.set("class","legend") << std::endl;

			for (std::vector<FEDCrate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {

				int crateNumber = (*iCrate)->number();

				// Status table
				*out << cgicc::table()
					.set("style","width: 90%; margin: 10px auto 10px auto; border: solid 2px #009; border-collapse: collapse;") << std::endl;

				*out << cgicc::tr()
					.set("style","background-color: #009; color: #FFF; text-align: center; font-size: 12pt; font-weight: bold;") << std::endl;

				*out << cgicc::td()
					.set("colspan","6") << std::endl;
				*out << "Crate " << crateNumber << std::endl;
				*out << cgicc::td() << std::endl;

				*out << cgicc::tr() << std::endl;

				*out << cgicc::tr()
					.set("style","background-color: #009; color: #FFF; text-align: center; font-size: 10pt; font-weight: bold;") << std::endl;

				*out << cgicc::td()
					.set("colspan","6") << std::endl;

				*out << "Thread started " << TM_->data()->startTime[crateNumber] << cgicc::br();
				*out << TM_->data()->ticks[crateNumber] << " ticks, ";
				*out << "last tick " << TM_->data()->tickTime[crateNumber] << std::endl;
				*out << cgicc::td() << std::endl;
				*out << cgicc::tr() << std::endl;

				*out << cgicc::tr()
					.set("style","background-color: #009; color: #FFF; text-align: center; font-size: 10pt; font-weight: bold; border: solid 1px #000") << std::endl;
				*out << cgicc::td("Time of error") << std::endl;
				*out << cgicc::td("Slot") << std::endl;
				*out << cgicc::td("RUI") << std::endl;
				*out << cgicc::td("Fiber(s)") << std::endl;
				*out << cgicc::td("Chamber(s)") << std::endl;
				*out << cgicc::td("Action taken") << std::endl;
				*out << cgicc::tr() << std::endl;

				std::vector<IRQError *> errorVector = TM_->data()->errorVectors[crateNumber];
				// Print something pretty if there is no error
				if (errorVector.size() == 0) {
					*out << cgicc::tr() << std::endl;
					*out << cgicc::td()
						.set("colspan","6")
						.set("style","border: 1px solid #000;")
						.set("class","undefined") << std::endl;
					*out << "No errors detected (yet)" << std::endl;
					*out << cgicc::td() << std::endl;
					*out << cgicc::tr() << std::endl;
				} else {

					for (std::vector<IRQError *>::reverse_iterator iError = errorVector.rbegin(); iError != errorVector.rend(); iError++) {
						// Mark the error as grey if there has been a reset.
						std::string errorClass = "";
						std::string chamberClass = "error";
						if ((*iError)->reset) {
							errorClass = "undefined";
							chamberClass = "undefined";
						}

						*out << cgicc::tr() << std::endl;
						
						// Time
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << std::endl;
						time_t interruptTime = (*iError)->errorTime;
						struct tm* interruptTimeInfo = localtime(&interruptTime);
						*out << asctime(interruptTimeInfo) << std::endl;
						*out << cgicc::td() << std::endl;

						// Slot
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << std::endl;
						*out << (*iError)->ddu->slot() << std::endl;
						*out << cgicc::td() << std::endl;

						// RUI
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << std::endl;
						*out << (*iCrate)->getRUI((*iError)->ddu->slot()) << std::endl;
						*out << cgicc::td() << std::endl;

						// Fibers
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << std::endl;
						for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
							if ((*iError)->fibers & (1<<iFiber)) *out << iFiber << " ";
						}
						*out << cgicc::td() << std::endl;

						// Chambers
						*out << cgicc::td()
							.set("class",chamberClass)
							.set("style","border: 1px solid #000;") << std::endl;
						for (unsigned int iFiber = 0; iFiber < 16; iFiber++) {
							if ((*iError)->fibers & (1<<iFiber)) {
								if (iFiber == 15) *out << "DDU ";
								else *out << (*iError)->ddu->getChamber(iFiber)->name() << " ";
								// Pointer-foo!
							}
						}
						*out << cgicc::td() << std::endl;

						// Action performed
						*out << cgicc::td()
							.set("class",errorClass)
							.set("style","border: 1px solid #000;") << std::endl;
						*out << (*iError)->action << std::endl;
						*out << cgicc::td() << std::endl;
						
						*out << cgicc::tr() << std::endl;
					}
				}

				*out << cgicc::table() << std::endl;

			}

		} else {
			*out << cgicc::div("IRQ Monitoring Disabled")
				.set("class","legend") << std::endl;
			*out << cgicc::span("Set state to \"Enabled\" to begin IRQ monitoring threads.")
				.set("style","color: #A00; font-size: 11pt;") << std::endl;
		}

		*out << cgicc::fieldset() << std::endl;
	} // End hiding from TrackFinder.
	*/

	*out << Footer() << std::endl;
}



// PGK Ugly, but it must be done.  We have to update the parameters that the
//  EmuFCrateManager asks for or else they won't be updated!
/*
xoap::MessageReference emu::fed::EmuFCrate::onGetParameters(xoap::MessageReference message)
{
	
	if (state_.toString() == "Enabled") {
		// dccInOut update
		dccInOut_.clear();
		errorChambers_ = "";

		// PGK Update the DCC rate info.  This is confusing as all get-out.
		
		// FTW!
		std::ostringstream errorChamberString;
		
		for (std::vector<FEDCrate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
			int crateNumber = (*iCrate)->number();
			if (crateNumber > 4) continue; // Skip TF
			std::vector<DCC *> myDccs = (*iCrate)->getDCCs();
			xdata::Vector<xdata::UnsignedInteger> crateV;
			//std::cout << "Updating getParameters Crate Number " << (*iCrate)->number() << std::endl;
			crateV.push_back((*iCrate)->number());
			std::vector<DCC *>::iterator idcc;
			for (idcc = myDccs.begin(); idcc != myDccs.end(); idcc++) {
				unsigned short int status[6];
				int dr[6];
				for (int igu=0;igu<6;igu++) {
					status[igu]=(*idcc)->readRate(igu);
					dr[igu]=((status[igu]&0x3fff)<<(((status[igu]>>14)&0x3)*4));
				}
				crateV.push_back(dr[0]);
				crateV.push_back(dr[1]);
				crateV.push_back(dr[2]);
				crateV.push_back(dr[3]);
				crateV.push_back(dr[4]);
				crateV.push_back(dr[5]);

				//std::cout << "Slink0: " << dr[0] << std::endl;

				for (int igu=6;igu<12;igu++) {
					status[igu-6]=(*idcc)->readRate(igu);
					dr[igu-6]=((status[igu-6]&0x3fff)<<(((status[igu-6]>>14)&0x3)*4));
				}
				crateV.push_back(dr[0]);
				crateV.push_back(dr[1]);
				crateV.push_back(dr[2]);
				crateV.push_back(dr[3]);
				crateV.push_back(dr[4]);
				crateV.push_back(dr[5]);

				//std::cout << "Slink1: " << dr[0] << std::endl;
			}
			//std::cout << "Pushing back." << std::endl;
			dccInOut_.push_back(crateV);
			
			// Now for the chamber errors from IRQ...

			std::vector<IRQError *> errorVector = TM_->data()->errorVectors[crateNumber];
			for (std::vector<IRQError *>::iterator iError = errorVector.begin(); iError != errorVector.end(); iError++) {
				//LOG4CPLUS_DEBUG(getApplicationLogger(), "I think that there is an error on crate " << (*iCrate)->number() << " slot " << (*iError)->ddu->slot() << " with reset " << (*iError)->reset);
				// Skip things that have already been reset (we think)
				if ((*iError)->reset) continue;
				// Report the chamber names and RUI names that are in an error state.
				for (unsigned int iFiber = 0; iFiber < 16; iFiber++) {
					if ((*iError)->fibers & (1<<iFiber)) {
						//LOG4CPLUS_DEBUG(getApplicationLogger(), "I think that there is an error on crate " << (*iCrate)->number() << " slot " << (*iError)->ddu->slot() << " fiber " << iFiber << " with reset " << (*iError)->reset);
						if (errorChamberString.str() != "") errorChamberString << ", ";
						if (iFiber == 15) {
							errorChamberString << "RUI#" << (*iCrate)->getRUI((*iError)->ddu->slot());
						} else errorChamberString << (*iError)->ddu->getChamber(iFiber)->name() + " ";
					}
				}
			}
		}
		
		// FTW!
		errorChambers_ = errorChamberString.str();
	}
	// PGK Following is just what EmuFEDApplication does.

	xdata::soap::Serializer serializer;

	xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPBody body = envelope.getBody();

	xoap::SOAPName bodyElementName = envelope.createName("data", "xdaq", XDAQ_NS_URI);
	xoap::SOAPBodyElement bodyElement = body.addBodyElement ( bodyElementName );

	//bool first = true;

	std::map<std::string, xdata::Serializable *>::iterator iParam;
	for (iParam = getApplicationInfoSpace()->begin(); iParam != getApplicationInfoSpace()->end(); iParam++) {
		std::string name = (*iParam).first;
		xoap::SOAPName elementName = envelope.createName(name, "xdaq", XDAQ_NS_URI);
		xoap::SOAPElement element = bodyElement.addChildElement(elementName);
		serializer.exportAll((*iParam).second, dynamic_cast<DOMElement*>(element.getDOMNode()), true);
		//first = false;
	}

	//reply->writeTo(std::cout);
	//std::cout << std::endl;
	
	return reply;
	
	//return createReply(message);

}
*/


// Stolen from the now-defunct EmuFController
void emu::fed::EmuFCrate::writeTTSBits(unsigned int crate, unsigned int slot, int bits)
throw (emu::fed::TTSException)
{

	LOG4CPLUS_DEBUG(getApplicationLogger(), "Setting TTS bits on crate " << crate << ", slot " << slot << ", bits " << std::hex << bits << std::dec);
	for (std::vector<FEDCrate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		if ((*iCrate)->number() != crate) continue;
	
		if (slot == 8 || slot == 18) {
			
			std::vector<DCC *> dccVector = (*iCrate)->getDCCs();
			for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); iDCC++) {
				if ((*iDCC)->slot() != slot) continue;
				try {
					(*iDCC)->writeFMM((bits | 0x10) & 0xffff);
				} catch (emu::fed::Exception &e) {
					std::ostringstream error;
					error << "Set TTS bits on DCC in crate " << (*iCrate)->number() << " slot " << (*iDCC)->slot() << " has failed";
					XCEPT_RETHROW(emu::fed::TTSException, error.str(), e);
				}
			}
		} else {
			
			std::vector<DDU *> dduVector = (*iCrate)->getDDUs();
			for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
				if ((*iDDU)->slot() != slot) continue;
				try {
					(*iDDU)->writeFMM((bits | 0xf0e0) & 0xffff);
				} catch (emu::fed::Exception &e) {
					std::ostringstream error;
					error << "Set TTS bits on DDU in crate " << (*iCrate)->number() << " slot " << (*iDDU)->slot() << " has failed";
					XCEPT_RETHROW(emu::fed::Exception, error.str(), e);
				}
			}
		}
	}
}


// Stolen from the now-defunct EmuFController
int emu::fed::EmuFCrate::readTTSBits(unsigned int crate, unsigned int slot)
throw (emu::fed::TTSException)
{
	LOG4CPLUS_DEBUG(getApplicationLogger(), "Reading TTS bits on crate " << crate << ", slot " << slot);
	for (std::vector<FEDCrate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		if ((*iCrate)->number() != crate) continue;
		
		if (slot == 8 || slot == 18) {
			
			std::vector<DCC *> dccVector = (*iCrate)->getDCCs();
			for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); iDCC++) {
				if ((*iDCC)->slot() != slot) continue;
				try {
					return (*iDCC)->readFMM() & 0xf;
				} catch (emu::fed::Exception &e) {
					std::ostringstream error;
					error << "Set TTS bits on DCC in crate " << (*iCrate)->number() << " slot " << (*iDCC)->slot() << " has failed";
					XCEPT_RETHROW(emu::fed::Exception, error.str(), e);
				}
			}
		} else {
			
			std::vector<DDU *> dduVector = (*iCrate)->getDDUs();
			for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
				if ((*iDDU)->slot() != slot) continue;
				try {
					return (*iDDU)->readFMM() & 0xf;
				} catch (emu::fed::Exception &e) {
					std::ostringstream error;
					error << "Set TTS bits on DDU in crate " << (*iCrate)->number() << " slot " << (*iDDU)->slot() << " has failed";
					XCEPT_RETHROW(emu::fed::Exception, error.str(), e);
				}
			}
		}
	}

	return 0;
}

// End of file
// vim: set sw=4 ts=4: