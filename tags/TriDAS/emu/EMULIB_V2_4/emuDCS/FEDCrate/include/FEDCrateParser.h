//-----------------------------------------------------------------------
// $Id: FEDCrateParser.h,v 1.2 2006/01/21 20:20:14 gilmore Exp $
// $Log: FEDCrateParser.h,v $
// Revision 1.2  2006/01/21 20:20:14  gilmore
// *** empty log message ***
//
// Revision 1.16  2004/06/11 09:38:17  tfcvs
// improved checks on top elements. print out warnings in case they do not
// comply. (FG)
//
//-----------------------------------------------------------------------
#ifndef FEDCrateParser_h
#define FEDCrateParser_h
/*
 *  class FEDCrateParser
 *  author Alex Tumanov 6/12/03
 *     
 */


#include "DDUParser.h"
#include "DCCParser.h"
#include "VMEParser.h"


class FEDCrateParser {

public:
  FEDCrateParser() {}

  /** Parse the file
      @param name File Name
  */
  void parseFile(const char* name);

  DDUParser dduParser() const {return dduParser_;}
  DCCParser dccParser()     const {return dccParser_;}
  VMEParser vmeParser()     const {return vmeParser_;}

protected:
  DDUParser dduParser_;
  DCCParser dccParser_;
  VMEParser vmeParser_;
  int crateNumber;
};

#endif






