/*************************************************************************

The philosophy behind the objects FEDCrate, VMEModule, VMEController, DDU,
DCC, and IRQThread are as follows:

The FEDCrate contains DDU (pointer)s, DCC (pointer)s, and a VMEController
(pointer).  Because it contains everything, the FEDCrate is the highest
level object in the scheme.  When you pass a properly formatted XML file
to the FEDCrateParser, it will hand you back a std::vector of properly
initialized and filled FEDCrates.  Note that the FEDCrateParser is actually
a brain-dead object with only minimal error checking--the FEDCrate, DDU, DCC,
and VMEController objects it makes do all the magic of proper
initialization in their constructors.

Once you have a std::vector of FEDCrates from the FEDCrateParser, you can pick out
the various objects from the FEDCrate using the FEDCrate::ddus(), dccs(), and
vmeController() methods.  The VMEController object is the only thing that
is even remotely aware of which FEDCrate in which it resides (given by the
_private_ VMEController::crateNumber member).  Because of this, it is the
user's responsiblility to keep track of which FEDCrate he is addressing.

The VMEModule class is a dummy class with members that both DDU and DCC
objects inherit.

The VMEController object owns an instance of the IRQThread class.  All of
the interrupt handling is performed through this object.  You start and
stop the threads by addressing the VMEController with the methods
VMEController::start_thread(runnumber), stop_thread(), and kill_thread().
Once the thread has been started, you may access the thread directly by
picking it out of the VMEController with VMEController::thread().

Everything mentioned here has an example in the code.

 *************************************************************************/


#ifndef __FEDCRATE_H__
#define __FEDCRATE_H__

#include <vector>
#include <iostream>

#include "EmuFEDLoggable.h"

class VMEModule;
class VMEController;
class DDU;
class DCC;

class FEDCrate: public EmuFEDLoggable {
public:
	FEDCrate(int number, VMEController* theController = NULL);
	~FEDCrate();

	int number() const {return theNumber;}

	void addModule(VMEModule* module);
	void setController(VMEController* controller);

	VMEController * vmeController() const {return theController;}

	/// uses RTTI to find types
	std::vector<DDU *> ddus() const;
	std::vector<DCC *> dccs() const;
	inline std::vector<VMEModule *> modules() { return theModules; }

	// Return the rui of the DDU in the given slot.  The crate number is
	//  needed to figure this out.
	int getRUI(int slot);

	void enable();
	void disable();
	void configure();
	void init();
// PGK, silly to have this belong to the crate and not the thread
//	int irqtest(int crate,int ival);

private:

	template<class T> T * findBoard() const
	{
		for(unsigned i = 0; i < theModules.size(); ++i) {
			T * result = dynamic_cast<T *>(theModules[i]);
			if(result != 0) return result;
		}
		return 0;
	}

	int theNumber;
	/// indexed by slot
	std::vector<VMEModule *> theModules;
	VMEController * theController;
};

#endif

