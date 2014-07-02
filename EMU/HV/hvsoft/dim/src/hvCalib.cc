#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>
#include<sys/types.h>
#include<sys/socket.h>
#include <sys/time.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
#include <unistd.h>
#include <list>
#include <set>
#include <stdio.h>
#include <math.h>

#include <dic.hxx>
#include "hvdim.h"
#include "hvprimary.h"

using namespace std;

typedef struct calData
{
    int V1;                                       // RDB 1st point ADC value
    int V2;                                       // RDB 2nd point ADC value
    float V1pr;                                   // Primary PS 1st Volt value
    float V2pr;                                   // --- 2nd ---
    float V1volt;                                 // Voltmeter 1st point Volt value
    float V2volt;                                 // ---- 2nd ----
    float Aadc;                                   // V1volt = A*V1 + B
    float Badc;                                   //
    float Adac;
    float Bdac;
} calData;

vector<calData> calib;

string servername = HV_DIM_NAME;
int Host = 600;
int Card = -1;
int Addr = -1;
float AvgSlope= 1.0;

int vc_card = -1;
int vc_addr = -1; // voltage calibrator card and addr
string vc_adc_file = ""; // voltage calibrator ADC values
#define MAX_DB 511 // max distr boards
#define MAX_M  24 // max masters
#define MAX_PR 4 // max primaries
#define MAX_HC 16 // max hostcards
#define MAX_A 16 // max addresses
#define MAX_COM 2 // max com ports
#define MAX_CA 5 // max primary addresses on com ports (0-4)
#define MAX_CH 37 // max channels (one additional for voltmeter channel)

// lists of all distribution and master boards
list <HVboardDIM*> distr; // distribution boards
list <HVboardDIM*> master; // master boards
list <HVPrimaryDIM*> primary; // primaries
// storages for primary calib values
int pr_set[5][2], pr_v[5][2];

set <int> include_db; // dboard numbers to calibrate, if empty calibrate all
set <int> exclude_db; // dboards to exclude from calibration, if empty do not exclude any


bool fHostID = false;
bool fCurrentCalibration = false;
bool fCurrentCalibOldMethod = false;
bool fCalib100M = false;
bool fLoadFromFile = false;
bool fCalibTest = false;
bool fFixedSlope = false;
bool fMasterModule = false;
bool f_interpol = false;
bool f_interpol_mult = false;
bool f_distribution_global_adc = false;
bool f_distribution_global_dac = false;
bool f_check_v_calib_const = false;
bool f_check_v_ADC_measurements = false;
bool f_dump_adc_volts = false;
bool f_calib_wo_master = false;
bool f_master_global_adc = false;
bool f_master_global_dac = false;

string pattern = HV_DIM_NAME;
string Server = "cms-uf77.phys.ufl.edu";

HVboardDIM mRDB, m_vc; // Distribution board and voltage calibrator
HVboardDIM mMaster;
HVPrimaryDIM mPrimary;
DimRpcInfo* rpcPrCmd = NULL;
DimRpcInfo* rpcPrData = NULL;
DimRpcInfo* rpcModCmd = NULL;
DimRpcInfo* rpcModData = NULL;
DimRpcInfo* rpcModCmdReal = NULL;
DimRpcInfo* rpcModDataReal = NULL;

int V_PR1=1100;                                   // RDB 1st calibration point Vset=2400
int V_PR2=2700;                                   // RDB 2nd calibration point Vset=2900

int nSamples = 5;                                 // ADC Samples to read for averaging
float Volt_coef = 0.995;                          // 0.995;// 0.995
int V_CHECK=3600;                                 // calibration check voltage
int V_DELTA=5;                                    // +/-5V calibration check range
int M_DAC1 = 1000;
int M_DAC2 = 2200;
int DAC_DIP=400;                                  // voltage DAC difference on unmeasured channels
int VOLT_DIP=400;                                 // voltage (V) difference on unmeasured channels

void usage()
{
    cout << "Usage: hvCalib -[options]\n"
        << "Options:\n"
        << "\t-?\tshow this usage info\n"
        << "\t-hN\tset computer host ID (default - use first matching name DIM server)\n"
        << "\t-nN\tset hostcard address (default=" << Card << ")\n"
        << "\t-aN\tset module address N[0..15] (default="<< Addr <<")\n"
        << "\t-sN\tset number ADC readout samples for averaging (default=" << nSamples << ")\n"
        << "\t-cN\tset Multimeter' calibration coefficient (default="<< Volt_coef << ")\n"
        << "\t-mS\tset Multimeter's host address (default="<< Server << ")\n"
        << "\t-C\tset Current ADC calibration mode (default is Voltage calibration\n"
        << "\t-o\tset older method of current calibration\n"
        << "\t-M\tuse 100MOhm load\n"
        << "\t-F\tuse fixed slope coefficient - " << AvgSlope << "\n"
        << "\t-T\tselect voltage calibration check mode\n"
        << "\t-vN\tset calibration check voltage in V (default="<< V_CHECK << ")\n"
        << "\t-dN\tset calibration check range in V (default= +/-"<<V_DELTA << ")\n"
        << "\t-NN\tvoltage calibrator hostcard address\n"
        << "\t-AN\tvoltage calibrator module address N[0..15]\n"
        << "\t-u\tvoltage ADC calibration of all distribution boards in system\n"
        << "\t-U\tvoltage DAC calibration of all distribution boards in system\n"
	    << "\t-V\t check V calibration constants, report errors\n"
	    << "\t-k\t check V ADC measurements in pass-thru mode\n"
	    << "\t-X\t dump V ADC measurements into adc_volts.csv\n"

        << endl;
}


string now()
{
    char buf[255];
    time_t now=time(NULL);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    string time = string(buf);
    return time.substr(0,time.find("\n",0));

};

float readMeterU(string Address)
{
    bool flag = true;
    bool connectionFlag = false;
    int sockId;
    int i,size;
    float MeterResult = 0;
    struct sockaddr_in ServerAddr;

//this gets the details used by sockets from the name of the server
    struct hostent *ht = gethostbyname(Address.c_str());
//char *name = inet_ntoa( *((struct in_addr *)ht->h_addr) );
//cout<<name<<endl;

//create a socket for communication
//this is UDP socket
    sockId = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
//socket ID should be greater or equal than 3
    if(sockId <3)
    {
        cout<<"Error in creating socket";
        return 0;
    }

//set the parameters to be used by socket

//set family to internet family
    ServerAddr.sin_family = AF_INET;

//set port to server port
//we are communicating on 12000 port on server
    ServerAddr.sin_port = htons(12000);

//copy the IP address of the server received from gethostbyname function
    memcpy((char*)&ServerAddr.sin_addr,(char*)ht->h_addr,ht->h_length);

    size = sizeof(ServerAddr);
//this message is sent to server just for handshaking
    char message[] = "Start Reading";
    i = sendto(sockId,message,sizeof(message),0,(struct sockaddr *)&ServerAddr,size);
//if -1 then message not sent for some reason
    if(i == -1)
    {
        cout<<"error in sending";
    }
    char buffer[100];
    int addrlen;
// receieve result of handshakking message from server
    addrlen = sizeof(struct sockaddr);
    i = recvfrom(sockId,buffer,sizeof(buffer),0,(struct sockaddr *)&ServerAddr,(socklen_t *)&addrlen);
    buffer[i] = '\0';
//  cout<<buffer<<endl;
//server should send Accepted
    if(strcmp(buffer,"Accepted"))
    {
        cout<<"not able to make connection with client";
        cout<<"expecting to receive Accepted from the Server";
    }
    else
    {
        connectionFlag = true;
    }
//if hans shaking done, then send the message to receive meter reading
    if(connectionFlag)
    {

        char buffer1[100];
        char ReadMessage[] = "Read";
//send on the same socket to same address
        i = sendto(sockId,ReadMessage,sizeof(ReadMessage),0,(struct sockaddr *)&ServerAddr,size);
        if(i != sizeof(ReadMessage))
        {
            cout<<"error in sending"<<endl;
            cout<<"it should have sent "<<sizeof(ReadMessage)<<" bytes but it have send "<<i<<" byes"<<endl;
        }
//receive the result
        i = recvfrom(sockId,buffer1,sizeof(buffer1),0,(struct sockaddr *)&ServerAddr,(socklen_t *)&addrlen);
        buffer1[i] = '\0';
//      for(int j =0;j<i;j++)
//      cout<<buffer1[j];
//      cout<<endl;

//the resultis in string format,convert it to float format
//sscanf takes a string and return float
        sscanf(buffer1,"%f",&MeterResult);
// cout<<"Result in Float Format is :"<<MeterResult<<endl;

    }
//close the socket and return
    close(sockId);
    return MeterResult;
}


float readMeter(string Address)
{
    bool flag = true;
    bool connectionFlag = false;
    int sockId;
    int i,size;
    float MeterResult = 0;
    struct sockaddr_in ServerAddr;

//this gets the details used by sockets from the name of the server
    struct hostent *ht = gethostbyname(Address.c_str());
//char *name = inet_ntoa( *((struct in_addr *)ht->h_addr) );
//cout<<name<<endl;

//create a socket for communication
//this is UDP socket
    sockId = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
//socket ID should be greater or equal than 3
    if(sockId <3)
    {
        cout<<"Error in creating socket";
        return 0;
    }

//set the parameters to be used by socket

//set family to internet family
    ServerAddr.sin_family = AF_INET;

//set port to server port
//we are communicating on 12000 port on server
    ServerAddr.sin_port = htons(12001);

//copy the IP address of the server received from gethostbyname function
    memcpy((char*)&ServerAddr.sin_addr,(char*)ht->h_addr,ht->h_length);

    size = sizeof(ServerAddr);
//this message is sent to server just for handshaking
    char message[] = "Start Reading";

    if(connect(sockId,(struct sockaddr*)&ServerAddr,sizeof(ServerAddr)))
    {
        cout << "Error connecting to server" << endl;
        close(sockId);

        return 0;
    }
// i = sendto(sockId,message,sizeof(message),0,(struct sockaddr *)&ServerAddr,size);
    i = send(sockId,message,sizeof(message),0);
//if -1 then message not sent for some reason
    if(i == -1)
    {
        cout<<"error in sending";
    }
    char buffer[100];
    int addrlen;
// receieve result of handshakking message from server
    addrlen = sizeof(struct sockaddr);
//  i = recvfrom(sockId,buffer,sizeof(buffer),0,(struct sockaddr *)&ServerAddr,(socklen_t *)&addrlen);
    i = recv(sockId,buffer,sizeof(buffer),0);
    buffer[i] = '\0';
//  cout<<buffer<<endl;
//server should send Accepted
    if(strcmp(buffer,"Accepted"))
    {
        cout<<"not able to make connection with client";
        cout<<"expecting to receive Accepted from the Server";
    }
    else
    {
        connectionFlag = true;
    }
//if hans shaking done, then send the message to receive meter reading
    if(connectionFlag)
    {

        char buffer1[100];
        char ReadMessage[] = "Read";
//send on the same socket to same address
// i = sendto(sockId,ReadMessage,sizeof(ReadMessage),0,(struct sockaddr *)&ServerAddr,size);
        i = send(sockId,ReadMessage,sizeof(ReadMessage),0);
        if(i != sizeof(ReadMessage))
        {
            cout<<"error in sending"<<endl;
            cout<<"it should have sent "<<sizeof(ReadMessage)<<" bytes but it have send "<<i<<" byes"<<endl;
        }
//receive the result
        i = recv(sockId,buffer1,sizeof(buffer1),0);
// i = recvfrom(sockId,buffer1,sizeof(buffer1),0,(struct sockaddr *)&ServerAddr,(socklen_t *)&addrlen);
        buffer1[i] = '\0';
//      for(int j =0;j<i;j++)
//      cout<<buffer1[j];
//      cout<<endl;

//the resultis in string format,convert it to float format
//sscanf takes a string and return float
        sscanf(buffer1,"%f",&MeterResult);
// cout<<"Result in Float Format is :"<<MeterResult<<endl;

    }
//close the socket and return
    close(sockId);
    return MeterResult;
}


int readModuleData(UCHAR card, UCHAR addr, HVboardDIM* board, bool fRealData)
{
    int res=0;
    HVcmdDIM hvcmd;
    hvcmd.data.card = card;
    hvcmd.data.module = addr;
    hvcmd.data.chan = ALL;
    hvcmd.data.cmd = HV_GET_MOD_DATA;
    hvcmd.data.data = 0;
    hvcmd.data.size = 0;
/*string service = servername;
  if (fRealData)
  service +="/HV_REAL_DATA";
  else
  service +="/HV_RAW_DATA";
  DimRpcInfo rpc(service.c_str(), -1);
*/
    DimRpcInfo* rpc;
    if (fRealData)
        rpc = rpcModDataReal;
    else
        rpc = rpcModData;
    hvcmd.ID = rpc->getId();
    rpc->setData((void *) &hvcmd, sizeof(hvcmd));
    if (rpc->getSize() != sizeof(struct HVboardDIM))
    {
//        cerr << "Wrong card or module number" << endl;
        return -1;
    }
    memcpy(board, rpc->getData(), rpc->getSize());

    return res;
}


int readPrimaryData(UCHAR card, UCHAR addr, HVPrimaryDIM* prime)
{
    int res=0;
    HVcmdDIM hvcmd;
    hvcmd.data.card = card;
    hvcmd.data.module = addr;
    hvcmd.data.chan = ALL;
    hvcmd.data.cmd = HV_GET_MOD_DATA;
    hvcmd.data.data = 0;
    hvcmd.data.size = 0;
// string service = servername+"/HV_PRIMARY_DATA";
// DimRpcInfo rpc(service.c_str(), -1);
    hvcmd.ID = rpcPrData->getId();
    rpcPrData->setData((void *) &hvcmd, sizeof(hvcmd));
    if (rpcPrData->getSize() != sizeof(struct HVPrimaryDIM))
    {
//        cerr << "Wrong Primary PS" << endl;
        return -1;
    }
    memcpy(prime, rpcPrData->getData(), rpcPrData->getSize());

    return res;
}


int sendModuleCmd(UCHAR card, UCHAR addr, UCHAR chan, HV_CMD cmd, ULONG data, ULONG size, bool fRealData)
{
    int res=0;
    HVcmdDIM hvcmd;
    hvcmd.data.card = card;
    hvcmd.data.module = addr;
    hvcmd.data.chan = chan;
    hvcmd.data.cmd = cmd;
    hvcmd.data.data = data;
    hvcmd.data.size = size;
/*
  string service = servername;
  if (fRealData)
  service +="/HV_REAL_CMD";
  else
  service +="/HV_RAW_CMD";
  DimRpcInfo rpc(service.c_str(), -1);
*/
    DimRpcInfo* rpc;
    if (fRealData)
        rpc = rpcModCmdReal;
    else
        rpc = rpcModCmd;
    hvcmd.ID = rpc->getId();
    rpc->setData((void *) &hvcmd, sizeof(hvcmd));
    return res;
}


int sendPrimaryCmd(UCHAR card, UCHAR addr, UCHAR chan, HV_CMD cmd, ULONG data, ULONG size)
{
    int res=0;
    HVcmdDIM hvcmd;
    hvcmd.data.card = card;
    hvcmd.data.module = addr;
    hvcmd.data.chan = chan;
    hvcmd.data.cmd = cmd;
    hvcmd.data.data = data;
    hvcmd.data.size = size;
/*  if (rpcPr == NULL) {
    string service = servername+"/HV_PRIMARY_CMD";
    DimRpcInfo* rpcPr = new DimRpcInfo(service.c_str(), -1);
    }
*/
// if (rpcPr != NULL) {
    hvcmd.ID = rpcPrCmd->getId();
    rpcPrCmd->setData((void *) &hvcmd, sizeof(hvcmd));
// }
    return res;
}


class dimHVErrHandler : public DimErrorHandler
{
    public:
        void errorHandler(int severity, int code, char *msg)
        {
            cout << msg << endl;
            string DNSNode = DimClient::getDnsNode();
            int DNSPort = DimClient::getDnsPort();
            cout << "DNS at " << DNSNode << ":" <<DNSPort << " is not responding" << endl;
            switch (code)
            {
                case DIMDNSCNERR:
                    if (DNSPort != BACKUP_DNS_PORT)
                    {
                        cout << "connecting to backup DNS " << BACKUP_DNS_NODE << ":" << BACKUP_DNS_PORT << endl;
                        DimClient::setDnsNode(BACKUP_DNS_NODE, BACKUP_DNS_PORT);
                    }
                    break;
            }
        }
};

bool getServerName(string pattern, string& name)
{

    DimBrowser br;
    vector<string> servers;
    char *server, *ptr, *ptr1;
    vector<string>::iterator pos;
    br.getServers();
    while(br.getNextServer(server, ptr1))
    {
        servers.push_back(server);
        cout << server << " @ " << ptr1 << endl;
    }
    for (int i=0; i<servers.size(); i++)
    {
        if (servers[i].find(pattern) != string::npos)
        {
            cout << "Found matching pattern " << pattern << " server " << servers[i] << endl;
            name = servers[i];
            cout << "Base servername: " << name << endl;
            return true;
        }
    }
    return false;
}


int saveVoltageCalibrationResults(int ID, vector<calData>& data)
{

    ofstream ofs;

    ostringstream st;
    if (data.size() <1)
    {
        cerr << "Empty results data" << endl;
        return -1;
    }

    st << "T10_M" << setw(3) << setfill('0') << ID << ".log";

    int delim = data.size()/3;

    ofs.open(st.str().c_str(), ios_base::app);
    ofs << "========================================================\n"
        << "Test10 (ADC Calibration) results " << now()
        << "\nBoard ID" << ID << " " << data.size()-1 << " channels\n"
        << "Linux calibration software\n\n"
                                                  // !!! Need actual coefficients for Voltemeter channel
        << "Voltmeter channel: A=" << data[data.size()-1].Aadc <<", B=" << data[data.size()-1].Badc << "\n" << endl;

    ofs << "A:" << endl;
    for (int i=0; i<data.size()-1; i++)
    {
        ofs << data[i].Aadc << ((i && ((i+1)%delim==0))?"\n":",");
    }
    ofs << "\n" << endl;
    ofs << "B:" << endl;
    for (int i=0; i<data.size()-1; i++)
    {
        ofs << data[i].Badc << ((i && ((i+1)%delim==0))?"\n":",");
    }
    ofs << "\n" << endl;
    ofs.close();

    st.clear();
    st.flush();
    st.seekp(0);
    st << "T11_M" << setw(3) << setfill('0') << ID << ".log";

    ofs.open(st.str().c_str(), ios_base::app);
    ofs << "========================================================\n"
        << "Test11 (DAC Calibration) results " << now()
        << "\nBoard ID" << ID << " " << data.size()-1 << " channels\n"
        << "Linux calibration software\n"
        << "Calibration DAC Range: [" << V_PR1 << " - " << V_PR2 << "]\n" << endl;
    ofs << "A:" << endl;
    for (int i=0; i<data.size()-1; i++)
    {
        ofs << data[i].Adac << ((i && ((i+1)%delim==0))?"\n":",");
    }
    ofs << "\n" << endl;
    ofs << "B:" << endl;
    for (int i=0; i<data.size()-1; i++)
    {
        ofs << data[i].Bdac << ((i && ((i+1)%delim==0))?"\n":",");
    }
    ofs << "\n" << endl;
    ofs.close();

    st.clear();
    st.flush();
    st.seekp(0);
    st << "Calib_M" << setw(3) << setfill('0') << ID << ".dat";
    ofs.open(st.str().c_str(), ios_base::app);
    ofs << "========================================================\n"
        << "ADC/DAC Calibration) data " << now()
        << "\nBoard ID" << ID << " " << data.size()-1 << " channels\n"
        << "Linux calibration software\n"
        << "Calibration DAC Range: [" << V_PR1 << " - " << V_PR2 << "]\n"
        << "Multimeter coefficient: " << Volt_coef << endl;
    for (int i=0; i<data.size(); i++)
    {
        ofs << "ch#" << i+1 << " V1=" << data[i].V1/nSamples << " V2=" << data[i].V2/nSamples
            << " V1volt=" << data[i].V1volt << " V2volt=" << data[i].V2volt << endl;;
    }
    ofs.close();

    return 0;
}


