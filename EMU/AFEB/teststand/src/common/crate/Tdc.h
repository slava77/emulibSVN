/*-------------------------------------------
		TDC - LeCroy 3377
    See operator's manual
    "Model 3377 32 Channel CAMAC TDC"
--------------------------------------------*/

#ifndef TdcH
#define TdcH

#include "Camac.h"
//-----------------------------------------------------------
namespace tdc3377_module {
//-----------------------------------------------------------

enum TdcMode {
    M0,    //Mode0 - Common Stop  , Single Word  ( Default )
    M1,    //Mode1 - Common Start , Single Word
    M2,    //Mode2 - Common Stop  , Double Word
    M3  }; //Mode3 - Common Start , Double Word

enum TdcReg { R0 , R1 , R2 , R3 , R4 , R5 };

static const int chs_tdc = 16 * 4;

//-----------------------------------------------------------
class CamTdc: public CCamac
{
private:

	stat Station;
	int Channel;
	int TimeCh;
	TdcMode Mode;
	int EdgeRec;
  unsigned short Data[chs_tdc];

public:

	CamTdc ( int branch, stat s );

  short station_number ( short station );
  short station_number ( void );

	bool HeadRd ();
	bool HeadRdBlock ();
	bool DataRd ();
	void DataWr ( int data );

  int TimeChRd ( void ) { return TimeCh; };
  bool BlockRd ( int ch );
  void BlockClear ();
  int ChannelRd ( void ) { return Channel; };
  float ShiftTime ( void ) { return static_cast<float>(1<<Shift)/2; };

  bool Set ( TdcMode mode, short shift, short hit,
               short edge, int mpi, int time_out, int time_enf );

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

	int RegRd( TdcReg reg );
	void RegWr( int data , TdcReg reg );

	bool DataHead( int data );
	bool SingleRd( int data );
	bool DoubleRd( int data );

/*-------  Mode = M1, M2, M3, M4  ------------------------*/
	TdcMode ModeRd();
	void ModeWr();
	void ModeWr( TdcMode mode );
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
//---------------------------------------------------------------------------
} //end namespace tdc3377_module
using namespace tdc3377_module;

//extern CamTdc* Tdc;
//-----------------------------------------------
#endif
