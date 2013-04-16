
/*
 * File : test_23.c (in V1.2 test_18.c) 
 * Neighbor Comparator Tests
 *
 * Author : Lisa Gorn      
 * First version : March 12, 2001
 */

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <math.h>
#include <time.h>
#include "cfortran.h"
#include "hbook.h"
#include "event_ana.h"       /* Function prototypes    */
#include "daq_conf.h"
#include "csc_event.h"       /* upevt_                 */
#include "csc_parameters.h"  /* NLAYER, NSTRIP, etc... */
#include "misc.h"            /* logical typedef */
#include "one_page_plot.h"   /* STRIP_PLOT,...         */
#include "test_utils.h"      /* chamber labels         */
#include "test_params.h"     /* test config parameters */

//#include "csc_unpacker.h"
//#include "emu/daq/reader/RawDataFile.h"

/* Local constants */
#define  MAXBINS                   200
#define  NCLCT_FIFO_TBINS            7
#define  NOISE_LOWER_LIMIT           0.
#define  NOISE_UPPER_LIMIT          12.5
#define  OCCUPANCY_LOWER_LIMIT      0.5
#define  OCCUPANCY_UPPER_LIMIT      1500.
#define  OFFSET_LOWER_LIMIT        -20.
#define  OFFSET_UPPER_LIMIT         20.
#define  OFFSET_ERROR_UPPER_LIMIT    5.
#define  NOISE_ERROR_UPPER_LIMIT     5.
#define  CHISQ_UPPER_LIMIT          10.

/* Local function declarations */
int test_23_begin(int pass);
int test_23_event(int pass);
int test_23_end(int pass);
int test_23_finish(void);

void book_histos_23f_(void);
void lfitw_(float *x, float *y, float *w, int *l, int *key, float *a, float *b, 
	    float *var);
void erf_fit_(int *hid, float *par, float *sigpar, float *chisq);
int print_result_bad_19 (void);
int print_result_19 (void);

/* Global variables local to this test */
static int     nbad;
//static float   mean[NLAYER][NSTRIP], rms[NLAYER][NSTRIP];
//static float   occupancy[NLAYER][2*NSTRIP];
static FILE    *fp;

//float   offset[NLAYER][NSTRIP], noise[NLAYER][NSTRIP];     // for neighbour strips
//float   offset_2[NLAYER][NSTRIP], noise_2[NLAYER][NSTRIP]; // for separated strips
int bad_chan_sum = 0;

char test_23_file_his_el[]="/test_results/temp/test_23_01.his";

float *poffset, *poffset_2, *pnoise, *pnoise_2, *poccupancy, *pmean, *prms;
float *offset[NLAYER], *noise[NLAYER];     // for neighbour strips
float *offset_2[NLAYER], *noise_2[NLAYER]; // for separated strips
float *occupancy[NLAYER];
float *mean[NLAYER];
float *rms[NLAYER];



/*******************************************************************************
   Define functions and parameters for FAST site test 23
*******************************************************************************/
static int nstrips, ncfebs;
int test_23_init(void)
{
    int l;
    test_begin  = test_23_begin;
    test_event  = test_23_event;
    test_end    = test_23_end;
    test_finish = test_23_finish;

    num_passes = 3;
    num_events[0] = 50;
    num_events[1] = 5000;
    num_events[2] = 191000;

    switch (csc_type)
    {
    case 2:  nstrips = 64; ncfebs = 4; break;
    default: nstrips = 80; ncfebs = 5; break;
    }

    printf ("csc_type = %d, nstrips = %d\n", csc_type, nstrips);

    poffset    = malloc (sizeof(float) * NLAYER * nstrips);
    poffset_2  = malloc (sizeof(float) * NLAYER * nstrips);
    pnoise     = malloc (sizeof(float) * NLAYER * nstrips);
    pnoise_2   = malloc (sizeof(float) * NLAYER * nstrips);
    poccupancy = malloc (sizeof(float) * NLAYER * nstrips * 2);
    pmean      = malloc (sizeof(float) * NLAYER * nstrips);
    prms       = malloc (sizeof(float) * NLAYER * nstrips);

    for (l = 0; l < NLAYER; l++)
    {
	offset   [l] = poffset     + nstrips * l;
	offset_2 [l] = poffset_2   + nstrips * l;
	noise    [l] = pnoise      + nstrips * l;
	noise_2  [l] = pnoise_2    + nstrips * l;
	occupancy[l] = poccupancy  + nstrips * l * 2;
	mean     [l] = pmean       + nstrips * l;
	rms      [l] = prms        + nstrips * l;
    }

    memset(poccupancy, 0, NLAYER * 2 * nstrips * sizeof(float));  
    return 0;
}

