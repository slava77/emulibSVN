//
// EmuMonitorV.cc

#include "EmuMonitorV.h"

#include "toolboxV.h"
#include "xoapV.h"
#include "xdaqV.h"

GETPACKAGEINFO(EmuMonitor)

void EmuMonitor::checkPackageDependencies()
		throw (toolbox::PackageInfo::VersionException)
{
	CHECKDEPENDENCY(toolbox)
	CHECKDEPENDENCY(xoap)
	CHECKDEPENDENCY(xdaq)
}

set<string, less<string> > xdaq::getPackageDependencies()
{
	set<string, less<string> > dependencies;

	ADDDEPENDENCY(dependencies, toolbox);
	ADDDEPENDENCY(dependencies, xoap);
	ADDDEPENDENCY(dependencies, xdaq);

	return dependencies;
}
