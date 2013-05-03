/*
 * test_16.c (in V1.2  test_11.c)
 * CFEB Connectivity Test
 *
 * Author : Lisa Gorn
 * Date   : April 18, 2001
 *
 *  modified 12/11/2001 by V.Sytnik
 *  ( new method of connectivity check  )
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
#include "test_params_cxx.h"     /* test config parameters */
#include "test_config_struct.h"


// root headers
#include "TH1.h" // histograms
#include "TFile.h" // writing files

#include "one-page-plot.h"
#include "basic-utilities.h"


#define MAX_POINTS      256

/* Local function declarations */
int test_16_begin(int pass);
int test_16_event(int pass);
int test_16_end(int pass);
int test_16_finish(void);

/* Global variables local to this test */
static FILE       *fp;
static FILE   *fp_bad;
static int        first_istrip, nevents[NLAYER][NSTRIP], tsmax[2];
//static float      adc_diff[NLAYER][NSTRIP], adc_diff_err[NLAYER][NSTRIP];
static float      sum[NLAYER][NSTRIP], sumsq[NLAYER][NSTRIP];
static logical    first = _TRUE;

TH1F * minimum_adc_histogram;
TH1F * maximum_adc_histogram;

static float *padc_diff, *padc_diff_err;
static float *adc_diff[NLAYER], *adc_diff_err[NLAYER];

extern test_config_struct tcs;
extern int tot_evt;

char test_16_file_el[]="/test_results/temp/test_16_01.results";
char test_16_file_bad_el[]="/test_results/temp/test_16_01.bad";
char test_16_file_his_el[]="/test_results/temp/test_16_01.his";

/*******************************************************************************
   Define functions and parameters for FAST site test 16
*******************************************************************************/
static int nstrips, ncfebs;
extern "C" int test_16_init(void)
{
  int l;

  test_begin  = test_16_begin;
  test_event  = test_16_event;
  test_end    = test_16_end;
  test_finish = test_16_finish;

  num_passes = 2;
  num_events[0] = 10000;
  num_events[1] = 10000;

  switch (csc_type)
    {
    case 0:  nstrips = 48, ncfebs = 3; break;
    case 1:  nstrips = 64; ncfebs = 4; break;
    case 2:  nstrips = 64; ncfebs = 4; break;
    default: nstrips = 80; ncfebs = 5; break;
    }

  printf ("csc_type = %d, nstrips = %d\n", csc_type, nstrips);

  padc_diff     = (float*)malloc(sizeof(float) * NLAYER * nstrips);
  padc_diff_err = (float*)malloc(sizeof(float) * NLAYER * nstrips);

  for (l = 0; l < NLAYER; l++)
    {
      adc_diff[l]     = padc_diff     + nstrips * l;
      adc_diff_err[l] = padc_diff_err + nstrips * l;
    }

  return 0;
}

/*******************************************************************************
   Book histograms for FAST site test 16
*******************************************************************************/

int test_16_begin(int pass)
{
  int    ilayer, istrip;
  char *filename;
  char *filename_bad;

  if (pass == 0)
    {
      /* Initializations */
      first = _TRUE;
      filename = (char*)malloc(strlen(datadir_translation) +
                               strlen(test_16_file_el) + 1 );
      if (filename == NULL)
        {
          printf("malloc() failed on filename.\n");
          return 1;
        }

      filename_bad = (char*)malloc(strlen(datadir_translation) +
                                   strlen(test_16_file_bad_el) + 1 );
      if (filename_bad == NULL)
        {
          printf("malloc() failed on filename.\n");
          return 1;
        }
      sprintf(filename,"%s%s",datadir_translation,test_16_file_el);
      sprintf(filename_bad,"%s%s",datadir_translation,test_16_file_bad_el);

      /* Open result file in the temporary directory */
      fp = fopen(filename, "w");
      if(fp == NULL)
        {
          printf("File %s could not be opened.\n",filename);
          free(filename);
          return 1;
        }
      fp_bad = fopen(filename_bad, "w");
      if(fp_bad == NULL)
        {
          printf("File %s could not be opened.\n",filename_bad);
          free(filename_bad);
          return 1;
        }

      free(filename_bad);
      free(filename);

      fprintf(fp, "--------------------------------------------------------\n");
      fprintf(fp, "Results of test 16 (CFEB Connectivity) for %s\n", csc_name);
      fprintf(fp, "--------------------------------------------------------\n\n");

      fprintf(fp_bad, "--------------------------------------------------------\n");
      fprintf(fp_bad, "Results of test 16 (CFEB Connectivity) for %s\n", csc_name);
      fprintf(fp_bad, "--------------------------------------------------------\n\n");



      /* Book histograms */
      minimum_adc_histogram = new TH1F("timesample with minimum ADC",
                                       "minimum_adc_histogram",
                                       16, 0.0, 16.0);
      maximum_adc_histogram = new TH1F("timesample with minimum ADC",
                                       "maximum_adc_histogram",
                                       16, 0.0, 16.0);
    }

  else if (pass == 1) {
    for (ilayer = 0; ilayer < NLAYER; ilayer++) {
      for (istrip = 0; istrip < nstrips; istrip++){

        nevents[ilayer][istrip] = 0;
        sum[ilayer][istrip] = 0;
        sumsq[ilayer][istrip] = 0;

      }
    }
  }

  return 0;
}

