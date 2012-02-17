#ifndef __AFEB_teststand_DAC_h__
#define __AFEB_teststand_DAC_h__

#include "AFEB/teststand/crate/Module.h"

#include "TMatrixD.h"
#include "TMatrixDSym.h"

#include <string>
#include <iostream>

using namespace std;

namespace AFEB { namespace teststand {

    class DAC;
    ostream& operator<<( ostream& os, const DAC& dac );

    class DAC{

      friend ostream& operator<<( ostream& os, const DAC& dac );

    public:
      enum CalibrationParameter_t { intercept, slope, nCalibrationParameters };

      enum Type_t { pulse, threshold, nTypes };

      DAC( const string& moduleId,
      	   const string& moduleName,
      	   const AFEB::teststand::DAC::Type_t type,
      	   const int     socket );
      DAC( const string& moduleId,
      	   const string& moduleName,
      	   const string& typeString,
      	   const int     socket );
      DAC( const DAC& other );
      ~DAC();
      DAC& operator=( const DAC& rhs );
      bool operator<( const DAC& rhs ) const;
      bool operator==( const DAC& rhs ) const;
      string getModuleId  () const { return moduleId_;           }  
      string getModuleName() const { return moduleName_;         }
      Type_t getType      () const { return type_;               }      
      string getTypeString() const { return typeStrings_[type_]; }      
      int    getSocket    () const { return socket_;             }   
      static Type_t getType      ( const string& typeString );
      static string getTypeString( const Type_t type );      
      static Type_t getType      ( const AFEB::teststand::Module::Type_t type );      
      const TMatrixD*    getCalibrationParameters()           const { return calibrationParameters_;           }
      const TMatrixDSym* getCalibrationParametersCovariance() const { return calibrationParametersCovariance_; }

      void setCalibrationParameters( const TMatrixD& parameters,
				     const TMatrixDSym& covariance );

      /// Convert DAC setting's value and error to voltage [mV]
      ///
      /// @param valueInDACUnits Value in DAC units.
      /// @param errorInDACUnits Error in DAC units.
      ///
      /// @return Value and error in milliVolts.
      ///
      pair<double,double> mV_from_DACUnit( const double valueInDACUnits, 
					   const double errorInDACUnits=0. ) const;

    private:
      string moduleId_;
      string moduleName_;
      Type_t type_;
      int    socket_;

      TMatrixD    *calibrationParameters_;
      TMatrixDSym *calibrationParametersCovariance_;
      static const char* const typeStrings_[nTypes];
    };
    
  }
}

#endif
