/*
 * test_27.c (in V1.2 test_22.c)
 * High-Statistics Cosmics test
 *
 * Author : Lisa Gorn      
 * Date   : 2 Jan 2001
 * Last modification: March 27, 2003 S.D.
 * Add the different types of chamber
 */

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <math.h>
#include "cfortran.h"
#include "hbook.h"
#include "daq_conf.h"
#include "csc_event.h"       /* upevt_                 */
#include "csc_parameters.h"  /* NLAYER, NSTRIP, etc... */
#include "one_page_plot.h"   /* STRIP_PLOT,...         */
#include "test_utils.h"      /* chamber labels         */
#include "test_params.h"     /* test config parameters */
#include "misc.h"            /* definition of data type logical */
#include "clctsim.h"         /* trigger simulation utilities */
#include "csc_geom.h"        /* more chamber geometry */

//#include "csc_unpacker.h"
//#include "emu/daq/reader/RawDataFile.h"

//add next 3 lines
//extern emu::daq::reader::RawDataFile *fd;
int event_num;
extern char file_name[1000];

#define  NHALFSTRIP          (2 * NSTRIP)
#define  NALCT_FIFO_TBINS    8
#define  NCLCT_FIFO_TBINS    7
#define  PRETRIG_ALG         2
#define  TRIG_ALG            3

/* Local function declarations */
int test_27_begin(int pass);
int test_27_event(int pass);
int test_27_end(int pass);
int test_27_finish(void);
void lfitw_(float *x, float *y, float *w, int *l, int *key, float *a, 
	    float *b, float *var);

int compsim(float sca[NLAYER][NSTRIP][16], int nsample, int clkphase, 
	    int cmode, int ctime, float thr, unsigned int comphits[NLAYER][NHALFSTRIP]);
int clusana(unsigned int comphits[NLAYER][NHALFSTRIP], int isegment);
int clusana2(unsigned int comphits[NLAYER][NHALFSTRIP], int isegment);
int clctsim(unsigned int comphits[NLAYER][NHALFSTRIP], int *trigbin_sca, 
	    int pretrig_alg, int trig_alg, short int *clct_key, short int *clct_pattern, 
	    short int *clct_valid);
int alctsim(int pretrig_time, short int *accel, short int *alct_key, 
	    short int *alct_pattern, short int *alct_valid, int nwires);
void gatfit_(int *key_strip, int *clct_time, float mean[NLAYER][NSTRIP], 
	     float rms[NLAYER][NSTRIP], int *ilayer, 
	     float *strip_width, int *iseg, float *xclus, 
	     float *xclus_error, float *xclus_chisq, float *qclus, float *tclus,
	     float *fclus, int *status, int *pass);
void trackfit_(float xclus[NLAYER], float xerror[NLAYER], float xalign[NLAYER],
	       float z[NLAYER], int layer_ok[NLAYER], int *rlayer, float *emm, float *bee, 
	       float *track_chisq, float xtrack[NLAYER], float resid[NLAYER], 
	       int *nlayers_in_fit, int *trackfit_ok);
void book_histos_27f_(int *pass, int *stage, int *nwires);
void strip_alignment_(float c[NLAYER][2], float var[NLAYER], int *nwires);
void strip_alignment_x_(float x_wg[NWIRES_MAX], float c[NLAYER][2], float var[NLAYER], int *nwires);
void crossana(float xt[NLAYER][NCFEB][2], float var[NLAYER][NCFEB]);
void crossfill(int ilayer, float xclus, float tclus, 
	       float sca[NLAYER][NSTRIP][16]);
float scintana(int *s1, int *s2);

// next subroutines are in csc_geom.c file 
/* prototype for  calculation of strips width csc_type and wg counts from 1 */
float w_str(int csc_type,int wg);
/* prototype for  calculation of wg center, csc_type and  wg counts from 1 */
float r_cntr_wg(int csc_type,int wg);
float r_al_pin(int csc_type);
/*prototype for calculations segment of WG, everything counts from 1 */
int segm_wg(int wire_group, int csc_type);
/*prototype for checkig that WG is on boundary of HV segments (not edges of CSC), everything counts from 1 */
logical at_bound_wg(int wire_group, int csc_type);

/* Global variables local to this test */
static int     pretrig_time;
static float   mean[NLAYER][NSTRIP], rms[NLAYER][NSTRIP];
static FILE    *fp;
static FILE    *fp2;
static float   z[NLAYER] = { -50.8, -25.4, 0., 25.4, 50.8, 76.2 };
static float   c[NLAYER][2] = {
  { 0.,  0. }, { 0.,  0. },
  { 0.,  0. }, { 0.,  0. },
  { 0.,  0. }, { 0.,  0. } };

char test_27_file_el[]="/test_results/temp/test_27.results";
char test_27_file_his_template[]="%s/test_results/temp/test27_%d.his";
char test_27_file_xclust_el[]="/test_results/temp/xclusters.dat";

static int nwires;
static int nstrs, ncfebs;
/*******************************************************************************
   Define functions and parameters for FAST site test 27
*******************************************************************************/

int test_27_init(void)
{
    int l;
    test_begin  = test_27_begin;
    test_event  = test_27_event;
    test_end    = test_27_end;
    test_finish = test_27_finish;

    num_passes = 4;
    num_events[0] = 50;
    num_events[1] = 5000;
    num_events[2] = 125000;
    num_events[3] = 125000;
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

    switch (csc_type)
    {
    case 2:  nstrs = 64; ncfebs = 4; break;
    default: nstrs = 80; ncfebs = 5; break;
    }

    printf ("csc_type = %d, nwires = %d\n", csc_type, nwires);
    return 0;
}

/*******************************************************************************
   Book histograms for FAST site test 27                                       
*******************************************************************************/

