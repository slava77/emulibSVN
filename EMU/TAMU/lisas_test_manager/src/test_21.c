/*
 * test_21.c (in V1.2 test_16.c)
 * Left/Right Comparator Logic test
 *
 * Author : Valeri Sytnik and Kanglin He 
 * Date   : June, 2001
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "test_params.h"     /* test config parameters */
#include "csc_parameters.h"     /* test config parameters */
#include "daq_conf.h" 
#include "csc_event.h"     /* test config parameters */
#include <math.h>
#include "cfortran.h"
#include "hbook.h"

//#include "csc_unpacker.h"
//#include "emu/daq/reader/RawDataFile.h"

//=====================

#include "misc.h"

#include <sys/time.h>
#include <time.h>

#include "test_utils.h"
#include "one_page_plot.h"
#include "test_config_struct.h"

extern test_config_struct tcs;
extern int tot_evt;


//=================================

#define FCORRECT_LOWER_LIMIT   0.95
#define FCORRECT_UPPER_LIMIT   1.05

int hid_fhstripR[NLAYER];

const int NPOINTS = 100;

static   int num_points;
static   int trigger_per_step;

short clct_halfstrips[NLAYER][NSTRIP*2];

static int event_number;
int first;

char test_21_file_el[]="/test_results/temp/test_21_01.results";
char test_21_file_his_el[]="/test_results/temp/test_21_01.his";


//=================================
void GetCLCTHalfStrips_21();
void OnePagePlot_21();   // make one_page_plot files 
void AnalysisFinish_21(); // interface to test_finish
void SaveResults_21();

int test_21_init(void);
int test_21_begin(int pass);
int test_21_event(int pass);
int test_21_end(int pass); 
int test_21_finish(void);

extern int event_number;
static int nstrips, ncfebs;

int test_21_init()
{
  
    test_begin  = test_21_begin;
    test_event  = test_21_event;
    test_end    = test_21_end;
    test_finish = test_21_finish;

    num_passes = 1;
    num_events[0] = 32000;
    event_number=0;
				   
    switch (csc_type)
    {
    case 0:  nstrips = 48; ncfebs = 3; break;
    case 1:  nstrips = 64; ncfebs = 4; break;
    case 2:  nstrips = 64; ncfebs = 4; break;
    default: nstrips = 80; ncfebs = 5; break;
    }
    printf("star test 21, nstrips, ncfebs %d%d\n",nstrips,ncfebs);

    printf ("csc_type = %d, nstrips = %d\n", csc_type, nstrips);

    return 0;

}
//=============================================================================

int    test_21_end(int pass)
  {

  return 0;
  }

int    test_21_finish()
  {

  AnalysisFinish_21();
  SaveResults_21();
  HDELET(0);
  return 0;
  }

//=====================================================
int  test_21_begin( int pass)
  { 
  int iLayer;
  char tmp[120];

  printf("****** Test 21 Start ******\n");

  first = 1;

  for(iLayer = 0; iLayer < NLAYER; iLayer++)
    {
    hid_fhstripR[iLayer]=iLayer+1;
    sprintf(tmp, "halfstrip%d", iLayer+1);
    printf(" t21b halfstrip %d\n", iLayer+1);
    HBOOK1( hid_fhstripR[iLayer] ,tmp,  2*nstrips, 0.5, 2*nstrips+0.5   ,       0.); //mod++   
    }
    
  return 0;
  }

