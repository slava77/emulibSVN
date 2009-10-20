// Application.cc

#include "emu/supervisor/Application.h"

#include <sstream>
#include <set>
#include <map>
#include <cstdlib>  // strtol()
#include <iomanip>
#include <sys/time.h>  // gettimeofday()

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"
#include "xoap/MessageFactory.h"  // createMessage()
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPSerializer.h"
#include "xoap/domutils.h"
#include "toolbox/task/WorkLoopFactory.h" // getWorkLoopFactory()

#include "xoap/DOMParser.h"
#include "xoap/DOMParserFactory.h"
#include "xoap/domutils.h"
#include "xdata/soap/Serializer.h"

#include "cgicc/HTMLClasses.h"
#include "xgi/Utils.h"

#include "xcept/tools.h"
#include "emu/supervisor/ELog.h"
#include "xdaq2rc/RcmsStateNotifier.h"
#include "toolbox/fsm/FailedEvent.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationDescriptorImpl.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/XceptSerializer.h"
#include "xdata/UnsignedShort.h" 
#include "xdata/UnsignedLong.h" 
#include "xdata/Float.h" 
#include "xdata/Double.h" 
#include "xdata/Boolean.h"

#include "emu/supervisor/exception/Exception.h"
#include "emu/supervisor/alarm/Alarm.h"
#include "emu/base/Alarm.h"

using namespace std;
using namespace cgicc;

XDAQ_INSTANTIATOR_IMPL(emu::supervisor::Application)

static const string NS_XSI = "http://www.w3.org/2001/XMLSchema-instance";
static const unsigned int N_LOG_MESSAGES = 50;
static const string STATE_UNKNOWN = "unknown";

void emu::supervisor::Application::CalibParam::registerFields(xdata::Bag<CalibParam> *bag)
{
  key_ = "";
  command_ = "";
  loop_ = 1U;
  delay_ = 1U;
  ltc_ = "LTCConfiguration.txt";
  
  bag->addField("key",     &key_);
  bag->addField("command", &command_);
  bag->addField("loop",    &loop_);
  bag->addField("delay",   &delay_);
  bag->addField("ltc",     &ltc_);
}

emu::supervisor::Application::Application(xdaq::ApplicationStub *stub)
  //throw (xcept::Exception) 
  :
  xdaq::WebApplication(stub),
  emu::base::Supervised(stub),
  logger_(Logger::getInstance("emu::supervisor::Application")),
  run_type_("Monitor"), run_number_(1), runSequenceNumber_(0),
  daq_mode_("UNKNOWN"), ttc_source_(""),
  rcmsStateNotifier_(getApplicationLogger(), getApplicationDescriptor(), getApplicationContext()),
  TFCellOpState_(""), TFCellOpName_("EmuLocal"), TFCellClass_("Cell"), TFCellInstance_(8), 
  wl_semaphore_(toolbox::BSem::EMPTY), quit_calibration_(false),
  daq_descr_(NULL), tf_descr_(NULL), ttc_descr_(NULL),
  nevents_(-1),
  step_counter_(0),
  error_message_(""), keep_refresh_(false), hide_tts_control_(true),
  controlTFCellOp_(false), // Default MUST be false, i.e., hands off the TF Cell.
  curlHost_("cmsusr1.cms"),
  runInfo_(NULL),
  runDbBookingCommand_( "java -jar runnumberbooker.jar" ),
  runDbWritingCommand_( "java -jar runinfowriter.jar" ),
  runDbAddress_       ( "" ),
  runDbUserFile_      ( "" ),
  isBookedRunNumber_  ( false ),
  state_table_(this)
{
  start_attr.insert(std::map<string, string>::value_type("Param", "Start"));
  stop_attr.insert(std::map<string, string>::value_type("Param", "Stop"));
  
  appDescriptor_ = getApplicationDescriptor();
  
  xdata::InfoSpace *i = getApplicationInfoSpace();
  i->fireItemAvailable("RunType", &run_type_);
  i->fireItemAvailable("RunNumber", &run_number_);
  
  i->fireItemAvailable("configKeys", &config_keys_);
  i->fireItemAvailable("calibParams", &calib_params_);
  i->fireItemAvailable("pcKeys",     &pc_keys_);
  i->fireItemAvailable("pcConfigs",  &pc_configs_);
  i->fireItemAvailable("fcKeys",     &fc_keys_);
  i->fireItemAvailable("fcConfigs",  &fc_configs_);
  
  i->fireItemAvailable("DAQMode", &daq_mode_);
  i->fireItemAvailable("TTCSource", &ttc_source_);
  
  i->fireItemAvailable("TFCellOpState",  &TFCellOpState_);
  i->fireItemAvailable("TFCellOpName",   &TFCellOpName_);
  i->fireItemAvailable("TFCellClass",    &TFCellClass_);
  i->fireItemAvailable("TFCellInstance", &TFCellInstance_);

  i->fireItemAvailable("ttsID", &tts_id_);
  i->fireItemAvailable("ttsBits", &tts_bits_);

  i->fireItemAvailable("controlTFCellOp", &controlTFCellOp_);

  // Track Finder Key
  tf_key_ = "310309";   // default key as of 31/03/2009
  i->fireItemAvailable("TrackFinderKey", &tf_key_);  

  i->fireItemAvailable( "curlHost",       &curlHost_     );
  i->fireItemAvailable( "curlCommand",    &curlCommand_  );
  i->fireItemAvailable( "curlCookies", 	&curlCookies_  );
  i->fireItemAvailable( "CMSUserFile", 	&CMSUserFile_  );
  i->fireItemAvailable( "eLogUserFile",	&eLogUserFile_ );
  i->fireItemAvailable( "eLogURL",     	&eLogURL_      );
  i->fireItemAvailable( "peripheralCrateConfigFiles", &peripheralCrateConfigFiles_ );
  
  i->fireItemAvailable( "runDbBookingCommand", &runDbBookingCommand_ );
  i->fireItemAvailable( "runDbWritingCommand", &runDbWritingCommand_ );
  i->fireItemAvailable( "runDbAddress",        &runDbAddress_        );
  i->fireItemAvailable( "runDbUserFile",       &runDbUserFile_       );
  
  //Interface to Run Control
  i->fireItemAvailable("rcmsStateListener", rcmsStateNotifier_.getRcmsStateListenerParameter());
  i->fireItemAvailable("foundRcmsStateListener", rcmsStateNotifier_.getFoundRcmsStateListenerParameter());
  rcmsStateNotifier_.subscribeToChangesInRcmsStateListener(getApplicationInfoSpace());
	
  xgi::bind(this, &emu::supervisor::Application::webDefault,   "Default");
  xgi::bind(this, &emu::supervisor::Application::webConfigure, "Configure");
  xgi::bind(this, &emu::supervisor::Application::webStart,    "Start");
  xgi::bind(this, &emu::supervisor::Application::webStop,   "Stop");
  xgi::bind(this, &emu::supervisor::Application::webHalt,      "Halt");
  xgi::bind(this, &emu::supervisor::Application::webReset,     "Reset");
  xgi::bind(this, &emu::supervisor::Application::webSetTTS,    "SetTTS");
  xgi::bind(this, &emu::supervisor::Application::webSwitchTTS, "SwitchTTS");
  
  xoap::bind(this, &emu::supervisor::Application::onConfigure, "Configure", XDAQ_NS_URI);
  xoap::bind(this, &emu::supervisor::Application::onStart,    "Start",    XDAQ_NS_URI);
  xoap::bind(this, &emu::supervisor::Application::onStop,   "Stop",   XDAQ_NS_URI);
  xoap::bind(this, &emu::supervisor::Application::onHalt,      "Halt",      XDAQ_NS_URI);
  xoap::bind(this, &emu::supervisor::Application::onReset,     "Reset",     XDAQ_NS_URI);
  xoap::bind(this, &emu::supervisor::Application::onSetTTS,    "SetTTS",    XDAQ_NS_URI);
  
  wl_ = toolbox::task::getWorkLoopFactory()->getWorkLoop("CSC SV", "waiting");
  wl_->activate();
  configure_signature_ = toolbox::task::bind(
					     this, &emu::supervisor::Application::configureAction,  "configureAction");
  start_signature_ = toolbox::task::bind(
					 this, &emu::supervisor::Application::startAction,  "startAction");
  halt_signature_ = toolbox::task::bind(
					this, &emu::supervisor::Application::haltAction,       "haltAction");
  calibration_signature_ = toolbox::task::bind(
					       this, &emu::supervisor::Application::calibrationAction, "calibrationAction");
  
  fsm_.addState('H', "Halted",     this, &emu::supervisor::Application::stateChanged);
  fsm_.addState('C', "Configured", this, &emu::supervisor::Application::stateChanged);
  fsm_.addState('E', "Running",    this, &emu::supervisor::Application::stateChanged);
  
  fsm_.setStateName('F', "Error");
  fsm_.setFailedStateTransitionAction(this, &emu::supervisor::Application::transitionFailed);
  
  //fsm_.setStateName('F',"Error", this, &emu::supervisor::Application::transitionFailed);
  
  //	fsm_.addState('c', "Configuring", this, &emu::supervisor::Application::stateChanged);
  
  fsm_.addStateTransition(
			  'H', 'C', "Configure", this, &emu::supervisor::Application::configureAction);
  fsm_.addStateTransition(
			  'C', 'C', "Configure", this, &emu::supervisor::Application::configureAction);
  fsm_.addStateTransition(
			  'C', 'E', "Start",    this, &emu::supervisor::Application::startAction);
  fsm_.addStateTransition(
			  'E', 'C', "Stop",   this, &emu::supervisor::Application::stopAction);
  fsm_.addStateTransition(
			  'C', 'H', "Halt",      this, &emu::supervisor::Application::haltAction);
  fsm_.addStateTransition(
			  'E', 'H', "Halt",      this, &emu::supervisor::Application::haltAction);
  fsm_.addStateTransition(
			  'H', 'H', "Halt",      this, &emu::supervisor::Application::haltAction);
  fsm_.addStateTransition(
			  'E', 'E', "SetTTS",    this, &emu::supervisor::Application::setTTSAction);
  
  fsm_.setInitialState('H');
  fsm_.reset();
  
  state_ = fsm_.getStateName(fsm_.getCurrentState());
  
  state_table_.addApplication("emu::fed::Manager");
  state_table_.addApplication("emu::pc::EmuPeripheralCrateManager");
  state_table_.addApplication("emu::daq::manager::Application");
  state_table_.addApplication("TTCciControl");
  state_table_.addApplication("LTCControl");
  
  // last_log_.size(N_LOG_MESSAGES);
  
  getAppDescriptors();

  LOG4CPLUS_INFO(logger_, "emu::supervisor::Application constructed for " << state_table_ );
}

void emu::supervisor::Application::getAppDescriptors(){

  try {
    daq_descr_ = getApplicationContext()->getDefaultZone()
      ->getApplicationDescriptor("emu::daq::manager::Application", 0);
  } catch (xdaq::exception::ApplicationDescriptorNotFound& e) {
    LOG4CPLUS_ERROR(logger_, "Failed to get application descriptor for local DAQ Manager. "
		    << xcept::stdformat_exception_history(e));
    stringstream ss;
    ss <<  "Failed to get application descriptor for local DAQ Manager. ";
    XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss.str(), e );
    this->notifyQualified( "error", eObj );
  }

  try {
    ttc_descr_ = getApplicationContext()->getDefaultZone()
      ->getApplicationDescriptor("TTCciControl", 0);
  } catch (xdaq::exception::ApplicationDescriptorNotFound& e) {
    LOG4CPLUS_ERROR(logger_, "Failed to get application descriptor for TTCciControl. "
		    << xcept::stdformat_exception_history(e));
    stringstream ss;
    ss <<  "Failed to get application descriptor for TTCciControl. ";
    XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss.str(), e );
    this->notifyQualified( "error", eObj );
  }

  try {
    tf_descr_ = getApplicationContext()->getDefaultZone()
      ->getApplicationDescriptor( TFCellClass_.toString(), TFCellInstance_.value_ );
  } catch (xdaq::exception::ApplicationDescriptorNotFound& e) {
    stringstream ss;
    ss << "No Track Finder application \"" << TFCellClass_.toString() 
       << "\" of instance " << TFCellInstance_.value_ << " found.";
    LOG4CPLUS_ERROR(logger_, ss.str() << xcept::stdformat_exception_history(e));
    XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss.str(), e );
    this->notifyQualified( "error", eObj );
  }  
  
}

xoap::MessageReference emu::supervisor::Application::onConfigure(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  run_number_ = 1;
  nevents_ = -1;
  
  submit(configure_signature_);
  
  return createReply(message);
}

xoap::MessageReference emu::supervisor::Application::onStart(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  //fireEvent("Start");
  submit(start_signature_);
  
  return createReply(message);
}

xoap::MessageReference emu::supervisor::Application::onStop(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  fireEvent("Stop");
  
  return createReply(message);
}

xoap::MessageReference emu::supervisor::Application::onHalt(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  quit_calibration_ = true;
  
  submit(halt_signature_);
  
  return createReply(message);
}

xoap::MessageReference emu::supervisor::Application::onReset(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  resetAction();
  
  return onHalt(message);
}

xoap::MessageReference emu::supervisor::Application::onSetTTS(xoap::MessageReference message)
  throw (xoap::exception::Exception)
{
  fireEvent("SetTTS");
  
  return createReply(message);
}

