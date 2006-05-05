#ifndef __LTC_CONTROL_V_H__
#define __LTC_CONTROL_V_H__

#include "PackageInfo.h"

namespace LTCControl {
	const string package     = "LTCControl";
	const string versions    = "1.0";
	const string description = "dummy XDAQ application to test CSC Supervisor";
	const string link        = "http://localhost:40000/";

	toolbox::PackageInfo getPackageInfo();
	void checkPackageDependencies() throw (toolbox::PackageInfo::VersionException);
	set<string, less<string> > getPackageDependencies();
};

#endif  // ifndef __LTC_CONTROL_V_H__
// vim: set ai sw=4 ts=4:
