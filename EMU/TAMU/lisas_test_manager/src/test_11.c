/*
 * test_11.c (in V1.2 test_06.c)
 * AFEB Noise Test
 *
 * Author : Valentin Sulimov      
 * Date   : 18 Sept. 2000
 * Last Change: Oct.19 2001
 * To add 2 cases HV=3600 and HV=3800 S.D.
 *
 * Last Change: June 6 2002
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
#include "csc_event.h"       /* upevt_                 */
#include "csc_geom.h"        /* stripwidth, etc        */
#include "daq_histo.h"       /* PED_ID                 */
#include "daq_module.h"      /* map_.AFEB              */
#include "one_page_plot.h"   /* STRIP_PLOT,...         */
#include "test_utils.h"      /* chamber labels         */
#include "test_params.h"     /* test config parameters */
#include "test_11_ini.h"     /* limits for test 11 */ 
#include "test_config_struct.h"

//#include "emu/daq/reader/RawDataFile.h"

extern test_config_struct tcs;
extern int tot_evt;


/* Local function declarations */
int test_11_begin(int pass);
int test_11_event(int pass);
int test_11_end(int pass);
int test_11_finish(void);

//extern emu::daq::reader::RawDataFile *rawFile;

/* Global variables , have to be in some *.h file  SD*/

extern char  number_of_run[80];

/* Global variables local to this test */

static int             hv_status;          // hv for this run, status=0 HV=3600, status=1 hv=3800
static char * hv_val[2] ={ "3600 V","3800 V"};
//why should it be INT and not floats?
static int           first_time,  del_time,  all_time;
static long            del_scaler;
static unsigned long   first_scaler,all_scaler;
static FILE            *fp1;
static FILE            *fp2;
static FILE            *fp_bad;

static char * fp1_name_el[2]      = {"/test_results/temp/test_11_01.results",
                                  "/test_results/temp/test_11_03.results"};
static char * fp2_name_el[2]      = {"/test_results/temp/test_11_02.results",
                                  "/test_results/temp/test_11_04.results"};
static char * fp_bad_name_el[2]   = {"/test_results/temp/test_11_01.bad",
                                  "/test_results/temp/test_11_02.bad"};
static char * file_his_name_el[2] = {"/test_results/temp/test_11_01.his",
                                  "/test_results/temp/test_11_02.his"};

  int n_hit=0;
  int n_hit_i=0;
  int n_hit_im=0;

static int nwires;
/*******************************************************************************
   Define functions and parameters for FAST site test 11
*******************************************************************************/