void emu::supervisor::Application::webDefault(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  if (keep_refresh_) {
    HTTPResponseHeader &header = out->getHTTPResponseHeader();
    header.addHeader("Refresh", "2");
  }
  
  // Header
  *out << HTMLDoctype(HTMLDoctype::eStrict) << endl;
  *out << html() << endl;
  
  *out << head() << endl;
  *out << title("emu::supervisor::Application") << endl;
  *out << cgicc::link().set("rel", "stylesheet")
    .set("href", "/emu/supervisor/html/emusupervisor.css")
    .set("type", "text/css") << endl;
  *out << head() << endl;
  
  // Body
  *out << body() << endl;
  
  // Config listbox
  *out << form().set("action",
		     "/" + getApplicationDescriptor()->getURN() + "/Configure") << endl;
  
  int n_keys = config_keys_.size();
  
  *out << "Run Type: " << endl;
  *out << cgicc::select().set("name", "runtype") << endl;
  
  int selected_index = keyToIndex(run_type_.toString());
  
  for (int i = 0; i < n_keys; ++i) {
    if (i == selected_index) {
      *out << option()
	.set("value", (string)config_keys_[i])
	.set("selected", "");
    } else {
      *out << option()
	.set("value", (string)config_keys_[i]);
    }
    *out << (string)config_keys_[i] << option() << endl;
  }
  
  *out << cgicc::select() << endl;
  
  *out << input().set("type", "submit")
    .set("name", "command")
    .set("value", "Configure") << endl;
  *out << form() << endl;
  
  
  /*
   *out << "Run Number: " << endl;
   *out << input().set("type", "text")
   .set("name", "runnumber")
   .set("value", run_number_.toString())
   .set("size", "40") << br() << endl;
   
   *out << "Max # of Events: " << endl;
   *out << input().set("type", "text")
   .set("name", "nevents")
   .set("value", toString(nevents_))
   .set("size", "40") << br() << endl;
  */
  
  // Buttons
  *out << table() << tbody() << tr();
  
  *out << td() << form().set("action",
			"/" + getApplicationDescriptor()->getURN() + "/Start") << endl;
  *out << input().set("type", "submit")
    .set("name", "command")
    .set("value", "Start") << endl;
  *out << form() << td() << endl;
  
  *out << td() << form().set("action",
			     "/" + getApplicationDescriptor()->getURN() + "/Stop") << endl;
  *out << input().set("type", "submit")
    .set("name", "command")
    .set("value", "Stop") << endl;
  *out << form() << td() << endl;
  
  *out << td() << form().set("action",
			     "/" + getApplicationDescriptor()->getURN() + "/Halt") << endl;
  *out << input().set("type", "submit")
    .set("name", "command")
    .set("value", "Halt") << endl;
  *out << form() << td() << endl;
  
  *out << td() << form().set("action",
			     "/" + getApplicationDescriptor()->getURN() + "/Reset") << endl;
  *out << input().set("type", "submit")
    .set("name", "command")
    .set("value", "Reset") << endl;
  *out << form() << td() << endl;
  
  *out << tr() << tbody() << table();
  
  // TTS operation
  if (hide_tts_control_) {
    *out << form().set("action",
		       "/" + getApplicationDescriptor()->getURN() + "/SwitchTTS") << endl;
    *out << input().set("type", "submit")
      .set("name", "command")
      .set("value", "ShowTTSControl") << endl;
    *out << form() << endl;
  } else {
    *out << form().set("action",
		       "/" + getApplicationDescriptor()->getURN() + "/SetTTS") << endl;
    
    *out << small() << "(See, "
	 << a().set("href", "https://twiki.cern.ch/twiki/bin/view/CMS/CSCcommissioningFED#FED_Mapping")
	 << "FED commissioning Wiki page"
	 << a()
	 << " to find out sTTS IDs (FMM-ID).)" << small() << br() << endl;
    *out << "sTTS ID: (750-757,760,830-869)" << endl;
    *out << input().set("type", "text")
      .set("name", "tts_id")
				.set("value", tts_id_.toString())
      .set("size", "10") << br() << endl;
    
    *out << "sTTS value: (0-15)" << endl;
    *out << input().set("type", "text")
      .set("name", "tts_bits")
      .set("value", tts_bits_.toString())
      .set("size", "10") << br() << endl;
    
    *out << input().set("type", "submit")
      .set("name", "command")
      .set("value", "SetTTS") << endl;
    *out << form() << endl;
    
    *out << form().set("action",
		       "/" + getApplicationDescriptor()->getURN() + "/SwitchTTS") << endl;
    *out << input().set("type", "submit")
      .set("name", "command")
      .set("value", "HideTTSControl") << endl;
    *out << form() << endl;
  }
  
  // Error message, if exists.
  if (!error_message_.empty()) {
    *out  << p() << span().set("style", "color: red;")
	  << error_message_ << span() << p() << endl;
    error_message_ = "";
  }
  
  // Configuration parameters
  *out << hr() << endl;
  *out << "Step counter: " << step_counter_ << br() << endl;
  
  refreshConfigParameters();
  
  *out << "TTCci inputs(Clock:Orbit:Trig:BGo): " << ttc_source_.toString() << br() << endl;
  
  *out << "Mode of DAQManager: " << daq_mode_.toString() << br() << endl;
  string localDAQState = getLocalDAQState();
  *out << "Local DAQ state: " << span().set("class",localDAQState) << localDAQState << span() << br() << endl;

  *out << "State of TF operation " << cite() << TFCellOpName_.toString() << cite() << ": " 
       << span().set("class",TFCellOpState_.toString()) << TFCellOpState_.toString() 
       << span() << br() << endl;
  
  // Application states
  *out << hr() << endl;
  state_table_.webOutput(out, (string)state_);
  
  // Message logs
  *out << hr() << endl;
  // last_log_.webOutput(out);
  
  *out << body() << html() << endl;
}

void emu::supervisor::Application::webConfigure(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  string value;
  
  value = getCGIParameter(in, "runtype");
  if (value.empty()) { error_message_ += "Please select run type.\n"; }
  run_type_ = value;
  
  run_number_ = 1;
  nevents_ = -1;
  
  if (error_message_.empty()) {
    submit(configure_signature_);
  }
  
  keep_refresh_ = true;
  webRedirect(in, out);
}

void emu::supervisor::Application::webStart(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  // Book run number here to make sure it's done 
  // only when requested by the user from the web page,
  // and not by the FunctionManager via SOAP.
  bookRunNumber();
  
  fireEvent("Start");
  
  keep_refresh_ = true;
  webRedirect(in, out);
  
  if (!isCalibrationMode()) {
    keep_refresh_ = false;
  }
}

void emu::supervisor::Application::webStop(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  fireEvent("Stop");
  
  keep_refresh_ = true;
  webRedirect(in, out);
}

void emu::supervisor::Application::webHalt(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  quit_calibration_ = true;
  
  submit(halt_signature_);

  keep_refresh_ = true;
  webRedirect(in, out);
}

void emu::supervisor::Application::webReset(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  resetAction();
  
  webHalt(in, out);
}

void emu::supervisor::Application::webSetTTS(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  tts_id_.fromString(getCGIParameter(in, "tts_id"));
  tts_bits_.fromString(getCGIParameter(in, "tts_bits"));
  
  if (error_message_.empty()) {
    fireEvent("SetTTS");
  }
  
  webRedirect(in, out);
}

void emu::supervisor::Application::webSwitchTTS(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  hide_tts_control_ = getCGIParameter(in, "command").find("Hide", 0) == 0;
  
  webRedirect(in, out);
}

void emu::supervisor::Application::webRedirect(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception)
{
  string url = in->getenv("PATH_TRANSLATED");
  
  HTTPResponseHeader &header = out->getHTTPResponseHeader();
  
  header.getStatusCode(303);
  header.getReasonPhrase("See Other");
  header.addHeader("Location",
		   url.substr(0, url.find("/" + in->getenv("PATH_INFO"))));
}

bool emu::supervisor::Application::configureAction(toolbox::task::WorkLoop *wl)
{
  fireEvent("Configure");
  
  return false;
}

bool emu::supervisor::Application::startAction(toolbox::task::WorkLoop *wl)
{
  fireEvent("Start");
  
  return false;
}

bool emu::supervisor::Application::haltAction(toolbox::task::WorkLoop *wl)
{
  fireEvent("Halt");
  
  return false;
}

bool emu::supervisor::Application::calibrationAction(toolbox::task::WorkLoop *wl)
{
  LOG4CPLUS_DEBUG(logger_, "calibrationAction " << "(begin)");
  
  string command, ltc;
  unsigned int loop, delay;
  
  int index = getCalibParamIndex(run_type_);
  command = calib_params_[index].bag.command_;
  loop    = calib_params_[index].bag.loop_;
  delay   = calib_params_[index].bag.delay_;
  ltc     = calib_params_[index].bag.ltc_;
  
  LOG4CPLUS_DEBUG(logger_, "command: " << command
		  << " loop: " << loop << " delay: " << delay << " ltc: " << ltc);
  
  for (step_counter_ = 0; step_counter_ < loop; ++step_counter_) {
    if (quit_calibration_) { break; }
    LOG4CPLUS_DEBUG(logger_,
		    "calibrationAction: " << step_counter_);
    
    sendCommand(command, "emu::pc::EmuPeripheralCrateManager");
    sendCommandWithAttr("Cyclic", start_attr, "LTCControl");
    sleep(delay);
  }
  
  if (!quit_calibration_) {
    submit(halt_signature_);
  }

  LOG4CPLUS_DEBUG(logger_, "calibrationAction " << "(end)");

  return false;
}

void emu::supervisor::Application::configureAction(toolbox::Event::Reference evt) 
  throw (toolbox::fsm::exception::Exception)
{
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(begin)");
  LOG4CPLUS_DEBUG(logger_, "runtype: " << run_type_.toString()
		  << " runnumber: " << run_number_ << " nevents: " << nevents_);
  

  rcmsStateNotifier_.findRcmsStateListener();      	
  step_counter_ = 0;

  try {

    //
    // Clean up leftover ops and halt apps
    //
    
    if ( tf_descr_ != NULL && controlTFCellOp_.value_ ){
      TFCellOpState_ = OpGetStateCell();
      if ( TFCellOpState_.toString() != "UNKNOWN" ){
	// Reset csctf-cell operation before killing it to allow it to stop in an orderly fashion
	OpResetCell();
	waitForTFCellOpToReach("halted",60);
      }
      // Kill leftover csctf-cell operation
      sendCommandCellOpkill();
      if ( waitForTFCellOpToReach("UNKNOWN",60) ){
	// Creating csctf-cell operation
	sendCommandCellOpInit();
      }
    }

    try {
      state_table_.refresh();
      if (state_table_.getState("emu::daq::manager::Application", 0) != "Halted") {
	sendCommand("Halt", "emu::daq::manager::Application");
	waitForDAQToExecute("Halt", 10, true);
      }
       
      if (state_table_.getState("TTCciControl", 0) != "Halted") {
	sendCommand("Halt", "TTCciControl");
      }
      if (state_table_.getState("LTCControl", 0) != "Halted") {
	sendCommand("Halt", "LTCControl");
	// Allow LTCControl some time to halt:
	::sleep(2);
      }
    } catch (xcept::Exception ignored) {}
    

    //
    // Configure
    //

    try {
      setParameter("emu::daq::manager::Application", "maxNumberOfEvents", "xsd:integer",
		   toString(nevents_));
      setParameter("emu::daq::manager::Application", "runType", "xsd:string",
		   run_type_.toString());
    } catch (xcept::Exception ignored) {}
    
    // Configure local DAQ first as its FSM is driven asynchronously,
    // and it will probably finish the transition by the time the others do.
    try {
      sendCommand("Configure", "emu::daq::manager::Application");
    } catch (xcept::Exception ignored) {}
    
    // Configure PCrate
    string str = trim(getCrateConfig("PC", run_type_.toString()));
    if (!str.empty()) {
      setParameter(
		   "emu::pc::EmuPeripheralCrateManager", "xmlFileName", "xsd:string", str);
    }
    if (!isCalibrationMode()) {
      sendCommand("Configure", "emu::pc::EmuPeripheralCrateManager");
    } else 
	{
		if (isAlctCalibrationMode())
			sendCommand("ConfigCalALCT", "emu::pc::EmuPeripheralCrateManager");
		else
			sendCommand("ConfigCalCFEB", "emu::pc::EmuPeripheralCrateManager");
    }   
       
    // Configure TF Cell operation
    if ( tf_descr_ != NULL && controlTFCellOp_.value_ ){
      if ( waitForTFCellOpToReach("halted",60) ){
	sendCommandCell("configure");
	waitForTFCellOpToReach("configured",60);
      }
      if ( TFCellOpState_.toString() != "configured" ){
	stringstream ss;
	ss << "TF Cell Operation \"" << TFCellOpName_.toString() 
	   << "\" failed to reach configured state. Aborting.";
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss.str() );
	this->notifyQualified( "error", eObj );
	throw eObj;
      } 
    }

    // Configure FED
    sendCommand("Configure", "emu::fed::Manager");
    
    // Configure TTC
    sendCommand("Configure", "TTCciControl");
    
    // Configure LTC
    int index = getCalibParamIndex(run_type_);
    if (index >= 0) {
      setParameter("LTCControl", "Configuration", "xsd:string",
		   "[file=" + calib_params_[index].bag.ltc_.toString() + "]");
    }
    sendCommand("Configure", "LTCControl");

    // If necessary, wait a bit for DAQ to finish configuring
    waitForDAQToExecute("Configure", 10, true);
       
    state_table_.refresh();
    if (!state_table_.isValidState("Configured")) {
      stringstream ss;
      ss << state_table_;
      XCEPT_RAISE(xcept::Exception,
		  "Applications got to unexpected states: "+ss.str() );
    }
    refreshConfigParameters();
    
  } catch (xoap::exception::Exception e) {
    LOG4CPLUS_ERROR(logger_,
		    "Exception in " << evt->type() << ": " << e.what());
    stringstream ss0;
    ss0 << 
		    "Exception in " << evt->type() << ": " << e.what();
    XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss0.str() );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "SOAP fault was returned", e);
  } catch (xcept::Exception e) {
    LOG4CPLUS_ERROR(logger_,
		    "Exception in " << evt->type() << ": " << e.what());
    stringstream ss1;
    ss1 << 
		    "Exception in " << evt->type() << ": " << e.what();
    XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss1.str() );
    this->notifyQualified( "error", eObj );
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "Failed to configure", e);
	}
  
  state_table_.refresh();
  LOG4CPLUS_DEBUG(logger_,  "Current state is: [" << fsm_.getStateName (fsm_.getCurrentState()) << "]");
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(end of this function)");
}

void emu::supervisor::Application::startAction(toolbox::Event::Reference evt) 
  throw (toolbox::fsm::exception::Exception)
{
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(begin)");
  LOG4CPLUS_DEBUG(logger_, "runtype: " << run_type_.toString()
		  << " runnumber: " << run_number_ << " nevents: " << nevents_);
  
  try {
    state_table_.refresh();

      setParameter("emu::fed::Manager",
		   "runNumber", "xsd:unsignedLong", run_number_.toString());
/*      setParameter("emu::fed::Manager",
		   "runType", "xsd:string", run_type_.toString());
*/
    sendCommand("Enable", "emu::fed::Manager");
    
    if (!isCalibrationMode()) {
      sendCommand("Enable", "emu::pc::EmuPeripheralCrateManager");
    }
    
    try {
      if (state_table_.getState("emu::daq::manager::Application", 0) == "Halted") {
	setParameter("emu::daq::manager::Application",
		     "maxNumberOfEvents", "xsd:integer", toString(nevents_));
	sendCommand("Configure", "emu::daq::manager::Application");
	waitForDAQToExecute("Configure", 10, true);
      }
      
      setParameter("emu::daq::manager::Application",
		   "runNumber", "xsd:unsignedLong", run_number_.toString());
      
      sendCommand("Enable", "emu::daq::manager::Application");
      waitForDAQToExecute("Enable", 10, true);
      
    } catch (xcept::Exception ignored) {}
    
    state_table_.refresh();
    
    if (state_table_.getState("TTCciControl", 0) != "Enabled") {
      sendCommand("Enable", "TTCciControl");
    }
    if (state_table_.getState("LTCControl", 0) != "Enabled") {
      sendCommand("Enable", "LTCControl");
    }
    sendCommandWithAttr("Cyclic", stop_attr, "LTCControl");
    
    // Enable TF Cell operation
    if ( tf_descr_ != NULL && controlTFCellOp_.value_ ){
      sendCommandCell("enable");
      waitForTFCellOpToReach("enabled",10);
    }

    refreshConfigParameters();
    
  } catch (xoap::exception::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "SOAP fault was returned", e);
    
  } catch (xcept::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "Failed to send a command", e);
    
  }
  
  if (isCalibrationMode()) {
    quit_calibration_ = false;
    submit(calibration_signature_);
  }
  
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(end)");
}

