#include "hvserver.h"

int GetHVData(int fd, UCHAR card, UCHAR module, UCHAR chan, HV_CMD cmd, ULONG *data, ULONG size)
{
  int res=0;
  HVcmd hvcmd;
  hvcmd.card = card;
  hvcmd.module = module;
  hvcmd.chan = chan;
  hvcmd.cmd = cmd;
  if (size)
    hvcmd.data = (unsigned long)data;
  else
    hvcmd.data = *data;
  hvcmd.size = size;
  res = ioctl(fd, HVCARD_PCIE_IOXCMD, &hvcmd);
  if (!size)
    *data = hvcmd.data;
  return res;
};

