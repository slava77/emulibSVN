/*****************************************************************************\
* $Id: TStoreRequest.cc,v 1.2 2010/05/13 09:44:10 banicz Exp $
\*****************************************************************************/
#include "emu/database/TStoreRequest.h"

#include "tstore/client/AttachmentUtils.h"
#include "tstore/client/Client.h"

#include "xoap/MessageFactory.h"
#include "xoap/Method.h"
#include "xoap/domutils.h"
#include "xoap/SOAPBody.h"



emu::database::TStoreRequest::TStoreRequest(const std::string &commandName, const std::string &viewClass): 
viewClass_(viewClass),
commandName_(commandName)
{
}



void emu::database::TStoreRequest::addTStoreParameter(const std::string &parameterName, const std::string &parameterValue)
{
	generalParameters_[parameterName] = parameterValue;
}



void emu::database::TStoreRequest::addViewSpecificParameter(const std::string &parameterName, const std::string &parameterValue)
throw (xcept::Exception)
{
	if (viewClass_.empty()) {
		XCEPT_RAISE(xcept::Exception,"Could not add view specific parameter to message. No view class was specified.");
	}
	viewSpecificParameters_[parameterName] = parameterValue;
}



void emu::database::TStoreRequest::addParametersWithNamespace(xoap::SOAPElement &element, xoap::SOAPEnvelope &envelope, const std::map<const std::string,std::string> &parameters, const std::string &namespaceURI, const std::string &namespacePrefix)
{
	if (!parameters.empty()) element.addNamespaceDeclaration(namespacePrefix, namespaceURI); 

	for (std::map<const std::string,std::string>::const_iterator parameter = parameters.begin(); parameter != parameters.end(); parameter++) {
		//(*parameter).first is the attribute name, (*parameter).second is the value
		xoap::SOAPName property = envelope.createName((*parameter).first, namespacePrefix, namespaceURI);
		element.addAttribute(property, (*parameter).second); 
	}
}



xoap::MessageReference emu::database::TStoreRequest::toSOAP()
{
	xoap::MessageReference message = xoap::createMessage();
	xoap::SOAPEnvelope envelope = message->getSOAPPart().getEnvelope();
	
	//add a <commandName/> element in the TStore SOAP namespace, e.g. <tstoresoap:connect/>
	xoap::SOAPName msgName = envelope.createName(commandName_, "tstoresoap", TSTORE_NS_URI);
	xoap::SOAPElement element = envelope.getBody().addBodyElement(msgName);
	
	//loop through the general parameters, and add them as attributes to this element
	//using the namespace TSTORE_NS_URI
	//it doesn't matter what we use for the prefix as long as the URI is correct
	addParametersWithNamespace(element, envelope, generalParameters_, TSTORE_NS_URI, "tstoresoap");
	
	//loop through the view-specific parameters, and add them as attributes to this element
	//using the view class as the namespace
	//it doesn't matter what we use for the prefix as long as the URI is correct
	addParametersWithNamespace(element, envelope, viewSpecificParameters_, viewClass_, "viewspecific");

	return message;
}

