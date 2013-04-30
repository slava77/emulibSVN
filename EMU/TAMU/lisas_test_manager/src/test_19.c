/*
 * test_19.c  (based on test_13.c)
 * CFEB Threshold and Analog Noise Test
 *
 * Author : Sergei Dolinsky( based on sorce of Valentin Sulimov)
 * Date   : 08 May 2002
 *
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
#include "packlib.h"
#include "daq_conf.h"
#include "csc_event.h"       /* upevt_                    */
#include "csc_parameters.h"  /* NLAYER, NSTRIP, etc...    */
#include "daq_histo.h"       /* PED_ID                    */
#include "one_page_plot.h"   /* STRIP_PLOT,...            */
#include "test_utils.h"      /* chamber labels            */
#include "test_params.h"     /* test config parameters    */
#include "test_19_ini.h"     /* test 19 config parameters */
#include "test_config_struct.h"

//#include "csc_unpacker.h"
//#include "emu/daq/reader/RawDataFile.h"

extern test_config_struct tcs;
extern int tot_evt;


/* Local function declarations */
int test_19_begin(int pass);
int test_19_event(int pass);
int test_19_end(int pass);
int test_19_finish(void);

int print_time_close_19(FILE *);
int print_scan_result_19(void );
int check_bad_channels_19(float **value , float , float , char* );
int get_next_scan_19(int );

int save_hist_19(void );
int fit_thresh_19(int );
int init_hist_19(void );
int get_CLCT_strips_19(short **clct_strips);

/* Global variables local to this test */
static FILE  *fp_bad;
static int    number_bad_channels = 0; // statistics of bad channels;
static int    num_points, num_scans;
static int    nscan = -1, nthresh = -1;

static float  thr[NMAXSCAN][NLAYER][NSTRIP], noise[NMAXSCAN][NLAYER][NSTRIP];
static float  calibration_pulse[NMAXSCAN];
static float  calibration_thresh[NMAXTHRESH];
static int    current_strip = -1 ;
/******************************************************************************
   Define functions and parameters for FAST site test 19
******************************************************************************/
int nstrips, ncfebs;

int test_19_init(void)
{
  test_begin  = test_19_begin;
  test_event  = test_19_event;
  test_end    = test_19_end;
  test_finish = test_19_finish;

  num_passes = 1;
  num_events[0] = 100000;

  switch (csc_type)
    {
    case 0:  nstrips = 48; ncfebs = 3; break;
    case 1:  nstrips = 64; ncfebs = 4; break;
    case 2:  nstrips = 64; ncfebs = 4; break;
    default: nstrips = 80; ncfebs = 5; break;
    }

  printf ("csc_type = %d, nstrips = %d\n", csc_type, nstrips);


  return 0;
}

/******************************************************************************
   Book histograms for FAST site test 19
******************************************************************************/

int test_19_begin(int pass)
{
  char *file_name;
  printf ("The calibration coeficients are A0=%3.2f A1=%3.2f \n",
          INJECT_PULSE_A0, INJECT_PULSE_A1);
  /* compute the filename */
  file_name = malloc(strlen(datadir_translation) +
                     strlen(FILE_RESULTS) + 20);
  if (file_name == NULL)
    {
      printf("malloc() failed for file_name.\n");
      return -1;
    }

  /* Open result file in the temporary directory */
  sprintf(file_name,"%s%s%02d.bad", datadir_translation, FILE_RESULTS, 7);
  if ( (fp_bad = fopen(file_name, "w")) == NULL)
    {
      printf (" Test_19 Print results: can not open file %s\n", file_name);
      free(file_name);
      return 1;
    }
  free(file_name);
  fprintf(fp_bad, "------------------------------------------"
          "------------------------------------------\n");
  fprintf(fp_bad, "Results of test 19 (CFEB Threshold"
          " and Analog Noise Test ) %s\n", csc_name);
  fprintf(fp_bad, "Bad channels(Layer, Strip and problem description:\n");
  fprintf(fp_bad, "------------------------------------------"
          "------------------------------------------\n");
  init_hist_19();
  return 0;
}

/******************************************************************************
   Do analysis and fill histograms for FAST site test 19
******************************************************************************/

