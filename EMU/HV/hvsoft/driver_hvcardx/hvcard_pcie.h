#ifndef _HVCARD_PCIE_H
#define _HVCARD_PCIE_H

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif

#include <linux/ioctl.h>
#include "hv_types.h"


// #define PCI_VENDOR_ID_XILINX    0x10ee // Xilinx Vendor ID 
// #define PCI_DEVICE_ID_HVCARD_PCIE      0x6021 // Xilinx uTCA Device ID 
// #define  PCI_DEVICE_ID_HVCARD_PCIE 	 0x7021
// #define  PCI_DEVICE_ID_HVCARD_PCIE_XILLY 0xebeb  // Xilliybus PCIe Core
// #define MAX_HVDATA_SIZE 	3072   // Maximum data buffer size in bytes for host card 
#define MAX_HVDATA_SIZE         49152   // Maximum data buffer size in bytes for host card 
#define MAX_ADC_DATA_SIZE	MAX_HVDATA_SIZE
#define MAX_DAC_DATA_SIZE	12288
#define MAX_DMA_READ_PAGE	8192	// crashes  with single transfer size more than 8192
#define ADC_DATA_OFFSET		0
#define DAC_DATA_OFFSET		0x20000
// #define MEM_BASE 0x40000 // 0xc0000
// #define REG_BASE 0x40ULL // 0x60ULL
#define ID_START                39 // Module ID starting channel 
#define ID_END                  48 // Module ID ending channel 
#define INTERLOCK               38 // Interlock status channel
#define MASTER_INTERLOCK        1  // Master Module Interlock status channel
#define HVMON                   37 // Distribution board HV Monitor channel
#define POS5VMON                38 // +5V channel
#define NEG5VMON                39 // -5V channel 
#define HVCARD_IOC_MAGIC        'H' 
#define HVCARD_MAX_MODULES      16 // Maximum modules controlled by one hostcard
#define MODULE_MAX_CHANS        48 // Maximum channels per HV module
#define DEF_RAMP_UP             4 // 4DAC/s default ramp-up speed
#define DEF_RAMP_DOWN           75 // 75DAC/s default ramp-down speed
#define DEF_MAX_CURRENT       1000      
#define DEF_MAX_RAMP_CURRENT  1000
#define DEF_MAX_VOLTAGE       4000
#define DEF_SET_VOLTAGE          0
#define DEF_TRIP_DELAY          200 // 200ms
#define DEF_OV_TRIP_DELAY       1000 // 1000ms
#define DEF_MASTER_TRIP_DELAY  2000 // 2000ms
#define DEF_ILOCK_DELAY         200 // 200ms
#define DEF_CHAN_TRIP_RESET_DELAY 30000 // 30sec
#define DEF_CHAN_MAX_TRIP_RESETS 0 // 1 retry
#define DEF_CHAN_OV_LIMIT       37 // ~50 V     
#define OV_MIN_VSET             2300 // Overvoltage trip check policy should start to work at RDB vset ~3000V (2300DAC)
#define DEF_CHAN_DEAD_MAX_CNT   20   // Number of channel DEAD state transitions before triggering TRIP

#define RDBALL_TYPE             249
#define RDB30_TYPE              250
#define RDB36_TYPE              251
#define MASTER_TYPE             252

#define CHAMB_1                 253 // First 18 channels for RDB36
#define CHAMB_2                 254 // Last 18 channels for RDB36
#define ALL                     255
#define NA                      -1
#define TRANSN_RANGE            50 // RDB Transition point skip range +/- Volts

#define MAX_CONTROL_LINES       10
#define DMA_BUF_SIZE            0x40000
#define DMA_INIT_OFFSET         0xFFC
#define LOOPBACK_ADDR           0x0

#define MAX_RAMP_UP             100

#define POLICY_DATAWRITE        0x1
#define POLICY_OVERVOLT         0x2
#define DEF_POLICY              0x0

#define MAX_HIST_BUF_SIZE       1500 // Length of channel monitoring history buffer in samples


#define HVCARD_PCIE_IOC_MAGIC        'H'

#define DRV_MODULE_NAME "hvcardx"


