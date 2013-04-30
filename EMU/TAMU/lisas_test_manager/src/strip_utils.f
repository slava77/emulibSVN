
*-------------------------------------------------------------------------------
* File: strip_utils.f
* This file contains gatfit, trackfit, xMinuit and the Minuit subroutine "fcn",
* and strip_alignment.
* First adapted for FAST site tests in February 2001.
*-------------------------------------------------------------------------------
*
* Function: gatfit
* Find position associated with a cluster, using Minuit fit to Gatti function.
* Ranges of arguments: key_strip 0-79, clct_time 0-15, key_wg 0-63.
* The fit result xclus is returned in units of stripwidths (?)
* Gatfit takes care of the position-dependence of the fit inputs (stripwidth, 
* height).
*
        subroutine gatfit(cpeak, tcpeak, mean, rms, ilayer, 
*     &   wg, iseg, 
     &   strip_width, iseg, 
     &   xclus, xerror, chisq, qclus, tclus, fclus, status, pass)
        implicit none

* Global variables
        include 'csc_event.inc'
        include 'csc_geom.inc'

* Arguments
*        integer*4    wg
        integer*4    cpeak, ilayer, tcpeak, iseg, status, pass
        real*4       mean(NSTRIP,NLAYER), rms(NSTRIP,NLAYER)
        real*4       xclus, xerror, qclus, tclus, fclus, chisq
        real*4       strip_width

* Local constants
        integer*4    nstrips, ntimesamples, nscasamples
        parameter    (nstrips = 5)
        parameter    (ntimesamples = 5)
        parameter    (nscasamples = 16)

        integer*4    peak_min, peak_max, tpeak_min, tpeak_max
        parameter    (peak_min = 1 + nstrips/2)
        parameter    (peak_max = 80 - nstrips/2)
        parameter    (tpeak_min = 1 + ntimesamples/2)
        parameter    (tpeak_max = nscasamples - ntimesamples/2)

        real*4       h_adj(5,NLAYER) / 30 * 0 /
*    &                -0.20,  0.06, -0.05, -0.08, -0.17, 
*    &                 0.15,  0.06,  0.12,  0.16,  0.08,
*    &                 0.13,  0.00,  0.03,  0.11, -0.06, 
*    &                 0.06,  0.08,  0.05,  0.02, -0.18,
*    &                 0.02,  0.06, -0.01, -0.03, -0.22, 
*    &                 0.15,  0.18,  0.10,  0.10,  0.08 /


* Minuit common block (used to pass data values to Minuit)
        common /stripdata/ q, e, h, k3_r8, stripwidth_r8
        real*8       q(ntimesamples, -nstrips/2: nstrips/2)
        real*8       e(-nstrips/2: nstrips/2)
        real*8       h, k3_r8, stripwidth_r8

* Local variables
        integer*4    j, layer, peak, strip, tpeak, ts
        real*8       x, a, t, f, xerr, aerr, terr, ferr, chisq8
        logical      first/.true./
        save         first
        integer*4    oldpass/-1/

* Init
        if (first) then
          first = .false.
          k3_r8 = k3
        endif

* Fortran arrays start with 1, not 0
        peak = cpeak + 1
        tpeak = tcpeak + 1
        layer = ilayer + 1

* Near the chamber edges, fit the first or last 5 strips
        if (peak.lt.peak_min) peak = peak_min
        if (peak.gt.peak_max) peak = peak_max

* For times near the sample limits, use the first or last 5 timesamples
        if (tpeak.lt.tpeak_min) tpeak = tpeak_min
        if (tpeak.gt.tpeak_max) tpeak = tpeak_max

* Special reinit every event; only uncomment when calling two_track_fit
        if (oldpass.ne.pass) then
          call mninit(5,22,7)
          print *, '******** Calling MNINIT with pass = ', pass
          oldpass = pass
        endif

        do j = -nstrips/2, nstrips/2
          strip = peak + j
          e(j) = rms(strip,layer) * rms(strip,layer)
