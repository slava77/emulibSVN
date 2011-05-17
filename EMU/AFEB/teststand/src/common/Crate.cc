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
      os << "    " << setw(2) << i << " " << m->getName() << " " << m->getType() << " " << m->getSlot() << endl;
    }
  }

  return os;
}

// AFEB::teststand::Crate::~Crate(){
//   vector<AFEB::teststand::Module*>::iterator m;
//   for ( m = modules_.begin(); m != modules_.end(); ++m ) delete *m;
// }

void AFEB::teststand::Crate::insert( AFEB::teststand::Module* module, int slot ){
  modules_[slot] = module;
  modules_[slot]->setSlot( slot );
}

void AFEB::teststand::Crate::insertController( AFEB::teststand::CrateController* controller, int slot ){
  modules_[slot] = controller;
  modules_[slot]->setSlot( slot );
  controllerSlot_ = slot;
}
