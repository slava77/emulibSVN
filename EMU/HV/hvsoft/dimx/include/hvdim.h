#ifndef HVDIM_H
#define HVDIM_H
#include <iostream>
#include <vector>
#include <fstream>
#include <map>

// #include <dis.hxx>
// #include <dic.hxx>

#include "log4cplus/coloredlayout.h"
#include "log4cplus/dimlayout.h"
#include "log4cplus/dimappender.h"
#include "hvcard_pcie.h"
#include "hvprimary.h"

using  namespace std;

#define ADC_COUNTS 4000 	// Adc counts range used for current calibration 
#define MAIN_OFFS 3		// 
#define ADC_CONV_COEF 11	// Current mkA correction coefficient
#define MAX_COM_PORT 2		// Maximum number of serial COM ports allowed to use on computer
#define MAX_GPIB_ADDR 8		// Maximum number of GPIB addresses for Primary PSs
#define SEC 1000000		// 1 second definition
#define UPDATE_DELAY 10000	// Main loop update delay 10ms (actual update time is about ~20ms or more)
#define MAX_CHANS 36
#define CSC_MAX_CHANS 30	// Maximum channels per CSC service 30 RDB

// #define DATABUF_DEPTH 250	// Maximum data buffer depth (~5sec)
#define DATABUF_DEPTH 750
#define M_OFFS 	200		// Target vset for linked master channel - RDB vset + M_OFFS
#define MAX_VSET	4000

#define DEF_RDB_VMON_THRESH 15
#define DEF_RDM_VMON_RAMP_THRESH 60
#define DEF_RDB_IMON_THRESH 2 
#define DEF_UPDATE_INTERVAL 10
#define DEF_MASTER_VMON_THRESH 30
#define DEF_MASTER_IMON_THRESH 120


#define USLEEP_DELAY 250000	// General usleep() delay for timeouts

typedef class TripMarker
{
public:
  TripMarker(): marker(0),flag(false),tstamp(0) /*,fn("") */ {};
  int marker;		// Trip marker in data buffer
  bool flag;		// Set trip flag
  time_t tstamp;
  // std::string fn;		// Output trip data dump filename
} TripMarker;

// Internal buffer for saving monitored voltages and currents
typedef struct HVdatabuf
{
  int cnt;
  int vmon[MAX_CHANS][DATABUF_DEPTH];
  int imon[MAX_CHANS][DATABUF_DEPTH];
  TripMarker trip_data[MAX_CHANS];
} HVdatabuf;

// Voltage DAC and ADC calibration data entry V=A*x+b
typedef struct HVcalib
{
  float A;	// Coefficient
  float B;	// Offset
}  HVcalib;



// Calibration data structure definition for one module
typedef struct HVcalibdata
{
  int card;		// Host card (dataslot #)
  int addr;		// Module's bus address
  int type;		// Module's type
  int id;			// Module's ID
  bool fLoaded;		// calibration data is loaded for module flag
  HVcalib vmon;		// calibration data for HV monitoring channel of RDBs
  HVcalib dac[MAX_CHANS];	// calibration data for voltage setting DACs
  HVcalib adc[MAX_CHANS];	// calibration data for voltage reading ADCs
  short cur[MAX_CHANS][ADC_COUNTS];	// current calibration data lookup table
} HVcalibdata;

// Internal DIM Server structure for single hostcard
typedef struct HVdataservice
{
  string server;
  string name;
  string realname;
  HVcalibdata calib[HVCARD_MAX_MODULES]; // Calibration data array for 16 modules
  int flog[HVCARD_MAX_MODULES];		// Enable/Disable data save to file fla
  string ilogname[HVCARD_MAX_MODULES];
  string vlogname[HVCARD_MAX_MODULES];
  ofstream * ilog[HVCARD_MAX_MODULES];	// Array of pointers to current data files
  ofstream * vlog[HVCARD_MAX_MODULES];	// Array of pointers to voltage data files
  HVdatabuf databuf[HVCARD_MAX_MODULES];	// Data buffer for monitored voltages and currents
  HVhostcard * data;			// Pointer to HVhostcard data read from driver
  HVhostcard * setdata;			// Set Parameters
  int card;				// Host Card number
  // DimInfo * diminfo;
  DimService * dimservice;
}  HVdataservice;

#pragma pack(push,1)