int test_11_init(void) 
  {
  test_begin  = test_11_begin;
  test_event  = test_11_event;
  test_end    = test_11_end;
  test_finish = test_11_finish;

  num_passes = 1;
  num_events[0] = 100000;
  switch (csc_type)
  {
  case 0: nwires = 48; break;
  case 1: nwires = 48; break;
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
   Book histograms for FAST site test 11                                      
*******************************************************************************/

int test_11_begin(int pass) 
  {
  int j;
  int adb, hid, ilayer, ifeb, iwire;
  float his_min = 0.5, his_max = nwires +0.5; // they depend on CSC type
  char htitle[80];

  char *fp1_name, *fp2_name, *fp_bad_name;

  printf("****** Test 11 Start ******\n");

  printf("Was the high voltage 3600 or 3800 for this run (0-3600/1-3800) ? \n");
  getint(&j);
  hv_status=j; 
  if (hv_status <0) hv_status = 0;
  if (hv_status >1) hv_status = 1;
  printf("you entered hv_status %d for HV=%s\n", hv_status, hv_val[hv_status]);

  /* create the names */
  fp1_name = malloc(strlen(datadir_translation) + 
		    strlen(fp1_name_el[hv_status]) + 5);
  if (fp1_name == NULL)
    {
    printf("malloc() failed for fp1_name.\n");
    return -1;
    }
  sprintf(fp1_name,"%s%s",datadir_translation,fp1_name_el[hv_status]);
  fp2_name = malloc(strlen(datadir_translation) + 
		    strlen(fp2_name_el[hv_status]) + 5);
  if (fp2_name == NULL)
    {
    free(fp1_name);
    printf("malloc() failed for fp2_name.\n");
    return -1;
    }
  sprintf(fp2_name,"%s%s",datadir_translation,fp2_name_el[hv_status]);
  fp_bad_name = malloc(strlen(datadir_translation) + 
		       strlen(fp_bad_name_el[hv_status]) + 5);
  if (fp_bad_name == NULL)
    {
    free(fp1_name);
    free(fp2_name);
    printf("malloc() failed for fp_bad_name.\n");
    }
  sprintf(fp_bad_name,"%s%s",datadir_translation,fp_bad_name_el[hv_status]);

/* Open result file in the temporary directory */
  fp1 = fopen(fp1_name, "w");
  if (fp1 == 0)
    {
    printf("File %s could not be opened.\n",fp1_name);
    free(fp1_name);
    free(fp2_name);
    free(fp_bad_name);
    return -1;
    }
  free(fp1_name);
  fprintf(fp1, "-----------------------------------------------------\n");
  fprintf(fp1, "Results of test 11 (AFEB Noise Test HV=%s) for %s\n", hv_val[hv_status], csc_name);
  fprintf(fp1, "-----------------------------------------------------\n");

/* Open result file in the temporary directory */
  fp2 = fopen(fp2_name, "w");
  if (fp1 == 0)
    {
    printf("File %s could not be opened.\n",fp2_name);
    fclose(fp1);
    free(fp2_name);
    free(fp_bad_name);
    return -1;
    }
  free(fp2_name);
  fprintf(fp2, "-----------------------------------------------------\n");
  fprintf(fp2, "Results of test 11 (AFEB Noise Test HV=%s) for %s\n", hv_val[hv_status], csc_name);
  fprintf(fp2, "-----------------------------------------------------\n");

/* Open result file in the temporary directory */
  fp_bad = fopen(fp_bad_name, "w");
  if (fp1 == 0)
    {
    printf("File %s could not be opened.\n",fp_bad_name);
    fclose(fp1);
    fclose(fp2);
    free(fp_bad_name);
    return -1;
    }
  free(fp_bad_name);
  fprintf(fp_bad, "-----------------------------------------------------\n");
  fprintf(fp_bad, "Results of test 11 (AFEB Noise Test HV=%s) for %s\n", hv_val[hv_status], csc_name);
  fprintf(fp_bad, "-----------------------------------------------------\n");

/* Book histograms for CSC wires */

  for (ilayer = 0; ilayer < NLAYER; ilayer++) 
    {
    for (iwire = 0; iwire < nwires; iwire++) 
      {
      hid = WIRE_HIST_OFFSET + 1000+ 200*(ilayer+1) + iwire+1;
      sprintf( htitle, "ALCT Wire times, L%d Wire %d HV=%s", ilayer+1, iwire+1, hv_val[hv_status]);
      HBOOK1( hid, htitle, 32, 0.5, 32.5, 0.0);
      }
    }
  
  HBOOK1(WIRE_HIST_OFFSET, "Number of layers with hit wires", 
   7, -0.5, 6.5, 0.0);

  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    hid = WIRE_HIST_OFFSET + ilayer + 1;
    sprintf( htitle, "ALCT Wire occupancy -- sep. tbins, Layer %d HV=%s", ilayer+1, hv_val[hv_status]);
    HBOOK1( hid, htitle, nwires, his_min, his_max, 0.0);

    hid = WIRE_HIST_OFFSET + 10 + ilayer + 1 ;
    sprintf( htitle, "Isolated hit occupancy, Layer %d HV=%s", ilayer+1, hv_val[hv_status]);
    HBOOK1( hid, htitle, nwires, his_min, his_max, 0.0);

    hid = WIRE_HIST_OFFSET + 20 + ilayer + 1 ;
    sprintf( htitle, "second.tbins for separ.tbins, Layer %d HV=%s", ilayer+1, hv_val[hv_status]);
    HBOOK1( hid, htitle, nwires, his_min, his_max, 0.0);

    hid = WIRE_HIST_OFFSET + 30 + ilayer + 1 ;
    sprintf( htitle, "ALCT Wire occupancy -- first tbin, Layer %d HV=%s", ilayer+1, hv_val[hv_status]);
    HBOOK1( hid, htitle, nwires, his_min, his_max, 0.0);

    hid = WIRE_HIST_OFFSET + 40 + ilayer + 1 ;
    sprintf( htitle, "second. tbins for first tbin, Layer %d HV=%s", ilayer+1, hv_val[hv_status]);
    HBOOK1( hid, htitle, nwires, his_min, his_max, 0.0);

  }


  hid = WIRE_HIST_OFFSET + 3000;
  sprintf( htitle, "Time for event");
  HBOOK1( hid, htitle, 10000, 0.5, 10000.5, 0.0);
  sprintf( htitle, "Delta Time for event");
  HBOOK1( hid+1, htitle, 10000, 0.5, 10000.5, 0.0);
  sprintf( htitle, "Scaler for event");
  HBOOK1( hid+2, htitle, 10000, 0.5, 10000.5, 0.0);
  sprintf( htitle, "Del. Scaler for event");
  HBOOK1( hid+3, htitle, 10000, 0.5, 10000.5, 0.0);


  /* Book anode front-end board (AFEB) histograms */

//  for (ifeb = 1; ifeb <= NAFEB; ifeb++) 
  for (ifeb = 1; ifeb <= (nwires*3/8); ifeb++) 
  {
//    for (ichip = 1; ichip <= NAFEB_CHIP; ichip++) 
    {
//      adb = 6 * ifeb + ichip -6;   /* Front end board 2000 numbering */
	adb = ifeb;	// ALCT 2001 numbering
	hid = WIRE_HIST_OFFSET + 100 + adb;
	sprintf( htitle, "ALCT Wire times, ADB %d HV=%s", adb, hv_val[hv_status]);
	HBOOK1( hid, htitle, 32, 0.5, 32.5, 0.0); 
    }
  }

  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
//    hid = WIRE_HIST_OFFSET + 3000+100*(ilayer+1);
    hid = WIRE_HIST_OFFSET + 3000+120*(ilayer+1);
    sprintf( htitle, "MEAN hit times, Layer %d HV=%s", ilayer+1, hv_val[hv_status]);
    HBOOK1( hid, htitle, nwires, his_min, his_max, 0.0);
//    hid = WIRE_HIST_OFFSET + 3001+100*(ilayer+1);
    hid = WIRE_HIST_OFFSET + 3001+120*(ilayer+1);
    sprintf( htitle, "RMS of hit times, Layer %d HV=%s ", ilayer+1, hv_val[hv_status]);
    HBOOK1( hid, htitle, nwires, his_min, his_max, 0.0);
//    hid = WIRE_HIST_OFFSET + 3002+100*(ilayer+1);
    hid = WIRE_HIST_OFFSET + 3002+120*(ilayer+1);
    sprintf( htitle, "NOISE, Layer %d HV=%s", ilayer+1, hv_val[hv_status]);
    HBOOK1( hid, htitle, nwires, his_min, his_max, 0.0);
  }
  return 0;
}

