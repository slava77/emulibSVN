#include "hvserver.h"

typedef struct chan_calib
{
  UINT chan;
  ULONG vmon;
  UINT cnt;
  float Aadc;
  float Badc;
  float Adac;
  float Bdac;
} chan_calib;

#define MAX_DB 171 // max distr boards
#define MAX_M  24 // max masters
#define MAX_PR 4 // max primaries
#define MAX_HC 16 // max hostcards
#define MAX_A 16 // max addresses
#define MAX_COM 2 // max com ports
#define MAX_CA 5 // max primary addresses on com ports (0-4)
#define MAX_CH 37 // max channels
//#define CAL_RAMP_UP 5
#define	CAL_RAMP_UP 15 // !!! Comment it for production
#define CAL_RAMP_DOWN 100
#define MAX_PRIMARY_ADDR 256


int vadc[MAX_DB][MAX_CH];
int vdac1[MAX_DB][MAX_CH];
int vdac2[MAX_DB][MAX_CH];
// storages for adc calib coefficients
float adcA[MAX_DB][MAX_CH];
float adcB[MAX_DB][MAX_CH];

float dacA[MAX_DB][MAX_CH];
float dacB[MAX_DB][MAX_CH];

set <int> include_db; // dboard numbers to calibrate, if empty calibrate all
set <int> exclude_db; // dboards to exclude from calibration, if empty do not exclude any
// storages for primary calib values
int pr_set[MAX_PRIMARY_ADDR][2], pr_v[MAX_PRIMARY_ADDR][2];

// Load and fill lists of included and excluded modules from files
void loadModulesLists()
{


  // read list of boards to calibrate
  ifstream include_db_f(string(HV_DATADIR+"/include_db.txt").c_str());

  if (include_db_f.is_open())
    {
      include_db.clear();
      int dbn;
      while (!include_db_f.eof())
        {
          include_db_f >> dbn;
          if (dbn < 512) include_db.insert(dbn);
        }

      include_db_f.close();
    }

  // read list of boards to exclude
  ifstream exclude_db_f(string(HV_DATADIR+"/exclude_db.txt").c_str());

  if (exclude_db_f.is_open())
    {
      exclude_db.clear();
      int dbn;
      while (!exclude_db_f.eof())
        {
          exclude_db_f >> dbn;
          if (dbn < 512) exclude_db.insert(dbn);
        }

      exclude_db_f.close();
    }

  // print included and excluded boards
  if (include_db.empty())
    {
      LOG4CPLUS_DEBUG (hvcaliblog, "All boards included");
    }
  else
    {
      LOG4CPLUS_DEBUG (hvcaliblog,"List of included boards from file: " << string(HV_DATADIR+"/include_db.txt"));
      set<int>:: iterator it;
      for (it = include_db.begin(); it != include_db.end(); it++)
        LOG4CPLUS_DEBUG (hvcaliblog,"RDB ID#" << *it);
    }

  if (exclude_db.empty())
    {
      LOG4CPLUS_DEBUG (hvcaliblog, "No excluded boards");
    }
  else
    {
      LOG4CPLUS_DEBUG (hvcaliblog,"List of excluded boards from file: " << string(HV_DATADIR+"/exclude_db.txt"));
      set<int>:: iterator it;
      for (it = exclude_db.begin(); it != exclude_db.end(); it++)
        LOG4CPLUS_DEBUG (hvcaliblog,"RDB ID#" << *it);
    }
}


// Load primary ps calibration data from file
void loadPrimaryCalibrations()
{
  // read primary calibrations
  ifstream primcal(string(HV_DATADIR+"/adc_calib_primary_points.txt").c_str());

  if (primcal.is_open())
    {
      string buf;
      int port, addr, p1_set, p1_v, p2_set, p2_v;

      while (!primcal.eof())
        {
          getline(primcal, buf);
          if (buf[0] == '#') continue; // skip comment lines
          istringstream bufstr(buf);
          bufstr >> addr >> p1_set >> p1_v >> p2_set >> p2_v;
          // validate a bit
          if
          (
            addr < 0 || addr > MAX_PRIMARY_ADDR ||
            p1_set < 0 || p1_set > 2000 ||
            p1_v   < 0 || p1_v   > 2000 ||
            p2_set < 2000 || p2_set > 3800 ||
            p2_v   < 2000 || p2_v   > 3800
          )
            {
              LOG4CPLUS_WARN (hvcaliblog, "Error in primary calib file, line: " << buf);
            }

          // store
          pr_set[addr][0] = p1_set;
          pr_v[addr][0]   = p1_v;
          pr_set[addr][1] = p2_set;
          pr_v[addr][1]   = p2_v;
        }

      LOG4CPLUS_DEBUG (hvcaliblog, "primary calibration points:");
      for (int i = 0; i < MAX_PRIMARY_ADDR; i++)
        {
          if (pr_set[i][0] || pr_v[i][0] || pr_set[i][1] || pr_v[i][1])
            LOG4CPLUS_DEBUG (hvcaliblog, i << " " << pr_set[i][0] << " " << pr_v[i][0] << " " << pr_set[i][1] << " " << pr_v[i][1]);
        }

      primcal.close();
    }
}


