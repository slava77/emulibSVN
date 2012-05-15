#ifndef __AFEB_teststand_AnalyzedDevice_h__
#define __AFEB_teststand_AnalyzedDevice_h__

#include "AFEB/teststand/TestedDevice.h"
#include "AFEB/teststand/AnalyzedChannel.h"
#include "AFEB/teststand/DAC.h"
#include "AFEB/teststand/Measurement.h"
#include "AFEB/teststand/Cut.h"

#include "TPDF.h"

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

      void setMeasurementDate      ( const string& value ){ measurementDate_       = value; }
      void setAdaptorId            ( const string& value ){ adaptorId_             = value; }
      void setAdaptorType          ( const string& value ){ adaptorType_           = value; }
      void setCorrectionCoefficient( const double  value ){ correctionCoefficient_ = value; }
      void setInjectionCapacitance ( const double  value ){ injectionCapacitance_  = value; }
      void setPulseDivisionFactor  ( const double  value ){ pulseDivisionFactor_   = value; }
      void setPulseDAC    ( const DAC* dac ){ pulseDAC_     = dac; }
      void setThresholdDAC( const DAC* dac ){ thresholdDAC_ = dac; }

      /// Converts voltage [mV] to charge [fC].
      ///
      /// @param voltage Voltage [mV]
      /// @param capacitorType Type of pulsed capacitor (internal or external)
      ///
      /// @return Charge [fC]
      ///
      double chargeFromVoltage( const double voltage, Measurement::Capacitor_t capacitorType ) const;

      /// Converts voltage [mV] to charge [fC].
      ///
      /// @param voltage Voltage [mV] value and error
      /// @param capacitorType Type of pulsed capacitor (internal or external)
      ///
      /// @return Charge [fC] value and error
      ///
      pair<double,double> chargeFromVoltage( const pair<double,double> voltage, Measurement::Capacitor_t capacitorType ) const ;

      void calculateGains               ();
      void calculateInternalCapacitances();
      void saveResults                  ( const string& afebRootDir, const string& analyzedResultsDir );
      bool passesSelectionCuts          ( const string& analyzedResultsDir, const vector<Cut>& cuts );
      

    private:
      string                     rawResultsDir_;
      vector<Measurement*>       measurements_;	         ///< Measurements preformed on this device.
      vector<AnalyzedChannel>    channels_;	         ///< Channels with analysis data and results.
      string                     measurementDate_;       ///< The date and time of the measurement analyzed here.
      string                     adaptorId_;             ///< The id of the adaptor this device is plugged into.
      string                     adaptorType_;           ///< The type of the adaptor this device is plugged into.
      double                     correctionCoefficient_; ///< The ratio of the measured threshold to the threshold measured when plugged into a reference adaptor socket.
      double                     injectionCapacitance_;  ///< Injection capacitance [pF] of the adaptor in use.
      double                     pulseDivisionFactor_;   ///< Pulse division factor of the adaptor in use.
      const DAC                 *pulseDAC_;              ///< The DAC that pulses this device.
      const DAC                 *thresholdDAC_;          ///< The DAC that sets the threshold for this device.
      static const string        analyzedDeviceNamespace_;      ///< the namespace to be used in the analyzed result XML file
      static const double        nominalInputCharges_[];        ///< The nominal input charge values [fC] at which the mean and RMS of times are to be reported. The actual values may be slightly different due to the finite pulse amplitude step size.
      static const double        nominalInputChargeRangeStart_; ///< The lower end of the input charge range [fC] for the slew time calculation.
      static const double        nominalInputChargeRangeEnd_;   ///< The upper end of the input charge range [fC] for the slew time calculation.

      void addThresholdMeasurement( const int iChannel,
				    const pair<double,double> V_setThreshold ,
				    const pair<double,double> Q_measuredThreshold,
				    const pair<double,double> Q_measuredNoise );
      // valarray<double> getThresholds() const;
      valarray<double> getNoises() const;
      valarray<double> getGains() const;
      valarray<double> getOffsets() const;
      valarray<double> getInternalCapacitances() const;
      TH1D histogramContents( const TH1D* h ) const;
      string measurementsToXMLAndPlots( const string& analyzedResultsDir, TPDF& pdf );
      string statisticsToXML( const string& name, const valarray<double>& values ) const;
      double getMaxMeasuredThreshold( const double setThreshold );
    };

  }
}

#endif
