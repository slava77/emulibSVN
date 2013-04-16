
      subroutine one_page_plot_v3(test_num, data, error, type, limit, 
     & cscname, timestring, plot_title, summary_title, filename,
     & daq_version1, anal_version1, user_name1, site1, 
     & number_of_run1, run_date1, testname1, flag)
      implicit     none


*      include 'csc_parameters.inc'
      include 'csc_event.inc'
* Global variables
      INTEGER ERRFIL,WKID,WTYPE,CONID
      COMMON/WKID/ERRFIL,WKID,WTYPE,CONID

      real*4  rquest(100)
      common /quest/ rquest

* Arguments
      integer*4        type, test_num, flag
      real*4           limit(*), data(*), error(*)
      character*(*)    cscname, timestring
      character*(*)    plot_title, summary_title, filename
      character*(*)    daq_version1, anal_version1, user_name1, site1  
      character*(*)    number_of_run1, run_date1, testname1
* Local constants
      integer*4        NSTRIPS, NWIRES, NLAYERS
      parameter        (NLAYERS =  6)
*      parameter        (NSTRIPS = 80, NWIRES = 64)
      integer*4        NBINS
      parameter        (NBINS = 50)
      real*4           STRIP_PLOT, WIRE_PLOT, HSTRIP_PLOT, GAIN_PLOT
      real*4           MSTRIP_PLOT, MWIRE_PLOT, MHSTRIP_PLOT
      real*4           STRIP_PLOT_ODD, STRIP_PLOT_EVEN
      parameter        (STRIP_PLOT = 1)
      parameter        (WIRE_PLOT = 2)
      parameter        (HSTRIP_PLOT = 3)
      parameter        (MSTRIP_PLOT = 4) ! for multiple planes trigger
      parameter        (MWIRE_PLOT = 5)
      parameter        (MHSTRIP_PLOT = 6)
      parameter        (GAIN_PLOT = 7)
      parameter        (STRIP_PLOT_ODD = 8)
      parameter        (STRIP_PLOT_EVEN = 9)

* Local variables
      integer*4        i, il, j, bin, lun/17/, ndiv, layer
      integer*4        num_xbins, offset
      integer*4        n, nentries, noverflow, nunderflow
      real*4           cmax, r, xleft, ybottom, limit2(2), rval(4)
      real*4           xws(2), x1, x2, y1, y2, xl(2), yl(2)
      real*4           contents(NBINS)
      real*4           xbin(160), xex(160), yex(160), ydat(160)
      real*4           mean, rms, sum, xmin, xmax, ymin, ymax
      real*4           lower_limit_fraction, upper_limit_fraction
      logical          first/.TRUE./
*      character*9      limit6
      character*9      label, limit5, points_b_char 
      character*20     string
      character*39     title1, title2, title3
      real*4           delta_upper1,delta_upper2
      real*4           delta_lower,step_lower        
      integer*4        points_beyond

* Initialize HIGZ
*      call init_display 
*   66 continue
*      print *, 'Finished init_display'
      NWIRES = 64
      NSTRIPS = 80
* if strip plot then limit(7) carries number of strips to plot
      if (type.eq.STRIP_PLOT .or. type.eq.MSTRIP_PLOT .or.
     &    type.eq.STRIP_PLOT_EVEN .or. type.eq.STRIP_PLOT_ODD .or. 
     &    type.eq.HSTRIP_PLOT) then
         NSTRIPS = limit(7)
         if (NSTRIPS.ne.80 .and. NSTRIPS.ne.64) then
            NSTRIPS = 80
         endif
      else
* a different kind of strip plot. NSTRIPS is in limits(20)
         if (type.eq.MHSTRIP_PLOT) then
            NSTRIPS = limit(20)
            if (NSTRIPS.ne.80 .and. NSTRIPS.ne.64) then
               NSTRIPS = 80
            endif
         else

* otherwise this is a wire plot, and limit(7) contains number of wires, 
* or (HV segments * number of CFEBs) sometimes (see below)
            NWIRES = limit(7);
            if (NWIRES.ne.32.and.NWIRES.ne.48.and.NWIRES.ne.64.and.
     &           NWIRES.ne.96.and.NWIRES.ne.112.and.NWIRES.ne.5.and.
     &           NWIRES.ne.12.and.NWIRES.ne.15.and.NWIRES.ne.25) then
               NWIRES = 64
            endif
         endif
      endif

      if (first) then
        CALL IGINIT(0)
         errfil = 44