// CSC location data structure
typedef struct HVcsc
{
  char            name[12];       // Standard CSC name ex. "ME+3/3/10"
  UINT		endcap;		// Endcap +/- (1/0)
  UINT		station;	// Station
  UINT 		ring;		// Ring
  UINT		position;	// Position on ring
} HVcsc;

// Host Card identification data structure
typedef struct HVcard
{
  UINT            hostid; 	// Computer host id
  UINT            bus;		// PCI bus number
  UINT            slot;		// PCI bus slot number (!not physical PCI slot number)
  UINT            dataslot;  	// Logical dataslot number
} HVcard;

// Part of Dim Rpc single module-board data structure returned from RPC_REAL_DATA service
typedef struct HVboard
{
  HVcard card;			// Host Card information
  HVmodule module;		// Module data from driver
} HVboard;

// Part of Dim Rpc CSC data structure returned from RPC_CSC_DATA
typedef struct HVcscdata
{
  HVcsc		csc;		// CSC information
  HVPrimary 	primary;	// Primary PS data
  HVcard		hvcard;		// Host Card information
  HVchannel 	master;		// Linked Master Board channel data
  HVmodule 	rdb;		// RDB module data
  UINT		rdb_part;	// RDB part which control CSC (1/2 in case of RDB 36 modules)
} HVcscdata;


// Part of Dim Rpc command data structure for CSC operation mode via RPC_CSC_CMD service
typedef struct HVcsccmd
{
  char	csc[12];	// CSC standard name ex. "ME+3/2/10"
  UINT   chan;		// CSC HV channel number
  HV_CMD  cmd;		// Command code from HV_CMD enum
  ULONG   data;		// Command data
  ULONG   size;		// Size of data (not used directly, but can be used by Valeri's PVSS parts
} HVcsccmd;


// Encapsulated Dim Rpc command data structure HVcmd for single module operation mode via RPC_REAL_CMD service
typedef struct HVcmdDIM
{
  ULONG ID;	// DimRpcInfo client instance ID
  HVcmd data;	// Actual module Dim Rpc command data structure (see driver/include/hvcard.h for definition)
} HVcmdDIM;

// Encapsulated Dim Rpc data structure for Primary PS returned via RPC_PRIMARY_DATA
typedef struct HVPrimaryDIM
{
  ULONG ID;	// DimRpcInfo client instance ID
  HVPrimary data;	// Actual Primary PS data
} HVPrimaryDIM;

// Encapsulated Dim Rpc data structure for single module returned via RPC_REAl_DATA
typedef struct HVboardDIM
{
  ULONG ID;	// DimRpcInfo client instance ID
  HVboard data;	// Single module-board data structure
} HVboardDIM;

// Encapsulated single CSC Dim Rpc data structure returned from RPC_CSC_DATA
typedef struct HVcscdataDIM
{
  ULONG ID;	// DimRpcInfo client instance ID
  HVcscdata data;	// CSC data structure
} HVcscdataDIM;


// #pragma pack(push, 1)

// Encapsulated Dim Rpc command data structure HVcsccmd for CSC operation mode via RPC_CSC_CMD
typedef struct HVcsccmdDIM
{
  ULONG ID;
  HVcsccmd data;
} HVcsccmdDIM;

// #pragma pack(pop)

// Data structure to store summary information for chamber
typedef struct HVstatus
{
  char	csc[12];		// Chamber ID
  UINT	rdb;			// Corresponding RDB ID
  UINT 	rdb_part;		// RDB part 1 or 2
  UINT	vset;			// Average Vset
  UINT	vmon_min;		// Minimum Vmon value
  UINT	vmon_max;		// Maximum Vmon value
  float   imon_sum;		// Total current
  float	imon_min;		// Minimum current value
  float	imon_max;		// Maximum current value
  UINT	chans;			// Number of channels
  UINT	chans_on;		// Number of channels in On state
  HV_STATE	state;		// Prevailing state
  HV_STATE	status;		// Prevailing status
} HVstatus;


// CSC Channel Fast Monitoring structure
typedef struct HVchanFastMon
{
  int		vmon;	
  int		imon;
  HV_STATE      status;
} HVchanFastMon;

// CSC Channel Slow Monitoring structure
typedef struct HVchanSlowMon
{
  UINT		vset;
  UINT		ramp_up;
  UINT		ramp_down;
  UINT		imax;
  UINT		imax_ramp;
  UINT		vov;
  UINT		vtrip_delay;
  UINT		itrip_delay;
  UINT		itrip;
  UINT		vtrip;
  UINT	        vmon_threshold;		// Vmon Threshold for triggering update
  UINT 		imon_threshold;		// Imon threshold for triggering update
  HV_STATE	state;
} HVchanSlowMon;

