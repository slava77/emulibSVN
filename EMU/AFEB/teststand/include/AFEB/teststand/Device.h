#ifndef __AFEB_teststand_Device_h__
#define __AFEB_teststand_Device_h__

#include "AFEB/teststand/crate/Crate.h"

#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace AFEB::teststand;

namespace AFEB { namespace teststand {

    class Device;
    ostream& operator<<( ostream& os, const Device& d );

    class Device{
    public:
      friend ostream& operator<<( ostream& os, const Device& d );

      Device( const string type, const int nChannels );
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
    };

  }}

#endif
