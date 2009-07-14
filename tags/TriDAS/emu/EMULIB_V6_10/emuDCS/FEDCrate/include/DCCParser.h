/*****************************************************************************\
* $Id: DCCParser.h,v 3.8 2009/01/29 15:31:22 paste Exp $
*
* $Log: DCCParser.h,v $
* Revision 3.8  2009/01/29 15:31:22  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.7  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.6  2008/08/15 10:40:20  paste
* Working on fixing CAEN controller opening problems
*
* Revision 3.4  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __DCCPARSER_H__
#define __DCCPARSER_H__

#include "Parser.h"
#include "FEDException.h"

namespace emu {
	namespace fed {

		class DCC;

		/** @class DCCParser A parser that builds DCC objects to be loaded into a FEDCrate.
		*	@sa DCC
		**/
		class DCCParser: public Parser
		{
		
		public:

			/** Default constructor.
			*
			*	@param pNode the XML DOM element node to parse.
			**/
			explicit DCCParser(xercesc::DOMElement *pNode)
			throw (ParseException);
				
			/** @returns a pointer to the parsed DCC object. **/
			inline DCC *getDCC() { return dcc_; }
		
		private:

			/// A DCC object built from the parsed attributes of the DOM node.
			DCC *dcc_;
		};

	}
}

#endif





