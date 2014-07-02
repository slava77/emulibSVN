#include <iostream>
#include <sstream>

#include "hvserver.h"

using namespace std;

typedef pair<int, HVPrimary*> PrimaryPS;
typedef map<int, HVPrimary*> PrimariesList;
typedef map<PrimaryPS, ModuleLinksList> HVSystemLinksTree;
typedef map<HVcmd, HVcmd> ModulesMap;

PrimariesList primariesList;
ModulesList mastersList;
ModulesList dbsList;
HVSystemLinksTree hvLinksTree;
ModulesMap dbs_to_masterMap;
ModulesMap master_to_primaryMap;
map<HVPrimary*,vector<HVcmd> > primaries_map;


bool operator<(const HVcmd& s1,const HVcmd& s2)
{
  return ((s1.card < s2.card)||((s1.card==s2.card)&&(s1.module<s2.module)));
}

bool operator==(const HVcmd& s1,const HVcmd& s2)
{
  return ((s1.card == s2.card) && (s1.module==s2.module));
}

bool operator>(const HVcmd& s1,const HVcmd& s2)
{
  return ((s1.card > s2.card)||((s1.card==s2.card)&&(s1.module>s2.module)));

}

/*
typedef pair<int, HVcmd> Module;
typedef map<int, HVcmd> ModulesList;
typedef map<Module, ModulesList> ModuleLinksList;
typedef map<Module, ModuleLinksList> ModuleLinksTree;
*/

string MODULE_STR(const HVcmd& mod)
{
  return MODULE_STR(mod.card, mod.module);
}

string MODULE_STR(int card, int addr)
{

  HVmodule* m = & (dataservices[card].data->module[addr]);
  stringstream modstr;
  modstr << modtype_str[m->type] << " ID#" << setw(3) << setfill('0') << m->ID << " at " << card << ":" << addr;
  return modstr.str();
}

string PRIMARY_STR(const HVPrimary* mod)
{
  stringstream modstr;
  modstr << "Primary PS at "<<  mod->port << ":" << mod->addr;
  return modstr.str();
}

bool isMaskedBoard(HVcmd& board)
{
  // if ((board.card == 6) && (board.module == 13)) return true;
  return false;
}

int DetectModules()
{
  LOG4CPLUS_INFO( hvlog, "Detecting available Modules");
  HVcmd cmd;
  cmd.card=ALL;
  cmd.module=ALL;
  cmd.cmd = LAST_CMD;
  cmd.chan = 0;
  cmd.data = 0xFFFFFFFF;
  cmd.size = 0;

  ReadHVData();

  primaries_map.clear();
  masters.clear();
  masters_map.clear();
  mastersList.clear();
  dbs.clear();
  dbs_map.clear();
  dbsList.clear();

  for (int i=0; i<dataservices.size(); i++)
    {
      cmd.card = i;
      for (int j=0; j<HVCARD_MAX_MODULES; j++)
        {
          cmd.module = j;
	  if (isMaskedBoard(cmd)) continue;
          HVmodule* m = &(dataservices[i].data->module[j]);
          if (m!=NULL && m->type!=NONE)
            {
              int ID = GetModuleID(cmd);
              if (m->type==MASTER)
                {
                  /* Master Type Module */
                  masters.push_back(cmd);
                  masters_map[ID] = cmd;
                  mastersList[ID] = cmd;
                }
              else
                {
                  /* RDB Type Module */
                  dbs.push_back(cmd);
                  dbs_map[ID] = cmd;
                  dbsList[ID] = cmd;
                }
              LOG4CPLUS_DEBUG( hvlog, MODULE_STR(cmd));
//          if (!LoadCalibration(i,j)) { LOG4CPLUS_WARN (hvlog, "Unable to load calibration data")};
            }
        }
    }
   LOG4CPLUS_INFO( hvlog, "Detected " << masters.size() << " Master modules");
   LOG4CPLUS_INFO( hvlog, "Detected " << dbs.size() << " RDB modules");

  return 0;

}

int LoadModulesCalibrations()
{
  for (int i=0; i<dataservices.size(); i++)
    {
      memset(dataservices[i].calib, 0, sizeof(dataservices[i].calib));
      for (int j=0; j<HVCARD_MAX_MODULES; j++)
        {
          HVmodule* m = &(dataservices[i].data->module[j]);
          if (m!=NULL && m->type!=NONE)
            {
              if (!LoadCalibration(i,j))
                {
                  LOG4CPLUS_WARN (hvlog, "Unable to load calibration data");
                };
            }
        }
    }
  return 0;

}

int AutoDetect()
{
  LOG4CPLUS_INFO (hvlog, "Start Auto Detection");

  srvStatus="AUTODETECTION";
  svc_srvStatus->updateService();

  int res=0;

  PrimaryControlUpdateTimer->start();

  for (map<int, map<int, HVPrimary*> >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr)
    {
      for (map<int, HVPrimary*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
        {
          HVPrimary* p = h_itr->second;

          int ID = (p->port<<8) + (p->addr&0xFF);
          primariesList[ID] = p;
        }
    }

  ResetAllPrimaries(false);


  // Reset Drivers data and force autodetect
  ResetDriver(); // disabled by AM - some modules are not detected in this mode. Has to be done during startup

  usleep(2*SEC);

  // === Create list of modules
  DetectModules();

  LoadModulesCalibrations();

  ResetAllModules(mastersList);

  ResetAllModules(dbsList);


  // === If fLoadConfig true, try to load configuration information from database.
  // === Force autodetection if failed to load from database or fLoadConfig is false
  if (!fLoadConfig || (fLoadConfig && (LoadConfig(cfgname)<0)))
    res = DetectMasterLinks();

  ShowPrimaryMap();
  // === Loose policy! Let continue if autodetection failed
  if (res<0) LOG4CPLUS_ERROR (hvlog, "Autodetection of Master<->RDB links is failed");

//  PrimaryControlUpdateTimer->pause();
  // usleep(2000000);
  ResetAllPrimaries(true);
  ReadHVData();

  SetDefaultParameters(mastersList);

  SetDefaultParameters(dbsList);



  LOG4CPLUS_INFO (hvlog, "Auto Detection is finished");
  srvStatus="READY";
  svc_srvStatus->updateService(srvStatus);
  PrimaryControlUpdateTimer->stop();
  // ResetAllPrimaries(true);

  return res;
}

int GetModuleID(const HVcmd& mod)
{
  return dataservices[mod.card].data->module[mod.module].ID;
}

int ResetDriver()
{
  HVcmd m;
  m.data = 0;
  m.module = 0;
  m.chan = 0;
  m.size = 0;
  m.cmd = HV_DETECT;
 //  for (int i=0; i<dataservices.size(); i++)
 int cardsFound = ioctl(fd, HVCARD_PCIE_IOQDEVSFOUND);
 if (cardsFound >0 ) {
   m.card = cardsFound;
   ioctl(fd, HVCARD_PCIE_IOXCMD, &m);
 }
 /*
 for (int i=0; i<cardsFound; i++)
    {
      m.card = i;
      ioctl(fd, HVCARD_PCIE_IOXCMD, &m);
    }
 */

  return 0;
}

int ReadHVData()
{
  for (int i=0; i<dataservices.size(); i++)
    GetHVData(fd,i,0,0, HV_GET_CARD_DATA, (ULONG*)dataservices[i].data,sizeof(struct HVhostcard));
  return 0;
}

/* Set module and all its channels to OFF state and set Vset to 0 */

int ResetModule(const HVcmd& mod)
{
  HVcmd m = mod;
  m.cmd = HV_SET_MOD_STATE;
  m.data = HV_STATE_OFF;
  ioctl(fd, HVCARD_PCIE_IOXCMD, &m);

  m.cmd = HV_SET_STATE;
  m.data = HV_STATE_OFF;
  m.chan = ALL;
  ioctl(fd, HVCARD_PCIE_IOXCMD, &m);

  int ramp_up=DEF_RAMP_UP;
  int ramp_down=DEF_RAMP_DOWN;
  int imax=DEF_MAX_CURRENT;
  int imax_ramp=DEF_MAX_CURRENT;

  // == Set specific values for Master modules
  if (dataservices[mod.card].data->module[mod.module].type == MASTER)
    {
      /** 
	Change default current trip level for Master channels to 20 counts and trip delay to 60 ms 
	to prevent burning of Master channels because of dead RDB channels
       **/
      imax = 20;
      imax_ramp = 20;
      int trip_delay = 60;
      m.cmd = HV_SET_TRIP_DELAY;
      m.data = trip_delay;
      m.chan = ALL;
      ioctl(fd, HVCARD_PCIE_IOXCMD, &m);	

      /*
      imax=1;
      imax_ramp=1;
      */
      
      ramp_up=DEF_RAMP_UP+1;
    }

  m.cmd = HV_SET_RAMP_UP;
  m.data = ramp_up;
  m.chan = ALL;
  ioctl(fd, HVCARD_PCIE_IOXCMD, &m);

  m.cmd = HV_SET_VSET;
  m.data = 0;
  m.chan = ALL;
  ioctl(fd, HVCARD_PCIE_IOXCMD, &m);

  m.cmd = HV_SET_IMAX_RAMP;
  m.data = imax_ramp;
  m.chan = ALL;
  ioctl(fd, HVCARD_PCIE_IOXCMD, &m);

  m.cmd = HV_SET_IMAX;
  m.data = imax;
  m.chan = ALL;
  ioctl(fd, HVCARD_PCIE_IOXCMD, &m);

  return 0;
}

int SetModule(HVcmd& mod, int vset=0, int chan=ALL)
{
  HVcmd rcmd = mod;
  rcmd.cmd = HV_SET_RAMP_UP;
  rcmd.data = vset;
  rcmd.chan = chan;
  ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);

  rcmd.cmd = HV_SET_MOD_STATE;
  rcmd.data = HV_STATE_OFF;
  ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);

  rcmd.cmd = HV_SET_STATE;
  rcmd.data = HV_STATE_OFF;
  rcmd.chan = chan;
  ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);

  rcmd.cmd = HV_SET_VSET;
  rcmd.data = vset;
  ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);

  rcmd.cmd = HV_SET_MOD_STATE;
  rcmd.data = HV_STATE_ON;
  ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);

  rcmd.cmd = HV_SET_STATE;
  rcmd.data = HV_STATE_ON;
  ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);

  return 0;
}



