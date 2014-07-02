/* hvcard.c: PNPI PCI HV Host Card device driver.
*
* Copyright (C) 2004 Victor Barashko (barvic@ufl.edu)
*/
#include "hvcard_pcie_drv.h"


inline void hvcard_pcie_init_one_module(HVmodule *m)
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

inline void hvcard_pcie_init_modules(hvcard_pcie_card *card)
{
  int i,j,k,t;
  int ID=0;
  struct HVmodule *m;
  unsigned long flags;
  uint32_t word;
  uint64_t ioaddr_mem = 0;

  busy=1;
  spin_lock_irqsave(&hvdata_lock, flags);
  // down_interruptible(&hvdata_lock);
  printk(KERN_INFO PFX "hostid:%d pci:%d:%d dataslot:%d rev.%d ioaddr:0x%x %s mode detect modules\n", card->hostid,
         card->bus, card->slot, card->dataslot, card->revision, card->ioaddr[0], (card->revision>=1 && hvcard_pcie_enable_dma)?"dma":"non-dma");


  // == Reset DAC values to zero
  ioaddr_mem = card->ioaddr[0]+MEM_BASE;
  for (i=0; i<MAX_DAC_DATA_SIZE/8; i++)
    { 
        writeq(card->dacdata[i], ((uint64_t *)ioaddr_mem) + i);

    }

  for (i=0; i<MAX_CONTROL_LINES; i++)
    {
      for (j=0; j<HVCARD_MAX_MODULES; j++)
        {
	  
          m = &(card->link[i].module[j]);
          m->hostcard = &(card->link[i]);
          m->busaddr = j;
          m->ioaddr = card->dma->buf+i+(0xF-j)*MODULE_MAX_CHANS*16;
          // m->ioaddr = card->ioaddr[0] + MEM_BASE;

	  /*
          if (i<1 && j==15)
            {

              printk(KERN_INFO PFX "line:%d card:%d busaddr:%d ioaddr:0x%llX\n",
                     i, j, m->busaddr, m->ioaddr);
              for (t=0; t<MODULE_MAX_CHANS; t++)
                {
                  word = readl((uint32_t *)(m->ioaddr+t*64));
                  printk(KERN_INFO PFX "addr:0x%0x data:0x%08x%s",m->ioaddr+t*64, word, (t<(MODULE_MAX_CHANS-1))?"\n":" ");
                }

            }
	   */

          m->type = MASTER;
          ID = get_module_id(m);
          if ((ID<1) || (ID>62) || (!check_master_id(m)))
            {
              m->type = RDB30;
              m->type = get_module_type(m);

            }
	hvcard_pcie_init_one_module(m);
        if (m->type != NONE)
          printk(KERN_INFO PFX "Detected %s module ID#%03d at hostcard %i, bus address %X\n", 
		modtype_str[m->type], m->ID, card->link[i].dataslot, m->busaddr);


        }
    }
  /*
    for (i=0; i<HVCARD_MAX_MODULES; i++)
      {
        m = &(card->module[i]);
        m->hostcard = card;
        m->busaddr = i;
        if (card->revision>=2 && hvcard_pcie_enable_dma)
          {
            m->ioaddr = (ULONG)(card->dma->buf) + (((0xF-i)*MODULE_MAX_CHANS)<<2);
          }
        else
          {
            m->ioaddr = card->ioaddr + (((0xF-i)*MODULE_MAX_CHANS)<<2);
          }
        // == Autodetect Module Type ==
        m->type = MASTER;
        ID = get_module_id(m);
        if ((ID<1) || (ID>62) || (!check_master_id(m)))
          {
            m->type = RDB30;
            m->type = get_module_type(m);

          }
        // =====

        if (card->ioaddr)
          {
            memset((UCHAR *)card->ioaddr, 0, MAX_HVDATA_SIZE);
          }

        hvcard_pcie_init_one_module(m);
        if (m->type != NONE)
          printk(KERN_INFO PFX "Detected %s module ID#%03d at hostcard %i, bus address %X\n", modtype_str[m->type], m->ID, card->dataslot,
                 m->busaddr);

        // == Reset history buffer
        for (k=0; k<MODULE_MAX_CHANS; k++) {
  	histdata[card->dataslot*HVCARD_MAX_MODULES + i]->chan[k].trip_marker = 0xFFFF;
        }

      }
    */
  spin_unlock_irqrestore(&hvdata_lock, flags);
  // up(&hvdata_lock);
  busy=0;
}


