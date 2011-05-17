#include "AFEB/teststand/Crate.h"

#include <iomanip>

ostream& AFEB::teststand::operator<<( ostream& os, const AFEB::teststand::Crate& c ){

  os << "Number " << c.getNumber() << endl
     << "Name   " << c.getName()   << endl
     << "Type   " << c.getType()   << endl
     << "Modules:" << endl;

  for ( int i=1; i<=AFEB::teststand::Crate::maxModules_; ++i ){
    const AFEB::teststand::Module* m = c.getModule( i );
    if ( m ){
      os << right << setw(10) << i
	 << "  "
	 << left  << setw(20) << m->getName() << m->getType() << endl;
    }
  }

  return os;
}

int AFEB::teststand::Crate::getNumber() const {
  if ( 1 <= controllerSlot_ && controllerSlot_ <= maxModules_ ){
    if ( modules_[controllerSlot_] != NULL ) return getCrateController()->getCrateNumber();
  }
  return -1;
}

void AFEB::teststand::Crate::insert( AFEB::teststand::Module* module, int slot ){
  modules_[slot] = module;
  modules_[slot]->setSlot( slot );
}

void AFEB::teststand::Crate::insertController( AFEB::teststand::CrateController* controller, int slot ){
  modules_[slot] = controller;
  modules_[slot]->setSlot( slot );
  controllerSlot_ = slot;
}
