/*
 * test_25.c (in V1.2 test_20.c)
 * ALCT Self-trigger Test
 *
 * Author : Lisa Gorn      
 * Date   : 26 Dec 2000
 * Last modified: 26 Jun 2002, add case with radioactive source S.D.
 */

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "cfortran.h"
#include "hbook.h"
#include "event_ana.h"       /* Function prototypes    */
#include "daq_conf.h"
#include "csc_event.h"       /* upevt_                 */
#include "csc_geom.h"        /* chamber geometry (of ME234/2 chambers) */
#include "csc_parameters.h"  /* NLAYER, NSTRIP, etc... */
#include "one_page_plot.h"   /* STRIP_PLOT,...         */
#include "test_utils.h"      /* chamber labels         */
#include "test_params.h"     /* test config parameters */
#include "misc.h"            /* definition of logical variables */
#include "alct_params.h"         /* definition of alct_params_type structure */
//#include "alct_declarations.h"   /* declaration of read_afeb_config() */

#include "setup_parameters.h"
#include "alct2_params.h"
//#include "alct2_declarations.h"
#include "test_25_ini.h"

//#include "csc_unpacker.h"
//#include "emu/daq/reader/RawDataFile.h"


#define   MAX_FIFO_SAMPLES   16
#define   NLCT                4

/* Local function declarations */
int test_25_begin(int pass);
int test_25_event(int pass);
int test_25_end(int pass);
int test_25_finish(void);
int alct2000(void);

/* Global variables local to this test */
static int         first, first_alct2000_call;
static int         n_active_lct_errors, n_valid_patt_errors;
static int         n_patt_quality_errors, n_accel_muon_errors;
static int         n_wire_group_errors, n_header_errors;
static int         n_events_with_errors;
static int         plane_threshold, pattern_threshold;
static long int    nmatches[6], nevents[6];
static FILE        *fp;
static FILE        *fp_bad;
static float       all_time[6], all_scaler[6];

int src_status;                      // Status of radioactive source for this run, 
static char * src_val[2] ={ "No rad.source","With rad.source"};

char* test_25_file_el[2]={"/test_results/temp/test_25_01.results",
			  "/test_results/temp/test_25_02.results"};
char* test_25_file_bad_el[2]={"/test_results/temp/test_25_01.bad",
			      "/test_results/temp/test_25_02.bad"};
char* test_25_file_his_el[2]={"/test_results/temp/test_25_01.his",
			      "/test_results/temp/test_25_02.his"};

static int nwires;
static int source_type = 1; // source is high-intensity by default

/*******************************************************************************
   Define functions and parameters for FAST site test 25
*******************************************************************************/

int test_25_init(void)
  {
  test_begin  = test_25_begin;
  test_event  = test_25_event;
  test_end    = test_25_end;
  test_finish = test_25_finish;

  num_passes = 1;
  num_events[0] = 32000;
  first = _TRUE;
  switch (csc_type)
  {
  case 0: nwires = 48; break;
  case 1: nwires = 64; break;
  case 2: nwires = 32; break;
  case 3: nwires = 112; break;
  case 4: nwires = 96; break;
  case 5: nwires = 64; break;
  case 6: nwires = 96; break;
  default: nwires = 64; break;
  }

  printf ("csc_type = %d, nwires = %d\n", csc_type, nwires);

  return 0;
  }

/*******************************************************************************
   Book histograms for FAST site test 25, and do other initialization
*******************************************************************************/

