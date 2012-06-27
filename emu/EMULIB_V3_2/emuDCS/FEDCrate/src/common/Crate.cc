#include "Crate.h"
#include "VMEModule.h"
#include "VMEController.h"
#include "CrateSetup.h"
#include "Singleton.h"
#include "DDU.h"
#include "DCC.h"

int irqprob;
long int timer,xtimer;

Crate::Crate(int number, VMEController *theController):
	theNumber(number),  
	theModules(31),
	theController(NULL)
{
	Singleton<CrateSetup>::instance()->addCrate(number, this);
}


Crate::~Crate() {
	for(unsigned i = 0; i < theModules.size(); ++i) {
		delete theModules[i];
	}
	delete theController;
}


void Crate::addModule(VMEModule *module) {
	module->setController(theController);
	theModules[module->slot()] = module;
}

void Crate::setController(VMEController *controller) {
	if (theController != NULL) {
		cout << "WARNING: Trying change the VMEController of crate " << theNumber << endl;
	}
	cout << "Setting controller in crate " << theNumber << endl;
	theController = controller;
	theController->setCrate(theNumber);
	for (int i=0; i<theModules.size(); i++) {
		if (theModules[i] == NULL) continue;
		theModules[i]->setController(theController);
	}
}

vector<DDU *> Crate::ddus() const {
  vector<DDU *> result;
  for(unsigned i = 0; i < theModules.size(); ++i) {
    DDU * ddu = dynamic_cast<DDU *>(theModules[i]);
    if(ddu != 0) result.push_back(ddu);
  }
  return result;
}

vector<DCC *> Crate::dccs() const {
  vector<DCC *> result;
  for(unsigned i = 0; i < theModules.size(); ++i) {
    DCC * dcc = dynamic_cast<DCC *>(theModules[i]);
    if(dcc != 0) result.push_back(dcc);
  }
  return result;
}

void Crate::enable() {
  //
  std::cout << "Crate::enable called " << std::endl;
}

//
void Crate::disable() {
  //
  std::cout << "Crate::disable called " << std::endl;
  //
}
//
void Crate::configure(long unsigned int runnumber) {
// JRG, downloads to all boards, then starts the IRQ handler.
	printf(" ********   Crate::configure is called with run number %d \n",runnumber);
	std::vector<DDU*> myDdus = this->ddus();
	for(unsigned i =0; i < myDdus.size(); ++i) {
		myDdus[i]->configure();
	}
	std::vector<DCC*> myDccs = this->dccs();
	for(unsigned i =0; i < myDccs.size(); ++i) {
		myDccs[i]->configure(); 
	}

// LSD, move IRQ start to Init phase:
// JRG, we probably want to keep IRQ clear/reset here (End, then Start):
// PGK, new objects (IRQThread) in town.  Use these instead.
// PGK, better yet, just call init.
	cout << " ********   Crate::configure complete, running init..." << endl;
	this->init(runnumber);
}

void Crate::init(long unsigned int runnumber) {
// PGK, new objects (IRQThread) in town.  Use these instead.
	cout << " Crate::init: theController->start_thread_on_init="<<theController->start_thread_on_init<<".  Calling thread end" << endl;
	theController->end_thread();
	if(theController->start_thread_on_init){
		cout << " Crate::init: theController->start_thread_on_init="<<theController->start_thread_on_init<<".  Calling thread start" << endl;
		theController->start_thread(runnumber);
	}
}
