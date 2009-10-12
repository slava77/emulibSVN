/*****************************************************************************\
* $Id: DDUDebugger.h,v 1.6 2009/10/12 15:07:03 paste Exp $
\*****************************************************************************/
#ifndef __EMU_FED_DDUDEBUGGER_H__
#define __EMU_FED_DDUDEBUGGER_H__

#include <string>
#include <vector>
#include <map>
#include <utility>

#include "emu/fed/JTAG_constants.h"
#include "emu/fed/DDU.h"

/** A namespace that reads particular variables from the DDU and returns debugging
*	information.  Everything is returned as a std::vector of std::pairs, the 2nd element
*	being the class of the particular debug message
*	(LocalEmuApplication::CSS) and the 1st is the actual debug message.
*
*	@author Phillip Killewald
*	@author Jason Gilmore
**/

namespace emu {

	namespace fed {

		namespace DDUDebugger {
		
			/** Reads from method DDU::readFPGAStatus(DDUFPGA) **/
			std::map<std::string, std::string> DDUFPGAStat(const uint32_t &stat) __attribute__ ((deprecated));
			
			/** Reads from method DDU::readFPGAStatus() **/
			std::map<std::string, std::string> FPGAStatus(const enum DEVTYPE &dev, const uint32_t &stat);
		
			/** Reads from method DDU::readOutputStatus() **/
			std::map<std::string, std::string> OutputStat(const uint16_t &stat) __attribute__ ((deprecated));
			
			/** Reads from method DDU::readOutputStatus() **/
			std::map<std::string, std::string> OutputStatus(const uint16_t &stat);
		
			/** Reads from method DDU::readEBReg(1) **/
			std::map<std::string, std::string> EBReg1(const uint16_t &stat) __attribute__ ((deprecated));
		
			/** Reads from method DDU::readEBReg(2) **/
			std::map<std::string, std::string> EBReg2(const uint16_t &stat) __attribute__ ((deprecated));
		
			/** Reads from method DDU::readEBReg(3) **/
			std::map<std::string, std::string> EBReg3(const uint16_t &stat) __attribute__ ((deprecated));
			
			/** Reads from method DDU::readEBRegister() **/
			std::map<std::string, std::string> EBRegister(const uint8_t &reg, const uint16_t &stat);
		
			/** Reads from method DDU::checkFIFO(2) **/
			std::map<std::string, std::string> FIFO2(const uint16_t &stat) __attribute__ ((deprecated));
		
			/** Reads from method DDU::readWarnMon() **/
			std::map<std::string, std::string> WarnMon(const uint16_t &stat);
		
			/** Reads from method DDU::readDebugTrap(DDUFPGA) **/
			std::vector<std::string> DDUDebugTrap(const std::vector<uint16_t> &lcode, DDU *thisDDU);
		
			/** Reads from method DDU::readKillFiber() **/
			std::map<std::string, std::string> KillFiber(const uint32_t &stat);
		
			/** Reads from method DDU::readFPGAStat(INFPGAX) **/
			std::map<std::string, std::string> InFPGAStat(const enum DEVTYPE &dev, const uint32_t &stat) __attribute__ ((deprecated));
		
			/** Reads from method DDU::readFIFOStat(INFPGAX) **/
			std::map<std::string, std::string> FIFOStat(const enum DEVTYPE &dt, const uint16_t &stat) __attribute__ ((deprecated));
		
			/** Reads from method DDU::readFIFOFull(INFPGAX) **/
			std::map<std::string, std::string> FIFOFull(const enum DEVTYPE &dt, const uint16_t &stat) __attribute__ ((deprecated));
		
			/** Reads from method DDU::readCCodeStat(INFPGAX) **/
			std::map<std::string, std::string> CCodeStat(const enum DEVTYPE &dt, const uint16_t &stat) __attribute__ ((deprecated));
		
			/** Reads from method DDU::readFiberDiagnostics(INFPGAX,Y) **/
			std::map<std::string, std::string> FiberDiagnostics(const enum DEVTYPE &dt, const uint16_t &reg, const uint32_t &stat);
		
			/** Reads from method DDU::readWriteMemoryActive(INFPGAX) **/
			std::map<std::string, std::string> WriteMemoryActive(const enum DEVTYPE &dt, const uint16_t &iFiber, const uint16_t &stat) __attribute__ ((deprecated));
		
			/** Reads from method DDU::readDebugTrap(INFPGAX) **/
			std::vector<std::string> INFPGADebugTrap(const std::vector<uint16_t> &lcode, const enum DEVTYPE &dt) __attribute__ ((deprecated));
			
			/** Reads from method DDU::readDebugTrap(INFPGAX) **/
			std::vector<std::string> INFPGADebugTrap(const enum DEVTYPE &dt, const std::vector<uint16_t> &lcode);
		
