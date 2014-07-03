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

#ifdef WATCHDOG
#include <librebound.h>
ReboundTimer watchdog;
#endif

#include "hvserver.h"
#include "hvprimary.h"
#include "hvdb.h"
#include "version.h"
#include "hvpvss.h"

#include "Wiener_PL512.h"
#include "hvwiener.h"

extern Wiener_PL512::Dictionary Wiener_PL512::dict;
extern char Wiener_PL512::OutputString[];

using namespace std;
using namespace mysqlpp;
using namespace log4cplus;

string      PRIMARY_DNS_NODE="localhost";
int         PRIMARY_DNS_PORT=2505;
string HV_DATADIR="/home/hvuser/data";   // Default path for calibration data files. Can be overridden by HV_DATADIR env variable
string HV_LOGSDIR="/home/hvuser/logs";   // Default path for data files and server logs. Can be overriden by HV_LOGSDIR env variable
string HV_NETLOGSERVER=DEF_NETLOGSERVER;   // Default net logger server address
string WIENER_PL512= DEF_WIENER_PL512_ADDR;
string cfgname=DEF_CONFIG_NAME;
string log_msg="Start";
char * srvStatus = "UNKNOWN";
string servername = HV_DIM_NAME;

unsigned long PVSS_UPDATE_INTERVAL=DEF_PVSS_UPDATE_INTERVAL;
unsigned long PVSS_VMON_THRESHOLD=DEF_PVSS_VMON_THRESHOLD;
unsigned long PVSS_IMON_THRESHOLD=DEF_PVSS_IMON_THRESHOLD;

map<std::string, DimService*> wiener_services;
map<std::string, DimCommand*> wiener_commands;
dimWienerPS wiener;

ULONG cmd_cntr=1;
int fd=-1;
int hcHostID=0;
int hcFound=0;
int hcMax=0;

bool fMaster = true, fNode = false, fPrimary = true, fFirstRun = true;
bool fAutoDetect = false, fFastDetect=true, fLoadConfig=false,  fCSCMap = true;
bool fLockDimData = false, fDBUpdateCSC_DATA=false;
bool fForceCalibrationUpdate=false;
bool fUpdatePVSS=false, fNetLog=false;
bool fRun=true;
bool fRpcProcessing=false, fControlWienerPL512=false;
bool fDoCalibration=false;
bool fCalibrateOnlyTurnedOn=false;
bool fDebug = false;
bool fTestMode = false;

HV_STATE GlobalState = HV_STATE_ON;

sem_t main_sem;
pthread_mutex_t IOmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t PrimeIOmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t CmdIOmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t LVIOmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t WDmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t CSCMonmutex = PTHREAD_MUTEX_INITIALIZER;
int watch_wiener_ctl_cnt=0; // Watch Dog counter for Wiener Control
int watch_wiener_mon_cnt=0; // Watch Dog counter for Wiener Monitor
int csc_mon_cnt=0;	    // Watch Dog Counter for CSC Monitoring Thread
int csc_mon_cnt_freq=0;
int lock_id=0;

pthread_t       save_thread=0;
pthread_t       read_thread=0;
pthread_t	pvss_thread=0;
pthread_t       wiener_mon_thread=0;
pthread_t       wiener_ctl_thread=0;
pthread_t	primary_thread=0;
pthread_t 	calib_thread=0;
pthread_t       watch_thread=0;
pthread_t	csc_mon_thread=0;

DimService* svc_hcFound=NULL;
DimService* svc_logger=NULL;
DimService* svc_srvVersion=NULL;
DimService* svc_srvStatus=NULL;
DimService* svc_hvGlobalState=NULL;
DimCommand* cmd_srvControl=NULL;
DimCommand* pvss_cmdControl=NULL;

vector<HVdataservice> dataservices;
string logname;

vector<Wiener_PL512 *> WienerPSs;

vector<HVhostcard *> cards;
vector<DimService *> services;
map<string, vector<DimService *> >nodes;
map<int, map<int, HVPrimary*> > primaries;
vector<HVcmd>masters;
vector<HVcmd>dbs;
map<int, HVcmd>masters_map;
map<int, HVcmd>dbs_map;
map<string, pair<int, int> > csc_map;
std::queue<std::pair<std::string, std::string> > commands_queue;
std::queue<std::string> cscdata_updates_queue;
map<string, HVcscMon> cscMonData;
map<string, HVprimaryMon> primaryMonData;


Logger hvlog = Logger::getRoot();
Logger hvmonlog = Logger::getRoot();
Logger hvtiminglog = Logger::getRoot();
Logger hvcmdlog = Logger::getRoot();
Logger hvcaliblog = Logger::getRoot();
Logger hvdebug = Logger::getRoot();


string now();
string now(time_t tstamp, const char* format);
int GetHVData(int fd, UCHAR card, UCHAR module, UCHAR chan, HV_CMD cmd, ULONG *data, ULONG size);
int AutoDetect();
bool LoadCalibration(int card, int module);
bool LoadCSCMap();
bool ParseCSC(string csc_name , HVcsc *csc);
int UpdateDataDB();
int UpdateDataDim();
int UpdateConfigDatabase(int hostid);
bool isValidModule(int mod_type, int ID);
void PrepareDataFileName(int hcard, int mod);

void *trdUpdatePVSS(void *);
void *trdSaveDataToFile(void *);
void *trdReadDataFromDriver(void *);
void *trdWienerMonitor(void *);
void *trdWienerControl(void *);
void *trdWatchDog(void *);
void *trdCSCMon(void *);

HVDimDataUpdateTimer* DimDataUpdateTimer;
HVDataBaseUpdateTimer* DataBaseUpdateTimer;
HVPrimaryControlUpdateTimer* PrimaryControlUpdateTimer;

void print_ds_entry(struct HVdataservice ds)
{
  cout << "server: " << ds.server << " name:" << ds.name << " realname:" << ds.realname <<
       " card: " << ds.card<< endl;
}

string now(time_t tstamp, const char* format)
{
  char buf[255];
  time_t now = tstamp;
  if (tstamp == 0) now=time(NULL);
  const struct tm * timeptr = localtime(&now);
  strftime(buf, sizeof(buf),format, timeptr);
  std::string time = std::string(buf);
  if (time.find("\n",0) != std::string::npos)
    time = time.substr(0,time.find("\n",0));
  else
    {
      if (time.length() == 0)
        time = "---";
    }
  return time;

}


std::string now()
{
  return now(0);
}

/*
string now()
{
  char buf[255];
  time_t now=time(NULL);
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
  string time = string(buf);
  return time.substr(0,time.find("\n",0));

};
*/

string now_fnstr(time_t t=0)
{
  char buf[255];
  time_t now;
  if (t==0) now=time(NULL);
  else now=t;

  strftime(buf, sizeof(buf), "%y%m%d-%H%M%S", localtime(&now));
  string time = string(buf);
  return time.substr(0,time.find("\n",0));

}

