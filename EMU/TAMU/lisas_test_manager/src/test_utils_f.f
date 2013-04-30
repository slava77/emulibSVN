      subroutine get_offset(n, d, offset, offset_err)
      implicit none
      integer*4    n, d
      real*4       offset, offset_err
      real*4       x, xerr, xm, xp, offset_m, offset_p
      real*4    gausin
      external  gausin

      if (d.eq.0) then
        offset = -20.
        offset_err = 0.

      else
        x = n / float(d)

        if (x.le.0) then
          offset = -9.5
          xerr = 1. / d
        else if (x.ge.1) then
          offset = +9.5
          xerr = 1. / d
        else 
          offset = gausin(x)
          xerr = sqrt(n * (d-n) / float(d**3))
        endif

        xm = x - xerr
        xp = x + xerr
        if (xm.gt.0.) offset_m = gausin(xm)
        if (xp.lt.1.) offset_p = gausin(xp)

        if (xm.le.0.) then
          offset_err = offset_p - offset
        else if (xp.ge.1.) then
          offset_err = offset - offset_m
        else 
          offset_err = 0.5 * (offset_p - offset_m)
        endif
      endif
      return
      end

*------------------------------------------------------------------------------

      subroutine offset_fit(intercept, intercept_err, chisq)
      implicit none

* Global variables
      include 'csc_parameters.inc'            ! For NSTRIP, NLAYER

      common / quest / iquest(100)
      integer*4    iquest

      common / myparam / param
      real*4       param(3)

* Arguments
      real*4       intercept(NSTRIP,NLAYER)
      real*4       intercept_err(NSTRIP,NLAYER)
      real*4       chisq(NSTRIP,NLAYER)

* Local variables
      integer*4    hid, ilayer, istrip, noent
      real*4       step(2), pmin(2), pmax(2), sigpar(2)

* Functions
      real*4       hmax
      logical      hexist
      external     hmax, hexist

* Loop over layers and strips
      do ilayer = 1, NLAYER
        do istrip = 1, NSTRIP
          hid = 100 * ilayer + istrip

* Default results
          intercept(istrip,ilayer) = 999.
          intercept_err(istrip,ilayer) = 999.
          chisq(istrip,ilayer) = -1.

* Check that the histogram exists, and isn't empty
          if (hexist(hid)) then
            call hnoent(hid, noent)
            if (noent.gt.0) then
* Set fit limits 
              iquest(11) = 3
              iquest(12) = 10

* Do a linear fit
              call hfithn(hid, 'P1', 'QR', 0, param, step, pmin, pmax, 
     &         sigpar, chisq(istrip,ilayer))

* Save the parameters
              intercept(istrip,ilayer) = param(1)
              intercept_err(istrip,ilayer) = sigpar(1)
            endif
          endif
        enddo
      enddo

      return
      end

*------------------------------------------------------------------------------

      subroutine landau_fit(hid, par, sigpar, chisq)
      implicit none
* Arguments
      integer*4    hid
      real*4       par(3), sigpar(3), chisq

* Global variables
      common / myparam / param
      real*4       param(3)

* Local variables
      integer*4    noent
      real*4       mean, step(3), pmin(3), pmax(3)

* Functions
      real*4       hstati, landau
      external     hstati, landau


      call hnoent(hid, noent)
      mean = hstati(hid, 1, ' ', 0)

      call hfithn(hid, 'G', 'Q', 0, param, step, pmin, pmax, sigpar, 
     & chisq)

*     param(1) = noent / 5.
*     param(2) = 0.50 * mean
*     param(3) = 0.260 * param(2) - 13.7 ! for 3.6 kV

*     print *, 'param after Gaussian fit: ', param

      param(1) = param(1) * 6
      param(2) = param(2) * 0.75
      param(3) = param(3) * 0.5

*     print *, 'param initial values: ', param

      call hfith(hid, landau, 'Q', 3, param, step, pmin, pmax, sigpar, 
     & chisq)

      par(1) = param(1)
      par(2) = param(2)
      par(3) = param(3)

      return
      end

*-------------------------------------------------------------
      Function FitFun(X)
*     Implicit None
      Integer*4  NPAR
      Parameter (NPAR=3)
      Common/PAWPAR/par(NPAR)
      Real*4    par
      Real*4    x
     
*     FitFun = par(1)*(1.0+erf((x- par(2))/par(3)))
      FitFun = par(1) * (1.0 + erf((x- par(2)) / (sqrt(2.) * par(3))))

      End

*-------------------------------------------------------------
      Function my_erf(X)
