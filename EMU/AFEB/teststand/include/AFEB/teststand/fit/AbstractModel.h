#ifndef __AFEB_teststand_fit_AbstractModel_h__
#define __AFEB_teststand_fit_AbstractModel_h__

#include "TMatrixD.h"
#include "TMatrixDSym.h"

using namespace std;

namespace AFEB { namespace teststand { namespace fit {
      class AbstractModel{
	
      public:
	AbstractModel() :
	  jacobian_( NULL ){
	}
	virtual ~AbstractModel(){
	  delete jacobian_;
	}
	virtual TMatrixD& getJacobian( const TMatrixD& x )=0;
	virtual TMatrixD  getModelFunctionValue( const TMatrixD& x, const TMatrixD& parameters )=0;

      protected:
	TMatrixD* jacobian_;	       ///< dModelFunction/dParameters

      };
    }
  }
}
#endif
