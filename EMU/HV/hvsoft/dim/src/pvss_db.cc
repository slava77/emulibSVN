#include <iostream>
#include <sstream>
#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h>
#include "hvdb.h"
#include "pvss_db.h"

using namespace mysqlpp;
using namespace std;

int fill_pvss_databases(int hostid)
{
  stringstream st;
  string res_str="";

  // Clear lists
  primary_database.clear();
  primary_database_2.clear();
  card_database.clear();
  module_database.clear();

  // host_database="200";
  char hostid_str[20];
  sprintf(hostid_str,"%d",hostid);
  host_database= string(hostid_str);
  try
    {
      mysqlpp::Connection con(HV_DB_NAME, HV_DB_HOST, HV_DB_USER, HV_DB_USER_PASS, mysqlpp::use_exceptions);
      mysqlpp::Query query = con.query();

      // TODO: There are could be more than one Primary PS
      //-----------------------------------------------------------------------------------
      // Fill Host Cards database
      // TODO: card should be addressed as PCI bus:slot pair, not only PCI slot
      query.reset();
      //                query << "select HVCARD.DATASLOT from HVCARD where HVCARD.HOST = " << hostid;
      query << "select PRIME.PORT, PRIME.ADDR from PRIME where PRIME.HOST=" << hostid << " order by PRIME.PORT";
      mysqlpp::StoreQueryResult prime_res = query.store();
      if (prime_res.empty())
        {
          cerr << "MySQL: Can't find entries for HostID=" << hostid << " in PRIME table" << endl;
        }
      mysqlpp::Row row;
      mysqlpp::StoreQueryResult::iterator i;
      for (i = prime_res.begin(); i!=prime_res.end(); i++)
        {
          row = *i;
          cout << "primary port " << row.at(0) <<  " primary address" << row.at(1) << endl;
          primary_database.push_back(row.at(0));
          primary_database_2.push_back(row.at(1));
        }
      //	        primary_database.push_back(1); //hardcode
      //------------------------------------------------------------------------------------
      //------------------------------------------------------------------------------------
      // Fill Host Cards database
      // TODO: card should be addressed as PCI bus:slot pair, not only PCI slot
      query.reset();
      query << "select HVCARD.DATASLOT from HVCARD where HVCARD.HOST = " << hostid;
      mysqlpp::StoreQueryResult card_res = query.store();
      if (card_res.empty())
        {
          cerr << "MySQL: Can't find entries for HostID=" << hostid << " in HVCARD table" << endl;
        }
      //		mysqlpp::Row row;
      //		mysqlpp::StoreQueryResult::iterator i;
      for (i = card_res.begin(); i!=card_res.end(); i++)
        {
          row = *i;
          cout << "hostcard " << row.at(0) << endl;
          card_database.push_back(row.at(0));
        }
      /*
                  card_database.push_back(12); // card 0 !!!
                  card_database.push_back(14); // card 1 !!!
                  card_database.push_back(16); // card 2 !!!
                  card_database.push_back(18); // card 3 !!!
                  */
      //-------------------------------------------------------------------------------------
      // Fill Masters database
      query.reset();
      query << "select HVCARD.DATASLOT,MASTERDB.ADDR from MASTERDB, HVCARD where MASTERDB.HOSTCARD = HVCARD.ID and HVCARD.HOST=" << hostid;
      mysqlpp::StoreQueryResult master_res = query.store();
      if (master_res.empty())
        {
          cerr << "MySQL: Can't find entries for HostID=" << hostid << " in MASTERDB table" << endl;
        }
      for (i = master_res.begin(); i!=master_res.end(); i++)
        {
          row = *i;
          st.clear();
          res_str.clear();
          st << ";" << row.at(0) << ";" << row.at(1);
          st >> res_str;
          cout << "master " << res_str << endl;
          module_database.push_back(res_str);
        }

      // masters go first
      /*
            module_database.push_back(";12;0"); //6   // any order !!!
          	module_database.push_back(";12;1"); //14
            module_database.push_back(";12;2"); //11
            module_database.push_back(";12;3"); //17
            module_database.push_back(";12;4"); //7
            module_database.push_back(";12;5"); //16
      */

      number_of_masters=module_database.size();

      // Fill RDBs database
      query.reset();
      query << "select HVCARD.DATASLOT,RDB.ADDR from RDB, HVCARD where RDB.HOSTCARD = HVCARD.ID and HVCARD.HOST=" << hostid;
      mysqlpp::StoreQueryResult rdb_res = query.store();
      if (rdb_res.empty())
        {
          cerr << "MySQL: Can't find entries for HostID=" << hostid << " in RDB table" << endl;
        }
      for (i = rdb_res.begin(); i!=rdb_res.end(); i++)
        {
          row = *i;
          st.clear();
          res_str.clear();
          st << ";" << row.at(0) << ";" << row.at(1);
          st >> res_str;
          cout << "rdb "<< res_str << endl;
          module_database.push_back(res_str);
        }

      /*
            module_database.push_back(";14;0"); //34 ME+3/1/07 ME+2/1/18 // any order !!!
            module_database.push_back(";14;1"); //36 ME+2/1/03 ME+2/1/02
            module_database.push_back(";14;2"); //39 ME+3/1/01 ME+3/1/17
            module_database.push_back(";14;3"); //30 ME+2/1/17 ME+2/1/01
            module_database.push_back(";14;4"); //25 ME+3/1/18 ME+3/1/02
      //        module_database.push_back(";14;5"); //44
      //        module_database.push_back(";14;6"); //24
            module_database.push_back(";14;7"); //28    ME+2/1/06 ME+2/1/07
            module_database.push_back(";14;8"); //41 // ME+3/1/04 ME+3/1/06
            module_database.push_back(";14;9"); //308 ME+2/2/06 **
            module_database.push_back(";14;10"); //306 ME+2/2/12  **
            module_database.push_back(";14;11"); //300 ME+2/2/10   **
            module_database.push_back(";14;12"); //275 // ME+2/2/08 **
            module_database.push_back(";14;13"); //277 ME+2/2/07   **
            module_database.push_back(";14;14"); //303 ME+2/2/13  **
            module_database.push_back(";14;15"); //301 ME+2/2/09 **

            module_database.push_back(";16;0"); //279 ME+2/2/11   **
      //        module_database.push_back(";16;1"); //282 ME+2/2/14 -------- **
      //        module_database.push_back(";16;2"); //312
      //        module_database.push_back(";16;3"); //302
      //        module_database.push_back(";16;4"); //258
      //        module_database.push_back(";16;5"); //278
      //        module_database.push_back(";16;6"); //298
      //        module_database.push_back(";16;7"); //286
      //        module_database.push_back(";16;8"); //289
      //        module_database.push_back(";16;9"); //294
      //        module_database.push_back(";16;10"); //284
            module_database.push_back(";16;11"); //313 // ME+2/2/03 **
            module_database.push_back(";16;12"); //310 ME+2/2/33  **
            module_database.push_back(";16;13"); //285  //ME+2/2/01 **
            module_database.push_back(";16;14"); //315  //ME+2/2/05 **
            module_database.push_back(";16;15"); //259 ME+2/2/35 **


      //        module_database.push_back(";18;0"); //263
      //        module_database.push_back(";18;1"); //265
            module_database.push_back(";18;2"); //261 ME+2/2/34 **
            module_database.push_back(";18;3"); //314 ME+2/2/02 **
            module_database.push_back(";18;4"); //287 ME+2/2/04 **
            module_database.push_back(";18;5"); //304 ME+2/2/36 **
      //        module_database.push_back(";18;6"); //
      //        module_database.push_back(";18;7"); //
      //        module_database.push_back(";18;8"); //
      //        module_database.push_back(";18;9"); //290
      //        module_database.push_back(";18;10"); //297
      //        module_database.push_back(";18;11"); //299
      //        module_database.push_back(";18;12"); //280
      //        module_database.push_back(";18;13"); //274
      //        module_database.push_back(";18;14"); //288
      //        module_database.push_back(";18;15"); //296
      */

    }
  catch (mysqlpp::BadQuery& er)
    {
#ifdef USE_STANDARD_EXCEPTION
      cerr << "Error: " << er.what() << endl;
#endif
      return -1;
    }
#ifdef USE_STANDARD_EXCEPTION
  catch (exception & er)
    {
      cerr << "Error: " << er.what() << endl;
      return -1;
    }
#endif

  return 1;

}
