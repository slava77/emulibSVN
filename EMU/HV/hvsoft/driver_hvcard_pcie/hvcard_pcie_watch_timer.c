/* hvcard.c: PNPI PCI HV Host Card device driver.
 *
 * Copyright (C) 2004 Victor Barashko (barvic@ufl.edu)
 */
#include "hvcard_pcie_drv.h"
#include <linux/time.h>

void hvcard_pcie_watch_timer(unsigned long data)
{
//  return;  //!!! Remove after tests


  int i,j,k,n , ID,mID, state;
  int chan_trips[MODULE_MAX_CHANS];
  int imax, imin, itrip_limit;
  int vmon[MODULE_MAX_CHANS], delta_vmon[MODULE_MAX_CHANS];
  //  int vdiff;
  //  ULONG val;
  unsigned long ioaddr;
  struct HVmodule *m;
  int iface_noise;
  int trip_state=0;
  ULONG hist_pos=0;
  int hist_module=0;
  ULONG stop_marker=0xFFFF;
  int card, line=0;

  unsigned long flags;

  UINT new_sum_status=0;

  spin_lock_irqsave(&hvdata_lock, flags);

  if (!busy) // --> Busy check
    {
      for (i=0; i<hvcard_pcie_found; i++)
        {
          for (n=0; n<MAX_CONTROL_LINES; n++)
            {
              for (j=0; j<16; j++)
                {
                  // ioaddr = (unsigned long)hostcard[i]->ioaddr + (((0xF-j)*MODULE_MAX_CHANS)<<2);
                  ioaddr = hostcard[i]->dma->buf+n+(0xF-j)*MODULE_MAX_CHANS*16;
                  iface_noise = 0;
                  m = &(hostcard[i]->link[n].module[j]);

                  if (m->type != NONE) // --> Module type is not NONE check
                    {
                      // Read module ID
                      ID = get_module_id(m);

                      // If ID changed report interface noise
                      if (ID != m->ID)
                        {
                          m->ilock_delay_cnt+=hvcard_pcie_update;
                          iface_noise = 1;

                          // Turn off module and connected Master channel if interface noise longer than ilock_delay
                          if (m->ilock_delay_cnt>=m->ilock_delay)
                            {
                              m->state = HV_STATE_OFF;
                              if (master_assigned(m))
                                {
				  card = m->master_hvcard/MAX_CONTROL_LINES;
				  line = m->master_hvcard%MAX_CONTROL_LINES;
                                  if ((m->master_chan>=0) || (m->master_chan<8))
                                    {
                                      hostcard[card]->link[line].module[(int)m->master_busaddr].chan[(int)m->master_chan].state
                                        = HV_STATE_OFF;
                                    }
                                }
                              printk(KERN_INFO PFX "%s at %i:%X ID change from #%03i to #%03i detected\n", 
					modtype_str[m->type], i*MAX_CONTROL_LINES+n, j, m->ID, ID);
                              iface_noise = 0;
                              m->ilock_delay_cnt = 0;
			      // m->type = NONE;
			      // continue;

                            }

                        } // ID check end

                      // Trying to skip interface noise errors
                      if (iface_noise)
                        {
                          printk(KERN_INFO PFX "interface noise %s at %i:%X\n", modtype_str[m->type], i*MAX_CONTROL_LINES+n, j);
                          continue;
                        }

                      m->ID = ID;

                      // Reset module type to NONE if invalid ID detected
                      if (is_valid_id(m, ID) != 0)
                        {
                          printk(KERN_INFO PFX "%s at %i:%X Invalid ID #%03i detected. Set module type to NONE\n", modtype_str[m->type], i*MAX_CONTROL_LINES+n, j, m->ID);
                          // m->type=NONE;
                          reset_module(m);
                          continue;
                        }

                      if (master_assigned(m))
                        {
			  card = m->master_hvcard/MAX_CONTROL_LINES;
                          line = m->master_hvcard%MAX_CONTROL_LINES;
                          mID = get_module_id(&hostcard[card]->link[line].module[(int)m->master_busaddr]);
                          if (is_valid_id(&hostcard[card]->link[line].module[(int)m->master_busaddr], mID) != 0)
                            printk(KERN_INFO PFX "Invalid Master ID#%d detected at %i:%X\n",mID, m->master_hvcard, m->master_busaddr);
                          else m->master_id = mID;
                        }

                      // Check for interlock state change
                      if ((state = get_interlock_state(m)) != m->interlock)
                        {
                          m->interlock = state;
                          printk(KERN_INFO PFX "%s ID#%03i at %i:%X interlock is %s\n", modtype_str[m->type],ID, i*MAX_CONTROL_LINES+n, j, m->interlock ? "On": "Off");
                        }

                      // Set interlock status after ilock_delay
                      if (!m->interlock && (m->status != HV_STATE_ILOCK))
                        {
                          m->ilock_delay_cnt+=hvcard_pcie_update;
                          if (m->ilock_delay_cnt>=m->ilock_delay)
                            {
                              // m->state = HV_STATE_OFF;
                              m->status = HV_STATE_ILOCK;
                              if (master_assigned(m))
                                {
				  card = m->master_hvcard/MAX_CONTROL_LINES;
                                  line = m->master_hvcard%MAX_CONTROL_LINES;
                                  if ((m->master_chan>=0) || (m->master_chan<8))
                                    {
                                      //    hostcard[(int)m->master_hvcard]->module[(int)m->master_busaddr].chan[(int)m->master_chan].state = HV_STATE_OFF;
                                      hostcard[card]->link[line].module[(int)m->master_busaddr].chan[(int)m->master_chan].status = HV_STATE_ILOCK;

                                      hostcard[card]->link[line].module[(int)m->master_busaddr].chan[(int)m->master_chan].vcur = 0;
                                      //    printk(KERN_INFO PFX "master module ID#%03i at %d:%X channel %i is Off due interlock on DB ID#%03i\n", hostcard[(int)m->master_hvcard]->module[(int)m->master_busaddr].ID, m->master_hvcard, m->master_busaddr, m->master_chan, ID);
                                    }
                                  else
                                    {
                                      //    hostcard[(int)m->master_hvcard]->module[(int)m->master_busaddr].state = HV_STATE_OFF;
                                      hostcard[card]->link[line].module[(int)m->master_busaddr].status = HV_STATE_ILOCK;
                                      printk(KERN_INFO PFX "%s ID#%03i at %d:%X is Off due interlock on DB ID#%03i\n",
                                             modtype_str[hostcard[card]->link[line].module[(int)m->master_busaddr].type],
                                             hostcard[card]->link[line].module[(int)m->master_busaddr].ID,
                                             m->master_hvcard, m->master_busaddr, ID);
                                    }
                                }
                            }
                        }
                      else
                        {
                          m->ilock_delay_cnt = 0;
                        }

                      // Check for +/-5V
                      if (!check_pos5vmon(m))
                        {
                          m->pos5v = get_pos5vmon(m);
                          if (m->type!=MASTER)
                            m->state = HV_STATE_OFF;
                          printk(KERN_INFO PFX "%s ID#%03i at %i:%X +5V: %i - FAILURE\n", modtype_str[m->type],ID, i*MAX_CONTROL_LINES+n, j, m->pos5v);
                        }
                      if (!check_neg5vmon(m))
                        {
                          m->neg5v = get_neg5vmon(m);
                          if (m->type!=MASTER) m->state = HV_STATE_OFF;
                          printk(KERN_INFO PFX "%s ID#%03i at %i:%X -5V: %i - FAILURE\n", modtype_str[m->type],ID, i*MAX_CONTROL_LINES+n, j, m->neg5v);
                        }

                      m->hvmon = get_hvmon(m);

                      memset(chan_trips,0, sizeof(chan_trips));
                      imax = 0xFFF;
                      imin = 0;
                      trip_state = 0;

                      // Read voltage and current values
                      for (k=0; k< m->num_chans; k++)
                        {
                          if (m->type == MASTER)
                            {
                              vmon[k] = m->chan[k].vmon;
                              m->chan[k].vmon = (*(uint32_t *)(m->ioaddr+(m->map[k]<<7))) & 0xffff;
                              m->chan[k].imon = (*(uint32_t *)(m->ioaddr+(m->map[k]<<7)))>>16;
                              m->chan[k].fuse = (*(uint32_t *)(m->ioaddr+(((m->map[k]<<1)+1)<<6)))>>16;
                            }
                          else
                            {
                              vmon[k] = m->chan[k].vmon;
                              m->chan[k].vmon = (*(uint32_t *)(m->ioaddr+(m->map[k]<<6))) & 0xffff;
                              m->chan[k].imon = (*(uint32_t *)(m->ioaddr+(m->map[k]<<6)))>>16;
                            }
                          delta_vmon[k] = ((vmon[k] - m->chan[k].vmon) > 0) ? vmon[k] - m->chan[k].vmon: m->chan[k].vmon - vmon[k];
                          imax = (m->chan[k].imon < imax)? m->chan[k].imon: imax;
                          imin = (m->chan[k].imon > imin)? m->chan[k].imon: imin;
                        }

                      // == Loop over module channels

                      hist_module = i*HVCARD_MAX_MODULES*MAX_CONTROL_LINES + n*HVCARD_MAX_MODULES + j;
                      
                      for (k=0; k< m->num_chans; k++)
                        {
                          // == Calculate stop marker for history buffer when trip is detected
                          if (histdata[hist_module]->chan[k].trip_marker < MAX_HIST_BUF_SIZE)
                            {
                              if (histdata[hist_module]->chan[k].trip_marker < MAX_HIST_BUF_SIZE/2)
                                stop_marker = histdata[hist_module]->chan[k].trip_marker
                                              + MAX_HIST_BUF_SIZE/2;
                              else
                                stop_marker = histdata[hist_module]->chan[k].trip_marker
                                              - MAX_HIST_BUF_SIZE/2;
                            }

                          hist_pos = histdata[hist_module]->chan[k].pos;

                          // == Save monitoring values to history buffer until trip condition occurs
                          if ( (histdata[hist_module]->chan[k].trip_marker > MAX_HIST_BUF_SIZE)
                               || ((histdata[hist_module]->chan[k].trip_marker < MAX_HIST_BUF_SIZE)
                                   && (hist_pos != stop_marker)) )
                            {
                              histdata[hist_module]->chan[k].vmon[hist_pos] = m->chan[k].vmon;
                              histdata[hist_module]->chan[k].imon[hist_pos] = m->chan[k].imon;
                              if ( (hist_pos+1) == MAX_HIST_BUF_SIZE)
                                histdata[hist_module]->chan[k].pos = 0;
                              else
                                histdata[hist_module]->chan[k].pos++;
                            }

                          if ((histdata[hist_module]->chan[k].trip_marker < MAX_HIST_BUF_SIZE)
                              && (hist_pos == stop_marker))
                            {
                              m->chan[k].flags |= 0x1;
                            }
                          /* Current ADCs are reverse: lower ADC value - higher current */

                          // ===============================================
                          // == Dead channels detection
                          // ===============================================
                          if ((m->chan[k].imon == 0) || (m->chan[k].imon >= 0xFFF))
                            {
                              if (m->chan[k].dead_cnt < m->chan[k].dead_max_cnt)
                                m->chan[k].dead_cnt++;
                              if (m->chan[k].dead_cnt >= m->chan[k].dead_max_cnt)
                                {
                                  if ( m->chan[k].status != HV_STATE_DEAD)
                                    {
                                      printk(KERN_INFO PFX "%s ID#%03i at %i:%X Ch#%d DEAD status detected\n",
                                             modtype_str[m->type],ID,i*MAX_CONTROL_LINES+n,j,k+1) ;
                                    }
                                  m->chan[k].status = HV_STATE_DEAD;
                                  m->chan[k].trip_cnt = 0;
                                  m->chan[k].vcur = 0;
                                }
                              // == Dont turn off channel automatically
                              // m->chan[k].state = HV_STATE_OFF;

                            }
                          else if (m->chan[k].status == HV_STATE_DEAD)
                            {
                              // == Set channel and module status to trip conditions
                              // == after recovery from DEAD state and when DEAD state counter exceeds dead_max_cnt
                              if (m->chan[k].dead_cnt >= m->chan[k].dead_max_cnt)
                                {
                                  printk(KERN_INFO PFX
                                         "%s ID#%03i at %i:%X Ch#%d switch from DEAD status to CURRENT TRIP detected\n",
                                         modtype_str[m->type],ID,i*MAX_CONTROL_LINES+n,j,k+1) ;
                                  m->chan[k].vcur = 0;
                                  m->status = HV_STATE_INT_TRIP;
                                  m->chan[k].status = HV_STATE_CHAN_TRIP;
                                  m->chan[k].dead_cnt = 0;
                                }
                              /*
                              else
                                {
                                  m->chan[k].vcur = 0;
                                  m->chan[k].status = m->chan[k].state;
                                }
                              */
                            }

                          // Skip trip detection logic if reading wrong ID
                          if (ID == 0 || ID >= 511 || (m->type == MASTER && ID>=63)) continue;

                          // Check for overall system status
                          if ((m->chan[k].status > new_sum_status) && (m->chan[k].status < HV_STATE_DEAD))
                            new_sum_status=m->chan[k].status;

                          // ===============================================
                          // == Trip reset logic
                          // ===============================================

                          if ((m->chan[k].status == HV_STATE_CHAN_TRIP)
                              && (m->chan[k].vcur == 0)
                              && ((m->chan[k].trip_resets_cnt)<m->chan[k].trip_max_resets))
                            {
                              m->chan[k].trip_reset_delay_cnt+=hvcard_pcie_update;
                              if ((m->chan[k].trip_reset_delay_cnt>=m->chan[k].trip_reset_delay)
                                  && (m->chan[k].state = HV_STATE_ON))
                                {
                                  m->chan[k].status = m->chan[k].state;
                                  m->chan[k].trip_resets_cnt++;
                                  printk(KERN_INFO PFX "%s ID#%03i at %i:%X trip reset retry#%i in Ch#%d after delay of %dms\n",
                                         modtype_str[m->type],ID,i*MAX_CONTROL_LINES+n,j,m->chan[k].trip_resets_cnt, k+1, m->chan[k].trip_reset_delay_cnt);
                                  m->chan[k].trip_reset_delay_cnt = 0;
                                  // -- Reset trip marker in history buffer
                                  histdata[i*HVCARD_MAX_MODULES + j]->chan[k].trip_marker = 0xFFFF;
                                }
                            }


                          // ===============================================
                          // == Current Trip detection
                          // ===============================================

                          // -- If Primary->Master->RDB chain is in ramping state use trip level of max(imax,imax_ramp)
                          // -- otherwise use trip_level of imax
                          if (is_ramping(m,k))
                            itrip_limit = (m->chan[k].imax < m->chan[k].imax_ramp)?m->chan[k].imax:m->chan[k].imax_ramp;
                          else
                            itrip_limit = m->chan[k].imax;

                          if ((m->chan[k].imon >0) && (m->chan[k].imon <= itrip_limit)
                              // && (m->chan[k].state != HV_STATE_OFF)
                              && (m->chan[k].status != HV_STATE_RAMP_DOWN))
                            {
                              // ==============================================
                              // == Prevent trips on RDB at transition point.
                              // == Disabling trip detection for actual RDB voltage
                              // == less than Vinput - 1000V  (1000V ~= 930 ADC counts)
                              //vdiff = (m->chan[k].vmon*107)/100 - ((4*m->chan[k].vcur)/3)-80;
                              if (((m->type == RDB30) || (m->type == RDB36))
                                  && is_ramping(m,k)
                                  && (m->chan[k].vmon < (m->hvmon-930)))
                                {
                                  /*
                                  	printk(KERN_INFO PFX "%s ID#%03i at %i:%X CURRENT RAMP UP pre-TRIP in Ch#%d Vtrip=%d Itrip=%d\n",
                                                                                                       modtype_str[m->type],ID,i, j, k+1, m->chan[k].vmon, m->chan[k].imon);
                                  */
                                }
                              // ==============================================
                              else // ---> Normal trip action
                                {
                                  m->chan[k].trip_cnt+=hvcard_pcie_update;

                                  if (m->chan[k].trip_cnt >= m->chan[k].trip_delay)
                                    {
                                      // == Turn off connected master channel
                                      // == if trip condition stays in turned off state

                                      // --> Handle linked Master
                                      if (master_assigned(m) && (m->chan[k].vcur==0)
                                          && (delta_vmon[k] < 8)
                                          && (m->chan[k].trip_cnt >= (m->master_trip_delay + m->chan[k].trip_delay)))
                                        {
					  card = m->master_hvcard/MAX_CONTROL_LINES;
	                                  line = m->master_hvcard%MAX_CONTROL_LINES;
                                          if ((m->master_chan>=0) || (m->master_chan<8))
                                            {
                                              // hostcard[(int)m->master_hvcard]->module[(int)m->master_busaddr].chan[(int)m->master_chan].state = HV_STATE_OFF;
                                              hostcard[card]->link[line].module[(int)m->master_busaddr].chan[(int)m->master_chan].status = HV_STATE_CHAN_TRIP;

                                              hostcard[card]->link[line].module[(int)m->master_busaddr].chan[(int)m->master_chan].vcur = 0;
                                              printk(KERN_INFO PFX "%s ID#%03i at %d:%X Ch#%i is Off due trip on DB ID#%03i\n",
                                                     modtype_str[hostcard[card]->link[line].module[(int)m->master_busaddr].type],
                                                     hostcard[card]->link[line].module[(int)m->master_busaddr].ID,
                                                     m->master_hvcard, m->master_busaddr, m->master_chan+1, ID);
                                            }
                                          else
                                            {
                                              hostcard[card]->link[line].module[(int)m->master_busaddr].state = HV_STATE_OFF;
                                              hostcard[card]->link[line].module[(int)m->master_busaddr].status = HV_STATE_OFF;
                                              printk(KERN_INFO PFX "%s ID#%03i at %d:%X is Off due channel trip on DB ID#%03i\n",
                                                     modtype_str[hostcard[card]->link[line].module[(int)m->master_busaddr].type],
                                                     hostcard[card]->link[line].module[(int)m->master_busaddr].ID,
                                                     m->master_hvcard, m->master_busaddr, ID);
                                            }
                                        }
                                      // <-- Handle linked Master


                                      // ---> Set module and channel Trip status and set voltage to 0
                                      if (m->chan[k].imon < (imax + 500) )
                                        {
                                          m->status = HV_STATE_INT_TRIP;
                                          m->chan[k].status = HV_STATE_CHAN_TRIP;
                                          // == Set trip marker in history buffer
                                          histdata[hist_module]->chan[k].trip_marker = hist_pos;

                                          // == Current policy - do not change channel state to OFF
                                          // == Uncomment line below to disable this policy
                                          // m->chan[k].state = HV_STATE_OFF;
                                          // =====================================================

                                          // m->chan[k].vset = 0;

                                          // == Uncomment to set voltage to 0 immediately without ramp down
                                          // m->chan[k].vcur = 0;
                                          m->chan[k].itrip = m->chan[k].imon;
                                          m->chan[k].vtrip = m->chan[k].vmon;
                                          printk(KERN_INFO PFX "%s ID#%03i at %i:%X CURRENT TRIP in Ch#%d Vtrip=%d Itrip=%d Period=%dms (hist%d, pos%d)\n",
                                                 modtype_str[m->type],ID,i*MAX_CONTROL_LINES+n, j, k+1, m->chan[k].vtrip, m->chan[k].itrip, m->chan[k].trip_cnt, hist_module, hist_pos);
					  /*
                                          if (m->type == MASTER)
                                            {
                                              writel( m->chan[k].vcur, (UINT *)(ioaddr+(m->map[i]<<3)) );
                                              writel( m->chan[k].vcur, (UINT *)(ioaddr+((m->map[i]+8)<<3)) );
                                              writel( m->chan[k].vcur, (UINT *)(ioaddr+((m->map[i]+16)<<3)) );
                                            }
                                          else
                                            {
                                              writel( m->chan[k].vcur, (UINT *)(ioaddr+(m->map[i]<<2)) );
                                            }
					  */
                                        }
                                      // <--- Set module and channel Trip status and set voltage to 0

                                    }
                                  else // == Report pre-trip
                                    printk(KERN_INFO PFX "%s ID#%03i at %i:%X CURRENT pre-TRIP in Ch#%d Vtrip=%d Itrip=%d Period=%dms\n",
                                           modtype_str[m->type],ID,i*MAX_CONTROL_LINES+n, j, k+1, m->chan[k].vmon, m->chan[k].imon, m->chan[k].trip_cnt);

                                } // <--- Normal trip action

                            }
                          else // == No current trip detected. Reset channel trip counter
                            /* if (m->chan[k].state != HV_STATE_OFF) */ m->chan[k].trip_cnt = 0;


                          // ===============================================
                          // == Overvoltage detection
                          // ===============================================
                          if ( /* (m->policy & POLICY_OVERVOLT) && */
                            m->chan[k].status == HV_STATE_ON && m->chan[k].vset > OV_MIN_VSET)
                            {
                              if (m->chan[k].vmon > (m->chan[k].vset_adc + m->chan[k].vov))
                                {
                                  m->chan[k].vtrip_cnt+=hvcard_pcie_update;
                                  if (m->chan[k].vtrip_cnt >= m->chan[k].vtrip_delay)
                                    {
                                      // --> Handle linked Master
                                      if (master_assigned(m) && (m->chan[k].vcur==0)
                                          && (m->chan[k].vtrip_cnt >= (m->master_trip_delay + m->chan[k].vtrip_delay)))
                                        {
					  card = m->master_hvcard/MAX_CONTROL_LINES;
	                                  line = m->master_hvcard%MAX_CONTROL_LINES;
                                          if ((m->master_chan>=0) || (m->master_chan<8))
                                            {
                                              if 	(hostcard[card]->link[line].module[(int)m->master_busaddr].chan[(int)m->master_chan].status == HV_STATE_ON)
                                                {
                                                  // hostcard[(int)m->master_hvcard]->module[(int)m->master_busaddr].chan[(int)m->master_chan].state = HV_STATE_OFF;
                                                  hostcard[card]->link[line].module[(int)m->master_busaddr].chan[(int)m->master_chan].status = HV_STATE_OV_TRIP;

                                                  hostcard[card]->link[line].module[(int)m->master_busaddr].chan[(int)m->master_chan].vcur = 0;
                                                  printk(KERN_INFO PFX "%s ID#%03i at %d:%X Ch#%i is Off due OVER VOLTAGE TRIP on DB ID#%03i\n",
                                                         modtype_str[hostcard[card]->link[line].module[(int)m->master_busaddr].type],
                                                         hostcard[card]->link[line].module[(int)m->master_busaddr].ID,
                                                         m->master_hvcard, m->master_busaddr, m->master_chan+1, ID);
                                                }
                                            }
                                          else
                                            {
                                              hostcard[card]->link[line].module[(int)m->master_busaddr].state = HV_STATE_OFF;
                                              hostcard[card]->link[line].module[(int)m->master_busaddr].status = HV_STATE_OFF;
                                              printk(KERN_INFO PFX "%s ID#%03i at %d:%X is Off due channel OVER VOLTAGE TRIP on DB ID#%03i\n",
                                                     modtype_str[hostcard[card]->link[line].module[(int)m->master_busaddr].type],
                                                     hostcard[card]->link[line].module[(int)m->master_busaddr].ID,
                                                     m->master_hvcard, m->master_busaddr, ID);
                                            }
                                        }
                                      // <-- Handle linked Master

                                      m->status = HV_STATE_INT_TRIP;
                                      m->chan[k].status = HV_STATE_OV_TRIP;
                                      // == Uncomment to set voltage to 0 immediately without ramp down
                                      // m->chan[k].vcur = 0;
                                      m->chan[k].itrip = m->chan[k].imon;
                                      m->chan[k].vtrip = m->chan[k].vmon;

                                      printk(KERN_INFO PFX "%s ID#%03i at %i:%X OVER VOLTAGE TRIP in Ch#%d Vtrip=%d Itrip=%d Period=%dms\n",
                                             modtype_str[m->type],ID,i*MAX_CONTROL_LINES+n, j, k+1, m->chan[k].vtrip, m->chan[k].itrip, m->chan[k].vtrip_cnt);
				       /*
                                      if (m->type == MASTER)
                                        {
                                          writel( m->chan[k].vcur, (UINT *)(ioaddr+(m->map[i]<<3)) );
                                          writel( m->chan[k].vcur, (UINT *)(ioaddr+((m->map[i]+8)<<3)) );
                                          writel( m->chan[k].vcur, (UINT *)(ioaddr+((m->map[i]+16)<<3) ));
                                        }
                                      else
                                        {
                                          writel( m->chan[k].vcur, (UINT*)(ioaddr+(m->map[i]<<2)) );
                                        }
					*/
                                    }
                                  else // == Report pre-trip
                                    printk(KERN_INFO PFX "%s ID#%03i at %i:%X OVER VOLTAGE pre-TRIP in Ch#%d Vtrip=%d Itrip=%d Period=%dms\n",
                                           modtype_str[m->type],ID,i*MAX_CONTROL_LINES+n, j, k+1, m->chan[k].vmon, m->chan[k].imon, m->chan[k].vtrip_cnt);

                                }
                              else
                                {
                                  /*if (m->chan[k].state != HV_STATE_OFF)*/
                                  m->chan[k].vtrip_cnt=0;
                                }
                            }
                          if (m->chan[k].status == HV_STATE_CHAN_TRIP || m->chan[k].status == HV_STATE_OV_TRIP) trip_state++;

                        } // == Channels loop
                      // == Automatically reset module TRIP status, there are no channels with TRIP status
                      if (m->status == HV_STATE_INT_TRIP && trip_state ==0) m->status =m->state;

                    } // <-- Module type is not NONE check
                } // == Modules loop
            } // == Control Lines loop
        } // == HostCards loop
      if (new_sum_status != sum_status)
        {
          printk(KERN_INFO PFX "Overall system status changed from %s to %s\n",hvstate_str[sum_status], hvstate_str[new_sum_status]);
        }
      sum_status = new_sum_status;
      // == Initiate new DMA readout transfer
      for (i=0; i<hvcard_pcie_found; i++)  start_dma_transfer(i);

    } // <-- Busy check

  spin_unlock_irqrestore(&hvdata_lock, flags);

  // == Reschedule watch timer
  watch_timer.expires = RUN_AT((hvcard_pcie_update*HZ)/1000);
  add_timer(&watch_timer);

#if defined(timer_exit)
  timer_exit(&watch_timer);
#endif
}
