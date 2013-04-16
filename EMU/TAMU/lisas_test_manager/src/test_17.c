
/*
 * CFEB Calibration
 *
 * previous version was written by: 
 * Author : Lisa Gorn
 * Date   : April 3, 2001
 * 
 * Version#2 (without any fits)
 * (in current version there is practically nothing from the old one)
 *
 * AUTHOR: Alexei Drozdetski
 * STARTED: 05/08/02
 * FINISHED: 05/23/02
 *
 * LAST CHANGED: 12/03/02, Alexei Drozdetski
 *
 * Contact information: drozdetski@phys.ufl.edu
 *
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "cfortran.h"
#include "hbook.h"
#include "daq_conf.h"
#include "csc_event.h"       /* unpacked data, upevt_ */
#include "misc.h"            /* definition of logical data type */
#include "one_page_plot.h"   /* STRIP_PLOT,...         */
#include "test_utils.h"      /* chamber labels         */
#include "test_params.h"     /* test config parameters */
#include "test_17_ini.h"     //test#17 DEFINEs (global constants for test#17)
#include "test_config_struct.h"

//#include "csc_unpacker.h"
//#include "emu/daq/reader/RawDataFile.h"

extern test_config_struct tcs;
extern int tot_evt;

/* Local function declarations */
int test_17_begin(int pass);
int test_17_event(int pass);
int test_17_end(int pass);
int test_17_finish(void);

/* Global variables local to this test */
#define DAC_SCAN_NPOINTS 26
#define DELAY_SCAN_NDELAYS 16
//#define DELAY_SCAN_NDELAYS 8 //<- WRONG!
#define DELAY_SCAN_NPOINTS 16

//#define DAC2FC 0.125

static logical first = _TRUE;
static FILE *fp1;
static int test_amp[DAC_SCAN_NPOINTS+3];
static int first_dac, last_dac, npoints;
static int peak_sample, scan_mode;
 
//'v2_' - in names of local variables means 'version#2' of this test program...
float v2_event_mid_array[NLAYER][NSTRIP][DELAY_SCAN_NPOINTS][DELAY_SCAN_NDELAYS];
float v2_event_delay_counter[NLAYER][NSTRIP][DELAY_SCAN_NPOINTS][DELAY_SCAN_NDELAYS];
float v2_event_left_array[NLAYER][NSTRIP][DELAY_SCAN_NPOINTS][DELAY_SCAN_NDELAYS];
float v2_event_right_array[NLAYER][NSTRIP][DELAY_SCAN_NPOINTS][DELAY_SCAN_NDELAYS];
float v2_charge_longxt[NLAYER][NSTRIP][DELAY_SCAN_NPOINTS][DELAY_SCAN_NDELAYS];
float v2_charge_longxt_counter[NLAYER][NSTRIP][DELAY_SCAN_NPOINTS][DELAY_SCAN_NDELAYS];
int v2_istrip, v2_i, shift;
int v2_ilayer, v2_idac;
int v2_Itmp=1;
  
float v2_test_amp[NLAYER][NSTRIP][DAC_SCAN_NPOINTS];
float v2_ampl_output[NLAYER][NSTRIP][DAC_SCAN_NPOINTS];
float v2_dac_counter[NLAYER][NSTRIP][DAC_SCAN_NPOINTS];
float v2_charge_output[NLAYER][NSTRIP][DAC_SCAN_NPOINTS];
float v2_charge_sigma_output[NLAYER][NSTRIP][DAC_SCAN_NPOINTS];

char test_17_file_peak_el[]="/test_results/temp/test_17pass0-peak.his";
char v2_test_17_file_his_delay[]="/test_results/temp/test_17_delay_scan.his";
char v2_test_17_file_his_dac[]="/test_results/temp/test_17_dac_scan.his";
char v2_test_17_01_result[]="/test_results/temp/test_17_01.result";
char v2_test_17_02_result[]="/test_results/temp/test_17_02.result";
char v2_test_17_03_result[]="/test_results/temp/test_17_03.result";
char v2_test_17_04_result[]="/test_results/temp/test_17_04.result";
char v2_test_17_05_result[]="/test_results/temp/test_17_05.result";
char v2_test_17_06_result[]="/test_results/temp/test_17_06.result";
char v2_test_17_07_bad[]="/test_results/temp/test_17_07.bad";
char v2_test_17_08_result[]="/test_results/temp/test_17_08.result";
char v2_test_17_09_result[]="/test_results/temp/test_17_09.result";
char v2_test_17_10_result[]="/test_results/temp/test_17_10.result";
char v2_test_17_11_bad[]="/test_results/temp/test_17_11.bad";

int reverse_phase_error = 0;




/******************************************************************************
   Define functions and parameters for FAST site test 17
******************************************************************************/
static int nstrips, ncfebs;

int test_17_init(void) 
{
	int v2_inpoint,v2_kndelay;

	//this subroutine is called only once from OUTSIDE program to initialize test work
	printf("\n\n\nTEST#17 starting... \n");
	printf("TEST#17: Initialization procedure begins\n");
  
	//in order to delete all old histograms from buffer...
	HDELET(0);
  
	test_begin  = test_17_begin;
	test_event  = test_17_event;
	test_end    = test_17_end;
	test_finish = test_17_finish;

	//pass = 0 - calibration, 1 - data processing...
	num_passes = 2;
	//max number of events in 0 pass, and in pass=1
	num_events[0] = 1100;
	num_events[1] = 20000;

    switch (csc_type)
    {
    case 2:  nstrips = 64; ncfebs = 4; break;
    default: nstrips = 80; ncfebs = 5; break;
    }

    printf ("csc_type = %d, nstrips = %d\n", csc_type, nstrips);
	//initialization of some arrays to 0 values for further usage...
	for(v2_ilayer=0;v2_ilayer<NLAYER;v2_ilayer++)
    {
		for(v2_istrip=0;v2_istrip<nstrips;v2_istrip++)
		{
			for(v2_idac=0;v2_idac<DAC_SCAN_NPOINTS;v2_idac++)
			{
				v2_test_amp[v2_ilayer][v2_istrip][v2_idac]=0;
				v2_ampl_output[v2_ilayer][v2_istrip][v2_idac]=0;
				v2_dac_counter[v2_ilayer][v2_istrip][v2_idac]=0;
				v2_charge_output[v2_ilayer][v2_istrip][v2_idac]=0;
			}
		}
    }

	//initialization of some arrays to 0 values for further usage...  
	for(v2_ilayer=0;v2_ilayer<NLAYER;v2_ilayer++)
    {
		for(v2_istrip=0;v2_istrip<nstrips;v2_istrip++)
		{
			for(v2_kndelay=0;v2_kndelay<DELAY_SCAN_NDELAYS;v2_kndelay++)
			{
				for(v2_inpoint=0;v2_inpoint<DELAY_SCAN_NPOINTS;v2_inpoint++)
				{
					v2_event_mid_array[v2_ilayer][v2_istrip][v2_inpoint][v2_kndelay] = 0.0;
					v2_event_delay_counter[v2_ilayer][v2_istrip][v2_inpoint][v2_kndelay] = 0.0+0.000001;
					v2_event_left_array[v2_ilayer][v2_istrip][v2_inpoint][v2_kndelay] = 0.0;
					v2_event_right_array[v2_ilayer][v2_istrip][v2_inpoint][v2_kndelay] = 0.0;
					v2_charge_longxt[v2_ilayer][v2_istrip][v2_inpoint][v2_kndelay] = 0.0;
					v2_charge_longxt_counter[v2_ilayer][v2_istrip][v2_inpoint][v2_kndelay] = 0.0+0.000001;
				}
			}
		}
    }

	return 0;
}

/******************************************************************************
   Book histogram for FAST site test 17, pass0                                       
******************************************************************************/

int test_17_begin(int pass) 
  {

  printf("Data reading and processing...\n");
  printf("Test_17_begin subroutine, pass is equal to %d \n", pass);

  if (pass == 0) //calibration pass
    {
/* Initializations */
    npoints = 0;
    first = _TRUE;
    HBOOK1( 1, "timesample with peak", 20, -0.5, 19.5, 0.);  
    }
  else if (pass == 1) //data processing pass
    {
    //scan_mode = 2 - DAC scan (amplitude scan), 3 - DELAY scan
    //npoints in DAC scan = 26
    printf("PASS=1, scan_mode = %d (where: 2 - DAC scan (amplitude scan), 3 - DELAY scan) \n",
	   scan_mode);

//NOTE: source of pass, scan_mode, csc_name, ... - test_manager program, which is the shell
//for all data processing tests programs (subroutines) 

//NOTE: before here was the area where summary files for both scans were opened
//if I'll do something similar it's a good place for it
//[maybe, it's better to have one summary file for whole test...]
//for now it's not necessary, let's just leave next 10 lines as is...
    if (scan_mode == DELAY_SCAN) 
      {
      }
    else if (scan_mode == DAC_SCAN) 
      {
      }
    
    if (scan_mode == DAC_SCAN) 
      {
      }
    }

  return 0;
  }


/******************************************************************************
   Do analysis and fill arrays for FAST site test 17
******************************************************************************/

