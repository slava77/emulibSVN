/*
 * File : test_15.c (in V1.2  test_10.c)
 * CFEB Noise Test
 *
 * Author : Lisa Gorn
 * First version : June 7, 2000

 * Updated : March 2, 2001
 *           In this "old electronics" version, pedestal data from the
 *           first SCA time-sample is ignored when calculating the pedestal
 *           mean and rms. This time-sample should be re-included when we
 *           switch to the production CFEB's.
 * Updated : May 7, 2002 S.D.
 *           For long term pedestal behaviour we calculate now just rms of
 *           pedestals for "subruns" of 1000 ev.
 */

#include <stdio.h>
#include <math.h>
#include "daq_conf.h"
#include "csc_event.h"       /* upevt_                 */
#include "csc_parameters.h"  /* NLAYER, NSTRIP, etc... */
#include "one_page_plot.h"   /* STRIP_PLOT,...         */
#include "test_utils_for_root.h" /* calc_peds_for_root */
#include "test_params_cxx.h"     /* test config parameters */
#include "test_config_struct.h"

#include <boost/shared_ptr.hpp>
#include "one-page-plot.h"
#include "basic-utilities.h"

// root headers
#include "TH1.h" // histograms
#include "TProfile.h" // profile histograms
#include "TFile.h" // writing files
#include <vector>
#include <sstream> // ostringstream

/* Local constants */
#define   NUM_SCA_CELLS          96
#define   NUM_SCA_BLOCKS         12
#define   NUM_SCA_SAMPLES        16
#define   NUM_EVT_BINS           50

/* Local function declarations */
int test_15_begin(int pass);
int test_15_event(int pass);
int test_15_end(int pass);
int test_15_finish(void);
void book_histos_15f_(void);
void lfitw_(float *x, float *y, float *w, int *l, int *key, float *a, float *b,
            float *var);

/* Global variables local to this test */
static int     occupancy_error;
static float   cell_offset[NLAYER][NSTRIP][NUM_SCA_CELLS];
static FILE    *fp_res, *fp_bad;

static float   *mean[NLAYER], *rms[NLAYER];
static float   *sca_rms[NLAYER], *tbin_rms[NLAYER];
static float   *sca_slope[NLAYER];
static float   *pmean, *prms;
static float   *psca_rms, *ptbin_rms;
static float   *psca_slope;

extern test_config_struct tcs;
// extern tot_evt;


typedef boost::shared_ptr<TH1F> th1f_ptr;
typedef boost::shared_ptr<TProfile> tprofile_ptr;

typedef std::vector<std::vector<th1f_ptr> > th1f_2dvector_t;
typedef std::vector<std::vector<tprofile_ptr> > tprofile_2dvector_t;

th1f_2dvector_t * histograms;
TH1F * sca_block_occupancy_histogram;
TH1F * peds_event_number_histogram;
tprofile_2dvector_t * peds_sca_profile_histograms;
tprofile_2dvector_t * peds_timebin_profile_histograms;
tprofile_2dvector_t * peds_event_number_profile_histograms;

char test_15_file_bad_el[]="/test_results/temp/test_15_01.bad";
char test_15_file_res_el[]="/test_results/temp/test_15_01.results";
char test_15_file_his_el[]="/test_results/temp/test_15_01.his";
char test_15_file_sca_el[]="/test_results/temp/sca_offsets.dat";
/*******************************************************************************
   Define functions and parameters for FAST site test 15
*******************************************************************************/
static int nstrips, ncfebs;

extern "C" int test_15_init(void)
{
  int l;

  test_begin  = test_15_begin;
  test_event  = test_15_event;
  test_end    = test_15_end;
  test_finish = test_15_finish;

  num_passes = 3;
  num_events[0] = 50;
  num_events[1] = 2000;
  num_events[2] = 50000;

  switch (csc_type)
    {
    case 0:  nstrips = 48, ncfebs = 3; break;
    case 1:  nstrips = 64; ncfebs = 4; break;
    case 2:  nstrips = 64; ncfebs = 4; break;
    default: nstrips = 100, ncfebs = 5; printf ("CSC type = bullshit (%d)\n", csc_type); break;
    }

  printf ("csc_type = %d, nstrips = %d\n", csc_type, nstrips);

  pmean      = (float*) malloc(sizeof(float) * NLAYER * nstrips);
  prms       = (float*) malloc(sizeof(float) * NLAYER * nstrips);
  psca_rms   = (float*) malloc(sizeof(float) * NLAYER * nstrips);
  ptbin_rms  = (float*) malloc(sizeof(float) * NLAYER * nstrips);
  psca_slope = (float*) malloc(sizeof(float) * NLAYER * nstrips);

  for (l = 0; l < NLAYER; l++)
    {
      mean     [l] = pmean       + nstrips * l;
      rms      [l] = prms        + nstrips * l;
      sca_rms  [l] = psca_rms    + nstrips * l;
      tbin_rms [l] = ptbin_rms   + nstrips * l;
      sca_slope[l] = psca_slope  + nstrips * l;
    }

  return 0;
}