/*******************************************************************************
   Do analysis and fill histograms for FAST site test 16
*******************************************************************************/

int test_16_event(int pass)
{
  int           i, j, ifeb, ilayer, istrip, status;
  int           adc_max, adc_min, jmax, jmin;
  float         x;
  int layerpair;

  // fill the params of legacy upevt_
  upevt_.alctcal_scan_mode = 7;
  // current layer pair
  upevt_.alctcal_current_value = upevt_.event_number / tcs.t16.events_per_layer;

  status = 0;

  if (pass == 0)
    {
      if (first)
        {
          first = _FALSE;
          first_istrip = nstrips;
          if (upevt_.alctcal_scan_mode != 7) printf("Wrong data file for test 16:"
                                                    " ALCTCAL scan mode should be 7, not %d\n", upevt_.alctcal_scan_mode);
        }

      /* Fill histogram to find typical locations of max and min samples */
      int first_cfeb = csc_type == 0 ? 4 : 0;
      for (ifeb = first_cfeb; ifeb < ncfebs + first_cfeb; ifeb++)
        {
          if (upevt_.active_febs[ifeb] > 0)
            {
              for (ilayer = 0; ilayer < NLAYER; ilayer++)
                {
                  layerpair = upevt_.alctcal_current_value;
                  for (istrip = 16 * ifeb; istrip < 16 + 16 * ifeb; istrip++)
                    {
                      if (first_istrip >= nstrips) first_istrip = istrip;
                      adc_max = -1;
                      adc_min = 4096;
                      for (j = 0; j < upevt_.nsca_sample; j++)
                        {
                          if (upevt_.sca[ilayer][istrip][j] > adc_max)
                            {
                              adc_max = upevt_.sca[ilayer][istrip][j];
                              jmax = j;
                            }
                          if (upevt_.sca[ilayer][istrip][j] < adc_min)
                            {
                              adc_min = upevt_.sca[ilayer][istrip][j];
                              jmin = j;
                            }
                        }
                      minimum_adc_histogram->Fill((float)jmin);
                      maximum_adc_histogram->Fill((float)jmax);
                    }
                }
            }
        }
    }

  else if (pass == 1)
    {
      layerpair = upevt_.alctcal_current_value;

      /* Fill arrays of sum of max-min and (max-min)**2 for each strip */
      int first_cfeb = csc_type == 0 ? 4 : 0;
      for (ifeb = first_cfeb; ifeb < ncfebs + first_cfeb; ifeb++)
        {
          if (upevt_.active_febs[ifeb] > 0)
            {
              for (ilayer = 2*layerpair; ilayer < 2*(layerpair+1); ilayer++)
                {
                  for (istrip = 16 * ifeb; istrip < 16 + 16 * ifeb; istrip++)
                    {
                      if(upevt_.sca[ilayer][istrip][tsmax[1]]==0)continue;
                      x = 0.;
                      for (i = tsmax[0]; i<=tsmax[1]; i++)
                        {
                          x += (float)upevt_.sca[ilayer][istrip][i] - (float)upevt_.sca[ilayer][istrip][0];
                        }
                      sum[ilayer][istrip] += x;
                      sumsq[ilayer][istrip] += x * x;
                      nevents[ilayer][istrip]++;
                    }
                }
            }
        }

    }
  return status;
}

/*******************************************************************************
   End-of-run analysis for FAST site test 16
*******************************************************************************/

int test_16_end(int pass)
{
  int j,i_max;
  float * contents;
  float max;
  char * file_his_name;

  printf("what is pass content%d\n",pass);

  if (pass == 0)
    {
      /* Find the maximum bin of the adc_max histrogram  */
      contents = maximum_adc_histogram->GetArray();
      max = -1.;
      for (j = 0; j < 16; j++)
        {
          if (contents[j] > max)
            {
              max = contents[j];
              i_max = j;
            }
        }
      tsmax[0] = i_max-2;
      tsmax[1] = i_max+2;
      if (tsmax[0] <= 0) tsmax[0] =  1;
      if (tsmax[1] > 16) tsmax[1] = 15;
      printf("Timesample with ADC min is %d, with ADC max is %d\n", tsmax[0],
             tsmax[1]);
      printf("First strip with data is %d\n", first_istrip + 1);

      /* Save histograms */
      file_his_name = (char*)malloc(strlen(datadir_translation) +
                                    strlen(test_16_file_his_el) + 1);
      if (file_his_name == NULL)
        {
          printf("malloc() failed for file_his_name.  "
                 "Could not save histograms.\n");
        }
      else
        {
          sprintf(file_his_name,"%s%s",
                  datadir_translation,test_16_file_his_el);
          printf("Saving histograms\n");
          TFile histogram_output_file(file_his_name, "RECREATE");
          minimum_adc_histogram->Write();
          maximum_adc_histogram->Write();
          free(file_his_name);
        }
    }

  return 0;
}