/*******************************************************************************
   Do analysis and fill histograms for FAST site test 11
*******************************************************************************/

int test_11_event(int pass) 
{
	short int     ifeb, ichip, ichan, afeb, tmp;
	int           hid, ilayer, iwire;
	int           hit_layers, first_layer, first_wire;
	int           num_wires_hit[NLAYER];
	int           last_wire[NLAYER];
	int           j, disc_on, disc_on_2;

	int wire_array[NLAYER][nwires];

	static int    first = 1, tbit[32];

	hid = WIRE_HIST_OFFSET +3000;

	//printf ("event %i, time musec int %d, time musec float %d\n", upevt_.event_number, upevt_.time_musec, upevt_.time_musec);

	/* GET TIME and SCALER */
	if(upevt_.event_number == 1)
	{
		// all events are now ungated
		upevt_.num_ungated_triggers = tcs.t11.events_total;

		all_time=0;
		first_time=upevt_.time_musec;
		del_time= first_time;

		all_scaler=0;
		first_scaler=upevt_.num_ungated_triggers;
		del_scaler= first_scaler;
	}
	else {
		del_time   = upevt_.time_musec - first_time;
		del_scaler = upevt_.num_ungated_triggers - first_scaler;
		if( del_time > 0) all_time+=0.001*del_time; // in milliseconds
		else {
			del_time= del_time+1000000;
			all_time+=0.001*del_time; // in milliseconds
		}
		if( del_scaler >= 0) all_scaler+=del_scaler;
		else {
			del_scaler= del_scaler+0xffffff;
			all_scaler+= del_scaler;
		}
	}

//  printf("%ld %ld %ld %ld\n", first_time, upevt_.time_musec, del_time, all_time); 
// scaler is 32 bits in CCB, so we can afford not to calculate wrapping anymore
	all_scaler = upevt_.num_ungated_triggers;

	first_time   =upevt_.time_musec;
	first_scaler =upevt_.num_ungated_triggers;

	HFILL(hid,   (float )(upevt_.event_number), 0.0, upevt_.time_musec);          
	HFILL(hid+1, (float )(upevt_.event_number), 0.0, del_time);
	HFILL(hid+2, (float )(upevt_.event_number), 0.0, upevt_.num_ungated_triggers);
	HFILL(hid+3, (float )(upevt_.event_number), 0.0, del_scaler);

	for (ilayer = 0; ilayer < NLAYER; ilayer++) {
		for (iwire = 0; iwire < nwires; iwire++) {
			wire_array[ilayer][iwire]=-10;
		}
	}

/* Make an array for later use */
	if (first) {
		for (j = 0; j < 32; j++) tbit[j] = 1 << j;
		first = 0;
	}

/* Loop over the layers and wires of the alct_dump array */
	first_layer = -1;
	for (ilayer = 0; ilayer < NLAYER; ilayer++) {
		num_wires_hit[ilayer] = 0;

		for (iwire = 0; iwire < nwires; iwire++) {
			if (upevt_.alct_dump[ilayer][iwire]) {

				tmp   = map_.AFEB_CHIP[ilayer][iwire];
				ifeb  = (tmp & 0xf000) >> 12;
				ichip = (tmp & 0x0f00) >> 8;
				ichan = (tmp & 0x00ff) ;
//        afeb  = 6 * ifeb + ichip - 6;      /* Front end board 2000 numbering */
				afeb = (iwire/8)*3 + 1 + ilayer/2; // ALCT2001 numbering

				/* Fill histograms for each hit for this wire (t is in 25 ns units) */
				disc_on = 0;
				disc_on_2=0;
				for (j = 0; j < 32; j++) {
					if (upevt_.alct_dump[ilayer][iwire] & tbit[j]) {
 
						if(!disc_on_2){
							disc_on_2++;
	   
							hid = WIRE_HIST_OFFSET + 30 + ilayer + 1;
							HFILL(hid, (float )(iwire+1), 0.0, 1.0);
	    
						}
						else{
							if(!disc_on){
								printf("event=%ld: afterpulses ===ilayer=%d iwire=%d tbin=%d \n",upevt_.event_number,ilayer,iwire,j);
								hid = WIRE_HIST_OFFSET + 40 + ilayer + 1;
								HFILL(hid, (float )(iwire+1), 0.0, 1.0);  
							}
						}

						if (!disc_on) {
							disc_on = 1;

							hid = WIRE_HIST_OFFSET + 1000 + 200*(ilayer+1) + iwire+1;
							HFILL(hid, (float )(j+1), 0.0, 1.0);

							/* Fill Anode front-end board histograms */
							hid = WIRE_HIST_OFFSET + 100 + afeb;
							HFILL(hid, (float )(j+1), 0.0, 1.0);

							/* Fill wire occupancy in a layer */
							hid = WIRE_HIST_OFFSET + ilayer + 1;
							HFILL(hid, (float )(iwire+1), 0.0, 1.0);

							n_hit++;
						} 
						else  {

							hid = WIRE_HIST_OFFSET + 20 + ilayer + 1;
							HFILL(hid, (float )(iwire+1), 0.0, 1.0);
						}
					}
 
					else disc_on = 0;
	  
				}

				/* How many layers have hits?  */
 
				wire_array[ilayer][num_wires_hit[ilayer]]=iwire;
				num_wires_hit[ilayer]++; 
				last_wire[ilayer]=iwire;

				/* Keep track of first hit found (to use later in cross-talk check) */
				if (first_layer < 0) {
					first_layer = ilayer;
					first_wire = iwire;
				}
			}
		}
	}

/* Fill layers hit */

	hit_layers = 0;
	for (ilayer = 0; ilayer < NLAYER ; ilayer++) {
		if (num_wires_hit[ilayer] > 0) hit_layers++;
	}
	hid = WIRE_HIST_OFFSET;
	HFILL(hid, (float)hit_layers, 0., 1.);

/* Count isolated hits to check for short-circuits between wire groups */

	for (ilayer = 0; ilayer < NLAYER ; ilayer++) {

		if (num_wires_hit[ilayer] == 1) {
			hid = WIRE_HIST_OFFSET + 10 + ilayer + 1;
			HFILL(hid, (float )(last_wire[ilayer]+1), 0.0, 1.0);
			n_hit_i++;
		}
		else if(num_wires_hit[ilayer] > 1) { 

			for(iwire=0; iwire<num_wires_hit[ilayer]; iwire++)
			{          
				if(iwire==0) {if(wire_array[ilayer][iwire]!=(wire_array[ilayer][iwire+1]-1))
				{
					HFILL(hid, (float )(wire_array[ilayer][iwire]+1), 0.0, 1.0);n_hit_im++;
				}
				}
				else if(iwire==(num_wires_hit[ilayer]-1)) 
				{
					if(wire_array[ilayer][iwire]!=(wire_array[ilayer][iwire-1]+1))  
					{
						HFILL(hid, (float )(wire_array[ilayer][iwire]+1), 0.0, 1.0);n_hit_im++;
					}
				}
				else {
					if(  wire_array[ilayer][iwire]!=(wire_array[ilayer][iwire+1]-1) &&
						 wire_array[ilayer][iwire]!=(wire_array[ilayer][iwire-1]+1)    ) 
					{
						HFILL(hid, (float )(wire_array[ilayer][iwire]+1), 0.0, 1.0); n_hit_im++;
					} 
				}
			}
		}
	} 
	return 0;
}