/*******************************************************************************
   Book histograms for FAST site test 23                                       
*******************************************************************************/

int test_23_begin(int pass)
  {
  int     j, hid, ilayer, istrip, nbins;
  char    htitle[70];
  //  char *filename;

  if (pass == 0)
    {

/* Book histograms */
    nbins = MAXBINS;
    printf("Booking pass 0 histograms for test 23\n");
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      for (istrip = 0; istrip < nstrips; istrip++)
	{
        hid = 100 * (ilayer + 1) + (istrip + 1);
        sprintf( htitle, "Layer %d Strip %d", ilayer+1, istrip+1);
        HBOOK1( hid, htitle, nbins, 0., 4096., 0.);
	}
      }
    }

  else if (pass == 1)
    {
    HDELET(0);
    nbins = MAXBINS/2;
    printf("Booking pass 1 histograms for test 23\n");
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      for(istrip = 0; istrip < nstrips; istrip++)
	{
        hid = 100 * (ilayer + 1) + (istrip + 1);
        sprintf( htitle, "Layer %d Strip %d", ilayer+1, istrip+1);
        j = (int)mean[ilayer][istrip];
        HBOOK1( hid, htitle, nbins, (float)(j+0.5-nbins/2),
		(float)(j+0.5+nbins/2), 0.0);
	}
      }
    }

  else if (pass == 2)
    {
    HDELET(0);
    printf("Booking pass 2 histograms for test 23\n");
    HBOOK1(1, "Chisq of error function fit", 100, 0., 5., 0.); 
    HBOOK1(2, "Offset error", 100, 0., 10., 0.); 
    HBOOK1(3, "Noise error", 100, 0., 10., 0.); 
    HBOOK1(4, "Chisq of error function fit (separ.str.)", 100, 0., 5., 0.); 
    HBOOK1(5, "Offset error(separ.str.)", 100, 0., 10., 0.); 
    HBOOK1(6, "Noise error(separ.str.)", 100, 0., 10., 0.); 

    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
	sprintf ( htitle, " Occupancy for layer %d", ilayer+1);
	hid = 10 + ilayer +1;
	HBOOK1( hid, htitle, 160, 0.5, 160.5, 0.);
      for(istrip = 0; istrip < nstrips - 1; istrip++)
	{
        hid = 100 * (ilayer + 1) + (istrip + 1);
        sprintf( htitle, "L%d s%d-%d comp-on dQ occ, numer", ilayer+1, istrip+1,
		 istrip+2);
        HBOOK1( hid, htitle, 50, -100., 100., 0.);

        hid = 1000 + 100 * (ilayer + 1) + (istrip + 1);
        sprintf( htitle, "L%d s%d-%d comp-on dQ occ, denom", ilayer+1, istrip+1,
		 istrip+2);
        HBOOK1( hid, htitle, 50, -100., 100., 0.);

        hid = 2000 + 100 * (ilayer + 1) + (istrip + 1);
        sprintf( htitle, "L%d s%d-%d comp-on dQ occ, denom", ilayer+1, istrip+1,
		 istrip+2);
        HBOOK1( hid, htitle, 50, -100., 100., 0.);
        hid = 3000 + 100 * (ilayer + 1) + (istrip + 1);
        sprintf( htitle, "L%d s%d-%d comp-on dQ occ, denom", ilayer+1, istrip,
		 istrip+2);
        HBOOK1( hid, htitle, 50, -50., 50., 0.);
        hid = 4000 + 100 * (ilayer + 1) + (istrip + 1);
        sprintf( htitle, "L%d s%d-%d comp-on dQ occ, denom", ilayer+1, istrip,
		 istrip+2);
        HBOOK1( hid, htitle, 50, -50., 50., 0.);
        hid = 5000 + 100 * (ilayer + 1) + (istrip + 1);
        sprintf( htitle, "L%d s%d-%d comp-on dQ occ, denom", ilayer+1, istrip,
		 istrip+2);
        HBOOK1( hid, htitle, 50, -50., 50., 0.);

	}
      }
    }

  return 0;
  }

/*******************************************************************************
   Do analysis and fill histograms for FAST site test 23
*******************************************************************************/