int test_25_begin(int pass)
  {
  int     j=0;
  char    htitle[80];
  char *filename;

  if (pass == 0)
    {
    first = _TRUE;
    first_alct2000_call = _TRUE;
    n_active_lct_errors = 0;
    n_valid_patt_errors = 0;
    n_patt_quality_errors = 0;
    n_accel_muon_errors = 0;
    n_wire_group_errors = 0;
    n_header_errors = 0;
    n_events_with_errors = 0;

    printf("****** Test 25 Start ******\n");

    printf("Was rad.source used in this run (0-No rad.source/1-With rad.source) ?[%d] \n",j);
    getint(&j);
    src_status=j; 
    if (src_status <0) src_status = 0;
    if (src_status >1) src_status = 1;
    printf("You entered status %d for run %s\n", src_status, src_val[src_status]);

    if (src_status)
    {
	j = 1;
	printf("Radiation source strength (0-Weak(IHEP) / 1-Normal(all other FAST sites)) ?[%d] \n",j);
	getint(&j);
	source_type = j; 
	if (source_type <0) source_type = 0;
	if (source_type >1) source_type = 1;
	printf("Radiation source strength is %s\n", (source_type)? "Normal" : "Weak");
    }


/* Open result file in the temporary directory */
    filename = malloc(strlen(datadir_translation) + 
		      strlen(test_25_file_el[src_status])+1 );
    if (filename == NULL)
      {
      printf("malloc() failed on filename.\n");
      return 1;
      }
    sprintf(filename,"%s%s",datadir_translation,test_25_file_el[src_status]);

/* Open result file in the temporary directory */
    fp = fopen(filename, "w");
    if(fp == NULL)
      {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
      }
    free(filename);

/* Open  file for bad channels in the temporary directory */
    filename = malloc(strlen(datadir_translation) + 
		      strlen(test_25_file_bad_el[src_status])+1 );
    if (filename == NULL)
      {
      printf("malloc() failed on filename.\n");
      return 1;
      }
    sprintf(filename,"%s%s",datadir_translation,test_25_file_bad_el[src_status]);

/* Open result file in the temporary directory */
    fp_bad = fopen(filename, "w");
    if(fp_bad == NULL)
      {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
      }
    free(filename);

    fprintf(fp, "--------------------------------------------------------\n");
    fprintf(fp, "Results of test 25 (ALCT Self-trigger) for %s\n", csc_name);
    fprintf(fp, 
	    "--------------------------------------------------------\n\n");
    fprintf(fp_bad, "--------------------------------------------------------\n");
     fprintf(fp_bad, "Bad channels of test 25 (ALCT Self-trigger) for %s\n", csc_name);
    fprintf(fp_bad, 
	    "--------------------------------------------------------\n\n");

/* Book hists for some trigger result distributions */
    strcpy(htitle, "full bxn");
    HBOOK1( 1, htitle, 64, 0., 4096., 0.);
    strcpy(htitle, "full bxn");
    HBOOK1( 2, htitle, 64, 0., 64., 0.);

    for (j = 1; j <= 6;  j++)
      {
      sprintf(htitle, "key wiregroups, all valid muons");
      HBOOK1( 10 + j, htitle, nwires, 0., (float)nwires, 0.);

      sprintf(htitle, "key wg, avg ALCT quality");
      HBOOK1( 20 + j, htitle, nwires, 0., (float)nwires, 0.);

      sprintf(htitle, "key wg, accelerator muons");
      HBOOK1( 30 + j, htitle, nwires, 0., (float)nwires, 0.);
      }

    }

  return 0;
  }

/*******************************************************************************
   Do analysis and fill histograms for FAST site test 25
*******************************************************************************/

