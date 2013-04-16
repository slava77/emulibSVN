/*
 * test_14.c (in V1.2 test_09.c)
 * ALCT Time Delay Calibration
 *
 * Author : Lisa Gorn
 * Date   : Dec 6, 2000
 * Last update: Jun 05, 2002
 * Try to introduce total delay :Aug 22, 2002 . SD
 */

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <math.h>
#include <time.h>
#include "cfortran.h"
#include "hbook.h"
#include "daq_conf.h"
#include "csc_event.h"       /* upevt_                 */
#include "csc_parameters.h"  /* NLAYER, NSTRIP, etc... */
#include "one_page_plot.h"   /* STRIP_PLOT,...         */
#include "test_utils.h"      /* chamber labels         */
#include "test_params.h"     /* test config parameters */
#include "test_14_ini.h"
#include "test_config_struct.h"

//#include "csc_unpacker.h"
//#include "emu/daq/reader/RawDataFile.h"

#define  LOWER_TAVG_LIMIT     6
#define  UPPER_TAVG_LIMIT    10
#define  LOWER_TAVG_LIMIT_2   0
#define  UPPER_TAVG_LIMIT_2  16

#define  MAX_NDELAYS         16
#define  JTAG_DELAY_SCAN      2

/* Measured offsets (ns) of test pulse arrival time at AFEB input connector */
//#define  CABLE_DELAY_LCT1       11.2         // 10.6
//#define  CABLE_DELAY_LCT2        8.2         //  6.0
//#define  CABLE_DELAY_LCT3        3.2         //  1.5
//#define  CABLE_DELAY_LCT4          0
/*
float cable_del[6][7] = // 6 is the number of different chamber types, 7 is max AFEB group number
{
    {0, 0, 0, 0, 0, 0, 0},
    {11.2, 8.2, 3.2, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {1.9, 0, 0, 0, 0, .3, 2.5},
    {0, 0, 0, 0, 0, 0, 0},
    {11.2, 8.2, 3.2, 0, 0, 0, 0}
};
*/

float cable_del[7][14] = // 6 is the number of different chamber types, 14 is max AFEB group number
{
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	  // Dummy
  {3.0, 3.0, 1.6, 1.6, 0, 0, 0, 1.6, 0, 0, 0, 0, 0, 0}, // ME1.2
  {4.6, 1.6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	  // ME1.3
  {1.4, 1.4, 1.4, 1.4, 0, 0, 0, 0, 0, 0, 0, 0, 1.4, 1.4},	  // M2.1
  {1.4, 1.4, 0, 0, 0, 0, 0, 0, 0, 0, 1.4, 1.4, 0, 0},	  // ME3.1
  {10.7, 10.7, 7.7, 7.7, 3.0, 3.0, 0, 0, 0, 0, 0, 0, 0, 0}, // ME234.2
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1.4, 1.4, 1.4, 0, 0}  // ME4.1
};


// #define  DELAY_OFFSET              0  // this offset is using only for plots

/* Local function declarations */
int test_14_begin(int pass);
int test_14_event(int pass);
int test_14_end(int pass);
int test_14_finish(void);
int analyze_delay_hists(int idelay);
void book_histos_14f_(int *nwires);

/* Global variables local to this test */
static float   tavg[NLAYER][NWIRE][MAX_NDELAYS];
static float   terr[NLAYER][NWIRE][MAX_NDELAYS];

static float   delay[MAX_NDELAYS];
static int     current_delay, ndelays, nbadch = 0;
static FILE    *fp;

char test_14_file_el[]="/test_results/temp/test_14_01.results";
char test_14_file_his_el[]="/test_results/temp/test_14_01.his";

int nwires, nchips;

extern int tot_evt;
extern test_config_struct tcs;

/******************************************************************************
   Define functions and parameters for FAST site test 14
******************************************************************************/