int ResetAllModules(ModulesList& mods)
{
  //  for_each(mods.begin(),mods.end(), ResetModule);
  for (ModulesList::const_iterator itr = mods.begin(); itr != mods.end(); ++itr)
    {
      ResetModule(itr->second);
    }
  return 0;
}


int ResetAllModules(vector<HVcmd>& mods)
{
  for_each(mods.begin(),mods.end(), ResetModule);
  return 0;
}


int ResetAllPrimaries(bool disableOutput=false)
{
  for (PrimariesList::const_iterator itr = primariesList.begin(); itr != primariesList.end(); ++itr)
    {
      HVPrimary* p = itr->second;
      // ResetPrimary(p->port, p->addr, 0, p->imax , false);
      ResetPrimary(p, 0, p->imax , disableOutput);
    }
  return 0;
}

int ResetPrimary(HVPrimary* p, int volt=0, float imax=PRIME_IMAX, bool disableOutput=true)
{
  pthread_mutex_lock(&PrimeIOmutex);
  int port = p->port;
  int addr = p->addr;
  unsigned control = p->control;
//   SendHVCmd(port, addr, "REN");
  ResetHV(port, addr);
  usleep(100000);
  SetHVMaxCurrent(port, addr, imax);
  p->imax = imax;
  SetHVMaxVoltage(port,addr, volt);
  p->vset = volt;
  p->rampup = PRIME_RAMPUP;
  if (disableOutput)
    {
      DisableHVOutput(port, addr);
      p->outset = 0;
      p->state = HV_STATE_OFF;
      p->status = HV_STATE_OFF;

      if (GetControlStatus( p->port, p->addr, & ( control ) ) < 0)
        {
          LOG4CPLUS_WARN(hvmonlog, "Error reading control status for " << PRIMARY_STR(p));
        }

    }
  else
    {
      EnableHVOutput(port, addr);
      p->outset = 1;
      p->state = HV_STATE_ON;
      p->status = HV_STATE_ON;
    }
  UpdatePrimaryStatus(p);
  pthread_mutex_unlock(&PrimeIOmutex);

}

