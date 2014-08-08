#
# HV software makefile
#

## include $(DIMDIR)/makefile_common

# comment to disable Kick-Off! watchdog support
# WATCHDOG = 1


SDIR = src
LDIR = lib
LIBS = $(EXTRALIBS) -lnetsnmp -lcrypto
LIBDIR = linux
ODIR =obj
BDIR =bin

CC = gcc
CXX = g++
CPP = yes

CFLAGS += -I$(DIMDIR)/dim -I./include -I../driver_hvcardx  -I/usr/include/mysql++ \
		-I/usr/include/mysql -I/opt/xdaq/include \
		-D_REENTRANT -ggdb -Wno-write-strings 
LDFLAGS += -L/usr/lib64/mysql -L/opt/xdaq/lib -lmysqlpp -lmysqlclient -llog4cplus -lpthread  
ifdef WATCHDOG
CFLAGS += -g -I/usr/include/kickoff 

LDFLAGS +=  -lrebound
endif
SERVERO= dimappender.o \
	hvAutoDetect.o \
	hvGetHVData.o \
	UpdateTimers.o \
	UpdateDataDim.o \
	Calibration.o \
	LoadCalibration.o \
	SaveConfig.o \
	LoadConfig.o \
	SaveRealConfig.o \
	LoadRealConfig.o \
	CheckForTrips.o \
	hvDB.o \
	hvPVSSNew.o \
	ipc.o \
	pvss_db.o \
	pvss_common.o \
	RpcCSCData.o \
	RpcPrimeHV.o \
	RpcMySQL.o \
	RpcRealData.o \
	RpcHVStatus.o \
	hvDimServer.o \
	hvPVSSInterface.o \
	hvSetDefaultParameters.o \
	SaveTripInfo.o 

LVSERVER= hvDimServer.o


OBJECTS = $(ODIR)/hvPrimaryPS.o

ifeq ($(CPP),yes)
all:	$(BDIR)/hvServer $(BDIR)/hvClient $(LDIR)/libPrimaryPS.a $(BDIR)/hvPrimeCtrl $(BDIR)/hv_pvss $(BDIR)/delete_processes_by_name $(BDIR)/hvGet $(BDIR)/hvNetLogger $(BDIR)/hvDimLogger $(BDIR)/hvCalib $(BDIR)/genHVmap $(BDIR)/hvMon $(BDIR)/lvClient $(BDIR)/lvServer
else
all:
endif

vpath %.cc src
vpath %.o obj

$(ODIR)/%.o : $(SDIR)/%.cc
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

default:


$(BDIR)/hvServer: $(SDIR)/hvServer.cc $(SERVERO:%.o=$(ODIR)/%.o) $(DIMDIR)/$(LIBDIR)/libdim.a $(LDIR)/libPrimaryPS.a
	$(CXX) $(CFLAGS) $(LDFLAGS) $(SDIR)/hvServer.cc -L$(DIMDIR)/$(LIBDIR) -ldim -lncurses  -o $(BDIR)/hvServer $(LIBS) $(LDIR)/libPrimaryPS.a $(SERVERO:%.o=$(ODIR)/%.o)

$(BDIR)/hvClient:       $(SDIR)/hvClient.cc $(DIMDIR)/$(LIBDIR)/libdim.a $(LDIR)/libPrimaryPS.a
	$(CXX) -ggdb $(CFLAGS) $(LDFLAGS) $(SDIR)/hvClient.cc -L$(DIMDIR)/$(LIBDIR) -ldim -lncurses -o $(BDIR)/hvClient $(LIBS) $(LDIR)/libPrimaryPS.a

$(BDIR)/hvCalib:       $(SDIR)/hvCalib.cc $(DIMDIR)/$(LIBDIR)/libdim.a $(LDIR)/libPrimaryPS.a
	$(CXX) -ggdb $(CFLAGS) $(LDFLAGS) $(SDIR)/hvCalib.cc -L$(DIMDIR)/$(LIBDIR) -ldim -lncurses -o $(BDIR)/hvCalib $(LIBS) $(LDIR)/libPrimaryPS.a

$(BDIR)/lvServer: $(SDIR)/lvServer.cc $(SERVERO:%.o=$(ODIR)/%.o) $(DIMDIR)/$(LIBDIR)/libdim.a 
	$(CXX) $(CFLAGS) $(LDFLAGS) $(SDIR)/lvServer.cc -L$(DIMDIR)/$(LIBDIR) -ldim -lncurses  -o $(BDIR)/lvServer $(LIBS) $(LVSERVER:%.o=$(ODIR)/%.o)

