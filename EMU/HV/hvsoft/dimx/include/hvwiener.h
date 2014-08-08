#ifndef hvwiener_h
#define hvwiener_h
#include "dis.hxx"
#include <vector>

#define MAIN_ON		0x80
#define MAIN_INHIBIT	0x40
#define LOCAL_CTRL_ONLY	0x20
#define	INPUT_FAILURE	0x10
#define OUTPUT_FAILURE	0x08
#define FANTRAY_FAILURE	0x04
#define SENSOR_FAILURE	0x02
#define VME_SYSFAIL	0x01

#define OUTPUT_ON	0x80
#define OUTPUT_INHIBIT	0x40
#define OUTPUT_FAILURE_MIN_SENSE_VOLTAGE	0x20
#define OUTPUT_FAILURE_MAX_SENSE_VOLTAGE	0x10
#define OUTPUT_FAILURE_MAX_TERMINAL_VOLTAGE	0x08
#define OUTPUT_FAILURE_MAX_CURRENT		0x04
#define OUTPUT_FAILURE_MAX_TEMPERATURE		0x02
#define	OUTPUT_FAILURE_MAX_POWER		0x01

#define WIENER_UPD_DELAY 	20	// 2sec*20

typedef struct dimWienerPS_system_status
{
  int     mainOn;         // (0), system is switched on, individual outputs may be controlled by their specific ON/INHIBIT
  int     mainInhibit;    // (1)  external (hardware-)inhibit of the complete system
  int     localControlOnly;//(2), local control only (CANBUS write access denied)
  int     inputFailure;   // (3), any input failure (e.g. power fail)
  int     outputFailure;  // (4), any output failure, details in outputTable
  int     fantrayFailure; // (5), fantray failure
  int     sensorFailure;  // (6), temperature of the external sensors too high
  int     VmeSysfail;     // (7), VME SYSFAIL signal is active
//      int     plugAndPlayIncompatible;// (8)  wrong power supply and rack connected

} dimWienerPS_system_status;

typedef struct dimWienerPS_channel_status
{
  int     outputOn;                       // (0), output channel is on
  int     outputInhibit;                  // (1), external (hardware-)inhibit of the output channel
  int     outputFailureMinSenseVoltage;   // (2), Supervision limit hurt: Sense voltage is too low
  int     outputFailureMaxSenseVoltage;   // (3), Supervision limit hurt: Sense voltage is too high
  int     outputFailureMaxTerminalVoltage;// (4), Supervision limit hurt: Terminal voltage is too high
  int     outputFailureMaxCurrent;        // (5), Supervision limit hurt: Current is too high
  int     outputFailureMaxTemperature;    // (6), Supervision limit hurt: Heat sink temperature is too high
  int     outputFailureMaxPower;          // (7), Supervision limit hurt: Output power is too high
  /*
          int     outputFailureTimeout;           // (9), Communication timeout between output channel and main control
          int     outputCurrentLimited;           // (10),Current limiting is active (constant current mode)
          int     outputRampUp;                   // (11),Output voltage is increasing (e.g. after switch on)
          int     outputRampDown;                 // (12),Output voltage is decreasing (e.g. after switch off)
  */
}  dimWienerPS_channel_status;


typedef struct dimWienerPS_channel
{
  char name[4];		// Channel name
  float meas_volt;	// Measurement Sense Voltage
  float meas_cur;		// Measurement Sense Current
  float meas_term_volt;	// Measurement Terminal Voltage
  // int temp;
  float out_volt;		// Set output Voltage
  int adj_volt;		// Set Adjustment Voltage
  float cur_limit;	// Set Current Limit
  float min_sense_volt;	// Set Minimum Sense Voltage
  float max_sense_volt;	// Set Maximum Sense Voltage
  float max_term_volt;	// Max Terminal Voltage
  float max_cur;		// Maximum Current
  float cfg_max_sense_volt;	// Configuration Maximum Sense Voltage
  float cfg_max_term_volt;	// Configuration Maximum Terminal Voltage
  float cfg_max_cur;		// Configuration Maximum Current
  float max_power;		// Maximum Power
  int group;			// Channel Group to which channel belongs
  int out_switch;	// Output Switch State
  // unsigned int sup_behavior;
  int status;			// Channel Status bit-field
  dimWienerPS_channel_status decoded_status;
  int update_value;
} dimWienerPS_channel;

typedef struct dimWienerPS_system
{
  unsigned int 	sysStatus;
  int  	sysMainSwitch;
  int     psOperatingTime;
  float   sysUpTime;
  char  	nodeID[40];
  char  	sysDescr[40];
  char  	psSerialNumber[8];
  dimWienerPS_system_status decoded_status;
  int update_value;
} dimWienerPS_system;

typedef struct dimWienerPS
{
  dimWienerPS_system sys;
  std::vector<dimWienerPS_channel> chans;
} dimWienerPS;


extern void decodeWienerSysStatus(int, dimWienerPS_system_status* );
extern void decodeWienerChannelStatus(int, dimWienerPS_channel_status* );

#endif
