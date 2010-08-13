/*****************************************************************************\
* $Id: Monitor.cc,v 1.19 2010/08/13 03:00:07 paste Exp $
\*****************************************************************************/
#include "emu/fed/Monitor.h"

#include <sstream>
#include <map>
#include <utility>
#include <iomanip>

#include "xgi/Method.h"
#include "cgicc/HTMLClasses.h"
#include "emu/base/Alarm.h"
#include "emu/fed/Crate.h"
#include "emu/fed/XMLConfigurator.h"
#include "emu/fed/DBConfigurator.h"
#include "emu/fed/AutoConfigurator.h"
#include "emu/fed/DDU.h"
#include "emu/fed/DCC.h"
#include "emu/fed/DataTable.h"
#include "emu/fed/DDUDebugger.h"
#include "emu/fed/DCCDebugger.h"
#include "emu/fed/Fiber.h"
#include "emu/fed/FIFO.h"
#include "emu/fed/JSONSpiritWriter.h"

XDAQ_INSTANTIATOR_IMPL(emu::fed::Monitor)



emu::fed::Monitor::Monitor(xdaq::ApplicationStub *stub):
xdaq::WebApplication(stub),
emu::fed::Application(stub),
emu::fed::Configurable(stub)
{
	
	// HyperDAQ pages
	xgi::bind(this, &emu::fed::Monitor::webDefault, "Default");
	xgi::bind(this, &emu::fed::Monitor::webGetTemperatures, "GetTemperatures");
	xgi::bind(this, &emu::fed::Monitor::webGetVoltages, "GetVoltages");
	xgi::bind(this, &emu::fed::Monitor::webGetOccupancies, "GetOccupancies");
	xgi::bind(this, &emu::fed::Monitor::webGetCounts, "GetCounts");
	xgi::bind(this, &emu::fed::Monitor::webGetFiberStatus, "GetFiberStatus");
	xgi::bind(this, &emu::fed::Monitor::webGetDDUStatus, "GetDDUStatus");
	xgi::bind(this, &emu::fed::Monitor::webGetDCCStatus, "GetDCCStatus");
	
	// Other SOAP call-back functions
	xoap::bind(this, &emu::fed::Monitor::onGetParameters, "GetParameters", XDAQ_NS_URI);
	
	// Other initializations
	configMode_ = "XML";
}



emu::fed::Monitor::~Monitor()
{
}



