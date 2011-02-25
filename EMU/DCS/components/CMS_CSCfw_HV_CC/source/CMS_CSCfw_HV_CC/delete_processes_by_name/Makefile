#***************************************************************************
#
# Purpose: makefile for the hv_display utility
#
#***************************************************************************

## name of the operating system
OS := $(shell uname)

## Directory structure
#HDIR = /home/fast/fast_daq/my_bsd_clientserver_closesocket
#HDIR = /afs/cern.ch/user/v/vsytnik/delete_processes_by_name
#/home/fast/fast_daq/delete_processes_by_name

BDIR = $(HOME)/bin
SDIR = ./src
ODIR = ./obj
IDIR = ./include
LDIR = ./lib

#***************************************************************************
## names for commands
CC         = gcc
F77        = g77

#***************************************************************************
## options for compiling

# Uncomment the next line when debugging
DEBUG      = -Wall -g

# Uncomment the next line when NOT debugging
#DEBUG      = -Wall -O

CCOPT      = $(DEBUG) -Wall -Df2cFortran
F77OPT     = $(DEBUG) -Wall -ff90 -fno-second-underscore

#***************************************************************************
## includes and linking X libraries and forms libs

#CXXFLAGS      = -O -Wall -fno-rtti -fno-exceptions -fPIC -I$(ROOTSYS)/include


#***************************************************************************
## other includes and libs

ifeq ($(OS),Linux)
             # True for CERN version of RH 5.1 and RH 5.2
  LINUX_VERSION := $(shell uname -r)
  ifeq ($(LINUX_VERSION),2.0.36)
    LIB    = /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.90.29/libf2c.a -lm -lfl
  else
    LIB    = -lg2c -lnsl -lm -lfl
  endif
endif

#***************************************************************************
## get object file names from source file names

#***************************************************************************

all : $(BDIR)/delete_processes_by_name

$(BDIR)/delete_processes_by_name: $(SDIR)/delete_processes_by_name.c Makefile 
	gcc $(CCOPT) $(CXXFLAGS) -o $(BDIR)/delete_processes_by_name \
        $(SDIR)/delete_processes_by_name.c

clean :
	cd obj ; rm -f *.o


