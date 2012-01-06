#ifndef __AFEB_teststand_Analysis_h__
#define __AFEB_teststand_Analysis_h__
#include "AFEB/teststand/Configuration.h"
#include "AFEB/teststand/DAC.h"

#include <string>
#include <vector>

using namespace std;

namespace AFEB { namespace teststand {

    class Analysis{
    public:
      Analysis( const string& resultsDir );
      ~Analysis();

    private:
      void calibrateDACs( const string& configXML );
      void calculateGain();
      string rawResultXML_;
      Configuration *configuration_;
      vector<DAC> DACs_;
    };
    
  }
}

#endif
