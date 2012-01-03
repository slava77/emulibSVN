#include "AFEB/teststand/Application.h"
#include "AFEB/teststand/version.h"
#include "AFEB/teststand/utils/DOM.h"
#include "AFEB/teststand/utils/Cgi.h"
#include "AFEB/teststand/utils/Xalan.h"
#include "AFEB/teststand/utils/System.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/utils/Xcept.h"

#include "xcept/tools.h"
#include "xoap/domutils.h" // for XMLCh2String
#include "toolbox/task/WorkLoopFactory.h"
#include "toolbox/net/URL.h"
#include "toolbox/fsm/FailedEvent.h"

#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/framework/LocalFileInputSource.hpp"

#include "xalanc/PlatformSupport/XSLException.hpp"
#include "xalanc/XPath/XPathEvaluator.hpp"
#include "xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp"
#include "xalanc/XercesParserLiaison/XercesDOMSupport.hpp"
#include "xalanc/XercesParserLiaison/XercesDOMSupport.hpp"
#include "xalanc/XalanTransformer/XercesDOMWrapperParsedSource.hpp"
#include "xalanc/XercesParserLiaison/XercesParserLiaison.hpp"
#include "xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp"
#include "xalanc/XSLT/XSLTInputSource.hpp"

#include <iterator>

#include "AFEB/teststand/crate/Crate.h"
#include "AFEB/teststand/crate/Jorway73A.h"
#include "AFEB/teststand/crate/LE32.h"
#include "AFEB/teststand/crate/LeCroy3377.h"


AFEB::teststand::Application::Application(xdaq::ApplicationStub *s)
  throw (xdaq::exception::Exception) :
  xdaq::WebApplication(s),
  bsem_( toolbox::BSem::EMPTY ), // locked
  logger_(Logger::getInstance(generateLoggerName())),
  mode_( AFEB::teststand::Application::measurement )
{
  createFSM();
  bindWebInterface();
  exportParams();

  measurementSignature_  = toolbox::task::bind( this, &AFEB::teststand::Application::measurementInWorkLoop, "measurementInWorkLoop" );
  calibrationSignature_  = toolbox::task::bind( this, &AFEB::teststand::Application::calibrationInWorkLoop, "calibrationInWorkLoop" );

  vector<string> lines = AFEB::teststand::utils::execShellCommand( string( "hostname -s" ) );
  if ( lines.size() == 1 ) host_ = lines[0];

  bsem_.give();
}

void AFEB::teststand::Application::createFSM(){
    fsm_.addState('H', "Halted",         this, &AFEB::teststand::Application::stateChanged);
    fsm_.addState('C', "Configured",     this, &AFEB::teststand::Application::stateChanged);
    fsm_.addState('E', "Enabled",        this, &AFEB::teststand::Application::stateChanged);
    fsm_.setFailedStateTransitionChanged(this, &AFEB::teststand::Application::stateChanged);
    
    fsm_.addStateTransition('H', 'C', "Configure", this, &AFEB::teststand::Application::configureAction);
    fsm_.addStateTransition('C', 'C', "Configure", this, &AFEB::teststand::Application::noAction);
    fsm_.addStateTransition('C', 'E', "Enable",    this, &AFEB::teststand::Application::enableAction);
    fsm_.addStateTransition('E', 'C', "Stop",      this, &AFEB::teststand::Application::stopAction);
    fsm_.addStateTransition('C', 'H', "Halt",      this, &AFEB::teststand::Application::haltAction);
    fsm_.addStateTransition('E', 'H', "Halt",      this, &AFEB::teststand::Application::haltAction);
    fsm_.addStateTransition('H', 'E', "Enable",    this, &AFEB::teststand::Application::noAction);    
    fsm_.addStateTransition('H', 'H', "Halt",      this, &AFEB::teststand::Application::noAction);
    fsm_.addStateTransition('E', 'E', "Enable",    this, &AFEB::teststand::Application::noAction);
    fsm_.setFailedStateTransitionAction(           this, &AFEB::teststand::Application::failAction);
    // Define "Fail" transition explicitly for moveToFailedState to work:
    fsm_.addStateTransition('H', 'F', "Fail",      this, &AFEB::teststand::Application::failAction);
    fsm_.addStateTransition('C', 'F', "Fail",      this, &AFEB::teststand::Application::failAction);
    fsm_.addStateTransition('E', 'F', "Fail",      this, &AFEB::teststand::Application::failAction);

    fsm_.setStateName('F', "Failed");

    fsm_.setInitialState( 'H' );
    fsm_.reset();
}

