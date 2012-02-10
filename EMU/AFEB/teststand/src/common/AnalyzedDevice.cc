#include "AFEB/teststand/AnalyzedDevice.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "xcept/Exception.h"

#include <iomanip>

ostream& AFEB::teststand::operator<<( ostream& os, const AnalyzedDevice& d ){
  os << *dynamic_cast<const TestedDevice*>( &d ) << endl;
  return os;
}

AFEB::teststand::AnalyzedDevice::AnalyzedDevice( const TestedDevice& device ) :
  TestedDevice( device ){
  channels_.resize( nChannels_ );
}

AFEB::teststand::AnalyzedDevice::AnalyzedDevice( const AnalyzedDevice& other ) :
  TestedDevice( other ){
  for ( vector<AnalyzedChannel>::const_iterator c = other.channels_.begin(); c != other.channels_.end(); ++c ){
    channels_.push_back( *c );
  }
}