void unwind()
{
  if (fd<0) return;

  fRun=false;
  srvStatus = "HALT";
  svc_srvStatus->updateService();

  if (csc_mon_thread)
    {
      LOG4CPLUS_DEBUG (hvlog, "Stopping CSC Monitoring Thread");
      if (pthread_cancel(csc_mon_thread) != 0)
        LOG4CPLUS_WARN (hvlog, "Can not stop CSC Monitoring Thread");
    }

  if (watch_thread)
    {
      LOG4CPLUS_DEBUG (hvlog, "Stopping WatchDog Thread");
      if (pthread_cancel(watch_thread) != 0)
        LOG4CPLUS_WARN (hvlog, "Can not stop WatchDog Thread");
    }

  if (pvss_thread)
    {
      LOG4CPLUS_DEBUG (hvlog, "Stopping PVSS Thread");
      if (pthread_cancel(pvss_thread) != 0)
        LOG4CPLUS_WARN (hvlog, "Can not stop PVSS Thread");
    }

  if (save_thread)
    {
      LOG4CPLUS_DEBUG (hvlog, "Stopping Data Save Thread");
      if (pthread_cancel(save_thread) != 0)
        LOG4CPLUS_WARN (hvlog, "Can not stop Data Save Thread");
    }

  if (read_thread)
    {
      LOG4CPLUS_DEBUG (hvlog, "Stopping Data Read Thread");
      if (pthread_cancel(read_thread) != 0)
        LOG4CPLUS_WARN (hvlog, "Can not stop Data Read Thread");
    }

  if (wiener_ctl_thread)
    {
      LOG4CPLUS_DEBUG (hvlog, "Stopping Wiener Control Thread");
      if (pthread_cancel(wiener_ctl_thread) != 0)
        LOG4CPLUS_WARN (hvlog, "Can not stop Wiener Control Thread");
    }

  if (wiener_mon_thread)
    {
      LOG4CPLUS_DEBUG (hvlog, "Stopping Wiener Monitor Thread");
      if (pthread_cancel(wiener_mon_thread) != 0)
        LOG4CPLUS_WARN (hvlog, "Can not stop Wiener Monitor Thread");
    }

  if (calib_thread)
    {
      LOG4CPLUS_DEBUG (hvlog, "Stopping Calibration Thread");
      if (pthread_cancel(calib_thread) != 0)
        LOG4CPLUS_WARN (hvlog, "Can not stop Calibration Thread");
    }

  if (PrimaryControlUpdateTimer)
    {
      PrimaryControlUpdateTimer->stop();
      delete PrimaryControlUpdateTimer;
    }

  if (primary_thread)
    {
      LOG4CPLUS_DEBUG (hvlog, "Stopping Primary PS Control Thread");
      if (pthread_cancel(primary_thread) != 0)
        LOG4CPLUS_WARN (hvlog, "Can not stop Primary PS Control Thread");
    }


  if (!fAutoDetect) SaveConfig(DEF_CONFIG_NAME);
  if (fDBUpdateCSC_DATA)
    DataBaseUpdateTimer->stop();

  for (int i=0; i<WienerPSs.size(); i++)
    {
      delete WienerPSs[i];
    }
  WienerPSs.clear();

  if (fDBUpdateCSC_DATA)
    delete DataBaseUpdateTimer;

  DimServer::stop();

  for (int i=0; i<dataservices.size(); i++)
    for (int j=0; j<HVCARD_MAX_MODULES; j++)
      {
        if (dataservices[i].vlog[j]!=NULL)
          {
            dataservices[i].vlog[j]->close();
            delete dataservices[i].vlog[j];
          }
        if (dataservices[i].ilog[j]!=NULL)
          {
            dataservices[i].ilog[j]->close();
            delete dataservices[i].ilog[j];
          }
      }

//   pthread_mutex_lock(&PrimeIOmutex);

  for (map<int, map<int, HVPrimary*> >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr)
    {
      for (map<int, HVPrimary*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
        {
          HVPrimary* h = h_itr->second;
          LOG4CPLUS_DEBUG (hvlog, "Turn Off " << PRIMARY_STR(h));
          h->port= itr->first;
          h->addr= h_itr->first;
          SendHVCmd(h->port, h->addr, "REN");
          SetHVMaxCurrentPercent(h->port, h->addr, 10);
          ResetHV(h->port, h->addr);
          // EnableHVOutput(h->port, h->addr);
          SetHVMaxVoltage(h->port, h->addr, 0);
          DisableHVOutput(h->port, h->addr);
        }
      int i = itr->first;
      if (CloseCOM(i) < 0)
        {
          LOG4CPLUS_ERROR (hvlog, "Unable to close COM" << i);
        }
      else
        {
          LOG4CPLUS_DEBUG (hvlog, "COM" << i << " is closed");
        }
    }
//   pthread_mutex_unlock(&PrimeIOmutex);
#ifdef WATCHDOG
  if (watchdog != kNullTimerRef) Rebound_CloseTimer(watchdog);
#endif
  if (pvss_cmdControl) delete pvss_cmdControl;

  // == Cleanup and delete DIM services associated with CSC Monitoring
  for (map<string, HVcscMon>::iterator itr = cscMonData.begin(); itr != cscMonData.end(); ++itr)
    {
      if (itr->second.fast_mon_dim != NULL)
        {
          delete  itr->second.fast_mon_dim;
          itr->second.fast_mon_dim = NULL;
        }
      if (itr->second.slow_mon_dim != NULL)
        {
          delete  itr->second.slow_mon_dim;
          itr->second.slow_mon_dim = NULL;
        }
      if (itr->second.stats_dim != NULL)
        {
          delete  itr->second.stats_dim;
          itr->second.stats_dim = NULL;
        }
    }

  delete svc_hcFound;
  delete svc_logger;
  delete svc_srvVersion;
  delete svc_srvStatus;
  delete cmd_srvControl;
  LOG4CPLUS_INFO (hvlog, "hvServer: exit called.");
  Logger::shutdown();
  // unlink(process_lock_filename);
}

// Ctrl-C Handler
static void catch_sigint(int sig)
{
  LOG4CPLUS_DEBUG (hvlog, "hvServer: received signal "<< sig <<  " [Ctrl-C] from user, exiting.");
  exit(0);
}

void signal_handler(int sig)
{
  LOG4CPLUS_DEBUG (hvlog,"hvServer: received signal " << sig << ", exiting.");
  exit(0);
}

void usage()
{
  printf("Usage: hvServer -[options]\n");
  printf("Options:\n");
  printf("-h\tshow usage help\n");
  printf("-m\tset server in master mode (set by default)\n");
  printf("-p\tset primary control mode (set by default)\n");
  printf("-a\tforce fast hardware configuration auto detection\n");
  printf("-l\tload configuration from DB\n");
  printf("-u\tenable periodical update of CSC data in database (off by default)\n");
  printf("-c\tforce updating calibration data in database\n");
  printf("-s\tupdate PVSS services\n");
  printf("-n\tsend log to network logging server\n");
  printf("-d\tenable DIM debug output\n");
  printf("-T\tenable hvServer Tests Mode\n");
  printf("-wADDR\tcontrol Wiener-PL512 LV PS (ADDR - PS IP Address default=%s\n", WIENER_PL512.c_str());
}

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
}

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
}


