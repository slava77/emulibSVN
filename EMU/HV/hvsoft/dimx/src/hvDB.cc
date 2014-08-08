#include <netdb.h>

#include "hvserver.h"
#include "hvdb.h"

using namespace mysqlpp;

// == MySQL DNS table
sql_create_3(DNS,
             1,3,
             int, ID,
             string, DIM_DNS_NODE,
             int, DIM_DNS_PORT)

// == MySQL HOST table
sql_create_4(HOST,
             1,4,
             int, ID,
             string, ADDRESS,
             int, PRIMARY_DNS,
             int, BACKUP_DNS)

// == MySQL PRIME table
sql_create_4(PRIME,
             1,4,
             int, ID,
             int, PORT,
             int, ADDR,
             int, HOST)

// == MySQL RDB table
sql_create_4(RDB,
             1,4,
             int, ID,
             int, HOSTCARD,
             int, ADDR,
             int, TYPE)

// == MySQL MASTER table
sql_create_4(MASTERDB,
             1,4,
             int, ID,
             int, HOSTCARD,
             int, ADDR,
             int, TYPE)

// == MySQL HVCARD table
sql_create_5(HVCARD,
             1,5,
             int, ID,
             int, HOST,
             int, BUS,
             int, SLOT,
             int, DATASLOT)

// == MySQL MAP table
sql_create_7(MAP,
             1,7,
             int, CSC,
             int, RDB,
             int, RDB_PART,
             int, MASTER,
             int, MASTER_CHAN,
             int, PRIME,
             int, HOST)


sql_create_17(CSC_DATA,
              1,17,
              int, CSC,
              DateTime, TIMESTAMP,
              int, CHAN,
              int, VMON,
              float, IMON,
              int, VSET,
              int, VCUR,
              int, RAMP_UP,
              int, RAMP_DOWN,
              int, VMAX,
              int, IMAX,
              int, VTRIP,
              int, ITRIP,
              int, TRIPCNT,
              int, TRIPDLY,
              int, STATE,
              int, STATUS)

