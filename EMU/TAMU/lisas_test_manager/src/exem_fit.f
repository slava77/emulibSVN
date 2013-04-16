      Program Hfitv_Example
 *
       Parameter (Npoints=100,Npar=3)
       Dimension X(Npoints),Y(Npoints),E(Npoints)
       Dimension Pmin(Npar),Pmax(Npar),Step(Npar),Sigpar(Npar)
       Common/PAWPAR/PAR(Npar)
       Common /PAWC/ H(100000)
       External FitFun
 *.___________________________________________
 *
       Call Igwkty(Kwtype)
       Call Hlimit(100000)
       Call Hplint(Kwtype)
 *
 * Vectors to br fitted
 *
       Do i=1,Npoints
          R    = 5.*rndm(i)
          X(i) = Float(i)
          Y(i) = X(i)*X(i)+i*R
          E(i) = R
       Enddo
 *
 * Parameters needed by Hfitv
 *
       Do i=1,Npar
          PAR(i)  = 1.
          Pmin(i) = 0.
          Pmax(i) = 0.
          Step(i) = -1.
       enddo
 *
 * Perform the fit
 *
       Call Hfitv(Npoints,Npoints,1,X,Y,E,FitFun,
      +           ' ',Npar,PAR,Step,Pmin,Pmax,Sigpar,Chi2)
 *
 * Draw the curve
 *
       Call Hplfra(0.,101.,0.,10100.,' ')
       Call Igraph(100,X,Y,'L')
 *
 * Draw the fit
 *
       Do i=1,Npoints
          Y(i) = FitFun(X(i))
       Enddo
       Call Igset('PLCI',2.)
       Call Igraph(100,X,Y,'L')
 *
       Call Igterm
       Read(*,*)
       Call Hplend
       End

       Function FitFun(X)
       Parameter (Npar=3)
       Common/PAWPAR/PAR(Npar)
       FitFun = PAR(1)*X*X + PAR(2)*X + PAR(3)
       End
