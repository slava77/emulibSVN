/********************************************************************
 * Wiener PL512 SNMP interface                                      *
 * Created on 06 June 2006                                          *
 * Iouri Sedykh   |  Iouri.Sedykh@cern.ch                           *
 ********************************************************************/

#ifndef WIENER_PL512_H
#define WIENER_PL512_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/session_api.h>
#include <net-snmp/library/mib.h>

class SNMP_Init_Class
{
public:
  SNMP_Init_Class()
  {
    init_snmp("snmpapp");
  }
};

static SNMP_Init_Class SNMP_Init;

static char *Time_Stamp();

class Wiener_PL512
{
  struct Dictionary
  {
    struct Command
    {
      const char *Name;
      const char *Alias;
      const char *IndexAlias;
      oid *OID;
      size_t OID_Length;
    };

    Command *Cmd;
    int *Index;
    int CmdNumber;

    Dictionary() : Cmd(NULL), Index(NULL)
    {
      static const char *cmd[] =
      {
        "outputNumber.0", "NumberOfChannels", ""
        , "groupsNumber.0","NumberOfGroups", ""
        , "groupsSwitch.1", "GroupSwitch", "NumberOfGroups"
        , "sysUpTime.0", "UpTime", ""
        , "sysDescr.0", "SystemDescription", ""
        , "sysMainSwitch.0", "MainSwitch", ""
        , "sysStatus.0", "SystemStatus", ""
        , "outputIndex.1", "ChannelIndex", "NumberOfChannels"
        , "outputName.1", "ChannelName", "NumberOfChannels"
        , "outputGroup.1", "ChannelGroup", "NumberOfChannels"
        , "outputStatus.1", "ChannelStatus", "NumberOfChannels"
        , "outputMeasurementSenseVoltage.1", "ChannelMeasuredVoltage",
        "NumberOfChannels"
        , "outputMeasurementTerminalVoltage.1", "ChannelMeasuredTerminalVoltage",
        "NumberOfChannels"
        , "outputMeasurementCurrent.1", "ChannelMeasuredCurrent",
        "NumberOfChannels"
        , "outputMeasurementTemperature.1", "ChannelTemperatureStatus",
        "NumberOfChannels"
        , "outputSwitch.1", "ChannelOutputSwitch", "NumberOfChannels"
        , "outputVoltage.1", "ChannelOutputVoltage", "NumberOfChannels"
        //, "outputAdjustVoltage.1", "ChannelAdjustVoltage", "NumberOfChannels"
        , "outputCurrent.1", "ChannelCurrentLimit", "NumberOfChannels"
        , "outputSupervisionBehavior.1", "ChannelSupervisionBehavior",
        "NumberOfChannels"
        , "outputSupervisionMinSenseVoltage.1", "ChannelMinimumSenseVoltage",
        "NumberOfChannels"
        , "outputSupervisionMaxSenseVoltage.1", "ChannelMaximumSenseVoltage",
        "NumberOfChannels"
        , "outputSupervisionMaxTerminalVoltage.1",
        "ChannelMaxTerminalVoltage", "NumberOfChannels"
        , "outputSupervisionMaxCurrent.1", "ChannelMaxCurrent",
        "NumberOfChannels"
        , "outputConfigMaxSenseVoltage.1", "ChannelConfigMaxSenseVoltage",
        "NumberOfChannels"
        , "outputConfigMaxTerminalVoltage.1",
        "ChannelConfigMaxTerminalVoltage", "NumberOfChannels"
        , "outputConfigMaxCurrent.1", "ChannelConfigMaxCurrent",
        "NumberOfChannels"
        , "psSerialNumber.0", "SerialNumber", ""
        , "psOperatingTime.0", "OperatingTime", ""
        , "fanSerialNumber.0", "fanSerialNumber", ""
        , "outputSupervisionMaxPower.1", "ChannelMaxPower",
        "NumberOfChannels"
      };

      CmdNumber = sizeof(cmd) / sizeof(cmd[0]) / 3;
      Cmd = new Command[CmdNumber];
      Index = new int[CmdNumber];

      oid OID[MAX_OID_LEN];

      for (int i=CmdNumber-1; i>=0; i--)
        {
          Cmd[i].Name = cmd[i * 3];
          Cmd[i].Alias = cmd[i * 3 + 1];
          Cmd[i].IndexAlias = cmd[i * 3 + 2];
          Index[i] = i;

          Cmd[i].OID_Length = MAX_OID_LEN;
          if (!snmp_parse_oid(Cmd[i].Name, OID, &Cmd[i].OID_Length))
            {
              snmp_perror(Cmd[i].Name);

              printf("%sCannot find the parameter \"%s\" in the dictionary!\n",
                     Time_Stamp(), Cmd[i].Name);

              exit(1);
            }
          else
            {
              Cmd[i].OID = new oid[Cmd[i].OID_Length];
              for (int j=Cmd[i].OID_Length-1; j>=0; j--) Cmd[i].OID[j] = OID[j];

              if (0)
                {
                  printf ("%sOID( \"%s\" ) (%d of %d) =", Time_Stamp(),
                          Cmd[i].Name, Cmd[i].OID_Length, MAX_OID_LEN);
                  for (int j=0; j<Cmd[i].OID_Length; j++)
                    printf(" %d", Cmd[i].OID[j]);
                  printf("\n");

                  fflush(stdout);
                }
            }
        }

      for (int i=CmdNumber-1; i>0; i--) for (int j=0; j<i; j++)
          if (strcmp(Cmd[Index[j]].Alias, Cmd[Index[i]].Alias) > 0)
            {
              int k = Index[i];
              Index[i] = Index[j];
              Index[j] = k;
            }
    }

    ~Dictionary()
    {
      if ( ! Cmd)
        {
          for (int i=CmdNumber-1; i>=0; i--) if (! Cmd[i].OID)
              {
                delete [] Cmd[i].OID;

                Cmd[i].OID = NULL;
              }

          delete [] Cmd;

          Cmd = NULL;
        }

      if ( ! Index)
        {
          delete [] Index;

          Index = NULL;
        }
    }