*        open(unit=errfil, file='higz.messages', form='formatted', 
*     &   status='unknown')
        call iopks(errfil)
        wkid = 1
        wtype = 2
        conid = 22
        first = .false.
      endif
*      call igmeta(-30,-111)
*      print *, 'Finished igmeta'

* Set up x-axis
      if (type.eq.STRIP_PLOT .or. type.eq.MSTRIP_PLOT) then
        num_xbins = NSTRIPS
      else if (type.eq.STRIP_PLOT_EVEN .or. type.eq.STRIP_PLOT_ODD) then
        num_xbins = NSTRIPS
      else if (type.eq.WIRE_PLOT .or. type.eq.MWIRE_PLOT) then
        num_xbins = NWIRES
      else if(type.eq.HSTRIP_PLOT .or. type.eq.MHSTRIP_PLOT) then
        num_xbins = 2 * NSTRIPS
      else if(type.eq.GAIN_PLOT) then
*        num_xbins = 5 * (NSTRIPS/16)
* NWIRES actually carries number of HV segments - 5, 3 or 1, 
* multiplied by number of CFEBS,
* so possible values are 5, 12, 15, 25
        num_xbins = NWIRES
      endif

      do j = 1, num_xbins
        xbin(j) = j
      enddo

* Fill contents array for summary histogram
      do bin = 1, NBINS
        contents(bin) = 0
      enddo
      nentries = 0

* Fill histogram with all data, except only "layer" 4 for Mxxxxxx plots.
      do j = 1, num_xbins * NLAYERS
        layer = 1 + (j - 1) / num_xbins
        bin = ((data(j) - limit(3)) / (limit(4) - limit(3))) * NBINS + 1
        if ((type.ne.MSTRIP_PLOT.and.type.ne.MHSTRIP_PLOT.and.
     &   type.ne.MWIRE_PLOT).or.layer.eq.4) then
          nentries = nentries + 1
          if (bin.ge.1.and.bin.le.NBINS) contents(bin) = 
     &     contents(bin) + 1
        endif
      enddo

* Find the max bin entries (for y-scale of summary plot) 
      cmax = 1.
      do bin = 1, NBINS
        if (contents(bin).gt.cmax) cmax = contents(bin)
      enddo

* Find histogram statistics
      sum = 0.
      n = 0
      do bin = 1, NBINS
        sum = sum + contents(bin) * (limit(3) + (limit(4) - limit(3)) * 
     &   (bin - 0.5) / NBINS)
        n = n + contents(bin)
      enddo
      mean = 0
      if (n.gt.0) mean = sum / n
      

      noverflow = 0
      nunderflow = 0
      sum = 0.

      do j = 1, num_xbins * NLAYERS
        layer = 1 + (j - 1) / num_xbins
        if ((type.ne.MSTRIP_PLOT.and.type.ne.MHSTRIP_PLOT.and.
     &   type.ne.MWIRE_PLOT).or.layer.eq.4) then
          if (data(j).lt.limit(3)) then
            nunderflow = nunderflow + 1
          else if (data(j).gt.limit(4)) then
            noverflow = noverflow + 1
          else 
            sum = sum + (mean - data(j))**2
          endif
        endif
      enddo

      if (nentries.gt.0) then
        rms = sqrt(sum / nentries)
      else
        rms = 0
      endif

* Open a postscript file
      open(unit=lun, file=filename, form='formatted', 
     & status='unknown', err=100)
      call igqwk(wkid, 'OPEN', rval)
      if (rval(1).ne.1) call iopwk(wkid, lun, -100112)
      call igqwk(wkid, 'ACTI', rval)
      if (rval(1).ne.1) call iacwk(wkid)
*     call igmeta(-1*lun, -1111)
*     call igmeta(-1*lun, -100112)
      
* Get some info
      call igrng(25.,19.)
*     print 2, rquest(11), rquest(12), rquest(13), rquest(14), 
*    & rquest(15) 
*   2 format('After igrng call, rquest=', 5f7.3)
      r = rquest(11)
      xleft = rquest(12)
      ybottom = rquest(13)
      call igqwk(wkid,'MXDS',xws)
