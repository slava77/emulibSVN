
/*******************************************************************************
 * File: clctsim.c
 ******************************************************************************/

#include <stdio.h>
#include "csc_parameters.h"
#include "misc.h"
#include "clctsim.h"

#define NHALFSTRIP           (2 * NSTRIP)
#define NDISTRIP             (NSTRIP / 2)
#define MAX_FIFO_SAMPLES     32
#define NLCT                 5
#define NCODES               39

/* Global variables */
static unsigned long    tpattern[NCODES];
static unsigned long    clct_s[NLAYER][NHALFSTRIP];
static unsigned long    clct_d[NLAYER][NHALFSTRIP/4];
static int              nplanes_for_pretrig, pattern_num_thr;

/*
 * Function: clctsim
 * Modified CLCT simulation subroutine used for offline strip track-finding
 * Takes CLCT raw hit data as input, and finds the expected CLCT results.
 */

int clctsim(unsigned int comphits[NLAYER][NHALFSTRIP], int *trigbin_sca, 
 int pt_alg, int t_alg, short int *key, short int *pattern, short int *valid) {
  int                     i, j, k, layer;
  int                     pretrigger_time;
  static int              num_fifo_samples;
  static int              num_fifo_before_pretrig, drift_delay;
  short int               active_lct;
  static unsigned long    mask[MAX_FIFO_SAMPLES];
  logical                 pulse_on, pretrigger;
  static logical          first = { _TRUE };

/* Initialize */
  for (i = 0; i < 2; i++) {
    key[i] = 0;
    pattern[i] = 0;
    valid[i] = 0;
  }

  if (first) {
    first = _FALSE;
    for (k = 0; k < MAX_FIFO_SAMPLES; k++) mask[k] = 1 << k;
    get_tpatterns();

    num_fifo_samples = 16;
    num_fifo_before_pretrig = 1;
    drift_delay = 3;
    nplanes_for_pretrig = 2;
    pattern_num_thr = 4;
  }

/* Convert (as well as possible) from raw data to 150-ns-pulse-stretched data */
/* Logic is for one-shot that's retriggerable after input has gone to zero. */

  for (layer = 0; layer < NLAYER; layer++) {
    for (j = 0; j < NHALFSTRIP; j++) {
      clct_s[layer][j] = 0;

      if (comphits[layer][j] != 0) {
        pulse_on = _FALSE;
        for (k = 0; k <  num_fifo_samples; k++) {
          if ((mask[k] & comphits[layer][j]) && !pulse_on) {
            for (i = k; i < k + 6; i++) clct_s[layer][j] |= mask[i];
            pulse_on = _TRUE;
          }
          else if (!(mask[k] & comphits[layer][j])) pulse_on = _FALSE;
        }
      }
    }
  }

/* Look for a pretrigger */
  pretrigger = _FALSE;
  pretrigger_time = -1;
  *trigbin_sca = 0;
  for (k = 0; k < 32; k++) {
    if ((pretrigger = pretrig_alg(pt_alg, k, &active_lct))) break;
  }

/* Look for a trigger */
  if (pretrigger) {
    pretrigger_time = k;
    k += drift_delay;
    trig_alg(t_alg, active_lct, k, key, pattern, valid);

    if (valid[0]) *trigbin_sca = k / 2;
  }

/* Debug */ /*
  if (pretrigger) {
    printf("clctsim: pretrig_time=%d, SCA peak=%d, active_lct=%d%d%d%d%d\n", 
     pretrigger_time, *trigbin_sca, (active_lct>>4)&1, (active_lct>>3)&1, 
     (active_lct>>2)&1, (active_lct>>1)&1, active_lct&1);
    k = 0;
    for (i = 0; i < 2; i++) {
      printf("clctsim: LCT%d: key=%3d, pattern=%d, valid=%d\n", 
       i, key[i], pattern[i], valid[i]);
      if (valid[i]) {
        if (hs(pattern[i])) k |= (1 << (key[i] / 32));
        else k |= (1 << (key[i] / 8));
      }
    }
    printf("active_lct(trig)=%d%d%d%d%d\n", (k>>4)&1, (k>>3)&1, (k>>2)&1, 
     (k>>1)&1, k&1);
    if ((k & active_lct) != k) wait_for_return();
  }
  else printf("clctsim: no pretrigger found\n");
*/ /* End debug */

  return(0);
}

/*----------------------------------------------------------------------------*/

/*
 * Function pretrig_alg
 */

int pretrig_alg(int algorithm, int t, short int *active_lct) {
  int              j, ilct, layer, nhits;
  short int        jm1, jm2, jp1, jp2, jp3;
  unsigned long    tbit;
  logical          pretrigger;

/* Init */
  pretrigger = _FALSE;
  *active_lct = 0;
  tbit = 1 << t;

/* Algorithm 1: count the number of layers with a comparator hit in the dogbone 
 * pattern and keep track of which lct's satisfy the pretrigger criteria.
 */
  if (algorithm == 1) {
    for (ilct = 0; ilct < NLCT; ilct++) {
      for (j = 32 * ilct; j < 32 * (ilct+1); j++) {
        jm1 = (j - 1 < 0) ? 0 : j - 1;
        jp1 = (j + 1 > NHALFSTRIP-1) ? NHALFSTRIP - 1 : j + 1;
        jp2 = (j + 2 > NHALFSTRIP-1) ? NHALFSTRIP - 1 : j + 2;

/* Dog-bone pattern (with staggering) */
        nhits = 0;
        for (layer = 0; layer < NLAYER; layer++) {
          if (layer == 2) {
            if (clct_s[layer][j] & tbit) nhits++; 
//          if (clct_s[layer][j] & tbit) 
//           printf("Found a hit in layer %d for halfstrip %d; now nhits-%d\n", 
//           layer+1, j+1, nhits);
          }
          else if (layer == 0 || layer == 4) {
            if ((clct_s[layer][jm1] | clct_s[layer][j] | clct_s[layer][jp1]) 
             & tbit) nhits++; 
//          if ((clct_s[layer][jm1] | clct_s[layer][j] | clct_s[layer][jp1]) 
//           & tbit) 
//           printf("Found a hit in layer %d for halfstrip %d; now nhits-%d\n", 
//           layer+1, j+1, nhits);
          }
          else {
            if ((clct_s[layer][j] | clct_s[layer][jp1] | clct_s[layer][jp2]) 
             & tbit) nhits++; 
//          if ((clct_s[layer][j] | clct_s[layer][jp1] | clct_s[layer][jp2]) 
//           & tbit) 
//           printf("Found a hit in layer %d for halfstrip %d; now nhits-%d\n", 
//           layer+1, j+1, nhits);
          }
        }

/* Debug */
        if (nhits > 0) printf("pretrig_alg: found nhits=%d for hs=%d\n",
         nhits, j+1);

/* Apply the pretrigger threshold, and set active_lct bits. Also set bits
 * for the neighbor LCT if j is in the first or last 2 strips of the CFEB.
 */
        if (nhits >= nplanes_for_pretrig) {
//        printf("pretrig (algorithm 1) logic satisfied for halfstrip %d\n", 
//         j+1);
          pretrigger = _TRUE;
          *active_lct |= 1 << ilct;
          if (j % NHALFSTRIP < 4 && ilct > 0) *active_lct |= 1 << (ilct - 1);
          else if (j % NHALFSTRIP > 27 && ilct < 4) 
           *active_lct |= 1 << (ilct + 1);
        }
      }     /* end of loop over halfstrips */
    }     /* end of loop over LCT's */
  }

/* Algorithm 2: count the number of layers with a distrip hit in the bufferfly 
 * pattern.
 */
  else if (algorithm == 2) {

/* Fill distrip array from comparator hit array */
    for (layer = 0; layer < NLAYER; layer++) {
      for (j = 0; j < NHALFSTRIP; j+=4) {
        clct_d[layer][j/4] = clct_s[layer][j] | clct_s[layer][j+1] | 
         clct_s[layer][j+2] | clct_s[layer][j+3];
      }
    }

    for (ilct = 0; ilct < NLCT; ilct++) {
      for (j = 8 * ilct; j < 8 * (ilct+1); j++) {
        jm1 = (j - 1 < 0) ? 0 : j - 1;
        jm2 = (j - 2 < 0) ? 0 : j - 2;
        jp1 = (j + 1 > NDISTRIP-1) ? NDISTRIP - 1 : j + 1;
        jp2 = (j + 2 > NDISTRIP-1) ? NDISTRIP - 1 : j + 2;
        jp3 = (j + 3 > NDISTRIP-1) ? NDISTRIP - 1 : j + 3;

/* Butterfly pattern, max width 5 halfstrips (with staggering) */
        nhits = 0;
        for (layer = 0; layer < NLAYER; layer++) {
          if (layer == 2) {
            if (clct_d[layer][j] & tbit) nhits++; 
          }
            else if (layer == 1 || layer == 3) {
            if ((clct_d[layer][j] | clct_d[layer][jp1] | clct_d[layer][jp2]) 
             & tbit) nhits++; 
          }
          else if (layer == 0 || layer == 4) {
            if ((clct_d[layer][jm2] | clct_d[layer][jm1] | clct_d[layer][j]
             | clct_d[layer][jp1] | clct_d[layer][jp2]) & tbit) nhits++; 
          }
          else {
            if ((clct_d[layer][jm1] | clct_d[layer][j] | clct_d[layer][jp1] 
             | clct_d[layer][jp2] | clct_d[layer][jp3]) & tbit) nhits++; 
          }
        }

/* Apply the pretrigger threshold, and set active_lct bits. Also set bits
 * for the neighbor LCT if j is in the first or last 2 distrips of the CFEB.
 */
        if (nhits >= nplanes_for_pretrig) {
//        printf("pretrig (algorithm 2) logic satisfied for distrip %d\n", j+1);
          pretrigger = _TRUE;
          *active_lct |= 1 << ilct;
          if (j % 8 < 2 && ilct > 0) *active_lct |= 1 << (ilct - 1);
          else if (j % 8 > 5 && ilct < 4) *active_lct |= 1 << (ilct + 1);
//        printf("ilct=%d, j%8=%d, active_lct now = %#x\n", ilct, j%8, 
//         *active_lct);
        }
      }     /* end of loop over halfstrips */
    }     /* end of loop over LCT's */
  }
//printf("Finished pretrig_alg for timebin %d\n", t);
  return pretrigger;
}

