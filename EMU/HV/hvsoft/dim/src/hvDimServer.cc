#include <iostream>
#include "hvserver.h"
#include "Wiener_PL512.h"


extern std::vector<Wiener_PL512 *> WienerPSs;

HVDimServer::HVDimServer(string servername): DimServer()
{
  sname = servername;

  string DNSNode = DimServer::getDnsNode();
  int DNSPort = DimServer::getDnsPort();
  setDnsNode(DNSNode.c_str(), DNSPort);
};

void HVDimServer::commandHandler()
{
  string client = ":"+string(getClientName());
  DimCommand *currCmd = getCommand();
  string cmdname = currCmd->getName();
  string cmdvalue = currCmd->getString();
  LOG4CPLUS_DEBUG(hvcmdlog, client << ":RCV_CMD:" << cmdname
                  << " " << cmdvalue);

  if ((cmdname.find("SERVER_CONTROL") != string::npos))
    {

      std::string cmdptrn = "SAVE_CONFIG=";
      std::string value=DEF_CONFIG_NAME;
      if (cmdvalue.find(cmdptrn) != string::npos)
        {
          value = cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length());
          if (value.length() == 0) value=DEF_CONFIG_NAME;
          SaveConfig(value);
        }
      cmdptrn = "LOAD_CONFIG=";
      if (cmdvalue.find(cmdptrn) != string::npos)
        {
          value = cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length());
          if (value.length() == 0) value=DEF_CONFIG_NAME;
          LoadConfig(value);
        }

      cmdptrn = "SAVE_REAL_CONFIG=";
      value=DEF_CONFIG_NAME;
      if (cmdvalue.find(cmdptrn) != string::npos)
        {
          value = cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length());
          if (value.length() == 0) value=DEF_CONFIG_NAME;
          SaveRealConfig(value);
        }

      cmdptrn = "LOAD_REAL_CONFIG=";
      if (cmdvalue.find(cmdptrn) != string::npos)
        {
          value = cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length());
          if (value.length() == 0) value=DEF_CONFIG_NAME;
          LoadRealConfig(value);
        }

      cmdptrn = "PVSS_UPDATE_INTERVAL=";
      if (cmdvalue.find(cmdptrn) != string::npos)
        {
          value = cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length());
          errno = 0;
          unsigned long val = strtoul(value.c_str(), NULL, 10);
          if ( (errno == 0) && (val > 5) && (val < 3600) )
            {
              PVSS_UPDATE_INTERVAL = val;
              LOG4CPLUS_DEBUG(hvlog, "Set " << cmdptrn << val);
            }
          else
            {
              LOG4CPLUS_WARN(hvlog, "Wrong value for " << cmdptrn << value);
            }
        }

      cmdptrn = "PVSS_IMON_THRESHOLD=";
      if (cmdvalue.find(cmdptrn) != string::npos)
        {
          value = cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length());
          errno = 0;
          unsigned long val = strtoul(value.c_str(), NULL, 10);
          if ( (errno == 0) && (val > 1) && (val < 1100) )
            {
              PVSS_IMON_THRESHOLD = val;
              LOG4CPLUS_DEBUG(hvlog, "Set " << cmdptrn << val);
            }
          else
            {
              LOG4CPLUS_WARN(hvlog, "Wrong value for " << cmdptrn << value);
            }
        }

      cmdptrn = "PVSS_VMON_THRESHOLD=";
      if (cmdvalue.find(cmdptrn) != string::npos)
        {
          value = cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length());
          errno = 0;
          unsigned long val = strtoul(value.c_str(), NULL, 10);
          if ( (errno == 0) && (val >= 4 ) && (val < 3600) )
            {
              PVSS_VMON_THRESHOLD = val;
              LOG4CPLUS_DEBUG(hvlog, "Set " << cmdptrn << val);
            }
          else
            {
              LOG4CPLUS_WARN(hvlog, "Wrong value for " << cmdptrn << value);
            }
        }

    }

  if (fControlWienerPL512)
    {

      if (cmdname.find("WIENER") != string::npos)
        {
          commands_queue.push(make_pair(cmdname,cmdvalue));
        }
//!-- Wiener Commands handling moved to separate thread

      /*
            // NOTE: Only one Wiener PS (unit) is supported
            int unit=0;
            Wiener_PL512* PS = WienerPSs[unit];

            string cmdptrn = "";
            // Handle Wiener System commands
            if ((cmdname.find("WIENER") != string::npos)
                && (cmdname.rfind("SYSCMD") != string::npos))
              {
                if (cmdvalue.find("SET_MAIN_ON") != string::npos)
                  {
                    PS->SetMainSwitch(1);
                  }
                if (cmdvalue.find("SET_MAIN_OFF") != string::npos)
                  {
                    PS->SetMainSwitch(0);
                  }
                if (cmdvalue.find("SET_ALL_CHANS_ON") != string::npos)
                  {
       	      int nchans = PS->GetNumberOfChannels();
                    if ((nchans >= 1) && (nchans <= 12)) {
                    	for (int ch=1; ch <= nchans; ch++)
                        PS->SetChannelOutputSwitch(ch, 1);
                    }
                  }
                if (cmdvalue.find("SET_ALL_CHANS_OFF") != string::npos)
                  {
      	      int nchans = PS->GetNumberOfChannels();
      	      if ((nchans >= 1) && (nchans <= 12)) {
                    	for (int ch=1; ch <= nchans; ch++)
                        PS->SetChannelOutputSwitch(ch, 0);
      	      }
                  }
                cmdptrn="SET_GROUP_SWITCH_ON=";
                if (cmdvalue.find(cmdptrn) != string::npos)
                  {
                    int group = atoi ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                    if (group>=0) PS->SetGroupSwitch(group, 1);
                  }
                cmdptrn="SET_GROUP_SWITCH_OFF=";
                if (cmdvalue.find(cmdptrn) != string::npos)
                  {
                    int group = atoi ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                    if (group>=0) PS->SetGroupSwitch(group, 0);
                  }

                if (cmdvalue.find("GET_SYS_DATA") != string::npos)
                  {
                    wiener.sys.psOperatingTime = PS->GetOperatingTime();
                    wiener.sys.sysUpTime = ((float)(PS->GetUpTime()))/100;
                    wiener.sys.sysStatus = PS->GetSystemStatus();
                    decodeWienerSysStatus(wiener.sys.sysStatus, &(wiener.sys.decoded_status));
                    wiener.sys.sysMainSwitch = PS->GetMainSwitchStatus();
                    wiener_services["SYSDATA"]->updateService();
                  }


                if (cmdvalue.find("GET_DATA") != string::npos)
                  {
                    wiener.sys.psOperatingTime = PS->GetOperatingTime();
                    wiener.sys.sysUpTime = ((float)(PS->GetUpTime()))/100;
                    wiener.sys.sysStatus = PS->GetSystemStatus();
                    decodeWienerSysStatus(wiener.sys.sysStatus, &(wiener.sys.decoded_status));
                    wiener.sys.sysMainSwitch = PS->GetMainSwitchStatus();
                    wiener_services["SYSDATA"]->updateService();
                    for (int ch=0; ch<wiener.chans.size(); ch++)
                      {
                        ostringstream ssch;
                        ssch << "CHAN" << setw(2) << setfill('0') <<ch;
                        wiener.chans[ch].status = PS->GetChannelStatus(ch+1);
                        decodeWienerChannelStatus(wiener.chans[ch].status, &(wiener.chans[ch].decoded_status));
                        // wiener.chans[ch].sup_behaviour = PS->GetChannelSupervisionBehavior(ch+1);
                        wiener.chans[ch].meas_volt = PS->GetChannelMeasuredVoltage(ch+1);
                        wiener.chans[ch].meas_cur = PS->GetChannelMeasuredCurrent(ch+1);
                        wiener.chans[ch].meas_term_volt = PS->GetChannelMeasuredTerminalVoltage(ch+1);
                        wiener.chans[ch].out_switch = PS->GetChannelOutputSwitch(ch+1);
                        wiener.chans[ch].out_volt = PS->GetChannelOutputVoltage(ch+1);
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
                        wiener_services[ssch.str()]->updateService();
                      }

                  }

              }

            // Handle Wiener Channels related commands
            float value = 0.0;
            if ((cmdname.find("WIENER") != string::npos)
                && (cmdname.rfind("CHAN") != string::npos))
              {
                int ch = atoi ((cmdname.substr(cmdname.rfind("CHAN")+4,cmdname.length())).c_str());
                if (cmdvalue.find("GET_DATA") != string::npos && ch>=0)
                  {
                    ostringstream ssch;
                    ssch << "CHAN" << setw(2) << setfill('0') <<ch;
                    memcpy(wiener.chans[ch].name, const_cast<char *>(PS->GetChannelName(ch+1)), sizeof(wiener.chans[ch].name)) ;
                    wiener.chans[ch].status = PS->GetChannelStatus(ch+1);
                    decodeWienerChannelStatus(wiener.chans[ch].status, &(wiener.chans[ch].decoded_status));
                    // wiener.chans[ch].sup_behaviour = PS->GetChannelSupervisionBehavior(ch+1);
                    wiener.chans[ch].meas_volt = PS->GetChannelMeasuredVoltage(ch+1);
                    wiener.chans[ch].meas_cur = PS->GetChannelMeasuredCurrent(ch+1);
                    wiener.chans[ch].meas_term_volt = PS->GetChannelMeasuredTerminalVoltage(ch+1);
                    wiener.chans[ch].out_switch = PS->GetChannelOutputSwitch(ch+1);
                    wiener.chans[ch].out_volt = PS->GetChannelOutputVoltage(ch+1);
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
                    wiener_services[ssch.str()]->updateService();

                  }
                ch++;
                cmdptrn = "SET_CHAN_ON";
                if (cmdvalue.find("SET_CHAN_ON") != string::npos)
                  {
                    PS->SetChannelOutputSwitch(ch, 1);
                  }
                if (cmdvalue.find("SET_CHAN_OFF") != string::npos)
                  {
                    PS->SetChannelOutputSwitch(ch, 0);
                  }

                cmdptrn = "SET_CHAN_OUTPUT_VOLT=";
                if (cmdvalue.find(cmdptrn) != string::npos)
                  {
                    value = atof ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                    PS->SetChannelOutputVoltage(ch, value);
                  }
                cmdptrn = "SET_CHAN_CURRENT_LIMIT=";
                if (cmdvalue.find(cmdptrn) != string::npos)
                  {
                    value = atof ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                    PS->SetChannelCurrentLimit(ch, value);
                  }
                cmdptrn = "SET_CHAN_MIN_SENSE_VOLT=";
                if (cmdvalue.find(cmdptrn) != string::npos)
                  {
                    value = atof ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                    PS->SetChannelMinimumSenseVoltage(ch, value);
                  }
                cmdptrn = "SET_CHAN_MAX_SENSE_VOLT=";
                if (cmdvalue.find(cmdptrn) != string::npos)
                  {
                    value = atof ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                    PS->SetChannelMaximumSenseVoltage(ch, value);
                  }
                cmdptrn = "SET_CHAN_MAX_TERMINAL_VOLT=";
                if (cmdvalue.find(cmdptrn) != string::npos)
                  {
                    value = atof ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                    PS->SetChannelMaximumTerminalVoltage(ch, value);
                  }
                cmdptrn = "SET_CHAN_MAX_POWER=";
                if (cmdvalue.find(cmdptrn) != string::npos)
                  {
                    value = atof ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                    PS->SetChannelMaximumPower(ch, value);
                  }
                cmdptrn = "SET_CHAN_MAX_CURRENT=";
                if (cmdvalue.find(cmdptrn) != string::npos)
                  {
                    value = atof ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                    PS->SetChannelMaximumCurrent(ch, value);
                  }
                cmdptrn = "SET_CHAN_GROUP=";
                if (cmdvalue.find(cmdptrn) != string::npos)
                  {
                    int ivalue = atoi ((cmdvalue.substr(cmdvalue.rfind(cmdptrn)+cmdptrn.length(),cmdvalue.length())).c_str());
                    PS->SetChannelGroup(ch, ivalue);
                  }
              }
      	*/
    }

};

