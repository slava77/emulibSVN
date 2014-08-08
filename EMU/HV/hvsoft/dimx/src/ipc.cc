#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#include "ipc.h"

#define KEY_FILE "/tmp"
int ipc_project_id=1;

key_t  shmem_key;
int shmem_id;

shmem_type *shmem_address;
//============================================

int shmem_setup()
{
  shmem_key = ftok(KEY_FILE, ipc_project_id);

  if ( shmem_key < (key_t)0  )
    {
      printf("daq_global_ipc_setup : ftok failed\n");
      return -1;
    }
  else printf("key=%x\n",(int)shmem_key);

  shmem_id =  shmget(shmem_key, sizeof(shmem_type), 0660 | IPC_CREAT | IPC_EXCL);
  if ( shmem_id < 0  )
    {
      // Must already exist. Try to attach as client
      ////server = 0;
      shmem_id =  shmget(shmem_key, sizeof(shmem_type), 0660);
      if ( shmem_id < 0  )
        {
          printf("shmget failed\n");
          return -1;
        }
    }

  shmem_address = (shmem_type *)shmat(shmem_id, 0, 0 );

  if ( shmem_address == (shmem_type *)-1 )
    {
      printf("shmat failed\n");
      return -1;
    }
  else printf("shared memory attached OK\n");


  return 1;

}