int test_19_event(int pass)
{
  int           hid, ilayer, istrip, iboard, ibucket;
  static int    first = 1;
  static int    current_pulse_dac = -1 ;
  static int    old_current_threshold = -1;

  short clct_strips[NLAYER][NSTRIP];

  // assign legacy values
  upevt_.stripcal_num_points = tcs.t19.dmb_tpamps_per_strip;
  upevt_.stripcal_num_points_turnoff = tcs.t19.threshs_per_tpamp;

  upevt_.stripcal_current_strip = // current strip
    (upevt_.event_number/(tcs.t19.dmb_tpamps_per_strip * tcs.t19.threshs_per_tpamp * tcs.t19.events_per_thresh)) * tcs.t19.strip_step +
    tcs.t19.strip_first;


  upevt_.stripcal_current_value = // current DAC value
    ((upevt_.event_number % (tcs.t19.dmb_tpamps_per_strip * tcs.t19.threshs_per_tpamp * tcs.t19.events_per_thresh)) / tcs.t19.threshs_per_tpamp / tcs.t19.events_per_thresh) *  tcs.t19.dmb_tpamp_step +
    tcs.t19.dmb_tpamp_first;

  // calculate thresh_first based on current dac value
  tcs.t19.thresh_first = upevt_.stripcal_current_value * tcs.t19.scale_turnoff / 16 - tcs.t19.range_turnoff;
  if (tcs.t19.thresh_first < 0) tcs.t19.thresh_first = 0;

  upevt_.stripcal_current_value_2 = // current threshold
    ((upevt_.event_number % (tcs.t19.threshs_per_tpamp * tcs.t19.events_per_thresh)) / tcs.t19.events_per_thresh) * tcs.t19.thresh_step +
    tcs.t19.thresh_first;
  //	printf("event %d, current strip %d, current value %d, current thre %d, curr strip cal2 %d\n",upevt_.event_number, upevt_.stripcal_current_strip, upevt_.stripcal_current_value, tcs.t19.thresh_first,upevt_.stripcal_current_value_2);
  /* Make an array for later use */
  if (first)
	{
      first = 0;
      /* we need to know num_points_with_thresh_scans and num_points_with_pulse_scan */
      num_scans = upevt_.stripcal_num_points;
      num_points = upevt_.stripcal_num_points_turnoff;
      printf (" The number of points  is : %d  \n",num_points);
      printf (" The number of scans is %d  \n",num_scans);
      if (num_scans > NMAXTHRESH )
		{
          printf ("*********** Test 19 Error ***************\n");
          printf (" The number of points %d is more than NMAXTHRESH=%d \n",num_points,NMAXTHRESH);
		}
      if (num_scans > NMAXSCAN )
		{
          printf ("*********** Test 19 Error ***************\n");
          printf (" The number of scans %d is more than NMAXSCAN=%d \n",num_scans,NMAXSCAN);
		}
	}

  /* Each events */

  if (upevt_.stripcal_current_strip != current_strip)
	{                                                     // next strip
      if ( nscan >=0) fit_thresh_19 (nscan);
      current_strip = upevt_.stripcal_current_strip;
      //		printf (" evnum: %d, strip: %d\n", upevt_.event_number, current_strip);
      nscan   = -1;
      current_pulse_dac = -1;
	}

  /* Next check, when we change amplitude of pulse (curent value),
     that means we begin new scan */
  if (upevt_.stripcal_current_value != current_pulse_dac)
	{
      if ( nscan >=0) fit_thresh_19 (nscan);
      nscan++ ;
      calibration_pulse[nscan] = upevt_.stripcal_current_value;
      current_pulse_dac = upevt_.stripcal_current_value;
      nthresh = -1;
      //		printf ("evnum: %d,  DAC: %d, nscan = %d\n", upevt_.event_number, current_pulse_dac, nscan);
	}

  if (upevt_.stripcal_current_value_2 != old_current_threshold)
	{
      old_current_threshold = upevt_.stripcal_current_value_2;
      nthresh++;
      calibration_thresh[nthresh] = old_current_threshold;
      //		printf ("evnum: %d,  thresh: %d, nscan = %d\n", upevt_.event_number, old_current_threshold, nscan);
      //	printf ("calth[%d] = %.2f, current = %.2f\n", nthresh, calibration_thresh[nthresh], upevt_.stripcal_current_value_2);
	}
  //    printf ("number in upevt%d, upevttime%d\n",upevt_.event_number,upevt_.time_musec);
  hid = STRIP_HIST_OFFSET+10000*(nscan+1) +3000;
  HFILL(hid, (float )(upevt_.event_number), 0.0, upevt_.time_musec);

  memset (clct_strips, 0,NLAYER*NSTRIP*sizeof(short));
  //get_CLCT_strips_19(clct_strips); // unpack DiStrips in Strips

  for (ilayer = 0; ilayer < NLAYER; ilayer++)
	{
      for (iboard= 0; iboard < NCFEB; iboard++)
		{
          istrip = iboard *16 + current_strip-1;
          clct_strips[ilayer][istrip]=upevt_.clct_dump_strips[ilayer][istrip];
          if (clct_strips[ilayer][istrip]) // we believe that there is just one hit per event
			{
              hid = STRIP_HIST_OFFSET+10000*(nscan+1) + 2000 +
                100 *  (ilayer + 1) + istrip + 1;
              HFILL(hid, (float )(nthresh+1), 0.0, 1.0);

              for (ibucket = 0; ibucket < upevt_.clct_nbucket; ibucket++)
				{
                  if (clct_strips[ilayer][istrip] & 1<<ibucket)
					{
                      hid = STRIP_HIST_OFFSET+10000*(nscan+1) +1000+
                        100*(ilayer+1) + istrip+1;
                      HFILL(hid, (float )(ibucket+1), 0.0, 1.0);
                      /* Fill Cathode front-end board histograms */
                      hid = STRIP_HIST_OFFSET+10000*(nscan+1)
                        + 100 + NCFEB*ilayer + iboard + 1;
                      HFILL(hid, (float )ibucket, 0.0, 1.0);
					}
				}
              /* Fill Strip occupancy in a layer */
              hid = STRIP_HIST_OFFSET+10000*(nscan+1) + ilayer + 1;
              HFILL(hid, (float )(istrip+1), 0.0, 1.0);

              /*  Calibration CLCT pulses  */

              hid = STRIP_HIST_OFFSET+10000*(nscan+1) + 200 + NCFEB*ilayer + iboard + 1;
              HFILL(hid, (float )current_pulse_dac, 0.0, 1.0);

			}
		}
	}

  return 0;
}
/******************************************************************************
   End-of-run analysis for FAST site test 19
******************************************************************************/