//=====================================================
int test_21_event(int pass)
{

	int iLayer;
	int iHStrip;
	int iBucket;
	int current_hstrip;

	event_number++;

	// fill legacy params
	upevt_.stripcal_num_points = 1;
	upevt_.stripcal_trigger_per_step = tcs.t21.events_per_hstrip;
	upevt_.stripcal_current_strip = (event_number / tcs.t21.events_per_hstrip) *  tcs.t21.hstrip_step + tcs.t21.hstrip_first;

	if(event_number==1)
    {

		first = 0;
		num_points = upevt_.stripcal_num_points;
		trigger_per_step = upevt_.stripcal_trigger_per_step;
		//printf("cal_num_points %d",upevt_.stripcal_num_points);
		//printf(" trig_set %d",trigger_per_step);
		//printf( "cur strip %d\n ",upevt_.stripcal_current_strip);
    }
   
   
	if(! upevt_.clct_active) 
	{
		printf("event: %d -- clct not active\n", event_number);
		return 0;
	}
 
	//GetCLCTHalfStrips_21();
   
	current_hstrip = upevt_.stripcal_current_strip;

	for(iLayer = 0; iLayer < NLAYER; iLayer++)
    {
		for(iHStrip = 0; iHStrip < nstrips*2; iHStrip++)
		{
		   clct_halfstrips[iLayer][iHStrip]=upevt_.clct_dump_halfstrips[iLayer][iHStrip];
			for(iBucket = 0; iBucket < upevt_.clct_nbucket; iBucket++)
			{
				if(clct_halfstrips[iLayer][iHStrip] & (1 << iBucket))
				{ 
					if(iHStrip % 32 + 1 == current_hstrip )
					{
						HFILL(hid_fhstripR[iLayer], (float )(iHStrip+1), 0.0, 1.0); //mod++			               

						/*	printf
					    (
							"correct Evt# %d, layer %d, get HalfStrip - %d, expected HalfStrip - %d, filled layer %d, strip %d\n", 
							event_number, 
							iLayer, 
							iHStrip %32 + 1, 
							current_hstrip,
							hid_fhstripR[iLayer],
							(float )(iHStrip+1)
							);*/
					} 
					else 
					{
					  /*
						printf
					    (
							"Evt# %d, layer %d, get HalfStrip - %d, expected HalfStrip - %d\n", 
							event_number, 
							iLayer, 
							iHStrip %32 + 1, 
							current_hstrip
							);
					  */
					}

				}
			}
		}
    }
   
	return 0;
}


//==============================================================
void  AnalysisFinish_21()
  {
//////   TH1F *ftmp;
  float h_content[nstrips*2];   
  int iLayer;
  int iHStrip; 

  for(iLayer = 0; iLayer < NLAYER; iLayer++)
    {

    for(iHStrip = 1; iHStrip <= nstrips*2; iHStrip++)
      { 

      h_content[iHStrip-1]=HI(hid_fhstripR[iLayer],iHStrip)/((float)(trigger_per_step*num_points));
      }
    HPAK(hid_fhstripR[iLayer],h_content); 

    }
   
  OnePagePlot_21();
   
  printf("Test 21 finished!\n");
  return;
  }

//===============================================================

void  SaveResults_21()
  {
  int     badchan;
  time_t  t;
  char    timestring[40];
  int iLayer;
  int iHStrip;
  float eff;
  char *filename, *file_his_name;
  FILE* fp;

  filename = malloc(strlen(datadir_translation) + 
		    strlen(test_21_file_el) + 1 );
  if (filename == NULL)
    {
    printf("malloc() failed on filename.\n");
    return; /* void prototype prohibits error signal */
    }
  sprintf(filename,"%s%s",datadir_translation,test_21_file_el);

/* Open result file in the temporary directory */
  fp = fopen(filename, "w");
  if(fp == NULL)
    {
    printf("File %s could not be opened.\n",filename);
    free(filename);
    return; /* void prototype prohibits error signal */
    }
  free(filename);

  fprintf(fp, "----------------------------------\n");
  fprintf(fp, "Results of test 21 for %s\n", csc_name);
  fprintf(fp, "----------------------------------\n\n");
  badchan = 0;

  fprintf(fp, "Bad halfstrips (fraction correct < %4.2f or > %4.2f):\n",
	  FCORRECT_LOWER_LIMIT, FCORRECT_UPPER_LIMIT); 
  fprintf(fp, "------------------------------------\n");
  fprintf(fp, "Layer   Halfstrip   Fraction correct\n");
  fprintf(fp, "------------------------------------\n");
  for(iLayer = 0; iLayer < NLAYER; iLayer++)
    {
    for(iHStrip = 2; iHStrip <= 2*nstrips-1; iHStrip++)
      {
	
      eff=HI(hid_fhstripR[iLayer],iHStrip);    // mod++
      if(eff < FCORRECT_LOWER_LIMIT || eff > FCORRECT_UPPER_LIMIT)
	{
	badchan++;
	fprintf(fp, "  %d       %3d          %8.3f \n", iLayer+1, iHStrip, 
		eff);
	}
      }
    }
  fprintf(fp, "------------------------------------\n");
   
  fprintf(fp, "\n");

/* Mandatory summary statement */
  fprintf(fp, "Total number of bad halfstrips = %d\n", badchan);
  fprintf(fp, "\n");
  if (badchan > 0) fprintf(fp, "Test result = Failed\n\n");
  else fprintf(fp, "Test result = OK\n\n");

/* Add the date and time */
  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp, "%s\n", timestring);

  fclose(fp);
 
  /* Save histograms */
  file_his_name = malloc(strlen(datadir_translation) + 
			 strlen(test_21_file_his_el) + 1);
  if (file_his_name == NULL)
    {
    printf("malloc() failed for file_his_name.  Could not save histograms.\n");
    }
  else
    {
    sprintf(file_his_name,"%s%s",
	    datadir_translation,test_21_file_his_el);
    printf("Saving histograms\n");
    HRPUT(0, file_his_name, "N");
    free(file_his_name);    
    }
  }
   

