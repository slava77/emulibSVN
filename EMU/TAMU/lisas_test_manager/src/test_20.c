/*
 * test_20.c (in V1.2 test_15.c)
 * Comparator Timing Test
 *
 * previous version was written by: 
 * Author : Lisa Gorn
 * Date   : April 3, 2001
 * 
 * Version#2 (without any fits)
 * (in current version there is practically nothing from the old one)
 *
 * AUTHOR: Alexei Drozdetski
 * STARTED: 05/24/02
 * FINISHED: 05/28/02
 * MODIFIED: no modifications yet
 *
 * Contact information: drozdetski@phys.ufl.edu
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
#include "csc_event.h"       /* unpacked data */
#include "misc.h"            /* definition of logical data type */
#include "one_page_plot.h"   /* STRIP_PLOT,...         */
#include "test_utils.h"      /* chamber labels         */
#include "test_params.h"     /* test config parameters */

//#include "csc_unpacker.h"
//#include "emu/daq/reader/RawDataFile.h"

#define  NFIFO_TBINS    7

// Local function declarations 
int test_20_begin(int pass);
int test_20_event(int pass);
int test_20_end(int pass);
int test_20_finish(void);

// Global variables local to this test 
#define NTEST_STRIPS 16
#define NDELAYS 16

#define TIME_OFFSET_FRAME_MIN -50.0
#define TIME_OFFSET_FRAME_MAX 50.0
#define TIME_OFFSET_LOWER_BOUND -25.0
#define TIME_OFFSET_UPPER_BOUND 25.0

static logical    first = _TRUE;
static FILE *fp1;
char *v2_filename;
char *v2_file_his_name;
char v2_test_20_01_result[]="/test_results/temp/test_20_01.result";
char v2_test_20_02_bad[]="/test_results/temp/test_20_02.bad";
char v2_test_20_file_his[]="/test_results/temp/test_20_01.his";

float v2_delay[NLAYER][NCFEB][NTEST_STRIPS][NDELAYS];
float v2_clock[NLAYER][NCFEB][NTEST_STRIPS][NDELAYS];
float v2_delay_counter[NLAYER][NCFEB][NTEST_STRIPS][NDELAYS];
int v2_ilayer,v2_icfeb,v2_itest_strip,v2_idelay,v2_istrip;
int v2_max_idelay;
float v2_max_delay_value;
float v2_average_clock[NLAYER][NCFEB][NTEST_STRIPS];
float v2_average_cfeb_clock[NCFEB];

//float v2_time_offset[NLAYER][NSTRIP];
//float v2_time_offset_output[NLAYER][NSTRIP];

float v2_average_clock_output[NLAYER][NSTRIP][NDELAYS];

char v2_htitle[50];
int bad1,v2_hid;
int v2_numb_bad_chan;

static float   *v2_time_offset[NLAYER], *v2_time_offset_output[NLAYER];
static float   *pv2_time_offset, *pv2_time_offset_output;


/*******************************************************************************
   Define functions and parameters for FAST site test 20
*******************************************************************************/
static int nstrips, ncfebs;

int test_20_init(void)
{

    int l;
    test_begin  = test_20_begin;
    test_event  = test_20_event;
    test_end    = test_20_end;
    test_finish = test_20_finish;

    //arrays initialization...
    for(v2_ilayer=0;v2_ilayer<NLAYER;v2_ilayer++)
    {
	for(v2_icfeb=0;v2_icfeb<ncfebs;v2_icfeb++)
	{
	    for(v2_itest_strip=0;v2_itest_strip<NTEST_STRIPS;v2_itest_strip++)
	    {   
		for(v2_idelay=0;v2_idelay<NDELAYS;v2_idelay++)
		{
		    v2_delay[v2_ilayer][v2_icfeb][v2_itest_strip][v2_idelay] = 0.0;
		    v2_delay_counter[v2_ilayer][v2_icfeb][v2_itest_strip][v2_idelay] = 0.000001;
		    v2_clock[v2_ilayer][v2_icfeb][v2_itest_strip][v2_idelay] = 0.0;
		}
	    }
	}
    }

    num_passes = 1;
    num_events[0] = 100000;

    switch (csc_type)
    {
    case 2:  nstrips = 64; ncfebs = 4; break;
    default: nstrips = 80; ncfebs = 5; break;
    }

    printf ("csc_type = %d, nstrips = %d\n", csc_type, nstrips);

    pv2_time_offset        = malloc (sizeof(float) * NLAYER * nstrips);
    pv2_time_offset_output = malloc (sizeof(float) * NLAYER * nstrips);

    for (l = 0; l < NLAYER; l++)
    {
	v2_time_offset       [l] = pv2_time_offset        + nstrips * l;
	v2_time_offset_output[l] = pv2_time_offset_output + nstrips * l;
    }
    return 0;
}








/*******************************************************************************
   Book histograms for FAST site test 20                                       
*******************************************************************************/

int test_20_begin(int pass)
  {
  int      hid, ilayer;
  char     htitle[50];

  if (pass == 0)
    {
// Initializations 
    first = _TRUE;

// Book first part of histograms 
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      hid = ilayer + 1;
      sprintf(htitle, "L%d comparator hit occupancy vs strip", ilayer + 1);
      HBOOK1(hid, htitle, 80., 0.5, 80.5, 0.);
      }
    }

  return 0;
  }




/*******************************************************************************
   Do analysis and fill histograms for FAST site test 20
*******************************************************************************/

int test_20_event(int pass)
  {
  int    j, k, icfeb, delay, distrip, hid, ilayer, istrip, status;
  int    test_strip;
  unsigned int    data;
  static short unsigned int    tbits[16];

  status = 0;

  if (pass == 0)
    {
// On the first event, check that the data file is of the right type 
    if (first)
      {
      first = _FALSE;
      if (upevt_.stripcal_scan_mode != 3) printf("Wrong data file for test 20:"
						 " STRIPCAL scan mode should be 3\n");

      for (j = 0; j < 16; j++) tbits[j] = 1 << j;
      }

// Get current calibration parameters 
    test_strip = upevt_.stripcal_current_strip;
    delay      = upevt_.stripcal_current_value;
    //NOTE: here test_strip=1..16, delay=0..14, NFIFO_TBINS=7
//    printf("strip %d, delay %d\n", test_strip, delay);
// Find the time bin in which the test strip comparator came on
    for (ilayer = 0; ilayer < NLAYER; ilayer++) //0..5
      {
      for (icfeb = 0; icfeb < ncfebs; icfeb++) //0..4
	{
        istrip = 16 * icfeb + test_strip - 1; 
        distrip = istrip / 2; 
        if ((data = upevt_.clct_dump[ilayer][distrip]))
	  {
          k = distrip * 2;
	   for (j = 0; j < upevt_.clct_nbucket; j++)
//          for (j = 0; j < NFIFO_TBINS - 1; j++)
	    {
            if (data & tbits[j])
	      {
              if (data & tbits[j+1]) k++;
              break;
	      }
	    }

// Fill histograms 
//          if (j < NFIFO_TBINS - 1 && k == istrip)
	   if (j < upevt_.clct_nbucket && k == istrip)
	    {
            hid = ilayer + 1;
            HFILL(hid, (float)(k + 1), 0., 1.);


//filling arrays for further calculations
	    v2_delay[ilayer][icfeb][test_strip-1][delay] = (float)delay + 
	      v2_delay[ilayer][icfeb][test_strip-1][delay];
	    v2_delay_counter[ilayer][icfeb][test_strip-1][delay] =
	      v2_delay_counter[ilayer][icfeb][test_strip-1][delay] +1;
	    v2_clock[ilayer][icfeb][test_strip-1][delay] = (float)j + 
	      v2_clock[ilayer][icfeb][test_strip-1][delay];
	    //NOTE: here (for one particular file: delay=0..14,) clock=0..4, counter=1..40
	    }
	  }
	}
      }
    }
  return status;
  }






/*******************************************************************************
   End-of-run analysis for FAST site test 20
*******************************************************************************/

int test_20_end(int pass)
  {
  return 0;
  }




/*******************************************************************************
   End-of-test analysis for FAST site test 20
*******************************************************************************/

int test_20_finish(void)
  {
  float   limits[7];
  time_t  t;
  char    timestring[40];


  //calculations
  for(v2_ilayer=0;v2_ilayer<NLAYER;v2_ilayer++)
    {
    for(v2_icfeb=0;v2_icfeb<ncfebs;v2_icfeb++)
      {
      for(v2_itest_strip=0;v2_itest_strip<NTEST_STRIPS;v2_itest_strip++)
	{   
	for(v2_idelay=0;v2_idelay<NDELAYS;v2_idelay++)
	  {
	  v2_delay[v2_ilayer][v2_icfeb][v2_itest_strip][v2_idelay] = 
	    v2_delay[v2_ilayer][v2_icfeb][v2_itest_strip][v2_idelay]/
	    v2_delay_counter[v2_ilayer][v2_icfeb][v2_itest_strip][v2_idelay];
	  v2_clock[v2_ilayer][v2_icfeb][v2_itest_strip][v2_idelay] = 
	    v2_clock[v2_ilayer][v2_icfeb][v2_itest_strip][v2_idelay]/
	    v2_delay_counter[v2_ilayer][v2_icfeb][v2_itest_strip][v2_idelay];

	  if(v2_max_delay_value < v2_delay[v2_ilayer][v2_icfeb][v2_itest_strip][v2_idelay])
	    v2_max_idelay = v2_idelay;

	  }
	}
      }
    }

  for(v2_icfeb=0;v2_icfeb<ncfebs;v2_icfeb++)
    {
    for(v2_ilayer=0;v2_ilayer<NLAYER;v2_ilayer++)
      {
      for(v2_itest_strip=0;v2_itest_strip<NTEST_STRIPS;v2_itest_strip++)
	{   
	v2_average_clock[v2_ilayer][v2_icfeb][v2_itest_strip] = 0.0;

	for(v2_idelay=0;v2_idelay<=v2_max_idelay;v2_idelay++)
	  {
	  v2_average_clock[v2_ilayer][v2_icfeb][v2_itest_strip] = 
	    v2_average_clock[v2_ilayer][v2_icfeb][v2_itest_strip] + 
	    v2_clock[v2_ilayer][v2_icfeb][v2_itest_strip][v2_idelay];	    	  

	  v2_istrip = 16 * v2_icfeb + v2_itest_strip;
	  v2_average_clock_output[v2_ilayer][v2_istrip][v2_idelay] = 
	    v2_clock[v2_ilayer][v2_icfeb][v2_itest_strip][v2_idelay];	  
	  }
	
	v2_average_clock[v2_ilayer][v2_icfeb][v2_itest_strip] = 
	  v2_average_clock[v2_ilayer][v2_icfeb][v2_itest_strip]/v2_max_idelay;
	}
      }
    }


  for(v2_icfeb=0;v2_icfeb<ncfebs;v2_icfeb++)
    {
    v2_average_cfeb_clock[v2_icfeb] = 0.0;

    for(v2_ilayer=0;v2_ilayer<NLAYER;v2_ilayer++)
      {
      for(v2_itest_strip=0;v2_itest_strip<NTEST_STRIPS;v2_itest_strip++)
	{   
	v2_average_cfeb_clock[v2_icfeb] = v2_average_cfeb_clock[v2_icfeb] + 
	  v2_average_clock[v2_ilayer][v2_icfeb][v2_itest_strip];
	}
      }
    v2_average_cfeb_clock[v2_icfeb] = v2_average_cfeb_clock[v2_icfeb]/
      (NLAYER*NTEST_STRIPS);
    }




  for(v2_ilayer=0;v2_ilayer<NLAYER;v2_ilayer++)
    {    
    for(v2_icfeb=0;v2_icfeb<ncfebs;v2_icfeb++)
      {
      for(v2_itest_strip=0;v2_itest_strip<NTEST_STRIPS;v2_itest_strip++)
	{   
	v2_istrip = 16 * v2_icfeb + v2_itest_strip;

	v2_time_offset[v2_ilayer][v2_istrip] = 
	  v2_average_clock[v2_ilayer][v2_icfeb][v2_itest_strip] -
	  v2_average_cfeb_clock[v2_icfeb];

	v2_time_offset_output[v2_ilayer][v2_istrip] = 
	  25.0*v2_time_offset[v2_ilayer][v2_istrip];
	if (v2_average_clock[v2_ilayer][v2_icfeb][v2_itest_strip] == 0)
	  v2_time_offset_output[v2_ilayer][v2_istrip] = -999.0;	
	}
      }
    }

  
//saving summary plot
  limits[0] = TIME_OFFSET_FRAME_MIN;
  limits[1] = TIME_OFFSET_FRAME_MAX;
  limits[2] = TIME_OFFSET_FRAME_MIN;
  limits[3] = TIME_OFFSET_FRAME_MAX;
  limits[4] = TIME_OFFSET_LOWER_BOUND;
  limits[5] = TIME_OFFSET_UPPER_BOUND;
  limits[6] = nstrips;
  one_page_plot_v3(20, 1, STRIP_PLOT, (float *)pv2_time_offset_output, (float *)pv2_time_offset,
		   limits, csc_name, "Comparator time offsets (ns)",
		   "time offset, ns", 0);


// Add the date and time 
  time(&t);
  sprintf(timestring, "%s", ctime(&t));


  //writing *.bad file
  v2_numb_bad_chan = 0;
  v2_filename = malloc(strlen(datadir_translation) + 
		    strlen(v2_test_20_02_bad) + 5 );
  if (v2_filename == NULL)
    {
    printf("malloc() failed on filename.\n");
    return 1;
    }
  sprintf(v2_filename,"%s%s",datadir_translation,v2_test_20_02_bad);
  
  fp1 = fopen(v2_filename, "w");
  if(fp1 == NULL)
    {
    printf("File %s could not be opened or created.\n",v2_filename);
    free(v2_filename);
    }
  else
    {
    fprintf(fp1,"Test 20:            Comparator output timing\n");
    fprintf(fp1,"Test_20_02.bad:     bad channels\n");
    fprintf(fp1,"Chamber#            %s\n", csc_name);
    fprintf(fp1,"Test performed:     see *.ps files coorresponding to this test\n");
    fprintf(fp1,"Analisys done:      %s\n\n",timestring);

    fprintf(fp1,"NOTE: \n");
    fprintf(fp1,"Time Offset bounds: %5.1f %5.1f\n\n",TIME_OFFSET_LOWER_BOUND,TIME_OFFSET_UPPER_BOUND);
//    fprintf(fp1,"NOTE: 0 - means OK, 1 - BAD\n\n");    
    fprintf(fp1,"Plane#   Strip#  TimeOffset   problem_code\n");
    
    for(v2_ilayer=0;v2_ilayer<NLAYER;v2_ilayer++)
      {
      for(v2_istrip=0;v2_istrip<nstrips;v2_istrip++)
	{
	bad1=0;
	
	if((v2_time_offset_output[v2_ilayer][v2_istrip] < TIME_OFFSET_LOWER_BOUND) ||
	   (v2_time_offset_output[v2_ilayer][v2_istrip] > TIME_OFFSET_UPPER_BOUND))
	  {
	  bad1 = 1;
	  v2_numb_bad_chan = v2_numb_bad_chan + 1;
	  }
	
	if(bad1!=0)
	  {
	  fprintf(fp1,"  %d        %2d    %6.3f   large time offset\n",
		  (v2_ilayer+1),(v2_istrip+1),v2_time_offset_output[v2_ilayer][v2_istrip]);
	  }
	}
      }

    if (v2_numb_bad_chan == 0)
      {
      fprintf(fp1,"\n\nSummary: test OK\n\n\n");
      }
    else
      {
      fprintf(fp1,"\n\nSummary: test FAILED\n");
      fprintf(fp1,"Number of bad channels = %d\n\n",v2_numb_bad_chan);
      }
    fclose(fp1);
    free(v2_filename);
    }



  //WRITING RESULTS TO *.result FILE FOR DELAY SCAN STARTS HERE
  //NOTE: number of *.result file corresponds to one of the *.ps file
  v2_filename = malloc(strlen(datadir_translation) + 
		    strlen(v2_test_20_01_result) + 5 );
  if (v2_filename == NULL)
    {
    printf("malloc() failed on filename.\n");
    return 1;
    }
  sprintf(v2_filename,"%s%s",datadir_translation,v2_test_20_01_result);
  
  fp1 = fopen(v2_filename, "w");
  if(fp1 == NULL)
    {
    printf("File %s could not be opened or created.\n",v2_filename);
    free(v2_filename);
    }
  else
    {
    fprintf(fp1,"Test 20:            Comparator output timing\n");
    fprintf(fp1,"Test_20_01.result:  Comparator time offsets (ns) vs (plane, strip)\n");
    fprintf(fp1,"Chamber#            %s\n", csc_name);
    fprintf(fp1,"Test performed:     see *.ps files coorresponding to this test\n");
    fprintf(fp1,"Analisys done:      %s\n",timestring);
    if (v2_numb_bad_chan == 0)
      {
      fprintf(fp1,"Summary: test OK\n\n\n");
      }
    else
      {
      fprintf(fp1,"Summary: test FAILED\n");
      fprintf(fp1,"Number of bad channels = %d\n",v2_numb_bad_chan);
      fprintf(fp1,"See *.bad file for more informaion\n\n\n");      
      }
    fprintf(fp1,"Result table (time offsets, ns): \n\n");
    fprintf(fp1,"Strip#   Layer1   Layer2   Layer3   Layer4   Layer5   Layer6  \n");
      
    for(v2_istrip=0;v2_istrip<nstrips;v2_istrip++)
      {
	  
      fprintf(fp1,"  %2d   %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n",
	      (v2_istrip+1),
	      v2_time_offset_output[0][v2_istrip],
	      v2_time_offset_output[1][v2_istrip],
	      v2_time_offset_output[2][v2_istrip],
	      v2_time_offset_output[3][v2_istrip],
	      v2_time_offset_output[4][v2_istrip],
	      v2_time_offset_output[5][v2_istrip]
	      );	    
      }    
    fclose(fp1);
    free(v2_filename);
    }
  


  //writing histogram to file
  v2_file_his_name = malloc(strlen(datadir_translation) + 
			 strlen(v2_test_20_file_his) + 5);
  if (v2_file_his_name == NULL)
    {
    printf("malloc() failed for file_his_name.  "
	   "Could not save histograms.\n");
    }
  else
    {
    sprintf(v2_file_his_name,"%s%s",datadir_translation,v2_test_20_file_his);
    printf(" Saving histograms\n");
    
    
    //BOOKING AND FILLING last part of HISTOGRAMS
    for(v2_ilayer=0;v2_ilayer<NLAYER;v2_ilayer++)
      {
      for(v2_istrip=0;v2_istrip<nstrips;v2_istrip++)
	{
	v2_hid = 1000 + 100*(v2_ilayer+1) + v2_istrip+1;
	sprintf(v2_htitle, "Comparator bin vs pulse delay (strip=%d, layer=%d)",
		(v2_istrip+1),(v2_ilayer+1));
	 HBOOK1(v2_hid,v2_htitle,v2_max_idelay,0.,v2_max_idelay,0.);
	
	for(v2_idelay=0;v2_idelay<v2_max_idelay;v2_idelay++)
	  {
	    HFILL(v2_hid,v2_idelay,0.,
		  (25.0*v2_average_clock_output[v2_ilayer][v2_istrip][v2_idelay]));
	  }
	}
      }    
    HRPUT(0, v2_file_his_name, "N");
    HDELET(0);	    
    free(v2_file_his_name);    
    }


//end of program...
  HDELET(0);
  return 0;
  }
