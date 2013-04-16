/*
 * test_22.c (in V1.2 test_17.c)
 * Comparator Left/Right Offset Tests
 *
 * Author : Lisa Gorn
 * Date   : April 3, 2001
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

#define MAX_NPOINTS     10

/* Local function declarations */
int test_22_begin(int pass);
int test_22_event(int pass);
int test_22_end(int pass);
int test_22_finish(void);
void get_offset_(int *num, int *denom, float *offset, float *offset_err);
void offset_fit_(float intercept[NLAYER][NSTRIP], 
		 float intercept_err[NLAYER][NSTRIP], float chisq[NLAYER][NSTRIP]);

/* Global variables local to this test */
static FILE       *fp;
static logical    first = _TRUE;
static int        first_dac, npoints, triggers_per_strip;


char test_22_file_el[]="/test_results/temp/test_22.results";
char test_22_file_his_el[]="/test_results/temp/test_22_01.his";

/*******************************************************************************
   Define functions and parameters for FAST site test 22
*******************************************************************************/

int test_22_init(void)
  {
  test_begin  = test_22_begin;
  test_event  = test_22_event;
  test_end    = test_22_end;
  test_finish = test_22_finish;

  num_passes = 2;
  num_events[0] = 1;
  num_events[1] = 100000;
  return 0;
  }

/*******************************************************************************
   Book histograms for FAST site test 22                                       
*******************************************************************************/

int test_22_begin(int pass)
  {
  int    hid, ilayer, istrip;
  char   htitle[40];
  char *filename;

  if (pass == 0)
    {
/* Initializations */
    first = _TRUE;
    filename = malloc(strlen(datadir_translation) + 
		      strlen(test_22_file_el) + 1 );
    if (filename == NULL)
      {
      printf("malloc() failed on filename.\n");
      return 1;
      }
    sprintf(filename,"%s%s",datadir_translation,test_22_file_el);

/* Open result file in the temporary directory */
    fp = fopen(filename, "w");
    if(fp == NULL)
      {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
      }
    free(filename);

/* Open result file in the temporary directory */
    
    fprintf(fp, "---------------------------------------------------"
	    "-------\n");
    fprintf(fp, "Results of test 22 (Comparator L/R Offset) for %s\n", 
	    csc_name);
    fprintf(fp, "---------------------------------------------------"
	    "-------\n\n");
    }

  else if (pass == 1)
    {
/* Book histograms */
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      for (istrip = 0; istrip < NSTRIP; istrip++)
	{
        hid = 100 * (ilayer + 1) + (istrip + 1);
        sprintf(htitle, "L%d s%2d offset vs test pulse DAC\n", ilayer + 1,
		istrip + 1);
        HBOOK1(hid, htitle, npoints, first_dac - 0.5, 
	       npoints + first_dac - 0.5, 0.);
	}
      }
    HBARX(0);
    }

  return 0;
  }

/*******************************************************************************
   Do analysis and fill histograms for FAST site test 22
*******************************************************************************/

#define FIRST_IFEB   0

