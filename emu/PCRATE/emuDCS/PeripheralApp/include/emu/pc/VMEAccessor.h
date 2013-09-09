#ifndef __emu_pc_VMEAccessor_h__
#define __emu_pc_VMEAccessor_h__

#include "emu/pc/Application.h"
#include "emu/utils/Progress.h"

#include "xdaq/WebApplication.h"
#include "xdata/String.h"
#include "xdata/Boolean.h"
#include "xdata/Integer.h"
#include "toolbox/task/WorkLoop.h"
#include "toolbox/task/Action.h"
#include "xoap/Method.h"

using namespace std;

namespace emu { namespace pc {

    class VMEAccessor 
      // : public virtual xdaq::WebApplication
      : public virtual emu::pc::Application
      , public virtual xdata::ActionListener
    {
    public:
      struct Status_t {
	std::string      crateIdsCSV;
	std::string      state;
	std::string      reasonForFailure;
	xcept::Exception exception;
	int              progressPercent;
	bool             isDone;
	Status_t(){
	  progressPercent = 0;
	  isDone = true;
	}
      };
      VMEAccessor( xdaq::ApplicationStub *s );
      
    protected:
      virtual bool vmeAccessInWorkLoop( toolbox::task::WorkLoop *wl ) = 0;
      virtual void actionPerformed( xdata::Event& received ) = 0;
      xoap::MessageReference onQueryStatus( xoap::MessageReference message );

      xdata::Integer ethPort_;
      xdata::Boolean isSlave_;
      xdata::Boolean isDistributed_;
      xdata::String crateIdsCSV_;
      xdata::String currentTaskName_;
      xdata::Integer progressPercent_;	///< [%]
      xdata::Boolean isDone_;
      bool isToStop_;
      emu::utils::Progress progress_;
      static const string workLoopType_; ///< the type of the work loop
      string workLoopName_;              ///< the name of the work loop
      toolbox::task::WorkLoop *workLoop_; ///< work loop for the test procedure to be executed in a separate thread
      xcept::Exception workLoopsCatch_; ///< exception caught in the work loop
      toolbox::task::ActionSignature *vmeAccessSignature_;
     };

}}

#endif