void setChanCurrentTripLimit(int card, int module, int chan, int triplimit)
{
  HVcmd cmd;
  if ( LoadCalibration( card, module ) )
    {
      cmd.cmd = HV_SET_IMAX;
      HVcalibdata data = dataservices[card].calib[module];
      int v = 0;
      if ( data.type == MASTER )
        {
          triplimit = ( ( triplimit * 25 ) / 10 > data.cur[chan] [0] ) ? data.cur[chan] [0] : triplimit;
          while ( ( v <= 3999 ) && ( data.cur[chan] [v + 1] > ( triplimit * 25 ) / 10 )
                  || ( data.cur[chan] [v] == data.cur[chan] [v + 1] ) )v++;
        }
      else
        {
          triplimit = ( triplimit * 10 > data.cur[chan] [0] ) ? data.cur[chan] [0] : triplimit;
          while ( ( v <= 3999 ) && ( data.cur[chan] [v + 1] > ( triplimit * ADC_CONV_COEF ) )
                  || ( data.cur[chan] [v] == data.cur[chan] [v + 1] ) )v++;
        }
      //            cout << i << ": " << data.cur[k][i]     << endl;
      cmd.data = v;
      cmd.card = card;
      cmd.module = module;
      cmd.chan = chan;
      ioctl( fd, HVCARD_PCIE_IOXCMD, & cmd );
    }

}

void setModuleCurrentTripLimit(int card, int module, int triplimit)
{
  if (dataservices[card].data->module[module].type != NONE)
    {
      int num_chans = dataservices[card].data->module[module].num_chans;
      for (int i=0; i< num_chans; i++)
        {
          setChanCurrentTripLimit(card, module, i, triplimit);
        }
    }
}


bool checkRDBLink(int card, int module)
{

  HVmodule* r = &(dataservices[card].data->module[module]);
  if ((r->type != RDB30) && (r->type != RDB36))
    {
      LOG4CPLUS_WARN (hvcaliblog, MODULE_STR(card, module) << " wrong type");
      return false;
    }

  if (r->master_id ==0 || r->master_id>=63)
    {
      LOG4CPLUS_WARN (hvcaliblog, MODULE_STR(card, module) << " invalid linked Master ID ");
      return false;
    }

  if (r->master_chan >= 8)
    {
      LOG4CPLUS_WARN (hvcaliblog, MODULE_STR(card, module) << " invalid linked Master channel: " << r->master_chan);
      return false;
    }


  bool include_all = include_db.empty();
  bool include_this = (include_db.find(r->ID) != include_db.end());
  bool exclude_this = (exclude_db.find(r->ID) != exclude_db.end());
  if (!(include_all || include_this) || exclude_this)
    {
      LOG4CPLUS_WARN (hvcaliblog, MODULE_STR(card, module) << " explicitly excluded from calibration");
      return false;
    }


  if (r->master_hvcard>=0 && r->master_busaddr>=0)
    {
      HVmodule* m = &(dataservices[r->master_hvcard].data->module[r->master_busaddr]);
      if (m->type != MASTER)
        {
          LOG4CPLUS_WARN (hvcaliblog, MODULE_STR(card, module) << " invalid linked Master");
          return false;
        }

      if (m->chan[r->master_chan].status == HV_STATE_DEAD)
        {
          LOG4CPLUS_WARN (hvcaliblog, MODULE_STR(card, module) << " linked Master channel: " << r->master_chan << " is dead");
          return false;
        }

      map<int, map<int, HVPrimary*> >::iterator pr_port = primaries.find(m->master_busaddr);

      if (pr_port == primaries.end())
        {
          LOG4CPLUS_WARN (hvcaliblog, MODULE_STR(card, module) << " invalid linked Primary Port: " << (m->master_busaddr));
          return false;
        }


      map<int, HVPrimary*>::iterator pr_addr = pr_port->second.find(m->master_hvcard);
      if (pr_addr == pr_port->second.end())
        {
          LOG4CPLUS_WARN (hvcaliblog, MODULE_STR(card, module) << " invalid linked Primary Address: " << (m->master_hvcard));
          return false;
        }
    }

  return true;
}

// Get linked Primary PS index for RDB
int getLinkedPrimaryIndex(int card, int module)
{
  if (checkRDBLink(card, module))
    {
      HVmodule* r = &(dataservices[card].data->module[module]);
      HVmodule* m = &(dataservices[r->master_hvcard].data->module[r->master_busaddr]);
      map<int, map<int, HVPrimary*> >::iterator pr_port = primaries.find(m->master_busaddr);
      map<int, HVPrimary*>::iterator pr_addr = pr_port->second.find(m->master_hvcard);
      return pr_addr->first;
    }
  else return -1;

}

// Find expected timeout for setting voltage on RDB
int findVoltageSetupTimeout(vector<HVcmd>& rdbs)
{
  int maxVcurDiff = 0;
  ReadHVData();
  for (int i=0; i<rdbs.size(); i++)
    {
      HVcmd& rcmd = rdbs[i];
      HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);
      HVmodule* m = &(dataservices[r->master_hvcard].data->module[r->master_busaddr]);
      // if ( abs(int(m->chan[r->master_chan].vset) -  int(m->chan[r->master_chan].vcur)) > maxVcurDiff)
      // 	maxVcurDiff = abs(int(m->chan[r->master_chan].vset) - int(m->chan[r->master_chan].vcur));
      for (int k=0; k<r->num_chans; k++)
        {
          if ((r->chan[k].status> HV_STATE_OFF) && (r->chan[k].status < HV_STATE_DEAD))
            {
              if ( abs(int(r->chan[k].vset) -  int(r->chan[k].vcur))/CAL_RAMP_UP > maxVcurDiff)
                maxVcurDiff = abs(int(r->chan[k].vset) -  int(r->chan[k].vcur))/CAL_RAMP_UP;
            }

        }

    }
  // Add 10 seconds to timeout
  return (maxVcurDiff + 10);

}

