//---------------------------------------------------------------------------

#include <math.h>
#include <string.h>
#include <fstream.h>
#include "Crate.h"

//---------------------------------------------------------------------------

//CCrate* Crate = new CCrate;
//---------------------------------------------------------------------------
CCrate::CCrate ( void )
{
  first = 0; last = 1024; step = 25; pls_max = 100;
  timeMin = 140; timeMax = 320;
  threshold = 100;
  trhMin = 0; trhMax = 255; trhStep = 20;
  stop = false;
  next_dac = true;
  finished = true;

  mask_gen = Gen12;

  Le32 = new CLe32  ( int(16), N9 ); // TODO: add branch
  Tdc  = new CamTdc ( int(16), N7 ); // TODO: get branch dynamically

  pls[0]   = pls[1]  = 0;
  time[0]  = time[1]  = 0;
  rms[0]   = rms[1]   = 0;
  distr[0] = distr[1] = 0;

  measurement = count_vs_dac;

  // pMeasure [count_vs_dac]    = CountVsDac;
  // pMeasure [ocs_vs_dac]      = OscVsDac;
  // pMeasure [time_vs_dac]     = TimeVsDac;
  // pMeasure [count_vs_time]   = CountVsTime;
  // pMeasure [time_vs_delay]   = TimeVsDelay;
  // pMeasure [osc_vs_trh_up]   = OscVsTrhUp;
  // pMeasure [osc_vs_trh_down] = OscVsTrhDown;
  // pMeasure [test_pulse]      = TestPulse;
}
//---------------------------------------------------------------------------
CCrate::~CCrate ( void )
{
  if ( Le32 ) delete Le32;
  if ( Tdc  ) delete Tdc;
}
//---------------------------------------------------------------------------

void CCrate::SetDefaultParam ( meas_param& param )
{
  param.condition = 0;
  param.dac_min = 0; param.dac_max = 1000;
  param.step = 20; param.pulses = 100;
  param.time_min = 80; param.time_max = 160;
  param.n_tdc = 1; param.n_le32 = 6;
  param.threshold = 100; param.n_trh = 6;
  param.pulse_gen = Gen12;
}
//---------------------------------------------------------------------------

bool CCrate::SetModules()
{
  // CCamac::z();
  // CCamac::def_crate( C1 );

  // TODO: Needed?
  // camacController_.z();
  // camacController_.def_crate( C1 );

  // Tdc::Set ( Mode, Shift, Hit, Edge, Mpi, TimeOut, TimeEnf )
  Tdc->Set ( M1, 0, 2, 0, 0, 550, 511 );

  return true;
}

int CCrate::ChipNumber ( int& channel )
{
  int chip = ( channel < chs_chip ) ? 1 : 2 ;
  if ( chip == 2 ) channel -= chs_chip;
  return chip;
}

//---------------------------------------------------------------------------

void CCrate::set_pulse_generator ( int min, int max, int stp, int pulses )
{
  first = short ( min ); last = short ( max );
  step = short ( stp ); pls_max = short ( pulses );
}
//---------------------------------------------------------------------------

void CCrate::set_time_region ( int min, int max )
{
  timeMin = short ( 2 * min ) ;
  timeMax = short ( 2 * max ) ;
}
//---------------------------------------------------------------------------

void CCrate::set_camac_stations ( int n_tdc, int n_le32 )
{
  Tdc->station_number ( static_cast<short>(n_tdc) );
  Le32->station_number ( static_cast<short>(n_le32) );
}
//---------------------------------------------------------------------------

void CCrate::set_trh ( int trh, int min, int max, int trh_stp )
{
  threshold = short ( trh ); trhMin = short ( min );
  trhMax = short ( max ); trhStep = short ( trh_stp );
}
//---------------------------------------------------------------------------