    int GetIndex(const char *name)
    {
      int i = -1;
      int j = CmdNumber;
      while (j - i > 1)
        {
          int k = (i + j) / 2;

          int m = strcmp(name, Cmd[Index[k]].Alias);
          if (m == 0) return Index[k];
          if (m < 0) j = k;
          else i = k;
        }

      return -1;
    }
  } static dict;

  int Initialized;
  char *IP;
  struct ValueStructure
  {
    char **Value;
    int N;

    ValueStructure() : Value(NULL), N(-1) {};

    ~ValueStructure()
    {
      DeleteValue();
    }

    inline void DeleteValue()
    {
      if (Value)
        {
          if (N <= 0 && Value[0])
            {
              delete [] Value[0];

              Value[0] = NULL;
            }
          else for (int i=N-1; i>=0; i--) if (Value[i])
                {
                  delete [] Value[i];

                  Value[i] = NULL;
                }

          delete [] Value;

          Value = NULL;
        }

      N = -1;
    }
  };

  ValueStructure *Value;
  time_t ValueTime;
  static char OutputString[1024];
  int Retries, Timeout;

  void DeleteAll()
  {
    if (IP)
      {
        delete [] IP;

        IP = NULL;
      }

    if (Value)
      {
        delete [] Value;

        Value = NULL;
      }

    Initialized = 0;
  }

  int GetInteger(const char *PName) const
  {
    int ind = dict.GetIndex(PName);

    if (ind < 0)
      {
        printf("%sName \"%s\" does not exist in the dictionary!\n",
               Time_Stamp(), PName);

        exit(2);
      }

    if (Initialized == 0 || Value[ind].Value == NULL ||
        Value[ind].Value[0] == NULL)
      {
        printf("%sValue \"%s\" is not set!\n", Time_Stamp(), PName);

        fflush(stdout);

        return -1;
      }

    const char *pnt = strstr(Value[ind].Value[0], "INTEGER:");
    if (pnt == NULL)
      {
        printf("%sCannot parse the value for parameter \"%s\":\n", Time_Stamp(),
               PName);
        printf("  \"%s\"\n", Value[ind].Value[0]);

        exit(3);
      }

    int val;
    if (sscanf(pnt, "INTEGER: %d", &val) <= 0)
      {
        printf("%sCannot parse the value for parameter \"%s\":\n", Time_Stamp(),
               PName);
        printf("  \"%s\" from the string \"%s\"\n", Value[ind].Value[0], pnt);

        exit(4);
      }

    return val;
  }

  int GetTimeticks(const char *PName) const
  {
    int ind = dict.GetIndex(PName);

    if (ind < 0)
      {
        printf("%sName \"%s\" does not exist in the dictionary!\n",
               Time_Stamp(), PName);

        exit(5);
      }

    if (Initialized == 0 || Value[ind].Value == NULL ||
        Value[ind].Value[0] == NULL)
      {
        printf("%sValue \"%s\" is not set!\n", Time_Stamp(), PName);

        fflush(stdout);

        return -1;
      }

    const char *pnt = strstr(Value[ind].Value[0], "Timeticks:");
    if (pnt == NULL)
      {
        printf("%sCannot parse the value for parameter \"%s\":\n", Time_Stamp(),
               PName);
        printf("  \"%s\"\n", Value[ind].Value[0]);

        exit(6);
      }

    int val;
    if (sscanf(pnt, "Timeticks: (%d", &val) <= 0)
      {
        printf("%sCannot parse the value for parameter \"%s\":\n", Time_Stamp(),
               PName);
        printf("  \"%s\" from the string \"%s\"\n", Value[ind].Value[0], pnt);

        exit(7);
      }

    return val;
  }

  const char *GetString(const char *PName) const
  {
    int ind = dict.GetIndex(PName);

    if (ind < 0)
      {
        printf("%sName \"%s\" does not exist in the dictionary!\n",
               Time_Stamp(), PName);

        exit(8);
      }

    if (Initialized == 0 || Value[ind].Value == NULL ||
        Value[ind].Value[0] == NULL)
      {
        printf("%sValue \"%s\" is not set!\n", Time_Stamp(), PName);

        fflush(stdout);

        OutputString[0] = '\x0';
        return OutputString;
      }

    const char *pnt = strstr(Value[ind].Value[0], "STRING:");
    if (pnt == NULL)
      {
        printf("%sCannot parse the value for parameter \"%s\":\n", Time_Stamp(),
               PName);
        printf("  \"%s\"\n", Value[ind].Value[0]);

        exit(9);
      }

    strncpy(OutputString, pnt + 8, sizeof(OutputString));
    OutputString[sizeof(OutputString) - 1] = '\x0';

    return OutputString;
  }

  int GetEnum(const char *PName, char *OutValue = NULL) const
  {
    int ind = dict.GetIndex(PName);

    if (ind < 0)
      {
        printf("%sName \"%s\" does not exist in the dictionary!\n",
               Time_Stamp(), PName);

        exit(10);
      }

    if (Initialized == 0 || Value[ind].Value == NULL ||
        Value[ind].Value[0] == NULL)
      {
        printf("%sValue \"%s\" is not set!\n", Time_Stamp(), PName);

        fflush(stdout);

        return -1;
      }

    const char *pnt = strstr(Value[ind].Value[0], "INTEGER:");
    if (pnt == NULL)
      {
        printf("%sCannot parse the value for parameter \"%s\":\n", Time_Stamp(),
               PName);
        printf("  \"%s\"\n", Value[ind].Value[0]);

        exit(11);
      }

    int val;
    char StrVal[1024];
    if (sscanf(pnt, "INTEGER: %[0-9a-zA-Z](%d", StrVal, &val) < 2)
      {
        StrVal[0] = '\x0';

        if (sscanf(pnt, "INTEGER: %d", &val) <= 0)
          {
            printf("%sCannot parse the value for parameter \"%s\":\n",
                   Time_Stamp(), PName);
            printf("  \"%s\" from the string \"%s\"\n", Value[ind].Value[0], pnt);

            exit(12);
          }
      }

    StrVal[sizeof(StrVal) - 1] = '\x0';
    while (strlen(StrVal) > 0 &&
           strspn(StrVal + strlen(StrVal) - 1, " \t\n\r") > 0)
      StrVal[strlen(StrVal) - 1] = '\x0';
    if (OutValue) strcpy(OutValue, StrVal);

    return val;
  }

