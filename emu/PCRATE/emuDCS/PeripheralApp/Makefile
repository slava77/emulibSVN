##
#
# This is the TriDAS/emu/emuDCS/PeripheralApps Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)
include $(BUILD_HOME)/emu/rpm_version

# Packages to be built
#
Project=emu
Package=emuDCS/PeripheralApp
PackageName=PeripheralApp
Description="XDAQ Application libraries for Peripheral Crates"

Sources=\
        Application.cc \
        AppNavigator.cc \
        Configurator.cc \
        Configurator_CCBMPC.cc \
        Configurator_DDU.cc \
        Configurator_DMB.cc \
        Configurator_VCC.cc \
        VMEAccessor.cc \
        Configuration.cc \
        AppsVersion.cc \
        CalibDAQ.cc \
        ChamberUtilities.cc \
	CrateUtilities.cc \
        XMLParser.cc

#Sources=\
#        Application.cc \
#        AppNavigator.cc \
#        Configurator.cc \
#        Configurator_CCBMPC.cc \
#        Configurator_DDU.cc \
#        Configurator_DMB.cc \
#        Configurator_VCC.cc \
#        VMEAccessor.cc \
#        Configuration.cc \
#        AppsVersion.cc \
#        CalibDAQ.cc \
#        ChamberUtilities.cc \
#	CrateUtilities.cc \
#        XMLParser.cc \
#        EmuPCrateConfigTStore.cc \
#        EmuPeripheralCrateBase.cc \
#	EmuPeripheralCrateBroadcast.cc \
#        EmuPeripheralCrateConfig.cc \
#        EmuPeripheralCrateConfig_VCC.cc \
#        EmuPeripheralCrateConfig_CCBMPC.cc \
#        EmuPeripheralCrateConfig_DMB.cc \
#        EmuPeripheralCrateConfig_DDU.cc \
#        EmuPeripheralCrateCommand.cc \
#        EmuPeripheralCrateMonitor.cc \
#        EmuPeripheralCrateTimer.cc \
#        EmuPeripheralCrateManager.cc \
#        EmuPeripheralCrateService.cc \
#        EmuCompareMcsFiles.cc \
#        EmuEndcapConfigWrapper.cc

IncludeDirs = \
        $(XDAQ_ROOT)/include \
        $(BUILD_HOME)/emu/emuDCS/PeripheralCore/include \
	$(BUILD_HOME)/emu/emuDCS/OnlineDB/include \
        $(BUILD_HOME)/emu/base/include

TestLibraryDirs = \
	$(XDAQ_ROOT)/lib \
        $(BUILD_HOME)/emu/emuDCS/PeripheralCore/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(BUILD_HOME)/emu/emuDCS/PeripheralApp/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(BUILD_HOME)/emu/base/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

UserCFlags  = 
UserCCFlags = 
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer
TestLibraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer \
                mimetic logxmlappender logudpappender asyncresolv config \
                CSCSupervisor EmuPeripheralCore EmuPeripheralApp uuid
                
#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuPeripheralApp
StaticLibrary= 

Executables=
TestExecutables= \
	slicetestconfig.cpp \
	EmuConfig.cpp \
	CrateTiming.cpp \
	parsemain.cpp \
	mpc2sp.cpp \
	ParserTester.cpp \
	loadALCT.cpp \
	loadTMB.cpp \
        resetcc.cpp \
        scancrate.cpp \
	loadMPC.cpp \
	loadCCB.cpp\
	loadDMB.cpp \
	testtmbmain.cpp

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