int test_25_event(int pass)
  {
  int              p;
  int              ifeb,  got_an_feb = 0;
  static int       last_p,  tlast_usec, tnow_usec;
  static time_t                  tlast_sec,  tnow_sec;
  static unsigned long     last_scaler;
  static long      del_scaler;
  float del_time;

  if (pass == 0)
    {
    
/* Initialization */
    if (first)
      {
      for (p = 0; p < 6; p++)
	{
	  last_p = -1;
        nmatches[p] = 0;
	}
      first = _FALSE;
      }

/* Get time and scaler data */
    p = upevt_.alctcal_current_value - 1;
    if (p != last_p)
      {
      last_scaler = upevt_.num_ungated_triggers;
      tlast_sec  = seconds_since_1900();
      tlast_usec = upevt_.time_musec;
      last_p = p;
      all_scaler[p] = 0.;
      all_time[p]   = 0.;
      nevents[p] = 0; 
     }

    del_scaler = upevt_.num_ungated_triggers - last_scaler;

    tnow_sec  = seconds_since_1900();
    tnow_usec = upevt_.time_musec;
    del_time = (tnow_sec-tlast_sec) +1e-6* (tnow_usec-tlast_usec);

    if( del_scaler < 0)  del_scaler = del_scaler + 0xffffff;

    if (del_scaler >0 && del_scaler < MAX_SCALER)
      {
	all_scaler[p] += del_scaler;
	all_time[p]   += del_time;
	(nevents[p])++;
      }
    else
      {
	printf (" del_scaler in %ld event  out of limit, skip it. del_scaler was %ld  \n", 
		upevt_.event_number,
		del_scaler);
      }
   
    last_scaler = upevt_.num_ungated_triggers;
    tlast_sec   = tnow_sec;
    tlast_usec  = tnow_usec;

/* Check to see if we have an empty event */
    for (ifeb=0; ifeb < NCFEB; ifeb++) 
      if (upevt_.alct_febs_read > 0) got_an_feb = 1;
    if (! got_an_feb)
      {
      printf("No AFEB data in event %ld\n", upevt_.event_number);
      return 0;
      }

/* Get the current calibration parameters; for the NPLANES_SCAN mode, we just 
 * have to know the rule for setting the plane threshold, since it isn't 
 * recorded in the calibration data block. The rule is defined in alctcal.c.
 */
    if (upevt_.alctcal_scan_mode != 5)
      {
      printf("Wrong ALCT scan mode for test 25 (found %d, should be 5)\n",
	     upevt_.alctcal_scan_mode);
      }
    pattern_threshold = upevt_.alctcal_current_value;    
    if (pattern_threshold < 2) plane_threshold = 1;
    else plane_threshold = 2;


/* Fill trigger result histograms */
    if (upevt_.alct_valid_patt[0])
      {
      HFILL(1, (float)upevt_.alct_full_bxn, 0., 1.);
      HFILL(2, (float)(upevt_.alct_full_bxn % 64), 0., 1.);
      HFILL(11 + p, (float)upevt_.alct_wire_group[0], 0., 1.);
      HFILL(21 + p, (float)upevt_.alct_wire_group[0], 0., 
	    (float)upevt_.alct_patt_quality[0]);
      if (upevt_.alct_accel_muon[0]) 
	HFILL(31 + p, (float)upevt_.alct_wire_group[0], 0., 1.);
      }
    else printf("Skipping event %ld which had no valid alct\n", 
		upevt_.event_number);
    }

  return 0;
  }


/*******************************************************************************
   End-of-run analysis for FAST site test 25
*******************************************************************************/

int test_25_end(int pass)
  {
  int j;

  if (pass == 0)
    {
    fprintf(fp, "# planes    time      ungated     total     rates\n");
    fprintf(fp, "required    (sec)     triggers    events     Hz  \n");
    fprintf(fp, "---------------------------------------------------\n");
    for (j = 0; j < 6; j++)
      {
      fprintf(fp, "    %d     %8.2e    %8.2e   %6ld    %8.2e\n", 
	      j+1, all_time[j], all_scaler[j], nevents[j], all_scaler[j]/all_time[j]);
      }
    fprintf(fp, "\n");
    }

  return 0;
  }

/*******************************************************************************
   End-of-test analysis for FAST site test 25
*******************************************************************************/

