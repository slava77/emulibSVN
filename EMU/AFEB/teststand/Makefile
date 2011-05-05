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
	version.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include

LibraryDirs = $(XDAQ_ROOT)/lib

UserSourcePath =
UserCFlags =
UserCCFlags = -g -Wall -pedantic-errors -Wno-long-long
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries =
ExternalObjects = 

#
# Compile the source files and create a shared library
#
DynamicLibrary = AFEBteststand
StaticLibrary =

TestLibraries=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
