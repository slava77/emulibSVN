//-----------------------------------------------------------------------
// $Id: CrateSelector.cc,v 3.1 2006/07/21 07:48:27 mey Exp $
// $Log: CrateSelector.cc,v $
// Revision 3.1  2006/07/21 07:48:27  mey
// Cleaned up code.Got rid of include Singleton.h
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.2  2006/07/14 12:33:26  mey
// New XML structure
//
// Revision 2.1  2006/07/14 08:11:58  mey
// Got rid of Singleton
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include<iostream>
#include<stdio.h>
#include<unistd.h> // for sleep()
#include "CrateSelector.h"
#include "CCB.h"
#include "DAQMB.h"
#include "TMB.h"
#include "Crate.h"
#include "CrateSetup.h"
//#include "Singleton.h"
#include "VMEController.h"
#include "JTAG_constants.h"
#include <string>

CrateSelector::CrateSelector() : emuSystem_(0)
{
}


CrateSelector::~CrateSelector() { 
}


void CrateSelector::setSlot(int slot) {
  theSelectedSlots.clear();
  // -1 means all slots
  if(slot != -1) {
    theSelectedSlots.push_back(slot);
  }
}

void CrateSelector::setSlot(std::string strSlot) {
  theSelectedSlots.clear();
  int slot = -255;
  sscanf(strSlot.c_str(), "%d", &slot);
  std::cout << "slot in TBB::setSlot = " << slot << std::endl;
  // -1 means all slots
  if(slot != -1) {
    theSelectedSlots.push_back(slot);
  }
}


void CrateSelector::setCrate(int crate) {
  theSelectedCrates.clear();
  // -1 means all crates
  if(crate != -1) {
    theSelectedCrates.push_back(crate);
  }
}

void CrateSelector::setCrate(std::string strCrate) {
  theSelectedCrates.clear();
  
  int crate = -255;
  sscanf(strCrate.c_str(), "%d", &crate);
  std::cout << "Crate in TBB::setCrate = " << crate << std::endl;
  //-1 means all crates
  if(crate != -1) {
    theSelectedCrates.push_back(crate);
  }
}





std::vector<Crate *> CrateSelector::crates() const {
  //std::vector<Crate *> allCrates = Singleton<CrateSetup>::instance()->crates();
  if (!emuSystem_) std::cout << "EmuSystem not defined" << std::endl;
  std::vector<Crate *> allCrates = emuSystem_->crates();
  std::cout << "Crates size = " << allCrates.size() << std::endl;
  std::vector<Crate *> result;
  if(theSelectedCrates.empty()) {
    result = allCrates;
  } else {
    for(unsigned icrate = 0; icrate < allCrates.size(); ++icrate) {
      for(unsigned iSelect = 0; iSelect < theSelectedCrates.size(); ++iSelect) {
        if(allCrates[icrate]->number() == theSelectedCrates[iSelect]) {
          result.push_back(allCrates[icrate]);
        }
      }
    }
  }
  return result;
}


std::vector<DAQMB *> CrateSelector::daqmbs() const {
  std::vector<DAQMB *> result;
  std::vector<Crate *> myCrates = crates();
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    std::vector<DAQMB *> newCards = daqmbs(myCrates[i]);
    result.insert(result.end(), newCards.begin(), newCards.end());
  }
  return result;
}

    
std::vector<DAQMB *> CrateSelector::daqmbs(Crate * crate) const {
  std::vector<DAQMB *> result;
  std::vector<DAQMB*> cards = crate->daqmbs();
  for(unsigned idmb = 0; idmb < cards.size(); ++idmb) {
    if(theSelectedSlots.empty()) {
      result.push_back(cards[idmb]);
    } else {
      for(unsigned iSelect = 0; iSelect < theSelectedSlots.size(); ++iSelect) {
        if(cards[idmb]->slot() == theSelectedSlots[iSelect]) {
          result.push_back(cards[idmb]);
        }
      }
    }
  }
  return result;
}


std::vector<TMB *> CrateSelector::tmbs() const {
  std::vector<TMB *> result;
  std::vector<Crate *> myCrates = crates();
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    std::vector<TMB *> newCards = tmbs(myCrates[i]);
    result.insert(result.end(), newCards.begin(), newCards.end());
  }
  return result;
}


std::vector<TMB *> CrateSelector::tmbs(Crate * crate) const {
  std::vector<TMB *> result;
  std::vector<TMB*> cards ;
  if (crate) cards = crate->tmbs();
  for(unsigned itmb = 0; itmb < cards.size(); ++itmb) {
    if(theSelectedSlots.empty()) {
      result.push_back(cards[itmb]);
    } else {
      for(unsigned iSelect = 0; iSelect < theSelectedSlots.size(); ++iSelect) {
        if(cards[itmb]->slot() == theSelectedSlots[iSelect]) {
          result.push_back(cards[itmb]);
        }
      }
    }
  }
  return result;
}