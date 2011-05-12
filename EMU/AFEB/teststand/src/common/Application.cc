#include "AFEB/teststand/Application.h"
#include "AFEB/teststand/version.h"
#include "AFEB/teststand/utils/DOM.h"
#include "AFEB/teststand/utils/Cgi.h"
#include "AFEB/teststand/utils/Xalan.h"
#include "AFEB/teststand/utils/System.h"

#include "xcept/tools.h"
#include "xoap/domutils.h" // for XMLCh2String

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


AFEB::teststand::Application::Application(xdaq::ApplicationStub *s)
  throw (xdaq::exception::Exception) :
  xdaq::WebApplication(s),
  logger_(Logger::getInstance(generateLoggerName()))
{

  bindWebInterface();
  exportParams();
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
  xgi::bind(this, &AFEB::teststand::Application::defaultWebPage,      "Default"         );
  xgi::bind(this, &AFEB::teststand::Application::configEditorWebPage, "configEditor"    );
}

void AFEB::teststand::Application::exportParams(){

  xdata::InfoSpace *s = getApplicationInfoSpace();;

  HTML_ROOT_            = "BUILD_HOME";
  configurationDir_     = "/tmp";
  configFileNameFilter_ = "AFEB*.xml";

  s->fireItemAvailable( "configurationDir"    , &configurationDir_     );
  s->fireItemAvailable( "configFileNameFilter", &configFileNameFilter_ );

}

void AFEB::teststand::Application::initializeParameters(){
  loadConfigurationTemplate();

  stringstream ss;
  if ( applicationURLPath_.size() == 0 ){
    ss << "/urn:xdaq-application:lid=" << getApplicationDescriptor()->getLocalId();
    applicationURLPath_ = ss.str();
    ss.str("");
  }
  
  ss << "<?xml-stylesheet type=\"text/xml\" href=\"/AFEB/teststand/html/htmlRenderer_XSLT.xml\"?>" << endl 
     << "<root>" << endl
     << "<a:application xmlns:a=\"" << applicationNamespace_ 
     << "\" urlPath=\"" << applicationURLPath_ 
     << "\"/>" << endl
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


void AFEB::teststand::Application::defaultWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){

  initializeParameters();

  *out << AFEB::teststand::utils::appendToSelectedNode( xmlWebPageSkeleton_, "/root", configurationXML_ );

}

void AFEB::teststand::Application::configEditorWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){

  initializeParameters();

  cgicc::Cgicc cgi( in );
  std::vector<cgicc::FormEntry> fev = cgi.getElements();

  map<string,string> values = AFEB::teststand::utils::selectFromQueryString( fev, "^/" );
  // map<string,string>::const_iterator v;
  // for ( v = values.begin(); v != values.end(); ++v ){
  //   cout << v->first << "\t" << v->second << endl;
  // }

  string XML = configurationXML_;
  cout << "XML" << endl << XML << endl << flush;

  configurationXML_ = AFEB::teststand::utils::setSelectedNodesValues( XML, values );

  cout << "configurationXML" << endl << configurationXML_ << endl << flush;

  cout << "Selected value: " << AFEB::teststand::utils::getSelectedNodeValue( configurationXML_, "/c:configuration[1]/@c:date" ) << endl;

  vector<string> selectedValues = AFEB::teststand::utils::getSelectedNodesValues( configurationXML_, "/c:configuration/c:description/child::*" );
  vector<string>::iterator v;
  cout << "Selected values:" << endl;
  for ( v = selectedValues.begin(); v != selectedValues.end(); ++v ){
    cout << *v << endl;
  }

  AFEB::teststand::utils::redirectTo( applicationURLPath_, out );
  return;
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
