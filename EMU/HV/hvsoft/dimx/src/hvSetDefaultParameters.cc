#include <iostream>
#include "hvserver.h"

#define DEF_RAMP_UP_REAL 11
#define DEF_RAMP_DOWN_REAL 100
#define DEF_IMAX_REAL 5
#define DEF_IMAX_RAMP_REAL 10

int SetDefaultParameters(ModulesList& mods)
{
  //  for_each(mods.begin(),mods.end(), ResetModule);
  for (ModulesList::const_iterator itr = mods.begin(); itr != mods.end(); ++itr)
    {
      SetModuleDefaultParameters(itr->second);
    }
  return 0;
}


int SetDefaultParameters(vector<HVcmd>& mods)
{
  for_each(mods.begin(),mods.end(), SetModuleDefaultParameters);
  return 0;
}


int SetModuleDefaultParameters(const HVcmd& mod)
{
  HVcmd m = mod;

  int ramp_up=DEF_RAMP_UP;
  int ramp_down=DEF_RAMP_DOWN;
  int imax=DEF_MAX_CURRENT;
  int imax_ramp=DEF_MAX_CURRENT;
  float val;

  LOG4CPLUS_DEBUG(hvlog,MODULE_STR(mod)
                  << " Setting Default Parameters");
  // -- Set default real values (per channel)
  if (LoadCalibration(mod.card, mod.module))
    {
      HVcalibdata& data = dataservices[mod.card].calib[mod.module];
      HVmodule& setdata = dataservices[mod.card].setdata->module[mod.module];
      for ( int chan = 0; chan < dataservices[mod.card].data->module[mod.module].num_chans ; chan++ )
        {
          setdata.chan[chan].ramp_up = DEF_RAMP_UP_REAL;
          val = DEF_RAMP_UP_REAL * data.dac[chan].A;
          ramp_up = ( val < 0 ) ? 0 : ( unsigned long ) val;

          setdata.chan[chan].ramp_down = DEF_RAMP_DOWN_REAL;
          val = DEF_RAMP_DOWN_REAL * data.dac[chan].A;
          ramp_down = ( val < 0 ) ? 0 : ( unsigned long ) val;

          if (dataservices[mod.card].data->module[mod.module].type == MASTER)
            {
              imax=1;
              setdata.chan[chan].imax_ramp = 1500;
            }
          else
            {
              imax =  DEF_IMAX_REAL;
              setdata.chan[chan].imax = imax;
              int v = 0;
              imax = ( imax * 10 > data.cur[chan] [0] ) ? data.cur[chan] [0] : imax;
              while ( ( v <= 3999 ) && ( data.cur[chan] [v + 1] > ( imax * ADC_CONV_COEF ) )
                      || ( data.cur[chan] [v] == data.cur[chan] [v + 1] ) )v++;
              imax = v;
            }

          if (dataservices[mod.card].data->module[mod.module].type == MASTER)
            {
              imax_ramp=1;
              setdata.chan[chan].imax_ramp = 1500;
            }
          else
            {
              imax_ramp =  DEF_IMAX_RAMP_REAL;
              setdata.chan[chan].imax_ramp = imax_ramp;
              int v = 0;
              imax_ramp = ( imax_ramp * 10 > data.cur[chan] [0] ) ? data.cur[chan] [0] : imax_ramp;
              while ( ( v <= 3999 ) && ( data.cur[chan] [v + 1] > ( imax_ramp * ADC_CONV_COEF ) )
                      || ( data.cur[chan] [v] == data.cur[chan] [v + 1] ) )v++;
              imax_ramp = v;
            }

          m.cmd = HV_SET_RAMP_UP;
          m.data = ramp_up;
          m.chan = chan;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &m);

          m.cmd = HV_SET_RAMP_DOWN;
          m.data = ramp_down;
          m.chan = chan;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &m);

          m.cmd = HV_SET_IMAX_RAMP;
          m.data = imax_ramp;
          m.chan = chan;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &m);

          m.cmd = HV_SET_IMAX;
          m.data = imax;
          m.chan = chan;
          ioctl(fd, HVCARD_PCIE_IOXCMD, &m);
        }

    }
  else   // -- No calibration data -> Set default values in DAC/ADC counts
    {

      // == Set specific values for Master modules
      if (dataservices[mod.card].data->module[mod.module].type == MASTER)
        {
          imax=1;
          imax_ramp=1;

        }

      m.cmd = HV_SET_RAMP_UP;
      m.data = ramp_up;
      m.chan = ALL;
      ioctl(fd, HVCARD_PCIE_IOXCMD, &m);

      m.cmd = HV_SET_RAMP_DOWN;
      m.data = ramp_up;
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
    }
  return 0;

}