int test_17_event(int pass) 
{
    int      i, j, k, ifeb, ilayer, istrip, status;
    int      sca_max, test_strip;
    float    x;
    int c = 0;

	int ev_in_strip = (upevt_.event_number - 1)  % (tcs.t17.delays_per_strip * tcs.t17.events_per_delay);
	int delay_index = ev_in_strip / tcs.t17.events_per_delay;

	// fill legacy upevt parameters
	upevt_.stripcal_scan_mode = DELAY_SCAN;

	upevt_.stripcal_current_value = delay_index * tcs.t17.delay_step + tcs.t17.delay_first; 

	upevt_.stripcal_num_points = tcs.t17.delays_per_strip;

	upevt_.stripcal_current_strip = 
		((upevt_.event_number - 1)  / tcs.t17.delays_per_strip / tcs.t17.events_per_delay) * tcs.t17.strip_step + 
		tcs.t17.strip_first;

	upevt_.stripcal_trigger_per_step = tcs.t17.events_per_delay;
/*
	printf
	(
		"cur_strip: %02d  cur_delay: %02d  ev_in_strip: %03d  delay_index: %02d\n",
		upevt_.stripcal_current_strip,
		upevt_.stripcal_current_value,
		ev_in_strip,
		delay_index
	);
*/
    //This subroutine is using for extraxt data from the *.dat file and input it 
    //to arrays, which we use during our analysis...

    //I'm not sure, what this 'status' variable for here...
    status = 0;

/* Use the first pass to find out what the calibration ramp parameters were */
    if (pass == 0) 
    {
/* On the first event, check that the data file is of the right type */
		if (first) 
		{
			first = _FALSE;
			//HERE npoints=0, upevt_.stripcal_current_value = 0...
			test_amp[npoints] = upevt_.stripcal_current_value;
			npoints++;

			scan_mode = upevt_.stripcal_scan_mode;
			if (!(scan_mode == DAC_SCAN || scan_mode == DELAY_SCAN)) 
			{
				printf("Wrong data file for test 17?\n");
				printf("Found STRIPCAL scan mode %d, should be 2 (DAC) or 3 (delay)\n",
					   scan_mode);
			}
	    
			// ask user if he/she wants to reverse phase correction. This is necessary because
			// on some setups the phase error of 25 ns must be added, and on some subtracted 
			// from the absolute peak time. Still don't know why.
			if (scan_mode == DELAY_SCAN)
			{
				printf ("\nReverse CFEB clock phase correction: (1-yes, 0-no) [0]: ");
//		scanf("%c", &c);
				getint(&c);
				if (c == 1) reverse_phase_error = 1;
				else        reverse_phase_error = 0;
				printf("\nCFEB clock phase correction is %sreversed\n\n", reverse_phase_error ? "" : "NOT ");
			}

		}

//    printf("Event %ld: current_value = %d\n", upevt_.event_number, upevt_.stripcal_current_value);
/* Check this event's calibration parameters */
		//HERE npoints=1, upevt_.event_number=1..640, upevt_.stripcal_num_points=16
		if (upevt_.stripcal_current_value != test_amp[npoints-1]) 
		{
			if (npoints < upevt_.stripcal_num_points) 
			{
				test_amp[npoints] = upevt_.stripcal_current_value;
				npoints++;
			}
			else
			{ 
				printf("Error in event %ld: found npoints=%d\n", 
					   upevt_.event_number, npoints);
			}
		}



		if (scan_mode == DAC_SCAN) 
		{
/* Fill histogram to find location of typical peak sample */
			for (ifeb=0; ifeb < ncfebs; ifeb++) 
			{
				if (upevt_.active_febs[ifeb] > 0) 
				{
					for (ilayer = 0; ilayer < NLAYER; ilayer++) 
					{
						istrip = 16 * ifeb + upevt_.stripcal_current_strip - 1;
						sca_max = 0;
						peak_sample = -1;
						//HERE upevt_.nsca_sample = 16
						for (j = 0; j < upevt_.nsca_sample; j++) 
						{
							if (upevt_.sca[ilayer][istrip][j] > sca_max) 
							{
								sca_max = upevt_.sca[ilayer][istrip][j];
								peak_sample = j;
							}
						}
						HFILL(1, (float)peak_sample, 0., 1.);
						//HERE peak_sample~4 and sometimes it is equal to 12 (if hardware was initialized incorrectly)
					}
				}
			}
//      printf ("peak: %d ", peak_sample);
		}

		else if (scan_mode == DELAY_SCAN) 
		{
		}

		//HERE upevt_.stripcal_trigger_per_step = 40, upevt_.stripcal_num_points = 26
		//the first number = the number of loops for one strip in one layer for one
		//particular calibration amplitude in the DAC scan...
		//the second number = number of DAC (with different calibration amplitude) scans


//in the next 'if' part we are getting test_amp-s for the DAC scan
//test_amp = calibration amplitudes for the DAC scan
		if (upevt_.event_number == upevt_.stripcal_trigger_per_step *
			upevt_.stripcal_num_points) 
		{
			if (scan_mode == DAC_SCAN) 
			{
				if (test_amp[npoints-1] >= test_amp[0]) 
				{
					first_dac = test_amp[0];
					last_dac = test_amp[npoints-1];
				}
				else 
				{
					last_dac = test_amp[0];
					first_dac = test_amp[npoints-1];
				}
				//HERE first_dac=0, last_dac=250 <- 26 steps in DAC scan with different amplitude
				//of the calibration signal
			}
			else if (scan_mode == DELAY_SCAN) 
				printf("Finished with test 17 pass 0\n");
			status = -2; //WHAT IS THIS AND FOR WHAT???
		}
    }


    else if (pass == 1) 
    {
		//HERE upevt_.stripcal_current_strip=1..16 - the number of calibrated strip in 
		//EACH CFEB (simultaneously we calibrate NCFEB*NLAYERS=5*6=30 strips)
    
		//from 1 to 16, 40 events for each strip
		test_strip = upevt_.stripcal_current_strip;

		if (scan_mode == DAC_SCAN) 
		{
/* Get current calibration parameters */
			//HERE upevt_.stripcal_trigger_per_step=40, see description above...
			//HERE upevt_.stripcal_num_points=26, see description above...
			k = (upevt_.event_number-1) / upevt_.stripcal_trigger_per_step;
			k = k % upevt_.stripcal_num_points;

/* Look for active cfebs */
			for (ifeb=0; ifeb < ncfebs; ifeb++) 
			{
				if (upevt_.active_febs[ifeb] > 0) 
				{
					for (ilayer = 0; ilayer < NLAYER; ilayer++) 
					{
						for (istrip = 16 * ifeb; istrip < 16 * (ifeb + 1); istrip++) 
						{
							j = test_strip + 16 * ifeb - 1;
							if (istrip == j) 
							{
								v2_test_amp[ilayer][istrip][k] = (float)test_amp[k]/DAC2FC;
								v2_ampl_output[ilayer][istrip][k] =
									v2_ampl_output[ilayer][istrip][k] + 
									(float)(upevt_.sca[ilayer][istrip][peak_sample]) -
									(float)(upevt_.sca[ilayer][istrip][0]);
								v2_dac_counter[ilayer][istrip][k] = 
									v2_dac_counter[ilayer][istrip][k] + 1.0;
								v2_charge_output[ilayer][istrip][k] = 
									v2_charge_output[ilayer][istrip][k] + 
									(float)(upevt_.sca[ilayer][istrip][peak_sample]) + 
									(float)(upevt_.sca[ilayer][istrip][peak_sample+1]) +
									(float)(upevt_.sca[ilayer][istrip][peak_sample+2]) +
									(float)(upevt_.sca[ilayer][istrip][peak_sample-1]) +
									(float)(upevt_.sca[ilayer][istrip][peak_sample-2]) - 
									((float)(upevt_.sca[ilayer][istrip][0]))*5.0;		  
							}
						}
					}
				}
			}
//      printf ("peak: %d amp_out: %f dac_cnt: %f charge_out: %f sca[pk]: %d sca[0]: %d\n", 
//	      peak_sample,
//	      v2_ampl_output  [3][1][k],
//	      v2_dac_counter  [3][1][k],
//	      v2_charge_output[3][1][k],
//	      upevt_.sca      [3][1][peak_sample],
//	      upevt_.sca      [3][1][0]
//	      );
		}


		else if (scan_mode == DELAY_SCAN) 
		{
// Get current calibration parameters; here, k is the delay point 
// HERE: upevt_.event_number=1..about11000, upevt_.stripcal_trigger_per_step=40
//       upevt_.stripcal_num_points = 16 (number of shift delays)

			k = (upevt_.event_number-1) / upevt_.stripcal_trigger_per_step;
// HERE: k++ for each 16 events
			k = k % upevt_.stripcal_num_points; // % - MEANS OSTATOK OT DELENIQ NA...
// HERE: as a result k=0..15


//FOR EACH EVENT WE DO THE NEXT LOOP: 
// Look for active cfebs 

//HERE: NCFEB=5, 5cfeb*16strips/cfeb=80 strips per plane
			for (ifeb=0; ifeb < ncfebs; ifeb++) 
			{
				if (upevt_.active_febs[ifeb] > 0) 
				{
					//HERE: upevt_.active_febs[ifeb]=1, if active
					//test_strip = 1..16 for each CFEB
					//             IT MEANS, THAT istrip=0to79 for whole plane...
					// NLAYER=6, NSTRIP=80
					istrip = test_strip + 16 * ifeb - 1;

					for (ilayer = 0; ilayer < NLAYER; ilayer++) 
					{
						j = 100 * (ilayer + 1) + (istrip + 1); //ID for hist
						for (i = 0; i < DELAY_SCAN_NPOINTS; i++) 
						{
							//HERE: DELAY_SCAN_NPOINTS = 10 points, +50ns eacs (0..450ns - basic points)
							x = (float)(i + 10 * k);

							v2_event_mid_array[ilayer][istrip][i][k] = 
								v2_event_mid_array[ilayer][istrip][i][k] + 
								(float)(upevt_.sca[ilayer][istrip][i]);
							//HERE: ilayer=0..5, istrip=0..79, i=0..9, k=0..15

							v2_event_delay_counter[ilayer][istrip][i][k] = 
								v2_event_delay_counter[ilayer][istrip][i][k] + 1.0;

							if (istrip > 0) 
							{
								v2_event_left_array[ilayer][istrip][i][k] = 
									v2_event_left_array[ilayer][istrip][i][k] + 
									(float)(upevt_.sca[ilayer][istrip-1][i]);
							}
							if (istrip < nstrips - 1) 
							{
								v2_event_right_array[ilayer][istrip][i][k] = 
									v2_event_right_array[ilayer][istrip][i][k] + 
									(float)(upevt_.sca[ilayer][istrip+1][i]);
							}

							//FOR LONG RANGE CROSS TALK:
							shift = istrip%16;
							for(v2_istrip=0;v2_istrip<nstrips;v2_istrip++)
							{
								if (
									v2_istrip<0*16+shift-2 ||
									(v2_istrip<1*16+shift-2 && v2_istrip>0*16+shift+2) ||
									(v2_istrip<2*16+shift-2 && v2_istrip>1*16+shift+2) ||
									(v2_istrip<3*16+shift-2 && v2_istrip>2*16+shift+2) ||
									(v2_istrip<4*16+shift-2 && v2_istrip>3*16+shift+2) ||
									v2_istrip>4*16+shift+2
									)
								{
		  
									v2_charge_longxt[ilayer][istrip][i][k] = v2_charge_longxt[ilayer][istrip][i][k] + 
										(float)(upevt_.sca[ilayer][v2_istrip][i]) - 
										(float)(upevt_.sca[ilayer][v2_istrip][0]);
		  
									v2_charge_longxt_counter[ilayer][istrip][i][k] = 
										v2_charge_longxt_counter[ilayer][istrip][i][k] + 1;		  
								}
							}
							//-----------------------------------------------------

						}	    
					}
				}
			}
		}
    }

    return status;
}



/******************************************************************************
   End-of-run analysis for FAST site test 17
******************************************************************************/

#define BEST_AMP       130

