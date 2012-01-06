#ifndef __AFEB_teststand_fit_LeastSquaresFitter_h__
#define __AFEB_teststand_fit_LeastSquaresFitter_h__

#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

using namespace std;

namespace AFEB { namespace teststand { namespace fit {

      template <class Model>
      class LeastSquaresFitter{

      public:
	LeastSquaresFitter( unsigned int nObservations = 0 ) :
	  model_               ( new Model() ),
	  dataCovariance_      ( NULL ),
	  jacobian_            ( NULL ),
	  y_                   ( NULL ),
	  Gamma_               ( NULL ),
	  beta_                ( NULL ),
	  parameters_          ( NULL ),
	  parametersCovariance_( NULL ){
	}

	~LeastSquaresFitter(){
	  delete model_;
	  delete dataCovariance_;
	  delete jacobian_;
	  delete y_;
	  delete Gamma_;
	  delete beta_;
	  delete parameters_;
	  delete parametersCovariance_;
	}

	void setParameterNames( const vector<string>& parameterNames ){
	  parameterNames_ = parameterNames;
	}


	/// Add an observation (i.e., ameasurement)
	///
	/// @param x  the observed independent coordinate of the data
	/// @param y  y the observed dependent coordinate of the data (which we model by y=f(x), where f is the model function)
	/// @param covariance  the covariance of the dependent data coordinates
	/// @param iObservation index of this observation
	///	
	void addObservation( const TMatrixD& x, const TMatrixD& y, const TMatrixDSym& covariance, const unsigned int iObservation=0 ){
	  // Case of uncorrelated observations.

	  if ( jacobian_ == NULL ){

	    dataCovariance_ = new TMatrixDSym( covariance );
	    jacobian_       = new TMatrixD   ( model_->getJacobian ( x ) );

	    // Build Gamma for the normal equation:
	    TMatrixDSym dataCovariance_Inv( TMatrixDSym::kInverted, *dataCovariance_ );
	    Gamma_ = new TMatrixD( TMatrixD( *jacobian_, TMatrixD::kTransposeMult, dataCovariance_Inv ),  TMatrixD::kMult, *jacobian_ );

	    // Build beta for the normal equation:
	    beta_ = new TMatrixD( TMatrixD( *jacobian_, TMatrixD::kTransposeMult, dataCovariance_Inv ),  TMatrixD::kMult, y );

	  }
	  else{

	    (*dataCovariance_) = covariance;
	    (*jacobian_)       = model_->getJacobian( x );

	    // Build Gamma for the normal equation:
	    TMatrixDSym dataCovariance_Inv( TMatrixDSym::kInverted, *dataCovariance_ );
	    *Gamma_ += TMatrixD( TMatrixD( *jacobian_, TMatrixD::kTransposeMult, dataCovariance_Inv ),  TMatrixD::kMult, *jacobian_ );

	    // Build beta for the normal equation:
	    *beta_ += TMatrixD( TMatrixD( *jacobian_, TMatrixD::kTransposeMult, dataCovariance_Inv ),  TMatrixD::kMult, y );

	  }
	  observationCount_++;
	  
	}

	// void addObservation( const TMatrixD& x, const TMatrixD& y, const TMatrixDSym& covariance, const unsigned int iObservation=0 ){
	//   // This is how to solve it when the observations are correlated.
	//   if ( jacobian_ == NULL ){
	//     dataCovariance_ = new TMatrixDSym( model_->getDataCovariance( iObservation, x, covariance ) );
	//     jacobian_       = new TMatrixD   ( model_->getJacobian      ( iObservation, x,            ) );
	//     y_              = new TMatrixD   ( y );
	//   }
	//   else{
	//     (*dataCovariance_) += model_->getDataCovariance( iObservation, x, covariance );
	//     (*jacobian_)       += model_->getJacobian      ( iObservation, x,            );
	//     (*y_)              += y;
	//   }
	//   observationCount_++;
	// }

	TMatrixD getFittedParameters( bool refit=false ){
	  if ( observationCount_ == 0 ) throw logic_error( "No observations yet." );

	  if ( parameters_ == NULL || refit ) solve();

	  return *parameters_;
	}

	double getFittedParameter( const unsigned int index, const bool refit=false ){
	  if ( observationCount_ == 0 ) throw logic_error( "No observations yet." );

	  if ( jacobian_->GetNcols() <= index ){
	    stringstream ss;
	    ss << "No parameter of index " << index;
	    throw out_of_range( ss.str() );
	  }

	  if ( parameters_ == NULL || refit ) solve();

	  return (*parameters_)( index, 0 );
	}

	double getFittedParameter( const string& name, const bool refit=false ){
	  if ( observationCount_ == 0 ) throw logic_error( "No observations yet." );

	  map<string,int>::const_iterator i = parameterIndices_.find( name );
	  if ( i == parameterIndices_.end() ){
	    stringstream ss;
	    ss << "No parameter named \"" << name << "\"";
	    throw out_of_range( ss.str() );
	  }

	  if ( parameters_ == NULL || refit ) solve();

	  return (*parameters_)( i->second, 0 );
	}

	TMatrixDSym getFittedParametersCovariance( const bool refit=false ){
	  if ( observationCount_ == 0 ) throw logic_error( "No observations yet." );
	  if ( parameters_ == NULL || refit ) solve();

	  return *parametersCovariance_;
	}	

