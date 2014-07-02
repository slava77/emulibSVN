#include "hvserver.h"


vector<HVcsccmd> getCommands(string cmdstr)
{
  vector<HVcsccmd> cmds;
  HVcsccmd cmd;
  string tmpstr = cmdstr;
  vector<string> csclist;
  cmds.clear();
  csclist.clear();
  while (tmpstr.find(";") != string::npos)
    {
      string token = tmpstr.substr(0,tmpstr.find(";"));
      if (token.find("CHAN:")!=string::npos)
        {
          string chan = token.erase(0, token.find("CHAN:")+5);
          cmd.chan = atoi(chan.c_str());
        }
      if (token.find("CMD:")!=string::npos)
        {
          string scmd = token.erase(0,token.find("CMD:")+4);
          cmd.cmd = (HV_CMD)atoi(scmd.c_str());
        }
      if (token.find("DATA:")!=string::npos)
        {
          string data = token.erase(0, token.find("DATA:")+5);
          cmd.data = atoi(data.c_str());
        }
      if (token.find("SIZE:")!=string::npos)
        {
          string size = token.erase(0,token.find("SIZE:")+5);
          cmd.size = atoi(size.c_str());
        }
      if (token.find("CSC:")!=string::npos)
        {
          string cscname=token.erase(0, token.find("CSC:")+4);
          csclist.push_back(cscname);
        }
      tmpstr.erase(0, tmpstr.find(";")+1);
    }

  for (int i=0; i<csclist.size(); i++)
    {
      memcpy(&(cmd.csc), csclist[i].c_str(), sizeof(cmd.csc));
      cmds.push_back(cmd);
    }
  return cmds;
}

// Check if all Channels on Chamber are set to OFF state
bool isAllChannelsAreOff(HVmodule* module, int rdb_part=0)
{
  if (module == NULL) return false;
  int start_chan = 0;
  int end_chan = module->num_chans;

  if (module->type == RDB36)
    {
      // if RDB part is defined check only shared RDB36 channels
      if (rdb_part > 0)
        {
          if (rdb_part == 1)
            {
              start_chan = 18;
              end_chan = module->num_chans;
            }
          else
            {
              start_chan = 0;
              end_chan = 18;
            }
        }
      for (int i = start_chan; i < end_chan; i++)
        if (module->chan[i].state != HV_STATE_OFF) return false;

      return true;

    }
  else return false;
}

bool ParseCSC(string csc_name, HVcsc * csc)
{
  int (*pf)(int)=toupper;
  string name = csc_name;
  if (name.length() > 12) return false;
  transform(name.begin(), name.end(), name.begin(), pf);
  int pos = 0;
  if (pos = name.find("ME-") != string::npos)
    {
      csc->endcap = 0;
    }
  else    if (pos = name.find("ME+") != string::npos)
    {
      csc->endcap = 1;
    }
  else return false;

  name = name.substr(pos+2, name.length());
  replace(name.begin(), name.end(), '/', ' ');
  replace(name.begin(), name.end(), '-', ' ');
  stringstream st(name);
  while (st>> csc->station >> csc->ring >> csc->position)
    {
      sprintf(csc->name, "ME%s%1d/%1d/%02d", ((csc->endcap)?"+":"-"), csc->station, csc->ring, csc->position);
      return true;
    }
  return false;
}

bool LoadCSCMap()
{
  char line[256];
  string mapfname = "map.txt";
  int count = 0;
  stringstream st;
  st << HV_DATADIR << "/" << mapfname;
  st >> mapfname;
  LOG4CPLUS_INFO (hvlog, "Load CSC Map from " << mapfname);
  ifstream mapfile(mapfname.c_str());
  if (mapfile)
    {
      csc_map.clear();
      while (!mapfile.eof())
        {
          mapfile.getline(line,255);
          string str(line);
          if (str.find("ME") ==0)
            {
              stringstream st(str);
              string csc_name;
              int rdb, rdb_part;
              while (st >> csc_name >> rdb >> rdb_part)
                {
                  LOG4CPLUS_DEBUG (hvlog, csc_name << " " << rdb << " " << rdb_part);
                  csc_map[csc_name] = make_pair(rdb, rdb_part);
                  count++;
                }
            }
        }
    }
  else return false;
  LOG4CPLUS_INFO (hvlog, "Found " << count << " mappings");
  return true;

}

