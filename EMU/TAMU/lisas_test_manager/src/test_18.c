/*
 * test_18.c (in V1.2 test_13.c)
 * Comparator Noise test
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
//=====================

#include "misc.h"

#include <sys/time.h>
#include <time.h>

#include "test_utils.h"
#include "one_page_plot.h"

#include "test_18_ini.h"
#include "test_config_struct.h"

//#include "csc_unpacker.h"
//#include "emu/daq/reader/RawDataFile.h"
extern test_config_struct tcs;
extern int tot_evt;


//================================

static   int TotalTriggerNumber;
static   int TotalUngatedTriggerNumber;
static   struct timeval TotalRunTime;

// unpacked the half-strip data
short clct_halfstrips[NLAYER][NSTRIP*2];
short clct_strips[NLAYER][NSTRIP];
   
static int event_number;

int hv_status;                      // hv for this run, status=0 HV=3600, status=1 hv=3800
                                    // hv for this run, status=2 HV=3600 odd planes
                                    // hv for this run, status=3 HV=3600 even planes

float chamb_coef_list[7] = CHAMB_LEN_COEF;

static char * hv_val[4] ={ "3600 V","3800 V", "3600 V odd", "3600 V even"};

char * test_18_file_hv_el[4]= {"/test_results/temp/test_18_01.results",
                               "/test_results/temp/test_18_02.results",
                               "/test_results/temp/test_18_03.results",
                               "/test_results/temp/test_18_04.results"};
char * test_18_file_his_el[4]={"/test_results/temp/test_18_01.his",
			       "/test_results/temp/test_18_02.his",
			       "/test_results/temp/test_18_03.his",
			       "/test_results/temp/test_18_04.his"};
char * test_18_file_bad_el[4]={"/test_results/temp/test_18_01.bad",
			       "/test_results/temp/test_18_02.bad",
			       "/test_results/temp/test_18_03.bad",
			       "/test_results/temp/test_18_04.bad"};

static float chmb_len_coef;
static int nstrips;
//=================================

void OnePagePlot_18();
void AnalysisFinish_18(); // interface to test_finish
int SaveResults_18();   
void GetCLCTHalfStrips_18();

int test_18_init(void);
int test_18_begin(int pass);
int test_18_event(int pass);
int test_18_end(int pass); 
int test_18_finish(void);


int test_18_init()
  {
  test_begin  = test_18_begin;
  test_event  = test_18_event;
  test_end    = test_18_end;
  test_finish = test_18_finish;

  num_passes = 1;
  num_events[0] = 3200000;				   
  
  event_number = 0;
  TotalTriggerNumber = TotalUngatedTriggerNumber = 0;
  memset(&TotalRunTime, 0, sizeof(struct timeval));

    switch (csc_type)
    {
    case 0:  nstrips = 48; break;
    case 1:  nstrips = 64; break;
    case 2:  nstrips = 64; break;
    default: nstrips = 80; break;
    }

    printf ("csc_type = %d, nstrips = %d\n", csc_type, nstrips);
  /*
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
  */
  chmb_len_coef = chamb_coef_list[csc_type];
  printf ("csc_type = %d, Chamber length correction coefficient = %.3f\n", csc_type, chmb_len_coef);

  return 0;
  }
//=============================================================================

int test_18_end(int pass)
  {
  return 0;
  }

int test_18_finish()
  {
  AnalysisFinish_18();
  SaveResults_18();
  
  HDELET(0);   
  return 0;
  }

//====================================================

