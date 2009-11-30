/*****************************************************************************\
* $Id: VMEControllerDBAgent.h,v 1.4 2009/11/09 11:46:32 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_VMECONTROLLERDBAGENT_H__
#define __EMU_FED_VMECONTROLLERDBAGENT_H__

#include "emu/fed/DBAgent.h"

#include <vector>
#include <string>

#include "emu/fed/Exception.h"
#include "xdata/UnsignedShort.h"

namespace emu {
	namespace fed {
		
		class VMEController;

		/** @class VMEControllerDBAgent A utility class that will download a Controller configuration from the database and properly configure a VMEController object based on that information.
		**/
		class VMEControllerDBAgent: public DBAgent {
		
		public:

			/** Default constructor.
			**/
			VMEControllerDBAgent(xdaq::WebApplication *application);

			/** Build a controller corresponding to a given crate ID **/
			emu::fed::VMEController *getController(xdata::UnsignedInteger64 &id)
			throw (emu::fed::exception::DBException);
			
			/** Build a controller corresponding to a configuration key and a crate number **/
			emu::fed::VMEController *getController(xdata::UnsignedInteger64 &key, xdata::UnsignedShort &number)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build the controller from the table returned **/
			emu::fed::VMEController *buildController(xdata::Table &table)
			throw (emu::fed::exception::DBException);
			
		};

	}
}

#endif





