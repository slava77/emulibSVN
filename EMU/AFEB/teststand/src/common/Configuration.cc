#include "AFEB/teststand/Configuration.h"

#include "AFEB/teststand/utils/DOM.h"
#include "AFEB/teststand/utils/Xalan.h"

using namespace std;

AFEB::teststand::Configuration::Configuration( const string XML ) : 
  xml_( XML )
{}

AFEB::teststand::Crate AFEB::teststand::Configuration::getCrate() const {
  int crateNumber = AFEB::teststand::utils::stringTo<int>( AFEB::teststand::utils::getSelectedNodeValue( xml_, "//c:crate/@c:number" ) );
  AFEB::teststand::Crate crate( crateNumber );
  for ( int slot=1; slot<=AFEB::teststand::Crate::maxModules_; ++slot ){
    stringstream xpath;
    xpath << "//c:crate/c:module[@c:slot=\"" << slot << "\"]/@c:name"
    string moduleName = AFEB::teststand::utils::getSelectedNodeValue( xml_, xpath.str() );
    string moduleType = AFEB::teststand::utils::getSelectedNodeValue( xml_, xpath.str() );
    if ( moduleName.size() != 0 && moduleType.size() != 0 ){
      if ( moduleName = "CrateController" ){
	
	crate.insertController();
    }
  }
}
