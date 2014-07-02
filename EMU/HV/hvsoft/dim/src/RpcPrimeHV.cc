#include <iostream>
#include "hvserver.h"
#include "hvprimary.h"
#include "hvpvss.h"

using namespace std;

bool InitPrimaryMonData()
{
  if (!csc_map.size())
    {
      LOG4CPLUS_WARN (hvlog, "Empty Primaries PS list");
      return false;
    }

  string primary_mon_size = "I:1;I:2;I:4;I:1;I:1;I:2;I:5";
  string stats_size = "L:1";

  for ( map < int, map < int, HVPrimary * > >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr )
    {
      for ( map < int, HVPrimary * >::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr )
        {
          pthread_mutex_lock(&PrimeIOmutex);

          HVPrimary * h = h_itr->second;
          pthread_mutex_unlock(&PrimeIOmutex);

          if (h != NULL)
            {
              LOG4CPLUS_DEBUG(hvlog, "Initialize DIM services for " << PRIMARY_STR(h));

              stringstream st;
              st << "HV_PR_" << h->hostid << "_" << h->port << "_" << h->addr;
              string sname = st.str();
              // cout << sname << endl;
              HVprimaryMon& primaryMon = primaryMonData[sname];
              memset(&primaryMon,0, sizeof(primaryMon));
              // Publish Fast Monitoring Dim service for CSC
              primaryMon.primary_mon_dim       = new DimService(sname.c_str(), (char *)primary_mon_size.c_str(),
                  &(primaryMon.primary_mon), sizeof(primaryMon.primary_mon));
              // Publish Monitoring Update Statistics Dim service for CSC
              primaryMon.stats_dim          = new DimService((sname + "/STATS").c_str(), (char *)stats_size.c_str(),
                  &(primaryMon.stats), sizeof(primaryMon.stats));

	      primaryMon.update_interval = DEF_UPDATE_INTERVAL;
              primaryMon.vmon_threshold  = 30;
	      primaryMon.imon_threshold  = 10;
              UpdatePrimaryMonData(sname, primaryMon);
              primaryMon.primary_mon_dim->updateService();
              primaryMon.stats_dim->updateService();
            }

        }
    }

  return true;
}

bool UpdatePrimaryMonData(std::string primaryID, HVprimaryMon& pm)
{
  for ( map < int, map < int, HVPrimary * > >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr )
    {
      for ( map < int, HVPrimary * >::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr )
        {
          pthread_mutex_lock(&PrimeIOmutex);
          HVPrimary * h = h_itr->second;

          if (h != NULL)
            {
              HVPrimary data;
              memset( ( char * ) & data, 0, sizeof( data ) );
              memcpy( ( char * ) & data, h, sizeof( struct HVPrimary ) );

              stringstream st;
              st << "HV_PR_" << h->hostid << "_" << h->port << "_" << h->addr;
              string sname = st.str();
              if (sname == primaryID)
                {
                  // LOG4CPLUS_DEBUG(hvlog, "Updating data for " << PRIMARY_STR(h));

                  pm.primary_mon.hostid = data.hostid;
                  pm.primary_mon.port = (int)(data.port&0xFF);
                  pm.primary_mon.addr = (int)(data.addr&0xFF);
                  pm.primary_mon.polarity = data.polarity;
                  pm.primary_mon.outset = data.outset;
                  pm.primary_mon.output = data.output;
                  pm.primary_mon.control = data.control;
                  pm.primary_mon.imax = (int)data.imax;
                  pm.primary_mon.vset = data.vset;
                  pm.primary_mon.imon = (int)data.imon;
                  pm.primary_mon.vmon = (int)data.vmon;
                  pm.primary_mon.vcur = data.vcur;
                  pm.primary_mon.rampup = data.rampup;
                  pm.primary_mon.rampdown = data.rampdown;
                  pm.primary_mon.status=data.status;
		  pthread_mutex_unlock(&PrimeIOmutex);
		  return true;
                }
            }
          pthread_mutex_unlock(&PrimeIOmutex);

        }
    }
  return false;
}