  int GetBits(const char *PName, char *OutValue = NULL) const
  {
    int ind = dict.GetIndex(PName);

    if (ind < 0)
      {
        printf("%sName \"%s\" does not exist in the dictionary!\n",
               Time_Stamp(), PName);

        exit(10);
      }

    if (Initialized == 0 || Value[ind].Value == NULL ||
        Value[ind].Value[0] == NULL)
      {
        printf("%sValue \"%s\" is not set!\n", Time_Stamp(), PName);

        fflush(stdout);

        return -1;
      }

    const char *pnt = strstr(Value[ind].Value[0], "BITS:");
    if (pnt == NULL)
      {
        printf("%sCannot parse the value for parameter \"%s\":\n", Time_Stamp(),
               PName);
        printf("  \"%s\"\n", Value[ind].Value[0]);

        exit(11);
      }

    int val;
    char StrVal[1024];
    int tr = sscanf(pnt, "BITS: %x %[ 0-9a-zA-Z()]", &val, StrVal);
    if (tr <= 0)
      {
        printf("%sCannot parse the value for parameter \"%s\":\n",
               Time_Stamp(), PName);
        printf("  \"%s\" from the string \"%s\"\n", Value[ind].Value[0], pnt);

        exit(12);
      }

    if (tr == 1) StrVal[0] = '\x0';
    else
      {
        StrVal[sizeof(StrVal) - 1] = '\x0';
        while (strlen(StrVal) > 0 &&
               strspn(StrVal + strlen(StrVal) - 1, " \t\n\r") > 0)
          StrVal[strlen(StrVal) - 1] = '\x0';
      }
    if (OutValue) strcpy(OutValue, StrVal);

    return val;
  }

  int GetChEnum(const char *PName, int Channel, char *OutValue = NULL) const
  {
    int ind = dict.GetIndex(PName);

    if (ind < 0)
      {
        printf("%sName \"%s\" does not exist in the dictionary!\n",
               Time_Stamp(), PName);

        exit(13);
      }

    if (Channel <= 0 || Channel > Value[ind].N)
      {
        printf("%sChannel number %d of \"%s\" is out of range (%d - %d)!\n",
               Time_Stamp(), Channel, PName, 1, Value[ind].N);

        return -2;
      }

    if (Initialized <= 0 || Value[ind].Value == NULL ||
        Value[ind].Value[Channel - 1] == NULL)
      {
        printf("%sChannel %d of value \"%s\" is not set!\n", Time_Stamp(),
               Channel, PName);

        fflush(stdout);

        return -1;
      }

    const char *pnt = strstr(Value[ind].Value[Channel - 1], "INTEGER:");
    if (pnt == NULL)
      {
        printf("%sCannot parse the value for parameter \"%s\" (channel %d):\n",
               Time_Stamp(), PName, Channel);
        printf("  \"%s\"\n", Value[ind].Value[Channel - 1]);

        exit(14);
      }

    int val;
    char StrVal[1024];
    if (sscanf(pnt, "INTEGER: %[0-9a-zA-Z](%d", StrVal, &val) < 2)
      {
        StrVal[0] = '\x0';

        if (sscanf(pnt, "INTEGER: %d", &val) <= 0)
          {
            printf("%sCannot parse the value for parameter \"%s\" (channel %d):\n",
                   Time_Stamp(), PName, Channel);
            printf("  \"%s\" from the string \"%s\"\n",
                   Value[ind].Value[Channel - 1], pnt);

            exit(15);
          }
      }

    StrVal[sizeof(StrVal) - 1] = '\x0';
    while (strlen(StrVal) > 0 &&
           strspn(StrVal + strlen(StrVal) - 1, " \t\n\r") > 0)
      StrVal[strlen(StrVal) - 1] = '\x0';
    if (OutValue) strcpy(OutValue, StrVal);

    return val;
  }

  const char *GetChString(const char *PName, int Channel) const
  {
    int ind = dict.GetIndex(PName);

    if (ind < 0)
      {
        printf("%sName \"%s\" does not exist in the dictionary!\n",
               Time_Stamp(), PName);

        exit(16);
      }

    if (Channel <= 0 || Channel > Value[ind].N)
      {
        printf("%sChannel number %d of \"%s\" is out of range (%d - %d)!\n",
               Time_Stamp(), Channel, PName, 1, Value[ind].N);

        OutputString[0] = '\x0';
        return OutputString;
      }

    if (Initialized <= 0 || Value[ind].Value == NULL ||
        Value[ind].Value[Channel - 1] == NULL)
      {
        printf("%sChannel %d of value \"%s\" is not set!\n", Time_Stamp(),
               Channel, PName);

        fflush(stdout);

        OutputString[0] = '\x0';
        return OutputString;
      }

    const char *pnt = strstr(Value[ind].Value[Channel - 1], "STRING:");
    if (pnt == NULL)
      {
        printf("%sCannot parse the value for parameter \"%s\" (channel %d):\n",
               Time_Stamp(), PName, Channel);
        printf("  \"%s\"\n", Value[ind].Value[Channel - 1]);

        exit(17);
      }

    strncpy(OutputString, pnt + 8, sizeof(OutputString));
    OutputString[sizeof(OutputString) - 1] = '\x0';

    return OutputString;
  }

