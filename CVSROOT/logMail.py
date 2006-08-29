#!/usr/bin/python
#
# watch out, this is python 1.5.2 ... :-{
#
# gets called once per directory ...
#
# History (newest on top please) :
#
# 27-aug-2002, ap:  more checks to get it running ...
# 27-aug-2002, ap:  initial version
#

import os, os.path, sys, string
import smtplib
import time

# TO BE MODIFIED: replace ??? by your repository
cvsDir  = "/local/reps/emu"

# TO BE MODIFIED: put your librarian's e-mail address 
# (usually just replace ??? by your repository)
fromaddr = "emu.CVSLibrarian@cern.ch"

# TO BE MODIFIED: provide a comma-separated list of e-mail addresses
toaddrs  = "Frank.Geurts@cern.ch"

# the place (and name) of the logfile (by default top level in repository)
logDir  = cvsDir + "/logs"
logName = logDir + "/commitlog"

# from here on there should no mods be needed ...

now    = time.asctime(time.localtime(time.time()))

# try to get "real" username, fallback to $USER (usually cvsuser)
try:
    author = os.environ["CVS_USER"]
except:
    author = os.environ["USER"]

# create log dir if not yet there:
if ( not os.path.exists(logDir) ) :
    os.mkdir(logDir)

# get list of files to process ...
files = string.split( string.join(sys.argv[1:]) )

# mail and log

logMsg1 = "date : " + now + "  author : " + author + "   "

logMsg2 = string.join(files[1:], "\n")
logMsg2 = logMsg2 + "\n"

logFile = open(logName, 'a')
logFile.write(logMsg1 + "in " + files[0] + " : " + logMsg2)
logFile.close()

logMsg3 = ""
for line in sys.stdin.readlines() :
    logMsg3 = logMsg3 + line

mailMsg = logMsg1 + "\n"
mailMsg = mailMsg + "\n" + logMsg3

try:
    mail = os.popen("/usr/lib/sendmail -t -f " + fromaddr, 'w')
    mail.write("To:" + toaddrs + "\r\n")
    mail.write("From:" + fromaddr + "\r\n")
    mail.write("Subject: CVS - commit " + author + " - " + files[0] + "\r\n")
    mail.write("\n")
    mail.write(mailMsg)
    mail.write("\n")
    mail.close()
except:
    pass

