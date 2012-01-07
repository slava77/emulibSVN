#include "AFEB/teststand/DAC.h"

#include "AFEB/teststand/utils/ROOTIO.h"
#include "TMath.h"

using namespace std;
using namespace AFEB::teststand;

ostream& AFEB::teststand::operator<<( ostream& os, const DAC& dac ){
  os << "DAC channel " << dac.channel_
     << " in "         << dac.moduleName_
     << " module "     << dac.moduleId_
     << " for "        << dac.type_;
  if ( dac.getCalibrationParameters() != NULL ) 
    os << endl << "Parameters" << endl << *dac.getCalibrationParameters();
  if ( dac.getCalibrationParametersCovariance() != NULL ) 
    os << endl << "Parameters' covariance" << endl << *dac.getCalibrationParametersCovariance();
  return os;
}

AFEB::teststand::DAC::DAC( const string& moduleId,
			   const string& moduleName,
			   const string& type,
			   const int     channel    )
  : moduleId_  ( moduleId   )
  , moduleName_( moduleName )
  , type_      ( type       )
  , channel_   ( channel    )
  , calibrationParameters_          ( NULL )
  , calibrationParametersCovariance_( NULL ){
}

AFEB::teststand::DAC::DAC( const DAC& other )  
  : moduleId_  ( other.getModuleid()   )
  , moduleName_( other.getModuleName() )
  , type_      ( other.getType()       )
  , channel_   ( other.getChannel()    )
  , calibrationParameters_          ( NULL )
  , calibrationParametersCovariance_( NULL ){
  if ( other.getCalibrationParameters()           != NULL ) calibrationParameters_           = new TMatrixD   ( *other.getCalibrationParameters()           );
  if ( other.getCalibrationParametersCovariance() != NULL ) calibrationParametersCovariance_ = new TMatrixDSym( *other.getCalibrationParametersCovariance() );
}

AFEB::teststand::DAC::~DAC(){
  delete calibrationParameters_;
  delete calibrationParametersCovariance_;
}

AFEB::teststand::DAC& 
AFEB::teststand::DAC::operator=( const DAC& rhs ){
  if ( this == &rhs ) return *this;

  moduleId_   = rhs.getModuleid()  ;
  moduleName_ = rhs.getModuleName();
  type_       = rhs.getType()      ;
  channel_    = rhs.getChannel()   ;

  if ( rhs.getCalibrationParameters()           != NULL ) calibrationParameters_           = new TMatrixD   ( *rhs.getCalibrationParameters()           );
  if ( rhs.getCalibrationParametersCovariance() != NULL ) calibrationParametersCovariance_ = new TMatrixDSym( *rhs.getCalibrationParametersCovariance() );
  
  return *this;
}

bool 
AFEB::teststand::DAC::operator<( const DAC& rhs ) const {
  return ( (moduleId_+":"+moduleName_+":"+type_) 
	   <
	   (rhs.getModuleid()+":"+rhs.getModuleName()+":"+rhs.getType()) );
}

bool 
AFEB::teststand::DAC::operator==( const DAC& rhs ) const {
  return ( (moduleId_+":"+moduleName_+":"+type_) 
	   == 
	   (rhs.getModuleid()+":"+rhs.getModuleName()+":"+rhs.getType()) );
}

void 
AFEB::teststand::DAC::setCalibrationParameters( const TMatrixD& parameters,
						const TMatrixDSym& covariance ){
  calibrationParameters_           = new TMatrixD   ( parameters );
  calibrationParametersCovariance_ = new TMatrixDSym( covariance );
}

pair<double,double> 
AFEB::teststand::DAC::toMilliVolts( const double valueInDACUnits, 
				    const double errorInDACUnits ) const {
  TMatrixD Jacobian( DAC::nCalibrationParameters, 0 );
  Jacobian( DAC::intercept, 0 ) = 1.;
  Jacobian( DAC::slope    , 0 ) = valueInDACUnits;
  double varianceDueToParameters = TMatrixD( Jacobian, 
					     TMatrixD::kMult, 
					     TMatrixD( *calibrationParametersCovariance_, 
						       TMatrixD::kMultTranspose, 
						       Jacobian ) 
					     )( 0, 0 );
  double varianceDueToObservation = TMath::Power( (*calibrationParameters_)( DAC::slope, 0 ) * errorInDACUnits, 2 );
  return make_pair<double,double>( (*calibrationParameters_)( DAC::intercept, 0 ) + 
				   (*calibrationParameters_)( DAC::slope    , 0 ) * valueInDACUnits,
				   TMath::Sqrt( varianceDueToParameters + varianceDueToObservation ) );
}
