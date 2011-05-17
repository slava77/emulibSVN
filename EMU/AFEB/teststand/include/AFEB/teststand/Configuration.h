#ifndef __AFEB_teststand_Configuration_h__
#define __AFEB_teststand_Configuration_h__

#include "AFEB/teststand/Crate.h"

using namespace std;

namespace AFEB { namespace teststand {

    class Configuration{

    public:
      Configuration( const string XML );
      Crate getCrate() const;
    private:
      string xml_;

    };

}}

#endif
