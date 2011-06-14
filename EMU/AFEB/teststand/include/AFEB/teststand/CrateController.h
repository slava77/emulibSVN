#ifndef __AFEB_teststand_CrateController_h__
#define __AFEB_teststand_CrateController_h__

#include "AFEB/teststand/Module.h"
#include "AFEB/teststand/CAMAC.h"

#include <string>
#include <iostream>

using namespace std;
using namespace CAMAC;

namespace AFEB { namespace teststand {

    class CrateController : public Module
    {      
    public:
      CrateController( const string name, const int crateNumber )
	: Module( name, "CrateController" ),
	  crateNumber_( crateNumber ){}
      virtual ~CrateController(){}
      int getCrateNumber() const { return crateNumber_; }

      // Generic virtual method to be implemented by any crate controller:

      virtual void initialize() const;

      // Virtual methods for CAMAC to be implemented in a derived CAMAC crate controller class:

      virtual void z() const;
      virtual void c() const;
      virtual bool x() const;
      virtual bool q() const;
      virtual unsigned short lam() const;
      virtual void write( const unsigned int data, 
      			  const Subaddress_t subaddress, 
      			  const Function_t function,
      			  const Station_t station ) const;
      virtual unsigned int read( const Subaddress_t subaddress, 
      				 const Function_t function,
      				 const Station_t station ) const;
      virtual void readBlock( const Subaddress_t subaddress, 
      			      const Function_t function,
      			      const Station_t station,
      			      unsigned short *data,
      			      const int blockSize ) const;
      virtual void execute( const Subaddress_t subaddress, 
      			    const Function_t function,
      			    const Station_t station ) const;

    protected:
      int crateNumber_;

    };

  }}

#endif