int test_14_init(void)
{
  test_begin  = test_14_begin;
  test_event  = test_14_event;
  test_end    = test_14_end;
  test_finish = test_14_finish;

  num_passes = 1;
  num_events[0] = 32000;

  switch (csc_type)
  {
  case 0:
    nwires = 48;
    break;
  case 1:
    nwires = 64;
    break;
  case 2:
    nwires = 32;
    break;
  case 3:
    nwires = 112;
    break;
  case 4:
    nwires = 96;
    break;
  case 5:
    nwires = 64;
    break;
  case 6:
    nwires = 96;
    break;
  default:
    nwires = 64;
    break;
  }
  nchips = nwires*3/8;

  printf ("csc_type = %d, nwires = %d\n", csc_type, nwires);
  return 0;
}

/******************************************************************************
   Book histograms for FAST site test 14, and do other initialization
******************************************************************************/

int test_14_begin(int pass)
{
  int     hid, ilayer, iwg;
  char    htitle[80];
  char *filename;

  /* Delete old histograms and book new ones which will show fit input */
    HDELET(0);

  memset(tavg, 0, sizeof(tavg));
  memset(terr, 0, sizeof(terr));
  if (pass == 0)
  {
    filename = malloc(strlen(datadir_translation) +
                      strlen(test_14_file_el) + 1 );
    if (filename == NULL)
    {
      printf("malloc() failed on filename.\n");
      return 1;
    }
    sprintf(filename,"%s%s",datadir_translation,test_14_file_el);

    /* Open result file in the temporary directory */
    fp = fopen(filename, "w");
    if (fp == NULL)
    {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
    }
    free(filename);
    fprintf(fp, "--------------------------------------------------------"
            "-------\n");
    fprintf(fp, "Results of test 14 (ALCT Time Delay Calibration) for %s\n",
            csc_name);
    fprintf(fp, "--------------------------------------------------------"
            "-------\n\n");

    printf("Booking pass 0 histograms for test 14\n");

    HBOOK1( 1, "Wiregroup average times", 100, 0., 15., 0.);

    HBOOK1( 2, "Fraction of events in peak or next-largest timebin",
            105, 0., 1.05, 0.);

    for (ilayer=0; ilayer < NLAYER; ilayer++)
    {
      for (iwg=0; iwg < nwires; iwg++)
      {
        hid = 120 * (ilayer + 1) +  (iwg + 1);
        sprintf( htitle, "Layer %d Wiregroup %d", ilayer+1, iwg+1);
        HBOOK1( hid, htitle, 16, -0.5, 15.5, 0.);
      }
    }

    ndelays = -1;
    current_delay = -1;
  }
  return 0;
}

/******************************************************************************
   Do analysis and fill histograms for FAST site test 14
******************************************************************************/