int ResetPrimary(int port, int addr, int volt=0, float imax=PRIME_IMAX, bool disableOutput=true)
{
  pthread_mutex_lock(&PrimeIOmutex);
  ResetHV(port, addr);
  usleep(100000);
  SetHVMaxCurrent(port, addr, imax);
  SetHVMaxVoltage(port, addr, volt);
  if (disableOutput)
    {
      DisableHVOutput(port, addr);
    }
  else
    {
      EnableHVOutput(port, addr);
    }
  pthread_mutex_unlock(&PrimeIOmutex);
}



map<HVPrimary*,vector<HVcmd> > DetectPrimaryToMasterLinks()
{
  int vset = 100;


  LOG4CPLUS_INFO(hvlog, "Detecting Primary PSs to Masters Links");
  for (int i=0; i<masters.size(); i++)
    {
      SetModule(masters[i], vset+20);
    }

  usleep(2*SEC);


  map<HVPrimary*,vector<HVcmd> > primary_master_links;
  for (map<int, map<int, HVPrimary*> >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr)
    {
      for (map<int, HVPrimary*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
        {
//	  PrimaryControlUpdateTimer->stop();
          vector<HVcmd> m_links;
          HVPrimary* h = h_itr->second;
          LOG4CPLUS_DEBUG(hvlog, "Scanning " << PRIMARY_STR(h));
          h->port= itr->first;
          h->addr= h_itr->first;
          ResetPrimary(h->port, h->addr, vset+20, PRIME_IMAX, false);
          usleep(USLEEP_DELAY);
//	  usleep(SEC);
          pthread_mutex_lock(&PrimeIOmutex);
          ReadHVVoltage(h->port, h->addr, &(h->vmon));
          pthread_mutex_unlock(&PrimeIOmutex);
          int timeout = 0;
          m_links.clear();
          while ((h->vmon< vset) && (timeout<=5*SEC))
            {
              usleep(SEC);
              timeout +=SEC;
              // cout << h->vmon << endl;
              pthread_mutex_lock(&PrimeIOmutex);
              ReadHVVoltage(h->port, h->addr, &(h->vmon));
              pthread_mutex_unlock(&PrimeIOmutex);
            }
          if (timeout>5*SEC)
            {
              LOG4CPLUS_ERROR(hvlog, "Unable to set " << vset
                              << "V on " << PRIMARY_STR(h) << ". Skipping...");
              // continue;
            }

          ReadHVData();
          // m_links.clear();

          if (h->vmon>=vset)
            {
              m_links.clear();
              for (int i=0; i<masters.size(); i++)
                {
                  HVcmd rcmd = masters[i];
                  HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);
                  // === TODO: Check if not in bad channels list

                  if (r->status != HV_STATE_ON)
                    {
                      LOG4CPLUS_WARN(hvlog, MODULE_STR(rcmd)
                                     << " STATE is not ON, but " << hvstate_str[r->status] << ". Skipping...");
                      continue;
                    }
                  int chans_set = 0;
                  int bad_chans = 0;
                  for (int j=0; j<r->num_chans; j++)
                    {
                      if ((r->chan[j].vmon > vset/2) && (r->chan[j].status == HV_STATE_ON ))
                        chans_set++;
                      if (r->chan[j].status == HV_STATE_DEAD) bad_chans++;
                    }

                  if ( (chans_set>0)
                       && (chans_set<=(r->num_chans - bad_chans)) )
                    {
                      ModulesMap::iterator itr = master_to_primaryMap.find(rcmd);
                      if (itr != master_to_primaryMap.end())
                        {
                          LOG4CPLUS_ERROR(hvlog, MODULE_STR(rcmd)<< " to " << PRIMARY_STR(h) << " Multiple Links detected. "
                                          << "Already linked" );
                          exit(1);
                        }
                      rcmd.data = SetPrimaryLink(rcmd, h).data;
                      masters[i].data = rcmd.data;
                      m_links.push_back(rcmd);
                      LOG4CPLUS_INFO(hvlog, "Found link from " << PRIMARY_STR(h) << " to " << MODULE_STR(rcmd));

                    }
                }
            }

          pthread_mutex_lock(&PrimeIOmutex);
          SetHVMaxVoltage(h->port, h->addr, 0);
          DisableHVOutput(h->port, h->addr);
          pthread_mutex_unlock(&PrimeIOmutex);

          primary_master_links[h]=m_links;
//	  PrimaryControlUpdateTimer->start();
          usleep(2*SEC);
        }
    }

  return primary_master_links;
}

