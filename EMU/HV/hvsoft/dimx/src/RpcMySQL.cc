#include "hvserver.h"
#include "hvdb.h"

void RpcMySQL::rpcHandler()
{
  string query_str = string(getString());
  string res_str = "";
  stringstream st;
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
      for (i = res.begin(); i!=res.end(); i++)
        {

          row = *i;
          for (int j=0; j<row.size(); j++)
            st << row[j] << string((j<(row.size()-1))?",":";");
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
  st >> res_str;
  // cout << res_str << endl;
  setData((char *)res_str.c_str());
};

