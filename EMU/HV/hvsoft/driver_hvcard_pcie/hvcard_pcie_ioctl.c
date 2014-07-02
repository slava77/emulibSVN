/* hvcard_pcie UF PCIe microTCA SP Card device driver.
*
* Copyright (C) 2012 Victor Barashko (barvic@ufl.edu)
*/
#include "hvcard_pcie_drv.h"

int hvcard_pcie_ioctl(struct inode *inode, struct file *filp, UINT cmd, ULONG arg)
{
  int err = 0;
  int ret = 0;
  hvcard_pcie_card *dev = NULL;
  UINT dataslot = MINOR(inode->i_rdev);

  if (!filp->private_data)
    {
      printk(KERN_ERR PFX "Cannot find data slot\n");
      return -EFAULT;
    }
  dev = filp->private_data;

  if ((dataslot >= hvcard_pcie_nr_devs) || (hostcard[dataslot]->pdev == 0) || (dataslot != dev->dataslot))
    return -ENODEV;

  if (_IOC_TYPE(cmd) != HVCARD_PCIE_IOC_MAGIC)
    {
      if (hvcard_pcie_debug)
        printk(KERN_ERR PFX "Invalid ioctl type %d\n",_IOC_TYPE(cmd));
      return -ENOTTY;
    }
  if (_IOC_NR(cmd) > HVCARD_PCIE_IOC_MAXNR)
    {
      if (hvcard_pcie_debug)
        printk(KERN_ERR PFX "Invalid ioctl command %d\n",_IOC_NR(cmd));
      return -ENOTTY;
    }
  if (_IOC_DIR(cmd) & _IOC_READ)
    err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
  else if (_IOC_DIR(cmd) & _IOC_WRITE)
    err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
  if (err) return -EFAULT;

  switch (cmd)
    {
    case HVCARD_PCIE_IOCRESET:
      break;
    case HVCARD_PCIE_IOQNRDEVS:
      return hvcard_pcie_nr_devs;
    case HVCARD_PCIE_IOQDEVSFOUND:
      return hvcard_pcie_found;
    case HVCARD_PCIE_IOQDEVBUS:
      return dev->pdev->bus->number;
    case HVCARD_PCIE_IOTDEBUG:
      if (!capable (CAP_SYS_ADMIN))
        return -EPERM;
      hvcard_pcie_debug = arg;
      printk(KERN_INFO PFX "Set hvcard_pcie_debug to %i\n", hvcard_pcie_debug);
      break;
    case HVCARD_PCIE_IOQDEBUG:
      return hvcard_pcie_debug;
    case HVCARD_PCIE_IOQHOSTID:
      return hvcard_pcie_hostid;
    case HVCARD_PCIE_IOQIOADDR:
      return dev->ioaddr[1];
    case HVCARD_PCIE_IOTSETTIME:
      if (!capable (CAP_SYS_ADMIN))
        return -EPERM;
      hvcard_pcie_settime = arg;
      printk(KERN_INFO PFX "hvcard_pcie set time delay is %ims\n", hvcard_pcie_settime);
      break;
    case HVCARD_PCIE_IOTUPDATE:
      if (!capable (CAP_SYS_ADMIN))
        return -EPERM;
      hvcard_pcie_update = arg;
      printk(KERN_INFO PFX "hvcard_pcie set update delay is %ims\n", hvcard_pcie_settime);
      break;
    case HVCARD_PCIE_IOXCMD:
      return hvcard_pcie_exec_cmd(arg);
    default:
      return -ENOTTY;
    }
  return ret;
}