/*******************************************************************************
   End-of-test analysis for FAST site test 16
*******************************************************************************/

#define ADC_DIFF_LOWER_LIMIT              100.0
#define ADC_DIFF_UPPER_LIMIT              300.0

int test_16_finish(void) {
  int     ilayer, istrip, n, nbadch;
  float   avg, sigma_sq, limits[7];
  time_t  t;
  char    timestring[40];

  /* Initialize */
  nbadch = 0;

  /* Get average max response - min response */
  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    for (istrip = 0; istrip < nstrips; istrip++) {
      if ((n = nevents[ilayer][istrip]) > 1) {
        avg = sum[ilayer][istrip] / n;
        sigma_sq = (n / (n - 1)) * (sumsq[ilayer][istrip] / n - avg * avg);
        adc_diff[ilayer][istrip] = avg;
        adc_diff_err[ilayer][istrip] = sqrt(sigma_sq / n);
      }
      else if (n == 1) {
        adc_diff[ilayer][istrip]  = sum[ilayer][istrip];
        adc_diff_err[ilayer][istrip] = 10.;
      }
      else {
        adc_diff[ilayer][istrip] = -999;
        adc_diff_err[ilayer][istrip] = 0.;
      }
    }
  }

  limits[0] =  0.;                   /* layer histo lower limit */
  limits[1] =  500.;                   /* layer histo upper limit */
  limits[2] =  0.;                   /* summary histo lower limit */
  limits[3] =  500.;                   /* summary histo upper limit */
  limits[4] = ADC_DIFF_LOWER_LIMIT;   /* lower limit line */
  limits[5] = ADC_DIFF_UPPER_LIMIT;   /* upper limit line */
  limits[6] = nstrips;

  /* TODO: we need to decide how many cfebs there are per layer based on
     chamber type */
  int cfebs_per_layer = 3;

  one_page_plot_v3(16, 1, STRIP_PLOT, (float *)padc_diff, (float *)padc_diff_err,
                   limits, csc_name, "ADC difference", "diff, ADC counts",
                   1);
  one_page_plot(16, "ADC Difference", "CFEB Connectivity",
                limits[0], limits[1], "difference v ADC counts",
                csc_name, limits[4], limits[5],
                nstrips, cfebs_per_layer, NLAYER, twod_array_to_vectors((float*)padc_diff, NLAYER, nstrips),
                "test_16_01.pdf");

  fprintf(fp, "List of strips with (ADC max - ADC min)"
          "%4.1f - %4.1f :\n", ADC_DIFF_LOWER_LIMIT, ADC_DIFF_UPPER_LIMIT);
  fprintf(fp, "layer strip   adc_diff   adc_diff_error\n");
  fprintf(fp, "---------------------------------------------------------\n\n");
  /* Check that results are within limits */
  fprintf(fp_bad, "List of strips with (ADC max - ADC min) outside range "
          "%4.1f - %4.1f :\n", ADC_DIFF_LOWER_LIMIT, ADC_DIFF_UPPER_LIMIT);
  fprintf(fp_bad, "layer strip   adc_diff   adc_diff_error\n");
  fprintf(fp_bad, "---------------------------------------------------------\n\n");
  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    for (istrip = first_istrip; istrip < nstrips; istrip++) {
      fprintf(fp, "  %d    %2d     %8.3f  %8.3f\n", ilayer + 1, istrip + 1,
              adc_diff[ilayer][istrip], adc_diff_err[ilayer][istrip]);
      if ((adc_diff[ilayer][istrip] < ADC_DIFF_LOWER_LIMIT) ||
          (adc_diff[ilayer][istrip] > ADC_DIFF_UPPER_LIMIT)) {
        nbadch++;
        fprintf(fp_bad, "  %d    %2d     %8.3f  %8.3f\n", ilayer + 1, istrip + 1,
                adc_diff[ilayer][istrip], adc_diff_err[ilayer][istrip]);
      }
    }
  }
  fprintf(fp, "---------------------------------------------------------\n\n");
  fprintf(fp_bad, "---------------------------------------------------------\n\n");

  fprintf(fp_bad, "Total number of bad channels = %d\n", nbadch);

  /* Save histograms */

  /* Print mandatory summary statement */
  if (nbadch > 0)
    {
      fprintf(fp, "Test result = Failed\n\n");
      fprintf(fp_bad, "Test result = Failed\n\n");
    }
  else
    {
      fprintf(fp, "Test result = OK\n\n");
      fprintf(fp_bad, "Test result = OK\n\n");
    }

  /* Add the date and time */
  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp_bad, "%s\n\n", timestring);
  fprintf(fp, "%s\n\n", timestring);

  fclose(fp);
  fclose(fp_bad);

  delete minimum_adc_histogram;
  delete maximum_adc_histogram;

  return 0;
}
