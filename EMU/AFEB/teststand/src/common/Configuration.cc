#include "AFEB/teststand/Configuration.h"

#include "AFEB/teststand/utils/DOM.h"
#include "AFEB/teststand/utils/Xalan.h"
#include "AFEB/teststand/utils/String.h"

#include "AFEB/teststand/Jorway73A.h"
#include "AFEB/teststand/LE32.h"
#include "AFEB/teststand/LeCroy3377.h"

using namespace std;

AFEB::teststand::Configuration::Configuration( const string XML ) : 
  xml_( XML )
{}

AFEB::teststand::Configuration::~Configuration(){
  // cout << "Configuration destroying " << modules_.size() << " modules:" << endl;
  vector<AFEB::teststand::Module*>::iterator m;
  for ( m = modules_.begin(); m != modules_.end(); ++m ){
    // cout << "   " << *m << " " << (*m)->getName() << " " << (*m)->getType() << endl << flush;
    delete *m;
  }
}

AFEB::teststand::Crate AFEB::teststand::Configuration::getCrate() {
  int crateNumber = AFEB::teststand::utils::stringTo<int>( AFEB::teststand::utils::getSelectedNodeValue( xml_, "//c:crate/@c:number" ) );
  AFEB::teststand::Crate crate;
  for ( int slot=1; slot<=AFEB::teststand::Crate::maxModules_; ++slot ){
    stringstream xpath;
    xpath << "//c:configuration/c:crate/c:module[@c:slot=\"" << slot << "\"]/@c:name";
    string moduleName = AFEB::teststand::utils::getSelectedNodeValue( xml_, xpath.str() );
    xpath.str("");
    xpath << "//c:configuration/c:crate/c:module[@c:slot=\"" << slot << "\"]/@c:type";
    string moduleType = AFEB::teststand::utils::getSelectedNodeValue( xml_, xpath.str() );
    // cout << slot << " " << moduleName << " " << moduleType << endl << flush;
    if ( moduleName.size() != 0 && moduleType.size() != 0 ){

      if ( moduleType == "CrateController" ){
	if ( moduleName == "Jorway73A" ){
	  AFEB::teststand::Jorway73A *module = new AFEB::teststand::Jorway73A( 8, crateNumber ); // TODO: branch
	  crate.insertController( module, slot );
	  modules_.push_back( module ); // for bookkeeping
	}
	else{
	  // TODO: throw an exception
	}
      }
      else if ( moduleType == "TDC" )
	if ( moduleName == "LeCroy3377" ){
	  AFEB::teststand::LeCroy3377 *module = new AFEB::teststand::LeCroy3377();
	  crate.insert( module, slot );
	  modules_.push_back( module ); // for bookkeeping
	}
	else{
	  // TODO: throw an exception
	}
      else if ( moduleType == "ThresholdSetter" ){
	if ( moduleName == "LE32" ){
	  AFEB::teststand::LE32 *module = new AFEB::teststand::LE32( moduleType  );
	  crate.insert( module, slot );
	  modules_.push_back( module ); // for bookkeeping
	}
	else{
	  // TODO: throw an exception
	}
      }
      else if ( moduleType == "PulseGenerator" ){
	if ( moduleName == "LE32" ){
	  AFEB::teststand::LE32 *module = new AFEB::teststand::LE32( moduleType  );
	  crate.insert( module, slot );
	  modules_.push_back( module ); // for bookkeeping
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
  return crate;
}