int test_18_begin(int pass)
  { 
  int iLayer;
  char tmp[120];
  int hid;
  int j;

  printf("****** Test 18 Start ******\n");
  do
    {
      printf("Where and what was  the high for this run \n");
      printf("\t0-3600 \n\t1-3800  \n\t2-3600 odd\n\t3-3600 even) ? \n");
      getint(&j);
      hv_status=j; 
    } while (hv_status <0 || hv_status >3);

  printf("you entered hv_status %d for HV=%s\n",hv_status, hv_val[hv_status]);

  for(iLayer = 0; iLayer < NLAYER; iLayer++) 
    {
    hid=iLayer+1;
    sprintf(tmp, "Strip L%d", iLayer+1);
    HBOOK1( hid,tmp , nstrips, 0.5, nstrips+0.5 , 0.);

    sprintf(tmp, "Half Strip L%d", iLayer+1);
    HBOOK1( NLAYER+hid,tmp , 2*nstrips, 0.5,2*nstrips+0.5 , 0.);
    }
  return 0;
  }

//=====================================================

int test_18_event(int pass)
{
	static int last_trigger_number;
	static int last_ungated_trigger_number;
	static struct timeval last_time;
	int current_trigger_number;
	int current_ungated_trigger_number;
	struct timeval current_time;
	int DeltaTriggerNumber;
	int DeltaUngatedTriggerNumber;
	struct timeval DeltaTime;
	struct tm lct;

	static int first = 1;
   
	int iLayer;
	int iStrip;
	int iBucket;
	int iHStrip;

	event_number++;
	// fill legacy params
	upevt_.trigger_number = event_number;
	upevt_.num_ungated_triggers = event_number; // no gating anymore
   
	if(event_number==1) 
    {
		last_trigger_number = upevt_.trigger_number;
		last_ungated_trigger_number = upevt_.num_ungated_triggers;
      
		lct.tm_year = ((upevt_.time_YYMM) >> 8) & 0xff;
		lct.tm_mon = ((upevt_.time_YYMM) & 0xff) - 1;
		lct.tm_mday = ((upevt_.time_DDHH) >> 8) & 0xff;
		lct.tm_hour = (upevt_.time_DDHH) & 0xff;
		lct.tm_min = ((upevt_.time_MMSS) >> 8) & 0xff;
		lct.tm_sec = (upevt_.time_MMSS) & 0xff;
      
		last_time.tv_sec = mktime(&lct);
		last_time.tv_usec = upevt_.time_musec;
      
		first = 0;

		return 0;
    }
   
	current_trigger_number = upevt_.trigger_number;
	current_ungated_trigger_number = upevt_.num_ungated_triggers;

	lct.tm_year = ((upevt_.time_YYMM) >> 8) & 0xff;
	lct.tm_mon = ((upevt_.time_YYMM) & 0xff) - 1;
	lct.tm_mday = ((upevt_.time_DDHH) >> 8) & 0xff;
	lct.tm_hour = (upevt_.time_DDHH) & 0xff;
	lct.tm_min = ((upevt_.time_MMSS) >> 8) & 0xff;
	lct.tm_sec = (upevt_.time_MMSS) & 0xff;
      
	current_time.tv_sec = mktime(&lct);
	current_time.tv_usec = upevt_.time_musec;
   
	timersub(&current_time, &last_time, &DeltaTime);
	DeltaTriggerNumber = current_trigger_number - last_trigger_number;
	DeltaUngatedTriggerNumber =
		current_ungated_trigger_number - last_ungated_trigger_number;
   

	timeradd(&TotalRunTime, &DeltaTime, &TotalRunTime);
	TotalTriggerNumber += DeltaTriggerNumber;
	TotalUngatedTriggerNumber += DeltaUngatedTriggerNumber;

 
	last_time = current_time;
	last_trigger_number = current_trigger_number;
	last_ungated_trigger_number = current_ungated_trigger_number;

	if(! upevt_.clct_active) return 0;
   
	GetCLCTHalfStrips_18();
   
	for(iLayer = 0; iLayer < NLAYER; iLayer++) 
    {
		for(iStrip = 0; iStrip < nstrips; iStrip++) 
		{  
			for(iBucket = 0; iBucket < upevt_.clct_nbucket; iBucket++)  
				if(clct_strips[iLayer][iStrip] & (1 << iBucket))
				{ 
					HFILL(iLayer+1, (float )(iStrip+1.), 0.0, 1.0); 
				}
		}
		for(iHStrip = 0; iHStrip < 2*nstrips; iHStrip++) 
		{  
			for(iBucket = 0; iBucket < upevt_.clct_nbucket; iBucket++)  
				if(clct_halfstrips[iLayer][iHStrip] & (1 << iBucket))
				{ 
					HFILL(NLAYER+iLayer+1, (float )(iHStrip+1.), 0.0, 1.0); 
				}
		}
    }
	return 0;
}


