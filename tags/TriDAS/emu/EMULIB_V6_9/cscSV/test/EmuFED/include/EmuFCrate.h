#ifndef __EMU_FCRATE_H__
#define __EMU_FCRATE_H__

#include "EmuApplication.h"

#include "toolbox/fsm/FiniteStateMachine.h"
#include "xdata/UnsignedInteger.h"

class EmuFCrate : public EmuApplication
{
public:
	XDAQ_INSTANTIATOR();

	EmuFCrate(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);

	// SOAP interface
	xoap::MessageReference onConfigure(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onEnable(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onDisable(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onHalt(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onSetTTSBits(xoap::MessageReference message)
			throw (xoap::exception::Exception);

	// State transitions
	void configureAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);
	void enableAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);
	void disableAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);
	void haltAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);
	void setTTSBitsAction(toolbox::Event::Reference e)
			throw (toolbox::fsm::exception::Exception);

private:
    void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
			throw (toolbox::fsm::exception::Exception);

private:
	xdata::UnsignedInteger tts_id_;
	xdata::UnsignedInteger tts_bits_;
};

#endif  // ifndef __EMU_FCRATE_H__
// vim: set ai sw=4 ts=4: