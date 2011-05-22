#include "AFEB/teststand/LeCroy3377.h"
#include "AFEB/teststand/Jorway73A.h"
#include "AFEB/teststand/Crate.h"

#include <assert.h>
#include <stdlib.h> // for exit()

AFEB::teststand::LeCroy3377::LeCroy3377() :
  Module( "LeCroy3377", "TDC" ){
}

/*-----  SET THE MODULE  ----------------------------------------*/

bool AFEB::teststand::LeCroy3377::Set ( TdcMode_t mode, short shift, short hit,
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

bool AFEB::teststand::LeCroy3377::HeadRd()
  { return DataHead( crate_->getCrateController()->read ( A0, F0, (Station_t)slot_ ) ); }

bool AFEB::teststand::LeCroy3377::HeadRdBlock()
{
  BlockClear();
  if ( DataHead( crate_->getCrateController()->read ( A0, F0, (Station_t)slot_ ) ) )
  {
    // read_block ( F0, Data, chs_tdc );
    crate_->getCrateController()->readBlock ( A0, F0, (Station_t)slot_, Data, nShortsData );
    return true;
  }
  else return false;//ret;
}

bool AFEB::teststand::LeCroy3377::DataHead( int data )
{
  Edge =  static_cast<short>( (data & 0x0400) >> 10 );
  Shift = static_cast<short>( (data & 0x0300) >> 8 );
  Single = ( ~data & 0x4000 ) >> 14;
  return ( data & 0x8000 );
}
//-----------------------------------------------------------
void AFEB::teststand::LeCroy3377::BlockClear ( void )
{
  for ( int ch = 0; ch < nShortsData; ch++ )
    Data[ch] = 0;
}

bool AFEB::teststand::LeCroy3377::BlockRd ( int ch )
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
  if ( ( Data[ch] == 0 )&&( ch != nShortsData - 1 ) )
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

  if ( ch == nShortsData - 1 )
    { last_channel = last_2_channel = -1; return false; }

  return true;
}

//-----------------------------------------------------------

bool AFEB::teststand::LeCroy3377::DataRd()
{
  // int data = read();
  int data = crate_->getCrateController()->read( A0, F0, (Station_t)slot_ );
  // if ( !q()) return false;
  if ( !crate_->getCrateController()->q() ) return false;
  Channel = ( data & 0x7C00 ) >> 10 ;
  if ( Single ) return SingleRd ( data );
  if ( !DoubleRd ( data ) ) return false;
  int time_tmp = TimeCh;
  // if ( !DoubleRd ( data = read() ) ) return false;
  if ( !DoubleRd ( data = crate_->getCrateController()->read( A0, F0, (Station_t)slot_ ) ) ) return false;
  TimeCh += time_tmp;
  return true;
}

bool AFEB::teststand::LeCroy3377::SingleRd( int data )
{
  if ( data & 0x8000 ) return false;
  TimeCh = ( Edge == 0 ) ?  data & 0x03FF : data & 0x01FF;
  EdgeRec = ( data & 0x0200 ) >> 9;
  return true;
}

bool AFEB::teststand::LeCroy3377::DoubleRd( int data )
{
  if ( data & 0x8000 ) return false;
  TimeCh = ( data & 0x0100 ) ? ( (data & 0x00FF) << 8 ):( data & 0x00FF );
  EdgeRec = (data & 0x0200)>>9;
  return true;
}

void AFEB::teststand::LeCroy3377::DataWr ( int data )
	{ crate_->getCrateController()->write ( data, A0, F16, (Station_t)slot_ ); }

//-----------------------------------------------------------

void AFEB::teststand::LeCroy3377::Reset ()
	{ crate_->getCrateController()->write ( 0, A0, F30, (Station_t)slot_ ); }

void AFEB::teststand::LeCroy3377::Clear ()
	{ crate_->getCrateController()->write ( 0, A0, F9, (Station_t)slot_ ); }

void AFEB::teststand::LeCroy3377::ClearLam ()
	{ crate_->getCrateController()->write ( 0, A0, F10, (Station_t)slot_ ); }

bool  AFEB::teststand::LeCroy3377::EventReady ()
	{ crate_->getCrateController()->write ( 0, A2, F27, (Station_t)slot_ ); return crate_->getCrateController()->q(); }

