#include <iomanip>
#include <fstream>
#include <vector>
#include <cctype>
#include <unistd.h>
#include <netdb.h>
#include <map>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>

#include "hvserver.h"
#include "version.h"
#include "hvpvss.h"

#include "Wiener_PL512.h"
#include "hvwiener.h"

extern Wiener_PL512::Dictionary Wiener_PL512::dict;
extern char Wiener_PL512::OutputString[];

using namespace std;
using namespace mysqlpp;
using namespace log4cplus;

// Wiener_PL512::Dictionary Wiener_PL512::dict;
// char Wiener_PL512::OutputString[];

string      PRIMARY_DNS_NODE="localhost";
int         PRIMARY_DNS_PORT=2505;
string HV_DATADIR="/home/hvuser/data";   // Default path for calibration data files. Can be overridden by HV_DATADIR env variable
string HV_LOGSDIR="/home/hvuser/logs";   // Default path for data files and server logs. Can be overriden by HV_LOGSDIR env variable
string CFG_FILE="/etc/hvcard.cfg";
string HV_NETLOGSERVER=DEF_NETLOGSERVER;   // Default net logger server address
string WIENER_PL512= DEF_WIENER_PL512_ADDR;
string cfgname=DEF_CONFIG_NAME;
string log_msg="Start";
string servername = HV_DIM_NAME;
unsigned long PVSS_UPDATE_INTERVAL=DEF_PVSS_UPDATE_INTERVAL;
unsigned long PVSS_VMON_THRESHOLD=DEF_PVSS_VMON_THRESHOLD;
unsigned long PVSS_IMON_THRESHOLD=DEF_PVSS_IMON_THRESHOLD;
map<std::string, DimService*> wiener_services;
map<std::string, DimCommand*> wiener_commands;
dimWienerPS wiener;
std::queue<std::pair<std::string, std::string> > commands_queue;

int hcHostID=0;
bool fControlWienerPL512=true;

pthread_mutex_t IOmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t       wiener_thread=0;
DimService* svc_logger;

string logname;

vector<Wiener_PL512 *> WienerPSs;

Logger hvlog = Logger::getRoot();
Logger hvmonlog = Logger::getRoot();
Logger hvtiminglog = Logger::getRoot();
Logger hvcmdlog = Logger::getRoot();

string now();

void *trdWienerMonitor(void *);

int SaveConfig(string cfgname)
{
  return 0;
}
int LoadConfig(string cfgname)
{
  return 0;
}
int SaveRealConfig(string cfgname)
{
  return 0;
}
int LoadRealConfig(string cfgname)
{
  return 0;
}

void print_ds_entry(struct HVdataservice ds)
{
  cout << "server: " << ds.server << " name:" << ds.name << " realname:" << ds.realname <<
       " card: " << ds.card<< endl;
}

string now()
{
  char buf[255];
  time_t now=time(NULL);
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
  string time = string(buf);
  return time.substr(0,time.find("\n",0));

};

string now_fnstr()
{
  char buf[255];
  time_t now=time(NULL);
  strftime(buf, sizeof(buf), "%y%m%d-%H%M%S", localtime(&now));
  string time = string(buf);
  return time.substr(0,time.find("\n",0));

};

void unwind()
{

  if (wiener_thread) pthread_cancel(wiener_thread);
  for (int i=0; i<WienerPSs.size(); i++)
    {
      delete WienerPSs[i];
    }
  WienerPSs.clear();

  LOG4CPLUS_INFO (hvlog, "lvServer: exit called.");
  Logger::shutdown();
  DimServer::stop();
  delete svc_logger;
}

// Ctrl-C Handler
static void catch_sigint(int sig)
{
  LOG4CPLUS_DEBUG (hvlog, "lvServer: received signal "<< sig <<  " [Ctrl-C] from user, exiting.");
  exit(0);
}

void signal_handler(int sig)
{
  LOG4CPLUS_DEBUG (hvlog,"lvServer: received signal " << sig << ", exiting.");
  exit(0);
}