// HyperDAQ pages
void emu::fed::Monitor::webDefault(xgi::Input *in, xgi::Output *out)
{
	// Configure yourself if you haven't yet.  This is a software-only configure.
	if (!crateVector_.size()) {
		try {
			softwareConfigure();
			REVOKE_ALARM("MonitorDefault", NULL);
		} catch (emu::fed::exception::ConfigurationException &e) {
			std::ostringstream error;
			error << "Unable to properly configure the Monitor application";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "MonitorDefault", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		}
	}
	
	std::stringstream sTitle;
	sTitle << "FED Crate Monitor (" << systemName_.toString() << ")";
	std::vector<std::string> jsFileNames;
	jsFileNames.push_back("definitions.js");
	jsFileNames.push_back("errorFlasher.js");
	jsFileNames.push_back("monitor.js");
	jsFileNames.push_back("configurable.js");
	jsFileNames.push_back("common.js");
	*out << Header(sTitle.str(), jsFileNames);


	
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		
		std::ostringstream crateName;
		crateName << (*iCrate)->number();
	
		*out << cgicc::div()
			.set("class", "titlebar monitor_width")
			.set("id", "FED_Monitor_" + crateName.str() + "_titlebar") << std::endl;
		*out << cgicc::div("Crate " + crateName.str() + " Monitor")
			.set("class", "titletext") << std::endl;
		*out << cgicc::div() << std::endl;
		
		*out << cgicc::div()
			.set("class", "statusbar monitor_width")
			.set("id", "FED_Monitor_" + crateName.str() + "_statusbar") << std::endl;
		*out << cgicc::div("Time of last update:")
			.set("class", "timetext") << std::endl;
		*out << cgicc::div("never")
			.set("class", "loadtime")
			.set("name", "FED_Monitor_" + crateName.str())
			.set("id", "FED_Monitor_" + crateName.str() + "_loadtime") << std::endl;
		*out << cgicc::img()
			.set("class", "loadicon")
			.set("id", "FED_Monitor_" + crateName.str() + "_loadicon")
			.set("src", "/emu/emuDCS/FEDApps/images/empty.gif")
			.set("alt", "Loading...") << std::endl;
		*out << cgicc::br()
			.set("class", "clear") << std::endl;
		*out << cgicc::div() << std::endl;
		
		*out << cgicc::fieldset()
			.set("class", "dialog monitor_width")
			.set("id", "FED_Monitor_" + crateName.str() + "_dialog") << std::endl;
		
		*out << cgicc::table()
			.set("class", "noborder dialog tier1") << std::endl;
		*out << cgicc::tr() << std::endl;
		*out << cgicc::td("Currently monitoring: ") << std::endl;
		*out << cgicc::td() << std::endl;
		*out << cgicc::select()
			.set("class", "monitor_select")
			.set("name", "FED_Monitor_" + crateName.str())
			.set("crate", crateName.str()) << std::endl;
		*out << cgicc::option("Fiber status")
			.set("value", "FiberStatus")
			.set("selected", "true");
		*out << cgicc::option("Board occupancies")
			.set("value", "Occupancies");
			*out << cgicc::option("FPGA L1A counts")
			.set("value", "Counts");
			*out << cgicc::option("Temperatures")
			.set("value", "Temperatures");
			*out << cgicc::option("Voltages")
			.set("value", "Voltages");
		*out << cgicc::select() << std::endl;
		*out << cgicc::td() << std::endl;
		*out << cgicc::tr() << std::endl;
		*out << cgicc::table() << std::endl;
		
		
		// Draw the crate
		*out << cgicc::div()
			.set("class", "crate_table_container tier1") << std::endl;
		DataTable crateTable;
		crateTable->set("class", "crate_table");
		crateTable->set("id", "crate_" + crateName.str());
		crateTable->set("crate", crateName.str());
		
		// Draw the header
		
		crateTable(1, 0) << "Slot";
		crateTable(1, 1) << "Board" << cgicc::br() << "#L1A";
		crateTable(1, 2) << "Monitor";
		crateTable[1]->set("class", "headers");
		
		std::vector<DDU *> myDDUs = (*iCrate)->getDDUs();
		unsigned int iRow = 2;
		for (std::vector<DDU *>::iterator iDDU = myDDUs.begin(); iDDU != myDDUs.end(); iDDU++, iRow++) {
			std::ostringstream slotText;
			slotText << (*iDDU)->slot();
			crateTable[iRow]->set("class", "board ddu");
			crateTable[iRow]->set("id", "crate_" + crateName.str() + "_slot_" + slotText.str());
			crateTable(iRow, 0).setClass("slot");
			crateTable(iRow, 0)->set("id", "crate_" + crateName.str() + "_slot_" + slotText.str() + "_slot");
			crateTable(iRow, 0) << slotText.str();
			crateTable(iRow, 1).setClass("name");
			crateTable(iRow, 1) << "DDU " << (*iDDU)->getRUI() << cgicc::br() << "(FMM " << (*iDDU)->getFMMID() << ")" << cgicc::br();
			crateTable(iRow, 1) << cgicc::div(" ")
				.set("class", "l1a")
				.set("id", "crate_" + crateName.str() + "_slot_" + slotText.str() + "_l1a");
			crateTable(iRow, 2)->set("id", "crate_" + crateName.str() + "_slot_" + slotText.str() + "_data");
			crateTable(iRow, 2).setClass("data");
			crateTable(iRow, 2)->set("crate", crateName.str());
			crateTable(iRow, 2)->set("slot", slotText.str());
			crateTable(iRow, 2) << "Awaiting data....";
			
			crateTable(++iRow, 0).setClass("spacer");
			crateTable(iRow, 0)->set("colspan", "3");
		}
		
		std::vector<DCC *> myDCCs = (*iCrate)->getDCCs();
		for (std::vector<DCC *>::iterator iDCC = myDCCs.begin(); iDCC != myDCCs.end(); iDCC++, iRow++) {
			std::ostringstream slotText;
			slotText << (*iDCC)->slot();
			crateTable[iRow]->set("class", "board dcc");
			crateTable[iRow]->set("id", "crate_" + crateName.str() + "_slot_" + slotText.str());
			crateTable(iRow, 0).setClass("slot");
			crateTable(iRow, 0)->set("id", "crate_" + crateName.str() + "_slot_" + slotText.str() + "_slot");
			crateTable(iRow, 0) << slotText.str();
			crateTable(iRow, 1).setClass("name");
			crateTable(iRow, 1) << "DCC" << cgicc::br() << "(FMM " << (*iDCC)->getFMMID() << ")" << cgicc::br();
			crateTable(iRow, 1) << cgicc::span(" ")
				.set("class", "l1a")
				.set("id", "crate_" + crateName.str() + "_slot_" + slotText.str() + "_l1a");
			crateTable(iRow, 2)->set("id", "crate_" + crateName.str() + "_slot_" + slotText.str() + "_data");
			crateTable(iRow, 2).setClass("data");
			crateTable(iRow, 2)->set("crate", crateName.str());
			crateTable(iRow, 2)->set("slot", slotText.str());
			crateTable(iRow, 2) << "Awaiting data....";
		}
		
		*out << crateTable.toHTML() << std::endl;
		*out << cgicc::div() << std::endl;
		
		
		*out << cgicc::button()
			.set("class", "right button statechange start_button")
			.set("name", "FED_Monitor_" + crateName.str())
			.set("command", "start")
			.set("disabled", "true") << std::endl;
		*out << cgicc::img()
			.set("class", "icon")
			.set("src", "/emu/emuDCS/FEDApps/images/media-playback-start.png");
		*out << "Begin monitoring" << std::endl;
		*out << cgicc::button() << std::endl;
		
		*out << cgicc::button()
			.set("class", "right button statechange pause_button")
			.set("name", "FED_Monitor_" + crateName.str())
			.set("command", "pause") << std::endl;
		*out << cgicc::img()
			.set("class", "icon")
			.set("src", "/emu/emuDCS/FEDApps/images/media-playback-pause.png");
		*out << "Pause monitoring" << std::endl;
		*out << cgicc::button() << std::endl;
	
		*out << cgicc::fieldset() << std::endl;
	}
	
	// Advanced configuration options
	*out << cgicc::div()
		.set("class", "titlebar default_width")
		.set("id", "FED_Monitor_Configuration_titlebar") << std::endl;
	*out << cgicc::div("FED Commander Configuration Options")
		.set("class", "titletext") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::div()
		.set("class", "statusbar default_width")
		.set("id", "FED_Monitor_Configuration_statusbar") << std::endl;
	*out << cgicc::div("Time of last update:")
		.set("class", "timetext") << std::endl;
	*out << cgicc::div("never")
		.set("class", "loadtime")
		.set("id", "FED_Monitor_Configuration_loadtime") << std::endl;
	*out << cgicc::img()
		.set("class", "loadicon")
		.set("id", "FED_Monitor_Configuration_loadicon")
		.set("src", "/emu/emuDCS/FEDApps/images/empty.gif")
		.set("alt", "Loading...") << std::endl;
	*out << cgicc::br()
		.set("class", "clear") << std::endl;
	*out << cgicc::div() << std::endl;
	
	*out << cgicc::fieldset()
		.set("class", "dialog configuration_dialog default_width")
		.set("id", "FED_Monitor_Configuration_dialog")
		.set("name", "FED_Monitor_Configuration") << std::endl;
	
	*out << printConfigureOptions() << std::endl;;
	
	*out << cgicc::fieldset() << std::endl;
	
	*out << Footer() << std::endl;
	
}



