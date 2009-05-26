/*****************************************************************************\
* $Id: FiberParser.h,v 1.1 2009/05/21 15:30:48 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_FIBERPARSER_H__
#define __EMU_FED_FIBERPARSER_H__

#include "emu/fed/Parser.h"
#include "emu/fed/Exception.h"

namespace emu {
	namespace fed {

		class Fiber;

		/** @class FiberParser A parser that builds Fiber objects to be loaded into the DDU.
		*	@sa Fiber
		**/
		class FiberParser: public Parser
		{
		
		public:

			/** Default constructor.
			*
			*	@param pNode the XML DOM element node to parse.
			**/
			explicit FiberParser(xercesc::DOMElement *pNode)
			throw (emu::fed::exception::ParseException);

			/** @returns a pointer to the parsed Fiber object. **/
			inline Fiber *getFiber() { return fiber_; }

			/** @returns whether or not the chamber is marked to be masked off (killed) at the DDU. **/
			inline const bool isKilled() { return killed_; }

			/** @returns the parsed fiber number of the chamber. **/
			inline const unsigned int getNumber() { return number_; }
		
		private:

			/// A Fiber object built from the parsed attributes of the DOM node.
			Fiber *fiber_;

			/// Whether or not the fiber is killed.
			bool killed_;

			/// The fiber number of the chamber.
			unsigned int number_;
		};

	}
}

#endif
