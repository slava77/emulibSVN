#ifndef __AFEB_teststand_AnalyzedChannel_h__
#define __AFEB_teststand_AnalyzedChannel_h__

#include "AFEB/teststand/fit/LeastSquaresFitter.h"
#include "AFEB/teststand/fit/Polynomial.h"

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
      double noise_;		///< noise [fC], i.e., the width of the efficiency S-curve
      double noiseError_;	///< error on noise [fC]
      double gain_;		///< gain [mV/fC], i.e., the slope of the line fitted to V_setThreshold( Q_measuredThreshold )
      double gainError_;	///< error on gain [mV/fC]
      double offset_;		///< offset [mV], i.e., the intercept of the line fitted to V_setThreshold( Q_measuredThreshold ))
      double offsetError_;	///< error on offset [mV]
      fit::LeastSquaresFitter< fit::Polynomial<1> > QofVfitter_;	///< straight-line fitter to get offset and gain from Q_measuredThreshold( V_setThreshold )
      fit::LeastSquaresFitter< fit::Polynomial<0> > noiseAverager_;	///< constant function fitter to get average of noise measurements
    };

  }
}

#endif
