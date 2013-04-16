      subroutine dops(limit, cscname, timestring, title)
      INTEGER ERRFIL,WKID,WTYPE,CONID
      COMMON/WKID/ERRFIL,WKID,WTYPE,CONID
c      dimension par(6)
      real*4           limit(*),var
      character*(*)    cscname, timestring, title, filename


c      CALL VZERO(6,par)
      open(unit=34,file='/home/fast/data/test_results/temp/test6.ps'
     +     ,form='formatted',status='unknown')
c      open(unit=34,file=filename,form='formatted',status='unknown')

      CALL HBOOK2(9999,'Wrong Channels',6,0.5,6.5,66 ,-0.5, 65.5,0.)
      CALL IGINIT(0)
      call igmeta(-34,-111)
c      call hpltab(200,6,par,chopt)
        CALL HPLINT(0)
        CALL HPLCAP(-34)
      CALL HPLZON(2,3,1,' ')
    
      CALL HPLSET('PWID',2.)
c      CALL HPLSET('HTYP',333.)
      CALL HPLSET('HCOL',1203.)

      do il=1,6
      ih=4000+il*100 
c      CALL HPLTIT('UTIT')   
      do i=1,64
      var=hi(ih+2,i)
      print *, i,var,limit(5),limit(6)
      if(var.ge.limit(5).and.var.le.limit(6)) then
c for tes5     if(var.ge.20) then

      CALL HPLSET('HCOL',1202.)
c      print *,i,il
      CALL HFILL(9999,float(il),float(i),float(i))
      endif
      enddo
c      CALL HPLOT(ih,' ',0,'  ')
c      CALL HPLOT(ih+1,' ',0,'  ')
   
      CALL HPLOT(ih+2,' ',0,'  ')
c      call hpltit('Channel')
      CALL HPLSET('HCOL',1203.)
      enddo

      call hpltit(cscname)


c      call hplset('HTYP',244.)
      CALL HPLSET('HCOL',1202.)

      call hplopt('GRID',4)
      call hplset('2SIZ',1.)
      CALL HPLZON(1,1,1,' ')
      CALL HPLOT(9999,'TEXT',0,' ')

c      call igtext(24.5, 7.0, title, 0.30, 0., 'R')
c      call igtext(22.5, 17.0, cscname, 0.35, 0., 'R')
c      call igtext(24.5, 18.0, timestring, 0.25, 0., 'R')
        CALL HPLEND
      close(34)
      end
