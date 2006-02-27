#ifndef _EmuServer_h_
#define _EmuServer_h_

#include <string>
#include "log4cplus/logger.h"
#include "xdaq/include/xdaq/ApplicationGroup.h"
#include "xdaq/include/xdaq/WebApplication.h"
#include "xdata/include/xdata/InfoSpace.h"
#include "xdata/include/xdata/String.h"
#include "xdata/include/xdata/UnsignedLong.h"
#include "xdata/include/xdata/Boolean.h"


#include "xoap/MessageReference.h"

// #include "log4cplus/logger.h"
// #include "i2o/Method.h"
// #include "xcept/tools.h"
// #include "toolbox/mem/CommittedHeapAllocator.h"
// #include "i2o/i2oDdmLib.h"
// #include "xdaq/ApplicationGroup.h"
// #include "xdaq/WebApplication.h"
// #include "xdata/InfoSpace.h"
// #include "xdata/String.h"
// #include "xdata/UnsignedLong.h"
// #include "xdata/Boolean.h"
// #include "toolbox/mem/MemoryPoolFactory.h"
// #include "i2o/utils/AddressMap.h"

class EmuServer {
protected:

  xdaq::Application                          *parentApp_;

  string                                      name_;
  xdaq::ApplicationDescriptor                *appDescriptor_;
  xdaq::ApplicationContext                   *appContext_;
  xdaq::ApplicationGroup                     *appGroup_;

  string                                      clientName_;
  xdaq::ApplicationDescriptor                *clientDescriptor_;


  xdata::UnsignedLong                        *prescaling_;
  xdata::Boolean                             *sendDataOnRequestOnly_;// if true, data is sent only upon request
  xdata::UnsignedLong                        *nEventCreditsHeld_;       // number of events (from client) yet to be sent
  bool                                        dataIsPendingTransmission_;

  Logger                                      logger_;

  int                                         runNumber_;

  string         createName();
  void           findClientDescriptor();

public:
  // clientName must be passed by value to make sure it remains unchanged here
  // even after the it is modified in the parent app's info space
  EmuServer( xdaq::Application                    *parentApp,
	     const string                          clientName,
	     xdata::Serializable                  *prescaling,
	     xdata::Serializable                  *onRequest,
	     xdata::Serializable                  *creditsHeld,
	     const Logger                         *logger )
    throw( xcept::Exception );
  virtual ~EmuServer(){}
  string getClientName(){ return clientName_; }
  xdaq::ApplicationDescriptor* getClientDescriptor(){ return clientDescriptor_; }
  virtual unsigned long getClientTid()=0;
  void   addCredits( const int nCredits, const int prescaling );
  virtual void   addData(  const int  runNumber, 
			   const int  nEvents, 
			   const bool completesEvent, 
			   char* const data, 
			   const int  dataLength )=0;
//     throw ( xcept::Exception )=0;
  virtual void   sendData()
    throw ( xcept::Exception )=0;
  bool   dataIsPendingTransmission(){ return dataIsPendingTransmission_; }
  virtual xoap::MessageReference getOldestMessagePendingTransmission()=0;
};

#endif
