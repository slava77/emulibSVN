/*
 * LogReader.cc
 *
 *  Created on: Jun 30, 2013
 *      Author: Austin Schneider
 */


#include "emu/pc/XMLWrapper.h"

#include "emu/pc/XMLUtils.h"

namespace emu {
namespace pc {

XalanDocument * XMLWrapper::parseXMLDoc()
{
  dumpFileToString(filePath.c_str(), plainText);
  plainText = "<root>" + plainText + "</root>";
  theLiaison.setDoNamespaces(true); // although it seems to be already set...
  theLiaison.setBuildWrapperNodes(true);
  theLiaison.setBuildMaps(true);

  const char* const id = "dummy";
  MemBufInputSource theInputSource((const XMLByte*) plainText.c_str(), (unsigned int) plainText.size(), id);
  return theLiaison.parseXMLStream(theInputSource);
}

XMLWrapper::XMLWrapper(std::string path)
:init(),
 filePath(path),
 theDOMSupport(),
 theLiaison(theDOMSupport),
 XMLDoc(parseXMLDoc()),
 resolver(new XalanDocumentPrefixResolver(XMLDoc))
{
  plainText.clear();
}

XMLWrapper::~XMLWrapper()
{
  delete resolver;
  result.release();
}

void XMLWrapper::refresh()
{
  XMLDoc = parseXMLDoc();
  plainText.clear();
  if(resolver != NULL) delete resolver;
  resolver = new XalanDocumentPrefixResolver(XMLDoc);
}

XObjectPtr XMLWrapper::evaluate(XPathEvaluator & eval, XMLCh * xpath)
{
  return eval.evaluate(support, XMLDoc, xpath, *resolver);
}

XObjectPtr XMLWrapper::evaluate(XPathEvaluator & eval, std::string xpath_s)
{
  XMLCh * xpath(XMLString::transcode(xpath_s.c_str()));
  XObjectPtr xop = evaluate(eval, xpath);
  XMLString::release(&xpath);
  return xop;
}

XObjectPtr XMLWrapper::evaluate(XMLCh * xpath)
{
  result = evaluator.evaluate(support, XMLDoc, xpath, *resolver);
  return result;
}

XObjectPtr XMLWrapper::evaluate(std::string xpath_s)
{
  XMLCh * xpath(XMLString::transcode(xpath_s.c_str()));
  evaluate(xpath);
  XMLString::release(&xpath);
  return result;
}

}
}