pthread_attr_t *makepriority(int priority)      /* create attribute object */
{
  pthread_attr_t *attr;
  int error;
  struct sched_param param;

  if ((attr = (pthread_attr_t *)malloc(sizeof(pthread_attr_t))) == NULL)
    return NULL;
  if (!(error = pthread_attr_init(attr)) &&
      !(error = pthread_attr_getschedparam(attr, &param)))
    {

      int resulting_priority = param.sched_priority+priority;
      resulting_priority = (resulting_priority<0)?0:((resulting_priority>=32)?32:resulting_priority);
      param.sched_priority += resulting_priority;
      error = pthread_attr_setschedparam(attr, &param);
    }
  if (error)                        /* if failure, be sure to free memory */
    {
      free(attr);
      errno = error;
      return NULL;
    }
  return attr;
}


void usage()
{
  printf("Usage: lvServer -wADDR\n");
  printf("Options:\n");
  printf("-wADDR\tcontrol Wiener-PL512 LV PS (ADDR - PS IP Address default=%s\n", WIENER_PL512.c_str());
};

void decodeWienerSysStatus(int status, dimWienerPS_system_status* s_str)
{
  s_str->mainOn 		= (status & MAIN_ON)?1:0;
  s_str->mainInhibit 	= (status & MAIN_INHIBIT)?1:0;
  s_str->localControlOnly	= (status & LOCAL_CTRL_ONLY)?1:0;
  s_str->inputFailure	= (status & INPUT_FAILURE)?1:0;
  s_str->outputFailure	= (status & OUTPUT_FAILURE)?1:0;
  s_str->fantrayFailure	= (status & FANTRAY_FAILURE)?1:0;
  s_str->sensorFailure	= (status & SENSOR_FAILURE)?1:0;
  s_str->VmeSysfail	= (status & VME_SYSFAIL)?1:0;
};

void decodeWienerChannelStatus(int status, dimWienerPS_channel_status* c_str)
{
  c_str->outputOn				= (status & OUTPUT_ON)?1:0;
  c_str->outputInhibit			= (status & OUTPUT_INHIBIT)?1:0;
  c_str->outputFailureMinSenseVoltage 	= (status & OUTPUT_FAILURE_MIN_SENSE_VOLTAGE)?1:0;
  c_str->outputFailureMaxSenseVoltage	= (status & OUTPUT_FAILURE_MAX_SENSE_VOLTAGE)?1:0;
  c_str->outputFailureMaxTerminalVoltage	= (status & OUTPUT_FAILURE_MAX_TERMINAL_VOLTAGE)?1:0;
  c_str->outputFailureMaxCurrent		= (status & OUTPUT_FAILURE_MAX_CURRENT)?1:0;
  c_str->outputFailureMaxTemperature 	= (status & OUTPUT_FAILURE_MAX_TEMPERATURE)?1:0;
  c_str->outputFailureMaxPower 		= (status & OUTPUT_FAILURE_MAX_POWER)?1:0;
};

int getHostID()
{
  char line[255];
  int HostID = 0;
  ifstream cfgfile(CFG_FILE.c_str());
  if (cfgfile)
    {
      LOG4CPLUS_DEBUG (hvlog, CFG_FILE <<" Opened");
      while (!cfgfile.eof())
        {
          cfgfile.getline(line,255);
          if (string(line).find("#") == 0) continue;
          if (string(line).find("HOSTID") != string::npos)
            {
              string str = string(line);
              int pos = str.find("=", 0);
              if (pos != string::npos)
                {
                  stringstream st(str.substr(pos+1, str.size()));
                  st >> HostID;
                }


            }
        }
    }
  return HostID;

}

