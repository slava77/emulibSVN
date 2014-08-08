#include <iostream>
#include <sstream>

#include "hvserver.h"
#include "hvdb.h"

using namespace std;
using namespace mysqlpp;

int LoadRealConfig(string cfgname)
{
  LOG4CPLUS_INFO (hvlog, "Loading Real "<< cfgname << " config from database for host:" << hcHostID << " and " << dataservices.size() << " hostcards") ;
  std::string realcfgname = cfgname + "_REAL";

  try
    {
      mysqlpp::Connection con(HV_DB_NAME, HV_DB_HOST, HV_DB_USER, HV_DB_USER_PASS, mysqlpp::use_exceptions);
      mysqlpp::Query query = con.query();
      query << "select * from CONFIG where CONFIG.HOSTID = " << hcHostID
      << " and CONFIG.DATASLOT < " << 256
      << " and CONFIG.NAME = " << mysqlpp::quote_only << realcfgname;
      mysqlpp::StoreQueryResult config_res = query.store();
      if (config_res.empty())
        {
          LOG4CPLUS_WARN (hvlog, cfgname << ": No configuration data in database");
          return -1;
        }
      if (config_res.size() != dataservices.size())
        {
          LOG4CPLUS_WARN (hvlog, cfgname << ": Config inconsistency. HostCards in DB:" << config_res.size()
                          << " Actual:" << dataservices.size());
          return -1;
        }
      // std::cout << config_res.size() << std::endl;
      for (map<int, map<int, HVPrimary*> >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr)
        {
          int port = itr->first;
          for (map<int, HVPrimary*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
            {

              int addr = h_itr->first;
              HVPrimary * h = h_itr->second;

              query.clear();
              query << "select CONFIG.DATA from CONFIG where CONFIG.HOSTID = " << hcHostID
              << " and CONFIG.DATASLOT=" << ((addr & 0xFF) + ((port+1)<<8)) << " and CONFIG.NAME = "
              << mysqlpp::quote_only << realcfgname;
              UseQueryResult res = query.use();
              Row cfg_row = res.fetch_row();
              const unsigned long *size = res.fetch_lengths();
              HVPrimary cfg_data;
              if (*size != sizeof(struct HVPrimary))
                {
                  LOG4CPLUS_ERROR (hvlog, cfgname << ": Wrong config data size for Primary PS "
                                   << port << ":" << addr);
                  continue;
                }
              memcpy(&cfg_data, cfg_row[0].data(), *size);
              h->imax = cfg_data.imax;
              h->vset = cfg_data.vset;
              h->rampup = cfg_data.rampup;
              h->rampdown = cfg_data.rampdown;
              LOG4CPLUS_DEBUG (hvlog, "Loaded config for Primary PS " << port << ":" << addr);

            }
        }

      DetectModules();

      vector<HVhostcard> cfg;
      for (int i=0; i<config_res.size(); i++)
        {
          query.clear();
          query << "select CONFIG.DATA from CONFIG where CONFIG.HOSTID = " << hcHostID
          << " and CONFIG.DATASLOT=" << i << " and CONFIG.NAME = "
          << mysqlpp::quote_only << realcfgname;
          UseQueryResult res = query.use();
          Row cfg_row = res.fetch_row();
          const unsigned long *size = res.fetch_lengths();
          HVhostcard cfg_data;
          if (*size != sizeof(struct HVhostcard))
            {
              LOG4CPLUS_ERROR (hvlog, cfgname << ": Wrong config data size for dataslot " << i
                               << " " << (*size) << " " << sizeof(struct HVhostcard) );
              continue;
            }

          memcpy(&cfg_data, cfg_row[0].data(), *size);

          HVhostcard *cur_data = dataservices[i].data;
          for (int j=0; j<HVCARD_MAX_MODULES; j++)
            {
              if ((cur_data->module[j].ID != cfg_data.module[j].ID)
                  && (cur_data->module[j].type != cfg_data.module[j].type))
                {
                  LOG4CPLUS_WARN (hvlog, cfgname << ": Config inconsistency. Module at "
                                  << i << ":" << j
                                  << " DB: "    <<  modtype_str[cfg_data.module[j].type]
                                  << " ID#" << cfg_data.module[j].ID
                                  << " Actual:" <<  modtype_str[cur_data->module[j].type]
                                  << " ID#" << cur_data->module[j].ID);
                  return -1;
                }

            }
          cfg.push_back(cfg_data);
        }

      HVcmd cmd;


//		for (int i=0; i<cfg.size(); i++) {
//			for (int j=0; j<HVCARD_MAX_MODULES; j++) {
      for (int i=0; i<masters.size(); i++)
        {
          cmd = masters[i];
          HVmodule m = cfg[cmd.card].module[cmd.module];
          memcpy( (char *)(&(dataservices[cmd.card].setdata->module[cmd.module])), &m, sizeof(HVmodule));
          cmd.chan = ALL;
          cmd.cmd = HV_SET_MASTER;
          cmd.data = (m.master_chan&0xF) + ((m.master_busaddr&0xF)<<4)
                     + ((m.master_hvcard&0xFF)<<8) + ((m.master_hostid&0xFFFF)<<16);
          cmd.size = 0;
          if (((int)m.master_hvcard <0)
              || ((int)m.master_busaddr<0)) continue;
          LOG4CPLUS_DEBUG(hvlog, "Master ID#"<< m.ID << " at "
                          << (int)cmd.card << ":" << (int)cmd.module
                          << " is linked to Primary PS at "
                          << (int)m.master_busaddr << ":" << (int)m.master_hvcard);
          ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
          cmd.cmd = HV_SET_ILOCK_DELAY;
          cmd.data = m.ilock_delay;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
          cmd.cmd = HV_SET_MASTER_TRIP_DELAY;
          cmd.data = m.master_trip_delay;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
          cmd.cmd = HV_SET_MOD_POLICY;
          cmd.data = m.policy;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
          if ( LoadCalibration( cmd.card, cmd.module ) )
            {
              HVcalibdata calib = dataservices[cmd.card].calib[cmd.module];
              float adcA, adcB, dacA, dacB;
              for (int i = 0; i < m.num_chans; i++)
                {

                  adcA = calib.adc[i].A;
                  adcB = calib.adc[i].B;
                  dacA = calib.dac[i].A;
                  dacB = calib.dac[i].B;
                  cmd.chan = i;
                  cmd.size = 0;
                  float val;

                  // == Set channel Ramp Up
                  cmd.cmd = HV_SET_RAMP_UP;
                  val =  m.chan[i].ramp_up * dacA;
                  cmd.data = ( val < 0 ) ? 0 : ( unsigned long ) val;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel Ramp Down
                  cmd.cmd = HV_SET_RAMP_DOWN;
                  val =  m.chan[i].ramp_down * dacA;
                  cmd.data = ( val < 0 ) ? 0 : ( unsigned long ) val;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel OV Limit
                  cmd.cmd = HV_SET_OV_LIMIT;
                  if (adcA>0)
                    {
                      float fval =  m.chan[i].vov/adcA;
                      int ival= (int) rint(fval);
                      cmd.data = (ival < 0) ? 0: ival;
                    }

                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel OV Trip Delay
                  cmd.cmd = HV_SET_OV_TRIP_DELAY;
                  cmd.data = m.chan[i].vtrip_delay;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel Vset
                  cmd.cmd = HV_SET_VSET;
                  val = m.chan[i].vset * dacA + dacB;
                  cmd.data = ( val < 0 ) ? 0 : ( unsigned long ) val;
                  if (m.chan[i].vset==0) cmd.data = 0;
                  int ival = 4000;
                  if (adcA>0)
                    {
                      float fval = ((float)(m.chan[i].vset)-adcB)/adcA;
                      ival = (int) rint(fval);
                    }
                  cmd.data += (ival <= 0) ? 20: (unsigned long) (ival<<16);

                  // cmd.data = m.chan[i].vset+ ((m.chan[i].vset_adc)<<16);
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel Imax
                  cmd.cmd = HV_SET_IMAX;
                  int v = 0;
                  int imax = m.chan[i].imax;
                  if ( calib.type == MASTER )
                    {
                      imax = ( ( imax * 25 ) / 10 > calib.cur[i] [0] ) ? calib.cur[i] [0] : imax;
                      while ( ( v <= 3999 ) && ( calib.cur[i] [v + 1] > ( imax * 25 ) / 10 )
                              || ( calib.cur[i] [v] == calib.cur[i] [v + 1] ) )v++;
                    }
                  else
                    {
                      imax = ( imax * 10 > calib.cur[i] [0] ) ? calib.cur[i] [0] : imax;
                      while ( ( v <= 3999 ) && ( calib.cur[i] [v + 1] > ( imax * ADC_CONV_COEF ) )
                              || ( calib.cur[i] [v] == calib.cur[i] [v + 1] ) )v++;
                    }
                  cmd.data = v;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel ImaxRamp
                  cmd.cmd = HV_SET_IMAX_RAMP;
                  v = 0;
                  int imax_ramp = m.chan[i].imax_ramp;
                  if ( calib.type == MASTER )
                    {
                      imax_ramp = ( ( imax_ramp * 25 ) / 10 > calib.cur[i] [0] ) ? calib.cur[i] [0] : imax_ramp;
                      while ( ( v <= 3999 ) && ( calib.cur[i] [v + 1] > ( imax_ramp * 25 ) / 10 )
                              || ( calib.cur[i] [v] == calib.cur[i] [v + 1] ) )v++;
                    }
                  else
                    {
                      imax_ramp = ( imax_ramp * 10 > calib.cur[i] [0] ) ? calib.cur[i] [0] : imax_ramp;
                      while ( ( v <= 3999 ) && ( calib.cur[i] [v + 1] > ( imax_ramp * ADC_CONV_COEF ) )
                              || ( calib.cur[i] [v] == calib.cur[i] [v + 1] ) )v++;
                    }
                  cmd.data = v;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel Vmax
                  cmd.cmd = HV_SET_VMAX;
                  cmd.data = m.chan[i].vmax;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel Trip Delay
                  cmd.cmd = HV_SET_TRIP_DELAY;
                  cmd.data = m.chan[i].trip_delay;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel Trip Reset Delay
                  cmd.cmd = HV_SET_TRIP_RESET_DELAY;
                  cmd.data = m.chan[i].trip_reset_delay;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel Max Trip Reset attempts
                  cmd.cmd = HV_SET_TRIP_MAX_RESETS;
                  cmd.data = m.chan[i].trip_max_resets;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
                }
            }
          else
            {
              LOG4CPLUS_WARN (hvlog, MODULE_STR(cmd.card,cmd.module) << "  Unable to Load Calibration Data");
            }
        }


      for (int i=0; i<dbs.size(); i++)
        {
          cmd = dbs[i];
          HVmodule m = cfg[cmd.card].module[cmd.module];
          memcpy( (char *)(&(dataservices[cmd.card].setdata->module[cmd.module])), &m, sizeof(HVmodule));
          int ID = m.ID;
          cmd.chan = ALL;
          cmd.cmd = HV_SET_MASTER;
          cmd.data = (m.master_chan&0xF) + ((m.master_busaddr&0xF)<<4)
                     + ((m.master_hvcard&0xFF)<<8) + ((m.master_hostid&0xFFFF)<<16);
          cmd.size = 0;
          if (((int)m.master_id <= 0)
              || ((int)m.master_hvcard <0)
              || ((int)m.master_busaddr<0)) continue;
          LOG4CPLUS_DEBUG(hvlog, modtype_str[m.type]<< " ID#"<< m.ID << " at "
                          << (int)cmd.card << ":" << (int)cmd.module
                          << " is linked to Master ID#"
                          << (int)m.master_id  << " at " << (int)m.master_hvcard << ":" << (int)m.master_busaddr
                          << " Ch" << (int)m.master_chan+1);
          ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
          HVmodule master = cfg[m.master_hvcard].module[m.master_busaddr];
          dbs[i].data = master.master_busaddr + (master.master_hvcard<<16);
          dbs_map[ID].data = master.master_busaddr + (master.master_hvcard<<16);

          ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
          cmd.cmd = HV_SET_ILOCK_DELAY;
          cmd.data = m.ilock_delay;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
          cmd.cmd = HV_SET_MASTER_TRIP_DELAY;
          cmd.data = m.master_trip_delay;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
          cmd.cmd = HV_SET_MOD_POLICY;
          cmd.data = m.policy;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
          if ( LoadCalibration( cmd.card, cmd.module ) )
            {
              HVcalibdata calib = dataservices[cmd.card].calib[cmd.module];
              float adcA, adcB, dacA, dacB;
              for (int i = 0; i < m.num_chans; i++)
                {
                  adcA = calib.adc[i].A;
                  adcB = calib.adc[i].B;
                  dacA = calib.dac[i].A;
                  dacB = calib.dac[i].B;
                  cmd.chan = i;
                  cmd.size = 0;
                  float val;

                  // == Set channel Ramp Up
                  cmd.cmd = HV_SET_RAMP_UP;
                  val =  m.chan[i].ramp_up * dacA;
                  cmd.data = ( val < 0 ) ? 0 : ( unsigned long ) val;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel Ramp Down
                  cmd.cmd = HV_SET_RAMP_DOWN;
                  val =  m.chan[i].ramp_down * dacA;
                  cmd.data = ( val < 0 ) ? 0 : ( unsigned long ) val;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel OV Limit
                  cmd.cmd = HV_SET_OV_LIMIT;
                  if (adcA>0)
                    {
                      float fval =  m.chan[i].vov/adcA;
                      int ival= (int) rint(fval);
                      cmd.data = (ival < 0) ? 0: ival;
                    }

                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel OV Trip Delay
                  cmd.cmd = HV_SET_OV_TRIP_DELAY;
                  cmd.data = m.chan[i].vtrip_delay;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel Vset
                  cmd.cmd = HV_SET_VSET;
                  val = m.chan[i].vset * dacA + dacB;
                  cmd.data = ( val < 0 ) ? 0 : ( unsigned long ) val;
                  if (m.chan[i].vset==0) cmd.data = 0;
                  int ival = 4000;
                  if (adcA>0)
                    {
                      float fval = ((float)(m.chan[i].vset)-adcB)/adcA;
                      ival = (int) rint(fval);
                    }
                  cmd.data += (ival < 0) ? 0: (unsigned long) (ival<<16);

                  // cmd.data = m.chan[i].vset+ ((m.chan[i].vset_adc)<<16);
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel Imax
                  cmd.cmd = HV_SET_IMAX;
                  int v = 0;
                  int imax = m.chan[i].imax;
                  if ( calib.type == MASTER )
                    {
                      imax = ( ( imax * 25 ) / 10 > calib.cur[i] [0] ) ? calib.cur[i] [0] : imax;
                      while ( ( v <= 3999 ) && ( calib.cur[i] [v + 1] > ( imax * 25 ) / 10 )
                              || ( calib.cur[i] [v] == calib.cur[i] [v + 1] ) )v++;
                    }
                  else
                    {
                      imax = ( imax * 10 > calib.cur[i] [0] ) ? calib.cur[i] [0] : imax;
                      while ( ( v <= 3999 ) && ( calib.cur[i] [v + 1] > ( imax * ADC_CONV_COEF ) )
                              || ( calib.cur[i] [v] == calib.cur[i] [v + 1] ) )v++;
                    }
                  cmd.data = v;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel ImaxRamp
                  cmd.cmd = HV_SET_IMAX_RAMP;
                  v = 0;
                  int imax_ramp = m.chan[i].imax_ramp;
                  if ( calib.type == MASTER )
                    {
                      imax_ramp = ( ( imax_ramp * 25 ) / 10 > calib.cur[i] [0] ) ? calib.cur[i] [0] : imax_ramp;
                      while ( ( v <= 3999 ) && ( calib.cur[i] [v + 1] > ( imax_ramp * 25 ) / 10 )
                              || ( calib.cur[i] [v] == calib.cur[i] [v + 1] ) )v++;
                    }
                  else
                    {
                      imax_ramp = ( imax_ramp * 10 > calib.cur[i] [0] ) ? calib.cur[i] [0] : imax_ramp;
                      while ( ( v <= 3999 ) && ( calib.cur[i] [v + 1] > ( imax_ramp * ADC_CONV_COEF ) )
                              || ( calib.cur[i] [v] == calib.cur[i] [v + 1] ) )v++;
                    }
                  cmd.data = v;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);

                  // == Set channel Vmax
                  cmd.cmd = HV_SET_VMAX;
                  cmd.data = m.chan[i].vmax;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
                  // == Set channel Trip Delay
                  cmd.cmd = HV_SET_TRIP_DELAY;
                  cmd.data = m.chan[i].trip_delay;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
                  // == Set channel Trip Reset Delay
                  cmd.cmd = HV_SET_TRIP_RESET_DELAY;
                  cmd.data = m.chan[i].trip_reset_delay;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
                  // == Set channel Max Trip Reset attempts
                  cmd.cmd = HV_SET_TRIP_MAX_RESETS;
                  cmd.data = m.chan[i].trip_max_resets;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &cmd);
                }
            }
          else
            {
              LOG4CPLUS_WARN (hvlog, MODULE_STR(cmd.card,cmd.module) << "  Unable to Load Calibration Data");
            }
        }

      LOG4CPLUS_INFO (hvlog, cfgname << " config is loaded") ;

    }
  catch (mysqlpp::BadQuery& er)
    {
      cerr << "Query Error: " << er.what() << endl;
      return -1;
    }
  catch (exception & er)
    {
      cerr << "Error: " << er.what() << endl;
      return -1;
    }
  return 0;
}
