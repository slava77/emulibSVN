      subroutine fit_delay(npoints, content, errors, param, chi2)
      implicit none
*     
*
      Integer*4 npoints, Content(*)
      Real*4    errors(*), param(*), chi2

      Integer*4 MAX_POINTS, NPAR
      Parameter (MAX_POINTS=256,NPAR=3)

      Integer*4 i
      Real*4    X(MAX_POINTS), Y(MAX_POINTS), E(MAX_POINTS)
      Real*4    Pmin(NPAR), Pmax(NPAR), Step(NPAR), Sigpar(NPAR)
      Common/PAWPAR/par(NPAR)
      Real*4    par

      Real*4   delay3
      External FitFun2
*___________________________________________
*

*
* Vectors to be fitted
*
      Do i=1,Npoints
          X(i) = Float(i)
          Y(i) = Content(i)
          E(i) = errors(i)
*         print 10, x(i), y(i), e(i)
*  10     format('x=', f5.2, '  y=', f5.0, '  e=', f7.2)
      Enddo
*
* Parameters needed by Hfitv
*
      Do i=1,NPAR
          par(i)  = param(i)
          Pmin(i) = 0.
          Pmax(i) = 0.
          Step(i) = -1.
      enddo
*
* Perform the fit
*
      Call Hfitv(Npoints, MAX_POINTS, 1, X, Y, E, delay3,
     +           'Q', NPAR, par, Step, Pmin, Pmax, Sigpar, Chi2)  
  
      Do i=1, NPAR
        param(i)  = par(i)  
      enddo

      End

      Function delay2(X)
*     Implicit None
      Integer*4  NPAR
      Parameter (NPAR=1)
      Common/PAWPAR/par(NPAR)
      Integer*4    j
      Real*4       par
      Real*4       x
      Real*4       xp, stepsize, sigma
     
      xp = x - par(1)
      sigma = par(2)
      stepsize = 25. / 7.

      delay2 = 0.
      do j = 1, 6
        delay2 = delay2 + 
     &   0.5 * (1 + erf((xp - j * stepsize) / (sqrt(2.) * sigma)))
      enddo

      End

      Function delay3(X)
*     Implicit None
      Integer*4  NPAR
      Parameter (NPAR=3)
      Common/PAWPAR/par(NPAR)
      Integer*4    j
      Real*4       par
      Real*4       x
      Real*4       xp, stepsize, sigma
     
      xp = x - par(1)
      sigma = par(2)
      stepsize = 25. / par(3)

      delay3 = 0.
      do j = 1, 6
        delay3 = delay3 + 
     &   0.5 * (1 + erf((xp - j * stepsize) / (sqrt(2.) * sigma)))
      enddo

      End
