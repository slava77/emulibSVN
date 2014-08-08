#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdarg.h>
#include <ncurses.h>
#include <signal.h>
#include <unistd.h>

#include "hvprimary.h"


int port=0, addr=1;
// =========================================
// End of HV Library Functions
// =========================================
void usage()
{
  printf("Usage: HVLib -[options]\n");
  printf("Options:\n");
  printf("\t-h(-?)\tshow this usage info\n");
  printf("\t-pN\tset serial port to use (default 0)\n");
  printf("\t-aN\tset GPIB module address (default 1)\n");
  printf("\t-e\tenable HV power supply output\n");
  printf("\t-d\tdisable HV powersupply output\n");
  printf("\t-r\treset HV power supply\n");
  printf("\t-iNN\tset maximum output current in percents (default 10% = 6mA)\n");
  printf("\t-vNNNN\tset output voltage in Volts\n");
  printf("\t-u\tset ramping speed in Volts/sec (default 50V/s)\n");

};

void unwind()
{
  endwin();
  if (CloseCOM(port)==0)
    {
      printf("COM Port%d for Module%d is closed\n", port, addr);
    }
  printf("hvPrimeCtrl: exit called.\n");
  // unlink(process_lock_filename);
}

// Ctrl-C Handler
static void catch_sigint(int sig)
{
  printf("hvPrimeCtrl: received signal %d [Ctrl-C] from user, exiting.\n", sig);
  // syslog(LOG_INFO|LOG_LOCAL1,"received signal %d [Ctrl-C] from user, exiting.",sig);
  exit(0);
}

void signal_handler(int sig)
{
  printf("hvPrimCtrl: received signal %d, exiting.",sig);
  // syslog(LOG_INFO|LOG_LOCAL1,"received signal %d, exiting.",sig);
  exit(0);
}


int main(int argc, char **argv)
{
  int imax=10, vset=-1, ramp=50, vcur=0, delta=0;
  int fEnaOut=1, fReset=0, fShow=1;
  unsigned int val=0,i=0;
  float fval;
  struct sigaction sa_old;
  struct sigaction sa_new;
  if (argc >= 2)
    {
      for (i=1; i< argc; i++)
        {
          if (argv[i][0] == '-')
            {
              switch (argv[i][1])
                {
                case 'h' :
                case '?' :
                  usage();
                  return 0;
                case 'p' :
                  port = atoi(argv[i]+2);
                  port = (port>0) ? (port<2?port:1):0;
                  break;
                case 'a' :
                  addr =  atoi(argv[i]+2);
                  addr = (addr>0) ? (addr<=0x10?addr:0x10) :0;
                  break;
                case 'e' :
                  fEnaOut=1;
                  break;
                case 'd' :
                  fEnaOut=0;
                  break;
                case 'r' :
                  fReset=1;
                  break;
                case 'i' :
                  imax = atoi(argv[i]+2);
                  imax = (imax>0) ? (imax<100?imax:100):0;
                  break;
                case 'v' :
                  vset = atoi(argv[i]+2);
                  vset = (vset>0) ? (vset<4000?vset:4000):0;
                  break;
                case 'u' :
                  ramp = atoi(argv[i]+2);
                  ramp = (ramp>0) ? (ramp<4000?ramp:4000):1;
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
      return 0;
    }


  /* set up our exit routine */
  atexit(unwind);

  /* set up the signal handler */
  signal(SIGTERM,signal_handler);

  // set up signal handling
  sa_new.sa_handler = catch_sigint;
  sigemptyset(&sa_new.sa_mask);
  sa_new.sa_flags = 0;
  sigaction(SIGINT, &sa_new, &sa_old );

  if (InitCOM(port)<0)
    {
      printf("Unable to open COM Port%d\n", port );
      return -1;
    }
  else printf("COM Port%d is opened\n", port);
  //  Enable and start HV Remote Control
  if (SendHVCmd(port, addr, "REN")<0)
    return -1;

  if (GetControlStatus(port, addr, &val) <0)
    {
      printf("ERROR: Unable to communicate with Module%d at COM%d. Exiting...\n", addr, port);
      return -1;
    };
  /*if (InitHV(port, addr, fEnaOut)==0) {
  	printf("COM Port%d for Module%d is opened\n", port, addr);
  }
  */
  if (fReset) ResetHV(port, addr);
  if (SetHVMaxCurrentPercent(port, addr, (float)imax) <0) return -1;

  if (fEnaOut) EnableHVOutput(port, addr);
  else
    {
      DisableHVOutput(port, addr);
    }

  // if ((vset>-1) && (vset<=5000))
  // 	SetHVMaxVoltage(port, addr, (float)vset);

  // ReadHVMaxCurrent(port, addr, &fval);
  // printf("Imax(mkA): %.f ", fval);
  ReadHVVoltage(port, addr, &fval);
  vcur = (int)fval;

  initscr();
  while (1)
    {
      sleep(1);
      mvprintw(0,0,"COM:%d GPIB Addr:%d\n", port, addr);

      ReadHVMaxCurrent(port, addr, &fval);
      printw("Imax:\t %4.f mkA ", fval);
      ReadHVMaxCurrentHex(port, addr, (int*)&val);
      printw("\t0x%04x ", val);
      ReadHVMaxCurrentPercent(port, addr, &fval);
      printw("\t%5.2f\%\n", fval);

      ReadHVMaxVoltage(port, addr, &fval);
      printw("Vmax:\t %4.f V ", fval);
      ReadHVMaxVoltageHex(port, addr, (int*)&val);
      printw("\t0x%04x ", val);
      ReadHVMaxVoltagePercent(port, addr, &fval);
      printw("\t%5.2f\%\n", fval);


      ReadHVCurrent(port, addr, &fval);
      printw("Imon:\t %4.f mkA ", fval);
      ReadHVCurrentHex(port, addr, (int*)&val);
      printw("\t0x%04x ", val);
      ReadHVCurrentPercent(port, addr, &fval);
      printw("\t%5.2f\%\n", fval);

      ReadHVVoltage(port, addr, &fval);
      printw("Vmon:\t %4.f V ", fval);
      ReadHVVoltageHex(port, addr, (int*)&val);
      printw("\t0x%04x ", val);
      ReadHVVoltagePercent(port, addr, &fval);
      printw("\t%5.2f\%\n", fval);

      GetOutputStatus(port, addr, &val);
      printw("Output:\t %s\n", (val)?"Enabled":"Disabled");
      GetControlStatus(port, addr, &val);
      printw("Remote:\t %s\n", (val)?"Enabled":"Disabled");


      if ((vset>-1) && (vset<=4000))
        {
          delta = vset - vcur;
          delta = (delta>0)?((ramp>abs(delta))?delta:ramp):((ramp>abs(delta))?delta:-ramp);
          vcur += delta;
          SetHVMaxVoltage(port, addr, (float)vcur);
        }
      refresh();

    }
  endwin();
//	sleep(2);
  if (CloseCOM(port)==0)
    {
      printf("COM Port%d for Module%d is closed\n", port, addr);
    }

}
