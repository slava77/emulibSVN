#include "AFEB/teststand/LeCroy3377.h"
#include "AFEB/teststand/Jorway73A.h"
#include "AFEB/teststand/Crate.h"

#include <assert.h>
#include <stdlib.h> // for exit()

AFEB::teststand::LeCroy3377::LeCroy3377( const string name ) :
  Module( name, "CAMAC" ){
}
