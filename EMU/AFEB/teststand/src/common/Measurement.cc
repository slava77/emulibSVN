#include "AFEB/teststand/Measurement.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/crate/LE32.h"
#include "AFEB/teststand/crate/LeCroy3377.h"
#include "AFEB/teststand/crate/Jorway73A.h"
#include "xcept/Exception.h"
#include "TRandom3.h"

using namespace std;
using namespace AFEB::teststand;

const char* const AFEB::teststand::Measurement::typeString_[] = { "count_vs_dac", "time_vs_dac" };
const char* const AFEB::teststand::Measurement::statusString_[] = { "waiting", "running", "done" };
const char* const AFEB::teststand::Measurement::capacitorString_[] = { "external", "internal" };

ostream& AFEB::teststand::operator<<( ostream& os, const Measurement& m ){

  os << "Measurement " << m.index_ << ": '" << m.name_ << "' of type " << m.getTypeString() << " (" << m.type_ << ") at position " << m.position_ << endl
     << " injectionCapacitor="  << m.capacitorString_[m.injectionCapacitor_]
     << " amplitudeMin="        << m.amplitudeMin_        
     << " amplitudeMax="        << m.amplitudeMax_        
     << " amplitudeStep="       << m.amplitudeStep_       << endl
     << " nPulses="             << m.nPulses_             
     << " thresholdValue="      << m.thresholdValue_      << endl
     << " tdcTimeMin="          << m.tdcTimeMin_          
     << " tdcTimeMax="          << m.tdcTimeMax_          << endl 
     << "  Tested devices:"                               << endl;

  map<TestedDevice*,Results*>::const_iterator r;
  for ( r = m.results_.begin(); r != m.results_.end(); ++r ){
    os << *r->first;
  }

  return os;
}

AFEB::teststand::Measurement::Measurement( const int position, const int index, const string name, const string type, const string resultDir, bool generateDummyData ) :
  bsem_( toolbox::BSem::EMPTY ), // locked
  position_( position ),
  index_( index ),
  name_( name ),
  type_( getType( type ) ),
  status_( AFEB::teststand::Measurement::waiting ),
  resultDir_( resultDir ),
  generateDummyData_( generateDummyData ),
  isToKeepRunning_( true )
{
  bsem_.give();
}

AFEB::teststand::Measurement::~Measurement(){
  // Delete results. (The tested devices were created by Configuration, and will also be deleted by it.)
  for ( map<TestedDevice*,Results*>::iterator r = results_.begin(); r != results_.end(); ++r ) delete r->second;
}

void AFEB::teststand::Measurement::setPulseParameters( const vector< pair<string,string> >& param ){
  bsem_.take();
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if      ( p->first.compare( "amplitudeMin"  ) == 0 ) amplitudeMin_       = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "amplitudeMax"  ) == 0 ) amplitudeMax_       = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "amplitudeStep" ) == 0 ) amplitudeStep_      = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "nPulses"       ) == 0 ) nPulses_            = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "capacitor"     ) == 0 ) injectionCapacitor_ = getInjectionCapacitor( p->second );
  } // for ( p = param.begin(); p != param.end(); ++p )

  //cout << param << endl << *this << endl;
  bsem_.give();
}

void AFEB::teststand::Measurement::setThresholdParameters( const vector< pair<string,string> >& param ){
  bsem_.take();
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if ( p->first.compare( "thresholdValue" ) == 0 ) thresholdValue_ = utils::stringTo<int>( p->second );
  }
  bsem_.give();
}

void AFEB::teststand::Measurement::setTDCParameters( const vector< pair<string,string> >& param ){
  bsem_.take();
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if      ( p->first.compare( "timeMin" ) == 0 ) tdcTimeMin_ = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "timeMax" ) == 0 ) tdcTimeMax_ = utils::stringTo<int>( p->second );
  }
  bsem_.give();
}