// == Calibrate RDB Voltage ADC
int CalibrateRDBVoltage(int card, int addr)
{

    cout << "Calibrating DAC/ADC of "<< modtype_str[ mRDB.data.module.type]<< " ID#" << mRDB.data.module.ID
        << " at " << card<< ":" << addr << endl;
//   vector<calData> calib;
    calib.clear();
    calData calchan;
    int PR_V = 4000;
    int PR_VL = 2000;

    int m_card = mRDB.data.module.master_hvcard;
    int m_addr = mRDB.data.module.master_busaddr;
    int m_chan = mRDB.data.module.master_chan;
    int p_addr = mMaster.data.module.master_hvcard;
    int p_card = mMaster.data.module.master_busaddr;

// = Initialise RDB parameters
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 800, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 800, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 100, 0, true);
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_PR1-DAC_DIP, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);

// = Configure Master channel set Vset to 4000 DAC counts
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_UP, 100, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_DOWN, 200, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_IMAX, 1, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_VSET, 4000, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_STATE, HV_STATE_ON, 0, false);

// = Configure Primary PS parameters
// sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 50, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 100, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, PR_VL, 1);

// Set Primary to 2000V

    cout << "Set Primary PS Vset=" << PR_VL << endl;
//  sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, V_PR1, 0);

    do
    {
        readModuleData(m_card, m_addr, &mMaster, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
// cout << "\033[0m\033[1m\0338Primary:" << mPrimary.data.vcur << " Master:" << mMaster.data.module.chan[m_chan].vcur << "\0338\033[0m"<< endl;
        sleep(2);
    } while ((mPrimary.data.vcur < PR_VL) || (mMaster.data.module.chan[m_chan].vcur < 4000));

    int nchans = mRDB.data.module.num_chans;

    cout << "Set 1st calibration point DAC Vset=" << V_PR1 << endl;
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_PR1-DAC_DIP, 0, false);
    for (int i=0; i<nchans; i++)
    {
        sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_PR1-DAC_DIP, 0, false);
        sendModuleCmd(card, addr, i, HV_SET_VSET, V_PR1, 0, false);
        do
        {
            readModuleData(card, addr, &mRDB, false);
//      sleep(1);
//      readMeter(Server);
        } while (mRDB.data.module.chan[i].vcur != V_PR1);
        sleep(2);                                 // wait until the voltage actually settles on the output
        calchan.V1 = 0;
        for (int j=0; j<nSamples; j++)
        {
            usleep(100000);
            readModuleData(card, addr, &mRDB, false);
            calchan.V1 += mRDB.data.module.chan[i].vmon;

        }
//    sleep(2); // no need to wait here - the voltage should be stable already
//    readMeter(Server);
        calchan.V1volt = readMeter(Server)*100;
        cout << "ch#" << i+1
            << " V1adc:" << (calchan.V1)/nSamples
            << " V1volt:" << calchan.V1volt<< endl;

        calib.push_back(calchan);
    }

    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, PR_V, 1);

// Set Primary to 4000V

    cout << "Set Primary PS Vset=" << PR_V << endl;

    do
    {
        readModuleData(m_card, m_addr, &mMaster, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
		// cout << "\033[0m\033[1m\0338Primary:" << mPrimary.data.vcur << " Master:" << mMaster.data.module.chan[m_chan].vcur << "\0338\033[0m"<< endl;
        sleep(2);
    } while ((mPrimary.data.vcur < PR_V) || (mMaster.data.module.chan[m_chan].vcur < 4000));

    cout << "Set 2nd calibration point DAC Vset=" << V_PR2 << endl;
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_PR2-DAC_DIP, 0, false);
    for (int i=0; i<nchans; i++)
    {
        sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_PR2-DAC_DIP, 0, false);
        sendModuleCmd(card, addr, i, HV_SET_VSET, V_PR2, 0, false);
        do
        {
            readModuleData(card, addr, &mRDB, false);
//      sleep(1);
//      readMeter(Server);
        } while (mRDB.data.module.chan[i].vcur != V_PR2);
        sleep(2);                                 // wait until the voltage actually settles on the output
        calib[i].V2 = 0;
        for (int j=0; j<nSamples; j++)
        {
            usleep(100000);
            readModuleData(card, addr, &mRDB, false);
            calib[i].V2 += mRDB.data.module.chan[i].vmon;
        }
//    sleep(2);
//    readMeter(Server);
        calib[i].V2volt = readMeter(Server)*100;
        cout << "ch#" << i+1
            << " V2adc:" << (calib[i].V2)/nSamples
            << " V2volt:" << calib[i].V2volt<< endl;

    }

// ===
    cout << "Calibrating RDB voltmeter channel" << endl;
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 3600, 1);
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, 0, 0, false);
    sendModuleCmd(card, addr, 0, HV_SET_VSET, 4000, 0, false);
    cout << "Set 1st point - 3600V" << endl;
    do
    {
        readModuleData(card, addr, &mRDB, false);
        readPrimaryData(p_card, p_addr, &mPrimary);

        sleep(1);
        readMeter(Server);
    } while ((mRDB.data.module.chan[0].vcur != 4000) || (mPrimary.data.vcur != 3600));
    sleep(2);
    calchan.V2 = 0;
    for (int j=0; j<nSamples; j++)
    {
        usleep(100000);
        readModuleData(card, addr, &mRDB, false);
        calchan.V2 += mRDB.data.module.hvmon;
        readMeter(Server)*100;
    }
    calchan.V2volt = readMeter(Server)*100;

    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 2000, 1);
    cout << "Set 2nd point - 2000V" << endl;
    do
    {
        readModuleData(card, addr, &mRDB, false);
        readPrimaryData(p_card, p_addr, &mPrimary);

        sleep(1);
        readMeter(Server);
    } while ((mRDB.data.module.chan[0].vcur != 4000) || (mPrimary.data.vcur != 2000));
    sleep(2);
    calchan.V1 = 0;
    for (int j=0; j<nSamples; j++)
    {
        usleep(100000);
        readModuleData(card, addr, &mRDB, false);
        calchan.V1 += mRDB.data.module.hvmon;
        readMeter(Server)*100;
    }
    calchan.V1volt = readMeter(Server)*100;

    cout << "Calculate calibration coefficients" << endl;

    cout << "V1=" << calchan.V1/nSamples << " V2=" << calchan.V2/nSamples << " V1volt=" << calchan.V1volt << " V2volt=" << calchan.V2volt << endl;
    calchan.Aadc = ((calchan.V2volt - calchan.V1volt)*Volt_coef)/(calchan.V2/nSamples-calchan.V1/nSamples);
    calchan.Badc = calchan.V2volt*Volt_coef - calchan.Aadc*(calchan.V2/nSamples);
    cout << "Voltmeter Aadc=" << calchan.Aadc << " Badc=" << calchan.Badc << endl;
// ===
    calib.push_back(calchan);

// = Calculate ADC/DAC calibration coefficients
    for (int i=0; i<nchans; i++)
    {
        float V1 = calib[i].V1/nSamples;
        float V2 = calib[i].V2/nSamples;
        float dp = V2-V1;
        if (dp==0)
        {
            cerr << "Zero difference V2-V1" << endl;
            continue;
        }
        float dpVolt = (calib[i].V2volt - calib[i].V1volt)*Volt_coef;
        calib[i].Aadc = dpVolt/dp;
        calib[i].Badc = calib[i].V2volt*Volt_coef  - calib[i].Aadc*V2;
// calib[i].Adac = (V_PR2-V_PR1)/(calib[i].Aadc*dp);
// calib[i].Bdac = V_PR2-calib[i].Adac*(calib[i].Aadc*V2+calib[i].Badc);
        calib[i].Adac = (V_PR2-V_PR1)/(dpVolt);
        calib[i].Bdac = V_PR2-calib[i].Adac*calib[i].V2volt*Volt_coef;
        cout << "ch#" << i+1
// << " dp:" << dp << " dpVolt:" << dpVolt
            << " Aadc:" << calib[i].Aadc << " Badc:" << calib[i].Badc
            << " Adac:" << calib[i].Adac << " Bdac:" << calib[i].Bdac << endl;

    }

    saveVoltageCalibrationResults(mRDB.data.module.ID,calib);

    sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_OFF, 0, false);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    cout << "Done." << endl;
    return 0;
}

// == Calibrate RDB Voltage ADC with voltage calibrator board
int CalibrateRDBVoltage_vc(int card, int addr, int vc_card, int vc_addr)
{

    cout << "Calibrating DAC/ADC  of " << modtype_str[ mRDB.data.module.type]<< " ID#" << mRDB.data.module.ID
		 << " at " << card<< ":" << addr 
		 << endl 
		 << "with Voltage Calibrator " << modtype_str[ m_vc.data.module.type]<< " ID#" << m_vc.data.module.ID
		 << " at " << vc_card<< ":" << vc_addr
		 << endl;
    int nchans = mRDB.data.module.num_chans;

	// 30-channel jumper cable has irregular pin assignment, so have to use the maps below for calibration
	int dl_map[] = {13,14,15,1,2,3,25,26,27,7,8,9,19,20,21,16,17,18,4,5,6,28,29,30,10,11,12,22,23,24};
	int ds_map[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36};
	int *vc_map;

	if (nchans == 30) vc_map = dl_map;
	else vc_map = ds_map;

    calib.clear();
    calData calchan[nchans];
	calData calchanV; // for voltmeter channel 
    int PR_V = 4000;
    int PR_VL = 2000;

    int m_card = mRDB.data.module.master_hvcard;
    int m_addr = mRDB.data.module.master_busaddr;
    int m_chan = mRDB.data.module.master_chan;
    int p_addr = mMaster.data.module.master_hvcard;
    int p_card = mMaster.data.module.master_busaddr;

// = Initialise RDB parameters
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 800, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 800, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 1, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_PR1, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);

// = Configure Master channel set Vset to 4000 DAC counts
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_UP, 100, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_DOWN, 200, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_IMAX, 1, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_VSET, 4000, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_STATE, HV_STATE_ON, 0, false);

// = Configure Primary PS parameters
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 50, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 100, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);
// Set Primary to 2000V
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, PR_VL, 1);

    cout << "Set Primary PS Vset=" << PR_VL << endl;

    do
    {
        readModuleData(m_card, m_addr, &mMaster, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
        sleep(2);
    } while ((mPrimary.data.vcur < PR_VL) || (mMaster.data.module.chan[m_chan].vcur < 4000));

    cout << "Set 1st calibration point DAC Vset=" << V_PR1 << endl;
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_PR1, 0, false);
	do
	{
		readModuleData(card, addr, &mRDB, false);
	} while (mRDB.data.module.chan[0].vcur != V_PR1); // wait until it settles
	sleep (20);

	// reset voltage ADC and Voltmeter values
	for (int i=0; i<nchans; i++)
	{
		calchan[i].V1 = 0;
		calchan[i].V1volt = 0;
	}

	// read all channels, ADC and Voltmeter values
    for (int j=0; j < nSamples; j++)
    {
		usleep(100000);
		// read board being calibrated
		readModuleData(card,    addr,    &mRDB, false);
		// read calibrator
		readModuleData(vc_card, vc_addr, &m_vc, true);
		for (int i=0; i<nchans; i++)
		{

            calchan[i].V1     += mRDB.data.module.chan[i].vmon; // ADC value 
			calchan[i].V1volt += m_vc.data.module.chan[vc_map[i]-1].vmon; // Volts
		}
	}

	for (int i=0; i<nchans; i++)
	{
		// average Volts to make compatible with old code
		calchan[i].V1volt /= nSamples;

		cout << "ch#" << i+1
			 << " V1adc:" << (calchan[i].V1)/nSamples
			 << " V1volt:" << calchan[i].V1volt << endl;
		

		calib.push_back(calchan[i]);
	}


    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, PR_V, 1);

// Set Primary to 4000V

    cout << "Set Primary PS Vset=" << PR_V << endl;

    do
    {
        readModuleData(m_card, m_addr, &mMaster, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
        sleep(2);
    } while ((mPrimary.data.vcur < PR_V) || (mMaster.data.module.chan[m_chan].vcur < 4000));

    cout << "Set 2nd calibration point DAC Vset=" << V_PR2 << endl;
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_PR2, 0, false);
	do
	{
		readModuleData(card, addr, &mRDB, false);

		cout << "current V setting: " << mRDB.data.module.chan[0].vcur << endl;
		sleep(1);

	} while (mRDB.data.module.chan[0].vcur != V_PR2); // wait until it settles
	sleep (20);

	// reset voltage ADC and Voltmeter values
	for (int i=0; i<nchans; i++)
	{
		calib[i].V2 = 0;
		calib[i].V2volt = 0;
	}

	// read all channels, ADC and Voltmeter values
    for (int j=0; j < nSamples; j++)
    {
		usleep(100000);
		// read board being calibrated
		readModuleData(card,    addr,    &mRDB, false);
		// read calibrator
		readModuleData(vc_card, vc_addr, &m_vc, true);
		for (int i=0; i<nchans; i++)
		{
            calib[i].V2     += mRDB.data.module.chan[i].vmon; // ADC value 
			calib[i].V2volt += m_vc.data.module.chan[vc_map[i]-1].vmon; // Volts
		}
	}
	for (int i=0; i<nchans; i++)
	{
		// average Volts to make compatible with old code
		calib[i].V2volt /= nSamples;

        cout << "ch#" << i+1
            << " V2adc:" << (calib[i].V2)/nSamples
            << " V2volt:" << calib[i].V2volt<< endl;
	}


// ===
    cout << "Calibrating RDB voltmeter channel" << endl;
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 3600, 1);
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, 0, 0, false);
    sendModuleCmd(card, addr, 0, HV_SET_VSET, 4000, 0, false);
    cout << "Set 1st point - 3600V" << endl;
    do
    {
        readModuleData(card, addr, &mRDB, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
        sleep(1);
    } while ((mRDB.data.module.chan[0].vcur != 4000) || (mPrimary.data.vcur != 3600));
    sleep(2);
    calchanV.V2 = 0;
    calchanV.V2volt = 0;
    for (int j=0; j<nSamples; j++)
    {
        usleep(100000);
		// read calibrated board
        readModuleData(card, addr, &mRDB, false); 
		// read calibrator
		readModuleData(vc_card, vc_addr, &m_vc, true);

        calchanV.V2     += mRDB.data.module.hvmon;
		calchanV.V2volt += m_vc.data.module.chan[0].vmon; // take volts from channel 0 - should be the same on all channels
    }

    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 2000, 1);
    cout << "Set 2nd point - 2000V" << endl;
    do
    {
        readModuleData(card, addr, &mRDB, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
        sleep(1);
    } while ((mRDB.data.module.chan[0].vcur != 4000) || (mPrimary.data.vcur != 2000));
    sleep(2);
    calchanV.V1 = 0;
    calchanV.V1volt = 0;
    for (int j=0; j<nSamples; j++)
    {
        usleep(100000);
		// read calibrated board
        readModuleData(card, addr, &mRDB, false); 
		// read calibrator
		readModuleData(vc_card, vc_addr, &m_vc, true);

        calchanV.V1     += mRDB.data.module.hvmon;
		calchanV.V1volt += m_vc.data.module.chan[0].vmon;
    }

	// average volts to make compatible with older code below
	calchanV.V1volt /= nSamples;
	calchanV.V2volt /= nSamples;

    cout << "Calculate calibration coefficients" << endl;

    cout <<  "V1=" << calchanV.V1/nSamples 
		 << " V2=" << calchanV.V2/nSamples 
		 << " V1volt=" << calchanV.V1volt 
		 << " V2volt=" << calchanV.V2volt << endl;

    calchanV.Aadc = (calchanV.V2volt - calchanV.V1volt)/(calchanV.V2/nSamples-calchanV.V1/nSamples);
    calchanV.Badc = calchanV.V2volt - calchanV.Aadc*(calchanV.V2/nSamples);
    cout << "Voltmeter Aadc=" << calchanV.Aadc << " Badc=" << calchanV.Badc << endl;
// ===
    calib.push_back(calchanV);

// = Calculate ADC/DAC calibration coefficients
    for (int i=0; i<nchans; i++)
    {
        float V1 = calib[i].V1/nSamples;
        float V2 = calib[i].V2/nSamples;
        float dp = V2-V1;
        if (dp==0)
        {
            cerr << "Zero difference V2-V1" << endl;
            continue;
        }
        float dpVolt = (calib[i].V2volt - calib[i].V1volt);
        calib[i].Aadc = dpVolt/dp;
        calib[i].Badc = calib[i].V2volt  - calib[i].Aadc*V2;
        calib[i].Adac = (V_PR2-V_PR1)/(dpVolt);
        calib[i].Bdac = V_PR2-calib[i].Adac*calib[i].V2volt;
        cout << "ch#" << i+1
            << " Aadc:" << calib[i].Aadc << " Badc:" << calib[i].Badc
            << " Adac:" << calib[i].Adac << " Bdac:" << calib[i].Bdac << endl;

    }

    saveVoltageCalibrationResults(mRDB.data.module.ID,calib);

  sendModuleCmd(card, addr, ALL, HV_SET_VSET, 0, 0, true);
  sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0, true);
  sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_OFF, 0, true);

  sendModuleCmd(m_card, m_addr, m_chan,HV_SET_VSET, 0, 0, false);
  sendModuleCmd(m_card, m_addr, m_chan,HV_SET_MOD_STATE, HV_STATE_OFF, 0, false);
  sendModuleCmd(m_card, m_addr, m_chan,HV_SET_STATE, HV_STATE_OFF, 0, false);

    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    cout << "Done." << endl;
    return 0;
}


typedef struct curData
{
    unsigned long     val;
    int       samples;
} curData;

int saveCurrentCalibrationResults(int ID, curData data[][1001])
{
    ofstream ofs;

	string prepath = "/home/hvuser/data/Remote_";
    ostringstream st;

    st << prepath << setw(3) << setfill('0') << ID << "/"
	   << "T09_M" << setw(3) << setfill('0') << ID << ".log";

    ofs.open(st.str().c_str(), ios_base::app);
    ofs << "========================================================\n"
        << "Test9 (Current ADC Calibration) results " << now()
        << "\nBoard ID" << ID << " " << mRDB.data.module.num_chans << " channels\n"
		<< "Host: " << getenv("HOST") << "\n"
        << "Linux calibration software\n\n"
        << "Current Tab:" << endl;
    ofs.setf(ios_base::fixed);
    int val;
    for (int j=0; j<1001; j++)
    {
        for (int i=0; i<mRDB.data.module.num_chans; i++)
        {
            if (data[i][j].samples>0) val = data[i][j].val/data[i][j].samples;
            else val=data[i][j].val;
            ofs << val << ((i<(mRDB.data.module.num_chans-1))?",":"");
        }
        ofs << endl;
    }
    ofs.close();
}


int smoothCalibData(curData data[][1001])
{
    int nchans = mRDB.data.module.num_chans;
    int cnt=0;
    bool fFillLast=false;
    bool fFillFirst=false;
    int val2=0;
    int val1=0;
    float step=0;
    for (int i=0; i<nchans; i++)
    {
// cout << i << endl;
        for (int j=1000; j>=0; j--)
        {
            if (data[i][j].val==0 && j>0)
            {
                if (j==1000) fFillLast = true;
                cnt++;
            }
            else
            {
                if (j==0 && data[i][j].val==0)    fFillFirst = true;
                val1=data[i][j].val/data[i][j].samples;
                if (fFillLast)
                {
                    val2=val1-cnt;
                    step=1;
                    fFillLast=false;
                }
                else if (fFillFirst)
                {
                    val1=val2+cnt;
                    step=1;
                    data[i][j].val = val1;
                    data[i][j].samples = 1;
                    fFillFirst=false;
                }
                else
                {
                    if (cnt>0)
                        step=(val1-val2)/(cnt+1);

                }
// cout << cnt << " " << step <<  endl;
                while (cnt>0)
                {
                    data[i][j+cnt].val = val1-((int)rint(cnt*step));
                    data[i][j+cnt].samples = 1;
//	cout << data[i][j+cnt].val << endl;
                    cnt--;
                }
                val2=val1;
            }

        }

    }

}