bool isPrimaryMonUpdateNeeded(HVprimaryMon& newData, HVprimaryMon& curData)
{
  bool fUpdate = false;

  HVPrimaryMonData& new_mon = newData.primary_mon;
  HVPrimaryMonData& cur_mon = curData.primary_mon;

  // Check for data changes in Primary PS parameters
  if ( cur_mon.status != new_mon.status )
    {
      return true;
    }

  if ( cur_mon.outset != new_mon.outset )
    {
      return true;
    }

  if ( cur_mon.output != new_mon.output )
    {
      return true;
    }

  if ( cur_mon.control != new_mon.control )
    {
      return true;
    }

  if ( cur_mon.vset != new_mon.vset )
    {
      return true;
    }

  if ( cur_mon.rampup != new_mon.rampup )
    {
      return true;
    }

  if ( cur_mon.rampdown != new_mon.rampdown )
    {
      return true;
    } 

  if ( cur_mon.imax != new_mon.imax)
    {
      return true;
    }

  if (abs(cur_mon.vmon - new_mon.vmon) >= newData.vmon_threshold)
    {
      return true;
    }

  if (abs(cur_mon.imon - new_mon.imon) >= newData.imon_threshold)
    {
      return true;     
    }

  if ( cur_mon.polarity != new_mon.polarity )
    {
      return true;
    }


  return false;
}


int ReadPrimaryConfig( int hostid )
{

  for ( int port = 0; port < MAX_COM_PORT; port++ )
    {
      map < int, HVPrimary * > prime;
      int primecnt = 0; // Number of Primary PS found on COM port
      if ( InitCOM( port ) < 0 )
        {
          LOG4CPLUS_ERROR (hvlog, "Unable to open COM" << port);
        }
      else
        {
          // Autodetect primaries first 16 GPIB addresses scan
          LOG4CPLUS_DEBUG (hvlog, "COM" << port << " is opened");
          unsigned int val = 0;
          for ( int addr = 0; addr < MAX_GPIB_ADDR; addr++ )
            {

//	      if ( ( SendHVCmd( port, addr, "REN" ) > 0 ) && ( GetControlStatus( port, addr, & val ) == 0 ) )
              if ( SendHVCmd( port, addr, "REN" ) > 0 )
                {
                  usleep(100000);
                  if ( GetControlStatus( port, addr, & val ) != 0 ) continue;
                  if (val==1)
                    {
                      float volt=0;
                      SetHVMaxVoltage(port, addr, 0);
                      ResetHV(port, addr);
                      usleep(100000);
                      EnableHVOutput(port, addr);
                      usleep(100000);
                      unsigned int status=10;
                      GetOutputStatus(port, addr, &status);
                      if (status==0)
                        {
                          LOG4CPLUS_WARN (hvlog, "Unable to control Primary PS at COM" << port << " GPIB Address#" << addr <<". Removed");
                        }
                      else
                        {

                          LOG4CPLUS_INFO (hvlog, "Primary PS found at COM" << port << " GPIB Address#" << addr);
                          primecnt++;
                          prime[addr] = new HVPrimary;
                          memset( prime[addr], 0, sizeof( HVPrimary ) );
                          prime[addr]->hostid = hostid;
                          prime[addr]->port = port;
                          prime[addr]->addr = addr;
                          prime[addr]->rampup = PRIME_RAMPUP;
                          prime[addr]->rampdown = PRIME_RAMPDOWN;
                          prime[addr]->imax = PRIME_IMAX;
                          prime[addr]->control = 1;
                          prime[addr]->vset=0;
                          ResetHV(port, addr);
                        }
                    }
                }
            }
        }
      if ( primecnt ) primaries[port] = prime;
      else
        {
          LOG4CPLUS_INFO (hvlog, "No Primary PS found at COM" << port);
          if ( CloseCOM( port ) < 0 )
            {
              LOG4CPLUS_ERROR (hvlog, "Unable to close COM" << port);
            }
          else
            {
              LOG4CPLUS_DEBUG (hvlog, "COM" << port << " is closed");
            }
        }
    }
  return 0;
}

int countPrimaries()
{
  int cntr=0;
  for ( map < int, map < int, HVPrimary * > >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr )
    for ( map < int, HVPrimary * >::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr )
      cntr++;
  return cntr;

}

