/* hvcard_pcie UF PCIe microTCA SP Card device driver.
*
* Copyright (C) 2012 Victor Barashko (barvic@ufl.edu)
*/
#include "hvcard_pcie_drv.h"
#include <linux/timex.h>

struct file_operations hvcard_pcie_fops =
{
  .owner        = THIS_MODULE, 
  .read         = hvcard_pcie_read,
  .write        = hvcard_pcie_write,
  .ioctl	= hvcard_pcie_ioctl,
  .open         = hvcard_pcie_open,
  .release      = hvcard_pcie_release
};

int hvcard_pcie_open (struct inode *inode, struct file *filp)
{
  UINT dev = MINOR(inode->i_rdev);

  if ((dev >= hvcard_pcie_nr_devs) || (hostcard[dev]->pdev == 0) )
    return -ENODEV;

  filp->f_op = &hvcard_pcie_fops;
  filp->private_data = hostcard[dev];
  if (hvcard_pcie_debug)
    printk (KERN_INFO PFX "data slot:%i is opened by "
            "\"%s\" (pid %i)\n",dev, current->comm, current->pid);
  return 0;
}

int hvcard_pcie_release(struct inode *inode, struct file *filp)
{
  UINT dev = MINOR(inode->i_rdev);

  if ((dev >= hvcard_pcie_nr_devs) || (hostcard[dev]->pdev == 0) )
    return -ENODEV;

  filp->private_data = NULL;
  if (hvcard_pcie_debug)
    printk (KERN_INFO PFX "data slot:%i is released by "
            "\"%s\" (pid %i)\n", dev, current->comm, current->pid);
  return 0;
}



ssize_t hvcard_pcie_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
  ssize_t ret = 0;
  UINT *data = NULL;
  uint64_t ioaddr_reg = 0;
  hvcard_pcie_card *dev = NULL;
  dma_addr_t bmaddr;
  uint32_t treg;

  if (!filp->private_data)
    {
      printk(KERN_ERR PFX "Cannot find data slot\n");
      return -EFAULT;
    }

  // only single 32-bit or block 64-bit word transfers supported at this time
  if (count != 4 && count % 8 != 0)
    {
      printk (KERN_ERR PFX "Invalid read count: %ld\n", count);
      return -EFAULT;
    }

  dev = filp->private_data;
  ioaddr_reg = dev->ioaddr[1]; 

  data = kmalloc(count, GFP_KERNEL);
  if (!data)
    {
      printk (KERN_ERR PFX "Failed to alloc read data buffer\n");
      return -ENOMEM;
    }


  if (count == 4)
    {
      // single register read
      treg = readl((uint32_t *)(ioaddr_reg + *f_pos));
      if (hvcard_pcie_debug) printk (KERN_INFO PFX "Register space read addr 0x%X: data: 0x%08X\n", *f_pos, treg);
      ret = count;
      if (copy_to_user(buf, &treg, count))
	{
	  printk (KERN_ERR PFX "Failed to copy data to user space\n");
	  ret = -EFAULT;
	}
    }
  else
    {
      // memory access
      if (hvcard_pcie_debug) printk (KERN_INFO PFX "Memory space read\n");

      // register offset
      uint64_t reg_base = (*f_pos) >> 32; // register base is passed in MSB of f_pos
      uint32_t from_adr = (*f_pos) & 0xffffffff; // "from" address is passed in LSB
      size_t rcount = 0;
      uint32_t rfrom_adr = 0;

      ioaddr_reg += reg_base; // correct register aperture to point at correct base
      // write beam length
      writel(count, (uint32_t *)(ioaddr_reg + 0)); 
      rcount =  readl((uint32_t *)(ioaddr_reg + 0));

      // write "from" address
      writel(from_adr, (uint32_t *)(ioaddr_reg + 4)); 
      rfrom_adr = readl((uint32_t *)(ioaddr_reg + 4));

      bmaddr = dev->dma->dma_addr; // physical memory address for dma transfer

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

      ret = count;
      if (copy_to_user(buf, dev->dma->buf, count))
	{
	  printk (KERN_ERR PFX "Failed to copy data to user space\n");
	  ret = -EFAULT;
	}
    }

  kfree(data);
  return ret;
}

ssize_t hvcard_pcie_write(struct file *filp, const char *buf, size_t count,loff_t *f_pos)
{
  ssize_t ret = 0;
  uint32_t *data = NULL;
  uint64_t ioaddr_mem = 0, ioaddr_reg = 0;
  hvcard_pcie_card *dev = NULL;
  int i;

  if (!filp->private_data)
    {
      printk(KERN_ERR PFX "Cannot find data slot\n");
      return -EFAULT;
    }

  // only single 32-bit or block 64-bit word transfers supported at this time
  if (count != 4 && count % 8 != 0)
    {
      printk (KERN_ERR PFX "Invalid write count: %ld\n", count);
      return -EFAULT;
    }

  dev = filp->private_data;
  ioaddr_mem = dev->ioaddr[0] + *f_pos;
  ioaddr_reg = dev->ioaddr[1] + *f_pos; 

  data  = kmalloc(count, GFP_KERNEL);
  if (!data)
    {
      printk (KERN_ERR PFX "Failed to alloc write data buffer\n");
      return -ENOMEM;
    }

  ret = count;
  if (copy_from_user(data, buf, count))
    {
      printk (KERN_ERR PFX "Failed to copy data from user space\n");
      ret = -EFAULT;
    }

  // write data to board

  if (count == 4)
    { 
      // single register write
      if (hvcard_pcie_debug) printk (KERN_INFO PFX "Register space write addr 0x%X: data: 0x%08X\n", *f_pos, ((uint32_t*)data)[0]);
      writel(((uint32_t*)data)[0], (uint32_t *)ioaddr_reg);
    }
  else
    {
      // memory access

      // using writeq transfers here to make sure all data packages are 64-bit aligned
      // root complex combines single writes into block transfers
      if (hvcard_pcie_debug) printk (KERN_INFO PFX "Memory space write\n");
      for (i = 0; i < count/8; i++)
	{
	  writeq(((uint64_t*)data)[i], ((uint64_t *)ioaddr_mem) + i);
	}
    }

  kfree(data);
  return ret;
}