int extrapolateCalibData(curData data[][1001])
{
    int p1 = 350;
    int p2 = 380;
    int nchans = mRDB.data.module.num_chans;
    for (int i=0; i<nchans; i++)
    {
// Linear Extarpolation of data A*x+B
        int val1 =data[i][p1].val/data[i][p1].samples;
        int val2 =data[i][p2].val/data[i][p2].samples;
        int B = val2;
        float A=AvgSlope;
        if (!fFixedSlope)
            A = (val1-val2)/((float)(p2-p1));
        cout << "Ch" << i+1 << " p2=" << val2 << " p1=" << val1
            << " A=" << A << " B=" << B << endl;
        for (int j=p2+1; j<1001; j++)
        {
            data[i][j].samples = 1;
            data[i][j].val = B-(int)rint(A*(j-p2));
        }
    }
    return 0;
}

int interpol
(
	curData data[][1001], 
	int n, // total number of calib points
	int p, // number of precisely calibrated points
	int s  // step of rough calibration
)
{
    int nchans = mRDB.data.module.num_chans;
	int ci, ni;
	float cv, nv, slope;

    for (int i = 0; i < nchans; i++) // channel loop
    {
		for (int j = p; j < n-1; j++) // calib point loop (don't do last point as the loop goes one point ahead)
		{
			ci = (j - p)*s + p; // index of current rough point in curData
			ni = ci + s; // next rough point
			cv = (float)data[i][ci].val/(float)data[i][ci].samples; // current value
			nv = (float)data[i][ni].val/(float)data[i][ni].samples; // next value
		    slope = (nv - cv)/(float)s; // slope between ci and ni

			for (int k = 1; k < s; k++) // assign all points in between
			{
				data[i][ci + k].val = (long unsigned)(cv + slope * (float)k);
				data[i][ci + k].samples = 1;
			}
		}
	}

    return 0;
}

int master_interpol
(
	curData data[][4001], 
	int n, // total number of calib points
	int p, // number of precisely calibrated points
	int s  // step of rough calibration
)
{
    int nchans = mMaster.data.module.num_chans;
	int ci, ni;
	float cv, nv, slope;

    for (int i = 0; i < nchans; i++) // channel loop
    {
		for (int j = p; j < n-1; j++) // calib point loop (don't do last point as the loop goes one point ahead)
		{
			ci = (j - p)*s + p; // index of current rough point in curData
			ni = ci + s; // next rough point
			cv = (float)data[i][ci].val/(float)data[i][ci].samples; // current value
			nv = (float)data[i][ni].val/(float)data[i][ni].samples; // next value
		    slope = (nv - cv)/(float)s; // slope between ci and ni

			for (int k = 1; k < s; k++) // assign all points in between
			{
				data[i][ci + k].val = (long unsigned)(cv + slope * (float)k);
				data[i][ci + k].samples = 1;
			}
		}
	}

    return 0;
}

int loadCurrentCalibrationResults(int ID)
{
    ifstream curfile;

    ostringstream st;

    st << "T09_M" << setw(3) << setfill('0') << ID << ".dat";
    int i;
    bool fC;
    curData C[mRDB.data.module.num_chans][1001];

    memset(C, 0, sizeof(C));
    char line[255];
    curfile.open(st.str().c_str());
    if (curfile)
    {
        while(!curfile.eof())
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
                    C[ch][i].val = (int)t;
                    C[ch][i].samples = 1;
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
            }
        }
        curfile.close();
    }
/*
  for (int i=0;i<1001;i++) {
  cout << i << ": ";
  cout.setf(ios_base::fixed);
  int val;
  for(int j=0; j< mRDB.data.module.num_chans; j++) {
  val = C[j][i].val/C[j][i].samples;///nSamples;
  cout << val << ",";
  }
  cout << endl;
  }
*/
    smoothCalibData(C);
    saveCurrentCalibrationResults(ID,C);

}


int CalibrateCurrentADC(int card, int addr)
{
    curData curcalib[36][1001];
    int m_card = mRDB.data.module.master_hvcard;
    int m_addr = mRDB.data.module.master_busaddr;
    int m_chan = mRDB.data.module.master_chan;
    int p_addr = mMaster.data.module.master_hvcard;
    int p_card = mMaster.data.module.master_busaddr;

// = Initialise RDB parameters
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 150, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 300, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 1, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, 4000, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);

// = Configure Master channel set Vset to 4000 DAC counts
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_UP, 100, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_DOWN, 200, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_IMAX, 1, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_VSET, 4000, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_STATE, HV_STATE_ON, 0, false);

// = Configure Primary PS parameters
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 50, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_DOWN, 200, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 0, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);

    cout << "Set Master and RDB" << endl;

    do
    {
        readModuleData(m_card, m_addr, &mMaster, false);
        readModuleData(card, addr, &mRDB, false);
        sleep(2);
    } while ((mRDB.data.module.chan[0].vcur != 4000 ) || (mMaster.data.module.chan[m_chan].vcur != 4000));

    struct timeval t1, t2, t3;

    calData calchan;

    for (int j=0; j< mRDB.data.module.num_chans; j++)
    {
        calib.push_back(calchan);
    }

    memset(curcalib,0, sizeof(curcalib));
    int npoints = 1130;
    if (fCurrentCalibOldMethod) npoints=1001;
    if (fCalib100M) npoints=381;
    for (int i=0; i<npoints; i++)
    {
        gettimeofday(&t1, NULL);
        int pvset=i;
        if (fCalib100M) pvset = 10*i;
        sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, pvset, 1);
// gettimeofday(&t2, NULL);

        if (fCalib100M)
        {
            float delta = 0.0;
            float step = 3;
            do
            {
                usleep(100000);
                readPrimaryData(p_card, p_addr, &mPrimary);
                if (mPrimary.data.vset) delta = (mPrimary.data.vset - mPrimary.data.vmon);

//       if (fabs(delta) < step) cout << delta << endl;
            } while (mPrimary.data.vcur != pvset || fabs(delta)>(step+mPrimary.data.vset/200));
        }
        else
        {
            do
            {
                usleep(100000);
                readPrimaryData(p_card, p_addr, &mPrimary);
            } while (mPrimary.data.vcur != pvset);

        }
        gettimeofday(&t2, NULL);
        cout << "Set primary V time: " << ((t2.tv_sec+t2.tv_usec/1000000.) - (t1.tv_sec+t1.tv_usec/1000000.)) << endl;

        cout << "HVset: " << i;

        for (int n=0; n<nSamples; n++)
        {
            readModuleData(card, addr, &mRDB, false);
            usleep(5000);

            for(int j=0; j< mRDB.data.module.num_chans; j++)
            {
/*
  calib[j].Aadc = 1.09;
  calib[j].Badc = -10;
  int volt = (int)rint(mRDB.data.module.chan[j].vmon*calib[j].Aadc +calib[j].Badc);
  if (volt<0 || i==0) volt=0;
*/
                int volt = i;
                if (!fCurrentCalibOldMethod)
                {
                    calib[j].Aadc = 1.09;
                    calib[j].Badc = -10;
                    int volt = (int)rint(mRDB.data.module.chan[j].vmon*calib[j].Aadc +calib[j].Badc);
                    if (volt<0 || i==0) volt=0;
                }
                if (volt<1001)
                {
                    curcalib[j][volt].val += mRDB.data.module.chan[j].imon;
                    curcalib[j][volt].samples++;
                }
// cout << volt << ":" << curcalib[j][volt] << ",";

// curcalib[j][i] += mRDB.data.module.chan[j].imon;
            }
        }
        cout << endl;
/*gettimeofday(&t3, NULL);
  cout << "Read RDB samples time:  "<< ((t3.tv_sec+t3.tv_usec/1000000.) - (t2.tv_sec+t2.tv_usec/1000000.)) << endl;
  cout << "Sum time: "<< ((t3.tv_sec+t3.tv_usec/1000000.) - (t1.tv_sec+t1.tv_usec/1000000.)) << endl;
*/

/*
  cout << i << ": ";
  cout.setf(ios_base::fixed);
  float val;
  for(int j=0; j< mRDB.data.module.num_chans; j++) {
  val = curcalib[j][i].val/curcalib[j][i].samples;///nSamples;
  cout << setprecision(1) << val << ",";
  }
  cout << endl;
*/
    }
    if (!fCurrentCalibOldMethod) smoothCalibData(curcalib);
    if (fCalib100M) extrapolateCalibData(curcalib);
    saveCurrentCalibrationResults(mRDB.data.module.ID,curcalib);

    sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_OFF, 0, false);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    return 0;

}


int CalibrateCurrentADC_interpol(int card, int addr)
{
	curData curcalib[36][1001];

	int m_card;
	int m_addr;
	int m_chan;
	// if direct primary-distr connection, assume single primary at 1:1
	int p_addr = 1;
	int p_card = 1;

	if (!f_calib_wo_master)
	{
		m_card = mRDB.data.module.master_hvcard;
		m_addr = mRDB.data.module.master_busaddr;
		m_chan = mRDB.data.module.master_chan;
		p_addr = mMaster.data.module.master_hvcard;
		p_card = mMaster.data.module.master_busaddr;
	}
	// = Initialise RDB parameters
	sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 500, 0, false);
	sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 500, 0, false);
	sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 1, 0, false);
	sendModuleCmd(card, addr, ALL, HV_SET_VSET, 4000, 0, false);
	sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
	sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);
  
	if (!f_calib_wo_master)
	{
		// = Configure Master channel set Vset to 4000 DAC counts
		sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_UP, 500, 0, false);
		sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_DOWN, 500, 0, false);
		sendModuleCmd(m_card, m_addr, m_chan,HV_SET_IMAX, 1, 0, false);
		sendModuleCmd(m_card, m_addr, m_chan,HV_SET_VSET, 4000, 0, false);
		sendModuleCmd(m_card, m_addr, m_chan,HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
		sendModuleCmd(m_card, m_addr, m_chan,HV_SET_STATE, HV_STATE_ON, 0, false);
	}

	// = Configure Primary PS parameters
	sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
	sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 200, 0);
	sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_DOWN, 200, 0);
	sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 0, 0);
	sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);

	cout << "Set Master and RDB" << endl;

	do
    {
		if (!f_calib_wo_master) 
			readModuleData(m_card, m_addr, &mMaster, false);
		readModuleData(card, addr, &mRDB, false);
		sleep(2);
    } 
	while 
	(
		(mRDB.data.module.chan[0].vcur != 4000 ) || 
		(!f_calib_wo_master && (mMaster.data.module.chan[m_chan].vcur != 4000))
	);

	struct timeval t1, t2, t3;

	calData calchan;

	for (int j=0; j< mRDB.data.module.num_chans; j++)
    {
		calib.push_back(calchan);
    }

	memset(curcalib,0, sizeof(curcalib));

	int npoints = 48; // 48 points
	int precise = 10; // first 10 poins - every 100 nA

	for (int i=0; i<npoints; i++)
    {
		gettimeofday(&t1, NULL);
		int pvset, volt;
		// every 100 na first 'precise' points, then 1 uA
		pvset = (i <= precise) ? 10*i : 100*(i-precise) + precise*10; 
		sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, pvset, 1);

		float delta = 0.0;
		float step = 5;
		int timeout = 0;
		cout << "primary set to: " << pvset << "  waiting for it to reach voltage" << endl;
		do
		{
			usleep(100000);
			readPrimaryData(p_card, p_addr, &mPrimary);
			if (mPrimary.data.vset) delta = (mPrimary.data.vset - mPrimary.data.vmon);
			timeout++;
			if (timeout > 100) // waiting for more than 10 sec, bug user
			{
				cout << "time out waiting for primary to set. pvset: " << pvset << "   mPrimary.data.vcur: " << mPrimary.data.vcur
					 << "delta: " << delta << endl;
			}
			
		} while (mPrimary.data.vcur != pvset || fabs(delta)>(step+mPrimary.data.vset/200));

		gettimeofday(&t2, NULL);

		volt = pvset/10; // volt is actually storage index
		cout << "Voltage: " << pvset << "\tstoring: " << volt << "\tpoint:  " << i;
		cout << "\tPrimary setting time: " << ((t2.tv_sec+t2.tv_usec/1000000.) - (t1.tv_sec+t1.tv_usec/1000000.)) << endl;

		for (int n=0; n<nSamples; n++)
        {
			readModuleData(card, addr, &mRDB, false);
			usleep(20000);

			for(int j=0; j< mRDB.data.module.num_chans; j++)
            {
				curcalib[j][volt].val += mRDB.data.module.chan[j].imon;
				curcalib[j][volt].samples++;
            }
        }
    }

	interpol(curcalib, npoints, precise, 10);// 10 is step of rough calibration (10 points)
	extrapolateCalibData(curcalib);
	saveCurrentCalibrationResults(mRDB.data.module.ID,curcalib);

	sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
	sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_OFF, 0, false);
	return 0;

}

void include_exclude_db()
{
	// read list of boards to calibrate
	ifstream include_db_f("/home/hvuser/data/include_db.txt");

	if (include_db_f.is_open())
	{
		int dbn;
		while (!include_db_f.eof())
		{
			include_db_f >> dbn;
			include_db.insert(dbn);
		}

		include_db_f.close();
	}

	// read list of boards to exclude
	ifstream exclude_db_f("/home/hvuser/data/exclude_db.txt");

	if (exclude_db_f.is_open())
	{
		int dbn;
		while (!exclude_db_f.eof())
		{
			exclude_db_f >> dbn;
			exclude_db.insert(dbn);
		}

		exclude_db_f.close();
	}

	// print included and excluded boards
	if (include_db.empty()) cout << "all boards included" << endl;
	else
	{
		cout << "included boards: ";
		set<int>:: iterator it;
		for (it = include_db.begin(); it != include_db.end(); it++)
			cout << *it << " ";

		cout << endl;
	}

	if (exclude_db.empty()) cout << "no excluded boards" << endl;
	else
	{
		cout << "excluded boards: ";
		set<int>:: iterator it;
		for (it = exclude_db.begin(); it != exclude_db.end(); it++)
			cout << *it << " ";

		cout << endl;
	}
}

void read_primary_calibrations()
{
	// read primary calibrations
	ifstream primcal("/home/hvuser/data/adc_calib_primary_points.txt");
	
	if (primcal.is_open())
	{
		string buf;
		int addr, p1_set, p1_v, p2_set, p2_v;

		while (!primcal.eof())
		{
			getline(primcal, buf);
			if (buf[0] == '#') continue; // skip comment lines
			istringstream bufstr(buf);
			bufstr >> addr >> p1_set >> p1_v >> p2_set >> p2_v;
			// validate a bit
			if 
			(
				addr < 0 || addr > 4 ||
				p1_set < 0 || p1_set > 2000 ||
				p1_v   < 0 || p1_v   > 2000 ||
				p2_set < 2000 || p2_set > 3800 ||
				p2_v   < 2000 || p2_v   > 3800
			)
			{
				cout << "error in primary calib file, line: " << buf;
				exit(0);
			}

			// store
			pr_set[addr][0] = p1_set;
			pr_v[addr][0]   = p1_v;
			pr_set[addr][1] = p2_set;
			pr_v[addr][1]   = p2_v;
		}

		cout << "primary calibration points:" << endl;
		for (int i = 0; i < 5; i++)
		{
			cout << i << " " << pr_set[i][0] << " " << pr_v[i][0] << " " << pr_set[i][1] << " " << pr_v[i][1] << endl;
		}

		primcal.close();
	}
	else 
	{
		cout << "cannot read primary calibrations from /home/hvuser/data/adc_calib_primary_points.txt" << endl;
	}
}

void find_distr_masters_primaries()
{
	// scan all hostcards and addresses to find all boards, fill distr and master lists
	for (int hi = 0; hi < MAX_HC; hi++) // host card loop
	{
		for (int ai = 0; ai < MAX_A; ai++) // address loop
		{
			HVboardDIM* aboard = new HVboardDIM;
			if (readModuleData(hi, ai, aboard, false) == 0)
			{
				if (aboard->data.module.type != NONE)
				{
					// found a board

					if (aboard->data.module.type == MASTER)
					{
						// it's master
						master.push_back(aboard);
						cout << "found master " << aboard->data.module.ID << " at: " << hi << ":" << ai << endl;
					}
					else
					{
						// distr
						// check master link
						int m_card = aboard->data.module.master_hvcard;
						int m_addr = aboard->data.module.master_busaddr;
						int m_chan = aboard->data.module.master_chan;
						int m_lock = aboard->data.module.interlock;

						// check if this board was included or excluded
						bool include_all = include_db.empty();
						bool include_this = (include_db.find(aboard->data.module.ID) != include_db.end());
						bool exclude_this = (exclude_db.find(aboard->data.module.ID) != exclude_db.end());

						// add to list only if master link is valid and ilock is OK and not excluded
						// also add to list if user wants to calibrate w/o master link (direct connection to primary)
						if 
						(
							((m_card != NA && m_addr != NA && m_chan != NA && m_lock == 1) || f_calib_wo_master) &&
							(include_all || include_this) && !exclude_this
						)
						{
							distr.push_back(aboard);
							cout << "found distr " << aboard->data.module.ID << " at: " << hi << ":" << ai << endl;
						}
						else
						{
							// distr boards w/o master link not added to list, not calibrated
							// report only if included and not excluded
							if ((include_all || include_this) && !exclude_this)
								cout << "found distr " << aboard->data.module.ID << " w/o master link at: " << hi << ":" << ai << endl;
						}
					}

				}
				else delete aboard; // no board at this address
			}
			else delete aboard; // no board at this address
		}
	}

	// scan all com ports to find primaries
	for (int ci = 0; ci < MAX_COM; ci++) // host card loop
	{
		for (int ai = 0; ai < MAX_CA; ai++) // address loop
		{
			HVPrimaryDIM* aprimary = new HVPrimaryDIM;
		    if (readPrimaryData(ci, ai, aprimary) == 0)
			{
				cout << "found primary at: " << ci << ":" << ai << endl;
				primary.push_back (aprimary);
			}
			else delete aprimary;
		}
	}
}



void CalibrateCurrentADC_interpol_mult()
{
	int npoints = 48; // 48 points
	int precise = 10; // first 10 poins - every 100 nA
	int brdn;
	struct timeval t1, t2, t3;
	curData curcalib[9][36][1001];
	cout << "global distribution board ADC calibration" << endl;

	// read included and excluded chambers
	include_exclude_db();

	read_primary_calibrations();

	// find all boards and primaries, put them into lists
	find_distr_masters_primaries();

	if (distr.empty())
	{
		cout << "no distr. boards to calibrate" << endl;
		exit (0);
	}

	// open all distr boards wide
	list <HVboardDIM*>::iterator it;
	for (it = distr.begin(); it != distr.end(); it++)
	{
		HVboardDIM* dboard = *it;
		int card = dboard->data.card.dataslot;
		int addr = dboard->data.module.busaddr;

		cout << "setting up distr at: " << card << ":" << addr << endl;

		sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 500, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 500, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 1, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_VSET, 4000, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);
	
	}

	// primary directly connected, no masters
	

	// prepare primary, assume only one for this test
	int p_card = 1;
	int p_addr = 1;
	list <HVPrimaryDIM*>::iterator pit;
	for (pit = primary.begin(); pit != primary.end(); pit++)
	{
		HVPrimaryDIM* prim = *pit;
		p_card = prim->data.port;
		p_addr = prim->data.addr;
	}

	sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
	sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 200, 0);
	sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_DOWN, 200, 0);
	sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 0, 0);
	sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);

	cout << "Set Master and RDB" << endl;

	brdn = 0;
	for (it = distr.begin(); it != distr.end(); it++)
	{
		if (brdn > 8) break; // cannot calibrate more than 9 boards

		HVboardDIM* dboard = *it;
		int card = dboard->data.card.dataslot;
		int addr = dboard->data.module.busaddr;
		do
		{
			readModuleData(card, addr, dboard, false);
			sleep(2);
		} 
		while (dboard->data.module.chan[0].vcur != 4000);
	}
	
	// clear calibration array
	memset(curcalib,0, sizeof(curcalib));

	for (int i=0; i<npoints; i++)
    {
		gettimeofday(&t1, NULL);
		int pvset, volt;
		// every 100 na first 'precise' points, then 1 uA
		pvset = (i <= precise) ? 10*i : 100*(i-precise) + precise*10; 
		sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, pvset, 1);

		float delta = 0.0;
		float step = 5;
		int timeout = 0;
		do
		{
			usleep(100000);
			readPrimaryData(p_card, p_addr, &mPrimary);
			if (mPrimary.data.vset) delta = (mPrimary.data.vset - mPrimary.data.vmon);
			timeout ++;
			if (timeout > 100) // waiting for more than 10 sec, bug user
			{
				cout << "time out waiting for primary to set. pvset: " << pvset << "   mPrimary.data.vcur: " << mPrimary.data.vcur
					 << "  delta: " << delta << endl;
			}
			
		} while (mPrimary.data.vcur != pvset || fabs(delta)>(step+mPrimary.data.vset/200));

		gettimeofday(&t2, NULL);

		volt = pvset/10; // volt is actually storage index
		cout << "Voltage: " << pvset << "\tstoring: " << volt << "\tpoint:  " << i;
		cout << "\tPrimary setting time: " << ((t2.tv_sec+t2.tv_usec/1000000.) - (t1.tv_sec+t1.tv_usec/1000000.)) << endl;

		for (int n=0; n<nSamples; n++)
        {
			brdn = 0;
			for (it = distr.begin(); it != distr.end(); it++)
			{
				if (brdn > 8) break; // cannot calibrate more than 9 boards

				HVboardDIM* dboard = *it;
				int card = dboard->data.card.dataslot;
				int addr = dboard->data.module.busaddr;
				readModuleData(card, addr, dboard, false);

				for(int j=0; j< dboard->data.module.num_chans; j++)
				{

					curcalib[brdn][j][volt].val += dboard->data.module.chan[j].imon;
					curcalib[brdn][j][volt].samples++;
				}
				brdn++;
			}
			usleep(20000);
        }
    }

	sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);

	brdn = 0;
	for (it = distr.begin(); it != distr.end(); it++)
	{
		if (brdn > 8) break; // cannot calibrate more than 9 boards
		HVboardDIM* dboard = *it;
		int card = dboard->data.card.dataslot;
		int addr = dboard->data.module.busaddr;
		readModuleData(card, addr, &mRDB, false);
		// procedures below use mRDB object
		interpol(curcalib[brdn], npoints, precise, 10);// 10 is step of rough calibration (10 points)
		extrapolateCalibData(curcalib[brdn]);
		saveCurrentCalibrationResults(dboard->data.module.ID, curcalib[brdn]);
		brdn++;
	}
}