int test_14_event(int pass)
{
  int          disc_on, hid, ilayer, iwg, tbin;
  int          ifeb,  got_an_feb = 0;
  int activetbins;
  static int    event_number = 0;
  // total events
  tot_evt = tcs.t14.events_per_delay *  tcs.t14.delays_per_run;

  // simulate legacy alctcal module by assigning the params below
  upevt_.alctcal_scan_mode            = JTAG_DELAY_SCAN;
  // current delay
  upevt_.alctcal_current_value        = tcs.t14.delay_first + (event_number/tcs.t14.events_per_delay) * tcs.t14.delay_step;
  upevt_.alctcal_current_threshold    = 0; // not used in 14
  upevt_.alctcal_trigger_per_step     = 0;
  upevt_.alctcal_num_points           = 0;
  upevt_.alctcal_num_points_2nd_param = 0;


  if (upevt_.alctcal_scan_mode != JTAG_DELAY_SCAN)
    printf("### Error: data was taken with the wrong ALCT scan mode ###\n");

  /* Check to see if we have an empty event */
  //uses datamember from ALCT_header2006 - now in use ALCT_Header2007
  //is per default 0
  //for (ifeb=0; ifeb < NCFEB; ifeb++)
  /*if (upevt_.alct_febs_read > 0) got_an_feb = 1;
  if (! got_an_feb) {
  printf("No AFEB data in event %ld\n", upevt_.event_number);
  return 0;
  }
  */

  /* Check to see if this event has a new delay */
  if (upevt_.alctcal_current_value != current_delay) {
    if (current_delay >= 0) {
      analyze_delay_hists(ndelays);
      HRESET(0," ");
    }
    ndelays++;
    current_delay = upevt_.alctcal_current_value;
    delay[ndelays] = (float)current_delay;
    printf("current delay %d, delay[%d] %f\n", current_delay,ndelays,delay[ndelays]);
  }


  /* Fill timebin histograms. */
  for (ilayer = 0; ilayer < NLAYER; ilayer++)
  {
    for (iwg = 0; iwg < nwires; iwg++)
    {
      hid = 120*(ilayer + 1) + (iwg + 1);
      if (upevt_.alct_dump[ilayer][iwg])
      {
        disc_on = 0;
        activetbins=0;
        for (tbin = 0; tbin < upevt_.alct_nbucket; tbin++)
        {
          if (upevt_.alct_dump[ilayer][iwg] & (1 << tbin))
          {
            activetbins++;
            if (!disc_on && (activetbins<ACTIVETBINS_LIMIT))
            {
              HFILL(hid, (float)tbin, 0.0, 1.0);
              disc_on = 1;
            }
          }
          else disc_on = 0;
        }
      }
    }
  }
  event_number++;
  return 0;

}


/******************************************************************************
   End-of-run analysis for FAST site test 14
******************************************************************************/

#define NCHIP    NAFEB * NAFEB_CHIP
#define SLOPE_LOWER_LIMIT       1.5
#define SLOPE_UPPER_LIMIT       2.5
#define CHISQ_UPPER_LIMIT      80.0

