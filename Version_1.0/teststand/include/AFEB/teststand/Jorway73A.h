#ifndef __AFEB_teststand_Jorway73A_h__
#define __AFEB_teststand_Jorway73A_h__

#include "AFEB/teststand/CrateController.h"
#include "AFEB/teststand/CAMAC.h"

#include <string>

using namespace std;

namespace AFEB { namespace teststand {

    class Jorway73A : public CrateController
    {      
    public:
      Jorway73A( const string id, const int crateNumber );
      int getBranch() const { return branch_; }

      // Implement generic virtual methods of CrateController

      void initialize() const;

      
      // Implement CAMAC-type virtual methods of CrateController

      void z() const;
      void c() const;
      bool x() const;
      bool q() const;
      unsigned short lam() const;
      void write( const unsigned int data, 
		  const Subaddress_t subaddress, 
		  const Function_t function,
		  const Station_t station ) const;
      unsigned int read( const Subaddress_t subaddress, 
			 const Function_t function,
			 const Station_t station ) const;
      void readBlock( const Subaddress_t subaddress, 
		      const Function_t function,
		      const Station_t station,
		      unsigned short *data,
		      const int blockSize ) const;
      void execute( const Subaddress_t subaddress, 
		    const Function_t function,
		    const Station_t station ) const;

    private:
      int findBranch();
      int branch_;
    };

  }}

#endif
