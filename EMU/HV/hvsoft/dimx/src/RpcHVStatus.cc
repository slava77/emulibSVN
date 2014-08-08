#include "hvserver.h"
#include <vector>

string HVstatus_toString(HVstatus * status)
{
  stringstream st;
  string res="";
  st << status->csc << "," << status->rdb << "," << status->rdb_part << ","
  << status->chans << "," << status->chans_on << ","
  << status->vset << "," << status->vmon_min << ","
  << status->vmon_max << "," << fixed << setprecision(1) << status->imon_sum << ","
  << setprecision(1) << status->imon_min << ","
  << setprecision(1) << status->imon_max << ","
  << hvstate_str[status->state] << "," << hvstate_str[status->status]
  << ";\n";
  res = st.str();
  return res;
}


void RpcHVStatus::rpcHandler()
{
  // cout << "Received CSC cmd" << endl;
  if (getSize() == 0 )
    {
      cout << "wrong packet size " << (int)getSize() << endl;
    }

  string res="";

  if (fAutoDetect)
    {
      // LOG4CPLUS_DEBUG (hvcmdlog,"RpcHVStatus: Can not process request from " << HVDimServer::getClientName() << ": Waiting for autodetection to finish");
      setData((char *) res.c_str());
      return;
    }


  if (rpcname == string(servername+"/HV_MODULE_STATUS"))
    {
      HVcmd * module_info;
      HVhostcard * hvcard;
      HVmodule module;
      stringstream st;
      st.clear();

      for (map<int, HVcmd >::iterator masters_itr = masters_map.begin();
           masters_itr != masters_map.end(); ++masters_itr)
        {
          module_info = &masters_itr->second;
          if (LoadCalibration(module_info->card, module_info->module))
            {
              int chans_on = 0;
              int vset = 0;
              int vmon_min = 4000;
              int vmon_max = 0;
              float imon_sum = 0;
              float imon_min = 1500;
              float imon_max = 0;
              hvcard = dataservices[module_info->card].data;
              HVcalibdata data = dataservices[module_info->card].calib[module_info->module];
              memcpy(&module, &hvcard->module[module_info->module], sizeof(module));
              if (module.type == NONE) continue;

              int alarm_chans = 0;
              vector<int> ch_states = vector<int>(HV_STATE_SET+1);
              if ((module.status == HV_STATE_ILOCK) || (module.status == HV_STATE_INT_TRIP))
                alarm_chans++;
              for (int i=0; i< module.num_chans; i++)
                {
                  HVchannel ch = module.chan[i];
                  ch_states[ch.status] = ch_states[ch.status]+1;
                  if (ch.status>HV_STATE_OFF && ch.status < HV_STATE_DEAD)
                    chans_on++;

                  if (ch.status >= HV_STATE_DEAD) alarm_chans++;

                  if (ch.status == HV_STATE_DEAD)
                    {
                      ch.vmon = 0;
                      ch.imon = 0;

                    }
                  else
                    {
                      float val = ch.vmon*data.adc[i].A;
                      val += (ch.vmon>0)?data.adc[i].B:0;
                      ch.vmon = (val <0)?0:(UINT)val;

                      val = (ch.vset - data.dac[i].B)/data.dac[i].A;
                      // ch.vset = (val <0)?0:(UINT)val;
                      ch.vset = ( val < 0 ) ? 0 : ((val>4000)?4000:( UINT )val);

                      ch.imon = ( ( data.cur[i] [ch.imon] - 1 ) * 100 ) / 25;
                      ch.imax = ( data.cur[i] [ch.imax] * 10 ) / 25;
                    }
                  imon_sum += ((float)ch.imon/ADC_CONV_COEF);
                  vset += ch.vset;
                  if (ch.vmon > vmon_max)
                    vmon_max = ch.vmon;
                  if (ch.vmon < vmon_min)
                    vmon_min = ch.vmon;
                  if (((float)ch.imon/ADC_CONV_COEF) > imon_max)
                    imon_max = ((float)ch.imon)/ADC_CONV_COEF;
                  if (((float)ch.imon/ADC_CONV_COEF) < imon_min)
                    imon_min = ((float)ch.imon)/ADC_CONV_COEF;
                }
              vset /= module.num_chans;
              /*
                  if (alarm_chans == 0) {

                          for (int i=HV_STATE_OFF; i<HV_STATE_DEAD; i++) {
                                  if (ch_states[i] >= ch_states[module.status])
                                          module.status = (HV_STATE)i;
                          }
                        }
              */

              st << modtype_str[module.type] << "," << module.ID << "," << module.num_chans << ","
              << chans_on << "," << vset << "," << vmon_min << ","
              << vmon_max << "," << fixed << setprecision(1) << imon_sum << ","
              << setprecision(1) << imon_min << ","
              << setprecision(1) << imon_max << ","
              << hvstate_str[module.state] << ","
              << hvstate_str[module.status] << ";\n";
            }
        }
      for (map<int, HVcmd >::iterator dbs_itr = dbs_map.begin();
           dbs_itr != dbs_map.end(); ++dbs_itr)
        {
          module_info = &dbs_itr->second;
          if (LoadCalibration(module_info->card, module_info->module))
            {
              int chans_on = 0;
              int vset = 0;
              int vmon_min = 4000;
              int vmon_max = 0;
              float imon_sum = 0;
              float imon_min = 1500;
              float imon_max = 0;
              hvcard = dataservices[module_info->card].data;
              HVcalibdata data = dataservices[module_info->card].calib[module_info->module];
              memcpy(&module, &hvcard->module[module_info->module], sizeof(module));
              if (module.type == NONE) continue;

              int alarm_chans = 0;
              vector<int> ch_states = vector<int>(HV_STATE_SET+1);
              if ((module.status == HV_STATE_ILOCK) || (module.status == HV_STATE_INT_TRIP))
                alarm_chans++;
              for (int i=0; i< module.num_chans; i++)
                {
                  HVchannel ch = module.chan[i];
                  ch_states[ch.status] = ch_states[ch.status]+1;

                  if (ch.status>HV_STATE_OFF && ch.status < HV_STATE_DEAD)
                    chans_on++;

                  if (ch.status >= HV_STATE_DEAD) alarm_chans++;

                  if (ch.status == HV_STATE_DEAD)
                    {
                      ch.vmon = 0;
                      ch.imon = 0;

                    }
                  else
                    {
                      float val = ch.vmon*data.adc[i].A;
                      val += (ch.vmon>0)?data.adc[i].B:0;
                      ch.vmon = (val <0)?0:(UINT)val;

                      val = (ch.vset - data.dac[i].B)/data.dac[i].A;
                      ch.vset = (val <0)?0:(UINT)val;

                      ch.imon = data.cur[i][ch.imon]-1;
                      ch.imax = data.cur[i][ch.imax]/ADC_CONV_COEF;
                    }
                  imon_sum += ((float)ch.imon/ADC_CONV_COEF);
                  vset += ch.vset;
                  if (ch.vmon > vmon_max)
                    vmon_max = ch.vmon;
                  if (ch.vmon < vmon_min)
                    vmon_min = ch.vmon;
                  if (((float)ch.imon/ADC_CONV_COEF) > imon_max)
                    imon_max = ((float)ch.imon)/ADC_CONV_COEF;
                  if (((float)ch.imon/ADC_CONV_COEF) < imon_min)
                    imon_min = ((float)ch.imon)/ADC_CONV_COEF;
                }
              vset /= module.num_chans;
              if (alarm_chans == 0)
                {
                  for (int i=HV_STATE_OFF; i<HV_STATE_DEAD; i++)
                    {
                      if (ch_states[i] >= ch_states[module.status])
                        module.status = (HV_STATE)i;
                    }
                }
              st << modtype_str[module.type] << "," << module.ID << "," << module.num_chans << ","
              << chans_on << "," << vset << "," << vmon_min << ","
              << vmon_max << "," << fixed << setprecision(1) << imon_sum << ","
              << setprecision(1) << imon_min << ","
              << setprecision(1) << imon_max << ","
              << hvstate_str[module.state] << ","
              << hvstate_str[module.status] << ";\n";
            }
        }
      res = st.str();
      // cout << res  << endl;
      setData((char *) res.c_str());
    };

  if (rpcname == string(servername+"/HV_CSC_STATUS"))
    {

      for (map<string, pair<int, int> >::iterator csc_itr = csc_map.begin();
           csc_itr != csc_map.end(); ++csc_itr)
        {
          int rdbID = csc_itr->second.first;

          map<int, HVcmd >::iterator rdb_itr = dbs_map.find(rdbID);
          if (rdb_itr == dbs_map.end() || (dbs_map.size()==0))
            {
              //  LOG4CPLUS_ERROR (hvlog, "RDB " << rdbID << " can not find config info");
              // setData(csccmd, sizeof(HVcsccmd));
              // return;
              continue;
            }
          HVcmd *rdb_info = &rdb_itr->second;

          HVstatus csc_status;
          memset(&csc_status, 0, sizeof(csc_status));
          memcpy(csc_status.csc, (csc_itr->first).c_str(), 12);
          csc_status.rdb = rdbID;

          int rdb_part = csc_itr->second.second;
          csc_status.rdb_part = rdb_part;
          if (LoadCalibration(rdb_info->card, rdb_info->module))
            {
              HVhostcard * hvcard = dataservices[rdb_info->card].data;
              HVmodule rdb;
              HVcalibdata data = dataservices[rdb_info->card].calib[rdb_info->module];

              memcpy(&rdb, &hvcard->module[rdb_info->module], sizeof(rdb));
              if (rdb.type == NONE) continue;

              csc_status.chans = (rdb.type == RDB30)? 30: 18;
              csc_status.state = rdb.state;
              csc_status.status = rdb.status;
              csc_status.vmon_min = 4000;
              csc_status.imon_min = 100.;
              int alarm_chans = 0;
              vector<int> ch_states = vector<int>(HV_STATE_SET+1);
              if ((rdb.status == HV_STATE_ILOCK) || (rdb.status == HV_STATE_INT_TRIP))
                alarm_chans++;

              int ch_start = (rdb_part == 1)?0:18;
              for (int i=ch_start; i< ch_start+csc_status.chans; i++)
                {
                  HVchannel ch = rdb.chan[i];
                  ch_states[ch.status] = ch_states[ch.status]+1;
                  if (ch.status>HV_STATE_OFF && ch.status < HV_STATE_DEAD)
                    csc_status.chans_on++;

                  if (ch.status >= HV_STATE_DEAD) alarm_chans++;
                  if (ch.status == HV_STATE_DEAD)
                    {
                      ch.vmon = 0;
                      ch.imon = 0;

                    }
                  else
                    {
                      float val = ch.vmon*data.adc[i].A;
                      val += (ch.vmon>0)?data.adc[i].B:0;
                      ch.vmon = (val <0)?0:(UINT)val;

                      val = (ch.vset - data.dac[i].B)/data.dac[i].A;
                      ch.vset = (val <0)?0:(UINT)val;

                      ch.imon = data.cur[i][ch.imon]-1;
                      ch.imax = data.cur[i][ch.imax]/ADC_CONV_COEF;
                    }
                  csc_status.imon_sum += ((float)ch.imon/ADC_CONV_COEF);
                  csc_status.vset += ch.vset;
                  if (ch.vmon > csc_status.vmon_max)
                    csc_status.vmon_max = ch.vmon;
                  if (ch.vmon < csc_status.vmon_min)
                    csc_status.vmon_min = ch.vmon;
                  if (((float)ch.imon/ADC_CONV_COEF) > csc_status.imon_max)
                    csc_status.imon_max = ((float)ch.imon)/ADC_CONV_COEF;
                  if (((float)ch.imon/ADC_CONV_COEF) < csc_status.imon_min)
                    csc_status.imon_min = ((float)ch.imon)/ADC_CONV_COEF;
                }
              if (alarm_chans == 0)
                {
                  for (int i=HV_STATE_OFF; i<HV_STATE_DEAD; i++)
                    {
                      if (ch_states[i] >= ch_states[csc_status.status])
                        csc_status.status = (HV_STATE)i;
                    }
                }
              csc_status.vset /= csc_status.chans;
            }
          res += HVstatus_toString(&csc_status);
        }
      // cout << res << endl;
      setData((char *) res.c_str());
    }
};
