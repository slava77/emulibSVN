/*****************************************************************************\
* $Id: VMEModule.h,v 1.3 2009/04/14 17:50:50 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_VMEMODULE_H__
#define __EMU_FED_VMEMODULE_H__

#include <pthread.h> // For mutexes

#include "emu/fed/JTAG_constants.h"
#include "emu/fed/Exception.h"

#include "emu/fed/JTAGElement.h"

namespace emu {

	namespace fed {

		class VMEController;

		struct JTAGElement;

		/** @class VMEModule A inherited class for DCC and DDU classes.
		*	A VMEModule should be ignorant of the controller and crate, just as how a hardware board
		*	doesn't really care into which crate it is inserted.  In other words, a VMEModule does not
		*	own a VMEController or a VMECrate.
		*/
		class VMEModule
		{
		public:

			/** Default constructor.
			*
			*	@param mySlot is the board's slot number in the crate (needed for proper VME communication.)
			**/
			VMEModule(unsigned int mySlot);

			virtual ~VMEModule() {};

			/** @returns the slot number. **/
			inline unsigned int slot() {return slot_;}

			/** Sets the appropriate BHandle for proper CAEN communication.
			*
			*	@param myHandle is the new BHandle to use.
			**/
			inline void setBHandle(int16_t myHandle) { BHandle_ = myHandle; }

			/** @returns the current BHandle. **/
			inline int16_t getBHandle() { return BHandle_; }

			/** Call the appropriate methods to configure the board. **/
			virtual void configure() = 0;

			/** Parses and loads a given .svf file into a given PROM.
			 *
			 * @param dev the PROM to which to load the parsed .svf file.
			 * @param fileName the name on the local disk of the .svf file.
			 * @param startString if set will cause the loader to ignore all instructions until the line after the one matching it.
			 * @param stopString if set will cause the loader to stop immidately if it is found in the current line being read.  The line will not be loaded.
			 *
			 * @returns zero if no errors occurred, a positive int for warnings, a negative int for errors.
			 **/
			int loadPROM(enum DEVTYPE dev, char *fileName, std::string startString = "", std::string stopString = "", bool debug = false)
			throw (emu::fed::exception::FileException, emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

			int loadPROM(enum DEVTYPE dev, const char *fileName, std::string startString = "", std::string stopString = "", bool debug = false)
			throw (emu::fed::exception::FileException, emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
			{
				try {
					return loadPROM(dev, (char *) fileName, startString, stopString, debug);
				} catch (...) {
					throw;
				}
			}

			int loadPROM(enum DEVTYPE dev, std::string fileName, std::string startString = "", std::string stopString = "", bool debug = false)
			throw (emu::fed::exception::FileException, emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
			{
				try {
					return loadPROM(dev, fileName.c_str(), startString, stopString, debug);
				} catch (...) {
					throw;
				}
			}

			/** Writes some data to a particular JTAG device.
			*
			*	@param dev the JTAG device to which the data will be sent
			*	@param nbits the number of bits to write
			*	@param myData the data to write, with the first element of the vector being the LSB
			*	@param noRead if true, will read back the data shifted out of the JTAG device and return it
			**/
			std::vector<uint16_t> jtagWrite(enum DEVTYPE dev, unsigned int nBits, std::vector<uint16_t> myData, bool noRead = false, bool debug = false)
			throw(emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

			/** Reads data from a particular JTAG device.
			*
			*	@param dev the JTAG device from which the data will be read
			*	@param nbits the number of bits to read
			**/
			std::vector<uint16_t> jtagRead(enum DEVTYPE dev, unsigned int nBits, bool debug = false)
			throw(emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

		protected:

			/** Sends a JTAG command cycle to a given JTAG device.
			*	@param dev the JTAG device to which the command will be sent
			*	@param myCommand the command code to send
			**/
			void commandCycle(enum DEVTYPE dev, uint16_t myCommand, bool debug = false)
			throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

			/** Reads 16 bits from a given VME address.
			*
			*	@param myAddress the address from which to read the data.
			*
			*	@note The slot number should NOT be encoded in myAddress.
			**/
			uint16_t readVME(uint32_t myAddress, bool debug = false)
			throw (emu::fed::exception::CAENException);

			/** Writes 16 bits to a given VME address.
			*
			*	@param myAddress the address to which to send the data.
			*	@param myData the data to send.
			*
			*	@note The slot number should NOT be encoded in myAddress.
			**/
			void writeVME(uint32_t myAddress, uint16_t myData, bool debug = false)
			throw (emu::fed::exception::CAENException);

			/** Reads any arbitrary number of bits from a given VME address.
			*
			*	@param myAddress the address from which to read the data.
			*	@param nBits the number of bits to read.
			*
			*	@note The slot number should NOT be encoded in myAddress.
			**/
			std::vector<uint16_t> readCycle(uint32_t myAddress, unsigned int nBits, bool debug = false)
			throw(emu::fed::exception::CAENException);

			/** Writes any arbitrary number of bits to a given VME address.
			*
			*	@param myAddress the address to which the data will be sent.
			*	@param nBits the number of bits to send.
			*
			*	@note The slot number should NOT be encoded in myAddress.
			**/
			void writeCycle(uint32_t myAddress, unsigned int nBits, std::vector<uint16_t> myData, bool debug = false)
			throw(emu::fed::exception::CAENException);

			/// A map of JTAG chains on this device.
			std::map<enum DEVTYPE, JTAGChain> JTAGMap;

		private:

			/// The slot number of the device (its location within it respective VME crate).
			unsigned int slot_;

			/// The CAEN BHandle used to communicate to the controller of the crate containing this device.
			int16_t BHandle_;

			/// The base address of the device as calculated from the slot number.
			uint32_t vmeAddress_;

			/// Mutex so that communication to and from the board is atomic.
			pthread_mutex_t mutex_;

		};

	}
}

#endif
