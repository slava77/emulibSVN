#
# Script to initialize the environment in a Bourne-like shell
#

if [ -z "$ORACLE_CERN" ]; then 
  echo "profile_oracle: Error: ORACLE_CERN not set"
  echo "                       Please contact Oracle Support."
else
# this else will be closed at the end of the file

ORACLE_MOUNT=$ORACLE_CERN/@sys; export ORACLE_MOUNT

if [ ! -d $ORACLE_MOUNT ] ; then
  echo "profile_oracle: Error: Value for @sys does not exist in Oracle "
  echo "                       Please contact Oracle Support"
else
TNS_ADMIN=$ORACLE_CERN/admin; export TNS_ADMIN

echo $PATH | grep $ORACLE_CERN/script > /dev/null
if [ $? -eq 1 ]; then
  PATH=$PATH:$ORACLE_CERN/script; export PATH
fi

if [ -z "$ORACLE_PATH" ]; then 
  ORACLE_PATH=$HOME:$ORACLE_CERN/sql:.
else
  echo $ORACLE_PATH | grep $ORACLE_CERN/sql > /dev/null
  if [ $? -eq 1 ]; then
    ORACLE_PATH=`echo $ORACLE_PATH`:$HOME:$ORACLE_CERN/sql:.
  fi
fi

#
# We accept an input parameter that can define if we want prod  (default)
# or test as working environment
#
case "$1" in
     'test') ORACLE_HOME=$ORACLE_MOUNT/test
             ;;
      'old') ORACLE_HOME=$ORACLE_MOUNT/old
             ;;
          *) ORACLE_HOME=$ORACLE_MOUNT/10201
             ;;
esac
 
export ORACLE_HOME
 
if [ -d "$ORACLE_HOME" ]; then
 
  . $ORACLE_CERN/script/baliases
 
#
# Same settings for Y2K in Nice/Unix. To solve problem with 8.x clients
# getting only the Century instead of the full year (20 versus 2000)
#
NLS_DATE_FORMAT='DD-MON-FXYYYY'; export NLS_DATE_FORMAT

#
# This is code that depends on the value of ORACLE_HOME
#
  . $ORACLE_CERN/script/oracle_Benv.sh
else
  echo "profile_oracle.sh : Error: $ORACLE_HOME does not exist"
  echo "Please contact Oracle Support "
fi
fi
# This last fi comes from the 'if' that tests if ORACLE_MOUNT is a valid 
# platform. For instance if the user is in a SGI, it will exit completely
# causing problems in the HepIx scripts; now we just skip all the initial 
# setting.

fi 

# to close initial test for ORACLE_CERN