int UpdateDataDB()
{
  try
    {
      mysqlpp::Connection con(HV_DB_NAME, HV_DB_HOST, HV_DB_ADMIN, HV_DB_ADMIN_PASS, mysqlpp::use_exceptions);
      mysqlpp::Query query = con.query();

      for (map<string, pair<int, int> >::iterator csc_itr = csc_map.begin(); csc_itr != csc_map.end(); ++csc_itr)
        {
          string CSC = csc_itr->first;
          int rdbID = csc_itr->second.first;
          int rdbPart = csc_itr->second.second;
          query.reset();
          query << "select CSC.ID from CSC where CSC.NAME = " << mysqlpp::quote_only << CSC;
          mysqlpp::StoreQueryResult csc_res = query.store();
          if (csc_res.empty())
            {
              LOG4CPLUS_ERROR (hvlog, "MySQL: Can't find entry for " << CSC << " in CSC table");
            }
          mysqlpp::Row csc_row = csc_res.at(0);
          int csc_idx = csc_row.at(0);
          query.reset();
          query << "delete from CSC_DATA where CSC_DATA.CSC = " << csc_idx;
          query.execute();
          // cout << now() << endl;
          map<int, HVcmd >::iterator rdb_itr = dbs_map.find(rdbID);
          if (rdb_itr != dbs_map.end() && (dbs_map.size()>0))
            {
              HVcmd *rdb_info = &rdb_itr->second;
              if (LoadCalibration(rdb_info->card, rdb_info->module))
                {
                  HVmodule rdb;// = dataservices[rdb_info->card].data->module[rdb_info->module];
                  memcpy(&rdb, &dataservices[rdb_info->card].data->module[rdb_info->module], sizeof(rdb));
                  int start = (rdbPart==1)?0:rdb.num_chans/2;
                  int end = (rdb.type==RDB30||(rdb.type==RDB36&&rdbPart==2))?rdb.num_chans:rdb.num_chans/2;
                  int chnum=0;
                  HVcalibdata data = dataservices[rdb_info->card].calib[rdb_info->module];

                  for (int i=start; i<end; i++)
                    {
                      chnum++;
                      HVchannel ch = rdb.chan[i];
                      if (ch.status == HV_STATE_DEAD)
                        {
                          ch.vmon = 0;
                          ch.imon = 0;

                        }
                      else
                        {
                          float val = ch.vmon*data.adc[i].A + data.adc[i].B;
                          ch.vmon = (val <0)?0:(UINT)val;

                          val = (ch.vtrip>0)?ch.vtrip*data.adc[i].A + data.adc[i].B:ch.vtrip;
                          ch.vtrip = (val <0)?0:(UINT)val;


                          val = ch.ramp_up/data.dac[i].A;
                          ch.ramp_up = (val <0)?0:(UINT)val;

                          val = ch.ramp_down/data.dac[i].A;
                          ch.ramp_down = (val <0)?0:(UINT)val;

                          val = (ch.vset - data.dac[i].B)/data.dac[i].A;
                          ch.vset = (val <0)?0:(UINT)val;

                          val = (ch.vcur - data.dac[i].B)/data.dac[i].A;
                          ch.vcur = (val <0)?0:(UINT)val;

                          ch.imon = (data.cur[i][ch.imon]-1);
                          ch.itrip = (ch.itrip>0)?(data.cur[i][ch.itrip]-1)/ADC_CONV_COEF:ch.itrip;
                          ch.imax = data.cur[i][ch.imax]/ADC_CONV_COEF;
                        }
                      // Update Database
                      query.reset();
                      CSC_DATA row(csc_idx, mysqlpp::DateTime::now(), chnum,
                                   ch.vmon, round((float)ch.imon*10/ADC_CONV_COEF)/10, ch.vset, ch.vcur,
                                   ch.ramp_up, ch.ramp_down, ch.vmax, ch.imax,
                                   ch.vtrip, ch.itrip, ch.trip_cnt, ch.trip_delay, ch.state, ch.status );
                      query.insert(row);
                      query.execute();
                    }
                }

            }
        }


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

};

int UpdateConfigDatabase(int hostid)
{
  LOG4CPLUS_INFO (hvlog, "Updating HV Configuration Database");
  try
    {
      mysqlpp::Connection con(HV_DB_NAME, HV_DB_HOST, HV_DB_ADMIN, HV_DB_ADMIN_PASS, mysqlpp::use_exceptions);
      mysqlpp::Query query = con.query();
      query << "select * from DNS where DNS.DIM_DNS_NODE = "
      << mysqlpp::quote_only << string(DimServer::getDnsNode())
      << " and DNS.DIM_DNS_PORT = " << DimServer::getDnsPort();
      mysqlpp::StoreQueryResult prime_dns_res = query.store();
      query.clear();
      query.reset();
      LOG4CPLUS_DEBUG (hvlog, "Updating DIM DNS tables");
      if (prime_dns_res.empty())
        {
          string dns_node = DimServer::getDnsNode();
          int dns_port = DimServer::getDnsPort();
          DNS row(0, dns_node, dns_port);
          query.insert(row);
          query.execute();
          query.clear();
          query.reset();
          query << "select * from DNS where DNS.DIM_DNS_NODE = "
          << mysqlpp::quote_only << string(DimServer::getDnsNode())
          << " and DNS.DIM_DNS_PORT = " << DimServer::getDnsPort();
          prime_dns_res = query.store();
          if (prime_dns_res.empty())
            {
              LOG4CPLUS_ERROR (hvlog, "MySQL: Primary DNS entry insert error");
              return -1;
            }
          // LOG4CPLUS_DEBUG (hvlog, "MySQL: Primary DNS entry updated");

        }
      mysqlpp::Row prime_dns_row = prime_dns_res.at(0);
      int prime_dns_id = prime_dns_row.at(0);

      query.clear();
      query.reset();
      query << "select * from DNS where DNS.DIM_DNS_NODE = "
      << mysqlpp::quote_only << string(BACKUP_DNS_NODE)
      << " and DNS.DIM_DNS_PORT = " << BACKUP_DNS_PORT ;
      mysqlpp::StoreQueryResult backup_dns_res = query.store();
      if (backup_dns_res.empty())
        {
          string dns_node = string(BACKUP_DNS_NODE);
          int dns_port = BACKUP_DNS_PORT;
          DNS row(0, dns_node, dns_port);
          query.insert(row);
          query.execute();
          query.clear();
          query << "select * from DNS where DNS.DIM_DNS_NODE = "
          << mysqlpp::quote_only << string(BACKUP_DNS_NODE)
          << " and DNS.DIM_DNS_PORT = " << BACKUP_DNS_PORT ;
          backup_dns_res = query.store();
          if (backup_dns_res.empty())
            {
              LOG4CPLUS_ERROR (hvlog, "MySQL: Backup DNS entry insert error");
              return -1;
            }
          // LOG4CPLUS_DEBUG (hvlog, "MySQL: Backup DNS entry updated");
        }


      mysqlpp::Row backup_dns_row = backup_dns_res.at(0);
      int backup_dns_id = backup_dns_row.at(0);

      query.clear();
      query.reset();
      LOG4CPLUS_DEBUG (hvlog, "Updating HOST table");
      query << "select * from HOST where HOST.ID = " << hostid;
      mysqlpp::StoreQueryResult host_res = query.store();
      query.clear();
      char* hostname = new char[255];
      string host_addr = string("localhost");
      if (!gethostname(hostname,255))
        {
          struct hostent * hostentry = gethostbyname(hostname);
          if (hostentry != NULL) host_addr = string(hostentry->h_name);
        }
      delete hostname;
      if (host_res.empty())
        {
          HOST row(hostid, host_addr, prime_dns_id, backup_dns_id);
          query.insert(row);
          query.execute();
        }
      else
        {
          HOST host_row = host_res.at(0);
          HOST orig_host_row = host_row;
          host_row.ADDRESS = host_addr;
          host_row.PRIMARY_DNS = prime_dns_id;
          host_row.BACKUP_DNS = backup_dns_id;
          query.update(orig_host_row, host_row);
          query.execute();
        }
      query.clear();
      query << "delete from MAP where MAP.HOST = " << hostid;
      query.execute();
      query.clear();
      query << "delete from PRIME where PRIME.HOST = " << hostid;
      query.execute();
      query.clear();
      query << "delete MASTERDB from MASTERDB,HVCARD where MASTERDB.HOSTCARD = HVCARD.ID and HVCARD.HOST=" << hostid;
      query.execute();
      query.clear();
      query << "delete RDB from RDB,HVCARD where RDB.HOSTCARD = HVCARD.ID and HVCARD.HOST = " << hostid;
      query.execute();
      query.clear();
      query << "delete from HVCARD where HVCARD.HOST = " << hostid;
      query.execute();
      query.clear();
      LOG4CPLUS_DEBUG (hvlog, "Updating PRIME table");
      for (map<int, map<int, HVPrimary*> >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr)
        {
          for (map<int, HVPrimary*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
            {
              int port= itr->first;
              int addr= h_itr->first;
              query.clear();

              PRIME row(0,port, addr, hostid);
              query.insert(row);
              query.execute();
              query.clear();

            }
        }

      LOG4CPLUS_DEBUG (hvlog, "Updating RDB and MASTERDB tables");
      for (int i=0; i<dataservices.size(); i++)
        {
          query.clear();
          int hvcard_id = dataservices[i].data->hostid+dataservices[i].data->dataslot;
          HVCARD row(hvcard_id,
                     dataservices[i].data->hostid,
                     dataservices[i].data->bus,
                     dataservices[i].data->slot,
                     dataservices[i].data->dataslot);
          query.insert(row);
          query.execute();
          for (int j=0; j<HVCARD_MAX_MODULES; j++)
            {
              HVmodule mod = dataservices[i].data->module[j];
              if (mod.type==MASTER)
                {
                  query.clear();
                  query << "delete MASTERDB, MAP from MASTERDB, MAP where MASTERDB.ID = " << mod.ID
                  << " and MAP.MASTER = " << mod.ID;
                  query.execute();
                  query.clear();
                  MASTERDB row(mod.ID, hvcard_id, j, mod.type);
                  query.insert(row);
                  query.execute();
                }
              if ((mod.type==RDB30) || (mod.type==RDB36))
                {
                  query.clear();
                  query << "delete RDB, MAP from RDB, MAP where RDB.ID = " << mod.ID
                  << " and MAP.RDB = " << mod.ID;
                  query.execute();
                  query.clear();
                  RDB row(mod.ID, hvcard_id, j, mod.type);
                  query.insert(row);
                  query.execute();
                  query.clear();
                }
            }
        }


      LOG4CPLUS_DEBUG (hvlog, "Updating MAP table");
      for (map<string, pair<int, int> >::iterator csc_itr = csc_map.begin(); csc_itr != csc_map.end(); ++csc_itr)
        {
          string CSC = csc_itr->first;
          int rdbID = csc_itr->second.first;
          int rdbPart = csc_itr->second.second;
          map<int, HVcmd >::iterator rdb_itr = dbs_map.find(rdbID);
          if (rdb_itr != dbs_map.end() && (dbs_map.size()>0))
            {
              HVcmd *rdb_info = &rdb_itr->second;
              HVmodule mod = dataservices[rdb_info->card].data->module[rdb_info->module];
              int master_id = mod.master_id;
              int master_chan = mod.master_chan;
              // == Get CSC index from CSC table
              query.clear();
              query << "select * from CSC where NAME=" << mysqlpp::quote_only << CSC;
              mysqlpp::StoreQueryResult csc_res = query.store();
              if (csc_res.empty())
                {
                  LOG4CPLUS_ERROR (hvlog, "MySQL: Can't find entry for " << CSC << " in CSC table");
                  continue;
                  // return -1;
                }
              mysqlpp::Row csc_row = csc_res.at(0);
              int csc_idx = csc_row.at(0);

              // == Delete CSC entry in MAP table
              query.clear();
              query << "delete from MAP where CSC=" << csc_idx;
              query.execute();

              // == Get Primary PS index from PRIME table

              // First check that CSC has Primary PS assigned
              if (((rdb_info->data & 0xFFFF) == 0xFFFF) && ((rdb_info->data>>16) & 0xFFFF) == 0xFFFF)
                {
                  LOG4CPLUS_ERROR (hvlog, CSC << ": Can not find Primary PS for CSC ");
                  continue;
                }
              query.clear();
              query << "select * from PRIME where PORT=" << (int)(rdb_info->data & 0xFFFF)
              <<" and ADDR = " << (int)((rdb_info->data>>16) & 0xFFFF)
              << " and HOST = " << hostid ;
              mysqlpp::StoreQueryResult prime_res = query.store();
              if (prime_res.empty())
                {
                  LOG4CPLUS_ERROR (hvlog, CSC << " MySQL: Can't find entry for Primary PS  in PRIME table");
                  continue;
                  // return -1;
                }
              mysqlpp::Row prime_row = prime_res.at(0);
              int prime_idx = prime_row.at(0);

              // == Update mapping for CSC in MAP table
              if (master_id == 0)
                {
                  LOG4CPLUS_ERROR (hvlog, CSC << "<->RDB#" << rdbID
                                   << " Invalid linked Master ID#" << master_id);
                  continue;
                }
              MAP row(csc_idx, rdbID, rdbPart, master_id, master_chan, prime_idx, hostid);
              /*
              LOG4CPLUS_DEBUG(hvlog, csc_idx << ":"<< rdbID<<":"
              << rdbPart <<":" << master_id << ":"
              <<  master_chan <<":" << prime_idx <<":"<< hostid);
              */
              query.clear();
              query.insert(row);
              query.execute();
              query.clear();

            }
          else
            {
              LOG4CPLUS_ERROR (hvlog, CSC << ": Can not find associated RDB ID#" << rdbID);
            }
        }

      map<int, HVcmd >::iterator rdb_itr;
      vector<int> unassigned_dbs;
      for (rdb_itr=dbs_map.begin(); rdb_itr != dbs_map.end(); ++rdb_itr)
        {
          int id=rdb_itr->first;
          bool fAssigned=false;
          for (map<string, pair<int, int> >::iterator csc_itr = csc_map.begin(); csc_itr != csc_map.end(); ++csc_itr)
            {
              if (csc_itr->second.first == id) fAssigned=true;
            }
          if (!fAssigned) unassigned_dbs.push_back(id);

        }
      for (int i=0; i<unassigned_dbs.size(); i++)
        {
          LOG4CPLUS_WARN (hvlog, "RDB#" << unassigned_dbs[i] << " is not assigned to any CSC");
        }

    }
  catch (mysqlpp::BadQuery& er)
    {

      cerr << "Error: " << er.what() << endl;
      // return -1;
    }
  catch (exception & er)
    {
      cerr << "Error: " << er.what() << endl;
      // return -1;
    }
  LOG4CPLUS_INFO (hvlog, "Database is Updated");

}


