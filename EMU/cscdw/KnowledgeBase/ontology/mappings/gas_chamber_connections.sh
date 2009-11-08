#!/bin/zsh
print "<!-- Generated from gas_chamber_connections.txt by gas_chamber_connections.sh -->"
print "<gasSystem>"
cat gas_chamber_connections.txt | while read LINE
do
    if [[ $LINE =~ "Supply" ]]; then
	LINEASARRAY=( $( echo $LINE) )
	print "    <gasChannel number=\"${LINEASARRAY[8]#*#}\" rackNumber=\"${LINEASARRAY[16]}\">"
	print "        <supply gasRack=\"${LINEASARRAY[8]}\" pipeline=\"${LINEASARRAY[5,7]}\"/>"
	print "        <chamber name=\"${LINEASARRAY[4]}\"/>"
	if [[ ${LINEASARRAY[9]} =~ "^ME[+-][1-4]/[1-3]/[0-3][0-9]$" ]]; then
	    print "        <chamber name=\"${LINEASARRAY[9]}\"/>"
	else
	    print "        <chamber name=\"${LINEASARRAY[9]%/*}/${LINEASARRAY[9][8,9]}\"/>"
	    print "        <chamber name=\"${LINEASARRAY[9]%/*}/${LINEASARRAY[9][11,12]}\"/>"
	fi
	print "        <chamber name=\"${LINEASARRAY[11]}\"/>"
	print "        <return gasRack=\"${LINEASARRAY[15]}\" pipeline=\"${LINEASARRAY[12,14]}\"/>"
	print "    </gasChannel>"
    fi
done
print "</gasSystem>"