void dimHVErrHandler::errorHandler(int severity, int code, char *msg)
{
  LOG4CPLUS_DEBUG (hvlog, msg);
  string DNSNode = DimServer::getDnsNode();
  int DNSPort = DimServer::getDnsPort();
  switch (code)
    {
    case DIMDNSCNERR:
      LOG4CPLUS_ERROR (hvlog, "DNS at " << DNSNode << ":" <<DNSPort << " is not responding");
      /*
      if ((DNSPort != BACKUP_DNS_PORT) && (DNSNode != BACKUP_DNS_NODE))
        {
          LOG4CPLUS_DEBUG (hvlog, "Switching to backup DNS " << BACKUP_DNS_NODE << ":" << BACKUP_DNS_PORT);
          DimServer::setDnsNode(BACKUP_DNS_NODE, BACKUP_DNS_PORT);
        }
      else
      {
      LOG4CPLUS_DEBUG (hvlog, "Switching to primary DNS " << PRIMARY_DNS_NODE << ":" << PRIMARY_DNS_PORT);
          DimServer::setDnsNode(PRIMARY_DNS_NODE.c_str(), PRIMARY_DNS_PORT);
      }
      */
      break;
    // default:
	//LOG4CPLUS_ERROR (hvlog, "DIM Error detected: " << code << " severity(" << severity << ") " << msg );
    }
//  usleep(SEC);
};