/*
// Xillybus PCIe FPGA registers
#define fpga_msg_ctrl_reg 0x0002
#define fpga_dma_control_reg 0x0008
#define fpga_dma_bufno_reg 0x0009
#define fpga_dma_bufaddr_lowaddr_reg 0x000a
#define fpga_dma_bufaddr_highaddr_reg 0x000b
#define fpga_buf_ctrl_reg 0x000c
#define fpga_buf_offset_reg 0x000d
#define fpga_endian_reg 0x0010
*/

typedef struct range {
        long start;
        long end;
        long len;
} range;

extern char hvcard_string[255];


// HV commands codes
// // NOTE: marked with '-' almost obsolete discrete data commands which 
// //      are not supported by HV DIM Server and should be replaced by calls to 
// //      read of hostcard's data block commands
typedef enum {
        HV_SET_OFF      = 0,            // + Turn Off module or channel
        HV_SET_ON       = 1,            // + Turn On module or channel
        HV_SET_LOCK     = 2,            // - Lock module or channel (obsolete)
        HV_SET_RAMP_UP  = 3,            // + Set Ramp Up rate
        HV_SET_RAMP_DOWN = 4,           // + Set Ramp Down rate
        HV_SET_VMAX     = 5,            // + Set maximum voltage output value
        HV_SET_IMAX     = 6,            // + Set maximum current limit (trip level)
        HV_SET_VSET     = 7,            // + Set output voltage
        HV_SET_STATE    = 8,            // + Set channel state
        HV_GET_VMON     = 9,            // - Read channel monitored voltage value
        HV_GET_IMON     = 10,           // - Read channel monitored current value
        HV_GET_RAMP_UP  = 11,           // - Read channel Ramp Up rate
        HV_GET_RAMP_DOWN = 12,          // - Read channel Ramp Down rate
        HV_GET_VMAX     = 13,           // - Read channel maximum voltage output value
        HV_GET_IMAX     = 14,           // - Read channel maximum current limit (trip level)
        HV_GET_VCUR     = 15,           // - Read internal vcur voltage value
        HV_GET_VSET     = 16,           // - Read set output voltage value
        HV_GET_STATE    = 17,           // - Read channel state
        HV_GET_STATUS   = 18,           // - Read channel status
        HV_GET_ITRIP    = 19,           // - Read trip current value
        HV_GET_VTRIP    = 20,           // - Read trip voltage value
        HV_GET_MOD_ADDR = 21,           // - Read module's bus address
        HV_GET_MOD_ID   = 22,           // - Read module's ID
        HV_GET_MOD_ILOCK = 23,          // - Read module's interlock state
        HV_GET_MOD_POS5V = 24,          // - Read module's +5V status
        HV_GET_MOD_NEG5V = 25,          // - Read module's -5V status
        HV_GET_MOD_TYPE = 26,           // - Read module's type
        HV_SET_MOD_TYPE = 27,           // + Set module's type
        HV_GET_MOD_NUM_CHANS = 28,      // - Read module's number of HV channels
        HV_GET_MOD_RAWDATA = 29,        // - 
        HV_SET_MOD_STATE = 30,          // + Set module state
        HV_GET_CARD_DATA = 31,          // + Read hostcard's data block from driver
        HV_GET_MOD_DATA = 32,           // + Read module's  data from driver (controls data file writing in Dim Server)
        HV_GET_CHAN_DATA = 33,          // - Read channel's data from driver 
        HV_SET_RELAY = 34,              // + Set relay state
        HV_GET_RELAY = 35,              // - Read relay state
        HV_GET_FUSE_STATUS = 36,        // - Read fuse status
        HV_RESET_TRIP = 37,             // + Reset trip state
        HV_SET_TRIP_DELAY = 38,         // + Set trip delay in ms
        HV_GET_TRIP_DELAY = 39,         // - Read set trip delay in ms
        HV_GET_TRIP_CNT = 40,           // - Read trip delay counter in ms
        HV_GET_MOD_HVMON = 41,          // - Read voltage from monitoring channel of RDB
        HV_SET_MASTER = 42,             // + Set linked Master information
        HV_GET_MASTER = 43,             // - Read linked Master information
        HV_SET_MASTER_TRIP_DELAY = 44,  // + Set linked master trip delay in ms
        HV_GET_MASTER_TRIP_DELAY = 45,  // - Read linked master trip delay in ms
        HV_SET_TRIP_RESET_DELAY = 46,   // + Set trip auto reset delay in ms
        HV_GET_TRIP_RESET_DELAY = 47,   // - Read set trip auto reset delay in ms
        HV_GET_TRIP_RESET_DELAY_CNT = 48,       // - Read trip auto reset delay counter in ms
        HV_SET_TRIP_MAX_RESETS = 49,    // + Set max number of auto trip resets
        HV_GET_TRIP_MAX_RESETS = 50,    // - Read set max number of auto trip resets
        HV_GET_TRIP_RESETS_CNT = 51,    // - Read auto trip resets counter
        HV_SET_ILOCK_DELAY = 52,        // + Set interlock and interface noise errors ignore delay in ms
        HV_GET_ILOCK_DELAY = 53,        // - Read set interlock and interface noise errors ignore delay in ms
        HV_GET_ILOCK_DELAY_CNT = 54,    // - Read interlock and interface noise errors ignore counter in ms
        HV_DETECT = 55,                 // Force Modules detection
        HV_SET_MOD_POLICY = 56,
        HV_GET_MOD_POLICY = 57,
        HV_UPDATE_CALIB = 58,
        HV_SET_OV_LIMIT = 59,           // Set OverVoltage limit
        HV_SET_OV_TRIP_DELAY = 60,      // Set OverVoltage trip delay
        HV_DO_CALIB = 61,
        HV_SET_RAMPDOWN_SET_DELAY = 62, // Set ramp down delay in seconds
        HV_SET_IMAX_RAMP = 63,          // Set trip level for ramp state
        HV_GET_IMAX_RAMP = 64,          // Get trip level for ramp state
        HV_SET_MOD_PRIMARY_STATUS = 65, // Set linked Primary PS status for module
        HV_SET_DEAD_MAX_CNT = 66,       // Set channel DEAD state max count before trip
        HV_GET_MOD_HIST_DATA = 67,      // Read Module monitoring history data
        HV_SET_GLOBAL_STATE = 68,       // Dim Server Set Global System state
        LAST_CMD
} HV_CMD;

