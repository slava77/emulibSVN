#include "AFEB/teststand/crate/Module.h"

#include "xcept/Exception.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace AFEB::teststand;

const char* const AFEB::teststand::Module::typeStrings_[Module::nTypes] = { "CrateController", 
									    "PulseGenerator", 
									    "SignalConverter", 
									    "TDC" };

ostream& AFEB::teststand::operator<<( ostream& os, const Module& m ){
  os << "Module name: \t" << setw(20) << m.getName()
     << "\t id: \t"       << m.getId()
     << "\t type: \t"     << setw(20) << m.getType()
     << "\t slot: \t"     << m.getSlot()
     << endl;
  return os;
}

AFEB::teststand::Module::Module( const string name, const AFEB::teststand::Module::Type_t type, const string id )
  : name_( name ),
    type_( type ),
    id_  (   id ){}

AFEB::teststand::Module::Module( const string name, const string type, const string id )
  : name_( name                    ),
    type_( Module::getType( type ) ),
    id_  (   id                    ){}

// static function
AFEB::teststand::Module::Type_t 
AFEB::teststand::Module::getType( const string& typeString ){
  AFEB::teststand::Module::Type_t type = AFEB::teststand::Module::nTypes;
  bool isValidType = false;
  for ( int i=0; i<AFEB::teststand::Module::nTypes && !isValidType; ++i ){
    isValidType |= ( typeString.compare( typeStrings_[i] ) == 0 );
    if ( isValidType ) type = (AFEB::teststand::Module::Type_t) i;
  }
  if ( !isValidType ){
    stringstream ss;
    ss << "\"" << typeString << "\" is not a valid Module type.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return type;
}

// static function
string AFEB::teststand::Module::getTypeString( const Type_t type ){
  if ( type < (AFEB::teststand::Module::Type_t)0 || type >=AFEB::teststand::Module::nTypes ){
    stringstream ss;
    ss << "'" << type << "' is not a valid Module type.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return typeStrings_[type];
}