void *trdUpdatePrimaryControl(void* arg)
{
//	LOG4CPLUS_DEBUG (hvmonlog, "Updating Primaries");
  // CheckForTrips();
  int MAX_DELAY=4;
  int delay=50000;
  float loop_time=0;
  float full_loop_time=0;
  int numPS=0;
  struct timeval t0, t1, t2;
  while (1)
    {
      gettimeofday(&t0, NULL);
      usleep(delay);
      if (fAutoDetect)
        {
          usleep(100000);
          continue;
        }

      gettimeofday(&t1, NULL);

      numPS=0;
      for ( map < int, map < int, HVPrimary * > >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr )
        {
          for ( map < int, HVPrimary * >::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr )
            {
              numPS++;
              pthread_mutex_lock(&PrimeIOmutex);
              HVPrimary * h = h_itr->second;
              // check by Madorsky to prevent Seg. faults
              if (h != NULL)
                {
                  h->hostid = hcHostID;
                  h->port = itr->first;
                  h->addr = h_itr->first;
                  short t;

                  UINT control=h->control;
                  if (GetControlStatus( h->port, h->addr, & ( control ) ) < 0)
                    {
                      LOG4CPLUS_WARN(hvmonlog, "Error reading control status for " << PRIMARY_STR(h));
                      h->control = 0;
                    }
                  else h->control = control;

                  if (h->control == 0)
                    {
                      LOG4CPLUS_WARN(hvmonlog, PRIMARY_STR(h) << " remote control is disabled. Re-enabling...");
                      SendHVCmd( h->port, h->addr, "REN" );
                      SetHVMaxVoltage( h->port, h->addr, 0 );
                      ResetHV( h->port, h->addr );
                      // SetHVMaxVoltage( h->port, h->addr, 0 );
                      h->vcur = 0;
                      h->status = HV_STATE_ILOCK;
                      UpdatePrimaryStatus(h);
                    }
                  else
                    {

                      UINT output = h->output;
                      if (GetOutputStatus( h->port, h->addr, & ( output ) ) < 0)
                        {
                          LOG4CPLUS_WARN(hvmonlog, "Error reading output status for " << PRIMARY_STR(h));
                        }
                      else h->output = output;

                      int attempts = 0;
                      while ((h->output != h->outset) && (attempts < PRIMARY_SET_OUTPUT_ATTEMPTS))
                        {

                          LOG4CPLUS_DEBUG (hvmonlog,
                                           PRIMARY_STR(h) << " Set state to "
                                           << (h->outset? "ON": "OFF"));
                          SendHVCmd( h->port, h->addr, "REN" );
                          SetHVMaxVoltage( h->port, h->addr, 0 );
                          h->vcur = 0;
                          if ( h->outset )
                            {
                              EnableHVOutput( h->port, h->addr );
                              h->state = HV_STATE_ON;
                            }
                          else
                            {
                              DisableHVOutput( h->port, h->addr );
                              h->state = HV_STATE_OFF;
                            }
                          usleep(SEC/4);
                          attempts++;
                          if (GetOutputStatus( h->port, h->addr, & ( output ) ) < 0)
                            {
                              LOG4CPLUS_WARN(hvmonlog, "Error reading output status for " << PRIMARY_STR(h));
                            }
                          else h->output = output;
                        }
                      if (attempts >= PRIMARY_SET_OUTPUT_ATTEMPTS)
                        {
                          LOG4CPLUS_WARN(hvmonlog, PRIMARY_STR(h) << " Unable to set " << (h->outset? "ON": "OFF") << " state. Resetting...");
                          SendHVCmd( h->port, h->addr, "REN" );
                          SetHVMaxVoltage( h->port, h->addr, 0 );
                          ResetHV( h->port, h->addr );
                          // SetHVMaxVoltage( h->port, h->addr, 0 );
                          h->vcur = 0;
                          h->status = HV_STATE_ILOCK;
                          UpdatePrimaryStatus(h);
                        }
                      // GetPolarityStatus( h->port, h->addr, & ( h->polarity ) );
                      if ( (h->state == HV_STATE_ON) && (h->status != HV_STATE_ILOCK) )
                        {
                          if ( h->vcur < 0 ) h->vcur = 0;
                          int delta = h->vset - h->vcur;
                          HV_STATUS status = h->status;
                          h->status = (delta > 0) ? HV_STATE_RAMP_UP:
                                      ((delta<0)? HV_STATE_RAMP_DOWN: HV_STATE_ON);
                          if (status != h->status) UpdatePrimaryStatus(h);
                          delta = ( delta > 0 ) ? ( h->rampup > abs( delta ) ? delta : h->rampup ) :
                                      ( h->rampdown > abs( delta ) ? delta : -h->rampdown );

                          h->vcur += delta;
                          SetHVMaxVoltage( h->port, h->addr, ( float )h->vcur );
                        }

                      if ( h->state == HV_STATE_OFF )
                    {
                          if (h->vcur > 0)
                            {
                              int delta = h->rampdown;

                              HV_STATUS status = h->status;
                              h->status = HV_STATE_RAMP_DOWN;
                              if (status != h->status) UpdatePrimaryStatus(h);
                              if (h->vcur < h->rampdown ) delta = h->vcur;
                              h->vcur -= delta;
                              SetHVMaxVoltage( h->port, h->addr, ( float )h->vcur );
                            }
                          else
                            {
                              h->vcur = 0;
                              HV_STATUS status = h->status;
                              h->status = HV_STATE_OFF;
                              if (status != h->status)
                                {
                                  UpdatePrimaryStatus(h);
                                  h->outset = 0;
                                }

                            }
                        }
                      ReadHVMaxCurrent( h->port, h->addr, & ( h->imax ) );
                      ReadHVVoltage( h->port, h->addr, & ( h->vmon ) );
                      ReadHVCurrent( h->port, h->addr, & ( h->imon ) );
                    }
                }
              pthread_mutex_unlock(&PrimeIOmutex);
            }
        }

      gettimeofday(&t2, NULL);

      loop_time = ((t2.tv_sec+t2.tv_usec/1000000.) - (t1.tv_sec+t1.tv_usec/1000000.));
      if (loop_time < 1.)
        delay=(int)((1.-loop_time)*1000000);
      else
        {
          delay = 100000;
        }

      full_loop_time = ((t2.tv_sec+t2.tv_usec/1000000.) - (t0.tv_sec+t0.tv_usec/1000000.));
//		LOG4CPLUS_DEBUG (hvtiminglog, "Pr PS timing> full loop: " << fixed << full_loop_time
//						 << "s, ctrl loop: " << loop_time << "s, added delay: " << (delay/1000000.) <<"s");

//		if (numPS) delay /= numPS;

      if (full_loop_time > MAX_DELAY)  LOG4CPLUS_WARN (hvtiminglog, "primary loop exceeds " << MAX_DELAY << "sec: " << fixed << full_loop_time << "sec");
    }

}

