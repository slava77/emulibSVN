/* hvcard_pcie UF PCIe microTCA SP Card device driver.
*
* Copyright (C) 2012 Victor Barashko (barvic@ufl.edu)
*/
#include "hvcardx_drv.h"
#include "linux/jiffies.h"


inline void reset_module(HVmodule *m)
{
  int mID=0;
  if (master_assigned(m))
    {
      int card = m->master_hvcard/MAX_CONTROL_LINES;
      int line = m->master_hvcard%MAX_CONTROL_LINES;
      mID = get_module_id(&hostcard[card]->link[line].module[(int)m->master_busaddr]);
      if (is_valid_id(&hostcard[card]->link[line].module[(int)m->master_busaddr], mID) != 0)
        printk(KERN_INFO PFX "Invalid Master ID#%d detected at %i:%X\n",mID, m->master_hvcard, m->master_busaddr);
      else
        {
          // if Master channel assigned reset it
          if ((m->master_chan>=0) || (m->master_chan<8))
            {
              hostcard[card]->link[line].module[(int)m->master_busaddr].chan[(int)m->master_chan].status = HV_STATE_ILOCK;
              hostcard[card]->link[line].module[(int)m->master_busaddr].chan[(int)m->master_chan].vcur = 0;
            }
          else
            {
              hostcard[card]->link[line].module[(int)m->master_busaddr].status = HV_STATE_ILOCK;
            }
        }
    }
  m->type= NONE;
}


inline int check_master_id(HVmodule *m)
{
  int i,j;
  UINT ID[3];
  UINT value = 0;
  unsigned long ioaddr=0;
  memset(ID,0,sizeof(ID));
  if (m->ioaddr)
    {
      ioaddr = m->ioaddr;
      if (m->type == MASTER)
        {
          for (j=0; j<3; j++)
            {
              ioaddr = m->ioaddr+(64*j)*16;
              for (i=0; i<6; i++)
                {
                  // if (m->busaddr == 15) printk (KERN_INFO PFX "a:0x%0x d:0x%x", (uint32_t *)(ioaddr+(((i<<1)+5)<<6)), value);
                  value = readl( (uint32_t *)(ioaddr+(((i<<1)+5)<<6)) );
                  ID[j] |= (((value & 0xFFFF) > 4000)?1:0)<<i;
                }
            }
          // ID[2]=ID[1];
          // printk (KERN_INFO PFX "%d %d %d\n", ID[0],ID[1], ID[2]);

          return ((ID[0] == ID[1]) && (ID[0] == ID[2]));

        }
    }
  return 0;
};


inline int get_module_id(HVmodule *m)
{
  int i;
  int ID = 0;
  UINT value = 0;
  if (m->ioaddr)
    {
      if ((m->type == MASTER) && check_master_id(m))
        {
          for (i=0; i<6; i++)
            {
              value = readl( (uint32_t *)(m->ioaddr+(((i<<1)+5)<<6)) );
              // if (m->busaddr <= 15) printk (KERN_INFO PFX "a:0x%0x d:0x%x", (uint32_t *)(m->ioaddr+(((i<<1)+5)<<6)), value);
              ID |= (((value & 0xFFFF) > 4000)?1:0)<<i;
            }
        }
      else
        {
          for (i=ID_START; i<ID_END; i++)
            {
              value = readl( (uint32_t *)(m->ioaddr+(i<<6)) );
              // if (value&0xFFFF < 0xFE0)
              {
                ID=(ID<<1)+((value & 0x800)?1:0);
              }
            }
        }
      return ID;
    }
  else return 0;
}


// == Check if module's channel is in ramping state
inline int is_ramping(HVmodule *m, int ch)
{
  if (m->chan[ch].status == HV_STATE_RAMP_UP || m->chan[ch].status == HV_STATE_RAMP_DOWN) return 1;
  if (m->pr_status == HV_STATE_RAMP_UP || m->pr_status == HV_STATE_RAMP_DOWN) return 1;
  if (master_assigned(m))
    {
      if ((m->master_chan>=0) || (m->master_chan<8))
        {
          int card = m->master_hvcard/MAX_CONTROL_LINES;
          int line = m->master_hvcard%MAX_CONTROL_LINES;
          if (hostcard[card]->link[line].module[(int)m->master_busaddr].chan[(int)m->master_chan].status == HV_STATE_RAMP_UP
              || hostcard[card]->link[line].module[(int)m->master_busaddr].chan[(int)m->master_chan].status == HV_STATE_RAMP_DOWN)
            return 1;
        }
    }
  return 0;
}


inline int is_valid_id(HVmodule *m, int ID)
{
  if ((m->type == MASTER) && (ID>0) && (ID<63)) return 0;
  if ((m->type == RDB36) && (ID>1) && (ID<170)) return 0;
  if ((m->type == RDB30) && (ID>=170) && (ID<511)) return 0;
  return -1;
}

