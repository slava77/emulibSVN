#include "AFEB/teststand/AnalyzedChannel.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "xcept/Exception.h"

#include <iomanip>

ostream& AFEB::teststand::operator<<( ostream& os, const AnalyzedChannel& d ){
  os << " gain="   << showpos << d.gain_ 
     << " offset=" << showpos << d.offset_;
  return os;
}

void AFEB::teststand::AnalyzedChannel::calculateGain(){
  gain_   = fitter_.getFittedParameter( AFEB::teststand::fit::StraightLine2D::slope     );
  offset_ = fitter_.getFittedParameter( AFEB::teststand::fit::StraightLine2D::intercept );
}
