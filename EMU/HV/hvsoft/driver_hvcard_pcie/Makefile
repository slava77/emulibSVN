#
# Makefile for hvcard_pcie driver
#
ifneq ($(KERNELRELEASE),)
obj-m:=hvcard_pcie.o
hvcard_pcie-objs = \
	hvcard_pcie_consts.o \
	hvcard_pcie_fops.o \
	hvcard_pcie_init_modules.o \
	hvcard_pcie_init_drv.o \
	hvcard_pcie_init_pci.o \
        hvcard_pcie_misc.o \
	hvcard_pcie_exec_cmd.o \
	hvcard_pcie_watch_timer.o \
	hvcard_pcie_ramp_timer.o \
        hvcard_pcie_ioctl.o 

else 
KDIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

default:
	$(MAKE) -C $(KDIR) V=1 M=$(PWD) modules

clean:
	$(RM) .*.cmd *.mod.c *.o *.ko -r .tmp* 

endif
