#ifndef __AFEB_teststand_LeCroy3377_h__
#define __AFEB_teststand_LeCroy3377_h__

#include "AFEB/teststand/CrateController.h"
#include "AFEB/teststand/CAMAC.h"

namespace AFEB { namespace teststand {

    class LeCroy3377 : public Module
    {
    public:
      enum TdcMode_t {
	M0,    //Mode0 - Common Stop  , Single Word  ( Default )
	M1,    //Mode1 - Common Start , Single Word
	M2,    //Mode2 - Common Stop  , Double Word
	M3     //Mode3 - Common Start , Double Word
      };
      enum TdcRegister_t { R0 , R1 , R2 , R3 , R4 , R5 };
      static const int nTdcChannels = 16 * 4;
      LeCroy3377( const string name );
      bool Set( TdcMode_t mode, short shift, short hit,
		short edge, int mpi, int time_out, int time_enforced );
      bool HeadRd ();
      bool HeadRdBlock ();
      bool DataRd ();
      void DataWr ( int data );
      
      int TimeChRd ( void ) { return TimeCh; };
      bool BlockRd ( int ch );
      void BlockClear ();
      int ChannelRd ( void ) { return Channel; };
      float ShiftTime ( void ) { return static_cast<float>(1<<Shift)/2; };
      
      // void Reset ();
      void Clear ();
      // void ClearLam ();
      void EnableAcq ();
      // void DisableAcq ();
      // void EnableLam ();
      // void DisableLam ();
      // void ExecuteTest ();
      // bool EventReady ();
      // bool Busy ();
      
    private:
      bool Single;
      //stat Station;
      int Channel;
      int TimeCh;
      TdcMode_t Mode;
      int EdgeRec;
      unsigned short Data[nTdcChannels];
      
      int RegRd( TdcRegister_t reg );
      void RegWr( int data , TdcRegister_t reg );
      
      bool DataHead( int data );
      bool SingleRd( int data );
      bool DoubleRd( int data );
      
      /*-------  Mode = M1, M2, M3, M4  ------------------------*/
      TdcMode_t ModeRd();
      void ModeWr();
      void ModeWr( TdcMode_t mode );
      /*----------------------------------------------------------
	Edge = 0 - Leading edge ONLY is recorded  ( Default )
	Edge = 1 - Both edges are recorded
	---------------------*/
      short Edge;
      short EdgeRd();
      void EdgeWr();
      void EdgeWr( short edge );
      /*----------------------------------------------------------
	Shift = 0 - 0.5nS resolution  ( Default )
	Shift = 1 - 1.0nS resolution
	Shift = 2 - 2.0nS resolution
	Shift = 3 - 4.0nS resolution
	---------------------*/
      short Shift;
      short ShiftRd();
      void ShiftWr();
      void ShiftWr( short shift );
      
      /*-------  Hit = 1.. 16  ----------------------------------*/
      short Hit;
      short HitRd();
      void HitWr();
      void HitWr( short hit );
      
      /*-------  MPI = 0, 0.8, 1.6, 3.2 uS  ---------------------*/
      int Mpi;
      int MpiRd();
      void MpiWr();
      void MpiWr( int mpi );
      
      int TimeOut;
      int ToutRd();
      void ToutWr();
      void ToutWr( int tout );
      
      int TimeEnf;
      int TenfRd();
      void TenfWr();
      void TenfWr( int tenf );
      
      int TestEnable;
      int TestClock;
      int TestPulses;
      bool Test();
      bool TestRd();
      bool TestWr();
    };

  }}

#endif