int test_25_finish(void)
  {
  int      j, p, layer, nbadch=0;
  float    accel_lct[6][nwires], all_lct[6][nwires], quality[6][nwires];
  float    data[6][nwires], f, limits[19], wire_length;
  char     timestring[40];
  time_t   t;
  char *file_his_name;
  char plot_title[80];
  float low_limit, high_limit;
  float lim[2][12]={
    {HIST_1PLANE_LOW_LIMIT0, HIST_1PLANE_HIGH_LIMIT0,
     HIST_2PLANE_LOW_LIMIT0, HIST_2PLANE_HIGH_LIMIT0,
     HIST_3PLANE_LOW_LIMIT0, HIST_3PLANE_HIGH_LIMIT0,
     HIST_4PLANE_LOW_LIMIT0, HIST_4PLANE_HIGH_LIMIT0,
     HIST_5PLANE_LOW_LIMIT0, HIST_5PLANE_HIGH_LIMIT0,
     HIST_6PLANE_LOW_LIMIT0, HIST_6PLANE_HIGH_LIMIT0},
    {HIST_1PLANE_LOW_LIMIT1, HIST_1PLANE_HIGH_LIMIT1,
     HIST_2PLANE_LOW_LIMIT1, HIST_2PLANE_HIGH_LIMIT1,
     HIST_3PLANE_LOW_LIMIT1, HIST_3PLANE_HIGH_LIMIT1,
     HIST_4PLANE_LOW_LIMIT1, HIST_4PLANE_HIGH_LIMIT1,
     HIST_5PLANE_LOW_LIMIT1, HIST_5PLANE_HIGH_LIMIT1,
     HIST_6PLANE_LOW_LIMIT1, HIST_6PLANE_HIGH_LIMIT1}};

/* Unpack histograms */
  for (p  = 0; p < 6; p++)
    {
    HUNPAK(11 + p, all_lct[p], "HIST", 0);
    HUNPAK(21 + p, quality[p], "HIST", 0);
    HUNPAK(31 + p, accel_lct[p], "HIST", 0);

    if (nevents[p] > 0)
      {
      if (all_scaler[p] == 0)
	{
        printf("*** No ungated trigger rate data available for nplanes=%d "
	       "***\n", p+1);
        printf("*** Rates will not be deadtime corrected ***\n");
        all_scaler[p] = nevents[p];
	}
      f = (all_scaler[p] / nevents[p]) / all_time[p];
      }
    else f = -1;

/* Plot rates */
    for (j = 0; j < nwires; j++)
      {
      // wire_length = 2 * tan(NSTRIP * STRIP_ANGLE / 2.) * 
      // (R_WG_0 + WG_WIDTH * j) * 1E-3;             /* wire length in meters */
      // data[p][j] = f * all_lct[p][j]/ wire_length;
      data[p][j] = f * all_lct[p][j];
      }
    }
  fprintf(fp, "#key WG     npl=1     npl=2     npl=3      npl=4      npl=5      npl=6\n");
  fprintf(fp, "----------------------------------------------------------------------\n");
  
  for (j = 0; j<nwires; j++)
    {
      fprintf(fp,"    %2d   %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f \n",
	      j+1, data[0][j], data[1][j], data[2][j],
	      data[3][j], data[4][j], data[5][j]);
    }
  fprintf(fp, "----------------------------------------------------------------\n");
  
  limits[2] = lim[src_status][3*2+0];   // HIST_4PLANE_LOW_LIMIT[src_status]
  limits[3] = lim[src_status][3*2+1];   // HIST_4PLANE_HIGH_LIMIT[src_status]
  limits[4] = .0005; // .15;   /* make cut at this fraction of lower limit */
  limits[5] = .75;   /* make cut at this fraction of upper limit */
  limits[6] = nwires;

  low_limit  = limits[2]+ limits[4]*(limits[3]-limits[2])  ;
  high_limit = limits[2]+ limits[5]*(limits[3]-limits[2])  ;
/* Count and print bad_channels */
  for (j = 0; j < nwires; j++)
    {
      if (data[3][j]<low_limit  || data[3][j]> high_limit )
      { 
	nbadch++;
	fprintf(fp_bad,"Key WG %2d rate %9.2f is out of limit %9.2f  %9.2f\n",
		j+1, data[3][j], low_limit, high_limit);
      }
    }


  for (layer = 0; layer < NLAYER; layer++)
    { 
      limits[6+2*layer +1] = lim[src_status][layer*2];
      limits[7+2*layer +1] = lim[src_status][layer*2+1];
    }

  sprintf(plot_title, "LCT rates, %s", src_val[src_status]);
  one_page_plot_v3(25, src_status*2+1, MWIRE_PLOT, (float *)data, (float *)data, limits, 
		   csc_name, plot_title, "4/6 planes rate, Hz/m", 0);

/* Plot average muon quality */
  for (p  = 0; p < 6; p++)
    {
    for (j = 0; j < nwires; j++)
      {
      if (all_lct[p][j] > 0) data[p][j] = quality[p][j] / all_lct[p][j];
      else data[p][j] = -1;
      }
    }

  limits[2] = 0.0;
  limits[3] = 3.5;
  limits[4] =  .0;   /* make cut at this fraction of lower limit */
  limits[5] = 1.0;   /* make cut at this fraction of upper limit */
  limits[6] = nwires;

  for (layer = 0; layer < NLAYER; layer++) limits[6+2*layer +1] = 0.0;
  limits[7+2*0 +1] = 3.5;
  limits[7+2*1 +1] = 3.5;
  limits[7+2*2 +1] = 3.5;
  limits[7+2*3 +1] = 3.5;
  limits[7+2*4 +1] = 3.5;
  limits[7+2*5 +1] = 3.5;

  sprintf(plot_title, "average quality, %s", src_val[src_status]);
  one_page_plot_v3(25, src_status*2+2, MWIRE_PLOT, (float *)data, (float *)data, limits, 
		   csc_name, plot_title, 
		   "4/6 planes rate, avg quality", 0);

  /* Save histograms */
  file_his_name = malloc(strlen(datadir_translation) + 
			 strlen(test_25_file_his_el[src_status]) + 5);
  if (file_his_name == NULL)
    {
    printf("malloc() failed for file_his_name.  Could not save histograms.\n");
    }
  else
    {
    sprintf(file_his_name,"%s%s",
	    datadir_translation,test_25_file_his_el[src_status]);
    printf("Saving histograms\n");
    HRPUT(0, file_his_name, "N");
    free(file_his_name);    
    }


/* Print mandatory summary statement */
  if (nbadch > 0)
    {
      fprintf(fp,     "Test result = Failed\n Total %d bad channels\n", nbadch);
      fprintf(fp_bad, "Test result = Failed\n Total %d bad channels\n", nbadch);
    }
  else
    {
      fprintf(fp, "Test result = OK\n");
      fprintf(fp_bad, "Test result = OK\n");
    }

/* Add the date and time */
  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp, "%s\n\n", timestring);
  fprintf(fp_bad, "%s\n\n", timestring);

  fclose(fp);
  fclose(fp_bad);

  HDELET(0);

  return 0;
  }