int test_27_begin(int pass)
  {
  int     j, hid, ilayer, istrip, iwg, nbins;
  char    htitle[80];
  char *filename;

  if (pass == 0)
    {
    filename = malloc(strlen(datadir_translation) + 
		      strlen(test_27_file_el) + 1 );
    if (filename == NULL)
      {
      printf("malloc() failed on filename.\n");
      return 1;
      }
    sprintf(filename,"%s%s",datadir_translation,test_27_file_el);

/* Open result file in the temporary directory */
    fp = fopen(filename, "w");
    if(fp == NULL)
      {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
      }
    free(filename);

    fprintf(fp, "------------------------------------------------------------"
	    "\n");
    fprintf(fp, "Results of test 27 (High Statistics Cosmics) for %s\n", 
	    csc_name);
    fprintf(fp, "------------------------------------------------------------"
	    "\n");

    nbins = 200;
    printf("Booking pass 0 histograms for test 27\n");
    HBOOK1( 10, "ALCT hit times", 32, 0., 32., 0.);
    for (ilayer=0; ilayer < NLAYER; ilayer++)
      {
      for(istrip=0; istrip < NSTRIP; istrip++)
	{
        hid = 100 * (ilayer + 1) +  (istrip + 1);
        sprintf( htitle, "Layer %d Strip %d", ilayer+1, istrip+1);
        HBOOK1( hid, htitle, nbins, 0., 4096., 0.);
	}
      }
    }

  else if (pass == 1)
    {
    HDELET(0);
    nbins = 100;
    printf("Booking pass 1 histograms for test 27\n");
    HBOOK1( 1, "mean", 100, 400., 700., 0.);
    HBOOK1( 2, "rms", 100, 0., 10., 0.);
    for (ilayer=0; ilayer < NLAYER; ilayer++)
      {
      for(istrip=0; istrip < NSTRIP; istrip++)
	{
        hid = 100 * (ilayer + 1) +  (istrip + 1);
        sprintf( htitle, "Layer %d Strip %d", ilayer+1, istrip+1);
        j = (int)mean[ilayer][istrip];
        HBOOK1( hid, htitle, nbins, (float)(j+0.5-nbins/2), 
		(float)(j+0.5+nbins/2), 0.0);
	}
      }
    }

  else if (pass == 2)
    {
    filename = malloc(strlen(datadir_translation) + 
		      strlen(test_27_file_xclust_el) + 1 );
    if (filename == NULL)
      {
      printf("malloc() failed on filename.\n");
      return 1;
      }
    sprintf(filename,"%s%s",datadir_translation,test_27_file_xclust_el);

/* Open result file in the temporary directory */
    fp2 = fopen(filename, "w");
    if(fp2 == NULL)
      {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
      }
    free(filename);

    HDELET(0);
    HBOOK1( 1, "Key wiregroup occupancy", nwires, 0.5, nwires+.5, 0.);
    HBOOK1( 2, "Key halfstrip occupancy", 160, 0.5, 160.5, 0.);
    HBOOK1( 3, "Key distrip occupancy", 40, 0.5, 40.5, 0.);
    HBOOK1( 4, "Track slope, radians", 100, -1.00, 1.00, 0.);
    HBOOK2( 5, "Scint delta-t vs track angle, radians", 100, -1.00, 1.00, 
	    100, -100., 100., 0.);
    HBOOK2( 6, "Top scint vs bottom scint", 15, 0., 15., 15, 0., 15., 0.);
    HBOOK2( 7, "cluster var vs qclus", 50, 0., 20., 50, 0., 120., 0.);
    HBOOK1( 8, "fclus", 100, 0.5, 1.5, 0.);
    HBOOK2( 9, "qclus vs wg", nwires, -0.5, nwires-0.5, 50, 0., 20., 0.);
    HBOOK2( 10, "fclus vs wg", nwires, -0.5, nwires-0.5, 50, 0.75, 1.25, 0.);

//  for (j = 0; j < 12; j++) {
//    hid = 40 + j + 1;
//    sprintf( htitle, "delta-t, scint %d", j+1);
//    HBOOK2( hid, htitle, 100, -1.00, 1.00, 100, -100., 100., 0.);
//  }

/* Cluster study histograms (not a standard part of test 27) */ /*
   HBOOK1( 13, "isegment occupancy",  10, -2.5, 7.5, 0.);
   HBOOK1( 14, "jmax distribution",  16, -0.5, 15.5, 0.);
   HBOOK1( 20, "t1",  100, -0.5, 0.5, 0.);
   for (j = 0; j < 5; j++)
   {
   hid = 20 + j + 1;
   sprintf( htitle, "t1, segment %d", j);
   HBOOK1( hid, htitle,  100, -0.5, 0.5, 0.);
   }
   HBOOK1( 26, "tpeak distr, seg 2, 50 ns units", 100, -1., 1., 0.);
   HBOOK1( 27, "tpeak distr, seg 2, sums ok, 50 ns units", 100, -1., 1., 0.);


   HBOOK1( 30, "SCA shape, peak strip, 50 ns units", 500, -4.0, 6.0, 0.);
   HBOOK1( 31, "SCA shape, neighbor, 50 ns units", 500, -4.0, 6.0, 0.);
   HBOOK1( 32, "SCA shape, next neighbor, 50 ns units", 500, -4.0, 6.0, 0.);
								*/

/* For clusana */ /*
   HBOOK1( 10, "sum, adc counts",  101, -100., 10000., 0.);
   HBOOK1( 11, "sum/sum5",  100, 0., 2., 0.);
   HBOOK2( 12, "sum/sum5 vs sum",  101, -100., 10000., 100, 0., 2., 0.);

   HBOOK2( 100, "C/S vs (L-R)/S, adc counts", 100, 0., 1., 100, 0., 1., 0.);
   for (j = 0; j < 5; j++)
   {
   hid = 100 + j + 1;
   sprintf( htitle, "C/S vs (L-R)/S, segment %d", j+1);
   HBOOK2( hid, htitle, 100, 0., 1., 100, 0., 1., 0.);
   }
   HBOOK2( 111, "C/S vs (L-R)/S, adc counts",  100, 0., 1., 100, 0., 1., 0.);
   HBOOK2( 112, "C/S vs (L-R)/S, adc counts",  100, 0., 1., 100, 0., 1., 0.);
		  */

/* For clusana2 */ /*
   HBOOK1( 10, "sum4, adc counts",  101, -100., 10000., 0.);
   HBOOK1( 11, "sum2/sum6",  100, 0., 2., 0.);
   HBOOK2( 12, "sum2/sum6 vs sum4",  101, -100., 10000., 100, 0., 2., 0.);

   HBOOK2( 100, "sum2/sum4 vs (R-L)/sum4, adc counts", 
   100, 0., 1., 100, 0.5, 1.5, 0.);
   for (j = 0; j < 5; j++)
   {
   hid = 100 + j + 1;
   sprintf( htitle, "sum2/sum4 vs (R-L)/sum4 segment %d", j+1);
   HBOOK2( hid, htitle, 100, 0., 1., 100, 0.5, 1.5, 0.);
   }
   HBOOK2( 111, "sum2/sum4 vs (R-L)/sum4, adc counts",  
   100, 0., 1., 100, 0.5, 1.5, 0.);
   HBOOK2( 112, "sum2/sum4 vs (R-L)/sum4, adc counts",  
   100, 0., 1., 100, 0.5, 1.5, 0.);
		   */

/* End of cluster study histograms */

/* Clusterfit x-occupancy histograms vs segment */ 
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      for (j = 0; j < 5; j++)
	{
        hid = 400 + 10 * (ilayer + 1) + (j + 1);
        sprintf( htitle, "L%d seg%d clusterfit x-occu", ilayer+1, j+1);
        HBOOK1( hid, htitle, 25, -0.5, 0.5, 0.);

        hid = 500 + 10 * (ilayer + 1) + (j + 1);
        sprintf( htitle, "L%d seg%d clusterfit x-occu, theta lt .05", ilayer+1, 
		 j+1);
        HBOOK1( hid, htitle, 25, -0.5, 0.5, 0.);
	}
      }

/* Crosstalk 2-D histos (non-std for looking at small amplitude anomaly) */ /*
   for (ilayer = 0; ilayer < NLAYER; ilayer++)
   {
   for (j = 2; j < 5; j++)
   {
   hid = 500 + 10 * (ilayer + 1) + (j + 1);
   sprintf( htitle, "L%d cfeb%d crosstalk vs Qsum", ilayer+1, j+1);
   HBOOK2( hid, htitle, 50, 0., 2000., 50, -150., 150., 0.);
   }
   }
									    */

/* Trackfit residuals */
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      for (iwg = 0; iwg < nwires; iwg++)
	{
        hid = 1000 + 120 * (ilayer+1) + iwg + 1;
        sprintf( htitle, "Trackfit residual, L%d, wg%2d", ilayer+1, iwg+1);
        HBOOK1( hid, htitle, 100, -0.40, 0.40, 0.);
	}
      }

    j = 0;
    book_histos_27f_(&pass, &j, &nwires);
    }

  else if (pass == 3)
    {
    filename = malloc(strlen(datadir_translation) + 
		      strlen(test_27_file_xclust_el) + 1 );
    if (filename == NULL)
      {
      printf("malloc() failed on filename.\n");
      return 1;
      }
    sprintf(filename,"%s%s",datadir_translation,test_27_file_xclust_el);

/* Open result file in the temporary directory */
    fp2 = fopen(filename, "r");
    if(fp == NULL)
      {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
      }
    free(filename);

    HDELET(0);

    HBOOK1( 101, "Trackfit chisq/ndf", 100, 0., 50., 0.);
    HBOOK1( 102, "Nlayers in trackfit", 10, 0., 10., 0.);
    HBOOK1( 103, "Theta ", 100, -5., 5., 0.);
    HBOOK1( 104, "KeyWG ", 120, 0., 120., 0.);

    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      hid = 110 + ilayer + 1;
      sprintf( htitle, "Cluster fit chisq, L%d", ilayer+1);
      HBOOK1( hid, htitle, 100, 0., 200., 0.);

      hid = 120 + ilayer + 1;
      sprintf( htitle, "L%d strip efficiency numerator", ilayer+1);
      HBOOK1( hid, htitle, 80, -0.5, 79.5, 0.);

      hid = 130 + ilayer + 1;
      sprintf( htitle, "L%d strip efficiency denominator", ilayer+1);
      HBOOK1( hid, htitle, 80, -0.5, 79.5, 0.);

      hid = 140 + ilayer + 1;
      sprintf( htitle, "L%d comparator efficiency numerator", ilayer+1);
      HBOOK1( hid, htitle, 160, 0., 160., 0.);

      hid = 150 + ilayer + 1;
      sprintf( htitle, "L%d comparator efficiency denominator", ilayer+1);
      HBOOK1( hid, htitle, 160, 0., 160., 0.);

      hid = 170 + ilayer + 1;
      sprintf( htitle, "L%d qclus from gatfit", ilayer+1);
      HBOOK1( hid, htitle, 100, 0., 20., 0.);
      }

/* Trackfit residuals */
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      for (iwg = 0; iwg < nwires; iwg++)
	{
        hid = 1000 + 120 * (ilayer+1) + iwg + 1;
        sprintf( htitle, "Trackfit residual, L%d, wg%2d", ilayer+1, iwg+1);
        HBOOK1( hid, htitle, 100, -0.40, 0.40, 0.);
	}

/* Clusterfit residuals vs wire group and local x */
/**** To be replaced with profile hists ? ****/
      for (iwg = 0; iwg < nwires; iwg++)
	{
        for (j = 0; j < 5; j++)
	  {
          hid = 20000 + 1200 * (ilayer + 1) + 10 * (iwg + 1) + j;
          sprintf( htitle, "L%d wg%d xbin %d residuals", ilayer+1, iwg+1, j);
          HBOOK1( hid, htitle, 50, -0.4, 0.4, 0.);
	  }
	}
      }

    j = 0;
    book_histos_27f_(&pass, &j, &nwires);

    }
  return 0;
  }

/*******************************************************************************
   Do analysis and fill histograms for FAST site test 27
*******************************************************************************/

int test_27_event(int pass)
  {
  int              j, k, n, distrip, hid, ihs, ihs1, ilayer, istrip, isample;
  int              key_wg, rlayer, status;
  int              ifeb, isegment, trigbin_sca;
  int              dummy, layer_ok[NLAYER], nlayers_in_fit;
  short int        clct_key[2], clct_pattern[2], clct_valid[2];
  short int        accel[2], alct_key[2], alct_pattern[2], alct_valid[2];
  static int       first_call = 1;
  unsigned int     comphits[NLAYER][NHALFSTRIP];
  unsigned long    data, triad;
  static long      file_event_number;
  float            xclus[NLAYER], xclus_error[NLAYER], xclus_chisq[NLAYER];
  float            dx, scint_dt, xalign[NLAYER], xcomp, xdummy, xst;
  float            fclus[NLAYER], qclus[NLAYER], tclus[NLAYER];
  float            emm, bee, track_chisq, xtrack[NLAYER], resid[NLAYER], var;
  float            sca[NLAYER][NSTRIP][16], stripwidth, theta;
  float            strip_width;
  logical          at_boundary, comp_hit_found, pulse_on, trackfit_ok;

  if (first_call)
    {
    first_call = 0;
    }

/* The first pass is used to histogram the ALCT hit time distribution 
 * in order to get the pretrigger time bin needed by alctsim.
 */

  if (pass == 0)
    {

/* Look for ALCT hits */
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      for (j = 0; j < nwires; j++)
	{
        if ((data = upevt_.alct_dump[ilayer][j]))
	  {
          pulse_on = _FALSE;
          for (k = 0; k < 32; k++)
	    {
            if ((data & 0x1) && !pulse_on)
	      {
              HFILL(10, (float)k, 0., 1.);
              pulse_on = _TRUE;
	      }
            else if (!(data & 0x1)) pulse_on = _FALSE;
            if ((data = data >> 1) == 0) break;
	    }
	  }
	}
      }
    }

