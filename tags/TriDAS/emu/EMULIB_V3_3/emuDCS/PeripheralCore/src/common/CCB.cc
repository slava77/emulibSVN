//-----------------------------------------------------------------------
// $Id: CCB.cc,v 3.15 2007/08/27 22:50:57 liu Exp $
// $Log: CCB.cc,v $
// Revision 3.15  2007/08/27 22:50:57  liu
// update
//
// Revision 3.14  2007/04/02 17:31:39  liu
// fix setCCBMode problem
//
// Revision 3.13  2007/03/28 16:01:38  rakness
// remove l1areset from TMB-MPC test
//
// Revision 3.12  2007/03/26 14:41:39  rakness
// delay after l1aReset to fix TMB-MPC crate timing test
//
// Revision 3.11  2007/03/26 08:15:25  rakness
// Read TTCrx ID in configure
//
// Revision 3.10  2007/03/16 18:06:22  rakness
// put scans+TTCrxID in summary file
//
// Revision 3.9  2007/03/14 11:59:45  rakness
// extract TTCrxID
//
// Revision 3.8  2006/10/10 11:10:09  mey
// Update
//
// Revision 3.7  2006/09/27 16:44:27  mey
// UPdate
//
// Revision 3.6  2006/09/13 14:13:32  mey
// Update
//
// Revision 3.5  2006/08/10 15:46:30  mey
// UPdate
//
// Revision 3.4  2006/08/07 13:31:29  mey
// Added TTCrx coarse delay
//
// Revision 3.3  2006/08/04 15:49:58  mey
// Update
//
// Revision 3.2  2006/08/03 18:59:13  mey
// Got rid of version number
//
// Revision 3.1  2006/08/03 18:50:49  mey
// Replaced sleep with ::sleep
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.51  2006/07/20 14:03:12  mey
// Update
//
// Revision 2.50  2006/07/20 09:49:55  mey
// UPdate
//
// Revision 2.49  2006/07/19 09:37:59  mey
// Update
//
// Revision 2.48  2006/07/18 15:23:14  mey
// UPdate
//
// Revision 2.47  2006/07/17 02:34:00  liu
// update
//
// Revision 2.46  2006/07/16 04:15:20  liu
// remove CCB2001 support, code cleanup
//
// Revision 2.45  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.44  2006/07/12 07:58:18  mey
// Update
//
// Revision 2.43  2006/07/04 15:06:19  mey
// Fixed JTAG
//
// Revision 2.42  2006/07/04 09:38:14  mey
// Update
//
// Revision 2.41  2006/06/12 12:47:17  mey
// Update
//
// Revision 2.40  2006/06/09 11:38:32  mey
// Update
//
// Revision 2.39  2006/06/02 07:48:12  mey
// Disable TTCrx hardreset
//
// Revision 2.38  2006/05/23 09:01:21  rakness
// Update
//
// Revision 2.37  2006/05/22 16:20:38  mey
// UPdate
//
// Revision 2.36  2006/05/22 14:07:39  rakness
// Update
//
// Revision 2.35  2006/05/19 12:46:48  mey
// Update
//
// Revision 2.34  2006/05/10 10:24:32  mey
// Update
//
// Revision 2.33  2006/04/28 13:41:17  mey
// Update
//
// Revision 2.32  2006/04/27 18:46:04  mey
// UPdate
//
// Revision 2.31  2006/04/24 14:57:21  mey
// Update
//
// Revision 2.30  2006/04/18 08:17:29  mey
// UPdate
//
// Revision 2.29  2006/03/10 10:13:54  mey
// Got rid of DCS
//
// Revision 2.28  2006/02/25 11:25:11  mey
// UPdate
//
// Revision 2.27  2006/02/21 12:44:00  mey
// fixed bug
//
// Revision 2.26  2006/02/15 22:39:57  mey
// UPdate
//
// Revision 2.25  2006/02/13 19:51:12  mey
// Fix bugs
//
// Revision 2.24  2006/02/06 14:06:55  mey
// Fixed stream
//
// Revision 2.23  2006/02/02 15:34:23  mey
// Update
//
// Revision 2.22  2006/02/02 14:27:32  mey
// Update
//
// Revision 2.21  2006/01/12 23:44:21  mey
// Update
//
// Revision 2.20  2006/01/12 22:36:23  mey
// UPdate
//
// Revision 2.19  2006/01/12 12:28:24  mey
// UPdate
//
// Revision 2.18  2006/01/12 11:32:26  mey
// Update
//
// Revision 2.17  2006/01/11 13:47:27  mey
// Update
//
// Revision 2.16  2006/01/10 19:46:48  mey
// UPdate
//
// Revision 2.15  2006/01/09 07:17:30  mey
// Update
//
// Revision 2.14  2005/12/20 14:19:58  mey
// Update
//
// Revision 2.13  2005/12/15 14:22:56  mey
// Update
//
// Revision 2.12  2005/11/25 23:42:26  mey
// Update
//
// Revision 2.11  2005/11/21 18:02:50  mey
// Update
//
// Revision 2.10  2005/11/21 17:51:23  mey
// Update
//
// Revision 2.9  2005/11/15 15:37:13  mey
// Update
//
// Revision 2.8  2005/11/07 16:23:39  mey
// Update
//
// Revision 2.7  2005/11/04 10:26:24  mey
// Update
//
// Revision 2.6  2005/11/03 18:24:53  mey
// I2C routines
//
// Revision 2.5  2005/09/28 16:59:57  mey
// Update
//
// Revision 2.4  2005/08/22 07:55:45  mey
// New TMB MPC injector routines and improved ALCTTiming
//
// Revision 2.3  2005/08/11 08:12:42  mey
// Update
//
// Revision 2.2  2005/07/08 10:29:34  geurts
// introduce debug switch to hide debugging messages
//
// Revision 2.1  2005/06/06 11:10:53  geurts
// default power-up mode DLOG. updated for calibration code.
// direct read/write access to registers
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "CCB.h"
#include "VMEController.h"
#include <iostream>
#include <iomanip>
#include <unistd.h> // for sleep
#include <vector>
#include <string>

