/*
 *  This file is alct_params.h
 *  Created April 4, 2000 (Lisa Gorn)
 */

#ifndef alct_params_h_
#define alct_params_h_

#define DEFAULT_DEVICE              3   /* 1 at FNAL, 3 at UF */
#define DEFAULT_AFEB_CONFIG_NUM     1

#define NUM_LCT_CHIPS    4
#define NUM_AFEB         6 * NUM_LCT_CHIPS
#define NRUN             4
/* #define ALCT_HEADER_WC  10 */

/* Types of ALCT software triggers */
#define EXT_TRIGGER      1
#define PATTERN_INJECT   2
#define TEST_PULSE       3

/* Note that day, month and year are BCD, ie this year is 0x2000, not 0x7d0 */
typedef struct {
  unsigned char    chip;
  unsigned char    version;
  unsigned char    day;
  unsigned char    month;
  short int        year;
} id_reg_type;

typedef struct {
  id_reg_type              idreg;
  unsigned char            lct_jtag_disable;
  unsigned char            test_pat_mode;
  unsigned char            accel_mu_mode;
  unsigned char            mask_all;
  unsigned char            unassigned;
  short int                hot_chan_mask[6];
  short int                test_pattern[6];
} lct_type;

typedef struct {
  id_reg_type               sc_idreg;
  short int                 test_pulse_amp;
  unsigned char             threshold_amp[NUM_AFEB];
  unsigned char             delay[NUM_AFEB];
  unsigned char             test_pulse_group_mask;
  unsigned char             test_pulse_strip_mask;
  unsigned char             standby_mask[NUM_LCT_CHIPS];
  unsigned char             test_pulse_power_up;

  id_reg_type               conc_idreg;
  unsigned char             trigger_mode;
  unsigned char             external_trig_enable;
  unsigned char             pretrigger_and_halt;
  unsigned char             inject_test_pattern;
  unsigned char             inject_mode;
  unsigned char             inj_lct_chip_mask;
  unsigned char             nplanes_for_pretrig;
  unsigned char             pattern_num_thr;
  unsigned char             drift_delay;
  unsigned char             num_fifo_samples;
  unsigned char             num_fifo_before_pretrig;
  unsigned char             fifo_mode;
  unsigned char             fifo_last_feb;
  unsigned char             l1acc_delay;
  unsigned char             l1acc_window_width;
  unsigned char             l1acc_counter_offset;
  unsigned char             internal_l1acc;
  unsigned char             board_id;
  unsigned char             bx_counter_offset;
  unsigned char             ccb_enable;
  unsigned char             alct_jtag_disable;
  unsigned char             alct_test_pat_mode;
  unsigned char             alct_accel_mu_mode;
  unsigned char             alct_mask_all;
  unsigned char             config_spare;
  unsigned char             trig;
  unsigned char             csc_id;

  lct_type                  lct[NUM_LCT_CHIPS];
} alct_params_type;

#endif           /* included already */
