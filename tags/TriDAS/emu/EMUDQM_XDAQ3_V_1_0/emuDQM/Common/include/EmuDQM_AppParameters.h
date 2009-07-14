#ifndef EmuDQM_AppParameters_h
#define EmuDQM_AppParameters_h

#include "xdaq.h"
#include "toolbox.h"
#include "xdata.h"

#include "xdaq.h"
#include "xgi/Method.h"
#include "i2o/Method.h"
#include "i2o/utils/AddressMap.h"
#include "pt/PeerTransportAgent.h"
#include "xoap/Method.h"
#include "xdaq/NamespaceURI.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"
#include "xcept/tools.h"


namespace emu {
  namespace dqm {
    DOMNode *findNode
      (
       DOMNodeList *nodeList,
       const std::string nodeLocalName
       )
       throw (xcept::Exception);

    /**
     * Gets and returns the value of the specified parameter from the specified
     * application.
     */

    std::string getScalarParam
      (
       xdaq::ApplicationContext *appContext_,
       xdaq::ApplicationDescriptor* appDescriptor,
       const std::string                 paramName,
       const std::string                 paramType
       );
      // throw (xcept::Exception);
    /**
     * Sets the specified parameter of the specified application to the
     * specified value.
     */
    void setScalarParam
      (
       xdaq::ApplicationContext *appContext_,       
       xdaq::ApplicationDescriptor* appDescriptor,
       const std::string                 paramName,
       const std::string                 paramType,
       const std::string                 paramValue
       );
      // throw (xcept::Exception);

    /**
     * Creates a ParameterGet SOAP message.
     */
    xoap::MessageReference createParameterGetSOAPMsg
      (
       const std::string appClass,
       const std::string paramName,
       const std::string paramType
       )
      throw (xcept::Exception);

    /**
     * Creates a ParameterSet SOAP message.
     */
    xoap::MessageReference createParameterSetSOAPMsg
      (
       const std::string appClass,
       const std::string paramName,
       const std::string paramType,
       const std::string paramValue
       )
      throw (xcept::Exception);

    /**
     * Returns the value of the specified parameter from the specified SOAP
     * message.
     */
    std::string extractScalarParameterValueFromSoapMsg
      (
       xoap::MessageReference msg,
       const std::string           paramName
       );
      // throw (xcept::Exception);
  }
}

#endif