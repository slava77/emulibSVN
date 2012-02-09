#ifndef __AFEB_teststand_TestedDevice_h__
#define __AFEB_teststand_TestedDevice_h__

#include "AFEB/teststand/Device.h"
#include "AFEB/teststand/crate/Crate.h"

#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace AFEB::teststand;

namespace AFEB { namespace teststand {

    class TestedDevice;
    ostream& operator<<( ostream& os, const TestedDevice& d );

    class TestedDevice {
    public:
      friend ostream& operator<<( ostream& os, const TestedDevice& d );
      TestedDevice( const string type, const int nChannels, Crate* const crate );
      void setParameters( const vector< pair<string,string> >& param );
      int getTDCSlot() const { return tdcSlot_; }
      int getSignalConverterSlot() const { return signalConverterSlot_; }
      int getSignalConverterSocket() const { return signalConverterSocket_; }
      int getPulseGeneratorSlot() const { return pulseGeneratorSlot_; }
      int getPulseGeneratorSocket() const { return pulseGeneratorSocket_; }
      int getTDCSocket() const { return tdcSocket_; }
      string getId() const { return id_; }
      string getType() const { return type_; }
      int getNChannels() const { return nChannels_; }
      Crate* getCrate() const { return crate_; }
    protected:
      string type_;
      int nChannels_;
      string id_;
      int socket_;
      int tdcSlot_;
      int tdcSocket_;
      int signalConverterSlot_;
      int signalConverterSocket_;
      int pulseGeneratorSlot_;
      int pulseGeneratorSocket_;
      Crate* crate_;
    };

  }}

#endif
