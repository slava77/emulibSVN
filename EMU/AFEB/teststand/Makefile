##
#
# This is the emusupervisor Makefile
#
##
include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)
PACKAGE_VER_MAJOR=01
PACKAGE_VER_MINOR=00
PACKAGE_VER_PATCH=00

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)


#
# Packages to be built
#
Project = AFEB
Package = teststand
PackageName= AFEBteststand
Description="Emu (CSC) AFEB test stand"
Summary="Emu (CSC) AFEB test stand"
Authors="Karoly Banicz"
Link=""

Sources = \
	Application.cc \
	DOM.cc \
	Cgi.cc \
	Xalan.cc \
	System.cc \
	CAMAC.cc \
	Crate.cc \
	LE32.cc \
	LeCroy3377.cc \
	Jorway73A.cc \
	Configuration.cc \
	Measurement.cc \
	TestedDevice.cc \
	Results.cc \
	version.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(BUILD_HOME)/AFEB/Jorway/include \
	$(ROOTSYS)/include

LibraryDirs = \
	$(XDAQ_ROOT)/lib

UserSourcePath =
UserCFlags =
#UserCCFlags = -g -Wall -pedantic-errors -Wno-long-long
UserCCFlags = -g -Wall -Wno-long-long
UserDynamicLinkFlags = $(ROOTLIBS)
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries =
ExternalObjects = $(BUILD_HOME)/AFEB/Jorway/lib/libsjy.a

#
# Compile the source files and create a shared library
#
DynamicLibrary = AFEBteststand
StaticLibrary =

TestLibraries=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
