/* fast_ana.c
 *
 * Author : Lisa Gorn
 * Date   : 6 June 2000
 * This is a modified version of David Chrisman's offline_ana.
 * It does special-purpose analyses for the FAST site tests.
 * (Unlike event_ana_offline or event_ana_online, which are called by the 
 * main routine in event_ana.c, event_ana_fast is called by the test_manager.)
 *
 * August 02, 2001 I Changed the numeration of tests , SD
 *
*/
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "daq_module.h" 
#include "misc.h" 
#include "daq_conf.h"
#include "csc_event.h"        /* upevt       */
//#include "cfortran.h"
//#include "hbook.h"
#include "test_params.h"
#include "test_functions.h"

#define BUF_SIZE 32000        /* Rough estimate of max event size */
#define NWPAW    1000000
#define HISTO_SIZE      NWPAW*sizeof(float)

typedef struct
  {
  char h[HISTO_SIZE];
  }PAWC;

typedef struct
  {
  int    num;
  int    type;
  char   name[12];
  } csc_id;

PAWC pawc_;           /* A global variable */



static unsigned short buf[BUF_SIZE];

int fast_ana(char *in_file, int max_events, int nskip, int test_num, 
	     csc_id *csc);
void dump_event(unsigned short *buf);
void wait_for_return(void);

/*---------------------------------------------------------------------------*/
/* Program for testing fast_ana (instead of using the test manager to call it
 */
/*
  int main(void)
  {
  int       max_events, nskip, test_num; 
  int       status;
  char      in_file[] = { "/csc_data/cscdata_000719.dat" };
  csc_id    csc;

  max_events = 20;
  nskip = 0;
  test_num = 9;
  csc.num = 1;
  csc.type = 3;
  
  status = fast_ana(in_file, max_events, nskip, test_num, &csc);
  return 0;
  }
*/

/*---------------------------------------------------------------------------*/

