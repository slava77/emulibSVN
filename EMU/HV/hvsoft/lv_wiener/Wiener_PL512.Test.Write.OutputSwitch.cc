#include "Wiener_PL512.h"

int main() {
  Wiener_PL512 PS("UFCMSHVLVPS3.cern.ch");

  if (PS.ReadValues()) {
    printf("PL512 \"%s\" cannot be read!\n", PS.GetIP() ? PS.GetIP() : "NULL");

    exit(1);
  }
  else {
    time_t Time = PS.GetTime();
    printf("PL512 \"%s\" was successfully read on %s",
                               PS.GetIP() ? PS.GetIP() : "NULL", ctime(&Time));

//  PS.PrintValues();
  }

  printf("Channel output switches:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    char Text[1024];
    int ival = PS.GetChannelOutputSwitch(i, Text);
    if (i > 1) printf(",");
    printf(" %d : \"%s\" (%d)", i, Text, ival);
  }
  printf("\n");

  fflush(stdout);

  PS.SetChannelOutputSwitch(12, 1);
  PS.SetChannelOutputSwitch(11, 1);
  PS.SetChannelOutputSwitch(2, 1);
  PS.SetChannelOutputSwitch(1, 1);
  PS.SetChannelOutputSwitch(4, 1);
  PS.SetChannelOutputSwitch(3, 1);
  PS.SetChannelOutputSwitch(6, 1);
  PS.SetChannelOutputSwitch(5, 1);

/*
  printf("Channel output switches:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    char Text[1024];
    int ival = PS.GetChannelOutputSwitch(i, Text);
    if (i > 1) printf(",");
    printf(" %d : \"%s\" (%d)", i, Text, ival);
  }
  printf("\n");

  for (int ch=PS.GetNumberOfChannels(); ch>0; ch--) {
    PS.SetChannelOutputSwitch(ch, 1);

    PS.ReadValues();

    printf("Channel output switches:");
    for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
      char Text[1024];
      int ival = PS.GetChannelOutputSwitch(i, Text);
      if (i > 1) printf(",");
      printf(" %d : \"%s\" (%d)", i, Text, ival);
    }
    printf("\n");

    fflush(stdout);
  }

  for (int ch=PS.GetNumberOfChannels(); ch>0; ch--) {
    PS.SetChannelOutputSwitch(ch, 0);

    PS.ReadValues();

    printf("Channel output switches:");
    for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
      char Text[1024];
      int ival = PS.GetChannelOutputSwitch(i, Text);
      if (i > 1) printf(",");
      printf(" %d : \"%s\" (%d)", i, Text, ival);
    }
    printf("\n");

    fflush(stdout);
  }

  sleep(2);
*/

  PS.ReadValues();

  printf("Channel output switches:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    char Text[1024];
    int ival = PS.GetChannelOutputSwitch(i, Text);
    if (i > 1) printf(",");
    printf(" %d : \"%s\" (%d)", i, Text, ival);
  }
  printf("\n");

  fflush(stdout);
/*
  PS.ReadValues();

//  PS.PrintValues();

  printf("Number of channels = %d\n", PS.GetNumberOfChannels());
  int UpTime = PS.GetUpTime();
  printf("Up Time = %.2f seconds (%d days, %02d:%02d:%02d.%02d)\n",
         UpTime/100., UpTime/100/3600/24, UpTime/100/3600%24, UpTime/100/60%60,
                                                    UpTime/100%60, UpTime%100);
  printf("System description: \"%s\"\n", PS.GetSystemDescription());

  char Text[1024];
  int ival;
  float fval;

  ival = PS.GetMainSwitchStatus(Text);
  printf("System Main Switch = %d (\"%s\")\n", ival, Text);

  ival = PS.GetSystemStatus(Text);
  printf("System Status = 0x%02x (\"%s\")\n", ival, Text);

  printf("Channel indexes:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    ival = PS.GetChannelIndex(i, Text);
    if (i > 1) printf(",");
    printf(" %d : \"%s\" (%d)", i, Text, ival);
  }
  printf("\n");

  printf("Channel names:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    if (i > 1) printf(",");
    printf(" %d : \"%s\"", i, PS.GetChannelName(i));
  }
  printf("\n");

  printf("Channel groups:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    if (i > 1) printf(",");
    printf(" %d : %d", i, PS.GetChannelGroup(i));
  }
  printf("\n");

  printf("Channel statuses:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    ival = PS.GetChannelStatus(i, Text);
    if (i > 1) printf(",");
    printf(" %d : 0x%02x (\"%s\")", i, ival, Text);
  }
  printf("\n");

  printf("Channel measured voltages:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    fval = PS.GetChannelMeasuredVoltage(i, Text);
    if (i > 1) printf(",");
    printf(" %d : %f %s", i, fval, Text);
  }
  printf("\n");

  printf("Channel measured currents:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    fval = PS.GetChannelMeasuredCurrent(i, Text);
    if (i > 1) printf(",");
    printf(" %d : %f %s", i, fval, Text);
  }
  printf("\n");

  printf("Channel temperature statuses:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    ival = PS.GetChannelTemperatureStatus(i, Text);
    if (i > 1) printf(",");
    printf(" %d : \"%s\" (%d)", i, Text, ival);
  }
  printf("\n");

  printf("Channel output switches:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    ival = PS.GetChannelOutputSwitch(i, Text);
    if (i > 1) printf(",");
    printf(" %d : \"%s\" (%d)", i, Text, ival);
  }
  printf("\n");

  printf("Channel output voltages:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    fval = PS.GetChannelOutputVoltage(i, Text);
    if (i > 1) printf(",");
    printf(" %d : %f %s", i, fval, Text);
  }
  printf("\n");

  printf("Channel adjust voltage:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    if (i > 1) printf(",");
    printf(" %d : %d", i, PS.GetChannelAdjustVoltage(i));
  }
  printf("\n");

  printf("Channel current limit:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    fval = PS.GetChannelCurrentLimit(i, Text);
    if (i > 1) printf(",");
    printf(" %d : %f %s", i, fval, Text);
  }
  printf("\n");

  printf("Channel supervision behavior:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    if (i > 1) printf(",");
    printf(" %d : 0x%x", i, PS.GetChannelSupervisionBehavior(i));
  }
  printf("\n");

  printf("Channel minimum sense voltage:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    fval = PS.GetChannelMinimumSenseVoltage(i, Text);
    if (i > 1) printf(",");
    printf(" %d : %f %s", i, fval, Text);
  }
  printf("\n");

  printf("Channel maximum sense voltage:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    fval = PS.GetChannelMaximumSenseVoltage(i, Text);
    if (i > 1) printf(",");
    printf(" %d : %f %s", i, fval, Text);
  }
  printf("\n");

  printf("Channel max terminal voltage:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    fval = PS.GetChannelMaxTerminalVoltage(i, Text);
    if (i > 1) printf(",");
    printf(" %d : %f %s", i, fval, Text);
  }
  printf("\n");

  printf("Channel maximal current:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    fval = PS.GetChannelMaxCurrent(i, Text);
    if (i > 1) printf(",");
    printf(" %d : %f %s", i, fval, Text);
  }
  printf("\n");

  printf("Channel config max sense voltage:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    fval = PS.GetChannelConfigMaxSenseVoltage(i, Text);
    if (i > 1) printf(",");
    printf(" %d : %f %s", i, fval, Text);
  }
  printf("\n");

  printf("Channel config max terminal voltage:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    fval = PS.GetChannelConfigMaxTerminalVoltage(i, Text);
    if (i > 1) printf(",");
    printf(" %d : %f %s", i, fval, Text);
  }
  printf("\n");

  printf("Channel config max current:");
  for (int i=1; i<=PS.GetNumberOfChannels(); i++) {
    fval = PS.GetChannelConfigMaxCurrent(i, Text);
    if (i > 1) printf(",");
    printf(" %d : %f %s", i, fval, Text);
  }
  printf("\n");

  printf("Serial Number: \"%s\"\n", PS.GetSerialNumber());

  ival = PS.GetOperatingTime();
  printf("Operating Time = %d seconds (%d days, %02d:%02d:%02d)\n",
                        ival, ival/3600/24, ival/3600%24, ival/60%60, ival%60);
*/

  return 0;
}