int test_19_end(int pass)
{
  fit_thresh_19(nscan);  // we should analyze last scan
  return 0;
}

/******************************************************************************
   End-of-test analysis for FAST site test 19
******************************************************************************/

int test_19_finish(void)
{

  char *file_name;
  FILE *fp_board, *fp_thr;
  FILE *fp_slop;

  float   lim_slop[7] =    SLOP_LIMITS_SCAN_00;
  float   lim_delta_dac[10][7] = { DELTA_DAC_LIMITS_SCAN_00,
                                   DELTA_DAC_LIMITS_SCAN_01};
  float   delta_dac[NMAXSCAN][NLAYER][NSTRIP];
  int iscan, istrip, ilayer, iboard, ichan, n_good_channels;
  int num_bad_thr, num_bad_slop;
  float dac[2][NCFEB];
  int idac[2][NCFEB];
  float slop[NLAYER][NSTRIP];
  float delta_inj;
  int page_number;
  char title[80];
  int i;

  float zzz[NLAYER * nstrips];

  save_hist_19();

  print_scan_result_19();

  for( iscan = 0 ; iscan < 2; iscan++)
    {
      file_name = malloc(strlen(datadir_translation) +
                         strlen(FILE_RESULTS) + 20);
      if(file_name == NULL)
        {
          printf("malloc() failed for filename.\n");
          return 1;
        }
      sprintf(file_name,"%s%s%02d.result", datadir_translation,
              FILE_RESULTS, 2*iscan + 1);
      if ( (fp_board = fopen(file_name, "w")) == NULL)
        {
          free(file_name);
          printf (" Test_19 Print results: can not open file %s\n", file_name);
          return 1;
        }

      free(file_name);
      fprintf(fp_board, "-----------------------------------------"
              "------------------------------\n");
      fprintf(fp_board, "Results of test 19 (CFEB Threshold and Analog "
              "Noise Test ) %s\n", csc_name);
      fprintf(fp_board, "Optimal mV%1d values per CFEB for %.2f "
              "fC threshold\n",iscan+1,
              INJECT_PULSE_A0+INJECT_PULSE_A1*calibration_pulse[iscan]);
      fprintf(fp_board, "-----------------------------------------"
              "------------------------------\n");

      for (iboard = 0; iboard < NCFEB; iboard++)
        {
          n_good_channels = 0;
          dac[iscan][iboard] = -1.5;
          for (ilayer= 0; ilayer < NLAYER; ilayer++)
            {
              for (ichan = 0; ichan<16 ; ichan++)
                {
                  istrip  = 16 * iboard + ichan;
                  if (thr[iscan][ilayer][istrip] > 0.)
                    {
                      dac[iscan][iboard] += thr[iscan][ilayer][istrip];
                      n_good_channels++;
                    }
                }
            }

          if (n_good_channels > 0) dac[iscan][iboard] /= n_good_channels;
          idac[iscan][iboard] = dac[iscan][iboard] + 0.5;
        }

      for(iboard = 0; iboard < ncfebs; iboard++)
        {
          fprintf (fp_board,
                   " %2d \t %3d \n",
                   iboard+1, idac[iscan][iboard]);
        }
      print_time_close_19(fp_board);

      file_name = malloc(strlen(datadir_translation) +
                         strlen(FILE_RESULTS) + 20);
      if(file_name == NULL)
        {
          printf("malloc() failed for filename.\n");
          return 1;
        }
      sprintf(file_name,"%s%s%02d.result",
              datadir_translation, FILE_RESULTS, 2*iscan+2);
      if ( (fp_thr = fopen(file_name, "w")) == NULL)
        {
          free(file_name);
          printf (" Test_19 Print results: can not open file %s\n", file_name);
          return 1;
        }
      free(file_name);

      fprintf(fp_thr, "--------------------------------------"
              "---------------------------------\n");
      fprintf(fp_thr, "Results of test 19 (CFEB Threshold and "
              "Analog Noise Test ) %s\n", csc_name);
      fprintf(fp_thr, "Threshold offset (in mV) per chanel for %.2f "
              "fC threshold\n",
              INJECT_PULSE_A0+INJECT_PULSE_A1*calibration_pulse[iscan]);
      fprintf(fp_thr, "--------------------------------------"
              "---------------------------------\n");

      for (istrip = 0; istrip < NSTRIP; istrip++)
        {
          for (ilayer = 0; ilayer < NLAYER; ilayer++)
            {
              delta_dac[iscan][ilayer][istrip] =
                thr[iscan][ilayer][istrip] - idac[iscan][istrip/16];
              if(thr[iscan][ilayer][istrip] <0.) delta_dac[iscan][ilayer][istrip] = -999.;
            }

          if (istrip < nstrips)
            fprintf(fp_thr," %2d %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f\n", istrip+1,
                    delta_dac[iscan][0][istrip], delta_dac[iscan][1][istrip],
                    delta_dac[iscan][2][istrip], delta_dac[iscan][3][istrip],
                    delta_dac[iscan][4][istrip], delta_dac[iscan][5][istrip]);
        }

      page_number = 2*(iscan)+2;
      sprintf( title, "CFEB Thr. offset for %6.1f fC ",
               INJECT_PULSE_A0+calibration_pulse[iscan]*INJECT_PULSE_A1 );


      for (i = 0; i < NLAYER; i++)
        memmove (&zzz [nstrips*i], &((float*)delta_dac[iscan])[NSTRIP*i], nstrips*sizeof(float));

      one_page_plot_v3(19, page_number, STRIP_PLOT,
                       //		       (float *)delta_dac[iscan],
                       //		       (float *)delta_dac[iscan],
                       zzz, zzz,
                       lim_delta_dac[iscan], csc_name,
                       title,
                       "Threshold, mV ", 0);

      /*--------- check bad channels ------------*/
      num_bad_thr =0;
      for (istrip = 0; istrip < nstrips; istrip++)
        {
          for (ilayer = 0; ilayer < NLAYER; ilayer++)
            {
              if ((delta_dac[iscan][ilayer][istrip] < lim_delta_dac[iscan][4] ) ||
                  (delta_dac[iscan][ilayer][istrip] > lim_delta_dac[iscan][5] ))
                {
                  num_bad_thr++;
                  number_bad_channels++; // statistics of bad channels;
                  fprintf (fp_bad,"%d %3d %6.1f%s  %6.1f  %6.1f  %s %6.1f fC\n",
                           ilayer+1, istrip+1, delta_dac[iscan][ilayer][istrip],
                           "\tDelta_mV out of limit",
                           lim_delta_dac[iscan][4], lim_delta_dac[iscan][5],
                           "for inj.pulse ",
                           INJECT_PULSE_A0+calibration_pulse[iscan]*INJECT_PULSE_A1 );
                }
            }
        }

      if (num_bad_thr > 0)
        fprintf(fp_thr," Total %d problem(s) was(were) found.\n Test Failed. \n",
                num_bad_thr);
      else
        fprintf(fp_thr," There was no problem for this test.\n Test OK. \n");
      print_time_close_19(fp_thr);
    }

  file_name = malloc(strlen(datadir_translation) +
                     strlen(FILE_RESULTS) + 20);
  if(file_name == NULL)
    {
      printf("malloc() failed for filename.\n");
      return 1;
    }
  sprintf(file_name,"%s%s%02d.result", datadir_translation,
          FILE_RESULTS, 5);
  if ( (fp_slop = fopen(file_name, "w")) == NULL)
    {
      free(file_name);
      printf (" Test_19 Print results: can not open file %s\n", file_name);
      return 1;
    }
  free(file_name);

  fprintf(fp_slop, "------------------------------------------"
          "------------------------------------------\n");
  fprintf(fp_slop, "Results of test 19 (CFEB Threshold and "
          "Analog Noise Test ) %s\n", csc_name);
  fprintf(fp_slop, "Threshold slop (in mV_Thr/fC) vs chanel\n");
  fprintf(fp_slop, "------------------------------------------"
          "------------------------------------------\n");

  for (istrip = 0; istrip < NSTRIP; istrip++)
    {
      for (ilayer = 0; ilayer < NLAYER; ilayer++)
        {
          slop[ilayer][istrip] = -1;
          if ((thr[0][ilayer][istrip] >0.) &&(thr[1][ilayer][istrip] >0.) &&
              (noise[0][ilayer][istrip] >0.) &&(noise[1][ilayer][istrip] >0.))
            {
              slop[ilayer][istrip] = (thr[1][ilayer][istrip]-thr[0][ilayer][istrip]) /
                (calibration_pulse[1]-calibration_pulse[0])/INJECT_PULSE_A1;
            }
        }
      if (istrip < nstrips)
        fprintf(fp_slop," %2d   %7.2f   %7.2f   %7.2f   %7.2f   %7.2f   %7.2f\n", istrip+1,
                slop[0][istrip], slop[1][istrip], slop[2][istrip],
                slop[3][istrip], slop[4][istrip], slop[5][istrip]);
    }

  num_bad_slop = 0;
  for (istrip = 0; istrip < nstrips; istrip++)
    {
      for (ilayer = 0; ilayer < NLAYER; ilayer++)
        {
          if ((slop[ilayer][istrip] < lim_slop[4] ) ||
              (slop[ilayer][istrip] > lim_slop[5] ))
            {
              num_bad_slop++;
              number_bad_channels++; // statistics of bad channels
              fprintf (fp_bad,"%d %3d %8.2f %s %6.2f  %6.2f\n",
                       ilayer+1, istrip+1, slop[ilayer][istrip],
                       "mV/fC slope out of limit ",
                       lim_slop[4], lim_slop[5]);
            }
        }
    }
  if (num_bad_slop > 0)
    fprintf(fp_slop," Total %d problem(s) was(were) found.\n Test Failed. \n",
            num_bad_slop);
  else
    fprintf(fp_slop," There was no problem for this test.\n Test OK. \n");

  print_time_close_19(fp_slop);

  for (i = 0; i < NLAYER; i++)
    memmove (&zzz [nstrips*i], &((float*)slop)[NSTRIP*i], nstrips*sizeof(float));

  page_number = 5;
  one_page_plot_v3(19, page_number, STRIP_PLOT,
                   //		   (float *)slop,
                   //		   (float *)slop,
                   zzz, zzz,
                   lim_slop, csc_name,
                   "CFEB Threshold slopes  ",
                   "Slope, mV_thr/fC ", 0);


  file_name = malloc(strlen(datadir_translation) +
                     strlen(FILE_RESULTS) + 20);
  if(file_name == NULL)
    {
      printf("malloc() failed for filename.\n");
      return 1;
    }

  sprintf(file_name,"%s%s%02d.result",
          datadir_translation, FILE_RESULTS, 6);
  printf (" Test_19 Print result for  %s\n", file_name);
  if ( (fp_slop = fopen(file_name, "w")) == NULL)
    {
      free(file_name);
      printf (" Test_19 Print results: can not open file %s\n", file_name);
      return 1;
    }
  free(file_name);

  fprintf(fp_slop, "-------------------------------------------"
          "-------------------------------------------\n");
  fprintf(fp_slop, "Results of test 19 (CFEB Threshold and "
          "Analog Noise Test ) %s\n", csc_name);
  fprintf(fp_slop, "Threshold slop (in mV_thr/fC) vs CFEB\n");
  fprintf(fp_slop, "-------------------------------------------"
          "-------------------------------------------\n");

  delta_inj = (calibration_pulse[1] - calibration_pulse[0])*INJECT_PULSE_A1;
  for (iboard = 0; iboard < ncfebs; iboard++)
    {
      fprintf (fp_slop," %2d %6.2f\n",
               iboard+1,  (dac[1][iboard] - dac[0][iboard] ) / delta_inj);
    }

  print_time_close_19(fp_slop);

  if (number_bad_channels > 0)
    fprintf(fp_bad," Total %d problem(s) was(were) found.\n Test Failed. \n",
            number_bad_channels);
  else
    fprintf(fp_bad," There was no problem for this test.\n Test OK. \n");
  print_time_close_19(fp_bad);

  return 0;
}

/*----------------------------- print_time_close --------------------------*/
int print_time_close_19(FILE *fp)
{
  char timestring[40];
  time_t t;

  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp, "\n%s\n\n", timestring);

  fclose(fp);
  return 0;
}

/*-------------------------- print_scan_result_19 -----------------------------*/
#define MAX_CALIB_POINTS   256

int print_scan_result_19()
{
  int       n_bad_str_thr, n_bad_str_noise;
  int       istrip, ilayer, iscan;
  char prototype_name[80] = FILE_RESULTS;
  char *file_name;
  char title[80];
  FILE *fp_noise, *fp_thr;

  float   lim_thr[10][7] = {
    THR_LIMITS_SCAN_00,    THR_LIMITS_SCAN_01,
    THR_LIMITS_SCAN_02,    THR_LIMITS_SCAN_03,
    THR_LIMITS_SCAN_04,    THR_LIMITS_SCAN_05,
    THR_LIMITS_SCAN_06,    THR_LIMITS_SCAN_07,
    THR_LIMITS_SCAN_08,    THR_LIMITS_SCAN_09};

  float   lim_noise[10][7] = {
    NOISE_LIMITS_SCAN_00,    NOISE_LIMITS_SCAN_01,
    NOISE_LIMITS_SCAN_02,    NOISE_LIMITS_SCAN_03,
    NOISE_LIMITS_SCAN_04,    NOISE_LIMITS_SCAN_05,
    NOISE_LIMITS_SCAN_06,    NOISE_LIMITS_SCAN_07,
    NOISE_LIMITS_SCAN_08,    NOISE_LIMITS_SCAN_09};

  int page_number;
  int i;
  float zzz[NLAYER * nstrips];


  // Open files for results
  for (iscan = 0; iscan <= nscan; iscan++)
    {

      file_name = malloc(strlen(datadir_translation) +
                         strlen(FILE_RESULTS) + 20);
      if(file_name == NULL)
        {
          printf("malloc() failed for filename.\n");
          return 1;
        }
      sprintf(file_name,"%s%s%02d.result",
              datadir_translation, prototype_name, 2*iscan+8);
      if ( (fp_noise = fopen(file_name, "w")) == NULL)
        {
          free(file_name);
          printf (" Test_19 Print results: can not open file %s\n", file_name);
          return 1;
        }
      free(file_name);

      file_name = malloc(strlen(datadir_translation) +
                         strlen(FILE_RESULTS) + 20);
      if(file_name == NULL)
        {
          printf("malloc() failed for filename.\n");
          return 1;
        }
      sprintf(file_name,"%s%s%02d.result",
              datadir_translation, prototype_name, 2*iscan+9);
      if ( (fp_thr = fopen(file_name, "w")) == NULL)
        {
          free(file_name);
          printf (" Test_19 Print results: can not open file %s\n", file_name);
          return 1;
        }
      free(file_name);

      fprintf(fp_noise, "-----------------------------------------------------\n");
      fprintf(fp_noise, "Results of test 19 (CFEB Threshold and "
              "Analog Noise Test ) %s\n", csc_name);

      fprintf(fp_noise, "-----------------------------------------------------\n");

      fprintf(fp_thr, "-----------------------------------------------------\n");
      fprintf(fp_thr, "Results of test 19 (CFEB Threshold and "
              "Analog Noise Test ) %s\n", csc_name);
      fprintf(fp_thr, "-----------------------------------------------------\n");

      fprintf(fp_noise, "\nMeasured Noise for all channels,");
      fprintf(fp_noise, " in mV_thr for %.2f fC injected pulse\n",
              INJECT_PULSE_A0+INJECT_PULSE_A1*calibration_pulse[iscan]);

      fprintf(fp_noise, "Strips       Layer 1    Layer 2    Layer 3    "
              "Layer 4    Layer 5    Layer 6  \n");
      fprintf(fp_noise, "--------------------------------------"
              "--------------------------------------\n");

      for (istrip = 0; istrip < nstrips; istrip++)
        {
          fprintf(fp_noise, "   %2d     %6.2f    %6.2f    %6.2f  "
                  " %6.2f    %6.2f    %6.2f _\n", istrip + 1,
                  noise[iscan][0][istrip], noise[iscan][1][istrip],
                  noise[iscan][2][istrip], noise[iscan][3][istrip],
                  noise[iscan][4][istrip], noise[iscan][5][istrip]);
        }

      fprintf(fp_thr, "\nMeasured threshold for all channels,");
      fprintf(fp_thr, " in mV_thr for %.2f fC injected pulse\n",
              INJECT_PULSE_A0+INJECT_PULSE_A1*calibration_pulse[iscan]);

      fprintf(fp_thr, "wiregroup    Layer 1    Layer 2    Layer 3    "
              "Layer 4    Layer 5    Layer 6  \n");
      fprintf(fp_thr, "--------------------------------------"
              "--------------------------------------\n");
      for (istrip = 0; istrip < nstrips; istrip++)
        {
          fprintf(fp_thr, "   %2d     %6.2f    %6.2f    %6.2f    "
                  "%6.2f    %6.2f    %6.2f _\n",
                  istrip+1, thr[iscan][0][istrip], thr[iscan][1][istrip],
                  thr[iscan][2][istrip], thr[iscan][3][istrip],
                  thr[iscan][4][istrip], thr[iscan][5][istrip]);
        }

      /*--- check bad channels only for first two scans ---*/
      if(iscan <2)
        {
          n_bad_str_thr = 0;
          n_bad_str_noise = 0;

          for (ilayer = 0 ; ilayer < NLAYER; ilayer++)
            {
              for (istrip = 0; istrip < nstrips; istrip++)
                {
                  if ( (thr[iscan][ilayer][istrip] < lim_thr[iscan][4] ) ||
                       (thr[iscan][ilayer][istrip] > lim_thr[iscan][5]))
                    {
                      n_bad_str_thr++;
                      number_bad_channels++; // statistics of bad channels;
                      fprintf (fp_bad,"%d %3d %7.2f %s   %7.2f  %7.2f %s %8.2f \n",
                               ilayer+1, istrip+1, thr[iscan][ilayer][istrip],
                               "\tThreshold out of limit\t",
                               lim_thr[iscan][4], lim_thr[iscan][5],
                               "inj.pulse ",
                               INJECT_PULSE_A0+calibration_pulse[iscan]*INJECT_PULSE_A1 );

                    }

                  if ( (noise[iscan][ilayer][istrip] < lim_noise[iscan][4] ) ||
                       (noise[iscan][ilayer][istrip] > lim_noise[iscan][5]))
                    {
                      n_bad_str_noise++;
                      number_bad_channels++; // statistics of bad channels;
                      fprintf (fp_bad,"%d %3d %7.2f %s   %7.2f  %7.2f %s %8.2f\n",
                               ilayer+1, istrip+1, noise[iscan][ilayer][istrip],
                               "\tNoise out of limit\t",
                               lim_noise[iscan][4], lim_noise[iscan][5],
                               "inj.pulse ",
                               INJECT_PULSE_A0+calibration_pulse[iscan]*INJECT_PULSE_A1 );


                    }
                }
            }

          /* Print mandatory summary statement */
          if (n_bad_str_noise > 0) fprintf(fp_noise, "Test result = Failed\n\n");
          else                     fprintf(fp_noise, "Test result = OK\n\n");
          if (n_bad_str_thr > 0)   fprintf(fp_thr,   "Test result = Failed\n\n");
          else                     fprintf(fp_thr,   "Test result = OK\n\n");
        }

      /* Add the date and time */
      print_time_close_19(fp_noise);
      print_time_close_19(fp_thr);

      /* Plot results */

      page_number = 2*iscan + 9;

      sprintf( title, "Thresholds for %6.1f fC ",
               INJECT_PULSE_A0+calibration_pulse[iscan]*INJECT_PULSE_A1 );

      for (i = 0; i < NLAYER; i++)
        memmove (&zzz [nstrips*i], thr[iscan][i], nstrips*sizeof(float));

      one_page_plot_v3(19, page_number, STRIP_PLOT,
                       //		       (float *)thr[iscan],
                       //		       (float *)thr[iscan],
                       zzz, zzz,
                       lim_thr[iscan], csc_name,
                       title,
                       "Threshold, mV ", 0);
      sprintf( title, "Noise for %6.1f fC ",
               INJECT_PULSE_A0+calibration_pulse[iscan]*INJECT_PULSE_A1 );

      for (i = 0; i < NLAYER; i++)
        memmove (&zzz [nstrips*i], noise[iscan][i], nstrips*sizeof(float));

      page_number = 2*(iscan) + 8;
      one_page_plot_v3(19, page_number, STRIP_PLOT,
                       //		       (float *)noise[iscan],
                       //		       (float *)noise[iscan],
                       zzz, zzz,
                       lim_noise[iscan], csc_name,
                       title,
                       "Noise, mV ", 0);
    }
  return 0;
}

