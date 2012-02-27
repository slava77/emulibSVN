#include "AFEB/teststand/Analysis.h"
#include "AFEB/teststand/utils/System.h"
#include "AFEB/teststand/utils/DOM.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/utils/ROOTIO.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/fit/Polynomial.h"
#include "AFEB/teststand/fit/LeastSquaresFitter.h"
#include "TFile.h"
#include "TH1D.h"
#include <iostream>
#include <sstream>
#include <set>

using namespace AFEB::teststand;

AFEB::teststand::Analysis::Analysis( const string& resultsDir )
  : rawResultsDir_     ( resultsDir + "/raw"      )
  , analyzedResultsDir_( resultsDir + "/analyzed" ){
  string       XML = utils::readFile( rawResultsDir_ + "/results.xml" );
  string configXML = utils::getSelectedNode( XML, "/root/c:configuration" );
  rawResultXML_    = utils::getSelectedNode( XML, "/root/a:results" );

  configuration_ = new Configuration( configXML, "" );

  collectAnalyzedDevices( configXML );
  calibrateDACs( configXML );
  assignDACs();
  calculateGain();
  calculateInternalCapacitance();
  cout << analyzedDevices_ << endl;
  saveResults();
}

AFEB::teststand::Analysis::~Analysis(){
  delete configuration_;
}

