
/*
 * test_24.c (in V1.2 test_19.c)
 * Chamber Gain Map
 *
 * Author : Lisa Gorn      
 * Date   : 2 Jan 2001
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include "cfortran.h"
#include "hbook.h"
#include "alct_params.h"       /* definition of alct_params_type structure */
//#include "alct_declarations.h" /* declaration of read_afeb_config() */
#include "daq_conf.h"
#include "csc_event.h"       /* upevt_                 */
#include "csc_parameters.h"  /* NLAYER, NSTRIP, etc... */
#include "misc.h"            /* definition of logical variables */
#include "one_page_plot.h"   /* STRIP_PLOT,...         */
#include "test_params.h"     /* test config parameters */
#include "test_utils.h"      /* chamber labels         */

//#include "csc_unpacker.h"
//#include "emu/daq/reader/RawDataFile.h"

#define   LANDAU_THRESHOLD     50 
#define   MAX_FIFO_SAMPLES     16
#define   NLCT                  4
#define   NCFEB                 5

/* Local function declarations */
int test_24_begin(int pass);
int test_24_event(int pass);
int test_24_end(int pass);
int test_24_finish(void);

/* Function declarations */
int alctsim_24(short int *accel, short int *key, short int *patt, 
	       short int *valid);
int landau_fit_(int *hid, float *par, float *sigpar, float *chisq);

/* Global variables local to this test */
static int        n_out_of_time[NLAYER];
static float      chisq[NLAYER][25];
static float      dv[NLAYER][5];
static float      dv_all[NLAYER];
//static float      peak[NLAYER][25];
static float *peak[NLAYER];
static float *ppeak;
static logical    first;
static FILE       *fp;
static int nwires, nsegments;
static int* ks = NULL;

int keyseg[7][5] = 
{
    {48,  0,  0,  0, 0},  // 1.1
    {24, 48, 64,  0, 0},  // 1.2
    {12, 22, 32,  0, 0},  // 1.3
    {44, 80, 112, 0, 0},  // 2.1
    {32, 64, 96,  0, 0},  // 3.1
    {16, 28, 40,  52, 64}, // 234.2
    {32, 64, 96,  0, 0}   // 4.1
};

char test_24_file_el[]="/test_results/temp/test_24_01.results";
char test_24_file_his_el[]="/test_results/temp/test_24_01.his";

/*******************************************************************************
   Define functions and parameters for FAST site test 24
*******************************************************************************/
static int nstrips, ncfebs;

int test_24_init(void)
{
    int l;

    test_begin  = test_24_begin;
    test_event  = test_24_event;
    test_end    = test_24_end;
    test_finish = test_24_finish;

    num_passes = 1;
    num_events[0] = 1000000;

    switch (csc_type)
    {
    case 2:  nstrips = 64; ncfebs = 4; break;
    default: nstrips = 80; ncfebs = 5; break;
    }

    switch (csc_type)
    {
    case 0: nwires = 48;  nsegments = 1; break;
    case 1: nwires = 64;  nsegments = 3; break;
    case 2: nwires = 32;  nsegments = 3; break;
    case 3: nwires = 112; nsegments = 3; break;
    case 4: nwires = 96;  nsegments = 3; break;
    case 5: nwires = 64;  nsegments = 5; break;
    case 6: nwires = 96;  nsegments = 3; break;
    default: nwires = 64; nsegments = 5; break;
    }
    ks = keyseg[csc_type];

    printf ("csc_type = %d, nwires = %d\n", csc_type, nwires);

    ppeak = malloc (sizeof(float) * nsegments * ncfebs * NLAYER);
  
    for (l = 0; l < NLAYER; l++)
    {
	peak[l] = ppeak + l * nsegments * ncfebs;
    }

    return 0;
}

/*******************************************************************************
   Book histograms for FAST site test 24                                       
*******************************************************************************/

