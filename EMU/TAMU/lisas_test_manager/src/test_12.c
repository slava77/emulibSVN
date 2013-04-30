/*
 * test_12.c (in V1.2 test_7.c)
 * AFEB Connectivity Test
 *
 * Author : Valentin Sulimov      
 * Date   : 18 Sept. 2000
 * 
 * Last Change : Oct.22 2001
 * Different output files S.D.
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
#include "daq_histo.h"       /* PED_ID                 */
#include "daq_module.h"      /* map_.AFEB              */
#include "misc.h"            /* definition of logical data type */
#include "one_page_plot.h"   /* STRIP_PLOT,...         */
#include "test_utils.h"      /* chamber labels         */
#include "test_params.h"     /* test config parameters */
#include "test_12_ini.h"     /* limits for test 12 */ 
#include "test_config_struct.h"

//#include "csc_unpacker.h"
//#include "emu/daq/reader/RawDataFile.h"

/* Local function declarations */
int test_12_begin(int pass);
int test_12_event(int pass);
int test_12_end(int pass);
int test_12_finish(void);

/* Global variables , have to be in some *.h file  SD*/
extern char  number_of_run[80];
int tot_evt;
test_config_struct tcs; // test configuration from test_config.xml

/* Global variables local to this test */
static FILE    *fp1, *fp2, *fp3, *fp_bad;
static int     nevents[6], nevents1[6], nevents2[6];

static char * fp1_name_el      = "/test_results/temp/test_12_01.results";
static char * fp2_name_el      = "/test_results/temp/test_12_02.results";
static char * fp3_name_el      = "/test_results/temp/test_12_03.results";
static char * fp_bad_name_el   = "/test_results/temp/test_12_04.bad";
static char * file_his_name_el = "/test_results/temp/test_12_01.his";
static int nwires;

/*******************************************************************************
   Define functions and parameters for FAST site test 12
*******************************************************************************/

int test_12_init() 
  {
    printf("before test begin\n");
  test_begin  = test_12_begin;
  printf("after test begin\n");
  test_event  = test_12_event;
  printf("after test event\n");
  test_end    = test_12_end;
  test_finish = test_12_finish;

  num_passes = 1;
  num_events[0] = 100000;

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

  printf ("csc_type = %d, nwires = %d\n", csc_type, nwires);
  return 0;
  }

/*******************************************************************************
   Book histograms for FAST site test 12                                       
*******************************************************************************/

