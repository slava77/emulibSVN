#ifndef __EMU_PERIPHERAL_CRATE_V_H__
#define __EMU_PERIPHERAL_CRATE_V_H__

#include "config/PackageInfo.h"

namespace EmuPeripheralCrate {
	const std::string package     = "EmuPeripheralCrate";
	const std::string versions    = "3.0";
	const std::string description = "dummy EmuPeripherlCrate";

	const std::string summary     = "emu/cscSV";
	const std::string authors     = "Ichiro Suzuki";
	const std::string link        = "http://localhost:40000/";

	config::PackageInfo getPackageInfo();
	void checkPackageDependencies()
			throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
};

#endif  // ifndef __EMU_PERIPHERAL_CRATE_V_H__
// vim: set sw=4 ts=4: