/*****************************************************************************\
* $Id: UtilsVersion.h,v 1.26 2009/11/02 18:26:21 liu Exp $
\*****************************************************************************/
#ifndef __EMU_FED_UTILSVERSION_H__
#define __EMU_FED_UTILSVERSION_H__

#include "config/PackageInfo.h"

namespace EmuFEDUtils {
	const std::string package     = "EmuFEDUtils";
	const std::string versions    = "09.00.00";
	const std::string description = "Emu (CSC) FED Utility Libraries";

	const std::string summary     = "Utility libraries used by the Front End Device (FED) crate XDAQ applications (FEDApps) for the CMS Emu (CSC) subdetector";
	const std::string authors     = "Phillip Killewald, Stan Durkin, Jason Gilmore, Jianhui Gu";
	const std::string link        = "http://www.physics.ohio-state.edu/~cms";

	config::PackageInfo getPackageInfo();

	void checkPackageDependencies()
	throw (config::PackageInfo::VersionException);

	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