/*---------------------------------------------------------------------------*/

/*
 * Function trig_alg
 */

void trig_alg(int algorithm, short int active_lct, int t, short int *key, 
 short int *pattern, short int *valid) {
  int              i, j, k, c0, c1, ilct, layer, nhits, nlayers;
  int              max_nhits[NCFEB], best_key[NCFEB], best_pattern[NCFEB];
  short int        imin, imax, jm1, jm2, jp1, jp2, offset;
  unsigned long    hit_pattern, match_pattern, tbit;

/* Init */
  tbit = 1 << t;

  for (k = 0; k < 2; k++) {
    key[k] = 0;
    pattern[k] = -1;
    valid[k] = 0;
  }

/* Algorithm 1: count the number of layers with a comparator hit in the dogbone 
 * pattern and find the key halfstrip in each CFEB which has the most layers.
 */
  if (algorithm == 1) {
    for (ilct = 0; ilct < 5; ilct++) {
      max_nhits[ilct] = 0;
      best_key[ilct] = 0;
    }

    for (ilct = 0; ilct < NLCT; ilct++) {
//    if ((active_lct & (1 << ilct)) == 0) continue;

      for (j = 32 * ilct; j < 32 * (ilct+1); j++) {
        jm1 = (j - 1 < 0) ? 0 : j - 1;
        jp1 = (j + 1 > NHALFSTRIP-1) ? NHALFSTRIP - 1 : j + 1;
        jp2 = (j + 2 > NHALFSTRIP-1) ? NHALFSTRIP - 1 : j + 2;

/* Dog-bone pattern (with staggering) */
        nhits = 0;
        for (layer = 0; layer < NLAYER; layer++) {
          if (layer == 2) {
            if (clct_s[layer][j] & tbit) nhits++; 
          }
          else if (layer == 0 || layer == 4) {
            if ((clct_s[layer][jm1] | clct_s[layer][j] | clct_s[layer][jp1]) 
             & tbit) nhits++; 
          }
          else {
            if ((clct_s[layer][j] | clct_s[layer][jp1] | clct_s[layer][jp2]) 
             & tbit) nhits++; 
          }
        }
//      if (nhits > 0) printf("trig_alg: found %d hits for L%d hs %3d\n",
//       nhits, layer+1, j+1);

/* Keep track of the halfstrip with the most hits */
        if (nhits > max_nhits[ilct]) {
          best_key[ilct] = j;
          max_nhits[ilct] = nhits;
//        printf("trig_alg: best_key[%d]=%3d\n", ilct, best_key[ilct]+1);
        }
      }     /* end of loop over halfstrips */

/* Simulate "resolver" which removes duplicate LCT's at cfeb boundaries */
      if (ilct > 0) {
        if ((max_nhits[ilct] > 0) && (max_nhits[ilct-1] > 0) && 
         ((best_key[ilct] - best_key[ilct-1]) <= 4)) {
          if (max_nhits[ilct] > max_nhits[ilct-1]) max_nhits[ilct-1] = 0;
          else max_nhits[ilct] = 0;
        }
      } 
    }     /* end of loop over LCT's */

//  printf("trig_alg: best_keys=%3d %3d %3d %3d %3d, max_nhits=%d %d %d %d %d\n"
//   , best_key[0]+1, best_key[1]+1, best_key[2]+1, best_key[3]+1, 
//   best_key[4]+1, max_nhits[0], max_nhits[1], max_nhits[2], max_nhits[3], 
//   max_nhits[4]);

/* Find the two best LCT's */ 
    for (ilct = 0; ilct < NLCT; ilct++) {
      for (i = 0; i < 2; i++) {
        if (max_nhits[ilct] > pattern[i]) {
          pattern[i] = max_nhits[ilct];
          key[i] = best_key[ilct];
          break;
        }
      }
    }      /* end of loop over LCT's */

/* Compare with threshold */
    for (i = 0; i < 2; i++) 
     if (pattern[i] >= pattern_num_thr) valid[i] = 1;
//  printf("pattern_num_thr=%d, valid[0],[1] = %d, %d\n", pattern_num_thr, 
//   valid[0], valid[1]);
  }

/* Algorithm 2: count the number of layers with a distrip hit in the bufferfly 
 * pattern, and find the key halfstrip in each CFEB which has the most layers.
 */
  else if (algorithm == 2) {
    for (ilct = 0; ilct < 5; ilct++) {
      max_nhits[ilct] = 0;
      best_key[ilct] = 0;
    }

/* Fill distrip array from comparator hit array */
    for (layer = 0; layer < NLAYER; layer++) {
      for (j = 0; j < NHALFSTRIP; j+=4) {
        if (layer % 2 == 0) {
          clct_d[layer][j/4] = clct_s[layer][j] | clct_s[layer][j+1] | 
           clct_s[layer][j+2] | clct_s[layer][j+3];
        }
        else if (j != NHALFSTRIP - 4) {
          clct_d[layer][j/4] = clct_s[layer][j+1] | clct_s[layer][j+2] | 
           clct_s[layer][j+3] | clct_s[layer][j+4];
        }
        else {
          clct_d[layer][j/4] = clct_s[layer][j+1] | clct_s[layer][j+2] | 
           clct_s[layer][j+3];
        }
//      if (clct_d[layer][j/4]) printf("found clct_d=%lx for L%d, ds %d\n",
//       clct_d[layer][j/4], layer+1, j/4+1);
      }
    }

    for (ilct = 0; ilct < NLCT; ilct++) {
//    if ((active_lct & (1 << ilct)) == 0) continue;

      for (j = 8 * ilct; j < 8 * (ilct+1); j++) {
        jm1 = (j - 1 < 0) ? 0 : j - 1;
        jm2 = (j - 2 < 0) ? 0 : j - 2;
        jp1 = (j + 1 > NDISTRIP-1) ? NDISTRIP - 1 : j + 1;
        jp2 = (j + 2 > NDISTRIP-1) ? NDISTRIP - 1 : j + 2;

/* Butterfly pattern, max width 5 halfstrips (no staggering for distrips) */
        nhits = 0;
        for (layer = 0; layer < NLAYER; layer++) {
          if (layer == 2) {
            if (clct_d[layer][j] & tbit) nhits++; 
          }
            else if (layer == 1 || layer == 3) {
            if ((clct_d[layer][jm1] | clct_d[layer][j] | clct_d[layer][jp1]) 
             & tbit) nhits++; 
          }
          else {
            if ((clct_d[layer][jm2] | clct_d[layer][jm1] | clct_d[layer][j]
             | clct_d[layer][jp1] | clct_d[layer][jp2]) & tbit) nhits++; 
          }
        }

//      if (nhits > 0) printf("trig_alg: found %d hits for ds %2d\n",
//       nhits, j+1);

/* Keep track of the halfstrip with the most hits */
        if (nhits > max_nhits[ilct]) {
          best_key[ilct] = j;
          max_nhits[ilct] = nhits;
//        printf("trig_alg: best_key[%d]=%3d\n", ilct, best_key[ilct]+1);
        }
      }     /* end of loop over halfstrips */

/* Simulate "resolver" which removes duplicate LCT's at cfeb boundaries */
      if (ilct > 0) {
        if ((max_nhits[ilct] > 0) && (max_nhits[ilct-1] > 0) && 
         ((best_key[ilct] - best_key[ilct-1]) <= 4)) {
          if (max_nhits[ilct] > max_nhits[ilct-1]) max_nhits[ilct-1] = 0;
          else max_nhits[ilct] = 0;
        }
      } 
    }      /* end of loop over LCT's */

//  printf("trig_alg: best_keys=%3d %3d %3d %3d %3d, max_nhits=%d %d %d %d %d\n"
//   , best_key[0]+1, best_key[1]+1, best_key[2]+1, best_key[3]+1, 
//   best_key[4]+1, max_nhits[0], max_nhits[1], max_nhits[2], max_nhits[3], 
//   max_nhits[4]);

/* Find the two best LCT's */ 
    for (ilct = 0; ilct < NLCT; ilct++) {
      for (i = 0; i < 2; i++) {
        if (max_nhits[ilct] > pattern[i]) {
          pattern[i] = max_nhits[ilct];
          key[i] = best_key[ilct];
          break;
        }
      }
    }

/* Compare with threshold */
    for (i = 0; i < 2; i++) 
     if (pattern[i] >= pattern_num_thr) valid[i] = 1;
//  printf("pattern_num_thr=%d, valid[0],[1] = %d, %d\n", pattern_num_thr, 
//   valid[0], valid[1]);
  }

/* Algorithm 3: find the lowest numbered pattern in each CFEB which matches 
 * the comparator hits in the most layers.
 */
  else if (algorithm == 3) {
//  printf("active_lct = %#x\n", active_lct);
    for (ilct = 0; ilct < NLCT; ilct++) {
      best_pattern[ilct] = -1;
      if ((active_lct & (1 << ilct)) == 0) continue;

/* Check halfstrip patterns first */
      for (j = 32 * ilct; j < 32 * (ilct+1); j++) {
        imin = -2;
        imax = +2;
        if (j < 2) imin = -j;
        else if (j > 157) imax = 159 - j;

/* Construct a word specifying the pattern in 6 layers x 5 halfstrips */
        hit_pattern = 0;
        for (layer = 0; layer < NLAYER; layer++) {
          offset =  2 + 5 * layer;
          k = layer % 2;               /* allow for stagger */
          for (i = imin; i <= imax; i++) 
           if (clct_s[layer][j+i+k] & tbit) hit_pattern |= 1 << (i + offset);
        }

/* Debug */ /*
        if (hit_pattern) {
          printf("For halfstrip %3d, hit_pattern=", j+1);
          for (i = 0; i < 30; i++) {
            printf("%ld", (hit_pattern >> i) & 1);
            if (i % 5 == 4) printf(" ");
          }
          printf("\n");
        }
*/

/* Compare this word with each pattern in turn, and count the number of layers
 * with matched hits. Lower-numbered codes are preferred when the number of
 * layers is equal.
 */
        if (hit_pattern != 0) {
          for (i = 0; i < NCODES; i++) {
            nlayers = 0;
            if ((match_pattern = hit_pattern & tpattern[i]) != 0) {
              for (k = 0; k < NLAYER; k++) {
                if (match_pattern & 0x1f) nlayers++;
                match_pattern >>= 5;
              }
              if (tcode(0,nlayers,i) > best_pattern[ilct]) {
                best_pattern[ilct] = tcode(0,nlayers,i);
                best_key[ilct] = j;
              }
//            if (tcode(0,nlayers,i) > -1) printf("found a match to code %2d "
//             "with %d layers hit (tcode=%3d)\n", i, nlayers, 
//             tcode(0,nlayers,i));
            }
          }
        }
      }     /* end of loop over halfstrips in one CFEB */

/* If no halfstrip patterns are matched, check the distrip patterns */
      if (best_pattern[ilct] < 0) {
//      printf("trig_alg: checking distrip patterns for ilct=%d\n", ilct);
        for (j = 8 * ilct; j < 8 * (ilct+1); j++) {
          imin = -2;
          imax = +2;
          if (j < 2) imin = -j;
          else if (j > 37) imax = 39 - j;

/* Construct a word specifying the pattern in 6 layers x 5 distrips */
          hit_pattern = 0;
          for (layer = 0; layer < NLAYER; layer++) {
            offset =  2 + 5 * layer;
            k = layer % 2;               /* allow for stagger */

          for (i = imin; i <= imax; i++) 
           if (clct_s[layer][j+i+k] & tbit) hit_pattern |= 1 << (i + offset);

            for (i = imin; i <= imax; i++) 
              if ((j+i) == 39 && k == 1) {
                if ((clct_s[layer][4*(j+i)+k] | clct_s[layer][4*(j+i)+k+1] | 
                 clct_s[layer][4*(j+i)+k+2]) 
                 & tbit) hit_pattern |= 1 << (i + offset);
              }
              else {
                if ((clct_s[layer][4*(j+i)+k] | clct_s[layer][4*(j+i)+k+1] | 
                 clct_s[layer][4*(j+i)+k+2] | clct_s[layer][4*(j+i)+k+3]) 
                 & tbit) hit_pattern |= 1 << (i + offset);
              }
          }

/* Debug */ /*
          if (hit_pattern) {
            printf("For distrip %3d, hit_pattern=", j+1);
            for (i = 0; i < 30; i++) {
              printf("%ld", (hit_pattern >> i) & 1);
              if (i % 5 == 4) printf(" ");
            }
            printf("\n");
          }
*/
  
/* Compare this word with each pattern in turn, and count the number of layers
 * with matched hits. Lower-numbered codes are preferred when the number of
 * layers is equal.
 */
          if (hit_pattern != 0) {
            for (i = 0; i < NCODES; i++) {
              nlayers = 0;
              if ((match_pattern = hit_pattern & tpattern[i]) != 0) {
                for (k = 0; k < NLAYER; k++) {
                  if (match_pattern & 0x1f) nlayers++;
                  match_pattern >>= 5;
                }
                
                if (tcode(1,nlayers,i) > best_pattern[ilct]) {
                  best_pattern[ilct] = tcode(1,nlayers,i);
                  best_key[ilct] = j;
                }

//              if (tcode(0,nlayers,i) > -1) printf("found a match to code %2d "
//               "with %d layers hit (tcode=%3d)\n", i, nlayers, 
//               tcode(1,nlayers,i));
              }
            }
          }
        }     /* end of loop over distrips */
      }     /* end of searching for distrip patterns in this LCT */

/* Simulate "resolver" which removes duplicate LCT's at cfeb boundaries */
/* It removes halfstrips or distrips which are within 2 strips of each other */
/* (Actual resolver logic is still undefined except in the 2-halfstrip case.) */
      if (ilct > 0) {
        c0 = best_pattern[ilct-1];
        c1 = best_pattern[ilct];

        if ((c1 > 0) && (c0 > 0)) { 
          if ((hs(c1) && hs(c0) && (best_key[ilct]-best_key[ilct-1]) <= 4) ||
           (hs(c1) && !hs(c0) && (best_key[ilct]-4*best_key[ilct-1]+3) <= 4) ||
           (!hs(c1) && hs(c0) && (4*best_key[ilct]-best_key[ilct-1]+3) <= 4) ||
           (!hs(c1) && !hs(c0) && (best_key[ilct]-best_key[ilct-1]) <= 4) ) {
            if (c1 > c0) best_pattern[ilct-1] = -1;
            else best_pattern[ilct] = -1;
          }
        }
      } 
    }     /* end of loop over LCT's */

    
/* Find the best two codes from the 5 LCT's */
    for (ilct = 0; ilct < NLCT; ilct++) {
      for (k = 0; k < 2; k++) {
        if (best_pattern[ilct] > pattern[k]) {
          key[k] = best_key[ilct];
          pattern[k] = best_pattern[ilct];
          break;
        }
      }
//    if (((active_lct & (1 << ilct)) == 0) && (best_pattern[ilct] > -1)) {
//      printf("found pattern=%d in lct %d although no pretrigger\n",
//       best_pattern[ilct], ilct);
//      wait_for_return();
//    }
    }

/* Validity still depends on the number of layers hit */
    for (k = 0; k < 2; k++) valid[k] = nl(pattern[k]) >= pattern_num_thr;

  }
//printf("Finished trig_alg\n");
  return;
}

