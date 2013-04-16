/*
 * test_13.c  (in V1.2 test_8.c)
 * AFEB Threshold and Analog Noise Test
 *
 * Author : Valentin Sulimov      
 * Date   : 06 Nov. 2000
 * 
 * Last change: 21 May 2002
 * SD, change err_fit on calc_thresh 
 */

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "cfortran.h"
#include "graflib.h" 
#include "packlib.h" 
#include "daq_conf.h"
#include "csc_event.h"       /* upevt_                    */
#include "csc_parameters.h"  /* NLAYER, NSTRIP, etc...    */
#include "daq_histo.h"       /* PED_ID                    */
#include "daq_module.h"      /* map_.AFEB                 */
#include "one_page_plot.h"   /* STRIP_PLOT,...            */
#include "test_utils.h"      /* chamber labels            */
#include "test_params.h"     /* test config parameters    */
#include "test_13_ini.h"     /* test 13 config parameters */
#include "test_config_struct.h"

//#include "csc_unpacker.h"
//#include "emu/daq/reader/RawDataFile.h"

//#define  PAWC_SIZE 50000

//#define  LEVEL_OCC     50.
     
/* Local function declarations */
int test_13_begin(int pass);
int test_13_event(int pass);
int test_13_end(int pass);
int test_13_finish(void);
int print_time_close(FILE *);
int print_scan_result(int );
int check_bad_channels(float** value , float , float , char* );
int get_afeb(int , int );
int get_next_scan(int );
int save_hist(int );
int fit_thresh(int );
int init_hist(int );
int print_20_result(void );
int calc_dac_20_fC(void );
int cal_cap_init(void );        // read file with calibration capcacitances

/* Global variables local to this test */
static FILE  *fp_bad;
static int    number_bad_channels = 0; // statistics of bad channels;
static int    npoints, nperpoint;
static int    nscan = 0;
static int    first_threshold_in_scan;
static int nwires;
static float  calibration_pulse[NMAXSCAN];

//static float  thr[NMAXSCAN][NLAYER][NWIRE], noise[NMAXSCAN][NLAYER][NWIRE];
//static int    idac_20[NLAYER*NWIRE/16];
//static float  thr_20[NLAYER][NWIRE];
// static float  cal_cap[3*NWIRE/8];      // array with calibration capcacitances
// static float  cal_gain[3*NWIRE/8];     // array with calibration gaines


static float  *thr[NMAXSCAN][NLAYER], *noise[NMAXSCAN][NLAYER];
static int    *idac_20;
static float  *thr_20[NLAYER];
static float  *cal_cap;      // array with calibration capcacitances
static float  *cal_gain;     // array with calibration gaines

static float  *pthr, *pnoise;
static float  *pthr_20;

extern int tot_evt;
extern test_config_struct tcs; // test configuration from test_config.xml
/******************************************************************************
   Define functions and parameters for FAST site test 13
******************************************************************************/

int test_13_init()
{
	int s,l;
	test_begin  = test_13_begin;
	test_event  = test_13_event;
	test_end    = test_13_end;
	test_finish = test_13_finish;

	num_passes = 1;
	num_events[0] = 100000;
	nscan = 0;
	number_bad_channels = 0;
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
  
	pthr     = malloc (sizeof(float) * nwires * NMAXSCAN * NLAYER);
	pnoise   = malloc (sizeof(float) * nwires * NMAXSCAN * NLAYER);
	pthr_20  = malloc (sizeof(float) * nwires * NLAYER);
	idac_20 = malloc (sizeof(int) * nwires * NLAYER/16);
	cal_cap = malloc (sizeof(float) * 3 * nwires/8);
	cal_gain = malloc (sizeof(float) * 3 * nwires/8);

	for (l = 0; l < NLAYER; l++)
	{
		for (s = 0; s < NMAXSCAN; s++)
		{
			thr  [s][l] = pthr   + (s * NLAYER + l) * nwires;
			noise[s][l] = pnoise + (s * NLAYER + l) * nwires;
		}
		thr_20[l] = pthr_20 + l * nwires;
	}


	return 0;
}

/******************************************************************************
   Book histograms for FAST site test 13                                       
******************************************************************************/

int test_13_begin(int pass) 
{
	char *file_name;
    
	if (cal_cap_init() !=0)
    {
		printf("Can't init capacitances\n");
		//    return -1;
    }

	/* compute the filename */
	file_name = malloc(strlen(datadir_translation) +
					   strlen(FILE_RESULTS) + 20);
	if (file_name == NULL)
    {
		printf("malloc() failed for file_name.\n");
		return -1;
    }
  
	/* Open result file in the temporary directory */ 
	sprintf(file_name,"%s%s%02d.bad", datadir_translation, FILE_RESULTS, 11); 
	if ( (fp_bad = fopen(file_name, "w")) == NULL)
    {
		printf (" Test_13 Print results: can not open file %s\n", file_name);
		free(file_name);
		return 1;
    }
	free(file_name);
	fprintf(fp_bad, "------------------------------------------"
			"------------------------------------------\n");
	fprintf(fp_bad, "Results of test 13 (AFEB Threshold"
			" and Analog Noise Test ) %s\n", csc_name);
	fprintf(fp_bad, "Bad channels(Layer, Wire and problem description:\n");
	fprintf(fp_bad, "------------------------------------------"
			"------------------------------------------\n");
  
	return 0;
}

/******************************************************************************
   Do analysis and fill histograms for FAST//int calc_peds(int mode);
 site test 13
******************************************************************************/