int main(int argc, char **argv)
{

  int node=0;
  struct sigaction sa_old;
  struct sigaction sa_new;

  string ps_addr="";
  if (argc >= 2)
    {
      for (int i=1; i< argc; i++)
        {
          if (argv[i][0] == '-')
            {
              switch (argv[i][1])
                {
                case 'h' :
                case '?' :
                  usage();
                  return 0;
                case 'm':
                  fMaster = true;
                  break;
                case 'n':
                  fNetLog = true;
                  break;
                case 'p':
                  fPrimary = true;
                  break;
                case 'a':
                  fAutoDetect = true;
                  fFastDetect = true;
                  break;
                case 'l':
                  fLoadConfig = true;
                  break;
                case 'u':
                  fDBUpdateCSC_DATA = true;
                  break;
                case 'c':
                  fForceCalibrationUpdate = true;
                  break;
                case 's':
                  fUpdatePVSS = true;
                  break;
                case 'w':
                  fControlWienerPL512 = true;
                  ps_addr = argv[i]+2;
                  if (ps_addr.length()>0) WIENER_PL512=ps_addr;
                  break;
                case 'd':
                  fDebug = true;
		  break;
		case 'T':
		  fTestMode = true;
                default :
                  printf("Unknown option %s\n",argv[i]);

                }
            }

        }
    }
#ifdef WATCHDOG
  watchdog = Rebound_OpenTimer("HVServer watchdog");
  if (watchdog != kNullTimerRef) Rebound_SetTimer(watchdog, 5);
#endif

  if (getenv("HV_DATADIR") && string(getenv("HV_DATADIR")).size())
    HV_DATADIR=getenv("HV_DATADIR");

  if (getenv("HV_LOGSDIR") && string(getenv("HV_LOGSDIR")).size())
    HV_LOGSDIR=getenv("HV_LOGSDIR");

  if (getenv("HV_NETLOGSERVER") && string(getenv("HV_NETLOGSERVER")).size())
    HV_NETLOGSERVER=getenv("HV_NETLOGSERVER");
  // helpers::LogLog::getLogLog()->setInternalDebugging(true);

  Logger root = Logger::getRoot();

  /*
    SharedAppenderPtr append_0(new SocketAppender(HV_NETLOGSERVER, 9998, servername));
    append_0->setName("netLog");
    Logger::getRoot().addAppender(append_0);
  */

  SharedAppenderPtr append_1(new RollingFileAppender(HV_LOGSDIR+"/hvServer.log", 5*1024*1024, 10));
  append_1->setName("ServerLog");
  append_1->setLayout( std::auto_ptr<Layout>(new SimpleColoredLayout(false)) );
  Logger::getInstance("hvServer").addAppender(append_1);

  SharedAppenderPtr append_2(new ConsoleAppender());
  append_2->setLayout( std::auto_ptr<Layout>(new SimpleColoredLayout()) );
  // Logger::getRoot().addAppender(append_2);
  Logger::getInstance("hvServer").addAppender(append_2);

  hvlog = Logger::getInstance("hvServer");


  SharedAppenderPtr append_3(new RollingFileAppender(HV_LOGSDIR+"/hvMon.log", 5*1024*1024, 10));
  append_3->setName("HVMonitorLog");
  append_3->setLayout( std::auto_ptr<Layout>(new SimpleColoredLayout(false)) );
  Logger::getInstance("hvMon").addAppender(append_3);
  Logger::getInstance("hvMon").addAppender(append_2);

  hvmonlog = Logger::getInstance("hvMon");

  SharedAppenderPtr append_4(new RollingFileAppender(HV_LOGSDIR+"/hvTiming.log", 1024*1024, 10));
  append_4->setName("HVTimingLog");
  append_4->setLayout( std::auto_ptr<Layout>(new SimpleColoredLayout(false)) );
  Logger::getInstance("hvTiming").addAppender(append_4);

  hvtiminglog = Logger::getInstance("hvTiming");

  SharedAppenderPtr append_5(new RollingFileAppender(HV_LOGSDIR+"/hvCommands.log", 5*1024*1024, 10));
  append_5->setName("HVCommandsLog");
  append_5->setLayout( std::auto_ptr<Layout>(new SimpleColoredLayout(false)) );
  Logger::getInstance("hvCommands").addAppender(append_5);

  hvcmdlog = Logger::getInstance("hvCommands");


  SharedAppenderPtr append_6(new RollingFileAppender(HV_LOGSDIR+"/hvCalibration.log", 5*1024*1024, 10));
  append_6->setName("HVCalibrationLog");
  append_6->setLayout( std::auto_ptr<Layout>(new SimpleColoredLayout(false)) );
  Logger::getInstance("hvCalibration").addAppender(append_6);
  Logger::getInstance("hvCalibration").addAppender(append_2);


  hvcaliblog = Logger::getInstance("hvCalibration");

  SharedAppenderPtr append_7(new RollingFileAppender(HV_LOGSDIR+"/hvDebug.log", 5*1024*1024, 10));
  append_7->setName("HVDebugLog");
  append_7->setLayout( std::auto_ptr<Layout>(new SimpleColoredLayout(false)) );
  Logger::getInstance("hvDebug").addAppender(append_7);

  hvdebug = Logger::getInstance("hvDebug");

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

  hcFound = 0;
  hcMax = 0;
  string devfn = "/dev/hvcard_pcie0";
  fd = open(devfn.c_str(),O_RDWR);
  if (fd<0)
    {
      LOG4CPLUS_FATAL(hvlog, "Can not open device file for HV HostCard\n" <<
                      "Check that HostCard is present and hvcard device driver is loaded\n" <<
                      "Exiting.");
      exit(0);
    }
  if (fd>0 )
    {
      // === Get Host ID from Driver
      hcHostID = ioctl(fd, HVCARD_PCIE_IOQHOSTID);
      LOG4CPLUS_DEBUG(hvlog, "HostID: "<< hcHostID);

      // === Make DIM Server name
      stringstream st;
      st.clear();
      st << servername << "_HID" << hcHostID;
      st >> servername;
      if (fNetLog)
        {
          SharedAppenderPtr append_0(new SocketAppender(HV_NETLOGSERVER, 9998, servername));
          append_0->setName("netLog");
          Logger::getInstance("hvMon").addAppender(append_0);
          Logger::getInstance("hvServer").addAppender(append_0);
          // Logger::getRoot().addAppender(append_0);
        }

      // === Get number of found Host Cards
      hcFound = ioctl(fd, HVCARD_PCIE_IOQDEVSFOUND);
      hcMax = ioctl(fd, HVCARD_PCIE_IOQNRDEVS);
      LOG4CPLUS_DEBUG(hvlog, hcFound << " hvcards found, max allowed: " << hcMax);
    }

  if (fDebug) dis_set_debug_on();

  HVDimServer hvserver(servername);
  HVDimServer::addErrorHandler(new dimHVErrHandler());


  // Init Wiener PL512 LV control
  if (fControlWienerPL512)
    {
      string psAddr = WIENER_PL512;
      LOG4CPLUS_INFO(hvlog, "Initializing Wiener PL512 at " << psAddr);
      WienerPSs.push_back(new Wiener_PL512(psAddr.c_str()));
      // sleep(2);
      ostringstream ss;
      string psname = "WIENER_PL512_HID";

      for (int i=0; i<WienerPSs.size(); i++)
        {

          Wiener_PL512* PS = WienerPSs[i];
          PS->ReadValues();
          //if (PS->ReadValues()) {
          ss.clear();

          ss << psname << (hcHostID+i);
          string basename = ss.str();
          wiener.sys.sysStatus = PS->GetSystemStatus();
          wiener.sys.sysMainSwitch = -1;

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
              wiener.chans[ch].out_switch = -1;
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

  // == Init DIM server services
  char* srvVersion 	= HV_DIM_SERVER_VER;
  srvStatus 		= "INIT";
  svc_hcFound 		= new DimService((servername+"/HCARDS_FOUND").c_str(), hcFound);
  svc_srvVersion 	= new DimService((servername+"/SERVER_VERSION").c_str(), srvVersion);
  svc_srvStatus 	= new DimService((servername+"/SERVER_STATUS").c_str(), srvStatus);
  int* GState = (int*)&GlobalState;
  svc_hvGlobalState     = new DimService("HV_GLOBAL_STATE", *GState);
  cmd_srvControl 	= new DimCommand((servername+"/SERVER_CONTROL").c_str(), "C", &hvserver);


  // == Net Logger appender
  if (fNetLog)
    {
      svc_logger = new DimService((servername+"/LOGGER").c_str(), const_cast<char *>(log_msg.c_str()));
      SharedAppenderPtr append_5(new DimAppender(svc_logger));
      append_5->setLayout( std::auto_ptr<Layout>(new DimLayout(false)) );
      Logger::getRoot().addAppender(append_5);
    }

  // == Define RPC services
  RpcRealData rpc_rawCmd(servername,"/HV_RAW_CMD");
  RpcRealData rpc_rawData(servername,"/HV_RAW_DATA");
  RpcRealData rpc_realData(servername,"/HV_REAL_DATA");
  RpcRealData rpc_realCmd(servername, "/HV_REAL_CMD");
  RpcPrimeHV rpc_PrimeHVData(servername,"/HV_PRIMARY_DATA");
  RpcPrimeHV rpc_PrimeHVCmd(servername, "/HV_PRIMARY_CMD");
  RpcCSCData rpc_cscData(servername,"/HV_CSC_DATA");
  RpcCSCData rpc_cscCmd(servername, "/HV_CSC_CMD");
  RpcMySQL sqlrpc(servername,"/HVDB");
  RpcHVStatus rpc_cscStatus(servername,"/HV_CSC_STATUS");
  RpcHVStatus rpc_moduleStatus(servername,"/HV_MODULE_STATUS");


  // == Set DIM network writing timeout to 30 sec
  hvserver.setWriteTimeout(10);

  // == Start DIM server
  hvserver.start(servername.c_str());
  sleep(2);

  PRIMARY_DNS_NODE = DimServer::getDnsNode();
  PRIMARY_DNS_PORT = DimServer::getDnsPort();

  LOG4CPLUS_INFO(hvlog, "Ver." << HV_DIM_SERVER_VER);
  LOG4CPLUS_INFO(hvlog, servername <<" DIM Server started");
  LOG4CPLUS_DEBUG(hvlog, "DIM_DNS_NODE=" << PRIMARY_DNS_NODE << ":" << PRIMARY_DNS_PORT);
  LOG4CPLUS_DEBUG(hvlog, "HV_DATADIR="<< HV_DATADIR);
  LOG4CPLUS_DEBUG(hvlog, "HV_LOGSDIR="<< HV_LOGSDIR);
  LOG4CPLUS_DEBUG(hvlog, "DIM Server Write timeout: " << hvserver.getWriteTimeout() << " sec");

  stringstream st;
  st << "C:" << sizeof(struct HVhostcard);
  string fmtstr;
  st >> fmtstr;
  int hcCount=0;


  // == Initialize HVdataservices
  dataservices.clear();
  cards.clear();
  services.clear();
  for (int i=0; i<hcFound*MAX_CONTROL_LINES; i++)
    {
      HVdataservice *datasvc = new(HVdataservice);
      st.clear();
      st << servername << "/HV_DATA" << i;
      string name;
      st >> name;
      LOG4CPLUS_DEBUG(hvlog, name);
      HVhostcard * card = new(HVhostcard);
      HVhostcard * setdata = new(HVhostcard);
      DimService * service = new DimService(name.c_str(), (char *)fmtstr.c_str(), card, sizeof(struct HVhostcard));
      cards.push_back(card);
      services.push_back(service);
      datasvc->server = servername;
      datasvc->name = name;
      datasvc->realname = name;
      datasvc->data = card;
      datasvc->setdata = setdata;
      datasvc->card = i;
      // datasvc->diminfo = NULL;
      datasvc->dimservice = service;
      memset(datasvc->flog, 0, sizeof(datasvc->flog));
      memset(datasvc->ilog, 0, sizeof(datasvc->ilog));
      memset(datasvc->vlog, 0, sizeof(datasvc->vlog));
      memset(datasvc->calib, 0, sizeof(datasvc->calib));
      memset(datasvc->databuf, 0, sizeof (datasvc->databuf));
      dataservices.push_back(*datasvc);
      hcCount++;
    }


  hcFound = dataservices.size();
  svc_hcFound->updateService(hcFound);

  nodes[servername] = services;


  DataBaseUpdateTimer  = new HVDataBaseUpdateTimer(2);

  // === Read Primary PS Configuration
  if (fPrimary) ReadPrimaryConfig(hcHostID);
  PrimaryControlUpdateTimer = new HVPrimaryControlUpdateTimer(1);


  for (int i=0; i<dataservices.size(); i++)
    {
      GetHVData(fd,i,0,0, HV_GET_CARD_DATA, (ULONG*)dataservices[i].data,sizeof(struct HVhostcard));
    }


  if (fCSCMap && LoadCSCMap()) LOG4CPLUS_INFO(hvlog, "CSC Mappings are loaded");

  if (fAutoDetect)
    {
//        PrimaryControlUpdateTimer->start();
      AutoDetect();
      UpdateConfigDatabase(hcHostID);
//        PrimaryControlUpdateTimer->stop();
      fAutoDetect = false;
      InitPrimaryMonData();
      InitCSCMonData();
    }


  if (fDBUpdateCSC_DATA)
    {
      LOG4CPLUS_INFO (hvlog, "Start CSC_DATA table update timer");
      DataBaseUpdateTimer->start();
    }


  pthread_attr_t *tattr;

  if (pthread_create(&read_thread,  NULL, trdReadDataFromDriver, NULL) != 0)
    {
      LOG4CPLUS_ERROR (hvlog, "Unable to create data read thread");
      perror("pthread_create");
    }

  if (pthread_create(&save_thread,  NULL, trdSaveDataToFile, NULL) != 0)
    {
      LOG4CPLUS_ERROR (hvlog, "Unable to create data file saving thread");
      perror("pthread_create");
    }

  if (pthread_create(&primary_thread,  NULL, trdUpdatePrimaryControl, NULL) != 0)
    {
      LOG4CPLUS_ERROR (hvlog, "Unable to create primary control thread");
      perror("pthread_create");
    }

  if (pthread_create(&calib_thread,  NULL, doCalibration, NULL) != 0)
    {
      LOG4CPLUS_ERROR (hvlog, "Unable to create calibration thread");
      perror("pthread_create");
    }

  if (pthread_create(&csc_mon_thread,  NULL, trdCSCMon, NULL) != 0)
    {
      LOG4CPLUS_ERROR (hvlog, "Unable to create CSC Monitoring thread");
      perror("pthread_create");
    }

  if (fControlWienerPL512 && pthread_create(&wiener_mon_thread,  NULL, trdWienerMonitor, NULL) != 0)
    {
      LOG4CPLUS_ERROR (hvlog, "Unable to create Wiener PL512 monitor thread");
      perror("pthread_create");
    }

  if (fControlWienerPL512 && pthread_create(&wiener_ctl_thread,  NULL, trdWienerControl, NULL) != 0)
    {
      LOG4CPLUS_ERROR (hvlog, "Unable to create Wiener PL512 control thread");
      perror("pthread_create");
    }

  if (pthread_create(&watch_thread,  NULL, trdWatchDog, NULL) != 0)
    {
      LOG4CPLUS_ERROR (hvlog, "Unable to create WatchDog thread");
      perror("pthread_create");
    }



  // === Start PVSS interface

  pvss_cmdControl = new PvssCommand("HV_1_COMMAND");

  /*
  if (fUpdatePVSS)
    {
      initPVSS(hcHostID);
      // pvss_cmdControl = new PvssCommand("HV_1_COMMAND");
      clearPVSSqueue();
      shmem_address->HV_PVSS_STARTED_UP = true;
      if (pthread_create(&pvss_thread,  NULL, trdUpdatePVSS, NULL) != 0)
        {
          LOG4CPLUS_ERROR (hvlog, "Unable to create data PVSS update thread");
          perror("pthread_create");
        }
    }
  */

  LOG4CPLUS_INFO (hvlog, "Server is ready to work");

  fRun=true;

  srvStatus="READY";
  svc_srvStatus->updateService(srvStatus);

  //time_t t0 = time(NULL);
  while (fRun)
    {
      time_t t1 = time(NULL);
      usleep(SEC);

      // == Handle LOAD_CONFIG command while server in running state
      if (fLoadConfig)
        {
          int res = LoadConfig(DEF_CONFIG_NAME);
          fLoadConfig = false;

        }

      // == Handle AUTO_DETECT command while server in running state
      if (fAutoDetect)
        {
          PrimaryControlUpdateTimer->start();
          AutoDetect();
          UpdateConfigDatabase(hcHostID);

          if (fUpdatePVSS)
            {
              initPVSS(hcHostID);
              clearPVSSqueue();
              shmem_address->HV_PVSS_STARTED_UP = true;
            }
          PrimaryControlUpdateTimer->stop();
          fAutoDetect = false;
        }

      CheckForTrips();

#ifdef WATCHDOG
      if (watchdog != kNullTimerRef)  Rebound_SetTimer(watchdog, 5);
#endif
      time_t t3 = time(NULL);
      if ((t3-t1)>2 && !fAutoDetect) LOG4CPLUS_WARN(hvtiminglog, "Time between Primaries PS updates " << (int)(t3-t1) << "sec");

      /*
      // === Measure main loop timing
      time_t t2 = time(NULL);
      if ((t2-t1)>5)  LOG4CPLUS_WARN(hvtiminglog, "Main loop time " << (int)(t2-t1) << "sec");
      */

    }

  if (fd>0) close(fd);

  return 0;
}



void *trdCSCMon(void* arg)
{
  LOG4CPLUS_INFO(hvlog, "CSC Monitoring thread is started");
  int MON_TRD_UPDATE = 5; // 5 Sec
  int FAST_MON_FORCED_INTERVAL = 300; // 5min
  std::string savedStatus = srvStatus;
  int isUpdated = 0;
  while (1)
    {
      usleep(MON_TRD_UPDATE*SEC);
      std::string status = srvStatus;
      if (pthread_mutex_trylock(&CSCMonmutex) == 0)
        {
          /*
                    ///* Set server status to UPDATING before sending updates
          	  savedStatus = srvStatus;
                    srvStatus="UPDATING";
                    svc_srvStatus->updateService(srvStatus);
          */
	  lock_id=1;
          if (csc_mon_cnt_freq > 0) csc_mon_cnt_freq--;
          pthread_mutex_unlock(&CSCMonmutex);
        }

      isUpdated = 0;

      for (map<string, HVcscMon >::iterator itr = cscMonData.begin(); itr != cscMonData.end(); ++itr)
        {
          if (pthread_mutex_trylock(&CSCMonmutex) == 0)
            {
	      lock_id=2;
              itr->second.update_counter++;
              if ( (itr->second.update_counter > 0) && ( itr->second.update_counter % (itr->second.slow_mon.update_interval/MON_TRD_UPDATE) == 0) )
                {

                  if (csc_mon_cnt_freq == 0) /// Delay after receiving DIM commands passed
                    {
                      // Save current data state
                      HVcscMon cur_cscMon;
                      memset(&cur_cscMon, 0, sizeof(cur_cscMon));
                      memcpy(&cur_cscMon, &(itr->second), sizeof(HVcscMon));
                      // Update Data
                      if (UpdateCSCMonData(itr->first, itr->second))
                        {

                          // Check if Fast Monitoring data DIM update is needed
                          if ( isFastMonUpdateNeeded(itr->second, cur_cscMon) || ( itr->second.update_counter % (FAST_MON_FORCED_INTERVAL/MON_TRD_UPDATE) == 0))
                            {
                              if (isUpdated == 0) isUpdated = 1;
                              if (isUpdated == 1 && fTestMode)
                                {
                                  isUpdated = 2;
                                  ///* Set server status to UPDATING before sending updates
                                  savedStatus = srvStatus;
                                  srvStatus="UPDATING";
                                  svc_srvStatus->updateService(srvStatus);
                                }
                              LOG4CPLUS_DEBUG(hvdebug, itr->first << " Fast Mon update initiated");
                              itr->second.fast_mon_dim->updateService();
                              itr->second.stats.fast_mon_cnt++;
                              itr->second.stats_dim->updateService();
                            }
                          // Check if Slow Monitoring data DIM update is needed
                          if ( isSlowMonUpdateNeeded(itr->second, cur_cscMon) )
                            {

                              if (isUpdated == 0) isUpdated = 1;
                              if (isUpdated == 1 && fTestMode)
                                {
                                  isUpdated = 2;
                                  ///* Set server status to UPDATING before sending updates
                                  savedStatus = srvStatus;
                                  srvStatus="UPDATING";
                                  svc_srvStatus->updateService(srvStatus);

                                }
                              LOG4CPLUS_DEBUG(hvdebug, itr->first << " Slow Mon update initiated");
                              itr->second.slow_mon_dim->updateService();
                              itr->second.stats.slow_mon_cnt++;
                              itr->second.stats_dim->updateService();
                            }

                        }

                    }
                }
              pthread_mutex_unlock(&CSCMonmutex);
            }
        }

      for (map<string, HVprimaryMon >::iterator itr = primaryMonData.begin(); itr != primaryMonData.end(); ++itr)
        {
          if (pthread_mutex_trylock(&CSCMonmutex) == 0)
            {
	      lock_id=3;
              itr->second.update_counter++;
              if ( (itr->second.update_counter > 0) && ( itr->second.update_counter % (itr->second.update_interval/MON_TRD_UPDATE) == 0) )
                {

                  if (csc_mon_cnt_freq == 0) /// Delay after receiving DIM commands passed
                    {
                      // Save current data state
                      HVprimaryMon cur_primaryMon;
                      memset(&cur_primaryMon, 0, sizeof(cur_primaryMon));
                      memcpy(&cur_primaryMon, &(itr->second), sizeof(HVprimaryMon));
                      // Update Data
                      if (UpdatePrimaryMonData(itr->first, itr->second))
                        {

                          // Check if Primary PS Monitoring data DIM update is needed
                          // if ( isPrimaryMonUpdateNeeded(itr->second, cur_primaryMon) )
                          {

                            if (isUpdated == 0) isUpdated = 1;
                            if (isUpdated == 1 && fTestMode)
                              {
                                isUpdated = 2;
                                ///* Set server status to UPDATING before sending updates
                                savedStatus = srvStatus;
                                srvStatus="UPDATING";
                                svc_srvStatus->updateService(srvStatus);
                              }
                            LOG4CPLUS_DEBUG(hvdebug, itr->first << " Primary PS Mon update initiated");
                            itr->second.primary_mon_dim->updateService();
                            itr->second.stats++;
                            itr->second.stats_dim->updateService();
                          }

                        }

                    }
                }
              pthread_mutex_unlock(&CSCMonmutex);
            }
        }

      if (isUpdated > 0 && fTestMode)
        {
          pthread_mutex_lock(&CSCMonmutex);
	  lock_id=4;

          ///* Set server status to saved one after sending updates
          srvStatus=(char *)savedStatus.c_str();
          // srvStatus = "READY";
          svc_srvStatus->updateService(srvStatus);

          pthread_mutex_unlock(&CSCMonmutex);
        }

      pthread_mutex_lock(&WDmutex);
      csc_mon_cnt = 0; /// Reset Watchdog counter
      pthread_mutex_unlock(&WDmutex);

    }
}

void *trdWatchDog(void *arg)
{
  LOG4CPLUS_INFO(hvlog, "WatchDog thread is started");
  while (1)
    {
      usleep(2*SEC);

      // Check Wiener Control Thread
      if (wiener_ctl_thread)
        {
          pthread_mutex_lock(&WDmutex);
          watch_wiener_ctl_cnt++;
          if (watch_wiener_ctl_cnt > 10)
            {
              LOG4CPLUS_WARN(hvlog, "WatchDog timeout expired for Wiener Control thread. Restarting...");
              // dim_unlock();
              watch_wiener_ctl_cnt = 0;

              /*
                    LOG4CPLUS_DEBUG (hvlog, "Stopping Wiener Control Thread");
                    if (pthread_cancel(wiener_ctl_thread) != 0)
                      {
                        LOG4CPLUS_WARN (hvlog, "Can not stop Wiener Control Thread");
                      }
                    else
              */
              {
                if (pthread_mutex_trylock(&LVIOmutex) == EBUSY)
                  {
                    LOG4CPLUS_DEBUG (hvlog, "Wiener IO mutex is in BUSY state. Unlocking...");
                  }
                pthread_mutex_unlock(&LVIOmutex);
                /*
                              if (pthread_create(&wiener_ctl_thread,  NULL, trdWienerControl, NULL) != 0)
                                {
                                  LOG4CPLUS_ERROR (hvlog, "Unable to create Wiener PL512 control thread");
                                  perror("pthread_create");
                                }
                */
              }

            }
          pthread_mutex_unlock(&WDmutex);
        }

      // Check Wiener Monitor Thread
      if (wiener_mon_thread)
        {
          pthread_mutex_lock(&WDmutex);
          watch_wiener_mon_cnt++;
          if (watch_wiener_mon_cnt > 10)
            {
              LOG4CPLUS_WARN(hvlog, "WatchDog timeout expired for Wiener Monitor thread. Restarting...");
              // dim_unlock();
              watch_wiener_mon_cnt = 0;
              /*
                    LOG4CPLUS_DEBUG (hvlog, "Stopping Wiener Monitor Thread");
                    if (pthread_cancel(wiener_mon_thread) != 0)
                      {
                        LOG4CPLUS_WARN (hvlog, "Can not stop Wiener Monitor Thread");
                      }
                    else
              */
              {
                if (pthread_mutex_trylock(&LVIOmutex) == EBUSY)
                  {
                    LOG4CPLUS_DEBUG (hvlog, "Wiener IO mutex is in BUSY state. Unlocking...");

                  }
                pthread_mutex_unlock(&LVIOmutex);
                /*
                              if (pthread_create(&wiener_mon_thread,  NULL, trdWienerMonitor, NULL) != 0)
                                {
                                  LOG4CPLUS_ERROR (hvlog, "Unable to create Wiener PL512 monitor thread");
                                  perror("pthread_create");
                                }
                */
              }

            }
          pthread_mutex_unlock(&WDmutex);
        }

      // Check CSC Monitoring Control Thread
      if (csc_mon_thread)
        {
          pthread_mutex_lock(&WDmutex);
          csc_mon_cnt++;
          if (csc_mon_cnt > 10)
            {
              LOG4CPLUS_WARN(hvlog, "WatchDog timeout expired for CSC Monitoring thread. Restarting...");
              // dim_unlock();
              csc_mon_cnt = 0;

              /*
                    LOG4CPLUS_DEBUG (hvlog, "Stopping CSC Monitoring Thread");
                    if (pthread_cancel(csc_mon_thread) != 0)
                      {
                        LOG4CPLUS_WARN (hvlog, "Can not stop CSC Monitoring Thread");
                      }
                    else
              */
              {
                if (pthread_mutex_trylock(&CSCMonmutex) == EBUSY)
                  {
                    LOG4CPLUS_DEBUG (hvlog, "CSC Monitoring thread mutex is in BUSY state. Unlocking...");
		    LOG4CPLUS_DEBUG (hvlog, "Lock location: " << lock_id);
                    // LOG4CPLUS_DEBUG (hvlog, "DIM Global mutex locks counter: " << dim_lock_counter());
                  }
                pthread_mutex_unlock(&CSCMonmutex);
                /*
                           if (pthread_create(&csc_mon_thread,  NULL, trdCSCMon, NULL) != 0)
                             {
                               LOG4CPLUS_ERROR (hvlog, "Unable to create CSC Monitoring thread");
                               perror("pthread_create");
                             }
                */
              }

            }
          pthread_mutex_unlock(&WDmutex);
        }

    }
}

void *trdWienerControl(void *arg)
{
  string cmdname = "";
  string cmdvalue = "";
  std::pair<string, string> cmd;
  LOG4CPLUS_INFO(hvlog, "Wiener Control Thread is started");
  while (1)
    {
      usleep(250000);
      pthread_mutex_lock(&LVIOmutex);
      while ( !commands_queue.empty() )
        {
          cmd = commands_queue.front();
          cmdname = cmd.first;
          cmdvalue = cmd.second;

          if (fControlWienerPL512)
            {
              // NOTE: Only one Wiener PS (unit) is supported
              int unit=0;
              Wiener_PL512* PS = WienerPSs[unit];

              string cmdptrn = "";
              // Handle Wiener System commands
              if ((cmdname.find("WIENER") != string::npos)
                  && (cmdname.rfind("SYSCMD") != string::npos))
                {
                  if (cmdvalue.find("SET_MAIN_ON") != string::npos)
                    {
                      PS->SetMainSwitch(1);
                    }
                  if (cmdvalue.find("SET_MAIN_OFF") != string::npos)
                    {
                      PS->SetMainSwitch(0);
                    }
                  if (cmdvalue.find("SET_ALL_CHANS_ON") != string::npos)
                    {
                      int nchans = PS->GetNumberOfChannels();
                      if ((nchans >= 1) && (nchans <= 12))
                        {
                          for (int ch=1; ch <= nchans; ch++)
                            PS->SetChannelOutputSwitch(ch, 1);
                        }
                    }
                  if (cmdvalue.find("SET_ALL_CHANS_OFF") != string::npos)
                    {
                      int nchans = PS->GetNumberOfChannels();
                      if ((nchans >= 1) && (nchans <= 12))
                        {
                          for (int ch=1; ch <= nchans; ch++)
                            PS->SetChannelOutputSwitch(ch, 0);
                        }
                    }
                  cmdptrn="SET_GROUP_SWITCH_ON=";
                  if (cmdvalue.find(cmdptrn) != string::npos)
                    {
                      int group = atoi ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                      if (group>=0) PS->SetGroupSwitch(group, 1);
                    }
                  cmdptrn="SET_GROUP_SWITCH_OFF=";
                  if (cmdvalue.find(cmdptrn) != string::npos)
                    {
                      int group = atoi ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                      if (group>=0) PS->SetGroupSwitch(group, 0);
                    }

                  if (cmdvalue.find("GET_SYS_DATA") != string::npos)
                    {
                      wiener.sys.psOperatingTime = PS->GetOperatingTime();
                      wiener.sys.sysUpTime = ((float)(PS->GetUpTime()))/100;
                      wiener.sys.sysStatus = PS->GetSystemStatus();
                      decodeWienerSysStatus(wiener.sys.sysStatus, &(wiener.sys.decoded_status));
                      wiener.sys.sysMainSwitch = PS->GetMainSwitchStatus();
                      if (pthread_mutex_trylock(&CSCMonmutex) != EBUSY)
                        {
                          wiener_services["SYSDATA"]->updateService();
                          pthread_mutex_unlock(&CSCMonmutex);
                        }
                    }


                  if (cmdvalue.find("GET_DATA") != string::npos)
                    {
                      wiener.sys.psOperatingTime = PS->GetOperatingTime();
                      wiener.sys.sysUpTime = ((float)(PS->GetUpTime()))/100;
                      wiener.sys.sysStatus = PS->GetSystemStatus();
                      decodeWienerSysStatus(wiener.sys.sysStatus, &(wiener.sys.decoded_status));
                      wiener.sys.sysMainSwitch = PS->GetMainSwitchStatus();
                      if (pthread_mutex_trylock(&CSCMonmutex) != EBUSY)
                        {
                          wiener_services["SYSDATA"]->updateService();
                          pthread_mutex_unlock(&CSCMonmutex);
                        }
                      for (int ch=0; ch<wiener.chans.size(); ch++)
                        {
                          ostringstream ssch;
                          ssch << "CHAN" << setw(2) << setfill('0') <<ch;
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
                          if (pthread_mutex_trylock(&CSCMonmutex) != EBUSY)
                            {
                              wiener_services[ssch.str()]->updateService();
                              pthread_mutex_unlock(&CSCMonmutex);
                            }
                        }

                    }

                }


              // Handle Wiener Channels related commands
              float value = 0.0;
              if ((cmdname.find("WIENER") != string::npos)
                  && (cmdname.rfind("CHAN") != string::npos))
                {
                  int ch = atoi ((cmdname.substr(cmdname.rfind("CHAN")+4,cmdname.length())).c_str());
                  if (cmdvalue.find("GET_DATA") != string::npos && ch>=0)
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
                      if (pthread_mutex_trylock(&CSCMonmutex) != EBUSY)
                        {
                          wiener_services[ssch.str()]->updateService();
                          pthread_mutex_unlock(&CSCMonmutex);
                        }

                    }

                  ch++;
                  cmdptrn = "SET_CHAN_ON";
                  if (cmdvalue.find("SET_CHAN_ON") != string::npos)
                    {
                      PS->SetChannelOutputSwitch(ch, 1);
                    }
                  if (cmdvalue.find("SET_CHAN_OFF") != string::npos)
                    {
                      PS->SetChannelOutputSwitch(ch, 0);
                    }

                  cmdptrn = "SET_CHAN_OUTPUT_VOLT=";
                  if (cmdvalue.find(cmdptrn) != string::npos)
                    {
                      value = atof ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                      PS->SetChannelOutputVoltage(ch, value);
                    }
                  cmdptrn = "SET_CHAN_CURRENT_LIMIT=";
                  if (cmdvalue.find(cmdptrn) != string::npos)
                    {
                      value = atof ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                      PS->SetChannelCurrentLimit(ch, value);
                    }
                  cmdptrn = "SET_CHAN_MIN_SENSE_VOLT=";
                  if (cmdvalue.find(cmdptrn) != string::npos)
                    {
                      value = atof ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                      PS->SetChannelMinimumSenseVoltage(ch, value);
                    }
                  cmdptrn = "SET_CHAN_MAX_SENSE_VOLT=";
                  if (cmdvalue.find(cmdptrn) != string::npos)
                    {
                      value = atof ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                      PS->SetChannelMaximumSenseVoltage(ch, value);
                    }
                  cmdptrn = "SET_CHAN_MAX_TERMINAL_VOLT=";
                  if (cmdvalue.find(cmdptrn) != string::npos)
                    {
                      value = atof ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                      PS->SetChannelMaximumTerminalVoltage(ch, value);
                    }
                  cmdptrn = "SET_CHAN_MAX_POWER=";
                  if (cmdvalue.find(cmdptrn) != string::npos)
                    {
                      value = atof ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                      PS->SetChannelMaximumPower(ch, value);
                    }
                  cmdptrn = "SET_CHAN_MAX_CURRENT=";
                  if (cmdvalue.find(cmdptrn) != string::npos)
                    {
                      value = atof ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                      PS->SetChannelMaximumCurrent(ch, value);
                    }
                  cmdptrn = "SET_CHAN_GROUP=";
                  if (cmdvalue.find(cmdptrn) != string::npos)
                    {
                      int ivalue = atoi ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                      PS->SetChannelGroup(ch, ivalue);
                    }

                }
              LOG4CPLUS_DEBUG(hvdebug, "Wiener Control executed " << cmdvalue << ". Left in queue: " << (commands_queue.size()-1) );

            }
          commands_queue.pop();
        }
      pthread_mutex_unlock(&LVIOmutex);

      //*** Reset WatchDog timer
      pthread_mutex_lock(&WDmutex);
      watch_wiener_ctl_cnt = 0;
      pthread_mutex_unlock(&WDmutex);
    }
}

void *trdWienerMonitor(void *arg)
{
  int upd_cnt = 0;
  LOG4CPLUS_INFO(hvlog, "Wiener Monitor Thread is started");
  while (1)
    {

      usleep(2*SEC);
      if (fAutoDetect) continue;
      for (int i=0; i<WienerPSs.size(); i++)
        {
          Wiener_PL512* PS = WienerPSs[i];
          if (PS != NULL)
            {
              pthread_mutex_lock(&LVIOmutex);
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

                  if (fFirstRun)
                    {
                      wiener.sys.sysMainSwitch=-1;
                    }
                  else
                    {
                      wiener.sys.sysMainSwitch = PS->GetMainSwitchStatus();
                    }
                  if (fSysUpdate || upd_cnt == WIENER_UPD_DELAY)
                    {
                      if (pthread_mutex_trylock(&CSCMonmutex) != EBUSY)
                        {
                          wiener_services["SYSDATA"]->updateService();
                          pthread_mutex_unlock(&CSCMonmutex);
                        }
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
                      if (fFirstRun)
                        {
                          wiener.chans[ch].out_switch = -1;
                        }
                      else
                        {
                          wiener.chans[ch].out_switch = PS->GetChannelOutputSwitch(ch+1);
                        }
                      wiener.chans[ch].out_volt = PS->GetChannelOutputVoltage(ch+1);
                      /* disabled because Wiener's firmware version 2.1.0.17 doesn't support ChannelAdjustVoltage parameter */
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
                          if (pthread_mutex_trylock(&CSCMonmutex) != EBUSY)
                            {
                              wiener_services[ssch.str()]->updateService();
                              pthread_mutex_unlock(&CSCMonmutex);
                            }
                        }
                    }
                  fFirstRun = false;
                  if (upd_cnt >= WIENER_UPD_DELAY) upd_cnt = 0;
                }
              pthread_mutex_unlock(&LVIOmutex);
            }
        }
      //*** Reset WatchDog timer
      pthread_mutex_lock(&WDmutex);
      watch_wiener_mon_cnt = 0;
      pthread_mutex_unlock(&WDmutex);

    }
}


