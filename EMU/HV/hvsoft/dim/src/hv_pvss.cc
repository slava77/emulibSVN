#include <dic.hxx>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <ncurses.h>

#include "hvdim.h"
#include "hvprimary.h"
#include <list>

#include <sys/time.h>

using namespace std;

list<HVcmdDIM> commands_cmd_queue;
list<HVcmdDIM> commands_data_queue;
list<HVcmdDIM> commands_cmdpr_queue;
list<HVcmdDIM> commands_datapr_queue;

DimRpcInfo *rpc_pvss_client_cmd;
DimRpcInfo *rpc_pvss_client_data;
DimRpcInfo *rpc_pvss_client_pr_cmd;
DimRpcInfo *rpc_pvss_client_pr_data;
HVcmdDIM pvss_hvcmd;


#include "ipc.h"
#include "pvss_db_ext.h"

extern shmem_type *shmem_address;

class PvssCommand;
PvssCommand *pvss_command;
PvssCommand *pvss_command_2;
PvssCommand *pvss_command_3;

int STATUS_DELAY_KEEP=15; // cycle is now 1 sec //50*15; // number of cycles in the server loop (20 ms) to keep status until defining it by channel statuses.

vector<string> command_names;



FILE *fp2;


//====================================


class PvssCommand: public DimCommand
{

  string command_name;