/*******************************************************************************
   ALCT simulation subroutine
*******************************************************************************/

/*
 * Subroutine which takes ALCT raw hit data as input, and finds the expected
 * ALCT results.
 *
 * Necessary parameters are stored in an alct_params structure.
 */

int alct2000(void)
  {
  int              i, j, k, ilct, layer, mode, status;
  int              nhits_a, nhits_c;
  int              best_wg_a[2][NLCT], max_nhits_a[2][NLCT];
  int              best_wg_c[2][NLCT], max_nhits_c[2][NLCT];
  int              accel_mu[NLCT], nhits[NLCT], best_wg[NLCT];
  short int        accel[2], key[2], patt[2], valid[2], jm[3], jp[3];
  short int        active_lct, patt_quality;
  unsigned long    alct_s[NLAYER][nwires], data, *ldat, tbit;
  static unsigned long    mask[MAX_FIFO_SAMPLES];
  logical          debug_on, event_error, local_file;
  logical          pulse_on, pretrig_a, pretrig_c, pretrigger;

  static alct_params_type    p;

/* Initialize */
  valid[0] = 0;
  valid[1] = 0;
  debug_on = _FALSE;
  if (first_alct2000_call)
    {
    local_file = _TRUE;
    // this routine is not used anyway - Madorsky
//    read_afeb_config(upevt_.afeb_config_id, local_file, -1, &p);
    for (k = 0; k < MAX_FIFO_SAMPLES; k++) mask[k] = 1 << k;
    first_alct2000_call = _FALSE;
    }

/* These config file parameters are changed by the calibration software */
  p.nplanes_for_pretrig = plane_threshold;
  p.pattern_num_thr = pattern_threshold;
  status = 0;

/* Convert (as well as possible) from raw data to 150-ns-pulse-stretched data */

  for (layer = 0; layer < NLAYER; layer++)
    {
    for (j = 0; j < nwires; j++)
      {
      alct_s[layer][j] = 0;

      /* Logic for one-shot that's continuously retriggerable */
/*    if ((data = upevt_.alct_dump[layer][j]))
      {
      *      for (k = 0; k <  p.num_fifo_samples; k++)
      {
      *        if (mask[k] & data)
      {
      *          for (i = k; i < k + 6; i++) alct_s[layer][j] |= mask[i];
      *        }
      *      }
      *    }
      */
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
  
      /* Logic for one-shot that's retriggerable after output has gone to 0 */
/*    if ((data = upevt_.alct_dump[layer][j]))
      {
      for (k = 0; k <  p.num_fifo_samples; k++)
      {
      if (mask[k] & data)
      {
      for (i = k; i < k + 6; i++) alct_s[layer][j] |= mask[i];
      k += 6;
      }
      }
      }
*/
      /* Debug *//*
	 if (data) printf("L%d wg%2d upevt=%8x, alct_s=%8x\n", layer, j, 
	 data, alct_s[layer][j]);
		 */
      }
    }

/* Test to find out which channel is bad in ME234/2-001 */
//alct_s[0][16] = 0;

/* Do trigger logic twice, at pretrigger time and after the drift delay */
  /* I changed sligtly logic ( Because ALCT2001 is different), but now simulation is not
     correct  S.D. May 29,2002 */ 
  for (i = 0; i < 2; i++)
    {
      if (i == 0) k = p.num_fifo_samples-1; // was k = p.num_fifo_before_pretrig;
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
	  if ((ldat[jm[0]] | ldat[jm[1]] | ldat[jm[2]]) & tbit) nhits_c++; 
	  }
        else if (layer == 1)
	  {
          if ((ldat[jm[0]] | ldat[jm[1]]) & tbit) nhits_c++; 
	  }
        else if (layer == 2)
	  {
          if ((ldat[jm[0]]) & tbit) nhits_c++; 
	  }
        else if (layer == 3)
	  {
          if ((ldat[jp[0]] | ldat[jp[1]]) & tbit) nhits_c++; 
	  }
        else
	  {
          if ((ldat[jp[0]] | ldat[jp[1]] | ldat[jp[2]]) & tbit) nhits_c++; 
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

/* Debug 
   for (ilct = 0; ilct < 4; ilct++)
   {
   if (nhits[ilct] > 0) printf("Best LCT %d before resolver: nhits=%d, "
   "best_wg=%d, accel_mu=%d\n", ilct, nhits[ilct], best_wg[ilct], 
   accel_mu[ilct]);
   }
*/

/* Simulate "resolver" which removes duplicate LCT's at feb boundaries */ 
/* Note: the resolver doesn't work in the ALCT2000, so this code is disabled */
/*
  for (ilct = 0; ilct < 3; ilct++)
  {
  if ((nhits[ilct] > 0) && (nhits[ilct+1] > 0) && 
  ((best_wg[ilct+1] - best_wg[ilct]) < 4))
  {

//      if ((nhits[ilct+1] >= nhits[ilct]) && 
//       ((nhits[ilct] & 0x7) >= p.pattern_num_thr)){
//        printf("Event %ld: dropped track with key=%d nhits=%d from LCT %d\n", 
//         upevt_.event_number, best_wg[ilct], nhits[ilct], ilct);
//      }
//      else if ((nhits[ilct+1] < nhits[ilct]) && 
//       ((nhits[ilct+1] & 0x7) >= p.pattern_num_thr)){
//        printf("Event %ld: dropped track with key=%d nhits=%d from LCT %d\n", 
//         upevt_.event_number, best_wg[ilct+1], nhits[ilct+1], ilct+1);
//      }
//      debug_on = _TRUE;

if (nhits[ilct+1] >= nhits[ilct]) nhits[ilct] = 0;
else nhits[ilct+1] = 0;
}
} 
*/

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

/* Compare results with unpacked data from the ALCT */
  event_error = _FALSE;

  if (upevt_.alct_active_febs != active_lct)
    {
    printf("Error in event %ld: upevt_.alct_active_febs=%#x, active_lct=%#x\n",
	   upevt_.event_number, upevt_.alct_active_febs, active_lct);
    n_active_lct_errors++;
    event_error = _TRUE;
    }

  for (k = 0; k < 2; k++)
    {
    if (upevt_.alct_valid_patt[k] != valid[k])
      {
      printf("Error in event %ld: upevt_.alct_valid_patt[%d]=%d, "
	     "valid[%d]=%d\n", upevt_.event_number, k, upevt_.alct_valid_patt[k], 
	     k, valid[k]);
      n_valid_patt_errors++;
      event_error = _TRUE;
      }
    }

  for (k = 0; k < 2; k++)
    {
    patt_quality = (patt[k] & 0x7) - 3;
    if (patt_quality < 0) patt_quality = 0;
    if (upevt_.alct_patt_quality[k] != patt_quality)
      {
      printf("Error in event %ld: upevt_.alct_patt_quality[%d]=%d, "
	     "patt_quality=%d\n", upevt_.event_number, k, 
	     upevt_.alct_patt_quality[k], patt_quality);
      n_patt_quality_errors++;
      event_error = _TRUE;
      }
    }

  for (k = 0; k < 2; k++)
    {
    if (upevt_.alct_accel_muon[k] != accel[k])
      {
      printf("Error in event %ld: upevt_.alct_accel_muon[%d]=%d, "
	     "accel[%d]=%d\n", upevt_.event_number, k, upevt_.alct_accel_muon[k], 
	     k, accel[k]);
      n_accel_muon_errors++;
      event_error = _TRUE;
      }
    }

  for (k = 0; k < 2; k++)
    {
    if (upevt_.alct_wire_group[k] != key[k])
      {
      printf("Error in event %ld: upevt_.alct_wire_group[%d]=%d, key[%d]=%d\n",
	     upevt_.event_number, k, upevt_.alct_wire_group[k], k, key[k]);
      n_wire_group_errors++;
      event_error = _TRUE;
      }
    }

/* Also compare unpacked data with parameters read from the afeb_config file */
  if (upevt_.alct_nbucket != p.num_fifo_samples)
    {
    printf("Error in event %ld: upevt_.alct_nbucket=%d, "
	   "p.num_fifo_samples=%d\n", upevt_.event_number, upevt_.alct_nbucket, 
	   p.num_fifo_samples);
    n_header_errors++;
    event_error = _TRUE;
    }

  if (upevt_.alct_tr_mode != p.fifo_mode)
    {
    printf("Error in event %ld: upevt_.alct_tr_mode=%d, p.fifo_mode=%d\n",
	   upevt_.event_number, upevt_.alct_tr_mode, p.fifo_mode);
    n_header_errors++;
    event_error = _TRUE;
    }

/* Debug */
  if (event_error || debug_on)
    {
    for (layer = 0; layer < NLAYER; layer++)
      {
      for (j = 0; j < nwires; j++)
	{
        if (upevt_.alct_dump[layer][j]) printf("L%d wg %2d : 0x%08lx\n",
					       layer, j, upevt_.alct_dump[layer][j]);
	}
      }

    for (ilct = 0; ilct < 4; ilct++)
      {
      if (max_nhits_a[1][ilct] > 0 || max_nhits_c[1][ilct] > 0) 
	printf("PreTrig LCT %d: max_nhits_a=%d, max_nhits_c=%d, best_wg_a=%d, "
	       "best_wg_c=%d\n", ilct, max_nhits_a[0][ilct], max_nhits_c[0][ilct], 
	       best_wg_a[0][ilct], best_wg_c[0][ilct]);
      }

    for (ilct = 0; ilct < 4; ilct++)
      {
      if (max_nhits_a[1][ilct] > 0 || max_nhits_c[1][ilct] > 0) 
	printf("Trigger LCT %d: max_nhits_a=%d, max_nhits_c=%d, best_wg_a=%d, "
	       "best_wg_c=%d\n", ilct, max_nhits_a[1][ilct], max_nhits_c[1][ilct], 
	       best_wg_a[1][ilct], best_wg_c[1][ilct]);
      }

    for (ilct = 0; ilct < 4; ilct++)
      {
      if (nhits[ilct] > 0) printf("Best LCT %d after  resolver: nhits=%d, "
				  "best_wg=%d, accel_mu=%d\n", ilct, nhits[ilct], best_wg[ilct], 
				  accel_mu[ilct]);
      }

    for (j = 0; j < 2; j++)
      {
      if (valid[j]) printf("j=%d, patt[%d]=%d, key[%d]=%d, accel[%d]=%d\n",
			   j, j, patt[j], j, key[j], j, accel[j]);
      }

    printf("plane threshold = %d, pattern threshold = %d\n", 
	   p.nplanes_for_pretrig, p.pattern_num_thr);
    }

  if (event_error)
    {
    n_events_with_errors++;
    status = -2;
    }

  return(status);
  }

/*---------------------------------------------------------------------------*/