/*---------------------------------------------------------------------------*/

/* 
 * Function: get_tpatterns
 * Subroutine to construct 32-bit words (6 layers * 5 bits) corresponding
 * to the trigger codes. These words could just be parameters; it's done this
 * way because the patterns seem easier to read and write if specified as 
 * character arrays.
 */

void get_tpatterns(void) {
  int        j, k, code[NCODES];
  char       tbits[31];
  char       *p;

  char   code_definitions[NCODES][42] = {
   "  0   00100 00100 00100 00100 00100 00100",
   "  1   01000 00100 00100 00100 00100 00100",
   "  2   00010 00100 00100 00100 00100 00100",
   "  3   00100 00100 00100 00100 00100 00010",
   "  4   00100 00100 00100 00100 00100 01000",
   "  5   01000 01000 00100 00100 00100 00100",
   "  6   00010 00010 00100 00100 00100 00100",
   "  7   00100 00100 00100 00100 00010 00010",
   "  8   00100 00100 00100 00100 01000 01000",
   "  9   00100 00100 00100 00010 00010 00010",
   " 10   00100 00100 00100 01000 01000 01000",
   " 11   01000 00100 00100 00100 00100 00010",
   " 12   00010 00100 00100 00100 00100 01000",
   " 13   01000 00100 00100 00100 00010 00010",
   " 14   00010 00100 00100 00100 01000 01000",
   " 15   00010 00010 00100 00100 00100 00010",
   " 16   01000 01000 00100 00100 00100 01000",
   " 17   01000 00100 00100 00010 00010 00010",
   " 18   00010 00100 00100 01000 01000 01000",
   " 19   00100 00100 00100 00010 00010 00001",
   " 20   00100 00100 00100 01000 01000 10000",
   " 21   01000 01000 00100 00100 00010 00010",
   " 22   00010 00010 00100 00100 01000 01000",
   " 23   01000 00100 00100 00010 00010 00001",
   " 24   00010 00100 00100 01000 01000 10000",
   " 25   01000 01000 00100 00100 00010 00001",
   " 26   00010 00010 00100 00100 01000 10000",
   " 27   10000 01000 00100 00100 00010 00010",
   " 28   00001 00010 00100 00100 01000 01000",
   " 29   01000 01000 00100 00010 00010 00001",
   " 30   00010 00010 00100 01000 01000 10000",
   " 31   01000 00100 00100 00010 00001 00001",
   " 32   00010 00100 00100 01000 10000 10000",
   " 33   01000 01000 00100 00010 00001 00001",
   " 34   00010 00010 00100 01000 10000 10000",
   " 35   10000 01000 00100 00100 00010 00001",
   " 36   00001 00010 00100 00100 01000 10000",
   " 37   10000 01000 00100 00010 00010 00001",
   " 38   00001 00010 00100 01000 01000 10000" };

   for (j = 0; j < NCODES; j++) {
     p = tbits;
     sscanf(code_definitions[j], " %2d   %s %s %s %s %s %s", &code[j], p, p+5, 
      p+10, p+15, p+20, p+25);
/*   printf("code %d: bits %s  ", code[j], tbits); */

     tpattern[j] = 0;
     for (k = 0; k < 30; k++) if (tbits[k] == '1') tpattern[j] |= (1<<k);
/*   printf(", tpattern %08lx\n", tpattern[j]); */
  }
}

/*---------------------------------------------------------------------------*/

int tcode(int distrip, int nlayer, int code) {
  int    j;
  if (nlayer < 4) j = -1;
  else j = 255 - (code + (6 - nlayer) * NCODES + distrip * 3 * NCODES);     
  return j;
}

/*---------------------------------------------------------------------------*/

int hs(int tcode) {
  if (255 - tcode < 3 * NCODES) return 1;
  else return 0;
}

/*---------------------------------------------------------------------------*/

int nl(int tcode) {
  int    j;
  if (tcode < 0) j = 0;
  else j = 6 - (((255 - tcode) % (3 * NCODES)) / NCODES);
  return j;
}

/*---------------------------------------------------------------------------*/

int so(int tcode, int layer) {
/* Calculate offset in key-strip units from key strip */
  int    i, j, k;
  i = (255 - tcode) % NCODES;
  k = (tpattern[i] >> (5 * layer)) & 0x1f;
  for (j = 0; j < 5; j++) if (k & (1 << j)) break;
//printf("strip offset for L%d code %d = %d\n", layer+1, i, j-2);
  return j-2;
}
