#ifndef __AFEB_teststand_fit_StraightLine2D_h__
#define __AFEB_teststand_fit_StraightLine2D_h__

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
