#include <iostream>
#include <iomanip>

#include "Crate.h"
#include "ieee_fun_types.h"
// #include "camac_globals.h"

using namespace std;

// extern "C" unsigned char* sjy_inquiry(int);

int main( int argc, char** argv ){

  int branch = 0;
  if ( argc == 2 ){
    branch = atoi( argv[1] );
  }
  cout << "Inquiring ID " << ID(branch)  << endl
       << "BUS "          << BUS(branch) << endl;
       

  cdchn( branch, 1, 0 );

  unsigned char *buf;
  buf = sjy_inquiry (branch);
  cout << "Buffer is at " << hex << buf << endl;
  cout << "Jorway Hardware Version is" << &buf[8] << endl;


  meas_param MeasurementParameters;
  MeasurementParameters.condition = 0;
  MeasurementParameters.dac_min   = 20;
  MeasurementParameters.dac_max   = 90;
  MeasurementParameters.step      = 2;
  MeasurementParameters.pulses    = 400;
  MeasurementParameters.time_min  = 0;
  MeasurementParameters.time_max  = 120;
  MeasurementParameters.n_tdc     = 7;
  MeasurementParameters.n_le32    = 9; // LE32 out ?
  MeasurementParameters.threshold = 50;
  MeasurementParameters.n_trh     = 9; // LE32 in ?
  MeasurementParameters.pulse_gen = Gen12;

  // OK?
  // MeasurementParameters.n_tdc     = 7;
  // MeasurementParameters.n_le32    = 9; // LE32 out ?
  // MeasurementParameters.n_trh     = 9; // LE32 in ?

  // OK?
  // MeasurementParameters.n_tdc     = 7;
  // MeasurementParameters.n_le32    = 9; // LE32 out ?
  // MeasurementParameters.n_trh     = 6; // LE32 in ?

  // All 0...
  // MeasurementParameters.n_tdc     = 7;
  // MeasurementParameters.n_le32    = 6; // LE32 out ?
  // MeasurementParameters.n_trh     = 9; // LE32 in ?

  CCrate *crate = new CCrate();
  crate->set_param( MeasurementParameters );


  cout << "Measurement: " << crate->get_measurement() << endl;;

  crate->CountVsDac();
  Storage<int,int>** pulses = crate->get_pls();
  pulses[0]->SaveFile("pulses0.txt");
  pulses[1]->SaveFile("pulses1.txt");


  return 0;
}