void emu::fed::Monitor::webGetTemperatures(xgi::Input *in, xgi::Output *out)
{
	// Configure yourself if you haven't yet.  This is a software-only configure.
	if (!crateVector_.size()) {
		try {
			softwareConfigure();
			REVOKE_ALARM("MonitorGetTemperatures", NULL);
		} catch (emu::fed::exception::ConfigurationException &e) {
			std::ostringstream error;
			error << "Unable to properly configure the Monitor application";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "MonitorGetTemperatures", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		}
	}
	
	cgicc::Cgicc cgi(in);
	
	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}
	
	// Make a JSON output object
	JSONSpirit::Object output;
	
	output.push_back(JSONSpirit::Pair("action", "Temperatures"));
	
	// Peel the crate number from the input
	Crate *myCrate = NULL;
	try {
		myCrate = parseCrate(in);
	} catch (emu::fed::exception::ParseException &e) {
		output.push_back(JSONSpirit::Pair("exception", e.message()));
		*out << JSONSpirit::write(output);
		return;
	}
	
	output.push_back(JSONSpirit::Pair("crate", (int) myCrate->number()));
	
	// Make an array of DDUs
	JSONSpirit::Array dduArray;
	
	// Cycle through DDUs
	std::vector<DDU *> dduVector = myCrate->getDDUs();
	for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
		
		JSONSpirit::Object dduObject;
		
		dduObject.push_back(JSONSpirit::Pair("slot", (int) (*iDDU)->slot()));
		
		// Get temperatures
		JSONSpirit::Array tempArray;
		for (unsigned int iTemp = 0; iTemp < 4; iTemp++) {
			JSONSpirit::Object tempObject;
			tempObject.push_back(JSONSpirit::Pair("number", (int) iTemp));
			float temperature = -999;
			std::string status;
			std::string message;
			try {
				temperature = (*iDDU)->readTemperature(iTemp);
				std::pair<std::string, std::string> debugged = DDUDebugger::Temperature(temperature);
				status = debugged.second;
				message = debugged.first;
			} catch (emu::fed::exception::DDUException &e) {
				tempObject.push_back(JSONSpirit::Pair("exception", e.what()));
			}
			std::ostringstream tempStream;
			tempStream << std::fixed << std::setprecision(2) << temperature;
			tempObject.push_back(JSONSpirit::Pair("temperature", tempStream.str()));
			tempObject.push_back(JSONSpirit::Pair("status", status));
			tempObject.push_back(JSONSpirit::Pair("message", message));
			tempArray.push_back(tempObject);
		}
		
		dduObject.push_back(JSONSpirit::Pair("temperatures", tempArray));
		
		dduArray.push_back(dduObject);
		
	}
	
	output.push_back(JSONSpirit::Pair("ddus", dduArray));
	
	*out << JSONSpirit::write(output);
}