//
CCB::CCB(Crate * theCrate ,int slot)
: VMEModule(theCrate, slot), 
  TTC(NO_TTC),
  CLK_INIT_FLAG(0),
  mCCBMode(CCB::DLOG),
  BX_Orbit_(924),
  SPS25ns_(0),
  l1aDelay_(0),
  TTCrxID_(-1),
  TTCrxCoarseDelay_(0),
  TTCrxFineDelay_(0),
  l1enabled_(false),
  mDebug(false)
{
  MyOutput_ = &std::cout ;

  ReadTTCrxID_ = -1;
    
  (*MyOutput_) << "CCB: configured for crate=" << this->crate()
	    << " slot=" << this->slot() << std::endl;
 };

CCB::~CCB() {
  (*MyOutput_) << "CCB: removing CCB from crate=" << this->crate() 
       << ", slot=" << this->slot() << std::endl;
}

void CCB::end() {
}

std::ostream & operator<<(std::ostream & os, CCB & ccb) {
  os << std::dec << "l1enabled_ " << ccb.l1enabled_ << std::endl
     << "TTC " << ccb.TTC << std::endl
     << "CLK_INIT_FLAG " << ccb.CLK_INIT_FLAG << std::endl
     << "BX_Orbit_ " << ccb.BX_Orbit_ << std::endl
     << "SPS25ns_ " << ccb.SPS25ns_ << std::endl
     << "TTCrxID_ " << ccb.TTCrxID_ << std::endl
     << "l1aDelay_ " << ccb.l1aDelay_ << std::endl
     << "mDebug " << ccb.mDebug << std::endl
     << "mCCBMode " << ccb.mCCBMode << std::endl
     << "TTCrxCoarseDelay_ " << ccb.TTCrxCoarseDelay_ << std::endl
     << "TTCrxFineDelay_ " << ccb.TTCrxFineDelay_ << std::endl;
  return os;
    }

void CCB::pulse(int Num_pulse,unsigned int pulse_delay, char vme)
{
  // when in DLOG mode, briefly switch to FPGA mode so we can 
  // have the CCB issue the backplane reset. This is *only* for
  // TestBeam purposes.
  bool switchedMode = false;
  if (mCCBMode == (CCB2004Mode_t)CCB::DLOG){
    setCCBMode(CCB::VMEFPGA);
    switchedMode=true;
    if (mDebug) (*MyOutput_) << "CCB: NOTE -- switching from DLOG to FPGA mode for pulse" << std::endl; 
  }
  for(int j=0;j<Num_pulse;j++){
    (*MyOutput_) << "Pulsing..."<<std::endl;
    theController->sleep_vme(pulse_delay);
    //
    sndbuf[0]=0x00;
    sndbuf[1]=0x00;
    if(j<(Num_pulse-1)){
      do_vme(VME_WRITE,vme,sndbuf,rcvbuf,LATER);
    }
    else{
      do_vme(VME_WRITE,vme,sndbuf,rcvbuf,NOW);
    }
  }

  if (switchedMode){
    setCCBMode(CCB::DLOG);
    if (mDebug) (*MyOutput_) << "CCB: NOTE -- switching back to DLOG" << std::endl;
  };
}


void CCB::pulse(int Num_pulse,unsigned int pulse_delay)
{
  //old pulse(Num_pulse, pulse_delay, 0x48;)
  pulse(Num_pulse, pulse_delay, DMB_CFEB_CAL0);
}


void CCB::inject(int Num_pulse,unsigned int pulse_delay)
{
  //old pulse(Num_pulse, pulse_delay, 0x4a);
  pulse(Num_pulse, pulse_delay, DMB_CFEB_CAL1);
}

void CCB::pedestal(int Num_pulse,unsigned int pulse_delay)
{
  //old pulse(Num_pulse, pulse_delay, 0x48;)
  pulse(Num_pulse, pulse_delay, DMB_CFEB_CAL2);
}

