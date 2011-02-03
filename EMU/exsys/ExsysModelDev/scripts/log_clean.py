import sys
import os
import re
import getopt

FILES_TO_KEEP = 50

def usage():
  print "Usage: ", sys.argv[0], "-c 100 -d path_to_domain"
  print " where"
  print "   -c, --count=  :  number of files to keep, default = ", FILES_TO_KEEP
  print "   -d, --domain= :  path to domain"

def main():

  try:
    opts, args = getopt.getopt(sys.argv[1:], "c:d:", ["count=", "domain="])
  except getopt.GetoptError, err:
    print str(err)
    usage()
    sys.exit(2)

  domain_path = ""
  files_to_keep = FILES_TO_KEEP

  for o, a in opts:
    if o in ("-h", "--help"):
      usage()
      sys.exit()
    elif o in ("-d", "--domain"):
      domain_path = a
    elif o in ("-c", "--count"):
      files_to_keep = int(a)
    else:
      assert False, "unhandled option"

  if domain_path == "" or not os.path.isdir(domain_path):
    print "Path to domain bad or is not defined!"
    usage()
    sys.exit(1)

  logs_path = domain_path + "/logs"
  pat = re.compile("server.log_.*")
  f_rm = []
  f_kp = []

  if not os.path.isdir(logs_path):
    print "Path to domain logs is bad or is not defined!"
    usage()
    sys.exit(1)

  for f in sorted(os.listdir(logs_path), reverse=True):
    ff = logs_path + "/" + f
    if os.path.isfile(ff):
      if pat.match(f):
        if len(f_kp) < files_to_keep:
          f_kp.append(ff)
        else:
          f_rm.append(ff)

  for f in f_rm:
    print f
    #os.remove(f)

if __name__ == "__main__":
    main()