int test_13_event(int pass)
{
	short int     afeb;
	int           hid, ilayer, iwire;
	int           hit_layers, nhits;
	int           layers_hit[NLAYER];
	int           j, disc_on, itime[32];
	static int    first = 1, max_scan = 0, tbit[32];
	static int    old_current_value = -1 ;
	static int    old_current_threshold = -1;
	static int    event_number = 0;

	// these params should be taken from config file
	int num_tpamps     = tcs.t13.tpamps_per_run; // how many test pulse amplitides to run
	int num_thresh     = tcs.t13.thresholds_per_tpamp; // number of thresholds to scan with each test pulse amp
	int first_thresh   = tcs.t13.threshold_first; // first thresh
	int thresh_step    = tcs.t13.threshold_step; // threshold step
	int ev_per_thresh  = tcs.t13.events_per_threshold; // events per each threshold
	// total events
	tot_evt = tcs.t13.events_per_threshold * tcs.t13.thresholds_per_tpamp * tcs.t13.tpamps_per_run;

	// simulate legacy alctcal module by assigning the params below
	upevt_.alctcal_scan_mode            = 6;
	int i = event_number/(num_thresh * ev_per_thresh); // index of tpamp to use
	upevt_.alctcal_current_value        = tcs.t13.tpamp_first + i * tcs.t13.tpamp_step; // test pulse amplitude
	upevt_.alctcal_current_threshold    = ((event_number / ev_per_thresh) % num_thresh) * thresh_step + first_thresh;
	upevt_.alctcal_trigger_per_step     = ev_per_thresh; 
	upevt_.alctcal_num_points           = num_tpamps;
	upevt_.alctcal_num_points_2nd_param = num_thresh;

/* Make an array for later use */
	if (first)
    {
		for (j = 0; j < 32; j++) tbit[j] = 1 << j;
		first = 0;
		/* we need to know first threshold in scan, to simplify calculations */
		first_threshold_in_scan = upevt_.alctcal_current_threshold;


    }
/*
	if(event_number % 100 == 0)
		printf 
			(
				"ev: %05d "
				"current_tpamp : %02d  "
				"current_threshold : %02d\n",
				event_number,
				upevt_.alctcal_current_value,
				upevt_.alctcal_current_threshold
		   );
*/

	/* First check that amount of scans is less than NMAXSCAN, 
	   in other case skip event */
	if (nscan > NMAXSCAN)
    {
		if (max_scan == 0 )
		{
			printf ("*********** Test 13 Warning ***************\n");
			printf (" The scan number is more than NMAXSCAN=%d \n",NMAXSCAN);
			printf (" all events will be skipped \n");
			max_scan = 1;
		}
		return 0;
    }

	/* Next check, when we change amplitude (curent value), 
	   that means we begin new scan */
	if (upevt_.alctcal_current_value != old_current_value)
    {
		calibration_pulse[nscan] = upevt_.alctcal_current_value;
		nscan++ ;
		old_current_value = upevt_.alctcal_current_value;
		get_next_scan (nscan);
		npoints = 0;    
    }
 
/* Keep count of the number of calibration points (could get this from
 * the calibration data block, except right now get_next_event doesn't
 * unpack it), and the number of events at each point.
 */

	if (upevt_.alctcal_current_threshold != old_current_threshold)
    {
		npoints++;
		old_current_threshold = upevt_.alctcal_current_threshold;
		nperpoint = 0;
    }
	nperpoint++;

	hid = WIRE_HIST_OFFSET+10000*(nscan-1) +3000;
	HFILL(hid, (float )(upevt_.event_number), 0.0, upevt_.time_musec);

/* Loop over the layers and wires of the alct_dump array */
	for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
		for (iwire = 0; iwire < nwires; iwire++)
		{
			if (upevt_.alct_dump[ilayer][iwire] > 0)
			{
				hid = WIRE_HIST_OFFSET+10000*(nscan-1) + 2000 + 120 * 
					(ilayer + 1) + (iwire + 1);
				HFILL(hid, (float )(upevt_.alctcal_current_threshold), 0.0, 1.0);
			}
		}
    }

	for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
		for (iwire = 0; iwire < nwires; iwire++)
		{
			if (upevt_.alct_dump[ilayer][iwire])
			{

				/* Make a list of times for this wire (time is in units of 25 ns) */
				nhits = 0;
				disc_on = 0;
				for (j = 0; j < 32; j++)
				{
					if (upevt_.alct_dump[ilayer][iwire] & tbit[j])
					{
						if (!disc_on)
						{
							itime[nhits++] = j;
							hid = WIRE_HIST_OFFSET+10000*(nscan-1) +1000+ 
								120*(ilayer+1) + iwire+1;
							HFILL(hid, (float )(j+1), 0.0, 1.0);
							disc_on = 1;
						}
					}
					else disc_on = 0;
				}

				afeb = (iwire/8)*3 + 1 + ilayer/2; // ALCT2001 numbering
				for (j = 0; j < nhits; j++)
				{
					/* Fill wire occupancy in a layer */
					hid = WIRE_HIST_OFFSET+10000*(nscan-1) + ilayer + 1;
					HFILL(hid, (float )(iwire + 1), 0.0, 1.0);

					/* Fill Anode front-end board histograms */
					hid = WIRE_HIST_OFFSET+10000*(nscan-1) + 100 + afeb;
					for (j = 0; j < nhits; j++) HFILL(hid, (float )itime[j], 0.0, 1.0);
				}
				/*  Calibration ALCT  */

//	afeb = (iwire/8) + 1 + 3 * ( 1+(int)((ilayer)/2) );
				hid = WIRE_HIST_OFFSET+10000*(nscan-1) + 200 + afeb;
				HFILL(hid, (float )(upevt_.alctcal_current_threshold), 0.0, 1.0);

				/* How many layers have hits?  */ 
				layers_hit[ilayer] = 1; 
			}
		}
    }

	/* Fill layers hit             */
	hit_layers = 0;
	for (ilayer = 0; ilayer < NLAYER ; ilayer++)
    {
		if (layers_hit[ilayer] == 1) hit_layers++;
    }
	hid = WIRE_HIST_OFFSET+10000*(nscan-1);
	HFILL(hid, (float)hit_layers, 0., 1.);

	event_number++;
	return 0;
}


/******************************************************************************
   End-of-run analysis for FAST site test 13
******************************************************************************/

int test_13_end(int pass)
  {
  printf("npoints=%d, nperpoint=%d\n", npoints, nperpoint);
  get_next_scan(nscan+1);  // we should analyze last scan
  return 0;
  }

/******************************************************************************
   End-of-test analysis for FAST site test 13
******************************************************************************/

