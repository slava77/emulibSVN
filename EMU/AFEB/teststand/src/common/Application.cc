#include "AFEB/teststand/Application.h"
#include "AFEB/teststand/version.h"
#include "AFEB/teststand/utils/Cgi.h"
#include "AFEB/teststand/utils/Xalan.h"
#include "AFEB/teststand/utils/System.h"

#include "xgi/Method.h"
#include "xgi/Utils.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPBodyElement.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/MessageFactory.h"
#include "cgicc/HTMLClasses.h"

#include "toolbox/Runtime.h"
#include "toolbox/regex.h"
//#include <xercesc/util/XMLURL.hpp>
#include "xcept/tools.h"
#include "xoap/DOMParser.h"
#include "xoap/DOMParserFactory.h"
#include "xoap/domutils.h"

#include <iterator>

// headers needed for Xalan
#include <xalanc/Include/PlatformDefinitions.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
// #include <xalanc/PlatformSupport/XalanMemoryManagerDefault.hpp> // TODO: remove with old stuff

#include <xercesc/parsers/XercesDOMParser.hpp>
// #include <xercesc/sax/SAXException.hpp>

#include "xalanc/Include/XalanMemoryManagement.hpp"
#include "xalanc/PlatformSupport/XSLException.hpp"
#include "xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp"
#include "xalanc/XPath/XObject.hpp"
#include "xalanc/XPath/XPathEvaluator.hpp"
#include "xalanc/XPath/XObject.hpp"
#include "xalanc/XPath/NodeRefList.hpp"
#include "xalanc/XalanDOM/XalanNodeList.hpp"
#include "xalanc/XalanDOM/XalanElement.hpp"
#include "xalanc/XalanDOM/XalanNamedNodeMap.hpp"
#include "xalanc/XercesParserLiaison/XercesDOMSupport.hpp"
#include "xalanc/XercesParserLiaison/XercesParserLiaison.hpp"
#include "xalanc/XalanTransformer/XercesDOMWrapperParsedSource.hpp"



#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xalanc/XalanSourceTree/XalanSourceTreeDOMSupport.hpp>
#include <xalanc/XalanSourceTree/XalanSourceTreeInit.hpp>
#include <xalanc/XalanSourceTree/XalanSourceTreeParserLiaison.hpp>
#include <xalanc/PlatformSupport/XSLException.hpp>


#include <xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp>


AFEB::teststand::Application::Application(xdaq::ApplicationStub *s)
  throw (xdaq::exception::Exception) :
  xdaq::WebApplication(s),
  logger_(Logger::getInstance(generateLoggerName()))
{

  stringstream ss( "urn:xdaq-application:lid=" );
  ss << getApplicationDescriptor()->getLocalId();
  applicationURL_ = ss.str();

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


void AFEB::teststand::Application::defaultWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){

  // cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();
  
  // header.getStatusCode(303);
  // header.getReasonPhrase("See Other");
  // header.addHeader("Location", "/AFEB/teststand/html/Default.html");
  
  string fileName;
  string xsltName;
  string xmlFile;
  if ( getenv(HTML_ROOT_.toString().c_str()) != NULL ){
    try{
      fileName = string( getenv(HTML_ROOT_.toString().c_str()) ) + "/AFEB/teststand/html/testdummy.xml";
      cout << "Reading in " << fileName << endl;
      xmlFile = AFEB::teststand::utils::readFile( fileName );
    }
    catch( xcept::Exception& e ){
      LOG4CPLUS_ERROR( logger_, "Failed to load " << fileName << " : " << xcept::stdformat_exception_history(e) );
      XCEPT_RETHROW( xgi::exception::Exception, "Failed to load " + fileName, e );
    }
    try{
      xsltName = "/AFEB/teststand/html/htmlRenderer_XSLT.xml";
      //cout << setProcessingInstruction( xmlFile, xsltName ) << endl;
      *out << setProcessingInstruction( xmlFile, xsltName );
    }
    catch( xcept::Exception& e ){
      LOG4CPLUS_ERROR( logger_, "Failed to set processing instruction to " << xsltName << " in "<< fileName << " : "<< xcept::stdformat_exception_history(e) );
      XCEPT_RETHROW( xgi::exception::Exception, "Failed to set processing instruction to " + xsltName + " in " + fileName, e );
    }
  }
  else{
    LOG4CPLUS_ERROR( logger_, HTML_ROOT_.toString() + " is not defined." );
    XCEPT_RAISE( xgi::exception::Exception, HTML_ROOT_.toString() + " is not defined." );
  }

  return;
}

