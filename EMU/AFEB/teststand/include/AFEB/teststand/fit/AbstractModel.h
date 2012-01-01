#ifndef __AFEB_teststand_fit_AbstractModel_h__
#define __AFEB_teststand_fit_AbstractModel_h__

#include "TMatrixD.h"
#include "TMatrixDSym.h"

using namespace std;

namespace AFEB { namespace teststand { namespace fit {
      class AbstractModel{
	
      public:
	AbstractModel( unsigned int nObservations ) :
	  nObservations_(),
	  dataCovariance_( NULL ),
	  jacobian_( NULL ){
	}
	virtual ~AbstractModel(){
	  delete dataCovariance_;
	  delete jacobian_;
	}
	virtual TMatrixDSym& getDataCovariance( const unsigned int iObservation,
						const TMatrixD& x,
						const TMatrixD& y,
						const TMatrixDSym& covariance )=0;
	virtual TMatrixD&    getJacobian      ( const unsigned int iObservation, 
						const TMatrixD& x, 
						const TMatrixD& y )=0;

      protected:
	unsigned int    nObservations_;	       ///< number of observations
	TMatrixDSym*    dataCovariance_;       ///< covariance of the data
	TMatrixD*       jacobian_;	       ///< dModelFunction/dParameters

      };
    }
  }
}
#endif
