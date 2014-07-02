/* hvcard.c: PNPI PCI HV Host Card device driver.
 *
 * Copyright (C) 2004 Victor Barashko (barvic@ufl.edu)
 */
#include "hvcard_pcie_drv.h"

inline void hvcard_pcie_set(struct HVmodule *m)
{

// return; // !!! Remove after tests


  int i;
  UINT vset, vcur, ramp_up, ramp_down, relay, freq, gap, delay_cnt, ramp_down_cnt;
  // UINT m_status;
  ULONG ioaddr = (ULONG)(m->hostcard->ioaddr) + (((0xF - m->busaddr)*MODULE_MAX_CHANS)<<2);


  if (m->type!=NONE)
    {
      for (i=0; i<m->num_chans; i++)
        {
          // if (hvcard_pcie_debug & m->state) printk(KERN_INFO PFX "ch#%i - %s\n",i,hvstate_str[m->chan[i].status]);
          if (m->chan[i].ramp_up>=hvcard_pcie_ramp_steps)
            {
              ramp_up = m->chan[i].ramp_up/hvcard_pcie_ramp_steps;
              // == if > 0.5 round to 1
              if ((m->chan[i].ramp_up%hvcard_pcie_ramp_steps)*2>=hvcard_pcie_ramp_steps) ramp_up += 1;
            }
          else
            {
              freq=hvcard_pcie_ramp_steps/m->chan[i].ramp_up;
              gap=hvcard_pcie_ramp_steps%m->chan[i].ramp_up;
              if ((ramp_cnt%freq==0) && (ramp_cnt<=(hvcard_pcie_ramp_steps-gap)))
                {
                  ramp_up=1;
                }
              else
                {
                  ramp_up=0;
                }
            }
          if (m->chan[i].ramp_down>=hvcard_pcie_ramp_steps)
            {
              ramp_down = m->chan[i].ramp_down/hvcard_pcie_ramp_steps;
              // == if > 0.5 round to 1
              if ((m->chan[i].ramp_down%hvcard_pcie_ramp_steps)*2>=hvcard_pcie_ramp_steps) ramp_down += 1;
            }
          else
            {
              freq=hvcard_pcie_ramp_steps/m->chan[i].ramp_down;
              gap=hvcard_pcie_ramp_steps%m->chan[i].ramp_down;
              if ((ramp_cnt%freq==0) && (ramp_cnt<=(hvcard_pcie_ramp_steps-gap)))
                {
                  ramp_down=1;
                }
              else
                {
                  ramp_down=0;
                }
            }
          vset = m->chan[i].vset;
          vcur = m->chan[i].vcur;
          switch (m->state)
            {
            case HV_STATE_OFF:
              m->chan[i].state = m->state;
            case HV_STATE_ON:

              if (m->status == HV_STATE_ILOCK) vcur = 0;
              switch (m->chan[i].state)
                {
                case HV_STATE_OFF:
                  // Use ramp down instead of turn off
                  if (vcur > 0)
                    {
                      if ((m->chan[i].status != HV_STATE_DEAD)
                          && (m->chan[i].status != HV_STATE_CHAN_TRIP)
                          && (m->chan[i].status != HV_STATE_OV_TRIP)
                          && (m->chan[i].status != HV_STATE_ILOCK))
                        m->chan[i].status = HV_STATE_RAMP_DOWN;
                      vcur += ((vcur>ramp_down)?-ramp_down:-vcur);
                    }
                  else
                    {
                      if ((m->chan[i].status != HV_STATE_DEAD)
                          && (m->chan[i].status != HV_STATE_CHAN_TRIP)
                          && (m->chan[i].status != HV_STATE_OV_TRIP)
                          && (m->chan[i].status != HV_STATE_ILOCK))
                        m->chan[i].status = m->chan[i].state;
                    }
                  // vcur = 0;
                  break;
                case HV_STATE_ON:
                  if ((m->chan[i].status != HV_STATE_CHAN_TRIP)
                      && (m->chan[i].status != HV_STATE_OV_TRIP)
                      && (m->chan[i].status != HV_STATE_DEAD)
                      && (m->chan[i].status != HV_STATE_OFF)
                      && (m->chan[i].status != HV_STATE_ILOCK)
                      && (m->status != HV_STATE_ILOCK))
                    {
                      if (vcur < vset)
                        {
                          vcur += (((vset-vcur)>ramp_up)?ramp_up:vset-vcur);
                          m->chan[i].status = HV_STATE_RAMP_UP;
                        }
                      else if (vcur > vset)
                        {
                          vcur += (((vcur-vset)>ramp_down)?-ramp_down:vset-vcur);
                          m->chan[i].status = HV_STATE_RAMP_DOWN;
                        }
                      else
                        {
                          if ( (m->chan[i].vset > OV_MIN_VSET)
                               && (m->chan[i].status == HV_STATE_RAMP_DOWN)
                               && (m->chan[i].vmon > (m->chan[i].vset_adc + m->chan[i].vov + 1)) )
                            {
                              /* Wait for CSC to ramp down */
                            }
                          else
                            {
                              m->chan[i].status = m->chan[i].state;
                              /*
                              								// == Check if linked Master channel is still ramping
                              								// == and set RDB channel status to master channel status

                              								if ( (master_assigned(m)) && ((m->master_chan>=0) || (m->master_chan<8)) ) {
                                                                                                      m_status = hostcard[(int)m->master_hvcard]->module[(int)m->master_busaddr].chan[(int)m->master_chan].status;
                              									if ((m_status==HV_STATE_RAMP_UP) || (m_status==HV_STATE_RAMP_DOWN))
                              										m->chan[i].status = m_status;

                                                                      			}
                              */
                            }

                        }
                    }
                  // Ramp down when trips occurs
                  if ((m->chan[i].status == HV_STATE_CHAN_TRIP)
                      || (m->chan[i].status == HV_STATE_OV_TRIP))
                    {
                      if (vcur > 0)
                        {
                          delay_cnt = m->chan[i].ramp_counter&0xFF;
                          ramp_down_cnt = (m->chan[i].ramp_counter>>8)&0xFF;
                          // == Ramp down cycle - 1sec ramp down + rampdown_set_delay sec wait
                          if (delay_cnt < (hvcard_pcie_ramp_steps + hvcard_pcie_rampdown_set_delay*hvcard_pcie_ramp_steps) )
                            {
                              // == Ramp down for 1 sec
                              if (delay_cnt < hvcard_pcie_ramp_steps)
                                {
                                  vcur += ((vcur>ramp_down)?-ramp_down:-vcur);
                                }
                              // == Drop voltage to zero after 10 ramp down cycles
                              if (ramp_down_cnt>=10)
                                {
                                  vcur=0;
                                }
                              delay_cnt++;
                              m->chan[i].ramp_counter = (m->chan[i].ramp_counter & 0xFF00) | (delay_cnt);
                            }
                          else
                            {
                              // Reset delay counter
                              ramp_down_cnt++;
                              m->chan[i].ramp_counter = ramp_down_cnt << 8;
                            }
                        }
                      else
                        {
                          m->chan[i].ramp_counter = 0;
                        }
                    }


                  break;
                default:
                  break;
                }

              m->chan[i].vcur = vcur;
              /*
                    if (m->type == MASTER)
                      {
                        relay = m->chan[i].relay;
                        writel(vcur, (UINT *)(ioaddr+(m->map[i]<<3)));
                        writel(vcur, (UINT *)(ioaddr+((m->map[i]+8)<<3)));
                        writel(vcur, (UINT *)(ioaddr+((m->map[i]+16)<<3)));
                        writel(relay, (UINT *)(ioaddr+(((m->map[i]<<1)+1)<<2)));
                        writel(relay, (UINT *)(ioaddr+((((m->map[i]+8)<<1)+1)<<2)));
                        writel(relay, (UINT *)(ioaddr+((((m->map[i]+16)<<1)+1)<<2)));
                      }
                    else
                      {
                        writel(vcur, (UINT *)(ioaddr+(m->map[i]<<2)));
                      }
              */

              break;
            default:
              break;
            }
        }
    }
}


