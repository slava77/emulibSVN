/* This is test_utils.h */

#define NUM_CSC_TYPES   8
#define MAXBINS       200
#define MAX_TEST_NUM   29

#include "csc_parameters.h"

#ifdef __cplusplus
extern "C" {
#endif

  int getint(int *n);

  const char* gettestname(int itest);
  void fitit(float *x, float *y, float *yerr, int npoints, float *emm,
	     float *bee, float *chisq);
  
  int make_name(char *csc_name, int csc_num, int csc_type);
  
  int one_page_plot_v3(int test_id, int plot_id, int plot_type, float *data, 
		       float *error, float *limits, char *cscname, char *plot_title, 
		       char *summary_title, int iflag); 
  
  /*
    void one_page_plot_v3_(int *test_id, float *data, float *error, int *plot_type, 
    float *limits, char *cscname, char *timestring, char *plot_title, 
    char *summary_title, char *filename, 
    char *daq_version, char *anal_version, char *user_name, char *site,
    int *iflag, int cscname_length, 
    int timestring_length, int plot_title_length, int summary_title_length, 
    int filename_length, 
    int daq_version_length,  int anal_version_length, int user_name_length, int site_length);
  */
  
  void one_page_plot_v3_(int *test_id, float *data, float *error, int *plot_type, 
			 float *limits, char *cscname, char *timestring, char *plot_title, 
			 char *summary_title, char *filename, 
			 char *daq_version, char *anal_version, char *user_name, char *site, 
			 char *number_of_run, char *run_date, char *testnames, 
			 int *iflag, int cscname_length, 
			 int timestring_length, int plot_title_length, int summary_title_length, 
			 int filename_length, 
			 int daq_version_length,  int anal_version_length, int user_name_length, 
			 int site_length, 
			 int number_of_run_length, int run_date_length, int testname_length);
  
  int calc_peds_(int mode, float mean[NLAYER][NSTRIP], float rms[NLAYER][NSTRIP]);
  int calc_peds (int mode, float *mean[NLAYER], float *rms[NLAYER], int nstrips);
  
  void dops_(float *limits,char *cscname, char *title);
  
  void fit_erf_(int *npoints, int *data, float *errors, float *par, float *chisq);
  
  void init_display_(void);
  
  void wait_for_return(void);
  
  int tm_create_header(int testnum, int reserved); // header_propagation
  
  char* get_most_recent();
  
  time_t seconds_since_1900(void);
/* Convert time info unpacked from the event header back to seconds since 
 * Jan 1, 1900, which is called "calendar time" in Unix. The time info is 
 * originally obtained as calendar time by the acquire_data function and 
 * passed to the buf_handler (in time_info.h), which converts it to YMDHMS 
 * format and puts it into the event header.
 */

/*
 
  * calc_thresh(...) - it's the program that calculates the MEAN value 
  and the RMS of the MEAN value for particular 1-dimensional  array 
  CONTENT of data with NPOINTS number of bins in it. 

  NOT USED VARIABLES: 1-dimensional array ERRORS - was included in order 
  not to break down old structure of the calling of fit_thresh() function
  in test 13. I not see the reason to save it in the calc_thresh() function
  and suppose to delete it in the next version.
  
   OUTPUT VARIABLES:
  -- one dimensional array par (before was used to input initial parameters 
  for errf_fit, but here it is used only for output). par[1] = MEAN, 
  par[2] = SIGMA, par[3] = 3 (not used, just from the old architecture of the
  test_13.c file).
  -- chisq - is used for diagnostic output:
         1 - 'there is no errors'
	 2 - 'bad "all_diff_events_sum" value on the first itteration' 
	     (see comments in the subruotine) simply it means that the array
	     CONTENT of threshold data is terrible :-) and cann't actualy be
	     threshold data
	 3 - 'bad "all_diff_events_sum" value on the second itteration' 
	     (see comments in the subruotine) simply it means that the array
	     CONTENT of threshold data is very strange. It's hard to imagine
	     that it can actualy be threshold data
  
  FUNCTION OUTPUT: if there weren't errors during calculation, function 
  returns 1, if there were some then 0 (in this case - see the value of
  the chisq variable and the description of its possible values). 
  By the way, in principle we can change 'int calc_thresh()' to
  'void calc_thresh()', because we can do all the analysis of the data 
  using only chisq values. 
  

  AUTHOR: Alexei A Drozdetski, drozdetski@phys.ufl.edu, CMS EMU Group
  Creation date: 05/01/02
  Last change: 05/02/02

  P.S. ALL COMMENTS AND SUGGESTIONS ARE WELCOME TO ABOVE E-MAIL OR BY
  PERSONAL CONTACT. ALL CHANGES IN THIS CODE CAN BE DONE ONLY BY ME.
  I'M RESPONSIBLE FOR MY PROGRAMS AND DON'T WANT TO FIND YOUR BUGS.
  IF YOU KNOW MORE AND BETTER, JUST SHARE YOR KNOWLEDGE WITH ME.
  AGAIN, I'LL BE VERY APPRICIATED FOR ALL YOU ADVISES, HELP AND 
  SUGGESTIONS. 
  errare humanum est. :-)

  Thank you.

 */

int calc_thresh(int npoints, int* content, float* errors, float* par, float* chisq);


#ifdef __cplusplus
}
#endif







