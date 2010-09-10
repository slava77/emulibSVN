# .bash_profile

# Get the aliases and functions
if [ -f ~/.bashrc ]; then
	. ~/.bashrc
fi

# User specific environment and startup programs

PATH=$PATH:$HOME/bin

export DIM_DNS_NODE=`hostname`
#dcspcS2G19-0

# ProjectHome should be commented out as it is redefined in the DCS-s scripts 
export ProjectHome=/cmssrv0/nfshome0/cscdcsdev/csc_station_p2

export PVSS_II_HOME=/opt/pvss/pvss2_v3.6
export Framework=/cmssrv0/nfshome0/cscdcsdev/fwComponents_320
#export Framework=/home/cscdcs/csc_station

export PVSS_II_PATH=$PVSS_II_HOME
export PVSS_SYSTEM_ROOT=$PVSS_II_HOME
export PVSS_II=$ProjectHome/config/config




PATH=$PVSS_II_HOME/bin:/opt/pvss:$PATH # it is important to put  $PATH at the end tp prevent call of previous pvss version from /usr/local

#export LD_LIBRARY_PATH=/opt/pvss/pvss2_v3.6/api/lib.linux_RH90:/opt/pvss/pvss2_v3.6/bin:/usr/local/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$PVSS_II_HOME/api/lib.linux:$PVSS_II_HOME/bin:/usr/local/lib:$LD_LIBRARY_PATH
export API_ROOT=$PVSS_II_HOME/api

    export PATH=$PATH:$ProjectHome/bin:$Framework/bin
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$Framework/bin

# nedit important for the PVSS on Linux
alias xemacs='nedit'

#----- orcale part -----
#the  /afs/cern.ch/project/oracle/admin/tnsnames.ora should be place to $HOME
export TNS_ADMIN=$HOME
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/oracle/10.2.0.3/client/lib
#---------------------

alias xemacs='nedit'

export PATH

unset USERNAME

