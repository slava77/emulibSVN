#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "version.h"
#include <ncurses.h>
#include <time.h>


#include "Wiener_PL512.h"
#include "hvwiener.h"

dimWienerPS wiener;

extern Wiener_PL512::Dictionary Wiener_PL512::dict;
extern char Wiener_PL512::OutputString[];


using namespace std;

int updateWienerData(Wiener_PL512* PS);
void showWienerData();

std::string now()
{
    char buf[255];
    time_t now=time(NULL);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    std::string time = std::string(buf);
    return time.substr(0,time.find("\n",0));

};



void usage()
{
    printf("Usage: lvClient -[options]\n");
    printf("Options:\n");
    printf("\t-?\tshow this usage info\n");
    printf("\t-wADDR\tset Wiener Net Address\n");
    printf("\t-cN\tset channel (N > 12 set ALL channels mode)\n");
    printf("\t-gN\tset group (N=0 ALL channels)\n");
    printf("\t-oN\tset main switch state (0-OFF, 1-ON)\n");
    printf("\t-sN\tset group state or channel state (with -c) (0-OFF, 1-ON)\n");
    printf("\t-TN\tcheck connection between client and Wiener (N - delay in secs)\n");
    printf("\t-p\tshow Wiener PL monitor\n");

    /*
      printf("\t-x\tturn all modules on all hostcards Off\n");
      printf("\t-vN\tset voltage in DAC counts (0..4000)\n");
      printf("\t-tN\tset maximum trip current level in counts (0..3000)\n");
      printf("\t-yN\tset maximum voltage level in counts (0..4000)\n");
      printf("\t-wN\tsaving of data from module in file mode (0-OFF, >0-ON\n");
      printf("\t-p\tdisplay monitored values from modules\n");
    */
};

// Ctrl-C Handler
static void catch_sigint(int sig)
{
    std::cerr << "lvClient: received signal "<< sig <<  " [Ctrl-C] from user, exiting." << std::endl;
    endwin();
    exit(0);
}

void signal_handler(int sig)
{
    std::cerr << "lvClient: received signal " << sig << ", exiting." << std:: endl;
    endwin();
    exit(0);
}