void emu::fed::Monitor::webGetVoltages(xgi::Input *in, xgi::Output *out)
{
	// Configure yourself if you haven't yet.  This is a software-only configure.
	if (!crateVector_.size()) {
		try {
			softwareConfigure();
			REVOKE_ALARM("MonitorGetVoltages", NULL);
		} catch (emu::fed::exception::ConfigurationException &e) {
			std::ostringstream error;
			error << "Unable to properly configure the Monitor application";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "MonitorGetVoltages", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		}
	}
	
	cgicc::Cgicc cgi(in);
	
	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}
	
	// Make a JSON output object
	JSONSpirit::Object output;
	
	output.push_back(JSONSpirit::Pair("action", "Voltages"));
	
	// Peel the crate number from the input
	Crate *myCrate = NULL;
	try {
		myCrate = parseCrate(in);
	} catch (emu::fed::exception::ParseException &e) {
		output.push_back(JSONSpirit::Pair("exception", e.message()));
		*out << JSONSpirit::write(output);
		return;
	}
	
	output.push_back(JSONSpirit::Pair("crate", (int) myCrate->number()));
	
	// Make an array of DDUs
	JSONSpirit::Array dduArray;
	
	// Cycle through DDUs
	std::vector<DDU *> dduVector = myCrate->getDDUs();
	for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
		
		// Get universal DDU information
		JSONSpirit::Object dduObject;
		
		dduObject.push_back(JSONSpirit::Pair("slot", (int) (*iDDU)->slot()));
		
		JSONSpirit::Array voltArray;
		
		for (unsigned int iVolt = 0; iVolt < 4; iVolt++) {
			JSONSpirit::Object voltObject;
			voltObject.push_back(JSONSpirit::Pair("number", (int) iVolt));
			float voltage = -999;
			std::string status;
			std::string message;
			try {
				voltage = (*iDDU)->readVoltage(iVolt);
				std::pair<std::string, std::string> debugged = DDUDebugger::Voltage(iVolt, voltage);
				status = debugged.second;
				message = debugged.first;
			} catch (emu::fed::exception::DDUException &e) {
				voltObject.push_back(JSONSpirit::Pair("exception", e.what()));
			}
			std::ostringstream voltStream;
			voltStream << std::fixed << std::setprecision(0) << voltage;
			voltObject.push_back(JSONSpirit::Pair("voltage", voltStream.str()));
			voltObject.push_back(JSONSpirit::Pair("status", status));
			voltObject.push_back(JSONSpirit::Pair("message", message));
			voltArray.push_back(voltObject);
		}

		dduObject.push_back(JSONSpirit::Pair("voltages", voltArray));
		
		dduArray.push_back(dduObject);
		
	}
	
	output.push_back(JSONSpirit::Pair("ddus", dduArray));
	
	*out << JSONSpirit::write(output);
}



void emu::fed::Monitor::webGetOccupancies(xgi::Input *in, xgi::Output *out)
{
	// Configure yourself if you haven't yet.  This is a software-only configure.
	if (!crateVector_.size()) {
		try {
			softwareConfigure();
			REVOKE_ALARM("MonitorGetOccupancies", NULL);
		} catch (emu::fed::exception::ConfigurationException &e) {
			std::ostringstream error;
			error << "Unable to properly configure the Monitor application";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "MonitorGetOccupancies", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		}
	}
	
	cgicc::Cgicc cgi(in);
	
	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}
	
	// Make a JSON output object
	JSONSpirit::Object output;
	
	output.push_back(JSONSpirit::Pair("action", "Occupancies"));
	
	// Peel the crate number from the input
	Crate *myCrate = NULL;
	try {
		myCrate = parseCrate(in);
	} catch (emu::fed::exception::ParseException &e) {
		output.push_back(JSONSpirit::Pair("exception", e.message()));
		*out << JSONSpirit::write(output);
		return;
	}
	
	output.push_back(JSONSpirit::Pair("crate", (int) myCrate->number()));
	
	// Make an array of DDUs
	JSONSpirit::Array dduArray;
	
	// Cycle through DDUs
	std::vector<DDU *> dduVector = myCrate->getDDUs();
	for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
		
		// Get universal DDU information
		JSONSpirit::Object dduObject;
		
		dduObject.push_back(JSONSpirit::Pair("slot", (int) (*iDDU)->slot()));
		
		// This will be needed for calculations
		uint32_t l1aScaler = 0;
		try {
			l1aScaler = (*iDDU)->readL1Scaler(emu::fed::DDUFPGA);
		} catch (emu::fed::exception::DDUException &e) {
			dduObject.push_back(JSONSpirit::Pair("exception", e.what()));
		}
		
		JSONSpirit::Array fiberArray;
		
		for (unsigned int iFiber = 0; iFiber < 15; iFiber++) {
			
			JSONSpirit::Object fiberObject;
			fiberObject.push_back(JSONSpirit::Pair("fiber", (int) iFiber));
			fiberObject.push_back(JSONSpirit::Pair("name", (*iDDU)->getFiber(iFiber)->getName()));
			
			JSONSpirit::Array countArray;
			
			std::vector<uint32_t> occupancies(4, 0);
			try {
				occupancies = (*iDDU)->readOccupancyMonitor();
			} catch (emu::fed::exception::DDUException &e) {
				fiberObject.push_back(JSONSpirit::Pair("exception", e.what()));
			}
			
			uint32_t dmbCount = (occupancies[0] & 0x0fffffff);
			uint32_t alctCount = (occupancies[1] & 0x0fffffff);
			uint32_t tmbCount = (occupancies[2] & 0x0fffffff);
			uint32_t cfebCount = (occupancies[3] & 0x0fffffff);
			
			JSONSpirit::Object dmbObject;
			dmbObject.push_back(JSONSpirit::Pair("type", "DMB"));
			dmbObject.push_back(JSONSpirit::Pair("count", (int) dmbCount));
			std::ostringstream percent;
			if (l1aScaler != 0) percent << std::fixed << std::setprecision(3) << ((float) dmbCount * 100 / (float) l1aScaler) << "%";
			dmbObject.push_back(JSONSpirit::Pair("percent", percent.str()));
			countArray.push_back(dmbObject);
			
			JSONSpirit::Object tmbObject;
			tmbObject.push_back(JSONSpirit::Pair("type", "TMB"));
			tmbObject.push_back(JSONSpirit::Pair("count", (int) tmbCount));
			percent.str("");
			if (dmbCount != 0) percent << std::fixed << std::setprecision(3) << ((float) tmbCount * 100 / (float) dmbCount) << "%";
			tmbObject.push_back(JSONSpirit::Pair("percent", percent.str()));
			countArray.push_back(tmbObject);
			
			JSONSpirit::Object alctObject;
			alctObject.push_back(JSONSpirit::Pair("type", "ALCT"));
			alctObject.push_back(JSONSpirit::Pair("count", (int) alctCount));
			percent.str("");
			if (dmbCount != 0) percent << std::fixed << std::setprecision(3) << ((float) alctCount * 100 / (float) dmbCount) << "%";
			alctObject.push_back(JSONSpirit::Pair("percent", percent.str()));
			countArray.push_back(alctObject);
			
			JSONSpirit::Object cfebObject;
			cfebObject.push_back(JSONSpirit::Pair("type", "CFEB"));
			cfebObject.push_back(JSONSpirit::Pair("count", (int) cfebCount));
			percent.str("");
			if (dmbCount != 0) percent << std::fixed << std::setprecision(3) << ((float) cfebCount * 100 / (float) dmbCount) << "%";
			cfebObject.push_back(JSONSpirit::Pair("percent", percent.str()));
			countArray.push_back(cfebObject);
			
			fiberObject.push_back(JSONSpirit::Pair("occupancies", countArray));
			
			fiberArray.push_back(fiberObject);
			
		}
		
		dduObject.push_back(JSONSpirit::Pair("fibers", fiberArray));
		
		dduArray.push_back(dduObject);
		
	}
	
	output.push_back(JSONSpirit::Pair("ddus", dduArray));

	*out << JSONSpirit::write(output);
}