/* The first pass is used also to make a rough calculation of the pedestals in
 * order to find the right range to use when booking the pedestal histograms
 * for the next pass.
 * The second pass is used to calculate pedestals more exactly. */
  if ((pass == 1) || (pass == 0))
    {

/* Look for active cfebs */
    for (ifeb=0; ifeb < NCFEB; ifeb++)
      {
      if (upevt_.active_febs[ifeb] > 0)
	{

/* Fill histograms */
        for (ilayer = 0; ilayer < NLAYER; ilayer++)
	  {
          for (istrip = 16 * ifeb; istrip < 16 * (ifeb + 1); istrip++)
	    {
            hid = 100*(ilayer + 1) + (istrip + 1);
//          for (isample = 0; isample < upevt_.nsca_sample; isample++) 
            for (isample = 1; isample < 3; isample++){ 
	      HFILL(hid, (float )(upevt_.sca[ilayer][istrip][isample]), 0.0, 
		    1.0); 
	    }
	    }
	  }
	
      }
    }
    }
  else if (pass == 2)
    {

/* Do wire track-finding to find the radial position of the track. Use the
 * ALCT algorithm for track-finding. Continue with cluster analysis if
 * exactly one track is found.
 */

    if ((status = alctsim(pretrig_time, accel, alct_key, alct_pattern, 
			  alct_valid, nwires)))
      {
      printf("call to alctsim failed; skipping event %ld\n",
	     upevt_.event_number);
      return -1;
      }

    if (!alct_valid[0] || alct_valid[1]) isegment = -1; 
    else isegment = segm_wg((int)(alct_key[0]+1), csc_type);
    /*
    else if (alct_key[0] < 16) isegment = 0;
    else if (alct_key[0] < 28) isegment = 1;
    else if (alct_key[0] < 40) isegment = 2;
    else if (alct_key[0] < 52) isegment = 3;
    else isegment = 4;
    */

/* Must have a wire coordinate to fix the stripwidth in the clusterfit */
    if (isegment < 0)
      {
/*     	  printf ("Segment <0, key_wg is%d ", alct_key[0]);
	  if (!alct_valid[0]) printf("Skipping event %ld with no ALCT\n", 
	                             upevt_.event_number);
	  if (alct_valid[1]) printf("Skipping event %ld with two ALCT's\n", 
	                             upevt_.event_number); 
*/
      return 0;
      }

/* Fill anode timing histogram */
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      for (j = alct_key[0] - 3; j <= alct_key[0] + 3; j++)
	{
        if (j < 0 || j >= nwires) continue;
        if (upevt_.alct_dump[ilayer][j])
	  {
          for (k = 0; k < NALCT_FIFO_TBINS; k++)
	    {
	    if (upevt_.alct_dump[ilayer][j] & (1 << k)) 
	      HFILL(60 + ilayer + 1, (float)j, (float)k, 1.);
	    }
	  }
	}
      }

/* Now start strip analysis; use the halfstrip algorithm to find clusters, 
 * and the clct algorithm to find tracks 
 */
/* Subtract approximate pedestals */
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      for (istrip = 0; istrip < NSTRIP; istrip++)
	{
        for (j = 0; j < upevt_.nsca_sample; j++) sca[ilayer][istrip][j] = 
				   upevt_.sca[ilayer][istrip][j] - mean[ilayer][istrip];
	}
      }

/* Simulate comparators */

    compsim(sca, 16, 0, 2, 2, 15, comphits);

/* Do cluster studies */
//  clusana(comphits, isegment);
//  clusana2(comphits, isegment);

/* Simulate CLCT logic */

    clctsim(comphits, &trigbin_sca, PRETRIG_ALG, TRIG_ALG, clct_key, 
	    clct_pattern, clct_valid);
/* Prepare to do cluster fits */

    if (clct_valid[0])
      { 
/* Save the (simulated) trigger data for this event to xclusters.dat */
      fprintf(fp2, "%7ld %2d", upevt_.event_number, trigbin_sca);
      for (j = 0; j < 2; j++)
	{
        fprintf(fp2, " %hd %3hd %hd %2hd %3hd %3hd %2hd", accel[j], alct_key[j], 
		alct_pattern[j], alct_valid[j], clct_key[j], clct_pattern[j], 
		clct_valid[j]);
	}
      fprintf(fp2, "\n");

/* Fill some cross-check histograms */
      key_wg = alct_key[0];
      HFILL(1, (float)(alct_key[0]+1), 0., 1.);
      if (hs(clct_pattern[0])) HFILL(2, (float)(clct_key[0]+1), 0., 1.);
      else HFILL(3, (float)(clct_key[0]+1), 0., 1.);

/* Approximate pulse locations are determined by the trigger pattern and the
 * key halfstrip and/or distrip; the meaning of key is algorithm-dependent */
      for (ilayer = 0; ilayer < NLAYER; ilayer++)
	{
        if (TRIG_ALG == 1) istrip = (clct_key[0] + ilayer % 2) / 2;
        else if (TRIG_ALG == 2) istrip = 2 * clct_key[0] + 1;
        else if (TRIG_ALG == 3)
	  {
          if (hs(clct_pattern[0])) istrip = (clct_key[0] + ilayer % 2 + 
					     so(clct_pattern[0],ilayer)) / 2;
          else istrip = 2 * (clct_key[0] + so(clct_pattern[0],ilayer)) + 1;
	  } 
/* Call gatfit for one cluster */

	strip_width= w_str(csc_type, key_wg+1); 
        gatfit_(&istrip, &trigbin_sca, mean, rms, &ilayer, 
	        &strip_width, &isegment,
		&xclus[ilayer], &xclus_error[ilayer], &xclus_chisq[ilayer], 
		&qclus[ilayer], &tclus[ilayer], &fclus[ilayer], &status, &pass);

/* Debug */
//      printf("L%d s%2d has xclus=%f, xclus_chisq=%f, tclus=%f, "
//       "status=%d\n", ilayer+1, istrip+1, xclus[ilayer], 
//        xclus_chisq[ilayer], tclus[ilayer], status);

        HFILL(7, qclus[ilayer], xclus_chisq[ilayer], 1.);
        HFILL(8, fclus[ilayer], 0., 1.);
        HFILL(9, key_wg, qclus[ilayer], 1.);
        HFILL(10, key_wg, fclus[ilayer], 1.);

/* Save the fit parameters to xclusters.dat */
        fprintf(fp2, "%8.3e %8.3e %8.3e %1d\n", xclus[ilayer], 
		xclus_error[ilayer], xclus_chisq[ilayer], status);

/* Make "long-range" (but within chip) crosstalk plots */
        if (status == 3) crossfill(ilayer, xclus[ilayer], tclus[ilayer], sca);

/* Redefine layer_ok for trackfit */
        layer_ok[ilayer] = (status == 3) && (xclus_chisq[ilayer] < 100);
	}

/* Do track fit (unweighted) */
      rlayer = -1;
      for (ilayer = 0; ilayer < NLAYER; ilayer++)
	{
        xclus_error[ilayer] = 0.1;
        xalign[ilayer] = 0;
	}

/* Test the alignment correction */ /*
   for (ilayer = 0; ilayer < NLAYER; ilayer++)
   xalign[ilayer] = c[ilayer][0] + c[ilayer][1] * (key_wg + 1);
				    */
       
/* Do the track fit */
      trackfit_(xclus, xclus_error, xalign, z, layer_ok, &rlayer, &emm, &bee, 
		&track_chisq, xtrack, resid, &nlayers_in_fit, &trackfit_ok);

//    printf("pass 2 trackfit for Event %ld returns x(L3)=%f, chisq=%f, 
//     nlayers_in_fit=%d, trackfit_ok=%d\n", upevt_.event_number, xtrack[2], 
//     track_chisq, nlayers_in_fit, trackfit_ok);

      if (trackfit_ok)
	{
        if (nlayers_in_fit >= 4)
	  {
          for (ilayer = 0; ilayer < NLAYER; ilayer++)
	    {
            if (layer_ok[ilayer])
	      {
              hid = 1000 + 120 * (ilayer+1) + key_wg + 1;
              HFILL(hid, resid[ilayer], 0., 1.);
	      }
	    }

	  // S.D. March 12, 2003
	  //          stripwidth = (R_WG_0 + WG_WIDTH * key_wg) * STRIP_ANGLE;
          stripwidth = w_str(csc_type, key_wg+1);

          theta = stripwidth * emm; //????
          HFILL(4, theta, 0., 1.);

/* We are not reading scintillators with setup of version 4
          scint_dt = scintana(&j, &k);
          HFILL(5, theta, scint_dt, 1.);
          if (abs(j-k) <= 1) HFILL(40000 + (100 * j) + k, theta, scint_dt, 1.);
	  */
/* Clusterfit x-occupancy histos */
          for (ilayer = 0; ilayer < NLAYER; ilayer++)
	    {
            if (status == 3 && xclus[ilayer] > 34. && xclus[ilayer] < 77. 
		&& (!(ilayer == 4) || xclus[ilayer] < 45 || xclus[ilayer] > 66.))
	      {
              hid = 400 + 10 * (ilayer + 1) + (isegment + 1);
              HFILL(hid, xclus[ilayer] - rint(xclus[ilayer]), 0., 1.);
              if (fabs(theta) < 0.05)
		{
                hid = 500 + 10 * (ilayer + 1) + (isegment + 1);
                HFILL(hid, xclus[ilayer] - rint(xclus[ilayer]), 0., 1.);
		}
	      }
	    }

	  }
	}
      }   /* end of "if valid CLCT" */

    }  /* End of pass 2 */
  

  else if (pass == 3)
    {
	  HFILL(4,2., 0., 1.);

/* Redo track fits with alignment corrections, to get strip resolution and
 * efficiency. Also look at comparator resolution and efficiency. 
 */

/* Read the (simulated) trigger results for this event from xclusters.dat */
      if (upevt_.event_number == 1) file_event_number =0; 
      if (file_event_number < upevt_.event_number)
	{
	  n = fscanf(fp2, "%7ld ", &file_event_number);	  
	}

      if (file_event_number > upevt_.event_number) return 0;
      
    fscanf(fp2, "%2d ", &trigbin_sca);

    for (j = 0; j < 2; j++)
      {
	n = fscanf(fp2, " %hd %3hd %hd %2hd %3hd %3hd %2hd", &accel[j], &alct_key[j], 
	     &alct_pattern[j], &alct_valid[j], &clct_key[j], &clct_pattern[j], 
	     &clct_valid[j]);
	//      printf("j = %d, n= %d, clct_key = %d", j,n, clct_key[j]); 
      }
	  HFILL(1,(float)
		(alct_valid[0]+2*alct_valid[1]
		+4*clct_valid[0]+8*clct_valid[1]), 0., 1.);
    for (j = 0; j < NLAYER; j++)
      {
      n = fscanf(fp2, "%e %e %e %d", &xdummy, &xdummy, &xdummy, &dummy); 
      if (n != 4) printf("Error reading xclusters.dat for event %ld: "
			 "req/got 4/%d words\n", upevt_.event_number, n);
      }

/* Require exactly one alct track and at least one clct track */
    if (!alct_valid[0] || alct_valid[1] || !clct_valid[0]) return 0;
	  HFILL(2,(float)
		(alct_valid[0]+2*alct_valid[1]
		+4*clct_valid[0]+8*clct_valid[1]), 0., 1.);

    key_wg = alct_key[0];

    isegment = segm_wg(key_wg+1, csc_type)-1;
    at_boundary = at_bound_wg(key_wg+1, csc_type);

/* Approximate pulse locations are determined by the trigger pattern and the
 * key halfstrip and/or distrip; the units of key is algorithm-dependent.
 */
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      if (TRIG_ALG == 1) istrip = (clct_key[0] + ilayer % 2) / 2;
      else if (TRIG_ALG == 2) istrip = 2 * clct_key[0] + 1;
      else if (TRIG_ALG == 3)
	{
        if (hs(clct_pattern[0])) istrip = (clct_key[0] + ilayer % 2 + 
					   so(clct_pattern[0],ilayer)) / 2;
        else istrip = 2 * (clct_key[0] + so(clct_pattern[0],ilayer)) + 1;
	} 

/* Call gatfit for one cluster */
//    printf("calling gatfit for L%d strip %2d, tpeak %2d, key_wg=%d\n", 
//     ilayer+1, istrip+1, trigbin_sca, key_wg);

      strip_width= w_str(csc_type, key_wg+1); 
      gatfit_(&istrip, &trigbin_sca, mean, rms, &ilayer, 
	      &strip_width, &isegment,
	      &xclus[ilayer], &xclus_error[ilayer], &xclus_chisq[ilayer], 
	      &qclus[ilayer], &tclus[ilayer], &fclus[ilayer], &status, &pass);

/* Debug */
//    printf("L%d s%2d has xclus=%f, xclus_chisq=%f, tclus=%f, status=%d\n", 
//     ilayer+1, istrip+1, xclus[ilayer], xclus_chisq[ilayer], tclus[ilayer],
//     status);

      if (status == 3)
	{
	  HFILL(110+ilayer+1, xclus_chisq[ilayer], 0., 1.);
	}
      layer_ok[ilayer] = (status == 3) && (xclus_chisq[ilayer] < 100);
      }

