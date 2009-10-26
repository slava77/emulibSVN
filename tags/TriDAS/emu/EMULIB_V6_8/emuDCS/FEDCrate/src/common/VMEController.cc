//#define CAEN_DEBUG 1
/*****************************************************************************\
* $Id: VMEController.cc,v 3.21 2008/10/01 14:10:04 paste Exp $
*
* $Log: VMEController.cc,v $
* Revision 3.21  2008/10/01 14:10:04  paste
* Fixed phantom reset bug in IRQ threads and shifted IRQ handling functions to VMEController object.
*
* Revision 3.20  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 3.19  2008/09/22 14:31:54  paste
* /tmp/cvsY7EjxV
*
* Revision 3.18  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.17  2008/09/07 22:25:36  paste
* Second attempt at updating the low-level communication routines to dodge common-buffer bugs.
*
* Revision 3.16  2008/09/03 17:52:59  paste
* Rebuilt the VMEController and VMEModule classes from the EMULIB_V6_4 tagged versions and backported important changes in attempt to fix "high-bits" bug.
*
* Revision 3.15  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.14  2008/08/19 14:51:02  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 3.13  2008/08/15 16:14:51  paste
* Fixed threads (hopefully).
*
* Revision 3.12  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 3.11  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "VMEController.h"

#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
//#include <string.h>

#include <sys/time.h> // POSIX gettimeofday routine for microsecond timers

#include "CAENVMElib.h"
#include "CAENVMEtypes.h"
#include "VMEModule.h"

/*
#define DELAY2 0.016
#define DELAY3 16.384

extern unsigned long vmeadd;
//
// the following variables must be kept global to assure
// no conflict when running EmuFRunControlHyperDAQ and
// EmuFCrateHyperDAQ simultaneously
//

extern int delay_type;
long OpenBHandle[4][4] = {{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1}};


int delay_type;
char adcreg[5]={0x00,0x0e,0x0d,0x0b,0x07};
char adcbyt[3][8]={{0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9},{0x89,0x99,0xa9,0xb9,0xc9,0xd9,0xe9,0xf9},{0x85,0x95,0xa5,0xb5,0xc5,0xd5,0xe5,0xf5}};

unsigned short int tird[3]={1,1,3};

int pows(int n,int m);

char *bufvme;
int cntvme;
int cntvme_rd;

int ife;

unsigned long vmeadd;
unsigned long vmeadd_tmp;
unsigned long add_i,add_d,add_dh,add_ds,add_dt,add_sw,add_sr,add_rst,add_r;
unsigned long add_reset;
unsigned long add_control_r;
unsigned long add_vmepara;
unsigned long add_vmeser;
unsigned long add_dcc_r,add_dcc_w;
unsigned long add_adcr,add_adcw,add_adcrbb,add_adcws,add_adcrs;

unsigned long msk00={0x00000000};
unsigned long msk01={0x00001000};
unsigned long msk02={0x00002000};
unsigned long msk03={0x00003000};
unsigned long msk04={0x00004000};
unsigned long msk05={0x00005000};
unsigned long msk06={0x00006000};
unsigned long msk07={0x00007000};
unsigned long msk08={0x00008000};
unsigned long msk09={0x00009000};
unsigned long msk0d={0x0000d000};
unsigned long msk7f={0x0007fffe};
unsigned long msk0f={0x0000fffe};

unsigned long msk_clr={0xfffff0ff};
unsigned long msk_rst={0x00000018};
unsigned long msk_i={0x0000001c};
unsigned long msk_d={0x0000000c};
unsigned long msk_dh={0x00000004};
unsigned long msk_ds={0x00000000};
unsigned long msk_dt={0x00000008};
unsigned long msk_sw={0x00000020};
unsigned long msk_sr={0x00000024};
unsigned long msk_r={0x00000014};
unsigned long msk_control_r={0x00082800};
unsigned long msk_vmeser={0x00040000};
unsigned long msk_vmepara={0x00030000};
unsigned long msk_dcc_r={0x00000000};
unsigned long msk_dcc_w={0x00000000};
unsigned long msk_adcr={0x00000004};
unsigned long msk_adcw={0x00000000};
unsigned long msk_adcrbb={0x0000000c};
unsigned long msk_adcws={0x00000020};
unsigned long msk_adcrs={0x00000024};

#define debugV 0
*/

emu::fed::VMEController::VMEController(int Device, int Link)
	throw (FEDException): 
	Device_(Device),
	Link_(Link),
	BHandle_(-1)
	//currentModule_(0)
	//endian_(SWAP)
{
	CVBoardTypes VMEBoard = cvV2718;

	int32_t BHandle;

	CVErrorCodes err = CAENVME_Init(VMEBoard, Device_, Link_, &BHandle);

	if (err != cvSuccess) {
		std::ostringstream error;
		error << "error " << err << ": " << CAENVME_DecodeError(err);
		//XCEPT_RAISE(FEDException, error.str());
	} else {
		BHandle_ = BHandle;
	}
	
	// Initialize mutexes
	pthread_mutex_init(&mutex_, NULL);

}



emu::fed::VMEController::~VMEController() {
	//std::cout << "destructing VMEController .. closing socket " << std::endl;
	CAENVME_End(BHandle_);
}