void emu::supervisor::Application::stopAction(toolbox::Event::Reference evt) 
  throw (toolbox::fsm::exception::Exception)
{
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(begin)");
  
  try {
    state_table_.refresh();
    
    // Stop TF Cell operation
    if ( tf_descr_ != NULL && controlTFCellOp_.value_ ){
      sendCommandCell("stop");
      waitForTFCellOpToReach("configured",60);
    }

    if (state_table_.getState("LTCControl", 0) != "Halted") {
      sendCommand("Halt", "LTCControl");
    }
    if (state_table_.getState("TTCciControl", 0) != "Halted") {
      sendCommand("Halt", "TTCciControl");
    }
    
    try {
      sendCommand("Halt", "emu::daq::manager::Application");
      waitForDAQToExecute("Halt", 10, true);
    } catch (xcept::Exception ignored) {}
    
    writeRunInfo( true, false );
    sendCommand("Disable", "emu::fed::Manager");
    sendCommand("Disable", "emu::pc::EmuPeripheralCrateManager");
    sendCommand("Configure", "TTCciControl");
    sendCommand("Configure", "LTCControl");
  } catch (xoap::exception::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "SOAP fault was returned", e);
  } catch (xcept::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "Failed to send a command", e);
  }
  
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(end)");
}

void emu::supervisor::Application::haltAction(toolbox::Event::Reference evt) 
  throw (toolbox::fsm::exception::Exception)
{
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(begin)");
  
  try {
    state_table_.refresh();
    
    // Stop and destroy TF Cell operation
    if ( tf_descr_ != NULL && controlTFCellOp_.value_ ){
      sendCommandCell("stop");
      waitForTFCellOpToReach("configured",60);
      sendCommandCellOpkill();
    }

    if (state_table_.getState("LTCControl", 0) != "Halted") {
      sendCommand("Halt", "LTCControl");
    }
    if (state_table_.getState("TTCciControl", 0) != "Halted") {
      sendCommand("Halt", "TTCciControl");
    }
    sendCommand("Halt", "emu::fed::Manager");
    sendCommand("Halt", "emu::pc::EmuPeripheralCrateManager");
    
    try {
      sendCommand("Halt", "emu::daq::manager::Application");
      waitForDAQToExecute("Halt", 10, true);
    } catch (xcept::Exception ignored) {}
    writeRunInfo( true, false );
  } catch (xoap::exception::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "SOAP fault was returned", e);
  } catch (xcept::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "Failed to send a command", e);
  }
  
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(end)");
}

void emu::supervisor::Application::resetAction() throw (toolbox::fsm::exception::Exception)
{
  LOG4CPLUS_DEBUG(logger_, "reset(begin)");
  
  fsm_.reset();
  state_ = fsm_.getStateName(fsm_.getCurrentState());
  
  LOG4CPLUS_DEBUG(logger_, "reset(end)");
}

void emu::supervisor::Application::setTTSAction(toolbox::Event::Reference evt) 
  throw (toolbox::fsm::exception::Exception)
{
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(begin)");
  
  const string fed_app = "emu::fed::Manager";
  
  try {
    setParameter(fed_app, "ttsID",   "xsd:unsignedInt", tts_id_.toString());
    setParameter(fed_app, "ttsBits", "xsd:unsignedInt", tts_bits_.toString());
    
    sendCommand("SetTTSBits", fed_app, 0);
  } catch (xoap::exception::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "SOAP fault was returned", e);
  } catch (xcept::Exception e) {
    XCEPT_RETHROW(toolbox::fsm::exception::Exception,
		  "Failed to send a command", e);
  }
  
  LOG4CPLUS_DEBUG(logger_, evt->type() << "(end)");
}

void emu::supervisor::Application::submit(toolbox::task::ActionSignature *signature)
{
  wl_->submit(signature);
}

void emu::supervisor::Application::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
  throw (toolbox::fsm::exception::Exception)
{
  keep_refresh_ = false;
  
  LOG4CPLUS_DEBUG(getApplicationLogger(),"Current state is: [" << fsm.getStateName (fsm.getCurrentState()) << "]");
  // Send notification to Run Control
  state_=fsm.getStateName (fsm.getCurrentState());
  try
    {
      LOG4CPLUS_DEBUG(getApplicationLogger(),"Sending state changed notification to Run Control.");
      rcmsStateNotifier_.stateChanged((std::string)state_,"");
    }
  catch(xcept::Exception &e)
    {
      LOG4CPLUS_ERROR(getApplicationLogger(), "Failed to notify state change to Run Control."
		      << xcept::stdformat_exception_history(e));
      stringstream ss2;
      ss2 << "Failed to notify state change to Run Control.";
      XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss2.str(), e );
      this->notifyQualified( "error", eObj );
      std::cout << "rcmsFailed to notify state change to Run Control:" << std::endl;
    }
  
  emu::base::Supervised::stateChanged(fsm);
}

void emu::supervisor::Application::transitionFailed(toolbox::Event::Reference event)
  throw (toolbox::fsm::exception::Exception)
{
  keep_refresh_ = false;
  toolbox::fsm::FailedEvent &failed = dynamic_cast<toolbox::fsm::FailedEvent&>(*event);
  
  // Send notification to Run Control
  try {
    LOG4CPLUS_DEBUG(getApplicationLogger(),"Sending state changed notification to Run Control.");
    rcmsStateNotifier_.stateChanged("Error",xcept::stdformat_exception_history(failed.getException()));
    
  } catch(xcept::Exception &e) {
    LOG4CPLUS_ERROR(getApplicationLogger(), "Failed to notify state change to Run Control : "
		    << xcept::stdformat_exception_history(e));
    stringstream ss3;
    ss3 << "Failed to notify state change to Run Control : ";
    XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss3.str(), e );
    this->notifyQualified( "error", eObj );
  }
  
  LOG4CPLUS_INFO(getApplicationLogger(),
		 "Failure occurred when performing transition"
		 << " from: " << failed.getFromState()
		 << " to: " << failed.getToState()
		 << " exception: " << failed.getException().what());
}

void emu::supervisor::Application::sendCommand(string command, string klass)
  throw (xoap::exception::Exception, xcept::Exception)
{
  // Exceptions:
  // xoap exceptions are thrown by analyzeReply() for SOAP faults.
  // xdaq exceptions are thrown by postSOAP() for socket level errors.

  
  LOG4CPLUS_INFO( logger_, "Sending " + command + " to " + klass );
  // find applications
  std::set<xdaq::ApplicationDescriptor *> apps;
  apps = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(klass);
  if ( apps.size() == 0 ){
    LOG4CPLUS_INFO( logger_, "Sending " + command + " to " + klass + " aborted: No application descriptor found.");
    return; // Do nothing if the target doesn't exist
  }
  
  if (klass == "emu::daq::manager::Application" && !isDAQManagerControlled(command)) {
    LOG4CPLUS_INFO( logger_, "Sending " + command + " to " + klass + " aborted: Local DAQ is not controlled.");
    return;  // Do nothing if emu::daq::manager::Application is not under control.
  }
  
  // prepare a SOAP message
  xoap::MessageReference message = createCommandSOAP(command);
  xoap::MessageReference reply;

  // send the message one-by-one
  std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
  for (; i != apps.end(); ++i) {
    // postSOAP() may throw an exception when failed.
    reply = getApplicationContext()->postSOAP(message, *appDescriptor_, **i);

    analyzeReply(message, reply, *i);
  }
  LOG4CPLUS_INFO( logger_, "Sent " + command + " to " + klass );
}

void emu::supervisor::Application::sendCommand(string command, string klass, int instance)
  throw (xoap::exception::Exception, xcept::Exception)
{
  // Exceptions:
  // xoap exceptions are thrown by analyzeReply() for SOAP faults.
  // xdaq exceptions are thrown by postSOAP() for socket level errors.

  // find applications
  xdaq::ApplicationDescriptor *app;
  try {
    app = getApplicationContext()->getDefaultZone()
      ->getApplicationDescriptor(klass, instance);
  } catch (xdaq::exception::ApplicationDescriptorNotFound e) {
    return; // Do nothing if the target doesn't exist
  }
  
  if (klass == "emu::daq::manager::Application" && !isDAQManagerControlled(command)) {
    return;  // Do nothing if emu::daq::manager::Application is not under control.
  }
  
  // prepare a SOAP message
  xoap::MessageReference message = createCommandSOAP(command);
  xoap::MessageReference reply;
  
  // send the message
  // postSOAP() may throw an exception when failed.
  reply = getApplicationContext()->postSOAP(message, *appDescriptor_, *app);
  
  analyzeReply(message, reply, app);
}

void emu::supervisor::Application::sendCommandWithAttr(
					string command, std::map<string, string> attr, string klass)
  throw (xoap::exception::Exception, xcept::Exception)
{
  // Exceptions:
	// xoap exceptions are thrown by analyzeReply() for SOAP faults.
	// xdaq exceptions are thrown by postSOAP() for socket level errors.
  
	// find applications
  std::set<xdaq::ApplicationDescriptor *> apps;
  try {
    apps = getApplicationContext()->getDefaultZone()
      ->getApplicationDescriptors(klass);
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
	  return; // Do nothing if the target doesn't exist
	}

  if (klass == "emu::daq::manager::Application" && !isDAQManagerControlled(command)) {
    return;  // Do nothing if emu::daq::manager::Application is not under control.
	}
  
  // prepare a SOAP message
  xoap::MessageReference message = createCommandSOAPWithAttr(command, attr);
  xoap::MessageReference reply;
  
	// send the message one-by-one
  std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
  for (; i != apps.end(); ++i) {
    // postSOAP() may throw an exception when failed.
    reply = getApplicationContext()->postSOAP(message, *appDescriptor_, **i);
    
    analyzeReply(message, reply, *i);
  }
}

void emu::supervisor::Application::sendCommandCellOpInit()
  //throw (xoap::exception::Exception, xcept::Exception)
{

  xoap::MessageReference request; 
  xoap::MessageReference reply;
  
  if ( tf_descr_ == NULL ) return;
  
  // prepare a SOAP message  
  std::string sid="73";  
  std::string cid="10";
  std::string op="Configuration";
  std::string cb="NULL";
  std::string urn="NULL";
  std::string url="NULL";
  std::map<std::string, xdata::Serializable*> param;
//  std::string mnumb="310309";
//  param["KEY"] = new xdata::String(mnumb);
  param["KEY"] = new xdata::String(tf_key_);
  std::string ns="urn:ts-soap:3.0";
  std::string opId=TFCellOpName_.toString();
  bool async=false;

  request = doSoapOpInit(ns, cid, sid, async, op, param, cb, url, urn, opId);

  std::string tmp;
  xoap::dumpTree(request->getEnvelope(),tmp);
  std::cout << "--SOAP init message: " << tmp <<std::endl;
  
  std::cout << "sending the request" << std::endl;
  // send the message
  try{
    reply = getApplicationContext()->postSOAP(request, *getApplicationDescriptor(), *tf_descr_); 
    
    std::string tmp;
    xoap::dumpTree(reply->getEnvelope(),tmp);
    std::cout << "--SOAP init message reply: " << std::endl << tmp <<std::endl;
    
//     xdata::Serializable* serial = getPayload(reply);
//     std::string sresult = serial->toString();
  } catch (xcept::Exception& e){}
  
  return;
}

/*
  catch (xcept::Exception& e){
  std::cout << xcept::stdformat_exception_history(e) << std::endl;
  
  //(xcept::Exception &e) {std::cout << xcept::stdformat_exception_history(e) << std::endl;
  }
  std::cout << "the request has been sent" << std::endl;   
  //analyzeReply(request, reply, d);  
  return reply;
*/

void emu::supervisor::Application::sendCommandCell(string command)
  //throw (xoap::exception::Exception, xcept::Exception)
{
  
  xoap::MessageReference request; 
  xoap::MessageReference reply;
  
  if ( tf_descr_ == NULL ) return;

  
  std::string sid="73";  
  std::string cid="10";
  std::string op="Configuration";
  std::string cb="";
  std::string urn="";
  std::string url="";
  std::map<std::string, xdata::Serializable*> param;
//  std::string mnumb="310309";
//  param["KEY"] = new xdata::String(mnumb);
  param["KEY"] = new xdata::String(tf_key_);
  std::string ns="urn:ts-soap:3.0";
  std::string opid=TFCellOpName_.toString();
  bool async=false;
  
  std::cout << "preparing the sendcomm request" << std::endl;
  request = doSoapOpSendComand(ns, cid, sid, async, opid, command, param, cb, url, urn);
  std::string tmp;
  xoap::dumpTree(request->getEnvelope(),tmp);
  std::cout << "--SOAP message: " << tmp <<std::endl;

  // send the message
  // postSOAP() may throw an exception when failed.
  try{
    reply = getApplicationContext()->postSOAP(request, *getApplicationDescriptor(), *tf_descr_); 
    
//     xdata::Serializable* serial = getPayload(reply);
//     std::string sresult = serial->toString();
  } catch (xcept::Exception& e){}
  
  return;
}


