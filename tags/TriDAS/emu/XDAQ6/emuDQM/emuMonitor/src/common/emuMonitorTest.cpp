// #include "Muon/METBRawFormat/interface/FileReaderDDU.h"
// #include "FileReaderDDU.h"
// #include "CSCEventData.h"
#include "EmuFileReader.h"
#include "EmuSpyReader.h"
#include "EmuPlotter.h"
#include <iomanip>
#include <iostream>

#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/helpers/appenderattachableimpl.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/pointer.h"
#include "log4cplus/spi/loggingevent.h"
#include "coloredlayout.h"

using namespace log4cplus;
using namespace log4cplus::helpers;
using namespace log4cplus::spi;

int main(int argc, char **argv) {
  
//	For time performance	
	//long t0, t1;
	//t0 = time(0);
	Logger logger = Logger::getRoot();  
	// Initialize log system
        SharedAppenderPtr appender(new ConsoleAppender());
	appender->setLayout( std::auto_ptr<Layout>(new SimpleColoredLayout()) );
        logger.addAppender(appender);
	logger.setLogLevel(WARN_LOG_LEVEL);

	std::string cfgDir="";
        if (getenv("HOME") && string(getenv("HOME")).size())
        cfgDir=getenv("HOME"); //+"/config/";

        cfgDir+="/config/";

//	string xmlcfg = "/home/dqm/TriDAS/emu/emuDQM/emuMonitor/xml/EmuDQMBooking.xml";
	std::string xmlHistosBookingCfg = "file://" + cfgDir + "emuDQMBooking.xml";
        std::string xmlCanvasesCfg = "file://" + cfgDir + "emuDQMCanvases.xml";
        std::string cscMapFile = cfgDir+"csc_map.txt";


	EmuPlotter plotter(logger);
	plotter.setXMLHistosBookingCfgFile(xmlHistosBookingCfg);
	plotter.setXMLCanvasesCfgFile(xmlCanvasesCfg);
	plotter.setCSCMapFile(cscMapFile);
	plotter.book();

	uint32_t NumberOfEvents = 0xFFFFFFFF;
	uint32_t startEvent = 0;
	string datafile = "";
  	string histofile = "dqm_results.root";
	string plotsdir = "images";	// Output images path
	string imgFormat = "png"; 	// Output image format
	uint32_t imgWidth = 1200;	// Output image width
	uint32_t imgHeight = 900;	// Putput image height
        // EmuFileReader ddu; //( inputDeviceName_.toString(), inputDataFormatInt_ );
	// FileReaderDDU ddu;
	uint32_t dduCheckMask=0xFFFFDFFF;
        uint32_t binCheckMask=0xF7FB3BF6;

	int node=0;
	// int binCheckMask=0xFFFFFFFF;
	// int dduCheckMask = 0x0;
//	int binCheckMask = 0x0;
	switch (argc) {
		case 7: binCheckMask = strtoul(argv[6], NULL, 0);	
		case 6: dduCheckMask = strtoul(argv[5], NULL, 0);
		case 5: startEvent = atoi(argv[4]);
		case 4: histofile = argv[3];
		case 3: NumberOfEvents = atoi(argv[2]);
		case 2:	datafile = argv[1];
		break;
	}

	
	if (datafile.find(".bin") != std::string::npos) {	
		histofile = datafile;
		if (histofile.rfind("/") != std::string::npos) 
			histofile.erase(0, histofile.rfind("/")+1);
		plotsdir = histofile;
                plotsdir = plotsdir.replace(plotsdir.find(".bin"), 4, ".plots");
		histofile = histofile.replace(histofile.find(".bin"), 4, ".root");
	}
        if (datafile.find(".raw") != std::string::npos) {
                histofile = datafile;
                if (histofile.rfind("/") != std::string::npos)
                        histofile.erase(0, histofile.rfind("/")+1);
		plotsdir = histofile;
		plotsdir = plotsdir.replace(plotsdir.find(".raw"), 4, ".plots");
                histofile = histofile.replace(histofile.find(".raw"), 4, ".root");
        }

	// Try to extract Node ID from data file name (should match pattern EmuRUInn)
	if (datafile.find("EmuRUI") != std::string::npos) {
		std::string nodestr = datafile.substr(datafile.find("EmuRUI"), 8);
		nodestr.erase(0,6); // remove "EmuRUI"
		node = atoi(nodestr.c_str());
		LOG4CPLUS_WARN (logger, "Found Node ID " << node);
        }

	if (datafile.find(".root") != std::string::npos) {
		LOG4CPLUS_WARN (logger, "Load MEs from ROOT file " << datafile);
		histofile = datafile;
                if (histofile.rfind("/") != std::string::npos)
                        histofile.erase(0, histofile.rfind("/")+1);
                plotsdir = histofile;
		std::string runname = histofile; 
		runname = runname.replace(runname.find(".root"), 5, "");
                plotsdir = plotsdir.replace(plotsdir.find(".root"), 5, ".plots");
		plotter.loadFromROOTFile(datafile);
		plotter.saveCanvasImages(plotsdir.c_str(), imgFormat , imgWidth, imgHeight, runname);
		plotter.generateLayout("csc-layouts.py", "EMU");
	//	plotter.generateCanvasesListFile("canvases_list.js", imgFormat);
		return 0;
	}

	plotter.setHistoFile(histofile.c_str());	
	if (dduCheckMask >= 0) {
        	plotter.setDDUCheckMask(dduCheckMask);
	}
	if (binCheckMask >= 0) {
		plotter.setBinCheckMask(binCheckMask);
	}
	// plotter.SetDDU2004(1);
	
	// if(debug_printout)  
		LOG4CPLUS_WARN (logger,  "Opening file " << datafile << " from event = " << startEvent 
			<< " to event = " << NumberOfEvents);
	EmuFileReader ddu(datafile.c_str(), EmuReader::DDU);
	ddu.open(datafile.c_str());
	// if(debug_printout)  
	LOG4CPLUS_WARN (logger, "File " << datafile << " is opened");
        uint32_t i=0;

	long t0, t1;
        t0 = time(0);

	while (ddu.readNextEvent()) {
		i++;
		// short data = *((short *) ddu.data());
		int status = 0;
//KK
                if( ddu.getErrorFlag()==EmuFileReader::Type2 ) status |= 0x8000;
                if( ddu.getErrorFlag()==EmuFileReader::Type3 ) status |= 0x4000;
                if( ddu.getErrorFlag()==EmuFileReader::Type4 ) status |= 0x2000;
                if( ddu.getErrorFlag()==EmuFileReader::Type5 ) status |= 0x1000;
                if( ddu.getErrorFlag()==EmuFileReader::Type6 ) status |= 0x0800;
		if(status) continue;
//KKend
		if ((i>=startEvent) && (i<=(startEvent+NumberOfEvents))) { 
//			cout << hex << data << endl; 
			// plotter.fill((unsigned char*) ddu.data(), ddu.dataLength(), status);			
	//		if(debug_printout) 
			LOG4CPLUS_INFO (logger, "Event#"<< dec << i<< " **** Buffer size: " << ddu.dataLength() << " bytes");
			plotter.processEvent(ddu.data(), ddu.dataLength(), status, node);
	//		if (i%20000 == 0)
          //                      plotter.saveToROOTFile(histofile.c_str());
			if (i%1000 == 0) LOG4CPLUS_WARN (logger, "Processed Events: "<< dec << i);
		}
	
		
		if (i+1>(startEvent+NumberOfEvents)) break;  
	} 


	t1 = time(0);
        LOG4CPLUS_WARN (logger, "Processed Total time: " << t1-t0 << " seconds");

	LOG4CPLUS_WARN (logger, "Processed Events: " << i << " Rate: " << (i/(t1-t0)) << " Events/sec" );
//	plotter.saveToROOTFile(histofile.c_str());
//	plotter.saveImages("images", "png" , 1600, 1200);
//	plotter.saveCanvasImages(plotsdir.c_str(), imgFormat , imgWidth, imgHeight);
	plotter.saveToROOTFile(histofile.c_str());
//        plotter.generateCanvasesListFile("canvases_list.js", imgFormat);
//	plotter.saveCanvasImages(plotsdir.c_str(), imgFormat , imgWidth, imgHeight);

	ddu.close();

	// delete plotter;
	
	return 0;
};
