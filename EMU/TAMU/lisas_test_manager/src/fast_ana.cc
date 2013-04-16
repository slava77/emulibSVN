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

#include <iomanip>
#include <iostream>
extern "C"
{
#include "daq_module.h" 
#include "misc.h" 
#include "daq_conf.h"
#include "csc_event.h"        /* upevt       */
#include "cfortran.h"
#include "hbook.h"
#include "test_functions.h"
}

#include "test_params_cxx.h"

/// -- CMSSW Unpacker --
#include "csc_unpacker.h"

//#include "EmuFileReader.h"
#include "emu/daq/reader/RawDataFile.h"
#include "emuSTEPConfigRead.h"
#include "test_config_struct.h"


#define BUF_SIZE 32000        /* Rough estimate of max event size */
#define NWPAW    1000000
#define HISTO_SIZE      NWPAW*sizeof(float)

using namespace std;

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

extern "C" int tot_evt;

static unsigned short buf[BUF_SIZE];

extern "C" int fast_ana(char *in_file, int max_events, int nskip, int test_num, 
	     csc_id *csc);
void dump_event(unsigned short *buf);

extern "C"
{
	void wait_for_return(void);
}

extern "C" test_config_struct tcs; // test configuration from test_config.xml

/*---------------------------------------------------------------------------*/

