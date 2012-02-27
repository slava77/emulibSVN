#ifndef __AFEB_teststand_AnalyzedDevice_h__
#define __AFEB_teststand_AnalyzedDevice_h__

#include "AFEB/teststand/TestedDevice.h"
#include "AFEB/teststand/AnalyzedChannel.h"
#include "AFEB/teststand/DAC.h"
#include "AFEB/teststand/Measurement.h"

#include <valarray>
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
      AnalyzedDevice( const TestedDevice& device, const string& rawResultsDir, const vector<Measurement*>& measurements );

      void addThresholdMeasurement( const int iChannel,
				    const pair<double,double> V_setThreshold ,
				    const pair<double,double> V_measuredThreshold,
				    const pair<double,double> V_measuredNoise );

      /// Get the pulse DAC descriptor for this device.
      /// It's to be used for identifying the DAC object that already contains the calibration.
      ///
      /// @return A DAC object that does not contain the calibration.
      ///
      DAC  getPulseDACDescriptor() const;

      /// Get the threshold DAC descriptor for this device. 
      /// It's to be used for identifying the DAC object that already contains the calibration.
      ///
      /// @return A DAC object that does not contain the calibration.
      ///
      DAC  getThresholdDACDescriptor() const;

      void setAdaptorName          ( const string& value ){ adaptorName_           = value; }
      void setCorrectionCoefficient( const double  value ){ correctionCoefficient_ = value; }
      void setInjectionCapacitance ( const double  value ){ injectionCapacitance_  = value; }
      void setPulseDivisionFactor  ( const double  value ){ pulseDivisionFactor_   = value; }
      void setPulseDAC    ( const DAC* dac ){ pulseDAC_     = dac; }
      void setThresholdDAC( const DAC* dac ){ thresholdDAC_ = dac; }

      /// Converts voltage [mV] to charge [fC] taking into account the correction coefficient for the adapter socket, too.
      ///
      /// @param voltage Voltage [mV]
      ///
      /// @return Charge [fC]
      ///
      double chargeFromVoltage( const double voltage ) const { return injectionCapacitance_ * voltage / pulseDivisionFactor_ / correctionCoefficient_; }

      void calculateGains               ();
      void calculateInternalCapacitances();
      void saveResults                  ( const string& analyzedResultsDir ) const;

    private:
      string                     rawResultsDir_;
      vector<Measurement*>       measurements_;	         ///< Measurements preformed on this device.
      vector<AnalyzedChannel>    channels_;	         ///< Channels with analysis data and results.
      string                     adaptorName_;           ///< The name of the adaptor this device is plugged into.
      double                     correctionCoefficient_; ///< The ratio of the measured threshold to the threshold measured when plugged into a reference adaptor socket.
      double                     injectionCapacitance_;  ///< Injection capacitance [pF] of the adaptor in use.
      double                     pulseDivisionFactor_;   ///< Pulse division factor of the adaptor in use.
      const DAC                 *pulseDAC_;              ///< The DAC that pulses this device.
      const DAC                 *thresholdDAC_;          ///< The DAC that sets the threshold for this device.
      static const string        analyzedDeviceNamespace_; ///< the namespace to be used in the analyzed result XML file

      // valarray<double> getThresholds() const;
      valarray<double> getNoises() const;
      valarray<double> getGains() const;
      valarray<double> getOffsets() const;
      valarray<double> getInternalCapacitances() const;
      string measurementsToXML() const;
      string statisticsToXML( const string& name, const valarray<double>& values ) const;
    };

  }
}

#endif
