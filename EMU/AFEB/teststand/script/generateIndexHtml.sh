#!/bin/zsh

# Creates an index.html file in each directory listed as arguments and their subdirectories.

if [[ ${#@} -eq 0 ]]; then
    print "No directory specified. Exiting."
    print "Usage:"
    print "   $0 <resultDirecotry1> [<resultDirecotry2> ...]"
    exit 1
fi

function dirList(){
    local D=$1
    [[ -d $D ]] || return;
    cd $D
    # Generate index.html
    print "Generating $D/index.html"
    {
	print "<html>"
	print "<head><title>$D</title><head>"
	print "<body>"
	# In directories 'raw' and 'analyzed', list links to result files.
	if [[ ${D:t} == "raw" && -f results.xml ]]; then
	    print "<h2><a href=\"results.xml\">Raw results</a> of run at ${D:h:t}</h2><hr/>"
	fi
	if [[ ${D:t} == "analyzed" ]]; then
	    print "<h2>Parameters of devices"
	    for PAR in [0-9]*.xml; do
		print " <a href=\"${PAR}\">${PAR:t:r}</a>"
	    done
	    print "</h2><h2>Plots of devices"
	    for PDF in [0-9]*.pdf; do
		print " <a href=\"${PDF}\">${PDF:t:r}</a>"
	    done
	    print "</h2><hr/>"
	fi
	# List directory contents in a table.
	print "<table style=\"font-size:small;\"><tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>"
	ls -l --time-style=long-iso $D | while read LINE; do
	    FIELDS=( $(print -- $LINE) )
	    if [[ ${#FIELDS} -eq 8 && ${FIELDS[8]} != "index.html" ]]; then
		print "<tr><td><a href=\"${FIELDS[8]}\">${FIELDS[8]}</a></td><td style=\"text-align:right;\">${FIELDS[5]}</td><td>${FIELDS[6]}&nbsp;${FIELDS[7]}</td></tr>"
	    fi
	done
	print "<table>"
	print "</body>"
	print "</html>"
    } > index.html
    # Descend into subdirectories to do the same
    for SUBD in $(print $D/*(/N)); do
	dirList $SUBD
    done
}

for A in "$@"; do
    dirList $A
done
