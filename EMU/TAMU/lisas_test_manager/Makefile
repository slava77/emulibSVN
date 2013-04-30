#***************************************************************************
#
# Purpose: makefile for the test_manager utility
#
#***************************************************************************
#OPT_FORMAT = METB_FORMAT
USE_DDUREADOUT = DDU

## name of the operating system
OS := $(shell uname)

#export DAQHOME=/home/cscme42/TriDAS/emu/fast_daq
#export CERNDIR=/usr/libexec/CERNLIB/2005

$(info DAQHOME = $(DAQHOME) )
ifndef DAQHOME 
$(warning no DAQHOME is set setting to /home/cscme11/TriDAS/emu/old_code/fast_daq_before_2013Apr08)
DAQHOME=/home/cscme11/TriDAS/emu/old_code/fast_daq_before_2013Apr08
endif
ifndef ROOTSYS
$(error set ROOTSYS)
endif
ifndef XDAQ_ROOT
$(error set XDAQ_ROOT)
endif
ifndef TRIDASDIST
$(error set TRIDASDIST)
endif

## Directory structure
HDIR = $(PWD)
#$(DAQHOME)/lisas_test_manager
DAQCOMMON =$(DAQHOME)/daq_common
READOUT_CONTROL=$(DAQHOME)/readout_control
EVTANA=$(DAQHOME)/event_analysis

#this may have reset the default already set
BUILD_HOME=$(TRIDASDIST)
# now check if subdirs exist. Want this in any case, hence don't make a rule 
haveDIRS=$(shell test -d $(DAQCOMMON) -a -d $(READOUT_CONTROL) -a -d $(EVTANA) \
	|| echo no ) 
ifeq  "$(haveDIRS)" "no" 
$(error Prerequisite dirs are missing)
endif

