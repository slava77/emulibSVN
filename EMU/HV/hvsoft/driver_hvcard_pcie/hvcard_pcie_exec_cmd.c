/* hvcard.c: PNPI PCI HV Host Card device driver.
*
* Copyright (C) 2004 Victor Barashko (barvic@ufl.edu)
*/
#include "hvcard_pcie_drv.h"

int hvcard_pcie_exec_cmd(ULONG pusercmd)
{
  HVcmd cmd;
  int i, j, k, n, l, res=0, err=0;
  range itr_card, itr_module, itr_chan;
  int card, module, chan;
  int fTypeFilter = 0;
  int fSkip = 0;
  HV_TYPE mod_type = NONE;
  ULONG *buf = NULL;
  // ULONG bufsize = 0;
  ULONG val = 0;
  memset(&cmd, 0, sizeof(cmd));

  err = copy_from_user(&cmd, (char *)pusercmd, sizeof(cmd));
  if (err > 0) { printk(KERN_ERR PFX "Unable to copy %i bytes from user space\n", err); return -EFAULT;}
  if ((cmd.card!=ALL && cmd.card>=hvcard_pcie_found*MAX_CONTROL_LINES) ||
      (((cmd.module!=ALL) && (cmd.module != RDBALL_TYPE) &&
        (cmd.module != RDB30_TYPE) && (cmd.module != RDB36_TYPE) &&
        (cmd.module != MASTER_TYPE)) && cmd.module>=HVCARD_MAX_MODULES) ||
      (((cmd.chan!=ALL) && (cmd.chan!=CHAMB_1) && (cmd.chan!=CHAMB_2)) && (cmd.chan>=MODULE_MAX_CHANS)))
    return -EFAULT;

  fTypeFilter = (cmd.module >= RDBALL_TYPE && cmd.module <= MASTER_TYPE) ? 1: 0;

  itr_card.start = (cmd.card==ALL)?0:cmd.card;
  itr_card.end   = (cmd.card==ALL)?hvcard_pcie_found*MAX_CONTROL_LINES:cmd.card+1;
  itr_card.len = itr_card.end - itr_card.start;
  itr_module.start = (cmd.module==ALL)?0:cmd.module;
  itr_module.end = (cmd.module==ALL)?HVCARD_MAX_MODULES:cmd.module+1;
  itr_module.len = itr_module.end - itr_module.start;
  switch (cmd.chan)
    {
    case ALL:
      itr_chan.start = 0;
      itr_chan.end = MODULE_MAX_CHANS;
      break;
    case CHAMB_1:
      itr_chan.start = 0;
      itr_chan.end = 18;
      break;
    case CHAMB_2:
      itr_chan.start = 18;
      itr_chan.end = 36;
      break;
    default:
      itr_chan.start = cmd.chan;
      itr_chan.end = cmd.chan+1;
    }
  // itr_chan.start = (cmd.chan==ALL)?0:cmd.chan;
  // itr_chan.end = (cmd.chan==ALL)?MODULE_MAX_CHANS:cmd.chan+1;
  itr_chan.len = itr_chan.end - itr_chan.start;
  // if (hvcard_pcie_debug) printk(KERN_INFO PFX "card%ld-%ld(%ld), module%ld-%ld(%ld), chans%ld-%ld(%ld)\n", itr_card.start, itr_card.end, itr_card.len, itr_module.start, itr_module.end, itr_module.len, itr_chan.start, itr_chan.end, itr_chan.len);
  /*
  bufsize = ((itr_card.end-itr_card.start)?itr_card.end-itr_card.start:1)*
  ((itr_module.end-itr_module.start)?(itr_module.end-itr_module.start):1)*        ((itr_chan.end-itr_chan.start)?(itr_chan.end-itr_chan.start):1)<<2;
  bufsize = (cmd.size==0)?0:bufsize;
  if ((bufsize) && (cmd.size==bufsize)) {
  if (!(buf = kmalloc(bufsize, GFP_KERNEL))) {
  printk(KERN_ERR PFX "unable allocate buffer for parameters\n");
  return -ENOMEM;
  }
  memset(buf, 0, bufsize);
  if (hvcard_pcie_debug)
  printk(KERN_INFO PFX "buffer %li bytes allocated\n", bufsize);
  copy_from_user(buf, (UCHAR *)cmd.data, cmd.size);
  }
  */
  if (hvcard_pcie_debug) printk(KERN_INFO PFX "hvcmd: %s\n", hvcmd_str[cmd.cmd]);

  if (cmd.cmd == HV_DETECT)
    {
      busy = 1;
      // Delay to signal watch timer to stop
      udelay(20000);
      for (card =0; card < hvcard_pcie_found; card++)
        {
          i = card;// + itr_card.start;
          start_dma_transfer(i);
          udelay(20000); 
          hvcard_pcie_init_modules(hostcard[i]);
        }
      busy = 0;
    }

  if (cmd.cmd == HV_SET_RAMPDOWN_SET_DELAY)
    {
      if (cmd.data>0) hvcard_pcie_rampdown_set_delay = cmd.data;
    }

  for (card =0; card < itr_card.len; card++)
    {
      i = card + itr_card.start;

      int ncard = i/MAX_CONTROL_LINES;
      int line = i%MAX_CONTROL_LINES;
      
      //  for (i=itr_card.start; i<itr_card.end; i++) {
      val = (buf)?*(buf+i):cmd.data;
      switch (cmd.cmd)
        {
        case HV_GET_CARD_DATA:
          if (cmd.size == itr_card.len*sizeof(struct HVhostcard))
            {
	      
              err = copy_to_user((UCHAR *)(cmd.data+card*sizeof(struct HVhostcard)), &(hostcard[ncard]->link[line]), sizeof(struct HVhostcard));
	      if (err > 0) { printk(KERN_ERR PFX "Unable to copy %i bytes to user space\n", err); res = -EFAULT;}
              //  printk(KERN_INFO PFX "copy to user %d bytes\n", sizeof(struct HVhostcard));
            }
          break;
        case HV_DETECT:
        case HV_UPDATE_CALIB:
        case HV_SET_RAMPDOWN_SET_DELAY:
	case HV_SET_GLOBAL_STATE:
          break;
        default:
          for (module = 0; module<itr_module.len; module++)
            {

              j = module + itr_module.start;
              fSkip = 0;
              mod_type = hostcard[ncard]->link[line].module[j].type;
              if (fTypeFilter)
                {
                  switch (cmd.module)
                    {
                    case RDBALL_TYPE:
                      if (mod_type != RDB30 && mod_type != RDB36) fSkip = 1;
                      break;
                    case RDB30_TYPE:
                      if (mod_type != RDB30) fSkip = 1;
                      break;
                    case RDB36_TYPE:
                      if (mod_type != RDB36) fSkip = 1;
                      break;
                    case MASTER_TYPE:
                      if (mod_type != MASTER) fSkip = 1;
                      break;
                    }
                }
              if (fSkip) continue;
              val = (buf)?*(buf+card*itr_module.len+module):cmd.data;
              switch (cmd.cmd)
                {
                case HV_SET_MASTER:
                  if (hostcard[ncard]->link[line].module[j].type != NONE)   // && (hostcard[ncard]->link[line].module[j].type != MASTER)) {
                    {

                      hostcard[ncard]->link[line].module[j].master_chan = (cmd.data & 0xF);
                      hostcard[ncard]->link[line].module[j].master_busaddr = (cmd.data>>4 & 0xF);
                      hostcard[ncard]->link[line].module[j].master_hvcard = (cmd.data>>8 & 0xFF);
                      hostcard[ncard]->link[line].module[j].master_hostid = (cmd.data>>16 & 0xFFFF);
                      if (hostcard[ncard]->link[line].module[j].type != MASTER)
                        {
			  n = (cmd.data>>8 & 0xFF)/MAX_CONTROL_LINES;
			  l = (cmd.data>>8 & 0xFF)%MAX_CONTROL_LINES;
                          hostcard[ncard]->link[line].module[j].master_id =
                            get_module_id(&(hostcard[n]->link[l].module[(cmd.data>>4 & 0xF)]));
                        }
                      else
                        {
                          hostcard[ncard]->link[line].module[j].master_id = 0;
                        };
                      /*
                                printk(KERN_INFO PFX "%08lx ID%d mID%d %d:%d:%d:%d\n", cmd.data, hostcard[ncard]->link[line].module[j].ID,
                          hostcard[ncard]->link[line].module[j].master_id, hostcard[ncard]->link[line].module[j].master_hostid,
                                hostcard[ncard]->link[line].module[j].master_hvcard, hostcard[ncard]->link[line].module[j].master_busaddr,
                                hostcard[ncard]->link[line].module[j].master_chan);
                      */
                    }
                  break;
                case HV_GET_MASTER:
                  cmd.data = hostcard[ncard]->link[line].module[j].master_chan + (hostcard[ncard]->link[line].module[j].master_busaddr<<8) + (hostcard[ncard]->link[line].module[j].master_hvcard<<16) + (hostcard[ncard]->link[line].module[j].master_hostid<<24);
                  break;
                case HV_SET_MASTER_TRIP_DELAY:
                  hostcard[ncard]->link[line].module[j].master_trip_delay = cmd.data;
                  break;
                case HV_GET_MASTER_TRIP_DELAY:
                  cmd.data = hostcard[ncard]->link[line].module[j].master_trip_delay;
                  break;
                case HV_GET_MOD_DATA:
                  if (cmd.size == sizeof(struct HVmodule))
                    {
                      err = copy_to_user((UCHAR *)cmd.data, &(hostcard[ncard]->link[line].module[j]), cmd.size);
		      if (err > 0) { printk(KERN_ERR PFX "Unable to copy %i bytes to user space\n", err); res = -EFAULT;}
                    }
                  break;
                case HV_GET_MOD_HIST_DATA:
                  if (cmd.size == sizeof(struct HVhistmodule))
                    {
                      printk(KERN_INFO PFX "Hist%d Position: %d %d %d\n", i*HVCARD_MAX_MODULES+j, histdata[i*HVCARD_MAX_MODULES+j]->chan[0].pos,
                             histdata[i*HVCARD_MAX_MODULES+j]->chan[0].vmon[histdata[i*HVCARD_MAX_MODULES+j]->chan[0].pos],
                             histdata[i*HVCARD_MAX_MODULES+j]->chan[0].imon[histdata[i*HVCARD_MAX_MODULES+j]->chan[0].pos]);
                      err = copy_to_user((UCHAR *)cmd.data, histdata[i*HVCARD_MAX_MODULES+j], cmd.size);
		      if (err > 0) { printk(KERN_ERR PFX "Unable to copy %i bytes to user space\n", err); res = -EFAULT;}
                    }
                  break;
                case HV_SET_MOD_STATE:
                  val = cmd.data;
                  if ((val<HV_STATE_OFF)||(val>HV_STATE_SET))
                    {
                      printk(KERN_WARNING PFX "Wrong module state: %li\n", val);
                      res = -EFAULT;
                    }
                  else
                    {
                      //        if (hvcard_pcie_debug) printk(KERN_INFO PFX "hvcmd: %s %s\n", hvcmd_str[cmd.cmd], hvstate_str[val]);
                      hostcard[ncard]->link[line].module[j].state = val;
                      hostcard[ncard]->link[line].module[j].status = val;
                      res = 0;
                    }
                  break;
                case HV_SET_MOD_TYPE:
                  //      val = (HV_TYPE)((cmd.size)?buf[i][j]:*buf);
                  val = cmd.data;
                  if ((val<NONE)||(val>MON36))
                    {
                      printk(KERN_WARNING PFX "Wrong module type: %li\n", val);
                      res = -EFAULT;
                    }
                  else
                    {
                      if (hostcard[ncard]->link[line].module[j].state != HV_STATE_OFF)
                        {
                          printk(KERN_WARNING PFX "Unable to switch module type while is not OFF, but %s\n", hvstate_str[hostcard[ncard]->link[line].module[j].state]);
                          res = -EFAULT;
                        }
                      else
                        {
                          if (val != hostcard[ncard]->link[line].module[j].type)
                            {
                              //            printk(KERN_INFO PFX "Module type set to %s\n", modtype_str[val]);
                              hostcard[ncard]->link[line].module[j].type = val;
                              hvcard_pcie_init_one_module(&(hostcard[ncard]->link[line].module[j]));
                            }
                          res = 0;
                        }
                    }
                  break;
                case HV_SET_MOD_PRIMARY_STATUS:
                  val = cmd.data;
                  if ((val<HV_STATE_OFF)||(val>HV_STATE_SET))
                    {
                      printk(KERN_WARNING PFX "Wrong module state: %li\n", val);
                      res = -EFAULT;
                    }
                  else
                    {
                      //        if (hvcard_pcie_debug) printk(KERN_INFO PFX "hvcmd: %s %s\n", hvcmd_str[cmd.cmd], hvstate_str[val]);
                      hostcard[ncard]->link[line].module[j].pr_status = val;
                      res = 0;
                    }
                  break;
                case HV_GET_MOD_TYPE:
                  cmd.data = hostcard[ncard]->link[line].module[j].type;
                  // if (cmd.size) buf[i][j] = hostcard[ncard]->link[line].module[j]->type;
                  // else *buf = hostcard[ncard]->link[line].module[j]->type;
                  break;
                case HV_GET_MOD_ADDR:
                  cmd.data = hostcard[ncard]->link[line].module[j].busaddr;
                  // if (cmd.size) buf[i][j] = hostcard[ncard]->link[line].module[j]->busaddr;
                  // else *buf = hostcard[ncard]->link[line].module[j]->busaddr;
                  break;
                case HV_GET_MOD_ID:
                  cmd.data = hostcard[ncard]->link[line].module[j].ID;
                  // if (cmd.size) buf[i][j] = hostcard[ncard]->link[line].module[j]->ID;
                  // else *buf = hostcard[ncard]->link[line].module[j]->ID;
                  break;
                case HV_GET_MOD_ILOCK:
                  cmd.data = hostcard[ncard]->link[line].module[j].interlock;
                  // if (cmd.size) buf[i][j] = hostcard[ncard]->link[line].module[j]->interlock;
                  // else *buf = hostcard[ncard]->link[line].module[j]->interlock;
                  break;
                case HV_GET_MOD_POS5V:
                  cmd.data = hostcard[ncard]->link[line].module[j].pos5v;
                  // if (cmd.size) buf[i][j] = hostcard[ncard]->link[line].module[j]->pos5v;
                  // else *buf = hostcard[ncard]->link[line].module[j]->pos5v;
                  break;
                case HV_GET_MOD_NEG5V:
                  cmd.data = hostcard[ncard]->link[line].module[j].neg5v;
                  // if (cmd.size) buf[i][j] = hostcard[ncard]->link[line].module[j]->neg5v;
                  // else *buf = hostcard[ncard]->link[line].module[j]->neg5v;
                  break;
                case HV_GET_MOD_NUM_CHANS:
                  cmd.data = hostcard[ncard]->link[line].module[j].num_chans;
                  // if (cmd.size) buf[i][j] = hostcard[ncard]->module[j]->num_chans;
                  // else *buf = hostcard[ncard]->link[line].module[j]->num_chans;
                  break;
                case HV_GET_MOD_HVMON:
                  cmd.data = hostcard[ncard]->link[line].module[j].hvmon;
                  break;
                case HV_SET_ILOCK_DELAY:
                  if (cmd.data>0)
                    hostcard[ncard]->link[line].module[j].ilock_delay = cmd.data;
                  break;
                case HV_GET_ILOCK_DELAY:
                  cmd.data = hostcard[ncard]->link[line].module[j].ilock_delay;
                  break;
                case HV_GET_ILOCK_DELAY_CNT:
                  cmd.data = hostcard[ncard]->link[line].module[j].ilock_delay_cnt;
                  break;
                case HV_SET_MOD_POLICY:
                  if (cmd.data>=0)
                    hostcard[ncard]->link[line].module[j].policy = cmd.data;
                  break;
                case HV_GET_MOD_POLICY:
                  cmd.data = hostcard[ncard]->link[line].module[j].policy;
                  break;

                default:

                  for (chan = 0; chan<itr_chan.len; chan++)
                    {
                      k = chan + itr_chan.start;
                      val = (buf)?*(buf+(card*itr_module.len+module)*itr_chan.len+chan):cmd.data;
                      switch (cmd.cmd)
                        {
                        case HV_SET_STATE:
                          if ((cmd.data>=HV_STATE_OFF)&&(cmd.data<=HV_STATE_SET))
                            {
                              hostcard[ncard]->link[line].module[j].chan[k].state = cmd.data;
                              if (hostcard[ncard]->link[line].module[j].chan[k].status != HV_STATE_DEAD)
                                {
                                  if (cmd.data==HV_STATE_ON)
                                    {
                                      hostcard[ncard]->link[line].module[j].chan[k].status = HV_STATE_RAMP_UP;
                                    }
                                  else
                                    {
                                      hostcard[ncard]->link[line].module[j].chan[k].status = cmd.data;
                                    }
                                }
                              hostcard[ncard]->link[line].module[j].chan[k].trip_reset_delay_cnt = 0;
                              hostcard[ncard]->link[line].module[j].chan[k].trip_resets_cnt = 0;
                              histdata[i*HVCARD_MAX_MODULES + j]->chan[k].trip_marker = 0xFFFF;
			      hostcard[ncard]->link[line].module[j].chan[k].flags &= 0xFFFFFFFE;
                            }
                          break;
                        case HV_SET_OFF:
                          hostcard[ncard]->link[line].module[j].chan[k].state = HV_STATE_OFF;
                          if (hostcard[ncard]->link[line].module[j].chan[k].status != HV_STATE_DEAD)
                            hostcard[ncard]->link[line].module[j].chan[k].status = HV_STATE_OFF;
                          hostcard[ncard]->link[line].module[j].chan[k].trip_reset_delay_cnt = 0;
                          hostcard[ncard]->link[line].module[j].chan[k].trip_resets_cnt = 0;
                          histdata[i*HVCARD_MAX_MODULES + j]->chan[k].trip_marker = 0xFFFF;
			  hostcard[ncard]->link[line].module[j].chan[k].flags &= 0xFFFFFFFE;
                          break;
                        case HV_SET_ON:
                          hostcard[ncard]->link[line].module[j].chan[k].state = HV_STATE_ON;
                          if (hostcard[ncard]->link[line].module[j].chan[k].status != HV_STATE_DEAD)
                            hostcard[ncard]->link[line].module[j].chan[k].status = HV_STATE_RAMP_UP;
                          hostcard[ncard]->link[line].module[j].chan[k].trip_reset_delay_cnt = 0;
                          hostcard[ncard]->link[line].module[j].chan[k].trip_resets_cnt = 0;
                          histdata[i*HVCARD_MAX_MODULES + j]->chan[k].trip_marker = 0xFFFF;
			  hostcard[ncard]->link[line].module[j].chan[k].flags &= 0xFFFFFFFE;
                          break;
                        case HV_SET_LOCK:
                          hostcard[ncard]->link[line].module[j].chan[k].state = HV_STATE_ILOCK;
                          hostcard[ncard]->link[line].module[j].chan[k].status = HV_STATE_ILOCK;
                          break;
                        case HV_SET_RAMP_UP:
                          if (cmd.data>0)
                            hostcard[ncard]->link[line].module[j].chan[k].ramp_up = (cmd.data>MAX_RAMP_UP)?MAX_RAMP_UP:cmd.data;
                          break;
                        case HV_SET_RAMP_DOWN:
                          if (cmd.data>0)
                            hostcard[ncard]->link[line].module[j].chan[k].ramp_down = cmd.data;
                          break;
                        case HV_SET_IMAX:
                          // TODO: Disable or check against hard limit
                          if (cmd.data>0)
                            hostcard[ncard]->link[line].module[j].chan[k].imax = cmd.data;
                          break;
                        case HV_SET_IMAX_RAMP:
                          // TODO: Disable or check against hard limit
                          if (cmd.data>0)
                            hostcard[ncard]->link[line].module[j].chan[k].imax_ramp = cmd.data;
                          break;
                        case HV_SET_VMAX:
                          // TODO: Disable or check against hard limit
                          if (cmd.data>0)
                            hostcard[ncard]->link[line].module[j].chan[k].vmax = cmd.data;
                          break;
                        case HV_SET_VSET:
                          val = cmd.data & 0xFFFF;
                          if ((val>=0)
                              && (val<=hostcard[ncard]->link[line].module[j].chan[k].vmax))
                            {
                              /* Force channel status change to prevent false OV trips */
                              if (hostcard[ncard]->link[line].module[j].chan[k].status == HV_STATE_ON)
                                {
                                  if ((hostcard[ncard]->link[line].module[j].chan[k].vset-val) > 0)
                                    {
                                      hostcard[ncard]->link[line].module[j].chan[k].status = HV_STATE_RAMP_DOWN;
                                    }
                                  else if ((hostcard[ncard]->link[line].module[j].chan[k].vset-val) < 0)
                                    {
                                      hostcard[ncard]->link[line].module[j].chan[k].status = HV_STATE_RAMP_UP;
                                    }
                                }
                              hostcard[ncard]->link[line].module[j].chan[k].vset = val;
                              val = (cmd.data>>16) & 0xFFFF;
                              hostcard[ncard]->link[line].module[j].chan[k].vset_adc = val;
                            }
                          /*
                                if (cmd.data>=0 &&
                                cmd.data<=hostcard[ncard]->link[line].module[j].chan[k].vmax)
                                hostcard[ncard]->link[line].module[j].chan[k].vset = cmd.data;
                          */
                          break;
                        case HV_SET_OV_LIMIT:
                          if (cmd.data>=0)
                            hostcard[ncard]->link[line].module[j].chan[k].vov = cmd.data;
                          break;
                        case HV_SET_RELAY:
                          if (cmd.data>=0 && hostcard[ncard]->link[line].module[j].type == MASTER)
                            hostcard[ncard]->link[line].module[j].chan[k].relay = cmd.data;
                          break;
                        case HV_GET_RELAY:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].relay;
                          break;
                        case HV_GET_FUSE_STATUS:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].fuse;
                          break;
                        case HV_GET_VMON:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].vmon;
                          break;
                        case HV_GET_IMON:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].imon;
                          break;
                        case HV_GET_STATE:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].state;
                          break;
                        case HV_GET_STATUS:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].status;
                          break;
                        case HV_GET_VSET:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].vset;
                          break;
                        case HV_GET_VCUR:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].vcur;
                          break;
                        case HV_GET_IMAX:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].imax;
                          break;
                        case HV_GET_IMAX_RAMP:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].imax_ramp;
                          break;
                        case HV_GET_ITRIP:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].itrip;
                          break;
                        case HV_GET_VTRIP:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].vtrip;
                          break;
                        case HV_GET_VMAX:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].vmax;
                          break;
                        case HV_GET_RAMP_UP:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].ramp_up;
                          break;
                        case HV_GET_RAMP_DOWN:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].ramp_down;
                          break;
                        case HV_RESET_TRIP:
                          hostcard[ncard]->link[line].module[j].chan[k].itrip = 0;
                          hostcard[ncard]->link[line].module[j].chan[k].vtrip = 0;
                          hostcard[ncard]->link[line].module[j].chan[k].trip_cnt = 0;
                          hostcard[ncard]->link[line].module[j].chan[k].trip_reset_delay_cnt = 0;
                          hostcard[ncard]->link[line].module[j].chan[k].trip_resets_cnt = 0;
                          hostcard[ncard]->link[line].module[j].chan[k].status = hostcard[ncard]->link[line].module[j].chan[k].state;
                          histdata[i*HVCARD_MAX_MODULES + j]->chan[k].trip_marker = 0xFFFF;
			  hostcard[ncard]->link[line].module[j].chan[k].flags &= 0xFFFFFFFE;
                          break;
                        case HV_SET_TRIP_DELAY:
                          if (cmd.data>0)
                            hostcard[ncard]->link[line].module[j].chan[k].trip_delay = cmd.data;
                          break;
                        case HV_SET_OV_TRIP_DELAY:
                          if (cmd.data>0)
                            hostcard[ncard]->link[line].module[j].chan[k].vtrip_delay = cmd.data;
                          break;
                        case HV_GET_TRIP_DELAY:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].trip_delay;
                          break;
                        case HV_GET_TRIP_CNT:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].trip_cnt;
                          break;
                        case HV_SET_TRIP_RESET_DELAY:
                          if (cmd.data>0)
                            hostcard[ncard]->link[line].module[j].chan[k].trip_reset_delay = cmd.data;
                          break;
                        case HV_GET_TRIP_RESET_DELAY:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].trip_reset_delay;
                          break;
                        case HV_GET_TRIP_RESET_DELAY_CNT:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].trip_reset_delay_cnt;
                          break;
                        case HV_SET_TRIP_MAX_RESETS:
                          if (cmd.data>0)
                            hostcard[ncard]->link[line].module[j].chan[k].trip_max_resets = cmd.data;
                          break;
                        case HV_GET_TRIP_MAX_RESETS:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].trip_max_resets;
                          break;
                        case HV_GET_TRIP_RESETS_CNT:
                          cmd.data = hostcard[ncard]->link[line].module[j].chan[k].trip_resets_cnt;
                          break;
                        case HV_GET_CHAN_DATA:
                          if (cmd.size == sizeof(struct HVchannel))
                            {
                              err = copy_to_user((UCHAR *)cmd.data, &(hostcard[ncard]->link[line].module[j].chan[k]), cmd.size);
			      if (err > 0) { printk(KERN_ERR PFX "Unable to copy %i bytes to user space\n", err); res = -EFAULT; }
                            }
                          break;
                        case HV_SET_DEAD_MAX_CNT:
                          if (cmd.data>0)
                            hostcard[ncard]->link[line].module[j].chan[k].dead_max_cnt = cmd.data;
                          break;
                        default:
                          printk(KERN_WARNING PFX "Unknown HV command %i\n", cmd.cmd);
                          res = -ENOTTY;
                          break;
                        }
                    }
                  break;
                }
            }
        }
    }

  // if ((buf) && (bufsize) && (cmd.size==bufsize))
  //  copy_to_user((UCHAR *)cmd.data, buf, bufsize);

  err = copy_to_user((char *)pusercmd, &cmd, sizeof(cmd));
  if (err > 0) { printk(KERN_ERR PFX "Unable to copy %i bytes to user space\n", err); res = -EFAULT;}

  if (buf) kfree(buf);
  return res;
}

