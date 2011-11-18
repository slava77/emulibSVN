#ifndef __AFEB_teststand_LE32_h__
#define __AFEB_teststand_LE32_h__

#include "AFEB/teststand/CrateController.h"
#include "AFEB/teststand/CAMAC.h"

namespace AFEB { namespace teststand {

    class LE32 : public Module
    {
    public:
      enum Channel_t { NoCh=0x0, Ch1=0x1, Ch2=0x2 }; 
      LE32( const string type, const string id );
      void turnOff();
      void writeAmplitude( const Channel_t pulseChannels, const unsigned int amplitude );
      void writeThreshold( const Channel_t thresholdChannels, const unsigned int threshold );
      void enablePulses( const Channel_t pulseChannels );
      void disablePulses();
      void enablePower( const Channel_t supplyChannels );
      void disablePower();
      void exec();
      bool readQ();
      short testLAM();
    };

  }}

#endif