bool UpdateCSCMonData(std::string cscID, HVcscMon& cscMon)
{

  int trip_state = 0; // Trip states counter

  map<string, pair<int, int> >::iterator csc_itr = csc_map.find(cscID);
  if (csc_itr == csc_map.end() || (csc_map.size()==0))
    {
      LOG4CPLUS_ERROR (hvlog, "CSC " << cscID << " can not find mapping");
      return false;
    }

  int rdbID = csc_itr->second.first;
  int rdb_part = csc_itr->second.second;

  map<int, HVcmd >::iterator rdb_itr = dbs_map.find(rdbID);
  if (rdb_itr == dbs_map.end() || (dbs_map.size()==0))
    {
      LOG4CPLUS_ERROR (hvlog, "RDB " << rdbID << " can not find config info");
      return false;;

    }

  HVcmd *rdb_info = &rdb_itr->second;

  // Copy RDB part data
  if (LoadCalibration(rdb_info->card, rdb_info->module))
    {
      HVmodule rdb;
      pthread_mutex_lock(&IOmutex);
      HVhostcard * hvcard = dataservices[rdb_info->card].data;

      HVcalibdata& data = dataservices[rdb_info->card].calib[rdb_info->module];
      memcpy(&rdb, &hvcard->module[rdb_info->module], sizeof(rdb));
      pthread_mutex_unlock(&IOmutex);

      HVcscFastMon& fast_mon = cscMon.fast_mon;
      HVcscSlowMon& slow_mon = cscMon.slow_mon;


      // Find number of channels and RDB channels range offset for CSC
      int num_chans = 30;
      if (rdb.type == RDB36) num_chans = 18;
      int ch_offset = 0;
      if ((rdb.type == RDB36) && (rdb_part == 2)) ch_offset = 18;

      // Copy module related monitorable parameters to chamber data structure
      float hvmon = rdb.hvmon*data.vmon.A +  data.vmon.B;
      slow_mon.num_chans = num_chans;
      fast_mon.num_chans = num_chans;
      fast_mon.hvmon = (hvmon<0)?0:(UINT)hvmon;
      fast_mon.mod_status = rdb.status;
      fast_mon.interlock = rdb.interlock;
      fast_mon.pos5v = rdb.pos5v;
      fast_mon.neg5v = rdb.neg5v;
      slow_mon.rdb_type = rdb.type;
      slow_mon.rdb_id = rdb.ID;
      slow_mon.mod_state = rdb.state;
      slow_mon.ilock_delay = rdb.ilock_delay;
      slow_mon.master_trip_delay = rdb.master_trip_delay;

      // Copy and convert RDB channels parameters to CSC channels
      for (int i=0; i< num_chans; i++)
        {
          HVchanFastMon& fast_ch = cscMon.fast_mon.chan[i];
          HVchanSlowMon& slow_ch = cscMon.slow_mon.chan[i];
          int k = i+ch_offset;
          HVchannel& ch = rdb.chan[k];
          fast_ch.status = ch.status;
          slow_ch.state = ch.state;
          slow_ch.vtrip_delay = ch.vtrip_delay;
          slow_ch.itrip_delay = ch.trip_delay;

          // Count number of channels in TRIP state
          if (fast_ch.status == HV_STATE_CHAN_TRIP || fast_ch.status == HV_STATE_OV_TRIP) trip_state++;

          if (fast_ch.status == HV_STATE_DEAD)
            {
              fast_ch.vmon = 0;
              fast_ch.imon = 0;
            }
          else
            {
              float val = ch.vmon*data.adc[k].A;
              val += (ch.vmon>data.adc[k].B)?data.adc[k].B:0;
              fast_ch.vmon = (val <0)?0:(UINT)val;

              val = ch.vov * data.adc[k].A;
              slow_ch.vov = ( val < 0 ) ? 0 : ( UINT )val;

              val = (ch.vtrip>0)?ch.vtrip*data.adc[k].A + data.adc[k].B:ch.vtrip;
              slow_ch.vtrip = (val <0)?0:(UINT)val;

              if (data.dac[k].A >0)
                {

                  val = ch.ramp_up/data.dac[k].A;
                  slow_ch.ramp_up = (val <0)?0:(UINT)val;

                  val = ch.ramp_down/data.dac[k].A;
                  slow_ch.ramp_down = (val <0)?0:(UINT)val;

                  val = (ch.vset - data.dac[k].B)/data.dac[k].A;
                  slow_ch.vset = (val <0)?0:(UINT)val;
                }
              if (data.type == MASTER)
                {
                  fast_ch.imon = (((data.cur[k][ch.imon]-1)*4000))/ADC_CONV_COEF; //nanoAmps
                  slow_ch.itrip = (ch.itrip>0)?((data.cur[k][ch.itrip]-1)*10)/25:ch.itrip;
                  slow_ch.imax = (data.cur[k][ch.imax]*10)/25;
                  slow_ch.imax_ramp = (data.cur[k][ch.imax_ramp]*10)/25;
                }
              else
                {
                  fast_ch.imon = (1000*(data.cur[k][ch.imon]-1))/ADC_CONV_COEF; // nanoAmps
                  slow_ch.itrip = (ch.itrip>0)?(data.cur[k][ch.itrip]-1)/ADC_CONV_COEF:ch.itrip;
                  slow_ch.imax = data.cur[k][ch.imax]/ADC_CONV_COEF;
                  slow_ch.imax_ramp = data.cur[k][ch.imax_ramp]/ADC_CONV_COEF;
                }
            }
        }

      // Reset module TRIP status for shared RDB36 module without tripped channels
      if ( (fast_mon.mod_status == HV_STATE_INT_TRIP) && (trip_state == 0) ) fast_mon.mod_status = slow_mon.mod_state;

      // Copy linked Master channel data
      if (LoadCalibration(rdb.master_hvcard, rdb.master_busaddr))
        {
          HVhostcard * hvcard = dataservices[rdb.master_hvcard].data;
          HVmodule master;

          pthread_mutex_lock(&IOmutex);
          HVcalibdata data = dataservices[rdb.master_hvcard].calib[rdb.master_busaddr];
          memcpy(&master, &hvcard->module[rdb.master_busaddr], sizeof(master));
          pthread_mutex_unlock(&IOmutex);

          fast_mon.master_mod_status = master.status;
          slow_mon.master_type = master.type;
          slow_mon.master_id   = master.ID;
          HVchanFastMon& fast_ch = cscMon.fast_mon.master_chan;
          HVchanSlowMon& slow_ch = cscMon.slow_mon.master_chan;
          HVchannel& ch = master.chan[rdb.master_chan];
          int m_ch = rdb.master_chan;
          fast_ch.status = ch.status;
          slow_ch.state = ch.state;
          slow_ch.vtrip_delay = ch.vtrip_delay;
          slow_ch.itrip_delay = ch.trip_delay;


          if (fast_ch.status == HV_STATE_DEAD)
            {
              fast_ch.vmon = 0;
              fast_ch.imon = 0;
            }
          else
            {
              float val = ch.vmon*data.adc[m_ch].A;
              val += (ch.vmon>data.adc[m_ch].B)?data.adc[m_ch].B:0;
              fast_ch.vmon = (val <0)?0:(UINT)val;

              val = (ch.vtrip>0)?ch.vtrip*data.adc[m_ch].A + data.adc[m_ch].B:ch.vtrip;
              slow_ch.vtrip = (val <0)?0:(UINT)val;

              val = ch.ramp_up/data.dac[m_ch].A;
              slow_ch.ramp_up = (val <0)?0:(UINT)val;

              val = ch.ramp_down/data.dac[m_ch].A;
              slow_ch.ramp_down = (val <0)?0:(UINT)val;

              val = (ch.vset - data.dac[m_ch].B)/data.dac[m_ch].A;
              slow_ch.vset = ( val < 0 ) ? 0 : ((val>MAX_VSET)?MAX_VSET:( UINT )val);

              if (data.type == MASTER)
                {
                  fast_ch.imon =((data.cur[m_ch][ch.imon]-1)*4000)/ADC_CONV_COEF;
                  slow_ch.itrip = (ch.itrip>0)?((data.cur[m_ch][ch.itrip]-1)*10)/25:ch.itrip;
                  slow_ch.imax = (data.cur[m_ch][ch.imax]*10)/25;
                  slow_ch.imax_ramp = (data.cur[m_ch][ch.imax_ramp]*10)/25;
                }
              else
                {
                  fast_ch.imon = (1000*(data.cur[m_ch][ch.imon]-1))/ADC_CONV_COEF;
                  slow_ch.itrip = (ch.itrip>0)?(data.cur[m_ch][ch.itrip]-1)/ADC_CONV_COEF:ch.itrip;
                  slow_ch.imax = data.cur[m_ch][ch.imax]/ADC_CONV_COEF;
                  slow_ch.imax_ramp = data.cur[m_ch][ch.imax_ramp]/ADC_CONV_COEF;
                }
            }
        }
    }

  return true;
}

bool InitCSCMonData()
{
  if (!csc_map.size())
    {
      LOG4CPLUS_WARN (hvlog, "Empty CSC Mapping list");
      return false;
    }

  string fast_mon_size = "I:100";
  string slow_mon_size = "I:412";
  string stats_size = "L:2";

  for (map<string, pair<int, int> >::iterator itr = csc_map.begin(); itr != csc_map.end(); ++itr)
    {
      LOG4CPLUS_DEBUG(hvlog, "Initialize DIM services for CSC " << itr->first);
      HVcscMon& cscMon = cscMonData[itr->first];

      memset(&cscMon,0, sizeof(cscMon));

      // Publish Fast Monitoring Dim service for CSC
      cscMon.fast_mon_dim 	= new DimService(("HV/" + itr->first + "/FAST_MON").c_str(), (char *)fast_mon_size.c_str(),
                                            &(cscMon.fast_mon), sizeof(cscMon.fast_mon));
      // Publish Slow Monitroing Dim service for CSC
      cscMon.slow_mon_dim 	= new DimService(("HV/" + itr->first + "/SLOW_MON").c_str(), (char *)slow_mon_size.c_str(),
                                            &(cscMon.slow_mon), sizeof(cscMon.slow_mon));
      // Publish Monitoring Update Statistics Dim service for CSC
      cscMon.stats_dim 		= new DimService(("HV/" + itr->first + "/STATS").c_str(), (char *)stats_size.c_str(),
                                          &(cscMon.stats), sizeof(cscMon.stats));

      InitCSCMonUpdateParameters(cscMon);
      UpdateCSCMonData(itr->first, cscMon);
      cscMon.fast_mon_dim->updateService();
      cscMon.slow_mon_dim->updateService();
      cscMon.stats_dim->updateService();
//      cscMonData[itr->first] = cscMon;
    }

  return true;
}

