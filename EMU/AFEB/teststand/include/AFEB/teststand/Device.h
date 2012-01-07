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
      int getSignalConverterInput() const { return signalConverterInput_; }
      int getPulseGeneratorSlot() const { return pulseGeneratorSlot_; }
      int getPulseGeneratorOutput() const { return pulseGeneratorOutput_; }
      int getTDCInput() const { return tdcInput_; }
      string getId() const { return id_; }
      string getType() const { return type_; }
      int getNChannels() const { return nChannels_; }
    protected:
      string type_;
      int nChannels_;
      string id_;
      int socket_;
      int tdcSlot_;
      int tdcInput_;
      int signalConverterSlot_;
      int signalConverterInput_;
      int pulseGeneratorSlot_;
      int pulseGeneratorOutput_;
    };

  }}

#endif
