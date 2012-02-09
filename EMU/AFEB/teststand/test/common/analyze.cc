#include "AFEB/teststand/Analysis.h"
#include "AFEB/teststand/utils/IO.h"

using namespace AFEB::teststand;

int main( int argc, char** argv ){
  
  for ( int i=1; i<argc; ++i ){
    cout << i << "  " << argv[i] << endl;
    AFEB::teststand::Analysis a( argv[i] );
  }

  return 0;
}