int saveLowRangeVoltageCalibrationResults(int ID, vector<calData>& data)
{

    ofstream ofs;

    ostringstream st;
    if (data.size() <1)
    {
        cerr << "Empty results data" << endl;
        return -1;
    }

    st << "LowRange_M" << setw(3) << setfill('0') << ID << ".log";

    int delim = data.size()/3;

    ofs.open(st.str().c_str(), ios_base::app);
    ofs << "========================================================\n"
        << "Low Range ADC Calibration results " << now()
        << "\nBoard ID" << ID << " " << data.size()-1 << " channels\n" << endl;

    ofs << "A:" << endl;
    for (int i=0; i<data.size()-1; i++)
    {
        ofs << data[i].Aadc << ((i && ((i+1)%delim==0))?"\n":",");
    }
    ofs << "\n" << endl;
    ofs << "B:" << endl;
    for (int i=0; i<data.size()-1; i++)
    {
        ofs << data[i].Badc << ((i && ((i+1)%delim==0))?"\n":",");
    }
    ofs << "\n" << endl;
    ofs.close();
    st.clear();
    st.flush();
    st.seekp(0);
    st << "LowRangeCalib_M" << setw(3) << setfill('0') << ID << ".dat";
    ofs.open(st.str().c_str(), ios_base::app);
    ofs << "========================================================\n"
        << "ADC/DAC Calibration) data " << now()
        << "\nBoard ID" << ID << " " << data.size()-1 << " channels\n"
        << "Linux calibration software\n" << endl;
    for (int i=0; i<data.size(); i++)
    {
        ofs << "ch#" << i+1 << " V1=" << data[i].V1/nSamples << " V2=" << data[i].V2/nSamples
            << " V1volt=" << data[i].V1volt << " V2volt=" << data[i].V2volt << endl;;
    }
    ofs.close();
    return 0;
}


// == Calibrate RDB Low Range Voltage ADC
int CalibrateLowRangeADC(int card, int addr)
{

    cout << "Calibrating DAC/ADC of "<< modtype_str[ mRDB.data.module.type]<< " ID#" << mRDB.data.module.ID
        << " at " << card<< ":" << addr << endl;
// vector<calData> calib;
    calib.clear();
    calData calchan;

    int m_card = mRDB.data.module.master_hvcard;
    int m_addr = mRDB.data.module.master_busaddr;
    int m_chan = mRDB.data.module.master_chan;
    int p_addr = mMaster.data.module.master_hvcard;
    int p_card = mMaster.data.module.master_busaddr;
    int nchans = mRDB.data.module.num_chans;

// = Initialise RDB parameters
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 150, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 300, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 10, 0, true);
// sendModuleCmd(card, addr, ALL, HV_SET_VSET, 4000, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, 0, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);

// = Configure Master channel set Vset to 4000 DAC counts
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_UP, 100, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_DOWN, 200, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_IMAX, 1, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_VSET, 4000, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_STATE, HV_STATE_ON, 0, false);

// = Configure Primary PS parameters
// sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 50, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 100, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);

    int V_PR1=300;
    cout << "Set 1st calibration point " << V_PR1-200 << "V" << endl;
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, V_PR1-200, 1);

    do
    {
        sleep(1);
        readModuleData(m_card, m_addr, &mMaster, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
    } while ((mPrimary.data.vcur != V_PR1-200)||(mMaster.data.module.chan[m_chan].vcur<4000)) ;

    sendModuleCmd(card, addr, ALL, HV_SET_VSET, 0, 0, false);
    for (int i=0; i<nchans; i++)
    {
        sendModuleCmd(card, addr, ALL, HV_SET_VSET, 0, 0, false);
        sendModuleCmd(card, addr, i, HV_SET_VSET, V_PR1, 0, false);
        do
        {
            readModuleData(card, addr, &mRDB, false);
            sleep(1);
            readMeter(Server);
        } while (mRDB.data.module.chan[i].vcur != V_PR1);
        calchan.V1 = 0;
        for (int j=0; j<nSamples; j++)
        {
            usleep(100000);
            readModuleData(card, addr, &mRDB, false);
            calchan.V1 += mRDB.data.module.chan[i].vmon;

        }
        sleep(2);
        readMeter(Server);
        calchan.V1volt = readMeter(Server)*100;
        cout << "ch#" << i+1
            << " V1adc:" << (calchan.V1)/nSamples
            << " V1volt:" << calchan.V1volt<< endl;

        calib.push_back(calchan);
    }

/*
  sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 0, 0);

  do {
  sleep(1);
  readModuleData(m_card, m_addr, &mMaster, false);
  readPrimaryData(p_card, p_addr, &mPrimary);
  } while ((mPrimary.data.vmon > 2.0)||(mMaster.data.module.chan[m_chan].vcur<4000)) ;

  bool fADCAboveZero = true;
  int i=0;
do {
fADCAboveZero = true;
sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, ++i, 0);
cout << "Set Primary Vset=" << i << " V" << endl;
sleep(3);
readModuleData(card, addr, &mRDB, false);
for (int ch=0; ch < nchans; ch++) {
fADCAboveZero &= (mRDB.data.module.chan[ch].vmon > 0);
}
} while (!fADCAboveZero);

sleep(3);
readPrimaryData(p_card, p_addr, &mPrimary);
readModuleData(card, addr, &mRDB, false);
calchan.V1volt = readMeter(Server)*100;
calchan.V1pr = mPrimary.data.vmon;
for (int ch=0; ch < nchans; ch++) {
calchan.V1 = mRDB.data.module.chan[ch].vmon;
calib.push_back(calchan);
cout << ch << ": " << calchan.V1 << " " << calchan.V1volt << " " << calchan.V1pr << endl;
}

*/
    int V_PR2=900;

    cout << "Set 2nd calibration point " << V_PR2-200 << "V" << endl;
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, V_PR2-200, 1);
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_PR2-400, 0, false);
    do
    {
        sleep(1);
        readModuleData(card, addr, &mRDB, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
    } while ((mPrimary.data.vcur != (V_PR2-200)) || (mRDB.data.module.chan[0].vcur != V_PR2-400));

    for (int i=0; i<nchans; i++)
    {
        sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_PR2-400, 0, false);
        sendModuleCmd(card, addr, i, HV_SET_VSET, V_PR2, 0, false);
        do
        {
            readModuleData(card, addr, &mRDB, false);
            sleep(1);
            readMeter(Server);
        } while (mRDB.data.module.chan[i].vcur != V_PR2);
        calib[i].V2 = 0;
        for (int j=0; j<nSamples; j++)
        {
            usleep(100000);
            readModuleData(card, addr, &mRDB, false);
            calib[i].V2 += mRDB.data.module.chan[i].vmon;
        }
        sleep(2);
        readMeter(Server);
        calib[i].V2volt = readMeter(Server)*100+1;
        cout << "ch#" << i+1
            << " V2adc:" << (calib[i].V2)/nSamples
            << " V2volt:" << calib[i].V2volt<< endl;

    }

    for (int i=0; i<nchans; i++)
    {
        float V1 = calib[i].V1/nSamples;
        float V2 = calib[i].V2/nSamples;
        float dp = V2-V1;
        if (dp==0)
        {
            cerr << "Zero difference V2-V1" << endl;
            continue;
        }
        float dpVolt = (calib[i].V2volt - calib[i].V1volt);
        calib[i].Aadc = dpVolt/dp;
        calib[i].Badc = calib[i].V2volt - calib[i].Aadc*V2;
        cout << "ch#" << i+1
// << " dp:" << dp << " dpVolt:" << dpVolt
            << " Aadc:" << calib[i].Aadc << " Badc:" << calib[i].Badc << endl;

    }

    saveLowRangeVoltageCalibrationResults(mRDB.data.module.ID,calib);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 0, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    do
    {
        sleep(2);
        readPrimaryData(p_card, p_addr, &mPrimary);
    } while (mPrimary.data.vmon > 2.0) ;

    return 0;

}


int saveMasterVoltageCalibrationResults(int ID, vector<calData>& data)
{

    ofstream ofs;

    ostringstream st;
    if (data.size() <1)
    {
        cerr << "Empty results data" << endl;
        return -1;
    }

    st << "T15_M" << setw(3) << setfill('0') << ID << ".log";

    ofs.open(st.str().c_str(), ios_base::app);
    ofs << "========================================================\n"
        << "Test15 (Master ADC Calibration) results " << now()
        << "\nBoard ID" << ID << " " << data.size()-1 << " channels\n"
        << "Linux calibration software\n\n" << endl;

    ofs << "A:" << endl;
    for (int i=0; i<data.size(); i++)
    {
        ofs << data[i].Aadc << ((i<(data.size()-1))?",":"");
    }
    ofs << "\n" << endl;
    ofs << "B:" << endl;
    for (int i=0; i<data.size(); i++)
    {
        ofs << data[i].Badc << ((i<(data.size()-1))?",":"");
    }
    ofs << "\n" << endl;
    ofs.close();

    st.clear();
    st.flush();
    st.seekp(0);
    st << "T16_M" << setw(3) << setfill('0') << ID << ".log";

    ofs.open(st.str().c_str(), ios_base::app);
    ofs << "========================================================\n"
        << "Test16 (Master DAC Calibration) results " << now()
        << "\nBoard ID" << ID << " " << data.size()-1 << " channels\n"
        << "Linux calibration software\n"
        << "Calibration DAC Range: [" << M_DAC1 << " - " << M_DAC2 << "]\n" << endl;
    ofs << "A:" << endl;
// ofs << "Limits unknown" << endl;
    for (int i=0; i<data.size(); i++)
    {
        ofs << data[i].Adac << ((i<(data.size()-1))?",":"");
    }
    ofs << "\n" << endl;
    ofs << "B:" << endl;
// ofs << "Limits unknown" << endl;
    for (int i=0; i<data.size(); i++)
    {
        ofs << data[i].Bdac << ((i<(data.size()-1))?",":"");
    }
    ofs << "\n" << endl;
    ofs.close();

    return 0;
}


// == Calibrate Master Voltage ADC/DAC
int CalibrateMasterVoltage(int card, int addr)
{

    if (!fMasterModule)
    {
        cout << "No Master module found" << endl;
        return -1;
    }
    cout << "Calibrating Voltage ADC of "<< modtype_str[ mRDB.data.module.type]
        << " ID#" << mRDB.data.module.ID << " at " << card<< ":" << addr << endl;
//   vector<calData> calib;
    calib.clear();
    calData calchan;
    int PR_V1 = 1000;
    int PR_V2 = 3750;
	int MAX_PR_OV = 50; // max voltage to go over PR_V2 trying to reach it

    int p_addr = mMaster.data.module.master_hvcard;
    int p_card = mMaster.data.module.master_busaddr;

// = Initialise RDB parameters
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 100, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 200, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 1, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, 4000, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);

// = Configure Primary PS parameters
// sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 25, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_DOWN, 100, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, PR_V1, 1);

// Set Primary to 2000V

// cout << "Set Primary PS Vset=" << PR_V1 << endl;
    cout << "Set 1st calibration point Primary PS Vset=" << PR_V1 << endl;
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, PR_V1, 1);

    do
    {
        readModuleData(card, addr, &mMaster, false);
        readPrimaryData(p_card, p_addr, &mPrimary);

        sleep(2);
    } while ((mPrimary.data.vcur != PR_V1) || (mMaster.data.module.chan[0].vcur < 4000));

	int vbump = PR_V1;
	while ( mPrimary.data.vmon < PR_V1 && vbump < PR_V2 + MAX_PR_OV)
	{
		if ((PR_V1 - mPrimary.data.vmon) > 10) vbump += 10;
		else vbump += 1;
		cout << "bumping: vset = " << vbump;
		sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, vbump, 1);
		sleep(2);
        readPrimaryData(p_card, p_addr, &mPrimary);
		cout << " vmon = " << mPrimary.data.vmon << endl;
	}
	
	cout << "Primary reached 1st point: vmon = " << mPrimary.data.vmon << endl;

    int nchans = mMaster.data.module.num_chans;

    calchan.V1 = 0;
    calchan.V2 = 0;
    for (int i=0; i< nchans; i++)
    {
        calib.push_back(calchan);
    }
    for (int j=0; j<nSamples; j++)
    {
        usleep(10000);
        readModuleData(card, addr, &mMaster, false);
        for (int i=0; i< nchans; i++)
        {
            calib[i].V1 += mMaster.data.module.chan[i].vmon;
        }
    }

    cout << "Set 2nd calibration point Primary PS Vset=" << PR_V2 << endl;
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, PR_V2, 1);
    do
    {
        readModuleData(card, addr, &mMaster, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
        sleep(2);
    } while ((mPrimary.data.vcur != PR_V2) || (mMaster.data.module.chan[0].vcur < 4000));

	vbump = PR_V2;
	while ( mPrimary.data.vmon < PR_V2 && vbump < PR_V2 + MAX_PR_OV)
	{
		if ((PR_V2 - mPrimary.data.vmon) > 10) vbump += 10;
		else vbump += 1;
		cout << "bumping: vset = " << vbump;
		sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, vbump, 1);
		sleep(2);
        readPrimaryData(p_card, p_addr, &mPrimary);
		cout << " vmon = " << mPrimary.data.vmon << endl;
	}
	
	cout << "Primary reached 2nd point: vmon = " << mPrimary.data.vmon << endl;

    for (int j=0; j<nSamples; j++)
    {
        usleep(10000);
        readModuleData(card, addr, &mMaster, false);
        for (int i=0; i< nchans; i++)
        {
            calib[i].V2 += mMaster.data.module.chan[i].vmon;
        }
    }

    cout << "Calculate ADC calibration coefficients" << endl;

// = Calculate ADC/DAC calibration coefficients
    for (int i=0; i<nchans; i++)
    {
        float V1 = calib[i].V1/nSamples;
        float V2 = calib[i].V2/nSamples;
        float dp = V2-V1;
        if (dp==0)
        {
            cerr << "Zero difference V2-V1" << endl;
            continue;
        }
        float dpVolt = PR_V2-PR_V1;
        calib[i].Aadc = dpVolt/dp;
        calib[i].Badc = PR_V2  - calib[i].Aadc*V2;
    }
/*
  calib[i].Adac = (V_PR2-V_PR1)/(dpVolt);
  calib[i].Bdac = V_PR2-calib[i].Adac*calib[i].V2volt*Volt_coef;
  cout << "ch#" << i+1
  // << " dp:" << dp << " dpVolt:" << dpVolt
  << " Aadc: " << calib[i].Aadc << " Badc:" << calib[i].Badc
  << " Adac: " << calib[i].Adac << " Bdac:" << calib[i].Bdac << endl;
*/

// }

// ===
    cout << "Calibrating Voltage DAC of "<< modtype_str[ mRDB.data.module.type]
        << " ID#" << mRDB.data.module.ID << " at " << card<< ":" << addr << endl;

    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 4000, 1);

    cout << "Set 1st point Master DAC " << M_DAC2 << endl;
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, M_DAC2, 0, false);
    do
    {
        readModuleData(card, addr, &mMaster, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
        sleep(1);
    } while ((mMaster.data.module.chan[0].vcur != M_DAC2) || (mPrimary.data.vcur != 4000));

    for (int j=0; j<nSamples; j++)
    {
        usleep(10000);
        readModuleData(card, addr, &mMaster, false);
        for (int i=0; i< nchans; i++)
        {
            calib[i].V2volt += mMaster.data.module.chan[i].vmon;
        }
    }

    cout << "Set 2nd point Master DAC " << M_DAC1 << endl;
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, M_DAC1, 0, false);

    do
    {
        readModuleData(card, addr, &mMaster, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
        sleep(1);
    } while ((mMaster.data.module.chan[0].vcur != M_DAC1) || (mPrimary.data.vcur != 4000));
    for (int j=0; j<nSamples; j++)
    {
        usleep(10000);
        readModuleData(card, addr, &mMaster, false);
        for (int i=0; i< nchans; i++)
        {
            calib[i].V1volt += mMaster.data.module.chan[i].vmon;
        }
    }

    cout << "Calculate DAC calibration coefficients" << endl;

// = Calculate ADC/DAC calibration coefficients
    for (int i=0; i<nchans; i++)
    {
        float V1 = calib[i].V1volt/nSamples;
        float V2 = calib[i].V2volt/nSamples;
        float dp = (V2-V1)*calib[i].Aadc;
        if (dp==0)
        {
            cerr << "Zero difference V2-V1" << endl;
            continue;
        }
        float dpVolt = M_DAC2 - M_DAC1;
        float V2volt = calib[i].Aadc*V2+calib[i].Badc;

        calib[i].Adac = dpVolt/dp;
        calib[i].Bdac = M_DAC2-calib[i].Adac*V2volt;
        cout << "ch#" << i+1
// << " dp:" << dp << " dpVolt:" << dpVolt
            << " Aadc:" << calib[i].Aadc << " Badc:" << calib[i].Badc
            << " Adac:" << calib[i].Adac << " Bdac:" << calib[i].Bdac << endl;

    }

    saveMasterVoltageCalibrationResults(mMaster.data.module.ID,calib);

    sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_OFF, 0, false);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    cout << "Done." << endl;
    return 0;
}


int saveMasterCurrentCalibrationResults(int ID, curData data[][4001])
{
    ofstream ofs;

    ostringstream st;

    st << "T14_M" << setw(3) << setfill('0') << ID << ".log";

    ofs.open(st.str().c_str(), ios_base::app);
    ofs << "========================================================\n"
        << "Test14 (Master Current ADC Calibration) results " << now()
        << "\nBoard ID" << ID << " " << mRDB.data.module.num_chans << " channels\n"
        << "Linux calibration software\n\n"
        << "Current Tab:" << endl;
    ofs.setf(ios_base::fixed);
    int val;
    for (int j=0; j<4000; j++)
    {
        for (int i=0; i<mMaster.data.module.num_chans; i++)
        {
            if (data[i][j].samples>0) val = data[i][j].val/data[i][j].samples;
            else val=data[i][j].val;
            ofs << val << ((i<(mMaster.data.module.num_chans-1))?",":"");
        }
        ofs << endl;
    }
    ofs.close();
}


