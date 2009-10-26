/*****************************************************************************\
* $Id: Chamber.h,v 1.9 2008/09/19 16:53:51 paste Exp $
*
* $Log: Chamber.h,v $
* Revision 1.9  2008/09/19 16:53:51  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 1.8  2008/08/26 13:40:08  paste
* Updating and adding documentation
*
* Revision 1.6  2008/08/15 10:23:01  paste
* Fixed EmuFCrateLoggable function definitions
*
* Revision 1.5  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __CHAMBER_H__
#define __CHAMBER_H__

#include <string>

namespace emu {
	namespace fed {

		/** @class Chamber A class for easy access to chamber names and locations. **/
		class Chamber
		{
		public:
			friend class ChamberParser; // Friends can access my private members.
		
			Chamber();
			~Chamber();
		
			/// The endcap where the chamber is located.  Is either "+" or "-"
			std::string endcap;

			/// The station where the chamber is located.  Is a number 1, 2, 3, or 4.
			int station;

			/// The "type" of the chamber, or on which annulus it is located.  Is a number 1, 2, or 3.
			int type;

			/// The chamber number.  The combination of endcap, station, type, and number uniquely defines each chamber.
			int number;
		
			/// @returns a human-readable string naming the chamber, like "+1/2/33".
			std::string name();

			/// @returns a human-readable string naming the peripheral crate where the DMB reading out the chamber is located, like "VMEp1_2"
			std::string peripheralCrate();

			/*
			/// @returns a human-readable string naming the fiber cassette where the DMB-to-DDU fiber is connected, like "1/2/c"
			std::string fiberCassette();
			*/
			
		private:

			/* PGK We might need to make these visible to the outside world at some point.
			For now, though I don't see why they can't be private. */
			/*
			int fiberCassetteCrate_;
			int fiberCassettePos_;
			std::string fiberCassetteSocket_;
			*/
			
			//int peripheralCrateId_;
			int peripheralCrateVMECrate_;
			//int peripheralCrateVMESlot_;
		
		};

	}
}

#endif