void InitCSCMonUpdateParameters(HVcscMon& cscMon)
{
  cscMon.slow_mon.update_interval = DEF_UPDATE_INTERVAL;
  cscMon.slow_mon.master_chan.vmon_threshold = DEF_MASTER_VMON_THRESH;
  cscMon.slow_mon.master_chan.imon_threshold = DEF_MASTER_IMON_THRESH;
  for (int i=0; i<CSC_MAX_CHANS; i++)
    {
      cscMon.slow_mon.chan[i].vmon_threshold = DEF_RDB_VMON_THRESH;
      cscMon.slow_mon.chan[i].imon_threshold = DEF_RDB_IMON_THRESH;
    }
}

bool isSlowMonUpdateNeeded(HVcscMon& newData, HVcscMon& curData)
{
  bool fUpdate = false;
  HVcscSlowMon& new_slow = newData.slow_mon;
  HVcscSlowMon& cur_slow = curData.slow_mon;

  // Check for data changes in common RDB module parameters
  if ( cur_slow.num_chans != new_slow.num_chans ) return true;
  else if ( cur_slow.mod_state != new_slow.mod_state ) return true;
  else if ( cur_slow.ilock_delay != new_slow.ilock_delay ) return true;
  else if ( cur_slow.master_trip_delay != new_slow.master_trip_delay ) return true;
  else if ( cur_slow.update_interval != new_slow.update_interval ) return true;
  else if ( cur_slow.rdb_type != new_slow.rdb_type ) return true;
  else if ( cur_slow.rdb_id != new_slow.rdb_id ) return true;

  // Check for data changes in RDB channels parameters
  for (int i=0; i<new_slow.num_chans; i++)
    {
      if (cur_slow.chan[i].state != new_slow.chan[i].state ) return true;
      else if ( cur_slow.chan[i].vset != new_slow.chan[i].vset ) return true;
      else if ( cur_slow.chan[i].imax != new_slow.chan[i].imax ) return true;
      else if ( cur_slow.chan[i].vov != new_slow.chan[i].vov ) return true;
      else if ( cur_slow.chan[i].ramp_up != new_slow.chan[i].ramp_up ) return true;
      else if ( cur_slow.chan[i].ramp_down != new_slow.chan[i].ramp_down ) return true;
      else if ( cur_slow.chan[i].imax_ramp != new_slow.chan[i].imax_ramp ) return true;
      else if ( cur_slow.chan[i].vtrip_delay != new_slow.chan[i].vtrip_delay ) return true;
      else if ( cur_slow.chan[i].itrip_delay != new_slow.chan[i].itrip_delay ) return true;
      else if ( cur_slow.chan[i].itrip != new_slow.chan[i].itrip ) return true;
      else if ( cur_slow.chan[i].vtrip != new_slow.chan[i].vtrip ) return true;
      else if ( cur_slow.chan[i].vmon_threshold != new_slow.chan[i].vmon_threshold ) return true;
      else if ( cur_slow.chan[i].imon_threshold != new_slow.chan[i].imon_threshold ) return true;
    }

  // Check for data changes in linked Master channel parameters
  if (cur_slow.master_chan.state != new_slow.master_chan.state ) return true;
  else if ( cur_slow.master_chan.vset != new_slow.master_chan.vset ) return true;
  else if ( cur_slow.master_chan.imax != new_slow.master_chan.imax ) return true;
  else if ( cur_slow.master_chan.vov != new_slow.master_chan.vov ) return true;
  else if ( cur_slow.master_chan.ramp_up != new_slow.master_chan.ramp_up ) return true;
  else if ( cur_slow.master_chan.ramp_down != new_slow.master_chan.ramp_down ) return true;
  else if ( cur_slow.master_chan.imax_ramp != new_slow.master_chan.imax_ramp ) return true;
  else if ( cur_slow.master_chan.vtrip_delay != new_slow.master_chan.vtrip_delay ) return true;
  else if ( cur_slow.master_chan.itrip_delay != new_slow.master_chan.itrip_delay ) return true;
  else if ( cur_slow.master_chan.itrip != new_slow.master_chan.itrip ) return true;
  else if ( cur_slow.master_chan.vtrip != new_slow.master_chan.vtrip ) return true;
  else if ( cur_slow.master_chan.vmon_threshold != new_slow.master_chan.vmon_threshold ) return true;
  else if ( cur_slow.master_chan.imon_threshold != new_slow.master_chan.imon_threshold ) return true;

  return fUpdate;
}

bool isFastMonUpdateNeeded(HVcscMon& newData, HVcscMon& curData)
{
  bool fUpdate = false;

  HVcscFastMon& new_fast = newData.fast_mon;
  HVcscFastMon& cur_fast = curData.fast_mon;

  // Check for data changes in common RDB module parameters
  if ( cur_fast.mod_status != new_fast.mod_status )
    {
      return true;
    }
  else if ( cur_fast.master_mod_status != new_fast.master_mod_status )
    {
      return true;
    }
  else if ( cur_fast.interlock != new_fast.interlock )
    {
      return true;
    }
  else if ( abs(cur_fast.hvmon - new_fast.hvmon) >= PVSS_VMON_THRESHOLD)
    {
      return true;
    }
  else if ( cur_fast.pos5v != new_fast.pos5v )
    {
      return true;
    }
  else if ( cur_fast.neg5v != new_fast.neg5v )
    {
      return true;
    }
  else if ( cur_fast.num_chans != new_fast.num_chans )
    {
      return true;
    }

  // Check for data changes in RDB channels parameters
  for (int i=0; i<new_fast.num_chans; i++)
    {
      if (cur_fast.chan[i].status != new_fast.chan[i].status)
        {
          return true;
        }
      else if ( ( (new_fast.chan[i].status != HV_STATE_RAMP_UP) && (new_fast.chan[i].status != HV_STATE_RAMP_DOWN) )
                && abs(cur_fast.chan[i].vmon - new_fast.chan[i].vmon) >= newData.slow_mon.chan[i].vmon_threshold )
        {
          return true;
        }
      else if ( ( (new_fast.chan[i].status == HV_STATE_RAMP_UP) || (new_fast.chan[i].status == HV_STATE_RAMP_DOWN) )
                && abs(cur_fast.chan[i].vmon - new_fast.chan[i].vmon) >= DEF_RDM_VMON_RAMP_THRESH )
        {
          return true;
        }
      else if ( abs(cur_fast.chan[i].imon - new_fast.chan[i].imon) >= 1000*((float)newData.slow_mon.chan[i].imon_threshold)/ADC_CONV_COEF )
        {
          return true;
        }
    }


  // Check for data changes in linked Master channel parameters
  if (cur_fast.master_chan.status != new_fast.master_chan.status)
    {
      return true;
    }
  else if ( ( (new_fast.master_chan.status != HV_STATE_RAMP_UP) && (new_fast.master_chan.status != HV_STATE_RAMP_DOWN) )
            && abs(cur_fast.master_chan.vmon - new_fast.master_chan.vmon) >= newData.slow_mon.master_chan.vmon_threshold )
    {
      return true;
    }
  else if ( ( (new_fast.master_chan.status == HV_STATE_RAMP_UP) || (new_fast.master_chan.status == HV_STATE_RAMP_DOWN) )
            && abs(cur_fast.master_chan.vmon - new_fast.master_chan.vmon) >= newData.slow_mon.master_chan.vmon_threshold )
    {
      return true;
    }
  else if ( abs(cur_fast.master_chan.imon - new_fast.master_chan.imon) >= 1000*((float)newData.slow_mon.master_chan.imon_threshold)/ADC_CONV_COEF )
    {
      return true;
    }



  return fUpdate;
}