/*******************************************************************************
   Book histograms for FAST site test 15
*******************************************************************************/

int test_15_begin(int pass) {
  int     j, hid, nbins;
  char    htitle[80];
  char    *filename;

  if (pass == 0) {
    filename = (char*)malloc(strlen(datadir_translation) +
                             strlen(test_15_file_res_el) + 1 );
    if (filename == NULL)
      {
        printf("malloc() failed on filename.\n");
        return 1;
      }
    sprintf(filename,"%s%s",datadir_translation,test_15_file_res_el);

    /* Open result file in the temporary directory */

    fp_res = fopen(filename, "w");
    if(fp_res == NULL)
      {
        printf("File %s could not be opened.\n",filename);
        free(filename);
        return 1;
      }
    free(filename);
    fprintf(fp_res, "-----------------------------------------------------\n");
    fprintf(fp_res, "Results of Test 15 (CFEB Noise) for CSC %s\n", csc_name);
    fprintf(fp_res, "-----------------------------------------------------\n\n");

    filename = (char*)malloc(strlen(datadir_translation) +
                             strlen(test_15_file_bad_el) + 1 );
    if (filename == NULL)
      {
        printf("malloc() failed on filename.\n");
        return 1;
      }
    sprintf(filename,"%s%s",datadir_translation,test_15_file_bad_el);

    /* Open file for bad channels in the temporary directory */

    fp_bad = fopen(filename, "w");
    if(fp_bad == NULL)
      {
        printf("File %s could not be opened.\n",filename);
        free(filename);
        return 1;
      }
    free(filename);
    fprintf(fp_bad, "-----------------------------------------------------\n");
    fprintf(fp_bad, "Results of Test 15 (CFEB Noise) for CSC %s\n", csc_name);
    fprintf(fp_bad, "-----------------------------------------------------\n\n");

    /* Book histograms */
    nbins = MAXBINS;
    printf("Booking pass 0 histograms for test 15\n");

    histograms = new vector<vector<th1f_ptr> >();
    for(int layer=0; layer < NLAYER; layer++) {
      vector<th1f_ptr> row = vector<th1f_ptr>();
      for(int strip=0; strip < nstrips; strip++) {
        sprintf( htitle, "Layer %d Strip %d", layer+1, strip+1);

        ostringstream name;
        name << "histogram_" << layer << "_" << strip;

	th1f_ptr element(new TH1F(name.str().c_str(),
				  htitle,
				  nbins,
				  0,
				  4096));
	row.push_back(element);
      }
      histograms->push_back(row);
    }
  } else if (pass == 1 || pass == 3) {
    delete histograms;

    nbins = MAXBINS/2;
    printf("Booking pass 1 histograms for test 15\n");

    histograms = new vector<vector<th1f_ptr> >();
    for (int layer=0; layer < NLAYER; layer++) {
      vector<th1f_ptr> row = vector<th1f_ptr>();
      for (int strip=0; strip < nstrips; strip++) {
        sprintf( htitle, "Layer %d Strip %d", layer+1, strip+1);
        j = (int)mean[layer][strip];

        ostringstream name;
        name << "histogram_" << layer << "_" << strip;

	th1f_ptr element(new TH1F(name.str().c_str(),
			 htitle,
			 nbins,
			 (j + 0.5 - nbins/2.0),
			 (j + 0.5 + nbins/2.0)));
        row.push_back(element);
      }
      histograms->push_back(row);
    }
  } else if (pass == 2) {
    delete histograms;

    histograms = new vector<vector<th1f_ptr> >();

    sca_block_occupancy_histogram = new TH1F("sca_block_occupancy_histogram",
                                             "SCA block occupancy",
                                             20,
                                             0.0,
                                             20.0);
    peds_event_number_histogram = new TH1F("ped_event_number_histograms",
                                           "ped vs evtnum slope chisq/dof",
                                           100,
                                           0.0,
                                           20.0);

    // populate peds_sca_profile_histograms
    peds_sca_profile_histograms = new vector<vector<tprofile_ptr> >;
    for(int layer = 0; layer < NLAYER; ++layer) {
      vector<tprofile_ptr> row = vector<tprofile_ptr>();
      for(int strip = 0; strip < nstrips; ++strip) {
        ostringstream title;
        title << "Layer" << layer << ", strip" << strip << " peds vs SCA cell";
        ostringstream name;
        name << "peds_sca_profile_histogram_" << layer << "_" << strip;

	tprofile_ptr element(new TProfile(name.str().c_str(),
					  title.str().c_str(),
					  96,
					  0.0, 96.0,
					  -50.0, 50.0));
        row.push_back(element);
      }
      peds_sca_profile_histograms->push_back(row);
    }
    // populate peds_timebin_profile_histograms
    peds_timebin_profile_histograms = new vector<vector<tprofile_ptr> >;
    for(int layer = 0; layer < NLAYER; ++layer) {
      vector<tprofile_ptr> row = vector<tprofile_ptr>();
      for(int strip = 0; strip < nstrips; ++strip) {
        ostringstream title;
        title << "Layer" << layer << ", strip" << strip << " peds vs timebin";
        ostringstream name;
        name << "peds_timebin_profile_histogram_" << layer << "_" << strip;

	tprofile_ptr element(new TProfile(name.str().c_str(),
					  title.str().c_str(),
					  16,
					  0.0, 16.0,
					  -50.0, 50.0));
        row.push_back(element);
      }
      peds_timebin_profile_histograms->push_back(row);
    }
    // populate peds_event_number_profile_phistograms
    peds_event_number_profile_histograms = new vector<vector<tprofile_ptr> >;
    for(int layer = 0; layer < NLAYER; ++layer) {
      vector<tprofile_ptr> row = vector<tprofile_ptr>();
      for(int strip = 0; strip < nstrips; ++strip) {
        ostringstream title;
        title << "Layer" << layer << ", strip" << strip << " peds vs event number";
        ostringstream name;
        name << "peds_event_number_profile_histogram_" << layer << "_" << strip;

	tprofile_ptr element (new TProfile(name.str().c_str(),
					   title.str().c_str(),
					   50,
					   0.0, 50000.0,
					   -50.0, 50.0));
        row.push_back(element);
      }
      peds_event_number_profile_histograms->push_back(row);
    }
  }
  return 0;
}

