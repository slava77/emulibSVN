/*****************************************************************************\
* $Id: DDU.h,v 1.19 2010/08/13 02:53:00 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DDU_H__
#define __EMU_FED_DDU_H__

#include <vector>
#include <string>

#include "emu/fed/JTAG_constants.h"
#include "emu/fed/Exception.h"

#include "emu/fed/VMEModule.h"

namespace emu {

	namespace fed {

		class Fiber;

		/** @class DDU A class representing the Detector-Dependent Unit boards in the EMU FED Crates. **/
		class DDU: public VMEModule
		{
			//friend class DDUParser;
			//friend class AutoConfigurator;
			//friend class DDUDBAgent;

		public:

			/** @param slot the slot of the board for VME addressing purposes. **/
			DDU(const unsigned int &mySlot, const bool &fake = false);

			/** Default destructor. **/
			virtual ~DDU();

			/** @returns the GbEPrescale setting from the configuration. **/
			inline uint16_t getGbEPrescale() { return gbe_prescale_; }

			/** Sets the GbEPrescale **/
			inline void setGbEPrescale(const uint16_t &gbe_prescale) { gbe_prescale_ = gbe_prescale; }

			/** @returns the KillFiber bit-mask (LS 15 bits) and the options bits (MS 5 bits) from the configuration. **/
			inline uint32_t getKillFiber() { return killfiber_; }

			/** Sets the KillFiber **/
			inline void setKillFiber(const uint32_t &killfiber) { killfiber_ = killfiber; reloadFiberKillBits(killfiber_); }

			/** @returns the RUI from the configuration **/
			inline uint16_t getRUI() { return rui_; }

			/** Sets the RUI **/
			inline void setRUI(const uint16_t &rui) { rui_ = rui; }

			/** @returns the FMM ID from the configuration **/
			inline uint16_t getFMMID() { return fmm_id_; }

			/** Sets the FMM ID **/
			inline void setFMMID(const uint16_t &fmm_id) { fmm_id_ = fmm_id; }

			/** Configures the DDU. **/
			void configure()
			throw (emu::fed::exception::DDUException);

			/** Part of the suite of fiber methods.
			*	@returns a vector of fibers in fiber-order.
			**/
			inline std::vector<Fiber *> getFibers() { return fiberVector_; }

			/** Part of the suite of fiber methods.
			*	@param fiberNumber runs from 0-14.
			*	@returns the fiber at the given fiber input number.
			**/
			Fiber *getFiber(const unsigned int &fiberNumber)
			throw (emu::fed::exception::OutOfBoundsException);

			/** Adds a fiber object to the DDU.
			*	@param fiber is the fiber being added.
			**/
			void addFiber(Fiber *fiber)
			throw (emu::fed::exception::OutOfBoundsException);

			/** Deletes a fiber object from the DDU.
			 * @param fiber is a pointer to the fiber to delete.
			 **/
			inline void deleteFiber(Fiber *fiber)
			{
				for (std::vector<Fiber *>::iterator iFiber = fiberVector_.begin(); iFiber != fiberVector_.end(); ++iFiber) {
					if (fiber == *iFiber) {
						fiberVector_.erase(iFiber);
						break;
					}
				}
				reloadKillFiber();
			}

			/** Sets the vector of fiber objects in the DDU to some vector.
			*	@param fiberVector is a vector of chambers to copy to the internal vector.
			**/
			void setFibers(const std::vector<Fiber *> &fiberVector)
			throw (emu::fed::exception::OutOfBoundsException);

			/** Reloads the killfiber setting from the stored fiber objects **/
			void reloadKillFiber();

			// PGK New interface
			// Read VME Parallel registers
			/** @returns the FMM register. **/
			uint16_t readFMM()
			throw (emu::fed::exception::DDUException);

			/** @returns the combined error status of each fiber. **/
			uint16_t readCSCStatus()
			throw (emu::fed::exception::DDUException);

			/** @returns the FMM busy status of each fiber. **/
			uint16_t readFMMBusy()
			throw (emu::fed::exception::DDUException);

			/** @returns the FMM warning status of each fiber. **/
			uint16_t readFMMFullWarning()
			throw (emu::fed::exception::DDUException);

			/** @returns the FMM sync lost status of each fiber. **/
			uint16_t readFMMLostSync()
			throw (emu::fed::exception::DDUException);

			/** @returns the FMM error status of each fiber. **/
			uint16_t readFMMError()
			throw (emu::fed::exception::DDUException);

			/** @returns the status of the hardware switches on-board. **/
			uint16_t readSwitches()
			throw (emu::fed::exception::DDUException);

			/** @returns the VME parallel status register. **/
			uint16_t readParallelStatus()
			throw (emu::fed::exception::DDUException);

			/** @returns the value of a given parallel input register. **/
			uint16_t readInputRegister(const uint8_t &iReg)
			throw (emu::fed::exception::DDUException);

			/** @returns the value of the fake L1 register. **/
			uint16_t readFakeL1()
			throw (emu::fed::exception::DDUException);

			/** @returns the value of the GbE prescale register. **/
			uint16_t readGbEPrescale()
			throw (emu::fed::exception::DDUException);

			/** @returns the value of one of the (unused) test registers. **/
			uint16_t readTestRegister(const uint8_t &iReg)
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers have experienced a busy status since the last resync. **/
			uint16_t readBusyHistory()
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers have experienced a warning status since the last resync. **/
			uint16_t readWarningHistory()
			throw (emu::fed::exception::DDUException);

			// Write VME Parallel registers

			/** Writes a value to the FMM register (useful for TTS tests). **/
			void writeFMM(const uint16_t &value)
			throw (emu::fed::exception::DDUException);

			/** Writes a value to the fake L1 register. **/
			void writeFakeL1(const uint16_t &value)
			throw (emu::fed::exception::DDUException);

			/** Writes to the GbE prescale register. **/
			void writeGbEPrescale(const uint8_t &value)
			throw (emu::fed::exception::DDUException);

			/** Writes to one of the parallel input registers (for flash writing). **/
			void writeInputRegister(const uint16_t &value)
			throw (emu::fed::exception::DDUException);

			// Read VME Serial/Flash registers

			/** @returns the status of the serial path. **/
			uint8_t readSerialStatus()
			throw (emu::fed::exception::DDUException);

			/** @returns reads back the kill fiber setting stored in flash. **/
			uint16_t readFlashKillFiber()
			throw (emu::fed::exception::DDUException);

			/** @returns the board ID stored in flash. **/
			uint16_t readFlashBoardID()
			throw (emu::fed::exception::DDUException);

			/** @returns the board RUI stored in flash. **/
			uint16_t readFlashRUI()
			throw (emu::fed::exception::DDUException);

			/** @returns the GbE FIFO thresholds stored in flash. **/
			std::vector<uint16_t> readFlashGbEFIFOThresholds()
			throw (emu::fed::exception::DDUException);

			/*const uint64_t readFlashGbEFIFOThresholds()
			throw (emu::fed::exception::DDUException);*/

			const uint32_t readFlashInFIFOThresholds()
			throw (emu::fed::exception::DDUException);

			// Write VME Serial/Flash registers

			/** Writes a value to the kill fiber flash register. **/
			void writeFlashKillFiber(const uint16_t &value)
			throw (emu::fed::exception::DDUException);

			/** Writes a value to the board ID flash register. **/
			void writeFlashBoardID(const uint16_t &value)
			throw (emu::fed::exception::DDUException);

			/** Writes a value to the board RUI flash register. **/
			void writeFlashRUI(const uint16_t &value)
			throw (emu::fed::exception::DDUException);

			/** Writes a value to the GbE FIFO threshold flash register. **/
			void writeFlashGbEFIFOThresholds(const std::vector<uint16_t> &values)
			throw (emu::fed::exception::DDUException);

			void writeFlashGbEFIFOThresholds(const uint64_t values)
			throw (emu::fed::exception::DDUException);

			void writeFlashInFIFOThresholds(const std::vector<uint16_t> &values)
			throw (emu::fed::exception::DDUException);

			void writeFlashInFIFOThresholds(const uint32_t values)
			throw (emu::fed::exception::DDUException);

			// Read SADC registers

			enum TEMPSCALE {
				FAHRENHEIT,
				CELSIUS,
				KELVIN,
				RANKINE,
				REAUMUR,
				ROMER,
				NEWTON,
				DELISLE
			};
			/** @returns the calculated temperature of a given SADC sensor and a temperature scale **/
			float readTemperature(const uint8_t &sensor, const enum TEMPSCALE &scale = CELSIUS)
			throw (emu::fed::exception::DDUException);

			/** @returns the raw temperature from a given SADC sensor **/
			uint16_t readRawTemperature(const uint8_t &sensor)
			throw (emu::fed::exception::DDUException);

			/** @returns the calculated voltage of a given SADC sensor. **/
			float readVoltage(const uint8_t &sensor)
			throw (emu::fed::exception::DDUException);

			/** @returns the raw voltage from a given SADC sensor **/
			uint16_t readRawVoltage(const uint8_t &sensor)
			throw (emu::fed::exception::DDUException);

			// Read DDUFPGA JTAG registers

			/** @returns the status of the DDUFPGA output to the INFPGAs. **/
			uint16_t readOutputStatus()
			throw (emu::fed::exception::DDUException);

			/** @returns the status of a given DDUFPGA input FIFO. **/
			uint16_t readFIFOStatus(const uint8_t &fifo)
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers are experiencing a FIFO-full error. **/
			uint16_t readFFError()
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers are experiencing a CRC error. **/
			uint16_t readCRCError()
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers are experiencing a transmit error. **/
			uint16_t readXmitError()
			throw (emu::fed::exception::DDUException);

			/** @returns the kill fiber mask that is loaded into the DDUFPGA. **/
			uint32_t readKillFiber()
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers report corruption in the DMB data. **/
			uint16_t readDMBError()
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers report corruption in the TMB data. **/
			uint16_t readTMBError()
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers report corruption in the ALCT data. **/
			uint16_t readALCTError()
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers are experiencing a lost-in-event error. **/
			uint16_t readLIEError()
			throw (emu::fed::exception::DDUException);

			/** @returns the status of the paths out to the INFPGAs. **/
			uint16_t readInRDStatus()
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers have ever experienced input corruption (?). **/
			uint16_t readInCHistory()
			throw (emu::fed::exception::DDUException);

			/** @returns the infamous "Error B" register. **/
			uint16_t readEBRegister(const uint8_t reg)
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers report communication with the DMB. **/
			uint16_t readDMBLive()
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers reported communication with the DMB when the first L1A came though. **/
			uint16_t readDMBLiveAtFirstEvent()
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers report a warning status. **/
			uint16_t readWarningMonitor()
			throw (emu::fed::exception::DDUException);

			/** @returns the maximum timeout between the L1A and the data sent from the DMBs. **/
			uint16_t readMaxTimeoutCount()
			throw (emu::fed::exception::DDUException);

			/** @returns the bunch-crossing orbit setting. **/
			uint16_t readBXOrbit()
			throw (emu::fed::exception::DDUException);

			/** Magically toggles the L1 calibration setting. **/
			void toggleL1Calibration()
			throw (emu::fed::exception::DDUException);

			/** @returns the board RUI stored in the DDUFPGA. **/
			uint16_t readRUI()
			throw (emu::fed::exception::DDUException);

			/** Magically sends a fake L1A through the DDU. **/
			void sendFakeL1A()
			throw (emu::fed::exception::DDUException);

			/** @returns the occupancies of the fiber it is point to, then increments its internal pointer to point at the next fiber. **/
			std::vector<uint32_t> readOccupancyMonitor()
			throw (emu::fed::exception::DDUException);

			/** @returns some errors that escaped the DDUFPGA previously. **/
			uint16_t readAdvancedFiberErrors()
			throw (emu::fed::exception::DDUException);

			// Write DDUFPGA JTAG registers

			/** Writes to the kill fiber register on the DDUFPGA. **/
			void writeKillFiber(const uint32_t &value)
			throw (emu::fed::exception::DDUException);

			/** Writes to the bunch-crossing orbit register on the DDUFPGA. **/
			void writeBXOrbit(const uint16_t &value)
			throw (emu::fed::exception::DDUException);

			// Read INFPGA JTAG registers

			/** @returns the LS 32 bits of the L1 scaler. **/
			uint32_t readL1Scaler1(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns the combined status of the fibers connected to the INFPGA. **/
			uint16_t readFiberStatus(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns the DMB sync status of the fibers connected to the INFPGA. **/
			uint16_t readDMBSync(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns the combined input FIFO status of a given INFPGA. **/
			uint16_t readFIFOStatus(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers on a given INFPGA have a full input FIFO. **/
			uint16_t readFIFOFull(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers on a given INFPGA have a receiving error. **/
			uint16_t readRxError(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers on a given INFPGA have a timeout error. **/
			uint16_t readTimeout(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers on a given INFPGA have a transmit error. **/
			uint16_t readTxError(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns which memory modules are being written by a given fiber on a given INFPGA. **/
			uint16_t readActiveWriteMemory(const enum DEVTYPE &dev, const uint8_t &iFiber)
			throw (emu::fed::exception::DDUException);

			/** @returns how many memory modules are available for a given INFPGA. **/
			uint16_t readAvailableMemory(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns the minimum number of memory modules an INFPGA has had to use since the last resync. **/
			uint16_t readMinMemory(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers on a given INFPGA have a lost-in-event error (?). **/
			uint16_t readLostError(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns the infamous "C-Code" register. **/
			uint16_t  readCCodeStatus(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns which fibers on a given INFPGA are experiencing a DMB warning condition. **/
			uint16_t readDMBWarning(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns the fiber diagnostic register on a given INFPGA. **/
			uint32_t readFiberDiagnostics(const enum DEVTYPE &dev, const uint8_t &iDiagnostic)
			throw (emu::fed::exception::DDUException);

			// User and ID code reading

			/** @returns the user code of the given device. **/
			uint32_t readUserCode(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns the ID code of the given device. **/
			uint32_t readIDCode(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			// Reset FPGAs

			/** Sends a reset command to the given FPGA. **/
			void resetFPGA(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			// Read Universal FPGA JTAG registers

			/** @returns the status register of a given FPGA. **/
			uint32_t readFPGAStatus(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns the L1 scaler register of a given FPGA. **/
			uint32_t readL1Scaler(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			/** @returns the debug trap from a given FPGA. **/
			std::vector<uint16_t> readDebugTrap(const enum DEVTYPE &dev)
			throw (emu::fed::exception::DDUException);

			// Misc. routines

			/** Loads the proper value in the FMM register to disable sending FMM signals. **/
			void disableFMM()
			throw (emu::fed::exception::DDUException);

			/** Loads the proper value in the FMM register to enable sending FMM signals for a short time. **/
			void enableFMM()
			throw (emu::fed::exception::DDUException);

			/** @returns a bit-mask of which fibers are present and alive. **/
			uint16_t readLiveFibers()
			throw (emu::fed::exception::DDUException);

			/** @returns a bit-mask of which fibers have errors (an OR of CSCStatus and AdvancedFiberStatus). **/
			uint16_t readFiberErrors()
			throw (emu::fed::exception::DDUException);

			/** @returns the real 4-bit FMM status.  This will report an error even if FMM reporting is disabled. **/
			uint8_t readRealFMM()
			throw (emu::fed::exception::DDUException);

		protected:

			/** Reloads the killed members of the owned fibers based on the supplied killfiber setting
			*
			*	@param killfiber is the killfiber setting to parse into killed bits for the fibers
			**/
			void reloadFiberKillBits(const uint16_t &killfiber);

			// PGK New interface
			/** Reads an arbitrary number of bits from a given register on a given device.
			*
			*	@param dev is the device from which to read.
			*	@param myReg is the register on the device from which to read.
			*	@param @nBits is the number of bits to read.
			**/
			std::vector<uint16_t> readRegister(const enum DEVTYPE &dev, const uint16_t &myReg, const unsigned int &nBits)
			throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

			/** Writes an arbitrary number of bits to a given register on a given device.
			*
			*	@param dev is the device to which to write.
			*	@param myReg is the register on the device to which to write.
			*	@param @nBits is the number of bits to write.
			*	@param @myData is the data to write.
			**/
			std::vector<uint16_t> writeRegister(const enum DEVTYPE &dev, const uint16_t &myReg, const unsigned int &nBits, const std::vector<uint16_t> &myData)
			throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException);

			/// The fibers that are plugged into this DDU, in fiber-order.
			std::vector<Fiber *> fiberVector_;

			/// The GbE prescale code as read from the configuration.
			uint16_t gbe_prescale_;

			/// The kill fiber mask as read from the configuration.
			uint32_t killfiber_;

			/// The RUI from the configuration
			uint16_t rui_;

			/// The FMM ID from the configuration
			uint16_t fmm_id_;

		};

	}
}

#endif

