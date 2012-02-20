#ifndef __AFEB_teststand_AnalyzedDevice_h__
#define __AFEB_teststand_AnalyzedDevice_h__

#include "AFEB/teststand/TestedDevice.h"
#include "AFEB/teststand/AnalyzedChannel.h"

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

      void addThresholdMeasurement( const int iChannel,
				    const pair<double,double> V_setThreshold ,
				    const pair<double,double> V_measuredThreshold,
				    const pair<double,double> V_measuredNoise );

      double getCorrectionCoefficient() const { return correctionCoefficient_; }
      double getInjectionCapacitance () const { return injectionCapacitance_;  }
      double getPulseDivisionFactor  () const { return pulseDivisionFactor_;   }
      void setCorrectionCoefficient( const double value ){ correctionCoefficient_ = value; }
      void setInjectionCapacitance ( const double value ){ injectionCapacitance_  = value; }
      void setPulseDivisionFactor  ( const double value ){ pulseDivisionFactor_   = value; }

      void calculateGains();

    private:
      vector<AnalyzedChannel> channels_;	      ///< Channels with analysis data and results.
      double                  correctionCoefficient_; ///< The ratio of the measured threshold to the threshold measured when plugged into a reference adaptor socket.
      double                  injectionCapacitance_;  ///< Injection capacitance [pF] of the adaptor in use.
      double                  pulseDivisionFactor_;   ///< Pulse division factor of the adaptor in use.
    };

  }
}

#endif