void AFEB::teststand::Application::stateChanged( toolbox::fsm::FiniteStateMachine &fsm ){
  LOG4CPLUS_INFO( logger_, string( "State changed to " ) + fsm_.getStateName( fsm_.getCurrentState() ) );
}

void AFEB::teststand::Application::fireEvent( const string name ){
  toolbox::Event::Reference event( new toolbox::Event( name, this ) );
  //XCEPT_RAISE( xcept::Exception, "Test exception." );
  try{
    // fsm_.fireEvent( event ) expects toolbox::fsm::exception::Exception to be thrown.
    // It catches it but won't itself rethrow it but sets the state to 'F' (Failed).
    fsm_.fireEvent( event );
  } catch ( toolbox::fsm::exception::Exception &e ){
    XCEPT_RETHROW( xcept::Exception, "State transition failed. ", e );
  }
}

void AFEB::teststand::Application::configureAction(toolbox::Event::Reference e){
  delete configuration_;
  currentMeasurementIndex_ = -1;
  try{
    resultURLDir_ = resultBaseURLDir_.toString() + "/" + AFEB::teststand::utils::getDateTime();
    resultSystemDir_ = string( getenv(HTML_ROOT_.toString().c_str()) ) + resultURLDir_;
    AFEB::teststand::utils::execShellCommand( string( "mkdir -p " ) + resultSystemDir_ );
    configuration_ = new Configuration( configurationXML_, resultSystemDir_ );
    LOG4CPLUS_DEBUG( logger_, "Crate:" << endl << *configuration_->getCrate() );
  } catch ( xcept::Exception &e ){
    XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Configuration failed. ", e );
  }

  // Remove leftover work loop, if any...
  try{
    list<toolbox::task::WorkLoop*> workloops = toolbox::task::getWorkLoopFactory()->getWorkLoops();
    LOG4CPLUS_INFO( logger_, "Found " << workloops.size() << " work loops." );
    for ( list<toolbox::task::WorkLoop*>::const_iterator wl = workloops.begin(); wl != workloops.end(); ++ wl ){
      LOG4CPLUS_INFO( logger_, "Found " << (*wl)->getType() << " work loop " << (*wl)->getName() );
    }
    // It's tricky to find a work loop by name as 
    // toolbox::task::WorkLoopFactory::getWorkLoop(const std::string & name, const std::string & type)
    // creates a work loop named '<name>/<type>' for some reason.
    // Let's just try to remove it blindly instead, and catch the exception it throws if no such work loop exists.
    toolbox::task::getWorkLoopFactory()->removeWorkLoop( workLoopName_, workLoopType_ );
    LOG4CPLUS_INFO( logger_, "Removed " << workLoopType_ << " work loop " << workLoopName_ );
  } catch( xcept::Exception &e ){
    LOG4CPLUS_WARN( logger_, "Failed to remove leftover work loop: " << xcept::stdformat_exception_history( e ) );
  }
  //...and get a new one
  try{
    workLoop_ = toolbox::task::getWorkLoopFactory()->getWorkLoop( workLoopName_, workLoopType_ );
    LOG4CPLUS_INFO( logger_, "Created " << workLoopType_ << " work loop " << workLoopName_ );
  } catch( xcept::Exception &e ){
    stringstream ss;
    ss << "Failed recreate " << workLoopType_ << " work loop " << workLoopName_;
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }

  // Schedule measurement or calibration to be executed in a separate thread
  if ( mode_ == AFEB::teststand::Application::measurement ){
    try{
      workLoop_->submit( measurementSignature_ );
      LOG4CPLUS_INFO( logger_, "Submitted measurement action to " << workLoopType_ << " work loop " << workLoopName_ );
    } catch( xcept::Exception &e ){
      XCEPT_RETHROW(toolbox::fsm::exception::Exception, "Failed to submit measurement action to work loop.", e);
    }
  } 
  else if ( mode_ == AFEB::teststand::Application::calibration ){
    try{
      workLoop_->submit( calibrationSignature_ );
      LOG4CPLUS_INFO( logger_, "Submitted calibration action to work loop." );
    } catch( xcept::Exception &e ){
      XCEPT_RETHROW(toolbox::fsm::exception::Exception, "Failed to submit calibration action to work loop.", e);
    }    
  }

}

