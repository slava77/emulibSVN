#include "AFEB/teststand/Analysis.h"
#include "AFEB/teststand/utils/IO.h"

// Run it as
// LD_LIBRARY_PATH=$BUILD_HOME/AFEB/teststand/lib/$XDAQ_OS/$XDAQ_PLATFORM:$XDAQ_ROOT/lib:$ROOTSYS/lib AFEB_ROOT=$BUILD_HOME $BUILD_HOME/AFEB/teststand/test/$XDAQ_OS/$XDAQ_PLATFORM/select.exe <list of results directories>

using namespace AFEB::teststand;

int main( int argc, char** argv ){

  char* AFEB_ROOT = getenv( "AFEB_ROOT" );
  if ( getenv( "AFEB_ROOT" ) != NULL ){
    for ( int i=1; i<argc; ++i ){
      cout << i << "  " << argv[i] << endl;
      AFEB::teststand::Analysis a( AFEB_ROOT, argv[i] );
      a.applySelection();
    }
  }

  return 0;
}
