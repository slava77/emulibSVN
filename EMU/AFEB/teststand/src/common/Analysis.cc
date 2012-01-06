#include "AFEB/teststand/Analysis.h"
#include "AFEB/teststand/utils/System.h"
#include "AFEB/teststand/utils/DOM.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/utils/String.h"
#include <iostream>

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
  //
  // Create DACs
  //
  stringstream xpath;
  size_t nDACs = utils::getSelectedNodesValues( configXML, "/c:configuration/c:calibrations/c:DACs/c:DAC" ).size();
  cout << "nDACs=" << nDACs << endl;
  for ( size_t iDAC=1; iDAC<=nDACs; ++iDAC ){
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
    //cout << DACs_.back();
  }
  cout << DACs_ << endl;

  //
  // Load calibration data
  //
  

}

void AFEB::teststand::Analysis::calculateGain(){
  // Find the count_vs_dac measurements with charge injection into external capacitors
  for ( vector<Measurement*>::const_iterator m = configuration_->getMeasurements().begin(); 
	m != configuration_->getMeasurements().end(); ++m ){
    //if ( (*m)->getTypeType() == Measurement::count_vs_dac && (*m)->getPulsedCapacitor() == "external" ){
    if ( (*m)->getTypeType() == Measurement::dummy && (*m)->getPulsedCapacitor() == "external" ){
      cout << **m;
    }
  }
}
