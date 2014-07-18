/*
 * linux/drivers/misc/hvcardx_pcie.c
 *
 * Copyright 2011 Xillybus Ltd, http://hvcardx.com
 *
 * Driver for the Xillybus FPGA/host framework using PCI Express.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the smems of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/pci-aspm.h>
#include <linux/slab.h>
#include "hvcardx_drv.h"

MODULE_DESCRIPTION("UF HV Card Xillybus-based driver for PCIe");
MODULE_AUTHOR("Victor Barashko, UF; Eli Billauer, Xillybus Ltd.");
MODULE_VERSION("1.00");
MODULE_ALIAS("hvcardx_pcie");
MODULE_LICENSE("GPL v2");

#define PCI_DEVICE_ID_HVCARDX		0xebeb

#define PCI_VENDOR_ID_ALTERA		0x1172
#define PCI_VENDOR_ID_ACTEL		0x11aa
#define PCI_VENDOR_ID_LATTICE		0x1204

static const char hvcardxname[] = DRV_MODULE_NAME;

static const struct pci_device_id hvcardxids[] =
{
  {PCI_DEVICE(PCI_VENDOR_ID_XILINX, PCI_DEVICE_ID_HVCARDX)},
  {PCI_DEVICE(PCI_VENDOR_ID_ALTERA, PCI_DEVICE_ID_HVCARDX)},
  {PCI_DEVICE(PCI_VENDOR_ID_ACTEL, PCI_DEVICE_ID_HVCARDX)},
  {PCI_DEVICE(PCI_VENDOR_ID_LATTICE, PCI_DEVICE_ID_HVCARDX)},
  { /* End: all zeroes */ }
};

static int hvcardx_pci_direction(int direction)
{
  switch (direction)
    {
    case DMA_TO_DEVICE:
      return PCI_DMA_TODEVICE;
    case DMA_FROM_DEVICE:
      return PCI_DMA_FROMDEVICE;
    default:
      return PCI_DMA_BIDIRECTIONAL;
    }
}

static void hvcardx_dma_sync_single_for_cpu_pci(struct hvcardx_endpoint *ep,
    dma_addr_t dma_handle,
    size_t size,
    int direction)
{
  pci_dma_sync_single_for_cpu(ep->pdev,
                              dma_handle,
                              size,
                              hvcardx_pci_direction(direction));
}

static void hvcardx_dma_sync_single_for_device_pci(struct hvcardx_endpoint *ep,
    dma_addr_t dma_handle,
    size_t size,
    int direction)
{
  pci_dma_sync_single_for_device(ep->pdev,
                                 dma_handle,
                                 size,
                                 hvcardx_pci_direction(direction));
}

/*
 * Map either through the PCI DMA mapper or the non_PCI one. Behind the
 * scenes exactly the same functions are called with the same parameters,
 * but that can change.
 */

static dma_addr_t hvcardx_map_single_pci(struct hvcardx_cleanup *mem,
    struct hvcardx_endpoint *ep,
    void *ptr,
    size_t size,
    int direction
                                        )
{

  dma_addr_t addr = 0;
  struct hvcardx_dma *this;
  int pci_direction;

  this = kmalloc(sizeof(struct hvcardx_dma), GFP_KERNEL);
  if (!this)
    return 0;

  pci_direction = hvcardx_pci_direction(direction);
  addr = pci_map_single(ep->pdev, ptr, size, pci_direction);
  this->direction = pci_direction;

  if (pci_dma_mapping_error(ep->pdev, addr))
    {
      kfree(this);
      return 0;
    }

  this->dma_addr = addr;
  this->pdev = ep->pdev;
  this->size = size;

  list_add_tail(&this->node, &mem->to_unmap);

  return addr;
}

static void hvcardx_unmap_single_pci(struct hvcardx_dma *entry)
{
  pci_unmap_single(entry->pdev,
                   entry->dma_addr,
                   entry->size,
                   entry->direction);
}

static struct hvcardx_endpoint_hardware pci_hw =
{
  .owner = THIS_MODULE,
  .hw_sync_sgl_for_cpu = hvcardx_dma_sync_single_for_cpu_pci,
  .hw_sync_sgl_for_device = hvcardx_dma_sync_single_for_device_pci,
  .map_single = hvcardx_map_single_pci,
  .unmap_single = hvcardx_unmap_single_pci
};