void AFEB::teststand::Application::configEditorWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){

  cgicc::Cgicc cgi( in );
  std::vector<cgicc::FormEntry> fev = cgi.getElements();

  map<string,string> values = AFEB::teststand::utils::selectFromQueryString( fev, "^/" );
  map<string,string>::const_iterator v;

  // string XML = AFEB::teststand::utils::readFile( string( getenv(HTML_ROOT_.toString().c_str()) ) + "/AFEB/teststand/html/testdummy.xml" );
  string XML = string( getenv(HTML_ROOT_.toString().c_str()) ) + "/AFEB/teststand/html/testdummy.xml";
  cout << "XML" << endl << XML << endl;
  for ( v = values.begin(); v != values.end(); ++v ){
    cout << v->first << "\t" << v->second << endl;
    try{
      setNodeValue( XML, v->first, "blabla" );
    }
    catch( xcept::Exception& e){
      LOG4CPLUS_ERROR( logger_, "Failed to configure: " << xcept::stdformat_exception_history(e) );
    }
    
  }

  setNodeValue( XML, "/root", "blabla" );
  setNodeValue( XML, "/root/c:configuration[1]", "blabla" );
  setNodeValue( XML, "/root/configuration[1]", "blabla" );

  AFEB::teststand::utils::redirect( in, out );
  return;
}


