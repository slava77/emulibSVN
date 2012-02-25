#include "AFEB/teststand/AnalyzedDevice.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "xcept/Exception.h"

#include "TMath.h"

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
							       const pair<double,double> V_setThreshold ,
							       const pair<double,double> V_measuredThreshold,
							       const pair<double,double> V_measuredNoise ){
  // Fit Q(V) instead of V(Q) to better estimate the error of the fit parameters. (The error is on Q rather than on V.)

  TMatrixD    x  ( 1, 1 );
  TMatrixD    y  ( 1, 1 );
  TMatrixDSym var( 1 );
  // Convert measured threshold voltage to injected charge [fC]. Take into account the correction coefficient as well:
  x  ( 0, 0 ) = V_setThreshold.first;
  y  ( 0, 0 ) = fC_from_mV( V_measuredThreshold.first );
  var( 0, 0 ) = TMath::Power( fC_from_mV( V_measuredThreshold.second ), 2 );
  channels_[iChannel].QofVfitter_   .addObservation( x, y, var );
  y  ( 0, 0 ) = fC_from_mV( V_measuredNoise.first );
  var( 0, 0 ) = TMath::Power( fC_from_mV( V_measuredNoise.second ), 2 );
  channels_[iChannel].noiseAverager_.addObservation( x, y, var );
  // cout << channels_[iChannel].noiseAverager_.getObservationCount() << " Added ( " << x(0,0) << ", " << y(0,0) << "+-" << TMath::Sqrt( var(0,0) ) << " )" << endl;
}


void AFEB::teststand::AnalyzedDevice::calculateGains(){
  for ( vector<AnalyzedChannel>::iterator c = channels_.begin(); c != channels_.end(); ++c ){
    c->calculateGain();
  }
  // cout << channels_ << endl;
}

void AFEB::teststand::AnalyzedDevice::calculateInternalCapacitance( const int iChannel,
								    const pair<double,double> V_setThreshold,
								    const pair<double,double> V_measuredThreshold ){
  channels_[iChannel].calculateInternalCapacitance( V_setThreshold, V_measuredThreshold );
}

valarray<double> AFEB::teststand::AnalyzedDevice::getNoises() const {
  valarray<double> v( channels_.size() );
  size_t n = 0;
  for ( vector<AnalyzedChannel>::const_iterator c = channels_.begin(); c != channels_.end(); ++c ) v[n++] = c->noise_;
  return v;
}

valarray<double> AFEB::teststand::AnalyzedDevice::getGains() const {
  valarray<double> v( channels_.size() );
  size_t n = 0;
  for ( vector<AnalyzedChannel>::const_iterator c = channels_.begin(); c != channels_.end(); ++c ) v[n++] = c->gain_;
  return v;
}

valarray<double> AFEB::teststand::AnalyzedDevice::getOffsets() const {
  valarray<double> v( channels_.size() );
  size_t n = 0;
  for ( vector<AnalyzedChannel>::const_iterator c = channels_.begin(); c != channels_.end(); ++c ) v[n++] = c->offset_;
  return v;
}

valarray<double> AFEB::teststand::AnalyzedDevice::getInternalCapacitances() const {
  valarray<double> v( channels_.size() );
  size_t n = 0;
  for ( vector<AnalyzedChannel>::const_iterator c = channels_.begin(); c != channels_.end(); ++c ) v[n++] = c->internalCapacitance_;
  return v;
}
