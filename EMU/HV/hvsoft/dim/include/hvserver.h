#ifndef HVSERVER_H
#define HVSERVER_H
#include <sstream>
#include <iomanip>
#include <queue>
#include <mysql++.h>
#include <ssqls.h>
// #include <custom.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <semaphore.h>

#include <dis.hxx>

#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/socketappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/tstring.h>

#include "hvdim.h"
#include "pvss.h"
#include "hvwiener.h"


using namespace log4cplus;

// HV Dim Server class declaration
class HVDimServer: public DimServer
{
public:
  HVDimServer(string servername);
private:
  vector<DimCommand *>commands;
  map<string,int> cmdlist;
  string sname;
  void commandHandler();
};

// Error handler for HVDimServer declaration
class dimHVErrHandler : public DimErrorHandler
{
public:
  void errorHandler(int severity, int code, char *msg);
};

// DimRpc service declaration for module operation mode
class RpcRealData : public DimRpc
{
  void rpcHandler();
  int pvssInterface(HVboard& realcard, HVcmd *cmd, int mode); // mycorr++
  string rpcname;
  string servername;
public:
  RpcRealData(string sname, string rname): DimRpc((sname+rname).c_str(),"C","C"),rpcname(sname+rname), servername(sname) {};
};

// DimRpc service declaration for Primary PS control
class RpcPrimeHV : public DimRpc
{
  void rpcHandler();
  int pvssInterface(HVPrimary *data, HVcmd *cmd, int mode); // mycorr++
  string rpcname;
  string servername;
public:
  RpcPrimeHV(string sname, string rname): DimRpc((sname+rname).c_str(),"C","C"),rpcname(sname+rname), servername(sname) {};
};

// DimRpc service declaration for CSC operation mode
class RpcCSCData : public DimRpc
{
  void rpcHandler();
  string rpcname;
  string servername;
public:
  RpcCSCData(string sname, string rname): DimRpc((sname+rname).c_str(),"C","C"),rpcname(sname+rname), servername(sname) {};
};


// DimRpc service declaration for MySQL configuration database access mode
class RpcMySQL : public DimRpc
{
  void rpcHandler();
  string rpcname;
  string servername;
public:
  RpcMySQL(string sname, string rname): DimRpc((sname+rname).c_str(),"C","C"),rpcname(sname+rname), servername(sname) {};
};

class RpcHVStatus : public DimRpc
{
  void rpcHandler();
  string rpcname;
  string servername;
public:
  RpcHVStatus(string sname, string rname): DimRpc((sname+rname).c_str(),"C","C"),rpcname(sname+rname), servername(sname) {};
};

class PvssCommand : public DimCommand
{
  string command_name;
  void commandHandler();
public:
  PvssCommand(char *cmd_name):DimCommand(cmd_name,"C"),command_name(cmd_name) {};
};

// Generic HV Update timer definition based on DimTimer
class HVUpdateTimer : public DimTimer
{
protected:
  struct timeval tstamp;
  int delay;
  bool fStart;
public:
  HVUpdateTimer(): DimTimer() {};
  HVUpdateTimer(int time)
  {
    delay = time;
    fStart = false;
    gettimeofday(&tstamp, NULL);
  }

  HVUpdateTimer(int time, bool start)
  {
    delay = time;
    fStart = start;
    if (fStart)
      {
        gettimeofday(&tstamp, NULL);
        DimTimer::start(time);
      }
  };
  void pause()
  {
    fStart = false;
  }
  void stop()
  {
    fStart = false;
    DimTimer::stop();
  };
  void start()
  {
    fStart = true;
    DimTimer::start(delay);
  };
  virtual void timerHandler()=0;
};


typedef pair<int, HVcmd> Module;
typedef map<int, HVcmd> ModulesList;
typedef map<Module, ModulesList> ModuleLinksList;
typedef map<Module, ModuleLinksList> ModuleLinksTree;

