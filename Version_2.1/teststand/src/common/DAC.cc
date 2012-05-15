#include "AFEB/teststand/DAC.h"

#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/ROOTIO.h"
#include "xcept/Exception.h"
#include "TMath.h"

using namespace std;
using namespace AFEB::teststand;

const char* const AFEB::teststand::DAC::typeStrings_[DAC::nTypes] = { "pulse", "threshold" };

ostream& AFEB::teststand::operator<<( ostream& os, const DAC& dac ){
  os << "DAC socket "  << dac.getSocket()
     << " in "         << dac.getModuleName()
     << " module "     << dac.getModuleId()
     << " for "        << dac.getTypeString();
  if ( dac.getCalibrationParameters() != NULL ) 
    os << endl << "Parameters" << endl << *dac.getCalibrationParameters();
  if ( dac.getCalibrationParametersCovariance() != NULL ) 
    os << endl << "Parameters' covariance" << endl << *dac.getCalibrationParametersCovariance();
  return os;
}

AFEB::teststand::DAC::DAC( const string& moduleId,
			   const string& moduleName,
			   const AFEB::teststand::DAC::Type_t type,
			   const int     socket    )
  : moduleId_  ( moduleId   )
  , moduleName_( moduleName )
  , type_      ( type       )
  , socket_    ( socket     )
  , calibrationParameters_          ( NULL )
  , calibrationParametersCovariance_( NULL ){
}

AFEB::teststand::DAC::DAC( const string& moduleId,
			   const string& moduleName,
			   const string& typeString,
			   const int     socket    )
  : moduleId_  ( moduleId                   )
  , moduleName_( moduleName                 )
  , type_      ( DAC::getType( typeString ) )
  , socket_    ( socket                     )
  , calibrationParameters_          ( NULL )
  , calibrationParametersCovariance_( NULL ){
}

AFEB::teststand::DAC::DAC( const DAC& other )  
  : moduleId_  ( other.getModuleId()   )
  , moduleName_( other.getModuleName() )
  , type_      ( other.getType()       )
  , socket_    ( other.getSocket()     )
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

  moduleId_   = rhs.getModuleId()  ;
  moduleName_ = rhs.getModuleName();
  type_       = rhs.getType()      ;
  socket_     = rhs.getSocket()   ;

  if ( rhs.getCalibrationParameters()           != NULL ) calibrationParameters_           = new TMatrixD   ( *rhs.getCalibrationParameters()           );
  if ( rhs.getCalibrationParametersCovariance() != NULL ) calibrationParametersCovariance_ = new TMatrixDSym( *rhs.getCalibrationParametersCovariance() );
  
  return *this;
}

bool 
AFEB::teststand::DAC::operator<( const DAC& rhs ) const {
  return ( ( moduleId_        +":"+moduleName_        +":"+typeStrings_[type_]+":"+utils::stringFrom<int>( socket_ )       )
	   <
	   ( rhs.getModuleId()+":"+rhs.getModuleName()+":"+rhs.getTypeString()+":"+utils::stringFrom<int>( rhs.getSocket() ) )
	   );
}

bool 
AFEB::teststand::DAC::operator==( const DAC& rhs ) const {
  if ( moduleId_   != rhs.getModuleId()   ) return false;
  if ( moduleName_ != rhs.getModuleName() ) return false;
  if ( type_       != rhs.getType()       ) return false;
  if ( socket_     != rhs.getSocket()     ) return false;
  return true;
}

void 
AFEB::teststand::DAC::setCalibrationParameters( const TMatrixD& parameters,
						const TMatrixDSym& covariance ){
  calibrationParameters_           = new TMatrixD   ( parameters );
  calibrationParametersCovariance_ = new TMatrixDSym( covariance );
}

pair<double,double> 
AFEB::teststand::DAC::mV_from_DACUnit( const double valueInDACUnits, 
				       const double errorInDACUnits ) const {
  TMatrixD Jacobian( DAC::nCalibrationParameters, 1 );
  Jacobian( DAC::intercept, 0 ) = 1.;
  Jacobian( DAC::slope    , 0 ) = valueInDACUnits;
  // Jacobian.Print();
  // calibrationParametersCovariance_->Print();
  // TMatrixD( *calibrationParametersCovariance_, 
  // 	    TMatrixD::kMult, 
  // 	    Jacobian ).Print();
  // TMatrixD( Jacobian, 
  // 	    TMatrixD::kTransposeMult, 
  // 	    TMatrixD( *calibrationParametersCovariance_, 
  // 		      TMatrixD::kMult, 
  // 		      Jacobian ) 
  // 	    ).Print();
  double varianceDueToParameters = TMatrixD( Jacobian, 
					     TMatrixD::kTransposeMult, 
					     TMatrixD( *calibrationParametersCovariance_, 
						       TMatrixD::kMult, 
						       Jacobian ) 
					     )( 0, 0 );
  double varianceDueToObservation = TMath::Power( (*calibrationParameters_)( DAC::slope, 0 ) * errorInDACUnits, 2 );
  return make_pair<double,double>( (*calibrationParameters_)( DAC::intercept, 0 ) + 
				   (*calibrationParameters_)( DAC::slope    , 0 ) * valueInDACUnits,
				   TMath::Sqrt( varianceDueToParameters + varianceDueToObservation ) );
}

// double AFEB::teststand::DAC::DACUnit_from_mV( const double valueInmV ) const {
//   if ( (*calibrationParameters_)( DAC::slope, 0 ) == double(0.) ) return 0.;
//   return ( valueInmV - (*calibrationParameters_)( DAC::intercept, 0 ) ) / (*calibrationParameters_)( DAC::slope, 0 ); 
// }

// static function
AFEB::teststand::DAC::Type_t 
AFEB::teststand::DAC::getType( const string& typeString ){
  AFEB::teststand::DAC::Type_t type = AFEB::teststand::DAC::nTypes;
  bool isValidType = false;
  for ( int i=0; i<AFEB::teststand::DAC::nTypes && !isValidType; ++i ){
    isValidType |= ( typeString.compare( typeStrings_[i] ) == 0 );
    if ( isValidType ) type = (AFEB::teststand::DAC::Type_t) i;
  }
  if ( !isValidType ){
    stringstream ss;
    ss << "\"" << typeString << "\" is not a valid DAC type.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return type;
}

// static function
string 
AFEB::teststand::DAC::getTypeString( const Type_t type ){
  if ( type < (AFEB::teststand::DAC::Type_t)0 || type >=AFEB::teststand::DAC::nTypes ){
    stringstream ss;
    ss << "'" << type << "' is not a valid DAC type.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return typeStrings_[type];
}

// static function
AFEB::teststand::DAC::Type_t 
AFEB::teststand::DAC::getType( const AFEB::teststand::Module::Type_t type ){
  switch( type ){
  case AFEB::teststand::Module::PulseGenerator:
    return pulse;
  case AFEB::teststand::Module::SignalConverter:
    return threshold;
  default:
    string moduleTypeString = AFEB::teststand::Module::getTypeString( type );
    stringstream ss;
    ss << "Modules of type " << moduleTypeString << " have no DACs.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
}
