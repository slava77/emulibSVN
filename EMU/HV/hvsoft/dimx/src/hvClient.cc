#include <dic.hxx>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <ncurses.h>
#include <sys/time.h>

#include "hvdim.h"
#include "hvprimary.h"

using namespace std;

static int fRealData=1;
static int fPrimary=0;
static int fCSCMode=0;
static int fRDBMode=0;
string cscname = "";
string rdbname = "";
string servername = HV_DIM_NAME;

void usage()
{
  printf("Usage: hvClient -[options]\n");
  printf("Options:\n");
  printf("\t-?\tshow this usage info\n");
  printf("\t-hN\tset computer host ID (default - use first matching name DIM server)\n");
  printf("\t-nN\tset hostcard address (default 0)\n");
  printf("\t-aN\tset module address N (0..15)\n");
  printf("\t-bN\tset module type to (0-NONE, 1-MASTER, 2-RDB30, 3-RDB36, 4-MON30, 5-MON36)\n");
  printf("\t-cN\tset modules channel (N > 36 set ALL channels mode)\n");
  printf("\t-oN\tset module state (0-OFF, 1-ON)\n");
  printf("\t-sN\tset module state or channel state (with -c) (0-OFF, 1-ON)\n");
  printf("\t-x\tturn all modules on all hostcards Off\n");
  printf("\t-r\treset trip state condition\n");
  printf("\t-vN\tset voltage\n");
  printf("\t-tN\tset maximum trip current level\n");
  printf("\t-RN\tset maximum trip current level for ramp up\n");
  printf("\t-iN\tset trip reset delay in msec\n");
  printf("\t-mN\tset max trip resets attempts\n");
  printf("\t-jN\tset trip delay in msec\n");
  printf("\t-kN\tset linked master trip delay in msec\n");
  printf("\t-yN\tset maximum voltage level in counts (0..4000)\n");
  printf("\t-uN\tset ramp up speed\n");
  printf("\t-dN\tset ramp down speed\n");
  printf("\t-fN\tset relay state for master board (0-OFF, >0-ON\n");
  printf("\t-qN\tset interlock delay in ms)\n");
  printf("\t-VN\tset overvoltage limit\n");
  printf("\t-JN\tset overvoltage trip delay in msec\n");
  printf("\t-z\tdisable calibration data, raw DAC-ADC mode\n");
  printf("\t-wN\tsaving of data from module in file mode (0-OFF, >0-ON\n");
  printf("\t-p\tdisplay monitored values from modules\n");
  printf("\t-g\tset Primary Supply Control mode\n");
  printf("\t-A\tforce hostcards module detection and re-configuration\n");
  printf("\t-C\treload Calibration data from file\n");
  printf("\t-Ttype\tforce module type filter (type: RDBALL, RDB30, RDB36, MASTER)\n");
  printf("\t-D\tperform RDB modules ADC and DAC voltage calibration\n");
  printf("\t-Ucfgname\tsave system configuartion into database\n");
  printf("\t-Lcfgname\tload system configuartion from database\n");
  printf("\t-B\tperform DIM benchmarking\n");
  printf("\t-FN\tset maximum channel DEAD state transitions before trip\n");
  printf("\t-PN\tPVSS Update Interval in sec\n");
  printf("\t-IN\tPVSS Imon update threshold in n/11 mkA\n");
  printf("\t-KN\tPVSS Vmon update threshold in V\n");
};

string now()
{
  char buf[255];
  time_t now=time(NULL);
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
  string time = string(buf);
  return time.substr(0,time.find("\n",0));

};


int send_hvcmd_rpc(UCHAR card, UCHAR module, UCHAR chan, HV_CMD cmd, ULONG data, ULONG size)
{
  int res=0;
  HVcmdDIM hvcmd;
  hvcmd.data.card = card;
  hvcmd.data.module = module;
  hvcmd.data.chan = chan;
  hvcmd.data.cmd = cmd;
  hvcmd.data.data = data;
  hvcmd.data.size = size;

  string name = servername + "/" + string(hvcmd_str[cmd]);
  if (fCSCMode)
    {
      HVcsccmdDIM csccmdDIM;
      HVcsccmd csccmd;
      memset(&csccmdDIM, 0, sizeof(csccmdDIM));
      memset(&csccmd, 0, sizeof(csccmd));
      memcpy(csccmd.csc, cscname.c_str(), sizeof(csccmd.csc));
      csccmd.cmd = cmd;
      csccmd.chan = chan;
      csccmd.data = data;
      csccmd.size = size;
      DimRpcInfo rpc(string(servername+"/HV_CSC_CMD").c_str(), -1);
      csccmdDIM.ID = rpc.getId();
      memcpy(&csccmdDIM.data, &csccmd, sizeof(csccmd));
      rpc.setData((void *) &csccmdDIM, sizeof(csccmdDIM));
    }
  else
    {
      if (fPrimary)
        {
          DimRpcInfo rpc(string(servername+"/HV_PRIMARY_CMD").c_str(), -1);
          hvcmd.ID = rpc.getId();
          rpc.setData((void *) &hvcmd, sizeof(hvcmd));
        }
      else
        {
          if (fRealData)
            {
              DimRpcInfo rpc(string(servername+"/HV_REAL_CMD").c_str(), -1);
              hvcmd.ID = rpc.getId();
              rpc.setData((void *) &hvcmd, sizeof(hvcmd));
            }
          else
            {
              DimRpcInfo rpc(string(servername+"/HV_RAW_CMD").c_str(), -1);
              hvcmd.ID = rpc.getId();
              rpc.setData((void *) &hvcmd, sizeof(hvcmd));
              // DimClient::sendCommand(name.c_str(), &hvcmd.data, sizeof(hvcmd.data));
            }
        }
    }
  // res = ioctl(fd, HVCARD_PCIE_IOXCMD, &hvcmd);
  return res;

};

int send_hvcmd(UCHAR card, UCHAR module, UCHAR chan, HV_CMD cmd, ULONG data, ULONG size)
{
  int res=0;
  HVcmdDIM hvcmd;
  hvcmd.data.card = card;
  hvcmd.data.module = module;
  hvcmd.data.chan = chan;
  hvcmd.data.cmd = cmd;
  hvcmd.data.data = data;
  hvcmd.data.size = size;

  string name = servername + "/" + string(hvcmd_str[cmd]);
  if (fCSCMode)
    {
      HVcsccmdDIM csccmdDIM;
      HVcsccmd csccmd;
      memset(&csccmdDIM, 0, sizeof(csccmdDIM));
      memset(&csccmd, 0, sizeof(csccmd));
      memcpy(csccmd.csc, cscname.c_str(), sizeof(csccmd.csc));
      csccmd.cmd = cmd;
      csccmd.chan = chan;
      csccmd.data = data;
      csccmd.size = size;

      // memcpy(&csccmdDIM.data, &csccmd, sizeof(csccmd));
      // DimClient::sendCommand(string(servername+"/HV_CSC_CMD/RpcIn").c_str(),&csccmdDIM, sizeof(csccmdDIM));

      char stc[100];
      sprintf(stc, "500|HVCSCCMD;%s;%d;%d;%d;%d", cscname.c_str(), chan, cmd, data,size);
      string cmdstr = stc;
      DimClient::sendCommand("HV_1_COMMAND", (char *)cmdstr.c_str(), cmdstr.size()+1);

    }
  else
    {
      if (fPrimary)
        {
//      DimRpcInfo rpc(string(servername+"/HV_PRIMARY_CMD").c_str(), -1);
//      hvcmd.ID = rpc.getId();
//     rpc.setData((void *) &hvcmd, sizeof(hvcmd));
          DimClient::sendCommand(string(servername+"/HV_PRIMARY_CMD/RpcIn").c_str(), &hvcmd, sizeof(hvcmd));
        }
      else
        {
          if (fRealData)
            {
//        DimRpcInfo rpc(string(servername+"/HV_REAL_CMD").c_str(), -1);
//        hvcmd.ID = rpc.getId();
//        rpc.setData((void *) &hvcmd, sizeof(hvcmd));
              DimClient::sendCommand(string(servername+"/HV_REAL_CMD/RpcIn").c_str(), &hvcmd, sizeof(hvcmd));
            }
          else
            {
//        DimRpcInfo rpc(string(servername+"/HV_RAW_CMD").c_str(), -1);
//        hvcmd.ID = rpc.getId();
//        rpc.setData((void *) &hvcmd, sizeof(hvcmd));
              // DimClient::sendCommand(name.c_str(), &hvcmd.data, sizeof(hvcmd.data));
              DimClient::sendCommand(string(servername+"/HV_RAW_CMD/RpcIn").c_str(), &hvcmd, sizeof(hvcmd));
            }
        }
    }
  // res = ioctl(fd, HVCARD_PCIE_IOXCMD, &hvcmd);
  return res;

};


