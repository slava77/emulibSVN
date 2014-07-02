/* hvcard_pcie UF PCIe microTCA SP Card device driver.
*
* Copyright (C) 2012 Victor Barashko (barvic@ufl.edu)
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/version.h>

#include "hvcard_pcie_drv.h"

UINT hvcard_pcie_major = 0; // Dynamic allocation of MAJOR number
UINT hvcard_pcie_debug = 1; // Debugging info
UINT hvcard_pcie_found=0;
UINT hvcard_pcie_nr_devs = 2;
char hvcard_pcie_string[255];
UINT hvcard_pcie_hostid = 0;
UINT hvcard_pcie_settime=200; // 200ms
UINT hvcard_pcie_update=20;   // 20ms
UINT hvcard_pcie_state = HV_STATE_OFF;
UINT hvcard_pcie_ramp_up = DEF_RAMP_UP;
UINT hvcard_pcie_ramp_down = DEF_RAMP_DOWN;
UINT hvcard_pcie_imax = DEF_MAX_CURRENT;
UINT hvcard_pcie_imax_ramp = DEF_MAX_RAMP_CURRENT;
UINT hvcard_pcie_vmax = DEF_MAX_VOLTAGE;
UINT hvcard_pcie_vset = DEF_SET_VOLTAGE;
UINT hvcard_pcie_trip_delay = DEF_TRIP_DELAY;
UINT hvcard_pcie_master_trip_delay = DEF_MASTER_TRIP_DELAY;
UINT hvcard_pcie_ilock_delay = DEF_ILOCK_DELAY;
UINT hvcard_pcie_trip_reset_delay = DEF_CHAN_TRIP_RESET_DELAY;
UINT hvcard_pcie_trip_max_resets = DEF_CHAN_MAX_TRIP_RESETS;
UINT hvcard_pcie_ramp_steps = 10; // Ramp timer  1sec/hvcard_ramp_steps
UINT hvcard_pcie_enable_dma = 1;
UINT hvcard_pcie_policy = DEF_POLICY;
UINT hvcard_pcie_skip_mod_init = 0;
UINT hvcard_pcie_disabled_list[20];
int  disabled_list_argc = 0;
UINT hvcard_pcie_rampdown_set_delay = 1; // 1 sec ramp down set delay for trip condition
UINT start_delay = 1;  // 1secs timers start delay
UINT busy=0;
UINT do_detect = 0;
UINT ramp_cnt = 0;

UINT sum_status = 0;
spinlock_t hvdata_lock = SPIN_LOCK_UNLOCKED;

hvcard_pcie_card **hostcard;  // one hostcard entry
UINT *buf;
HVhistmodule **histdata;  // Monitoring history data

struct timer_list watch_timer;
struct timer_list ramp_timer;

module_param(hvcard_pcie_debug, int, 0);
module_param(hvcard_pcie_hostid, int, 0);
module_param(hvcard_pcie_nr_devs, int, 0);

module_param(hvcard_pcie_settime, int, 0);
module_param(hvcard_pcie_update, int, 0);
module_param(hvcard_pcie_state, int, 0);
module_param(hvcard_pcie_ramp_up,int, 0);
module_param(hvcard_pcie_ramp_down, int, 0);
module_param(hvcard_pcie_imax, int, 0);
module_param(hvcard_pcie_vmax, int, 0);
module_param(hvcard_pcie_vset, int, 0);
module_param(hvcard_pcie_ramp_steps, int, 0);
module_param(hvcard_pcie_trip_delay, int, 0);
module_param(hvcard_pcie_master_trip_delay, int, 0);
module_param(hvcard_pcie_policy, int, 0);
module_param(hvcard_pcie_skip_mod_init, int, 0);
module_param(hvcard_pcie_enable_dma, int, 0);


#if defined(module_param_array) && (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9))
module_param_array(hvcard_pcie_disabled_list, int, &disabled_list_argc, 0);
#else
MODULE_PARM(hvcard_pcie_disabled_list, "1-20i");
#endif

MODULE_PARM_DESC(hvcard_disabled_list, "Disable listed modules (Format: 100*PCIeBus+PCIeSlot)");

MODULE_AUTHOR("Victor Barashko (barvic@ufl.edu)");
MODULE_DESCRIPTION("UF HV Host Card PCI-e driver");
MODULE_LICENSE("GPL");

static int __init hvcard_pcie_init(void)
{
  int i, result, datasize=0;

  result = register_chrdev(hvcard_pcie_major, DRV_MODULE_NAME, &hvcard_pcie_fops);
  if (result < 0)
    {
      printk(KERN_WARNING PFX "Unable to get major %d\n",hvcard_pcie_major);
      return result;
    }
  if (hvcard_pcie_major == 0) hvcard_pcie_major = result; /* dynamic */

  hostcard = kmalloc(hvcard_pcie_nr_devs*sizeof(ULONG), GFP_ATOMIC);
  if (!hostcard)
    {
      result = -ENOMEM;
      goto fail;
    }
  memset(hostcard, 0, hvcard_pcie_nr_devs*sizeof(ULONG));

  for (i=0; i< hvcard_pcie_nr_devs; i++)
    {
      hostcard[i] = kmalloc(sizeof(hvcard_pcie_card), GFP_ATOMIC);
      if (!hostcard[i])
        {
          result = -ENOMEM;
          while (i--) kfree(hostcard[i]);
          goto fail_hostcard;
        }
      memset(hostcard[i], 0, sizeof(hvcard_pcie_card));
      datasize += sizeof(hvcard_pcie_card);
    }

  if (hvcard_pcie_debug)
    {
      printk(KERN_INFO PFX "hostcard list size - %ld bytes\n", hvcard_pcie_nr_devs*sizeof(ULONG));
      printk(KERN_INFO PFX "all hostcards data - %d bytes\n", datasize);
    }


  /* Allocate static buffer for HV data from one hostcard */
  buf = kmalloc(MAX_HVDATA_SIZE, GFP_ATOMIC);
  if (!buf)
    {
      result = -ENOMEM;
      goto fail_buf;
    }
  memset(buf, 0, MAX_HVDATA_SIZE);


  /* Allocate buffer for monitoring history data */
  histdata = kmalloc(hvcard_pcie_nr_devs*MAX_CONTROL_LINES*HVCARD_MAX_MODULES*sizeof(ULONG), GFP_ATOMIC);
  if (!histdata)
    {
      result = -ENOMEM;
      goto fail_hist;
    }
  memset(histdata, 0, hvcard_pcie_nr_devs*MAX_CONTROL_LINES*sizeof(ULONG));

  datasize = 0;
  for (i=0; i< hvcard_pcie_nr_devs*MAX_CONTROL_LINES*HVCARD_MAX_MODULES; i++)
    {
      histdata[i] = vmalloc(sizeof(HVhistmodule));
      if (!histdata[i])
        {
          result = -ENOMEM;
          while (i--) vfree(histdata[i]);
          goto fail_histdata;
        }
      memset(histdata[i], 0, sizeof(HVhistmodule));
      datasize += sizeof(HVhistmodule);
    }