void UpdatePrimaryStatus(HVPrimary * pr)
{
  map<HVPrimary*,vector<HVcmd> >::iterator itr = primaries_map.find(pr);
  if (itr != primaries_map.end())
    {
      for (vector<HVcmd>::iterator m_itr = itr->second.begin(); m_itr != itr->second.end(); ++m_itr)
        {
          HVcmd cmd = *m_itr;
          cmd.data = (int)pr->status;
          cmd.cmd = HV_SET_MOD_PRIMARY_STATUS;
          cmd.chan = ALL;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
        }
    }
}

void RpcPrimeHV::rpcHandler()
{
  HVcmdDIM * cmdDIM = ( HVcmdDIM * ) getData();
  HVcmd * cmd = & ( cmdDIM->data );

  // ========= mycorr++ PVSS_MODE  =============================================================
  if ( cmd->size == -1 )
    {
      PVSS_MODE = 1;
      cmd->size = 0;
    }
  else if ( cmd->size == -2 )
    {
      PVSS_MODE = 2;
      cmd->size = 0;
    }
  else
    PVSS_MODE = 0;
  //========== end mycorr++ PVSS_MODE ==========================================================

  if ( fAutoDetect)
    {
      // LOG4CPLUS_DEBUG (hvcmdlog,"RpcPrimeHV: Can not process request from " << HVDimServer::getClientName() << ": Waiting for autodetection to finish");
      return;
    }


  if ( rpcname == string( servername + "/HV_PRIMARY_CMD" ) )
    {
      LOG4CPLUS_DEBUG (hvcmdlog,rpcname << " Command request from " << HVDimServer::getClientName()
                       << " card:" << (int)cmd->card << " module:" << (int)cmd->module
                       << " chan:" << (int)cmd->chan << " cmd:" << (int)cmd->cmd << "(" << hvcmd_str[(int)cmd->cmd]
                       << ") data:" << (int)cmd->data << " size:" << (int)cmd->size);
      pthread_mutex_lock(&CSCMonmutex);
//      cout << "took prime1" << endl;
//      commands_queue["HVPRCMD"].push_back(*cmdDIM);
      processPrimaryHVCommand(& (cmdDIM->data));
      pthread_mutex_unlock(&CSCMonmutex);
      setData(cmdDIM, sizeof(cmdDIM));
    }


  if ( rpcname == string( servername + "/HV_PRIMARY_DATA" ) )
    {
      // LOG4CPLUS_DEBUG (hvcmdlog,rpcname << " Request for data from " << HVDimServer::getClientName());
      if ( IsPrimaryExists( cmd->card, cmd->module ) )
        {
          HVPrimaryDIM data;
          memset( ( char * ) & data, 0, sizeof( data ) );
          data.ID = cmdDIM->ID;

          pthread_mutex_lock(&PrimeIOmutex);
          memcpy( ( char * ) & data.data, primaries[cmd->card] [cmd->module], sizeof( struct HVPrimary ) );
          pthread_mutex_unlock(&PrimeIOmutex);

          if ( !PVSS_MODE ) setData( ( char * ) & data, sizeof( data ) ); // mycorr++
          else
            {
              pthread_mutex_lock(&CSCMonmutex);
//              cout << "took prime2" << endl;
              pvssInterface( & data.data, cmd, PVSS_MODE ); // mycorr++ piece 21
              pthread_mutex_unlock(&CSCMonmutex);
            }
        }
      else
        {
          setData(cmdDIM, sizeof(cmdDIM));
        }
    }
};



