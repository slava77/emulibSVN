#include <iostream>
#include <iomanip>

#include "hvserver.h"
#include "hvdb.h"

#define RDB_CUR 9
#define RDB_ADC 10
#define RDB_DAC 11

#define MASTER_CUR 14
#define MASTER_ADC 15
#define MASTER_DAC 16

using namespace std;
using namespace mysqlpp;


// Load Voltage ADC calibration data from file
bool LoadADCCalibration(int card, int module)
{
  if (card>=dataservices.size() || module >= HVCARD_MAX_MODULES) return false;
  HVcalibdata& data = dataservices[card].calib[module];
  data.card = card;
  data.addr = module;
  data.type = dataservices[data.card].data->module[data.addr].type;
  data.id = dataservices[data.card].data->module[data.addr].ID;
  if ((data.id<=0) || (data.id>=511))	return false;

  stringstream st;
  char line[255];
  string modname = "";
  string adcfname = "";

  switch (data.type)
    {
    case RDB30 :
    case RDB36 :

      st << HV_DATADIR << "/Remote_" << setw(3) << setfill('0') << data.id << "/"
      << "T" << setw(2) << setfill('0') << RDB_ADC << "_M" << setw(3) << setfill('0') << data.id
      << ".log";
      st >> adcfname;

      break;
    case MASTER :
      st << HV_DATADIR << "/Master_" << setw(3) << setfill('0') << data.id << "/"
      << "T" << setw(2) << setfill('0') << MASTER_ADC << "_M" << setw(3) << setfill('0') << data.id
      << ".log";
      st >> adcfname;
      break;
    case NONE :
      return true;
    }

  vector<float> A(36), B(36);
  bool fA=false, fB=false;
  int i=0;
  A.clear();
  B.clear();

  ifstream adcfile(adcfname.c_str());
  if (adcfile)
    {
      LOG4CPLUS_DEBUG (hvlog, "Load Voltage ADC Calibration from " << adcfname);
      while (!adcfile.eof())
        {
          adcfile.getline(line,255);
          if (string(line).find("Limits") ==0)
            {
              adcfile.getline(line,255);
            }

          if (fA)
            {
              int cnt=0;

              string str(line);
              replace(str.begin(), str.end(), ',', ' ');
              stringstream st(str);

              float t;
              while (st >> t )
                {
                  A.push_back(t);
                  //cout << A[i]<<":";
                  i++;
                  cnt++;
                }
              //cout << endl;
              if (cnt==0) fA=false;
            }
          if (fB)
            {
              int cnt=0;
              string str(line);
              replace(str.begin(), str.end(), ',', ' ');
              stringstream st(str);
              float t;
              while (st >> t )
                {
                  B.push_back(t);
                  //   cout << B[i]<<":";
                  i++;
                  cnt++;
                }
              // cout << endl;
              if (cnt==0) fB=false;
            }
          if (string(line).find("A:") == 0)
            {
              i=0;
              fB=false;
              fA=true;
              A.clear();
            }
          if (string(line).find("B:") ==0)
            {
              i=0;
              fA=false;
              fB=true;
              B.clear();
            }

          string str = string(line);
          int pos = str.find("A=", 0);
          if (pos != string::npos)
            {
              stringstream st(str.substr(pos+2, str.size()));
              st >> data.vmon.A;
              // cout << data.vmon.A << endl;
            }
          pos = str.find("B=", 0);
          if ( pos != string::npos)
            {
              stringstream stB(str.substr(pos+2, str.size()));
              stB >> data.vmon.B;
              // cout << data.vmon.B << endl;
            }

        }
      adcfile.close();
    }
  else
    {
      LOG4CPLUS_ERROR (hvlog, "Unable to open "<< adcfname);
      return false;
    }

  if (A.size() && B.size() && (A.size()==B.size()))
    {
      HVcalib c;
      for (int i=0; i< A.size(); i++)
        {
          data.adc[i].A = A[i];
          data.adc[i].B = B[i];
        }
    }
  return true;

}