*     Implicit None
      Integer*4  NPAR
      Parameter (NPAR=2)
      Common/MY_PAWPAR/par
      Real*4    par(2)
      Real*4    x, my_erf
     
*      FitFun = par(1)*(1.0+erf((x- par(2))/par(3)))
*      FitFun = par(1) * (1.0 + erf((x- par(2)) / (sqrt(2.) * par(3))))
      my_erf = 0.5 * (1.0 + erf((par(1)-x) / par(2)))
      return
      End

*------------------------------------------------------------------------------
      function my_polly(x)
*      common / testpar / par
*      real*4       par(2)
      Common/MY_PAWPAR/par
      real*4       par(2)
      real*4  x, my_polly

      my_polly = par(1) + x * par(2)
      return
      end
 

*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      subroutine erf_fit(hid1, par, sigpar, chisq)
      implicit none
* Arguments
      integer*4    j, hid1
      real*4       par(2), sigpar(2), chisq

* Global variables
      common / myparam / param
      real*4       param(3)

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
        else
          y(j) = 0
          ey(j) = 999
        endif
          if (ey(j) .le. 0.)then
             ey(j) = 1./denom(j)
*             print *, hid1, j, ey(j)
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

*------------------------------------------------------------------------------
* In this 2-parameter version, the third parameter is a constant which is
* the previously determined time-per-delay-bit.

      function delay2(X)

      common/myparam/par
      real*4       par(3)

      integer*4    j
      real*4       x
      real*4       xp, stepsize, sigma
     
      xp = x - par(1)
      sigma = par(2)
      stepsize = 25. / par(3)

      delay2 = 0.
      do j = 1, 6
        delay2 = delay2 + 
     &   0.5 * (1 + erf((xp - j * stepsize) / (sqrt(2.) * sigma)))
      enddo

      end

*------------------------------------------------------------------------------
* This 3-parameter version of the function can be used to find the time-per-
* delay-bit, nominally 7 ns. For the old CFEBs, the best fit was 6.89 ns.

      function delay3(X)

      common/myparam/par
      real*4       par(3)

      integer*4    j
      real*4       x
      real*4       xp, stepsize, sigma
     
      xp = x - par(1)
      sigma = par(2)
      stepsize = 25. / par(3)

      delay3 = 0.
      do j = 1, 6
        delay3 = delay3 + 
     &   0.5 * (1 + erf((xp - j * stepsize) / (sqrt(2.) * sigma)))
      enddo

      end

*------------------------------------------------------------------------------
 
      function polly(x)
      common / testpar / par
      real*4       par(2)
      real*4  x, polly
      polly = par(1) + x * par(2)
      return
      end
 
*------------------------------------------------------------------------------

      function pulseshape(xt)

* Arguments
      real*4       xt

* Local constants
      integer*4    LAST_TS, NPOINTS
      parameter    (LAST_TS = 10)
      parameter    (NPOINTS = LAST_TS * 16)
      real*4       TIMESTEP
      parameter    (TIMESTEP = 6.89)       ! test pulse delay step, ns

* Global variables
      common / refpoints / ref, time, first
      real*4       ref(NPOINTS), time(NPOINTS)
      logical      first

      common / testpar / par
      real*4       par(2)

* Local variables
      integer*4    i, j, index(NPOINTS)
      real*4       f(NPOINTS), t(NPOINTS)
      real*4       pulseshape

      save index, f, t

* On the first call, generate the reference shape from the 160 points provided
* in the common block by using the CERNLIB routine SORTZV to sort them into 
* increasing time order. 
      if (first) then
        first = .FALSE.
        do j = 0, 15
          do i = 1, LAST_TS
            time(j * LAST_TS + i) = 100 + i * 50 - j * TIMESTEP
          enddo
        enddo

        call sortzv(time, index, NPOINTS, 1, 0, 0)
        do j = 1, NPOINTS
          t(j) = time(index(j))
          f(j) = ref(index(j))
* debug SD
         print 10, j, t(j), f(j)
  10     format('j, time, ref: ', i3, f10.0, f10.1)
        enddo

* Print out typical function values, using DIVDIF to interpolate. 
        do j = 1, 100
          xx = 50 + j * 5
          pulseshape = divdif(f, t, NPOINTS, xx, 1)
*         print 20, j, xx, pulseshape
*  20     format('pulseshape: j=', i3, ' xx=', f8.2, ' f(xx)=', f10.1)
        enddo
      endif

* Calculate the function value from interpolation
      xx = xt - par(2)
*     if (xx.lt.55..or.xx.gt.550.) then 
*       pulseshape = 0
*     else
        pulseshape = par(1) * divdif(f, t, NPOINTS, xx, 1)
*     endif

      return
      end