int test_17_end(int pass) 
  {
  int      j, jmax;
  float    contents[20], max;
  char     *file_his_name;

  if (pass == 0) 
    {
    if (scan_mode == DELAY_SCAN) return(0);

/* Look for the peak time sample */
    HUNPAK(1, contents, "HIST", 1);
    //transfer the contents of a histogram or ... into the local array
    for (j = 2; j < 20; j++) 
      {
      if (j == 2 || contents[j] > max) 
	{
	max = contents[j];
	jmax = j;
	}
      }
    peak_sample = jmax;
    printf("Found peak sample = %d\n", peak_sample);
    
     //write histograms for pass=0 to file
    file_his_name = malloc(strlen(datadir_translation) + 
			   strlen(test_17_file_peak_el) + 5);
    if (file_his_name == NULL)
      {
      printf("malloc() failed for file_his_name.  "
	     "Could not save histograms.\n");
      }
    else
      {
      sprintf(file_his_name,"%s%s",
	      datadir_translation,test_17_file_peak_el);
      printf("Saving histograms\n");
      HRPUT(0, file_his_name, "N");
      HDELET(0);      
      free(file_his_name);    
      }
    
    
// Look for the test point with the largest DAC value <= BEST_AMP
    for (j = 0; j < npoints; j++) 
      {
      if ((j == 0) || (test_amp[j] <= BEST_AMP && test_amp[j] > max)) 
	{
	max = test_amp[j];
	jmax = j;
	}
      }
    }

  else if (pass == 1) 
    {
    }

  return 0;
  }






/*******************************************************************************
   End-of-test analysis for FAST site test 17,
   writing histograms, results and bads to file...
*******************************************************************************/

int test_17_finish(void) 
{
#define NHISCHAN 100//57 // 87 //45+11+1
#define HISSTEP 10.
#define LEFT_X_HIS 100.  //was: -110.
#define RIGHT_X_HIS 660. //was:  450.

    char    *filename, *file_his_name;
    float   limits[7];

    float v2_max_amplitude=0,v2_time_max[NLAYER][nstrips],v2_time_err[NLAYER][nstrips];
    float v2_time_max_mid=0,v2_time_max_rel[NLAYER][nstrips];
    int v2_all_points;
    int v2_nlayer,v2_nstrip,v2_inpoint,v2_kndelay;
    float v2_ampl_max[NLAYER][nstrips];
    float v2_charge[NLAYER][nstrips];
    int v2_inpoint_max,v2_kndelay_max;
    float v2_left_crosstalk[NLAYER][nstrips],v2_right_crosstalk[NLAYER][nstrips];
    int v2_shift;
    int v2_dac, v2_counter;
    float v2_slope[NLAYER][nstrips],v2_interseption[NLAYER][nstrips];
    float v2_L,v2_M,v2_N,v2_O,v2_P,v2_sigma_squared;
    float v2_dac_err[NLAYER][nstrips];
    float v2_moment1[NLAYER][nstrips],v2_moment2[NLAYER][nstrips];
    int v2_mid_hid,v2_left_hid,v2_right_hid,v2_lrxt_hid;
    char v2_htitle[50];
    int v2_flag_his_file;
  
  
    float v2_max_amplitude_left=0;
    int v2_kndelay_max_left,v2_inpoint_max_left;
    float v2_max_amplitude_right=0;
    int v2_kndelay_max_right,v2_inpoint_max_right;
    float v2_max_amplitude_lrxt=0;
    int v2_kndelay_max_lrxt,v2_inpoint_max_lrxt;

    float v2_charge_longxt_output[NLAYER][nstrips];

    int his_index,arr_index;
    float his_x;
    float mid_his[NLAYER][nstrips][NHISCHAN],left_his[NLAYER][nstrips][NHISCHAN];
    float right_his[NLAYER][nstrips][NHISCHAN],lrxt_his[NLAYER][nstrips][NHISCHAN];
    float mid_his_counter[NLAYER][nstrips][NHISCHAN],left_his_counter[NLAYER][nstrips][NHISCHAN];
    float right_his_counter[NLAYER][nstrips][NHISCHAN],lrxt_his_counter[NLAYER][nstrips][NHISCHAN];
  
    int bad1,bad2,bad3,bad4,bad5,bad6;
    int v2_numb_bad_chan1,v2_numb_bad_chan2,v2_numb_bad_chan3,v2_numb_bad_chan4;
    int v2_numb_bad_chan5,v2_numb_bad_chan6;

    time_t  t;
    char    timestring[40];
    float phase_error;
//  char *file_his_name;
  

    time(&t);
    sprintf(timestring, "%s", ctime(&t));

//FOR DELAY SCAN..............
    if (scan_mode == DELAY_SCAN) 
    {
		for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
		{
			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{

				v2_max_amplitude = 0;
				v2_max_amplitude_left = 0;
				v2_max_amplitude_right = 0;
				v2_max_amplitude_lrxt = 0;

				// here we correct the phase error created by the random phase of SCA 50 ns clock on different CFEBs.
				// sca_clock_phase 5 LS bits contain the phase information for each CFEB
				phase_error = ((upevt_.sca_clock_phase >> (v2_nstrip/16)) & 1) * 25;

				for(v2_inpoint=0;v2_inpoint<DELAY_SCAN_NPOINTS;v2_inpoint++)
				{
					for(v2_kndelay=0;v2_kndelay<DELAY_SCAN_NDELAYS;v2_kndelay++)
					{
						v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay] = 
							v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay]/
							v2_event_delay_counter[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];
						v2_event_left_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay] = 
							v2_event_left_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay]/
							v2_event_delay_counter[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];
						v2_event_right_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay] = 
							v2_event_right_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay]/
							v2_event_delay_counter[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];
	
						v2_charge_longxt[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay] = 
							v2_charge_longxt[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay]/
							v2_charge_longxt_counter[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];

						if(v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay] > v2_max_amplitude)
						{
							v2_max_amplitude = v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];
//	      v2_time_max[v2_nlayer][v2_nstrip] = 50*v2_inpoint - 7*v2_kndelay;

							// on some setups the phase error has to be reversed. Still no understanding why. 
							// Let user select the sign of correction.
							if (reverse_phase_error)
								v2_time_max[v2_nlayer][v2_nstrip] = 50*v2_inpoint - 6.25 * v2_kndelay + phase_error;
							else
								v2_time_max[v2_nlayer][v2_nstrip] = 50*v2_inpoint - 6.25 * v2_kndelay - phase_error;

							v2_time_err[v2_nlayer][v2_nstrip] = 0;	      
							v2_kndelay_max = v2_kndelay;
							v2_inpoint_max = v2_inpoint;
						}

						if(v2_event_left_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay] > v2_max_amplitude_left)
						{
							v2_max_amplitude_left = v2_event_left_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];
							v2_kndelay_max_left = v2_kndelay;
							v2_inpoint_max_left = v2_inpoint;
						}

						if(v2_event_right_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay] > v2_max_amplitude_right)
						{
							v2_max_amplitude_right = v2_event_right_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];
							v2_kndelay_max_right = v2_kndelay;
							v2_inpoint_max_right = v2_inpoint;
						}

					}	  
				}



				v2_time_max_mid = v2_time_max_mid + v2_time_max[v2_nlayer][v2_nstrip];
				//writing amplitude with counting into account the value of pedestal
				v2_ampl_max[v2_nlayer][v2_nstrip] = v2_max_amplitude - 
					v2_event_mid_array[v2_nlayer][v2_nstrip][0][0];

				//LEFT CROSSTALK CALCULATIONS:
				v2_left_crosstalk[v2_nlayer][v2_nstrip] = 0; //initialization
				if(v2_nstrip>0)
				{
					v2_left_crosstalk[v2_nlayer][v2_nstrip] = 
						(v2_event_left_array[v2_nlayer][v2_nstrip][v2_inpoint_max_left][v2_kndelay_max_left] -
						 v2_event_left_array[v2_nlayer][v2_nstrip][0][0])/
						(v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max][v2_kndelay_max] -
						 v2_event_mid_array[v2_nlayer][v2_nstrip][0][0]);
				}
        
				//RIGHT CROSSTALK CALCULATIONS:
				v2_right_crosstalk[v2_nlayer][v2_nstrip] = 0; //initialization
				if(v2_nstrip<(nstrips-1))
				{
					v2_right_crosstalk[v2_nlayer][v2_nstrip] = 
						(v2_event_right_array[v2_nlayer][v2_nstrip][v2_inpoint_max_right][v2_kndelay_max_right] -
						 v2_event_right_array[v2_nlayer][v2_nstrip][0][0])/
						(v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max][v2_kndelay_max] -
						 v2_event_mid_array[v2_nlayer][v2_nstrip][0][0]);
				}

				//CHARGE (SHAPE) CALCULATIONS
				if(v2_inpoint_max>=2)
				{
					v2_charge[v2_nlayer][v2_nstrip] = 
						(v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max][v2_kndelay_max] +
						 v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max-1][v2_kndelay_max] +
						 v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max-2][v2_kndelay_max] +
						 v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max+2][v2_kndelay_max] +
						 v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max+1][v2_kndelay_max] -
						 5.0*v2_event_mid_array[v2_nlayer][v2_nstrip][0][0])/
						(v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max][v2_kndelay_max] - 
						 v2_event_mid_array[v2_nlayer][v2_nstrip][0][0]);
