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

    class TestedDevice : public Device {
    public:
      friend ostream& operator<<( ostream& os, const TestedDevice& d );
      TestedDevice( const string type, const int nChannels, Crate* const crate );
      Crate* getCrate() const { return crate_; }
    private:
      Crate* const crate_;
    };

  }}

#endif
