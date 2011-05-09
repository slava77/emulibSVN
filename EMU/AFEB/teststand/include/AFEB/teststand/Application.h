#ifndef __AFEB_teststand_Application_h__
#define __AFEB_teststand_Application_h__

#include "xdaq/ApplicationGroup.h"
#include "xdaq/WebApplication.h"
#include "xdata/Boolean.h"
#include "xdata/InfoSpace.h"
#include "xdata/String.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/Vector.h"

#include <set>
#include <map>
#include <vector>

//---------------------
#include "xoap/MessageReference.h"

using namespace std;

namespace AFEB { namespace teststand {

/// \class AFEB::teststand::Application
/// \brief Dispatch XDAQ executives on the AFEB farm via JobControl.

class Application : public xdaq::WebApplication
{

public:

  /// define factory method for the instantion of AFEBTeststand applications
  XDAQ_INSTANTIATOR();
  
  /// constructor
  Application(xdaq::ApplicationStub *s)
    throw (xdaq::exception::Exception);

  /// the logger of this application
  Logger logger_;

private:
  ////////////////////////////////////////////////////////
  // Beginning of exported parameters for configuration //
  ////////////////////////////////////////////////////////

  ///
  /// \defgroup Application_config_param Application parameters exported for configuration
  ///
/// @{

  ///
  /// 
  ///
  xdata::String HTML_ROOT_;

  ///
  /// 
  ///
  xdata::String configurationDir_;

  ///
  /// 
  ///
  xdata::String configFileNameFilter_;

/// @}

  ////////////////////////////////////////////////////////
  // End of exported parameters for configuration       //
  ////////////////////////////////////////////////////////

  /// Generates logger name.

  ///
  /// @return logger name
  ///  
  string generateLoggerName();

  /// Binds web address to a method.
  void bindWebInterface();

  /// Exports xdata parameters.
  void exportParams();


  /// Creates a display web page.

  ///
  /// @param in cgi input
  /// @param out cgi output
  ///
  void defaultWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  void loadConfigurationTemplate();

  void initializeParameters();

  ///
  /// @param in cgi input
  /// @param out cgi output
  ///
  void configEditorWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  string setProcessingInstruction( const string XML, const string xslURI )
    throw( xcept::Exception );

  string applicationURLPath_; ///< the path of the URL of this application
  static const string applicationNamespace_; ///< the namespace of this application
  string processingInstructionSetter_; ///< XSLT to set the processing instruction for XML
  string configurationXML_; ///< XML of the configuration
  string xmlWebPageSkeleton_; ///< skeleton of the XML web page

};

}}

#endif
