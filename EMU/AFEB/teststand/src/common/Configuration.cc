#include "AFEB/teststand/Configuration.h"

#include "AFEB/teststand/utils/DOM.h"
#include "AFEB/teststand/utils/Xalan.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"

#include "AFEB/teststand/Jorway73A.h"
#include "AFEB/teststand/LE32.h"
#include "AFEB/teststand/LeCroy3377.h"

#include "xcept/Exception.h"

using namespace std;
using namespace AFEB::teststand;

AFEB::teststand::Configuration::Configuration( const string XML ) : 
  xml_( XML )
{
  createMeasurements();
}

AFEB::teststand::Configuration::~Configuration(){
  delete crate_;
  for ( vector<Measurement*>::iterator  i = measurements_.begin() ; i != measurements_.end() ; ++i ) delete *i;
  for ( vector<TestedDevice*>::iterator i = testedDevices_.begin(); i != testedDevices_.end(); ++i ) delete *i;
}

void AFEB::teststand::Configuration::createCrate() {
  int crateNumber = utils::stringTo<int>( utils::getSelectedNodeValue( xml_, "//c:crate/@c:number" ) );
  crate_ = new AFEB::teststand::Crate();
  for ( int slot=1; slot<=AFEB::teststand::Crate::maxModules_; ++slot ){
    stringstream xpath;
    xpath << "//c:configuration/c:crate/c:module[@c:slot=\"" << slot << "\"]/@c:name";
    string moduleName = utils::getSelectedNodeValue( xml_, xpath.str() );
    xpath.str("");
    xpath << "//c:configuration/c:crate/c:module[@c:slot=\"" << slot << "\"]/@c:type";
    string moduleType = utils::getSelectedNodeValue( xml_, xpath.str() );
    if ( moduleName.size() != 0 && moduleType.size() != 0 ){

      if ( moduleType == "CrateController" ){
	if ( moduleName == "Jorway73A" ){
	  AFEB::teststand::Jorway73A *module = new AFEB::teststand::Jorway73A( 8, crateNumber ); // TODO: branch
	  crate_->insertController( module, slot );
	}
	else{
	  // TODO: throw an exception
	}
      }
      else if ( moduleType == "TDC" )
	if ( moduleName == "LeCroy3377" ){
	  AFEB::teststand::LeCroy3377 *module = new AFEB::teststand::LeCroy3377();
	  crate_->insert( module, slot );
	}
	else{
	  // TODO: throw an exception
	}
      else if ( moduleType == "SignalConverter" ){
	if ( moduleName == "LE32" ){
	  AFEB::teststand::LE32 *module = new AFEB::teststand::LE32( moduleType  );
	  crate_->insert( module, slot );
	}
	else{
	  // TODO: throw an exception
	}
      }
      else if ( moduleType == "PulseGenerator" ){
	if ( moduleName == "LE32" ){
	  AFEB::teststand::LE32 *module = new AFEB::teststand::LE32( moduleType  );
	  crate_->insert( module, slot );
	}
	else{
	  // TODO: throw an exception
	}
      }
      else{
	  // TODO: throw an exception
      }
    }
  }
}

void AFEB::teststand::Configuration::createMeasurements() {
  // Find the requested (enabled) measurements in the XML
  vector< pair<string,string> > enabledMeasurementTypes = utils::getSelectedNodesValues( xml_, "/c:configuration/c:measurements/c:measurement[@c:enabled!='0']/@c:type" );
  if ( enabledMeasurementTypes.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, "No measurements are selected." );
  }

  // Create the crate setup
  createCrate();

  // Find the type of tested device
  string testedDeviceType   = utils::getSelectedNodeValue( xml_, "/c:configuration/c:testedDevice/@c:type" );
  int testedDeviceNChannels = utils::stringTo<int>( utils::getSelectedNodeValue( xml_, "/c:configuration/c:testedDevice/@c:nChannels" ) );


  // Find the socket number of devices to be tested (i.e., those for which an id is given):
  vector< pair<string,string> > testedDevices = utils::getSelectedNodesValues( xml_, "/c:configuration/c:inputs/c:testedDevice[@c:id!='']/@c:socket" );
  if ( testedDevices.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, "No device ids to be tested are specified." );
  }

  // Loop over the devices to be tested:
  vector< pair<string,string> >::const_iterator t;
  for ( t = testedDevices.begin(); t != testedDevices.end(); ++t ){
    // Create this device
    stringstream xpath;
    xpath << "/c:configuration/c:inputs/c:testedDevice[@c:socket='" << t->second << "']/@*";
    vector< pair<string,string> > deviceParameters = utils::getSelectedNodesValues( xml_, xpath.str() );
    TestedDevice* testedDevice = new TestedDevice( testedDeviceType, testedDeviceNChannels, crate_ );
    testedDevice->setParameters( deviceParameters );
    
    cout << deviceParameters << endl;
    cout << "Created device" << endl << *testedDevice << endl;

    // Loop over the requested (enabled) measurement types
    vector< pair<string,string> >::const_iterator m;
    for ( m = enabledMeasurementTypes.begin(); m != enabledMeasurementTypes.end(); ++m ){

      // Check if a measurement with this TDC module already exists:
      Measurement* measurement = findMeasurement( m->second, testedDevice->getTDCSlot() );
      if ( measurement == NULL ){
	// Create this measurement
	xpath.str("");
	xpath << "/c:configuration/c:measurements/c:measurement[@c:type='" << m->second << "']/@c:name";
	string name = utils::getSelectedNodeValue( xml_, xpath.str() );
	measurement = new Measurement( name, m->second );
	// Set pulse generator parameters
	xpath.str("");
	xpath << "/c:configuration/c:measurements/c:measurement[@c:type='" << m->second << "']/c:PulseGenerator/@*";
	vector< pair<string,string> > parameters = utils::getSelectedNodesValues( xml_, xpath.str() );
	measurement->setPulseParameters( parameters );
	// Set thereshold parameters
	xpath.str("");
	xpath << "/c:configuration/c:measurements/c:measurement[@c:type='" << m->second << "']/c:SignalConverter/@*";
	parameters.clear();
	parameters = utils::getSelectedNodesValues( xml_, xpath.str() );
	measurement->setThresholdParameters( parameters );
	// Set TDC parameters
	xpath.str("");
	xpath << "/c:configuration/c:measurements/c:measurement[@c:type='" << m->second << "']/c:TDC/@*";
	parameters.clear();
	parameters = utils::getSelectedNodesValues( xml_, xpath.str() );
	measurement->setTDCParameters( parameters );
	
	measurements_.push_back( measurement );
	
      } // if ( measurement == NULL )

      // Add this device to be tested in this measurement
      measurement->addTestedDevice( testedDevice );
      
    } // for ( m = enabledMeasurementTypes.begin(); m != enabledMeasurementTypes.end(); ++m )
  } // for ( t = testedDevices.begin(); t != testedDevices.end(); ++t )
}


Measurement* AFEB::teststand::Configuration::findMeasurement( const string type, const int tdcSlot ) const {
  vector<Measurement*>::const_iterator m;
  for ( m = measurements_.begin(); m != measurements_.end(); ++m ){
    if ( (*m)->getType().compare( type ) == 0       && 
	 (*m)->getTDCSlot()              == tdcSlot    ){
      return *m;
    }
  }
  return NULL;
}