int test_13_finish(void)
{
  
	char *file_name;
	FILE *fp_chip, *fp_thr;
	FILE *fp_slop;

	float   lim_slop[7] =    SLOP_LIMITS_SCAN_00;
	float   lim_delta_dac[10][7] = { DELTA_DAC_LIMITS_SCAN_00,  
									 DELTA_DAC_LIMITS_SCAN_01}; 
	float   delta_dac[NMAXSCAN][NLAYER][nwires];
	int iscan, iwire, ilayer, icolumn, iafeb, ichan;
	float dac[2][3*nwires/8];
	int idac[2][3*nwires/8];
	float slop[NLAYER][nwires];
//  float delta_inj;
	float delta_pulse;
	int page_number;
	char title[80];

	for( iscan = 0 ; iscan < 2; iscan++) 
    {
		file_name = malloc(strlen(datadir_translation) +
						   strlen(FILE_RESULTS) + 20);
		if(file_name == NULL)
		{
			printf("malloc() failed for filename.\n");
			return 1;
		}
		sprintf(file_name,"%s%s%02d.result", datadir_translation,
				FILE_RESULTS, 4*iscan + 3); 
		if ( (fp_chip = fopen(file_name, "w")) == NULL)
		{
			free(file_name);
			printf (" Test_13 Print results: can not open file %s\n", file_name);
			return 1;
		}
		free(file_name);
		fprintf(fp_chip, "-----------------------------------------"
				"------------------------------\n");
		fprintf(fp_chip, "Results of test 13 (AFEB Threshold and Analog "
				"Noise Test ) %s\n", csc_name);
		fprintf(fp_chip, "Optimal DAC%1d values  per chip for %f "
				"fC threshold\n",iscan+1,
				C_DEFAULT*(CALIBRATION_PULSE_A0+CALIBRATION_PULSE_A1*calibration_pulse[iscan]));
	    //	    INJECT_PULSE_A0+INJECT_PULSE_A1*calibration_pulse[iscan]);
		fprintf(fp_chip, "-----------------------------------------"
				"------------------------------\n");

		for (iafeb = 0; iafeb < 3*nwires/8; iafeb++)
		{
			ilayer = 2 * (iafeb%3);
			icolumn = iafeb/3;
			dac[iscan][iafeb] = 0.;

			for (ichan = 0; ichan<8; ichan++)
			{
				iwire  = 8 * icolumn + ichan; 
				dac[iscan][iafeb] += thr[iscan][ilayer][iwire]; 
				dac[iscan][iafeb] += thr[iscan][ilayer+1][iwire]; 
			}
			dac[iscan][iafeb] /= 16.;
			idac[iscan][iafeb] = dac[iscan][iafeb] + 0.5;
		}
    
		for(icolumn = 0; icolumn < nwires/8; icolumn++)
		{
			fprintf (fp_chip," %2d %3d     %2d %3d     %2d %3d\n", 
					 icolumn*3+1, idac[iscan][icolumn*3],  
					 icolumn*3+2, idac[iscan][icolumn*3+1],
					 icolumn*3+3, idac[iscan][icolumn*3+2]);
		} 
		print_time_close(fp_chip);

		file_name = malloc(strlen(datadir_translation) +
						   strlen(FILE_RESULTS) + 20);
		if(file_name == NULL)
		{
			printf("malloc() failed for filename.\n");
			return 1;
		}
		sprintf(file_name,"%s%s%02d.result", 
				datadir_translation, FILE_RESULTS, 4*iscan+4); 
		if ( (fp_thr = fopen(file_name, "w")) == NULL)
		{
			free(file_name);
			printf (" Test_13 Print results: can not open file %s\n", file_name);
			return 1;
		}
		free(file_name);

		fprintf(fp_thr, "--------------------------------------"
				"---------------------------------\n");
		fprintf(fp_thr, "Results of test 13 (AFEB Threshold and "
				"Analog Noise Test ) %s\n", csc_name);
		fprintf(fp_thr, "Threshold offset (in DAC) per chanel for %f "
				"fC threshold\n", 
				C_DEFAULT*(CALIBRATION_PULSE_A0+CALIBRATION_PULSE_A1*calibration_pulse[iscan]));
	    //	   INJECT_PULSE_A0+INJECT_PULSE_A1*calibration_pulse[iscan]);
		fprintf(fp_thr, "--------------------------------------"
				"---------------------------------\n");

		for (iwire = 0; iwire < nwires; iwire++)
		{
			for (ilayer = 0; ilayer < NLAYER; ilayer++)
			{
				delta_dac[iscan][ilayer][iwire] =
					thr[iscan][ilayer][iwire] - idac[iscan][get_afeb(ilayer,iwire)];
			}

			fprintf(fp_thr," %2d %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f\n", iwire+1,
					delta_dac[iscan][0][iwire], delta_dac[iscan][1][iwire],
					delta_dac[iscan][2][iwire], delta_dac[iscan][3][iwire],
					delta_dac[iscan][4][iwire], delta_dac[iscan][5][iwire]);
		}
		print_time_close(fp_thr);

		page_number = 4*(iscan+1); 
		sprintf( title, "AFEB Thr. offset for %6.1f fC ", 
				 C_DEFAULT*(CALIBRATION_PULSE_A0+CALIBRATION_PULSE_A1*calibration_pulse[iscan]));
		// INJECT_PULSE_A0+calibration_pulse[iscan]*INJECT_PULSE_A1 );
    

		one_page_plot_v3(13, page_number, WIRE_PLOT, 
						 (float *)delta_dac[iscan], 
						 (float *)delta_dac[iscan], 
						 lim_delta_dac[iscan], csc_name, 
						 title, 
						 "Threshold, DAC counts", 0);

		check_bad_channels( delta_dac[iscan], lim_delta_dac[iscan][4], 
							lim_delta_dac[iscan][5],
							" Delta DAC of Threshold out of Range");
    }

	file_name = malloc(strlen(datadir_translation) +
					   strlen(FILE_RESULTS) + 20);
	if(file_name == NULL)
    {
		printf("malloc() failed for filename.\n");
		return 1;
    }
	sprintf(file_name,"%s%s%02d.result", datadir_translation,
			FILE_RESULTS, 9); 
	if ( (fp_slop = fopen(file_name, "w")) == NULL)
    {
		free(file_name);
		printf (" Test_13 Print results: can not open file %s\n", file_name);
		return 1;
    }
	free(file_name);
  
	fprintf(fp_slop, "------------------------------------------"
			"-----------------------------\n");
	fprintf(fp_slop, "Results of test 13 (AFEB Threshold and "
			"Analog Noise Test ) %s\n", csc_name);
	fprintf(fp_slop, "Threshold slop (in DACthr/fC) vs chanel\n");
	fprintf(fp_slop, "------------------------------------------"
			"-----------------------------\n");
  
	for (iwire = 0; iwire < nwires; iwire++)
    {
    
		for (ilayer = 0; ilayer < NLAYER; ilayer++)
			//      slop[ilayer][iwire] = (thr[1][ilayer][iwire]-thr[0][ilayer][iwire]) /
			//(calibration_pulse[1]-calibration_pulse[0])/INJECT_PULSE_A1;
			slop[ilayer][iwire] = (thr[1][ilayer][iwire]-thr[0][ilayer][iwire]) /
				( (calibration_pulse[1]-calibration_pulse[0])*
				  cal_cap[get_afeb(ilayer,iwire)]*
				  CALIBRATION_PULSE_A1 );
  
		fprintf(fp_slop," %2d %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f\n", iwire+1,
				slop[0][iwire], slop[1][iwire], slop[2][iwire],
				slop[3][iwire], slop[4][iwire], slop[5][iwire]);
    }
	print_time_close(fp_slop);

	page_number = 9; 
	one_page_plot_v3(13, page_number, WIRE_PLOT, 
					 (float *)slop, 
					 (float *)slop, 
					 lim_slop, csc_name, 
					 "AFEB Threshold slopes  ", 
					 "Slope, DACthr/fC ", 0);

 
	file_name = malloc(strlen(datadir_translation) +
					   strlen(FILE_RESULTS) + 20);
	if(file_name == NULL)
    {
		printf("malloc() failed for filename.\n");
		return 1;
    }
    
	sprintf(file_name,"%s%s%02d.result", 
			datadir_translation, FILE_RESULTS, 10); 
	printf (" Test_13 Print result for  %s\n", file_name);
	if ( (fp_slop = fopen(file_name, "w")) == NULL)
    {
		free(file_name);
		printf (" Test_13 Print results: can not open file %s\n", file_name);
		return 1;
    }
	free(file_name);

	fprintf(fp_slop, "-------------------------------------------"
			"-------------------------------------------\n");
	fprintf(fp_slop, "Results of test 13 (AFEB Threshold and "
			"Analog Noise Test ) %s\n", csc_name);
	fprintf(fp_slop, "Threshold slop (in DACthr/fC) vs AFEB\n");
	fprintf(fp_slop, "-------------------------------------------"
			"-------------------------------------------\n");
  
	//  delta_inj = (calibration_pulse[1] - calibration_pulse[0])*INJECT_PULSE_A1; 
	delta_pulse = (calibration_pulse[1] - calibration_pulse[0])*CALIBRATION_PULSE_A1;
	for (icolumn = 0; icolumn < nwires/8; icolumn++)
    {
		iafeb=icolumn*3;
		fprintf (fp_slop," %2d %6.2f       %2d %6.2f        %2d %6.2f\n",
				 iafeb+1,
				 (dac[1][iafeb]   - dac[0][iafeb]  ) / delta_pulse/ cal_cap[iafeb],
				 iafeb+2,
				 (dac[1][iafeb+1] - dac[0][iafeb+1]) / delta_pulse/ cal_cap[iafeb+1],
				 iafeb+3,
				 (dac[1][iafeb+2] - dac[0][iafeb+2]) / delta_pulse/ cal_cap[iafeb+2]);
    }
	/*
	  for (icolumn = 0; icolumn < nwires/8; icolumn++)
	  {
	  fprintf (fp_slop," %2d %6.2f       %2d %6.2f        %2d %6.2f\n", 
	  icolumn*3+1, 
	  (dac[1][icolumn*3]   - dac[0][icolumn*3]  ) / delta_inj,  
	  icolumn*3+2, 
	  (dac[1][icolumn*3+1] - dac[0][icolumn*3+1]) / delta_inj,
	  icolumn*3+3, 
	  (dac[1][icolumn*3+2] - dac[0][icolumn*3+2]) / delta_inj)  ;
	  } 
	*/
	print_time_close(fp_slop);  

	check_bad_channels( slop, lim_slop[4], lim_slop[5], 
						" DAC/fc slope for Threshold out of Range");

	calc_dac_20_fC();
	print_20_result();

	if (number_bad_channels > 0) 
		fprintf(fp_bad," Total %d problem(s) was(were) found.\n Test Failed. \n",
				number_bad_channels);
	else
		fprintf(fp_bad," There was no problem for this test.\n Test OK. \n");
	print_time_close(fp_bad);    
  
	free(pthr);
	free(pnoise);
	free(pthr_20);
	free(idac_20);
	free(cal_cap);
	free(cal_gain);
	return 0;
}