void *doCalibration(void* arg)
{

  int ADC_CALIB_PR_VSET=3500;
  int CALIB_SAMPLES=20;
  int OFFSET=42;
  int DB_V1=3500;
  int DB_V2=3100;
  int DAC_CALIB_PR_VSET=3600;
  /*
  	int CAL_RAMP_UP=5;
  	CAL_RAMP_UP=25; // !!! Comment it for production
  	int CAL_RAMP_DOWN=50;
  */
  struct timeval t1, t2;



  while (1)
    {
      if (!fDoCalibration || fAutoDetect)
        {
          usleep(SEC);
          continue;
        }

      gettimeofday(&t1, NULL);

      LOG4CPLUS_INFO (hvcaliblog, "=== Starting RDB Voltage Calibration");

      srvStatus="CALIBRATION";
      svc_srvStatus->updateService(srvStatus);

      SaveRealConfig("preCALIB");

      memset(adcA, 0, sizeof(adcA));
      memset(adcB, 0, sizeof(adcB));
      memset(dacA, 0, sizeof(dacA));
      memset(dacB, 0, sizeof(dacB));
      memset(vadc, 0, sizeof(vadc));
      memset(vdac1, 0, sizeof(vdac1));
      memset(vdac2, 0, sizeof(vdac2));

      loadModulesLists();
      loadPrimaryCalibrations();


      // Create list of valid DBs to calibrate
      vector<HVcmd> calib_dbs;

      for (int i=0; i<dbs.size(); i++)
        {
          HVcmd rcmd = dbs[i];
          if (checkRDBLink(rcmd.card, rcmd.module))
            {
              if (fCalibrateOnlyTurnedOn)   // Only add RDBs, which state is not OFF
                {
                  if (dataservices[rcmd.card].data->module[rcmd.module].state == HV_STATE_OFF)
                    {
                      LOG4CPLUS_WARN (hvcaliblog, "Skipped " << MODULE_STR(rcmd) << " because it is turned Off");
                      continue;
                    }
                }
              rcmd.data=0; // Used as flag during calibration
              calib_dbs.push_back(rcmd);
              LOG4CPLUS_INFO (hvcaliblog, "Added " << MODULE_STR(rcmd) << " to calibration list");
            }
        }


      // Do not proceed with calibration if RBDs list is empty
      if (calib_dbs.size() == 0)
        {
          LOG4CPLUS_WARN (hvcaliblog, "No valid RDBs modules found for calibration");
          LOG4CPLUS_INFO (hvcaliblog, "=== Calibration Finished");
          fDoCalibration=false;
          LoadConfig("preCALIB");
          srvStatus="READY";
          svc_srvStatus->updateService(srvStatus);
          continue;
        }



      // Setup RDBs and linked masters for ADC calibration
      for (int i=0; i<calib_dbs.size(); i++)
        {

          int tripcnt=0;
          HVcmd rcmd = calib_dbs[i];
          HVcmd mcmd;

          HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);


          // if (r->master_hvcard>=0 && r->master_busaddr>=0) {

          LOG4CPLUS_DEBUG (hvcaliblog, "Setting " << MODULE_STR(rcmd) << " for ADC calibration");
          HVmodule* m = &(dataservices[r->master_hvcard].data->module[r->master_busaddr]);

          // Setup linked Master channel
          mcmd.card = r->master_hvcard;
          mcmd.module = r->master_busaddr;
          mcmd.chan = r->master_chan;
          mcmd.size = 0;

          mcmd.cmd = HV_SET_RAMP_UP;
          mcmd.data = CAL_RAMP_UP * 4; // Madosrky: master ramp is 4x that of primary, so Master reaches target well ahead
          ioctl(fd, HVCARD_PCIE_IOXCMD, &mcmd);

          mcmd.cmd = HV_SET_RAMP_DOWN;
          mcmd.data = CAL_RAMP_DOWN;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &mcmd);

          mcmd.cmd = HV_SET_VSET;
          mcmd.data = 4000+(4000<<16);
          ioctl(fd, HVCARD_PCIE_IOXCMD, &mcmd);

          mcmd.cmd = HV_SET_MOD_STATE;
          mcmd.data = HV_STATE_ON;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &mcmd);

          mcmd.cmd = HV_SET_STATE;
          mcmd.data = HV_STATE_ON;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &mcmd);

          // Setup RDB
          rcmd.size = 0;
          rcmd.chan = ALL;

          rcmd.cmd = HV_SET_RAMP_UP;
          rcmd.data = CAL_RAMP_UP;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);

          rcmd.cmd = HV_SET_RAMP_DOWN;
          rcmd.data = CAL_RAMP_DOWN;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);

          rcmd.cmd = HV_SET_VSET;
          rcmd.data = 4000+(4000<<16);
          ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);

          setModuleCurrentTripLimit(rcmd.card, rcmd.module, 10);

          rcmd.cmd = HV_SET_MOD_STATE;
          rcmd.data = HV_STATE_ON;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);

          rcmd.cmd = HV_SET_STATE;
          rcmd.data = HV_STATE_ON;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);

        }

      // Setup Primaries for ADC calibration
      pthread_mutex_lock(&PrimeIOmutex);

      for (map<int, map<int, HVPrimary*> >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr)
        {
          for (map<int, HVPrimary*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
            {


              HVPrimary* h = h_itr->second;
              LOG4CPLUS_DEBUG (hvcaliblog, "Setting " << PRIMARY_STR(h) << " to " << ADC_CALIB_PR_VSET << "V");
              h->port= itr->first;
              h->addr= h_itr->first;
              h->rampup = CAL_RAMP_UP;
              h->rampdown =CAL_RAMP_DOWN;
              h->vset = ADC_CALIB_PR_VSET;
              h->outset = 1;
              h->state = HV_STATE_ON;
              h->status = HV_STATE_ON;

            }
        }
      pthread_mutex_unlock(&PrimeIOmutex);

      // Waiting for Primaries to set
      for (map<int, map<int, HVPrimary*> >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr)
        {
          for (map<int, HVPrimary*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
            {


              HVPrimary* h = h_itr->second;
              LOG4CPLUS_DEBUG (hvcaliblog, "Waiting for " << PRIMARY_STR(h) << " to set " << ADC_CALIB_PR_VSET << "V");
              h->port= itr->first;
              h->addr= h_itr->first;
              while ((h->vcur != ADC_CALIB_PR_VSET))
                {
                  usleep(SEC);
                }
              if (h->vcur == ADC_CALIB_PR_VSET)
                {
                  LOG4CPLUS_INFO (hvcaliblog, PRIMARY_STR(h) << " Voltage set to " << ADC_CALIB_PR_VSET << "V");
                }

            }
        }

      // Waiting for RDBs to set
      bool isReady = false;
      int timecntr = 0;
      int timeout = findVoltageSetupTimeout(calib_dbs);
      LOG4CPLUS_INFO(hvcaliblog, "Expected timeout for setting voltage on RDBs " << timeout << "sec");
      vector<long long> ready_chans(calib_dbs.size());

      while (!isReady && (timecntr <= timeout))
        {
          usleep(SEC);
          ReadHVData();
          for (int rdb=0; rdb<calib_dbs.size(); rdb++)
            {

              HVcmd& rcmd = calib_dbs[rdb];
              if (rcmd.data <2)
                {
                  HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);
                  HVmodule* m = &(dataservices[r->master_hvcard].data->module[r->master_busaddr]);

                  // If master channel in OFF or failed state skip further calibration
                  if ((m->chan[r->master_chan].status <= HV_STATE_OFF) ||
                      (m->chan[r->master_chan].status >= HV_STATE_DEAD))
                    {
                      LOG4CPLUS_ERROR(hvcaliblog, MODULE_STR(r->master_hvcard,r->master_busaddr)
                                      << " Ch:"<< (r->master_chan+1) << " unable to turn On (Status:" << hvstate_str[m->chan[r->master_chan].status]<< ")");
                      calib_dbs[rdb].data=2;
                      continue;
                    }

                  int isLinkedMasterReady = (m->chan[r->master_chan].vcur == 4000);

                  int isModuleReady = 0;

                  int nReady=0;
                  int nChans=0;

                  for (int k=0; k<r->num_chans; k++)
                    {
                      if (r->chan[k].vcur == 4000)
                        {
                          nReady++;
                          nChans++;
                          ready_chans[rdb] |= (1LL<<k);
                        }
                      else
                        {
                          ready_chans[rdb] |= ~(1LL<<k);
                          if ((r->chan[k].status> HV_STATE_OFF) && (r->chan[k].status < HV_STATE_DEAD))
                            {
                              nChans++;
                            }
                          else
                            {
                              LOG4CPLUS_ERROR (hvcaliblog,  MODULE_STR(rcmd) << " Ch: " << (k+1)
                                               << " Failed to setup voltage (State:" << hvstate_str[r->chan[k].status] << ")");
                              calib_dbs[rdb].data=2;
                            }

                          continue;
                        }
                    }
                  isModuleReady=(nReady==nChans);

                  if (isLinkedMasterReady && isModuleReady) calib_dbs[rdb].data=1;
                }

            }

          int nModulesNotReady=0;
          for (int i=0; i<calib_dbs.size(); i++)
            {
              if (calib_dbs[i].data == 0) nModulesNotReady++;
            }
          isReady = (nModulesNotReady == 0);
          timecntr ++;

        }

      // Mark timed out modules as failed
      if (timecntr>timeout)
        {
          for (int i=0; i<calib_dbs.size(); i++)
            {
              if (calib_dbs[i].data == 0)
                {
                  calib_dbs[i].data = 2;

                  LOG4CPLUS_ERROR(hvcaliblog, MODULE_STR(calib_dbs[i].card,calib_dbs[i].module)
                                  << " timed out to setup voltage after " << timecntr << "sec" );
                  HVcmd rcmd = calib_dbs[i];
                  HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);
                  long long ready = ready_chans[i];
                  for (int j=0; j<r->num_chans; j++)
                    {
                      if (!((ready>>j) & 0x1LL))
                        {
                          LOG4CPLUS_ERROR(hvcaliblog, MODULE_STR(calib_dbs[i].card,calib_dbs[i].module)
                                          << " Ch:" << (j+1)<< " failed to set" );
                        }
                    }

                }
            }
        }

      sleep(10);

      // Read ADC values
      for (int i=0; i<CALIB_SAMPLES; i++)
        {
          ReadHVData();

          for (int i=0; i<calib_dbs.size(); i++)
            {
              HVcmd rcmd = calib_dbs[i];
              HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);
              for (int j=0; j<r->num_chans; j++)
                {
                  vadc[i][j] += r->chan[j].vmon;
                }
              vadc[i][36] += r->hvmon;

            }
          usleep(25000);
        }

      ofstream ofs(string(HV_DATADIR+"/adc_calib.csv").c_str());
      ofstream ofsv(string(HV_DATADIR+"/adc_calib_voltages.csv").c_str());

      // Calculate ADC slopes (offset=0)
      for (int i=0; i<calib_dbs.size(); i++)
        {
          HVcmd rcmd = calib_dbs[i];
          if (rcmd.data == 1)
            {
              HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);

              LOG4CPLUS_DEBUG (hvcaliblog, "Voltage ADC calibration for " <<  MODULE_STR(rcmd));
              ofs << "board: " << r->ID << endl;
              ofsv << "board: " << r->ID << endl;
              int V_PR = ADC_CALIB_PR_VSET+OFFSET;
              int pr_idx = getLinkedPrimaryIndex(rcmd.card, rcmd.module);
              if (pr_idx>=0 && pr_idx < MAX_PRIMARY_ADDR && pr_v[pr_idx][1] != 0 )
                {
                  V_PR = pr_v[pr_idx][1];
                }
              for (int j=0; j<r->num_chans; j++)
                {
                  adcA[i][j] = (float(V_PR))/(vadc[i][j]/(float)CALIB_SAMPLES);
                  LOG4CPLUS_DEBUG (hvcaliblog,  "ch" <<(j+1)<< " A:" << adcA[i][j]);
                  ofs << adcA[i][j] << ",";
                  ofsv << (vadc[i][j]/CALIB_SAMPLES) << ",";
                }
              adcA[i][36] = (float(V_PR))/(vadc[i][36]/(float)CALIB_SAMPLES);
              ofs << endl;
              ofsv << endl;
            }
        }


      ofs.close();
      ofsv.close();


      // save ADC calibrations into files
      string prepath = HV_DATADIR+"/Remote_";


      for (int ibrd=0; ibrd<calib_dbs.size(); ibrd++)
        {
          HVcmd rcmd = calib_dbs[ibrd];
          HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);
          if (rcmd.data == 1)
            {
              int delim = r->num_chans/3;
              ostringstream boardn;
              boardn << setw(3) << setfill('0') << r->ID;
              ostringstream fname;
              fname << prepath << boardn.str() << "/" << "T10_M" << setw(3) << setfill('0') << boardn.str() << ".log";
              ofstream ofs(fname.str().c_str(), ios_base::app);

              ofs << "========================================================\n"
              << "Test10 (ADC Calibration) results " << now()
              << "\nBoard ID" << boardn.str() << " "
              << r->num_chans << " channels\n"
              << "Linux calibration (server mode)\n\n"
              // !!! Need actual coefficients for Voltemeter channel
              << "Voltmeter channel: A=" << adcA[ibrd][36] <<", B=" << 0 << "\n" << endl; // B=0 correction by Madorsky

              ofs << "A:" << endl;
              for (int i = 0; i < r->num_chans; i++)
                {
                  ofs << adcA[ibrd][i] << ((i && ((i+1)%delim==0))?"\n":",");
                }
              ofs << "\n" << endl;
              ofs << "B:" << endl;
              for (int i = 0; i < r->num_chans; i++)
                {
                  ofs << adcB[ibrd][i] << ((i && ((i+1)%delim==0))?"\n":",");
                }
              ofs << "\n" << endl;

              ofs.close();
              // dataservices[rcmd.card].calib[rcmd.module].fLoaded = false;
              // fForceCalibrationUpdate = 1;
              LoadADCCalibration(rcmd.card, rcmd.module);
            }
          else LOG4CPLUS_ERROR (hvcaliblog, "Failed to calibrate ADC for " <<  MODULE_STR(rcmd));
        }


      // === Calibrate Voltage DAC


      // Setup Primaries for DAC calibration
      pthread_mutex_lock(&PrimeIOmutex);

      for (map<int, map<int, HVPrimary*> >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr)
        {
          for (map<int, HVPrimary*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
            {


              HVPrimary* h = h_itr->second;
              LOG4CPLUS_DEBUG (hvcaliblog, "Setting " << PRIMARY_STR(h) << " to " << DAC_CALIB_PR_VSET << "V");
              h->port= itr->first;
              h->addr= h_itr->first;
              h->rampup = 5;
              h->rampdown = 50;
              h->vset = DAC_CALIB_PR_VSET;
              h->outset = 1;
              h->state = HV_STATE_ON;
              h->status = HV_STATE_ON;

            }
        }
      pthread_mutex_unlock(&PrimeIOmutex);


      // Setup RDBs and Masters for DAC calibration point 1
      for (int i=0; i<calib_dbs.size(); i++)
        {
          HVcmd rcmd = calib_dbs[i];
          HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);
          if ( LoadCalibration( rcmd.card, rcmd.module) )
            {
              HVcalibdata data = dataservices[rcmd.card].calib[rcmd.module];
              for (int k=0; k<r->num_chans; k++)
                {
                  float val = DB_V1 * data.dac[k].A + data.dac[k].B;
                  rcmd.data = ( val < 0 ) ? 0 : ( unsigned long ) val;
                  rcmd.cmd = HV_SET_VSET;
                  if (data.adc[k].A>0)
                    {
                      float fval = ((float)(DB_V1)-data.adc[k].B)/data.adc[k].A;
                      int ival = (int) rint(fval);
                      // rcmd.data += (ival < 0) ? 0: (unsigned long) (ival<<16);
                      rcmd.data += (ival < 0) ? 0: (unsigned long) (4000<<16);

                    }
                  rcmd.chan = k;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                }

            }
        }

      // Waiting for Primaries to set
      for (map<int, map<int, HVPrimary*> >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr)
        {
          for (map<int, HVPrimary*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
            {


              HVPrimary* h = h_itr->second;
              LOG4CPLUS_DEBUG (hvcaliblog, "Waiting for " << PRIMARY_STR(h) << " to set " << DAC_CALIB_PR_VSET << "V");
              h->port= itr->first;
              h->addr= h_itr->first;
              while ((h->vcur != DAC_CALIB_PR_VSET))
                {
                  usleep(SEC);
                }
              if (h->vcur == DAC_CALIB_PR_VSET)
                {
                  LOG4CPLUS_INFO (hvcaliblog, PRIMARY_STR(h) << " Voltage set to " <<DAC_CALIB_PR_VSET << "V");
                }

            }
        }

      for (int i=0; i<calib_dbs.size(); i++)
        {
          calib_dbs[i].data = 0;

        }

      // Waiting for RDBs to set at point 1

      isReady = false;
      timecntr = 0;
      timeout = findVoltageSetupTimeout(calib_dbs)*2+40;
      LOG4CPLUS_INFO(hvcaliblog, "Expected timeout for setting P1 voltage on RDBs " << timeout << "sec");
      ready_chans.clear();

      while (!isReady && (timecntr <= timeout))
        {
          usleep(500000);
          ReadHVData();
          for (int rdb=0; rdb<calib_dbs.size(); rdb++)
            {

              HVcmd rcmd = calib_dbs[rdb];
              if ( calib_dbs[rdb].data <2)
                {
                  if ( LoadCalibration( rcmd.card, rcmd.module) )
                    {
                      HVcalibdata data = dataservices[rcmd.card].calib[rcmd.module];
                      HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);
                      HVmodule* m = &(dataservices[r->master_hvcard].data->module[r->master_busaddr]);

                      // If master channel in OFF or failed state skip further calibration
                      if ((m->chan[r->master_chan].status <= HV_STATE_OFF) ||
                          (m->chan[r->master_chan].status >= HV_STATE_DEAD))
                        {
                          LOG4CPLUS_ERROR(hvcaliblog, MODULE_STR(r->master_hvcard,r->master_busaddr)
                                          << " Ch:"<< (r->master_chan+1) << " unable to turn On (Status:" << hvstate_str[m->chan[r->master_chan].status]<< ")");
                          calib_dbs[rdb].data=2;
                          continue;
                        }

                      int isLinkedMasterReady = (m->chan[r->master_chan].vcur == 4000);

                      int isModuleReady = 0;

                      int nReady=0;
                      int nChans=0;

                      for (int k=0; k<r->num_chans; k++)
                        {
                          if (r->chan[k].vcur == r->chan[k].vset)
                            {
                              float fval = ((float)(DB_V1)-data.adc[k].B)/data.adc[k].A;
                              int ival = (int) rint(fval);
//								LOG4CPLUS_DEBUG (hvcaliblog,  "ch" <<k<< " adc diff:" << ((int)(r->chan[k].vmon)-ival)
//												 << " " << r->chan[k].vmon << " " << ival << " " << r->chan[k].vset);
                              if (abs((int)(r->chan[k].vmon)-ival) <=2)
                                {
                                  nReady++;
                                  nChans++;
                                  ready_chans[rdb] |= (1LL<<k);
//									LOG4CPLUS_DEBUG (hvcaliblog,  "ch" <<k<< " adc diff:" << ((int)(r->chan[k].vmon)-ival)
//													 << " " << r->chan[k].vmon << " " << ival);
                                  vdac1[rdb][k]=r->chan[k].vset;

                                }
                              else
                                {
                                  ready_chans[rdb] &= ~(1LL<<k);
                                  nChans++;
                                  int vset = r->chan[k].vset + int((ival-(int)(r->chan[k].vmon))*data.adc[k].A*data.dac[k].A);
//									LOG4CPLUS_DEBUG (hvcaliblog,  "ch" <<k<< " cur vset:" << r->chan[k].vset << " new vset: " << vset);
//									rcmd.data = vset+(ival<<16);
                                  rcmd.data = vset+(4000<<16);

                                  rcmd.chan = k;
                                  rcmd.cmd = HV_SET_VSET;
                                  ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);

                                }

                            }
                          else
                            {
                              ready_chans[rdb] &= ~(1LL<<k);
                              if ((r->chan[k].status> HV_STATE_OFF) && (r->chan[k].status < HV_STATE_DEAD))
                                {
                                  nChans++;
                                }
                              else
                                {
                                  LOG4CPLUS_ERROR (hvcaliblog,  MODULE_STR(rcmd) << " Ch: " << (k+1)
                                                   << " Failed to setup voltage (State:" << hvstate_str[r->chan[k].status] << ")");
                                  calib_dbs[rdb].data=2;
                                }

                              // if (r->chan[i].status != HV_STATE_DEAD) isReady = false;
                              continue;
                            }
                        }

                      isModuleReady=(nReady==nChans);

                      if (isLinkedMasterReady && isModuleReady) calib_dbs[rdb].data=1;
                    }
                }

            }

          int nModulesNotReady=0;
          for (int i=0; i<calib_dbs.size(); i++)
            {
              if (calib_dbs[i].data == 0) nModulesNotReady++;
            }
          isReady = (nModulesNotReady == 0);
          timecntr ++;

        }

      // Mark timed out modules as failed
      if (timecntr>timeout)
        {
          for (int i=0; i<calib_dbs.size(); i++)
            {
              if (calib_dbs[i].data == 0)
                {
                  calib_dbs[i].data = 2;
                  LOG4CPLUS_ERROR(hvcaliblog, MODULE_STR(calib_dbs[i].card,calib_dbs[i].module)
                                  << " timed out to setup P1 voltage after " << timecntr << "sec" );
                  HVcmd rcmd = calib_dbs[i];
                  HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);
                  long long ready = ready_chans[i];
                  for (int j=0; j<r->num_chans; j++)
                    {
                      if (!((ready>>j) & 0x1LL))
                        {
                          LOG4CPLUS_ERROR(hvcaliblog, MODULE_STR(calib_dbs[i].card,calib_dbs[i].module)
                                          << " Ch:" << (j+1)<< " failed to set" );
                        }
                    }
                }
            }
        }


      // Setup RDBs and Masters for DAC calibration point 2
      for (int i=0; i<calib_dbs.size(); i++)
        {
          HVcmd rcmd = calib_dbs[i];
          HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);
          if ( LoadCalibration( rcmd.card, rcmd.module) )
            {
// start of comment
              HVcalibdata data = dataservices[rcmd.card].calib[rcmd.module];
              for (int k=0; k<r->num_chans; k++)
                {
                  float val = DB_V2 * data.dac[k].A + data.dac[k].B;
                  rcmd.data = ( val < 0 ) ? 0 : ( unsigned long ) val;
                  rcmd.cmd = HV_SET_VSET;
                  if (data.adc[k].A>0)
                    {
                      float fval = ((float)(DB_V2)-data.adc[k].B)/data.adc[k].A;
                      int ival = (int) rint(fval);
//						rcmd.data += (ival < 0) ? 0: (unsigned long) (ival<<16);
                      rcmd.data += (ival < 0) ? 0: (unsigned long) (4000<<16);

                    }
                  rcmd.chan = k;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                }
// commented out by Madorsky
            }
        }

      usleep(SEC); // commented out by Madorsky
      ReadHVData();


