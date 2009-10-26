//-----------------------------------------------------------------------
// $Id: AnodeChannel.h,v 3.0 2006/07/20 21:15:47 geurts Exp $
// $Log: AnodeChannel.h,v $
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef AnodeChannel_h
#define AnodeChannel_h

class AnodeChannel {
public:
  AnodeChannel(int number, int delay, int threshold);
 
  int number() const {return number_;}
  int delay() const {return delay_;}
  int threshold() const {return threshold_;}

private:
  int number_, delay_, threshold_;
};

#endif