bool emu::fed::VMEController::waitIRQ(unsigned int mSecs)
throw (FEDException)
{
	// If the BHandle is not set properly, just return a good signal (true)
	if (BHandle_ < 0) return true;
	
	pthread_mutex_lock(&mutex_);
	CVErrorCodes err = CAENVME_IRQEnable(BHandle_, cvIRQ1);
	pthread_mutex_unlock(&mutex_);
	
	if (err != cvSuccess) {
		std::ostringstream error;
		error << "error " << err << ": " << CAENVME_DecodeError(err);
		if (err == cvBusError) {
			//std::cerr << error.str() << std::endl;
			//std::cerr << "    sleeping it off..." << std::endl;
			//sleep((unsigned int) 1);
		} else {
			XCEPT_RAISE(FEDException, error.str());
			//std::cerr << error.str() << std::endl;
		}
	}
	
	pthread_mutex_lock(&mutex_);
	bool status = CAENVME_IRQWait(BHandle_, cvIRQ1, mSecs);
	pthread_mutex_unlock(&mutex_);
	return status;
}


uint16_t emu::fed::VMEController::readIRQ()
throw (FEDException)
{
	// If the BHandle is not set properly, return nothing
	if (BHandle_ < 0) return 0;
	
	uint16_t errorData;
	
	pthread_mutex_lock(&mutex_);
	CVErrorCodes err = CAENVME_IACKCycle(BHandle_, cvIRQ1, &errorData, cvD16);
	pthread_mutex_unlock(&mutex_);
	
	if (err != cvSuccess) {
		std::ostringstream error;
		error << "error " << err << ": " << CAENVME_DecodeError(err);
		if (err == cvBusError) {
			//std::cerr << error.str() << std::endl;
			//std::cerr << "    sleeping it off..." << std::endl;
			//sleep((unsigned int) 1);
		} else {
			XCEPT_RAISE(FEDException, error.str());
			//std::cerr << error.str() << std::endl;
		}
	}
	
	return errorData;
}