*         print *, 'layer=', layer, ' strip=', strip, ' rms=', 
*    &     rms(strip,layer)
          do ts = 1, ntimesamples
            q(ts,j) = sca((tpeak-ntimesamples/2)+(ts-1),strip,layer) - 
     &       mean(strip,layer)
          enddo
        enddo

* Other layer-dependent parameters (uses small-angle approximation for stripw.)
        h = HEIGHT + h_adj(iseg+1,layer)
*       if (pass.eq.2) h = h + 0.2
*       if (pass.eq.3) h = h - 0.2
*       if (pass.eq.2) h = h + 0.5
*       if (pass.eq.3) h = h - 0.5

* S.D. 13 March 2003
*        stripwidth_r8 = (R_WG_0 + WG_WIDTH * wg) * STRIP_ANGLE
        stripwidth_r8 = strip_width
        
* Find the measured position using Minuit
        call xMinuit(x, a, t, f, xerr, aerr, terr, ferr, chisq8, status)

        if (status.eq.3) then

* Convert to real*4
          xclus = x
          qclus = a
          tclus = t
          fclus = f
          xerror = xerr
          chisq = chisq8

* Don't trust fits with xclus > 2
          if (xclus.gt.2.0) status = -1;

* Add position of peak strip (counting from 0)
          xclus = peak + xclus - 1

* Add fitted time to peak time (counting from 0)
          tclus = (tpeak - tclus) - 1
        endif

* If cluster was not fit, put default non-physical values 
        if (status.eq.0.or.status.eq.1) then
          xclus = 999.
          qclus = 999.
          tclus = 999.
          fclus = 999.
        endif
        return
        end

*-----------------------------------------------------------------------------

        subroutine trackfit(xclus, xerror, xalign, z, layer_ok, rlayer, 
     &   emm, bee, chisq, xtrack, resid, nlayers, trackfit_ok)
* Fit track; find residual. Trackfit subtracts the alignment corrections 
* plane_offset (stagger) and xalign from the xclus arguments, does the trackfit 
* and finds the residuals, then removes the alignment corrections from xtrack 
* before returning (so that xtrack can be directly compared with comparator
* and clusterfit coordinates).
        implicit none
 
* Arguments
        real*4        xclus(6), xerror(6), xalign(6), z(6)
        logical       layer_ok(6)
        integer*4     rlayer, nlayers
        real*4        bee, emm, chisq, xtrack(6), resid(6)
        logical*4     trackfit_ok
 
* Local constants
        real*4       plane_offset(6) / 0.0, +0.5, 0.0, +0.5, 0.0, +0.5 /
        save         plane_offset

* Local variables
        integer*4       ilayer, rl
        real*8          sum, sumx, sumx2, sumy, sumxy, x(6)
        real*8          denom, weight
 
        trackfit_ok = .false.
        sum = 0.
        sumx = 0.
        sumx2 = 0.
        sumy = 0.
        sumxy = 0.
        rl = rlayer + 1      ! calling routines count layers starting from 0
        
* Check that there are at least two layers in the fit
        nlayers = 0
        do ilayer = 1, 6
          if (ilayer.ne.rl.and.layer_ok(ilayer))
     &     nlayers = nlayers + 1
          
        enddo
        if (nlayers.le.1)then
*            print *,'### layer number too small ###'
           return
        endif

* Add alignment corrections
        do ilayer = 1, 6
          x(ilayer) = xclus(ilayer) - (plane_offset(ilayer) 
     &     + xalign(ilayer))
        enddo

* Skip rl, where we'll measure residuals, and any rejected layer
        do ilayer = 1, 6
          if (ilayer.ne.rl.and.layer_ok(ilayer)) then
            weight = 1. / (xerror(ilayer) * xerror(ilayer))
            sum = sum + 1. * weight
            sumx = sumx + z(ilayer) * weight
            sumx2 = sumx2 + z(ilayer) * z(ilayer) * weight
            sumy = sumy + x(ilayer) * weight
            sumxy = sumxy + z(ilayer) * x(ilayer) * weight
            print *,'### get into ok layer blablabla weight ###'
         else 
            print *,'### get NOT into ok layer blablabla weight ###'
          endif
        enddo
 