#$(STEPHOME)=$(HOME)
ifndef CSCHOME
$(info setting CSCHOME=/home/cscme11)
CSCHOME=/home/cscme11
endif  
BDIR = $(HDIR)/bin
SDIR = $(HDIR)/src
ODIR = $(HDIR)/obj
IDIR = $(HDIR)/include
LDIR = $(HDIR)/lib
CMSSW=$(CSCHOME)/CMSSW/src
KUMAC = $(HDIR)/kumac
KUMACFILES := $(notdir $(wildcard $(KUMAC)/*.kumac))

#***************************************************************************
## names for commands
CC         = gcc
FC         = gfortran
F77        = gcc
CXX        = g++
#5 lines below now commented outx
#ifeq ($(VERSIONING_BUILDALL),TRUE)
#VC1        =
#VC2        = 
#else
#VC1        = @echo To get a production version, invoke the main makefile
#VC2        = @$(DAQHOME)/check_version ANA_SW_TAG $(IDIR)/ana_version.h ANA
#endif

#***************************************************************************
## options for compiling

# Uncomment the next line when debugging
DEBUG      = -ggdb -W -DDEBUG

# Uncomment the next line when NOT debugging
#DEBUG      = -Wall -O3

CCOPT      = $(DEBUG) -Wall -Df2cFortran  -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DCSC_DISPLAY -DLOCALDAQ
F77OPT     = $(DEBUG) -c -C -Wall  -fno-second-underscore
#CXXOPT     = $(DEBUG) -fno-rtti -fno-exceptions
ROOTCFLAG  := $(shell root-config --cflags)
ROOTLIB    := $(shell root-config --libs)
ROOTINC    := $(shell root-config --incdir)
CXXOPT     = $(DEBUG) -m64 -fPIC -Wall -Df2cFortran -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DCSC_DISPLAY -DLOCALDAQ
CXXOPT    += $(ROOTCFLAG)
#***************************************************************************
## location of CERN stuff



CERNLIB	:=$(shell /cern/pro/bin/cernlib -v pro graflib packlib grafX11 mathlib kernlib)

#***************************************************************************
## includes and linking X libraries and forms libs
XDAQ_DS    = $(XDAQ_OS)/$(XDAQ_PLATFORM)
XINC       = -I/usr/include/X11 -I/usr/X11R6/include -I/usr/include
#XLIB       = -lforms -L/usr/X11R6/lib -lXpm -lX11
XLIB       = -L/usr/X11R6/lib -L/usr/lib64 -lXpm -lX11
EXTLIB  = -L$(XDAQ_ROOT)/lib  -L$(TRIDASDIST)/$(XDAQ_OS)/lib
#avoid local includes for now
#EXTLIB += -L$(BUILD_HOME)/emu/daq/reader/lib/$(XDAQ_DS)
#EXTLIB += -L$(BUILD_HOME)/emu/dqm/common/lib/$(XDAQ_DS)
#EXTLIB += -L$(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_DS)
#EXTLIB += -L$(DAQHOME)/csc_display/lib
EXTLIB += -lemudaqreader_sa  -lemudqmcommon_sa -lCSCRawToDigi -lCSCDigi -lemuSTEPConfigRead 
#xdaq stuff (isn't there a magic command to make this list?) 
EXTLIB += -lxdaq -lxerces-c -lxoap -lxdata -ltoolbox -lxcept -lxgi -lcgicc -llogxmlappender -lconfig -lmimetic -lasyncresolv -llog4cplus -lpeer -llogudpappender -lappweb
EXTLIB += -luuid
EXTLIB += $(ROOTLIB)


#***************************************************************************
## other includes and libs
Project=emu
LOCINC = \
	-I$(IDIR) \
	-I$(DAQCOMMON)/include \
	-I$(EVTANA)/include \
        -I$(DAQHOME)/alct_jtag/include \
        -I$(CERNDIR)/include/cfortran \
	-I/usr/local/include \
	-I$(CMSSW) \
	-I$(TRIDASDIST)/$(XDAQ_PLATFORM)/include \
	-I$(XDAQ_ROOT)/include
LOCINC += -I$(TRIDASDIST)/$(XDAQ_PLATFORM)/include/emu/dqm/cmssw
#LOCINC += -I$(BUILD_HOME)/emu/emuSTEP/emuSTEPConfigRead/include
#exists in TRIDASDIST
#LOCINC += -I$(BUILD_HOME)/emu/daq/reader/include 
#LOCINC += -I$(BUILD_HOME)/emu/dqm/common/include
#LOCINC += -I$(BUILD_HOME)/emu/daq/reader/include



ifeq ($(OS),Linux)
             # True for CERN version of RH 5.1 and RH 5.2
  LINUX_VERSION := $(shell uname -r)
  ifeq ($(LINUX_VERSION),2.0.36)
    LIB    = -lm -ldl -lc -lfl \
             /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.90.29/libf2c.a
  else
    LIB    = -L$(LDIR)  -lm -ldl -lc -lfl -lgfortran -lpthread -L$(BUILD_HOME)/$(XDAQ_PLATFORM)/lib -lg2c
  endif
endif

#***************************************************************************
## get object file names from source file names
OBJECTS      = \
	$(ODIR)/test_manager.o \
	$(ODIR)/test_utils.o \
	$(ODIR)/fast_ana.o \
	$(ODIR)/test_11.o \
	$(ODIR)/test_12.o \
	$(ODIR)/test_13.o \
	$(ODIR)/test_14.o \
	$(ODIR)/test_15.o \
	$(ODIR)/test_16.o \
	$(ODIR)/test_17.o \
	$(ODIR)/test_18.o \
	$(ODIR)/test_19.o \
	$(ODIR)/test_20.o \
	$(ODIR)/test_21.o \
	$(ODIR)/test_22.o \
	$(ODIR)/test_23.o \
	$(ODIR)/test_24.o \
	$(ODIR)/test_25.o \
	$(ODIR)/test_26.o \
	$(ODIR)/test_27.o \
	$(ODIR)/test_28.o \
	$(ODIR)/clctsim.o \
	$(ODIR)/alctsim.o \
	$(ODIR)/book_histos_f.o \
	$(ODIR)/errfun.o \
	$(ODIR)/landau.o \
	$(ODIR)/one_page_plot_v3.o \
	$(ODIR)/one-page-plot.o \
	$(ODIR)/strip_utils.o \
	$(ODIR)/csc_geom.o \
	$(ODIR)/test_utils_f.o \
	$(ODIR)/test_utils_for_root.o \
	$(ODIR)/screen.o \
	$(ODIR)/unpack_data.o \
	$(ODIR)/io_utils.o
# now some extra nonstandard stuff 
OBJECTS += $(DAQCOMMON)/lib/daq_common_offline.a
# \
#	$(DAQHOME)/readout_control/obj/io_utils.o \
#	$(DAQCOMMON)/obj/daq_ipc.o $(DAQCOMMON)/lib/daq_common_offline.a


#already in EXT_Library
#	$(DAQCOMMON)/lib/daq_common_offline.a 
#        $(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_DS)/libCSCDigi.a \
#	$(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_DS)/libCSCRawToDigi.a \
#	$(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_DS)/libCSCDQMCommon.a \
#        $(BUILD_HOME)/emu/dqm/common/lib/$(XDAQ_DS)/libemudqmcommon_sa.a \
#	$(BUILD_HOME)/emu/daq/reader/lib/$(XDAQ_DS)/libemudaqreader_sa.a \
#	$(BUILD_HOME)/emu/emuSTEP/emuSTEPConfigRead/lib/$(XDAQ_DS)/libemuSTEPConfigRead.a \

# cannot figure out how to generate static lib for xerces, so just link all objects

#               $(DAQHOME)/alct_jtag/lib/alct_jtag.a \
#***************************************************************************

# all : versioning  daq_common_lib $(BDIR)/lisas_test_manager
#all : versioning  $(BDIR)/lisas_test_manager kumac

#versioning:
#	$(VC1)
#	$(VC2)

#daq_common_lib:
#	cd  ../daq_common       ; make

$(BDIR)/lisas_test_manager : $(BDIR) $(OBJECTS)
	$(CC) $(CCOPT) $(LOCINC)  -o $(BDIR)/lisas_test_manager $(OBJECTS) \
          $(LIB) $(XLIB) $(EXTLIB) $(CERNLIB)

$(BDIR) :
	mkdir -p $(BDIR)

$(ODIR)/fast_ana.o : $(SDIR)/fast_ana.cc $(IDIR)/test_functions.h \
          $(IDIR)/test_params.h $(DAQCOMMON)/include/csc_event.h \
          $(DAQCOMMON)/include/daq_module.h
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/fast_ana.o \
          -c $(SDIR)/fast_ana.cc

$(ODIR)/test_manager.o : $(SDIR)/test_manager.c \
	  $(IDIR)/test_params.h \
          $(IDIR)/test_utils.h $(DAQCOMMON)/include/misc.h \
          $(DAQCOMMON)/include/daq_conf.h $(DAQCOMMON)/include/daq_ipc.h \
          $(DAQCOMMON)/include/daq_module.h 
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_manager.o \
          -c $(SDIR)/test_manager.c 


$(ODIR)/test_11.o : $(SDIR)/test_11.c $(IDIR)/one_page_plot.h $(IDIR)/test_functions.h  \
          $(IDIR)/test_params.h $(IDIR)/test_utils.h $(DAQCOMMON)/include/csc_event.h \
          $(DAQCOMMON)/include/daq_module.h \
	  $(DAQCOMMON)/include/misc.h 
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_11.o \
          -c $(SDIR)/test_11.c

$(ODIR)/test_12.o : $(SDIR)/test_12.c $(IDIR)/one_page_plot.h \
          $(IDIR)/test_params.h $(IDIR)/test_utils.h \
          $(DAQCOMMON)/include/csc_event.h  $(DAQCOMMON)/include/misc.h 
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_12.o \
          -c $(SDIR)/test_12.c

$(ODIR)/test_13.o : $(SDIR)/test_13.c  \
          $(IDIR)/one_page_plot.h $(DAQCOMMON)/include/csc_event.h 
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_13.o \
          -c $(SDIR)/test_13.c

$(ODIR)/test_14.o : $(SDIR)/test_14.cpp  \
          $(IDIR)/one_page_plot.h $(DAQCOMMON)/include/csc_event.h 
	$(CXX) $(CXXOPT) $(LOCINC) -o $(ODIR)/test_14.o \
          -c $(SDIR)/test_14.cpp

$(ODIR)/test_15.o : $(SDIR)/test_15.cpp  \
	 $(IDIR)/one_page_plot.h \
          $(IDIR)/test_params.h $(IDIR)/test_utils.h \
          $(DAQCOMMON)/include/csc_event.h $(DAQCOMMON)/include/misc.h 
	$(CXX) $(CXXOPT) $(LOCINC) -o $(ODIR)/test_15.o \
          -c $(SDIR)/test_15.cpp

$(ODIR)/test_16.o : $(SDIR)/test_16.cpp  \
          $(IDIR)/one_page_plot.h $(DAQCOMMON)/include/csc_event.h \
	$(IDIR)/basic-utilities.h
	$(CXX) $(CXXOPT) $(LOCINC) -o $(ODIR)/test_16.o \
          -c $(SDIR)/test_16.cpp

$(ODIR)/one-page-plot.o : $(SDIR)/one-page-plot.cpp \
          $(IDIR)/one-page-plot.h $(DAQCOMMON)/include/csc_event.h 
	$(CXX) $(CXXOPT) $(LOCINC) -o $(ODIR)/one-page-plot.o \
          -c $(SDIR)/one-page-plot.cpp

$(ODIR)/basic-utilities.o : $(SDIR)/basic-utilities.cpp $(IDIR)/basic-utilities.h \
          $(IDIR)/one-page-plot.h $(DAQCOMMON)/include/csc_event.h 
	$(CXX) $(CXXOPT) $(LOCINC) -o $(ODIR)/basic-utilities.o \
          -c $(SDIR)/basic-utilities.cpp

$(ODIR)/test_17.o : $(SDIR)/test_17.c $(IDIR)/one_page_plot.h \
          $(IDIR)/test_params.h $(IDIR)/test_utils.h \
          $(DAQCOMMON)/include/csc_event.h $(DAQCOMMON)/include/misc.h 
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_17.o \
          -c $(SDIR)/test_17.c

$(ODIR)/test_18.o : $(SDIR)/test_18.c $(IDIR)/one_page_plot.h \
          $(IDIR)/test_params.h $(DAQCOMMON)/include/csc_event.h
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_18.o \
          -c $(SDIR)/test_18.c

$(ODIR)/test_19.o : $(SDIR)/test_19.c $(IDIR)/one_page_plot.h \
          $(IDIR)/test_params.h $(IDIR)/test_utils.h \
          $(DAQCOMMON)/include/csc_event.h $(DAQCOMMON)/include/misc.h 
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_19.o \
          -c $(SDIR)/test_19.c

$(ODIR)/test_20.o : $(SDIR)/test_20.c $(IDIR)/one_page_plot.h \
          $(DAQCOMMON)/include/csc_event.h \
          $(DAQCOMMON)/include/csc_parameters.h $(DAQCOMMON)/include/misc.h \
          $(IDIR)/test_params.h $(IDIR)/test_utils.h \
          $(IDIR)/alct_params.h 
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_20.o -c $(SDIR)/test_20.c

$(ODIR)/test_21.o : $(SDIR)/test_21.c  \
          $(IDIR)/one_page_plot.h $(DAQCOMMON)/include/csc_event.h 
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_21.o \
          -c $(SDIR)/test_21.c

$(ODIR)/test_22.o : $(SDIR)/test_22.c $(IDIR)/test_params.h $(IDIR)/clctsim.h \
          $(IDIR)/one_page_plot.h $(IDIR)/csc_geom.h $(IDIR)/test_utils.h \
          $(DAQCOMMON)/include/csc_event.h \
          $(DAQCOMMON)/include/csc_parameters.h $(DAQCOMMON)/include/misc.h
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_22.o \
          -c $(SDIR)/test_22.c

$(ODIR)/test_23.o : $(SDIR)/test_23.c $(IDIR)/one_page_plot.h \
          $(IDIR)/csc_geom.h $(IDIR)/test_params.h \
          $(IDIR)/test_utils.h $(DAQCOMMON)/include/csc_event.h \
          $(DAQCOMMON)/include/daq_histo.h $(DAQCOMMON)/include/daq_module.h
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_23.o \
          -c $(SDIR)/test_23.c

$(ODIR)/test_24.o : $(SDIR)/test_24.c $(IDIR)/one_page_plot.h \
          $(IDIR)/test_params.h $(IDIR)/test_utils.h \
          $(DAQCOMMON)/include/csc_event.h $(DAQCOMMON)/include/misc.h \
          $(DAQCOMMON)/include/daq_histo.h $(DAQCOMMON)/include/daq_module.h
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_24.o \
          -c $(SDIR)/test_24.c

$(ODIR)/test_25.o : $(SDIR)/test_25.c $(IDIR)/one_page_plot.h \
          $(IDIR)/test_params.h $(IDIR)/test_utils.h \
	  $(READOUT_CONTROL)/include/setup_parameters.h \
          $(DAQCOMMON)/include/csc_event.h \
          $(DAQCOMMON)/include/csc_parameters.h \
          $(DAQCOMMON)/include/daq_histo.h $(DAQCOMMON)/include/daq_module.h 
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_25.o \
          -c $(SDIR)/test_25.c -I$(READOUT_CONTROL)/include

$(ODIR)/test_26.o : $(SDIR)/test_26.c $(IDIR)/one_page_plot.h \
          $(IDIR)/test_params.h $(IDIR)/test_utils.h \
          $(DAQCOMMON)/include/csc_event.h \
          $(DAQCOMMON)/include/csc_parameters.h 
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_26.o \
          -c $(SDIR)/test_26.c

$(ODIR)/test_27.o : $(SDIR)/test_27.c $(SDIR)/csc_geom.c\
	   $(IDIR)/one_page_plot.h \
          $(IDIR)/test_params.h $(IDIR)/test_utils.h \
          $(DAQCOMMON)/include/csc_event.h \
          $(DAQCOMMON)/include/csc_parameters.h 
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_27.o \
          -c $(SDIR)/test_27.c

$(ODIR)/test_28.o : $(SDIR)/test_28.c $(SDIR)/csc_geom.c\
	   $(IDIR)/one_page_plot.h \
          $(IDIR)/test_params.h $(IDIR)/test_utils.h \
          $(DAQCOMMON)/include/csc_event.h \
          $(DAQCOMMON)/include/csc_parameters.h 
	$(CXX) $(CXXOPT) $(LOCINC) -o $(ODIR)/test_28.o \
          -c $(SDIR)/test_28.c

$(ODIR)/test_utils.o : $(SDIR)/test_utils.c $(IDIR)/test_params.h \
          $(IDIR)/test_utils.h $(DAQCOMMON)/include/csc_parameters.h \
	  $(IDIR)/ana_version.h
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/test_utils.o -c \
          $(SDIR)/test_utils.c 

$(ODIR)/test_utils_for_root.o : $(SDIR)/test_utils_for_root.cpp $(IDIR)/test_params.h \
          $(IDIR)/test_utils_for_root.h $(DAQCOMMON)/include/csc_parameters.h \
	  $(IDIR)/ana_version.h
	$(CXX) $(CXXOPT) $(LOCINC) -o $(ODIR)/test_utils_for_root.o -c \
          $(SDIR)/test_utils_for_root.cpp 

$(ODIR)/alctsim.o : $(SDIR)/alctsim.c $(DAQCOMMON)/include/misc.h \
          $(DAQCOMMON)/include/daq_conf.h
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/alctsim.o -c \
          $(SDIR)/alctsim.c


$(ODIR)/clctsim.o : $(SDIR)/clctsim.c $(DAQCOMMON)/include/misc.h \
          $(DAQCOMMON)/include/csc_parameters.h
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/clctsim.o -c \
          $(SDIR)/clctsim.c

$(ODIR)/errfun.o : src/errfun.for
	$(F77) $(F77OPT) $(LOCINC) -o obj/errfun.o src/errfun.for

$(ODIR)/landau.o : src/landau.for
	$(F77) $(F77OPT) $(LOCINC) -o obj/landau.o src/landau.for

$(ODIR)/book_histos_f.o : src/book_histos_f.f \
         $(DAQCOMMON)/include/csc_parameters.inc
	$(F77) $(F77OPT) $(LOCINC) -o obj/book_histos_f.o src/book_histos_f.f

$(ODIR)/one_page_plot_v3.o : src/one_page_plot_v3.f
	$(F77) $(F77OPT)  $(LOCINC) -o obj/one_page_plot_v3.o \
         src/one_page_plot_v3.f

$(ODIR)/strip_utils.o : src/strip_utils.f $(DAQCOMMON)/include/csc_event.inc \
         $(IDIR)/csc_geom.inc $(DAQCOMMON)/include/csc_parameters.inc
	$(F77) $(F77OPT) $(LOCINC) -o obj/strip_utils.o \
         src/strip_utils.f

$(ODIR)/csc_geom.o : src/csc_geom.c $(DAQCOMMON)/include/misc.h \
       $(IDIR)/csc_geom.h $(DAQCOMMON)/include/csc_parameters.h 
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/csc_geom.o -c $(SDIR)/csc_geom.c 

$(ODIR)/test_utils_f.o : src/test_utils_f.f
	$(F77) $(F77OPT)  $(LOCINC) -o obj/test_utils_f.o src/test_utils_f.f

$(ODIR)/screen.o : $(SDIR)/screen.c
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/screen.o -c $(SDIR)/screen.c 

$(ODIR)/io_utils.o : $(SDIR)/io_utils.c 
	 $(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/io_utils.o -c $(SDIR)/io_utils.c

$(ODIR)/unpack_data.o : $(SDIR)/unpack_data.cc \
					   $(DAQCOMMON)/include/csc_event.h \
                                           $(DAQCOMMON)/include/daq_conf.h \
                                           $(DAQCOMMON)/include/j_common_data.h
	$(CC) $(CCOPT) $(LOCINC) $(XINC) -o $(ODIR)/unpack_data.o \
          -c $(SDIR)/unpack_data.cc 

.PHONY: clean kumac

kumac: 
	cp $(KUMAC)/*.kumac $(BDIR)

clean :
	rm -f obj/*.o
	rm -f src/*~
	rm -f *~
	rm -f $(BDIR)/lisas_test_manager
	rm -f $(BDIR)/*.kumac

check : $(DAQCOMMON)
	$(error check failed)