int processCSCHVCommand(HVcsccmd* csccmd)
{

  string CSC(csccmd->csc);
  HVcsc csc;
  if (!ParseCSC(CSC, &csc))
    {
      LOG4CPLUS_ERROR (hvlog, "CSC " << CSC << " can not parse name");
      return -1;
    }

  CSC = csc.name;
  map<string, pair<int, int> >::iterator csc_itr = csc_map.find(CSC);
  if (csc_itr == csc_map.end() || (csc_map.size()==0))
    {
      LOG4CPLUS_ERROR (hvlog, "CSC " << CSC << " can not find mapping");
      return -2;
    }
  int rdbID = csc_itr->second.first;

  map<int, HVcmd >::iterator rdb_itr = dbs_map.find(rdbID);
  if (rdb_itr == dbs_map.end() || (dbs_map.size()==0))
    {
      LOG4CPLUS_ERROR (hvlog, "RDB " << rdbID << " can not find config info");
      return -3;

    }
  HVcmd *rdb_info = &rdb_itr->second;


  int prime_port = rdb_info->data & 0xFFFF;
  int prime_addr = (rdb_info->data >> 16)&0xFFFF;
  map<int, map<int, HVPrimary *> >::iterator prime_port_itr = primaries.find(prime_port);
  if (prime_port_itr == primaries.end() || (primaries.size()==0))
    {
      LOG4CPLUS_ERROR (hvlog, CSC << " can not find Primary PS");
      return -4;
    }
  else
    {
      map<int, HVPrimary *>::iterator prime_addr_itr = prime_port_itr->second.find(prime_addr);
      if (prime_addr_itr == prime_port_itr->second.end() || (prime_port_itr->second.size()==0))
        {
          LOG4CPLUS_ERROR (hvlog, CSC << " can not find Primary PS");
          return -5;
        }
    }

  /// Force data update from driver
  HVhostcard newdata;
  // for (int i=0; i<dataservices.size(); i++)
  // if ( ( (csccmd->cmd == HV_SET_MOD_STATE) || (csccmd->cmd == HV_SET_STATE) || (csccmd->cmd == HV_SET_ON) || (csccmd->cmd == HV_SET_OFF) )
  //	&& rdb_info->card >=0 && rdb_info->card < dataservices.size())
  if (rdb_info->card >=0 && rdb_info->card < dataservices.size())
    {
      pthread_mutex_lock(&IOmutex);
      GetHVData(fd,rdb_info->card,0,0, HV_GET_CARD_DATA, (ULONG*)&newdata,sizeof(struct HVhostcard));
      // memcpy(dataservices[rdb_info->card].data, &newdata, sizeof(struct HVhostcard));
      pthread_mutex_unlock(&IOmutex);
    }

  HVcmd realcmd;
  HVcmd mastercmd  = masters_map[newdata.module[rdb_info->module].master_id];
  HVPrimary * prime = primaries[prime_port][prime_addr];
  mastercmd.chan = newdata.module[rdb_info->module].master_chan;
  mastercmd.cmd = csccmd->cmd;
  mastercmd.data = csccmd->data;
  mastercmd.size = 0;

  int rdb_part = csc_itr->second.second;

  range itr_chan;
  switch (csccmd->chan)
    {
    case ALL:
      itr_chan.start = (newdata.module[rdb_info->module].type == RDB36
                        && rdb_part==2)?18:0;
      itr_chan.end = (newdata.module[rdb_info->module].type == RDB36 &&
                      rdb_part==1)?18:newdata.module[rdb_info->module].num_chans;
      break;
    case CHAMB_1:
      itr_chan.start =0;
      itr_chan.end = (newdata.module[rdb_info->module].type == RDB36)
                     ?18:newdata.module[rdb_info->module].num_chans;
      break;
    case CHAMB_2:
      itr_chan.start =(newdata.module[rdb_info->module].type == RDB36)?
                      18:0;
      itr_chan.end = newdata.module[rdb_info->module].num_chans;
      break;
    default:
      itr_chan.start =(newdata.module[rdb_info->module].type == RDB36 && rdb_part==2)
                      ?csccmd->chan+18:csccmd->chan;
      itr_chan.end = (newdata.module[rdb_info->module].type == RDB36 &&
                      rdb_part==2)?csccmd->chan+1+18:csccmd->chan+1;
    }

  // cout << "start:" << itr_chan.start << " end:" << itr_chan.end << endl;

  int i = rdb_info->card;
  int j = rdb_info->module;
  itr_chan.len = itr_chan.end - itr_chan.start;
  realcmd.data = csccmd->data;
  realcmd.cmd = csccmd->cmd;
  realcmd.card = i;
  realcmd.module = j;
  realcmd.chan = 0;
  realcmd.size = 0;
  int res=0;
  switch (csccmd->cmd)
    {
    case HV_SET_GLOBAL_STATE: // Set System Global State
      if ((csccmd->data>=HV_STATE_OFF) && (csccmd->data<HV_STATE_SET))
        {
          GlobalState = (HV_STATE)csccmd->data;
          svc_hvGlobalState->updateService();
        }
      return 0;
    case HV_SET_MOD_STATE:

      // TODO: Set OFF module state for RDB36, only when shared part is turned off
      if ((csccmd->data == (HV_STATE)HV_STATE_OFF)
          && (newdata.module[rdb_info->module].type == RDB36)
          && !isAllChannelsAreOff(&(newdata.module[rdb_info->module]), rdb_part))
        {
          // cout << "not all channels are off" << endl;
        }
      else
        {
          res = ioctl(fd, HVCARD_PCIE_IOXCMD, &realcmd);
        }

      if (GlobalState != HV_STATE_OFF && (csccmd->data == (HV_STATE)HV_STATE_ON)) // Don't turn on master and primary if system Global state is OFF
        {
          int state = mastercmd.data;
          mastercmd.data = HV_STATE_ON;
          res = ioctl(fd, HVCARD_PCIE_IOXCMD, &mastercmd);
          mastercmd.cmd=HV_SET_STATE;
          mastercmd.data = state;
          res = ioctl(fd, HVCARD_PCIE_IOXCMD, &mastercmd);
          prime->outset = 1;
          prime->state = HV_STATE_ON;
          prime->status = HV_STATE_ON;
        }
      return 0;
    }

  if (LoadCalibration(realcmd.card,realcmd.module) && (LoadCalibration(mastercmd.card, mastercmd.module)))
    {
      HVcalibdata& data = dataservices[i].calib[j];
      HVmodule& setdata = dataservices[i].setdata->module[j];
      HVcalibdata& mdata = dataservices[mastercmd.card].calib[mastercmd.module];
      HVmodule& msetdata = dataservices[mastercmd.card].setdata->module[mastercmd.module];
      int mch = mastercmd.chan;
      int mvset = 0;
      int cur = 0;
      int v=0;

      for (int chan = 0; chan<itr_chan.len; chan++)
        {
          int k = chan + itr_chan.start;
          float val;
          switch (csccmd->cmd)
            {
            case HV_SET_RAMP_UP:
              setdata.chan[k].ramp_up = csccmd->data;
              val = csccmd->data*data.dac[k].A;
              realcmd.data = (val <0)?0:(unsigned long)val;

              msetdata.chan[mastercmd.chan].ramp_up = csccmd->data;
              val = csccmd->data*mdata.dac[mastercmd.chan].A;
              mastercmd.data = (val <0)?0:(unsigned long)val;
              ioctl(fd, HVCARD_PCIE_IOXCMD, &mastercmd);
              break;
            case HV_SET_RAMP_DOWN:
              setdata.chan[k].ramp_down = csccmd->data;
              val = csccmd->data*data.dac[k].A;
              realcmd.data = (val <0)?0:(unsigned long)val;

              msetdata.chan[mastercmd.chan].ramp_up = csccmd->data;
              val = csccmd->data*mdata.dac[mastercmd.chan].A;
              mastercmd.data = (val <0)?0:(unsigned long)val;
              ioctl(fd, HVCARD_PCIE_IOXCMD, &mastercmd);
              break;
              // case HV_SET_VMAX:
            case HV_SET_VSET:
              setdata.chan[k].vset = csccmd->data;
              val = csccmd->data*data.dac[k].A+data.dac[k].B;
              realcmd.data = (val <0)?0:(unsigned long)val;
              if (csccmd->data==0) realcmd.data = 0;
              if (data.adc[k].A>0)
                {
                  float fval = ((float)(csccmd->data)-data.adc[k].B)/data.adc[k].A;
                  int ival = (int) rint(fval);
                  realcmd.data += (ival < 0) ? 0: (unsigned long) (ival<<16);
                }

              mvset = csccmd->data+M_OFFS;
              msetdata.chan[mastercmd.chan].vset = mvset;
              val = (mvset)*mdata.dac[mch].A+mdata.dac[mch].B;
              mastercmd.data = ((csccmd->data==0) || (val<9))?0:(unsigned long)val;
              // Set master to open state (4000) if RDB vset more than 3000V
              if (csccmd->data>=3000)
                {
                  mastercmd.data = MAX_VSET;
                  mvset=4000;
                }
              if (mdata.adc[mch].A>0)
                {
                  float fval = ((float)(mvset)-mdata.adc[mch].B)/mdata.adc[mch].A;
                  int ival = (int) rint(fval);
                  mastercmd.data += (ival < 0) ? 0: (unsigned long) (ival<<16);
                }

              ioctl(fd, HVCARD_PCIE_IOXCMD, &mastercmd);
              if (prime->vset < csccmd->data+M_OFFS) prime->vset = ((csccmd->data+M_OFFS) > MAX_PRIME_VSET)? MAX_PRIME_VSET: csccmd->data+M_OFFS;
              break;
            case HV_SET_OV_LIMIT:
              setdata.chan[k].vov = csccmd->data;
              if (data.adc[k].A>0)
                {
                  float fval = csccmd->data/data.adc[k].A;
                  int ival= (int) rint(fval);
                  realcmd.data = (ival < 0) ? 0: ival;
                }
              break;
            case HV_SET_IMAX:
              setdata.chan[k].imax = csccmd->data;
              cur = csccmd->data;
              v=0;

              if (data.type == MASTER)
                {
                  csccmd->data=((csccmd->data*25)/10>data.cur[k][0])?data.cur[k][0]:csccmd->data;
                  while ((v<4000) &&
                         (data.cur[k][v+1]>(csccmd->data*25)/10) ||
                         (data.cur[k][v] == data.cur[k][v+1])) v++;
                }
              else
                {
                  csccmd->data=(csccmd->data*10>data.cur[k][0])?data.cur[k][0]:csccmd->data;
                  while ((v<4000) &&
                         (data.cur[k][v+1]>(csccmd->data*ADC_CONV_COEF)) ||
                         (data.cur[k][v] == data.cur[k][v+1])) v++;
                }
              realcmd.data = v;

              //== Override master channel current trip level with 20 ADC counts value
              mastercmd.data = 20;
              msetdata.chan[mastercmd.chan].imax = ((mdata.cur[k][mastercmd.data]-1)*10)/25;


              /*
                    v=0;
                    msetdata.chan[mastercmd.chan].imax = 1500;
                    if (cur >40)
                      {
                        mastercmd.data = 1;
                      }
                    else
                      {

                        mastercmd.data=((1500*25)/10>mdata.cur[mastercmd.chan][0])?mdata.cur[mastercmd.chan][0]:1500;
                        while ((i<=3999) &&
                               (mdata.cur[mastercmd.chan][v+1]>(mastercmd.data*25)/10) ||
                               (mdata.cur[mastercmd.chan][v] == mdata.cur[mastercmd.chan][v+1])) v++;
                        mastercmd.data = i;
                      }
              */
              ioctl(fd, HVCARD_PCIE_IOXCMD, &mastercmd);
              break;
            case HV_SET_IMAX_RAMP:
              setdata.chan[k].imax_ramp = csccmd->data;
              cur = csccmd->data;
              v=0;

              if (data.type == MASTER)
                {
                  csccmd->data=((csccmd->data*25)/10>data.cur[k][0])?data.cur[k][0]:csccmd->data;
                  while ((v<4000) &&
                         (data.cur[k][v+1]>(csccmd->data*25)/10) ||
                         (data.cur[k][v] == data.cur[k][v+1])) v++;
                }
              else
                {
                  csccmd->data=(csccmd->data*10>data.cur[k][0])?data.cur[k][0]:csccmd->data;
                  while ((v<4000) &&
                         (data.cur[k][v+1]>(csccmd->data*ADC_CONV_COEF)) ||
                         (data.cur[k][v] == data.cur[k][v+1])) v++;
                }
              realcmd.data = v;

              // msetdata.chan[mastercmd.chan].imax = 1500;
              // mastercmd.data = 1;

              mastercmd.data = 20;
              msetdata.chan[mastercmd.chan].imax_ramp = ((mdata.cur[k][mastercmd.data]-1)*10)/25;
              ioctl(fd, HVCARD_PCIE_IOXCMD, &mastercmd);
              break;
            case HV_SET_TRIP_DELAY:
              //== Override Master channel current trip delay with 60ms value
              msetdata.chan[mastercmd.chan].trip_delay = 60;
              mastercmd.data = 60;
              ioctl(fd, HVCARD_PCIE_IOXCMD, &mastercmd);
              break;



            }

          realcmd.card = i;
          realcmd.module = j;
          realcmd.chan = k;
          int res = ioctl(fd, HVCARD_PCIE_IOXCMD, &realcmd);
        }
      if (csccmd->chan>=CHAMB_1)
        {
          // Turn Off master channel for RDB36 only when all channels are set to OFF state
          switch (csccmd->cmd)
            {
            case HV_SET_STATE:
              if ( (csccmd->data == (int)HV_STATE_OFF)
                   && ((newdata.module[rdb_info->module].type == RDB36)
                       && isAllChannelsAreOff(&(newdata.module[rdb_info->module]), rdb_part))
                   || (newdata.module[rdb_info->module].type == RDB30) )
                int res = ioctl(fd, HVCARD_PCIE_IOXCMD, &mastercmd);
              break;
            case HV_SET_OFF:
              if ( ((newdata.module[rdb_info->module].type == RDB36)
                    && isAllChannelsAreOff(&(newdata.module[rdb_info->module]), rdb_part))
                   || (newdata.module[rdb_info->module].type == RDB30) )
                int res = ioctl(fd, HVCARD_PCIE_IOXCMD, &mastercmd);
              break;
            default:
              int res = ioctl(fd, HVCARD_PCIE_IOXCMD, &mastercmd);
            }

        }
    }
  else return -7;

  return 0;
}


