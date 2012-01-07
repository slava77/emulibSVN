#include "AFEB/teststand/Device.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "xcept/Exception.h"

#include <iomanip>

ostream& AFEB::teststand::operator<<( ostream& os, const Device& d ){
  os << "socket    type                id                      TDC slot   TDC input      converter slot" << endl
     << right
     << setw(6) << d.socket_ << "    "
     << left
     << setw(20) << d.type_ 
     << setw(20) << d.id_
     << right 
     << setw(12) << d.tdcSlot_ 
     << setw(12) << d.tdcInput_ 
     << setw(20) << d.signalConverterSlot_ << endl;
  return os;
}

AFEB::teststand::Device::Device( const string type, const int nChannels ) :
  type_( type ),
  nChannels_( nChannels ){}


void AFEB::teststand::Device::setParameters( const vector< pair<string,string> >& param ){
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if      ( p->first.compare( "c:id"       ) == 0 ) id_       = p->second;
    else if ( p->first.compare( "c:socket"   ) == 0 ) socket_   = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:tdcSlot"  ) == 0 ) tdcSlot_  = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:tdcInput" ) == 0 ) tdcInput_ = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:signalConverterSlot"  ) == 0 ) signalConverterSlot_  = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:signalConverterInput" ) == 0 ) signalConverterInput_ = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:pulseGeneratorSlot"   ) == 0 ) pulseGeneratorSlot_   = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "c:pulseGeneratorOutput" ) == 0 ) pulseGeneratorOutput_ = utils::stringTo<int>( p->second );
  }
  //cout << param << endl << *this << endl;
}
