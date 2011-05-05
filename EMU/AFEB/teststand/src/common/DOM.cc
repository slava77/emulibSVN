#include "AFEB/teststand/utils/DOM.h"

#include <exception>
#include <sstream>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

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