int DetectLinks()
{
  int res = 0;
  int vset = 120;
  string linkfname = "linkmap.txt";
  stringstream st;
  st << HV_DATADIR << "/" << linkfname;
  st >> linkfname;
  ofstream linkfile(linkfname.c_str());
  list<string> linksList;

//  PrimaryControlUpdateTimer->pause();
  map<HVPrimary*,vector<HVcmd> > primary_master_links =  DetectPrimaryToMasterLinks();
//  PrimaryControlUpdateTimer->start();
  LOG4CPLUS_INFO(hvlog, "Detecting Master to RDBs Links");

  map<HVcmd, vector<HVcmd> > master_db_links;
  ResetAllPrimaries();
  ResetAllModules(masters);
  usleep(2*SEC);
  int max_masters = 0;
  dbs_to_masterMap.clear();
  for ( map<HVPrimary*,vector<HVcmd> >::iterator itr = primary_master_links.begin();
        itr !=  primary_master_links.end(); ++itr)
    {

      ResetPrimary(itr->first->port, itr->first->addr, vset, PRIME_IMAX, false);

      // LOG4CPLUS_DEBUG(hvlog, "Scanning " << PRIMARY_STR(itr->first));
      usleep(2*SEC);
      max_masters = (max_masters<itr->second.size())?itr->second.size():max_masters;
      for (int i=0; i< itr->second.size(); i++)
        {
          HVcmd master = itr->second[i];
          // LOG4CPLUS_DEBUG(hvlog, "Scanning " << MODULE_STR(master));

          vector<HVcmd> d_links;
          d_links.clear();
          SetModule(master,vset);
          usleep(2*SEC);
          ReadHVData();
          int connected_dbs = 0;
          for (int k=0; k<dbs.size(); k++)
            {
              HVcmd dbcmd = dbs[k];
              HVmodule* db = &(dataservices[dbcmd.card].data->module[dbcmd.module]);
              //		LOG4CPLUS_DEBUG(hvlog, MODULE_STR(dbcmd) << " HVmon=" << db->hvmon);
		
              if (db->hvmon > 50)
                {
                  connected_dbs++;
                  d_links.push_back(dbcmd);

                  ModulesMap::iterator d_itr = dbs_to_masterMap.find(dbcmd);
                  if (d_itr == dbs_to_masterMap.end())
                    {
                      dbs_to_masterMap[dbcmd] = itr->second[i];
                      // dbs[k].data = itr->second[i].data;
                    }
                  else
                    {
                      HVcmd link = d_itr->second;
                      LOG4CPLUS_FATAL(hvlog, MODULE_STR(dbcmd)<< " to " << MODULE_STR(master) << " Multiple Links detected. "
                                      << "Already linked to " << MODULE_STR(link) << ". Exiting...");
                      exit(1);
                    }
                }
            }
          ResetAllModules(masters);
          usleep(2*SEC);
          if (connected_dbs>8)
            {
              LOG4CPLUS_FATAL(hvlog, "Found " << connected_dbs
                              << " RDBs linked to " << MODULE_STR(master) << " (Max=8)" << ". Exiting..." );
              for (int j=0; j< d_links.size(); j++)
                {
                  LOG4CPLUS_DEBUG(hvlog, MODULE_STR(d_links[j]));
                }
              exit(1);

            }
          else
            {
              LOG4CPLUS_INFO(hvlog, "Found " << connected_dbs
                             << " RDBs linked to " << MODULE_STR(master));
              for (int j=0; j< d_links.size(); j++)
                {
                  LOG4CPLUS_DEBUG(hvlog, MODULE_STR(d_links[j]));
                }
              master_db_links[master] = d_links;
            }
        }
//	ResetPrimary(itr->first->port, itr->first->addr);

    }

  ResetAllModules(masters);
// for (int k=0; k<max_masters; k++) {
  map<HVcmd, vector<HVcmd> > ms;
  ms.clear();
//	LOG4CPLUS_DEBUG(hvlog, "Master Channels Scan Pass " << k+1 << " of " << max_masters);
  for (map<HVPrimary*,vector<HVcmd> >::const_iterator itr = primary_master_links.begin();
       itr != primary_master_links.end(); ++itr)
    {
      primaries_map[itr->first] = itr->second;
      if (itr->second.size()>0)
        {
          for (int k=0; k<itr->second.size(); k++)
            {
              HVcmd m = itr->second[k];
              map<HVcmd, vector<HVcmd> >::iterator m_itr = master_db_links.find(m);
              if ( m_itr != master_db_links.end() && (m_itr->second.size()>0))
                {
                  ms[m] = m_itr->second;
                  for (vector<HVcmd>::iterator r_itr = ms[m].begin(); r_itr != ms[m].end(); ++r_itr)
                    {
                      primaries_map[itr->first].push_back(*r_itr);
                    }
                  /*
                  for (int j=0; j< m_itr->second.size(); j++) {
                    	      LOG4CPLUS_INFO(hvlog, j+1 << ": " << MODULE_STR(m_itr->second[j]));
                  }
                  */
                }
            }
        }
    }
  if (!ms.empty())
    {

      for (int ch=0; ch<8; ch++)
        {
          LOG4CPLUS_INFO(hvlog, "Scanning Masters Ch#" << ch+1 << "...");
          for ( map<HVcmd, vector<HVcmd> >::iterator m_itr = ms.begin(); m_itr != ms.end(); ++m_itr)
            {
//	    LOG4CPLUS_DEBUG(hvlog, "In scan " << MODULE_STR(m_itr->first));
              HVcmd m = m_itr->first;
              SetModule(m, vset, ch);
            }
          sleep(2);
          ReadHVData();
          int rdbs_found = 0;
          for (int j=0; j<dbs.size(); j++)
            {
              HVcmd mcmd = dbs[j];
              HVmodule* m = &(dataservices[mcmd.card].data->module[mcmd.module]);
              if (m->hvmon > 50)
                {
                  ModulesMap::iterator itr = dbs_to_masterMap.find(mcmd);
                  if (itr != dbs_to_masterMap.end())
                    {
                      HVcmd db = itr->first;
                      HVcmd master = itr->second;
                      SetMasterLink(db, master, ch);
                      HVmodule* h = &(dataservices[master.card].data->module[master.module]);
                      int primary =  h->master_busaddr + (h->master_hvcard<<16);
                      dbs[j].data = primary;
                      dbs_map[GetModuleID(db)].data = primary;
                      LOG4CPLUS_INFO(hvlog, MODULE_STR(db) << " linked to " << MODULE_STR(master) << " Ch#" << ch+1);
                      //if (linkfile)
                      stringstream entry;
                      entry << "Primary PS" << h->master_busaddr << ":" << h->master_hvcard
                      << "-> Link "<< MODULE_STR(master)
                      << " Chan:"<< (int)ch+1
                      << " to "<< MODULE_STR(db);

                      linksList.push_back(entry.str());
                    }
                }
            }
          ResetAllModules(masters);
          sleep(2);
        }
    }
  // }
  if (linkfile)
    {
      linksList.sort();
      list<string>::iterator i;
      for (i= linksList.begin(); i != linksList.end(); ++i)
        linkfile << *i << endl;
      linkfile.close();
    }


  return res;

}