int test_12_begin(int pass) 
{
	int     hid, ilayer;
	char    htitle[80];
	char    *fp1_name, *fp2_name, *fp3_name, *fp_bad_name;

	printf("****** Test 12 Start ******\n");
 
	/* create the names */
	fp1_name = malloc(strlen(datadir_translation) + 
					  strlen(fp1_name_el) + 1);
	if (fp1_name == NULL)
    {
		printf("malloc() failed for fp1_name.\n");
		return -1;
    }
	sprintf(fp1_name,"%s%s",datadir_translation,fp1_name_el);
	fp2_name = malloc(strlen(datadir_translation) + 
					  strlen(fp2_name_el) + 1);
	if (fp2_name == NULL)
    {
		free(fp1_name);
		printf("malloc() failed for fp2_name.\n");
		return -1;
    }
	sprintf(fp2_name,"%s%s",datadir_translation,fp2_name_el);
	fp3_name = malloc(strlen(datadir_translation) + 
					  strlen(fp3_name_el) + 1);
	if (fp3_name == NULL)
    {
		free(fp1_name);
		free(fp2_name);
		printf("malloc() failed for fp3_name.\n");
		return -1;
    }
	sprintf(fp3_name,"%s%s",datadir_translation,fp3_name_el);
	fp_bad_name = malloc(strlen(datadir_translation) + 
						 strlen(fp_bad_name_el) + 1);
	if (fp_bad_name == NULL)
    {
		free(fp1_name);
		free(fp2_name);
		free(fp3_name);
		printf("malloc() failed for fp_bad_name.\n");
		return -1;
    }
	sprintf(fp_bad_name,"%s%s",datadir_translation,fp_bad_name_el);

/* Open result file in the temporary directory for efficiency */

	fp1 = fopen(fp1_name, "w");
	if (fp1 == 0)
    {
		printf("File %s could not be opened.\n",fp1_name);
		free(fp1_name);
		free(fp2_name);
		free(fp3_name);
		free(fp_bad_name);
		return -1;
    }
	free(fp1_name);
	fprintf(fp1, "-------------------------------------------------------\n");
	fprintf(fp1, "Results of test 12 (AFEB Connectivity) for %s\n", csc_name);
	fprintf(fp1, "-------------------------------------------------------\n\n");

/* Open result file in the temporary directory for cross-talk of pair-plane */

	fp2 = fopen(fp2_name, "w");
	if (fp2 == 0)
    {
		printf("File %s could not be opened.\n",fp2_name);
		fclose(fp1);
		free(fp2_name);
		free(fp3_name);
		free(fp_bad_name);
		return -1;
    }
	free(fp2_name);
	fprintf(fp2, "-------------------------------------------------------\n");
	fprintf(fp2, "Results of test 12 (AFEB Connectivity) for %s\n", csc_name);
	fprintf(fp2, "-------------------------------------------------------\n\n");

/* Open result file in the temporary directory for cross-talk of non-pair-plane */

	fp3 = fopen(fp3_name, "w");
	if (fp3 == 0)
    {
		printf("File %s could not be opened.\n",fp3_name);
		fclose(fp1);
		fclose(fp2);
		free(fp3_name);
		free(fp_bad_name);
		return -1;
    }
	free(fp3_name);
	fprintf(fp3, "-------------------------------------------------------\n");
	fprintf(fp3, "Results of test 12 (AFEB Connectivity) for %s\n", csc_name);
	fprintf(fp3, "-------------------------------------------------------\n\n");

/* Open result file in the temporary directory for bad channels */

	fp_bad = fopen(fp_bad_name, "w");
	if (fp_bad == 0)
    {
		printf("File %s could not be opened.\n",fp_bad_name);
		fclose(fp1);
		fclose(fp2);
		fclose(fp3);
		free(fp_bad_name);
		return -1;
    }
	free(fp_bad_name);
	fprintf(fp_bad, "-------------------------------------------------------\n");
	fprintf(fp_bad, "Results of test 12 (AFEB Connectivity) for %s\n", csc_name);
	fprintf(fp_bad, "-------------------------------------------------------\n\n");

/* Zero arrays */

	for (ilayer = 0; ilayer < NLAYER; ilayer++) {
		nevents[ilayer] = 0;
		nevents1[ilayer] = 0;
		nevents2[ilayer] = 0;
	}

/* Book histograms for CSC wires */

	for (ilayer = 0; ilayer < NLAYER; ilayer++) 
	{
		hid = ilayer + 1;
		sprintf( htitle, "L%d Expected wg hits", ilayer + 1);
		HBOOK1( hid, htitle, nwires, 0.5, (float)nwires+0.5, 0.0);

		hid = 10 + ilayer + 1;
		sprintf( htitle, "L%d Crosstalk hits in pair layer", ilayer + 1);
		HBOOK1( hid, htitle, nwires, 0.5, (float)nwires+0.5, 0.0);

		hid = 20 + ilayer + 1 ;
		sprintf( htitle, "L%d Crosstalk hits in other layers", ilayer + 1);
		HBOOK1( hid, htitle, nwires, 0.5, (float)nwires+0.5, 0.0);
	}

	printf("end of test12 init\n");

	return 0;
}

/*******************************************************************************
   Do analysis and fill histograms for FAST site test 12
*******************************************************************************/