xdata::Serializable* emu::supervisor::Application::getPayload(xoap::MessageReference msg) 
{ 
  
  xoap::SOAPBody body = msg->getSOAPPart().getEnvelope().getBody();
  if (body.hasFault() ) {
    std::ostringstream err;
    err << "SOAPFault found while getting payload from message. ";
    if ( body.getFault().hasDetail() ) {
      xoap::SOAPElement detail = body.getFault().getDetail();
      xcept::Exception rae;
      xdaq::XceptSerializer::importFrom (detail.getDOM(), rae);
      XCEPT_RETHROW(xcept::Exception, err.str(), rae);
    } else {
      err << body.getFault().getFaultString();
      XCEPT_RAISE(xcept::Exception,err.str());
      
    }
  }

  xdata::Serializable* ret = NULL; 
  DOMNode* node  = msg->getSOAPPart().getEnvelope().getBody().getDOMNode(); 
  DOMNodeList* bodyList = node->getChildNodes(); 

  //cout<< toolbox::toString("hola1"); 
  for (unsigned int i = 0; i < bodyList->getLength(); i++)  
    { 
      //cout << toolbox::toString("hola2"); 
      DOMNode* command = bodyList->item(i); 
      if (command->getNodeType() == DOMNode::ELEMENT_NODE) 
	{ 
	  std::string coma = xoap::XMLCh2String(command->getLocalName()); 
	  //cout << toolbox::toString("hola3%s",coma.c_str()); 
	  //we are at the command level 
	  DOMNodeList* bodyList2 = command->getChildNodes();
	  for (unsigned int j = 0; j < bodyList2->getLength(); j++)  
	    { 
	      //cout << toolbox::toString("hola4"); 
	      DOMNode* command2 = bodyList2->item(j); 
	      if (command2->getNodeType() == DOMNode::ELEMENT_NODE) 
		{ 
		  std::string com = xoap::XMLCh2String(command2->getLocalName()); 
		  
		  //cout << toolbox::toString("hola5,%s",com.c_str()); 
		  if(com=="payload") 

		    { 

		      try 
			{ 

			  xdata::soap::Serializer serializer; 
			  //cout << "hola6" << endl; 
			  //we must ask the type of the embedded object 
			  ret = analyse(command2); 
			  //xdata::String id; 
			  //cout << "hola7" << endl; 
			  //string a; 
			  //xoap::dumpTree(command2, a); 
			  //cout << "node payload: " << a << endl; 
			  //DOMElement* el2 = (DOMElement*) command2; 
			  //std::string type = xoap::XMLCh2String( el2->getAttributeNS (xoap::XStr(XSI_NAMESPACE_URI), xoap::XStr("type")));//(XStr("xsi:type")));//NS 
			  //cout << "TYPE: " << type << endl; 
			  serializer.import(ret, command2);//ret, command2); 
			   
			  //cout << "hola8" << endl;			   
			} catch (xdata::exception::Exception & xde) 
			  { 
			    std::ostringstream str; 
			    str << "Failed to de decode payload"; 
			    
			    XCEPT_RETHROW(xcept::Exception,str.str(),xde);  
			    
			    //XCEPT_RETHROW (tsexception::SoapParsingError, "Failed to de decode payload", xde); 
			  } 
		    } 
		} 
	    } 
	} 
    } 
  return ret; 
} 
 
xdata::Serializable* emu::supervisor::Application::analyse(DOMNode* command2) 
{ 
  xdata::Serializable* ret = NULL; 
  if(command2->hasAttributes()) 
    { 
      //cout << "hola9" << endl; 
      DOMNamedNodeMap* nodeMap = command2->getAttributes(); 
      //cout << "hola10" << endl; 
      std::string attrs = "xsi:type"; 
      //cout << "hola11" << endl; 
      XMLCh* attrx =  XMLString::transcode(attrs.c_str()); 
      //cout << "hola12" << endl; 
      DOMNode* asyncNode = nodeMap->getNamedItem(attrx); 
      //cout << "hola13" << endl; 
      if(asyncNode!=NULL) 
	{ 
	  const XMLCh* value = asyncNode->getNodeValue(); 
	  //cout << "hola14" << endl; 
	  std::string typev = xoap::XMLCh2String (value); 
	  //cout << "hola15 " << typev<<endl; 
	  if(typev=="xsd:integer") 
	  	{ 
	    ret = new xdata::Integer(); 
	  	} 
	  else if(typev=="xsd:unsignedShort") 
	    ret = new xdata::UnsignedShort(); 
	  else if(typev=="xsd:unsignedLong")                            
	    ret = new xdata::UnsignedLong(); 
	  else if(typev=="xsd:float") 
	    ret = new xdata::Float(); 
	  else if(typev=="xsd:double") 
	    ret = new xdata::Double(); 
	  else if(typev=="xsd:boolean") 
	    ret = new xdata::Boolean(); 
	  else if(typev=="xsd:unsignedInt") 
	    ret = new xdata::UnsignedInteger(); 
	  else if(typev=="xsd:string") 
	    { 
	      //cout << "hola16" << endl; 
	      xdata::String id; 
	      ret = new xdata::String(id); 
	      //cout << "hola17" << endl; 
	    } 
	  else if(typev=="soapenc:Struct") 
	    { 
	      ret = analyseSoapBag(command2); 
	    } 
	  else if(typev=="soapenc:Array") 
	    { 
	      ret = analyseSoapVector(command2); 
	    } 
	  //else 
	    //XCEPT_RAISE(tsexception::SoapEncodingError, "Not supported type " + typev); 
	} 
    } 
  else 
    { 
      std::string msg = "Payload in response message has no type, therefore it can not be de-serialized"; 
      //XCEPT_RAISE(tsexception::SoapEncodingError, msg); 
    } 
  return ret; 
} 

xdata::Serializable* emu::supervisor::Application::analyseSoapBag(DOMNode* command2) 
{ 
  xdata::Serializable* ret = NULL; 
  std::string msg = "Sorry, by the time being I can not de-serialize bags. :("; 
  //XCEPT_RAISE(tsexception::SoapParsingError, msg); 
  return ret; 
} 
 
xdata::Serializable* emu::supervisor::Application::analyseSoapVector(DOMNode* command2) 
{ 
  xdata::Serializable* ret = NULL; 
  std::string msg = "Sorry, by the time being I can not de-serialize vectors. :("; 
  //XCEPT_RAISE(tsexception::SoapParsingError, msg); 
  //ret = new xdata::Bag<xdata::Integer>; 
  return ret; 
} 
 

//////////////////////////////////////////////////////////////////////

xoap::MessageReference emu::supervisor::Application::doSoapOpInit(const std::string& ns, const std::string& cid, const std::string& sid, bool async, const std::string& op, std::map<std::string,xdata::Serializable*> param, const std::string& cb,const std::string& url,const std::string& urn, const std::string& opId){ 
  std::string asyncs; 
  if (async)  
    asyncs = "true"; 
  else  
    asyncs = "false"; 
    //xoap::MessageReference reply = getContext()->getApContext()->postSOAP(request, *getApplicationDescriptor(),*getTargetDescriptor());

  xoap::MessageReference msg = xoap::createMessage(); 
  try  
    { 
      xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope(); 
      envelope.addNamespaceDeclaration("soap-enc","http://schemas.xmlsoap.org/soap/encoding/"); 
      envelope.addNamespaceDeclaration("xsi","http://www.w3.org/2001/XMLSchema-instance"); 
      envelope.addNamespaceDeclaration("xsd","http://www.w3.org/2001/XMLSchema"); 
      
      xoap::SOAPBody body = envelope.getBody(); 
      xoap::SOAPName opInit = envelope.createName( "OpInit", "cell", ns); 
      xoap::SOAPElement element = body.addBodyElement(opInit); 
      
      element.addNamespaceDeclaration("cell",ns); 
      
      xoap::SOAPName commandId = envelope.createName("cid"); 
      element.addAttribute(commandId, cid); 
      xoap::SOAPName sesionId = envelope.createName("sid"); 
      element.addAttribute(sesionId, sid); 
      xoap::SOAPName asynchronous = envelope.createName("async"); 
      element.addAttribute(asynchronous, asyncs); 
      
      xoap::SOAPName opSoapName = envelope.createName( "operation"); 
      xoap::SOAPElement operationElement = element.addChildElement(opSoapName ); 
      operationElement.addTextNode(op); 
      if (!opId.empty()) 
	{ 
	  xoap::SOAPName opIdName = envelope.createName("opId"); 
	  operationElement.addAttribute(opIdName, opId); 
	} 
				 
      xoap::SOAPName callbackFunName = envelope.createName( "callbackFun"); 
      xoap::SOAPElement callbackFunElement = element.addChildElement ( callbackFunName ); 
      callbackFunElement.addTextNode(cb); 
      
      xoap::SOAPName callbackUrlName = envelope.createName( "callbackUrl"); 
      xoap::SOAPElement callbackUrlElement = element.addChildElement ( callbackUrlName ); 
      callbackUrlElement.addTextNode(url); 
      
      xoap::SOAPName callbackUrnName = envelope.createName( "callbackUrn"); 
      xoap::SOAPElement callbackUrnElement = element.addChildElement ( callbackUrnName ); 
      callbackUrnElement.addTextNode(urn); 
    } catch(xcept::Exception& e)  
      { 
	std::ostringstream str; 
	str << "Can not create the SOAP message in doOpInit"; 
	
	XCEPT_RETHROW(xcept::Exception,str.str(),e); 
      } 
  
  return msg; 
  
} 
/////////////////////////////////////////////////////////////////////////////////////////////
xoap::MessageReference emu::supervisor::Application::doSoapOpSendComand(const std::string& ns, const std::string& cid, const std::string& sid, bool async, const std::string& opid, const std::string& command, std::map<std::string,xdata::Serializable*> param, const std::string& cb,const std::string& url,const std::string& urn) 
{ 
  
  std::string asyncs; 
  if (async)
    asyncs = "true"; 
  else  
    asyncs = "false"; 
	   xoap::MessageReference msg = xoap::createMessage();
	   try  
	     {
	       xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope(); 
	       envelope.addNamespaceDeclaration("soap-enc","http://schemas.xmlsoap.org/soap/encoding/"); 
	       envelope.addNamespaceDeclaration("xsi","http://www.w3.org/2001/XMLSchema-instance"); 
	       envelope.addNamespaceDeclaration("xsd","http://www.w3.org/2001/XMLSchema"); 
		 
	       xoap::SOAPBody body = envelope.getBody(); 
	       xoap::SOAPName opSendCommand = envelope.createName( "OpSendCommand", "cell", ns); 
	       xoap::SOAPElement element = body.addBodyElement(opSendCommand); 
	       
	       element.addNamespaceDeclaration("cell",ns); 
	       
	       xoap::SOAPName commandId = envelope.createName("cid"); 
	       element.addAttribute(commandId, cid); 
	       xoap::SOAPName sesionId = envelope.createName("sid"); 
	       element.addAttribute(sesionId, sid); 
	       xoap::SOAPName asynchronous = envelope.createName("async"); 
	       element.addAttribute(asynchronous, asyncs); 
	       
	       xoap::SOAPName opSoapName = envelope.createName( "operation"); 
	       xoap::SOAPElement operationElement = element.addChildElement(opSoapName ); 
	       operationElement.addTextNode(opid); 
	       
	       xoap::SOAPName commandName = envelope.createName( "command"); 
	       xoap::SOAPElement commandElement = element.addChildElement ( commandName ); 
	       commandElement.addTextNode(command); 
	       for (std::map<std::string,xdata::Serializable*>::const_iterator i = param.begin(); i != param.end(); ++i) 
		 { 
		   std::string mytype=""; 
    		if(i->second->type()=="int")  
		  mytype="integer"; 
    		else if(i->second->type()=="string")  
		  mytype="string"; 
    		else if (i->second->type()=="bool")  
		  mytype="boolean"; 
		else if (i->second->type()=="unsigned long")  
		  mytype="unsignedLong"; 
		else if (i->second->type()=="unsigned short")  
		  mytype="unsignedShort"; 
		else if (i->second->type()=="unsigned int")  
		  mytype="unsignedInt"; 
		else 
		  mytype=i->second->type(); 
		xoap::SOAPName paramName = envelope.createName( "param", "cell", ns); 
		xoap::SOAPElement paramElement = element.addChildElement ( paramName ); 
		
		xoap::SOAPName typeName = envelope.createName("type","xsi","http://www.w3.org/2001/XMLSchema-instance"); 
		paramElement.addAttribute(typeName, "xsd:" + mytype);
		
		xoap::SOAPName nameName = envelope.createName("name"); 
		paramElement.addAttribute(nameName, i->first); 
		
    		paramElement.addTextNode(i->second->toString()); 
		 }  
	       
	       xoap::SOAPName callbackFunName = envelope.createName( "callbackFun"); 
	       xoap::SOAPElement callbackFunElement = element.addChildElement ( callbackFunName ); 
	       callbackFunElement.addTextNode(cb); 
	       
	       xoap::SOAPName callbackUrlName = envelope.createName( "callbackUrl"); 
	       xoap::SOAPElement callbackUrlElement = element.addChildElement ( callbackUrlName ); 
	       callbackUrlElement.addTextNode(url); 
	       
	       xoap::SOAPName callbackUrnName = envelope.createName( "callbackUrn"); 
	       xoap::SOAPElement callbackUrnElement = element.addChildElement ( callbackUrnName ); 
	       callbackUrnElement.addTextNode(urn); 
	       
	     } catch(xcept::Exception& e)  
	       { 
		 std::ostringstream str; 
		 str << "Can not create the SOAP message in doOpSendCommand"; 
		 
		 XCEPT_RETHROW(xcept::Exception,str.str(),e);  
	       } 
	   
	   return msg; 
} 
void emu::supervisor::Application::sendCommandCellOpkill()
  //throw (xoap::exception::Exception, xcept::Exception)
{
  xoap::MessageReference request; 
  xoap::MessageReference reply;
  
  if ( tf_descr_ == NULL ) return;

  
  // prepare a SOAP message  
  std::string sid="73";  
  std::string cid="10";
  std::string op=TFCellOpName_.toString();//"Configuration";
  std::string cb="NULL";
  std::string urn="NULL";
  std::string url="NULL";
  std::string ns="urn:ts-soap:3.0";
  bool async=false;
  
  request = doSoapOpKill(ns, cid, sid, async, op, cb,url,urn);
  std::string tmp;
  xoap::dumpTree(request->getEnvelope(),tmp);
  std::cout << "--SOAP kill message: " <<std::endl << tmp <<std::endl;
  
  // send the message
  try{
    reply = getApplicationContext()->postSOAP(request, *getApplicationDescriptor(),*tf_descr_);

    std::string tmp;
    xoap::dumpTree(reply->getEnvelope(),tmp);
    std::cout << "--SOAP message killreply: " <<std::endl<< tmp <<std::endl;

//     xdata::Serializable* serial = getPayload(reply);
//     std::string sresult = serial->toString();
  } catch (xcept::Exception& e){}
  
  return;
}

