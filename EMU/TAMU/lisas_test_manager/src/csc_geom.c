/*
 * csc_geom.c 
 * file contains functions to determine different geometrical parameters 
 * of different types of Chambers
 * Author : Sergei Dolinsky      
 * Date   : 12 March 2003
 */

#include <stdlib.h>
#include <stdio.h>
//#include <syslog.h>
#include <math.h>
//#include "cfortran.h"
//#include "hbook.h"
//#include "daq_conf.h"
//#include "csc_event.h"       /* upevt_                 */
#include "csc_parameters.h"  /* NLAYER, NSTRIP, etc... */
//#include "one_page_plot.h"   /* STRIP_PLOT,...         */
//#include "test_utils.h"      /* chamber labels         */
//#include "test_params.h"     /* test config parameters */
#include "misc.h"            /* definition of data type logical */
//#include "clctsim.h"         /* trigger simulation utilities */
#include "csc_geom.h"        /* more chamber geometry */

int first_wire_wg[CSC_TYPES][NWIRES_MAX] = FSW_WG;
int width_wg[CSC_TYPES][NWIRES_MAX]      = WIDTH_WG;
char* chamber_types[CSC_TYPES] = CSC_NAME;
float al_pin[CSC_TYPES] =  R_ALGNM;
float fw_al[CSC_TYPES] =  FW_ALGNM;
float w_spc[CSC_TYPES]  = WIRE_SPACE;

logical is_wg(int csc_type, int  wg)
{
  if ((csc_type <1 ) || (CSC_TYPES <csc_type))
    return _FALSE;
  if ((wg <1) || (NWIRES_MAX < wg) )
    return _FALSE;
  if (width_wg[csc_type-1][wg-1] > 0)
    return _TRUE;
  else
    return _FALSE;
}    
float r_al_pin(int csc_type)
{
  return al_pin[csc_type-1];
} 
//the width
float w_wg(int csc_type,int wg)
{
  if ( is_wg(csc_type,wg))
    {
      return   width_wg[csc_type-1][wg-1]*w_spc[csc_type-1];
    }
  else 
    return -1.;
} 

//the dist to al_pin of first wire 
float fw_wg(int csc_type,int wg)
{
  if ( is_wg(csc_type,wg))
    {
      return fw_al[csc_type-1] + \
	(first_wire_wg[csc_type-1][wg-1])*w_spc[csc_type-1];
    }
  else 
    return -1.;
} 

//the center wg to al_pin
float cntr_wg(int csc_type,int wg)
{
  if ( is_wg(csc_type,wg))
    {
      return fw_wg(csc_type,wg)+0.5*(w_wg(csc_type,wg)-w_spc[csc_type-1]);
    }
  else 
    return -1.;
} 
//the radius center wg 
float r_cntr_wg(int csc_type,int wg)
{
  if ( is_wg(csc_type,wg))
    {
      return al_pin[csc_type-1]+ cntr_wg(csc_type, wg); 
    }
  else 
    return -1.;
} 

float w_str(int csc_type,int wg)
{
  float r_cntr_wg ;
  float width_strip = -1.;
  float strip_angle=0.;

  if ( is_wg(csc_type,wg))
    {

      switch(csc_type){
      case 1:
	  strip_angle = 10./75.  * PI/180.;
	  break;
      case 2:
	  strip_angle = 10./64.5 * PI/180.;
	  break;
      case 3:
	  strip_angle = 20./75.  * PI/180.;
	  break;
      case 4:
	  strip_angle = 20./75.  * PI/180.;
	  break;
      case 5:
	  strip_angle = 10./75.  * PI/180.;
	  break;
      case 6:
	  strip_angle = 20./75.  * PI/180.;
	  break;
      }         

      r_cntr_wg = cntr_wg(csc_type, wg) + al_pin[csc_type-1];      
      width_strip = r_cntr_wg *2*tan(strip_angle/2.);
      return width_strip;
    }
  else 
    return -1.;
} 

int segm_wg(int wire_group, int csc_type)
{
  int isegm;

  if (wire_group <1 ) return -1;

  isegm = -1;
  switch (csc_type){
  case(1):
    if      (wire_group <= 24) isegm = 1; 
    else if (wire_group <= 48) isegm = 2;
    else if (wire_group <= 64) isegm = 3;
    break;
  case(2):
    if      (wire_group <= 12) isegm = 1;
    else if (wire_group <= 22) isegm = 2;
    else if (wire_group <= 32) isegm = 3;
    break;
  case(3):
    if      (wire_group <= 44) isegm = 1; 
    else if (wire_group <= 80) isegm = 2;
    else if (wire_group <= 112) isegm = 3;
    break;
  case(4):
    if      (wire_group <= 32) isegm = 1; 
    else if (wire_group <= 64) isegm = 2;
    else if (wire_group <= 96) isegm = 3;
    break;
  case(5):
    if      (wire_group <= 16) isegm = 1; 
    else if (wire_group <= 28) isegm = 2;
    else if (wire_group <= 40) isegm = 3;
    else if (wire_group <= 52) isegm = 4;
    else if (wire_group <= 64) isegm = 5;
    break;
  case(6):
    if      (wire_group <= 32) isegm = 1;
    else if (wire_group <= 64) isegm = 2;
    else if (wire_group <= 96) isegm = 3;
    break;
      }
  return isegm;
}

logical at_bound_wg(int wire_group, int csc_type)
{
  logical at_b;

  if (wire_group <1 ) return _FALSE;

  at_b = _FALSE;
  switch (csc_type){
  case(1):
    at_b = (wire_group == 24  ||wire_group == 25  ||
	    wire_group == 48  ||wire_group == 49);
    break;
  case(2):
    at_b = (wire_group == 12  ||wire_group == 13  ||
	    wire_group == 22  ||wire_group == 23);
    break;
  case(3):
    at_b = (wire_group == 44  ||wire_group == 43  ||
	    wire_group == 80  ||wire_group == 81);
    break;
  case(4):
    at_b = (wire_group == 32  ||wire_group == 33  ||
	    wire_group == 64  ||wire_group == 65);
    break;
  case(5):
    at_b = (wire_group == 16  ||wire_group == 17  ||
	    wire_group == 28  ||wire_group == 29  ||
	    wire_group == 40  ||wire_group == 41  ||
	    wire_group == 52  ||wire_group == 53);
    break;
  case(6):
    at_b = (wire_group == 32  ||wire_group == 33  ||
	    wire_group == 64  ||wire_group == 65);
    break;
      }
  return at_b;
}




/*
int main()
{
  int i, icsc;
  float x, wx, rcx, cx, wy;
  for(icsc = 0; icsc< CSC_TYPES; icsc++)
    {
      //      printf ( " Chamber type:  %s\n", chamber_types[icsc]);
      printf ( "\nve/cr   x%d(%d) r _\n",icsc+1,NWIRES_MAX);
      for (i = 0; i<NWIRES_MAX; i++)
	{
	  if (is_wg(icsc+1, i+1))
	  {
	    x =fw_wg(icsc+1, i+1);
	    wx = w_wg(icsc+1, i+1);
	    rcx = r_cntr_wg(icsc+1, i+1);
	    cx = cntr_wg(icsc+1, i+1);
	    wy = w_str(icsc+1, i+1);
     //	    printf(" f_w %4d wg is %7.2f, w_wg is :%6.2f,\
     //                     c_wg is %7.2f, w_str is %5.2f\n", i+1,x,wx, cx,wy);
	      printf(" %6.2f ", cx);
	      if (((i+1)%10) == 0)
	      printf(" _\n");
	  }
	}
    }
  return 0;
}
*/
