#!/usr/bin/python
#
# watch out, this is python 1.5.2 ... :-{
#
# gets called once per directory ...
#
# History (newest on top please) :
#
# 27-aug-2002, ap:  initial version
#

import os,os.path,sys,string
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
logName = logDir + "/taglog"

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

taggedDir = sys.argv[3]

# mail and log only top-level directory

words = string.split(taggedDir, "/")

# If you want to send mail only for tags on top-level dirs (e.g. in
# environment with /cvs/Infrastructure/<tld>), uncomment the next line
# if ( len(words) != 3 ) :  sys.exit(0)

logMsg1 = "date : " + now + "  author : " + author + "  "
logMsg2 = sys.argv[1] + " " + sys.argv[2] + " for " + taggedDir + "\n"

logFile = open(logName, 'a')
logFile.write(logMsg1 + logMsg2)
logFile.close()

try:
    mail = os.popen("/usr/lib/sendmail -t -f " + fromaddr, 'w')
    mail.write("To:" + toaddrs + "\r\n")
    mail.write("From:" + fromaddr + "\r\n")
    mail.write("Subject: CVS - tag " + os.path.basename(taggedDir) + "\r\n")
    mail.write("\n")
    mail.write(logMsg1 + "\n\n" + logMsg2)
    mail.write("\n")
    mail.close()
except:
    pass
