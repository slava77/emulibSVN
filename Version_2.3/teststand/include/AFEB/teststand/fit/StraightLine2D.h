#ifndef __AFEB_teststand_fit_StraightLine2D_h__
#define __AFEB_teststand_fit_StraightLine2D_h__

#warning "Do not use AFEB::teststand::fit::StraightLine2D, which is deprecated. Use AFEB::teststand::fit::Polynomial<1> instead."

#include "AFEB/teststand/fit/AbstractModel.h"

namespace AFEB { namespace teststand { namespace fit {
      class StraightLine2D : public AbstractModel{

      public:
	enum Parameter_t { intercept, slope };
	TMatrixD& getJacobian( const TMatrixD& x );
	TMatrixD  getModelFunctionValue( const TMatrixD& x, const TMatrixD& parameters );

      };
    }
  }
}
#endif