int fast_ana(char *in_file, int max_events, int nskip, int test_num, 
	     csc_id *csc)
{
	int            ievent;
	int            fd;
	int            nevents;
	int            rtval;
	int            pass;
	int            error_code = 0;
	short          event_wc;
	static logical first = _TRUE;

/* Copy csc id info to global variables shared by all tests (in test_params.h).
 * This is just a temporary solution; the global variable strategy will have 
 * to be rethought anyhow when we switch the the xforms version of the user 
 * interface.
 */
	strcpy(csc_name, csc->name);
	csc_num = csc->num;
	csc_type = csc->type;

/* Call the test initialization function */
	switch(test_num)
    {

    case 11:
		if (test_11_init()) goto init_err;
		break;

    case 12:
		if (test_12_init()) goto init_err;
		break;

    case 13:
		if (test_13_init()) goto init_err;
		break;

    case 14:
		if (test_14_init()) goto init_err;
		break;

    case 15:
		if (test_15_init()) goto init_err;
		break;

    case 16:
		if (test_16_init()) goto init_err;
		break;

    case 17:
		if (test_17_init()) goto init_err;
		break;

    case 18:
		if (test_18_init()) goto init_err;
		break;

    case 19:
		if (test_19_init()) goto init_err;
		break;

    case 20:
		if (test_20_init()) goto init_err;
		break;

    case 21:
		if (test_21_init()) goto init_err;
		break;

    case 22:
		if (test_22_init()) goto init_err;
		break;

    case 23:
		if (test_23_init()) goto init_err;
		break;

    case 24:
		if (test_24_init()) goto init_err;
		break;

    case 25:
		if (test_25_init()) goto init_err;
		break;

    case 26:
		if (test_26_init()) goto init_err;
		break;

    case 27:
		if (test_27_init()) goto init_err;
		break;

		//    case 28:
		//      if (test_23_init()) goto init_err;
		//      break;

		//    case 29:
		//      if (test_24_init()) goto init_err;
		//      break;

	init_err:
    default:
		printf("Couldn't complete initialization for test %d\n", test_num);
		break;
    }

/* Set up space for PAW common */
	if (first)
    {
		HLIMIT(NWPAW);
/*  printf("histo_nwords = % d\n", NWPAW); */
		first = _FALSE;
    }


/* Loop over the data as many times as required by this test */
	for (pass = 0; pass < num_passes; pass++)
    {
		printf("Beginning PASS %d with %d events requested", pass, max_events);

		/* Analyze the smaller of max_events or the default num_events[pass] */
		nevents = num_events[pass];
		if (max_events < 0) break; 
		else if (max_events == 0 || max_events > num_events[pass]) nevents = 
																	   num_events[pass];
		else nevents = max_events;
		printf("... will analyze %d events\n", nevents);

		/* Run initialization (histogram booking, etc) */
		if (0 != test_begin(pass))
		{
			printf("Initialization failed.\n");
			return -1;
		}

		/* Open the data file */
		if((fd = open(in_file, O_RDONLY)) < 0 )
		{
			printf("Failed to open data file \"%s\"\n", in_file);          
			return -1;
		}

		/* Loop until we read nevents number of events or reach end of file */
		for (ievent = 0; ievent <= nevents; ievent++)
		{
			if (ievent == 0) printf("\n");
			else if (ievent % 1000 == 0) printf("Event %d\n", ievent);

			rtval = read( fd, &event_wc, sizeof(short));
			if (rtval == 0)
			{
				ievent--;
				printf("Reached end of data file %s after %d events\n", 
					   in_file, ievent);
				error_code = 0;
				break;
			}
			else if ((rtval == -1) || (rtval < (int)sizeof(short)) )
			{
				printf("Error reading event word count, data file %s, event_number"
					   " %d \n", in_file, ievent);
				error_code = 1;
				break;
			}
      
			/* Check to see if event word count is reasonable */
			if (event_wc > BUF_SIZE)
			{
				printf("ERROR: Huge event! event_number = %d, event_wc = %d\n",
					   ievent, event_wc);
				error_code = 1;
				break;
			} 

			buf[0] = event_wc;
			rtval = read( fd, buf + 1, (event_wc - 1)*sizeof(short));
			if (rtval == 0)
			{
				ievent--;
				printf("Reached end of data file %s after %d events\n", 
					   in_file, ievent);
				error_code = 0;
				break;
			}
			else if ((rtval == -1) || (rtval < (event_wc - 1)*(int)sizeof(short)) )
			{
				printf("Error reading data file %s, event number %d, event_wc %d \n", 
					   in_file, ievent, event_wc);
				error_code = 1;
				break;
			}

			/* Skip nskip events */
			if (ievent > 0 && ievent < nskip) continue;

			/* Unpack this event's data and analyze it */
			if (ievent == 0 && pass == 0)
			{
				rtval = get_next_event(buf);
				if (rtval == 0)
				{
					/* Read the cable map file */
					if (read_cable_map(upevt_.cable_map_id) != 0)
					{
						printf("ERROR: Failed to read cable map file.\n");
						wait_for_return();
						exit(1);
					}
				} 
				else
				{
					printf("ERROR: Failed to extract config files from first event.\n");
					wait_for_return();
					exit(1);
				}
			}
			else if (ievent > 0)
			{
				rtval = get_next_event(buf);
				if (rtval == 0)
				{
/*        printf("Calling test_event for event %d\n", ievent); */
					if (test_event(pass) == -2) break;
				}
				else
				{
					printf("ERROR: Failed to get next event.\n");
					error_code = 1;
					break;

					/*
					  dump_event(buf);
					  exit(1);
					*/
				}
			}

		}    /* End for ievent <= nevents */

		printf("FAST test %d analysis pass %d ending after reading %d events.\n", 
			   test_num, pass, ievent);
//   test_num, pass, --ievent);
		test_end(pass);

    } /* end of loop over passes */

	test_finish();

	wait_for_return();
	return error_code;
}

void dump_event(unsigned short *buf)
  {
  int i;

  for (i = 0; i < buf[0]; i++)
    {
    printf("%#hx ", buf[i]);
    if ((i % 8) == 0) printf("\n");
    }
  }