//==============================================================

void AnalysisFinish_18() 
{
	int iLayer;
	int iStrip;
	int iHStrip;
   
	float y[nstrips];
	float y2[2*nstrips];
	float Time;
	// = (float)TotalRunTime.tv_sec + (float)(TotalRunTime.tv_usec)/1000000.;
	float Factor = (float)TotalUngatedTriggerNumber / (float)TotalTriggerNumber;
	float CorrectHZ;
	// Not Good Idea result mean single rate value multiplied by chmb_len_coef
	// float CorrectHZ = (1. / Time) * Factor*chmb_len_coef;
	int j;

	printf("Please input number of seconds it took to collect events: \n");
	getint(&j);
	Time = (float)j;
	CorrectHZ = (1. / Time) * Factor;
   
	for(iLayer = 0; iLayer < NLAYER; iLayer++) 
    {
		HUNPAK(iLayer+1,y,"HISTS",0);        
		for(iStrip = 1; iStrip <= nstrips ; iStrip++)
		{  
			y[iStrip-1]*=CorrectHZ;
		}
		HPAK(iLayer+1,y);
		HUNPAK(NLAYER+iLayer+1,y2,"HISTS",0);
		for(iHStrip = 1; iHStrip <= 2*nstrips ; iHStrip++)
		{  
			y2[iHStrip-1]*=CorrectHZ;
		}
		HPAK(NLAYER+iLayer+1,y2);
    }
	OnePagePlot_18();
  
	printf("Test 18 finished!\n");
	return;
}
//==========================================

