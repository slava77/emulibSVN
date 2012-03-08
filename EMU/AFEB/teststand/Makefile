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

ROOTCFLAGS    = $(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --libs)
ROOTGLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs)

info:
	@echo "Values of required variables inside Makefile:"
	@echo "   BUILD_HOME=$(BUILD_HOME)"
	@echo "   XDAQ_ROOT =$(XDAQ_ROOT)"
	@echo "   ROOTSYS   =$(ROOTSYS)"

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
	Configuration.cc \
	Measurement.cc \
	Calibration.cc \
	TestedDevice.cc \
	AnalyzedChannel.cc \
	AnalyzedDevice.cc \
	Results.cc \
	Analysis.cc \
	DAC.cc \
	Legend.cc \
	version.cc \
	utils/DOM.cc \
	utils/Cgi.cc \
	utils/Xalan.cc \
	utils/System.cc \
	utils/String.cc \
	utils/Xcept.cc \
	crate/Module.cc \
	crate/Crate.cc \
	crate/CrateController.cc \
	crate/LE32.cc \
	crate/LeCroy3377.cc \
	crate/Jorway73A.cc \
	fit/StraightLine2D.cc

TestSources =
TestIncludeDirs =

IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(BUILD_HOME)/AFEB/Jorway/include
LibraryDirs =

UserSourcePath =
UserCFlags = $(ROOTCFLAGS)
#UserCCFlags = -g -Wall -pedantic-errors -Wno-long-long
UserCCFlags = -g -Wall -Wno-long-long $(ROOTCFLAGS)
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

# To build tests:
#     export BUILD_HOME=... 
#     make tests
# To run it:
#     LD_LIBRARY_PATH=$BUILD_HOME/AFEB/teststand/lib/$XDAQ_OS/$XDAQ_PLATFORM:$XDAQ_ROOT/lib:$ROOTSYS/lib $BUILD_HOME/AFEB/teststand/test/$XDAQ_OS/$XDAQ_PLATFORM/fitterTest.exe
TestLibraries=xcept toolbox log4cplus xdata xerces-c xalan-c xdaq xalanMsg xoap xgi cgicc pthttp peer asyncresolv mimetic config logudpappender logxmlappender uuid
TestExecutables=fitterTest.cc analyze.cc


# To build fitterTest:
#     export BUILD_HOME=... 
#     make fitterTest
# To run it:
#     $BUILD_HOME/AFEB/teststand/test/$XDAQ_OS/$XDAQ_PLATFORM/fitterTest.exe
fitterTest:
	mkdir -p $(BUILD_HOME)/AFEB/teststand/test/$(XDAQ_OS)/$(XDAQ_PLATFORM)
	g++ $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS) -I$(BUILD_HOME)/AFEB/teststand/include \
	-o $(BUILD_HOME)/AFEB/teststand/test/$(XDAQ_OS)/$(XDAQ_PLATFORM)/fitterTest.exe \
	$(BUILD_HOME)/AFEB/teststand/test/common/fitterTest.cc \
	$(BUILD_HOME)/AFEB/teststand/src/common/fit/StraightLine2D.cxx

analyze:
	mkdir -p $(BUILD_HOME)/AFEB/teststand/test/$(XDAQ_OS)/$(XDAQ_PLATFORM)
	g++ $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS) -I$(BUILD_HOME)/AFEB/teststand/include -I$(XDAQ_ROOT)/include  -I$(XDAQ_ROOT)/include/$(XDAQ_OS) \
	-L $(XDAQ_ROOT)/lib \
	-lxcept -ltoolbox -llog4cplus -lxdata -lxerces-c -lxalan-c -lxalanMsg -lxoap -lasyncresolv -lmimetic -lconfig -luuid \
	-o $(BUILD_HOME)/AFEB/teststand/test/$(XDAQ_OS)/$(XDAQ_PLATFORM)/analyze.exe \
	$(BUILD_HOME)/AFEB/teststand/test/common/analyze.cc \
	$(BUILD_HOME)/AFEB/teststand/src/common/Analysis.cc \
	$(BUILD_HOME)/AFEB/teststand/src/common/utils/DOM.cc \
	$(BUILD_HOME)/AFEB/teststand/src/common/utils/String.cc \
	$(BUILD_HOME)/AFEB/teststand/src/common/utils/System.cc

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