  void commandHandler()
  {


    string sub_command=getString();

//-----------------------
    char buf[255];
    time_t now;
    now=time(NULL);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
//printf("cmd 1 --- %s %d %d %d %d %d\n",buf,buffer_command.data.card,buffer_command.data.module,buffer_command.data.chan,buffer_command.data.cmd,buffer_command.data.data);

///fprintf(fp2,"cmd 2 %s %s \n",buf,sub_command.c_str());
//------------------------


    int index_for_confirmation;
    for (int i=0; i<command_names.size(); i++)
      {
        if (command_names[i] == command_name)index_for_confirmation=i;
      }


    if (sub_command=="CONFIRMATION")
      {
        shmem_address->PVSS_CONFIRMATION[index_for_confirmation]=true;
#ifdef PVSSDEBUG_1
        printf("PVSS_CONFIRMATION=true\n");
#endif
      }
    else if (sub_command.find("HVCMD",0) != string::npos || sub_command.find("HVDATA",0) != string::npos
             || sub_command.find("HVPRCMD",0) != string::npos || sub_command.find("HVPRDATA",0) != string::npos)
      {
#ifdef PVSSDEBUG_5
        printf("server: %s\n",sub_command.c_str());
#endif

        int pos[6], pos_prev=0;
        string set_host=sub_command.substr(0,pos_prev=sub_command.find("|",0));
        pos_prev=pos_prev+1;
        for (int i=0; i<6; i++)
          {
            pos[i]=sub_command.find(";",pos_prev);
            pos_prev=pos[i]+1;
          }
        for (int i=0; i<6; i++)
          {
            if (i==0)pvss_hvcmd.data.card=atoi((sub_command.substr(pos[i]+1, pos[i+1]-pos[i]-1)).c_str());
            if (i==1)pvss_hvcmd.data.module=atoi((sub_command.substr(pos[i]+1, pos[i+1]-pos[i]-1)).c_str());
            if (i==2)pvss_hvcmd.data.chan=atoi((sub_command.substr(pos[i]+1, pos[i+1]-pos[i]-1)).c_str());
            if (i==3)pvss_hvcmd.data.cmd=(HV_CMD)atoi((sub_command.substr(pos[i]+1, pos[i+1]-pos[i]-1)).c_str());
            if (i==4)pvss_hvcmd.data.data=atoi((sub_command.substr(pos[i]+1, pos[i+1]-pos[i]-1)).c_str());
            if (i==5)pvss_hvcmd.data.size=atoi((sub_command.substr(pos[i]+1)).c_str());
          }




        int found=0;
#ifdef PVSSDEBUG_10
        cout << set_host << endl;
        cout << (sub_command.substr(pos[0]+1, pos[0+1]-pos[0]-1)) << endl;
        cout << (sub_command.substr(pos[1]+1, pos[1+1]-pos[1]-1)) << endl;
#endif

        if (sub_command.find("HVCMD",0) != string::npos || sub_command.find("HVDATA",0) != string::npos)
          {
            for (int i=0; i<module_database.size(); i++)
              {
                if (set_host+";"+(sub_command.substr(pos[0]+1, pos[0+1]-pos[0]-1))+";"+ (sub_command.substr(pos[1]+1, pos[1+1]-pos[1]-1)) == host_database + module_database[i])
                  {
                    found=1;
                    break;
                  }
              }
            if (!found)return;
            found=0;
            for (int i=0; i<card_database.size(); i++)
              {
                if (pvss_hvcmd.data.card == card_database[i])
                  {
                    //	  pvss_hvcmd.data.card=i;
                    pvss_hvcmd.data.card=card_database[i];
                    found=1;
                    break;
                  }
              }
            if (!found)return;
          }
        else
          {
            found=0;
            for (int i=0; i<primary_database.size(); i++)
              {
                if (set_host == host_database && pvss_hvcmd.data.card == primary_database[i] && pvss_hvcmd.data.module == primary_database_2[i])
                  {
                    //	  pvss_hvcmd.data.card=i;
                    pvss_hvcmd.data.card=primary_database[i];
                    found=1;
                    break;
                  }
              }
            if (!found)return;

          }
        //----------------------------------------------
        if (sub_command.find("HVCMD",0) != string::npos)
          {
            //     usleep(20000);
            // printf("command======%s\n",sub_command.c_str());
///	shmem_address->data_refresh_needed=true;
            if (pvss_hvcmd.data.chan==253 && pvss_hvcmd.data.cmd <=1)
              {
///	  for(int i=0;i<18;i++){
///	    pvss_hvcmd.data.chan=i;
                commands_cmd_queue.push_back(pvss_hvcmd);
                /// rpc_pvss_client_cmd->setData((void *) &pvss_hvcmd, sizeof(pvss_hvcmd));
///	  }
///	  shmem_address->map_module_statuses[pvss_hvcmd.data.card][pvss_hvcmd.data.module]=
///	    (shmem_address->map_module_statuses[pvss_hvcmd.data.card][pvss_hvcmd.data.module]/1000)*1000
///	    +(pvss_hvcmd.data.cmd+1);
///	  shmem_address->module_statuses_count[pvss_hvcmd.data.card][pvss_hvcmd.data.module][0]=STATUS_DELAY_KEEP; // part 1 of the module

              }
            else if (pvss_hvcmd.data.chan==254 && pvss_hvcmd.data.cmd <=1)
              {
///	  for(int i=18;i<36;i++){
///	    pvss_hvcmd.data.chan=i;
                commands_cmd_queue.push_back(pvss_hvcmd);
                ///rpc_pvss_client_cmd->setData((void *) &pvss_hvcmd, sizeof(pvss_hvcmd));
///	  }
///	  shmem_address->map_module_statuses[pvss_hvcmd.data.card][pvss_hvcmd.data.module]=
///	    (shmem_address->map_module_statuses[pvss_hvcmd.data.card][pvss_hvcmd.data.module]%1000)
///	    +(pvss_hvcmd.data.cmd+1)*1000;
///	  shmem_address->module_statuses_count[pvss_hvcmd.data.card][pvss_hvcmd.data.module][1]=STATUS_DELAY_KEEP; // part 2 of the module
              }
            else
              {
                commands_cmd_queue.push_back(pvss_hvcmd);
                /////rpc_pvss_client_cmd->setData((void *) &pvss_hvcmd, sizeof(pvss_hvcmd));
///	  if(pvss_hvcmd.data.chan==255 && pvss_hvcmd.data.cmd <=1){
///	    shmem_address->map_module_statuses[pvss_hvcmd.data.card][pvss_hvcmd.data.module]=pvss_hvcmd.data.cmd+1;
///	    shmem_address->module_statuses_count[pvss_hvcmd.data.card][pvss_hvcmd.data.module][0]=STATUS_DELAY_KEEP; // part 1 of the module
///	  }
              }
          } // if HVCMD
        else if (sub_command.find("HVDATA",0) != string::npos)
          {
            //   usleep(20000);
            commands_data_queue.push_back(pvss_hvcmd);
            ////rpc_pvss_client_data->setData((void *) &pvss_hvcmd, sizeof(pvss_hvcmd));
            // printf("HVDATA %s\n", sub_command.c_str());
          }
        else if (sub_command.find("HVPRCMD",0) != string::npos)
          {
            commands_cmdpr_queue.push_back(pvss_hvcmd);
            // printf("HVPRCMD %s\n",sub_command.c_str());
            ///rpc_pvss_client_pr_cmd->setData((void *) &pvss_hvcmd, sizeof(pvss_hvcmd));
          }
        else if (sub_command.find("HVPRDATA",0) != string::npos)
          {
            commands_datapr_queue.push_back(pvss_hvcmd);
            //  printf("HVPRDATA %s\n",sub_command.c_str());
            ///rpc_pvss_client_pr_data->setData((void *) &pvss_hvcmd, sizeof(pvss_hvcmd));
          }

      }
  }

public:
  PvssCommand(char *command_name):DimCommand(command_name,"C")
  {

    this->command_name=string(command_name);
    //   this->cfeb=cfeb;
    //    this->alct=alct;

  }

};
//== end of mycorr++ piece 1




