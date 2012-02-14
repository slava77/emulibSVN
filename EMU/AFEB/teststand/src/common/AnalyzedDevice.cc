#include "AFEB/teststand/AnalyzedDevice.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "xcept/Exception.h"

#include <iomanip>

ostream& AFEB::teststand::operator<<( ostream& os, const AnalyzedDevice& d ){
  os << *dynamic_cast<const TestedDevice*>( &d );
  os << "correction coefficient      injection capacitance [pF]      pulse division factor" << endl
     << right
     << setw(22) << d.getCorrectionCoefficient() << "     "
     << setw(26) << d.getInjectionCapacitance()  << "     "
     << setw(21) << d.getPulseDivisionFactor()
     << endl << "Channels:" << endl;
  int i=1;
  for ( vector<AnalyzedChannel>::const_iterator c = d.channels_.begin(); c != d.channels_.end(); ++c ){
    cout << right << setw(10) << i++ << *c << endl;
  }  
  return os;
}

AFEB::teststand::AnalyzedDevice::AnalyzedDevice( const TestedDevice& device )
  : TestedDevice( device )
  , correctionCoefficient_( 0. )
  , injectionCapacitance_ ( 0. )
  , pulseDivisionFactor_  ( 0. )
{
  channels_.resize( nChannels_ );
}

void AFEB::teststand::AnalyzedDevice::addThresholdMeasurement( const int iChannel, 
							       const pair<double,double> setThreshold, 
							       const pair<double,double> measuredThreshold ){
  TMatrixD    x  ( 1, 1 );
  TMatrixD    y  ( 1, 1 );
  TMatrixDSym var( 1 );
  x  ( 0, 0 ) = setThreshold.first;
  y  ( 0, 0 ) = measuredThreshold.first;
  var( 0, 0 ) = measuredThreshold.second * measuredThreshold.second;
  //cout << "Variance: "; var.Print();
  channels_[iChannel].fitter_.addObservation( x, y, var );
}

void AFEB::teststand::AnalyzedDevice::calculateGains(){
  for ( vector<AnalyzedChannel>::iterator c = channels_.begin(); c != channels_.end(); ++c ){
    c->calculateGain();
  }
  cout << channels_ << endl;
}
