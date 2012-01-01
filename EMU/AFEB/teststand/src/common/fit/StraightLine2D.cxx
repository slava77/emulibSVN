#include "AFEB/teststand/fit/StraightLine2D.h"

AFEB::teststand::fit::StraightLine2D::StraightLine2D() :
  AFEB::teststand::fit::AbstractModel(){
}

TMatrixD& AFEB::teststand::fit::StraightLine2D::getJacobian( const TMatrixD& x ){
  if ( jacobian_ == NULL ) jacobian_ = new TMatrixD( 1, 2 );
  (*jacobian_)( 0, 0 ) = 1;
  (*jacobian_)( 0, 1 ) = x( 0, 0 );
  return *jacobian_;
}

TMatrixD AFEB::teststand::fit::StraightLine2D::getModelFunctionValue( const TMatrixD& x, const TMatrixD& parameters ){
  TMatrixD value( 1, 1 );
  value( 0, 0 ) = parameters( 0, 0 ) + parameters( 1, 0 ) * x( 0, 0 );
  return value;
}
