#include<iostream>
#include<stdio.h>
#include<unistd.h> // for sleep()
#include "CrateSelector.h"
#include "DDU.h"
#include "DCC.h"
#include "Crate.h"
#include "CrateSetup.h"
#include "Singleton.h"
#include "VMEController.h"
#include "JTAG_constants.h"
#include <string>



CrateSelector::CrateSelector()
{
}


CrateSelector::~CrateSelector() { 
}


void CrateSelector::setSlot(int slot) {
  theSelectedSlots.clear();
  // -1 means all slots
  cout << " ## CrateSelector::setSlot,  slot=" << slot << endl;
  if(slot != -1) {
    theSelectedSlots.push_back(slot);
  }
}

void CrateSelector::setSlot(string strSlot) {
  theSelectedSlots.clear();
  int slot = -255;
  sscanf(strSlot.c_str(), "%d", &slot);
  cout<< "slot in TBB::setSlot = " << slot << endl;
  // -1 means all slots
  if(slot != -1) {
    theSelectedSlots.push_back(slot);
  }
}


void CrateSelector::setCrate(int crate) {
  theSelectedCrates.clear();
  // -1 means all crates
  cout << " ## CrateSelector::setCrate,  crate=" << crate << endl;
  if(crate != -1) {
    theSelectedCrates.push_back(crate);
    //    cout << "  if the crate is not there then I should abort here...? " << std::endl;
    // ^^^ but how to do that and pass it up the chain to EmuFController?
  }
}

void CrateSelector::setCrate(string strCrate) {
  theSelectedCrates.clear();
  
  int crate = -255;
  sscanf(strCrate.c_str(), "%d", &crate);
  cout<< "Crate in TBB::setCrate = " << crate << endl;
  //-1 means all crates
  if(crate != -1) {
    theSelectedCrates.push_back(crate);
  }
}





vector<Crate *> CrateSelector::crates() const {
  vector<Crate *> allCrates = Singleton<CrateSetup>::instance()->crates();
  vector<Crate *> result;
  int iFoundIt=0;
  //  cout << "  # In CrateSelector::crates, I see " << allCrates.size() << " allCrates" << endl;
  //  cout << "                  and I see " << theSelectedCrates.size() << " theSelectedCrates" << endl;
  if(theSelectedCrates.empty()) {
    //    cout << "    --> there are no crates! " << std::endl;
    result = allCrates;
  } else {
    for(unsigned icrate = 0; icrate < allCrates.size(); ++icrate) {
      for(unsigned iSelect = 0; iSelect < theSelectedCrates.size(); ++iSelect) {
        if(allCrates[icrate]->number() == theSelectedCrates[iSelect]) {
	  cout << "  # In CrateSelector::crates ---> the requested crate was found: " << allCrates[icrate] << ", " << allCrates[icrate]->number() << std::endl;
	  iFoundIt=1;
          result.push_back(allCrates[icrate]);
        }
      }
    }
    if(iFoundIt<1)cout << "  # In CrateSelector::crates ---> the requested crate was not found! " << std::endl;
  }
  return result;
}


vector<DDU *> CrateSelector::ddus() const {
  vector<DDU *> result;
  cout << " ## Enter CrateSelector::ddus " << endl;
  vector<Crate *> myCrates = crates();
  cout << " ## In CrateSelector::ddus, I see " << myCrates.size() << " myCrates" << endl;
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    vector<DDU *> newCards = ddus(myCrates[i]);
    result.insert(result.end(), newCards.begin(), newCards.end());
  }
  //  cout << "    returning newCards... " << std::endl;
  return result;
}

    
vector<DDU *> CrateSelector::ddus(Crate * crate) const {
  vector<DDU *> result;
  vector<DDU*> cards = crate->ddus();
  for(unsigned iddu = 0; iddu < cards.size(); ++iddu) {
    if(theSelectedSlots.empty()) {
      result.push_back(cards[iddu]);
    } else {
      for(unsigned iSelect = 0; iSelect < theSelectedSlots.size(); ++iSelect) {
        if(cards[iddu]->slot() == theSelectedSlots[iSelect]) {
          result.push_back(cards[iddu]);
        }
      }
    }
  }
  return result;
}

vector<DCC *> CrateSelector::dccs() const {
  vector<DCC *> result;
  vector<Crate *> myCrates = crates();
  for(unsigned i = 0; i < myCrates.size(); ++i) {
    vector<DCC *> newCards = dccs(myCrates[i]);
    result.insert(result.end(), newCards.begin(), newCards.end());
  }
  return result;
}

    
vector<DCC *> CrateSelector::dccs(Crate * crate) const {
  vector<DCC *> result;
  vector<DCC*> cards = crate->dccs();
  for(unsigned idcc = 0; idcc < cards.size(); ++idcc) {
    if(theSelectedSlots.empty()) {
      result.push_back(cards[idcc]);
    } else {
      for(unsigned iSelect = 0; iSelect < theSelectedSlots.size(); ++iSelect) {
        if(cards[idcc]->slot() == theSelectedSlots[iSelect]) {
          result.push_back(cards[idcc]);
        }
      }
    }
  }
  return result;
}
