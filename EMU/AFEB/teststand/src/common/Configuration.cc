#include "AFEB/teststand/Configuration.h"

#include "AFEB/teststand/utils/DOM.h"
#include "AFEB/teststand/utils/Xalan.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/System.h"
#include "AFEB/teststand/utils/IO.h"

#include "AFEB/teststand/crate/Jorway73A.h"
#include "AFEB/teststand/crate/LE32.h"
#include "AFEB/teststand/crate/LeCroy3377.h"

#include "xcept/Exception.h"

#include <iomanip>

using namespace std;
using namespace AFEB::teststand;

AFEB::teststand::Configuration::Configuration( const string XML, const string resultDir ) : 
  xml_( XML ),
  resultDir_( resultDir )
{
  createMeasurements();
  for ( vector<Measurement*>::iterator  i = measurements_.begin() ; i != measurements_.end() ; ++i ) cout << **i;
  createCalibration();
}

AFEB::teststand::Configuration::~Configuration(){
  delete calibration_;
  calibration_ = NULL;
  delete crate_;
  crate_ = NULL;
  for ( vector<Measurement*>::iterator  i = measurements_.begin() ; i != measurements_.end() ; ++i ) delete *i;
  measurements_.clear();
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
    string moduleTypeString = utils::getSelectedNodeValue( xml_, xpath.str() );
    xpath.str("");
    xpath << "//c:configuration/c:crate/c:module[@c:slot=\"" << slot << "\"]/@c:id";
    string moduleId = utils::getSelectedNodeValue( xml_, xpath.str() );
    if ( moduleName.size() != 0 && moduleTypeString.size() != 0 ){

      AFEB::teststand::Module::Type_t moduleType = AFEB::teststand::Module::getType( moduleTypeString );

      if ( moduleType == AFEB::teststand::Module::CrateController ){
	if ( moduleName == "Jorway73A" ){
	  try{
	    AFEB::teststand::Jorway73A *module = new AFEB::teststand::Jorway73A( moduleId, crateNumber );
	    crate_->insertController( module, slot );
	  } catch( xcept::Exception &e ){
	    XCEPT_RETHROW( xcept::Exception, "Failed to create Jorway73A.", e );
	  }	  
	}
	else{
	  // TODO: throw an exception
	}
      }
      else if ( moduleType == AFEB::teststand::Module::TDC )
	if ( moduleName == "LeCroy3377" ){
	  AFEB::teststand::LeCroy3377 *module = new AFEB::teststand::LeCroy3377( moduleId );
	  crate_->insert( module, slot );
	}
	else{
	  // TODO: throw an exception
	}
      else if ( moduleType == AFEB::teststand::Module::SignalConverter ){
	if ( moduleName == "LE32" ){
	  AFEB::teststand::LE32 *module = new AFEB::teststand::LE32( moduleType, moduleId );
	  crate_->insert( module, slot );
	}
	else{
	  // TODO: throw an exception
	}
      }
      else if ( moduleType == AFEB::teststand::Module::PulseGenerator ){
	if ( moduleName == "LE32" ){
	  AFEB::teststand::LE32 *module = new AFEB::teststand::LE32( moduleType, moduleId );
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
  // Get the 'enabled' flag of the measurements ["yes" or "no"]:
  vector< pair<string,string> > measurementEnabledFlags = utils::getSelectedNodesValues( xml_, "/c:configuration/c:measurements/c:measurement/@c:enabled" );

  // Find the requested (enabled) measurements in the XML
  vector< pair<string,string> > enabledMeasurementTypes = utils::getSelectedNodesValues( xml_, "/c:configuration/c:measurements/c:measurement[@c:enabled='yes']/@c:type" );
  if ( enabledMeasurementTypes.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, "No measurements are selected." );
  }

  // Create the crate setup:
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
    
    // Loop over the measurements
    int index = 0; // counts the selected measurements
    for ( size_t i = 0; i < measurementEnabledFlags.size(); ++i ){
      
      // Check if it's selected (i.e., enabled):
      if ( measurementEnabledFlags.at( i ).second == "yes" ){
	// In XPath, position is counted from 1:
	int position = i + 1;
	
	// Check if a measurement with this TDC module already exists:
	Measurement* measurement = findMeasurement( position, testedDevice->getTDCSlot() );
	if ( measurement == NULL ){
	  // Create this measurement
	  xpath.str("");
	  xpath << "/c:configuration/c:measurements/c:measurement[position()=" << position << "]/@c:name";
	  string name = utils::getSelectedNodeValue( xml_, xpath.str() );
	  xpath.str("");
	  xpath << "/c:configuration/c:measurements/c:measurement[position()=" << position << "]/@c:type";
	  string type = utils::getSelectedNodeValue( xml_, xpath.str() );
	  bool dummyData = ( utils::getSelectedNodesValues( xml_, "//c:dummyData" ).size() > 0 ); 
	  measurement = new Measurement( position, index++, name, type, resultDir_, dummyData );
	  // Set pulse generator parameters
	  xpath.str("");
	  xpath << "/c:configuration/c:measurements/c:measurement[position()=" << position << "]/c:PulseGenerator/@*";
	  vector< pair<string,string> > parameters = utils::getSelectedNodesValues( xml_, xpath.str() );
	  measurement->setPulseParameters( parameters );
	  // Set thereshold parameters
	  xpath.str("");
	  xpath << "/c:configuration/c:measurements/c:measurement[position()=" << position << "]/c:SignalConverter/@*";
	  parameters.clear();
	  parameters = utils::getSelectedNodesValues( xml_, xpath.str() );
	  measurement->setThresholdParameters( parameters );
	  // Set TDC parameters
	  xpath.str("");
	  xpath << "/c:configuration/c:measurements/c:measurement[position()=" << position << "]/c:TDC/@*";
	  parameters.clear();
	  parameters = utils::getSelectedNodesValues( xml_, xpath.str() );
	  measurement->setTDCParameters( parameters );
	  
	  measurements_.push_back( measurement );
	  
	} // if ( measurement == NULL )
	
	// Add this device to be tested in this measurement
	measurement->addTestedDevice( testedDevice );
	
      } // if ( measurementEnabledFlags.at( i ).second == "yes" )
    } // for ( size_t i = 0; i < measurementEnabledFlags.size(); ++i )
  } // for ( t = testedDevices.begin(); t != testedDevices.end(); ++t )
}

void AFEB::teststand::Configuration::createCalibration(){
  calibration_ = new Calibration();
  for ( int iSlot = 1; iSlot<=crate_->maxModules_; ++iSlot ){
    Module* module = crate_->getModule( iSlot );
    if ( module != NULL ){
      if ( module->getType() == AFEB::teststand::Module::PulseGenerator ||
	   module->getType() == AFEB::teststand::Module::SignalConverter   ){
	calibration_->addModule( module );
      }
    }
  }
  cout << *calibration_;
}


Measurement* AFEB::teststand::Configuration::findMeasurement( const int position, const int tdcSlot ) const {
  vector<Measurement*>::const_iterator m;
  for ( m = measurements_.begin(); m != measurements_.end(); ++m ){
    if ( (*m)->getPosition() == position && 
	 (*m)->getTDCSlot()  == tdcSlot     ){
      return *m;
    }
  }
  return NULL;
}

string AFEB::teststand::Configuration::resultsXML(){
  stringstream ss;
  for ( vector<Measurement*>::const_iterator m = measurements_.begin(); m != measurements_.end(); ++m ){
    ss << "<a:measurement a:index=\""  << (*m)->getIndex()
       <<             "\" a:type=\""   << (*m)->getType()
       <<             "\" a:name=\""   << (*m)->getName()
       <<             "\" a:status=\"" << (*m)->getStatus() 
       <<             "\">" << endl;
    map<TestedDevice*,Results*> results = (*m)->getResults();
    for ( map<TestedDevice*,Results*>::const_iterator r = results.begin(); r != results.end(); ++r ){
      ss << "<a:device a:id=\"" << r->first->getId() << "\">" << endl
	 << "<a:file a:name=\"" << r->second->getFileName() << "\"/>" << endl;
      // Loop over channels and get fit results
      for ( int iChannel = 0; iChannel < r->first->getNChannels(); ++iChannel ){
	ss << "<a:channel a:number=\"" <<  iChannel << "\">";
	map<string,pair<double,double> > parameters = r->second->getParameters( iChannel );
	for ( map<string,pair<double,double> >::const_iterator p = parameters.begin(); p != parameters.end(); ++p ){ 
	  ss << "<a:parameter a:name=\""  << p->first
	     <<           "\" a:value=\"" <<   showpos << showpoint << setprecision(6) << p->second.first
	     <<           "\" a:error=\"" << noshowpos << showpoint << setprecision(6) << p->second.second
	     <<           "\"/>" << noshowpos << noshowpoint;
	}
	ss << "</a:channel>" << endl;
      } // for ( int iChannel = 1; iChannel <= r->first->getNChannels(); ++iChannel )
      // Add statistics to assess stability and uniformity
      ss << "<a:statistics>" << endl;
      map<string,double> stat = r->second->getThresholdStats();
      ss << "<a:parameter a:name=\"threshold [ADC units]\"";
      for ( map<string,double>::const_iterator s=stat.begin(); s!=stat.end(); ++s ){
	ss << " a:" << s->first << "=\"" << noshowpos << showpoint << setprecision(6) << s->second << "\"";
      }
      ss << "/>" << endl;
      stat = r->second->getNoiseStats();
      ss << "<a:parameter a:name=\"noise [ADC units]\"";
      for ( map<string,double>::const_iterator s=stat.begin(); s!=stat.end(); ++s ){
	ss << " a:" << s->first << "=\"" << noshowpos << showpoint << setprecision(6) << s->second << "\"";
      }
      ss << "/>" << endl;
      stat = r->second->getTimeStats();
      ss << "<a:parameter a:name=\"mean time on plateau [TDC units]\"";
      for ( map<string,double>::const_iterator s=stat.begin(); s!=stat.end(); ++s ){
	ss << " a:" << s->first << "=\"" << noshowpos << showpoint << setprecision(6) << s->second << "\"";
      }
      ss << "/>" << endl;      
      ss << "</a:statistics>" << endl;
      ss << "</a:device>" << endl;
    } // for ( map<TestedDevice*,Results*>::const_iterator r = results.begin(); r != results.end(); ++r )
    ss << "</a:measurement>" << endl;
  } // for ( vector<Measurement*>::const_iterator m = measurements.begin(); m != measurements.end(); ++m )
  return ss.str();
}
