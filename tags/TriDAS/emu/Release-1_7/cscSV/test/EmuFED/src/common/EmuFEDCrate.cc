// EmuFEDCrate.cc

#include "EmuFEDCrate.h"

#include "xdaq/NamespaceURI.h"
#include "xoap/Method.h"

XDAQ_INSTANTIATOR_IMPL(EmuFEDCrate);

EmuFEDCrate::EmuFEDCrate(xdaq::ApplicationStub *stub)
		throw (xdaq::exception::Exception) :
		EmuApplication(stub)
{
	xoap::bind(this, &EmuFEDCrate::onConfigure, "Configure", XDAQ_NS_URI);
	xoap::bind(this, &EmuFEDCrate::onHalt,      "Halt",      XDAQ_NS_URI);

	fsm_.addState('H', "Halted",     this, &EmuFEDCrate::stateChanged);
	fsm_.addState('C', "Configured", this, &EmuFEDCrate::stateChanged);

	fsm_.addStateTransition(
			'H', 'C', "Configure", this, &EmuFEDCrate::configureAction);
	fsm_.addStateTransition(
			'C', 'C', "Configure", this, &EmuFEDCrate::configureAction);
	fsm_.addStateTransition(
			'H', 'H', "Halt",      this, &EmuFEDCrate::haltAction);
	fsm_.addStateTransition(
			'C', 'H', "Halt",      this, &EmuFEDCrate::haltAction);

	fsm_.setInitialState('H');
	fsm_.reset();

	state_ = fsm_.getStateName(fsm_.getCurrentState());

	LOG4CPLUS_INFO(getApplicationLogger(), "EmuFEDCrate");
}

xoap::MessageReference EmuFEDCrate::onConfigure(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Configure");

	return createReply(message);
}

xoap::MessageReference EmuFEDCrate::onHalt(xoap::MessageReference message)
		throw (xoap::exception::Exception)
{
	fireEvent("Halt");

	return createReply(message);
}

void EmuFEDCrate::configureAction(toolbox::Event::Reference e)
        throw (toolbox::fsm::exception::Exception)
{   
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuFEDCrate::haltAction(toolbox::Event::Reference e)
		throw (toolbox::fsm::exception::Exception)
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), e->type());
}

void EmuFEDCrate::stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
        throw (toolbox::fsm::exception::Exception)
{
	EmuApplication::stateChanged(fsm);
}

// End of file
// vim: set ai sw=4 ts=4:
