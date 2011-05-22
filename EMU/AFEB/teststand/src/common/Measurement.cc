#include "AFEB/teststand/Measurement.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/LE32.h"
#include "AFEB/teststand/LeCroy3377.h"
#include "xcept/Exception.h"

using namespace std;
using namespace AFEB::teststand;

const char* const AFEB::teststand::Measurement::types_[] = { "count_vs_dac", "time_vs_dac" };

ostream& AFEB::teststand::operator<<( ostream& os, const Measurement& m ){

  os << "Measurement '" << m.name_ << "' of type " << m.type_ << endl
     << "  Tested devices:" << endl;

  map<TestedDevice*,Results*>::const_iterator r;
  for ( r = m.results_.begin(); r != m.results_.end(); ++r ){
    os << *r->first;
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

AFEB::teststand::Measurement::~Measurement(){
  // Delete results. (The tested devices were created by Configuration, and will also be deleted by it.)
  for ( map<TestedDevice*,Results*>::iterator r = results_.begin(); r != results_.end(); ++r ) delete r->second;
}

void AFEB::teststand::Measurement::setPulseParameters( const vector< pair<string,string> >& param ){
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if      ( p->first.compare( "c:amplitudeMin"  ) == 0 ) amplitudeMin_       = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:amplitudeMax"  ) == 0 ) amplitudeMax_       = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:amplitudeStep" ) == 0 ) amplitudeStep_      = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:nPulses"       ) == 0 ) nPulses_            = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:slot"          ) == 0 ) pulseGeneratorSlot_ = utils::stringTo<int>( p->second );
  }
  //cout << param << endl << *this << endl;
}

void AFEB::teststand::Measurement::setThresholdParameters( const vector< pair<string,string> >& param ){
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if ( p->first.compare( "c:value" ) == 0 ) thresholdValue_ = utils::stringTo<int>( p->second );
  }
}

void AFEB::teststand::Measurement::setTDCParameters( const vector< pair<string,string> >& param ){
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if      ( p->first.compare( "c:tdcTimeMin" ) == 0 ) tdcTimeMin_ = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:tdcTimeMax" ) == 0 ) tdcTimeMax_ = utils::stringTo<int>( p->second );
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

void AFEB::teststand::Measurement::execute(){
  switch ( type_t_ ){
  case count_vs_dac:
    //countVsDAQ();
    dummyResultGenerator();
    break;
  case time_vs_dac:
    //countVsTime();
    dummyResultGenerator();
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
  int nDeviceChannels = results_.begin()->first->getNChannels();
  Crate* crate = results_.begin()->first->getCrate();
  LeCroy3377*       tdc = static_cast<LeCroy3377*>( crate->getModule( results_.begin()->first->getTDCSlot() ) );
  LE32*  pulseGenerator = static_cast<LE32*>( crate->getModule( pulseGeneratorSlot_                         ) );
  LE32* signalConverter = static_cast<LE32*>( crate->getModule( results_.begin()->first->getSignalConverterSlot() ) );
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
      
      tdc->HeadRdBlock(); // This doeas the actual hardware readout.

      // Read out TDC channels
      for ( int iShort = 0; iShort < LeCroy3377::nShortsData; ++iShort ){
	// LeCroy3377::BlockRd sets LeCroy3377::TimeCh, which can be accessed by LeCroy3377::TimeChRd(),
	//                     and LeCroy3377::Channel, which can be accessed by LeCroy3377::ChannelRd()
	if ( !tdc->BlockRd( iShort ) ) break;
	// if ( tdc->TimeChRd() >= tdcTimeMin_ && 
	//      tdc->TimeChRd() <= tdcTimeMax_    ){
	int tdcInput = ( tdc->ChannelRd() < nDeviceChannels ? 1 : 2 );
	Results* results = findResults( tdcInput );
	if ( results ) results->add( tdc->ChannelRd(), amplitude, tdc->TimeChRd() );
	// }
      } // for ( int iShort = 0; iShort < LeCroy3377::nShortsData; ++iShort )

    } // for ( int iPulse = 0; iPulse < nPulses_; ++iPulse )

    // TODO: store results

  } // for ( int amplitude = amplitudeMin_; amplitude <= amplitudeMax_; amplitude += amplitudeStep_ )
}

void AFEB::teststand::Measurement::countVsTime(){
  // TODO: implement
}

void AFEB::teststand::Measurement::dummyResultGenerator(){
  // All tested devices in one measurement are read out by the same TDC. Take the first device to get its.
  int nDeviceChannels = results_.begin()->first->getNChannels();
  Crate* crate = results_.begin()->first->getCrate();
  LeCroy3377*       tdc = static_cast<LeCroy3377*>( crate->getModule( results_.begin()->first->getTDCSlot() ) );
  LE32*  pulseGenerator = static_cast<LE32*>( crate->getModule( pulseGeneratorSlot_                         ) );
  LE32* signalConverter = static_cast<LE32*>( crate->getModule( results_.begin()->first->getSignalConverterSlot() ) );
  const CrateController* controller = crate->getCrateController();

  // Gradually crank up the pulse height:
  for ( int amplitude = amplitudeMin_; amplitude <= amplitudeMax_; amplitude += amplitudeStep_ ){

    cout << "amplitude=" << amplitude << endl;

    // Send pulses
    for ( int iPulse = 0; iPulse < nPulses_; ++iPulse ){

      //cout << "   pulse=" << iPulse << endl;

      // Read out TDC channels
      for ( int iShort = 0; iShort < LeCroy3377::nShortsData; ++iShort ){
	//cout << "         data word=" << iShort << endl;
	int tdcInput = ( iShort < nDeviceChannels ? 1 : 2 );
	Results* results = findResults( tdcInput );
	if ( results ) results->add( iShort, amplitude, 2*amplitude );
      } // for ( int iShort = 0; iShort < LeCroy3377::nShortsData; ++iShort )

    } // for ( int iPulse = 0; iPulse < nPulses_; ++iPulse )

  } // for ( int amplitude = amplitudeMin_; amplitude <= amplitudeMax_; amplitude += amplitudeStep_ )

  map<TestedDevice*,Results*>::iterator r;
  for ( r = results_.begin(); r != results_.end(); ++r ) r->second->save();

}