int test_24_begin(int pass)
  {
  int     hid, icfeb, ilayer, isegment;
  char    htitle[80];
  char *filename;

  for (ilayer = 0; ilayer < NLAYER; ilayer++) n_out_of_time[ilayer] = 0;
  first = _TRUE;

  if (pass == 0)
    {
    filename = malloc(strlen(datadir_translation) + 
		      strlen(test_24_file_el) + 1 );
    if (filename == NULL)
      {
      printf("malloc() failed on filename.\n");
      return 1;
      }
    sprintf(filename,"%s%s",datadir_translation,test_24_file_el);

/* Open result file in the temporary directory */
    fp = fopen(filename, "w");
    if(fp == NULL)
      {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
      }
    free(filename);

    fprintf(fp, "-------------------------------------------------------\n"); 
    fprintf(fp, "Results of test 24 (Chamber Gain Map) for %s\n", csc_name); 
    fprintf(fp, "-------------------------------------------------------\n\n"); 

/* Book histograms */
    printf("Booking pass 0 histograms for test 24\n");

    hid = 20;
    sprintf(htitle, "Peak time bin of strip with max 5-bin sum");
    HBOOK1(hid, htitle, 16, -0.5, 15.5, 0.0);

    hid = 21;
    sprintf(htitle, "Peak time bin of strip with max 5-bin sum > 100");
    HBOOK1(hid, htitle, 16, -0.5, 15.5, 0.0);

    hid = 22;
    sprintf(htitle, "Slope in stripwidths/layer_spacing");
    HBOOK1(hid, htitle, 100, -5., 5., 0.0);

    hid = 23;
    sprintf(htitle, "Track x in stripwidths");
    HBOOK1(hid, htitle, 100, -10., 90., 0.0);

    hid = 24;
    sprintf(htitle, "Peak of fitted Landau distr, ADC counts (5x5 sum)");
    HBOOK1(hid, htitle, 100, 0., 2000., 0.0);

    hid = 25;
    sprintf(htitle, "Chisq of fitted Landau distr");
    HBOOK1(hid, htitle, 100, 0., 10., 0.0);

    for (ilayer = 1; ilayer <= NLAYER; ilayer++)
      {
      hid = ilayer;
      sprintf(htitle, "L%d max 5-bin adc sum", ilayer);
      HBOOK1(hid, htitle, 100, 0., 4000, 0.0);

      hid = 10 + ilayer;
      sprintf(htitle, "L%d strip with max 5-bin sum", ilayer);
      HBOOK1(hid, htitle, 80, 0.5, 80.5, 0.0);

      for (isegment = 1; isegment <= nsegments; isegment++)
	{
        for (icfeb = 1; icfeb <= ncfebs; icfeb++)
	  {
          hid = 100 * ilayer + 10 * isegment + icfeb;
          sprintf(htitle, "L%d seg %d cfeb %d, Landau plot, 5x5 adc sum", 
		  ilayer, isegment, icfeb);
          HBOOK1(hid, htitle, 100, 0., 4000., 0.0);
	  }
	}
      }
    }
  else if (pass == 1)
    {
    }
  return 0;
  }

/*******************************************************************************
   Do analysis and fill histograms for FAST site test 24
*******************************************************************************/