inline int master_assigned(HVmodule *m)
{
  int card = m->master_hvcard/MAX_CONTROL_LINES;
  int line = m->master_hvcard%MAX_CONTROL_LINES;
  return  ((m->type != MASTER) && (m->master_hostid == hvcard_pcie_hostid) && (m->master_hvcard != NA) &&
           (m->master_hvcard<hvcard_pcie_found) && (m->master_busaddr != NA) &&
           (hostcard[card]->link[line].module[(int)m->master_busaddr].type==MASTER));
}


inline int get_module_type(HVmodule *m)
{
  int ID = get_module_id(m);
  // printk(KERN_INFO PFX "mod %i:%0x ID - %i\n", m->hostcard->dataslot, m->busaddr, ID);
  if ((m->type == MASTER)&&(ID>0)&&(ID<63)) return MASTER;
  if ((ID>0) && (ID<170))
    {
      // printk(KERN_INFO PFX "mod %i:%0x ID - %i\n", m->hostcard->dataslot, m->busaddr, ID);
      return RDB36;
    }
  if ((ID>=170) && (ID<511))
    {
      // printk(KERN_INFO PFX "mod %i:%0x ID - %i\n", m->hostcard->dataslot, m->busaddr, ID);
      return RDB30;
    }
  return NONE;

}

inline int is_RDB(HVmodule *m)
{
  if ((m->type == RDB30) || (m->type == RDB36)) return 1;
  else return 0;
}

inline int get_hvmon(HVmodule *m)
{
  if ((m->type != MASTER) && (m->ioaddr))
    return (readl( (uint32_t *)(m->ioaddr+(HVMON<<6)) ) & 0xFFFF);
  else return 0;
}

inline int get_interlock_state(HVmodule *m)
{
  if (m->ioaddr)
    {
      if (m->type == MASTER)
        return (readl( (uint32_t *)(m->ioaddr+(MASTER_INTERLOCK<<6)))) > 4000;
      else
        return (readl( (uint32_t *)(m->ioaddr+(INTERLOCK<<6)))>>16) < 1000;
    }
  else  return -EFAULT;
}

inline int get_pos5vmon(HVmodule *m)
{
  if (m->ioaddr)  return (readl( (uint32_t *)(m->ioaddr+(POS5VMON<<6))) & 0xFFFF);
  else  return -EFAULT;

}

inline int get_neg5vmon(HVmodule *m)
{
  if (m->ioaddr)  return (readl( (uint32_t *)(m->ioaddr+(NEG5VMON<<6))) >> 16);
  else  return -EFAULT;
}

inline int check_pos5vmon(HVmodule *m)
{
  if (m->ioaddr)
    {
      int value = get_pos5vmon(m);
      return (value>=2000 || value<=4000);
    }
  else  return -EFAULT;
}

inline int check_neg5vmon(HVmodule *m)
{
  if (m->ioaddr)
    {
      int value = get_neg5vmon(m);
      return (value>=1000 || value<=4000);
    }
  else  return -EFAULT;

}