// start of comment
      for (int i=0; i<calib_dbs.size(); i++)
        {
          if (calib_dbs[i].data == 1) calib_dbs[i].data = 0;

        }

      // Waiting for RDBs to set at point 2

      isReady = false;
      timecntr = 0;
      timeout = findVoltageSetupTimeout(calib_dbs)*2+40;
      LOG4CPLUS_INFO(hvcaliblog, "Expected timeout for setting P2 voltage on RDBs " << timeout << "sec");
      ready_chans.clear();

      while (!isReady && (timecntr <= timeout))
        {
          usleep(500000);
          ReadHVData();
          for (int rdb=0; rdb<calib_dbs.size(); rdb++)
            {

              HVcmd rcmd = calib_dbs[rdb];
              if (calib_dbs[rdb].data <2)
                {
                  if ( LoadCalibration( rcmd.card, rcmd.module) )
                    {
                      HVcalibdata data = dataservices[rcmd.card].calib[rcmd.module];
                      HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);
                      HVmodule* m = &(dataservices[r->master_hvcard].data->module[r->master_busaddr]);

                      // If master channel in OFF or failed state skip further calibration
                      if ((m->chan[r->master_chan].status <= HV_STATE_OFF) ||
                          (m->chan[r->master_chan].status >= HV_STATE_DEAD))
                        {
                          LOG4CPLUS_ERROR(hvcaliblog, MODULE_STR(r->master_hvcard,r->master_busaddr)
                                          << " Ch:"<< (r->master_chan+1) << " unable to turn On (Status:" << hvstate_str[m->chan[r->master_chan].status]<< ")");
                          calib_dbs[rdb].data=2;
                          continue;
                        }

                      int isLinkedMasterReady = (m->chan[r->master_chan].vcur == 4000);

                      int isModuleReady = 0;

                      int nReady=0;
                      int nChans=0;

                      for (int k=0; k<r->num_chans; k++)
                        {
                          if (r->chan[k].vcur == r->chan[k].vset)
                            {
                              float fval = ((float)(DB_V2)-data.adc[k].B)/data.adc[k].A;
                              int ival = (int) rint(fval);
//								LOG4CPLUS_DEBUG (hvcaliblog,  "ch" <<k<< " adc diff:" << ((int)(r->chan[k].vmon)-ival)
//												 << " " << r->chan[k].vmon << " " << ival << " " << r->chan[k].vset);
                              if (abs((int)(r->chan[k].vmon)-ival) <=2)
                                {
                                  ready_chans[rdb] |= (1LL<<k);
                                  nReady++;
                                  nChans++;
//									LOG4CPLUS_DEBUG (hvcaliblog,  "ch" <<k<< " adc diff:" << ((int)(r->chan[k].vmon)-ival)
//													 << " " << r->chan[k].vmon << " " << ival);
                                  vdac2[rdb][k]=r->chan[k].vset;
                                }
                              else
                                {
                                  ready_chans[rdb] &= ~(1LL<<k);
                                  nChans++;
                                  int vset = int(r->chan[k].vset + (ival-(int)(r->chan[k].vmon))*data.adc[k].A*data.dac[k].A);
//									LOG4CPLUS_DEBUG (hvcaliblog,  "ch" <<k<< " cur vset:" << r->chan[k].vset << " new vset: " << vset);
//									rcmd.data = vset+(ival<<16);
                                  rcmd.data = vset+(4000<<16);

                                  rcmd.chan = k;
                                  rcmd.cmd = HV_SET_VSET;
                                  ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);

                                }

                            }
                          else
                            {
                              ready_chans[rdb] &= ~(1LL<<k);
                              if ((r->chan[k].status> HV_STATE_OFF) && (r->chan[k].status < HV_STATE_DEAD))
                                {
                                  nChans++;
                                }
                              else
                                {
                                  LOG4CPLUS_ERROR (hvcaliblog,  MODULE_STR(rcmd) << " Ch: " << (k+1)
                                                   << " Failed to setup voltage (State:" << hvstate_str[r->chan[k].status] << ")");
                                  calib_dbs[rdb].data=2;
                                }

                              continue;
                            }
                        }

                      isModuleReady=(nReady==nChans);

                      if (isLinkedMasterReady && isModuleReady) calib_dbs[rdb].data=1;
                    }
                }

            }

          int nModulesNotReady=0;
          for (int i=0; i<calib_dbs.size(); i++)
            {
              if (calib_dbs[i].data == 0) nModulesNotReady++;
            }
          isReady = (nModulesNotReady == 0);
          timecntr ++;

        }

      // Mark timed out modules as failed
      if (timecntr>timeout)
        {
          for (int i=0; i<calib_dbs.size(); i++)
            {
              if (calib_dbs[i].data == 0)
                {
                  calib_dbs[i].data = 2;
                  LOG4CPLUS_ERROR(hvcaliblog, MODULE_STR(calib_dbs[i].card,calib_dbs[i].module)
                                  << " timed out to setup P2 voltage after " << timecntr << "sec" );
                  HVcmd rcmd = calib_dbs[i];
                  HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);
                  long long ready = ready_chans[i];
                  for (int j=0; j<r->num_chans; j++)
                    {
                      if (!((ready>>j) & 0x1LL))
                        {
                          LOG4CPLUS_ERROR(hvcaliblog, MODULE_STR(calib_dbs[i].card,calib_dbs[i].module)
                                          << " Ch:" << (j+1)<< " failed to set" );
                        }
                    }
                }

            }
        }