extern string now(time_t tstamp, const char* format= "%Y-%m-%d %H:%M:%S");
extern string now();	// Returns current time string
extern void print_ds_entry(struct HVdataservice ds);
extern string MODULE_STR(const HVcmd& mod);
extern string MODULE_STR(int card, int addr);
extern string PRIMARY_STR(const HVPrimary *p);
// Read HV data from driver
extern int GetHVData(int fd, UCHAR card, UCHAR module, UCHAR chan, HV_CMD cmd, ULONG *data, ULONG size);
extern int AutoDetect();			// Modules configuration auto detection
extern int DetectModules();
extern int LoadModulesCalibrations();
extern int GetModuleID(const HVcmd& mod);
extern int ResetDriver();
extern int ReadHVData();
extern int ResetAllPrimaries(bool disableOutput);
extern int ResetPrimary(int port, int addr, int volt, float imax, bool disableOutput);
extern int ResetPrimary(HVPrimary* p, int volt, float imax, bool disableOutput);

// extern int ResetModule(const Module& mod);
extern int ResetModule(const HVcmd& mod);
extern HVcmd SetPrimaryLink(HVcmd& master, HVPrimary* h);
extern HVcmd SetMasterLink(HVcmd& rdb, HVcmd& master,int chan );
extern int SetModule(HVcmd& mod, int vset, int chan);
extern int ResetAllModules(ModulesList& mods);
extern int ResetAllModules(vector<HVcmd>& mods);
extern int SetModuleDefaultParameters(const HVcmd& mod);
extern int SetDefaultParameters(vector<HVcmd>& mods);
extern int SetDefaultParameters(ModulesList& mods);
extern int DetectMasterLinks();
extern void ShowPrimaryMap();
extern int SaveConfig(string cfgname);
extern int LoadConfig(string cfgname);
extern int SaveRealConfig(string cfgname); // Save Real Voltages currents
extern int LoadRealConfig(string cfgname); // Load Real Voltages currents
extern int ReadPrimaryConfig(int hostid);	// Read Primary PS configuration
extern bool LoadADCCalibration(int card, int module);
extern bool LoadDACCalibration(int card, int module);
extern bool LoadCurrentCalibration(int card, int module);
extern bool LoadADCCalibrationFromDB(int card, int module);
extern bool LoadDACCalibrationFromDB(int card, int module);
extern bool LoadCurrentCalibrationFromDB(int card, int module);
extern bool SaveCalibrationToDB(int card, int module);
extern bool LoadCalibration(int card, int module);	// Load calibration data for module
extern bool LoadCSCMap();				// Load CSC to RDB mapping
extern bool InitCSCMonData();
extern void InitCSCMonUpdateParameters(HVcscMon& cscMon);
extern bool UpdateCSCMonData(std::string cscID, HVcscMon& cscMon);
extern bool isFastMonUpdateNeeded(HVcscMon& newData, HVcscMon& curData);
extern bool isSlowMonUpdateNeeded(HVcscMon& newData, HVcscMon& curData);
extern bool InitPrimaryMonData();
extern bool UpdatePrimaryMonData(std::string primaryID, HVprimaryMon& primaryMon);
extern bool isPrimaryMonUpdateNeeded(HVprimaryMon& newData, HVprimaryMon& curData);
extern bool ParseCSC(string csc_name , HVcsc *csc);	// Parse CSC name string into HVcsc structure
extern int UpdateDataDB();				// Update current CSC HV data in MySQL
extern int UpdateDataDim();				// Update module "raw" data mode DimServices (obsolete)
extern int UpdateConfigDatabase(int hostid);		// Update configuration database
extern bool SaveTripInfoToDB(int card, int module, int chan, HVhistchannel& trip_data, int trip_type); 
extern void* trdUpdatePrimaryControl(void *);
extern void UpdatePrimaryStatus(HVPrimary*);
extern void CheckForTrips();				// Check for trips in modules and turn off Primary PS in case of longtime trips
extern bool isValidModule(int mod_type, int ID);
extern bool IsPrimaryExists( int port, int addr );
extern void* doCalibration(void *);				// Perform ADC and DAC calibration
extern int getRealHVData(HVcmd* cmd, HVboard& board);
extern int processRealHVCommand(HVcmd* cmd);
extern int processPrimaryHVCommand(HVcmd* cmd);
extern int processCSCHVCommand(HVcsccmd* csccmd);
extern int pvssInterfacePrimaryHVData( HVcmd * cmd);
extern int pvssInterfaceRealHVData(HVcmd * cmd);
extern int pvssChannelCopy(int i, pvss_module_summary *pvss_l, HVboard * realcard);