/* Do track fit (unweighted) */ /* Temporary for x-occupancy study */ /*
   for (ilayer = 0; ilayer < NLAYER; ilayer++)
   {
   xclus_error[ilayer] = 0.1;
   }
								      */

/* Do track fit (weighted) */
    rlayer = -1;
    for (ilayer = 0; ilayer < NLAYER; ilayer++) 
      xalign[ilayer] = c[ilayer][0] + c[ilayer][1] * (key_wg + 1);

    trackfit_(xclus, xclus_error, xalign, z, layer_ok, &rlayer, &emm, &bee, 
	      &track_chisq, xtrack, resid, &nlayers_in_fit, &trackfit_ok);

//  printf("pass 3 trackfit for Event %ld returns x(L3)=%f, chisq=%f, 
//   nlayers_in_fit=%d, trackfit_ok=%d\n", upevt_.event_number, xtrack[2], 
//   track_chisq, nlayers_in_fit, trackfit_ok);

    if (trackfit_ok)
      {
      if (nlayers_in_fit > 2)
	{
        var = track_chisq / (nlayers_in_fit - 2);
        HFILL(101, var, 0., 1.);
	}
      HFILL(102, (float)nlayers_in_fit, 0., 1.);

/* Clusterfit x-occupancy histos */
      if (nlayers_in_fit >= 4)
	{
	  // S.D. March 12, 2003
	  //          stripwidth = (R_WG_0 + WG_WIDTH * key_wg) * STRIP_ANGLE;
          stripwidth = w_str(csc_type, key_wg+1);

          theta = stripwidth * emm; //????

        for (ilayer = 0; ilayer < NLAYER; ilayer++)
	  {
          if (status == 3 && xclus[ilayer] > 34. && xclus[ilayer] < 77. 
	      && (!(ilayer == 4) || xclus[ilayer] < 45 || xclus[ilayer] > 66.))
	    {
            hid = 400 + 10 * (ilayer + 1) + (isegment + 1);
            HFILL(hid, xclus[ilayer] - rint(xclus[ilayer]), 0., 1.);
            if (fabs(theta) < 0.05)
	      {
              hid = 500 + 10 * (ilayer + 1) + (isegment + 1);
              HFILL(hid, xclus[ilayer] - rint(xclus[ilayer]), 0., 1.);
	      }
	    }
	  }
	}

/* Plot comparator residuals and efficiency */
      if (var < 20 && nlayers_in_fit >= 4)
	{
        for (ilayer = 0; ilayer < NLAYER; ilayer++)
	  {
          ihs = rint(2 * xtrack[ilayer] + 0.5);
          distrip = ihs / 4;
          comp_hit_found = _FALSE;

          for (j = distrip - 1; j <= distrip + 1; j++)
	    {
            if (j < 0 || j >= 0.5 * NSTRIP) continue;
            if ((triad = upevt_.clct_dump[ilayer][j]) != 0)
	      {
              for (k = 0; k < NCLCT_FIFO_TBINS - 2; k++)
		{
                if (triad & (1 << k))
		  {
                  ihs1 = 4 * j;
                  if (triad & (1 << (k + 1))) ihs1 += 2;
                  if (triad & (1 << (k + 2))) ihs1 += 1;

                  if (abs(ihs1 - ihs) <= 3)
		    {
                    xcomp = -0.25 + 0.50 * ihs;
                    dx = xcomp - xtrack[ilayer];
                    HFILL(160 + ilayer + 1, (float)ihs, dx, 1.);
                    if (abs(dx) < 0.5) comp_hit_found = _TRUE;
		    }
                  break;
		  }
		}
	      }
	    }

          HFILL(150+ilayer+1, (float)ihs, 0., 1.);
          if (comp_hit_found) HFILL(140+ilayer+1, (float)ihs, 0., 1.);
	  }
	}
      }

/* Do 5-layer trackfits; plot clusterfit residuals and efficiency */
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      trackfit_(xclus, xclus_error, xalign, z, layer_ok, &ilayer, &emm, 
		&bee, &track_chisq, xtrack, resid, &nlayers_in_fit, &trackfit_ok);
      if (trackfit_ok && nlayers_in_fit >= 4)
	{
        if ((var = track_chisq / (nlayers_in_fit - 2)) < 20)
	  {
          xst = xtrack[ilayer] - (int)(xtrack[ilayer] + 0.5);
          hid = 300 + 10 * (ilayer + 1) + (isegment + 1);
          if (!at_boundary) HFILL(hid, xst, resid[ilayer], 1.);
	  
	  // S.D. May 2 2003
          HFILL(103 , theta, 0., 1.);
	  //          if (fabs(theta) < 0.1)
	    {
            j = 10 * fabs(xst);
            if (j > 4) j = 4;
            hid = 20000 + 1200 * (ilayer + 1) + 10 * (key_wg + 1) + j;
            HFILL(hid, resid[ilayer], 0., 1.);
	    HFILL(104 , (float)key_wg, 0., 1.);
	    }

          HFILL(130 + (ilayer + 1), xtrack[ilayer], 0., 1.);
          if (fabs(resid[ilayer]) < 0.5) HFILL(120 + (ilayer + 1), 
					       xtrack[ilayer], 0., 1.);
          HFILL(170 + (ilayer + 1), qclus[ilayer], 0., 1.);
	  }
	}
      }

    }  /* End of pass 3 */
  return 0;
  }


/*******************************************************************************
   End-of-run analysis for FAST site test 27
*******************************************************************************/

#define AFEB_HIT_LOWER_LIMIT     -0.5
#define AFEB_HIT_UPPER_LIMIT     +0.5
#define COMP_RES_LOWER_LIMIT     -0.03
#define COMP_RES_UPPER_LIMIT     +0.03
#define COMP_EFF_LOWER_LIMIT      0.9
#define COMP_EFF_UPPER_LIMIT     +1.0
#define STRIP_EFF_LOWER_LIMIT    +0.65
#define STRIP_EFF_UPPER_LIMIT    +0.95

