#include "AFEB/teststand/crate/CrateController.h"

// Generic virtual method to be implemented by any crate controller

void AFEB::teststand::CrateController::initialize() const {
  cerr << "AFEB::teststand::CrateController::initialize is not implemented."; return; 
}


// Virtual methods for CAMAC to be implemented in a derived CAMAC crate controller class:

void AFEB::teststand::CrateController::z() const {
  cerr << "AFEB::teststand::CrateController::z is not implemented."; return;
}

void AFEB::teststand::CrateController::c() const {
  cerr << "AFEB::teststand::CrateController::c is not implemented."; return;
}

bool AFEB::teststand::CrateController::x() const {
  cerr << "AFEB::teststand::CrateController::x is not implemented."; return false;
}

bool AFEB::teststand::CrateController::q() const {
  cerr << "AFEB::teststand::CrateController::q is not implemented."; return false;
}

unsigned short AFEB::teststand::CrateController::lam() const {
  cerr << "AFEB::teststand::CrateController::lam is not implemented."; return 0;
}

void AFEB::teststand::CrateController::write( const unsigned int data, 
		    const Subaddress_t subaddress, 
		    const Function_t function,
		    const Station_t station ) const {
  cerr << "AFEB::teststand::CrateController::write is not implemented."; return;
}

unsigned int AFEB::teststand::CrateController::read( const Subaddress_t subaddress, 
			   const Function_t function,
			   const Station_t station ) const {
  cerr << "AFEB::teststand::CrateController::read is not implemented."; return 0;
}

void AFEB::teststand::CrateController::readBlock( const Subaddress_t subaddress, 
			const Function_t function,
			const Station_t station,
			unsigned short *data,
			const int blockSize ) const {
  cerr << "AFEB::teststand::CrateController::readBlock is not implemented."; return;
}

void AFEB::teststand::CrateController::execute( const Subaddress_t subaddress, 
		      const Function_t function,
		      const Station_t station ) const {
  cerr << "AFEB::teststand::CrateController::execute is not implemented."; return;
}
