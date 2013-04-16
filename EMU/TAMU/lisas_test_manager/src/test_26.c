/*
 * test_26.c (in V1.2 test_21.c)
 * CLCT self-trigger test
 *
 * Author : Valeri Sytnik and Kanglin He
 * Date   : June, 2001
 *
 * Last modified: 27 Jun 2002, add case with radioactive source S.D.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "daq_conf.h"
#include "test_params.h"     /* test config parameters */
#include "csc_parameters.h"     /* test config parameters */
#include "csc_event.h"     /* test config parameters */

#include <math.h>
#include "cfortran.h"
#include "hbook.h"

#include "misc.h"

#include <sys/time.h>
#include <time.h>

#include "test_utils.h"
#include "one_page_plot.h"
#include "test_26_ini.h"

//#include "csc_unpacker.h"
//#include "emu/daq/reader/RawDataFile.h"

//=========================================
void GetCLCTHalfStrips_26();
void GetSCAHalfStrips_26();
void GetCompThreshold_26();
void OnePagePlot_26();
void FinalOnePagePlot_26();
void AnalysisFinish_26(); // interface to test_finish
int SaveResults_26();

//====================================================

//===  mod++
int	hid_fpl_clct[NLAYER];
int     hid_fpl_key[3][NLAYER];
int     hid_fpl_pat[3][NLAYER]; 
int     hid_fcfeb; 
//===

int screen(char *mode);

int TotalTriggerNumber;
int TotalUngatedTriggerNumber;
   
   // unpack the CLCT and software-halfstrips
short clct_halfstrips[NLAYER][NSTRIP*2];
//float halfstrip_map[NLAYER][NSTRIP*2];

static int src_status;                      // Status of radioactive source for this run, 
char * src_val[2] ={ "No rad.source","With rad.source"};

char* test_26_file_el[2]     = {"/test_results/temp/test_26_01.results",
				"/test_results/temp/test_26_02.results"};
char* test_26_file_bad_el[2] = {"/test_results/temp/test_26_01.bad",
				"/test_results/temp/test_26_02.bad"};
char* test_26_file_his_el[2] = {"/test_results/temp/test_26_01.his",
				"/test_results/temp/test_26_02.his"};
static  int event_number;
static  int first = _TRUE;
static float chmb_len_coef;

float low_limit, high_limit; // limits for testing of bad channels,
                             // defined by FinalOnePage, used by SaveResults

static int source_type = 1;

float *halfstrip_map[NLAYER];
float *phalfstrip_map;


//============================================
int test_26_init(void);
int test_26_begin(int pass);
int test_26_event(int pass);
int test_26_end(int pass); 
int test_26_finish(void);
int get_default_peds(float *, float *);

int nstrips, ncfebs;
int test_26_init()
{
    int l;
    test_begin  = test_26_begin;
    test_event  = test_26_event;
    test_end    = test_26_end;
    test_finish = test_26_finish;

    num_passes = 1;
    num_events[0] = 100000;

    event_number = 0;
    first = _TRUE;
    switch (csc_type)
    {
    case 0: chmb_len_coef = 4.; break;
    case 1: chmb_len_coef = 2.; break;
    case 2: chmb_len_coef = 2.; break;
    case 3: chmb_len_coef = 2.; break;
    case 4: chmb_len_coef = 1.; break;
    case 5: chmb_len_coef = 1.; break;
    default: chmb_len_coef = 1.; break;
    }

    switch (csc_type)
    {
    case 2:  nstrips = 64; ncfebs = 4; break;
    default: nstrips = 80; ncfebs = 5; break;
    }

    printf ("csc_type = %d, Chamber length correction coefficient = %.3f\n", csc_type, chmb_len_coef);


    phalfstrip_map = malloc (sizeof(float) * NLAYER * nstrips * 2);

    for (l = 0; l < NLAYER; l++)
    {
	halfstrip_map [l] = phalfstrip_map + nstrips * l * 2;
    }

    return 0;

}
//=============================================================================

int    test_26_end(int pass)
  {
    return 0;
  }
//=============================================================================
int    test_26_finish()
  {

  AnalysisFinish_26();
  SaveResults_26();
  return 0;
  }
//=============================================================================

