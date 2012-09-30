//----------------------------------------------------------------------
// $Id: VMEModule.cc,v 3.37 2012/09/30 21:19:42 liu Exp $
// $Log: VMEModule.cc,v $
// Revision 3.37  2012/09/30 21:19:42  liu
// update for ME11 new electronics
//
// Revision 3.36  2012/09/26 22:15:16  liu
// update SVF loading
//
// Revision 3.35  2012/09/05 21:39:11  liu
// remove ODMB class
//
// Revision 3.33  2012/06/20 08:45:01  kkotov
//
// New faster DMB/CFEB EPROM readback routines
//
// Revision 3.32  2012/05/09 20:59:00  liu
// fix missing standard header files
//
// Revision 3.31  2012/02/09 12:34:05  liu
// turn on vme package delay in svfLoad
//
// Revision 3.30  2011/11/17 00:13:44  liu
// clear buffer before read PROM
//
// Revision 3.29  2011/11/04 20:05:52  liu
// change svfLoad interface
//
// Revision 3.28  2011/07/02 15:41:00  liu
// restore default/longer delay value in SVFload
//
// Revision 3.27  2011/07/01 03:37:20  liu
// new JTAG functions
//
// Revision 3.26  2011/02/23 11:42:04  liu
// updated svfLoad, added PROM read back functions
//
// Revision 3.25  2011/02/22 12:26:31  liu
// remove obsolete scan_alct() and RestoreIdle_alct()
//
// Revision 3.24  2011/02/04 11:45:23  liu
// modified the behaviour of RUNTEST, STATE; changed progress indicator
//
// Revision 3.23  2010/05/05 11:46:58  liu
// make some stdout prints optional
//
// Revision 3.22  2009/03/25 10:19:42  liu
// move header files to include/emu/pc
//
// Revision 3.21  2008/09/30 14:26:03  liu
// remove EMU_JTAG_constants from VMEModule
//
// Revision 3.20  2008/08/13 11:30:54  geurts
// introduce emu::pc:: namespaces
// remove any occurences of "using namespace" and make std:: references explicit
//
// Revision 3.19  2008/05/28 10:35:31  liu
// DMB counters in jumbo packet
//
// Revision 3.18  2008/04/22 08:32:35  liu
// Ben's Crate controller utilities
//
// Revision 3.17  2008/04/02 13:41:54  liu
// add f/w downloading verify for CCB & MPC
//
// Revision 3.16  2008/02/18 12:09:19  liu
// new functions for monitoring
//
// Revision 3.15  2008/02/04 15:04:33  liu
// update SVFload process
//
// Revision 3.14  2008/01/17 12:51:48  liu
// disable VME readback in svfLoad()
//
// Revision 3.13  2007/12/27 00:33:54  liu
// update
//
// Revision 3.12  2007/12/25 00:12:00  liu
// *** empty log message ***
//
// Revision 3.11  2007/09/12 16:02:00  liu
// remove log4cplus dependency
//
// Revision 3.10  2007/05/17 18:49:44  liu
// svfload update
//
// Revision 3.9  2007/01/31 16:51:19  rakness
// remove excessive output from prom programming
//
// Revision 3.8  2006/11/15 16:01:37  mey
// Cleaning up code
//
// Revision 3.7  2006/10/12 17:52:13  mey
// Update
//
// Revision 3.6  2006/10/02 12:58:34  mey
// Update
//
// Revision 3.5  2006/09/05 16:14:43  mey
// Update
//
// Revision 3.4  2006/08/09 09:32:39  mey
// Included EMUjtag into VMEModule
//
// Revision 3.3  2006/08/08 19:23:08  mey
// Included Jtag sources
//
// Revision 3.2  2006/08/08 15:44:38  mey
// Went back to bootstrap
//
// Revision 3.1  2006/08/08 14:48:42  rakness
// give TMB JTAG chains to FPGA
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.27  2006/07/13 15:52:22  mey
// New Parser structure
//
// Revision 2.26  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.25  2006/07/12 07:58:18  mey
// Update
//
// Revision 2.24  2006/07/11 12:46:43  mey
// UPdate
//
// Revision 2.23  2006/07/11 09:31:12  mey
// Update
//
// Revision 2.22  2006/07/06 07:31:48  mey
// MPC firmware loading added
//
// Revision 2.21  2006/07/05 09:29:18  mey
// Update
//
// Revision 2.20  2006/07/04 15:29:31  mey
// Update
//
// Revision 2.19  2006/06/20 12:25:37  mey
// Update
//
// Revision 2.18  2006/06/19 14:17:58  mey
// Update
//
// Revision 2.17  2006/06/16 13:05:24  mey
// Got rid of Compiler switches
//
// Revision 2.16  2006/05/10 23:57:24  liu
// // Update for Production Controller with firmware 3.59
//
// Revision 2.15  2006/03/23 12:41:41  mey
// UPdate
//
// Revision 2.14  2006/03/10 13:13:13  mey
// Jinghua's changes
//
// Revision 2.11  2006/01/31 08:52:29  mey
// UPdate
//
// Revision 2.10  2006/01/23 15:01:34  mey
// Update
//
// Revision 2.9  2006/01/23 15:00:10  mey
// Update
//
// Revision 2.8  2006/01/14 22:25:09  mey
// UPdate
//
// Revision 2.7  2006/01/12 11:48:17  mey
// Update
//
// Revision 2.6  2006/01/12 11:32:50  mey
// Update
//
// Revision 2.5  2005/12/08 12:00:32  mey
// Update
//
// Revision 2.4  2005/12/06 15:32:09  mey
// Fix bug
//
// Revision 2.3  2005/12/02 18:12:19  mey
// get rid of D360
//
// Revision 2.2  2005/11/21 19:42:28  mey
// Update
//
// Revision 2.1  2005/11/02 16:16:43  mey
// Update for new controller
//
// Revision 1.23  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//
//----------------------------------------------------------------------
#include "emu/pc/VMEModule.h"
#include "emu/pc/VMEController.h"
#include "emu/pc/Crate.h"

