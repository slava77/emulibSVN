#define FILE_RESULTS "/test_results/temp/test_19_"
#define NMAXSCAN             5 
#define NMAXTHRESH          40 
#define NMAXBUCKET          31
#define NCFEB_CHIP           6
#define NCFEB                5

// Injected pulse in fC is A0+A1*DAC_calibration_pulse
//#ifdef UCLA
//#define INJECT_PULSE_A0  8. 
//#define INJECT_PULSE_A1  10.5 
//#elseifdef UF
//#define INJECT_PULSE_A0  -11. 
//#define INJECT_PULSE_A1  10.5 
//#else
#define INJECT_PULSE_A0  0. 
#define INJECT_PULSE_A1  0.655 // for the new DAC 
//#endif

// #define SLOP_LIMITS_SCAN_00 { 0., 4., 0., 4., .8, 1.8 }
#define SLOP_LIMITS_SCAN_00 { 0., 5., 0., 5., 1.6, 3.6, nstrips }

#define DELTA_DAC_LIMITS_SCAN_00 {-50., 50., -50., 50.,  -30., 30., nstrips}
#define DELTA_DAC_LIMITS_SCAN_01 {-50., 50., -50., 50.,  -30., 30., nstrips}
#define DELTA_DAC_LIMITS_SCAN_02 {-10., 10., -10., 10.,  -4., 4., nstrips}
#define DELTA_DAC_LIMITS_SCAN_03 {-10., 10., -10., 10.,  -4., 4., nstrips}
#define DELTA_DAC_LIMITS_SCAN_04 {-10., 10., -10., 10.,  -4., 4., nstrips}
#define DELTA_DAC_LIMITS_SCAN_05 {-10., 10., -10., 10.,  -4., 4., nstrips}
#define DELTA_DAC_LIMITS_SCAN_06 {-10., 10., -10., 10.,  -4., 4., nstrips}
#define DELTA_DAC_LIMITS_SCAN_07 {-10., 10., -10., 10.,  -4., 4., nstrips}
#define DELTA_DAC_LIMITS_SCAN_08 {-10., 10., -10., 10.,  -4., 4., nstrips}
#define DELTA_DAC_LIMITS_SCAN_09 {-10., 10., -10., 10.,  -4., 4., nstrips}

#define THR_LIMITS_SCAN_00 {0., 100., 0., 100.,  10.,  60., nstrips}
#define THR_LIMITS_SCAN_01 {0., 200., 0., 200.,  40., 150., nstrips} // [5] changed to accomodate all types of chambers, was 120
#define THR_LIMITS_SCAN_02 {0., 300., 0., 300.,  70., 200., nstrips}
#define THR_LIMITS_SCAN_03 {0., 40., 0., 40., 15., 26., nstrips}
#define THR_LIMITS_SCAN_04 {0., 40., 0., 40., 15., 26., nstrips}
#define THR_LIMITS_SCAN_05 {0., 40., 0., 40., 15., 26., nstrips}
#define THR_LIMITS_SCAN_06 {0., 40., 0., 40., 15., 26., nstrips}
#define THR_LIMITS_SCAN_07 {0., 40., 0., 40., 15., 26., nstrips}
#define THR_LIMITS_SCAN_08 {0., 40., 0., 40., 15., 26., nstrips}
#define THR_LIMITS_SCAN_09 {0., 40., 0., 40., 15., 26., nstrips}

#define NOISE_LIMITS_SCAN_00 {0., 10., 0., 10.,  1., 6., nstrips}
#define NOISE_LIMITS_SCAN_01 {0., 10., 0., 10.,  1., 6., nstrips}
#define NOISE_LIMITS_SCAN_02 {0., 10., 0., 10.,  1., 6., nstrips}
#define NOISE_LIMITS_SCAN_03 {0.,  5., 0.,  5.,  .5, 2.5, nstrips}
#define NOISE_LIMITS_SCAN_04 {0.,  5., 0.,  5.,  .5, 2.5, nstrips}
#define NOISE_LIMITS_SCAN_05 {0.,  5., 0.,  5.,  .5, 2.5, nstrips}
#define NOISE_LIMITS_SCAN_06 {0.,  5., 0.,  5.,  .5, 2.5, nstrips}
#define NOISE_LIMITS_SCAN_07 {0.,  5., 0.,  5.,  .5, 2.5, nstrips}
#define NOISE_LIMITS_SCAN_08 {0.,  5., 0.,  5.,  .5, 2.5, nstrips}
#define NOISE_LIMITS_SCAN_09 {0.,  5., 0.,  5.,  .5, 2.5, nstrips}

#define THR_LOWER_LIMIT 1.
#define THR_UPPER_LIMIT 50.
#define NOISE_LOWER_LIMIT 0.1
#define NOISE_UPPER_LIMIT 10.