// Load Voltage DAC calibration data from file
bool LoadDACCalibration(int card, int module)
{
  if (card>=dataservices.size() || module >= HVCARD_MAX_MODULES) return false;
  HVcalibdata& data = dataservices[card].calib[module];
  data.card = card;
  data.addr = module;
  data.type = dataservices[data.card].data->module[data.addr].type;
  data.id = dataservices[data.card].data->module[data.addr].ID;
  if ((data.id<=0) || (data.id>=511))	return false;

  stringstream st;
  char line[255];
  string modname = "";
  string dacfname = "";

  switch (data.type)
    {
    case RDB30 :
    case RDB36 :
      st << HV_DATADIR << "/Remote_" << setw(3) << setfill('0') << data.id << "/"
      << "T" << setw(2) << setfill('0') << RDB_DAC << "_M" << setw(3) << setfill('0') << data.id
      << ".log";
      st >> dacfname;

      break;
    case MASTER :
      st << HV_DATADIR << "/Master_" << setw(3) << setfill('0') << data.id << "/"
      << "T" << setw(2) << setfill('0') << MASTER_DAC << "_M" << setw(3) << setfill('0') << data.id
      << ".log";
      st >> dacfname;
      break;
    case NONE :
      return true;
    }
  vector<float> A(36), B(36);
  bool fA=false, fB=false;
  int i=0;
  A.clear();
  B.clear();
  ifstream dacfile(dacfname.c_str());
  if (dacfile)
    {
      LOG4CPLUS_DEBUG (hvlog,"Load Voltage DAC calibration from " <<  dacfname);
      while (!dacfile.eof())
        {
          dacfile.getline(line,255);
          if (string(line).find("Limits") ==0)
            {
              dacfile.getline(line,255);
            }
          if (fA)
            {
              int cnt=0;

              string str(line);
              replace(str.begin(), str.end(), ',', ' ');
              stringstream st(str);

              float t;
              while (st >> t )
                {
                  A.push_back(t);
                  //cout << A[i]<<":";
                  i++;
                  cnt++;
                }
              //cout << endl;
              if (cnt==0) fA=false;
            }
          if (fB)
            {
              int cnt=0;
              string str(line);
              replace(str.begin(), str.end(), ',', ' ');
              stringstream st(str);
              float t;
              while (st >> t )
                {
                  B.push_back(t);
                  //   cout << B[i]<<":";
                  i++;
                  cnt++;
                }
              // cout << endl;
              if (cnt==0) fB=false;
            }
          if (string(line).find("A:") == 0)
            {
              i=0;
              fB=false;
              fA=true;
              A.clear();
            }
          if (string(line).find("B:") ==0)
            {
              i=0;
              fA=false;
              fB=true;
              B.clear();
            }

        }
      dacfile.close();
    }
  else
    {
      LOG4CPLUS_ERROR (hvlog, "Unable to open "<< dacfname);
      return false;
    }

  if (A.size() && B.size() && (A.size()==B.size()))
    {
      HVcalib c;
      for (int i=0; i< A.size(); i++)
        {
          data.dac[i].A = A[i];
          data.dac[i].B = B[i];
          // data.dac.push_back(c);
        }
    }
  return true;
}

// Load Current ADC calibration data from file
bool LoadCurrentCalibration(int card, int module)
{
  if (card>=dataservices.size() || module >= HVCARD_MAX_MODULES) return false;
  HVcalibdata& data = dataservices[card].calib[module];

  data.card = card;
  data.addr = module;
  data.type = dataservices[data.card].data->module[data.addr].type;
  data.id = dataservices[data.card].data->module[data.addr].ID;
  if ((data.id<=0) || (data.id>=511))	return false;

  stringstream st;
  char line[255];
  string modname = "";
  string curfname = "";
  string calibfname = "";

  switch (data.type)
    {
    case RDB30 :
    case RDB36 :
      st << HV_DATADIR << "/Remote_" << setw(3) << setfill('0') << data.id << "/"
      << "T" << setw(2) << setfill('0') << RDB_CUR << "_M" << setw(3) << setfill('0') << data.id
      << ".log";
      st >> curfname;
      st.clear();
      st << HV_DATADIR << "/Remote_" << setw(3) << setfill('0') << data.id << "/"
      << "Calib_ID" << setw(3) << setfill('0') << data.id
      << ".dump";
      st >> calibfname;
      break;
    case MASTER :
      st << HV_DATADIR << "/Master_" << setw(3) << setfill('0') << data.id << "/"
      << "T" << setw(2) << setfill('0') << MASTER_CUR << "_M" << setw(3) << setfill('0') << data.id
      << ".log";
      st >> curfname;
      st.clear();
      st << HV_DATADIR << "/Master_" << setw(3) << setfill('0') << data.id << "/"
      << "Calib_ID" << setw(3) << setfill('0') << data.id
      << ".dump";
      st >> calibfname;
      break;
    case NONE :
      return true;
    }

  map<char, map<short, short> >C;
  bool fC=false;
  int i=0;

  ifstream calibfile(calibfname.c_str());
  if (calibfile && !fForceCalibrationUpdate)
    {
      LOG4CPLUS_DEBUG (hvlog, "Load Current ADC Calibration from " << calibfname);
      while (!calibfile.eof())
        {
          calibfile.getline(line,255);
          string str(line);
          replace(str.begin(), str.end(), '#', ' ');
          replace(str.begin(), str.end(), ':', ' ');
          stringstream st(str);
          int idx;
          if (st >> idx)
            {
              int val;
              char ch=0;
              while (st >> val)
                {
                  data.cur[ch][idx] = val;
                  ch++;
                }
            }
        }

    }
  else
    {
      C.clear();
      ifstream curfile(curfname.c_str());
      if (curfile)
        {
          LOG4CPLUS_DEBUG (hvlog, "Load Current ADC Calibration from " << curfname);
          while (!curfile.eof())
            {
              curfile.getline(line,255);
              if (fC)
                {
                  int cnt=0;
                  string str(line);
                  replace(str.begin(), str.end(), ',', ' ');
                  stringstream st(str);
                  float t;
                  char ch=0;
                  while (st >> t )
                    {
                      C[ch][(int)t] = i+1;
                      // C[ch][(int)t] = (i-MAIN_OFFS)<0?1:(i-MAIN_OFFS)+1;
                      ch++;
                      cnt++;
                    }
                  i++;
                  if (cnt==0) fC=false;
                }
              if (string(line).find("Current Tab:") ==0)
                {
                  i=0;
                  fC=true;
                  C.clear();
                }
            }
          curfile.close();
        }
      else
        {
          LOG4CPLUS_ERROR (hvlog, "Unable to open "<< curfname);
          return false;
        }

      // cout << C.size() << endl;
      if (C.size())
        {
          for (int ch=0; ch<C.size(); ch++)
            {
              // cout << "Ch"<<ch<<":"<< C[ch].size()<< endl;
              for (int j=ADC_COUNTS-1; j>=0; j--)
                {
                  data.cur[ch][j] = C[ch][j];
                }

              int adc_offs=0;
              int zero_offs = 1;
              int offs = ADC_COUNTS-1;

              int o_cnt=0;
              // if (data.type != MASTER)
              {
                while (offs>=0 && data.cur[ch][offs]==0)
                  {
                    offs--;
                  }
                zero_offs = data.cur[ch][offs];
                adc_offs = offs;
                // cout << adc_offs << ":"<< zero_offs << endl;
                for (int iter=0; iter < 5; iter++)
                  {
                    o_cnt=0;
                    do
                      {
                        offs--;
                        o_cnt++;
                      }
                    while (offs>=0 && data.cur[ch][offs]==0);
                    //while (offs>=0 && data.cur[ch][offs]==0)
                    //      { offs--; o_cnt++;}
                    // cout << offs << ":"<< data.cur[ch][offs] << ":" << o_cnt << endl;
                    if (o_cnt<10)
                      {
                        zero_offs = (data.cur[ch][offs]>zero_offs)?data.cur[ch][offs]:zero_offs;
                        adc_offs = offs;
                      }
                    // offs--;
                  }
                //cout << adc_offs << ":"<< zero_offs << endl;
                for (int k=0; k<ADC_COUNTS; k++)
                  if (data.cur[ch][k])
                    data.cur[ch][k] = (data.cur[ch][k]-zero_offs)<1?1:data.cur[ch][k]-zero_offs;
              }
              int i=0;
              bool first=true;
              while (i<ADC_COUNTS)
                {
                  int j=0;
                  while (data.cur[ch][i+j]==0 && first && (j<ADC_COUNTS))
                    {
                      j++;
                    }
                  if (j && ((i+j)<ADC_COUNTS)) first =false;
                  for (int k=0; k<j; k++)
                    {
                      data.cur[ch][i+k]=data.cur[ch][i+j];
                    }
                  i+=j+1;
                  if (data.cur[ch][i]==0) data.cur[ch][i]=data.cur[ch][i-1];
                }
              /*
                for (int i=0; i< ADC_COUNTS-1; i++) {
                if (ch==1)
                cout << i << ":" << data.cur[ch][i] << endl;
                }
              */

            }
          ofstream calib(calibfname.c_str());
          for (int j=ADC_COUNTS-1; j>=0; j--)
            {
              calib << "#" << setw(3) << setfill('0') << j << ":";
              for (int ch=0; ch<C.size(); ch++)
                {
                  calib << " " << data.cur[ch][j];
                }
              calib << endl;
            }
          calib.close();
        }
    }
  return true;
}


