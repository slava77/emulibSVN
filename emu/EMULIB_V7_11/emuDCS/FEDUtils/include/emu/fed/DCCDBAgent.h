/*****************************************************************************\
* $Id: DCCDBAgent.h,v 1.4 2009/08/20 13:41:01 brett Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DCCDBAGENT_H__
#define __EMU_FED_DCCDBAGENT_H__

#include "emu/fed/DBAgent.h"

#include <vector>
#include <string>

#include "emu/fed/Exception.h"
#include "xdata/UnsignedInteger.h"
#include "xdata/UnsignedShort.h"

namespace emu {
	namespace fed {
	
		class DCC;

		/** @class DCCDBAgent A utility class that will download a Controller configuration from the database and properly configure a DCC object based on that information.
		**/
		class DCCDBAgent: public DBAgent {
		
		public:

			/** Default constructor **/
			DCCDBAgent(xdaq::WebApplication *application)
			throw (emu::fed::exception::DBException);
			
			/** Build a bunch of DCCs corresponding to a crate ID **/
			std::map<xdata::UnsignedInteger64, emu::fed::DCC *, emu::fed::DBAgent::comp> getDCCs(xdata::UnsignedInteger64 &key,xdata::UnsignedShort &crateNumber)
			throw (emu::fed::exception::DBException);
			
			/** Build a bunch of DCC objects corresponding to a configuration key and an RUI **/
			std::map<xdata::UnsignedInteger64, emu::fed::DCC *, emu::fed::DBAgent::comp> getDCCs(xdata::UnsignedInteger64 &key, xdata::UnsignedInteger &fmm_id)
			throw (emu::fed::exception::DBException);

		private:
		
			/** Build the crates from the table returned **/
			std::map<xdata::UnsignedInteger64, emu::fed::DCC *, emu::fed::DBAgent::comp> buildDCCs(xdata::Table &table)
			throw (emu::fed::exception::DBException);

		};

	}
}

#endif