int SaveResults_18() 
  {
  time_t  t;
  char    timestring[40];
  FILE    *fp;
  FILE    *fp_bad;
  float y[NLAYER][nstrips+1];
  char *filename;

  int iLayer;
  int iStrip;
  int BadChan;
  float TrigRate;
  char *file_his_name;

  float   rate_lower_limit[4]  = { SINGLES_RATE_LOWER_LIMIT0/chmb_len_coef, 
				   SINGLES_RATE_LOWER_LIMIT1/chmb_len_coef,
                                   SINGLES_RATE_LOWER_LIMIT2/chmb_len_coef,
				   SINGLES_RATE_LOWER_LIMIT3};
  float   rate_upper_limit[4]  = { SINGLES_RATE_UPPER_LIMIT0/chmb_len_coef, 
				   SINGLES_RATE_UPPER_LIMIT1/chmb_len_coef,
				   SINGLES_RATE_UPPER_LIMIT2/chmb_len_coef,
				   SINGLES_RATE_UPPER_LIMIT3};

  float   rate_HV_up_limit[4]    = {  SINGLES_HIGH_HV_LIMIT0, 
				      SINGLES_HIGH_HV_LIMIT1,
				      SINGLES_HIGH_HV_LIMIT2/chmb_len_coef,
				      SINGLES_HIGH_HV_LIMIT3/chmb_len_coef};

  float   rate_noHV_up_limit[4]  = {  SINGLES_HIGH_NOHV_LIMIT0, 
				      SINGLES_HIGH_NOHV_LIMIT1,
				      SINGLES_HIGH_NOHV_LIMIT2,
				      SINGLES_HIGH_NOHV_LIMIT3};
				      
 float   rate_HV_low_limit[4]    = {  SINGLES_LOW_HV_LIMIT0, 
				      SINGLES_LOW_HV_LIMIT1,
				      SINGLES_LOW_HV_LIMIT2,
				      SINGLES_LOW_HV_LIMIT3};

  float   rate_noHV_low_limit[4]  = {  SINGLES_LOW_NOHV_LIMIT0/chmb_len_coef, 
				       SINGLES_LOW_NOHV_LIMIT1/chmb_len_coef,
				       SINGLES_LOW_NOHV_LIMIT2,
				       SINGLES_LOW_NOHV_LIMIT3};



    filename = malloc(strlen(datadir_translation) + 
		      strlen(test_18_file_hv_el[hv_status]) + 1 );
    if (filename == NULL)
      {
      printf("malloc() failed on filename.\n");
      return 1;
      }
    sprintf(filename,"%s%s",datadir_translation,test_18_file_hv_el[hv_status]);

    /* Open result file in the temporary directory */
    fp = fopen(filename, "w");
    if(fp == NULL)
      {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
      }
    free(filename);

    fprintf(fp, "--------------------------------------------\n");
    fprintf(fp, "Results of test 18 for %s -- HV= %s V\n", csc_name, hv_val[hv_status]);
    fprintf(fp, "--------------------------------------------\n\n");

    filename = malloc(strlen(datadir_translation) + 
		      strlen(test_18_file_bad_el[hv_status]) +1 );
    if (filename == NULL)
      {
      printf("malloc() failed on filename.\n");
      return 1;
      }
    sprintf(filename,"%s%s",datadir_translation,test_18_file_bad_el[hv_status]);

    /* Open result file in the temporary directory */
    fp_bad = fopen(filename, "w");
    if(fp_bad == NULL)
      {
      printf("File %s could not be opened.\n",filename);
      free(filename);
      return 1;
      }
    free(filename);

    fprintf(fp_bad, "-------------------------------------------\n");
    fprintf(fp_bad, "Results of test 18 for %s -- HV= %s\n", csc_name, hv_val[hv_status]);
    fprintf(fp_bad, "-------------------------------------------\n\n");


  fprintf(fp_bad, "List of bad channels\n (rate outside the range %5.2f - %5.2f Hz):"
	  "\n", rate_lower_limit[hv_status], rate_upper_limit[hv_status]);
  fprintf(fp, "Layer  Strip   Trigger rate (Hz)\n"); 
  fprintf(fp, "--------------------------------\n");
  fprintf(fp_bad, "Layer  Strip   Trigger rate (Hz)\n");
  fprintf(fp_bad, "--------------------------------\n");

  BadChan = 0;
  for(iLayer = 0; iLayer < NLAYER; iLayer++) 
    {
    HUNPAK(iLayer+1,y[iLayer],"HIST",0);

    for(iStrip = 1; iStrip <= nstrips; iStrip++) 
      {
      TrigRate = y[iLayer][iStrip-1];
      //	fprintf(fp, "  %1d      %2d      %8.2f\n", iLayer+1, iStrip, TrigRate);
	switch (hv_status)
	  {
	  case (0):
	  case (1):
	    {
	      if(TrigRate < rate_lower_limit[hv_status] || TrigRate > rate_upper_limit[hv_status])
		{
		  BadChan++;
		  fprintf(fp_bad, "  %1d      %2d      %8.2f\n", iLayer+1, iStrip, TrigRate);
		}
	      break;
	    }
	  case (2): //HV=3600 on odd planes iLayer =0,2,4
	    {
	      if( ((iLayer%2)==0) &&
		 ( TrigRate < rate_HV_low_limit[hv_status] ||
		   TrigRate > rate_HV_up_limit[hv_status]) )
		{
		  BadChan++;
		  fprintf(fp_bad, "  %1d      %2d      %8.2f\n", iLayer+1, iStrip, TrigRate);
		}
	      if( ((iLayer%2)==1) && // layers without HV
		  ( TrigRate > rate_noHV_up_limit[hv_status]))
		{
		  BadChan++;
		  fprintf(fp_bad, "noHV %1d      %2d      %8.2f\n", iLayer+1, iStrip, TrigRate);
		}

	      break;
	    }
	  case(3): // HV=3600 on even planes
	    {
	      if( ((iLayer%2)==1) &&
		  ( TrigRate < rate_HV_low_limit[hv_status] ||
		   TrigRate > rate_HV_up_limit[hv_status]))
		{
		  BadChan++;
		  fprintf(fp_bad, "  %1d      %2d      %8.2f\n", iLayer+1, iStrip, TrigRate);
		}
	      if( ((iLayer%2)==0) && // layers without HV
		  ( TrigRate > rate_noHV_up_limit[hv_status]))
		{
		  BadChan++;
		  fprintf(fp_bad, "noHV %1d      %2d      %8.2f\n", iLayer+1, iStrip, TrigRate);
		}
	      break;
	    }
	  }
      }
    
    }

  for(iStrip = 0; iStrip < nstrips; iStrip++) 
    {
      fprintf(fp, " %2d  %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f\n", iStrip+1,
	      y[0][iStrip], y[1][iStrip], y[2][iStrip],
	      y[3][iStrip], y[4][iStrip], y[5][iStrip]);
    }
  
  fprintf(fp, "--------------------------------\n");
  fprintf(fp, "Total number of bad channels = %d\n\n", BadChan);

  fprintf(fp_bad, "--------------------------------\n");
  fprintf(fp_bad, "Total number of bad channels = %d\n\n", BadChan);


  if (BadChan > 0) 
    {
      fprintf(fp,     "Test result = Failed\n\n");
      fprintf(fp_bad, "Test result = Failed\n\n");
    }
  else
    {
      fprintf(fp,     "Test result = OK\n\n");
      fprintf(fp_bad, "Test result = OK\n\n");
    }
/* Add the date and time */
  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp, "%s\n", timestring);
  fprintf(fp_bad, "%s\n", timestring);

  fclose(fp);
  fclose(fp_bad);
   
  file_his_name = malloc(strlen(datadir_translation) + 
			 strlen(test_18_file_his_el[hv_status]) + 1);
  if (file_his_name == NULL)
    {
    printf("malloc() failed for file_his_name.  Could not save histograms.\n");
    }
  else
    {
    sprintf(file_his_name,"%s%s",
	    datadir_translation,test_18_file_his_el[hv_status]);
    printf("Saving histograms\n");
    HRPUT(0, file_his_name, "N");
    free(file_his_name);    
    }
  return 0;
  }
