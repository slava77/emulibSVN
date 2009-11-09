#ifndef TStoreTMBParser_h
#define TStoreTMBParser_h

#include <iostream>
#include <fstream>

#include "DAQMB.h"
#include "TMB.h"
#include "CCB.h"
#include "MPC.h"
#include "ALCTController.h"
#include "VMEController.h"
#include "Crate.h"
#include "EmuSystem.h"
//
class TStoreTMBParser {
  //
 public:
  //
  TStoreTMBParser(
		  std::vector <std::string > ,
		  std::vector<std::vector <std::string > >,
		  std::vector<std::vector <std::string > >,
		  Crate * , Chamber *
		  );//
  //
  ~TStoreTMBParser();
  //
  std::vector<std::string> TStoreTMBParser::FindALCT(std::vector <std::string>, std::vector < std::vector <std::string > > );
  //
 private:
  //
  TMB * tmb_;
  //
};

#endif