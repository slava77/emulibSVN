/*--------------------------------------------------
 *    LE32 - CAMAC 32 Channels Converter and
 *    2 Channels Test Generator
 *-------------------------------------------------*/

#ifndef Le32H
#define Le32H

#include "Camac.h"
//-----------------------------------------------------------
namespace le32_module {
//-----------------------------------------------------------
enum pchan
  { NoGen, Gen1,  Gen2, Gen12 };
enum esupply
  { Sup0, Sup1,  Sup2, Sup12 };
//-----------------------------------------------------------
class CLe32: public CCamac
{

public:

  CLe32 ( int branch, stat _s ) : CCamac( branch ),  station(_s) {};

  short station_number ( short _station );
  short station_number ( void );

  // static void module_off ( short _station );

  // static void write_ampl_in_module ( short _ampl, short _station );
  // static void write_trh_in_module ( short _trh, short _station );
  // static void disable_out_in_module ( short _station );
  // static void disable_power_in_module ( short _station );

  void module_off ( short _station );

  void write_ampl_in_module ( short _ampl, short _station );
  void write_trh_in_module ( short _trh, short _station );
  void disable_out_in_module ( short _station );
  void disable_power_in_module ( short _station );

  void write_ampl ( short _chip, short _ampl );
  void write_ampl ( short _ampl );
  void write_ampl ( void );

  void write_trh ( short _chip, short _trh );
  void write_trh ( short _trh );
  void write_trh ( void );

  void enable_pulses ( void );
  void enable_pulses ( pchan _pulse_channel );
  void enable_pulses ( pchan _pulse_channel, short _station );

  void enable_power ( void );
  void disable_power ( void );
  void enable_power ( esupply _supply );

  void exec ( void );
  bool  read_q ( void );
  short test_lam ( void );

private:

  stat station;
  pchan pulse_channel;
  esupply supply;
  short chip;
  short ampl[2];
  short trh[2];
};
//---------------------------------------------------------------------------
} //end namespace le32_module
using namespace le32_module;

//extern CLe32* Le32;
//-----------------------------------------------
#endif