int test_27_end(int pass)
  {
  int          i, j, k, n, hid, ilayer, iseg, key_wg, nok;
  int          lok[NLAYER], nlayers_in_fit, xstat[NLAYER];
  short int    alct_key[2], dummy;
  long int     evtnum;
  float        xchisq, xclus[NLAYER], xerr[NLAYER], xtrack[NLAYER];
  float        emm, bee, track_chisq, resid[NLAYER], xalign[NLAYER], xst;
  float        ctest[NLAYER][2], limits[7];
  float        denom[2*NSTRIP], numer[2*NSTRIP];
  float        xt[NLAYER][NCFEB][2], y_avg;
  float        var[NLAYER], var2[NLAYER][NCFEB]; 
  float        y[NLAYER*NHALFSTRIP], yerr[NLAYER*NHALFSTRIP];
  logical      at_boundary, trackfit_ok;
  char *filename,*file_his_name;
  float        x_wg[NWIRES_MAX];

  printf("Beginning pass %d end-of-run analysis\n", pass);

  if (pass == 0)
    {
    calc_peds_(0, mean, rms);
    pretrig_time = rint(0.5 + HSTATI(10, 1, "HIST", 1));
    printf("pretrig_time = %d\n", pretrig_time);

    /* Save histograms */
    file_his_name = malloc(strlen(datadir_translation) + 
			   strlen(test_27_file_his_template) + 1);
    if (file_his_name == NULL)
      {
      printf("malloc() failed for file_his_name.  "
	     "Could not save histograms.\n");
      }
    else
      {
      sprintf(file_his_name,test_27_file_his_template,
	      datadir_translation,0);
      printf("Saving histograms\n");
      HRPUT(0, file_his_name, "N");
      free(file_his_name);    
      }
    }

  else if (pass == 1)
    {
    calc_peds_(1, mean, rms);
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      for (j = 0; j < NSTRIP; j++)
	{
        HFILL(1, mean[ilayer][j], 0., 1.);
        HFILL(2, rms[ilayer][j], 0., 1.);
	}
      }
    /* Save histograms */
    file_his_name = malloc(strlen(datadir_translation) + 
			   strlen(test_27_file_his_template) + 1);
    if (file_his_name == NULL)
      {
      printf("malloc() failed for file_his_name.  "
	     "Could not save histograms.\n");
      }
    else
      {
      sprintf(file_his_name,test_27_file_his_template,
	      datadir_translation,1);
      printf("Saving histograms\n");
      HRPUT(0, file_his_name, "N");
      free(file_his_name);    
      }
    }

  else if (pass == 2)
    {
      fclose(fp2);
/* Do analysis of available histograms */

/* Find alignment constants */
    strip_alignment_(ctest, var, &nwires); 

    fprintf(fp, "\nStrip alignment constants from pass 2:\n");
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      fprintf(fp, "L%d: c0=%10.3e  c1=%10.3e  var=%10.3e\n", ilayer+1, 
	      ctest[ilayer][0], ctest[ilayer][1], var[ilayer]);
      }
    fprintf(fp, "\n");

    for (i=0; i<nwires;i++) 
	  x_wg[i] = r_cntr_wg(csc_type, i+1)- r_al_pin(csc_type);

    strip_alignment_x_(x_wg, ctest, var, &nwires); 

    fprintf(fp, "\nStrip alignment constants from pass 2: in mm\n");
    for (ilayer = 0; ilayer < NLAYER; ilayer++)
      {
      fprintf(fp, "L%d: c0=%10.3e  c1=%10.3e  var=%10.3e\n", ilayer+1, 
	      ctest[ilayer][0], ctest[ilayer][1], var[ilayer]);
      }
    fprintf(fp, "\n");

/* Measure long-range crosstalk */
    crossana(xt, var2);

/* Check anode timing */
    y_avg = 0;
    for (j = 0; j < NLAYER; j++)
      {
      k = j * nwires;
      HUNPAK(61 + j, &y[k], "HIST", 1.);
      HUNPKE(61 + j, &yerr[k], "HIST", 1.);
      for (i = 0; i < nwires; i++) y_avg += y[k++];
      }
    y_avg = y_avg / (NLAYER * nwires);

    limits[0] = y_avg - 1.5;          /* layer histo lower limit */
    limits[1] = y_avg + 1.5;          /* layer histo upper limit */
    limits[2] = y_avg - 1.5;          /* summary histo lower limit */
    limits[3] = y_avg + 1.5;          /* summary histo upper limit */
    limits[4] = y_avg + AFEB_HIT_LOWER_LIMIT;  /* lower limit line */
    limits[5] = y_avg + AFEB_HIT_UPPER_LIMIT;  /* upper limit line */
    limits[6] = nwires;
    
    one_page_plot_v3(27, 1, WIRE_PLOT, (float *)y, (float *)yerr, limits, 
		     csc_name, "AFEB average hit times", "avg hit time, 25 ns bins", 1);

/* Now delete old histograms and book new ones for end-of-pass analysis */
    /* Save histograms */
    file_his_name = malloc(strlen(datadir_translation) + 
			   strlen(test_27_file_his_template) + 1);
    if (file_his_name == NULL)
      {
      printf("malloc() failed for file_his_name.  "
	     "Could not save histograms.\n");
      }
    else
      {
      sprintf(file_his_name,test_27_file_his_template,
	      datadir_translation,2);
      printf("Saving histograms\n");
      HRPUT(0, file_his_name, "N");
      HDELET(0);
      free(file_his_name);    
      }

    j = 1;
    book_histos_27f_(&pass, &j, &nwires);

/* Redo trackfits with new alignment constants 
   to get resolution & efficiency */
    filename = malloc(strlen(datadir_translation) + 
		      strlen(test_27_file_xclust_el) + 1 );
    if (filename == NULL)
      {
      printf("malloc() failed on filename.\n");
      return 1;
      }
    sprintf(filename,"%s%s",datadir_translation,test_27_file_xclust_el);

/* Open result file in the temporary directory */
    fp2 = fopen(filename, "r");
    if(fp2 == NULL)
      {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
      }
    free(filename);

    evtnum = 0;
    while (evtnum < 125000)
      {

/* Read one event's clusters from xclusters.dat */
      n = fscanf(fp2, "%7ld %2d", &evtnum, &j);
      if (n != 2) goto fileend;
      for (j = 0; j < 2; j++) fscanf(fp2, "%hd %2hd %hd %2hd %3hd %3hd %2hd", 
				     &dummy, &alct_key[j], &dummy, &dummy, &dummy, &dummy, &dummy);
      key_wg = alct_key[0];

      for (j = 0; j < NLAYER; j++)
	{
        n = fscanf(fp2, "%e %e %e %d", &xclus[j], &xerr[j], &xchisq, &xstat[j]); 
        if (n != 4) goto fileend;
        lok[j] = (xstat[j] == 3) && (xchisq < 50);
        xalign[j] = c[j][0] + c[j][1] * (key_wg + 1);
	}

/* Find out which HV segment the track is in, and if it's near an HV boundary */
      iseg = segm_wg(key_wg+1, csc_type)-1;
      at_boundary = at_bound_wg(key_wg+1, csc_type);

/* Loop over layer left out of the fit */
      for (k = 0; k < NLAYER; k++)
	{
        nok = 0;
        for (i = 0; i < NLAYER; i++) if (lok[i] && (i != k)) nok++;
        if (nok >= 4)
	  {
          trackfit_(xclus, xerr, xalign, z, lok, &k, &emm, &bee, 
		    &track_chisq, xtrack, resid, &nlayers_in_fit, &trackfit_ok);

          if (trackfit_ok && track_chisq / (nok - 2) < 20)
	    {
/* Plot residuals */
            xst = xtrack[k] - (int)(xtrack[k] + 0.5);
            hid = 300 + 10 * (k + 1) + (iseg + 1);
            if (!at_boundary) HFILL(hid, xst, resid[k], 1.);
	    }
	  }
	}

      }
      fileend: 
    fclose(fp2);
    /* Save histograms */
    file_his_name = malloc(strlen(datadir_translation) + 
			   strlen(test_27_file_his_template) + 1);
    if (file_his_name == NULL)
      {
      printf("malloc() failed for file_his_name.  "
	     "Could not save histograms.\n");
      }
    else
      {
      sprintf(file_his_name,test_27_file_his_template,
	      datadir_translation,2);
      printf("Saving histograms\n");
      HRPUT(0, file_his_name, "U");
      free(file_his_name);    
      }
    }

  else if (pass == 3)
    {
/* Check comparator residuals (agreement with tracks) */
    for (j = 0; j < NLAYER; j++)
      {
      k = j * NHALFSTRIP;
      HUNPAK(161 + j, &y[k], "HIST", 1.);
      HUNPKE(161 + j, &yerr[k], "HIST", 1.);
      }

    if (nstrs < NSTRIP)
    {
	for (i = 1; i < 6; i++)
	{
	    memmove (&y   [nstrs*2*i], &y   [NHALFSTRIP*i], nstrs*2*sizeof(float));
	    memmove (&yerr[nstrs*2*i], &yerr[NHALFSTRIP*i], nstrs*2*sizeof(float));
	}
    }

    limits[0] = -0.1;                 /* layer histo lower limit */
    limits[1] =  0.1;                 /* layer histo upper limit */
    limits[2] = -0.1;                 /* summary histo lower limit */
    limits[3] =  0.1;                 /* summary histo upper limit */
    limits[4] = COMP_RES_LOWER_LIMIT;   /* lower limit line */
    limits[5] = COMP_RES_UPPER_LIMIT;   /* upper limit line */
    limits[6] = nstrs;
    
    one_page_plot_v3(27, 2, HSTRIP_PLOT, (float *)y, (float *)yerr, limits, 
		     csc_name, "Comparator mean offsets from track position", 
		     "offset, stripwidths", 1);

/* Check comparator efficiency */
    i = 0;
    for (j = 0; j < NLAYER; j++)
      {
      HUNPAK(141 + j, numer, "HIST", 1.);
      HUNPAK(151 + j, denom, "HIST", 1.);
      for (k = 0; k < 2*NSTRIP; k++)
	{
        if (denom[k] > 0) y[i++] = numer[k] / denom[k];
        else y[i++] = 0.;
	}
      }

    if (nstrs < NSTRIP)
    {
	for (i = 1; i < 6; i++)
	{
	    memmove (&y   [nstrs*2*i], &y   [NHALFSTRIP*i], nstrs*2*sizeof(float));
	}
    }


    limits[0] = 0.;                      /* layer histo lower limit */
    limits[1] = 1.;                      /* layer histo upper limit */
    limits[2] = 0.;                      /* summary histo lower limit */
    limits[3] = 1.;                      /* summary histo upper limit */
    limits[4] = COMP_EFF_LOWER_LIMIT;    /* lower limit line */
    limits[5] = COMP_EFF_UPPER_LIMIT;    /* upper limit line */
    limits[6] = nstrs;
    
    one_page_plot_v3(27, 3, HSTRIP_PLOT, (float *)y, (float *)y, limits, 
		     csc_name, "Comparator efficiency", "efficiency", 0);

/* Check strip efficiency */
    i = 0;
    for (j = 0; j < NLAYER; j++)
      {
      HUNPAK(121 + j, numer, "HIST", 1.);
      HUNPAK(131 + j, denom, "HIST", 1.);
      for (k = 0; k < NSTRIP; k++)
	{
        if (denom[k] > 0) y[i++] = numer[k] / denom[k];
        else y[i++] = 0.;
	}
      }

    if (nstrs < NSTRIP)
    {
	for (i = 1; i < 6; i++)
	{
	    memmove (&y   [nstrs*i], &y   [NSTRIP*i], nstrs*sizeof(float));
	}
    }

    limits[0] = 0.;                      /* layer histo lower limit */
    limits[1] = 1.;                      /* layer histo upper limit */
    limits[2] = 0.;                      /* summary histo lower limit */
    limits[3] = 1.;                      /* summary histo upper limit */
    limits[4] = STRIP_EFF_LOWER_LIMIT;   /* lower limit line */
    limits[5] = STRIP_EFF_UPPER_LIMIT;   /* upper limit line */
    limits[6] = nstrs;

    one_page_plot_v3(27, 4, STRIP_PLOT, (float *)y, (float *)y, limits, 
		     csc_name, "Strip cluster efficiency", "efficiency", 0);
    fclose(fp2);
    /* Save histograms */
    file_his_name = malloc(strlen(datadir_translation) + 
			   strlen(test_27_file_his_template) + 1);
    if (file_his_name == NULL)
      {
      printf("malloc() failed for file_his_name.  "
	     "Could not save histograms.\n");
      }
    else
      {
      sprintf(file_his_name,test_27_file_his_template,
	      datadir_translation,3);
      printf("Saving histograms\n");
      HRPUT(0, file_his_name, "N");
      free(file_his_name);    
      }
    }

  return 0;
  }

