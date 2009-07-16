#ifdef D360
//----------------------------------------------------------------------
// $Id: VMEModule.cc,v 2.1 2005/11/02 16:16:43 mey Exp $
// $Log: VMEModule.cc,v $
// Revision 2.1  2005/11/02 16:16:43  mey
// Update for new controller
//
// Revision 2.0  2005/04/12 08:07:06  geurts
// *** empty log message ***
//
// Revision 1.23  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//----------------------------------------------------------------------
#include "VMEModule.h"
#include "VMEController.h"
#include "Crate.h"
#include "CrateSetup.h"
#include "Singleton.h"
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h> // read and write

#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl; 
#define PRINTSTRING(x) cout << #x << endl; 
#endif



VMEModule::VMEModule(int newcrate, int newslot): 
  theSlot(newslot), cnt(0)
{
  theCrate = Singleton<CrateSetup>::instance()->crate(newcrate);
  #ifdef debugV
    cout << "creating VMEModule in crate " << theCrate->number() << endl;
  #endif 
  theController = theCrate->vmeController();
  theCrate->addModule(this);
}


void VMEModule::start() {
  PRINTSTRING(OVAL: start() from VMEModule have been called...);
  char sendline[2];
  int n=2;
  char c=0;
  unsigned int lev=0x01;
  c=c|lev;
  // next two bits are board type
  c=c|(boardType()<<2);
#ifdef debugV
  cout << "starting VMEModule for slot " << dec << theSlot << " boardType " << boardType() << " line " << (int) c << endl;
#endif
  sendline[0]=c;
  sendline[1]=theSlot;
  theController->writen(sendline, n);
}


void VMEModule::end() {
  char sendline[2];
  int n=2;
  sendline[0]=0x02;
  writen(sendline, n);
}

 
void VMEModule::endDevice() {
  theController->end();
}


void VMEModule::do_vme(char fcn, char vme, 
                       const char *snd,char *rcv, int when) {
  theController->start(this);
  Tdata[cnt]=fcn;
  Tdata[cnt+1]=vme;
  Tdata[cnt+2]=snd[0];
  Tdata[cnt+3]=snd[1];
  cnt += 4;
  assert(cnt < TDATASIZE);
  ///@@ might be a bug with ORed codes in fcn
  if(fcn!=VME_READ && when==LATER)return;
  theController->writenn(Tdata,cnt);
  cnt=0;
  if(fcn==VME_READ){
    theController->readn(rcv);
  }
}

int VMEModule::readn(char *line) {
  theController->start(this);
  return theController->readn(line);
}


int VMEModule::writen(register const char *ptr, register int nbytes) {
  theController->start(this);
  return theController->writen(ptr, nbytes);
}


int VMEModule::readline(char * line, int maxbytes) {
  theController->start(this);
  return theController->readline(line, maxbytes);
}


void VMEModule::devdo(DEVTYPE dev,int ncmd, const char *cmd,int nbuf,
                     char *inbuf,char *outbuf,int irdsnd) {
  theController->start(this);
  theController->devdo(dev, ncmd, cmd, nbuf, inbuf, outbuf, irdsnd);
}


void VMEModule::scan(int reg,const char *snd,int cnt,char *rcv,int ird) {
  theController->start(this);
  theController->scan(reg, snd, cnt, rcv, ird);
}


void VMEModule::InitJTAG(int port) {
  theController->start(this);
  theController->InitJTAG(port);
}


void VMEModule::CloseJTAG() {
  theController->start(this);
  theController->CloseJTAG();
  theController->end();
}


VMEController* VMEModule::getTheController(){
  return theController;
}
#endif

#ifdef OSUcc

//----------------------------------------------------------------------
// $Id: VMEModule.cc,v 2.1 2005/11/02 16:16:43 mey Exp $
// $Log: VMEModule.cc,v $
// Revision 2.1  2005/11/02 16:16:43  mey
// Update for new controller
//
// Revision 1.23  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//
//----------------------------------------------------------------------
#include "VMEModule.h"
#include "VMEController.h"
#include "Crate.h"
#include "CrateSetup.h"
#include "Singleton.h"
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h> // read and write

extern unsigned long add_ucla;
extern unsigned long add_control_r;
extern unsigned long msk_control_r;
extern int delay_type;

#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl; 
#define PRINTSTRING(x) cout << #x << endl; 
#endif