/*******************************************************************************
   Do analysis and fill histograms for FAST site test 15
*******************************************************************************/

int test_15_event(int pass)
{
  int           j, block, hid, hid1, hid2, isample, offset;
  int           ifeb, got_an_feb = 0;
  float         sca;
  static int    first_call = 1;
  static int    nblock[16]
    = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 9, 9, 10, 10, 11, 11 };

  if (first_call) {
    first_call = 0;
  }

  /* Find pedestals, first approximately with full-range histograms in pass 0,
   * then more precisely and with higher statistics in pass 1.
   */
  if (pass == 0 || pass == 1) {
    for (int layer = 0; layer < NLAYER; layer++) {
      for (int strip = 0; strip < nstrips; strip++) {
        /* Include sample 0 when using final CFEB's */
        for (isample = 0; isample < upevt_.nsca_sample; isample++) {
          /* Fill pedestal histograms. */
          (*histograms)[layer][strip]->Fill((float)(upevt_.sca[layer][strip][isample]));
        }
      }
    }
  }

  /* Using all the data, look at SCA cell histograms */
  else if (pass == 2) {
    for (int layer = 0; layer < NLAYER; layer++) {
      for (int strip = 0; strip < nstrips; strip++) {
        /* Find where the block number changes */
        for (isample = 0; isample < upevt_.nsca_sample - 1; isample++) {
          if (upevt_.sca_block[layer][strip][isample+1] !=
              upevt_.sca_block[layer][strip][isample]) break;
        }
        offset = 7 - isample;

        /* Fill histograms */
        for (isample = 0; isample < upevt_.nsca_sample; isample++) {
          block = upevt_.sca_block[layer][strip][isample];
          if (layer % 6 == 0 && strip % 16 == 0)
            sca_block_occupancy_histogram->Fill((float)block, 0.125);

          j = 8 * nblock[block] + (isample + offset) % 8;

          sca = upevt_.sca[layer][strip][isample] - mean[layer][strip];

          (*peds_timebin_profile_histograms)[layer][strip]->Fill((float)isample, sca);
          (*peds_sca_profile_histograms)[layer][strip]->Fill((float)j, sca);
          (*peds_event_number_profile_histograms)[layer][strip]->Fill((float)upevt_.event_number, sca);
        }
      }
    }
  }

  /* Find truer noise by correcting pedestals with SCA means */
  else if (pass == 3) {
    for (int layer = 0; layer < NLAYER; layer++) {
      for (int strip = 0; strip < nstrips; strip++) {

        /* Find where the block number changes */
        for (isample = 0; isample < upevt_.nsca_sample - 1; isample++) {
          if (upevt_.sca_block[layer][strip][isample+1] !=
              upevt_.sca_block[layer][strip][isample]) break;
        }
        offset = 7 - isample;

        for (isample = 1; isample < upevt_.nsca_sample; isample++) {
          block = upevt_.sca_block[layer][strip][isample];
          j = 8 * nblock[block] + (isample + offset) % 8;

          /* Fill corrected pedestal histograms. */
          (*histograms)[layer][strip]->Fill(upevt_.sca[layer][strip][isample] -
                                            cell_offset[layer][strip][j]);
        }
      }
    }
  }

  return 0;
}