int test_23_event(int pass)
  {
  int              i, j, k, hid, ilayer, istrip, istrip2, hs, imax;
  int              adc, adcmax, adc2max, ifeb, isample;
  unsigned long    triad;
  static int       first_call = 1;
  float            q, q2, delta_q;
  logical          got_an_feb;

  if (first_call)
    {
    first_call = 0;
    }

/* Check to see if we have an empty event */
  for (ifeb = 0, got_an_feb = 0; ifeb < ncfebs; ifeb++) 
    if (upevt_.active_febs[ifeb] > 0) got_an_feb = 1;
  if (!got_an_feb)
    {
    printf("No active febs in this event\n");
    return 0;
    }

/* The first two passes are used to calculate pedestals */
  if (pass == 0 || pass == 1)
    {
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      for (istrip = 0; istrip < nstrips; istrip++)
	{
        hid = 100 * (ilayer + 1) + (istrip + 1);

//      for (isample = 0; isample < upevt_.nsca_sample; isample++) 
        for (isample = 1; isample < 3; isample++)
	  HFILL(hid, (float )(upevt_.sca[ilayer][istrip][isample]), 0.0, 1.0);
	}
      }
    }

  else if (pass == 2)
    {
/* Look for triads */
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      for (j = 0; j < nstrips / 2; j++)
	{

        if ((triad = upevt_.clct_dump[ilayer][j]) != 0)
	  {
          hs = 4 * j;

/* Find the strip and halfstrip */
          for (k = 0; k < NCLCT_FIFO_TBINS - 2; k++)
	    {
            if (triad & (1 << k))
	      {
              if (triad & (1 << (k + 1))) hs += 2;
              if (triad & (1 << (k + 2))) hs += 1;
              istrip = hs / 2;

	      HFILL (10+ilayer+1, (float) (hs+1), 0., 1.);
	      occupancy[ilayer][hs] += 1.;
/* Find the peak sample in the strip with the comparator hit */
              adcmax = -1;
              for (i = 0; i < upevt_.nsca_sample; i++)
		{
                if ((adc = upevt_.sca[ilayer][istrip][i]) > adcmax)
		  {
                  adcmax = adc; 
                  imax = i;
		  }
		}
              q = adcmax - mean[ilayer][istrip];

/* Find the charge in the same sample in the neighbor strip */
              if (hs % 2 == 0) istrip2 = istrip - 1;
              else istrip2 = istrip + 1;
              if (istrip2 > nstrips - 1) break;

              adc2max = upevt_.sca[ilayer][istrip2][imax];
              q2 = adc2max - mean[ilayer][istrip2];

/* Fill histograms for each strip with the charge difference */
              delta_q = q - q2;

              if (hs % 2 == 0)
		{
                hid = 100 * (ilayer + 1) + (istrip2 + 1);
                HFILL(hid, delta_q, 0., 1.);
                hid = 1000 + 100 * (ilayer + 1) + (istrip2 + 1);
                HFILL(hid, delta_q, 0., 1.);
		}
              else
		{
                hid = 1000 + 100 * (ilayer + 1) + (istrip + 1);
                HFILL(hid, -delta_q, 0., 1.);
		}
	      if (istrip>0 && istrip <(nstrips-1))
		{
		  delta_q = upevt_.sca[ilayer][istrip+1][imax] - mean[ilayer][istrip+1]  
		    - (upevt_.sca[ilayer][istrip-1][imax] - mean[ilayer][istrip-1]);

		  hid = 3000 + 100 * (ilayer + 1) + (istrip + 1);
		  HFILL(hid+1000, delta_q, 0., 1.);
		  if (hs %2 != 0) HFILL(hid, delta_q, 0., 1.);			       
		} 	      
              break;
	      }
	    }
	  }
	}
      }
    }
  
  return 0;
  }


/*******************************************************************************
   End-of-run analysis for FAST site test 23
*******************************************************************************/

