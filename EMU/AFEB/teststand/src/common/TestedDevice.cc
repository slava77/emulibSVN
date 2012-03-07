#include "AFEB/teststand/TestedDevice.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "xcept/Exception.h"

#include <iomanip>

ostream& AFEB::teststand::operator<<( ostream& os, const TestedDevice& d ){
  os << "socket    type                id                      TDC slot   TDC input      converter slot" << endl
     << right
     << setw(6) << d.socket_ << "    "
     << left
     << setw(20) << d.type_ 
     << setw(20) << d.id_
     << right 
     << setw(12) << d.tdcSlot_ 
     << setw(12) << d.tdcSocket_ 
     << setw(20) << d.signalConverterSlot_ << endl;
  return os;
}

AFEB::teststand::TestedDevice::TestedDevice( const string type, const int nChannels, const double nominalCint, Crate* const crate ) :
  type_( type ),
  nChannels_( nChannels ),
  nominalInternalCapacitance_( nominalCint ),
  crate_( crate ){}

void AFEB::teststand::TestedDevice::setParameters( const vector< pair<string,string> >& param ){
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if      ( p->first.compare( "id"       ) == 0 ) id_       = p->second;
    else if ( p->first.compare( "socket"   ) == 0 ) socket_   = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "tdcSlot"  ) == 0 ) tdcSlot_  = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "tdcSocket" ) == 0 ) tdcSocket_ = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "signalConverterSlot"  ) == 0 ) signalConverterSlot_  = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "signalConverterSocket" ) == 0 ) signalConverterSocket_ = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "pulseGeneratorSlot"    ) == 0 ) pulseGeneratorSlot_    = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "pulseGeneratorSocket"  ) == 0 ) pulseGeneratorSocket_  = utils::stringTo<int>( p->second );
  }
  //cout << param << endl << *this << endl;
}
