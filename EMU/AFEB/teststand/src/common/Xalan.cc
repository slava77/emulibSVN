#include "AFEB/teststand/utils/Xalan.h"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/XMLException.hpp>

#include <xalanc/XalanTransformer/XalanTransformer.hpp>
#include <xalanc/PlatformSupport/XSLException.hpp>
#include <xalanc/PlatformSupport/XalanStdOutputStream.hpp>
#include <xalanc/PlatformSupport/XalanOutputStreamPrintWriter.hpp>
#include <xalanc/XMLSupport/FormatterToXML.hpp>
#include <xalanc/XMLSupport/FormatterTreeWalker.hpp>
#include <xalanc/XPath/XPathEvaluator.hpp>
#include <xalanc/XalanSourceTree/XalanSourceTreeDOMSupport.hpp>
#include <xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp>
#include "xalanc/XercesParserLiaison/XercesDOMSupport.hpp"
#include "xalanc/XercesParserLiaison/XercesParserLiaison.hpp"

#include "xoap/domutils.h" // for XMLCh2String

#include <sstream>
#include <exception>

using namespace std;

int
AFEB::teststand::utils::transformStreams( istream& source, istream& stylesheet, ostream& target )
  throw( xcept::Exception ){

  XALAN_USING_XALAN( XSLException );
  int result = 1;
  try{

    XALAN_USING_XALAN(XalanTransformer);    
    // Initialize Xalan.
    XalanTransformer::initialize();
    // Create a XalanTransformer.
    XalanTransformer theXalanTransformer;

    XALAN_USING_XALAN(XSLTInputSource);
    XALAN_USING_XALAN(XSLTResultTarget);
    XSLTInputSource theSource( source );
    XSLTInputSource theStylesheet( stylesheet );
    XSLTResultTarget theTarget( target );

    // Do the transform.
    result = theXalanTransformer.transform(theSource, theStylesheet, theTarget);
    
    if(result != 0){
      stringstream ss; ss << "Error: " << theXalanTransformer.getLastError();
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }

    // Terminate Xalan...
    XalanTransformer::terminate();
    // Clean up the ICU, if it's integrated...
    XalanTransformer::ICUCleanUp();
  }
  catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "XSLT transformation failed: ", e );
  }
  catch( XSLException& e ){
    XALAN_USING_XALAN(XalanDOMString)
    stringstream ss; ss << "XSLT transformation failed: XSLException type: " << XalanDOMString( e.getType() ) << ", message: " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( std::exception& e ){
    stringstream ss; ss << "XSLT transformation failed: " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    XCEPT_RAISE( xcept::Exception, "XSLT transformation failed: Unknown exception." );
  }
  return result;
}

int
AFEB::teststand::utils::transformWithParams( istream& source, istream& stylesheet, ostream& target, const map<std::string,std::string>& params )
  throw( xcept::Exception ){

  XALAN_USING_XALAN( XSLException );
  int result = 1;
  try{

    XALAN_USING_XALAN(XalanTransformer);    
    // Initialize Xalan.
    XalanTransformer::initialize();
    // Create a XalanTransformer.
    XalanTransformer theXalanTransformer;

    XALAN_USING_XALAN(XSLTInputSource);
    XALAN_USING_XALAN(XSLTResultTarget);
    XSLTInputSource theSource( source );
    XSLTInputSource theStylesheet( stylesheet );
    XSLTResultTarget theTarget( target );

    // Set the stylesheet parameters
    XALAN_USING_XALAN(XalanDOMString);
    for ( map<std::string,std::string>::const_iterator p = params.begin(); p != params.end(); ++p ){
      XalanDOMString name( p->first.c_str() );
      XalanDOMString value( p->second.c_str() );
      theXalanTransformer.setStylesheetParam( name, value );
    }
    // Do the transform.
    result = theXalanTransformer.transform(theSource, theStylesheet, theTarget);
    
    if(result != 0){
      stringstream ss; ss << "Error: " << theXalanTransformer.getLastError();
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }

    // Terminate Xalan...
    XalanTransformer::terminate();
    // Clean up the ICU, if it's integrated...
    XalanTransformer::ICUCleanUp();
  }
  catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "XSLT transformation failed: ", e );
  }
  catch( XSLException& e ){
    XALAN_USING_XALAN(XalanDOMString)
    stringstream ss; ss << "XSLT transformation failed: XSLException type: " << XalanDOMString( e.getType() ) << ", message: " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( std::exception& e ){
    stringstream ss; ss << "XSLT transformation failed: " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    XCEPT_RAISE( xcept::Exception, "XSLT transformation failed: Unknown exception." );
  }
  return result;
}


XALAN_USING_XALAN(XalanDocument)

string
AFEB::teststand::utils::serialize( const XalanNode* node )
  throw( xcept::Exception ){
  XALAN_USING_XALAN(XalanStdOutputStream);
  XALAN_USING_XALAN(XalanOutputStreamPrintWriter);
  XALAN_USING_XALAN(FormatterToXML);
  XALAN_USING_XALAN(FormatterTreeWalker);
  stringstream target_stream; // this is the output stream
  XalanStdOutputStream output_stream(target_stream);
  XalanOutputStreamPrintWriter writer(output_stream);
  FormatterToXML formatter(writer);
  FormatterTreeWalker tree_walker(formatter);
  tree_walker.traverse( node );
  return target_stream.str();
}

