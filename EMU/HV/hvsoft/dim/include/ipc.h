#ifndef IPC_H
#define IPC_H

// attention!!!!!!!!!!
// if we change the  shmem_type structure: the ipcrm -m 0x0103270b should be used to delete the current shmem
// !!!!!!!!!!

#define BROKER_NUMBER 180
#define MAX_CARD_NUMBER 16

typedef struct
{
  int map_module_statuses[MAX_CARD_NUMBER][16];
  int module_statuses_count[MAX_CARD_NUMBER][16][2];
  bool PVSS_CONFIRMATION[BROKER_NUMBER];
  bool HV_PVSS_STARTED_UP;
  bool data_refresh_needed;
} shmem_type ;

int shmem_setup();
#endif