void CCB::HardReset_crate()
{
  /// Performs a Hard-Reset to all modules in the crate
  /// through a dedicated VME register (not FastControl)
  //
  (*MyOutput_) << "CCB: hard reset" << std::endl;
  setCCBMode(CCB::VMEFPGA);
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  do_vme(VME_WRITE,CRATE_HARD_RESET,sndbuf,rcvbuf,NOW);
  ::sleep(2);
  setCCBMode(CCB::DLOG);

}
//
void CCB::SoftReset_crate()
{
  /// Reinitializes the FPGAs on DMB, TMB and MPC boards
  /// through a FastControl soft-reset.
  //
  (*MyOutput_) << "CCB: soft reset" << std::endl;
  setCCBMode(CCB::VMEFPGA);
  int i_ccb=0x1c;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc;
  do_vme(VME_WRITE, CSRB2, sndbuf,rcvbuf,NOW);
  ::sleep(1);
  setCCBMode(CCB::DLOG);
  
}
//
void CCB::EnableL1aCounter()
{
  //
  setCCBMode(CCB::VMEFPGA);
  //
  sndbuf[0]=0x00; 
  sndbuf[1]=0x01;
  //
  do_vme(VME_WRITE,enableL1aCounter,sndbuf,rcvbuf,NOW);
  //
}
//
void CCB::ResetL1aCounter()
{
  //
  setCCBMode(CCB::VMEFPGA);
  //
  sndbuf[0]=0x00; 
  sndbuf[1]=0x01;
  //
  do_vme(VME_WRITE,resetL1aCounter,sndbuf,rcvbuf,NOW);
  //
}
//
int CCB::ReadL1aCounter()
{
  //
  int val=0;
  //
  setCCBMode(CCB::VMEFPGA);
  //
  do_vme(VME_READ,readL1aCounterLSB,sndbuf,rcvbuf,NOW);
  //
  val = ((rcvbuf[0]&0xff)<<8) | (rcvbuf[1]&0xff);
  //
  do_vme(VME_READ,readL1aCounterMSB,sndbuf,rcvbuf,NOW);
  //
  val |= ((rcvbuf[0]&0xff)<<24) | ((rcvbuf[1]&0xff)<<16);
  //
  return val;
  //
}
//
void CCB::GenerateAlctAdbASync(){
  //
  (*MyOutput_) << "CCB: GenerateAlctAdbPulseASync" << std::endl;
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  do_vme(VME_WRITE, 0x84, sndbuf,rcvbuf,NOW);
  //
}
//
void CCB::GenerateAlctAdbSync(){
   //
   (*MyOutput_) << "CCB: GenerateAlctAdbPulseSync" << std::endl;
   //int i_ccb=0x19;
   //sndbuf[0]=0x00;
   //sndbuf[1]=(i_ccb<<2)&0xfc;
   //do_vme(VME_WRITE, CSRB2, sndbuf,rcvbuf,NOW);
   //
   sndbuf[0]=0x00;
   sndbuf[1]=0x00;
   do_vme(VME_WRITE, 0x82, sndbuf,rcvbuf,NOW);
   //
   
   //
}
//
void CCB::DumpAddress(int address) {
  //
  do_vme(VME_READ,address,sndbuf,rcvbuf,NOW);  
  //
  printf("CCB.Dump %x %x \n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  //
}
//
std::bitset<8> CCB::ReadTTCrxReg(const unsigned short registerAdd){
  //
  if (TTCrxID_ == -1) {
    (*MyOutput_) << "ReadTTCrxReg.No TTCrxID" << std::endl;
    return 0;
  }
  //
  std::bitset<7> pointerRegAddress(TTCrxID_*2);
  std::bitset<7> DataRegAddress(TTCrxID_*2+1);
  std::bitset<8> regAddress(registerAdd);
  //
  //(*MyOutput_) << " " << pointerRegAddress << " " << DataRegAddress << " " << regAddress << std::endl ;
  //
  // start I2C
  //
  startI2C();
  //
  do_vme(VME_READ,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  // write address of the pointer  register
  for( int i(6); i>=0; --i) {
    writeI2C(pointerRegAddress[i]);
  }
  //
  writeI2C(0); // write bit 0 (write) = 0
  //
  readI2C();
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x04; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x02; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  for( int i(7); i>=0; --i) {
    writeI2C(regAddress[i]);
  }
  //
  readI2C() ;
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x06; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  stopI2C();
  //
  // start condition
  startI2C();
  //
  //
  // write address of the data  register (pointer + 1)
  for( int i(6); i>=0; --i) {
    writeI2C(DataRegAddress[i]);
  }
  //
  writeI2C(1); // write bit 0 (read) = 1
  //
  readI2C();
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x04; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x02; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  // read data
  int Data;
  std::bitset<8> data;
  for(int i(7); i>=0; --i) {
    Data = readI2C();    
    //(*MyOutput_) << Data ;
    data.set(i,Data&0x1);
  }      
  //(*MyOutput_) << std::endl;
  //
  readI2C() ;
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x04 ; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x02; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //  
  // stop condition
  stopI2C();
  //
  return data;
  //
}
//
void CCB::WriteTTCrxReg(const unsigned short registerAdd,int value){
  //
  if (TTCrxID_ == -1) {
    (*MyOutput_) << "WriteTTCrxReg.No TTCrxID" << std::endl;
    return;
  }
  //
  std::bitset<7> pointerRegAddress(TTCrxID_*2);
  std::bitset<7> DataRegAddress(TTCrxID_*2+1);
  std::bitset<8> regAddress(registerAdd);
  //
  //(*MyOutput_) << "TTCrxAddresses = " << pointerRegAddress << " " << DataRegAddress << " " << regAddress << std::endl ;
  //
  // start I2C
  //
  startI2C();
  //
  do_vme(VME_READ,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  // write address of the pointer  register
  for( int i(6); i>=0; --i) {
    writeI2C(pointerRegAddress[i]);
  }
  //
  writeI2C(0); // write bit 0 (write) = 0
  //
  readI2C();
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x04; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x02; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  for( int i(7); i>=0; --i) {
    writeI2C(regAddress[i]);
  }
  //
  readI2C() ;
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x06; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  stopI2C();
  //
  // start condition
  startI2C();
  //
  //
  // write address of the data  register (pointer + 1)
  for( int i(6); i>=0; --i) {
    writeI2C(DataRegAddress[i]);
  }
  //
  writeI2C(0); // write bit 0 (write) = 1
  //
  readI2C();
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x04; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x02; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  bitset<8> inputreadBackDataBit(value);
  // 
  // Write data
  for(int i(7); i>=0; --i) {
    writeI2C(inputreadBackDataBit[i]);
  }      
  //(*MyOutput_) << std::endl;
  //
  readI2C() ;
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x06 ; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);    
  //
  // stop condition
  stopI2C();
  //
  return;
  //
}
//
void CCB::HardResetTTCrx(){
  //
  // Hard reset TTCrx....
  //
  sndbuf[0]=0x00; 
  sndbuf[1]=0x01;
  //
  do_vme(VME_WRITE,TTCrxReset,sndbuf,rcvbuf,NOW);
  //
  ::sleep(1);
  //
  do_vme(VME_READ,CSRB18,sndbuf,rcvbuf,NOW);
  //
  printf("hardResetTTCrx. %x %x \n",rcvbuf[0],rcvbuf[1]);
  //
  if (rcvbuf[0]==0 and rcvbuf[1]==0 ) std::cout << "Failed TTCrxReset" <<std::endl;
  //
}

void CCB::ReadTTCrxID(){
  //
  // Hardreset TTCrx
  //
  sndbuf[0]=0x00; 
  sndbuf[1]=0x01;
  //
  // Reset TTCrx
  do_vme(VME_WRITE,TTCrxReset,sndbuf,rcvbuf,NOW);
  //
  // Wait some time
  ::sleep(1);
  //
  // Read TTCrx ID number
  do_vme(VME_READ,CSRB18,sndbuf,rcvbuf,NOW);
  //
  //  printf("ReadTTCRxID.%02x%02x \n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  //
  ReadTTCrxID_ = ( (rcvbuf[0]&0xff) << 8 ) | (rcvbuf[1]&0xff);
  TTCrxID_ = ReadTTCrxID_;
  //
  (*MyOutput_) << "TTCrx ID = " << std::dec << TTCrxID_ << std::endl;
  //
}
//
int CCB::readI2C(){
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x04; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0c; 
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //  
  do_vme(VME_READ,CSRA1, sndbuf,rcvbuf,NOW);
  //
  //printf(" %x rcv \n",rcvbuf[1]&0xff);
  //
  return ((rcvbuf[1]>>4)&0x1);
  //
} 
//
void CCB::writeI2C(int data){
  //
  //
  const int i2cBaseData(0x2);
  const int i2cData((data&0x1)<<2);
  const int i2cClkBit(0x1 << 3);
  const int i2cLowInput (i2cBaseData|i2cData);
  const int i2cHighInput(i2cBaseData|i2cData|i2cClkBit);
  //
  sndbuf[0]= 0x00; 
  sndbuf[1]= (i2cBaseData&0xff);
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]= 0x00;
  sndbuf[1]=(i2cLowInput&0xff);
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]= 0x00;
  sndbuf[1]=(i2cHighInput&0xff);
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]= 0x00;
  sndbuf[1]=(i2cLowInput&0xff);
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]= 0x00;
  sndbuf[1]= (i2cBaseData&0xff);
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
}
//
void CCB::start() {
  // send the first signal
  SetupJtag();
  VMEModule::start();
  theController->initDevice(1);
  theController->goToScanLevel();
}
//
void CCB::startI2C(){
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0e ;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0a ;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x02 ;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
}
//
void CCB::stopI2C(){
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x02;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0a ;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
  sndbuf[0]=0x00;
  sndbuf[1]=0x0e;
  do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);  
  //
}
//

