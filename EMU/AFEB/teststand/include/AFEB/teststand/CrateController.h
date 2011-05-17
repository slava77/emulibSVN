#ifndef __AFEB_teststand_CrateController_h__
#define __AFEB_teststand_CrateController_h__

#include "AFEB/teststand/Module.h"
#include "AFEB/teststand/CAMAC.h"

#include <string>
#include <iostream>

using namespace std;

namespace AFEB { namespace teststand {

    class CrateController : public Module
    {      
    public:
      CrateController( const string name, const int crateNumber )
	: Module( name, "CrateController" ),
	  crateNumber_( crateNumber ){}
      virtual ~CrateController(){}
      int getCrateNumber() const { return crateNumber_; }

      // Virtual methods for CAMAC to be implemented in a derived CAMAC crate controller class:
      virtual void z() const { cerr << "AFEB::teststand::CrateController::z is not implemented."; return; }
      virtual void c() const { cerr << "AFEB::teststand::CrateController::c is not implemented."; return; }
      virtual bool x() const { cerr << "AFEB::teststand::CrateController::x is not implemented."; return false; }
      virtual bool q() const { cerr << "AFEB::teststand::CrateController::q is not implemented."; return false; }
      virtual unsigned short lam() const { cerr << "AFEB::teststand::CrateController::lam is not implemented."; return 0; }
      virtual void write( const unsigned int data, 
			  const Subaddress_t subaddress, 
			  const Function_t function,
			  const Station_t station ) const { cerr << "AFEB::teststand::CrateController::write is not implemented."; return; }
      virtual unsigned int read( const Subaddress_t subaddress, 
				 const Function_t function,
				 const Station_t station ) const { cerr << "AFEB::teststand::CrateController::read is not implemented."; return 0; }
      virtual void readBlock( const Subaddress_t subaddress, 
			      const Function_t function,
			      const Station_t station,
			      unsigned short *data,
			      const int blockSize ) const { cerr << "AFEB::teststand::CrateController::readBlock is not implemented."; return; }
      virtual void execute( const Subaddress_t subaddress, 
			    const Function_t function,
			    const Station_t station ) const { cerr << "AFEB::teststand::CrateController::execute is not implemented."; return; }
      
    protected:
      int crateNumber_;

    };

  }}

#endif
