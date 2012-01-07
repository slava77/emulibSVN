#ifndef __AFEB_teststand_Measurement_h__
#define __AFEB_teststand_Measurement_h__

#include "AFEB/teststand/utils/STL.h"
#include "AFEB/teststand/crate/Crate.h"
#include "AFEB/teststand/TestedDevice.h"
#include "AFEB/teststand/Results.h"

#include "toolbox/BSem.h"

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
      enum Type_t { count_vs_dac, time_vs_dac, dummy, nTypes };
      enum Status_t { waiting, running, done, nStatus };
      enum Injection_t { common, individual };

      friend ostream& operator<<( ostream& os, const Measurement& m );

      Measurement( const int position, const int index, const string name, const string type, const string resultDir );
      ~Measurement();
      void setPulseParameters( const vector< pair<string,string> >& param );
      void setThresholdParameters( const vector< pair<string,string> >& param );
      void setTDCParameters( const vector< pair<string,string> >& param );
      void addTestedDevice( TestedDevice* device );
      int getPosition() const { return position_; }
      int getIndex() const { return index_; }
      string getName() const { return name_; }
      string getType() const { return type_; }
      Type_t getTypeType() const { return type_t_; }
      string getResultDir() const { return resultDir_; }
      const TestedDevice* getTestedDevice( const int tdcInput ) const;
      const map<TestedDevice*,Results*> getResults() const { return results_; }
      const set<TestedDevice*> getTestedDevices() const { return utils::keys( results_ ); }
      int getTDCSlot() const;
      int getPulseGeneratorSlot() const { return pulseGeneratorSlot_; }
      string getPulsedCapacitor() const { return pulsedCapacitor_; }
      int getAmplitudeMin () const { return amplitudeMin_; }
      int getAmplitudeMax () const { return amplitudeMax_; }
      int getAmplitudeStep() const { return amplitudeStep_;}
      int getNPulses() const { return nPulses_; }
      int getSetThreshold() const { return thresholdValue_; }
      int getTDCTimeMin() const { return tdcTimeMin_; }
      int getTDCTimeMax() const { return tdcTimeMax_; }
      string getStatus() const { return status_[status_t_]; }
      bool execute();
      void abort(){ bsem_.take(); isToKeepRunning_ = false; bsem_.give(); }

    private:
      toolbox::BSem bsem_;	///< Binary semaphore.
      static const char* const types_[nTypes];
      static const char* const status_[nStatus];
      int position_; ///< The position of the corresponding c:measurement element in the configuration XML.
      int index_; ///< The index of this measurement among the selected (c:enabled="yes") measurements.
      string name_;
      string type_;
      Type_t type_t_;
      Status_t status_t_;
      string resultDir_; ///< The full path to the directory to save the results in.
      bool isToKeepRunning_;
      map<TestedDevice*,Results*> results_;

      int amplitudeMin_;
      int amplitudeMax_;
      int amplitudeStep_;
      int nPulses_;
      int pulseGeneratorSlot_; ///< The slot of the pulse generator to the exteranl capacitor. When the internal capacitor is pulsed, it's set equal to -1 as it is then each device's signal converter that generates the pulses for it.
      string pulsedCapacitor_;
      Injection_t injection_;

      int thresholdValue_;

      int tdcTimeMin_;
      int tdcTimeMax_;

      bool countVsDAQ();
      // bool countVsTime(); // Will not be implemented, being essentially the same as countVsDAQ with only the parameters different.
      bool dummyResultGenerator();
      TestedDevice* findTestedDevice( const int tdcInput );
      Results* findResults( const int tdcInput );
    };

  }}

#endif