/*******************************************************************************
   End-of-run analysis for FAST site test 11
*******************************************************************************/

int test_11_end(int pass) {
  return 0;
}

/*******************************************************************************
   End-of-test analysis for FAST site test 11
*******************************************************************************/

int test_11_finish(void) {
  int     hid, hidd, ilayer, iwire, num=1, n_wr_ch;
  int     num_page;
  int     i;
  float   events, mean, rms, theta, wire_length;
  float   nhits[nwires], limits[7];
  float   lim[2][12] = { 
    { HIST_LAYER_LOW_LIMIT0,      HIST_LAYER_HIGH_LIMIT0,
      HIST_SUMMARY_LOW_LIMIT0,    HIST_SUMMARY_HIGH_LIMIT0,   
      SINGLES_RATE_LOWER_LIMIT0,  SINGLES_RATE_UPPER_LIMIT0,
      HIST_PROB_LAYER_LOW_LIMIT0,      HIST_PROB_LAYER_HIGH_LIMIT0,
      HIST_PROB_SUMMARY_LOW_LIMIT0,    HIST_PROB_SUMMARY_HIGH_LIMIT0,  
      ISOLATED_PROB_LOWER_LIMIT0, ISOLATED_PROB_UPPER_LIMIT0}, 
    { HIST_LAYER_LOW_LIMIT1,      HIST_LAYER_HIGH_LIMIT1,
      HIST_SUMMARY_LOW_LIMIT1,    HIST_SUMMARY_HIGH_LIMIT1,
      SINGLES_RATE_LOWER_LIMIT1,  SINGLES_RATE_UPPER_LIMIT1,
      HIST_PROB_LAYER_LOW_LIMIT1,      HIST_PROB_LAYER_HIGH_LIMIT1,
      HIST_PROB_SUMMARY_LOW_LIMIT1,    HIST_PROB_SUMMARY_HIGH_LIMIT1,  
      ISOLATED_PROB_LOWER_LIMIT1, ISOLATED_PROB_UPPER_LIMIT1} };

  float   rate[NLAYER][nwires], rate1[NLAYER][nwires], rate2[NLAYER][nwires]; 
  float   rate3[NLAYER][nwires]; 
  float   enters[NLAYER][nwires] ;
  int     bad_wires [NLAYER][nwires];

  float   rate4[NLAYER][nwires]; 
  float   ratio_tbins[NLAYER][nwires]; 

  float   rate_1[NLAYER][nwires], rate_4[NLAYER][nwires]; 
  float   ratio_tbins_a[NLAYER][nwires];

  time_t  t;
  char    choice[4];

  char    timestring[40];
  char    title[80];

  char  *file_his_name;
  int j;
  float f;

  //assign legacy values for rate analysis
  all_scaler = upevt_.event_number;
  printf("Please input number of milliseconds it took to collect events: \n");
  getint(&j);
  //in earlier time here a factor of 1000 - take it out since
  //we report NOW ms directly
  all_time = (float)j; // in ms

  printf("all_scaler= %ld all_time= %f sec,  event_number= %ld \n", all_scaler,
	 (float)(0.001*all_time), upevt_.event_number);

  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    for (iwire = 0; iwire < nwires; iwire++) {
      hid = WIRE_HIST_OFFSET + 1000 + 200*(ilayer+1) + iwire+1;
      mean   = HSTATI(hid,1,choice,num);
      rms    = HSTATI(hid,2,choice,num);
      events = HSTATI(hid,3,choice,num);

      events = (float)(all_scaler)*1000.*(float)(HSTATI(hid,3,choice,num));
      events = events/(float)(upevt_.event_number);
      //all time in old codes in seconds
      events = events/(float)(all_time);

      enters[ilayer][iwire] = events;

//      hidd = WIRE_HIST_OFFSET+3000+100*(ilayer+1);
      hidd = WIRE_HIST_OFFSET+3000+120*(ilayer+1);
      HFILL(hidd, (float )(iwire+1), 0.0, mean);

//      hidd = WIRE_HIST_OFFSET+3001+100*(ilayer+1);
      hidd = WIRE_HIST_OFFSET+3001+120*(ilayer+1);
      HFILL(hidd, (float )(iwire+1), 0.0, rms);

//      hidd = WIRE_HIST_OFFSET+3002+100*(ilayer+1);
      hidd = WIRE_HIST_OFFSET+3002+120*(ilayer+1);
      HFILL(hidd, (float )(iwire+1), 0.0, events);
    }
  }

