#include "AFEB/teststand/Measurement.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/LE32.h"
#include "AFEB/teststand/LeCroy3377.h"
#include "AFEB/teststand/Jorway73A.h"
#include "xcept/Exception.h"
#include "TRandom3.h"

#include "ieee_fun_types.h" // for cdchn

using namespace std;
using namespace AFEB::teststand;

const char* const AFEB::teststand::Measurement::types_[] = { "count_vs_dac", "time_vs_dac", "dummy" };
const char* const AFEB::teststand::Measurement::status_[] = { "waiting", "running", "done" };

ostream& AFEB::teststand::operator<<( ostream& os, const Measurement& m ){

  os << "Measurement " << m.index_ << ": '" << m.name_ << "' of type " << m.type_ << " (" << m.type_t_ << ") at position " << m.position_ << endl
     << " amplitudeMin="        << m.amplitudeMin_        
     << " amplitudeMax="        << m.amplitudeMax_        
     << " amplitudeStep="       << m.amplitudeStep_       << endl
     << " nPulses="             << m.nPulses_             
     << " pulseGeneratorSlot="  << m.pulseGeneratorSlot_
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

AFEB::teststand::Measurement::Measurement( const int position, const int index, const string name, const string type, const string resultDir ) :
  position_( position ),
  index_( index ),
  name_( name ),
  type_( type ),
  status_t_( AFEB::teststand::Measurement::waiting ),
  resultDir_( resultDir ),
  isToKeepRunning_( true )
{
  bool isValidType = false;
  for ( int i=0; i<nTypes && !isValidType; ++i ){
    isValidType |= ( type.compare( types_[i] ) == 0 );
    if ( isValidType ) type_t_ = (Type_t) i;
  }
  if ( !isValidType ){
    XCEPT_RAISE( xcept::Exception, type + " is not a valid meaurement type." );
  }
}

AFEB::teststand::Measurement::~Measurement(){
  // Delete results. (The tested devices were created by Configuration, and will also be deleted by it.)
  for ( map<TestedDevice*,Results*>::iterator r = results_.begin(); r != results_.end(); ++r ) delete r->second;
}

void AFEB::teststand::Measurement::setPulseParameters( const vector< pair<string,string> >& param ){
  string pulseGeneratorSlot;
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if      ( p->first.compare( "c:amplitudeMin"  ) == 0 ) amplitudeMin_       = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:amplitudeMax"  ) == 0 ) amplitudeMax_       = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:amplitudeStep" ) == 0 ) amplitudeStep_      = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:nPulses"       ) == 0 ) nPulses_            = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:capacitor"     ) == 0 ) pulsedCapacitor_    = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:slot"          ) == 0 ){
      if ( p->second.compare( "SignalConverter" ) == 0 ){
	injection_ = individual;
	pulseGeneratorSlot_ = -1;
      }
      else {
	injection_ = common;
	pulseGeneratorSlot_ = utils::stringTo<int>( p->second );
      }
    }
  }

  //cout << param << endl << *this << endl;
}

void AFEB::teststand::Measurement::setThresholdParameters( const vector< pair<string,string> >& param ){
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if ( p->first.compare( "c:thresholdValue" ) == 0 ) thresholdValue_ = utils::stringTo<int>( p->second );
  }
}

void AFEB::teststand::Measurement::setTDCParameters( const vector< pair<string,string> >& param ){
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if      ( p->first.compare( "c:timeMin" ) == 0 ) tdcTimeMin_ = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:timeMax" ) == 0 ) tdcTimeMax_ = utils::stringTo<int>( p->second );
  }
}

void AFEB::teststand::Measurement::addTestedDevice( TestedDevice* device ){
  // Check if this input has already been taken:
  map<TestedDevice*,Results*>::const_iterator r;
  for ( r = results_.begin(); r != results_.end(); ++r ){
    if ( r->first->getTDCSlot()  == device->getTDCSlot()  && 
	 r->first->getTDCInput() == device->getTDCInput()    ){
      stringstream ss;
      ss << "A tested device (" << r->first->getId() 
	 << ") is already configured to be read out by input " << device->getTDCInput() 
	 << " of the TDC in slot " << device->getTDCSlot();
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
  }
  // Create results for it, too:
  Results *results = new Results( this, device );
  results_.insert( make_pair( device, results ) );
}

const TestedDevice* AFEB::teststand::Measurement::getTestedDevice( const int tdcInput ) const {
  map<TestedDevice*,Results*>::const_iterator r;
  for ( r = results_.begin(); r != results_.end(); ++r ){
    if ( r->first->getTDCInput() == tdcInput ) return r->first;
  }
  return NULL;
}

TestedDevice* AFEB::teststand::Measurement::findTestedDevice( const int tdcInput ) {
  map<TestedDevice*,Results*>::iterator r;
  for ( r = results_.begin(); r != results_.end(); ++r ){
    if ( r->first->getTDCInput() == tdcInput ) return r->first;
  }
  return NULL;
}

Results* AFEB::teststand::Measurement::findResults( const int tdcInput ) {
  map<TestedDevice*,Results*>::iterator r;
  for ( r = results_.begin(); r != results_.end(); ++r ){
    if ( r->first->getTDCInput() == tdcInput ) return r->second;
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
  if ( ! isToKeepRunning_ ){
    status_t_ = AFEB::teststand::Measurement::waiting;
    return false;
  }
  bool keepRunning = true;
  status_t_ = AFEB::teststand::Measurement::running;
  switch ( type_t_ ){
  case count_vs_dac:
  case time_vs_dac:
    keepRunning = countVsDAQ();
    break;
  case dummy:
    keepRunning = dummyResultGenerator();
    break;
  default:
    stringstream ss;
    ss << "Unknown measurement type " << type_t_ << " specified as '" << type_ << "'";
    XCEPT_RAISE( xcept::Exception, ss.str() );
    break;
  }

  // Save everything if it hasn't been aborted:
  if ( keepRunning ) for ( map<TestedDevice*,Results*>::iterator r = results_.begin(); r != results_.end(); ++r ) r->second->save( resultDir_ );

  status_t_ = AFEB::teststand::Measurement::done;
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
  if      ( injection_ == common     ) pulseGenerator = static_cast<LE32*>( crate->getModule( pulseGeneratorSlot_ ) );
  else if ( injection_ == individual ) pulseGenerator = signalConverter;
  const CrateController* controller = crate->getCrateController();

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
	int tdcInput = tdc->ChannelRd() / nDeviceChannels + 1;
	Results* results = findResults( tdcInput );
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
	int tdcInput = iShort / nDeviceChannels + 1;
	Results* results = findResults( tdcInput );
	if ( results ){
	  if ( double(amplitude) > rndm.Gaus( 0.3 * ( amplitudeMax_ + amplitudeMin_ ) + thresholdOffset[ iShort ],
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
