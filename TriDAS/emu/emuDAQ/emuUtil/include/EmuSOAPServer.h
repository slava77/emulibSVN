#ifndef _EmuSOAPServer_h_
#define _EmuSOAPServer_h_

#include "xcept/include/xcept/tools.h"

#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"

#include "EmuServer.h"

class EmuSOAPServer : public EmuServer {
private:

  // The message under construction
  xoap::MessageReference                     messageReference_;
  bool                                       messageComplete_;

  // The queue of already complete messages waiting to be transmitted
  deque<xoap::MessageReference>              messages_; // each message carries one event
  unsigned int                               maxMessagesPendingTransmission_; // limit their number

  // To be included in the SOAP message's attachment's MIME part header
  string                                     contentId_;
  string                                     contentType_;
  string                                     contentLocation_;
  string                                     contentEncoding_;

  void   createMessage()
    throw ( xoap::exception::Exception );
  void   createMIMEInfo();
  void   appendData( char* const         data, 
		     const unsigned long dataLength, 
		     const bool          completesEvent )
    throw( xoap::exception::Exception );
  void   fillMessage( char* const        data,
		      const unsigned int dataLength )
    throw( xoap::exception::Exception );

public:
  // clientName must be passed by value to make sure it remains unchanged here
  // even after the it is modified in the parent app's info space
  EmuSOAPServer( xdaq::Application                    *parentApp,
		 const string                          clientName,
		 xdata::Serializable                  *prescaling,
		 xdata::Serializable                  *onRequest,
		 xdata::Serializable                  *creditsHeld,
		 const Logger                         *logger )
    throw( xcept::Exception );
  ~EmuSOAPServer();
  void   addData(  const int  runNumber, 
		   const int  nEvents, 
		   const bool completesEvent, 
		   char* const data, 
		   const int  dataLength );
  void   sendData()
    throw ( xcept::Exception );
  unsigned long getClientTid(){ return( (unsigned long)0 ); }
  xoap::MessageReference getOldestMessagePendingTransmission();
  void printMessages();
};

#endif