void emu::fed::Monitor::webGetCounts(xgi::Input *in, xgi::Output *out)
{
	// Configure yourself if you haven't yet.  This is a software-only configure.
	if (!crateVector_.size()) {
		try {
			softwareConfigure();
			REVOKE_ALARM("MonitorGetCounts", NULL);
		} catch (emu::fed::exception::ConfigurationException &e) {
			std::ostringstream error;
			error << "Unable to properly configure the Monitor application";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "MonitorGetCounts", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		}
	}
	
	cgicc::Cgicc cgi(in);
	
	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}
	
	// Make a JSON output object
	JSONSpirit::Object output;
	
	output.push_back(JSONSpirit::Pair("action", "Counts"));
	
	// Peel the crate number from the input
	Crate *myCrate = NULL;
	try {
		myCrate = parseCrate(in);
	} catch (emu::fed::exception::ParseException &e) {
		output.push_back(JSONSpirit::Pair("exception", e.message()));
		*out << JSONSpirit::write(output);
		return;
	}
	
	output.push_back(JSONSpirit::Pair("crate", (int) myCrate->number()));
	
	// Make an array of DDUs
	JSONSpirit::Array dduArray;
	
	// Cycle through DDUs
	std::vector<DDU *> dduVector = myCrate->getDDUs();
	for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
		
		// Get universal DDU information
		JSONSpirit::Object dduObject;
		
		dduObject.push_back(JSONSpirit::Pair("slot", (int) (*iDDU)->slot()));
		
		JSONSpirit::Array countsArray;
		
		JSONSpirit::Object ddufpgaObject;
		ddufpgaObject.push_back(JSONSpirit::Pair("name", "DDUFPGA"));
		uint32_t scaler = 0;
		try {
			scaler = (*iDDU)->readL1Scaler(emu::fed::DDUFPGA);
		} catch (emu::fed::exception::DDUException &e) {
			ddufpgaObject.push_back(JSONSpirit::Pair("exception", e.what()));
		}
		ddufpgaObject.push_back(JSONSpirit::Pair("count", (int) scaler));
		countsArray.push_back(ddufpgaObject);
		
		JSONSpirit::Object infpga01Object;
		infpga01Object.push_back(JSONSpirit::Pair("name", "INFPGA01"));
		scaler = 0;
		try {
			scaler = (*iDDU)->readL1Scaler(emu::fed::INFPGA0);
		} catch (emu::fed::exception::DDUException &e) {
			infpga01Object.push_back(JSONSpirit::Pair("exception", e.what()));
		}
		infpga01Object.push_back(JSONSpirit::Pair("count", (int) scaler));
		countsArray.push_back(infpga01Object);
		
		JSONSpirit::Object infpga02Object;
		infpga02Object.push_back(JSONSpirit::Pair("name", "INFPGA02"));
		scaler = 0;
		try {
			scaler = (*iDDU)->readL1Scaler1(emu::fed::INFPGA0);
		} catch (emu::fed::exception::DDUException &e) {
			infpga02Object.push_back(JSONSpirit::Pair("exception", e.what()));
		}
		infpga02Object.push_back(JSONSpirit::Pair("count", (int) scaler));
		countsArray.push_back(infpga02Object);
		
		JSONSpirit::Object infpga11Object;
		infpga11Object.push_back(JSONSpirit::Pair("name", "INFPGA11"));
		scaler = 0;
		try {
			scaler = (*iDDU)->readL1Scaler(emu::fed::INFPGA1);
		} catch (emu::fed::exception::DDUException &e) {
			infpga11Object.push_back(JSONSpirit::Pair("exception", e.what()));
		}
		infpga11Object.push_back(JSONSpirit::Pair("count", (int) scaler));
		countsArray.push_back(infpga11Object);
		
		JSONSpirit::Object infpga12Object;
		infpga12Object.push_back(JSONSpirit::Pair("name", "INFPGA12"));
		scaler = 0;
		try {
			scaler = (*iDDU)->readL1Scaler1(emu::fed::INFPGA1);
		} catch (emu::fed::exception::DDUException &e) {
			infpga12Object.push_back(JSONSpirit::Pair("exception", e.what()));
		}
		infpga12Object.push_back(JSONSpirit::Pair("count", (int) scaler));
		countsArray.push_back(infpga12Object);
		
		dduObject.push_back(JSONSpirit::Pair("counts", countsArray));
		
		dduArray.push_back(dduObject);
		
	}
	
	output.push_back(JSONSpirit::Pair("ddus", dduArray));
	
	*out << JSONSpirit::write(output);
}



