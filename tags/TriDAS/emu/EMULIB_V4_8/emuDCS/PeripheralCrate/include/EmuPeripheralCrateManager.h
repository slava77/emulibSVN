// $Id: EmuPeripheralCrateManager.h,v 1.40 2007/12/05 13:48:59 gujh Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuPeripheralCrateManager_h_
#define _EmuPeripheralCrateManager_h_

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

#include "EmuApplication.h"
#include "EmuController.h"
#include "Crate.h"
#include "TStoreParser.h"
#include "CrateUtilities.h"

using namespace cgicc;
using namespace std;

//class EmuPeripheralCrateManager: public xdaq::Application
class EmuPeripheralCrateManager: public EmuApplication
{
  
public:
  
  XDAQ_INSTANTIATOR();
  
  xdata::UnsignedLong runNumber_;
  xdata::String ConfigureState_;
  xdata::Table table_;                            
  int LTCDone;
  //
  //
  EmuPeripheralCrateManager(xdaq::ApplicationStub * s);
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void CheckEmuPeripheralCrateState(xgi::Input * in, xgi::Output * out );
  int CompareEmuPeripheralCrateCalibrationState(std::string state_compare);
  void UploadDB(xgi::Input * in, xgi::Output * out );
  void AddRow(int Row,xdata::Table & table, std::vector<std::string> NewColumn);
  int CompareEmuPeripheralCrateState(std::string state_compare);
  //
  void CheckEmuPeripheralCrateCalibrationState(xgi::Input * in, xgi::Output * out );
  //
  void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  void enableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  void disableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  void haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  void stateChanged(toolbox::fsm::FiniteStateMachine &fsm) throw (toolbox::fsm::exception::Exception);
  void MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) throw (xgi::exception::Exception);
  //
  string extractCalibrationState(xoap::MessageReference message);
  string extractState(xoap::MessageReference message);
  string extractRunNumber(xoap::MessageReference message);
  //
  // What to do when we receive soap messages
  //
  xoap::MessageReference LTCResponse (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onConfigure (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnable (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onDisable (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onHalt (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onConfigCalCFEB (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBCrossTalk (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBSCAPed (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBGains (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBComparator (xoap::MessageReference message) throw (xoap::exception::Exception);

//
  // Sending soap messages
  //
  xoap::MessageReference PCcreateCommandSOAP(string command);
  void PCsendCommand(string command, string klass) throw (xoap::exception::Exception, xdaq::exception::Exception);
  //
  void relayMessage (xoap::MessageReference msg) throw (xgi::exception::Exception);
  void SendSOAPMessageConnectTStore(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void SendSOAPMessageDisconnectTStore(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SendSOAPMessageQueryTStore(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void RetrieveTStoreTable(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  std::vector < std::vector <std::string > > ConvertTable(xdata::Table thisTable) throw (xgi::exception::Exception);
  void RetrieveTStoreTable(xgi::Input * in, xgi::Output * out, std::string CrateLabel) throw (xgi::exception::Exception);
  void SendSOAPMessageQueryTStore(xgi::Input * in, xgi::Output * out, std::string TableName ) throw (xgi::exception::Exception);
  void SendSOAPMessageDefinitionTStore(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void SendSOAPMessageDefinitionTStore(xgi::Input * in, xgi::Output * out, std::string TableName ) throw (xgi::exception::Exception);
  std::string MIMETypeFromXdataType(std::string xdataType);
  void addAttachment(xoap::MessageReference message,xdata::Serializable &data,std::string contentId);
  void SendSOAPMessageInsertTStore(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception);
  void SendSOAPMessageInsertTStore(xgi::Input * in, xgi::Output * out, std::string TableName ) throw (xgi::exception::Exception);
  void SendSOAPMessageDeleteTStore(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SendSOAPMessageClearTStore(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SendSOAPMessageUpdateTStore(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SendSOAPMessageUpdateTStore(xgi::Input * in, xgi::Output * out, std::string TableName ) throw (xgi::exception::Exception);
  void SetRandomDataTable(xdata::Table & table,int Row);
  void OutputTable(xgi::Input * in, xgi::Output * out,xdata::Table &results);
  void SendSOAPMessageOpenFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SendSOAPMessageConfigureLTC(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SendSOAPMessageExecuteSequence(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SendSOAPMessageJobControlStartAllEmuperipheralCrate(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SendSOAPMessageJobControlkillAll(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SendSOAPMessageQueryLTC(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SendSOAPMessageXRelaySimple(std::string command,std::string setting);
  void SendSOAPMessageXRelayBroadcast(std::string command,std::string setting);
  void SendSOAPMessageConfigureXRelay(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SendSOAPMessageCalibrationXRelay(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void SendSOAPMessageConfigure(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  //  void PCanalyzeReply(xoap::MessageReference message, xoap::MessageReference reply,xdaq::ApplicationDescriptor *app);

  xoap::MessageReference killAllMessage();
  xoap::MessageReference QueryPeripheralCrateInfoSpace();
  xoap::MessageReference QueryLTCInfoSpace();
  xoap::MessageReference QueryJobControlInfoSpace();
  xoap::MessageReference ExecuteCommandMessage(std::string port);
  xoap::MessageReference createXRelayMessage(const std::string & command, const std::string & setting,
                                             std::set<xdaq::ApplicationDescriptor * > descriptor );
  //
};

#endif