/*----------------------------- calc_dac_20_fC ----------------------------*/
int calc_dac_20_fC()
{
  int iafeb, ilayer, iwire, icolumn, ichan;
  float dac_20[NLAYER*nwires/16];
  //  float weight1, weight2, delta_signal;
    float q1, q2;


  for (iwire = 0; iwire < nwires; iwire++)
    {
      for (ilayer = 0; ilayer < NLAYER; ilayer++)
        {
          q1 = (CALIBRATION_PULSE_A0+CALIBRATION_PULSE_A1 * calibration_pulse[0])
                * cal_cap[get_afeb(ilayer,iwire)] ;
          q2 = (CALIBRATION_PULSE_A0+CALIBRATION_PULSE_A1 * calibration_pulse[1])
                * cal_cap[get_afeb(ilayer,iwire)] ;
          if(abs(q2-q1) > 1.)
            thr_20[ilayer][iwire] =
              ((q2-20.)*thr[0][ilayer][iwire]+ (20.-q1)*thr[1][ilayer][iwire])/(q2-q1);
          else
            thr_20[ilayer][iwire] = -99.; //not defined
        }
    }

  /*
  delta_signal = (calibration_pulse[1]-calibration_pulse[0]);
  if (abs(delta_signal) <= 0.0001) // too small difference
    {
      printf(" The difference of injected pulses for 1 and 2 scan is too small\n");    
      weight1= 0.;
      weight2= 0.;
    }
  else
    {
      weight1 = (calibration_pulse[1] - (20.-INJECT_PULSE_A0)/INJECT_PULSE_A1) /delta_signal;
      weight2 = ((20.-INJECT_PULSE_A0)/INJECT_PULSE_A1 - calibration_pulse[0] ) /delta_signal;
    }

  for (iwire = 0; iwire < nwires; iwire++)
`    {
      for (ilayer = 0; ilayer < NLAYER; ilayer++)
	{
	  thr_20[ilayer][iwire] = weight1*thr[0][ilayer][iwire]+ weight2*thr[1][ilayer][iwire];
	}
    }
  */
  for (iafeb = 0; iafeb < 3*nwires/8; iafeb++)
    {
      ilayer = 2 * (iafeb%3);
      icolumn = iafeb/3;
      dac_20[iafeb] = 0.;
      
      for (ichan = 0; ichan<8; ichan++)
	{
	  iwire  = 8 * icolumn + ichan; 
	  dac_20[iafeb] += thr_20[ilayer][iwire]; 
	  dac_20[iafeb] += thr_20[ilayer+1][iwire]; 
	}
      dac_20[iafeb] /= 16.;
      idac_20[iafeb] = dac_20[iafeb] + 0.5;
    }

  return 0;
}
/*----------------------------- get_afeb ----------------------------------*/
int get_afeb(int ilayer, int iwire)
  {
  int icolumn;
  icolumn = iwire/8;
  return (ilayer/2 + 3 * icolumn);
  }

/*----------------------------- print_time_close --------------------------*/
int print_time_close(FILE *fp)
  {
  char timestring[40];
  time_t t;

  time(&t);
  sprintf(timestring, "%s", ctime(&t));
  fprintf(fp, "\n# %s\n\n", timestring);

  fclose(fp);
  return 0;
  }