void AFEB::teststand::Measurement::addTestedDevice( TestedDevice* device ){
  bsem_.take();
  // Check if this input has already been taken:
  map<TestedDevice*,Results*>::const_iterator r;
  for ( r = results_.begin(); r != results_.end(); ++r ){
    if ( r->first->getTDCSlot()  == device->getTDCSlot()  && 
	 r->first->getTDCSocket() == device->getTDCSocket()    ){
      stringstream ss;
      ss << "A tested device (" << r->first->getId() 
	 << ") is already configured to be read out by input " << device->getTDCSocket() 
	 << " of the TDC in slot " << device->getTDCSlot();
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
  }
  // Create results for it, too:
  Results *results = new Results( this, device );
  results_.insert( make_pair( device, results ) );
  bsem_.give();
}

const TestedDevice* AFEB::teststand::Measurement::getTestedDevice( const int tdcSocket ) const {
  map<TestedDevice*,Results*>::const_iterator r;
  for ( r = results_.begin(); r != results_.end(); ++r ){
    if ( r->first->getTDCSocket() == tdcSocket ) return r->first;
  }
  return NULL;
}

TestedDevice* AFEB::teststand::Measurement::findTestedDevice( const int tdcSocket ) {
  map<TestedDevice*,Results*>::iterator r;
  for ( r = results_.begin(); r != results_.end(); ++r ){
    if ( r->first->getTDCSocket() == tdcSocket ) return r->first;
  }
  return NULL;
}

Results* AFEB::teststand::Measurement::findResults( const int tdcSocket ) {
  map<TestedDevice*,Results*>::iterator r;
  for ( r = results_.begin(); r != results_.end(); ++r ){
    if ( r->first->getTDCSocket() == tdcSocket ) return r->second;
  }
  return NULL;
}

int AFEB::teststand::Measurement::getTDCSlot() const {
  if ( results_.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, "Failed to get TDC slot number for this measurement as it has no tested devices." );
  }
  // By definition, all tested devices of a measurement are read out by the same TDC. Take the first one:
  return results_.begin()->first->getTDCSlot();
}

bool AFEB::teststand::Measurement::execute(){
  bsem_.take();
  if ( ! isToKeepRunning_ ){
    status_ = AFEB::teststand::Measurement::waiting;
    bsem_.give();
    return false;
  }
  bool keepRunning = true;
  status_ = AFEB::teststand::Measurement::running;
  bsem_.give();

  switch ( type_ ){
  case count_vs_dac:
  case time_vs_dac:
    keepRunning = ( generateDummyData_ ? dummyResultGenerator() : countVsDAQ() );
    break;
  default:
    stringstream ss;
    ss << "Unknown measurement type " << type_;
    XCEPT_RAISE( xcept::Exception, ss.str() );
    break;
  }

  bsem_.take();
  // Save everything if it hasn't been aborted:
  if ( keepRunning ) for ( map<TestedDevice*,Results*>::iterator r = results_.begin(); r != results_.end(); ++r ) r->second->save( resultDir_ );

  status_ = AFEB::teststand::Measurement::done;
  bsem_.give();

  return keepRunning;
}