void CCB::WriteRegister(int reg, int value){
  //
  sndbuf[0] = (value>>8)&0xff;
  sndbuf[1] = value&0xff;
  //
  do_vme(VME_WRITE,reg,sndbuf,rcvbuf,NOW);
  //
}

int CCB::ReadRegister(int reg){
  //
  do_vme(VME_READ,reg,sndbuf,rcvbuf,NOW);
  //
  int value = ((rcvbuf[0]&0xff)<<8)|(rcvbuf[1]&0xff);
  //
  //printf(" CCB.reg=%x %x %x %x\n", reg, rcvbuf[0]&0xff, rcvbuf[1]&0xff,value&0xffff);
  //
  return value;
  //
}


void CCB::enableL1() {
  (*MyOutput_) << "CCB: Enable L1A." << std::endl;
  do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);

  //sndbuf[0]=(rcvbuf[0]&0xFE);
  sndbuf[0]=(rcvbuf[0]&0xDF);
  //sndbuf[1]=(rcvbuf[1]&0x07)|(CCB_CSR1_SAV&0xF8);
  sndbuf[1]=(rcvbuf[1]&0x07)|(CCB_CSR1_SAV&0xF0);

  if (mDebug) (*MyOutput_) << "CCB: CSRB1=0x" << std::hex 
	    << std::setw(2) << int(sndbuf[0]&0xff) << std::setw(2) 
	    <<int(sndbuf[1]&0xff) << std::dec << std::endl;

  do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
  l1enabled_ = true;

  //enableCLCT();

}


void CCB::disableL1() {
  (*MyOutput_) << "CCB: Disable L1A." << std::endl;
  do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);
  sndbuf[0]=rcvbuf[0];
  CCB_CSR1_SAV=rcvbuf[1];
  sndbuf[1]=(rcvbuf[1]|0xf8); // Disable L1ACC from CCB to custom backplane

  //(*MyOutput_) << "disableL1.CCB: CSRB1=0x" << hex 
  //     << setw(2) << int(sndbuf[0]&0xff) << setw(2) <<int(sndbuf[1]&0xff)
  //     << dec << std::endl;
  do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
  l1enabled_ = false;
}


