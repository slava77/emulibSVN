	function landau(x)
        common / myparam / par
        real*4    par(3)
	real*4    denlan, x, landau
	landau = par(1) * denlan((x-par(2))/par(3))
	return
	end
