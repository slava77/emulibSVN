/*****************************************************************************\
* $Id: AppsVersion.h,v 1.13 2009/04/28 02:23:42 paste Exp $
\*****************************************************************************/

#ifndef __EMU_FED_APPSVERSION_H__
#define __EMU_FED_APPSVERSION_H__

#include "config/PackageInfo.h"

namespace emufedapps {
	const std::string package     = "FEDApps";
	const std::string versions    = "07.03.00";
	const std::string description = "Emu (CSC) FED XDAQ Applications";

	const std::string summary     = "Applications to control and monitor the Front End Device (FED) crates for the CMS Emu (CSC) subdetector";
	const std::string authors     = "Phillip Killewald, Stan Durkin, Jason Gilmore, Jianhui Gu";
	const std::string link        = "http://www.physics.ohio-state.edu/~cms";

	config::PackageInfo getPackageInfo();

	void checkPackageDependencies()
	throw (config::PackageInfo::VersionException);

	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