void CCB::hardReset() {

  // when in DLOG mode, briefly switch to FPGA mode so we can 
  // have the CCB issue the backplane reset. This is *only* for
  // TestBeam purposes and only for the  HardReset_crate function.
  bool switchedMode = false;
  if (mCCBMode == (CCB2004Mode_t)CCB::DLOG){
    setCCBMode(CCB::VMEFPGA);
    switchedMode=true;
    (*MyOutput_) << "CCB: NOTE -- switching from DLOG to FPGA mode for BackPlane HardReset" << std::endl; 
  }
  
  ReadRegister(0x0);

  HardResetTTCrx();

//  setCCBMode(CCB::VMEFPGA);

  ReadRegister(0x0);

  HardReset_crate();

  ReadRegister(0x0);


  //
  //fg note: these 10seconds are not necessary for new/old TMB
  //fg sleep(10);
  //
  // sequence of 2nd hard reset with additional delays (Jianhui)
  ::sleep(2);
    HardReset_crate();
    ::sleep(1); // could go down to ~100msec (Jianhui)
  //  std::cout << ReadRegister(0x0) << std::endl;

  SoftReset_crate();
  //fg note: this 1second is not necessary
  ::sleep(1);
  syncReset();
  //fg note: *keep* this 1second!
  ::sleep(1);

  if (switchedMode){
    setCCBMode(CCB::DLOG);
    (*MyOutput_) << "CCB: NOTE -- switching back to DLOG" << std::endl;
  };

  
}


void CCB::syncReset() {
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;

  int i_ccb=0x03;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc;
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  
} 


void CCB::bx0() {
  /// send ccb_bx0 on the backplane:
  (*MyOutput_) << "CCB: BX-zero" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
/* JRG, old way, write to directly to VME adr 0x36, but this
      DOES NOT drive the ccb_cmd bus (no cmd_strobe):
  do_vme(VME_WRITE,0x36,sndbuf,rcvbuf,NOW);
*/

// JRG, best way, write to CSRB2[7-2] to drive ccb_cmd bus; this way
//      also drives dedicated decode lines on the backplane:
  int i_ccb=0x01;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc;
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  
}


void CCB::bxr() {
  (*MyOutput_) << "CCB: BX-reset" << std::endl;
  usleep(10000);

// set ccb_cmd bits (CSRB2[7:2]), EvCntRes (CSR2[1]) and BcntRes (CSR2[0]):
  sndbuf[0]=0x00;
  sndbuf[1]=0xC8;
// send CSRB2 setting on backplane:
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  usleep(10000);
}


void CCB::reset_bxevt() {
  usleep(10000);

//  Send BCR, ECR and BC0  --GUJH   Mar. 20, 2003
  sndbuf[0]=0x00;
  sndbuf[1]=0x03;
// send CSR2 setting on backplane:
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  usleep(10000);
  
}

bool CCB::SelfTest() {
  //
  return 0;
  //
}

void CCB::init() {
  //
  //
}
void CCB::PrintTTCrxRegs(){
  //
  std::cout << "Register 0 " ;
  std::cout << ReadTTCrxReg(0);
  std::cout << std::endl;
  //
  std::cout << "Register 1 " ;
  std::cout << ReadTTCrxReg(1);
  std::cout << std::endl;
      //
  std::cout << "Register 2 " ;
  std::cout << ReadTTCrxReg(2);
  std::cout << std::endl;
  //
  std::cout << "Register 3 " ;
  std::cout << ReadTTCrxReg(3);
  std::cout << std::endl;
  //
  std::cout << "Register 8 " ;
  std::cout << ReadTTCrxReg(8);
  std::cout << std::endl;
  //
  std::cout << "Register 9 " ;
  std::cout << ReadTTCrxReg(9);
  std::cout << std::endl;
  //
  std::cout << "Register 10 " ;
  std::cout << ReadTTCrxReg(10);
  std::cout << std::endl;
  //
  std::cout << "Register 11 " ;
  std::cout << ReadTTCrxReg(11);
  std::cout << std::endl;
  //
  std::cout << "Register 19 " ;
  std::cout << ReadTTCrxReg(19);
  std::cout << std::endl;
  //
  std::cout << "Register 20 " ;
  std::cout << ReadTTCrxReg(20);
  std::cout << std::endl;
  //
  std::cout << "Register 21 " ;
  std::cout << ReadTTCrxReg(21);
  std::cout << std::endl;
  //
  std::cout << "Register 22 " ;
  std::cout << ReadTTCrxReg(22);
  std::cout << std::endl;
  //
  std::cout << "Register 24 " ;
  std::cout << ReadTTCrxReg(24);
  std::cout << std::endl;
  //
  std::cout << "Register 25 " ;
  std::cout << ReadTTCrxReg(25);
  std::cout << std::endl;
  //
  std::cout << "Register 26 " ;
  std::cout << ReadTTCrxReg(26);
  std::cout << std::endl;
      //
  std::cout << "Register 27 " ;
  std::cout << ReadTTCrxReg(27);
  std::cout << std::endl;
  //
  std::cout << "Register 28 " ;
  std::cout << ReadTTCrxReg(28);
  std::cout << std::endl;
  //
}