  int GetChInteger(const char *PName, int Channel) const
  {
    int ind = dict.GetIndex(PName);

    if (ind < 0)
      {
        printf("%sName \"%s\" does not exist in the dictionary!\n",
               Time_Stamp(), PName);

        exit(18);
      }

    if (Channel <= 0 || Channel > Value[ind].N)
      {
        printf("%sChannel number %d of \"%s\" is out of range (%d - %d)!\n",
               Time_Stamp(), Channel, PName, 1, Value[ind].N);

        return -2;
      }

    if (Initialized <= 0 || Value[ind].Value == NULL ||
        Value[ind].Value[Channel - 1] == NULL)
      {
        printf("%sChannel %d of value \"%s\" is not set!\n", Time_Stamp(),
               Channel, PName);

        fflush(stdout);

        return -1;
      }

    const char *pnt = strstr(Value[ind].Value[Channel - 1], "INTEGER:");
    if (pnt == NULL)
      {
        printf("%sCannot parse the value for parameter \"%s\" (channel %d):\n",
               Time_Stamp(), PName, Channel);
        printf("  \"%s\"\n", Value[ind].Value[Channel - 1]);

        exit(19);
      }

    int val;
    if (sscanf(pnt, "INTEGER: %d", &val) <= 0)
      {
        printf("%sCannot parse the value for parameter \"%s\" (channel %d):\n",
               Time_Stamp(), PName, Channel);
        printf("  \"%s\" from the string \"%s\"\n",
               Value[ind].Value[Channel - 1], pnt);

        exit(20);
      }

    return val;
  }

  int GetChBits(const char *PName, int Channel, char *OutValue = NULL) const
  {
    int ind = dict.GetIndex(PName);

    if (ind < 0)
      {
        printf("%sName \"%s\" does not exist in the dictionary!\n",
               Time_Stamp(), PName);

        exit(21);
      }

    if (Channel <= 0 || Channel > Value[ind].N)
      {
        printf("%sChannel number %d of \"%s\" is out of range (%d - %d)!\n",
               Time_Stamp(), Channel, PName, 1, Value[ind].N);

        return -2;
      }

    if (Initialized <= 0 || Value[ind].Value == NULL ||
        Value[ind].Value[Channel - 1] == NULL)
      {
        printf("%sChannel %d of value \"%s\" is not set!\n", Time_Stamp(),
               Channel, PName);

        fflush(stdout);

        return -1;
      }

    const char *pnt = strstr(Value[ind].Value[Channel - 1], "BITS:");
    if (pnt == NULL)
      {
        printf("%sCannot parse the value for parameter \"%s\" (channel %d):\n",
               Time_Stamp(), PName, Channel);
        printf("  \"%s\"\n", Value[ind].Value[Channel - 1]);

        exit(22);
      }

    int val;
    char StrVal[1024];
    int tr = sscanf(pnt, "BITS: %x %[ 0-9a-zA-Z()]", &val, StrVal);
    if (tr <= 0)
      {
        printf("%sCannot parse the value for parameter \"%s\" (channel %d):\n",
               Time_Stamp(), PName, Channel);
        printf("  \"%s\" from the string \"%s\"\n",
               Value[ind].Value[Channel - 1], pnt);

        exit(23);
      }

    if (tr == 1) StrVal[0] = '\x0';
    else
      {
        StrVal[sizeof(StrVal) - 1] = '\x0';
        while (strlen(StrVal) > 0 &&
               strspn(StrVal + strlen(StrVal) - 1, " \t\n\r") > 0)
          StrVal[strlen(StrVal) - 1] = '\x0';
      }
    if (OutValue) strcpy(OutValue, StrVal);

    return val;
  }

  float GetChOpaqueFloat(const char *PName, int Channel, char *OutValue = NULL)
  const
  {
    int ind = dict.GetIndex(PName);

    if (ind < 0)
      {
        printf("%sName \"%s\" does not exist in the dictionary!\n",
               Time_Stamp(), PName);

        exit(21);
      }

    if (Channel <= 0 || Channel > Value[ind].N)
      {
        printf("%sChannel number %d of \"%s\" is out of range (%d - %d)!\n",
               Time_Stamp(), Channel, PName, 1, Value[ind].N);

        return -2;
      }

    if (Initialized <= 0 || Value[ind].Value == NULL ||
        Value[ind].Value[Channel - 1] == NULL)
      {
        printf("%sChannel %d of value \"%s\" is not set!\n", Time_Stamp(),
               Channel, PName);

        fflush(stdout);

        return -1;
      }

    const char *pnt = strstr(Value[ind].Value[Channel - 1], "Opaque: Float:");
    if (pnt == NULL)
      {
        printf("%sCannot parse the value for parameter \"%s\" (channel %d):\n",
               Time_Stamp(), PName, Channel);
        printf("  \"%s\"\n", Value[ind].Value[Channel - 1]);

        exit(22);
      }

    float val;
    char StrVal[1024];
    int tr = sscanf(pnt, "Opaque: Float: %f %[ 0-9a-zA-Z()]", &val, StrVal);
    if (tr <= 0)
      {
        printf("%sCannot parse the value for parameter \"%s\" (channel %d):\n",
               Time_Stamp(), PName, Channel);
        printf("  \"%s\" from the string \"%s\"\n",
               Value[ind].Value[Channel - 1], pnt);

        exit(23);
      }

    if (tr == 1) StrVal[0] = '\x0';
    else
      {
        StrVal[sizeof(StrVal) - 1] = '\x0';
        while (strlen(StrVal) > 0 &&
               strspn(StrVal + strlen(StrVal) - 1, " \t\n\r") > 0)
          StrVal[strlen(StrVal) - 1] = '\x0';
      }
    if (OutValue) strcpy(OutValue, StrVal);

    return val;
  }