int main(int argc, char **argv)
{
    struct sigaction sa_old;
    struct sigaction sa_new;

    int pause = 3;

    bool fChan=false, fGroup=false, fMainSwitch=false, fState=false, fAddr=false, fMonitor=false;
    bool fCheckConnectivity = false;
    std::string ps_addr="";
    std::vector<int> chans;
    int ch=0;
    int group=0;
    int num_chans = 12;
    int mainswitch = 0;
    int state = 0;

    int updatetime = 3; // 3secs
    int delay = updatetime;

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
                case 'w' :
                    ps_addr = argv[i]+2;
                    if (ps_addr.length()>0)
                    {
                        fAddr = true;
                    }
                    break;
                case 'c' :
                    ch = atoi(argv[i]+2);
                    //          ch = (ch>0) ? (ch<=12 ? ch :ALL) :0;
                    ch = (ch>0) ? ch : 0;
                    chans.push_back(ch);
                    fChan = true;
                    break;
                case 'g' :
                    group = atoi(argv[i]+2);
                    group = (group>0) ? group: 0;
                    fGroup = true;
                    break;
                case 'o' :
                    mainswitch = atoi(argv[i]+2);
                    mainswitch = (mainswitch>0) ? 1 : 0;
                    fMainSwitch = true;
                    break;
                case 's' :
                    state = atoi(argv[i]+2);
                    state = (state>0) ? 1 : 0;
                    fState = true;
                    break;
                case 'p' :
                    fMonitor = true;
                    break;
                case 'T' :
                    fCheckConnectivity = true;
                    delay = atoi(argv[i]+2);
                    if (delay >=1 && delay <60) updatetime = delay;
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

    if (!fAddr)
    {
        cerr << "ERROR: Invalid or empty Wiener PL network address" <<  endl;
        return 0;
    }
    // set up the signal handler
    signal(SIGTERM,signal_handler);

    // set up signal handling
    sa_new.sa_handler = catch_sigint;
    sigemptyset(&sa_new.sa_mask);
    sa_new.sa_flags = 0;
    sigaction(SIGINT, &sa_new, &sa_old );

    cout << "Initializing Wiener PL512 at " << ps_addr << endl;
    Wiener_PL512* PS = new Wiener_PL512(ps_addr.c_str());
    if (!updateWienerData(PS))
    {
        cerr << "ERROR: Unable to read data from Wiener PL" << endl;
//    return 1;
    }

    cout << "Timeout: " << PS->GetTimeout() << " Retries: " << PS->GetRetries() << endl;

    if (fMainSwitch)
    {
        cout << "Set Main Switch to " << mainswitch << endl;
        PS->SetMainSwitch(mainswitch);
    }
    if (fState)
    {
        if (state)
        {
            PS->SetMainSwitch(1);
        }
        // Set output state for group
        if (fGroup)
        {
            cout << "Set Group " << group << " output state to " << state << endl;
            PS->SetGroupSwitch(group, state);
            if (state > 0) {
                sleep(pause);
                updateWienerData(PS);
                int num_chans = PS->GetNumberOfChannels();
                int n_fails = 0;
                for (unsigned ch=0; ch < num_chans; ch++)
                {
                    if ((wiener.chans[ch].out_switch != state) &&
                            (wiener.chans[ch].group == group)) {
                        cerr << "ERROR: Failed to turn ON channel "  << wiener.chans[ch].name << " for group " << group << endl;
                        n_fails++;
                    }
                }
                if (n_fails>0) {
                    cerr << "ERROR: Turning OFF all channels for group " << group <<  endl;
                    PS->SetGroupSwitch(group, 0);
                }
            }
        }
        // Set output state for channels
        if (fChan)
        {
            for (unsigned int i=0; i< chans.size(); i++)
            {
                if (chans[i] <= num_chans)
                {
                    cout << "Set Channel " << chans[i]<< " output state to " << state << endl;
                    PS->SetChannelOutputSwitch(chans[i], state);
                }
            }
            if (state > 0) {
                sleep(pause);
                updateWienerData(PS);
                for (unsigned int i=0; i< chans.size(); i++)
                {
                    if (chans[i] <= num_chans)
                    {
                        if (wiener.chans[chans[i]].out_switch != state) {
                            cerr << "ERROR: Failed to turn ON channel "  << wiener.chans[chans[i]].name << endl;
                        }
                    }
                }

            }
        }
        // Set output state for all channels if group and channels are not specified
        if (!fGroup && !fChan)
        {
            cout << "Set all channels state to " << state << endl;
            PS->SetGroupSwitch(0, state);
            if (state > 0) {
                sleep(pause);
                updateWienerData(PS);
                int num_chans = PS->GetNumberOfChannels();
                int n_fails = 0;
                for (unsigned ch=0; ch < num_chans; ch++)
                {
                    if (wiener.chans[ch].out_switch != state) {
                        cerr << "ERROR: Failed to turn ON channel "  << wiener.chans[ch].name << endl;
                        n_fails++;
                    }
                }
                if (n_fails>0) {
                    cerr << "ERROR: Turning OFF all channels." <<  endl;
                    PS->SetGroupSwitch(0, 0);
                }
            }
        }
    }
    if (fMonitor)
    {
        initscr();
        while (1)
        {
            sleep(updatetime);
            updateWienerData(PS);
            showWienerData();
            refresh();
        }

    }

    if (fCheckConnectivity)
    {
        int num_chans = PS->GetNumberOfChannels();
        ofstream fout("LV_Check.log", ios::out | ios::app);
        initscr();
        while (1)
        {
            sleep(updatetime);
            num_chans = PS->GetNumberOfChannels();
            mvprintw(0,0,"=== %s === %d ================================\n", now().c_str(), num_chans);
            if (num_chans != 12) {
                if (fout.is_open())
                    fout << now() << " " << num_chans << std::endl;
            } else {
                for (unsigned int i=1; i<= num_chans; i++)
                {
                    printw ("Set Channel %d output state to ON\n", i);
                    PS->SetChannelOutputSwitch(i, 1);
                }
            }
            refresh();
        }
    }

}

