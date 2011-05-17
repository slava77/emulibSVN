#ifndef __AFEB_teststand_Jorway73A_h__
#define __AFEB_teststand_Jorway73A_h__

#include "AFEB/teststand/CrateController.h"
#include "AFEB/teststand/CAMAC.h"

#include <string>

using namespace std;

namespace AFEB { namespace teststand {

    class Jorway73A : public CrateController, public CAMAC
    {      
    public:
      Jorway73A( const int branch, const int crateNumber ) :
	CrateController( "Jorway73A", crateNumber ),
	CAMAC( branch, crateNumber )
      {}

      // Implement CAMAC-type virtual methods of CrateController
      void z() const { CAMAC::z(); }
      void c() const { CAMAC::c(); }
      bool x() const { return CAMAC::x(); }
      bool q() const { return CAMAC::q(); }
      unsigned short lam() const { return CAMAC::lam(); }
      void write( const unsigned int data, 
		  const Subaddress_t subaddress, 
		  const Function_t function,
		  const Station_t station ) const { CAMAC::write( data, subaddress, function, station ); }
      unsigned int read( const Subaddress_t subaddress, 
			 const Function_t function,
			 const Station_t station ) const { return CAMAC::read( subaddress, function, station ); }
      void readBlock( const Subaddress_t subaddress, 
		      const Function_t function,
		      const Station_t station,
		      unsigned short *data,
		      const int blockSize ) const { CAMAC::readBlock( subaddress, function, station,data, blockSize ); }
      void execute( const Subaddress_t subaddress, 
		    const Function_t function,
		    const Station_t station ) const { CAMAC::execute( subaddress, function, station ); }
      
    };

  }}

#endif