  int SetValue(const char *Community, u_char Type, const void *Val, int ValLen,
               const char *PName, int Channel = 0, bool fGroupMode = false)
  {
    int ind = dict.GetIndex(PName);

    if (ind < 0)
      {
        printf("%sName \"%s\" does not exist in the dictionary!\n",
               Time_Stamp(), PName);

        return -8;
      }

    if (Value[ind].N <= 0)
      {
        if (Channel != 0)
          {
            printf("%sChannel (%d) is wrong for parameter \"%s\"!\n", Time_Stamp(),
                   Channel, PName);

            return -10;
          }
      }
    else
      {
        if (fGroupMode)
          {
            if (Channel < 0 || Channel >= Value[ind].N)
              {
                printf("%sChannel number %d of \"%s\" is out of range (%d - %d)!\n",
                       Time_Stamp(), Channel, PName, 1, Value[ind].N);

                return -9;
              }
          }
        else
          {
            if (Channel <= 0 || Channel > Value[ind].N)
              {
                printf("%sChannel number %d of \"%s\" is out of range (%d - %d)!\n",
                       Time_Stamp(), Channel, PName, 1, Value[ind].N);

                return -9;
              }
          }
      }

    snmp_session SNMP_Session;

    snmp_sess_init(&SNMP_Session);

    SNMP_Session.peername = IP;

    SNMP_Session.version = SNMP_VERSION_2c;

    SNMP_Session.retries = Retries;
    SNMP_Session.timeout = Timeout;

    free(SNMP_Session.community);
    SNMP_Session.community = (u_char *) strdup(Community);
    SNMP_Session.community_len = strlen((char *) SNMP_Session.community);

    netsnmp_session *snmpsess = snmp_open(&SNMP_Session);

    if (snmpsess == NULL)
      {
        int pcliberr, psnmperr;
        char *pmess;

        snmp_error(&SNMP_Session, &pcliberr, &psnmperr, &pmess);

        printf("%ssnmp_open ERROR. elib = %d, err = %d, Mess = \"%s\"\n",
               Time_Stamp(), pcliberr, psnmperr, pmess);
        fflush(stdout);

        free(pmess);

        return -1;
      }

//      printf("%ssnmp_open successful\n", Time_Stamp());

    netsnmp_pdu *pdu = snmp_pdu_create(SNMP_MSG_SET);

    if (pdu == NULL)
      {
        int pcliberr, psnmperr;
        char *pmess;

        snmp_error(&SNMP_Session, &pcliberr, &psnmperr, &pmess);

        printf("snmp_pdu_create ERROR. elib = %d, err = %d, Mess = \"%s\"\n",
               pcliberr, psnmperr, pmess);
        fflush(stdout);

        free(pmess);

        return -7;
      }

    if (Value[ind].N <= 0)
      {
        snmp_pdu_add_variable(pdu,  dict.Cmd[ind].OID, dict.Cmd[ind].OID_Length,
                              Type, (u_char *) Val, ValLen);
      }
    else
      {
        dict.Cmd[ind].OID[dict.Cmd[ind].OID_Length - 1] = Channel;

        snmp_pdu_add_variable(pdu,  dict.Cmd[ind].OID, dict.Cmd[ind].OID_Length,
                              Type, (u_char *) Val, ValLen);

        dict.Cmd[ind].OID[dict.Cmd[ind].OID_Length - 1] = 1;
      }

    netsnmp_pdu *response = NULL;
    int status = snmp_synch_response(snmpsess, pdu, &response);

    if (status == STAT_SUCCESS)
      {
        if (response->errstat == SNMP_ERR_NOERROR)
          {
          }
        else
          {
            const char *SNMP_ERR[] =
            {
              "SNMP_ERR_NOERROR",
              "SNMP_ERR_TOOBIG",
              "SNMP_ERR_NOSUCHNAME",
              "SNMP_ERR_BADVALUE",
              "SNMP_ERR_READONLY",
              "SNMP_ERR_GENERR",
              "SNMP_ERR_NOACCESS",
              "SNMP_ERR_WRONGTYPE",
              "SNMP_ERR_WRONGLENGTH",
              "SNMP_ERR_WRONGENCODING",
              "SNMP_ERR_WRONGVALUE",
              "SNMP_ERR_NOCREATION",
              "SNMP_ERR_INCONSISTENTVALUE",
              "SNMP_ERR_RESOURCEUNAVAILABLE",
              "SNMP_ERR_COMMITFAILED",
              "SNMP_ERR_UNDOFAILED",
              "SNMP_ERR_AUTHORIZATIONERROR",
              "SNMP_ERR_NOTWRITABLE",
              "SNMP_ERR_INCONSISTENTNAME",
            };

            printf("%sSet value failure \"%s\" (\"%s\").\n",
                   Time_Stamp(), response->errstat >= 0 &&
                   response->errstat < sizeof(SNMP_ERR) / sizeof(SNMP_ERR[0]) ?
                   SNMP_ERR[response->errstat] : "SNMP_ERR_UnknownError",
                   snmp_errstring(response->errstat));
            printf("                   IP \"%s\", Parameter \"%s\", Channel %d!\n",
                   IP, PName, Channel);
            fflush(stdout);
          }
      }
    else if (status == STAT_TIMEOUT)
      {
        printf("%sTimeout: No Response from %s.\n", Time_Stamp(), IP);
        fflush(stdout);

        snmp_close(snmpsess);

        return -11;
      }
    else
      {
        printf("%sSet value error. IP \"%s\", Parameter \"%s\", Channel %d!\n",
               Time_Stamp(), IP, PName, Channel);

        snmp_sess_perror("snmpset", snmpsess);

        snmp_close(snmpsess);

        return -12;
      }

    if (response) snmp_free_pdu(response);

    snmp_close(snmpsess);

    //  SOCK_CLEANUP;

    fflush(stdout);

    return 0;
  }

public:
  Wiener_PL512(const char *IPAddress = NULL) : IP(NULL), Value(NULL),
      Initialized(0), Retries(SNMP_DEFAULT_RETRIES),
      Timeout(SNMP_DEFAULT_TIMEOUT)
  {
    init(IPAddress);
  }

  ~Wiener_PL512()
  {
    DeleteAll();
  }

  void init(const char *IPAddress = NULL)
  {
    DeleteAll();

    if (IPAddress)
      {
        IP = new char[strlen(IPAddress) + 1];

        strcpy(IP, IPAddress);
      }

    if (dict.CmdNumber > 0)
      {
        Value = new ValueStructure[dict.CmdNumber];

        for (int i=dict.CmdNumber-1; i>=0; i--)
          {
            Value[i].Value = new char *[1];

            Value[i].Value[0] = NULL;
          }

        Initialized = -1;

        if (ReadValues())
          {
            if (IP)
              {
                printf("%sPL512 at \"%s\" cannot be initialized!\n",
                       Time_Stamp(), IP);

                fflush(stdout);

                return;
              }
          }

        for (int i=dict.CmdNumber-1; i>0; i--)
          {
            if (dict.GetIndex(dict.Cmd[i].IndexAlias) < 0) Value[i].N = 0;
            else
              {
                Value[i].N = GetInteger(dict.Cmd[i].IndexAlias);

                if (Value[i].N <= 0)
                  {
                    printf(
                      "%sPL512 at \"%s\". Parameter \"%s\" cannot be initialized!\n",
                      Time_Stamp(), IP, dict.Cmd[i].Alias);

                    fflush(stdout);

                    return;
                  }

                if (Value[i].Value && Value[i].Value[0])
                  {
                    delete [] Value[i].Value[0];

                    Value[i].Value[0] = NULL;
                  }

                if (Value[i].Value)
                  {
                    delete [] Value[i].Value;

                    Value[i].Value = NULL;
                  }

                Value[i].Value = new char *[Value[i].N];
                for (int j=Value[i].N-1; j>=0; j--) Value[i].Value[j] = NULL;
              }
          }

        Initialized = 1;
      }
  };