bool IsPrimaryExists( int port, int addr )
{
  map < int, map < int, HVPrimary * > >::iterator p_itr = primaries.find( port );
  if ( p_itr == primaries.end() || ( primaries.size() == 0 ) )
    {
      return false;
    }
  map < int, HVPrimary * >::iterator itr = p_itr->second.find( addr );
  if ( itr == p_itr->second.end() || ( p_itr->second.size() == 0 ) )
    {
      return false;
    }
  return true;
};



//=================== mycorr++  piece 31
int RpcPrimeHV::pvssInterface( HVPrimary * data, HVcmd * cmd, int mode )
{

  //::::::::::::
  bool UPDATE_FLAG = false;
  if ( mode == 1 ) UPDATE_FLAG = true;
  //::::::::::::



  HVPrimaryPVSS pvss_pr_l;


  memset(&pvss_pr_l, 0, sizeof(pvss_pr_l));

  //	data->hostid = atoi( host_database.c_str() );

  pvss_pr_l.hp.hostid = data->hostid;
  pvss_pr_l.hp.port = (int)(data->port&0xFF);
  pvss_pr_l.hp.addr = (int)(data->addr&0xFF);
  pvss_pr_l.hp.polarity = data->polarity;
  pvss_pr_l.hp.outset = data->outset;
  pvss_pr_l.hp.output = data->output;
  pvss_pr_l.hp.control = data->control;
  pvss_pr_l.hp.imax = (int)data->imax;
  pvss_pr_l.hp.vset = data->vset;
  pvss_pr_l.hp.imon = (int)data->imon;
  pvss_pr_l.hp.vmon = (int)data->vmon;
  pvss_pr_l.hp.vcur = data->vcur;
  pvss_pr_l.hp.rampup = data->rampup;
  pvss_pr_l.hp.rampdown = data->rampdown;

  pvss_pr_l.status=data->status;
  // memcpy( ( char * ) & pvss_pr_l, ( char * ) data, sizeof( pvss_pr_l ) );
#ifdef PVSSDEBUG_10
  printf( "HOST_ID %d %d\n", data->hostid, pvss_pr_l.hp.rampup );
#endif

  if (pvss_pr_l.hp.polarity != 0)printf("POLARITY !=0   -----> %d %d \n",pvss_pr_l.hp.polarity, data->polarity);


  //--


  int db_index = -1;
  for ( int i10 = 0; i10 < primary_database.size(); i10++ )
    {
      if ( primary_database[i10] == pvss_pr_l.hp.port &&  primary_database_2[i10] == pvss_pr_l.hp.addr)
        {
          db_index = i10;
          break;
        }
    }


  //::::::::::::::::
  if (pvss_pr_l.status == HV_STATE_ILOCK && previous_pr_error_status[db_index]==0)
    {
      UPDATE_FLAG=true;
      previous_pr_error_status[db_index]=1;
    }
  else if (pvss_pr_l.status != HV_STATE_ILOCK  && previous_pr_error_status[db_index]==1)
    {
      UPDATE_FLAG=true;
      previous_pr_error_status[db_index]=0;
    }
  //::::::::::::::::
  if ( UPDATE_FLAG ) //:::::::::::::
    {
      pvss_pr_queue.push_back( pvss_pr_l );
      db_index_pr_queue.push_back( db_index );
    } // if ( UPDATE_FLAG ) //::::::::::::

  //--
  return 0;
}


