#ifndef __AFEB_teststand_Measurement_h__
#define __AFEB_teststand_Measurement_h__

#include "AFEB/teststand/Crate.h"
#include "AFEB/teststand/TestedDevice.h"

#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace AFEB::teststand;

namespace AFEB { namespace teststand {

    class Measurement;
    ostream& operator<<( ostream& os, const Measurement& m );

    class Measurement{
    public:
      enum Type_t { count_vs_dac, time_vs_dac, nTypes };

      friend ostream& operator<<( ostream& os, const Measurement& m );

      Measurement( const string name, const string type );
      void setPulseParameters( const vector< pair<string,string> >& param );
      void setThresholdParameters( const vector< pair<string,string> >& param );
      void setTDCParameters( const vector< pair<string,string> >& param );
      void addTestedDevice( TestedDevice* device );
      string getType() const { return type_; }
      const TestedDevice* findTestedDevice( const int tdcInput ) const;
      int getTDCSlot() const;
      void execute();

    private:
      static const char* const types_[nTypes];
      string name_;
      string type_;
      Type_t type_t_;
      vector<TestedDevice*> testedDevices_;

      int amplitudeMin_;
      int amplitudeMax_;
      int amplitudeStep_;
      int nPulses_;
      int pulseGeneratorSlot_;

      int thresholdValue_;

      int tdcTimeMin_;
      int tdcTimeMax_;

      void countVsDAQ();
      void countVsTime();
    };

  }}

#endif
