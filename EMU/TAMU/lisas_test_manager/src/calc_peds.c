/*******************************************************************************
   Function to calculate pedestals from histograms
   (slightly modified version of David's routine used by the event_analysis)
*******************************************************************************/

int calc_peds(int mode, float *mean, float *rms) {
  int      hid, nx, ny, nwt, loc;
  int      ibin, ilayer, istrip, ilower, iupper, imax;
  float    histo_content[MAXBINS];
  float    xmi, xma, ymi, yma, xbin, xwidth;
  float    avg, wt_sum;
  double   sum, sum_sq;
  char     htitle[80];

  for (ilayer=0; ilayer < NLAYER; ilayer++) {
    for (istrip=0; istrip < NSTRIP; istrip++) {
      hid = 100*(ilayer + 1) + (istrip + 1);

      HGIVE(hid, htitle, nx, xmi, xma, ny, ymi, yma, nwt, loc);
      xwidth = (xma - xmi) / nx;

      HUNPAK(hid, histo_content, "HIST", 0);

      if (mode == 0) {
/* Find the bin with most entries */
        imax = 0;
        for (ibin = 0; ibin < nx; ibin++) {
          if (histo_content[ibin] > histo_content[imax]) imax = ibin;  
        }
   
/* If imax is not too close to one of the array boundaries, then
 * take 10 bins above and below imax. We will use this range of
 * bins to calculate the mean and standard deviation.
 */
        ilower = ((imax - 15) < 0) ? 0 : imax - 15;
        iupper = ((imax + 15) >= nx) ? nx : imax + 15;
      }
      else {
        ilower = 0;
        iupper = nx;
      }

/* Calculated the weighted mean */
      sum    = 0;
      sum_sq = 0;
      wt_sum = 0;
      xbin = xmi + (ilower + 0.5) * xwidth;

      for (ibin = ilower; ibin < iupper; ibin++) {
        sum    += xbin * histo_content[ibin];
        sum_sq += xbin * xbin * histo_content[ibin];
        wt_sum += histo_content[ibin];
        xbin   += xwidth;
      }
  
      if (wt_sum != 0.0) {
        avg = sum / wt_sum;
        mean[ilayer][istrip] = avg;
        rms[ilayer][istrip] = sqrt(sum_sq / wt_sum - avg * avg);
      }
      else {
        mean[ilayer][istrip] = 0.0;
        rms [ilayer][istrip] = 0.0;
      }
      printf("mean[%d][%d] = %f\n", ilayer, istrip, mean[ilayer][istrip]);

    }  /* end for istrip */
  }  /* end for ilayer */

  return 0;
}
