#include "AFEB/teststand/Application.h"
#include "AFEB/teststand/version.h"
#include "AFEB/teststand/utils/DOM.h"
#include "AFEB/teststand/utils/Cgi.h"
#include "AFEB/teststand/utils/Xalan.h"
#include "AFEB/teststand/utils/System.h"
#include "AFEB/teststand/utils/IO.h"

#include "xcept/tools.h"
#include "xoap/domutils.h" // for XMLCh2String
#include "toolbox/task/WorkLoopFactory.h"

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

#include "AFEB/teststand/Crate.h"
#include "AFEB/teststand/Jorway73A.h"
#include "AFEB/teststand/LE32.h"
#include "AFEB/teststand/LeCroy3377.h"


AFEB::teststand::Application::Application(xdaq::ApplicationStub *s)
  throw (xdaq::exception::Exception) :
  xdaq::WebApplication(s),
  logger_(Logger::getInstance(generateLoggerName()))
{
  createFSM();
  bindWebInterface();
  exportParams();
  //AFEB::teststand::utils::SCSI_t scsi = AFEB::teststand::utils::getSCSI( "Jorway", "73A" );
  AFEB::teststand::utils::SCSI_t scsi = AFEB::teststand::utils::getSCSI( "PIONEER", "BD-ROM" );
  cout << "SCSI device vendor: " << scsi.vendor 
       << " model: " << scsi.model 
       << " host: "  << scsi.host 
       << " channel: "<< scsi.channel
       << " id: " << scsi.id
       << " lun: " << scsi.lun
       << endl;

  measurementWorkLoop_ = toolbox::task::getWorkLoopFactory()->getWorkLoop( "AFEB::teststand::Application", "waiting" );
  measurementSignature_  = toolbox::task::bind( this, &AFEB::teststand::Application::measurementInWorkLoop, "measurementInWorkLoop" );

}

void AFEB::teststand::Application::createFSM(){
    fsm_.addState('H', "Halted",     this, &AFEB::teststand::Application::stateChanged);
    fsm_.addState('C', "Configured", this, &AFEB::teststand::Application::stateChanged);
    fsm_.addState('E', "Enabled",    this, &AFEB::teststand::Application::stateChanged);
    
    fsm_.addStateTransition('H', 'C', "Configure", this, &AFEB::teststand::Application::configureAction);
    fsm_.addStateTransition('C', 'C', "Configure", this, &AFEB::teststand::Application::noAction);
    fsm_.addStateTransition('C', 'E', "Enable",    this, &AFEB::teststand::Application::enableAction);
    fsm_.addStateTransition('E', 'C', "Configure", this, &AFEB::teststand::Application::noAction);
    fsm_.addStateTransition('C', 'H', "Halt",      this, &AFEB::teststand::Application::haltAction);
    fsm_.addStateTransition('E', 'H', "Halt",      this, &AFEB::teststand::Application::haltAction);
    fsm_.addStateTransition('H', 'E', "Enable",    this, &AFEB::teststand::Application::noAction);    
    fsm_.addStateTransition('H', 'H', "Halt",      this, &AFEB::teststand::Application::noAction);
    fsm_.addStateTransition('E', 'E', "Enable",    this, &AFEB::teststand::Application::noAction);

    //fsm_.setFailedStateTransitionChanged(this, &AFEB::teststand::Application::stateChanged);
    fsm_.setInitialState( 'H' );
    fsm_.reset();
}

void AFEB::teststand::Application::stateChanged( toolbox::fsm::FiniteStateMachine &fsm ){
  LOG4CPLUS_INFO( logger_, string( "State changed to " ) + fsm_.getStateName( fsm_.getCurrentState() ) );
}

void AFEB::teststand::Application::fireEvent( const string name ){
  toolbox::Event::Reference event( new toolbox::Event( name, this ) );
  fsm_.fireEvent( event );
}

void AFEB::teststand::Application::configureAction(toolbox::Event::Reference e){
  if ( configuration_ != NULL ){
    for ( vector<Measurement*>::const_iterator m = configuration_->getMeasurements().begin(); m != configuration_->getMeasurements().end(); ++m ) (*m)->abort();
  }
  delete configuration_;
  currentMeasurementIndex_ = -1;
  configuration_ = new Configuration( configurationXML_, string( getenv(HTML_ROOT_.toString().c_str()) ) + "/" + resultDir_.toString() );
  cout << configuration_->getCrate() << endl;
}

void AFEB::teststand::Application::enableAction(toolbox::Event::Reference e){ // TODO: in separate thread, with mutex
  // Execute measurement in a separate thread:
  if ( ! measurementWorkLoop_->isActive() ){
    try{
      measurementWorkLoop_->activate();
      LOG4CPLUS_INFO( logger_, "Activated measurement work loop." );
    } catch( xcept::Exception &e ){
      XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to activate measurement work loop.", e );
    }
  }
  try{
    measurementWorkLoop_->submit( measurementSignature_ );
  } catch( xcept::Exception &e ){
    XCEPT_RETHROW(toolbox::fsm::exception::Exception, "Failed to submit measurement action to work loop.", e);
  }
}