//======================================================================
//======================================================================
//======================================================================
//======================================================================
//======================================================================
//======================================================================
//======================================================================



int main(int argc, char **argv)
{

  FILE *fp;
  fp=fopen("temp_output.txt","wt");
  fp2=fopen("temp_output2.txt","wt");
  time_t now=time(NULL);
  char buf[255];







  //---  timing ------------

  double dif1=0;
  double dif2=0;

  const  static int timing_factor_1 =  1000; // just to decrease the number of seconds
  const  static int timing_factor_2 =  1000000; // 1000000 corr. to us

  static unsigned long long time_us2[100];
  static unsigned long long time_us2_save[100];
  static timeval tnow2;

  //-----------------------



  string servername=HV_DIM_NAME;
  stringstream sname;
  int hostid = 0;
  bool fHostID = false;
  HVcmdDIM buffer_command;

  if (argc >= 2)
    {
      for (int i=1; i< argc; i++)
        {
          if (argv[i][0] == '-')
            {
              switch  (argv[i][1])
                {
                case 'h' :
                  hostid =  atoi(argv[i]+2);
                  hostid = (hostid>0)?hostid:0;
                  sname << servername << "_HID" << hostid;
                  sname >> servername;
                  fHostID = true;
                  break;
                }
            }
        }
    }

  shmem_setup();
  //  pvss_databases();
  fill_pvss_databases(hostid);

  char tmp[20];

  shmem_address->data_refresh_needed=false;// initialization
  for (int i=1; i<=100; i++)
    {
      sprintf(tmp,"%d",i);
      command_names.push_back("HV_"+string(tmp)+"_COMMAND");
      shmem_address->PVSS_CONFIRMATION[i-1]=true; // initialization
    }

  rpc_pvss_client_cmd= new  DimRpcInfo(string(servername+"/HV_REAL_CMD").c_str(), -1);
  rpc_pvss_client_data= new  DimRpcInfo(string(servername+"/HV_REAL_DATA").c_str(), -1);
  rpc_pvss_client_pr_cmd= new  DimRpcInfo(string(servername+"/HV_PRIMARY_CMD").c_str(), -1);
  rpc_pvss_client_pr_data= new  DimRpcInfo(string(servername+"/HV_PRIMARY_DATA").c_str(), -1);

  pvss_command= new PvssCommand("HV_1_COMMAND");
  pvss_command_2= new PvssCommand("HV_2_COMMAND");
  pvss_command_3= new PvssCommand("HV_3_COMMAND");

  DimServer::start("PVSS_INTER");

  //        shmem_address->PVSS_CONFIRMATION=true; // initialization
  shmem_address->HV_PVSS_STARTED_UP=true; // for synchronization with hvServer
  while (1)
    {
      usleep(10000);

//------------------------------------------------------------------------
      if (commands_cmdpr_queue.size()>0)
        {

          buffer_command=commands_cmdpr_queue.front();
          rpc_pvss_client_pr_cmd->setData((void *) &buffer_command, sizeof(buffer_command));
          commands_cmdpr_queue.pop_front();

#ifdef PVSSDEBUG_1
          printf("update\n");
#endif

        } // if
//------------------------------------------------------------------------
//------------------------------------------------------------------------
      if (commands_datapr_queue.size()>0)
        {

          buffer_command=commands_datapr_queue.front();
          rpc_pvss_client_pr_data->setData((void *) &buffer_command, sizeof(buffer_command));
          commands_datapr_queue.pop_front();

#ifdef PVSSDEBUG_1
          printf("update\n");
#endif

        } // if

//------------------------------------------------------------------------

      if (commands_cmd_queue.size()>0)
        {
          shmem_address->data_refresh_needed=true;


          buffer_command=commands_cmd_queue.front();
//--- timing -------------------
          gettimeofday((timeval *) &tnow2,NULL);
          time_us2_save[0] = ((tnow2.tv_usec)%1000000);

//-------------------------------
          rpc_pvss_client_cmd->setData((void *) &buffer_command, sizeof(buffer_command));
//--- timing -------------------
          gettimeofday((timeval *) &tnow2,NULL);
          time_us2_save[1] = ((tnow2.tv_usec)%1000000);
          dif1= (((double)(time_us2_save[1] - time_us2_save[0]))/1000.);

//-------------------------------

          commands_cmd_queue.pop_front();

          now=time(NULL);
          strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
//printf("cmd 1 --- %s %d %d %d %d %d\n",buf,buffer_command.data.card,buffer_command.data.module,buffer_command.data.chan,buffer_command.data.cmd,buffer_command.data.data);

////fprintf(fp,"cmd 1 %s %f --- %d %d %d %d %d\n",buf,dif1,buffer_command.data.card,buffer_command.data.module,buffer_command.data.chan,buffer_command.data.cmd,buffer_command.data.data);

// strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
////fflush(fp);


          if (buffer_command.data.chan==253 && buffer_command.data.cmd <=1)
            {

              shmem_address->map_module_statuses[buffer_command.data.card][buffer_command.data.module]=
                (shmem_address->map_module_statuses[buffer_command.data.card][buffer_command.data.module]/1000)*1000
                +(buffer_command.data.cmd+1);
              shmem_address->module_statuses_count[buffer_command.data.card][buffer_command.data.module][0]=STATUS_DELAY_KEEP; // part 1 of the module

            }
          else if (buffer_command.data.chan==254 && buffer_command.data.cmd <=1)
            {

              shmem_address->map_module_statuses[buffer_command.data.card][buffer_command.data.module]=
                (shmem_address->map_module_statuses[buffer_command.data.card][buffer_command.data.module]%1000)
                +(buffer_command.data.cmd+1)*1000;
              shmem_address->module_statuses_count[buffer_command.data.card][buffer_command.data.module][1]=STATUS_DELAY_KEEP; // part 2 of the module
            }
          else
            {
              if (buffer_command.data.chan==255 && buffer_command.data.cmd <=1)
                {
                  shmem_address->map_module_statuses[buffer_command.data.card][buffer_command.data.module]=buffer_command.data.cmd+1;
                  shmem_address->module_statuses_count[buffer_command.data.card][buffer_command.data.module][0]=STATUS_DELAY_KEEP; // part 1 of the module
                }
            }


#ifdef PVSSDEBUG_1
          printf("update\n");
#endif
//printf("cmd 2 --- \n");
        } // if
//-------------------------------------------------------------------------
//------------------------------------------------------------------------
      if (commands_data_queue.size()>0)
        {
///		if(commands_cmd_queue.size()>0)continue;
//printf("data 1 %s\n");

          buffer_command=commands_data_queue.front();
          rpc_pvss_client_data->setData((void *) &buffer_command, sizeof(buffer_command));
          commands_data_queue.pop_front();

#ifdef PVSSDEBUG_1
          printf("update\n");
#endif
//printf("data 2 %s\n");
        } // if
//-------------------------------------------------------------------------


    }; // while


  return 0;
}

//=================