/* Save histograms */
  file_his_name = malloc(strlen(datadir_translation) + 
			 strlen(file_his_name_el[hv_status]) + 5);
  if (file_his_name == NULL)
    {
    printf("malloc() failed for file_his_name.  Could not save histograms.\n");
    }
  else
    {
    sprintf(file_his_name,"%s%s",
	    datadir_translation,file_his_name_el[hv_status]);
    printf("Saving histograms\n");
    HRPUT(0, file_his_name, "N");
    free(file_his_name);
    }
  printf("results will be plotted");
/* Plot results */

  theta = (NSTRIP / 2.) * STRIP_ANGLE;
 
  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    hid = WIRE_HIST_OFFSET + (ilayer+1);
    HUNPAK(hid, nhits, "HIST", 1);
    for (iwire = 0; iwire < nwires; iwire++) {
      wire_length = 2 * tan(theta) * (R_WG_0 + WG_WIDTH * iwire) * 1E-3; /* m */
      rate[ilayer][iwire] = (nhits[iwire] / (all_time * 1E-3)) * 
       (all_scaler / upevt_.event_number);
      // rate1[ilayer][iwire] = rate[ilayer][iwire] / wire_length;
      rate1[ilayer][iwire] = rate[ilayer][iwire];
    }
  }

  sprintf( title, "singles rate, %s,           ", 
	   hv_val[hv_status]/*, number_of_run*/);
  num_page = 1 + 3*hv_status;
 
  for (i = 0; i<6; i++){
    limits[i] = lim[hv_status][i];
  }

  limits[6] = nwires;

