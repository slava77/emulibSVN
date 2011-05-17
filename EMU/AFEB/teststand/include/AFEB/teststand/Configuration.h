#ifndef __AFEB_teststand_Configuration_h__
#define __AFEB_teststand_Configuration_h__

#include "AFEB/teststand/Crate.h"

using namespace std;

namespace AFEB { namespace teststand {

    class Configuration{

    public:
      Configuration( const string XML );
      ~Configuration();
      Crate getCrate();
    private:
      string xml_;
      vector<AFEB::teststand::Module*> modules_; ///< For bookkeeping so that we can destroy them at the end.
    };

}}

#endif