// Whole chamber Fast Monitoring structure
typedef struct HVcscFastMon
{
  UINT		num_chans;
  HV_STATE	mod_status;
  HV_STATE	master_mod_status;
  int		hvmon;
  UINT		interlock;
  UINT		pos5v;
  UINT		neg5v;
  HVchanFastMon master_chan;		// Linked Master Channel data 
  HVchanFastMon chan[CSC_MAX_CHANS];	// RDB Channels data
} HVcscFastMon;

// Whole chamber Slow Monitoring structure
typedef struct HVcscSlowMon
{
  UINT		num_chans;
  UINT		rdb_type;
  UINT		rdb_id;
  UINT		master_type;
  UINT		master_id;
  UINT		ilock_delay;
  UINT		master_trip_delay;
  HV_STATE	mod_state;
  UINT		update_interval;	// Data Update interval in seconds (should be >5)
  HVchanSlowMon master_chan;		// Linked Master Channel data
  HVchanSlowMon chan[CSC_MAX_CHANS];	// RDB Channels data
} HVcscSlowMon;

// Supplementary DIM service updates statistics data 
typedef struct HVcscMonStats
{
  ULONG fast_mon_cnt;			// Counter of Fast Monitoring updates calls
  ULONG slow_mon_cnt;			// COunter of Slow Monitoring updates calls
} HVcscMonStats;

// Internal structure for chamber data
typedef struct HVcscMon
{
  UINT		update_counter;		// Internal updates counter 
  HVcscFastMon  fast_mon;		// Fast Monitoring data
  HVcscSlowMon  slow_mon;		// Slow Monitoring data
  HVcscMonStats stats;			// Updates statistics data
  DimService*   fast_mon_dim;		// Pointer to Fast Monitoring DIM service
  DimService*   slow_mon_dim;		// Pointer to Slow Monitoring DIM service
  DimService*   stats_dim;		// Pointer to Updates statistics DIM service
} HVcscMon;


typedef struct HVPrimaryMonData
{
  int   hostid;         // Computer Host ID
  int   port;           // COM port number (COM1 - 0)
  int   addr;           // GPIB address
  int   polarity;       // Output polarity
  int   outset;         // Set value for Output mode
  int   output;         // Read value of actual Output mode
  int   control;        // Control status
  int   imax;           // Maximum current (trip level)
  int   vset;           // Set value for output voltage
  int   imon;           // Monitored value of current in mkA
  int   vmon;           // Monitored value of voltage in V
  int   vcur;           // Internal value of voltage for ramp ups
  int   rampup;         // Ramp up rate V/s
  int   rampdown;       // Ramp down rate V/s
  int 	update_value;
  int 	status;
} HVPrimaryMonData;

typedef struct HVprimaryMon
{
  UINT          	update_counter;
  UINT          	update_interval;        // DIM Update Interval
  UINT          	vmon_threshold;
  UINT          	imon_threshold;
  HVPrimaryMonData	primary_mon;
  ULONG			stats;
  DimService*   	primary_mon_dim;
  DimService*   	stats_dim;
} HVprimaryMon;

#pragma pack(pop)

extern string HV_DATADIR;	// Default path for calibration data files. Can be overridden by HV_DATADIR env variable
extern string HV_LOGSDIR;	// Default path for data files and server logs. Can be overriden by HV_LOGSDIR env variable
extern string HV_NETLOGSERVER;
extern string WIENER_PL512;

#define HV_DIM_NAME "HV_DIM_SERVER"	// Base HV Dim Server name. Final name is HV_DIM_SERVER_HIDxxx, where xxx - Computer host ID (100, 200, 300)
#define DEF_CONFIG_NAME "DEFAULT"
#define DEF_NETLOGSERVER "cms-uf77.phys.ufl.edu"
#define DEF_WIENER_PL512_ADDR "UFCMSHVLVPS3.cern.ch"
// static const string HV_DIM_NODE="HV_DIM_NODE";	// Obsolete

#define BACKUP_DNS_NODE "127.0.0.1"	// Backup DIM Dns server address (localhost by default)
#define BACKUP_DNS_PORT 2507		// Backup DIM Dns server port

#endif
