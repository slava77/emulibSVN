//-----------------------------------------------------------------------
// $Id: CrateSetup.cc,v 2.0 2005/04/12 08:07:05 geurts Exp $
// $Log: CrateSetup.cc,v $
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "CrateSetup.h"
#include "Crate.h"
#include <iostream>

CrateSetup::CrateSetup() {
}
 

CrateSetup::~CrateSetup() {
  for(unsigned int i = 0; i < theCrates.size(); ++i) {
    delete theCrates[i];
  }
}


Crate * CrateSetup::crate(unsigned int i) {
  return theCrates[i];
}


void CrateSetup::addCrate(unsigned int i, Crate * crate) {
  if(i >= theCrates.size()) theCrates.resize(i+1);
  theCrates[i] = crate;
}
