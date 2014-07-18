#ifndef _HVCARD_PCIE_CONSTS_
#define _HVCARD_PCIE_CONSTS_

#include "hvcard_pcie.h"
#include "hv_types.h"

const char* hvcmd_str[LAST_CMD] =
{
  "HV_SET_OFF",
  "HV_SET_ON",
  "HV_SET_LOCK",
  "HV_SET_RAMP_UP",
  "HV_SET_RAMP_DOWN",
  "HV_SET_VMAX",
  "HV_SET_IMAX",
  "HV_SET_VSET",
  "HV_SET_STATE",
  "HV_GET_VMON",
  "HV_GET_IMON",
  "HV_GET_RAMP_UP",
  "HV_GET_RAMP_DOWN",
  "HV_GET_VMAX",
  "HV_GET_IMAX",
  "HV_GET_VCUR",
  "HV_GET_VSET",
  "HV_GET_STATE",
  "HV_GET_STATUS",
  "HV_GET_ITRIP",
  "HV_GET_VTRIP",
  "HV_GET_MOD_ADDR",
  "HV_GET_MOD_ID",
  "HV_GET_MOD_ILOCK",
  "HV_GET_MOD_POS5V",
  "HV_GET_MOD_NEG5V",
  "HV_GET_MOD_TYPE",
  "HV_SET_MOD_TYPE",
  "HV_GET_MOD_NUM_CHANS",
  "HV_GET_MOD_RAWDATA",
  "HV_SET_MOD_STATE",
  "HV_GET_CARD_DATA",
  "HV_GET_MOD_DATA",
  "HV_GET_CHAN_DATA",
  "HV_SET_RELAY",
  "HV_GET_RELAY",
  "HV_GET_FUSE_STATUS",
  "HV_RESET_TRIP",
  "HV_SET_TRIP_DELAY",
  "HV_GET_TRIP_DELAY",
  "HV_GET_TRIP_CNT",
  "HV_GET_MOD_HVMON",
  "HV_SET_MASTER",
  "HV_GET_MASTER",
  "HV_SET_MASTER_TRIP_DELAY",
  "HV_GET_MASTER_TRIP_DELAY",
  "HV_SET_TRIP_RESET_DELAY",
  "HV_GET_TRIP_RESET_DELAY",
  "HV_GET_TRIP_RESET_DELAY_CNT",
  "HV_SET_TRIP_MAX_RESETS",
  "HV_GET_TRIP_MAX_RESETS",
  "HV_GET_TRIP_RESETS_CNT",
  "HV_SET_ILOCK_DELAY",
  "HV_GET_ILOCK_DELAY",
  "HV_GET_ILOCK_DELAY_CNT",
  "HV_DETECT",
  "HV_SET_MOD_POLICY",
  "HV_GET_MOD_POLICY",
  "HV_UPDATE_CALIB",
  "HV_SET_OV_LIMIT",
  "HV_SET_OV_TRIP_DELAY",
  "HV_DO_CALIB",
  "HV_SET_RAMPDOWN_SET_DELAY",
  "HV_SET_IMAX_RAMP",
  "HV_GET_IMAX_RAMP",
  "HV_SET_MOD_PRIMARY_STATUS",
  "HV_SET_DEAD_MAX_CNT",
  "HV_GET_MOD_HIST_DATA",
  "HV_SET_GLOBAL_STATE"
};


/* Default modules channel maps */
const UINT  mapdef[48] = { 0, 1,  2,  3,  4,  5,  6,  7,  8,  9,
                           10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                           20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                           30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                           40, 41, 42, 43, 44, 45, 46, 47
                         };

const UINT  map30[48] = {  0, 1,  2,  3,  4,  5,  6,  7,  16, 17,
                           18, 19, 20, 21, 22, 8,  9,  10, 11, 12,
                           13, 14, 15, 24, 25, 26, 27, 28, 29, 30,
                           23, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                           40, 41, 42, 43, 44, 45, 46, 47
                        };

const UINT  map36[48] = {  0, 1,  2,  3,  4,  5,  6,  7,  16, 17,
                           18, 19, 20, 21, 22, 23, 32, 33, 8,  9,
                           10, 11, 12, 13, 14, 15, 24, 25, 26, 27,
                           28, 29, 30, 31, 35, 36, 34, 37, 38, 39,
                           40, 41, 42, 43, 44, 45, 46, 47
                        };

const UINT  mapmon30[48] = {  28, 29,  30,  8,  9,  10,  6,  7,  16, 14,
                              15, 24, 0, 1, 2, 31,  35,  36, 11, 12,
                              13, 17, 18, 19, 25, 26, 27, 3, 4, 5,
                              31, 32, 33, 34, 35, 36, 37, 38, 39,
                              40, 41, 42, 43, 44, 45, 46, 47
                           };

const UINT* maps[6] = { NULL, mapdef, map30, map36, mapmon30, map36};

const char* modtype_str[6] = { "None", "Master", "RDB 30", "RDB 36", "MON 30", "MON 36"};
const UINT num_chans[6] = { 0, 8, 30, 36, 30, 36 };
const char* hvstate_str [HV_STATE_SET+1] = {"OFF", "ON", "RAMP UP", "RAMP DOWN", "DEAD", "ILOCK", "INT TRIP", "CH INT TRIP", "CH OV TRIP", "SET" };

#endif