void *trdUpdatePVSS(void *arg)
{
  LOG4CPLUS_INFO (hvlog, "HV System PVSS Update thread is started");
  while (1)
    {

      usleep(SEC);
      if (fAutoDetect) continue;
      if (fUpdatePVSS)
        {
          // cout << "Update PVSS" << endl;
          updatePVSSdata();
        }
    }
}

void *trdReadDataFromDriver(void *arg)
{
  int cnt=0;
  HVhostcard newdata;
  HVhostcard hvdata;
  bool fStart = true;
  LOG4CPLUS_INFO (hvmonlog, "HV System Monitoring logger is started");
  LOG4CPLUS_INFO (hvmonlog, "HV Data reading thread is started");
  while (1)
    {
      usleep(UPDATE_DELAY);
      cnt += UPDATE_DELAY;
      // Skip if in AutoDetection mode or Processing Rpc requests
      if (fAutoDetect || fRpcProcessing) continue;

      for (int i=0; i<dataservices.size(); i++)
        {
          pthread_mutex_lock(&IOmutex);
	  memset(&hvdata, 0, sizeof(hvdata));
          memset(&newdata, 0, sizeof(newdata));
          memcpy(&hvdata, dataservices[i].data, sizeof(hvdata));
          // HVhostcard * curdata = dataservices[i].data;
          HVhostcard * curdata = &hvdata;
          GetHVData(fd,i,0,0, HV_GET_CARD_DATA, (ULONG*)&newdata,sizeof(struct HVhostcard));
          pthread_mutex_unlock(&IOmutex);
          for (int j=0; j<HVCARD_MAX_MODULES; j++)
            {
              if (curdata->module[j].type != newdata.module[j].type)
                {
                  LOG4CPLUS_WARN (hvmonlog,
                                  "Module at " << i << ":" << j
                                  << " type change from " << modtype_str[curdata->module[j].type]
                                  << " to " <<  modtype_str[newdata.module[j].type] << " detected");
                }
              if (newdata.module[j].type!=NONE)
                {
                  // ==> Status checks and reports
                  if (curdata->module[j].ID != newdata.module[j].ID)
                    LOG4CPLUS_WARN (hvmonlog,
                                    modtype_str[newdata.module[j].type]
                                    << " at " << i << ":" << j
                                    << " ID change from #" << curdata->module[j].ID
                                    << " to #" <<  newdata.module[j].ID << " detected");

                  // if (newdata.module[j].ID == 0 || newdata.module[j].ID>=511) continue;
                  // == Interlock check
                  if (curdata->module[j].interlock != newdata.module[j].interlock)
                    LOG4CPLUS_WARN (hvmonlog,
                                    modtype_str[newdata.module[j].type]
                                    << " ID#" << newdata.module[j].ID
                                    << " at " << i << ":" << j
                                    << " Interlock is " << (newdata.module[j].interlock ? "On": "Off"));

                  // == Module Status Change checks
                  if (curdata->module[j].status != newdata.module[j].status)
                    {
                      switch (newdata.module[j].status)
                        {
                        case HV_STATE_ILOCK:
                          LOG4CPLUS_WARN (hvmonlog,
                                          modtype_str[newdata.module[j].type]
                                          << " ID#" << newdata.module[j].ID
                                          << " at " << i << ":" << j
                                          << " is " << hvstate_str[newdata.module[j].status]);
                          break;
                        case HV_STATE_INT_TRIP:
                          LOG4CPLUS_WARN (hvmonlog,
                                          modtype_str[newdata.module[j].type]
                                          << " ID#" << newdata.module[j].ID
                                          << " at " << i << ":" << j
                                          << " is " << hvstate_str[newdata.module[j].status]);
                          break;
                        default:
                          LOG4CPLUS_DEBUG (hvmonlog,
                                           modtype_str[newdata.module[j].type]
                                           << " ID#" << newdata.module[j].ID
                                           << " at " << i << ":" << j
                                           << " is " << hvstate_str[newdata.module[j].status]);
                        }

                    }

                  // == Channels checks
                  for (int k=0; k<newdata.module[j].num_chans; k++)
                    {
                      ofstream fnTripData;
                      stringstream stfname;
                      string type = (newdata.module[j].type==MASTER)?"MB":"RDB";

                      if (fStart && (newdata.module[j].chan[k].status == HV_STATE_DEAD))
                        {
                          LOG4CPLUS_WARN (hvmonlog,
                                          modtype_str[newdata.module[j].type]
                                          << " ID#" << newdata.module[j].ID
                                          << " at " << i << ":" << j << " Ch" << k+1
                                          << " is " << hvstate_str[newdata.module[j].chan[k].status]);
                        }


                      // == Dump After trip data
                      if ( newdata.module[j].chan[k].status == HV_STATE_CHAN_TRIP
                           && dataservices[i].databuf[j].trip_data[k].flag )
                        {
			  // std::cout << "trip " << i << " " << j << " " << k << " " << newdata.module[j].chan[k].flags << std::endl;
                          if (newdata.module[j].chan[k].flags & 0x1)
                            {

                              HVhistmodule trip_data;
                              memset(&trip_data, 0, sizeof(struct HVhistmodule));
                              GetHVData(fd,i,j,0, HV_GET_MOD_HIST_DATA,
                                        (ULONG*)(&trip_data),sizeof(struct HVhistmodule));
                              stfname << HV_LOGSDIR << "/tripdata_n" << i;

                              stfname << "_a" << j << "_ch" << k+1 << "_" << type << "#"
                              << setw( 3 ) << setfill( '0' )
                              << dataservices[i].data->module[j].ID << "_"
                              << now_fnstr(dataservices[i].databuf[j].trip_data[k].tstamp) << ".dump";

                              fnTripData.open(stfname.str().c_str(), ios::out|ios::app);

                              int trip_marker = trip_data.chan[k].trip_marker;
                              int stop_marker = 0;
                              if (trip_marker < MAX_HIST_BUF_SIZE/2)
                                {
                                  stop_marker = trip_marker + MAX_HIST_BUF_SIZE/2;
                                }
                              else
                                {
                                  stop_marker = trip_marker - MAX_HIST_BUF_SIZE/2;
                                }

                              fnTripData << "\n Trip Marker in buffer " << trip_marker << endl;
                              fnTripData << "#    Vmon Imon" << endl;
                              for (int l=0; l<MAX_HIST_BUF_SIZE; l++)
                                {

                                  int ent = l+stop_marker;
                                  if (ent>=MAX_HIST_BUF_SIZE) ent-=MAX_HIST_BUF_SIZE;
                                  fnTripData << setw( 4 ) << (ent)
                                  << " " << setw(4)
                                  << trip_data.chan[k].vmon[ent]
                                  << " " << setw(4)
                                  << trip_data.chan[k].imon[ent];
                                  if (ent == trip_marker) fnTripData << " *";
                                  fnTripData << endl;
                                }
                              dataservices[i].databuf[j].trip_data[k].flag = false;
                              dataservices[i].databuf[j].trip_data[k].tstamp = 0;
                              fnTripData.close();
                              if (!SaveTripInfoToDB(i,j,k, trip_data.chan[k], HV_STATE_CHAN_TRIP))
                                LOG4CPLUS_WARN(hvlog, "Error saving Trip Info to database");
                            }
                        }

                      // == Dump After trip data
                      if ( (newdata.module[j].chan[k].status == HV_STATE_OV_TRIP)
                           && dataservices[i].databuf[j].trip_data[k].flag
                           && (dataservices[i].databuf[j].trip_data[k].marker==dataservices[i].databuf[j].cnt ) )
                        {
                          // LOG4CPLUS_DEBUG(hvlog, now_fnstr() << " "  << dataservices[i].databuf[j].cnt);
                          if (dataservices[i].databuf[j].trip_data[k].marker==dataservices[i].databuf[j].cnt)
                            dataservices[i].databuf[j].trip_data[k].flag = false;

                          // if (dataservices[i].databuf[j].trip_data[k].fn != "")
                          {
                            if (newdata.module[j].chan[k].status == HV_STATE_OV_TRIP)
                              {
                                stfname << HV_LOGSDIR << "/vtripdata_n" << i;
                              }
                            stfname << "_a" << j << "_ch" << k+1 << "_" << type << "#"
                            << setw( 3 ) << setfill( '0' )
                            << dataservices[i].data->module[j].ID << "_"
                            << now_fnstr(dataservices[i].databuf[j].trip_data[k].tstamp) << ".dump";
                            // fnTripData.open(dataservices[i].databuf[j].trip_data[k].fn.c_str(), ios::out|ios::app);
                            fnTripData.open(stfname.str().c_str(), ios::out|ios::app);
                            for (int l=0; l<DATABUF_DEPTH-1; l++)
                              {
                                int ent = l+dataservices[i].databuf[j].cnt+1;
                                if (ent>=DATABUF_DEPTH) ent-=DATABUF_DEPTH;
                                fnTripData << setw( 4 ) << (l+1+DATABUF_DEPTH)
                                << " " << setw(4)
                                << dataservices[i].databuf[j].vmon[k][ent]
                                << " " << setw(4)
                                << dataservices[i].databuf[j].imon[k][ent]
                                << endl;


                              }
                            dataservices[i].databuf[j].trip_data[k].flag = false;
                            dataservices[i].databuf[j].trip_data[k].tstamp = 0;
                            // dataservices[i].databuf[j].trip_data[k].fn = "";
                            fnTripData.close();
                          }
                        }
                      time_t t;

                      if (curdata->module[j].chan[k].status != newdata.module[j].chan[k].status)
                        {
                          ofstream fnTripData;
                          stringstream stfname;
                          string type = (newdata.module[j].type==MASTER)?"MB":"RDB";

                          switch (newdata.module[j].chan[k].status)
                            {
                            case HV_STATE_CHAN_TRIP:
                              LOG4CPLUS_WARN (hvmonlog,
                                              modtype_str[newdata.module[j].type]
                                              << " ID#" << newdata.module[j].ID
                                              << " at " << i << ":" << j << " Ch" << k+1
                                              << " " << hvstate_str[newdata.module[j].chan[k].status]
                                              << " Vtrip=" << newdata.module[j].chan[k].vtrip << "(ADC)"
                                              << " Itrip=" << newdata.module[j].chan[k].itrip << "(ADC)");
                              // Dump Trip data to file
                              t = time(NULL);
                              stfname << HV_LOGSDIR << "/tripdata_n" << i
                              << "_a" << j << "_ch" << k+1 << "_" << type << "#"
                              << setw( 3 ) << setfill( '0' )
                              << dataservices[i].data->module[j].ID << "_"
                              << now_fnstr(t) << ".dump";
                              dataservices[i].databuf[j].vmon[k][dataservices[i].databuf[j].cnt]= newdata.module[j].chan[k].vmon;
                              dataservices[i].databuf[j].imon[k][dataservices[i].databuf[j].cnt]= newdata.module[j].chan[k].imon;

                              fnTripData.open(stfname.str().c_str(), ios::out);
                              fnTripData << now() << "\n"
                              << modtype_str[newdata.module[j].type]
                              << " ID#" << newdata.module[j].ID
                              << " " << i << ":" << j << " Ch" << k+1
                              << "\nVset=" << newdata.module[j].chan[k].vset << "(DAC)"
                              << " Imax=" << newdata.module[j].chan[k].imax << "(ADC)"
                              << " TripDelay="
                              << newdata.module[j].chan[k].trip_delay << "ms"
                              << "\nVtrip=" << newdata.module[j].chan[k].vtrip << "(ADC)"
                              << " Itrip=" << newdata.module[j].chan[k].itrip << "(ADC)";
                              /*
                                                << "\nRing buffer data dump (" << DATABUF_DEPTH
                                                << " entries)"
                                                << "\nTrip detected at entry #"
                                                << dataservices[i].databuf[j].cnt+1
                              */
                              // << "\n#    Vmon Imon" << endl;

                              // == Set Trip Marker for later dumping of additional data after the trip
                              dataservices[i].databuf[j].trip_data[k].flag = true;
                              dataservices[i].databuf[j].trip_data[k].tstamp = t;
                              //	LOG4CPLUS_DEBUG(hvlog, now_fnstr() << " "  << dataservices[i].databuf[j].trip_data[k].marker);
                              // dataservices[i].databuf[j].trip_data[k].fn = stfname.str();

                              fnTripData.close();
                              break;
                            case HV_STATE_OV_TRIP:
                              LOG4CPLUS_WARN (hvmonlog,
                                              modtype_str[newdata.module[j].type]
                                              << " ID#" << newdata.module[j].ID
                                              << " at " << i << ":" << j << " Ch" << k+1
                                              << " " << hvstate_str[newdata.module[j].chan[k].status]
                                              << " Vtrip=" << newdata.module[j].chan[k].vtrip << "(ADC)"
                                              << " Itrip=" << newdata.module[j].chan[k].itrip << "(ADC)");
                              // Dump Trip data to file
                              t = time(NULL);
                              stfname << HV_LOGSDIR << "/vtripdata_n" << i
                              << "_a" << j << "_ch" << k+1 << "_" << type << "#"
                              << setw( 3 ) << setfill( '0' )
                              << dataservices[i].data->module[j].ID << "_"
                              << now_fnstr(t) << ".dump";
                              dataservices[i].databuf[j].vmon[k][dataservices[i].databuf[j].cnt]= newdata.module[j].chan[k].vmon;
                              dataservices[i].databuf[j].imon[k][dataservices[i].databuf[j].cnt]= newdata.module[j].chan[k].imon;

                              fnTripData.open(stfname.str().c_str(), ios::out);
                              fnTripData << now() << "\n"
                              << modtype_str[newdata.module[j].type]
                              << " ID#" << newdata.module[j].ID
                              << " " << i << ":" << j << " Ch" << k+1
                              << "\nVset=" << newdata.module[j].chan[k].vset << "(DAC)"
                              << "\nVset_adc=" << newdata.module[j].chan[k].vset_adc
                              << " Vov=" << newdata.module[j].chan[k].vov
                              << " OVTripDelay="
                              << newdata.module[j].chan[k].vtrip_delay << "ms"
                              << "\nVtrip=" << newdata.module[j].chan[k].vtrip << "(ADC)"
                              << " Itrip=" << newdata.module[j].chan[k].itrip << "(ADC)"
                              /*
                                                << "\nRing buffer data dump (" << DATABUF_DEPTH
                                                << " entries)"
                                                << "\nTrip detected at entry #"
                                                << dataservices[i].databuf[j].cnt+1
                              */
                              << "\n#    Vmon Imon" << endl;
                              for (int l=0; l<DATABUF_DEPTH; l++)
                                {
                                  int ent = l+dataservices[i].databuf[j].cnt+1;
                                  if (ent>=DATABUF_DEPTH) ent-=DATABUF_DEPTH;
                                  // fnTripData << ((l==dataservices[i].databuf[j].cnt)
                                  fnTripData << setw( 4 ) << l+1
                                  << " " << setw(4)
                                  << dataservices[i].databuf[j].vmon[k][ent]
                                  << " " << setw(4)
                                  << dataservices[i].databuf[j].imon[k][ent]
                                  << ((l==DATABUF_DEPTH-1)?" *":"")
                                  << endl;
                                }

                              // == Set Trip Marker for later dumping of additional data after the trip
                              dataservices[i].databuf[j].trip_data[k].marker = dataservices[i].databuf[j].cnt;
                              dataservices[i].databuf[j].trip_data[k].flag = true;
                              dataservices[i].databuf[j].trip_data[k].tstamp = t;
                              // dataservices[i].databuf[j].trip_data[k].fn = stfname.str();

                              fnTripData.close();
                              break;

                            case HV_STATE_DEAD:
                              LOG4CPLUS_WARN (hvmonlog,
                                              modtype_str[newdata.module[j].type]
                                              << " ID#" << newdata.module[j].ID
                                              << " at " << i << ":" << j << " Ch" << k+1
                                              << " " << hvstate_str[newdata.module[j].chan[k].status]);
                              break;
                            case HV_STATE_ILOCK:
                              LOG4CPLUS_WARN (hvmonlog,
                                              modtype_str[newdata.module[j].type]
                                              << " ID#" << newdata.module[j].ID
                                              << " at " << i << ":" << j << " Ch" << k+1
                                              << " " << hvstate_str[newdata.module[j].chan[k].status]);
                              break;
                            default:
                              if (curdata->module[j].chan[k].status == HV_STATE_DEAD)
                                {
                                  LOG4CPLUS_WARN (hvmonlog,
                                                  modtype_str[newdata.module[j].type]
                                                  << " ID#" << newdata.module[j].ID
                                                  << " at " << i << ":" << j << " Ch" << k+1
                                                  <<" changed status from "
                                                  << hvstate_str[curdata->module[j].chan[k].status]
                                                  << " to " << hvstate_str[newdata.module[j].chan[k].status]);
                                }
                            }

                        }
                    }
                }

            }
          pthread_mutex_lock(&IOmutex);
          memcpy(dataservices[i].data, &newdata, sizeof(struct HVhostcard));
          pthread_mutex_unlock(&IOmutex);
        }
      fStart=false;
    }
}


