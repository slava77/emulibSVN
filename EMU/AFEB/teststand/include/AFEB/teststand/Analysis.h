#ifndef __AFEB_teststand_Analysis_h__
#define __AFEB_teststand_Analysis_h__
#include "AFEB/teststand/Configuration.h"

#include <string>

using namespace std;

namespace AFEB { namespace teststand {

    class Analysis{
    public:
      Analysis( const string& resultsDir );
      ~Analysis();

    private:
      void calculateGain();
      string rawResultXML_;
      Configuration *configuration_;
    };
    
  }
}

#endif
