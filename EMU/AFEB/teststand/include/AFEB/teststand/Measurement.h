#ifndef __AFEB_teststand_Measurement_h__
#define __AFEB_teststand_Measurement_h__

#include "AFEB/teststand/Crate.h"

#include <string>
#include <vector>
#include <map>

using namespace std;

namespace AFEB { namespace teststand {

    class Measurement{
    public:
      enum Type_t { count_vs_dac, time_vs_dac, nTypes };
      Measurement( const string name, const string type, const AFEB::teststand::Crate* const crate );
      void setPulseParameters( const vector< pair<string,string> >& param );
      void setThresholdParameters( const vector< pair<string,string> >& param );
      void setTDCParameters( const vector< pair<string,string> >& param );
    private:
      static const char* const types_[nTypes];
      string name_;
      string type_;
      const AFEB::teststand::Crate* const crate_;

      int amplitudeMin_;
      int amplitudeMax_;
      int amplitudeStep_;
      int nPulses_;
      int pulseGeneratorSlot_;

      int thresholdValue_;

      int tdcTimeMin_;
      int tdcTimeMax_;

      map<string,int> testedDeviceId_;
    };

  }}

#endif
