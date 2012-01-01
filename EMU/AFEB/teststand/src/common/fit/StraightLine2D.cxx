#include "AFEB/teststand/fit/StraightLine2D.h"

AFEB::teststand::fit::StraightLine2D::StraightLine2D( const unsigned int nObservations ) :
  AFEB::teststand::fit::AbstractModel( nObservations ){
}

TMatrixDSym& AFEB::teststand::fit::StraightLine2D::getDataCovariance( const unsigned int iObservation, 
								      const TMatrixD& x, 
								      const TMatrixD& y, 
								      const TMatrixDSym& covariance ){
  if ( dataCovariance_ == NULL ) dataCovariance_ = new TMatrixDSym( covariance );
  else                          *dataCovariance_ = covariance;
  return *dataCovariance_;
}

TMatrixD& AFEB::teststand::fit::StraightLine2D::getJacobian( const unsigned int iObservation, 
							     const TMatrixD& x, 
							     const TMatrixD& y ){
  if ( jacobian_ == NULL ) jacobian_ = new TMatrixD( 1, 2 );
  (*jacobian_)( 0, 0 ) = 1;
  (*jacobian_)( 0, 1 ) = x( 0, 0 );
  return *jacobian_;
}