void emu::fed::Monitor::webGetFiberStatus(xgi::Input *in, xgi::Output *out)
{
	// Configure yourself if you haven't yet.  This is a software-only configure.
	if (!crateVector_.size()) {
		try {
			softwareConfigure();
			REVOKE_ALARM("MonitorGetFiberStatus", NULL);
		} catch (emu::fed::exception::ConfigurationException &e) {
			std::ostringstream error;
			error << "Unable to properly configure the Monitor application";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "MonitorGetFiberStatus", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		}
	}
	
	cgicc::Cgicc cgi(in);
	
	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}
	
	// Make a JSON output object
	JSONSpirit::Object output;
	
	output.push_back(JSONSpirit::Pair("action", "FiberStatus"));
	
	// Peel the crate number from the input
	Crate *myCrate = NULL;
	try {
		myCrate = parseCrate(in);
	} catch (emu::fed::exception::ParseException &e) {
		output.push_back(JSONSpirit::Pair("exception", e.message()));
		*out << JSONSpirit::write(output);
		return;
	}
	
	output.push_back(JSONSpirit::Pair("crate", (int) myCrate->number()));
	
	// Make an array of DDUs
	JSONSpirit::Array dduArray;
	
	// Cycle through DDUs
	std::vector<DDU *> dduVector = myCrate->getDDUs();
	for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
		
		// Get universal DDU information
		JSONSpirit::Object dduObject;
		
		dduObject.push_back(JSONSpirit::Pair("slot", (int) (*iDDU)->slot()));
		
		JSONSpirit::Array fiberArray;
		
		uint16_t fiberStatus = 0;
		uint16_t liveFibers = 0;
		try {
			fiberStatus = (*iDDU)->readFiberErrors();
			liveFibers = (*iDDU)->readLiveFibers();
		} catch (emu::fed::exception::DDUException &e) {
			dduObject.push_back(JSONSpirit::Pair("exception", e.what()));
		}
		
		for (size_t iFiber = 0; iFiber < 15; iFiber++) {
		
			JSONSpirit::Object fiberObject;
			fiberObject.push_back(JSONSpirit::Pair("number", (int) iFiber));
			
			fiberObject.push_back(JSONSpirit::Pair("name", (*iDDU)->getFiber(iFiber)->getName()));
			
			std::string status = "ok";
			std::string message = "ok";
			
			Fiber *fiber;
			try {
				fiber = (*iDDU)->getFiber(iFiber);
			} catch (emu::fed::exception::OutOfBoundsException &e) {
				fiber = new Fiber(iFiber);
				fiberObject.push_back(JSONSpirit::Pair("exception", e.what()));
			}

			if (fiber->isKilled()) { // Killed fibers first
				status = "killed";
				message = "killed";
			} else if (!(liveFibers & (1 << iFiber))) { // Dead fibers second
				status = "undefined";
				message = "no link";
			} else if (fiberStatus & (1 << iFiber)) {
				status = "error";
				message = "error";
				// TODO what should the message be?
			}
			
			fiberObject.push_back(JSONSpirit::Pair("status", status));
			fiberObject.push_back(JSONSpirit::Pair("message", message));
			
			fiberArray.push_back(fiberObject);
		}
		
		dduObject.push_back(JSONSpirit::Pair("fibers", fiberArray));
		
		dduArray.push_back(dduObject);
		
	}
	
	output.push_back(JSONSpirit::Pair("ddus", dduArray));

	*out << JSONSpirit::write(output);
}