/*******************************************************************************
   End-of-test analysis for FAST site test 27
*******************************************************************************/

int test_27_finish(void)
  {

/* Save histograms */
  printf("Beginning test_27_finish\n");
  printf("Saving histograms\n");

/* Make plots which can't be done as one-page plots */
  printf("Go to the directory %s/test_results/temp and start "
	 "a PAW session\n",datadir_translation);
  printf("type \"exec test_27\"\n");
  printf("Hit Return when finished\n");
  wait_for_return();

/* Look for values outside limits */

/* Print mandatory summary statement */
  if (1) fprintf(fp, "Test result = Failed\n");
  else fprintf(fp, "Test result = OK\n");
  fclose(fp);

  HDELET(0);

  return 0;
  }

/*---------------------------------------------------------------------------*/
/*
 *  COMPSIM : Routine to generate comparator outputs from SCA data
 *  First version  1/24/01
 */

int compsim(float sca[NLAYER][NSTRIP][16], int nsample, int clkphase, 
	    int cmode, int ctime, float thr, unsigned int comphits[NLAYER][NHALFSTRIP])
  {
  int             i, j, k, hs, isample, l, s;
  float           sca_left, sca_center, sca_right;
  unsigned int    bit, over_thr[NSTRIP];
  unsigned int    pattern0, mask0;

  static logical         first = _TRUE;
  static unsigned int    mask[32], pattern[32];
  static int             jmax, jpeak[32];

/* 
 * Input:   sca array = pedestal-subtracted strip raw data (50 MHz clock)
 * Output:  halfstrip array (NOT triads)
 */

/* Sixteen SCA samples * 50 ns = 800 ns or 32 CLCT timebins. Maximum CLCT
 * full dump is 16 timebins, but the position of this window relative to
 * the CLCT pretrigger is variable. To simulate, we need 2 clock cycles
 * before the comp. threshold is crossed, and up to 7 clock cycles after
 * the threshold is crossed, for a total of 9 clocks = 5 SCA samples. 
 */

/* Parameters: 
 *     nsample  
 *     clkphase  
 *     cmode  
 *     ctime  
 *     thr  
 */

/* To improve efficiency, set up the mode-dependent masks and triggering
 * patterns once at the beginning. We assume cmode = 0, (ctime ignored) is 
 * actually the same case as cmode = 1 or 2, ctime = 0.
 */
  if (first)
    {
    first = _FALSE;

    if (cmode == 0) ctime = 0;

    pattern0 = 0x4 | (0x4 << ctime);
    if (cmode == 2) for (i = 1; i < ctime; i++) pattern0 |= (0x4 << i);
    mask0 = pattern0 | 0x3;

//  printf("j, mask, pattern, jpeak: \n");
    jmax = 32 - (ctime + 2);
    for (j = 0; j < jmax; j++)
      {
      mask[j] = mask0 << j;
      pattern[j] = pattern0 << j;
      jpeak[j] = j + ctime + 2;
//    printf(" %2d %08x %08x %08x\n", j, mask[j], pattern[j], jpeak[j]);
      }
    }

/* Convert SCA info into digital (over or under threshold) comparator info.
 * Use linear interpolation between SCA samples (at least for now).
 * clkphase may be 0 or 1.
 */
  for (l = 0; l < NLAYER; l++)
    {
    for (s = 0; s < NSTRIP; s++)
      {
      over_thr[s] = 0;
      bit = 1 << clkphase;
      for (j = 0; j < nsample; j++)
	{
        if (sca[l][s][j] >= thr) over_thr[s] |= bit;
        bit = bit << 1;
        if (j < nsample - 1)
	  {
          if ((sca[l][s][j+1] + sca[l][s][j]) / 2. >= thr) over_thr[s] |= bit;
          bit = bit << 1;
	  }
	}

/* Debug */ /*
   if (over_thr[s])
   {
   printf("L%d s%d has over_thr = ", l+1, s+1);
   for (j = 0; j < 32; j++) printf("%01d", (over_thr[s] >> j) & 1);
   printf("\n");

   for (isample = 0; isample < nsample; isample++)
   {
   printf("%8.1f", sca[l][s-1][isample]);
   if (isample % 8 == 7) printf("\n");
   }
   for (isample = 0; isample < nsample; isample++)
   {
   printf("%8.1f", sca[l][s][isample]);
   if (isample % 8 == 7) printf("\n");
   }
   for (isample = 0; isample < nsample; isample++)
   {
   printf("%8.1f", sca[l][s+1][isample]);
   if (isample % 8 == 7) printf("\n");
   }
   }
	    */
      }

/* Comparison is necessarily a little asymmetric, since if SCA values are
 * equal, we must choose arbitrarily. Since most comparators favor the right
 * halfstrip, we favor it too.
 */
    for (s = 0; s < NSTRIP; s++)
      {
      comphits[l][2*s] = 0;
      comphits[l][2*s+1] = 0;

      if (over_thr[s])
	{
        for (j = 0; j < jmax; j++)
	  {
          if ((over_thr[s] & mask[j]) == pattern[j])
	    {
            isample = jpeak[j] / 2;
            k = jpeak[j] % 2;
            if (k == 0 || (isample >= nsample-1))
	      {
              sca_left   = sca[l][s-1][isample];
              sca_center = sca[l][s  ][isample];
              sca_right  = sca[l][s+1][isample];
	      }
            else
	      {
              sca_left   = (sca[l][s-1][isample] + sca[l][s-1][isample+1]) / 2.;
              sca_center = (sca[l][s  ][isample] + sca[l][s  ][isample+1]) / 2.;
              sca_right  = (sca[l][s+1][isample] + sca[l][s+1][isample+1]) / 2.;
	      }

/* Debug */ /*
   printf("  s%2d: j=%d, jpeak=%d, k=%d, sca_left=%8.1f, "
   "sca_center=%8.1f, sca_right=%8.1f\n", s+1, j, jpeak[j], k, 
   sca_left, sca_center, sca_right);
	    */

            if ((sca_center > sca_left) && (sca_center > sca_right))
	      {
              if (sca_left > sca_right) hs = 2 * s;
              else hs = 2 * s + 1;
              comphits[l][hs] |= (1 << jpeak[j]);

/* Debug */ /*
   printf("L%d hs %3d has comphits = ", l+1, hs+1);
   for (j = 0; j < 32; j++) 
   printf("%01d", (comphits[l][hs] >> j) & 1);
   printf("\n");
	    */
	      }
	    }
	  }
	}
      }
    }

  return 0;
  }

/*---------------------------------------------------------------------------*/
/*
 *  CLUSANA2: Routine to study clusters (found by comparators)
 *  First version  3/14/01
 *  Look at pulse shapes using clusters centered between strips
 */

