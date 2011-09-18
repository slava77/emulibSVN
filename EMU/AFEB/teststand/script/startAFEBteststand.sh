#!/bin/zsh

# user's options
while [[ $1 = -* ]]; do
    case $1 in 
        -d ) RUNINDEBUGGER="true"
            shift ;;
         * ) cat <<EOF
Usage:
    $0 [-d]

Options:
    -d    Run in a debugger (ddd)
EOF
	    exit 1 ;;
    esac
done

# BUILD_HOME, XDAQ_ROOT and ROOTSYS should be set. Their default values are:
export BUILD_HOME=${BUILD_HOME:-$HOME/EMU}
export XDAQ_ROOT=${XDAQ_ROOT:-$HOME/XDAQ}
export ROOTSYS=${ROOTSYS:-$HOME/ROOT/root}

export XDAQ_OS=linux
[[ $(uname -m) == "x86_64" ]] && export XDAQ_PLATFORM="x86_64" || export XDAQ_PLATFORM="x86"
export XDAQ_DOCUMENT_ROOT=${XDAQ_ROOT}/${XDAQ_PLATFORM}/htdocs
export LD_LIBRARY_PATH=$ROOTSYS/lib:$XDAQ_ROOT/$XDAQ_PLATFORM/lib

print
print "Environment variables:"
print "    BUILD_HOME         = $BUILD_HOME"
print "    XDAQ_ROOT          = $XDAQ_ROOT"
print "    XDAQ_OS            = $XDAQ_OS"
print "    XDAQ_PLATFORM      = $XDAQ_PLATFORM"
print "    XDAQ_DOCUMENT_ROOT = $XDAQ_DOCUMENT_ROOT"
print "    ROOTSYS            = $ROOTSYS"
print "    LD_LIBRARY_PATH    = $LD_LIBRARY_PATH"
print

OPTIONS="\
 -h localhost \
 -p 10000 \
 -c ${BUILD_HOME}/AFEB/teststand/xml/AFEBteststand.xml \
 -e ${BUILD_HOME}/AFEB/teststand/xml/AFEBteststand.profile"

if [[ $RUNINDEBUGGER = "true" ]]; then
    print "Execute in ddd the command:"
    print "run $OPTIONS"
    print
    eval "ddd ${XDAQ_ROOT}/${XDAQ_PLATFORM}/bin/xdaq.exe"
else
    print "Executing:"
    print "${XDAQ_ROOT}/${XDAQ_PLATFORM}/bin/xdaq.exe $OPTIONS"
    print
    eval "${XDAQ_ROOT}/${XDAQ_PLATFORM}/bin/xdaq.exe $OPTIONS"
fi