void RpcCSCData::rpcHandler()
{

  HVcsccmdDIM *csccmdDIM=NULL ;
  HVcsccmd *csccmd =NULL;
  vector<HVcsccmd> cmds;
  cmds.clear();

  if (fAutoDetect)
    {
      // LOG4CPLUS_DEBUG (hvcmdlog,"RpcCSCData: Can not process request from " << HVDimServer::getClientName() << ": Waiting for autodetection to finish");
      // setData(csccmd, sizeof(HVcsccmd));
      return;
    }


  // std::cout << getSize() << " " << sizeof(HVcsccmdDIM) << std::endl;

  if (getSize() != sizeof(HVcsccmdDIM))
    {
      string cmdstr((char *)getData());
      LOG4CPLUS_DEBUG (hvcmdlog,rpcname << " Command request from " << HVDimServer::getClientName() << ": " << cmdstr);
      cmds = getCommands(cmdstr);
    }
  else
    {
      csccmdDIM = (HVcsccmdDIM *) getData();
      cmds.push_back(csccmdDIM->data);
    }


  fRpcProcessing = true;
  for (int i=0; i< cmds.size(); i++)
    {
      csccmd = &(cmds[i]);
      string CSC(csccmd->csc);
      HVcsc csc;
      if (!ParseCSC(CSC, &csc))
        {
          LOG4CPLUS_ERROR (hvlog, "CSC " << CSC << " can not parse name");
          // setData(csccmd, sizeof(HVcsccmd));
          continue;
        }

      CSC = csc.name;
      map<string, pair<int, int> >::iterator csc_itr = csc_map.find(CSC);
      if (csc_itr == csc_map.end() || (csc_map.size()==0))
        {
          LOG4CPLUS_ERROR (hvlog, "CSC " << CSC << " can not find mapping");
          // setData(csccmd, sizeof(HVcsccmd));
          continue;
          // return;
        }
      int rdbID = csc_itr->second.first;

      map<int, HVcmd >::iterator rdb_itr = dbs_map.find(rdbID);
      if (rdb_itr == dbs_map.end() || (dbs_map.size()==0))
        {
          LOG4CPLUS_ERROR (hvlog, "RDB " << rdbID << " can not find config info");
          // setData(csccmd, sizeof(HVcsccmd));
          continue;
          // return;

        }
      HVcmd *rdb_info = &rdb_itr->second;


      int prime_port = rdb_info->data & 0xFFFF;
      int prime_addr = (rdb_info->data >> 16)&0xFFFF;
      map<int, map<int, HVPrimary *> >::iterator prime_port_itr = primaries.find(prime_port);
      if (prime_port_itr == primaries.end() || (primaries.size()==0))
        {
          LOG4CPLUS_ERROR (hvlog, CSC << " can not find Primary PS");
          // setData(csccmd, sizeof(HVcsccmd));
          continue;
          //return;
        }
      else
        {
          map<int, HVPrimary *>::iterator prime_addr_itr = prime_port_itr->second.find(prime_addr);
          if (prime_addr_itr == prime_port_itr->second.end() || (prime_port_itr->second.size()==0))
            {
              LOG4CPLUS_ERROR (hvlog, CSC << " can not find Primary PS");
              // setData(csccmd, sizeof(HVcsccmd));
              continue;
              //return;
            }
        }


      if (rpcname == string(servername+"/HV_CSC_CMD"))
        {
          HVcmd realcmd;
          HVcmd mastercmd  = masters_map[dataservices[rdb_info->card].data->module[rdb_info->module].master_id];
          HVPrimary * prime = primaries[prime_port][prime_addr];
          mastercmd.chan = dataservices[rdb_info->card].data->module[rdb_info->module].master_chan;
          mastercmd.cmd = csccmd->cmd;
          mastercmd.data = csccmd->data;
          mastercmd.size = 0;
          int rdb_part = csc_itr->second.second;
          range itr_chan;
          switch (csccmd->chan)
            {
            case ALL:
              itr_chan.start = (dataservices[rdb_info->card].data->module[rdb_info->module].type == RDB36
                                && rdb_part==2)?18:0;
              itr_chan.end = (dataservices[rdb_info->card].data->module[rdb_info->module].type == RDB36 &&
                              rdb_part==1)?18:dataservices[rdb_info->card].data->module[rdb_info->module].num_chans;
              break;
            case CHAMB_1:
              itr_chan.start =0;
              itr_chan.end = (dataservices[rdb_info->card].data->module[rdb_info->module].type == RDB36)
                             ?18:dataservices[rdb_info->card].data->module[rdb_info->module].num_chans;
              break;
            case CHAMB_2:
              itr_chan.start =(dataservices[rdb_info->card].data->module[rdb_info->module].type == RDB36)?
                              18:0;
              itr_chan.end = dataservices[rdb_info->card].data->module[rdb_info->module].num_chans;
              break;
            default:
              itr_chan.start =(dataservices[rdb_info->card].data->module[rdb_info->module].type == RDB36 && rdb_part==2)
                              ?csccmd->chan+18:csccmd->chan;
              itr_chan.end = (dataservices[rdb_info->card].data->module[rdb_info->module].type == RDB36 &&
                              rdb_part==2)?csccmd->chan+1+18:csccmd->chan+1;
            }
          // itr_chan.start = (cmd->chan==ALL)?0:cmd->chan;
          // itr_chan.end = (cmd->chan==ALL)?dataservices[cmd->card].data->module[cmd->module].num_chans:cmd->chan+1;
          int i = rdb_info->card;
          int j = rdb_info->module;
          itr_chan.len = itr_chan.end - itr_chan.start;
          realcmd.data = csccmd->data;
          realcmd.cmd = csccmd->cmd;
          realcmd.card = i;
          realcmd.module = j;
          realcmd.chan = 0;
          realcmd.size = 0;
          int res=0;
          /*
            LOG4CPLUS_DEBUG (hvlog, "CSC:" << CSC << " RDB#"
            << rdbID << "." << rdb_part << " " << (int)realcmd.card << "."
            << (int)realcmd.module << " ch:" << itr_chan.start+1 << "-"
            << itr_chan.end << " cmd:" << hvcmd_str[realcmd.cmd]
            << "=" << realcmd.data);
          */
          // cout << "master: " << (int)mastercmd.card << "." << (int)mastercmd.module << " ch:" << (int)mastercmd.chan << " cmd:" << hvcmd_str[mastercmd.cmd] << "=" << mastercmd.data << endl;
          switch (csccmd->cmd)
            {
            case HV_SET_MOD_STATE:
              res = ioctl(fd, HVCARD_PCIE_IOXCMD, &realcmd);
              int state = mastercmd.data;
              mastercmd.data = HV_STATE_ON;
              res = ioctl(fd, HVCARD_PCIE_IOXCMD, &mastercmd);
              mastercmd.cmd=HV_SET_STATE;
              mastercmd.data = state;
              res = ioctl(fd, HVCARD_PCIE_IOXCMD, &mastercmd);
              prime->outset = 1;
              prime->state = HV_STATE_ON;
              prime->status = HV_STATE_ON;
              break;
            }
          if (LoadCalibration(realcmd.card,realcmd.module) && (LoadCalibration(mastercmd.card, mastercmd.module)))
            {
              HVcalibdata data = dataservices[i].calib[j];
              HVmodule& setdata = dataservices[i].setdata->module[j];
              HVcalibdata mdata = dataservices[mastercmd.card].calib[mastercmd.module];
              HVmodule& msetdata = dataservices[mastercmd.card].setdata->module[mastercmd.module];
              int mch = mastercmd.chan;
              int mvset = 0;
              int cur = 0;
              int v=0;

              for (int chan = 0; chan<itr_chan.len; chan++)
                {
                  int k = chan + itr_chan.start;
                  float val;
                  switch (csccmd->cmd)
                    {
                    case HV_SET_RAMP_UP:
                      setdata.chan[k].ramp_up = csccmd->data;
                      val = csccmd->data*data.dac[k].A;
                      realcmd.data = (val <0)?0:(unsigned long)val;

                      msetdata.chan[mastercmd.chan].ramp_up = csccmd->data;
                      val = csccmd->data*mdata.dac[mastercmd.chan].A;
                      mastercmd.data = (val <0)?0:(unsigned long)val;
                      break;
                    case HV_SET_RAMP_DOWN:
                      setdata.chan[k].ramp_down = csccmd->data;
                      val = csccmd->data*data.dac[k].A;
                      realcmd.data = (val <0)?0:(unsigned long)val;

                      msetdata.chan[mastercmd.chan].ramp_up = csccmd->data;
                      val = csccmd->data*mdata.dac[mastercmd.chan].A;
                      mastercmd.data = (val <0)?0:(unsigned long)val;
                      break;
                      // case HV_SET_VMAX:
                    case HV_SET_VSET:
                      setdata.chan[k].vset = csccmd->data;
                      val = csccmd->data*data.dac[k].A+data.dac[k].B;
                      realcmd.data = (val <0)?0:(unsigned long)val;
                      if (csccmd->data==0) realcmd.data = 0;
                      if (data.adc[k].A>0)
                        {
                          float fval = ((float)(csccmd->data)-data.adc[k].B)/data.adc[k].A;
                          int ival = (int) rint(fval);
                          realcmd.data += (ival < 0) ? 0: (unsigned long) (ival<<16);
                        }


                      mvset = csccmd->data+M_OFFS;
                      msetdata.chan[mastercmd.chan].vset = mvset;
                      val = (mvset)*mdata.dac[mch].A+mdata.dac[mch].B;
                      mastercmd.data = ((csccmd->data==0) || (val<9))?0:(unsigned long)val;
                      // Set master to open state (4000) if RDB vset more than 3000V
                      if (csccmd->data>=3000)
                        {
                          mastercmd.data = MAX_VSET;
                          mvset=4000;
                        }
                      if (mdata.adc[mch].A>0)
                        {
                          float fval = ((float)(mvset)-mdata.adc[mch].B)/mdata.adc[mch].A;
                          int ival = (int) rint(fval);
                          mastercmd.data += (ival < 0) ? 0: (unsigned long) (ival<<16);
                        }

                      if (prime->vset < csccmd->data+M_OFFS) prime->vset = ((csccmd->data+M_OFFS) > MAX_PRIME_VSET)? MAX_PRIME_VSET: csccmd->data+M_OFFS;
                      break;
                    case HV_SET_OV_LIMIT:
                      setdata.chan[k].vov = csccmd->data;
                      if (data.adc[k].A>0)
                        {
                          float fval = csccmd->data/data.adc[k].A;
                          int ival= (int) rint(fval);
                          realcmd.data = (ival < 0) ? 0: ival;
                        }
                      /*
                        if (mdata.adc[ch].A>0) {
                        float fval = csccmd->data/mdata.adc[k].A;
                        int ival= (int) rint(fval);
                        mastercmd.data += (ival < 0) ? 0: ival;
                        }
                      */
                      break;
                    case HV_SET_IMAX:
                      setdata.chan[k].imax = csccmd->data;
                      cur = csccmd->data;
                      v=0;

                      if (data.type == MASTER)
                        {
                          csccmd->data=((csccmd->data*25)/10>data.cur[k][0])?data.cur[k][0]:csccmd->data;
                          while ((v<4000) &&
                                 (data.cur[k][v+1]>(csccmd->data*25)/10) ||
                                 (data.cur[k][v] == data.cur[k][v+1])) v++;
                        }
                      else
                        {
                          csccmd->data=(csccmd->data*10>data.cur[k][0])?data.cur[k][0]:csccmd->data;
                          while ((v<4000) &&
                                 (data.cur[k][v+1]>(csccmd->data*ADC_CONV_COEF)) ||
                                 (data.cur[k][v] == data.cur[k][v+1])) v++;
                        }
                      realcmd.data = v;


                      //== Override master channel current trip level with 20 ADC counts value
                      mastercmd.data = 20;
                      msetdata.chan[mastercmd.chan].imax = ((mdata.cur[k][mastercmd.data]-1)*10)/25;

                      /*
                                  v=0;
                                  msetdata.chan[mastercmd.chan].imax = 1500;
                                  if (cur >40)
                                    {
                                      mastercmd.data = 1;
                                    }
                                  else
                                    {

                                      mastercmd.data=((1500*25)/10>mdata.cur[mastercmd.chan][0])?mdata.cur[mastercmd.chan][0]:1500;
                                      while ((i<=3999) &&
                                             (mdata.cur[mastercmd.chan][v+1]>(mastercmd.data*25)/10) ||
                                             (mdata.cur[mastercmd.chan][v] == mdata.cur[mastercmd.chan][v+1])) v++;
                                      mastercmd.data = i;
                                    }
                      */

                      break;

                    case HV_SET_IMAX_RAMP:
                      setdata.chan[k].imax_ramp = csccmd->data;
                      cur = csccmd->data;
                      v=0;

                      if (data.type == MASTER)
                        {
                          csccmd->data=((csccmd->data*25)/10>data.cur[k][0])?data.cur[k][0]:csccmd->data;
                          while ((v<4000) &&
                                 (data.cur[k][v+1]>(csccmd->data*25)/10) ||
                                 (data.cur[k][v] == data.cur[k][v+1])) v++;
                        }
                      else
                        {
                          csccmd->data=(csccmd->data*10>data.cur[k][0])?data.cur[k][0]:csccmd->data;
                          while ((v<4000) &&
                                 (data.cur[k][v+1]>(csccmd->data*ADC_CONV_COEF)) ||
                                 (data.cur[k][v] == data.cur[k][v+1])) v++;
                        }
                      realcmd.data = v;

                      // msetdata.chan[mastercmd.chan].imax = 1500;
                      mastercmd.data = 20;
                      msetdata.chan[mastercmd.chan].imax_ramp = ((mdata.cur[k][mastercmd.data]-1)*10)/25;

                      break;
                    case HV_SET_TRIP_DELAY:
                      //== Override Master channel current trip delay with 60ms value
                      msetdata.chan[mastercmd.chan].trip_delay = 60;
                      mastercmd.data = 60;
                      break;

                    }


                  realcmd.card = i;
                  realcmd.module = j;
                  realcmd.chan = k;
                  int res = ioctl(fd, HVCARD_PCIE_IOXCMD, &realcmd);
                }
              if (csccmd->chan>=CHAMB_1)
                {
                  int res = ioctl(fd, HVCARD_PCIE_IOXCMD, &mastercmd);
                }
            }
        }

      if (rpcname == string(servername+"/HV_CSC_DATA"))
        {
          HVcscdataDIM cscdataDIM;
          HVcscdata cscdata;
          // cout << csc.name << " " << rdbID << endl;;
          memset(&cscdata,0, sizeof(cscdata));
          memcpy(&cscdata.csc, &csc, sizeof(cscdata.csc));

          pthread_mutex_lock(&PrimeIOmutex);
          memcpy(&cscdata.primary, primaries[prime_port][prime_addr], sizeof(cscdata.primary));
          pthread_mutex_unlock(&PrimeIOmutex);

          cscdata.rdb_part = csc_itr->second.second;
          if (LoadCalibration(rdb_info->card, rdb_info->module))
            {
              HVhostcard * hvcard = dataservices[rdb_info->card].data;
              HVmodule rdb;
              HVcalibdata data = dataservices[rdb_info->card].calib[rdb_info->module];
              cscdata.hvcard.hostid = hvcard->hostid;
              cscdata.hvcard.bus = hvcard->bus;
              cscdata.hvcard.slot = hvcard->slot;
              cscdata.hvcard.dataslot = hvcard->dataslot;

              memcpy(&rdb, &hvcard->module[rdb_info->module], sizeof(rdb));
              float hvmon = rdb.hvmon*data.vmon.A +  data.vmon.B;
              rdb.hvmon = (hvmon<0)?0:(UINT)hvmon;
              for (int i=0; i< rdb.num_chans; i++)
                {
                  HVchannel ch = rdb.chan[i];
                  if (ch.status == HV_STATE_DEAD)
                    {
                      ch.vmon = 0;
                      ch.imon = 0;

                    }
                  else
                    {
                      float val = ch.vmon*data.adc[i].A;
                      val += (ch.vmon>data.adc[i].B)?data.adc[i].B:0;
                      // float val = ch.vmon*data.adc[i].A;
                      ch.vmon = (val <0)?0:(UINT)val;

                      val = ch.vset_adc * data.adc[i].A;
                      val += (ch.vset_adc>0)?data.adc[i].B:0;
                      ch.vset_adc = ( val < 0 ) ? 0 : ( UINT )val;

                      val = ch.vov * data.adc[i].A;
                      ch.vov = ( val < 0 ) ? 0 : ( UINT )val;

                      val = (ch.vtrip>0)?ch.vtrip*data.adc[i].A + data.adc[i].B:ch.vtrip;
                      // val = (ch.vtrip>0)?ch.vtrip*data.adc[i].A:ch.vtrip;
                      ch.vtrip = (val <0)?0:(UINT)val;

                      if (data.dac[i].A >0)
                        {
                          // val = (ch.vmax - data.dac[i].B)/data.dac[i].A;
                          // ch.vmax = (val <0)?0:(UINT)val;

                          val = ch.ramp_up/data.dac[i].A;
                          ch.ramp_up = (val <0)?0:(UINT)val;

                          val = ch.ramp_down/data.dac[i].A;
                          ch.ramp_down = (val <0)?0:(UINT)val;

                          val = (ch.vset - data.dac[i].B)/data.dac[i].A;
                          // ch.vset = (val <0)?0:((val>4000)?4000:(UINT)val);
                          ch.vset = (val <0)?0:(UINT)val;

                          val = (ch.vcur - data.dac[i].B)/data.dac[i].A;
                          // ch.vcur = (val <0)?0:(val>4000?4000:(UINT)val);
                          ch.vcur = (val <0)?0:(UINT)val;
                        }

                      //ch.imon = data.cur[i][ch.imon];
                      //ch.itrip = (ch.itrip>0)?data.cur[i][ch.itrip]:ch.itrip;
                      //ch.imax = data.cur[i][ch.imax];

                      if (data.type == MASTER)
                        {
                          ch.imon = ((data.cur[i][ch.imon]-1)*100)/25;
                          ch.itrip = (ch.itrip>0)?((data.cur[i][ch.itrip]-1)*10)/25:ch.itrip;
                          ch.imax = (data.cur[i][ch.imax]*10)/25;
                          ch.imax_ramp = (data.cur[i][ch.imax_ramp]*10)/25;
                          //ch.imon = (ch.imon*10)/25;
                          //ch.itrip = (ch.itrip*10)/25;
                          //ch.imax = (ch.imax*10)/25;
                        }
                      else
                        {
                          ch.imon = data.cur[i][ch.imon]-1;
                          ch.itrip = (ch.itrip>0)?(data.cur[i][ch.itrip]-1)/ADC_CONV_COEF:ch.itrip;
                          ch.imax = data.cur[i][ch.imax]/ADC_CONV_COEF;
                          ch.imax_ramp = data.cur[i][ch.imax_ramp]/ADC_CONV_COEF;
                        }
                    }
                  rdb.chan[i] = ch;
                }
              if (LoadCalibration(rdb.master_hvcard, rdb.master_busaddr))
                {
                  HVhostcard * hvcard = dataservices[rdb.master_hvcard].data;
                  HVmodule master;
                  HVcalibdata data = dataservices[rdb.master_hvcard].calib[rdb.master_busaddr];
                  memcpy(&master, &hvcard->module[rdb.master_busaddr], sizeof(master));
                  HVchannel ch = master.chan[rdb.master_chan];
                  int m_ch = rdb.master_chan;
                  if (ch.status == HV_STATE_DEAD)
                    {
                      ch.vmon = 0;
                      ch.imon = 0;

                    }
                  else
                    {
                      float val = ch.vmon*data.adc[m_ch].A;
                      //val += (ch.vmon>0)?data.adc[m_ch].B:0;
                      val += (ch.vmon>data.adc[m_ch].B)?data.adc[m_ch].B:0;
                      // float val = ch.vmon*data.adc[i].A;
                      ch.vmon = (val <0)?0:(UINT)val;

                      val = (ch.vtrip>0)?ch.vtrip*data.adc[m_ch].A + data.adc[m_ch].B:ch.vtrip;
                      // val = (ch.vtrip>0)?ch.vtrip*data.adc[i].A:ch.vtrip;
                      ch.vtrip = (val <0)?0:(UINT)val;

                      // val = (ch.vmax - data.dac[i].B)/data.dac[i].A;
                      // ch.vmax = (val <0)?0:(UINT)val;

                      val = ch.ramp_up/data.dac[m_ch].A;
                      ch.ramp_up = (val <0)?0:(UINT)val;

                      val = ch.ramp_down/data.dac[m_ch].A;
                      ch.ramp_down = (val <0)?0:(UINT)val;

                      val = (ch.vset - data.dac[m_ch].B)/data.dac[m_ch].A;
                      ch.vset = ( val < 0 ) ? 0 : ((val>MAX_VSET)?MAX_VSET:( UINT )val);

                      val = (ch.vcur - data.dac[m_ch].B)/data.dac[m_ch].A;
                      ch.vcur = ( val < 0 ) ? 0 : ((val>MAX_VSET)?MAX_VSET:( UINT )val);

                      //ch.imon = data.cur[i][ch.imon];
                      //ch.itrip = (ch.itrip>0)?data.cur[i][ch.itrip]:ch.itrip;
                      //ch.imax = data.cur[i][ch.imax];

                      if (data.type == MASTER)
                        {
                          ch.imon = ((data.cur[m_ch][ch.imon]-1)*100)/25;
                          ch.itrip = (ch.itrip>0)?((data.cur[m_ch][ch.itrip]-1)*10)/25:ch.itrip;
                          ch.imax = (data.cur[m_ch][ch.imax]*10)/25;
                          ch.imax_ramp = (data.cur[m_ch][ch.imax_ramp]*10)/25;
                          //ch.imon = (ch.imon*10)/25;
                          //ch.itrip = (ch.itrip*10)/25;
                          //ch.imax = (ch.imax*10)/25;
                        }
                      else
                        {
                          ch.imon = data.cur[m_ch][ch.imon]-1;
                          ch.itrip = (ch.itrip>0)?(data.cur[m_ch][ch.itrip]-1)/ADC_CONV_COEF:ch.itrip;
                          ch.imax = data.cur[m_ch][ch.imax]/ADC_CONV_COEF;
                          ch.imax_ramp = data.cur[m_ch][ch.imax_ramp]/ADC_CONV_COEF;
                        }
                    }
                  memcpy(&cscdata.master, &ch, sizeof(cscdata.master));
                }
              memcpy(&cscdata.rdb, &rdb, sizeof(rdb));
              cscdataDIM.ID = csccmdDIM->ID;
              memcpy(&cscdataDIM.data, &cscdata,sizeof(cscdata));
              setData((char *) &cscdataDIM, sizeof(cscdataDIM));
            }
        }
    }
  fRpcProcessing = false;
};


