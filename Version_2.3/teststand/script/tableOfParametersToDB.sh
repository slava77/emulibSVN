#!/bin/zsh

# This script prints to stdout a summary table of parameters of all analyzed devices in the specified result directories. The parameters are obtained from the <device_id>.txt files.

if [[ ${#@} -eq 0 ]]; then
    print "No result directory specified. Exiting."
    print "Usage:"
    print "   $0 <resultDirecotry1> [<resultDirecotry2> ...]"
    exit 1
fi

print "   N                Board                       Run    Cut            Q           Ns           Gn         Offs           Qm          U20         Cint            Q           Ns           Rt           Mt          Slt"

((IDEVICE=1))
for RESULTDIR in $@; do
    for DEVICEFILE in ${RESULTDIR}/analyzed/[0-9\-]*.txt; do
	((IPAR=1))
	typeset -a DATA
	DATA[1]=${(l:4:: :::)IDEVICE}
	DATA[4]="     7"
	cat ${DEVICEFILE} | while read NAME VALUE; do
	    case $NAME in
		Run)        DATA[3]=${(l:25:: :::)VALUE};;
		Analysis)   ;;
		DeviceId)   DATA[2]=${(l:20:: :::)VALUE};;
		FailedCuts) ;;
		*)          DATA[$((IPAR+4))]=${(l:12:: :::)VALUE};;
	    esac
	    ((IPAR++))
	done
	print ${DATA}
	((IDEVICE++))
    done    
done
