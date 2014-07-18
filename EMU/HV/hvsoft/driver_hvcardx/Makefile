# Makefile for HV Card Xillybus-based drivers
#
# Will compile and install for other kernel than the currently running,
# given the TARGET parameter (should be the name of a directory in
# /lib/modules) e.g.
# make TARGET=3.10.0-CUSTOM
#
# This makefile compiles the drivers for HV Card Xillybus-based using PCIe. For Device Tree
# (Open Firmware) support, uncomment where hvcardx_of is mentioned below
# (and possibly comment out occurrences of hvcardx_pcie).

ifneq ($(KERNELRELEASE),)
obj-m	:= hvcardx.o
#obj-m	+= hvcardx_pcie.o
# obj-m	+= hvcardx_of.o
#hvcardx_core-objs = \
	hvcardx_core.o

#hvcardx_pcie-objs = \
	hvcardx_pcie.o
hvcardx-objs = \
	hvcardx_consts.o \
	hvcardx_core.o \
	hvcardx_init_pcie.o \
	hvcardx_watch_timer.o \
	hvcardx_ramp_timer.o \
	hvcardx_ioctl.o \
	hvcardx_misc.o \
	hvcardx_init_modules.o \
	hvcardx_exec_cmd.o \
	hvcardx_dma.o

else
ifeq ($(TARGET),)
TARGET := $(shell uname -r)
endif
PWD := $(shell pwd)
KDIR := /lib/modules/$(TARGET)/build

default:
	@echo $(TARGET) > module.target
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	@rm -f *.ko *.o modules.order Module.symvers *.mod.? .hvcardx_*.* *~
	@rm -rf .tmp_versions module.target

install: hvcardx.ko
	install --mode 0744 hvcardx.ko /lib/modules/$(shell cat module.target)/kernel/drivers/misc/
#install: hvcardx_core.ko hvcardx_pcie.ko # hvcardx_of.ko
#	install --mode 0644 hvcardx_core.ko /lib/modules/$(shell cat module.target)/kernel/drivers/char/
#	install --mode 0644 hvcardx_pcie.ko /lib/modules/$(shell cat module.target)/kernel/drivers/char/
#	install --mode 0644 hvcardx_of.ko /lib/modules/$(shell cat module.target)/kernel/drivers/char/
	/sbin/depmod -a $(shell cat module.target)
hvcardx.ko:
	$(MAKE)
endif
