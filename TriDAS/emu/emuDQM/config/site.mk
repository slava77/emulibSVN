#################################################################
#
# site.mk
#
#  Site definitions
#   Environment variables set in 'TrigDAQ.env'
#
#  Author: Holger Stoeck
#  Modifications:
#
#  $Id: site.mk,v 1.3 2006/10/18 17:09:53 pakhotin Exp $
#
#  Revision History
#  $Log: site.mk,v $
#  Revision 1.3  2006/10/18 17:09:53  pakhotin
#
#  Definitions of errors from DDU trailer were updated. Some histograms were updated.
#
#  Revision 1.2  2006/08/14 13:23:10  barvic
#  config changes for emuDQM
#
#  Revision 1.1  2006/03/15 13:30:32  barvic
#  build files changes
#
#  Revision 1.3  2005/02/08 15:53:18  tfcvs
#  HS - Commit changes before tagging
#
#  Revision 1.2  2004/11/16 10:54:48  tfcvs
#  changed default locations to TestBeam set-up
#
#  Revision 1.1  2004/10/01 11:42:25  tfcvs
#  *** empty log message ***
#
#  Revision 1.16  2004/04/15 19:48:20  tfcvs
#  HS - Added SBS version in TrigDAQ.env
#
#  Revision 1.15  2004/04/15 15:25:02  tfcvs
#  HS - Changes in scripts and makefiles
#
#  Revision 1.14  2003/11/21 01:21:20  tfcvs
#  HS - Added FED directory to config files
# 
#
#################################################################


EMU_ROOT = $(HOME)
ORCA_DIR = $(HOME)/ORCA
CMSSW	 = $(HOME)/local/CMSSW/src
BOOST    = $(HOME)/local/cms/lcg/external/boost/1.33.1/slc3_ia32_gcc323/include/boost-1_33_1
XDAQDIR    = $(XDAQ_DIR)
XERDIR     = $(XER_DIR)
HALDIR     = $(HAL_DIR)
VMEDIR     = $(VME_DIR)
ORCADIR    = $(ORCA_DIR)
EMUDIR     = $(EMUDAQ)
ROOTDIR    = $(ROOTSYS)

VMEADAPTER = ADAPTER_$(VME_ADAPTER)
SBSVERSION = $(SBS_VERSION)

FPGAMETHOD = FPGA_$(FPGA_METHOD)
BOARDTYPE  = BOARD_$(BOARD_TYPE)


#################################################################
#
# End of site.mk
#
#################################################################