// commented out by Madorsky

      // calculate slopes and intercepts
      for (int ibrd=0; ibrd<calib_dbs.size(); ibrd++)
        {
          HVcmd rcmd = calib_dbs[ibrd];
          HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);
//			if (vdac1[ibrd][0] != -1) // the board is not removed
          if (calib_dbs[ibrd].data == 1)
            {
              LOG4CPLUS_DEBUG (hvcaliblog,  "===  Voltage DAC calibration for " << MODULE_STR(rcmd));
              for (int i = 0; i < r->num_chans; i++)
                {
// start of comment
                  float dacd = vdac1[ibrd][i] - vdac2[ibrd][i];
                  float vd = DB_V1 - DB_V2;
                  float Adac = dacd / vd;
                  float Bdac = vdac1[ibrd][i] - (DB_V1 * Adac);
// commented out by Madorsky

// addition by Madorsky
//					float Adac = ((float)vdac1[ibrd][i]) / (float)DB_V1;
//					float Bdac = 0;
// end addition by Madorsky

                  dacA[ibrd][i] = Adac;
                  dacB[ibrd][i] = Bdac;
                  LOG4CPLUS_DEBUG (hvcaliblog,  "ch" <<(i+1)<< " A:" << Adac << " B:" << Bdac << " vdac1: " << ((float)vdac1[ibrd][i]));
                }
            }

        }


      // printout in csv format
      ofs.open(string(HV_DATADIR+"/dac_calib.csv").c_str());

      for (int ibrd=0; ibrd<calib_dbs.size(); ibrd++)
        {
          HVcmd rcmd = calib_dbs[ibrd];
          HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);

          if (rcmd.data == 1)