/*-------------------------- print_20_result -------------------------------*/
int print_20_result()
  {
  int       n_wr_ch = 0;
  int       iwire, ilayer, iafeb;
  char prototype_name[80] = FILE_RESULTS;
  char *file_name;
  FILE *fp_offset, *fp_thr, *fp_thr_sum;

  float dac_offset[NLAYER][nwires];
  float   lim_thr_20[7] = THR_LIMITS_20;
  float   lim_offset_20[7] = OFF_LIMITS_20;

  // Open files for results

  file_name = malloc(strlen(datadir_translation) +
		     strlen(FILE_RESULTS) + 20);
  if(file_name == NULL)
    {
    printf("malloc() failed for filename.\n");
    return 1;
    }
  sprintf(file_name,"%s%s%02d.result",
	  datadir_translation, prototype_name, 13); 
  if ( (fp_thr_sum = fopen(file_name, "w")) == NULL)
    {
      free(file_name);
      printf (" Test_13 Print results: can not open file %s\n", file_name);
      return 1;
    } 
  free(file_name);

  file_name = malloc(strlen(datadir_translation) +
		     strlen(FILE_RESULTS) + 20);
  if(file_name == NULL)
    {
    printf("malloc() failed for filename.\n");
    return 1;
    }
  sprintf(file_name,"%s%s%02d.result",
	  datadir_translation, prototype_name, 11); 
  if ( (fp_offset = fopen(file_name, "w")) == NULL)
    {
      free(file_name);
      printf (" Test_13 Print results: can not open file %s\n", file_name);
      return 1;
    } 
  free(file_name);

  file_name = malloc(strlen(datadir_translation) +
		     strlen(FILE_RESULTS) + 20);
  if(file_name == NULL)
    {
      printf("malloc() failed for filename.\n");
      return 1;
    }
  sprintf(file_name,"%s%s%02d.result",
	  datadir_translation, prototype_name, 12); 
  if ( (fp_thr = fopen(file_name, "w")) == NULL)
    {
      free(file_name);
      printf (" Test_13 Print results: can not open file %s\n", file_name);
      return 1;
    }
  free(file_name);

  fprintf(fp_offset, "--------------------------------------"
           	     "--------------------------------------\n");
  fprintf(fp_offset, "Results of test 13 (AFEB Threshold and "
	             "Analog Noise Test ) %s\n", csc_name);
  fprintf(fp_offset, "--------------------------------------"
           	     "--------------------------------------\n");

  fprintf(fp_thr ,   "--------------------------------------"
           	     "--------------------------------------\n");
  fprintf(fp_thr, "Results of test 13 (AFEB Threshold and "
	          "Analog Noise Test ) %s\n", csc_name);
  fprintf(fp_thr ,   "--------------------------------------"
           	     "--------------------------------------\n");

  fprintf(fp_thr_sum, "#--------------------------------------"
              	       "--------------------------------------\n");
  fprintf(fp_thr_sum, "#Results of test 13 (AFEB Threshold and "
	              "#Analog Noise Test ) %s\n", csc_name);
  fprintf(fp_thr_sum, "#--------------------------------------"
              	       "--------------------------------------\n");


  fprintf(fp_offset, "\nCalculated offset  for 20 fC, in DACthr\n");
  fprintf(fp_offset, "wiregroup    Layer 1    Layer 2    Layer 3    "
	                         "Layer 4    Layer 5    Layer 6 \n");
  fprintf(fp_offset, "--------------------------------------"
           	     "--------------------------------------\n");

  for (iwire = 0; iwire < nwires; iwire++)
    {
      for (ilayer = 0; ilayer < NLAYER; ilayer++)
	{
	  dac_offset[ilayer][iwire] =thr_20[ilayer][iwire]-idac_20[get_afeb(ilayer,iwire)]; 
	}
    
      fprintf(fp_offset, "   %2d     %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f _\n", 
	               iwire + 1, 
	               dac_offset[0][iwire],dac_offset[1][iwire],
	               dac_offset[2][iwire],dac_offset[3][iwire],
	               dac_offset[4][iwire],dac_offset[5][iwire]);
    }

  fprintf(fp_thr, "\nCalculated threshold for 20 fC, in DACthr\n");
  fprintf(fp_thr, "wiregroup    Layer 1    Layer 2    Layer 3    "
	                       "Layer 4    Layer 5    Layer 6  \n");
  fprintf(fp_thr, "--------------------------------------"
	          "--------------------------------------\n");

  fprintf(fp_thr_sum, "\n#Calculated threshold for 20 fC, in DACthr\n");
  fprintf(fp_thr_sum, "#AFEB #   DAC thr for 20 fC\n");
  if (csc_type == 2 || csc_type == 4)
      fprintf(fp_thr_sum, "#For this chamber type, first 6 thresholds in this file are dummies (100)\n");
  fprintf(fp_thr_sum, "#--------------------------------------"
	              "--------------------------------------\n");

// if chamber type 1/3, 3/1 or 4/1 - add 6 dummy AFEBs in front of all the rest
// in file test_13_13.result, because ALCT slow control library 
// requires the exact number of AFEBs in the threshold config file 
// (Madorsky 12/07/03)

  if (csc_type == 2 || csc_type == 4 || csc_type == 6)
  {
      for (iafeb = 0; iafeb < 6; iafeb++)
	  fprintf(fp_thr_sum, "   %2d     100\n", iafeb+1); 

      for (iafeb = 0; iafeb < NLAYER*nwires/16; iafeb++)
      {
	  fprintf(fp_thr_sum, "   %2d     %4d\n", iafeb+7, idac_20[iafeb]); 
      }
  }
  else
  {
      for (iafeb = 0; iafeb < NLAYER*nwires/16; iafeb++)
      {
	  fprintf(fp_thr_sum, "   %2d     %4d\n", iafeb+1, idac_20[iafeb]); 
      }
  }

  for (iwire = 0; iwire < nwires; iwire++)
    {
      fprintf(fp_thr, "   %2d     %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f _\n", 
	      iwire + 1, 
	      thr_20[0][iwire], thr_20[1][iwire], 
	      thr_20[2][iwire], thr_20[3][iwire], 
	      thr_20[4][iwire], thr_20[5][iwire]); 
    }
    
/* Plot results */

  one_page_plot_v3(13, 11, WIRE_PLOT, 
		   (float *)pthr_20,
		   (float *)pthr_20,
		   lim_thr_20, csc_name, 
		   "Calculated Thr. for 20 fC ", 
		   "Threshold, DAC counts", 0);
 
   n_wr_ch = check_bad_channels((float*)pthr_20, lim_thr_20[4], lim_thr_20[5],
			"  Calc.Thr. for 20fC out of Range");

  if (n_wr_ch > 0) fprintf(fp_thr, "Test result = Failed\n\n");
  else fprintf(fp_thr, "Test result = OK\n\n");
  
  one_page_plot_v3(13, 12, WIRE_PLOT, 
		   (float *)dac_offset, 
		   (float *)dac_offset, 
		   lim_offset_20, csc_name, 
		   "Calc. Offsets for 20 fC ",  
		   "Offset, DAC counts", 0);
  n_wr_ch = check_bad_channels( dac_offset, lim_offset_20[4], lim_offset_20[5], 
			"Calc. Offsetfor 20 fC out of Range");
 if (n_wr_ch > 0) fprintf(fp_offset, "Test result = Failed\n\n");
  else fprintf(fp_offset, "Test result = OK\n\n");

/* Add the date and time */
  print_time_close(fp_offset);
  print_time_close(fp_thr);
  print_time_close(fp_thr_sum);
 
  return 0;
  }
/*-------------------------- print_scan_result -----------------------------*/
#define MAX_CALIB_POINTS   256

