#define FILE_CAPACITANCE "/anode_thresh/AFEB_"
#define FILE_RESULTS "/test_results/temp/test_13_"
#define NMAXSCAN            10 

// Injected pulse in fC is A0+A1*DAC_calibration_pulse
#define INJECT_PULSE_A0  4.75 
#define INJECT_PULSE_A1  1.15 

// Calibration pulse in mV is A0+A1*DAC_calibration_pulse
#define CALIBRATION_PULSE_A0  17.77 // adjusted with scope measurements, was 18 previously (Madorsky 3/6/07)
#define CALIBRATION_PULSE_A1  4.44  // adjusted with scope measurements, was 4.6 previously (Madorsky 3/6/07)
#define C_DEFAULT  0.25

#define OFF_LIMITS_20  {-12., 12., -12., 12.,  -8., 8., nwires}
#define THR_LIMITS_20  {-10., 80., -10., 80.,  -8., 30., nwires}

#define SLOP_LIMITS_SCAN_00 {.5, 2., .5, 2.,  .9, 1.9, nwires}

#define DELTA_DAC_LIMITS_SCAN_00 {-10., 10., -10., 10.,  -8., 8., nwires}
#define DELTA_DAC_LIMITS_SCAN_01 {-10., 10., -10., 10.,  -8., 8., nwires}
#define DELTA_DAC_LIMITS_SCAN_02 {-10., 10., -10., 10.,  -4., 4., nwires}
#define DELTA_DAC_LIMITS_SCAN_03 {-10., 10., -10., 10.,  -4., 4., nwires}
#define DELTA_DAC_LIMITS_SCAN_04 {-10., 10., -10., 10.,  -4., 4., nwires}
#define DELTA_DAC_LIMITS_SCAN_05 {-10., 10., -10., 10.,  -4., 4., nwires}
#define DELTA_DAC_LIMITS_SCAN_06 {-10., 10., -10., 10.,  -4., 4., nwires}
#define DELTA_DAC_LIMITS_SCAN_07 {-10., 10., -10., 10.,  -4., 4., nwires}
#define DELTA_DAC_LIMITS_SCAN_08 {-10., 10., -10., 10.,  -4., 4., nwires}
#define DELTA_DAC_LIMITS_SCAN_09 {-10., 10., -10., 10.,  -4., 4., nwires}

#define THR_LIMITS_SCAN_00 {0.,  80., 0.,  80., 3.5, 50., nwires}
#define THR_LIMITS_SCAN_01 {0., 100., 0., 100., 20., 80., nwires}
#define THR_LIMITS_SCAN_02 {0., 40., 0., 40., 15., 26., nwires}
#define THR_LIMITS_SCAN_03 {0., 40., 0., 40., 15., 26., nwires}
#define THR_LIMITS_SCAN_04 {0., 40., 0., 40., 15., 26., nwires}
#define THR_LIMITS_SCAN_05 {0., 40., 0., 40., 15., 26., nwires}
#define THR_LIMITS_SCAN_06 {0., 40., 0., 40., 15., 26., nwires}
#define THR_LIMITS_SCAN_07 {0., 40., 0., 40., 15., 26., nwires}
#define THR_LIMITS_SCAN_08 {0., 40., 0., 40., 15., 26., nwires}
#define THR_LIMITS_SCAN_09 {0., 40., 0., 40., 15., 26., nwires}

#define NOISE_LIMITS_SCAN_00 {0., 10., 0., 10.,  .5, 4., nwires}
#define NOISE_LIMITS_SCAN_01 {0., 10., 0., 10.,  .5, 4., nwires}
#define NOISE_LIMITS_SCAN_02 {0.,  5., 0.,  5.,  .5, 2.5, nwires}
#define NOISE_LIMITS_SCAN_03 {0.,  5., 0.,  5.,  .5, 2.5, nwires}
#define NOISE_LIMITS_SCAN_04 {0.,  5., 0.,  5.,  .5, 2.5, nwires}
#define NOISE_LIMITS_SCAN_05 {0.,  5., 0.,  5.,  .5, 2.5, nwires}
#define NOISE_LIMITS_SCAN_06 {0.,  5., 0.,  5.,  .5, 2.5, nwires}
#define NOISE_LIMITS_SCAN_07 {0.,  5., 0.,  5.,  .5, 2.5, nwires}
#define NOISE_LIMITS_SCAN_08 {0.,  5., 0.,  5.,  .5, 2.5, nwires}
#define NOISE_LIMITS_SCAN_09 {0.,  5., 0.,  5.,  .5, 2.5, nwires}

#define THR_LOWER_LIMIT 1.
#define THR_UPPER_LIMIT 50.
#define NOISE_LOWER_LIMIT 0.1
#define NOISE_UPPER_LIMIT 10.