/*	  if (v2_nlayer == 0 && (v2_nstrip == 0 || v2_nstrip == 1))
	  {
	  printf("layer=%d strip=%d point=%d delay=%d max=%f max-1=%f max-2=%f max+1=%f max+2=%f zero=%f charge=%f\n",
	  v2_nlayer,
	  v2_nstrip,
	  v2_inpoint_max,
	  v2_kndelay_max,
	  v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max][v2_kndelay_max],
	  v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max-1][v2_kndelay_max],
	  v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max-2][v2_kndelay_max],
	  v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max+1][v2_kndelay_max],
	  v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max+2][v2_kndelay_max],
	  v2_event_mid_array[v2_nlayer][v2_nstrip][0][0],
	  v2_charge[v2_nlayer][v2_nstrip]);
	  }
*/
				}
				else if(v2_inpoint_max==1)
				{
					v2_charge[v2_nlayer][v2_nstrip] = 
						(v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max][v2_kndelay_max] +
						 v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max-1][v2_kndelay_max] +
						 v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max+1][v2_kndelay_max] -
						 3.0*v2_event_mid_array[v2_nlayer][v2_nstrip][0][0])/
						(v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint_max][v2_kndelay_max] - 
						 v2_event_mid_array[v2_nlayer][v2_nstrip][0][0]);
				} 
				else
				{
					v2_charge[v2_nlayer][v2_nstrip] = 0;
				}

			}     
		}


		//LONG RANGE CROSSTALK CALCULATIONS...
		for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
		{
			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{
				v2_max_amplitude = 0;
				v2_max_amplitude_left = 0;
				v2_max_amplitude_right = 0;
				v2_max_amplitude_lrxt = 0;
				for(v2_inpoint=0;v2_inpoint<DELAY_SCAN_NPOINTS;v2_inpoint++)
				{
					for(v2_kndelay=0;v2_kndelay<DELAY_SCAN_NDELAYS;v2_kndelay++)
					{
						if(v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay] > v2_max_amplitude)
						{
							v2_max_amplitude = v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];
							v2_kndelay_max = v2_kndelay;
							v2_inpoint_max = v2_inpoint;
						}
						if(v2_event_left_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay] > v2_max_amplitude_left)
						{
							v2_max_amplitude_left = v2_event_left_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];
							v2_kndelay_max_left = v2_kndelay;
							v2_inpoint_max_left = v2_inpoint;
						}
						if(v2_event_right_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay] > v2_max_amplitude_right)
						{
							v2_max_amplitude_right = v2_event_right_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];
							v2_kndelay_max_right = v2_kndelay;
							v2_inpoint_max_right = v2_inpoint;
						}
						if(v2_charge_longxt[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay] < v2_max_amplitude_lrxt)
						{
							v2_max_amplitude_lrxt = v2_charge_longxt[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];
							v2_kndelay_max_lrxt = v2_kndelay;
							v2_inpoint_max_lrxt = v2_inpoint;
						}
					}	  
				}
				v2_shift = v2_nstrip%16;

				v2_charge_longxt_output[v2_nlayer][v2_nstrip] = 
					v2_charge_longxt[v2_nlayer][v2_nstrip][v2_inpoint_max_lrxt][v2_kndelay_max_lrxt]/
					(
						v2_event_mid_array[v2_nlayer][(int)(0*16+v2_shift)][v2_inpoint_max][v2_kndelay_max] -
						v2_event_mid_array[v2_nlayer][(int)(0*16+v2_shift)][0][0] +
						v2_event_mid_array[v2_nlayer][(int)(1*16+v2_shift)][v2_inpoint_max][v2_kndelay_max] -
						v2_event_mid_array[v2_nlayer][(int)(1*16+v2_shift)][0][0] +
						v2_event_mid_array[v2_nlayer][(int)(2*16+v2_shift)][v2_inpoint_max][v2_kndelay_max] -
						v2_event_mid_array[v2_nlayer][(int)(2*16+v2_shift)][0][0] +
						v2_event_mid_array[v2_nlayer][(int)(3*16+v2_shift)][v2_inpoint_max][v2_kndelay_max] -
						v2_event_mid_array[v2_nlayer][(int)(3*16+v2_shift)][0][0] +
						v2_event_mid_array[v2_nlayer][(int)(4*16+v2_shift)][v2_inpoint_max][v2_kndelay_max] -
						v2_event_mid_array[v2_nlayer][(int)(4*16+v2_shift)][0][0] 
						);
			}     
		}

		v2_all_points = NLAYER*nstrips;
		v2_time_max_mid = v2_time_max_mid/v2_all_points;
		for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
		{
			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{

				v2_time_max_rel[v2_nlayer][v2_nstrip] = 
					v2_time_max[v2_nlayer][v2_nstrip] - 
					v2_time_max_mid;
			}     
		}


// *.ps FILES WRITING STARTS HERE FOR DELAY SCAN
		//RELATIVE TIME
		limits[0] =  REL_TIME_FRAME_BOTTOM;
		limits[1] =  REL_TIME_FRAME_UP;
		limits[2] =  REL_TIME_FRAME_BOTTOM_SUM;
		limits[3] =  REL_TIME_FRAME_UP_SUM;
		limits[4] =  REL_TIME_BOTTOM_BOUND;
		limits[5] =  REL_TIME_UP_BOUND;
		limits[6] = nstrips;
		one_page_plot_v3(17, 1, STRIP_PLOT, (float *)v2_time_max_rel, (float *)v2_time_err,
						 limits, csc_name, "Relative time corresp. to max amplitude",
						 "relative time, ns",1);

		//AMPLITUDE
		limits[0] =  AMPL_FRAME_BOTTOM;
		limits[1] =  AMPL_FRAME_UP;
		limits[2] =  AMPL_FRAME_BOTTOM_SUM;
		limits[3] =  AMPL_FRAME_UP_SUM;
		limits[4] =  AMPL_BOTTOM_BOUND;
		limits[5] =  AMPL_UP_BOUND;
		limits[6] = nstrips;
		one_page_plot_v3(17, 2, STRIP_PLOT, (float *)v2_ampl_max, (float *)v2_time_err,
						 limits, csc_name, "Signal peak (Amax - A0) for Qin=620fQ", 
						 "peak amplitude, ADC", 1);

		//SHAPE
		limits[0] =  Q_FRAME_BOTTOM;
		limits[1] =  Q_FRAME_UP;
		limits[2] =  Q_FRAME_BOTTOM_SUM;
		limits[3] =  Q_FRAME_UP_SUM;
		limits[4] =  Q_BOTTOM_BOUND;
		limits[5] =  Q_UP_BOUND;
		limits[6] = nstrips;
/*    for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
      {
      for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
      {
      printf("layer=%d strip=%d charge=%.2f \n",v2_nlayer,v2_nstrip,v2_charge[v2_nlayer][v2_nstrip]);
      }     
      }
*/
		one_page_plot_v3(17, 3, STRIP_PLOT, (float *)v2_charge, (float *)v2_time_err,
						 limits, csc_name, "Signal shape quality", 
						 "(five sample sum)/amplitude", 1);


		//LEFT CROSS TALK
		limits[0] =  LXT_FRAME_BOTTOM;
		limits[1] =  LXT_FRAME_UP;
		limits[2] =  LXT_FRAME_BOTTOM_SUM;
		limits[3] =  LXT_FRAME_UP_SUM;
		limits[4] =  LXT_BOTTOM_BOUND;
		limits[5] =  LXT_UP_BOUND;
		limits[6] = nstrips;
		one_page_plot_v3(17, 4, STRIP_PLOT, (float *)v2_left_crosstalk, (float *)v2_time_err,
						 limits, csc_name, "Left crosstalk", 
						 "left crosstalk, relative", 1);


		//RIGHT CROSS TALK
		limits[0] =  RXT_FRAME_BOTTOM;
		limits[1] =  RXT_FRAME_UP;
		limits[2] =  RXT_FRAME_BOTTOM_SUM;
		limits[3] =  RXT_FRAME_UP_SUM;
		limits[4] =  RXT_BOTTOM_BOUND;
		limits[5] =  RXT_UP_BOUND;
		limits[6] = nstrips;
		one_page_plot_v3(17, 5, STRIP_PLOT, (float *)v2_right_crosstalk, (float *)v2_time_err,
						 limits, csc_name, "Right crosstalk", 
						 "right crosstalk, relative", 1);


		//LONG RANGE CROSSTALK
		limits[0] =  LRXT_FRAME_BOTTOM;
		limits[1] =  LRXT_FRAME_UP;
		limits[2] =  LRXT_FRAME_BOTTOM_SUM;
		limits[3] =  LRXT_FRAME_UP_SUM;
		limits[4] =  LRXT_BOTTOM_BOUND;
		limits[5] =  LRXT_UP_BOUND;
		limits[6] = nstrips;
		one_page_plot_v3(17, 6, STRIP_PLOT, (float *)v2_charge_longxt_output, (float *)v2_time_err,
						 limits, csc_name, "Long range crosstalk", 
						 "long crosstalk, relative", 1);


		//WRITING HISTOGRAMS INTO THE FILE FOR DELAY SCAN STARTS HERE
		v2_flag_his_file = 0;
		file_his_name = malloc(strlen(datadir_translation) + 
							   strlen(v2_test_17_file_his_delay) + 5);
		if (file_his_name == NULL)
		{
			printf("malloc() failed for file_his_name.  "
				   "Could not save histograms.\n");
		}
		else
		{
			sprintf(file_his_name,"%s%s",datadir_translation,v2_test_17_file_his_delay);
			printf(" Saving DELAY histograms\n");

			//INITIALIZATION
			for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
			{
				for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
				{
					for(arr_index=0;arr_index<NHISCHAN;arr_index++)
					{
						mid_his[v2_nlayer][v2_nstrip][arr_index] = 0.;
						left_his[v2_nlayer][v2_nstrip][arr_index] = 0.;
						right_his[v2_nlayer][v2_nstrip][arr_index] = 0.;
						lrxt_his[v2_nlayer][v2_nstrip][arr_index] = 0.;
						mid_his_counter[v2_nlayer][v2_nstrip][arr_index] = 0.0001;
						left_his_counter[v2_nlayer][v2_nstrip][arr_index] = 0.0001;
						right_his_counter[v2_nlayer][v2_nstrip][arr_index] = 0.0001;
						lrxt_his_counter[v2_nlayer][v2_nstrip][arr_index] = 0.0001;
					}	  
				}
			}
      
			//WRITING ARRAYS
			for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
			{
				for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
				{
					for(v2_kndelay=0;v2_kndelay<DELAY_SCAN_NDELAYS;v2_kndelay++)
					{
						for(v2_inpoint=0;v2_inpoint<DELAY_SCAN_NPOINTS;v2_inpoint++)
						{
							arr_index = floor((-(LEFT_X_HIS) + 50*v2_inpoint - 6.25*v2_kndelay)/HISSTEP);

							mid_his[v2_nlayer][v2_nstrip][arr_index] = mid_his[v2_nlayer][v2_nstrip][arr_index] +
								v2_event_mid_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];
							mid_his_counter[v2_nlayer][v2_nstrip][arr_index] = 
								mid_his_counter[v2_nlayer][v2_nstrip][arr_index] + 1;
							left_his[v2_nlayer][v2_nstrip][arr_index] = left_his[v2_nlayer][v2_nstrip][arr_index] +
								v2_event_left_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];
							left_his_counter[v2_nlayer][v2_nstrip][arr_index] =
								left_his_counter[v2_nlayer][v2_nstrip][arr_index] + 1;
							right_his[v2_nlayer][v2_nstrip][arr_index] = right_his[v2_nlayer][v2_nstrip][arr_index] +
								v2_event_right_array[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];
							right_his_counter[v2_nlayer][v2_nstrip][arr_index] =
								right_his_counter[v2_nlayer][v2_nstrip][arr_index] + 1;
							lrxt_his[v2_nlayer][v2_nstrip][arr_index] = lrxt_his[v2_nlayer][v2_nstrip][arr_index] + 
								v2_charge_longxt[v2_nlayer][v2_nstrip][v2_inpoint][v2_kndelay];
							lrxt_his_counter[v2_nlayer][v2_nstrip][arr_index] =
								lrxt_his_counter[v2_nlayer][v2_nstrip][arr_index] + 1;
						}	  
					}
				}
			}


			//WRITING ARRAYS
			for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
			{
				for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
				{
					for(arr_index=0;arr_index<NHISCHAN;arr_index++)
					{
						mid_his[v2_nlayer][v2_nstrip][arr_index] = 
							mid_his[v2_nlayer][v2_nstrip][arr_index]/
							mid_his_counter[v2_nlayer][v2_nstrip][arr_index];
						left_his[v2_nlayer][v2_nstrip][arr_index] = 
							left_his[v2_nlayer][v2_nstrip][arr_index]/
							left_his_counter[v2_nlayer][v2_nstrip][arr_index];
						right_his[v2_nlayer][v2_nstrip][arr_index] = 
							right_his[v2_nlayer][v2_nstrip][arr_index]/
							right_his_counter[v2_nlayer][v2_nstrip][arr_index];
						lrxt_his[v2_nlayer][v2_nstrip][arr_index] = 
							lrxt_his[v2_nlayer][v2_nstrip][arr_index]/
							lrxt_his_counter[v2_nlayer][v2_nstrip][arr_index];
					}	  
				}
			}

			//BOOKING AND FILLING HISTOGRAMS
			for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
			{
				for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
				{
					v2_mid_hid = 1000 + 100*v2_nlayer + v2_nstrip+1;
					sprintf(v2_htitle, "Middle Pulse Shape for strip# %d in layer# %d",
							(v2_nstrip+1),(v2_nlayer+1));
					HBOOK1(v2_mid_hid,v2_htitle,(NHISCHAN-1),LEFT_X_HIS,RIGHT_X_HIS,0.);
					v2_left_hid = 2000 + 100*(v2_nlayer+1) + v2_nstrip+1;
					sprintf(v2_htitle, "Left Pulse Shape for strip# %d in layer# %d",
							(v2_nstrip+1),(v2_nlayer+1));
					HBOOK1(v2_left_hid,v2_htitle,(NHISCHAN-1),LEFT_X_HIS,RIGHT_X_HIS,0.);
					v2_right_hid = 3000 + 100*(v2_nlayer+1) + v2_nstrip+1;
					sprintf(v2_htitle, "Right Pulse Shape for strip# %d in layer# %d",
							(v2_nstrip+1),(v2_nlayer+1));
					HBOOK1(v2_right_hid,v2_htitle,(NHISCHAN-1),LEFT_X_HIS,RIGHT_X_HIS,0.);
					v2_lrxt_hid = 10000 + 100*(v2_nlayer+1) + v2_nstrip+1;
					sprintf(v2_htitle, "Long Range XT Shape for strip# %d in layer# %d",
							(v2_nstrip+1),(v2_nlayer+1));
					HBOOK1(v2_lrxt_hid,v2_htitle,(NHISCHAN-1),LEFT_X_HIS,RIGHT_X_HIS,0.);

					for(his_index=0;his_index<NHISCHAN;his_index++)
					{
						his_x = (float)(his_index*10. +(LEFT_X_HIS) + HISSTEP/2.);

						HFILL(v2_mid_hid,his_x,0.,mid_his[v2_nlayer][v2_nstrip][his_index]);
						HFILL(v2_left_hid,his_x,0.,left_his[v2_nlayer][v2_nstrip][his_index]);
						HFILL(v2_right_hid,his_x,0.,right_his[v2_nlayer][v2_nstrip][his_index]);
						HFILL(v2_lrxt_hid,his_x,0.,lrxt_his[v2_nlayer][v2_nstrip][his_index]);
					}
				}
			}
      

			HRPUT(0, file_his_name, "N");
			HDELET(0);	    
			free(file_his_name);    
		}




