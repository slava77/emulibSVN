/*--------------------------------------------------
 *    LE32 - CAMAC 32 Channels Converter and
 *    2 Channels Test Generator
 *-------------------------------------------------*/

#include "Camac.h"
#include "Le32.h"

//-----------------------------------------------------------

//CLe32* Le32 = new CLe32 ( N6 );
//-----------------------------------------------------------

short CLe32::station_number ( short _station )
{
  return static_cast<short>
    ( station = static_cast<stat>(_station) );
}

short CLe32::station_number ( void )
  { return static_cast<short>(station); }

//-----------------------------------------------------------

void CLe32::module_off ( short _station )
{
  write_ampl_in_module ( 0, _station );
  write_trh_in_module ( 255, _station );
  disable_out_in_module ( _station );
  disable_power_in_module ( _station );
}
//-----------------------------------------------------------

void CLe32::write_ampl ( short _chip, short _ampl )
{
  if ( ( _chip==1 ) || ( _chip==2 ) )
  {
    chip = _chip;
    ampl[chip-1] = _ampl;
    write ( _ampl, static_cast<subadd>(chip-1), F16, station );
  }
}

void CLe32::write_ampl ( short _ampl )
{
  write_ampl ( 1, _ampl );
  write_ampl ( 2, _ampl );
}

void CLe32::write_ampl ( void )
{
  write_ampl ( 1, ampl[0] );
  write_ampl ( 2, ampl[1] );
}

void CLe32::write_ampl_in_module ( short _ampl, short _station )
{
  write( _ampl, static_cast <subadd> (0),
          F16, static_cast <stat> ( _station ) );
  write( _ampl, static_cast <subadd> (1),
          F16, static_cast <stat> ( _station ) );
}
//-----------------------------------------------------------

void CLe32::write_trh ( short _chip, short _trh )
{
  if ( ( _chip==1 ) || ( _chip==2 ) )
  {
    chip = _chip;
    trh[chip-1] = _trh;
    write( _trh, static_cast<subadd>(chip-1), F17, station );
  }
}

void CLe32::write_trh ( short _trh )
{
  write_trh ( 1, _trh );
  write_trh ( 2, _trh );
}

void CLe32::write_trh ( void )
{
  write_trh ( 1, trh[0] );
  write_trh ( 2, trh[1] );
}

void CLe32::write_trh_in_module ( short _trh, short _station )
{
  write( _trh, static_cast <subadd> (0),
          F17, static_cast <stat> ( _station ) );
  write( _trh, static_cast <subadd> (1),
          F17, static_cast <stat> ( _station ) );
}
//-----------------------------------------------------------

void CLe32::enable_pulses ( pchan _pulse_channel )
	{ write ( _pulse_channel, A4, F16, station ); }

void CLe32::enable_pulses ( pchan _pulse_channel, short _station )
	{ write ( _pulse_channel, A4, F16, static_cast <stat> ( _station ) ); }

void CLe32::enable_pulses ( void )
  { enable_pulses ( Gen12 ); }

void CLe32::disable_out_in_module ( short _station )
	{ write ( NoGen, A4, F16, static_cast <stat> ( _station ) ); }
//-----------------------------------------------------------

void CLe32::enable_power ( esupply _supply )
	{ write ( _supply, A5, F16, station ); }

void CLe32::enable_power ( void )
  { enable_power ( Sup12 ); }

void CLe32::disable_power ( void )
  { enable_power ( Sup0 ); }

void CLe32::disable_power_in_module ( short _station )
	{ write ( Sup0, A5, F16, static_cast <stat> ( _station ) ); }
//-----------------------------------------------------------

void CLe32::exec ( void )
	{ CCamac::execute ( A0, F25, station ); }

//-----------------------------------------------------------

bool CLe32::read_q ( void )
	{ return q(); }
//-----------------------------------------------------------

short CLe32::test_lam ( void )
	{
	CCamac::execute ( A0, F25, station );
	if ( lam() == static_cast<short>(station) )
    return station;
	else return 0;
  }
//-----------------------------------------------------------