  int ReadValues()
  {
    snmp_session SNMP_Session;
    int lcmd = dict.CmdNumber - 1;
    while (lcmd >= 0)
      {
        snmp_sess_init(&SNMP_Session);

        SNMP_Session.peername = IP;

        SNMP_Session.version = SNMP_VERSION_2c;

        SNMP_Session.retries = Retries;
        SNMP_Session.timeout = Timeout;

        free(SNMP_Session.community);
        SNMP_Session.community = (u_char *) strdup("public");
        SNMP_Session.community_len = strlen((char *) SNMP_Session.community);

        netsnmp_session *snmpsess = snmp_open(&SNMP_Session);

        if (snmpsess == NULL)
          {
            int pcliberr, psnmperr;
            char *pmess;

            snmp_error(&SNMP_Session, &pcliberr, &psnmperr, &pmess);

            printf("%ssnmp_open ERROR. elib = %d, err = %d, Mess = \"%s\"\n",
                   Time_Stamp(), pcliberr, psnmperr, pmess);
            fflush(stdout);

            free(pmess);

            return -1;
          }

//      printf("%ssnmp_open successful\n", Time_Stamp());

        netsnmp_pdu *pdu = snmp_pdu_create(SNMP_MSG_GET);

        if (pdu == NULL)
          {
            int pcliberr, psnmperr;
            char *pmess;

            snmp_error(&SNMP_Session, &pcliberr, &psnmperr, &pmess);

            printf("snmp_pdu_create ERROR. elib = %d, err = %d, Mess = \"%s\"\n",
                   pcliberr, psnmperr, pmess);
            fflush(stdout);

            free(pmess);

            return -7;
          }

        for (int lcurcmd=32; lcmd>=0 && lcurcmd>0; lcmd--)
          {
            if (Value[lcmd].N <= 0)
              {
                snmp_add_null_var(pdu, dict.Cmd[lcmd].OID,
                                  dict.Cmd[lcmd].OID_Length);
                lcurcmd--;
              }
            else
              {
                for (int j=Value[lcmd].N; j>0; j--)
                  {
                    dict.Cmd[lcmd].OID[dict.Cmd[lcmd].OID_Length - 1] = j;

                    snmp_add_null_var(pdu, dict.Cmd[lcmd].OID,
                                      dict.Cmd[lcmd].OID_Length);
                    lcurcmd--;
                  }
              }
          }

        netsnmp_pdu *response;
        int status = snmp_synch_response(snmpsess, pdu, &response);
        time_t VT = time(NULL);

        if (status == STAT_SUCCESS)
          {
            if (response->errstat == SNMP_ERR_NOERROR)
              {
                ValueTime = VT;

                for (netsnmp_variable_list *vars=response->variables; vars;
                     vars=vars->next_variable)
                  {
//          print_variable(vars->name, vars->name_length, vars);

                    int i, k;
                    for (i=dict.CmdNumber-1; i>=0; i--)
                      {
                        int j;
                        if (Value[i].N <= 0)
                          {
                            for (j=(vars->name_length > dict.Cmd[i].OID_Length ?
                                    vars->name_length : dict.Cmd[i].OID_Length) - 1; j>=0; j--)
                              if (vars->name_loc[j] != dict.Cmd[i].OID[j]) break;
                          }
                        else
                          {
                            for (k=Value[i].N; k>0; k--)
                              {
                                dict.Cmd[i].OID[dict.Cmd[i].OID_Length - 1] = k;

                                for (j=(vars->name_length > dict.Cmd[i].OID_Length ?
                                        vars->name_length : dict.Cmd[i].OID_Length) - 1; j>=0; j--)
                                  if (vars->name_loc[j] != dict.Cmd[i].OID[j]) break;

                                if (j < 0) break;
                              }
                          }

                        if (j < 0 && vars->name_length == dict.Cmd[i].OID_Length)
                          {
                            u_char *buf = NULL;
                            size_t buf_len = 256;
                            size_t out_len = 0;

                            buf = (u_char *) calloc(buf_len, 1);
                            if (buf == NULL)
                              {
                                printf("%scalloc() failed. No space?!\n", Time_Stamp());
                                fflush(stdout);

                                snmp_close(snmpsess);

                                return -4;
                              }

                            if (!sprint_realloc_variable(&buf, &buf_len, &out_len, 1,
                                                         vars->name, vars->name_length, vars))
                              {
                                free(buf);

                                snmp_close(snmpsess);

                                return -5;
                              }

                            if (Value[i].N <= 0) k = 0;
                            else k--;

                            if (Value[i].Value[k])
                              {
                                delete [] Value[i].Value[k];

                                Value[i].Value[k] = NULL;
                              }
                            Value[i].Value[k] = new char[out_len + 1];

                            strncpy(Value[i].Value[k], (char *) buf, out_len);
                            Value[i].Value[k][out_len] = '\x0';

                            free(buf);

                            break;
                          }
                      }

                    if (i < 0)
                      {
                        printf("%sParameter ", Time_Stamp());
                        for (int i=0; i<vars->name_length; i++)
                          {
                            if (i > 0) printf(".");
                            printf("%d", vars->name_loc[i]);
                          }
                        printf(
                          " is not found in the variable list returned by PL512.\n");
                        fflush(stdout);

                        snmp_close(snmpsess);

                        return -6;
                      }
                  }
              }
            else
              {
                const char *SNMP_ERR[] =
                {
                  "SNMP_ERR_NOERROR",
                  "SNMP_ERR_TOOBIG",
                  "SNMP_ERR_NOSUCHNAME",
                  "SNMP_ERR_BADVALUE",
                  "SNMP_ERR_READONLY",
                  "SNMP_ERR_GENERR",
                  "SNMP_ERR_NOACCESS",
                  "SNMP_ERR_WRONGTYPE",
                  "SNMP_ERR_WRONGLENGTH",
                  "SNMP_ERR_WRONGENCODING",
                  "SNMP_ERR_WRONGVALUE",
                  "SNMP_ERR_NOCREATION",
                  "SNMP_ERR_INCONSISTENTVALUE",
                  "SNMP_ERR_RESOURCEUNAVAILABLE",
                  "SNMP_ERR_COMMITFAILED",
                  "SNMP_ERR_UNDOFAILED",
                  "SNMP_ERR_AUTHORIZATIONERROR",
                  "SNMP_ERR_NOTWRITABLE",
                  "SNMP_ERR_INCONSISTENTNAME",
                };

                printf("%sReading failed for IP = %s. \"%s\" (\"%s\")\n",
                       Time_Stamp(), IP, response->errstat >= 0 &&
                       response->errstat < sizeof(SNMP_ERR) / sizeof(SNMP_ERR[0]) ?
                       SNMP_ERR[response->errstat] : "SNMP_ERR_UnknownError",
                       snmp_errstring(response->errstat));

                fflush(stdout);
              }
          }
        else if (status == STAT_TIMEOUT)
          {
            printf("%sTimeout: No Response from %s.\n", Time_Stamp(), IP);
            fflush(stdout);

            snmp_close(snmpsess);

            return -2;
          }
        else
          {
            snmp_sess_perror("snmpget", snmpsess);

            snmp_close(snmpsess);

            return -3;
          }

        if (response) snmp_free_pdu(response);

        snmp_close(snmpsess);

        fflush(stdout);
      }

    return 0;
  }

