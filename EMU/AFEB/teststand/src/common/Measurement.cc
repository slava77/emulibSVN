#include "AFEB/teststand/Measurement.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/LE32.h"
#include "AFEB/teststand/LeCroy3377.h"
#include "xcept/Exception.h"

using namespace std;
using namespace AFEB::teststand;

const char* const AFEB::teststand::Measurement::types_[] = { "count_vs_dac", "time_vs_dac" };

ostream& AFEB::teststand::operator<<( ostream& os, const Measurement& m ){

  os << "Measurement '" << m.name_ << "' of type " << m.type_ << endl
     << "  Tested devices:" << endl;

  vector<TestedDevice*>::const_iterator d;
  for ( d = m.testedDevices_.begin(); d != m.testedDevices_.end(); ++d ){
    os << **d;
  }

  return os;
}

AFEB::teststand::Measurement::Measurement( const string name, const string type ) :
  name_( name ),
  type_( type )
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

void AFEB::teststand::Measurement::setPulseParameters( const vector< pair<string,string> >& param ){
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if      ( p->first.rfind( "amplitudeMin"  ) != string::npos ) amplitudeMin_       = utils::stringTo<int>( p->second );
    else if ( p->first.rfind( "amplitudeMax"  ) != string::npos ) amplitudeMax_       = utils::stringTo<int>( p->second );
    else if ( p->first.rfind( "amplitudeStep" ) != string::npos ) amplitudeStep_      = utils::stringTo<int>( p->second );
    else if ( p->first.rfind( "nPulses"       ) != string::npos ) nPulses_            = utils::stringTo<int>( p->second );
    else if ( p->first.rfind( "slot"          ) != string::npos ) pulseGeneratorSlot_ = utils::stringTo<int>( p->second );
  }
}

void AFEB::teststand::Measurement::setThresholdParameters( const vector< pair<string,string> >& param ){
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if ( p->first.rfind( "value" ) != string::npos ) thresholdValue_ = utils::stringTo<int>( p->second );
  }
}

void AFEB::teststand::Measurement::setTDCParameters( const vector< pair<string,string> >& param ){
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if      ( p->first.rfind( "tdcTimeMin" ) != string::npos ) tdcTimeMin_ = utils::stringTo<int>( p->second );
    else if ( p->first.rfind( "tdcTimeMax" ) != string::npos ) tdcTimeMax_ = utils::stringTo<int>( p->second );
  }
}

void AFEB::teststand::Measurement::addTestedDevice( TestedDevice* device ){
  // Check if this input has already been taken:
  vector<TestedDevice*>::const_iterator d;
  for ( d = testedDevices_.begin(); d != testedDevices_.end(); ++d ){
    if ( (*d)->getTDCSlot()  == device->getTDCSlot()  && 
	 (*d)->getTDCInput() == device->getTDCInput()    ){
      stringstream ss;
      ss << "A tested device (" << (*d)->getId() 
	 << ") is already configured to be read out by input " << device->getTDCInput() 
	 << " of the TDC in slot " << device->getTDCSlot();
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
  }
  testedDevices_.push_back( device );
}

const TestedDevice* AFEB::teststand::Measurement::findTestedDevice( const int tdcInput ) const {
  vector<TestedDevice*>::const_iterator d;
  for ( d = testedDevices_.begin(); d != testedDevices_.end(); ++d ){
    if ( (*d)->getTDCInput() == tdcInput ) return *d;
  }
  return NULL;
}

int AFEB::teststand::Measurement::getTDCSlot() const {
  if ( testedDevices_.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, "Failed to get TDC slot number for this measurement as it has no tested devices." );
  }
  // By definition, all tested devices of a measurement are read out by the same TDC. Take the first one:
  return testedDevices_.at(0)->getTDCSlot();
}

void AFEB::teststand::Measurement::execute(){
  switch ( type_t_ ){
  case count_vs_dac:
    countVsDAQ();
    break;
  case time_vs_dac:
    countVsTime();
    break;
  default:
    stringstream ss;
    ss << "Unknown measurement type " << type_t_ << " specified as '" << type_ << "'";
    XCEPT_RAISE( xcept::Exception, ss.str() );
    break;
  }
}

void AFEB::teststand::Measurement::countVsDAQ(){
  // All tested devices in one measurement are read out by the same TDC. Take the first device to get its.
  int tdcSlot = testedDevices_.at(0)->getTDCSlot();
  Crate* crate = testedDevices_.at(0)->getCrate();
  LeCroy3377*       tdc = static_cast<LeCroy3377*>( crate->getModule( testedDevices_.at(0)->getTDCSlot() ) );
  LE32*  pulseGenerator = static_cast<LE32*>( crate->getModule( pulseGeneratorSlot_                         ) );
  LE32* signalConverter = static_cast<LE32*>( crate->getModule( testedDevices_.at(0)->getSignalConverterSlot() ) );
  const CrateController* controller = crate->getCrateController();

  // Zero CAMAC:
  controller->z();

  // Set up TDC:
  tdc->Set( LeCroy3377::M1, 0, 2, 0, 0, 550, 511 ); // ( Mode, Shift, Hit, Edge, Mpi, TimeOut, TimeEnf )

  // Set up threshold-setting module:
  signalConverter->writeThreshold( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch1 ), thresholdValue_ );
  signalConverter->writeAmplitude( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch1 ), 0 );
  signalConverter->enablePulses( LE32::NoCh );

  // Set pulse generator
  pulseGenerator->writeAmplitude( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch1 ), amplitudeMin_ );

  // Gradually crank up the pulse height:
  for ( int amplitude = amplitudeMin_; amplitude <= amplitudeMax_; amplitude += amplitudeStep_ ){

    // TODO: zero counters

    pulseGenerator->writeAmplitude( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch1 ), amplitude );
    pulseGenerator->enablePulses(  (LE32::Channel_t)( LE32::Ch1 | LE32::Ch1 ) );
    pulseGenerator->exec();
    tdc->Clear();
    tdc->EnableAcq();

    // Send pulses
    for ( int iPulse = 0; iPulse < nPulses_; ++iPulse ){

      tdc->Clear();
      pulseGenerator->exec();
      
      tdc->HeadRdBlock();

      // Read out TDC channels
      for ( int iChannel = 0; iChannel < LeCroy3377::nTdcChannels; ++iChannel ){
	if ( !tdc->BlockRd( iChannel ) ) break;
	if ( tdc->TimeChRd() >= tdcTimeMin_ && 
	     tdc->TimeChRd() <= tdcTimeMax_    ){
	}
      } // for ( int iChannel = 0; iChannel < LeCroy3377::nTdcChannels; ++iChannel )

    } // for ( int iPulse = 0; iPulse < nPulses_; ++iPulse )

    // TODO: store results

  } // for ( int amplitude = amplitudeMin_; amplitude <= amplitudeMax_; amplitude += amplitudeStep_ )
}

void AFEB::teststand::Measurement::countVsTime(){
  // TODO: implement
}
