//#define CAEN_DEBUG 1
/*****************************************************************************\
* $Id: VMEController.cc,v 1.10 2009/11/22 22:39:44 paste Exp $
\*****************************************************************************/
#include "emu/fed/VMEController.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

#include "CAENVMElib.h"
#include "CAENVMEtypes.h"
#include "emu/fed/VMEModule.h"
#include "emu/fed/VMELock.h"

emu::fed::VMEController::VMEController(const int &Device, const int &Link)
throw (emu::fed::exception::CAENException):
Device_(Device),
Link_(Link),
BHandle_(-1)
{

	CVBoardTypes VMEBoard = cvV2718;

	int32_t BHandle;

	CVErrorCodes err = CAENVME_Init(VMEBoard, Device_, Link_, &BHandle);
	
	// The file that will hold the BHandle.  It might already exist and have a valid handle, but we will see later.
	std::ostringstream fileName;
	fileName << "/tmp/CAEN_" << Device_ << "_" << Link_ << ".BHandle";

	// Check to see if the board has been initialized.
	if (err == cvGenericError) {
		// If this failed, then maybe some other process has already opened the device.
		// There should be a file that has the BHandle in it.
		std::ifstream inFile(fileName.str().c_str());
		if (inFile.is_open()) {
			inFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
			try {
				inFile >> BHandle_;
			} catch (std::ifstream::failure &e) {
				inFile.close();
				std::ostringstream error;
				error << "Failure extracting pre-opened BHandle from file " << fileName.str();
				XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
				throw e2;
			}
			inFile.close();
		} else {
			std::ostringstream error;
			error << "Encountered CAEN bus error, but no open BHandles detected in file " << fileName;
			XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
			throw e2;
		}

	} else if (err != cvSuccess) {
		std::ostringstream error;
		error << "error " << err << ": " << CAENVME_DecodeError(err);
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		throw e2;

	} else {
		BHandle_ = BHandle;

		// Now that you own the BHandle, make a file that shows this
		std::ofstream outFile(fileName.str().c_str(), std::ios_base::trunc);
		if (outFile.is_open()) {
			outFile << BHandle_;
			outFile.close();
			
			chmod(fileName.str().c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		} else {
			std::ostringstream error;
			error << "Unable to save BHandle information to external file " << fileName.str();
			XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
			throw e2;
		}
	}
	
	// I can use the device and link numbers to make a unique lock file name for this controller on this manchine.
	std::ostringstream lockName;
	lockName << "/tmp/CAEN_" << Device_ << "_" << Link_ << ".lock";
	try {
		mutex_ = new VMELock(lockName.str());
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Unable to create mutex: " << e.what();
		XCEPT_DECLARE_NESTED(emu::fed::exception::CAENException, e2, error.str(), e);
		throw e2;
	}

}



emu::fed::VMEController::~VMEController() {
	// This will free the resources in the driver, but that is not what we want to do, because
	// multiple processes might be talking to the same controller.
	//CAENVME_End(BHandle_);
}



bool emu::fed::VMEController::waitIRQ(const unsigned int &mSecs)
throw (emu::fed::exception::CAENException)
{
	// If the BHandle is not set properly, just return a good signal (true)
	if (BHandle_ < 0) return true;

	CVErrorCodes err = CAENVME_IRQEnable(BHandle_, cvIRQ1);

	if (err != cvSuccess) {
		std::ostringstream error;
		error << "Exception in waitIRQ: " << CAENVME_DecodeError(err);
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		throw e2;
	}

	const bool status = CAENVME_IRQWait(BHandle_, cvIRQ1, mSecs);
	return status;
}



uint16_t emu::fed::VMEController::readIRQ()
throw (emu::fed::exception::CAENException)
{
	// If the BHandle is not set properly, return nothing
	if (BHandle_ < 0) return 0;

	uint16_t errorData;

	CVErrorCodes err = CAENVME_IACKCycle(BHandle_, cvIRQ1, &errorData, cvD16);

	if (err != cvSuccess) {
		std::ostringstream error;
		error << "Exception in readIRQ: " << CAENVME_DecodeError(err);
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		throw e2;
	}

	return errorData;
}