bool SaveCalibrationToDB(int card, int module)
{
  if (card>=dataservices.size() || module >= HVCARD_MAX_MODULES) return false;
  HVcalibdata& data = dataservices[card].calib[module];

  if ((data.id<=0) || (data.id>=511))	return false;

  try
    {
      mysqlpp::Connection con(HV_DB_NAME, HV_DB_HOST, HV_DB_ADMIN, HV_DB_ADMIN_PASS, mysqlpp::use_exceptions);
      mysqlpp::Query query = con.query();
      query << "delete from CALIBRATION where CALIBRATION.ID = " << data.id << " and CALIBRATION.TYPE = " << data.type;
      query.execute();
      query.clear();
      query << "select * from CALIBRATION where CALIBRATION.ID = " << data.id << " and CALIBRATION.TYPE = " << data.type;
      mysqlpp::StoreQueryResult calib_res = query.store();
      string dac_str((char *)data.dac, sizeof(data.dac));
      string adc_str((char *)data.adc, sizeof(data.adc));
      string cur_str((char *)data.cur, sizeof(data.cur));
      string hvmon_str((char *)&data.vmon, sizeof(data.vmon));
      if (calib_res.empty())
        {
          query.clear();

          query << "insert into CALIBRATION VALUES (" << data.id
          << "," << data.type << ",\""
          << mysqlpp::escape << dac_str << "\",\""
          << mysqlpp::escape << adc_str << "\",\""
          << mysqlpp::escape << cur_str << "\",\""
          << mysqlpp::escape << hvmon_str <<"\")" << ends;
          query.execute();
          LOG4CPLUS_DEBUG (hvlog, "Calibration Data is saved into database");
          return true;
        }
      else
        {
          LOG4CPLUS_ERROR (hvlog, "Unalbe to delete old Calibration Data from database");
          return false;
        }
    }
  catch (mysqlpp::BadQuery& er)
    {
      cerr << "Error: " << er.what() << endl;
    }
  catch (exception & er)
    {
      cerr << "Error: " << er.what() << endl;
    }

  return false;

}


bool LoadADCCalibrationFromDB(int card, int module)
{
 
  if (card>=dataservices.size() || module >= HVCARD_MAX_MODULES) return false;
  HVcalibdata& data = dataservices[card].calib[module];
  if ((data.id<=0) || (data.id>=511))	return false;
  try
    {
      mysqlpp::Connection con(HV_DB_NAME, HV_DB_HOST, HV_DB_USER, HV_DB_USER_PASS, mysqlpp::use_exceptions);
      mysqlpp::Query query = con.query();
      query << "select * from CALIBRATION where CALIBRATION.ID = " << data.id << " and CALIBRATION.TYPE = " << data.type;
      mysqlpp::StoreQueryResult calib_res = query.store();
      if (!calib_res.empty())
        {

          query.clear();

          query.reset();
          query << "select CALIBRATION.ADC from CALIBRATION where CALIBRATION.ID = " << data.id << " and CALIBRATION.TYPE = " << data.type;
          UseQueryResult adc_res = query.use();
          Row adc_row = adc_res.fetch_row();
          const unsigned long *jj = adc_res.fetch_lengths();
          // cout << "Content-length: " << *jj << endl;
          memcpy(data.adc, adc_row[0].data(), *jj);

        }
    }
  catch (mysqlpp::BadQuery& er)
    {
      cerr << "Error: " << er.what() << endl;
    }
  catch (exception & er)
    {
      cerr << "Error: " << er.what() << endl;
    }

  try
    {
      mysqlpp::Connection con(HV_DB_NAME, HV_DB_HOST, HV_DB_USER, HV_DB_USER_PASS, mysqlpp::use_exceptions);
      mysqlpp::Query query = con.query();
      query << "select * from CALIBRATION where CALIBRATION.ID = " << data.id << " and CALIBRATION.TYPE = " << data.type;
      mysqlpp::StoreQueryResult calib_res = query.store();
      if (!calib_res.empty())
        {

          query.clear();


          query.reset();
          query << "select CALIBRATION.VMON from CALIBRATION where CALIBRATION.ID = " << data.id << " and CALIBRATION.TYPE = " << data.type;
          UseQueryResult vmon_res = query.use();
          Row vmon_row = vmon_res.fetch_row();
          const unsigned long *jj = vmon_res.fetch_lengths();
          // cout << "Content-length: " << *jj << endl;
          memcpy(&data.vmon, vmon_row[0].data(), *jj);

          return true;
        }
    }
  catch (mysqlpp::BadQuery& er)
    {
      cerr << "Error: " << er.what() << endl;
    }
  catch (exception & er)
    {
      cerr << "Error: " << er.what() << endl;
    }


  return false;

}


