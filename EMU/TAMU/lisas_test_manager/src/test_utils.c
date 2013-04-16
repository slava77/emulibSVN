
/* 
 *  File: test_utils.c
 *
 *  Author: Lisa Gorn
 *  Created: 30 March 2000
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include "csc_parameters.h"
#include "test_utils.h"
#include "test_params.h"
#include "cfortran.h"
#include "hbook.h"
#include "daq_conf.h"
#include "csc_event.h"
#include "ana_version.h"

#include <sys/stat.h>

#define MAX_POINTS   16
extern char  number_of_run[80];

char  testnames[30][100]=
{
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
}
;



/* Linear fit */

void fitit(float *x, float *y, float *yerr, int npoints, float *emm, 
	   float *bee, float *chisq) 
  {
 
  int       j;
  double    denom, weight;
  double    sum, sumx, sumx2, sumy, sumxy;
  double    resid[MAX_POINTS], yfit[MAX_POINTS];
 
  sum = 0;
  sumx = 0;
  sumx2 = 0;
  sumy = 0;
  sumxy = 0;
	
  for (j = 0; j < npoints; j++) 
    {
    weight = 1 / (yerr[j] * yerr[j]);
    sum = sum + 1. * weight;
    sumx = sumx + x[j] * weight;
    sumx2 = sumx2 + x[j] * x[j] * weight;
    sumy = sumy + y[j] * weight;
    sumxy = sumxy + x[j] * y[j] * weight;
    }
 
  denom = sumx2 * sum - sumx * sumx;
  if (denom == 0) 
    {
    printf("### Denominator is zero in fit ###\n");
    *chisq = -999;
    return;
    }
 
  *emm = (sumxy * sum - sumx * sumy) / denom;
  *bee = (sumx2 * sumy - sumx * sumxy) / denom;
 
  *chisq = 0;
  for (j = 0; j < npoints; j++) 
    {
    yfit[j] = *emm * x[j] + *bee;
    resid[j] = y[j] - yfit[j];
    *chisq += (resid[j] / yerr[j]) * (resid[j] / yerr[j]);
    }
  
  return;
  }

/* Converts csc_num and csc_type to a label */
/* csc_type ranges from 1 to NUM_CSC_TYPES, csc_num from 1 to ? */