int test_23_end(int pass)
  {
  int     hid, ilayer, istrip, mode;
  float   limits[7], par[2], sigpar[2], chisq;
  //  char    htitle[70];
  char *file_his_name;

  if (pass == 0)
    {
    mode = 0;
    calc_peds(mode, mean, rms, nstrips);
    }

  else if (pass == 1)
    {
    mode = 1;
    calc_peds(mode, mean, rms, nstrips);
    }

  else if (pass == 2)
    {
/* Next eleven lines for TEST only !!! (reads histos from existing file) */ 
      /*
  HDELET(0);
  HBOOK1(1, "Chisq of error function fit", 100, 0., 5., 0.); 
  HBOOK1(2, "Offset error", 100, 0., 10., 0.); 
  HBOOK1(3, "Noise error", 100, 0., 10., 0.); 

  HBOOK1(4, "Chisq of error function fit (separ.str.)", 100, 0., 5., 0.); 
  HBOOK1(5, "Offset error (separat. str.0", 100, 0., 10., 0.); 
  HBOOK1(6, "Noise error (separ.str.)", 100, 0., 10., 0.); 
  for (ilayer = 0; ilayer < NLAYER; ilayer++)
  {
  for(istrip = 0; istrip < nstrips - 1; istrip++)
  {
  hid = 2000 + 100 * (ilayer + 1) + (istrip + 1);
  sprintf( htitle, "L%d s%d-%d result", ilayer+1, istrip+1,
  istrip+2);
  HBOOK1( hid, htitle, 50, -100., 100., 0.);
  hid = 5000 + 100 * (ilayer + 1) + (istrip + 1);
  sprintf( htitle, "L%d s%d-%d result", ilayer+1, istrip,
  istrip+2);
  HBOOK1( hid, htitle, 50, -50., 50., 0.);
  }
  }
  HRGET(0, "/home/fast/data/test_results/temp/test_23.his", " ");
      */
/* End of special code for test */

      for (ilayer = 0; ilayer < NLAYER; ilayer++)
	{
	  offset[ilayer][nstrips-1] = -999.;
	  noise[ilayer][nstrips-1]  = -999.;
	  offset_2[ilayer][nstrips-1] = -999.;
	  noise_2[ilayer][nstrips-1]  = -999.;
	  
	  for (istrip = 0; istrip < nstrips - 1; istrip++)
	    {
	      offset[ilayer][istrip] = -999.;
	      noise[ilayer][istrip]  = -999.;
	      hid = 100 * (ilayer + 1) + (istrip + 1);
	      if (HEXIST(hid) && HEXIST(1000 + hid)
		  && (occupancy[ilayer][2*istrip+1] > 0.)
		  && (occupancy[ilayer][2*istrip+2] > 0.) )
		{
		  erf_fit_(&hid, par, sigpar, &chisq);
		  HFILL(1, chisq, 0., 1.);
		  HFILL(2, sigpar[0], 0., 1.);
		  HFILL(3, sigpar[1], 0., 1.);
		  if (chisq <= CHISQ_UPPER_LIMIT)
		    {
		      offset[ilayer][istrip] = par[0];
		      noise[ilayer][istrip]  = par[1];
		    }
		}
	      offset_2[ilayer][istrip] = -999.;
	      noise_2[ilayer][istrip]  = -999.;
	      hid = 3000 + 100 * (ilayer + 1) + (istrip + 1);
	      if (HEXIST(hid) && HEXIST(1000 + hid) && (istrip != 0)
		  && (occupancy[ilayer][2*istrip] > 0.)
		  && (occupancy[ilayer][2*istrip+1] > 0.) )
		{
		  erf_fit_(&hid, par, sigpar, &chisq);
		  HFILL(4, chisq, 0., 1.);
		  HFILL(5, sigpar[0], 0., 1.);
		  HFILL(6, sigpar[1], 0., 1.);
		  if (chisq <= CHISQ_UPPER_LIMIT)
		    {
		      offset_2[ilayer][istrip] = par[0];
		      noise_2[ilayer][istrip] = par[1];
		    }
		}
	    }
	}
      
      limits[0] = -40.0;
      limits[1] =  40.0;
      limits[2] = -40.0;
      limits[3] =  40.0;
      limits[4] = OFFSET_LOWER_LIMIT;
      limits[5] = OFFSET_UPPER_LIMIT;
      limits[6] = nstrips;
      
      one_page_plot_v3(23, 1, STRIP_PLOT, poffset, poffset, 
		       limits, csc_name, "(N+1)-N  comparator offset ", "offset, ADC counts", 0); 
      one_page_plot_v3(23, 3, STRIP_PLOT, poffset_2, poffset_2, 
		       limits, csc_name, "(N+1)-(N-1) comparator offset", "offset, ADC counts", 0); 
      
      limits[0] =   0.0;
      limits[1] =  20.0;
      limits[2] =   0.0;
      limits[3] =  20.0;
      limits[4] = NOISE_LOWER_LIMIT;
      limits[5] = NOISE_UPPER_LIMIT;
      limits[6] = nstrips;
      
      one_page_plot_v3(23, 2, STRIP_PLOT, pnoise, 
		       pnoise, limits, 
		       csc_name, "(N+1)-N comparator noise", 
		       "noise, ADC counts", 0); 
      one_page_plot_v3(23, 4, STRIP_PLOT, pnoise_2, 
		       pnoise_2, limits, 
		       csc_name, "(N+1)-(N-1) comparator noise", 
		       "noise, ADC counts", 0); 

      limits[0] =     0.;
      limits[1] =  2500.;
      limits[2] =     0.;
      limits[3] =  2500.;
      limits[4] = OCCUPANCY_LOWER_LIMIT;
      limits[5] = OCCUPANCY_UPPER_LIMIT;
      limits[6] = nstrips;
      
      one_page_plot_v3(23, 5, HSTRIP_PLOT, poccupancy, 
		       poccupancy, limits, 
		       csc_name, "Occupancy ", 
		       "Occupancy, counts", 0); 
      
    //  }    
    /* Save histograms */
      file_his_name = malloc(strlen(datadir_translation) + 
			     strlen(test_23_file_his_el) + 1);

      if (file_his_name == NULL)
	{
	  printf("malloc() failed for file_his_name.  "
		 "Could not save histograms.\n");
	}
      else
	{
	  sprintf(file_his_name,"%s%s",
		  datadir_translation,test_23_file_his_el);
	  printf("Saving pass 2 histograms\n");
	  HRPUT(0, file_his_name, "N");
	  free(file_his_name);    
	}
      print_result_bad_23();
      print_result_23();
    }
  return 0;
  }