xoap::MessageReference emu::supervisor::Application::doSoapOpKill(const std::string& ns, const std::string& cid, const std::string& sid, bool async, const std::string& op, const std::string& cb,const std::string& url,const std::string& urn) 
{ 
	std::string asyncs; 
	if(async)  
		asyncs = "true"; 
	else  
		asyncs = "false"; 
  	 
  	xoap::MessageReference msg = xoap::createMessage(); 
	try  
	{ 
		xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope(); 
  		envelope.addNamespaceDeclaration("soap-enc","http://schemas.xmlsoap.org/soap/encoding/"); 
		envelope.addNamespaceDeclaration("xsi","http://www.w3.org/2001/XMLSchema-instance"); 
		envelope.addNamespaceDeclaration("xsd","http://www.w3.org/2001/XMLSchema"); 
		 
		xoap::SOAPBody body = envelope.getBody(); 

		
  		xoap::SOAPName opKill = envelope.createName( "OpKill", "cell", ns); 
  		xoap::SOAPElement element = body.addBodyElement(opKill); 
  		 
  		element.addNamespaceDeclaration("cell",ns); 
  		 
  		xoap::SOAPName commandId = envelope.createName("cid"); 
		element.addAttribute(commandId, cid); 
		xoap::SOAPName sesionId = envelope.createName("sid"); 
		element.addAttribute(sesionId, sid); 
		xoap::SOAPName asynchronous = envelope.createName("async"); 
		element.addAttribute(asynchronous, asyncs); 
		 
		xoap::SOAPName opSoapName = envelope.createName( "operation"); 
		xoap::SOAPElement operationElement = element.addChildElement(opSoapName ); 
		operationElement.addTextNode(op); 
						 
		xoap::SOAPName callbackFunName = envelope.createName( "callbackFun"); 
		xoap::SOAPElement callbackFunElement = element.addChildElement ( callbackFunName ); 
		callbackFunElement.addTextNode(cb); 
		 
		xoap::SOAPName callbackUrlName = envelope.createName( "callbackUrl"); 
		xoap::SOAPElement callbackUrlElement = element.addChildElement ( callbackUrlName ); 
		callbackUrlElement.addTextNode(url); 
		 
		xoap::SOAPName callbackUrnName = envelope.createName( "callbackUrn"); 
		xoap::SOAPElement callbackUrnElement = element.addChildElement ( callbackUrnName ); 
		callbackUrnElement.addTextNode(urn); 
	} catch(xcept::Exception& e)  
	{ 
   		std::ostringstream str; 
   		str << "Can not create the SOAP message in doOpInit"; 

		XCEPT_RETHROW(xcept::Exception,str.str(),e);  

	} 
	 
//	std::string msgstr; 
//	xoap::dumpTree(msg->getEnvelope(),msgstr); 
//	std::cout << "doSoapOpKill: " << msgstr << std::endl; 
	 
	return msg; 
	 
}
std::string emu::supervisor::Application::OpGetStateCell()
  //throw (xoap::exception::Exception, xcept::Exception)
{

  xoap::MessageReference request; 
  xoap::MessageReference reply;
  
  if ( tf_descr_ == NULL ) return string("");

  
  // prepare a SOAP message  
  std::string sid="73";  
  std::string cid="10";
  std::string cb="NULL";
  std::string urn="NULL";
  std::string url="NULL";
  std::string ns="urn:ts-soap:3.0";
  std::string opid=TFCellOpName_.toString();
  bool async=false;
  std::string sresult="UNKNOWN" ;
  request = doSoapOpGetState(ns, cid, sid, async, opid, cb, url, urn);
  std::string tmp;
  xoap::dumpTree(request->getEnvelope(),tmp);
  std::cout << "--SOAP message getstate: " << std::endl << tmp <<std::endl;
  
  std::cout << "sending the request" << std::endl;
  // send the message
  try{
    reply = getApplicationContext()->postSOAP(request, *getApplicationDescriptor(),*tf_descr_);

    //std::string tmp;
    //xoap::dumpTree(reply->getEnvelope(),tmp);
    //std::cout << "--SOAP message: " << tmp <<std::endl;

    xdata::Serializable* serial = getPayload(reply);
    sresult = serial->toString();
    std::string tmp;
    xoap::dumpTree(reply->getEnvelope(),tmp);
    std::cout << "--SOAP getstate reply message: " << std::endl << tmp <<std::endl;
  } catch (xcept::Exception& e){
    return "UNKNOWN";
  }
  std::cout << "--I am here 3 --> sresult ==  " << sresult <<std::endl;

  return sresult;
}

void emu::supervisor::Application::OpResetCell()
  //throw (xoap::exception::Exception, xcept::Exception)
{

  xoap::MessageReference request; 
  xoap::MessageReference reply;

  if ( tf_descr_ == NULL ) return;

    
  std::string sid="73";  
  std::string cid="10";
  std::string op="Configuration";
  std::string cb="";
  std::string urn="";
  std::string url="";
  std::string ns="urn:ts-soap:3.0";
  std::string opid=TFCellOpName_.toString();
  bool async=false;
  
  std::cout << "preparing the sendcomm request" << std::endl;
  request = doSoapOpReset(ns, cid, sid, async, opid, cb, url, urn);
  std::string tmp;
  xoap::dumpTree(request->getEnvelope(),tmp);
  std::cout << "--SOAP reset message: " << std::endl << tmp <<std::endl;

  // send the message
  // postSOAP() may throw an exception when failed.
  try{
    reply = getApplicationContext()->postSOAP(request, *getApplicationDescriptor(), *tf_descr_);

  std::string tmp;
  xoap::dumpTree(reply->getEnvelope(),tmp);
  std::cout << "--SOAP reset message reply: " << std::endl << tmp <<std::endl;

//     xdata::Serializable* serial = getPayload(reply);
//     std::string sresult = serial->toString();
  } catch (xcept::Exception& e){}
  
  return;
}    
xoap::MessageReference emu::supervisor::Application::doSoapOpReset(const std::string& ns, const std::string& cid, const std::string& sid, bool async, const std::string& op, const std::string& cb,const std::string& url,const std::string& urn) 
{ 
	std::string asyncs; 
	if(async)  
		asyncs = "true"; 
	else  
		asyncs = "false"; 
   
 
	xoap::MessageReference msg = xoap::createMessage(); 
	try  
	{ 
		xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope(); 
  		envelope.addNamespaceDeclaration("soap-enc","http://schemas.xmlsoap.org/soap/encoding/"); 
		envelope.addNamespaceDeclaration("xsi","http://www.w3.org/2001/XMLSchema-instance"); 
		envelope.addNamespaceDeclaration("xsd","http://www.w3.org/2001/XMLSchema"); 
		 
		xoap::SOAPBody body = envelope.getBody(); 
  		xoap::SOAPName opReset = envelope.createName( "OpReset", "cell", ns); 
  		xoap::SOAPElement element = body.addBodyElement(opReset); 
  		 
  		element.addNamespaceDeclaration("cell",ns); 
  		 
  		xoap::SOAPName commandId = envelope.createName("cid"); 
		element.addAttribute(commandId, cid); 
		xoap::SOAPName sesionId = envelope.createName("sid"); 
		element.addAttribute(sesionId, sid); 
		xoap::SOAPName asynchronous = envelope.createName("async"); 
		element.addAttribute(asynchronous, asyncs); 
		 
		xoap::SOAPName opSoapName = envelope.createName( "operation"); 
		xoap::SOAPElement operationElement = element.addChildElement(opSoapName ); 
		operationElement.addTextNode(op); 
						 
		xoap::SOAPName callbackFunName = envelope.createName( "callbackFun"); 
		xoap::SOAPElement callbackFunElement = element.addChildElement ( callbackFunName ); 
		callbackFunElement.addTextNode(cb); 
		 
		xoap::SOAPName callbackUrlName = envelope.createName( "callbackUrl"); 
		xoap::SOAPElement callbackUrlElement = element.addChildElement ( callbackUrlName ); 
		callbackUrlElement.addTextNode(url); 
		 
		xoap::SOAPName callbackUrnName = envelope.createName( "callbackUrn"); 
		xoap::SOAPElement callbackUrnElement = element.addChildElement ( callbackUrnName ); 
		callbackUrnElement.addTextNode(urn); 
	} catch(xcept::Exception& e)  
	{ 
   		std::ostringstream str; 
   		str << "Can not create the SOAP message in doOpReset"; 
   		 
   		XCEPT_RETHROW(xcept::Exception,str.str(),e); 
	} 
	 
//	std::string msgstr; 
//	xoap::dumpTree(msg->getEnvelope(),msgstr); 
//	std::cout << "doSoapOpReset: " <<  msgstr << std::endl; 
	 
	return msg; 
}  
xoap::MessageReference emu::supervisor::Application::doSoapOpGetState(const std::string& ns, const std::string& cid, const std::string& sid, bool async, const std::string& opid, const std::string& cb,const std::string& url,const std::string& urn) 
  { 
	std::string asyncs; 
	if (async)  
		asyncs = "true"; 
	else  
		asyncs = "false"; 
 
	xoap::MessageReference msg = xoap::createMessage(); 
	try  
	{ 
		xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope(); 
  		envelope.addNamespaceDeclaration("soap-enc","http://schemas.xmlsoap.org/soap/encoding/"); 
		envelope.addNamespaceDeclaration("xsi","http://www.w3.org/2001/XMLSchema-instance"); 
		envelope.addNamespaceDeclaration("xsd","http://www.w3.org/2001/XMLSchema"); 
		 
		xoap::SOAPBody body = envelope.getBody();

		xoap::SOAPName opGetState = envelope.createName( "OpGetState", "cell", ns); 
  		xoap::SOAPElement element = body.addBodyElement(opGetState); 
  		 
  		element.addNamespaceDeclaration("cell",ns); 
  		 
  		xoap::SOAPName commandId = envelope.createName("cid"); 
		element.addAttribute(commandId, cid); 
		xoap::SOAPName sesionId = envelope.createName("sid"); 
		element.addAttribute(sesionId, sid); 
		xoap::SOAPName asynchronous = envelope.createName("async"); 
		element.addAttribute(asynchronous, asyncs); 
		 
		xoap::SOAPName opSoapName = envelope.createName( "operation"); 
		xoap::SOAPElement operationElement = element.addChildElement(opSoapName ); 
		operationElement.addTextNode(opid); 
		 
		xoap::SOAPName callbackFunName = envelope.createName( "callbackFun"); 
		xoap::SOAPElement callbackFunElement = element.addChildElement ( callbackFunName ); 
		callbackFunElement.addTextNode(cb); 
		 
		xoap::SOAPName callbackUrlName = envelope.createName( "callbackUrl"); 
		xoap::SOAPElement callbackUrlElement = element.addChildElement ( callbackUrlName ); 
		callbackUrlElement.addTextNode(url); 
		 
		xoap::SOAPName callbackUrnName = envelope.createName( "callbackUrn"); 
		xoap::SOAPElement callbackUrnElement = element.addChildElement ( callbackUrnName ); 
		callbackUrnElement.addTextNode(urn); 
	} catch(xcept::Exception& e)  
	{ 
   		std::ostringstream str; 
   		str << "Can not create the SOAP message in doOpGetState"; 
   		 
   		//XCEPT_RETHROW(tsexception::SoapEncodingError,str.str(),e); 
	} 
//	std::string msgstr; 
//	xoap::dumpTree(msg->getEnvelope(),msgstr); 
//	std::cout << "doOpGetState: " << msgstr << std::endl; 
//	 
	return msg; 
} 
xoap::MessageReference emu::supervisor::Application::createCommandSOAP(string command)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	xoap::SOAPName name = envelope.createName(command, "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement(name);

	return message;
}

xoap::MessageReference emu::supervisor::Application::createCommandSOAPWithAttr(
		string command, std::map<string, string> attr)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	xoap::SOAPName name = envelope.createName(command, "xdaq", XDAQ_NS_URI);
	xoap::SOAPElement element = envelope.getBody().addBodyElement(name);

	std::map<string, string>::iterator i;
	for (i = attr.begin(); i != attr.end(); ++i) {
		xoap::SOAPName p = envelope.createName((*i).first, "xdaq", XDAQ_NS_URI);
		element.addAttribute(p, (*i).second);
	}

	return message;
}

bool emu::supervisor::Application::waitForTFCellOpToReach( const string targetState, const unsigned int seconds ){
  if ( tf_descr_ == NULL ) return false;

  // Poll, and return TRUE if and only if DAQ gets into the expected state before timeout.
  for ( unsigned int i=0; i<=seconds; ++i ){
    TFCellOpState_ = OpGetStateCell();
    if ( TFCellOpState_.toString() == targetState ){ return true; }
    LOG4CPLUS_INFO( logger_, "Waited " << i << " sec so far for TF Cell Operation " 
		    << TFCellOpName_.toString() << " to get " << targetState 
		    << ". It is still in " << TFCellOpState_.toString() << " state." );
    ::sleep(1);
  }

  LOG4CPLUS_ERROR( logger_, "Timeout after waiting " << seconds << " sec so far for TF Cell Operation " 
		   << TFCellOpName_.toString() << " to get " << targetState 
		   << ". It is still in " << TFCellOpState_.toString() << " state." );

  stringstream ss10;
  ss10 <<  "Timeout after waiting " << seconds << " sec so far for TF Cell Operation "
       << TFCellOpName_.toString() << " to get "<< targetState 
       << ". It is still in " << TFCellOpState_.toString() << " state.";
  XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss10.str() );
  this->notifyQualified( "error", eObj );
  return false;
}

void emu::supervisor::Application::setParameter(
		string klass, string name, string type, string value)
{
	// find applications
	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = getApplicationContext()->getDefaultZone()
				->getApplicationDescriptors(klass);
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
		return; // Do nothing if the target doesn't exist
	}

	// prepare a SOAP message
	xoap::MessageReference message = createParameterSetSOAP(
			klass, name, type, value);
	xoap::MessageReference reply;

	// send the message one-by-one
	std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
	for (; i != apps.end(); ++i) {
		reply = getApplicationContext()->postSOAP(message, *appDescriptor_, **i);
		analyzeReply(message, reply, *i);
	}
}

xoap::MessageReference emu::supervisor::Application::createParameterSetSOAP(
		string klass, string name, string type, string value)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	envelope.addNamespaceDeclaration("xsi", NS_XSI);

	xoap::SOAPName command = envelope.createName(
			"ParameterSet", "xdaq", XDAQ_NS_URI);
	xoap::SOAPName properties = envelope.createName(
			"properties", "xapp", "urn:xdaq-application:" + klass);
	xoap::SOAPName parameter = envelope.createName(
			name, "xapp", "urn:xdaq-application:" + klass);
	xoap::SOAPName xsitype = envelope.createName("type", "xsi", NS_XSI);

	xoap::SOAPElement properties_e = envelope.getBody()
			.addBodyElement(command)
			.addChildElement(properties);
	properties_e.addAttribute(xsitype, "soapenc:Struct");

	xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
	parameter_e.addAttribute(xsitype, type);
	parameter_e.addTextNode(value);

	return message;
}