*     print 3, wkid, xws(1), xws(2) 
*   3 format('Current workstation has id', i3, ' and xmax=', f7.1, 
*    & ', ymax=', f7.1)
*     call igqwk(wkid,'NTVP',rval)
*     print 6, rval
*   6 format('Current viewport parameters are:', 4f7.2)
*     call igqwk(wkid,'NTWN',rval)
*     print 4, rval
*   4 format('Current window_parameters are:', 4f7.2)

      call igset('FASI',244.)
      call igset('BORD',1.)
      call igset('CHHE',.05)

* Trial display
      call iswkvp(wkid,0.,xws(1),0.,xws(2))     ! use maximum available space
      call iclrwk(wkid,1)

* Make a box around the useable space
      x1 = 0.00
      x2 = 1.00
      y1 = 0.00
      y2 = 1.00
      call iselnt(0)
*     call igbox(x1,x2,y1,y2)

* Make boxes and plot data for each of the layers
      x1 = -0.5
      x2 = num_xbins + 0.5

* Really, we should allow all limits to be set independently, but I don't
* want to modify Kanglin's code (which uses an 18-element array) right now.
* So this is a temporary fix.
      if (type.eq.MSTRIP_PLOT.or.type.eq.MWIRE_PLOT.or.
     & type.eq.MHSTRIP_PLOT) then
        lower_limit_fraction = limit(5)
        upper_limit_fraction = limit(6)
      endif

      do layer = 1, NLAYERS
        if (type.eq.MSTRIP_PLOT.or.type.eq.MWIRE_PLOT.or.
     &   type.eq.MHSTRIP_PLOT) then
          limit(1) = limit(2*layer+5 +1)
          limit(2) = limit(2*layer+6 +1)
          limit(5) = limit(2) * lower_limit_fraction
          limit(6) = limit(2) * upper_limit_fraction
        endif

* One more temporaqry fix ( I believe all temporary fixes live forever)
* The case of Odd/ Even planes
        if (type.eq.STRIP_PLOT_ODD) then
           il = MOD(layer+1,2) + 1
          limit(1) = limit(6*il+1)
          limit(2) = limit(6*il+2)
          limit(3) = limit(6*il+3)
          limit(4) = limit(6*il+4)
          limit(5) = limit(6*il+5)
          limit(6) = limit(6*il+6)
        endif
        if (type.eq.STRIP_PLOT_EVEN) then
           il = MOD((layer),2) + 1
          limit(1) = limit(6*il+1)
          limit(2) = limit(6*il+2)
          limit(3) = limit(6*il+3)
          limit(4) = limit(6*il+4)
          limit(5) = limit(6*il+5)
          limit(6) = limit(6*il+6)
        endif
        
        y1 = limit(1)
        y2 = limit(2)

        call iswn(20+layer, x1, x2, y1, y2)

*       xmin =  1.5 * r + xleft
*       xmax = 15.5 * r + xleft
        xmin =  1.7 * r + xleft
        xmax = 15.7 * r + xleft
*       ymin = (3.0 + 2.5 * (layer-1)) * r + ybottom
*       ymax = (3.0 + 2.5 * (layer  )) * r + ybottom
        ymin = (3.0 + 2.5 * ((7-layer)-1)) * r + ybottom
        ymax = (3.0 + 2.5 * ((7-layer)  )) * r + ybottom
        ndiv = 1605

        call isvp(20+layer, xmin, xmax, ymin, ymax)
        call iselnt(20+layer)

        call igbox(x1,x2,y1,y2)
* x-axis
        call igset('LAOF',  0.125 * (y2-y1))
        call igset('LASI',  0.100 * (y2-y1))
        call igset('TMSI',  0.100 * (y2-y1))

        if (layer.eq.6) then
          call igaxis(x1, x2, y1, y1, x1, x2,  ndiv, 'DHS')
        else
          call igaxis(x1, x2, y1, y1, x1, x2, ndiv, 'UDHS')
        endif
* y-axis
        ndiv = 205
*       call igset('LAOF', 1.0)
        call igset('LAOF', 0.3)
        call igset('LASI', 0.100 * (y2 - y1))
        call igset('TMSI', 0.75)
        call igaxis(x1, x1, y1, y2, y1+0.01*(y2-y1), y2, ndiv, 'DHS')

