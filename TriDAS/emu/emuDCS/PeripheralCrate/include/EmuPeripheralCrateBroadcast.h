// $Id: EmuPeripheralCrateBroadcast.h,v 1.1 2007/10/29 12:47:44 gujh Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _EmuPeripheralCrateBroadcast_h_
#define _EmuPeripheralCrateBroadcast_h_

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

static int calsetup;
EmuController * MyController;
Crate * broadcastCrate;
DAQMB * broadcastDMB;
TMB * broadcastTMB;
ALCTController * broadcastALCT;
CCB * broadcastCCB;

using namespace cgicc;
using namespace std;

//class EmuPeripheralCrateBroadcast: public xdaq::Application
class EmuPeripheralCrateBroadcast: public EmuApplication
{
  
public:
  
  XDAQ_INSTANTIATOR();
  
  //
  std::string HomeDir_;
  std::string ConfigDir_;  
  std::string FirmwareDir_;
  //
  void DefineBroadcastCrate();
  std::string PeripheralCrateBroadcastXmlFile_;
  std::string DmbControlFPGAFirmwareFile_;
  std::string DmbVmeFPGAFirmwareFile_;
  std::string CfebFPGAFirmwareFile_;
  std::string TMBFirmwareFile_;
  //
  std::string ALCTFirmwareDirectory_     ;
  //
  std::string ALCT192FirmwareFile_       ;
  std::string ALCT288FirmwareFile_       ;
  std::string ALCT288bnFirmwareFile_     ;
  std::string ALCT288bpFirmwareFile_     ;
  std::string ALCT288fpFirmwareFile_     ;
  std::string ALCT384FirmwareFile_       ;
  std::string ALCT384MirrorFirmwareFile_ ;
  std::string ALCT576MirrorFirmwareFile_ ;
  std::string ALCT672FirmwareFile_       ;
  std::string ALCT672MirrorFirmwareFile_ ;  
  //
  EmuPeripheralCrateBroadcast(xdaq::ApplicationStub * s);
  //
  void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void MainPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
  void LoadCFEBcalchannel(xgi::Input * in, xgi::Output * out );
  void LoadCFEBinternal(xgi::Input * in, xgi::Output * out );
  void LoadCFEBexternal(xgi::Input * in, xgi::Output * out );
  void DmbTurnOnPower(xgi::Input * in, xgi::Output * out );
  void DmbTurnOffPower(xgi::Input * in, xgi::Output * out );
  void LoadDACandTrigger(xgi::Input * in, xgi::Output * out );
  //
  // Firmware
  //
  void LoadDMBCFEBFPGAFirmware(xgi::Input * in, xgi::Output * out );
  void LoadDMBControlFPGAFirmware(xgi::Input * in, xgi::Output * out );
  void LoadDMBvmeFPGAFirmware(xgi::Input * in, xgi::Output * out ) throw(xgi::exception::Exception);
  void LoadCFEBFPGAFirmware(xgi::Input * in, xgi::Output * out );
  void LoadTMBFirmware(xgi::Input * in, xgi::Output * out );
  void LoadALCTFirmware(xgi::Input * in, xgi::Output * out );
  //
  void MyHeader(xgi::Input * in, xgi::Output * out, std::string title ) throw (xgi::exception::Exception);
  //
  xoap::MessageReference onConfigCalCFEB (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBGains (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBCrossTalk (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBSCAPed (xoap::MessageReference message) throw (xoap::exception::Exception);
  xoap::MessageReference onEnableCalCFEBComparator (xoap::MessageReference message) throw (xoap::exception::Exception);
  // Sending soap messages
  //
  xoap::MessageReference PCcreateCommandSOAP(string command);
  void PCsendCommand(string command, string klass) throw (xoap::exception::Exception, xdaq::exception::Exception);
  //
};

#endif