int test_26_begin(int pass)
{ 
  int iLayer;
  char tmp[120];
  int i;
  int j = 0;

  printf("****** Test 26 Start ******\n");
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

 
  for(iLayer = 0; iLayer < NLAYER; iLayer++)
    {
      hid_fpl_clct[iLayer]=iLayer+1;

      sprintf(tmp, "CLCT halfstrip%02d", iLayer+1);
      HBOOK1( hid_fpl_clct[iLayer] ,tmp,  nstrips*2, 0.5, nstrips*2+0.5   ,       0.);

    }
 

  for(i=0; i<NLAYER; i++)
    {
      hid_fpl_key[0][i]= 2*NLAYER+i+1;
      hid_fpl_key[1][i]= 2*NLAYER+6*1+i+1;
      hid_fpl_key[2][i]= 2*NLAYER+6*2+i+1;
      hid_fpl_pat[0][i]= 2*NLAYER+6*3+i+1;
      hid_fpl_pat[1][i]= 2*NLAYER+6*4+i+1;
      hid_fpl_pat[2][i]= 2*NLAYER+6*5+i+1;
    }

  for(i=0; i<NLAYER; i++)
    {      
      HBOOK1(hid_fpl_key[0][i], tmp,  nstrips*2, 0.5, nstrips*2+0.5, 0.);      
      HBOOK1(hid_fpl_key[1][i], tmp,  nstrips*2, 0.5, nstrips*2+0.5, 0.);      
      HBOOK1(hid_fpl_key[2][i], tmp,  nstrips*2, 0.5, nstrips*2+0.5, 0.);
      
      HBOOK1( hid_fpl_pat[0][i], tmp,  256, -0.5, 255.5, 0.);      
      HBOOK1( hid_fpl_pat[1][i], tmp,  256, -0.5, 255.5, 0.);      
      HBOOK1( hid_fpl_pat[2][i], tmp,  256, -0.5, 255.5, 0.);
    } 

  hid_fcfeb= 2*NLAYER+6*6+1;  
  HBOOK1( hid_fcfeb, tmp, ncfebs, 0.5, ncfebs+0.5, 0.);
  return 0;
}

static long int    nevents[6];
static float       all_time[6], all_scaler[6];

//=====================================================

int test_26_event(int pass)
  {
  int iLayer;
  int icfeb;
  int iHStrip;
  int trig_req;

  int              p;

  static int       last_p,  tlast_usec, tnow_usec;
  static time_t             tlast_sec,  tnow_sec;
  static unsigned long     last_scaler;
  static long      del_scaler;
  float del_time;
   
  event_number++;
      
  //===================
/* Initialization */
  if (first)
    {
    for (p = 0; p < 6; p++)
      {
	last_p =  -1;
      }
    first = _FALSE;
    }

/* Get time and scaler data */

  p = upevt_.stripcal_current_value - 1;
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

  if( del_scaler < 0) del_scaler += 0xffffff;
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

  //====================
  if(! upevt_.clct_active) return 0;
   
  GetCLCTHalfStrips_26();
  
  for(iLayer = 0; iLayer < NLAYER; iLayer++)
    {
    for(iHStrip = 0; iHStrip < 2*nstrips; iHStrip++)
      {  
	if(clct_halfstrips[iLayer][iHStrip] != 0)
	  {
    //        printf("#%d: %d - %d\n",event_number, iLayer, iHStrip);
	    HFILL(hid_fpl_clct[iLayer], (float )(iHStrip+1.), 0.0, 1.0); 
	  }
      }
    }


  /*
   * check cfeb active bits
   */
  for(icfeb = 0; icfeb < ncfebs; icfeb++)
    {
    if(upevt_.clct_active_febs & (1 << icfeb))
      {
	HFILL(hid_fcfeb, (float)(icfeb+1) , 0.0, 1.0); 
      }
    }
   
  /*
   *  Fill key halfstrip hist
   */
  trig_req=upevt_.stripcal_current_value-1;
   
  if(upevt_.tmb_clct_valid_patt[0])
    {  /* 1st best */
//    printf("#%d: 1st %d\n",event_number, upevt_.clct_half_strip[0]);
    HFILL(hid_fpl_key[0][trig_req], (float)(upevt_.clct_half_strip[0]+1.) , 0.0, 1.0); 
    HFILL(hid_fpl_key[1][trig_req], (float)(upevt_.clct_half_strip[0]+1.) , 0.0, 1.0); 
    HFILL(hid_fpl_pat[0][trig_req], (float)(upevt_.clct_patt_number[0]) , 0.0, 1.0); 
    HFILL(hid_fpl_pat[1][trig_req], (float)(upevt_.clct_patt_number[0]) , 0.0, 1.0); 

    }
  if(upevt_.tmb_clct_valid_patt[1])
    {  /* 2nd best */
//    printf("#%d: 2nd %d\n",event_number, upevt_.clct_half_strip[0]);
    HFILL(hid_fpl_key[0][trig_req], (float)(upevt_.clct_half_strip[1]+1.) , 0.0, 1.0); 
    HFILL(hid_fpl_key[2][trig_req], (float)(upevt_.clct_half_strip[1]+1.) , 0.0, 1.0); 
    HFILL(hid_fpl_pat[0][trig_req], (float)(upevt_.clct_patt_number[1]) , 0.0, 1.0); 
    HFILL(hid_fpl_pat[2][trig_req], (float)(upevt_.clct_patt_number[1]) , 0.0, 1.0); 

    }
  return 0;
  }