* Calculate residuals for all layers
        denom = sumx2 * sum - sumx * sumx
        if (denom.eq.0.) then
          print *, '### Denominator is zero in fit ###'
          chisq = -999.
          return
        endif
 
        emm = (sumxy * sum - sumx * sumy) / denom
        bee = (sumx2 * sumy - sumx * sumxy) / denom
 
        do ilayer = 1, 6
          xtrack(ilayer) = emm * z(ilayer) + bee
          if (layer_ok(ilayer)) then
            resid(ilayer) = x(ilayer) - xtrack(ilayer)
          else
            resid(ilayer) = -999
          endif
        enddo

* Remove the alignment corrections from xtrack
        do ilayer = 1, 6
          xtrack(ilayer) = xtrack(ilayer) + (plane_offset(ilayer) 
     &     + xalign(ilayer))
        enddo
        
* Calculate chi-squared of the fit
        sum = 0.
        do ilayer = 1, 6
          if (ilayer.ne.rl.and.layer_ok(ilayer)) then
            sum = sum + (resid(ilayer) / xerror(ilayer))**2
          endif
        enddo
        chisq = sum
        
        trackfit_ok = .true.
        return
        end

*------------------------------------------------------------------------------

        subroutine xMinuit (x, a, t, f, xerr, aerr, terr, ferr, chisq, 
     &   istat)
        implicit        none
* Arguments
        integer*4       istat
        real*8          x, a, t, f, xerr, aerr, terr, ferr, chisq

* Local constants
        real*8          zero/0./

* Minuit common block (used to pass data values to Minuit)
        integer*4       nstrips, nts
        parameter       (nstrips = 5)
        parameter       (nts = 5)
        common /stripdata/ q, e, h, k3, stripwidth
        real*8          q(nts, -nstrips/2: nstrips/2)
        real*8          e(-nstrips/2: nstrips/2)
        real*8          h, k3, stripwidth

* Local variables
        integer*4       k, lun/39/, status
        integer*4       npari, nparx
        real*8          edm, errdef
        real*8          arglis(10), bnd1, bnd2
        real*8          x_start/0.25/, x_step/0.10/
        real*8          a_start/100./, a_step/0.1/
        real*8          t_start/0./, t_step/0.10/
        real*8          f_start/1./, f_step/0.01/
        logical         first /.TRUE./
        character*10    name
        save            first

* Functions
        external        fcn

* MINUIT init
        if (first) then
          first = .FALSE.
          call mninit(5,lun,7)
*         open (unit=lun, file='xM.out', status='unknown',
*    &     form='formatted')
        endif

* Set print level
        arglis(1) = -1
*       arglis(1) = +1
        call mnexcm(fcn, 'SET PRINT', arglis, 1, status, 0.)

* Define starting parameter values
        a_start = 0.
        x_start = 0.
        t_start = 0.
        f_start = 1.
        do k = -nstrips/2, nstrips/2
          a_start = a_start + q(3,k)
          x_start = x_start + q(3,k) * k
        enddo
        x_start = x_start / a_start
*       a_start = 0.013 * a_start        ! empirical normalization
        a_start = 0.040 * a_start        ! empirical normalization
        call mnparm(1, 'x', x_start, x_step, zero, zero, status)
        call mnparm(2, 'a', a_start, a_step, zero, zero, status)
        call mnparm(3, 't', t_start, t_step, zero, zero, status)
        call mnparm(4, 'f', f_start, f_step, zero, zero, status)
*       print 6, x_start, a_start, t_start, f_start
*   6   format('Parameter initial values: ', 4e12.3)

* Initialize FCN
        arglis(1) = 1.
        call mnexcm(fcn, 'CALL', arglis, 1, status, 0.)

* Fix parameter 4
*       arglis(1) = +4
*       call mnexcm(fcn, 'FIX', arglis, 1, status, 0.)

* Do the fit
        call mnexcm(fcn, 'MINIMIZE', arglis, 0, status, 0.)

* Get the values and errors of x and a
        call mnpout(1, name, x, xerr, bnd1, bnd2, status)
        call mnpout(2, name, a, aerr, bnd1, bnd2, status)
        call mnpout(3, name, t, terr, bnd1, bnd2, status)
        call mnpout(4, name, f, ferr, bnd1, bnd2, status)

