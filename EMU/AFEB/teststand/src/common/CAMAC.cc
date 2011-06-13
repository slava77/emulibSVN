#include "AFEB/teststand/CAMAC.h"

#include "ieee_fun_types.h"

void AFEB::teststand::CAMAC::z() const{
  // We only need the branch and crate for cccz. Pick any valid station and subaddress to make ::cdreg happy.
  const unsigned int dummy = 1; // an arbitrary valid station and subaddress
  // Encode branch and crate in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, dummy, dummy );
  ::cccz( ext );
}

void AFEB::teststand::CAMAC::c() const{
  // We only need the branch and crate for cccc. Pick any valid station and subaddress to make ::cdreg happy.
  const unsigned int dummy = 1; // an arbitrary valid station and subaddress
  // Encode branch and crate in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, dummy, dummy );
  ::cccc( ext );
}

bool AFEB::teststand::CAMAC::x() const{
  return ::xrespn();
}

bool AFEB::teststand::CAMAC::q() const{
  return ::qrespn();
}

unsigned short AFEB::teststand::CAMAC::lam() const { // TODO: implement. (Needed?)
  return 0;
}

void AFEB::teststand::CAMAC::write( const unsigned int data, 
				    const Subaddress_t subaddress, 
				    const Function_t function,
				    const Station_t station ) const {
  unsigned short dataw = static_cast<unsigned short>(data);
  // Encode everything in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, station, subaddress );
  // Execute a single 16 bit data transfer function
  int qres[4];
  ::cssa( function, ext, &dataw, qres );
}

unsigned int AFEB::teststand::CAMAC::read( const Subaddress_t subaddress, 
					   const Function_t function,
					   const Station_t station ) const{
  unsigned short dataw = 0;
  // Encode full address in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, station, subaddress );
  // Execute a single 16 bit data transfer function
  int qres[4];
  ::cssa( function, ext, &dataw, qres );
  return dataw;
}

void AFEB::teststand::CAMAC::readBlock( const Subaddress_t subaddress, // TODO: is subaddress needed here?
					const Function_t function,
					const Station_t station,
					unsigned short *data,
					const int blockSize ) const{
  // Encode full address in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, station, subaddress );
  // Execute a single slot 16 bit "DMA" CAMAC operation
  int qres[4];
  qres[0] = blockSize;
  qres[3] = 0;
  ::csubc( function, ext, &data[0], qres );
}

void AFEB::teststand::CAMAC::execute( const Subaddress_t subaddress, 
				      const Function_t function,
				      const Station_t station ) const{
  // Encode full address in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, station, subaddress );
    // Execute a single 16 bit data transfer function
  unsigned short dataw = 0;
  int qres[4];
  ::cssa( function, ext, &dataw, qres );
}
