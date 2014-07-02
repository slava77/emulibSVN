CXXFLAGS      = -c -O -Wall -Wno-deprecated -fPIC `root-config --cflags` -I$(ROOTSYS)/include -I./
LDFLAGS       = -g -O `root-config --glibs`
LD            = g++

OBJS = hv_ana.o
OSIMPLE = hv_ana_simple.o
OCALPLOTS = hv_calib_plots.o

PROGRAM         = HVtest
PSIMPLE 	= HVtest_simple
PCALPLOTS	= HVCalibPlots
ATRIPS 		= analyze_trips


all:            $(PROGRAM) $(PSIMPLE) $(PCALPLOTS) $(ATRIPS)



$(PROGRAM):     $(OBJS)
	@echo "Linking $(PROGRAM) ..."
	@/bin/rm -f $(PROGRAM)
	@$(LD) $(LDFLAGS) $(OBJS) -o $(PROGRAM)
	@chmod 555 $(PROGRAM)
	@echo "done"

$(PSIMPLE):     $(OSIMPLE)
	@echo "Linking $(PSIMPLE) ..."
	@/bin/rm -f $(PSIMPLE)
	@$(LD) $(LDFLAGS) $(OSIMPLE) -o $(PSIMPLE)
	@chmod 555 $(PSIMPLE)
	@echo "done"

$(PCALPLOTS):     $(OCALPLOTS)
	@echo "Linking $(PCALPLOTS) ..."
	@/bin/rm -f $(PCALPLOTS)
	@$(LD) $(LDFLAGS) $(OCALPLOTS) -o $(PCALPLOTS)
	@chmod 555 $(PCALPLOTS)
	@echo "done"

###
$(ATRIPS): analyze_trips.o Tripdata_plotter.o
	g++ -ggdb -g -O `root-config --glibs` analyze_trips.o Tripdata_plotter.o -o $(ATRIPS)

analyze_trips.o: analyze_trips.cpp Tripdata_plotter.h
	g++ -ggdb -c -O -Wall -Wno-deprecated -fPIC `root-config --cflags` -I$(ROOTSYS)/include -I./ analyze_trips.cpp

Tripdata_plotter.o: Tripdata_plotter.cpp Tripdata_plotter.h
	g++ -ggdb -c -O -Wall -Wno-deprecated -fPIC `root-config --cflags` -I$(ROOTSYS)/include -I./ Tripdata_plotter.cpp


clean:;  @rm -f *.o $(PROGRAM) $(PSIMPLE) $(PCALPLOTS) core.* $(ATRIPS)

###
hv_ana.o: hv_ana.cxx
	${LD} ${CXXFLAGS} hv_ana.cxx

hv_ana_simple.o: hv_ana_simple.cxx
	${LD} ${CXXFLAGS} hv_ana_simple.cxx

hv_calib_plots.o: hv_calib_plots.cxx
	${LD} ${CXXFLAGS} hv_calib_plots.cxx
