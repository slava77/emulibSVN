#ifndef __AFEB_teststand_Calibration_h__
#define __AFEB_teststand_Calibration_h__

#include "AFEB/teststand/crate/LE32.h"

#include "toolbox/BSem.h"

#include <vector>
#include <map>
#include <iostream>

using namespace std;

namespace AFEB { namespace teststand {

    class Calibration{
    public:      
      friend ostream& operator<<( ostream& os, const Calibration& c );
      Calibration();
      void addModule( Module* module );
      int getThresholdLevel() const { return thresholdLevel_; }
      int getPulseAmplitude() const { return pulseAmplitude_; }
      void setThresholdLevel( const int thresholdLevel ){ bsem_.take(); thresholdLevel_ = thresholdLevel; bsem_.give(); }
      void setPulseAmplitude( const int pulseAmplitude ){ bsem_.take(); pulseAmplitude_ = pulseAmplitude; bsem_.give(); }
      void setDACValues( const map<string,string> DACValues );
      bool execute();
      void abort(){ bsem_.take(); isToKeepRunning_ = false; initialized_ = false; bsem_.give(); }
      
    private:
      toolbox::BSem bsem_;	///< Binary semaphore.
      vector<Module*> modules_; ///< Modules with DAC to calibrate.
      int thresholdLevel_;	///< Threshold level in DAC units.
      int pulseAmplitude_;	///< Test pulse amplitude in DAC units.
      bool initialized_;	///< Whether or not the crate has been initialized already.
      bool isToKeepRunning_;
    };

    ostream& operator<<( ostream& os, const Calibration& m );
  }
}

#endif