//==============================================
void  GetCLCTHalfStrips_18() 
  {

  int Strip, HalfStrip;
  int iLayer;
  int iDiStrip;
  int iBucket;
  short iTemp;


  memset(clct_strips, 0, NLAYER*NSTRIP*sizeof(short)); 
  memset(clct_halfstrips, 0, NLAYER*NSTRIP*2*sizeof(short)); 

  // Unpack halfstrip

  for(iLayer = 0; iLayer < NLAYER; iLayer++) 
    for(iDiStrip = 0; iDiStrip < nstrips / 2; iDiStrip++) 
      for(iBucket = 0; iBucket < (upevt_.clct_nbucket - 2); ) 
	{ 
	iTemp = upevt_.clct_dump[iLayer][iDiStrip]; 
	if(iTemp & (1 << iBucket)) 
	  {
	  Strip = (iTemp & (1 << (iBucket + 1))) ? (2 * iDiStrip + 1) : (2 * iDiStrip);
	  clct_strips[iLayer][Strip] |= (1 << iBucket);
	  HalfStrip = (iTemp & (1 << (iBucket + 2))) ? (2 * Strip + 1) : (2 * Strip);
	  clct_halfstrips[iLayer][HalfStrip] |= (1 << iBucket);
	  iBucket += 3;
	  } else 
	    {
	    iBucket++;
	    }
	  
	}
  }