/******************************************************************************
   End-of-test analysis for FAST site test 23
******************************************************************************/

int test_23_finish(void)
  {

/* Save histograms */
  printf("Saving histograms\n");


  HDELET(0);

  return 0;
  }

/*--------------------------- print_result_bad_23 -------------------------*/

int print_result_bad_23 (void) 
{
  char * filename;
  char test_23_file_el[]="/test_results/temp/test_23_01.bad";
  int istrip, ilayer;
  int bad_chan;
  time_t t;
  char   timestring[40];

  filename = malloc(strlen(datadir_translation) + 
		    strlen(test_23_file_el) + 1 );
  if (filename == NULL)
    {
      printf("malloc() failed on filename.\n");
      return 1;
    }
  sprintf(filename,"%s%s",datadir_translation,test_23_file_el);

/* Open result file in the temporary directory */
  fp = fopen(filename, "w");
  if(fp == NULL)
    {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
    }
  free(filename);
  
  fprintf(fp, "\n");
  fprintf(fp, "----------------------------------------------------------\n");
  fprintf(fp, "Results of test 23 ( Comparator offset) for %s\n", csc_name);
  fprintf(fp, "----------------------------------------------------------"
	  "\n\n");

  /* Header for list of bad channels */
  fprintf(fp, "Bad channel criteria are:\n");
  fprintf(fp, "  offset range       %5.1f to %4.1f ADC counts\n", 
	  OFFSET_LOWER_LIMIT, OFFSET_UPPER_LIMIT);
  fprintf(fp, "  noise range        %5.1f to %4.1f ADC counts\n", 
	  NOISE_LOWER_LIMIT, NOISE_UPPER_LIMIT);
  fprintf(fp, "  offset fit error   < %4.1f ADC counts\n", 
	  OFFSET_ERROR_UPPER_LIMIT);
  fprintf(fp, "  noise fit error    < %4.1f ADC counts\n", 
	  NOISE_ERROR_UPPER_LIMIT);
  fprintf(fp, "  chisq of fit       < %4.1f\n", CHISQ_UPPER_LIMIT);
  fprintf(fp, "\n");
  nbad = 0;
  fprintf(fp, "BAD CHANNELS : \n");
  fprintf(fp, "------------------------------------------"
	      "------------------------------------------\n");
  fprintf(fp, " Layer   Strips    Offset (ADC counts)     Noise (ADC coun"
	    "ts)     Chisq\n");
  fprintf(fp, "----------------------------+----------------------+------"
	  "---------------\n");

  for (ilayer =0; ilayer <NLAYER; ilayer++)
    {
      for (istrip = 0; istrip < nstrips-1; istrip++)
	{
	  bad_chan = 0;
	      if ((offset[ilayer][istrip] < OFFSET_LOWER_LIMIT) ||
		  (offset[ilayer][istrip] > OFFSET_UPPER_LIMIT) )
		{
		  bad_chan = 1;
		  fprintf(fp, "%d     Offset  %2d-%2d   %9.2e out of limit %9.2e   %9.2e\n",
			  ilayer+1, istrip+1, istrip+2, offset[ilayer][istrip],
			  OFFSET_LOWER_LIMIT, OFFSET_UPPER_LIMIT);

		}
	      if ((noise[ilayer][istrip] < NOISE_LOWER_LIMIT) ||
		  (noise[ilayer][istrip] > NOISE_UPPER_LIMIT) )
		{
		  bad_chan = 1;
		  fprintf(fp, "%d     Noise   %2d-%2d   %9.2e out of limit %9.2e   %9.2e\n",
			  ilayer+1, istrip+1, istrip+2, noise[ilayer][istrip],
			  NOISE_LOWER_LIMIT, NOISE_UPPER_LIMIT);
		}
	      if ( ( (offset_2[ilayer][istrip] < OFFSET_LOWER_LIMIT) ||
		     (offset_2[ilayer][istrip] > OFFSET_UPPER_LIMIT)   ) && (istrip != 0) )
		{
		  bad_chan = 1;
		  fprintf(fp, "%d     Offset  %2d-%2d   %9.2e out of limit %9.2e   %9.2e\n",
			  ilayer+1, istrip, istrip+2, offset_2[ilayer][istrip],
			  OFFSET_LOWER_LIMIT, OFFSET_UPPER_LIMIT);

		}
	      if (((noise_2[ilayer][istrip] < NOISE_LOWER_LIMIT) ||
		   (noise_2[ilayer][istrip] > NOISE_UPPER_LIMIT)    )   && (istrip != 0) )
		{
		  bad_chan = 1;
		  fprintf(fp, "%d     Noise   %2d-%2d   %9.2e out of limit %9.2e   %9.2e\n",
			  ilayer+1, istrip, istrip+2, noise_2[ilayer][istrip],
			  NOISE_LOWER_LIMIT, NOISE_UPPER_LIMIT);

		}
	      if( ((occupancy[ilayer][2*istrip] < OCCUPANCY_LOWER_LIMIT) ||
		   (occupancy[ilayer][2*istrip] > OCCUPANCY_UPPER_LIMIT)   )  &&
		   (istrip != 0) && (istrip != (nstrips-1))         )
		{
		  bad_chan = 1;
		  fprintf(fp, " Occup  %d %2d halfstr   %9.2e out of limit %9.2e   %9.2e\n", 
			  ilayer+1, 2*istrip, occupancy[ilayer][2*istrip], 
			  OCCUPANCY_LOWER_LIMIT, OCCUPANCY_UPPER_LIMIT);
		}
	      if( ((occupancy[ilayer][2*istrip+1] < OCCUPANCY_LOWER_LIMIT) ||
		   (occupancy[ilayer][2*istrip+1] > OCCUPANCY_UPPER_LIMIT) )  && 
		   (istrip != 0) && (istrip != nstrips-1))
		{
		  bad_chan += 1;
		  fprintf(fp, " Occup  %d %2d halfstr   %9.2e out of limit %9.2e   %9.2e\n", 
			  ilayer+1, 2*istrip+1, occupancy[ilayer][2*istrip+1],
			  OCCUPANCY_LOWER_LIMIT, OCCUPANCY_UPPER_LIMIT);
		}
	      bad_chan_sum += bad_chan;
	}
    }

/* Trailer for list of bad channels */
  fprintf(fp, "------------------------------------------"
	      "------------------------------------------\n");

  fprintf(fp, "\nTotal number of bad channels = %d\n\n", bad_chan_sum);

/* Print mandatory summary statement */
  if (bad_chan_sum > 0) fprintf(fp, "Test result = Failed\n\n");
  else fprintf(fp, "Test result = OK\n\n");

/* Add the date and time */
  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp, "%s\n\n", timestring);

  fclose(fp);
  return 0;
}