* limit lines
        call isln(12)         ! Dashed lines
        xl(1) = x1
        xl(2) = x2
        yl(1) = limit(5)
        yl(2) = limit(5)
        call ipl(2,xl,yl)

        yl(1) = limit(6)
        yl(2) = limit(6)
        call ipl(2,xl,yl)
        call isln(1)         ! Back to solid lines

* Plot the data
        offset = (layer - 1) * num_xbins

        do j = 1, num_xbins
          if (data(j+offset).lt.limit(1)) then
            ydat(j) = limit(1) - 1
          else if (data(j+offset).gt.limit(2)) then
            ydat(j) = limit(2) + 1
          else
            ydat(j) = data(j+offset)
          endif
        enddo

        call ismk(20)
        call ismksc(.40)
        call ispmci(1)
        call ipm(num_xbins, xbin, ydat)

* Plot the errors
        if (iand(flag,1).eq.1) then
          offset = (layer - 1) * num_xbins
          call isplci(1)

          do j = 1, num_xbins
            xl(1) = xbin(j) 
            xl(2) = xbin(j)
            if (ydat(j).gt.limit(2).or.ydat(j).lt.limit(1)) then
              yl(1) = 0.
              yl(2) = 0.
            else
              yl(1) = data(j+offset) - error(j+offset)
              yl(2) = data(j+offset) + error(j+offset)
              do i = 1, 2
                if (yl(i).lt.limit(1)) yl(i) = limit(1) - 1
                if (yl(i).gt.limit(2)) yl(i) = limit(2) + 1
              enddo
            endif
            call ipl(2,xl,yl)
          enddo
        endif

* Plot underflow
        i = 0
        do j = 1, num_xbins
          if (data(j+offset).lt.limit(1)) then
            i = i + 1
            xex(i) = xbin(j)
            yex(i) = limit(1) + (limit(2)-limit(1))/30
          endif
        enddo
        call ismk(23)
        call ismksc(.60)
        call ispmci(2)
        call ipm(i, xex, yex)

* Plot overflow
        i = 0
        do j = 1, num_xbins
          if (data(j+offset).gt.limit(2)) then
            i = i + 1
            xex(i) = xbin(j)
            yex(i) = limit(2) - (limit(2)-limit(1))/30
          endif
        enddo
        call ismk(22)
        call ismksc(.60)
        call ispmci(2)
        call ipm(i, xex, yex)

      enddo

* Make a summary box 
      call iswn(12, limit(3), limit(4), 0., 1.15*cmax)
      xmin = 17.0 * r + xleft
      xmax = 24.5 * r + xleft
      ymin = 8.0 * r + ybottom
      ymax = 14.5 * r + ybottom
      call isvp(12, xmin, xmax, ymin, ymax)
      call iselnt(12)

* Put a summary histogram in it
      limit2(1) = limit(3)
      limit2(2) = limit(4)
      call ighist(NBINS, limit2, contents, 'AH')

* Show limit lines
      if (type.eq.MSTRIP_PLOT.or.type.eq.MWIRE_PLOT.or.
     & type.eq.MHSTRIP_PLOT) then
        limit(5) = limit(2*4+6 +1) * lower_limit_fraction
        limit(6) = limit(2*4+6 +1) * upper_limit_fraction
      endif
      call isln(12)         ! Dashed lines
      xl(1) = limit(5)
      xl(2) = limit(5)
      yl(1) = 0
      yl(2) = 9999 
      call ipl(2,xl,yl)

      xl(1) = limit(6)
      xl(2) = limit(6)
      call ipl(2,xl,yl)
      call isln(1)         ! Back to solid lines

* ----------------------------------------------------------------------
      points_beyond = 0
      do j = 1, num_xbins * NLAYERS
        layer = 1 + (j - 1) / num_xbins
        if ((type.ne.MSTRIP_PLOT.and.type.ne.MHSTRIP_PLOT.and.
     &   type.ne.MWIRE_PLOT).or.layer.eq.4) then
           if (data(j) < limit(5) .or. data(j) > limit(6)) then
              points_beyond = points_beyond + 1
           endif
        endif
      enddo


          write(limit5,141), limit(5)
