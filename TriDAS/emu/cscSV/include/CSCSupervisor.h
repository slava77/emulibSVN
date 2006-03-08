#ifndef __CSC_SUPERVISOR_H__
#define __CSC_SUPERVISOR_H__

#include "EmuApplication.h"

#include <string>

#include "toolbox/fsm/FiniteStateMachine.h"
#include "xgi/Method.h"

class CSCSupervisor : public EmuApplication
{
public:
	XDAQ_INSTANTIATOR();

	CSCSupervisor(xdaq::ApplicationStub *stub) throw (xdaq::exception::Exception);

	// SOAP interface
	xoap::MessageReference onConfigure(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onHalt(xoap::MessageReference message)
			throw (xoap::exception::Exception);
	xoap::MessageReference onEnable(xoap::MessageReference message)
			throw (xoap::exception::Exception);

	// HyperDAQ interface
	void webDefault(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);
	void webConfigure(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);
	void webEnable(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);
	void webHalt(xgi::Input *in, xgi::Output *out)
			throw (xgi::exception::Exception);

	// State transitions
	void configureAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
	void haltAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);
	void enableAction(toolbox::Event::Reference e) 
			throw (toolbox::fsm::exception::Exception);

private:
	void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
			throw (toolbox::fsm::exception::Exception);

	string getRuntype(xoap::MessageReference message);
	string getRuntype(xgi::Input *in);

	string runtype;
};

#endif  // ifndef __CSC_SUPERVISOR_H__
// vim: set ai sw=4 ts=4:
