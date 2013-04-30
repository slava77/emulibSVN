#define NUM_CSC_TYPES   7
#define MAXBINS       200

#include "csc_parameters.h"
#include "TH1.h" // histograms
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<TH1F> th1f_ptr;
typedef std::vector<std::vector<th1f_ptr> > th1f_2dvector_t;

int calc_peds_for_root(int mode, float *mean[NLAYER], float *rms[NLAYER], int nstrips, th1f_2dvector_t * histograms);

int one_page_plot_v3(int test_id, int plot_id, int plot_type, float *data, 
 float *error, float *limits, char *cscname, char *plot_title, 
 char *summary_title, int iflag); 

extern "C" void one_page_plot_v3_(int *test_id, float *data, float *error, int *plot_type, 
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