XALAN_USING_XALAN(XalanNode)

XalanNode*
AFEB::teststand::utils::getSingleNode( XalanDocument* document, XalanNode* contextNode, const string xPath )
  throw( xcept::Exception ){

  XALAN_USING_XALAN(XalanNode);
  XalanNode* theNode;

  XALAN_USING_XALAN(XSLException);

  try{
    XALAN_USING_XERCES(XMLPlatformUtils);
    XMLPlatformUtils::Initialize();
    XALAN_USING_XALAN(XPathEvaluator);
    XPathEvaluator::initialize();

    // We'll use these to parse the XML file.
    XALAN_USING_XALAN(XalanSourceTreeDOMSupport);
    XalanSourceTreeDOMSupport    theDOMSupport;
    XALAN_USING_XALAN(XalanDocumentPrefixResolver);
    XalanDocumentPrefixResolver  thePrefixResolver(document);
    XPathEvaluator               theEvaluator;

    XALAN_USING_XALAN(XalanDOMString);
    theNode = theEvaluator.selectSingleNode( theDOMSupport,
					     contextNode,
					     XalanDOMString( xPath.c_str() ).c_str(),
					     thePrefixResolver );
    XMLPlatformUtils::Terminate();
    XPathEvaluator::terminate();
  }
  catch( xcept::Exception& e ){
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': ";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( XSLException& e ){
    XALAN_USING_XALAN(XalanDOMString)
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': XSLException type: " << XalanDOMString( e.getType() ) << ", message: " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( std::exception& e ){
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return theNode;
}

XALAN_USING_XALAN(NodeRefList)

void
AFEB::teststand::utils::getNodes( NodeRefList& resultNodeList, XalanDocument* document, XalanNode* contextNode, const string xPath )
  throw( xcept::Exception ){

  XALAN_USING_XALAN(XSLException);

  try{
    XALAN_USING_XERCES(XMLPlatformUtils);
    XMLPlatformUtils::Initialize();
    XALAN_USING_XALAN(XPathEvaluator);
    XPathEvaluator::initialize();

    // We'll use these to parse the XML file.
    XALAN_USING_XALAN(XalanSourceTreeDOMSupport);
    XalanSourceTreeDOMSupport    theDOMSupport;
    XALAN_USING_XALAN(XalanDocumentPrefixResolver);
    XalanDocumentPrefixResolver  thePrefixResolver(document);
    XPathEvaluator               theEvaluator;

    XALAN_USING_XALAN(XalanDOMString);
    theEvaluator.selectNodeList( resultNodeList, 
				 theDOMSupport,
				 contextNode,
				 XalanDOMString( xPath.c_str() ).c_str(),
				 thePrefixResolver );
    XPathEvaluator::terminate();
    XMLPlatformUtils::Terminate();
  }
  catch( xcept::Exception& e ){
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': ";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( XSLException& e ){
    XALAN_USING_XALAN(XalanDOMString)
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': XSLException type: " << XalanDOMString( e.getType() ) << ", message: " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( std::exception& e ){
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
}

string AFEB::teststand::utils::serializeSelectedNode( const string& XML, const string xPath )
  throw( xcept::Exception ){

  string serializedNode;

  XALAN_USING_XALAN(XercesDOMSupport)
  XALAN_USING_XALAN(XercesParserLiaison)
  XALAN_USING_XERCES(MemBufInputSource)
  XALAN_USING_XERCES(XMLException)
  XALAN_USING_XALAN(XalanDOMException)
  XALAN_USING_XALAN(XSLException)

  try{
    XALAN_USING_XERCES(XMLPlatformUtils)
    XALAN_USING_XALAN(XPathEvaluator)
    XMLPlatformUtils::Initialize();

    XercesDOMSupport theDOMSupport;
    XercesParserLiaison theLiaison(theDOMSupport);
    theLiaison.setDoNamespaces(true); // although it seems to be already set...
    theLiaison.setBuildWrapperNodes(true);
    theLiaison.setBuildMaps(true);
    
    const char* const id = "dummy";
    MemBufInputSource theInputSource( (const XMLByte*) XML.c_str(), (unsigned int) XML.size(), id );
    XalanDocument* document = theLiaison.parseXMLStream( theInputSource );


    serializedNode = serialize( getSingleNode( document, document, xPath ) );

    XMLPlatformUtils::Terminate();
  }
  catch( XMLException& e ){
    stringstream ss; ss << "Failed to serialize selected node: " << xoap::XMLCh2String( e.getMessage() );
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( DOMException& e ){
    stringstream ss; ss << "Failed to serialize selected node: " << xoap::XMLCh2String( e.getMessage() );
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( XalanDOMException& e ){
    stringstream ss; ss << "Failed to serialize selected node: exception code " << e.getExceptionCode();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( XSLException& e ){
    XALAN_USING_XALAN(XalanDOMString)
    stringstream ss; ss << "Failed to serialize selected node: XSLException type: " << XalanDOMString( e.getType() ) << ", message: " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "Failed to serialize selected node: ", e );
  }
  catch( std::exception& e ){
    stringstream ss; ss << "Failed to serialize selected node: " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    XCEPT_RAISE( xcept::Exception, "Failed to serialize selected node: Unknown exception." );
  }
  
  return serializedNode;
}
