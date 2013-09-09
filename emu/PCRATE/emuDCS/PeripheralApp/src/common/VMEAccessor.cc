#include "emu/pc/VMEAccessor.h"

#include "emu/soap/ToolBox.h"
#include "emu/soap/Messenger.h"
#include "emu/utils/String.h"

#include "toolbox/task/WorkLoopFactory.h"
#include "xoap/MessageFactory.h"
#include "xdaq/NamespaceURI.h"

const string emu::pc::VMEAccessor::workLoopType_( "waiting" );

emu::pc::VMEAccessor::VMEAccessor( xdaq::ApplicationStub *s )
  : xdaq::WebApplication( s )
  , emu::pc::Application( s )
  , ethPort_        ( -1    )
  , isSlave_        ( false )
  , isDistributed_  ( false )
  , crateIdsCSV_    ( ""    )
  , progressPercent_( 0     )
  , isDone_         ( false )
  , isToStop_       ( false )
{
  getApplicationInfoSpace()->fireItemAvailable( "ethPort"        , &ethPort_         );
  getApplicationInfoSpace()->fireItemAvailable( "crateIdsCSV"    , &crateIdsCSV_     );
  getApplicationInfoSpace()->fireItemAvailable( "isSlave"        , &isSlave_         );
  getApplicationInfoSpace()->fireItemAvailable( "isDone"         , &isDone_          );
  getApplicationInfoSpace()->fireItemAvailable( "currentTaskName", &currentTaskName_ );
  getApplicationInfoSpace()->fireItemAvailable( "progressPercent", &progressPercent_ );
  getApplicationInfoSpace()->addItemChangedListener( "isSlave", this );
  getApplicationInfoSpace()->addItemRetrieveListener( "progressPercent", this );

  xoap::bind(this, &emu::pc::VMEAccessor::onQueryStatus, "QueryStatus", XDAQ_NS_URI );

  vmeAccessSignature_ = toolbox::task::bind( this, &emu::pc::VMEAccessor::vmeAccessInWorkLoop, "vmeAccessInWorkLoop" );
  workLoopName_ = getApplicationDescriptor()->getClassName() + "." 
    + emu::utils::stringFrom<int>( getApplicationDescriptor()->getInstance() );
}

xoap::MessageReference emu::pc::VMEAccessor::onQueryStatus( xoap::MessageReference message ){
  xoap::MessageReference reply;
  try{
    bsem_.take();
    if ( workLoopsCatch_.message().size() > 0 ){
      XCEPT_RETHROW( xcept::Exception, "Exception caught in work loop.", workLoopsCatch_ );
    }
    if ( reasonForFailure_.toString().size() > 0 ){
      XCEPT_RAISE( xcept::Exception, reasonForFailure_.toString() );
    }
    xdata::String  state;
    xdata::String  reasonForFailure;
    xdata::Integer progressPercent;
    xdata::Boolean isDone;
    reply = emu::soap::createMessage( "QueryStatusReply", 
				      emu::soap::Parameters()
				      .add( "state"           , &state_            )
				      .add( "reasonForFailure", &reasonForFailure_ )
				      .add( "progressPercent" , &progressPercent_  )
				      .add( "isDone"          , &isDone_           ) );
    bsem_.give();
  }
  catch( xcept::Exception& e ){
    bsem_.give();
    XCEPT_RETHROW( xoap::exception::Exception, "Failed to query status.", e );
  }
  return reply;
}
