      subroutine erf_fit(hid1, par, sigpar, chisq)
      implicit none
* Arguments
      integer*4    j, hid1
      real*4       par(2), sigpar(2), chisq

* Global variables
      common / myparam / param
      real*4       param(2)

* Local constants
      integer*4    NBINS
      parameter    (NBINS = 50)

* Local variables
      integer*4    hid2, hid3, noent
      real*4       step(2), pmin(2), pmax(2)
      real*4       numer(NBINS), denom(NBINS)
      real*4       x(NBINS), y(NBINS), ey(NBINS)

* Functions
      real*4       errfun
      external     errfun

* Check that the "denominator" histogram isn't empty
      hid2 = 1000 + hid1
      call hnoent(hid2, noent)
      if (noent.le.0) then
        chisq = 999
        print *, 'Skipping empty histogram ', hid2
        return
      endif

* Unpack the two histograms
      call hunpak(hid1, numer, 'HIST', 1)
      call hunpak(hid2, denom, 'HIST', 1)

* Divide and calculate errors
      do j = 1, NBINS
        x(j) = 4 * j - 98
        if (denom(j).gt.0) then
          y(j) = 1 + numer(j) / denom(j) 
          ey(j) = sqrt(numer(j) * (denom(j) - numer(j)) / (denom(j)**3))
          if (ey(j) .le. 0.)then
             ey(j) = 1./denom(j)
          endif
        else
          y(j) = 0
          ey(j) = 999
        endif
      enddo

* Fill result histogram 
      hid3 = 2000 + hid1
      call hpak(hid3, y)
      call hpake(hid3, ey)

* Fit the result with the error function
      param(1) = 0.
      param(2) = 5.
      step(1) = 5.
      step(2) = 2.

      call hfith(hid3, errfun, 'QU', 2, param, step, pmin, pmax, sigpar, 
     & chisq)
*     call hfitv(50, 50, 1, x, y, ey, errfun, 'Q', 2, param, step, 
*    & pmin, pmax, sigpar, chisq)

      par(1) = param(1)
      par(2) = param(2)

      return
      end
