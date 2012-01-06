#include "AFEB/teststand/Analysis.h"
#include "AFEB/teststand/utils/System.h"
#include "AFEB/teststand/utils/DOM.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/utils/ROOTIO.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/fit/StraightLine2D.h"
#include "AFEB/teststand/fit/LeastSquaresFitter.h"
#include <iostream>
#include <sstream>

using namespace AFEB::teststand;

AFEB::teststand::Analysis::Analysis( const string& resultsDir ){
  string       XML = utils::readFile( resultsDir + "/raw/results.xml" );
  string configXML = utils::getSelectedNode( XML, "/root/c:configuration" );
  rawResultXML_    = utils::getSelectedNode( XML, "/root/a:results" );

  configuration_ = new Configuration( configXML, "" );
  calibrateDACs( configXML );
  calculateGain();
}

AFEB::teststand::Analysis::~Analysis(){
  delete configuration_;
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
  // Find the count_vs_dac measurements with charge injection into external capacitors
  for ( vector<Measurement*>::const_iterator m = configuration_->getMeasurements().begin(); 
	m != configuration_->getMeasurements().end(); ++m ){
    //if ( (*m)->getTypeType() == Measurement::count_vs_dac && (*m)->getPulsedCapacitor() == "external" ){
    if ( (*m)->getTypeType() == Measurement::dummy && (*m)->getPulsedCapacitor() == "external" ){
      //cout << **m;
    }
  }
}