xoap::MessageReference emu::supervisor::Application::createParameterGetSOAP(
		string klass, string name, string type)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	envelope.addNamespaceDeclaration("xsi", NS_XSI);

	xoap::SOAPName command = envelope.createName(
			"ParameterGet", "xdaq", XDAQ_NS_URI);
	xoap::SOAPName properties = envelope.createName(
			"properties", "xapp", "urn:xdaq-application:" + klass);
	xoap::SOAPName parameter = envelope.createName(
			name, "xapp", "urn:xdaq-application:" + klass);
	xoap::SOAPName xsitype = envelope.createName("type", "xsi", NS_XSI);

	xoap::SOAPElement properties_e = envelope.getBody()
			.addBodyElement(command)
			.addChildElement(properties);
	properties_e.addAttribute(xsitype, "soapenc:Struct");

	xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
	parameter_e.addAttribute(xsitype, type);
	parameter_e.addTextNode("");

	return message;
}

xoap::MessageReference emu::supervisor::Application::createParameterGetSOAP(
		string klass, std::map<string, string> name_type)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	envelope.addNamespaceDeclaration("xsi", NS_XSI);

	xoap::SOAPName command = envelope.createName(
			"ParameterGet", "xdaq", XDAQ_NS_URI);
	xoap::SOAPName properties = envelope.createName(
			"properties", "xapp", "urn:xdaq-application:" + klass);
	xoap::SOAPName xsitype = envelope.createName("type", "xsi", NS_XSI);

	xoap::SOAPElement properties_e = envelope.getBody()
			.addBodyElement(command)
			.addChildElement(properties);
	properties_e.addAttribute(xsitype, "soapenc:Struct");

	std::map<string, string>::iterator i;
	for (i = name_type.begin(); i != name_type.end(); ++i) {
		xoap::SOAPName n = envelope.createName(
				(*i).first, "xapp", "urn:xdaq-application:" + klass);
		xoap::SOAPElement e = properties_e.addChildElement(n);
		e.addAttribute(xsitype, (*i).second);
		e.addTextNode("");
	}

	return message;
}

void emu::supervisor::Application::analyzeReply(
		xoap::MessageReference message, xoap::MessageReference reply,
		xdaq::ApplicationDescriptor *app)
{
	string message_str, reply_str;

	reply->writeTo(reply_str);
	ostringstream s;
	s << "Reply from "
			<< app->getClassName() << "(" << app->getInstance() << ")" << endl
			<< reply_str;
	// last_log_.add(s.str());
	LOG4CPLUS_DEBUG(logger_, reply_str);

	xoap::SOAPBody body = reply->getSOAPPart().getEnvelope().getBody();

	// do nothing when no fault
	if (!body.hasFault()) { return; }

	ostringstream error;

	error << "SOAP message: " << endl;
	message->writeTo(message_str);
	error << message_str << endl;
	error << "Fault string: " << endl;
	error << reply_str << endl;

	LOG4CPLUS_ERROR(logger_, error.str());
	// Instead decide in the calling function whether or not to notify Sentinel.
	//   stringstream ss4;
	//   ss4 <<  error.str();
	//   XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss4.str() );
	//   this->notifyQualified( "error", eObj );
	XCEPT_RAISE(xoap::exception::Exception, "SOAP fault: \n" + reply_str);

	return;
}

string emu::supervisor::Application::extractParameter(
		xoap::MessageReference message, string name)
{
	xoap::SOAPElement root = message->getSOAPPart()
			.getEnvelope().getBody().getChildElements(
			*(new xoap::SOAPName("ParameterGetResponse", "", "")))[0];
	xoap::SOAPElement properties = root.getChildElements(
			*(new xoap::SOAPName("properties", "", "")))[0];
	xoap::SOAPElement parameter = properties.getChildElements(
			*(new xoap::SOAPName(name, "", "")))[0];

	return parameter.getValue();
}

void emu::supervisor::Application::refreshConfigParameters()
{
	daq_mode_ = getDAQMode();
	TFCellOpState_ = OpGetStateCell();
	ttc_source_ = getTTCciSource();
}

string emu::supervisor::Application::getCGIParameter(xgi::Input *in, string name)
{
	cgicc::Cgicc cgi(in);
	string value;

	form_iterator i = cgi.getElement(name);
	if (i != cgi.getElements().end()) {
		value = (*i).getValue();
	}

	return value;
}

int emu::supervisor::Application::keyToIndex(const string key)
{
	int index = -1;

	for (unsigned int i = 0; i < config_keys_.size(); ++i) {
		if (config_keys_[i] == key) {
			index = i;
			break;
		}
	}

	return index;
}

string emu::supervisor::Application::getCrateConfig(const string type, const string key) const
{
	xdata::Vector<xdata::String> keys;
	xdata::Vector<xdata::String> values;

	if (type == "PC") {
		keys = pc_keys_;
		values = pc_configs_;
	} else if (type == "FC") {
		keys = fc_keys_;
		values = fc_configs_;
	} else {
		return "";
	}

	string result = "";
	for (unsigned int i = 0; i < keys.size(); ++i) {
		if (keys[i] == key) {
			result = values[i];
			break;
		}
	}

	return result;
}

bool emu::supervisor::Application::isCalibrationMode()
{
	return (getCalibParamIndex(run_type_) >= 0);
}

bool emu::supervisor::Application::isAlctCalibrationMode()
{
	std::cout << "isAlctCalibMode: runtype: " << run_type_.toString() << "index" << getCalibParamIndex(run_type_);
	bool res = run_type_.toString().find("ALCT") != string::npos;
	std::cout << "isAlctCalibMode result: " << res << std::endl;
	return res;
}

int emu::supervisor::Application::getCalibParamIndex(const string name)
{
	int result = -1;

	for (size_t i = 0; i < calib_params_.size(); ++i) {
		if (calib_params_[i].bag.key_ == name) {
			result = i;
			break;
		}
	}

	return result;
}

string emu::supervisor::Application::trim(string orig) const
{
	string s = orig;

	s.erase(0, s.find_first_not_of(" \t\n"));
	s.erase(s.find_last_not_of(" \t\n") + 1);

	return s;
}

string emu::supervisor::Application::toString(const long int i) const
{
	ostringstream s;
	s << i;

	return s.str();
}

string emu::supervisor::Application::getDAQMode()
{
	string result = "";

	if (daq_descr_ != NULL) {

	  std::map<string, string> m;
	  m["supervisedMode"] = "xsd:boolean";
	  m["configuredInSupervisedMode"] = "xsd:boolean";
	  m["daqState"] = "xsd:string";
	  xoap::MessageReference daq_param = createParameterGetSOAP("emu::daq::manager::Application", m);

	  xoap::MessageReference reply;
	  try {
	    reply = getApplicationContext()->postSOAP(daq_param, *appDescriptor_, *daq_descr_);
	    analyzeReply(daq_param, reply, daq_descr_);
	    
	    result = extractParameter(reply, "supervisedMode");
	    result = (result == "true") ? "supervised" : "unsupervised";
	    REVOKE_ALARM( "noLocalDAQ", NULL );
	  } catch (xcept::Exception e) {
	    LOG4CPLUS_INFO(logger_, "Failed to get local DAQ mode. "
			    << xcept::stdformat_exception_history(e));
	    RAISE_ALARM( emu::supervisor::alarm::NoLocalDAQ, "noLocalDAQ", "warn", "Local DAQ is in down or inaccessible.", "", &logger_ );
	    result = "UNKNOWN";
	  }

	}

	return result;
}

string emu::supervisor::Application::getLocalDAQState()
{
	string result = "";

	if (daq_descr_ != NULL) {

        	std::map<string, string> m;
		m["supervisedMode"] = "xsd:boolean";
		m["configuredInSupervisedMode"] = "xsd:boolean";
		m["daqState"] = "xsd:string";
		xoap::MessageReference daq_param = createParameterGetSOAP("emu::daq::manager::Application", m);

		xoap::MessageReference reply;
		try {
			reply = getApplicationContext()->postSOAP(daq_param, *appDescriptor_, *daq_descr_);
			analyzeReply(daq_param, reply, daq_descr_);
			result = extractParameter(reply, "daqState");
			REVOKE_ALARM( "noLocalDAQ", NULL );
		} catch (xcept::Exception e) {
			LOG4CPLUS_INFO(logger_, "Failed to get local DAQ state. "
					<< xcept::stdformat_exception_history(e));
			RAISE_ALARM( emu::supervisor::alarm::NoLocalDAQ, "noLocalDAQ", "warn", "Local DAQ is in down or inaccessible.", "", &logger_ );
			result = "UNKNOWN";
		}
	}

	return result;
}


string emu::supervisor::Application::getTTCciSource()
{
	string result = "";

	if (ttc_descr_ == NULL) return result; // Do nothing if the target doesn't exist

	std::map<string, string> m;
	m["ClockSource"] = "xsd:string";
	m["OrbitSource"] = "xsd:string";
	m["TriggerSource"] = "xsd:string";
	m["BGOSource"] = "xsd:string";
	xoap::MessageReference ttc_param = createParameterGetSOAP("TTCciControl", m);

	xoap::MessageReference reply;
	try {
		reply = getApplicationContext()->postSOAP(ttc_param, *appDescriptor_, *ttc_descr_);
		analyzeReply(ttc_param, reply, ttc_descr_);

		result = extractParameter(reply, "ClockSource");
		result += ":" + extractParameter(reply, "OrbitSource");
		result += ":" + extractParameter(reply, "TriggerSource");
		result += ":" + extractParameter(reply, "BGOSource");
	} catch (xcept::Exception e) {
		result = "UNKNOWN";
	}

	return result;
}

bool emu::supervisor::Application::isDAQConfiguredInSupervisedMode()
{
	string result = "";

	if (daq_descr_ != NULL) {

 	        std::map<string, string> m;
		m["supervisedMode"] = "xsd:boolean";
		m["configuredInSupervisedMode"] = "xsd:boolean";
		m["daqState"] = "xsd:string";
		xoap::MessageReference daq_param = createParameterGetSOAP("emu::daq::manager::Application", m);

		xoap::MessageReference reply;
		try {
			reply = getApplicationContext()->postSOAP(daq_param, *appDescriptor_, *daq_descr_);
			analyzeReply(daq_param, reply, daq_descr_);

			result = extractParameter(reply, "configuredInSupervisedMode");
		} catch (xcept::Exception e) {
			LOG4CPLUS_ERROR(logger_, "Failed to get \"configuredInSupervisedMode\" from emu::daq::manager::Application. "
					<< xcept::stdformat_exception_history(e));
			stringstream ss8;
			ss8 <<  "Failed to get \"configuredInSupervisedMode\" from emu::daq::manager::Application. "
					;
			XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss8.str(), e );
			this->notifyQualified( "error", eObj );
			result = "UNKNOWN";
		}
	}

	return result == "true";
}

bool emu::supervisor::Application::waitForDAQToExecute( const string command, const unsigned int seconds, const bool poll ){
  string expectedState;
  if      ( command == "Configure" ){ expectedState = "Ready";   }
  else if ( command == "Enable"    ){ expectedState = "Enabled"; }
  else if ( command == "Halt"      ){ expectedState = "Halted";  }
  else                              { return true; }

  // If not polling (default), just wait and return TRUE:
  if ( !poll ){
    ::sleep( seconds );
    return true;
  }

  // Poll, and return TRUE if and only if DAQ gets into the expected state before timeout.
  string localDAQState;
  for ( unsigned int i=0; i<=seconds; ++i ){
    localDAQState = getLocalDAQState();
    if ( localDAQState != "Halted"  && localDAQState != "Ready" && 
	 localDAQState != "Enabled" && localDAQState != "INDEFINITE" ){
      LOG4CPLUS_ERROR( logger_, "Local DAQ is in " << localDAQState << " state. Please destroy and recreate local DAQ." );
      stringstream ss9;
      ss9 <<  "Local DAQ is in " << localDAQState << " state. Please destroy and recreate local DAQ." ;
      XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss9.str() );
      this->notifyQualified( "error", eObj );
      return false;
    }
    if ( localDAQState == expectedState ){ return true; }
    LOG4CPLUS_INFO( logger_, "Waited " << i << " sec so far for local DAQ to get " 
		    << expectedState << ". It is still in " << localDAQState << " state." );
    ::sleep(1);
  }

  LOG4CPLUS_ERROR( logger_, "Timeout after waiting " << seconds << " sec for local DAQ to get " << expectedState 
		   << ". It is in " << localDAQState << " state." );
  stringstream ss10;
  ss10 <<  "Timeout after waiting " << seconds << " sec for local DAQ to get " << expectedState 
		   << ". It is in " << localDAQState << " state." ;
  XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss10.str() );
  this->notifyQualified( "error", eObj );
  return false;
}

bool emu::supervisor::Application::isDAQManagerControlled(string command)
{
	// No point in sending any command when DAQ is in an irregular state (failed, indefinite, ...)
        string localDAQState = getLocalDAQState();
	if ( localDAQState != "Halted" && localDAQState != "Ready" && localDAQState != "Enabled" ){
	  LOG4CPLUS_WARN( logger_, "No command \"" << command << "\" sent to emu::daq::manager::Application because local DAQ is in " 
			  << localDAQState << " state. Please destroy and recreate local DAQ." );
	  stringstream ss11;
	  ss11 <<  "No command \"" << command << "\" sent to emu::daq::manager::Application because local DAQ is in " 
			   << localDAQState << " state. Please destroy and recreate local DAQ." ;
	  XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss11.str() );
	  this->notifyQualified( "warn", eObj );
	  return false;
	}

	// Enforce "Halt" irrespective of DAQ mode.
	// if (command == "Halt") { return true; }

	// Don't send any other command when DAQ is in unsupervised mode.
	if (getDAQMode() != "supervised") { return false; }

	// And don't send any other command when DAQ was configured in unsupervised mode, either.
	if (command != "Configure" && !isDAQConfiguredInSupervisedMode()) { return false; }

	return true;
}

emu::supervisor::Application::StateTable::StateTable(emu::supervisor::Application *sv) : app_(sv) {}

void emu::supervisor::Application::StateTable::addApplication(string klass)
{
	// find applications
	std::set<xdaq::ApplicationDescriptor *> apps;
	try {
		apps = app_->getApplicationContext()->getDefaultZone()
				->getApplicationDescriptors(klass);
	} catch (xdaq::exception::ApplicationDescriptorNotFound e) {
		return; // Do nothing if the target doesn't exist
	}

	// add to the table
	std::set<xdaq::ApplicationDescriptor *>::iterator i = apps.begin();
	for (; i != apps.end(); ++i) {
		table_.push_back(
				pair<xdaq::ApplicationDescriptor *, string>(*i, "NULL"));
	}
}