* Get the chisq and the status of the fit
        call mnstat(chisq, edm, errdef, npari, nparx, istat)

* (Debug) get dq for each strip, with final fit
*        arglis(1) = 3.
*        call mnexcm(fcn, 'CALL', arglis, 1, status, 0.)

*       print 7, x, a, t, f
*       print 8, xerr, aerr, terr, ferr
*   7   format('parameters :', 4e12.3)
*   8   format('errors     :', 4e12.3)
        return
        end

*------------------------------------------------------------------------------

      subroutine fcn (npar, grad, fval, xval, iflag) 
      implicit        none

* Local constants
      integer*4    nstrips, nts
      parameter    (nstrips = 5)
      parameter    (nts = 5)

      real*8       pi
      parameter    (pi=3.14159)

      real*8       t(90) /
     & -3.45, -3.35, -3.25, -3.15, -3.05, -2.95, -2.85, -2.75, -2.65, 
     & -2.55, -2.45, -2.35, -2.25, -2.15, -2.05, -1.95, -1.85, -1.75, 
     & -1.65, -1.55, -1.45, -1.35, -1.25, -1.15, -1.05, -0.95, -0.85, 
     & -0.75, -0.65, -0.55, -0.45, -0.35, -0.25, -0.15, -0.05,  0.05, 
     &  0.15,  0.25,  0.35,  0.45,  0.55,  0.65,  0.75,  0.85,  0.95, 
     &  1.05,  1.15,  1.25,  1.35,  1.45,  1.55,  1.65,  1.75,  1.85, 
     &  1.95,  2.05,  2.15,  2.25,  2.35,  2.45,  2.55,  2.65,  2.75, 
     &  2.85,  2.95,  3.05,  3.15,  3.25,  3.35,  3.45,  3.55,  3.65, 
     &  3.75,  3.85,  3.95,  4.05,  4.15,  4.25,  4.35,  4.45,  4.55, 
     &  4.65,  4.75,  4.85,  4.95,  5.05,  5.15,  5.25,  5.35,  5.45 /

      real*8       qs(90) /
     & 0.0000E+00, 0.0000E+00, 0.0000E+00, 0.0000E+00, 0.0000E+00, 
     & 0.0000E+00, 0.0000E+00, 0.0000E+00, 0.0000E+00, 0.0000E+00, 
     & 0.0000E+00, 0.4845E-02, 0.9717E-01, 0.4705E+00, 0.1335E+01, 
     & 0.2857E+01, 0.5129E+01, 0.8171E+01, 0.1194E+02, 0.1633E+02, 
     & 0.2121E+02, 0.2643E+02, 0.3182E+02, 0.3724E+02, 0.4252E+02, 
     & 0.4755E+02, 0.5222E+02, 0.5643E+02, 0.6013E+02, 0.6327E+02, 
     & 0.6584E+02, 0.6781E+02, 0.6920E+02, 0.7004E+02, 0.7035E+02, 
     & 0.7017E+02, 0.6954E+02, 0.6851E+02, 0.6712E+02, 0.6543E+02, 
     & 0.6349E+02, 0.6133E+02, 0.5901E+02, 0.5655E+02, 0.5401E+02, 
     & 0.5141E+02, 0.4878E+02, 0.4615E+02, 0.4354E+02, 0.4098E+02, 
     & 0.3848E+02, 0.3605E+02, 0.3370E+02, 0.3144E+02, 0.2929E+02, 
     & 0.2724E+02, 0.2529E+02, 0.2345E+02, 0.2172E+02, 0.2009E+02, 
     & 0.1857E+02, 0.1715E+02, 0.1582E+02, 0.1460E+02, 0.1346E+02, 
     & 0.1240E+02, 0.1143E+02, 0.1054E+02, 0.9719E+01, 0.8965E+01, 
     & 0.8275E+01, 0.7644E+01, 0.7068E+01, 0.6543E+01, 0.6065E+01, 
     & 0.5630E+01, 0.5236E+01, 0.4878E+01, 0.4554E+01, 0.4261E+01, 
     & 0.3996E+01, 0.3757E+01, 0.3541E+01, 0.3347E+01, 0.3172E+01, 
     & 0.3014E+01, 0.2873E+01, 0.2746E+01, 0.2632E+01, 0.2529E+01 /

      real*8       qx(90) /
     & 0.0000E+00, 0.0000E+00, 0.0000E+00, 0.0000E+00, 0.0000E+00, 
     & 0.0000E+00, 0.0000E+00, 0.0000E+00, 0.0000E+00, 0.0000E+00, 
     & 0.0000E+00, 0.6790E-03, 0.2401E-01, 0.1321E+00, 0.3888E+00, 
     & 0.8338E+00, 0.1475E+01, 0.2294E+01, 0.3247E+01, 0.4283E+01, 
     & 0.5343E+01, 0.6370E+01, 0.7314E+01, 0.8135E+01, 0.8802E+01, 
     & 0.9294E+01, 0.9602E+01, 0.9727E+01, 0.9674E+01, 0.9457E+01, 
     & 0.9093E+01, 0.8604E+01, 0.8010E+01, 0.7334E+01, 0.6599E+01, 
     & 0.5825E+01, 0.5032E+01, 0.4237E+01, 0.3455E+01, 0.2698E+01, 
     & 0.1977E+01, 0.1300E+01, 0.6732E+00, 0.1003E+00, -.4159E+00, 
     & -.8746E+00, -.1276E+01, -.1621E+01, -.1913E+01, -.2153E+01, 
     & -.2346E+01, -.2494E+01, -.2603E+01, -.2674E+01, -.2714E+01, 
     & -.2724E+01, -.2709E+01, -.2673E+01, -.2619E+01, -.2550E+01, 
     & -.2468E+01, -.2377E+01, -.2279E+01, -.2175E+01, -.2067E+01, 
     & -.1958E+01, -.1848E+01, -.1738E+01, -.1630E+01, -.1525E+01, 
     & -.1422E+01, -.1323E+01, -.1228E+01, -.1138E+01, -.1051E+01, 
     & -.9696E+00, -.8925E+00, -.8200E+00, -.7520E+00, -.6885E+00, 
     & -.6294E+00, -.5744E+00, -.5235E+00, -.4763E+00, -.4329E+00, 
     & -.3928E+00, -.3560E+00, -.3223E+00, -.2914E+00, -.2631E+00 /

      real*8       qxx(90) /
     & 0.0000E+00, 0.0000E+00, 0.0000E+00, 0.0000E+00, 0.0000E+00, 
     & 0.0000E+00, 0.0000E+00, 0.0000E+00, 0.0000E+00, 0.0000E+00, 
     & 0.0000E+00, 0.0000E+00, 0.3548E-05, 0.3702E-02, 0.3614E-01, 
     & 0.1271E+00, 0.2878E+00, 0.5085E+00, 0.7653E+00, 0.1028E+01, 
     & 0.1268E+01, 0.1463E+01, 0.1595E+01, 0.1657E+01, 0.1649E+01, 
     & 0.1576E+01, 0.1445E+01, 0.1271E+01, 0.1063E+01, 0.8361E+00, 
     & 0.6005E+00, 0.3665E+00, 0.1424E+00, -.6539E-01, -.2523E+00, 
     & -.4155E+00, -.5534E+00, -.6658E+00, -.7534E+00, -.8176E+00, 
     & -.8605E+00, -.8842E+00, -.8912E+00, -.8841E+00, -.8651E+00, 
     & -.8368E+00, -.8011E+00, -.7600E+00, -.7151E+00, -.6680E+00, 
     & -.6199E+00, -.5717E+00, -.5243E+00, -.4784E+00, -.4344E+00, 
     & -.3927E+00, -.3535E+00, -.3170E+00, -.2833E+00, -.2522E+00, 
     & -.2238E+00, -.1981E+00, -.1747E+00, -.1537E+00, -.1349E+00, 
     & -.1181E+00, -.1031E+00, -.8984E-01, -.7810E-01, -.6776E-01, 
     & -.5867E-01, -.5070E-01, -.4374E-01, -.3766E-01, -.3238E-01, 
     & -.2779E-01, -.2381E-01, -.2038E-01, -.1741E-01, -.1485E-01, 
     & -.1266E-01, -.1077E-01, -.9153E-02, -.7769E-02, -.6587E-02, 
     & -.5578E-02, -.4719E-02, -.3987E-02, -.3366E-02, -.2838E-02 /

