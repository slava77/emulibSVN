#ifndef __AFEB_teststand_Jorway73A_h__
#define __AFEB_teststand_Jorway73A_h__

#include "AFEB/teststand/CrateController.h"
#include "AFEB/teststand/CAMAC.h"

#include <string>

using namespace std;

namespace AFEB { namespace teststand {

    class Jorway73A : public CrateController, public CAMAC
    {      
    public:
      Jorway73A( const int branch, const int crateNumber ) :
	CrateController( "Jorway73A", "CAMAC", branch, crateNumber ),
	CAMAC( branch, crateNumber )
      {}
    };

  }}

#endif
