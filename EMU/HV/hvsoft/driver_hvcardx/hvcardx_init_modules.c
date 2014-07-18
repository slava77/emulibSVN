/* hvcard.c: PNPI PCI HV Host Card device driver.
*
* Copyright (C) 2004 Victor Barashko (barvic@ufl.edu)
*/
#include "hvcardx_drv.h"


inline void hvcardx_init_one_module(HVmodule *m)
{
  int i;

  m->state = hvcard_pcie_state;
  if (m->type)
    {
      m->ID = get_module_id(m);
      m->master_hostid = hvcard_pcie_hostid;
      m->master_hvcard = NA;
      m->master_busaddr = NA;
      m->master_chan = 0;
      m->master_id = 0;
      m->master_trip_delay = hvcard_pcie_master_trip_delay;
      m->ilock_delay = hvcard_pcie_ilock_delay;
      m->ilock_delay_cnt = 0;
      m->interlock = get_interlock_state(m);
      m->pos5v = get_pos5vmon(m);
      m->neg5v = get_neg5vmon(m);
      m->hvmon = get_hvmon(m);
      m->map = (UINT *)maps[m->type];
      m->num_chans = num_chans[m->type];
      m->policy = hvcard_pcie_policy;
      for (i=0; i< m->num_chans; i++)
        {
          m->chan[i].module = m;
          m->chan[i].state = hvcard_pcie_state;
          m->chan[i].status = hvcard_pcie_state;
          m->chan[i].ramp_up = hvcard_pcie_ramp_up;
          m->chan[i].ramp_down = hvcard_pcie_ramp_down;
          m->chan[i].imax = hvcard_pcie_imax;
          m->chan[i].imax_ramp = hvcard_pcie_imax_ramp;
          m->chan[i].vmax = hvcard_pcie_vmax;
          m->chan[i].vset = hvcard_pcie_vset;
          m->chan[i].vset_adc = hvcard_pcie_vset;
          m->chan[i].vcur = 0;
          m->chan[i].itrip = 0;
          m->chan[i].vtrip = 0;
          m->chan[i].vov = DEF_CHAN_OV_LIMIT;
          m->chan[i].vtrip_cnt = 0;
          m->chan[i].vtrip_delay = DEF_OV_TRIP_DELAY;
          m->chan[i].trip_cnt = 0;
          m->chan[i].trip_delay = hvcard_pcie_trip_delay;
          m->chan[i].trip_reset_delay = hvcard_pcie_trip_reset_delay;
          m->chan[i].trip_reset_delay_cnt = 0;
          m->chan[i].trip_max_resets = hvcard_pcie_trip_max_resets;
          m->chan[i].trip_resets_cnt = 0;
          m->chan[i].ramp_counter = 0;
          m->chan[i].dead_max_cnt = DEF_CHAN_DEAD_MAX_CNT;
          m->chan[i].dead_cnt = 0;
          m->chan[i].relay = 0;
          m->chan[i].fuse = 0;
          m->chan[i].flags = 0;
        }
    }
}

inline int hvcardx_init_modules(hvcard_pcie_card *card)
{
  int i,j;
  int ID=0;
  struct HVmodule *m;
  unsigned long flags;
  int modules_cnt = 0;
  int link_mod_cnt = 0;

  busy=1;

  if (card == NULL) return -1;

  spin_lock_irqsave(&hvdata_lock, flags);

  /*
  printk(KERN_INFO PFX "hostid:%d pci:%d:%d dataslot:%d rev.%d detect modules\n", card->hostid,
         card->bus, card->slot, card->dataslot, card->revision);
  */

  memset(card->dacdata, 0, sizeof(card->dacdata)); /// Reset DAC write buffer

  for (i=0; i<MAX_CONTROL_LINES; i++)
    {
      link_mod_cnt = 0;
      for (j=0; j<HVCARD_MAX_MODULES; j++)
        {

          m = &(card->link[i].module[j]);
          m->hostcard = &(card->link[i]);
          m->busaddr = j;
          m->ioaddr = (ULONG)(card->adcdata+i+(0xF-j)*MODULE_MAX_CHANS*16);

          m->type = MASTER;	/// Try to detect as Master
          ID = get_module_id(m);
          if ((ID<1) || (ID>62) || (!check_master_id(m)))
            {
              m->type = RDB30; /// Try to detect as RDB
              m->type = get_module_type(m);
            }

          hvcardx_init_one_module(m);

          if (m->type != NONE)
            {
              printk(KERN_INFO PFX "Detected %s module ID#%03d on hostcard: %i, link: %i, address: %X\n",
                     modtype_str[m->type], m->ID, card->dataslot, card->link[i].dataslot, m->busaddr);
              modules_cnt++;
              link_mod_cnt++;
            }

        }
      if (link_mod_cnt > 0)
        printk(KERN_INFO PFX "Found %i module(s) on hostcard: %i, link: %i\n",
               modules_cnt, card->dataslot, card->link[i].dataslot);
    }

  printk(KERN_INFO PFX "Total %i module(s) found on hostcard: %i\n", modules_cnt, card->dataslot);

  spin_unlock_irqrestore(&hvdata_lock, flags);

  busy=0;

  return modules_cnt;
}


