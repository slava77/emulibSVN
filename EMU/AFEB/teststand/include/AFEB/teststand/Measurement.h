#ifndef __AFEB_teststand_Measurement_h__
#define __AFEB_teststand_Measurement_h__

#include "AFEB/teststand/Crate.h"
#include "AFEB/teststand/TestedDevice.h"
#include "AFEB/teststand/Results.h"

#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace AFEB::teststand;

namespace AFEB { namespace teststand {

    class Measurement;
    ostream& operator<<( ostream& os, const Measurement& m );

    class Results;

    class Measurement{
    public:
      enum Type_t { count_vs_dac, time_vs_dac, nTypes };
      enum Status_t { waiting, running, done, nStatus };

      friend ostream& operator<<( ostream& os, const Measurement& m );

      Measurement( const int index, const string name, const string type, const string resultDir );
      ~Measurement();
      void setPulseParameters( const vector< pair<string,string> >& param );
      void setThresholdParameters( const vector< pair<string,string> >& param );
      void setTDCParameters( const vector< pair<string,string> >& param );
      void addTestedDevice( TestedDevice* device );
      int getIndex() const { return index_; }
      string getName() const { return name_; }
      string getType() const { return type_; }
      string getResultDir() const { return resultDir_; }
      const TestedDevice* getTestedDevice( const int tdcInput ) const;
      const map<TestedDevice*,Results*> getResults() const { return results_; }
      int getTDCSlot() const;
      int getAmplitudeMin () const { return amplitudeMin_; }
      int getAmplitudeMax () const { return amplitudeMax_; }
      int getAmplitudeStep() const { return amplitudeStep_;}
      int getNPulses() const { return nPulses_; }
      string getStatus() const { return status_[status_t_]; }
      bool execute();
      void abort() const { isToKeepRunning_ = false; }

    private:
      static const char* const types_[nTypes];
      static const char* const status_[nStatus];
      int index_;
      string name_;
      string type_;
      Type_t type_t_;
      Status_t status_t_;
      string resultDir_; ///< The full path to the directory to save the results in.
      mutable bool isToKeepRunning_;
      map<TestedDevice*,Results*> results_;

      int amplitudeMin_;
      int amplitudeMax_;
      int amplitudeStep_;
      int nPulses_;
      int pulseGeneratorSlot_;

      int thresholdValue_;

      int tdcTimeMin_;
      int tdcTimeMax_;

      bool countVsDAQ();
      bool countVsTime();
      bool dummyResultGenerator();
      TestedDevice* findTestedDevice( const int tdcInput );
      Results* findResults( const int tdcInput );
    };

  }}

#endif
