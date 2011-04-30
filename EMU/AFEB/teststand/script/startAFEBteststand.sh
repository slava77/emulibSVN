#!/bin/zsh

export BUILD_HOME=/home/csctest/EMU
export XDAQ_ROOT=/home/csctest/XDAQ
export XDAQ_OS=linux
export XDAQ_PLATFORM=x86

export XDAQ_DOCUMENT_ROOT=${XDAQ_ROOT}/${XDAQ_PLATFORM}/htdocs

#ddd ${XDAQ_ROOT}/${XDAQ_PLATFORM}/bin/xdaq.exe
# -h emuslice03.cern.ch -p 10000 -c ${BUILD_HOME}/AFEB/teststand/xml/AFEBteststand.xml -e ${BUILD_HOME}/AFEB/teststand/xml/AFEBteststand.profile

${XDAQ_ROOT}/${XDAQ_PLATFORM}/bin/xdaq.exe \
    -h emuslice03.cern.ch \
    -p 10000 \
    -c ${BUILD_HOME}/AFEB/teststand/xml/AFEBteststand.xml \
    -e ${BUILD_HOME}/AFEB/teststand/xml/AFEBteststand.profile
