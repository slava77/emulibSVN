#include "hvserver.h"

void HVDataBaseUpdateTimer::timerHandler()
{
  UpdateDataDB();
  if (fStart) start();
};


void HVDimDataUpdateTimer::timerHandler()
{
  UpdateDataDim();
#ifdef WATCHDOG
  if (watchdog != kNullTimerRef)  Rebound_SetTimer(watchdog, 5);
#endif
  svc_hcFound->updateService();
  if (fStart) start();
};

void HVPrimaryControlUpdateTimer::timerHandler()
{
  if (!fStart)
    {
      return;
    }
  struct timeval t1, t2;
  // gettimeofday(&t1, NULL);
  // t1 = tstamp;
  pthread_mutex_lock(&PrimeIOmutex);
  for (map<int, map<int, HVPrimary*> >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr)
    {
      for (map<int, HVPrimary*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
        {
          HVPrimary* h = h_itr->second;
          // SendHVCmd( h->port, h->addr, "REN" );
          if (GetControlStatus( h->port, h->addr, & (h->control ) ) < 0)
            {
              LOG4CPLUS_WARN(hvmonlog, "Timer: Error reading control status for " << PRIMARY_STR(h));
              ResetHV( h->port, h->addr );
              SetHVMaxVoltage( h->port, h->addr, 0 );
            }

        }
    }

  pthread_mutex_unlock(&PrimeIOmutex);
  // gettimeofday(&t2, NULL);
  // LOG4CPLUS_DEBUG(hvlog, "update primary timer: " << ((t2.tv_sec+t2.tv_usec/1000000.) - (t1.tv_sec+t1.tv_usec/1000000.)) << "sec");
  //tstamp = t2;
  DimTimer::start(delay);
};