void CCrate::set_param ( meas_param& param )
{
  first   = static_cast<short>( param.dac_min );
  last    = static_cast<short>( param.dac_max );
  step    = static_cast<short>( param.step );
  pls_max = static_cast<short>( param.pulses );
  timeMin = static_cast<short>( 2 * param.time_min );
  timeMax = static_cast<short>( 2 * param.time_max );
  threshold = static_cast<short>( param.threshold );
  Tdc->station_number  ( static_cast<short>( param.n_tdc ) );
  Le32->station_number ( static_cast<short>( param.n_le32 ) );
  mask_gen = param.pulse_gen;
  trh_station_number = static_cast<short>( param.n_trh );
}
//---------- COUNT OF THE PULSES VS TEST PULSE DAC --------------------------

void CCrate::CountVsDac()
{
  finished = false;

  int chn, n_pls, channel, i;
  int count1[chs_chip], count2[chs_chip];

  SetModules();
  for ( i = 0; i < 2; i++ ){
    if ( pls[i] ){ 
      pls[i]->erase();
    }
    else{
      pls[i] = new Storage<int,int>();
    } 
  }

  Le32->write_trh_in_module ( threshold, trh_station_number );
  Le32->write_ampl_in_module ( 0, trh_station_number );
  Le32->enable_pulses ( NoGen, trh_station_number );
    
  Le32->write_ampl ( first );

  for( ampl = first; ampl <= last; ampl += step )
  {
    if ( stop ) return;

    for ( chn = 0; chn < chs_chip; chn++ )
      { count1[chn] = 0; count2[chn] = 0; }

    Le32->write_ampl ( ampl );
    Le32->enable_pulses ( mask_gen );
    Le32->exec();
    Tdc->Clear();
    Tdc->EnableAcq();

    for ( n_pls = 0; n_pls < pls_max; n_pls++ )
    {
      if ( stop ) return;
      Tdc->Clear();
      Le32->exec();

      // Tdc->HeadRd();
      Tdc->HeadRdBlock();

      for ( chn = 0; chn < chs_tdc; chn++ )
      {
        // if ( !Tdc->DataRd() ) break;
        if ( !Tdc->BlockRd ( chn ) ) break;
        if ( ( Tdc->TimeChRd() >= timeMin ) && ( Tdc->TimeChRd() <= timeMax ) )
          ChipNumber ( channel = Tdc->ChannelRd() ) == 1
            ? count1 [ channel ] ++ : count2 [ channel ] ++ ;
      }
    }
    pls[0]->put ( ampl, count1 );
    pls[1]->put ( ampl, count2 );
    next_dac = true;
  }
  finished = true;
}

//-------- OSCILLATIONS , CALCULATION --------------------------------------

void CCrate::Oscillations( int* osc1, int* osc2 )
{
  int chn, n_pls, channel;

  for ( chn = 0; chn < chs_chip; chn++ )
    { osc1[chn] = 0; osc2[chn] = 0; }

  Le32->write_ampl( ampl );
  for ( n_pls = 0; n_pls < pls_max; n_pls++ )
  {
    if ( stop ) return;
    Tdc->Clear(); Tdc->EnableAcq();
    Le32->exec();
    Tdc->HeadRd();
    for ( chn = 0; chn < chs_tdc; chn++ )
    {
      if ( !Tdc->DataRd() ) break;
      if ( ( Tdc->TimeChRd() > timeMax ) || ( Tdc->TimeChRd() < timeMin ) )
        ( ChipNumber ( channel = Tdc->ChannelRd() ) == 1 )
          ? osc1 [ channel ] ++ : osc2 [ channel ] ++ ;
    }
  }
}

//----------- OSCILLATIONS VS DAC ------------------------------------------

void CCrate::OscVsDac()
{
  int oscil1[chs_chip], oscil2[chs_chip];
  int chn, sum1, sum2;

  SetModules();
  pls[0]->erase();
  pls[1]->erase();

  Le32->write_trh ( threshold );
  for( ampl = first; ampl <= last; ampl += step )
  {
    if ( stop ) return;
    Oscillations( oscil1, oscil2 );
    for ( chn = 0, sum1 =0, sum2 = 0; chn < chs_chip; chn++ )
      { sum1 += oscil1[chn]; sum2 += oscil2[chn]; }
    if ( sum1 ) pls[0]->put ( ampl, oscil1 );
    if ( sum2 ) pls[1]->put ( ampl, oscil2 );
  }
}