int test_22_event(int pass)
  {
  int                     k, hid, ifeb, ilayer, istrip, jstrip, status;
  int                     distrip, right_halfstrip;
//int                     n;
  static int              this_point;
  static int              nall[NLAYER][NCFEB], nright[NLAYER][NCFEB];
  static unsigned long    tbits[32];
  float                   offset, x;
//float                   xerr;
  float                   offset_err[NLAYER][NSTRIP][MAX_NPOINTS];

  status = 0;

  if (pass == 0)
    {
/* Get current calibration parameters */
    npoints = upevt_.stripcal_num_points;
    if (npoints > MAX_NPOINTS) 
      printf("### ERROR: number of test amplitudes "
	     "(%d) exceeds max expected (%d)\n", npoints, MAX_NPOINTS);

    triggers_per_strip = upevt_.stripcal_trigger_per_step *
      upevt_.stripcal_num_points;
    first_dac = upevt_.stripcal_current_value;
    }

  else if (pass == 1)
    {
    if (first)
      {
      first = _FALSE;
      if (upevt_.stripcal_scan_mode != 5) 
	printf("Wrong data file for test 22:"
	       " STRIPCAL scan mode should be 5, not %d\n", 
	       upevt_.stripcal_scan_mode);
      
      for (k = 0; k < 32; k++) tbits[k] = 1 << k;

/* Debug */ /*
   n = 200;
   for (k = 1; k < n; k++)
   {
   x = k / (float)n;
   xerr = sqrt(k * (n-k) / (float)(n * n * n));
   get_offset_(&k, &n, &offset, &offset_err[0][0][0]);
   printf("%3d  x=%6.3f, xerr=%6.3f, offset=%6.3f, offset_err=%6.3f\n", 
   k, x, xerr, offset, offset_err[0][0][0]);
   }
	    */
      }

/* Zero arrays at the beginning of each point */
    if (upevt_.event_number % upevt_.stripcal_trigger_per_step == 1)
      {
      printf("Event %ld: zeroing arrays...\n", upevt_.event_number);
      for (ilayer = 0; ilayer < NLAYER; ilayer++)
	{
        for (ifeb = 0; ifeb < NCFEB; ifeb++)
	  {
          nall[ilayer][ifeb] = 0;
          nright[ilayer][ifeb] = 0;
	  }
	}

/* Also reset the point counter if we're beginning a new set of test strips */
      if (upevt_.event_number % triggers_per_strip == 1) this_point = 0;
      }

/* Fill arrays */
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      for (ifeb = FIRST_IFEB; ifeb < NCFEB; ifeb++)
	{
        istrip = 16 * ifeb + (upevt_.stripcal_current_strip - 1);
        distrip = istrip / 2;
        jstrip = 0;
        if (upevt_.clct_dump[ilayer][distrip])
	  {
          for (k = 0; k < upevt_.clct_nbucket - 2; k++)
	    {
            if (upevt_.clct_dump[ilayer][distrip] & tbits[k])
	      {
              jstrip = 2 * distrip;
              if (upevt_.clct_dump[ilayer][distrip] & tbits[k+1]) jstrip++;
              if (istrip == jstrip)
		{
                right_halfstrip = (upevt_.clct_dump[ilayer][distrip] & 
				   tbits[k+2]);
                nall[ilayer][ifeb]++;
                if (right_halfstrip) nright[ilayer][ifeb]++;
		}
//            if (nall[ilayer][ifeb] == 0){
//              printf("L%d s%2d: distrip=%2d, triad=%2x, jstrip=%d, "
//               "right=%d\n", ilayer+1, istrip+1, distrip+1, 
//               upevt_.clct_dump[ilayer][distrip], jstrip, right_halfstrip);
//            }
              break;
	      }
	    }
//        if (jstrip == 0) printf("L%d s%2d: triad=%2x... no match\n", 
//         ilayer+1, istrip+1, upevt_.clct_dump[ilayer][distrip]);
	  }
//      else printf("L%d s%2d: no triad received\n", ilayer+1, istrip+1);
	}
      }

/* When each DAC amplitude has been completed, analyze fraction of events */
    if (upevt_.event_number % upevt_.stripcal_trigger_per_step == 0)
      {
      printf("Event %ld: filling histograms for test pulse DAC=%d\n",
	     upevt_.event_number, upevt_.stripcal_current_value);
	
      for (ilayer = 0; ilayer < NLAYER; ilayer++)
	{
	for (ifeb = FIRST_IFEB; ifeb < NCFEB; ifeb++)
	  {
	  istrip = 16 * ifeb + (upevt_.stripcal_current_strip - 1);
	  if (nall[ilayer][ifeb] > 0)
	    {
	    x = (float)nright[ilayer][ifeb] / nall[ilayer][ifeb];
	    get_offset_(&nright[ilayer][ifeb], &nall[ilayer][ifeb], &offset,
			&offset_err[ilayer][ifeb][this_point]);
	    }
	  else
	    {
	    x = -999.;
	    offset = -20.;
	    offset_err[ilayer][ifeb][this_point] = 10.;
	    }

	  hid = 100 * (ilayer + 1) + (istrip + 1);
	  HFILL(hid, (float)upevt_.stripcal_current_value, 0., offset);
	  printf("L%d, s%2d: nright=%4d, nall=%4d, x=%9.3f, offset=%8.3f"
		 " error=%6.3f %3d\n", ilayer+1, istrip+1, 
		 nright[ilayer][ifeb], 
		 nall[ilayer][ifeb], x, offset, 
		 offset_err[ilayer][ifeb][this_point],
		 this_point);

	  if (upevt_.event_number % triggers_per_strip == 0) 
	    HPAKE(hid, offset_err[ilayer][ifeb]);
	  }
	}
      this_point++;
      }
    }
  return status;
  }


