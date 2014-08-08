// Module:  Log4CPLUS
// File:    dimappender.cxx
// Created: 6/2006
// Author:  Victor Barashko
//
//
// Copyright (C) Victor Barashko  All rights reserved.
//
// This software is published under the terms of the Apache Software
// License version 1.1, a copy of which has been included with this
// distribution in the LICENSE.APL file.
//
// $Log: dimappender.cxx,v $
//

#include <log4cplus/layout.h>
#include <log4cplus/dimappender.h>
#include <log4cplus/streams.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/spi/loggingevent.h>

using namespace std;
using namespace log4cplus::helpers;


//////////////////////////////////////////////////////////////////////////////
// log4cplus::DimAppender ctors and dtor
//////////////////////////////////////////////////////////////////////////////



log4cplus::DimAppender::DimAppender(const log4cplus::helpers::Properties properties)
    : Appender(properties)
{
}

log4cplus::DimAppender::DimAppender(DimService* svc)
    : logsvc(svc)
{
}



log4cplus::DimAppender::~DimAppender()
{
  destructorImpl();
}



//////////////////////////////////////////////////////////////////////////////
// log4cplus::DimAppender public methods
//////////////////////////////////////////////////////////////////////////////

void
log4cplus::DimAppender::close()
{
  getLogLog().debug(LOG4CPLUS_TEXT("Entering DimAppender::close().."));
  closed = true;
}



//////////////////////////////////////////////////////////////////////////////
// log4cplus::DimAppender protected methods
//////////////////////////////////////////////////////////////////////////////

// Normally, append() methods do not need to be locked since they are
// called by doAppend() which performs the locking.  However, this locks
// on the LogLog instance, so we don't have multiple threads writing to
// tcout and tcerr
void
log4cplus::DimAppender::append(const spi::InternalLoggingEvent& event)
{
  LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( getLogLog().mutex )
  log4cplus::tostringstream output;
  output << logsvc->getName() << "|";
  layout->formatAndAppend(output, event);
  // std::cout << "DimLogger-> " << LOG4CPLUS_TSTRING_TO_STRING(output.str()) << std::endl;
  logsvc->updateService(const_cast<char *>(LOG4CPLUS_TSTRING_TO_STRING(output.str()).c_str()));

  // if(immediateFlush) {
  //     output.flush();
  // }
  LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
}


