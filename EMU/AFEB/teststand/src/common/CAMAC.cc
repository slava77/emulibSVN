#include "AFEB/teststand/CAMAC.h"

#include "ieee_fun_types.h"

void AFEB::teststand::CAMAC::z(){
  // We only need the branch and crate for cccz
  const unsigned int dummy = 0;
  // Encode branch and crate in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, dummy, dummy );
  ::cccz( ext );
}

void AFEB::teststand::CAMAC::c(){
  // We only need the branch and crate for cccz
  const unsigned int dummy = 0;
  // Encode branch and crate in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, dummy, dummy );
  ::cccc( ext );
}

bool AFEB::teststand::CAMAC::x(){
  return ::xrespn();
}

bool AFEB::teststand::CAMAC::q(){
  return ::qrespn();
}

unsigned short AFEB::teststand::CAMAC::lam(){
  return 0;
}

void AFEB::teststand::CAMAC::write( const unsigned int data, 
				    const unsigned int subaddress, 
				    const unsigned int function,
				    const unsigned int station ){
  unsigned short dataw = static_cast<unsigned short>(data);
  // Encode everything in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, station, subaddress );
  // Execute a single 16 bit data transfer function
  int qres[4];
  ::cssa( function, ext, &dataw, qres );
}

unsigned int AFEB::teststand::CAMAC::read( const unsigned int subaddress, 
					   const unsigned int function,
					   const unsigned int station ){
  unsigned short dataw = 0;
  // Encode full address in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, station, subaddress );
  // Execute a single 16 bit data transfer function
  int qres[4];
  ::cssa( function, ext, &dataw, qres );
  return dataw;
}

void AFEB::teststand::CAMAC::readBlock( const unsigned int subaddress, // TODO: is subaddress needed here?
					const unsigned int function,
					const unsigned int station,
					unsigned short *data,
					const int blockSize ){
  // Encode full address in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, station, subaddress );
  // Execute a single slot 16 bit "DMA" CAMAC operation
  int qres[4];
  qres[0] = blockSize;
  qres[3] = 0;
  ::csubc( function, ext, &data[0], qres );
}

void AFEB::teststand::CAMAC::execute( const unsigned int subaddress, 
				      const unsigned int function,
				      const unsigned int station ){
  // Encode full address in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, station, subaddress );
    // Execute a single 16 bit data transfer function
  unsigned short dataw = 0;
  int qres[4];
  ::cssa( function, ext, &dataw, qres );
}
