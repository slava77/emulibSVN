
/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuMonitor_h_
#define _EmuMonitor_h_

#include <vector>

#include "xdaq.h"
#include "xdata.h"
#include "toolbox.h"
#include "xoap.h"
#include "xgi.h"

#include "Task.h"

#include "i2o/Method.h"
#include "i2o/utils/AddressMap.h"

#include "i2oEmuMonitorMsg.h"

#include "EmuFileReader.h"
#include "EmuSpyReader.h"

#include "FileReaderDDU.h"

#include "EmuPlotter.h"

#include "TMessage.h"

#include "EmuDQM_AppParameters.h"
#include "EmuDQM_SOAPCommands.h"
/*
#include "EmuPlotterInterface.h"
#include "EmuLocalPlotter.h"
#include "EmuGlobalPlotter.h"
*/

/*
  #include "Task.h"
  #include "toolbox/mem/Reference.h"
  #include "toolbox/mem/Pool.h"
  #include "toolbox/fsm/FiniteStateMachine.h"
  #include "xdaq/Application.h"
  #include "xdata/UnsignedLong.h"
  #include "xdata/include/xdata/String.h"
  #include "xdata/Event.h"
  #include "xdata/ActionListener.h"
*/

class EmuMonitorTimerTask: public Task
{

public:
   EmuMonitorTimerTask(): Task("EmuMonitorTimerTask") {
        timerDelay=60;
        plotter = NULL;
	fname = "";
	fActive= false;
        }
   EmuMonitorTimerTask(std::string name): Task("EmuMonitorTimerTask"), fname(name) {
        timerDelay=120;
        plotter = NULL;
	fActive=false;
        }
  ~EmuMonitorTimerTask() { }

   void setTimer(int delay) { timerDelay = delay;}
   void setPlotter(EmuPlotter* pl) {plotter = pl;}
   void setROOTFileName(std::string name) {fname=name;};
   bool isActive() const {return fActive;};

   int svc() {
	fActive=true;
	if (plotter != NULL) plotter->saveToROOTFile(fname);

	/*
        while (1) {
                sleep(timerDelay);
                if (timerDelay && plotter) {
                //      sleep(timerDelay);
                //        cout << "++++++++++++ Saving Histos ++++++++++++" << endl;
                //         plotter->saveHistos();
			plotter->saveToROOTFile(fname);
                }
        }
	*/
	fActive=false;
        return 0;
   }

private:
	bool fActive;
        int timerDelay;
	std::string fname;
        EmuPlotter * plotter;

};


class EmuMonitor: public xdaq::WebApplication, xdata::ActionListener, Task
{	
 public:
  XDAQ_INSTANTIATOR();

  EmuMonitor(xdaq::ApplicationStub* c) throw(xdaq::exception::Exception);

 protected:

  // readout service routine
  int svc();

  bool onError ( xcept::Exception& ex, void * context );

  // == Callback for requesting current exported parameter values
  void actionPerformed (xdata::Event& e);

  // == Callback for incoming i2o message
  void emuDataMsg(toolbox::mem::Reference *bufRef);

  int sendDataRequest(unsigned long last);

  xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception);
  xoap::MessageReference onReset (xoap::MessageReference msg) throw (xoap::exception::Exception);

  void ConfigureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  void EnableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception );
  void HaltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception );
  void noAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception );

  void doConfigure();
  void doStart();
  void doStop();

  // Web callback functions
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void dispatch (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
  void Configure(xgi::Input * in ) throw (xgi::exception::Exception);
  void Enable(xgi::Input * in ) throw (xgi::exception::Exception);
  void Halt(xgi::Input * in ) throw (xgi::exception::Exception);
  void InvalidWebAction(xgi::Input * in ) throw (xgi::exception::Exception);
  void stateMachinePage( xgi::Output * out ) throw (xgi::exception::Exception);
  void failurePage(xgi::Output * out, xgi::exception::Exception & e)  throw (xgi::exception::Exception);

  // Process Event data
  void processEvent(const char * data, int dataSize, unsigned long errorFlag, int node=0);

  void updateList(xdata::Integer);
  void updateObjects(xdata::Integer);
   
  xoap::MessageReference requestObjectsList (xoap::MessageReference msg) throw (xoap::exception::Exception);
  xoap::MessageReference requestObjects (xoap::MessageReference msg) throw (xoap::exception::Exception); 
  xoap::MessageReference requestCanvasesList (xoap::MessageReference msg) throw (xoap::exception::Exception);
  xoap::MessageReference requestCanvas (xoap::MessageReference msg) throw (xoap::exception::Exception); 

  // Construct ROOT output file name
  std::string getROOTFileName();
  
 private:
  
  void initProperties(); 
  void defineFSM();
  void defineWebSM();
  void bindI2Ocallbacks();
  void bindSOAPcallbacks();
  void bindCGIcallbacks();

  void getDataServers(xdata::String className);
  void getCollectors(xdata::String className);

  void createDeviceReader();
  void destroyDeviceReader();

  void configureReadout();
  void enableReadout();
  void disableReadout();

  void setMemoryPool();
  void setupPlotter();

  void printParametersTable( xgi::Output * out ) throw (xgi::exception::Exception);
 protected:

  // == Plotter 
  EmuPlotter* plotter_;
  xdata::String xmlHistosBookingCfgFile_;
  xdata::String xmlCanvasesCfgFile_;
  xdata::String outputROOTFile_;
  xdata::String outputImagesPath_;
  xdata::Integer plotterSaveTimer_;
  xdata::UnsignedLong binCheckMask_;
  xdata::UnsignedLong dduCheckMask_;
  xdata::Boolean fSaveROOTFile_;



  EmuMonitorTimerTask * timer_;

  xdata::UnsignedLong   creditMsgsSent_;
  xdata::UnsignedLong   eventsRequested_;
  xdata::UnsignedLong   eventsReceived_;
  xdata::UnsignedLong   creditsHeld_;
  xdata::String		stateName_;
  xdata::String		stateChangeTime_;
  xdata::String		lastEventTime_;
  xdata::UnsignedLong   nDAQEvents_;


  // == Total processed events counter
  xdata::UnsignedLong 	totalEvents_;
  // == Session processed events counter
  xdata::UnsignedLong 	sessionEvents_;

  toolbox::PerformanceMeter * pmeter_;
  toolbox::PerformanceMeter * pmeterCSC_;
  // == Data processing rate
  xdata::String		dataBw_;
  xdata::String         dataLatency_;
  xdata::String 	dataRate_;
  xdata::String		cscRate_;
  xdata::UnsignedLong   cscUnpacked_;
  xdata::UnsignedLong   runNumber_;


  xdata::Boolean 	useAltFileReader_;


  EmuReader*         	deviceReader_;         // device reader
  FileReaderDDU*        altFileReader_;      // alternative device reader
  xdata::String         inputDeviceName_;      // input device name (file path or board number)
  xdata::String         inputDeviceType_;      // spy, slink or file
  xdata::String         inputDataFormat_;      // "DDU" or "DCC"
  int                   inputDataFormatInt_;   // EmuReader::DDU or EmuReader::DCC

  // == Data Readout Modes (external, internal)
  xdata::String 	readoutMode_;
  // == Net Transports for external Readout (i20, soap)
  xdata::String 	transport_; 
  // == Collectors classname
  xdata::String 	collectorsClassName_;
  xdata::Integer	collectorID_;

  // == Total memory for credit messages
  xdata::UnsignedLong 	committedPoolSize_;     
  // == Servers' class name
  xdata::String       	serversClassName_;
  // == Server TIDs
  xdata::Vector<xdata::UnsignedLong> serverTIDs_;
  // == Send this many event credits at a time
  xdata::UnsignedLong 	nEventCredits_;
  // == Prescaling factor for data to be received
  xdata::UnsignedLong 	prescalingFactor_; 

  // == Vector of all external data servers tids
  std::vector<xdaq::ApplicationDescriptor*> dataservers_;
  // == Vector of all collectors tids
  std::vector<xdaq::ApplicationDescriptor*> collectors_; 

  // == The maximum frame size to be allocated by the Client
  xdata::UnsignedLong maxFrameSize_; 
 
  // == Exception handler
  toolbox::exception::HandlerSignature  * errorHandler_;

  // == Memory pool for allocating messages for sending
  toolbox::mem::Pool* pool_;
 
  deque<toolbox::mem::Reference*> dataMessages_; // the queue of data messages waiting to be processed
 
  BSem applicationBSem_;

  // == Application state machine
  toolbox::fsm::FiniteStateMachine fsm_; 
  // == Web dialog state machine
  xgi::WSM wsm_;

  int sTimeout; // Timeout (in secs) waiting for plotter's busy flag to clear
  int appTid_;
  bool isReadoutActive;
};

#endif