//==============================================================
void AnalysisFinish_26()
  {

  float h0_content[2* nstrips];
  int iLayer;
  int iHStrip;
  int i;

  float Time = 0.0, UngatedTriggers = 0.0, Triggers = 0.0, CorrectHZ;

//==============================================================================
  float f;
  int p=0;

//=============================================================================
 
  for (iLayer = 0; iLayer < NLAYER; iLayer++)
    {
      Time            += all_time[iLayer];
      UngatedTriggers += all_scaler[iLayer];
      Triggers        += nevents[iLayer]; 
    }
  if (Triggers > 0.0 && Time > 0.0) CorrectHZ = UngatedTriggers/Time/Triggers;
  else CorrectHZ = 1.;

  // Correction for different chambers lengths
  CorrectHZ *= chmb_len_coef;

  for(iLayer = 0; iLayer < NLAYER; iLayer++)
    {
    for(iHStrip = 1; iHStrip <= nstrips * 2; iHStrip++)
      {  
	h0_content[iHStrip-1]=CorrectHZ *HI(hid_fpl_clct[iLayer],iHStrip);
      }
    HPAK(hid_fpl_clct[iLayer],h0_content);  
    }
      
  for(i = 0; i < 3; i++)
    {
      for(p = 0; p < NLAYER; p++)
	{
	  {  
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
	    CorrectHZ=f;
//=================================================================================
	    
	    for(iHStrip = 1; iHStrip <= nstrips * 2; iHStrip++)
	      { 
		h0_content[iHStrip-1]=CorrectHZ *HI(hid_fpl_key[i][p],iHStrip); 
	      }
	    HPAK(hid_fpl_key[i][p],h0_content);  
	  }
	}
    }

  printf("Test 26 finished!\n");
   
  return;
  }
//=================================================================================
int  SaveResults_26()
  {
  FILE *fp;
  FILE *fp_bad;
  char *filename;
  int j;
  int nbadch = 0;
  char     timestring[40];
  time_t   t;


  FinalOnePagePlot_26();

/* Open result file in the temporary directory */
  filename = malloc(strlen(datadir_translation) + 
		    strlen(test_26_file_el[src_status]) + 1);
  if (filename == NULL)
    {
    printf("malloc() failed on filename.\n");
    return 1;
    }
  sprintf(filename,"%s%s",datadir_translation,test_26_file_el[src_status]);
  
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
		      strlen(test_26_file_bad_el[src_status])+1 );
    if (filename == NULL)
      {
      printf("malloc() failed on filename.\n");
      return 1;
      }
    sprintf(filename,"%s%s",datadir_translation,test_26_file_bad_el[src_status]);

/* Open result file in the temporary directory */
    fp_bad = fopen(filename, "w");
    if(fp_bad == NULL)
      {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
      }
    free(filename);