void *trdSaveDataToFile(void *arg)
{
  struct stat sts;
  int cnt=0;
  HVhostcard newdata;
  LOG4CPLUS_INFO (hvmonlog, "HV Data saving thread is started");
  while (1)
    {
      usleep(UPDATE_DELAY);
      cnt += UPDATE_DELAY;
      if (fAutoDetect) continue;
      for (int i=0; i<dataservices.size(); i++)
        {
          pthread_mutex_lock(&IOmutex);
          memcpy(&newdata, dataservices[i].data, sizeof(struct HVhostcard));
          pthread_mutex_unlock(&IOmutex);
          for (int j=0; j<HVCARD_MAX_MODULES; j++)
            {
              if (isValidModule( newdata.module[j].type,  newdata.module[j].ID))
                {
                  for (int k=0; k<newdata.module[j].num_chans; k++)
                    {
                      dataservices[i].databuf[j].vmon[k][dataservices[i].databuf[j].cnt]= newdata.module[j].chan[k].vmon;
                      dataservices[i].databuf[j].imon[k][dataservices[i].databuf[j].cnt]= newdata.module[j].chan[k].imon;
                    }
                  dataservices[i].databuf[j].cnt++;

                  // ==> Handle file open/close operations
                  int write_flag = newdata.module[j].policy & POLICY_WRITE_FILE;
                  if (write_flag>0)
                    {
                      if (write_flag != dataservices[i].flog[j])
                        {
                          dataservices[i].databuf[j].cnt=0;
                          PrepareDataFileName(i, j);
                          if ( dataservices[i].ilog[j] == NULL )
                            dataservices[i].ilog[j] = new ofstream();
                          if ( dataservices[i].vlog[j] == NULL )
                            dataservices[i].vlog[j] = new ofstream();

                          dataservices[i].vlog[j]->open(dataservices[i].vlogname[j].c_str(), ios::out );
                          dataservices[i].ilog[j]->open(dataservices[i].ilogname[j].c_str(), ios::out );
                        }

                    }
                  else
                    {
                      if (write_flag != dataservices[i].flog[j])
                        {
                          if ( ( dataservices[i].vlog[j] != NULL ) &&
                               ( dataservices[i].vlog[j]->is_open() ) )
                            dataservices[i].vlog[j]->close();
                          if ( ( dataservices[i].ilog[j] != NULL ) &&
                               ( dataservices[i].ilog[j]->is_open() ) )
                            dataservices[i].ilog[j]->close();
                        }
                    }
                  dataservices[i].flog[j] = write_flag;



                  // ==> Save data to file
                  if ((dataservices[i].flog[j]>0)
                      && (dataservices[i].ilog[j]!=NULL)
                      && (dataservices[i].vlog[j]!=NULL)
                      && dataservices[i].ilog[j]->is_open()
                      && dataservices[i].vlog[j]->is_open()
                      && dataservices[i].databuf[j].cnt>=DATABUF_DEPTH)
                    {

                      if (stat(dataservices[i].vlogname[j].c_str(), &sts) == -1
                          && errno == ENOENT)
                        {
                          dataservices[i].vlog[j]->close();
                          dataservices[i].vlog[j]->open(dataservices[i].vlogname[j].c_str(), ios::out);
                        }
                      if (stat(dataservices[i].ilogname[j].c_str(), &sts) == -1
                          && errno == ENOENT)
                        {
                          dataservices[i].ilog[j]->close();
                          dataservices[i].ilog[j]->open(dataservices[i].ilogname[j].c_str(), ios::out);
                        }
                      stringstream v_st, i_st;
                      for (int l=0; l<DATABUF_DEPTH; l++)
                        {
                          for (int k=0; k<newdata.module[j].num_chans; k++)
                            {
                              * dataservices[i].vlog[j]
                              << dataservices[i].databuf[j].vmon[k][l] << " ";
                              * dataservices[i].ilog[j]
                              << dataservices[i].databuf[j].imon[k][l] << " ";
                            }
                          * dataservices[i].vlog[j] << "\n";
                          * dataservices[i].ilog[j] << "\n";
                        }
                      dataservices[i].vlog[j]->flush();
                      dataservices[i].ilog[j]->flush();
                    } // <== Save data to file

                  if (dataservices[i].databuf[j].cnt>=DATABUF_DEPTH)
                    dataservices[i].databuf[j].cnt=0;
                }
            }
        }

    }
}

