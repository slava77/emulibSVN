// $Id: EmuRunControlHyperDAQ.h,v 3.2 2006/08/15 15:59:45 mey Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuRunControlHyperDAQ_h_
#define _EmuRunControlHyperDAQ_h_

#include "xgi/Utils.h"
#include "xgi/Method.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"


#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "xdata/exdr/FixedSizeInputStreamBuffer.h"
#include "xdata/exdr/AutoSizeOutputStreamBuffer.h"
#include "xdata/exdr/Serializer.h"
#include <xdata/String.h>
#include <xdata/Float.h>
#include <xdata/Double.h>
#include <xdata/Integer.h>
#include <xdata/Boolean.h>
#include <xdata/UnsignedLong.h>
#include <xdata/Table.h>


using namespace cgicc;
using namespace std;

class EmuRunControlHyperDAQ: public xdaq::Application
{
  
public:
  
  XDAQ_INSTANTIATOR();
  
  EmuRunControlHyperDAQ(xdaq::ApplicationStub * s): xdaq::Application(s) 
  {	
    //
    // Bind SOAP callback
    //
    xgi::bind(this,&EmuRunControlHyperDAQ::Default, "Default");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageConfigure, "SendSOAPMessageConfigure");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageInit, "SendSOAPMessageInit");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageInitXRelay, "SendSOAPMessageInitXRelay");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageConfigureXRelay, "SendSOAPMessageConfigureXRelay");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageOpenFile, "SendSOAPMessageOpenFile");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageConnectTStore, "SendSOAPMessageConnectTStore");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageDisconnectTStore, "SendSOAPMessageDisconnectTStore");
    xgi::bind(this,&EmuRunControlHyperDAQ::SendSOAPMessageQueryTStore, "SendSOAPMessageQueryTStore");
    //
  }  
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
  {
    //
    *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
    //
    *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
    *out << cgicc::title("EmuRunControlHyperDAQ") << std::endl;
    //
    *out << cgicc::h1("EmuRunControlHyperDAQ") << std::endl ;
    //
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
    //
    *out << cgicc::legend("Crates in Configuration file").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    std::vector<xdaq::ApplicationDescriptor * >  descriptor =
      getApplicationContext()->getApplicationGroup()->getApplicationDescriptors("EmuPeripheralCrate");
    //
    //
    vector <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
    for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) 
      {
	std::string classNameStr = (*itDescriptor)->getClassName();
	*out << classNameStr << " " << std::endl ;
	std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
	*out << url << " " << std::endl;
	std::string urn = (*itDescriptor)->getURN();  	
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
    *out << cgicc::legend("XRelays in Configuration file").set("style","color:blue") 
	 << cgicc::p() << std::endl ;
    //
    std::vector<xdaq::ApplicationDescriptor * >  descriptorXRelay =
      getApplicationContext()->getApplicationGroup()->getApplicationDescriptors("XRelay");
    //
    vector <xdaq::ApplicationDescriptor *>::iterator itDescriptorXRelay;
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
    std::string methodSOAPMessageInit =
      toolbox::toString("/%s/SendSOAPMessageInitXRelay",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageInit) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Init Crate") << std::endl ;
    *out << cgicc::form();
    //
    //
    std::string methodSOAPMessageConfigure =
      toolbox::toString("/%s/SendSOAPMessageConfigureXRelay",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageConfigure) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Configure Crate") << std::endl ;
    *out << cgicc::form();
    //
    std::string methodSOAPMessageOpenFile =
      toolbox::toString("/%s/SendSOAPMessageOpenFile",getApplicationDescriptor()->getURN().c_str());
    //
    *out << cgicc::form().set("method","GET").set("action",methodSOAPMessageOpenFile) << std::endl ;
    *out << cgicc::input().set("type","submit")
      .set("value","Send SOAP message : Open File") << std::endl ;
    *out << cgicc::form();
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
  }
  //
  // Create a XRelay SOAP Message
  //
  xoap::MessageReference createXRelayMessage(const std::string & command, std::vector<xdaq::ApplicationDescriptor * > descriptor )
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
    std::vector<xdaq::ApplicationDescriptor * >  descriptorsXrelays =
      getApplicationContext()->getApplicationGroup()->getApplicationDescriptors("XRelay");
    //
    std::cout << "descriptorXrelays size = " << descriptorsXrelays.size() << std::endl;
    //
    int location = -1;
    //
    vector <xdaq::ApplicationDescriptor *>::iterator itDescriptor;
    for ( itDescriptor = descriptor.begin(); itDescriptor != descriptor.end(); itDescriptor++ ) 
      {
	//
	location++;
	//
	std::string classNameStr = (*itDescriptor)->getClassName();
	//
	std::string url = (*itDescriptor)->getContextDescriptor()->getURL();
	std::string urn = (*itDescriptor)->getURN();  	
	//
	int XRelaySize = descriptorsXrelays.size();
	std::string urlXRelay = (*descriptorsXrelays.at(location%XRelaySize)).getContextDescriptor()->getURL();
	std::string urnXRelay = (*descriptorsXrelays.at(location%XRelaySize)).getURN();
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
	  getApplicationGroup()->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),4);
	
	xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, xrelay);
	
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
	xdaq::ApplicationDescriptor * tstoreDescriptor = getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),400);
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
	xdaq::ApplicationDescriptor * tstoreDescriptor = getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),400);
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
	queryElement.addAttribute(property, "myTest");
      }
      catch(xoap::exception::Exception& e) {
	std::cout << "Got exception 1" << std::endl;
      }
      
      try {
	xdaq::ApplicationDescriptor * tstoreDescriptor = getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(getApplicationContext()->getContextDescriptor(),400);
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
  // 
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
	  getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("DDUHyperDAQ",0);
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
	  getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("EmuCrateSOAP", 0);
	xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
	//
	d = 
	  getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("EmuCrateSOAP", 1);
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
    std::vector<xdaq::ApplicationDescriptor * >  descriptors =
      getApplicationContext()->getApplicationGroup()->getApplicationDescriptors("EmuCrateSOAP");
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
    std::vector<xdaq::ApplicationDescriptor * >  descriptors =
      getApplicationContext()->getApplicationGroup()->getApplicationDescriptors("EmuCrateSOAP");
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
	  getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("EmuCrateSOAP", 0);
	xoap::MessageReference reply    = getApplicationContext()->postSOAP(msg, d);
	//
	d = 
	  getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("EmuCrateSOAP", 1);
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
};

#endif