$(BDIR)/lvClient: $(SDIR)/lvClient.cc $(SERVERO:%.o=$(ODIR)/%.o) 
	$(CXX) $(CFLAGS) $(LDFLAGS) $(SDIR)/lvClient.cc -L$(DIMDIR)/$(LIBDIR) -lncurses  -o $(BDIR)/lvClient $(LIBS)

$(BDIR)/hvPrimeCtrl:       $(SDIR)/hvPrimeCtrl.cc $(LDIR)/libPrimaryPS.a
	$(CXX) $(CFLAGS) $(LDFLAGS) $(SDIR)/hvPrimeCtrl.cc -lncurses -o $(BDIR)/hvPrimeCtrl $(LIBS) $(LDIR)/libPrimaryPS.a

$(BDIR)/genHVmap:	$(SDIR)/genHVmap.cc
	$(CXX) $(CFLAGS) $(LDFLAGS) $(SDIR)/genHVmap.cc -o $(BDIR)/genHVmap $(LIBS)

$(BDIR)/hvMon:       $(SDIR)/hvMon.cc
	$(CXX) $(CFLAGS) $(LDFLAGS) $(SDIR)/hvMon.cc -lncurses -o $(BDIR)/hvMon 

$(LDIR)/libPrimaryPS.a : $(OBJECTS)
	ar -r $(LDIR)/libPrimaryPS.a $(OBJECTS)


$(ODIR)/hvPrimaryPS.o:	$(SDIR)/hvPrimaryPS.cc
	$(CXX) $(CFLAGS) -c $(SDIR)/hvPrimaryPS.cc -o $(ODIR)/hvPrimaryPS.o

# === PVSS Code ===

$(BDIR)/hv_pvss:        $(SDIR)/hv_pvss.cc $(DIMDIR)/$(LIBDIR)/libdim.a $(ODIR)/ipc.o $(ODIR)/pvss_db.o
	$(CXX) $(CFLAGS) $(LDFLAGS) $(SDIR)/hv_pvss.cc -L$(DIMDIR)/$(LIBDIR) -ldim -lncurses -o $(BDIR)/hv_pvss $(LIBS) $(ODIR)/ipc.o $(ODIR)/pvss_db.o

$(BDIR)/delete_processes_by_name: $(SDIR)/delete_processes_by_name.c Makefile
	gcc $(CFLAGS) -o $(BDIR)/delete_processes_by_name \
        $(SDIR)/delete_processes_by_name.c

$(BDIR)/hvGet:       $(SDIR)/hvGet.cc $(DIMDIR)/$(LIBDIR)/libdim.a
	$(CXX) $(CFLAGS) $(SDIR)/hvGet.cc -L$(DIMDIR)/$(LIBDIR) -ldim -lncurses -o $(BDIR)/hvGet -L/opt/xdaq/lib -llog4cplus -lpthread

$(BDIR)/hvDimLogger:       $(SDIR)/hvDimLogger.cc $(DIMDIR)/$(LIBDIR)/libdim.a
	$(CXX) $(CFLAGS) $(LDFLAGS) $(SDIR)/hvDimLogger.cc -L$(DIMDIR)/$(LIBDIR) -ldim -o $(BDIR)/hvDimLogger 


$(ODIR)/ipc.o:  $(SDIR)/ipc.cc  ./include/ipc.h
	$(CXX) $(CFLAGS) -c $(SDIR)/ipc.cc -o $(ODIR)/ipc.o

$(ODIR)/pvss_db.o:      $(SDIR)/pvss_db.cc  ./include/pvss_db.h
	$(CXX) $(CFLAGS) -c $(SDIR)/pvss_db.cc -o $(ODIR)/pvss_db.o

$(ODIR)/pvss_common.o:  $(SDIR)/pvss_common.cc  ./include/pvss_common.h
	$(CXX) $(CFLAGS) -c $(SDIR)/pvss_common.cc -o $(ODIR)/pvss_common.o

$(BDIR)/hvNetLogger:       $(SDIR)/hvNetLogger.cc 
	$(CXX) $(CFLAGS) $(SDIR)/hvNetLogger.cc -L/opt/xdaq/lib -llog4cplus -lpthread -o $(BDIR)/hvNetLogger

# =================

clean:
	rm -f $(BDIR)/* $(LDIR)/libPrimaryPS.a $(ODIR)/*.o








