#ifndef _HVCARD_PCIE_DRV_H
#define _HVCARD_PCIE_DRV_H

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/capability.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/ioctl.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/random.h>
#include <linux/spinlock.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include "hvcard_pcie.h"

#define DRV_MODULE_NAME   "hvcard_pcie"
#define PFX DRV_MODULE_NAME ": "
#define DRV_MODULE_VERSION  "1.00"
#define DRV_MODULE_RELDATE  "Dec 20, 2013"

static char VARIABLE_IS_NOT_USED version[] __devinitdata =
DRV_MODULE_NAME ".c:v" DRV_MODULE_VERSION " (" DRV_MODULE_RELDATE ")";


#define RUN_AT(x) (jiffies + (x))

extern UINT hvcard_pcie_major; // Dynamic allocation of MAJOR number
extern UINT hvcard_pcie_debug; // Debugging info
extern UINT hvcard_pcie_found;
extern UINT hvcard_pcie_nr_devs;
extern char hvcard_pcie_string[255];
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
extern UINT hvcard_pcie_enable_dma;
extern UINT hvcard_pcie_policy;
extern UINT hvcard_pcie_skip_mod_init;
extern UINT hvcard_pcie_disabled_list[];
extern UINT hvcard_pcie_rampdown_set_delay;
extern UINT start_delay;  // 1secs timers start delay
extern ULONG exectime;
extern UINT busy;
extern UINT do_detect;
extern UINT ramp_cnt;
extern UINT sum_status;

extern spinlock_t hvdata_lock;


//  ==> PCI Initialization and cleanup
extern struct pci_driver hvcard_pcie_driver;
// <==
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
extern inline void hvcard_pcie_init_one_module(HVmodule *m);
extern inline void hvcard_pcie_init_modules(hvcard_pcie_card *card);

//  ==> HV misc support functions
//  -- Loopback Test
unsigned long loopback_test(hvcard_pcie_card *card);
//  -- Memory I/O performance Test
unsigned long memperf_test(hvcard_pcie_card *card);
//  -- Start DMA transfer
inline ssize_t start_dma_transfer(int card);

// ==> File operations
int hvcard_pcie_open (struct inode *inode, struct file *filp);
int hvcard_pcie_release(struct inode *inode, struct file *filp);
ssize_t hvcard_pcie_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t hvcard_pcie_write(struct file *filp, const char *buf, size_t count,loff_t *f_pos);
int hvcard_pcie_ioctl(struct inode *inode, struct file *filp, UINT cmd, ULONG arg);

extern struct file_operations hvcard_pcie_fops;
// <==

//  ==> User defined ioctl commands
extern int hvcard_pcie_exec_cmd(ULONG pusercmd);
// <==

//  ==> Main watch timer loop
extern struct timer_list watch_timer;
extern void hvcard_pcie_watch_timer(unsigned long  data);
// <==

//  ==> Ramp timer
extern struct timer_list ramp_timer;
extern void hvcard_pcie_ramp_timer(unsigned long data);

inline void hvcard_set(struct HVmodule *m);
inline void hvcard_update_dac_values(int card);
// <==



// Data structure for DMA transfer handling
typedef struct hvcard_pcie_dma {
   dma_addr_t           dma_addr;
   UINT*                buf;
} hvcard_pcie_dma;

typedef union
{
  unsigned long longword;
  unsigned char bytes[4];
} union_rnd_data;

extern HVcmd * hv_cmd;
extern UINT *buf;
extern hvcard_pcie_card **hostcard;  // one hvcard_pcie card entry
extern HVhistmodule **histdata;

#endif
