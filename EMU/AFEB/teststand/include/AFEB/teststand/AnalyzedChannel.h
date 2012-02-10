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
      friend ostream& operator<<( ostream& os, const AnalyzedChannel& d );

    private:
      double gain_;	///< gain
      fit::LeastSquaresFitter<fit::StraightLine2D> fitter_; ///< straight-line fitter
    };

  }
}

#endif
