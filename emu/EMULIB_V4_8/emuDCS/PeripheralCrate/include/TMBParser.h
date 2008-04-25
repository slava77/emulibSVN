//-----------------------------------------------------------------------
// $Id: TMBParser.h,v 3.2 2006/11/15 16:01:36 mey Exp $
// $Log: TMBParser.h,v $
// Revision 3.2  2006/11/15 16:01:36  mey
// Cleaning up code
//
// Revision 3.1  2006/08/17 15:02:31  mey
// Modified Parser to accept globals
//
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.3  2006/07/13 15:46:37  mey
// New Parser strurture
//
// Revision 2.2  2006/07/11 15:00:38  mey
// Update
//
// Revision 2.1  2006/03/30 13:55:38  mey
// Update
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef TMBParser_h
#define TMBParser_h
#include "EmuParser.h"
#include <xercesc/dom/DOM.hpp>

class TMB;
class Chamber;
class Crate;
class TMBParser
{
public:
  TMBParser(){}
  ~TMBParser();
  explicit TMBParser(xercesc::DOMNode * , Crate *, Chamber *, xercesc::DOMNode * );
  //
  /// returns the last tmb parsed
  TMB * tmb() const {return tmb_;}

private:
  TMB * tmb_;
  EmuParser parser_;
  EmuParser parserGlobal_;
};

#endif






