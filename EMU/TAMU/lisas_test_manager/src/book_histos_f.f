
*-----------------------------------------------------------------------------
C
C  Book histograms here for which C-language booking doesn't compile
C  (ie profile histograms)
C
*-----------------------------------------------------------------------------

      subroutine book_histos_14f(nwires)
      implicit none

* Global constants
      include 'csc_parameters.inc'            ! For NLAYER, NWIRE
      integer*4    nwires

* Local variables
      integer*4       hid, layer, wg
      character*40    htitle

* Input for ALCT delay calibration
      do layer = 1, NLAYER
*        do wg = 1, NWIRE
        do wg = 1, nwires
          hid = 120 * layer + wg
          write(htitle, 10) layer, wg
   10     format('L', i1, ' wg' , i3, ' time (25 ns clocks) vs delay ',
     &     'code')
          call hbprof(hid, htitle, 16, -0.5, 15.5, 0., 500., ' ')
        enddo
      enddo

      return
      end

*-----------------------------------------------------------------------------

      subroutine book_histos_15f
      implicit none  

* Global constants
      include 'csc_parameters.inc'            ! For NSTRIP, NLAYER

* Local variables
      integer*4       hid, layer, strip
      character*40    htitle

* SCA capacitor cells 
      do layer = 1, 6
        do strip = 1, NSTRIP
          hid = 1000 + 100 * layer + strip
          write(htitle, 10) layer, strip
   10     format('L', i1, ' strip' , i2, ' peds vs SCA cell')
          call hbprof(hid, htitle, 96, 0., 96., -50., 50., ' ')
        enddo
      enddo

* Pedestal timesample dependence
      do layer = 1, 6
        do strip = 1, NSTRIP
          hid = 2000 + 100 * layer + strip
          write(htitle, 20) layer, strip
   20     format('L', i1, ' strip' , i2, ' peds vs timebin')
          call hbprof(hid, htitle, 16, 0., 16., -50., 50., ' ')
        enddo
      enddo

* Pedestal stability 
      do layer = 1, 6
        do strip = 1, NSTRIP
          hid = 3000 + 100 * layer + strip
          write(htitle, 30) layer, strip
   30     format('L', i1, ' strip' , i2, ' peds vs event number')
          call hbprof(hid, htitle, 50, 0., 50000., -50., 50., ' ')
        enddo
      enddo

      return
      end

*-----------------------------------------------------------------------------

      subroutine book_histos_17f(scan_mode, npoints, xmin, xmax)
      implicit none  

* Global constants
      include 'csc_parameters.inc'            ! For NSTRIP, NLAYER

* Arguments
      integer*4    scan_mode, npoints
      real*4       xmin, xmax

* Local variables
      integer*4       FIRST_STRIP
      parameter       (FIRST_STRIP = 1)

* Local variables
      integer*4       hid, layer, strip
      character*50    htitle

      if (scan_mode .eq. 2) then
* Mean pulse heights, all strips
        call hbprof(7, 'mean peak SCA vs test pulse DAC', 
     &   npoints, xmin, xmax, -10., 4100., ' ')

* Calibration input: pulsed strip ADC values
        do layer = 1, NLAYER
          do strip = FIRST_STRIP, NSTRIP
            hid = 1000 + 100 * layer + strip
            write(htitle, 10) layer, strip
   10       format('L', i1, ' s' , i2, ' peak strip, ADC vs DAC')
            call hbprof(hid, htitle, npoints, xmin, xmax, -1., 4100., 
     &       'I')
          enddo
        enddo

* Calibration input: ADC values of strips far ( >2 strips) from pulsed strip 
        do layer = 1, NLAYER
          do strip = FIRST_STRIP, NSTRIP
            hid = 2000 + 100 * layer + strip
            write(htitle, 20) layer, strip
   20       format('L', i1, ' s' , i2, ' away from peak, ADC vs DAC')
            call hbprof(hid, htitle, npoints, xmin, xmax, -1., 4100., 
     &       'I')
          enddo
        enddo

      else if (scan_mode .eq. 3) then
* Extended pulse shapes
        do layer = 1, NLAYER
          do strip = FIRST_STRIP, NSTRIP
            hid = 5000 + 100 * layer + strip
            write(htitle, 50) layer, strip
   50       format('L', i1, ' s' , i2, ' ADC vs timesample')
            call hbprof(hid, htitle, 160, 0., 160., -1., 4100., 'I')
          enddo
        enddo
        do layer = 1, NLAYER
          do strip = FIRST_STRIP, NSTRIP
            hid = 6000 + 100 * layer + strip
            write(htitle, 60) layer, strip
   60       format('L', i1, ' s' , i2, ' left xt ADC vs timesample')
            call hbprof(hid, htitle, 160, 0., 160., -1., 4100., 'I')
          enddo
        enddo

        do layer = 1, NLAYER
          do strip = FIRST_STRIP, NSTRIP
            hid = 7000 + 100 * layer + strip
            write(htitle, 70) layer, strip
   70       format('L', i1, ' s' , i2, ' right xt ADC vs timesample')
            call hbprof(hid, htitle, 160, 0., 160., -1., 4100., 'I')
          enddo
        enddo
      endif
      return
      end