class dimHVErrHandler : public DimErrorHandler
{
public:
  void errorHandler(int severity, int code, char *msg)
  {
    cout << msg << endl;
    string DNSNode = DimClient::getDnsNode();
    int DNSPort = DimClient::getDnsPort();
    switch (code)
      {
      case DIMDNSCNERR:
        cout << "DNS at " << DNSNode << ":" <<DNSPort << " is not responding" << endl;
        /*
                if (DNSPort != BACKUP_DNS_PORT)
                  {
                    cout << "connecting to backup DNS " << BACKUP_DNS_NODE << ":" << BACKUP_DNS_PORT << endl;
                    DimClient::setDnsNode(BACKUP_DNS_NODE, BACKUP_DNS_PORT);
                  }
        */
        break;
      }
  }
};

int main(int argc, char **argv)
{
  char *server, *ptr, *ptr1;
  DimClient::addErrorHandler(new dimHVErrHandler());
  DimBrowser br;
  int rdb=0xE, mst=0xF;
  int svctype, addr=ALL, type=NONE, ch=ALL, hostid=0;
  HVhostcard card;
  HVboard realcard;
  HVcscdata cscdata;
  HVcsccmd csccmd;
  HVPrimary primecard;
  HVcmd cmd;

  HVboardDIM realcardDIM;
  HVcscdataDIM cscdataDIM;
  HVcsccmdDIM csccmdDIM;
  HVPrimaryDIM primecardDIM;
  HVcmdDIM cmdDIM;

  vector<int> chans;
  vector<string> servers;
  string str="";
  int cardaddr=ALL, state=HV_STATE_OFF, rup=0, rdn=0;
  int vset=0, imax=1000, vmax=4000, relay=0, imax_ramp=1000;
  int value=0, mvalue = 0, savedata=0, tripdelay = DEF_TRIP_DELAY ;
  int master_tripdelay = DEF_MASTER_TRIP_DELAY;
  int tripresetdelay = DEF_CHAN_TRIP_RESET_DELAY;
  int maxtripresets = DEF_CHAN_MAX_TRIP_RESETS;
  int ilock_delay = DEF_ILOCK_DELAY;
  int overvolt = DEF_CHAN_OV_LIMIT;
  int modaddr = ALL;
  int vtripdelay = DEF_OV_TRIP_DELAY;
  int dead_max_cnt = DEF_CHAN_DEAD_MAX_CNT;
  int pvss_update_interval = 30;
  int pvss_vmon_thresh = 30;
  int pvss_imon_thresh = 6;
  char *fn="";
  int  fWrite=0, fChan=0, fMaster=0, fRelay=0, fSet = 0, fShow = 0, fReset=0, fTripDelay=0;
  int fType=0, fState=0, fRampUp=0, fRampDown=0, fVset=0, fImax=0, fVmax=0, fAllOff=0;
  int fVCalib=0, fICalib=0, fModState=0, fLogOpen=0, fSaveData=0, fSetMaster=0; //, fRealData=1;
  int fMasterTripDelay=0, fTripResetDelay=0, fTripMaxResets = 0, fILockDelay = 0, fHostID = 0;
  int fForceAutoDetect=0, fHVCSCStatus=0, fHVModuleStatus=0, fTypeFilter = 0;
  int fSetOvervoltage=0, fOVTripDelay = 0;
  int fUpdateCalibration=0, fDoCalibration=0, calibMode=0, fDeadMaxCnt=0;
  int fSaveConfig=0, fLoadConfig=0, fRunDimBenchmark = 0, fImaxRamp=0;
  int fPVSS_UpdateInterval=0, fPVSS_ImonThresh=0, fPVSS_VmonThresh=0;
  int fReadHistData=0;
  int fWienerTest=0;
  std::string cfgName="";

  int found = 0;
  unsigned long masteraddr = 0;
  int updatetime=2000000;
  int nCommands=2000;
  ofstream vlogfile;
  ofstream ilogfile;
  stringstream sname;
  int (*pf)(int)=toupper;

  //	string cscname = "";
  //	string rdbname = "";
  /*
    br.getServices("*");
    while((svctype = br.getNextService(ptr, ptr1))!= 0)
    {
    cout << "type = " << svctype << " - " << ptr << " " << ptr1 << endl;
    }
  */

  /*
  std::cout << "HVcsccmdDIM " << sizeof(HVcsccmdDIM) <<  std::endl;
  std::cout << "HVcard " << sizeof(HVcard) << std::endl;
  std::cout << "HVmodule " << sizeof(HVmodule) << std::endl;
  std::cout << "HVchannel " << sizeof(HVchannel) << std::endl;
  */
  br.getServers();
  while (br.getNextServer(server, ptr1))
    {
      servers.push_back(server);
      std::string serverstr(server);
      if (serverstr.find("HV_DIM") != std::string::npos)
        cout << server << " @ " << ptr1 << endl;
    }
  /*
    br.getServerClients("DIS_DNS");
    while(br.getNextServerClient(ptr, ptr1))
    {
    cout << ptr << " @ " << ptr1 << endl;
    }
  */
  chans.clear();
  if (argc >= 2)
    {
      for (int i=1; i< argc; i++)
        {
          if (argv[i][0] == '-')
            {
              switch (argv[i][1])
                {
                case '?' :
                  usage();
                  return 0;
                case 'h' :
                  hostid =  atoi(argv[i]+2);
                  hostid = (hostid>0)?hostid:0;
                  sname << servername << "_HID" << hostid;
                  sname >> servername;
                  fHostID = 1;
                  break;
                case 'a' :
                  addr =  atoi(argv[i]+2);
                  addr = (addr>0) ? (addr<HVCARD_MAX_MODULES?addr:ALL) :0;
                  break;
                case 'b' :
                  type = atoi(argv[i]+2);
                  type = (type>0) ? (type<=MON36?type:MON36) :NONE;
                  fType = 1;
                  break;
                case 'n' :
                  cardaddr = atoi(argv[i]+2);
                  cardaddr = (cardaddr>0)? (cardaddr<=19?cardaddr:ALL):0;
                  break;
                case 'u' :
                  rup = atoi(argv[i]+2);
                  rup = (rup>0) ? (rup<=4000?rup:4000):0;
                  fRampUp=1;
                  break;
                case 'd' :
                  rdn = atoi(argv[i]+2);
                  rdn = (rdn>0) ? (rdn<=4000?rdn:4000):0;
                  fRampDown=1;
                  break;
                case 'o' :
                  state = atoi(argv[i]+2);
                  state = (state>0)?(state <= HV_STATE_ON?state:HV_STATE_ON):HV_STATE_OFF;
                  fModState = 1;
                  break;
                case 's' :
                  state = atoi(argv[i]+2);
                  state = (state>0)?(state <= HV_STATE_ON?state:HV_STATE_ON):HV_STATE_OFF;
                  fState = 1;
                  break;
                case 'v' :
                  vset = atoi(argv[i]+2);
                  vset = (vset>0) ? (vset<=4000?vset:4000) :0;
                  fVset = 1;
                  break;
                case 't' :
                  imax = atoi(argv[i]+2);
                  imax = (imax>0) ? (imax<=5000?imax:5000) :0;
                  fImax = 1;
                  break;
                case 'R' :
                  imax_ramp = atoi(argv[i]+2);
                  imax_ramp = (imax_ramp>0) ? (imax_ramp<=5000?imax_ramp:5000) :0;
                  fImaxRamp = 1;
                  break;
                case 'y' :
                  vmax = atoi(argv[i]+2);
                  vmax = (vmax>0) ? (vmax<=4000?vmax:4000) :0;
                  fVmax = 1;
                  break;
                case 'l' :
                  fSet = 1;
                  break;
                case 'r' :
                  fReset = 1;
                  break;
                case 'x' :
                  fAllOff = 1;
                  break;
                case 'c' :
                  ch = atoi(argv[i]+2);
                  if ((ch != CHAMB_1) && (ch != CHAMB_2))
                    {
                      ch = (ch>0) ? (ch<=36 ? ch-1 :ALL) :0;
                      chans.push_back(ch);
                    }
                  fChan = 1;
                  break;
                case 'f' :
                  relay = atoi(argv[i]+2);
                  relay = (relay>0) ? 4000 :0;
                  fRelay = 1;
                  break;
                case 'p' :
                  fShow = 1;
                  break;
                case 'j' :
                  fTripDelay = 1;
                  tripdelay = atoi(argv[i]+2);
                  tripdelay = (tripdelay>20) ? tripdelay: 20;
                  break;
                case 'J' :
                  fOVTripDelay = 1;
                  vtripdelay = atoi(argv[i]+2);
                  vtripdelay = (vtripdelay>20) ? vtripdelay: 20;
                  break;

                case 'k' :
                  fMasterTripDelay = 1;
                  master_tripdelay = atoi(argv[i]+2);
                  master_tripdelay = (master_tripdelay>20) ? master_tripdelay: 20;
                  break;
                case 'i' :
                  fTripResetDelay = 1;
                  tripresetdelay = atoi(argv[i]+2);
                  tripresetdelay = (tripresetdelay>20) ? tripresetdelay: 20;
                  break;
                case 'm' :
                  fTripMaxResets = 1;
                  maxtripresets = atoi(argv[i]+2);
                  maxtripresets = (maxtripresets>0) ? maxtripresets: 0;
                  break;
                case 'q' :
                  fILockDelay = 1;
                  ilock_delay = atoi(argv[i]+2);
                  ilock_delay = (ilock_delay>20) ? ilock_delay: 20;
                  break;
                case 'V' :
                  fSetOvervoltage = 1;
                  overvolt = atoi(argv[i]+2);
                  break;
                case 'z' :
                  fRealData = 0;
                  break;
                case 'w' :
                  savedata = atoi(argv[i]+2);
                  fSaveData = 1;
                  break;
                case 'F' :
                  dead_max_cnt = atoi(argv[i]+2);
                  fDeadMaxCnt = 1;
                  break;
                case 'g' :
                  fPrimary = 1;
                  break;
                case 'A' :
                  fForceAutoDetect = 1;
                  break;
                case 'C' :
                  fUpdateCalibration = 1;
                  break;
                case 'S' :
                  fHVCSCStatus = 1;
                  break;
                case 'M' :
                  fHVModuleStatus = 1;
                  break;
                case 'D' :
                  fDoCalibration = 1;
                  calibMode = atoi(argv[i]+2);
                  break;
                case 'U' :
                  fSaveConfig = 1;
                  cfgName=argv[i]+2;
                  break;
                case 'L' :
                  fLoadConfig = 1;
                  cfgName=argv[i]+2;
                  break;
                case 'T' :
                  fTypeFilter = 1;
                  str = argv[i]+2;

                  modaddr = ALL;

                  if (str.find("RDBALL") == 0)
                    {
                      modaddr = RDBALL_TYPE;
                    };
                  if (str.find("RDB30") == 0)
                    {
                      modaddr = RDB30_TYPE;
                    };
                  if (str.find("RDB36") == 0)
                    {
                      modaddr = RDB36_TYPE;
                    };
                  if (str.find("MASTER") == 0)
                    {
                      modaddr = MASTER_TYPE;
                    };

                  break;
                  /*
                    case 'm' :
                    str = string(argv[i]+2);
                    cout << str << endl;

                    fSetMaster = 1;
                    break;
                  */
                case 'B' :
                  fRunDimBenchmark = 1;
                  break;
                case 'H' :
                  fReadHistData = 1;
                  break;
                case 'P' :
                  pvss_update_interval = atoi(argv[i]+2);
                  fPVSS_UpdateInterval = 1;
                  break;
                case 'I' :
                  pvss_imon_thresh = atoi(argv[i]+2);
                  fPVSS_ImonThresh = 1;
                  break;
                case 'K' :
                  pvss_vmon_thresh = atoi(argv[i]+2);
                  fPVSS_VmonThresh = 1;
                  break;
                case 'W' :
                  fWienerTest = 1;
                  break;
                default :
                  printf("Unknown option %s\n",argv[i]);

                }
            } else {
          string opt(argv[i]);
          transform(opt.begin(), opt.end(), opt.begin(), pf);
	  std::cout << opt<< std::endl;
          if (opt.find("ME") == 0)
            {
              fCSCMode = 1;
              // cscname = argv[i];
              cscname = opt;
            }
          if (opt.find("RDB") == 0)
            {
              fRDBMode = 1;
              // rdbname = argv[i];
              rdbname = opt;
            }
	  }

        }
    }
  else
    {
      usage();
      return 0;
    }
  vector<string>::iterator pos;
  if (fHostID)
    {
      if ( (pos = find(servers.begin(), servers.end(), servername)) != servers.end())
        {
          cout << "Found " << servername;
        }
      else
        {
          cout << "ERROR: Can not find " << servername << ". Exiting." << endl;
          return 0;
        }
    }
  else
    {
      for (int i=0; i<servers.size(); i++)
        {
          if (servers[i].find(servername) != string::npos)
            {
              cout << "Found matching pattern " << servername << " server " << servers[i] << endl;
              servername = servers[i];
              break;
            }
        }
    }
  cout << "Base servername: " << servername << endl;

  DimInfo info_hcFound((servername+"/HCARDS_FOUND").c_str(),-1);
  DimInfo info_srvStatus((servername+"/SERVER_STATUS").c_str(),2,-1);

  vector <DimInfo *> data;
  // delay to initialize DimInfo service
  usleep(500000);

  found = info_hcFound.getInt();
  if (!fPrimary) 
  cardaddr = (cardaddr<found)?cardaddr:(cardaddr==ALL)?ALL:0;
  cout << found << " hostcards found" << endl;
  if (found<1)
    {
      cout << "Hostcards not found. Exiting..." << endl;
      return 0;
    }

  string srvStatus = info_srvStatus.getString();
  if (srvStatus.size())
    {
      cout << "Server Status: " << srvStatus << std::endl;
    }

  /*
    for (int i=0; i<found; i++) {
    stringstream st;
    st << servername << "/HV_DATA" << i;
    string name; st >> name;
    data.push_back(new DimInfo(name.c_str(), -1));
    // cout << name << endl;
    }
  */
  // DimInfo info_hcData((HV_DIM_NAME+"/HV_DATA").c_str(),-1);


  if (fTypeFilter && (addr >= HVCARD_MAX_MODULES)) addr = modaddr;

  string vlog = "vmon_";
  string ilog = "imon_";
  DimRpcInfo rpc(string(servername+string((fPrimary)?"/HV_PRIMARY_DATA":(fRealData)?"/HV_REAL_DATA":"/HV_RAW_DATA")).c_str(),3, 0);
  DimRpcInfo cscrpc(string(servername+"/HV_CSC_DATA").c_str(),3, 0);
  if (fCSCMode)
    {
      memset(&csccmdDIM, 0, sizeof(csccmdDIM));
      csccmdDIM.ID = cscrpc.getId();
      memset(&csccmd, 0, sizeof(csccmd));
      memcpy(csccmd.csc, cscname.c_str(), sizeof(csccmd.csc));
      memcpy(&csccmdDIM.data, &csccmd, sizeof(csccmd));
      cscrpc.setData((void *) &csccmdDIM, sizeof(csccmdDIM));
      if (cscrpc.getSize() != sizeof(struct HVcscdataDIM))
        {
          cerr << "Unknown CSC or no mapping" << endl;
          return -1;
        }
      memcpy(&cscdataDIM, cscrpc.getData(), cscrpc.getSize());
      memcpy(&cscdata, &cscdataDIM.data, sizeof(cscdata));

    }
  else
    {
      if ((addr<HVCARD_MAX_MODULES) && (cardaddr<found))
        {
          cmd.card = cardaddr;
          cmd.module = addr;
          cout << "size:" << sizeof(cmd) << endl;
          cmdDIM.ID = rpc.getId();
          memcpy(&cmdDIM.data, &cmd, sizeof(cmd));
          rpc.setData((void *) &cmdDIM, sizeof(cmdDIM));

          if (rpc.getSize() != ((fPrimary)?sizeof(struct HVPrimaryDIM):sizeof(struct HVboardDIM)))
            {
              cerr << "Wrong card or module number" << endl;
              //        return -1;
            }
          if (fPrimary)
            {
              memcpy(&primecardDIM, rpc.getData(), rpc.getSize());
              memcpy(&primecard, &primecardDIM.data, sizeof(primecard));
            }
          else
            {
              memcpy(&realcardDIM, rpc.getData(), rpc.getSize());
              memcpy(&realcard, &realcardDIM.data, sizeof(realcard));
            }
        }
    }

  if (fHVCSCStatus)
    {
      string status = "STATUS";
      DimRpcInfo hvstatus_rpc(string(servername+"/HV_CSC_STATUS").c_str(),3, 0);
      hvstatus_rpc.setData((char *) status.c_str(), status.size()+1);
      status = (char *)hvstatus_rpc.getData();
      cout << status << endl;
      return 0;


    }
  if (fHVModuleStatus)
    {
      string status = "STATUS";
      DimRpcInfo hvstatus_rpc(string(servername+"/HV_MODULE_STATUS").c_str(),3, 0);
      hvstatus_rpc.setData((char *) status.c_str(), status.size()+1);
      status = (char *)hvstatus_rpc.getData();
      cout << status << endl;
      return 0;

    }

  if (fAllOff)
    {
      printf("Turn Off all modules\n");
      send_hvcmd(ALL, ALL, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
      if (!fPrimary)	send_hvcmd(ALL, ALL, ALL, HV_SET_STATE, HV_STATE_OFF, 0);
    }
  if (fReset)
    {
      if (!fChan)
        {
          printf("Module%d:%0X reset trip: %d\n", cardaddr, addr, rup);
          send_hvcmd( cardaddr, addr, ALL, HV_RESET_TRIP, 0, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d reset trip: %d\n", cardaddr, addr,ch, rup);
              send_hvcmd( cardaddr, addr, ch, HV_RESET_TRIP, 0, 0);
            }
        }
    }
  if (fType)
    {
      printf("Module%d:%0X set type: %s\n", cardaddr, addr, modtype_str[type]);
      send_hvcmd(cardaddr, addr, 0, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
      send_hvcmd(cardaddr, addr, 0, HV_SET_MOD_TYPE, type, 0);
    }

  if (fReadHistData)
    {
      printf("Module%d:%0X read history data\n", cardaddr, addr);
      send_hvcmd(cardaddr, addr, 0, HV_GET_MOD_HIST_DATA, 0, 0);
    }


  if (fSaveConfig)
    {
      printf("Send SAVE_CONFIG=%s to %s\n", cfgName.c_str(), string(servername+"/SERVER_CONTROL").c_str());
      string cmd="SAVE_CONFIG="+cfgName;
      DimClient::sendCommand(string(servername+"/SERVER_CONTROL").c_str(), (char*)(cmd.c_str()));
    }

  if (fLoadConfig)
    {
      printf("Send LOAD_CONFIG=%s to %s\n", cfgName.c_str(), string(servername+"/SERVER_CONTROL").c_str());
      string cmd="LOAD_CONFIG="+cfgName;
      DimClient::sendCommand(string(servername+"/SERVER_CONTROL").c_str(), (char*)(cmd.c_str()));
    }


  if (fWienerTest)
    {

      printf("Send Wiener command SET_ALL_CHANS_ON\n");
      DimClient::sendCommand(string("WIENER_PL512_HID500/SYSCMD").c_str(), "SET_ALL_CHANS_ON");
    }

  if (fPVSS_UpdateInterval)
    {
      stringstream st;
      st << "PVSS_UPDATE_INTERVAL=" << pvss_update_interval;
      string cmd=st.str();;
      printf("Send %s to %s\n", cmd.c_str(), string(servername+"/SERVER_CONTROL").c_str());
      DimClient::sendCommand(string(servername+"/SERVER_CONTROL").c_str(), (char*)(cmd.c_str()));
    }

  if (fPVSS_ImonThresh)
    {
      stringstream st;
      st << "PVSS_IMON_THRESHOLD=" << pvss_imon_thresh;
      string cmd=st.str();;
      printf("Send %s to %s\n", cmd.c_str(), string(servername+"/SERVER_CONTROL").c_str());
      DimClient::sendCommand(string(servername+"/SERVER_CONTROL").c_str(), (char*)(cmd.c_str()));
    }

  if (fPVSS_VmonThresh)
    {
      stringstream st;
      st << "PVSS_VMON_THRESHOLD=" << pvss_vmon_thresh;
      string cmd=st.str();;
      printf("Send %s to %s\n", cmd.c_str(), string(servername+"/SERVER_CONTROL").c_str());
      DimClient::sendCommand(string(servername+"/SERVER_CONTROL").c_str(), (char*)(cmd.c_str()));
    }



  if (fDoCalibration)
    {
      printf("Perform Calibration\n");
      send_hvcmd(cardaddr, addr, ALL, HV_DO_CALIB, calibMode, 0);
    }

  if (fUpdateCalibration)
    {
      printf("Module%d:%0X Update Calibration\n", cardaddr, addr);
      send_hvcmd(cardaddr, addr, ALL, HV_UPDATE_CALIB, 0, 0);
    }

  if (fRunDimBenchmark)
    {
      printf("Module%d:%0X Run Dim Benchmark\n", cardaddr, addr);
      std::cout << "Send " << nCommands  << " DIM Commands" << std::endl;
      struct timeval t0, t1;
      gettimeofday(&t0, NULL);
      std::ostringstream st;
      char stc[100];// = "500|HVCMD;0;13;255;7;500;1";
      std::string cmdstr(stc); //"500|HVCMD;0;13;255;7;500;1";

      for (int i=0; i<nCommands; i++)
        {
          // sprintf(stc, "500|HVCMD;0;2;255;7;%d;1", i);
          sprintf(stc, "500|HVCSCCMD;ME-1/3/10;255;7;%d;1", i);
          // sprintf(stc, "PVSS_UPDATE_INTERVAL=%d", i);
          cmdstr = stc;
          DimClient::sendCommand("HV_1_COMMAND", (char *)cmdstr.c_str(), cmdstr.size()+1);
          // DimClient::sendCommand(string(servername+"/SERVER_CONTROL").c_str(), (char*)(cmdstr.c_str()));

        }
      gettimeofday(&t1, NULL);
      float loop_time = ((t1.tv_sec+t1.tv_usec/1000000.) - (t0.tv_sec+t0.tv_usec/1000000.));
      std::cout << "time: " << fixed << loop_time << std::endl;
      return 0;
    }

  if (fModState)
    {
      printf("Module%d:%0X set state: %s\n", cardaddr, addr, hvcmd_str[state]);
      send_hvcmd(cardaddr, addr, 0, HV_SET_MOD_STATE, state, 0);
    }
  if (fState)
    {
      if (!fChan)
        {
          printf("Module%d:%0X set state: %s\n", cardaddr, addr, hvcmd_str[state]);
          send_hvcmd(cardaddr, addr, 0, HV_SET_MOD_STATE, state, 0);
          if (!fPrimary) send_hvcmd(cardaddr, addr, ALL, HV_SET_STATE, state, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d set state: %s\n", cardaddr, addr,ch, hvcmd_str[state]);
              send_hvcmd(cardaddr, addr, ch, HV_SET_STATE, state, 0);
            }
        }

    }
  if (fRampUp)
    {
      if (!fChan)
        {
          printf("Module%d:%0X set ramp up: %d\n", cardaddr, addr, rup);
          send_hvcmd( cardaddr, addr, ALL, HV_SET_RAMP_UP, rup, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d set ramp up: %d\n", cardaddr, addr,ch, rup);
              send_hvcmd( cardaddr, addr, ch, HV_SET_RAMP_UP, rup, 0);
            }
        }
    }
  if (fRampDown)
    {
      if (!fChan)
        {
          printf("Module%d:%0X set ramp down: %d\n", cardaddr, addr, rdn);
          send_hvcmd( cardaddr, addr, ALL, HV_SET_RAMP_DOWN, rdn, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d set ramp down: %d\n", cardaddr, addr,ch, rdn);
              send_hvcmd( cardaddr, addr, ch, HV_SET_RAMP_DOWN, rdn, 0);
            }
        }
    }
  if (fVset)
    {
      if (!fChan)
        {
          printf("Module%d:%0X set Vset: %d\n", cardaddr, addr, vset);
          send_hvcmd( cardaddr, addr, ALL, HV_SET_VSET, vset, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d set Vset: %d\n", cardaddr, addr,ch, vset);
              send_hvcmd( cardaddr, addr, ch, HV_SET_VSET, vset, 0);
            }
        }
    }
  if (fImax)
    {
      if (!fChan)
        {
          printf("Module%d:%0X set Imax : %d\n", cardaddr, addr, imax);
          send_hvcmd( cardaddr, addr, ALL, HV_SET_IMAX, imax, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d set Imax: %d\n", cardaddr, addr,ch, imax);
              send_hvcmd( cardaddr, addr, ch, HV_SET_IMAX, imax, 0);
            }
        }
    }

  if (fImaxRamp)
    {
      if (!fChan)
        {
          printf("Module%d:%0X set ImaxRamp : %d\n", cardaddr, addr, imax_ramp);
          send_hvcmd( cardaddr, addr, ALL, HV_SET_IMAX_RAMP, imax_ramp, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d set ImaxRamp: %d\n", cardaddr, addr,ch, imax_ramp);
              send_hvcmd( cardaddr, addr, ch, HV_SET_IMAX_RAMP, imax_ramp, 0);
            }
        }
    }


  if (fVmax)
    {
      if (!fChan)
        {
          printf("Module%d:%0X set vmax : %d\n", cardaddr, addr, vmax);
          send_hvcmd( cardaddr, addr, ALL, HV_SET_VMAX, vmax, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d set Vmax: %d\n", cardaddr, addr,ch, vmax);
              send_hvcmd( cardaddr, addr, ch, HV_SET_VMAX, vmax, 0);
            }
        }
    }
  if (fTripDelay)
    {
      if (!fChan)
        {
          printf("Module%d:%0X set trip delay : %d\n", cardaddr, addr, tripdelay);
          send_hvcmd( cardaddr, addr, ALL, HV_SET_TRIP_DELAY, tripdelay, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d set trip delay: %d\n", cardaddr, addr,ch, tripdelay);
              send_hvcmd( cardaddr, addr, ch, HV_SET_TRIP_DELAY, tripdelay, 0);
            }
        }
    }

  if (fOVTripDelay)
    {
      if (!fChan)
        {
          printf("Module%d:%0X set overvoltage trip delay : %d\n", cardaddr, addr, vtripdelay);
          send_hvcmd( cardaddr, addr, ALL, HV_SET_OV_TRIP_DELAY, vtripdelay, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d set overvoltage trip delay: %d\n", cardaddr, addr,ch, vtripdelay);
              send_hvcmd( cardaddr, addr, ch, HV_SET_OV_TRIP_DELAY, vtripdelay, 0);
            }
        }
    }


  if (fTripResetDelay)
    {
      if (!fChan)
        {
          printf("Module%d:%0X set trip reset delay : %d\n", cardaddr, addr, tripresetdelay);
          send_hvcmd( cardaddr, addr, ALL, HV_SET_TRIP_RESET_DELAY, tripresetdelay, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d set trip reset delay: %d\n", cardaddr, addr,ch, tripresetdelay);
              send_hvcmd( cardaddr, addr, ch, HV_SET_TRIP_RESET_DELAY, tripresetdelay, 0);
            }
        }
    }

  if (fTripMaxResets)
    {
      if (!fChan)
        {
          printf("Module%d:%0X set max trip reset : %d\n", cardaddr, addr, maxtripresets);
          send_hvcmd( cardaddr, addr, ALL, HV_SET_TRIP_MAX_RESETS, maxtripresets, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d set trip delay: %d\n", cardaddr, addr,ch, maxtripresets);
              send_hvcmd( cardaddr, addr, ch, HV_SET_TRIP_MAX_RESETS, maxtripresets, 0);
            }
        }
    }


  if (fSetOvervoltage)
    {
      if (!fChan)
        {
          printf("Module%d:%0X set Vov (overvoltage limit) : %d\n", cardaddr, addr, overvolt);
          send_hvcmd( cardaddr, addr, ALL, HV_SET_OV_LIMIT, overvolt, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d set Vov (overvoltage limit): %d\n", cardaddr, addr,ch, overvolt);
              send_hvcmd( cardaddr, addr, ch, HV_SET_OV_LIMIT, overvolt, 0);
            }
        }
    }


  if (fDeadMaxCnt)
    {
      if (!fChan)
        {
          printf("Module%d:%0X set maximum DEAD state transitions : %d\n", cardaddr, addr, dead_max_cnt);
          send_hvcmd( cardaddr, addr, ALL, HV_SET_DEAD_MAX_CNT, dead_max_cnt, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d maximum DEAD state transitions: %d\n", cardaddr, addr,ch, dead_max_cnt);
              send_hvcmd( cardaddr, addr, ch, HV_SET_DEAD_MAX_CNT, dead_max_cnt, 0);
            }
        }
    }



  if (fMasterTripDelay)
    {
      printf("Module%d:%0X set master trip delay : %d\n", cardaddr, addr, master_tripdelay);
      send_hvcmd( cardaddr, addr, ALL, HV_SET_MASTER_TRIP_DELAY, master_tripdelay, 0);
    }
  if (fILockDelay)
    {
      printf("Module%d:%0X set interlock delay : %d\n", cardaddr, addr, ilock_delay);
      send_hvcmd( cardaddr, addr, ALL, HV_SET_ILOCK_DELAY, ilock_delay, 0);
    }

  if (fSaveData)
    {
      printf("Module%d:%0X %s save data\n", cardaddr, addr, (savedata>0)?"Start":"Stop");
      send_hvcmd( cardaddr, addr, ALL, HV_GET_MOD_DATA, savedata, 0);
    }
  if (fRelay)
    {
      if (!fChan)
        {
          printf("Module%d:%0X set relay : %d\n", cardaddr, addr, relay);
          send_hvcmd( cardaddr, addr, ALL, HV_SET_RELAY, relay, 0);
        }
      else
        {
          for (int i=0; i<chans.size(); i++)
            {
              int ch=chans[i];
              printf("Module%d:%0X Ch:%d set relay: %d\n", cardaddr, addr,ch, relay);
              send_hvcmd( cardaddr, addr, ch, HV_SET_RELAY, relay, 0);
            }
        }
    }
  if (fForceAutoDetect)
    {
      printf("Force hostcard detection and re-configuration\n");
      send_hvcmd( ALL, ALL, ALL, HV_DETECT, 0, 0);
    }
  /*
    if (fSetMaster) {
    printf("Module%d:%0X assign to Master Module at hostcard %d, busaddr %d, channel %d\n", cardaddr, addr, (masteraddr>>16)&0xFF, (masteraddr>>8)&0xFF, masteraddr&0xFF);
    send_hvcmd( cardaddr, addr, ALL, HV_SET_MASTER, masteraddr, 0);
    }
  */



  if (fShow)
    {
      initscr();

      // --- Init DIM Info service for new CSC mode
      if (fCSCMode)
        {
          HVcscFastMon csc_fast_mon;
          HVcscSlowMon csc_slow_mon;
          HVcscMonStats csc_mon_stats;
          string updcmd = "HVCSCDATA|" + cscname;
          stringstream st;
          st << "HV/" << cscname << "/FAST_MON";
          DimInfo cscFastMon((st.str()).c_str(),30, &csc_fast_mon, sizeof(csc_fast_mon));
          st.str("");
          st << "HV/" << cscname << "/SLOW_MON";
          DimInfo cscSlowMon((st.str()).c_str(),30, &csc_slow_mon, sizeof(csc_slow_mon));
          st.str("");
          st << "HV/" << cscname << "/STATS";
          DimInfo cscStats((st.str()).c_str(),30, &csc_mon_stats, sizeof(csc_mon_stats));

          while (1)
            {

              usleep(updatetime);
              // ------------> Read in new  CSC Mode

              // -- Get CSC Fast Mon Data
              if ( cscFastMon.getSize() != sizeof (struct HVcscFastMon))
                {
                  clear();
                  mvprintw(0,0,"Updated at %s; Server Status: %s\n",now().c_str(), info_srvStatus.getString());
                  printw("Fast Mon Invalid received data size: %d bytes (Expected: %d bytes)\n",
                         cscFastMon.getSize(), sizeof(struct HVcscFastMon ));
                  refresh();
                  continue;
                }
              memcpy(&csc_fast_mon, cscFastMon.getData(), cscFastMon.getSize());

              // -- Get CSC Slow Mon data
              if ( cscSlowMon.getSize() != sizeof (struct HVcscSlowMon))
                {
                  clear();
                  mvprintw(0,0,"Updated at %s; Server Status: %s\n",now().c_str(), info_srvStatus.getString());
                  printw("Slow Mon Invalid received data size: %d bytes (Expected: %d bytes)\n",
                         cscSlowMon.getSize(), sizeof(struct HVcscSlowMon ));
                  refresh();
                  continue;
                }
              memcpy(&csc_slow_mon, cscSlowMon.getData(), cscSlowMon.getSize());

              mvprintw (0,0,"Updated at %s; Server Status: %s",now().c_str(), info_srvStatus.getString());

              // -- Get CSC Mon stats
              if ( cscStats.getSize() == sizeof (struct HVcscMonStats))
                {
                  memcpy(&csc_mon_stats, cscStats.getData(), cscStats.getSize());
                  printw(", FastMon cntr: %d, SlowMon cntr: %d", csc_mon_stats.fast_mon_cnt, csc_mon_stats.slow_mon_cnt);
                }

              printw ("\nCSC:%s, %d channels, %s, ID#%d, RDB Status: %s linked to %s ID#%d Status: %s\nInterlock: %d, HVmon: %d,  +5V: %d, -5V: %d\n",
                      cscname.c_str(), csc_fast_mon.num_chans,
		      modtype_str[csc_slow_mon.rdb_type], csc_slow_mon.rdb_id,
                      hvstate_str[csc_fast_mon.mod_status],
		      modtype_str[csc_slow_mon.master_type], csc_slow_mon.master_id,
                      hvstate_str[csc_fast_mon.master_mod_status],
                      csc_fast_mon.interlock,
                      csc_fast_mon.hvmon,
                      csc_fast_mon.pos5v,
                      csc_fast_mon.neg5v);
              printw ("Update Interval: %ds, RDB Module State: %s, Ilock delay: %dms, Master Trip delay: %dms\n",
                      csc_slow_mon.update_interval,
                      hvstate_str[csc_slow_mon.mod_state],
                      csc_slow_mon.ilock_delay,
                      csc_slow_mon.master_trip_delay);
              printw ("===========================");
              printw ("======================================================\n");
              printw ("Ch#| Vmon   Imon   Status  ");
              printw ("Vset  Imax RampUp RampDn Itrip Vtrip Vthr  Ithr  State\n");
              printw ("- MASTER CHAN -------------");
              printw ("------------------------------------------------------\n");
              printw ("   | %4ld %6.1f %7s",
                      csc_fast_mon.master_chan.vmon,
                      csc_fast_mon.master_chan.imon/1000.,
                      hvstate_str[csc_fast_mon.master_chan.status]);
              printw ("%7ld %5ld   %4ld   %4ld %5ld  %4ld %4ld %5.1f %6s\n",
                      csc_slow_mon.master_chan.vset,
                      csc_slow_mon.master_chan.imax,
                      csc_slow_mon.master_chan.ramp_up,
                      csc_slow_mon.master_chan.ramp_down,
                      csc_slow_mon.master_chan.itrip,
                      csc_slow_mon.master_chan.vtrip,
                      csc_slow_mon.master_chan.vmon_threshold,
                      ((float)csc_slow_mon.master_chan.imon_threshold)/ADC_CONV_COEF,
                      hvstate_str[csc_slow_mon.master_chan.state]);
              printw ("- RDB ---------------------");
              printw ("------------------------------------------------------\n");
              for (int i=0; i < csc_fast_mon.num_chans; i++)
                {
                  printw ("%02d | %4ld %6.1f %7s",
                          i+1,
                          csc_fast_mon.chan[i].vmon,
                          csc_fast_mon.chan[i].imon/1000.,
                          hvstate_str[csc_fast_mon.chan[i].status]);

                  printw ("%7ld %5ld   %4ld   %4ld %5ld  %4ld %4ld %5.1f %6s\n",
                          csc_slow_mon.chan[i].vset,
                          csc_slow_mon.chan[i].imax,
                          csc_slow_mon.chan[i].ramp_up,
                          csc_slow_mon.chan[i].ramp_down,
                          csc_slow_mon.chan[i].itrip,
                          csc_slow_mon.chan[i].vtrip,
                          csc_slow_mon.chan[i].vmon_threshold,
                          ((float)csc_slow_mon.chan[i].imon_threshold)/ADC_CONV_COEF,
                          hvstate_str[csc_slow_mon.chan[i].state]);
                }
              printw ("===========================");
              printw ("======================================================\n");
              refresh();

              // DimClient::sendCommand("HV_1_COMMAND", updcmd.c_str());
              continue;
            }
// ------------< new CSC Mode
        }
      else
        {

          while (1)
            {
              usleep(updatetime);
              if (fCSCMode)
                {
                  cscrpc.setData((void *) &csccmdDIM, sizeof(csccmdDIM));
                  if (cscrpc.getSize() != sizeof(struct HVcscdataDIM))
                    {
                      // cerr << "Unknown CSC or no mapping" << endl;
                      // endwin();
                      // cerr << "Unknown CSC or no mapping" << endl;
                      clear();
                      mvprintw(0,0,"Updated at %s; Server Status: %s\n",now().c_str(), info_srvStatus.getString());
                      printw("Invalid received data size: %d bytes (Expected: %d bytes)\n", cscrpc.getSize(), sizeof(struct HVcscdataDIM));
                      refresh();
                      continue;
                    }
                  memcpy(&cscdataDIM, cscrpc.getData(), cscrpc.getSize());
                  memcpy(&cscdata, &cscdataDIM.data, sizeof(cscdata));
                  mvprintw(0,0,"Updated at %s; Server Status: %s\n",now().c_str(), info_srvStatus.getString());
                  printw("CSC:%s - endcap: %s , station:%d, ring:%d, position:%d\n",
                         cscdata.csc.name, ((cscdata.csc.endcap)?"+":"-"),cscdata.csc.station,
                         cscdata.csc.ring, cscdata.csc.position);
                  printw("RDB ID#%d, part%d, bus address:%d.%d on ", cscdata.rdb.ID, cscdata.rdb_part,
                         cscdata.hvcard.dataslot,cscdata.rdb.busaddr);
                  printw("hostcard - hostid:%ld, pci:%d.%d, dataslot:%d\n", cscdata.hvcard.hostid,
                         cscdata.hvcard.bus, cscdata.hvcard.slot, cscdata.hvcard.dataslot);
                  printw("RDB - state:%s, status:%s, type:%s, channels:%d, interlock:%s, hvmon:%d\n",
                         hvstate_str[cscdata.rdb.state],hvstate_str[cscdata.rdb.status],
                         modtype_str[cscdata.rdb.type], cscdata.rdb.num_chans,
                         ((cscdata.rdb.interlock)?"ON":"OFF"), cscdata.rdb.hvmon);
                  if (cscdata.rdb.master_id)
                    printw("RDB is linked to Master ID#%d bus address:%d.%d, channel:%d, trip_delay:%d ms\n",
                           cscdata.rdb.master_id, cscdata.rdb.master_hvcard, cscdata.rdb.master_busaddr,
                           cscdata.rdb.master_chan+1, cscdata.rdb.master_trip_delay);
                  else printw("!!! No logical assignement to Master module channel !!!\n");
                  printw("Primary PS - hostid:%d port:%d addr:%d\n", cscdata.primary.hostid, cscdata.primary.port, cscdata.primary.addr);
                  /*
                    printw("output: %s, ", (cscdata.primary.output)?"Enabled":"Disabled");
                    printw("remote: %s\n", (cscdata.primary.control)?"Enabled":"Disabled");
                    printw("Imax: %5.f mkA, ", cscdata.primary.imax);
                    printw("Imon: %5.f mkA, ", cscdata.primary.imon);
                    printw("Vmon: %5.f V, ", cscdata.primary.vmon);
                    printw("Vset: %4d V, ", cscdata.primary.vset);
                    printw("Vcur: %4d V", cscdata.primary.vcur);
                    printw("RmpUp: %3d V, ", cscdata.primary.rampup);
                    printw("RmpDn: %3d V\n", cscdata.primary.rampdown);
                  */
                  printw("--------------------------------------------------------------------------------------------------------------------------\n");
                  printw("Ch# Vmon   Imon Vset Vcur RmpUp RmpDn Vmax Imax ImaxR Vtrip Itrip TripCnt ITripDly  RstDly MaxRsts Relay Fuse State Status\n");
                  printw("       V    mkA    V    V     V     V    V  mkA   mkA     V   mkA      ms      ms      ms\n");
                  //			printw("-----------------------------------------------------------------------------------------------------\n");
                  //			printw("-- PRIMARY - hostid:%3d port:%d addr:%3d\n", cscdata.primary.hostid, cscdata.primary.port, cscdata.primary.addr);
                  printw("-- PRIMARY ---------------------------------------------------------------------------------------------------------------\n");
                  printw("   %5.f %6.1f %4ld %4ld  %4ld  %4ld     %5.f                                                              %5s %6s\n",
                         cscdata.primary.vmon, cscdata.primary.imon,
                         cscdata.primary.vset, cscdata.primary.vcur,
                         cscdata.primary.rampup, cscdata.primary.rampdown,
                         cscdata.primary.imax,
                         hvstate_str[cscdata.primary.state],
                         hvstate_str[cscdata.primary.status]);
                  //			printw("-- MASTER #%d bus address:%d.%d, channel:%d, trip_delay:%d ms\n", cscdata.rdb.master_id, cscdata.rdb.master_hvcard, cscdata.rdb.master_busaddr, cscdata.rdb.master_chan+1, cscdata.rdb.master_trip_delay);
                  printw("-- MASTER ----------------------------------------------------------------------------------------------------------------\n");
                  printw("%02d| %4ld %6.1f %4ld %4ld  %4ld  %4ld %4ld %4ld  %4ld  %4ld  %4ld  %6d  %6d  %6d  %6d  %4d      %5s %6s\n",cscdata.rdb.master_chan+1,
                         cscdata.master.vmon, ((float)cscdata.master.imon)/ADC_CONV_COEF,
                         cscdata.master.vset, cscdata.master.vcur,
                         cscdata.master.ramp_up, cscdata.master.ramp_down,
                         cscdata.master.vmax, cscdata.master.imax,cscdata.master.imax_ramp,
                         cscdata.master.vtrip, cscdata.master.itrip,
                         cscdata.master.trip_cnt, cscdata.master.trip_delay,
                         cscdata.master.trip_reset_delay,
                         cscdata.master.trip_max_resets,
                         cscdata.master.relay,
                         hvstate_str[cscdata.master.state],
                         hvstate_str[cscdata.master.status]);
                  printw("-- RDB --------------------------------------------------------------------------------------------------------------------\n");

                  // !!! Current ADC calibrated with 10mOhm external + 1mOhm internal resistors.. ADC_CONV_COEF Coeffiecent should be 11


                  int start_ch = (cscdata.rdb_part==1)?0:cscdata.rdb.num_chans/2;
                  int end_ch = (cscdata.rdb_part==1  && cscdata.rdb.type == RDB36)?cscdata.rdb.num_chans/2:cscdata.rdb.num_chans;
                  for (int j=start_ch; j<end_ch; j++)
                    {
                      printw("%02d| %4ld %6.1f %4ld %4ld  %4ld  %4ld %4ld %4ld  %4ld  %4ld  %4ld  %6d  %6d  %6d  %6d       %4ld %5s %6s\n",j+1-start_ch,
                             cscdata.rdb.chan[j].vmon, ((float)cscdata.rdb.chan[j].imon)/ADC_CONV_COEF,
                             cscdata.rdb.chan[j].vset, cscdata.rdb.chan[j].vcur,
                             cscdata.rdb.chan[j].ramp_up, cscdata.rdb.chan[j].ramp_down,
                             cscdata.rdb.chan[j].vmax, cscdata.rdb.chan[j].imax,cscdata.rdb.chan[j].imax_ramp,
                             cscdata.rdb.chan[j].vtrip, cscdata.rdb.chan[j].itrip,
                             cscdata.rdb.chan[j].trip_cnt, cscdata.rdb.chan[j].trip_delay,
                             cscdata.rdb.chan[j].trip_reset_delay,
                             cscdata.rdb.chan[j].trip_max_resets,
                             cscdata.rdb.chan[j].fuse,
                             hvstate_str[cscdata.rdb.chan[j].state],
                             hvstate_str[cscdata.rdb.chan[j].status]);
                    }
                  printw("---------------------------------------------------------------------------------------------------------------------------\n");

                }
              else
                {
                  cmd.card = cardaddr;
                  cmd.module = addr;
                  memcpy(&cmdDIM.data, &cmd, sizeof(cmd));
                  // cout << "size:" << sizeof(calib) << endl;
                  rpc.setData((void *) &cmdDIM, sizeof(cmdDIM));
                  if (rpc.getSize() != ((fPrimary)?sizeof(struct HVPrimaryDIM):sizeof(struct HVboardDIM)))
                    {
                      // cerr << "Wrong card or module number" << endl;
                      clear();
                      mvprintw(0,0,"Updated at %s; Server Status: %s\n",now().c_str(), info_srvStatus.getString());
                      printw("Invalid received data size: %d bytes (Expected: %d bytes)\n", rpc.getSize(), (fPrimary)?sizeof(struct HVPrimaryDIM):sizeof(struct HVboardDIM));
                      refresh();
                      // endwin();
//	    cerr << "Wrong card or module number (data size:" << rpc.getSize() << ")" << endl;
                      continue;
                      // return -1;
                    }
                  if (fPrimary)
                    {
                      memcpy(&primecardDIM, rpc.getData(), rpc.getSize());
                      memcpy(&primecard, &primecardDIM.data, sizeof(primecard));
                    }
                  else
                    {
                      memcpy(&realcardDIM, rpc.getData(), rpc.getSize());
                      memcpy(&realcard, &realcardDIM.data, sizeof(realcard));
                    }

                  if (fPrimary)
                    {
                      mvprintw(0,0,"Updated at %s; Server Status: %s\n",now().c_str(), info_srvStatus.getString());
                      printw("HostID:%d COM:%d GPIB Addr:%d\n", primecard.hostid, primecard.port, primecard.addr);
                      printw("Imax:\t %5.f mkA\n", primecard.imax);
                      printw("Imon:\t %5.f mkA\n", primecard.imon);
                      printw("Vmon:\t %5.f V\n", primecard.vmon);
                      printw("Vset:\t %5d V\n", primecard.vset);
                      printw("Vcur:\t %5d V\n", primecard.vcur);
                      printw("RampUp:\t %5d V\n", primecard.rampup);
                      printw("RampDn:\t %5d V\n", primecard.rampdown);
                      printw("Output:\t %s\n", (primecard.output)?"Enabled":"Disabled");
                      printw("Remote:\t %s\n", (primecard.control)?"Enabled":"Disabled");
                      printw("Polarity: %s\n", (primecard.polarity)?"Negative":"Positive");
                      printw("State:\t %s\n", hvstate_str[primecard.state]);
                      printw("Status:\t %s\n", hvstate_str[primecard.status]);
                    }
                  else
                    {
                      int i = addr;
                      mvprintw(0,0,"Updated at %s; Server Status: %s\n",now().c_str(), info_srvStatus.getString());
                      printw("HostID:%ld, PCI:%d:%d, dataslot:%d, ioaddr:0x%0lX\n",
                             realcard.card.hostid, realcard.card.bus, realcard.card.slot,
                             realcard.card.dataslot, realcard.module.ioaddr);
                      printw("Module:0x%x: state:%s, status:%s, type:%s, ID:%d, Chans:%d\n",
                             realcard.module.busaddr, hvstate_str[realcard.module.state],
                             hvstate_str[realcard.module.status], modtype_str[realcard.module.type],
                             realcard.module.ID, realcard.module.num_chans);
                      printw("ILock:%d, ILock delay:%dms, ILock delay cnt:%dms, HVmon:%d, Policy:0x%x, +5V:%d, -5V:%d\n",
                             realcard.module.interlock, realcard.module.ilock_delay,
                             realcard.module.ilock_delay_cnt, realcard.module.hvmon, realcard.module.policy, realcard.module.pos5v, realcard.module.neg5v);
                      if (realcard.module.type == MASTER && realcard.module.master_hvcard>0)
                        printw("Attached to Primary PS at HostID%d COM%d Addr:%d Status:%s\n",
                               realcard.module.master_hostid,
                               realcard.module.master_busaddr, realcard.module.master_hvcard, hvstate_str[realcard.module.pr_status]);
                      if ((realcard.module.type != NONE) && (realcard.module.type != MASTER))
                        {
                          if (realcard.module.master_id)
                            printw("Attached to Master Module HostID%d ID:%d %d:%x:%d Master TripDelay:%d ms\n",
                                   realcard.module.master_hostid,
                                   realcard.module.master_id, realcard.module.master_hvcard,
                                   realcard.module.master_busaddr,	realcard.module.master_chan,
                                   realcard.module.master_trip_delay);
                          else printw("No logical assignement to Master module channel\n");
                          printw("Linked Primary PS Status: %s\n", hvstate_str[realcard.module.pr_status]);
                        }
                      printw("Ch# Vmon   Imon Vset Vcur Vadc  Vov RmpUp RmpDn Vmax Imax ImaxR Vtrip Itrip OVTripDly ITripDly  RstDly RsDlyCnt MaxRsts RstCnt MaxDCnt DeadCnt Relay Fuse State Status\n");
                      if (fRealData)
                        printw("       V    mkA    V    V    V    V     V     V    V  mkA   mkA     V   mkA        ms       ms      ms       ms\n");
                      printw("----------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

                      // !!! Current ADC calibrated with 10mOhm external + 1mOhm internal resistors.. ADC_CONV_COEF Coeffiecent should be 11
                      for (int j=0; j<realcard.module.num_chans; j++)
                        {
                          if (fRealData)
                            {
                              printw("%02d| %4ld %6.1f %4ld %4ld %4ld %4ld  %4ld  %4ld %4ld %4ld  %4ld  %4ld  %4ld    %6d   %6d  %6d   %6d  %6d  %5d   %5d   %5d  %4ld %4ld %5s %6s\n",j+1,
                                     realcard.module.chan[j].vmon,
                                     ((float)realcard.module.chan[j].imon)/ADC_CONV_COEF,
                                     realcard.module.chan[j].vset, realcard.module.chan[j].vcur,
                                     realcard.module.chan[j].vset_adc, realcard.module.chan[j].vov,
                                     realcard.module.chan[j].ramp_up, realcard.module.chan[j].ramp_down,
                                     realcard.module.chan[j].vmax, realcard.module.chan[j].imax,
                                     realcard.module.chan[j].imax_ramp,
                                     realcard.module.chan[j].vtrip, realcard.module.chan[j].itrip,
                                     realcard.module.chan[j].vtrip_delay,
                                     realcard.module.chan[j].trip_delay,
                                     realcard.module.chan[j].trip_reset_delay,
                                     realcard.module.chan[j].trip_reset_delay_cnt,
                                     realcard.module.chan[j].trip_max_resets,
                                     realcard.module.chan[j].trip_resets_cnt,
                                     realcard.module.chan[j].dead_max_cnt,
                                     realcard.module.chan[j].dead_cnt,
                                     realcard.module.chan[j].relay, realcard.module.chan[j].fuse,
                                     hvstate_str[realcard.module.chan[j].state],
                                     hvstate_str[realcard.module.chan[j].status]);
                              if (fLogOpen)
                                {
                                  vlogfile << realcard.module.chan[j].vmon << " ";
                                  ilogfile << ((float)realcard.module.chan[j].imon)/ADC_CONV_COEF << " ";
                                };
                            }
                          else
                            {
                              printw("%02d| %4ld   %4ld %4ld %4ld %4ld %4ld  %4ld  %4ld %4ld %4ld %4ld  %4ld  %4ld    %6d   %6d  %6d   %6d  %6d  %5d  %4ld %4ld %5s %6s\n",j+1,
                                     realcard.module.chan[j].vmon,
                                     realcard.module.chan[j].imon,
                                     realcard.module.chan[j].vset, realcard.module.chan[j].vcur,
                                     realcard.module.chan[j].vset_adc, realcard.module.chan[j].vov,
                                     realcard.module.chan[j].ramp_up, realcard.module.chan[j].ramp_down,
                                     realcard.module.chan[j].vmax, realcard.module.chan[j].imax,
                                     realcard.module.chan[j].imax_ramp,
                                     realcard.module.chan[j].vtrip, realcard.module.chan[j].itrip,
                                     realcard.module.chan[j].vtrip_delay,
                                     realcard.module.chan[j].trip_delay,
                                     realcard.module.chan[j].trip_reset_delay,
                                     realcard.module.chan[j].trip_reset_delay_cnt,
                                     realcard.module.chan[j].trip_max_resets,
                                     realcard.module.chan[j].trip_resets_cnt,
                                     realcard.module.chan[j].relay, realcard.module.chan[j].fuse,
                                     hvstate_str[realcard.module.chan[j].state],
                                     hvstate_str[realcard.module.chan[j].status]);
                            }
                        }
                      if (fLogOpen && fRealData)
                        {
                          vlogfile << endl;
                          ilogfile << endl;
                        }
                    }
                }
              refresh();
            }
        }
      endwin();
    }
  return 0;
}


