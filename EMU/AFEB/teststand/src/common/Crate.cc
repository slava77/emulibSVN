#include "AFEB/teststand/Crate.h"

#include <iomanip>

ostream& AFEB::teststand::operator<<( ostream& os, const AFEB::teststand::Crate& c ){

  os << "Crate number " << c.getNumber() << ", controller slot " << c.controllerSlot_ << endl
     << "      slot  module name         module type         slot from module" << endl;

  for ( int i=1; i<=AFEB::teststand::Crate::maxModules_; ++i ){
    const AFEB::teststand::Module* m = c.getModule( i );
    if ( m ){
      os << right << setw(10) << i
	 << "  "
	 << left  << setw(20) << m->getName() 
	 << left  << setw(20) << m->getType() 
	 << right << setw(10) << endl;
    }
  }

  return os;
}

AFEB::teststand::Crate::Crate() : 
  modules_( vector<AFEB::teststand::Module*>( maxModules_ + 1 ) )
{
  vector<AFEB::teststand::Module*>::iterator m;
  for ( m = modules_.begin(); m != modules_.end(); ++m ){
    *m = NULL;
  }
}

AFEB::teststand::Crate::~Crate(){
  vector<AFEB::teststand::Module*>::iterator m;
  for ( m = modules_.begin(); m != modules_.end(); ++m ){
    delete *m;
  }
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
  modules_[slot]->setCrate( this );
}

void AFEB::teststand::Crate::insertController( AFEB::teststand::CrateController* controller, int slot ){
  modules_[slot] = controller;
  modules_[slot]->setSlot( slot );
  modules_[slot]->setCrate( this );
  controllerSlot_ = slot;
}
