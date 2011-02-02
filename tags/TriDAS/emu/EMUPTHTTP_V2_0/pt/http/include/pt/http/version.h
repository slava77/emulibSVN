// $Id: version.h,v 1.8 2011/01/25 18:32:18 banicz Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

//
// Version definition for HTTP peer transport
//
#ifndef _pt_http_h_
#define _pt_http_h_

#include "config/PackageInfo.h"
// !!! Edit this line to reflect the latest package version !!!

// #define PTHTTP_VERSION_MAJOR 3
// #define PTHTTP_VERSION_MINOR 12
// #define PTHTTP_VERSION_PATCH 2
#define EMUPTHTTP_VERSION_MAJOR 2
#define EMUPTHTTP_VERSION_MINOR 0
#define EMUPTHTTP_VERSION_PATCH 0
// If any previous versions available E.g. #define EMUPTHTTP_PREVIOUS_VERSIONS "3.8.0,3.8.1"
#undef EMUPTHTTP_PREVIOUS_VERSIONS


//
// Template macros
//
#define EMUPTHTTP_VERSION_CODE PACKAGE_VERSION_CODE(EMUPTHTTP_VERSION_MAJOR,EMUPTHTTP_VERSION_MINOR,EMUPTHTTP_VERSION_PATCH)
#ifndef EMUPTHTTP_PREVIOUS_VERSIONS
#define EMUPTHTTP_FULL_VERSION_LIST  PACKAGE_VERSION_STRING(EMUPTHTTP_VERSION_MAJOR,EMUPTHTTP_VERSION_MINOR,EMUPTHTTP_VERSION_PATCH)
#else 
#define EMUPTHTTP_FULL_VERSION_LIST  EMUPTHTTP_PREVIOUS_VERSIONS "," PACKAGE_VERSION_STRING(EMUPTHTTP_VERSION_MAJOR,EMUPTHTTP_VERSION_MINOR,EMUPTHTTP_VERSION_PATCH)
#endif 
namespace emupthttp
{
    const std::string package  =  "emupthttp";
    const std::string versions =  EMUPTHTTP_FULL_VERSION_LIST;
    const std::string summary = "HTTP peer transport with SOAP and CGI service implementations modified to comply with SOAP 1.1";
    const std::string description = "Based on daq/pt/http v3.12.02 (pthttp_S_03_12_02)";
    const std::string authors = "Johannes Gutleber, Luciano Orsini, Karoly Banicz";
    const std::string link = "http://xdaqwiki.cern.ch/index.php/Core_Tools";
    config::PackageInfo getPackageInfo();
    void checkPackageDependencies() throw (config::PackageInfo::VersionException);
    std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