string AFEB::teststand::Application::setProcessingInstruction( const string XML, const string xslURI )
  throw( xcept::Exception ){
  stringstream target;
  try{
    if ( XML.size() == 0 ){
      XCEPT_RAISE( xcept::Exception, "XML document is empty." );
    }
    // Load P.I. setting XSLT if it hasn't yet beed loaded.
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
    AFEB::teststand::utils::transformWithParams( source, stylesheet, target, params );
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


string AFEB::teststand::Application::setNodeValue2( const string XML, const string xPathToNode, const string value )
  throw( xcept::Exception ){

  string modifiedXML;

  XALAN_USING_XALAN(XalanDOMException)
  XALAN_USING_XALAN(XSLException)

    try{
      XALAN_USING_XERCES(XMLPlatformUtils)

      XALAN_USING_XALAN(XPathEvaluator)


      XMLPlatformUtils::Initialize();
      XPathEvaluator::initialize();

      XALAN_USING_XERCES(LocalFileInputSource)

      XALAN_USING_XALAN(XalanDocument)
      XALAN_USING_XALAN(XalanDocumentPrefixResolver)
      XALAN_USING_XALAN(XalanDOMString)
      XALAN_USING_XALAN(XalanNode)
      XALAN_USING_XALAN(XalanSourceTreeInit)
      XALAN_USING_XALAN(XalanSourceTreeDOMSupport)
      XALAN_USING_XALAN(XalanSourceTreeParserLiaison)
      XALAN_USING_XALAN(XObjectPtr)

      // Initialize the XalanSourceTree subsystem...
      XalanSourceTreeInit             theSourceTreeInit;

      // We'll use these to parse the XML file.
      XalanSourceTreeDOMSupport               theDOMSupport;
      XalanSourceTreeParserLiaison    theLiaison(theDOMSupport);

      // Hook the two together...
      theDOMSupport.setParserLiaison(&theLiaison);

      const XalanDOMString    theFileName(XML.c_str());

      // Create an input source that represents a local file...
      const LocalFileInputSource      theInputSource(theFileName.c_str());

      // Parse the document...
      // XalanDocument* const    theDocument =
      XalanDocument* theDocument =
	theLiaison.parseXMLStream(theInputSource);

      XalanDocumentPrefixResolver             thePrefixResolver(theDocument);

      XPathEvaluator  theEvaluator;

      // // OK, let's find the context node...
      // string contextNode = "/root";
      // XalanNode* const        theContextNode =
      //                 theEvaluator.selectSingleNode(
      //                         theDOMSupport,
      //                         theDocument,
      //                         XalanDOMString(contextNode.c_str()).c_str(),
      //                         thePrefixResolver);

      // XalanNode* const node = theEvaluator.selectSingleNode( theDOMSupport,
      XalanNode* node = theEvaluator.selectSingleNode( theDOMSupport,
							     theDocument,
							     XalanDOMString(xPathToNode.c_str()).c_str(),
							     thePrefixResolver );

      cout << "xpath: " << xPathToNode << endl;
      if ( node ){
	cout << "---------" << endl;
	cout << "   node->getNodeName()  " << node->getNodeName()   << endl;
	cout << "   node->getNodeValue() " << node->getNodeValue()  << endl;
	cout << "   node->getNodeType()  " << node->getNodeType()   << endl;
	cout << "   node->hasChildNodes()  " << node->hasChildNodes() << endl;
	// if ( node->hasChildNodes() )
	//   cout << "   node->getChildNodes()->getLength()  " << node->getChildNodes()->getLength() << endl;
	if ( node->getAttributes() )
	  cout << "   node->getAttributes()  " << node->getAttributes()->getLength() << endl;
	
	XalanDOMString newValue( value.c_str() );

	node->setNodeValue( newValue ); // does not work (NO_MODIFICATION_ALLOWED_ERR)
      }

      modifiedXML = AFEB::teststand::utils::serialize( theDocument );
      cout << "modifiedXML" << endl << modifiedXML << endl;

      // if (theContextNode == 0)
      //  {
      //          cout << "Warning -- No nodes matched the location path \""
      //                   << contextNode
      //                   << "\"."
      //                   << endl;
      //  }
      //  else
      //  {
      //          // OK, let's evaluate the expression...
      //          const XObjectPtr        theResult(
      //                  theEvaluator.evaluate(
      //                                  theDOMSupport,
      //                                  theContextNode,
      //                                  XalanDOMString(xPathToNode.c_str()).c_str(),
      //                                  thePrefixResolver));

      //          assert(theResult.null() == false);

      // 		cout << "xpath: " << xPathToNode << endl;
      //          cout << "The string value of the result is:"
      //                   << endl
      //                   << theResult->str()
      //                   << endl
      //                   << endl;
      //  }

      XPathEvaluator::terminate();
      XMLPlatformUtils::Terminate();
    }
    catch( XMLException& e ){
      stringstream ss; ss << "Failed to set node value: " << xoap::XMLCh2String( e.getMessage() );
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    catch( DOMException& e ){
      stringstream ss; ss << "Failed to set node value: " << xoap::XMLCh2String( e.getMessage() );
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    catch( XalanDOMException& e ){
      stringstream ss; ss << "Failed to set node value: exception code " << e.getExceptionCode();
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    catch( XSLException& e ){
      stringstream ss; ss << "Failed to set node value: " << e.getMessage(); // TODO: check if XalanDOMString is properly serialized this way.
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    catch( xcept::Exception& e ){
      XCEPT_RETHROW( xcept::Exception, "Failed to set node value: ", e );
    }
    catch( std::exception& e ){
      stringstream ss; ss << "Failed to set node value: " << e.what();
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    // catch(...){
    //   XCEPT_RAISE( xcept::Exception, "Failed to set node value: Unknown exception." );
    // }

  return modifiedXML;
}

string AFEB::teststand::Application::setNodeValue( const string XML, const string xPathToNode, const string value )
  throw( xcept::Exception ){
  //XALAN_USING_XALAN(FormatterToXercesDOM)
    XALAN_USING_XALAN(NodeRefList)
    XALAN_USING_XALAN(XPathEvaluator)
    XALAN_USING_XALAN(XalanDOMString)
    XALAN_USING_XALAN(XalanDocument)
    XALAN_USING_XALAN(XalanDocumentPrefixResolver)
    XALAN_USING_XALAN(XalanNode)
    XALAN_USING_XALAN(XercesDOMSupport)
    XALAN_USING_XALAN(XercesDOMWrapperParsedSource)
    XALAN_USING_XALAN(XercesDocumentWrapper)
    XALAN_USING_XALAN(XercesParserLiaison)
    XALAN_USING_XERCES(MemBufInputSource)
    XALAN_USING_XALAN(XalanElement)
    XALAN_USING_XERCES(DOMNode)

  XercesDOMSupport theDOMSupport;
  XercesParserLiaison theLiaison(theDOMSupport);
  
  theLiaison.setBuildWrapperNodes(true);
  theLiaison.setBuildMaps(true);
  
  const XalanDOMString    theFileName(XML.c_str());
  // Create an input source that represents a local file...
  const LocalFileInputSource      theInputSource(theFileName.c_str());
  XalanDocument* xalan_document = theLiaison.parseXMLStream( theInputSource );
  
  XercesDocumentWrapper* docWrapper = theLiaison.mapDocumentToWrapper(xalan_document);
  
  //XalanDocumentPrefixResolver thePrefixResolver( docWrapper );
  XalanDocumentPrefixResolver thePrefixResolver( xalan_document );

  //XalanNode * contextNode = xalan_document;
  
  //XalanElement * namespace_node = xalan_document->getDocumentElement();
  
  XPathEvaluator theEvaluator;
  
  XalanNode * xalan_node = theEvaluator.selectSingleNode( theDOMSupport,
							  xalan_document,
							  XalanDOMString(xPathToNode.c_str()).c_str(),
							  thePrefixResolver ); // does not work with namespaces...

  cout << "xpath: " << xPathToNode << endl;

  if (xalan_node)
    {
      XalanDOMString nodeName = xalan_node->getNodeName();
      std::cout << "Found node " << nodeName << std::endl;
      
      const DOMNode * xerces_node = docWrapper->mapNode(xalan_node);
    }

  return string();
}

/**
 * Provides the factory method for the instantiation of RUBuilderTester
 * applications.
 */
XDAQ_INSTANTIATOR_IMPL(AFEB::teststand::Application)