int test_14_end(int pass) {
  int      n, chip, hid, ilayer, iwg, idelay[nchips], ngood[nchips];
  float    limits[7], intercept_lower_limit, intercept_upper_limit;
  float    bmax, btemp, cable_delay[nwires], mtemp, y;
  float    avg, avg_slope, d[nchips], b_avg[nchips], m_avg[nchips];
  float    b[NLAYER][nwires], m[NLAYER][nwires], chisq[NLAYER][nwires];
//  char     csc_name[15];

  if (pass == 0) {
    printf("Number of buckets read = %d\n", upevt_.alct_nbucket);
    /*
        for (iwg =  0; iwg < 16; iwg++) cable_delay[iwg] = CABLE_DELAY_LCT1 + DELAY_OFFSET ;
        for (iwg = 16; iwg < 32; iwg++) cable_delay[iwg] = CABLE_DELAY_LCT2 + DELAY_OFFSET;
        for (iwg = 32; iwg < 48; iwg++) cable_delay[iwg] = CABLE_DELAY_LCT3 + DELAY_OFFSET;
        for (iwg = 48; iwg < 64; iwg++) cable_delay[iwg] = CABLE_DELAY_LCT4 + DELAY_OFFSET;
    */
    printf("CSC type: %d\n", csc_type);
    for (iwg =  0; iwg < nwires; iwg++)
    {
//	cable_delay[iwg] = cable_del[csc_type][iwg/16] + DELAY_OFFSET ;
      cable_delay[iwg] = cable_del[csc_type][iwg/8] + DELAY_OFFSET ;

    }


    analyze_delay_hists(ndelays);
    ndelays++;
    if (ndelays > MAX_NDELAYS) ndelays = MAX_NDELAYS;

    /* Delete old histograms and book new ones which will show fit input */
    HDELET(0);
    book_histos_14f_(&nwires);    /* histos in the range 100-664   */
    HBOOK1(1, "Equalized delays", 100, -5., 5., 0.);

    /* Do the fit */
    for (ilayer = 0; ilayer < NLAYER; ilayer++) {
      for (iwg = 0; iwg < nwires; iwg++) {
        fitit(delay, &tavg[ilayer][iwg][0], &terr[ilayer][iwg][0], ndelays,
              &mtemp, &btemp, &chisq[ilayer][iwg]);
        // printf("%d %d - tavg %.3f terr %.3f chisq %.3f btemp %.3f mtemp %.3f ndelays %d\n", ilayer, iwg, tavg[ilayer][iwg][0], terr[ilayer][iwg][0], chisq[ilayer][iwg], btemp, mtemp, ndelays);
        hid = 120 * (ilayer + 1) +  (iwg + 1);
        HPAK(hid, tavg[ilayer][iwg]);
        HPAKE(hid, terr[ilayer][iwg]);

        /* Convert time measurements from timebins to ns (25 ns / timebin), and correct
         * for the delay of the test pulse arrival at the AFEB.
         */
        b[ilayer][iwg] = (btemp * 25.) - cable_delay[iwg];
        m[ilayer][iwg] = mtemp * 25.;
        // printf("%d %d - b %.3f, m %.3f\n", ilayer, iwg, b[ilayer][iwg], m[ilayer][iwg]);
      }
    }

    /* Find the overall average intercept (to make the results independent of
     * the exact timing of the test pulse).
     */
    for (ilayer = 0, avg = 0, n = 0; ilayer < NLAYER; ilayer++) {
      for (iwg = 0; iwg < nwires; iwg++) {
        if (m[ilayer][iwg] >= SLOPE_LOWER_LIMIT &&
            m[ilayer][iwg] <= SLOPE_UPPER_LIMIT) {
          avg += b[ilayer][iwg];
          n++;
        }
      }
    }

    if (n > 0) avg = avg / n;
    else avg = b[0][0];

    intercept_lower_limit = avg - 15;
    intercept_upper_limit = avg + 15;

    /* Average the slopes and intercepts (for good channels) over the 16 channels
     * of each chip. Each 16 channels are from 8 adjacent wire groups x 2 layers.
     */
    for (chip = 0; chip < nchips; chip++) {
      b_avg[chip] = 0;
      m_avg[chip] = 0;
      ngood[chip] = 0;
    }

    for (ilayer = 0; ilayer < NLAYER; ilayer++) {
      for (iwg = 0; iwg < nwires; iwg++) {
        chip = 3 * (iwg / 8) + (ilayer / 2);
        if (b[ilayer][iwg] >= intercept_lower_limit &&
            b[ilayer][iwg] <= intercept_upper_limit &&
            m[ilayer][iwg] >= SLOPE_LOWER_LIMIT &&
            m[ilayer][iwg] <= SLOPE_UPPER_LIMIT) {
          b_avg[chip] += b[ilayer][iwg];
          m_avg[chip] += m[ilayer][iwg];
          ngood[chip]++;
        }
        else
          printf("BAD: Layer %d WG %d chip %d - b %.3f m %.3f\n", ilayer, iwg, chip, b[ilayer][iwg], m[ilayer][iwg] );
      }
    }

    bmax = -1;
    for (chip = 0; chip < nchips; chip++) {
      if (ngood[chip] > 0) {
        b_avg[chip] = b_avg[chip] / ngood[chip];
        m_avg[chip] = m_avg[chip] / ngood[chip];
        if (b_avg[chip] > bmax) bmax = b_avg[chip];
      }
    }

    /* Find the setting for each chip which gives this delay. */
    for (chip = 0; chip < nchips; chip++) {
      if (ngood[chip] > 0) d[chip] = (bmax - b_avg[chip]) / m_avg[chip];
      else d[chip] = -1;
    }

    /* Find the average slope and delay offset of all the chips */
    for (chip = 0, avg = 0, avg_slope = 0, n = 0; chip < nchips; chip++) {
      if (ngood[chip] >= 0) {
        avg_slope += m_avg[chip];
        avg += d[chip] - rint(d[chip]);
        n++;
      }
    }
    if (n > 0) {
      avg_slope = avg_slope / n;
      avg = avg / n;
    }
    if (avg < 0) avg = avg + 1;
    avg = avg * avg_slope;

    /* Find the setting for each chip which gives the maximum delay plus the
     * average offset (this avoid the situation where half the chips have
     * delay 1.4 -> 1 and the other half have delay 1.6 -> 2).
     */
    for (chip = 0; chip < nchips; chip++) {
      if (ngood[chip] > 0) d[chip] = ((bmax + avg) - b_avg[chip])
                                       / m_avg[chip];
      else d[chip] = -1;

      printf("chip%d bmax:%.f avg:%.f b_avg:%.f m_avg:%.f dchip:%.f\n", chip, bmax, avg, b_avg[chip], m_avg[chip], d[chip]);
      idelay[chip] = rint(d[chip]);
    }




    /* Print a table of results */
    fprintf(fp, " chip   ngood   slope   intercept   delay   "
            "equalized delay\n");
    for (chip = 0; chip < nchips; chip++) {
      y = m_avg[chip] * idelay[chip] + b_avg[chip];
      fprintf(fp, "  %2d     %2d     %5.2f    %6.2f      %2d         %6.2f"
              "\n", chip+1, ngood[chip], m_avg[chip], b_avg[chip], idelay[chip], y);
      HFILL(1, y - (bmax + avg), 0., 1.);
    }
    fprintf(fp, "\n");

    /* Plot results */
    limits[0] =    0;                     /* layer histo lower limit */
    limits[1] =   intercept_upper_limit + 5; //100;                     /* layer histo upper limit */
    limits[2] =    0;                     /* summary histo lower limit */
    limits[3] =   intercept_upper_limit + 5; //100;                     /* summary histo upper limit */
    limits[4] =   intercept_lower_limit;  /* lower limit line */
    limits[5] =   intercept_upper_limit;  /* upper limit line */
    limits[6] = nwires;

    one_page_plot_v3(14, 1, WIRE_PLOT, (float *)b, (float *)b, limits, csc_name,
                     "Intercept", "intercept, ns", 0);

    limits[0] =  0;                       /* layer histo lower limit */
    limits[1] =  4;                       /* layer histo upper limit */
    limits[2] =  0;                       /* summary histo lower limit */
    limits[3] =  4;                       /* summary histo upper limit */
    limits[4] =  SLOPE_LOWER_LIMIT;       /* lower limit line */
    limits[5] =  SLOPE_UPPER_LIMIT;       /* upper limit line */
    limits[6] = nwires;

    one_page_plot_v3(14, 2, WIRE_PLOT, (float *)m, (float *)m, limits, csc_name,
                     "Slope", "slope, ns/bit", 0);

    limits[0] =   0;                      /* layer histo lower limit */
    limits[1] = 100;                      /* layer histo upper limit */
    limits[2] =   0;                      /* summary histo lower limit */
    limits[3] = 100;                      /* summary histo upper limit */
    limits[4] =   0.;                     /* lower limit line */
    limits[5] =  CHISQ_UPPER_LIMIT;       /* upper limit line */
    limits[6] = nwires;

    one_page_plot_v3(14, 3, WIRE_PLOT, (float *)chisq, (float *)chisq, limits,
                     csc_name, "Chi-squared of fit", "chisq", 0);

    /* Count bad channels */
    fprintf(fp, "Channels with bad fit chisq (> %4.1f):\n", CHISQ_UPPER_LIMIT);
    for (ilayer = 0; ilayer < NLAYER; ilayer++) {
      for (iwg = 0; iwg < nwires; iwg++) {
        chip = 3 * (iwg / 8) + (ilayer / 2);
        if ((b[ilayer][iwg] < intercept_lower_limit) ||
            (b[ilayer][iwg] > intercept_upper_limit) ||
            (m[ilayer][iwg] < SLOPE_LOWER_LIMIT) ||
            (m[ilayer][iwg] > SLOPE_UPPER_LIMIT) ||
            (chisq[ilayer][iwg] > CHISQ_UPPER_LIMIT)) {
          nbadch++;
          if (chisq[ilayer][iwg] > CHISQ_UPPER_LIMIT)
            fprintf(fp, "Layer %d wiregroup %2d (chip %2d) has chisq = %10.3e\n",
                    ilayer+1, iwg+1, chip+1, chisq[ilayer][iwg]);
        }
      }
    }
    fprintf(fp, "\nTotal number of bad channels = %d\n\n", nbadch);
  }

  return 0;
}