HVcmd SetPrimaryLink(HVcmd& master, HVPrimary* h)
{
  HVcmd m = master;
  m.cmd = HV_SET_MASTER;
  m.data = ((h->port&0xF)<<4) + ((h->addr&0xFF)<<8)
           + ((dataservices[m.card].data->hostid&0xFFFF)<<16);
  ioctl(fd, HVCARD_PCIE_IOXCMD, &m);
  return m;
}

HVcmd SetMasterLink(HVcmd& rdb, HVcmd& master,int chan )
{
  HVcmd r = rdb;
  HVcmd m = master;
  r.cmd = HV_SET_MASTER;
  r.data = (chan&0xF) + ((m.module&0xF)<<4)
           + ((m.card&0xFF)<<8)  + ((dataservices[m.card].data->hostid&0xFFFF)<<16);
  ioctl(fd, HVCARD_PCIE_IOXCMD, &r);
  return r;
}

int DetectMasterLinks()
{
  if (fFastDetect)   return DetectLinks();

  int res=0;
  string linkfname = "linkmap.txt";
  stringstream st;
  st << HV_DATADIR << "/" << linkfname;
  st >> linkfname;
  ofstream linkfile(linkfname.c_str());

  for (map<int, map<int, HVPrimary*> >::iterator itr = primaries.begin(); itr != primaries.end(); ++itr)
    {
      for (map<int, HVPrimary*>::iterator h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
        {
          HVPrimary* h = h_itr->second;
          h->port= itr->first;
          h->addr= h_itr->first;
          pthread_mutex_lock(&PrimeIOmutex);
          ResetPrimary(h->port, h->addr, 120, PRIME_IMAX, false);
          pthread_mutex_unlock(&PrimeIOmutex);
          usleep(SEC);
          pthread_mutex_lock(&PrimeIOmutex);
          ReadHVVoltage(h->port, h->addr, &(h->vmon));
          pthread_mutex_unlock(&PrimeIOmutex);

          if (h->vmon>100)
            {
              for (int i=0; i<masters.size(); i++)
                {
                  HVcmd rcmd = masters[i];
                  /*  Check if Primary PS is already assigned,
                  then skip detection for this Master
                  */
                  if (((rcmd.data>>8)&0xFF) > 0) continue;
                  HVmodule* r = &(dataservices[rcmd.card].data->module[rcmd.module]);;
                  rcmd.cmd = HV_SET_RAMP_UP;
                  rcmd.data = 100;
                  rcmd.chan = ALL;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                  rcmd.cmd = HV_SET_MOD_STATE;
                  rcmd.data = HV_STATE_OFF;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                  rcmd.cmd = HV_SET_STATE;
                  rcmd.data = HV_STATE_OFF;
                  rcmd.chan = ALL;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                  rcmd.cmd = HV_SET_VSET;
                  rcmd.data = 0;
                  rcmd.chan = ALL;
                  ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                  int fPrimeLinked = 0;
                  for (int j=0; j<8; j++)
                    {
                      rcmd.cmd = HV_SET_VSET;
                      rcmd.data = 120;
                      rcmd.chan = j;
                      ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                      rcmd.cmd = HV_SET_MOD_STATE;
                      rcmd.data = HV_STATE_ON;
                      ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                      rcmd.cmd = HV_SET_STATE;
                      rcmd.data = HV_STATE_ON;
                      rcmd.chan = j;
                      ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                      sleep(2);
                      int fLinked = 0;
                      for (int k=0; k<dbs.size(); k++)
                        {
                          HVcmd mcmd = dbs[k];
                          int ID = dataservices[mcmd.card].data->module[mcmd.module].ID;
                          HVmodule* m = &(dataservices[mcmd.card].data->module[mcmd.module]);
                          GetHVData(fd,mcmd.card,0,0, HV_GET_CARD_DATA, (ULONG*)dataservices[mcmd.card].data,
                                    sizeof(struct HVhostcard));
                          if (dataservices[mcmd.card].data->module[mcmd.module].hvmon > 50)
                            {
                              fPrimeLinked ++;

                              mcmd.cmd = HV_SET_MASTER;
                              mcmd.data = (rcmd.chan&0xF) + ((rcmd.module&0xF)<<4)
                                          + ((rcmd.card&0xFF)<<8)  + ((dataservices[rcmd.card].data->hostid&0xFFFF)<<16);
                              LOG4CPLUS_INFO (hvlog, "Primary PS" << h->port << ":" << h->addr
                                              << "-> Link "<< modtype_str[r->type]
                                              << " ID#"<< r->ID << " Chan:"<< (int)rcmd.chan+1
                                              << " to "<< modtype_str[m->type] << " ID#"<< m->ID);
                              if (linkfile)
                                linkfile << "Primary PS" << h->port << ":" << h->addr
                                << "-> Link "<< modtype_str[r->type]
                                << " ID#"<< r->ID << " Chan:"<< (int)rcmd.chan+1
                                << " to "<< modtype_str[m->type] << " ID#"<< m->ID << endl;
                              ioctl(fd, HVCARD_PCIE_IOXCMD, &mcmd);
                              // == Assign Primary PS for RDB and Master
                              dbs[k].data = h->port;
                              dbs[k].data |= h->addr<<16;
                              dbs_map[ID].data = h->port + (h->addr<<16);
                              // ++ Check ++
                              /*
                                usleep(500);
                                rcmd.cmd = HV_SET_MASTER;
                                rcmd.data = ((h->port&0xF)<<4) + ((h->addr&0xFF)<<8)
                                + ((dataservices[rcmd.card].data->hostid&0xFFFF)<<16);
                                ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                              */
                              // +++++++++++
                              fLinked++;
                            }
                        }
                      if (!fLinked)
                        LOG4CPLUS_INFO (hvlog, "Primary PS" << h->port << ":" << h->addr
                                        << "-> Link "<< modtype_str[r->type] << " ID#"<< r->ID
                                        << " Chan:"<< (int)rcmd.chan+1 << " to NONE");
                      if (fLinked >1)
                        LOG4CPLUS_ERROR (hvlog, "ERROR: Primary PS" << h->port << ":" << h->addr
                                         << "-> Link "<< modtype_str[r->type] << " ID#"<< r->ID
                                         << " Chan:"<< (int)rcmd.chan+1 << " Multiple RDBs detected");
                      rcmd.cmd = HV_SET_MOD_STATE;
                      rcmd.data = HV_STATE_OFF;
                      ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                      rcmd.cmd = HV_SET_STATE;
                      rcmd.data = HV_STATE_OFF;
                      rcmd.chan = j;
                      ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                      rcmd.cmd = HV_SET_RAMP_UP;
                      rcmd.data = 10;
                      rcmd.chan = j;
                      ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                      rcmd.cmd = HV_SET_VSET;
                      rcmd.data = 0;
                      rcmd.chan = j;
                      ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                      sleep(2);
                    }
                  if (fPrimeLinked)
                    {
                      rcmd.cmd = HV_SET_MASTER;
                      rcmd.data = ((h->port&0xF)<<4) + ((h->addr&0xFF)<<8)
                                  + ((dataservices[rcmd.card].data->hostid&0xFFFF)<<16);
                      masters[i].data = rcmd.data;
                      ioctl(fd, HVCARD_PCIE_IOXCMD, &rcmd);
                      usleep(UPDATE_DELAY*4);
                    }
                }
            }
          else
            {
              LOG4CPLUS_ERROR (hvlog, "No output voltage on Primary PS"<< (int)h->port << ":" <<  (int)h->addr);
              res = -1;
            }
          pthread_mutex_lock(&PrimeIOmutex);
          SetHVMaxVoltage(h->port, h->addr, 0);
          DisableHVOutput(h->port, h->addr);
          pthread_mutex_unlock(&PrimeIOmutex);

        }
    }
  //        LOG4CPLUS_INFO (hvlog, "Auto Detection is finished");
  if (linkfile) linkfile.close();
  return res;
}

void ShowPrimaryMap()
{
  for ( map<HVPrimary*,vector<HVcmd> >::iterator itr = primaries_map.begin();
        itr !=  primaries_map.end(); ++itr)
    {
      LOG4CPLUS_DEBUG(hvlog, PRIMARY_STR(itr->first) << " Linked to ");
      for (vector<HVcmd>::iterator r_itr = itr->second.begin(); r_itr != itr->second.end(); ++r_itr)
        {
          LOG4CPLUS_DEBUG(hvlog, MODULE_STR(*r_itr));
        }
    }

}