void AFEB::teststand::Application::enableAction(toolbox::Event::Reference e){
  // Execute measurement or calibration in a separate thread:
  if ( ! workLoop_->isActive() ){
    try{
      workLoop_->activate();
      LOG4CPLUS_INFO( logger_, "Activated work loop." );
    } catch( xcept::Exception &e ){
      XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to activate work loop.", e );
    }
  }
}

bool AFEB::teststand::Application::measurementInWorkLoop(toolbox::task::WorkLoop *wl){

  bsem_.take();
  vector<Measurement*>::const_iterator m;
  cout << configuration_->getMeasurements().size() << " measurements" << endl;
  currentMeasurementIndex_ = -1;
  bsem_.give();

  for ( m = configuration_->getMeasurements().begin(); m != configuration_->getMeasurements().end(); ++m ){
    bsem_.take();
    ++currentMeasurementIndex_;
    bsem_.give();
    LOG4CPLUS_INFO( logger_, "Executing measurement " << currentMeasurementIndex_ << "/" << configuration_->getMeasurements().size() << endl << **m );
    try{
      if ( ! (*m)->execute() ) return false; // Measurement::execute returns false if aborted.
    } catch ( xcept::Exception& e ){
      stringstream ss;
      ss << "Failed to execute measurement " << currentMeasurementIndex_ << "/" << configuration_->getMeasurements().size() << endl << **m;
      XCEPT_DECLARE_NESTED( xcept::Exception, ee, ss.str(), e );
      LOG4CPLUS_FATAL( logger_, xcept::stdformat_exception_history( ee ) );
      moveToFailedState( ee );
      return false;
    }
  }

  // Create (and save) results' XML file
  createResultsXML();

  bsem_.take();
  fsm_.reset(); // To go back to initial state (Halted) without triggering haltAction.
  bsem_.give();

  return false;
}

bool AFEB::teststand::Application::calibrationInWorkLoop(toolbox::task::WorkLoop *wl){
  return configuration_->getCalibration()->execute();
}

void AFEB::teststand::Application::haltAction(toolbox::Event::Reference e){
  // abort() will cause the method not to be automatically rescheduled any more
  for ( vector<Measurement*>::const_iterator m = configuration_->getMeasurements().begin(); m != configuration_->getMeasurements().end(); ++m ) (*m)->abort();
  if ( configuration_->getCalibration() != NULL ) configuration_->getCalibration()->abort();
}

void AFEB::teststand::Application::stopAction(toolbox::Event::Reference e){
  // Just stop the task of the work loop without removing the scheduled method
  if ( workLoop_->isActive() ){
    try{
      workLoop_->cancel();
      LOG4CPLUS_INFO( logger_, "Cancelled work loop." );
    } catch( xcept::Exception &e ){
      XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to cancel work loop.", e );
    }
  }
}

void AFEB::teststand::Application::noAction(toolbox::Event::Reference e){
}

