#include "AFEB/teststand/Analysis.h"
#include "AFEB/teststand/utils/System.h"
#include "AFEB/teststand/utils/DOM.h"
#include "AFEB/teststand/utils/IO.h"
#include <iostream>

using namespace AFEB::teststand;

AFEB::teststand::Analysis::Analysis( const string& resultsDir ){
  string       XML = utils::readFile( resultsDir + "/raw/results.xml" );
  string configXML = utils::getSelectedNode( XML, "/root/c:configuration" );
  rawResultXML_    = utils::getSelectedNode( XML, "/root/a:results" );

  configuration_ = new Configuration( configXML, "" );
}

AFEB::teststand::Analysis::~Analysis(){
  delete configuration_;
}

void AFEB::teststand::Analysis::calculateGain(){
}