int smoothMasterCalibData(curData data[][4001])
{
    int nchans = mMaster.data.module.num_chans;
    int cnt=0;
    bool fFillLast=false;
    bool fFillFirst=false;
    int val2=0;
    int val1=0;
    float step=0;
    for (int i=0; i<nchans; i++)
    {
// cout << i << endl;
        for (int j=3999; j>=0; j--)
        {
            if (data[i][j].val==0 && j>0)
            {
                if (j==3999) fFillLast = true;
                cnt++;
            }
            else
            {
                if (j==0 && data[i][j].val==0)    fFillFirst = true;
                val1=data[i][j].val/data[i][j].samples;
                if (fFillLast)
                {
                    val2=val1-cnt;
                    step=1;
                    fFillLast=false;
                }
                else if (fFillFirst)
                {
                    val1=val2+cnt;
                    step=1;
                    data[i][j].val = val1;
                    data[i][j].samples = 1;
                    fFillFirst=false;
                }
                else
                {
                    if (cnt>0)
                        step=(val1-val2)/(cnt+1);

                }
// cout << cnt << " " << step <<  endl;
                while (cnt>0)
                {
                    data[i][j+cnt].val = val1-((int)rint(cnt*step));
                    data[i][j+cnt].samples = 1;
//    cout << data[i][j+cnt].val << endl;
                    cnt--;
                }
                val2=val1;
            }

        }

    }

}


int CalibrateMasterCurrentADC(int card, int addr)
{
    if (!fMasterModule)
    {
        cout << "No Master module found" << endl;
        return -1;
    }
    cout << "Calibrate Master Current ADC" << endl;

    curData curcalib[8][4001];

    int p_addr = mMaster.data.module.master_hvcard;
    int p_card = mMaster.data.module.master_busaddr;

// = Initialise Master parameters
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 150, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 300, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 1, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, 4000, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);

// = Configure Primary PS parameters
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 50, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_DOWN, 200, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 0, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);

    do
    {
        readModuleData(card, addr, &mMaster, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
        sleep(2);
    } while ((mMaster.data.module.chan[0].vcur != 4000 ) || (mPrimary.data.vcur > 0));

    struct timeval t1, t2, t3;

    calData calchan;

    for (int j=0; j< mMaster.data.module.num_chans; j++)
    {
        calib.push_back(calchan);
    }

    memset(curcalib,0, sizeof(curcalib));
    int npoints = 4000;
    for (int i=0; i<npoints; i++)
    {
//gettimeofday(&t1, NULL);
        if (i>200) i=i+9;
        int pvset=i;
        sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, pvset, 1);
// gettimeofday(&t2, NULL);

        float delta = 0.0;
        float step = 3;
        do
        {
            usleep(100000);
            readPrimaryData(p_card, p_addr, &mPrimary);
            if (mPrimary.data.vset) delta = (mPrimary.data.vset - mPrimary.data.vmon);
        } while (mPrimary.data.vcur != pvset || fabs(delta)>(step+mPrimary.data.vset/300));
/*
    do {
      usleep(100000);
      readPrimaryData(p_card, p_addr, &mPrimary);
    } while (mPrimary.data.vcur != pvset);
*/
//gettimeofday(&t2, NULL);
//cout << "Set primary V time: " << ((t2.tv_sec+t2.tv_usec/1000000.) - (t1.tv_sec+t1.tv_usec/1000000.)) << endl;

        cout << "HVset: " << i;

        for (int n=0; n<nSamples; n++)
        {
            readModuleData(card, addr, &mMaster, false);
            usleep(5000);

            for(int j=0; j< mMaster.data.module.num_chans; j++)
            {
                curcalib[j][i].val += mMaster.data.module.chan[j].imon;
                curcalib[j][i].samples++;
            }
        }
        cout << endl;
/*gettimeofday(&t3, NULL);
  cout << "Read RDB samples time:  "<< ((t3.tv_sec+t3.tv_usec/1000000.) - (t2.tv_sec+t2.tv_usec/1000000.)) << endl;
  cout << "Sum time: "<< ((t3.tv_sec+t3.tv_usec/1000000.) - (t1.tv_sec+t1.tv_usec/1000000.)) << endl;
*/

/*
  cout << i << ": ";
  cout.setf(ios_base::fixed);
  float val;
  for(int j=0; j< mRDB.data.module.num_chans; j++) {
  val = curcalib[j][i].val/curcalib[j][i].samples;///nSamples;
  cout << setprecision(1) << val << ",";
  }
  cout << endl;
*/
    }
    smoothMasterCalibData(curcalib);
    saveMasterCurrentCalibrationResults(mMaster.data.module.ID,curcalib);

    sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_OFF, 0, false);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    return 0;

}

int CalibrateMasterCurrentADC_interpol(int card, int addr)
{
    if (!fMasterModule)
    {
        cout << "No Master module found" << endl;
        return -1;
    }
    cout << "Calibrate Master Current ADC interpolation" << endl;

    int npoints = 4001; // one added so we can get the last point for interpolation 
    curData curcalib[8][4001];

    int p_addr = mMaster.data.module.master_hvcard;
    int p_card = mMaster.data.module.master_busaddr;

// = Initialise Master parameters
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 150, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 300, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 1, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, 4000, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);

// = Configure Primary PS parameters
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 50, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_DOWN, 200, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 0, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);

    do
    {
        readModuleData(card, addr, &mMaster, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
        sleep(2);
    } while ((mMaster.data.module.chan[0].vcur != 4000 ) || (mPrimary.data.vcur > 0));

    struct timeval t1, t2, t3;

    calData calchan;

    for (int j=0; j< mMaster.data.module.num_chans; j++)
    {
        calib.push_back(calchan);
    }

    memset(curcalib,0, sizeof(curcalib));
    for (int i=0; i<npoints; i+=100) // 100 V steps
    {
//gettimeofday(&t1, NULL);
        int pvset=i;
        sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, pvset, 1);
// gettimeofday(&t2, NULL);

        float delta = 0.0;
        float step = 3;
        do
        {
            usleep(100000);
            readPrimaryData(p_card, p_addr, &mPrimary);
        } while (mPrimary.data.vcur != pvset);
/*
    do {
      usleep(100000);
      readPrimaryData(p_card, p_addr, &mPrimary);
    } while (mPrimary.data.vcur != pvset);
*/
//gettimeofday(&t2, NULL);
//cout << "Set primary V time: " << ((t2.tv_sec+t2.tv_usec/1000000.) - (t1.tv_sec+t1.tv_usec/1000000.)) << endl;

        cout << "Voltage:\t" << i;

        for (int n=0; n<nSamples; n++)
        {
            readModuleData(card, addr, &mMaster, false);
            usleep(100000);

            for(int j=0; j< mMaster.data.module.num_chans; j++)
            {
                curcalib[j][i].val += mMaster.data.module.chan[j].imon;
                curcalib[j][i].samples++;
            }
        }
        cout << endl;
/*gettimeofday(&t3, NULL);
  cout << "Read RDB samples time:  "<< ((t3.tv_sec+t3.tv_usec/1000000.) - (t2.tv_sec+t2.tv_usec/1000000.)) << endl;
  cout << "Sum time: "<< ((t3.tv_sec+t3.tv_usec/1000000.) - (t1.tv_sec+t1.tv_usec/1000000.)) << endl;
*/

/*
  cout << i << ": ";
  cout.setf(ios_base::fixed);
  float val;
  for(int j=0; j< mRDB.data.module.num_chans; j++) {
  val = curcalib[j][i].val/curcalib[j][i].samples;///nSamples;
  cout << setprecision(1) << val << ",";
  }
  cout << endl;
*/
    }
	master_interpol(curcalib, 41, 0, 100);
    // 41 is the number of raw data points taken, 0 means no precisely calibrated points, 100 is step of rough calibration
	// I know, hardcoding is bad, rushing...
	cout << "Saving results" << endl;
    saveMasterCurrentCalibrationResults(mMaster.data.module.ID,curcalib);

    sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_OFF, 0, false);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    return 0;

}


int PrimaryScan(int card, int addr)
{
    int m_card = mRDB.data.module.master_hvcard;
    int m_addr = mRDB.data.module.master_busaddr;
    int m_chan = mRDB.data.module.master_chan;
    int p_addr = mMaster.data.module.master_hvcard;
    int p_card = mMaster.data.module.master_busaddr;
    int nchans = mRDB.data.module.num_chans;
    int CH = 18;

// = Initialise RDB parameters
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 150, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 300, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 10, 0, true);
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, 0, 0, false);
    sendModuleCmd(card, addr, CH, HV_SET_VSET, 4000, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);

// = Configure Master channel set Vset to 4000 DAC counts
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_UP, 100, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_DOWN, 200, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_IMAX, 1, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_VSET, 4000, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_STATE, HV_STATE_ON, 0, false);

// = Configure Primary PS parameters
// sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 50, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 100, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 0, 0);

    do
    {
        sleep(1);
        readModuleData(m_card, m_addr, &mMaster, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
    } while ((mPrimary.data.vcur != 0)||(mMaster.data.module.chan[m_chan].vcur<4000)) ;

    ofstream ofs;

    ostringstream st;

    st << "PrimaryScan.log";

    ofs.open(st.str().c_str(), ios_base::app);
    ofs << "========================================================\n"
        << "Primary Scan " << now() << endl;

    int vset=0;
    float volt = 0;
    do
    {
        sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, vset, 1);
        sleep(2);
        readPrimaryData(p_card, p_addr, &mPrimary);
        readModuleData(card, addr, &mRDB, false);
        volt = readMeter(Server)*100;
        cout << "Vset=" << vset << " Vp_mon=" << mPrimary.data.vmon
            << " Vvolt=" << volt << " Vr_mon=" << mRDB.data.module.chan[CH].vmon << endl;
        ofs <<  vset << ", " << mPrimary.data.vmon
            << ", " << volt << ", " << mRDB.data.module.chan[CH].vmon << endl;
        vset += 20;
    } while (vset<=4000);

    ofs.close();
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 0, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, 0, 0, false);

    return 0;
}


// == Read configuration for RDB module (Master and Primary)
int DetectConfig(int card, int addr)
{
	if (card == -1 && addr == -1)
		return -1;

    if (readModuleData(card, addr, &mRDB, false)!=0)
    {
        cerr << "Unable to read module data" << endl;
        return -1;
    }
    if (mRDB.data.module.type == MASTER)
    {
        fMasterModule = true;
        cout << "Master ID#"<<mRDB.data.module.ID << " at " << card << ":" <<mRDB.data.module.busaddr << endl;
    }
    else if (mRDB.data.module.type != NONE)
    {
        fMasterModule = false;
        cout << "RDB ID#"<<mRDB.data.module.ID << " at " << card << ":" <<mRDB.data.module.busaddr << endl;
    } else cout << "Can not find module at " << card << ":" <<mRDB.data.module.busaddr << endl;

    if (!fMasterModule)
    {
        int m_card = mRDB.data.module.master_hvcard;
        int m_addr = mRDB.data.module.master_busaddr;
        int m_chan = mRDB.data.module.master_chan;
        if ((m_card == NA || m_addr == NA || m_chan == NA) & !f_calib_wo_master)
        {
            cerr << "No valid Master link" << endl;
            return -1;
        }

		if (!f_calib_wo_master)
		{
			if (readModuleData(m_card, m_addr, &mMaster, false)!=0)
			{
				cerr << "Unable to read linked Master module data" << endl;
				return -1;
			}
			cout << "Master ID#"<<mMaster.data.module.ID << " at " << m_card << ":" <<mMaster.data.module.busaddr <<endl;
		}
    }
    else
    {
        readModuleData(card, addr, &mMaster, false);
    }

    int p_addr = 1; // assume single primary at 1:1 for direct connection to distributions
    int p_card = 1;

	if (!f_calib_wo_master)
	{
		p_addr = mMaster.data.module.master_hvcard;
		p_card = mMaster.data.module.master_busaddr;
	}

    if (p_card == NA || p_addr == NA)
    {
        cerr << "No valid Primary PS link" << endl;
        return -1;
    }

    if (readPrimaryData(p_card, p_addr, &mPrimary)!=0)
    {
        cerr << "Unable to read linked Primary PS data" << endl;
        return -1;
    }
    cout << "PrimaryPS at " << mPrimary.data.addr << ":" <<mPrimary.data.addr
        << " Status: "<< hvstate_str[mPrimary.data.status] << endl;

    return 0;
}

// == Read configuration for voltage calibrator
int DetectConfig_vc(int card, int addr)
{

	if (card == -1 && addr == -1)
		return -1;

    if (readModuleData(card, addr, &m_vc, false)!=0)
    {
        cerr << "Unable to read module data" << endl;
        return -1;
    }
    if (m_vc.data.module.type == MASTER)
    {
		cout << "Voltage calibrator cannot be Master. Check your hostcard number and address parameters (N and A)\n";
	    return -1;
    }
    else if (m_vc.data.module.type != NONE)
    {
        cout << "VC ID#"<<m_vc.data.module.ID << " at " << card << ":" <<m_vc.data.module.busaddr << endl;
    } 
	else 
		cout << "Can not find module at " << card << ":" <<m_vc.data.module.busaddr << endl;

	int m_card = m_vc.data.module.master_hvcard;
	int m_addr = m_vc.data.module.master_busaddr;
	int m_chan = m_vc.data.module.master_chan;
	if (m_card != NA || m_addr != NA)
	{
		cerr << "Voltage calibrator cannot have link to Master" << endl;
		cerr << "Mcard " << m_card << "  Maddr " << m_addr << "  Mchan " << m_chan << endl;
		return -1;
	}
	
    return 0;
}


int saveCheckCalibrationResults(int ID, vector<calData>& data)
{

    ofstream ofs;

    ostringstream st;
    if (data.size() <1)
    {
        cerr << "Empty results data" << endl;
        return -1;
    }

    st << "CheckCalibration_M" << setw(3) << setfill('0') << ID << ".log";

    int delim = data.size()/3;

    ofs.open(st.str().c_str(), ios_base::app);
    ofs << "========================================================\n"
        << "Calibration Check results " << now()
        << "\nBoard ID" << ID << " " << data.size() << " channels" << endl;
    ofs << "Set module Vset:" << V_CHECK << "V Delta:+/-" << V_DELTA << "V" << endl;
    ofs << "Voltmeter correction coefficient:" << Volt_coef << "\n" << endl;

    for (unsigned int i=0; i<data.size() ; i++)
    {
        ofs << "ch#" << i+1
            << "\tVmon=" << (data[i].V1)/nSamples
            << "\tVvolt=" << fixed << setprecision(1) << data[i].V1volt
            << "\tVset-Vmon=" << V_CHECK-(data[i].V1)/nSamples
            << "\tVset-Vvolt=" << setprecision(1) << V_CHECK-data[i].V1volt*Volt_coef;
        if (fabs(V_CHECK-data[i].V1volt*Volt_coef) > V_DELTA)
        {
            ofs << " (!)";
        }
        ofs << "\t\tVmon-Vvolt=" << setprecision(1) << ((data[i].V1)/nSamples  - data[i].V1volt*Volt_coef);
        if (fabs(((data[i].V1)/nSamples  - data[i].V1volt*Volt_coef)) > V_DELTA)
        {
            ofs << " (!)";
        }

        ofs << endl;
    }

    ofs.close();
    return 0;
}


int CheckCalibration(int card, int addr)
{

    cout << "Check Voltage Calibration data of "<< modtype_str[ mRDB.data.module.type]<< " ID#" << mRDB.data.module.ID
		 << " at " << card<< ":" << addr << endl;

    calib.clear();
    calData calchan;

    int m_card = mRDB.data.module.master_hvcard;
    int m_addr = mRDB.data.module.master_busaddr;
    int m_chan = mRDB.data.module.master_chan;
    int p_addr = mMaster.data.module.master_hvcard;
    int p_card = mMaster.data.module.master_busaddr;

// = Initialise RDB parameters
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 100, 0, true);
    sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 200, 0, false);
    sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 50, 0, true);
//  sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_CHECK-200, 0, true);
//  sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, true);
//  sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, true);

// = Configure Master channel set Vset to 4000 DAC counts
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_UP, 100, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_DOWN, 200, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_IMAX, 1, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_VSET, 4000, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
    sendModuleCmd(m_card, m_addr, m_chan,HV_SET_STATE, HV_STATE_ON, 0, false);

// = Configure Primary PS parameters
// sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 100, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_DOWN, 200, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, V_CHECK+200, 1);

    cout << "Set Primary PS Vset=" << V_CHECK+200 << endl;

    do
    {
        readModuleData(m_card, m_addr, &mMaster, false);
        readPrimaryData(p_card, p_addr, &mPrimary);
        sleep(2);
    } while ((mPrimary.data.vcur < V_CHECK+200) || (mMaster.data.module.chan[m_chan].vcur < 4000));

    sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_CHECK, 0, true);
    sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, true);
    sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, true);

    int nchans = mRDB.data.module.num_chans;

    cout << "Set module Vset:" << V_CHECK << "V Delta:+/-" << V_DELTA << "V" << endl;
    cout << "Voltmeter correction coefficient:" << Volt_coef << endl;
    sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_CHECK-VOLT_DIP, 0, true);
    for (int i=0; i<nchans; i++)
    {
        sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_CHECK-VOLT_DIP, 0, true);
        sendModuleCmd(card, addr, i, HV_SET_VSET, V_CHECK, 0, true);
        do
        {
            readModuleData(card, addr, &mRDB, true);
//      sleep(1);
//      readMeter(Server);
        } while (mRDB.data.module.chan[i].vcur <= V_CHECK-5);
        sleep(2);
        calchan.V1 = 0;
        for (int j=0; j<nSamples; j++)
        {
            usleep(100000);
            readModuleData(card, addr, &mRDB, true);
            calchan.V1 += mRDB.data.module.chan[i].vmon;

        }
//    sleep(2);
//    readMeter(Server);
        calchan.V1volt = readMeter(Server)*100;
        cout << "ch#" << i+1
            << "\tVmon=" << (calchan.V1)/nSamples
            << "\tVvolt=" << fixed << setprecision(1) << calchan.V1volt
            << "\tVset-Vmon=" << V_CHECK-(calchan.V1)/nSamples
            << "\tVset-Vvolt=" << setprecision(1) << V_CHECK-calchan.V1volt*Volt_coef;
        if (fabs(V_CHECK-calchan.V1volt*Volt_coef) > V_DELTA)
        {
            cout << " (!)";
        }
        cout << "\t\tVmon-Vvolt=" << setprecision(1) << ((calchan.V1)/nSamples  - calchan.V1volt*Volt_coef);
        if (fabs(((calchan.V1)/nSamples  - calchan.V1volt*Volt_coef)) > V_DELTA)
        {
            cout << " (!)";
        }
        cout << endl;

        calib.push_back(calchan);
    }
    saveCheckCalibrationResults(mRDB.data.module.ID,calib);
    cout << "Turning Off Primary PS. Please wait..." << endl;
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 0, 0);
//  sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    do
    {
        sleep(2);
        readPrimaryData(p_card, p_addr, &mPrimary);
    } while (mPrimary.data.vmon > 10.0) ;
    sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
    cout << "Done." << endl;

    return 0;
}