//=======================mycorr++ end of piece 31

//=================== mycorr++  piece 31
int pvssInterfacePrimaryHVData( HVcmd * cmd)
{

  // LOG4CPLUS_DEBUG(hvlog, "Update Primary Data");
  if ( cmd->size == -1 )
    {
      PVSS_MODE = 1;
      cmd->size = 0;
    }
  else if ( cmd->size == -2 )
    {
      PVSS_MODE = 2;
      cmd->size = 0;
    }
  else
    PVSS_MODE = 0;

  //::::::::::::
  bool UPDATE_FLAG = false;
  if ( PVSS_MODE == 1 ) UPDATE_FLAG = true;
  //::::::::::::

  HVPrimary data;
  memset( ( char * ) & data, 0, sizeof( data ) );
  pthread_mutex_lock(&PrimeIOmutex);
  memcpy( ( char * ) & data, primaries[cmd->card] [cmd->module], sizeof( struct HVPrimary ) );
  pthread_mutex_unlock(&PrimeIOmutex);

  HVPrimaryPVSS pvss_pr_l;


  memset(&pvss_pr_l, 0, sizeof(pvss_pr_l));

  //	data->hostid = atoi( host_database.c_str() );

  pvss_pr_l.hp.hostid = data.hostid;
  pvss_pr_l.hp.port = (int)(data.port&0xFF);
  pvss_pr_l.hp.addr = (int)(data.addr&0xFF);
  pvss_pr_l.hp.polarity = data.polarity;
  pvss_pr_l.hp.outset = data.outset;
  pvss_pr_l.hp.output = data.output;
  pvss_pr_l.hp.control = data.control;
  pvss_pr_l.hp.imax = (int)data.imax;
  pvss_pr_l.hp.vset = data.vset;
  pvss_pr_l.hp.imon = (int)data.imon;
  pvss_pr_l.hp.vmon = (int)data.vmon;
  pvss_pr_l.hp.vcur = data.vcur;
  pvss_pr_l.hp.rampup = data.rampup;
  pvss_pr_l.hp.rampdown = data.rampdown;

  pvss_pr_l.status=data.status;
  // memcpy( ( char * ) & pvss_pr_l, ( char * ) data, sizeof( pvss_pr_l ) );
#ifdef PVSSDEBUG_10
  printf( "HOST_ID %d %d\n", data.hostid, pvss_pr_l.hp.rampup );
#endif

  if (pvss_pr_l.hp.polarity != 0) printf("POLARITY !=0   -----> %d %d \n",pvss_pr_l.hp.polarity, data.polarity);


  //--


  int db_index = -1;
  for ( int i10 = 0; i10 < primary_database.size(); i10++ )
    {
      if ( primary_database[i10] == pvss_pr_l.hp.port &&  primary_database_2[i10] == pvss_pr_l.hp.addr)
        {
          db_index = i10;
          break;
        }
    }


  //::::::::::::::::
  if (pvss_pr_l.status == HV_STATE_ILOCK && previous_pr_error_status[db_index]==0)
    {
      UPDATE_FLAG=true;
      previous_pr_error_status[db_index]=1;
    }
  else if (pvss_pr_l.status != HV_STATE_ILOCK  && previous_pr_error_status[db_index]==1)
    {
      UPDATE_FLAG=true;
      previous_pr_error_status[db_index]=0;
    }
  //::::::::::::::::
  if ( UPDATE_FLAG ) //:::::::::::::
    {
      pvss_pr_queue.push_back( pvss_pr_l );
      db_index_pr_queue.push_back( db_index );
    } // if ( UPDATE_FLAG ) //::::::::::::

  //--
  return 0;
}