#include <cmath>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h> // read and write
// #include "emu/pc/EMU_JTAG_constants.h"

#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl; 
#define PRINTSTRING(x) cout << #x << endl; 
#endif


namespace emu {
  namespace pc {

VMEModule::VMEModule(Crate * theCrate, int newslot): 
  theSlot(newslot)
{
  theCrate_ = theCrate;
  //
#ifdef debugV
  std::cout << "creating VMEModule in crate " << theCrate->CrateID() << std::endl;
#endif
  //
  theController = theCrate->vmeController();
  theCrate->addModule(this);
  //
#ifdef debugV
  std::cout << "Done VMEModule in crate " << theCrate->CrateID() << std::endl;
#endif
  //
}

int VMEModule::crate() { return theCrate_->CrateID(); }

void VMEModule::start() {
  PRINTSTRING(OVAL: start() from VMEModule have been called...);
  //
#ifdef debugV
  std::cout << "starting VMEModule for slot " << std::dec << theSlot << " boardType " << boardType() << " line " << (int) c << std::endl;
#endif  
  theController->start( theSlot, boardType() );
}


void VMEModule::end() {
}

 
void VMEModule::endDevice() {
  theController->end();
}

void VMEModule::do_vme(char fcn, char vme, 
                       const char *snd,char *rcv, int when) {
   theController->start( theSlot, boardType() );
   theController->do_vme(fcn, vme, snd, rcv, when);
}

void VMEModule::devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,
                     const char *inbuf,char *outbuf,int irdsnd) {
  //printf("VMEModule::devdo\n");
  //std::cout << "VMEModule. Setting slot=" << theSlot << std::endl;
  theController->start( theSlot, boardType() );
  theController->devdo(dev, ncmd, cmd, nbuf, inbuf, outbuf, irdsnd);
}

void VMEModule::new_devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,
                     const char *inbuf,char *outbuf,int irdsnd) {
  //printf("VMEModule::devdo\n");
  //std::cout << "VMEModule. Setting slot=" << theSlot << std::endl;
  theController->start( theSlot, boardType() );
  theController->new_devdo(dev, ncmd, cmd, nbuf, inbuf, outbuf, irdsnd);
}

void VMEModule::scan(int reg,const char *snd,int cnt,char *rcv,int ird) {
  theController->start( theSlot, boardType() );
  if(boardType()==TMB_ENUM || boardType()==MPC_ENUM || boardType()==CCB_ENUM)
    theController->scan_jtag(reg, snd, cnt, rcv, ird);
  else
    theController->scan(reg, snd, cnt, rcv, ird);
}

void VMEModule::scan_reset(int reg,const char *snd,int cnt,char *rcv,int ird) {
  theController->start( theSlot, boardType());
  theController->scan_reset(reg, snd, cnt, rcv, ird);
}

void VMEModule::scan_dmb_headtail(int reg,const char *snd,int cnt,char *rcv,int ird,int headtail, int when) {
  theController->start( theSlot, boardType());
  theController->scan_dmb_headtail(reg, snd, cnt, rcv, ird, headtail, when);
}

void VMEModule::RestoreIdle() {
  theController->start( theSlot, boardType() );
  if(boardType()==TMB_ENUM || boardType()==MPC_ENUM || boardType()==CCB_ENUM)
    theController->RestoreIdle_jtag();
  else
    theController->RestoreIdle();
}

void VMEModule::RestoreReset() {
  theController->start( theSlot, boardType() );
  if(boardType()==TMB_ENUM || boardType()==MPC_ENUM || boardType()==CCB_ENUM)
    theController->RestoreReset_jtag();
  else
    theController->RestoreIdle_reset();
}

void VMEModule::SetupJtag() {
  //
  if(boardType()==TMB_ENUM){
    if ( JtagSource_ ) {
      theController->SetupJtagBaseAddress(0x10);
    }else {
      theController->SetupJtagBaseAddress(0x70000);
    }
    theController->SetupTCK(2);
    theController->SetupTMS(1);
    theController->SetupTDI(0);
    theController->SetupTDO(7);
  }
  else if(boardType()==MPC_ENUM || boardType()==CCB_ENUM){
    theController->SetupJtagBaseAddress(0x0);
    theController->SetupTCK(7);
    theController->SetupTMS(6);
    theController->SetupTDI(5);
    theController->SetupTDO(0);
  }
}

void VMEModule::InitJTAG(int port) {
  theController->start( theSlot, boardType() );
  theController->InitJTAG(port);
}

void VMEModule::CloseJTAG() {
  theController->start( theSlot, boardType() );
  theController->CloseJTAG();
  theController->end();
}

int VMEModule::eth_write(){
  theController->nwbuf=nwbuf;
  int lim=nwbuf;
  if(lim<0||lim>9000)lim=9000;
  for(int i=0;i<lim;i++)theController->wbuf[i]=wbuf[i];
  int n=theController->eth_write();
  return n;
}

int VMEModule::eth_read(){
   theController->nrbuf=0;
  int n=theController->eth_read();
  int lim=theController->nrbuf;
  for(int i=0;i<lim;i++)rbuf[i]=theController->rbuf[i];
  return n;
}

int VMEModule::eth_read_previous(){
  nrbuf=theController->nrbuf;
  for(int i=0;i<nrbuf;i++)rbuf[i]=theController->rbuf[i];
  return nrbuf;
}

int VMEModule::eth_read_timeout(int rd_tmo){
  int n=theController->eth_read_timeout(rd_tmo);
  return n;
}

int VMEModule::LeftToRead(){
  return theController->LeftToRead();
}


VMEController* VMEModule::getTheController(){
  return theController;
}

bool VMEModule::exist(){
  return theController->exist( theSlot );
}

