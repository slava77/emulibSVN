#include "AFEB/teststand/CAMAC.h"

#include "ieee_fun_types.h"

void AFEB::teststand::CAMAC::z(){
}

void AFEB::teststand::CAMAC::c(){
}

bool AFEB::teststand::CAMAC::x(){
}

bool AFEB::teststand::CAMAC::q(){
}

unsigned short AFEB::teststand::CAMAC::lam(){
}

void AFEB::teststand::CAMAC::write( const unsigned int data, 
				    const unsigned int subaddress, 
				    const unsigned int function,
				    const unsigned int station ){
}

unsigned int AFEB::teststand::CAMAC::read( const unsigned int subaddress, 
					   const unsigned int function,
					   const unsigned int station ){
}

void AFEB::teststand::CAMAC::readBlock( const unsigned int function,
					unsigned short *data,
					const int blockSize ){
}

void AFEB::teststand::CAMAC::execute( const unsigned int subaddress, 
				      const unsigned int function,
				      const unsigned int station ){
}