int clusana2( unsigned int comphits[NLAYER][NHALFSTRIP], int isegment)
  {

  int      i, j, k, hs, ilayer, istrip, jmax, kmax, kl, kr, sca_max;
  float    left_center_sum, right_center_sum, left_sum, right_sum;
  float    left_left_sum, right_right_sum, r1, r2, r3, sum2, sum4, sum6;
  float    tpeak, t0, t1, t1step; 

/* This set is for clusters at all radii, starting at t0 = -0.190 (which is
 * to say: x[0] = -.200)
 */
  float    ysum[60] = {
    0.00000, 0.00012, 0.00112, 0.00174, 0.00483, 0.01227, 0.02702, 0.05739,
    0.09148, 0.12768, 0.16276, 0.19487, 0.22301, 0.25313, 0.27991, 0.30309,
    0.32714, 0.35168, 0.37424, 0.39717, 0.42383, 0.44874, 0.47440, 0.50056,
    0.52659, 0.55151, 0.57729, 0.60729, 0.63382, 0.66072, 0.68972, 0.71576,
    0.74204, 0.76869, 0.79212, 0.81765, 0.84120, 0.86538, 0.89129, 0.91471,
    0.93430, 0.95438, 0.97211, 0.98351, 0.99269, 0.99554, 0.99690, 0.99777,
    0.99864, 0.99888, 0.99926, 0.99926, 0.99950, 0.99963, 0.99975, 0.99988,
    0.99988, 0.99988, 1.00000, 1.00000 };

/* Each comparator hit marks a cluster */
  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
    for (istrip = 34; istrip < NSTRIP - 2; istrip++)
      {
      if (ilayer == 4 && istrip > 45 && istrip < 66) continue;
      hs = istrip * 2;

      if (comphits[ilayer][2*istrip] || comphits[ilayer][2*istrip+1])
	{
        if (comphits[ilayer][hs]) hs = 2 * istrip;
        else hs = 2 * istrip + 1;

        for (i = 0; i < upevt_.nsca_sample; i++)
	  if (comphits[ilayer][hs] & (3 << (2*i))) break;
        if (i < 2) i = 2;
        if (i > upevt_.nsca_sample - 3) i = upevt_.nsca_sample - 3;
      
/* Find the real peak timesample for the strip with the comparator hit */
        sca_max = -999;
        for (j = i - 2; j <= i + 2; j++)
	  {
          for (k = -1; k <= 1; k++)
	    {
            if ((upevt_.sca[ilayer][istrip+k][j] - mean[ilayer][istrip+k]) 
		> sca_max)
	      {
              sca_max = upevt_.sca[ilayer][istrip+k][j]-mean[ilayer][istrip+k];
              jmax = j;
              kmax = k;
	      }
	    }
	  }

/* Find the next largest strip */
        kl = istrip + kmax;
        if ((upevt_.sca[ilayer][kl-1][jmax] - mean[ilayer][kl-1]) >
	    (upevt_.sca[ilayer][kl+1][jmax] - mean[ilayer][kl+1])) kl -= 1;
        kr = kl + 1;
        if (kl < 34 || kr > NSTRIP - 3) return 0;

/* Calculate timesample sums around jmax for the peak strip and its neighbors,
 * skipping clusters which are peaked in the first or last two timesamples 
 */
/*      if (jmax > 1 && jmax < upevt_.nsca_sample - 2)
	{ */
        if (jmax >= 5 && jmax <= 7)
	  {
          left_center_sum  = -9 * mean[ilayer][kl];
          right_center_sum = -9 * mean[ilayer][kr];
          left_sum         = -9 * mean[ilayer][kl-1];
          right_sum        = -9 * mean[ilayer][kr+1];
          left_left_sum    = -9 * mean[ilayer][kl-2];
          right_right_sum  = -9 * mean[ilayer][kr+2];

          for (j = jmax - 4; j <= jmax + 4; j++)
	    {
            left_center_sum  += upevt_.sca[ilayer][kl][j];
            right_center_sum += upevt_.sca[ilayer][kr][j];
            left_sum         += upevt_.sca[ilayer][kl-1][j];
            right_sum        += upevt_.sca[ilayer][kr+1][j];
            left_left_sum    += upevt_.sca[ilayer][kl-2][j];
            right_right_sum  += upevt_.sca[ilayer][kr+2][j];
	    }

          sum2 = left_center_sum + right_center_sum;
          sum4 = left_sum + sum2 + right_sum;
          sum6 = left_left_sum + sum4 + right_right_sum;

          r1 = (float)fabs(right_center_sum - left_center_sum) / sum4;
          r2 = sum2 / sum4;
          r3 = sum2 / sum6;

/* Fill histograms */

          HFILL(10, sum4, 0, 1.);
          HFILL(11, r3, 0, 1.);
          HFILL(12, sum4, r3, 1.);
          HFILL(13, (float)isegment, 0., 1.);
          HFILL(14, (float)jmax, 0., 1.);

          if (r3 >= 0.75 && r3 <= 1.15)
	    {
            if (sum4 > 200)
	      {
              HFILL(100, r1, r2, 1.);
              if (isegment >= 0 && isegment <= 4) HFILL(100+isegment+1, r1, r2, 
							1.);
	      }
            else HFILL(111, r1, r2, 1.);
	    }
          else
	    {
            HFILL(112, r1, r2, 1.);
	    }

/* Find time-ordering */
          if (r3 >= 0.75 && r3 <= 1.15 && sum4 > 200 && r1 < 0.02 && sum2 != 0
	      && r2 > 0.88 && r2 < 1.02)
	    {
            t1 = ((upevt_.sca[ilayer][kl][jmax+1] + 
		   upevt_.sca[ilayer][kr][jmax+1]) - (upevt_.sca[ilayer][kl][jmax-1] 
						      + upevt_.sca[ilayer][kr][jmax-1])) / sum2;
            HFILL(20, t1, 0., 1.);

            if (isegment >= 0 && isegment <= 4) HFILL(20+isegment+1, t1, 0., 
						      1.);

/* Time ordering uses result of numerical integration, with linear 
 * interpolation between data points.
 */
            t1step = 0.010;
            t0 = -0.190;
            j = (int)((t1 - t0) / t1step);
            if (j < 0) j = 0;
            if (j > 58) j = 58;
            tpeak = 0.5 - (ysum[j] + ((t1 - (t0 + j * t1step)) / t1step) *
			   (ysum[j+1] - ysum[j]));
            if (isegment == 2)
	      {
              HFILL(26, tpeak, 0., 1.);
              if (left_center_sum != 0 && right_center_sum != 0 &&
		  left_sum != 0 && right_sum != 0 && left_left_sum != 0 && 
		  right_right_sum != 0)
		{
                HFILL(27, tpeak, 0., 1.);

/* Save SCA measurements */
                for (j = jmax - 3; j <= jmax + 5; j++)
		  {
                  HFILL(30, (tpeak+j-jmax), 0., (upevt_.sca[ilayer][kl][j] 
						 - mean[ilayer][kl]) / sum2);
                  HFILL(30, (tpeak+j-jmax), 0., (upevt_.sca[ilayer][kr][j] 
						 - mean[ilayer][kr]) / sum2);
                  HFILL(31, (tpeak+j-jmax), 0., (upevt_.sca[ilayer][kl-1][j]
						 - mean[ilayer][kl-1]) / sum2);
                  HFILL(31, (tpeak+j-jmax), 0., (upevt_.sca[ilayer][kr+1][j]
						 - mean[ilayer][kr+1]) / sum2);
                  HFILL(32, (tpeak+j-jmax), 0., (upevt_.sca[ilayer][kl-2][j]
						 - mean[ilayer][kl-2]) / sum2);
                  HFILL(32, (tpeak+j-jmax), 0., (upevt_.sca[ilayer][kr+2][j]
						 - mean[ilayer][kr+2]) / sum2);
		  }
		}
	      }
	    }
	  }
	}     /* Finished with comparator hit */

      }
    }
  return 0;
  }

/*---------------------------------------------------------------------------*/
/*
 *  CLUSANA: Routine to study clusters (found by comparators)
 *  First version  1/31/01
 *  Look at pulse shapes using clusters centered on one strip
 */