//  one_page_plot_v3(11, num_page, WIRE_PLOT, (float *)rate1, (float *)rate1, limits, 
//   csc_name, title, "Hz/m", 0);
  one_page_plot_v3(11, num_page, WIRE_PLOT, (float *)rate1, (float *)rate1, limits,
		     csc_name, title, "Hz", 0);
  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    hid = WIRE_HIST_OFFSET + 10 + (ilayer+1);
    HUNPAK(hid, nhits, "HIST", 1);

    for (iwire = 0; iwire < nwires; iwire++) {
      wire_length = 2 * tan(theta) * (R_WG_0 + WG_WIDTH * iwire) * 1E-3; /* m */
      // rate2[ilayer][iwire] = (nhits[iwire] / (all_time * 1E-3)) * 
      // (all_scaler / upevt_.event_number) / wire_length;

      rate2[ilayer][iwire] = (nhits[iwire] / (all_time * 1E-3)) *
	      (all_scaler / upevt_.event_number);
      if (rate1[ilayer][iwire] > 0) {
	rate3[ilayer][iwire] = rate2[ilayer][iwire] / rate1[ilayer][iwire];
      }
      else{
	rate3[ilayer][iwire] = -99.; /* notdefined */
      }

    }
  }

  for (i = 0; i<6; i++){
    limits[i] = lim[hv_status][i+6];
  }
 
  sprintf( title, "isolated hit probability, %s  ", 
	   hv_val[hv_status]/*, number_of_run*/);
  num_page = 2 + 3*hv_status;

  limits[6] = nwires;
  one_page_plot_v3(11, num_page, WIRE_PLOT, (float *)rate3, (float *)rate3, limits, 
   csc_name, title, "Probability", 0);
