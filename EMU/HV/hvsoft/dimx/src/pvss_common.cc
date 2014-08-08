#include "pvss_common.h"
#include "pvss_db_ext.h"
#include "signal.h"



//#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int return_card_database_index_by_slot(int slot)
{


  for (int i=0; i<card_database.size(); i++)
    {
      if (slot == card_database[i])
        {
          return i;
          break;
        }
    }

  return -1;

}
void catchterm(int signo)
{

  printf("!!!!!!!!!!! killing hv_pvss process !!!!!!!!!!!!!!\n");
  kill(0,SIGTERM);

  return;

}

int slice_test_turn_on()
{

//==============mycorr++ execv===================================
  char *myargv[10];
  for (int i=0; i<10; i++)  //name of program and /0
    {
      myargv[i]= (char *) malloc(100); // alloc 100 bytes
    }

  if (child_pid=fork()==0)
    {

      strcpy(myargv[0], "slice");
      strcat(myargv[1],"");

      myargv[2]=(char *)0;

      execv("/home/hvuser/hvcard/dim/slice",myargv);

    }
  wait(0);

//==============end mycorr++ execv======================================

}