*          write(limit6,141), limit(6)
          write(points_b_char,142), points_beyond 
  141     format(f6.1)
  142     format(i5)  
*      call igtext(16.5, 18.5, limit5, 0.30, 0., 'L')
*      call igtext(16.5, 18.0, limit6, 0.30, 0., 'L')
* ----------------------------------------------------------------------

* Add text (coordinates in cm)
      call iselnt(1)
      if (type.eq.STRIP_PLOT .or. 
     &     type.eq.MSTRIP_PLOT .or.
     &     type.eq.STRIP_PLOT_ODD .or.
     &     type.eq.STRIP_PLOT_EVEN) then      
        call igtext(15.5, 2.0, 'Strip number', 0.30, 0., 'R')
      else if (type.eq.WIRE_PLOT .or. type.eq.MWIRE_PLOT) then
        call igtext(15.5, 2.0, 'Wiregroup number', 0.30, 0., 'R')
      else if(type.eq.HSTRIP_PLOT .or. type.eq.MHSTRIP_PLOT) then
        call igtext(15.5, 2.0, 'Halfstrip number', 0.30, 0., 'R')
      else if(type.eq.GAIN_PLOT) then
        call igtext(15.5, 2.0, 'NCFEBs * HV segment + CFEB number', 
     &   0.30, 0., 'R')
      endif
      do layer = 1, 6
      	if (type .le. HSTRIP_PLOT .or.
     &      type .eq. GAIN_PLOT   .or.
     &     type.eq.STRIP_PLOT_EVEN .or.
     &     type.eq.STRIP_PLOT_ODD ) then     
          write(label,41), layer
   41     format('Layer ', i1)
        else          ! for multiple plane triggers
          write(label, 42), layer
   42     format(i1,'/6 plane')
        endif
*       call igtext(0.25, 1.75+2.5*(7-layer), label, 0.25, 90., 'C')
        call igtext(0.30, 1.75+2.5*(7-layer), label, 0.25, 90., 'C')
      enddo
      write (string,43) test_num
   43 format('Test ', i2)
* the former step was 0.8
      delta_upper1=0.5
      delta_upper2=1.0

*      call igtext(16.5, 18.5, limit5, 0.30, 0., 'L')
*      call igtext(16.5, 18.0, limit6, 0.30, 0., 'L')

      call igtext(16.5, 18.5-delta_upper1, cscname, 0.30, 0., 'L')
      call igtext(16.5, 18.0-delta_upper1, string, 0.30, 0., 'L')
      call igtext(16.5, 17.5-delta_upper1, testname1, 0.30, 0., 'L')

*     call igtext(16.5, 16.9, plot_title, 0.40, 0., 'L')
      call max_string_25(plot_title, n, title1, title2, title3)
      call igtext(16.5, 17.5-delta_upper2, title1, 0.30, 0., 'L')
      if (n > 1) 
     & call igtext(16.5, 17.0-delta_upper2, title2, 0.30, 0., 'L')
      if (n > 2) 
     & call igtext(16.5, 16.5-delta_upper2, title3, 0.30, 0., 'L')

      if(n.eq.1) 
     & call igtext(16.5, 17.0-delta_upper2, 
     & number_of_run1, 0.30, 0., 'L')
      if(n.eq.2) 
     & call igtext(16.5, 16.5-delta_upper2, 
     & number_of_run1, 0.30, 0., 'L')
      if(n.eq.3) 
     & call igtext(16.5, 16.0-delta_upper2, 
     & number_of_run1, 0.30, 0., 'L')

      call igtext(24.5, 7.0,  summary_title, 0.30, 0., 'R')
      call igtext(24.5, 0.5,  timestring, 0.22, 0., 'R')
      call igtext(24.5, 0.0,  run_date1, 0.22, 0., 'R')