//=================================================================================


  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    hid = WIRE_HIST_OFFSET + 20 + (ilayer+1);
    HUNPAK(hid, nhits, "HIST", 1);

    for (iwire = 0; iwire < nwires; iwire++) {
      wire_length = 2 * tan(theta) * (R_WG_0 + WG_WIDTH * iwire) * 1E-3; /* m */
      //rate4[ilayer][iwire] = (nhits[iwire] / (all_time * 1E-3)) * 
      // (all_scaler / upevt_.event_number) / wire_length;
      rate4[ilayer][iwire] = (nhits[iwire] / (all_time * 1E-3)) *
	  (all_scaler / upevt_.event_number);
      
      if (rate1[ilayer][iwire] > 0) {
	ratio_tbins[ilayer][iwire] = (rate4[ilayer][iwire]) / rate1[ilayer][iwire];
      }
      else{
	ratio_tbins[ilayer][iwire] = -99.; /* notdefined */
      }

    }
  } // for

  for (i = 0; i<6; i++){
    limits[i] = lim[hv_status][i+6];
  }
 
  sprintf( title, "secondary tbins probability, %s  ", 
	   hv_val[hv_status]/*, number_of_run*/);

//=================================================================================

  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    hid = WIRE_HIST_OFFSET + 30 + (ilayer+1);
    HUNPAK(hid, nhits, "HIST", 1);

    for (iwire = 0; iwire < nwires; iwire++) {
      wire_length = 2 * tan(theta) * (R_WG_0 + WG_WIDTH * iwire) * 1E-3; /* m */

      //rate_1[ilayer][iwire] = (nhits[iwire] / (all_time * 1E-3)) * 
      // (all_scaler / upevt_.event_number) / wire_length;
      rate_1[ilayer][iwire] = (nhits[iwire] / (all_time * 1E-3)) *
	    (all_scaler / upevt_.event_number);
    } 

    hid = WIRE_HIST_OFFSET + 40 + (ilayer+1);
    HUNPAK(hid, nhits, "HIST", 1);

    for (iwire = 0; iwire < nwires; iwire++) {
      wire_length = 2 * tan(theta) * (R_WG_0 + WG_WIDTH * iwire) * 1E-3; /* m */

      // rate_4[ilayer][iwire] = (nhits[iwire] / (all_time * 1E-3)) * 
      //  (all_scaler / upevt_.event_number) / wire_length;
      
      rate_4[ilayer][iwire] = (nhits[iwire] / (all_time * 1E-3)) *
	       (all_scaler / upevt_.event_number);
      if (rate_1[ilayer][iwire] > 0) {
	ratio_tbins_a[ilayer][iwire] = (rate_4[ilayer][iwire]) / rate_1[ilayer][iwire];
      }
      else{
	ratio_tbins_a[ilayer][iwire] = -99.; /* notdefined */
      }
    } 
  }

  limits[0] =  0.;                /* layer histo lower limit */
  limits[1] =  0.1;                /* layer histo upper limit */
  limits[2] =  0.;                /* summary histo lower limit */
  limits[3] =  0.1;                /* summary histo upper limit */
  limits[4] =  0.;    /* lower limit line */
  limits[5] =  0.05;    /* upper limit line */
 
  sprintf( title, "afterpulsing probability, %s  ", 
	   hv_val[hv_status]/*, number_of_run*/);
  num_page = 3 + 3*hv_status;

  limits[6] = nwires;
  one_page_plot_v3(11, num_page, WIRE_PLOT, (float *)ratio_tbins_a, (float *)ratio_tbins_a, limits, 
   csc_name, title, "Probability", 0);

/* Table of rates */