int test_24_event(int pass)
  {
  int        j, hid, ilayer, isegment, istrip, icfeb, jm;
  int        peak_strip[NLAYER], status;
  int        jmax[nstrips], lfirst, llast, pssum, sca_max, strip_max;
  short int  accel[2], key[2], patt[2], valid[2];
  
  float      max_strip_sum, slope, x;
  float      cluster_sum[NLAYER], strip_sum[NLAYER][nstrips];
  logical    got_an_feb = _FALSE;


/* Check to see if we have an empty event */
  for (j=0; j < ncfebs; j++) 
    if (upevt_.active_febs[j] > 0) got_an_feb = _TRUE;
  if (! got_an_feb)
    {
    printf("No active febs in this event\n");
    return 0;
    }

/* Initializations to avoid compiler warnings */
  llast = 0;
  sca_max = 0;
  strip_max = 0;
  max_strip_sum = 0;

/* Do wire track-finding to find the radial position of the track. Use the
 * ALCT algorithm for track-finding. Continue with cluster analysis if 
 * exactly one track is found.
 */
  if ((status = alctsim_24(accel, key, patt, valid)))
    {
    printf("call to alctsim_24 failed; skipping event %ld\n", 
	   upevt_.event_number);
    return -1;
    }

/* Debug *//*
   if (valid[0]) printf("Event %ld LCT 1 has key wg=%d, pattern=%d, 
   accel_mu=%d\n", upevt_.event_number, key[0]+1, patt[0], accel[0]);
   if (valid[1]) printf("Event %ld LCT 2 has key wg=%d, pattern=%d, 
   accel_mu=%d\n", upevt_.event_number, key[1]+1, patt[1], accel[1]);
	   *//* End debug */

  if (!valid[0])
    {
  //  printf("skipping event %ld with no alct tracks\n", upevt_.event_number);
    return 0;
    }

  if (valid[1])
    {
      if (abs(key[1]- key[0]) > 5)
	{
	  printf("skipping event %ld with > 1 alct track\n", upevt_.event_number);
	  return 0;
	}
    }

/* Convert key wire group to segment */
/*
  if (key[0] < 16) isegment = 0;
  else if (key[0] < 28) isegment = 1;
  else if (key[0] < 40) isegment = 2;
  else if (key[0] < 52) isegment = 3;
  else isegment = 4;
*/

  for (isegment = 0; isegment < nsegments; isegment++)
  {
      if (key[0] < ks[isegment]) break;
  }

/* Find the biggest cluster in each layer. Biggest is defined as the 5-strip
 * cluster which is centered on the strip which has the largest adc sum, and
 * where the adc sum is a 5-sample sum centered on the peak time-sample.
 * Obviously, there could be other definitions of biggest.
 */
  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
    peak_strip[ilayer] = -1;

    for (istrip = 0; istrip < nstrips; istrip++)
      {

/* Calculate pedestal from last three samples */
      pssum = 0;
//      for (j = 0; j < 3; j++) pssum += upevt_.sca[ilayer][istrip][j];
      for (j = upevt_.nsca_sample-3; j < upevt_.nsca_sample; j++) pssum += upevt_.sca[ilayer][istrip][j];
/* Find timebin with maximum */
      for (j = 0; j < upevt_.nsca_sample; j++)
	{
        if (j == 0 || upevt_.sca[ilayer][istrip][j] > sca_max)
	  {
          sca_max = upevt_.sca[ilayer][istrip][j];
          jmax[istrip] = j;
	  }
	}
/* Calculate cluster sum around the maximum */
      strip_sum[ilayer][istrip] = -5/3. * pssum;
      jm = (jmax[istrip] < 2) ? 2 : jmax[istrip];
      jm = (jm >= upevt_.nsca_sample - 2) ? upevt_.nsca_sample - 3 : jm;
      for (j = jm - 2; j <= jm + 2; j++)
	{
        strip_sum[ilayer][istrip] += upevt_.sca[ilayer][istrip][j]; 
	}
//      printf("%d %d %d %d %d %.3f\n", ilayer, istrip, pssum, sca_max,jmax[istrip], strip_sum[ilayer][istrip]);
/* Keep track of the strip with the maximum strip_sum */
      if (istrip == 0 || (strip_sum[ilayer][istrip] > max_strip_sum))
	{
        max_strip_sum = strip_sum[ilayer][istrip];
        strip_max = istrip;
	}
      }

/* Debug *//*
    if (strip_max < 32)
      {
      printf("Event %ld L%d max sum is %.3f in timebin %d on strip %d\n", 
	     upevt_.event_number, ilayer, strip_sum[ilayer][strip_max], 
	     jmax[strip_max], strip_max);
      }
*//* End of Debug */

    hid = ilayer + 1;
    HFILL(hid, strip_sum[ilayer][strip_max], 0., 1.);
    HFILL(20, (float)jmax[strip_max], 0., 1.);
    if (strip_sum[ilayer][strip_max] > 100) HFILL(21, 
						  (float)jmax[strip_max], 0., 1.);

    // Was 5 
    if (jmax[strip_max] < 3) n_out_of_time[ilayer]++;
    else
      {
      peak_strip[ilayer] = strip_max;

/* Plot strip_max occupancy (as cross-check) */
      if (strip_sum[ilayer][strip_max] > 100)
	{
        hid = 10 + ilayer + 1;
        HFILL(hid, (float)(strip_max+1), 0.0, 1.0);
	}
      }

/* Calculate cluster sum */
    if (strip_max < 2) strip_max = 2;
    if (strip_max > nstrips - 3) strip_max = nstrips - 3;
    cluster_sum[ilayer] = 0;
    for (istrip = strip_max - 2; istrip <= strip_max + 2; istrip++) 
      cluster_sum[ilayer] += strip_sum[ilayer][istrip];
    }

/* Do simple cuts to select more-or-less vertical tracks */
  lfirst = -1;
  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
//    printf("+%d %.3f %d\n", ilayer, cluster_sum[ilayer], peak_strip[ilayer]);
    if (peak_strip[ilayer] > -1 && cluster_sum[ilayer] > 200)
      {
      if (lfirst == -1) lfirst = ilayer;
      llast = ilayer;
      }
    }
  if (lfirst == -1)
    {
  // printf("Skipping event %ld with no clusters above threshold\n", 
  // upevt_.event_number);
    return 0;
    }

  if (llast == lfirst)
    {
  // printf("Skipping event %ld with only one cluster above threshold\n",
  //  upevt_.event_number);
    return 0;
    }

  slope = (float)(peak_strip[llast] - peak_strip[lfirst]) / (llast - lfirst);
  HFILL(22, slope, 0.0, 1.0);
  x = slope * (2.5 - lfirst) + peak_strip[lfirst];
  HFILL(23, x, 0.0, 1.0);

/* Fill Landau plots */
  if (abs(slope) < 2.0)
    {
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      if (cluster_sum[ilayer] > LANDAU_THRESHOLD)
	{
        j = peak_strip[ilayer] % 16;
        if (j > 0 && j < 15)
	  {
          icfeb = peak_strip[ilayer] / 16;
          hid = 100 * (ilayer + 1) + 10 * (isegment + 1) + (icfeb + 1);
          HFILL(hid, cluster_sum[ilayer], 0.0, 1.0);
	  }
	}
      }
    }
//  else printf("Skipping event %ld with slope=%f\n", upevt_.event_number, slope);
  return 0;
  }


/*******************************************************************************
   End-of-run analysis for FAST site test 24
*******************************************************************************/

#define   PNOM       600
#define   C         .00561

int test_24_end(int pass)
  {
  int      j, hid, icfeb, ilayer, isegment, n, noent;
  float    par[3], sigpar[3];
  float    avg_all, avg[NLAYER][5];

/* Fit the cluster sum distribution in each section with the Landau function
 * (two-parameter), and save the average Landau peak in each HV segment.
 */
  fprintf(fp, "\nLandau fit parameters, errors, and chisq\n");
  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
    for (isegment = 0; isegment < nsegments; isegment++)
      {
      avg[ilayer][isegment] = 0;
      n = 0;
      for (icfeb = 0; icfeb < ncfebs; icfeb++)
	{
        j = ncfebs * isegment + icfeb;
        hid = 100 * (ilayer + 1) + 10 * (isegment + 1) + (icfeb + 1);
        HNOENT(hid, noent);
        if (noent > 0)
	  {
          landau_fit_(&hid, par, sigpar, &chisq[ilayer][j]);
          peak[ilayer][j] = par[1];

          fprintf(fp, "  L%d seg%d cfeb%d  %6.0f %6.0f %6.0f  %4.0f %4.0f "
		  "%4.0f  %6.2f\n", ilayer+1, isegment+1, icfeb+1, par[0], par[1],
		  par[2], sigpar[0], sigpar[1], sigpar[2], chisq[ilayer][j]);

          avg[ilayer][isegment] += peak[ilayer][j];
          n += 1;

          HFILL(24, peak[ilayer][j], 0.0, 1.0);
          HFILL(25, chisq[ilayer][j], 0.0, 1.0);
	  }
        else
	  {
          peak[ilayer][j] = 0;
          chisq[ilayer][j] = 0;
	  }
	}

      if (n > 0) avg[ilayer][isegment] /= n;
      else avg[ilayer][isegment] = 0.;
      }
    }

