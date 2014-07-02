#include "hvserver.h"
#include "hvpvss.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>


//=============  mycorr++ ============

int pvss_waiting_conf_cnt[QUEUE_NUMBER];

int pvss_waiting_conf_cnt_pr=0;
int PVSS_WAITING_FOR_CONF_MODE=0;

int PVSS_MODE=0;
pvss_module_summary pvss_o[BROKER_NUMBER];
DimService *pvss_service[BROKER_NUMBER];
int modules_first[BROKER_NUMBER];
int primaries_first[12];

HVPrimaryPVSS pvss_pr_o;
DimService *pvss_pr_service[MAX_PR_NUMBER];
//:::::::
int previous_pr_error_status[MAX_PR_NUMBER];

//::::::::
list<pvss_module_summary> pvss_queue[QUEUE_NUMBER];
list<int> db_index_queue[QUEUE_NUMBER];
list<int> db_index_pr_queue;
list<HVPrimaryPVSS> pvss_pr_queue;


int pvss_channel_status_save[MAX_CARD_NUMBER][HVCARD_MAX_MODULES][36]; // channel_status[card][module][channel]
int pvss_module_status_save[MAX_CARD_NUMBER][HVCARD_MAX_MODULES];
int pvss_module_state_save[MAX_CARD_NUMBER][HVCARD_MAX_MODULES];
int pvss_module_changestatus_save[MAX_CARD_NUMBER][HVCARD_MAX_MODULES];
int pvss_channel_vmon_save[MAX_CARD_NUMBER][HVCARD_MAX_MODULES][36];
int pvss_channel_imon_save[MAX_CARD_NUMBER][HVCARD_MAX_MODULES][36];
int pvss_channel_imax_save[MAX_CARD_NUMBER][HVCARD_MAX_MODULES][36];

int pvss_cnt=0;
int pvss_cnt_pr=0;
int pvss_cnt_freq=0;


static bool FIRST_PVSS_CALL=true;
int STATUS_DELAY_KEEP=15;

bool MASTER_UPDATE_FLAG=false;
bool IS_CONFIRMATION_NEEDED=false;//false;//true; // debug++


//================ end mycorr++ =============


int initPVSS(int hostid)
{
  // ====================== my_corr++ piece 4=============================================================

  if (shmem_setup()<0)exit(0);

  fill_pvss_databases(hostid);
  for (int i=0; i<BROKER_NUMBER; i++)
    {
      if (pvss_service[i] != NULL) delete pvss_service[i];
    }
  for (int i=0; i<4; i++)
    {
      if (pvss_pr_service[i] != NULL) delete pvss_pr_service[i];
    }

  char tmp[20];
  char tmp_2[20];
  string tmp_str;


  int pos, pos_prev=0;

  for (int i=0; i<module_database.size(); i++)
    {
      tmp_str=string("HV_1_")+host_database;

      pos_prev=module_database[i].find(";",0)+1;

      for (int j=0; j<2; j++)
        {
          pos=module_database[i].find(";",pos_prev);
          if (pos == string::npos)pos=module_database[i].size();
          tmp_str+=string("_")+module_database[i].substr(pos_prev,pos-pos_prev);
          pos_prev=pos+1;
        }

      memset(&pvss_o[i],0,sizeof(pvss_o[i]));

      pvss_service[i] = new DimService(tmp_str.c_str(),"I:457",(void *)&pvss_o[i],sizeof(pvss_o[i]));
      LOG4CPLUS_DEBUG (hvlog, "Added PVSS Service: " <<tmp_str.c_str());
    }

  for (int i=0; i<primary_database.size(); i++)
    {
      sprintf(tmp,"%d",primary_database[i]);
      sprintf(tmp_2,"%d",primary_database_2[i]);
      string hv_pr_service="HV_PR_"+host_database+"_"+string(tmp)+"_"+string(tmp_2); // debug++
      pvss_pr_service[i] = new DimService(hv_pr_service.c_str(),"I:1;I:2;I:4;I:1;I:1;I:2;I:5",(void *)&pvss_pr_o,sizeof(pvss_pr_o)); //debug++
      LOG4CPLUS_DEBUG (hvlog, "Added PVSS Service: " <<hv_pr_service.c_str());
      //:::::::::::::
      previous_pr_error_status[i]=0;


      //:::::::::::::
    }

  for (int i=0; i<card_database.size(); i++)  // currently two cards
    {
      for (int j=0; j<16; j++)  // maximum 16 modules per card
        {
          //(map_module_statuses[i]).push_back(0);
          shmem_address->map_module_statuses[i][j]=0;
          for (int p=0; p<2; p++)  // maximum two parts in a module
            {
              shmem_address->module_statuses_count[i][j][p]=0;
            }
        }
    }
  shmem_address->HV_PVSS_STARTED_UP=false;// for synchronization with child (hv_pvss): it comes true after hv_pvss start

  pvss_cnt=0;
  pvss_cnt_pr=0;
  pvss_cnt_freq=0;
  FIRST_PVSS_CALL=true;
  for (int i=0; i<QUEUE_NUMBER; i++)
    {
      pvss_waiting_conf_cnt[i]=0;
    }

  for (int i=0; i<module_database.size(); i++)
    {
      modules_first[i]=-1;
    }
  for (int i=0; i<10; i++)
    {
      primaries_first[i]=-1;
    }
//  LOG4CPLUS_DEBUG (hvlog, "end initPVSS");
  //=========================== end of piece 4================================================================
  return 0;
}

