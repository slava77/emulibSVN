#ifndef __AFEB_teststand_Jorway73A_h__
#define __AFEB_teststand_Jorway73A_h__

#include <string>

using namespace std;

namespace AFEB { namespace teststand {

    class Jorway73A : public Module, public CAMAC
    {      
    public:
      Jorway73A()
	: Module( "Jorway73A", "CAMAC" ),
	  CAMAC(){}
    };

  }}

#endif
