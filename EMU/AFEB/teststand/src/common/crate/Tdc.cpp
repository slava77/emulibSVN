/*-------------------------------------------
		TDC - LeCroy 3377
    See operator's manual
    "Model 3377 32 Channel CAMAC TDC"
--------------------------------------------*/

#include "Camac.h"
#include "Tdc.h"
#include "fstream.h"
// #include <vcl>
//-----------------------------------------------------------
// CamTdc* Tdc = new CamTdc ( N1 );
// int last_ch = -1;
//-----------------------------------------------------------

CamTdc::CamTdc ( int branch, stat s ) : CCamac( branch )
{
  Station = s;
  Mode = M1; Shift = 0; Hit = 1;
  Edge = 0; Mpi = 0;
  TimeOut = 550; TimeEnf = 511;
}
//-----------------------------------------------------------
short CamTdc::station_number ( short station )
{
  return static_cast<short>
    ( Station = static_cast<stat>(station) );
}

short CamTdc::station_number ( void )
  { return static_cast<short>(Station); }

/*-----  SET THE MODULE  ----------------------------------------*/

bool CamTdc::Set ( TdcMode mode, short shift, short hit,
                   short edge, int mpi, int time_out, int time_enf )
{
  if ( ModeRd() != mode )
    ModeWr( mode ); // M1

  if ( ModeRd() != mode )
    return false;

  HitWr( hit );// 2
  if ( HitRd() != hit ) return false;

  ShiftWr( shift );// 0
  if ( ShiftRd() != shift ) return false;

  EdgeWr( edge );// 0
  if ( EdgeRd() != edge ) return false;

  ToutWr( time_out );// 550

  TenfWr( time_enf );// 511
  Clear();
  EnableAcq();
  return true;
}

/*----  DATA READ AND CLEAR  ---------------*/

bool CamTdc::HeadRd()
  { return DataHead( read ( A0, F0, Station) ); }

bool CamTdc::HeadRdBlock()
{
  BlockClear();
  if ( DataHead( read ( A0, F0, Station) ) )
  {
    read_block ( F0, Data, chs_tdc );
    return true;
  }
  else return false;//ret;
}

bool CamTdc::DataHead( int data )
{
  Edge =  static_cast<short>( (data & 0x0400) >> 10 );
  Shift = static_cast<short>( (data & 0x0300) >> 8 );
  Single = ( ~data & 0x4000 ) >> 14;
  return ( data & 0x8000 );
}
//-----------------------------------------------------------
void CamTdc::BlockClear ( void )
{
  for ( int ch = 0; ch < chs_tdc; ch++ )
    Data[ch] = 0;
}

bool CamTdc::BlockRd ( int ch )
{
  static int last_channel = 0;
  static int last_timech ;
  static int last_2_channel = 0;
  bool bad_channel = false;

  Channel = ( Data[ch] & 0x7C00 ) >> 10 ;
  TimeCh = ( Edge == 0 ) ?  Data[ch] & 0x03FF : Data[ch] & 0x01FF ;
/*
  static int num = 0;
  static ofstream file ( "test.txt", ios::out | ios::app ) ;
  if ( ch == 0 ) { file << "---   = " << num << endl ; num++ ; }
  file << ch << "\t" ;
  file << Channel << "\t" ;
  file << Edge << "\t" ;
  file << TimeCh  << "\t" << Data[ch] << endl ;
*/
  if ( ( Data[ch] == 0 )&&( ch != chs_tdc - 1 ) )
    if ( Data[ch+1] == 0 )
      { last_channel = last_2_channel = -1; return false; }

  if ( Channel < last_channel )
    { last_channel = last_2_channel = -1; return false; }
/*
    static int num = 0;
    static ofstream file ( "test.txt", ios::out | ios::app ) ;
    if ( ch == 0 ) { file << "---   = " << num << endl ; num++ ; }
    if (( Channel == last_channel )&&( Channel == 31 ))
    {
      file << ( ch - 1 ) << "\t" ;
      file << last_channel << "\t" ;
      file << Edge << "\t" ;
      file << last_timech  << "\t" << endl ;

      file << ch << "\t" ;
      file << Channel << "\t" ;
      file << Edge << "\t" ;
      file << TimeCh  << "\t" << endl ;
    }
//*/
  if (( Channel == last_channel ))
  {
    if ( last_channel == last_2_channel )
      { last_channel = last_2_channel = -1; return false; }

    if (( TimeCh < last_timech + 100 ) && ( TimeCh > last_timech - 100 ))
      { last_channel = last_2_channel = -1; return false; }
  }

  last_timech = TimeCh;
  last_2_channel = last_channel;
  last_channel = Channel;

  if ( bad_channel ) TimeCh = -1000;

  if ( ch == chs_tdc - 1 )
    { last_channel = last_2_channel = -1; return false; }

  return true;
}

