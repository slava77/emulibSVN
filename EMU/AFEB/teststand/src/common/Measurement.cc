#include "AFEB/teststand/Measurement.h"
#include "AFEB/teststand/utils/String.h"
#include "xcept/Exception.h"

using namespace std;
using namespace AFEB::teststand;

const char* const AFEB::teststand::Measurement::types_[] = { "count_vs_dac", "time_vs_dac" };

AFEB::teststand::Measurement::Measurement( const string name, const string type, const AFEB::teststand::Crate* const crate ) :
  name_( name ),
  crate_( crate ){
  bool isValidType = false;
  for ( int i=0; i<nTypes && !isValidType; ++i ){
    isValidType |= ( type_ == types_[i] );
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