int CheckCalibration_vc(int card, int addr, int vc_card, int vc_addr)
{

  cout << "Check Voltage Calibration data of "<< modtype_str[ mRDB.data.module.type]<< " ID#" << mRDB.data.module.ID
       << " at " << card<< ":" << addr 
       << endl 
       << "with Voltage Calibrator " << modtype_str[ m_vc.data.module.type]<< " ID#" << m_vc.data.module.ID
       << " at " << vc_card<< ":" << vc_addr
       << endl;
  int nchans = mRDB.data.module.num_chans;
  calib.clear();
  calData calchan[nchans];

  // 30-channel jumper cable has irregular pin assignment, so have to use the maps below for calibration
  int dl_map[] = {13,14,15,1,2,3,25,26,27,7,8,9,19,20,21,16,17,18,4,5,6,28,29,30,10,11,12,22,23,24};
  int ds_map[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36};
  int *vc_map;

  if (nchans == 30) vc_map = dl_map;
  else vc_map = ds_map;

  int m_card = mRDB.data.module.master_hvcard;
  int m_addr = mRDB.data.module.master_busaddr;
  int m_chan = mRDB.data.module.master_chan;
  int p_addr = mMaster.data.module.master_hvcard;
  int p_card = mMaster.data.module.master_busaddr;

  // = Initialise RDB parameters
  sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 100, 0, true);
  sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 200, 0, false);
  sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 1, 0, false);
  //  sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_CHECK-200, 0, true);
  //  sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, true);
  //  sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, true);

  // = Configure Master channel set Vset to 4000 DAC counts
  sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_UP, 100, 0, false);
  sendModuleCmd(m_card, m_addr, m_chan,HV_SET_RAMP_DOWN, 200, 0, false);
  sendModuleCmd(m_card, m_addr, m_chan,HV_SET_IMAX, 1, 0, false);
  sendModuleCmd(m_card, m_addr, m_chan,HV_SET_VSET, 4000, 0, false);
  sendModuleCmd(m_card, m_addr, m_chan,HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
  sendModuleCmd(m_card, m_addr, m_chan,HV_SET_STATE, HV_STATE_ON, 0, false);

  // = Configure Primary PS parameters
  // sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
  sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_UP, 100, 0);
  sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_RAMP_DOWN, 200, 0);
  sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);
  sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, V_CHECK+200, 1);

  cout << "Set Primary PS Vset=" << V_CHECK+200 << endl;

  do
    {
      readModuleData(m_card, m_addr, &mMaster, false);
      readPrimaryData(p_card, p_addr, &mPrimary);
      sleep(2);
    } while ((mPrimary.data.vcur < V_CHECK+200) || (mMaster.data.module.chan[m_chan].vcur < 4000));

  sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_CHECK, 0, true);
  sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, true);
  sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, true);


  cout << "Set module Vset:" << V_CHECK << "V Delta:+/-" << V_DELTA << "V" << endl;

  sendModuleCmd(card, addr, ALL, HV_SET_VSET, V_CHECK, 0, true);

  //-----------------------------
  do // wait until Distr board reaches the voltage
    {
      readModuleData(card, addr, &mRDB, true);
      sleep(1);
    } while (mRDB.data.module.chan[0].vcur <= V_CHECK-5);
  sleep(20);

  // reset voltage ADC and Voltmeter values
  for (int i=0; i<nchans; i++)
    {
      calchan[i].V1 = 0;
      calchan[i].V1volt = 0;
    }


  cout << "RDB reached the voltage, reading Voltage calibrator..." << endl;
  // read all channels, ADC and Voltmeter values
  for (int j=0; j < nSamples; j++)
    {
      usleep(100000);
      // read board being calibrated
      readModuleData(card,    addr,    &mRDB, true);
      // read calibrator
      readModuleData(vc_card, vc_addr, &m_vc, true);
      for (int i=0; i<nchans; i++)
	{
	  calchan[i].V1     += mRDB.data.module.chan[i].vmon; // volts from calibrated 
	  calchan[i].V1volt += m_vc.data.module.chan[vc_map[i]-1].vmon; // Volts from calibrator
	}
    }

  for (int i=0; i<nchans; i++)
    {
      // average Volts to make compatible with old code below
      calchan[i].V1volt /= nSamples;

      cout << "ch#" << i+1
	   << "\tVmon=" << (calchan[i].V1)/nSamples
	   << "\tVvolt=" << fixed << setprecision(1) << calchan[i].V1volt
	   << "\tVset-Vmon=" << V_CHECK-(calchan[i].V1)/nSamples
	   << "\tVset-Vvolt=" << setprecision(1) << V_CHECK-calchan[i].V1volt;
      if (fabs(V_CHECK-calchan[i].V1volt) > V_DELTA)
        {
	  cout << " (!)";
        }
      cout << "\t\tVmon-Vvolt=" << setprecision(1) << ((calchan[i].V1)/nSamples  - calchan[i].V1volt);
      if (fabs(((calchan[i].V1)/nSamples  - calchan[i].V1volt)) > V_DELTA)
        {
	  cout << " (!)";
        }
      cout << endl;

      calib.push_back(calchan[i]);
    }

  //-----------------------------

  saveCheckCalibrationResults(mRDB.data.module.ID,calib);

  sendModuleCmd(card, addr, ALL, HV_SET_VSET, 0, 0, true);
  sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0, true);
  sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_OFF, 0, true);

  sendModuleCmd(m_card, m_addr, m_chan,HV_SET_VSET, 0, 0, false);
  sendModuleCmd(m_card, m_addr, m_chan,HV_SET_MOD_STATE, HV_STATE_OFF, 0, false);
  sendModuleCmd(m_card, m_addr, m_chan,HV_SET_STATE, HV_STATE_OFF, 0, false);

  cout << "Turning Off Primary PS. Please wait..." << endl;
  sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_VSET, 0, 0);
  do
    {
      sleep(2);
      readPrimaryData(p_card, p_addr, &mPrimary);
    } while (mPrimary.data.vmon > 10.0) ;
  sendPrimaryCmd(p_card, p_addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
  cout << "Done." << endl;

  return 0;
}


void primary_set_all(int point)
{

    // set voltage on all primaries
	list <HVPrimaryDIM*>::iterator pit;
	int voltage;

	for (pit = primary.begin(); pit != primary.end(); pit++)
	{
		HVPrimaryDIM* prim = *pit;
		int card = prim->data.port;
		int addr = prim->data.addr;
		if (point < 5) voltage = pr_set[addr][point];
		else voltage = point;
		sendPrimaryCmd(card, addr, ALL, HV_SET_VSET, voltage, 1);
	}

	// wait until primaries reach voltages 
	bool reached;
    do
    {
		for (pit = primary.begin(); pit != primary.end(); pit++)
		{
			HVPrimaryDIM* prim = *pit;
			int card = prim->data.port;
			int addr = prim->data.addr;
			readPrimaryData(card, addr, prim);
		}
        sleep(2);

		reached = true;

		for (pit = primary.begin(); pit != primary.end(); pit++)
		{
			HVPrimaryDIM* prim = *pit;
			int card = prim->data.port;
			int addr = prim->data.addr;
			if (point < 5) voltage = pr_set[addr][point];
			else voltage = point;
			if (prim->data.vcur < (voltage-2)) reached = false;
		}
				
    } while (!reached);
	

}

// storages for voltage adc values
int vadc1[MAX_DB][MAX_CH];
int vadc2[MAX_DB][MAX_CH];
// storages for adc calib coefficients
float adcA[MAX_DB][MAX_CH];
float adcB[MAX_DB][MAX_CH];

// current value of DAC for each channel
int vdac[MAX_DB][MAX_CH];
// storage for vdac values from first calibration point
int vdac1[MAX_DB][MAX_CH];
// show if board is ramping to voltage properly
bool brd_reached[MAX_DB][MAX_CH]; 

void read_distr_voltages(int vadc[MAX_DB][MAX_CH])
{

	memset(vadc, 0, sizeof (vadc));

    for (int j=0; j < nSamples; j++)
    {
		usleep(100000);

		list <HVboardDIM*>::iterator it;
		int ibrd = 0;
		for (it = distr.begin(); it != distr.end(); it++)
		{
			HVboardDIM* dboard = *it;
			int card = dboard->data.card.dataslot;
			int addr = dboard->data.module.busaddr;

			readModuleData(card, addr, dboard, false);
			for (int i = 0; i < dboard->data.module.num_chans; i++)
			{
				vadc[ibrd][i] += dboard->data.module.chan[i].vmon; // ADC value 
			}
			// stick voltmeter value into last channel + 1
			vadc[ibrd][dboard->data.module.num_chans] += dboard->data.module.hvmon;

			ibrd++;
		}
	}
	
}

void read_master_voltages(int vadc[MAX_DB][MAX_CH])
{

	memset(vadc, 0, sizeof (vadc));

    for (int j=0; j < nSamples; j++)
    {
		usleep(100000);

		list <HVboardDIM*>::iterator it;
		int ibrd = 0;
		for (it = master.begin(); it != master.end(); it++)
		{
			HVboardDIM* mboard = *it;
			int card = mboard->data.card.dataslot;
			int addr = mboard->data.module.busaddr;

			readModuleData(card, addr, mboard, false);
			for (int i = 0; i < mboard->data.module.num_chans; i++)
			{
				vadc[ibrd][i] += mboard->data.module.chan[i].vmon; // ADC value 
			}

			ibrd++;
		}
	}
	
}



void distribution_global_v_adc_calib()
{

	cout << "global distribution board ADC calibration" << endl;

	// read included and excluded chambers
	include_exclude_db();

	read_primary_calibrations();

	// find all boards and primaries, put them into lists
	find_distr_masters_primaries();

	if (distr.empty())
	{
		cout << "no distr. boards to calibrate" << endl;
		exit (0);
	}

	// open all distr boards wide
	list <HVboardDIM*>::iterator it;
	for (it = distr.begin(); it != distr.end(); it++)
	{
		HVboardDIM* dboard = *it;
		int card = dboard->data.card.dataslot;
		int addr = dboard->data.module.busaddr;

		cout << "setting up distr at: " << card << ":" << addr << endl;

		sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 100, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 200, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 10, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_VSET, 4000, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);
	
	}

	// open all master boards wide
	for (it = master.begin(); it != master.end(); it++)
	{
		HVboardDIM* mboard = *it;
		int card = mboard->data.card.dataslot;
		int addr = mboard->data.module.busaddr;

		cout << "setting up master at: " << card << ":" << addr << endl;

		sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 100, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 200, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 1, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_VSET, 4000, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);
	
	}

	// prepare primaries
	list <HVPrimaryDIM*>::iterator pit;
	for (pit = primary.begin(); pit != primary.end(); pit++)
	{
		HVPrimaryDIM* prim = *pit;
		int card = prim->data.port;
		int addr = prim->data.addr;

		sendPrimaryCmd(card, addr, ALL, HV_SET_RAMP_UP, 25, 0);
		sendPrimaryCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 100, 0);
		sendPrimaryCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);
	}

	// set first voltage 
	cout << "setting first point" <<  endl;
	primary_set_all(0);
	for (int wi = 10; wi > 0; wi--)
	{
		sleep(1);
		cout << "\rwaiting: " << wi << " s left   " << flush; 
	}
	cout << endl;
	// read ADC values
	cout << "reading ADC values" << endl;
	read_distr_voltages(vadc1);

	// set second voltage 
	cout << "setting second point" << endl;
	primary_set_all(1);
	for (int wi = 10; wi > 0; wi--)
	{
		sleep(1);
		cout << "\rwaiting: " << wi << " s left   " << flush; 
	}
	cout << endl;
	// read ADC values
	cout << "reading ADC values" << endl;
	read_distr_voltages(vadc2);

	// kill primaries
	cout << "removing voltage" <<  endl;
	for (pit = primary.begin(); pit != primary.end(); pit++)
	{
		HVPrimaryDIM* prim = *pit;
		int card = prim->data.port;
		int addr = prim->data.addr;

		sendPrimaryCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
	}

	// calculate slope and intercept
	int ibrd = 0;
	for (it = distr.begin(); it != distr.end(); it++)
	{
		HVboardDIM* dboard = *it;

		int p_addr;
		if (f_calib_wo_master)
		{
			// no masters, assume only one primary at address 1
			p_addr = 1;
		}
		else
		{
			// find which primary is driving this board
			HVboardDIM mboard;
			int m_card = dboard->data.module.master_hvcard;
			int m_addr = dboard->data.module.master_busaddr;
			// get master board object
			readModuleData(m_card, m_addr, &mboard, false);
			// get primary address
			p_addr = mboard.data.module.master_hvcard;
		}
		// these are real voltages provided by primary at calibration points
		int PR_V1 = pr_v[p_addr][0];
		int PR_V2 = pr_v[p_addr][1];

		for (int i = 0; i < dboard->data.module.num_chans + 1; i++) // +1 for voltmeter channel
		{
			float V1 = (float)vadc1[ibrd][i]/nSamples;
			float V2 = (float)vadc2[ibrd][i]/nSamples;
			float dp = V2 - V1;
			float dpVolt = PR_V2 - PR_V1;
			float Aadc = (dp == 0) ? 0 : dpVolt/dp;
			float Badc = PR_V2 - Aadc*V2;
			
			adcA[ibrd][i] = Aadc;
			adcB[ibrd][i] = Badc;

		}
		ibrd++;
	}	

	// printout in csv format
	ofstream ofs("adc_calib.csv");
	ibrd = 0;
	for (it = distr.begin(); it != distr.end(); it++)
	{
		HVboardDIM* dboard = *it;

		ofs << "board: " << dboard->data.module.ID << endl;

		for (int i = 0; i < dboard->data.module.num_chans; i++)
		{
			ofs << adcA[ibrd][i] << ",";
		}
		ofs << endl;

		for (int i = 0; i < dboard->data.module.num_chans; i++)
		{
			ofs << adcB[ibrd][i] << ",";
		}
		ofs << endl;

		ibrd++;
	}	
	ofs.close();

	ofstream ofsv("adc_calib_voltages.csv");
	ibrd = 0;
	for (it = distr.begin(); it != distr.end(); it++)
	{
		HVboardDIM* dboard = *it;

		ofsv << "board: " << dboard->data.module.ID << endl;

		for (int i = 0; i < dboard->data.module.num_chans; i++)
		{
			ofsv << (vadc1[ibrd][i]/nSamples) << ",";
		}
		ofsv << endl;

		for (int i = 0; i < dboard->data.module.num_chans; i++)
		{
			ofsv << (vadc2[ibrd][i]/nSamples) << ",";
		}
		ofsv << endl;

		ibrd++;
	}	
	ofsv.close();

	// save ADC calibrations into files
	string prepath = "/home/hvuser/data/Remote_";

	ibrd = 0;
	for (it = distr.begin(); it != distr.end(); it++)
	{
		HVboardDIM* dboard = *it;
		int delim = dboard->data.module.num_chans/3;
		ostringstream boardn;
		boardn << setw(3) << setfill('0') << dboard->data.module.ID;
		ostringstream fname; 
		fname << prepath << boardn.str() << "/" << "T10_M" << setw(3) << setfill('0') << boardn.str() << ".log";
		ofstream ofs(fname.str().c_str(), ios_base::app);

		ofs << "========================================================\n"
			<< "Test10 (ADC Calibration) results " << now()
			<< "\nBoard ID" << boardn.str() << " " 
			<< dboard->data.module.num_chans << " channels\n"
			<< "Host: " << getenv("HOST") << "\n"
			<< "Linux calibration software\n\n"
			<< "Voltmeter channel: A=" << adcA[ibrd][dboard->data.module.num_chans] 
			<<", B=" << adcB[ibrd][dboard->data.module.num_chans] << "\n" << endl;

		ofs << "A:" << endl;
		for (int i = 0; i < dboard->data.module.num_chans; i++)
		{
			ofs << adcA[ibrd][i] << ((i && ((i+1)%delim==0))?"\n":",");
		}
		ofs << "\n" << endl;
		ofs << "B:" << endl;
		for (int i = 0; i < dboard->data.module.num_chans; i++)
		{
			ofs << adcB[ibrd][i] << ((i && ((i+1)%delim==0))?"\n":",");
		}
		ofs << "\n" << endl;

		ofs.close();
		ibrd++;
	}
}

void master_global_v_adc_calib()
{
	cout << "global master board ADC calibration" << endl;

	// read included and excluded chambers
	include_exclude_db();

	read_primary_calibrations();

	// find all boards and primaries, put them into lists
	find_distr_masters_primaries();

	if (master.empty())
	{
		cout << "no master boards to calibrate" << endl;
		exit (0);
	}

	list <HVboardDIM*>::iterator it;

	// open all master boards wide
	for (it = master.begin(); it != master.end(); it++)
	{
		HVboardDIM* mboard = *it;
		int card = mboard->data.card.dataslot;
		int addr = mboard->data.module.busaddr;

		cout << "setting up master at: " << card << ":" << addr << endl;

		sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 100, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 200, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 1, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_VSET, 4000, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);
	
	}

	// prepare primaries
	list <HVPrimaryDIM*>::iterator pit;
	for (pit = primary.begin(); pit != primary.end(); pit++)
	{
		HVPrimaryDIM* prim = *pit;
		int card = prim->data.port;
		int addr = prim->data.addr;

		sendPrimaryCmd(card, addr, ALL, HV_SET_RAMP_UP, 25, 0);
		sendPrimaryCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 100, 0);
		sendPrimaryCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);
	}

	// set first voltage 
	cout << "setting first point" <<  endl;
	primary_set_all(0);
	for (int wi = 10; wi > 0; wi--)
	{
		sleep(1);
		cout << "\rwaiting: " << wi << " s left   " << flush; 
	}
	cout << endl;
	// read ADC values
	cout << "reading ADC values" << endl;
	read_master_voltages(vadc1);

	// set second voltage 
	cout << "setting second point" << endl;
	primary_set_all(1);
	for (int wi = 10; wi > 0; wi--)
	{
		sleep(1);
		cout << "\rwaiting: " << wi << " s left   " << flush; 
	}
	cout << endl;
	// read ADC values
	cout << "reading ADC values" << endl;
	read_master_voltages(vadc2);

	// kill primaries
	cout << "removing voltage" <<  endl;
	for (pit = primary.begin(); pit != primary.end(); pit++)
	{
		HVPrimaryDIM* prim = *pit;
		int card = prim->data.port;
		int addr = prim->data.addr;

		sendPrimaryCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
	}

	// calculate slope and intercept
	int ibrd = 0;
	for (it = master.begin(); it != master.end(); it++)
	{
		HVboardDIM* mboard = *it;

		int p_addr;
		if (f_calib_wo_master)
		{
			// no masters, assume only one primary at address 1
			p_addr = 1;
		}
		else
		{
			// find which primary is driving this board
			// get primary address
			p_addr = mboard->data.module.master_hvcard;
		}
		// these are real voltages provided by primary at calibration points
		int PR_V1 = pr_v[p_addr][0];
		int PR_V2 = pr_v[p_addr][1];

		for (int i = 0; i < mboard->data.module.num_chans ; i++)
		{
			float V1 = (float)vadc1[ibrd][i]/nSamples;
			float V2 = (float)vadc2[ibrd][i]/nSamples;
			float dp = V2 - V1;
			float dpVolt = PR_V2 - PR_V1;
			float Aadc = (dp == 0) ? 0 : dpVolt/dp;
			float Badc = PR_V2 - Aadc*V2;
			
			adcA[ibrd][i] = Aadc;
			adcB[ibrd][i] = Badc;

		}
		ibrd++;
	}	

	// printout in csv format
	ofstream ofs("master_adc_calib.csv");
	ibrd = 0;
	for (it = master.begin(); it != master.end(); it++)
	{
		HVboardDIM* mboard = *it;

		ofs << "board: " << mboard->data.module.ID << endl;

		for (int i = 0; i < mboard->data.module.num_chans; i++)
		{
			ofs << adcA[ibrd][i] << ",";
		}
		ofs << endl;

		for (int i = 0; i < mboard->data.module.num_chans; i++)
		{
			ofs << adcB[ibrd][i] << ",";
		}
		ofs << endl;

		ibrd++;
	}	
	ofs.close();

	ofstream ofsv("master_adc_calib_voltages.csv");
	ibrd = 0;
	for (it = master.begin(); it != master.end(); it++)
	{
		HVboardDIM* mboard = *it;

		ofsv << "board: " << mboard->data.module.ID << endl;

		for (int i = 0; i < mboard->data.module.num_chans; i++)
		{
			ofsv << (vadc1[ibrd][i]/nSamples) << ",";
		}
		ofsv << endl;

		for (int i = 0; i < mboard->data.module.num_chans; i++)
		{
			ofsv << (vadc2[ibrd][i]/nSamples) << ",";
		}
		ofsv << endl;

		ibrd++;
	}	
	ofsv.close();

	// save ADC calibrations into files
	string prepath = "/home/hvuser/data/Master_";

	ibrd = 0;
	for (it = master.begin(); it != master.end(); it++)
	{
		HVboardDIM* mboard = *it;
		int delim = 8;
		ostringstream boardn;
		boardn << setw(3) << setfill('0') << mboard->data.module.ID;
		ostringstream fname; 
		fname << prepath << boardn.str() << "/" << "T15_M" << setw(3) << setfill('0') << boardn.str() << ".log";
		ofstream ofs(fname.str().c_str(), ios_base::app);

		ofs << "========================================================\n"
			<< "Test15 (ADC Calibration) results " << now()
			<< "\nBoard ID" << boardn.str() << " " 
			<< mboard->data.module.num_chans << " channels\n"
			<< "Host: " << getenv("HOST") << "\n"
			<< "Linux calibration software\n\n";

		ofs << "A:" << endl;
		for (int i = 0; i < mboard->data.module.num_chans; i++)
		{
			ofs << adcA[ibrd][i] << ((i && ((i+1)%delim==0))?"\n":",");
		}
		ofs << "\n" << endl;
		ofs << "B:" << endl;
		for (int i = 0; i < mboard->data.module.num_chans; i++)
		{
			ofs << adcB[ibrd][i] << ((i && ((i+1)%delim==0))?"\n":",");
		}
		ofs << "\n" << endl;

		ofs.close();
		ibrd++;
	}

}