/*******************************************************************************
   End-of-run analysis for FAST site test 22
*******************************************************************************/

int test_22_end(int pass)
  {
  return 0;
  }

/*******************************************************************************
   End-of-test analysis for FAST site test 22
*******************************************************************************/

#define OFFSET_LOWER_LIMIT              -3.0
#define OFFSET_UPPER_LIMIT               3.0
#define CHISQ_UPPER_LIMIT               50.0

int test_22_finish(void)
  {
  int     ilayer, istrip, nbadch;
  float   limits[6];
//float   slope[NLAYER][NSTRIP];
  float   intercept[NLAYER][NSTRIP];
  float   intercept_err[NLAYER][NSTRIP];
  float   chisq[NLAYER][NSTRIP];
  time_t  t;
  char    timestring[40];
  char *file_his_name;

/* Initialize */
  nbadch = 0;

/* Do linear fits */
  offset_fit_(intercept, intercept_err, chisq);

/* Debug */ 
/*
  for (ilayer = 0; ilayer < NLAYER; ilayer++){
  for (istrip = 33; istrip < NSTRIP; istrip++){
  printf("L%d s%d intercept %6.3f error %6.3f chisq %6.3f\n",
  ilayer + 1, istrip + 1, intercept[ilayer][istrip],
  intercept_err[ilayer][istrip], chisq[ilayer][istrip]);
  }
  }
*/

/* Make plot */
  limits[0] = -10.;              
  limits[1] =  10.;             
  limits[2] = -10.;            
  limits[3] =  10.;           
  limits[4] = OFFSET_LOWER_LIMIT;
  limits[5] = OFFSET_UPPER_LIMIT;

  one_page_plot_v3(22, 1, STRIP_PLOT, (float *)intercept, 
		   (float *)intercept_err, limits, csc_name, "L/R Comparator  Offsets", 
		   "offsets, noise sigma units", 1);

/* Check that calibration parameters are within limits */
  fprintf(fp, "List of strips with calibration parameters outside limits:\n");
  fprintf(fp, "layer strip offset\n");
  fprintf(fp, "--------------------------\n");
  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
//  for (istrip = 0; istrip < NSTRIP; istrip++) {
    for (istrip = 33; istrip < NSTRIP; istrip++)
      {
      if ((intercept[ilayer][istrip] < OFFSET_LOWER_LIMIT) || 
	  (intercept[ilayer][istrip] > OFFSET_UPPER_LIMIT) ||
	  (chisq[ilayer][istrip] > CHISQ_UPPER_LIMIT))
	{
	nbadch++;
	fprintf(fp, "  %d    %2d   %8.2f  %8.2f\n", ilayer + 1, istrip + 1, 
		intercept[ilayer][istrip], chisq[ilayer][istrip]);
	}
      }
    }
  fprintf(fp, "--------------------------\n");

  fprintf(fp, "Total number of bad channels = %d\n", nbadch);

  /* Save histograms */
  file_his_name = malloc(strlen(datadir_translation) + 
			 strlen(test_22_file_his_el) + 1);
  if (file_his_name == NULL)
    {
    printf("malloc() failed for file_his_name.  Could not save histograms.\n");
    }
  else
    {
    sprintf(file_his_name,"%s%s",
	    datadir_translation,test_22_file_his_el);
    printf("Saving histograms\n");
    HRPUT(0, file_his_name, "N");
    free(file_his_name);    
    }

/* Print mandatory summary statement */
  if (nbadch > 0) fprintf(fp, "Test result = Failed\n\n");
  else fprintf(fp, "Test result = OK\n\n");

/* Add the date and time */
  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp, "%s\n\n", timestring);

  fclose(fp);

  HDELET(0);

  return 0;
  }











