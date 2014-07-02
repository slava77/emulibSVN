#include "hvserver.h"

// == Check for current trip condition in Distribution boards and turn off Primary PS
// == if trip condition stays after turned off connected Master board

// addition by Madorsky
// list of channels to ignore
// each value corresponds to the board with the same number as index in this array
// value itself is a binary mask of known dead channels
long long unsigned db_dead_ch[512];
// end of addition by Madorsky

void CheckForTrips()
{

  // slap-on addition by Madorsky
  // has to be done properly somehow
  // purpose: to prevent known DEAD channels on certain Distr boards from
  // tripping the primary

  static bool first_time = true;
  if (first_time) // do this only very first time this routine is called
    {
      // zero all dead channels
      memset (db_dead_ch, 0, sizeof (db_dead_ch));
      // read list of ignored boards and channels from "/home/hvuser/data/db_dead_channels.txt"
      // format of each line: NNN xxxxxxxxx
      // NNN is the board number, decimal
      // xxxxxxxxx is the hex mask of ignored channels, channel 1 is LSB
      ifstream db_dead_ifs(string(HV_DATADIR+"/db_dead_channels.txt").c_str());

      if (db_dead_ifs.is_open())
        {
          string buf;
          int dbnum;
          long long unsigned chmask;

          while (!db_dead_ifs.eof())
            {
              getline(db_dead_ifs, buf);
              if (buf[0] == '#') continue; // skip comment lines
              istringstream bufstr(buf);
              bufstr >> dec >> dbnum >> hex >> chmask;
              // validate a bit
              if (dbnum < 2 || dbnum > 511)
                {
                  cout << "error in db_dead_channels.txt, line: " << buf << endl;
                }
              else
                {
                  // store
                  db_dead_ch[dbnum] = chmask;
                  cout << "found known dead channel in DB #" << dec << dbnum << " mask " << hex << chmask << endl;
                }
            }

          db_dead_ifs.close();
        }

      first_time = false;
    }

  // end of addition by Madorsky

  for (int i=0; i<dbs.size(); i++)
    {
      int tripcnt=0;
      HVcmd rcmd = dbs[i];
      HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);

      // addition by Madorsky
      // read known dead channels for this board
      unsigned long long deadch = db_dead_ch[r->ID & 511];
      // end of addition by Madorsky

      if (r->master_hvcard>=0 && r->master_busaddr>=0)
        {
          // flag yes_master added by Madorsky to avoid Segmentation faults when masters are not used.
          bool yes_master = (r->master_hvcard != 0xffffffff) && (r->master_busaddr != 0xffffffff);

          HVmodule* m = NULL;
          if (yes_master) m = &(dataservices[r->master_hvcard].data->module[r->master_busaddr]);

          for (int ch=0; ch< r->num_chans; ch++)
            {
              // addition by Madorsky
              // check if this channel is not dead
              if (((deadch >> ch) & 1LL) == 0)
                {
                  // flag m_vcur_0 added by Madorsky to avoid Segmentation faults when masters are not used.
                  bool m_vcur_0;
                  if (yes_master) m_vcur_0 = m->chan[r->master_chan].vcur == 0;
                  else m_vcur_0 = true;
                  // Check for Channel Current Trip
                  if ((r->chan[ch].imon <= r->chan[ch].imax)
                      && m_vcur_0)
                    {
                      tripcnt++;
                    }

                  // Check for Channel Overvoltage Trip
                  if ( (r->chan[ch].vset > OV_MIN_VSET)
                       && (r->chan[ch].vmon > (r->chan[ch].vset_adc + r->chan[ch].vov))
                       && m_vcur_0)
                    {
                      tripcnt++;
                    }
                }
            }

          if (tripcnt)
            {
              (dbs[i].size)++;
            }
          else
            {
              dbs[i].size = 0;
            }

          if (dbs[i].size >= PRIME_TRIP_DELAY)
            {
              HVPrimary* p = (primaries[rcmd.data&0xFFFF])[rcmd.data>>16];
              if (p != NULL) // check by Madorsky, workaround for Seg. faults
                {
                  if (p->vcur > 0 && p->vset > 0)
                    LOG4CPLUS_WARN (hvmonlog,
                                    "Primary PS " << (int)p->port << ":" << (int)p->addr
                                    << " output set to 0 due trips on "
                                    << modtype_str[r->type]
                                    << " ID#" << r->ID
                                    << " " << rcmd.card << ":" << rcmd.module);
                  p->vcur = 0;
                  p->vset = 0;
                }
            }
        }
    }
}