/*------------------------ get_CLCT_strips_19 ------------------------------*/

int  get_CLCT_strips_19(short **clct_strips)
{
  int ilayer, idistrip, ibucket;
  short itemp;
  int unpk_strip;

  memset (clct_strips, 0,NLAYER*NSTRIP*sizeof(short));

  if ( upevt_.clct_nbucket > NMAXBUCKET)
    {
      printf ( " Error get_CLCT_strips_19 in event %ld nbucket:%d more then NMAXBUCKET \n",
               upevt_.event_number, upevt_.clct_nbucket);
      return -1;
    }
  else
    {
      for (ilayer = 0; ilayer< NLAYER; ilayer++)
        {
          for (idistrip = 0; idistrip< NSTRIP/2; idistrip++)
            {
              for (ibucket = 0 ; ibucket < (upevt_.clct_nbucket -2); )
                {
                  itemp = upevt_.clct_dump[ilayer][idistrip];
                  if (itemp & 1<<ibucket)
                    {
                      unpk_strip = (itemp & (1<<(ibucket+1)) ) ? (2*idistrip+1) : (2*idistrip);
                      clct_strips[ilayer][unpk_strip] |= (1<<ibucket);
                      ibucket += 3;

                    }
                  else
                    {
                      ibucket++;
                    }
                }
            }
        }
    }
  return 0;
}

