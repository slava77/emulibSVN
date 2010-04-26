#!/bin/sh
export XDAQ_OS=linux
XDAQ_PLATFORM=x86
export XDAQ_ROOT=/opt/xdaq
PATH=$PATH:$XDAQ_ROOT/bin:$XDAQ_ROOT/sbin:$XDAQ_ROOT/daq/xdaq/bin/$XDAQ_OS/$XDAQ_PLATFORM/

export XDAQ_OS XDAQ_PLATFORM

LD_LIBRARY_PATH=$BUILD_HOME/lib:$XDAQ_ROOT/lib:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/$XDAQ_PLATFORM/lib:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/xdaq/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/extern/xerces/$XDAQ_OS$XDAQ_PLATFORM/lib/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/extern/cgicc/$XDAQ_OS$XDAQ_PLATFORM/lib/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/extern/asyncresolv/$XDAQ_OS$XDAQ_PLATFORM/lib/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/extern/log4cplus/$XDAQ_OS$XDAQ_PLATFORM/lib/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/extern/log4cplus/udpappender/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/extern/log4cplus/xmlappender/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/extern/mimetic/$XDAQ_OS$XDAQ_PLATFORM/lib/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/toolbox/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/xoap/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/xdata/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/xgi/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/xcept/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/extern/log4cplus/$XDAQ_OS$XDAQ_PLATFORM/lib/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/pt/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/pt/soap/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/pt/fifo/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$XDAQ_ROOT/daq/i2o/utils/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH

LD_LIBRARY_PATH=$EMUDAQ/emuReadout/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH

LD_LIBRARY_PATH=$EMUDQM/dduBinExaminer/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$EMUDQM/EmuROOTDisplayServer/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$EMUDQM/emuMonitor/lib/$XDAQ_OS/$XDAQ_PLATFORM/:$LD_LIBRARY_PATH

CONSUMER_ICONS=$EMUDQM/EmuROOTDisplayServer/res

export LD_LIBRARY_PATH CONSUMER_ICONS

export XDAQ_ROOT=/nfshome0/csctfpro/TriDAS/