void CCB::configure() {
  //
  SendOutput("CCB : configure()","INFO");
  //
  // Set the CCB mode  
  setCCBMode((CCB2004Mode_t)mCCBMode);
  // report firmware version
  firmwareVersion();
  
  //  std::cout << ReadRegister(0x0) << std::endl;
  hardReset();
  //  std::cout << ReadRegister(0x0) << std::endl;
  
  // this line from the old rice_clk_setup(), not sure if it's needed
  SetL1aDelay(l1aDelay_);
  
  //  std::cout << ReadRegister(0x0) << std::endl;
  if(l1enabled_)  
     enableL1();
  else  
     disableL1();
  //  std::cout << ReadRegister(0x0) << std::endl;
  //
  // need to read the TTCrxID before TTCrx registers can be touched
  ReadTTCrxID();
  //
  // Download coarse delay to TTCrx
  //
  //PrintTTCrxRegs();
  //std::cout << "write TTCrxCoarseDelay_ = " << TTCrxCoarseDelay_ << " to register 2" << std::endl;
  //
  int delay = ((TTCrxCoarseDelay_&0xf)<<4) + (TTCrxCoarseDelay_&0xf);
  WriteTTCrxReg(2,delay);
  //
  // Download fine delay to TTCrx
  //
  delay = (TTCrxFineDelay_&0xff);
  WriteTTCrxReg(0,delay);
  WriteTTCrxReg(1,delay);
  //
  //
  //PrintTTCrxRegs();
  setCCBMode(CCB::DLOG);
  //
}

void CCB::SetL1aDelay(int l1adelay){
  //
  do_vme(VME_READ,CSRB5,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=l1adelay;  
  do_vme(VME_WRITE,CSRB5,sndbuf,rcvbuf,NOW);
  //
}

void CCB::setCCBMode(CCB2004Mode_t mode){
  /// Set the mode of operation for the CCB2004 model

  char tmpb1[2];

  do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);
  tmpb1[0]=rcvbuf[0];
  tmpb1[1]=rcvbuf[1];
  
  switch (mode) {
  case TTCrqFPGA:
    sndbuf[0]=0x00;
    sndbuf[1]=0x0E;
    do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);
    if((tmpb1[1]&0x01)==1) {
       sndbuf[0]=tmpb1[0];
       sndbuf[1]=tmpb1[1] & 0xFE;
       do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
    }
    break;
  case VMEFPGA:
    sndbuf[0]=0x00;
    sndbuf[1]=0x0E;
    do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);
    if((tmpb1[1]&0x01)==0) {
       sndbuf[0]=tmpb1[0];
       sndbuf[1]=tmpb1[1] | 0x01;
       do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
    }
    break;
  case DLOG:
    sndbuf[0]=0x00;
    sndbuf[1]=0x0F;
    do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);
    break;
  default:
    std::cerr << "Warning: unkown CCB2004 operation mode. Using DLOG instead" << std::endl;
    sndbuf[0]=0x00;
    sndbuf[1]=0x0F;
    do_vme(VME_WRITE,CSRA1,sndbuf,rcvbuf,NOW);
  }
  mCCBMode=mode;
}

 
void CCB::enable() {
  (*MyOutput_) << "Sync reset, mainly for TMB " << std::endl;
  if(l1enabled_) disableL1();
  syncReset();
  usleep(50000);

  (*MyOutput_) << "BXR and EVTCNTRST" << std::endl;
  reset_bxevt();

  if (SPS25ns_==0) {
    startTrigger();
    usleep(10000);
    (*MyOutput_) << "CCB BC0" << std::endl;

    bx0();
    usleep(10000);
    setCCBMode(CCB::DLOG);
  }

    if (TTC==ALL_TTC)
      enableTTCControl();

  enableL1();
}


void CCB::enableTTCControl() {
  (*MyOutput_) << "CCB: Enable TTC control " << std::endl;

  do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);

  usleep(50000);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(rcvbuf[1]&0xfe);

  do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
  usleep(50000);
  (*MyOutput_) << "CCB: CSRB1(read)=0x" << std::hex 
	    << std::setw(2) << int(rcvbuf[0]&0xff) << std::setw(2) 
	    <<int(rcvbuf[1]&0xff) << "changed to CSRB1(set)=0x"
	    << std::setw(2) << int(sndbuf[0]&0xff) << std::setw(2)
	    <<int(sndbuf[1]&0xff) << std::dec << std::endl;
}