extern string servername;
extern ULONG cmd_cntr;
extern int fd;
extern int hcHostID;	// Host ID read from driver
extern int hcFound;
extern int hcMax;
extern bool fMaster, fNode, fPrimary, fFirstRun;
extern bool fAutoDetect, fFastDetect, fLoadConfig, fCSCMap, fRpcProcessing;
extern bool fLockDimData;
extern bool fForceCalibrationUpdate, fControlWienerPL512, fDoCalibration, fCalibrateOnlyTurnedOn;
extern DimService* svc_hcFound;	// DimService for reading number of available on computer Host Cards
extern DimService* svc_logger; // Logger service
extern DimService* svc_srvStatus;
extern DimService* svc_hvGlobalState;
extern DimCommand* pvss_cmdControl;
extern char* srvStatus;
extern string log_msg;		// Log message string
extern string 		PRIMARY_DNS_NODE;
extern int 		PRIMARY_DNS_PORT;
extern int watch_cnt;
extern int csc_mon_cnt_freq;
extern unsigned long PVSS_UPDATE_INTERVAL;
extern unsigned long PVSS_VMON_THRESHOLD;
extern unsigned long PVSS_IMON_THRESHOLD;
extern HV_STATE GlobalState;

extern vector<HVdataservice> dataservices;	// vector for internal HVdataservice data for available Host Cards
extern string logname, cfgname;

extern pthread_mutex_t IOmutex;
extern pthread_mutex_t PrimeIOmutex;
extern pthread_mutex_t CmdIOmutex;
extern pthread_mutex_t LVIOmutex;
extern pthread_mutex_t WDmutex;
extern pthread_mutex_t CSCMonmutex;
extern int lock_id;

extern vector<HVhostcard *> cards;
extern vector<DimService *> services;
// extern vector <DimInfo *> nodesdata;
extern map<string, vector<DimService *> >nodes;
extern map<int, map<int, HVPrimary*> > primaries;
extern vector<HVcmd>masters;
extern vector<HVcmd>dbs;
extern map<int, HVcmd>masters_map;
extern map<int, HVcmd>dbs_map;
extern map<HVPrimary*,vector<HVcmd> > primaries_map;
extern map<string, pair<int, int> > csc_map;
// extern std::map<std::string, std::list<HVcmdDIM> > commands_queue;
extern std::queue< std::pair<std::string, std::string> > commands_queue;
extern std::queue< std::string > cscdata_updates_queue; 
extern map<string, HVcscMon> cscMonData;
extern map<string, HVprimaryMon> primaryMonData;

extern map<std::string, DimService*> wiener_services;
extern map<std::string, DimCommand*> wiener_commands;
extern dimWienerPS wiener;

extern Logger hvlog; 		// log4cplus Server Logger
extern Logger hvmonlog;		// log4cplus HV System Monitoring Logger
extern Logger hvtiminglog;
extern Logger hvcmdlog;
extern Logger hvcaliblog;
extern Logger hvdebug;

// MySQL database update timer
class HVDataBaseUpdateTimer : public HVUpdateTimer
{
public:
  HVDataBaseUpdateTimer() : HVUpdateTimer() {};
  HVDataBaseUpdateTimer(int time) : HVUpdateTimer(time) {};
  HVDataBaseUpdateTimer(int time, bool start) : HVUpdateTimer(time, start) {};
  void timerHandler();
};

// module "raw" data service update timer
class HVDimDataUpdateTimer : public HVUpdateTimer
{
public:
  HVDimDataUpdateTimer() : HVUpdateTimer() {};
  HVDimDataUpdateTimer(int time) : HVUpdateTimer(time) {};
  HVDimDataUpdateTimer(int time, bool start) : HVUpdateTimer(time, start) {};

  void timerHandler();
};

// Primary PS control update timer
class HVPrimaryControlUpdateTimer : public HVUpdateTimer
{
public:
  HVPrimaryControlUpdateTimer() : HVUpdateTimer() {};
  HVPrimaryControlUpdateTimer(int time) : HVUpdateTimer(time) {};
  HVPrimaryControlUpdateTimer(int time, bool start) : HVUpdateTimer(time, start) {};

  void timerHandler();
};

extern HVPrimaryControlUpdateTimer* PrimaryControlUpdateTimer;

#endif