int test_12_event(int pass) 
{
  //printf("test12event reached, pass %d\n",pass);
	int               hid, layer, layer1, layer2, iwire, j;
	static int        pair_layer[6] = { 2, 1, 4, 3, 6, 5 };
	static logical    first = _TRUE;
	static int        event_number = 0;

	int strips = 6;
	int ev_per_strip = tcs.t12.events_per_strip; // from config file
	tot_evt =  ev_per_strip * strips;

	// simulate legacy alctcal module by assigning the params below
	upevt_.alctcal_scan_mode            = 4;
	// layer number, starting from 1
	upevt_.alctcal_current_value        = event_number/ev_per_strip + 1;
	upevt_.alctcal_current_threshold    = 0; // rest is unused in 12
	upevt_.alctcal_trigger_per_step     = 0;
	upevt_.alctcal_num_points           = 0;
	upevt_.alctcal_num_points_2nd_param = 0;

	if (first) 
	{
		printf("Test 12 first event\n");
		first = _FALSE;

		if (upevt_.alctcal_scan_mode != 4) printf("Wrong data file for test 12:"
												  " ALCTCAL scan mode should be 4, decoded: %d\n", 
												  upevt_.alctcal_scan_mode);
	}

/* Fill histograms for layer with expected hits */
	layer = upevt_.alctcal_current_value;
	hid = layer;
	for (iwire = 0; iwire < nwires; iwire++) {
	  if (upevt_.alct_dump[layer-1][iwire] > 0){ 
	    // printf (" wire %d, layer %d, dump %d,event%d\n",iwire,layer,upevt_.alct_dump[layer-1][iwire],event_number);

	    HFILL(hid, (float)(iwire+1), 0., 1.);
	  }
	}
	//printf("\n");
	nevents[layer-1]++;
/* Fill histograms for the layer paired with layer1 (which might have xtalk) */
	layer1 = pair_layer[layer-1];
	hid = 10 + layer1;
	for (iwire = 0; iwire < nwires; iwire++) {
	  if (upevt_.alct_dump[layer1-1][iwire] > 0) {
	    HFILL(hid, (float)(iwire+1), 0., 1.);
	    //printf (" wire %d, layer1 %d, layer %d ,dump %d ",iwire,layer1, layer, upevt_.alct_dump[layer-1][iwire]);
	  }
	}
	nevents1[layer1-1]++;
	//printf("\n");
/* Fill histograms for the layers with no hits expected */
	for (layer2 = 1; layer2 <= NLAYER; layer2++) {
		if (layer2 != layer && layer2 != layer1) {
			hid = 20 + layer2;
			for (iwire = 0; iwire < nwires; iwire++) {
			  if (upevt_.alct_dump[layer2-1][iwire] > 0){ 
			    HFILL(hid, (float)(iwire+1), 0., 1.);
			    //printf (" wire %d, layer %d, layer2 %d, dump %d ", iwire, layer, layer2, upevt_.alct_dump[layer-1][iwire]);
			  }
			}
			nevents2[layer2-1]++;
		}
		//printf("\n");
	}

	//  nevents1[layer1-1]++;
	event_number++;
	return 0;  
}


/*******************************************************************************
   End-of-run analysis for FAST site test 12
*******************************************************************************/

int test_12_end(int pass) {
  return 0;
}

/*******************************************************************************
   End-of-test analysis for FAST site test 12
*******************************************************************************/

// #define EFFICIENCY_LOWER_LIMIT        0.90
// #define EFFICIENCY_UPPER_LIMIT        1.10
// #define PAIR_XTALK_UPPER_LIMIT        0.05
// #define NON_PAIR_XTALK_UPPER_LIMIT    0.00

int test_12_finish(void) {
  int     bad_wires [NLAYER][nwires];
  int     ilayer, iwire, n_wr_ch;
  float   limits[7], eff[NLAYER][nwires], xtalk1[NLAYER][nwires], xtalk2[NLAYER][nwires];
  time_t  t;
  char    timestring[40];
  char title[80];
  char *file_his_name;
 
/* Initialize */
  n_wr_ch = 0;
  printf(" Test 12 finish\n");
/* Measure efficiency */


  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    HUNPAK(ilayer + 1, &eff[ilayer][0], "HIST", 1);
    for (iwire = 0; iwire < nwires; iwire++) {

	    
      eff[ilayer][iwire] /= nevents[ilayer];
    }
  }

  fprintf(fp1, "Result of efficiency for connectivity test 12\n");
  fprintf(fp1, "wiregroup    Layer 1  Layer 2  Layer 3  "
                            "Layer 4  Layer 5  Layer 6  \n");
  for (iwire = 0; iwire < nwires; iwire++) {
      fprintf(fp1, "   %2d     %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f _\n", iwire + 1, 
	    eff[0][iwire],    eff[1][iwire],    eff[2][iwire],
	    eff[3][iwire],    eff[4][iwire],    eff[5][iwire]);
  }