/* Equalization logic */
/* Assume Landau peak p = exp(c * (volts-2683)) as a typical dependence, where
 * c = ln(2)/130 (gain increases by a factor of two every 130 volts) = .00533. 
 * To first approx, dv = dp / (c * p). Find dv to make all segments peak at 
 * PNOM adc counts.
 * From HV scan of ME234/2-001, we find p = exp(5.61 * (volts - 2470)), and
 * c = ln(2)/124 = .00561.
 */
  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
    for (isegment = 0; isegment < nsegments; isegment++)
      {
      if (avg[ilayer][isegment] > 0) dv[ilayer][isegment] = 
				       (PNOM - avg[ilayer][isegment]) / (C * avg[ilayer][isegment]); 
      else dv[ilayer][isegment] = 9990.;
      }
    }

  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
    avg_all = 0;
    n = 0;
    for (isegment = 0; isegment < nsegments; isegment++)
      {
      if (avg[ilayer][isegment] > 1.)
	{
        avg_all += avg[ilayer][isegment];
        n++;
	}
      }
    if (n > 0)
      {
      avg_all /= n;
      dv_all[ilayer] = (PNOM - avg_all) / (C * avg_all);
      }
    else dv_all[ilayer] = 9990.;
    }

  return 0;
  }

/*******************************************************************************
   End-of-test analysis for FAST site test 24
*******************************************************************************/

#define LOWER_LIMIT   250
#define UPPER_LIMIT   800

int test_24_finish(void)
  {
  int      j, ilayer, isegment, icfeb, n, nfail;
  logical  channel_ok;
  float    limits[7];
  char     timestring[40];
  time_t   t;
  char *file_his_name;

  /* Save histograms */
  file_his_name = malloc(strlen(datadir_translation) + 
			 strlen(test_24_file_his_el) + 1);
  if (file_his_name == NULL)
    {
    printf("malloc() failed for file_his_name.  Could not save histograms.\n");
    }
  else
    {
    sprintf(file_his_name,"%s%s",
	    datadir_translation, test_24_file_his_el);
    printf("Saving histograms\n");
    HRPUT(0, file_his_name, "N");
    free(file_his_name);    
    }

/* Look for values outside limits */
  fprintf(fp, "\nList of sections with peak outside limits %d-%d, or with "
	  "chisq/ndf > 10:\n", LOWER_LIMIT, UPPER_LIMIT);
  nfail = 0;

  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
    for (isegment = 0; isegment < nsegments; isegment++)
      {
      for (icfeb = 0; icfeb < ncfebs; icfeb++)
	{
        channel_ok = _TRUE;
        j = ncfebs * isegment + icfeb;

        if (peak[ilayer][j] < LOWER_LIMIT || peak[ilayer][j] > UPPER_LIMIT)
	  {
          channel_ok = _FALSE;
          fprintf(fp, "Layer %d segment %d cfeb %d has peak= %4.0f\n", 
		  ilayer+1, isegment+1, icfeb+1, peak[ilayer][j]);
	  }

        if (chisq[ilayer][j] > 10)
	  {
          channel_ok = _FALSE;
          fprintf(fp, "Layer %d segment %d cfeb %d has chisq=%8.2f\n", 
		  ilayer+1, isegment+1, icfeb+1, chisq[ilayer][j]);
	  }

        if (!channel_ok) nfail++;
	}
      }
    }
  fprintf(fp, "\n");
  fprintf(fp, "Total bad channels = %d\n", nfail);

