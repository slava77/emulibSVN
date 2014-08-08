#include <iostream>
#include <sstream>

#include "hvserver.h"
#include "hvdb.h"

using namespace std;
using namespace mysqlpp;

int SaveConfig(string cfgname)
{
  try
    {
      LOG4CPLUS_INFO (hvlog, "HostID " << hcHostID<< ": Saving config as " << cfgname
                      << " for " << dataservices.size() <<  " hostcards into database");
      mysqlpp::Connection con(HV_DB_NAME, HV_DB_HOST, HV_DB_ADMIN, HV_DB_ADMIN_PASS, mysqlpp::use_exceptions);
      mysqlpp::Query query = con.query();
      query << "delete from CONFIG where CONFIG.HOSTID = " << hcHostID
      << " and CONFIG.NAME = " << mysqlpp::quote_only << cfgname;
      query.execute();
      for (int i=0; i<dataservices.size(); i++)
        {
          string hvcard_data_str((char *)dataservices[i].data, sizeof(struct HVhostcard));
          query.clear();
          query << "insert into CONFIG VALUES (" << mysqlpp::quote_only << cfgname << ","
          << hcHostID << ","  << i << ",\""
          << mysqlpp::escape << hvcard_data_str << "\")" << ends;
          query.execute();
          LOG4CPLUS_DEBUG (hvlog,	"Saved dataslot " << i);
        }
      for (map<int, map<int, HVPrimary*> >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr)
        {
          int port = itr->first;
          for (map<int, HVPrimary*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
            {
              int addr = h_itr->first;
              string hvprime_data_str((char *) h_itr->second, sizeof(struct HVPrimary));
              query.clear();
              query << "insert into CONFIG VALUES (" << mysqlpp::quote_only << cfgname << ","
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

