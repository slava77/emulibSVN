#ifndef __AFEB_teststand_Analysis_h__
#define __AFEB_teststand_Analysis_h__
#include "AFEB/teststand/Configuration.h"
#include "AFEB/teststand/DAC.h"
#include "AFEB/teststand/AnalyzedDevice.h"

#include <string>
#include <vector>

using namespace std;

namespace AFEB { namespace teststand {

    class Analysis{
    public:
      Analysis( const string& resultsDir );
      ~Analysis();

    private:
      void collectAnalyzedDevices( const string& configXML );
      void calibrateDACs( const string& configXML );
      void calculateGain();
      void calculateInternalCapacitance();
      void saveResults();

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
