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
#include "toolbox/task/Action.h"
#include "toolbox/task/WorkLoop.h"

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

  enum Mode_t { measurement, calibration, nModes };

  /// define factory method for the instantion of AFEBTeststand applications
  XDAQ_INSTANTIATOR();
  
  /// constructor
  Application(xdaq::ApplicationStub *s)
    throw (xdaq::exception::Exception);

  /// the logger of this application
  Logger logger_;

private:
  static const char* const modeNames_[nModes];

  //////////////////////////////////////
  // Beginning of exported parameters //
  //////////////////////////////////////

  ///
  /// \defgroup Application_config_param Application parameters exported for configuration
  ///
/// @{

  xdata::String HTML_ROOT_;
  xdata::String configurationDir_;
  xdata::String configFileNameFilter_;
  xdata::String resultBaseURLDir_; ///< results' base directory relative to HTML_ROOT_
/// @}

  xdata::String reasonForFailure_;

  ////////////////////////////////
  // End of exported parameters //
  ////////////////////////////////

  void createFSM();
  void stateChanged(toolbox::fsm::FiniteStateMachine &fsm);
  void fireEvent( const string name );
  // State transitions
  void configureAction(toolbox::Event::Reference e);
  void enableAction(toolbox::Event::Reference e);
  void haltAction(toolbox::Event::Reference e);
  void stopAction(toolbox::Event::Reference e);
  void noAction(toolbox::Event::Reference e);
  void failAction(toolbox::Event::Reference e);

  void moveToFailedState( xcept::Exception exception );

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

  ///
  /// @param in cgi input
  /// @param out cgi output
  ///
  void resultsWebPage(xgi::Input *in, xgi::Output *out)
    throw (xgi::exception::Exception);

  void loadConfigurationTemplate();

  vector<pair<string,string> > loadConfigurationFileList();

  string createXMLWebPageSkeleton();

  string createResultsXML();

  void copyStyleFilesToResultsDir();

  string setProcessingInstruction( const string XML, const string xslURI )
    throw( xcept::Exception );

  bool measurementInWorkLoop(toolbox::task::WorkLoop *wl);
  bool calibrationInWorkLoop(toolbox::task::WorkLoop *wl);

  toolbox::BSem bsem_;	///< Binary semaphore.
  string applicationURLPath_; ///< the path part of the URL of this application
  static const string applicationNamespace_; ///< the namespace of this application
  static const string workLoopName_; ///< the name of the work loop
  static const string workLoopType_; ///< the type of the work loop
  string processingInstructionSetter_; ///< XSLT to set the processing instruction for XML
  string configurationXML_; ///< XML of the configuration
  toolbox::fsm::FiniteStateMachine fsm_; ///< finite state machine
  Mode_t mode_;			///< Whether to measure or calibrate.
  AFEB::teststand::Configuration* configuration_;  ///< configuration
  int currentMeasurementIndex_; ///< the index of the measurement being performed
  toolbox::task::WorkLoop *workLoop_; ///< work loop for the calibrations or measurements to be executed in a separate thread
  toolbox::task::ActionSignature *measurementSignature_;
  toolbox::task::ActionSignature *calibrationSignature_;
  string rawResultURLDir_; ///< Raw result directory relative to the HTML root.
  string rawResultSystemDir_; ///< The raw result directory given as full path.
  string analyzedResultURLDir_; ///< Analyzed result directory relative to the HTML root.
  string analyzedResultSystemDir_; ///< The analyzed result directory given as full path.
  string host_;  ///< The name of the host we're running on.
};

}}

#endif
