#include "AFEB/teststand/AnalyzedDevice.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "xcept/Exception.h"

#include <iomanip>

ostream& AFEB::teststand::operator<<( ostream& os, const AnalyzedDevice& d ){
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

AFEB::teststand::AnalyzedDevice::AnalyzedDevice( const string type, const int nChannels ) :
  Device( type, nChannels ){
  resizeContainers();
}

AFEB::teststand::AnalyzedDevice::AnalyzedDevice( const Device& device ) :
  Device( device ){
  resizeContainers();
}

void AFEB::teststand::AnalyzedDevice::resizeContainers(){
  gains_.resize( nChannels_ );
  fitters_.resize( nChannels_ );
}

