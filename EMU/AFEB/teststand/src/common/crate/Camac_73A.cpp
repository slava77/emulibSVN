/*---------------------------------------------------------------------------
  This realisation of the CAMAC interface made for
  the Jorway 73A Crate Controller and work with Windows NT
  with the driver "sjy.sys"

  Aleksandr Golyash      email: golyash@fnal.gov      March 2001
  -------------------------------------------------------------------------*/

// #include <vcl.h>
// #pragma hdrstop

#include "Camac.h"
// #include "Jorway_73A.h"
#include "ieee_fun_types.h"
// #pragma package(smart_init)
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CCamac::CCamac ( int Branch )
  : branch ( Branch )
  , j_crate( 1 )
  , slot   ( 1 )
  , subad  ( 8 )
  , type   ( 1 )
  , dataw  ( 0 )
  , err    ( 0 )
  , route  ( 0 )
{}
//---------------------------------------------------------------------------
CCamac::CCamac ( crate crt )
  : branch ( 1 )
  , j_crate( 1 )
  , slot   ( 1 )
  , subad  ( 8 )
  , type   ( 1 )
  , dataw  ( 0 )
  , err    ( 0 )
  , route  ( 0 )
{
  // init_j73a();
}
//---------------------------------------------------------------------------
CCamac::~CCamac ( void )
{
  // end_j73a();
}
//---------------------------------------------------------------------------
void CCamac::write ( unsigned int d, subadd a, func f, stat n )
{
  dataw = static_cast<unsigned short>(d);
  cdreg ( &ext, branch, j_crate, n, a );
  cssa ( fun = f, ext, &dataw, qres );
}
//---------------------------------------------------------------------------
void CCamac::write ( unsigned int d )
{
  dataw = static_cast<unsigned short>(d);
  cssa ( fun, ext, &dataw, qres );
}
//---------------------------------------------------------------------------
void CCamac::write_bl ( unsigned int d, subadd a, func f, stat n )
{
  dataw = static_cast<unsigned short>(d);
  cdreg ( &ext, branch, j_crate, n, a );
  cssa ( fun = f, ext, &dataw, qres );
}
//---------------------------------------------------------------------------
unsigned int CCamac::read ( subadd a, func f, stat n )
{
  dataw = 0;
  cdreg ( &ext, branch, j_crate, n, a );
  cssa ( fun = f, ext, &dataw, qres );
  return  dataw;
}
//---------------------------------------------------------------------------
unsigned int CCamac::read ( void )
{
  dataw = 0;
  cssa ( fun, ext, &dataw, qres );
  return  dataw;
}
//---------------------------------------------------------------------------
void CCamac::read_block ( func f, unsigned short *data, int block_size )
{
  qres[0] = block_size;
  qres[3] = 0;
  csubc ( f, ext, &data[0], qres );
}
//---------------------------------------------------------------------------
void CCamac::execute ( subadd a, func f, stat n )
{
  cdreg ( &ext, branch, j_crate, n, a );
  cssa ( fun = f, ext, &dataw, qres );
}
//---------------------------------------------------------------------------
void CCamac::execute ( void )
{
  cssa ( fun, ext, &dataw, qres );
}
//---------------------------------------------------------------------------
bool CCamac::x ( void )
{
  return xrespn();
}
//---------------------------------------------------------------------------
bool CCamac::q ( void )
{
  return qrespn();
}
//---------------------------------------------------------------------------
void CCamac::def_crate ( crate crt ) // TODO: this should be branch (SCSI id + (8*SCSI bus))
{
  branch = crt;
}
//---------------------------------------------------------------------------
void CCamac::z ( void )
{
  cccz ( ext );
}
//---------------------------------------------------------------------------
void CCamac::c ( void )
{
  cccc ( ext );
}
//---------------------------------------------------------------------------
unsigned short CCamac::lam ( void )
{
  return 0;
}