* Input arguments
      integer*4    npar, iflag
      real*8       xval(*)

* Output arguments
      real*8       fval, grad(*)

* Minuit common block (used to pass data values to Minuit)
      real*8       q(nts, -nstrips/2: nstrips/2)
      real*8       e(-nstrips/2: nstrips/2)
      real*8       h, k3, stripwidth
      common /stripdata/ q, e, h, k3, stripwidth

* Local variables
      integer*4    j, k, strip, ts
      real*8       sf, total, x0, x1
      real*8       dt/0.1/, f, k1, k2, sqrtk3, ti, tf

      real*8       pulseshape(nts,0:4)
      real*8       qgatti(nts, -nstrips/2: nstrips/2)
      real*8       qtmp(-nstrips/2: nstrips/2)
      logical      initialized/.false./
      save         f, initialized, k1, k2, pulseshape, sqrtk3
*      save qgatti, sf

* Initialize if iflag=1, or (optionally) calculate grad if iflag=2
      if (iflag.eq.1) then
        if (.not.initialized) then
          initialized = .true.
          sqrtk3 = sqrt(k3)
          k2 = .5*pi*(1.-.5*sqrtk3)
          k1 = (k2*sqrtk3)/(4.*atan(sqrtk3))
          f = k1 / (k2 * sqrtk3)

          do k = 0, 4
            do ts = 1, nts
              pulseshape(ts,k) = 0
            enddo
          enddo
        endif
      endif