void AFEB::teststand::Application::failAction(toolbox::Event::Reference event)
//throw (toolbox::fsm::exception::Exception)
{
    try
    {
      if ( typeid(*event) == typeid(toolbox::fsm::FailedEvent) ){
        toolbox::fsm::FailedEvent &failedEvent = dynamic_cast<toolbox::fsm::FailedEvent&>(*event);
        xcept::Exception exception = failedEvent.getException();
	reasonForFailure_ = AFEB::teststand::utils::xhtmlformat_exception_history( exception );

        stringstream ss;
        ss << "Failure occurred when performing transition from "
	   << fsm_.getStateName( failedEvent.getFromState() ) 
	   << " to " 
	   << fsm_.getStateName( failedEvent.getToState() )
	   << ": " << xcept::stdformat_exception_history( exception );
        LOG4CPLUS_FATAL(logger_, ss.str() );
      }
    }catch( xcept::Exception& e ){
      stringstream ss;
      ss <<  "Caught exception while moving to Failed state: " << xcept::stdformat_exception_history(e);
      LOG4CPLUS_FATAL(logger_, ss.str() );
    }catch( std::exception& e ){
      stringstream ss;
      ss <<  "Caught exception while moving to Failed state: " << e.what();
      LOG4CPLUS_FATAL(logger_, ss.str() );
    }catch(...){
      stringstream ss;
      ss <<  "Caught an unknown exception while moving to Failed state.";
      LOG4CPLUS_FATAL(logger_, ss.str() );
    }
}

void AFEB::teststand::Application::moveToFailedState( xcept::Exception exception ){
  // Use this from inside the work loop to force the FSM to Failed state 
  bsem_.take();
  try{
    toolbox::Event::Reference evtRef( new toolbox::Event( "Fail", this ) );
    reasonForFailure_ = AFEB::teststand::utils::xhtmlformat_exception_history( exception );
    fsm_.fireEvent( evtRef );
  } 
  catch( xcept::Exception &e ){
    LOG4CPLUS_FATAL( logger_, "Failed to move to the Failed state : " << xcept::stdformat_exception_history(e) );
  }
  bsem_.give();
}


string AFEB::teststand::Application::generateLoggerName()
{
    xdaq::ApplicationDescriptor *appDescriptor = getApplicationDescriptor();
    string                      appClass       = appDescriptor->getClassName();
    unsigned long               appInstance    = appDescriptor->getInstance();
    stringstream                ss;
    string                      loggerName;

    ss << appClass << appInstance;
    loggerName = ss.str();

    return loggerName;
}

void AFEB::teststand::Application::bindWebInterface(){
  xgi::bind(this, &AFEB::teststand::Application::defaultWebPage, "Default" );
  xgi::bind(this, &AFEB::teststand::Application::controlWebPage, "control" );
  xgi::bind(this, &AFEB::teststand::Application::resultsWebPage, "results" );
}

void AFEB::teststand::Application::exportParams(){

  xdata::InfoSpace *s = getApplicationInfoSpace();;

  HTML_ROOT_            = "BUILD_HOME";
  configurationDir_     = "/tmp";
  configFileNameFilter_ = "AFEB*.xml";
  resultBaseURLDir_     = "";
  reasonForFailure_     = "";

  s->fireItemAvailable( "configurationDir"    , &configurationDir_     );
  s->fireItemAvailable( "configFileNameFilter", &configFileNameFilter_ );
  s->fireItemAvailable( "resultBaseURLDir"    , &resultBaseURLDir_     );
  s->fireItemAvailable( "reasonForFailure"    , &reasonForFailure_     );
}