//==============================================
void  GetCLCTHalfStrips_21()
{

	int Strip, HalfStrip;
	int iLayer;
	int iDiStrip;
	int iBucket;
	short iTemp;


	memset(clct_halfstrips, 0, NLAYER*NSTRIP*2*sizeof(short)); 

	// Unpack halfstrips and strips

	for(iLayer = 0; iLayer < NLAYER; iLayer++){ 
	  for(iDiStrip = 0; iDiStrip < nstrips / 2; iDiStrip++){ 
			for(iBucket = 0; iBucket < (upevt_.clct_nbucket - 2); )
			{ 
				iTemp = upevt_.clct_dump[iLayer][iDiStrip]; 
				if(iTemp & (1 << iBucket))
				{
				  Strip = (iTemp & (1 << (iBucket + 1))) ? (2 * iDiStrip + 1) : (2 * iDiStrip); //wrong at the moment
				  printf("bit shift %d, iTemp thingy  %d, bool %d",(1 << (iBucket + 1)),iTemp,(iTemp & (1 << (iBucket + 1))));
					HalfStrip = (iTemp & (1 << (iBucket + 2))) ? (2 * Strip + 1) : (2 * Strip);//to be checked
					clct_halfstrips[iLayer][HalfStrip] |= (1 << iBucket);//works?
					printf("layer %d, distrip %d ,nstrip/2 %d,iBucket_max %d, iBucket %d, clct_halfstrips %d, clct-Strip %d,clct-Halfstrip %d\n",iLayer,iDiStrip, nstrips/2, upevt_.clct_nbucket - 2, iBucket, clct_halfstrips[iLayer][HalfStrip], Strip,HalfStrip);
					iBucket += 3;
				}
				else
				{
				  //printf("should we get here,iBucket?%d\n",iBucket);
					iBucket++;
				}
	  
			}
	  }
	}
}

//==========================================
void  OnePagePlot_21()
{

    char csc_name[15];
    float strip_map[NLAYER][nstrips*2];
    float limits[7];
    int iLayer;
    int iHStrip;

    make_name(csc_name, csc_num, csc_type);
    printf("test_21: csc_name  %s\n", csc_name);

    for(iLayer = 0; iLayer < NLAYER; iLayer++)
	for(iHStrip = 0; iHStrip < nstrips*2; iHStrip++)
	{

	    strip_map[iLayer][iHStrip] = HI(hid_fhstripR[iLayer],iHStrip+1); //mod++
	}
    limits[0] = 0.01;            
    limits[1] = 1.10;
    limits[2] = 0.01;
    limits[3] = 1.10;
    limits[4] = FCORRECT_LOWER_LIMIT;
    limits[5] = FCORRECT_UPPER_LIMIT;
    limits[6] = nstrips;

    one_page_plot_v3(21, 1, HSTRIP_PLOT, (float*)strip_map, (float*)strip_map, limits, csc_name, 
		     "fraction of correct responses", 
		     "fraction correct",0);

}