/*******************************************************************************
   End-of-run analysis for FAST site test 15
*******************************************************************************/

#define   PEDESTAL_LOWER_LIMIT       400.
#define   PEDESTAL_UPPER_LIMIT       800.
#define   RMS_LOWER_LIMIT            1.5 // Fixed to accomodate all types of chambers, was 2.0
#define   RMS_UPPER_LIMIT            6.0
#define   SCA_RMS_LOWER_LIMIT        0.0
#define   SCA_RMS_UPPER_LIMIT        2.0
#define   TBIN_RMS_LOWER_LIMIT       0.0
#define   TBIN_RMS_UPPER_LIMIT       2.0
#define   SCA_SLOPE_LOWER_LIMIT     -0.50
#define   SCA_SLOPE_UPPER_LIMIT      3.
#define   LINFIT_CHISQ_UPPER_LIMIT   15.0

int test_15_end(int pass)
{
  int     j, k, hid, ilayer, istrip, mode, ngood;
  float   limits[7], block_occ_error, mean_block_occ;
  float   sum, sumsq, ey[NUM_EVT_BINS];
  Double_t * y;
  int     block_num[NUM_SCA_BLOCKS] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14 };
  FILE    *fp2;
  char    *filename, *file_his_name, *path;

  /* TODO: we need to decide how many cfebs there are per layer based on
     chamber type */
  int cfebs_per_layer = 3;


  if (pass == 0) {
    mode = 0;
    calc_peds_for_root(mode, mean, rms, nstrips, histograms);
  }

  else if (pass == 1) {
    mode = 1;
    calc_peds_for_root(mode, mean, rms, nstrips, histograms);

    limits[0] = 350.;                   /* layer histo lower limit */
    limits[1] = 1000.;                   /* layer histo upper limit */
    limits[2] = 350.;                   /* summary histo lower limit */
    limits[3] = 1000.;                   /* summary histo upper limit */
    limits[4] = PEDESTAL_LOWER_LIMIT;   /* lower limit line */
    limits[5] = PEDESTAL_UPPER_LIMIT;   /* upper limit line */
    limits[6] = nstrips;

    one_page_plot_v3(15, 1, STRIP_PLOT, pmean, pmean, limits,
                     csc_name, "CFEB Pedestals", "Pedestal mean, ADC counts", 0);
    one_page_plot(15, "CFEB DAQ-Path Noise", "CFEB Pedestals",
                  limits[0], limits[1], "Pedestal mean v ADC counts",
                  csc_name, limits[4], limits[5],
                  nstrips, cfebs_per_layer, NLAYER, twod_array_to_vectors(pmean, NLAYER, nstrips),
                  "test_15_01.pdf");
    limits[0] =  0.0;
    limits[1] =  7.5;
    limits[2] =  0.0;
    limits[3] =  7.5;
    limits[4] =  RMS_LOWER_LIMIT;
    limits[5] =  RMS_UPPER_LIMIT;
    limits[6] = nstrips;

    one_page_plot_v3(15, 2, STRIP_PLOT, prms, prms, limits,
                     csc_name, "CFEB Noise", "Pedestal rms, ADC counts", 0);
    one_page_plot(15, "CFEB DAQ-Path Noise", "CFEB Noise",
                  limits[0], limits[1], "Pedestal rms v ADC counts",
                  csc_name, limits[4], limits[5],
                  nstrips, cfebs_per_layer, NLAYER, twod_array_to_vectors(prms, NLAYER, nstrips),
                  "test_15_02.pdf");
  }

  else if (pass == 2) {
    /* Check SCA block occupancy */
    y = (Double_t *)sca_block_occupancy_histogram->GetArray();
    sum = 0;
    occupancy_error = 0;
    printf("\n");
    fprintf(fp_res,"\n");

    for (j = 0; j < NUM_SCA_BLOCKS; j++) {
      k = block_num[j];
      sum += y[k];
      printf("Block %2d occupancy = %10d\n", k, (int)y[k]);
      fprintf(fp_res, "Block %2d occupancy = %10d\n", k, (int)y[k]);
    }
    mean_block_occ = sum / NUM_SCA_BLOCKS;
    block_occ_error = sqrt(5.) * sqrt(mean_block_occ);
    printf("mean block occupancy = %10.1f\n\n", mean_block_occ);
    fprintf(fp_res, "mean block occupancy = %8.1f +- %8.1f\n\n", mean_block_occ,
            block_occ_error);

    for (j = 0; j < NUM_SCA_BLOCKS; j++) {
      k = block_num[j];
      if (abs(y[k] - mean_block_occ) > 3 * block_occ_error) {
        printf("Possible problem with occupancy of block %2d; check histo 1\n",
               k);
        occupancy_error = 1;
      }
    }

    /* Look at uniformity of SCA pedestals */
    filename = (char*)malloc(strlen(datadir_translation) +
                             strlen(test_15_file_sca_el) + 1 );
    if (filename == NULL)
      {
        printf("malloc() failed on filename.\n");
        return 1;
      }
    sprintf(filename,"%s%s",datadir_translation,test_15_file_sca_el);
    fp2 = fopen(filename, "w");
    if (fp2 == NULL)
      {
        printf("File %s could not be opened.\n",filename);
        free(filename);
        return 1;
      }
    free(filename);
    fprintf(fp2, "\nMean SCA offsets for CSC %s (from test 15)\n\n", csc_name);

    for (ilayer = 0; ilayer < NLAYER; ilayer++) {
      for (istrip = 0; istrip < nstrips; istrip++) sca_rms[ilayer][istrip] = 0;
    }

    for (ilayer = 0; ilayer < NLAYER; ilayer++) {
      for (istrip = 0; istrip < nstrips; istrip++) {
        y = (*peds_sca_profile_histograms)[ilayer][istrip]->GetArray();
        sum = 0;
        sumsq = 0;
        for (j = 0; j < NUM_SCA_CELLS; j++) {
          sum += y[j];
          sumsq += y[j] * y[j];
        }
        sca_rms[ilayer][istrip] = sqrt(sumsq / NUM_SCA_CELLS -
                                       (sum / NUM_SCA_CELLS) * (sum / NUM_SCA_CELLS));

        for (j = 0; j < NUM_SCA_CELLS; j++)
          cell_offset[ilayer][istrip][j] = y[j];

        /* Save corrections to file */
        fprintf(fp2, "Layer %d, strip %2d\n", ilayer+1, istrip+1);
        for (j = 0; j < NUM_SCA_CELLS; j++) {
          fprintf(fp2, "%6.2f", cell_offset[ilayer][istrip][j]);
          if (j % 12 == 11) fprintf(fp2, "\n");
        }
      }
    }
    fclose(fp2);

    limits[0] = 0.0;
    limits[1] = 3.;
    limits[2] = 0.0;
    limits[3] = 3.;
    limits[4] = SCA_RMS_LOWER_LIMIT;
    limits[5] = SCA_RMS_UPPER_LIMIT;
    limits[6] = nstrips;

    one_page_plot_v3(15, 3, STRIP_PLOT, psca_rms, psca_rms,
                     limits, csc_name, "RMS of SCA means", "SCA rms, ADC counts", 0);
    one_page_plot(15, "CFEB DAQ-Path Noise", "RMS of SCA means",
                  limits[0], limits[1], "SCA rms v ADC counts",
                  csc_name, limits[4], limits[5],
                  nstrips, cfebs_per_layer, NLAYER, twod_array_to_vectors(psca_rms, NLAYER, nstrips),
                  "test_15_03.pdf");

    /* Look at sample-time dependence of pedestals */
    for (ilayer = 0; ilayer < NLAYER; ilayer++) {
      for (istrip = 0; istrip < nstrips; istrip++) {
        y = (*peds_timebin_profile_histograms)[ilayer][istrip]->GetArray();
        sum = 0;
        sumsq = 0;
        for (j = 0; j < NUM_SCA_SAMPLES; j++) {
          sum += y[j];
          sumsq += y[j] * y[j];
        }
        tbin_rms[ilayer][istrip] = sqrt(sumsq / NUM_SCA_SAMPLES -
                                        (sum / NUM_SCA_SAMPLES) * (sum / NUM_SCA_SAMPLES));
      }
    }

    limits[0] = 0.0;
    limits[1] = 3.0;
    limits[2] = 0.;
    limits[3] = 3.0;
    limits[4] = TBIN_RMS_LOWER_LIMIT;
    limits[5] = TBIN_RMS_UPPER_LIMIT;
    limits[6] = nstrips;

    one_page_plot_v3(15, 4, STRIP_PLOT, ptbin_rms, ptbin_rms,
                     limits, csc_name, "RMS of time-sample means", "SCA rms, ADC counts", 0);
    one_page_plot(15, "CFEB DAQ-Path Noise", "RMS of time-sample means",
                  limits[0], limits[1], "SCA rms v ADC counts",
                  csc_name, limits[4], limits[5],
                  nstrips, cfebs_per_layer, NLAYER, twod_array_to_vectors(ptbin_rms, NLAYER, nstrips),
                  "test_15_04.pdf");

    /* Look at pedestal stability */
    for (ilayer = 0; ilayer < NLAYER; ilayer++) {
      for (istrip = 0; istrip < nstrips; istrip++) {
        sca_slope[ilayer][istrip] = 999;
        sum   = 0.;
        sumsq = 0.;
        ngood = 0;
        y = (*peds_event_number_profile_histograms)[ilayer][istrip]->GetArray();
        for (j = 0; j < NUM_EVT_BINS; j++) {
          if (y[j] != 0 || ey[j] != 0) {
            ngood++;
            sum   += y[j];
            sumsq += y[j]*y[j];
          }
        }

        if (ngood > 0) {
          sca_slope[ilayer][istrip] = sqrt(sumsq/ngood-
                                           (sum/ngood)*(sum/ngood));
        }
      }
    }

    limits[0] = -5.0;
    limits[1] =  5.0;
    limits[2] = -5.0;
    limits[3] =  5.0;
    limits[4] = SCA_SLOPE_LOWER_LIMIT;
    limits[5] = SCA_SLOPE_UPPER_LIMIT;
    limits[6] = nstrips;

    one_page_plot_v3(15, 5, STRIP_PLOT, psca_slope, psca_slope,
                     limits, csc_name, "RMS of mean ped vs event number",
                     "ADC counts ", 0);
    one_page_plot(15, "CFEB DAQ-Path Noise", "RMS of mean ped vs event number",
                  limits[0], limits[1], "ADC counts",
                  csc_name, limits[4], limits[5],
                  nstrips, cfebs_per_layer, NLAYER, twod_array_to_vectors(psca_slope, NLAYER, nstrips),
                  "test_15_05.pdf");

    printf("Saving pass 2 histograms\n");
    file_his_name = (char*)malloc(strlen(datadir_translation) +
                                  strlen(test_15_file_his_el) + 1 );
    if (file_his_name == NULL)
      {
        printf("malloc() failed for file_his_name.  "
               "Could not save histograms.\n");
      }
    else
      {
        sprintf(file_his_name,"%s%s",
                datadir_translation,test_15_file_his_el);
        printf("Saving histograms\n");

        TFile histogram_output_file(file_his_name, "RECREATE");
        write_all_histograms_in_2d_vector(histograms);
        write_all_histograms_in_2d_vector(peds_sca_profile_histograms);
        write_all_histograms_in_2d_vector(peds_timebin_profile_histograms);
        write_all_histograms_in_2d_vector(peds_event_number_profile_histograms);
        sca_block_occupancy_histogram->Write();
        peds_event_number_histogram->Write();
        free(file_his_name);
      }

  }

  /* This section is for test only; the standard test has no pass 3 */
  else if (pass == 3) {
    mode = 1;
    calc_peds_for_root(mode, mean, rms, nstrips, histograms);

    limits[0] =  0.0;
    limits[1] =  7.5;
    limits[2] =  0.0;
    limits[3] =  7.5;
    limits[4] =  RMS_LOWER_LIMIT;
    limits[5] =  RMS_UPPER_LIMIT;
    limits[6] = nstrips;

    /* again produce a vector representing the prms */
    one_page_plot_v3(15,
                     6,
                     STRIP_PLOT,
                     prms,
                     prms,
                     limits,
                     csc_name,
                     "CFEB Noise, corrected by SCA means",
                     "ped rms, ADC counts",
                     0);
    one_page_plot(15, "CFEB DAQ-Path Noise", "ped rms vs ADC counts",
                  limits[0], limits[1], "CFEB Noise, corrected by SCA means",
                  csc_name, limits[4], limits[5],
                  nstrips, cfebs_per_layer, NLAYER, twod_array_to_vectors(prms, NLAYER, nstrips),
                  "test_15_06.pdf");
  }

  return 0;
}