#ifdef __KERNEL__
extern const char* hvcmd_str[];
#else 
static const char* hvcmd_str[LAST_CMD] = {
        "HV_SET_OFF",
        "HV_SET_ON",
        "HV_SET_LOCK",
        "HV_SET_RAMP_UP",
        "HV_SET_RAMP_DOWN",
        "HV_SET_VMAX",
        "HV_SET_IMAX",
        "HV_SET_VSET",
        "HV_SET_STATE",
        "HV_GET_VMON",
        "HV_GET_IMON",
        "HV_GET_RAMP_UP",
        "HV_GET_RAMP_DOWN",
        "HV_GET_VMAX",
        "HV_GET_IMAX",
        "HV_GET_VCUR",
        "HV_GET_VSET",
        "HV_GET_STATE",
        "HV_GET_STATUS",
        "HV_GET_ITRIP",
        "HV_GET_VTRIP",
        "HV_GET_MOD_ADDR",
        "HV_GET_MOD_ID",
        "HV_GET_MOD_ILOCK",
        "HV_GET_MOD_POS5V",
        "HV_GET_MOD_NEG5V",
        "HV_GET_MOD_TYPE",
        "HV_SET_MOD_TYPE",
        "HV_GET_MOD_NUM_CHANS",
        "HV_GET_MOD_RAWDATA",
        "HV_SET_MOD_STATE",
        "HV_GET_CARD_DATA",
        "HV_GET_MOD_DATA",
        "HV_GET_CHAN_DATA",
        "HV_SET_RELAY",
        "HV_GET_RELAY",
        "HV_GET_FUSE_STATUS",
        "HV_RESET_TRIP",
        "HV_SET_TRIP_DELAY",
        "HV_GET_TRIP_DELAY",
        "HV_GET_TRIP_CNT",
        "HV_GET_MOD_HVMON",
        "HV_SET_MASTER",
        "HV_GET_MASTER",
        "HV_SET_MASTER_TRIP_DELAY",
        "HV_GET_MASTER_TRIP_DELAY",
        "HV_SET_TRIP_RESET_DELAY",
        "HV_GET_TRIP_RESET_DELAY",
        "HV_GET_TRIP_RESET_DELAY_CNT",
        "HV_SET_TRIP_MAX_RESETS",
        "HV_GET_TRIP_MAX_RESETS",
        "HV_GET_TRIP_RESETS_CNT",
        "HV_SET_ILOCK_DELAY",
        "HV_GET_ILOCK_DELAY",
        "HV_GET_ILOCK_DELAY_CNT",
        "HV_DETECT",
        "HV_SET_MOD_POLICY",
        "HV_GET_MOD_POLICY",
        "HV_UPDATE_CALIB",
        "HV_SET_OV_LIMIT",
        "HV_SET_OV_TRIP_DELAY",
        "HV_DO_CALIB",
        "HV_SET_RAMPDOWN_SET_DELAY",
        "HV_SET_IMAX_RAMP",
        "HV_GET_IMAX_RAMP",
        "HV_SET_MOD_PRIMARY_STATUS",
        "HV_SET_DEAD_MAX_CNT",
        "HV_GET_MOD_HIST_DATA",
        "HV_SET_GLOBAL_STATE"
};

