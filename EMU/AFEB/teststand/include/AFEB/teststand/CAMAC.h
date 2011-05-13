#ifndef __AFEB_teststand_CAMAC_h__
#define __AFEB_teststand_CAMAC_h__

#include <string>

using namespace std;

namespace AFEB { namespace teststand {

    class CAMAC{
    public:
      CAMAC( const int branch, const int crateNumber ) :
	branch_( branch ),
	crateNumber_( crateNumber ){}
      void z();
      void c();
      bool x();
      bool q();
      unsigned short lam();
    protected:
      void write( const unsigned int data, 
		  const unsigned int subaddress, 
		  const unsigned int function,
		  const unsigned int station );
      unsigned int read( const unsigned int subaddress, 
			 const unsigned int function,
			 const unsigned int station );
      void readBlock( const unsigned int subaddress, 
		      const unsigned int function,
		      const unsigned int station,
		      unsigned short *data,
		      const int blockSize );
      void execute( const unsigned int subaddress, 
		    const unsigned int function,
		    const unsigned int station );
    private:
      int branch_;
      int crateNumber_;
   };

  }}

#endif
