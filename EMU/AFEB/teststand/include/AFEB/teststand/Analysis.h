#ifndef __AFEB_teststand_Analysis_h__
#define __AFEB_teststand_Analysis_h__
#include "AFEB/teststand/Configuration.h"
#include "AFEB/teststand/DAC.h"
#include "AFEB/teststand/AnalyzedDevice.h"
#include "AFEB/teststand/Cut.h"

#include <string>
#include <vector>

using namespace std;

namespace AFEB { namespace teststand {

    class Analysis{
    public:
      Analysis( const string& AFEBRootDir, const string& resultsDir );
      ~Analysis();

      /// 
      /// Save results in the standard analyzed results directory (<resultsDir>/analyzed)
      ///
      void saveResults();

      /// Save results in the specified directory instead of the standard one.
      ///
      /// @param destinationDir Directory to save the analyzed results in.
      ///
      void saveResults( const string& destinationDir );

      /// 
      /// Apply selection cuts in the standard analyzed results directory (<resultsDir>/analyzed)
      ///
      void applySelection();

      /// Apply selection cuts in the specified directory instead of the standard one.
      ///
      /// @param destinationDir Directory to save the analyzed results in.
      ///
      void applySelection( const string& analyzedResultsDirectory );

    private:
      void collectAnalyzedDevices( const string& calibXML, const string& configXML );
      void calibrateDACs( const string& calibXML, const string& configXML );
      void assignDACs();
      void calculateGain();
      void calculateInternalCapacitance();
      vector<Cut> loadSelectionCuts( const string& dir ) const;
      string createSelectionCutsXSLT( const vector<Cut>& cuts ) const;

      string afebRootDir_;
      string rawResultsDir_;
      string analyzedResultsDir_;
      string rawResultXML_;
      Configuration *configuration_;
      vector<DAC> DACs_;
      vector<AnalyzedDevice> analyzedDevices_;
    };
    
  }
}

#endif
