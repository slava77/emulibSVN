#include "DCCParser.h"

#include <iostream>

#include "DCC.h"

emu::fed::DCCParser::DCCParser(xercesc::DOMNode * pNode, int crate)
{
	parseNode(pNode);
	
	int slot = 0;
	fillInt("slot", slot);
	if(slot == 0) {
		std::cerr << "No slot specified for DCC! " << std::endl;
	} else { 
		dcc_ = new DCC(slot);
		fillHex("fifoinuse", dcc_->fifoinuse_);
		fillHex("softwareswitch", dcc_->softsw_);
	}
}



