string AFEB::teststand::Application::createXMLWebPageSkeleton(){
  loadConfigurationTemplate();

  vector<pair<string,string> > configList = loadConfigurationFileList();

  stringstream ss;
  if ( applicationURLPath_.size() == 0 ){
    ss << "/urn:xdaq-application:lid=" << getApplicationDescriptor()->getLocalId();
    applicationURLPath_ = ss.str();
    ss.str("");
  }
  
  ss << "<?xml-stylesheet type=\"text/xml\" href=\"/AFEB/teststand/html/htmlRenderer_XSLT.xml\"?>" << endl 
     << "<root htmlDir=\"/AFEB/teststand/html/\" softwareVersion=\"" <<  AFEBteststand::versions << "\">" << endl;

  // Application info
  ss << "  <a:application xmlns:a=\""    << applicationNamespace_ 
     <<               "\" a:urlPath=\""  << applicationURLPath_ 
     <<               "\" a:dateTime=\"" << AFEB::teststand::utils::getDateTime()
     <<               "\" a:state=\""    << fsm_.getStateName( fsm_.getCurrentState() )
     <<               "\" a:mode=\""     << modeNames_[ mode_ ]
     <<               "\">" << endl;

  // Message, if any
  if ( fsm_.getCurrentState() == 'F' ){
    ss << "    <a:message>" << reasonForFailure_.toString() << "</a:message>" << endl;
  }

  // Configuration files
  ss << "    <a:configuration>" << endl;
  for ( vector<pair<string,string> >::const_iterator c=configList.begin(); c!=configList.end(); ++c ){
    ss << "      <a:file a:time=\"" << c->first << "\" a:name=\"" << c->second << "\"/>" << endl;
  }
  ss << "    </a:configuration>" << endl;
  // Calibration
  if ( mode_ == AFEB::teststand::Application::calibration && configuration_->getCalibration() != NULL ){
    ss << "    <a:calibration a:thresholdLevel=\"" << configuration_->getCalibration()->getThresholdLevel()
       <<                 "\" a:pulseAmplitude=\"" << configuration_->getCalibration()->getPulseAmplitude()
       <<                 "\"/>" << endl;
  }
  ss << "  </a:application>" << endl;


  // Results
  toolbox::net::URL url( getApplicationDescriptor()->getContextDescriptor()->getURL() );
  ss << "  <a:results xmlns:a=\"" << applicationNamespace_ 
     <<           "\" a:host=\"" << host_
     <<           "\" a:systemPath=\"" << resultSystemDir_
     <<           "\" a:urlHost=\"" << url.getHost()
     <<           "\" a:urlPath=\"" << resultURLDir_ << "/"
     <<           "\" a:file=\"" << "results.xml"
     <<           "\">" << endl;
  if ( configuration_ != NULL ) ss << configuration_->resultsXML();
  ss << "  </a:results>" << endl;

  ss << "</root>";

  return ss.str();
}

string AFEB::teststand::Application::createResultsXML(){
  bsem_.take();

  string resultXML;

  try{
    stringstream ss;

    //
    // First create a file that can be opened directly in a browser.
    //

    // The XSL file should be in the same directory as the XML file.
    ss << "<?xml-stylesheet type=\"text/xml\" href=\"htmlRenderer_XSLT.xml\"?>" << endl 
       << "<root htmlDir=\"\" softwareVersion=\"" << AFEBteststand::versions << "\">" << endl;

    toolbox::net::URL url( getApplicationDescriptor()->getContextDescriptor()->getURL() );
    ss << "  <a:results xmlns:a=\"" << applicationNamespace_ 
       <<           "\" a:host=\"" << host_
       <<           "\" a:systemPath=\"" << resultSystemDir_
       <<           "\" a:urlHost=\"" << url.getHost()
       <<           "\" a:urlPath=\"" // Everything is in the current directory.
       <<           "\" a:file=\"" << "results.xml"
       <<           "\">" << endl;
    if ( configuration_ != NULL ) ss << configuration_->resultsXML();
    ss << "  </a:results>" << endl;

    // This already has an XML declaration...: ss << configurationXML_ << endl;

    ss << "</root>";

    // Append the configuration to it:
    string resultsXML = AFEB::teststand::utils::appendToSelectedNode( ss.str(), "/root", configurationXML_ );

    // Save it:
    AFEB::teststand::utils::execShellCommand( string( "mkdir -p " ) + resultSystemDir_ );
    AFEB::teststand::utils::writeFile( resultSystemDir_ + "/results.xml", resultsXML );
    copyStyleFilesToResultsDir();


    //
    // Now create it as a web page to be accessed via the web server
    //

    ss.str("");
    ss << "<?xml-stylesheet type=\"text/xml\" href=\"/AFEB/teststand/html/htmlRenderer_XSLT.xml\"?>" << endl 
       << "<root htmlDir=\"/AFEB/teststand/html/\" softwareVersion=\"" << AFEBteststand::versions << "\">" << endl;

    ss << "  <a:results xmlns:a=\"" << applicationNamespace_ 
       <<           "\" a:host=\"" << host_
       <<           "\" a:systemPath=\"" << resultSystemDir_
       <<           "\" a:urlHost=\"" << url.getHost()
       <<           "\" a:urlPath=\"" << resultURLDir_ << "/"
       <<           "\" a:file=\"" << "results.xml"
       <<           "\">" << endl;
    if ( configuration_ != NULL ) ss << configuration_->resultsXML();
    ss << "  </a:results>" << endl;

    // This already has an XML declaration...: ss << configurationXML_ << endl;

    ss << "</root>";

    // Append the configuration to it:
    resultXML = AFEB::teststand::utils::appendToSelectedNode( ss.str(), "/root", configurationXML_ );
  }
  catch( xcept::Exception& e ){
    bsem_.give();
    XCEPT_RETHROW( xcept::Exception, "Failed to create results XML", e );
  }

  bsem_.give();
  return resultXML;
}