int make_name(char *cscname, int cscnum, int csctype) 
  {
  static char    csc_label[NUM_CSC_TYPES][8] 
    = {"ME1.2", "ME1.3", "ME2.1", "ME3.1", "ME234.2", "ME4.1"};
  csc_num = cscnum;
  csc_type = csctype;

  if (csc_type < 1 || csc_type > NUM_CSC_TYPES) return -1;
  else 
    {
    sprintf(csc_name, "%s.%03d", csc_label[csc_type-1], csc_num);
    strcpy(cscname, csc_name);  
    return 0;
    }
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
  filename = malloc(strlen(datadir_translation) + 40);
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

/****************************************************************************
 *  another variant  of function: max_string_25_   (to be called from Fortran)
 *  Takes "input" as input string, and copies it to 3 or fewer strings each
 *  less than 25 characters long. (There has to be a better way to do this.)
 ***************************************************************************/

void max_string_nn_(char *input, int *n, char *str1, char *str2, char *str3, 
		    int input_length, int length1, int length2, int length3) 
  {
  int     j, k;
  int param=40;  // attention : should be the same as in fortran + 1
  int spaces_pos[200]; 
  int lines_pos[10];
  int n1;
  int nlines;
  int last_notspace;

  printf("%d \n%s\n",input_length,input);
  k=0; // number of spaces

  strcpy(str1,"");
  strcpy(str2,"");
  strcpy(str3,"");

  for(j=0;j<input_length;j++)
    {
     
    if (*(input + j) == ' ') 
      {spaces_pos[k]=j; k++; }

       
    if (j!=0 && *(input + j-1) == ' ' &&  (*(input + j) == ' ') )
      {k--;spaces_pos[k-1]=j; printf("%d %d\n",k,j);}
    

    // try to comment out
    if (*(input + j) != ' ')last_notspace=j;
    if(j-last_notspace>2){k-=3; break;} 
 
    }

  // if( *(input+input_length-1) == ' ')  k=k-1;
  
  for(j=0;j<k;j++)
    { 
    printf("=%d\n",spaces_pos[j]);
    }
  
  nlines=0;
  n1=0;
  lines_pos[0]=0;
  for(j=0;j<k;j++)
    {

    if(j==0)
      n1+=spaces_pos[j];
    else 
      n1+=spaces_pos[j]-spaces_pos[j-1];

    if((n1+(spaces_pos[j+1]-spaces_pos[j]))>param)
      {
      n1=0;

      if(nlines==0)
	{
	strncpy(str1,input + lines_pos[0] , 
		spaces_pos[j]-lines_pos[0]); 
	lines_pos[1]=spaces_pos[j]+1;
	str1[lines_pos[1]-lines_pos[0]]= '\0';
	nlines++;
	}
      else if(nlines==1)
	{
	strncpy(str2,input + lines_pos[1] , 
		spaces_pos[j]-lines_pos[1]); 
	lines_pos[2]=spaces_pos[j]+1;
	str2[lines_pos[2]-lines_pos[1]]= '\0';
	nlines++;
	}
      else if(nlines==2)
	{
	strncpy(str3,input + lines_pos[2] , 
		spaces_pos[j]-lines_pos[2]); 
	lines_pos[3]=spaces_pos[j]+1;
	str3[lines_pos[3]-lines_pos[2]]= '\0';
	nlines++;
	}


      }

    if(j==k-1 && nlines==0)
      {
      strncpy(str1,input + lines_pos[0], 
	      spaces_pos[j]-lines_pos[0]);
      nlines=1;
      str1[spaces_pos[j]-lines_pos[0]]= '\0';
      }
    else if(j==k-1 && nlines==1)
      {
      strncpy(str2,input + lines_pos[1], 
	      spaces_pos[j]-lines_pos[1]);
      nlines=2;
      str2[spaces_pos[j]-lines_pos[1]]= '\0';
      }
    else if(j==k-1 && nlines==2)
      {
      strncpy(str3,input + lines_pos[2], 
	      spaces_pos[j]-lines_pos[2]);
      nlines=3;
      str3[spaces_pos[j]-lines_pos[2]]= '\0';
      }
    }


  *n=nlines;

  length1=strlen(str1);
  length2=strlen(str2);
  length3=strlen(str3);
  
  printf("lenghth=%d %d %d\n",length1,length2,length3);

  
  for(j=0;j<(param-length1-1);j++)
    {
    strcat(str1," ");   
    }

  for(j=0;j<(param-length2-1);j++)
    {
    strcat(str2," ");   
    }  

  for(j=0;j<(param-length3-1);j++)
    {
    strcat(str3," ");   
    }

  length1=strlen(str1);
  length2=strlen(str2);
  length3=strlen(str3);
  
  
  printf("%s\n %s\n %s\n",str1,str2,str3);
  return;
  }
/****************************************************************************
 *  function: max_string_25_   (to be called from Fortran)
 *  Takes "input" as input string, and copies it to 3 or fewer strings each
 *  less than (param) characters long. (There has to be a better way to do this.)
 ***************************************************************************/

void max_string_25_(char *input, int *n, char *str1, char *str2, char *str3, 
		    int input_length, int length1, int length2, int length3) 
  {
  int     j, k;
  char    *p1, *p2, *p3;
  int param=39; // should correspond to fortarn stuff
 
  p1 = input;
  *n = 1;

  printf("%d\n",input_length);

/* Look for a break before param characters */
  if ((length1 = input_length) >param) 
    {
    for (k =param; k > 0; k--) if (*(p1 + k) == ' ') break;

    if (k > 0) length1 = k;
    else length1 = param;

    p2 = p1 + length1 + 1;
    *n = *n + 1;

/* Look for a break in the next param characters */
    if ((length2 = strlen(p2)) > param) 
      {
      for (k = param; k > 0; k--) if (*(p2 + k) == ' ') break;
  
      if (k > 0) length2 = k;
      else length2 = param;

      p3 = p2 + length2 + 1;
      *n = *n + 1;

/* Find length of final string */
      if ((length3 = strlen(p3)) > param) length3 = param;
      }
    }

/* Copy the strings */
  strncpy(str1, p1, length1);
  for (j = length1; j < param; j++) *(str1 + j) = ' ';

  if (*n > 1) 
    {
    strncpy(str2, p2, length2);
    for (j = length2; j < param; j++) *(str2 + j) = ' ';
    }

  if (*n > 2) 
    {
    strncpy(str3, p3, length3);
    for (j = length3; j < param; j++) *(str3 + j) = ' ';
    }

  return;
  }

/*******************************************************************************
   Function to calculate pedestals from histograms
   (slightly modified version of David's routine used by the event_analysis)
*******************************************************************************/

int calc_peds_(int mode, float mean[NLAYER][NSTRIP], float rms[NLAYER][NSTRIP])
{
    int      hid, nx, ny, nwt, loc;
    int      ibin, ilayer, istrip, ilower, iupper, imax;
    float    histo_content[MAXBINS];
    float    xmi, xma, ymi, yma, xbin, xwidth;
    float    avg, wt_sum;
    double   sum, sum_sq;
    char     htitle[80];

    for (ilayer=0; ilayer < NLAYER; ilayer++) 
    {
	for (istrip=0; istrip < NSTRIP; istrip++) 
	{
	    hid = 100*(ilayer + 1) + (istrip + 1);

	    HGIVE(hid, htitle, nx, xmi, xma, ny, ymi, yma, nwt, loc);
	    xwidth = (xma - xmi) / nx;

	    HUNPAK(hid, histo_content, "HIST", 0);

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

int calc_peds(int mode, float *mean[NLAYER], float *rms[NLAYER], int nstrips)
{
    int      hid, nx, ny, nwt, loc;
    int      ibin, ilayer, istrip, ilower, iupper, imax;
    float    histo_content[MAXBINS];
    float    xmi, xma, ymi, yma, xbin, xwidth;
    float    avg, wt_sum;
    double   sum, sum_sq;
    char     htitle[80];

    for (ilayer=0; ilayer < NLAYER; ilayer++) 
    {
	for (istrip=0; istrip < nstrips; istrip++) 
	{
	    hid = 100*(ilayer + 1) + (istrip + 1);

	    HGIVE(hid, htitle, nx, xmi, xma, ny, ymi, yma, nwt, loc);
	    xwidth = (xma - xmi) / nx;

	    HUNPAK(hid, histo_content, "HIST", 0);

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

//===============================================================================


int tm_create_header(int testnum, int reserved)
  { // header_propagation
  
  char *fname;
  char test_res_files[100];  

  char daq_version[20];
  char anal_version[20];
  char user_name[20];
  char site[20];  
  struct timeval    tnow;
  char              timestring[40];
  struct tm         lct;

  FILE *fp;

  fname = malloc(strlen(datadir_translation) + 40);
  if (fname == NULL)
    {
    printf("malloc() failed for fname.\n");
    return 1;
    }
  sprintf(fname, "%s/test_results/temp/test_%02d.header",
	  datadir_translation,testnum); 
  sprintf(test_res_files,"test_%02d_*.results",testnum);

  sprintf(daq_version, "%d.%d", upevt_.version, upevt_.revision );
  sprintf(anal_version, "%d.%d", ANA_VERSION, ANA_REVISION );
  strcpy(user_name, upevt_.user_name);
  strcpy(site, upevt_.site);

  //------------------------

  gettimeofday(&tnow, NULL);
  lct = *localtime((time_t *)&tnow.tv_sec);
  sprintf(timestring, "%d/%d/%d %2d\"J#%02d\"J#%02d", 
	  lct.tm_mon+1, lct.tm_mday,
	  lct.tm_year+1900, lct.tm_hour, lct.tm_min, lct.tm_sec);

  //------------------------

  printf("Test %d %s \n",testnum,testnames[testnum-1]);
  printf("test results files files: %s \n",test_res_files);
  printf("%s\n",csc_name);
  printf("Test performed: %s %s %s %s DAQ-%s\n",
	 timestring,site,user_name,number_of_run,daq_version);
  printf("Analysis done:  %s Analysis-%s\n",timestring,anal_version);
  printf("OK - \n");
  printf("FAILED - \n");

  fp = fopen(fname, "w");
  if(fp == NULL)
    {
      free(fname);
      printf("%s could not be opened for header.\n",fname);
      return 1;
    }
  free(fname);

  fprintf(fp,"Test %d %s \n",testnum,testnames[testnum-1]);
  fprintf(fp,"test results files files: %s \n",test_res_files);
  fprintf(fp,"%s\n",csc_name);
  fprintf(fp,"Test performed: %s %s %s %s DAQ-%s\n",
	  timestring,site,user_name,number_of_run,daq_version);
  fprintf(fp,"Analysis done:  %s Analysis-%s\n",timestring,anal_version);
  fprintf(fp,"OK - \n");
  fprintf(fp,"FAILED - \n");

  fclose(fp);

  return 1;
  }

static char csc_data_default[]="/csc_data/test_000000.dat";

char *get_most_recent()
  /* this routine fetches the most recent version of what it finds
     in $CSC_DIR 
     It will always try to return something, but if it fails, then
     it will gracefully return NULL. 
  */
  {
  char *csc_dir_translation;
  char *found_name;
  DIR *dp;
  struct dirent *ep;
  long latest_version, this_version;
  char work[20];
  
  csc_dir_translation = getenv("CSC_DIR");
  if (csc_dir_translation == NULL)
    {
    printf("The CSC_DIR environment variable could not be translated,\n");
    printf(" assuming a default value.\n");
    found_name = malloc(sizeof(csc_data_default));
    if (found_name == NULL)
      {
      printf("malloc() failed for found_name.\n");
      return NULL;
      }
    strcpy(found_name,csc_data_default);
    wait_for_return();
    return found_name;
    }

  /* scan the directory for examples of test_XXXXXX.dat, and key the biggest
     one. */
  dp = opendir(csc_dir_translation);
  if (dp != NULL)
    {
    latest_version = -1;
    while ((ep = readdir(dp)))
      {
      if (strlen(ep->d_name) == 15) /* 15 is the size of "test_XXXXXX.dat" */
	{
	/* see if the first 5 are test_ */
	strcpy(work,ep->d_name);
	work[5] = 0;
	if (0 == strcmp(work,"test_"))
	  {
	  /* see if the last 4 are .dat */
	  if (0 == strcmp(work + 11,".dat"))
	    {
	    /* make sure the 6 left over are numeric */
	    strcpy(work,ep->d_name);
	    work[11] = 0;
	    if (1 == sscanf(work + 5, "%ld", &this_version))
	      {
	      /* ok we got one, let's compare it */
	      latest_version = latest_version > this_version ? 
		latest_version : this_version;
	      }
	    }
	  }
	}
      }
    (void)closedir(dp);
    /* if latest_version is -1 then we found nothing. */
    if (latest_version == -1)
      {
      printf("No data files found in %s,\n",csc_dir_translation);
      printf(" assuming a default value.\n");
      found_name = malloc(sizeof(csc_data_default));
      if (found_name == NULL)
	{
	printf("malloc() failed for found_name.\n");
	return NULL;
	}
      strcpy(found_name,csc_data_default);
      wait_for_return();
      return found_name;
      }
    /* ok we have latest_version, so create a place for it and 
       return */
    found_name = malloc(sizeof(csc_dir_translation) + 20);
    if (found_name == NULL)
      {
      printf("malloc() failed for found_name.\n");
      return NULL;
      }
    sprintf(found_name,"%s/test_%6.6ld.dat",csc_data_default,latest_version);
    return found_name;
    }
  else
    {
    printf("Couldn't open directory %s.\n",csc_dir_translation);
    printf("Be sure that the environment variable CSC_DIR points to the\n");
    printf("right place.\n");
    found_name = malloc(sizeof(csc_data_default));
    if (found_name == NULL)
      {
      printf("malloc() failed for found_name.\n");
      return NULL;
      }
    strcpy(found_name,csc_data_default);
    wait_for_return();
    return found_name;
    }
  }

/*
  AUTHOR: Alexei A Drozdetski, drozdetski@phys.ufl.edu, CMS EMU Group
  Creation date: 05/01/02
  Last change: 05/20/02

SUBROUTINE OUTPUT:
OK:  return value = 0;  chisq = 0.0
WARNINGS: return value = 1; chisq = 1.0; 
        //warning: there is more than zero hits in the last bin OR:
	//all bins - with max value of hits...

ERRORS: return value = -1;
        chisq = -1.0; //error: no events with number of hits more than 0 
	              //in any bin 
	chisq = -3.0; //error: all_diff_events_sum<=0 here 
	              //(first iteration)... it's impossible!:-)
                      //show me corresponding histogram, please.
	chisq = -4.0; //error: sigma<0 here (first iteration)... 
	              //it's impossible! :-) 
                      //show me corresponding histogram, please.
        chisq = -5.0; //error: all_diff_events_sum < 0 here... 
	              // (second iteration), very strange 
        chisq = -6.0; //error: sigma<0 here... (second iteration), very strange 
*/

#define MAX_CALIB_POINTS 256

int calc_thresh(int npoints, int* content, 
		float* errors, float* par, float* chisq)
{

// Local variables initialization...
 int i,first_index;
 float diff_content[MAX_CALIB_POINTS], all_diff_events_sum;
 float mean, sigma;
 double left_bound, right_bound;
 int array_not_empty,array_not_full;
 float array_max_value;
 int warning_flag=0;

// Initialization of the threshold parameter array to zero values.
 par[0] = 0.;
 par[1] = 0.;
 par[2] = 0.;

 if (content[npoints-1] != 0.0)
   {
   content[npoints-1] = 0.0;
   *chisq = 1.0; //warning: there is more than zero hits in the last bin
   warning_flag=1;
   }

 array_not_empty = 0;
 array_not_full = 0;
 array_max_value = 0.0;
 for (i=0;i<npoints;i++)
   {
   if (content[i] != 0.0) 
     {
     array_not_empty = 1;
     }
   if(content[i] > array_max_value)
     {
     array_max_value = content[i];
     }
   }
 for (i=0;i<npoints;i++)
   {
   if (content[i] != array_max_value) 
     {
     array_not_full = 1;
     }
   }
 if (array_not_empty == 0)
   {
   *chisq = -1.0;//error: no events with number of hits more than 0 in any bin
   return -1;
   }

 first_index = npoints-1;

 //looking for the first (left) "good" index of the data array
 for (i=(npoints-1); i>-0.1; i--)
   {
   if(content[first_index] <= content[i])
     {
     first_index = i;
     }
   } 

// Here we starting to calculate MEAN and SIGMA for CONTENT array using all data,
// all NPOINTS bins. It is th first itteration...

// initialization of the local variables
 mean = 0.;
 sigma = 0.;
 all_diff_events_sum = 0.;

// beginning of the MEAN loop calculation...
 for (i=first_index; i<(npoints-1); i++)
   {
//   printf("content   i= %d  data= %d  error= %f \n",i,content[i],errors[i]);
   diff_content[i] = -(content[i+1]-content[i]);
   mean = mean + i*diff_content[i];
   all_diff_events_sum = all_diff_events_sum + diff_content[i];
   } 


// checking the all_diff_events_sum value. For all 'good' CONTENT arrays
// its value should be equal to max_events_per_bin_of_the_CONTENT_array
// for the first itteration and positive value <= max_events_per_...
// for the second itteration...
 if (all_diff_events_sum <= 0) 
   {
   *chisq = -3.; //error: all_diff_events_sum<=0 here... it's impossible! :-) 
                 //show me corresponding histogram, please.
   return -1;
   }
// ... '+ 0.5' bins shift is neccessary just by obvious features of our algorithm
 mean = mean/all_diff_events_sum + 0.5;

// beginning of the SIGMA loop calculation...
 for (i=first_index; i<(npoints-1); i++)
   {
//   printf("diff content   i= %d  data= %f \n",i,diff_content[i]); 
   sigma = sigma + (i-mean)*(i-mean)*diff_content[i];
   } 
 if (sigma < 0.0) 
   {
   *chisq = -4.; //error: sigma<0 here... it's impossible! :-) 
                 //show me corresponding histogram, please.
   return -1;
   }
 sigma = sqrt(sigma/all_diff_events_sum);
// printf("all_diff_events_sum = %f sigma= %f\n",all_diff_events_sum, sigma);

// Here we starting to calculate MEAN and SIGMA for CONTENT array using all data,
// all NPOINTS bins. It is the second itteration...

// calculation of the bounds for MEAN and SIGMA calculation
// in the second itteration. We do it in order to recalculate MEAN and SIGMA
// without taking into account far away lieing noise peaks...
 if (sigma < 2 ) sigma = 2.;
 if (mean - 3*sigma < 0) 
   {
   left_bound = 0;
   }
 else 
   {
   left_bound = floor(mean - 3*sigma);
   }
 if (mean + 3*sigma > npoints) 
   {
   right_bound = npoints;
   }
 else 
   {
   right_bound = ceil(mean + 3*sigma); 
   }
// printf("bounds: %f %f \n",(mean - 3*sigma),(mean + 3*sigma));
// printf("bounds: %f %f \n",left_bound,right_bound); 

// initialization of the local variables...
 all_diff_events_sum = 0.;
 mean = 0.;
 sigma = 0.;

// FOLLOWING STRUCTURE OF THE mean AND sigma CALCULATION
// IS THE SAME AS IN THE PREVIOUS HALF OF THE SUBROUTINE.
// SO, WE WILL NOT REPEAT SOME NOTES HERE AGAIN.

 //AGAIN: looking for the first (left) "good" index of the data array
 for (i=(right_bound-1); i>left_bound-0.1; i--)
 {
   if(content[first_index] <= content[i])
     {
       first_index = i;
     }
 } 


// beginning of the MEAN loop calculation...
 for (i=first_index; i<(right_bound-1); i++)
   {
     diff_content[i] = -(content[i+1]-content[i]);
     mean = mean + i*diff_content[i];
     all_diff_events_sum = all_diff_events_sum + diff_content[i];
   } 
 mean = mean/all_diff_events_sum + 0.5;
 par[0] = all_diff_events_sum;
 par[1] = mean;

 if (all_diff_events_sum <= 0) 
   {
     *chisq = -5.;//error: all_diff_events_sum < 0 here...
     return -1;
   }

// beginning of the SIGMA loop calculation...
 for (i=first_index; i<(right_bound-1); i++)
   {
     sigma = sigma + (i-mean)*(i-mean)*diff_content[i];
   } 
 if (sigma < 0.0) 
   {
     *chisq = -6.; //error: sigma<0 here...
     return -1;
   }
 sigma = sqrt(sigma/all_diff_events_sum);
 par[2] = sigma;
 
// calculation process finished...
 if (warning_flag==1)
   {
     return 1;
   }
 else if (warning_flag==0)
   {
     *chisq = 0.0;
     return 0;
   }
 return 0;
}


time_t seconds_since_1900(void)
  {
    //  time_t       ttt;
  struct tm    ts;

/* Convert time info unpacked from the event header back to seconds since 
 * Jan 1, 1900, which is called "calendar time" in Unix. The time info is 
 * originally obtained as calendar time by the acquire_data function and 
 * passed to the buf_handler (in time_info.h), which converts it to YMDHMS 
 * format and puts it into the event header.
 */
  ts.tm_sec   =  (upevt_.time_MMSS & 0xff);        /* 0 - 61 */
  ts.tm_min   =  (upevt_.time_MMSS >> 8) & 0xff;   /* 0 - 59 */
  ts.tm_hour  =  (upevt_.time_DDHH & 0xff);        /* 0 - 23 */
  ts.tm_mday  =  (upevt_.time_DDHH >> 8) & 0xff;   /* range 1 - 31 */
  ts.tm_mon   =  (upevt_.time_YYMM & 0xff) - 1;    /* range 0 - 11 */
  ts.tm_year  =  (upevt_.time_YYMM >> 8) & 0xff;   /* since 1900 */
  ts.tm_isdst =  -1;                               /* < 0 since DST unknown */

  return mktime(&ts);

  } 





































