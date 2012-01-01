#ifndef __AFEB_teststand_Configuration_h__
#define __AFEB_teststand_Configuration_h__

#include "AFEB/teststand/Crate.h"
#include "AFEB/teststand/Measurement.h"
#include "AFEB/teststand/Calibration.h"

using namespace std;
using namespace AFEB::teststand;

namespace AFEB { namespace teststand {

    class Configuration{

    public:
      Configuration( const string XML, const string resultDir );
      ~Configuration();
      const Crate* getCrate() const { return crate_; }
      const vector<Measurement*>& getMeasurements() const { return measurements_; }
      Calibration* getCalibration() const { return calibration_; }
      string getResultDir() const { return resultDir_; }
      string resultsXML();
    private:
      string xml_; ///< The configuration XML.
      string resultDir_; ///< The full path to the directory to save the results in.
      Crate* crate_; ///< The crate setup. It's adopted by all measurements of this configuration.
      vector<TestedDevice*> testedDevices_; ///< The tested devices. It's adopted by all measurements of this configuration.
      vector<Measurement*> measurements_; ///< The selected measurements of this configuration.
      Calibration* calibration_;
      void createCrate( bool forDummyMeasurements );
      void createMeasurements();
      void createCalibration();
      Measurement* findMeasurement( const int position, const int tdcSlot ) const;
    };

}}

#endif