void AFEB::teststand::Application::copyStyleFilesToResultsDir(){
  stringstream command;
  if ( getenv( HTML_ROOT_.toString().c_str() ) != NULL ){
    string dir( getenv( HTML_ROOT_.toString().c_str() ) );
    command << "cp "
	    << dir << "/AFEB/teststand/html/*_XSLT.xml "
	    << dir << "/AFEB/teststand/html/*.css "
	    << resultSystemDir_;
    AFEB::teststand::utils::execShellCommand( command.str() );
  }
}


void AFEB::teststand::Application::loadConfigurationTemplate(){

  if ( bool( configurationXML_.size() ) ) return;

  string fileName;

  if ( getenv(HTML_ROOT_.toString().c_str()) != NULL ){
    try{
      fileName = string( getenv(HTML_ROOT_.toString().c_str()) ) + "/AFEB/teststand/xml/ConfigurationTemplate.xml";
      cout << "Reading in " << fileName << endl;
      configurationXML_ = AFEB::teststand::utils::readFile( fileName );
    }
    catch( xcept::Exception& e ){
      LOG4CPLUS_ERROR( logger_, "Failed to load " << fileName << " : " << xcept::stdformat_exception_history(e) );
    }
  }
  else{
    LOG4CPLUS_FATAL( logger_, HTML_ROOT_.toString() + " is not defined. Exiting." );
    exit(1);
  }

  if ( configurationXML_.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, "Configuration template is empty." );
  } 

}

vector<pair<string,string> >
AFEB::teststand::Application::loadConfigurationFileList(){
  vector<pair<string,string> > configFiles; 
  try{
    string configPath 
      = configurationDir_.toString() 
      + "/"
      + configFileNameFilter_.toString();
    string command = string("ls -1 ") + configPath;
    vector<string> files = AFEB::teststand::utils::execShellCommand( command );
    //cout << command << endl;
    //cout << files << endl;
    command = string("ls -l --time-style=long-iso ") + configPath + " | awk '{ print $6\" \"$7 }'";
    vector<string> times = AFEB::teststand::utils::execShellCommand( command );
    if ( files.size() != times.size() ){
      XCEPT_RAISE( xcept::Exception, "The number of configuration files has changed?!" );
    }
    for ( size_t i=0; i<files.size(); ++i )
      configFiles.push_back( make_pair( times[i], files[i] ) );
    //cout << configFiles << endl;
  }catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "Failed to read in list of .duck files: ", e );
  }catch( std::exception& e ){
    stringstream ess; ess << "Failed to read in list of .duck files: " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }catch(...){
    XCEPT_RAISE( xcept::Exception, "Failed to read in list of .duck files: unexpected exception." );
  }
  return configFiles;
}