//---------- OSCILATIONS VS THRESHOLD WHEN IT GO UP -------------------------

void CCrate::OscVsTrhUp()
{
  int oscil1[chs_chip];
  int oscil2[chs_chip];

  SetModules();
  pls[0]->erase();
  pls[1]->erase();

  for( short trh = trhMin; trh <= trhMax; trh += trhStep )
  {
    if ( stop ) return;
    Le32->write_trh( trh );
    Oscillations( oscil1, oscil2 );
    pls[0]->put ( trh, oscil1 );
    pls[1]->put ( trh, oscil2 );
  }
}

//---------- OSCILATIONS VS THRESHOLD WHEN IT GO DOWN -----------------------

void CCrate::OscVsTrhDown()
{
  int oscil1[chs_chip];
  int oscil2[chs_chip];

  SetModules();
  pls[0]->erase();
  pls[1]->erase();

  for( short trh = trhMax; trh >= trhMin; trh -= trhStep )
  {
    if ( stop ) return;
    Le32->write_trh( trh );
    Oscillations( oscil1, oscil2 );
    pls[0]->put ( trh, oscil1 );
    pls[1]->put ( trh, oscil2 );
  }
}

//---------- MEAN TIME AND RMS , CALCULATION --------------------------------

void CCrate::MeanTime( float* mean1, float* mean2, float* rms1, float* rms2 )
{
  int count1[time_max][chs_chip];
  int count2[time_max][chs_chip];
  int chn, tm, n_pls, channel;
  long pulses1, pulses2;
  double time1,time2, rmsTime1,rmsTime2;
  float shift = Tdc->ShiftTime();

  timeMin = ( timeMin < 0 ) ? short(0) :
    ( timeMin >= time_max ) ? short( time_max - 1 ) : timeMin ;
  timeMax = ( timeMax < 0 ) ? short(0) :
    ( timeMax >= time_max ) ? short( time_max - 1 ) : timeMax ;

  for ( chn = 0; chn < chs_chip; chn++ )
     for ( tm = 0; tm < time_max; tm++ )
      { count1[tm][chn] = 0; count2[tm][chn] = 0; }

  for ( n_pls=0; n_pls < pls_max; n_pls++ )
  {
    if ( stop ) return;
    Tdc->Clear();
    Le32->exec();

    // Tdc->HeadRd();
    Tdc->HeadRdBlock();

    for ( chn = 0; chn < chs_tdc; chn++ )
    {
      // if ( !Tdc->DataRd() ) break;
      if ( !Tdc->BlockRd ( chn ) ) break;
      ( ChipNumber ( channel = Tdc->ChannelRd() ) == 1 )
        ? count1[Tdc->TimeChRd()][channel]++
        : count2[Tdc->TimeChRd()][channel]++ ;
    }
  }

  for ( chn = 0; chn < chs_chip; chn++ )
  {
    pulses1 = 0; time1 = 0; rmsTime1 = 0;
    pulses2 = 0; time2 = 0; rmsTime2 = 0;

    for ( tm = timeMin; tm <= timeMax; tm ++ )
    {
      time1 += count1[tm][chn] * tm;
      time2 += count2[tm][chn] * tm;
      pulses1 += count1[tm][chn];
      pulses2 += count2[tm][chn];
    }
    mean1[chn] = pulses1 ? ( time1 * shift / pulses1 ) : 0;
    mean2[chn] = pulses2 ? ( time2 * shift / pulses2 ) : 0;

    for ( tm = timeMin; tm <= timeMax; tm ++ )
    {
      rmsTime1 += ( mean1[chn] - tm * shift )
                * ( mean1[chn] - tm * shift ) * count1[tm][chn];

      rmsTime2 += ( mean2[chn] - tm * shift )
                * ( mean2[chn] - tm * shift ) * count2[tm][chn];
    }
    rms1[chn] =  pulses1 ? sqrt( rmsTime1 / pulses1 ) : 0;
    rms2[chn] =  pulses2 ? sqrt( rmsTime2 / pulses2 ) : 0;
  }
}

//-------- MEAN TIME VS DAC ------------------------------------------------