//-----------------------------------------------------------

bool CamTdc::DataRd()
{
  int data = read();
  if ( !q()) return false;
  Channel = ( data & 0x7C00 ) >> 10 ;
  if ( Single ) return SingleRd ( data );
  if ( !DoubleRd ( data ) ) return false;
  int time_tmp = TimeCh;
  if ( !DoubleRd ( data = read() ) ) return false;
  TimeCh += time_tmp;
  return true;
}

bool CamTdc::SingleRd( int data )
{
  if ( data & 0x8000 ) return false;
  TimeCh = ( Edge == 0 ) ?  data & 0x03FF : data & 0x01FF;
  EdgeRec = ( data & 0x0200 ) >> 9;
  return true;
}

bool CamTdc::DoubleRd( int data )
{
  if ( data & 0x8000 ) return false;
  TimeCh = ( data & 0x0100 ) ? ( (data & 0x00FF) << 8 ):( data & 0x00FF );
  EdgeRec = (data & 0x0200)>>9;
  return true;
}

void CamTdc::DataWr ( int data )
	{ write ( data, A0, F16, Station ); }

//-----------------------------------------------------------

// void CamTdc::Reset ()
// 	{ write ( 0, A0, F30, Station ); }

void CamTdc::Clear ()
	{ write ( 0, A0, F9, Station ); }

// void CamTdc::ClearLam ()
// 	{ write ( 0, A0, F10, Station ); }

// bool  CamTdc::EventReady ()
// 	{ write ( 0, A2, F27, Station ); return q(); }

// bool  CamTdc::Busy ()
// 	{ write ( 0, A1, F27, Station ); return q(); }

void CamTdc::EnableAcq ()
	{ write ( 0, A1, F26, Station ); }

// void CamTdc::DisableAcq ()
// 	{ write ( 0, A1, F24, Station ); }

// void CamTdc::EnableLam ()
// 	{ write ( 0, A0, F26, Station ); }

// void CamTdc::DisableLam ()
// 	{ write ( 0, A0, F24, Station ); }

// void CamTdc::ExecuteTest ()
// 	{ write ( 0, A0, F25, Station ); }

/*----  READ AND WRITE REGISTER  -------------*/

int CamTdc::RegRd( TdcReg reg )
  { return read ( static_cast<subadd>(reg), F1, Station ); }

void CamTdc::RegWr( int data , TdcReg reg )
	{ write ( data, static_cast<subadd>(reg), F17, Station ); }

/*-----  COMMON START TIMEOUT  ----------------------------*/

int CamTdc::ToutRd()
	{ return TimeOut = ( RegRd(R4) & 0x03FF ) * 50 + 25; }

void CamTdc::ToutWr()
	{ ToutWr( TimeOut ); }


  // tout in nS
void CamTdc::ToutWr( int tout )
	{ RegWr( ( tout - 25 ) / 50 , R4 ); }

/*------  ENFORCED COMMON START TIME OUT  -----------------*/

int CamTdc::TenfRd()
	{ return TimeEnf = (RegRd(R3) & 0xFFF0)>>1; }

void CamTdc::TenfWr()
	{ TenfWr( TimeEnf ); }


  //tenf in nS
void CamTdc::TenfWr( int tenf )
	{ RegWr( ((tenf<<1)&0xFFF0)|(RegRd(R3)&(!0xFFF0)) , R3 ); }

/*------  RESOLUTION  -------------------------------------*/