void emu::supervisor::Application::StateTable::refresh()
{
	string klass = "";
	xoap::MessageReference message, reply;

	vector<pair<xdaq::ApplicationDescriptor *, string> >::iterator i =
			table_.begin();
	for (; i != table_.end(); ++i) {
		if (klass != i->first->getClassName()) {
			klass = i->first->getClassName();
			message = createStateSOAP(klass);
		}

		try {
			reply = app_->getApplicationContext()->postSOAP(message, *app_->appDescriptor_, *i->first);
			app_->analyzeReply(message, reply, i->first);

			i->second = extractState(reply, klass);
		} catch (xcept::Exception e) {
			i->second = STATE_UNKNOWN;
			LOG4CPLUS_ERROR(app_->logger_, "Exception when trying to get state of "
					<< klass << ": " << xcept::stdformat_exception_history(e));
			stringstream ss12;
			ss12 << "Exception when trying to get state of "
			     << klass << ": " ;
			XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss12.str(), e );
			app_->notifyQualified( "error", eObj );
		} catch (...) {
			LOG4CPLUS_ERROR(app_->logger_, "Unknown exception when trying to get state of " << klass);
			stringstream ss13;
			ss13 << "Unknown exception when trying to get state of " << klass;
			XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss13.str() );
			app_->notifyQualified( "error", eObj );
			i->second = STATE_UNKNOWN;
		}

		if (klass == "emu::daq::manager::Application" && i->second == STATE_UNKNOWN) {
			LOG4CPLUS_WARN(app_->logger_, "State of emu::daq::manager::Application will be unknown.");
			stringstream ss14;
			ss14 << "State of emu::daq::manager::Application will be unknown.";
			XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss14.str() );
			app_->notifyQualified( "warn", eObj );
		}
	}
}

string emu::supervisor::Application::StateTable::getState(string klass, unsigned int instance)
{
	string state = "";

	vector<pair<xdaq::ApplicationDescriptor *, string> >::iterator i =
			table_.begin();
	for (; i != table_.end(); ++i) {
		if (klass == i->first->getClassName()
				&& instance == i->first->getInstance()) {
			state = i->second;
			break;
		}
	}

	return state;
}

bool emu::supervisor::Application::StateTable::isValidState(string expected)
{
	bool is_valid = true;

	vector<pair<xdaq::ApplicationDescriptor *, string> >::iterator i =
			table_.begin();
	for (; i != table_.end(); ++i) {
		string checked = expected;
		string klass = i->first->getClassName();

		// Ignore emu::daq::manager::Application. 
		if ( klass == "emu::daq::manager::Application" ) continue;

		if (klass == "TTCciControl" || klass == "LTCControl") {
			if (expected == "Configured") { checked = "Ready"; }
		}

		if (i->second != checked) {
			is_valid = false;
			break;
		}
	}

	return is_valid;
}

void emu::supervisor::Application::StateTable::webOutput(xgi::Output *out, string sv_state)
		throw (xgi::exception::Exception)
{
	refresh();
	*out << table() << tbody() << endl;

	// My state
	*out << tr();
	*out << td() << "Application" << "(" << "0" << ")" << td();
	*out << td().set("class", sv_state) << sv_state << td();
	*out << tr() << endl;

	// Applications
	vector<pair<xdaq::ApplicationDescriptor *, string> >::iterator i =
			table_.begin();
	for (; i != table_.end(); ++i) {
		string klass = i->first->getClassName();
		int instance = i->first->getInstance();
		string state = i->second;

		*out << tr();
		*out << td() << klass << "(" << instance << ")" << td();
		*out << td().set("class", state) << state << td();
		*out << tr() << endl;
	}

	*out << tbody() << table() << endl;
}

xoap::MessageReference emu::supervisor::Application::StateTable::createStateSOAP(
		string klass)
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	envelope.addNamespaceDeclaration("xsi", NS_XSI);

	xoap::SOAPName command = envelope.createName(
			"ParameterGet", "xdaq", XDAQ_NS_URI);
	xoap::SOAPName properties = envelope.createName(
			"properties", "xapp", "urn:xdaq-application:" + klass);
	xoap::SOAPName parameter = envelope.createName(
			"stateName", "xapp", "urn:xdaq-application:" + klass);
	xoap::SOAPName xsitype = envelope.createName("type", "xsi", NS_XSI);

	xoap::SOAPElement properties_e = envelope.getBody()
			.addBodyElement(command)
			.addChildElement(properties);
	properties_e.addAttribute(xsitype, "soapenc:Struct");

	xoap::SOAPElement parameter_e = properties_e.addChildElement(parameter);
	parameter_e.addAttribute(xsitype, "xsd:string");

	return message;
}

string emu::supervisor::Application::StateTable::extractState(xoap::MessageReference message, string klass)
{
	xoap::SOAPElement root = message->getSOAPPart()
			.getEnvelope().getBody().getChildElements(
			*(new xoap::SOAPName("ParameterGetResponse", "", "")))[0];
	xoap::SOAPElement properties = root.getChildElements(
			*(new xoap::SOAPName("properties", "", "")))[0];
	xoap::SOAPElement state = properties.getChildElements(
			*(new xoap::SOAPName("stateName", "", "")))[0];

	return state.getValue();
}

ostream& emu::supervisor::operator<<( ostream& os, emu::supervisor::Application::StateTable& st ){
  os << "Application(0) " << st.app_->fsm_.getCurrentState();
  for (vector<pair<xdaq::ApplicationDescriptor *, string> >::iterator i = st.table_.begin(); i != st.table_.end(); ++i) {
    os << ", " << i->first->getClassName() << "(" << i->first->getInstance() << ")" << i->second << endl;
  }
  return os;
}

void emu::supervisor::Application::LastLog::size(unsigned int size)
{
	size_ = size;
}

unsigned int emu::supervisor::Application::LastLog::size() const
{
	return size_;
}

void emu::supervisor::Application::LastLog::add(string message)
{
	messages_.push_back(getTime() + " " + message);

	if (messages_.size() > size_) { messages_.pop_front(); }
}

string emu::supervisor::Application::LastLog::getTime() const
{
	timeval tv;
	gettimeofday(&tv, 0);

	ostringstream line;
	char s[100];

	strftime(s, 100, "%Y-%m-%dT%H:%M:%S.", localtime(&tv.tv_sec));
	line << s;
	line << tv.tv_usec / 1000;
	strftime(s, 100, "%z", localtime(&tv.tv_sec));
	line << s;

	return line.str();
}

void emu::supervisor::Application::LastLog::webOutput(xgi::Output *out)
		throw (xgi::exception::Exception)
{
	*out << "Last " << messages_.size() << " log messages:" << br() << endl;
	*out << textarea().set("cols", "120").set("rows", "20")
			.set("readonly").set("class", "log") << endl;

	deque<string>::iterator i = messages_.begin();
	for (; i != messages_.end(); ++i) {
		*out << *i << endl;
	}

	*out << textarea() << endl;
}

xoap::MessageReference emu::supervisor::Application::getRunSummary()
  throw( xcept::Exception ){

  xoap::MessageReference reply;

  // find emu::daq::manager::Application
  xdaq::ApplicationDescriptor *daqManagerDescriptor;
  try {
    daqManagerDescriptor = getApplicationContext()->getDefaultZone()->getApplicationDescriptor("emu::daq::manager::Application", 0);
  } catch (xdaq::exception::ApplicationDescriptorNotFound e) {
    XCEPT_RETHROW(xcept::Exception, "Failed to get run summary from emu::daq::manager::Application", e);
  }

  // prepare a SOAP message
  xoap::MessageReference message = createCommandSOAP("QueryRunSummary");

  // send the message
  try{
    reply = getApplicationContext()->postSOAP(message, *appDescriptor_, *daqManagerDescriptor);

    // Check if the reply indicates a fault occurred
    xoap::SOAPBody replyBody = reply->getSOAPPart().getEnvelope().getBody();
    
    if(replyBody.hasFault()){

      stringstream oss;
      string s;
      
      oss << "Received fault reply from emu::daq::manager::Application: " << replyBody.getFault().getFaultString();
      s = oss.str();
      
      XCEPT_RAISE(xcept::Exception, s);
    }
  } 
  catch(xcept::Exception e){
    XCEPT_RETHROW(xcept::Exception, "Failed to get run summary from emu::daq::manager::Application", e);
  }

  return reply;
}

string emu::supervisor::Application::reformatTime( string time ){
  // reformat from YYMMDD_hhmmss_UTC to YYYY-MM-DD hh:mm:ss UTC
  string reformatted("");
  reformatted += "20";
  reformatted += time.substr(0,2);
  reformatted += "-";
  reformatted += time.substr(2,2);
  reformatted += "-";
  reformatted += time.substr(4,2);
  reformatted += " ";
  reformatted += time.substr(7,2);
  reformatted += ":";
  reformatted += time.substr(9,2);
  reformatted += ":";
  reformatted += time.substr(11,2);
  reformatted += " UTC";
  return reformatted;
}

vector< vector<string> > emu::supervisor::Application::getFUEventCounts()
{
  vector< vector<string> > ec;

  unsigned int totalProcessed = 0;

  std::set<xdaq::ApplicationDescriptor *> EmuFUs;
  try {
    EmuFUs = getApplicationContext()->getDefaultZone()
      ->getApplicationDescriptors("EmuFU");
  } 
  catch (...){}
  // Zone::getApplicationDescriptors doesn't throw!
  if ( EmuFUs.size() == 0 ){  
    LOG4CPLUS_WARN(logger_, 
		   "Failed to get application descriptors for EmuFUs");
    stringstream ss15;
    ss15 <<  
		   "Failed to get application descriptors for EmuFUs";
    XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss15.str() );
    this->notifyQualified( "warn", eObj );
    vector<string> svt;
    svt.push_back( "Total" );
    svt.push_back( "UNKNOWN" );
    ec.push_back( svt );
    return ec;
  }

  xoap::MessageReference message = createParameterGetSOAP(
		"EmuFU", "nbEventsProcessed", "xsd:unsignedLong");

  std::set<xdaq::ApplicationDescriptor *>::iterator fu;
  for ( fu = EmuFUs.begin(); fu!=EmuFUs.end(); ++fu ){
	string       count;
	stringstream name;
    unsigned int nProcessed = 0;
    stringstream ss;
    try
    {
	  name << "EmuFU" << setfill('0') << setw(2) << (*fu)->getInstance();
	  xoap::MessageReference reply =
			getApplicationContext()->postSOAP(message, *appDescriptor_, **fu);
	  analyzeReply(message, reply, *fu);
	  count = extractParameter(reply, "nbEventsProcessed");
      ss << count;
      ss >> nProcessed;
      totalProcessed += nProcessed;
    }
    catch(xcept::Exception e)
    {
      count = "UNKNOWN";
      LOG4CPLUS_WARN(logger_,
			"Failed to get event count of " << name.str()
			<< " : " << xcept::stdformat_exception_history(e));
      stringstream ss16;
      ss16 << 
			"Failed to get event count of " << name.str()
			<< " : " ;
      XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss16.str(), e );
      this->notifyQualified( "warn", eObj );
    }
    vector<string> sv;
    sv.push_back( name.str() );
    sv.push_back( count      );
    ec.push_back( sv );
  }
  stringstream sst;
  sst << totalProcessed;
  vector<string> svt;
  svt.push_back( "Total" );
  svt.push_back( sst.str()  );
  ec.push_back( svt );

  return ec;
}

vector< vector<string> > emu::supervisor::Application::getRUIEventCounts()
{
  vector< vector<string> > ec;

  std::set<xdaq::ApplicationDescriptor *> EmuRUIs;
  try {
    EmuRUIs = getApplicationContext()->getDefaultZone()
      ->getApplicationDescriptors("EmuRUI");
  } 
  catch (...){}
  // Zone::getApplicationDescriptors doesn't throw!
  if ( EmuRUIs.size() == 0 ) {
    LOG4CPLUS_WARN(logger_, 
		    "Failed to get application descriptors for EmuRUIs");
    stringstream ss17;
    ss17 <<  
		    "Failed to get application descriptors for EmuRUIs";
    XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss17.str() );
    this->notifyQualified( "warn", eObj );
    return ec;
  }

  std::map<string, string> m;
  m["nEventsRead"     ] = "xsd:unsignedLong";
  m["hardwareMnemonic"] = "xsd:string";
  xoap::MessageReference message = createParameterGetSOAP("EmuRUI", m);

  std::set< xdaq::ApplicationDescriptor* >::iterator rui;
  for ( rui = EmuRUIs.begin(); rui!=EmuRUIs.end(); ++rui ){
    string count;
	string mnemonic;
    stringstream name;
	try {
	  xoap::MessageReference reply =
			getApplicationContext()->postSOAP(message, *appDescriptor_, **rui);
	  analyzeReply(message, reply, *rui);
	  count = extractParameter(reply, "nEventsRead");
	  mnemonic = extractParameter(reply, "hardwareMnemonic");
	}
    catch(xcept::Exception e)
    {
      count    = "UNKNOWN";
      LOG4CPLUS_WARN(logger_,
			"Failed to get event count of "
			<< "EmuRUI" << setfill('0') << setw(2) << (*rui)->getInstance()
			<< " [" << mnemonic << "]"
		    << " : " << xcept::stdformat_exception_history(e));
      stringstream ss18;
      ss18 << 
			"Failed to get event count of "
			<< "EmuRUI" << setfill('0') << setw(2) << (*rui)->getInstance()
			<< " [" << mnemonic << "]"
		    << " : " ;
      XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss18.str(), e );
      this->notifyQualified( "warn", eObj );
    }
    vector<string> sv;
    sv.push_back( name.str() );
    sv.push_back( count      );
    ec.push_back( sv );
  }

  return ec;
}

void emu::supervisor::Application::postToELog( string subject, string body, vector<string> *attachments ){
  // Post to eLog:
  emu::supervisor::ELog *eel;
  try
    {
      eel = new emu::supervisor::ELog(curlHost_.toString(),
			      curlCommand_.toString(),
			      curlCookies_.toString(),
			      CMSUserFile_.toString(),
			      eLogUserFile_.toString(),
			      eLogURL_.toString());
    }
  catch( string e ){
    LOG4CPLUS_WARN(logger_, e);
    stringstream ss19;
    ss19 <<  e;
    XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss19.str() );
    this->notifyQualified( "warn", eObj );
    eel = 0;
  }
  if ( eel ) {
    string attachmentList;
    if ( attachments )
      for ( vector<string>::iterator attm = attachments->begin(); attm != attachments->end(); ++attm )
	attachmentList += *attm + "\n";
    LOG4CPLUS_INFO(logger_, 
		   "<![CDATA[Posting to eLog address " << eLogURL_.toString() << 
		   " as user " << eel->eLogUser() << " (" << eel->CMSUser() << ") " <<
		   ":\nSubject: " << subject << 
		   "\nBody:\n" << body <<
		   "\nAttachments:\n" << attachmentList << "]]>");
    eel->postMessage( subject, body, attachments );
  }
  delete eel;
}

