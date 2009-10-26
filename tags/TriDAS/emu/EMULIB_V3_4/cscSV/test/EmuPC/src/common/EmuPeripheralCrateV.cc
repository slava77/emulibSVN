//
// EmuPeripheralCrateV.cc

#include "EmuPeripheralCrateV.h"

#include "toolboxV.h"
#include "xoap/version.h"
#include "xdaqV.h"

GETPACKAGEINFO(EmuPeripheralCrate)

void EmuPeripheralCrate::checkPackageDependencies()
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

// End of file
// vim: set ai sw=4 ts=4: