#include "AFEB/teststand/AnalyzedChannel.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/utils/ROOTIO.h"
#include "xcept/Exception.h"

#include "TMath.h"

#include <iomanip>

using namespace AFEB::teststand;

ostream& AFEB::teststand::operator<<( ostream& os, const AnalyzedChannel& d ){
  os << " gain="     << showpos << d.gain_   << " +- " << noshowpos << d.gainError_
     << "   offset=" << showpos << d.offset_ << " +- " << noshowpos << d.offsetError_;
  return os;
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

  double p0 = fitter_.getFittedParameter( fit::StraightLine2D::intercept );
  double p1 = fitter_.getFittedParameter( fit::StraightLine2D::slope     );

  offset_ = - p0 / p1;
  gain_   =    1 / p1;

  TMatrixD A( 2, 2 );
  A( 0, 0 ) = -1/p1;  A( 0, 1 ) = -p0/(p1*p1);
  A( 1, 0 ) =    0.;  A( 1, 1 ) = - 1/(p1*p1);

  TMatrixD Var( A, TMatrixD::kMult, TMatrixD( fitter_.getFittedParametersCovariance(), TMatrixD::kMultTranspose, A ) );
  offsetError_ = TMath::Sqrt( Var( fit::StraightLine2D::intercept, fit::StraightLine2D::intercept ) );
  gainError_   = TMath::Sqrt( Var( fit::StraightLine2D::slope    , fit::StraightLine2D::slope     ) );

  // cout << "pO,p1" << endl << fitter_.getFittedParameters() << endl;
  // cout << "Var for pO,p1" << endl << fitter_.getFittedParametersCovariance() << endl;
  // cout << "A" << endl << A << endl;
  // cout << "Var" << endl << Var << endl;
  // cout << *this << endl;
}
