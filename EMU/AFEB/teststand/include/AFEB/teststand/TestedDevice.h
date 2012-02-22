#ifndef __AFEB_teststand_TestedDevice_h__
#define __AFEB_teststand_TestedDevice_h__

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
      int getAdaptorSocket() const { return socket_; }
      int getNChannels() const { return nChannels_; }
      Crate* getCrate() const { return crate_; }
    protected:
      string type_;		///< The device type.
      int nChannels_;		///< The number of channels in this device.
      string id_;		///< The unique id of this device.
      int socket_;		///< The adaptor socket this device is plugged into.
      int tdcSlot_;		///< The crate slot the TDC reading out this device is inserted into.
      int tdcSocket_;		///< The TDC's input socket used for this device.
      int signalConverterSlot_;	///< The crate slot the signal converter reading out this device is inserted into.
      int signalConverterSocket_; ///< The signal converter's input socket the cable from this device is plugged into.
      int pulseGeneratorSlot_;	///< The crate slot the pulse generator used for this device is inserted into.
      int pulseGeneratorSocket_; ///< The pulse generator's output socket used for this device.
      Crate* crate_;
    };

  }}

#endif