/*----------------------------- save_hist_19 ----------------------------------*/
int save_hist_19()
{

  char file_name_element[] = "/test_results/temp/test_19_";
  char *file_name;
  file_name = malloc(strlen(datadir_translation) +
                     strlen(file_name_element) + 10);
  if (file_name == NULL)
    {
      printf("malloc() failed for file_name.\n");
      return -1;
    }
  sprintf(file_name,"%s%s%02d.his",datadir_translation,
          file_name_element,1);
  /* Save histograms */
  printf("Saving histograms in file %s\n", file_name);
  HRPUT(0, file_name, "N");
  HDELET(0);
  free(file_name);
  return 0;
}

/*-------------------------- fit_thresh_19 -------------------------------------*/
int fit_thresh_19(int scan_number)
{

  int       i, hid, ID, ilayer, istrip, iboard, i_ch;
  int       content[MAX_CALIB_POINTS];
  float     chisq, mean, par[3], rms;
  float     errors[MAX_CALIB_POINTS];

  i_ch = 0;
  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
      for (iboard = 0; iboard < NCFEB; iboard++)
        {
          istrip = iboard * 16 + current_strip-1;
          if ((iboard == 0) && (current_strip == 16)) printf( "%d %d: ", scan_number, upevt_.event_number);
          ID = STRIP_HIST_OFFSET + (scan_number+1) * 10000 + 2000 +
            100 * (ilayer + 1) + (istrip + 1);
          for (i = 0 ; i <  num_points; i++)
            {
              content[i] = HI(ID,i+1);
              if ((iboard == 0) && (current_strip == 16))

                printf( "%2d %s", content[i], (i==(num_points-1)?"\n": ""));
            }

          calc_thresh(num_points, content, errors, par, &chisq);

          hid = STRIP_HIST_OFFSET + (scan_number+1)*10000 + 10;
          HFILL(hid, chisq, 0., 1.);

          if (chisq >= 0.)
            {
              mean = par[1]*(calibration_thresh[1]-calibration_thresh[0])+ calibration_thresh[0];
              //	      printf ("par[1]: %.2f calth[1]: %.2f calth[0]: %.2f \n", par[1], calibration_thresh[1], calibration_thresh[0]);
              rms =  par[2]*(calibration_thresh[1]-calibration_thresh[0]);
            }
          else
            {
              mean = -1;
              rms = -1;
              if (istrip < nstrips)
                {
                  printf("  Layer %2d Strip %2d has threshold = %6.2f and noise = %6.2f\n",
                         ilayer + 1, istrip + 1,            mean,             rms);
                  printf("par0 %.2f, par1 %.2f, par2 %.2f, chisq %.2f \n",
                         par[0],    par[1],    par[2],    chisq);
                }
            }

          hid = STRIP_HIST_OFFSET + (scan_number+1)*10000 + 300;
          i_ch++;
          HFILL(hid+4, (float)(i_ch) , 0.0, mean);
          HFILL(hid+5, mean , 0.0, 1.);
          HFILL(hid+6, (float)(i_ch) , 0.0, rms);
          HFILL(hid+7, rms , 0.0, 1.);

          thr[scan_number][ilayer][istrip] = mean;
          noise[scan_number][ilayer][istrip] = rms;
        }
    }

  return 0;

}

