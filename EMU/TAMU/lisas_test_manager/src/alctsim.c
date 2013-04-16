#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include "cfortran.h"
#include "hbook.h"
#include "daq_conf.h"
#include "event_ana.h"       /* Function prototypes    */
#include "csc_event.h"       /* upevt_                 */
#include "misc.h"            /* definition of logical variables */
#include "csc_parameters.h"  /* NLAYER, NSTRIP, etc... */
#include "test_utils.h"      /* chamber labels         */
#include "test_params.h"     /* test config parameters */
#include "one_page_plot.h"   /* STRIP_PLOT,...         */
#include "alct_params.h"         /* definition of alct_params_type structure */
//#include "alct_declarations.h"   /* declaration of read_afeb_config() */

#define   MAX_FIFO_SAMPLES     16
#define   NLCT                 NWIRES_MAX/16  


/*******************************************************************************
   Modified ALCT simulation subroutine used for offline wire track-finding
*******************************************************************************/

/*
 * Subroutine which takes ALCT raw hit data as input, and finds the expected
 * ALCT results.
 */

int alctsim(int pretrig_time, short int *accel, short int *key, 
 short int *patt, short int *valid, int nwires) {
  int              i, j, k, ilct, layer, mode, nhits_a, nhits_c;
  int              best_wg_a[2][NLCT], max_nhits_a[2][NLCT];
  int              best_wg_c[2][NLCT], max_nhits_c[2][NLCT];
  int              accel_mu[NLCT], nhits[NLCT], best_wg[NLCT];
  short int        active_lct, jm[3], jp[3];
  unsigned long    alct_s[NLAYER][NWIRES_MAX], data, *ldat, tbit;
  static unsigned long    mask[MAX_FIFO_SAMPLES];
  static logical    first = _TRUE;
  logical          debug_on, local_file;
  logical          pulse_on, pretrig_a, pretrig_c, pretrigger;

  static alct_params_type    p;

/* Initialize */
  valid[0] = 0;
  valid[1] = 0;
  debug_on = _FALSE;
  if (first) {
    first = _FALSE;
    local_file = _TRUE;
//    read_afeb_config(upevt_.afeb_config_id, local_file, -1, &p);
    for (k = 0; k < MAX_FIFO_SAMPLES; k++) mask[k] = 1 << k;

/* Override some config-file values */
//  p.num_fifo_before_pretrig = 4;
//  p.num_fifo_before_pretrig = 7;
    p.drift_delay = 2;
    p.alct_jtag_disable = 1;
    p.alct_accel_mu_mode = 1;
    p.nplanes_for_pretrig = 2;
    p.trigger_mode = 0;
    p.pattern_num_thr = 4;
    p.num_fifo_samples = 12;
  }

/* Convert (as well as possible) from raw data to 150-ns-pulse-stretched data */

  for (layer = 0; layer < NLAYER; layer++) {
    for (j = 0; j < nwires; j++) {
      alct_s[layer][j] = 0;

      /* Logic for one-shot that's retriggerable after input has gone to zero */
      if ((data = upevt_.alct_dump[layer][j])) {
        pulse_on = _FALSE;
        for (k = 0; k <  p.num_fifo_samples; k++) {
          if ((mask[k] & data) && !pulse_on) {
            for (i = k; i < k + 6; i++) alct_s[layer][j] |= mask[i];
            pulse_on = _TRUE;
          }
          else if (!(mask[k] & data)) pulse_on = _FALSE;
        }
//      printf("L%d wg%2d: alct_dump=%lx, alct_s=%lx\n", layer+1, j+1, 
//       upevt_.alct_dump[layer][j], alct_s[layer][j]);
      }
    }
  }

/* Do trigger logic twice, at pretrigger time and after the drift delay */
  for (i = 0; i < 2; i++) {
    if (i == 0) k = pretrig_time;
    else k = pretrig_time + p.drift_delay;

    if (k < 0 || k >= p.num_fifo_samples) {
      printf("Can't simulate trigger with these run parameters:\n");
      printf("Num fifo samples=%d, before pretrigger=%d, drift_delay=%d\n",
       p.num_fifo_samples, pretrig_time, p.drift_delay);
      return(-1);
    }

    tbit = 1 << k;

    for (ilct = 0; ilct < NLCT; ilct++) {
      max_nhits_a[i][ilct] = 0;
      max_nhits_c[i][ilct] = 0;
      best_wg_a[i][ilct] = 0;
      best_wg_c[i][ilct] = 0;
    }

    for (j = 0; j < nwires; j++) {
      nhits_a = 0;
      nhits_c = 0;
      ilct = j / 16;
      for (k = 0; k < 3; k++) {
        jm[k] = j - k;
        jp[k] = j + k;
        if (jm[k] < 0) jm[k] = 0;
        else if (jp[k] > nwires-1) jp[k] = nwires - 1;
      }

/* Collision and accelerator muon algorithms */
      for (layer = 0; layer < NLAYER; layer++) {
        ldat = alct_s[layer];
        if ((ldat[jm[0]]) & tbit) nhits_a++; 

        if (layer == 0) {
         if ((ldat[jp[0]] | ldat[jp[1]] | ldat[jp[2]]) & tbit) nhits_c++; 
        }
        else if (layer == 1) {
          if ((ldat[jp[0]] | ldat[jp[1]]) & tbit) nhits_c++; 
        }
        else if (layer == 2) {
          if ((ldat[jp[0]]) & tbit) nhits_c++; 
        }
        else if (layer == 3) {
          if ((ldat[jm[0]] | ldat[jm[1]]) & tbit) nhits_c++; 
        }
        else {
          if ((ldat[jm[0]] | ldat[jm[1]] | ldat[jm[2]]) & tbit) nhits_c++; 
        }
      }

/* Compare with previous best track in this LCT section (16 wires) */
      if (nhits_a >= max_nhits_a[i][ilct]) {
        best_wg_a[i][ilct] = j;
        max_nhits_a[i][ilct] = nhits_a;
      }

      if (nhits_c >= max_nhits_c[i][ilct]) {
        best_wg_c[i][ilct] = j;
        max_nhits_c[i][ilct] = nhits_c;
      }

/* Debug */
//    if (nhits_a > 0 || nhits_c > 0) printf("i=%d, wiregroup %2d: nhits_a=%d, "
//     "nhits_c=%d, max_a[%d]=%d, max_c[%d]=%d\n", i, j+1, nhits_a, nhits_c, 
//     ilct, max_nhits_a[i][ilct], ilct, max_nhits_c[i][ilct]);
    }
  }

/* Assign promotion bits */
  for (i = 0; i < 2; i++) {
    for (ilct = 0; ilct < NLCT; ilct++) {
      if (p.alct_jtag_disable == 1) mode = p.alct_accel_mu_mode;
      else mode = p.lct[ilct].accel_mu_mode;

      if (mode == 0) max_nhits_a[i][ilct] = 0;
      else if (mode == 1 && max_nhits_c[i][ilct] > 0) max_nhits_c[i][ilct] += 8;
      else if (mode == 2 && max_nhits_a[i][ilct] > 0) max_nhits_a[i][ilct] += 8;
      else if (mode == 3) max_nhits_c[i][ilct] = 0;
    }
  }

/* Apply the pretrigger threshold, and set active_lct bits (???)  */
  pretrig_a = _FALSE;
  pretrig_c = _FALSE;
  active_lct = 0;
  for (ilct = 0; ilct < NLCT; ilct++) {
    if ((max_nhits_a[0][ilct] > max_nhits_c[0][ilct]) && 
     ((max_nhits_a[0][ilct] & 0x7) >= p.nplanes_for_pretrig)) {
      pretrig_a = _TRUE;
      active_lct |= (1 << ilct);
    }
    if ((max_nhits_c[0][ilct] > max_nhits_a[0][ilct]) && 
     ((max_nhits_c[0][ilct] & 0x7) >= p.nplanes_for_pretrig)) {
      pretrig_c = _TRUE;
      active_lct |= (1 << ilct);
    }
  }
  if (p.trigger_mode == 0) pretrigger = pretrig_a || pretrig_c;
  else if (p.trigger_mode == 1) pretrigger = pretrig_a;
  else if (p.trigger_mode == 2) pretrigger = pretrig_c;
  else if (p.trigger_mode == 3) pretrigger = pretrig_c && !pretrig_a;
//printf("pretrig_a=%d, pretrig_c=%d, pretrigger=%d\n", pretrig_a, pretrig_c, 
// pretrigger);

/* For each LCT, choose between accelerator and collision muons */
  for (ilct = 0; ilct < NLCT; ilct++) {
    if (max_nhits_a[1][ilct] > max_nhits_c[1][ilct]) {
      nhits[ilct] = max_nhits_a[1][ilct];
      best_wg[ilct] = best_wg_a[1][ilct];
      accel_mu[ilct] = _TRUE;
    }
    else if (max_nhits_c[1][ilct] > max_nhits_a[1][ilct]) {
      nhits[ilct] = max_nhits_c[1][ilct];
      best_wg[ilct] = best_wg_c[1][ilct];
      accel_mu[ilct] = _FALSE;
    }
    else nhits[ilct] = 0;
  }

/* Simulate "resolver" which removes duplicate LCT's at feb boundaries */
    for (ilct = 0; ilct < NLCT-1; ilct++) {
      if ((nhits[ilct] > 0) && (nhits[ilct+1] > 0) && 
//     ((best_wg[ilct+1] - best_wg[ilct]) < 4)) {
       ((best_wg[ilct+1] - best_wg[ilct]) <= 2)) {
        if (nhits[ilct+1] >= nhits[ilct]) nhits[ilct] = 0;
        else nhits[ilct+1] = 0;
      }
    } 

/* Find the two best LCT's */
  for (i = 0; i < 2; i++) {
    patt[i] = 0;
    key[i] = 0;
    accel[i] = 0;

    for (ilct = 0; ilct < NLCT; ilct++) {
      if (nhits[ilct] >= patt[i] &&
       (i == 0 || best_wg[ilct] != key[0])) {
        if ((nhits[ilct] & 0x7) >= p.pattern_num_thr) {
          patt[i] = nhits[ilct];
          valid[i] = 1;
          key[i] = best_wg[ilct];
          accel[i] = accel_mu[ilct];
        }
      }
    }
  }

  return(0);
}