* Show statistics
      call igtext(21.0, 14.1, 'Entries', 0.22, 0., 'L')
      call igtext(21.0, 13.7, 'Mean', 0.22, 0., 'L')
      call igtext(21.0, 13.3, 'RMS', 0.22, 0., 'L')
      call igtext(21.0, 12.9, 'UDFLW', 0.22, 0., 'L')
      call igtext(21.0, 12.5, 'OVFLW', 0.22, 0., 'L')
      write (string,50) nentries
      call igtext(24.4, 14.1, string, 0.22, 0., 'R')
      call four_significant_digits(mean,string)
      call igtext(24.4, 13.7, string, 0.22, 0., 'R')
      call four_significant_digits(rms,string)
      call igtext(24.4, 13.3, string, 0.22, 0., 'R')
      write (string,50) nunderflow
      call igtext(24.4, 12.9, string, 0.22, 0., 'R')
      write (string,50) noverflow
      call igtext(24.4, 12.5, string, 0.22, 0., 'R')
   50 format(i10)
      call igbox(20.8, 24.5, 12.3, 14.5)
* ---------------------------------------------------

*      call igtext(16.5, 4.5, 'NAME =', 0.30, 0., 'L')
*      call igtext(22.0, 4.5, 'SITE =', 0.30, 0., 'L')
*      call igtext(16.5, 3.8, 'DAQ VERSION =', 0.30, 0., 'L')
*      call igtext(21.0, 3.8, 'ANA VERSION =', 0.30, 0., 'L')

*      call igtext(18.0, 4.5, user_name1, 0.30, 0., 'L')
*      call igtext(23.5, 4.5, site1,       0.30, 0., 'L')
*      call igtext(19.0, 3.8, daq_version1, 0.30, 0., 'L')
*      call igtext(23.5, 3.8, anal_version1, 0.30, 0., 'L')

* ---------------------------------------------------

* ---------------------------------------------------
*      delta_header=0.3
       step_lower=-0.6
       delta_lower=0.
      call igtext(16.5+0.3, 6.0, 'points out of limits =', 
     & 0.30, 0., 'L')
      call igtext(21.5, 6.0, points_b_char, 0.30, 0., 'L')

      call igtext(16.5+0.3, 
     &5.2+delta_lower, 'NAME =', 0.30, 0., 'L')
      call igtext(16.5+0.3, 
     &5.2+delta_lower+step_lower*1, 'SITE =', 0.30, 0., 'L')
*      call igtext(22.0, 4.5, 'SITE =', 0.30, 0., 'L')
      call igtext(16.5+0.3, 
     &5.2+delta_lower+step_lower*2, 'DAQ VERSION =', 0.30, 0., 'L')
      call igtext(16.5+0.3, 
     &5.2+delta_lower+step_lower*3, 'ANA VERSION =', 0.30, 0., 'L')

      call igtext(18.5+0.3, 5.2, user_name1, 0.30, 0., 'L')
      call igtext(18.5+0.3, 
     &5.2+delta_lower+step_lower*1, site1,       0.30, 0., 'L')
*      call igtext(23.5, 4.5, site,       0.30, 0., 'L')
      call igtext(20.5+0.3, 
     &5.2+delta_lower+step_lower*2, daq_version1, 0.30, 0., 'L')
      call igtext(20.5+0.3, 
     &5.2+delta_lower+step_lower*3, anal_version1, 0.30, 0., 'L')

* ---------------------------------------------------

      call iuwk(0,1)
*     print *, 'Hit return to continue:  '
*     read (*,5), i
* Finish the postscript file
*     call igmeta(0,0)
      call idawk(wkid)
      call iclwk(wkid)
      close(lun)

* Maybe repeat
*     if (first) then
*       goto 66
*       first = .FALSE.
*     endif
      call igend
      return

 100  continue
      print 101, filename
 101  format('Error opening output file ', a, ', error unknown')
      call igend
      return
      end

*----------------------------------------------------------------------------

      subroutine four_significant_digits(x, string)
      implicit none
      real*4           x
      character*(*)    string

      if (abs(x).gt.9999.5) then
        write(string,10) x
  10    format(e10.4)
      else if (abs(x).ge.999.95) then
        write(string,20) x
  20    format(f10.0)
      else if (abs(x).ge.99.995) then
        write(string,30) x
  30    format(f10.1)
      else if (abs(x).ge.9.9995) then
        write(string,40) x
  40    format(f10.2)
      else if (abs(x).ge.0.99995) then
        write(string,50) x
  50    format(f10.3)
      else if (abs(x).ge.0.09995) then
        write(string,60) x
  60    format(f10.4)
      else 
        write(string,70) x
  70    format(e10.4)
      endif
      return
      end

*----------------------------------------------------------------------------
