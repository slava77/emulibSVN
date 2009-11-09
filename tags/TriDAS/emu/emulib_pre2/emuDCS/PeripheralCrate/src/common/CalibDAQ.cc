//-----------------------------------------------------------------------
// $Id: CalibDAQ.cc,v 2.0 2005/06/06 10:01:03 geurts Exp $
// $Log: CalibDAQ.cc,v $
// Revision 2.0  2005/06/06 10:01:03  geurts
// calibration routines by Alex Tumanov and Jason Gilmore
//
//
//-----------------------------------------------------------------------
#include <iostream>
#include <unistd.h> // for sleep
#include <string>
#include "CalibDAQ.h"
#include "Crate.h"
#include "DAQMB.h"
#include "CCB.h"
#include "JTAG_constants.h"


void CalibDAQ::loadConstants() {

  std::vector<Crate*> myCrates = theSelector.crates();

  for(unsigned i = 0; i < myCrates.size(); ++i) {
    loadConstants(myCrates[i]);
  }

}


void CalibDAQ::loadConstants(Crate * crate) {
  CCB * ccb = crate->ccb();
  ccb->configure();
  ::sleep(1);

  std::vector<DAQMB*> myDmbs = theSelector.daqmbs(crate);
  for(unsigned i =0; i < myDmbs.size(); ++i) {
    myDmbs[i]->restoreCFEBIdle();
    myDmbs[i]->restoreMotherboardIdle();
    myDmbs[i]->configure();
  }

}

void CalibDAQ::rateTest() {


  int chip,ch,brd, nstrip;
  int counter = 0;
  float dac;

  std::vector<Crate*> myCrates = theSelector.crates();

  for(unsigned j = 0; j < myCrates.size(); ++j) {
    CCB * ccb = myCrates[j]->ccb();
    std::vector<DAQMB*> myDmbs = theSelector.daqmbs(myCrates[j]);
  
    for (int j=0; j<10; j++) {
      dac=0.2+0.2*j;
      for(unsigned i =0; i < myDmbs.size(); ++i) {
	myDmbs[i]->set_cal_dac(dac,dac);
      }
      for (nstrip=0;nstrip<1;nstrip++) {
	for(unsigned i =0; i < myDmbs.size(); ++i) {    
	  for(brd=0;brd<5;brd++){
	    for(chip=0;chip<6;chip++){
	      for(ch=0;ch<16;ch++){
		myDmbs[i]->shift_array[brd][chip][ch]=NORM_RUN;
	      }
	      myDmbs[i]->shift_array[brd][chip][nstrip]=EXT_CAP;
	    }
	  }
	  myDmbs[i]->buck_shift();
	}
  
	for (int tries=0;tries<10000; tries++){
          counter++;
	  std::cout << "dac = " << dac <<
	    "  strip = " << nstrip << 
	    "  try = " << tries << 
	    "  event  = " << counter << std::endl;
	  ccb->pulse(1, 0xff);//pulse all dmbs in this crate
	  ::usleep(1000);
	}

      } //end of loop by strips
    } //endof loop by amplitudes
  }//end of loop by crates
}



void CalibDAQ::pulseAllDMBs(int ntim, int nstrip, float dac, int nsleep) { 
//injects identical pulse to all dmbs (EXT capacitors)
//in all crates one crate at a time          

  int chip,ch,brd;

  std::vector<Crate*> myCrates = theSelector.crates();
  
  for(unsigned j = 0; j < myCrates.size(); ++j) {

    CCB * ccb = myCrates[j]->ccb();
    std::vector<DAQMB*> myDmbs = theSelector.daqmbs(myCrates[j]);
    for(unsigned i =0; i < myDmbs.size(); ++i) {
      
      // set amplitude
      myDmbs[i]->set_cal_dac(dac,dac);

      // set external pulser for strip # nstrip on all 6 chips
      for(brd=0;brd<5;brd++){
	for(chip=0;chip<6;chip++){
	  for(ch=0;ch<16;ch++){
	    myDmbs[i]->shift_array[brd][chip][ch]=NORM_RUN;
	  }
	  myDmbs[i]->shift_array[brd][chip][nstrip]=EXT_CAP;
	}
      }

      myDmbs[i]->buck_shift();
      
      //set timing
      //ntim is the same as pulse_delay initially set in xml configuration file
      myDmbs[i]->set_cal_tim_pulse(ntim);   

    }
  
    //std::cout << "pulsing one time" << std::endl;
    ::usleep(nsleep);
    ccb->pulse(1, 0xff);//pulse all dmbs in this crate
    ::usleep(nsleep);

  }
}
  
void CalibDAQ::injectAllDMBs(int ntim) { //injects identical pulse to all dmbs

  std::vector<Crate*> myCrates = theSelector.crates();

  for(unsigned j = 0; j < myCrates.size(); ++j) {

    CCB * ccb = myCrates[j]->ccb();
    std::vector<DAQMB*> myDmbs = theSelector.daqmbs(myCrates[j]);
    for(unsigned i =0; i < myDmbs.size(); ++i) {

      // set dac
      myDmbs[i]->set_cal_dac(1.0,1.0);

      // shift patterns for half strip 16 on all planes in all cfebs
      for(int k=0;k<6;k++) {
        for (int l=0;l<5;l++){
          myDmbs[i]->halfset(l,k,16);
        }
      }

      myDmbs[i]->buck_shift();

      // set timing
      // ntim is the same as inject_delay and it overrides the initial setting in the xml file
      myDmbs[i]->set_cal_tim_inject(ntim);  // 0-22 are valid times
    }

    ::usleep(1000);
    std::cout << "injecting one time" << std::endl;
    ccb->inject(1, 0xff);//inject all dmbs in this crate
  }
}