void emu::supervisor::Application::bookRunNumber(){

  isBookedRunNumber_ = false;

  // Don't book debug runs:
  if ( run_type_.toString() == "Debug" ) return;
  
  // Just in case it's left over from the previuos run:
  if ( runInfo_ ) {
    delete runInfo_; 
    runInfo_ = 0;
  }

  try
    {
      runInfo_ = emu::supervisor::RunInfo::Instance( runDbBookingCommand_.toString(),
					     runDbWritingCommand_.toString(),
					     runDbUserFile_.toString(),
					     runDbAddress_.toString() );
    }
  catch( string e )
    {
      LOG4CPLUS_ERROR(logger_, e);
      stringstream ss20;
      ss20 <<  e;
      XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss20.str() );
      this->notifyQualified( "error", eObj );
    }

  if ( runInfo_ ){

    const string sequence = "CMS.CSC";
    
    LOG4CPLUS_INFO(logger_, "Booking run number with " <<
		   runDbBookingCommand_.toString() << " at " <<
		   runDbAddress_.toString()  << " for " << sequence );
    
    bool success = runInfo_->bookRunNumber( sequence );
    
    if ( success ){
      isBookedRunNumber_ = true;
      run_number_        = runInfo_->runNumber();
      runSequenceNumber_ = runInfo_->runSequenceNumber();
      LOG4CPLUS_INFO(logger_, "Booked run rumber " << run_number_.toString() <<
		     " (" << sequence << " " << runSequenceNumber_.toString() << ")");
    }
    else{
      LOG4CPLUS_ERROR(logger_,
		      "Failed to book run number: " 
		      <<  runInfo_->errorMessage()
		      << " ==> Falling back to run number " << run_number_.toString() 
		      << " specified by user." );
      stringstream ss21;
      ss21 << 
		      "Failed to book run number: " 
		      <<  runInfo_->errorMessage()
		      << " ==> Falling back to run number " << run_number_.toString() 
		      << " specified by user." ;
      XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss21.str() );
      this->notifyQualified( "error", eObj );
    }
  } // if ( runInfo_ ){

}

void emu::supervisor::Application::writeRunInfo( bool toDatabase, bool toELog ){
  // Update run info db and post to eLog as well

  // Don't write about debug runs:
  if ( run_type_.toString() == "Debug" ) return;

  // emu::daq::manager::Application's FSM is asynchronous. Wait for it.
  if ( ! waitForDAQToExecute("Halt", 10, true ) ){
    LOG4CPLUS_WARN(logger_, "Nothing written to run database as local DAQ has not stopped.");
    stringstream ss22;
    ss22 <<  "Nothing written to run database as local DAQ has not stopped.";
    XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss22.str() );
    this->notifyQualified( "warn", eObj );
    return;
  }

  // If it's not a debug run, it should normally have been booked. If not, inform the user that it somehow wasn't.
  if ( toDatabase && !isBookedRunNumber_ ){
    LOG4CPLUS_WARN(logger_, "Nothing written to run database as no run number was booked.");
    stringstream ss23;
    ss23 <<  "Nothing written to run database as no run number was booked.";
    XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss23.str() );
    this->notifyQualified( "warn", eObj );
  }

    stringstream subjectToELog;
    stringstream htmlMessageToELog;

    bool success = false;
    const string nameSpace = "CMS.CSC";
    string name, value;

    //
    // Deserialize reply to run summary query
    //

    // Start and end times
    xdata::String start_time = "UNKNOWN"; // xdata can readily be serialized into SOAP...
    xdata::String stop_time  = "UNKNOWN";
    // FU event count
    xdata::String built_events = "0";
    // RUI event counts and instances
    xdata::Vector<xdata::String> rui_counts; // xdata can readily be serialized into SOAP...
    xdata::Vector<xdata::String> rui_instances; // xdata can readily be serialized into SOAP...

    xoap::DOMParser* parser = xoap::getDOMParserFactory()->get("ParseFromSOAP");
    xdata::soap::Serializer serializer;

    try{
      xoap::MessageReference reply = getRunSummary();
      std::stringstream ss;
      reply->writeTo( ss );
      DOMDocument* doc = parser->parse( ss.str() );
      
      DOMNode* n;
      n = doc->getElementsByTagNameNS( xoap::XStr("urn:xdaq-soap:3.0"), xoap::XStr("start_time") )->item(0);
      serializer.import( &start_time, n );
      n = doc->getElementsByTagNameNS( xoap::XStr("urn:xdaq-soap:3.0"), xoap::XStr("stop_time") )->item(0);
      serializer.import( &stop_time, n );
      n = doc->getElementsByTagNameNS( xoap::XStr("urn:xdaq-soap:3.0"), xoap::XStr("built_events") )->item(0);
      serializer.import( &built_events, n );
      n = doc->getElementsByTagNameNS( xoap::XStr("urn:xdaq-soap:3.0"), xoap::XStr("rui_counts") )->item(0);
      serializer.import( &rui_counts, n );
      n = doc->getElementsByTagNameNS( xoap::XStr("urn:xdaq-soap:3.0"), xoap::XStr("rui_instances") )->item(0);
      serializer.import( &rui_instances, n );

      // We're responsible for releasing the memory allocated to DOMDocument
      doc->release();
    }
    catch (xoap::exception::Exception& e){
      LOG4CPLUS_WARN( logger_, "Failed to parse run summary: " << xcept::stdformat_exception_history(e) );
      stringstream ss24;
      ss24 <<  "Failed to parse run summary: "  ;
      XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss24.str(), e );
      this->notifyQualified( "warn", eObj );
    }
    catch( xcept::Exception e ){
      LOG4CPLUS_WARN( logger_, "Run summary unknown: " << xcept::stdformat_exception_history(e) );
      stringstream ss25;
      ss25 <<  "Run summary unknown: "  ;
      XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss25.str(), e );
      this->notifyQualified( "warn", eObj );
    }
    
    string runNumber;
    if ( run_number_.toString().size() < 2 || run_number_.toString().size() > 8 ){
      // Something fishy with this run number. Use start time instead.
      runNumber = start_time.toString();
    }
    else{
      runNumber = run_number_.toString();
    }

    subjectToELog << "Emu local run " << runNumber
		  << " (" << run_type_.toString() << ")"// << ( badRun_? " is bad" : "" );
		  << " ---Supervisor";

    //
    // run number; bad run; global run number
    //
    htmlMessageToELog << " <b>Emu local run</b><br/><br/>"; // Attention: Body must not start with html tag (elog feature...)
    htmlMessageToELog << "<table>";
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">run number</td><td>" << run_number_.toString() << "</td></tr>";

    //
    // run type
    //
    name      = "run_type";
    value     = run_type_.toString();
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">run type</td><td>" << run_type_.toString() << "</td></tr>";
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() );
	stringstream ss26;
	ss26 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss26.str() );
	this->notifyQualified( "error", eObj );
      }
    }

    //
    // start time and stop time
    //
    name      = "start_time";
    value     = start_time.toString();
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">start time</td><td>" << value << "</td></tr>";
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString()  <<
			" : " << runInfo_->errorMessage() );
	stringstream ss27;
	ss27 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString()  <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss27.str() );
	this->notifyQualified( "error", eObj );
      }
    }
    name      = "stop_time";
    value     = stop_time.toString();
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">stop time</td><td>" << value << "</td></tr>";
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString()  <<
			" : " << runInfo_->errorMessage() );
	stringstream ss28;
	ss28 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString()  <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss28.str() );
	this->notifyQualified( "error", eObj );
      }
    }

    xdaq::ApplicationDescriptor *app;

    //
    // trigger sources
    //
    std::map <string,string> namesAndTypes;
    namesAndTypes.clear();
    namesAndTypes["ClockSource"  ] = "xsd:string";
    namesAndTypes["OrbitSource"  ] = "xsd:string";
    namesAndTypes["TriggerSource"] = "xsd:string";
    namesAndTypes["BGOSource"    ] = "xsd:string";
    string ClockSource("UNKNOWN");
    string OrbitSource("UNKNOWN");
    string TriggerSource("UNKNOWN");
    string BGOSource("UNKNOWN");
    try{
      app = getApplicationContext()->getDefaultZone()->getApplicationDescriptor("TTCciControl",2);
	  xoap::MessageReference message =
			createParameterGetSOAP("TTCciControl", namesAndTypes);
	  xoap::MessageReference reply =
			getApplicationContext()->postSOAP(message, *appDescriptor_, *app);
	  analyzeReply(message, reply, app);
      ClockSource   = extractParameter(reply, "ClockSource");
      OrbitSource   = extractParameter(reply, "OrbitSource");
      TriggerSource = extractParameter(reply, "TriggerSource");
      BGOSource     = extractParameter(reply, "BGOSource");
    }
    catch(xdaq::exception::ApplicationDescriptorNotFound e) {
      LOG4CPLUS_ERROR(logger_,"Failed to get trigger sources from TTCciControl 2: " << 
		      xcept::stdformat_exception_history(e) );
      stringstream ss29;
      ss29 << "Failed to get trigger sources from TTCciControl 2: ";
      XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss29.str(), e );
      this->notifyQualified( "error", eObj );
    }
    catch(xcept::Exception e){
      LOG4CPLUS_ERROR(logger_,"Failed to get trigger sources from TTCciControl 2: " << 
		      xcept::stdformat_exception_history(e) );
      stringstream ss30;
      ss30 << "Failed to get trigger sources from TTCciControl 2: ";
      XCEPT_DECLARE_NESTED( emu::supervisor::exception::Exception, eObj, ss30.str(), e );
      this->notifyQualified( "error", eObj );
    }
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">TTCci</td>";
    htmlMessageToELog << "<td><table>";
    htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "clock source"   << "</td><td align=\"right\">" 
		      << ClockSource   << "</td></tr>";
    htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "orbit source"   << "</td><td align=\"right\">" 
		      << OrbitSource   << "</td></tr>";
    htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "trigger source" << "</td><td align=\"right\">" 
		      << TriggerSource << "</td></tr>";
    htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << "BGO source"     << "</td><td align=\"right\">" 
		      << BGOSource     << "</td></tr>";
    htmlMessageToELog << "</table></td></tr>";
    name  = "clock_source";
    value = ClockSource;
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() );
	stringstream ss31;
	ss31 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss31.str() );
	this->notifyQualified( "error", eObj );
      }
    }
    name  = "orbit_source";
    value = OrbitSource;
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
				     nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() );
	stringstream ss32;
	ss32 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss32.str() );
	this->notifyQualified( "error", eObj );
      }
    }
    name  = "trigger_source";
    value = TriggerSource;
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() );
	stringstream ss33;
	ss33 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() ;
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss33.str() );
	this->notifyQualified( "error", eObj );
      }
    }
    name  = "BGO_source";
    value = BGOSource;
    if ( toDatabase && isBookedRunNumber_ ){
      success = runInfo_->writeRunInfo( name, value, nameSpace );
      if ( success ){
	LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
		       nameSpace << ":" << name << " = " << value );
      }
      else{
	LOG4CPLUS_ERROR(logger_,
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() );
	stringstream ss34;
	ss34 << 
			"Failed to write " << nameSpace << ":" << name << 
			" to run database " << runDbAddress_.toString() <<
			" : " << runInfo_->errorMessage() ;
	XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss34.str() );
	this->notifyQualified( "error", eObj );
      }
    }

    //
    // EmuFU event count
    //
      name  = "built_events";
      value = built_events.toString();
      htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">events built</td><td>" << value << "</td></tr>";
      if ( toDatabase && isBookedRunNumber_ ){
	success = runInfo_->writeRunInfo( name, value, nameSpace );
	if ( success ){
	  LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
			 nameSpace << ":" << name << " = " << value );
	}
	else{
	  LOG4CPLUS_ERROR(logger_,
			  "Failed to write " << nameSpace << ":" << name << 
			  " to run database " << runDbAddress_.toString() <<
			  " : " << runInfo_->errorMessage() );
	  stringstream ss35;
	  ss35 << 
			  "Failed to write " << nameSpace << ":" << name << 
			  " to run database " << runDbAddress_.toString() <<
			  " : " << runInfo_->errorMessage() ;
	  XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss35.str() );
	  this->notifyQualified( "error", eObj );
	}
      }

    //
    // EmuRUI event counts
    //
    htmlMessageToELog << "<tr><td bgcolor=\"#dddddd\">events read</td><td><table>";
    for ( unsigned int i = 0; i < rui_counts.elements(); ++i ){
      name  = "EmuRUI" + (dynamic_cast<xdata::String*>(rui_instances.elementAt(i)))->toString();
      value = (dynamic_cast<xdata::String*>(rui_counts.elementAt(i)))->toString();
      htmlMessageToELog << "<tr><td bgcolor=\"#eeeeee\">" << name << "</td><td align=\"right\">" << value << "</td></tr>";
      if ( toDatabase && isBookedRunNumber_ ){
	success = runInfo_->writeRunInfo( name, value, nameSpace );
	if ( success ){
	  LOG4CPLUS_INFO(logger_, "Wrote to run database: " << 
				       nameSpace << ":" << name << " = " << value );
	}
	else{
	  LOG4CPLUS_ERROR(logger_,
			  "Failed to write " << nameSpace << ":" << name << 
			  " to run database " << runDbAddress_.toString() <<
			  " : " << runInfo_->errorMessage() );
	  stringstream ss36;
	  ss36 << 
			  "Failed to write " << nameSpace << ":" << name << 
			  " to run database " << runDbAddress_.toString() <<
			  " : " << runInfo_->errorMessage() ;
	  XCEPT_DECLARE( emu::supervisor::exception::Exception, eObj, ss36.str() );
	  this->notifyQualified( "error", eObj );
	}
      }
    }
    htmlMessageToELog << "</table>";

    htmlMessageToELog << "</td></tr></table>";


    if ( toELog ){
      vector<string> attachments;
      for ( unsigned int i=0; i<peripheralCrateConfigFiles_.elements(); ++i ){
	xdata::String* f = dynamic_cast<xdata::String*>(peripheralCrateConfigFiles_.elementAt(i));
	attachments.push_back( f->toString() );
      }
      postToELog( subjectToELog.str(), htmlMessageToELog.str(), &attachments );

      // Just in case submission to e-log failed...
      cout << 
	"\n========================================================================\n" <<
	"If automatic posting to eLog address " << eLogURL_.toString() << 
	" failed, post this manually:\nSubject: " << subjectToELog.str() << 
	"\nBody:\n" << htmlMessageToELog.str() <<
	"\n========================================================================\n";
    }

  // Parser must be explicitly removed, or else it stays in the memory
  xoap::getDOMParserFactory()->destroy("ParseFromSOAP");
}

// End of file
// vim: set sw=4 ts=4:
// End of file
// vim: set sw=4 ts=4:
