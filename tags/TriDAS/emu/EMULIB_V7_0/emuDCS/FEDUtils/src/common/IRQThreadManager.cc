/*****************************************************************************\
* $Id: IRQThreadManager.cc,v 1.2 2009/03/24 19:13:44 paste Exp $
*
* $Log: IRQThreadManager.cc,v $
* Revision 1.2  2009/03/24 19:13:44  paste
* Fixed crashing when ending threads after an IRQ
* Made threads more robust by using slot numbers instead of DDU pointers as map indices
*
* Revision 1.1  2009/03/05 16:07:52  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
* Revision 3.34  2009/01/30 19:14:16  paste
* New emu::base namespace and emu::base::Supervised inheritance added.
*
* Revision 3.33  2009/01/29 15:31:24  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.32  2008/11/14 09:34:31  paste
* Updated IRQ thread handling to fix and abstract FMM enabling and disabling.
*
* Revision 3.31  2008/10/30 12:56:11  paste
* Fixing more map-related bugs in IRQData
* Changing IRQ FMM threshold to > 8 chambers (from > 1 chamber)
*
* Revision 3.30  2008/10/22 20:23:58  paste
* Fixes for random FED software crashes attempted.  DCC communication and display reverted to ancient (pointer-based communication) version at the request of Jianhui.
*
* Revision 3.29  2008/10/15 00:46:56  paste
* Attempt to solve certain crashes on Enable/Disable commands.
*
* Revision 3.28  2008/10/01 14:10:04  paste
* Fixed phantom reset bug in IRQ threads and shifted IRQ handling functions to VMEController object.
*
* Revision 3.27  2008/09/30 21:45:00  paste
* Fixed a bug where single errors could be mistaken as multiple IRQs.
*
* Revision 3.26  2008/09/30 09:17:14  paste
* Removed debugging statements in IRQThreadManager
*
* Revision 3.25  2008/09/30 08:32:40  paste
* Updated IRQ Threads so that the endcap name is mentioned in the log filename
*
* Revision 3.24  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 3.23  2008/09/22 14:31:54  paste
* /tmp/cvsY7EjxV
*
* Revision 3.22  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.21  2008/09/03 17:52:59  paste
* Rebuilt the VMEController and VMEModule classes from the EMULIB_V6_4 tagged versions and backported important changes in attempt to fix "high-bits" bug.
*
* Revision 3.20  2008/09/01 11:30:32  paste
* Added features to DDU, IRQThreads corresponding to new DDU firmware.
*
* Revision 3.19  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.18  2008/08/15 16:14:51  paste
* Fixed threads (hopefully).
*
* Revision 3.17  2008/08/15 09:59:22  paste
* Fixed bug where transitions to Halted state while threads were not active caused a crash.
*
* Revision 3.16  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "emu/fed/IRQThreadManager.h"

#include <utility>
#include <string>
#include <iostream>
#include <iomanip>
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
//#include <errno.h>

#include "emu/fed/Crate.h"
#include "emu/fed/VMEController.h"
#include "emu/fed/DDU.h"
#include "emu/fed/DDUDebugger.h"
#include "emu/fed/Chamber.h"
#include "emu/fed/IRQData.h"
#include "CAENVMElib.h"
#include "CAENVMEtypes.h"



emu::fed::IRQThreadManager::IRQThreadManager(std::string myEndcap = ""):
endcap_(myEndcap)
{
	data_ = new IRQData();
	threadVector_.clear();
}



emu::fed::IRQThreadManager::~IRQThreadManager()
{

	//endThreads();

}



void emu::fed::IRQThreadManager::attachCrate(Crate *crate)
{

	pthread_t threadID = 0;
	threadVector_.push_back(std::pair<Crate *, pthread_t>(crate, threadID));

}




void emu::fed::IRQThreadManager::startThreads(unsigned long int runNumber)
throw (emu::fed::exception::FMMThreadException)
{

	//std::cout << "emu::fed::IRQThreadManager::startThreads Create unique Logger for EmuFEDVME" << std::endl;

	// Make the shared data object that will be passed between threads and the
	// mother program.
	data_ = new IRQData();

	char datebuf[32];
	//char filebuf[255];
	std::stringstream fileName;
	time_t theTime = time(NULL);

	// log file format: EmuFMMThread_(EndcapName_)YYYYMMDD-hhmmss_rRUNNUMBER.log
	strftime(datebuf, sizeof(datebuf), "%Y%m%d-%H%M%S", localtime(&theTime));
	fileName << "EmuFMMThread_" << (endcap_ != "" ? endcap_ + "_" : "") << datebuf << "_r" << std::setw(5) << std::setfill('0') << std::dec << runNumber;
	//sprintf(filebuf,"EmuFMMThread_%s_r%05u.log",datebuf,(unsigned int) runNumber);

	log4cplus::SharedAppenderPtr myAppend = new log4cplus::FileAppender(fileName.str().c_str());
	myAppend->setName("EmuFMMIRQAppender");

	//Appender Layout
	std::auto_ptr<log4cplus::Layout> myLayout = std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%D{%m/%d/%Y %j-%H:%M:%S.%q} %-5p %c, %m%n"));
	// for date code, use the Year %Y, DayOfYear %j and Hour:Min:Sec.mSec
	// only need error data from Log lines with "ErrorData" tag
	myAppend->setLayout( myLayout );

	log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFMMIRQ");
	logger.addAppender(myAppend);

	//std::cout << "emu::fed::IRQThreadManager::startThreads Clearing shared data" << std::endl;
	
	data_->runNumber = runNumber;
	// Do not quit the threads immediately.
	data_->exit = false;

	// First, load up the data_ object with the crates that I govern.
	for (unsigned int iThread = 0; iThread < threadVector_.size(); iThread++) {
		data_->crateQueue.push(threadVector_[iThread].first);
	}
	
	// Next, execute the threads.
	
	// Check the crates to see if any of the DDUs are in an error state and
	//  make a note in the log.  This is because DDUs that are already throwing
	//  errors will not set interrupts, so we want to notify the user somehow
	//  about the possible problem.
	for (unsigned int iThread = 0; iThread < threadVector_.size(); iThread++) {
		
		//std::cout << "emu::fed::IRQThreadManager::startThread Starting thread for crate number " << threadVector_[i].first->number() << std::endl;
		emu::fed::Crate *myCrate = threadVector_[iThread].first;
		std::vector<emu::fed::DDU *> dduVector = myCrate->getDDUs();
		for (std::vector<emu::fed::DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
			if ((*iDDU)->slot() >= 21) continue;
			unsigned int cscStatus;
			try {
				cscStatus = (*iDDU)->readCSCStatus();
			} catch (emu::fed::exception::DDUException &e) {
				std::ostringstream error;
				error << "Exception in communicating to DDU in crate " << myCrate->number() << ", slot " << (*iDDU)->slot();
				LOG4CPLUS_FATAL(logger, error.str());
				XCEPT_RETHROW(emu::fed::exception::FMMThreadException, error.str(), e);
			}
			if (cscStatus) {
				std::string chambers;
				for (unsigned int iFiber = 0; iFiber < 16; iFiber++) {
					if (cscStatus & (1 << iFiber)) {
						if (iFiber == 15) chambers += "DDU ";
						else chambers += (*iDDU)->getChamber(iFiber)->name() + " ";
					}
				}
				LOG4CPLUS_WARN(logger, "Crate " << myCrate->number() << " Slot " << (*iDDU)->slot() << " shows errors in " << chambers << " before the threads actually started.  These may not show up as interrupts!");
			}
		}

		int err = pthread_create(&(threadVector_[iThread].second), NULL, IRQThread, data_);
		if (err) {
			std::ostringstream error;
			error << "Exception in starting IRQThread for crate " << myCrate->number() << ": " << err;
			LOG4CPLUS_FATAL(logger, error.str());
			XCEPT_RAISE(emu::fed::exception::FMMThreadException, error.str());
		}
	}
}



void emu::fed::IRQThreadManager::endThreads()
throw (emu::fed::exception::FMMThreadException)
{

	log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFMMIRQ");
	
	if (data_->exit || threadVector_.size() == 0) {
		//LOG4CPLUS_DEBUG(logger,"Threads already stopped.");
		return;
	}
	//LOG4CPLUS_DEBUG(logger,"Gracefully killing off all threads.");
		
	data_->exit = true;
		
		
	// We probably do not need to return the status of the threads,
	//  but this may be used later for whatever reason.
	std::vector<int> returnStatus;
		
	// The threads should be stopping now.  Let's join them.
	for (unsigned int iThread=0; iThread < threadVector_.size(); iThread++) {
		void *tempException = NULL; // I have to do this for type safety.
		int err = pthread_join(threadVector_[iThread].second, &tempException); // Waits until the thread calls pthread_exit(void *return_status)
			
		if (err) {
			std::ostringstream error;
			error << "Error in joining IRQThread " << iThread << " for crate " << threadVector_[iThread].first->number() << ": " << err;
			LOG4CPLUS_FATAL(logger, error.str());
			XCEPT_RAISE(emu::fed::exception::FMMThreadException, error.str());
		}

		// Note:  tempException points to a pointer of a value that
		//  the pthread returned, while error is the error
		//  status of the join routine itself.
			
		if (tempException != NULL) {
			std::ostringstream error;
			error << "Exception in joining IRQThread " << iThread << " for crate " << threadVector_[iThread].first->number();
			LOG4CPLUS_FATAL(logger, error.str());
			XCEPT_RAISE(emu::fed::exception::FMMThreadException, error.str());
		}
			
		LOG4CPLUS_INFO(logger, "Joined IRQThread " << iThread << " for crate " << threadVector_[iThread].first->number());

	}
		
	delete data_;
	data_ = new IRQData();
	threadVector_.clear();
}



/// The big one
void *emu::fed::IRQThreadManager::IRQThread(void *data)
{

	// Recast the void pointer as something more useful.
	IRQData *locdata = (IRQData *)data;

	// Grab the crate that I will be working with (and pop off the crate so
	//  that I am the only one working with this particular crate.)
	// Use mutexes to serialize
	pthread_mutex_lock(&(locdata->crateQueueMutex));
	Crate *myCrate = locdata->crateQueue.front();
	locdata->crateQueue.pop();
	pthread_mutex_unlock(&(locdata->crateQueueMutex));

	unsigned int crateNumber = myCrate->number();
	
	log4cplus::Logger logger = log4cplus::Logger::getInstance("EmuFMMIRQ");

	// Knowing what DDUs we are talking to is useful as well.
	std::vector<DDU *> dduVector = myCrate->getDDUs();

	// This is when we started.  Don't know why this screws up sometimes...
	time_t tock;
	time(&tock);
	tm *tockInfo = localtime(&tock);
	std::string tockText(asctime(tockInfo));
	locdata->startTime[crateNumber] = tockText;

	// A local tally of what the last error on a given DDU was.
	std::map<unsigned int, int> lastError;
	
	// Continue unless someone tells us to stop.
	while (locdata->exit == false) {

		//LOG4CPLUS_DEBUG(logger, "Start of loop reached.");

		// Increase the ticks.
		locdata->ticks[crateNumber]++;

		// Set the time of the last tick.
		time_t tick;
		time(&tick);
		tm *tickInfo = localtime(&tick);
		std::string tickText(asctime(tickInfo));
		locdata->tickTime[crateNumber] = tickText;
		
		// Enable the IRQ and wait for something to happen for 5 seconds...
		bool allClear;
		try {
			allClear = myCrate->getController()->waitIRQ(5000);
		} catch (emu::fed::exception::CAENException &e) {
			std::ostringstream error;
			error << "Exception waiting for IRQ in crate number " << myCrate->number();
			XCEPT_DECLARE_NESTED(emu::fed::exception::FMMThreadException, e2, error.str(), e);
			LOG4CPLUS_FATAL(logger, error.str());
			pthread_exit((void *) &e2);
		}
		

		// If allClear is non-zero, then there was not an error.
		// If there was no error, check to see if we were in an error state
		//  before...
		if (allClear && locdata->errorCount[crateNumber] > 0) {

			// Get the last DDU to report an error
			DDU *myDDU = NULL;
			for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
				if ((*iDDU)->slot() == locdata->lastDDU[crateNumber]) {
					myDDU = (*iDDU);
					break;
				}
			}

			// If my status has cleared, then all is cool, right?
			//  Reset all my data.
			try {
				if (myDDU == NULL || (myDDU->readCSCStatus() | myDDU->readAdvancedFiberErrors()) < lastError[myDDU->slot()]) {
					LOG4CPLUS_INFO(logger, "Reset detected on crate " << crateNumber << ": checking again to make sure...");
					usleep(100);
					
					if (myDDU == NULL || (myDDU->readCSCStatus() | myDDU->readAdvancedFiberErrors()) < lastError[myDDU->slot()]) {
						LOG4CPLUS_INFO(logger, "Reset confirmed on crate " << crateNumber);
						LOG4CPLUS_ERROR(logger, " ErrorData RESET Detected" << std::endl);

						// Increment the reset count on all the errors from that crate...
						std::vector<IRQError *> myErrors = locdata->errorVectors[crateNumber];
						for (std::vector<IRQError *>::iterator iError = myErrors.begin(); iError != myErrors.end(); iError++) {
							(*iError)->reset++;
						}

						// Reset the total error count and the saved errors.
						locdata->errorCount[crateNumber] = 0;
						locdata->lastDDU[crateNumber] = 0;
						lastError.clear();
					} else {
						
						LOG4CPLUS_INFO(logger, "No reset.  Continuing as normal.");
						
					}

				}
			} catch (emu::fed::exception::Exception &e) {
				std::ostringstream error;
				error << "Exception reading last DDU status for crate number " << myCrate->number();
				XCEPT_DECLARE_NESTED(emu::fed::exception::FMMThreadException, e2, error.str(), e);
				LOG4CPLUS_FATAL(logger, error.str());
				pthread_exit((void *) &e2);
			}

			continue;
			
		}
		
		// If there was no error, and there was no previous error (or the
		//  previous error was not cleared), then do nothing.
		else if (allClear) continue;

		// We have an error!

		//CVDataWidth DW=cvD16;
		//CVIRQLevels IRQLevel=cvIRQ1;
		//unsigned int ERR,SYNC,FMM,NUM_ERR,NUM_SYNC,SLOT;

		// Read out the error information into a local variable.
		uint16_t errorData;
		try {
			errorData = myCrate->getController()->readIRQ();
		} catch (emu::fed::exception::CAENException &e) {
			std::ostringstream error;
			error << "Exception reading IRQ in crate number " << myCrate->number();
			XCEPT_DECLARE_NESTED(emu::fed::exception::FMMThreadException, e2, error.str(), e);
			LOG4CPLUS_FATAL(logger, error.str());
			pthread_exit((void *) &e2);
		}

		// In which slot did the error occur?  Get the DDU that matches.
		DDU *myDDU = NULL;
		for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
			if ((*iDDU)->slot() == (unsigned int) ((errorData & 0x1f00) >> 8)) {
				locdata->lastDDU[crateNumber] = (*iDDU)->slot();
				myDDU = (*iDDU);
				break;
			}
		}

		// Problem if there is no matching DDU...
		if (myDDU == NULL) {
			// Looks like this happens all the time.  Squelch errors.
			//LOG4CPLUS_FATAL(logger, "IRQ set from an unrecognized slot!  Crate " << myCrate->number() << " slot " << std::dec << (errorData[1] & 0x1f) << " error data " << std::hex << std::setw(2) << std::setfill('0') << (int) errorData[1] << std::setw(2) << std::setfill('0') << (int) errorData[0]);
			continue;
		}

		// Collect the present CSC status and store...
		try {
			unsigned int cscStatus = myDDU->readCSCStatus();
			unsigned int advStatus = myDDU->readAdvancedFiberErrors();
			unsigned int xorStatus = (cscStatus | advStatus)^lastError[myDDU->slot()];
			
			// What type of error did I see?
			bool hardError = (errorData & 0x8000);
			bool syncError = (errorData & 0x4000);

			// If the DDU wants a reset, it will request it (basically an OR of
			//  the two above values.)
			bool resetWanted = (errorData & 0x2000);

			// How many CSCs are in an error state on the given DDU?
			unsigned int cscsWithHardError = ((errorData >> 4) & 0x000f);

			// How many CSCs are in a bad sync state on the given DDU?
			unsigned int cscsWithSyncError = (errorData & 0x000f);
			
			// Log everything now.
			LOG4CPLUS_ERROR(logger, "Interrupt detected!");
			time_t theTime = time(NULL);
			LOG4CPLUS_ERROR(logger, " ErrorData " << std::dec << crateNumber << " " << myDDU->slot() << " " << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << cscStatus << " " << std::dec << (uintmax_t) theTime);

			std::stringstream fiberErrors, chamberErrors;
			for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
				if (xorStatus & (1<<iFiber)) {
					fiberErrors << iFiber << " ";
					chamberErrors << myDDU->getChamber(iFiber)->name() << " ";
				}
			}

			LOG4CPLUS_INFO(logger, "Decoded information follows" << std::endl
				<< "FEDCrate   : " << crateNumber << std::endl
				<< "Slot       : " << myDDU->slot() << std::endl
				<< "RUI        : " << myCrate->getRUI(myDDU->slot()) << std::endl
				<< "CSC Status : " << std::hex << cscStatus << std::endl
				<< "ADV Status : " << advStatus << std::endl
				<< "XOR Status : " << xorStatus << std::dec << std::endl
				<< "DDU error  : " << ((cscStatus & 0x8000) == 0x8000) << std::endl
				<< "Fibers     : " << fiberErrors.str() << std::endl
				<< "Chambers   : " << chamberErrors.str() << std::endl
				<< "Hard Error : " << hardError << std::endl
				<< "Sync Error : " << syncError << std::endl
				<< "Wants Reset: " << resetWanted);
			
			LOG4CPLUS_INFO(logger, cscsWithHardError << " CSCs on this DDU have hard errors");
			LOG4CPLUS_INFO(logger, cscsWithSyncError << " CSCs on this DDU have sync errors");
			
			if (!xorStatus) {
				LOG4CPLUS_INFO(logger, "No CSC or DDU errors detected...  Ignoring interrupt");
				continue;
			}

			std::vector<std::string> trapInfo = DDUDebugger::DDUDebugTrap(myDDU->readDebugTrap(DDUFPGA), myDDU);
			std::ostringstream trapStream;
			for (std::vector<std::string>::iterator iTrap = trapInfo.begin(); iTrap != trapInfo.end(); iTrap++) {
				trapStream << (*iTrap) << std::endl;
			}
			
			LOG4CPLUS_INFO(logger, "Logging DDUFPGA diagnostic trap information:" << std::endl << trapStream.str());

			trapInfo = DDUDebugger::INFPGADebugTrap(myDDU->readDebugTrap(INFPGA0), INFPGA0);
			trapStream.str("");
			for (std::vector<std::string>::iterator iTrap = trapInfo.begin(); iTrap != trapInfo.end(); iTrap++) {
				trapStream << (*iTrap) << std::endl;
			}

			LOG4CPLUS_INFO(logger, "Logging INFPGA0 diagnostic trap information:" << std::endl << trapStream.str());

			trapInfo = DDUDebugger::INFPGADebugTrap(myDDU->readDebugTrap(INFPGA1), INFPGA1);
			trapStream.str("");
			for (std::vector<std::string>::iterator iTrap = trapInfo.begin(); iTrap != trapInfo.end(); iTrap++) {
				trapStream << (*iTrap) << std::endl;
			}
			
			LOG4CPLUS_INFO(logger, "Logging INFPGA1 diagnostic trap information:" << std::endl << trapStream.str());

			// Record the error in an accessable history of errors.
			lastError[myDDU->slot()] = (cscStatus | advStatus);
			IRQError *myError = new IRQError(myCrate, myDDU);
			myError->fibers = xorStatus;
			
			// Log all errors in persisting array...
			// PGK I am not so worried about DDU-only errors...
			for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
				if (xorStatus & (1<<iFiber)) {
					locdata->errorCount[crateNumber]++;
				}
			}
			// Just in case there is some bizarre error at this point that causes this to
			// overflow...
			if (locdata->errorCount[crateNumber] > 15) {
				locdata->errorCount[crateNumber] = 15;
			}
			locdata->lastDDU[crateNumber] = myDDU->slot();


			// Check to see if any of the fibers are troublesome and report
			std::vector<IRQError *> errorVector = locdata->errorVectors[crateNumber];
			unsigned int liveFibers = myDDU->readKillFiber();
			//LOG4CPLUS_DEBUG(logger, "Checking for problem fibers in crate " << myCrate->number() << " slot " << myDDU->slot());
			for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
				// Skip it if it is already killed or if it didn't cause a problem
				if (!(liveFibers & (1<<iFiber)) || !(xorStatus & (1<<iFiber))) {
					//LOG4CPLUS_DEBUG(logger, "Fiber is either killed (killFiber " << std::hex << liveFibers << ") or did not cause a problem (xorStatus " << std::hex << xorStatus << ")");
					continue;
				}
				// Look through the history of problem fibers and count them
				unsigned long int problemCount = 0;
				for (std::vector<IRQError *>::iterator iError = errorVector.begin(); iError != errorVector.end(); iError++) {
					// Make sure it's the correct DDU
					if ((*iError)->ddu != myDDU) {
						//LOG4CPLUS_DEBUG(logger, "This error had DDU " << (*iError)->ddu << " and mine is " << myDDU);
						continue;
					}
					if ((*iError)->fibers & (1<<iFiber)) {
						//LOG4CPLUS_DEBUG(logger, "Problem detected, error fibers were " << (*iError)->fibers);
						problemCount++;
					}
				}
				// If the threshold has been reached, Warn (no death yet)
				if (problemCount >= 3) {
					LOG4CPLUS_INFO(logger, "Fiber " << iFiber << " in crate " << crateNumber << " slot " << myDDU->slot() << " (RUI " << myCrate->getRUI(myDDU->slot()) << ", chamber " << myDDU->getChamber(iFiber)->name() << ") has set an error " << problemCount << " times.  Please check this chamber for harware problems.");
					// Forgot this last time...  oops.
					//liveFibers &= ~(1<<iFiber); // Bit-foo!
					//myDDU->ddu_loadkillfiber(liveFibers);
					// Record the action taken.
					std::stringstream actionTaken;
					actionTaken << "Fiber " << iFiber << " (" << myDDU->getChamber(iFiber)->name() << ") has had " << problemCount << " errors since the last hard reset.  Check for hardware problems. ";
					myError->action += actionTaken.str();
				}
			}
			
			// Discover the error counts of the other crates.
			unsigned long int totalChamberErrors = 0;
			for (std::map<unsigned int, unsigned long int>::iterator iCount = locdata->errorCount.begin(); iCount != locdata->errorCount.end(); iCount++) {
				if (iCount->first != crateNumber) {
					LOG4CPLUS_INFO(logger,"Crate " << iCount->first << " reports " << iCount->second << " CSCs in an error state.");
				}
				totalChamberErrors += iCount->second;
			}
			
			// Check if we have sufficient error conditions to reset.
			if (totalChamberErrors > 8) {
				LOG4CPLUS_INFO(logger, "A resync will be requested because the total number of CSCs in an error state on this endcap is " << totalChamberErrors);
				// Make a note of it in the error log.
				std::stringstream actionTaken;
				actionTaken << "A resync has been requested for this endcap. ";
				myError->action += actionTaken.str();
				// Loop over the crates and take away SFTU
				/*
				for (std::map<unsigned int, unsigned long int>::iterator iCount = locdata->errorCount.begin(); iCount != locdata->errorCount.end(); iCount++) {
					// Find the broadcast slot on this crate.
					iCount->first->getBroadcastDDU()->writeFMM(0xFED0);
				}
				*/
				// I only have to do this to my crate:  eventually, a reset will come.
				myCrate->getBroadcastDDU()->enableFMM();
			}

			// Save the error.
			locdata->errorVectors[crateNumber].push_back(myError);
			
			//LOG4CPLUS_DEBUG(logger, "End of loop reached.");
		} catch (emu::fed::exception::Exception &e) {
			std::ostringstream error;
			error << "Exception dealing with IRQ";
			XCEPT_DECLARE_NESTED(emu::fed::exception::FMMThreadException, e2, error.str(), e);
			LOG4CPLUS_FATAL(logger, error.str());
			pthread_exit((void *) &e2);
		}
	}
	pthread_exit(NULL);
}


