#include "AFEB/teststand/Analysis.h"
#include "AFEB/teststand/utils/System.h"
#include "AFEB/teststand/utils/DOM.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/utils/ROOTIO.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/fit/StraightLine2D.h"
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

  collectAnalyzedDevices();
  calibrateDACs( configXML );
  calculateGain();
}

AFEB::teststand::Analysis::~Analysis(){
  delete configuration_;
}

void AFEB::teststand::Analysis::collectAnalyzedDevices(){
  // Get devices from configuration's first measurement
  if ( configuration_->getMeasurements().size() == 0 ){
    XCEPT_RAISE( xcept::Exception, "No measurements found!" );
  }
  set<TestedDevice*> td = configuration_->getMeasurements().front()->getTestedDevices();
  for ( set<TestedDevice*>::iterator tdi = td.begin(); tdi != td.end(); ++tdi ){
    analyzedDevices_.push_back( AnalyzedDevice( **tdi ) );
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
    fit::LeastSquaresFitter<fit::StraightLine2D> slfitter;
    vector< pair<string,string> >::const_iterator ix, iy;
    TMatrixD X(1,1);
    TMatrixD Y(1,1);
    TMatrixDSym V(1);
    V( 0, 0 ) = 1.; // [mV^2], the estimated variance of the measurement
    for ( ix = x.begin(), iy = y.begin(); ix != x.end() && iy != y.end(); ++ix, ++iy ){
      X( 0, 0 ) = utils::stringTo<double>( ix->second );
      Y( 0, 0 ) = utils::stringTo<double>( iy->second );
      slfitter.addObservation( X, Y, V );
    }
    DACs_.back().setCalibrationParameters( slfitter.getFittedParameters(),
					   slfitter.getFittedParametersCovariance() );
    cout << DACs_.back().getCalibrationParametersCovariance() << endl;
    TMatrixT<double> mm(2,2); mm(0,0)=-1.; mm(0,1)=2.; mm(1,0)=3.; mm(1,1)=4.;
    cout << mm << endl;
  }
  cout << DACs_ << endl;
}

void AFEB::teststand::Analysis::calculateGain(){
  stringstream xpath;

  // Loop over the devices
  for ( vector<AnalyzedDevice>::const_iterator d = analyzedDevices_.begin(); d != analyzedDevices_.end(); ++d ){

    // Find the pulse generator's and the threshold generator's DACs for this device
    DAC pulseDAC( d->getCrate()->getModule( d->getPulseGeneratorSlot() )->getId(),
		  d->getCrate()->getModule( d->getPulseGeneratorSlot() )->getName(),
		  DAC::getType( d->getCrate()->getModule( d->getPulseGeneratorSlot() )->getType() ),
		  d->getPulseGeneratorSocket() ); // Just the descriptor, no calibration yet.
    DAC thresholdDAC( d->getCrate()->getModule( d->getSignalConverterSlot() )->getId(),
		      d->getCrate()->getModule( d->getSignalConverterSlot() )->getName(),
		      DAC::getType( d->getCrate()->getModule( d->getSignalConverterSlot() )->getType() ),
		      d->getSignalConverterSocket() ); // Just the descriptor, no calibration yet.
    for ( vector<DAC>::const_iterator dac=DACs_.begin(); dac!=DACs_.end(); ++dac ){
      if ( *dac == pulseDAC ){
	cout << "Found pulse DAC" << endl << *dac << endl;
	pulseDAC = *dac; // Now it contains the calibration, too.
      }
      else if ( *dac == thresholdDAC ){
	cout << "Found threshold DAC" << endl << *dac << endl;
	thresholdDAC = *dac; // Now it contains the calibration, too.
      }
    }

    // Find the count_vs_dac measurements with charge injection through external capacitors
    for ( vector<Measurement*>::const_iterator m = configuration_->getMeasurements().begin(); m != configuration_->getMeasurements().end(); ++m ){
      if ( (*m)->getTypeType() == Measurement::count_vs_dac && (*m)->getInjectionCapacitorType() == Measurement::external ){      
	// Open the root file of this measurement's results...
	string fileName = Results::getFileName( (*m)->getIndex(), (*m)->getType(), d->getId() );
	TFile f( ( rawResultsDir_ + "/" + fileName +".root").c_str(), "READ" );
	if ( f.IsZombie() ){
	  stringstream ss;
	  ss << "Failed to open ROOT file " << f.GetName();
	  XCEPT_RAISE( xcept::Exception, ss.str() );
	}
	cout << "Opened " << f.GetName() << endl;
	f.cd();
	// ...and get the threshold histogram
	string histogramName = string( "threshold__" ) + fileName;
	TH1D *histogram;
	f.GetObject( histogramName.c_str(), histogram );

	// Loop over the channels
	for ( int iChannel=0; iChannel<d->getNChannels(); ++iChannel ){
	  cout << iChannel+1 << "  " 
	       << histogram->GetBinContent( iChannel+1 ) << "   " 
	       << histogram->GetBinError( iChannel+1 ) << endl;

	}

	f.Close();
      }
    }
  }
}
