#include <iostream>
#include <sstream>

#include "hvserver.h"
#include "hvdb.h"

using namespace std;
using namespace mysqlpp;


int SaveRealConfig(string cfgname)
{
  std::string realcfgname = cfgname + "_REAL";
  try
    {
      LOG4CPLUS_INFO (hvlog, "HostID " << hcHostID<< ": Saving Real config as " << cfgname
                      << " for " << dataservices.size() <<  " hostcards into database");
      mysqlpp::Connection con(HV_DB_NAME, HV_DB_HOST, HV_DB_ADMIN, HV_DB_ADMIN_PASS, mysqlpp::use_exceptions);
      mysqlpp::Query query = con.query();
      query << "delete from CONFIG where CONFIG.HOSTID = " << hcHostID
      << " and CONFIG.NAME = " << mysqlpp::quote_only << realcfgname;
      query.execute();
      for (int i=0; i<dataservices.size(); i++)
        {
          HVhostcard tmpcard; // copy of hvcard data
          HVhostcard* setdata =  dataservices[i].setdata;
          memcpy((char*)(&tmpcard), dataservices[i].data,  sizeof(struct HVhostcard));
          for (int j=0; j<HVCARD_MAX_MODULES; j++)
            {
              if (tmpcard.module[j].type != NONE)
                {
                  if ( LoadCalibration( i, j ) )
                    {
                      HVcalibdata calib = dataservices[i].calib[j];
                      float adcA, adcB, dacA, dacB;
                      for (int k=0; k < tmpcard.module[j].num_chans; k++)
                        {

                          adcA = calib.adc[k].A;
                          adcB = calib.adc[k].B;
                          dacA = calib.dac[k].A;
                          dacB = calib.dac[k].B;
                          // TODO: Add calibration coefficients range checks

                          HVchannel ch = tmpcard.module[j].chan[k];

                          float val = ch.vset_adc * adcA;
                          val += (ch.vset_adc>0)?adcB:0;
                          ch.vset_adc = ( val < 0 ) ? 0 : ( UINT )val;

                          val = ch.vov * adcA;
                          ch.vov = ( val < 0 ) ? 0 : ( UINT )val;

                          // val = ( ch.vtrip > 0 ) ? ch.vtrip * adcA + adcB : ch.vtrip;
                          // ch.vtrip = ( val < 0 ) ? 0 : ( UINT )val;

                          if (dacA > 0)
                            {
                              val = ch.ramp_up / dacA;
                              ch.ramp_up = ( val < 0 ) ? 0 : ( UINT )val;

                              val = ch.ramp_down / dacA;
                              ch.ramp_down = ( val < 0 ) ? 0 : ( UINT )val;

                              val = ( ch.vset - dacB ) / dacA;
                              ch.vset = ( val < 0 ) ? 0 : ((val>4000)?4000:( UINT )val);


                              val = ( ch.vcur - dacB ) / dacA;
                              ch.vcur = ( val < 0 ) ? 0 : ((val>4000)?4000:( UINT )val);
                            }


                          if ( calib.type == MASTER )
                            {
                              // ch.imon = ( ( calib.cur[k] [ch.imon] - 1 ) * 100 ) / 25;
                              // ch.itrip = ( ch.itrip > 0 ) ? ( ( calib.cur[k] [ch.itrip] - 1 ) * 10 ) / 25 : ch.itrip;
                              ch.imax = ( calib.cur[k] [ch.imax] * 10 ) / 25;
                            }
                          else
                            {
                              // ch.imon = calib.cur[k] [ch.imon] - 1;
                              // ch.itrip = ( ch.itrip > 0 ) ? ( calib.cur[k] [ch.itrip] - 1 ) / ADC_CONV_COEF : ch.itrip;
                              ch.imax = calib.cur[k] [ch.imax] / ADC_CONV_COEF;
                            }

                          if (setdata->module[j].chan[k].ramp_up>0)
                            ch.ramp_up = setdata->module[j].chan[k].ramp_up;
                          if (setdata->module[j].chan[k].ramp_down>0)
                            ch.ramp_down = setdata->module[j].chan[k].ramp_down;

                          ch.vset = setdata->module[j].chan[k].vset;

                          if (setdata->module[j].chan[k].vov>0)
                            ch.vov = (setdata->module[j].chan[k].vov);
                          if (setdata->module[j].chan[k].ramp_up>0)
                            ch.imax = setdata->module[j].chan[k].imax;

                          tmpcard.module[j].chan[k] = ch;
                        }

                    }
                  else   // Unable to Load Calibration
                    {
                      LOG4CPLUS_ERROR (hvlog, MODULE_STR(i,j) << "  Unable to Load Calibration Data");
                    }
                }
            }
          string hvcard_data_str((char *)(&tmpcard), sizeof(struct HVhostcard));
          query.clear();
          query << "insert into CONFIG VALUES (" << mysqlpp::quote_only << realcfgname << ","
          << hcHostID << ","  << i << ",\""
          << mysqlpp::escape << hvcard_data_str << "\")" << ends;
          query.execute();
          LOG4CPLUS_DEBUG (hvlog, "Saved dataslot " << i);
        }
      for (map<int, map<int, HVPrimary*> >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr)
        {
          int port = itr->first;
          for (map<int, HVPrimary*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
            {
              int addr = h_itr->first;
              string hvprime_data_str((char *) h_itr->second, sizeof(struct HVPrimary));
              query.clear();
              query << "insert into CONFIG VALUES (" << mysqlpp::quote_only << realcfgname << ","
              << hcHostID << ","  << ((addr & 0xFF) + ((port+1)<<8)) << ",\""
              << mysqlpp::escape << hvprime_data_str << "\")" << ends;
              query.execute();
              LOG4CPLUS_DEBUG (hvlog, "Saved Primary PS " << port << ":" << addr );

            }
        }

      LOG4CPLUS_INFO (hvlog, cfgname << " config is saved") ;

    }

  catch (mysqlpp::BadQuery& er)
    {
      cerr << "Error: " << er.what() << endl;
      return -1;
    }
  catch (exception & er)
    {
      cerr << "Error: " << er.what() << endl;
      return -1;
    }
  return 0;
}
