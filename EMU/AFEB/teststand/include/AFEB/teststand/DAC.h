#ifndef __AFEB_teststand_DAC_h__
#define __AFEB_teststand_DAC_h__

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

      DAC( const string& moduleId,
	   const string& moduleName,
	   const string& type,
	   const int     channel );
      DAC( const DAC& other );
      ~DAC();
      DAC& operator=( const DAC& rhs );
      bool operator<( const DAC& rhs ) const;
      bool operator==( const DAC& rhs ) const;
      string getModuleid  () const { return moduleId_;   }  
      string getModuleName() const { return moduleName_; }
      string getType      () const { return type_;       }      
      int    getChannel   () const { return channel_;    }   
      const TMatrixD*    getCalibrationParameters()           const { return calibrationParameters_;           }
      const TMatrixDSym* getCalibrationParametersCovariance() const { return calibrationParametersCovariance_; }

      void setCalibrationParameters( const TMatrixD& parameters,
				     const TMatrixDSym& covariance );
      pair<double,double> toMilliVolts( const double valueInDACUnits, 
					const double errorInDACUnits ) const;

    private:
      string moduleId_;
      string moduleName_;
      string type_;
      int    channel_;

      TMatrixD    *calibrationParameters_;
      TMatrixDSym *calibrationParametersCovariance_;
    };
    
  }
}

#endif
