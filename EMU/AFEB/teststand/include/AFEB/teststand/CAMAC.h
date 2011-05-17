#ifndef __AFEB_teststand_CAMAC_h__
#define __AFEB_teststand_CAMAC_h__

#include <string>

using namespace std;

namespace AFEB { namespace teststand {

    enum Station_t {
      N1=1, N2,  N3,  N4,  N5,  N6,  N7, N8,
      N9,  N10, N11, N12, N13, N14, N15, N16, 
      N17, N18, N19, N20, N21, N22, N23, N24 };
    
    enum Function_t {
      F0,  F1,  F2,  F3 , F4,  F5,  F6,  F7,
      F8,  F9,  F10, F11, F12, F13, F14, F15,
      F16, F17, F18, F19, F20, F21, F22, F23,
      F24, F25, F26, F27, F28, F29, F30, F31 };
    
    enum Subaddress_t {
      A0,  A1,  A2,  A3,  A4,  A5,  A6,  A7,
      A8,  A9,  A10, A11, A12, A13, A14, A15 };
    
    enum Crate_t { C1 = 1 , C2 , C3 , C4 };

    class CAMAC{
    public:
      CAMAC( const int branch, const int crateNumber ) :
	branch_( branch ),
	crateNumber_( crateNumber ){}
      virtual ~CAMAC(){}
    protected:
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
      int branch_;
      int crateNumber_;
   };

  }}

#endif