void AFEB::teststand::Application::defaultWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){
  bsem_.take();
  try{
    string XMLWebPageSkeleton( createXMLWebPageSkeleton() );
    *out << AFEB::teststand::utils::appendToSelectedNode( XMLWebPageSkeleton, "/root", configurationXML_ );
  } catch( xcept::Exception& e ){
    bsem_.give();
    XCEPT_RETHROW( xgi::exception::Exception, "Failed to create default web page.", e );
  } catch( std::exception& e ){
    bsem_.give();
    XCEPT_RAISE( xgi::exception::Exception, string( "Failed to create default web page: ") + e.what() );
  } catch( ... ){
    bsem_.give();
    XCEPT_RAISE( xgi::exception::Exception, "Failed to create default web page. Unknown exception." );
  }
  bsem_.give();
}

void AFEB::teststand::Application::controlWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){
  bsem_.take();
  cgicc::Cgicc cgi( in );
  std::vector<cgicc::FormEntry> fev = cgi.getElements();

  //
  // Find out what to do:
  //

  // Manipulate configuration?
  map<string,string> action;
  try{
    action = AFEB::teststand::utils::selectFromQueryString( fev, "^config$" );
    if ( action.size() == 1 ){
      // Save?
      if ( action["config"].compare( "save" ) == 0 ){
	map<string,string> values = AFEB::teststand::utils::selectFromQueryString( fev, "^/" );
	values["/c:configuration[1]/@c:dateTime"] = AFEB::teststand::utils::getDateTime();
	for ( map<string,string>::const_iterator v = values.begin(); v != values.end(); ++v ){
	  cout << v->first << "\t" << v->second << endl;
	}
	configurationXML_ = AFEB::teststand::utils::setSelectedNodesValues( configurationXML_, values );
	cout << "configurationXML" << endl << configurationXML_ << endl << flush;
	AFEB::teststand::utils::execShellCommand( string( "mkdir -p " ) + configurationDir_.toString() );
	string expandedConfigDir = AFEB::teststand::utils::performExpansions( configurationDir_.toString() );
	LOG4CPLUS_INFO( logger_, string("Expanded ") + configurationDir_.toString() + string( " to " ) +  expandedConfigDir );
	string fileToSaveConfigIn = expandedConfigDir + "/" + values["/c:configuration[1]/@c:name"] + ".xml";
	AFEB::teststand::utils::writeFile( fileToSaveConfigIn, configurationXML_ );
	LOG4CPLUS_INFO( logger_, string("Saved configuration to ") + fileToSaveConfigIn );
      }
      // Load?
      else if ( action["config"].compare( "load" ) == 0 ){
	map<string,string> v = AFEB::teststand::utils::selectFromQueryString( fev, "^file$" );
	if ( v.size() == 1 ){
	  LOG4CPLUS_INFO( logger_, string("Reading configuration from ") + v["file"] );
	  configurationXML_ = AFEB::teststand::utils::readFile( v["file"] );
	} 
      }
    }

    // Drive FSM?
    action = AFEB::teststand::utils::selectFromQueryString( fev, "^fsm$" );
    if ( action.size() == 1 ){

      if ( action["fsm"].compare( "Configure" ) == 0 ){

	if ( fsm_.getCurrentState() == 'H' ){
	  // Set mode
	  map<string,string> modes = AFEB::teststand::utils::selectFromQueryString( fev, "^mode$" );
	  if ( modes.size() == 1 ) mode_ = ( modes["mode"].compare( modeNames_[ AFEB::teststand::Application::calibration ] ) == 0 ? 
					     AFEB::teststand::Application::calibration : 
					     AFEB::teststand::Application::measurement );
	  // Set parameter values
	  map<string,string> values = AFEB::teststand::utils::selectFromQueryString( fev, "^/" );
	  for ( map<string,string>::const_iterator v = values.begin(); v != values.end(); ++v ){
	    cout << v->first << "\t" << v->second << endl;
	  }
	  configurationXML_ = AFEB::teststand::utils::setSelectedNodesValues( configurationXML_, values );
	  cout << "configurationXML" << endl << configurationXML_ << endl << flush;
	  fireEvent( "Configure" );
	}

      }
      else if ( action["fsm"].compare( "Enable" ) == 0 ){

	if ( fsm_.getCurrentState() == 'C' ){
	  if ( mode_ == AFEB::teststand::Application::calibration ){
	    map<string,string> DACValues = AFEB::teststand::utils::selectFromQueryString( fev, "^(thresholdLevel|pulseApmlitude)$" );
	    if ( DACValues.size() == 2 ) configuration_->getCalibration()->setDACValues( DACValues );
	    cout << "DACValues" << DACValues << endl;
	    cout << *configuration_->getCalibration() << endl;
	  }
	  fireEvent( "Enable" );
	}

      }
      else if ( action["fsm"].compare( "Halt" ) == 0 ){

	if ( fsm_.getCurrentState() == 'C' || fsm_.getCurrentState() == 'E' ){
	  fireEvent( "Halt" );
	}

      }
      else if ( action["fsm"].compare( "Stop" ) == 0 ){

	if ( fsm_.getCurrentState() == 'E' ){
	  fireEvent( "Stop" );
	}

      }

    }

  } catch ( xcept::Exception& e ){
    bsem_.give();
    XCEPT_RETHROW( xgi::exception::Exception, string("Failed to execute ") + action.begin()->second, e );
  }

  AFEB::teststand::utils::redirectTo( applicationURLPath_, out );

  bsem_.give();

  return;
}

