/*****************************************************************************\
* $Id: DCCDebugger.cc,v 1.6 2008/10/09 11:21:19 paste Exp $
*
* $Log: DCCDebugger.cc,v $
* Revision 1.6  2008/10/09 11:21:19  paste
* Attempt to fix DCC MPROM load.  Added debugging for "Global SOAP death" bug.  Changed the debugging interpretation of certain DCC registers.  Added inline SVG to EmuFCrateManager page for future GUI use.
*
* Revision 1.5  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "DCCDebugger.h"

#include <iostream>
#include <iomanip>
#include <sstream>



std::map<std::string, std::string> emu::fed::DCCDebugger::FMMStat(short int stat)
{
	std::map<std::string, std::string> returnValues;

	if (0xf&stat == 0x3) returnValues["DCC Error"] = "error";
	else if (0x1&stat) returnValues["DCC Busy"] = "yellow";
	else if (0x2&stat) returnValues["DCC Ready"] = "green";
	else if (0x4&stat) returnValues["DCC Warning"] = "orange";
	else if (0x8&stat) returnValues["DCC Out-of-Sync"] = "error";

	return returnValues;
}



std::map<std::string, std::string> emu::fed::DCCDebugger::SLinkStat(short int stat)
{
	std::map<std::string, std::string> returnValues;
	for (int iLink = 0; iLink < 2; iLink++) {
		if (stat & (1 << (iLink*2 + 1))) {
			std::stringstream linkStat;
			linkStat << "S-Link " << (iLink+1) << " active";
			returnValues[linkStat.str()] = "green";
		} else {
			std::stringstream linkStat;
			linkStat << "S-Link " << (iLink+1) << " inactive";
			returnValues[linkStat.str()] = "red";
		}

		if (!(stat & (1 << (iLink*2)))) {
			std::stringstream linkStat;
			linkStat << "S-Link " << (iLink+1) << " backpressure";
			returnValues[linkStat.str()] = "orange";
		}
	}

	return returnValues;
}



std::map<std::string, std::string> emu::fed::DCCDebugger::InFIFOStat(short int stat) {
	std::map<std::string, std::string> returnValues;
	for (unsigned int iBit = 0; iBit < 3; iBit++) {
		if (!(stat & (1 << iBit))) {
			std::stringstream bitStat;
			bitStat << "InFIFO " << (iBit*2 + 1) << "/" << (iBit*2 + 2) << " half full";
			returnValues[bitStat.str()] = "orange";
		}
	}
	for (unsigned int iBit = 3; iBit < 8; iBit++) {
		if (!(stat & (1 << iBit))) {
			std::stringstream bitStat;
			bitStat << "InFIFO " << ((iBit - 3)*2 + 1) << "/" << ((iBit - 3)*2 + 2) << " almost full";
			returnValues[bitStat.str()] = "red";
		}
	}
	
	return returnValues;
}