void emu::fed::Monitor::webGetDCCStatus(xgi::Input *in, xgi::Output *out)
{
	// Configure yourself if you haven't yet.  This is a software-only configure.
	if (!crateVector_.size()) {
		try {
			softwareConfigure();
			REVOKE_ALARM("MonitorGetDCCStatus", NULL);
		} catch (emu::fed::exception::ConfigurationException &e) {
			std::ostringstream error;
			error << "Unable to properly configure the Monitor application";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "MonitorGetDCCStatus", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		}
	}
	
	cgicc::Cgicc cgi(in);
	
	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}
	
	// Make a JSON output object
	JSONSpirit::Object output;
	
	// Peel the crate number from the input
	Crate *myCrate = NULL;
	try {
		myCrate = parseCrate(in);
	} catch (emu::fed::exception::ParseException &e) {
		output.push_back(JSONSpirit::Pair("exception", e.message()));
		*out << JSONSpirit::write(output);
		return;
	}
	
	output.push_back(JSONSpirit::Pair("crate", (int) myCrate->number()));
	
	// Make an array of DCCs
	JSONSpirit::Array dccArray;
	
	// Cycle through DCCs
	std::vector<DCC *> dccVector = myCrate->getDCCs();
	for (std::vector<DCC *>::iterator iDCC = dccVector.begin(); iDCC != dccVector.end(); iDCC++) {
		
		// Get universal DCC information
		JSONSpirit::Object dccObject;
		
		// Slot number
		dccObject.push_back(JSONSpirit::Pair("slot", (int) (*iDCC)->slot()));
		
		// FMM Status
		std::string statusDecoded = "undefined";
		try {
			uint8_t fmmStatus = (*iDCC)->readFMMStatus();
			std::multimap<std::string, std::string> debugged = DCCDebugger::FMMStatus(fmmStatus);
			if (debugged.find("undefined") != debugged.end()) statusDecoded = "undefined";
			else if (debugged.find("error") != debugged.end()) statusDecoded = "error";
			else if (debugged.find("warning") != debugged.end()) statusDecoded = "warning";
			else if (debugged.find("caution") != debugged.end()) statusDecoded = "caution";
			else statusDecoded = "ok";
		} catch (emu::fed::exception::DCCException &e) {
			dccObject.push_back(JSONSpirit::Pair("exception", e.what()));
		}
		dccObject.push_back(JSONSpirit::Pair("fmmStatus", statusDecoded));
		
		// L1A count
		uint32_t dccL1A = 0;
		try {
			dccL1A = (*iDCC)->readL1A();
		} catch (emu::fed::exception::DCCException &e) {
			dccObject.push_back(JSONSpirit::Pair("exception", e.what()));
		}
		dccObject.push_back(JSONSpirit::Pair("L1A", (int) dccL1A));
		
		// DDU FIFO rates
		JSONSpirit::Array fifoArray;
		uint16_t fifoStatus = 0;
		try {
			fifoStatus = (*iDCC)->readFIFOStatus();
		} catch (emu::fed::exception::DCCException &e) {
			dccObject.push_back(JSONSpirit::Pair("exception", e.what()));
		}
		for (size_t iFIFO = 1; iFIFO <= 11; iFIFO++) {
			// This code here is because iFIFO is an index from 1-11,
			// but it does not account for the SLinks that are index 0 and 6.
			// What we really want is to count 1, 2, 3, 4, 5, 7, 8, 9, 10, 11.
			if (iFIFO == 6) continue;
			
			JSONSpirit::Object fifoObject;
			
			unsigned int iSlot = 0;
			try {
				iSlot = (*iDCC)->getDDUSlotFromFIFO(iFIFO);
			} catch (emu::fed::exception::OutOfBoundsException &e) {
				// Not a valid FIFO number?
				fifoObject.push_back(JSONSpirit::Pair("exception", e.what()));
			}
			fifoObject.push_back(JSONSpirit::Pair("slot", (int) iSlot));
			
			uint16_t rate = 0;
			std::string status = "ok";
			std::string message = "bytes/s";
			try {
				// Look up rates by slot, not by FIFO
				rate = (*iDCC)->readDDURate(iSlot);
			} catch (emu::fed::exception::DCCException &e) {
				fifoObject.push_back(JSONSpirit::Pair("exception", e.what()));
			}
			
			if (!(*iDCC)->getFIFO(iFIFO)->isUsed()) {
				status = "killed";
				message = "not used";
			} else {
				unsigned int jFIFO = iFIFO - 1;
				if (iFIFO > 5) --jFIFO;
				std::pair<std::string, std::string> debuggedStatus = DCCDebugger::decodeFIFOStatus(fifoStatus, jFIFO);
				status = debuggedStatus.second;
				if (status != "ok") message = debuggedStatus.first;
			}
			
			fifoObject.push_back(JSONSpirit::Pair("rate", rate));
			fifoObject.push_back(JSONSpirit::Pair("status", status));
			fifoObject.push_back(JSONSpirit::Pair("message", message));
			
			fifoArray.push_back(fifoObject);
		}
		dccObject.push_back(JSONSpirit::Pair("ddurates", fifoArray));
		
		// SLink rates
		JSONSpirit::Array slinkArray;
		uint16_t slinkStatus = 0;
		try {
			slinkStatus = (*iDCC)->readSLinkStatus();
		} catch (emu::fed::exception::DCCException &e) {
			dccObject.push_back(JSONSpirit::Pair("exception", e.what()));
		}
		for (unsigned int iLink = 1; iLink <= 2; iLink++) {
			
			JSONSpirit::Object slinkObject;
			
			slinkObject.push_back(JSONSpirit::Pair("slink", (int) iLink));
			
			uint16_t rate = 0;
			std::string status = "ok";
			std::string message = "bytes/s";
			
			try {
				rate = (*iDCC)->readSLinkRate(iLink);
			} catch (emu::fed::exception::DCCException &e) {
				slinkObject.push_back(JSONSpirit::Pair("exception", e.what()));
			}

			std::pair<std::string, std::string> debuggedStatus = DCCDebugger::decodeSLinkStatus(slinkStatus, iLink - 1);
			status = debuggedStatus.second;
			if (status != "ok") message = debuggedStatus.first;
			
			slinkObject.push_back(JSONSpirit::Pair("rate", rate));
			slinkObject.push_back(JSONSpirit::Pair("status", status));
			slinkObject.push_back(JSONSpirit::Pair("message", message));
			
			slinkArray.push_back(slinkObject);
		}
		
		dccObject.push_back(JSONSpirit::Pair("slinkrates", slinkArray));
		
		dccArray.push_back(dccObject);
		
	}
	
	output.push_back(JSONSpirit::Pair("dccs", dccArray));
	
	*out << JSONSpirit::write(output);
}