/*
void emu::fed::VMEController::start(int slot){
  vmeadd=slot<<19;
}



void emu::fed::VMEController::end() {
  if(currentModule_ != 0) {
    currentModule_->end();
    currentModule_ = 0;
  }
  assert(plev !=2);
  idevo_ = 0;
  feuseo = 0;
}


void emu::fed::VMEController::send_last() {
}


int emu::fed::VMEController::CAEN_reset(void)
{ 
  return 0;
}

void emu::fed::VMEController::CAEN_err_reset(void) {
	caen_err = 0;
	return;
}

int emu::fed::VMEController::CAEN_read(unsigned long Address,unsigned short int *data)
{
	int err;
	CVAddressModifier AM=cvA24_U_DATA;
	CVDataWidth DW=cvD16;
// printf("BHandle_ %08x \n",BHandle_);
// printf(" +++++ CAENVME read sent +++++\n");
#ifdef CAEN_DEBUG
	std::clog << std::hex << "Old Read  BHandle_(" << BHandle_ << ") Address(" << Address << ") " << std::flush;
#endif
	err=CAENVME_ReadCycle(BHandle_,Address,data,AM,DW);
#ifdef CAEN_DEBUG
	std::clog << std::hex << "data(" << (uint16_t) *data << ")" << std::flush << std::endl;
#endif
	if(err!=0){
		caen_err=err;
		printf(" CAENVME read err %d \n",caen_err);
	//printf(" read: address %08x data %04x \n",Address,*data);
	}
	return err;
}


int emu::fed::VMEController::CAEN_write(unsigned long Address,unsigned short int *data)
{
	int err = 0;
	CVAddressModifier AM=cvA24_U_DATA;
	CVDataWidth DW=cvD16;

	//printf(" write: handle %d address %08x data %04x AM %d DW %d \n",BHandle_,Address,*data,AM,DW);
#ifdef CAEN_DEBUG
	std::clog << std::hex << "Old Write BHandle_(" << BHandle_ << ") Address(" << Address << ") data(" << (uint16_t) *data << ")" << std::flush << std::endl;
#else
	err=CAENVME_WriteCycle(BHandle_,Address,(char *)data,AM,DW);
#endif
	if(err!=0){
		caen_err=err;
		printf(" CAENVME write err %d \n",caen_err);
	}
	// JG, temporary!   usleep(1000);
	return err;
}


void emu::fed::VMEController::CAEN_close(void)
{
    CAENVME_End(BHandle_);
}


void emu::fed::VMEController::vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv)
{
//printf("vme_controller with irdwr %d ptr %08x data %04x rcv %08x\n",irdwr,ptr,*data,rcv);
static int ird=0;
static long int packet_delay=0;
char rdata[2];

long unsigned int pttr;
// irdwr:
//               0 bufread
//               1 bufwrite 
//               2 bufread snd  
//               3 bufwrite snd 
//               4 flush to VME
//               5 loop back 
//               6 delay

// LOG4CPLUS_INFO(getApplicationLogger(), " EmuFEDVME: Inside controller");
 pttr=(long unsigned int)ptr;
 if(irdwr==0){
   CAEN_read(pttr,(unsigned short int *)rdata);
   rcv[ird]=rdata[0];
   ird=ird+1;
   rcv[ird]=rdata[1];
   ird=ird+1;
 }else if(irdwr==1){ 
   CAEN_write(pttr,data);
 }else if(irdwr==2){
   CAEN_read(pttr,(unsigned short int *)rdata);
   rcv[ird]=rdata[0];
   ird=ird+1;
   rcv[ird]=rdata[1];
   ird=ird+1;
   ird=0;
 }else if(irdwr==3){
   CAEN_write(pttr,data);
 }else if(irdwr==6){
   if(delay_type==2)packet_delay= (long int) ((*data)*DELAY2);
   if(delay_type==3)packet_delay= (long int) ((*data)*DELAY3);
   // printf(" packet_delay %d %ld \n",*data,packet_delay);
   udelay(packet_delay);
 }

}


int emu::fed::VMEController::udelay(long int itim)
{
	timeval startTime;
	timeval endTime;
	gettimeofday(&startTime,NULL);
  int i,j;
  for(j=0;j<itim;j++){
      for(i=0;i<200;i++);
  }
	gettimeofday(&endTime,NULL);
	unsigned long int diffTime = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	std::clog << "--udelay time: " << diffTime << " microseconds" << std::endl;
  return 0; 
}






void emu::fed::VMEController::devdo(enum DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd)
{
	char cmd2[9000];
	
	char tmp[4];
	int kbit,kbybit;
	char kbypass;
	
	int ppnt,pcmd,pow2;
	int idev,i,k,m;
	int ncmd2,nbcmd2,nbuf2;
	
	
	int init;
	
	
	unsigned short int ishft,temp;
	unsigned long int vmeaddo;
	static int feuse;  

//  irdsnd for jtag
// 			irdsnd = 0 send immediately, no read
// 			irdsnd = 1 send immediately, read
// 			irdsnd = 2 send in buffer, no read

	if(dev!=99){
	idev=geo[dev].jchan;
	}else{
		idev=idevo_;
		if(idev>4&idev!=12)return;
	}
	// printf(" enter devdo %d %d \n",dev,idev);
	
	// printf(" idev idevo_ dev %d %d %d \n",idev,idevo_,dev);

	init=0;
	if(idev!=idevo_||vmeadd!=vmeaddo){
		init=1;
	}
	idevo_=idev;
	vmeaddo=vmeadd;

  //  printf(" about to initialize plev idve devo init %d %d %d %d \n",plev,idev,idevo_,init);


  switch(idev){

   case 1:  
     if(init==1){ 
      feuse=0xff;
      ife=1;
      add_i=vmeadd|msk01|msk_i;
      add_d=vmeadd|msk01|msk_d;
      add_dh=vmeadd|msk01|msk_dh;
      add_ds=vmeadd|msk01|msk_ds;
      add_dt=vmeadd|msk01|msk_dt;
      add_rst=vmeadd|msk01|msk_rst;
      add_sw=vmeadd|msk01|msk_sw;
      add_sr=vmeadd|msk01|msk_sr;
      add_r=vmeadd|msk01|msk_r;
     }
   break;

   case 2:   
     if(init==1){
      feuse=0xff;
      ife=0; 
      add_i=vmeadd|msk02|msk_i;
      add_d=vmeadd|msk02|msk_d;
      add_dh=vmeadd|msk02|msk_dh;
      add_ds=vmeadd|msk02|msk_ds;
      add_dt=vmeadd|msk02|msk_dt;
      add_rst=vmeadd|msk02|msk_rst;
      add_sw=vmeadd|msk02|msk_sw;
      add_sr=vmeadd|msk02|msk_sr;
      add_r=vmeadd|msk02|msk_r;
     }
   break;

   case 3:   
     if(init==1){
      ife=0;
      // printf(" init daqmb prom \n");
      add_i=vmeadd|msk03|msk_i;
      add_d=vmeadd|msk03|msk_d;
      add_dh=vmeadd|msk03|msk_dh;
      add_ds=vmeadd|msk03|msk_ds;
      add_dt=vmeadd|msk03|msk_dt;
      add_rst=vmeadd|msk03|msk_rst;
      add_sw=vmeadd|msk03|msk_sw;
      add_sr=vmeadd|msk03|msk_sr;
      add_r=vmeadd|msk03|msk_r;    
     }
   break;

  case 4:  
    if(init==1){
     feuse=0xff;
     ife=0;
     // printf(" init daqmb vme prom \n");
     add_i=vmeadd|msk04|msk_i;
     add_d=vmeadd|msk04|msk_d;
     add_dh=vmeadd|msk04|msk_dh;
     add_ds=vmeadd|msk04|msk_ds;
     add_dt=vmeadd|msk04|msk_dt;
     add_rst=vmeadd|msk04|msk_rst;
     add_sw=vmeadd|msk04|msk_sw;
     add_sr=vmeadd|msk04|msk_sr;
     add_r=vmeadd|msk04|msk_r;    
    }
   break;

  case 5:  
    if(init==1){
     feuse=0xff;
     ife=0;
     // printf(" init daqmb vme prom \n");
     add_i=vmeadd|msk05|msk_i;
     add_d=vmeadd|msk05|msk_d;
     add_dh=vmeadd|msk05|msk_dh;
     add_ds=vmeadd|msk05|msk_ds;
     add_dt=vmeadd|msk05|msk_dt;
     add_rst=vmeadd|msk05|msk_rst;
     add_sw=vmeadd|msk05|msk_sw;
     add_sr=vmeadd|msk05|msk_sr;
     add_r=vmeadd|msk05|msk_r;    
    }
   break;

  case 6:   
    if(init==1){
     feuse=0xff;
     ife=0;
     // printf(" init daqmb vme prom \n");
     add_i=vmeadd|msk06|msk_i;
     add_d=vmeadd|msk06|msk_d;
     add_dh=vmeadd|msk06|msk_dh;
     add_ds=vmeadd|msk06|msk_ds;
     add_dt=vmeadd|msk06|msk_dt;
     add_rst=vmeadd|msk06|msk_rst;
     add_sw=vmeadd|msk06|msk_sw;
     add_sr=vmeadd|msk06|msk_sr;
     add_r=vmeadd|msk06|msk_r;    
    }
   break;

  case 7:   
    if(init==1){
     feuse=0xff;
     ife=0;
     // printf(" init daqmb vme prom \n");
     add_i=vmeadd|msk07|msk_i;
     add_d=vmeadd|msk07|msk_d;
     add_dh=vmeadd|msk07|msk_dh;
     add_ds=vmeadd|msk07|msk_ds;
     add_dt=vmeadd|msk07|msk_dt;
     add_rst=vmeadd|msk07|msk_rst;
     add_sw=vmeadd|msk07|msk_sw;
     add_sr=vmeadd|msk07|msk_sr;
     add_r=vmeadd|msk07|msk_r;    
    }
   break;

  case 8:   
    if(init==1){
     feuse=0xff;
     ife=0;
     add_i=vmeadd|msk08|msk_i;
     add_d=vmeadd|msk08|msk_d;
     add_dh=vmeadd|msk08|msk_dh;
     add_ds=vmeadd|msk08|msk_ds;
     add_dt=vmeadd|msk08|msk_dt;
     add_rst=vmeadd|msk08|msk_rst;
     add_sw=vmeadd|msk08|msk_sw;
     add_sr=vmeadd|msk08|msk_sr;
     add_r=vmeadd|msk08|msk_r;    
    }
   break;

  case 9:
     add_vmepara=vmeadd|msk_vmepara;
     vmepara(cmd,inbuf,outbuf);
   break;
 
  case 10: 
     add_vmeser=vmeadd|msk_vmeser;
     vmeser(cmd,inbuf,outbuf);
   break;

  case 11: 
     add_dcc_r=vmeadd|msk00|msk_dcc_r;
     add_dcc_w=vmeadd|msk00|msk_dcc_w;
     dcc(cmd,outbuf); 
   break;

   case 12:   
     if(init==1){
       feuse=0x99;
       ife=99;
       add_reset=vmeadd|msk0f;
     }
   break;

   case 13:
     add_adcr=vmeadd|msk0d|msk_adcr;
     add_adcw=vmeadd|msk0d|msk_adcw;
     add_adcrbb=vmeadd|msk0d|msk_adcrbb;
     add_adcrs=vmeadd|msk0d|msk_adcrs; 
     add_adcws=vmeadd|msk0d|msk_adcws; 
     vme_adc(cmd[0],cmd[1],outbuf);
    break;  
  }


	if(idev<=8||idev==12){
		if(ncmd>0){
// 			 stan jtag kludge kludge for serial devices 
			if(geo[dev].nser!=0){
				ppnt=0;
				cmd2[0]=0x00;
				cmd2[1]=0x00;
				m=geo[dev].nser;
				for(i=0;i<geo[dev].nser;i++){
					if(geo[dev].seri[m-i-1]<0) {
						tmp[0]=cmd[0];
						pcmd=geo[-1*geo[dev].seri[m-i-1]].kbit;
					} else {
						tmp[0]=geo[geo[dev].seri[m-i-1]].kbypass;
						pcmd=geo[geo[dev].seri[m-i-1]].kbit;
					}


					// Take a look at this code:
					
					for(k=0;k<pcmd;k++){
						ppnt=ppnt+1;
						if(((tmp[0]>>k)&0x01)!=0){
							if(ppnt<9) {
								pow2=pows(2,ppnt-1);
								cmd2[0]=cmd2[0]+pow2;
							}
							if(ppnt>8) {
								pow2=pows(2,ppnt-9);
								cmd2[1]=cmd2[1]+pow2;
							}
						}
					}
					

					// PGK Now take a look at this:
					
					// Make a bit mask of pcmd bits
					
// 					ppnt = (1 << pcmd) - 1;
// 
// 					// Load cmd2 with the value in tmp, masked by ppnt.
// 					cmd2[0] += tmp[0] & (ppnt & 0xff);
// 					cmd2[1] += tmp[1] & ((ppnt >> 8) & 0xff);
// 
// 					// And add the ppnt
// 					ppnt += pcmd;
					
					// Which do you prefer?
				}
				
				ncmd2=ppnt;
				nbcmd2=ncmd2/8+1;
				nbuf2=geo[dev].sxtrbits;
				if(nbuf>0){
					nbuf2=nbuf2+nbuf;
				} else {
					nbuf2=0;
				}
				kbit=geo[dev].kbit;
				kbybit=geo[dev].kbybit;
				kbypass=geo[dev].kbypass;

			} else {
				nbuf2=nbuf;
				ncmd2=ncmd;
				k=ncmd2/8+1;
				if(k>100)printf(" ****************CATASTROPY STOP STOP ");
				for(i=0;i<k;i++){
					cmd2[i]=cmd[i];
				}
			}
		// printf(" ********** %s dev prev_dev %d %d \n",geo[dev].nam,dev,prev_dev);
// 		 end stan kludge
		}
	}

switch(idev){
    case 1:  
      if(ncmd==-99){sleep_vme(cmd);break;}    
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd); 
     if(irdsnd==1&&nbuf2%16!=0){
        ishft=16-nbuf2%16;
        temp=((outbuf[nbuf2/8+1]<<8)&0xff00)|(outbuf[nbuf2/8]&0xff);
	temp=(temp>>ishft);
        outbuf[nbuf2/8+1]=(temp&0xff00)>>8;
        outbuf[nbuf2/8]=temp&0x00ff;
      }

    break;
    case 2:
      if(ncmd==-99){sleep_vme(cmd);break;}        
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd); 
     if(irdsnd==1&&nbuf2%16!=0){
        ishft=16-nbuf2%16;
        temp=((outbuf[nbuf2/8+1]<<8)&0xff00)|(outbuf[nbuf2/8]&0xff);
	temp=(temp>>ishft);
        outbuf[nbuf2/8+1]=(temp&0xff00)>>8;
        outbuf[nbuf2/8]=temp&0x00ff;
      }

    break;
    case 3: 
      if(ncmd==-99){sleep_vme(cmd);break;}   
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0&nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd); 
     if(irdsnd==1&&nbuf2%16!=0){
        ishft=16-nbuf2%16;
        temp=((outbuf[nbuf2/8+1]<<8)&0xff00)|(outbuf[nbuf2/8]&0xff);
	temp=(temp>>ishft);
        outbuf[nbuf2/8+1]=(temp&0xff00)>>8;
        outbuf[nbuf2/8]=temp&0x00ff;
      }

    break;
    case 4:
      if(ncmd==-99){sleep_vme(cmd);break;}
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd); 
     if(irdsnd==1&&nbuf2%16!=0){
        ishft=16-nbuf2%16;
        temp=((outbuf[nbuf2/8+1]<<8)&0xff00)|(outbuf[nbuf2/8]&0xff);
	temp=(temp>>ishft);
        outbuf[nbuf2/8+1]=(temp&0xff00)>>8;
        outbuf[nbuf2/8]=temp&0x00ff;
      }

    break;
    case 5:
      if(ncmd==-99){sleep_vme(cmd);break;}
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd); 
     if(irdsnd==1&&nbuf2%16!=0){
        ishft=16-nbuf2%16;
        temp=((outbuf[nbuf2/8+1]<<8)&0xff00)|(outbuf[nbuf2/8]&0xff);
	temp=(temp>>ishft);
        outbuf[nbuf2/8+1]=(temp&0xff00)>>8;
        outbuf[nbuf2/8]=temp&0x00ff;
      }

    break;
    case 6:
      if(ncmd==-99){sleep_vme(cmd);break;}
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd);
      if(irdsnd==1&&nbuf2%16!=0){
        ishft=16-nbuf2%16;
        temp=((outbuf[nbuf2/8+1]<<8)&0xff00)|(outbuf[nbuf2/8]&0xff);
	temp=(temp>>ishft);
        outbuf[nbuf2/8+1]=(temp&0xff00)>>8;
        outbuf[nbuf2/8]=temp&0x00ff;   
      }

    break;
    case 7:
      if(ncmd==-99){sleep_vme(cmd);break;}
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd);
      if(irdsnd==1&&nbuf2%16!=0){
        ishft=16-nbuf2%16;
        temp=((outbuf[nbuf2/8+1]<<8)&0xff00)|(outbuf[nbuf2/8]&0xff);
	temp=(temp>>ishft);
        outbuf[nbuf2/8+1]=(temp&0xff00)>>8;
        outbuf[nbuf2/8]=temp&0x00ff;
      }

    break;
    case 8:
      if(ncmd==-99){sleep_vme(cmd);break;}
      if(ncmd<0){RestoreIdle();break;}
      if(ncmd>0){
      if(nbuf>0){
        scan(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan(DATA_REG,inbuf,nbuf2,outbuf,irdsnd); 
      if(irdsnd==1&&nbuf2%16!=0){
        ishft=16-nbuf2%16;
        temp=((outbuf[nbuf2/8+1]<<8)&0xff00)|(outbuf[nbuf2/8]&0xff);
	temp=(temp>>ishft);
        outbuf[nbuf2/8+1]=(temp&0xff00)>>8;
        outbuf[nbuf2/8]=temp&0x00ff;
      }
    break;
   case 12: 
      // printf(" reset vme prom ncmd2 %d %d nbuf2 %d \n",ncmd2,ncmd,nbuf2);     
      if(ncmd==-99){sleep_vme(cmd);break;}
      if(ncmd<0){RestoreIdle_reset();break;}
      if(ncmd>0){
      if(nbuf>0){
	scan_reset(INSTR_REG,cmd2,ncmd2,outbuf,0);
      }else{
        scan_reset(INSTR_REG,cmd2,ncmd2,outbuf,irdsnd);
      }
      }
      if(nbuf>0)scan_reset(DATA_REG,inbuf,nbuf,outbuf,irdsnd);
    break;

}

}


void emu::fed::VMEController::scan(int reg,const char *snd,int cnt,char *rcv,int ird)
{
int i;
int cnt2;

int byte,bit;
unsigned short int tmp[2]={0x0000};
unsigned short int *data;

unsigned short int *ptr_i;
unsigned short int *ptr_d;
unsigned short int *ptr_dh;
unsigned short int *ptr_ds;
unsigned short int *ptr_dt;
unsigned short int *ptr_r;
 
if(cnt==0)return;


 cnt2=cnt-1;
 // printf(" ****** cnt cnt2 %d %d \n",cnt,cnt2);
 // printf(" reg ird %d %d \n",reg,ird);
 data=(unsigned short int *) snd;



 if(reg==0){
   add_i=add_i&msk_clr;
   add_i=add_i|(cnt2<<8);
   ptr_i=(unsigned short int*)add_i;
   bit=cnt; 
   // xif(bit>8)*ptr_i=*data;
   // xif(bit<=8)*ptr_i=((*data)>>8);
   // if(bit<=8)*data=((*data)>>8);
   // printf(" 1 VME W: %08x %04x \n",ptr_i,*data);
   vme_controller(tird[ird],ptr_i,data,rcv);
   return;
 }



  if(reg==1){
   byte=cnt/16;
   bit=cnt-byte*16;
   // printf(" bit byte %d %d \n",bit,byte);
   if(byte==0|(byte==1&bit==0)){
     add_d=add_d&msk_clr;
     add_d=add_d|(cnt2<<8);
     ptr_d=(unsigned short int *)add_d; 
     // printf(" 2 VME W: %08x %04x \n",ptr_d,*data);
     // xif(bit>8|byte==1)*ptr_d=*data;
     // xif(bit<=8&byte!=1)*ptr_d=((*data)>>8);
     // if(bit<=8&byte!=1)*data=((*data)>>8);
     vme_controller(tird[ird],ptr_d,data,rcv);
     //  printf("2 VME W: %08x %04x \n",ptr_dh,*data);
     if(ird==1){
       ptr_r=(unsigned short int *)add_r;
       // x*data2=*ptr_r;
       // printf(" R %08x \n",ptr_r);
       vme_controller(2,ptr_r,tmp,rcv);
     }
      return;
   }
  add_dh=add_dh&msk_clr;
  add_dh=add_dh|0x0f00;
  ptr_dh=(unsigned short int *)add_dh;
  // printf(" 3 VME W: %08x %04x \n",ptr_dh,*data);
  vme_controller(1,ptr_dh,data,rcv);
  // x*ptr_dh=*data;
  data=data+1;
  if(ird==1){       
     ptr_r=(unsigned short int *)add_r;
     // printf("3 R %08x \n",ptr_r);
     vme_controller(0,ptr_r,tmp,rcv);
     // x*data2=*ptr_r; 
     // printf(" rddata %04x \n",*data2);
  }
  add_ds=add_ds&msk_clr;
  ptr_ds=(unsigned short int *)add_ds;
  for(i=0;i<byte-1;i++){
    if(i==byte-2&bit==0){
      add_dt=add_dt&msk_clr;
      add_dt=add_dt|0x0f00;
      ptr_dt=(unsigned short int *)add_dt;
      // printf("4 VME W: %08x %04x \n",ptr_dt,*data);
      vme_controller(tird[ird],ptr_dt,data,rcv);
      // x*ptr_dt=*data;
      if(ird==1){
        ptr_r=(unsigned short int *)add_r;
	//  printf("4 R %08x \n",ptr_r);
        vme_controller(2,ptr_r,data,rcv);
        // x*data2=*ptr_r;  
        // printf(" rddata %04x \n",*data2);
      }
      return;
    }else{
      add_ds=add_ds&msk_clr;
      add_ds=add_ds|0x0f00;
      ptr_ds=(unsigned short *)add_ds;
      // printf("5 VME W: %08x %04x \n",ptr_ds,*data);
      vme_controller(1,ptr_ds,data,rcv);
      // x*ptr_ds=*data;
      data=data+1;
      if(ird==1){
        ptr_r=(unsigned short int *)add_r;
        // printf(" R %08x \n",ptr_r);
        vme_controller(0,ptr_r,tmp,rcv);
        // x*data2=*ptr_r; 
	// printf(" rddata %04x \n",*data2);
      }
    }
  }
  cnt2=bit-1;
  add_dt=add_dt&msk_clr;
  add_dt=add_dt|(cnt2<<8);
  ptr_dt=(unsigned short int *)add_dt; 
  // printf("6 VME W: %08x %04x \n",ptr_dt,*data);
  // xif(bit>8)*ptr_dt=*data;
  // xif(bit<=8)*ptr_dt=*data>>8;
  // if(bit<=8)*data=*data>>8;
  vme_controller(tird[ird],ptr_dt,data,rcv);
  if(ird==1){
     ptr_r=(unsigned short int *)add_r;
     // printf(" R %08x \n",ptr_r);
     vme_controller(2,ptr_r,tmp,rcv);
     // x*data2=*ptr_r; 
     // printf(" rddata %04x \n",*data2);
  }

  return;
 }
}


void emu::fed::VMEController::initDevice(int idev) {
  if(debugV)std::cout << "InitDevice " << idev << std::endl;
    vmeadd=0x00000000;
}





void emu::fed::VMEController::RestoreIdle()
{
 char tmp[2]={0x00,0x00};
 unsigned short int tmp2[1]={0x0000};
unsigned short int *ptr_rst;
 ptr_rst=(unsigned short *)add_rst;
 // printf(" call restore idle %08x %08x \n",ptr_rst,add_rst);
  vme_controller(3,ptr_rst,tmp2,tmp);

}


void emu::fed::VMEController::InitJTAG(int port)
{
}


void emu::fed::VMEController::CloseJTAG()
{ 

}





int pows(int n,int m)
{int l,i;
l=1;
 for(i=0;i<m;i++)l=l*n;
return l;
}

void emu::fed::VMEController::RestoreIdle_reset()
{
unsigned short int one[1]={0x01};
char tmp[2]={0x00,0x00};
unsigned short int *ptr;
 int i;
 ptr=(unsigned short int *)add_reset;
 for(i=0;i<5;i++){vme_controller(1,ptr,one,tmp);sdly();}
}






void  emu::fed::VMEController::scan_reset(int reg,const char *snd, int cnt, char *rcv,int ird)
{
int i,j;

int byte,bit;
unsigned short int x00[1]={0x00};
unsigned short int x01[1]={0x01};
unsigned short int x02[1]={0x02};
unsigned short int x03[1]={0x03};
unsigned short int ival,ival2;
unsigned short int *data;
unsigned short int *ptr;
 if(cnt==0)return;
 ptr=(unsigned short int *)add_reset;
 data=(unsigned short int *) snd;
 // printf("scan_reset %d %d %02x %02x \n",reg,cnt,snd[1]&0xff,snd[0]&0xff);


 if(reg==0){
    vme_controller(1,ptr,x00,rcv);sdly();
    vme_controller(1,ptr,x00,rcv);sdly();
    vme_controller(1,ptr,x01,rcv);sdly();
    vme_controller(1,ptr,x01,rcv);sdly();
    vme_controller(1,ptr,x00,rcv);sdly();
    vme_controller(1,ptr,x00,rcv);sdly();
 }



 if(reg==1){ 
    vme_controller(1,ptr,x00,rcv);sdly();
    vme_controller(1,ptr,x00,rcv);sdly();
    vme_controller(1,ptr,x01,rcv);sdly();
    vme_controller(1,ptr,x00,rcv);sdly();
    vme_controller(1,ptr,x00,rcv);sdly();
 }
 byte=cnt/16;
 bit=cnt-byte*16;
 for(i=0;i<byte;i++){
   for(j=0;j<16;j++){
      ival=*data>>j;
      ival2=ival&0x01;
      if(i!=byte-1|bit!=0|j!=15){
        if(ival2==0){vme_controller(1,ptr,x00,rcv);sdly();}
        if(ival2==1){vme_controller(1,ptr,x02,rcv);sdly();}
      }else{
        if(ival2==0){vme_controller(1,ptr,x01,rcv);sdly();}
        if(ival2==1){vme_controller(1,ptr,x03,rcv);sdly();}
      }
   }
   data=data+1;
 }  
 for(j=0;j<bit;j++){
   ival=*data>>j;
   ival2=ival&0x01;
   if(j<bit-1){
     if(ival2==0){vme_controller(1,ptr,x00,rcv);sdly();}
     if(ival2==1){vme_controller(1,ptr,x02,rcv);sdly();}

   }else{
     if(ival2==0){vme_controller(1,ptr,x01,rcv);sdly();}
     if(ival2==1){vme_controller(1,ptr,x03,rcv);sdly();}
   }
 }
  vme_controller(1,ptr,x01,rcv);sdly();       
  vme_controller(3,ptr,x00,rcv);sdly();       

}

void emu::fed::VMEController::sdly()
{

}


void  emu::fed::VMEController::sleep_vme(const char *outbuf)   // in usecs (min 16 usec)
{

char tmp[1]={0x00};
unsigned short int tmp2[1]={0x0000};
unsigned short int *ptr;
// printf(" outbuf[0-1] %02x %02x \n",outbuf[0]&0xff,outbuf[1]&0xff);
 delay_type=3; 
       tmp2[0]=((outbuf[1]<<8)&0xff00) + (outbuf[0]&0xff);
       tmp2[0]=(unsigned short int)(tmp2[0]/16.0);
       tmp2[0]=tmp2[0]+1;
       //       printf(" tmp2 %d \n",tmp2[0]);
       vme_controller(6,ptr,tmp2,tmp);
}

void  emu::fed::VMEController::sleep_vme2(unsigned short int time) // time in usec
{
float tmp_time;
unsigned short int itime;
char tmp[1]={0x00};
unsigned short int tmp2[1]={0x0000};
unsigned short int *ptr;
 delay_type=3;
       tmp_time=time/16.384;
       itime=(unsigned short int)(tmp_time);
       itime=itime+1;
       tmp2[0]=itime;
       vme_controller(6,ptr,tmp2,tmp);
}

void  emu::fed::VMEController::long_sleep_vme2(float time)   // time in usec
{
float tmp_time;
unsigned long int itime[0];
char tmp[1]={0x00};
unsigned short int *tmp2;
unsigned short int *ptr;
 delay_type=4;
       tmp_time=time/0.004;
       itime[0]=(unsigned long int)(tmp_time+1);
       printf(" time %f tmp_time %f itime %08lx \n",time,tmp_time,itime[0]);
       tmp2=(unsigned short int *)itime;
       vme_controller(6,ptr,tmp2,tmp);
}

void emu::fed::VMEController::handshake_vme()
{
char tmp[1]={0x00};
unsigned short int tmp2[1]={0x0000};
unsigned short int *ptr;
       add_control_r=msk_control_r;   
       ptr=(unsigned short int *)add_control_r;
       vme_controller(4,ptr,tmp2,tmp); // flush
       vme_controller(5,ptr,tmp2,tmp); // handshake
}

void emu::fed::VMEController::flush_vme()
{
char tmp[1]={0x00};
unsigned short int tmp2[1]={0x0000};
unsigned short int *ptr;
// printf(" flush buffers to VME \n");
       vme_controller(4,ptr,tmp2,tmp); // flush
}

void emu::fed::VMEController::vmeser(const char *cmd,const char *snd,char *rcv)
{
 int i;
 int nt;
 short int seri[16]={2,2,0,2,4,6,0,2,0,2,0,0,4,6,0,2};
 unsigned short int icmd;
 unsigned short int iadr;
 
 unsigned long int add_vmesert;
 unsigned short int *ptr;
 
 char tr;
 unsigned short int tmp[1]={0x0000};
 int nrcv;

	add_vmesert=add_vmeser+(cmd[0]&0x000f)*4096+4*(cmd[1]&0x000f);
        icmd=cmd[1]&0x000f;  //DDU command
        iadr=cmd[0]&0x000f;  //DDU device
        if((icmd>8&&iadr==4)||(iadr>=8)){  //write, but ignore snd data
	  ptr=(unsigned short int *)add_vmesert;
          vme_controller(3,ptr,tmp,rcv);
	}
	else if(icmd<9&&iadr==4){      //read
          nt=seri[icmd]/2;
          for(i=0;i<nt-1;i++){
	    ptr=(unsigned short int *)add_vmesert;
            vme_controller(0,ptr,tmp,rcv);
          }
	  ptr=(unsigned short int *)add_vmesert;
	  vme_controller(2,ptr,tmp,rcv);

          nrcv=nt*2;
	  //	  printf(" scan vmeser: return from read, nrcv=%d,",nrcv);
          for(i=0;i<nt;i++){
	    tr=rcv[2*i]; rcv[2*i]=rcv[(2*i)+1]; rcv[2*i+1]=tr;
	    //	    printf(" %02x %02x",rcv[2*i]&0xff,rcv[(2*i)+1]&0xff);
	  }
	  //	  printf("\n");

        }
	else if(iadr<4){      //read
          nt=2;
          for(i=0;i<nt-1;i++){
	    ptr=(unsigned short int *)add_vmesert;
            vme_controller(0,ptr,tmp,rcv);
          }
	  ptr=(unsigned short int *)add_vmesert;
	  vme_controller(2,ptr,tmp,rcv);

          nrcv=nt*2;
	  //	  printf(" scan vmeser: return from read, nrcv=%d,",nrcv);
          for(i=0;i<nt;i++){
	    tr=rcv[2*i]; rcv[2*i]=rcv[(2*i)+1]; rcv[2*i+1]=tr;
	    //	    printf(" %02x %02x",rcv[2*i]&0xff,rcv[(2*i)+1]&0xff);
	  }
	  //	  printf("\n");
        }
     return;
}

void emu::fed::VMEController::vmepara(const char *cmd,const char *snd,char *rcv)
{
int i,nt;

unsigned long int add_vmepart;
unsigned short int idev, icmd;
unsigned short int *ptr;
unsigned short int *data;
unsigned short int tmp[1]={0x0000};

 int nrcv;
   icmd=cmd[1]&0x00ff;  //0-127 read, >=128 write
   idev=cmd[0]&0x000f;  //0-7 CMD ignored, >=8 CMD req'd.
   add_vmepart=add_vmepara+(4096*idev)+(4*icmd);

//JRG, added Write case:
   if(icmd>127&&idev>=8){  //Write
     nt=2;
     nt=nt/2; 
     data=(unsigned short int *)snd;
     for(i=0;i<nt-1;i++){
       ptr=(unsigned short int *)add_vmepart;
       vme_controller(1,ptr,data,rcv);
       data=data+1;
     } 
     ptr=(unsigned short int *)add_vmepart;
     vme_controller(3,ptr,data,rcv);
   }
//JRG, end Write case ^^^^

   else{   //Read
     nt=1;
     ptr=(unsigned short int *)add_vmepart;
     vme_controller(2,ptr,tmp,rcv);
     nrcv=nt*2;
     //     printf(" scan vmepar: return from read, nrcv=%d,",nrcv);
     //     for(i=0;i<nrcv;i++)printf(" %02x",rcv[i]&0xff);
     //     printf("\n");
   }
}

void emu::fed::VMEController::dcc(const char *cmd,char *rcv)
{
unsigned long add;
unsigned short int *ptr;
unsigned short int *data;

unsigned short int tcmd;
char c[2];

// n = readline(sockfd,cmd,4); 
// printf(" dcc: cmd %02x %02x %02x %02x \n",cmd[0]&0xff,cmd[1]&0xff,cmd[2]&0xff,cmd[3]&0xff);
 tcmd=cmd[1]<<2;
 if(cmd[0]==0x00){
   c[0]=cmd[3];
   c[1]=cmd[2]; 
   data=(unsigned short int *)c;

   add=add_dcc_w+tcmd;
   ptr=(unsigned short int *)add;
   vme_controller(3,ptr,data,rcv);
   //  printf(" dcc write: ptr %08x data %04x \n",ptr,*data);
   // printf(" about to write \n");
   // *ptr=*data;
 }
 if(cmd[0]==0x01){
   // data2=rcv;
   add=add_dcc_w+tcmd;
   ptr=(unsigned short int *)add;
   vme_controller(2,ptr,data,rcv);
   // *data2=*ptr; 
   //  printf(" dcc read: ptr %08x data %04x \n",ptr,*data2);
   // n = writen(sockfd,rcv,2);
 }
}

void emu::fed::VMEController::vme_adc(int ichp,int ichn,char *rcv)
{
 
 unsigned short int *ptr;
 unsigned short int val[2];
 unsigned short int tmp[2]={0x0000,0x0000}; 
 // printf(" enter vme_adc \n");
      
      val[0]=adcreg[ichp];
      val[1]=adcbyt[ichp-1][ichn];
      if(val[0]==0){ 
         ptr=(unsigned short int *)add_adcrs;
	 vme_controller(2,ptr,tmp,rcv);     
         return;
       }
      // ptr=(unsigned short int *)add_adcws;
       // printf(" select register%08x  %02x \n",ptr,val[0]&0xff);
      //  vme_controller(3,ptr,&val[0],rcv);     
         ptr=(unsigned short int *)add_adcw;
         // printf(" adc write %08x %02x \n",ptr,val[1]&0xff);
	 vme_controller(3,ptr,&val[1],rcv);    // *ptr=val[1]; 
      if(val[0]!=0x07){
          ptr=(unsigned short int *)add_adcr;
      }else{
          ptr=(unsigned short int *)add_adcrbb;
      }
      vme_controller(2,ptr,tmp,rcv);     

      // print(" adc read %08x %02x %02x\n",ptr,rbuf[1]&0xff,rbuf[0]&0xff);
}
*/