  void PrintValues() const
  {
    static char time_string[30];

    tm *stime = localtime(&ValueTime);

    sprintf(time_string, "%d-%02d-%02d %02d:%02d:%02d. ",
            stime->tm_year + 1900, stime->tm_mon + 1, stime->tm_mday,
            stime->tm_hour, stime->tm_min, stime->tm_sec);

    printf("%sValues from PL512 address \"%s\":\n", time_string, IP);

    if (0)
      {
        for (int i=0; i<dict.CmdNumber; i++)
          {
            if (Value[i].N <= 0)
              printf("\"%s\" : \"%s\"\n", dict.Cmd[i].Name, Value[i].Value[0]);
            else
              {
                printf("\"%s\" list follows :\n", dict.Cmd[i].Name);

                for (int j=0; j<Value[i].N; j++)
                  printf("%d : \"%s\"\n", j + 1, Value[i].Value[j]);
              }
          }
      }
    else
      {
        for (int i=0; i<dict.CmdNumber; i++)
          {
            int ind = dict.Index[i];

            if (Value[ind].N <= 0) printf("\"%s\" : \"%s\"\n",
                                            dict.Cmd[ind].Name, Value[ind].Value[0]);
            else
              {
                printf("\"%s\" list follows :\n", dict.Cmd[ind].Name);

                for (int j=0; j<Value[ind].N; j++)
                  printf("%d : \"%s\"\n", j + 1, Value[ind].Value[j]);
              }
          }
      }

    printf("==============================================================\n");

    fflush(stdout);
  }