* Calculate fcn (always)
* Find total charge on each strip (from Gatti formula)
      total = 0.
      do strip = -nstrips/2, nstrips/2
        x0 = ((strip-0.5)-xval(1)) * k2 * stripwidth / h
        x1 = ((strip+0.5)-xval(1)) * k2 * stripwidth / h
        qtmp(strip) = f*(atan(sqrtk3*tanh(x1))-atan(sqrtk3*tanh(x0)))
        total = total + qtmp(strip)
      enddo

* Find peak and nearest-neighbor crosstalk shapes for current value of t=xval(3)
*      do k = 0, 1
*        do ts = 1, nts
**          ti = (ts-3) + xval(3)
*          ti = xval(4) * ((ts-3) + xval(3))
*          pulseshape(ts,k) = c(0,k) + c(1,k)*ti + c(2,k)*ti**2 + 
*     &     c(3,k)*ti**3 + c(4,k)*ti**4 + c(5,k)*ti**5 + 
*     &     c(6,k)*ti**6 + c(7,k)*ti**7
*        enddo
*      enddo

      do ts = 1, nts
*       ti = (ts-3) + xval(3)
        ti = xval(4) * ((ts-3) + xval(3))
        j = 1 + (ti - t(1)) / dt
        if (j.lt.1 .or. j.gt.89) then
          pulseshape(ts,0) = 0
          pulseshape(ts,1) = 0
          pulseshape(ts,2) = 0
        else
          tf = (ti - t(j)) / dt
          pulseshape(ts,0) = qs(j) + tf * (qs(j+1) - qs(j))
          pulseshape(ts,1) = qx(j) + tf * (qx(j+1) - qx(j))
          pulseshape(ts,2) = qxx(j) + tf * (qxx(j+1) - qxx(j))
        endif

*        print 88, ts, j, ti, (pulseshape(ts,k), k=0,2)
*  88    format('ts=', i1, ' j=', i3, ' ti=', f8.3, ' pulseamps: ', 
*    &    3e12.3)
      enddo