//  fprintf(fp1, "\nMeasured rates for all channels, in Hz/m HV=%s:\n", hv_val[hv_status]);
   fprintf(fp1, "\nMeasured rates for all channels, in Hz HV=%s:\n", hv_val[hv_status]);
  fprintf(fp1, "wiregroup    Layer 1    Layer 2    Layer 3    "
                            "Layer 4    Layer 5    Layer 6  \n");
  fprintf(fp1, "--------------------------------------"
               "-------------------------------------\n");

  for (iwire = 0; iwire < nwires; iwire++) {
    fprintf(fp1, "   %2d     %10.2e %10.2e %10.2e %10.2e %10.2e %10.2e _\n", 
     iwire + 1, rate1[0][iwire], rate1[1][iwire], rate1[2][iwire], 
     rate1[3][iwire], rate1[4][iwire], rate1[5][iwire]);
  }

  fprintf(fp1, "--------------------------------------"
               "-------------------------------------\n");

  fprintf(fp2, "\nProbability of isolated hits for all channels, HV=%s:"
   "\n", hv_val[hv_status]);
  fprintf(fp2, "wiregroup    Layer 1  Layer 2  Layer 3  "
                            "Layer 4  Layer 5  Layer 6  \n");

  fprintf(fp2, "--------------------------------------"
               "-------------------------------------\n");

  for (iwire = 0; iwire < nwires; iwire++) {
    fprintf(fp2, "   %2d     %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f _\n", iwire + 1, 
	    rate3[0][iwire],	rate3[1][iwire],    rate3[2][iwire],
	    rate3[3][iwire],    rate3[4][iwire],    rate3[5][iwire]);
  }
  fprintf(fp2, "--------------------------------------"
               "-------------------------------------\n");


/* List bad channels */
  fprintf(fp_bad, "Rates of bad channels:\n");
  n_wr_ch = 0;

  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    for (iwire = 0; iwire < nwires; iwire++) {
      bad_wires[ilayer][iwire] = 0;
    }
  }
  
  for (ilayer = 0; ilayer < NLAYER; ilayer++) {
    for (iwire = 0; iwire < nwires; iwire++) {

      if (rate1[ilayer][iwire] < lim[hv_status][4]){
	fprintf(fp_bad, "  L %d W %3d %8.2f %8.3f Low Noise       \n", 
                          ilayer+1, iwire+1, rate1[ilayer][iwire], rate3[ilayer][iwire]);
	bad_wires[ilayer][iwire] += 1;
      }
      if (rate1[ilayer][iwire] > lim[hv_status][5]){
	fprintf(fp_bad, "  L %d W %3d %8.2f %8.3f High Noise      \n", 
                          ilayer+1, iwire+1, rate1[ilayer][iwire], rate3[ilayer][iwire]);
	bad_wires[ilayer][iwire] += 2;
      }
      if (rate3[ilayer][iwire] < lim[hv_status][10]){
	fprintf(fp_bad, "  L %d W %3d %8.2f %8.3f Lower Isolation \n", 
                          ilayer+1, iwire+1, rate1[ilayer][iwire], rate3[ilayer][iwire]);
	bad_wires[ilayer][iwire] += 4;
      }
      if (rate3[ilayer][iwire] >  lim[hv_status][11]){
	fprintf(fp_bad, "  L %d W %3d %8.2f %8.3f High Isolation  \n", 
                          ilayer+1, iwire+1, rate1[ilayer][iwire], rate3[ilayer][iwire]);
	bad_wires[ilayer][iwire] += 8;
      }
      if (bad_wires[ilayer][iwire] != 0) n_wr_ch++;
    }
  }
  fprintf(fp_bad, "Total number of bad channels for this CSC = %3d \n\n", n_wr_ch);

/* Print mandatory summary statement */
  if (n_wr_ch > 0) {
    fprintf(fp1, "Test result = Failed\n\n");
    fprintf(fp2, "Test result = Failed\n\n");
    fprintf(fp_bad, "Test result = Failed\n\n");

  }
  else {
    fprintf(fp1, "Test result = OK\n\n");
    fprintf(fp2, "Test result = OK\n\n");
    fprintf(fp_bad, "Test result = OK\n\n");
  }
  printf("Total number of ungated triggers = %ld  \n", all_scaler);
  printf("Time of run = %10.3e seconds \n", all_time * 1e-3);
  printf("Ungated trigger rate = %10.3e Hz\n", all_scaler / (all_time * 1e-3));
//  printf("Scaler = %ld Hz \n", upevt_.num_ungated_triggers);

/* Add the date and time */
  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp1, "%s\n\n", timestring);
  fprintf(fp2, "%s\n\n", timestring);
  fprintf(fp_bad, "%s\n\n", timestring);

  fclose(fp1);
  fclose(fp2);
  fclose(fp_bad);

  HDELET(0);

  return 0;
}

