//==========================================
void  OnePagePlot_18() 
  {

  float y[nstrips+1];
  int  plot_num;
  char csc_name[15];
  char *plot_title;

  float strip_map[NLAYER][nstrips];

  float limits[18];

  float   lim[4][6] = { 
    { HIST_LAYER_LOW_LIMIT0,      HIST_LAYER_HIGH_LIMIT0/chmb_len_coef,
      HIST_SUMMARY_LOW_LIMIT0,    HIST_SUMMARY_HIGH_LIMIT0/chmb_len_coef,   
      SINGLES_RATE_LOWER_LIMIT0/chmb_len_coef,  SINGLES_RATE_UPPER_LIMIT0/chmb_len_coef},
    { HIST_LAYER_LOW_LIMIT1,      HIST_LAYER_HIGH_LIMIT1/chmb_len_coef,
      HIST_SUMMARY_LOW_LIMIT1,    HIST_SUMMARY_HIGH_LIMIT1/chmb_len_coef,
      SINGLES_RATE_LOWER_LIMIT1/chmb_len_coef,  SINGLES_RATE_UPPER_LIMIT1/chmb_len_coef},
    //paramters for planes with HV
    { HIST_LAYER_LOW_LIMIT2,      HIST_LAYER_HIGH_LIMIT2/chmb_len_coef,
      HIST_SUMMARY_LOW_LIMIT2,    HIST_SUMMARY_HIGH_LIMIT2/chmb_len_coef,
      SINGLES_RATE_LOWER_LIMIT2/chmb_len_coef,  SINGLES_RATE_UPPER_LIMIT2/chmb_len_coef},
    //paramters for planes without HV
    { HIST_LAYER_LOW_LIMIT3,      HIST_LAYER_HIGH_LIMIT3,
      HIST_SUMMARY_LOW_LIMIT3,    HIST_SUMMARY_HIGH_LIMIT3,
      SINGLES_RATE_LOWER_LIMIT3,  SINGLES_RATE_UPPER_LIMIT3} };

  int iLayer, iStrip;
  int i;

  make_name(csc_name, csc_num, csc_type);
  printf("test_18: csc_name  %s\n", csc_name);

  for(iLayer = 0; iLayer < NLAYER; iLayer++) 
    {
      HUNPAK(iLayer+1,y,"HIST",0);

      for(iStrip = 0; iStrip < nstrips; iStrip++)
	strip_map[iLayer][iStrip] = y[iStrip];
    
    }
   
    plot_num = hv_status+1;
    printf(" cscname: %s\n",csc_name);
    plot_title = malloc(strlen("Comparator Noise test, trigger rate,\n HV= ")+
			strlen(hv_val[hv_status])+1);
    sprintf(plot_title, "Comparator Noise test, trigger rate,\n HV= %s", hv_val[hv_status]);
    switch (hv_status)
      {
      case (0):
      case (1):
	  for (i=0; i<6; i++){limits[i] = lim[hv_status][i];}
	  limits[6] = nstrips;
	  one_page_plot_v3(18, plot_num, STRIP_PLOT, (float*)strip_map, (float*)strip_map, limits,
			 csc_name, plot_title, "rate (Hz)",0);
	
	break;

	// cases 2 and 3 will work icorrectly with ME1/3. But they are not used now anyway.
      case(2):
	for (i=0; i<6; i++)
	  {
	    limits[i]    = lim[0][i];
	    limits[i+6]  = lim[2][i];
	    limits[i+12] = lim[3][i];
	  }
	  one_page_plot_v3(18, plot_num, STRIP_PLOT_ODD,
			   (float*)strip_map, (float*)strip_map, limits,
			   csc_name, plot_title, "rate (Hz)",0);
	  break;
      case(3):
	for (i=0; i<6; i++)
	  {
	    limits[i]    = lim[0][i];
	    limits[i+ 6] = lim[2][i];
	    limits[i+12] = lim[3][i];
	  }
	  one_page_plot_v3(18, plot_num, STRIP_PLOT_EVEN,
			   (float*)strip_map, (float*)strip_map, limits,
			   csc_name, plot_title, "rate (Hz)",0);
	break;
      }
 
  return;
  }


