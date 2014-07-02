#include "hvdim.h"
#include "hvpvss.h"
#include "hvserver.h"
#include <iostream>


void PvssCommand::commandHandler()
{
  string sub_command=getString();

  if (sub_command.find("HVCSCDATA",0) != string::npos)
    {
      if (pthread_mutex_trylock(&CSCMonmutex) == 0)
        {
          // pthread_mutex_lock(&CSCMonmutex);
          // cscdata_updates_queue.push(sub_command);
	  lock_id = 11;

          string cscID=sub_command.substr(sub_command.find("|",0)+1,sub_command.size());
          map<string, HVcscMon >::iterator itr = cscMonData.find(cscID);
          if ( itr != cscMonData.end())
            {
              if (UpdateCSCMonData(itr->first, itr->second))
                {
                  itr->second.fast_mon_dim->updateService();
                  itr->second.stats.fast_mon_cnt++;
                  itr->second.slow_mon_dim->updateService();
                  itr->second.stats.slow_mon_cnt++;
                  itr->second.stats_dim->updateService();
                  LOG4CPLUS_DEBUG(hvmonlog, itr->first << " Mon data update requested");
                  // csc_mon_cnt_freq = 3;
                }
            }
          else
            {
              LOG4CPLUS_WARN(hvcmdlog, "Unable to find CSC with ID: " << cscID);
            }
          pthread_mutex_unlock(&CSCMonmutex);
        }
      return;
    }
  else if (sub_command.find("HVCSCCMD",0) != string::npos)
    {
      HVcsccmd csccmd;
      int pos[5], pos_prev=0;
      string set_host=sub_command.substr(0,pos_prev=sub_command.find("|",0));
      pos_prev=pos_prev+1;
      for (int i=0; i<5; i++)
        {
          pos[i]=sub_command.find(";",pos_prev);
          pos_prev=pos[i]+1;
        }
      for (int i=0; i<5; i++)
        {
          if (i==0) memcpy(&(csccmd.csc), (sub_command.substr(pos[i]+1, pos[i+1]-pos[i]-1)).c_str(), sizeof(csccmd.csc));
          if (i==1) csccmd.chan=atoi((sub_command.substr(pos[i]+1, pos[i+1]-pos[i]-1)).c_str());
          if (i==2) csccmd.cmd=(HV_CMD)atoi((sub_command.substr(pos[i]+1, pos[i+1]-pos[i]-1)).c_str());
          if (i==3) csccmd.data=atoi((sub_command.substr(pos[i]+1, pos[i+1]-pos[i]-1)).c_str());
          if (i==4) csccmd.size=atoi((sub_command.substr(pos[i]+1, pos[i+1]-pos[i]-1)).c_str());
        }

      // pthread_mutex_lock(&CSCMonmutex);
      lock_id=12;
      LOG4CPLUS_DEBUG (hvcmdlog, "PVSS HVCSCCMD: Command request from " << HVDimServer::getClientName()
                       << " CSC:" << csccmd.csc
                       << " chan:" << (int)csccmd.chan << " cmd:" << (int)csccmd.cmd << "(" << hvcmd_str[(int)csccmd.cmd]
                       << ") data:" << (int)csccmd.data << " size:" << (int)csccmd.size);
      csc_mon_cnt_freq = 3;
      processCSCHVCommand(&csccmd);
      // pthread_mutex_unlock(&CSCMonmutex);
      return;
    }

  if (sub_command.find("HVCMD",0) != string::npos || sub_command.find("HVDATA",0) != string::npos
      || sub_command.find("HVPRCMD",0) != string::npos || sub_command.find("HVPRDATA",0) != string::npos)
    {
      HVcmdDIM pvss_hvcmd;
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
                  pvss_hvcmd.data.card=card_database[i];
                  found=1;
                  break;
                }
            }
          if (!found)return;
        }
      /*
          else
            {
              found=0;
              for (int i=0; i<primary_database.size(); i++)
                {
                  if (set_host == host_database && pvss_hvcmd.data.card == primary_database[i] && pvss_hvcmd.data.module == primary_database_2[i])
                    {
                      pvss_hvcmd.data.card=primary_database[i];
                      found=1;
                      break;
                    }
                }
              if (!found)return;

            }
      */

      pthread_mutex_lock(&CSCMonmutex);
      lock_id=13;
      HVcmd& cmd = pvss_hvcmd.data;
      //----------------------------------------------
      if (sub_command.find("HVCMD",0) != string::npos)
        {
          LOG4CPLUS_DEBUG (hvcmdlog, "PVSS HVCMD: Command request from " << HVDimServer::getClientName()
                           << " card:" << (int)cmd.card << " module:" << (int)cmd.module
                           << " chan:" << (int)cmd.chan << " cmd:" << (int)cmd.cmd << "(" << hvcmd_str[(int)cmd.cmd]
                           << ") data:" << (int)cmd.data << " size:" << (int)cmd.size);

          pvss_cnt_freq = 0; // Reset PVSS update counter to delay DIM service update
          processRealHVCommand(& (pvss_hvcmd.data));
        } // if HVCMD
      else if (sub_command.find("HVDATA",0) != string::npos)
        {
          pvssInterfaceRealHVData( &(pvss_hvcmd.data));
        }
      else if (sub_command.find("HVPRCMD",0) != string::npos)
        {
          LOG4CPLUS_DEBUG (hvcmdlog, "PVSS HVPRCMD: Command request from " << HVDimServer::getClientName()
                           << " card:" << (int)cmd.card << " module:" << (int)cmd.module
                           << " chan:" << (int)cmd.chan << " cmd:" << (int)cmd.cmd << "(" << hvcmd_str[(int)cmd.cmd]
                           << ") data:" << (int)cmd.data << " size:" << (int)cmd.size);
          pvss_cnt_freq = 0; // Reset PVSS update counter to delay DIM service update
          csc_mon_cnt_freq = 3;
          processPrimaryHVCommand(& (pvss_hvcmd.data));

        }
      else if (sub_command.find("HVPRDATA",0) != string::npos)
        {
          LOG4CPLUS_DEBUG (hvcmdlog, "PVSS HVPRDATA: Data request from " << HVDimServer::getClientName()
                           << " card:" << (int)cmd.card << " module:" << (int)cmd.module
                           << " chan:" << (int)cmd.chan << " cmd:" << (int)cmd.cmd << "(" << hvcmd_str[(int)cmd.cmd]
                           << ") data:" << (int)cmd.data << " size:" << (int)cmd.size);
          // pvssInterfacePrimaryHVData( &(pvss_hvcmd.data));
          stringstream st;
          st << "HV_PR_" << set_host << "_" << cmd.card << "_" << cmd.module;
          string primaryID = st.str();
          // cout << primaryID << endl;
          map<string, HVprimaryMon >::iterator itr = primaryMonData.find(primaryID);
          if ( itr != primaryMonData.end())
            {
              if (UpdatePrimaryMonData(itr->first, itr->second))
                {
                  itr->second.primary_mon_dim->updateService();
                  itr->second.stats++;
                  itr->second.stats_dim->updateService();
                  LOG4CPLUS_DEBUG(hvmonlog, itr->first << " Primary PS Mon data update requested");
                  // csc_mon_cnt_freq = 3;
                }
            }
          else
            {
              LOG4CPLUS_WARN(hvcmdlog, "Unable to find Primary PS with ID: " << primaryID);
            }

        }

      pthread_mutex_unlock(&CSCMonmutex);
    }
};