			/** Reads from method DDU::readParallelStat() **/
			std::map<std::string, std::string> ParallelStat(const uint16_t &stat);
		
			/** Reads from DDU::readFMM **/
			std::map<std::string, std::string> FMM(const uint16_t &stat);
		
			/** Reads from method DDU::readGbEPrescale() **/
			std::map<std::string, std::string> GbEPrescale(const uint16_t &stat);
		
			/** Reads from method DDU::readFakeL1Reg() **/
			std::map<std::string, std::string> FakeL1Reg(const uint16_t &stat);
		
			/** Reads from method DDU::readFMMReg() **/
			std::map<std::string, std::string> F0EReg(const uint16_t &stat);

			/** Reads from DDU::readTemperature() **/
			std::pair<std::string, std::string> Temperature(const float &temp, const enum DDU::TEMPSCALE &scale = DDU::CELSIUS);

			/** Reads from DDU::readVoltage() **/
			std::pair<std::string, std::string> Voltage(const uint8_t &sensor, const float &voltage);
			
			/** Reads a 16-bit value and spits out the names of the fibers associated with the DDU for each bit high in the number.  **/
			std::map<std::string, std::string> DebugFiber(DDU *ddu, const uint16_t &value, const std::string &className = "none");
			
			/** Reads from method DDI::readInRDStat() **/
			std::map<std::string, std::string> InRDStat(const uint16_t &stat);
			
			/** Spits out a space-deliminated string of "fibers X-Y" based on the 4-bit input. **/
			std::string decodeInRD(const uint8_t &stat);
			
			/** Spits out a space-deliminated string of fiber names based on the input INFPGA, the DDU, and the 8-bit status. **/
			std::string decodeINFPGAFibers(const enum DEVTYPE &dev, const uint8_t &stat, DDU *duu);
			
			/** Reads from method DDI::readInCHistory() **/
			std::map<std::string, std::string> InCHistory(const uint16_t &stat);
			
			/** Reads from method DDU::readFFError() **/
			std::map<std::string, std::string> FFError(const uint16_t &stat);
			
			/** Reads from method DDU::readMaxTimeoutCount() **/
			std::map<std::string, std::string> MaxTimeoutCount(const uint16_t &stat);
			
			/** Reads from method DDU::readFIFOStatus(FIFO) **/
			std::map<std::string, std::string> FIFOStatus(const uint8_t &fifo, const uint16_t &stat);
			
			/** Reads from method DDU::readFIFOStatus(DEVTYPE) **/
			std::map<std::string, std::string> FIFOStatus(const enum DEVTYPE &dev, const uint8_t &fifo, const uint16_t &stat);
			
			/** Reads from method DDU::readFIFOStatus(DEVTYPE) **/
			std::map<std::string, std::string> FIFOStatus(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);

			/** Reads from method DDU::readSerialStatus() **/
			std::pair<std::string, std::string> SerialStatus(const uint8_t &stat);
			
			/** Reads from method DDU::readFiberStatus(DEVTYPE) **/
			std::map<std::string, std::string> FiberStatus(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readDMBSync(DEVTYPE) **/
			std::map<std::string, std::string> DMBSync(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readFIFOFull(DEVTYPE) **/
			std::map<std::string, std::string> FIFOFull(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readRxError(DEVTYPE) **/
			std::map<std::string, std::string> RxError(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readTxError(DEVTYPE) **/
			std::map<std::string, std::string> TxError(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readTimeout(DEVTYPE) **/
			std::map<std::string, std::string> Timeout(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readActiveWriteMemeory(DEVTYPE) **/
			std::map<std::string, std::string> ActiveWriteMemory(const enum DEVTYPE &dev, const uint8_t &reg, const uint16_t &stat, DDU *ddu);
		
			/** Reads from method DDU::readAvailableMemeory(DEVTYPE) **/
			std::map<std::string, std::string> AvailableMemory(const enum DEVTYPE &dev, const uint16_t &stat);
			
			/** Reads from method DDU::readLostError(DEVTYPE) **/
			std::map<std::string, std::string> LostError(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readCCodeStatus(DEVTYPE) **/
			std::map<std::string, std::string> CCodeStatus(const enum DEVTYPE &dev, const uint16_t &stat);
			
			/** Reads from method DDU::readDMBWarning(DEVTYPE) **/
			std::map<std::string, std::string> DMBWarning(const enum DEVTYPE &dev, const uint16_t &stat, DDU *ddu);
			
			/** Reads from method DDU::readFiberDiagnostics(DEVTYPE, reg) **/
			//std::map<std::string, std::string> FiberDiagnostics(const enum DEVTYPE &dev, const uint8_t &reg, const uint16_t &stat);
		}

	}
}


#endif
