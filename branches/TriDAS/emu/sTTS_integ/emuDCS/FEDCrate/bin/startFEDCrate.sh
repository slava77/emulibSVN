#!/bin/sh
# Source function library.
if [ -f /etc/init.d/functions ] ; then
        . /etc/init.d/functions
else
    echo -n 'ERROR xdaqjc: Could not find /etc/init.d/functions, stopping'
    exit -1
fi
#
#killproc xdaq.exe
killall -q $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.exe
#
cd $XDAQ_ROOT/emu/emuDCS/FEDCrate/
nohup $XDAQ_ROOT/daq/xdaq/scripts/xdaq.sh -p 40100 -c xml/EmuFCluster.xml -l INFO >& ~/log/xdaq.log &
