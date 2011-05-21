#ifndef __AFEB_teststand_Configuration_h__
#define __AFEB_teststand_Configuration_h__

#include "AFEB/teststand/Crate.h"
#include "AFEB/teststand/Measurement.h"

using namespace std;
using namespace AFEB::teststand;

namespace AFEB { namespace teststand {

    class Configuration{

    public:
      Configuration( const string XML );
      ~Configuration();
      const Crate* getCrate() const { return crate_; }
      const vector<Measurement*>& getMeasurements() const { return measurements_; }
    private:
      string xml_; ///< The configuration XML.
      Crate* crate_; ///< The crate setup. It's adopted by all measurements of this configuration.
      vector<TestedDevice*> testedDevices_; ///< The tested devices. It's adopted by all measurements of this configuration.
      vector<Measurement*> measurements_; ///< The selected measurements of this configuration.
      void createCrate();
      void createMeasurements();
      Measurement* findMeasurement( const string type, const int tdcSlot ) const;
    };

}}

#endif