void CCrate::TimeVsDac()
{
  float time1[chs_chip], rms1[chs_chip];
  float time2[chs_chip], rms2[chs_chip];

  SetModules();
  time[0]->erase();
  time[1]->erase();
  rms[0]->erase();
  rms[1]->erase();

  Le32->write_trh_in_module ( threshold, trh_station_number );
  Le32->write_ampl_in_module ( 0, trh_station_number );
  Le32->enable_pulses ( NoGen, trh_station_number );
    
  Le32->write_ampl ( first );

  for( ampl = first; ampl <= last; ampl += step )
	{
    if ( stop ) return;

    Le32->write_ampl ( ampl );
    Le32->enable_pulses ( mask_gen );
    Le32->exec();
    Tdc->Clear();
    Tdc->EnableAcq();

    MeanTime ( time1, time2, rms1, rms2 );

    time[0]->put ( ampl, time1 );
    time[1]->put ( ampl, time2 );
    rms[0]->put ( ampl, rms1 );
    rms[1]->put ( ampl, rms2 );

    next_dac = true;
	}
}

//-------- TIME VS DELAY CODE -----------------------------------------------

void CCrate::TimeVsDelay()
{
  float time1[chs_chip], time2[chs_chip];
  float rms1[chs_chip], rms2[chs_chip];
  int del;

  SetModules();
  time[0]->erase();
  time[1]->erase();

  Le32->write_trh( threshold );
  Le32->write_ampl( ampl );
  for( del = 0; del < del_max; del ++ )
	{
    if ( stop ) return;
    MeanTime ( time1, time2, rms1, rms2 );
    time[0]->put ( ampl, time1 );
    time[1]->put ( ampl, time2 );
	}
}

//-------- COUNT OF THE PULSES VS TIME -------------------------------------

void CCrate::CountVsTime()
{
  finished = false;

  int count1[time_max][chs_chip];
  int count2[time_max][chs_chip];
  int sum1, sum2, n_pls, chn, tm, channel;
  float shift = Tdc->ShiftTime();

  SetModules();
  distr[0]->erase();
  distr[1]->erase();
  //...............................................
  for ( chn = 0; chn < chs_chip; chn++ )
    for ( tm = 0; tm < time_max; tm++ )
      { count1[tm][chn] = 0; count2[tm][chn] = 0; }

  Le32->write_trh( threshold );
  Le32->write_ampl( ampl );
  for ( n_pls=0; n_pls < pls_max; n_pls++ )
  {
    if ( stop ) return;

    Tdc->Clear();
    Le32->exec();

    Tdc->HeadRd();

    for ( chn = 0; chn < chs_tdc; chn++ )
    {
      if ( !Tdc->DataRd() ) break;
      ( ChipNumber ( channel = Tdc->ChannelRd() ) == 1 )
        ? count1 [Tdc->TimeChRd()][channel] ++
        : count2 [Tdc->TimeChRd()][channel] ++ ;
    }
  }

  for ( tm = 0; tm < time_max; tm++ )
  {
    for ( chn = 0, sum1 = 0, sum2 = 0; chn < chs_chip; chn++ )
      { sum1 += count1[tm][chn]; sum2 += count2[tm][chn]; }
    if ( sum1 ) distr[0]->put ( tm * shift, count1[tm] );
    if ( sum2 ) distr[1]->put ( tm * shift, count2[tm] );
  }
  
  finished = true;
}

//---------------- TEST -----------------------------------------------------

void CCrate::TestPulse()
{
  unsigned long int i, nn = 1000000 ;
  SetModules();
  Le32->write_trh( threshold );
  Le32->write_ampl( ampl );
  for( i=0; i<nn; i++ ) Le32->exec();
}

//---------------------------------------------------------------------------

void CCrate::Write ( short _first, short _last, short _step, short pulses )
{
  first = _first;
  last = _last;
  step = _step;
  pls_max = pulses;
}

//---------------------------------------------------------------------------

void CCrate::Read ( short& _first, short& _last, short& _step, short& pulses )
{
  _first = first;
  _last = last;
  _step = step;
  pulses = pls_max;
}

//---------------------------------------------------------------------------

