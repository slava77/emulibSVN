/*****************************************************************************\
* $Id: VMEController.h,v 1.5 2009/07/06 16:05:40 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_VMECONTROLLER_H__
#define __EMU_FED_VMECONTROLLER_H__

#include "emu/fed/JTAG_constants.h"
#include "emu/fed/Exception.h"

namespace emu {

	namespace fed {

		class VMEModule;

		/** @class VMEController A class for establishing communication with a CAEN VME controller **/
		class VMEController
		{
		public:

			/** Default constructor.
			*
			*	@param Device the PCI device number.
			*	@param Link the position of the target controller in a daisy chain.
			**/
			VMEController(const int &Device, const int &Link)
			throw (emu::fed::exception::CAENException);

			/** Default destructor. **/
			~VMEController();

			/** @returns the device number. **/
			inline int getDevice() {return Device_;}

			/** @returns the link number. **/
			inline int getLink() {return Link_;}

			/** @returns the CAEN BHandle, **/
			inline int32_t getBHandle() { return BHandle_; }

			/** Set the IRQ enabled and wait until an interrupt comes through.
			*
			*	@param mSecs the number of milliseconds to wait for an
			*	interrupt before exiting with all clear status.
			*
			*	@returns false if there was an interrupt set, true otherwise.
			**/
			bool waitIRQ(const unsigned int &mSecs = 5000)
			throw (emu::fed::exception::CAENException);

			/** Read the VME IRQ channel
			*
			*	@returns the 16-bits read from the IRQ channel.
			**/
			uint16_t readIRQ()
			throw (emu::fed::exception::CAENException);

			/**	I am doing something smart here.  Instead of making the BHandles extern,
			*	I am going to have the original constructor set its own BHanlde to a
			*	crazy value in case of an error.  Thus, if the device is already open, I
			*	will be able to check the BHandle to figure that out, then simply replace
			*	the BHandle with the correct value once I know it.
			**/
			inline void setBHandle(const int32_t &BHandle) { BHandle_ = BHandle; }

		private:

			/// The device number.  One driver instance controls all the CAEN PCI boards on a machine, so this number chooses to which CAEN board communication will be addressed.
			int Device_;

			/// The daisy-chain link number.  The controllers can be connected in a daisy-chain fashion, with the transmit line of one plugged into the receive line of the next.  This chooses which device in the daisy-chain to address.
			int Link_;

			/// The BHandle.  This is a unique number generated by the driver and used to pick which Device/Link pair a given command addresses.
			int32_t BHandle_;


		};

	}
}

#endif