static int __devinit hvcardx_probe(struct pci_dev *pdev,
                                   const struct pci_device_id *ent)
{
  int i,j;
  struct hvcardx_endpoint *endpoint;
  struct hvcard_pcie_card *dev;
  int revision;
  int rc = 0;


  endpoint = hvcardx_init_endpoint(pdev, &pdev->dev, &pci_hw);

  if (!endpoint)
    return -ENOMEM;

  pci_read_config_dword(pdev, PCI_REVISION_ID, &revision);
  revision &= 0xFF;
  // if (hvcardx_debug)
  {
    dev_info(endpoint->dev, "vendor: 0x%X, device: 0x%X, revision: %d, bus: %i (%s)\n", 
		pdev->vendor, pdev->device, revision, pdev->bus->number,pdev->bus->name);
//    dev_info(endpoint->dev, "bus: %i (%s)\n", pdev->bus->number,pdev->bus->name);
//    dev_info(endpoint->dev, "slot: %i (%s)\n",PCI_SLOT(pdev->devfn) ,pci_name(pdev));
//    dev_info(endpoint->dev, "devfn: 0x%0X\n", pdev->devfn);
    dev_info(endpoint->dev, "hostid: %d\n", hvcard_pcie_hostid);
  }

  //pci_set_drvdata(pdev, endpoint);

  rc = pci_enable_device(pdev);

  /* L0s has caused packet drops. No power saving, thank you. */

  pci_disable_link_state(pdev, PCIE_LINK_STATE_L0S);

  if (rc)
    {
      dev_err(endpoint->dev,
              "pci_enable_device() failed. Aborting.\n");
      goto no_enable;
    }

  if (!(pci_resource_flags(pdev, 0) & IORESOURCE_MEM))
    {
      dev_err(endpoint->dev,
              "Incorrect BAR configuration. Aborting.\n");
      rc = -ENODEV;
      goto bad_bar;
    }

  rc = pci_request_regions(pdev, hvcardxname);
  if (rc)
    {
      dev_err(endpoint->dev,
              "pci_request_regions() failed. Aborting.\n");
      goto failed_request_regions;
    }

  endpoint->registers = pci_iomap(pdev, 0, 128);
  if (!endpoint->registers)
    {
      dev_err(endpoint->dev, "Failed to map BAR 0. Aborting.\n");
      rc = -EIO;
      goto failed_iomap0;
    }

  pci_set_master(pdev);

  //** HV parts

  dev = kzalloc(sizeof(struct hvcard_pcie_card), GFP_ATOMIC);
  if (!dev)
    {
      dev_err(endpoint->dev, "HVCard device data structure memory allocation failed, aborting.\n");
      rc = -ENOMEM;
      goto err_out_free_res;
    }

  dev->pdev = pdev;
  dev->ep = endpoint;
  dev->hostid = hvcard_pcie_hostid;
  dev->bus = pdev->bus->number;
  dev->slot = pdev->devfn>>3;
  dev->revision = revision;

  dev->dataslot = NA;
  for (i=0; i<hvcard_pcie_nr_devs; i++)
    {
      if (!hostcard[i]->pdev)
        {
          dev->dataslot = i;
          for (j=0; j<MAX_CONTROL_LINES; j++)
            {
              dev->link[j].dataslot = i*MAX_CONTROL_LINES+j;
              dev->link[j].pdev = pdev;
              dev->link[j].hostid = hvcard_pcie_hostid;
              dev->link[j].bus = pdev->bus->number;
              dev->link[j].slot = i*MAX_CONTROL_LINES+j;
              dev->link[j].revision = revision;
              dev->link[j].ioaddr = (ULONG)(dev->adcdata);
            }
          break;
        }
    }

  if (dev->dataslot == NA)
    {
      dev_err(endpoint->dev, "Unable to allocate data slot for device pci:%d:%d. "
              "Check SLOTS parameter in /etc/hvcard_pcie.cfg\n", dev->bus, dev->slot);
      rc = -ENOMEM;
      goto err_dev_cfg;
    }

//   memset(dev->dacdata, 0, sizeof(dev->dacdata));
//  dev_info(endpoint->dev, "dac data dma buffer size: %ld bytes\n", sizeof(dev->dacdata));
//   memset(dev->adcdata, 0, sizeof(dev->adcdata));
//  dev_info(endpoint->dev, "adc data dma buffer size: %ld bytes\n", sizeof(dev->adcdata));


  memcpy(hostcard[dev->dataslot], dev, sizeof(*dev));

  if (hvcard_pcie_debug)
    dev_info(endpoint->dev, "data slot:%i allocated\n",dev->dataslot);

  hvcard_pcie_found++;
  dev_info(endpoint->dev, "pci:%d:%d dataslot:%d rev.%d installed\n",
           dev->bus, dev->slot, dev->dataslot, dev->revision );

  kfree(dev);

  pci_set_drvdata(pdev, endpoint);



  /* Set up a single MSI interrupt */
  if (pci_enable_msi(pdev))
    {
      dev_err(endpoint->dev,
              "Failed to enable MSI interrupts. Aborting.\n");
      rc = -ENODEV;
      goto failed_enable_msi;
    }
  rc = request_irq(pdev->irq, hvcardx_isr, 0, hvcardxname, endpoint);

  if (rc)
    {
      dev_err(endpoint->dev,
              "Failed to register MSI handler. Aborting.\n");
      rc = -ENODEV;
      goto failed_register_msi;
    }

  /*
   * In theory, an attempt to set the DMA mask to 64 and dma_using_dac=1
   * is the right thing. But some unclever PCIe drivers report it's OK
   * when the hardware drops those 64-bit PCIe packets. So trust
   * nobody and use 32 bits DMA addressing in any case.
   */

  if (!pci_set_dma_mask(pdev, DMA_BIT_MASK(32)))
    endpoint->dma_using_dac = 0;
  else
    {
      dev_err(endpoint->dev, "Failed to set DMA mask. Aborting.\n");
      rc = -ENODEV;
      goto failed_dmamask;
    }

  rc = hvcardx_endpoint_discovery(endpoint);

  if (!rc)
    return 0;

failed_dmamask:
  free_irq(pdev->irq, endpoint);
failed_register_msi:
  pci_disable_msi(pdev);
failed_enable_msi:
err_dev_cfg:
  kfree(pdev);
  //  pci_clear_master(pdev); Nobody else seems to do this * /

err_out_free_res:
  pci_iounmap(pdev, endpoint->registers);
failed_iomap0:
  pci_release_regions(pdev);
failed_request_regions:
bad_bar:
  pci_disable_device(pdev);
no_enable:
  hvcardx_do_cleanup(&endpoint->cleanup, endpoint);

  kfree(endpoint);
  return rc;
}

