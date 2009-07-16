// $Id: EmuRunControlHyperDAQ.cc,v 3.1 2007/02/26 01:57:02 liu Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include "EmuRunControlHyperDAQ.h"

using namespace cgicc;
using namespace std;

  EmuRunControlHyperDAQ::EmuRunControlHyperDAQ(xdaq::ApplicationStub * s): xdaq::Application(s) 
  {	
    //
    // Bind SOAP callback
    //
    xgi::bind(this,&EmuRunControlHyperDAQ::Default, "Default");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageConfigure, "SendSOAPMessageConfigure");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageInit, "SendSOAPMessageInit");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageInitXRelay, "SendSOAPMessageInitXRelay");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageConfigureXRelay, "SendSOAPMessageConfigureXRelay");
    //xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageOpenFile, "SendSOAPMessageOpenFile");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageConnectTStore, "SendSOAPMessageConnectTStore");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageDisconnectTStore, "SendSOAPMessageDisconnectTStore");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageQueryTStore, "SendSOAPMessageQueryTStore");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageConfigureLTC, "SendSOAPMessageConfigureLTC");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageJobControlExecuteCommand, 
	      "SendSOAPMessageJobControlExecuteCommand");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageQueryLTC, "SendSOAPMessageQueryLTC");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageQueryLTC, "SendSOAPMessageQueryJobControl");
    //
  }  
  //
  void EmuRunControlHyperDAQ::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    //*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    //*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    //*out << cgicc::title("EmuRunControlHyperDAQ") << std::endl;
    //
    MyHeader(in,out,"EmuRunControlHyperDAQ");
    //
    *out << cgicc::h1("EmuRunControlHyperDAQ") << std::endl ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    *out << cgicc::legend("Crates in Configuration file").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    std::set<xdaq::ApplicationDescriptor * >  descriptor =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
    //
    //
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
    for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) 
      {
	std::string classNameStr = (*itDescriptor)->getClassName();
	*out << classNameStr << " " << std::endl ;
	std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
	*out << url << " " << std::endl;
	std::string urn = (*itDescriptor)->getURN();  	
	*out << urn << std::endl;
	//
	xoap::MessageReference reply;
	//
	bool failed = false ;
	//
	try{
	  xoap::MessageReference msg   = QueryPeripheralCrateInfoSpace();
	  reply = getApplicationContext()->postSOAP(msg, (*itDescriptor));
	}
	//
	catch (xdaq::exception::Exception& e) 
	  {
	    *out << cgicc::span().set("style","color:red");
	    *out << "(Not running)"<<std::endl;
	    *out << cgicc::span();
	    failed = true;
	  }
	//
	if(!failed) {
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  if (body.hasFault()) {
	    std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    *out << "(" << extractState(reply) << ")";
	    *out << cgicc::span();
	  }
	}
	//
	*out << cgicc::br();
	//
      }    
    //
    *out << cgicc::fieldset() ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    *out << cgicc::legend("XRelays in Configuration file").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    std::set<xdaq::ApplicationDescriptor * >  descriptorXRelay =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("XRelay");
    //
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptorXRelay;
    for ( itDescriptorXRelay = descriptorXRelay.begin(); itDescriptorXRelay != descriptorXRelay.end(); itDescriptorXRelay++ ) 
      {
	std::string classNameStr = (*itDescriptorXRelay)->getClassName();
	*out << classNameStr << " " << std::endl ;
	std::string url = (*itDescriptorXRelay)->getContextDescriptor()->getURL();
	*out << url << " " << std::endl;
	std::string urn = (*itDescriptorXRelay)->getURN();  	
	*out << urn << std::endl;
	//
	*out << cgicc::br();
	//
      }    
    //
    *out << cgicc::fieldset() ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    *out << cgicc::legend("TStore in Configuration file").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    std::set<xdaq::ApplicationDescriptor * >  descriptorTStore =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("TStore");
    //
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptorTStore;
    for ( itDescriptorTStore = descriptorTStore.begin(); itDescriptorTStore != descriptorTStore.end(); itDescriptorTStore++ ) 
      {
	std::string classNameStr = (*itDescriptorTStore)->getClassName();
	*out << classNameStr << " " << std::endl ;
	std::string url = (*itDescriptorTStore)->getContextDescriptor()->getURL();
	*out << url << " " << std::endl;
	std::string urn = (*itDescriptorTStore)->getURN();  	
	*out << urn << std::endl;
	//
	*out << cgicc::br();
	//
      }    
    //
    *out << cgicc::fieldset() ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    *out << cgicc::legend("JobControl in Configuration file").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    std::set<xdaq::ApplicationDescriptor * >  descriptorJobControl =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("JobControl");
    //
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptorJobControl;
    for ( itDescriptorJobControl = descriptorJobControl.begin(); itDescriptorJobControl != descriptorJobControl.end(); itDescriptorJobControl++ ) 
      {
	std::string classNameStr = (*itDescriptorJobControl)->getClassName();
	*out << classNameStr << " " << std::endl ;
	std::string url = (*itDescriptorJobControl)->getContextDescriptor()->getURL();
	*out << url << " " << std::endl;
	std::string urn = (*itDescriptorJobControl)->getURN();  	
	*out << urn << std::endl;
	//
	xoap::MessageReference reply;
	//
	bool failed = false ;
	//
	try{
	  xoap::MessageReference msg   = QueryJobControlInfoSpace();
	  reply = getApplicationContext()->postSOAP(msg, (*itDescriptorJobControl));
	}
	//
	catch (xdaq::exception::Exception& e) 
	  {
	    *out << cgicc::span().set("style","color:red");
	    *out << "(Not running)"<<std::endl;
	    *out << cgicc::span();
	    failed = true;
	  }
	//
	if(!failed) {
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  if (body.hasFault()) {
	    std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    *out << "(" << extractState(reply) << ")";
	    *out << cgicc::span();
	    //
	    *out << cgicc::br();
	    //
	  }
	}
	//
	*out << cgicc::br();
	//
      }    
    //
    *out << cgicc::fieldset() ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    *out << cgicc::legend("LTCControl in Configuration file").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    std::set<xdaq::ApplicationDescriptor * >  descriptorLTCControl =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("LTCControl");
    //
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptorLTCControl;
    for ( itDescriptorLTCControl = descriptorLTCControl.begin(); 
	  itDescriptorLTCControl != descriptorLTCControl.end(); itDescriptorLTCControl++ ) 
      {
	//
	std::string classNameStr = (*itDescriptorLTCControl)->getClassName();
	*out << classNameStr << " " << std::endl ;
	std::string url = (*itDescriptorLTCControl)->getContextDescriptor()->getURL();
	*out << url << " " << std::endl;
	std::string urn = (*itDescriptorLTCControl)->getURN();  	
	*out << urn << std::endl;
	//
	xoap::MessageReference reply;
	//
	bool failed = false ;
	//
	try{
	  xoap::MessageReference msg   = QueryLTCInfoSpace();
	  reply = getApplicationContext()->postSOAP(msg, (*itDescriptorLTCControl));
	}
	//
	catch (xdaq::exception::Exception& e) 
	  {
	    *out << cgicc::span().set("style","color:red");
	    *out << "(Not running)"<<std::endl;
	    *out << cgicc::span();
	    failed = true;
	  }
	//
	if(!failed) {
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  if (body.hasFault()) {
	    std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
	  } else {
	    *out << cgicc::span().set("style","color:green");
	    *out << "(" << extractState(reply) << ")";
	    *out << cgicc::span();
	    //
	    *out << cgicc::br();
	    *out << "Run number = " << extractRunNumber(reply) << std::endl;
	    //
	  }
	}
	//
	*out << cgicc::br();
	//
      }    
    //
    *out << cgicc::fieldset() ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    /*
    std::string methodSOAPMessageInit =
      toolbox::toString("/%s/SendSOAPMessageInitXRelay",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageInit) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Init Crate") << std::endl ;
    *out << cgicc::form();
    */
    //
    std::string methodSOAPMessageConfigure =
      toolbox::toString("/%s/SendSOAPMessageConfigureXRelay",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageConfigure) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Configure Crate") << std::endl ;
    *out << cgicc::form();
    /*
    std::string methodSOAPMessageOpenFile =
      toolbox::toString("/%s/SendSOAPMessageOpenFile",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageOpenFile) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Open File") << std::endl ;
    *out << cgicc::form();
    */
    *out << cgicc::fieldset();
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    std::string methodSOAPMessageConnectTStore =
      toolbox::toString("/%s/SendSOAPMessageConnectTStore",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageConnectTStore) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Connect TStore") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageDisconnectTStore =
      toolbox::toString("/%s/SendSOAPMessageDisconnectTStore",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageDisconnectTStore) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Disconnect TStore") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageQueryTStore =
      toolbox::toString("/%s/SendSOAPMessageQueryTStore",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageQueryTStore) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Query TStore") << std::endl ;
    *out << cgicc::form();
    //
    *out << cgicc::fieldset() ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    std::string methodSOAPMessageConfigureLTC =
      toolbox::toString("/%s/SendSOAPMessageConfigureLTC",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageConfigureLTC) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Configure LTC") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageQueryLTC =
      toolbox::toString("/%s/SendSOAPMessageQueryLTC",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageQueryLTC) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Query LTC InfoSpace") << std::endl ;
    *out << cgicc::form();
    //
    *out << cgicc::fieldset() ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    std::string methodSOAPMessageJobControlExecuteCommand =
      toolbox::toString("/%s/SendSOAPMessageJobControlExecuteCommand",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageJobControlExecuteCommand) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : JobControl Execute Command") << std::endl ;
    *out << cgicc::form();
    //
    *out << cgicc::fieldset() ;
    //
  }
  //
  void EmuRunControlHyperDAQ::MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) 
    throw (xgi::exception::Exception)
    {
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title(title) << std::endl;
    *out << "<a href=\"/\"><img border=\"0\" src=\"/daq/xgi/images/XDAQLogo.gif\" title=\"XDAQ\" alt=\"\" style=\"width: 145px; height: 89px;\"></a>" << std::endl;
    //
  }
  //
  //
  string EmuRunControlHyperDAQ::extractState(xoap::MessageReference message)
    {
      xoap::SOAPElement root = message->getSOAPPart()
	.getEnvelope().getBody().getChildElements(*(new xoap::SOAPName("ParameterGetResponse", "", "")))[0];
      xoap::SOAPElement properties = root.getChildElements(*(new xoap::SOAPName("properties", "", "")))[0];
      xoap::SOAPElement state = properties.getChildElements(*(new xoap::SOAPName("stateName", "", "")))[0];
      
      return state.getValue();
    }
  //
  string EmuRunControlHyperDAQ::extractRunNumber(xoap::MessageReference message)
    {
      xoap::SOAPElement root = message->getSOAPPart()
	.getEnvelope().getBody().getChildElements(*(new xoap::SOAPName("ParameterGetResponse", "", "")))[0];
      xoap::SOAPElement properties = root.getChildElements(*(new xoap::SOAPName("properties", "", "")))[0];
      xoap::SOAPElement state = properties.getChildElements(*(new xoap::SOAPName("RunNumber", "", "")))[0];
      
      return state.getValue();
    }
  // 
  // Create a XRelay SOAP Message
  //
  xoap::MessageReference EmuRunControlHyperDAQ::QueryLTCInfoSpace()
    {
      xoap::MessageReference message = xoap::createMessage();
      xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
      envelope.addNamespaceDeclaration("xsi", "http://www.w3.org/2001/XMLSchema-instance");
      
      xoap::SOAPName command = envelope.createName("ParameterGet", "xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPName properties = envelope.createName("properties", "LTCControl", "urn:xdaq-application:LTCControl");
      xoap::SOAPName parameter  = envelope.createName("stateName", "LTCControl", "urn:xdaq-application:LTCControl");
      xoap::SOAPName parameter2 = envelope.createName("RunNumber", "LTCControl", "urn:xdaq-application:LTCControl");
      xoap::SOAPName xsitype    = envelope.createName("type", "xsi", "http://www.w3.org/2001/XMLSchema-instance");
      
      xoap::SOAPElement properties_e = envelope.getBody()
	.addBodyElement(command)
	.addChildElement(properties);
      properties_e.addAttribute(xsitype, "soapenc:Struct");
      
      xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
      parameter_e.addAttribute(xsitype, "xsd:string");

      xoap::SOAPElement parameter_e2 = properties_e.addChildElement(parameter2);
      parameter_e2.addAttribute(xsitype, "xsd:unsignedLong");      

      return message;
    }
  //
  xoap::MessageReference EmuRunControlHyperDAQ::QueryPeripheralCrateInfoSpace()
    {
      xoap::MessageReference message = xoap::createMessage();
      xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
      envelope.addNamespaceDeclaration("xsi", "http://www.w3.org/2001/XMLSchema-instance");
      
      xoap::SOAPName command = envelope.createName("ParameterGet", "xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPName properties = envelope.createName("properties", "EmuPeripheralCrate", "urn:xdaq-application:EmuPeripheralCrate");
      xoap::SOAPName parameter  = envelope.createName("stateName", "EmuPeripheralCrate", "urn:xdaq-application:EmuPeripheralCrate");
      xoap::SOAPName xsitype    = envelope.createName("type", "xsi", "http://www.w3.org/2001/XMLSchema-instance");
      
      xoap::SOAPElement properties_e = envelope.getBody()
	.addBodyElement(command)
	.addChildElement(properties);
      properties_e.addAttribute(xsitype, "soapenc:Struct");
      
      xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
      parameter_e.addAttribute(xsitype, "xsd:string");

      return message;
    }
  //
  xoap::MessageReference EmuRunControlHyperDAQ::QueryJobControlInfoSpace()
    {
      xoap::MessageReference message = xoap::createMessage();
      xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
      envelope.addNamespaceDeclaration("xsi", "http://www.w3.org/2001/XMLSchema-instance");
      
      xoap::SOAPName command = envelope.createName("ParameterGet", "xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPName properties = envelope.createName("properties", "JobControl", "urn:xdaq-application:JobControl");
      xoap::SOAPName parameter  = envelope.createName("stateName", "JobControl", "urn:xdaq-application:JobControl");
      xoap::SOAPName xsitype    = envelope.createName("type", "xsi", "http://www.w3.org/2001/XMLSchema-instance");
      
      xoap::SOAPElement properties_e = envelope.getBody()
	.addBodyElement(command)
	.addChildElement(properties);
      properties_e.addAttribute(xsitype, "soapenc:Struct");
      
      xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
      parameter_e.addAttribute(xsitype, "xsd:string");

      return message;
    }
  //
  xoap::MessageReference EmuRunControlHyperDAQ::createXRelayMessage(const std::string & command, std::set<xdaq::ApplicationDescriptor * > descriptor )
  {
    // Build a SOAP msg with the Xrelay header:
    xoap::MessageReference msg  = xoap::createMessage();
    //
    std::string topNode = "relay";
    std::string prefix = "xr";
    std::string httpAdd = "http://xdaq.web.cern.ch/xdaq/xsd/2004/XRelay-10";
    xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
    xoap::SOAPName envelopeName = envelope.getElementName();
    xoap::SOAPHeader header = envelope.addHeader();
    xoap::SOAPName relayName = envelope.createName(topNode, prefix,  httpAdd);
    xoap::SOAPHeaderElement relayElement = header.addHeaderElement(relayName);
    
    // Add the actor attribute
    xoap::SOAPName actorName = envelope.createName("actor", envelope.getElementName().getPrefix(),
						   envelope.getElementName().getURI());
    relayElement.addAttribute(actorName,httpAdd);
    
    // Add the "to" node
    std::string childNode = "to";
    // Send to all the destinations:
    //
    std::set <xdaq::ApplicationDescriptor * >  descriptorsXrelays =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("XRelay");
    //
    std::cout << "descriptorXrelays size = " << descriptorsXrelays.size() << std::endl;
    //
    std::set<xdaq::ApplicationDescriptor * >::iterator  itDescriptorsXrelays = descriptorsXrelays.begin();
    std::set <xdaq::ApplicationDescriptor *>::iterator itDescriptor;

    for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) 
      {
	std::string classNameStr = (*itDescriptor)->getClassName();
	//
	std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
	std::string urn = (*itDescriptor)->getURN();  	
	//
        std::string urlXRelay = (*itDescriptorsXrelays)->getContextDescriptor()->getURL();
        std::string urnXRelay = (*itDescriptorsXrelays)->getURN();
        itDescriptorsXrelays++;
        if (itDescriptorsXrelays ==  descriptorsXrelays.end()) itDescriptorsXrelays=descriptorsXrelays.begin();
	//
	xoap::SOAPName toName = envelope.createName(childNode, prefix, " ");
	xoap::SOAPElement childElement = relayElement.addChildElement(toName);
	xoap::SOAPName urlName = envelope.createName("url");
	xoap::SOAPName urnName = envelope.createName("urn");
	childElement.addAttribute(urlName,urlXRelay);
	childElement.addAttribute(urnName,urnXRelay);
	xoap::SOAPElement childElement2 = childElement.addChildElement(toName);
	childElement2.addAttribute(urlName,url);
	childElement2.addAttribute(urnName,urn);
	//
      }
    //
    // Create body
    //
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName cmd = envelope.createName(command,"xdaq","urn:xdaq-soap:3.0");
    body.addBodyElement(cmd);
    
    msg->writeTo(std::cout);
	
    return msg;
    
  }
  
  // Post XRelay SOAP message to XRelay application
  void EmuRunControlHyperDAQ::relayMessage (xoap::MessageReference msg) throw (xgi::exception::Exception)
  {
    // Retrieve the list of applications expecting this command and build the XRelay header
    
    try 
      {	
	// Get the Xrelay application descriptor and post the message:
	xdaq::ApplicationDescriptor * xrelay = getApplicationContext()->
	  getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),4);
	
	xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, xrelay);
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	if (body.hasFault()) {
	  std::cout << "No connection. " << body.getFault().getFaultString() << std::endl;
	}
      } 
    catch (xdaq::exception::Exception& e) 
      {
	XCEPT_RETHROW (xgi::exception::Exception, "Cannot relay message", e);
      }
  }
  //
  void EmuRunControlHyperDAQ::SendSOAPMessageConnectTStore(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout << "Send SOAP message connect tstore" <<std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      try {
	xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
	xoap::SOAPName msgName = envelope.createName( "connect", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
	xoap::SOAPElement queryElement = envelope.getBody().addBodyElement ( msgName );
	
	xoap::SOAPName id = envelope.createName("id", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
	queryElement.addAttribute(id, "myTStore");
	xoap::SOAPName passwordName = envelope.createName("password", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
	queryElement.addAttribute(passwordName, "alct2_emu");       
      }
      catch(xoap::exception::Exception& e) {
	std::cout << "Got exception 1" <<std::endl;
      }
      
      try {
	xdaq::ApplicationDescriptor * tstoreDescriptor = getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),400);
	xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, tstoreDescriptor);
	//
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	if (body.hasFault()) {
	  std::cout << "No connection to TStore. " << body.getFault().getFaultString() << std::endl;
	}
      } 
      catch (xdaq::exception::Exception& e) {
	std::cout << "Didn't find TStore" <<std::endl;
      }
      //
      this->Default(in,out);      
      //
    }
  //
  void EmuRunControlHyperDAQ::SendSOAPMessageDisconnectTStore(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout << "Send SOAP message disconnect tstore" <<std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      try {
	xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
	xoap::SOAPName msgName = envelope.createName( "disconnect", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
	xoap::SOAPElement queryElement = envelope.getBody().addBodyElement ( msgName );
	
	xoap::SOAPName id = envelope.createName("id", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
	queryElement.addAttribute(id, "myTStore");
	xoap::SOAPName passwordName = envelope.createName("password", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
	queryElement.addAttribute(passwordName, "alct2_emu");       
      }
      catch(xoap::exception::Exception& e) {
	std::cout << "Got exception 1" <<std::endl;
      }
      
      try {
	xdaq::ApplicationDescriptor * tstoreDescriptor = getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),400);
	xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, tstoreDescriptor);
	//
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	if (body.hasFault()) {
	  std::cout << "No connection to TStore" <<std::endl;
	}
      } 
      catch (xdaq::exception::Exception& e) {
	std::cout << "Didn't find TStore" <<std::endl;
      }
      //
      this->Default(in,out);      
      //
    }
  //
  void EmuRunControlHyperDAQ::SendSOAPMessageQueryTStore(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout <<"Send SOAP query" << std::endl;
      //
      *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
      //
      *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
      *out << cgicc::title("EmuRunControlHyperDAQ") << std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      try {
  	xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  	xoap::SOAPName msgName = envelope.createName( "query", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
  	xoap::SOAPElement queryElement = envelope.getBody().addBodyElement ( msgName );
	//
  	xoap::SOAPName id = envelope.createName("id", "tstore", "http://xdaq.web.cern.ch/xdaq/xsd/2006/tstore-10.xsd");
  	queryElement.addAttribute(id, "myTStore");
	//
	//add the parameters to the message
	queryElement.addNamespaceDeclaration("sql",  "urn:tstore-view-SQL");
	xoap::SOAPName property = envelope.createName("name", "sql","urn:tstore-view-SQL");
	queryElement.addAttribute(property, "myTable1");
      }
      catch(xoap::exception::Exception& e) {
	std::cout << "Got exception 1" << std::endl;
      }
      
      try {
	xdaq::ApplicationDescriptor * tstoreDescriptor = getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),400);
	//
	xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, tstoreDescriptor);
	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	if (body.hasFault()) {
	  std::cout << "Wrong query. " << body.getFault().getFaultString() << std::endl;
	} else {
	  std::cout <<"Good query" << std::endl;
	  //
	  std::list<xoap::AttachmentPart*> attachments = reply->getAttachments();
	  //
	  std::list<xoap::AttachmentPart*>::iterator j;
	  for ( j = attachments.begin(); j != attachments.end(); j++ )
	    {//
	      if ((*j)->getContentType() == "application/xdata+table")
		{
		  xdata::exdr::FixedSizeInputStreamBuffer inBuffer((*j)->getContent(),(*j)->getSize());
		  std::string contentEncoding = (*j)->getContentEncoding();
		  std::string contentId = (*j)->getContentId();
		  //
		  xdata::Table table;                            
		  try 
		    {
		      xdata::exdr::Serializer serializer;
		      serializer.import(&table, &inBuffer );
		      OutputTable(out,table);
		    }
		  catch(xdata::exception::Exception & e )
		    {
		      // failed to import table
		      std::cout << "Failed to import table" << std::endl;
		    }
		}
	      else
		{
		  // unknown attachment type 
		  std::cout << "Unknown attachment type" <<std::endl;
		}
	    }
	}
      }
      catch (xdaq::exception::Exception& e) {
	std::cout << "Didn't find TStore" << std::endl;
      }
      //
      this->Default(in,out);      
      //
    }
  //
  void EmuRunControlHyperDAQ::OutputTable(xgi::Output * out,xdata::Table &results) {
    std::vector<std::string> columns=results.getColumns();
    vector<std::string>::iterator columnIterator;
    *out << results.getRowCount() << " rows";
    *out << cgicc::table().set("border","2");
    *out << cgicc::tr();
    for(columnIterator=columns.begin(); columnIterator!=columns.end(); columnIterator++) {
      *out << cgicc::td() << *columnIterator << " (" << results.getColumnType(*columnIterator) << ")" << cgicc::td();
    }
    *out << cgicc::tr();
    unsigned long rowIndex;
    for (rowIndex=0;rowIndex<results.getRowCount();rowIndex++ ) {
      *out << cgicc::tr();
      for(columnIterator=columns.begin(); columnIterator!=columns.end(); columnIterator++) {
	*out << cgicc::td() << results.getValueAt(rowIndex,*columnIterator)->toString() << cgicc::td();
	
      }
      *out << cgicc::tr();
    }
    *out << cgicc::table();
  }
  /*
  void EmuRunControlHyperDAQ::SendSOAPMessageOpenFile(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "SendSOAPMessage Open File" << std::endl;
    //
    xoap::MessageReference msg = xoap::createMessage();
    xoap::SOAPPart soap = msg->getSOAPPart();
    xoap::SOAPEnvelope envelope = soap.getEnvelope();
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName command = envelope.createName("OpenFile","xdaq", "urn:xdaq-soap:3.0");
    body.addBodyElement(command);
    //
    try
      {	
	xdaq::ApplicationDescriptor * d = 
	  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor("DDUHyperDAQ",0);
	xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
      } 
    catch (xdaq::exception::Exception& e)
      {
	XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
      }
    //
    this->Default(in,out);
    //
  }
  */
  void EmuRunControlHyperDAQ::SendSOAPMessageConfigureLTC(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout << "SendSOAPMessage Configure LTC" << std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      xoap::SOAPPart soap = msg->getSOAPPart();
      xoap::SOAPEnvelope envelope = soap.getEnvelope();
      xoap::SOAPBody body = envelope.getBody();
      xoap::SOAPName command = envelope.createName("Configure","xdaq", "urn:xdaq-soap:3.0");
      body.addBodyElement(command);
      //
      try
	{	
	  xdaq::ApplicationDescriptor * d = 
	    getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor("LTCControl", 0);
	  xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  reply->writeTo(std::cout);
	  std::cout << std::endl;
	  if (body.hasFault()) {
	    std::cout << "Fault = " << body.getFault().getFaultString() << std::endl;
	  }
	  //
	} 
      catch (xdaq::exception::Exception& e)
	{
	  XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
	}
      //
      this->Default(in,out);
      //
    }
  //
  void EmuRunControlHyperDAQ::SendSOAPMessageJobControlExecuteCommand(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout << "SendSOAPMessage JobControl executeCommand" << std::endl;
      //
      xoap::MessageReference msg = xoap::createMessage();
      xoap::SOAPPart soap = msg->getSOAPPart();
      xoap::SOAPEnvelope envelope = soap.getEnvelope();
      xoap::SOAPBody body = envelope.getBody();
      xoap::SOAPName command = envelope.createName("executeCommand","xdaq", "urn:xdaq-soap:3.0");
      xoap::SOAPName user     = envelope.createName("user", "", "http://www.w3.org/2001/XMLSchema-instance");
      xoap::SOAPName execPath = envelope.createName("execPath", "", "http://www.w3.org/2001/XMLSchema-instance");
      xoap::SOAPBodyElement itm = body.addBodyElement(command);
      itm.addAttribute(execPath,"/home/meydev/DAQkit/3.9/TriDAS/daq/xdaq/bin/linux/x86/xdaq.exe");
      itm.addAttribute(user,"mey");
      //
      try
	{
	  //
	  msg->writeTo(std::cout);
	  std::cout << std::endl;
	  //
	  xdaq::ApplicationDescriptor * d = 
	    getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor("JobControl",0);
	  std::cout << d << std::endl;
	  xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  reply->writeTo(std::cout);
	  std::cout << std::endl;
	  if (body.hasFault()) {
	    std::cout << "Fault = " << body.getFault().getFaultString() << std::endl;
	  }
	  //
	} 
      catch (xdaq::exception::Exception& e)
	{
	  XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
	}
      //
      this->Default(in,out);
      //
    }
  //
  void EmuRunControlHyperDAQ::SendSOAPMessageQueryLTC(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
    {
      //
      std::cout << "SendSOAPMessage Query LTC" << std::endl;
      //
      xoap::MessageReference queryLTC = QueryLTCInfoSpace();
      //
      try
	{	
	  xdaq::ApplicationDescriptor * d = 
	    getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor("LTCControl", 0);
	  xoap::MessageReference reply    = getApplicationContext()->postSOAP(queryLTC, d);
	  xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();
	  std::cout << "Reply" << std::endl;
	  reply->writeTo(std::cout);
	  std::cout << std::endl;
	  if (body.hasFault()) {
	    std::cout << "Fault = " << body.getFault().getFaultString() << std::endl;
	  }
	  //
	} 
      catch (xdaq::exception::Exception& e)
	{
	  XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
	}
      //
      this->Default(in,out);
      //
    }
  //
  void EmuRunControlHyperDAQ::SendSOAPMessageInit(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "SendSOAPMessage Init" << std::endl;
    //
    xoap::MessageReference msg = xoap::createMessage();
    xoap::SOAPPart soap = msg->getSOAPPart();
    xoap::SOAPEnvelope envelope = soap.getEnvelope();
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName command = envelope.createName("Init","xdaq", "urn:xdaq-soap:3.0");
    body.addBodyElement(command);
    //
    try
      {	
	xdaq::ApplicationDescriptor * d = 
	  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor("EmuPeripheralCrate", 0);
	xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
	//
	d = 
	  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor("EmuPeripheralCrate", 1);
	reply    = getApplicationContext()->postSOAP(msg, d);
      } 
    catch (xdaq::exception::Exception& e)
      {
	XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
      }
    //
    this->Default(in,out);
    //
  }
  //
  void EmuRunControlHyperDAQ::SendSOAPMessageInitXRelay(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "SendSOAPMessageInitXRelay" <<std::endl;
    //
    std::set <xdaq::ApplicationDescriptor * >  descriptors =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
    //
    std::cout << "SendSOAPMessageInitXRelay 2" <<std::endl;
    //
    std::cout << "Size = " << descriptors.size() << std::endl;
    //
    xoap::MessageReference configure = createXRelayMessage("Init", descriptors);
    //
    this->relayMessage(configure);
    // 
    this->Default(in,out);
    //
  }
  //
  void EmuRunControlHyperDAQ::SendSOAPMessageConfigureXRelay(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::set <xdaq::ApplicationDescriptor * >  descriptors =
      getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptors("EmuPeripheralCrate");
    //
    xoap::MessageReference configure = createXRelayMessage("Configure", descriptors);
    //
    this->relayMessage(configure);
    //
    this->Default(in,out);
    //
  }
  //
  void EmuRunControlHyperDAQ::SendSOAPMessageConfigure(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "SendSOAPMessage Configure" << std::endl;
    //
    xoap::MessageReference msg = xoap::createMessage();
    xoap::SOAPPart soap = msg->getSOAPPart();
    xoap::SOAPEnvelope envelope = soap.getEnvelope();
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName command = envelope.createName("Configure","xdaq", "urn:xdaq-soap:3.0");
    body.addBodyElement(command);
    //
    try
      {	
	xdaq::ApplicationDescriptor * d = 
	  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor("EmuPeripheralCrate", 0);
	xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
	//
	d = 
	  getApplicationContext()->getDefaultZone()->getApplicationGroup("default")->getApplicationDescriptor("EmuPeripheralCrate", 1);
	reply    = getApplicationContext()->postSOAP(msg, d);
	//
      } 
    catch (xdaq::exception::Exception& e)
      {
	XCEPT_RETHROW (xgi::exception::Exception, "Cannot send message", e);	      	
      }
    //
    this->Default(in,out);
    //
  }
  //  
//
// provides factory method for instantion of SimpleSOAPSender application
//
XDAQ_INSTANTIATOR_IMPL(EmuRunControlHyperDAQ)
  //