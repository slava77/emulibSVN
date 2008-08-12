#include "DCCDebugger.h"

using namespace std;


DCCDebugger::DCCDebugger()
{
}



DCCDebugger::~DCCDebugger()
{
}



std::map<string, string> DCCDebugger::FMMStat(short int stat)
{
	std::map<string, string> returnValues;

	if (0xf&stat == 0x3) returnValues["DCC Error"] = "error";
	else if (0x1&stat) returnValues["DCC Busy"] = "yellow";
	else if (0x2&stat) returnValues["DCC Ready"] = "green";
	else if (0x4&stat) returnValues["DCC Warning"] = "orange";
	else if (0x8&stat) returnValues["DCC Out-of-Sync"] = "error";

	return returnValues;
}



std::map<string, string> DCCDebugger::SLinkStat(short int stat)
{
	std::map<string, string> returnValues;
	for (int iLink = 0; iLink < 2; iLink++) {
		if (stat & (1 << (iLink*2))) {
			ostringstream linkStat;
			linkStat << "S-Link " << (iLink+1) << " active";
			returnValues[linkStat.str()] = "green";
		} else {
			ostringstream linkStat;
			linkStat << "S-Link " << (iLink+1) << " inactive";
			returnValues[linkStat.str()] = "red";
		}

		if (!(stat & (1 << (iLink*2 + 1)))) {
			ostringstream linkStat;
			linkStat << "S-Link " << (iLink+1) << " backpressure";
			returnValues[linkStat.str()] = "orange";
		}
	}

	return returnValues;
}

