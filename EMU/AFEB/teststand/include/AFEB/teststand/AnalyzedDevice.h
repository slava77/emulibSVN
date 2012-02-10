#ifndef __AFEB_teststand_AnalyzedDevice_h__
#define __AFEB_teststand_AnalyzedDevice_h__

#include "AFEB/teststand/TestedDevice.h"
#include "AFEB/teststand/AnalyzedChannel.h"
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

      /// CTOR from TestedDevice
      ///
      /// @param device Tested device to be used as a base for this analyzed device.
      ///
      AnalyzedDevice( const TestedDevice& device );

      /// Copy CTOR needed for STL containers
      ///
      /// @param other Analyzed device to be copied.
      ///
      AnalyzedDevice( const AnalyzedDevice& other );

    private:
      vector<AnalyzedChannel> channels_;	///< Channels with analysis data and results.
    };

  }
}

#endif