void showWienerData()
{
    mvprintw(0,0, "%s\tat %s\tSerial Number: %s\n",
             wiener.sys.sysDescr,
             wiener.sys.nodeID,
             wiener.sys.psSerialNumber);
    printw("Uptime: %.2f\tOperating Time: %d\n",
           wiener.sys.sysUpTime,
           wiener.sys.psOperatingTime);
    printw("Main Switch: %s\tSystem Status: 0x%X\n",
           (wiener.sys.sysMainSwitch>0)?"ON":"OFF",
           wiener.sys.sysStatus);
    printw("Ch#\tVmon\tImon\tVset\tIlim\tGroup\tState\tStatus\n");
    printw("==============================================================\n");
    for (unsigned int ch=0; ch< wiener.chans.size(); ch++)
    {
        printw("%s\t%.2f\t%.2f\t%.2f\t%.2f\t%d\t%s\t0x%x\n",
               wiener.chans[ch].name,
               wiener.chans[ch].meas_volt,
               wiener.chans[ch].meas_cur,
               wiener.chans[ch].out_volt,
               wiener.chans[ch].cur_limit,
               wiener.chans[ch].group,
               (wiener.chans[ch].out_switch>0)?"ON":"OFF",
               wiener.chans[ch].status  );
    }
    printw("=== %s ======================================\n", now().c_str());
}

int updateWienerData(Wiener_PL512* PS)
{
    int res = 1;
    PS->ReadValues();
    if (res)
    {
        wiener.sys.sysStatus = PS->GetSystemStatus();
        wiener.sys.sysMainSwitch = PS->GetMainSwitchStatus();
        wiener.sys.psOperatingTime = PS->GetOperatingTime();
        wiener.sys.sysUpTime = ((float)(PS->GetUpTime()))/100;
        memcpy(wiener.sys.nodeID, const_cast<char *>(PS->GetIP()), sizeof(wiener.sys.nodeID));
        memcpy(wiener.sys.sysDescr, PS->GetSystemDescription(), sizeof(wiener.sys.sysDescr));
        memcpy(wiener.sys.psSerialNumber, PS->GetSerialNumber(), sizeof(wiener.sys.psSerialNumber));
        wiener.sys.update_value = 0;
        //  decodeWienerSysStatus(wiener.sys.sysStatus, &(wiener.sys.decoded_status));

        int num_chans = PS->GetNumberOfChannels();
        wiener.chans.clear();
        wiener.chans = std::vector<dimWienerPS_channel>(num_chans);
        for (int ch=0; ch<num_chans; ch++)
        {
            ostringstream ssch;
            ssch << "CHAN" << setw(2) << setfill('0') <<ch;
            memcpy(wiener.chans[ch].name, const_cast<char *>(PS->GetChannelName(ch+1)), sizeof(wiener.chans[ch].name)) ;
            wiener.chans[ch].status = PS->GetChannelStatus(ch+1);
            // decodeWienerChannelStatus(wiener.chans[ch].status, &(wiener.chans[ch].decoded_status));
            // wiener.chans[ch].sup_behaviour = PS->GetChannelSupervisionBehavior(ch+1);
            wiener.chans[ch].meas_volt = PS->GetChannelMeasuredVoltage(ch+1);
            wiener.chans[ch].meas_cur = PS->GetChannelMeasuredCurrent(ch+1);
            wiener.chans[ch].meas_term_volt = PS->GetChannelMeasuredTerminalVoltage(ch+1);
            wiener.chans[ch].out_switch = PS->GetChannelOutputSwitch(ch+1);
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
            wiener.chans[ch].update_value = 0;
        }
    }
    return res;
}
