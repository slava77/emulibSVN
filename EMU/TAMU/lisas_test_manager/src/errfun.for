	function errfun(x)
        double precision dpar(24), fitfun
        common / hcfitd / dpar, fitfun
	real*4    errfun, x
        real*8    derf

	fitfun = 1 + (1 + derf( (x-dpar(1)) / (sqrt(2.)*dpar(2)) )) / 2.
        errfun = fitfun
	return
	end
