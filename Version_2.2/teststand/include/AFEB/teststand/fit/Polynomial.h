#ifndef __AFEB_teststand_fit_Polynomial_h__
#define __AFEB_teststand_fit_Polynomial_h__

#include "AFEB/teststand/fit/AbstractModel.h"
#include "TMath.h"

namespace AFEB { namespace teststand { namespace fit {

      template <int Degree>
      class Polynomial : public AbstractModel{

      public:
	TMatrixD& getJacobian( const TMatrixD& x ){
	  if ( jacobian_ == NULL ) jacobian_ = new TMatrixD( 1, Degree + 1 );
	  for ( int d = 0; d <= Degree; ++d ) (*jacobian_)( 0, d ) = TMath::Power( x( 0, 0 ), d );
	  return *jacobian_;
	}

	TMatrixD  getModelFunctionValue( const TMatrixD& x, const TMatrixD& parameters ){
	  TMatrixD value( 1, 1 );
	  value( 0, 0 ) = 0.;
	  for ( int d = 0; d <= Degree; ++d ) value( 0, 0 ) += parameters( d, 0 ) * TMath::Power( x( 0, 0 ), d );
	  return value;
	}

      };

    }
  }
}
#endif