//  if (hvcard_debug)
  {
    printk(KERN_INFO PFX "histdata list size - %ld bytes\n", hvcard_pcie_nr_devs*sizeof(ULONG));
    printk(KERN_INFO PFX "all history data - %d bytes\n", datasize);
  }

  

  result = pci_register_driver(&hvcard_pcie_driver);

  if (hvcard_pcie_skip_mod_init>0) printk(KERN_INFO PFX "Initial modules detection is skipped\n");

  if (result == 0)
    {

      /* Ramp Up/Down Timer */
      init_timer(&ramp_timer);
      ramp_timer.expires = RUN_AT(start_delay*HZ);
      ramp_timer.data = 0L;
      ramp_timer.function = hvcard_pcie_ramp_timer;
      add_timer(&ramp_timer);

      /* Monitoring timer */
      init_timer(&watch_timer);
      watch_timer.expires = RUN_AT(start_delay*HZ);
      watch_timer.data = 0L;
      watch_timer.function = hvcard_pcie_watch_timer;
      add_timer(&watch_timer);
      return result;
    }
  else
    {
        printk (KERN_ERR PFX "Unable to initialize PCIe hvcard driver. Check for physical card(s) presense.\n");
    }


//   return result;

fail_histdata:
   if (histdata) kfree(histdata);
fail_hist:

fail_buf:
  for (i=0; i<hvcard_pcie_nr_devs; i++)
    {
      if (hostcard[i]) kfree(hostcard[i]);
    }
fail_hostcard:
  if (hostcard) kfree(hostcard);
fail:
  unregister_chrdev(hvcard_pcie_major, DRV_MODULE_NAME);
  return result;
}

static void __exit hvcard_pcie_cleanup(void)
{
  int i;
  del_timer_sync(&watch_timer);
  del_timer_sync(&ramp_timer);

  unregister_chrdev(hvcard_pcie_major, DRV_MODULE_NAME);
  pci_unregister_driver(&hvcard_pcie_driver);

  for (i=0; i<hvcard_pcie_nr_devs*MAX_CONTROL_LINES*HVCARD_MAX_MODULES; i++)
    {
      if (histdata[i]) vfree(histdata[i]);
    }
  if (histdata)   kfree(histdata);

  if (buf)    kfree(buf);

  for (i=0; i<hvcard_pcie_nr_devs; i++)
    {
      if (hostcard[i]) kfree(hostcard[i]);
    }
  if (hostcard)   kfree(hostcard);
}

module_init(hvcard_pcie_init);
module_exit(hvcard_pcie_cleanup);