void VMEModule::scan_word(int reg,const char *snd, int cnt, char *rcv,int ird)
{
  theController->start( theSlot, boardType() );
  theController->scan_word(reg, snd, cnt, rcv, ird);
}

void VMEModule::shift_bits(int reg,const char *snd, int cnt, char *rcv,int ird)
{
  theController->start( theSlot, boardType() );
  theController->shift_bits(reg, snd, cnt, rcv, ird);
}

void VMEModule::shift_state(int cnt, int mask)
{
   theController->start( theSlot, boardType() );
   theController->shift_state( cnt, mask);
}

int VMEModule::svfLoad(int jch, const char *fn, int db, int verify )
{
  int MAXBUFSIZE=8200;
  unsigned char snd[MAXBUFSIZE], rcv[MAXBUFSIZE], expect[MAXBUFSIZE],rmask[MAXBUFSIZE],smask[MAXBUFSIZE],cmpbuf[MAXBUFSIZE];
  unsigned char sndbuf[MAXBUFSIZE],rcvbuf[MAXBUFSIZE], realsnd[MAXBUFSIZE];
  unsigned char sndhdr[MAXBUFSIZE],sndtdr[MAXBUFSIZE], sndhir[MAXBUFSIZE], sndtir[MAXBUFSIZE];
  unsigned char hdrsmask[MAXBUFSIZE],tdrsmask[MAXBUFSIZE], hirsmask[MAXBUFSIZE], tirsmask[MAXBUFSIZE];
  FILE *dwnfp;
  char buf[MAXBUFSIZE+200], buf2[256];
  //  char buf[8192],buf2[256];
  char *Word[256],*lastn;
  const char *downfile;
  //char fStop;
  int jchan;
  //  unsigned char sndvalue;
  //  fpos_t ftdi_pos, fsmask_pos;
  unsigned char send_tmp, tmp;
  int i,j,Count,nbytes,tbytes, nbits,nframes,step_mode,pause;
  int hdrbits = 0, tdrbits = 0, hirbits = 0, tirbits = 0;
  int hdrbytes = 0, tdrbytes = 0, hirbytes = 0, tirbytes = 0; 
  int nowrit, cmpflag, errcntr;
  static int count;
  // MvdM struct JTAG_BBitStruct   driver_data;
  // int jtag_chain[4] = {1, 0, 5, 4};
  //int jtag_chain_tmb[6] = {7, 6, 9, 8, 3, 1};
  // === SIR Go through SelectDRScan->SelectIRScan->CaptureIR->ShiftIR  
  //char tms_pre_sir[4]={ 1, 1, 0, 0 }; 
  char tdi_pre_sir[4]={ 0, 0, 0, 0 };
  // === SDR Go through SelectDRScan->CaptureDR->ShiftDR
  // char tms_pre_sdr[3]={ 1, 0, 0 };
  char tdi_pre_sdr[3]={ 0, 0, 0 };
  // === SDR,SIR Go to RunTestIdle after scan
  // char tms_post[4]={ 0, 1, 1, 0 };
  char tdi_post[4]={ 0, 0, 0, 0 };
  int total_packages, one_pct;
  int send_packages ;
  bool readprom=false;
  int read_packages=0, repeat=1, total_read=0;
  
  total_packages = 0 ;
  send_packages = 0 ;
  jchan = jch;
  downfile = fn;
  errcntr = 0;
  if (downfile==NULL)    downfile="default.svf";
  
  dwnfp    = fopen(downfile,"r");
  if (dwnfp == NULL)
    {
      fprintf(stderr, "ERROR: failed to open file %s\n", downfile);
      
      return -1;
    }
  
  while (fgets(buf,256,dwnfp) != NULL) 
    {
      Parse(buf, &Count, &(Word[0]));
      if( strcmp(Word[0],"SDR")==0 ) 
         total_packages++ ;
      else if( strcmp(Word[0],"READ")==0)
      {
         total_read++;
         readprom=true;
      }
    }
  fseek(dwnfp, 0, SEEK_SET);
  
  printf("=== Programming Design with %s through JTAG chain %d\n",downfile, jchan);  
  printf("=== Have to send %d DATA packages \n",total_packages) ;
  one_pct=(total_packages+99)/100;
  if(one_pct<=0) one_pct=1;
  
  this->start(); 
// turn on delay, otherwise the VCC's FIFO full
  theController->SetUseDelay(true);
  count=0; 
  nowrit=1;
  step_mode=0;
  while (fgets(buf,256,dwnfp) != NULL)  
    {
      if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!')
	{
	  if (db>4)          printf("%s",buf);
	}
      else 
	{
	  if(strrchr(buf,';')==0)
	    {
	      lastn=strrchr(buf,'\r');
	      if(lastn!=0)lastn[0]='\0';
	      lastn=strrchr(buf,'\n');
	      if(lastn!=0)lastn[0]='\0';
	      memcpy(buf2,buf,256);
	      Parse(buf2, &Count, &(Word[0]));
	      if(( strcmp(Word[0],"SDR")==0) || (strcmp(Word[0],"SIR")==0) || (strcmp(Word[0],"SWR")==0) || (strcmp(Word[0],"SBR")==0) || (strcmp(Word[0],"SER")==0))
		{
		  sscanf(Word[1],"%d",&nbits);
		  if (nbits>MAXBUFSIZE) // === Handle Big Bitstreams
		    {
		      //(*MyOutput_) << "EMUjtag. nbits larger than buffer size" << std::endl;
		    }
		  else do  // == Handle Normal Bitstreams
		    {
		      lastn=strrchr(buf,'\r');
		      if(lastn!=0)lastn[0]='\0';
		      lastn=strrchr(buf,'\n');
		      if(lastn!=0)lastn[0]='\0';
		      if (fgets(buf2,256,dwnfp) != NULL)
			{
			  strcat(buf,buf2);
			}
		      else 
			{
			  if (db)              printf("End of File encountered.  Quiting\n");
			  return -1;
			}
		    }
		  while (strrchr(buf,';')==0);
		}
	    } 
	  bzero(snd, sizeof(snd));
	  bzero(cmpbuf, sizeof(cmpbuf));
	  bzero(sndbuf, sizeof(sndbuf));
	  bzero(rcvbuf, sizeof(rcvbuf));
	  
	  Parse(buf, &Count, &(Word[0]));
	  count=count+1;
	  cmpflag=0;
	  // ==================  Parsing commands from SVF file ====================
	  // === Handling HDR ===
	  if(strcmp(Word[0],"HDR")==0)
	    {
	      sscanf(Word[1],"%d",&hdrbits);
	      hdrbytes=(hdrbits)?(hdrbits-1)/8+1:0;
	    if (db)	  
	      printf("Sending %d bits of Data Header\n", hdrbits);
	    // if (db>3)          printf("HDR: Num of bits - %d, num of bytes - %d\n",hdrbits,hdrbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&sndhdr[j]);
			// printf("%2X",sndhdr[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
      		  	sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&hdrsmask[j]);
		      }
		  }
     	 	if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&expect[j]);
		      }
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	    }
	  
	  // === Handling HIR ===
	  else if(strcmp(Word[0],"HIR")==0)
	    {
	      sscanf(Word[1],"%d",&hirbits);
	      hirbytes=(hirbits)?(hirbits-1)/8+1:0;
	      if (db)	  
		printf("Sending %d bits of Instruction Header\n", hirbits);
	      // if (db>3)          printf("HIR: Num of bits - %d, num of bytes - %d\n",hirbits,hirbytes);
	      for(i=2;i<Count;i+=2)
		{
		  if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<hirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&sndhir[j]);
			// printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
		  if(strcmp(Word[i],"SMASK")==0)
		    {
		      for(j=0;j<hirbytes;j++)
		      {
      		  	sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&hirsmask[j]);
		      }
		    }
		  if(strcmp(Word[i],"TDO")==0)
		    {
		      //if (db>2)             cmpflag=1;
		      cmpflag=1;
		      for(j=0;j<hirbytes;j++)
			{
			  sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&expect[j]);
			}
		    }
		  if(strcmp(Word[i],"MASK")==0)
		    {
		      for(j=0;j<hirbytes;j++)
			{
			  sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&rmask[j]);
			}
		    }
		}
	    }	
	  
	  // === Handling TDR ===
	  else if(strcmp(Word[0],"TDR")==0)
	    {
	      sscanf(Word[1],"%d",&tdrbits);
	      tdrbytes=(tdrbits)?(tdrbits-1)/8+1:0;
	      if (db)	  
		printf("Sending %d bits of Data Tailer\n", tdrbits);
	      // if (db>3)          printf("TDR: Num of bits - %d, num of bytes - %d\n",tdrbits,tdrbytes);
	      for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&sndtdr[j]);
			// printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&tdrsmask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&expect[j]);
		      }
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }	
		  }
	      }
	    }
	  
	  // === Handling TIR ===
	 else if(strcmp(Word[0],"TIR")==0)
	 {
	    sscanf(Word[1],"%d",&tirbits);
	    tirbytes=(tirbits)?(tirbits-1)/8+1:0;
	    if (db)	  
	      printf("Sending %d bits of Instruction Tailer\n", tdrbits);
	    // if (db>3)          printf("TIR: Num of bits - %d, num of bytes - %d\n",tirbits,tirbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&sndtir[j]);
			    // printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&tirsmask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&expect[j]);
		      }
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	 }
	 // === Handling SDR ===
	 else if(strcmp(Word[0],"SDR")==0 || strcmp(Word[0],"SWR")==0)
	 {
            bool word_by_word=false;
            if(strcmp(Word[0],"SWR")==0) word_by_word=true; 
	      //std::cout << "SDR" << std::endl;
	      for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	      // cmpflag=1;    //disable the comparison for no TDO SDR
	    sscanf(Word[1],"%d",&nbits);
	    nbytes=(nbits+7)/8;
            tbytes=(hdrbits+nbits+tdrbits+7)/8;
	    if (db)	  printf("Sending %d bits Data\n", nbits);
	    // if (db>3)          printf("SDR: Num of bits - %d, num of bytes - %d\n",nbits,nbytes);
	    for(i=2;i<Count;i+=2)
	      {
	      if(strcmp(Word[i],"TDI")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&snd[j]);
		      //                printf("%2X",snd[j]);
		    }
		  //                printf("\n%d\n",nbytes);
		}
	      if(strcmp(Word[i],"SMASK")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&smask[j]);
		    }
		}
	      if(strcmp(Word[i],"TDO")==0)
		{
		  //if (db>2)             cmpflag=1;
		  cmpflag=1;
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&expect[j]);
		    }
		}
	      if(strcmp(Word[i],"MASK")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		    }
		}
	      }
	    for(i=0;i<nbytes;i++)
	      {
	      send_tmp = snd[i]&smask[i];
	      for(j=0;j<8;j++)
		{
		  if ((i*8+j)< nbits) 
		    { 
		      sndbuf[i*8+j+3]=send_tmp&0x01; 
		    }
		  send_tmp = send_tmp >> 1;
		}
	      }
	    for(i=0;i<4;i++)sndbuf[nbits+3]=tdi_post[i];         
	    nframes=nbits+7;
            // Put send SDR here
	    for (i=0; i< tbytes; i++)
	      realsnd[i] = 0;
	    if (hdrbytes>0) {
	      for (i=0;i<hdrbytes;i++)
		realsnd[i]=sndhdr[i];
	    }
	    for (i=0;i<nbits;i++)
 	      realsnd[(i+hdrbits)/8] |= (snd[i/8] >> (i%8)) << ((i+hdrbits)%8);
	    if (tdrbytes>0) {
	      for (i=0;i<tdrbits;i++)
		realsnd[(i+hdrbits+nbits)/8] |= (sndtdr[i/8] >> (i%8)) << ((i+hdrbits+nbits)%8);
	    }	    
	    //
	    send_packages++ ;
            if(!readprom)
            {
               if ( (send_packages%one_pct)==0 ) 
                  std::cout << "Sending " << std::dec << send_packages/one_pct << "%..." << std::endl;
	       if ( send_packages == total_packages ) std::cout << "Done!" << std::endl;
            }
	    //
            if(word_by_word)
              this->scan_word(DATA_REG, (char*)realsnd, hdrbits+nbits+tdrbits, (char*)rcv, (verify>0 && cmpflag>0)?1:0);
            else
	      this->scan(DATA_REG, (char*)realsnd, hdrbits+nbits+tdrbits, (char*)rcv, (verify>0 && cmpflag>0)?1:0); 
	    //
	    if (db)
	    {	
	      printf("SDR Sent Data: ");
	      for (i=0; i< tbytes; i++) 
		printf("%02X",realsnd[i]);
	      printf("\n");
	      //
	      printf("SDR Readback Data: ");
	      for (i=0; i< tbytes; i++) 
		printf("%02X",rcv[i]);
	      printf("\n");
	    }		    
	    //
	    if (verify && cmpflag==1)
	    {     
               if(hdrbits>0)
               {
                  //   1. expend bytes into bits
   	          for(i=0;i<tbytes;i++)
	          {
                      tmp=rcv[i];
                      for(j=0; j<8; j++)
                      {
                          buf[i*8+j]=tmp&1;
                          tmp >>= 1;
                      }
                  }
                  //   2. put bits (without HDR & TDR) back into bytes
                  int rcvindex=0;
	          for(i=0;i<nbytes;i++)
	          {
                      tmp=0;
                      for(j=7; j>=0; j--)
                      {
                          tmp <<= 1;
                          tmp |= (buf[i*8+j+hdrbits]&1);
                      }
                      rcv[rcvindex++]=tmp;
                  }
               } //end of removing HDR & TDR           

	       for(i=0;i<nbytes;i++)
	       {
		   if (((rcv[i]^expect[i]) & rmask[i])!=0)
		   {
		      if(db) printf("SDR read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcv[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF);
		      errcntr++;
		   }
	       }	
	    }
         }
        // === Handling SIR ===
        else if(strcmp(Word[0],"SIR")==0)
          {
	    for(i=0;i<4;i++)sndbuf[i]=tdi_pre_sir[i];
	    // cmpflag=1;    //disable the comparison for no TDO SDR
	    sscanf(Word[1],"%d",&nbits);
	    nbytes=(nbits+7)/8;
            tbytes=(hirbits+nbits+tirbits+7)/8;
	    if (db)	  printf("Sending %d bits of Command\n",nbits);
	    // if (db>3)          printf("SIR: Num of bits - %d, num of bytes - %d\n",nbits,nbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&snd[j]);
		      }
		  }
		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&smask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
			cmpflag=1;
			// if (db>2)              cmpflag=1;
			for(j=0;j<nbytes;j++)
			  {
			    sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&expect[j]);
			  }
		  }
		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	    for(i=0;i<nbytes;i++)
	      {
		send_tmp = snd[i]&smask[i];
		// printf("\n%d - ", send_tmp);
		for(j=0;j<8;j++)
		  {
		    if ((i*8+j)< nbits) 
		      {
			sndbuf[i*8+j+4]=send_tmp&0x01;
			// printf("%d", sndbuf[i*8+j+4]);
		      }
		    send_tmp = send_tmp >> 1;
		  }
	      }
	    for(i=0;i<4;i++)sndbuf[nbits+4]=tdi_post[i];
	    nframes=nbits+8;
	    // Put send SIR here
	    for (i=0; i< tbytes;  i++)
	      realsnd[i] = 0;
	    if (hirbytes>0) {
	      for (i=0;i<hirbytes;i++)
		realsnd[i]=sndhir[i];
	    }
	    for (i=0;i<nbits;i++)
	      realsnd[(i+hirbits)/8] |= (snd[i/8] >> (i%8)) << ((i+hirbits)%8);
	    if (tirbytes>0) {
	      for (i=0;i<tirbits;i++)
		realsnd[(i+hirbits+nbits)/8] |= (sndtir[i/8] >> (i%8)) << ((i+hirbits+nbits)%8);
	    }
	    //
	    this->scan(INSTR_REG, (char*)realsnd, hirbits+nbits+tirbits, (char*)rcv, (verify>0 && cmpflag>0)?1:0); 
	    //	   
	    if (db)
	    { 	printf("SIR Send Data: ");
	        for (i=0; i< tbytes;  i++)
	           printf("%02X",realsnd[i]);
	        printf("\n");

	        printf("SIR Readback Data: ");
	        for (i=0; i< nbytes;  i++)
	           printf("%02X",rcv[i]);
	        printf("\n");
	    }
	    //
	    if (verify && cmpflag==1)
	    {
               if(hirbits>0)
               {
                  //   1. expend bytes into bits
   	          for(i=0;i<tbytes;i++)
	          {
                      tmp=rcv[i];
                      for(j=0; j<8; j++)
                      {
                          buf[i*8+j]=tmp&1;
                          tmp >>= 1;
                      }
                  }
                  //   2. put bits (without HIR & TIR) back into bytes
                  int rcvindex=0;
	          for(i=0;i<nbytes;i++)
	          {
                      tmp=0;
                      for(j=7; j>=0; j--)
                      {
                          tmp <<= 1;
                          tmp |= (buf[i*8+j+hirbits]&1);
                      }
                      rcv[rcvindex++]=tmp;
                  }
               } //end of removing HIR & TIR           

                for(i=0;i<nbytes;i++)
		{
		    if (((rcv[i]^expect[i]) & rmask[i])!=0)
		    {
			if(db) printf("SIR read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcv[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF);
                	errcntr++;
		    }
		}
	    }
          }
	  // === Handling RUNTEST ===
	  else if(strcmp(Word[0],"RUNTEST")==0)
	  {
	    // printf("RUNTEST:  %d\n",pause);
            if (Count>1 && strcmp(Word[2],"SEC")==0)
            {
               //  if it is not "xxxxE-6 SEC", we have to use float number
               //  float fpause=0.;
               //  sscanf(Word[1],"%g",&fpause);
               sscanf(Word[1],"%d",&pause);
               if(pause>5) ::usleep(pause-5);
            }
            else
            {
	       sscanf(Word[1],"%d",&pause);
               if( pause<20 || (pause%10)>0 )
               {
                  // std::cout << "STATE: cycle idle state" << std::endl;
                  theController->CycleIdle_jtag(pause);
               }
               else
               {
                  // pause /= 2;
                  if(pause>=1000000)
                  {
                     // printf("pause %d seconds. ", pause/1000000);
                     ::sleep(pause/1000000);
                  }
                  else if(pause>5)
                  {
                     // sending a VME command needs more than 5 micro-sec,
                     // we can safely ignore those short delays
                     ::usleep(pause-5);
                  }
               }
            }
	  }
	  // === Handling STATE ===
	  else if((strcmp(Word[0],"STATE")==0))
	    {
          // the following different statements in SVF file:
          //   1)  STATE RESET; 
          //   2)  STATE IDLE;
          //   3)  STATE RESET IDLE;
          //   4)  STATE RESET; STATE IDLE;
          // all imply the same action: 
          //    ==> bring the TAP to RESET state, then to IDLE state which is required
          // for all other actions. And this action is exactly RestoreIdle().
		  RestoreIdle();
	    }
	  else if(strcmp(Word[0],"TRST")==0)
	    {
	      //          printf("TRST\n");
	    }
	  // === Handling ENDIR ===
	  else if(strcmp(Word[0],"ENDIR")==0)
	    {
	      //          printf("ENDIR\n");
	    }
	  // === Handling ENDDR ===
	  else if(strcmp(Word[0],"ENDDR")==0)
	    {
	    //	   printf("ENDDR\n");
	    }
	  // === Handling READ ===
	  else if(strcmp(Word[0],"READ")==0)
	  {
 	    sscanf(Word[1],"%d",&nbits);
            if (Count>2 && strcmp(Word[2],"REPEAT")==0) sscanf(Word[3],"%d",&repeat);
            if(repeat<=1) repeat=1;
            for(int jj=0; jj<repeat; jj++)
            {
               if(jj>0) ::usleep(50);
               nbytes=(nbits-1)/8+1;
               tbytes=(hdrbits+nbits+tdrbits-1)/8+1;
	       for (i=0; i< tbytes; i++)   realsnd[i] = 0;
	       read_packages++ ;
               std::cout << "Reading " << std::dec << read_packages << "..." << std::endl;
	       this->scan(DATA_REG, (char*)realsnd, hdrbits+nbits+tdrbits, (char*)rcv, verify); 
	       if (db)
	       {	
	           printf("Readback Data: ");
	           for (i=0; i< tbytes; i++)  printf("%02X",rcv[i]);
	           printf("\n");
	       }		    
	       // Next to extract real bitstream  (remove HDR and TDR bits if any)
               if(hdrbits==0)
               {
                   for(i=0;i<nbytes;i++)
                   {
                      bitstream[bitbufindex++]=rcv[i];
                   }
               }
               else
               {
                  //   1. expend bytes into bits
   	          for(i=0;i<tbytes;i++)
	          {
                      tmp=rcv[i];
                      for(j=0; j<8; j++)
                      {
                          buf[i*8+j]=tmp&1;
                          tmp >>= 1;
                      }
                  }
                  //   2. put bits (without HDR & TDR) back into bytes
	          for(i=0;i<nbytes;i++)
	          {
                      tmp=0;
                      for(j=7; j>=0; j--)
                      {
                          tmp <<= 1;
                          tmp |= (buf[i*8+j+hdrbits]&1);
                      }
                      bitstream[bitbufindex++]=tmp;
                  }
               } //end of extracting bitstream           
            }  // end of repeat
	  } //end of READ
	 // === Handling SBR & SER===
	 else if(strcmp(Word[0],"SBR")==0 || strcmp(Word[0],"SER")==0)
	 {
	      //std::cout << "SBR/SER" << std::endl;
            int exit_state=0;
            if(strcmp(Word[0],"SER")==0) exit_state=1;
	    sscanf(Word[1],"%d",&nbits);
	    nbytes=(nbits+7)/8;
            tbytes=(hdrbits+nbits+tdrbits+7)/8;
	    if (db)	  printf("Sending %d bits Data\n", nbits);
	    for(i=2;i<Count;i+=2)
	      {
	      if(strcmp(Word[i],"TDI")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&snd[j]);
		      //                printf("%2X",snd[j]);
		    }
		  //                printf("\n%d\n",nbytes);
		}
	      if(strcmp(Word[i],"SMASK")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&smask[j]);
		    }
		}
	      if(strcmp(Word[i],"TDO")==0)
		{
		  //if (db>2)             cmpflag=1;
		  cmpflag=1;
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&expect[j]);
		    }
		}
	      if(strcmp(Word[i],"MASK")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		    }
		}
	      }
            // Put send SDR here
	    for (i=0; i< tbytes; i++)
	      realsnd[i] = 0;
	    if (hdrbytes>0) {
	      for (i=0;i<hdrbytes;i++)
		realsnd[i]=sndhdr[i];
	    }
	    for (i=0;i<nbits;i++)
 	      realsnd[(i+hdrbits)/8] |= (snd[i/8] >> (i%8)) << ((i+hdrbits)%8);
	    if (tdrbytes>0) {
	      for (i=0;i<tdrbits;i++)
		realsnd[(i+hdrbits+nbits)/8] |= (sndtdr[i/8] >> (i%8)) << ((i+hdrbits+nbits)%8);
	    }	    
	    //
	    send_packages++ ;
            if(!readprom)
            {
               if ( (send_packages%one_pct)==0 ) 
                  std::cout << "Sending " << std::dec << send_packages/one_pct << "%..." << std::endl;
	       if ( send_packages == total_packages ) std::cout << "Done!" << std::endl;
            }
	    //
	    this->shift_bits(exit_state, (char*)realsnd, hdrbits+nbits+tdrbits, (char*)rcv, verify); 
	    //
	    if (db)
	    {	
	      printf("SBR Sent Data: ");
	      for (i=0; i< tbytes; i++) 
		printf("%02X",realsnd[i]);
	      printf("\n");
	      //
	      printf("SBR Readback Data: ");
	      for (i=0; i< tbytes; i++) 
		printf("%02X",rcv[i]);
	      printf("\n");
	    }		    
	    //
	    if (verify && cmpflag==1)
	    {     
               if(hdrbits>0)
               {
                  //   1. expend bytes into bits
   	          for(i=0;i<tbytes;i++)
	          {
                      tmp=rcv[i];
                      for(j=0; j<8; j++)
                      {
                          buf[i*8+j]=tmp&1;
                          tmp >>= 1;
                      }
                  }
                  //   2. put bits (without HDR & TDR) back into bytes
                  int rcvindex=0;
	          for(i=0;i<nbytes;i++)
	          {
                      tmp=0;
                      for(j=7; j>=0; j--)
                      {
                          tmp <<= 1;
                          tmp |= (buf[i*8+j+hdrbits]&1);
                      }
                      rcv[rcvindex++]=tmp;
                  }
               } //end of removing HDR & TDR           

	       for(i=0;i<nbytes;i++)
	       {
		   if (((rcv[i]^expect[i]) & rmask[i])!=0)
		   {
		      if(db) printf("SBR read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcv[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF);
		      errcntr++;
		   }
	       }	
	    }
         }
	 // === Handling SST ===
	 else if(strcmp(Word[0],"SST")==0)
	 {
	      //std::cout << "SST" << std::endl;
	    sscanf(Word[1],"%d",&nbits);
	    tbytes=(nbits+7)/8;
	    if (db)	  printf("Sending %d bits Data\n", nbits);
            int sndint;
	    for(i=2;i<Count;i+=2)
	      {
	      if(strcmp(Word[i],"TMS")==0)
		{
		    sscanf(&Word[i+1][1],"%8X",&sndint);
		}
	      }
	    this->shift_state(nbits, sndint); 
	    //
	    if (db)
	    {	
	      printf("SST Sent Data: ");
              printf("%08X\n",sndint);
	    }		    
	    //
         }
	}
    }
  // At the end of downloading, bring JTAG to RESET state.
  // Not absolutely necessary if it is always followed by a Hard-Reset.
  RestoreReset();
  this->endDevice();
  // turn off delay.
  theController->SetUseDelay(false);

  if(readprom && bitbufindex>0)
  {
      std::cout << "Total read back " <<  bitbufindex << " bytes from PROM." << std::endl;
  }
  fclose(dwnfp);
  return errcntr; 
}