void AFEB::teststand::Analysis::collectAnalyzedDevices( const string& configXML ){

  // Get devices from configuration's first measurement
  if ( configuration_->getMeasurements().size() == 0 ){
    XCEPT_RAISE( xcept::Exception, "No measurements found!" );
  }
  set<TestedDevice*> td = configuration_->getMeasurements().front()->getTestedDevices();
  for ( set<TestedDevice*>::iterator tdi = td.begin(); tdi != td.end(); ++tdi ){
    analyzedDevices_.push_back( AnalyzedDevice( **tdi, rawResultsDir_, configuration_->getMeasurements() ) );
  }

  // Associate with the analyzed devices the parameters of the adaptor used
  for ( vector<AnalyzedDevice>::iterator ad = analyzedDevices_.begin(); ad != analyzedDevices_.end(); ++ad ){
    // Get name of adaptor in use
    stringstream xpath;
    xpath << "/c:configuration/c:inputs/@c:adaptor";
    string adaptorName = utils::getSelectedNodeValue( configXML, xpath.str() );
    ad->setAdaptorName( adaptorName );
    // Get the injection capacitance [pF] of this adaptor
    xpath.str("");
    xpath << "/c:configuration/c:calibrations/c:adaptors/c:adaptor[@c:name='" << adaptorName << "']/@c:injectionCapacitance";
    string capacitance = utils::getSelectedNodeValue( configXML, xpath.str() );
    cout << xpath.str() << endl;
    cout << capacitance << endl;
    if ( capacitance.size() == 0 ){
      stringstream ss; ss << "Failed to find the injection capacitance for adaptor " << adaptorName;
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    ad->setInjectionCapacitance( utils::stringTo<double>( capacitance ) );
    // Get the pulse division factor of this adaptor
    xpath.str("");
    xpath << "/c:configuration/c:calibrations/c:adaptors/c:adaptor[@c:name='" << adaptorName << "']/@c:pulseDivisionFactor";
    string factor = utils::getSelectedNodeValue( configXML, xpath.str() );
    if ( capacitance.size() == 0 ){
      stringstream ss; ss << "Failed to find the pulse division factor for adaptor " << adaptorName;
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    ad->setPulseDivisionFactor( utils::stringTo<double>( factor ) );
    // Get the correction coefficient for this adaptor's socket into which this device is plugged into
    xpath.str("");
    xpath << "/c:configuration/c:calibrations/c:adaptors/c:adaptor[@c:name='" << adaptorName 
	  << "']/c:socket[@c:number='" << ad->getAdaptorSocket() << "']/@c:coefficient";
    string coefficient = utils::getSelectedNodeValue( configXML, xpath.str() );
    if ( coefficient.size() == 0 ){
      stringstream ss; ss << "Failed to find the threshold correction coefficient for socket " << ad->getAdaptorSocket() << " of adaptor " << adaptorName;
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    ad->setCorrectionCoefficient( utils::stringTo<int>( coefficient ) );
  }

  cout << "Analyzed devices" << endl << analyzedDevices_ << endl;
}

void AFEB::teststand::Analysis::calibrateDACs( const string& configXML ){
  stringstream xpath;
  // Count DACs with calibration data
  size_t nDACs = utils::getSelectedNodesValues( configXML, "/c:configuration/c:calibrations/c:DACs/c:DAC" ).size();
  cout << "nDACs=" << nDACs << endl;
  for ( size_t iDAC=1; iDAC<=nDACs; ++iDAC ){
    //
    // Create DAC objects
    //
    xpath.str("");
    xpath << "/c:configuration/c:calibrations/c:DACs/c:DAC[position()=" << iDAC << "]/@c:moduleId";
    string moduleId = utils::getSelectedNodeValue( configXML, xpath.str() );
    xpath.str("");
    xpath << "/c:configuration/c:calibrations/c:DACs/c:DAC[position()=" << iDAC << "]/@c:moduleName";
    string moduleName = utils::getSelectedNodeValue( configXML, xpath.str() );
    xpath.str("");
    xpath << "/c:configuration/c:calibrations/c:DACs/c:DAC[position()=" << iDAC << "]/@c:type";
    string type = utils::getSelectedNodeValue( configXML, xpath.str() );
    xpath.str("");
    xpath << "/c:configuration/c:calibrations/c:DACs/c:DAC[position()=" << iDAC << "]/@c:channel";
    int channel = utils::stringTo<int>( utils::getSelectedNodeValue( configXML, xpath.str() ) );
    DACs_.push_back( DAC( moduleId, moduleName, type, channel ) );
    //
    // Load calibration data
    //
    xpath.str("");
    xpath << "/c:configuration/c:calibrations/c:DACs/c:DAC[position()=" << iDAC << "]/*[name()='c:value']/@c:DACUnits";
    vector< pair<string,string> > x = utils::getSelectedNodesValues( configXML, xpath.str() );
    cout << x << endl;
    xpath.str("");
    xpath << "/c:configuration/c:calibrations/c:DACs/c:DAC[position()=" << iDAC << "]/*[name()='c:value']/@c:milliVolts";
    vector< pair<string,string> > y = utils::getSelectedNodesValues( configXML, xpath.str() );
    cout << y << endl;
    //
    // Fit calibration data
    //
    if ( x.size() != y.size() ){
      stringstream ss;
      ss << "Calibration data size mismatch for " << DACs_.back();
      // DAC dac( moduleId, moduleName, type, channel );
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    fit::LeastSquaresFitter< fit::Polynomial<1> > p1fitter;
    vector< pair<string,string> >::const_iterator ix, iy;
    TMatrixD X(1,1);
    TMatrixD Y(1,1);
    TMatrixDSym V(1);
    V( 0, 0 ) = 1.; // [mV^2], the estimated variance of the measurement
    for ( ix = x.begin(), iy = y.begin(); ix != x.end() && iy != y.end(); ++ix, ++iy ){
      X( 0, 0 ) = utils::stringTo<double>( ix->second );
      Y( 0, 0 ) = utils::stringTo<double>( iy->second );
      p1fitter.addObservation( X, Y, V );
    }
    DACs_.back().setCalibrationParameters( p1fitter.getFittedParameters(),
					   p1fitter.getFittedParametersCovariance() );
    cout << *DACs_.back().getCalibrationParameters() << endl;
    cout << *DACs_.back().getCalibrationParametersCovariance() << endl;
    // TMatrixT<double> mm(2,2); mm(0,0)=-1.; mm(0,1)=2.; mm(1,0)=3.; mm(1,1)=4.;
    // cout << mm << endl;
  }
  cout << DACs_ << endl;
}

void AFEB::teststand::Analysis::assignDACs(){
  // Loop over the devices
  for ( vector<AnalyzedDevice>::iterator d = analyzedDevices_.begin(); d != analyzedDevices_.end(); ++d ){

    // Find the pulse generator's and the threshold generator's DACs for this device
    for ( vector<DAC>::const_iterator dac=DACs_.begin(); dac!=DACs_.end(); ++dac ){
      if ( *dac == d->getPulseDACDescriptor() ){
	cout << "Found pulse DAC" << endl << *dac << endl;
	d->setPulseDAC( &*dac );
      }
      else if ( *dac == d->getThresholdDACDescriptor() ){
	cout << "Found threshold DAC" << endl << *dac << endl;
	d->setThresholdDAC( &*dac );
      }
    }

  }  
}

void AFEB::teststand::Analysis::calculateGain(){
  // Loop over the devices
  for ( vector<AnalyzedDevice>::iterator d = analyzedDevices_.begin(); d != analyzedDevices_.end(); ++d ){
    d->calculateGains();
  }  
}


void AFEB::teststand::Analysis::calculateInternalCapacitance(){
  for ( vector<AnalyzedDevice>::iterator d = analyzedDevices_.begin(); d != analyzedDevices_.end(); ++d ){
    d->calculateInternalCapacitances();
  }
}


void AFEB::teststand::Analysis::saveResults(){
  for ( vector<AnalyzedDevice>::iterator d = analyzedDevices_.begin(); d != analyzedDevices_.end(); ++d ){
    d->saveResults( analyzedResultsDir_ );
  }
}

void AFEB::teststand::Analysis::saveResults( const string& destinationDir ){
  for ( vector<AnalyzedDevice>::iterator d = analyzedDevices_.begin(); d != analyzedDevices_.end(); ++d ){
    d->saveResults( destinationDir );
  }
}
