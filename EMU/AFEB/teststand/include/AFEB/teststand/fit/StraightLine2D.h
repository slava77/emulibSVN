#ifndef __AFEB_teststand_fit_StraightLine2D_h__
#define __AFEB_teststand_fit_StraightLine2D_h__

#include "AFEB/teststand/fit/AbstractModel.h"

namespace AFEB { namespace teststand { namespace fit {
      class StraightLine2D : public AbstractModel{

      public:
	StraightLine2D( unsigned int nObservations );
	TMatrixDSym& getDataCovariance( const unsigned int iObservation, const TMatrixD& x, const TMatrixD& y, const TMatrixDSym& covariance );
	TMatrixD&    getJacobian( const unsigned int iObservation, const TMatrixD& x, const TMatrixD& y );

      };
    }
  }
}
#endif