static void __devexit hvcardx_remove(struct pci_dev *pdev)
{
  int i;
  struct hvcardx_endpoint *endpoint = pci_get_drvdata(pdev);
  UINT dataslot, bus, slot;
  struct hvcard_pcie_card *dev = NULL;


  for (i=0; i< hvcard_pcie_nr_devs; i++)
    {
      if (hostcard[i]->ep == endpoint)
        {
          dev = hostcard[i];
        }
    }

  if (dev!=NULL)
    {
      // * Set HV data to 0 * /
      // hvcard_pcie_init_modules(dev);
      dataslot = dev->dataslot;
      bus = dev->bus;
      slot = dev->slot;
      if ((dev->dataslot >=0) && (dev->dataslot < hvcard_pcie_nr_devs))
        {
          memset(hostcard[dev->dataslot],0,sizeof(*dev));
          if (hvcard_pcie_debug)
            dev_info(endpoint->dev, "data slot:%i released\n", dev->dataslot);
        }
      else
        {
          dev_err(endpoint->dev, "invalid data slot %i\n", dev->dataslot);
        }
      dev_info(endpoint->dev, "pci:%d:%d dataslot:%d removed\n", bus, slot, dataslot);
    }

  hvcardx_endpoint_remove(endpoint);

  free_irq(pdev->irq, endpoint);

  pci_disable_msi(pdev);
  pci_iounmap(pdev, endpoint->registers);
  pci_release_regions(pdev);
  pci_disable_device(pdev);

  hvcardx_do_cleanup(&endpoint->cleanup, endpoint);

  kfree(endpoint);
  hvcard_pcie_found--;
}

MODULE_DEVICE_TABLE(pci, hvcardxids);

// static struct pci_driver hvcardx_driver = {
struct pci_driver hvcardx_driver =
{
  .name = DRV_MODULE_NAME,
  .id_table = hvcardxids,
  .probe = hvcardx_probe,
  .remove = hvcardx_remove,
};

/*
static int __init hvcardx_pcie_init(void)
{
	return pci_register_driver(&hvcardx_driver);
}

static void __exit hvcardx_pcie_exit(void)
{
	pci_unregister_driver(&hvcardx_driver);
}

module_init(hvcardx_pcie_init);
module_exit(hvcardx_pcie_exit);
*/