void CCB::startTrigger() {
  //
  if(mCCBMode!=VMEFPGA) setCCBMode(CCB::VMEFPGA);
  //
  /// Send "Start Trigger" command on the Fast Control Bus
  (*MyOutput_) << "CCB: Start Trigger" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=0x18;          //cmd[5:0]=0x06 for start_trigger     GUJH
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  //
}
//
void CCB::injectTMBPattern() {
  //
  if(mCCBMode!=VMEFPGA) setCCBMode(CCB::VMEFPGA);
  //
  /// Inject TMB Pattern data
  (*MyOutput_) << "CCB: Start TMB pattern injection to MPC" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=(0x24<<2);          
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}
//
void CCB::l1aReset(){
  //
  if(mCCBMode!=VMEFPGA) setCCBMode(CCB::VMEFPGA);
  //
  /// Send "L1a Reset" command on the Fast Control Bus
  (*MyOutput_) << "CCB: L1a Reset" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=(0x3<<2);          //cmd[5:0]=0x03 
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  //
}
//
void CCB::CLCTexternalTrigger() {
  //
  sndbuf[0]=0x00;
  sndbuf[1]=(0x1A<<2);          //cmd[5:0]=0x03 
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
  //
}
//
void CCB::bc0() {
  //
  if(mCCBMode!=VMEFPGA) setCCBMode(CCB::VMEFPGA);
  //
  /// Send "bc0" command on the Fast Control Bus
  (*MyOutput_) << "CCB: bc0 Trigger" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=0x4; 
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);

}
//
void CCB::stopTrigger() {
  //
  if(mCCBMode!=VMEFPGA) setCCBMode(CCB::VMEFPGA);
  //
  /// Send "Stop Trigger" command on the Fast Control Bus
  (*MyOutput_) << "CCB: Stop Trigger" << std::endl;
  sndbuf[0]=0x00;
  sndbuf[1]=0x1C;          //cmd[5:0]=0x07 for stop_trigger     GUJH
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}
//
void CCB::disable() {
  (*MyOutput_) << "CCB: disable" << std::endl;
  disableTTCControl();
  if (SPS25ns_==0) {
    //stop trigger
    stopTrigger();
  }
}
//
void CCB::disableTTCControl() {
  (*MyOutput_) << "CCB: disable TTC control" << std::endl;
  do_vme(VME_READ,CSRB1,sndbuf,rcvbuf,NOW);
  usleep(50000);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(rcvbuf[1]|0x01);
  do_vme(VME_WRITE,CSRB1,sndbuf,rcvbuf,NOW);
  usleep(5000);
}
//
void CCB::executeCommand(std::string command) {
  if(command=="Program Backplane") HardReset_crate();
  if(command=="Reset Backplane")   SoftReset_crate();

  if(command=="Enable L1")        enableL1();
  if(command=="Disable L1")       disableL1();

  if(command=="Hard Reset")       hardReset();
  if(command=="Sync Reset")       syncReset();
  if(command=="Reset BXEVT")      reset_bxevt();
  if(command=="BX0")              bx0();
  if(command=="BXR")              bxr();
}
//
void CCB::FireCCBMpcInjector(){
  //
  //
  // Issue L1Reset
  //
  // l1aReset();
  //
  // Issue L1 Start
  //
  //startTrigger();
  //
  // Issue BC0
  //
  //bc0();
  //
  // Inject TMB Pattern
  //
  injectTMBPattern();
  //
  // Stop Trigger
  //
  //stopTrigger();
  //
}
//
void CCB::firmwareVersion(){
  /// report the firmware version
  do_vme(VME_READ,CSRB17,sndbuf,rcvbuf,NOW);  
  int versionWord = (rcvbuf[0]<<8) + (rcvbuf[1]&0xFF);
  //  std::cout << std::hex << "Word=" << versionWord << std::endl;
  int day   =  versionWord & 0x1F;
  int month = (versionWord >> 5   ) & 0xF;
  int year  = (versionWord >>(5+4)) + 2000;
  (*MyOutput_) << "CCB: firmware day-month-year: " 
	       << std::dec << day << "-" << month << "-" << year << std::endl;
}
//
//fg NOTE: CSR1 (2001) vs CSRB1(2004) bit  incompatibilities ...
void CCB::cmd_source_to_ttcrx(){
// 2004
  sndbuf[0]=0x00; // ??????????
  sndbuf[1]=0;  // ??????????
  do_vme(VME_WRITE,0x20,sndbuf,rcvbuf,NOW);
}
//
void CCB::cmd_source_to_vme(){
/// this function should be executed before any *_csr2 function 
// 2004
  sndbuf[0]=0x00; // ??????????
  sndbuf[1]=1;  // ??????????
  do_vme(VME_WRITE,0x20,sndbuf,rcvbuf,NOW);
}

#if 0
////// commented out by Liu 2006-7-15
////// seems no one using these *_csr2 functions
//
void CCB::hard_reset_all_csr2(){
/// reload all fpga-s from eprom (CCB specification)
// Hard reset ALL:
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
// JRG, old way, write to directly to VME adr 0x34, but this
//    DOES NOT drive the ccb_cmd bus (no cmd_strobe):
//rice_vme(0x02,0x34,sndbuf,rcvbuf,NOW);
//
// JRG, best way, write to CSRB2[7-2] to drive ccb_cmd bus; this way
//      also drives dedicated decode lines on the backplane:

// 2004
  int i_ccb=0x04;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/12/2003 ccb_cmd[5..0] (CSRB2[7...2]) ; command code = 4
  
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}
//
void CCB::sync_reset_csr2(){
/// reset L1 readout buffers and resynchronize optical links (CCB specification) ???

// Generate ccb_L1_Reset (sync reset):
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
// 2004
  int i_ccb=0x03;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/13/2003 ccb_cmd[5..0] (CSRB2[7...2]) ; command code = 3
  
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}
//
void CCB::soft_reset_all_csr2(){
/// initialize the FPGA-s on dmb,tmb,mpc (CCB specification)

// Soft reset ALL:
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
// JRG, old way, write to directly to VME adr 0x3c, but this
//    DOES NOT drive the ccb_cmd bus (no cmd_strobe):
//rice_vme(0x02,0x3c,sndbuf,rcvbuf,NOW);
//
//
// JRG, best way, write to CSRB2[7-2] to drive ccb_cmd bus; this way
//      also drives dedicated decode lines on the backplane:
// 2004
  int i_ccb=0x1c;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/13/2003 ccb_cmd[5..0] (CSRB2[7...2]) ; command code = 1c
  
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}
//
void CCB::soft_reset_dmb_csr2(){
/// initialize the FPGA-s on dmb (CCB specification)

// Soft reset DMBs:
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
// JRG, old way, write to directly to VME adr 0x6a, but this
//    DOES NOT drive the ccb_cmd bus (no cmd_strobe):
//rice_vme(0x02,0x6a,sndbuf,rcvbuf,NOW);
//
//
// JRG, best way, write to CSRB2[7-2] to drive ccb_cmd bus; this way
//      also drives dedicated decode lines on the backplane:

// 2004
  int i_ccb=0x1d;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/13/2003 ccb_cmd[5..0] (CSRB2[7...2]) ; command code = 1d
  
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}
//
void CCB::soft_reset_tmb_csr2(){
/// initialize the FPGA-s on tmb (CCB specification)

// Soft reset TMBs:
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
// JRG, old way, write to directly to VME adr 0x7e, but this
//    DOES NOT drive the ccb_cmd bus (no cmd_strobe):
//rice_vme(0x02,0x7e,sndbuf,rcvbuf,NOW);
//
//
// JRG, best way, write to CSRB2[7-2] to drive ccb_cmd bus; this way
//      also drives dedicated decode lines on the backplane:
// 2004
  int i_ccb=0x1e;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/13/2003 ccb_cmd[5..0] (CSRB2[7...2]) ; command code = 1e
  
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}
//
void CCB::soft_reset_mpc_csr2(){
/// initialize the FPGA-s on mpc (CCB specification)

// Soft reset MPCs:
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;

// 2004
  int i_ccb=0x1f;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/13/2003 ccb_cmd[5..0] (CSRB2[7...2]) ; command code = 1f
  
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}
//
void CCB::hard_reset_dmb_csr2(){
/// initialize the FPGA-s on dmb (CCB specification)
// Hard reset DMBs:
// 2004
  int i_ccb=0x12;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/21/2003 ccb_cmd[5..0] (CSRB2[7...2]) ; command code = 12
  
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}
//
void CCB::hard_reset_tmb_csr2(){
/// initialize the FPGA on TMB (CCB specification)
// Hard reset TMBs:
// 2004
  int i_ccb=0x10;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/21/2003 ccb_cmd[5..0] (CSRB2[7...2]) ; command code = 10
  
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}
//
void CCB::hard_reset_alct_csr2(){
/// initialize the FPGA-s on ALCT (CCB specification)
// Hard reset ALCT:
// 2004
  int i_ccb=0x11;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/21/2003 ccb_cmd[5..0] (CSRB2[7...2]) ; command code = 11
  
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}
//
void CCB::hard_reset_ccb_csr2(){
/// initialize the FPGA-s on CCB (CCB specification)
// Hard reset CCB:
// 2004
  int i_ccb=0x0f;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; //
  
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}