/*  Print headers */
    fprintf(fp, "--------------------------------------------------------\n");
    fprintf(fp, "Results of test 26 (CLCT Self-trigger) for %s\n", csc_name);
    fprintf(fp, 
	    "--------------------------------------------------------\n\n");
    fprintf(fp_bad, "--------------------------------------------------------\n");
    fprintf(fp_bad, "Bad channels of test 26 (CLCT Self-trigger) for %s\n", csc_name);
    fprintf(fp_bad, 
	    "--------------------------------------------------------\n\n");

    fprintf(fp, "# planes    time      ungated     total     rates\n");
    fprintf(fp, "required    (sec)     triggers    events     Hz  \n");
    fprintf(fp, "---------------------------------------------------\n");
    for (j = 0; j < 6; j++)
      {
      fprintf(fp, "    %d     %8.2e    %8.2e   %6ld    %8.2e\n", 
	      j+1, all_time[j], all_scaler[j], nevents[j], all_scaler[j]/all_time[j]);
      }
    fprintf(fp, "\n");

    fprintf(fp, "#key HalfStr  npl=1     npl=2     npl=3      npl=4      npl=5      npl=6\n");
    fprintf(fp, "-----------------------------------------------------------------------\n");

    for (j = 0; j<2*nstrips; j++)
      {
	fprintf(fp,"    %3d   %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f \n",
		j+1, halfstrip_map[0][j], halfstrip_map[1][j], halfstrip_map[2][j],
		halfstrip_map[3][j], halfstrip_map[4][j], halfstrip_map[5][j]);
      }
    fprintf(fp, "----------------------------------------------------------------\n");

    /* Count and print bad_channels */
    for (j = 2; j < 2*nstrips-2; j++)
      {
	if (halfstrip_map[3][j]<low_limit  || halfstrip_map[3][j]> high_limit )
      { 
	nbadch++;
	fprintf(fp_bad,"Key HalfStr %3d rate %9.2f is out of limit %9.2f  %9.2f\n",
		j+1, halfstrip_map[3][j], low_limit, high_limit);
      }
    }


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


  /* Save histograms */
  filename = malloc(strlen(datadir_translation) + 
			 strlen(test_26_file_his_el[src_status]) + 1);
  if (filename == NULL)
    {
    printf("malloc() failed for file_his_name.  Could not save histograms.\n");
    }
  else
    {
    sprintf(filename,"%s%s",
	    datadir_translation, test_26_file_his_el[src_status]);
    printf("Saving histograms\n");
    HRPUT(0, filename, "N");
    free(filename);    
    }
  HDELET(0);

  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp, "%s\n\n", timestring);
  fprintf(fp_bad, "%s\n\n", timestring);

  fclose(fp);
  fclose(fp_bad);
  return 0;
  }

//==============================================
void  GetCLCTHalfStrips_26()
  {

  int Strip, HalfStrip;
  int iLayer;
  int iDiStrip;
  int iBucket;
  short iTemp;

  memset(clct_halfstrips, 0, NLAYER*nstrips*2*sizeof(short)); 

  // Unpack halfstrip

  for(iLayer = 0; iLayer < NLAYER; iLayer++) 
    for(iDiStrip = 0; iDiStrip < nstrips / 2; iDiStrip++) 
      for(iBucket = 0; iBucket < (upevt_.clct_nbucket - 2); )
	{ 
	iTemp = upevt_.clct_dump[iLayer][iDiStrip]; 
	if(iTemp & (1 << iBucket))
	  {
	  Strip = (iTemp & (1 << (iBucket + 1))) ? (2 * iDiStrip + 1) : (2 * iDiStrip); 
	  HalfStrip = (iTemp & (1 << (iBucket + 2))) ? (2 * Strip + 1) : (2 * Strip);
	  clct_halfstrips[iLayer][HalfStrip] |= (1 << iBucket);
	  iBucket += 3;
	  }
	else
	  {
	    iBucket++;
	  }
	}
  }


//==========================================
void  FinalOnePagePlot_26()
  {

  char csc_name[15];
  float limits[20];
  int iLayer;
  int iHStrip;
  char plot_title[80];

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


  make_name(csc_name, csc_num, csc_type);
  printf("test_26: csc_name  %s\n", csc_name);

  for(iLayer = 0; iLayer < NLAYER; iLayer++)
    {
  //        HUNPAK(hid_fpl_key[0][iLayer],halfstrip_map[iLayer],"HIST",0);
          for(iHStrip = 0; iHStrip < nstrips*2; iHStrip++)
      {
      	halfstrip_map[iLayer][iHStrip] = HI(hid_fpl_key[0][iLayer],iHStrip+1);
      }
    }

  limits[2] = lim[src_status][3*2+0];   // HIST_4PLANE_LOW_LIMIT[src_status]
  limits[3] = lim[src_status][3*2+1];   // HIST_4PLANE_HIGH_LIMIT[src_status]
  limits[4] = .001; //.1;   /* make cut at this fraction of lower limit */
  limits[5] = .8;   /* make cut at this fraction of upper limit */
  limits[6] = 64;
 
  low_limit  = limits[2]+ limits[4]*(limits[3]-limits[2])  ;
  high_limit = limits[2]+ limits[5]*(limits[3]-limits[2])  ;
     
  for(iLayer = 0; iLayer < NLAYER; iLayer++)
    {
      limits[6+2*iLayer +1] = lim[src_status][iLayer*2];
      limits[7+2*iLayer +1] = lim[src_status][iLayer*2+1];
    }

  limits[19] = nstrips;

  sprintf(plot_title, "LCT rates, %s", src_val[src_status]);
  one_page_plot_v3(26, src_status+1, MHSTRIP_PLOT, phalfstrip_map,
		   phalfstrip_map, limits, csc_name, plot_title, 
		   "4/6 Trigger rate(Hz)",0);
  return;
  }