short CamTdc::ShiftRd()
	{ return Shift = static_cast<short>((RegRd(R0) & 0x0300)>>8); }

void CamTdc::ShiftWr()
	{ ShiftWr( Shift ); }

void CamTdc::ShiftWr( short shift )
{
	if( shift >= 4 ) shift = 3;
	RegWr( (shift<<8) | (RegRd(R0) & (!0x0300)) , R0 );
  Shift = shift;
}

/*------  MEASURED PAUSE INTERVAL  -----------------------*/

int CamTdc::MpiRd()
	{ return Mpi = ( (RegRd(R1) & 0x0C00)>>10 )*800; }

void CamTdc::MpiWr()
	{ MpiWr( Mpi ); }

void CamTdc::MpiWr( int mpi )
	{ RegWr( (((mpi/800)<<10)&0x0C00)|(RegRd(R1)&(!0x0C00)),R1 ); }

/*------  HIT  --------------------------------------------*/

short CamTdc::HitRd()
{
	short tmp = static_cast<short>( RegRd(R2) & 0x000F );
	return Hit = tmp ? tmp : static_cast<short>(16);
}

void CamTdc::HitWr()
	{ HitWr( Hit ); }

void CamTdc::HitWr( short hit )
{
	if ( hit > 16 ) hit = 16;
	RegWr( hit | (RegRd(R2)&(!0x000F)) , R2 );
}

/*-----  EDGE  --------------------------------------------*/

short CamTdc::EdgeRd()
	{ return Edge = static_cast<short>((RegRd(R0)&0x0400) >> 10); }

void CamTdc::EdgeWr()
	{ EdgeWr( Edge ); }

void CamTdc::EdgeWr( short _edge )
{
	if( _edge >= 2 ) return;
	RegWr( (_edge<<10) | (RegRd(R0)&(!0x0400)) , R0 );
}

/*-----  MODE  --------------------------------------------*/

TdcMode CamTdc::ModeRd()
	{ return Mode = (TdcMode)( (RegRd(R0) & 0xC000) >> 14 ); }

void CamTdc::ModeWr()
	{ ModeWr( Mode ); }

void CamTdc::ModeWr( TdcMode _mode )
{
  // F9,AO Clear all data and events (The F9 command also MUST be the FIRST command received by the module after power up.):
  write ( 0, A0, F9, Station );
  // F30, any subaddress. This selects programming mode and resets the Xilinx gate array:
  write ( 0, A0, F30, Station );
  //
  write ( 0, A0, static_cast<func>(F20 + _mode), Station );
  // F25, any subaddress. This begins the programming of the xilinx chip, using the selected program load (or the default load). This will take about 200 milliseconds to complete.
  write ( 0, A0, F25, Station ); 
  for ( long int i = 0; i < 10; ++i )
  {
    ::sleep(1);
    // F13, any subaddress. Test the done flag, return Q=l when programming is complete. The host computer should loop on this command until Q is equal to 1.
    write( 0, A0, F13, Station );
    if ( q() ) break;
  }
  // F9,AO Clear all data and events:
  write( 0, A0, F9, Station );
}

/*-----  TEST  --------------------------------------------*/

bool CamTdc::Test()
{
	RegWr ( RegRd(R0) | 0x00FF , R0 );
	if ( (RegRd(R0) & 0x00FF) != 0xFF ) return false;
	RegWr( RegRd(R0) & 0xFF00 , R0 );
	if ( RegRd(R0) & 0x00FF ) return false;
	return true;
}

bool CamTdc::TestRd()
{
	if ( (ModeRd()==0) || (Mode==2) ) return false;
	unsigned int reg = RegRd(R5);
	TestEnable = (reg & 0x0100) >> 8;
	TestClock = (reg & 0x0060) >> 5;
	TestPulses = reg & 0x001F;
	return true;
}

bool CamTdc::TestWr()
{
	if ( (ModeRd()==0) || (Mode==2) ) return false;
	RegWr(  ( (TestEnable & 0x0001) << 8 )
		| ( (TestClock & 0x0003) << 5 )
		| (TestPulses & 0x001F),  R5 );
	return true;
}

//---- END  --------------------------------------------------