// ====
// SVF File Parser module
// ====
void VMEModule::Parse(char *buf,int *Count,char **Word)
{

  //std::cout << buf << std::endl;

  *Word = buf;
  *Count = 0;
  while(*buf != '\0')  {
    while ((*buf==' ') || (*buf=='\t') || (*buf=='\n') || (*buf=='"')) *(buf++)='\0';
    if ((*buf != '\n') && (*buf != '\0'))  {
      Word[(*Count)++] = buf;

    }
    while ((*buf!=' ')&&(*buf!='\0')&&(*buf!='\n')&&(*buf!='\t')&&(*buf!='"')) {
      buf++;
    }
  }
  buf = '\0';
}


void VMEModule::SendOutput(std::string Output, std::string MessageType){
  //
  if(MessageType=="INFO") 
    {
    std::cout << "INFO INFO BEGIN" << std::endl 
              << Output << std::endl
              << "INFO INFO END" << std::endl; 
    }
  if(MessageType=="ERROR")
    {
    std::cout << "ERROR ERROR BEGIN" << std::endl 
              << Output << std::endl
              << "ERROR ERROR END" << std::endl; 
    }
  //
}

int VMEModule::new_vme(char fcn, unsigned vme, 
                       unsigned short data, char *rcv, int when) {
   theController->start( theSlot, boardType() );
   return theController->new_vme(fcn, vme, data, rcv, when);
}