int startPVSSinterface()
{
  LOG4CPLUS_DEBUG (hvlog,"start PVSS");
  //==============mycorr++ execv===================================
  char *myargv[10];
  for (int i=0; i<10; i++)  //name of program and /0
    {
      myargv[i]= (char *) malloc(100); // alloc 100 bytes
      memset(myargv[i],0,100);
    }
  // Kill already fork-ed child
  if (child_pid>0)
    {
      kill(child_pid, SIGKILL);
      waitpid(child_pid, NULL,0);
    }

  child_pid=fork();
  if (child_pid==0)
    {

      strcpy(myargv[0], "hv_pvss");
      // strcat(myargv[1],"");
      sprintf(myargv[1],"-h%d",hcHostID);
      myargv[2]=(char *)0;

      execv("/home/hvuser/bin/hv_pvss",myargv);
      exit(0);

    }
  /// wait(0);
  //signal(SIGTERM,catchterm);
  signal(SIGINT,catchterm);
  LOG4CPLUS_DEBUG (hvlog,"end  startPVSS");
  //signal(SIGQUIT,catchterm);
  //==============end mycorr++ execv======================================
  return 0;
}

int updatePVSSdata()
{
//	printf("loop\n");
  int check=0;
  double dif1=0;
  double dif2=0;
  double dif11=0;
  double dif12=0;

  // ====================== my_corr++ piece 5 =============================================================

  //---  timing ------------
  const  static int timing_factor_1 =  1000; // just to decrease the number of seconds
  const  static int timing_factor_2 =  1000000; // 1000000 corr. to us

  static unsigned long long time_us2[100];
  static unsigned long long time_us2_save[100];
  static timeval tnow2;

  //-----------------------

  //----- timing -----------------
  gettimeofday((timeval *) &tnow2,NULL);
  time_us2_save[0] = ((tnow2.tv_sec)%timing_factor_1)*timing_factor_2 +
                     tnow2.tv_usec*(timing_factor_2/1000000);
  time_us2_save[10] = ((tnow2.tv_sec)%timing_factor_1);
  //-----------------------

  string pvss_command;
  HVcmdDIM cmdDIM;
  cmdDIM.ID = 0;
  cmdDIM.data.chan=ALL;
  cmdDIM.data.cmd = (HV_CMD)0;
  cmdDIM.data.data = 0;
  cmdDIM.data.size = (ULONG)-1;
  char tmp[20];
  char tmp_2[20];


  if (shmem_address->HV_PVSS_STARTED_UP == true) // for synchronization with child (hv_pvss): it comes true after hv_pvss start
    {


      pvss_cnt++;
      pvss_cnt_pr++;
      if (!FIRST_PVSS_CALL)pvss_cnt_freq++;


      pvss_waiting_conf_cnt_pr++;

      for (int i=0; i<card_database.size(); i++)  // currently two cards
        {
          for (int j=0; j<HVCARD_MAX_MODULES; j++)  // maximum 16 modules per card
            {
              //(map_module_statuses[i]).push_back(0);
              for (int p=0; p<2; p++)  // maximum two parts in a module
                {
                  if (shmem_address->module_statuses_count[i][j][p])shmem_address->module_statuses_count[i][j][p]--;
                }
            }
        }

      if (FIRST_PVSS_CALL && pvss_cnt ==1)
        {
          LOG4CPLUS_DEBUG (hvlog, "PVSS waiting for DIM services to be initialized. Delay=" << (module_database.size() +3));
        }

      if (pvss_cnt == ((module_database.size()) +3) && FIRST_PVSS_CALL) // get_data for all modules in database at the start of the program :attention: delay (proportional to modules number) to let domserveices to be initialized
        {
          clearPVSSqueue(); //debug++
          for (int i=0; i<masters.size(); i++)
            {
              cmdDIM.data.card = masters[i].card;
              cmdDIM.data.module = masters[i].module;
              cmdDIM.data.size = (ULONG)-1;
              pthread_mutex_lock(&CSCMonmutex);
              // commands_queue["HVDATA"].push_back(cmdDIM);
              pvssInterfaceRealHVData( &(cmdDIM.data));
              pthread_mutex_unlock(&CSCMonmutex);
            }

          for (int i=0; i<dbs.size(); i++)
            {
              cmdDIM.data.card = dbs[i].card;
              cmdDIM.data.module = dbs[i].module;
              cmdDIM.data.size = (ULONG)-1;
              pthread_mutex_lock(&CSCMonmutex);
              // commands_queue["HVDATA"].push_back(cmdDIM);
              pvssInterfaceRealHVData( &(cmdDIM.data));
              pthread_mutex_unlock(&CSCMonmutex);
            }

          for (int i=0; i<primary_database.size(); i++)
            {
              cmdDIM.data.card = primary_database[i];
              cmdDIM.data.module = primary_database_2[i];
              pthread_mutex_lock(&CSCMonmutex);
              // commands_queue["HVPRDATA"].push_back(cmdDIM);
              pvssInterfacePrimaryHVData( &(cmdDIM.data));
              pthread_mutex_unlock(&CSCMonmutex);
            }

          pvss_cnt=0;
          pvss_cnt_pr=0;
          pvss_cnt_freq=0;
          FIRST_PVSS_CALL=false;
        }

      if (pvss_cnt>(3200*2)) // 24h periodical get_data for all distr. modules in the database
        {
          for (int i=0; i<dbs.size(); i++)
            {
              cmdDIM.data.card = dbs[i].card;
              cmdDIM.data.module = dbs[i].module;
              cmdDIM.data.size = (ULONG)-1; // modified
              pthread_mutex_lock(&CSCMonmutex);
              // commands_queue["HVDATA"].push_back(cmdDIM);
              pvssInterfaceRealHVData( &(cmdDIM.data));
              pthread_mutex_unlock(&CSCMonmutex);
            }

          pvss_cnt=0;
        }

      if (pvss_cnt_pr>(3200)) // periodical get_data for all primaries in the database
        {
          for (int i=0; i<primary_database.size(); i++)
            {
              cmdDIM.data.card = primary_database[i];
              cmdDIM.data.module = primary_database_2[i];
              pthread_mutex_lock(&CSCMonmutex);
              // commands_queue["HVPRDATA"].push_back(cmdDIM);
              pvssInterfacePrimaryHVData( &(cmdDIM.data));
              pthread_mutex_unlock(&CSCMonmutex);
            }
          pvss_cnt_pr=0;
        }



      if (pvss_cnt_freq>(PVSS_UPDATE_INTERVAL)) // periodical get_data for all distr. modules in the database
        {
          for (int i=0; i<masters.size(); i++)
            {
              cmdDIM.data.card = masters[i].card;
              cmdDIM.data.module = masters[i].module;
              cmdDIM.data.size = (ULONG)-2;
              pthread_mutex_lock(&CSCMonmutex);
              // commands_queue["HVDATA"].push_back(cmdDIM);
              pvssInterfaceRealHVData( &(cmdDIM.data));
              pthread_mutex_unlock(&CSCMonmutex);
            }

          for (int i=0; i<dbs.size(); i++)
            {
              cmdDIM.data.card = dbs[i].card;
              cmdDIM.data.module = dbs[i].module;
              cmdDIM.data.size = (ULONG)-2;
              pthread_mutex_lock(&CSCMonmutex);
              // commands_queue["HVDATA"].push_back(cmdDIM);
              pvssInterfaceRealHVData( &(cmdDIM.data));
              pthread_mutex_unlock(&CSCMonmutex);
            }

          for (int i=0; i<primary_database.size(); i++)
            {

              cmdDIM.data.card = primary_database[i];
              cmdDIM.data.module = primary_database_2[i];
              cmdDIM.data.size = (ULONG)-1;
              pthread_mutex_lock(&CSCMonmutex);
              // commands_queue["HVPRDATA"].push_back(cmdDIM);
              pvssInterfacePrimaryHVData( &(cmdDIM.data));
              pthread_mutex_unlock(&CSCMonmutex);
            }
          //:::::::::::::::::::::::::::::::::::::::

          pvss_cnt_freq=0;
        }
      //===========
      //===========

      //------ timing ----------------
      gettimeofday((timeval *) &tnow2,NULL);
      time_us2_save[1] = ((tnow2.tv_sec)%timing_factor_1)*timing_factor_2 +
                         tnow2.tv_usec*(timing_factor_2/1000000);
      time_us2_save[11] = ((tnow2.tv_sec)%timing_factor_1);
      //-----------------------

      check=0;


      while (pvss_queue[0].size()>0) // || PVSS_WAITING_FOR_CONF_MODE==1)
        {

          int db_index=db_index_queue[0].front();
          pvss_o[db_index]=pvss_queue[0].front();

          if (pvss_pr_queue.size()>0)break;

          //--------------- synch with pvss states-----------------------------
          if (modules_first[db_index]==-1)
            {
              if (pvss_o[db_index].module_type==3)
                {
                  pvss_o[db_index].module_state=(HV_STATE)5005;
                }
              else if (pvss_o[db_index].module_type==2)
                {
                  pvss_o[db_index].module_state=(HV_STATE)5;
                }
              modules_first[db_index]=0;
            }
          //------------------------------------------------------------

#ifdef PVSSDEBUG_1
          printf("update 1   module_number=%d %d \n", pvss_o[db_index].module_number, pvss_o[db_index].module_state);
          printf("update 2   module_number=%d module_type=%d host_id=%d host_slot=%d\n",
                 pvss_o[db_index].module_number, pvss_o[db_index].module_type, pvss_o[db_index].host_hostid,pvss_o[db_index].host_slot);
#endif
          // pthread_mutex_lock(&CSCMonmutex);
          // pvss_service[db_index]->updateService();
          // pthread_mutex_unlock(&CSCMonmutex);

          pvss_queue[0].pop_front();
          db_index_queue[0].pop_front();

//--- timing -------------------
          gettimeofday((timeval *) &tnow2,NULL);
          time_us2_save[2] = ((tnow2.tv_sec)%timing_factor_1)*timing_factor_2 +
                             tnow2.tv_usec*(timing_factor_2/1000000);
          dif2= (((double)(time_us2_save[2] - time_us2_save[1]))/1000.);
          if (dif2 > 1900)break;
//-------------------------------
        } // while

      if (pthread_mutex_trylock(&CSCMonmutex) == EBUSY)
        {
          LOG4CPLUS_DEBUG (hvlog, "#4 CSC Monitoring thread mutex is in BUSY state.Skipping...");
        }
      else
        {
          while (pvss_pr_queue.size()>0) // || PVSS_WAITING_FOR_CONF_MODE==1)
            {
//--- timing -------------------
              gettimeofday((timeval *) &tnow2,NULL);
              time_us2_save[2] = ((tnow2.tv_sec)%timing_factor_1)*timing_factor_2 +
                                 tnow2.tv_usec*(timing_factor_2/1000000);
              dif2= (((double)(time_us2_save[2] - time_us2_save[1]))/1000.);
              if (dif2 > 1900)break;
//-------------------------------
              //if(!PVSS_WAITING_FOR_CONF_MODE){
              int db_index=db_index_pr_queue.front();
	      cout << "updating Primary PS data " << db_index << endl;
/////		printf("pr size=%d %d\n",pvss_pr_queue.size(),db_index);
              pvss_pr_o=pvss_pr_queue.front();
              //--------------- synch with pvss states-----------------------------
              if (primaries_first[db_index]==-1)
                {
                  pvss_pr_o.hp.output=5;
                  primaries_first[db_index]=0;
                  //		  printf("%d %d\n",db_index);
                }

//--- timing -------------------
              gettimeofday((timeval *) &tnow2,NULL);
              time_us2_save[21] = ((tnow2.tv_sec)%timing_factor_1)*timing_factor_2 +
                                  tnow2.tv_usec*(timing_factor_2/1000000);
//-------------------------------
              pvss_pr_service[db_index]->updateService(); // delay here can be caused by the parallel work of the getting data (in driver?) vcaused by sending commands
//--- timing -------------------
              gettimeofday((timeval *) &tnow2,NULL);
              time_us2_save[22] = ((tnow2.tv_sec)%timing_factor_1)*timing_factor_2 +
                                  tnow2.tv_usec*(timing_factor_2/1000000);
              dif2= (((double)(time_us2_save[22] - time_us2_save[21]))/1000.);
//-------------------------------
              PVSS_WAITING_FOR_CONF_MODE=0;
              pvss_waiting_conf_cnt_pr=0;

              if (pvss_pr_o.hp.polarity != 0) LOG4CPLUS_WARN(hvlog, "POLARITY !=0   -----+++ > " << pvss_pr_o.hp.polarity);

              pvss_pr_queue.pop_front();
              db_index_pr_queue.pop_front();
#ifdef PVSSDEBUG_1
              printf("update\n");
#endif
              /////////     break;

//--- timing -------------------
              gettimeofday((timeval *) &tnow2,NULL);
              time_us2_save[2] = ((tnow2.tv_sec)%timing_factor_1)*timing_factor_2 +
                                 tnow2.tv_usec*(timing_factor_2/1000000);
              dif2= (((double)(time_us2_save[2] - time_us2_save[1]))/1000.);
              if (dif2 > 1900)break;
//-------------------------------
              break;
            } // while
          pthread_mutex_unlock(&CSCMonmutex);
        }


//--- timing -------------------
      gettimeofday((timeval *) &tnow2,NULL);
      time_us2_save[2] = ((tnow2.tv_sec)%timing_factor_1)*timing_factor_2 +
                         tnow2.tv_usec*(timing_factor_2/1000000);
      time_us2_save[12] = ((tnow2.tv_sec)%timing_factor_1);


      dif1= (((double)(time_us2_save[1] - time_us2_save[0]))/1000.);
      dif2= (((double)(time_us2_save[2] - time_us2_save[1]))/1000.);

      dif11= ((double)(time_us2_save[11] - time_us2_save[10] ));
      dif12= ((double)(time_us2_save[12] - time_us2_save[11] ));


    } // if(shmem_address->HV_PVSS_STARTED_UP == true)
  else
    {
      printf("HV_PVSS_STARTED_UP = false\n");
    }


  //========================  end of piece 5====================================================================


  if (dif1 > 4000 || dif2 > 4000 || (dif1+dif2) > 4000)
    {
      LOG4CPLUS_WARN(hvtiminglog, "PVSS timing exceeds 2 seconds: part1="<< dif1 <<"  part2=" << dif2);
      LOG4CPLUS_WARN(hvtiminglog, "PVSS timing exceeds 2 seconds: part11="<< dif11 <<"  part12=" << dif12);
    }
  if (dif11 > 4. || dif12 > 4. || (dif11+dif12) > 4.)
    {
      LOG4CPLUS_WARN(hvtiminglog, "PVSS timing exceeds 2 seconds: part1="<< dif1 <<"  part2=" << dif2);
      LOG4CPLUS_WARN(hvtiminglog, "PVSS timing exceeds 2 seconds: part11="<< dif11 <<"  part12=" << dif12);
    }

  //-----------------------

  return 0;
}

void clearPVSSqueue()
{
  //============  my_corr++ ========================================
  for (int i=0; i<QUEUE_NUMBER; i++)
    {
      pvss_queue[i].clear(); // clear masters after autodetect //temp?
      db_index_queue[i].clear();
    }
  db_index_pr_queue.clear();
  pvss_pr_queue.clear(); //debug++
  //================================================================
}

