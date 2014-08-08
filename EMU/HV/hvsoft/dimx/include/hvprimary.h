#ifndef HVPRIMARY_H
#define HVPRIMARY_H

#include "hvcard_pcie.h"

#define BAUDRATE B9600
#define SERIALDEVICE "/dev/ttyS"

#define MAX_COM_PORTS 2

#define MAX_PRIME_VSET 3700 // Maximum Output voltage allowed to set
#define MAX_PRIME_RAMP_UP 100 // Maximum RampUp Speed
#define MAX_VOLT 5000.0   // Maximum Output Voltage - 5kV
#define MAX_CURR 60000.0  // Maximum Output Current - 60mA


// #define DAC_COMPENSE 1.0055
#define DAC_COMPENSE 1.0
#define DAC_OFFSET 3.0

#define VOLT_STEP 65535.0/MAX_VOLT // DAC counts per 1 V 
#define CURR_STEP 65535.0/MAX_CURR // DAC counts per 1 mkA

#define PRIME_RAMPUP 10
#define PRIME_RAMPDOWN 100
#define PRIME_TRIP_DELAY 5 // 5 seconds
#define PRIME_IMAX 50000 // 50 mA
#define PRIMARY_SET_OUTPUT_ATTEMPTS 2

// =========================================
// HV Library Functions Declarations
// =========================================

static int comfd[MAX_COM_PORTS];


// Primary PS data structure
typedef struct HVPrimary
{
  UINT 	hostid;		// Computer Host ID
  UINT 	port;		// COM port number (COM1 - 0)
  UINT 	addr;		// GPIB address
  UINT 	polarity;	// Output polarity
  UINT 	outset;		// Set value for Output mode
  UINT 	output;		// Read value of actual Output mode
  UINT 	control;	// Control status
  float 	imax;		// Maximum current (trip level)
  UINT 	vset;		// Set value for output voltage
  float 	imon;		// Monitored value of current in mkA
  float 	vmon;		// Monitored value of voltage in V
  UINT 	vcur;		// Internal value of voltage for ramp ups
  UINT 	rampup;		// Ramp up rate V/s
  UINT 	rampdown;	// Ramp down rate V/s
  HV_STATE        state;          // state
  HV_STATUS       status;         // status
} HVPrimary;



// Old Primary PS data structure
typedef struct HVPrimaryOld
{
  UINT    hostid;         // Computer Host ID
  char    port;           // COM port number (COM1 - 0)
  char    addr;           // GPIB address
  UINT    polarity;       // Output polarity
  UINT    outset;         // Set value for Output mode
  UINT    output;         // Read value of actual Output mode
  UINT    control;        // Control status
  float   imax;           // Maximum current (trip level)
  UINT    vset;           // Set value for output voltage
  float   imon;           // Monitored value of current in mkA
  float   vmon;           // Monitored value of voltage in V
  UINT    vcur;           // Internal value of voltage for ramp ups
  UINT    rampup;         // Ramp up rate V/s
  UINT    rampdown;       // Ramp down rate V/s
} HVPrimaryOld;

int ComWrt(int port, char *buf, int size);
int ComRdTerm(int port, char *buf, int size, char delim);
int Fmt(char *str, const char *format, va_list ap);

int SendHVCmd(int port, int addr, char* cmd);

int InitCOM(int port);
int InitHV(int port, int addr, int enable_out);
int ResetHV(int port, int addr);
int CloseCOM(int port);
int ResetCOM(int port);
int CloseHV(int port, int addr);
int EnableHVOutput(int port, int addr);
int DisableHVOutput(int port, int addr);
int SetPolarity(int port, int addr, unsigned int polarity);
int GetPolarityStatus(int port, int addr, unsigned int* status);
int GetPolaritySetting(int port, int addr, unsigned int* setting);
int GetOutputSetting(int port, int addr, unsigned int* output);
int GetOutputStatus(int port, int addr, unsigned int* outstatus);
int GetControlStatus(int port, int addr, unsigned int* ctrlstatus);


int SetHVMaxCurrentPercent(int port, int addr, float percent);
int SetHVMaxVoltagePercent(int port, int addr, float percent);
int SetHVMaxCurrentHex(int port, int addr, int hexvalue);
int SetHVMaxVoltageHex(int port, int addr, int hexvalue);
int SetHVMaxCurrent(int port, int addr, float current);
int SetHVMaxVoltage(int port, int addr, float voltage);

int ReadHVMaxCurrentPercent(int port, int addr, float* current);
int ReadHVMaxVoltagePercent(int port, int addr, float* voltage);
int ReadHVMaxCurrentHex(int port, int addr, int* current);
int ReadHVMaxVoltageHex(int port, int addr, int* voltage);
int ReadHVMaxCurrent(int port, int addr, float* current);
int ReadHVMaxVoltage(int port, int addr, float* voltage);

int ReadHVCurrentPercent(int port, int addr, float* current);
int ReadHVVoltagePercent(int port, int addr, float* voltage);
int ReadHVCurrentHex(int port, int addr, int* current);
int ReadHVVoltageHex(int port, int addr, int* voltage);
int ReadHVCurrent(int port, int addr, float* current);
int ReadHVVoltage(int port, int addr, float* voltage);
#endif