int processPrimaryHVCommand(HVcmd* cmd)
{
  int port = cmd->card;
  int addr = cmd->module;
//	LOG4CPLUS_DEBUG(hvlog, "Process Primary Command");

  for (map<int, map<int, HVPrimary*> >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr)
    {
      if (cmd->card == ALL) port = itr->first;
      for (map<int, HVPrimary*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
        {
          pthread_mutex_lock(&PrimeIOmutex);
          if (cmd->module == ALL) addr = h_itr->first;
          if ( IsPrimaryExists( itr->first, h_itr->first )
               && (itr->first == port)
               && (h_itr->first == addr) )
            {
              switch ( cmd->cmd )
                {
                case HV_RESET_TRIP:
                  SendHVCmd( port, addr, "REN" );
                  ResetHV( port, addr );
                  //	      primaries[port] [addr]->outset = 0;
                  //	      primaries[port] [addr]->state = HV_STATE_OFF;
                  //	      primaries[port] [addr]->status = HV_STATE_OFF;

                  break;
                case HV_SET_ON:
                  // EnableHVOutput(cmd->card, cmd->module);
                  primaries[port] [addr]->outset = 1;
                  primaries[port] [addr]->state = HV_STATE_ON;
                  // primaries[port] [addr]->status = HV_STATE_ON;
                  UpdatePrimaryStatus(primaries[port] [addr]);
                  break;
                case HV_SET_OFF:
                  // DisableHVOutput(cmd->card, cmd->module);
                  // primaries[port] [addr]->outset = 0;
                  primaries[port] [addr]->state = HV_STATE_OFF;
                  // primaries[port] [addr]->status = HV_STATE_OFF;
                  UpdatePrimaryStatus(primaries[port] [addr]);
                  break;
                case HV_SET_MOD_STATE:
                  primaries[port] [addr]->state = (HV_STATE)cmd->data;
                  switch ( cmd->data )
                    {
                    case HV_STATE_OFF:
                      // primaries[port] [addr]->outset = 0;
                      primaries[port] [addr]->state = HV_STATE_OFF;
                      //  primaries[port] [addr]->status = HV_STATE_OFF;
                      UpdatePrimaryStatus(primaries[port] [addr]);
                      // DisableHVOutput(cmd->card, cmd->module);
                      break;
                    case HV_STATE_ON:
                      primaries[port] [addr]->outset = 1;
                      primaries[port] [addr]->state = HV_STATE_ON;
                      // primaries[port] [addr]->status = HV_STATE_ON;
                      UpdatePrimaryStatus(primaries[port] [addr]);
                      // EnableHVOutput(cmd->card, cmd->module);
                      break;
                    };
                  break;
                case HV_SET_RAMP_UP:
                  primaries[port] [addr]->rampup = (cmd->data>MAX_PRIME_RAMP_UP)?MAX_PRIME_RAMP_UP:cmd->data;
                  break;
                case HV_SET_RAMP_DOWN:
                  primaries[port] [addr]->rampdown = cmd->data;
                  break;
                case HV_SET_VSET:
                  if (cmd->size==1)   // Calibration mode with max VSet=4000V
                    {
                      primaries[port] [addr]->vset = (cmd->data>4000)?4000:cmd->data;
                    }
                  else   // Standard mode with max VSet=MAX_PRIME_VSET
                    {
                      primaries[port] [addr]->vset = (cmd->data>MAX_PRIME_VSET)?MAX_PRIME_VSET:cmd->data;
                    }
                  if (labs (primaries[port] [addr]->vset - primaries[port] [addr]->vcur) < primaries[port] [addr]->rampup)
                    {
                      primaries[port] [addr]->vcur = primaries[port] [addr]->vset;
                      SetHVMaxVoltage( port, addr, ( float )primaries[port] [addr]->vcur );
                      usleep(10000);
                      ReadHVVoltage( port, addr, & ( primaries[port] [addr]->vmon ) );
                    }
                  break;
                case HV_SET_IMAX:
                  primaries[port] [addr]->imax = ( float )cmd->data;
                  SetHVMaxCurrent( port, addr, ( float )cmd->data );
                  break;
                }
            }
          pthread_mutex_unlock(&PrimeIOmutex);
        }
    }
}
