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
      const vector<Measurement*> getMeasurements() const { return measurements_; }
    private:
      string xml_;
      Crate* crate_;
      vector<Measurement*> measurements_;
      //vector<AFEB::teststand::Module*> modules_; ///< For bookkeeping so that we can destroy them at the end.
      void createCrate();
      void createMeasurements();
    };

}}

#endif
