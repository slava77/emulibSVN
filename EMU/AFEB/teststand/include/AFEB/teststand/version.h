#ifndef __AFEB_teststand_version_h__
#define __AFEB_teststand_version_h__

#include "config/PackageInfo.h"

namespace AFEBteststand {
	const std::string package     = "AFEBteststand";
	const std::string versions    = "02.01.00";
	const std::string description = "Emu (CSC) AFEB Teststand";

	const std::string summary     = "AFEB/teststand";
	const std::string authors     = "K. Banicz";
	const std::string link        = "";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif

