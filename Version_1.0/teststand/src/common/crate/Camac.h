/*---------------------------------------------------------------------------
  This is the additional interface to work with CAMAC
  -------------------------------------------------------------------------*/

#ifndef CamacH
#define CamacH
//---------------------------------------------------------------------------
namespace camac_controller {
//---------------------------------------------------------------------------
enum stat {
	N1 = 1,   N2,  N3,  N4,  N5,  N6,  N7,
	N8,  N9,  N10, N11, N12, N13, N14, N15,
	N16, N17, N18, N19, N20, N21, N22, N23 };

enum func {
	F0,  F1,  F2,  F3 , F4,  F5,  F6,  F7,
	F8,  F9,  F10, F11, F12, F13, F14, F15,
	F16, F17, F18, F19, F20, F21, F22, F23,
	F24, F25, F26, F27, F28, F29, F30, F31 };

enum subadd {
	A0,  A1,  A2,  A3,  A4,  A5,  A6,  A7,
	A8,  A9,  A10, A11, A12, A13, A14, A15 };

enum crate { C1 = 1 , C2 , C3 , C4 };

// const CamPort = 0x3A0;
//---------------------------------------------------------------------------
class CCamac
{

public:

  CCamac ( int Branch );
  CCamac ( crate crt = C1 );
  virtual ~CCamac();

  void z ( void );
  void c ( void );
  void def_crate ( crate crt );

  bool x ( void );
  bool q ( void );
  unsigned short lam ( void );

protected:

  void write ( unsigned int d, subadd a, func f, stat n );
  void write ( unsigned int d );
  void write_bl ( unsigned int d, subadd a, func f, stat n );

  unsigned int read ( subadd a, func f, stat n );
  unsigned int read ( void );
  void read_block ( func f, unsigned short *data, int block_size );

  void execute ( subadd a, func f, stat n );
  void execute ( void );

private:
  // From Jorway_73A.h
  int ext;
  int branch;
  int j_crate;
  int slot;
  int subad;
  int fun;
  int type;
  unsigned short dataw;
  int err;
  int qres[4];
  int route;
  int rdev;
  char rnode[80];

};
//---------------------------------------------------------------------------
} //end namespace camac_controller
using namespace camac_controller;
//---------------------------------------------------------------------------
#endif
