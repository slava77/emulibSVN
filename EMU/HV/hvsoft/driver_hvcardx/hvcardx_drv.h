/*
 * linux/drivers/misc/hvcardx_drv.h
 *
 * Copyright 2011 Xillybus Ltd, http://hvcardx.com
 *
 * Header file for the Xillybus FPGA/host framework.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the smems of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

#ifndef __HVCARDX_H
#define __HVCARDX_H

#include <linux/list.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/pci.h>

#include "hv_types.h"
#include "hvcard_pcie.h"

#define RUN_AT(x) (jiffies + (x))

#define DRV_CLASS_NAME  "hvctl"
// #define DRV_MODULE_NAME "hvcardx"
#define PFX DRV_MODULE_NAME ": "

#define DRV_MODULE_VERSION  "2.00"
#define DRV_MODULE_RELDATE  "Jul 15, 2014"

static char VARIABLE_IS_NOT_USED version[] __devinitdata =
DRV_MODULE_NAME ": v" DRV_MODULE_VERSION " (" DRV_MODULE_RELDATE ")";


/* General timeout is 100 ms, rx timeout is 10 ms */
#define HVCARDX_RX_TIMEOUT (10*HZ/1000)
#define HVCARDX_TIMEOUT (100*HZ/1000)

#define fpga_msg_ctrl_reg 0x0002
#define fpga_dma_control_reg 0x0008
#define fpga_dma_bufno_reg 0x0009
#define fpga_dma_bufaddr_lowaddr_reg 0x000a
#define fpga_dma_bufaddr_highaddr_reg 0x000b
#define fpga_buf_ctrl_reg 0x000c
#define fpga_buf_offset_reg 0x000d
#define fpga_endian_reg 0x0010

#define HVCARDXMSG_OPCODE_RELEASEBUF 1
#define HVCARDXMSG_OPCODE_QUIESCEACK 2
#define HVCARDXMSG_OPCODE_FIFOEOF 3
#define HVCARDXMSG_OPCODE_FATAL_ERROR 4
#define HVCARDXMSG_OPCODE_NONEMPTY 5

struct hvcardx_endpoint_hardware;

struct hvcardx_page {
	struct list_head node;
	unsigned long addr;
	unsigned int order;
};

struct hvcardx_dma {
	struct list_head node;
	struct pci_dev *pdev;
	struct device *dev;
	dma_addr_t dma_addr;
	size_t size;
	int direction;
};

struct hvcardx_buffer {
	void *addr;
	dma_addr_t dma_addr;
	int end_offset; /* Counting elements, not bytes */
};

struct hvcardx_cleanup {
	struct list_head to_kfree;
	struct list_head to_pagefree;
	struct list_head to_unmap;
};

struct hvcardx_idt_handle {
	unsigned char *chandesc;
	unsigned char *idt;
	int entries;
};

/*
 * Read-write confusion: wr_* and rd_* notation sticks to FPGA view, so
 * wr_* buffers are those consumed by read(), since the FPGA writes to them
 * and vice versa.
 */

struct hvcardx_channel {
	struct hvcardx_endpoint *endpoint;
	int chan_num;
	int log2_element_size;
	int seekable;

	struct hvcardx_buffer **wr_buffers; /* FPGA writes, driver reads! */
	int num_wr_buffers;
	unsigned int wr_buf_size; /* In bytes */
	int wr_fpga_buf_idx;
	int wr_host_buf_idx;
	int wr_host_buf_pos;
	int wr_empty;
	int wr_ready; /* Significant only when wr_empty == 1 */
	int wr_sleepy;
	int wr_eof;
	int wr_hangup;
	spinlock_t wr_spinlock;
	struct mutex wr_mutex;
	wait_queue_head_t wr_wait;
	wait_queue_head_t wr_ready_wait;
	int wr_ref_count;
	int wr_synchronous;
	int wr_allow_partial;
	int wr_exclusive_open;
	int wr_supports_nonempty;

	struct hvcardx_buffer **rd_buffers; /* FPGA reads, driver writes! */
	int num_rd_buffers;
	unsigned int rd_buf_size; /* In bytes */
	int rd_fpga_buf_idx;
	int rd_host_buf_pos;
	int rd_host_buf_idx;
	int rd_full;
	spinlock_t rd_spinlock;
	struct mutex rd_mutex;
	wait_queue_head_t rd_wait;
	int rd_ref_count;
	int rd_allow_partial;
	int rd_synchronous;
	int rd_exclusive_open;
	struct delayed_work rd_workitem;
	unsigned char rd_leftovers[4];
};

struct hvcardx_endpoint {
	/*
	 * One of pdev and dev is always NULL, and the other is a valid
	 * pointer, depending on the type of device
	 */
	struct pci_dev *pdev;
	struct device *dev;
	struct resource res; /* OF devices only */
	struct hvcardx_endpoint_hardware *ephw;

	struct list_head ep_list;
	int dma_using_dac; /* =1 if 64-bit DMA is used, =0 otherwise. */
	__iomem u32 *registers;
	int fatal_error;

	struct mutex register_mutex;
	wait_queue_head_t ep_wait;

	/* List of memory allocations, to make release easy */
	struct hvcardx_cleanup cleanup;

	/* Channels and message handling */
	struct cdev cdev;

	int major;
	int lowest_minor; /* Highest minor = lowest_minor + num_channels - 1 */

	int num_channels; /* EXCLUDING message buffer */
	struct hvcardx_channel **channels;
	int msg_counter;
	int failed_messages;
	int idtlen;

	u32 *msgbuf_addr;
	dma_addr_t msgbuf_dma_addr;
	unsigned int msg_buf_size;
};

