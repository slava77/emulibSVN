#include "EmuFarmerV.h"

#include "toolbox/version.h"
#include "xdata/version.h"
#include "xdaq/version.h"
#include "xoap/version.h"

#include <string>
#include <set>

GETPACKAGEINFO(EmuFarmer)

void EmuFarmer::checkPackageDependencies() throw (config::PackageInfo::VersionException)
{
    CHECKDEPENDENCY(toolbox)
    CHECKDEPENDENCY(xdata)
    CHECKDEPENDENCY(xdaq)
    CHECKDEPENDENCY(xoap)
}

std::set<std::string, std::less<std::string> > EmuFarmer::getPackageDependencies()
{
    std::set<std::string, std::less<std::string> > dependencies;


    ADDDEPENDENCY(dependencies,toolbox);
    ADDDEPENDENCY(dependencies,xdata);
    ADDDEPENDENCY(dependencies,xdaq);
    ADDDEPENDENCY(dependencies,xoap);

    return dependencies;
}