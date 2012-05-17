#include "AFEB/teststand/Configuration.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/utils/DOM.h"
#include "AFEB/teststand/utils/System.h"
#include "AFEB/teststand/utils/String.h"

// Run it as
// LD_LIBRARY_PATH=$BUILD_HOME/AFEB/teststand/lib/$XDAQ_OS/$XDAQ_PLATFORM:$XDAQ_ROOT/lib:$ROOTSYS/lib AFEB_ROOT=$BUILD_HOME $BUILD_HOME/AFEB/teststand/test/$XDAQ_OS/$XDAQ_PLATFORM/refitSCurves.exe <list of results directories>

using namespace AFEB::teststand;

int main( int argc, char** argv ){

  for ( int i=1; i<argc; ++i ){
    cout << i << "  " << argv[i] << endl;

    // First back up raw results:
    utils::execShellCommand( string("cp -r ") + argv[i] + "/raw " + argv[i] + "/raw_before_" + utils::getDateTime() );
    
    // Read in results' XML, which contains the configuration, too:
    string resultXML = utils::readFile( string( argv[i] ) + "/raw/results.xml" );

    // Create configuration:
    Configuration *c = new Configuration( resultXML, argv[i] );

    // Loop over measurements and refit S curves
    vector<Measurement*> measurements = c->getMeasurements();
    for ( vector<Measurement*>::iterator m = measurements.begin(); m != measurements.end(); ++m ){
      // Do it only for threshold measurements:
      if ( (*m)->getType() == Measurement::count_vs_dac ){
	(*m)->refitSCurves();
	// Update XML with the new fit results
	resultXML = utils::removeSelectedNode( resultXML, "//a:results/a:measurement[@index='" + utils::stringFrom<int>( (*m)->getIndex() ) + "']" );
	resultXML = utils::appendToSelectedNode( resultXML, "//a:results", (*m)->resultsXML() );	
      }
    }
    // Overwrite results.xml with the new one
    utils::writeFile( string( argv[i] ) + "/raw/results.xml", resultXML );

  }

  return 0;
}