#endif


typedef enum {
        POLICY_WRITE_FILE = 0x1
} MOD_POLICIES;

/* HV Command Data structure */
typedef struct HVcmd {
        UINT   card;            // Hostcard's dataslot number (255 - all hostcards on this computer)
        UINT   module;          // Module's bus address (255 - all modules on this hostcard)
        UINT   chan;            // Channel number       (255 - all channels on this module)
        HV_CMD  cmd;            // HV_CMD command code
        ULONG   data;           // command data
        ULONG   size;           // size of data (ignored by driver)
} HVcmd;

extern HVcmd *hv_cmd;

#define HVCARD_PCIE_IOCRESET    _IO(HVCARD_PCIE_IOC_MAGIC,0)
#define HVCARD_PCIE_IOQNRDEVS        _IO(HVCARD_PCIE_IOC_MAGIC,1)
#define HVCARD_PCIE_IOQDEVSFOUND     _IO(HVCARD_PCIE_IOC_MAGIC,2)
#define HVCARD_PCIE_IOGDEVSLOT       _IOR(HVCARD_IOC_MAGIC,3,HVcmd)
#define HVCARD_PCIE_IOQDEVBUS        _IO(HVCARD_PCIE_IOC_MAGIC,4)
#define HVCARD_PCIE_IOTDEBUG         _IO(HVCARD_PCIE_IOC_MAGIC,5)
#define HVCARD_PCIE_IOQDEBUG            _IO(HVCARD_PCIE_IOC_MAGIC,6)
#define HVCARD_PCIE_IOQIOADDR        _IO(HVCARD_PCIE_IOC_MAGIC,7)
#define HVCARD_PCIE_IOTSETTIME  _IO(HVCARD_PCIE_IOC_MAGIC,8)
#define HVCARD_PCIE_IOTUPDATE   _IO(HVCARD_PCIE_IOC_MAGIC,9)
#define HVCARD_PCIE_IOXCMD           _IOWR(HVCARD_IOC_MAGIC,10, HVcmd)
#define HVCARD_PCIE_IOQHOSTID        _IO(HVCARD_IOC_MAGIC,11)
#define HVCARD_PCIE_IOCHARDRESET     _IO(HVCARD_IOC_MAGIC,15)

#define HVCARD_PCIE_IOC_MAXNR   15   

/* HV Modules Types */
typedef enum {
        NONE   = 0,
        MASTER = 1, // Master card
        RDB30  = 2, // Remote distribution board 30-channels 
        RDB36  = 3, // Remote distribution board 36-channels
        MON30  = 4, // Monitoring board for 30-channels 
        MON36  = 5  // Monitoring board for 36-channels
} HV_TYPE;