/* Check that crosstalk is within limits */
  fprintf(fp2, "Pair crosstalk between layers sharing the same chips\n");
  fprintf(fp2, "ie, between layers 1 and 2, 3 and 4, or 5 and 6.\n\n");

  fprintf(fp3, "Pair crosstalk between layers not sharing the same chips\n\n");

  fprintf(fp_bad, "List of wires not connectd to CSC or  with crosstalk above limits:\n");
  fprintf(fp_bad, " Layer, Wire, Effic. Pair Xtalk, NonPair Xtalk\n");

  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    HUNPAK(10 + (ilayer + 1),&xtalk1[ilayer][0], "HIST", 1);
    HUNPAK(20 + (ilayer + 1),&xtalk2[ilayer][0], "HIST", 1);
    for (iwire = 0; iwire < nwires; iwire++) {
      xtalk1[ilayer][iwire] /= nevents1[ilayer];
      xtalk2[ilayer][iwire] /= nevents2[ilayer];
    }
  }


  fprintf(fp2, "wiregroup    Layer 1  Layer 2  Layer 3  "
                            "Layer 4  Layer 5  Layer 6  \n");
  fprintf(fp3, "wiregroup    Layer 1  Layer 2  Layer 3  "
                            "Layer 4  Layer 5  Layer 6  \n");
  for (iwire = 0; iwire < nwires; iwire++) {
      fprintf(fp2, "   %2d     %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f _\n", iwire + 1, 
	    xtalk1[0][iwire],    xtalk1[1][iwire],    xtalk1[2][iwire],
	    xtalk1[3][iwire],    xtalk1[4][iwire],    xtalk1[5][iwire]);
      fprintf(fp3, "   %2d     %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f _\n", iwire + 1, 
	    xtalk2[0][iwire],    xtalk2[1][iwire],    xtalk2[2][iwire],
	    xtalk2[3][iwire],    xtalk2[4][iwire],    xtalk2[5][iwire]);
  }

