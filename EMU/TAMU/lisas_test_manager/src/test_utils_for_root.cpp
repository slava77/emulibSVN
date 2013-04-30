#include "test_utils_for_root.h"
#include <math.h>
#include <sys/stat.h>
#include "ana_version.h"
#include <sys/time.h>
#include "test_params.h"
extern char  number_of_run[80];

using namespace std;
using namespace boost;

int calc_peds_for_root(int mode, float *mean[NLAYER], float *rms[NLAYER], int nstrips, th1f_2dvector_t * histograms)
{
  int      hid, nx, ny, nwt, loc;
  int      ibin, ilayer, istrip, ilower, iupper, imax;
  float    * histo_content;
  float    xmi, xma, ymi, yma, xbin, xwidth;
  float    avg, wt_sum;
  double   sum, sum_sq;
  char     htitle[80];

  for (ilayer=0; ilayer < NLAYER; ilayer++) 
    {
      for (istrip=0; istrip < nstrips; istrip++) 
	{
	  shared_ptr<TH1F> current_histogram = (*histograms)[ilayer][istrip];

	  nx = current_histogram->GetNbinsX();
	  xmi = current_histogram->GetXaxis()->GetXmin();
	  xma = current_histogram->GetXaxis()->GetXmax();
	  xwidth = (xma - xmi) / nx;

	  /* this should be a call to getarray returning into histo_content */
	  histo_content = current_histogram->GetArray();

	  if (mode == 0) 
	    {
	      /* Find the bin with most entries */
	      imax = 0;
	      for (ibin = 0; ibin < nx; ibin++) 
		{
		  if (histo_content[ibin] > histo_content[imax]) imax = ibin;  
		}
   
	      /* If imax is not too close to one of the array boundaries, then
	       * take 10 bins above and below imax. We will use this range of
	       * bins to calculate the mean and standard deviation.
	       */
	      ilower = ((imax - 15) < 0) ? 0 : imax - 15;
	      iupper = ((imax + 15) >= nx) ? nx : imax + 15;
	    }
	  else 
	    {
	      ilower = 0;
	      iupper = nx;
	    }

	  /* Calculated the weighted mean */
	  sum    = 0;
	  sum_sq = 0;
	  wt_sum = 0;
	  xbin = xmi + (ilower + 0.5) * xwidth;

	  for (ibin = ilower; ibin < iupper; ibin++) 
	    {
	      sum    += xbin * histo_content[ibin];
	      sum_sq += xbin * xbin * histo_content[ibin];
	      wt_sum += histo_content[ibin];
	      xbin   += xwidth;
	    }

	  if (wt_sum != 0.0) 
	    {
	      avg = sum / wt_sum;
	      mean[ilayer][istrip] = avg;
	      rms[ilayer][istrip] = sqrt(sum_sq / wt_sum - avg * avg);
	    }
	  else 
	    {
	      mean[ilayer][istrip] = 0.0;
	      rms [ilayer][istrip] = 0.0;
	    }
	  //    printf("mean[%d][%d] = %f\n", ilayer, istrip, mean[ilayer][istrip]);

	}  /* end for istrip */
    }  /* end for ilayer */

  return 0;
}