/*--------------------------------- print_result_23 ---------------------------*/

int print_result_23 (void) 
{
  char * filename;
  int ihs, istrip, ilayer;
  char test_23_01[]="/test_results/temp/test_23_01.result";
  char test_23_02[]="/test_results/temp/test_23_02.result";
  char test_23_03[]="/test_results/temp/test_23_03.result";
  char test_23_04[]="/test_results/temp/test_23_04.result";
  char test_23_05[]="/test_results/temp/test_23_05.result";
  time_t t;
  char   timestring[40];

  filename = malloc(strlen(datadir_translation) + 
		    strlen(test_23_01) + 1 );
  if (filename == NULL)
    {
      printf("malloc() failed on filename.\n");
      return 1;
    }
  sprintf(filename,"%s%s",datadir_translation,test_23_01);

/* Open result file in the temporary directory */
  fp = fopen(filename, "w");
  if(fp == NULL)
    {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
    }

  
  fprintf(fp, "\n");
  fprintf(fp, "----------------------------------------------------------\n");
  fprintf(fp, "Results of test 23 ( Comparator offset for (N+1)-N for %s\n", csc_name);
  fprintf(fp, "----------------------------------------------------------"
	  "\n\n");


  fprintf(fp, "------------------------------------------"
	      "------------------------------------------\n");
  fprintf(fp, " Strips, Layer 1, Layer 2, Layer 3 , Layer 4 , Layer 5, Layer 6 \n ");
  fprintf(fp, "----------------------------+----------------------+------"
	  "---------------\n");

  for (ilayer =0; ilayer <NLAYER; ilayer++)
    {
      for (istrip = 0; istrip < nstrips-1; istrip++)
	{
	  fprintf(fp, " %d  %2d-%2d   %9.2e  %9.2e  %9.2e %9.2e %9.2e %9.2e\n",
			  ilayer+1, istrip+1,istrip+2,
		  offset[0][istrip], offset[1][istrip],
		  offset[2][istrip], offset[3][istrip],
		  offset[3][istrip], offset[5][istrip]);
	}
    }

/* Trailer for list of bad channels */
  fprintf(fp, "------------------------------------------"
	      "------------------------------------------\n");

  fprintf(fp, "\nTotal number of bad channels = %d\n\n", bad_chan_sum);

/* Print mandatory summary statement */
  if (bad_chan_sum > 0) fprintf(fp, "Test result = Failed\n\n");
  else fprintf(fp, "Test result = OK\n\n");

/* Add the date and time */
  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp, "%s\n\n", timestring);

  fclose(fp);

  /* --------- file 02 -------------------*/

  sprintf(filename,"%s%s",datadir_translation,test_23_02);

