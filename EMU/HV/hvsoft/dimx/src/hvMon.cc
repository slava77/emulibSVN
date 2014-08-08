#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ncurses.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>


typedef struct chanData
{
  int imin;
  int imax;
  int idelta;
  int iavg;
  long isum;
  int vmin;
  int vmax;
  int vdelta;
  int vavg;
  long vsum;
} chanData;

void usage(char * name)
{
  std::cout << "Usage: " << name << " datafile" << std::endl;
};

std::string now()
{
  char buf[255];
  time_t now=time(NULL);
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
  std::string time = std::string(buf);
  return time.substr(0,time.find("\n",0));

};

// == Data files reading threads
pthread_t       vread_thread=0;
pthread_t       iread_thread=0;
void *trdVReadData(void *);
void *trdIReadData(void *);

// == Display update rate in sec
int updatedelay = 2;

std::string vfilename = "";
std::string ifilename = "";
std::string filename = "";

unsigned long nSamples = 0;
unsigned long vSamples = 0;
unsigned long iSamples = 0;
int nChans = 36;
int vChans = 0;
int iChans = 0;

struct stat vf_stat, if_stat;

struct chanData chdata;
std::vector<struct chanData> monData;

pthread_mutex_t IOmutex = PTHREAD_MUTEX_INITIALIZER;

std::string vstatus="";
std::string istatus="";

void saveData(std::string fn)
{
  FILE* log = fopen(fn.c_str(),"a");
  if (log)
    {
      std::cout << "Saving current state to log file " << fn << std::endl;
      fprintf(log,"Vfile: %s\tsize: %d bytes\tChannels: %d\tStatus: %s\n",vfilename.c_str(), (int)(vf_stat.st_size), vChans, vstatus.c_str());
      fprintf(log,"Ifile: %s\tsize: %d bytes\tChannels: %d\tStatus: %s\n",ifilename.c_str(), (int)(if_stat.st_size), iChans, istatus.c_str());
      fprintf(log,"VSamples read: %d\tISamples read: %d\n", vSamples, iSamples);
      fprintf(log,"Ch#\tImin\tIavg\t\tImax\tIdelta\tVmin\tVavg\t\tVmax\tVdelta\n");
      for (int i=0; i<nChans; i++)
        {
          fprintf(log,"%d\t%d\t%d<-%4d->%d\t%d\t%d\t%d\t%d<-%4d->%d\t%d\t%d\n", i+1,
                  monData[i].imin,
                  monData[i].iavg-monData[i].imin,
                  monData[i].iavg,
                  monData[i].imax-monData[i].iavg,
                  monData[i].imax,
                  monData[i].idelta,
                  monData[i].vmin,
                  monData[i].vavg-monData[i].vmin,
                  monData[i].vavg,
                  monData[i].vmax-monData[i].vavg,
                  monData[i].vmax,
                  monData[i].vdelta );
        }
      fprintf(log,"======= %s ========\n\n", now().c_str());
      fclose(log);
    }
  else
    {
      std::cerr << "Unable to open log file " << fn << std::endl;
    }
}

void unwind()
{
  if (vread_thread) pthread_cancel(vread_thread);
  if (iread_thread) pthread_cancel(iread_thread);

}

// Ctrl-C Handler
static void catch_sigint(int sig)
{
  std::cerr << "hvMon: received signal "<< sig <<  " [Ctrl-C] from user, exiting." << std::endl;
  endwin();
  saveData("mon.log");
  exit(0);
}

void signal_handler(int sig)
{
  std::cerr << "hvMon: received signal " << sig << ", exiting." << std:: endl;;
  exit(0);
}