int main(int argc, char **argv)
{

  int node=0;
  struct sigaction sa_old;
  struct sigaction sa_new;
  //	string servername = HV_DIM_NAME;

  string ps_addr="";
  if (argc >= 2)
    {
      for (int i=1; i< argc; i++)
        {
          if (argv[i][0] == '-')
            {
              switch (argv[i][1])
                {
                case 'w':
                  fControlWienerPL512 = true;
                  ps_addr = argv[i]+2;
                  if (ps_addr.length()>0) WIENER_PL512=ps_addr;
                  break;
                default :
                  printf("Unknown option %s\n",argv[i]);

                }
            }
        }
    }
  else
    {
      usage();
      return -1;
    }

  if (getenv("HV_DATADIR") && string(getenv("HV_DATADIR")).size())
    HV_DATADIR=getenv("HV_DATADIR");

  if (getenv("HV_LOGSDIR") && string(getenv("HV_LOGSDIR")).size())
    HV_LOGSDIR=getenv("HV_LOGSDIR");

  if (getenv("HV_NETLOGSERVER") && string(getenv("HV_NETLOGSERVER")).size())
    HV_NETLOGSERVER=getenv("HV_NETLOGSERVER");

  Logger root = Logger::getRoot();

  /*
    SharedAppenderPtr append_0(new SocketAppender(HV_NETLOGSERVER, 9998, servername));
    append_0->setName("netLog");
    Logger::getRoot().addAppender(append_0);
  */

  SharedAppenderPtr append_1(new RollingFileAppender(HV_LOGSDIR+"/lvServer.log", 1024*1024, 10));
  append_1->setName("ServerLog");
  append_1->setLayout( std::auto_ptr<Layout>(new TTCCLayout()) );
  Logger::getInstance("lvServer").addAppender(append_1);

  SharedAppenderPtr append_2(new ConsoleAppender());
  append_2->setLayout( std::auto_ptr<Layout>(new SimpleColoredLayout()) );
  Logger::getRoot().addAppender(append_2);

  hvlog = Logger::getInstance("lvServer");


  SharedAppenderPtr append_3(new RollingFileAppender(HV_LOGSDIR+"/lvMon.log", 1024*1024, 10));
  append_3->setName("LVMonitorLog");
  append_3->setLayout( std::auto_ptr<Layout>(new TTCCLayout()) );
  Logger::getInstance("hvMon").addAppender(append_3);

  hvmonlog = Logger::getInstance("hvMon");

  SharedAppenderPtr append_5(new RollingFileAppender(HV_LOGSDIR+"/hvCommands.log", 1024*1024, 10));
  append_5->setName("HVCommandsLog");
  append_5->setLayout( std::auto_ptr<Layout>(new TTCCLayout()) );
  Logger::getInstance("hvCommands").addAppender(append_5);

  hvcmdlog = Logger::getInstance("hvCommands");



  DimServer::addErrorHandler(new dimHVErrHandler());

  /* set up our exit routine */
  atexit(unwind);

  /* set up the signal handler */
  signal(SIGTERM,signal_handler);

  // set up signal handling
  sa_new.sa_handler = catch_sigint;
  sigemptyset(&sa_new.sa_mask);
  sa_new.sa_flags = 0;
  sigaction(SIGINT, &sa_new, &sa_old );


  hcHostID = getHostID();
  LOG4CPLUS_DEBUG(hvlog, "HostID: "<< hcHostID);

  // === Make DIM Server name
  stringstream st;
  st.clear();
  servername = "LV_DIM_SERVER";
  st << servername << "_HID" << hcHostID;
  st >> servername;

  HVDimServer hvserver(servername);
  HVDimServer::addErrorHandler(new dimHVErrHandler());

  if (fControlWienerPL512)
    {
      string psAddr = WIENER_PL512;
      LOG4CPLUS_INFO(hvlog, "Initializing Wiener PL512 at " << psAddr);
      WienerPSs.push_back(new Wiener_PL512(psAddr.c_str()));
      // sleep(2);
      ostringstream ss;
      string servername = "WIENER_PL512_HID";

      for (int i=0; i<WienerPSs.size(); i++)
        {

          Wiener_PL512* PS = WienerPSs[i];
          PS->ReadValues();
          //if (PS->ReadValues()) {
          ss.clear();

          ss << servername << (hcHostID+i);
          string basename = ss.str();
          wiener.sys.sysStatus = PS->GetSystemStatus();
          wiener.sys.sysMainSwitch = PS->GetMainSwitchStatus();
          wiener.sys.psOperatingTime = PS->GetOperatingTime();
          wiener.sys.sysUpTime = ((float)(PS->GetUpTime()))/100;
          memcpy(wiener.sys.nodeID, const_cast<char *>(PS->GetIP()), sizeof(wiener.sys.nodeID));
          memcpy(wiener.sys.sysDescr, PS->GetSystemDescription(), sizeof(wiener.sys.sysDescr));
          memcpy(wiener.sys.psSerialNumber, PS->GetSerialNumber(), sizeof(wiener.sys.psSerialNumber));
          wiener.sys.update_value = 0;
          decodeWienerSysStatus(wiener.sys.sysStatus, &(wiener.sys.decoded_status));
          wiener_services["SYSDATA"] = new DimService((basename+"/SYSDATA").c_str(), "I:3;F:1;C:40;C:40;C:8;I:9",
              &(wiener.sys), sizeof(dimWienerPS_system));


          wiener_commands["sysCmd"] = new DimCommand((basename+"/SYSCMD").c_str(),"C", &hvserver);

          int num_chans = PS->GetNumberOfChannels();
          wiener.chans.clear();
          wiener.chans = vector<dimWienerPS_channel>(num_chans);
          for (int ch=0; ch<num_chans; ch++)
            {
              ostringstream ssch;
              ssch << "CHAN" << setw(2) << setfill('0') <<ch;
              memcpy(wiener.chans[ch].name, const_cast<char *>(PS->GetChannelName(ch+1)), sizeof(wiener.chans[ch].name)) ;
              wiener.chans[ch].status = PS->GetChannelStatus(ch+1);
              decodeWienerChannelStatus(wiener.chans[ch].status, &(wiener.chans[ch].decoded_status));
              // wiener.chans[ch].sup_behaviour = PS->GetChannelSupervisionBehavior(ch+1);
              wiener.chans[ch].meas_volt = PS->GetChannelMeasuredVoltage(ch+1);
              wiener.chans[ch].meas_cur = PS->GetChannelMeasuredCurrent(ch+1);
              wiener.chans[ch].meas_term_volt = PS->GetChannelMeasuredTerminalVoltage(ch+1);
              wiener.chans[ch].out_switch = PS->GetChannelOutputSwitch(ch+1);
              wiener.chans[ch].out_volt = PS->GetChannelOutputVoltage(ch+1);
              // wiener.chans[ch].adj_volt = PS->GetChannelAdjustVoltage(ch+1);
              wiener.chans[ch].cur_limit = PS->GetChannelCurrentLimit(ch+1);
              wiener.chans[ch].min_sense_volt = PS->GetChannelMinimumSenseVoltage(ch+1);
              wiener.chans[ch].max_sense_volt = PS->GetChannelMaximumSenseVoltage(ch+1);
              wiener.chans[ch].max_term_volt = PS->GetChannelMaxTerminalVoltage(ch+1);
              wiener.chans[ch].max_cur = PS->GetChannelMaxCurrent(ch+1);
              wiener.chans[ch].cfg_max_sense_volt = PS->GetChannelConfigMaxSenseVoltage(ch+1);
              wiener.chans[ch].cfg_max_term_volt = PS->GetChannelConfigMaxTerminalVoltage(ch+1);
              wiener.chans[ch].cfg_max_cur = PS->GetChannelConfigMaxCurrent(ch+1);
              wiener.chans[ch].max_power = PS->GetChannelMaxPower(ch+1);
              wiener.chans[ch].group = PS->GetChannelGroup(ch+1);
              wiener.chans[ch].update_value = 0;

              wiener_services[ssch.str()] = new DimService((basename+"/" + ssch.str()+"/DATA").c_str(), "C:4;F:4;I:1;F:9;I:12",
                  &(wiener.chans[ch]), sizeof(dimWienerPS_channel));
              wiener_commands[ssch.str()] = new DimCommand((basename+"/" + ssch.str()+"/CMD").c_str(), "C", &hvserver);

            }
          //}
        }
    }



  hvserver.start(servername.c_str());
  sleep(2);

  PRIMARY_DNS_NODE = DimServer::getDnsNode();
  PRIMARY_DNS_PORT = DimServer::getDnsPort();

  LOG4CPLUS_INFO(hvlog, "Ver." << HV_DIM_SERVER_VER);
  LOG4CPLUS_INFO(hvlog, servername <<" DIM Server started");
  LOG4CPLUS_DEBUG(hvlog, "DIM_DNS_NODE=" << string(DimServer::getDnsNode()) << ":" << (int)(DimServer::getDnsPort()));
  LOG4CPLUS_DEBUG(hvlog, "HV_DATADIR="<< HV_DATADIR);
  LOG4CPLUS_DEBUG(hvlog, "HV_LOGSDIR="<< HV_LOGSDIR);

  if (fControlWienerPL512 && pthread_create(&wiener_thread,  NULL, trdWienerMonitor, NULL) != 0)
    {
      LOG4CPLUS_ERROR (hvlog, "Unable to create Wiener PL512 control thread");
      perror("pthread_create");
    }

  LOG4CPLUS_INFO (hvlog, "Server is ready to work");


  while (1)
    {
      time_t t1 = time(NULL);
      usleep(SEC);
    }
  return 0;
}