inline void set_dac_value(int card, int line, int module, int channel, int value)
{

  uint64_t dac_low, dac_high = 0;

  int index = ((0xF-module)*MODULE_MAX_CHANS+channel)<<1;
  dac_low = hostcard[card]->dacdata[index];
  dac_high = hostcard[card]->dacdata[index+1];


  if (line<5)
    {
      dac_low &= ~((uint64_t)(0xFFF) << line*12);
      dac_low |= ((uint64_t)value&0xFFF) << line*12;
    }
  if (line==5)
    {
      dac_low &= ~((uint64_t)(0xF) << 60);
      dac_low |= ((uint64_t)value&0xF) << 60;
      dac_high &= ~((uint64_t)(0xFF));
      dac_high |= ((uint64_t)value>>4)&0xFF;
    }
  if (line>5)
    {
      dac_high &= ~((uint64_t)(0xFFF) << ((line-6)*12+8));
      dac_high |= ((uint64_t)value&0xFFF) << ((line-6)*12+8);
    }



  hostcard[card]->dacdata[index] = dac_low;
  hostcard[card]->dacdata[index+1] = dac_high;

}


void hvcard_update_dac_values(int card)
{
  int i,j,n;
  struct HVmodule *m;
  uint64_t ioaddr_mem = 0;
  UINT relay = 0;
  // uint64_t dac0 = 0x0F00F00F00F00F00;
  // uint64_t dac1 = 0x00F00F00F00F00F0;


  ioaddr_mem = hostcard[card]->ioaddr[0]+MEM_BASE;

  // == Pack DAC data
  for (j=0; j<HVCARD_MAX_MODULES; j++)
    {
      for (n=0; n<MAX_CONTROL_LINES; n++)
        {
          m = &(hostcard[card]->link[n].module[j]);
          if (m->type!=NONE)
            {
              for (i=0; i<m->num_chans; i++)
                {

                  if (m->type == MASTER)
                    {
                      relay = m->chan[i].relay;
                      set_dac_value(card,n,j,m->map[i]<<1,m->chan[i].vcur );
                      set_dac_value(card,n,j,(m->map[i]+8)<<1,m->chan[i].vcur );
                      set_dac_value(card,n,j,(m->map[i]+16)<<1,m->chan[i].vcur );
                      set_dac_value(card,n,j,((m->map[i])<<1)+1,relay );
                      set_dac_value(card,n,j,((m->map[i]+8)<<1)+1,relay );
                      set_dac_value(card,n,j,((m->map[i]+16)<<1)+1,relay );

                      /*
                                        writel(vcur, (UINT *)(ioaddr+(m->map[i]<<3)));
                                        writel(vcur, (UINT *)(ioaddr+((m->map[i]+8)<<3)));
                                        writel(vcur, (UINT *)(ioaddr+((m->map[i]+16)<<3)));
                                        writel(relay, (UINT *)(ioaddr+(((m->map[i]<<1)+1)<<2)));
                                        writel(relay, (UINT *)(ioaddr+((((m->map[i]+8)<<1)+1)<<2)));
                                        writel(relay, (UINT *)(ioaddr+((((m->map[i]+16)<<1)+1)<<2)));
                      */
                    }
                  else
                    {
                      set_dac_value(card,n,j,m->map[i],m->chan[i].vcur );
                      // writel(vcur, (UINT *)(ioaddr+(m->map[i]<<2)));
                    }
                }
            }
	  else 
	    {
		for (i=0; i<MODULE_MAX_CHANS; i++) set_dac_value(card,n,j,i,0);
		
	    }

        }
    }


  // == Write packed DAC data
  for (i=0; i<MAX_DAC_DATA_SIZE/8; i++)
    {
      writeq(hostcard[card]->dacdata[i], ((uint64_t *)ioaddr_mem) + i);

    }
  /*
  for (i=0; i<2; i++) {
  writeq(dac0, ((uint64_t *)ioaddr_mem)+i+8);
  writeq(dac1, ((uint64_t *)ioaddr_mem) + i+ 9);
  }

  writeq(dac0, ((uint64_t *)ioaddr_mem)+20);
  writeq(dac1, ((uint64_t *)ioaddr_mem) + 21);
  */

};

