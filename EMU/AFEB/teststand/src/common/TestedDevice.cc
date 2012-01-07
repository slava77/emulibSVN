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
     << setw(12) << d.tdcInput_ 
     << setw(20) << d.signalConverterSlot_ << endl;
  return os;
}

AFEB::teststand::TestedDevice::TestedDevice( const string type, const int nChannels, Crate* const crate ) :
  Device( type, nChannels ),
  crate_( crate ){}