bool isValidModule(int mod_type, int ID)
{
  switch (mod_type)
    {
    case NONE:
      return false;
    case MASTER:
      return (ID>0 && ID<64)?true:false;
    case RDB30:
      return (ID>255 && ID<511)?true:false;
    case RDB36:
      return (ID>0 && ID<256)?true:false;
    default:
      return false;
    }
  return false;
}

void PrepareDataFileName(int hcard, int mod)
{
  int i=hcard;
  int j=mod;
  stringstream stfname;
  string type = "NONE";
  if (isValidModule(dataservices[i].data->module[j].type, dataservices[i].data->module[j].ID))
    {
      switch ( dataservices[i].data->module[j].type )
        {
        case MASTER:
          type = "MB";
          break;
        case RDB30:
          type = "DS";
          break;
        case RDB36:
          type = "DL";
          break;
        }

      stfname << HV_LOGSDIR << "/vmon_n" << i << "_a" << j << "_"
      << type << setw( 3 ) << setfill( '0' )
      << dataservices[i].data->module[j].ID << ".dat";
      stfname >> dataservices[i].vlogname[j];
      stfname.clear();
      stfname << HV_LOGSDIR << "/imon_n" << i << "_a" << j << "_"
      << type << setw( 3 ) << setfill( '0' )
      << dataservices[i].data->module[j].ID << ".dat";
      stfname >> dataservices[i].ilogname[j];
    }

}