// watch all voltages to see that ramp stops when 
// V sensor shows required voltage on all channels
void distr_iterate_to_voltage(int new_voltage, int old_voltage)
{
	memset (brd_reached, 0, sizeof (brd_reached));

	// calculate approx max num of iterations
	int max_iter = (new_voltage - old_voltage) / 10 + 10;
	cout << "max number of iterations to do: " << max_iter << endl;
	list <HVboardDIM*>::iterator it;
	time_t before, after;
	for (int iter = 0; iter < max_iter; iter++)
	{
		cout << "\riteration: " << iter << flush;

		before = time(NULL);
		int ibrd = 0;
		for (it = distr.begin(); it != distr.end(); it++)
		{
			cout << "." << flush;
			HVboardDIM* dboard = *it;
			int card = dboard->data.card.dataslot;
			int addr = dboard->data.module.busaddr;
			
			// read voltages in all channels in volts
			usleep(10000);
			readModuleData(card, addr, dboard, true);

			if (vdac[ibrd][0] != -1) // the board is not removed
			{
				// determine the correction value in DAC codes for each channel
				for (int i = 0; i < dboard->data.module.num_chans; i++)
				{
					if (vdac[ibrd][i] < 4000)
					{
						int under = new_voltage - dboard->data.module.chan[i].vmon;
						int corr = under / 2;
						if (corr > 10) corr = 10;
						if (corr < -10) corr = -10;
				
						// calculate and write new DAC code
						vdac[ibrd][i] += corr;
						sendModuleCmd(card, addr, i, HV_SET_VSET, vdac[ibrd][i], 0, false);
					
						if (under <= 2 && under >= -2) brd_reached[ibrd][i] = true;
					}
				}
			}
			ibrd++;
		}
		
		bool reached = true;
		ibrd = 0;
		for (it = distr.begin(); it != distr.end(); it++)
		{
			HVboardDIM* dboard = *it;
			if (vdac[ibrd][0] != -1) // the board is not removed
			{
				for (int i = 0; i < dboard->data.module.num_chans; i++)
				{
					if (!brd_reached[ibrd][i]) reached = false;
				}
			}
			ibrd++;
		}		
		
		after = time(NULL);
		if (after - before < 2) sleep(1); // this is done to allow voltage raise if doing just a few boards

		if (reached) break;
	}

	cout << endl;
	// throw away boards that did not reach this point
	int ibrd = 0;
	for (it = distr.begin(); it != distr.end(); it++)
	{
		HVboardDIM* dboard = *it;
		bool reached = true;
		if (vdac[ibrd][0] != -1) // the board is not removed
		{
			for (int i = 0; i < dboard->data.module.num_chans; i++)
			{
				if (!brd_reached[ibrd][i]) reached = false;
			}
		}
		if (!reached) 
		{
			cout << "board " << dboard->data.module.ID 
				 << " did not reach " << new_voltage << "V, removed from calibration" << endl;
			for (int i = 0; i < dboard->data.module.num_chans; i++)
			{
				if (!brd_reached[ibrd][i]) 
					cout << "ch " << (i+1) << endl;
			}
			vdac[ibrd][0] = -1; // this marks the board as removed
		}
		ibrd++;
	}
}	

// watch all voltages to see that ramp stops when 
// V sensor shows required voltage on all channels
void master_iterate_to_voltage(int new_voltage, int old_voltage)
{
	memset (brd_reached, 0, sizeof (brd_reached));

	// calculate approx max num of iterations
	int max_iter = (new_voltage - old_voltage) / 10 + 10;
	cout << "max number of iterations to do: " << max_iter << endl;
	list <HVboardDIM*>::iterator it;
	time_t before, after;
	for (int iter = 0; iter < max_iter; iter++)
	{
		cout << "\riteration: " << iter << flush;

		before = time(NULL);
		int ibrd = 0;
		for (it = master.begin(); it != master.end(); it++)
		{
			cout << "." << flush;
			HVboardDIM* mboard = *it;
			int card = mboard->data.card.dataslot;
			int addr = mboard->data.module.busaddr;
			
			// read voltages in all channels in volts
			usleep(10000);
			readModuleData(card, addr, mboard, true);

			if (vdac[ibrd][0] != -1) // the board is not removed
			{
				// determine the correction value in DAC codes for each channel
				for (int i = 0; i < mboard->data.module.num_chans; i++)
				{
					if (vdac[ibrd][i] < 4000)
					{
						int under = new_voltage - mboard->data.module.chan[i].vmon;
						int corr = under / 2;
						if (corr > 10) corr = 10;
						if (corr < -10) corr = -10;
				
						// calculate and write new DAC code
						vdac[ibrd][i] += corr;
						sendModuleCmd(card, addr, i, HV_SET_VSET, vdac[ibrd][i], 0, false);
					
						if (under <= 2 && under >= -2) brd_reached[ibrd][i] = true;
					}
				}
			}
			ibrd++;
		}
		
		bool reached = true;
		ibrd = 0;
		for (it = master.begin(); it != master.end(); it++)
		{
			HVboardDIM* mboard = *it;
			if (vdac[ibrd][0] != -1) // the board is not removed
			{
				for (int i = 0; i < mboard->data.module.num_chans; i++)
				{
					if (!brd_reached[ibrd][i]) reached = false;
				}
			}
			ibrd++;
		}		
		
		after = time(NULL);
		if (after - before < 2) sleep(1); // this is done to allow voltage raise if doing just a few boards

		if (reached) break;
	}

	cout << endl;
	// throw away boards that did not reach this point
	int ibrd = 0;
	for (it = master.begin(); it != master.end(); it++)
	{
		HVboardDIM* mboard = *it;
		bool reached = true;
		if (vdac[ibrd][0] != -1) // the board is not removed
		{
			for (int i = 0; i < mboard->data.module.num_chans; i++)
			{
				if (!brd_reached[ibrd][i]) reached = false;
			}
		}
		if (!reached) 
		{
			cout << "board " << mboard->data.module.ID 
				 << " did not reach " << new_voltage << "V, removed from calibration" << endl;
			for (int i = 0; i < mboard->data.module.num_chans; i++)
			{
				if (!brd_reached[ibrd][i]) 
					cout << "ch " << (i+1) << endl;
			}
			vdac[ibrd][0] = -1; // this marks the board as removed
		}
		ibrd++;
	}
}	


void distribution_global_v_dac_calib()
{
    int PR_V1 = 1100;
    int PR_V2 = 3600;

	int DB_V1 = 1000;
	int DB_V2 = 3500;

	cout << "global distribution board DAC calibration" << endl;

	// read included and excluded chambers
	include_exclude_db();

	// find all boards and primaries, put them into lists
	find_distr_masters_primaries();

	if (distr.empty())
	{
		cout << "no distr. boards to calibrate" << endl;
		exit (0);
	}

	// open masters wide
	list <HVboardDIM*>::iterator it;
	for (it = master.begin(); it != master.end(); it++)
	{
		HVboardDIM* mboard = *it;
		int card = mboard->data.card.dataslot;
		int addr = mboard->data.module.busaddr;

		cout << "setting up master at: " << card << ":" << addr << endl;

		sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 100, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 200, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 1, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_VSET, 4000, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);
	
	}

	// prepare distr boards
	for (it = distr.begin(); it != distr.end(); it++)
	{
		HVboardDIM* dboard = *it;
		int card = dboard->data.card.dataslot;
		int addr = dboard->data.module.busaddr;

		cout << "setting up distr at: " << card << ":" << addr << endl;

		sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 100, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 200, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 10, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_VSET, 0, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);
	
	}

	// prepare primaries
	list <HVPrimaryDIM*>::iterator pit;
	for (pit = primary.begin(); pit != primary.end(); pit++)
	{
		HVPrimaryDIM* prim = *pit;
		int card = prim->data.port;
		int addr = prim->data.addr;

		sendPrimaryCmd(card, addr, ALL, HV_SET_RAMP_UP, 25, 0);
		sendPrimaryCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 100, 0);
		sendPrimaryCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);
	}

	// ramp primary to first point
	cout << "setting first point on primary: " << PR_V1 << endl;
	primary_set_all(PR_V1);
	
	// ramp voltages on all distribution boards
	cout << "ramping distr boards to first point" << endl;
	distr_iterate_to_voltage(DB_V1, 0);

	// copy current dac values to srorage
	memcpy (vdac1, vdac, sizeof(vdac));

	// ramp primary to second point
	cout << "setting second point on primary: " << PR_V2 << endl;
	primary_set_all(PR_V2);

	cout << "ramping distr boards to second point" << endl;
	distr_iterate_to_voltage(DB_V2, DB_V1);
	
	// kill primaries
	cout << "removing voltage" <<  endl;
	for (pit = primary.begin(); pit != primary.end(); pit++)
	{
		HVPrimaryDIM* prim = *pit;
		int card = prim->data.port;
		int addr = prim->data.addr;

		sendPrimaryCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
		cout << "setting primary at " << card << ":" << addr << " off" << endl;
		sleep(1);
	}

	// calculate slopes and intercepts
	int ibrd = 0;
	for (it = distr.begin(); it != distr.end(); it++)
	{
		HVboardDIM* dboard = *it;
		if (vdac[ibrd][0] != -1) // the board is not removed
		{
			for (int i = 0; i < dboard->data.module.num_chans; i++)
			{
				float dacd = vdac[ibrd][i] - vdac1[ibrd][i];
				float vd = DB_V2 - DB_V1;
				float Adac = dacd / vd;
				float Bdac = vdac[ibrd][i] - (DB_V2 * Adac);

				adcA[ibrd][i] = Adac;
				adcB[ibrd][i] = Bdac;
			}
		}
		ibrd++;
	}		
	
	// printout in csv format
	ofstream ofs("dac_calib.csv");
	ibrd = 0;
	for (it = distr.begin(); it != distr.end(); it++)
	{
		HVboardDIM* dboard = *it;

		if (vdac[ibrd][0] != -1) // the board is not removed
		{
			ofs << "board: " << dboard->data.module.ID << endl;

			for (int i = 0; i < dboard->data.module.num_chans; i++)
			{
				ofs << adcA[ibrd][i] << ",";
			}
			ofs << endl;

			for (int i = 0; i < dboard->data.module.num_chans; i++)
			{
				ofs << adcB[ibrd][i] << ",";
			}
			ofs << endl;
		}
		ibrd++;
	}	
	ofs.close();

	// save DAC calibrations into files
	string prepath = "/home/hvuser/data/Remote_";

	ibrd = 0;
	for (it = distr.begin(); it != distr.end(); it++)
	{
		HVboardDIM* dboard = *it;
		if (vdac[ibrd][0] != -1) // the board is not removed
		{
			int delim = dboard->data.module.num_chans/3;
			ostringstream boardn;
			boardn << setw(3) << setfill('0') << dboard->data.module.ID;
			ostringstream fname; 
			fname << prepath << boardn.str() << "/" << "T11_M" << setw(3) << setfill('0') << boardn.str() << ".log";
			ofstream ofs(fname.str().c_str(), ios_base::app);

			ofs << "========================================================\n"
				<< "Test11 (ADC Calibration) results " << now()
				<< "\nBoard ID" << boardn.str() << " " 
				<< dboard->data.module.num_chans << " channels\n"
				<< "Host: " << getenv("HOST") << "\n"
				<< "Linux calibration software\n\n"
				<< "Calibration DAC Range: [" << DB_V1 << " - " << DB_V2 << "]\n" << endl;

			ofs << "A:" << endl;
			for (int i = 0; i < dboard->data.module.num_chans; i++)
			{
				ofs << adcA[ibrd][i] << ((i && ((i+1)%delim==0))?"\n":",");
			}
			ofs << "\n" << endl;
			ofs << "B:" << endl;
			for (int i = 0; i < dboard->data.module.num_chans; i++)
			{
				ofs << adcB[ibrd][i] << ((i && ((i+1)%delim==0))?"\n":",");
			}
			ofs << "\n" << endl;

			ofs.close();
		}
		ibrd++;
	}
}

void master_global_v_dac_calib()
{
    int PR_V1 = 1100;
    int PR_V2 = 3600;

	int DB_V1 = 1000;
	int DB_V2 = 3500;

	cout << "global master board DAC calibration" << endl;

	// read included and excluded chambers
	include_exclude_db();

	// find all boards and primaries, put them into lists
	find_distr_masters_primaries();

	if (master.empty())
	{
		cout << "no master. boards to calibrate" << endl;
		exit (0);
	}

	// open masters wide
	list <HVboardDIM*>::iterator it;
	for (it = master.begin(); it != master.end(); it++)
	{
		HVboardDIM* mboard = *it;
		int card = mboard->data.card.dataslot;
		int addr = mboard->data.module.busaddr;

		cout << "setting up master at: " << card << ":" << addr << endl;

		sendModuleCmd(card, addr, ALL, HV_SET_RAMP_UP, 100, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 200, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_IMAX, 1, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_VSET, 0, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0, false);
		sendModuleCmd(card, addr, ALL, HV_SET_STATE, HV_STATE_ON, 0, false);
	}

	// prepare primaries
	list <HVPrimaryDIM*>::iterator pit;
	for (pit = primary.begin(); pit != primary.end(); pit++)
	{
		HVPrimaryDIM* prim = *pit;
		int card = prim->data.port;
		int addr = prim->data.addr;

		sendPrimaryCmd(card, addr, ALL, HV_SET_RAMP_UP, 25, 0);
		sendPrimaryCmd(card, addr, ALL, HV_SET_RAMP_DOWN, 100, 0);
		sendPrimaryCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_ON, 0);
	}

	// ramp primary to first point
	cout << "setting first point on primary: " << PR_V1 << endl;
	primary_set_all(PR_V1);
	
	// ramp voltages on all master boards
	cout << "ramping master boards to first point" << endl;
	master_iterate_to_voltage(DB_V1, 0);

	// copy current dac values to srorage
	memcpy (vdac1, vdac, sizeof(vdac));

	// ramp primary to second point
	cout << "setting second point on primary: " << PR_V2 << endl;
	primary_set_all(PR_V2);

	cout << "ramping master boards to second point" << endl;
	master_iterate_to_voltage(DB_V2, DB_V1);
	
	// kill primaries
	cout << "removing voltage" <<  endl;
	for (pit = primary.begin(); pit != primary.end(); pit++)
	{
		HVPrimaryDIM* prim = *pit;
		int card = prim->data.port;
		int addr = prim->data.addr;

		sendPrimaryCmd(card, addr, ALL, HV_SET_MOD_STATE, HV_STATE_OFF, 0);
		cout << "setting primary at " << card << ":" << addr << " off" << endl;
		sleep(1);
	}

	// calculate slopes and intercepts
	int ibrd = 0;
	for (it = master.begin(); it != master.end(); it++)
	{
		HVboardDIM* mboard = *it;
		if (vdac[ibrd][0] != -1) // the board is not removed
		{
			for (int i = 0; i < mboard->data.module.num_chans; i++)
			{
				float dacd = vdac[ibrd][i] - vdac1[ibrd][i];
				float vd = DB_V2 - DB_V1;
				float Adac = dacd / vd;
				float Bdac = vdac[ibrd][i] - (DB_V2 * Adac);

				adcA[ibrd][i] = Adac;
				adcB[ibrd][i] = Bdac;
			}
		}
		ibrd++;
	}		
	
	// printout in csv format
	ofstream ofs("master_dac_calib.csv");
	ibrd = 0;
	for (it = master.begin(); it != master.end(); it++)
	{
		HVboardDIM* mboard = *it;

		if (vdac[ibrd][0] != -1) // the board is not removed
		{
			ofs << "board: " << mboard->data.module.ID << endl;

			for (int i = 0; i < mboard->data.module.num_chans; i++)
			{
				ofs << adcA[ibrd][i] << ",";
			}
			ofs << endl;

			for (int i = 0; i < mboard->data.module.num_chans; i++)
			{
				ofs << adcB[ibrd][i] << ",";
			}
			ofs << endl;
		}
		ibrd++;
	}	
	ofs.close();

	// save DAC calibrations into files
	string prepath = "/home/hvuser/data/Master_";

	ibrd = 0;
	for (it = master.begin(); it != master.end(); it++)
	{
		HVboardDIM* mboard = *it;
		if (vdac[ibrd][0] != -1) // the board is not removed
		{
			int delim = 8;
			ostringstream boardn;
			boardn << setw(3) << setfill('0') << mboard->data.module.ID;
			ostringstream fname; 
			fname << prepath << boardn.str() << "/" << "T16_M" << setw(3) << setfill('0') << boardn.str() << ".log";
			ofstream ofs(fname.str().c_str(), ios_base::app);

			ofs << "========================================================\n"
				<< "Test16 (ADC Calibration) results " << now()
				<< "\nBoard ID" << boardn.str() << " " 
				<< mboard->data.module.num_chans << " channels\n"
				<< "Host: " << getenv("HOST") << "\n"
				<< "Linux calibration software\n\n";

			ofs << "A:" << endl;
			for (int i = 0; i < mboard->data.module.num_chans; i++)
			{
				ofs << adcA[ibrd][i] << ((i && ((i+1)%delim==0))?"\n":",");
			}
			ofs << "\n" << endl;
			ofs << "B:" << endl;
			for (int i = 0; i < mboard->data.module.num_chans; i++)
			{
				ofs << adcB[ibrd][i] << ((i && ((i+1)%delim==0))?"\n":",");
			}
			ofs << "\n" << endl;

			ofs.close();
		}
		ibrd++;
	}

}

float a[36], b[36], va, vb;

void check_file(string keyword, string fname, int id, float mina, float maxa, float minb, float maxb)
{
		// open file
		ifstream fnames(fname.c_str());
		if (fnames.is_open())
		{
			char cbuf[1000];
			string buf;
			typedef enum {idle, readA, readB} parse_sm_states;
			parse_sm_states ps = idle;
			int lna, lnb; // line number
			int nf;
			float l[12];
			va = vb = 0;
			while (!fnames.eof())
			{
				fnames.getline(cbuf, 1000);
				buf = cbuf;
				switch(ps)
				{
				case idle:
					if (buf.find("A:") != string::npos) ps = readA;
					if (buf.find("B:") != string::npos) ps = readB;
					if (buf.find("Voltmeter channel") != string::npos) 
					{   
                        // read voltmeter params here because they're on the same line
						nf = sscanf(buf.c_str(), "Voltmeter channel: A=%f, B=%f", l, l+1);
						va = l[0];
						vb = l[1];
					}
					lna = lnb = 0;
					break;

				case readA:
					// try to split the line into 12 numbers
					nf = sscanf(buf.c_str(), "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
						   l, l+1,l+2,l+3,l+4,l+5,l+6,l+7,l+8,l+9,l+10,l+11);
					// copy the numbers into parameters array
					for (int i = 0; i < nf; i++)
					{ 
						a[lna] = l[i];
						lna++;
					} 
					if (lna > 28 || nf == 8) ps = idle; // finished reading A
					break;

				case readB:
					// try to split the line into 12 numbers
					nf = sscanf(buf.c_str(), "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
						   l, l+1,l+2,l+3,l+4,l+5,l+6,l+7,l+8,l+9,l+10,l+11);
					// copy the numbers into parameters array
					for (int i = 0; i < nf; i++)
					{ 
						b[lnb] = l[i];
						lnb++;
					} 
					if (lnb > 28 || nf == 8) ps = idle; // finished reading B
					break;
				}
				
			}

			int chnum;
			if (id < 0) 
			{
				id = -id;
				chnum = 8; // master
			}
			else
			{
				chnum = (id < 256) ? 36 : 30; // distribution
			}
			for (int i = 0; i < chnum; i++)
			{
				if (a[i] < mina || a[i] > maxa)
					cout << "board " << id << " ch " << (i+1) << " " << keyword << " A out of range: " << a[i] << endl;
			}	
			for (int i = 0; i < chnum; i++)
			{
				if (b[i] < minb || b[i] > maxb)
					cout << "board " << id << " ch " << (i+1) << " " << keyword << " B out of range: " << b[i] << endl;
			}	
			if (keyword.find("ADC") != string::npos && chnum > 8)
			{
				if (va < mina || va > maxa)
					cout << "board " << id << " voltmeter " << keyword << " A out of range: " << va << endl;
				if (vb < minb || vb > maxb)
					cout << "board " << id << " voltmeter " << keyword << " B out of range: " << vb << endl;
			}
		}
		else
		{
			cout << "cannot find file: " << fname << endl; 
		}				
}