/* Print voltage Equalization table */
  fprintf(fp, "\nEqualization table rounded to nearest 10 volts:\n");
  if (nsegments == 5) fprintf(fp, "HV segment:     1     2     3     4     5    all\n");
  else                fprintf(fp, "HV segment:     1     2     3    all\n");
  fprintf(fp, "------------------------------------------------\n");
  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
    fprintf(fp, "Layer %d:   ", ilayer+1);
    for (isegment = 0; isegment < nsegments; isegment++)
      {
      if (dv[ilayer][isegment]  >= 0) 
	n = 10 * (int)((dv[ilayer][isegment] / 10) + 0.5);
      else n = 10 * (int)((dv[ilayer][isegment] / 10) - 0.5);
      fprintf(fp, "  %4d", n);
      }
    if (dv_all[ilayer] >= 0) 
      n = 10 * (int)((dv_all[ilayer] / 10) + 0.5);
    else n = 10 * (int)((dv_all[ilayer] / 10) - 0.5);
    fprintf(fp, "  %4d\n", n);
    }
  fprintf(fp, "\n");


/* Plot results */
  limits[0] = 0;                   /* layer histo ymin */
  limits[1] = 1400;                /* layer histo ymax */
  limits[2] = 0;                   /* summary histo ymin */
  limits[3] = 1400;                /* summary histo ymax */
  limits[4] = LOWER_LIMIT;         /* lower limit line */
  limits[5] = UPPER_LIMIT;         /* upper limit line */
  limits[6] = nsegments * ncfebs;
  
  one_page_plot_v3(24, 1, GAIN_PLOT, (float *)ppeak, (float *)ppeak, limits, 
		   csc_name, "Landau peaks", "Landau peak, ADC "
		   "counts (5x5 sum)", 0);

/* Print mandatory summary statement */
  if (nfail > 0) fprintf(fp, "Test result = Failed\n\n");
  else fprintf(fp, "Test result = OK\n\n");

/* Add the date and time */
  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp, "%s\n\n", timestring);

  fclose(fp);

  HDELET(0);

  return 0;
  }

/*******************************************************************************
   Modified ALCT simulation subroutine used for offline wire track-finding
*******************************************************************************/

/*
 * Subroutine which takes ALCT raw hit data as input, and finds the expected
 * ALCT results.
 */