/* HV Status */
typedef enum {
        HV_STATE_OFF            = 0,    // Channel or module is turned Off
        HV_STATE_ON             = 1,    // Channel or module is turned On
        HV_STATE_RAMP_UP        = 2,    // Channel is ramping Up
        HV_STATE_RAMP_DOWN      = 3,    // Channel is ramping Down
        HV_STATE_DEAD           = 4,    // Channel is Dead
        HV_STATE_ILOCK          = 5,    // Module's Interlock 
        HV_STATE_INT_TRIP       = 6,    // Module detects Trip condition in one of channels
        HV_STATE_CHAN_TRIP      = 7,    // Trip condition in channel
        HV_STATE_OV_TRIP        = 8,    // Overvoltage Trip condition
        HV_STATE_SET            = 9     // Unused
} HV_STATE, HV_STATUS;

#ifdef __KERNEL__
extern const char* modtype_str[];
extern const UINT num_chans[];
extern const UINT* maps[];

extern const char* hvstate_str [];

#else

/* Default modules channel maps */
static const UINT  mapdef[48] = { 0, 1,  2,  3,  4,  5,  6,  7,  8,  9,
                                   10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                   20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                                   30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                                   40, 41, 42, 43, 44, 45, 46, 47};

static const UINT  map30[48] = {  0, 1,  2,  3,  4,  5,  6,  7,  16, 17,
                                   18, 19, 20, 21, 22, 8,  9,  10, 11, 12,
                                   13, 14, 15, 24, 25, 26, 27, 28, 29, 30,
                                   23, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                                   40, 41, 42, 43, 44, 45, 46, 47};

static const UINT  map36[48] = {  0, 1,  2,  3,  4,  5,  6,  7,  16, 17,
                                   18, 19, 20, 21, 22, 23, 32, 33, 8,  9,
                                   10, 11, 12, 13, 14, 15, 24, 25, 26, 27,
                                   28, 29, 30, 31, 35, 36, 34, 37, 38, 39,
                                   40, 41, 42, 43, 44, 45, 46, 47};

static const UINT  mapmon30[48] = {  28, 29,  30,  8,  9,  10,  6,  7,  16, 14,
                                   15, 24, 0, 1, 2, 31,  35,  36, 11, 12,
                                   13, 17, 18, 19, 25, 26, 27, 3, 4, 5,
                                   31, 32, 33, 34, 35, 36, 37, 38, 39,
                                   40, 41, 42, 43, 44, 45, 46, 47};

static const char* modtype_str[6] = { "None", "Master", "RDB 30", "RDB 36", "MON 30", "MON 36"};
static const UINT num_chans[6] = { 0, 8, 30, 36, 30, 36 };
static const UINT *maps[6] = { NULL, mapdef, map30, map36, mapmon30, map36};

static const char* hvstate_str [HV_STATE_SET+1] = {"OFF", "ON", "RAMP UP", "RAMP DOWN", "DEAD", "ILOCK", "INT TRIP", "CH INT TRIP", "CH OV TRIP", "SET" };

#endif

struct HVmodule;
struct HVchannel;
struct HVhostcard;
struct HVdma;
struct HVhistcard;
struct HVhistmodule;
struct HVhistchannel;

#pragma pack(push, 1)

/* HV Channel Data structure */
typedef struct HVchannel {
        struct HVmodule *module;// address of HVmodule data structure to which this channel assigned
        UINT vset;              // voltage value to set 
        UINT vset_adc;          // expected voltage value in adc counts
        UINT vmon;              // read voltage value
        UINT imon;              // read current value
        UINT vcur;              // voltage value during ramp 
        UINT ramp_up;           // voltage ramp up value
        UINT ramp_down;         // voltage ramp down value
        UINT vmax;              // maximum acceptable value for voltage
        UINT imax;              // maximum value for current (trip level)
        UINT imax_ramp;         // trip level for ramp up state
        UINT vov;               // relative to vset_adc overvoltage limit
        UINT itrip;             // current value when trip occured
        UINT vtrip;             // voltage value when trip occurred
        UINT vtrip_cnt;          // period of overvoltage pre-trip condition in ms
        UINT vtrip_delay;        // delay in ms before overvoltage trip state
        UINT trip_cnt;          // period of pre-trip condition in ms 
        UINT trip_delay;        // delay in ms before trip state                
        UINT trip_reset_delay;  // auto trip state reset delay in ms
        UINT trip_reset_delay_cnt;      // internal auto trip state reset delay counter
        UINT trip_max_resets;   // maximum number of allowed auto trip resets
        UINT trip_resets_cnt;   // internal counter of trip resets
        UINT dead_max_cnt;
        UINT dead_cnt;
        UINT ramp_counter;      // internal counter for smooth ramp downs
        UINT relay;             // state of relay for Distribution Board
        UINT fuse;              // state of fuse for Master Board
        UINT flags;
        HV_STATE state;         // channel state
        HV_STATUS status;       // channel status
} HVchannel;