bool  AFEB::teststand::LeCroy3377::Busy ()
	{ crate_->getCrateController()->write ( 0, A1, F27, (Station_t)slot_ ); return crate_->getCrateController()->q(); }

void AFEB::teststand::LeCroy3377::EnableAcq ()
	{ crate_->getCrateController()->write ( 0, A1, F26, (Station_t)slot_ ); }

void AFEB::teststand::LeCroy3377::DisableAcq ()
	{ crate_->getCrateController()->write ( 0, A1, F24, (Station_t)slot_ ); }

void AFEB::teststand::LeCroy3377::EnableLam ()
	{ crate_->getCrateController()->write ( 0, A0, F26, (Station_t)slot_ ); }

void AFEB::teststand::LeCroy3377::DisableLam ()
	{ crate_->getCrateController()->write ( 0, A0, F24, (Station_t)slot_ ); }

void AFEB::teststand::LeCroy3377::ExecuteTest ()
	{ crate_->getCrateController()->write ( 0, A0, F25, (Station_t)slot_ ); }

// -----  Wrappers for CAMAC methods -------
void AFEB::teststand::LeCroy3377::write( const unsigned int data, 
					 const Subaddress_t subaddress, 
					 const Function_t function ) const {
  crate_->getCrateController()->write( data, subaddress, function, (Station_t)slot_ );
}

unsigned int AFEB::teststand::LeCroy3377::read( const Subaddress_t subaddress, 
						const Function_t function ) const {
  return crate_->getCrateController()->read( subaddress, function, (Station_t)slot_ );
}

void AFEB::teststand::LeCroy3377::readBlock( const Subaddress_t subaddress, 
					     const Function_t function,
					     unsigned short *data,
					     const int blockSize ) const {
  crate_->getCrateController()->readBlock( subaddress, function, (Station_t)slot_, data, nShortsData );
}


/*----  READ AND WRITE REGISTER  -------------*/

int AFEB::teststand::LeCroy3377::RegRd( TdcRegister_t reg )
  // { return read ( static_cast<subadd>(reg), F1 ); }
{
  return crate_->getCrateController()->read( static_cast<Subaddress_t>(reg), F1, (Station_t)slot_ );
}

void AFEB::teststand::LeCroy3377::RegWr( int data , TdcRegister_t reg )
	// { write ( data, static_cast<subadd>(reg), F17 ); }
{
  crate_->getCrateController()->write( data, static_cast<Subaddress_t>(reg), F17, (Station_t)slot_ );
}
/*-----  COMMON START TIMEOUT  ----------------------------*/

int AFEB::teststand::LeCroy3377::ToutRd()
	{ return TimeOut = ( RegRd(R4) & 0x03FF ) * 50 + 25; }

void AFEB::teststand::LeCroy3377::ToutWr()
	{ ToutWr( TimeOut ); }


  // tout in nS
void AFEB::teststand::LeCroy3377::ToutWr( int tout )
	{ RegWr( ( tout - 25 ) / 50 , R4 ); }

/*------  ENFORCED COMMON START TIME OUT  -----------------*/

int AFEB::teststand::LeCroy3377::TenfRd()
	{ return TimeEnf = (RegRd(R3) & 0xFFF0)>>1; }

void AFEB::teststand::LeCroy3377::TenfWr()
	{ TenfWr( TimeEnf ); }


  //tenf in nS
void AFEB::teststand::LeCroy3377::TenfWr( int tenf )
	{ RegWr( ((tenf<<1)&0xFFF0)|(RegRd(R3)&(!0xFFF0)) , R3 ); }

/*------  RESOLUTION  -------------------------------------*/

short AFEB::teststand::LeCroy3377::ShiftRd()
	{ return Shift = static_cast<short>((RegRd(R0) & 0x0300)>>8); }

void AFEB::teststand::LeCroy3377::ShiftWr()
	{ ShiftWr( Shift ); }

void AFEB::teststand::LeCroy3377::ShiftWr( short shift )
{
	if( shift >= 4 ) shift = 3;
	RegWr( (shift<<8) | (RegRd(R0) & (!0x0300)) , R0 );
  Shift = shift;
}

/*------  MEASURED PAUSE INTERVAL  -----------------------*/

int AFEB::teststand::LeCroy3377::MpiRd()
	{ return Mpi = ( (RegRd(R1) & 0x0C00)>>10 )*800; }