// The following 5 wrapper functions can be used to build jumbo packet.
 
void VMEModule::write_later(unsigned  address, unsigned short data) 
{
  new_vme(VME_WRITE, address, data, NULL, LATER);
}

void VMEModule::read_later(unsigned  address) 
{
  new_vme(VME_READ, address, 0, NULL, LATER);
}

int VMEModule::write_now(unsigned  address, unsigned short data, char *rdbuf) 
{
  return new_vme(VME_WRITE, address, data, rdbuf, NOW);
}

int VMEModule::read_now(unsigned  address, char *rdbuf) 
{
  return new_vme(VME_READ, address, 0, rdbuf, NOW);
}

void VMEModule::vme_delay(unsigned short data)
{
  new_vme(VME_DELAY, 0, data, NULL, LATER);
}

int VMEModule::read_prom(const char * vrffile, const char * mcsfile)
{
   int tmp=0;

   bitstream=(char *)malloc(16*1024*1024);
   if(bitstream==NULL)
   {   
       std::cout << "ERROR: failed to allocate memory! Aborting..." << std::endl;
       return -1;
   }
   bzero(bitstream, 16*1024*1024);
   bitfile=fopen(mcsfile, "w");
   if(bitfile==NULL) 
   {  
       std::cout << "ERROR: failed to create mcs file " << mcsfile << "! Aborting..."<< std::endl;
       free (bitstream);
       return -2;
   }
   bitbufindex=0;
   svfLoad(tmp, vrffile, 0, 1);
   if(bitbufindex>0) 
   {    write_mcs(bitstream, bitbufindex, bitfile);
        std::cout << bitbufindex << " bytes of PROM image written to " << mcsfile << std::endl;
   }      
   fclose(bitfile);
   free(bitstream);   
   return 0;
}