/******************************************************************************
   End-of-test analysis for FAST site test 14
******************************************************************************/

int test_14_finish(void)
{
  time_t t;
  char   timestring[40];
  char* file_his_name;


  /* Save histograms */
  file_his_name = malloc(strlen(datadir_translation) +
                         strlen(test_14_file_his_el)+1);
  if (file_his_name == NULL)
  {
    printf("malloc() failed for file_his_name.  Could not save histograms.\n");
  }
  else
  {
    sprintf(file_his_name,"%s%s",
            datadir_translation,test_14_file_his_el);
    printf ("===> %s\n", file_his_name);
    printf("Saving histograms\n");
    HRPUT(0, file_his_name , "N");
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

/*******************************************************************************
   Analysis done for each delay value
*******************************************************************************/

int analyze_delay_hists(int idelay)
{
  int      ilayer, iwg, hid, t, max_t, nbad, nmax_t;
  int      noent;
  float    max, sum, w_sum, e_sum, total, x, harray[32];
  int 	 active_tbins;
  int	   t_start, t_end;

  if (idelay < 0 || idelay > 15) {
    printf("Error: idelay = %d\n", idelay);
    return -1;
  }

  nbad = 0;
  memset(harray, 0, sizeof (harray));

  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    // printf("ndelays at start%d\n ",ndelays);
    for (iwg = 0; iwg < nwires; iwg++) {
      hid = 120*(ilayer + 1) + (iwg + 1);
      HNOENT(hid, noent);
      HUNPAK(hid, harray, "HIST", 0);
      max = -1;
      max_t = 0;
      sum = w_sum = e_sum = 0;
      active_tbins = 0;
      total = 0;

      // printf ("dly%2d ly%d wg%2d: ", idelay, ilayer+1, iwg+1);
      for (t = 0; t < upevt_.alct_nbucket; t++) {
        // printf (" %3d", (int)harray[t]);
        if (harray[t] > 1.) active_tbins++;
        if (harray[t] > max) {
          max = harray[t];
          max_t = t;
        }
      }
      // printf("\n");


      ///* Find start and end timebins with data
      t_start = -1;
      t_end = -1;
      for (t = ((max_t>=2)?(max_t-2):0); t < ((max_t<(upevt_.alct_nbucket-2))?(max_t+2):upevt_.alct_nbucket); t++)
      {
        if  (harray[t] > 1.) {
          t_end = t;
          if (t_start < 0) t_start = t;
        }

      }

      ///* Warn and skip empty
      if (t_start < 0 || t_end < 0) {
        printf("WARNING: Delay setting %d -  No Entries in Layer %d, Wiregroup %d", idelay, ilayer+1, iwg+1 );
        nbad++;
        continue;
      }

      ///* Warn if too wide time spread is detected
      if (t_end - t_start > 4)
      {
        printf("WARNING: Delay setting %d - Too many active timebins (%d) Layer %d, Wiregroup %d\n",
               idelay, t_end - t_start, ilayer+1, iwg+1 );
      }


      for (t = t_start; t <= t_end; t++)
      {
        sum += harray[t];
        w_sum += harray[t]*(t);
        e_sum += harray[t]*(t)*(t);

      }

      if ( sum > 0 )
      {
        tavg[ilayer][iwg][idelay] = w_sum/sum;
        HFILL(1, tavg[ilayer][iwg][idelay], 0.0, 1.0);
        for (t=0, total=0; t < upevt_.alct_nbucket; t++) total += harray[t];
        if ((x = sum / total) < 0.95)
          //printf("Too wide timebin histogram for Layer %d, Wiregroup %d: "
          //"%f = %f / %f\n", ilayer+1, iwg+1, x, sum, total);
        if ((x = sum / total) > 1.00)
            //printf("??? Wrong ratio for Layer %d, Wiregroup %d: "
            //"%f = %f / %f\n", ilayer+1, iwg+1, x, sum, total);
            HFILL(2, x, 0.0, 1.0);
        terr[ilayer][iwg][idelay]
        = sqrt( (e_sum / sum) - tavg[ilayer][iwg][idelay]*tavg[ilayer][iwg][idelay]) / sqrt(sum);
        if (terr[ilayer][iwg][idelay] <= 0) terr[ilayer][iwg][idelay] = 0.001;

      }
      else
      {
        printf("No histogram entries for Layer %d, Wiregroup %d\n",
               ilayer+1, iwg+1);
        tavg[ilayer][iwg][idelay] = -1;
        terr[ilayer][iwg][idelay] = 1;
        nbad++;
      }

      /*
      for (t = 0; t < upevt_.alct_nbucket; t++) {
      	if (harray[t] > max) {
      		max = harray[t];
      		max_t = t;
      	}
      }

      if (max_t == 0) nmax_t = 1;
      else if (max_t == upevt_.alct_nbucket-1) nmax_t = upevt_.alct_nbucket-2;
      else if (harray[max_t-1] > harray[max_t+1]) nmax_t = max_t - 1;
      else nmax_t = max_t + 1;

      sum = harray[max_t] + harray[nmax_t];
      if (sum > 0) {
      	tavg[ilayer][iwg][idelay] = (max_t * harray[max_t] +
      								 nmax_t * harray[nmax_t]) / sum;
      	HFILL(1, tavg[ilayer][iwg][idelay], 0.0, 1.0);
      	for (t=0, total=0; t < upevt_.alct_nbucket; t++) total += harray[t];
      	if ((x = sum / total) < 0.95)
      	  //printf("Too wide timebin histogram for Layer %d, Wiregroup %d: "
      	  //"%f = %f / %f\n", ilayer+1, iwg+1, x, sum, total);
      	if ((x = sum / total) > 1.00)
      	  //printf("??? Wrong ratio for Layer %d, Wiregroup %d: "
      	  //"%f = %f / %f\n", ilayer+1, iwg+1, x, sum, total);
      	HFILL(2, x, 0.0, 1.0);

      	terr[ilayer][iwg][idelay] = sqrt(harray[max_t] * harray[nmax_t] / sum)
      		/ sum;
      	// =VB= This trick to make fit procedure happy
      	if (terr[ilayer][iwg][idelay] <= 0) terr[ilayer][iwg][idelay] = 0.001;
      //  printf("%d %d %d -  tavg %.3f terr %.3f %d %d %.3f %.3f\n", ilayer, iwg, idelay, tavg[ilayer][iwg][idelay], terr[ilayer][iwg][idelay], max_t, nmax_t, harray[max_t], harray[nmax_t]);
      }
      else {
      	printf("No histogram entries for Layer %d, Wiregroup %d\n",
      		   ilayer+1, iwg+1);
      	tavg[ilayer][iwg][idelay] = -1;
      	terr[ilayer][iwg][idelay] = 1;
      	nbad++;
      }

      */
    }
  }

  printf("Found %d bad channels for delay point %d\n", nbad, idelay);
  return 0;
}