/*
unsigned long loopback_test(hvcard_pcie_card *hostcard)
{
  unsigned int res=0;
  unsigned int ret=0;
  int i;
  union_rnd_data rnd;

  return res;

  // if (hostcard->revision >= 1 )
  {
    printk(KERN_INFO PFX "Performing Loopback test...\n" );


    for (i=0; i<32; i++)
      {
        writel(1<<i, (unsigned int *) (hostcard->ioaddr+LOOPBACK_ADDR));
        ret = readl((unsigned int *)(hostcard->ioaddr+LOOPBACK_ADDR));
        // res |= (~ret)^(1<<i);
        res |= (ret)^(1<<i);
      }

    if (res) printk(KERN_INFO PFX "Loopback test Running 1's failed 0x%X at hostcard %d\n", res, hostcard->dataslot);

    res = 0;
    for (i=0; i<1000; i++)
      {
        get_random_bytes(rnd.bytes, 4);
        writel(rnd.longword, (unsigned int *)(hostcard->ioaddr+LOOPBACK_ADDR));
        ret = readl((unsigned int *)(hostcard->ioaddr+LOOPBACK_ADDR));
        // if ((ret%200)==0) printk(KERN_INFO PFX "Random Number - %lX\n", ret);
        // res |= (~ret)^(rnd.longword);
        res |= (ret)^(rnd.longword);
      }
    if (res) printk(KERN_INFO PFX "Loopback test Random Numbers failed 0x%X at hostcard %d\n", res, hostcard->dataslot);


  }

  return res;
}

unsigned long memperf_test(hvcard_pcie_card *hostcard)
{
  unsigned int res=0;
  size_t count = 2048;
  int ncycles=100;
  UINT* data = NULL;
  size_t i;
  union_rnd_data rnd;
  unsigned long t0, t1;

  printk(KERN_INFO PFX "Memory I/O performance test for hostcard %d\n", hostcard->dataslot);

  return res;

  data  = kzalloc(count, GFP_KERNEL);
  if (!data)
    {
      printk (KERN_ERR PFX "Failed to alloc write data buffer\n");
    }
  else
    {
      // memset(data, 0, count);

      for (i=0; i<count/sizeof(UINT); i++)
        {
          get_random_bytes(rnd.bytes, 4);
          data[i] = rnd.longword;
        }

      t0 = jiffies;

      // writel(0, (unsigned int *) (hostcard->ioaddr));

      //  return res;

      for (i=0; i<ncycles; i++)
        {
          memcpy_toio((uint32_t *)(hostcard->ioaddr), data, count);
          readl((unsigned int *)(hostcard->ioaddr));
        }

      // return res;

      t1 = jiffies;
      printk(KERN_INFO PFX "%d Write cycles time: %ld msec, rate: %ld KB/sec\n", ncycles, (t1-t0)*1000/HZ,
             (ncycles*count)*HZ/(1024*(t1-t0)));

      t0 = jiffies;
      for (i=0; i<ncycles; i++)
        {
          memcpy_fromio(data, (uint32_t *)(hostcard->ioaddr), count);
        }
      t1 = jiffies;
      printk(KERN_INFO PFX "%d Read cycles time: %ld msec, rate: %ld KB/sec\n", ncycles, (t1-t0)*1000/HZ,
             (ncycles*count)*HZ/(1024*(t1-t0)));

      t0 = jiffies;
      for (i=0; i<ncycles; i++)
        {
          memcpy_toio((uint32_t *)(hostcard->ioaddr), data, count);
          memcpy_fromio(data, (uint32_t *)(hostcard->ioaddr), count);
        }
      t1 = jiffies;
      printk(KERN_INFO PFX "%d Read/Write cycles time: %ld msec\n", ncycles, (t1-t0)*1000/HZ);


      memset(data, 0, count);
      memcpy_toio((uint32_t *)(hostcard->ioaddr), data, count);
      kfree(data);

    }


  return res;
}
*/

/*
ssize_t start_dma_transfer(int card)
{
  uint16_t i;
  ssize_t ret = 0;
  uint64_t ioaddr_reg;
  dma_addr_t bmaddr;
  uint32_t treg;
  uint64_t reg_base = REG_BASE; // register base is passed in MSB of f_pos
  uint32_t from_adr; //= (*f_pos) & 0xffffffff; // "from" address is passed in LSB
  size_t rcount = 0;
  uint32_t rfrom_adr = 0;
  size_t count = MAX_DMA_READ_PAGE;

  if ((card < hvcard_pcie_found) )
    {
//            memset(hostcard[card]->dma->buf, 0, DMA_BUF_SIZE);
//            writel(hostcard[card]->dma->dma_addr,(UINT *)(hostcard[card]->ioaddr+DMA_INIT_OFFSET));

      // register offset

      ioaddr_reg = hostcard[card]->ioaddr[1] + REG_BASE; // correct register aperture to point at correct base
      memset(hostcard[card]->dma->buf,0,DMA_BUF_SIZE);

      for (i=0; i< MAX_HVDATA_SIZE/MAX_DMA_READ_PAGE; i++)
     {
      from_adr = (MEM_BASE +i*MAX_DMA_READ_PAGE) & 0xffffffff;
      // write beam length
      writel(count, (uint32_t *)(ioaddr_reg + 0));
      rcount =  readl((uint32_t *)(ioaddr_reg + 0));
      // printk(KERN_INFO PFX "transferring %ld bytes\n", rcount);

      // write "from" address
      writel(from_adr, (uint32_t *)(ioaddr_reg + 4));
      rfrom_adr = readl((uint32_t *)(ioaddr_reg + 4));

      bmaddr = hostcard[card]->dma->dma_addr+i*MAX_DMA_READ_PAGE; // physical memory address for dma transfer

      // write "to" address + command to beam data
      writel(bmaddr, (uint32_t *)(ioaddr_reg + 8));

      // Now read beam complete register.
      // readl will return when beaming is done.
      treg = readl((uint32_t *)(ioaddr_reg + 8));

      // check that beam complete register returned what it's supposed to
      if (treg != bmaddr)
        {
          printk(KERN_ERR PFX "beam complete register mismatch: w: %08x r: %08x \n", bmaddr, treg);
        }

      if (rcount != count)
        {
          printk(KERN_ERR PFX "beam count register mismatch: w: %08x r: %08x \n", rcount, count);
        }

      if (rfrom_adr != from_adr)
        {
          printk(KERN_ERR PFX "beam from_adr register mismatch: w: %08x r: %08x \n", rfrom_adr, from_adr);
        }

      ret += rcount;
     }
    }
  return ret;
}
*/