int print_scan_result(int scan_number)
  {
  int       n_wr_ch = 0;
  int       iwire, lim_scan;
  char prototype_name[80] = FILE_RESULTS;
  char *file_name;
  char title[80];
  FILE *fp_noise, *fp_thr;
  
  float   lim_thr[10][7] = { 
    THR_LIMITS_SCAN_00,    THR_LIMITS_SCAN_01, 
    THR_LIMITS_SCAN_02,    THR_LIMITS_SCAN_03, 
    THR_LIMITS_SCAN_04,    THR_LIMITS_SCAN_05, 
    THR_LIMITS_SCAN_06,    THR_LIMITS_SCAN_07, 
    THR_LIMITS_SCAN_08,    THR_LIMITS_SCAN_09};

  float   lim_noise[10][7] = { 
    NOISE_LIMITS_SCAN_00,    NOISE_LIMITS_SCAN_01, 
    NOISE_LIMITS_SCAN_02,    NOISE_LIMITS_SCAN_03, 
    NOISE_LIMITS_SCAN_04,    NOISE_LIMITS_SCAN_05, 
    NOISE_LIMITS_SCAN_06,    NOISE_LIMITS_SCAN_07, 
    NOISE_LIMITS_SCAN_08,    NOISE_LIMITS_SCAN_09};

  int page_number;

  // Open files for results

  if ( scan_number > 10) scan_number = 10 ;
  if ( scan_number <  1) scan_number =  1 ;

  file_name = malloc(strlen(datadir_translation) +
		     strlen(FILE_RESULTS) + 20);
  if(file_name == NULL)
    {
    printf("malloc() failed for filename.\n");
    return 1;
    }
  sprintf(file_name,"%s%s%02d.result",
	  datadir_translation, prototype_name, 4*(scan_number-1)+1); 
  if ( (fp_noise = fopen(file_name, "w")) == NULL)
    {
      free(file_name);
      printf (" Test_13 Print results: can not open file %s\n", file_name);
      return 1;
    } 
  free(file_name);

  file_name = malloc(strlen(datadir_translation) +
		     strlen(FILE_RESULTS) + 20);
  if(file_name == NULL)
    {
      printf("malloc() failed for filename.\n");
      return 1;
    }
  sprintf(file_name,"%s%s%02d.result",
	  datadir_translation, prototype_name, 4*(scan_number-1)+2); 
  if ( (fp_thr = fopen(file_name, "w")) == NULL)
    {
      free(file_name);
      printf (" Test_13 Print results: can not open file %s\n", file_name);
      return 1;
    }
  free(file_name);

  fprintf(fp_noise, "--------------------------------------"
           	    "--------------------------------------\n");
  fprintf(fp_noise, "Results of test 13 (AFEB Threshold and "
	            "Analog Noise Test ) %s\n", csc_name);
  fprintf(fp_noise, "--------------------------------------"
           	    "--------------------------------------\n");

  fprintf(fp_thr  , "--------------------------------------"
	            "--------------------------------------\n");
  fprintf(fp_thr ,  "Results of test 13 (AFEB Threshold and "
	            "Analog Noise Test ) %s\n", csc_name);
  fprintf(fp_thr  , "--------------------------------------"
	            "--------------------------------------\n");

  fprintf(fp_noise, "\nMeasured Noise for all channels, in DACthr\n");
  fprintf(fp_noise, "wiregroup    Layer 1    Layer 2    Layer 3    "
	                         "Layer 4    Layer 5    Layer 6 \n");
  fprintf(fp_noise, "--------------------------------------"
           	    "--------------------------------------\n");

  for (iwire = 0; iwire < nwires; iwire++)
    {
    fprintf(fp_noise, "   %2d     %10.2f %10.2f %10.2f %10.2f %10.2f "
	              "%10.2f _\n", 
	               iwire + 1, 
                       noise[scan_number-1][0][iwire], noise[scan_number-1][1][iwire], 
	               noise[scan_number-1][2][iwire], noise[scan_number-1][3][iwire], 
	               noise[scan_number-1][4][iwire], noise[scan_number-1][5][iwire]);
    }

  fprintf(fp_thr, "\nMeasured threshold for all channels, in DACthr\n");
  fprintf(fp_thr, "wiregroup    Layer 1    Layer 2    Layer 3    "
	                       "Layer 4    Layer 5    Layer 6  \n");
  fprintf(fp_thr, "--------------------------------------"
	          "--------------------------------------\n");

  for (iwire = 0; iwire < nwires; iwire++)
    {
    fprintf(fp_thr, "   %2d     %10.2f %10.2f %10.2f %10.2f %10.2f "
	    " %10.2f _\n", 
	             iwire + 1, 
                     thr[scan_number-1][0][iwire], thr[scan_number-1][1][iwire], 
	             thr[scan_number-1][2][iwire], thr[scan_number-1][3][iwire], 
	             thr[scan_number-1][4][iwire], thr[scan_number-1][5][iwire]);
    }

 
/* Plot results */

  lim_scan = scan_number - 1;
  page_number = 4*(scan_number-1) + 2; 

  //  sprintf( title, "Thresholds for %6.1f fC ", 
  //	INJECT_PULSE_A0+calibration_pulse[scan_number-1]*INJECT_PULSE_A1 );
  
  sprintf( title, "Thresholds for %6.1f fC ",    C_DEFAULT*
           (CALIBRATION_PULSE_A0+calibration_pulse[scan_number-1]*CALIBRATION_PULSE_A1) );
 
  one_page_plot_v3(13, page_number, WIRE_PLOT, 
		   (float *)thr[scan_number-1][0], 
		   (float *)thr[scan_number-1][0], 
		   lim_thr[lim_scan], csc_name, 
		   title, 
		   "Threshold, DAC counts", 0);
 
  // sprintf( title, "Thr for %6.1f fC out of limits ", 
  //	INJECT_PULSE_A0+calibration_pulse[scan_number-1]*INJECT_PULSE_A1 );
  
  sprintf( title, "Noise  for %6.1f fC ", C_DEFAULT*
           (CALIBRATION_PULSE_A0+calibration_pulse[scan_number-1]*CALIBRATION_PULSE_A1) );

  n_wr_ch = check_bad_channels( thr[scan_number-1][0], 
				lim_thr[lim_scan][4], lim_thr[lim_scan][5], 
				title);
 if (n_wr_ch > 0) fprintf(fp_thr, "Test result = Failed\n\n");
 else fprintf(fp_thr, "Test result = OK\n\n");

 //sprintf( title, "Noise  for %6.1f fC ", 
 //	INJECT_PULSE_A0+calibration_pulse[scan_number-1]*INJECT_PULSE_A1 );
  
 sprintf( title, "Noise for %6.1f fC out of limits ",  C_DEFAULT*
           (CALIBRATION_PULSE_A0+calibration_pulse[scan_number-1]*CALIBRATION_PULSE_A1) );

 page_number = 4*(scan_number-1) + 1; 
  one_page_plot_v3(13, page_number, WIRE_PLOT, 
		   (float *)noise[scan_number-1][0], 
		   (float *)noise[scan_number-1][0], 
		   lim_noise[lim_scan], csc_name, 
		   title, 
		   "Noise, DAC counts", 0);

 // sprintf( title, "Noise for %6.1f fC out of limits ", 
 //	INJECT_PULSE_A0+calibration_pulse[scan_number-1]*INJECT_PULSE_A1 );
   sprintf( title, "Noise for %6.1f fC out of limits ",  C_DEFAULT*		            
	(CALIBRATION_PULSE_A0+calibration_pulse[scan_number-1]*CALIBRATION_PULSE_A1) );

  n_wr_ch = check_bad_channels( noise[scan_number-1][0], 
				lim_noise[lim_scan][4], lim_noise[lim_scan][5], 
				title);
 if (n_wr_ch > 0) fprintf(fp_noise, "Test result = Failed\n\n");
 else fprintf(fp_noise, "Test result = OK\n\n");

/* Add the date and time */
  print_time_close(fp_noise);
  print_time_close(fp_thr);
 
  return 0;
  }

