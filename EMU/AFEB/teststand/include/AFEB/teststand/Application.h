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
// #include <multimap>
#include <vector>

//---------------------
#include "xoap/MessageReference.h"

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
  xdata::String configurationDir_;

/// @}

  ////////////////////////////////////////////////////////
  // End of exported parameters for configuration       //
  ////////////////////////////////////////////////////////

  /// Generates logger name.

  ///
  /// @return logger name
  ///  
  std::string generateLoggerName();

  /// Binds web address to a method.
  void bindWebInterface();

  /// Exports xdata parameters.
  void exportParams();


  /// Creates a display web page.

  ///
  /// @param in cgi input
  /// @param out cgi output
  ///
  void displayWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  ///
  /// @param in cgi input
  /// @param out cgi output
  ///
  void editorWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  ///
  /// @param in cgi input
  /// @param out cgi output
  ///
  void defaultWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

};

}}

#endif