/*******************************************************************************
   End-of-test analysis for FAST site test 15
*******************************************************************************/
int write_peds_nstrip(float *mean, float *rms, char *out_file, int nstrip);

int test_15_finish(void)
{
  int     ilayer, istrip;
  int     nbad;
  char *file_his_name, pedfilename[200], *path;

  /* Find and list bad channels */
  nbad = 0;
  fprintf(fp_bad, "BAD CHANNELS : \n");
  fprintf(fp_bad, "---------------------------------------------------------"
          "-------------------\n");
  fprintf(fp_bad, "Layer  Strip    Pedestal   Ped rms   SCA rms   Tbin rms "
          "Slope rms  \n");
  fprintf(fp_bad, " lower limits: %8.2f  %8.2f  %8.2f  %8.2f  %8.2f\n",
          PEDESTAL_LOWER_LIMIT, RMS_LOWER_LIMIT, SCA_RMS_LOWER_LIMIT,
          TBIN_RMS_LOWER_LIMIT, SCA_SLOPE_LOWER_LIMIT);
  fprintf(fp_bad, " upper limits: %8.2f  %8.2f  %8.2f  %8.2f  %8.2f\n",
          PEDESTAL_UPPER_LIMIT, RMS_UPPER_LIMIT, SCA_RMS_UPPER_LIMIT,
          TBIN_RMS_UPPER_LIMIT, SCA_SLOPE_UPPER_LIMIT );
  fprintf(fp_bad, "---------------------------------------------------------"
          "-------------------\n");

  fprintf(fp_res, "---------------------------------------------------------"
          "-------------------\n");
  fprintf(fp_res, "Layer  Strip    Pedestal   Ped rms   SCA rms   Tbin rms "
          "Slope rms  \n");
  fprintf(fp_res, " lower limits: %8.2f  %8.2f  %8.2f  %8.2f  %8.2f\n",
          PEDESTAL_LOWER_LIMIT, RMS_LOWER_LIMIT, SCA_RMS_LOWER_LIMIT,
          TBIN_RMS_LOWER_LIMIT, SCA_SLOPE_LOWER_LIMIT);
  fprintf(fp_res, " upper limits: %8.2f  %8.2f  %8.2f  %8.2f  %8.2f\n",
          PEDESTAL_UPPER_LIMIT, RMS_UPPER_LIMIT, SCA_RMS_UPPER_LIMIT,
          TBIN_RMS_UPPER_LIMIT, SCA_SLOPE_UPPER_LIMIT );
  fprintf(fp_res, "---------------------------------------------------------"
          "-------------------\n");

  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    for (istrip = 0; istrip < nstrips; istrip++) {
      fprintf(fp_res, "  %d      %2d    %8.2f  %8.2f  %8.2f  %8.2f  %8.2f"
              "\n", ilayer+1, istrip+1, mean[ilayer][istrip], rms[ilayer][istrip],
              sca_rms[ilayer][istrip], tbin_rms[ilayer][istrip],
              sca_slope[ilayer][istrip]);

      if ((    mean[ilayer][istrip]     < PEDESTAL_LOWER_LIMIT) ||
          (    mean[ilayer][istrip]     > PEDESTAL_UPPER_LIMIT) ||
          (     rms[ilayer][istrip]     < RMS_LOWER_LIMIT)      ||
          (     rms[ilayer][istrip]     > RMS_UPPER_LIMIT)      ||
          ( sca_rms[ilayer][istrip]     < SCA_RMS_LOWER_LIMIT)  ||
          ( sca_rms[ilayer][istrip]     > SCA_RMS_UPPER_LIMIT)  ||
          (tbin_rms[ilayer][istrip]     < TBIN_RMS_LOWER_LIMIT) ||
          (tbin_rms[ilayer][istrip]     > TBIN_RMS_UPPER_LIMIT) ||
          (sca_slope[ilayer][istrip]    < SCA_SLOPE_LOWER_LIMIT) ||
          (sca_slope[ilayer][istrip]    > SCA_SLOPE_UPPER_LIMIT)) {
        fprintf(fp_bad, "  %d      %2d    %8.2f  %8.2f  %8.2f  %8.2f  %8.2f"
                "\n", ilayer+1, istrip+1, mean[ilayer][istrip], rms[ilayer][istrip],
                sca_rms[ilayer][istrip], tbin_rms[ilayer][istrip],
                sca_slope[ilayer][istrip]);
        nbad++;
      }
    }
  }
  fprintf(fp_bad, "--------------------------------------------------------\n");
  fprintf(fp_bad, "\nTotal number of bad channels = %d\n\n", nbad);

  /* Print mandatory summary statement */
  if (nbad > 0){
    fprintf(fp_bad, "Test result = Failed, there are bad channel\n");
    fprintf(fp_res, "Test result = Failed, there are bad channel\n");
  }
  else{
    fprintf(fp_res, "Test result = OK; no bad channel\n");
    fprintf(fp_bad, "Test result = OK; no bad channel\n");
  }
  if (occupancy_error){
    fprintf(fp_bad, "Test result = Failed, there are some problem with occupancy\n");
    fprintf(fp_res, "Test result = Failed  there are some problem with occupancy\n");
  }
  else{
    fprintf(fp_res, "Test result = OK, no problem wtih occupancy\n");
    fprintf(fp_bad, "Test result = OK, no problem wtih occupancy\n");
  }

  fclose(fp_bad);
  fclose(fp_res);

  /* destroy all the histograms */
  delete histograms;
  delete sca_block_occupancy_histogram;
  delete peds_event_number_histogram;
  delete peds_sca_profile_histograms;
  delete peds_timebin_profile_histograms;
  delete peds_event_number_profile_histograms;

  // save pedestals as online analysis used to do for event display to use
  path = getenv("DATADIR");
  if(path == NULL)
    {
      printf("Can\'t save pedestals. Environment variable DATADIR is not set.\n");
    }
  else
    {
      sprintf(pedfilename,"%s/pedestals/DEFAULT.ped", path);
      printf("Saving pedestals to file %s\n", pedfilename);
      if(write_peds_nstrip(pmean, prms, pedfilename, nstrips) != 0)
        printf("Error while writing pedestal file\n");
    }

  return 0;
}

int write_peds_nstrip(float *ppmean, float *pprms, char *out_file, int nstrip)
{
  FILE *out_fp;
  int ilayer, istrip;
  int rtval = 0;

  out_fp = fopen (out_file, "w");

  if (out_fp == NULL)
    {
      printf("Failed to open pedestal file %s for writing EE", out_file);
      return -1;
    }

  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
      for (istrip = 0; istrip < NSTRIP; istrip++)
        {
          if (istrip < nstrip)
            {
              rtval = fprintf
                (
                 out_fp, "%5.2f %5.2f ",
                 ppmean[istrip + nstrip*ilayer],
                 pprms [istrip + nstrip*ilayer]
                 );
            }
          else
            {   // for chambers with 4 CFEBs, print 0 for 5-th CFEB for compatibility
              rtval = fprintf
                (
                 out_fp, "%5.2f %5.2f ",
                 0.0,
                 0.0
                 );
            }

          if (rtval == -1) return rtval;

          if (( (istrip+1) % 10) == 0 )
            {
              rtval = fprintf(out_fp, "\n");
              if (rtval == -1) return rtval;
            }
        }
    }

  fclose(out_fp);

  return 0;
}
