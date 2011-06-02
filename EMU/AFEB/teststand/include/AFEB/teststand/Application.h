#ifndef __AFEB_teststand_Application_h__
#define __AFEB_teststand_Application_h__

#include "xdaq/ApplicationGroup.h"
#include "xdaq/WebApplication.h"
#include "xdata/Boolean.h"
#include "xdata/InfoSpace.h"
#include "xdata/String.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/Vector.h"
#include "toolbox/fsm/FiniteStateMachine.h"

#include <set>
#include <map>
#include <vector>

#include "AFEB/teststand/Configuration.h"

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

  xdata::String HTML_ROOT_;
  xdata::String configurationDir_;
  xdata::String configFileNameFilter_;
  xdata::String resultDir_; ///< results' directory relative to HTML_ROOT_
/// @}

  ////////////////////////////////////////////////////////
  // End of exported parameters for configuration       //
  ////////////////////////////////////////////////////////

  void createFSM();
  void stateChanged(toolbox::fsm::FiniteStateMachine &fsm);
  void fireEvent( const string name );
  // State transitions
  void configureAction(toolbox::Event::Reference e);
  void enableAction(toolbox::Event::Reference e);
  void haltAction(toolbox::Event::Reference e);
  void noAction(toolbox::Event::Reference e);


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

  ///
  /// @param in cgi input
  /// @param out cgi output
  ///
  void controlWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  void loadConfigurationTemplate();

  vector<pair<string,string> > loadConfigurationFileList();

  void initializeParameters();

  string setProcessingInstruction( const string XML, const string xslURI )
    throw( xcept::Exception );

  string applicationURLPath_; ///< the path of the URL of this application
  static const string applicationNamespace_; ///< the namespace of this application
  string processingInstructionSetter_; ///< XSLT to set the processing instruction for XML
  string configurationXML_; ///< XML of the configuration
  string xmlWebPageSkeleton_; ///< skeleton of the XML web page
  toolbox::fsm::FiniteStateMachine fsm_; ///< finite state machine
  AFEB::teststand::Configuration* configuration_;  ///< configuration
  int currentMeasurementIndex_; ///< the index of the measurement being performed
};

}}

#endif