/*------------------------------ init_hist_19 ---------------------------------*/
int init_hist_19()
{
  int i, iboard, tmp_hist_offset;
  int hid, ilayer, ichip,istrip;
  char htitle[80];

  /* Book histograms for CSC wires */
  for (i=0; i<NMAXSCAN; i++)
    {
      tmp_hist_offset = STRIP_HIST_OFFSET + (i+1) * 10000;
      for (ilayer = 1; ilayer <= NLAYER; ilayer++)
		{
          hid = tmp_hist_offset + ilayer ;
          sprintf( htitle, "CLCT Strip occupancy, Layer %d Scan %d", ilayer, i+1);
          HBOOK1( hid, htitle, 80, 0.5, 80.5, 0.0);
		}

      /* Book cathode front-end board (CFEB) histograms */

      for (ilayer = 1; ilayer <= NLAYER; ilayer++)

		{
          for (iboard = 1; iboard <= NCFEB; iboard++)
			{
              ichip = NCFEB * (ilayer-1) + iboard;
              hid = tmp_hist_offset + 100 + ichip;
              sprintf( htitle, "CLCT  times  Layer %d Board %d", ilayer, iboard);
              HBOOK1( hid, htitle, 32, -0.5, 31.5, 0.0);

              hid = tmp_hist_offset + 200 + ichip;
              sprintf( htitle, "Strip cal.pulse: Layer %d Board %d", ilayer, iboard);
              HBOOK1( hid, htitle, 256, -0.5, 255.5, 0.0);
			}
		}

      for (ilayer = 0; ilayer < NLAYER; ilayer++)
		{
          for (istrip = 0; istrip < NSTRIP; istrip++)
			{
              hid = tmp_hist_offset +1000+ 100*(ilayer+1) +  istrip+1;
              sprintf( htitle, "CLCT time. Layer %d Strip %d", ilayer+1, istrip+1);
              HBOOK1( hid, htitle, 32, 0.5, 32.5, 0.0);

              hid = tmp_hist_offset + 2000 + 100 * (ilayer+1) + istrip + 1;
              sprintf( htitle, "Strip cal.stat.: Layer %d Strip %d ",
                       ilayer+1, istrip+1 );
              HBOOK1( hid, htitle, 128, 0.5, 128.5, 0.0);
			}
		}
    }
  return 0;
}
