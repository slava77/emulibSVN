#ifndef HVPVSS_H
#define HVPVSS_H
//=============  mycorr++ ============

#include "pvss.h"
#include "ipc.h"
#include "pvss_db_ext.h"
#include "pvss_common_ext.h"
#include "signal.h"
#include <list>


//#define BROKER_NUMBER 180
#define MAX_PR_NUMBER 4
//#define MAX_CARD_NUMBER 12
#define QUEUE_NUMBER 1

#define DEF_PVSS_UPDATE_INTERVAL	30 	// 15sec
#define DEF_PVSS_IMON_THRESHOLD  	2	// ~0.2mkA
#define DEF_PVSS_VMON_THRESHOLD  	30	// 50V


extern shmem_type *shmem_address;


typedef struct PVSSHVPrimaryOld
{
  int    hostid;         // Computer Host ID
  int/*char*/    port;           // COM port number (COM1 - 0)
  int /*char*/   addr;           // GPIB address
  int    polarity;       // Output polarity
  int    outset;         // Set value for Output mode
  int    output;         // Read value of actual Output mode
  int    control;        // Control status
  int/*float*/   imax;           // Maximum current (trip level)
  int    vset;           // Set value for output voltage
  int /*float*/   imon;           // Monitored value of current in mkA
  int/*float*/   vmon;           // Monitored value of voltage in V
  int    vcur;           // Internal value of voltage for ramp ups
  int    rampup;         // Ramp up rate V/s
  int    rampdown;       // Ramp down rate V/s

} PVSSHVPrimaryOld;

typedef struct HVPrimaryPVSS
{
  // TODO: Should be changed to new HVPrimary when ready
  PVSSHVPrimaryOld hp;
  int update_value;
  int status;
} HVPrimaryPVSS;

extern int pvss_waiting_conf_cnt[QUEUE_NUMBER];

extern int pvss_waiting_conf_cnt_pr;
extern int PVSS_WAITING_FOR_CONF_MODE;

extern int PVSS_MODE;
extern pvss_module_summary pvss_o[BROKER_NUMBER];
extern DimService *pvss_service[BROKER_NUMBER];
extern HVPrimaryPVSS pvss_pr_o;
extern DimService *pvss_pr_service[MAX_PR_NUMBER];
//:::::::
extern int previous_pr_error_status[MAX_PR_NUMBER];
//::::::::
extern list<pvss_module_summary> pvss_queue[QUEUE_NUMBER];
extern list<int> db_index_queue[QUEUE_NUMBER];
extern list<int> db_index_pr_queue;
extern list<HVPrimaryPVSS> pvss_pr_queue;


extern int pvss_channel_status_save[MAX_CARD_NUMBER][16][36]; // channel_status[card][module][channel]
extern int pvss_module_status_save[MAX_CARD_NUMBER][16];
extern int pvss_module_state_save[MAX_CARD_NUMBER][16];
extern int pvss_module_changestatus_save[MAX_CARD_NUMBER][16];
extern int pvss_channel_vmon_save[MAX_CARD_NUMBER][16][36];
extern int pvss_channel_imon_save[MAX_CARD_NUMBER][16][36];
extern int pvss_channel_imax_save[MAX_CARD_NUMBER][16][36];

extern bool MASTER_UPDATE_FLAG;


extern bool IS_CONFIRMATION_NEEDED;//false;//true;

extern int STATUS_DELAY_KEEP;

extern int pvss_cnt;
extern int pvss_cnt_pr;
extern int pvss_cnt_freq;

int initPVSS(int hostid);
int startPVSSinterface();
int updatePVSSdata();
void clearPVSSqueue();


//================ end mycorr++ =============
#endif