void CCB::hard_reset_mpc_csr2(){
/// initialize the FPGA on MPC (CCB specification)
// Hard reset MPC:
// 2004
  int i_ccb=0x13;
  sndbuf[0]=0x00;
  sndbuf[1]=(i_ccb<<2)&0xfc; // correct: 11/21/2003 ccb_cmd[5..0] (CSRB2[7...2]) ; command code = 13
  
  do_vme(VME_WRITE,CSRB2,sndbuf,rcvbuf,NOW);
}

#endif

void CCB::hard_reset_alct(){
/// Hard ALCT reset: programming
/// Generate ALCT "Hard Reset" 400ns pulse
  sndbuf[0]=0x00;
  sndbuf[1]=0x01; 
// Hard ALCT reset:
// 2004 
  do_vme(VME_WRITE,0x66,sndbuf,rcvbuf,NOW); // correct: 11/13/2003 base+0x30  data to write : anything
}



void CCB::hard_reset_dmb(){
/// Hard DMB reset: programming
/// Generate DMB "Hard Reset" 400ns pulse
  sndbuf[0]=0x00;
  sndbuf[1]=0x74; // 0x00;
// Hard DMB reset:
// 2004
  do_vme(VME_WRITE,0x64,sndbuf,rcvbuf,NOW);  // this is the correct DMB Hard Reset // correct: 11/13/2003 base+0x2e  data to write : anything
}



void CCB::hard_reset_tmb(){
/// Hard TMB reset: programming
/// Generate TMB "Hard Reset" 400ns pulse
  sndbuf[0]=0x00;
  sndbuf[1]=0x78; // 0x00;
// Hard TMB reset:
// 2004
  do_vme(VME_WRITE,0x62,sndbuf,rcvbuf,NOW); // correct: 11/13/2003 base+0x2c  data to write : anything
}



void CCB::hard_reset_mpc(){
/// Hard MPC reset: programming
/// Generate MPC "Hard Reset" 400ns pulse
  sndbuf[0]=0x00;
  sndbuf[1]=0x78; // 0x00;
// Hard MPC reset:
// 2004
  do_vme(VME_WRITE,0x68,sndbuf,rcvbuf,NOW); // correct: 11/13/2003 base+0x32  data to write : anything
}



void CCB::hard_reset_all(){
/// Generate "Hard Reset" 400ns pulse to all modules in crate
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  // 2004
  do_vme(VME_WRITE,0x60,sndbuf,rcvbuf,NOW);
}



void CCB::soft_reset_dmb(){
/// Generate "Soft Reset" 25 ns pulse to DMB
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  //2004
  do_vme(VME_WRITE,0x6e,sndbuf,rcvbuf,NOW);  //  base+0x6a
}



void CCB::soft_reset_tmb(){
/// Generate "Soft Reset" 25 ns pulse to TMB
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  //2004
  do_vme(VME_WRITE,0x6c,sndbuf,rcvbuf,NOW); // base+0x7e
}



void CCB::soft_reset_mpc(){
/// Generate "Soft Reset" 25 ns pulse to MPC
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  //2004
  do_vme(VME_WRITE,0x70,sndbuf,rcvbuf,NOW); // base+0x64
}



void CCB::soft_reset_all(){
/// Generate "Soft Reset" 25 ns pulse to all modules in crate!!!
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  //2004
  do_vme(VME_WRITE,0x6a,sndbuf,rcvbuf,NOW); // base+0x3c
}


void CCB::l1a_and_trig(){
/// Generate "L1ACC" 25 ns pulse and external trigger(if enabled)
  sndbuf[0]=0x00;
  sndbuf[1]=0x01;
  //2004
  do_vme(VME_WRITE,0x54,sndbuf,rcvbuf,NOW); // base+0x3c
}