/* Version 3 Wrapper for Fortran HIGZ plotting subroutine */
int one_page_plot_v3(int test_id, int plot_id, int plot_type, float *data, 
		     float *error, float *limits, char *cscname, char *plot_title, 
		     char *summary_title, int flag ) 
{
  int               cscname_length;
  int               filename_length, plot_title_length;
  int               summary_title_length, timestring_length;
  char              cscname_temp[20], *filename, timestring[50], run_date[50];
  char              *ptr;
  struct timeval    tnow;
  struct tm         lct;
  //------------------------

  char daq_version[20];
  char ana_version[20];
  char user_name[20];
  char site[20];
  int daq_version_length;
  int ana_version_length;
  int user_name_length;
  int site_length;
  int number_of_run_length;
  int run_date_length;
  int testname_length;
  struct stat stat1;
  static time_t time_change;

  char testnames[30][100] = {
    "Broken-wire test                     ",
    "HV Connectivity test                 ", 
    "Gas Leak test                        ",
    "Long-term HV test                    ",
    "Gas gain test(radioactive source)    ",
    "Assembly                             ",
    "Water leak test                      ",
    "HV-ON Current and Cabling test       ",
    "LV test                              ",
    "ALCT test                            ",
    "AFEB Noise test (noise pulses)       ",
    "AFEB Connectivity & Alive test       ",
    "AFEB Threshold and Analog Noise test ",
    "AFEB Time Delay verification         ",
    "CFEB Noise test                      ",
    "CFEB Connectivity and Alive test     ",
    "CFEB calibration                     ", 
    "Comparator noise tests               ",
    "Comparator threshold tests           ",
    "Comparator timing test               ",
    "Left/right Comparator tests          ",
    "Left/right Comparator Offset test    ",
    "Neighbor Comparator test             ",
    "Chamber gain map (with cosmics)      ",
    "ALCT self trigger on cosmics tests   ",
    "CLCT self-trigger on cosmics tests   ",
    "High-statistics cosmics test         ",
    "Performance vs HV test               ",
    "High rate test                       "
  };

  char * daq_ver = getenv("STEP_DAQ_VER");
  strcpy(daq_version, daq_ver == NULL ? "STEP" : daq_ver);
  sprintf(ana_version, "V%dR%d%s", 
	  ANA_VERSION, 
	  ANA_REVISION,
	  ANA_VERSION_MODIFIED ? "M" : "P" );

  char * this_user = getenv("USER");
  strcpy(user_name, this_user == NULL ? "unknown" : this_user);

  char * this_host = getenv("HOST");
  strcpy(site, this_host == NULL ? "unknown" : this_host);
  
  //------------------------
  gettimeofday(&tnow, NULL);
  lct = *localtime((time_t *)&tnow.tv_sec);
  sprintf(timestring, "ANA DATE\"J#  %02d/%02d/%04d %02d\"J#%02d\"J#%02d", 
	  lct.tm_mon+1, lct.tm_mday,
	  lct.tm_year+1900, lct.tm_hour, lct.tm_min, lct.tm_sec);
  //-------------------------

  stat(number_of_run,&stat1);
  time_change=stat1.st_ctime;

  lct = *localtime((time_t *)&time_change);
  sprintf(run_date,"RUN DATE\"J# %02d/%02d/%04d %02d\"J#%02d\"J#%02d",
	  lct.tm_mon+1,lct.tm_mday, lct.tm_year+1900,
	  lct.tm_hour,lct.tm_min,lct.tm_sec);  


  //--------------------------
  strcpy(cscname_temp, cscname);
  /* Change /'s to .'s */
  while ((ptr = strpbrk(cscname_temp,"/"))) *ptr = '.';  
  filename = (char*)malloc(strlen(datadir_translation) + 40);
  if(filename == NULL)
    {
      printf("malloc() failed for filename.\n");
      return 1;
    }
  sprintf(filename, "%s/test_results/temp/test_%02d_%02d.ps", 
	  datadir_translation, test_id, plot_id);
  cscname_length = strlen(cscname);
  timestring_length = strlen(timestring);
  filename_length = strlen(filename);
  plot_title_length = strlen(plot_title);
  summary_title_length = strlen(summary_title);

  //  printf("version: %s %s %s %s\n",daq_version,ana_version,user_name,site);
  daq_version_length=strlen(daq_version);
  ana_version_length=strlen(ana_version);
  user_name_length=strlen(user_name);
  site_length=strlen(site);
  number_of_run_length=strlen(number_of_run);
  run_date_length=strlen(run_date);
  testname_length=strlen(testnames[test_id-1]);

  one_page_plot_v3_(&test_id, data, error, &plot_type, limits, cscname, 
		    timestring, plot_title, summary_title, filename, 
		    daq_version, ana_version, user_name, site, 
		    number_of_run, run_date, testnames[test_id-1],
		    &flag, cscname_length, timestring_length, 
		    plot_title_length, summary_title_length, filename_length,
		    daq_version_length, ana_version_length, 
		    user_name_length, site_length, 
		    number_of_run_length, run_date_length, testname_length);
  free(filename);
  return 0;
}