* For each strip, add crosstalk proportional to its charge to neighbor strips
      do strip = -nstrips/2, nstrips/2
        do ts = 1, nts
          qgatti(ts,strip) = 0
        enddo

        do j = -nstrips/2, nstrips/2
          k = abs(strip-j)
          do ts = 1, nts
            qgatti(ts,strip) = qgatti(ts,strip) + 
     &       pulseshape(ts,k) * qtmp(j)
          enddo
        enddo
      enddo
 
* Calculate chisq
      fval = 0.
      sf = xval(2) / total
      do strip = -nstrips/2, nstrips/2
        do ts = 1, nts
          fval = fval + 
     &     (sf * qgatti(ts,strip) - q(ts,strip))**2 / e(strip)
        enddo
      enddo

* Last iflag possibility
      if (iflag.eq.3) then    ! finish
        sf = xval(2) / total
        do strip = -nstrips/2, nstrips/2
          print 11, strip, (q(ts,strip),ts=1,nts)
   11     format('q_data for s', i3, '=', 5f8.2)
          print 12, strip, (sf*qgatti(ts,strip),ts=1,nts)
   12     format('qgatti for s', i3, '=', 5f8.2)
        enddo
      endif

      return
      end

*******************************************************************************

*
* Subroutine to find the layer alignment; input is the mean offsets of the
* trackfit residuals in each layer as a function of wiregroup number.
* (Done in Fortran because some of the hbook calls don't seem to work in the
* C version.) Returns alignment constants c and var, the chisq/ndf of the
* trackfit used to get the alignment constants.
* First version:  Feb. 26, 2001
*

      subroutine strip_alignment(c, var, nwires)
      implicit none
* Global variables
      include 'csc_parameters.inc'            ! For NLAYER, NWIRE

* Arguments
      real*4       c(2,NLAYER), var(NLAYER)
      integer*4    nwires
* Local variables
      integer*4    hid, key, layer, wg
      real*4       param(3), step(3), pmin(3), pmax(3), sigpar(3), chisq
      real*4       x(NWIRES_MAX), y(NWIRES_MAX), w(NWIRES_MAX)

*     common / myparam / param
*     real*4       param(3)

* Initialization
      key = 1
      do wg = 1, nwires
        x(wg) = wg
      enddo

* Loop over layers
      do layer = 1, 6

* Loop over wiregroups, doing a Gaussian fit to the trackfit residuals
        do wg = 1, nwires
          hid = 1000 + 120 * layer + wg

          call hfithn(hid, 'G', 'Q', 0, param, step, pmin, pmax, sigpar, 
     &     chisq)

          y(wg) = param(2)
          w(wg) = 1 / sigpar(2)
        enddo

* Do linear fit to the means of the Gaussian fit vs wiregroup
        call lfitw(x, y, w, nwires, key, c(2,layer), c(1,layer), 
     &   var(layer))

      enddo

      return
      end

* A little bit modified version of strip_alignment subroutine

      subroutine strip_alignment_x(x_wg, c, var, nwires)
      implicit none
* Global variables
      include 'csc_parameters.inc'            ! For NLAYER, NWIRE

* Arguments
      real*4       x_wg(NWIRES_MAX)
      real*4       c(2,NLAYER), var(NLAYER)
      integer*4    nwires
* Local variables
      integer*4    hid, key, layer, wg
      real*4       param(3), step(3), pmin(3), pmax(3), sigpar(3), chisq
      real*4       x(NWIRES_MAX), y(NWIRES_MAX), w(NWIRES_MAX)

*     common / myparam / param
*     real*4       param(3)

* Initialization
      key = 1
      do wg = 1, nwires
        x(wg) = x_wg(wg)
      enddo

* Loop over layers
      do layer = 1, 6

* Loop over wiregroups, doing a Gaussian fit to the trackfit residuals
        do wg = 1, nwires
          hid = 1000 + 120 * layer + wg

          call hfithn(hid, 'G', 'Q', 0, param, step, pmin, pmax, sigpar, 
     &     chisq)

          y(wg) = param(2)
          w(wg) = 1 / sigpar(2)
        enddo

* Do linear fit to the means of the Gaussian fit vs wiregroup
        call lfitw(x, y, w, nwires, key, c(2,layer), c(1,layer), 
     &   var(layer))

      enddo

      return
      end
