#include "AFEB/teststand/utils/DOM.h"

#include <exception>
#include <sstream>

#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/framework/LocalFileInputSource.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"

#include "xalanc/PlatformSupport/XSLException.hpp"
#include "xalanc/XPath/XPathEvaluator.hpp"
#include "xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp"
#include "xalanc/XercesParserLiaison/XercesDOMSupport.hpp"
#include "xalanc/XercesParserLiaison/XercesDOMSupport.hpp"
#include "xalanc/XalanTransformer/XercesDOMWrapperParsedSource.hpp"
#include "xalanc/XercesParserLiaison/XercesParserLiaison.hpp"
#include "xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp"
#include "xalanc/XSLT/XSLTInputSource.hpp"

#include "xoap/domutils.h" // for XMLCh2String

using namespace std;

XERCES_CPP_NAMESPACE_USE

string AFEB::teststand::utils::serializeDOM( DOMNode* node ){

  string result;

  XMLCh tempStr[100];
  XMLString::transcode("LS", tempStr, 99);
  DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
  DOMWriter* theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();

  // optionally you can set some features on this serializer
  if (theSerializer->canSetFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true))
    theSerializer->setFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true);
  if (theSerializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
    theSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

  try {
    result = xoap::XMLCh2String( theSerializer->writeToString( *node ) );
  }
  catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "Failed to serialize DOM: ", e );
  }
  catch (const XMLException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    stringstream ss; ss << "Failed to serialize DOM: " << message;
    XMLString::release(&message);
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch (const DOMException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    stringstream ss; ss << "Failed to serialize DOM: " << message;
    XMLString::release(&message);
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( std::exception& e ){
    stringstream ss; ss << "Failed to serialize DOM: " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch (...) {
    XCEPT_RAISE( xcept::Exception, "Failed to serialize DOM: Unexpected exception." );
  }

  theSerializer->release();
  return result;

}

void AFEB::teststand::utils::setNodeValue( DOMNode* node, const string& value ){
  if ( node == NULL ) return;

  XMLCh* newValue = XMLString::transcode( value.c_str() );

  if ( node->getNodeType() ==  DOMNode::ELEMENT_NODE ){
    node->setTextContent( newValue );
  }
  else if ( node->getNodeType() ==  DOMNode::ATTRIBUTE_NODE ){
    node->setNodeValue( newValue );
  }

  XMLString::release( &newValue );
}

string AFEB::teststand::utils::appendToSelectedNode( const string XML, const string xPathToNode, const string xmlFragment )
  throw( xcept::Exception ){

  // Based on the idea in http://www.opensubscriber.com/message/xalan-c-users@xml.apache.org/2655850.html

  string modifiedXML;

  XALAN_USING_XALAN(XalanDOMString)
  XALAN_USING_XALAN(XalanDocument)
  XALAN_USING_XALAN(XalanDocumentPrefixResolver)
  XALAN_USING_XALAN(XalanNode)
  XALAN_USING_XALAN(XercesDOMSupport)
  XALAN_USING_XALAN(XercesDOMWrapperParsedSource)
  XALAN_USING_XALAN(XercesDocumentWrapper)
  XALAN_USING_XALAN(XercesParserLiaison)
  XALAN_USING_XALAN(XalanElement)
  XALAN_USING_XERCES(DOMNode)

  XALAN_USING_XALAN(XalanDOMException)
  XALAN_USING_XALAN(XSLException)

  XALAN_USING_XERCES(XMLPlatformUtils)
  XALAN_USING_XALAN(XPathEvaluator)

  try{
    // Namespaces won't work if these are not initialized:
    XMLPlatformUtils::Initialize();
    XPathEvaluator::initialize();
    
    XercesDOMSupport theDOMSupport;
    XercesParserLiaison theLiaison(theDOMSupport);
    theLiaison.setDoNamespaces(true); // although it seems to be already set...
    theLiaison.setBuildWrapperNodes(true);
    theLiaison.setBuildMaps(true);
    
    // Create an input source that represents a local file...
    // const XalanDOMString    theFileName(XML.c_str());
    // const LocalFileInputSource      theInputSource(theFileName.c_str());
    const char* const id = "dummy";
    MemBufInputSource theInputSource( (const XMLByte*) XML.c_str(), (unsigned int) XML.size(), id );
    XalanDocument* xalan_document = theLiaison.parseXMLStream( theInputSource );
    
    XercesDocumentWrapper* docWrapper = theLiaison.mapDocumentToWrapper(xalan_document);
    
    XalanDocumentPrefixResolver thePrefixResolver( docWrapper );
    
    XPathEvaluator theEvaluator;
    
    XalanNode* xalan_node = theEvaluator.selectSingleNode( theDOMSupport,
							   xalan_document,
							   XalanDOMString(xPathToNode.c_str()).c_str(),
							   thePrefixResolver ); // does not work with namespaces...
    
    
    if ( xalan_node ){
      
      // XalanDOMString nodeName = xalan_node->getNodeName();
      // std::cout << "Found node " << nodeName << std::endl;
      
      DOMNode* node = const_cast<DOMNode*>( docWrapper->mapNode( xalan_node ) );
      if ( node ){
	cout << "---------" << endl;
	cout << "   node->getNodeName()  " << xoap::XMLCh2String( node->getNodeName() )  << endl;
	cout << "   node->getNodeValue() " << xoap::XMLCh2String( node->getNodeValue() ) << endl;
	cout << "   node->getNodeType()  " << node->getNodeType()   << endl;
	
	DOMDocument *domDoc = const_cast<DOMDocument*>( docWrapper->getXercesDocument() );

	// Parse the XML fragment into a DOM
	MemBufInputSource xmlFragmentMBIS( (XMLByte*)xmlFragment.c_str(), xmlFragment.size(), "dummyId" );
	XercesDOMParser parser;
	parser.parse( xmlFragmentMBIS );
	DOMElement *xmlFragmentDOM = parser.getDocument()->getDocumentElement();	

	// Import and append XML fragment
 	DOMNode *importedNode = domDoc->importNode( xmlFragmentDOM , true );
	node->appendChild( importedNode );

	modifiedXML = AFEB::teststand::utils::serializeDOM( domDoc );
	cout << "modifiedXML" << endl << modifiedXML << endl;
	
      }
    }

    XMLPlatformUtils::Terminate();
    XPathEvaluator::terminate();
  }
  catch( XMLException& e ){
    stringstream ss; ss << "Failed to append to selected node: " << xoap::XMLCh2String( e.getMessage() );
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( DOMException& e ){
    stringstream ss; ss << "Failed to append to selected node: " << xoap::XMLCh2String( e.getMessage() );
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( XalanDOMException& e ){
    stringstream ss; ss << "Failed to append to selected node: exception code " << e.getExceptionCode();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( XSLException& e ){
    stringstream ss; ss << "Failed to append to selected node: " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "Failed to append to selected node: ", e );
  }
  catch( std::exception& e ){
      stringstream ss; ss << "Failed to append to selected node: " << e.what();
      XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    XCEPT_RAISE( xcept::Exception, "Failed to append to selected node: Unknown exception." );
  }
  
  return modifiedXML;
}

string AFEB::teststand::utils::setSelectedNodeValue( const string XML, const string xPathToNode, const string value )
  throw( xcept::Exception ){

  // Based on the idea in http://www.opensubscriber.com/message/xalan-c-users@xml.apache.org/2655850.html

  string modifiedXML;

  XALAN_USING_XALAN(XalanDOMString)
  XALAN_USING_XALAN(XalanDocument)
  XALAN_USING_XALAN(XalanDocumentPrefixResolver)
  XALAN_USING_XALAN(XalanNode)
  XALAN_USING_XALAN(XercesDOMSupport)
  XALAN_USING_XALAN(XercesDOMWrapperParsedSource)
  XALAN_USING_XALAN(XercesDocumentWrapper)
  XALAN_USING_XALAN(XercesParserLiaison)
  XALAN_USING_XALAN(XalanElement)
  XALAN_USING_XERCES(DOMNode)

  XALAN_USING_XALAN(XalanDOMException)
  XALAN_USING_XALAN(XSLException)

  XALAN_USING_XERCES(XMLPlatformUtils)
  XALAN_USING_XALAN(XPathEvaluator)

  try{
    // Namespaces won't work if these are not initialized:
    XMLPlatformUtils::Initialize();
    XPathEvaluator::initialize();
    
    XercesDOMSupport theDOMSupport;
    XercesParserLiaison theLiaison(theDOMSupport);
    theLiaison.setDoNamespaces(true); // although it seems to be already set...
    theLiaison.setBuildWrapperNodes(true);
    theLiaison.setBuildMaps(true);
    
    // Create an input source that represents a local file...
    // const XalanDOMString    theFileName(XML.c_str());
    // const LocalFileInputSource      theInputSource(theFileName.c_str());
    const char* const id = "dummy";
    MemBufInputSource theInputSource( (const XMLByte*) XML.c_str(), (unsigned int) XML.size(), id );
    XalanDocument* xalan_document = theLiaison.parseXMLStream( theInputSource );
    
    XercesDocumentWrapper* docWrapper = theLiaison.mapDocumentToWrapper(xalan_document);
    
    XalanDocumentPrefixResolver thePrefixResolver( docWrapper );
    
    XPathEvaluator theEvaluator;
    
    XalanNode* xalan_node = theEvaluator.selectSingleNode( theDOMSupport,
							   xalan_document,
							   XalanDOMString(xPathToNode.c_str()).c_str(),
							   thePrefixResolver ); // does not work with namespaces...
    
    
    if ( xalan_node ){
      
      // XalanDOMString nodeName = xalan_node->getNodeName();
      // std::cout << "Found node " << nodeName << std::endl;
      
      DOMNode* node = const_cast<DOMNode*>( docWrapper->mapNode( xalan_node ) );
      if ( node ){
	cout << "---------" << endl;
	cout << "   node->getNodeName()  " << xoap::XMLCh2String( node->getNodeName() )  << endl;
	cout << "   node->getNodeValue() " << xoap::XMLCh2String( node->getNodeValue() ) << endl;
	cout << "   node->getNodeType()  " << node->getNodeType()   << endl;
	
	AFEB::teststand::utils::setNodeValue( node, value );
	
	cout << "   node->getNodeValue() " << xoap::XMLCh2String( node->getNodeValue() ) << endl;
	
	DOMDocument *domDoc = const_cast<DOMDocument*>( docWrapper->getXercesDocument() );
	modifiedXML = AFEB::teststand::utils::serializeDOM( domDoc );
	cout << "modifiedXML" << endl << modifiedXML << endl;
	
      }
    }

    XMLPlatformUtils::Terminate();
    XPathEvaluator::terminate();
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
    stringstream ss; ss << "Failed to set node value: " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "Failed to set node value: ", e );
  }
  catch( std::exception& e ){
      stringstream ss; ss << "Failed to set node value: " << e.what();
      XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    XCEPT_RAISE( xcept::Exception, "Failed to set node value: Unknown exception." );
  }
  
  return modifiedXML;
}

string AFEB::teststand::utils::setSelectedNodesValues( const string XML, const map<string,string>& values )
  throw( xcept::Exception ){

  // Based on the idea in http://www.opensubscriber.com/message/xalan-c-users@xml.apache.org/2655850.html

  string modifiedXML;

  XALAN_USING_XALAN(XalanDOMString)
  XALAN_USING_XALAN(XalanDocument)
  XALAN_USING_XALAN(XalanDocumentPrefixResolver)
  XALAN_USING_XALAN(XalanNode)
  XALAN_USING_XALAN(XercesDOMSupport)
  XALAN_USING_XALAN(XercesDOMWrapperParsedSource)
  XALAN_USING_XALAN(XercesDocumentWrapper)
  XALAN_USING_XALAN(XercesParserLiaison)
  XALAN_USING_XALAN(XalanElement)
  XALAN_USING_XERCES(DOMNode)

  XALAN_USING_XALAN(XalanDOMException)
  XALAN_USING_XALAN(XSLException)

  XALAN_USING_XERCES(XMLPlatformUtils)
  XALAN_USING_XALAN(XPathEvaluator)

  try{
    // Namespaces won't work if these are not initialized:
    XMLPlatformUtils::Initialize();
    XPathEvaluator::initialize();
    
    XercesDOMSupport theDOMSupport;
    XercesParserLiaison theLiaison(theDOMSupport);
    theLiaison.setDoNamespaces(true); // although it seems to be already set...
    theLiaison.setBuildWrapperNodes(true);
    theLiaison.setBuildMaps(true);
    
    // Create an input source that represents a local file...
    // const XalanDOMString    theFileName(XML.c_str());
    // const LocalFileInputSource      theInputSource(theFileName.c_str());
    const char* const id = "dummy";
    MemBufInputSource theInputSource( (const XMLByte*) XML.c_str(), (unsigned int) XML.size(), id );
    XalanDocument* xalan_document = theLiaison.parseXMLStream( theInputSource );
    
    XercesDocumentWrapper* docWrapper = theLiaison.mapDocumentToWrapper(xalan_document);
    
    XalanDocumentPrefixResolver thePrefixResolver( docWrapper );
    
    XPathEvaluator theEvaluator;
    
    map<string,string>::const_iterator v;
    for ( v = values.begin(); v != values.end(); ++v ){
      XalanNode* xalan_node = theEvaluator.selectSingleNode( theDOMSupport,
							     xalan_document,
							     XalanDOMString(v->first.c_str()).c_str(),
							     thePrefixResolver ); // does not work with namespaces...
      
      if ( xalan_node ){
	
	// XalanDOMString nodeName = xalan_node->getNodeName();
	// std::cout << "Found node " << nodeName << std::endl;
	
	DOMNode* node = const_cast<DOMNode*>( docWrapper->mapNode( xalan_node ) );
	if ( node ){
	  // cout << "---------" << endl;
	  // cout << "   node->getNodeName()  " << xoap::XMLCh2String( node->getNodeName() )  << endl;
	  // cout << "   node->getNodeValue() " << xoap::XMLCh2String( node->getNodeValue() ) << endl;
	  // cout << "   node->getNodeType()  " << node->getNodeType()   << endl;
	  
	  AFEB::teststand::utils::setNodeValue( node, v->second );
	  
	  // cout << "   node->getNodeValue() " << xoap::XMLCh2String( node->getNodeValue() ) << endl;
	}
      }

    } // for ( v = values.begin(); v != values.end(); ++v )

    DOMDocument *domDoc = const_cast<DOMDocument*>( docWrapper->getXercesDocument() );
    modifiedXML = AFEB::teststand::utils::serializeDOM( domDoc );
    cout << "modifiedXML" << endl << modifiedXML << endl;

    XMLPlatformUtils::Terminate();
    XPathEvaluator::terminate();

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
    stringstream ss; ss << "Failed to set node value: " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "Failed to set node value: ", e );
  }
  catch( std::exception& e ){
    stringstream ss; ss << "Failed to set node value: " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    XCEPT_RAISE( xcept::Exception, "Failed to set node value: Unknown exception." );
  }
  
  return modifiedXML;
}
