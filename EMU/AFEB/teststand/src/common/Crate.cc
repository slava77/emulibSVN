#include "AFEB/teststand/Crate.h"

void AFEB::teststand::Crate::insert( AFEB::teststand::Module* module, int slot ){
  modules_[slot] = module;
  modules_[slot]->setSlot( slot );
}

void AFEB::teststand::Crate::insertController( AFEB::teststand::Module* controller, int slot ){
  modules_[slot] = controller;
  modules_[slot]->setSlot( slot );
  controllerSlot_ = slot;
}