  inline int GetNumberOfChannels() const
  {
    return GetInteger("NumberOfChannels");
  }
  inline int GetNumberOfGroups() const
  {
    return GetInteger("NumberOfGroups");
  }
  inline int GetUpTime() const
  {
    return GetTimeticks("UpTime");
  }
  inline const char *GetSystemDescription() const
  {
    return GetString("SystemDescription");
  }
  inline int GetMainSwitchStatus(char *OutValue = NULL)
  {
    return GetEnum("MainSwitch", OutValue);
  }
  inline int GetSystemStatus(char *OutValue = NULL)
  {
    return GetBits("SystemStatus", OutValue);
  }
  inline int GetChannelIndex(int Channel, char *OutValue = NULL) const
  {
    return GetChEnum("ChannelIndex", Channel, OutValue);
  }
  inline const char *GetChannelName(int Channel) const
  {
    return GetChString("ChannelName", Channel);
  }
  inline int GetChannelGroup(int Channel) const
  {
    return GetChInteger("ChannelGroup", Channel);
  }
  inline int GetChannelStatus(int Channel, char *OutValue = NULL)
  {
    return GetChBits("ChannelStatus", Channel, OutValue);
  }
  inline float GetChannelMeasuredVoltage(int Channel, char *OutValue = NULL)
  const
  {
    return GetChOpaqueFloat("ChannelMeasuredVoltage", Channel, OutValue);
  }
  inline float GetChannelMeasuredTerminalVoltage(int Channel, char *OutValue = NULL)
  const
  {
    return GetChOpaqueFloat("ChannelMeasuredTerminalVoltage", Channel, OutValue);
  }
  inline float GetChannelMeasuredCurrent(int Channel, char *OutValue = NULL)
  const
  {
    return GetChOpaqueFloat("ChannelMeasuredCurrent", Channel, OutValue);
  }
  inline int GetChannelTemperatureStatus(int Channel, char *OutValue = NULL)
  const
  {
    return GetChEnum("ChannelTemperatureStatus", Channel, OutValue);
  }
  inline int GetChannelOutputSwitch(int Channel, char *OutValue = NULL)
  const
  {
    return GetChEnum("ChannelOutputSwitch", Channel, OutValue);
  }
  inline float GetChannelOutputVoltage(int Channel, char *OutValue = NULL)
  const
  {
    return GetChOpaqueFloat("ChannelOutputVoltage", Channel, OutValue);
  }
/*
  inline int GetChannelAdjustVoltage(int Channel) const
  {
    return GetChInteger("ChannelAdjustVoltage", Channel);
  }
*/
  inline float GetChannelCurrentLimit(int Channel, char *OutValue = NULL) const
  {
    return GetChOpaqueFloat("ChannelCurrentLimit", Channel, OutValue);
  }
  inline int GetChannelSupervisionBehavior(int Channel) const
  {
    return GetChInteger("ChannelSupervisionBehavior", Channel);
  }
  inline float GetChannelMinimumSenseVoltage(int Channel,
      char *OutValue = NULL) const
  {
    return GetChOpaqueFloat("ChannelMinimumSenseVoltage", Channel, OutValue);
  }
  inline float GetChannelMaximumSenseVoltage(int Channel,
      char *OutValue = NULL) const
  {
    return GetChOpaqueFloat("ChannelMaximumSenseVoltage", Channel, OutValue);
  }
  inline float GetChannelMaxTerminalVoltage(int Channel,
      char *OutValue = NULL) const
  {
    return GetChOpaqueFloat("ChannelMaxTerminalVoltage", Channel, OutValue);
  }
  inline float GetChannelMaxCurrent(int Channel,
                                    char *OutValue = NULL) const
  {
    return GetChOpaqueFloat("ChannelMaxCurrent", Channel, OutValue);
  }
  inline float GetChannelConfigMaxSenseVoltage(int Channel,
      char *OutValue = NULL) const
  {
    return GetChOpaqueFloat("ChannelConfigMaxSenseVoltage", Channel, OutValue);
  }
  inline float GetChannelConfigMaxTerminalVoltage(int Channel,
      char *OutValue = NULL) const
  {
    return GetChOpaqueFloat("ChannelConfigMaxTerminalVoltage", Channel,
                            OutValue);
  }
  inline float GetChannelConfigMaxCurrent(int Channel,
                                          char *OutValue = NULL) const
  {
    return GetChOpaqueFloat("ChannelConfigMaxCurrent", Channel, OutValue);
  }
  inline const char *GetSerialNumber() const
  {
    return GetString("SerialNumber");
  }
  inline int GetOperatingTime() const
  {
    return GetInteger("OperatingTime");
  }
  inline const char *GetFanSerialNumber() const
  {
    return GetString("fanSerialNumber");
  }
  inline int SetChannelGroup(int Channel, int Group)
  {
    return SetValue("admin", ASN_INTEGER, &Group, sizeof(Group),
                    "ChannelGroup", Channel);
  }
  inline int SetGroupSwitch(int Group, int Value)
  {
    return SetValue("admin", ASN_INTEGER, &Value, sizeof(Value),
                    "GroupSwitch", Group, true);
  }
  /*
    inline int GetGroupSwitch(int Group, char *OutValue = NULL)
            const { return GetChEnum("GroupSwitch", Group, OutValue); }
  */
  inline int SetMainSwitch(int Value)
  {
    return SetValue("private", ASN_INTEGER, &Value, sizeof(Value),
                    "MainSwitch");
  }
  inline int SetChannelOutputSwitch(int Channel, int Val)
  {
    return SetValue("private", ASN_INTEGER, &Val, sizeof(Val),
                    "ChannelOutputSwitch", Channel);
  }
  inline int SetChannelOutputVoltage(int Channel, float Voltage)
  {
    return SetValue("guru", ASN_OPAQUE_FLOAT, &Voltage, sizeof(Voltage),
                    "ChannelOutputVoltage", Channel);
  }
  inline int SetChannelCurrentLimit(int Channel, float Current)
  {
    return SetValue("guru", ASN_OPAQUE_FLOAT, &Current, sizeof(Current),
                    "ChannelCurrentLimit", Channel);
  }
  inline int SetChannelSupervisionBehavior(int Channel, int Val)
  {
    return SetValue("admin", ASN_INTEGER, &Val, sizeof(Val),
                    "ChannelSupervisionBehavior", Channel);
  }
  inline int SetChannelMinimumSenseVoltage(int Channel, float Voltage)
  {
    return SetValue("admin", ASN_OPAQUE_FLOAT, &Voltage, sizeof(Voltage),
                    "ChannelMinimumSenseVoltage", Channel);
  }
  inline int SetChannelMaximumSenseVoltage(int Channel, float Voltage)
  {
    return SetValue("admin", ASN_OPAQUE_FLOAT, &Voltage, sizeof(Voltage),
                    "ChannelMaximumSenseVoltage", Channel);
  }
  inline int SetChannelMaximumTerminalVoltage(int Channel, float Voltage)
  {
    return SetValue("admin", ASN_OPAQUE_FLOAT, &Voltage, sizeof(Voltage),
                    "ChannelMaxTerminalVoltage", Channel);
  }
  inline int SetChannelMaximumPower(int Channel, float Power)
  {
    return SetValue("admin", ASN_OPAQUE_FLOAT, &Power, sizeof(Power),
                    "ChannelMaxPower", Channel);
  }
  inline int SetChannelMaximumCurrent(int Channel, float Current)
  {
    return SetValue("admin", ASN_OPAQUE_FLOAT, &Current, sizeof(Current),
                    "ChannelMaxCurrent", Channel);
  }
  inline float GetChannelMaxPower(int Channel, char *OutValue = NULL) const
  {
    return GetChOpaqueFloat("ChannelMaxPower", Channel, OutValue);
  }

  inline const char *GetIP() const
  {
    return IP;
  }

  inline time_t GetTime() const
  {
    return ValueTime;
  }
  inline int GetTimeout() const
  {
    return Timeout;
  }
  inline int GetRetries() const
  {
    return Retries;
  }
  inline int SetTimeout(int timeout)
  {
    int ret(Timeout);
    Timeout = timeout;
    return ret;
  }
  inline int SetRetries(int retries)
  {
    int ret(Retries);
    Retries = retries;
    return ret;
  }
};

/*
Wiener_PL512::Dictionary Wiener_PL512::dict;
char Wiener_PL512::OutputString[];
*/

//_____________________________________________________________________________
char *Time_Stamp()
{
  static char time_string[30];

  time_t ctm = time(NULL);

  tm *stime = localtime(&ctm);

  sprintf(time_string, "PL %d-%02d-%02d %02d:%02d:%02d ",
          stime->tm_year + 1900, stime->tm_mon + 1, stime->tm_mday,
          stime->tm_hour, stime->tm_min, stime->tm_sec);
  return time_string;
}

#endif