bool AFEB::teststand::Measurement::countVsDAQ(){
  // All tested devices in one measurement are read out by the same TDC. Take the first device to get its.
  int nDeviceChannels = results_.begin()->first->getNChannels();
  Crate* crate = results_.begin()->first->getCrate();
  LeCroy3377*       tdc = static_cast<LeCroy3377*>( crate->getModule( results_.begin()->first->getTDCSlot() ) );
  // All tested devices's signals in one measurement are fed through the same signal converter module. Take the first device to get its.
  LE32* signalConverter = static_cast<LE32*>( crate->getModule( results_.begin()->first->getSignalConverterSlot() ) );
  // Use either the specified pulse generator for common injection (through external capacitor), 
  // or the corresponding signal converter module to inject individually (through external or internal capacitor). 
  LE32*  pulseGenerator = NULL;
  if      ( injectionCapacitor_ == external ) pulseGenerator = static_cast<LE32*>( crate->getModule( results_.begin()->first->getPulseGeneratorSlot() ) );
  else if ( injectionCapacitor_ == internal ) pulseGenerator = signalConverter;
  CrateController* controller = crate->getCrateController();

  controller->initialize();

  // TODO: remove debug
  // cout << "***AFEB::teststand::Measurement::countVsDAQ***" << endl << *crate << endl;  
  // while ( true ){
  //   controller->read( CAMAC::A0, CAMAC::F1, CAMAC::N7 );
  // }
  //

  // Zero CAMAC:
  //controller->z(); // TODO: needed? Takes very long, see why.

  // BEGIN test pulses
  // pulseGenerator->writeAmplitude( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ), amplitudeMax_ );
  // pulseGenerator->enablePulses(  (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ) );
  // pulseGenerator->writeAmplitude( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ), amplitudeMax_ );
  // cout << "Wrote amplitude " << amplitudeMax_ 
  //      << " to channels " <<  (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 )
  //      << endl;
  // for( int i=0; i<100000; i++ ){
  //   if ( i%1000 == 0 ) std::cout << "pulse " << std::dec << i << std::endl;
  //   pulseGenerator->exec();
  // }
  // END test pulses

  // Set up TDC:
  tdc->Set( LeCroy3377::M1, 0, 2, 0, 0, 550, 511 ); // ( Mode, Shift, Hit, Edge, Mpi, TimeOut, TimeEnf )

  // Set up threshold-setting module:
  signalConverter->writeThreshold( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ), thresholdValue_ );
  signalConverter->writeAmplitude( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ), 0 );
  signalConverter->enablePulses( LE32::NoCh );

  // Set pulse generator
  pulseGenerator->writeAmplitude( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ), amplitudeMin_ );

  // Gradually crank up the pulse height:
  for ( int amplitude = amplitudeMin_; amplitude <= amplitudeMax_; amplitude += amplitudeStep_ ){

    pulseGenerator->writeAmplitude( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ), amplitude );
    pulseGenerator->enablePulses(  (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ) );
    pulseGenerator->exec();
    tdc->Clear();
    tdc->EnableAcq();

    // Send pulses
    for ( int iPulse = 0; iPulse < nPulses_; ++iPulse ){

      tdc->Clear();
      pulseGenerator->exec();
      
      tdc->HeadRdBlock(); // This does the actual hardware readout.

      // Check whether we've been instructed to abort in the meantime:
      if ( ! isToKeepRunning_ ) return false;

      // Read out TDC channels
      for ( int iShort = 0; iShort < LeCroy3377::nShortsData; ++iShort ){ // TODO: LeCroy3377::nShortsData / 2 ?
	// LeCroy3377::BlockRd sets LeCroy3377::TimeCh, which can be accessed by LeCroy3377::TimeChRd(),
	//                     and LeCroy3377::Channel, which can be accessed by LeCroy3377::ChannelRd()
	if ( !tdc->BlockRd( iShort ) ) break;
	// if ( tdc->TimeChRd() >= tdcTimeMin_ && 
	//      tdc->TimeChRd() <= tdcTimeMax_    ){
	int tdcSocket = tdc->ChannelRd() / nDeviceChannels + 1;
	Results* results = findResults( tdcSocket );
	if ( results ) results->add( tdc->ChannelRd() % nDeviceChannels, amplitude, tdc->TimeChRd() );
	// Check whether we've been instructed to abort in the meantime:
	if ( ! isToKeepRunning_ ) return false;
	// }
      } // for ( int iShort = 0; iShort < LeCroy3377::nShortsData; ++iShort )

    } // for ( int iPulse = 0; iPulse < nPulses_; ++iPulse )

    // Update stored results
    for ( map<TestedDevice*,Results*>::iterator r = results_.begin(); r != results_.end(); ++r ){
      r->second->createFigure( resultDir_, amplitudeMin_, amplitude );
    }

  } // for ( int amplitude = amplitudeMin_; amplitude <= amplitudeMax_; amplitude += amplitudeStep_ )

  return true;
}


