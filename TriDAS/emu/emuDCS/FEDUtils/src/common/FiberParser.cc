/*****************************************************************************\
* $Id: FiberParser.cc,v 1.6 2009/12/10 16:30:04 paste Exp $
\*****************************************************************************/
#include "emu/fed/FiberParser.h"

#include <sstream>

#include "emu/fed/Fiber.h"


emu::fed::Fiber *emu::fed::FiberParser::parse(xercesc::DOMElement *pNode)
throw (emu::fed::exception::ParseException)
{
	Parser parser(pNode);
	
	unsigned int fiberNumber = 0;
	try {
		fiberNumber = parser.extract<unsigned int>("FIBER_NUMBER");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse FIBER_NUMBER from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}
	
	bool killed;
	try {
		killed = parser.extract<bool>("KILLED");
	} catch (emu::fed::exception::ParseException &e) {
		std::ostringstream error;
		error << "Unable to parse KILLED from element";
		XCEPT_RETHROW(emu::fed::exception::ParseException, error.str(), e);
	}

	// This is optional
	std::string chamberName = "+0/0/00";
	try {
		chamberName = parser.extract<std::string>("CHAMBER");
	} catch (emu::fed::exception::ParseException &e) {
		// already set to the default.
	}
	
	std::string endcap = "?";
	unsigned int station = 0;
	unsigned int ring = 0;
	unsigned int number = 0;
	
	// Check normal station name first
	if (sscanf(chamberName.c_str(), "%*c%1u/%1u/%02u", &station, &ring, &number) == 3) {
		endcap = chamberName.substr(0,1);
		// Else it's probably an SP, so check that
	} else if (sscanf(chamberName.c_str(), "SP%02u", &number) == 1) {
		endcap = (number <= 6) ? "+" : "-";
	}
	
	// Set names now.
	return new Fiber(fiberNumber, endcap, station, ring, number, killed);
	
}



xercesc::DOMElement *emu::fed::FiberParser::makeDOMElement(xercesc::DOMDocument *document, emu::fed::Fiber *fiber)
throw (emu::fed::exception::ParseException)
{
	try {
		// Make a crate element
		xercesc::DOMElement *fiberElement = document->createElement(X("Fiber"));
		
		// Set attributes
		Parser::insert(fiberElement, "FIBER_NUMBER", fiber->getFiberNumber());
		if (fiber->isKilled()) Parser::insert(fiberElement, "KILLED", 1);
		else Parser::insert(fiberElement, "KILLED", 0);
		Parser::insert(fiberElement, "CHAMBER", fiber->getName());
		
		return fiberElement;
	} catch (xercesc::DOMException &e) {
		std::ostringstream error;
		error << "Unable to create Fiber element: " << X(e.getMessage());
		XCEPT_RAISE(emu::fed::exception::ParseException, error.str());
	}
}
