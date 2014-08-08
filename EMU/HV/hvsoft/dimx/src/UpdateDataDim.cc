#include "hvserver.h"

int UpdateDataDim()
{
  fLockDimData = true;
  for (int i=0; i<dataservices.size(); i++)
    dataservices[i].dimservice->updateService(dataservices[i].data, sizeof(struct HVhostcard));
  fLockDimData = false;
  return 0;
};

