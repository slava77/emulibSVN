#ifndef __AFEB_teststand_TestedDevice_h__
#define __AFEB_teststand_TestedDevice_h__

#include "AFEB/teststand/Crate.h"

#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace AFEB::teststand;

namespace AFEB { namespace teststand {

    class TestedDevice{
    public:
      TestedDevice( const string type, const int nChannels, Crate* const crate );
      void setParameters( const vector< pair<string,string> >& param );
      int getTDCSlot() const { return tdcSlot_; }
      int getTDCInput() const { return tdcInput_; }
      string getId() const { return id_; }
    private:
      string type_;
      int nChannels_;
      Crate* const crate_;
      string id_;
      int socket_;
      int tdcSlot_;
      int tdcInput_;
    };

  }}

#endif
