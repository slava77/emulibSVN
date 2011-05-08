//---------------------------------------------------------------------------
#ifndef CrateH
#define CrateH

#include "Le32.h"
#include "Tdc.h"
#include "Storage.h"
//---------------------------------------------------------------------------
namespace stand_crate
{
//---------------------------------------------------------------------------
static const int chs_chip = 16;
static const int chs_tdc = chs_chip * 4;
static const int time_max = 1024;
static const int del_max = 8;
//---------------------------------------------------------------------------
enum e_measure
  { count_vs_dac,   ocs_vs_dac,     time_vs_dac,      count_vs_time,
    time_vs_delay,  osc_vs_trh_up,  osc_vs_trh_down,  test_pulse,
    max_measurements };
//---------------------------------------------------------------------------
struct meas_param
{
  int condition;
  int dac_min; int dac_max;
  int step; int pulses;
  int time_min; int time_max;
  int n_tdc; int n_le32;
  int threshold; int n_trh;
  pchan pulse_gen;
};
//---------------------------------------------------------------------------
  class CCrate
  {
  private:

    short ampl, pls_max;
    short first, last, step;

    pchan mask_gen;

    short threshold, trhMin, trhMax, trhStep;
    short trh_station_number;
    short timeMin, timeMax;
    bool stop, next_dac;
    bool finished;

    CamTdc* Tdc;
    CLe32* Le32;

    e_measure measurement;

    CCamac camacController_;

  public:

    CCrate();
    virtual ~CCrate();

    static void SetDefaultParam ( meas_param& param );
    bool IsFinished ( void ) { return finished; }

    void Stop ( bool s = true ) { stop = s; }
    bool GetStop ( void ) { return stop; }
    int CurrentDac( void ) { return ampl; }
    bool Next ( void ){ return next_dac; }
    void ResetNext ( void ) { next_dac = false; }
    void SetPulseGen ( pchan generator )
        { Le32->enable_pulses ( generator ); }

    void set_param ( meas_param& param );

    void set_measurement ( e_measure msmn ) { measurement = msmn; }
    e_measure get_measurement ( void ) { return measurement; }

    void set_pulse_generator ( int min, int max, int stp, int pulses = 100 );
    void set_camac_stations ( int n_tdc, int n_le32 );
    void set_time_region ( int min, int max );

    void set_trh ( int trh, int min = 0, int max = 255, int trh_stp = 20 );

    // void ( __closure *pMeasure [max_measurements] )(void);

    void set_pls ( Storage<int,int>** stor )
      { pls[0] = stor[0]; pls[1] = stor[1];}
    void set_eff ( Storage<int,int>** stor )
      { pls[0] = stor[0]; pls[1] = stor[1];}
    void set_time ( Storage<int,float>** stor )
      { time[0] = stor[0]; time[1] = stor[1];}
    void set_rms ( Storage<int,float>** stor )
      { rms[0] = stor[0]; rms[1] = stor[1];}
    void set_distr ( Storage<float,int>** stor )
      { distr[0] = stor[0]; distr[1] = stor[1]; }

    Storage<int,int>**   get_pls   ( void ) { return pls; }
    Storage<int,int>**   get_eff   ( void ) { return pls; }
    Storage<int,float>** get_time  ( void ) { return time; }
    Storage<int,float>** get_rms   ( void ) { return rms; }
    Storage<float,int>** get_distr ( void ) { return distr; }

    void CountVsDac ( void );

  private:

    Storage<int,int>*   pls[2];
    Storage<int,float>* time[2];
    Storage<int,float>* rms[2];
    Storage<float,int>* distr[2];

    // void CountVsDac ( void ); // made public
    void OscVsDac ( void );
    void TimeVsDac ( void );
    void CountVsTime ( void );
    void TimeVsDelay ( void );
    void OscVsTrhUp ( void );
    void OscVsTrhDown ( void );
    void TestPulse ( void );

    bool SetModules ( void );
    int ChipNumber ( int& channel );
    void Write ( short _first, short _last, short _step, short pulses );
    void Read ( short& _first, short& _last, short& _step, short& pulses );
    void MeanTime( float* mean1, float* mean2, float* rms1, float* rms2 );
    void Oscillations ( int* osc1, int* osc2 );
  };
//---------------------------------------------------------------------------
} //end namespace stand_crate
using namespace stand_crate;

//extern CCrate* Crate;
//---------------------------------------------------------------------------
#endif
