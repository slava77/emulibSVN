// Module:  Log4CPLUS
// File:    dimappender.h
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

/** @file */

#ifndef _LOG4CPLUS_DIM_APPENDER_HEADER_
#define _LOG4CPLUS_DIM_APPENDER_HEADER_

#include <log4cplus/config.hxx>
#include <log4cplus/appender.h>
#include <dis.hxx>

namespace log4cplus
{
/**
 * DimAppender appends log events to <code>Dim Service</code>
 * using a layout specified by the
 * user.
 */
class LOG4CPLUS_EXPORT DimAppender : public Appender
{
public:
  // Ctors
  DimAppender(const log4cplus::helpers::Properties properties);
  DimAppender(DimService* svc);

  // Dtor
  ~DimAppender();

  // Methods
  virtual void close();

protected:
  virtual void append(const spi::InternalLoggingEvent& event);

  bool immediateFlush;
  DimService* logsvc;
};

} // end namespace log4cplus

#endif // _LOG4CPLUS_DIM_APPENDER_HEADER_

