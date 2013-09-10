#include "emu/pc/Configurator.h"

#include "emu/pc/AppsVersion.h"

#include "emu/utils/System.h"
#include "emu/utils/String.h"
#include "emu/utils/Cgi.h"
#include "emu/utils/IO.h"

#include "emu/soap/Messenger.h"
#include "emu/soap/ToolBox.h"

#include "xdaq/NamespaceURI.h"

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "toolbox/task/WorkLoopFactory.h"

#include <algorithm>

namespace emu {
  namespace pc {

typedef std::vector<CFEB>::iterator CFEBItr;

const std::string       CFEB_FIRMWARE_FILENAME = "cfeb/cfeb_pro.svf";
const std::string       CFEB_VERIFY_FILENAME = "cfeb/cfeb_verify.svf";
const std::string       CFEB_COMPARE_FILENAME = "cfeb/eprom_cfeb.cmp";
//
const std::string       DMB_FIRMWARE_FILENAME   = "dmb/dmb6cntl_pro.svf";
const std::string       DMB_VERIFY_FILENAME     = "dmb/dmb_mprom_verify.svf";
const std::string       DMB_COMPARE_FILENAME    = "dmb/eprom_dmb_mprom.cmp";

const std::string       DMBVME_FIRMWARE_FILENAME = "dmb/dmb6vme_pro.svf";
const std::string       DMBVME_VERIFY_FILENAME   = "dmb/dmb_vprom_verify.svf";
const std::string       DMBVME_COMPARE_FILENAME  = "dmb/eprom_dmb_vprom.cmp";

const std::string	ALCT_SLOW_FIRMWARE_FILENAME = "alct/slow/slow_control3_verify_noabstime.svf";
 //
//In order to load firmware automatically from the firmware values in the xml files, 
//the firmware needs to reside in directories in the form:
//    TMB  ->  $HOME/firmware/tmb/YEARMONTHDAY/type[A,C,D]/tmb.xsvf   <-- N.B. xsvf format for TMB
//    RAT  ->  $HOME/firmware/rat/YEARMONTHDAY/rat.svf
//    ALCT ->  $HOME/firmware/alct/YEARMONTHDAY/alctXXX/alctXXX.xsvf
// with the zero-values filled in with 0's.  
// In other words:  9 April 2007 firmware should reside in YEARMONTHDAY=20070409
//
// The XXX in the ALCT firmware specification corresponds to the following structure:
const std::string ALCT_FIRMWARE_FILENAME_ME11 = "alct288/alct288";
const std::string ALCT_READBACK_FILENAME_ME11 = "alct288/alct288_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_NEGATIVE = "alct288bn/alct288bn";
const std::string ALCT_READBACK_FILENAME_ME11_BACKWARD_NEGATIVE = "alct288bn/alct288bn_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_POSITIVE = "alct288bp/alct288bp";
const std::string ALCT_READBACK_FILENAME_ME11_BACKWARD_POSITIVE = "alct288bp/alct288bp_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME11_FORWARD_POSITIVE  = "alct288fp/alct288fp";
const std::string ALCT_READBACK_FILENAME_ME11_FORWARD_POSITIVE  = "alct288fp/alct288fp_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME12 = "alct384/alct384"; 
const std::string ALCT_READBACK_FILENAME_ME12 = "alct384/alct384_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME13 = "alct192/alct192"; 
const std::string ALCT_READBACK_FILENAME_ME13 = "alct192/alct192_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME21 = "alct672/alct672";
const std::string ALCT_READBACK_FILENAME_ME21 = "alct672/alct672_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME22 = "alct384/alct384";
const std::string ALCT_READBACK_FILENAME_ME22 = "alct384/alct384_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME31 = "alct576mirror/alct576mirror";
const std::string ALCT_READBACK_FILENAME_ME31 = "alct576mirror/alct576mirror_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME32 = "alct384mirror/alct384mirror";
const std::string ALCT_READBACK_FILENAME_ME32 = "alct384mirror/alct384mirror_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME41 = "alct576mirror/alct576mirror";
const std::string ALCT_READBACK_FILENAME_ME41 = "alct576mirror/alct576mirror_verify";
//
const std::string ALCT_FIRMWARE_FILENAME_ME42 = "alct384mirror/alct384mirror";
const std::string ALCT_READBACK_FILENAME_ME42 = "alct384mirror/alct384mirror_verify";
//
// Old svf files (to be deprecated once it is verified that the xsvf firmware is working...
//const std::string ALCT_FIRMWARE_FILENAME_ME11 = "alct288/alct288.svf"; //
//const std::string ALCT_READBACK_FILENAME_ME11 = "readback-192-288-384";//
//
//const std::string ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_NEGATIVE = "alct288bn/alct288bn.svf";//
//const std::string ALCT_READBACK_FILENAME_ME11_BACKWARD_NEGATIVE = "readback-192-288-384";   //
//
//const std::string ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_POSITIVE = "alct288bp/alct288bp.svf";//
//const std::string ALCT_READBACK_FILENAME_ME11_BACKWARD_POSITIVE = "readback-192-288-384";   //
//
//const std::string ALCT_FIRMWARE_FILENAME_ME11_FORWARD_POSITIVE  = "alct288fp/alct288fp.svf";//
//const std::string ALCT_READBACK_FILENAME_ME11_FORWARD_POSITIVE  = "readback-192-288-384";   //
//
//const std::string ALCT_FIRMWARE_FILENAME_ME12 = "alct384/alct384.svf"; //
//const std::string ALCT_READBACK_FILENAME_ME12 = "readback-192-288-384";//
//
//const std::string ALCT_FIRMWARE_FILENAME_ME13 = "alct192/alct192.svf"; //
//const std::string ALCT_READBACK_FILENAME_ME13 = "readback-192-288-384";//
//
//const std::string ALCT_FIRMWARE_FILENAME_ME21 = "alct672/alct672.svf";//
//const std::string ALCT_READBACK_FILENAME_ME21 = "readback-576-672";   //
//
//const std::string ALCT_FIRMWARE_FILENAME_ME22 = "alct384/alct384.svf"; //
//const std::string ALCT_READBACK_FILENAME_ME22 = "readback-192-288-384";//
//
//const std::string ALCT_FIRMWARE_FILENAME_ME31 = "alct576mirror/alct576mirror.svf";//
//const std::string ALCT_READBACK_FILENAME_ME31 = "readback-576-672";               //
//
//const std::string ALCT_FIRMWARE_FILENAME_ME32 = "alct384mirror/alct384mirror.svf";//
//const std::string ALCT_READBACK_FILENAME_ME32 = "readback-192-288-384";           //
//  
//const std::string ALCT_FIRMWARE_FILENAME_ME41 = "alct576mirror/alct576mirror.svf";//
//const std::string ALCT_READBACK_FILENAME_ME41 = "readback-576-672";               //
//
const int CCB_LABEL         = 0;
const int MPC_LABEL         = 1;
const int TMB_LABEL         = 2;
const int ALCT_LABEL        = 3;
const int DMB_VME_LABEL     = 4;
const int DMB_CONTROL_LABEL = 5;
const int CFEB_LABEL[5]     = {6, 7, 8, 9, 10};
}
}

using namespace std;

XDAQ_INSTANTIATOR_IMPL(emu::pc::Configurator)

emu::pc::Configurator::Configurator( xdaq::ApplicationStub *s )
  : xdaq::WebApplication( s )
  , emu::pc::Application( s )
  , emu::pc::VMEAccessor( s )
  , selectedSource_( emu::pc::Configuration::noSource )
  , configuration_( emu::pc::Configuration().setParentApplication( this ) ){
  initializeParameters();
  exportParameters();
  bindWebInterface();
  bindSOAPInterface();
}

void emu::pc::Configurator::configureAction( toolbox::Event::Reference e ){

  // Remove existing work loop...
  try{
    // It's tricky to find a work loop by name as 
    // toolbox::task::WorkLoopFactory::getWorkLoop(const std::string & name, const std::string & type)
    // creates a work loop named '<name>/<type>' for some reason.
    // Let's just try to remove it blindly instead, 
    // and catch the exception it throws if no such work loop exists.
    toolbox::task::getWorkLoopFactory()->removeWorkLoop( workLoopName_, workLoopType_ );
    LOG4CPLUS_DEBUG( logger_, "Removed " << workLoopType_ << " work loop " << workLoopName_ );
  }
  catch( xcept::Exception &e ){
    LOG4CPLUS_WARN( logger_, "Failed to remove leftover work loop: " << xcept::stdformat_exception_history( e ) );
  }
  //...and get a new one
  try{
    workLoop_ = toolbox::task::getWorkLoopFactory()->getWorkLoop( workLoopName_, workLoopType_ );
    LOG4CPLUS_INFO( logger_, "Created " << workLoopType_ << " work loop " << workLoopName_ );
  }
  catch( xcept::Exception &e ){
    stringstream ss;
    ss << "Failed to recreate " << workLoopType_ << " work loop " << workLoopName_;
    XCEPT_RETHROW( toolbox::fsm::exception::Exception, ss.str(), e );
  }

  try{
    if ( bool( isSlave_ ) ){
      //
      // SLAVE
      //
      if ( selectedConfigIds_.size() == 1 ){
	LOG4CPLUS_INFO( logger_, "Setting configuration from selectedSource=" << selectedSource_ << ", ethPort=" << int( ethPort_ ) << ", crateIdsCSV=" << emu::utils::csvTo<set<int> >( crateIdsCSV_.toString() ) ); 
	configuration_.setFrom( selectedSource_, selectedConfigIds_.at( 0 ), int( ethPort_ ), emu::utils::csvTo<set<int> >( crateIdsCSV_.toString() ) );
	LOG4CPLUS_INFO( logger_, "Configuration length: original=" << selectedConfigIds_.at( 0 ).size() << ", pruned=" << configuration_.getXML().size() << "\n" << configuration_.getXML() );
      }
      else{
	XCEPT_RAISE( toolbox::fsm::exception::Exception, "No configuration XML string found." );
      }

    }
    else{ // if ( bool( isSlave_ ) )
      //
      // MASTER
      //      
      configuration_.setFrom( selectedSource_, emu::utils::csvFrom<vector<string> >( selectedConfigIds_ ) );
      
      // Configure each slave
      emu::soap::Messenger m( this );
      set<int> allCrateIds; // just to check for duplicates
      map<xdaq::ApplicationDescriptor*,VMEAccessor::Status_t>::iterator ss;
      for ( ss = slavesStatus_.begin(); ss != slavesStatus_.end(); ++ss ){
	ss->second = VMEAccessor::Status_t(); // forget old status, start with a clean slate
	xdata::String crateIdsCSV;     
	m.getParameters( ss->first, emu::soap::Parameters().add( "crateIdsCSV", &crateIdsCSV ) );
	ss->second.crateIdsCSV = crateIdsCSV;
	const vector<int> crateIds = emu::utils::csvTo<vector<int> >( crateIdsCSV );
	for ( vector<int>::const_iterator id = crateIds.begin(); id != crateIds.end(); ++id ){
	  if ( ! allCrateIds.insert( *id ).second ){
	    ostringstream oss;
	    oss << "Failed to set the configuration due to multiple occurrence of crate id " << *id << " in slaves.";
	    webPageMessage_ = "<div class=\"warning\">" + oss.str() + "</div>";
	    XCEPT_RAISE( toolbox::fsm::exception::Exception, oss.str() );
	  }
	}
	xdata::String configurationXML = configuration_.getXML();
	LOG4CPLUS_INFO( logger_, "Configuration XML length: " << configurationXML.toString().size() );
	m.sendCommand( ss->first, "SetConfiguration", emu::soap::Parameters().add( "configurationXML", &configurationXML ) );
	m.sendCommand( ss->first, "Configure" );
      }

    } // if ( bool( isSlave_ ) ) else
    
  }
  catch( xcept::Exception &e ){
    XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to set configuration.", e );
  }

  configuration_.getEmuEndcap()->NotInDCS();
  crateVector = configuration_.getEmuEndcap()->crates();
  total_crates_=crateVector.size();
  this_crate_no_=0;

  cout << configuration_ << endl;
  // emu::pc::TMB *tmb = configuration_.getEmuEndcap()->tmbs().at( 0 );
  // cout << *tmb << endl;
}

void emu::pc::Configurator::enableAction( toolbox::Event::Reference e ){
  bsem_.take();

  // It's only to be stopped at user's request
  isToStop_ = false;

  // Zero our own progress
  progressPercent_ = 0;

  // Zero every slave's progress
  map<xdaq::ApplicationDescriptor*,VMEAccessor::Status_t>::iterator ss;
  for ( ss = slavesStatus_.begin(); ss != slavesStatus_.end(); ++ss ) ss->second.progressPercent = 0;

  // Reset exception caught in work thread to an empty one
  workLoopsCatch_ = xcept::Exception();

  bsem_.give();

  // Schedule VME access to be executed in a separate thread
  try{
    workLoop_->submit( vmeAccessSignature_ );
    LOG4CPLUS_INFO( logger_, "Submitted VME access action to " << workLoopType_ << " work loop " << workLoopName_ );
  } catch( xcept::Exception &e ){
    XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to submit VME access action to work loop.", e );
  }

  // Activate the separate thread in case it's not active
  if ( ! workLoop_->isActive() ){
    try{
      workLoop_->activate();
      LOG4CPLUS_INFO( logger_, "Activated work loop." );
    } catch( xcept::Exception &e ){
      XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to activate work loop.", e );
    }
  }
}

void emu::pc::Configurator::haltAction( toolbox::Event::Reference e ){}

void emu::pc::Configurator::stopAction( toolbox::Event::Reference e ){
  if ( ! bool( isSlave_ ) && bool( isDistributed_ ) ){
    try{
      stopSlaves();
    }
    catch( xcept::Exception &e ){
      XCEPT_RETHROW( toolbox::fsm::exception::Exception, "Failed to stop slaves.", e );
    }
  }
  else{
    bsem_.take();
    isToStop_ = true;
    bsem_.give();
  }
}

void emu::pc::Configurator::noAction( toolbox::Event::Reference e ){
  if ( e->type() == "Configure" && fsm_.getCurrentState() == 'E' ){
    webPageMessage_ = "<div class=\"warning\">Cannot set configuration as '" + currentTaskName_.toString() + "' is still running. Please stop it first or wait for it to finish.</div>";
    cout  << endl << endl << "noAction" << webPageMessage_ << endl << endl;
  }
}

void emu::pc::Configurator::resetAction( toolbox::Event::Reference e ){
  configuration_.unset();
}

bool emu::pc::Configurator::vmeAccessInWorkLoop( toolbox::task::WorkLoop *wl ){
  bsem_.take();
  isDone_ = false;
  bsem_.give();

  if ( bool( isSlave_ ) ){
    //
    // SLAVE
    //

    // dummyTask();

    if ( currentTaskName_.toString() == "ConfigAllCrates" ){
      try{
	ConfigureInit(0);
      }
      catch( xcept::Exception& e ){
	XCEPT_DECLARE_NESTED( xcept::Exception, ee, "Failed to configure crates.", e );
	LOG4CPLUS_FATAL( logger_, stdformat_exception_history( ee ) );
	bsem_.take();
	workLoopsCatch_ = ee;
	bsem_.give();
	return false;
      }
      catch( std::exception& e ){
	ostringstream oss;
	oss <<  "Failed to configure crates : " << e.what();
	XCEPT_DECLARE( xcept::Exception, ee, oss.str() );
	LOG4CPLUS_FATAL( logger_, oss.str() );
	bsem_.take();
	workLoopsCatch_ = ee;
	bsem_.give();
	return false;
      }
    }

    bsem_.take();
    isDone_ = true;
    bsem_.give();
  }
  else{
    //
    // MASTER
    //
    if ( bool( isDistributed_ ) ){
      //
      // DISTRIBUTED
      //
      emu::soap::Messenger m( this );
      //
      // Enable slaves
      //
      try{
	map<xdaq::ApplicationDescriptor*,VMEAccessor::Status_t>::iterator ss;
	for ( ss = slavesStatus_.begin(); ss != slavesStatus_.end(); ++ss ){
	  m.setParameters( ss->first, emu::soap::Parameters().add( "currentTaskName", &currentTaskName_ ) );
	  m.sendCommand( ss->first, "Enable" );
	}
      }
      catch( xcept::Exception &e ){
	XCEPT_DECLARE_NESTED( xcept::Exception, ee, "Failed to enable slaves.", e );
	LOG4CPLUS_FATAL( logger_, stdformat_exception_history( ee ) );
	bsem_.take();
	workLoopsCatch_ = ee;
	bsem_.give();
	return false;
      }
      //
      // Wait for slaves to finish
      //
      bool waitForUserToStop = false; // Wait for the user to click the stop button or stop automatically?
      try{
	waitForUserToStop = ! waitForSlavesToFinish();
      }
      catch( xcept::Exception &e ){
	XCEPT_DECLARE_NESTED( xcept::Exception, ee, string( "Task " ) + currentTaskName_.toString() + " failed.", e );
	LOG4CPLUS_FATAL( logger_, stdformat_exception_history( ee ) );
	bsem_.take();
	workLoopsCatch_ = ee;
	bsem_.give();
	waitForUserToStop = true;
      }
      //
      // Stop slaves
      //
      if ( !waitForUserToStop ){ // We stop automatically if all slaves seem to have completed normally.
	bsem_.take();
	isDone_ = true;
	bsem_.give();
	fireEvent( "Stop" );
      }
    }
    else{
      //
      // NOT DISTRIBUTED
      //
      bsem_.take();
      isDone_ = false;
      bsem_.give();
      try{
	if      ( currentTaskName_.toString() == "ConfigAllCrates"  ) ConfigureInit(0);
	else if ( currentTaskName_.toString() == "FastConfigCrates" ) ConfigureInit(2);
      }
      catch( std::exception& e ){
	XCEPT_DECLARE( xcept::Exception, ee, string( "Task " ) + currentTaskName_.toString() + " failed: " + e.what() );
	LOG4CPLUS_FATAL( logger_, stdformat_exception_history( ee ) );
	bsem_.take();
	workLoopsCatch_ = ee;
	bsem_.give();
      }
      bsem_.take();
      isDone_ = true;
      bsem_.give();
    }

  }

  return false;
}

bool emu::pc::Configurator::waitForSlavesToFinish(){
  emu::soap::Messenger m( this );
  bool allSurvivorsFinished = true; // Whether all that didn't fail have already finished.
  bool noneFailed = true; // Whether none of the slaves failed.
  do {
    // Query the slaves
    allSurvivorsFinished = true;
    xdata::String  state;
    xdata::String  reasonForFailure;
    xdata::Integer progressPercent;
    xdata::Boolean isDone;
    map<xdaq::ApplicationDescriptor*,VMEAccessor::Status_t>::iterator ss;
    for ( ss = slavesStatus_.begin(); ss != slavesStatus_.end(); ++ss ){
      try{
	LOG4CPLUS_INFO( logger_, "Querying slave instance " << ss->first->getInstance() );
	emu::soap::extractParameters( m.sendCommand( ss->first, "QueryStatus" ),
				      emu::soap::Parameters()
				      .add( "state"           , &state            )
				      .add( "reasonForFailure", &reasonForFailure ) // empty if not in failed state
				      .add( "progressPercent" , &progressPercent  )
				      .add( "isDone"          , &isDone           ) );
	bsem_.take();
	ss->second.state            = state;
	ss->second.reasonForFailure = reasonForFailure;
	ss->second.exception        = xcept::Exception();
	ss->second.progressPercent  = progressPercent;
	ss->second.isDone           = isDone;
	bsem_.give();
	allSurvivorsFinished &= bool( isDone );
      }
      catch( xcept::Exception &e ){
	ostringstream oss;
	oss << "Failed to query status of slave instance " << ss->first->getInstance()
	    << ". " << stdformat_exception_history( e );
	LOG4CPLUS_ERROR( logger_, oss.str() );
	bsem_.take();
	ss->second.exception = e;
	bsem_.give();
	noneFailed = false;
      }
    }
    // LOG4CPLUS_INFO( logger_, "Not all finished yet." );
    ::sleep( 1 );
  } while ( !allSurvivorsFinished && fsm_.getCurrentState() == 'E' );
  return noneFailed;
}

void emu::pc::Configurator::stopSlaves(){
  emu::soap::Messenger m( this );
  vector<xcept::Exception> exceptions;
  map<xdaq::ApplicationDescriptor*,VMEAccessor::Status_t>::iterator ss;
  for ( ss = slavesStatus_.begin(); ss != slavesStatus_.end(); ++ss ){
    try{
      m.sendCommand( ss->first, "Stop" );
    }
    catch( xcept::Exception &e ){
      exceptions.push_back( e );
    }
  }
  if ( exceptions.size() > 0 ){
    ostringstream oss;
    oss << exceptions.size() << " slaves failed to stop. The reason for the first failure is included.";
    XCEPT_DECLARE_NESTED( xcept::Exception, ee, oss.str(), exceptions.front() );
    throw ee;
  }
}

void emu::pc::Configurator::initializeParameters(){


  FirmwareDir_ = getenv("HOME");
  FirmwareDir_ += "/firmware/";
  XMLDIR = "/opt/xdaq/htdocs/emu/emuDCS/PeripheralApps/xml";
  //
  DisplayRatio_ = false;
  AutoRefresh_  = true;
  //thisTMB = 0;
  //thisDMB = 0;
  thisCCB = 0;
  thisMPC = 0;
  rat = 0;
  alct = 0;
  nTrigger_ = 100;
  MenuMonitor_ = 2;
  //
  tmb_vme_ready = -1;
  crate_controller_status = -1;
  //
  all_crates_ok = -1;
  for (int i=0; i<60; i++) {
    crate_check_ok[i] = -1;
    ccb_check_ok[i] = -1;
    mpc_check_ok[i] = -1;
    for (int j=0; j<9; j++) {
      alct_check_ok[i][j] = -1;
      tmb_check_ok[i][j] = -1;
      dmb_check_ok[i][j] = -1;
      time_since_reset[i][j] = -1;
      bc0_sync[i][j] = -1;
    }
  }
	total_bad_cfeb_bits = -1;
	total_good_cfeb_bits = -1;
  //
  crates_firmware_ok = -1;
  for (int i=0; i<60; i++) {
    crate_firmware_ok[i] = -1;
    ccb_firmware_ok[i] = -1;
    mpc_firmware_ok[i] = -1;
    for (int j=0; j<9; j++) {
      alct_firmware_ok[i][j] = -1;
      tmb_firmware_ok[i][j] = -1;
      dmb_vme_firmware_ok[i][j] = -1;
      dmb_control_firmware_ok[i][j] = -1;
      dmbcfg_ok[i][j]  = -1;
      dmb_config_ok[i][j]  = -1;
      alctcfg_ok[i][j]  = -1;
      alct_lvmb_current_ok[i][j]  = -1;
      alct_adc_current_ok[i][j]  = -1;
      //
      for (int k=0; k<5; k++) {
	cfeb_firmware_ok[i][j][k] = -1;
	cfeb_config_ok[i][j][k]   = -1;
	cfeb_current_ok[i][j][k]  = -1;
      }
    }
  }
  //
  firmware_checked_ = 0;
  number_of_checks_ = 4;
  number_of_fw_readbacks = 25;
  number_of_hard_resets_ = number_of_checks_ - 1;

  //
  xml_or_db = -1;  /* actual configuration source: 0: xml, 1: db, -1: unknown or error */
  XML_or_DB_ = "xml";
  EMU_config_ID_ = "1000001";
  xmlFile_ = "config.xml" ;
  Valid_config_ID="";
  InFlash_config_ID="";
  //
  for(unsigned int dmb=0; dmb<9; dmb++) {
    L1aLctCounter_.push_back(0);
    CfebDavCounter_.push_back(0);
    TmbDavCounter_.push_back(0);
    AlctDavCounter_.push_back(0);
  }
  //
  CCBRegisterRead_ = -1;
  CCBRegisterValue_ = -1;
  CCBRegisterWrite_ = -1;
  CCBWriteValue_ = -1;
  MPCRegisterRead_ = -1;
  MPCRegisterValue_ = -1;
  MPCRegisterWrite_ = -1;
  MPCWriteValue_ = -1;
  //
  CalibrationState_ = "None";
  standalone_ = false;
  //
  for (int i=0; i<9; i++) {
    able_to_load_alct[i] = -1;  
    number_of_tmb_firmware_errors[i] = -1;
    number_of_alct_firmware_errors[i] = -1;
  }
  for (int i=0; i<9; i++) 
    for (int j=0; j<5; j++)
      CFEBid_[i][j] = -2;
  //
  for(int i=0; i<9;i++) {
    OutputStringDMBStatus[i] << "DMB-CFEB Status " << i << " output:" << std::endl;
    OutputStringTMBStatus[i] << "TMB-RAT Status " << i << " output:" << std::endl;
  }
  CrateTestsOutput << "Crate Tests output:" << std::endl;

  // for XMAS minotoring:

  Monitor_On_ = false;
  Monitor_Ready_ = false;

  global_config_states[0]="UnConfigured";
  global_config_states[1]="Configuring";
  global_config_states[2]="Configured";
  global_run_states[0]="Halted";
  global_run_states[1]="Enabled";
  current_config_state_=0;
  current_run_state_=0;
  total_crates_=0;
  this_crate_no_=0;

  prbs_test_ = false;
  brddb= new emu::db::BoardsDB();

  parsed=0;
}

void emu::pc::Configurator::exportParameters(){
  this->getApplicationInfoSpace()->fireItemAvailable( "XMLorDB",            &XML_or_DB_        );
  this->getApplicationInfoSpace()->fireItemAvailable( "EmuConfigurationID", &EMU_config_ID_    );
  this->getApplicationInfoSpace()->fireItemAvailable( "xmlFileName",        &xmlFile_          );
  this->getApplicationInfoSpace()->fireItemAvailable( "CalibrationState",   &CalibrationState_ );
  this->getApplicationInfoSpace()->fireItemAvailable( "Calibtype",          &CalibType_        );
  this->getApplicationInfoSpace()->fireItemAvailable( "Calibnumber",        &CalibNumber_      );
  this->getApplicationInfoSpace()->fireItemAvailable( "isStandalone",       &isStandalone_     );
  this->getApplicationInfoSpace()->fireItemAvailable( "tStoreViewFile",     &tStoreViewFile_   );
  this->getApplicationInfoSpace()->addItemChangedListener( "EmuConfigurationID", this );
  this->getApplicationInfoSpace()->addItemChangedListener( "tStoreViewFile"    , this );
  this->getApplicationInfoSpace()->addItemChangedListener( "XMLorDB"           , this );
  this->getApplicationInfoSpace()->addItemChangedListener( "isStandalone"      , this );
}

void emu::pc::Configurator::bindWebInterface(){
  xgi::bind( this, &emu::pc::Configurator::Default           , "Default"            );
  xgi::bind( this, &emu::pc::Configurator::ConfigSelectorPage, "ConfigSelectorPage" );
  xgi::bind( this, &emu::pc::Configurator::MainPage          , "MainPage"           );

  //------------------------------------------------------
  // bind buttons -> Crate Configuration pages
  //------------------------------------------------------
  xgi::bind(this,&emu::pc::Configurator::TMBStatus, "TMBStatus");
  xgi::bind(this,&emu::pc::Configurator::TMBTests,  "TMBTests");
  xgi::bind(this,&emu::pc::Configurator::TMBUtils,  "TMBUtils");
  xgi::bind(this,&emu::pc::Configurator::DMBStatus, "DMBStatus");
  xgi::bind(this,&emu::pc::Configurator::DMBTests, "DMBTests");
  xgi::bind(this,&emu::pc::Configurator::DMBUtils, "DMBUtils");
  xgi::bind(this,&emu::pc::Configurator::CCBStatus, "CCBStatus");
  xgi::bind(this,&emu::pc::Configurator::CCBUtils, "CCBUtils");
  xgi::bind(this,&emu::pc::Configurator::MPCStatus, "MPCStatus");
  xgi::bind(this,&emu::pc::Configurator::MPCUtils, "MPCUtils");
  xgi::bind(this,&emu::pc::Configurator::DDUStatus, "DDUStatus");
  xgi::bind(this,&emu::pc::Configurator::DDUUtils, "DDUUtils");
  //
  //---------------------------------
  // bind check crate configuration
  //---------------------------------
  xgi::bind(this,&emu::pc::Configurator::CheckConfigurationPage,"CheckConfigurationPage");
  xgi::bind(this,&emu::pc::Configurator::CheckTimeSinceHardReset,"CheckTimeSinceHardReset");
  xgi::bind(this,&emu::pc::Configurator::CheckBC0Synchronization,"CheckBC0Synchronization");
  xgi::bind(this,&emu::pc::Configurator::CheckCratesConfiguration, "CheckCratesConfiguration");
  xgi::bind(this,&emu::pc::Configurator::CheckCratesConfigurationFull, "CheckCratesConfigurationFull");
  xgi::bind(this,&emu::pc::Configurator::CheckCrateConfiguration, "CheckCrateConfiguration");
  xgi::bind(this,&emu::pc::Configurator::CheckCrateFirmware, "CheckCrateFirmware");
  //
  //---------------------------------
  // bind check firmware
  //---------------------------------
  xgi::bind(this,&emu::pc::Configurator::PowerOnFixCFEB, "PowerOnFixCFEB");
  xgi::bind(this,&emu::pc::Configurator::CheckFirmware, "CheckFirmware");
  xgi::bind(this,&emu::pc::Configurator::FixCFEB, "FixCFEB");
  xgi::bind(this,&emu::pc::Configurator::SetNumberOfHardResets, "SetNumberOfHardResets");
  xgi::bind(this,&emu::pc::Configurator::ReadbackALCTFirmware, "ReadbackALCTFirmware");
  //
  //---------------------------------
  // bind Expert Tools pages
  //---------------------------------
  xgi::bind(this,&emu::pc::Configurator::ExpertToolsPage,"ExpertToolsPage");
  xgi::bind(this,&emu::pc::Configurator::StartPRBS, "StartPRBS");
  xgi::bind(this,&emu::pc::Configurator::StopPRBS, "StopPRBS");
  xgi::bind(this,&emu::pc::Configurator::SetRadioactivityTrigger, "SetRadioactivityTrigger");
  xgi::bind(this,&emu::pc::Configurator::SetRadioactivityTriggerALCTOnly, "SetRadioactivityTriggerALCTOnly");
  xgi::bind(this,&emu::pc::Configurator::SetTTCDelays, "SetTTCDelays");
  xgi::bind(this,&emu::pc::Configurator::MeasureAllTMBVoltages, "MeasureAllTMBVoltages");
  xgi::bind(this,&emu::pc::Configurator::SetTwoLayerTriggerForSystem, "SetTwoLayerTriggerForSystem");
  xgi::bind(this,&emu::pc::Configurator::MeasureL1AsAndDAVsForSystem,"MeasureL1AsAndDAVsForSystem");
  xgi::bind(this,&emu::pc::Configurator::MeasureALCTTMBRxTxForSystem,"MeasureALCTTMBRxTxForSystem");
  xgi::bind(this,&emu::pc::Configurator::MeasureCFEBTMBRxForSystem,"MeasureCFEBTMBRxForSystem");
  xgi::bind(this,&emu::pc::Configurator::QuickScanForSystem,"QuickScanForSystem");
  xgi::bind(this,&emu::pc::Configurator::UpdateInFlashKey, "UpdateInFlashKey");
  //
  //------------------------------
  // bind crate utilities
  //------------------------------
  xgi::bind(this,&emu::pc::Configurator::TmbMPCTest, "TmbMPCTest");
  xgi::bind(this,&emu::pc::Configurator::MPCSafeWindowScan, "MPCSafeWindowScan");
  //
  // xgi::bind(this,&emu::pc::Configurator::CheckCrates, "CheckCrates");
  // xgi::bind(this,&emu::pc::Configurator::CrateSelection, "CrateSelection");
  // xgi::bind(this,&emu::pc::Configurator::setRawConfFile, "setRawConfFile");
  // xgi::bind(this,&emu::pc::Configurator::UploadConfFile, "UploadConfFile");
  // xgi::bind(this,&emu::pc::Configurator::DefineConfiguration, "DefineConfiguration");

  xgi::bind(this,&emu::pc::Configurator::testTMB, "testTMB");
  xgi::bind(this,&emu::pc::Configurator::CrateConfiguration, "CrateConfiguration");
  xgi::bind(this,&emu::pc::Configurator::CrateTests, "CrateTests");
  xgi::bind(this,&emu::pc::Configurator::ChamberTests, "ChamberTests");
  // xgi::bind(this,&emu::pc::Configurator::ConfigAllCrates, "ConfigAllCrates");
  // xgi::bind(this,&emu::pc::Configurator::FastConfigCrates, "FastConfigCrates");
  // xgi::bind(this,&emu::pc::Configurator::FastConfigOne, "FastConfigOne");
  // xgi::bind(this,&emu::pc::Configurator::ConfigIDSelection, "ConfigIDSelection");
  //
  xgi::bind(this,&emu::pc::Configurator::MeasureL1AsAndDAVsForCrate,"MeasureL1AsAndDAVsForCrate");
  xgi::bind(this,&emu::pc::Configurator::MeasureL1AsAndDAVsForChamber,"MeasureL1AsAndDAVsForChamber");
  xgi::bind(this,&emu::pc::Configurator::MeasureL1AsForCrate,"MeasureL1AsForCrate");
  xgi::bind(this,&emu::pc::Configurator::MeasureDAVsForCrate,"MeasureDAVsForCrate");
  xgi::bind(this,&emu::pc::Configurator::MeasureALCTTMBRxTxForCrate,"MeasureALCTTMBRxTxForCrate");
  xgi::bind(this,&emu::pc::Configurator::MeasureCFEBTMBRxForCrate,"MeasureCFEBTMBRxForCrate");
  xgi::bind(this,&emu::pc::Configurator::SetTwoLayerTriggerForCrate, "SetTwoLayerTriggerForCrate");
  xgi::bind(this,&emu::pc::Configurator::QuickScanForChamber,"QuickScanForChamber");
  xgi::bind(this,&emu::pc::Configurator::QuickScanForCrate,"QuickScanForCrate");
  //
  //-----------------------------------------------
  // CCB & MPC routines
  //-----------------------------------------------
  xgi::bind(this,&emu::pc::Configurator::ReadCCBRegister, "ReadCCBRegister");
  xgi::bind(this,&emu::pc::Configurator::WriteCCBRegister, "WriteCCBRegister");
  xgi::bind(this,&emu::pc::Configurator::ReadMPCRegister, "ReadMPCRegister");
  xgi::bind(this,&emu::pc::Configurator::WriteMPCRegister, "WriteMPCRegister");
  xgi::bind(this,&emu::pc::Configurator::MPCLoadFirmware, "MPCLoadFirmware");
  xgi::bind(this,&emu::pc::Configurator::ReadTTCRegister, "ReadTTCRegister");
  xgi::bind(this,&emu::pc::Configurator::HardReset, "HardReset");
  xgi::bind(this,&emu::pc::Configurator::CCBLoadFirmware, "CCBLoadFirmware");
  xgi::bind(this,&emu::pc::Configurator::PrepareForTriggering, "PrepareForTriggering");
  xgi::bind(this,&emu::pc::Configurator::CCBConfig, "CCBConfig");
  xgi::bind(this,&emu::pc::Configurator::CCBSignals, "CCBSignals");
  xgi::bind(this,&emu::pc::Configurator::MPCConfig, "MPCConfig");
  xgi::bind(this,&emu::pc::Configurator::CCBReadFirmware, "CCBReadFirmware");
  xgi::bind(this,&emu::pc::Configurator::MPCReadFirmware, "MPCReadFirmware");
  //
  //-----------------------------------------------
  // VME Controller routines
  //-----------------------------------------------
  xgi::bind(this,&emu::pc::Configurator::ControllerUtils_Xfer, "ControllerUtils_Xfer");  
  xgi::bind(this,&emu::pc::Configurator::ControllerUtils, "ControllerUtils");  
  xgi::bind(this,&emu::pc::Configurator::VMECCGUI_GoTo_General,  "VMECCGUI_GoTo_General");
  xgi::bind(this,&emu::pc::Configurator::VMECCGUI_GoTo_Intermediate,  "VMECCGUI_GoTo_Intermediate");
  xgi::bind(this,&emu::pc::Configurator::VMECCGUI_GoTo_Expert,  "VMECCGUI_GoTo_Expert");
  xgi::bind(this,&emu::pc::Configurator::VMECCGUI_firmware_utils,  "VMECCGUI_firmware_utils");
  xgi::bind(this,&emu::pc::Configurator::VMECCGUI_cnfg_utils,  "VMECCGUI_cnfg_utils");
  xgi::bind(this,&emu::pc::Configurator::VMECCGUI_MAC_utils,  "VMECCGUI_MAC_utils");
  xgi::bind(this,&emu::pc::Configurator::VMECCGUI_FIFO_utils,  "VMECCGUI_FIFO_utils");
  xgi::bind(this,&emu::pc::Configurator::VMECCGUI_pkt_send,  "VMECCGUI_pkt_send");
  xgi::bind(this,&emu::pc::Configurator::VMECCGUI_pkt_rcv,  "VMECCGUI_pkt_rcv");
  xgi::bind(this,&emu::pc::Configurator::VMECCGUI_misc_utils,  "VMECCGUI_misc_utils");
  xgi::bind(this,&emu::pc::Configurator::VCC_VME_DO,  "VCC_VME_DO");
  xgi::bind(this,&emu::pc::Configurator::VCC_VME_FILL,  "VCC_VME_FILL");
  xgi::bind(this,&emu::pc::Configurator::VCC_CRSEL_DO,  "VCC_CRSEL_DO");
  xgi::bind(this,&emu::pc::Configurator::VCC_PSWD_DO,  "VCC_PSWD_DO");
  xgi::bind(this,&emu::pc::Configurator::VCC_CMNTSK_DO,  "VCC_CMNTSK_DO");
  xgi::bind(this,&emu::pc::Configurator::VCC_FRMUTIL_DO,  "VCC_FRMUTIL_DO");
  xgi::bind(this,&emu::pc::Configurator::VCC_CNFG_DO,  "VCC_CNFG_DO");
  xgi::bind(this,&emu::pc::Configurator::VCC_MAC_DO,  "VCC_MAC_DO");
  xgi::bind(this,&emu::pc::Configurator::VCC_FIFO_DO,  "VCC_FIFO_DO");
  xgi::bind(this,&emu::pc::Configurator::VCC_PKTSND_DO,  "VCC_PKTSND_DO");
  xgi::bind(this,&emu::pc::Configurator::VCC_PKTRCV_DO,  "VCC_PKTRCV_DO");
  xgi::bind(this,&emu::pc::Configurator::VCC_MISC_DO,  "VCC_MISC_DO");

  //-----------------------------------------------
  // DMB tests
  //-----------------------------------------------
  xgi::bind(this,&emu::pc::Configurator::DMBTestAll, "DMBTestAll");
  xgi::bind(this,&emu::pc::Configurator::DMBTest3, "DMBTest3");
  xgi::bind(this,&emu::pc::Configurator::DMBTest4, "DMBTest4");
  xgi::bind(this,&emu::pc::Configurator::DMBTest5, "DMBTest5");
  xgi::bind(this,&emu::pc::Configurator::DMBTest6, "DMBTest6");
  xgi::bind(this,&emu::pc::Configurator::DMBTest8, "DMBTest8");
  xgi::bind(this,&emu::pc::Configurator::DMBTest9, "DMBTest9");
  xgi::bind(this,&emu::pc::Configurator::DMBTest10, "DMBTest10");
  xgi::bind(this,&emu::pc::Configurator::DMBTest11, "DMBTest11");
  xgi::bind(this,&emu::pc::Configurator::RTRGlow, "RTRGlow");
  xgi::bind(this,&emu::pc::Configurator::RTRGhigh, "RTRGhigh");

  //
  //-----------------------------------------------
  // DMB utilities
  //-----------------------------------------------
  xgi::bind(this,&emu::pc::Configurator::CFEBStatus, "CFEBStatus");
  xgi::bind(this,&emu::pc::Configurator::DMBPrintCounters, "DMBPrintCounters");
  xgi::bind(this,&emu::pc::Configurator::DMBTurnOff, "DMBTurnOff");
  xgi::bind(this,&emu::pc::Configurator::DMBTurnOn, "DMBTurnOn");
  xgi::bind(this,&emu::pc::Configurator::CFEBTurnOn, "CFEBTurnOn");
  xgi::bind(this,&emu::pc::Configurator::MPCMask, "MPCMask");
  xgi::bind(this,&emu::pc::Configurator::DMBLoadFirmware, "DMBLoadFirmware");
  xgi::bind(this,&emu::pc::Configurator::DMBVmeLoadFirmware, "DMBVmeLoadFirmware");
  xgi::bind(this,&emu::pc::Configurator::DMBVmeLoadFirmwareEmergency, "DMBVmeLoadFirmwareEmergency");
  xgi::bind(this,&emu::pc::Configurator::CFEBLoadFirmware, "CFEBLoadFirmware");
  xgi::bind(this,&emu::pc::Configurator::CCBHardResetFromDMBPage, "CCBHardResetFromDMBPage");
  xgi::bind(this,&emu::pc::Configurator::CFEBReadFirmware, "CFEBReadFirmware");
  xgi::bind(this,&emu::pc::Configurator::CFEBLoadFirmwareID, "CFEBLoadFirmwareID");
  xgi::bind(this,&emu::pc::Configurator::RdVfyCFEBVirtexDMB, "RdVfyCFEBVirtexDMB");
  xgi::bind(this,&emu::pc::Configurator::RdVfyCFEBVirtexExpT, "RdVfyCFEBVirtexExpT");
  xgi::bind(this,&emu::pc::Configurator::DMBCheckConfiguration, "DMBCheckConfiguration");
  xgi::bind(this,&emu::pc::Configurator::DMBConfigure, "DMBConfigure");
  xgi::bind(this,&emu::pc::Configurator::CFEBUtils, "CFEBUtils");
  xgi::bind(this,&emu::pc::Configurator::CFEBFunction, "CFEBFunction");
  xgi::bind(this,&emu::pc::Configurator::DCFEBReadFirmware, "DCFEBReadFirmware");
  xgi::bind(this,&emu::pc::Configurator::DCFEBProgramFpga, "DCFEBProgramFpga");
  xgi::bind(this,&emu::pc::Configurator::DCFEBProgramEprom, "DCFEBProgramEprom");
  xgi::bind(this,&emu::pc::Configurator::DCFEBProgramEpromAll, "DCFEBProgramEpromAll");
  xgi::bind(this,&emu::pc::Configurator::LVMBStatus, "LVMBStatus");
  //
  //-----------------------------------------------
  // TMB tests
  //-----------------------------------------------
  //
  //-----------------------------------------------
  // TMB utilities
  //-----------------------------------------------
  xgi::bind(this,&emu::pc::Configurator::ALCTStatus, "ALCTStatus");
  xgi::bind(this,&emu::pc::Configurator::RATStatus, "RATStatus");
  xgi::bind(this,&emu::pc::Configurator::CheckCrateControllerFromTMBPage, "CheckCrateControllerFromTMBPage");
  xgi::bind(this,&emu::pc::Configurator::LoadTMBFirmware, "LoadTMBFirmware");
  xgi::bind(this,&emu::pc::Configurator::LoadCrateTMBFirmware, "LoadCrateTMBFirmware");
  xgi::bind(this,&emu::pc::Configurator::CCBHardResetFromTMBPage, "CCBHardResetFromTMBPage");
  xgi::bind(this,&emu::pc::Configurator::CheckTMBFirmware, "CheckTMBFirmware");
  xgi::bind(this,&emu::pc::Configurator::ClearTMBBootReg, "ClearTMBBootReg");
  xgi::bind(this,&emu::pc::Configurator::UnjamTMB, "UnjamTMB");  
  xgi::bind(this,&emu::pc::Configurator::CheckAbilityToLoadALCT, "CheckAbilityToLoadALCT");
  xgi::bind(this,&emu::pc::Configurator::LoadALCTFirmware, "LoadALCTFirmware");
  xgi::bind(this,&emu::pc::Configurator::LoadCrateALCTFirmware, "LoadCrateALCTFirmware");
  xgi::bind(this,&emu::pc::Configurator::LoadRATFirmware, "LoadRATFirmware");
  xgi::bind(this,&emu::pc::Configurator::EraseRATFirmware, "EraseRATFirmware");
  xgi::bind(this,&emu::pc::Configurator::TMBPrintCounters, "TMBPrintCounters");
  xgi::bind(this,&emu::pc::Configurator::TMBResetCounters, "TMBResetCounters");
  xgi::bind(this,&emu::pc::Configurator::TMBCounterForFixedTime, "TMBCounterForFixedTime");
  xgi::bind(this,&emu::pc::Configurator::CalibrationRuns, "CalibrationRuns");
  xgi::bind(this,&emu::pc::Configurator::TriggerTestInjectALCT, "TriggerTestInjectALCT");
  xgi::bind(this,&emu::pc::Configurator::TriggerTestInjectCLCT, "TriggerTestInjectCLCT");
  xgi::bind(this,&emu::pc::Configurator::armScope, "armScope");
  xgi::bind(this,&emu::pc::Configurator::forceScope, "forceScope");
  xgi::bind(this,&emu::pc::Configurator::readoutScope, "readoutScope");
  xgi::bind(this,&emu::pc::Configurator::TMBDumpAllRegisters, "TMBDumpAllRegisters");
  xgi::bind(this,&emu::pc::Configurator::TMBConfigure, "TMBConfigure");
  xgi::bind(this,&emu::pc::Configurator::TMBClearUserProms, "TMBClearUserProms");
  xgi::bind(this,&emu::pc::Configurator::TMBReadConfiguration, "TMBReadConfiguration");
  xgi::bind(this,&emu::pc::Configurator::TMBCheckConfiguration, "TMBCheckConfiguration");
  xgi::bind(this,&emu::pc::Configurator::TMBReadStateMachines, "TMBReadStateMachines");
  xgi::bind(this,&emu::pc::Configurator::TMBCheckStateMachines, "TMBCheckStateMachines");
  xgi::bind(this,&emu::pc::Configurator::TMBResetSyncError, "TMBResetSyncError");
  xgi::bind(this,&emu::pc::Configurator::TMBRawHits, "TMBRawHits");
  xgi::bind(this,&emu::pc::Configurator::ALCTRawHits, "ALCTRawHits");
  xgi::bind(this,&emu::pc::Configurator::TMBReadFirmware, "TMBReadFirmware");
  xgi::bind(this,&emu::pc::Configurator::ALCTReadFirmware, "ALCTReadFirmware");
  xgi::bind(this,&emu::pc::Configurator::LoadALCTSlowFirmware, "LoadALCTSlowFirmware");
  xgi::bind(this,&emu::pc::Configurator::LoadSpartan6ALCTFirmware, "LoadSpartan6ALCTFirmware");
  xgi::bind(this,&emu::pc::Configurator::LoadVirtex6TMBFirmware, "LoadVirtex6TMBFirmware");

  //
  //----------------------------
  // Bind logging methods
  //----------------------------
  xgi::bind(this,&emu::pc::Configurator::LogChamberTestsOutput, "LogChamberTestsOutput");
  xgi::bind(this,&emu::pc::Configurator::LogCrateTestsOutput, "LogCrateTestsOutput");
  xgi::bind(this,&emu::pc::Configurator::LogALCT_TMB_communicationOutput, "LogALCT_TMB_communicationOutput");
  xgi::bind(this,&emu::pc::Configurator::LogOutput, "LogOutput");
  xgi::bind(this,&emu::pc::Configurator::LogTestSummary, "LogTestSummary");
  xgi::bind(this,&emu::pc::Configurator::LogDMBTestsOutput, "LogDMBTestsOutput");
  xgi::bind(this,&emu::pc::Configurator::LogTMBTestsOutput, "LogTMBTestsOutput");
  //
  //----------------------------------------------
  // Bind BC0 methods
  //----------------------------------------------
  xgi::bind(this,&emu::pc::Configurator::ALCTBC0Scan, "ALCTBC0Scan");
  xgi::bind(this,&emu::pc::Configurator::ALCTBC0ScanForCrate,"ALCTBC0ScanForCrate");
  xgi::bind(this,&emu::pc::Configurator::ALCTBC0ScanForSystem,"ALCTBC0ScanForSystem");
  xgi::bind(this,&emu::pc::Configurator::Settmb_bxn_offset, "Settmb_bxn_offset");
  //
  //----------------------------------------------------
  // Bind phase determination (commmunication)  methods
  //----------------------------------------------------
  xgi::bind(this,&emu::pc::Configurator::ALCT_TMB_communication, "ALCT_TMB_communication");
  xgi::bind(this,&emu::pc::Configurator::ALCT_TMB_Loopback, "ALCT_TMB_Loopback");
  xgi::bind(this,&emu::pc::Configurator::TMB_to_ALCT_walking_ones, "TMB_to_ALCT_walking_ones");
  xgi::bind(this,&emu::pc::Configurator::CFEBTiming, "CFEBTiming");
  xgi::bind(this,&emu::pc::Configurator::CFEBTimingME11b, "CFEBTimingME11b");
  xgi::bind(this,&emu::pc::Configurator::CFEBTimingME11a, "CFEBTimingME11a");
  xgi::bind(this,&emu::pc::Configurator::RatTmbTiming, "RatTmbTiming");
  xgi::bind(this,&emu::pc::Configurator::RpcRatTiming, "RpcRatTiming");
  //
  //----------------------------------------------
  // Bind synchronization methods
  //----------------------------------------------
  xgi::bind(this,&emu::pc::Configurator::FindDistripHotChannel, "FindDistripHotChannel");
  xgi::bind(this,&emu::pc::Configurator::setupCoincidencePulsing, "setupCoincidencePulsing");
  xgi::bind(this,&emu::pc::Configurator::setTMBCounterReadValues, "setTMBCounterReadValues");
  xgi::bind(this,&emu::pc::Configurator::setDataReadValues, "setDataReadValues");
  xgi::bind(this,&emu::pc::Configurator::Automatic, "Automatic");
  //  xgi::bind(this,&emu::pc::Configurator::ALCTvpf,"ALCTvpf");
  xgi::bind(this,&emu::pc::Configurator::FindWinner, "FindWinner");
  xgi::bind(this,&emu::pc::Configurator::TmbLctCableDelay, "TmbLctCableDelay");
  xgi::bind(this,&emu::pc::Configurator::PrintDmbValuesAndScopes, "PrintDmbValuesAndScopes");
  xgi::bind(this,&emu::pc::Configurator::TMBL1aTiming, "TMBL1aTiming");
  xgi::bind(this,&emu::pc::Configurator::ALCTL1aTiming, "ALCTL1aTiming");
  xgi::bind(this,&emu::pc::Configurator::AlctDavCableDelay, "AlctDavCableDelay");
  xgi::bind(this,&emu::pc::Configurator::CfebDavCableDelay, "CfebDavCableDelay");
  xgi::bind(this,&emu::pc::Configurator::ALCTScan, "ALCTScan");
  xgi::bind(this,&emu::pc::Configurator::CFEBScan, "CFEBScan");
  //
  //----------------------------
  // Bind calibration methods
  //----------------------------
  xgi::bind(this,&emu::pc::Configurator::FindLv1aDelayComparator, "FindLv1aDelayComparator");   
  xgi::bind(this,&emu::pc::Configurator::FindLv1aDelayALCT, "FindLv1aDelayALCT");
  xgi::bind(this,&emu::pc::Configurator::CalibrationCFEBXtalk, "CalibrationCFEBXtalk");
  xgi::bind(this,&emu::pc::Configurator::CalibrationCFEBGain, "CalibrationCFEBGain");
  xgi::bind(this,&emu::pc::Configurator::CalibrationCFEBSaturation, "CalibrationSaturation");
  xgi::bind(this,&emu::pc::Configurator::CalibrationCFEBPedestal, "CalibrationCFEBPedestal");
  xgi::bind(this,&emu::pc::Configurator::CalibrationComparatorPulse, "CalibrationComparatorPulse");
  xgi::bind(this,&emu::pc::Configurator::CalibrationALCTThresholdScan, "CalibrationALCTThresholdScan");
  xgi::bind(this,&emu::pc::Configurator::CalibrationALCTConnectivity, "CalibrationALCTConnectivity");
  xgi::bind(this,&emu::pc::Configurator::CalibrationCFEBConnectivity, "CalibrationCFEBConnectivity");

  //
  //----------------------------
  // Bind monitoring methods
  //----------------------------
  xgi::bind(this,&emu::pc::Configurator::CrateDumpConfiguration, "CrateDumpConfiguration");

}

void emu::pc::Configurator::bindSOAPInterface(){
  xoap::bind(this, &emu::pc::Configurator::onSetConfiguration, "SetConfiguration", XDAQ_NS_URI );
  //
  // SOAP call-back functions, which relays to *Action method.
  //-----------------------------------------------------------
  //
  // xoap::bind(this,&emu::pc::Configurator::ReadAllVmePromUserid ,"ReadVmePromUserid" ,XDAQ_NS_URI);
  // xoap::bind(this,&emu::pc::Configurator::LoadAllVmePromUserid ,"LoadVmePromUserid" ,XDAQ_NS_URI);
  // xoap::bind(this,&emu::pc::Configurator::ReadAllCfebPromUserid,"ReadCfebPromUserid",XDAQ_NS_URI);
  // xoap::bind(this,&emu::pc::Configurator::LoadAllCfebPromUserid,"LoadCfebPromUserid",XDAQ_NS_URI);
}

xoap::MessageReference emu::pc::Configurator::onSetConfiguration( xoap::MessageReference message )
  //  throw (xoap::exception::Exception)
{
  xoap::MessageReference reply = xoap::createMessage();
  try{
    xdata::String configurationXML;
    emu::soap::extractParameters( message, emu::soap::Parameters().add( "configurationXML", &configurationXML ) );
    LOG4CPLUS_INFO( logger_, "Configuration XML length: " << configurationXML.toString().size() );
    selectedConfigIds_.clear();
    selectedConfigIds_.push_back( configurationXML );
    selectedSource_ = emu::pc::Configuration::stringSource;
  }
  catch( xcept::Exception& e ){
    XCEPT_RETHROW( xoap::exception::Exception, "Failed to set configuration from SOAP.", e );
  }
  return reply;
}

void emu::pc::Configurator::htmlHeader( xgi::Input *in, xgi::Output *out, const string& title, const int refreshPeriod ){
  if ( refreshPeriod >= 0 ) out->getHTTPResponseHeader().addHeader( "Refresh", emu::utils::stringFrom<int>( refreshPeriod ) );
  *out << "<!DOCTYPE html>" << endl; // HTML5
  *out << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << endl;
  *out << "  <head>" << endl;
  *out << "    <meta http-equiv=\"Content-Type\" content=\"text/html << endl; charset=UTF-8\"/>" << endl;
  *out << "    <title>" << title << "</title>" << endl;
  *out << "    <link rel=\"stylesheet\" type=\"text/css\" href=\"/emu/emuDCS/PeripheralApps/html/pc.css\"></link>" << endl;
  *out << "    <script type=\"text/javascript\" src=\"/emu/emuDCS/PeripheralApps/html/pc.js\"></script>" << endl;
  *out << "  </head>" << endl;
  *out << "  <body>" << endl;
}

void emu::pc::Configurator::htmlFooter( xgi::Input *in, xgi::Output *out ){
  *out << "  </body>" << endl;
  *out << "</html>" << endl;
}

void emu::pc::Configurator::pageHeader( xgi::Input *in, xgi::Output *out, const emu::pc::AppNavigator& appNav ){
  stringstream header;
  header << "<table><tr><th colspan=\"2\"><span class=\"title\">Emu Peripheral Crate Configurator " 
	 << getApplicationDescriptor()->getInstance() << " [" 
	 << ( bool( isSlave_ ) ? "slave" : "master" )
	 << "]</span></th></tr><tr>";
  if ( configuration_.getSource() == emu::pc::Configuration::stringSource ){
    header << "<td>Configuration obtained from the master instance.</td>";
  }
  else if( configuration_.getSource() == emu::pc::Configuration::fileSource ){
    header << "<td>Configured with <span style=\"font-weight:bold;\">" 
	   << emu::utils::csvFrom<set<string> >( configuration_.getIds() )
	   << "</span> obtained at " << configuration_.getCreationDateTime() << "</td>";
  }
  else if( configuration_.getSource() == emu::pc::Configuration::dbSource ){
    header << "<td>configured with DB id(s) <span style=\"font-weight:bold;\">" 
	   << emu::utils::csvFrom<set<string> >( configuration_.getIds() )
	   << "</span> obtained at " << configuration_.getCreationDateTime() << "</td>";
  }
  else{
    header << "<td class=\"warning\">No configuration selected yet.</td>";
  }
  header << "<td style=\"text-align:right;\">Page at " << emu::utils::getDateTime() << "</td></tr>"
	 << "<tr><td colspan=\"2\">" << appNav.getHTML() << "</td></tr></table>" << endl;
  
  *out << "<span class=\"header\">" << endl << header.str() << endl << "</span>" << endl;
  // Repeat, this time just as an invisible space holder to prevent the page from sliding under the header
  *out << "<span class=\"space\" >" << endl << header.str() << endl << "</span><br/>" << endl;
}

void emu::pc::Configurator::pageFooter( xgi::Input *in, xgi::Output *out ){
  *out << "  <hr/>" << endl;
  *out << "  <p class=\"footer\">Emu Peripheral Apps version " << EmuPeripheralApps::versions << "</p>" << endl;
}

string emu::pc::Configurator::configTimeStampHTML(){
  // This is to be included in every HTML form. 
  // The time stamp will be used to identify commands sent from stale web pages.
  return "<input type=\"hidden\" name=\"configTimeStamp\" value=\"" + configuration_.getCreationDateTime() + "\"/>\n";
}

string emu::pc::Configurator::disableInSlave(){
  return ( bool( isSlave_ ) ? " disabled=\"disabled\" " : "" );
}

bool emu::pc::Configurator::hasCurrentConfigTimeStamp( xgi::Input *in ){

  cgicc::Cgicc cgi( in );
  if ( (*cgi).size() == 0 ){
    // No command, no problem.
    // cout  << endl << endl << "hasCurrentConfigTimeStamp no command: " << webPageMessage_ << endl << endl;
    return true;
  }
  cgicc::const_form_iterator cfi( cgi.getElement( "configTimeStamp" ) );
  if( cfi != (*cgi).end() ){
    string commandsConfigTimeStamp( cfi->getValue() );
    // cout << endl << "request: " << commandsConfigTimeStamp
    // 	 << endl << "current: " << configuration_.getCreationDateTime()
    // 	 << endl;
    if ( commandsConfigTimeStamp == configuration_.getCreationDateTime() ){
      // cout  << endl << endl << "hasCurrentConfigTimeStamp true: " << webPageMessage_ << endl << endl;
      return true;
    }
    else if ( configuration_.getCreationDateTime().size() > 0 ){
      webPageMessage_ = "<div class=\"warning\">Refused to execute command issued from a stale web page of configuration time stamp <span style=\"font-weight:bold\">"
	+ commandsConfigTimeStamp 
	+ "</span>. The time stamp of the current configuration is <span style=\"font-weight:bold\">"
	+ configuration_.getCreationDateTime()
	+ "</span>.</div>\n";
    }
  }
  else{
    webPageMessage_ = "<div class=\"warning\">Command refused as it did not specify the configuration's time stamp.</div>\n";
  }

  // cout  << endl << endl << "hasCurrentConfigTimeStamp false: " << webPageMessage_ << endl << endl;

  return false;
}

void emu::pc::Configurator::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception){
  emu::utils::redirectTo( "/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage", out );
  cout << "Redirecting to /" << getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" << endl;
}

void emu::pc::Configurator::ConfigSelectorPage( xgi::Input * in, xgi::Output * out ){

  cgicc::Cgicc cgi( in );
  cgicc::const_form_iterator cfi1, cfi2;

  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)" );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector" )
	      .addLink( "Main Page", "/" + getApplicationDescriptor()->getURN() + "/MainPage" ) );

  // Slaves don't select their configuration but get it from their master:
  if ( bool( isSlave_ ) ){
    pageFooter( in, out );
    htmlFooter( in, out );
    emu::utils::redirectToSelf( in, out );
    return;
  }

  *out << webPageMessage_; // empty if no message to be displayed

  if ( (*cgi).size() > 0 && hasCurrentConfigTimeStamp( in ) ){

    //
    // Get user's selection
    //
    if( cgi.getElement( "configFromFile" ) != (*cgi).end() ){
      cfi1 = cgi.getElement( "xmlfile" );
      if( cfi1 != (*cgi).end() && ! cfi1->isEmpty() ){
	Valid_config_ID = cfi1->getValue();
	selectedConfigIds_.clear();
	selectedConfigIds_.push_back( cfi1->getValue() );
	selectedSource_ = emu::pc::Configuration::fileSource;
	webPageMessage_.clear();
	fireEvent( "Configure" );
	// fireEvent() won't throw but goes to Failed state instead. Let's get out of it:
	if ( fsm_.getCurrentState() == 'F' ) fireEvent( "Reset" );
      }
      else{
	webPageMessage_ = "<div class=\"warning\">No configuration file specified.</div>\n";
      }
    }
    else if( cgi.getElement( "configPlusSideFromDB" ) != (*cgi).end() ){
      cfi1 = cgi.getElement( "plusids" );
      if( cfi1 != (*cgi).end() && ! cfi1->isEmpty() ){
	Valid_config_ID = cfi1->getValue();
	selectedConfigIds_.clear();
	selectedConfigIds_.push_back( cfi1->getValue() );
	selectedSource_ = emu::pc::Configuration::dbSource;
	webPageMessage_.clear();
	fireEvent( "Configure" );
      }
      else if ( cfi1->isEmpty() ){
	webPageMessage_ = "<div class=\"warning\">No database configuration id specified for the plus side.</div>\n";
      }
    }
    else if( cgi.getElement( "configMinusSideFromDB" ) != (*cgi).end() ){
      cfi1 = cgi.getElement( "minusids" );
      if( cfi1 != (*cgi).end() && ! cfi1->isEmpty() ){
	Valid_config_ID = cfi1->getValue();
	selectedConfigIds_.clear();
	selectedConfigIds_.push_back( cfi1->getValue() );
	selectedSource_ = emu::pc::Configuration::dbSource;
	webPageMessage_.clear();
	fireEvent( "Configure" );
      }
      else if ( cfi1->isEmpty() ){
	webPageMessage_ = "<div class=\"warning\">No database configuration id specified for the minus side.</div>\n";
      }
    }
    else if( cgi.getElement( "configBothSidesFromDB" ) != (*cgi).end() ){
      cfi1 = cgi.getElement( "plusids"  );
      cfi2 = cgi.getElement( "minusids" );
      if( cfi1 != (*cgi).end() && ! cfi1->isEmpty() && 
	  cfi2 != (*cgi).end() && ! cfi2->isEmpty()    ){
	Valid_config_ID = cfi1->getValue();
	selectedConfigIds_.clear();
	selectedConfigIds_.push_back( cfi1->getValue() );
	selectedConfigIds_.push_back( cfi2->getValue() );
	selectedSource_ = emu::pc::Configuration::dbSource;
	webPageMessage_.clear();
	fireEvent( "Configure" );
      }
      else if ( cfi1->isEmpty() || cfi2->isEmpty() ){
	webPageMessage_ = "<div class=\"warning\">Less than two database configuration ids specified for the two sides.</div>\n";
      }
    }

  } // if ( (*cgi).size() > 0 && hasCurrentConfigTimeStamp( in ) )
  
  *out << "<form>\n" << configTimeStampHTML(); // to be included in every form

  //
  // Get list of files
  //
  cout << "xmlFile " << xmlFile_.toString() << endl;
  xmlFile_.fromString( emu::utils::performExpansions( xmlFile_ ) );
  string::size_type p = xmlFile_.toString().find_last_of( "/" );
  string xmlDir;
  if ( p != string::npos ){
    xmlDir = xmlFile_.toString().substr( 0, xmlFile_.toString().find_last_of( "/" ) );
  }
  else{
    *out << "<div class=\"warning\">Failed to get directory from XML file name '" << xmlFile_.toString() << "'</div>" << endl;
  }
  vector<string> xmlFiles = emu::utils::execShellCommand( "ls -1 " + xmlDir + "/*.xml" );
  
  //
  // Present the list of files to choose from
  //
  *out << "<select name=\"xmlfile\" size=\"10\">" << endl;
  for ( vector<string>::const_iterator f = xmlFiles.begin(); f != xmlFiles.end(); ++f ){
    *out << "<option value=\"" << *f << "\"";
    // Highlight the file specified in the app's config XML if no other file has been selected yet:
    if ( selectedSource_ != emu::pc::Configuration::fileSource && 
	 *f == xmlFile_.toString()                                ) *out << " selected=\"selected\"";
    // Highlight the previously selected file, if any:
    if ( selectedSource_ == emu::pc::Configuration::fileSource &&
	 find( selectedConfigIds_.begin(), 
	       selectedConfigIds_.end()  ,
	       *f                          ) != selectedConfigIds_.end() ) *out << " selected=\"selected\"";
    *out << ">" << *f << "</option>";
  }
  *out << "</select>" << endl;
  *out << "<button type=\"submit\" name=\"configFromFile\">Configure from file</button>" << endl;

  //
  // Get and present the list of DB ids to choose from
  //
  try{
    vector<string> plusSideDBIds = configuration_.getAvailableDBIds( emu::pc::Configuration::plusSide );
    *out << "<select name=\"plusids\" size=\"10\">" << endl;
    for ( vector<string>::const_iterator k = plusSideDBIds.begin(); k != plusSideDBIds.end(); ++k ){
      *out << "<option value=\"" << *k << "\"";
      // Highlight the id specified in the app's config XML if no other id has been selected yet:
      if ( selectedSource_ != emu::pc::Configuration::dbSource && 
	 *k == EMU_config_ID_.toString()                          ) *out << " selected=\"selected\"";
      // Highlight the previously selected id, if any:
      if ( selectedSource_ == emu::pc::Configuration::dbSource &&
	 find( selectedConfigIds_.begin(), 
	       selectedConfigIds_.end()  ,
	       *k                          ) != selectedConfigIds_.end() ) *out << " selected=\"selected\"";
      *out << ">" << *k << "</option>";
    }
    *out << "</select>" << endl;
    *out << "<button type=\"submit\" name=\"configPlusSideFromDB\">Configure plus side from DB</button>" << endl;
    
    vector<string> minusSideDBIds = configuration_.getAvailableDBIds( emu::pc::Configuration::minusSide );
    *out << "<select name=\"minusids\" size=\"10\">" << endl;
    for ( vector<string>::const_iterator k = minusSideDBIds.begin(); k != minusSideDBIds.end(); ++k ){
      *out << "<option value=\"" << *k << "\"";
      // Highlight the id specified in the app's config XML if no other id has been selected yet:
      if ( selectedSource_ != emu::pc::Configuration::dbSource && 
	 *k == EMU_config_ID_.toString()                          ) *out << " selected=\"selected\"";
      // Highlight the previously selected id, if any:
      if ( selectedSource_ == emu::pc::Configuration::dbSource && 
	 find( selectedConfigIds_.begin(), 
	       selectedConfigIds_.end()  ,
	       *k                          ) != selectedConfigIds_.end() ) *out << " selected=\"selected\"";
      *out << ">" << *k << "</option>";
    }
    *out << "</select>" << endl;
    *out << "<button type=\"submit\" name=\"configPlusSideFromDB\">Configure minus side from DB</button>" << endl;

    *out << "<button type=\"submit\" name=\"configBothSidesFromDB\">Configure both sides from DB</button>" << endl;
  }
  catch( xcept::Exception& e ){
    stringstream ss;
    *out << "<div class=\"warning\" title=\"" << xcept::stdformat_exception_history(e) << "\">Failed to obtain configuration ids from DB.</div>";
    ss <<  "Failed to obtain configuration ids from DB. " << xcept::stdformat_exception_history(e);
    LOG4CPLUS_WARN( logger_, ss.str() );
  }
  catch( std::exception& e ){
    stringstream ss;
    *out << "<div class=\"warning\" title=\"" << e.what() << "\">Failed to obtain configuration ids from DB.</div>";
    ss <<  "Failed to obtain configuration ids from DB. " << e.what();
    LOG4CPLUS_WARN( logger_, ss.str() );
  }
  catch(...){
    stringstream ss;
    *out << "<div class=\"warning\" title=\"Due to an unknown exception.\">Failed to obtain configuration ids from DB.</div>";
    ss <<  "Failed to obtain configuration ids from DB. Unknown exception.";
    LOG4CPLUS_WARN( logger_, ss.str() );
  }

  *out << "</form>" << endl;

  pageFooter( in, out );
  htmlFooter( in, out );

  emu::utils::redirectToSelf( in, out );
}

void emu::pc::Configurator::BusyPage(xgi::Input * in, xgi::Output * out ){
  if ( ! bool( isSlave_) ){

    //
    // Check for Stop command
    //
    cgicc::Cgicc cgi( in );
    cgicc::const_form_iterator cfi1, cfi2;   
    if ( (*cgi).size() > 0 && hasCurrentConfigTimeStamp( in ) ){
      if( cgi.getElement( "Stop" ) != (*cgi).end() ){
	fireEvent( "Stop" );
	webPageMessage_.clear();
      }
    }

    *out << "<div class=\"title\">Executing " << currentTaskName_.toString() << "...</div><br/>" << endl;

    //
    // Place Stop button
    //
    *out << "<form>\n" 
	 <<    configTimeStampHTML() // to be included in every form
	 << "  <button type=\"submit\" name=\"Stop\">Stop</button>\n"
	 << "</form><br/>\n";


    if ( bool( isDistributed_ ) ){
      //
      // Display every slave's progress
      //
      *out << "<table class=\"progress\"><tr><th>Crate Ids</th><th class=\"progress\">Progress</th></tr>" << endl;
      map<xdaq::ApplicationDescriptor*,VMEAccessor::Status_t>::iterator ss;
      for ( ss = slavesStatus_.begin(); ss != slavesStatus_.end(); ++ss ){
	ostringstream oss;
	if ( ss->second.exception.message().size() > 0 ){
	  oss << xhtmlformat_exception_history( ss->second.exception );
	}
	else if ( ss->second.reasonForFailure.size() > 0 ){
	  oss << ss->second.reasonForFailure;
	}
	else{
	  oss << "<div class=\"progressbox\"><div class=\"progressbar\" style=\"width:" 
	      << ss->second.progressPercent 
	      << "%;\"><div style=\"text-align:center;\">" 
	      << ss->second.progressPercent 
	      << "%</div></div></div>";
	}
	*out << "<tr><td><a href=\"" << ss->first->getContextDescriptor()->getURL()
	     << "/" << ss->first->getURN()
	     << "\" target=\"_blank\">" << ss->second.crateIdsCSV 
	     << "</a></td><td class=\"progress\">" << oss.str() 
	     << "</td></tr>";
      }
      *out << "</table>\n";
    } // if ( bool( isDistributed_ ) )
    else{
      //
      // Display master's (that is, our own) progress
      //
      *out << "<table class=\"progress\"><tr><th class=\"progress\">Progress</th></tr>" << endl;
      ostringstream oss;
      if ( workLoopsCatch_.message().size() > 0 ){
	oss << xhtmlformat_exception_history( workLoopsCatch_ );
      }
      else if ( reasonForFailure_.toString().size() > 0 ){
	oss << reasonForFailure_.toString();
      }
      else{
	oss << "<div class=\"progressbox\"><div class=\"progressbar\" style=\"width:" 
	    << progressPercent_ 
	    << "%;\"><div style=\"text-align:center;\">" 
	    << progressPercent_
	    << "%</div></div></div>";
      }
      *out << "<tr><td class=\"progress\">" << oss.str() << "</td></tr>"
	   << "</table>\n";
    }// if ( bool( isDistributed_ ) ) else
  }
}

void emu::pc::Configurator::MainPage(xgi::Input * in, xgi::Output * out ){
  // perform possible environment variable expansions in the configuration file name:
  xmlFile_.fromString(emu::utils::performExpansions(xmlFile_));

  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)", ( fsm_.getCurrentState() == 'E' ? 5 : -1 ) );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector", 
			"/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" )
	      .addLink( "Main Page" ) );

  if ( fsm_.getCurrentState() == 'E' ){
    BusyPage( in, out );
    pageFooter( in, out );
    htmlFooter( in, out );
    emu::utils::redirectToSelf( in, out );
    return;
  }

  *out << webPageMessage_;

  cgicc::Cgicc cgi( in );
  cgicc::const_form_iterator cfi1, cfi2;
  // cout << "(*cgi).size() " << (*cgi).size() << endl
  //      << "hasCurrentConfigTimeStamp( in ) " << hasCurrentConfigTimeStamp( in ) << endl;
  if ( (*cgi).size() > 0 && hasCurrentConfigTimeStamp( in ) ){
    
    //
    // Get user's command
    //
    if ( cgi.getElement( "ConfigAllCrates" ) != (*cgi).end() ){
      currentTaskName_ = "ConfigAllCrates";
      isDistributed_ = true;
      fireEvent( "Enable" );
      webPageMessage_.clear();
      try{
	configuration_.recordTimeOfWritingFlash();
      }
      catch( xcept::Exception& e ){
	if ( webPageMessage_.size() == 0 ) webPageMessage_ += "<div class=\"warning\" title=\"" 
					     + xcept::stdformat_exception_history(e) + "\">";
	webPageMessage_ += "Failed to record in db the time of writing FLASH";
      }
      if ( webPageMessage_.size() > 0 ) webPageMessage_ += "</div>";
    }
    else if ( cgi.getElement( "CheckControllers" ) != (*cgi).end() ){
      webPageMessage_.clear();
      for(unsigned i=0; i< crateVector.size(); i++){
	try{
	  crateVector[i]->CheckController();
	}
	catch( std::exception& e ){
	  if ( webPageMessage_.size() == 0 ) webPageMessage_ += "<div class=\"warning\">";
	  webPageMessage_ += "Failed to check controller of crate " + crateVector[i]->GetLabel() + " : " + e.what() + "<br/>";
	}
      }
      if ( webPageMessage_.size() > 0 ) webPageMessage_ += "</div>";
    }
    else if ( cgi.getElement( "FastConfigCrates" ) != (*cgi).end() ){
      currentTaskName_ = "FastConfigCrates";
      isDistributed_ = false;
      fireEvent( "Enable" );
      webPageMessage_.clear();
    }
    else if ( cgi.getElement( "CrateSelection" ) != (*cgi).end() ){
      if ( cgi.getElement( "crateName" ) != (*cgi).end() ){
	string in_value = cgi.getElement("crateName")->getValue();
	if(!in_value.empty()){
	  int k=in_value.find(" ",0);
	  std::string value = (k) ? in_value.substr(0,k):in_value;
	  ThisCrateID_=value;
	  for(unsigned i=0; i< crateVector.size(); i++){
	    if(value==crateVector[i]->GetLabel()) this_crate_no_=i;
	  }
	  SetCurrentCrate(this_crate_no_);
	}
      }
    }
    else if ( cgi.getElement( "FastConfigOne" ) != (*cgi).end() ){
      webPageMessage_.clear();
      try{
	if ( standalone_ ) thisCrate->configure(2);
      }
      catch( std::exception& e ){
	if ( webPageMessage_.size() == 0 ) webPageMessage_ += "<div class=\"warning\">";
	webPageMessage_ += "Failed to power-up init crate " + thisCrate->GetLabel() + " : " + e.what() + "<br/>";
	if ( webPageMessage_.size() > 0 ) webPageMessage_ += "</div>";
      }
    }
    else if ( cgi.getElement( "ConfigOneCrate" ) != (*cgi).end() ){
      webPageMessage_.clear();
      try{
	thisCrate->configure(0);
      }
      catch( std::exception& e ){
	if ( webPageMessage_.size() == 0 ) webPageMessage_ += "<div class=\"warning\">";
	webPageMessage_ += "Failed to configure crate " + thisCrate->GetLabel() + " : " + e.what() + "<br/>";
	if ( webPageMessage_.size() > 0 ) webPageMessage_ += "</div>";
      }
    }
    else if ( cgi.getElement( "CrateDumpConfiguration" ) != (*cgi).end() ){
      webPageMessage_.clear();
      try{
	thisCrate->DumpConfiguration();
      }
      catch( std::exception& e ){
	if ( webPageMessage_.size() == 0 ) webPageMessage_ += "<div class=\"warning\">";
	webPageMessage_ += "Failed to dump configuration of crate " + thisCrate->GetLabel() + " : " + e.what() + "<br/>";
	if ( webPageMessage_.size() > 0 ) webPageMessage_ += "</div>";
      }
    }
    else if ( cgi.getElement( "CheckCrateConfiguration" ) != (*cgi).end() ){
      webPageMessage_.clear();
      try{
	CheckPeripheralCrateConfiguration();
      }
      catch( std::exception& e ){
	if ( webPageMessage_.size() == 0 ) webPageMessage_ += "<div class=\"warning\">";
	webPageMessage_ += "Failed to check configuration of crate " + thisCrate->GetLabel() + " : " + e.what() + "<br/>";
	if ( webPageMessage_.size() > 0 ) webPageMessage_ += "</div>";
      }
    }
 }

  *out << "<table>\n"
       << "  <tr><td>Total Crates: </td><td>" << total_crates_ << "</td></tr>\n";
  unsigned int active_crates=0;
  for(unsigned i=0; i<crateVector.size(); i++)
     if(crateVector[i]->IsAlive()) active_crates++;
  if( active_crates <= total_crates_)
    *out << "  <tr><th>Active Crates: </th><td>" << active_crates << "</td></tr>\n";
  *out << "  </table>\n";


  *out << "<h2>System Utilities</h2>\n";


  *out << "<table><tr>\n";

  *out << "<td>\n"
       << "  <form>\n" 
       <<      configTimeStampHTML() // to be included in every form
       << "    <button type=\"submit\" name=\"CheckControllers\"" << disableInSlave() << ">Check Crate Controllers</button>\n"
       << "  </form>\n"
       << "</td>\n";

  // for non-standalone mode, use the one in Service (Blue) instead
  if(standalone_) 
  {
    *out << "<td>\n"
	 << "  <form>\n" 
	 <<      configTimeStampHTML() // to be included in every form
	 << "    <button type=\"submit\" name=\"FastConfigCrates\"" << disableInSlave() << ">Crates Power-up Init</button>\n"
	 << "  </form>\n"
	 << "</td>\n";
  }

  *out << "<td>\n"
       << "  <a href=\"/" << getApplicationDescriptor()->getURN() << "/PowerOnFixCFEB\">[Firmware Check]</a>\n"
       << "</td>\n";

  *out << "<td>\n"
       << "  <a href=\"/" << getApplicationDescriptor()->getURN() << "/CheckConfigurationPage\">[Configuration Check]</a>\n"
       << "</td>\n";

  *out << "<td>\n"
       << "  <a href=\"/" << getApplicationDescriptor()->getURN() << "/ExpertToolsPage\">[Expert Tools Page]</a>\n"
       << "</td>\n";

  *out << "</tr></table>\n";

  *out << "<br/>\n";


  *out << "<form>\n" 
       <<    configTimeStampHTML() // to be included in every form
       << "  <button type=\"submit\" name=\"ConfigAllCrates\"" << disableInSlave() << ">Write FLASH All Crates</button>\n"
       << "</form>\n";


  *out << "<br/><hr/>\n"
       << "<h2>Crate Utilities</h2>\n"
       << "<span style=\"color:blue; font-weight: bold;\">Current Crate : "
       << ThisCrateID_
       << "</span>\n<br/>\n";

  // Begin select crate
  *out << "<form>\n" 
       <<    configTimeStampHTML() // to be included in every form
       <<    "Choose Crate: \n"
       << "  <select name=\"crateName\">\n";
  for (int i = 0; i < int( crateVector.size() ); ++i){
    string CrateName = crateVector[i]->GetLabel();
    if( ! crateVector[i]->IsAlive() ) CrateName += " NG";
    *out << "    <option value=\"" << CrateName << "\""
	 << ( i == this_crate_no_ ? " selected=\"selected\"" : "" ) <<  "\">"
	 << CrateName << "</option>\n";
  }
  *out << "  </select>\n"
       << "  <button type=\"submit\" name=\"CrateSelection\"" << disableInSlave() << ">Select</button>\n"
       << "</form>\n";
  //End select crate

 
  *out << "<br/>\n";
  std::cout << "Main Page: "<< std::dec << active_crates << "/" <<total_crates_ << " Crates" << std::endl;
  //
  if (tmbVector.size()>0 || dmbVector.size()>0) {
    //
    // Crate Configuration
    //
    *out << "<a href=\"/" << getApplicationDescriptor()->getURN() << "/CrateConfiguration\">[Crate Configuration]</a>\n"
	 << "<a href=\"/" << getApplicationDescriptor()->getURN() << "/CrateTests\">[Crate Tests]</a>\n"
	 << "<a href=\"/" << getApplicationDescriptor()->getURN() << "/CalibrationRuns\">[Calibration Runs]</a>\n"
	 << "<br/><br/>\n";

    *out << "<table>\n"
	 << "  <tr>\n";

    // for non-standalone mode, use the one in Service (Blue) instead
    if(standalone_)
    {
      *out << "<td>"
	   << "  <form>\n" 
	   <<      configTimeStampHTML() // to be included in every form
	   << "    <button type=\"submit\" name=\"FastConfigOne\"" << disableInSlave() << ">Power-up Init</button>\n"
	   << "  </form>\n"
	   << "</td>";
    }

    // enable "Write FLASH" in certain cases
    // if(standalone_ || (xml_or_db==0) || (Valid_config_ID==InFlash_config_ID))
    if ( standalone_ || configuration_.getSource() == emu::pc::Configuration::fileSource )
    {
      *out << "<td>"
	   << "  <form>\n" 
	   <<      configTimeStampHTML() // to be included in every form
	   << "    <button type=\"submit\" name=\"ConfigOneCrate\"" << disableInSlave() << ">Write FLASH to Crate</button>\n"
	   << "  </form>\n"
	   << "</td>";
    }

    *out << "<td>"
	 << "  <form>\n" 
	 <<    configTimeStampHTML() // to be included in every form
	 << "    <button type=\"submit\" name=\"CrateDumpConfiguration\"" << disableInSlave() << ">Dump Crate Info</button>\n"
	 << "  </form>\n"
	 << "</td>";

    *out << "<td>"
	 << "  <form>\n" 
	 <<      configTimeStampHTML() // to be included in every form
	 << "    <button type=\"submit\" name=\"CheckCrateConfiguration\"" << disableInSlave();
    if      (crate_check_ok[current_crate_] ==  1) *out << " style=\"color:green;\"";
    else if (crate_check_ok[current_crate_] ==  0) *out << " style=\"color:red\"";
    else if (crate_check_ok[current_crate_] == -1) *out << " style=\"color:blue\"";
    *out <<      ">Check crate configuration</button>\n"
	 << "  </form>\n"
	 << "</td>";

    *out << "  <tr>\n";
    *out << "</table>\n";

    if (crate_check_ok[current_crate_] == 0) {
      bool alct_ok = true;
      bool tmb_ok = true;
      bool dmb_ok = true;
      //
      for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
    	if (alct_check_ok[current_crate_][chamber_index] > 0) alct_ok = false;
    	if (tmb_check_ok [current_crate_][chamber_index] > 0)  tmb_ok = false;
    	if (dmb_check_ok [current_crate_][chamber_index] > 0)  dmb_ok = false;
      }
      *out << "<span style=\"color:green;\">";
      if (!alct_ok) {
	*out << "</span>\n"
	     << "<span style=\"color:red;\">";
    	*out << "ALCT: " ;
    	for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
    	  if (alct_check_ok[current_crate_][chamber_index] > 0) {
    	    *out << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() << ", ";
    	  }
    	}
    	*out << "... not OK<br/>\n";
      } else {
    	*out << "ALCT OK<br/>\n";
      }
      *out << "</span>\n"
	   << "<span style=\"color:green;\">";
      if (!tmb_ok) {
	*out << "</span>\n"
	     << "<span style=\"color:red;\">";
    	*out << "TMB: " ;
    	for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
    	  if (tmb_check_ok[current_crate_][chamber_index] > 0) {
    	    *out << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() << ", ";
    	  }
    	}
    	*out << "... not OK<br/>\n";
      } else {
    	*out << "TMB OK<br/>\n";
      }
      *out << "</span>\n"
	   << "<span style=\"color:green;\">";
      if (!dmb_ok) {
	*out << "</span>\n"
	     << "<span style=\"color:red;\">";
    	*out << "DMB: " ;
    	for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
    	  if (dmb_check_ok[current_crate_][chamber_index] > 0) {
    	    *out << thisCrate->GetChamber(tmbVector[chamber_index]->slot())->GetLabel().c_str() << ", ";
    	  }
    	}
    	*out << "... not OK<br/>\n";
      } else {
    	*out << "DMB OK<br/>\n";
      }
      *out << "</span>\n";
    }

    *out << "<br/><hr/>\n";

    *out << "<fieldset style=\"font-size: 11pt; font-family: arial; background-color:yellow\">\n"
	 << "  <legend style=\"color:blue;\">Logging</legend>"
	 << "  <form>\n" 
	 <<       configTimeStampHTML() // to be included in every form
	 << "     <button type=\"submit\" name=\"LogOutput\"" << disableInSlave() << ">Log all output</button>\n"
	 << "     <button type=\"submit\" name=\"LogTestSummary\"" << disableInSlave() << ">Log Test Summary</button>\n"
	 << "  </form>\n"
	 << "</fieldset>\n";
  }

  // *out << cgicc::br() << cgicc::br() << std::endl; 
  // if(xml_or_db==0)
  // {
  //   *out << cgicc::b(cgicc::i("Configuration filename : ")) ;
  //   *out << xmlFile_.toString() << cgicc::br() << std::endl ;
  //   //
  //   std::string DefineConfiguration = toolbox::toString("/%s/DefineConfiguration",getApplicationDescriptor()->getURN().c_str());
  //   *out << cgicc::a("[Change Configuration File]").set("href",DefineConfiguration) << std::endl;
  // }
  // else if(xml_or_db==1)
  // {
  //   *out << cgicc::b(cgicc::i("TStore EMU_config_ID : ")) ;
  //   *out << Valid_config_ID << cgicc::br() << std::endl ;
  //   if(InFlash_config_ID!="") *out << "(Currently in FLASH is : " << InFlash_config_ID << " )" << std::endl;
  // }
  // *out << cgicc::br();
  // //
  pageFooter( in, out );
  htmlFooter( in, out );
  emu::utils::redirectToSelf( in, out );
}

  void emu::pc::Configurator::SetCurrentCrate( const int cr )
  {  
    if(total_crates_<=0) return;
    thisCrate = crateVector[cr];

    if ( ! thisCrate ) {
      std::cout << "Crate doesn't exist" << std::endl;
      assert(thisCrate);
    }
    
    ThisCrateID_=thisCrate->GetLabel();
    thisCCB = thisCrate->ccb();
    thisMPC = thisCrate->mpc();
    tmbVector = thisCrate->tmbs();
    dmbVector = thisCrate->daqmbs();
    chamberVector = thisCrate->chambers();
    dduVector = thisCrate->ddus();
    //  
    tmbTestVector = InitTMBTests(thisCrate);
    //
    for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
      MyTest[i][cr].SetTMB(tmbVector[i]);
      MyTest[i][cr].SetDMB(dmbVector[i]);
      MyTest[i][cr].SetCCB(thisCCB);
      MyTest[i][cr].SetMPC(thisMPC);
      // reset ALCT firmware check value for this crate
      able_to_load_alct[i] = -1;
    }
    //
    DefineFirmwareFilenames();
    //
    current_crate_ = cr;
  }

void emu::pc::Configurator::ConfigureInit( int mode ){
  if( ! configuration_.isSet() ) return;
  progress_.setTotal( crateVector.size() ).setCurrent( 0 );
  for(size_t i=0; i<crateVector.size(); i++){
    if(crateVector[i] && crateVector[i]->IsAlive()){
      try{
	crateVector[i]->configure( mode );
      }
      catch( std::exception& e ){
	XCEPT_DECLARE( xcept::Exception, xe, string( "Failed to configure crate " ) + crateVector[i]->GetLabel() + " : " + e.what() );
	bsem_.take();
	workLoopsCatch_ = xe;
	bsem_.give();
      }
    }
    progress_.setCurrent( i+1 );
    bsem_.take();
    progressPercent_ = progress_.getPercent();
    bsem_.give();
    if ( isToStop_ ) return;
  }
}

// This one came from CrateUtils class which no longer exist. 
// Better put into another class. Leave it here for now. 
// Liu Dec.25, 2007
std::vector<emu::pc::TMBTester> emu::pc::Configurator::InitTMBTests(Crate *MyCrate_)
{
  std::vector<TMB *>       tmbVector = MyCrate_->tmbs();
  CCB * MyCCB_ = MyCrate_->ccb();
  std::vector<emu::pc::TMBTester>   result;
  //
  for( unsigned i=0; i< tmbVector.size(); i++) {
    //
    emu::pc::TMBTester tmp;
    tmp.setTMB(tmbVector[i]);
    tmp.setCCB(MyCCB_);
    tmp.setRAT(tmbVector[i]->getRAT());
    result.push_back(tmp);
    //
  }
  //
  return result;
}


void emu::pc::Configurator::actionPerformed( xdata::Event& received )
{
  // implementation of virtual method of class xdata::ActionListener

  xdata::ItemEvent& e = dynamic_cast<xdata::ItemEvent&>(received);
  
  LOG4CPLUS_DEBUG(logger_, 
		 "Received an InfoSpace event" <<
		 " Event type: " << e.type() <<
		 " Event name: " << e.itemName() <<
		 " Serializable: " << std::hex << e.item() << std::dec <<
		 " Type of serializable: " << e.item()->type() );
  
  if ( e.itemName() == "EmuConfigurationID" && e.type() == "ItemChangedEvent" ){
    cout << "EmuConfigurationID "  << EMU_config_ID_.toString() << endl;
  }
  else if ( e.itemName() == "tStoreViewFile" && e.type() == "ItemChangedEvent" ){
    cout << "tStoreViewFile "  << tStoreViewFile_.toString() << endl;
    configuration_.setTStoreViewFile( tStoreViewFile_.toString() );
  }
  else if ( e.itemName() == "XMLorDB" && e.type() == "ItemChangedEvent" ){
    cout << "XMLorDB "  << XML_or_DB_.toString() << endl;
  }
  else if ( e.itemName() == "isStandalone" && e.type() == "ItemChangedEvent" ){
    standalone_ = bool( isStandalone_);
    cout << "isStandalone "  << isStandalone_.toString() << endl;
  }
  else if ( e.itemName() == "isSlave" && e.type() == "ItemChangedEvent" ){
    cout << "isSlave "  << isSlave_.toString() << endl;
    if ( ! bool ( isSlave_ ) ){
      set<xdaq::ApplicationDescriptor*> slaves( getApplicationContext()
						->getDefaultZone()
						->getApplicationDescriptors( "emu::pc::Configurator" ) );
      for ( set<xdaq::ApplicationDescriptor*>::iterator s = slaves.begin(); s != slaves.end(); ++ s ){
	slavesStatus_[ *s ] = VMEAccessor::Status_t();
      }
      slavesStatus_.erase( getApplicationDescriptor() ); // Remove ourselves, the master.
      selectedSource_ = emu::pc::Configuration::stringSource;
    }
  }
}

void emu::pc::Configurator::dummyTask(){
  int total = 30;
  progress_.setTotal( total ).setCurrent( 0 );
  for ( int i=0; i<total; ++i ){
    ::sleep( 1 );
    if ( getApplicationDescriptor()->getInstance() == 1 && i == 12 ){
      XCEPT_DECLARE( xcept::Exception, e, "Test exception." );
      bsem_.take();
      workLoopsCatch_ = e;
      bsem_.give();
    }
    progress_.setCurrent( i+1 );
    bsem_.take();
    progressPercent_ = progress_.getPercent();
    bsem_.give();
    if ( isToStop_ ) return;
  }
}


//#################


//////////////////////////////////////////////////////////////////////////
// Layouts of html pages
//////////////////////////////////////////////////////////////////////////
void emu::pc::Configurator::CrateTests(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  // if(!parsed) ParsingXML();  
  // MyHeader(in,out,"CrateTests");
  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)", ( fsm_.getCurrentState() == 'E' ? 5 : -1 ) );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector", 
			"/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" )
	      .addLink( "Main Page", 
			"/" + getApplicationDescriptor()->getURN() + "/MainPage" )
	      .addLink( "Crate Tests" )
	      );
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Crate Tests").set("style","color:blue") ;
  //
  *out << cgicc::pre();
  *out << "Use MPC_Utilities to mask out unwanted TMBs (Hard Reset will clear the masks)" << std::endl;
  *out << cgicc::pre();
  //
  std::string TmbMPCTest = toolbox::toString("/%s/TmbMPCTest",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TmbMPCTest) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Crate TMB/MPC test") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  if (myCrateTest.GetMpcTMBTestResult() == -1 ) {
    *out << "Not tested yet" << std::endl;
  } else if (myCrateTest.GetMpcTMBTestResult() == 0 ) {
    *out << "Failed" <<std::endl;
  } else {
    *out << "Passed" <<std::endl;
  }
  //
  *out<< cgicc::br() ;
  *out<< cgicc::br() ;
  //
  *out << cgicc::pre();
  *out << "If MPC switch S2-1/2=on/off, the following scan will pass for all values (CMS running)" << std::endl;
  *out << "If MPC switch S2-1/2=off/on, the following scan will determine the MPC safe window" << std::endl;
  *out << cgicc::pre();
  //
  std::string MPCSafeWindowScan = toolbox::toString("/%s/MPCSafeWindowScan",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MPCSafeWindowScan) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","MPC Safe Window Scan") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
  *out << cgicc::form().set("method","GET") << std::endl ;
  *out << cgicc::textarea().set("name","CrateTestsOutput").set("WRAP","OFF").set("rows","20").set("cols","100");
  *out << CrateTestsOutput.str() << std::endl ;
  *out << cgicc::textarea();
  *out << cgicc::form() << std::endl ;
  //
  std::string LogCrateTestsOutput = toolbox::toString("/%s/LogCrateTestsOutput",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LogCrateTestsOutput) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Log output").set("name","LogCrateTestsOutput") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Clear").set("name","ClearCrateTestsOutput") << std::endl ;
  *out << cgicc::form() << std::endl ;
  pageFooter( in, out );
  htmlFooter( in, out );

  emu::utils::redirectToSelf( in, out );
}
//
// void emu::pc::Configurator::DefineConfiguration(xgi::Input * in, xgi::Output * out ) 
//   throw (xgi::exception::Exception) {
//   //
//   MyHeader(in,out,"Change Configuration File");
//   //
//   *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
//   *out << std::endl;
//   //
//   *out << cgicc::legend("Upload Configuration...").set("style","color:blue") << std::endl ;
//   //
//   std::string method = toolbox::toString("/%s/setConfFile",getApplicationDescriptor()->getURN().c_str());
//   *out << cgicc::form().set("method","POST").set("action",method) << std::endl ;
//   *out << cgicc::input().set("type","text").set("name","xmlFilename").set("size","90").set("ENCTYPE","multipart/form-data").set("value",xmlFile_) << std::endl;
//   *out << cgicc::input().set("type","submit").set("value","Set configuration file local") << std::endl ;
//   *out << cgicc::form() << std::endl ;
//   //
//   // Upload file...
//   //
//   std::string methodUpload = toolbox::toString("/%s/UploadConfFile",getApplicationDescriptor()->getURN().c_str());
//   *out << cgicc::form().set("method","POST").set("enctype","multipart/form-data").set("action",methodUpload) << std::endl ;
//   *out << cgicc::input().set("type","file").set("name","xmlFilenameUpload").set("size","90") << std::endl;
//   *out << cgicc::input().set("type","submit").set("value","Send") << std::endl ;
//   *out << cgicc::form() << std::endl ;
//   //
//   *out << std::endl;
//   //
//   *out << cgicc::fieldset();
//   //
//   *out << std::endl;
//   //
// }
//
void emu::pc::Configurator::CalibrationRuns(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  // MyHeader(in,out,"CalibrationRuns");
  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)", ( fsm_.getCurrentState() == 'E' ? 5 : -1 ) );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector", 
			"/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" )
	      .addLink( "Main Page", 
			"/" + getApplicationDescriptor()->getURN() + "/MainPage" )
	      .addLink( "Calibration Runs" )
	      );
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Calibration Runs").set("style","color:blue") ;
  //
  std::string CalibrationCFEBXtalk = toolbox::toString("/%s/CalibrationCFEBXtalk",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBXtalk) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration Strips : CFEB Xtalk") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string CalibrationCFEBGain = toolbox::toString("/%s/CalibrationCFEBGain",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBGain) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration Strips : CFEB Gain") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string CalibrationCFEBPedestal = toolbox::toString("/%s/CalibrationCFEBPedestal",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBPedestal) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration Strips CFEB Pedestal") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string CalibrationCFEBSaturation = toolbox::toString("/%s/CalibrationSaturation",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBSaturation) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration Strips : CFEB high-end amplifier saturation") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  //
  std::string CalibrationALCTThresholdScan = toolbox::toString("/%s/CalibrationALCTThresholdScan",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationALCTThresholdScan) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration ALCT : Threshold Scan") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string CalibrationALCTConnectivity = toolbox::toString("/%s/CalibrationALCTConnectivity",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationALCTConnectivity) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration ALCT : Connectivity") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string CalibrationCFEBConnectivity = toolbox::toString("/%s/CalibrationCFEBConnectivity",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationCFEBConnectivity) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration CFEB : Connectivity") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string CalibrationComparatorPulse = toolbox::toString("/%s/CalibrationComparatorPulse",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CalibrationComparatorPulse) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Calibration Comparator Pulse") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << cgicc::legend("Calibration tools").set("style","color:blue") ;
  //
  std::string FindLv1aDelayComparator = toolbox::toString("/%s/FindLv1aDelayComparator",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",FindLv1aDelayComparator) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find l1a delay for Comparator") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string FindLv1aDelayALCT = toolbox::toString("/%s/FindLv1aDelayALCT",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",FindLv1aDelayALCT) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find l1a delay for ALCT") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
  pageFooter( in, out );
  htmlFooter( in, out );

  emu::utils::redirectToSelf( in, out );
}
//
void emu::pc::Configurator::CrateConfiguration(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  // if(!parsed)
  // {  this->Default(in,out);
  //    return;
  // }
  std::cout << "CrateConfiguration: " << ThisCrateID_ << std::endl;
  // MyHeader(in,out,"CrateConfiguration");
  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)", ( fsm_.getCurrentState() == 'E' ? 5 : -1 ) );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector", 
			"/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" )
	      .addLink( "Main Page", 
			"/" + getApplicationDescriptor()->getURN() + "/MainPage" )
	      .addLink( "Crate Configuration" )
	      );
  //
  if(thisCrate->IsAlive())
     *out << cgicc::h2("Current Crate: "+ ThisCrateID_ );
  else
     *out << cgicc::span().set("style","color:red") << cgicc::h2("Current Crate: "+ ThisCrateID_ + ",  Excluded") << cgicc::span();

  *out << std::endl;

  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial; background-color:#00FF00");
  *out << std::endl;
  //
  for(int ii=1; ii<22; ii++) {
    //
    *out << cgicc::table().set("border","1");
    //
    *out << cgicc::td();
    //
    *out << "Slot " << std::setfill('0') << std::setw(2) << std::dec << ii << std::endl;
    //
    *out << cgicc::td();
    //----------------------------------
    // Display crate controller buttons
    //----------------------------------
    //
    if(ii==1) {
      *out << cgicc::td();
      *out << "VME Crate Controller" ;
      *out << cgicc::td();
      //
      *out << cgicc::td();
      std::string ControllerUtils_Xfer = toolbox::toString("/%s/ControllerUtils_Xfer?ccb=%d",getApplicationDescriptor()->getURN().c_str(),ii);
      *out << cgicc::a("Controller Utils").set("href",ControllerUtils_Xfer) << std::endl;
      *out << cgicc::td();
      //
    }
    //
    //----------------------------------
    // Display CCB buttons
    //----------------------------------
    int slot = -1;
    if(thisCCB) slot=thisCCB->slot();
    if(slot == ii) {
      *out << cgicc::td();
      *out << "CCB" ;
      *out << cgicc::td();
      //
      *out << cgicc::td();
      std::string CCBStatus = toolbox::toString("/%s/CCBStatus?ccb=%d",getApplicationDescriptor()->getURN().c_str(),ii);
      *out << cgicc::a("CCB Status").set("href",CCBStatus) << std::endl;
      *out << cgicc::td();
      //
      *out << cgicc::td();
      std::string CCBUtils = toolbox::toString("/%s/CCBUtils?ccb=%d",getApplicationDescriptor()->getURN().c_str(),ii);
      *out << cgicc::a("CCB Utils").set("href",CCBUtils) << std::endl;
      *out << cgicc::td();
      //
    }
    //
    //----------------------------------
    // Display MPC buttons, if it exists
    //----------------------------------
    slot = -1;
    if ( thisMPC ) slot = thisMPC->slot() ;
    if(slot == ii) {
      //
      *out << cgicc::td();
      *out << "MPC" ;
      *out << cgicc::td();
      //
      if ( thisMPC ) {
	//
	*out << cgicc::td();
	std::string MPCStatus = toolbox::toString("/%s/MPCStatus?mpc=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	*out << cgicc::a("MPC Status").set("href",MPCStatus) << std::endl;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string MPCUtils = toolbox::toString("/%s/MPCUtils?mpc=%d",getApplicationDescriptor()->getURN().c_str(),ii);
	*out << cgicc::a("MPC Utils").set("href",MPCUtils) << std::endl;
	*out << cgicc::td();
      }
    }
    //
    //----------------------------------
    // Display DDU buttons, if it exists
    //----------------------------------
    //
    for (unsigned int i=0; i<dduVector.size(); i++) {
      //
      int slot = dduVector[i]->slot();
      if(slot == ii) {
	//
	*out << cgicc::td();
	*out << "DDU" ;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string DDUStatus = toolbox::toString("/%s/DDUStatus?ddu=%d",getApplicationDescriptor()->getURN().c_str(),i);
	*out << cgicc::a("DDU Status").set("href",DDUStatus) << std::endl;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string DDUUtils = toolbox::toString("/%s/DDUUtils?ddu=%d",getApplicationDescriptor()->getURN().c_str(),i);
	*out << cgicc::a("DDU Utils").set("href",DDUUtils) << std::endl;
	*out << cgicc::td();
      }
    }
    //
    //----------------------------------
    // Display TMB buttons, if it exists
    //----------------------------------
    std::string TMBStatus ;
    std::string TMBTests ;
    std::string TMBUtils ;
    //
    TMBStatus  = toolbox::toString("/%s/TMBStatus" ,getApplicationDescriptor()->getURN().c_str());
    TMBTests   = toolbox::toString("/%s/TMBTests"  ,getApplicationDescriptor()->getURN().c_str());
    TMBUtils   = toolbox::toString("/%s/TMBUtils"  ,getApplicationDescriptor()->getURN().c_str());
    //
    for (unsigned int i=0; i<tmbVector.size(); i++) {
      //
      int slot = tmbVector[i]->slot();
      if(slot == ii) {
	//
	*out << cgicc::td();
	*out << "TMB / RAT / ALCT" ;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string TMBStatus = toolbox::toString("/%s/TMBStatus?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	*out << cgicc::a("TMB Status").set("href",TMBStatus) << std::endl;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string TMBTests = toolbox::toString("/%s/TMBTests?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	*out << cgicc::a("TMB Tests").set("href",TMBTests) << std::endl;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string TMBUtils = toolbox::toString("/%s/TMBUtils?tmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	*out << cgicc::a("TMB Utils").set("href",TMBUtils) << std::endl;
	*out << cgicc::td();
	//
	//Found TMB...look for DMB...
	//
	for (unsigned int iii=0; iii<dmbVector.size(); iii++) {
	  //
	  int dmbslot = dmbVector[iii]->slot();
	  std::string ChamberTests = toolbox::toString("/%s/ChamberTests",getApplicationDescriptor()->getURN().c_str());    
	  //
	  if ( dmbslot == slot+1 ) {
	    *out << cgicc::td();
	    char Name[50];
	    sprintf(Name,"Chamber Tests: %s",(thisCrate->GetChamber(slot)->GetLabel()).c_str());
	    std::string ChamberTests = toolbox::toString("/%s/ChamberTests?tmb=%d&dmb=%d",getApplicationDescriptor()->getURN().c_str(),i,iii);
	    *out << cgicc::a(Name).set("href",ChamberTests) << std::endl;
	    *out << cgicc::td();
	  }
	  //
	}
      }
    }
    //
    std::string DMBStatus;
    std::string DMBTests;
    std::string DMBUtils;
    //
    DMBStatus  = toolbox::toString("/%s/DMBStatus",getApplicationDescriptor()->getURN().c_str());
    DMBTests   = toolbox::toString("/%s/DMBTests",getApplicationDescriptor()->getURN().c_str());
    DMBUtils   = toolbox::toString("/%s/DMBUtils",getApplicationDescriptor()->getURN().c_str());
    //
    for (unsigned int i=0; i<dmbVector.size(); i++) {
      int slot = dmbVector[i]->slot();
      if(slot == ii ) {
	//
	*out << cgicc::td();
	*out << "DMB / CFEB" ;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string DMBStatus = toolbox::toString("/%s/DMBStatus?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	*out << cgicc::a("DMB Status").set("href",DMBStatus) << std::endl;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string DMBTests = toolbox::toString("/%s/DMBTests?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	*out << cgicc::a("DMB Tests").set("href",DMBTests) << std::endl;
	*out << cgicc::td();
	//
	*out << cgicc::td();
	std::string DMBUtils = toolbox::toString("/%s/DMBUtils?dmb=%d",getApplicationDescriptor()->getURN().c_str(),i);
	*out << cgicc::a("DMB Utils").set("href",DMBUtils) << std::endl;
	*out << cgicc::td();
	//
      }
    }
    //
    *out << cgicc::table();
    *out << cgicc::br();
  }
  //
  //
  *out << cgicc::table().set("border","1");
  //
  *out << cgicc::td();
  *out << "Data readout synchronization parameters..." << std::endl;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string MeasureL1AsForCrate = toolbox::toString("/%s/MeasureL1AsForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureL1AsForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find L1A delays for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string MeasureDAVsForCrate = toolbox::toString("/%s/MeasureDAVsForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureDAVsForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find DAV delays for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string MeasureL1AsAndDAVsForCrate = toolbox::toString("/%s/MeasureL1AsAndDAVsForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureL1AsAndDAVsForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find L1A and DAV delays for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string QuickScanForCrate = toolbox::toString("/%s/QuickScanForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",QuickScanForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Quick Scan for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::tr();
  //
  *out << cgicc::td();
  *out << "Muonic timing scans" << std::endl;
  *out << cgicc::td();  
  //
  *out << cgicc::td();
  std::string MeasureALCTTMBRxTxForCrate = toolbox::toString("/%s/MeasureALCTTMBRxTxForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureALCTTMBRxTxForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find ALCT rx/tx for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string MeasureCFEBTMBRxForCrate = toolbox::toString("/%s/MeasureCFEBTMBRxForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureCFEBTMBRxForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find CFEB rx for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  char buf[20];
  std::string Settmb_bxn_offset = toolbox::toString("/%s/Settmb_bxn_offset",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",Settmb_bxn_offset) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Set tmb_bxn_offset for BC0 scan") << std::endl ;
  *out << "tmb_bxn_offset (bx)" << std::endl;
  sprintf(buf,"%d",MyTest[0][current_crate_].getLocalTmbBxnOffset());
  *out << cgicc::input().set("type","text").set("value",buf).set("name","tmb_bxn_offset") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string ALCTBC0ScanForCrate = toolbox::toString("/%s/ALCTBC0ScanForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTBC0ScanForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Synchronize ALCT BC0 for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::tr();
  //
  *out << cgicc::td();
  *out << "Other cratewide utilities" << std::endl;
  *out << cgicc::td();  
  //
  *out << cgicc::td();
  std::string SetTwoLayerTriggerForCrate = toolbox::toString("/%s/SetTwoLayerTriggerForCrate",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",SetTwoLayerTriggerForCrate) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Enable two-layer trigger for crate") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //*out << cgicc::body();
  *out << cgicc::fieldset();
  //
  pageFooter( in, out );
  htmlFooter( in, out );

  emu::utils::redirectToSelf( in, out );
}
//////////////////////////////////////////////////////////////////////////
// Action:  calibrations
//////////////////////////////////////////////////////////////////////////
void emu::pc::Configurator::CalibrationALCTThresholdScan(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(configuration_.getEmuEndcap());
  //
  calib.ALCTThresholdScan();
  //
  this->Default(in,out);
  //
}
//
void emu::pc::Configurator::CalibrationALCTConnectivity(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(configuration_.getEmuEndcap());
  //
  calib.ALCTConnectivity();
  //
  this->Default(in,out);
  //
}
//
void emu::pc::Configurator::CalibrationCFEBConnectivity(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(configuration_.getEmuEndcap());
  //
  calib.CFEBConnectivity();
  //
  this->Default(in,out);
  //
}
//
void emu::pc::Configurator::FindLv1aDelayComparator(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(configuration_.getEmuEndcap());
  //
  calib.FindL1aDelayComparator();
  //
  this->Default(in,out);
  //
}
//
void emu::pc::Configurator::FindLv1aDelayALCT(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(configuration_.getEmuEndcap());
  //
  calib.FindL1aDelayALCT();
  //
  this->Default(in,out);
  //
}
//
void emu::pc::Configurator::CalibrationCFEBGain(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(configuration_.getEmuEndcap());
  //
  calib.gainCFEB();
  //
  this->Default(in,out);
  //
}
//
void emu::pc::Configurator::CalibrationCFEBSaturation(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(configuration_.getEmuEndcap());
  //
  calib.CFEBSaturationTest();
  //
  this->Default(in,out);
  //
}
//
void emu::pc::Configurator::CalibrationCFEBXtalk(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(configuration_.getEmuEndcap());
  //
  calib.timeCFEB();
  //
  this->Default(in,out);
  //
}
//
void emu::pc::Configurator::CalibrationComparatorPulse(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(configuration_.getEmuEndcap());
  //
  calib.pulseComparatorPulse();
  //
  this->Default(in,out);
  //
}
//
void emu::pc::Configurator::CalibrationCFEBPedestal(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  CalibDAQ calib(configuration_.getEmuEndcap());
  //
  //calib.loadConstants();
  //
  //int nsleep, nstrip, tries;
  //float dac;
  //
  calib.pedestalCFEB();
  //
  this->Default(in,out);
  //
}
//
void emu::pc::Configurator::CrateDumpConfiguration(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  thisCrate->DumpConfiguration();
  //
  this->Default(in,out);
}
//
////////////////////////////////////////////////////////////////////////////////////
// Check Configuration
////////////////////////////////////////////////////////////////////////////////////
void emu::pc::Configurator::CheckConfigurationPage(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  char Name[100];
  sprintf(Name,"CSC Configuration Status");
  //
  // MyHeader(in,out,Name);
  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)" );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector", 
			"/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" )
	      .addLink( "Main Page", 
			"/" + getApplicationDescriptor()->getURN() + "/MainPage" )
	      .addLink( "Configuration Check" ) );
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  //
  *out << cgicc::legend("Actions").set("style","color:blue") << std::endl ;
  //
  *out << cgicc::table().set("border","1");
  //
  *out << cgicc::td();
  std::string CheckTimeSinceHardReset = toolbox::toString("/%s/CheckTimeSinceHardReset",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckTimeSinceHardReset) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check time since last hard reset") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string CheckBC0Synchronization = toolbox::toString("/%s/CheckBC0Synchronization",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckBC0Synchronization) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check BC0 synchronization") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string CheckCratesConfiguration = toolbox::toString("/%s/CheckCratesConfiguration",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckCratesConfiguration) << std::endl ;
  if (all_crates_ok == 1) {
    *out << cgicc::input().set("type","submit").set("value","Check configuration of crates").set("style","color:green") << std::endl ;
  } else if (all_crates_ok == 0) {
    *out << cgicc::input().set("type","submit").set("value","Check configuration of crates").set("style","color:red") << std::endl ;
  } else if (all_crates_ok == -1) {
    *out << cgicc::input().set("type","submit").set("value","Check configuration of crates").set("style","color:blue") << std::endl ;
  }
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  *out << cgicc::fieldset();
  //
  int initial_crate = current_crate_;
  //
  if (print_config_check_output) {
    //
    *out                     << "date_time = " << date_and_time_  << cgicc::br() << std::endl;
    OutputCheckConfiguration << "date_time = " << date_and_time_                << std::endl;
    //
  }
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Time since last hard reset").set("style","color:blue") << std::endl ;
  //
  bool print_times = false;
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) 
    for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
      if (time_since_reset[crate_number][chamber_index]>=0) print_times = true;
  //
  if ( print_times ) {
    //
    OutputCheckConfiguration << "Number of seconds since last hard reset for each TMB" << std::endl;
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      //
      *out                     << crateVector[crate_number]->GetLabel();
      OutputCheckConfiguration << crateVector[crate_number]->GetLabel();
      //
      for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	*out                     << " " << std::dec << time_since_reset[crate_number][chamber_index];
	OutputCheckConfiguration << " " << std::dec << time_since_reset[crate_number][chamber_index];
      }
      //
      *out                     << cgicc::br() << std::endl;
      OutputCheckConfiguration                << std::endl;
    }
  }
  *out << cgicc::fieldset();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Number of times BC0 is synchronized at TMB").set("style","color:blue") << std::endl ;
  //
  bool print_sync = false;
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) 
    for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) 
      if (bc0_sync[crate_number][chamber_index]>=0) print_sync = true;
  //
  if ( print_sync ) {
    //
    OutputCheckConfiguration << "Number of times BC0 from TMB matched BC0 from ALCT" << std::endl;
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      //
      *out                     << crateVector[crate_number]->GetLabel();
      OutputCheckConfiguration << crateVector[crate_number]->GetLabel();
      //
      for (unsigned chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	*out                     << " " << std::dec << bc0_sync[crate_number][chamber_index];
	OutputCheckConfiguration << " " << std::dec << bc0_sync[crate_number][chamber_index];
      }
      //
      *out                     << cgicc::br() << std::endl;
      OutputCheckConfiguration                << std::endl;
    }
  }
  //
  *out << cgicc::fieldset();
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Configuration Status").set("style","color:blue") << std::endl ;
  //
  if (all_crates_ok >= 0) 
  {  
     // all_crates_ok == -1 means not checked yet
     // all_crates_ok >= 0 means at least checked once, so show any problems here

    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      //
      if ( crate_check_ok[current_crate_]==0 ) {
	//
	//    OutputCheckConfiguration << "Problem summary for Crate " << thisCrate->GetLabel() << "..." << std::endl;
	//
	if( !(ccb_check_ok[current_crate_]) ) {
	  *out                     << thisCrate->GetLabel() << "<span style=\"color:red\" >, CCB config fail </span>" << cgicc::br() << std::endl;
	  OutputCheckConfiguration << thisCrate->GetLabel() << ", CCB"                << std::endl;
	}
	//
	if( !(mpc_check_ok[current_crate_]) ) {
	  *out                     << thisCrate->GetLabel() << "<span style=\"color:red\" >, MPC config fail </span>" << cgicc::br() << std::endl;
	  OutputCheckConfiguration << thisCrate->GetLabel() << ", MPC"                << std::endl;
	}
	//
	for (unsigned int chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	  //
	  if( tmb_check_ok[current_crate_][chamber_index]  > 0  ||
	      alct_check_ok[current_crate_][chamber_index] > 0  ||
	      dmb_check_ok[current_crate_][chamber_index]  > 0 ) {
	    //
	    *out << "<span style=\"color:black\" >";
	    *out                     << thisCrate->GetLabel() << ", " << (chamberVector[chamber_index]->GetLabel()).c_str();
	    OutputCheckConfiguration << thisCrate->GetLabel() << ", " << (chamberVector[chamber_index]->GetLabel()).c_str();
	    *out << "</span>";
	    //
	    bool print_description = false;
	    //
	    if( tmb_check_ok[current_crate_][chamber_index] > 0) {
	      //
	      OutputCheckConfiguration << ", TMB, " << tmb_check_ok[current_crate_][chamber_index];
	      //
	      if( tmb_check_ok[current_crate_][chamber_index]        == 1) {
		*out << "<span style=\"color:red\" > TMB config fail, </span>";
	      } else if( tmb_check_ok[current_crate_][chamber_index] == 2) {
		*out << "<span style=\"color:black\" > expected TMB config fail, </span>";
		print_description = true;
	      } else if( tmb_check_ok[current_crate_][chamber_index] == 3) {
		*out << "<span style=\"color:blue\" > did not see expected TMB config failure, </span>";
		print_description = true;
	      } else if( tmb_check_ok[current_crate_][chamber_index] == 4) {
		*out << "<span style=\"color:red\" > TMB FPGA did not program, </span>";
	      } 
	      //
	    } else {
	      //
	      *out                     << ", , ";
	      OutputCheckConfiguration << ", , ";
	    }
	    //
	    if( alct_check_ok[current_crate_][chamber_index] > 0) {
	      //
	      OutputCheckConfiguration << ", ALCT, " << alct_check_ok[current_crate_][chamber_index];
	      //
	      if( alct_check_ok[current_crate_][chamber_index]        == 1) {
		*out << "<span style=\"color:red\" > ALCT config fail, </span>";
	      } else if( alct_check_ok[current_crate_][chamber_index] == 2) {
		*out << "<span style=\"color:black\" > expected ALCT config fail, </span>";
		print_description = true;
	      } else if( alct_check_ok[current_crate_][chamber_index] == 3) {
		*out << "<span style=\"color:blue\" > did not see expected ALCT config failure, </span>";
		print_description = true;
	      } else if( alct_check_ok[current_crate_][chamber_index] == 4) {
		*out << "<span style=\"color:red\" > ALCT FPGA did not program, </span>";
	      } 
	      //
	    } else {
	      //
	      *out                     << ", , ";
	      OutputCheckConfiguration << ", , ";
	    }
	    //
	    if( dmb_check_ok[current_crate_][chamber_index] > 0) {
	      //
	      OutputCheckConfiguration << ", DMB, " << dmb_check_ok[current_crate_][chamber_index];
	      //
	      if( dmb_check_ok[current_crate_][chamber_index]        == 1) {
		*out << "<span style=\"color:red\" > DMB config fail, </span>";
	      } else if( dmb_check_ok[current_crate_][chamber_index] == 2) {
		*out << "<span style=\"color:black\" > expected DMB config fail, </span>";
		print_description = true;
	      } else if( dmb_check_ok[current_crate_][chamber_index] == 3) {
		*out << "<span style=\"color:blue\" > did not see expected DMB config failure, </span>";
		print_description = true;
	      } else if( dmb_check_ok[current_crate_][chamber_index] == 4) {
		*out << "<span style=\"color:red\" > DMB FPGA did not program, </span>";
	      } 
	      //
	    } else {
	      //
	      *out                     << ", , ";
	      OutputCheckConfiguration << ", , ";
	    }
	    //
	    if (print_description) {
	      *out                     << ", " << (chamberVector[chamber_index]->GetProblemDescription()).c_str();
	      OutputCheckConfiguration << ", " << (chamberVector[chamber_index]->GetProblemDescription()).c_str();
	    } else {
	      *out                     << ", ";
	      OutputCheckConfiguration << ", ";
	    }
	    //
	    *out                     << cgicc::br() << std::endl;
	    OutputCheckConfiguration                << std::endl;
	    //
	  } 
	}  // end chamber for-loop
      } else if (crate_check_ok[current_crate_] == -1) {
	//
	*out << cgicc::span().set("style","color:red");
	*out                     << crateVector[crate_number]->GetLabel() << " Not checked" << cgicc::br() << cgicc::span() << std::endl ;
	OutputCheckConfiguration << crateVector[crate_number]->GetLabel() << " Not checked" << std::endl;
      }

    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  *out << cgicc::fieldset();
  //
  if (print_config_check_output) {
    //
    //Output the errors to a file...
    //
    // The peripheral crate labels have the convention:  VME[p,n]N_M.  Here we use 
    // the "p" or "n" to label which endcap we are checking the firmware status on...
    const char * crate_name = crateVector[0]->GetLabel().c_str();
    char endcap_side = crate_name[3];
    //
    // This file is hardcoded as FirmwareDir_/status_check/YEARMODA_HRMN_[p,n]_firmware_status.log
    char filename[200];
    sprintf(filename,"%s/status_check/%s_%c_configuration_check.log",FirmwareDir_.c_str(),date_and_time_,endcap_side);
    //
    //  std::cout << "filename = " << filename << std::endl;
    //
    std::ofstream LogFileCheckConfiguration;
    LogFileCheckConfiguration.open(filename);
    LogFileCheckConfiguration << OutputCheckConfiguration.str() ;
    LogFileCheckConfiguration.close();
    //
  }
  print_config_check_output = false;
  OutputCheckConfiguration.clear();

  pageFooter( in, out );
  htmlFooter( in, out );
}
//
void emu::pc::Configurator::CheckTimeSinceHardReset(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::cout << emu::utils::getDateTime() << " Button: Check time since TMBs last received hard resets... " << std::endl; 
  //
  int initialcrate=current_crate_;
  //
  for (int i=0; i<60; i++) 
    for (int j=0; j<9; j++) 
      time_since_reset[i][j] = -1;
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	time_since_reset[i][tmb] = tmbVector[tmb]->ReadRegister(0xE8); 
	std::cout << " " << tmbVector[tmb]->ReadRegister(0xE8);
      }
      std::cout << std::endl;
    }
  }
  //
  SetCurrentCrate(initialcrate);	
  //
  this->CheckConfigurationPage(in, out);
}
//
void emu::pc::Configurator::CheckBC0Synchronization(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  cgicc::CgiEnvironment cgiEnvi(in);
  //
  std::cout << emu::utils::getDateTime() << " Button: Check BC0 synchronization at TMBs... " << std::endl; 
  //
  int initialcrate=current_crate_;
  //
  for (int i=0; i<60; i++) 
    for (int j=0; j<9; j++) 
      bc0_sync[i][j] = 0;
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	for (int count=0; count < 100; count++) {
	  tmbVector[tmb]->ReadRegister(0xCA);
	  bc0_sync[i][tmb] += tmbVector[tmb]->GetReadBx0Match();
	}
	std::cout << " " << bc0_sync[i][tmb];
      }
      std::cout << std::endl;
    }
  }
  //
  SetCurrentCrate(initialcrate);	
  //
  this->CheckConfigurationPage(in, out);
}
//
void emu::pc::Configurator::CheckCratesConfiguration(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  std::cout << emu::utils::getDateTime() << " Button:  Check Configuration of All Active Crates" << std::endl;
  //
  int initialcrate=current_crate_;
  //
  if(total_crates_<=0) return;
  //
  print_config_check_output = true;
  //
  OutputCheckConfiguration.str(""); //clear the output string
  //
  // get the date and time of this check:
  time_t rawtime;
  struct tm * timeinfo;
  //
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  //
  int yearAD = timeinfo->tm_year + 1900;
  int month_counting_from_one = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  int hour = timeinfo->tm_hour;     
  int minute = timeinfo->tm_min;     
  //
  sprintf(date_and_time_,"%4u%02u%02u_%02u%02u",yearAD,month_counting_from_one,day,hour,minute);
  //
  std::cout << "Check time since TMBs last received hard resets... " << std::endl; 
  //
  for (int i=0; i<60; i++) {
    for (int j=0; j<9; j++) {
      time_since_reset[i][j] = -1;
      bc0_sync[i][j] = 0;
    }
  }
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	time_since_reset[i][tmb] = tmbVector[tmb]->ReadRegister(0xE8); 
	std::cout << " " << tmbVector[tmb]->ReadRegister(0xE8);
      }
      std::cout << std::endl;
    }
  }
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	for (int count=0; count < 100; count++) {
	  tmbVector[tmb]->ReadRegister(0xCA); 
	  bc0_sync[i][tmb] += tmbVector[tmb]->GetReadBx0Match();
	}
	std::cout << " " << bc0_sync[i][tmb];
      }
      std::cout << std::endl;
    }
  }
  //
  all_crates_ok = 1;
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      CheckPeripheralCrateConfiguration();
      //
      all_crates_ok &= crate_check_ok[i];
      //
    } else {
      //
      crate_check_ok[i] = -1;
    }
  }
  //
  SetCurrentCrate(initialcrate);
  //
  this->CheckConfigurationPage(in, out);
}
//
void emu::pc::Configurator::CheckCratesConfigurationFull(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  std::cout << emu::utils::getDateTime() << " Button:  Check Configuration of All Active Crates including TTCrx" << std::endl;
  //
  int initialcrate=current_crate_;
  //
  if(total_crates_<=0) return;
  //
  print_config_check_output = true;
  //
  OutputCheckConfiguration.str(""); //clear the output string
  //
  // get the date and time of this check:
  time_t rawtime;
  struct tm * timeinfo;
  //
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  //
  int yearAD = timeinfo->tm_year + 1900;
  int month_counting_from_one = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  int hour = timeinfo->tm_hour;     
  int minute = timeinfo->tm_min;     
  //
  sprintf(date_and_time_,"%4u%02u%02u_%02u%02u",yearAD,month_counting_from_one,day,hour,minute);
  //
  std::cout << "Check time since TMBs last received hard resets... " << std::endl; 
  //
  for (int i=0; i<60; i++) {
    for (int j=0; j<9; j++) {
      time_since_reset[i][j] = -1;
      bc0_sync[i][j] = 0;
    }
  }
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	time_since_reset[i][tmb] = tmbVector[tmb]->ReadRegister(0xE8); 
	std::cout << " " << tmbVector[tmb]->ReadRegister(0xE8);
      }
      std::cout << std::endl;
    }
  }
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	for (int count=0; count < 100; count++) {
	  tmbVector[tmb]->ReadRegister(0xCA); 
	  bc0_sync[i][tmb] += tmbVector[tmb]->GetReadBx0Match();
	}
	std::cout << " " << bc0_sync[i][tmb];
      }
      std::cout << std::endl;
    }
  }
  //
  all_crates_ok = 1;
  //
  for(unsigned i=0; i< crateVector.size(); i++) {
    //
    if ( crateVector[i]->IsAlive() ) {
      //
      SetCurrentCrate(i);	
      //
      CheckPeripheralCrateConfiguration(1);
      //
      all_crates_ok &= crate_check_ok[i];
      //
    } else {
      //
      crate_check_ok[i] = -1;
    }
  }
  //
  SetCurrentCrate(initialcrate);
  //
  this->PowerOnFixCFEB(in,out);
}
//
void emu::pc::Configurator::CheckCrateConfiguration(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Check Crate Configuration" << std::endl;
  //
  //std::cout << "Crate address = 0x" << std::hex << thisCrate->vmeController()->ipAddress() << std::endl;
  //
  CheckPeripheralCrateConfiguration();
  //
  this->Default(in, out);
}
//
void emu::pc::Configurator::CheckCrateFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Check CSC firmware in one crate" << std::endl;
  //
  std::cout << "Crate address = 0x" << std::hex << thisCrate->vmeController()->ipAddress() << std::dec << std::endl;
  //
  CheckPeripheralCrateFirmware();
  //
  this->CheckConfigurationPage(in, out);
}
//
// Another method which would be better in another class... let's make it work, first....
void emu::pc::Configurator::CheckPeripheralCrateConfiguration(int full_check) {
  //
  std::cout << emu::utils::getDateTime() << " Hardware configuration check for " << thisCrate->GetLabel() << std::endl;
  //  OutputCheckConfiguration << "Hardware configuration check for " << thisCrate->GetLabel() << "..." << std::endl;
  //
  crate_check_ok[current_crate_] = 1;
  //
  //  OutputCheckConfiguration << "Crate " << thisCrate->GetLabel(); // << std::endl;
  //
  // perform the checks on the hardware.  Note that CCB needs to be checked to determine if boards' FPGA's are programmed.
  thisCrate->ccb()->RedirectOutput(&OutputCheckConfiguration);
  ccb_check_ok[current_crate_] = thisCrate->ccb()->CheckConfig(full_check);
  thisCrate->ccb()->RedirectOutput(&std::cout);
  //
  thisCrate->mpc()->RedirectOutput(&OutputCheckConfiguration);
  mpc_check_ok[current_crate_] = thisCrate->mpc()->CheckConfig();
  thisCrate->mpc()->RedirectOutput(&std::cout);
  //
  crate_check_ok[current_crate_] &=  ccb_check_ok[current_crate_];  
  crate_check_ok[current_crate_] &=  mpc_check_ok[current_crate_];  
  //
  //
  // perform the checks on the hardware
  for (unsigned int chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
    Chamber * thisChamber     = chamberVector[chamber_index];
    TMB * thisTMB             = tmbVector[chamber_index];
    ALCTController * thisALCT = thisTMB->alctController();
    DAQMB * thisDMB           = dmbVector[chamber_index];
    //
    std::cout << "Hardware configuration check for " << thisCrate->GetLabel() << ", " << (thisChamber->GetLabel()).c_str() << std::endl;
    OutputCheckConfiguration << "Hardware configuration check for " << thisCrate->GetLabel() << ", " << (thisChamber->GetLabel()).c_str() << "..." << std::endl;
    //
    thisTMB->RedirectOutput(&OutputCheckConfiguration);
    thisTMB->CheckTMBConfiguration();
    tmb_check_ok[current_crate_][chamber_index]  = (int) thisTMB->GetTMBConfigurationStatus();
    thisTMB->RedirectOutput(&std::cout);
    crate_check_ok[current_crate_] &=  tmb_check_ok[current_crate_][chamber_index];  
    //
    thisALCT->RedirectOutput(&OutputCheckConfiguration);
    thisALCT->CheckALCTConfiguration();
    alct_check_ok[current_crate_][chamber_index] = (int) thisALCT->GetALCTConfigurationStatus();
    thisALCT->RedirectOutput(&std::cout);
    crate_check_ok[current_crate_] &=  alct_check_ok[current_crate_][chamber_index];  
    //
    thisDMB->RedirectOutput(&OutputCheckConfiguration);
    dmb_check_ok[current_crate_][chamber_index]  = (int) thisDMB->checkDAQMBXMLValues();
    thisDMB->RedirectOutput(&std::cout); 
    crate_check_ok[current_crate_] &=  dmb_check_ok[current_crate_][chamber_index];  
    //
  }
  //
  // in the following loop, change check_ok values to match the TYPE of configuration error...
  // = 0 = read configuration not OK
  // = 1 = read configuration not OK
  // = 2 = read configuration not OK (or FPGA not programmed), and this has been masked in the problem_mask as such
  // = 3 = read configuration OK, but this has been masked in the problem_mask as a problem
  // = 4 = FPGA not programmed --> don't use this, as it just introduces other problems...
  //
  for (unsigned int chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
    Chamber * thisChamber     = chamberVector[chamber_index];
    //    TMB * thisTMB             = tmbVector[chamber_index];
    //													
    // The following mapping is needed when the crate is not full...
    //    const int slot_to_ccb_index_mapping[21] = {22, 22, 0, 22, 1, 22, 2, 22, 3, 22, 4, 22, 22, 22, 5, 22, 6, 22, 7, 22, 8};    
    //
    if(!(tmb_check_ok[current_crate_][chamber_index])) {
      tmb_check_ok[current_crate_][chamber_index] = 1;
      //
      if(thisChamber->GetExpectedConfigProblemTMB()) 
	tmb_check_ok[current_crate_][chamber_index] = 2;
      //
    } else if ( thisChamber->GetExpectedConfigProblemTMB() ) {
      tmb_check_ok[current_crate_][chamber_index] = 3;
    } else {
      tmb_check_ok[current_crate_][chamber_index] = 0;
    }
    //
    // do not use the signal sent to CCB.  It just hides what the problem is...
    //    if( thisCrate->ccb()->GetReadTMBConfigDone(slot_to_ccb_index_mapping[thisTMB->slot()]) != 
    //	thisCrate->ccb()->GetExpectedTMBConfigDone() ) {
    //      tmb_check_ok[current_crate_][chamber_index] = 4;
    //      //
    //      if(thisChamber->GetExpectedConfigProblemTMB()) 
    //	tmb_check_ok[current_crate_][chamber_index] = 2;
    //    }
    //    
    //
    if(!(alct_check_ok[current_crate_][chamber_index])) {
      alct_check_ok[current_crate_][chamber_index] = 1;
      //
      if(thisChamber->GetExpectedConfigProblemALCT()) 
	alct_check_ok[current_crate_][chamber_index] = 2;
      //
    } else if ( thisChamber->GetExpectedConfigProblemALCT() ) {
      alct_check_ok[current_crate_][chamber_index] = 3;
    } else {
      alct_check_ok[current_crate_][chamber_index] = 0;
    }
    //
    // do not use the signal sent to CCB.  It just hides what the problem is...
    //    if( thisCrate->ccb()->GetReadALCTConfigDone(slot_to_ccb_index_mapping[thisTMB->slot()]) !=  
    //	thisCrate->ccb()->GetExpectedALCTConfigDone() ) {
    //      alct_check_ok[current_crate_][chamber_index] = 4;
    //      //
    //      if(thisChamber->GetExpectedConfigProblemALCT()) 
    //	alct_check_ok[current_crate_][chamber_index] = 2;
    //    }
    //
    //    
    //
    std::string chamberName=toolbox::toString((char *)thisChamber->GetLabel().c_str());
    //
    // DMB has the CFEB check lumped in...
    if(!(dmb_check_ok[current_crate_][chamber_index])) {
      dmb_check_ok[current_crate_][chamber_index] = 1;
      //
      if(thisChamber->GetExpectedConfigProblemDMB()    ||
	 thisChamber->GetExpectedConfigProblemCFEB1()  ||
	 thisChamber->GetExpectedConfigProblemCFEB2()  ||
	 thisChamber->GetExpectedConfigProblemCFEB3()  ||
	 thisChamber->GetExpectedConfigProblemCFEB4()  ||
	 thisChamber->GetExpectedConfigProblemCFEB5()  ) 
	  dmb_check_ok[current_crate_][chamber_index] = 2;

	//
    } else if( (thisChamber->GetExpectedConfigProblemDMB()    ||
		thisChamber->GetExpectedConfigProblemCFEB1()  ||
		thisChamber->GetExpectedConfigProblemCFEB2()  ||
		thisChamber->GetExpectedConfigProblemCFEB3()  ||
		thisChamber->GetExpectedConfigProblemCFEB4()  ||
		thisChamber->GetExpectedConfigProblemCFEB5()) ) {
      dmb_check_ok[current_crate_][chamber_index] = 3;
    } else {
      dmb_check_ok[current_crate_][chamber_index] = 0;
    }
    //
    // do not use the signal sent to CCB.  It just hides what the problem is...
    //    if( (thisCrate->ccb()->GetReadDMBConfigDone(slot_to_ccb_index_mapping[thisTMB->slot()]) !=  
    //	 thisCrate->ccb()->GetExpectedDMBConfigDone()) &&  
    //	(chamberName.find("1/3/")==std::string::npos) ) {           // this is because ME1/3 chambers do not have CFEB4) 
    //      dmb_check_ok[current_crate_][chamber_index] = 4;
    //      //
    //      if(thisChamber->GetExpectedConfigProblemDMB()    ||
    //	 thisChamber->GetExpectedConfigProblemCFEB1()  ||
    //	 thisChamber->GetExpectedConfigProblemCFEB2()  ||
    //	 thisChamber->GetExpectedConfigProblemCFEB3()  ||
    //	 thisChamber->GetExpectedConfigProblemCFEB4()  ||
    //	 thisChamber->GetExpectedConfigProblemCFEB5()  ) 
    //	  dmb_check_ok[current_crate_][chamber_index] = 2;
    //    }
    //
    if ( tmb_check_ok[current_crate_][chamber_index]  > 0 ) crate_check_ok[current_crate_] = 0;
    if ( alct_check_ok[current_crate_][chamber_index] > 0 ) crate_check_ok[current_crate_] = 0;
    if ( dmb_check_ok[current_crate_][chamber_index]  > 0 ) crate_check_ok[current_crate_] = 0;
    //
  }
  //
  return;
}
//

void emu::pc::Configurator::CheckFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  std::cout << emu::utils::getDateTime() << " Button: Check Firmware" << std::endl;
  std::cout << "Checking System Firmware " << std::dec << number_of_checks_ << " times..." << std::endl;
  //
  //Reset the values of peripheral crate configuration check
  all_crates_ok = -1;
  print_config_check_output = false;
  //
  //
  int initial_crate = current_crate_;
  //
  const float cfeb_minimum_current_value =  0.01;
  const float cfeb_maximum_current_value =  0.85;
  const float alct_maximum_current_value =  0.2 ;
  const float alct_minimum_current_value =  0.05;
  //
  // initialize the number of times each of the following checks are OK.
  //
  for (int i=0; i<60; i++) {
    ccb_firmware_ok[i] = 0;
    mpc_firmware_ok[i] = 0;
    for (int j=0; j<9; j++) {
      alct_firmware_ok[i][j] = 0;
      tmb_firmware_ok[i][j] = 0;
      dmb_vme_firmware_ok[i][j] = 0;
      dmb_control_firmware_ok[i][j] = 0;
      dmbcfg_ok[i][j]  = 0;
      dmb_config_ok[i][j]  = 0;
      alctcfg_ok[i][j]  = 0;
      tmbcfg_ok[i][j]  = 0;
      alct_lvmb_current_ok[i][j]  = 0;
      alct_adc_current_ok[i][j]  = 0;
      //
      for (int k=0; k<5; k++) {
	cfeb_firmware_ok[i][j][k] = 0;
	cfeb_config_ok[i][j][k]   = 0;
	cfeb_current_ok[i][j][k]  = 0;
      }
    }
  }
  //

  for (int hard_reset_index=0; hard_reset_index<number_of_checks_; hard_reset_index++) {
    //
    std::cout << "Firmware check iteration " << hard_reset_index << std::endl;
    //
    // send hard reset from CCB to load FPGA's from EEPROM's in all electronics modules.  
    // If the user does not request a hard reset, just read the values
    if (number_of_hard_resets_>0){
      //
      for(unsigned crate_index=0; crate_index< crateVector.size(); crate_index++){
	//
	SetCurrentCrate(crate_index);
        //
        std::cout << "CCB Hard Reset for " << thisCrate->GetLabel() << std::endl;
        //
	if (!thisCrate->IsAlive()) continue;
	//
	thisCCB->hardReset();
      }
    }
    //
    // Check that the firmware status is A-OK:
    for(unsigned crate_index=0; crate_index< crateVector.size(); crate_index++){
      //
      SetCurrentCrate(crate_index);
      if (!thisCrate->IsAlive()) continue;
//
      // check CCB and MPC firmware versions by reading the FPGA ID's
      ccb_firmware_ok[current_crate_] = thisCrate->ccb()->CheckFirmwareDate();
      mpc_firmware_ok[current_crate_] = thisCrate->mpc()->CheckFirmwareDate();
      //
      // read the "FPGA configuration done" bits which have been sent to the CCB
      thisCCB->ReadConfigBits();
      //
      // check TMB, DMB, ALCT, and CFEB's attached to this chamber...
      for (unsigned int chamber_index=0; chamber_index<dmbVector.size(); chamber_index++) {
	DAQMB * thisDMB = dmbVector[chamber_index];
	std::vector<CFEB> thisCFEBs = thisDMB->cfebs() ;
        int dslot = thisDMB->slot();
	//
	TMB * thisTMB   = tmbVector[chamber_index];
	ALCTController * thisALCT   = thisTMB->alctController();
	//
        Chamber * thisChamber = chamberVector[chamber_index];
	//
	if(dslot<22){
	  //
	  // check if the CCB has received the "config done" bit from the component's FPGA
	  if (thisCCB->GetReadTMBConfigDone(chamber_index) == thisCCB->GetExpectedTMBConfigDone() )
	    tmbcfg_ok[current_crate_][chamber_index]++;
	  //
	  if (thisCCB->GetReadALCTConfigDone(chamber_index) == thisCCB->GetExpectedALCTConfigDone() )
	    alctcfg_ok[current_crate_][chamber_index]++;
	  //
	  // The DMB config done bit depends on whether or not it is ME1/3.  I.e., if it is ME1/3, this 
	  // check will always fail.  That's OK, that just means we look at the currents and ID's of 
	  // all CFEBs and DMB for ME1/3 in detail, below.
	  if (thisCCB->GetReadDMBConfigDone(chamber_index) == thisCCB->GetExpectedDMBConfigDone() )
	    dmbcfg_ok[current_crate_][chamber_index]++;
	  // 
	  // check firmware versions by reading the FPGA ID's
	  tmb_firmware_ok[current_crate_][chamber_index]         += (int) thisTMB->CheckFirmwareDate();
	  alct_firmware_ok[current_crate_][chamber_index]        += (int) thisALCT->CheckFirmwareDate();
	  dmb_vme_firmware_ok[current_crate_][chamber_index]     += (int) thisDMB->CheckVMEFirmwareVersion();
	  dmb_control_firmware_ok[current_crate_][chamber_index] += (int) thisDMB->CheckControlFirmwareVersion();
	  for(CFEBItr cfebItr = thisCFEBs.begin(); cfebItr != thisCFEBs.end(); ++cfebItr) {
	    int cfeb_index = (*cfebItr).number();
	    cfeb_firmware_ok[current_crate_][chamber_index][cfeb_index] += (int) thisDMB->CheckCFEBFirmwareVersion(*cfebItr);
	  }
	  //
	  // check if the configuration of the CFEBs and DMBs are OK...
	  // in particular, check if the "smoking gun" for firmware loss is OK...
	  //
	  thisDMB->checkDAQMBXMLValues();   //this has the CFEB check implicit in it
	  //
	  // greg, put in cfeb firmware version check in CFEB config check
	  // greg, split up CFEBs in config check
	  if ( thisDMB->GetDAQMBSmokingGunIsOK() ) {
	    dmb_config_ok[current_crate_][chamber_index]++;
	  }
	  //
	  for(unsigned int cfeb_index=0;cfeb_index<thisCFEBs.size();cfeb_index++){
	    int calling_index = cfeb_index+1;
	    if ( thisDMB->GetSmokingGunIsOK(calling_index) )  {   
	      cfeb_config_ok[current_crate_][chamber_index][cfeb_index]++;
	    }
	    //	    std::cout << "smoking gun CFEB " << calling_index << " = " << cfeb_config_ok[current_crate_][chamber_index][cfeb_index];
	  }
	  //
	  // check if the currents drawn by the FPGA are within bounds or without
	  // ALCT current reading from LVMB
          float alct_lvmb_current = thisDMB->lowv_adc(3,0)/1000.;
          if(alct_lvmb_current > alct_maximum_current_value)        //the FPGA is drawing sufficient current to be called "OK"
            alct_lvmb_current_ok[current_crate_][chamber_index]++;
	  else if (alct_lvmb_current < alct_minimum_current_value)
            alct_lvmb_current_ok[current_crate_][chamber_index] += 2; //the FPGA is drawing less current than an unloaded FPGA:  blown fuse!
	  //
	  // ALCT current reading from on-board ADC
	  thisALCT->ReadAlctTemperatureAndVoltages();
	  float alct_adc_current = thisALCT->GetAlct_1p8_Current();
          if(alct_adc_current > alct_maximum_current_value)        //the FPGA is drawing sufficient current to be called "OK"
            alct_adc_current_ok[current_crate_][chamber_index]++;
	  else if (alct_adc_current < alct_minimum_current_value)
            alct_adc_current_ok[current_crate_][chamber_index] += 2; //the FPGA is drawing less current than an unloaded FPGA:  blown fuse!
	  //
	  // get the CFEB currents from LVMB.  Note, the ME1/1 have the cabling in a non-standard order on the LVMB
	  for(unsigned int cfeb_index=0;cfeb_index<thisCFEBs.size();cfeb_index++){
	    //
	    int cur18a[5]={1,1,1,2,2};
	    int cur18b[5]={1,4,7,2,5};
	    int me11_odd[5]={3,1,0,2,4};
	    int me11_even[5]={1,3,4,2,0};
	    int misscable[3][6]={
	      {+14,1,4,3,2,0},
	      {+21,3,0,1,2,4},
	      {+29,3,0,1,2,4}
	    };
	    // std::cout << " LSD febpromid " << cfeb_index << " " << thisChamber->GetLabel() 
	    // << " " << std::hex << thisDMB->febpromid(thisCFEBs[numcfeb]) << std::dec << std::endl;
	    //	    unsigned int mbid = thisDMB->mbfpgaid();
	    //	    std::cout << "LSD:DMB Problem " << thisChamber << " mbfpgaid " << mbid << std::endl;  
	    int lv=cfeb_index;
	    int ts,tr,tc;
	    sscanf(thisChamber->GetLabel().c_str(),"ME%d/%d/%d",&ts,&tr,&tc);
	    int tus=ts;
	    if(ts<0)tus=-tus;
	    if(tus==1&&tr==1){
	      if(tc%2==0){
		lv=me11_even[cfeb_index]; 
	      }else{
		lv=me11_odd[cfeb_index]; 
	      }
	      for(int t=0;t<3;t++){
		if(ts*tc==misscable[t][0]){
		  lv=misscable[t][cfeb_index+1];
		}
	      } 
	    }
	    float cfeb_current = thisDMB->lowv_adc(cur18a[lv],cur18b[lv])/1000.;
	    if(cfeb_current > cfeb_maximum_current_value) {       
	      //the FPGA is drawing sufficient current to be called "OK"
	      cfeb_current_ok[current_crate_][chamber_index][cfeb_index]++;
	    } else if (cfeb_current < cfeb_minimum_current_value) {
	      //the FPGA is drawing less current than an unloaded FPGA:  blown fuse!
	      cfeb_current_ok[current_crate_][chamber_index][cfeb_index] += 2; 
	    }
	  }  //loop over cfeb current check 
	}   // if (slot<22)
      }     // loop over chambers in crate
    }       // loop over crates
  }         // loop over hard resets
  //
  // Now that we've done the hard resets and checked all the checks, next we make a list of which components need their firmware reloaded
  //
  crate_to_reload.clear();
  slot_to_reload.clear();
  component_to_reload.clear();
  component_string.clear();
  reason_for_reload.clear();
  loaded_ok.clear();
  //
  for(unsigned crate_index=0; crate_index< crateVector.size(); crate_index++){
    //
    SetCurrentCrate(crate_index);
    if (!thisCrate->IsAlive()) continue;
    //
    if (ccb_firmware_ok[crate_index] != number_of_checks_) {
      //
      int number_of_bad_readings = number_of_checks_ - ccb_firmware_ok[crate_index];
      //
      crate_to_reload.push_back(crate_index);
      slot_to_reload.push_back(thisCCB->slot());
      component_to_reload.push_back(CCB_LABEL);
      //
      std::ostringstream problem_label;
      problem_label << "CCB"; 
      component_string.push_back(problem_label.str());
      //
      std::ostringstream reason;
      reason << "userID bad " << number_of_bad_readings << "/" << number_of_checks_ << " times";
      reason_for_reload.push_back(reason.str());
      loaded_ok.push_back(-1);
    }
    //
    if (mpc_firmware_ok[crate_index] != number_of_checks_) {
      //
      int number_of_bad_readings = number_of_checks_ - mpc_firmware_ok[crate_index];
      //
      std::ostringstream problem_label;
      problem_label << "MPC"; 
      component_string.push_back(problem_label.str());
      //
      crate_to_reload.push_back(crate_index);
      slot_to_reload.push_back(thisMPC->slot());
      component_to_reload.push_back(MPC_LABEL);
      std::ostringstream reason;
      reason << "userID bad " << number_of_bad_readings << "/" << number_of_checks_ << " times";
      reason_for_reload.push_back(reason.str());
      loaded_ok.push_back(-1);
    }
    //
    for (unsigned int chamber_index=0; chamber_index<dmbVector.size(); chamber_index++) {
      DAQMB * thisDMB = dmbVector[chamber_index];
      std::vector<CFEB> thisCFEBs = thisDMB->cfebs();
      int dslot = thisDMB->slot();
      //
      TMB * thisTMB   = tmbVector[chamber_index];
      int tslot = thisTMB->slot();
      //
      if (
	  //	  alctcfg_ok[crate_index][chamber_index]           != number_of_checks_ ||
	  //	  alct_adc_current_ok[crate_index][chamber_index]  != number_of_checks_ || 
	  //	  alct_firmware_ok[crate_index][chamber_index]     != number_of_checks_ ||
	  alct_lvmb_current_ok[crate_index][chamber_index] != number_of_checks_ 
	  ) {
	crate_to_reload.push_back(crate_index);
	slot_to_reload.push_back(tslot);
	component_to_reload.push_back(ALCT_LABEL);
	//
	std::ostringstream problem_label;
	problem_label << "ALCT"; 
	component_string.push_back(problem_label.str());
	//
	std::ostringstream reason;
	//	if (alct_firmware_ok[crate_index][chamber_index]     != number_of_checks_ ) {
	//	  int number_of_bad_readings = number_of_checks_ - alct_firmware_ok[crate_index][chamber_index];
	//	  reason << "userID bad " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	//	}
	//
	//	if (alct_adc_current_ok[crate_index][chamber_index]     != number_of_checks_ ) {
	//	  int number_of_bad_readings = number_of_checks_ - alct_adc_current_ok[crate_index][chamber_index];
	//	  reason << "I(ADC) bad " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	//	}
	//
	if (alct_lvmb_current_ok[crate_index][chamber_index]     != number_of_checks_ ) {
	  int number_of_bad_readings = number_of_checks_ - alct_lvmb_current_ok[crate_index][chamber_index];
	  reason << "I(LVMB) low, " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	}
	//
	// Although we do not use this as a "smoking gun", we print out if the configuration "done" bit makes it to the CCB...
	if (alctcfg_ok[crate_index][chamber_index]     != number_of_checks_ ) {
	  int number_of_bad_readings = number_of_checks_ - alctcfg_ok[crate_index][chamber_index];
	  reason << "FPGA cfg done bad" << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	}
      	//
	reason_for_reload.push_back(reason.str());
	loaded_ok.push_back(-1);
      }
      //
      if (
	  //tmbcfg_ok[crate_index][chamber_index]       != number_of_checks_ || 
	  tmb_firmware_ok[crate_index][chamber_index] != number_of_checks_  
	  ) {
	crate_to_reload.push_back(crate_index);
	slot_to_reload.push_back(tslot);
	component_to_reload.push_back(TMB_LABEL);
	//
	std::ostringstream problem_label;
	problem_label << "TMB"; 
	component_string.push_back(problem_label.str());
	//
	std::ostringstream reason;
	if (tmb_firmware_ok[crate_index][chamber_index]     != number_of_checks_ ) {
	  int number_of_bad_readings = number_of_checks_ - tmb_firmware_ok[crate_index][chamber_index];
	  reason << "userID bad " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	}
	//
	if (tmbcfg_ok[crate_index][chamber_index]     != number_of_checks_ ) {
	  int number_of_bad_readings = number_of_checks_ - tmbcfg_ok[crate_index][chamber_index];
	  reason << "cfg bad " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	}
	//
	reason_for_reload.push_back(reason.str());
	loaded_ok.push_back(-1);
      }
      //
      if (dmb_vme_firmware_ok[crate_index][chamber_index] != number_of_checks_ ) { 
	crate_to_reload.push_back(crate_index);
	slot_to_reload.push_back(dslot);
	component_to_reload.push_back(DMB_VME_LABEL);
	//
	std::ostringstream problem_label;
	problem_label << "DMB VME"; 
	component_string.push_back(problem_label.str());
	//
	std::ostringstream reason;
	if (dmb_vme_firmware_ok[crate_index][chamber_index]     != number_of_checks_ ) {
	  int number_of_bad_readings = number_of_checks_ - dmb_vme_firmware_ok[crate_index][chamber_index];
	  reason << "userID bad " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	}
	//
	reason_for_reload.push_back(reason.str());
	loaded_ok.push_back(-1);
      }
      //  
      if (
	  //dmb_control_firmware_ok[crate_index][chamber_index] < number_of_checks_ ||
	  dmb_config_ok[crate_index][chamber_index]   < number_of_checks_ 
	  ) {
	crate_to_reload.push_back(crate_index);
	slot_to_reload.push_back(dslot);
	component_to_reload.push_back(DMB_CONTROL_LABEL);
	//
	std::ostringstream problem_label;
	problem_label << "DMB Control FPGA"; 
	component_string.push_back(problem_label.str());
	//
	std::ostringstream reason;
	//	if (dmb_control_firmware_ok[crate_index][chamber_index]     < number_of_checks_ ) {
	//	  int number_of_bad_readings = number_of_checks_ - dmb_control_firmware_ok[crate_index][chamber_index];
	//	  reason << "userID bad " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	//	}
	//
	if (dmb_config_ok[crate_index][chamber_index] < number_of_checks_ ) {
	  int number_of_bad_readings = number_of_checks_ - dmb_config_ok[crate_index][chamber_index];
	  reason << "Config bits wrong, " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	}
	//
	reason_for_reload.push_back(reason.str());
	loaded_ok.push_back(-1);
      }
      //
      for(unsigned int cfeb_index=0;cfeb_index<thisCFEBs.size();cfeb_index++){
	if (
	    //	    cfeb_firmware_ok[crate_index][chamber_index][cfeb_index] < number_of_checks_ ||
	    cfeb_config_ok[crate_index][chamber_index][cfeb_index]   < number_of_checks_ ||
	    cfeb_current_ok[crate_index][chamber_index][cfeb_index]  < number_of_checks_ ) {
	  crate_to_reload.push_back(crate_index);
	  slot_to_reload.push_back(dslot);
	  component_to_reload.push_back(CFEB_LABEL[cfeb_index]);
	  //
	  std::ostringstream problem_label;
	  problem_label << "CFEB " << cfeb_index+1; 
	  component_string.push_back(problem_label.str());
	  //
	  std::ostringstream reason;
	  //	  if (cfeb_firmware_ok[crate_index][chamber_index][cfeb_index]     < number_of_checks_ ) {
	  //	    int number_of_bad_readings = number_of_checks_ - cfeb_firmware_ok[crate_index][chamber_index][cfeb_index];
	  //	    reason << "userID bad " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	  //	  }
	  //	  //
	  if (cfeb_current_ok[crate_index][chamber_index][cfeb_index]     < number_of_checks_ ) {
	    int number_of_bad_readings = number_of_checks_ - cfeb_current_ok[crate_index][chamber_index][cfeb_index];
	    reason << "I(LVMB) low, " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	  }
	  //
	  if (cfeb_config_ok[crate_index][chamber_index][cfeb_index]     < number_of_checks_ ) {
	    int number_of_bad_readings = number_of_checks_ - cfeb_config_ok[crate_index][chamber_index][cfeb_index];
	    reason << "Config bits high, " << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	  }
	  //
	  // Although we do not use this as a "smoking gun", we print out if the configuration "done" bit makes it to the CCB...
	  if (dmbcfg_ok[crate_index][chamber_index] != number_of_checks_ ) {
	    //	    int number_of_bad_readings = number_of_checks_ - dmbcfg_ok[crate_index][chamber_index];
	    reason << "FPGA cfg done bad"; // << number_of_bad_readings << "/" << number_of_checks_ << " times ";
	  }
	  //
	  reason_for_reload.push_back(reason.str());
	  loaded_ok.push_back(-1);
	}
      } 
      //
    }    //loop over chambers 
  }      //loop over crates
  //
  //
  // Log the output...
  //
  char date_and_time[13];
  //
  OutputCheckFirmware.str(""); //clear the output string
  //
  // get the date and time of this check:
  time_t rawtime;
  struct tm * timeinfo;
  //
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  //
  int yearAD = timeinfo->tm_year + 1900;
  int month_counting_from_one = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  int hour = timeinfo->tm_hour;     
  int minute = timeinfo->tm_min;     
  //
  //
  sprintf(date_and_time,"%4u%02u%02u_%02u%02u",yearAD,month_counting_from_one,day,hour,minute);
  //
  //  *out                << "date_time = " << date_and_time << cgicc::br() << std::endl;
  OutputCheckFirmware << "date_time = " << date_and_time                << std::endl;
  OutputCheckFirmware << "Crate, Chamber, slot, Component, Problem "    << std::endl;
  //
  //
  for (unsigned problem_index=0; problem_index<crate_to_reload.size(); problem_index++) {
    int problem_crate     = crate_to_reload[problem_index];
    int problem_slot      = slot_to_reload[problem_index];
    //
    SetCurrentCrate(problem_crate);
    //
    int within_crate_problem_index = -1;
    for (unsigned int chamber_index=0; chamber_index<dmbVector.size(); chamber_index++) {
      DAQMB * thisDMB = dmbVector[chamber_index];
      int dslot = thisDMB->slot();
      //
      TMB * thisTMB   = tmbVector[chamber_index];
      int tslot = thisTMB->slot();
      //
      if (dslot == problem_slot || tslot == problem_slot) 
	within_crate_problem_index = chamber_index;
    }
    //
    if (within_crate_problem_index < 0) continue;  // this is a CCB or MPC.  skip it for the time being
    //
    Chamber * thisChamber = chamberVector[within_crate_problem_index];
    //
    OutputCheckFirmware << thisCrate->GetLabel()            << ", ";
    OutputCheckFirmware << thisChamber->GetLabel()          << ", ";
    OutputCheckFirmware << problem_slot                     << ", ";
    OutputCheckFirmware << component_string[problem_index]  << ", ";
    OutputCheckFirmware << reason_for_reload[problem_index] << std::endl;
    //
  }
  // write the output file
  //
  // The peripheral crate labels have the convention:  VME[p,n]N_M.  Here we use 
  // the "p" or "n" to label which endcap we are checking the firmware status on...
  const char * crate_name = crateVector[0]->GetLabel().c_str();
  char endcap_side = crate_name[3];
  //
  // This file is hardcoded as FirmwareDir_/status_check/YEARMODA_HRMN_[p,n]_firmware_status.log
  char filename[200];
  sprintf(filename,"%s/status_check/%s_%c_firmware_status.log",FirmwareDir_.c_str(),date_and_time,endcap_side);
  //
  //  std::cout << "filename = " << filename << std::endl;
  //
  std::ofstream LogFileCheckFirmware;
  LogFileCheckFirmware.open(filename);
  LogFileCheckFirmware << OutputCheckFirmware.str() ;
  LogFileCheckFirmware.close();
  //
  firmware_checked_ = 1;
  SetCurrentCrate(initial_crate);
  //
  this->PowerOnFixCFEB(in,out);
}
//
void emu::pc::Configurator::PowerOnFixCFEB(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  // MyHeader(in,out,"CSC DOC daily checklist");
  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)", ( fsm_.getCurrentState() == 'E' ? 5 : -1 ) );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector", 
			"/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" )
	      .addLink( "Main Page", 
			"/" + getApplicationDescriptor()->getURN() + "/MainPage" )
	      .addLink( "Firmware Check (CSC DOC daily checklist)" )
	      );
  //
  int initial_crate = current_crate_;
  //
  std::string GoToMainPage = toolbox::toString("/%s/MainPage",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",GoToMainPage) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Go back to Yellow Page").set("style","color:black") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  char buf[200];
  //
  //  *out << cgicc::br();
  //  std::string SetNumberOfHardResets = toolbox::toString("/%s/SetNumberOfHardResets",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",SetNumberOfHardResets) << std::endl ;
  //  *out << cgicc::input().set("type","submit").set("value","Number of hard resets").set("style","color:black") << std::endl ;
  //  sprintf(buf,"%d",number_of_hard_resets_);
  //  *out << cgicc::input().set("type","text").set("value",buf).set("name","number_of_hard_resets") << std::endl ;
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::br();
  //  //
  //  if (number_of_hard_resets_ > 0) {
  //    *out << cgicc::br();
  //    *out << "WARNING, you are going to check the firmware " << number_of_checks_ 
  //	 << " times, sending " << number_of_hard_resets_ << " hard resets" << cgicc::br();
  //    *out << "---> DO NOT DO THIS IN THE MIDDLE OF A RUN <---" << cgicc::br();
  //    *out << cgicc::br();
  //  }
  //
  *out << cgicc::br();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("STEP 1) Check System Firmware").set("style","color:blue") << std::endl ;

  *out << cgicc::table().set("border","2");
  //
  *out << cgicc::tr();
  *out << cgicc::td().set("ALIGN","center");
  *out << cgicc::b(" --> DO NOT CHECK FIRMWARE DURING A GLOBAL RUN <-- ").set("style","color:red");
  *out << cgicc::td() << std::endl;
  *out << cgicc::tr() << std::endl;
  //
  *out << cgicc::tr();
  *out << cgicc::td().set("ALIGN","center");
  *out << cgicc::b(" --> STOP XMAS MONITORING BEFORE FIRMWARE CHECK <-- ").set("style","color:blue");
  *out << cgicc::td() << std::endl;
  *out << cgicc::tr() << std::endl;
  //
  *out << cgicc::tr();
  *out << cgicc::td().set("ALIGN","center");
  std::string CheckFirmware = toolbox::toString("/%s/CheckFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check firmware in DMBs, CFEBs and ALCTs").set("style","color:black") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td() << std::endl;
  *out << cgicc::tr() << std::endl;
  //
  *out << cgicc::table();
  //
  *out << cgicc::fieldset();
  //
  //
  *out << cgicc::br();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("STEP 2) Fix Firmware").set("style","color:blue") << std::endl ;
  //
  if (firmware_checked_ == 1) {
    //
    // list the problems found...
    //
    *out << "The following FPGAs showed problems..." << std::endl;
    *out << cgicc::table().set("border","1");
    *out << cgicc::tr();
    *out << cgicc::td() << " Crate "     << cgicc::td();
    *out << cgicc::td() << " Chamber "   << cgicc::td();
    *out << cgicc::td() << " Slot "      << cgicc::td();
    *out << cgicc::td() << " Component " << cgicc::td();
    *out << cgicc::td() << " Reason "    << cgicc::td();
    *out << cgicc::tr() << std::endl;
    //
    for (unsigned problem_index=0; problem_index<crate_to_reload.size(); problem_index++) {
      int problem_crate     = crate_to_reload[problem_index];
      int problem_slot      = slot_to_reload[problem_index];
      int problem_component = component_to_reload[problem_index];
      //
      SetCurrentCrate(problem_crate);
      //
      int within_crate_problem_index = -1;
      for (unsigned int chamber_index=0; chamber_index<dmbVector.size(); chamber_index++) {
	DAQMB * thisDMB = dmbVector[chamber_index];
        int dslot = thisDMB->slot();
	//
	TMB * thisTMB   = tmbVector[chamber_index];
	int tslot = thisTMB->slot();
	//
	if (dslot == problem_slot || tslot == problem_slot) 
	  within_crate_problem_index = chamber_index;
      }
      //
      if (within_crate_problem_index < 0) continue;  // this is a CCB or MPC.  skip it for the time being
      //
      Chamber * thisChamber = chamberVector[within_crate_problem_index];
      //
      *out << cgicc::tr();
      *out << cgicc::td() << thisCrate->GetLabel()            << cgicc::td();
      *out << cgicc::td() << thisChamber->GetLabel()          << cgicc::td();
      *out << cgicc::td() << problem_slot                     << cgicc::td();
      *out << cgicc::td() << component_string[problem_index]  << cgicc::td();
      //
      // has the expert already acknowledged that this is a problem?
      bool known_problem = false;
      //
      if (thisChamber->GetExpectedConfigProblemTMB()   && problem_component == TMB_LABEL         ) known_problem = true;
      if (thisChamber->GetExpectedConfigProblemALCT()  && problem_component == ALCT_LABEL        ) known_problem = true;
      if (thisChamber->GetExpectedConfigProblemDMB()   &&(problem_component == DMB_CONTROL_LABEL||
							  problem_component == DMB_VME_LABEL)    ) known_problem = true;
      if (thisChamber->GetExpectedConfigProblemCFEB1() && problem_component == CFEB_LABEL[0]     ) known_problem = true;
      if (thisChamber->GetExpectedConfigProblemCFEB2() && problem_component == CFEB_LABEL[1]     ) known_problem = true;
      if (thisChamber->GetExpectedConfigProblemCFEB3() && problem_component == CFEB_LABEL[2]     ) known_problem = true;
      if (thisChamber->GetExpectedConfigProblemCFEB4() && problem_component == CFEB_LABEL[3]     ) known_problem = true;
      if (thisChamber->GetExpectedConfigProblemCFEB5() && problem_component == CFEB_LABEL[4]     ) known_problem = true;
      //
      if (known_problem) { 	// The expert has acknowledged this problem.  Label it as such...
	//
	*out << cgicc::td() << "Known problem" << cgicc::td();
	//
      } else {                  // The expert has NOT acknowledged this problem.  It is new and allow the user to fix it...
	//
	*out << cgicc::td() << reason_for_reload[problem_index] << cgicc::td();
	//

	*out << cgicc::td();
	std::string FixCFEB = toolbox::toString("/%s/FixCFEB",getApplicationDescriptor()->getURN().c_str());
	*out << cgicc::form().set("method","GET").set("action",FixCFEB) << std::endl ;
	if (loaded_ok[problem_index] < 0) {
	  *out << cgicc::input().set("type","submit").set("value","Load Firmware").set("style","color:blue") << std::endl ;
  } else if (loaded_ok[problem_index] == 0) {
	  *out << cgicc::input().set("type","submit").set("value","Load Firmware").set("style","color:green") << std::endl ;
	} else if (loaded_ok[problem_index] > 0) {
	  *out << cgicc::input().set("type","submit").set("value","Load Firmware").set("style","color:red") << std::endl ;
	}

  *out << "N readbacks" << std::endl;
  sprintf(buf,"%d",number_of_fw_readbacks);
  *out << cgicc::input().set("type","text").set("value",buf).set("name","readback") << std::endl ;
	sprintf(buf,"%d",problem_crate);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ncrt");
	sprintf(buf,"%d",within_crate_problem_index);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ndmb");
	sprintf(buf,"%d",problem_component);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ncfeb");
	sprintf(buf,"%d",problem_index);
	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ncmd"); 
	*out << cgicc::form() << std::endl ;
	*out << cgicc::td();
	//
	//	*out << cgicc::td();
	//	*out << cgicc::form().set("method","GET").set("action",FixCFEB) << std::endl ;
	//	*out << cgicc::input().set("type","submit").set("value","CCB Hard Reset").set("style","color:black") << std::endl ;
	//	sprintf(buf,"%d",problem_crate);
	//	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ncrt");
	//	sprintf(buf,"%d",within_crate_problem_index);
	//	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ndmb");
	//	sprintf(buf,"%d",problem_component);
	//	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ncfeb");
	//	int ccb_hard_reset = -1;
	//	sprintf(buf,"%d",ccb_hard_reset);
	//	*out << cgicc::input().set("type","hidden").set("value",buf).set("name","ncmd"); 
	//	*out << cgicc::form() << std::endl ;
	//	*out << cgicc::td();
      }
      //
      *out << cgicc::tr() << std::endl;
    }
    *out << cgicc::table() << std::endl;
  }
  //
  *out << cgicc::fieldset();
  //
  *out << cgicc::br();
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("STEP 3) Check Crate Configuration").set("style","color:blue") << std::endl ;

  *out << cgicc::table().set("border","2");
  //
  *out << cgicc::tr();
  *out << cgicc::td().set("ALIGN","center");
  std::string CheckCratesConfigurationFull = toolbox::toString("/%s/CheckCratesConfigurationFull",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckCratesConfigurationFull) << std::endl ;
  if (all_crates_ok == 1) {
    *out << cgicc::input().set("type","submit").set("value","Check full configuration of crates").set("style","color:green") << std::endl ;
  } else if (all_crates_ok == 0) {
    *out << cgicc::input().set("type","submit").set("value","Check full configuration of crates").set("style","color:red") << std::endl ;
  } else if (all_crates_ok == -1) {
    *out << cgicc::input().set("type","submit").set("value","Check full configuration of crates").set("style","color:blue") << std::endl ;
  }
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  *out << cgicc::tr() << std::endl;
  //
  *out << cgicc::table();
  //
  if (print_config_check_output) {
    //
    *out                     << "date_time = " << date_and_time_  << cgicc::br() << std::endl;
    OutputCheckConfiguration << "date_time = " << date_and_time_                << std::endl;
    //
  }
  //
  if (all_crates_ok >= 0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      //
      SetCurrentCrate(crate_number);
      //
      if ( crate_check_ok[current_crate_]==0 ) {
	//
	//    OutputCheckConfiguration << "Problem summary for Crate " << thisCrate->GetLabel() << "..." << std::endl;
	//
	if( !(ccb_check_ok[current_crate_]) ) {
	  *out                     << thisCrate->GetLabel() << "<span style=\"color:red\" >, CCB config fail </span>" << cgicc::br() << std::endl;
	  OutputCheckConfiguration << thisCrate->GetLabel() << ", CCB"                << std::endl;
	}
	//
	if( !(mpc_check_ok[current_crate_]) ) {
	  *out                     << thisCrate->GetLabel() << "<span style=\"color:red\" >, MPC config fail </span>" << cgicc::br() << std::endl;
	  OutputCheckConfiguration << thisCrate->GetLabel() << ", MPC"                << std::endl;
	}
	//
	for (unsigned int chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
	  //
	  if( tmb_check_ok[current_crate_][chamber_index]  > 0  ||
	      alct_check_ok[current_crate_][chamber_index] > 0  ||
	      dmb_check_ok[current_crate_][chamber_index]  > 0 ) {
	    //
	    *out << "<span style=\"color:black\" >";
	    *out                     << thisCrate->GetLabel() << ", " << (chamberVector[chamber_index]->GetLabel()).c_str();
	    OutputCheckConfiguration << thisCrate->GetLabel() << ", " << (chamberVector[chamber_index]->GetLabel()).c_str();
	    *out << "</span>";
	    //
	    bool print_description = false;
	    //
	    if( tmb_check_ok[current_crate_][chamber_index] > 0) {
	      //
	      OutputCheckConfiguration << ", TMB, " << tmb_check_ok[current_crate_][chamber_index];
	      //
	      if( tmb_check_ok[current_crate_][chamber_index]        == 1) {
		*out << "<span style=\"color:red\" > TMB config fail, </span>";
	      } else if( tmb_check_ok[current_crate_][chamber_index] == 2) {
		*out << "<span style=\"color:black\" > expected TMB config fail, </span>";
		print_description = true;
	      } else if( tmb_check_ok[current_crate_][chamber_index] == 3) {
		*out << "<span style=\"color:blue\" > did not see expected TMB config failure, </span>";
		print_description = true;
	      } else if( tmb_check_ok[current_crate_][chamber_index] == 4) {
		*out << "<span style=\"color:red\" > TMB FPGA did not program, </span>";
	      } 
	      //
	    } else {
	      //
	      *out                     << ", , ";
	      OutputCheckConfiguration << ", , ";
	    }
	    //
	    if( alct_check_ok[current_crate_][chamber_index] > 0) {
	      //
	      OutputCheckConfiguration << ", ALCT, " << alct_check_ok[current_crate_][chamber_index];
	      //
	      if( alct_check_ok[current_crate_][chamber_index]        == 1) {
		*out << "<span style=\"color:red\" > ALCT config fail, </span>";
	      } else if( alct_check_ok[current_crate_][chamber_index] == 2) {
		*out << "<span style=\"color:black\" > expected ALCT config fail, </span>";
		print_description = true;
	      } else if( alct_check_ok[current_crate_][chamber_index] == 3) {
		*out << "<span style=\"color:blue\" > did not see expected ALCT config failure, </span>";
		print_description = true;
	      } else if( alct_check_ok[current_crate_][chamber_index] == 4) {
		*out << "<span style=\"color:red\" > ALCT FPGA did not program, </span>";
	      } 
	      //
	    } else {
	      //
	      *out                     << ", , ";
	      OutputCheckConfiguration << ", , ";
	    }
	    //
	    if( dmb_check_ok[current_crate_][chamber_index] > 0) {
	      //
	      OutputCheckConfiguration << ", DMB, " << dmb_check_ok[current_crate_][chamber_index];
	      //
	      if( dmb_check_ok[current_crate_][chamber_index]        == 1) {
		*out << "<span style=\"color:red\" > DMB config fail, </span>";
	      } else if( dmb_check_ok[current_crate_][chamber_index] == 2) {
		*out << "<span style=\"color:black\" > expected DMB config fail, </span>";
		print_description = true;
	      } else if( dmb_check_ok[current_crate_][chamber_index] == 3) {
		*out << "<span style=\"color:blue\" > did not see expected DMB config failure, </span>";
		print_description = true;
	      } else if( dmb_check_ok[current_crate_][chamber_index] == 4) {
		*out << "<span style=\"color:red\" > DMB FPGA did not program, </span>";
	      } 
	      //
	    } else {
	      //
	      *out                     << ", , ";
	      OutputCheckConfiguration << ", , ";
	    }
	    //
	    if (print_description) {
	      *out                     << ", " << (chamberVector[chamber_index]->GetProblemDescription()).c_str();
	      OutputCheckConfiguration << ", " << (chamberVector[chamber_index]->GetProblemDescription()).c_str();
	    } else {
	      *out                     << ", ";
	      OutputCheckConfiguration << ", ";
	    }
	    //
	    *out                     << cgicc::br() << std::endl;
	    OutputCheckConfiguration                << std::endl;
	    //
	  } 
	} // end chamber for-loop
      } else if (crate_check_ok[current_crate_] == -1) {
	//
	*out << cgicc::span().set("style","color:red");
	*out                     << crateVector[crate_number]->GetLabel() << " Not checked" << cgicc::br() << cgicc::span() << std::endl ;
	OutputCheckConfiguration << crateVector[crate_number]->GetLabel() << " Not checked" << std::endl;
      }
    }
  }
  *out << cgicc::fieldset();
  //
  if (print_config_check_output) {
    //
    //Output the errors to a file...
    //
    // The peripheral crate labels have the convention:  VME[p,n]N_M.  Here we use 
    // the "p" or "n" to label which endcap we are checking the firmware status on...
    const char * crate_name = crateVector[0]->GetLabel().c_str();
    char endcap_side = crate_name[3];
    //
    // This file is hardcoded as FirmwareDir_/status_check/YEARMODA_HRMN_[p,n]_firmware_status.log
    char filename[200];
    sprintf(filename,"%s/status_check/%s_%c_configuration_check.log",FirmwareDir_.c_str(),date_and_time_,endcap_side);
    //
    //  std::cout << "filename = " << filename << std::endl;
    //
    std::ofstream LogFileCheckConfiguration;
    LogFileCheckConfiguration.open(filename);
    LogFileCheckConfiguration << OutputCheckConfiguration.str() ;
    LogFileCheckConfiguration.close();
    //
  }
  print_config_check_output = false;
  OutputCheckConfiguration.clear();
  //
  SetCurrentCrate(initial_crate);
  //
  pageFooter( in, out );
  htmlFooter( in, out );
}
//
void emu::pc::Configurator::FixCFEB(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  std::cout << "Entered FixCFEB" << std::endl;
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name0 = cgi.getElement("readback");
  int n_readbacks = 0;
/* disable DMB/CFEB PROM readback by letting n_readbacks always 0 */
//
//  if(name0 != cgi.getElements().end()) {
//    n_readbacks = cgi["readback"]->getIntegerValue();
//    std::cout << "readback = " << n_readbacks << std::endl;
//  }

	cgicc::form_iterator name1 = cgi.getElement("ncrt");
  int crate_index;
  if(name1 != cgi.getElements().end()) {
    crate_index = cgi["ncrt"]->getIntegerValue();
    std::cout << "crate_index = " << crate_index << std::endl;
  } else {
    std::cout << "No crate_index" << std::endl ;
    crate_index=-1;
  }
  cgicc::form_iterator name2 = cgi.getElement("ndmb");
  int chamber_index;
  if(name2 != cgi.getElements().end()) {
    chamber_index = cgi["ndmb"]->getIntegerValue();
    std::cout << "chamber_index = " << chamber_index << std::endl;
  } else {
    std::cout << "No chamber_index" << std::endl ;
    chamber_index=-1;
  }
  cgicc::form_iterator name3 = cgi.getElement("ncfeb");
  int problem_component;
  if(name3 != cgi.getElements().end()) {
    problem_component = cgi["ncfeb"]->getIntegerValue();
    std::cout << "problem_component = " << problem_component << std::endl;
  } else {
    std::cout << "No problem_component" << std::endl ;
    problem_component=-1;
  }
  //
  cgicc::form_iterator name4 = cgi.getElement("ncmd");
  int problem_index;
  if(name4 != cgi.getElements().end()) {
    problem_index = cgi["ncmd"]->getIntegerValue();
    std::cout << "problem_index = " << problem_index << std::endl;
  } else {
    std::cout << "No problem_index" << std::endl ;
    problem_index=-2;
  }
  //
  //
  int initial_crate = current_crate_;
  //
  if(crate_index>-1 && chamber_index>-1 && problem_component>-1 && problem_index>-1){
    //
    SetCurrentCrate(crate_index);
    //
    if (problem_component == CCB_LABEL) {
      //
      //
    } else if (problem_component == MPC_LABEL) {
      //
      //
    } else if (problem_component == TMB_LABEL) {
      //
      //
    } else if (problem_component == ALCT_LABEL) {
      //
      TMB * thisTMB = tmbVector[chamber_index];
      ALCTController  * thisALCT = thisTMB->alctController();
      //
      if (!thisALCT) {
	std::cout << "This ALCT not defined" << std::endl;
	SetCurrentCrate(initial_crate);
	this->PowerOnFixCFEB(in,out);
      }
      //
      int config_check = thisALCT->CheckFirmwareConfiguration();
      //
      if (config_check == 0 || config_check > 1) {
	std::cout << "----------------------------------------------------------------" << std::endl;
	std::cout << "---- ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR------" << std::endl;
	std::cout << "---- Firmware database check did not pass for this crate. ------" << std::endl;
	std::cout << "---- ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR------" << std::endl;
	std::cout << "----------------------------------------------------------------" << std::endl;
	SetCurrentCrate(initial_crate);
	this->PowerOnFixCFEB(in,out);
      }
      //
      // to read the ALCT's PROM content and save as a .mcs file

      std::string chambername= thisCrate->GetChamber(thisTMB)->GetLabel();
      unsigned t = chambername.find('/');
      unsigned s = chambername.size();
      while(t<=s )
      {
         chambername.replace(t,1,"_");
         t = chambername.find('/');
      }
      std::string mcsfile="/tmp/ALCT_"+ chambername + "_" + emu::utils::getDateTime(true) + ".mcs";
      std::string jtagfile;
      std::string alcttype=thisALCT->GetChamberType();
      if(alcttype=="ME21" || alcttype=="ME31" || alcttype=="ME41")
         jtagfile=XMLDIR+"/alct_big.vrf";
      else
         jtagfile=XMLDIR+"/alct_small.vrf";
      //

      // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during ALCT downloading...
      thisCCB->setCCBMode(CCB::VMEFPGA);
      std::cout  << emu::utils::getDateTime() << " Reading back ALCT PROM from slot " << thisTMB->slot() << std::endl;
      //
      thisTMB->setup_jtag(ChainAlctFastMezz);
      thisTMB->read_prom(jtagfile.c_str(),mcsfile.c_str());
      //
      LOG4CPLUS_INFO(getApplicationLogger(), "Program ALCT firmware");
      //
      std::cout <<  "Loading ALCT firmware to slot " << thisTMB->slot() 
		<< " with " << ALCTFirmware_[chamber_index].toString() 
		<< " in 5 seconds...  Current firmware types are:" << std::endl;
      //
      thisALCT->ReadSlowControlId();
      thisALCT->PrintSlowControlId();
      //
      thisALCT->ReadFastControlId();
      thisALCT->PrintFastControlId();
      //
      ::sleep(5);
      //
      thisTMB->disableAllClocks();
      //
      thisTMB->SetXsvfFilename(ALCTFirmware_[chamber_index].toString().c_str());
      thisALCT->ProgramALCTProms();
      thisTMB->ClearXsvfFilename();
      loaded_ok[problem_index] = thisTMB->GetNumberOfVerifyErrors();
      //
      // programming with svf file to be deprecated, since it cannot verify...
      //  int debugMode(0);
      //  int jch(3);
      //  int status = thisALCT->SVFLoad(&jch,ALCTFirmware_[chamber_index].toString().c_str(),debugMode);
      //
      thisTMB->enableAllClocks();
      //
      if (loaded_ok[problem_index] >= 0){
	LOG4CPLUS_INFO(getApplicationLogger(), "Program ALCT firmware finished");
	std::cout << "=== Programming finished"<< std::endl;
	std::cout << "=== " << loaded_ok[problem_index] << " Verify Errors  occured" << std::endl;
      } else {
	std::cout << "=== Fatal Error. Exiting with " << loaded_ok[problem_index] << std::endl;
      }
      //
      // Put CCB back into DLOG mode to listen to TTC commands...
      thisCCB->setCCBMode(CCB::DLOG);
      //
      thisCCB->hardReset(); 
      //
    } else if (problem_component == DMB_VME_LABEL) {
      //
      DAQMB * thisDMB = dmbVector[chamber_index];
      std::cout << "DMB Load VME Firmware in slot " << thisDMB->slot() << std::endl;
      //
      if (thisDMB && thisDMB->slot()!=25) {
	//
	thisCCB->hardReset();
	//
	std::string crate=thisCrate->GetLabel();
	int slot=thisDMB->slot();
	int dmbID=brddb->CrateToDMBID(crate,slot);
	//
	unsigned short int dword[2];	
	dword[0]=dmbID&0x03ff;
	dword[1]=0xDB00;
	std::cout<<" The DMB number is set to: " << dword[0] << " from database lookup: " << dmbID << std::endl;
	char * outp=(char *)dword;
	//  
	for(int readback=0; readback<n_readbacks; readback++){
     unlink("/tmp/eprom.bit");
     thisDMB->epromread(VPROM);
     std::ostringstream logs;
     int erropen = thisDMB->check_eprom_readback("/tmp/eprom.bit",DMBVmeCompare_.toString().c_str()); // hardcoded file name; bad, but I didn't start it //KK
     if(erropen>=0){
        logs<<" Total number of bad bits: "<<thisDMB->GetNumberOfBadReadbackBits()<<std::endl;
        for(unsigned int bit=0; bit<thisDMB->GetNumberOfBadReadbackBits() && bit<20; bit++ ){
           logs << " broken word position: " << std::setw(6) << thisDMB->GetWordWithBadReadbackBit(bit)
                << ", bad bit position: " << thisDMB->GetBadReadbackBitPosition(bit)
                << ", bad bit type (type=0 1->0 type=1 0->1): "<< thisDMB->GetBadReadbackBitType(bit)
                << std::endl;
        }
        if( thisDMB->GetNumberOfBadReadbackBits()>20 ) logs << "  only first 20 bad CFEB firmware bits were reported above " << std::endl;
     } else {
        logs << " file error in check_eprom_readback" << std::endl;
     }
    LOG4CPLUS_INFO(getApplicationLogger(), logs.str());
	}

	thisDMB->epromload(RESET,DMBVmeFirmware_.toString().c_str(),1,outp);  // load mprom
	//
	::sleep(1);
	thisCCB->hardReset(); //disable this when testing the random_trigger
      }
      loaded_ok[problem_index] = 0;
      //
    } else if (problem_component == DMB_CONTROL_LABEL) {
      //
      DAQMB * thisDMB = dmbVector[chamber_index];
      std::cout << "DMB Load Control FPGA Firmware in slot " << thisDMB->slot() << std::endl;
      //
      if (thisDMB && thisDMB->slot()!=25) {
	//
	thisCCB->hardReset();
	//
	unsigned short int dword[2];
	dword[0]=0;
	char *outp=(char *)dword;
	//
	for(int readback=0; readback<n_readbacks; readback++){
     unlink("/tmp/eprom.bit");
     thisDMB->epromread(MPROM);
     std::ostringstream logs;
     int erropen = thisDMB->check_eprom_readback("/tmp/eprom.bit",DMBCompare_.toString().c_str()); // hardcoded file name; bad, but I didn't start it //KK
     if(erropen>=0){
        logs<<" Total number of bad bits: "<<thisDMB->GetNumberOfBadReadbackBits()<<std::endl;
        for(unsigned int bit=0; bit<thisDMB->GetNumberOfBadReadbackBits() && bit<20; bit++ ){
           logs << " broken word position: " << std::setw(6) << thisDMB->GetWordWithBadReadbackBit(bit)
                << ", bad bit position: " << thisDMB->GetBadReadbackBitPosition(bit)
                << ", bad bit type (type=0 1->0 type=1 0->1): "<< thisDMB->GetBadReadbackBitType(bit)
                << std::endl;
        }
        if( thisDMB->GetNumberOfBadReadbackBits()>20 ) logs << "  only first 20 bad CFEB firmware bits were reported above " << std::endl;
     } else {
        logs << " file error in check_eprom_readback" << std::endl;
     }
    LOG4CPLUS_INFO(getApplicationLogger(), logs.str());
	}

	thisDMB->epromload(MPROM,DMBFirmware_.toString().c_str(),1,outp);  // load mprom
	//
	::sleep(5);
	thisCCB->hardReset();
      }
      loaded_ok[problem_index] = 0;
      //
    } else if (problem_component == CFEB_LABEL[0] ||
	       problem_component == CFEB_LABEL[1] ||
	       problem_component == CFEB_LABEL[2] ||
	       problem_component == CFEB_LABEL[3] ||
	       problem_component == CFEB_LABEL[4] ) {
      //
      int cfeb_index = problem_component - CFEB_LABEL[0];      // begin CFEB indexing at 0
      //
      DAQMB * thisDMB = dmbVector[chamber_index];
      std::vector<CFEB> thisCFEBs = thisDMB->cfebs();
      //
      unsigned short int dword[2];
      for (unsigned int i=0; i<thisCFEBs.size(); i++) {
	if (thisCFEBs[i].number() == cfeb_index ) {
	  dword[0]=thisDMB->febpromuser(thisCFEBs[i]);
	  CFEBid_[chamber_index][i] = dword[0];  // fill summary file with user ID value read from this CFEB
	  char * outp=(char *)dword;   // recast dword
		for(int readback=0; readback<n_readbacks; readback++){
          unlink("/tmp/eprom.bit");
          thisDMB->epromread(thisCFEBs[i].promDevice());
          std::ostringstream logs;
          int erropen = thisDMB->check_eprom_readback("/tmp/eprom.bit",CFEBCompare_.toString().c_str()); // hardcoded file name; bad, but I didn't start it //KK
          if(erropen>=0){
             logs<<" Total number of bad bits: "<<thisDMB->GetNumberOfBadReadbackBits()<<std::endl;
             for(unsigned int bit=0; bit<thisDMB->GetNumberOfBadReadbackBits() && bit<20; bit++ ){
                logs << " broken word position: " << std::setw(6) << thisDMB->GetWordWithBadReadbackBit(bit)
                     << ", bad bit position: " << thisDMB->GetBadReadbackBitPosition(bit)
                     << ", bad bit type (type=0 1->0 type=1 0->1): "<< thisDMB->GetBadReadbackBitType(bit)
                     << std::endl;
             }
             if( thisDMB->GetNumberOfBadReadbackBits()>20 ) logs << "  only first 20 bad CFEB firmware bits were reported above " << std::endl;
          } else {
             logs << " file error in check_eprom_readback" << std::endl;
          }
          LOG4CPLUS_INFO(getApplicationLogger(), logs.str());
		}

		thisDMB->epromload(thisCFEBs[i].promDevice(),CFEBFirmware_.toString().c_str(),1,outp);  // load mprom
		thisCCB->hardReset(); 
	}
      }
      loaded_ok[problem_index] = 0;
      //    
    }
  }  else if(problem_index == -1){
    //
    SetCurrentCrate(crate_index);
    thisCCB->hardReset(); 
    //
  }
  //
  SetCurrentCrate(initial_crate);
  //
  //
  //    if(ncmd==2){
  //      // now readback bit contents of prom
  //      char * outp="....";    // recast dword
  //      thisDMB->epromload_verify(thisCFEB.promDevice(),CFEBVerify_.toString().c_str(),1,outp);  // load mprom
  //      std::cout << " time calculation " << std::endl;
  //      time_t rawtime;
  //      time(&rawtime);
  //      std::string buf;
  //      std::string time_dump = ctime(&rawtime);
  //      std::string time = time_dump.substr(0,time_dump.length()-1);
  //      while( time.find(" ",0) != std::string::npos ) {
  //        int thispos = time.find(" ",0);
  //        time.replace(thispos,1,"_");
  //      }
  //      std::cout << "time " << time << std::endl;
  //      std::string temp = toolbox::toString("mv eprom.bit /tmp/verify_%s_slot%d_cfeb%d_%s.bit",crateVector[crate_index]->GetLabel().c_str(),thisDMB->slot(),thisCFEB.number()+1,time.c_str());
  //      std::cout  << temp << std::endl;
  //      system(temp.c_str());
  
  // now reprogram the prom
  //      unsigned short int dword[2];
  //    dword[0]=thisDMB->febpromuser(thisCFEB);
  //    char * outp2=(char *)dword;   // recast dword
  //    thisDMB->epromload(thisCFEB.promDevice(),CFEBFirmware_.toString().c_str(),1,outp2);
  //    // now do a hard reset
  //    thisDMB->lowv_onoff(0x20);
  //    usleep(500000);
  //    thisDMB->lowv_onoff(0x3f);
  //
  this->PowerOnFixCFEB(in,out);
}
//
void emu::pc::Configurator::ReadbackALCTFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  std::cout << "Readback ALCT..." << std::endl;
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name1 = cgi.getElement("ncrt");
  int ncrt;
  if(name1 != cgi.getElements().end()) {
    ncrt = cgi["ncrt"]->getIntegerValue();
    std::cout << "ncrt " << ncrt << std::endl;
  } else {
    std::cout << "No ncrt specified, taking the current crate..." << std::endl ;
    ncrt=current_crate_;
  }
  //
  cgicc::form_iterator name2 = cgi.getElement("ntmb");
  int tmb;
  if(name2 != cgi.getElements().end()) {
    tmb = cgi["ntmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
  } else {
    std::cout << "No TMB" << std::endl ;
    tmb=-1;
  }
  //
  int initial_crate = current_crate_;
  //
  if(ncrt>-1 && tmb>-1){
    //
    SetCurrentCrate(ncrt);
    TMB * thisTMB = tmbVector[tmb];
    ALCTController * thisALCT = thisTMB->alctController();
    //
    if (thisTMB->slot() < 22) {
      //
      // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during ALCT downloading...
      thisCCB->setCCBMode(CCB::VMEFPGA);
      //
      std::cout <<  "Reading back ALCT firmware from slot " << thisTMB->slot() 
		<< ", using " << ALCTReadback_[tmb].toString() 
		<< " in 5 seconds...  Current firmware types are:" << std::endl;
      //
      thisALCT->ReadSlowControlId();
      thisALCT->PrintSlowControlId();
      //
      thisALCT->ReadFastControlId();
      thisALCT->PrintFastControlId();
      //
      ::sleep(5);
      //
      thisTMB->disableAllClocks();
      //
      thisTMB->SetXsvfFilename(ALCTReadback_[tmb].toString().c_str());
      thisALCT->ProgramALCTProms();
      thisTMB->ClearXsvfFilename();
      //
      thisTMB->enableAllClocks();
      //
      // Put CCB back into DLOG mode to listen to TTC commands...
      thisCCB->setCCBMode(CCB::DLOG);
      //
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  this->PowerOnFixCFEB(in,out);
}

//
// Another method which would be better in another class... let's make it work, first....
void emu::pc::Configurator::CheckPeripheralCrateFirmware() {
  //
  //  std::cout << "Firmware check for " << thisCrate->GetLabel() << std::endl;
  //
  crate_firmware_ok[current_crate_] = 1;
  //
  ccb_firmware_ok[current_crate_] = thisCrate->ccb()->CheckFirmwareDate();
  crate_firmware_ok[current_crate_] &= ccb_firmware_ok[current_crate_];  
  //
  mpc_firmware_ok[current_crate_] = thisCrate->mpc()->CheckFirmwareDate();
  crate_firmware_ok[current_crate_] &= mpc_firmware_ok[current_crate_];  
  //
  for (unsigned int chamber_index=0; chamber_index<(tmbVector.size()<9?tmbVector.size():9) ; chamber_index++) {
    //	
    TMB * thisTMB             = tmbVector[chamber_index];
    ALCTController * thisALCT = thisTMB->alctController();
    DAQMB * thisDMB           = dmbVector[chamber_index];
    //
    //    Chamber * thisChamber     = chamberVector[chamber_index];
    //    std::cout << "Firmware check for " << thisCrate->GetLabel() << ", " << (thisChamber->GetLabel()).c_str() << std::endl;
    //
    tmb_firmware_ok[current_crate_][chamber_index]      = (int) thisTMB->CheckFirmwareDate();
    crate_firmware_ok[current_crate_] &= tmb_firmware_ok[current_crate_][chamber_index];
    //
    alct_firmware_ok[current_crate_][chamber_index]     = (int) thisALCT->CheckFirmwareDate();
    crate_firmware_ok[current_crate_] &= alct_firmware_ok[current_crate_][chamber_index];
    //
    dmb_vme_firmware_ok[current_crate_][chamber_index]  = (int) thisDMB->CheckVMEFirmwareVersion();
    crate_firmware_ok[current_crate_] &= dmb_vme_firmware_ok[current_crate_][chamber_index];
    //
    dmb_control_firmware_ok[current_crate_][chamber_index] = (int) thisDMB->CheckControlFirmwareVersion();
    crate_firmware_ok[current_crate_] &= dmb_control_firmware_ok[current_crate_][chamber_index];
    //
    std::vector<CFEB> cfebs = thisDMB->cfebs() ;
    //
    for(CFEBItr cfebItr = cfebs.begin(); cfebItr != cfebs.end(); ++cfebItr) {
      //
      int cfeb_index = (*cfebItr).number();
      //
      cfeb_firmware_ok[current_crate_][chamber_index][cfeb_index] = (int) thisDMB->CheckCFEBFirmwareVersion(*cfebItr);
      crate_firmware_ok[current_crate_] &= cfeb_firmware_ok[current_crate_][chamber_index][cfeb_index];
    }
    //
  }
  //
  return;
}
//
//
////////////////////////////////////////////////////////////////////////////////////
// Expert Tools
////////////////////////////////////////////////////////////////////////////////////
void emu::pc::Configurator::ExpertToolsPage(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  int initial_crate = current_crate_;
  //
  char Name[100];
  sprintf(Name,"Expert Tools Page");
  //
  //MyHeader(in,out,Name);
  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)", ( fsm_.getCurrentState() == 'E' ? 5 : -1 ) );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector", 
			"/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" )
	      .addLink( "Main Page", 
			"/" + getApplicationDescriptor()->getURN() + "/MainPage" )
	      .addLink( "Expert Tools Page" )
	      );
  //
  *out << cgicc::h2("!!!Do NOT click these buttons during a GLOBAL RUN!!!");
  *out << cgicc::br();
  //
  //  ///////////////////////
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Special test configuration").set("style","color:blue") << std::endl ;
  //
  *out << cgicc::table().set("border","0");
  //
  *out << cgicc::td();
  std::string SetRadioactivityTrigger = toolbox::toString("/%s/SetRadioactivityTrigger",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",SetRadioactivityTrigger) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Enable single-layer trigger") << std::endl ;
  *out << cgicc::form() << std::endl ;;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string SetTwoLayerTriggerForSystem = toolbox::toString("/%s/SetTwoLayerTriggerForSystem",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",SetTwoLayerTriggerForSystem) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Enable two-layer trigger") << std::endl ;
  *out << cgicc::form() << std::endl ;;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string SetRadioactivityTriggerALCTOnly = toolbox::toString("/%s/SetRadioactivityTriggerALCTOnly",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",SetRadioactivityTriggerALCTOnly) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Enable single-layer trigger - ALCT only") << std::endl ;
  *out << cgicc::form() << std::endl ;;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string SetTTCDelays = toolbox::toString("/%s/SetTTCDelays",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",SetTTCDelays) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","SetTTCDelays") << std::endl ;
  *out << cgicc::form() << std::endl ;;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string RdVfyCFEBVirtexExpT = toolbox::toString("/%s/RdVfyCFEBVirtexExpT",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",RdVfyCFEBVirtexExpT) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check CFEB FPGAs") << std::endl ;
  *out << cgicc::form() << std::endl ;;
  *out << cgicc::td();
  //
  *out << cgicc::table() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Result of CFEB firmware check").set("style","color:blue") << std::endl ;
	if (total_bad_cfeb_bits >= 0) {
		*out << "CFEB FPGA check:  Total bad bits =  " << std::dec << total_bad_cfeb_bits;
		*out << ", total good bits = " << total_good_cfeb_bits;
		*out << cgicc::br() << std::endl;
		*out << "See latest cfebvirtex_check.log in ~/firmware/status_check for more details";
		*out << cgicc::br() << std::endl;
		total_bad_cfeb_bits = -1;	// Display only once.
	}
	*out << cgicc::br() << std::endl;
	*out << cgicc::fieldset();
	//
  *out << cgicc::br();
  //  ///////////////////////
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Determine online synchronization parameters").set("style","color:blue") << std::endl ;
  //
  *out << cgicc::table().set("border","0");
  //
  *out << cgicc::td();
  std::string MeasureALCTTMBRxTxForSystem = toolbox::toString("/%s/MeasureALCTTMBRxTxForSystem",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureALCTTMBRxTxForSystem) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find ALCT rx/tx") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string MeasureCFEBTMBRxForSystem = toolbox::toString("/%s/MeasureCFEBTMBRxForSystem",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureCFEBTMBRxForSystem) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find CFEB rx") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string ALCTBC0ScanForSystem = toolbox::toString("/%s/ALCTBC0ScanForSystem",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTBC0ScanForSystem) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Synchronize ALCT BC0") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string MeasureL1AsAndDAVsForSystem = toolbox::toString("/%s/MeasureL1AsAndDAVsForSystem",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureL1AsAndDAVsForSystem) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Find L1As and DAVs") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td();
  std::string QuickScanForSystem = toolbox::toString("/%s/QuickScanForSystem",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",QuickScanForSystem) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","L1As and DAVs for TOF parameters only") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::table() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
  *out << cgicc::br();
  //
  //
  //  ///////////////////////
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Test MPC to SP links").set("style","color:blue") << std::endl ;
  //
  if(prbs_test_) {
    std::string StopPRBS = toolbox::toString("/%s/StopPRBS",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",StopPRBS) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Stop PRBS test").set("style","color:red") << std::endl ;
    *out << cgicc::form() << std::endl ;;
  } else {
    std::string StartPRBS = toolbox::toString("/%s/StartPRBS",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",StartPRBS) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Start PRBS test").set("style","color:blue") << std::endl ;
    *out << cgicc::form()<< std::endl ;;
  }
  //
  *out << cgicc::fieldset();
  //
  *out << cgicc::br();
  //
  //  ///////////////////////
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  *out << cgicc::legend("Actions spanning the full system").set("style","color:blue") 
       << std::endl ;
  //
  *out << cgicc::table().set("border","0");
  //
  *out << cgicc::td();
  std::string MeasureAllTMBVoltages = toolbox::toString("/%s/MeasureAllTMBVoltages",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureAllTMBVoltages) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Measure All TMB Voltages") << std::endl ;
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::table() << std::endl ;
  //
  *out << cgicc::fieldset() << cgicc::br();
  //
  if(xml_or_db==1)
  {
    *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
    *out << cgicc::legend("Update In-Flash Config ID").set("style","color:red") << std::endl ;  
    *out << "Current In-Flash Config ID is: " << InFlash_config_ID << std::endl << cgicc::br();
    *out << "If click this button, the In-Flash Config ID in database will be updated to: " << Valid_config_ID << std::endl << cgicc::br() << cgicc::br();  
    std::string ChangeInFlashKey = toolbox::toString("/%s/UpdateInFlashKey",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",ChangeInFlashKey) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Update In-Flash Config ID") << std::endl ;
    *out << cgicc::form() << std::endl << cgicc::br() ;
    *out << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl << cgicc::br();
    *out << "After click this button, please make sure the affected Crate(s) are properly configured!!!" << std::endl << cgicc::br();  
    *out << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    *out << cgicc::fieldset();
  }
  SetCurrentCrate(initial_crate);
  //
  pageFooter( in, out );
  htmlFooter( in, out );
}

void emu::pc::Configurator::UpdateInFlashKey(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Update In-Flash ID" << std::endl;
  configuration_.recordTimeOfWritingFlash();

  // std::cout << emu::utils::getDateTime() << " In-Flash ID changed to: " << InFlash_config_ID << std::endl;
  //   // record this action in configuration database as WRITE FLASH
  //   // TO DO: really should treat this action as a different one, as this one doesn't update any hardware
  //   if(xml_or_db==1)
  //   {
  //       InFlash_config_ID = Valid_config_ID;
  //       try 
  //       {
  //          xdata::UnsignedInteger64 id_64 = atoi(Valid_config_ID.c_str());
  //          myTStore->writeFlashTime(id_64);
  //          std::cout << emu::utils::getDateTime() << " UPDATE IN-FLASH ID recorded in database. Configuration ID changed to: " << Valid_config_ID << std::endl;
  //       }
  //       catch( const std::exception & e )
  //       {
  //          std::cout << "Failed to write the configuration database!" << std::endl;
  //       }
  //   }
  
  //
  this->ExpertToolsPage(in, out);
}
//
void emu::pc::Configurator::StartPRBS(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Start PRBS Test" << std::endl;
  //
  if(total_crates_>0)
  {
     for(unsigned i=0; i< crateVector.size(); i++) {
        if ( crateVector[i]->IsAlive() ) crateVector[i]->mpc()->enablePRBS();
     }
     prbs_test_=true;
  }
  this->ExpertToolsPage(in, out);
}

void emu::pc::Configurator::StopPRBS(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Start PRBS Test" << std::endl;
  //
  if(total_crates_>0)
  {
     for(unsigned i=0; i< crateVector.size(); i++) {
        if ( crateVector[i]->IsAlive() ) crateVector[i]->mpc()->disablePRBS();
     }
     prbs_test_=false;
  }
  this->ExpertToolsPage(in, out);
}
//
void emu::pc::Configurator::SetRadioactivityTrigger(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Configure all crates to have the Radioactivity Trigger" << std::endl;
  //
  int initial_crate = current_crate_;
  //
  if(total_crates_>0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      if ( crateVector[crate_number]->IsAlive() ) {
	//
	SetCurrentCrate(crate_number);
	//
	for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	  //
	  // std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	  //
	  TMB * thisTMB = tmbVector[tmb];
	  ALCTController * thisALCT = thisTMB->alctController();
	  //
	  int initial_alct_nplanes_hit_pretrig = thisALCT->GetPretrigNumberOfLayers();
	  int initial_alct_nplanes_hit_pattern = thisALCT->GetPretrigNumberOfPattern();
	  thisALCT->SetPretrigNumberOfLayers(1);
	  thisALCT->SetPretrigNumberOfPattern(1);
	  thisALCT->WriteConfigurationReg();
	  //
	  int initial_clct_nplanes_hit_pretrig = thisTMB->GetHsPretrigThresh();
	  int initial_clct_nplanes_hit_pattern = thisTMB->GetMinHitsPattern();
	  thisTMB->SetHsPretrigThresh(1);
	  thisTMB->SetMinHitsPattern(1);
	  thisTMB->WriteRegister(0x70);
	  //
	  // set the number of BX's that a CFEB channel must be ON in order for TMB to be labeled as "bad"
	  int initial_cfeb_badbits_nbx = thisTMB->GetCFEBBadBitsNbx();
	  thisTMB->SetCFEBBadBitsNbx(20);
	  thisTMB->WriteRegister(0x124);
	  //
	  // Do not send triggers to the SP in this mode... it is too large of rate...
	  //	  int initial_mpc_output_enable = thisTMB->GetMpcOutputEnable();
	  //	  thisTMB->SetMpcOutputEnable(0);
	  //	  thisTMB->WriteRegister(0x86);
	  //
	  //
	  // Reset the software back to the initial values.  Leave the hardware in radioactivity mode...
	  thisALCT->SetPretrigNumberOfLayers(initial_alct_nplanes_hit_pretrig);
	  thisALCT->SetPretrigNumberOfPattern(initial_alct_nplanes_hit_pattern);
	  //
	  thisTMB->SetHsPretrigThresh(initial_clct_nplanes_hit_pretrig);
	  thisTMB->SetMinHitsPattern(initial_clct_nplanes_hit_pattern);
	  //
	  thisTMB->SetCFEBBadBitsNbx(initial_cfeb_badbits_nbx);
	  //
	  //	  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
	}
      }
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  this->ExpertToolsPage(in,out);
}
//
void emu::pc::Configurator::SetTwoLayerTrigger(int tmb) {
  //
  TMB * thisTMB = tmbVector[tmb];
  ALCTController * thisALCT = thisTMB->alctController();
  //
  int initial_alct_nplanes_hit_pretrig = thisALCT->GetPretrigNumberOfLayers();
  int initial_alct_nplanes_hit_pattern = thisALCT->GetPretrigNumberOfPattern();
  thisALCT->SetPretrigNumberOfLayers(2);
  thisALCT->SetPretrigNumberOfPattern(2);
  thisALCT->WriteConfigurationReg();
  //
  int initial_clct_nplanes_hit_pretrig = thisTMB->GetHsPretrigThresh();
  int initial_clct_nplanes_hit_pattern = thisTMB->GetMinHitsPattern();
  thisTMB->SetHsPretrigThresh(2);
  thisTMB->SetMinHitsPattern(2);
  thisTMB->WriteRegister(0x70);
  //
  // set the number of BX's that a CFEB channel must be ON in order for TMB to be labeled as "bad"
  int initial_cfeb_badbits_nbx = thisTMB->GetCFEBBadBitsNbx();
  thisTMB->SetCFEBBadBitsNbx(20);
  thisTMB->WriteRegister(0x124);
  //
  // Do not send triggers to the SP in this mode... it is too large of rate...
  //	  int initial_mpc_output_enable = thisTMB->GetMpcOutputEnable();
  //	  thisTMB->SetMpcOutputEnable(0);
  //	  thisTMB->WriteRegister(0x86);
  //
  //
  // Reset the software back to the initial values.  Leave the hardware in radioactivity mode...
  thisALCT->SetPretrigNumberOfLayers(initial_alct_nplanes_hit_pretrig);
  thisALCT->SetPretrigNumberOfPattern(initial_alct_nplanes_hit_pattern);
  //
  thisTMB->SetHsPretrigThresh(initial_clct_nplanes_hit_pretrig);
  thisTMB->SetMinHitsPattern(initial_clct_nplanes_hit_pattern);
  //
  thisTMB->SetCFEBBadBitsNbx(initial_cfeb_badbits_nbx);
  //
  //	  thisTMB->SetMpcOutputEnable(initial_mpc_output_enable);
  //
  return;
}
void emu::pc::Configurator::SetTwoLayerTriggerForSystem(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Configure all crates to trigger on 2-layers ALCT*CLCT" << std::endl;
  //
  int initial_crate = current_crate_;
  //
  if(total_crates_>0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      if ( crateVector[crate_number]->IsAlive() ) {
	//
	SetCurrentCrate(crate_number);
	//
	for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	  // std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	  SetTwoLayerTrigger(tmb);
	}
      }
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  this->ExpertToolsPage(in,out);
}
//
void emu::pc::Configurator::SetTwoLayerTriggerForCrate(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Configure all chambers in this crate to trigger on 2-layers ALCT*CLCT" << std::endl;
  //
  if ( crateVector[current_crate_]->IsAlive() ) {
    //
    for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
      // std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
      SetTwoLayerTrigger(tmb);
    }
  }
  //
  this->CrateConfiguration(in,out);
}
//
void emu::pc::Configurator::SetRadioactivityTriggerALCTOnly(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Configure all crates to have the Radioactivity Trigger for ALCT only" << std::endl;
  //
  int initial_crate = current_crate_;
  //
  if(total_crates_>0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      if ( crateVector[crate_number]->IsAlive() ) {
	//
	SetCurrentCrate(crate_number);
	//
	for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	  //
	  // std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	  //
	  TMB * thisTMB = tmbVector[tmb];
	  ALCTController * thisALCT = thisTMB->alctController();
	  //
	  int initial_alct_nplanes_hit_pretrig = thisALCT->GetPretrigNumberOfLayers();
	  int initial_alct_nplanes_hit_pattern = thisALCT->GetPretrigNumberOfPattern();
	  thisALCT->SetPretrigNumberOfLayers(1);
	  thisALCT->SetPretrigNumberOfPattern(1);
	  thisALCT->WriteConfigurationReg();
	  //
	  // Reset the software back to the initial values.  Leave the hardware in radioactivity mode...
	  thisALCT->SetPretrigNumberOfLayers(initial_alct_nplanes_hit_pretrig);
	  thisALCT->SetPretrigNumberOfPattern(initial_alct_nplanes_hit_pattern);
	  //
	}
      }
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  this->ExpertToolsPage(in,out);
}
//
void emu::pc::Configurator::SetTTCDelays(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Set TTC Delays on all crates" << std::endl;
  //
  int initial_crate = current_crate_;
  //
  if(total_crates_>0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      if ( crateVector[crate_number]->IsAlive() ) {
	//
	SetCurrentCrate(crate_number);
	//
	// Put CCB into FPGA mode to talk to the TTCrx board...
	thisCCB->setCCBMode(CCB::VMEFPGA);
	//
	thisCCB->HardResetTTCrx();
	::usleep(1000);
	// need to read the TTCrxID before TTCrx registers can be touched
	thisCCB->ReadTTCrxID();
	//
	// Download coarse delay to TTCrx
	int TTCrxCoarseDelay_ = thisCCB->GetTTCrxCoarseDelay();
	//
	//PrintTTCrxRegs();
	std::cout << "write TTCrxCoarseDelay_ = " << TTCrxCoarseDelay_ << " to register 2" << std::endl;
	//
	if(TTCrxCoarseDelay_>0) {
	  int delay = ((TTCrxCoarseDelay_&0xf)<<4) + (TTCrxCoarseDelay_&0xf);
	  thisCCB->WriteTTCrxReg(2,delay);
	}
	//
	// Download fine delay to TTCrx
	int TTCrxFineDelay_ = thisCCB->GetTTCrxFineDelay();
	//
	if( TTCrxFineDelay_>=0) {
	  int delay = thisCCB->ConvertNanosecondsToFineDelayUnits(TTCrxFineDelay_&0xff);
	  std::cout << "write TTCrxFineDelay_ = " << TTCrxFineDelay_ 
		    << " (TTCrx value = 0x" << std::hex << delay << ") to registers 0 and 1" 
		    << std::dec << std::endl;
	  thisCCB->WriteTTCrxReg(0,delay);
	  thisCCB->WriteTTCrxReg(1,delay);
	}
	//
	// Enable TTCrx parallel output bus
	//
	thisCCB->WriteTTCrxReg(3,0xB3);  

	// check TTCrx registers
	int rx;
	rx=(int) (thisCCB->ReadTTCrxReg(2).to_ulong());
	if(((rx&0xf) != (TTCrxCoarseDelay_&0xf)) || ((rx&0xf0)>>4 != (TTCrxCoarseDelay_&0xf)))  
	  std::cout << "ERROR: TTCrx Coarse Delay register readback " << std::hex << (rx&0xff) << std::dec << std::endl; 
	//
	rx=(int)(thisCCB->ReadTTCrxReg(3).to_ulong());
	if((rx&0xff) != 0xB3) 
	  std::cout << "ERROR: TTCrx Control register readback " << std::hex << (rx&0xff) << std::dec << std::endl; 
	//
	std::cout << "After writing..." << std::endl;
	thisCCB->PrintTTCrxRegs();
	//
	// Put CCB back into DLOG mode to listen to TTC commands...
	thisCCB->setCCBMode(CCB::DLOG);
	//
      }
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  this->ExpertToolsPage(in,out);
}


void emu::pc::Configurator::ScanCFEBVirtex(DAQMB *const thisDMB,
	Chamber *const thisChamber, std::ofstream &LogFileCheckCFEBVtx,
	const std::string &checkDir)
	throw (xgi::exception::Exception)
{
	std::vector<CFEB> thisCFEBs = thisDMB->cfebs();
	for (CFEBItr cfebItr = thisCFEBs.begin(); cfebItr != thisCFEBs.end(); ++cfebItr) {
		int cfeb_index = cfebItr->number();
		std::cout << "CFEB virtex " << thisCrate->GetLabel() << ", ";
		std::cout << (thisChamber->GetLabel()).c_str() << " CFEB " << cfeb_index + 1;
		std::cout << std::endl;
		int cbits[10]; // Bigger than necessary
		unsigned int tries = 0;
		do {
			cbits[0] = 0;
			thisDMB->rdbkvirtex(cfebItr->scamDevice());
			thisDMB->vtx_cmpfiles(checkDir, cbits);
			++tries;
		// Try once more on failed read
		} while (tries <= 1 && cbits[0] == -1);
		LogFileCheckCFEBVtx << "CFEB virtex " << thisCrate->GetLabel() << ", ";
		LogFileCheckCFEBVtx << (thisChamber->GetLabel()).c_str() << " CFEB " << cfeb_index + 1;
		if (cbits[0] == -1)
			LogFileCheckCFEBVtx << " Communications problem -- bad read" <<  std::endl;
		else {
			LogFileCheckCFEBVtx << " correct bits " << cbits[0];
			LogFileCheckCFEBVtx << ", bad bits " << cbits[1];
			LogFileCheckCFEBVtx << ".  (masked bits " << cbits[2] << ")" << std::endl;
			total_bad_cfeb_bits += cbits[1];
			total_good_cfeb_bits += cbits[0];
		}
	}
}


void emu::pc::Configurator::RdVfyCFEBVirtex(const int dmbIndex)
  throw (xgi::exception::Exception) {
// get the date and time of this check:
  time_t rawtime;
  struct tm *timeinfo;
  //
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  //
  int yearAD = timeinfo->tm_year + 1900;
  int month_counting_from_one = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  int hour = timeinfo->tm_hour;
  int minute = timeinfo->tm_min;
  sprintf(date_and_time_,"%4u%02u%02u_%02u%02u",yearAD,month_counting_from_one,day,hour,minute);
  const char * crate_name = crateVector[0]->GetLabel().c_str();
  char endcap_side = crate_name[3];
  char filename[200];
	std::string logDir = FirmwareDir_;
	logDir += "/status_check";
  sprintf(filename, "%s/%s_%c_cfebvirtex_check.log", logDir.c_str(), date_and_time_, endcap_side);
  std::ofstream LogFileCheckCFEBVtx;
  LogFileCheckCFEBVtx.open(filename);
	std::string checkDir = FirmwareDir_;
	checkDir += "/cfeb/status_check";
	total_bad_cfeb_bits = 0;	// For screen output
	total_good_cfeb_bits = 0;	// For screen output
	if (dmbIndex >= 0) {
		DAQMB *thisDMB = dmbVector[dmbIndex];
		Chamber *thisChamber = chamberVector[dmbIndex];
		ScanCFEBVirtex(thisDMB, thisChamber, LogFileCheckCFEBVtx, checkDir);
	} else {
		for (unsigned crate_number = 0; crate_number < crateVector.size(); crate_number++) {
			//
			SetCurrentCrate(crate_number);
			//
			for (unsigned int chamber_index = 0; chamber_index < dmbVector.size(); chamber_index++) {
				Chamber *thisChamber = chamberVector[chamber_index];
				DAQMB *thisDMB = dmbVector[chamber_index];
				ScanCFEBVirtex(thisDMB, thisChamber, LogFileCheckCFEBVtx, checkDir);
			}
		}
	}
  LogFileCheckCFEBVtx.close();
}

void emu::pc::Configurator::RdVfyCFEBVirtexDMB(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception)
{
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  int dmb;
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    DMB_ = dmb;
  } else {
    dmb = DMB_;
  }
  RdVfyCFEBVirtex(dmb);
	this->DMBUtils(in, out);
}


void emu::pc::Configurator::RdVfyCFEBVirtexExpT(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception)
{
  RdVfyCFEBVirtex(-1);  // -1 means scan all crates
  this->ExpertToolsPage(in, out);
}

//
void emu::pc::Configurator::MeasureAllTMBVoltages(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //  
  std::cout << "Button: Measure voltages on all TMB's" << std::endl;
  //
  char current_date_and_time[13];
  //
  // get the date and time of this check:
  time_t rawtime;
  struct tm * timeinfo;
  //
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  //
  int yearAD = timeinfo->tm_year + 1900;
  int month_counting_from_one = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  int hour = timeinfo->tm_hour;     
  int minute = timeinfo->tm_min;     
  //
  //
  sprintf(current_date_and_time,"%4u%02u%02u_%02u%02u",yearAD,month_counting_from_one,day,hour,minute);
  std::cout  << "current_date_and_time = " << current_date_and_time << std::endl;
  //
  std::string buf;
  //  buf = "/tmp/TMBVoltages_"+current_date_and_time+".log";
  buf = "/tmp/TMBVoltages.log";
  std::cout << "Logging output to" << buf << std::endl ;
  //
  std::ofstream LogFile;
  LogFile.open(buf.c_str());
  //
  LogFile << "##date_time = " << current_date_and_time                << std::endl;
  LogFile << "##Crate, Chamber, slot, 5.0V, 5.5A, 3.3V, 3.3A, 1.5V core, 1.5A core, 1.5V TT, 1.0V TT, 1.8V RAT, 1.8A RAT" << std::endl;
  //
  int initial_crate = current_crate_;
  //
  if(total_crates_>0) {
    //
    for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
      if ( crateVector[crate_number]->IsAlive() ) {
	//
	SetCurrentCrate(crate_number);
	//
	for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	  //
	  Chamber * thisChamber = chamberVector[tmb];
	  TMB * thisTMB = tmbVector[tmb];
	  //
	  //	  char Name[100];
	  //	  sprintf(Name,"%s TMB status, crate=%s, slot=%d",
	  //		  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisTMB->slot());
	  // std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	  //
	  //
	  thisTMB->ADCvoltages();
	  //
	  LogFile << ThisCrateID_.c_str() << " ";
	  LogFile << thisChamber->GetLabel() << " ";
	  LogFile << thisTMB->slot() << " ";
	  LogFile << std::setprecision(2) << std::fixed << thisTMB->Get5p0v() << " ";
	  LogFile << std::setprecision(2) << std::fixed << thisTMB->Get5p0a() << " "; 
	  LogFile << std::setprecision(2) << std::fixed << thisTMB->Get3p3v() << " ";
	  LogFile << std::setprecision(2) << thisTMB->Get3p3a() << " ";
	  LogFile << std::setprecision(3) << thisTMB->Get1p5vCore() << " ";
	  LogFile << std::setprecision(2) << thisTMB->Get1p5aCore() << " ";
	  LogFile << std::setprecision(2) << thisTMB->Get1p5vTT() << " ";
	  LogFile << std::setprecision(2) << thisTMB->Get1p0vTT() << " ";
	  LogFile << std::setprecision(2) << thisTMB->Get1p8vRAT() << " ";
	  LogFile << std::setprecision(2) << thisTMB->Get1p8aRAT() << " ";
	  LogFile << std::endl;
	}
      }
    }
  }
  //
  SetCurrentCrate(initial_crate);
  //
  LogFile.close();    
  //
  this->ExpertToolsPage(in,out);
}
//
/////////////////////////////////////////////////////////////////////
// Chamber Utilities (synchronization) methods
/////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
// Crate utilities methods
//////////////////////////////////////////////////////////////////////////
void emu::pc::Configurator::TmbMPCTest(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  // if(!parsed) ParsingXML();
  myCrateTest.SetCrate(thisCrate);
  //
  myCrateTest.RedirectOutput(&CrateTestsOutput);
  int number_of_loops = 1000;
  myCrateTest.MpcTMBTest(number_of_loops);
  myCrateTest.RedirectOutput(&std::cout);
  //
  this->CrateTests(in,out);
  //
}
//
void emu::pc::Configurator::MPCSafeWindowScan(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  // if(!parsed) ParsingXML();
  //CrateUtilities myCrateTest;
  myCrateTest.SetCrate(thisCrate);
  //
  myCrateTest.RedirectOutput(&CrateTestsOutput);
  int number_of_loops = 10;
  int min_value       = 15;
  int max_value       = 75;
  myCrateTest.MpcTMBTest(number_of_loops,min_value,max_value);
  myCrateTest.RedirectOutput(&std::cout);
  //
  this->CrateTests(in,out);
  //
}
//
////////////////////////////////////////////////////////////////
// Chamber Tests (synchronization) methods
////////////////////////////////////////////////////////////////
void emu::pc::Configurator::ChamberTests(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb, dmb;
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  if( name != cgi.getElements().end() ) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "ChamberTests:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "ChamberTests:  No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if( name != cgi.getElements().end() ) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "ChamberTests:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "ChamberTests:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  DAQMB * thisDMB = dmbVector[dmb];
  alct = thisTMB->alctController();
  //
  Chamber * thisChamber = chamberVector[tmb];
  //
  char Name[100];
  sprintf(Name,"%s synchronization, crate=%s, TMBslot=%d, DMBslot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisTMB->slot(),thisDMB->slot());
  //
  // MyHeader(in,out,Name);
  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)", ( fsm_.getCurrentState() == 'E' ? 5 : -1 ) );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector", 
			"/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" )
	      .addLink( "Main Page", 
			"/" + getApplicationDescriptor()->getURN() + "/MainPage" )
	      .addLink( "Crate Configuration", 
			"/" + getApplicationDescriptor()->getURN() + "/CrateConfiguration" )
	      .addLink( Name )
	      );
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  //
  *out << cgicc::legend("Team A tests").set("style","color:blue") << std::endl ;
  //
  char buf[20];
  //
  //
  std::string ALCT_TMB_communication = toolbox::toString("/%s/ALCT_TMB_communication",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCT_TMB_communication) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Test ALCT-TMB communication") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  //
  //
  if ( (alct->GetChamberType()).find("ME11") != std::string::npos ) {
    *out << cgicc::table().set("border","0");
    //
    *out << cgicc::td().set("ALIGN","left");
    std::string CFEBTimingME11b = toolbox::toString("/%s/CFEBTimingME11b",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CFEBTimingME11b) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","ME1/1b - Scan CFEB rx phases") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::td().set("ALIGN","center");
    *out << "... or ...";
    *out << cgicc::td();
    //
    *out << cgicc::td().set("ALIGN","left");
    std::string CFEBTimingME11a = toolbox::toString("/%s/CFEBTimingME11a",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CFEBTimingME11a) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","ME1/1a - Scan CFEB rx phase") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
  } else if ( (alct->GetChamberType()).find("ME13") != std::string::npos ) {
    //
    std::string CFEBTimingME11b = toolbox::toString("/%s/CFEBTimingME11b",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CFEBTimingME11b) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","ME1/3 - Scan CFEB rx phases") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
    //
  } else {
    std::string CFEBTiming = toolbox::toString("/%s/CFEBTiming",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CFEBTiming) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Scan CFEB rx phases") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    sprintf(buf,"%d",dmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
    *out << cgicc::form() << std::endl ;
  }
  //
  for(int i=0;i<5;i++) {
    *out << "cfeb" << i << "delay = " << MyTest[tmb][current_crate_].GetCFEBrxPhaseTest(i) 
	 << " ("  << MyTest[tmb][current_crate_].GetCfebRxClockDelay(i) << ") " <<std::endl;
    *out << cgicc::br();
  }
  *out << cgicc::br();
  //
  //
  std::string RatTmbTiming = toolbox::toString("/%s/RatTmbTiming",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",RatTmbTiming) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Scan RAT-TMB phase") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "rat_tmb_delay = " << MyTest[tmb][current_crate_].GetRatTmbDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetRatTmbDelay()     << ") " << std::endl;
  *out << cgicc::br();
  //
  *out << cgicc::fieldset();
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  //
  *out << cgicc::legend("Expert tools").set("style","color:blue") << std::endl ;
  //
  //  *out << cgicc::pre();
  //  *out << "   c) Enter above values into xml" << std::endl;
  //  *out << "   d) Set up the xml file with the desired ALCT and CLCT trigger configuration" << std::endl;
  //  *out << "   e) Restart Peripheral Crate XDAQ" << std::endl;
  //  *out << "   f) Init System" << std::endl;
  //  *out << cgicc::pre();
  //
  //
  //  *out << cgicc::pre();
  //  *out << "2) Set up to perform synchronization" << std::endl;
  //  *out << "   a) LTC Hard Reset" << std::endl;
  //  *out << "   b) For muons from LHC beam or cosmic rays:" << std::endl;
  //  *out << "      i ) HV:  on" << std::endl;
  //  *out << "      ii) skip to step d)" << std::endl;
  //  *out << "   c) For pulsing from the TTC:" << std::endl;
  //  *out << "      i ) push the following button" << std::endl;
  //  *out << cgicc::pre();
  //  //
  *out << cgicc::pre();
  *out << "Snapshot of the DMB timing parameters..." << std::endl;
  *out << cgicc::pre();
  //
  std::string setDataReadValues = toolbox::toString("/%s/setDataReadValues",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",setDataReadValues) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Set DMB/ALCT-CLCT match data read values") << std::endl ;
  sprintf(buf,"%d",MyTest[tmb][current_crate_].getNumberOfDataReads());
  *out << "Number of reads" << std::endl;
  *out << cgicc::input().set("type","text").set("value",buf).set("name","number_of_reads") << std::endl ;
  sprintf(buf,"%d",MyTest[tmb][current_crate_].getPauseBetweenDataReads());
  *out << "Pause between reads (usec)" << std::endl;
  *out << cgicc::input().set("type","text").set("value",buf).set("name","pause_btw_reads") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string PrintDmbValuesAndScopes = toolbox::toString("/%s/PrintDmbValuesAndScopes",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",PrintDmbValuesAndScopes) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Read DMB Values/Scopes") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  //  *out << cgicc::pre();
  //  *out << "3) Perform all synchronization steps by pushing the following button" << std::endl;
  //  *out << "   -> This button performs the synchronization steps detailed below " << std::endl;
  //  *out << "      in order, propagating the measured values from one step to the" << std::endl;
  //  *out << "      next step" << std::endl;
  //  *out << cgicc::pre();
  //  //
  //  std::string Automatic = toolbox::toString("/%s/Automatic",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",Automatic) << std::endl ;
  //  *out << cgicc::input().set("type","submit").set("value","Automatic") << std::endl ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  sprintf(buf,"%d",dmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  //  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string MeasureL1AsAndDAVsForChamber = toolbox::toString("/%s/MeasureL1AsAndDAVsForChamber",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",MeasureL1AsAndDAVsForChamber) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Measure L1As and DAVs") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();
  //
  std::string ALCTBC0Scan = toolbox::toString("/%s/ALCTBC0Scan",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTBC0Scan) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Measure ALCT BC0 delay") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "alct_bx0_delay = " << MyTest[tmb][current_crate_].GetAlctBx0DelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetAlctBx0Delay() << ") " <<std::endl;
  *out << cgicc::br();
  *out << "match_trig_alct_delay = " << MyTest[tmb][current_crate_].GetMatchTrigAlctDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetALCTvpf_configvalue() << ") " << std::endl;
  *out << cgicc::br();
  //
  std::string QuickScanForChamber = toolbox::toString("/%s/QuickScanForChamber",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",QuickScanForChamber) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Quick Scan") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br();

  *out << cgicc::br();
  //
  //
  //  *out << cgicc::pre();
  *out << "--------------------------------------------------------------------------" << std::endl;
  *out << " Synchronization step-by-step" << std::endl;
  *out << "--------------------------------------------------------------------------" << std::endl;
  //  *out << "A) Measure CLCT-ALCT match timing" << std::endl;
  //  *out << "   -> Measured values are based on current values of:" << std::endl;
  //  *out << "        * match_trig_window_size" << std::endl;
  //  *out << "        * match_trig_alct_delay" << std::endl;
  //  *out << "        * mpc_tx_delay" << std::endl;
  //  *out << "(assuming that the trigger primitives have already been synchronized at the MPC)" << std::endl;
  //  *out << cgicc::pre();
  //
  //  std::string ALCTvpf = toolbox::toString("/%s/ALCTvpf",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",ALCTvpf) << std::endl ;
  //  *out << cgicc::input().set("type","submit").set("value","Measure CLCT-ALCT match timing") << std::endl ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  sprintf(buf,"%d",dmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  //  *out << cgicc::form() << std::endl ;
  //  //
  //  *out << "match_trig_alct_delay = " << MyTest[tmb][current_crate_].GetMatchTrigAlctDelayTest() 
  //       << " ("  << MyTest[tmb][current_crate_].GetALCTvpf_configvalue() << ") " << std::endl;
  //  *out << cgicc::br();
  //  *out << "mpc_tx_delay = " << MyTest[tmb][current_crate_].GetMpcTxDelayTest() 
  //       << " ("  << MyTest[tmb][current_crate_].GetMPCTxDelay_configvalue() << ") " << std::endl;
  //  *out << cgicc::br();
  //  *out << cgicc::br();
  //
  //
  *out << cgicc::pre();
  *out << "Measure the delay needed for the winner bit from MPC back to TMB" << std::endl;
  *out << cgicc::pre();
  //
  std::string FindWinner = toolbox::toString("/%s/FindWinner",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",FindWinner) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Determine Winner Bit Timing") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "mpc_rx_delay = " << MyTest[tmb][current_crate_].GetMpcRxDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetMPCdelay() << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //
  //
  *out << cgicc::pre();
  *out << "Find the L1A at the TMB and ALCT" << std::endl;
  *out << cgicc::pre();
  //
  std::string setTMBCounterReadValues = toolbox::toString("/%s/setTMBCounterReadValues",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",setTMBCounterReadValues) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Set scan values") << std::endl ;
  *out << "TMB L1A min value (bx)" << std::endl;
  sprintf(buf,"%d",MyTest[tmb][current_crate_].getMinTmbL1aDelayValue());
  *out << cgicc::input().set("type","text").set("value",buf).set("name","tmb_l1a_delay_min") << std::endl ;
  *out << "TMB L1A max value (bx)" << std::endl;
  sprintf(buf,"%d",MyTest[tmb][current_crate_].getMaxTmbL1aDelayValue());
  *out << cgicc::input().set("type","text").set("value",buf).set("name","tmb_l1a_delay_max") << std::endl ;
  *out << "ALCT L1A min value (bx)" << std::endl;
  sprintf(buf,"%d",MyTest[tmb][current_crate_].getMinAlctL1aDelayValue());
  *out << cgicc::input().set("type","text").set("value",buf).set("name","alct_l1a_delay_min") << std::endl ;
  *out << "ALCT L1A max value (bx)" << std::endl;
  sprintf(buf,"%d",MyTest[tmb][current_crate_].getMaxAlctL1aDelayValue());
  *out << cgicc::input().set("type","text").set("value",buf).set("name","alct_l1a_delay_max") << std::endl ;
  *out << "Pause at each setting (sec)" << std::endl;
  sprintf(buf,"%d",MyTest[tmb][current_crate_].getPauseAtEachSetting());
  *out << cgicc::input().set("type","text").set("value",buf).set("name","time_to_pause") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  std::string TMBL1aTiming = toolbox::toString("/%s/TMBL1aTiming",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBL1aTiming) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Determine TMB L1a Timing") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "tmb_l1a_delay = " << MyTest[tmb][current_crate_].GetTmbL1aDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetTMBL1aTiming_configvalue() << ") " << std::endl;
  *out << cgicc::br();
  //
  std::string ALCTL1aTiming = toolbox::toString("/%s/ALCTL1aTiming",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTL1aTiming) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Determine ALCT L1a Timing") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "alct_l1a_delay = " << MyTest[tmb][current_crate_].GetAlctL1aDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetALCTL1aDelay_configvalue() << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //
  //
  *out << cgicc::pre();
  *out << "Align the Data AVailable (DAV) bits for the ALCT" << std::endl;
  *out << cgicc::pre();
  //
  std::string AlctDavCableDelay = toolbox::toString("/%s/AlctDavCableDelay",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",AlctDavCableDelay) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Determine ALCT DAV (ALCT-TMB) cable delay") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "alct_dav_cable_delay = " << MyTest[tmb][current_crate_].GetAlctDavCableDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetAlctDavCableDelay_configvalue() << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //
  //  *out << cgicc::pre();
  //  *out << "-> The following parameter, cfeb_dav_cable_delay, depends on cfeb_cable_delay," << std::endl;
  //  *out << "   who should be set strictly according to its cable length" << std::endl;
  //  *out << cgicc::pre();
  //
  *out << cgicc::pre();
  *out << "Find the L1A at the CFEB" << std::endl;
  *out << cgicc::pre();
  //
  std::string TmbLctCableDelay = toolbox::toString("/%s/TmbLctCableDelay",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TmbLctCableDelay) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Determine Active FEB flag (CFEB-TMB) cable delay") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "tmb_lct_cable_delay = " << MyTest[tmb][current_crate_].GetTmbLctCableDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetTmbLctCableDelay_configvalue() << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //
  *out << cgicc::pre();
  *out << "Align the Data AVailable (DAV) bits for the CFEB" << std::endl;
  *out << cgicc::pre();
  //
  std::string CfebDavCableDelay = toolbox::toString("/%s/CfebDavCableDelay",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CfebDavCableDelay) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Determine CFEB DAV (CFEB-DMB) cable delay") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "cfeb_dav_cable_delay = " << MyTest[tmb][current_crate_].GetCfebDavCableDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetCfebDavCableDelay_configvalue() << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //
  //
  //  std::string setupCoincidencePulsing = toolbox::toString("/%s/setupCoincidencePulsing",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",setupCoincidencePulsing) << std::endl ;
  //  *out << cgicc::input().set("type","submit").set("value","Setup Coincidence Pulsing") << std::endl ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::br();
  //  *out << cgicc::br();
  //
  //
  *out << cgicc::pre();
  *out << "------------------------------------------------------------------" << std::endl;
  *out << "         Other tools available pertinent to the chamber           " << std::endl;
  *out << "------------------------------------------------------------------" << std::endl;
  *out << cgicc::pre();
  //
  std::string RpcRatTiming = toolbox::toString("/%s/RpcRatTiming",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",RpcRatTiming) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Determine RPC-RAT phase") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "rpc0_rat_delay = " << MyTest[tmb][current_crate_].GetRpcRatDelayTest() 
       << " ("  << MyTest[tmb][current_crate_].GetRpcRatDelay()     << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //  //
  //  //
  std::string FindDistripHotChannel = toolbox::toString("/%s/FindDistripHotChannel",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",FindDistripHotChannel) << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::input().set("type","submit").set("value","Find CLCT distrip hot channels") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string ALCTScan = toolbox::toString("/%s/ALCTScan",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTScan) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","ALCT Scan") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::form() << std::endl ;
  //
  for (int Wire = 0; Wire<(alct->GetNumberOfChannelsInAlct())/6; Wire++) 
    *out << MyTest[tmb][current_crate_].GetALCTWireScan(Wire) ;
  *out << std::endl;
  *out << cgicc::br();
  //
  std::string CFEBScan = toolbox::toString("/%s/CFEBScan",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CFEBScan) << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  sprintf(buf,"%d",dmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","dmb");
  *out << cgicc::input().set("type","submit").set("value","CFEB Scan") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  for (int CFEBs = 0; CFEBs<5; CFEBs++) {
    *out << "CFEB Id="<<CFEBs<< " " ;
    for (int HalfStrip = 0; HalfStrip<32; HalfStrip++) {
      *out << MyTest[tmb][current_crate_].GetCFEBStripScan(CFEBs,HalfStrip) ;
    }
    *out << std::endl;
    *out << cgicc::br();
  }
  *out << cgicc::br();
  //
  //
  *out << cgicc::fieldset();
  *out << std::endl;
  //
  *out << cgicc::form().set("method","GET") << std::endl ;
  *out << cgicc::textarea().set("name","ChamberTestOutput").set("WRAP","OFF").set("rows","20").set("cols","100");
  *out << ChamberTestsOutput[tmb][current_crate_].str() << std::endl ;
  *out << cgicc::textarea();
  *out << cgicc::form() << std::endl ;
  //
  std::string LogChamberTestsOutput = toolbox::toString("/%s/LogChamberTestsOutput",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LogChamberTestsOutput) << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","submit").set("value","Log output").set("name","LogChamberTestsOutput") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Clear").set("name","ClearChamberTestsOutput") << std::endl ;
  *out << cgicc::form() << std::endl ;
  pageFooter( in, out );
  htmlFooter( in, out );
}
//
//
void emu::pc::Configurator::setupCoincidencePulsing(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Setup coincidence pulsing" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Setup Coincidence Pulsing");
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb;
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Setup Coincidence Pulsing:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Setup Coincidence Pulsing:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].SetupCoincidencePulsing();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
////////////////////////////////////////////////////////////////
// Tests relevant to checking ALCT cable and rx/tx timing
////////////////////////////////////////////////////////////////
void emu::pc::Configurator::ALCT_TMB_communication(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb = 99;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if( name != cgi.getElements().end() ) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "ALCT_TMB_communication:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "ALCT_TMB_communication:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  alct = thisTMB->alctController();
  //
  Chamber * thisChamber = chamberVector[tmb];
  //
  char Name[100];
  sprintf(Name,"%s ALCT-TMB communication, crate=%s, TMBslot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisTMB->slot());
  //
  // MyHeader(in,out,Name);
  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)", ( fsm_.getCurrentState() == 'E' ? 5 : -1 ) );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector", 
			"/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" )
	      .addLink( "Main Page", 
			"/" + getApplicationDescriptor()->getURN() + "/MainPage" )
	      .addLink( "Crate Configuration", 
			"/" + getApplicationDescriptor()->getURN() + "/CrateConfiguration" )
	      .addLink( "Chamber Tests", 
			"/" + getApplicationDescriptor()->getURN() + "/ChamberTests" )
	      .addLink( Name )
	      );
  //
  char buf[20];
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;") << std::endl;
  //
  *out << cgicc::legend("New tests").set("style","color:blue") << std::endl ;
  //
  std::string ALCT_TMB_Loopback = toolbox::toString("/%s/ALCT_TMB_Loopback",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCT_TMB_Loopback) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Scan ALCT tx/rx phases") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << "alct_tx_clock_delay = " << MyTest[tmb][current_crate_].GetALCTtxPhaseTest() 
       <<  " (" << MyTest[tmb][current_crate_].GetAlctTxClockDelay() << ") " << std::endl;
  *out << cgicc::br();
  *out << "alct_rx_clock_delay = " << MyTest[tmb][current_crate_].GetALCTrxPhaseTest() 
       <<  " (" << MyTest[tmb][current_crate_].GetAlctRxClockDelay() << ") " << std::endl;
  *out << cgicc::br();
  *out << "alct_posneg = " << MyTest[tmb][current_crate_].GetAlctRxPosNegTest() 
       <<  " (" << MyTest[tmb][current_crate_].GetAlctRxPosNeg() << ") " << std::endl;
  *out << cgicc::br();
  *out << "alct_tx_posneg = " << MyTest[tmb][current_crate_].GetAlctTxPosNegTest() 
       <<  " (" << MyTest[tmb][current_crate_].GetAlctTxPosNeg() << ") " << std::endl;
  *out << cgicc::br();
  *out << cgicc::br();
  //
  std::string TMB_to_ALCT_walking_ones = toolbox::toString("/%s/TMB_to_ALCT_walking_ones",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMB_to_ALCT_walking_ones) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Check ALCT-TMB cable") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset() << std::endl;
  //
  //
  //
  *out << cgicc::form().set("method","GET") << std::endl ;
  *out << cgicc::textarea().set("name","ChamberTestOutput").set("WRAP","OFF").set("rows","20").set("cols","100");
  *out << ALCT_TMB_communicationOutput[tmb][current_crate_].str() << std::endl ;
  *out << cgicc::textarea();
  *out << cgicc::form() << std::endl ;
  //
  std::string LogALCT_TMB_communicationOutput = toolbox::toString("/%s/LogALCT_TMB_communicationOutput",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LogALCT_TMB_communicationOutput) << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","submit").set("value","Log output").set("name","LogALCT_TMB_communicationOutput") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Clear").set("name","ClearALCT_TMB_communicationOutput") << std::endl ;
  *out << cgicc::form() << std::endl ;
  pageFooter( in, out );
  htmlFooter( in, out );
}
//
void emu::pc::Configurator::ALCT_TMB_Loopback(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "ALCT_TMB_Loopback" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "ALCT_TMB_Loopback");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb = TMB_;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "ALCT_TMB_Loopback:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "ALCT_TMB_Loopback: No tmb" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ALCT_TMB_communicationOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].ALCT_TMB_Loopback();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ALCT_TMB_communication(in,out);
  //
}
//
void emu::pc::Configurator::TMB_to_ALCT_walking_ones(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "TMB_to_ALCT_walking_ones" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "TMB_to_ALCT_walking_ones");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb = TMB_;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB_to_ALCT_walking_ones:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "TMB_to_ALCT_walking_ones: No tmb" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ALCT_TMB_communicationOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].TMB_to_ALCT_walking_ones();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ALCT_TMB_communication(in,out);
  //
}
//
void emu::pc::Configurator::CFEBTiming(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "CFEBTiming" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "CFEBTiming");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb=0, dmb=0;
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "CFEBTiming:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "CFEBTiming:  No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "CFEBTiming:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "CFEBTiming:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].CFEBTiming();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
void emu::pc::Configurator::CFEBTimingME11b(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "CFEBTiming ME1/1B" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "CFEBTiming");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb=0, dmb=0;
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "CFEBTiming ME1/1B:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "CFEBTiming ME1/1B:  No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "CFEBTiming ME1/1B:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "CFEBTiming ME1/1B:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].CFEBTiming(ChamberUtilities::me11b_scan);
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
void emu::pc::Configurator::CFEBTimingME11a(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "CFEBTiming ME1/1A" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "CFEBTiming");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb=0, dmb=0;
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "CFEBTiming ME1/1A:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "CFEBTiming ME1/1A:  No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "CFEBTiming ME1/1A:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "CFEBTiming ME1/1A:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].CFEBTiming(ChamberUtilities::me11a_scan);
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}

void emu::pc::Configurator::ALCTBC0Scan(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "ALCTBC0Scan" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "ALCTBC0Scan");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb = TMB_;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "ALCTBC0Scan:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "ALCTBC0Scan" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].ALCTBC0Scan();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void emu::pc::Configurator::Automatic(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Perform all synchronization steps in order" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Automatic");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb=0;
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Automatic:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Automatic:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  int dmb;
  name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "Automatic:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "Automatic:  No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].Automatic();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void emu::pc::Configurator::MeasureL1AsAndDAVsForChamber(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find L1A and DAV delays for chamber" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find L1A and DAV delays for chamber");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb;
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Automatic:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Automatic:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].SetupRadioactiveTriggerConditions();
  MyTest[tmb][current_crate_].FindL1AAndDAVDelays();
  MyTest[tmb][current_crate_].ReturnToInitialTriggerConditions();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void emu::pc::Configurator::QuickScanForChamber(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Quick Scan for chamber" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Quick Scan for chamber");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb;
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Quick Scan for chamber:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Quick Scan for chamber:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].QuickTimingScan();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
//
void emu::pc::Configurator::MeasureL1AsAndDAVsForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find L1A and DAV delays for crate" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find L1A and DAV delays for crate");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ChamberTestsOutput[i][current_crate_]);
    MyTest[i][current_crate_].SetupRadioactiveTriggerConditions();
    MyTest[i][current_crate_].FindL1AAndDAVDelays();
    MyTest[i][current_crate_].ReturnToInitialTriggerConditions();
    MyTest[i][current_crate_].RedirectOutput(&std::cout);
  }
  //
  this->CrateConfiguration(in,out);
  //
}
//
void emu::pc::Configurator::MeasureL1AsForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find L1A delays for crate" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find L1A delays for crate");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ChamberTestsOutput[i][current_crate_]);
    MyTest[i][current_crate_].SetupRadioactiveTriggerConditions();
    MyTest[i][current_crate_].FindL1ADelays();
    MyTest[i][current_crate_].ReturnToInitialTriggerConditions();
    MyTest[i][current_crate_].RedirectOutput(&std::cout);
  }
  //
  this->CrateConfiguration(in,out);
  //
}
//
void emu::pc::Configurator::MeasureDAVsForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find DAV cable delays for Crate" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find DAV cable delays");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ChamberTestsOutput[i][current_crate_]);
    MyTest[i][current_crate_].SetupRadioactiveTriggerConditions();
    MyTest[i][current_crate_].FindDAVDelays();
    MyTest[i][current_crate_].ReturnToInitialTriggerConditions();
    MyTest[i][current_crate_].RedirectOutput(&std::cout);
  }
  //
  this->CrateConfiguration(in,out);
  //
}
//
void emu::pc::Configurator::QuickScanForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Quick Scan for crate" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Quick Scan for crate");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ChamberTestsOutput[i][current_crate_]);
    MyTest[i][current_crate_].QuickTimingScan();
    MyTest[i][current_crate_].RedirectOutput(&std::cout);
  }
  //
  this->CrateConfiguration(in,out);
  //
}
//
void emu::pc::Configurator::MeasureALCTTMBRxTxForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find ALCT rx/tx phase delays for Crate" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find ALCT rx/tx phase delays for the crate");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ALCT_TMB_communicationOutput[i][current_crate_]);
    MyTest[i][current_crate_].ALCT_TMB_Loopback();
    MyTest[i][current_crate_].RedirectOutput(&std::cout);
  }
  //
  this->CrateConfiguration(in,out);
  //
}
//
void emu::pc::Configurator::MeasureCFEBTMBRxForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find CFEB rx phase delays for Crate" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find CFEB rx phase delays for the crate");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ChamberTestsOutput[i][current_crate_]);
    MyTest[i][current_crate_].CFEBTiming();
    MyTest[i][current_crate_].RedirectOutput(&std::cout);
  }
  //
  this->CrateConfiguration(in,out);
  //
}
//
void emu::pc::Configurator::ALCTBC0ScanForCrate(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find alct_bx0_delays for Crate" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find alct_bx0_delays for the crate");
  //
  cgicc::Cgicc cgi(in);
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
    //    
    std::cout << "crate = " << current_crate_ << ", TMB " << i << std::endl;
    //
    MyTest[i][current_crate_].RedirectOutput(&ALCT_TMB_communicationOutput[i][current_crate_]);
    MyTest[i][current_crate_].ALCTBC0Scan();
    MyTest[i][current_crate_].RedirectOutput(&std::cout);
  }
  //
  this->CrateConfiguration(in,out);
  //
}
//
void emu::pc::Configurator::Settmb_bxn_offset(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb_bxn_offset");
  //
  int tmb_bxn_offset  = 3550;
  //
  if(name != cgi.getElements().end()) {
    tmb_bxn_offset = strtol(cgi["tmb_bxn_offset"]->getValue().c_str(),NULL,10);
    std::cout << "Settmb_bxn_offset = " << tmb_bxn_offset << std::endl;
  } else {
    std::cout << "No set value:  Settmb_bxn_offset = " << tmb_bxn_offset << std::endl;
  }
  //
  for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) 
    MyTest[i][current_crate_].setLocalTmbBxnOffset(tmb_bxn_offset);

  //
  this->CrateConfiguration(in,out);
  //
}
//
void emu::pc::Configurator::MeasureALCTTMBRxTxForSystem(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find ALCT rx/tx phase delays for System" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find ALCT rx/tx phase delays for the system");
  //
  cgicc::Cgicc cgi(in);
  //
  // if(!parsed) ParsingXML();
  //
  if(total_crates_<=0) return;
  //
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    if ( crateVector[crate_number]->IsAlive() ) {
      //
      SetCurrentCrate(crate_number);
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	//
	std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	//
	MyTest[tmb][current_crate_].RedirectOutput(&ALCT_TMB_communicationOutput[tmb][current_crate_]);
	MyTest[tmb][current_crate_].ALCT_TMB_Loopback();
	MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
      }
      //
      SaveLog();
      //
      SaveTestSummary();
      //
    }
  }
  //
  this->ExpertToolsPage(in,out);
  //
}
//
void emu::pc::Configurator::MeasureCFEBTMBRxForSystem(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find CFEB rx phase delays for System" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find CFEB rx phase delays for the system");
  //
  cgicc::Cgicc cgi(in);
  //
  // if(!parsed) ParsingXML();
  //
  if(total_crates_<=0) return;
  //
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    if ( crateVector[crate_number]->IsAlive() ) {
      //
      SetCurrentCrate(crate_number);
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	//
	std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	//
	MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
	MyTest[tmb][current_crate_].CFEBTiming();
	MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
      }
      //
      SaveLog();
      //
      SaveTestSummary();
      //
    }
  }
  //
  this->ExpertToolsPage(in,out);
  //
}
//
void emu::pc::Configurator::MeasureL1AsAndDAVsForSystem(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find L1As and DAVs for System" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find L1As and DAVs for the system");
  //
  cgicc::Cgicc cgi(in);
  //
  // if(!parsed) ParsingXML();
  //
  if(total_crates_<=0) return;
  //
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    if ( crateVector[crate_number]->IsAlive() ) {
      //
      SetCurrentCrate(crate_number);
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	//
	std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	//
	MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
	MyTest[tmb][current_crate_].SetupRadioactiveTriggerConditions();
	MyTest[tmb][current_crate_].FindL1AAndDAVDelays();
	MyTest[tmb][current_crate_].ReturnToInitialTriggerConditions();
	MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
	//
      }
      //
      SaveLog();
      //
      SaveTestSummary();
      //
    }
  }
  //
  this->ExpertToolsPage(in,out);
  //
}
//
void emu::pc::Configurator::ALCTBC0ScanForSystem(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find ALCT rx/tx phase delays for System" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Find ALCT rx/tx phase delays for the system");
  //
  cgicc::Cgicc cgi(in);
  //
  // if(!parsed) ParsingXML();
  //
  if(total_crates_<=0) return;
  //
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    if ( crateVector[crate_number]->IsAlive() ) {
      //
      SetCurrentCrate(crate_number);
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	//
	std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	//
	MyTest[tmb][current_crate_].RedirectOutput(&ALCT_TMB_communicationOutput[tmb][current_crate_]);
	MyTest[tmb][current_crate_].ALCTBC0Scan();
	MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
      }
      //
    }
  }
  //
  // This test is so fast, we only need to save stuff at the end....
  SaveLog();
  //
  SaveTestSummary();
  //
  this->ExpertToolsPage(in,out);
  //
}
//
void emu::pc::Configurator::QuickScanForSystem(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Quick Scan for System" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "Quick Scan for system");
  //
  cgicc::Cgicc cgi(in);
  //
  // if(!parsed) ParsingXML();
  //
  if(total_crates_<=0) return;
  //
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    if ( crateVector[crate_number]->IsAlive() ) {
      //
      SetCurrentCrate(crate_number);
      //
      for (unsigned int tmb=0; tmb<(tmbVector.size()<9?tmbVector.size():9) ; tmb++) {
	//
	std::cout << "crate = " << current_crate_ << ", TMB " << tmb << std::endl;
	//
	MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
	MyTest[tmb][current_crate_].QuickTimingScan();
	MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
	//
      }
      //
      SaveLog();
      //
      SaveTestSummary();
      //
    }
  }
  //
  this->ExpertToolsPage(in,out);
  //
}
//
//
void emu::pc::Configurator::setTMBCounterReadValues(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb;
  cgicc::form_iterator name = cgi.getElement("tmb");
  cgicc::form_iterator name2 = cgi.getElement("tmb_l1a_delay_min");
  cgicc::form_iterator name3 = cgi.getElement("tmb_l1a_delay_max");
  cgicc::form_iterator name4 = cgi.getElement("alct_l1a_delay_min");
  cgicc::form_iterator name5 = cgi.getElement("alct_l1a_delay_max");
  cgicc::form_iterator name6 = cgi.getElement("time_to_pause");
  //
  int tmb_l1a_delay_min  = 100;
  int tmb_l1a_delay_max  = 101;
  int alct_l1a_delay_min = 102;
  int alct_l1a_delay_max = 103;
  int time_to_pause      = 11;
  //
  if(name2 != cgi.getElements().end()) 
    tmb_l1a_delay_min = strtol(cgi["tmb_l1a_delay_min"]->getValue().c_str(),NULL,10);
  if(name3 != cgi.getElements().end()) 
    tmb_l1a_delay_max = strtol(cgi["tmb_l1a_delay_max"]->getValue().c_str(),NULL,10);
  if(name4 != cgi.getElements().end()) 
    alct_l1a_delay_min = strtol(cgi["alct_l1a_delay_min"]->getValue().c_str(),NULL,10);
  if(name5 != cgi.getElements().end()) 
    alct_l1a_delay_max = strtol(cgi["alct_l1a_delay_max"]->getValue().c_str(),NULL,10);
  if(name6 != cgi.getElements().end()) 
    time_to_pause = strtol(cgi["time_to_pause"]->getValue().c_str(),NULL,10);
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "setTMBCounterReadValues: TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "setTMBCounterReadValues:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  int dmb;
  name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "setTMBCounterReadValues:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "setTMBCounterReadValues:  No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  MyTest[tmb][current_crate_].setMinAlctL1aDelayValue(alct_l1a_delay_min);
  MyTest[tmb][current_crate_].setMaxAlctL1aDelayValue(alct_l1a_delay_max);
  MyTest[tmb][current_crate_].setMinTmbL1aDelayValue(tmb_l1a_delay_min);
  MyTest[tmb][current_crate_].setMaxTmbL1aDelayValue(tmb_l1a_delay_max);
  MyTest[tmb][current_crate_].setPauseAtEachSetting(time_to_pause);
  //
  this->ChamberTests(in,out);
  //
}
//
void emu::pc::Configurator::SetNumberOfHardResets(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "in here..." << std::endl;
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("number_of_hard_resets");
  //
  if(name != cgi.getElements().end()) {
    number_of_hard_resets_ = strtol(cgi["number_of_hard_resets"]->getValue().c_str(),NULL,10);
    std::cout << "Setting number of hard resets to " << number_of_hard_resets_ << std::endl;
    //
    number_of_checks_ = number_of_hard_resets_ + 1;
    std::cout << "Setting number of checks to " << number_of_checks_ << std::endl;
  }
  //
  this->PowerOnFixCFEB(in,out);
  //
}
//
void emu::pc::Configurator::setDataReadValues(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb;
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "setDataReadValues: TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "setDataReadValues:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  int dmb;
  name = cgi.getElement("dmb");
  cgicc::form_iterator name2 = cgi.getElement("number_of_reads");
  cgicc::form_iterator name3 = cgi.getElement("pause_btw_reads");
  //
  int number_of_reads    = 1;
  int pause_btw_reads    = 1000000;
  //
  if(name2 != cgi.getElements().end()) 
    number_of_reads = strtol(cgi["number_of_reads"]->getValue().c_str(),NULL,10);
  if(name3 != cgi.getElements().end()) 
    pause_btw_reads = strtol(cgi["pause_btw_reads"]->getValue().c_str(),NULL,10);
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "SetDataReadValues:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "setDataReadValues:  No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  MyTest[tmb][current_crate_].setNumberOfDataReads(number_of_reads);
  MyTest[tmb][current_crate_].setPauseBetweenDataReads(pause_btw_reads);
  //
  this->ChamberTests(in,out);
  //
}
//
void emu::pc::Configurator::TMBL1aTiming(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "TMBL1aTiming" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "TMBL1aTiming");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb, dmb;
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "TMBL1aTiming:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "TMBL1aTiming:  No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMBL1aTiming:  TMB " << std::dec << tmb 
	      << " scan from " << MyTest[tmb][current_crate_].getMinTmbL1aDelayValue()
	      << " to " << MyTest[tmb][current_crate_].getMaxTmbL1aDelayValue()
	      << ", pausing " << MyTest[tmb][current_crate_].getPauseAtEachSetting() << " seconds at each delay value" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "TMBL1aTiming:  No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].FindTMB_L1A_delay();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void emu::pc::Configurator::ALCTL1aTiming(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "ALCTL1aTiming" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "ALCTL1aTiming");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb, dmb;
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "ALCTL1aTiming:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "No dmb" << std::endl;
    dmb = DMB_;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "ALCTL1aTiming:  TMB " << std::dec << tmb 
	      << " scan from " << MyTest[tmb][current_crate_].getMinAlctL1aDelayValue()
	      << " to " << MyTest[tmb][current_crate_].getMaxAlctL1aDelayValue()
	      << ", pausing " << MyTest[tmb][current_crate_].getPauseAtEachSetting() << " seconds at each delay value" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "No tmb" << std::endl;
    tmb = TMB_;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].FindALCT_L1A_delay();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
//void emu::pc::Configurator::ALCTvpf(xgi::Input * in, xgi::Output * out ) 
  //  throw (xgi::exception::Exception) {
//  //
//  std::cout << "EmuPeripheralCrate:  ALCTvpf" << std::endl;
//  LOG4CPLUS_INFO(getApplicationLogger(), "ALCTvpf");
//  //
//  cgicc::Cgicc cgi(in);
//  //
//  int tmb=0;
//  //
//  cgicc::form_iterator name = cgi.getElement("tmb");
//  //
//  if(name != cgi.getElements().end()) {
//    tmb = cgi["tmb"]->getIntegerValue();
//    std::cout << "ALCTvpf:  TMB " << tmb << " will read TMB Raw Hits "
//	 << MyTest[tmb][current_crate_].getNumberOfDataReads() << " times" << std::endl;
//    TMB_ = tmb;
//  } else {
//    std::cout << "ALCTvpf:  No tmb" << std::endl;
//  }
//  //
//  int dmb;
//  name = cgi.getElement("dmb");
//  //
//  if(name != cgi.getElements().end()) {
//    dmb = cgi["dmb"]->getIntegerValue();
//    std::cout << "ALCTvpf:  DMB " << dmb << std::endl;
//    DMB_ = dmb;
//  } else {
//    std::cout << "ALCTvpf:  No dmb" << std::endl;
//  }
//  //
//  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
//  MyTest[tmb][current_crate_].FindALCTinCLCTMatchWindow();
//  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
//  //
//  this->ChamberTests(in,out);
//}
//
void emu::pc::Configurator::ALCTScan(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "ALCTScan" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "ALCTScan");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb=0, dmb=0;
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "ALCTScan:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "ALCTScan:  No dmb" << std::endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "ALCTScan:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "ALCTScan:  No tmb" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].ALCTChamberScan();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void emu::pc::Configurator::CFEBScan(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "CFEBScan" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "CFEBScan");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb=0, dmb=0;
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "CFEBScan:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "CFEBScan:  No dmb" << std::endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "CFEBScan:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "CFEBScan:  No tmb" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].CFEBChamberScan();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void emu::pc::Configurator::FindDistripHotChannel(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Find CLCT Distrip Hot Channels" << std::endl;
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb=0, dmb=0;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if (name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "FindDistripHotChannel:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "FindDistripHotChannel:  No tmb" << std::endl;
  }
  //
  name = cgi.getElement("dmb");
  //
  if (name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "FindDistripHotChannel:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "FindDistripHotChannel:  No dmb" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].FindDistripHotChannels();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void emu::pc::Configurator::FindWinner(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "EmuPeripheralCrate:  FindWinner" << std::endl;
  LOG4CPLUS_INFO(getApplicationLogger(), "FindWinner");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb=0, dmb=0;
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "FindWinner:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "FindWinner:  No dmb" << std::endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "FindWinner:  TMB " << tmb 
	 << ", pausing " << MyTest[tmb][current_crate_].getPauseAtEachSetting() 
	 << " seconds at each delay value" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "FindWinner:  No tmb" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].FindWinner();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
}
//
void emu::pc::Configurator::AlctDavCableDelay(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Measure ALCT DAV cable delay" << std::endl;
  //    LOG4CPLUS_INFO(getApplicationLogger(), "Measure ALCT DAV cable delay");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb=0, dmb=0;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "AlctDavCableDelay:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "AlctDavCableDelay:  No tmb" << std::endl;
  }
  //
  name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "AlctDavCableDelay:  DMB " << dmb 
	 << " Read " << MyTest[tmb][current_crate_].getNumberOfDataReads()
	 << " times, pausing " << MyTest[tmb][current_crate_].getPauseBetweenDataReads() 
	 << "usec between each read" << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "AlctDavCableDelay:  No dmb" << std::endl;
  }
  //
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].MeasureAlctDavCableDelay();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void emu::pc::Configurator::CfebDavCableDelay(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Measure CFEB DAV cable delay" << std::endl;
  //    LOG4CPLUS_INFO(getApplicationLogger(), "Measure CFEB DAV cable delay");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb=0, dmb=0;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "CfebDavCableDelay:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "CfebDavCableDelay:  No tmb" << std::endl;
  }
  //
  name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "CfebDavCableDelay:  DMB " << dmb 
	 << " Read " << MyTest[tmb][current_crate_].getNumberOfDataReads()
	 << " times, pausing " << MyTest[tmb][current_crate_].getPauseBetweenDataReads() 
	 << "usec between each read" << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "CfebDavCableDelay:  No dmb" << std::endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "CfebDavCableDelay:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "CfebDavCableDelay:  No tmb" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].MeasureCfebDavCableDelay();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void emu::pc::Configurator::TmbLctCableDelay(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Measure TMB LCT cable delay" << std::endl;
  //    LOG4CPLUS_INFO(getApplicationLogger(), "Measure TMB LCT cable delay");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb=0, dmb=0;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TmbLctCableDelay:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "TmbLctCableDelay:  No tmb" << std::endl;
  }
  //
  name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "TmbLctCableDelay:  DMB " << dmb << std::dec
	      << " Read " << MyTest[tmb][current_crate_].getNumberOfDataReads()
	      << " times, pausing " << MyTest[tmb][current_crate_].getPauseBetweenDataReads() 
	      << "usec between each read" << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "TmbLctCableDelay:  No dmb" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].MeasureTmbLctCableDelay();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void emu::pc::Configurator::PrintDmbValuesAndScopes(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Print All DMB Values and Scopes" << std::endl;
  //    LOG4CPLUS_INFO(getApplicationLogger(), "Print All DMB Values and Scopes");
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb=0, dmb=0;
  //
  cgicc::form_iterator name = cgi.getElement("dmb");
  //
  if(name != cgi.getElements().end()) {
    dmb = cgi["dmb"]->getIntegerValue();
    std::cout << "PrintDmbValuesAndScopes:  DMB " << dmb << std::endl;
    DMB_ = dmb;
  } else {
    std::cout << "PrintDmbValuesAndScopes:  No dmb" << std::endl;
  }
  //
  name = cgi.getElement("tmb");
  //
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "PrintDmbValuesAndScopes:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "PrintDmbValuesAndScopes:  No tmb" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].ReadAllDmbValuesAndScopes();
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].PrintAllDmbValuesAndScopes();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void emu::pc::Configurator::RatTmbTiming(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "RatTmbTiming" << std::endl;
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "RatTmbTiming:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "RatTmbTiming:  No tmb" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].RatTmbDelayScan();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//
void emu::pc::Configurator::RpcRatTiming(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "RpcRatTiming" << std::endl;
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "RpcRatTiming:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "RpcRatTiming:  No tmb" << std::endl;
  }
  //
  MyTest[tmb][current_crate_].RedirectOutput(&ChamberTestsOutput[tmb][current_crate_]);
  MyTest[tmb][current_crate_].RpcRatDelayScan();
  MyTest[tmb][current_crate_].RedirectOutput(&std::cout);
  //
  this->ChamberTests(in,out);
  //
}
//////////////////////////////////////////////////////////////////////////////////////
// TMB utilities
//////////////////////////////////////////////////////////////////////////////////////
void emu::pc::Configurator::TMBPrintCounters(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "TMBPrintCounters "<< std::endl;
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "Start PrintCounters");
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->GetCounters();
  thisTMB->PrintCounters();
  thisTMB->RedirectOutput(&std::cout);
  LOG4CPLUS_INFO(getApplicationLogger(), "Done PrintCounters");
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::TMBResetCounters(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  thisTMB->ResetCounters();
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::TMBCounterForFixedTime(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    TMB_ = tmb;
  }
  //
  int wait_time = 10;
  if(name != cgi.getElements().end()) {
    wait_time = cgi["time_to_wait"]->getIntegerValue();
  }
  //
  std::cout << "Read TMB " << tmb << " counters over " << wait_time << " seconds" << std::endl;
  //
  TMB * thisTMB = tmbVector[tmb];
  thisTMB->ResetCounters();
  //
  ::sleep(wait_time);
  //
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->GetCounters();
  thisTMB->PrintCounters();
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::TriggerTestInjectALCT(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->TriggerTestInjectALCT();
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::armScope(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->scope(1,0,29);
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::forceScope(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->ForceScopeTrigger();
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::readoutScope(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->ClearScintillatorVeto();
  thisTMB->scope(0,1);
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::TriggerTestInjectCLCT(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->TriggerTestInjectCLCT();
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::TMBDumpAllRegisters(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->DumpAllRegisters();
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::TMBClearUserProms(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->CheckAndProgramProm(ChipLocationTmbUserPromTMBClear);
  thisTMB->CheckAndProgramProm(ChipLocationTmbUserPromALCTClear);
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::TMBConfigure(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Configure:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  ALCTController * alct = thisTMB->alctController();
  thisTMB->configure();
  alct->configure();
  //
  this->TMBUtils(in,out);
  //
}
// 
void emu::pc::Configurator::TMBReadConfiguration(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  ALCTController * alct = thisTMB->alctController();
  //
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  alct->RedirectOutput(&OutputStringTMBStatus[tmb]);
  //
  thisTMB->ReadTMBConfiguration();
  thisTMB->PrintTMBConfiguration();
  alct->ReadALCTConfiguration();
  alct->PrintALCTConfiguration();
  //
  thisTMB->RedirectOutput(&std::cout);
  alct->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::TMBCheckConfiguration(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  ALCTController * alct = thisTMB->alctController();
  //
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  alct->RedirectOutput(&OutputStringTMBStatus[tmb]);
  //
  thisTMB->CheckTMBConfiguration();
  alct->CheckALCTConfiguration();
  //
  thisTMB->RedirectOutput(&std::cout);
  alct->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::TMBReadStateMachines(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  //
  thisTMB->ReadVMEStateMachine();
  thisTMB->PrintVMEStateMachine();
  thisTMB->ReadJTAGStateMachine();
  thisTMB->PrintJTAGStateMachine();
  thisTMB->ReadDDDStateMachine();
  thisTMB->PrintDDDStateMachine();
  thisTMB->ReadRawHitsHeader();
  thisTMB->PrintRawHitsHeader();
  //
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::TMBResetSyncError(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  thisTMB->SetSyncErrReset(1);
  thisTMB->WriteRegister(0x120);
  thisTMB->SetSyncErrReset(0);
  thisTMB->WriteRegister(0x120);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::TMBRawHits(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMBRawHits:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->TMBRawhits();
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::ALCTRawHits(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "ALCTRawHits:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  thisTMB->ALCTRawhits();
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::TMBCheckStateMachines(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  thisTMB->RedirectOutput(&OutputStringTMBStatus[tmb]);
  //
  thisTMB->CheckVMEStateMachine();
  thisTMB->CheckJTAGStateMachine();
  thisTMB->CheckDDDStateMachine();
  thisTMB->CheckRawHitsHeader();
  //
  thisTMB->RedirectOutput(&std::cout);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::ALCTStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Not tmb" << std::endl ;
    tmb = TMB_;
  }
  //
  ALCTController * alct = tmbVector[tmb]->alctController();
  //
  Chamber * thisChamber = chamberVector[tmb];
  //
  char Name[100];
  sprintf(Name,"%s ALCT status, crate=%s, TMBslot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),tmbVector[tmb]->slot());

  // MyHeader(in,out,Name);
  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)", ( fsm_.getCurrentState() == 'E' ? 5 : -1 ) );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector", 
			"/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" )
	      .addLink( "Main Page", 
			"/" + getApplicationDescriptor()->getURN() + "/MainPage" )
	      .addLink( "Crate Configuration", 
			"/" + getApplicationDescriptor()->getURN() + "/CrateConfiguration"  )
	      .addLink( "TMB Status", 
			"/" + getApplicationDescriptor()->getURN() + "/TMBStatus"  )
	      .addLink( Name )
	      );
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl;
  //
  *out << cgicc::legend("ALCT Firmware Status").set("style","color:blue") << std::endl ;
  //
  alct->ReadSlowControlId();
  //
  alct->RedirectOutput(out);
  alct->PrintSlowControlId();
  alct->RedirectOutput(&std::cout);
  //
  *out << cgicc::br();
  //
  alct->ReadFastControlId();
  //
  alct->RedirectOutput(out);
  //
  if ( alct->CheckFirmwareDate() ) {
    *out << cgicc::span().set("style","color:green");  
    alct->PrintFastControlId();
    *out << "...OK...";
  } else {
    *out << cgicc::span().set("style","color:red");    
    alct->PrintFastControlId();
    *out << " --->>  BAD  <<--- Should be (" 
	 << std::dec << alct->GetExpectedFastControlDay() 
	 << " "      << alct->GetExpectedFastControlMonth()
	 << " "      << alct->GetExpectedFastControlYear()
	 << ")";
  }
  //
  alct->RedirectOutput(&std::cout);
  //
  *out << cgicc::span();
  *out << cgicc::fieldset();
  //
  //
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  //
  *out << cgicc::legend("Voltages, Currents, and Temperatures").set("style","color:blue") 
       << std::endl ;
  //
  alct->ReadAlctTemperatureAndVoltages();
  //
  alct->RedirectOutput(out);
  alct->PrintAlctTemperature();
  alct->RedirectOutput(&std::cout);
  //
  *out << cgicc::br();    
  //
  *out << cgicc::table().set("border","1").set("cellspacing","1").set("cellpadding","8");
  //
  /////////////////////////////////////////////////
  *out << cgicc::td().set("ALIGN","center");
  *out << "power line" << std::endl;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "+3.3V" << std::endl;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");  
  *out << "+1.8V" << std::endl;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "+5.5V B" << std::endl;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "+5.5V A" << std::endl;
  *out << cgicc::td();
  //
  /////////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","center");  
  *out << "measured V" << std::endl;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  float value_3p3volt = alct->GetAlct_3p3_Voltage();
  if ( value_3p3volt < 3.3*0.95 ||
       value_3p3volt > 3.3*1.05 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
  }
  *out << std::setprecision(2) << value_3p3volt << "V";
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  float value_1p8volt = alct->GetAlct_1p8_Voltage();
  if ( value_1p8volt < 1.8*0.95 ||
       value_1p8volt > 1.8*1.95 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
  }
  *out << std::setprecision(2) << value_1p8volt << "V";
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  float value_5p5voltb = alct->GetAlct_5p5b_Voltage();
  // The +5.5V_B ADC inputs on ALCT288 boards are shorted to ground, hence, the ADC values are not useful
  if ( alct->GetNumberOfAfebs() <= 18 ) {
    *out << cgicc::span().set("style","color:black");
  } else if ( value_5p5voltb < 5.5*0.95 ||
	      value_5p5voltb > 5.5*1.05 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
  }
  // The +5.5V_B ADC inputs on ALCT288 boards are shorted to ground, hence, the ADC values are not useful
  if ( alct->GetNumberOfAfebs() <= 18 ) {
    *out << "XXXXX";
  } else {
    *out << std::setprecision(2) << value_5p5voltb << " V";
  }
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  float value_5p5volta = alct->GetAlct_5p5a_Voltage();
  if ( value_5p5volta < 5.5*0.95 ||
       value_5p5volta > 5.5*1.05 ) {
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");  
  }
  *out << std::setprecision(2) << value_5p5volta << " V" ;
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //////////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "measured I" << std::endl;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  float value_3p3amps = alct->GetAlct_3p3_Current();
  if ( value_3p3amps < 3.3*0.95 ||
       value_3p3amps > 3.3*1.05 ) {
    *out << cgicc::span().set("style","color:black");
  } else {
    *out << cgicc::span().set("style","color:black");  
  }
  *out << std::setprecision(2) << value_3p3amps << "A" ;
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  //    float value_1p8amps = thisDMB->lowv_adc(3,0)/1000.;
  float value_1p8amps = alct->GetAlct_1p8_Current();
  //
  if ( value_1p8amps < 1.8*0.95 ||
       value_1p8amps > 1.8*1.95 ) {
    *out << cgicc::span().set("style","color:black");
  } else {
    *out << cgicc::span().set("style","color:black");  
  }
  *out << std::setprecision(2) << value_1p8amps << "A" ;
  *out << cgicc::span();
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  //    float value_5p5ampsb = thisDMB->lowv_adc(3,2)/1000.;
  float value_5p5ampsb = alct->GetAlct_5p5b_Current();
  // The +5.5V_B ADC inputs on ALCT288 boards are shorted to ground, hence, the ADC values are not useful...
  if ( alct->GetNumberOfAfebs() <= 18 ) {
    *out << cgicc::span().set("style","color:black");
  } else if ( value_5p5ampsb < 5.5*0.95 ||
	      value_5p5ampsb > 5.5*1.05 ) {
    *out << cgicc::span().set("style","color:black");
  } else {
    *out << cgicc::span().set("style","color:black");  
  }
  if ( alct->GetNumberOfAfebs() <= 18 ) {
    *out << "XXXXX" ;
  } else {
    *out << std::setprecision(2) << value_5p5ampsb << " A" ;
  }
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  float value_5p5ampsa = alct->GetAlct_5p5a_Current();
  if ( value_5p5ampsa < 5.5*0.95 ||
       value_5p5ampsa > 5.5*1.05 ) {
    *out << cgicc::span().set("style","color:black");
  } else {
    *out << cgicc::span().set("style","color:black");  
  }
  *out << std::setprecision(2) << value_5p5ampsa << " A" ;
  *out << cgicc::span();
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  *out << cgicc::fieldset();
  *out << std::endl;
  //
  pageFooter( in, out );
  htmlFooter( in, out );
}
//
void emu::pc::Configurator::RATStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Not tmb" << std::endl ;
    tmb = TMB_;
  }
  //
  RAT * rat = tmbVector[tmb]->getRAT();
  //
  Chamber * thisChamber = chamberVector[tmb];
  //
  char Name[100];
  sprintf(Name,"%s RAT status, crate=%s, TMBslot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),tmbVector[tmb]->slot());
  //
  // MyHeader(in,out,Name);
  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)", ( fsm_.getCurrentState() == 'E' ? 5 : -1 ) );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector", 
			"/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" )
	      .addLink( "Main Page", 
			"/" + getApplicationDescriptor()->getURN() + "/MainPage" )
	      .addLink( "Crate Configuration", 
			"/" + getApplicationDescriptor()->getURN() + "/CrateConfiguration"  )
	      .addLink( "TMB Status", 
			"/" + getApplicationDescriptor()->getURN() + "/TMBStatus"  )
	      .addLink( Name )
	      );
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl;
  //
  *out << cgicc::legend("RAT Status").set("style","color:blue") << std::endl ;
  //
  *out << cgicc::br();
  //
  *out << cgicc::pre();
  //
  tmbTestVector[tmb].testRATuserCodes();
  //
  if ( tmbTestVector[tmb].GetResultTestRATuserCodes() == 1 ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  rat->RedirectOutput(out);
  rat->ReadRatUser1();
  rat->PrintRatUser1();
  rat->RedirectOutput(&std::cout);
  //
  *out << cgicc::span();
  *out << cgicc::pre();
  //
  *out << cgicc::fieldset();
  //
  pageFooter( in, out );
  htmlFooter( in, out );
}
//

//////////////////////////////////////////////////////////////////////////////
// TMB tests methods
//////////////////////////////////////////////////////////////////////////////
void emu::pc::Configurator::TMBTests(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  int tmb;
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMBTests: TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "TMBTests: No tmb" << std::endl ;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  Chamber * thisChamber = chamberVector[tmb];
  //
  char Name[100];
  sprintf(Name,"%s TMB tests, %s slot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisTMB->slot());
  //
  // MyHeader(in,out,Name);
  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)", ( fsm_.getCurrentState() == 'E' ? 5 : -1 ) );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector", 
			"/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" )
	      .addLink( "Main Page", 
			"/" + getApplicationDescriptor()->getURN() + "/MainPage" )
	      .addLink( "Crate Configuration", 
			"/" + getApplicationDescriptor()->getURN() + "/CrateConfiguration"  )
	      .addLink( Name )
	      );
  //
  char buf[20];
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl ;
  //
  std::string RunAllTests = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",RunAllTests) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Run All TMB tests").set("style","color:blue") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","0").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  //
  //
  *out << cgicc::table().set("border","1");
  //
  ///////////////////////////////////////////
  *out << cgicc::td().set("ALIGN","center");
  std::string testBootRegister = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testBootRegister) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestBootRegister() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test Boot register").set("style","color:blue" ) << std::endl ;
    //
  }  else if ( tmbTestVector[tmb].GetResultTestBootRegister() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test Boot register").set("style","color:green") << std::endl ;
    //
  } else {
    *out << cgicc::input().set("type","submit").set("value","TMB test Boot register").set("style","color:red"  ) << std::endl ;
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","1").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testFirmwareDate = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testFirmwareDate) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestFirmwareDate() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware date").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestFirmwareDate() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware date").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware date").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","3").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testFirmwareType = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testFirmwareType) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestFirmwareType() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware type").set("style","color:blue" )  << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestFirmwareType() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware type").set("style","color:green")  << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware type").set("style","color:red"  )  << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","4").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::tr();
  //
  ///////////////////////////////////////////////////////
  *out << cgicc::tr().set("ALIGN","center");
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testFirmwareVersion = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testFirmwareVersion) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestFirmwareVersion() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware version").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestFirmwareVersion() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware version").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware version").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","5").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testFirmwareRevCode = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testFirmwareRevCode) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestFirmwareRevCode() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware RevCode Id").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestFirmwareRevCode() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware RevCode Id").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test firmware RevCode Id").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","6").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //  
  *out << cgicc::td().set("ALIGN","center");
  std::string testMezzId = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testMezzId) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestMezzId() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test mezzanine Id").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestMezzId() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test mezzanine Id").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test mezzanine Id").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","7").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::tr();
  //
  /////////////////////////////////////////////////////////  
  *out << cgicc::tr().set("ALIGN","center");
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testPromId = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testPromId) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestPromId() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test PROM Id").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestPromId() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test PROM Id").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test PROM Id").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","8").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testPROMPath = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testPROMPath) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestPROMPath() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test PROM path").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestPROMPath() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test PROM path").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test PROM path").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","9").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testDSN = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testDSN) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestDSN() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test DSN").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestDSN() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test DSN").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test DSN").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","10").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::tr();
  //
  /////////////////////////////////////////////////////////  
  *out << cgicc::tr().set("ALIGN","center");
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testADC = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testADC) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestADC() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB Voltages and temps").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestADC() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB Voltages and temps").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB Voltages and temps").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","11").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string test3d3444 = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",test3d3444) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTest3d3444() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test 3d3444").set("style","color:blue" ) << std::endl ;
    //  
  } else if ( tmbTestVector[tmb].GetResultTest3d3444() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test 3d3444").set("style","color:green") << std::endl ;
    //
  } else {
    //    
    *out << cgicc::input().set("type","submit").set("value","TMB test 3d3444").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","12").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testRATtemper = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testRATtemper) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestRATtemper() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT temperature").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestRATtemper() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT temperature").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT temperature").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","13").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::tr();
  //
  /////////////////////////////////////////////////////////  
  *out << cgicc::tr().set("ALIGN","center");
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testRATidCodes = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testRATidCodes) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestRATidCodes() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT Id Codes").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestRATidCodes() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT Id Codes").set("style","color:green") << std::endl ;
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT Id Codes").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","14").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //  
  *out << cgicc::td().set("ALIGN","center");
  std::string testRATuserCodes = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testRATuserCodes) << std::endl ;
  if ( tmbTestVector[tmb].GetResultTestRATuserCodes() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT User Codes").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestRATuserCodes() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT User Codes").set("style","color:green") << std::endl ;
    //
  } else {
    *out << cgicc::input().set("type","submit").set("value","TMB test RAT User Codes").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","15").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  std::string testU760K = toolbox::toString("/%s/testTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",testU760K)<< std::endl ;
  if ( tmbTestVector[tmb].GetResultTestU76chip() == -1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test U760K").set("style","color:blue" ) << std::endl ;
    //
  } else if ( tmbTestVector[tmb].GetResultTestU76chip() > 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","TMB test U760K").set("style","color:green") << std::endl ;
    //
  } else {
    *out << cgicc::input().set("type","submit").set("value","TMB test U760K").set("style","color:red"  ) << std::endl ;
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","hidden").set("value","16").set("name","tmbTestid");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  /////////////////////////////////////////////////////////////////////
  //
  *out << cgicc::textarea().set("name","TMBTestOutput").set("WRAP","OFF").set("rows","20").set("cols","100");
  *out << OutputTMBTests[tmb][current_crate_].str() << std::endl ;
  *out << cgicc::textarea();
  //    
  std::string LogTMBTestsOutput = toolbox::toString("/%s/LogTMBTestsOutput",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LogTMBTestsOutput) << std::endl ;
  *out << "Log Filename: /tmp/TMBTestsLogFile_";
  *out << cgicc::input().set("type","text").set("name","LogNameSuffix").set("size","20") << ".log" << cgicc::br() << std::endl;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::input().set("type","submit").set("value","Log output").set("name","LogTMBTestsOutput") << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Clear").set("name","ClearTMBTestsOutput") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();
  //
  pageFooter( in, out );
  htmlFooter( in, out );
}
//
void emu::pc::Configurator::testTMB(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "testTMB:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  name = cgi.getElement("tmbTestid");
  //
  int tmbTestid=0;
  if(name != cgi.getElements().end()) {
    tmbTestid = cgi["tmbTestid"]->getIntegerValue();
    std::cout << "tmbTestid " << tmbTestid << std::endl;
  }
  //
  tmbTestVector[tmb].RedirectOutput(&OutputTMBTests[tmb][current_crate_]);
  if ( tmbTestid == 1 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testBootRegister();
    ::sleep(1);
    //printf("Test1");
  }
  if ( tmbTestid == 2 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testVMEfpgaDataRegister();
    //printf("Test2");
  }
  if ( tmbTestid == 3 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testFirmwareDate();
    //printf("Test3");
  }
  if ( tmbTestid == 4 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testFirmwareType();
    //printf("Test4");
  }
  if ( tmbTestid == 5 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testFirmwareVersion();
    //printf("Test5");
  }
  if ( tmbTestid == 6 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testFirmwareRevCode();
    //printf("Test6");
  }
  if ( tmbTestid == 7 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testMezzId();
    //printf("Test7");
  }
  if ( tmbTestid == 8 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testPROMid();
    //printf("Test8");
  }
  if ( tmbTestid == 9 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testPROMpath();
    //printf("Test9");
  }
  if ( tmbTestid == 10 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testDSN();
    //printf("Test10");
  }
  if ( tmbTestid == 11 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testADC();
    //printf("Test11");
  }
  if ( tmbTestid == 12 || tmbTestid == 0 ) {
    tmbTestVector[tmb].test3d3444();
    //printf("Test12");
  }
  if ( tmbTestid == 13 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testRATtemper();
    //printf("Test13");
  }
  if ( tmbTestid == 14 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testRATidCodes();
    //printf("Test14");
  }
  if ( tmbTestid == 15 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testRATuserCodes();
    //printf("Test15");
  }
  if ( tmbTestid == 16 || tmbTestid == 0 ) {
    tmbTestVector[tmb].testU76chip();
    //printf("Test16");
  }
  //
  tmbTestVector[tmb].RedirectOutput(&std::cout);
  //
  //std::cout << "Done" << std::endl ;
  //
  this->TMBTests(in,out);
  //
}
//
///////////////////////////////////////////////////////////////////////
// TMB utilities methods
///////////////////////////////////////////////////////////////////////
void emu::pc::Configurator::TMBStatus(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMBStatus:  TMB=" << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "TMBStatus: No TMB" << std::endl ;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  Chamber * thisChamber = chamberVector[tmb];
  //
  char Name[100];
  sprintf(Name,"%s TMB status, crate=%s, slot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisTMB->slot());
  //
  // MyHeader(in,out,Name);
  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)", ( fsm_.getCurrentState() == 'E' ? 5 : -1 ) );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector", 
			"/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" )
	      .addLink( "Main Page", 
			"/" + getApplicationDescriptor()->getURN() + "/MainPage" )
	      .addLink( "Crate Configuration", 
			"/" + getApplicationDescriptor()->getURN() + "/CrateConfiguration"  )
	      .addLink( Name )
	      );
  alct = thisTMB->alctController();
  rat  = thisTMB->getRAT();
  //
  if (alct) {
    std::string ALCTStatus =
      toolbox::toString("/%s/ALCTStatus?tmb=%d",getApplicationDescriptor()->getURN().c_str(),tmb);
    //
    *out << cgicc::a("ALCT Status").set("href",ALCTStatus) << std::endl;
    //
  }
  //
  if (rat) {
    std::string RATStatus =
      toolbox::toString("/%s/RATStatus?tmb=%d",getApplicationDescriptor()->getURN().c_str(),tmb);
    //
    *out << cgicc::a("RAT Status").set("href",RATStatus) << std::endl;
    //
  }
  //
  //*out << cgicc::h1(Name);
  //*out << cgicc::br();
  //
  char buf[200] ;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl;
  //
  *out << cgicc::legend("TMB Info").set("style","color:blue") << std::endl ;
  //
  *out << cgicc::pre();
  //
  // read the registers:
  thisTMB->FirmwareDate();
  thisTMB->FirmwareYear();
  thisTMB->FirmwareVersion();
  thisTMB->FirmwareRevCode();
  thisTMB->ReadRegister(0xCC);
  //
  // output the register information to the screen in a nice way:
  //
  int month        = thisTMB->GetReadTmbFirmwareMonth();
  int day          = thisTMB->GetReadTmbFirmwareDay();
  int year         = thisTMB->GetReadTmbFirmwareYear();
  int compile_type = thisTMB->GetReadTMBFirmwareCompileType();
  //  std::cout << " compile type = " << std::hex << compile_type << " or... " <<  thisTMB->GetReadTMBFirmwareCompileType() << std::endl;
  //
  sprintf(buf,"TMB Firmware version (month/day/year - compile type) : (%02d/%02d/%04d - %01x)",month,day,year,compile_type);
  //
  if ( thisTMB->CheckFirmwareDate() ) {
    *out << cgicc::span().set("style","color:green");
    *out << buf;
    *out << "...OK...";
    *out << cgicc::span();
  } else {
    *out << cgicc::span().set("style","color:red");
    *out << buf;
    *out << "--->> BAD <<--- should be ("
	 << std::dec << thisTMB->GetExpectedTmbFirmwareMonth()
	 << "/"      << thisTMB->GetExpectedTmbFirmwareDay()
	 << "/"      << thisTMB->GetExpectedTmbFirmwareYear()
	 << " - "   
	 << std::hex << thisTMB->GetTMBFirmwareCompileType() << std::dec
	 << ")";

    *out << cgicc::span();
  }
  *out << cgicc::br();
  //
  //
  int firmware_type = thisTMB->GetReadTmbFirmwareType();
  //
  sprintf(buf,"Firmware Type             : %01x",firmware_type);       
  //
  if ( firmware_type == thisTMB->GetExpectedTmbFirmwareType() ) {
    *out << cgicc::span().set("style","color:green");
    *out << buf;
    *out << cgicc::span();
  } else {
    *out << cgicc::span().set("style","color:red");
    *out << buf;
    *out << "--->> BAD <<--- should be " << std::hex << thisTMB->GetExpectedTmbFirmwareType() << std::dec;
    *out << cgicc::span();
  }
  *out << cgicc::br();
  //
  //
  int firmware_version = thisTMB->GetReadTmbFirmwareVersion();
  //
  if ( firmware_version == thisTMB->GetExpectedTmbFirmwareVersion() ){
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  sprintf(buf,"Firmware Version Code     : %01x ",firmware_version);
  *out << buf ;
  *out << cgicc::span();
  *out << cgicc::br();
  //
  //
  int slot_number = ((thisTMB->FirmwareVersion()>>8)&0x1f);
  //
  if ( slot_number == thisTMB->slot() ){
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  sprintf(buf,"Geographic Address        : %02d ",slot_number);       
  *out << buf ;
  *out << cgicc::span();
  //
  *out << cgicc::br();
  //
  //
  int firmware_revcode = thisTMB->GetReadTmbFirmwareRevcode();       
  //
  sprintf(buf,"Firmware Revision Code    : %04x ",firmware_revcode);
  *out << buf ;
  *out << cgicc::br();
  //
  //
  int power_status = thisTMB->PowerComparator() & 0x1f;
  //
  if ( power_status == 0x1f ) {
    *out << cgicc::span().set("style","color:green");
  } else {
    *out << cgicc::span().set("style","color:red");
  }
  sprintf(buf,"Power Comparator          : %02x ",power_status);       
  *out << buf ;
  *out << cgicc::span();
  //
  //
  *out << cgicc::pre();
  *out << "On-board temperatures and voltages (in decimal) :" << std::endl;
  *out << cgicc::pre();  
  //
  const int TMB_MAX_TEMP = 40;
  //
  int TMBtempFPGA = thisTMB->ReadTMBtempFPGA();  
  if ( TMBtempFPGA > TMB_MAX_TEMP) {  
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");
  }
  *out << std::dec << "TMB (FPGA)       =   " << TMBtempFPGA << " deg C" << std::endl;
  *out << cgicc::span();
  //
  int TMBtempPCB = thisTMB->ReadTMBtempPCB();  
  if ( TMBtempPCB > TMB_MAX_TEMP ) {  
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");
  }
  *out << "TMB (PCB)        =   " << TMBtempPCB << " deg C" << std::endl;
  *out << cgicc::span();
  //
  int RATtempHSink = rat->ReadRATtempHSink();  
  if ( RATtempHSink > TMB_MAX_TEMP ) {  
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");
  }
  *out << "RAT (Heat sink)  =   " << RATtempHSink << " deg C" << std::endl;
  *out << cgicc::span();
  //
  int RATtempPCB = rat->ReadRATtempPCB();  
  if ( RATtempPCB > TMB_MAX_TEMP ) {  
    *out << cgicc::span().set("style","color:red");
  } else {
    *out << cgicc::span().set("style","color:green");
  }
  *out << "RAT (PCB)        =   " << RATtempPCB << " deg C" << std::endl;
  *out << cgicc::span();
  //
  *out << cgicc::table().set("border","1");
  //
  bool adcOK = tmbTestVector[tmb].testADC();
  ////////////////////////////////////////
  *out << cgicc::td().set("ALIGN","left");
  *out << "Power line";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "Voltage (V)";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "Current (A)";
  *out << cgicc::td();
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB 5.0 V";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  if (adcOK) *out << cgicc::span().set("style","color:green");
  else       *out << cgicc::span().set("style","color:red");
  *out << std::setprecision(2) << std::fixed << thisTMB->Get5p0v();
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << std::setprecision(2) << std::fixed << thisTMB->Get5p0a();
  *out << cgicc::td();
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB 3.3 V";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  if (adcOK) *out << cgicc::span().set("style","color:green");
  else       *out << cgicc::span().set("style","color:red");
  *out << std::setprecision(2) << thisTMB->Get3p3v();
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << std::setprecision(2) << thisTMB->Get3p3a();
  *out << cgicc::td();
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB 1.5 V Core";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  if (adcOK) *out << cgicc::span().set("style","color:green");
  else       *out << cgicc::span().set("style","color:red");
  *out << std::setprecision(3) << thisTMB->Get1p5vCore();
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << std::setprecision(2) << thisTMB->Get1p5aCore();
  *out << cgicc::td();
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB 1.5 V TT";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  if (adcOK) *out << cgicc::span().set("style","color:green");
  else       *out << cgicc::span().set("style","color:red");
  *out << std::setprecision(2) << thisTMB->Get1p5vTT();
  *out << cgicc::span();
  *out << cgicc::td();
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB 1.0 V TT";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  if (adcOK) *out << cgicc::span().set("style","color:green");
  else       *out << cgicc::span().set("style","color:red");
  *out << std::setprecision(2) << thisTMB->Get1p0vTT();
  *out << cgicc::span();
  *out << cgicc::td();
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "RAT 1.8 V";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  if (adcOK) *out << cgicc::span().set("style","color:green");
  else       *out << cgicc::span().set("style","color:red");
  *out << std::setprecision(2) << thisTMB->Get1p8vRAT();
  *out << cgicc::span();
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << std::setprecision(2) << thisTMB->Get1p8aRAT();
  *out << cgicc::td();
  ////////////////////////////////////////
  *out << cgicc::table();
  //
  //
  *out << cgicc::br();
  //
  *out << cgicc::pre();
  //
  *out << cgicc::fieldset();
  //
  *out << cgicc::fieldset();
  *out << cgicc::legend("Comparator Badbits").set("style","color:blue") << std::endl ;
  *out << cgicc::pre();
  thisTMB->RedirectOutput(out);
  thisTMB->ReadRegister(0x122);
  thisTMB->PrintTMBRegister(0x122);
  thisTMB->ReadComparatorBadBits();
  thisTMB->PrintComparatorBadBits();
  thisTMB->RedirectOutput(&std::cout);
  *out << cgicc::pre();
  *out << cgicc::fieldset();
  //
  *out << cgicc::fieldset();
  *out << cgicc::legend("Sync Error status").set("style","color:blue") << std::endl ;
  *out << cgicc::pre();
  thisTMB->RedirectOutput(out);
  thisTMB->ReadRegister(0x120);
  thisTMB->PrintTMBRegister(0x120);
  thisTMB->RedirectOutput(&std::cout);
  *out << cgicc::pre();
  *out << cgicc::fieldset();
  //
  *out << cgicc::fieldset();
  *out << cgicc::legend("CLCT Info").set("style","color:blue") << std::endl ;
  *out << cgicc::pre();
  thisTMB->RedirectOutput(out);
  thisTMB->DecodeCLCT();
  thisTMB->PrintCLCT();
  thisTMB->RedirectOutput(&std::cout);
  *out << cgicc::pre();
  *out << cgicc::fieldset();
  //
  if (alct) {
    *out << cgicc::fieldset();
    *out << cgicc::legend("ALCT Info").set("style","color:blue") << std::endl ;
    *out << cgicc::pre();
    thisTMB->RedirectOutput(out);
    thisTMB->DecodeALCT();
    thisTMB->PrintALCT();
    thisTMB->RedirectOutput(&std::cout);
    *out << cgicc::pre();
    *out << cgicc::fieldset();
  }
  //
  *out << std::endl;    
  //
  pageFooter( in, out );
  htmlFooter( in, out );  
}
//
void emu::pc::Configurator::TMBUtils(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMBUtils:  TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "TMBUtils:  No TMB" << std::endl ;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  Chamber * thisChamber = chamberVector[tmb];
  //
  char Name[100];
  sprintf(Name,"%s TMB utilities, crate=%s slot=%d",
	  (thisChamber->GetLabel()).c_str(), ThisCrateID_.c_str(),thisTMB->slot());

  //
  alct = thisTMB->alctController();
  rat = thisTMB->getRAT();
  //
  // MyHeader(in,out,Name);
  htmlHeader( in, out, "Yellow Page (Emu Peripheral Crate Configurator)", ( fsm_.getCurrentState() == 'E' ? 5 : -1 ) );
  pageHeader( in, out, AppNavigator()
	      .addLink( "Configuration Selector", 
			"/" + getApplicationDescriptor()->getURN() + "/ConfigSelectorPage" )
	      .addLink( "Main Page", 
			"/" + getApplicationDescriptor()->getURN() + "/MainPage" )
	      .addLink( "Crate Configuration", 
			"/" + getApplicationDescriptor()->getURN() + "/CrateConfiguration"  )
	      .addLink( Name )
	      );
  //
  char buf[200] ;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl ;
  //
  *out << cgicc::legend("Load Firmware through TMB").set("style","color:blue") ;
  //
  //
  *out << cgicc::table().set("border","2");
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "-->  BE CAREFUL <--";
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "-->  Do NOT power off crate <--";
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  *out << cgicc::br();
  //
  *out << "TMB: " << cgicc::br() << std::endl;
  *out << "firmware version = " << TMBFirmware_[tmb].toString() << ".xsvf" << cgicc::br() << std::endl;
  //
  *out << "Step 1)  Disable DCS monitoring to crates, and TURN OFF ALCTs" << cgicc::br() << std::endl;
  //
  std::string CheckCrateControllerFromTMBPage = toolbox::toString("/%s/CheckCrateControllerFromTMBPage",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckCrateControllerFromTMBPage) << std::endl ;
  if ( crate_controller_status == 1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","Step 2) Check VME Crate Controller").set("style","color:green");
    //
  } else if ( crate_controller_status == 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","Step 2) Check VME Crate Controller").set("style","color:red");
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","Step 2) Check VME Crate Controller").set("style","color:blue");
    //
  }
  *out << cgicc::form() << std::endl ;
  //
  //
  *out << cgicc::table().set("border","0");
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string LoadTMBFirmware = toolbox::toString("/%s/LoadTMBFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadTMBFirmware) << std::endl ;
  sprintf(buf,"Step 3) Load Firmware for TMB in slot %d",tmbVector[tmb]->slot());
  *out << cgicc::input().set("type","submit").set("value",buf) << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","center");
  *out << "... or ...";
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string LoadCrateTMBFirmware = toolbox::toString("/%s/LoadCrateTMBFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",LoadCrateTMBFirmware) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Step 3) Load firmware (broadcast) to all TMBs in this crate") << std::endl ;
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::table();
  //
  bool print_it = false;
  for (int j=0;j<9;j++) 
    if (number_of_tmb_firmware_errors[j] >= 0) 
      print_it = true;
  //
  if (print_it) {
    for (unsigned i=0; i<tmbVector.size(); i++) {
      //
      if (number_of_tmb_firmware_errors[i] < 1) {
	*out << cgicc::span().set("style","color:black");
      } else {
	*out << cgicc::span().set("style","color:red");
      }
      *out << "Number of firmware verify errors for TMB in slot " << tmbVector[i]->slot() 
	   << " = " << number_of_tmb_firmware_errors[i] << cgicc::br() << std::endl;
      *out << cgicc::span() << std::endl ;
    }
  }
  //
  std::string CCBHardResetFromTMBPage = toolbox::toString("/%s/CCBHardResetFromTMBPage",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CCBHardResetFromTMBPage) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Step 4) CCB hard reset") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  std::string CheckTMBFirmware = toolbox::toString("/%s/CheckTMBFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",CheckTMBFirmware) ;
  if ( tmb_vme_ready == 1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","Step 5) Check TMB VME Ready").set("style","color:green");
    //
  } else if ( tmb_vme_ready == 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","Step 5) Check TMB VME Ready").set("style","color:red");
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","Step 5) Check TMB VME Ready").set("style","color:blue");
    //
  }
  *out << cgicc::form() << std::endl ;
  //
  std::string ClearTMBBootReg = toolbox::toString("/%s/ClearTMBBootReg",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ClearTMBBootReg) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Step 6) Enable VME Access to TMB FPGA") << std::endl ;
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::br() << std::endl;
  *out << cgicc::br() << std::endl;
  //
  if (alct) {
    *out << "ALCT: " << cgicc::br() << std::endl;
    *out << "firmware version = " << ALCTFirmware_[tmb].toString() << ".xsvf" << cgicc::br() << std::endl;
    //
    *out << "Step 1)  Disable DCS monitoring to crates" << cgicc::br() << std::endl;
    //
    std::string CheckAbilityToLoadALCT = toolbox::toString("/%s/CheckAbilityToLoadALCT",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",CheckAbilityToLoadALCT) << std::endl ;
    //
    int track_checked = -1;
    //
    for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++)
      if (able_to_load_alct[i] == 0) 
	track_checked = 0;
    //
    for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++)
      if (able_to_load_alct[i] > 0) 
	track_checked++;
    //
    //
    if ( track_checked < 0 ) {
      *out << cgicc::input().set("type","submit").set("value","Step 2) ALCT firmware loading check").set("style","color:blue");
    } else if ( track_checked == 0 ) {
      *out << cgicc::input().set("type","submit").set("value","Step 2) ALCT firmware loading check").set("style","color:green");
    } else {
      *out << cgicc::input().set("type","submit").set("value","Step 2) ALCT firmware loading check").set("style","color:red");
    }
    *out << cgicc::form() << std::endl ;
    //
    //
    *out << cgicc::table().set("border","0");
    //
    *out << cgicc::td().set("ALIGN","left");
    std::string LoadALCTFirmware = toolbox::toString("/%s/LoadALCTFirmware",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",LoadALCTFirmware) << std::endl ;
    sprintf(buf,"Step 3) Load Firmware for ALCT in slot %d",tmbVector[tmb]->slot());
    *out << cgicc::input().set("type","submit").set("value",buf) << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::td().set("ALIGN","center");
    *out << "... or ...";
    *out << cgicc::td();
    //
    *out << cgicc::td().set("ALIGN","left");
    std::string LoadCrateALCTFirmware = toolbox::toString("/%s/LoadCrateALCTFirmware",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",LoadCrateALCTFirmware) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Step 3) Load firmware (serially) to all ALCTs in this crate") << std::endl ;
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    print_it = false;
    for (int j=0;j<9;j++) 
      if (number_of_alct_firmware_errors[j] >= 0) 
	print_it = true;
    //
    if (print_it) {
      for (unsigned i=0; i<tmbVector.size(); i++) {
	if (number_of_alct_firmware_errors[i] < 1) {
	  *out << cgicc::span().set("style","color:black");
	} else {
	  *out << cgicc::span().set("style","color:red");
	}
	*out << "Number of firmware verify errors for ALCT in slot " << tmbVector[i]->slot() 
	     << " = " << number_of_alct_firmware_errors[i] << cgicc::br() << std::endl;
	*out << cgicc::span() << std::endl ;
      }
    }
    //
    *out << cgicc::form().set("method","GET").set("action",CCBHardResetFromTMBPage) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Step 4) CCB hard reset") << std::endl ;
    *out << cgicc::form() << std::endl ;
  }
  //
  *out << cgicc::br() << std::endl;
  *out << cgicc::br() << std::endl;
  //
  if (rat) {
    *out << "RAT: " << cgicc::br() << std::endl;
    *out << "firmware version = " << RATFirmware_[tmb].toString() << cgicc::br() << std::endl;
    *out << "firmware erase version = " << RATFirmwareErase_.toString() << cgicc::br() << std::endl;

    *out << cgicc::table().set("border","0");

    *out << cgicc::td().set("ALIGN","left");
    std::string LoadRATFirmware = toolbox::toString("/%s/LoadRATFirmware",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",LoadRATFirmware) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Step 1) Load RAT Firmware") << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::td().set("ALIGN","center");
    *out << "... or ...";
    *out << cgicc::td();
    //
    *out << cgicc::td().set("ALIGN","left");
    std::string EraseRATFirmware = toolbox::toString("/%s/EraseRATFirmware",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",EraseRATFirmware) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Step 1) Erase RAT Firmware") << std::endl ;
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    *out << cgicc::td();
    //
    *out << cgicc::table();
    //
    *out << cgicc::form().set("method","GET").set("action",CCBHardResetFromTMBPage) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Step 2) CCB hard reset") << std::endl ;
    *out << cgicc::form() << std::endl ;

  }
  //
  *out << cgicc::br() << std::endl;
  *out << cgicc::br() << std::endl;
  //
  if (alct) {
    *out << "ALCT Slow Control: " << cgicc::br() << std::endl;
    *out << "firmware version = " << FirmwareDir_ + ALCT_SLOW_FIRMWARE_FILENAME << cgicc::br() << std::endl;
    //
    *out << "Step 1)  Disable DCS monitoring to crates" << cgicc::br() << std::endl;
    //
    //
    std::string LoadALCTslowFirmware = toolbox::toString("/%s/LoadALCTSlowFirmware",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",LoadALCTslowFirmware) << std::endl ;
    sprintf(buf,"Step 2) Load Slow Control Firmware for ALCT in slot %d",tmbVector[tmb]->slot());
    *out << cgicc::input().set("type","submit").set("value",buf) << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
    *out << cgicc::form().set("method","GET").set("action",CCBHardResetFromTMBPage) << std::endl ;
    *out << cgicc::input().set("type","submit").set("value","Step 3) CCB hard reset") << std::endl ;
    *out << cgicc::form() << std::endl ;
  }
  //
  if (thisTMB->GetHardwareVersion()==2) {
    *out << cgicc::br() << std::endl;
    *out << cgicc::br() << std::endl;
    *out << "new TMB firmware version = " << FirmwareDir_ + "tmb/tmb_me11_virtex6.svf" << cgicc::br() << std::endl;

    std::string LoadVirtex6TMBFirmware = toolbox::toString("/%s/LoadVirtex6TMBFirmware",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",LoadVirtex6TMBFirmware) << std::endl ;
    sprintf(buf,"Load TMB Virtex 6 Firmware in slot %d",tmbVector[tmb]->slot());
    *out << cgicc::input().set("type","submit").set("value",buf) << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
  }

  if (alct->GetHardwareVersion()==2) {
    *out << cgicc::br() << std::endl;
    *out << cgicc::br() << std::endl;
    *out << "new ALCT firmware version = " << FirmwareDir_ + "alct/alct_mez_spartan6.svf" << cgicc::br() << std::endl;
    std::string LoadSpartan6ALCTFirmware = toolbox::toString("/%s/LoadSpartan6ALCTFirmware",getApplicationDescriptor()->getURN().c_str());
    *out << cgicc::form().set("method","GET").set("action",LoadSpartan6ALCTFirmware) << std::endl ;
    sprintf(buf,"Load ALCT Spartan 6 Firmware in slot %d",tmbVector[tmb]->slot());
    *out << cgicc::input().set("type","submit").set("value",buf) << std::endl ;
    sprintf(buf,"%d",tmb);
    *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
    *out << cgicc::form() << std::endl ;
  }
  //
  *out << cgicc::fieldset();
  //
  *out << cgicc::br() << std::endl;
  //
  *out << cgicc::fieldset().set("style","font-size: 11pt; font-family: arial;");
  *out << std::endl ;
  //
  *out << cgicc::legend("Other TMB Utilities").set("style","color:blue") ;
  //
  std::string TMBDumpAllRegisters = toolbox::toString("/%s/TMBDumpAllRegisters",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBDumpAllRegisters) ;
  *out << cgicc::input().set("type","submit").set("value","Dump All TMB VME Registers") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  //
  //--------------------------------------------------------
  *out << cgicc::table().set("border","0");
  //
  ////////////////////////////////////////
  *out << cgicc::td().set("ALIGN","left");
  *out << "FPGA/JTAG tools";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string UnjamTMB = toolbox::toString("/%s/UnjamTMB",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",UnjamTMB) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Unjam TMB JTAG chains") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  std::string ReadbackALCTFirmware = toolbox::toString("/%s/ReadbackALCTFirmware",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",ReadbackALCTFirmware) << std::endl ;
  //  *out << cgicc::input().set("type","submit").set("value","Readback ALCT PROM").set("style","color:blue") << std::endl ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","ntmb"); 
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::td();
  //
  ////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB Counters";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBPrintCounters = toolbox::toString("/%s/TMBPrintCounters",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBPrintCounters) ;
  *out << cgicc::input().set("type","submit").set("value","Print TMB Counters") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() ;
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBResetCounters = toolbox::toString("/%s/TMBResetCounters",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBResetCounters) ;
  *out << cgicc::input().set("type","submit").set("value","Reset TMB Counters") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBCounterForFixedTime = toolbox::toString("/%s/TMBCounterForFixedTime",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBCounterForFixedTime) << std::endl ;
  *out << cgicc::input().set("type","submit").set("value","Read TMB counters--fixed time") << std::endl ;
  *out << "Number of seconds" << std::endl;
  *out << cgicc::input().set("type","text").set("value","10").set("name","time_to_wait") << std::endl ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  ////////////////////////////////////////
  //  *out << cgicc::tr();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  *out << "Inject fake data";
  //  *out << cgicc::td();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  std::string TriggerTestInjectALCT = toolbox::toString("/%s/TriggerTestInjectALCT",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",TriggerTestInjectALCT) ;
  //  *out << cgicc::input().set("type","submit").set("value","TriggerTest : InjectALCT") ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::td();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  std::string TriggerTestInjectCLCT = toolbox::toString("/%s/TriggerTestInjectCLCT",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",TriggerTestInjectCLCT) ;
  //  *out << cgicc::input().set("type","submit").set("value","TriggerTest : InjectCLCT") ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::td();
  //  //
  //  //////////////////////////////////////////////
  //  *out << cgicc::tr();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  *out << "TMB Scope";
  //  *out << cgicc::td();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  std::string armScope = toolbox::toString("/%s/armScope",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",armScope) ;
  //  *out << cgicc::input().set("type","submit").set("value","arm Scope") ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::td();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  std::string readoutScope = toolbox::toString("/%s/readoutScope",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",readoutScope) ;
  //  *out << cgicc::input().set("type","submit").set("value","readout Scope") ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::td();
  //  //
  //  *out << cgicc::td().set("ALIGN","left");
  //  std::string forceScope = toolbox::toString("/%s/forceScope",getApplicationDescriptor()->getURN().c_str());
  //  *out << cgicc::form().set("method","GET").set("action",forceScope) ;
  //  *out << cgicc::input().set("type","submit").set("value","force Scope") ;
  //  sprintf(buf,"%d",tmb);
  //  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  //  *out << cgicc::form() << std::endl ;
  //  *out << cgicc::td();
  //
  //////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB+ALCT Configuration";
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBConfigure = toolbox::toString("/%s/TMBConfigure",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBConfigure) ;
  *out << cgicc::input().set("type","submit").set("value","Configure TMB+ALCT") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBReadConfiguration = toolbox::toString("/%s/TMBReadConfiguration",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBReadConfiguration) ;
  *out << cgicc::input().set("type","submit").set("value","Read TMB+ALCT Configuration") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBCheckConfiguration = toolbox::toString("/%s/TMBCheckConfiguration",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBCheckConfiguration) ;
  if ( thisTMB->GetTMBConfigurationStatus() == 1 &&
       alct->GetALCTConfigurationStatus()   == 1 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","Check TMB+ALCT Configuration").set("style","color:green");
    //
  } else if ( thisTMB->GetTMBConfigurationStatus() == 0 ||
	      alct->GetALCTConfigurationStatus()   == 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","Check TMB+ALCT Configuration").set("style","color:red");
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","Check TMB+ALCT Configuration").set("style","color:blue");
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB+ALCT User PROMS";
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBClearUserProms = toolbox::toString("/%s/TMBClearUserProms",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBClearUserProms) ;
  *out << cgicc::input().set("type","submit").set("value","Clear TMB+ALCT User Proms") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  /////////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "State Machines";
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBReadStateMachines = toolbox::toString("/%s/TMBReadStateMachines",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBReadStateMachines) ;
  *out << cgicc::input().set("type","submit").set("value","Read TMB State Machines") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBCheckStateMachines = toolbox::toString("/%s/TMBCheckStateMachines",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBCheckStateMachines) ;
  if ( thisTMB->GetVMEStateMachineStatus()  == 1 && 
       thisTMB->GetJTAGStateMachineStatus() == 1 && 
       thisTMB->GetDDDStateMachineStatus()  == 1 && 
       thisTMB->GetRawHitsHeaderStatus()    == 1 ) {
    *out << cgicc::input().set("type","submit")
      .set("value","Check TMB State Machines")
      .set("style","color:green");
  } else if ( thisTMB->GetVMEStateMachineStatus()  == 0 || 
	      thisTMB->GetJTAGStateMachineStatus() == 0 || 
	      thisTMB->GetDDDStateMachineStatus()  == 0 || 
	      thisTMB->GetRawHitsHeaderStatus()    == 0 ) {
    //
    *out << cgicc::input().set("type","submit").set("value","Check TMB State Machines").set("style","color:red");
    //
  } else {
    //
    *out << cgicc::input().set("type","submit").set("value","Check TMB State Machines").set("style","color:blue");
    //
  }
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBResetSyncError = toolbox::toString("/%s/TMBResetSyncError",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBResetSyncError) ;
  *out << cgicc::input().set("type","submit").set("value","Clear TMB Sync Error") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "Raw Hits";
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBRawHits = toolbox::toString("/%s/TMBRawHits",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBRawHits) ;
  *out << cgicc::input().set("type","submit").set("value","Read TMB Raw Hits") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string ALCTRawHits = toolbox::toString("/%s/ALCTRawHits",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTRawHits) ;
  *out << cgicc::input().set("type","submit").set("value","Read ALCT Raw Hits") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() << std::endl ;
  *out << cgicc::td();
  //
  //////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "TMB firmware";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string TMBreadFirmware = toolbox::toString("/%s/TMBReadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",TMBreadFirmware) ;
  *out << cgicc::input().set("type","submit").set("value","Read back TMB firmware") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() ;
  *out << cgicc::td();
  //
  //////////////////////////////////////////////
  *out << cgicc::tr();
  //
  *out << cgicc::td().set("ALIGN","left");
  *out << "ALCT firmware";
  *out << cgicc::td();
  //
  //
  *out << cgicc::td().set("ALIGN","left");
  std::string ALCTreadFirmware = toolbox::toString("/%s/ALCTReadFirmware",getApplicationDescriptor()->getURN().c_str());
  *out << cgicc::form().set("method","GET").set("action",ALCTreadFirmware) ;
  *out << cgicc::input().set("type","submit").set("value","Read back ALCT firmware") ;
  sprintf(buf,"%d",tmb);
  *out << cgicc::input().set("type","hidden").set("value",buf).set("name","tmb");
  *out << cgicc::form() ;
  *out << cgicc::td();
  //
  //--------------------------------------------------------
  *out << cgicc::table();
  //
  //
  // Output area
  //
  *out << cgicc::form().set("method","GET") << std::endl ;
  *out << cgicc::pre();
  *out << cgicc::textarea().set("name","CrateTestTMBOutput")
    .set("rows","50")
    .set("cols","150")
    .set("WRAP","OFF");
  *out << OutputStringTMBStatus[tmb].str() << std::endl ;
  *out << cgicc::textarea();
  OutputStringTMBStatus[tmb].str("");
  *out << cgicc::pre();
  *out << cgicc::form() << std::endl ;
  //
  *out << cgicc::fieldset();

  pageFooter( in, out );
  htmlFooter( in, out );
}
//
void emu::pc::Configurator::TMBReadFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name2 = cgi.getElement("tmb");
  int tmb;
  if(name2 != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Select TMB " << tmb << std::endl;
  } else {
    std::cout << "No TMB" << std::endl ;
    tmb=-1;
  }
  //
  TMB * thisTMB=NULL;
  if(tmb>=0 && (unsigned)tmb<tmbVector.size())  thisTMB = tmbVector[tmb];
  if(thisTMB)
  {
    std::string chambername= thisCrate->GetChamber(thisTMB)->GetLabel();
    unsigned t = chambername.find('/');
    unsigned s = chambername.size();
    while(t<=s )
    {
        chambername.replace(t,1,"_");
        t = chambername.find('/');
    }
    std::string mcsfile="/tmp/TMB_"+ chambername + ".mcs";
    std::string jtagfile=XMLDIR+"/tmb.vrf";
    // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset)
    thisCCB->setCCBMode(CCB::VMEFPGA);
      //
    std::cout  << emu::utils::getDateTime() <<  "Reading back TMB firmware from slot " << thisTMB->slot() << std::endl;
      //
    thisTMB->setup_jtag(ChainTmbMezz);
    thisTMB->read_prom(jtagfile.c_str(),mcsfile.c_str());

    // enable VME access to TMB FPGA
    // from function ClearTMBBootReg()
        short unsigned int BootReg;
        thisTMB->tmb_get_boot_reg(&BootReg);
        BootReg &= 0xff7f;                    // Give JTAG chain to the FPGA to configure ALCT on hard reset
        BootReg &= 0xf7ff;                    // Allow FPGA access to the VME register
        thisTMB->tmb_set_boot_reg(BootReg);

    // Put CCB back into DLOG mode to listen to TTC commands...
    thisCCB->setCCBMode(CCB::DLOG);
  }
  //
  this->TMBUtils(in,out);
}
//
void emu::pc::Configurator::ALCTReadFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name2 = cgi.getElement("tmb");
  int tmb;
  if(name2 != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Select TMB " << tmb << std::endl;
  } else {
    std::cout << "No TMB" << std::endl ;
    tmb=-1;
  }
  //
  TMB * thisTMB=NULL;
  if(tmb>=0 && (unsigned)tmb<tmbVector.size())  thisTMB = tmbVector[tmb];
  if(thisTMB)
  {
    std::string chambername= thisCrate->GetChamber(thisTMB)->GetLabel();
    unsigned t = chambername.find('/');
    unsigned s = chambername.size();
    while(t<=s )
    {
        chambername.replace(t,1,"_");
        t = chambername.find('/');
    }
    std::string mcsfile="/tmp/ALCT_"+ chambername + ".mcs";
    std::string jtagfile;
    ALCTController * thisALCT = thisTMB->alctController();
    std::string alcttype=thisALCT->GetChamberType();
    if(alcttype=="ME21" || alcttype=="ME31" || alcttype=="ME41")
       jtagfile=XMLDIR+"/alct_big.vrf";
    else
       jtagfile=XMLDIR+"/alct_small.vrf";
    // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset)
    thisCCB->setCCBMode(CCB::VMEFPGA);
      //
    std::cout  << emu::utils::getDateTime() << " Reading back ALCT firmware from slot " << thisTMB->slot() << std::endl;
      //
    thisTMB->setup_jtag(ChainAlctFastMezz);
    thisTMB->read_prom(jtagfile.c_str(),mcsfile.c_str());

    // Put CCB back into DLOG mode to listen to TTC commands...
    thisCCB->setCCBMode(CCB::DLOG);
  }
  //
  this->TMBUtils(in,out);
}
//
void emu::pc::Configurator::LoadALCTSlowFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name2 = cgi.getElement("tmb");
  int tmb;
  if(name2 != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Select TMB " << tmb << std::endl;
  } else {
    std::cout << "No TMB" << std::endl ;
    tmb=-1;
  }
  //
  TMB * thisTMB=NULL;
  if(tmb>=0 && (unsigned)tmb<tmbVector.size())  thisTMB = tmbVector[tmb];
  if(thisTMB)
  {
    std::string chambername = thisCrate->GetChamber(thisTMB)->GetLabel();
    std::string svffile = FirmwareDir_ + ALCT_SLOW_FIRMWARE_FILENAME;
    thisCCB->setCCBMode(CCB::VMEFPGA);
      //
    std::cout  << emu::utils::getDateTime() << " Download ALCT Slow Control firmware to " << chambername << std::endl;
      //
    thisTMB->setup_jtag(ChainAlctSlowMezz);
    thisTMB->svfLoad(0,svffile.c_str(), 0, 1);

    // Put CCB back into DLOG mode to listen to TTC commands...
    thisCCB->setCCBMode(CCB::DLOG);
  }
  //
  this->TMBUtils(in,out);
}
//
void emu::pc::Configurator::LoadVirtex6TMBFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name2 = cgi.getElement("tmb");
  int tmb;
  if(name2 != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Select TMB " << tmb << std::endl;
  } else {
    std::cout << "No TMB" << std::endl ;
    tmb=-1;
  }
  //
  TMB * thisTMB=NULL;
  if(tmb>=0 && (unsigned)tmb<tmbVector.size())  thisTMB = tmbVector[tmb];
  if(thisTMB && (thisTMB->GetHardwareVersion()==2))
  {
    std::string svffile = FirmwareDir_ + "tmb/tmb_me11_virtex6.svf";
    // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset)
    thisCCB->setCCBMode(CCB::VMEFPGA);
      //
    std::cout  << emu::utils::getDateTime() <<  " Write TMB ME11 (Virtex 6) firmware to slot " << thisTMB->slot() << std::endl;
      //
    thisTMB->setup_jtag(ChainTmbMezz);
    thisTMB->svfLoad(0,svffile.c_str(), 0, 1);

    // enable VME access to TMB FPGA
    // from function ClearTMBBootReg()
        short unsigned int BootReg;
        thisTMB->tmb_get_boot_reg(&BootReg);
        BootReg &= 0xff7f;                    // Give JTAG chain to the FPGA to configure ALCT on hard reset
        BootReg &= 0xf7ff;                    // Allow FPGA access to the VME register
        thisTMB->tmb_set_boot_reg(BootReg);

    // Put CCB back into DLOG mode to listen to TTC commands...
    thisCCB->setCCBMode(CCB::DLOG);
  }
  //
this->TMBUtils(in,out);
}

//
void emu::pc::Configurator::LoadSpartan6ALCTFirmware(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name2 = cgi.getElement("tmb");
  int tmb;
  if(name2 != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Select TMB " << tmb << std::endl;
  } else {
    std::cout << "No TMB" << std::endl ;
    tmb=-1;
  }
  //
  TMB * thisTMB=NULL;
  if(tmb>=0 && (unsigned)tmb<tmbVector.size())  thisTMB = tmbVector[tmb];
  if(thisTMB)
  {
    ALCTController * thisALCT = thisTMB->alctController();
    if(thisALCT && (thisALCT->GetHardwareVersion()==2))
    {
       std::string svffile = FirmwareDir_ + "alct/alct_mez_spartan6.svf";
       // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset)
       thisCCB->setCCBMode(CCB::VMEFPGA);
       //
       std::cout  << emu::utils::getDateTime() <<  "Write new ALCT Mezzanine (Spartan 6) firmware to slot " << thisTMB->slot() << std::endl;
       //
       thisTMB->setup_jtag(ChainAlctFastMezz);
       thisTMB->svfLoad(0,svffile.c_str(), 0, 1);

       // Put CCB back into DLOG mode to listen to TTC commands...
       thisCCB->setCCBMode(CCB::DLOG);
    }
  }
  //
this->TMBUtils(in,out);
}

//
void emu::pc::Configurator::DefineFirmwareFilenames() {
  //
  std::string DMBFirmware = FirmwareDir_+DMB_FIRMWARE_FILENAME;
  std::string DMBVerify   = FirmwareDir_+DMB_VERIFY_FILENAME;
  std::string DMBCompare  = FirmwareDir_+DMB_COMPARE_FILENAME;
  DMBFirmware_ = DMBFirmware;
  DMBVerify_   = DMBVerify;
  DMBCompare_  = DMBCompare;
  //
  std::string DMBVmeFirmware = FirmwareDir_+DMBVME_FIRMWARE_FILENAME;
  std::string DMBVmeVerify   = FirmwareDir_+DMBVME_VERIFY_FILENAME;
  std::string DMBVmeCompare  = FirmwareDir_+DMBVME_COMPARE_FILENAME;
  DMBVmeFirmware_ = DMBVmeFirmware;
  DMBVmeVerify_   = DMBVmeVerify;
  DMBVmeCompare_  = DMBVmeCompare;
  //
  std::string CFEBFirmware = FirmwareDir_+CFEB_FIRMWARE_FILENAME;
  std::string CFEBVerify = FirmwareDir_+CFEB_VERIFY_FILENAME;
  std::string CFEBCompare = FirmwareDir_+CFEB_COMPARE_FILENAME;
  CFEBVerify_ = CFEBVerify;
  CFEBCompare_ = CFEBCompare;
  CFEBFirmware_ = CFEBFirmware;

  //  create filename for CCB & MPC
    int year  = (thisCCB->GetExpectedFirmwareYear())%100;
    int month = thisCCB->GetExpectedFirmwareMonth();
    int day   = thisCCB->GetExpectedFirmwareDay();
    char ccbdate[7];
    sprintf(ccbdate,"%02u%02u%02u",month,day,year);
    std::ostringstream CCBFirmware;
    CCBFirmware << FirmwareDir_ << "ccb/ccb2004p_" << ccbdate << ".svf";
    CCBFirmware_ = CCBFirmware.str();

    year  = (thisMPC->GetExpectedFirmwareYear())%100;
    month = thisMPC->GetExpectedFirmwareMonth();
    day   = thisMPC->GetExpectedFirmwareDay();
    char mpcdate[7];
    sprintf(mpcdate,"%02u%02u%02u",month,day,year);
    std::ostringstream MPCFirmware;
    MPCFirmware << FirmwareDir_ << "mpc/mpc2004_" << mpcdate << ".svf";
    MPCFirmware_ = MPCFirmware.str();

  //  std::cout << "CCB firmware name: " << CCBFirmware_ << std::endl;
  //  std::cout << "MPC firmware name: " << MPCFirmware_ << std::endl;

  //
  //create filename for TMB, ALCT, and RAT firmware based on expected dates...
  for (unsigned tmb=0; tmb<tmbVector.size(); tmb++) {
    
    // must skip those broadcast slots. added by Liu, May 09 2008 
    if((tmbVector[tmb]->slot())>21) continue;   
    
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    year  = thisTMB->GetExpectedTmbFirmwareYear();
    month = thisTMB->GetExpectedTmbFirmwareMonth();
    day   = thisTMB->GetExpectedTmbFirmwareDay();
    char tmbdate[9];
    sprintf(tmbdate,"%04u%02u%02u",year,month,day);
    //
    std::ostringstream TMBFirmware;
    TMBFirmware << FirmwareDir_ << "tmb/" << tmbdate;
    //
    if (thisTMB->GetTMBFirmwareCompileType() == 0xa ) { 
      TMBFirmware << "/typeA";
    } else if (thisTMB->GetTMBFirmwareCompileType() == 0xc ) {
      TMBFirmware << "/typeC";
    } else if (thisTMB->GetTMBFirmwareCompileType() == 0xd ) {
      TMBFirmware << "/typeD";
    } else {
      std::cout << " = no type determined" << std::endl;
    }
    TMBFirmware << "/tmb";    // ".xsvf" is added in SetXsvfFilename
    TMBFirmware_[tmb] = TMBFirmware.str();
    //    std::cout << "TMB " << tmb << " load " << TMBFirmware_[tmb].toString() << std::endl;
    //
    //
    ALCTController  * thisALCT = tmbVector[tmb]->alctController();
    //
    year  = thisALCT->GetExpectedFastControlYear() ;
    month = thisALCT->GetExpectedFastControlMonth();
    day   = thisALCT->GetExpectedFastControlDay()  ;
    //
    char alctdate[9];
    sprintf(alctdate,"%4u%02u%02u",year,month,day);
    //
    // pre-DAQ06 format
    //  int expected_year       = thisALCT->GetExpectedFastControlYear() & 0xffff;
    //  int expected_month_tens = (thisALCT->GetExpectedFastControlMonth()>>4) & 0xf;
    //  int expected_month_ones = (thisALCT->GetExpectedFastControlMonth()>>0) & 0xf;
    //  int expected_day_tens   = (thisALCT->GetExpectedFastControlDay()  >>4) & 0xf;
    //  int expected_day_ones   = (thisALCT->GetExpectedFastControlDay()  >>0) & 0xf;
    //  sprintf(alctdate,"%4x%1x%1x%1x%1x",
    //	  expected_year,
    //	  expected_month_tens,
    //	  expected_month_ones,
    //	  expected_day_tens,
    //	  expected_day_ones);
    //
    std::ostringstream ALCTFirmware;
    std::ostringstream ALCTReadback;
    ALCTFirmware << FirmwareDir_ << "alct/" << alctdate << "/";
    ALCTReadback << FirmwareDir_ << "alct/readback/";
    //
    if ( (thisALCT->GetChamberType()).find("ME11") != std::string::npos ) {
      //
      if (thisALCT->GetExpectedFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE &&
	  thisALCT->GetExpectedFastControlNegativePositiveType() == NEGATIVE_FIRMWARE_TYPE ) {
	ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_NEGATIVE;
	ALCTReadback << ALCT_READBACK_FILENAME_ME11_BACKWARD_NEGATIVE;
	//
      } else if (thisALCT->GetExpectedFastControlBackwardForwardType() == BACKWARD_FIRMWARE_TYPE &&
		 thisALCT->GetExpectedFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE ) {
	ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME11_BACKWARD_POSITIVE;
	ALCTReadback << ALCT_READBACK_FILENAME_ME11_BACKWARD_POSITIVE;
	//
      } else if (thisALCT->GetExpectedFastControlBackwardForwardType() == FORWARD_FIRMWARE_TYPE &&
		 thisALCT->GetExpectedFastControlNegativePositiveType() == POSITIVE_FIRMWARE_TYPE ) {
	ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME11_FORWARD_POSITIVE;
	ALCTReadback << ALCT_READBACK_FILENAME_ME11_FORWARD_POSITIVE;
	//
      } else {
	ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME11;
	ALCTReadback << ALCT_READBACK_FILENAME_ME11;
      }
    } else if ( (thisALCT->GetChamberType()).find("ME12") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME12;
      ALCTReadback << ALCT_READBACK_FILENAME_ME12;
    } else if ( (thisALCT->GetChamberType()).find("ME13") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME13;
      ALCTReadback << ALCT_READBACK_FILENAME_ME13;
    } else if ( (thisALCT->GetChamberType()).find("ME21") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME21;
      ALCTReadback << ALCT_READBACK_FILENAME_ME21;
    } else if ( (thisALCT->GetChamberType()).find("ME22") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME22;
      ALCTReadback << ALCT_READBACK_FILENAME_ME22;
    } else if ( (thisALCT->GetChamberType()).find("ME31") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME31;
      ALCTReadback << ALCT_READBACK_FILENAME_ME31;
    } else if ( (thisALCT->GetChamberType()).find("ME32") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME32;
      ALCTReadback << ALCT_READBACK_FILENAME_ME32;
    } else if ( (thisALCT->GetChamberType()).find("ME41") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME41;
      ALCTReadback << ALCT_READBACK_FILENAME_ME41;
    } else if ( (thisALCT->GetChamberType()).find("ME42") != std::string::npos ) {
      ALCTFirmware << ALCT_FIRMWARE_FILENAME_ME42;
      ALCTReadback << ALCT_READBACK_FILENAME_ME42;
    } 
    ALCTFirmware_[tmb] = ALCTFirmware.str();
    ALCTReadback_[tmb] = ALCTReadback.str();
    //    std::cout << "ALCT " << tmb << " load " << ALCTFirmware_[tmb].toString() << std::endl;
    //    std::cout << "ALCT " << tmb << " read " << ALCTReadback_[tmb].toString() << std::endl;
    //
    char date[9];
    sprintf(date,"%4u%02u%02u",
	    thisTMB->GetExpectedRatFirmwareYear() ,
	    thisTMB->GetExpectedRatFirmwareMonth(),
	    thisTMB->GetExpectedRatFirmwareDay()  );
    //
    std::ostringstream RATFirmware; 
    RATFirmware << FirmwareDir_ << "rat/" << date << "/rat.svf";
    //
    RATFirmware_[tmb] = RATFirmware.str();
    //    std::cout << "RAT " << tmb << " load " << RATFirmware_[tmb].toString() << std::endl;
    //
    std::ostringstream RATFirmwareErase; 
    RATFirmwareErase << FirmwareDir_ << "rat/eraserat.svf";
    //
    RATFirmwareErase_ = RATFirmwareErase.str();
    //    std::cout << "RAT firmware erase " << RATFirmwareErase_.toString() << std::endl;
  }
  //
  return;
}
//
void emu::pc::Configurator::CheckCrateControllerFromTMBPage(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {  
  //
  crate_controller_status = (int) 
    (crateVector[current_crate_]->vmeController()->SelfTest()) && 
    (crateVector[current_crate_]->vmeController()->exist(13));
  //
  this->TMBUtils(in,out);
}
//
void emu::pc::Configurator::LoadTMBFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Load firmware for TMB[" << tmb << "]" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "No TMB defined to load... taking default = " << TMB_ << std::endl ;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  tmb_vme_ready = -1;
  //
  number_of_tmb_firmware_errors[tmb]=-1;
  //
  // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during TMB downloading...
  thisCCB->setCCBMode(CCB::VMEFPGA);
  //
  if (thisTMB->slot() < 22) {
    std::cout << "Loading TMB firmware to slot " << thisTMB->slot() 
	      << " with " << TMBFirmware_[tmb].toString()
	      << " in 5 seconds..." << std::endl;
    //
    ::sleep(5);
    //
    thisTMB->SetXsvfFilename(TMBFirmware_[tmb].toString().c_str());
    thisTMB->ProgramTMBProms();
    thisTMB->ClearXsvfFilename();
    //
    number_of_tmb_firmware_errors[tmb] = thisTMB->GetNumberOfVerifyErrors();
    std::cout << "=== Programming TMB firmware finished for slot " << thisTMB->slot() << std::endl;
    std::cout << "=== " << number_of_tmb_firmware_errors[tmb] << " Verify Errors occured" << std::endl;
    //
    if (number_of_tmb_firmware_errors[tmb] < 0) {
      std::cout << "File does not exist, programming did not occur..."<< std::endl;
      //
    } else if (number_of_tmb_firmware_errors[tmb] == 0) {
      std::cout << "Please perform a TTC/CCB hard reset to Load FPGA"<< std::endl;
      //
    } else {
      std::cout << "ERROR!! -> Number of errors = " << number_of_tmb_firmware_errors[tmb] << " not equal to 0!!" << std::endl;
      std::cout << std::endl;
      std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl;
      std::cout << "!!!!     IF you are BROADCASTING TO CRATE, this is OK     !!!! " << std::endl;
      std::cout << "!!!!                                                      !!!! " << std::endl;
      std::cout << "!!!! IF you are NOT, this is a PROBLEM, DO NOT HARD RESET !!!! " << std::endl;
      std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl;
    }
    //
  } 
  //
  // Put CCB back into DLOG mode to listen to TTC commands...
  thisCCB->setCCBMode(CCB::DLOG);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::LoadCrateTMBFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Load TMB firmware crate, called from TMB[" << tmb << "]" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "No TMB defined to load... taking default = " << TMB_ << std::endl ;
    tmb = TMB_;
  }
  //
  bool typeA_only = true;
  int ntmb_typea = 0;
  //
  for (int i=0; i<9;i++) 
    number_of_tmb_firmware_errors[i]=-1;
  //
  // if there is only typeA chambers in this crate, then a single broadcast will suffice...
  //
  for (unsigned ntmb=0;ntmb<(tmbVector.size()<9 ? 9 : tmbVector.size());ntmb++) {
    //
    if (tmbVector[ntmb]->GetClctStagger()) {
      ntmb_typea = ntmb;
    }
    typeA_only &= tmbVector[ntmb]->GetClctStagger();
    //
    //    std::cout << "TMB[" << ntmb << "] csc_stagger = " << tmbVector[ntmb]->GetClctStagger() << std::endl;
  }
  //
  // Create a TMB which all TMB's within a crate will listen to....
  //
  Chamber * thisChamber = new Chamber(thisCrate);  // a dummy chamber
  TMB * thisTMB = new TMB(thisCrate, thisChamber, 26); // must use a dummy chamber, not a real one
  //
  tmb_vme_ready = -1;
  //
  // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during TMB downloading...
  thisCCB->setCCBMode(CCB::VMEFPGA);
  //
  std::cout << "Broadcast TMB firmware " << TMBFirmware_[ntmb_typea].toString()
	    << " to slot " << thisTMB->slot() << " in 5 seconds..." << std::endl;
  ::sleep(5);
  //
  thisTMB->SetXsvfFilename(TMBFirmware_[ntmb_typea].toString().c_str());
  thisTMB->ProgramTMBProms();
  thisTMB->ClearXsvfFilename();
  //
  thisCrate->deleteModule(thisTMB);
  delete thisTMB;
  thisCrate->deleteChamber();
  //
  if (!typeA_only) {
    for (unsigned ntmb=0;ntmb<(tmbVector.size()<9 ? 9 : tmbVector.size());ntmb++) {
      //
      if (!tmbVector[ntmb]->GetClctStagger()) {
	std::cout << "Loading TMB firmware " << TMBFirmware_[ntmb].toString()
		  << " to slot " << tmbVector[ntmb]->slot() << " in 5 seconds..." << std::endl;
	::sleep(5);
	//
	tmbVector[ntmb]->SetXsvfFilename(TMBFirmware_[ntmb].toString().c_str());
	tmbVector[ntmb]->ProgramTMBProms();
	tmbVector[ntmb]->ClearXsvfFilename();
	number_of_tmb_firmware_errors[ntmb] = tmbVector[ntmb]->GetNumberOfVerifyErrors();
      }
    }
  }
  std::cout << "Please perform a TTC/CCB hard reset to Load FPGA"<< std::endl;
  //
  // Put CCB back into DLOG mode to listen to TTC commands...
  thisCCB->setCCBMode(CCB::DLOG);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::CCBHardResetFromTMBPage(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  thisCCB->hardReset();
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::CheckTMBFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Checking TMB VME Ready for all slots in crate" << std::endl;
  //
  tmb_vme_ready = 1;
  //
  for (unsigned tmb=0; tmb<tmbVector.size(); tmb++) {
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    if (thisTMB->slot() < 22) {
      short unsigned int BootReg;
      thisTMB->tmb_get_boot_reg(&BootReg);
      std::cout << "Boot register = 0x" << std::hex << BootReg << std::dec << std::endl;
      //
      if (thisTMB->GetBootVMEReady() != 1) tmb_vme_ready = 0;
    }
    //
  }
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::ClearTMBBootReg(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  if (tmb_vme_ready == 1) {
    //
    for (unsigned tmb=0; tmb<tmbVector.size(); tmb++) {
      //
      TMB * thisTMB = tmbVector[tmb];
      //
      if (thisTMB->slot() < 22) {
	short unsigned int BootReg;
	thisTMB->tmb_get_boot_reg(&BootReg);
	BootReg &= 0xff7f;                    // Give JTAG chain to the FPGA to configure ALCT on hard reset
	BootReg &= 0xf7ff;                    // Allow FPGA access to the VME register
	thisTMB->tmb_set_boot_reg(BootReg);
	//
      }
    }
  } else {
    //
    std::cout << "TMB is not ready for VME access" << std::endl;
  }
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::UnjamTMB(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  //
  int tmb=0;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  thisTMB->UnjamFPGA();
  //
  this->TMBUtils(in,out);
  //
}
//
//
void emu::pc::Configurator::CheckAbilityToLoadALCT(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  std::cout << "Check ability to load firmware for all ALCTs in this crate..." << std::endl;
  //
  int check_value[10] = {};
  //
  for (unsigned i=0; i<tmbVector.size(); i++) 
    check_value[i] = tmbVector[i]->alctController()->CheckFirmwareConfiguration();
  //
  // print out the results
  //
  for (unsigned i=0; i<tmbVector.size(); i++) {
    //
    std::cout << "TMB in slot " << tmbVector[i]->slot() << " ... ";
    if (check_value[i] == 1) {
      able_to_load_alct[i] = 0;
      std::cout << "OK";
    } else if (check_value[i] == 0) {
      able_to_load_alct[i] = 1;
      std::cout << " ---> FAIL <---";
    }
    std::cout << std::endl;
  }
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::LoadALCTFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Load Firmware for ALCT[" << tmb << "]" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "No ALCT defined to load taking default = " << TMB_ << std::endl ;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  ALCTController  * thisALCT = thisTMB->alctController();
  number_of_alct_firmware_errors[tmb]=-1;
  //
  if (!thisALCT) {
    std::cout << "This ALCT not defined" << std::endl;
    this->TMBUtils(in,out);
  }
  if (able_to_load_alct[tmb] != 0) {
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "---- ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR------" << std::endl;
    std::cout << "---- Firmware database check did not pass for this crate. ------" << std::endl;
    std::cout << "---- ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR------" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    this->TMBUtils(in,out);
  }
  // reset the ALCT check button
  able_to_load_alct[tmb] = -1;
  //
  // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during ALCT downloading...
  thisCCB->setCCBMode(CCB::VMEFPGA);
  //
  LOG4CPLUS_INFO(getApplicationLogger(), "Program ALCT firmware");
  //
  std::cout <<  "Loading ALCT firmware to slot " << thisTMB->slot() 
	    << " with " << ALCTFirmware_[tmb].toString() 
	    << " in 5 seconds...  Current firmware types are:" << std::endl;
  //
  thisALCT->ReadSlowControlId();
  thisALCT->PrintSlowControlId();
  //
  thisALCT->ReadFastControlId();
  thisALCT->PrintFastControlId();
  //
  ::sleep(5);
  //
  thisTMB->disableAllClocks();
  //
  thisTMB->SetXsvfFilename(ALCTFirmware_[tmb].toString().c_str());
  thisALCT->ProgramALCTProms();
  thisTMB->ClearXsvfFilename();
  number_of_alct_firmware_errors[tmb] = thisTMB->GetNumberOfVerifyErrors();
  //
  // programming with svf file to be deprecated, since it cannot verify...
  //  int debugMode(0);
  //  int jch(3);
  //  int status = thisALCT->SVFLoad(&jch,ALCTFirmware_[tmb].toString().c_str(),debugMode);
  //
  thisTMB->enableAllClocks();
  //
  if (number_of_alct_firmware_errors[tmb] >= 0){
    LOG4CPLUS_INFO(getApplicationLogger(), "Program ALCT firmware finished");
    std::cout << "=== Programming finished"<< std::endl;
    std::cout << "=== " << number_of_alct_firmware_errors[tmb] << " Verify Errors  occured" << std::endl;
  } else {
    std::cout << "=== Fatal Error. Exiting with " << number_of_alct_firmware_errors[tmb] << std::endl;
  }
  //
  // Put CCB back into DLOG mode to listen to TTC commands...
  thisCCB->setCCBMode(CCB::DLOG);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::LoadCrateALCTFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  for (int j=0;j<9;j++)
    number_of_alct_firmware_errors[j]=-1;
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "Load ALCT firmware crate, called from TMB[" << tmb << "]" << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "No ALCT defined to load... taking default = " << TMB_ << std::endl ;
    tmb = TMB_;
  }
  //
  // Put CCB in FPGA mode to make the CCB ignore TTC commands (such as hard reset) during ALCT downloading...
  thisCCB->setCCBMode(CCB::VMEFPGA);
  //
  for (unsigned i=0; i<tmbVector.size(); i++) {
    TMB * thisTMB = tmbVector[i];
    ALCTController  * thisALCT = thisTMB->alctController();
    //
    if (!thisALCT) {
      std::cout << "This ALCT not defined" << std::endl;
      this->TMBUtils(in,out);
    }
    //
    if (able_to_load_alct[i] != 0) {
      std::cout << "----------------------------------------------------------------" << std::endl;
      std::cout << "---- ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR------" << std::endl;
      std::cout << "---- Firmware database check did not pass for this crate. ------" << std::endl;
      std::cout << "---- ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR------" << std::endl;
      std::cout << "----------------------------------------------------------------" << std::endl;
      this->TMBUtils(in,out);
    }
    // reset the ALCT check button
    able_to_load_alct[i] = -1;
    //
    LOG4CPLUS_INFO(getApplicationLogger(), "Program ALCT firmware");
    //
    std::cout <<  "Loading ALCT firmware to slot " << std::dec << thisTMB->slot() 
	      << " with " << ALCTFirmware_[i].toString() 
	      << " in 5 seconds...  Current firmware types are:" << std::endl;
    //
    thisALCT->ReadSlowControlId();
    thisALCT->PrintSlowControlId();
    //
    thisALCT->ReadFastControlId();
    thisALCT->PrintFastControlId();
    //
    ::sleep(5);
    //
    thisTMB->disableAllClocks();
    //
    thisTMB->SetXsvfFilename(ALCTFirmware_[i].toString().c_str());
    thisALCT->ProgramALCTProms();
    thisTMB->ClearXsvfFilename();
    number_of_alct_firmware_errors[i] = thisTMB->GetNumberOfVerifyErrors();
    //
    // programming with svf file to be deprecated, since it cannot verify...
    //    int debugMode(0);
    //    int jch(3);
    //    int status = thisALCT->SVFLoad(&jch,ALCTFirmware_[i].toString().c_str(),debugMode);
    //
    thisTMB->enableAllClocks();
    //
    if (number_of_alct_firmware_errors[i] >= 0){
      LOG4CPLUS_INFO(getApplicationLogger(), "Program ALCT firmware finished");
      std::cout << "=== Programming finished"<< std::endl;
      std::cout << "=== " << number_of_alct_firmware_errors[i] << " Verify Errors  occured" << std::endl;
    } else {
      std::cout << "=== Fatal Error. Exiting with " << number_of_alct_firmware_errors[i] << std::endl;
    }
  }
  //
  // Put CCB back into DLOG mode to listen to TTC commands...
  thisCCB->setCCBMode(CCB::DLOG);
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::LoadRATFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Not tmb" << std::endl ;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  std::cout << "Loading RAT firmware " << std::endl;
  //
  rat = thisTMB->getRAT();
  if (!rat) {
    std::cout << "No RAT present" << std::endl;
    return;
  }
  //
  thisTMB->disableAllClocks();
  //
  int debugMode(0);
  int jch(7);
  int status = rat->SVFLoad(&jch,RATFirmware_[tmb].toString().c_str(),debugMode);
  //
  thisTMB->enableAllClocks();
  //
  if (status >= 0){
    std::cout << "=== Programming finished"<< std::endl;
    //    std::cout << "=== " << status << " Verify Errors  occured" << std::endl;
  }
  else{
    std::cout << "=== Fatal Error. Exiting with " <<  status << std::endl;
  }
  //
  this->TMBUtils(in,out);
  //
}
//
void emu::pc::Configurator::EraseRATFirmware(xgi::Input * in, xgi::Output * out ) 
  throw (xgi::exception::Exception) {
  //
  cgicc::Cgicc cgi(in);
  //
  cgicc::form_iterator name = cgi.getElement("tmb");
  int tmb;
  if(name != cgi.getElements().end()) {
    tmb = cgi["tmb"]->getIntegerValue();
    std::cout << "TMB " << tmb << std::endl;
    TMB_ = tmb;
  } else {
    std::cout << "Not tmb" << std::endl ;
    tmb = TMB_;
  }
  //
  TMB * thisTMB = tmbVector[tmb];
  //
  std::cout << "Erasing RAT firmware " << std::endl;
  //
  rat = thisTMB->getRAT();
  if (!rat) {
    std::cout << "No RAT present" << std::endl;
    return;
  }
  //
  thisTMB->disableAllClocks();
  //
  int debugMode(0);
  int jch(7);
  int status = rat->SVFLoad(&jch,RATFirmwareErase_.toString().c_str(),debugMode);
  //
  thisTMB->enableAllClocks();
  //
  if (status >= 0){
    std::cout << "=== Programming finished"<< std::endl;
    //    std::cout << "=== " << status << " Verify Errors  occured" << std::endl;
  }
  else{
    std::cout << "=== Fatal Error. Exiting with " <<  status << std::endl;
  }
  //
  this->TMBUtils(in,out);
  //
}
//
//////////////////////////////////////////////////////////////////
// Logging information
///////////////////////////////////////////////////////////////////
  void emu::pc::Configurator::LogTestSummary(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception){
    //
    SaveTestSummary();
    //
    this->Default(in,out);
    //
  }
  //
  void emu::pc::Configurator::LogOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception){
    //
    SaveLog();
    //
    this->Default(in,out);
    //
  }
void emu::pc::Configurator::SaveLog() {
  //
  int initial_crate = current_crate_;
  //
  // get the date and time of this check:
  time_t rawtime;
  time(&rawtime);
  //
  std::string buf;
  std::string time_dump = ctime(&rawtime);
  std::string time = time_dump.substr(0,time_dump.length()-1);
  //
  while( time.find(" ",0) != std::string::npos ) {
    //
    int thispos = time.find(" ",0); 
    time.replace(thispos,1,"_");
    //
  }
  //
  buf = "/tmp/EmuPeripheralCrateLogFile_"+time+".log";
  //
  std::cout << "Logging output to" << buf << std::endl ;
  //
  std::ifstream TextFile ;
  TextFile.open(xmlFile_.toString().c_str());
  //
  std::ofstream LogFile;
  LogFile.open(buf.c_str());
  while(TextFile.good()) LogFile << (char) TextFile.get() ;
  TextFile.close();
  //
  LogFile << CrateTestsOutput.str();
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    SetCurrentCrate(crate_number);
    //
    for (unsigned int i=0; i<tmbVector.size(); i++) {
      LogFile << OutputTMBTests[i][current_crate_].str() ;
      LogFile << ChamberTestsOutput[i][current_crate_].str() ;
      LogFile << ALCT_TMB_communicationOutput[i][current_crate_].str() ;
    }
    for (unsigned int i=0; i<dmbVector.size(); i++) {
      LogFile << OutputDMBTests[i][current_crate_].str() ;
    }
  }
  //
  LogFile.close();    
  //
  SetCurrentCrate(initial_crate);
  //
  return;
}
//
void emu::pc::Configurator::SaveTestSummary() {
  //
  int initial_crate = current_crate_;
  //
  time_t rawtime;
  time(&rawtime);
  //
  std::string buf;
  std::string time_dump = ctime(&rawtime);
  std::string time = time_dump.substr(0,time_dump.length()-1);
  //
  while( time.find(" ",0) != std::string::npos ) {
    //
    int thispos = time.find(" ",0); 
    time.replace(thispos,1,"_");
    //
  }
  //
  buf = "/tmp/EmuPeripheralCrateTestSummary_"+time+".log";
  //
  std::ofstream LogFile;
  LogFile.open(buf.c_str());
  //
  LogFile << " *** Output : Test Summary *** " << std::endl ;
  //
  LogFile << std::endl;
  //
  for(unsigned crate_number=0; crate_number< crateVector.size(); crate_number++) {
    //
    SetCurrentCrate(crate_number);
    //
    for(int i=0; i<20; i++) LogFile << "+";
    LogFile << std::endl ;
    LogFile << " Timing scans : " << std::endl;
    for(int i=0; i<20; i++) LogFile << "-";
    LogFile << std::endl ;
    //
    for (unsigned int i=0; i<(tmbVector.size()<9?tmbVector.size():9) ; i++) {
      //	
      Chamber * thisChamber = chamberVector[i];
      //
      LogFile << "slot                  " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << tmbVector[i]->slot()
	      << std::endl;
      LogFile << "cfeb0delay            " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(0)
	      << std::endl;
      LogFile << "cfeb1delay            " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(1)
	      << std::endl;
      LogFile << "cfeb2delay            " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(2)
	      << std::endl;
      LogFile << "cfeb3delay            " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(3)
	      << std::endl;
      LogFile << "cfeb4delay            " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPhaseTest(4)
	      << std::endl;
      LogFile << "cfeb0posneg           " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPosnegTest(0)
	      << std::endl;
      LogFile << "cfeb1posneg           " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPosnegTest(1)
	      << std::endl;
      LogFile << "cfeb2posneg           " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPosnegTest(2)
	      << std::endl;
      LogFile << "cfeb3posneg           " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPosnegTest(3)
	      << std::endl;
      LogFile << "cfeb4posneg           " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxPosnegTest(4)
	      << std::endl;
      LogFile << "cfeb0_rxd_int_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxdIntDelayTest(0)
	      << std::endl;
      LogFile << "cfeb1_rxd_int_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxdIntDelayTest(1)
	      << std::endl;
      LogFile << "cfeb2_rxd_int_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxdIntDelayTest(2)
	      << std::endl;
      LogFile << "cfeb3_rxd_int_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxdIntDelayTest(3)
	      << std::endl;
      LogFile << "cfeb4_rxd_int_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCFEBrxdIntDelayTest(4)
	      << std::endl;
      LogFile << "alct_tx_clock_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetALCTtxPhaseTest()
	      << std::endl;
      LogFile << "alct_rx_clock_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetALCTrxPhaseTest()
	      << std::endl;
      LogFile << "alct_posneg           " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetAlctRxPosNegTest()
	      << std::endl;
      LogFile << "alct_tx_posneg        " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetAlctTxPosNegTest()
	      << std::endl;
      LogFile << "alct_bx0_delay        " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetAlctBx0DelayTest()
	      << std::endl;
      LogFile << "match_trig_alct_delay " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetMatchTrigAlctDelayTest()
	      << std::endl;
      LogFile << "tmb_bxn_offset        " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetTmbBxnOffsetTest()
	      << std::endl;
      LogFile << "rat_tmb_delay         " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetRatTmbDelayTest()
	      << std::endl;
      LogFile << "mpc_rx_delay          " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetMpcRxDelayTest()
	      << std::endl;
      LogFile << "tmb_lct_cable_delay   " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetTmbLctCableDelayTest()
	      << std::endl;
      LogFile << "alct_dav_cable_delay  " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetAlctDavCableDelayTest()
	      << std::endl;
      LogFile << "cfeb_dav_cable_delay  " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetCfebDavCableDelayTest()
	      << std::endl;
      LogFile << "tmb_l1a_delay         " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetTmbL1aDelayTest()
	      << std::endl;
      LogFile << "alct_l1a_delay        " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetAlctL1aDelayTest()
	      << std::endl;
      LogFile << "rpc0_rat_delay        " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << MyTest[i][current_crate_].GetRpcRatDelayTest(0)
	      << std::endl;
      LogFile << "TTCrxID               " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(5) << thisCCB->GetReadTTCrxID() 
	      << std::endl;
      LogFile << "best_avg_aff_to_l1a      " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(10) << MyTest[i][current_crate_].GetBestAverageAFFtoL1A()
	      << std::endl;
      LogFile << "best_avg_alct_dav_scope  " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(10) << MyTest[i][current_crate_].GetBestAverageALCTDAVScope()
	      << std::endl;
      LogFile << "best_avg_cfeb_dav_scope  " 
	      << std::setw(10) << (thisChamber->GetLabel()).c_str()
	      << std::setw(10) << MyTest[i][current_crate_].GetBestAverageCFEBDAVScope()
	      << std::endl;
      //      for (int CFEBs = 0; CFEBs<5; CFEBs++) {
      //	LogFile << "cfeb" << CFEBs << "_scan " << std::setw(3) << i;
      //	for (int HalfStrip = 0; HalfStrip<32; HalfStrip++) 
      //	  LogFile << std::setw(3) << MyTest[i][current_crate_].GetCFEBStripScan(CFEBs,HalfStrip) ;
      //	LogFile << std::endl;
      //      }
      //      //
      //      LogFile << "alct_scan  " << std::setw(3) << i;
      //      for (int Wire = 0; Wire<(tmbVector[i]->alctController()->GetNumberOfChannelsInAlct())/6; Wire++) 
      //	LogFile << std::setw(3) << MyTest[i][current_crate_].GetALCTWireScan(Wire) ;
      //      LogFile << std::endl;
      //      //
      LogFile << std::endl;
    }
  }
  //
  LogFile.close();
  //
  SetCurrentCrate(initial_crate);
  //
  return;
}
  //
  void emu::pc::Configurator::LogDMBTestsOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "LogDMBTestsOutput" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    //
    cgicc::form_iterator name = cgi.getElement("dmb");
    //
    //
    int dmb=0;
    if(name != cgi.getElements().end()) {
      dmb = cgi["dmb"]->getIntegerValue();
      std::cout << "DMB " << dmb << std::endl;
      DMB_ = dmb;
    } else {
      std::cout << "Not dmb" << std::endl ;
      dmb = DMB_;
    }
    //
    cgicc::form_iterator name2 = cgi.getElement("ClearDMBTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      std::cout << "Clear..." << std::endl;
      std::cout << cgi["ClearDMBTestsOutput"]->getValue() << std::endl ;
      OutputDMBTests[dmb][current_crate_].str("");
      OutputDMBTests[dmb][current_crate_] << "DMB-CFEB Tests " 
					  << thisCrate->GetChamber(dmbVector[dmb]->slot())->GetLabel().c_str() 
					  << " output:" << std::endl;

      //
    this->DMBTests(in,out);
    return;
    }
    //
    DAQMB * thisDMB = dmbVector[dmb];
    //
    char buf[20];
    sprintf(buf,"/tmp/DMBTestsLogFile_%d.log",thisDMB->slot());
    //
    std::ofstream DMBTestsLogFile;
    DMBTestsLogFile.open(buf);
    DMBTestsLogFile << OutputDMBTests[dmb][current_crate_].str() ;
    DMBTestsLogFile.close();
    //
    OutputDMBTests[dmb][current_crate_].str("");
    //
    this->DMBTests(in,out);
    //
  }
  //
  void emu::pc::Configurator::LogTMBTestsOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "LogTMBTestsOutput" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    std::string suffix="";
    cgicc::form_iterator sfn=cgi.getElement("LogNameSuffix");
    if(sfn != cgi.getElements().end()) 
    {
       suffix=cgi["LogNameSuffix"]->getValue();
    }

    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      std::cout << "TMB " << tmb << std::endl;
      TMB_ = tmb;
    } else {
      std::cout << "Not tmb" << std::endl ;
      tmb = TMB_;
    }

    //
    cgicc::form_iterator name2 = cgi.getElement("ClearTMBTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      std::cout << "Clear..." << std::endl;
      std::cout << cgi["ClearTMBTestsOutput"]->getValue() << std::endl ;
      OutputTMBTests[tmb][current_crate_].str("");
      OutputTMBTests[tmb][current_crate_] << "TMB-RAT Tests " 
					  << thisCrate->GetChamber(tmbVector[tmb]->slot())->GetLabel().c_str() 
					  << " output:" << std::endl;
      //
      this->TMBTests(in,out);
      return ;
      //
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    std::string lfn="/tmp/TMBTestsLogFile_"+suffix+".log";
    //
    std::ofstream TMBTestsLogFile;
    TMBTestsLogFile.open(lfn.c_str());
    TMBTestsLogFile << OutputTMBTests[tmb][current_crate_].str() ;
    TMBTestsLogFile.close();
    //
    OutputTMBTests[tmb][current_crate_].str("");
    OutputTMBTests[tmb][current_crate_] << "TMB-RAT Tests " 
				      << thisCrate->GetChamber(tmbVector[tmb]->slot())->GetLabel().c_str() 
				      << " output:" << std::endl;
    //
    this->TMBTests(in,out);
    //
  }
  //
  void emu::pc::Configurator::LogChamberTestsOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "LogChamberTestsOutput" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      std::cout << "TMB " << tmb << std::endl;
      TMB_ = tmb;
    } else {
      std::cout << "Not tmb" << std::endl ;
      tmb = TMB_;
    }
    //
    cgicc::form_iterator name2 = cgi.getElement("ClearChamberTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      std::cout << "Clear..." << std::endl;
      std::cout << cgi["ClearChamberTestsOutput"]->getValue() << std::endl ;
      ChamberTestsOutput[tmb][current_crate_].str("");
      ChamberTestsOutput[tmb][current_crate_] << "Chamber-Crate Phases " 
					      << thisCrate->GetChamber(tmbVector[tmb]->slot())->GetLabel().c_str() 
					      << " output:" << std::endl;
      //
      this->ChamberTests(in,out);
      return ;
      //
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    char buf[20];
    sprintf(buf,"/tmp/ChamberTestsLogFile_%d.log",thisTMB->slot());
    //
    std::ofstream ChamberTestsLogFile;
    ChamberTestsLogFile.open(buf);
    ChamberTestsLogFile << ChamberTestsOutput[tmb][current_crate_].str() ;
    ChamberTestsLogFile.close();
    //
    ChamberTestsOutput[tmb][current_crate_].str("");
    //
    this->ChamberTests(in,out);
    //
  }
  //
  void emu::pc::Configurator::LogALCT_TMB_communicationOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "LogALCT_TMB_communicationOutput" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name = cgi.getElement("tmb");
    //
    int tmb;
    if(name != cgi.getElements().end()) {
      tmb = cgi["tmb"]->getIntegerValue();
      std::cout << "TMB " << tmb << std::endl;
      TMB_ = tmb;
    } else {
      std::cout << "Not tmb" << std::endl ;
      tmb = TMB_;
    }
    //
    cgicc::form_iterator name2 = cgi.getElement("ClearALCT_TMB_communicationOutput");
    //
    if(name2 != cgi.getElements().end()) {
      std::cout << "Clear..." << std::endl;
      std::cout << cgi["ClearALCT_TMB_communicationOutput"]->getValue() << std::endl ;
      ALCT_TMB_communicationOutput[tmb][current_crate_].str("");
      ALCT_TMB_communicationOutput[tmb][current_crate_] << "Chamber-Crate Phases " 
					      << thisCrate->GetChamber(tmbVector[tmb]->slot())->GetLabel().c_str() 
					      << " output:" << std::endl;
      //
      this->ALCT_TMB_communication(in,out);
      return ;
      //
    }
    //
    TMB * thisTMB = tmbVector[tmb];
    //
    char buf[20];
    sprintf(buf,"/tmp/ALCT_TMB_communicationLogFile_%d.log",thisTMB->slot());
    //
    std::ofstream ALCT_TMB_communicationLogFile;
    ALCT_TMB_communicationLogFile.open(buf);
    ALCT_TMB_communicationLogFile << ALCT_TMB_communicationOutput[tmb][current_crate_].str() ;
    ALCT_TMB_communicationLogFile.close();
    //
    ALCT_TMB_communicationOutput[tmb][current_crate_].str("");
    //
    this->ALCT_TMB_communication(in,out);
    //
  }
  //
  void emu::pc::Configurator::LogCrateTestsOutput(xgi::Input * in, xgi::Output * out ) 
    throw (xgi::exception::Exception)
  {
    //
    std::cout << "LogCrateTestsOutput" << std::endl;
    //
    cgicc::Cgicc cgi(in);
    //
    cgicc::form_iterator name2 = cgi.getElement("ClearCrateTestsOutput");
    //
    if(name2 != cgi.getElements().end()) {
      std::cout << "Clear..." << std::endl;
      std::cout << cgi["ClearCrateTestsOutput"]->getValue() << std::endl ;
      CrateTestsOutput.str("");
      //
      this->CrateTests(in,out);
      return ;
      //
    }
    //
    // char buf[20];
    // sprintf(buf,"/tmp/CrateTestsLogFile.log");
    string logFileName( "/tmp/CrateTestsLogFile.log" );
    //
    std::ofstream CrateTestsLogFile;
    CrateTestsLogFile.open( logFileName.c_str() );
    CrateTestsLogFile << CrateTestsOutput.str() ;
    CrateTestsLogFile.close();
    //
    CrateTestsOutput.str("");
    //
    this->CrateTests(in,out);
    //
  }
