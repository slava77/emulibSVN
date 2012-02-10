#include "AFEB/teststand/AnalyzedChannel.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "xcept/Exception.h"

#include <iomanip>

ostream& AFEB::teststand::operator<<( ostream& os, const AnalyzedChannel& d ){
  os << "Gain=" << d.gain_ << endl;
  return os;
}