void emu::fed::Monitor::webGetDDUStatus(xgi::Input *in, xgi::Output *out)
{
	// Configure yourself if you haven't yet.  This is a software-only configure.
	if (!crateVector_.size()) {
		try {
			softwareConfigure();
			REVOKE_ALARM("MonitorGetDDUStatus", NULL);
		} catch (emu::fed::exception::ConfigurationException &e) {
			std::ostringstream error;
			error << "Unable to properly configure the Monitor application";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "MonitorGetDDUStatus", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		}
	}
	
	cgicc::Cgicc cgi(in);
	
	// Need some header information to be able to return JSON
	if (cgi.getElement("debug") == cgi.getElements().end() || cgi["debug"]->getIntegerValue() != 1) {
		cgicc::HTTPResponseHeader jsonHeader("HTTP/1.1", 200, "OK");
		jsonHeader.addHeader("Content-type", "application/json");
		out->setHTTPResponseHeader(jsonHeader);
	}
	
	// Make a JSON output object
	JSONSpirit::Object output;
	
	// Peel the crate number from the input
	Crate *myCrate = NULL;
	try {
		myCrate = parseCrate(in);
	} catch (emu::fed::exception::ParseException &e) {
		output.push_back(JSONSpirit::Pair("exception", e.what()));
		*out << JSONSpirit::write(output);
		return;
	}
	
	output.push_back(JSONSpirit::Pair("crate", (int) myCrate->number()));
	
	// Make an array of DDUs
	JSONSpirit::Array dduArray;
	
	// Cycle through DDUs
	std::vector<DDU *> dduVector = myCrate->getDDUs();
	for (std::vector<DDU *>::iterator iDDU = dduVector.begin(); iDDU != dduVector.end(); iDDU++) {
		
		// Get universal DDU information
		JSONSpirit::Object dduObject;
		
		// Slot number
		dduObject.push_back(JSONSpirit::Pair("slot", (int) (*iDDU)->slot()));
		
		// FMM status (decoded)
		std::string statusDecoded = "undefined";
		try {
			uint8_t fmmStatus = (*iDDU)->readRealFMM();
			statusDecoded = DDUDebugger::RealFMM(fmmStatus).second;
		} catch (emu::fed::exception::DDUException &e) {
			dduObject.push_back(JSONSpirit::Pair("exception", e.what()));
		}
		dduObject.push_back(JSONSpirit::Pair("fmmStatus", statusDecoded));
		
		// L1A count
		uint32_t l1aScaler = 0;
		try {
			l1aScaler = (*iDDU)->readL1Scaler(emu::fed::DDUFPGA);
		} catch (emu::fed::exception::DDUException &e) {
			dduObject.push_back(JSONSpirit::Pair("exception", e.what()));
		}
		dduObject.push_back(JSONSpirit::Pair("L1A", (int) l1aScaler));
		
		dduArray.push_back(dduObject);
		
	}
	
	output.push_back(JSONSpirit::Pair("ddus", dduArray));
	
	*out << JSONSpirit::write(output);
}



emu::fed::Crate *emu::fed::Monitor::parseCrate(xgi::Input *in)
throw (emu::fed::exception::ParseException)
{
	cgicc::Cgicc cgi(in);
	
	// Peel the crate number from the input
	unsigned int crateNumber = 0;
	if (cgi.getElement("crateNumber") != cgi.getElements().end()) {
		crateNumber = cgi["crateNumber"]->getIntegerValue();
	} else {
		XCEPT_RAISE(emu::fed::exception::ParseException, "crateNumber not found");
	}
	
	// Pick the appropriate crate
	Crate *myCrate = NULL;
	for (std::vector<Crate *>::iterator iCrate = crateVector_.begin(); iCrate != crateVector_.end(); iCrate++) {
		if ((*iCrate)->number() == crateNumber) {
			myCrate = (*iCrate);
			break;
		}
	}
	
	if (myCrate == NULL) {
		XCEPT_RAISE(emu::fed::exception::ParseException, "crateNumber does not match any crates under the perview of this monitor");
	}
	
	return myCrate;
}



// PGK Ugly, but it must be done.  We have to update the parameters that the
//  Manager asks for or else they won't be updated!
xoap::MessageReference emu::fed::Monitor::onGetParameters(xoap::MessageReference message)
{
	// Configure yourself if you haven't yet.  This is a software-only configure.
	if (!crateVector_.size()) {
		try {
			softwareConfigure();
			REVOKE_ALARM("MonitorGetParameters", NULL);
		} catch (emu::fed::exception::ConfigurationException &e) {
			std::ostringstream error;
			error << "Unable to properly configure the Monitor application";
			LOG4CPLUS_ERROR(getApplicationLogger(), error.str());
			RAISE_ALARM_NESTED(emu::fed::exception::ConfigurationException, "MonitorGetParameters", "ERROR", error.str(), e.getProperty("tag"), NULL, e);
		}
	}
	return emu::fed::Application::onGetParameters(message);
	
}
