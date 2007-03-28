//-----------------------------------------------------------------------
// $Id: DAQMBParser.cc,v 3.7 2007/03/19 13:36:56 geurts Exp $
// $Log: DAQMBParser.cc,v $
// Revision 3.7  2007/03/19 13:36:56  geurts
// remove CFEB-specific parsing options. the parser will only deal with settings that apply to all CFEBs at once
//
// Revision 3.6  2007/03/14 20:43:03  gujh
// Divide the Cable_delay into four parts
//      ---Mar. 14, 2007    GU
//
// Revision 3.5  2006/11/28 14:17:16  mey
// UPdate
//
// Revision 3.4  2006/11/15 16:01:36  mey
// Cleaning up code
//
// Revision 3.3  2006/10/10 11:10:09  mey
// Update
//
// Revision 3.2  2006/09/19 08:16:52  mey
// UPdate
//
// Revision 3.1  2006/09/12 15:50:01  mey
// New software changes to DMB abd CFEB
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
//-----------------------------------------------------------------------
#include "DAQMBParser.h"
#include "DAQMB.h"
#include "CFEB.h"
#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>

#define debug false

DAQMBParser::DAQMBParser(xercesc::DOMNode * pNode, Crate * theCrate, Chamber * theChamber)
{
  //
  EmuParser parser_;
  //
  parser_.parseNode(pNode);
  //
  int slot = 0;
  parser_.fillInt("slot", slot);
  if(slot == 0) {
    std::cerr << "No slot specified for DMB! " << std::endl;
  } else {
    //
    daqmb_ = new DAQMB(theCrate,theChamber,slot);  
    //
    int delay;
    if ( parser_.fillInt("feb_dav_delay", delay)){
      daqmb_->SetFebDavDelay(delay);
    }
    if ( parser_.fillInt("tmb_dav_delay", delay) ){
      daqmb_->SetTmbDavDelay(delay);
    }
    if (parser_.fillInt("push_dav_delay", delay)){
      daqmb_->SetPushDavDelay(delay);
    }
    if(parser_.fillInt("l1acc_dav_delay", delay)){
      daqmb_->SetL1aDavDelay(delay);
    }
    if(parser_.fillInt("ALCT_dav_delay", delay)){
      daqmb_->SetAlctDavDelay(delay);
    }
    if(parser_.fillInt("calibration_LCT_delay", delay)){
      daqmb_->SetCalibrationLctDelay(delay);
    }
    if(parser_.fillInt("calibration_l1acc_delay", delay)){
      daqmb_->SetCalibrationL1aDelay(delay);
    }
    if(parser_.fillInt("pulse_delay", delay)){
      daqmb_->SetPulseDelay(delay);
    }
    if(parser_.fillInt("inject_delay", delay)){
      daqmb_->SetInjectDelay(delay);
    }
    float value;
    if(parser_.fillFloat("pul_dac_set", value)){
      daqmb_->SetPulseDac(value);
    }
    if(parser_.fillFloat("inj_dac_set", value)){
      daqmb_->SetInjectorDac(value);
    }
    if(parser_.fillFloat("set_comp_thresh", value)){
      daqmb_->SetCompThresh(value);
      for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetCompThresholdsCfeb(cfeb,value);
    }
    int mode;
    if(parser_.fillInt("comp_mode", mode)){
      daqmb_->SetCompMode(mode);
      for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetCompModeCfeb(cfeb,mode);
    }
    if(parser_.fillInt("comp_timing", delay)){
      daqmb_->SetCompTiming(delay);
      for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetCompTimingCfeb(cfeb,delay);
    }
    if(parser_.fillInt("pre_block_end", delay)){
      daqmb_->SetPreBlockEnd(delay);
      for(int cfeb=0; cfeb<5; cfeb++) daqmb_->SetPreBlockEndCfeb(cfeb,delay);
    }
    if (parser_.fillInt("cfeb_cable_delay", delay)) {
      daqmb_->SetCfebCableDelay(delay); }
    if (parser_.fillInt("tmb_lct_cable_delay", delay)) {
      daqmb_->SetTmbLctCableDelay(delay); }
    if (parser_.fillInt("cfeb_dav_cable_delay", delay)) {
      daqmb_->SetCfebDavCableDelay(delay); }
    if (parser_.fillInt("alct_dav_cable_delay", delay)) {
      daqmb_->SetAlctDavCableDelay(delay); }

    int id;
    if(parser_.fillInt("crate_id",id)){
      daqmb_->SetCrateId(id);
    }
    if ( parser_.fillInt("feb_clock_delay",delay)){
      daqmb_->SetCfebClkDelay(delay);
    }
    if ( parser_.fillInt("xLatency",delay)){
      daqmb_->SetxLatency(delay);
    }
    //
    int number=0;   
    int kill_chip[6]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};
    
    xercesc::DOMNode * daughterNode = pNode->getFirstChild();

    while (daughterNode) {
      if (daughterNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
	if (strcmp("CFEB",xercesc::XMLString::transcode(daughterNode->getNodeName()))==0){
	  parser_.parseNode(daughterNode);
	  if(parser_.fillInt("Number", number)){
	    //daqmb_->SendOutput("CFEB");
	    if ( number <5 ){
	      CFEB cfeb(number);
//fg explicitly prevend the parser from interpreting the following 5 options ...
//	      int ivalue;
//	      if ( parser_.fillInt("comp_mode",ivalue)){
//		daqmb_->SetCompModeCfeb(number,ivalue);
//	      }
//	      if ( parser_.fillInt("comp_timing",ivalue)){
//		daqmb_->SetCompTimingCfeb(number,ivalue);
//	      }
//	      int fvalue;
//	      if ( parser_.fillInt("comp_threshold",fvalue)){
//		daqmb_->SetCompThresholdsCfeb(number,fvalue);
//	      }
//	      if ( parser_.fillInt("pre_block_end",ivalue)){
//		daqmb_->SetPreBlockEndCfeb(number,ivalue);
//	      }
//	      if ( parser_.fillInt("L1A_extra",ivalue)){
//		daqmb_->SetL1aExtraCfeb(number,ivalue);
//	      }
//fg
	      parser_.fillIntX("kill_chip0",kill_chip[0]);
	      parser_.fillIntX("kill_chip1",kill_chip[1]);
	      parser_.fillIntX("kill_chip2",kill_chip[2]);
	      parser_.fillIntX("kill_chip3",kill_chip[3]);
	      parser_.fillIntX("kill_chip4",kill_chip[4]);
	      parser_.fillIntX("kill_chip5",kill_chip[5]);
	      //
	      for(int chip=0;chip<6;chip++){
		for(int chan=0;chan<16;chan++){
		  unsigned short int mask=(1<<chan);
		  if((mask&kill_chip[chip])==0x0000){
		    //daqmb_->shift_array_[number][5-chip][chan]=NORM_RUN;
		    daqmb_->SetShiftArray(number,5-chip,chan,NORM_RUN);
		  }else{
		    //daqmb_->shift_array_[number][5-chip][chan]=KILL_CHAN;
		    daqmb_->SetShiftArray(number,5-chip,chan,KILL_CHAN);
		  }
		}
	      } 	      
	      //
	      daqmb_->cfebs_.push_back(cfeb);
	    } else {
	      std::cout << "ERROR: CFEB range 0-4" <<std::endl;
	    }
	  }
	}
      }
      daughterNode = daughterNode->getNextSibling();
    }
  }
}
//
























