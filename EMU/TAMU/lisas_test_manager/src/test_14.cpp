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
#include "daq_conf.h"
#include "csc_event.h"       /* upevt_                 */
#include "csc_parameters.h"  /* NLAYER, NSTRIP, etc... */
#include "one_page_plot.h"   /* STRIP_PLOT,...         */
#include "test_utils.h"      /* chamber labels         */
#include "test_params_cxx.h" /* test config parameters */
#include "test_14_ini.h"
#include "test_config_struct.h"


// root headers
#include "TF1.h" // unary functions
#include "TH1.h" // histograms
#include "TProfile.h" // profile histograms
#include "TRandom.h" // pseudo-random number geneartor
#include "TMath.h" // the error function
#include "TFile.h" // writing files
#include <vector>
#include <fstream> // ofstream
#include <sstream> // ostringstrea
#include <cstdarg> // variable argument functions
#include <iomanip> // setw, setprecision, and scientific
#include <algorithm> // for for_each

#include "one-page-plot.h"
#include "basic-utilities.h"

#define  LOWER_TAVG_LIMIT     6
#define  UPPER_TAVG_LIMIT    10
#define  LOWER_TAVG_LIMIT_2   0
#define  UPPER_TAVG_LIMIT_2  16

#define  MAX_NDELAYS         16
#define  JTAG_DELAY_SCAN      2

using namespace std;

float cable_del[7][14] = // 7 is the number of different chamber types, 14 is max AFEB group number
  {
    {3.0, 3.0, 1.6, 1.6, 0, 0, 0, 1.6, 0, 0, 0, 0, 0, 0},   // ME1.1
    {3.0, 3.0, 1.6, 1.6, 0, 0, 0, 1.6, 0, 0, 0, 0, 0, 0}, // ME1.2
    {4.6, 1.6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},         // ME1.3
    {1.4, 1.4, 1.4, 1.4, 0, 0, 0, 0, 0, 0, 0, 0, 1.4, 1.4},         // M2.1
    {1.4, 1.4, 0, 0, 0, 0, 0, 0, 0, 0, 1.4, 1.4, 0, 0},     // ME3.1
    {10.7, 10.7, 7.7, 7.7, 3.0, 3.0, 0, 0, 0, 0, 0, 0, 0, 0}, // ME234.2
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1.4, 1.4, 1.4, 0, 0}  // ME4.1
  };


/* Local function declarations */
int test_14_begin(int pass);
int test_14_event(int pass);
int test_14_end(int pass);
int test_14_finish(void);
int analyze_delay_hists(int idelay);

// new root stuff
void reset_all_test_14_regular_histograms();
void delete_all_test_14_regular_histograms();
void delete_all_test_14_profile_histograms();
template <typename T>
void delete_all_histograms_in_matrix(vector < vector< T * > > * histograms);
vector< vector< TProfile * > > * create_profile_histograms(int);

/* Global variables local to this test */
static float   tavg[NLAYER][NWIRE][MAX_NDELAYS];
static float   terr[NLAYER][NWIRE][MAX_NDELAYS];

static float   delay[MAX_NDELAYS];
static int     current_delay, ndelays, nbadch = 0;

char test_14_file_el[]="/test_results/temp/test_14_01.results";
char test_14_file_his_el[]="/test_results/temp/test_14_01.his";

int nwires, nchips;

// new root stuff
vector < vector < TH1F * > > * regular_histograms;
vector < vector < TProfile * > > * profile_histograms;
TH1F * wiregroup_average_times;
TH1F * fraction_of_events_in_peak;
TH1F * equalized_delays;

extern int tot_evt;
extern test_config_struct tcs;

/******************************************************************************
   Define functions and parameters for FAST site test 14
******************************************************************************/

extern "C" int test_14_init(void)
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

char * char_star_append(char * x, char * y) {
  char * appended_string = new char[strlen(x) + strlen(y) + 1];
  if (appended_string == NULL) {
    return NULL;
  }
  strcpy(appended_string, x);
  strcat(appended_string, y);
  return appended_string;
}

