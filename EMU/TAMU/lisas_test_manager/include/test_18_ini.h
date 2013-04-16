#define CHAMB_LEN_COEF {2., 2., 2., 2., 2., 1., 2.} 

#define HIST_LAYER_LOW_LIMIT0        1.
#define HIST_LAYER_LOW_LIMIT1        1. //2.
#define HIST_LAYER_LOW_LIMIT2        1.
#define HIST_LAYER_LOW_LIMIT3        0.
#define HIST_LAYER_HIGH_LIMIT0      150.
#define HIST_LAYER_HIGH_LIMIT1      200.
#define HIST_LAYER_HIGH_LIMIT2      150.
#define HIST_LAYER_HIGH_LIMIT3      10.

#define HIST_SUMMARY_LOW_LIMIT0      1.
#define HIST_SUMMARY_LOW_LIMIT1      1.//2.
#define HIST_SUMMARY_LOW_LIMIT2      1.
#define HIST_SUMMARY_LOW_LIMIT3      0.
#define HIST_SUMMARY_HIGH_LIMIT0     150.
#define HIST_SUMMARY_HIGH_LIMIT1     200.
#define HIST_SUMMARY_HIGH_LIMIT2     150.
#define HIST_SUMMARY_HIGH_LIMIT3     1.

#define SINGLES_RATE_LOWER_LIMIT0   1. // corrected to accomodate all types of chambers. Was 5.
#define SINGLES_RATE_LOWER_LIMIT1   1. // corrected to accomodate all types of chambers. Was 5.
#define SINGLES_RATE_LOWER_LIMIT2   5.
#define SINGLES_RATE_LOWER_LIMIT3   -.001
#define SINGLES_RATE_UPPER_LIMIT0   100.
#define SINGLES_RATE_UPPER_LIMIT1   150.
#define SINGLES_RATE_UPPER_LIMIT2   100.
#define SINGLES_RATE_UPPER_LIMIT3   1.

// next definitions are for odd/even test-2,3 checks of bad channels
// for planes with hv=3600 limits [1.,100.]
// for planes without hv   limits [-.001,1.]  
#define SINGLES_HIGH_NOHV_LIMIT0   -1.
#define SINGLES_HIGH_NOHV_LIMIT1   -1.
#define SINGLES_HIGH_NOHV_LIMIT2    1.
#define SINGLES_HIGH_NOHV_LIMIT3    1.

#define SINGLES_HIGH_HV_LIMIT0    -1.
#define SINGLES_HIGH_HV_LIMIT1    -1.
#define SINGLES_HIGH_HV_LIMIT2   100.
#define SINGLES_HIGH_HV_LIMIT3   100.

#define SINGLES_LOW_NOHV_LIMIT0   99.
#define SINGLES_LOW_NOHV_LIMIT1   99.
#define SINGLES_LOW_NOHV_LIMIT2    -.001
#define SINGLES_LOW_NOHV_LIMIT3    -.001

#define SINGLES_LOW_HV_LIMIT0   -1.
#define SINGLES_LOW_HV_LIMIT1   -1.
#define SINGLES_LOW_HV_LIMIT2    1.
#define SINGLES_LOW_HV_LIMIT3    1.