void VMEModule::write_mcs(char *buf, int nbytes, FILE *outf)
{
   int segment=0, block=0, byte_index=0, total_blk, crc, i,j;
   int MAXBLOCK=4096;
   char tmp;

   total_blk=nbytes/16;
   if(nbytes%16>0) total_blk++;
   for (i=0; i<total_blk; i++)
   {
       if(block==0) 
       {  
          crc = 2+4+segment+(segment>>8);
          crc = ~crc+1;
          fprintf(outf, ":02000004%04X%02X\n", segment&0xFFFF, crc&0xFF);
       }
       fprintf(outf, ":10%04X00", (byte_index & 0xFFFF));
       crc=0x10+byte_index+(byte_index>>8);
       for(j=0;j<16;j++) 
       {
            if(byte_index>=nbytes) tmp=0xFF;
            else tmp=buf[byte_index++];
            fprintf(outf, "%02X", tmp&0xFF);
            crc += tmp;
       }
       crc = ~crc+1;
       fprintf(outf, "%02X\n", crc&0xFF);
       if(byte_index>=nbytes)
       {
            fprintf(outf, ":00000001FF\n");
            break;
       }
       block++;
       if(block==MAXBLOCK) 
       {   
            block=0;
            segment++;
       }
   }

}

void VMEModule::Jtag_Ohio(int dev, int reg,const char *snd,int cnt,char *rcv,int ird, int when)
{
// dev= device ID (0-0xF), bits 15-12 of VME address
// reg = 0 instruction shift;
//     = 1, data shift;
// when = 0  send vme packet LATER; 
//      = 1  send vme packet NOW;
int DEBUG=0;
int i;
int cnt2;
int byte,bit;
int tird[2]={0, 2};
int tiwt[2]={1, 3};
unsigned short int tmp[2]={0x0000};
unsigned short int *data;
unsigned int ptr_i;
unsigned int ptr_d;
unsigned int ptr_dh;
unsigned int ptr_ds;
unsigned int ptr_dt;
unsigned int ptr_r;
 
 if(dev<0 || dev>0xF) return;
 if(cnt==0)return;
 if(when!=0) when=1;
 if(ird==1 && reg==1) tiwt[1]=1;  // if READ is needed, then WRITEs are all buffered 

   if (DEBUG) {
      printf("Jtag_Ohio: dev=%d, reg=%d, cnt=%d, ird=%d, when=%d, Send %02x %02x\n",
              dev, reg, cnt, ird, when, snd[0]&0xff, snd[1]&0xff);
   }

 unsigned vme_base=(theSlot<<19)+(dev<<12);
 unsigned add_ds=vme_base;
 unsigned add_dh=vme_base+4;
 unsigned add_dt=vme_base+8;
 unsigned add_d=vme_base+0xC;
 unsigned add_r=vme_base+0x14;
 unsigned add_i=vme_base+0x1C;
 unsigned add_reset=vme_base+0x18;
 cnt2=cnt-1;
 data=(unsigned short int *) snd;

 /* instr */

 if(reg==0){
   if(cnt<0) 
   { 
     // cnt==-1   treated as Reset JTAG State Machine
     // data to write: anything
     ptr_i=add_reset;
   }
   else
   {
     ptr_i=add_i|(cnt2<<8);
   }
   theController->VME_controller(tiwt[when],ptr_i,data,rcv);
   return;
 }

 /* data */

 if(reg==1){
   byte=cnt/16;
   bit=cnt-byte*16;
   // printf(" bit byte %d %d \n",bit,byte);
   if(byte==0||(byte==1&&bit==0)){
     // single write
     ptr_d=add_d|(cnt2<<8);
     theController->VME_controller(tiwt[when],ptr_d,data,rcv);
     if(ird==1){
       ptr_r=add_r;
       theController->VME_controller(tird[when],ptr_r,tmp,rcv);
     }
     return;
   }
   // below for multiple writes
   // step 1. write 1 full word with header, no trailer 
   ptr_dh=add_dh|0x0f00;
   theController->VME_controller(1,ptr_dh,data,rcv);
   data=data+1;
   if(ird==1){       
     ptr_r=add_r;
     theController->VME_controller(0,ptr_r,tmp,rcv);
   }
   ptr_ds=add_ds;
   for(i=0;i<byte-1;i++){
     if(i==(byte-2)&&bit==0){
       // if this is the last full word with no more extra bits
       // step 3. write 1 full word with trailer
       ptr_dt=add_dt|0x0f00;
       theController->VME_controller(tiwt[when],ptr_dt,data,rcv);
       if(ird==1){
          ptr_r=add_r;
          theController->VME_controller(tird[when],ptr_r,data,rcv);
       }
       return;
     }else{
       // middle part
       // step 2. write 1 full word only, no header or trailer
       ptr_ds=add_ds|0x0f00;
       theController->VME_controller(1,ptr_ds,data,rcv);
       data=data+1;
       if(ird==1){
         ptr_r=add_r;
         theController->VME_controller(0,ptr_r,tmp,rcv);
       }
     }
   }
   // if the last few bits smaller than a full word
   // step 4. write bits with trailer
   cnt2=bit-1;
   ptr_dt=add_dt|(cnt2<<8);
   theController->VME_controller(tiwt[when],ptr_dt,data,rcv);
   if(ird==1){
     ptr_r=add_r;
     theController->VME_controller(tird[when],ptr_r,tmp,rcv);
   }
   return;
 }
}

  } // namespace emu::pc
} // namespace emu