*-----------------------------------------------------------------------------

      subroutine book_histos_20f
      implicit none  

* Global constants
      include 'csc_parameters.inc'            ! For NSTRIP, NLAYER

* Arguments

* Local variables
      integer*4       FIRST_STRIP
      parameter       (FIRST_STRIP = 1)

* Local variables
      integer*4       hid, layer, strip
      character*40    htitle

* Calibration input: pulsed strip ADC values
      do layer = 1, NLAYER
        do strip = FIRST_STRIP, NSTRIP
          hid = 1000 + 100 * layer + strip
          write(htitle, 10) layer, strip
   10     format('L', i1, ' strip ' , i2, ' hit time (25 ns bins)')
          call hbprof(hid, htitle, 16, -0.5, 15.5, -1., 8., 'I')
        enddo
      enddo

      return
      end

*-----------------------------------------------------------------------------

      subroutine book_histos_22f(npoints, xmin, xmax)
      implicit none  

* Global constants
      include 'csc_parameters.inc'            ! For NSTRIP, NLAYER

* Arguments
      integer*4    npoints
      real*4       xmin, xmax

* Local variables
      integer*4       FIRST_STRIP
      parameter       (FIRST_STRIP = 1)

* Local variables
      integer*4       hid, layer, strip
      character*40    htitle

* Profile histogram of compator response asymmetry vs test pulse amplitude
      do layer = 1, NLAYER
        do strip = FIRST_STRIP, NSTRIP
          hid = 100 * layer + strip
          write(htitle, 10) layer, strip
   10     format('L', i1, ' s ' , i2, ' Comp L/R offset vs DAC')
          call hbprof(hid, htitle, npoints, xmin, xmax, -21., 21., ' ')
        enddo
      enddo

* Reserve storage space for errors
      call HBARX(0)

      return
      end

*-----------------------------------------------------------------------------

      subroutine book_histos_27f(pass, stage, nwires)
      implicit none  

* Global constants
      include 'csc_parameters.inc'            ! For NSTRIP, NLAYER

* Arguments
      integer*4       pass, stage

* Local variables
      integer*4       hid, icfeb, iseg, layer
      integer*4       s1, s2
      integer*4       nwires
      character*40    htitle

C  Stage = 0 for histograms filled event-by-event, 1 for histograms filled
C  during end-of-run (pass) analysis.

* Long-range crosstalk *
      if (pass.eq.2.and.stage.eq.0) then
        do layer = 1, 6
          do icfeb = 1, 5
            hid = 200 + 10 * layer + icfeb
            write(htitle, 10) layer, icfeb
   10       format('L', i1, ' cfeb ', i1, ' qxt vs qclus, same cfeb')
            call hbprof(hid, htitle, 50, 0., 10000., -150., 150., ' ')

            hid = 600 + 10 * layer + icfeb
            write(htitle, 11) layer, icfeb
   11       format('L', i1, ' cfeb ', i1, ' qxt vs qclus, diff cfeb')
            call hbprof(hid, htitle, 50, 0., 10000., -150., 150., ' ')
          enddo
        enddo
      endif

* Clusterfit residuals *
      if ((pass.eq.2.and.stage.eq.1).or.(pass.eq.3.and.stage.eq.0)) then
        do layer = 1, 6
          do iseg = 1, 5
            hid = 300 + 10 * layer + iseg
            write(htitle, 20) layer, iseg
   20       format('L', i1, ' HV seg', i1, ' residuals vs xtrack')
            call hbprof(hid, htitle, 50, -0.5, 0.5, -0.4, 0.4, 'S')
          enddo
        enddo
      endif

* Anode timing *
      if (pass.eq.2.and.stage.eq.0) then
        do layer = 1, 6
          hid = 60 + layer
          write(htitle, 30) layer
   30     format('L', i1, ' wire hit time vs wg')
          call hbprof(hid, htitle, nwires, 0.,
     &     float(nwires), 0., 8., ' ')
        enddo
      endif

* Comparator residuals *
      if (pass.eq.3.and.stage.eq.0) then
        do layer = 1, 6
          hid = 160 + layer
          write(htitle, 40) layer
   40     format('L', i1, ' comparator residuals vs hs')
          call hbprof(hid, htitle, 160, 0., 2.*NSTRIP, -5.0, 5.0, ' ')
        enddo
      endif

* Scintillator timing (not part of standard test 27 analysis) * 
      if (pass.eq.2.and.stage.eq.0) then
        do s1 = 1, 12
          do s2 = s1 - 1, s1 + 1
            if (s2.ge.1.and.s2.le.12) then
              hid = 40000 + (100 * s1) + s2
              write(htitle, 50) s1, s2
   50         format('S', i2, ' - S', i2, ' delta-t diff, ns, vs phi')
              call hbprof(hid, htitle, 100, -1., 1., -100., 100., ' ')
            endif
          enddo
        enddo
      endif

      return
      end