bool AFEB::teststand::Measurement::dummyResultGenerator(){
  // All tested devices in one measurement are read out by the same TDC. Take the first device to get its.
  int nDeviceChannels = results_.begin()->first->getNChannels();

  TRandom3 rndm;

  // Spread the threshold so that not all channels have it at the same value:
  double thresholdOffset[LeCroy3377::nShortsData / 2];
  for ( int iShort = 0; iShort < LeCroy3377::nShortsData / 2; ++iShort ){
    thresholdOffset[ iShort ] = rndm.Gaus( 0., 0.08 * ( amplitudeMax_ - amplitudeMin_ ) );
  }

  // Gradually crank up the pulse height:
  for ( int amplitude = amplitudeMin_; amplitude <= amplitudeMax_; amplitude += amplitudeStep_ ){

    cout << "amplitude=" << amplitude << endl;

    ::sleep(1); // TODO: remove

    // Send pulses
    for ( int iPulse = 0; iPulse < nPulses_; ++iPulse ){

      //cout << "   pulse=" << iPulse << endl;

      // Read out TDC channels
      for ( int iShort = 0; iShort < LeCroy3377::nShortsData / 2; ++iShort ){
	//cout << "         data word=" << iShort << endl;
	int tdcSocket = iShort / nDeviceChannels + 1;
	Results* results = findResults( tdcSocket );
	if ( results ){
	  if ( double(amplitude) > rndm.Gaus( thresholdValue_ + thresholdOffset[ iShort ],
					      0.1 * ( amplitudeMax_ - amplitudeMin_ ) ) ){
	    results->add( iShort % nDeviceChannels, 
			  amplitude,
			  rndm.Gaus( 0.5 * ( tdcTimeMax_ + tdcTimeMin_ ), 
				     0.1 * ( tdcTimeMax_ - tdcTimeMin_ ) ) );
	    // Check whether we've been instructed to abort in the meantime:
	    if ( ! isToKeepRunning_ ) return false;
	  }
	}
      } // for ( int iShort = 0; iShort < LeCroy3377::nShortsData; ++iShort )

    } // for ( int iPulse = 0; iPulse < nPulses_; ++iPulse )

    // Update stored results
    for ( map<TestedDevice*,Results*>::iterator r = results_.begin(); r != results_.end(); ++r ){
      r->second->createFigure( resultDir_, amplitudeMin_, amplitude );
    }

  } // for ( int amplitude = amplitudeMin_; amplitude <= amplitudeMax_; amplitude += amplitudeStep_ )

  return true;
}

// static function
AFEB::teststand::Measurement::Type_t AFEB::teststand::Measurement::getType( const string& typeString ){
  AFEB::teststand::Measurement::Type_t type = AFEB::teststand::Measurement::nTypes;
  bool isValidType = false;
  for ( int i=0; i<AFEB::teststand::Measurement::nTypes && !isValidType; ++i ){
    isValidType |= ( typeString.compare( typeString_[i] ) == 0 );
    if ( isValidType ) type = (AFEB::teststand::Measurement::Type_t) i;
  }
  if ( !isValidType ){
    stringstream ss;
    ss << "\"" << typeString << "\" is not a valid measurement type.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return type;
}

// static function
string AFEB::teststand::Measurement::getTypeString( const Type_t type ){
  if ( type < (AFEB::teststand::Measurement::Type_t)0 || 
       type >= AFEB::teststand::Measurement::nTypes      ){
    stringstream ss;
    ss << "'" << type << "' is not a valid measurement type.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return typeString_[type];
}

// static function
AFEB::teststand::Measurement::Capacitor_t AFEB::teststand::Measurement::getInjectionCapacitor( const string& injectionCapacitorString ){
  AFEB::teststand::Measurement::Capacitor_t capacitor = AFEB::teststand::Measurement::nCapacitors;
  bool isValidType = false;
  for ( int i=0; i<AFEB::teststand::Measurement::nCapacitors && !isValidType; ++i ){
    isValidType |= ( injectionCapacitorString.compare( capacitorString_[i] ) == 0 );
    if ( isValidType ) capacitor = (AFEB::teststand::Measurement::Capacitor_t) i;
  }
  if ( !isValidType ){
    stringstream ss;
    ss << "\"" << injectionCapacitorString << "\" is not a valid injection capacitor type.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return capacitor;
}

// static function
string AFEB::teststand::Measurement::getInjectionCapacitorString( const Capacitor_t injectionCapacitor ){
  if ( injectionCapacitor < (AFEB::teststand::Measurement::Capacitor_t)0 || 
       injectionCapacitor >= AFEB::teststand::Measurement::nCapacitors      ){
    stringstream ss;
    ss << "'" << injectionCapacitor << "' is not a valid injection capacitor type.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return capacitorString_[injectionCapacitor];
}
