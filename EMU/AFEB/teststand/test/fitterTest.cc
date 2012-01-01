#include "AFEB/teststand/fit/AbstractModel.h"
#include "AFEB/teststand/fit/StraightLine2D.h"
#include "AFEB/teststand/fit/LeastSquaresFitter.h"

#include "TRandom.h"

#include <iostream>


namespace AFEB { namespace teststand { namespace fit {

      ///
      /// y = a + B * x
      /// with B = [B_ij] i=0,1,2 j=0,1,2
      ///      a = [a_j]  j=0,1,2
      ///

      class Linear3DTo3D : public AbstractModel{

      public:
	Linear3DTo3D( unsigned int nObservations ) : 
	  AFEB::teststand::fit::AbstractModel( nObservations ){
	}

	TMatrixDSym& getDataCovariance( const unsigned int iObservation, const TMatrixD& x, const TMatrixD& y, const TMatrixDSym& covariance ){
	  if ( dataCovariance_ == NULL ) dataCovariance_ = new TMatrixDSym( covariance );
	  else                          *dataCovariance_ = covariance;
	  return *dataCovariance_;
	}

	TMatrixD&    getJacobian( const unsigned int iObservation, const TMatrixD& x, const TMatrixD& y ){
	  if ( jacobian_ == NULL ) jacobian_ = new TMatrixD( 3, 12 );
	  /// parameters = ( a_0, a_1, a_2, B_00, B_01, ... B_21, B_22 ) 
	  (*jacobian_)( 0,  0 ) = 1.;
	  (*jacobian_)( 0,  1 ) = 0.;
	  (*jacobian_)( 0,  2 ) = 0.;
	  (*jacobian_)( 0,  3 ) = x( 0, 0 );  
	  (*jacobian_)( 0,  4 ) = x( 1, 0 ); 
	  (*jacobian_)( 0,  5 ) = x( 2, 0 );
	  (*jacobian_)( 0,  6 ) = 0.;
	  (*jacobian_)( 0,  7 ) = 0.;
	  (*jacobian_)( 0,  8 ) = 0.;
	  (*jacobian_)( 0,  9 ) = 0.;
	  (*jacobian_)( 0, 10 ) = 0.;
	  (*jacobian_)( 0, 11 ) = 0.;

	  (*jacobian_)( 1,  0 ) = 0.;
	  (*jacobian_)( 1,  1 ) = 1.;
	  (*jacobian_)( 1,  2 ) = 0.;
	  (*jacobian_)( 1,  3 ) = 0.;
	  (*jacobian_)( 1,  4 ) = 0.;
	  (*jacobian_)( 1,  5 ) = 0.;
	  (*jacobian_)( 1,  6 ) = x( 0, 0 );
	  (*jacobian_)( 1,  7 ) = x( 1, 0 );
	  (*jacobian_)( 1,  8 ) = x( 2, 0 );
	  (*jacobian_)( 1,  9 ) = 0.;
	  (*jacobian_)( 1, 10 ) = 0.;
	  (*jacobian_)( 1, 11 ) = 0.;

	  (*jacobian_)( 2,  0 ) = 0.;	  
	  (*jacobian_)( 2,  1 ) = 0.;	  
	  (*jacobian_)( 2,  2 ) = 1.;	  
	  (*jacobian_)( 2,  3 ) = 0.;
	  (*jacobian_)( 2,  4 ) = 0.;
	  (*jacobian_)( 2,  5 ) = 0.;
	  (*jacobian_)( 2,  6 ) = 0.;
	  (*jacobian_)( 2,  7 ) = 0.;
	  (*jacobian_)( 2,  8 ) = 0.;
	  (*jacobian_)( 2,  9 ) = x( 0, 0 );
	  (*jacobian_)( 2, 10 ) = x( 1, 0 );
	  (*jacobian_)( 2, 11 ) = x( 2, 0 );

	  //jacobian_->Print();

	  return *jacobian_;
	}

      };
    }
  }
}

using namespace std;
using namespace AFEB::teststand::fit;

int main( int argc, char** argv ){

  //
  // Straight line
  //
  TMatrixD x(1,1);
  TMatrixD y(1,1);
  TMatrixDSym C(1);
  C(0,0) = 1.;
  LeastSquaresFitter<StraightLine2D> slfitter;
  for ( unsigned int i=0; i<10; ++i ){
    x(0,0) = i;
    y(0,0) = 5*i;
    slfitter.addObservation( x, y, C );
  }
  cout << "Fitted parameters:" << endl;
  slfitter.getFittedParameters().Print();

  //
  // General linear 3D --> 3D
  //
  TMatrixD a( 3, 1 );
  TMatrixD B( 3, 3 );
  TMatrixD xx( 3, 1 );
  TMatrixD yy( 3, 1 );
  TMatrixDSym CC( 3 );

  a( 0, 0 ) = 1.;
  a( 1, 0 ) = 2.;
  a( 2, 0 ) = 3.;

  B( 0, 0 ) =  4.; B( 0, 1 ) = -5.; B( 0, 2 ) =  6.;
  B( 1, 0 ) =  7.; B( 1, 1 ) = -8.; B( 1, 2 ) =  9.;
  B( 2, 0 ) = 10.; B( 2, 1 ) = 11.; B( 2, 2 ) = 12.;

  CC( 0, 0 ) = 16;
  CC( 1, 1 ) = 25.;
  CC( 2, 2 ) = 49.;

  LeastSquaresFitter<Linear3DTo3D> l3dfitter;
  TRandom random(34565);
  for ( unsigned int i=0; i<100; ++i ){
    xx(0,0) = 20.*(random.Rndm( i+1 )-0.5);
    xx(1,0) = 20.*(random.Rndm( i+2 )-0.5);
    xx(2,0) = 20.*(random.Rndm( i+3 )-0.5);

    TMatrixD uncertainty( 3, 1 );
    uncertainty(0,0) = 4.*(random.Rndm( i+1 )-0.5);
    uncertainty(1,0) = 5.*(random.Rndm( i+2 )-0.5);
    uncertainty(2,0) = 7.*(random.Rndm( i+3 )-0.5);
    
    yy = a + B * xx + uncertainty;

    l3dfitter.addObservation( xx, yy, CC );
    // if ( i == 0 ){
    //   cout << "xx:" << endl; xx.Print();
    //   cout << "yy:" << endl; yy.Print();
    //   cout << "CC:" << endl; CC.Print();
    // }
  }
  cout << "Fitted parameters:" << endl;
  l3dfitter.getFittedParameters().Print();


  return 0;
}
