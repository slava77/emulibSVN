#ifndef __AFEB_teststand_CrateController_h__
#define __AFEB_teststand_CrateController_h__

#include "AFEB/teststand/Module.h"

#include <string>

using namespace std;

namespace AFEB { namespace teststand {

    class CrateController : public Module
    {      
    public:
      CrateController( const string name, const string type, const int branch, const int crateNumber )
	: Module( name, type ),
	  crateNumber_( crateNumber ){}
      int getCrateNumber() const { return crateNumber_; }
    private:
      int crateNumber_;
    };

  }}

#endif