extern "C" int fast_ana(char *in_file, int max_events, int nskip, int test_num, 
	     csc_id *csc)
{
  cout<<"do i get to the starting point"<<endl;

	int            ievent;
	int            nevents;
	int            rtval;
	int            pass;
	int            error_code = 0;
	short          event_wc, block_wc;
	static logical first = _TRUE;
	static int first_time = 1;

	emuSTEPConfigRead cr;

	if (cr.read_test_config("test_config.xml", &tcs) == -1)
	  //assumed directory is correctly set in emuSTEPConfigRead
	  //for ME42 B904 path should be
	  ///home/cscme42/STEP/data/xml/test_config
	{
		printf("cannot find test_config.xml");
		return 1;
	}

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
      //reset event number for each pass to 0
      //upevt_.event_number=0;

		printf("Beginning PASS %d with %d events requested", pass, max_events);

		/* Analyze the smaller of max_events or the default num_events[pass] */
		nevents = num_events[pass];
		if (max_events < 0) break; 
		else if (max_events == 0 || max_events > num_events[pass]) nevents =  num_events[pass];
		else nevents = max_events;
		printf("... will analyze %d events\n", nevents);

		/* Run initialization (histogram booking, etc) */
		if (0 != test_begin(pass))
		{
			printf("Initialization failed.\n");
			return -1;
		}

		string data_file=in_file;
		cout<<"infile "<<in_file<<endl;
		//EmuFileReader fd(in_file, EmuReader::DDU);
		emu::daq::reader::RawDataFile fd(data_file.c_str(), emu::daq::reader::Base::DDU);
		fd.open(data_file.c_str());

		if (max_events == 0) // only if user wants all events
		{
			// count events in file - there is no method to do it in EmuFileReader
			cout << "count events" << endl;
			tot_evt = 0;
			while (fd.readNextEvent()) tot_evt++;
//			tot_evt = fd.eventNumber();
			cout << "total number of events: " << tot_evt << endl;
		}

		// there is no rewind either
		fd.close();
		fd.open(in_file);

		int run_num;
		int sret = sscanf(in_file, "/csc_data/csc_%08d", &run_num);
		if (sret != 1) cout << "cannot read run number" << endl;

		cout << "File " << in_file << " is opened" << endl;

		// fake header, to satisfy the old unpacker
		short fake_head[] = 
			{
				0x001d, 0x4845, 0x4144, 0x0000, 0x0003, 0x04d2, 0x0000, 0x0000, 
				0x0002, 0x0000, 0x5a01, 0x0000, 0x6803, 0x100b, 0x201d,	0x000a, 
				0x9a58, 0x102c, 0x0fa0, 0x0fa0, 0x102c, 0x100e, 0x001f, 0x001a, 
				0x00c8, 0x001a, 0x0001, 0x0001, 0x001d
			};


		/* Loop until we read nevents number of events or reach end of file */
		for (ievent = 0; ievent <= nevents; ievent++)
		{

			if (ievent == 0) printf("event 0 \n");
			else if (ievent % 1000 == 0) printf("Event %d,upevt_.event_number%d\n", ievent,upevt_.event_number);
			
			//rtval = read( fd, &event_wc, sizeof(short));
			/*if (rtval == 0)
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
			*/
			rtval = fd.readNextEvent();
			//cout<<"rtval value "<<rtval<<endl;
			if (!rtval)
			{
				ievent--;
				printf("Reached end of data file %s after %d events\n", 
					   in_file, ievent);
				error_code = 0;
				break;
			}
			block_wc = event_wc = fd.dataLength() / sizeof (short); // convert bytes to words
			block_wc += 4; // block word count
//			int evnum = fd.eventNumber();
			int evnum = ievent;

			/* Check to see if event word count is reasonable */
			if (block_wc > BUF_SIZE)
			{
				printf("ERROR: Huge event! event_number = %d, event_wc = %d\n",
					   ievent, block_wc);
				error_code = 1;
				break;
			} 

			/* Skip nskip events */
			if (ievent > 0 && ievent < nskip) continue;

//			rtval = read( fd, buf + 1, (event_wc - 1)*sizeof(short));
			// emulate legacy event and block structure

			// run number
			fake_head[5] = run_num;

			// stick event number into the header
			fake_head[7] = evnum & 0xffff; // MSB first (argh..)
			fake_head[6] = (evnum << 16) & 0xffff;

			int i = 1;
			memcpy(buf + i, fake_head, sizeof(fake_head)); // fake event header
			i += sizeof(fake_head)/sizeof(short); // move index
			buf[i++] = block_wc; // event block word count
			buf[i++] = 0x4444; // 'DD' header
			buf[i++] = 0x5532; // 'U2' header
			memcpy(buf + i, fd.data(), fd.dataLength()); // event itself
			i +=  fd.dataLength() / sizeof(short); // move index				
			buf[i++] = block_wc; // duplicate block_wc at the end of data block

			event_wc = i;
			buf[0] = event_wc; // total event word count;
			// RawDataFile
			/* Unpack this event's data and analyze it */

			// int istat = get_next_event((short int *)buf, first_time);

			int istat = get_next_event_cmssw(fd.data(),fd.dataLength(), first_time);
			//test codes expect number of events to start with 0????? (check again for other tests)
			upevt_.event_number=ievent;
			bool test_istat=istat &2;
			//cout<<"istat "<<istat<<"/"<<test_istat<<endl;
			if (istat & 2){ first_time = 0;}
			else 
			  {
			    //cout <<  "starting new event, rtval: "<< rtval <<"\n";
			    first_time = 1;
			  }
			//comment out to see what happens
			//check the line below
			if (rtval != 0)
			{
		       
				// if very first event - Read the cable map file 
				
				if (ievent == 0 && pass == 0)
				{
				  cout<<"read the cable map"<<endl;
					if (read_cable_map(upevt_.cable_map_id) != 0)
					{
						printf("ERROR: Failed to read cable map file.\n");
						wait_for_return();
						exit(1);
					}
				}
				if (test_event(pass) == -2) break;
				
			} 
			else
			{
//					printf("ERROR: Failed to extract config files from first event.\n");
//					wait_for_return();
//					exit(1);
				printf("ERROR: Failed to get next event.\n");
				error_code = 1;
				break;
			}
			
		}    /* End for ievent <= nevents */

		printf("FAST test %d analysis pass %d ending after reading %d events.\n", 
			   test_num, pass, ievent);
		test_end(pass);

    } /* end of loop over passes */

	test_finish();

	// wait for user to press return only in interactive mode, so s/he can see the screen
	if (csc_name[0] != 'p' && csc_name[0] != 'm' && csc_name[0] != '_') wait_for_return();
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
