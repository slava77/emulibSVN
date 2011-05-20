#include "AFEB/teststand/Measurement.h"
#include "AFEB/teststand/utils/String.h"
#include "xcept/Exception.h"

using namespace std;
using namespace AFEB::teststand;

const char* const AFEB::teststand::Measurement::types_[] = { "count_vs_dac", "time_vs_dac" };

AFEB::teststand::Measurement::Measurement( const string name, const string type ) :
  name_( name ){
  bool isValidType = false;
  for ( int i=0; i<nTypes && !isValidType; ++i ){
    isValidType |= ( type.compare( types_[i] ) == 0 );
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

const TestedDevice* AFEB::teststand::Measurement::findTestedDevice( const int tdcSlot ) const {
  vector<TestedDevice*>::const_iterator d;
  for ( d = testedDevices_.begin(); d != testedDevices_.end(); ++d ){
    if ( (*d)->getTDCSlot() == tdcSlot ) return *d;
  }
  return NULL;
}
