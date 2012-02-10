#ifndef __AFEB_teststand_fit_AbstractModel_h__
#define __AFEB_teststand_fit_AbstractModel_h__

#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include <iostream>

using namespace std;

namespace AFEB { namespace teststand { namespace fit {

      class AbstractModel;
      ostream& operator<<( ostream& os, const AbstractModel& am );

      class AbstractModel{

	friend ostream& operator<<( ostream& os, const AbstractModel& am );
	
      public:
	AbstractModel() :
	  jacobian_( NULL ){
	}

	/// Copy CTOR needed for STL containers
	///
	/// @param other Model to be copied.
	///
	AbstractModel( const AbstractModel& other ) :
	  jacobian_( NULL ){
	  if ( other.jacobian_ ) jacobian_ = new TMatrixD( *other.jacobian_ );
	}

	~AbstractModel(){
	  delete jacobian_;
	}

	// std::vector doesn't seem to use this:
	AbstractModel& operator=( const AbstractModel& rhs ){
	  if ( this == &rhs ) return *this;
	  delete jacobian_;
	  jacobian_ = NULL;
	  if ( rhs.jacobian_ ) jacobian_ = new TMatrixD( *rhs.jacobian_ );
	  return *this;
	}

	// std::vector doesn't seem to use this:
	bool operator==( const AbstractModel& rhs ) const {
	  if ( jacobian_ == rhs.jacobian_ ) return true;
	  if ( jacobian_ == NULL ) return false;
	  return *jacobian_ == *rhs.jacobian_;
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
