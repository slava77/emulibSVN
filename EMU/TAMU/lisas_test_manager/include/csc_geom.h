
/* This is csc_geom.h */
#define PI 3.1415926
#define CSC_TYPES    6          /* the number of CSC's types */
#define NWIRES_MAX   112
#define CSC_NAME     {"ME1.2", "ME1.3", "ME2.1", "ME3.1", "ME234.2", "ME4.1"}
  
#define STRIP_ANGLE  0.0023271  /* Angle in radians between strip centers */
#define K3           0.330      /* Gatti function parameter for P2 */
#define HEIGHT       4.7625     /* Distance from wire plane to cathodes, mm */
#define WG_WIDTH    50.577      /* Width of a wire group */

#define WIRE_SPACE {3.16108,  3.16292, 3.11945, 3.11945, 3.16108, 3.11945}  
#define R_ALGNM    {2784.9,   5089.9,  1438.9,  1638.9,  3609.9,  1837.9 }
#define FW_ALGNM   {  30.1,     30.0,    28.7,    28.4,    28.7,    30.4 }

/* Distance to the center of the first wire group */
#define R_WG_0   3662.0   

#define FSW_WG    {\
{  1,   7 , 14,  21,  28,  35,  42,  49,  56,  63,\
  70,  77,  84,  91,  98, 105, 112, 120, 128, 136,\
 144, 152, 160, 168, 180, 184, 192, 200, 208, 216,\
 224, 232, 240, 249, 258, 267, 276, 285, 294, 303,\
 312, 321, 330, 339, 348, 357, 366, 375, 388, 394,\
 404, 414, 424, 434, 444, 454, 464, 475, 486, 497,\
 508, 519, 530, 541,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0},\
{  1,  16,  32,  48,  64,  80,  96, 112, 128, 144,\
 160, 176, 196, 208, 224, 240, 256, 272, 288, 304,\
 320, 336, 356, 368, 384, 401, 418, 435, 452, 469,\
 486, 503,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0},\
{  1,  5,   10,  15,  20,  25,  30,  35,  40,  45,\
  50,  55,  60,  65,  70,  75,  80,  85,  90,  95,\
 100, 105, 110, 115, 120, 125, 130, 135, 140, 145,\
 150, 155, 160, 165, 170, 175, 180, 185, 190, 195,\
 200, 205, 210, 215, 224, 225, 230, 235, 240, 245,\
 250, 255, 260, 265, 270, 275, 280, 285, 290, 295,\
 300, 305, 310, 315, 320, 326, 332, 338, 344, 350,\
 356, 362, 368, 374, 380, 386, 392, 398, 404, 410,\
 420, 422, 428, 434, 440, 446, 452, 458, 464, 470,\
 476, 482, 488, 494, 500, 506, 512, 518, 524, 530,\
 536, 542, 548, 554, 560, 566, 572, 578, 584, 590,\
 596, 602}, \
{  1,  5,   10,  15,  20,  25,  30,  35,  40,  45,\
  50,  55,  60,  65,  70,  75,  80,  85,  90,  95,\
 100, 105, 110, 115, 120, 125, 130, 135, 140, 145,\
 150, 155, 164, 166, 172, 178, 184, 190, 196, 202,\
 208, 214, 220, 226, 232, 238, 244, 250, 256, 262,\
 268, 274, 280, 286, 292, 298, 304, 310, 316, 322,\
 328, 334, 340, 346, 356, 358, 364, 370, 376, 382,\
 388, 394, 400, 406, 412, 418, 424, 430, 436, 442,\
 448, 454, 460, 466, 472, 478, 484, 490, 496, 502,\
 508, 514, 520, 526, 532, 538,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0},\
{  1,  16,  32,  48,  64,  80,  96, 112, 128, 144,\
 160, 176, 192, 208, 224, 240, 260, 272, 288, 304,\
 320, 336, 352, 368, 384, 400, 416, 432, 452, 464,\
 480, 496, 512, 528, 544, 560, 576, 592, 608, 624,\
 644, 656, 672, 688, 704, 720, 736, 752, 768, 784,\
 800, 816, 836, 848, 864, 880, 896, 912, 928, 944,\
 960, 976, 992,1008,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0},\
{  1,  5,   10,  15,  20,  25,  30,  35,  40,  45,\
  50,  55,  60,  65,  70,  75,  80,  85,  90,  95,\
 100, 105, 110, 115, 120, 125, 130, 135, 140, 145,\
 150, 155, 164, 165, 170, 175, 180, 185, 190, 195,\
 200, 205, 210, 215, 224, 225, 230, 235, 240, 245,\
 250, 255, 260, 265, 270, 275, 280, 285, 290, 295,\
 300, 305, 310, 315, 324, 325, 330, 335, 340, 345,\
 350, 355, 360, 365, 370, 375, 380, 385, 390, 395,\
 400, 405, 410, 415, 420, 425, 430, 435, 440, 445,\
 450, 455, 460, 465, 470, 475,   0,   0,   0,   0,\
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,\
   0,   0}\
}

#define WIDTH_WG { \
{  6,  7,  7,  7,  7,  7,  7,  7,  7,  7, \
   7,  7,  7,  7,  7,  7,  8,  8,  8,  8, \
   8,  8,  8,  4,  4,  8,  8,  8,  8,  8, \
   8,  8,  9,  9,  9,  9,  9,  9,  9,  9, \
   9,  9,  9,  9,  9,  9,  9,  5,  6, 10, \
  10, 10, 10, 10, 10, 10, 11, 11, 11, 11, \
  11, 11, 11, 12,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0}, \
{ 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, \
  16, 12, 12, 16, 16, 16, 16, 16, 16, 16, \
  16, 12, 12, 16, 17, 17, 17, 17, 17, 17, \
  17, 17,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0}, \
{  4,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  5,  5,  1,  1,  5,  5,  5,  5,  5, \
   5,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  5,  5,  5,  6,  6,  6,  6,  6,  6, \
   6,  6,  6,  6,  6,  6,  6,  6,  6,  2, \
   2,  6,  6,  6,  6,  6,  6,  6,  6,  6, \
   6,  6,  6,  6,  6,  6,  6,  6,  6,  6, \
   6,  6,  6,  6,  6,  6,  6,  6,  6,  6, \
   6,  7}, \
{  4,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  1,  2,  6,  6,  6,  6,  6,  6,  6, \
   6,  6,  6,  6,  6,  6,  6,  6,  6,  6, \
   6,  6,  6,  6,  6,  6,  6,  6,  6,  6, \
   6,  6,  6,  2,  2,  6,  6,  6,  6,  6, \
   6,  6,  6,  6,  6,  6,  6,  6,  6,  6, \
   6,  6,  6,  6,  6,  6,  6,  6,  6,  6, \
   6,  6,  6,  6,  6,  7,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0}, \
{ 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, \
  16, 16, 16, 16, 16, 12, 12, 16, 16, 16, \
  16, 16, 16, 16, 16, 16, 16, 12, 12, 16, \
  16, 16, 16, 16, 16, 16, 16, 16, 16, 12, \
  12, 16, 16, 16, 16, 16, 16, 16, 16, 16, \
  16, 12, 12, 16, 16, 16, 16, 16, 16, 16, \
  16, 16, 16, 15,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0}, \
{  4,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  1,  1,  5,  5,  5,  5,  5,  5,  5, \
   5,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  5,  5,  1,  1,  5,  5,  5,  5,  5, \
   5,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  5,  5,  5,  5,  5,  5,  5,  5,  5, \
   5,  5,  5,  5,  5,  5,  0,  0,  0,  0, \
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, \
   0,  0,} \
}