/*------------------------ check_bad_channels ------------------------------*/

int check_bad_channels( float** value, float lim_low,
		    float lim_high, char* comment)
  {
    int n_bad_channels; // number of bad channels for this check
    int ilayer, iwire;

    n_bad_channels= 0;

  for (ilayer = 0; ilayer< NLAYER; ilayer++)
    {
    for (iwire = 0; iwire< nwires; iwire++)
      {
      if ( (value[ilayer][iwire] < lim_low) ||
	   (value[ilayer][iwire] > lim_high))
	{
	fprintf (fp_bad,"%d %3d %8.3f  %8.3f  %8.3f %s\n",
		 ilayer+1, iwire+1, value[ilayer][iwire],
		 lim_low, lim_high, comment);
	number_bad_channels++; // statistics of bad channels;
	n_bad_channels++;
	}
      }
    }
  return n_bad_channels;
  }


/*----------------------------- get_next_scan -------------------------------*/
int get_next_scan( int nscan)
  {

  if ( nscan != 1)
    {              // we don't need to fit and save at the begining 
    fit_thresh(nscan-1);
    save_hist(nscan-1);
    print_scan_result(nscan-1);
    HDELET(0);
    }
  init_hist (nscan);
  return 0;
  }

/*----------------------------- save_hist ----------------------------------*/
int save_hist(int scan_number)
  {

  char file_name_element[] = "/test_results/temp/test_13_";
  char *file_name;
  if ( scan_number > 10) scan_number = 10 ;
  if ( scan_number <  1) scan_number =  1 ;

  file_name = malloc(strlen(datadir_translation) +
		     strlen(file_name_element) + 10);
  if (file_name == NULL)
    {
    printf("malloc() failed for file_name.\n");
    return -1;
    }
  sprintf(file_name,"%s%s%02d.his",datadir_translation,
	  file_name_element, scan_number); 
/* Save histograms */
  printf("Saving histograms in file %s\n", file_name);
  HRPUT(0, file_name, "N");

  free(file_name);
  return 0;
  }

/*-------------------------- fit_thresh -------------------------------------*/
int fit_thresh(int scan_number)
  {

  int       i, hid, ID, ilayer, iwire, i_ch;
  int       content[MAX_CALIB_POINTS];
  float     chisq, mean, par[3], rms;
  float     errors[MAX_CALIB_POINTS];
/*
  float   lim_thr[10][7] = { 
    THR_LIMITS_SCAN_00,    THR_LIMITS_SCAN_01, 
    THR_LIMITS_SCAN_02,    THR_LIMITS_SCAN_03, 
    THR_LIMITS_SCAN_04,    THR_LIMITS_SCAN_05, 
    THR_LIMITS_SCAN_06,    THR_LIMITS_SCAN_07, 
    THR_LIMITS_SCAN_08,    THR_LIMITS_SCAN_09};

  float   lim_noise[10][7] = { 
    NOISE_LIMITS_SCAN_00,    NOISE_LIMITS_SCAN_01, 
    NOISE_LIMITS_SCAN_02,    NOISE_LIMITS_SCAN_03, 
    NOISE_LIMITS_SCAN_04,    NOISE_LIMITS_SCAN_05, 
    NOISE_LIMITS_SCAN_06,    NOISE_LIMITS_SCAN_07, 
    NOISE_LIMITS_SCAN_08,    NOISE_LIMITS_SCAN_09};

*/
  
/* Fit individual wire group turn-on curves with the error function */
  i_ch = 0;
  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
    for (iwire = 0; iwire < nwires; iwire++)
      {
	ID = WIRE_HIST_OFFSET + (scan_number-1) * 10000 + 2000 + 
	120 * (ilayer + 1) + (iwire + 1);
	for (i = first_threshold_in_scan ; i < first_threshold_in_scan+npoints; i++)
	  {
	    content[i] = HI(ID,i+1);
	  }

	calc_thresh(npoints, &content[first_threshold_in_scan], errors, par, &chisq);

	hid = WIRE_HIST_OFFSET + (scan_number-1)*10000 + 10;
	HFILL(hid, chisq, 0., 1.);

      if (chisq >= 0.)
	{
	  mean = par[1]+first_threshold_in_scan;
	  rms = par[2];
	}
      else
	{
	  mean = -1;
	  rms = -1;
	  printf("  Layer %d wiregroup %2d has threshold = %6.2f and noise"
	       " = %6.2f", ilayer + 1, iwire + 1, mean, rms);
	  printf("\tpar0 %f, mean %f, rms %f, chisq %f \n", 
	       par[0], par[1]+first_threshold_in_scan, par[2], chisq); 
	}


      hid = WIRE_HIST_OFFSET + (scan_number-1)*10000 + 300;
      i_ch++;
      HFILL(hid+4, (float)(i_ch) , 0.0, mean);
      HFILL(hid+5, mean , 0.0, 1.);
      HFILL(hid+6, (float)(i_ch) , 0.0, rms);
      HFILL(hid+7, rms , 0.0, 1.);

      thr[scan_number-1][ilayer][iwire] = mean;
      noise[scan_number-1][ilayer][iwire] = rms;
      }
    }

  return 0;

  }

