/*****************************************************************************\
* $Id: DDUDebugger.h,v 1.10 2008/09/24 18:38:38 paste Exp $
*
* $Log: DDUDebugger.h,v $
* Revision 1.10  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 1.9  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 1.8  2008/08/15 08:35:50  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#ifndef __DDUDEBUGGER_H__
#define __DDUDEBUGGER_H__

#include <string>
#include <vector>
#include <map>

#include "JTAG_constants.h"

/** A namespace that reads particular variables from the DDU and returns debugging
*	information.  Everything is returned as a std::vector of std::pairs, the 2nd element
*	being the class of the particular debug message
*	(@sa LocalEmuApplication::CSS) and the 1st is the actual debug message.
*
*	@author Phillip Killewald
*	@author Jason Gilmore
**/

namespace emu {

	namespace fed {

		class DDU;

		namespace DDUDebugger {
		
			/** Reads from method @sa DDU::readFPGAStat(DDUFPGA) **/
			std::map<std::string, std::string> DDUFPGAStat(unsigned long int stat);
		
			/** Reads from method @sa DDU::readOutputStat **/
			std::map<std::string, std::string> OutputStat(int stat);
		
			/** Reads from method @sa DDU::readEBReg(1) **/
			std::map<std::string, std::string> EBReg1(int stat);
		
			/** Reads from method @sa DDU::readEBReg(2) **/
			std::map<std::string, std::string> EBReg2(int stat);
		
			/** Reads from method @sa DDU::readEBReg(3) **/
			std::map<std::string, std::string> EBReg3(int stat);
		
			/** Reads from method @sa DDU::checkFIFO(2) **/
			std::map<std::string, std::string> FIFO2(int stat);
		
			/** Reads from method @sa DDU::readFFError() **/
			std::map<std::string, std::string> FFError(int stat);
		
			/** Reads from method @sa DDU::readInCHistory() **/
			std::map<std::string, std::string> InCHistory(int stat);
		
			/** Reads from method @sa DDU::readWarnMon() **/
			std::map<std::string, std::string> WarnMon(int stat);
		
			/** Reads from method @sa DDU::readDebugTrap(DDUFPGA) **/
			std::vector<std::string> DDUDebugTrap(std::vector<uint16_t> lcode, DDU *thisDDU);
		
			/** Reads from method @sa DDU::readKillFiber() **/
			std::map<std::string, std::string> KillFiber(long int stat);
		
			/** Reads from method @sa DDU::readFPGAStat(INFPGAX) **/
			std::map<std::string, std::string> InFPGAStat(enum DEVTYPE dt, unsigned long int stat);
		
			/** Reads from method @sa DDU::readFIFOStat(INFPGAX) **/
			std::map<std::string, std::string> FIFOStat(enum DEVTYPE dt, int stat);
		
			/** Reads from method @sa DDU::readFIFOFull(INFPGAX) **/
			std::map<std::string, std::string> FIFOFull(enum DEVTYPE dt, int stat);
		
			/** Reads from method @sa DDU::readCCodeStat(INFPGAX) **/
			std::map<std::string, std::string> CCodeStat(enum DEVTYPE dt, int stat);
		
			/** Reads from method @sa DDU::readFiberDiagnostics(INFPGAX,Y) **/
			std::map<std::string, std::string> FiberDiagnostics(enum DEVTYPE dt, int reg, unsigned long int stat);
		
			/** Reads from method @sa DDU::readWriteMemoryActive(INFPGAX) **/
			std::map<std::string, std::string> WriteMemoryActive(enum DEVTYPE dt, int iFiber, int stat);
		
			/** Reads from method @sa DDU::readDebugTrap(INFPGAX) **/
			std::vector<std::string> INFPGADebugTrap(std::vector<uint16_t> lcode, enum DEVTYPE dt);
		
			/** Reads from method @sa DDU::readParallelStat() **/
			std::map<std::string, std::string> ParallelStat(int stat);
		
			/** Reads bits 8-11 from method @sa DDU::readParallelStat()
			*	(not readFMMReg?)
			**/
			std::map<std::string, std::string> FMMReg(int stat);
		
			/** Reads from method @sa DDU::readGbEPrescale() **/
			std::map<std::string, std::string> GbEPrescale(int stat);
		
			/** Reads from method @sa DDU::readFakeL1Reg() **/
			std::map<std::string, std::string> FakeL1Reg(int stat);
		
			/** Reads from method @sa DDU::readFMMReg() **/
			std::map<std::string, std::string> F0EReg(int stat);
		
		};

	}
}


#endif