int clusana( unsigned int comphits[NLAYER][NHALFSTRIP], int isegment)
  {

  int      i, j, hs, ilayer, istrip, jmax, sca_max;
  float    left_sum, center_sum, r1, r2, r3, right_sum, sum;
  float    left_left_sum, right_right_sum, sum5, t1;
  float    tpeak, t0, t1step; 

/* This set is for clusters at all radii, starting at t0 = -0.190 */ /*
   float    ysum[50] = {
   0.000,  0.000,  0.000,  0.000,  0.001,  0.003,  0.010,  0.029,
   0.060,  0.093,  0.126,  0.159,  0.192,  0.223,  0.253,  0.282,
   0.311,  0.339,  0.367,  0.392,  0.421,  0.447,  0.474,  0.501,
   0.527,  0.553,  0.578,  0.603,  0.629,  0.654,  0.679,  0.704,
   0.728,  0.754,  0.780,  0.803,  0.828,  0.853,  0.878,  0.902,
   0.926,  0.948,  0.966,  0.981,  0.990,  0.996,  0.998,  0.999,
   1.000,  1.000 };
								     */

/* This set is for clusters in the middle HV segment, starting at t0 = -0.180 */
  float    ysum[50] = { 
    0.00000, 0.00010, 0.00020, 0.00030, 0.00080, 0.00547, 0.01532, 0.03759,
    0.07021, 0.10711, 0.14073, 0.17782, 0.21372, 0.25052, 0.28284, 0.31149, 
    0.34321, 0.37185, 0.39722, 0.42596, 0.45430, 0.48026, 0.50940, 0.53744, 
    0.56300, 0.59234, 0.62029, 0.64873, 0.67946, 0.70990, 0.73933, 0.76967, 
    0.79771, 0.82785, 0.85758, 0.88573, 0.91119, 0.93844, 0.96022, 0.97802, 
    0.98876, 0.99523, 0.99871, 0.99940, 0.99980, 0.99980, 0.99990, 0.99990, 
    0.99990, 1.00000  };

/* Each comparator hit marks a cluster */
  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
    for (istrip = 34; istrip < NSTRIP - 2; istrip++)
      {
      if (ilayer == 4 && istrip > 45 && istrip < 66) continue;
      hs = istrip * 2;

      if (comphits[ilayer][2*istrip] || comphits[ilayer][2*istrip+1])
	{
        if (comphits[ilayer][hs]) hs = 2 * istrip;
        else hs = 2 * istrip + 1;

        for (i = 0; i < upevt_.nsca_sample; i++)
	  if (comphits[ilayer][hs] & (3 << (2*i))) break;
        if (i < 2) i = 2;
        if (i > upevt_.nsca_sample - 3) i = upevt_.nsca_sample - 3;
      
/* Find the real peak timesample for the strip with the comparator hit */
        for (j = i - 2; j <= i + 2; j++)
	  {
          if (j == (i - 2) || upevt_.sca[ilayer][istrip][j] > sca_max)
	    {
            sca_max = upevt_.sca[ilayer][istrip][j];
            jmax = j;
	    }
	  }

/* Calculate timesample sums around jmax for the peak strip and its neighbors,
 * skipping clusters which are peaked in the first or last two timesamples 
 */
/*      if (jmax > 1 && jmax < upevt_.nsca_sample - 2)
	{ */
        if (jmax >= 5 && jmax <= 7)
	  {
          center_sum      = -9 * mean[ilayer][istrip];
          left_sum        = -9 * mean[ilayer][istrip-1];
          right_sum       = -9 * mean[ilayer][istrip+1];
          left_left_sum   = -9 * mean[ilayer][istrip-2];
          right_right_sum = -9 * mean[ilayer][istrip+2];

          for (j = jmax - 4; j <= jmax + 4; j++)
	    {
            center_sum      += upevt_.sca[ilayer][istrip][j];
            left_sum        += upevt_.sca[ilayer][istrip-1][j];
            right_sum       += upevt_.sca[ilayer][istrip+1][j];
            left_left_sum   += upevt_.sca[ilayer][istrip-2][j];
            right_right_sum += upevt_.sca[ilayer][istrip+2][j];
	    }

          sum = left_sum + center_sum + right_sum;
          sum5 = left_left_sum + right_right_sum + sum;

/* Debug */ /*
   printf("L%d s%2d hs%3d: jmax=%2d, sums: left=%5.0f, center=%5.0f, "
   "right=%5.0f, all=%5.0f\n", ilayer+1, istrip+1, hs+1, jmax, 
   left_sum, center_sum, right_sum, sum);
	    */
          r1 = (float)fabs(right_sum-left_sum)/sum;
          r2 = center_sum/sum;
          r3 = sum/sum5;

/* Fill histograms */

          HFILL(10, sum, 0, 1.);
          HFILL(11, r3, 0, 1.);
          HFILL(12, sum, r3, 1.);
          HFILL(13, (float)isegment, 0., 1.);
          HFILL(14, (float)jmax, 0., 1.);

          if (r3 >= 0.90 && r3 <= 1.10)
	    {
            if (sum > 200)
	      {
              HFILL(100, r1, r2, 1.);
              if (isegment >= 0 && isegment <= 4) HFILL(100+isegment+1, r1, r2, 
							1.);
	      }
            else HFILL(111, r1, r2, 1.);
	    }
          else
	    {
            HFILL(112, r1, r2, 1.);
	    }

/* Find time-ordering */
          if (r3 >= 0.90 && r3 <= 1.10 && sum > 200 && r1 < 0.02
	      && r2 >= 0.725 && r2 <= 0.925 && center_sum != 0)
	    {
            t1 = (upevt_.sca[ilayer][istrip][jmax+1] -
		  upevt_.sca[ilayer][istrip][jmax-1]) / center_sum;
            HFILL(20, t1, 0., 1.);

            if (isegment >= 0 && isegment <= 4) HFILL(20+isegment+1, t1, 0., 
						      1.);

/* Time ordering uses result of numerical integration, with linear 
 * interpolation between data points.
 */
            t1step = 0.010;
            t0 = -0.180;
            j = (int)((t1 - t0) / t1step);
            if (j < 0) j = 0;
            if (j > 48) j = 48;
            tpeak = 0.5 - (ysum[j] + ((t1 - (t0 + j * t1step)) / t1step) *
			   (ysum[j+1] - ysum[j]));
            if (isegment == 2)
	      {
              HFILL(26, tpeak, 0., 1.);
              if (center_sum != 0 && left_sum != 0 && right_sum != 0 && 
		  left_left_sum != 0 && right_right_sum != 0) 
		HFILL(27, tpeak, 0., 1.);
	      }

/* Save SCA measurements */
            for (j = jmax - 3; j <= jmax + 5; j++)
	      {
              if (isegment == 2 && center_sum != 0 && left_sum != 0 && 
		  right_sum != 0 && left_left_sum != 0 && right_right_sum != 0)
		{
                HFILL(30, (tpeak+j-jmax), 0., (upevt_.sca[ilayer][istrip][j] 
					       - mean[ilayer][istrip]) / center_sum);
                HFILL(31, (tpeak+j-jmax), 0., (upevt_.sca[ilayer][istrip-1][j]
					       - mean[ilayer][istrip-1]) / (2 * center_sum));
                HFILL(31, (tpeak+j-jmax), 0., (upevt_.sca[ilayer][istrip+1][j]
					       - mean[ilayer][istrip+1]) / (2 * center_sum));
                HFILL(32, (tpeak+j-jmax), 0., (upevt_.sca[ilayer][istrip-2][j]
					       - mean[ilayer][istrip-2]) / (2 * center_sum));
                HFILL(32, (tpeak+j-jmax), 0., (upevt_.sca[ilayer][istrip+2][j]
					       - mean[ilayer][istrip+2]) / (2 * center_sum));
		}
	      }
	    }
	  }
	}     /* Finished with comparator hit */

      }
    }
  return 0;
  }

/*---------------------------------------------------------------------------*/
/*
 *  CROSSFILL: Routine to fill crosstalk histograms
 *  First version  2/28/01
 */

void crossfill(int ilayer, float xclus, float tclus, 
	       float sca[NLAYER][NSTRIP][16])
  {
  int      j, icfeb, nstrips, peak_cfeb, peak_strip, peak_time, t;
  float    qpeak, qxt;

/* Find the peak strip */
  peak_strip = rint(xclus);
  peak_time = rint(tclus);
  if (peak_strip < 1 || peak_strip > NSTRIP - 2 || peak_time < 2 || 
      peak_time > upevt_.nsca_sample - 3) return;

/* Exclude clusters within a halfstrip of cfeb boundaries, since their
 * crosstalk is divided between two cfebs (and the exact division is 
 * stripwidth dependent) 
 */
  if ((peak_strip % 16 ==  0 && xclus < rint(xclus)) ||
      (peak_strip % 16 == 15 && xclus > rint(xclus))) return;

/* Calculate qpeak */
  qpeak = 0;
  for (j = peak_strip - 2; j <= peak_strip + 2; j++)
    {
    if (j < 0 || j > NSTRIP - 1) continue;
    for (t = peak_time-2; t <= peak_time+2; t++) qpeak += sca[ilayer][j][t];
    }

  peak_cfeb = peak_strip / 16;
/* Loop over cfebs */
  for (icfeb = 0; icfeb < NCFEB; icfeb++)
    {
/* Calculate average crosstalk charge (per strip) */
    nstrips = 0;
    qxt = 0;
    for (j = 16 * icfeb; j < 16 * (icfeb + 1); j++)
      {
      if (abs(peak_strip - j) > 2)
	{
	for (t = peak_time-2; t <= peak_time+2; t++) qxt += sca[ilayer][j][t];
	nstrips++;
        }
      }
    qxt = qxt / nstrips;

/* Fill histograms */
    if (icfeb == peak_cfeb) 
      HFILL(200 + 10 * (ilayer + 1) + (icfeb + 1), qpeak, qxt, 1.);
    else HFILL(600 + 10 * (ilayer + 1) + (icfeb + 1), qpeak, qxt, 1.);

/* Debug */ /*
   printf("Event %ld L%d peak_strip %d peak time %d : icfeb=%d qpeak=%f, "
   "qxt=%f\n", upevt_.event_number, ilayer+1, peak_strip+1, peak_time, 
   icfeb, qpeak, qxt);
	    */
    }
  return;
  }

/*---------------------------------------------------------------------------*/
/*
 *  CROSSANA: Routine to derive long-range crosstalk from crosstalk histograms
 *  First version  3/19/01
 */

#define NBINS    100

void crossana(float xt[NLAYER][NCFEB][2], float var[NLAYER][NCFEB])
  {
  int      j, hid, icfeb, ilayer, key, l;
  float    x[NBINS], y[NBINS], ey[NBINS], yw[NBINS];

/* x is the cluster charge (5x5 adc sum); range = 0 to 10000 */
  for (j = 0; j < NBINS; j++) x[j] = 50 + j * 100;

/* y is the measured crosstalk (average 1x5 adc sum in the same chip) */
  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
    for (icfeb = 0; icfeb < NCFEB; icfeb++)
      {
      hid = 200 + 10 * (ilayer + 1) + (icfeb + 1);
      if (HEXIST(hid))
	{
        HUNPAK(hid, y, "HIST", 1.);
        HUNPKE(hid, ey, "HIST", 1.);
	}

/* Fix too-small errors in empty bins, and set weights = 1 / errors  */
      for (j = 0; j < NBINS; j++)
	{
        if (ey[j] < 0.10) ey[j] = 1000;
        yw[j] = 1 / ey[j];
	}

/* Do linear fit */
      key = 0;
      l = NBINS - 10;
      lfitw_(&x[10], &y[10], &yw[10], &l, &key, &xt[ilayer][icfeb][0],
	     &xt[ilayer][icfeb][1], &var[ilayer][icfeb]);
      }
    }
  return;
  }

/*---------------------------------------------------------------------------*/
/*
 *  SCINTANA: Routine to get the track angle from the scintillator times.
 *  First version  3/27/01
 */

float scintana(int *s1, int *s2)
  {
  int      j, group, scint, time[4], snum[4];
  float    dt;

/* Initialize */
  for (group = 0; group < 4; group++) time[group] = 0;

/* Look for a reasonable hit in Top East, Top West, Bottom East, Bottom West */
  for (j = 0; j < upevt_.num_scint_hits; j++)
    {
//  printf("%2d  scint time=%d, label=%#x\n", j, upevt_.scint_times[j],
//   upevt_.scint_labels[j]);
    scint = upevt_.scint_labels[j] & 0xff;
    group = ((upevt_.scint_labels[j] >> 8) & 0xf) - 1;
    if (upevt_.scint_times[j] > 200 && upevt_.scint_times[j] < 300)
      {
      time[group] = upevt_.scint_times[j];
      snum[group] = scint;
      }
    }
//for (group = 0; group < 4; group++) printf("group %d: time=%d, snum=%d\n",
// group, time[group], snum[group]);

/* Calculate the difference between the top and bottom time differences */
  if ((time[0] > 0) && (time[1] > 0) && (time[2] > 0) && (time[3] > 0) && 
      (snum[0] == snum[1]) && (snum[2] == snum[3]))
    {
    dt = (time[1] - time[0]) - (time[3] - time[2]);
    *s1 = snum[0];
    *s2 = snum[2];
    HFILL(6, (float)snum[0], (float)snum[2], 1.);
    }
  else dt = 999;
    
  return dt;
  }
