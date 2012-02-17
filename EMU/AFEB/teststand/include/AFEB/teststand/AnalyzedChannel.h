#ifndef __AFEB_teststand_AnalyzedChannel_h__
#define __AFEB_teststand_AnalyzedChannel_h__

#include "AFEB/teststand/fit/LeastSquaresFitter.h"
#include "AFEB/teststand/fit/StraightLine2D.h"

#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace AFEB::teststand;

namespace AFEB { namespace teststand {

    class AnalyzedChannel;
    ostream& operator<<( ostream& os, const AnalyzedChannel& d );

    class AnalyzedChannel{
    public:
      friend class AnalyzedDevice;
      friend ostream& operator<<( ostream& os, const AnalyzedChannel& d );
      void calculateGain();

    private:
      double gain_;	///< gain, i.e., the slope of the line fitted to V_setThreshold( Q_measuredThreshold )
      double offset_;	///< offset, i.e., the intercept of the line fitted to V_setThreshold( Q_measuredThreshold ))
      fit::LeastSquaresFitter<fit::StraightLine2D> fitter_; ///< straight-line fitter
    };

  }
}

#endif