void hvcard_pcie_ramp_timer(unsigned long data)
{
  int i,j,n;
  struct HVmodule *m;

  unsigned long flags;
  spin_lock_irqsave(&hvdata_lock, flags);

  ramp_cnt++;

  if (!busy)
    {
      for (i=0; i<hvcard_pcie_found; i++)
        {
          for (n=0; n<MAX_CONTROL_LINES; n++)
            {
              for (j=0; j<HVCARD_MAX_MODULES; j++)
                {
                  m = &(hostcard[i]->link[n].module[j]);
                  m->busaddr = j;
                  m->map = (UINT *)maps[m->type];
                  m->num_chans = num_chans[m->type];
                  if (m->type)
                    {
                      /*
                        if ((hvcard_pcie_debug) && (m->state))
                        printk(KERN_INFO PFX "%s Module at %i:%X state - %s\n", modtype_str[m->type], i, j, hvstate_str[m->state]);
                      */
                      hvcard_pcie_set(m);

                    }
                }
            }
          hvcard_update_dac_values(i);
        }
    }

  spin_unlock_irqrestore(&hvdata_lock, flags);

  ramp_timer.expires = RUN_AT(HZ/hvcard_pcie_ramp_steps);
  add_timer(&ramp_timer);
  if (ramp_cnt>=hvcard_pcie_ramp_steps) ramp_cnt=0;
#if defined(timer_exit)
  timer_exit(&ramp_timer);
#endif

}

