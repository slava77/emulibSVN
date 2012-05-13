#include "AFEB/teststand/AnalyzedChannel.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/utils/ROOTIO.h"
#include "xcept/Exception.h"

#include "TMath.h"

#include <iomanip>

using namespace AFEB::teststand;

ostream& AFEB::teststand::operator<<( ostream& os, const AnalyzedChannel& d ){
  os << " gain="     << showpos << d.gain_                 << " +- " << noshowpos << d.gainError_
     << "   offset=" << showpos << d.offset_               << " +- " << noshowpos << d.offsetError_
     // << "   noise="  << showpos << d.noise_                << " +- " << noshowpos << d.noiseError_
     << "   C_int="  << showpos << d.internalCapacitance_  << " +- " << noshowpos << d.internalCapacitanceError_;
  return os;
}

AFEB::teststand::AnalyzedChannel::AnalyzedChannel() :
  gain_                    ( 0. ),
  gainError_               ( 0. ),
  offset_                  ( 0. ),
  offsetError_             ( 0. ),
  internalCapacitance_     ( 0. ),
  internalCapacitanceError_( 0. ){
}

void AFEB::teststand::AnalyzedChannel::calculateGain(){

  // Remember, instead of
  // V(Q) = offset_ + gain_ * Q
  // we fit its inverse function
  // Q(V) = p0      + p1    * V
  // It means
  // offset_ = -p0 / p1
  // gain_   =   1 / p1
  // therefore
  //      d(offset_,gain_)    / -1/p1    -p0/p1^2 \
  // A =  ---------------- = |                     |
  //      d(p0,p1)            \  0       - 1/p1^2 /
  // and the covariance matrix is
  // Var = A V A^T
  // where V is the covariance for (p0,p1).

  double p0 = QofVfitter_.getFittedParameter( 0 );
  double p1 = QofVfitter_.getFittedParameter( 1 );

  offset_ = - p0 / p1;
  gain_   =    1 / p1;

  TMatrixD A( 2, 2 );
  A( 0, 0 ) = -1/p1;  A( 0, 1 ) = -p0/(p1*p1);
  A( 1, 0 ) =    0.;  A( 1, 1 ) = - 1/(p1*p1);

  TMatrixD Var( A, TMatrixD::kMult, TMatrixD( QofVfitter_.getFittedParametersCovariance(), TMatrixD::kMultTranspose, A ) );
  offsetError_ = TMath::Sqrt( Var( 0, 0 ) );
  gainError_   = TMath::Sqrt( Var( 1, 1 ) );

  // cout << "pO,p1" << endl << QofVfitter_.getFittedParameters() << endl;
  // cout << "Var for pO,p1" << endl << QofVfitter_.getFittedParametersCovariance() << endl;
  // cout << "A" << endl << A << endl;
  // cout << "Var" << endl << Var << endl;
  // cout << *this << endl;

  // // Calculate the average of the noise measurements, too:
  // noise_      = noiseAverager_.getFittedParameter( 0 );
  // noiseError_ = TMath::Sqrt( noiseAverager_.getFittedParametersCovariance()( 0, 0 ) );
}

void AFEB::teststand::AnalyzedChannel::calculateInternalCapacitance( const pair<double,double> V_setThreshold,
								     const pair<double,double> V_measuredThreshold ){
  // At this point, offset and gain must already have been calculated.

  if ( V_measuredThreshold.first == 0. ){
    internalCapacitance_ = internalCapacitanceError_ = 0.;
    return;
  }

  // From the Q(V) function, get the the charge needed for the pre-amp to produce a voltage equal to the set discriminator threshold.
  TMatrixD x( 1, 1 );
  x( 0, 0 ) = V_setThreshold.first;
  double Q_threshold = QofVfitter_.getY( x )( 0, 0 );
  
  // The internal capaciance (C=Q/V)...
  internalCapacitance_ = Q_threshold / V_measuredThreshold.first;
  // cout << "Q_threshold=" << Q_threshold << " variance=" << QofVfitter_.getYCovariance( x ) 
  //      << "    V_measuredThreshold=" << V_measuredThreshold.first 
  //      << "   C_int=" << internalCapacitance_ << endl;

  // ...and its error
  TMatrixDSym Var( 2 ); // variance of Q and V
  Var( 0, 0 ) = QofVfitter_.getYCovariance( x )( 0, 0 );
  Var( 1, 1 ) = TMath::Power( V_measuredThreshold.second, 2 );
  Var( 0, 1 ) = Var( 1, 0 ) = 0.;
  TMatrixD A( 1, 2 ); // A = ( dC/dQ, dC/dV ) = ( 1/V, -Q/V^2 )
  A( 0, 0 ) = 1. / V_measuredThreshold.first;
  A( 0, 1 ) = - Q_threshold / TMath::Power( V_measuredThreshold.first, 2 );
  internalCapacitanceError_ = TMath::Sqrt( TMatrixD( A, TMatrixD::kMult, TMatrixD( Var, TMatrixD::kMultTranspose, A ) )( 0, 0 ) );
}

double AFEB::teststand::AnalyzedChannel::getThresholdCharge( const double setThresholdVoltage ){
  TMatrixD x( 1, 1 );
  x( 0, 0 ) = setThresholdVoltage;
  return QofVfitter_.getY( x )( 0, 0 );
}