/* Add the date and time */
//	time(&t);
//	sprintf(timestring, "%s", ctime(&t));
        



//7777777777777777777777777777777777777777777777777777777777777
		v2_numb_bad_chan1 = 0;
		v2_numb_bad_chan2 = 0;
		v2_numb_bad_chan3 = 0;
		v2_numb_bad_chan4 = 0;
		v2_numb_bad_chan5 = 0;
		v2_numb_bad_chan6 = 0;
		filename = malloc(strlen(datadir_translation) + 
						  strlen(v2_test_17_07_bad) + 5 );
		if (filename == NULL)
		{
			printf("malloc() failed on filename.\n");
			return 1;
		}
		sprintf(filename,"%s%s",datadir_translation,v2_test_17_07_bad);

		fp1 = fopen(filename, "w");
		if(fp1 == NULL)
		{
			printf("File %s could not be opened or created.\n",filename);
			free(filename);
		}
		else
		{

			fprintf(fp1,"Test 17:            Comparator output timing\n");
			fprintf(fp1,"Test_17_07.bad:     bad channels (DELAY SCAN)\n");
			fprintf(fp1,"Chamber#            %s\n", csc_name);
			fprintf(fp1,"Test performed:     see *.ps files coorresponding to this test\n");
			fprintf(fp1,"Analysis done:      %s\n\n\n",timestring);
      
			fprintf(fp1,"NOTE: \n");
			fprintf(fp1,"Relative Time bounds: %5.1f %5.1f\n",REL_TIME_BOTTOM_BOUND,REL_TIME_UP_BOUND);
			fprintf(fp1,"AMPL bounds: %6.1f %6.1f\n",AMPL_BOTTOM_BOUND,AMPL_UP_BOUND);
			fprintf(fp1,"Q bounds: %3.1f %3.1f\n",Q_BOTTOM_BOUND,Q_UP_BOUND);
			fprintf(fp1,"LXT bounds: %4.2f %4.2f\n",LXT_BOTTOM_BOUND,LXT_UP_BOUND);
			fprintf(fp1,"RXT bounds: %4.2f %4.2f\n",RXT_BOTTOM_BOUND,RXT_UP_BOUND);
			fprintf(fp1,"LRXT bounds: %7.4f %7.4f\n\n\n",LRXT_BOTTOM_BOUND,LRXT_UP_BOUND);

//      fprintf(fp1,"NOTE: 0 - means OK, 1 - BAD\n\n");
      
//      fprintf(fp1,"Plane#   Strip#  REL_T   AMPL      Q       LXT      RXT      LRXT     problem_code\n");


			bad1=0;
			bad2=0;
			bad3=0;
			bad4=0;
			bad5=0;
			bad6=0;
			for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
			{
				for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
				{
					if((v2_time_max_rel[v2_nlayer][v2_nstrip] < REL_TIME_BOTTOM_BOUND) ||
					   (v2_time_max_rel[v2_nlayer][v2_nstrip] > REL_TIME_UP_BOUND))
					{
						if(bad1==0)
						{
							fprintf(fp1,"Plane#   Strip#  REL_T     problem_code\n");
						}
						bad1 = 1;
	    
						v2_numb_bad_chan1 = v2_numb_bad_chan1 + 1;
						fprintf(fp1,"  %d       %2d %8.1f   out of range\n",
								(v2_nlayer+1),(v2_nstrip+1),
								v2_time_max_rel[v2_nlayer][v2_nstrip]);
					}
				}
			}

			for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
			{
				for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
				{
					if((v2_ampl_max[v2_nlayer][v2_nstrip] < AMPL_BOTTOM_BOUND) ||
					   (v2_ampl_max[v2_nlayer][v2_nstrip] > AMPL_UP_BOUND))
					{
						if(bad2==0)
						{
							fprintf(fp1,"Plane#   Strip#  AMPL     problem_code\n");
						}
						bad2 = 1;
	    
						v2_numb_bad_chan2 = v2_numb_bad_chan2 + 1;
						fprintf(fp1,"  %d       %2d %8.1f   out of range\n",
								(v2_nlayer+1),(v2_nstrip+1),
								v2_ampl_max[v2_nlayer][v2_nstrip]);
					}
				}
			}

			for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
			{
				for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
				{
					if((v2_charge[v2_nlayer][v2_nstrip] < Q_BOTTOM_BOUND) ||
					   (v2_charge[v2_nlayer][v2_nstrip] > Q_UP_BOUND))
					{
						if(bad3==0)
						{
							fprintf(fp1,"Plane#   Strip#   Q     problem_code\n");
						}
						bad3 = 1;
	    
						v2_numb_bad_chan3 = v2_numb_bad_chan3 + 1;
						fprintf(fp1,"  %d       %2d %8.2f   out of range\n",
								(v2_nlayer+1),(v2_nstrip+1),
								v2_charge[v2_nlayer][v2_nstrip]);
					}
				}
			}

			for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
			{
				for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
				{
					if(((v2_left_crosstalk[v2_nlayer][v2_nstrip] < LXT_BOTTOM_BOUND) ||
						(v2_left_crosstalk[v2_nlayer][v2_nstrip] > LXT_UP_BOUND)) &&
					   (v2_nstrip>0))
					{
						if(bad4==0)
						{
							fprintf(fp1,"Plane#   Strip#  LXT     problem_code\n");
						}
						bad4 = 1;
	    
						v2_numb_bad_chan4 = v2_numb_bad_chan4 + 1;
						fprintf(fp1,"  %d       %2d %8.4f   out of range\n",
								(v2_nlayer+1),(v2_nstrip+1),
								v2_left_crosstalk[v2_nlayer][v2_nstrip]);
					}
				}
			}

			for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
			{
				for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
				{
					if(((v2_right_crosstalk[v2_nlayer][v2_nstrip] < RXT_BOTTOM_BOUND) ||
						(v2_right_crosstalk[v2_nlayer][v2_nstrip] > RXT_UP_BOUND)) && 
					   (v2_nstrip<(nstrips-1)))
					{
						if(bad5==0)
						{
							fprintf(fp1,"Plane#   Strip#  RXT     problem_code\n");
						}
						bad5 = 1;
	    
						v2_numb_bad_chan5 = v2_numb_bad_chan5 + 1;
						fprintf(fp1,"  %d       %2d %8.4f   out of range\n",
								(v2_nlayer+1),(v2_nstrip+1),
								v2_right_crosstalk[v2_nlayer][v2_nstrip]);
					}
				}
			}

			for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
			{
				for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
				{
					if((v2_charge_longxt_output[v2_nlayer][v2_nstrip] < LRXT_BOTTOM_BOUND) ||
					   (v2_charge_longxt_output[v2_nlayer][v2_nstrip] > LRXT_UP_BOUND))
					{
						if(bad6==0)
						{
							fprintf(fp1,"Plane#   Strip#  LRXT     problem_code\n");
						}
						bad6 = 1;
	    
						v2_numb_bad_chan6 = v2_numb_bad_chan6 + 1;
						fprintf(fp1,"  %d       %2d %9.6f   out of range\n",
								(v2_nlayer+1),(v2_nstrip+1),
								v2_charge_longxt_output[v2_nlayer][v2_nstrip]);
					}
				}
			}



			if ((v2_numb_bad_chan1 == 0) && (v2_numb_bad_chan2 == 0) && (v2_numb_bad_chan3 == 0) && 
				(v2_numb_bad_chan4 == 0) && (v2_numb_bad_chan5 == 0) && (v2_numb_bad_chan6 == 0))
			{
				fprintf(fp1,"\n\n\nSummary: test OK\n\n");
			}
			else
			{
				fprintf(fp1,"\n\n\nSummary: test FAILED\n");
				fprintf(fp1,"Number of errors in all channels = %d\n\n",
						(v2_numb_bad_chan1+v2_numb_bad_chan2+v2_numb_bad_chan3+
						 v2_numb_bad_chan4+v2_numb_bad_chan5+v2_numb_bad_chan6));
			}
      
			fclose(fp1);
			free(filename);
		}
    






		//WRITING RESULTS TO *.result FILE FOR DELAY SCAN STARTS HERE
		//NOTE: number of *.result file corresponds to one of the *.ps file
		filename = malloc(strlen(datadir_translation) + 
						  strlen(v2_test_17_01_result) + 5 );
		if (filename == NULL)
		{
			printf("malloc() failed on filename.\n");
			return 1;
		}
		sprintf(filename,"%s%s",datadir_translation,v2_test_17_01_result);

		fp1 = fopen(filename, "w");
		if(fp1 == NULL)
		{
			printf("File %s could not be opened or created.\n",filename);
			free(filename);
		}
		else
		{
			fprintf(fp1,"Test 17:            CFEB calibration\n");
			fprintf(fp1,"Test_17_01.result:  Output signal time offsets del(T) vs (plane, strip)\n");
			fprintf(fp1,"Chamber#            %s\n", csc_name);
			fprintf(fp1,"Test performed:     see *.ps files coorresponding to this test\n");
			fprintf(fp1,"Analysis done:      %s\n",timestring);
			if (v2_numb_bad_chan1 == 0)
			{
				fprintf(fp1,"Summary: test OK\n\n\n");
			}
			else
			{
				fprintf(fp1,"Summary: test FAILED\n");
				fprintf(fp1,"Number of bad channels for 1st result file = %d\n",v2_numb_bad_chan1);
				fprintf(fp1,"See *07.bad file for more informaion\n\n\n");      
			}
			fprintf(fp1,"Result table (time offsets, ns): \n\n");
			fprintf(fp1,"Strip#   Layer1   Layer2   Layer3   Layer4   Layer5   Layer6  \n");
      
			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{
				fprintf(fp1,"  %2d   %8.1f %8.1f %8.1f %8.1f %8.1f %8.1f \n",
						(v2_nstrip+1),
						v2_time_max_rel[0][v2_nstrip],
						v2_time_max_rel[1][v2_nstrip],
						v2_time_max_rel[2][v2_nstrip],
						v2_time_max_rel[3][v2_nstrip],
						v2_time_max_rel[4][v2_nstrip],
						v2_time_max_rel[5][v2_nstrip]
					);
	
			}
			fclose(fp1);
			free(filename);
		}