int main(int argc, char **argv)
{
  struct sigaction sa_old;
  struct sigaction sa_new;

  monData.clear();
  chdata.imin = 0xFFFF;
  chdata.imax = 0;
  chdata.vmin = 0xFFFF;
  chdata.vmax = 0;
  for (int i=0; i< nChans; i++) monData.push_back(chdata);


  if (argc<2)
    {
      usage(argv[0]);
      return 0;
    };
  switch (argc)
    {
    case 2:
      filename = argv[1];
      break;
    }

  if (filename.find("vmon_") != std::string::npos)
    {
      vfilename = filename;
      ifilename = filename.replace(filename.find("vmon_"), 5, "imon_");
    }
  else if (filename.find("imon_") != std::string::npos)
    {
      ifilename = filename;
      vfilename = filename.replace(filename.find("imon_"), 5, "vmon_");
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

  if (pthread_create(&vread_thread,  NULL, trdVReadData, NULL) != 0)
    {
      perror("pthread_create");
      exit(1);
    }
  if (pthread_create(&iread_thread,  NULL, trdIReadData, NULL) != 0)
    {
      perror("pthread_create");
      exit(1);
    }

  initscr();
  bool fRun = true;
  while (fRun)
    {
      sleep(updatedelay);
      stat(vfilename.c_str(), &vf_stat);
      stat(ifilename.c_str(), &if_stat);
      pthread_mutex_lock(&IOmutex);
      mvprintw(0,0,"Vfile: %s\tsize: %d bytes\tChannels: %d\tStatus: %s\n",vfilename.c_str(), (int)(vf_stat.st_size), vChans, vstatus.c_str());
      printw("Ifile: %s\tsize: %d bytes\tChannels: %d\tStatus: %s\n",ifilename.c_str(), (int)(if_stat.st_size), iChans, istatus.c_str());
      printw("VSamples read: %d\tISamples read: %d\n", vSamples, iSamples);
      printw("Ch#\tImin\tIavg\t\tImax\tIdelta\tVmin\tVavg\t\tVmax\tVdelta\n");
      for (int i=0; i<nChans; i++)
        {
          printw("%d\t%d\t%d<-%4d->%d\t%d\t%d\t%d\t%d<-%4d->%d\t%d\t%d\n", i+1,
                 monData[i].imin,
                 monData[i].iavg-monData[i].imin,
                 monData[i].iavg,
                 monData[i].imax-monData[i].iavg,
                 monData[i].imax,
                 monData[i].idelta,
                 monData[i].vmin,
                 monData[i].vavg-monData[i].vmin,
                 monData[i].vavg,
                 monData[i].vmax-monData[i].vavg,
                 monData[i].vmax,
                 monData[i].vdelta );
        }
      printw("======= %s ========\n", now().c_str());
      pthread_mutex_unlock(&IOmutex);
      refresh();
    }
  endwin();
  return 0;
}

void *trdVReadData(void *arg)
{
  char vbuf[255];
  int volt[36];
  memset(vbuf, 0, sizeof(vbuf));
  std::ifstream vfs;
  int vpos;

  vfs.open(vfilename.c_str());
  if (!vfs.good())
    {
      std::cerr << "Unable to open '" << vfilename << "' Exiting..." << std::endl;
      exit(0);
    }

  stat(vfilename.c_str(), &vf_stat);

  while (1)
    {
      if (vfs)
        {
          if (vf_stat.st_size - vpos > 255*4)
            {
              vfs.getline(vbuf, 255);
              if (vfs.eof())
                {
                  std::ostringstream st;
                  st << "End of file at " << vpos << ". Waiting for data...";
                  vstatus = st.str();
                  usleep(100000);
                  vfs.clear();
                  vfs.seekg(0);
                  vfs.seekg(vpos);
                }
              else
                {
                  vpos = vfs.tellg();
                  vstatus = "Reading...";
                  vSamples++;
                  std::stringstream v_st(vbuf);
                  pthread_mutex_lock(&IOmutex);
                  vChans = 0;
                  while (v_st >> volt[vChans]) vChans++;
                  for (int i=0; i<vChans; i++)
                    {
                      if (monData[i].vmin>volt[i]) monData[i].vmin = volt[i];
                      if (monData[i].vmax<volt[i]) monData[i].vmax = volt[i];
                      monData[i].vsum += volt[i];
                      monData[i].vavg = monData[i].vsum/vSamples;
                      monData[i].vdelta =  monData[i].vmax - monData[i].vmin;
                    }
                  pthread_mutex_unlock(&IOmutex);
                }
            }
          else
            {
              //	  vstatus = "Waiting for more data...";
              usleep(updatedelay*1000000);
              stat(vfilename.c_str(), &vf_stat);
            }
        }
    }
}

void *trdIReadData(void *arg)
{

  char ibuf[255];
  int cur[36];
  memset(ibuf, 0, sizeof(ibuf));
  std::ifstream ifs;
  int ipos;

  ifs.open(ifilename.c_str());
  if (!ifs.good())
    {
      std::cerr << "Unable to open '" << ifilename << "' Exiting..." << std::endl;
      exit(0);
    }

  stat(ifilename.c_str(), &if_stat);

  while (1)
    {
      if (ifs)
        {
          if (if_stat.st_size - ipos > 255*4)
            {
              ifs.getline(ibuf, 255);
              if (ifs.eof())
                {
                  std::ostringstream st;
                  st << "End of file at " << ipos << ". Waiting for data...";
                  istatus = st.str();
                  usleep(100000);
                  ifs.clear();
                  ifs.seekg(0);
                  ifs.seekg(ipos);
                  continue;
                }
              else
                {
                  ipos = ifs.tellg();
                  istatus = "Reading...";
                  iSamples++;
                  std::stringstream i_st(ibuf);
                  pthread_mutex_lock(&IOmutex);
                  iChans = 0;
                  while (i_st >> cur[iChans]) iChans++;
                  for (int i=0; i<iChans; i++)
                    {
                      if (monData[i].imin>cur[i]) monData[i].imin = cur[i];
                      if (monData[i].imax<cur[i]) monData[i].imax = cur[i];
                      monData[i].isum += cur[i];
                      monData[i].iavg = monData[i].isum/iSamples;
                      monData[i].idelta =  monData[i].imax - monData[i].imin;

                    }
                  pthread_mutex_unlock(&IOmutex);
                }
            }
          else
            {
              //	  istatus = "Waiting for more data...";
              usleep(updatedelay*1000000);
              stat(ifilename.c_str(), &if_stat);
            }
        }
    }
}