/* Open result file in the temporary directory */
  fp = fopen(filename, "w");
  if(fp == NULL)
    {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
    }
  
  fprintf(fp, "\n");
  fprintf(fp, "----------------------------------------------------------------------------\n");
  fprintf(fp, "Results of test 23 ( Comparator offset  noise for (N+1)-N for %s\n", csc_name);
  fprintf(fp, "----------------------------------------------------------------------------"
	  "\n\n");


  fprintf(fp, "------------------------------------------"
	      "------------------------------------------\n");
  fprintf(fp, " Strips, Layer 1, Layer 2, Layer 3 , Layer 4 , Layer 5, Layer 6 \n ");
  fprintf(fp, "----------------------------+----------------------+------"
	  "---------------\n");

  for (ilayer =0; ilayer <NLAYER; ilayer++)
    {
      for (istrip = 0; istrip < nstrips-1; istrip++)
	{
	  fprintf(fp, " %d  %2d-%2d   %9.2e  %9.2e  %9.2e %9.2e %9.2e %9.2e\n",
			  ilayer+1, istrip+1,istrip+2,
		  noise[0][istrip], noise[1][istrip],
		  noise[2][istrip], noise[3][istrip],
		  noise[3][istrip], noise[5][istrip]);
	}
    }

/* Trailer for list of bad channels */
  fprintf(fp, "------------------------------------------"
	      "------------------------------------------\n");

  fprintf(fp, "\nTotal number of bad channels = %d\n\n", bad_chan_sum);

/* Print mandatory summary statement */
  if (bad_chan_sum > 0) fprintf(fp, "Test result = Failed\n\n");
  else fprintf(fp, "Test result = OK\n\n");

/* Add the date and time */
  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp, "%s\n\n", timestring);

  fclose(fp);

 /* --------- file 03 -------------------*/

  sprintf(filename,"%s%s",datadir_translation,test_23_03);

/* Open result file in the temporary directory */
  fp = fopen(filename, "w");
  if(fp == NULL)
    {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
    }
  
  fprintf(fp, "\n");
  fprintf(fp, "----------------------------------------------------------------------------\n");
  fprintf(fp, "Results of test 23 ( Comparator offset for (N+1)-(N-1) for %s\n", csc_name);
  fprintf(fp, "----------------------------------------------------------------------------"
	  "\n\n");


  fprintf(fp, "------------------------------------------"
	      "------------------------------------------\n");
  fprintf(fp, " Strips, Layer 1, Layer 2, Layer 3 , Layer 4 , Layer 5, Layer 6 \n ");
  fprintf(fp, "----------------------------+----------------------+------"
	  "---------------\n");

  for (ilayer =0; ilayer <NLAYER; ilayer++)
    {
      for (istrip = 1; istrip < nstrips-1; istrip++)
	{
	  fprintf(fp, " %d  %2d-%2d   %9.2e  %9.2e  %9.2e %9.2e %9.2e %9.2e\n",
			  ilayer+1, istrip,istrip+2,
		  offset_2[0][istrip], offset_2[1][istrip],
		  offset_2[2][istrip], offset_2[3][istrip],
		  offset_2[3][istrip], offset_2[5][istrip]);
	}
    }