/* List bad channels */

  n_wr_ch = 0;

  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    for (iwire = 0; iwire < nwires; iwire++) {
      bad_wires[ilayer][iwire] = 0;
    }
  }
  
  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    for (iwire = 0; iwire < nwires; iwire++) {

      if (eff[ilayer][iwire] < NON_EFF_LOWER_LIMIT ){
	fprintf(fp_bad, "  L %d W %3d %8.2f   %8.3f   %8.3f    Not connected \n", 
          ilayer+1, iwire+1, eff[ilayer][iwire], xtalk1[ilayer][iwire], xtalk2[ilayer][iwire]);
	bad_wires[ilayer][iwire] += 1;
      }
      else  if (eff[ilayer][iwire] <  EFF_LOWER_LIMIT ){
	fprintf(fp_bad, "  L %d W %3d %8.2f   %8.3f   %8.3f    Low efficiency \n", 
          ilayer+1, iwire+1, eff[ilayer][iwire], xtalk1[ilayer][iwire], xtalk2[ilayer][iwire]);
	bad_wires[ilayer][iwire] += 2;
      }
      if (xtalk1[ilayer][iwire] > PAIR_XTALK_UPPER_LIMIT ){
	fprintf(fp_bad, "  L %d W %3d %8.2f   %8.3f   %8.3f    High pair-plane cross-talk \n", 
          ilayer+1, iwire+1, eff[ilayer][iwire], xtalk1[ilayer][iwire], xtalk2[ilayer][iwire]);
	bad_wires[ilayer][iwire] += 4;
      }
      if (xtalk2[ilayer][iwire] > NON_PAIR_XTALK_UPPER_LIMIT ){
	fprintf(fp_bad, "  L %d W %3d %8.2f   %8.3f   %8.3f    High non-pair-plane cross-talk \n", 
          ilayer+1, iwire+1, eff[ilayer][iwire], xtalk1[ilayer][iwire], xtalk2[ilayer][iwire]);
	bad_wires[ilayer][iwire] += 8;
      }
      if (bad_wires[ilayer][iwire] != 0) n_wr_ch++;
    }
  }
  fprintf(fp_bad, "Total number of bad channels for this CSC = %3d \n\n", n_wr_ch);

  limits[0] = HIST_LAYER_LOW_LIMIT0 ;        /* layer histo lower limit */
  limits[1] = HIST_LAYER_HIGH_LIMIT0;        /* layer histo upper limit */
  limits[2] = HIST_SUMMARY_LOW_LIMIT0;       /* summary histo lower limit */
  limits[3] = HIST_SUMMARY_HIGH_LIMIT0;      /* summary histo upper limit */
  limits[4] = EFF_LOWER_LIMIT;               /* lower limit line */
  limits[5] = EFF_UPPER_LIMIT;               /* upper limit line */
  limits[6] = nwires;

  sprintf( title, "Efficiency            %s", number_of_run);
  one_page_plot_v3(12, 1, WIRE_PLOT, (float *)eff, (float *)eff, limits, 
   csc_name, title, "Efficiency", 0);

  limits[0] = HIST_LAYER_LOW_LIMIT1;     /* layer histo lower limit */
  limits[1] = HIST_LAYER_HIGH_LIMIT1;    /* layer histo upper limit */
  limits[2] = HIST_SUMMARY_LOW_LIMIT1;   /* summary histo lower limit */
  limits[3] = HIST_SUMMARY_HIGH_LIMIT1;  /* summary histo upper limit */
  limits[4] = PAIR_XTALK_LOWER_LIMIT;    /* lower limit line */
  limits[5] = PAIR_XTALK_UPPER_LIMIT;    /* upper limit line */
  limits[6] = nwires;

  sprintf( title, "Pair-Plane Crosstalk  %s", number_of_run);
  one_page_plot_v3(12, 2, WIRE_PLOT, (float *)xtalk1, (float *)xtalk1, limits, 
   csc_name, title, "Probability", 0);

  limits[0] = HIST_LAYER_LOW_LIMIT2;         /* layer histo lower limit */
  limits[1] = HIST_LAYER_HIGH_LIMIT2;        /* layer histo upper limit */
  limits[2] = HIST_SUMMARY_LOW_LIMIT2;       /* summary histo lower limit */
  limits[3] = HIST_SUMMARY_HIGH_LIMIT2;      /* summary histo upper limit */
  limits[4] = NON_PAIR_XTALK_LOWER_LIMIT;    /* lower limit line */
  limits[5] = NON_PAIR_XTALK_UPPER_LIMIT;    /* upper limit line */
  limits[6] = nwires;

  sprintf( title, "Non-Pair-Plane Crosstalk   %s", number_of_run);
  one_page_plot_v3(12, 3, WIRE_PLOT, (float *)xtalk2, (float *)xtalk2, limits, 
   csc_name, title, "Probability", 0);

/* Save histograms */
  file_his_name = malloc(strlen(datadir_translation) + 
			 strlen(file_his_name_el) + 5);
  if (file_his_name == NULL)
    {
    printf("malloc() failed for file_his_name.  Could not save histograms.\n");
    }
  else
    {
    sprintf(file_his_name,"%s%s",
	    datadir_translation,file_his_name_el);
    printf("Saving histograms\n");
    HRPUT(0, file_his_name, "N");
    free(file_his_name);
    }

/* Print mandatory summary statement */
  if (n_wr_ch > 0){
    fprintf(fp1, "Test result = Failed\n\n");
    fprintf(fp2, "Test result = Failed\n\n");
    fprintf(fp3, "Test result = Failed\n\n");
    fprintf(fp_bad, "Test result = Failed\n\n");
  }
  else{
    fprintf(fp1, "Test result = OK\n\n");
    fprintf(fp2, "Test result = OK\n\n");
    fprintf(fp3, "Test result = OK\n\n");
    fprintf(fp_bad, "Test result = OK\n\n");
  }

/* Add the date and time */
  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp1, "%s\n\n", timestring);
  fprintf(fp2, "%s\n\n", timestring);
  fprintf(fp3, "%s\n\n", timestring);
  fprintf(fp_bad, "%s\n\n", timestring);

  fclose(fp1);
  fclose(fp2);
  fclose(fp3);
  fclose(fp_bad);


  HDELET(0);

  return 0;
}