int test_14_begin(int pass)
{
  memset(tavg, 0, sizeof(tavg));
  memset(terr, 0, sizeof(terr));

  if (pass == 0) {
    ostringstream filename;
    filename << datadir_translation << test_14_file_el;

    ofstream output_file(filename.str().c_str());

    output_file << "---------------------------------------------------------------" << endl
                << "Results of test 14 (ALCT Time Delay Calibration) for " << csc_name << endl
                << "---------------------------------------------------------------" << endl << endl;

    printf("Booking pass 0 histograms for test 14\n");

    wiregroup_average_times = new TH1F("wiregroup_average_times",
                                       "Wiregroup average times",
                                       100,
                                       0.,
                                       15.);

    fraction_of_events_in_peak = new TH1F("fraction_of_events_in_peak",
                                          "Fraction of events in peak or next-largest timebin",
                                          105,
                                          0.,
                                          1.05);

    regular_histograms = new vector< vector< TH1F * > >();

    for (int ilayer=0; ilayer < NLAYER; ilayer++) {
      vector<TH1F*> * row_of_histograms = new vector<TH1F*>();

      for (int iwg=0; iwg < nwires; iwg++) {
        ostringstream histogram_name;
        ostringstream histogram_title;
        histogram_name << "histogram_" << ilayer << "_" << iwg;
        histogram_title << "Layer " << ilayer + 1 << " Wiregroup " << iwg + 1;

        TH1F * current_histogram  = new TH1F(histogram_name.str().c_str(),
                                             histogram_title.str().c_str(),
                                             16,
                                             -0.5,
                                             15.5);

        row_of_histograms->push_back(current_histogram);
      }
      regular_histograms->push_back(*row_of_histograms);
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

  /* Check to see if this event has a new delay */
  if (upevt_.alctcal_current_value != current_delay) {
    if (current_delay >= 0) {
      analyze_delay_hists(ndelays);
      reset_all_test_14_regular_histograms();
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
                          (*regular_histograms)[ilayer][iwg]->Fill(tbin, 1.0);
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
    ostringstream filename;
    filename << datadir_translation << test_14_file_el;

    ofstream output_file(filename.str().c_str());

    printf("Number of buckets read = %d\n", upevt_.alct_nbucket);
    printf("CSC type: %d\n", csc_type);
    for (iwg =  0; iwg < nwires; iwg++)
      {
        cable_delay[iwg] = cable_del[csc_type][iwg/8] + DELAY_OFFSET ;

      }


    analyze_delay_hists(ndelays);
    ndelays++;
    if (ndelays > MAX_NDELAYS) ndelays = MAX_NDELAYS;

    profile_histograms = create_profile_histograms(nwires);
    equalized_delays = new TH1F("equalized_delays", "Equalized delays", 100, -5., 5.);

    /* Do the fit */
    for (ilayer = 0; ilayer < NLAYER; ilayer++) {
      for (iwg = 0; iwg < nwires; iwg++) {
        fitit(delay, &tavg[ilayer][iwg][0], &terr[ilayer][iwg][0], ndelays,
              &mtemp, &btemp, &chisq[ilayer][iwg]);
        hid = 120 * (ilayer + 1) +  (iwg + 1);
        for (int delay = 0; delay < MAX_NDELAYS; delay++) {
          (*profile_histograms)[ilayer][iwg]->SetBinContent(delay, tavg[ilayer][iwg][delay]);
          (*profile_histograms)[ilayer][iwg]->SetBinError(delay, terr[ilayer][iwg][delay]);
        }

        /* Convert time measurements from timebins to ns (25 ns / timebin), and correct
         * for the delay of the test pulse arrival at the AFEB.
         */
        b[ilayer][iwg] = (btemp * 25.) - cable_delay[iwg];
        m[ilayer][iwg] = mtemp * 25.;
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
    output_file << " chip   ngood   slope   intercept   delay equalized delay" << endl;
    for (chip = 0; chip < nchips; chip++) {
      y = m_avg[chip] * idelay[chip] + b_avg[chip];
      output_file << "  "        << setw(2)                    << chip + 1
                  << "     "     << setw(2)                    << ngood[chip]
                  << "     "     << setw(5) << setprecision(2) << m_avg[chip]
                  << "     "     << setw(6) << setprecision(2) << b_avg[chip]
                  << "      "    << setw(2)                    << idelay[chip]
                  << "         " << setw(6) << setprecision(2) << y
                  << endl;

      equalized_delays->Fill(y - (bmax + avg), 1.);
    }
    output_file << endl;

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
    one_page_plot(14, "AFEB-ALCT Time Delays", "subtest", limits[0], limits[1], "Intercept (ns)",
                  csc_name, limits[4], limits[5], nwires, 6, 6, twod_array_to_vectors((float *)b, NLAYER, nwires),
		  "test-14-01.pdf");

    limits[0] =  0;                       /* layer histo lower limit */
    limits[1] =  4;                       /* layer histo upper limit */
    limits[2] =  0;                       /* summary histo lower limit */
    limits[3] =  4;                       /* summary histo upper limit */
    limits[4] =  SLOPE_LOWER_LIMIT;       /* lower limit line */
    limits[5] =  SLOPE_UPPER_LIMIT;       /* upper limit line */
    limits[6] = nwires;

    one_page_plot_v3(14, 2, WIRE_PLOT, (float *)m, (float *)m, limits, csc_name,
                     "Slope", "slope, ns/bit", 0);
    one_page_plot(14, "AFEB-ALCT Time Delays", "subtest", limits[0], limits[1], "Slope (ns/bit)",
                  csc_name, limits[4], limits[5], nwires, 6, 6, twod_array_to_vectors((float *)m, NLAYER, nwires),
		  "test-14-02.pdf");

    limits[0] =   0;                      /* layer histo lower limit */
    limits[1] = 100;                      /* layer histo upper limit */
    limits[2] =   0;                      /* summary histo lower limit */
    limits[3] = 100;                      /* summary histo upper limit */
    limits[4] =   0.;                     /* lower limit line */
    limits[5] =  CHISQ_UPPER_LIMIT;       /* upper limit line */
    limits[6] = nwires;

    one_page_plot_v3(14, 3, WIRE_PLOT, (float *)chisq, (float *)chisq, limits,
                     csc_name, "Chi-squared of fit", "chisq", 0);
    one_page_plot(14, "AFEB-ALCT Time Delays", "subtest", limits[0], limits[1], "Chi-squared of fit",
                  csc_name, limits[4], limits[5], nwires, 6, 6, twod_array_to_vectors((float *)chisq, NLAYER, nwires),
		  "test-14-03.pdf");

    /* Count bad channels */
    output_file << "Channels with bad fit chisq (> "
                << setw(4) << setprecision(1) << CHISQ_UPPER_LIMIT
                << "):" << endl;
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
            output_file << "Layer " << ilayer + 1
                        << " wiregroup " << setw(2) << iwg + 1
                        << " (chip " << setw(2) << chip + 1
                        << ") has chisq = "
                        << setw(10) << setprecision(3) << scientific << chisq[ilayer][iwg]
                        << endl;
        }
      }
    }
    output_file << endl
                << "Total number of bad channels = " << nbadch
                << endl << endl;
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

  ostringstream filename;
  filename << datadir_translation << test_14_file_el;

  ofstream output_file(filename.str().c_str());

  /* Save histograms */
  ostringstream histogram_filename;
  histogram_filename << datadir_translation << test_14_file_his_el;

  printf ("===> %s\n", histogram_filename.str().c_str());
  printf("Saving histograms\n");

  TFile histogram_file("/tmp/definitely-not-used", "RECREATE");
  cout << "WERE GOOD!" << endl;

  cout.flush();

  for (int layer = 0; layer < NLAYER; layer++) {
    for (int wiregroup = 0; wiregroup < nwires; wiregroup++) {
      (*regular_histograms)[layer][wiregroup]->Write();
    }
  }
  for (int layer = 0; layer < NLAYER; layer++) {
    for (int wiregroup = 0; wiregroup < nwires; wiregroup++) {
      (*profile_histograms)[layer][wiregroup]->Write();
    }
  }
  wiregroup_average_times->Write();
  fraction_of_events_in_peak->Write();
  equalized_delays->Write();
  

  /* Print mandatory summary statement */
  if (nbadch > 0) {
    output_file << "Test result = Failed" << endl << endl;
  }
  else {
    output_file << "Test result = OK" << endl << endl;
  }


  /* Add the date and time */
  time(&t);
  output_file << ctime(&t) << endl << endl;

  output_file.close();

  delete_all_test_14_regular_histograms();
  delete_all_test_14_profile_histograms();

  return 0;
}

/*******************************************************************************
   Analysis done for each delay value
*******************************************************************************/

int analyze_delay_hists(int idelay)
{
  int      ilayer, iwg, hid, t, max_t, nbad, nmax_t;
  float    max, sum, w_sum, e_sum, total, x;
  int    active_tbins;
  int      t_start, t_end;

  if (idelay < 0 || idelay > 15) {
    printf("Error: idelay = %d\n", idelay);
    return -1;
  }

  nbad = 0;

  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    for (iwg = 0; iwg < nwires; iwg++) {
      float * bins = (*regular_histograms)[ilayer][iwg]->GetArray();
      int number_of_bins = (*regular_histograms)[ilayer][iwg]->GetNbinsX();

      max = -1;
      max_t = 0;
      sum = w_sum = e_sum = 0;
      active_tbins = 0;
      total = 0;

      for (t = 1; t <= number_of_bins; t++) {
        if (bins[t] > 1.) active_tbins++;
        if (bins[t] > max) {
          max = bins[t];
          max_t = t;
        }
      }

      // Find start and end timebins with data
      t_start = -1;
      t_end = -1;
      for (t = ((max_t>=2)?(max_t-2):0); t < ((max_t < (number_of_bins - 1))?(max_t+2):(number_of_bins + 1)); t++)
        {
          if  (bins[t] > 1.) {
            t_end = t;
            if (t_start < 0) t_start = t;
          }

        }

      // Warn and skip empty
      if (t_start < 0 || t_end < 0) {
        printf("WARNING: Delay setting %d -  No Entries in Layer %d, Wiregroup %d", idelay, ilayer+1, iwg+1 );
        nbad++;
        continue;
      }

      // Warn if too wide time spread is detected
      if (t_end - t_start > 4)
        {
          printf("WARNING: Delay setting %d - Too many active timebins (%d) Layer %d, Wiregroup %d\n",
                 idelay, t_end - t_start, ilayer+1, iwg+1 );
        }


      for (t = t_start; t <= t_end; t++)
        {
          sum += bins[t];
          w_sum += bins[t]*(t);
          e_sum += bins[t]*(t)*(t);

        }

      if ( sum > 0 )
        {
          tavg[ilayer][iwg][idelay] = w_sum/sum;
          wiregroup_average_times->Fill(tavg[ilayer][iwg][idelay]);
          for (t=1, total=0; t <= number_of_bins; t++) total += bins[t];
          if ((x = sum / total) < 0.95)
            if ((x = sum / total) > 1.00)
              fraction_of_events_in_peak->Fill(x);

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
    }
  }

  printf("Found %d bad channels for delay point %d\n", nbad, idelay);
  return 0;
}

void reset_all_test_14_regular_histograms() {
  for (vector< vector< TH1F * > >::iterator row = regular_histograms->begin();
       row != regular_histograms->end();
       ++row) {
    for (vector<TH1F*>::iterator histogram = row->begin();
         histogram != row->end();
         ++histogram) {
      // note that an iterator is a pointer to an element in the vector, and
      // each element of the vector is a pointer to a TH1F
      (*histogram)->Reset();
    }
  }

  wiregroup_average_times->Reset();
  fraction_of_events_in_peak->Reset();

  return;
}

void delete_all_test_14_regular_histograms() {
  delete wiregroup_average_times;
  delete fraction_of_events_in_peak;

  delete_all_histograms_in_matrix(regular_histograms);

  return;
}

void delete_all_test_14_profile_histograms() {
  delete equalized_delays;

  delete_all_histograms_in_matrix(profile_histograms);

  return;
}

template <typename T>
void delete_all_histograms_in_matrix(vector< vector< T * > > * histograms) {
  if (histograms == NULL)
    return;

  histograms->clear();

  return;
}

vector< vector< TProfile * > > * create_profile_histograms(int nwires) {
  vector< vector< TProfile * > > * profile_histograms = new vector< vector< TProfile * > >();

  for (int layer = 0; layer < NLAYER; layer++) {
    vector<TProfile*> * row_of_profile_histograms = new vector<TProfile*>();

    for (int wiregroup = 0; wiregroup < nwires; wiregroup++) {
      ostringstream histogram_title;
      histogram_title <<  "layer: " << layer << ", "
                      << "wiregroup: " << setw(3) << wiregroup << "; "
                      << "time (25 ns clocks) vs delay code";
      ostringstream histogram_name;
      histogram_name << "profile_histogram_" << layer << "_" << wiregroup;
      TProfile * profile_histogram = new TProfile(histogram_name.str().c_str(),
                                                  histogram_title.str().c_str(),
                                                  16,
                                                  -0.5, 15.5,
                                                  0.0, 500.0);
      row_of_profile_histograms->push_back(profile_histogram);
    }

    profile_histograms->push_back(*row_of_profile_histograms);
  }

  return profile_histograms;
}