/* HV Module Data structure */
typedef struct HVmodule {
struct HVhostcard       *hostcard;      // address of HVhostcard data structure to which this module assigned
        HV_TYPE         type;           // HV Module type
        ULONG           ioaddr;         // ignore (driver internals)
        UINT            ID;             // ID of HV module
        UINT            busaddr;        // module address on HV bus
        UINT            interlock;      // interlock status
        UINT            pos5v;          // status of +5V power 
        UINT            neg5v;          // status of -5V power
        UINT            hvmon;          // Voltage value on monitoring channel of RDB
        UINT            *map;           // ignore (driver internals)
        UINT            num_chans;      // number of HV channels on module
        UINT            master_id;      // ID of MB which is linked to this RDB
        UINT            master_hostid;  // Host ID of of linked MB
        UINT            master_hvcard;  // hostcard # of linked MB
        UINT            master_busaddr; // bus address of linked MB
        UINT            master_chan;    // MB channel which is linked to this RDB
        UINT            master_trip_delay;      // trip delay of linked MB in ms
        UINT            ilock_delay;    // module interlock delay in ms to suppress noise               
        UINT            ilock_delay_cnt;// internal interlock delay counter in ms
        HV_STATE        state;          // module state
        HV_STATUS       status;         // module status
        HV_STATUS       pr_status;      // linked Primary PS status
        UINT            policy;         // module policy bits
        HVchannel       chan[MODULE_MAX_CHANS];       // array of data for 48 channels on one module
} HVmodule;

typedef struct HVhostcard {
   struct pci_dev       *pdev;
   // struct hvcard_pcie_dma  *dma;
   UINT                 revision;
   ULONG                ioaddr;
   UINT                 hostid;
   UINT                 bus;
   UINT                 slot;
   UINT                 dataslot;
   struct HVmodule      module[HVCARD_MAX_MODULES];
} HVhostcard;

typedef struct HVhistchannel {
  USHORT                vmon[MAX_HIST_BUF_SIZE];        // Vmon values fuffer
  USHORT                imon[MAX_HIST_BUF_SIZE];        // Imon values buffer
  UINT                  trip_marker;                    // Trip position in buffer
  UINT                  pos;                            // Current position in buffer
} HVhistchan;

typedef struct HVhistmodule {
   struct HVhistchannel    chan[MODULE_MAX_CHANS];
} HVhistmodule;

typedef struct HVhistcard {
  struct HVhistmodule   module[HVCARD_MAX_MODULES];
} HVhistcard;

#pragma pack(pop)



typedef struct hvcard_pcie_card {
   struct pci_dev       	*pdev;
   // struct hvcard_pcie_dma       *dma;
   struct hvcardx_endpoint	*ep;
   UINT                 revision;
   // ULONG                ioaddr[2];
   UINT                 hostid;
   UINT                 bus;
   UINT                 slot;
   UINT                 dataslot;
   uint32_t		adcdata[MAX_ADC_DATA_SIZE/4];
   uint64_t		dacdata[MAX_DAC_DATA_SIZE/8];
   struct HVhostcard    link[MAX_CONTROL_LINES];
//   struct HVmodule      module[HVCARD_MAX_MODULES*MAX_CONTROL_LINES];
} hvcard_pcie_card;


#endif /* _HVCARD_PCIE_H */