int alctsim_24(short int *accel, short int *key, short int *patt, 
	       short int *valid)
  {
  int              i, j, k, ilct, layer, mode, nhits_a, nhits_c;
  short int              best_wg_a[2][NLCT], max_nhits_a[2][NLCT];
  short int              best_wg_c[2][NLCT], max_nhits_c[2][NLCT];
  int              accel_mu[NLCT], nhits[NLCT], best_wg[NLCT];
  short int        active_lct, jm[3], jp[3];
  unsigned long    alct_s[NLAYER][nwires], data, *ldat, tbit;
  static unsigned long    mask[MAX_FIFO_SAMPLES];
  logical          debug_on, local_file;
  logical          pulse_on, pretrig_a, pretrig_c, pretrigger;
  logical          simulation_off = _TRUE;
  static alct_params_type    p;

/* Initialize */
  valid[0] = 0;
  valid[1] = 0;

  if (simulation_off)
    {
      valid[0] = upevt_.alct_valid_patt[0];
      valid[1] = upevt_.alct_valid_patt[1];
      key[0]   = upevt_.alct_wire_group[0];
      key[1]   = upevt_.alct_wire_group[1];
      return 0;
    }

  debug_on = _FALSE;
  if (first)
    {
    first = _FALSE;
    local_file = _TRUE;
    // simulation is off anyway - Madorsky
//    read_afeb_config(upevt_.afeb_config_id, local_file, -1, &p);
    for (k = 0; k < MAX_FIFO_SAMPLES; k++) mask[k] = 1 << k;

    /* Override some config-file values */
    p.num_fifo_before_pretrig = 4;
    p.drift_delay = 2;
    p.alct_jtag_disable = 1;
    p.alct_accel_mu_mode = 1;
    p.nplanes_for_pretrig = 2;
    p.trigger_mode = 0;
    p.pattern_num_thr = 4;
    }

/* Convert (as well as possible) from raw data to 150-ns-pulse-stretched data */

  for (layer = 0; layer < NLAYER; layer++)
    {
    for (j = 0; j < nwires; j++)
      {
      alct_s[layer][j] = 0;

      /* Logic for one-shot that's retriggerable after input has gone to zero */
      if ((data = upevt_.alct_dump[layer][j]))
	{
        pulse_on = _FALSE;
        for (k = 0; k <  p.num_fifo_samples; k++)
	  {
          if ((mask[k] & data) && !pulse_on)
	    {
            for (i = k; i < k + 6; i++) alct_s[layer][j] |= mask[i];
            pulse_on = _TRUE;
	    }
          else if (!(mask[k] & data)) pulse_on = _FALSE;
	  }
	}
      }
    }

/* Do trigger logic twice, at pretrigger time and after the drift delay */
  for (i = 0; i < 2; i++)
    {
    if (i == 0) k = p.num_fifo_before_pretrig;
    else k = p.num_fifo_before_pretrig + p.drift_delay;

    if (k < 0 || k >= p.num_fifo_samples)
      {
      printf("Can't simulate trigger with these run parameters:\n");
      printf("Num fifo samples=%d, before pretrigger=%d, drift_delay=%d\n",
	     p.num_fifo_samples, p.num_fifo_before_pretrig, p.drift_delay);
      return(-1);
      }

    tbit = 1 << k;

    for (ilct = 0; ilct < 4; ilct++)
      {
      max_nhits_a[i][ilct] = 0;
      max_nhits_c[i][ilct] = 0;
      best_wg_a[i][ilct] = 0;
      best_wg_c[i][ilct] = 0;
      }

    for (j = 0; j < nwires; j++)
      {
      nhits_a = 0;
      nhits_c = 0;
      ilct = j / 16;
      for (k = 0; k < 3; k++)
	{
        jm[k] = j - k;
        jp[k] = j + k;
        if (jm[k] < 0) jm[k] = 0;
        else if (jp[k] > nwires-1) jp[k] = nwires - 1;
	}

/* Collision and accelerator muon algorithms */
      for (layer = 0; layer < NLAYER; layer++)
	{
        ldat = alct_s[layer];
        if ((ldat[jm[0]]) & tbit) nhits_a++; 

        if (layer == 0)
	  {
	  if ((ldat[jp[0]] | ldat[jp[1]] | ldat[jp[2]]) & tbit) nhits_c++; 
	  }
        else if (layer == 1)
	  {
          if ((ldat[jp[0]] | ldat[jp[1]]) & tbit) nhits_c++; 
	  }
        else if (layer == 2)
	  {
          if ((ldat[jp[0]]) & tbit) nhits_c++; 
	  }
        else if (layer == 3)
	  {
          if ((ldat[jm[0]] | ldat[jm[1]]) & tbit) nhits_c++; 
	  }
        else
	  {
          if ((ldat[jm[0]] | ldat[jm[1]] | ldat[jm[2]]) & tbit) nhits_c++; 
	  }
	}

/* Compare with previous best track in this LCT section (16 wires) */
      if (nhits_a >= max_nhits_a[i][ilct])
	{
        best_wg_a[i][ilct] = j;
        max_nhits_a[i][ilct] = nhits_a;
	}

      if (nhits_c >= max_nhits_c[i][ilct])
	{
        best_wg_c[i][ilct] = j;
        max_nhits_c[i][ilct] = nhits_c;
	}
      }
    }

/* Assign promotion bits */
  for (i = 0; i < 2; i++)
    {
    for (ilct = 0; ilct < 4; ilct++)
      {
      if (p.alct_jtag_disable == 1) mode = p.alct_accel_mu_mode;
      else mode = p.lct[ilct].accel_mu_mode;

      if (mode == 0) max_nhits_a[i][ilct] = 0;
      else if (mode == 1 && max_nhits_c[i][ilct] > 0) max_nhits_c[i][ilct] += 8;
      else if (mode == 2 && max_nhits_a[i][ilct] > 0) max_nhits_a[i][ilct] += 8;
      else if (mode == 3) max_nhits_c[i][ilct] = 0;
      }
    }

/* Apply the pretrigger threshold, and set active_lct bits (???)  */
  pretrig_a = _FALSE;
  pretrig_c = _FALSE;
  active_lct = 0;
  for (ilct = 0; ilct < 4; ilct++)
    {
    if ((max_nhits_a[0][ilct] > max_nhits_c[0][ilct]) && 
	((max_nhits_a[0][ilct] & 0x7) >= p.nplanes_for_pretrig))
      {
      pretrig_a = _TRUE;
      active_lct |= (1 << ilct);
      }
    if ((max_nhits_c[0][ilct] > max_nhits_a[0][ilct]) && 
	((max_nhits_c[0][ilct] & 0x7) >= p.nplanes_for_pretrig))
      {
      pretrig_c = _TRUE;
      active_lct |= (1 << ilct);
      }
    }
  if (p.trigger_mode == 0) pretrigger = pretrig_a || pretrig_c;
  else if (p.trigger_mode == 1) pretrigger = pretrig_a;
  else if (p.trigger_mode == 2) pretrigger = pretrig_c;
  else if (p.trigger_mode == 3) pretrigger = pretrig_c && !pretrig_a;

/* For each LCT, choose between accelerator and collision muons */
  for (ilct = 0; ilct < 4; ilct++)
    {
    if (max_nhits_a[1][ilct] > max_nhits_c[1][ilct])
      {
      nhits[ilct] = max_nhits_a[1][ilct];
      best_wg[ilct] = best_wg_a[1][ilct];
      accel_mu[ilct] = _TRUE;
      }
    else if (max_nhits_c[1][ilct] > max_nhits_a[1][ilct])
      {
      nhits[ilct] = max_nhits_c[1][ilct];
      best_wg[ilct] = best_wg_c[1][ilct];
      accel_mu[ilct] = _FALSE;
      }
    else nhits[ilct] = 0;
    }

/* Simulate "resolver" which removes duplicate LCT's at feb boundaries */
  for (ilct = 0; ilct < 3; ilct++)
    {
    if ((nhits[ilct] > 0) && (nhits[ilct+1] > 0) && 
	((best_wg[ilct+1] - best_wg[ilct]) < 4))
      {
      if (nhits[ilct+1] >= nhits[ilct]) nhits[ilct] = 0;
      else nhits[ilct+1] = 0;
      }
    } 

/* Find the two best LCT's */
  for (i = 0; i < 2; i++)
    {
    patt[i] = 0;
    key[i] = 0;
    accel[i] = 0;

    for (ilct = 0; ilct < 4; ilct++)
      {
      if (nhits[ilct] >= patt[i] &&
	  (i == 0 || best_wg[ilct] != key[0]))
	{
        if ((nhits[ilct] & 0x7) >= p.pattern_num_thr)
	  {
          patt[i] = nhits[ilct];
          valid[i] = 1;
          key[i] = best_wg[ilct];
          accel[i] = accel_mu[ilct];
	  }
	}
      }
    }
  
  return(0);
  }