//2222222222222222222222222222222222222222222222222222222
		filename = malloc(strlen(datadir_translation) + 
						  strlen(v2_test_17_02_result) +5 );
		if (filename == NULL)
		{
			printf("malloc() failed on filename.\n");
			return 1;
		}
		sprintf(filename,"%s%s",datadir_translation,v2_test_17_02_result);

		fp1 = fopen(filename, "w");
		if(fp1 == NULL)
		{
			printf("File %s could not be opened or created.\n",filename);
			free(filename);
		}
		else
		{
			fprintf(fp1,"Test 17:            CFEB calibration\n");
			fprintf(fp1,"Test_17_02.result:  Output signal amplitudes Apeak for Qin=100fC vs (plane, strip)\n");
			fprintf(fp1,"Chamber#            %s\n", csc_name);
			fprintf(fp1,"Test performed:     see *.ps files coorresponding to this test\n");
			fprintf(fp1,"Analysis done:      %s\n",timestring);
			if (v2_numb_bad_chan2 == 0)
			{
				fprintf(fp1,"Summary: test OK\n\n\n");
			}
			else
			{
				fprintf(fp1,"Summary: test FAILED\n");
				fprintf(fp1,"Number of bad channels for 2nd result file = %d\n",v2_numb_bad_chan2);
				fprintf(fp1,"See *07.bad file for more informaion\n\n\n");      
			}
			fprintf(fp1,"Result table (amplitudes, ADC): \n\n");
			fprintf(fp1,"Strip#   Layer1   Layer2   Layer3   Layer4   Layer5   Layer6  \n");

			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{
				fprintf(fp1,"  %2d   %8.1f %8.1f %8.1f %8.1f %8.1f %8.1f \n",
						(v2_nstrip+1),
						v2_ampl_max[0][v2_nstrip],
						v2_ampl_max[1][v2_nstrip],
						v2_ampl_max[2][v2_nstrip],
						v2_ampl_max[3][v2_nstrip],
						v2_ampl_max[4][v2_nstrip],
						v2_ampl_max[5][v2_nstrip]
					);
	
			}

			fclose(fp1);
			free(filename);
		}



//333333333333333333333333333333333333333333333333333333333
		filename = malloc(strlen(datadir_translation) + 
						  strlen(v2_test_17_03_result) +5 );
		if (filename == NULL)
		{
			printf("malloc() failed on filename.\n");
			return 1;
		}
		sprintf(filename,"%s%s",datadir_translation,v2_test_17_03_result);

		fp1 = fopen(filename, "w");
		if(fp1 == NULL)
		{
			printf("File %s could not be opened or created.\n",filename);
			free(filename);
		}
		else
		{
			fprintf(fp1,"Test 17:            CFEB calibration\n");
			fprintf(fp1,"Test_17_03.result:  Output signal shape distortion: A-five-sample-peak/Apeak vs (plane, strip)\n");
			fprintf(fp1,"Chamber#            %s\n", csc_name);
			fprintf(fp1,"Test performed:     see *.ps files coorresponding to this test\n");
			fprintf(fp1,"Analysis done:      %s\n",timestring);
			if (v2_numb_bad_chan3 == 0)
			{
				fprintf(fp1,"Summary: test OK\n\n\n");
			}
			else
			{
				fprintf(fp1,"Summary: test FAILED\n");
				fprintf(fp1,"Number of bad channels for 3rd result file = %d\n",v2_numb_bad_chan3);
				fprintf(fp1,"See *07.bad file for more informaion\n\n\n");      
			}
			fprintf(fp1,"Result table (shape distortion): \n\n");
			fprintf(fp1,"Strip#   Layer1   Layer2   Layer3   Layer4   Layer5   Layer6  \n");

			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{
				fprintf(fp1,"  %2d   %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f \n",
						(v2_nstrip+1),
						v2_charge[0][v2_nstrip],
						v2_charge[1][v2_nstrip],
						v2_charge[2][v2_nstrip],
						v2_charge[3][v2_nstrip],
						v2_charge[4][v2_nstrip],
						v2_charge[5][v2_nstrip]
					);	
			}

			fclose(fp1);
			free(filename);
		}



//444444444444444444444444444444444444444444444444444444444444444444
		filename = malloc(strlen(datadir_translation) + 
						  strlen(v2_test_17_04_result) +5 );
		if (filename == NULL)
		{
			printf("malloc() failed on filename.\n");
			return 1;
		}
		sprintf(filename,"%s%s",datadir_translation,v2_test_17_04_result);

		fp1 = fopen(filename, "w");
		if(fp1 == NULL)
		{
			printf("File %s could not be opened or created.\n",filename);
			free(filename);
		}
		else
		{
			fprintf(fp1,"Test 17:            CFEB calibration\n");
			fprintf(fp1,"Test_17_04.result:  Left-side crosstalk vs (plane, strip)\n");
			fprintf(fp1,"Chamber#            %s\n", csc_name);
			fprintf(fp1,"Test performed:     see *.ps files coorresponding to this test\n");
			fprintf(fp1,"Analysis done:      %s\n",timestring);
			if (v2_numb_bad_chan4 == 0)
			{
				fprintf(fp1,"Summary: test OK\n\n\n");
			}
			else
			{
				fprintf(fp1,"Summary: test FAILED\n");
				fprintf(fp1,"Number of bad channels for 4th result file = %d\n",v2_numb_bad_chan4);
				fprintf(fp1,"See *07.bad file for more informaion\n\n\n");      
			}
			fprintf(fp1,"Result table (left crosstalk): \n\n");
			fprintf(fp1,"Strip#   Layer1   Layer2   Layer3   Layer4   Layer5   Layer6  \n");

			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{
				fprintf(fp1,"  %2d   %8.4f %8.4f %8.4f %8.4f %8.4f %8.4f \n",
						(v2_nstrip+1),
						v2_left_crosstalk[0][v2_nstrip],
						v2_left_crosstalk[1][v2_nstrip],
						v2_left_crosstalk[2][v2_nstrip],
						v2_left_crosstalk[3][v2_nstrip],
						v2_left_crosstalk[4][v2_nstrip],
						v2_left_crosstalk[5][v2_nstrip]
					);		
			}
			fclose(fp1);
			free(filename);
		}



//555555555555555555555555555555555555555555555555555555555555555555
		filename = malloc(strlen(datadir_translation) + 
						  strlen(v2_test_17_05_result) +5);
		if (filename == NULL)
		{
			printf("malloc() failed on filename.\n");
			return 1;
		}
		sprintf(filename,"%s%s",datadir_translation,v2_test_17_05_result);

		fp1 = fopen(filename, "w");
		if(fp1 == NULL)
		{
			printf("File %s could not be opened or created.\n",filename);
			free(filename);
		}
		else
		{
			fprintf(fp1,"Test 17:            CFEB calibration\n");
			fprintf(fp1,"Test_17_05.result:  Right-side crosstalk vs (plane, strip)\n");
			fprintf(fp1,"Chamber#            %s\n", csc_name);
			fprintf(fp1,"Test performed:     see *.ps files coorresponding to this test\n");
			fprintf(fp1,"Analysis done:      %s\n",timestring);
			if (v2_numb_bad_chan5 == 0)
			{
				fprintf(fp1,"Summary: test OK\n\n\n");
			}
			else
			{
				fprintf(fp1,"Summary: test FAILED\n");
				fprintf(fp1,"Number of bad channels for 5th result file = %d\n",v2_numb_bad_chan5);
				fprintf(fp1,"See *07.bad file for more informaion\n\n\n");      
			}
			fprintf(fp1,"Result table (right crosstalk): \n\n");
			fprintf(fp1,"Strip#   Layer1   Layer2   Layer3   Layer4   Layer5   Layer6  \n");

			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{
				fprintf(fp1,"  %2d   %8.4f %8.4f %8.4f %8.4f %8.4f %8.4f \n",
						(v2_nstrip+1),
						v2_right_crosstalk[0][v2_nstrip],
						v2_right_crosstalk[1][v2_nstrip],
						v2_right_crosstalk[2][v2_nstrip],
						v2_right_crosstalk[3][v2_nstrip],
						v2_right_crosstalk[4][v2_nstrip],
						v2_right_crosstalk[5][v2_nstrip]
					);		
			}
			fclose(fp1);
			free(filename);
		}



