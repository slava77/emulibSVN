#include <iostream>
#include <fstream>
#include "hvserver.h"
#include "hvdb.h"

int main()
{
  string outfile="hvmap.csv";
//        string query_str = "select CSC.NAME, MAP.RDB, MAP.RDB_PART, HVCARD.DATASLOT, RDB.ADDR, MAP.MASTER, MHVCARD.DATASLOT, MASTERDB.ADDR, MAP.MASTER_CHAN from MAP, CSC, RDB, MASTERDB, HVCARD, HVCARD as MHVCARD where ((MAP.CSC=CSC.ID) and (MAP.RDB=RDB.ID) and (MAP.MASTER=MASTERDB.ID) and (RDB.HOSTCARD=HVCARD.ID) and (MASTERDB.HOSTCARD=MHVCARD.ID));";
  string query_str = "select CSC.NAME, MAP.RDB, MAP.RDB_PART, HVCARD.DATASLOT, RDB.ADDR, MAP.MASTER, MHVCARD.DATASLOT, MASTERDB.ADDR, MAP.MASTER_CHAN, PRIME.PORT, PRIME.ADDR from MAP, CSC, RDB, MASTERDB, PRIME, HVCARD, HVCARD as MHVCARD where ((MAP.CSC=CSC.ID) and (MAP.RDB=RDB.ID) and (MAP.MASTER=MASTERDB.ID) and (RDB.HOSTCARD=HVCARD.ID) and (MASTERDB.HOSTCARD=MHVCARD.ID) and (MAP.PRIME=PRIME.ID));";
  string res_str = "";
  ofstream st(outfile.c_str());
  cout << "Save Generated HV Map to " << outfile << endl;
  // cout << getSize() << ":" << query_str << endl;

  // ==> Test Section
  try
    {
      mysqlpp::Connection con(HV_DB_NAME, HV_DB_HOST, HV_DB_USER, HV_DB_USER_PASS, mysqlpp::use_exceptions);
      mysqlpp::Query query = con.query();
      // query << "SELECT * FROM HVPRIME WHERE HVPRIME.HOSTID = " << hcHostID << endl;
      query << query_str << endl;
      mysqlpp::StoreQueryResult res = query.store();
      // cout << "Query: " << query.preview() << endl;
      // cout << "Records Found: " << res.size() << endl;
      mysqlpp::Row row;
      mysqlpp::StoreQueryResult::iterator i;
      st.clear();
      st << "Chamber,DB,DB output,DB host,DB address,MB,MB host,MB address,MB channel, Primary Port, Primary Addr" << endl;
      for (i = res.begin(); i!=res.end(); i++)
        {

          row = *i;
          for (int j=0; j<row.size(); j++)
            st << row[j] << string((j<(row.size()-1))?",":"\n");
          // cout << setw(8) << row[0] << setw(8) << row[1] << setw(8)<< row[3] << endl;
          // cout << st.str() << endl;
        }
    }
  catch (mysqlpp::BadQuery& er)
    {
      st.clear();
      st << "Error: " << er.what() << endl;
      cerr << "Error: " << er.what() << endl;

      // return;
    }
  catch (exception & er)
    {
      st.clear();
      st << "Error: " << er.what() << endl;
      cerr << "Error: " << er.what() << endl;
      //return;
    }
  // <== End of Test Section
  st.close();
  return 0;
};

