#ifndef __AFEB_teststand_AnalyzedDevice_h__
#define __AFEB_teststand_AnalyzedDevice_h__

#include "AFEB/teststand/TestedDevice.h"
#include "AFEB/teststand/fit/LeastSquaresFitter.h"
#include "AFEB/teststand/fit/StraightLine2D.h"

#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace AFEB::teststand;

namespace AFEB { namespace teststand {

    class AnalyzedDevice;
    ostream& operator<<( ostream& os, const AnalyzedDevice& d );

    class AnalyzedDevice : public TestedDevice {
    public:
      friend ostream& operator<<( ostream& os, const AnalyzedDevice& d );
      AnalyzedDevice( const TestedDevice& device );
      // fit::LeastSquaresFitter<fit::StraightLine2D>* getFitter( const int channel ){ return fitters_[channel]; }
      vector< fit::LeastSquaresFitter<fit::StraightLine2D>* > fitters_;	///< straight-line fitter[channel]

    private:
      void resizeContainers();
      vector<double> gains_;	///< gain[channel]
    };

  }
}

#endif
