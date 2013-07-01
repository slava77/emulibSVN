/*
 * XMLUtils.cc
 *
 *  Created on: Jun 27, 2013
 *      Author: Austin Schneider
 */

#include "emu/pc/XMLUtils.h"

#include "emu/exception/Exception.h"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <xalanc/XalanTransformer/XalanTransformer.hpp>
#include <xalanc/PlatformSupport/XSLException.hpp>
#include <xalanc/PlatformSupport/XalanStdOutputStream.hpp>
#include <xalanc/PlatformSupport/XalanOutputStreamPrintWriter.hpp>
#include <xalanc/XMLSupport/FormatterToXML.hpp>
#include <xalanc/XMLSupport/FormatterTreeWalker.hpp>
#include <xalanc/XPath/XPathEvaluator.hpp>
#include <xalanc/XalanSourceTree/XalanSourceTreeDOMSupport.hpp>
#include <xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp>
#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>
#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>

#include "xoap/domutils.h" // for XMLCh2String

#include <fstream>
#include <iostream>
#include <string>
#include <cerrno>


XALAN_USING_XALAN(XalanDocument)
XALAN_USING_XALAN(XalanNode)

XalanDocument * emu::pc::parseXMLString(const std::string & XML)
{

    XALAN_USING_XALAN(XercesDOMSupport)
    XALAN_USING_XALAN(XercesParserLiaison)
    XALAN_USING_XERCES(MemBufInputSource)
    XALAN_USING_XERCES(XMLException)
    XALAN_USING_XERCES(SAXParseException)
    XALAN_USING_XALAN(XalanDOMException)
    XALAN_USING_XALAN(XSLException)

    XalanDocument* document;

    try
    {
      XALAN_USING_XERCES(XMLPlatformUtils)
      XALAN_USING_XALAN(XPathEvaluator)
      XMLPlatformUtils::Initialize();

      XercesDOMSupport theDOMSupport;
      XercesParserLiaison theLiaison(theDOMSupport);
      theLiaison.setDoNamespaces(true); // although it seems to be already set...
      theLiaison.setBuildWrapperNodes(true);
      theLiaison.setBuildMaps(true);

      const char* const id = "dummy";
      MemBufInputSource theInputSource((const XMLByte*) XML.c_str(), (unsigned int) XML.size(), id);
      document = theLiaison.parseXMLStream(theInputSource);

      // XMLPlatformUtils::Terminate() causes the program to crash unless XMLPlatformUtils::Initialize()
      // has been called more times than has XMLPlatformUtils::Terminate(). Anyway, as of Xerces-C++ 2.8.0:
      // "The termination call is currently optional, to aid those dynamically loading the parser
      // to clean up before exit, or to avoid spurious reports from leak detectors."
      // XMLPlatformUtils::Terminate();
    }
    catch (SAXParseException& e)
    {
      std::stringstream ss;
      ss << "Failed to parse the string: " << xoap::XMLCh2String(e.getMessage());
      std::cout << ss.str() << std::endl;
      XCEPT_RAISE( emu::exception::XMLException, ss.str());
    }
    catch (XMLException& e)
    {
      std::stringstream ss;
      ss << "Failed to parse the string: " << xoap::XMLCh2String(e.getMessage());
      std::cout << ss.str() << std::endl;
      XCEPT_RAISE( emu::exception::XMLException, ss.str());
    }
    catch (DOMException& e)
    {
      std::stringstream ss;
      ss << "Failed to parse the string: " << xoap::XMLCh2String(e.getMessage());
      std::cout << ss.str() << std::endl;
      XCEPT_RAISE( emu::exception::XMLException, ss.str());
    }
    catch (XalanDOMException& e)
    {
      std::stringstream ss;
      ss << "Failed to parse the string: exception code " << e.getExceptionCode();
      std::cout << ss.str() << std::endl;
      XCEPT_RAISE( emu::exception::XMLException, ss.str());
    }
    catch (XSLException& e)
    {
      XALAN_USING_XALAN(XalanDOMString)
      std::stringstream ss;
      ss << "Failed to parse the string: XSLException type: " << XalanDOMString(e.getType()) << ", message: " << e.getMessage();
      std::cout << ss.str() << std::endl;
      XCEPT_RAISE( emu::exception::XMLException, ss.str());
    }
    catch (xcept::Exception& e)
    {
      std::cout << "Rethrow" << std::endl;
      XCEPT_RETHROW( emu::exception::XMLException, "Failed to parse the string: ", e);
    }
    catch (std::exception& e)
    {
      std::stringstream ss;
      ss << "Failed to parse the string: " << e.what();
      std::cout << ss.str() << std::endl;
      XCEPT_RAISE( emu::exception::XMLException, ss.str());
    }
    catch (...)
    {
      std::cout << "Unknown" << std::endl;
      XCEPT_RAISE( emu::exception::UndefinedException, "Failed to parse the string: Unknown exception.");
    }
    return document;
}

void emu::pc::dumpFileToString(const char *filename, std::string & out)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in)
  {
    unsigned int init_size = (unsigned int)out.size();
    in.seekg(0, std::ios::end);
    out.resize((unsigned int)in.tellg() + init_size);
    in.seekg(0, std::ios::beg);
    in.read(&out[init_size], out.size());
    in.close();
  }
  else
    throw(errno);
}

std::string emu::pc::domStringToString(XalanDOMString & dom_string)
{
  std::string s;
  XalanDOMString::CharVectorType  cvec;
  dom_string.transcode(cvec);
  s.assign (cvec.begin(), cvec.end());
  return s;
}