//66666666666666666666666666666666666666666666666666666666666666666666666
		filename = malloc(strlen(datadir_translation) + 
						  strlen(v2_test_17_06_result) + 5);
		if (filename == NULL)
		{
			printf("malloc() failed on filename.\n");
			return 1;
		}
		sprintf(filename,"%s%s",datadir_translation,v2_test_17_06_result);

		fp1 = fopen(filename, "w");
		if(fp1 == NULL)
		{
			printf("File %s could not be opened or created.\n",filename);
			free(filename);
		}
		else
		{
			fprintf(fp1,"Test 17:            CFEB calibration\n");
			fprintf(fp1,"Test_17_06.result:  Long range crosstalk vs (plane, strip)\n");
			fprintf(fp1,"Chamber#            %s\n", csc_name);
			fprintf(fp1,"Test performed:     see *.ps files coorresponding to this test\n");
			fprintf(fp1,"Analysis done:      %s\n",timestring);
			if (v2_numb_bad_chan5 == 0)
			{
				fprintf(fp1,"Summary: test OK\n\n\n");
			}
			else
			{
				fprintf(fp1,"Summary: test FAILED\n");
				fprintf(fp1,"Number of bad channels for 6th result file = %d\n",v2_numb_bad_chan6);
				fprintf(fp1,"See *07.bad file for more informaion\n\n\n");      
			}
			fprintf(fp1,"Result table (long range crosstalk): \n\n");
			fprintf(fp1,"Strip#   Layer1    Layer2    Layer3    Layer4    Layer5    Layer6  \n");

			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{
				fprintf(fp1,"  %2d   %9.6f %9.6f %9.6f %9.6f %9.6f %9.6f \n",
						(v2_nstrip+1),
						v2_charge_longxt_output[0][v2_nstrip],
						v2_charge_longxt_output[1][v2_nstrip],
						v2_charge_longxt_output[2][v2_nstrip],
						v2_charge_longxt_output[3][v2_nstrip],
						v2_charge_longxt_output[4][v2_nstrip],
						v2_charge_longxt_output[5][v2_nstrip]
					);		
			}
			fclose(fp1);
			free(filename);
		}


    }







