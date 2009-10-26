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

//	string xmlcfg = "/home/dqm/TriDAS/emu/emuDQM/emuMonitor/xml/EmuDQMBooking.xml";
	string xmlHistosBookingCfg = "http://cms-dqm03.phys.ufl.edu/dqm/results/emuDQMBooking.xml";
        string xmlCanvasesCfg = "http://cms-dqm03.phys.ufl.edu/dqm/results/emuDQMCanvases.xml";
	EmuPlotter plotter(logger);
	plotter.setXMLHistosBookingCfgFile(xmlHistosBookingCfg);
	plotter.setXMLCanvasesCfgFile(xmlCanvasesCfg);
	plotter.book();

	int NumberOfEvents = 1000000;
	int startEvent = 0;
	string datafile = "";
  	string histofile = "dqm_results.root";
        // EmuFileReader ddu; //( inputDeviceName_.toString(), inputDataFormatInt_ );
	// FileReaderDDU ddu;
	unsigned long dduCheckMask=0xFFFFDFFF;
        unsigned long binCheckMask=0xF7FB7BF6;
	// int binCheckMask=0xFFFFFFFF;
	// int dduCheckMask = 0x0;
//	int binCheckMask = 0x0;
	switch (argc) {
		case 7: binCheckMask = atoi(argv[6]);	
		case 6: dduCheckMask = atoi(argv[5]);
		case 5: startEvent = atoi(argv[4]);
		case 4: histofile = argv[3];
		case 3: NumberOfEvents = atoi(argv[2]);
		case 2:	datafile = argv[1];
		break;
	}
	
	if (datafile.find(".bin") != string::npos) {	
		histofile = datafile;
		if (histofile.rfind("/") != string::npos) 
			histofile.erase(0, histofile.rfind("/")+1);
		histofile = histofile.replace(histofile.find(".bin"), 4, ".root");
	}
        if (datafile.find(".raw") != string::npos) {
                histofile = datafile;
                if (histofile.rfind("/") != string::npos)
                        histofile.erase(0, histofile.rfind("/")+1);
                histofile = histofile.replace(histofile.find(".raw"), 4, ".root");
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
	int i=0;

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
			LOG4CPLUS_WARN (logger, "Event#"<< dec << i<< " **** Buffer size: " << ddu.dataLength() << " bytes");
			plotter.processEvent(ddu.data(), ddu.dataLength(), status);
			if (i%20000 == 0)
                                plotter.saveToROOTFile(histofile.c_str());
		}
	
		
		if (i+1>(startEvent+NumberOfEvents)) break;  
	} 


	t1 = time(0);
        LOG4CPLUS_WARN (logger, "Total time: " << t1-t0);

	LOG4CPLUS_WARN (logger, "Events: " << i);
	plotter.saveToROOTFile(histofile.c_str());
	// plotter.saveImages("images", "png" , 1600, 1200);
	ddu.close();

	// delete plotter;
	
	return 0;
};