/*------------------------------ init_hist ---------------------------------*/
int init_hist( int scan_number)
  {
  int tmp_hist_offset;
  int adb, hid, ilayer, ifeb, iwire;
  // int ichip;
  char htitle[80];

  // since this histogramm will be needed only for a temporary purpose
  if (scan_number > 10) scan_number=10; 
  // 10 is just enough for debuging 
  /* Book histograms for CSC wires */
 
  tmp_hist_offset = WIRE_HIST_OFFSET + (scan_number-1) * 10000;

  HBOOK1(tmp_hist_offset+10, "chisq of erf fit", 100, 0., 20., 0.);
  HBOOK1(tmp_hist_offset, "Number of layers with hit wires", 
	 7, -0.5, 6.5, 0.0);

  for (ilayer=1; ilayer <= NLAYER; ilayer++)
    {
    hid = tmp_hist_offset + ilayer ;
    sprintf( htitle, "ALCT Wire occupancy, Layer %d", ilayer);
    HBOOK1( hid, htitle, nwires, 0.5, (float)nwires+0.5, 0.0);
    }

  /* Book anode front-end board (AFEB) histograms */

//  for (ifeb = 1; ifeb <= NAFEB; ifeb++)
  for (ifeb = 1; ifeb <= (nwires*3/8); ifeb++)
    {
//    for (ichip = 1; ichip <= NAFEB_CHIP; ichip++)
      {
//      adb = 6 * ifeb + ichip -6;   /* Front end board 2000 numbering */
	adb = ifeb;	// ALCT 2001 numbering
	hid = tmp_hist_offset + 100 + adb;
	sprintf( htitle, "ALCT Wire times: ADB %d", adb);
	HBOOK1( hid, htitle, 32, -0.5, 31.5, 0.0); 

	hid = tmp_hist_offset + 200 + adb;
	sprintf( htitle, "Wire cal: ADB %d", adb);
	HBOOK1( hid, htitle, 256, -0.5, 255.5, 0.0);
      }
    }

  hid = tmp_hist_offset + 300;
  sprintf( htitle, "Threshold vs Board number");
//  HBOOK1( hid, htitle, 24, -0.5, 23.5, 0.0); 
  HBOOK1( hid, htitle, nwires*3/8, -0.5, (float)(nwires*3/8)-0.5, 0.0); 

  sprintf( htitle, "Threshold for Board");
  HBOOK1( hid+1, htitle, 1200, -0.5, 119.5, 0.0);

  sprintf( htitle, "Noise vs Board number");
//  HBOOK1( hid+2, htitle, 24, -0.5, 23.5, 0.0); 
  HBOOK1( hid+2, htitle, nwires*3/8, -0.5, (float)(nwires*3/8)-0.5, 0.0); 

  sprintf( htitle, "Noise for Board ");
  HBOOK1( hid+3, htitle, 240, -0.5, 23.5, 0.0);

  sprintf( htitle, "Threshold vs Wire number");
//  HBOOK1( hid+4, htitle, 384, 0.5, 384.5, 0.0); 
  HBOOK1( hid+4, htitle, nwires*8, 0.5, nwires*8+0.5, 0.0); 

  sprintf( htitle, "Threshold for Wire");
  HBOOK1( hid+5, htitle, 1200, -0.5, 119.5, 0.0);

  sprintf( htitle, "Noise vs Wire number");
//  HBOOK1( hid+6, htitle, 384, 0.5, 384.5, 0.0); 
  HBOOK1( hid+6, htitle, nwires*8, 0.5, nwires*8+0.5, 0.0); 

  sprintf( htitle, "Noise for Wire");
  HBOOK1( hid+7, htitle, 240, -0.5, 23.5, 0.0);

  hid = tmp_hist_offset + 3000;
  sprintf( htitle, "Time for event");
  HBOOK1( hid, htitle, 10000, 0.5, 10000.5, 0.0);

  for (ilayer = 0; ilayer < NLAYER; ilayer++)
    {
    for (iwire = 0; iwire < nwires; iwire++)
      {
//      hid = tmp_hist_offset +1000+ 100*(ilayer+1) + iwire+1;
      hid = tmp_hist_offset +1000+ 120*(ilayer+1) + iwire+1;
      sprintf( htitle, "ALCT Layer %d Wire %d", ilayer+1,iwire+1);
      HBOOK1( hid, htitle, 32, 0.5, 32.5, 0.0);

//      hid = tmp_hist_offset + 2000 + 100 * (ilayer + 1) + (iwire + 1);
      hid = tmp_hist_offset + 2000 + 120 * (ilayer + 1) + (iwire + 1);
      sprintf( htitle, "Wire cal: Layer %d Wire %d ", ilayer+1,iwire+1 );
      HBOOK1( hid, htitle, 256, -0.5, 255.5, 0.0);
      }
    }

  return 0;
  }

/*------------------------------ cal_cap_init ---------------------------------*/
int cal_cap_init(void )        // read file with calibration capcacitances
{
  char* file_name;
  char tmp[160];
  FILE* fp_cap;
  int i, iparse;
//  int iread;
  int k1,k2,k3,k5;
  float f4,f6,f7; // ,f8,f9,f10,f11,f12;
  char* tmp_string;
  char parse_string[160];

  for (i=0; i<3*nwires/8; i++)
    {
      cal_cap[i]  = 0.25;
      cal_gain[i] = 1.00;
    }

  /* compute the filename with calibration capacitances */
  file_name = malloc(strlen(datadir_translation) +
                     strlen(FILE_CAPACITANCE) + 20);
  if (file_name == NULL)
    {
      printf("malloc() failed for file_name.\n");
      return -1;
    }

  /* Open result file in the temporary directory */
  sprintf(file_name,"%s%s%03d.txt", datadir_translation, FILE_CAPACITANCE,csc_num);
  if ( (fp_cap = fopen(file_name, "r")) == NULL)
    {
      printf (" Test_13 Can't open file with capacitance: %s\n", file_name);
      printf (" Test_13 continue analysis with all capacitance = 0.25 pF \n");
      return -1;
    }
  else
    {
      printf("The file %s was open succesfuly:\n", file_name);
      printf("Reading file with calibration capacitances:\n");
      for (i=0; i<3*nwires/8;)
        {
          tmp_string = fgets(tmp, sizeof(tmp), fp_cap);
          if (tmp_string!= NULL)
            {
              sscanf(tmp,"%2s",parse_string);
              if (parse_string[0] != '#')
                {
                  //              iparse=sscanf(tmp,"%d %d %d %d %d %f %f %f %f %f  %f %f",
                  //        &k1,&k2,&k3,&k4,&k5,&f6,&f7,&f8,&f9,&f10,&f11,&f12);
                  iparse=sscanf(tmp,"%d  %d  %d  %f %d %f %f",
				&k1,&k2,&k3,&f4,&k5,&f6,&f7);
                  if ((iparse == 7) && (k2<3*nwires/8+1) && (k2>0) )
                    {
                      printf ("Pos: %2d, afeb: %d,\t Ccal: %f \n",
			      k2, k3, f4);
                      cal_cap[k1-1] = f4;
                      cal_gain[k1-1] = f6;
                      i++;
                    }
                  else
                    printf("Not enough parameters in line, only %d\n", iparse);
                }
            }
          else {
            printf ("reach the EOF\n");
            break;
          }
        }
    }

  free(file_name);
  fclose(fp_cap);
  printf(" The calibration coefficients are:\n");
  for (i=0; i<3*nwires/8; i++)
    printf("AFEB: %d \tCin: %6.3f \t Gain: %8.3f\n", i+1, cal_cap[i], cal_gain[i]);
  return 0;
}