VMEModule::VMEModule(int newcrate, int newslot): 
  theSlot(newslot)
{
  theCrate = Singleton<CrateSetup>::instance()->crate(newcrate);
  #ifdef debugV
    cout << "creating VMEModule in crate " << theCrate->number() << endl;
  #endif 
  theController = theCrate->vmeController();
  theCrate->addModule(this);
}


void VMEModule::start() {
  PRINTSTRING(OVAL: start() from VMEModule have been called...);
#ifdef debugV
  cout << "starting VMEModule for slot " << dec << theSlot << " boardType " << boardType() << " line " << (int) c << endl;
#endif  
   vmebase=0x00000000|(theSlot<<19);
// Jinghua Liu to debug
   //printf("slot: %d, base address: %08x\n", theSlot, vmebase);
//
   if(boardType()==TMB_ENUM) {
      add_ucla=vmebase|0x70000|(theSlot<<19);
   }
}


void VMEModule::end() {
}

 
void VMEModule::endDevice() {
  theController->end();
}


void VMEModule::do_vme(char fcn, char vme, 
                       const char *snd,char *rcv, int when) {
theController->start(this);
unsigned short int it[1]; 
unsigned short int tmp[1]={0x0000};
unsigned short int *ptr_rice;
unsigned long add_rice;
unsigned short int itwr[2]={1,3};
unsigned short int itrd[2]={2,2};
//printf("in VMEModule::do_vme. fcn=%d, baseadd=%08X\n",fcn,vmebase);
if(fcn==15)return;
    it[0]=snd[0]&0x00ff;
    it[0]=it[0]|((snd[1]<<8)&0xff00);
    add_rice=vmebase|(unsigned char)vme;
    ptr_rice=(unsigned short int *)add_rice;
    if(fcn==2){
      //printf(" rice VME W:%08x %04x \n",ptr_rice,it[0]);
      vme_controller(itwr[when],ptr_rice,it,rcv);
    }
    if(fcn==1){
      //printf(" rice VME R: %08x %04x \n",ptr_rice,*rcv);
      vme_controller(itrd[when],ptr_rice,tmp,rcv);
    }
    if(fcn==4)ccb_handshake_vme(); // handshake
    if(fcn==3)ccb_sleep_vme(snd); //delay 
}



void VMEModule::devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,
                     const char *inbuf,char *outbuf,int irdsnd) {
  //printf("VMEModule::devdo\n");
  theController->start(this);
  theController->devdo(dev, ncmd, cmd, nbuf, inbuf, outbuf, irdsnd);
}

void VMEModule::vme_controller(int irdwr,unsigned short int *ptr,unsigned short int *data,char *rcv){
  //printf("VMEModule::vme_controller\n");
  theController->start(this);
  theController->vme_controller(irdwr,ptr,data,rcv);
}


void VMEModule::scan(int reg,const char *snd,int cnt,char *rcv,int ird) {
  theController->start(this);
  theController->scan(reg, snd, cnt, rcv, ird);
}

void VMEModule::scan_alct(int reg,const char *snd,int cnt,char *rcv,int ird) {
  theController->start(this);
  theController->scan_alct(reg, snd, cnt, rcv, ird);
}


void VMEModule::InitJTAG(int port) {
  theController->start(this);
  theController->InitJTAG(port);
}


void VMEModule::CloseJTAG() {
  theController->start(this);
  theController->CloseJTAG();
  theController->end();
}


VMEController* VMEModule::getTheController(){
  return theController;
}

void  VMEModule::ccb_sleep_vme(const char *outbuf)   // in usecs (min 16 usec)
{
unsigned short int pause;
char tmp[1]={0x00};
unsigned short int tmp2[1]={0x0000};
unsigned short int *ptr;
// printf(" outbuf[0-1] %02x %02x \n",outbuf[0]&0xff,outbuf[1]&0xff);
 delay_type=3;
       tmp2[0]=outbuf[1]*256+outbuf[0];
       tmp2[0]=2*tmp2[0];
       tmp2[0]=tmp2[0]+1;
       vme_controller(6,ptr,tmp2,tmp);
}

void VMEModule::ccb_handshake_vme()
{
char tmp[1]={0x00};
unsigned short int tmp2[1]={0x0000};
unsigned short int *ptr;
       add_control_r=msk_control_r;   
       ptr=(unsigned short int *)add_control_r;
       vme_controller(4,ptr,tmp2,tmp); // flush
       vme_controller(5,ptr,tmp2,tmp); // handshake
}

#endif