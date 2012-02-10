#include "AFEB/teststand/fit/StraightLine2D.h"


ostream& AFEB::teststand::fit::operator<<( ostream& os, const AbstractModel& am ){
  if ( am.jacobian_ ){
    os << "Jacobian:" << endl;
    am.jacobian_->Print();
  }
  else{
    os << "Jacobian undefined.";
  }
  return os;
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