//FOR DAC SCAN MODE..............................................
    else if (scan_mode == DAC_SCAN) 
    {

		//SLOPE AND INTERCEPT CALCULATIONS START HERE
		for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
		{
			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{
				v2_L = 0;
				v2_M = 0;
				v2_N = 0;
				v2_O = 0;
				v2_P = 0;
				v2_dac_err[v2_nlayer][v2_nstrip] = 0;
				for(v2_dac=0;v2_dac<DAC_SCAN_NPOINTS;v2_dac++)
				{
					v2_ampl_output[v2_nlayer][v2_nstrip][v2_dac] = 
						v2_ampl_output[v2_nlayer][v2_nstrip][v2_dac]/
						v2_dac_counter[v2_nlayer][v2_nstrip][v2_dac];
					v2_charge_output[v2_nlayer][v2_nstrip][v2_dac] = 
						v2_charge_output[v2_nlayer][v2_nstrip][v2_dac]/
						v2_dac_counter[v2_nlayer][v2_nstrip][v2_dac];
					v2_charge_sigma_output[v2_nlayer][v2_nstrip][v2_dac] = 
						sqrt(10*10 + 0.01*v2_charge_output[v2_nlayer][v2_nstrip][v2_dac]*
							 0.01*v2_charge_output[v2_nlayer][v2_nstrip][v2_dac]);
					if(v2_ampl_output[v2_nlayer][v2_nstrip][v2_dac] < 2000)
					{
						v2_sigma_squared = v2_charge_sigma_output[v2_nlayer][v2_nstrip][v2_dac]*
							v2_charge_sigma_output[v2_nlayer][v2_nstrip][v2_dac];
						v2_L = v2_L + v2_charge_output[v2_nlayer][v2_nstrip][v2_dac]*
							v2_test_amp[v2_nlayer][v2_nstrip][v2_dac]/v2_sigma_squared;
						v2_M = v2_M + v2_test_amp[v2_nlayer][v2_nstrip][v2_dac]*
							v2_test_amp[v2_nlayer][v2_nstrip][v2_dac]/v2_sigma_squared;
						v2_N = v2_N + v2_test_amp[v2_nlayer][v2_nstrip][v2_dac]/v2_sigma_squared;
						v2_O = v2_O + v2_charge_output[v2_nlayer][v2_nstrip][v2_dac]/v2_sigma_squared;
						v2_P = v2_P + 1.0/v2_sigma_squared;	  
					}
				}
				v2_slope[v2_nlayer][v2_nstrip] = (v2_L*v2_P-v2_O*v2_N)/(v2_P*v2_M-v2_N*v2_N);
				v2_interseption[v2_nlayer][v2_nstrip] = (v2_O-v2_N*v2_slope[v2_nlayer][v2_nstrip])/v2_P;
			}
		}
    

		for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
		{
			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{
				v2_moment1[v2_nlayer][v2_nstrip]= 0;
				v2_counter = 0;
				for(v2_dac=0;v2_dac<DAC_SCAN_NPOINTS;v2_dac++)
				{
					if(v2_ampl_output[v2_nlayer][v2_nstrip][v2_dac] < 2000)
					{
						v2_moment1[v2_nlayer][v2_nstrip] = v2_moment1[v2_nlayer][v2_nstrip] + 
							(v2_charge_output[v2_nlayer][v2_nstrip][v2_dac] - 
							 v2_slope[v2_nlayer][v2_nstrip]*v2_test_amp[v2_nlayer][v2_nstrip][v2_dac] - 
							 v2_interseption[v2_nlayer][v2_nstrip])/
							v2_charge_sigma_output[v2_nlayer][v2_nstrip][v2_dac];  
						v2_counter = v2_counter + 1;
					}
				}
				v2_moment1[v2_nlayer][v2_nstrip] = v2_moment1[v2_nlayer][v2_nstrip]/v2_counter;
			}
		}
		for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
		{
			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{
				v2_moment2[v2_nlayer][v2_nstrip]= 0;
				v2_counter = 0;
				for(v2_dac=0;v2_dac<DAC_SCAN_NPOINTS;v2_dac++)
				{
					if(v2_ampl_output[v2_nlayer][v2_nstrip][v2_dac] < 2000)
					{
						v2_moment2[v2_nlayer][v2_nstrip] = v2_moment2[v2_nlayer][v2_nstrip] + 
							(v2_moment1[v2_nlayer][v2_nstrip] - 
							 (v2_charge_output[v2_nlayer][v2_nstrip][v2_dac] - 
							  v2_slope[v2_nlayer][v2_nstrip]*v2_test_amp[v2_nlayer][v2_nstrip][v2_dac] - 
							  v2_interseption[v2_nlayer][v2_nstrip])/
							 v2_charge_sigma_output[v2_nlayer][v2_nstrip][v2_dac])*
							(v2_moment1[v2_nlayer][v2_nstrip] - 
							 (v2_charge_output[v2_nlayer][v2_nstrip][v2_dac] - 
							  v2_slope[v2_nlayer][v2_nstrip]*v2_test_amp[v2_nlayer][v2_nstrip][v2_dac] - 
							  v2_interseption[v2_nlayer][v2_nstrip])/
							 v2_charge_sigma_output[v2_nlayer][v2_nstrip][v2_dac]);
						v2_counter = v2_counter + 1;
					}
				}
				v2_moment2[v2_nlayer][v2_nstrip] = sqrt(v2_moment2[v2_nlayer][v2_nstrip]/v2_counter);
			}
		}

		//WRITING *.ps FILES FOR DAC SCAN STARTS HERE
		//SLOPE
		limits[0] =  SLOPE_FRAME_BOTTOM;
		limits[1] =  SLOPE_FRAME_UP;
		limits[2] =  SLOPE_FRAME_BOTTOM_SUM;
		limits[3] =  SLOPE_FRAME_UP_SUM;
		limits[4] =  SLOPE_BOTTOM_BOUND;
		limits[5] =  SLOPE_UP_BOUND;
		limits[6] = nstrips;
		one_page_plot_v3(17, 8, STRIP_PLOT, (float *)v2_slope, (float *)v2_dac_err,
						 limits, csc_name, "Slope (5-sample-sum vs fC)", 
						 "slope, ADC/fC", 1);

		//INTERCEPT
		limits[0] =  INTERCEPT_FRAME_BOTTOM;
		limits[1] =  INTERCEPT_FRAME_UP;
		limits[2] =  INTERCEPT_FRAME_BOTTOM_SUM;
		limits[3] =  INTERCEPT_FRAME_UP_SUM;
		limits[4] =  INTERCEPT_BOTTOM_BOUND;
		limits[5] =  INTERCEPT_UP_BOUND;
		limits[6] = nstrips;
		one_page_plot_v3(17, 9, STRIP_PLOT, (float *)v2_interseption, (float *)v2_dac_err,
						 limits, csc_name, "Intercept (5-sample-sum vs fC)", 
						 "Intercept, ADC", 1);

		//RMS OF DEVIATION
		limits[0] =  RMS_FRAME_BOTTOM;
		limits[1] =  RMS_FRAME_UP;
		limits[2] =  RMS_FRAME_BOTTOM_SUM;
		limits[3] =  RMS_FRAME_UP_SUM;
		limits[4] =  RMS_BOTTOM_BOUND;
		limits[5] =  RMS_UP_BOUND;
		limits[6] = nstrips;
		one_page_plot_v3(17, 10, STRIP_PLOT, (float *)v2_moment2, (float *)v2_dac_err,
						 limits, csc_name, "RMS of deviation (deviation/sigma)", 
						 "Non-linearity, RMS of dev./sigma", 1);




		//WRITING HISTOGRAMS FOR DAC SCAN STARTS HERE
		file_his_name = malloc(strlen(datadir_translation) + 
							   strlen(v2_test_17_file_his_dac) + 5);
		if (file_his_name == NULL)
		{
			printf("malloc() failed for file_his_name.  "
				   "Could not save histograms.\n");
		}
		else
		{
			sprintf(file_his_name,"%s%s",datadir_translation,v2_test_17_file_his_dac);
			printf(" Saving DAC histograms\n");

			for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
			{
				for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
				{
					v2_mid_hid = 4000 + 100*(v2_nlayer+1) + v2_nstrip+1;
					sprintf(v2_htitle, "Charge vs fC for strip# %d in layer# %d",
							(v2_nstrip+1),(v2_nlayer+1));
					HBOOK1(v2_mid_hid,v2_htitle,DAC_SCAN_NPOINTS,0.0,260.0/DAC2FC,0.);
					v2_left_hid = 5000 + 100*(v2_nlayer+1) + v2_nstrip+1;
					sprintf(v2_htitle, "delta Charge vs fC for strip# %d in layer# %d",
							(v2_nstrip+1),(v2_nlayer+1));
					HBOOK1(v2_left_hid,v2_htitle,DAC_SCAN_NPOINTS,0.0,260.0/DAC2FC,0.);

					for(v2_dac=0;v2_dac<DAC_SCAN_NPOINTS;v2_dac++)
					{
						HFILL(v2_mid_hid,(float)(v2_test_amp[v2_nlayer][v2_nstrip][v2_dac]),
							  0.,v2_charge_output[v2_nlayer][v2_nstrip][v2_dac]);
						HFILL(v2_left_hid,(float)(v2_test_amp[v2_nlayer][v2_nstrip][v2_dac]),
							  0.,(v2_charge_output[v2_nlayer][v2_nstrip][v2_dac] - 
								  v2_slope[v2_nlayer][v2_nstrip]*v2_test_amp[v2_nlayer][v2_nstrip][v2_dac] - 
								  v2_interseption[v2_nlayer][v2_nstrip])/
							  v2_charge_sigma_output[v2_nlayer][v2_nstrip][v2_dac]);
					}
				}
			}      
			HRPUT(0, file_his_name, "N");
			HDELET(0);	    
			free(file_his_name);    
		}







//11--------------11---------------11-----------------11------------------11-----------------11
		v2_numb_bad_chan1 = 0;
		v2_numb_bad_chan2 = 0;
		v2_numb_bad_chan3 = 0;

		filename = malloc(strlen(datadir_translation) + 
						  strlen(v2_test_17_11_bad) +5 );
		if (filename == NULL)
		{
			printf("malloc() failed on filename.\n");
			return 1;
		}
		sprintf(filename,"%s%s",datadir_translation,v2_test_17_11_bad);

		fp1 = fopen(filename, "w");
		if(fp1 == NULL)
		{
			printf("File %s could not be opened or created.\n",filename);
			free(filename);
		}
		else
		{
			fprintf(fp1,"Test 17:            Comparator output timing\n");
			fprintf(fp1,"Test_17_11.bad:     bad channels (DAC SCAN)\n");
			fprintf(fp1,"Chamber#            %s\n", csc_name);
			fprintf(fp1,"Test performed:     see *.ps files coorresponding to this test\n");
			fprintf(fp1,"Analysis done:      %s\n\n\n",timestring);
      
//      fprintf(fp1,"NOTE: 0 - means OK, 1 - BAD\n\n");
			fprintf(fp1,"NOTE: \n");
			fprintf(fp1,"Slope bounds: %3.1f %3.1f\n",SLOPE_BOTTOM_BOUND,SLOPE_UP_BOUND);
			fprintf(fp1,"Intercept bounds: %6.1f %6.1f\n",INTERCEPT_BOTTOM_BOUND,INTERCEPT_UP_BOUND);
			fprintf(fp1,"RMS bounds: %4.1f %4.1f\n\n",RMS_BOTTOM_BOUND,RMS_UP_BOUND);

      
			bad1=0;
			bad2=0;
			bad3=0;
//      fprintf(fp1,"Plane#   Strip#  Slope  Interc.   RMS    problem_code\n");
			for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
			{
				for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
				{
	  	  
					if(v2_slope[v2_nlayer][v2_nstrip] < SLOPE_BOTTOM_BOUND ||
					   v2_slope[v2_nlayer][v2_nstrip] > SLOPE_UP_BOUND)
					{
						if(bad1==0)
						{
							fprintf(fp1,"Plane#   Strip#  Slope    problem_code\n"); 
						}
						bad1 = 1;
						v2_numb_bad_chan1 = v2_numb_bad_chan1 + 1;
						fprintf(fp1,"  %d       %2d %8.2f   out of range\n",
								(v2_nlayer+1),(v2_nstrip+1),
								v2_slope[v2_nlayer][v2_nstrip]);
					}
				}
			}

			for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
			{
				for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
				{
					if(v2_interseption[v2_nlayer][v2_nstrip] < INTERCEPT_BOTTOM_BOUND ||
					   v2_interseption[v2_nlayer][v2_nstrip] > INTERCEPT_UP_BOUND)	  	  
					{
						if(bad2==0)
						{
							fprintf(fp1,"Plane#   Strip#  Interc.    problem_code\n"); 
						}
						bad2 = 1;
						v2_numb_bad_chan2 = v2_numb_bad_chan2 + 1;
						fprintf(fp1,"  %d       %2d %8.2f   out of range\n",
								(v2_nlayer+1),(v2_nstrip+1),
								v2_interseption[v2_nlayer][v2_nstrip]);
					}
				}
			}

			for(v2_nlayer=0;v2_nlayer<NLAYER;v2_nlayer++)
			{
				for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
				{
					if(v2_moment2[v2_nlayer][v2_nstrip] < RMS_BOTTOM_BOUND ||
					   v2_moment2[v2_nlayer][v2_nstrip] > RMS_UP_BOUND)
					{
						if(bad3==0)
						{
							fprintf(fp1,"Plane#   Strip#  RMS    problem_code\n"); 
						}
						bad3 = 1;
						v2_numb_bad_chan3 = v2_numb_bad_chan3 + 1;
						fprintf(fp1,"  %d       %2d %8.3f   out of range\n",
								(v2_nlayer+1),(v2_nstrip+1),
								v2_moment2[v2_nlayer][v2_nstrip]);
					}
				}
			}


			if ((v2_numb_bad_chan1 == 0) && (v2_numb_bad_chan2 == 0) && (v2_numb_bad_chan3 == 0))
			{
				fprintf(fp1,"\n\n\nSummary: test OK\n\n");
			}
			else
			{
				fprintf(fp1,"\n\n\nSummary: test FAILED\n");
				fprintf(fp1,"Number of errors in all channels = %d\n\n",
						(v2_numb_bad_chan1+v2_numb_bad_chan2+v2_numb_bad_chan3));
			}

			fclose(fp1);
			free(filename);
		}






		//WRITING *.result FILES FOR DAC SCAN STARTS HERE
		//NOTE: the number of *.result file correspond to *.ps one
//888888888888888888888888888888888888888888888888888888888888
		filename = malloc(strlen(datadir_translation) + 
						  strlen(v2_test_17_08_result) +5 );
		if (filename == NULL)
		{
			printf("malloc() failed on filename.\n");
			return 1;
		}
		sprintf(filename,"%s%s",datadir_translation,v2_test_17_08_result);

		fp1 = fopen(filename, "w");
		if(fp1 == NULL)
		{
			printf("File %s could not be opened or created.\n",filename);
			free(filename);
		}
		else
		{
			fprintf(fp1,"Test 17:            CFEB calibration\n");
			fprintf(fp1,"Test_17_08.result:  ADC (Q) vs fC slope vs (plane, strip)\n");
			fprintf(fp1,"Chamber#            %s\n", csc_name);
			fprintf(fp1,"Test performed:     see *.ps files coorresponding to this test\n");
			fprintf(fp1,"Analysis done:      %s\n",timestring);
			if (v2_numb_bad_chan1 == 0)
			{
				fprintf(fp1,"Summary: test OK\n\n\n");
			}
			else
			{
				fprintf(fp1,"Summary: test FAILED\n");
				fprintf(fp1,"Number of bad channels for 8th result file = %d\n",v2_numb_bad_chan1);
				fprintf(fp1,"See *11.bad file for more informaion\n\n\n");      
			}
			fprintf(fp1,"Result table (slope): \n\n");
			fprintf(fp1,"Strip#   Layer1   Layer2   Layer3   Layer4   Layer5   Layer6\n");

			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{
				fprintf(fp1,"  %2d   %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f \n",
						(v2_nstrip+1),
						v2_slope[0][v2_nstrip],v2_slope[1][v2_nstrip],v2_slope[2][v2_nstrip],
						v2_slope[3][v2_nstrip],v2_slope[4][v2_nstrip],v2_slope[5][v2_nstrip]
					);
			}
			fclose(fp1);
			free(filename);
		}



//99999999999999999999999999999999999999999999999999999999999
		filename = malloc(strlen(datadir_translation) + 
						  strlen(v2_test_17_09_result) +5 );
		if (filename == NULL)
		{
			printf("malloc() failed on filename.\n");
			return 1;
		}
		sprintf(filename,"%s%s",datadir_translation,v2_test_17_09_result);

		fp1 = fopen(filename, "w");
		if(fp1 == NULL)
		{
			printf("File %s could not be opened or created.\n",filename);
			free(filename);
		}
		else
		{
			fprintf(fp1,"Test 17:            CFEB calibration\n");
			fprintf(fp1,"Test_17_09.result:  ADC (Q) vs fC intercept vs (plane, strip)\n");
			fprintf(fp1,"Chamber#            %s\n", csc_name);
			fprintf(fp1,"Test performed:     see *.ps files coorresponding to this test\n");
			fprintf(fp1,"Analysis done:      %s\n",timestring);
			if (v2_numb_bad_chan2 == 0)
			{
				fprintf(fp1,"Summary: test OK\n\n\n");
			}
			else
			{
				fprintf(fp1,"Summary: test FAILED\n");
				fprintf(fp1,"Number of bad channels for 9th result file = %d\n",v2_numb_bad_chan2);
				fprintf(fp1,"See *11.bad file for more informaion\n\n\n");      
			}
			fprintf(fp1,"Result table (intercept): \n\n");
			fprintf(fp1,"Strip#   Layer1   Layer2   Layer3   Layer4   Layer5   Layer6\n");

			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{
				fprintf(fp1,"  %2d   %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f \n",
						(v2_nstrip+1),
						v2_interseption[0][v2_nstrip],
						v2_interseption[1][v2_nstrip],
						v2_interseption[2][v2_nstrip],
						v2_interseption[3][v2_nstrip],
						v2_interseption[4][v2_nstrip],
						v2_interseption[5][v2_nstrip]
					);		  
			}
			fclose(fp1);
			free(filename);
		}




//10----------10-----------------10------------------10-----------10----------10
		filename = malloc(strlen(datadir_translation) + 
						  strlen(v2_test_17_10_result) +5 );
		if (filename == NULL)
		{
			printf("malloc() failed on filename.\n");
			return 1;
		}
		sprintf(filename,"%s%s",datadir_translation,v2_test_17_10_result);

		fp1 = fopen(filename, "w");
		if(fp1 == NULL)
		{
			printf("File %s could not be opened or created.\n",filename);
			free(filename);
		}
		else
		{
			fprintf(fp1,"Test 17:            CFEB calibration\n");
			fprintf(fp1,"Test_17_10.result:  Goodness of 'fit' linearity vs (plane, strip)\n");
			fprintf(fp1,"Chamber#            %s\n", csc_name);
			fprintf(fp1,"Test performed:     see *.ps files coorresponding to this test\n");
			fprintf(fp1,"Analysis done:      %s\n",timestring);
			if (v2_numb_bad_chan3 == 0)
			{
				fprintf(fp1,"Summary: test OK\n\n\n");
			}
			else
			{
				fprintf(fp1,"Summary: test FAILED\n");
				fprintf(fp1,"Number of bad channels for 10th result file = %d\n",v2_numb_bad_chan3);
				fprintf(fp1,"See *11.bad file for more informaion\n\n\n");      
			}
			fprintf(fp1,"Result table (intercept): \n\n");
			fprintf(fp1,"Strip#   Layer1   Layer2   Layer3   Layer4   Layer5   Layer6\n");

			for(v2_nstrip=0;v2_nstrip<nstrips;v2_nstrip++)
			{
				fprintf(fp1,"  %2d   %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f \n",
						(v2_nstrip+1),
						v2_moment2[0][v2_nstrip],
						v2_moment2[1][v2_nstrip],
						v2_moment2[2][v2_nstrip],
						v2_moment2[3][v2_nstrip],
						v2_moment2[4][v2_nstrip],
						v2_moment2[5][v2_nstrip]
					);
			}
			fclose(fp1);
			free(filename);
		}





    }

    HDELET(0);    
    return 0;
}
