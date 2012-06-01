#!/bin/zsh

# This script should be redundant as the analysis program already produces the same output, namely a summary plain text file for each analyzed device.

if [[ ${#@} -eq 0 ]]; then
    print "No result directory specified. Exiting."
    print "Usage:"
    print "   $0 <resultDirecotry1> [<resultDirecotry2> ...]"
    exit 1
fi

# BUILD_HOME should be set. Its default value is:
export BUILD_HOME=${BUILD_HOME:-$PWD/${0:h}/../../..}

cd ${BUILD_HOME}/AFEB/teststand/xml

for RESULTDIR in $@; do
    xsltproc failedCutsListerGenerator.xsl ${RESULTDIR}/analyzed/SelectionCuts.xml > failedCutsList.xsl
    for DEVICEFILE in ${RESULTDIR}/analyzed/[0-9\-]*.xml; do
	xsltproc parametersToDB.xsl $DEVICEFILE > ${DEVICEFILE:r}.txt
	print "Wrote ${DEVICEFILE:r}.txt"
    done
done
