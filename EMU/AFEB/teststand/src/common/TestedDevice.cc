#include "AFEB/teststand/TestedDevice.h"
#include "AFEB/teststand/utils/String.h"
#include "xcept/Exception.h"

#include <iomanip>

ostream& AFEB::teststand::operator<<( ostream& os, const TestedDevice& d ){
  os << "socket    type                id                      TDC slot   TDC input" << endl
     << right
     << setw(6) << d.socket_ << "    "
     << left
     << setw(20) << d.type_ 
     << setw(20) << d.id_
     << right 
     << setw(12) << d.tdcSlot_ 
     << setw(12) << d.tdcInput_ << endl;
  return os;
}

AFEB::teststand::TestedDevice::TestedDevice( const string type, const int nChannels, Crate* const crate ) :
  type_( type ),
  nChannels_( nChannels ),
  crate_( crate ){}


void AFEB::teststand::TestedDevice::setParameters( const vector< pair<string,string> >& param ){
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){ //TODO: replace rfind with compare.
    if      ( p->first.rfind( "id"       ) != string::npos ) id_       = p->second;
    else if ( p->first.rfind( "socket"   ) != string::npos ) socket_   = utils::stringTo<int>( p->second );
    else if ( p->first.rfind( "tdcSlot"  ) != string::npos ) tdcSlot_  = utils::stringTo<int>( p->second );
    else if ( p->first.rfind( "tdcInput" ) != string::npos ) tdcInput_ = utils::stringTo<int>( p->second );
  }
}