bool LoadCurrentCalibrationFromDB(int card, int module)
{
  if (card>=dataservices.size() || module >= HVCARD_MAX_MODULES) return false;
  HVcalibdata& data = dataservices[card].calib[module];
  if ((data.id<=0) || (data.id>=511))	return false;
  try
    {
      mysqlpp::Connection con(HV_DB_NAME, HV_DB_HOST, HV_DB_USER, HV_DB_USER_PASS, mysqlpp::use_exceptions);
      mysqlpp::Query query = con.query();
      query << "select * from CALIBRATION where CALIBRATION.ID = " << data.id << " and CALIBRATION.TYPE = " << data.type;
      mysqlpp::StoreQueryResult calib_res = query.store();
      if (!calib_res.empty())
        {
          query.clear();

          query.reset();
          query << "select CALIBRATION.CUR from CALIBRATION where CALIBRATION.ID = " << data.id << " and CALIBRATION.TYPE = " << data.type;
          UseQueryResult cur_res = query.use();
          Row cur_row = cur_res.fetch_row();
          const unsigned long *jj = cur_res.fetch_lengths();
          // cout << "Content-length: " << *jj << endl;
          memcpy(data.cur, cur_row[0].data(), *jj);

          return true;
        }
    }
  catch (mysqlpp::BadQuery& er)
    {
      cerr << "Error: " << er.what() << endl;
    }
  catch (exception & er)
    {
      cerr << "Error: " << er.what() << endl;
    }
  return false;
}


bool LoadDACCalibrationFromDB(int card, int module)
{
  if (card>=dataservices.size() || module >= HVCARD_MAX_MODULES) return false;
  HVcalibdata& data = dataservices[card].calib[module];
  if ((data.id<=0) || (data.id>=511))	return false;
  try
    {
      mysqlpp::Connection con(HV_DB_NAME, HV_DB_HOST, HV_DB_USER, HV_DB_USER_PASS, mysqlpp::use_exceptions);
      mysqlpp::Query query = con.query();
      query << "select * from CALIBRATION where CALIBRATION.ID = " << data.id << " and CALIBRATION.TYPE = " << data.type;
      mysqlpp::StoreQueryResult calib_res = query.store();
      if (!calib_res.empty())
        {
          query.clear();
          query << "select CALIBRATION.DAC from CALIBRATION where CALIBRATION.ID = " << data.id << " and CALIBRATION.TYPE = " << data.type;
          UseQueryResult dac_res = query.use();
          Row dac_row = dac_res.fetch_row();
          const unsigned long *jj = dac_res.fetch_lengths();
          // cout << "Content-length: " << *jj << endl;
          memcpy(data.dac, dac_row[0].data(), *jj);
          return true;
        }
    }
  catch (mysqlpp::BadQuery& er)
    {
      cerr << "Error: " << er.what() << endl;
    }
  catch (exception & er)
    {
      cerr << "Error: " << er.what() << endl;
    }
  return false;
}


bool LoadCalibration(int card, int module)
{
  if (card>=dataservices.size() || module >= HVCARD_MAX_MODULES) return false;
  HVcalibdata& data = dataservices[card].calib[module];

  if (!data.fLoaded)
    {
      /*
        memset(data.dac,0,sizeof(data.dac));
        memset(data.adc,0,sizeof(data.adc));
        memset(data.cur,0,sizeof(data.cur));
      */
      data.card = card;
      data.addr = module;
      data.type = dataservices[data.card].data->module[data.addr].type;
      data.id = dataservices[data.card].data->module[data.addr].ID;

      bool isLoaded = false;
      if ((data.id<=0) || (data.id>=511)) return false;

      if (!fForceCalibrationUpdate)    // Check for fForceCalibrationUpdate flag
        {

          isLoaded = LoadADCCalibrationFromDB(card, module);
          isLoaded &=  LoadDACCalibrationFromDB(card, module);
          isLoaded &=  LoadCurrentCalibrationFromDB(card, module);
          data.fLoaded = isLoaded;

          LOG4CPLUS_DEBUG (hvlog, MODULE_STR(card, module) << " Calibration Data is loaded from database");
        }
      else
        {
          LOG4CPLUS_INFO (hvlog, MODULE_STR(card, module) << " Update Calibration");

          isLoaded = LoadADCCalibration(card, module);
          isLoaded &=  LoadDACCalibration(card, module);
          isLoaded &=  LoadCurrentCalibration(card, module);
          data.fLoaded = isLoaded;

          if (isLoaded) SaveCalibrationToDB(card, module);
        }

    }
  //    dataservices[data.card].calib[data.addr] = data;
  return data.fLoaded;
}