	TMatrixD getY( const TMatrixD& x, const bool refit=false ){
	  if ( observationCount_ == 0 ) throw logic_error( "No observations yet." );
	  if ( parameters_ == NULL || refit ) solve();

	  return model_->getModelFunctionValue( x, *parameters_ );
	}

	TMatrixD getYCovariance( const TMatrixD& x, const bool refit=false ){
	  if ( observationCount_ == 0 ) throw logic_error( "No observations yet." );
	  if ( parameters_ == NULL || refit ) solve();

	  TMatrixD yCovariance;
	  return TMatrixD( model_->getJacobian( x ), TMatrixD::kMult, TMatrixD( *parametersCovariance_, TMatrixD::kMultTranspose,  model_->getJacobian( x ) ) );
	}

      private:
	void solve(){
	  // Case of uncorrelated observations.
	  
	  // Solve Gamma * parameters = beta
	  // where Gamma = J^T * V^-1 * J   (is also the inverse of the fitted parameters' covariance)
	  //       beta  = J^T * V^-1 * y
	  // with J being the Jacobian (dModelFunction/dParameters), J^T its transpose
	  // V the (co)variance of the data
	  // y the observed dependent coordinate(s) of the data, i.e., calculate
	  // parameters = Gamma^-1 * beta

	  delete parameters_;
	  delete parametersCovariance_;
	  
	  // Invert Gamma as general square matrix. Inversion of TMatrixDSym is unstable.
	  //cout << "Gamma" << endl; Gamma_->Print();
	  double det = 0.;
	  Gamma_->Invert( &det ); // From here on, Gamma is GammaInverse!
	  //cout << "Gamma's determinant " << det << endl;
	  //cout << "Gamma inverse" << endl; Gamma_->Print();

	  // It's inverse is the inverse of the fitted parameters' covariance (a symmetric matrix):
	  parametersCovariance_ = new TMatrixDSym( Gamma_->GetNrows() );
	  parametersCovariance_->SetMatrixArray( Gamma_->GetMatrixArray() );
	  //cout << "parametersCovariance_" << endl; parametersCovariance_->Print();

	  // The fitted parameters:
	  parameters_ = new TMatrixD( *parametersCovariance_, TMatrixD::kMult, *beta_ );

	}

	// void solve(){
	//   // This is how to solve it when the observations are correlated.

	//   // Solve Gamma * parameters = beta
	//   // where Gamma = J^T * V^-1 * J   (is also the inverse of the fitted parameters' covariance)
	//   //       beta  = J^T * V^-1 * y
	//   // with J being the Jacobian (dModelFunction/dParameters), J^T its transpose
	//   // V the (co)variance of the data
	//   // y the observed dependent coordinate(s) of the data, i.e., calculate
	//   // parameters = Gamma^-1 * beta

	//   delete parameters_;
	//   delete parametersCovariance_;
	//   TMatrixDSym dataCovariance_Inv( TMatrixDSym::kInverted, *dataCovariance_ );
	//   TMatrixD beta ( TMatrixD( *jacobian_, TMatrixD::kTransposeMult, dataCovariance_Inv ),  TMatrixD::kMult, *y_ );
	//   TMatrixD Gamma( TMatrixD( *jacobian_, TMatrixD::kTransposeMult, dataCovariance_Inv ),  TMatrixD::kMult, *jacobian_ );
	//   cout << "dataCovariance_" << endl; dataCovariance_->Print();
	//   cout << "dataCovariance_Inv" << endl; dataCovariance_Inv.Print();
	//   cout << "jacobian_" << endl; jacobian_->Print();
	//   cout << "y_" << endl; y_->Print();
	//   cout << "beta" << endl; beta.Print();
	//   cout << "Gamma" << endl; Gamma.Print();
	//   //cout << "" << endl; Print();
	//   // Invert Gamma as general square matrix. Inversion of TMatrixDSym is unstable.
	//   Gamma.Invert(); // From here on, Gamma is GammaInverse!
	//   cout << "Gamma_Inv" << endl; Gamma.Print();
	//   // It's inverse is the inverse of the fitted parameters' covariance (a symmetric matrix):
	//   parametersCovariance_ = new TMatrixDSym( Gamma.GetNrows() );
	//   parametersCovariance_->SetMatrixArray( Gamma.GetMatrixArray() );
	//   cout << "parametersCovariance_" << endl; parametersCovariance_->Print();
	//   // The fitted parameters:
	//   parameters_ = new TMatrixD( *parametersCovariance_, TMatrixD::kMult, beta );
	// }

	Model*          model_;	               ///< the fitting model
	vector<string>  parameterNames_;       ///< parameterNames_[i] is the name of the parameter of index i
	map<string,int> parameterIndices_;     ///< parameter name --> parameter index mapping, the inverse of parameterNames_
	unsigned int    observationCount_;     ///< the number of observations so far
	
	TMatrixDSym*    dataCovariance_;       ///< covariance of the data
	TMatrixD*       jacobian_;	       ///< dModelFunction/dParameters
	TMatrixD*       y_;		       ///< the observed dependent coordinate(s) of the data

	TMatrixD*       Gamma_;	///< auxilliary matrix (for the Gamma * parameters = beta normal equation)
	TMatrixD*       beta_;	///< auxilliary matrix (for the Gamma * parameters = beta normal equation)

	TMatrixD*       parameters_;	       ///< the fitted parameters
	TMatrixDSym*    parametersCovariance_; ///< the fitted parameters' covariance
      };

    }
  }
}
#endif