void check_v_calib_const()
{

	// find all boards and primaries, put them into lists
	find_distr_masters_primaries();

	list <HVboardDIM*>::iterator it;
	
	for (it = distr.begin(); it != distr.end(); it++)
	{
		HVboardDIM* dboard = *it;

		// get module id
		int id = dboard->data.module.ID;
		
		// construct file names
		ostringstream adcf, dacf;
		adcf << "/home/hvuser/data/Remote_" 
			 << setfill('0') << setw(3) << id << "/T10_M" 
			 << setfill('0') << setw(3) << id << ".log"; 

		dacf << "/home/hvuser/data/Remote_" 
			 << setfill('0') << setw(3) << id << "/T11_M" 
			 << setfill('0') << setw(3) << id << ".log"; 

		check_file("dADC", adcf.str(), id, 0.9, 1.09, -5, 30);

		check_file("dDAC", dacf.str(), id, 0.73, 0.9, 50, 90);
	}

	for (it = master.begin(); it != master.end(); it++)
	{
		HVboardDIM* mboard = *it;

		// get module id
		int id = mboard->data.module.ID;
		
		// construct file names
		ostringstream adcf, dacf;
		adcf << "/home/hvuser/data/Master_" 
			 << setfill('0') << setw(3) << id << "/T15_M" 
			 << setfill('0') << setw(3) << id << ".log"; 

		dacf << "/home/hvuser/data/Master_" 
			 << setfill('0') << setw(3) << id << "/T16_M" 
			 << setfill('0') << setw(3) << id << ".log"; 

		check_file("mADC", adcf.str(), -id, 1.36, 1.47, -17, 30); // master ID is sent negative

		check_file("mDAC", dacf.str(), -id, 0.54, 0.6, 25, 70);
	}
		
}

#define ADC_CHECK_TOL 5 // voltage measurement tolerance (+-) in volts
//#define ADC_CHECK_PRIM 3600

float volts[MAX_DB][MAX_CH];

unsigned long long check_db_ch[MAX_DB];

void distribution_check_v_ADC_measurements()
{

	memset (check_db_ch, 0, sizeof (check_db_ch));
	// read list of channels to monitor
/*	ifstream check_db_channels_f("/home/hvuser/data/check_db_channels.txt");

	if (check_db_channels_f.is_open())
	{
		int dbn;
		while (!check_db_channels_f.eof())
		{
			check_db_channels_f >> dbn;
			exclude_db.insert(dbn);
		}

		check_db_channels_f.close();
	}
*/

	memset (volts, 0, sizeof(volts));

	// all boards already found , don't fill lists again
	
	list <HVboardDIM*>::iterator it;

		// read voltages in all channels in volts
	for (int ri = 0; ri < 20; ri++)
	{
		int ibrd = 0;
		for (it = distr.begin(); it != distr.end(); it++)
		{
			HVboardDIM* dboard = *it;
			int card = dboard->data.card.dataslot;
			int addr = dboard->data.module.busaddr;
			int id = dboard->data.module.ID;
			
			readModuleData(card, addr, dboard, true);
			for (int i = 0; i < dboard->data.module.num_chans; i++)
			{
				// read channel voltage
				volts[ibrd][i] += dboard->data.module.chan[i].vmon;
			}
			volts[ibrd][dboard->data.module.num_chans] += dboard->data.module.hvmon;
			ibrd++;
		}	
	
		usleep(20000);
	}

	int ibrd = 0;
	for (it = distr.begin(); it != distr.end(); it++)
	{
		HVboardDIM* dboard = *it;
		int card = dboard->data.card.dataslot;
		int addr = dboard->data.module.busaddr;
		int id = dboard->data.module.ID;
		int p_addr;
		if (f_calib_wo_master)
		{
			// no masters, assume only one primary at address 1
			p_addr = 1;
		}
		else
		{
			// find which primary is driving this board
			HVboardDIM mboard;
			int m_card = dboard->data.module.master_hvcard;
			int m_addr = dboard->data.module.master_busaddr;
			// get master board object
			readModuleData(m_card, m_addr, &mboard, false);
			// get primary address
			p_addr = mboard.data.module.master_hvcard;
		}
		// these are real voltages provided by primary at calibration points
		int PR_V1 = pr_v[p_addr][0];
		int PR_V2 = pr_v[p_addr][1]; // ADC_CHECK_PRIM

		for (int i = 0; i < dboard->data.module.num_chans + 1; i++) // +1 for voltmeter
		{
			// read channel voltage
			int va = (int) (volts[ibrd][i] / 20);
			// compare to limits
			ostringstream chname;

			if (i == dboard->data.module.num_chans) chname << "\tvmeter";
			else chname << "\tch " << (i+1);

			if (va < (PR_V2 - ADC_CHECK_TOL))
			{
				cout << "DB " << id << chname.str() << "\tlow  ADC " << (va - PR_V2) << endl;
			}
			if (va > (PR_V2 + ADC_CHECK_TOL))
			{
				cout << "DB " << id << chname.str() << "\thigh ADC " << (va - PR_V2) << endl;
			}
		
		}
		ibrd++;
	}
}

void master_check_v_ADC_measurements()
{

	memset (check_db_ch, 0, sizeof (check_db_ch));

	memset (volts, 0, sizeof(volts));

	// all boards already found , don't fill lists again
	
	list <HVboardDIM*>::iterator it;

		// read voltages in all channels in volts
	for (int ri = 0; ri < 20; ri++)
	{
		int ibrd = 0;
		for (it = master.begin(); it != master.end(); it++)
		{
			HVboardDIM* mboard = *it;
			int card = mboard->data.card.dataslot;
			int addr = mboard->data.module.busaddr;
			int id = mboard->data.module.ID;
			
			readModuleData(card, addr, mboard, true);
			for (int i = 0; i < mboard->data.module.num_chans; i++)
			{
				// read channel voltage
				volts[ibrd][i] += mboard->data.module.chan[i].vmon;
			}
			ibrd++;
		}	
	
		usleep(20000);
	}

	int ibrd = 0;
	for (it = master.begin(); it != master.end(); it++)
	{
		HVboardDIM* mboard = *it;
		int card = mboard->data.card.dataslot;
		int addr = mboard->data.module.busaddr;
		int id = mboard->data.module.ID;
		int p_addr;
		if (f_calib_wo_master)
		{
			// calibration setup, assume only one primary at address 1
			p_addr = 1;
		}
		else
		{
			// get primary address
			p_addr = mboard->data.module.master_hvcard;
		}
		// these are real voltages provided by primary at calibration points
		int PR_V1 = pr_v[p_addr][0];
		int PR_V2 = pr_v[p_addr][1]; // ADC_CHECK_PRIM

		for (int i = 0; i < mboard->data.module.num_chans; i++) 
		{
			// read channel voltage
			int va = (int) (volts[ibrd][i] / 20);
			// compare to limits
			ostringstream chname;
			chname << "\tch " << (i+1);

			if (va < (PR_V2 - ADC_CHECK_TOL))
			{
				cout << "DB " << id << chname.str() << "\tlow  ADC " << (va - PR_V2) << endl;
			}
			if (va > (PR_V2 + ADC_CHECK_TOL))
			{
				cout << "DB " << id << chname.str() << "\thigh ADC " << (va - PR_V2) << endl;
			}
		
		}
		ibrd++;
	}
}

void dump_adc_volts()
{

	memset (check_db_ch, 0, sizeof (check_db_ch));
	// read list of channels to monitor

	ifstream check_db_channels_f("/home/hvuser/data/check_db_channels.txt");

	if (check_db_channels_f.is_open())
	{
		string s;
		int c[36], bn;
		unsigned long long chm;
		while (!check_db_channels_f.eof())
		{
			for (int cn = 0; cn < 36; cn++) c[cn] = -1;
			bn = 0;
			getline(check_db_channels_f, s);
			sscanf 
			(
				s.c_str(), 
				"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", 
				&bn, 
				&c[0], &c[1], &c[2], &c[3], &c[4], &c[5], &c[6], &c[7], &c[8], &c[9], 
				&c[10], &c[11], &c[12], &c[13], &c[14], &c[15], &c[16], &c[17], &c[18], &c[19], 
				&c[20], &c[21], &c[22], &c[23], &c[24], &c[25], &c[26], &c[27], &c[28], &c[29], 
				&c[30], &c[31], &c[32], &c[33], &c[34], &c[35]
				
			);

			// sanity
			if (bn < 0 || bn > MAX_DB)
			{
				cout << "bad line in check_db_channels: " << s << endl;
				continue;
			}

			// build channel mask
			chm = 0;
			for (int cn = 0; cn < 36; cn++) 
			{
				// sanity
				if (c[cn] < -1 || c[cn] > 36)
					cout << "bad line in check_db_channels: " << s << endl;
				else
					if (c[cn] > 0) chm |= (1LL << (c[cn]-1));
			}
			// store mask
			check_db_ch[bn] = chm;
		}

		check_db_channels_f.close();
	}
	else
	{
		// channel list not found - enable all
		for (int i = 0; i < MAX_DB; i++) check_db_ch[i] = 0xfffffffffLL;
	}


	memset (volts, 0, sizeof(volts));
	ofstream adc_volts_f("/home/hvuser/data/adc_volts.csv");
	if (adc_volts_f.is_open())
	{
		// find all boards and primaries, put them into lists
		find_distr_masters_primaries();
	
		list <HVboardDIM*>::iterator it;

		// read voltages in all channels in volts
		int ibrd = 0;
		for (it = distr.begin(); it != distr.end(); it++)
		{
			HVboardDIM* dboard = *it;
			int card = dboard->data.card.dataslot;
			int addr = dboard->data.module.busaddr;
			int id = dboard->data.module.ID;
		
			readModuleData(card, addr, dboard, true);
			for (int i = 0; i < dboard->data.module.num_chans; i++)
			{
				// read channel voltage
				volts[ibrd][i] = dboard->data.module.chan[i].vmon;
//				adc_volts_f << dec << volts[ibrd][i] << endl;
			}
			ibrd++;
		}	
		read_primary_calibrations();

		ibrd = 0;
		for (it = distr.begin(); it != distr.end(); it++)
		{
			HVboardDIM* dboard = *it;
			int card = dboard->data.card.dataslot;
			int addr = dboard->data.module.busaddr;
			int id = dboard->data.module.ID;
			unsigned long long chm = check_db_ch[id];

			// find which primary is driving this board
			HVboardDIM mboard;
			int m_card = dboard->data.module.master_hvcard;
			int m_addr = dboard->data.module.master_busaddr;
			// get master board object
			readModuleData(m_card, m_addr, &mboard, false);
			// get primary address
			int p_addr = mboard.data.module.master_hvcard;

			// these are real voltages provided by primary at calibration points
			int PR_V1 = pr_v[p_addr][0];
			int PR_V2 = pr_v[p_addr][1];

			for (int i = 0; i < dboard->data.module.num_chans; i++)
			{
				if ((chm & (1LL << i)) != 0LL)
				{
					// calculate the difference
					int va = (int)volts[ibrd][i] - PR_V2;
					// print the difference
					adc_volts_f << dec << id << "\t" << (i+1)  << "\t" << va << endl;
				}
		
			}
			ibrd++;
		}
		adc_volts_f.close();
	}
	else 
	{
		cout << "cannot open /home/hvuser/data/adc_volts.csv for writing" << endl;
	}
}


int main(int argc, char **argv)
{
	/*
	  int Host = 600;
	  int Card = 3;
	  int Addr = 14;
	  bool fHostID = false;
	  string pattern = HV_DIM_NAME;
	*/
	stringstream sname;
	if (argc >= 2)
    {
		for(int i=1; i< argc; i++)
        {
			if(argv[i][0] == '-')
            {
				switch(argv[i][1])
                {
				case '?' :
					usage();
					return 0;
				case 'h' :
					Host =  atoi(argv[i]+2);
					Host = (Host>0)?Host:0;
					sname <<  pattern << "_HID" << Host;
					sname >> pattern;
					fHostID = true;
					break;
				case 'a' :
					Addr =  atoi(argv[i]+2);
					Addr = (Addr>0) ? (Addr<HVCARD_MAX_MODULES?Addr:15) :0;
					break;
				case 'n' :
					Card = atoi(argv[i]+2);
					Card = (Card>0)? (Card<=15?Card:15):0;
					break;
				case 'c' :
					Volt_coef=atof(argv[i]+2);
					break;
				case 's' :
					nSamples=atoi(argv[i]+2);
					break;
				case 'm':
					Server = argv[i]+2;
					break;
				case 'C':
					fCurrentCalibration = true;
					break;
				case 'f':
					fLoadFromFile = true;
					break;
				case 'T':
					fCalibTest = true;
					break;
				case 'i': // for calibration with interpolation, set all params here so user has to do only -i
					f_interpol = true;
					fCurrentCalibration = true;
					fCurrentCalibOldMethod = true;
					fCalib100M = true;
					nSamples = 10;
					fFixedSlope = true;
					break;
				case 'I': // for calibration with interpolation, set all params here so user has to do only -i
					f_interpol_mult = true;
					fCurrentCalibration = true;
					fCurrentCalibOldMethod = true;
					fCalib100M = true;
					nSamples = 10;
					fFixedSlope = true;
					break;
				case 'v':
					V_CHECK=atoi(argv[i]+2);
					V_CHECK = (V_CHECK<4000)?V_CHECK:4000;
					break;
				case 'd':
					V_DELTA=atoi(argv[i]+2);
					V_DELTA = (V_CHECK<50)?V_CHECK:50;
					break;
				case 'o':
					fCurrentCalibOldMethod = true;
					break;
				case 'M':
					fCalib100M = true;
					if (atof(argv[i]+2)>0) AvgSlope = atof(argv[i]+2);
					break;
				case 'F':
					fFixedSlope = true;
					break;
				case 'A' :
					vc_addr =  atoi(argv[i]+2);
					vc_addr = (vc_addr>0) ? (vc_addr<HVCARD_MAX_MODULES?vc_addr:15) :0;
					break;
				case 'N' :
					vc_card = atoi(argv[i]+2);
					vc_card = (vc_card>0)? (vc_card<=15?vc_card:15):0;
					break;

				case 'u':
					// v adc calibration of all distr boards using primaries
					f_distribution_global_adc = true;
					break;

				case 'U':
					// v dac calibration of all distr boards using primaries
					f_distribution_global_dac = true;
					break;

				case 'b':
					// v adc calibration of all distr boards using primaries
					f_master_global_adc = true;
					break;

				case 'B':
					// v dac calibration of all distr boards using primaries
					f_master_global_dac = true;
					break;

				case 'V':
					// check v calibration constants in all files
					f_check_v_calib_const = true;
					break;

				case 'k':
					// check voltage ADC measurements in pass-thru mode
					f_check_v_ADC_measurements = true;
					break;

				case 'X':
					// dump ADC voltage measurements
					f_dump_adc_volts = true;
					break;

				case 'l':
					// calibrate boards without master links (direct connection to primary)
					f_calib_wo_master = true;
					break;

				default :
					cerr << "Unknown option " << argv[i] << endl;

                }
            }

        }
    }
	else
    {
		usage();
		//  return 0;
    }

	DimClient::addErrorHandler(new dimHVErrHandler());

	if (!getServerName(pattern,servername))
    {
		cerr << "Unable to find HV server" << endl;
		return -1;
    }

	string service = servername+"/HV_PRIMARY_CMD";
	rpcPrCmd = new DimRpcInfo(service.c_str(), -1);
	service = servername+"/HV_PRIMARY_DATA";
	rpcPrData = new DimRpcInfo(service.c_str(), -1);
	service = servername + "/HV_REAL_CMD";
	rpcModCmdReal = new DimRpcInfo(service.c_str(), -1);
	service = servername + "/HV_RAW_CMD";
	rpcModCmd = new DimRpcInfo(service.c_str(), -1);
	service = servername + "/HV_REAL_DATA";
	rpcModDataReal = new DimRpcInfo(service.c_str(), -1);
	service = servername + "/HV_RAW_DATA";
	rpcModData = new DimRpcInfo(service.c_str(), -1);

	if (f_distribution_global_adc)
	{
		distribution_global_v_adc_calib();
	}
	else if (f_distribution_global_dac)
	{
		distribution_global_v_dac_calib();
	}
	else if (f_master_global_adc)
	{
		master_global_v_adc_calib();
	}
	else if (f_master_global_dac)
	{
		master_global_v_dac_calib();
	}
	else if (f_check_v_calib_const)
	{
		check_v_calib_const();
	}
	else if (f_check_v_ADC_measurements)
	{
		// find all boards and primaries, put them into lists
		find_distr_masters_primaries();
		read_primary_calibrations();
		distribution_check_v_ADC_measurements();
		master_check_v_ADC_measurements();
	}
	else if (f_dump_adc_volts)
	{
		dump_adc_volts();
	}
	else if (f_interpol_mult)
	{
		CalibrateCurrentADC_interpol_mult();
	}
	else if (DetectConfig(Card, Addr) ==0)
    {
		if (fCalibTest)
        {
			if (!fMasterModule)			
			{
				if (vc_card >= 0 && vc_addr >= 0) 
				{
					if (DetectConfig_vc(vc_card, vc_addr) == -1) return -1;
					CheckCalibration_vc(Card, Addr, vc_card, vc_addr); // check with voltage calibrator board
				}
				else 
					CheckCalibration(Card, Addr);
			}
        }
		else if (fCurrentCalibration)
        {
			//      PrimaryScan(Card, Addr);
			//      return 0;
			if (fMasterModule)
            {
				if (f_interpol)
					CalibrateMasterCurrentADC_interpol(Card, Addr);
				else
					CalibrateMasterCurrentADC(Card, Addr);
            }
			else
            {
				if (fLoadFromFile )
                {
					loadCurrentCalibrationResults(mRDB.data.module.ID);
                }
				else
                {
					if (!fCurrentCalibOldMethod)
                    {
						CalibrateLowRangeADC(Card, Addr);
						cout << "Disconnect Multimeter and connect Current calibration resistive loads. Press any key to continue.";
						char ch;
						cin.get(ch);
						cin.clear();

                    }
					if (f_interpol)
					{
						CalibrateCurrentADC_interpol(Card, Addr);
					}
					else
					{
						CalibrateCurrentADC(Card, Addr);
					}
                }
            }
        }
		else
        {
			usage();
/*			if (fMasterModule)
				CalibrateMasterVoltage(Card, Addr);
			else
			{  
				if (vc_card >= 0 && vc_addr >= 0) 
				{
					if (DetectConfig_vc(vc_card, vc_addr) == -1) return -1;
					CalibrateRDBVoltage_vc(Card, Addr, vc_card, vc_addr); // calibrate with voltage calibrator board
				}
				else 
					CalibrateRDBVoltage(Card, Addr); // calibrate with voltmeter and diode switch

			}
*/
        }
    }
	else
    {
		cerr << "Unable to detect correct RDB configuration" << endl;
		return -1;
    }

	return 0;
}