void AFEB::teststand::Application::resultsWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){
  *out << createResultsXML();
}


string AFEB::teststand::Application::setProcessingInstruction( const string XML, const string xslURI )
  throw( xcept::Exception ){
  stringstream target;
  try{
    if ( XML.size() == 0 ){
      XCEPT_RAISE( xcept::Exception, "XML document is empty." );
    }
    // Load P.I. setting XSLT if it hasn't yet been loaded.
    if ( processingInstructionSetter_.size() == 0 ){
      string xsltName;
      if ( getenv(HTML_ROOT_.toString().c_str()) != NULL ){
	xsltName = string( getenv( HTML_ROOT_.toString().c_str() ) ) + "/AFEB/teststand/xml/processingInstructionSetter.xsl";
	try{
	  processingInstructionSetter_ = AFEB::teststand::utils::readFile( xsltName );
	}
	catch( xcept::Exception& e ){
	  LOG4CPLUS_ERROR( logger_, "Failed to load " << xsltName<< xcept::stdformat_exception_history(e) );
	  XCEPT_RETHROW( xcept::Exception, "Failed to load " + xsltName, e );
	}
      }
    }
    stringstream stylesheet( processingInstructionSetter_ );
    stringstream source; source << XML;
    map<string,string> params;
    params["XSLURI"] = "'" + xslURI + "'";
    //cout << "BEFORE" << endl << "Source:" << endl << source.str() << endl << "Stylesheet:" << endl << stylesheet.str() << endl;
    AFEB::teststand::utils::transformWithParams( source, stylesheet, target, params );
    //cout << "AFTER" << endl << "Source:" << endl << source.str() << endl << "Stylesheet:" << endl << stylesheet.str() << endl;
  }catch( xcept::Exception& e ){
    stringstream ess; ess << "Failed to set processing instruction.";
    XCEPT_RETHROW( xcept::Exception, ess.str(), e );
  }catch( std::exception& e ){
    stringstream ess; ess << "Failed to set processing instruction: " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }catch(...){
    stringstream ess; ess << "Failed to set processing instruction: unexpected exception.";
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }

  return target.str();
}

const string AFEB::teststand::Application::applicationNamespace_( "http://cms.cern.ch/emu/afeb/teststand/application" );
const string AFEB::teststand::Application::workLoopName_( "AFEB::teststand::Application" );
const string AFEB::teststand::Application::workLoopType_( "waiting" );
const char* const AFEB::teststand::Application::modeNames_[] = { "measurement", "calibration" };

/**
 * Provides the factory method for the instantiation of RUBuilderTester
 * applications.
 */
XDAQ_INSTANTIATOR_IMPL(AFEB::teststand::Application)