bool AFEB::teststand::Application::measurementInWorkLoop(toolbox::task::WorkLoop *wl){
  vector<Measurement*>::const_iterator m;
  cout << configuration_->getMeasurements().size() << " measurements" << endl;
  currentMeasurementIndex_ = -1;
  for ( m = configuration_->getMeasurements().begin(); m != configuration_->getMeasurements().end(); ++m ){
    ++currentMeasurementIndex_;
    cout << **m;
    if ( ! (*m)->execute() ) return false; // Measurement::execute returns false if aborted.
  }
  fsm_.reset(); // To go back to initial state (Halted) without triggering haltAction.
  return false;
}

void AFEB::teststand::Application::haltAction(toolbox::Event::Reference e){
  for ( vector<Measurement*>::const_iterator m = configuration_->getMeasurements().begin(); m != configuration_->getMeasurements().end(); ++m ) (*m)->abort();
}

void AFEB::teststand::Application::noAction(toolbox::Event::Reference e){
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
}

void AFEB::teststand::Application::exportParams(){

  xdata::InfoSpace *s = getApplicationInfoSpace();;

  HTML_ROOT_            = "BUILD_HOME";
  configurationDir_     = "/tmp";
  configFileNameFilter_ = "AFEB*.xml";
  resultDir_            = "";

  s->fireItemAvailable( "configurationDir"    , &configurationDir_     );
  s->fireItemAvailable( "configFileNameFilter", &configFileNameFilter_ );
  s->fireItemAvailable( "resultDir"           , &resultDir_            );

}

void AFEB::teststand::Application::initializeParameters(){
  loadConfigurationTemplate();

  vector<pair<string,string> > configList = loadConfigurationFileList();

  stringstream ss;
  if ( applicationURLPath_.size() == 0 ){
    ss << "/urn:xdaq-application:lid=" << getApplicationDescriptor()->getLocalId();
    applicationURLPath_ = ss.str();
    ss.str("");
  }
  
  ss << "<?xml-stylesheet type=\"text/xml\" href=\"/AFEB/teststand/html/htmlRenderer_XSLT.xml\"?>" << endl 
     << "<root>" << endl
     << "  <a:application xmlns:a=\"" << applicationNamespace_ 
     << "\" a:urlPath=\"" << applicationURLPath_ 
     << "\" a:state=\"" << fsm_.getStateName( fsm_.getCurrentState() )
     << "\" a:dateTime=\"" << AFEB::teststand::utils::getDateTime()
     << "\">"
     << "    <a:results>" << endl;
  int measurementCount = 0;
  if ( configuration_ != NULL ){
    vector<Measurement*> measurements = configuration_->getMeasurements();
    for ( vector<Measurement*>::const_iterator m = measurements.begin(); m != measurements.end(); ++m ){
      map<TestedDevice*,Results*> results = (*m)->getResults();
      for ( map<TestedDevice*,Results*>::const_iterator r = results.begin(); r != results.end(); ++r ){
	ss << "      <a:plot a:name=\"" << r->second->getFileName() 
	   << "\" a:url=\"" << resultDir_.toString() << "/" << r->second->getFileName() << ".png"
	   << "\" a:current=\"" << ( measurementCount == currentMeasurementIndex_ ? "yes" : "no" ) 
	   << "\"/>" << endl;
      }
      ++measurementCount;
    }
  }
  ss << "    </a:results>" << endl;
  ss << "    <a:configuration>" << endl;
  for ( vector<pair<string,string> >::const_iterator c=configList.begin(); c!=configList.end(); ++c ){
    ss << "      <a:file a:time=\"" << c->first << "\" a:name=\"" << c->second << "\"/>" << endl;
  }
  ss << "    </a:configuration>" << endl
     << "  </a:application>" << endl
     << "</root>";

  xmlWebPageSkeleton_ = ss.str();
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

  initializeParameters();

  *out << AFEB::teststand::utils::appendToSelectedNode( xmlWebPageSkeleton_, "/root", configurationXML_ );

}

void AFEB::teststand::Application::controlWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){

  initializeParameters();

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
	string fileToSaveConfigIn = configurationDir_.toString() + "/" + values["/c:configuration[1]/@c:name"] + ".xml";
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
	  fireEvent( "Enable" );
	}

      }
      else if ( action["fsm"].compare( "Halt" ) == 0 ){

	if ( fsm_.getCurrentState() == 'C' || fsm_.getCurrentState() == 'E' ){
	  fireEvent( "Halt" );
	}
      }

    }

  } catch (  xcept::Exception& e ){
    XCEPT_RETHROW( xgi::exception::Exception, string("Failed to execute ") + action.begin()->second, e );
  }

  AFEB::teststand::utils::redirectTo( applicationURLPath_, out );
  return;
}

// TODO: result web page (file)

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
	xsltName = string( getenv(HTML_ROOT_.toString().c_str()) ) + "/AFEB/teststand/xml/processingInstructionSetter.xsl";
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

/**
 * Provides the factory method for the instantiation of RUBuilderTester
 * applications.
 */
XDAQ_INSTANTIATOR_IMPL(AFEB::teststand::Application)