/* Trailer for list of bad channels */
  fprintf(fp, "------------------------------------------"
	      "------------------------------------------\n");

  fprintf(fp, "\nTotal number of bad channels = %d\n\n", bad_chan_sum);

/* Print mandatory summary statement */
  if (bad_chan_sum > 0) fprintf(fp, "Test result = Failed\n\n");
  else fprintf(fp, "Test result = OK\n\n");

/* Add the date and time */
  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp, "%s\n\n", timestring);

  fclose(fp);

  /* --------- file 04 -------------------*/

  sprintf(filename,"%s%s",datadir_translation,test_23_04);

/* Open result file in the temporary directory */
  fp = fopen(filename, "w");
  if(fp == NULL)
    {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
    }
  
  fprintf(fp, "\n");
  fprintf(fp, "----------------------------------------------------------------------------\n");
  fprintf(fp, "Results of test 23 ( Comparator offset  noise for (N+1)-(N-1) for %s\n", csc_name);
  fprintf(fp, "----------------------------------------------------------------------------"
	  "\n\n");


  fprintf(fp, "------------------------------------------"
	      "------------------------------------------\n");
  fprintf(fp, " Strips, Layer 1, Layer 2, Layer 3 , Layer 4 , Layer 5, Layer 6 \n ");
  fprintf(fp, "----------------------------+----------------------+------"
	  "---------------\n");

  for (ilayer =0; ilayer <NLAYER; ilayer++)
    {
      for (istrip = 1; istrip < nstrips-1; istrip++)
	{
	  fprintf(fp, " %d  %2d-%2d   %9.2e  %9.2e  %9.2e %9.2e %9.2e %9.2e\n",
			  ilayer+1, istrip,istrip+2,
		  noise_2[0][istrip], noise_2[1][istrip],
		  noise_2[2][istrip], noise_2[3][istrip],
		  noise_2[3][istrip], noise_2[5][istrip]);
	}
    }

/* Trailer for list of bad channels */
  fprintf(fp, "------------------------------------------"
	      "------------------------------------------\n");

  fprintf(fp, "\nTotal number of bad channels = %d\n\n", bad_chan_sum);

/* Print mandatory summary statement */
  if (bad_chan_sum > 0) fprintf(fp, "Test result = Failed\n\n");
  else fprintf(fp, "Test result = OK\n\n");

/* Add the date and time */
  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp, "%s\n\n", timestring);

  fclose(fp);

  /* --------- file 05 occupancy -------------------*/

  sprintf(filename,"%s%s",datadir_translation,test_23_05);

/* Open result file in the temporary directory */
  fp = fopen(filename, "w");
  if(fp == NULL)
    {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
    }
  
  fprintf(fp, "\n");
  fprintf(fp, "----------------------------------------------------------------------------\n");
  fprintf(fp, "Results of test 23 ( Comparator halfstrip occupancy  for %s\n", csc_name);
  fprintf(fp, "----------------------------------------------------------------------------"
	  "\n\n");


  fprintf(fp, "------------------------------------------"
	      "------------------------------------------\n");
  fprintf(fp, " Halfstrips, Layer 1, Layer 2, Layer 3 , Layer 4 , Layer 5, Layer 6 \n ");
  fprintf(fp, "----------------------------+----------------------+------"
	  "---------------\n");

  for (ilayer =0; ilayer <NLAYER; ilayer++)
    {
      for (ihs = 0; ihs < 2*nstrips; ihs++)
	{
	  fprintf(fp, " %d  %2d   %9.2e  %9.2e  %9.2e %9.2e %9.2e %9.2e\n",
			  ilayer, ihs,
		  occupancy[0][ihs], occupancy[1][ihs],
		  occupancy[2][ihs], occupancy[3][ihs],
		  occupancy[3][ihs], occupancy[5][ihs]);
	}
    }

/* Trailer for list of bad channels */
  fprintf(fp, "------------------------------------------"
	      "------------------------------------------\n");

  fprintf(fp, "\nTotal number of bad channels = %d\n\n", bad_chan_sum);

/* Print mandatory summary statement */
  if (bad_chan_sum > 0) fprintf(fp, "Test result = Failed\n\n");
  else fprintf(fp, "Test result = OK\n\n");

/* Add the date and time */
  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp, "%s\n\n", timestring);

  fclose(fp);

  free(filename);
  return 0;
}