struct hvcardx_endpoint_hardware {
	struct module *owner;
	void (*hw_sync_sgl_for_cpu)(struct hvcardx_endpoint *,
				    dma_addr_t,
				    size_t,
				    int);
	void (*hw_sync_sgl_for_device)(struct hvcardx_endpoint *,
				       dma_addr_t,
				       size_t,
				       int);
	dma_addr_t (*map_single)(struct hvcardx_cleanup *,
				 struct hvcardx_endpoint *,
				 void *,
				 size_t,
				 int);
	void (*unmap_single)(struct hvcardx_dma *entry);
};

irqreturn_t hvcardx_isr(int irq, void *data);

void hvcardx_do_cleanup(struct hvcardx_cleanup *mem,
			 struct hvcardx_endpoint *endpoint);

struct hvcardx_endpoint *hvcardx_init_endpoint(struct pci_dev *pdev,
					      struct device *dev,
					      struct hvcardx_endpoint_hardware
					      *ephw);

int hvcardx_endpoint_discovery(struct hvcardx_endpoint *endpoint);

void hvcardx_endpoint_remove(struct hvcardx_endpoint *endpoint);


///* HV related stuff

extern struct pci_driver hvcardx_driver;

extern UINT hvcard_pcie_debug;
extern UINT hvcard_pcie_found;
extern UINT hvcard_pcie_nr_devs;
extern UINT hvcard_pcie_hostid;
extern UINT hvcard_pcie_settime; // 200ms
extern UINT hvcard_pcie_update;   // 20ms
extern UINT hvcard_pcie_state;
extern UINT hvcard_pcie_ramp_up;
extern UINT hvcard_pcie_ramp_down;
extern UINT hvcard_pcie_imax;
extern UINT hvcard_pcie_imax_ramp;
extern UINT hvcard_pcie_vmax;
extern UINT hvcard_pcie_vset;
extern UINT hvcard_pcie_trip_delay;
extern UINT hvcard_pcie_master_trip_delay;
extern UINT hvcard_pcie_ilock_delay;
extern UINT hvcard_pcie_trip_reset_delay;
extern UINT hvcard_pcie_trip_max_resets;
extern UINT hvcard_pcie_ramp_steps; // Ramp timer  1sec/hvcard_ramp_steps
extern UINT hvcard_pcie_policy;
extern UINT hvcard_pcie_rampdown_set_delay;
extern UINT hvcard_pcie_skip_mod_init;


extern UINT busy;
extern UINT sum_status;
extern UINT do_detect;
extern UINT ramp_cnt;

//  -- Reset Module
inline void reset_module(HVmodule *m);
//  -- Get Module ID
inline int get_module_id(HVmodule *m);
//  -- Check Master module ID
inline int check_master_id(HVmodule *m);
//  -- Check if module ID is valid
inline int is_valid_id(HVmodule *m, int ID);
//  -- Check if module channel is in ramping state (including check of primary and master statuses)
inline int is_ramping(HVmodule *m, int ch);
//  -- Check that Master module channel is assigned to RDB module
inline int master_assigned(HVmodule *m);
//  -- Check that module is RDB type
inline int is_RDB(HVmodule *m);
//  -- Get Module's type
inline int get_module_type(HVmodule *m);
//  -- Read voltage from monitoring channel of RDB module
inline int get_hvmon(HVmodule *m);
//  -- Read module's interlock state
inline int get_interlock_state(HVmodule *m);
//  -- Read module's +5V status
inline int get_pos5vmon(HVmodule *m);
//  -- Read module's -5V status
inline int get_neg5vmon(HVmodule *m);
//  -- Check module's +5V status
inline int check_pos5vmon(HVmodule *m);
//  -- Check module's -5V status
inline int check_neg5vmon(HVmodule *m);

//  ==> HV Module initialization 
extern inline void hvcardx_init_one_module(HVmodule *m);
extern inline int hvcardx_init_modules(hvcard_pcie_card *card);


int hvcardx_ioctl(struct inode *inode, struct file *filp, UINT cmd, ULONG arg);
int hvcardx_exec_cmd(ULONG pusercmd);

int hvcardx_open_dma_channel(int card);
int hvcardx_close_dma_channel(int card);
int hvcardx_read_adc_data(int card);
int hvcardx_write_dac_data(int card);

extern spinlock_t hvdata_lock;

//  ==> Main watch timer loop
extern struct timer_list watch_timer;
extern void hvcardx_watch_timer(unsigned long  data);

//  ==> Ramp timer
extern struct timer_list ramp_timer;
extern void hvcardx_ramp_timer(unsigned long data);

//  ==> DMA workqueues 
extern struct workqueue_struct *hvcardx_wq;
extern int hvcardx_myflush(struct hvcardx_channel *channel, long timeout);

extern struct workqueue_struct *hvcardx_dma_rq;
extern struct delayed_work dma_adc_read_work;

extern struct workqueue_struct *hvcardx_dma_wq;
extern struct delayed_work dma_dac_write_work;

extern struct mutex dma_op_mutex;


typedef struct perf_stats
{
  ktime_t start;
  ktime_t end;
  uint64_t cnt;
  s64 sum_time;
  s64 cur_time;
  s64 min_time;
  s64 max_time;
  s64 avg_time;
} perf_cnt;


typedef union
{
  unsigned long longword;
  unsigned char bytes[4];
} union_rnd_data;

extern HVcmd * hv_cmd;
// extern UINT *buf;
extern hvcard_pcie_card **hostcard;  // one hvcard_pcie card entry
extern HVhistmodule **histdata;

#endif /* __HVCARDX_H */
