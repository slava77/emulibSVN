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


#include <unistd.h> // for sleep()
#include <stdlib.h> // for getenv()

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
//   xgi::bind(this, &AFEB::teststand::Application::xslWebPage,        "XSL"        );
  xgi::bind(this, &AFEB::teststand::Application::defaultWebPage,    "Default"    );
  xgi::bind(this, &AFEB::teststand::Application::editorWebPage,     "Editor"     );
  xgi::bind(this, &AFEB::teststand::Application::displayWebPage,    "Display"    );
}

void AFEB::teststand::Application::exportParams(){

  xdata::InfoSpace *s = getApplicationInfoSpace();;

  configurationDir_  = "/tmp";

  s->fireItemAvailable( "configurationDir", &configurationDir_ );

}


void AFEB::teststand::Application::defaultWebPage(xgi::Input *in, xgi::Output *out)
  throw (xgi::exception::Exception){

  cgicc::HTTPResponseHeader &header = out->getHTTPResponseHeader();
  
  header.getStatusCode(303);
  header.getReasonPhrase("See Other");
  header.addHeader("Location", "/AFEB/teststand/html/Default.html");
  
  return;
}


/**
 * Provides the factory method for the instantiation of RUBuilderTester
 * applications.
 */
XDAQ_INSTANTIATOR_IMPL(AFEB::teststand::Application)