//			if (vdac1[ibrd][0] != -1) // the board is not removed
            {
              ofs << "board: " << r->ID << endl;

              for (int i = 0; i < r->num_chans; i++)
                {
                  ofs << dacA[ibrd][i] << ",";
                }
              ofs << endl;

              for (int i = 0; i < r->num_chans; i++)
                {
                  ofs << dacB[ibrd][i] << ",";
                }
              ofs << endl;
            }

        }
      ofs.close();


      // save DAC calibrations into files

      for (int ibrd=0; ibrd<calib_dbs.size(); ibrd++)
        {
          HVcmd rcmd = calib_dbs[ibrd];
          HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);
          if (rcmd.data == 1)
            {
              int delim = r->num_chans/3;
              ostringstream boardn;
              boardn << setw(3) << setfill('0') << r->ID;
              ostringstream fname;
              fname << prepath << boardn.str() << "/" << "T11_M" << setw(3) << setfill('0') << boardn.str() << ".log";
              ofstream ofs(fname.str().c_str(), ios_base::app);

              ofs << "========================================================\n"
              << "Test11 (DAC Calibration) results " << now()
              << "\nBoard ID" << boardn.str() << " "
              << r->num_chans << " channels\n"
              << "Linux calibration (server mode)\n\n"
              << "Calibration DAC Range: [" << DB_V2 << " - " << DB_V1 << "]\n" << endl;

              ofs << "A:" << endl;
              for (int i = 0; i < r->num_chans; i++)
                {
                  ofs << dacA[ibrd][i] << ((i && ((i+1)%delim==0))?"\n":",");
                }
              ofs << "\n" << endl;
              ofs << "B:" << endl;
              for (int i = 0; i < r->num_chans; i++)
                {
                  ofs << fixed << setprecision(1) << dacB[ibrd][i] << ((i && ((i+1)%delim==0))?"\n":",");
                }
              ofs << "\n" << endl;

              ofs.close();
//				dataservices[rcmd.card].calib[rcmd.module].fLoaded = false;
//				fForceCalibrationUpdate = 1;
//				LoadCalibration(rcmd.card, rcmd.module);
              LoadDACCalibration(rcmd.card, rcmd.module);
              SaveCalibrationToDB(rcmd.card, rcmd.module);
            }
          else LOG4CPLUS_ERROR (hvcaliblog, "Failed to calibrate DAC for " <<  MODULE_STR(rcmd));
        }

      fDoCalibration=false;

      LoadRealConfig("preCALIB");

      gettimeofday(&t2, NULL);
      LOG4CPLUS_INFO (hvcaliblog, "=== Calibration Finished in " << ((t2.tv_sec+t2.tv_usec/1000000.) - (t1.tv_sec+t1.tv_usec/1000000.)) << "sec");
      srvStatus="READY";
      svc_srvStatus->updateService(srvStatus);

    }


}