void *trdWienerMonitor(void *arg)
{
  int upd_cnt = 0;
  while (1)
    {

      usleep(2*SEC);
      for (int i=0; i<WienerPSs.size(); i++)
        {
          Wiener_PL512* PS = WienerPSs[i];
          if (PS->ReadValues())
            {
              LOG4CPLUS_DEBUG(hvlog, "PL512 \"" << (PS->GetIP() ? PS->GetIP() : "NULL")
                              << "\" cannot be read!");
            }
          else
            {
              upd_cnt++;
              // time_t Time = PS->GetTime();
              // LOG4CPLUS_INFO(hvlog, "PL512 \"" << (PS->GetIP() ? PS->GetIP() : "NULL")
              //		 << "\" was successfully read");
              bool fSysUpdate = false;
              wiener.sys.psOperatingTime = PS->GetOperatingTime();
              wiener.sys.sysUpTime = ((float)(PS->GetUpTime()))/100;
              int sysStatus = PS->GetSystemStatus();
              if (sysStatus != wiener.sys.sysStatus) fSysUpdate = true;
              wiener.sys.sysStatus = sysStatus;
              decodeWienerSysStatus(wiener.sys.sysStatus, &(wiener.sys.decoded_status));

              wiener.sys.sysMainSwitch = PS->GetMainSwitchStatus();
              if (fSysUpdate || upd_cnt == WIENER_UPD_DELAY)
                {
                  wiener_services["SYSDATA"]->updateService();
                }

              int old_ival=0;
              float old_fval=0.0;

              for (int ch=0; ch<wiener.chans.size(); ch++)
                {
                  bool fChUpdate = false;
                  ostringstream ssch;
                  ssch << "CHAN" << setw(2) << setfill('0') <<ch;
                  int chStatus = PS->GetChannelStatus(ch+1);
                  if (chStatus != wiener.chans[ch].status) fChUpdate = true;
                  wiener.chans[ch].status = chStatus;
                  decodeWienerChannelStatus(wiener.chans[ch].status, &(wiener.chans[ch].decoded_status));
                  // wiener.chans[ch].sup_behaviour = PS->GetChannelSupervisionBehavior(ch+1);
                  wiener.chans[ch].meas_volt = PS->GetChannelMeasuredVoltage(ch+1);
                  wiener.chans[ch].meas_cur = PS->GetChannelMeasuredCurrent(ch+1);
                  wiener.chans[ch].meas_term_volt = PS->GetChannelMeasuredTerminalVoltage(ch+1);
                  wiener.chans[ch].out_switch = PS->GetChannelOutputSwitch(ch+1);
                  wiener.chans[ch].out_volt = PS->GetChannelOutputVoltage(ch+1);
                  // wiener.chans[ch].adj_volt = PS->GetChannelAdjustVoltage(ch+1);
                  wiener.chans[ch].cur_limit = PS->GetChannelCurrentLimit(ch+1);
                  wiener.chans[ch].min_sense_volt = PS->GetChannelMinimumSenseVoltage(ch+1);
                  wiener.chans[ch].max_sense_volt = PS->GetChannelMaximumSenseVoltage(ch+1);
                  wiener.chans[ch].max_term_volt = PS->GetChannelMaxTerminalVoltage(ch+1);
                  wiener.chans[ch].max_cur = PS->GetChannelMaxCurrent(ch+1);
                  wiener.chans[ch].cfg_max_sense_volt = PS->GetChannelConfigMaxSenseVoltage(ch+1);
                  wiener.chans[ch].cfg_max_term_volt = PS->GetChannelConfigMaxTerminalVoltage(ch+1);
                  wiener.chans[ch].cfg_max_cur = PS->GetChannelConfigMaxCurrent(ch+1);
                  wiener.chans[ch].max_power = PS->GetChannelMaxPower(ch+1);
                  wiener.chans[ch].group = PS->GetChannelGroup(ch+1);
                  if (fChUpdate || upd_cnt == WIENER_UPD_DELAY)
                    {
                      wiener_services[ssch.str()]->updateService();
                    }
                }
              if (upd_cnt >= WIENER_UPD_DELAY) upd_cnt = 0;
            }
        }
    }
}