void AFEB::teststand::LeCroy3377::MpiWr()
	{ MpiWr( Mpi ); }

void AFEB::teststand::LeCroy3377::MpiWr( int mpi )
	{ RegWr( (((mpi/800)<<10)&0x0C00)|(RegRd(R1)&(!0x0C00)),R1 ); }

/*------  HIT  --------------------------------------------*/

short AFEB::teststand::LeCroy3377::HitRd()
{
	short tmp = static_cast<short>( RegRd(R2) & 0x000F );
	return Hit = tmp ? tmp : static_cast<short>(16);
}

void AFEB::teststand::LeCroy3377::HitWr()
	{ HitWr( Hit ); }

void AFEB::teststand::LeCroy3377::HitWr( short hit )
{
	if ( hit > 16 ) hit = 16;
	RegWr( hit | (RegRd(R2)&(!0x000F)) , R2 );
}

/*-----  EDGE  --------------------------------------------*/

short AFEB::teststand::LeCroy3377::EdgeRd()
	{ return Edge = static_cast<short>((RegRd(R0)&0x0400) >> 10); }

void AFEB::teststand::LeCroy3377::EdgeWr()
	{ EdgeWr( Edge ); }

void AFEB::teststand::LeCroy3377::EdgeWr( short _edge )
{
	if( _edge >= 2 ) return;
	RegWr( (_edge<<10) | (RegRd(R0)&(!0x0400)) , R0 );
}

/*-----  MODE  --------------------------------------------*/

AFEB::teststand::LeCroy3377::TdcMode_t AFEB::teststand::LeCroy3377::ModeRd()
	{ return Mode = (TdcMode_t)( (RegRd(R0) & 0xC000) >> 14 ); }

void AFEB::teststand::LeCroy3377::ModeWr()
	{ ModeWr( Mode ); }

void AFEB::teststand::LeCroy3377::ModeWr( TdcMode_t _mode )
{
  // F9,AO Clear all data and events (The F9 command also MUST be the FIRST command received by the module after power up.):
  crate_->getCrateController()->write ( 0, A0, F9, (Station_t)slot_  );
  // F30, any subaddress. This selects programming mode and resets the Xilinx gate array:
  crate_->getCrateController()->write ( 0, A0, F30, (Station_t)slot_  );
  //
  crate_->getCrateController()->write ( 0, A0, static_cast<Function_t>(F20 + _mode), (Station_t)slot_  );
  // F25, any subaddress. This begins the programming of the xilinx chip, using the selected program load (or the default load). This will take about 200 milliseconds to complete.
  crate_->getCrateController()->write ( 0, A0, F25, (Station_t)slot_  ); 
  for ( long int i = 0; i < 10; ++i )
    {
      ::sleep(1);
      // F13, any subaddress. Test the done flag, return Q=l when programming is complete. The host computer should loop on this command until Q is equal to 1.
      crate_->getCrateController()->write( 0, A0, F13, (Station_t)slot_  );
      if ( crate_->getCrateController()->q() ) break;
    }
  // F9,AO Clear all data and events:
  crate_->getCrateController()->write( 0, A0, F9, (Station_t)slot_  );
}

/*-----  TEST  --------------------------------------------*/

bool AFEB::teststand::LeCroy3377::Test()
{
	RegWr ( RegRd(R0) | 0x00FF , R0 );
	if ( (RegRd(R0) & 0x00FF) != 0xFF ) return false;
	RegWr( RegRd(R0) & 0xFF00 , R0 );
	if ( RegRd(R0) & 0x00FF ) return false;
	return true;
}

bool AFEB::teststand::LeCroy3377::TestRd()
{
	if ( (ModeRd()==0) || (Mode==2) ) return false;
	unsigned int reg = RegRd(R5);
	TestEnable = (reg & 0x0100) >> 8;
	TestClock = (reg & 0x0060) >> 5;
	TestPulses = reg & 0x001F;
	return true;
}

bool AFEB::teststand::LeCroy3377::TestWr()
{
	if ( (ModeRd()==0) || (Mode==2) ) return false;
	RegWr(  ( (TestEnable & 0x0001) << 8 )
		| ( (TestClock & 0x0003) << 5 )
		| (TestPulses & 0x001F),  R5 );
	return true;
}
