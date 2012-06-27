/*****************************************************************************\
* $Id: FEDException.h,v 1.4 2008/08/15 08:35:51 paste Exp $
*
* $Log: FEDException.h,v $
* Revision 1.4  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef FEDEXCEPTION_h
#define FEDEXCEPTION_h

#include "xcept/Exception.h"
#include <iostream>
#include <string>

//using namespace std;

/** An exception class that FED objects throw and catch.  This will hopefully
*	make debugging not as terrible a process as the old proceedural-based code
*	made it.
*
*	@note This is a bogus class that simply gives a name to an extension of
*	xdaq's xcept::Exception class.
*
*	@author Phillip Killewald
**/
namespace emu {
	namespace fed {

		class FEDException: public xcept::Exception
		{
		
		public:
		
			FEDException()
				throw ():
				xcept::Exception()
			{};
		
			FEDException( const std::string& name, const std::string& message, const std::string& module, int line, const std::string& function ):
				xcept::Exception(name, message, module, line, function)
			{};
		
			FEDException( const std::string& name, const std::string& message, const std::string& module, int line, const std::string& function,xcept::Exception& previous ):
				xcept::Exception(name, message, module, line, function, previous)
			{};
		};

	}
}

#endif