#include "AFEB/teststand/LE32.h"
#include "AFEB/teststand/Jorway73A.h"
#include "AFEB/teststand/Crate.h"

#include <assert.h>

//using namespace AFEB::teststand;

AFEB::teststand::LE32::LE32( const string name ) :
  Module( name, "CAMAC" ){
}
  
void AFEB::teststand::LE32::turnOff(){
  writeAmplitude( Channel_t( Ch1 | Ch2 ), short(  0) );
  writeThreshold( Channel_t( Ch1 | Ch2 ), short(255) );
  disablePulses();
  disablePower();
}

void AFEB::teststand::LE32::writeAmplitude( const Channel_t pulseChannels, const unsigned int amplitude ){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  if ( crate_->getCrateController()->getName() == "Jorway73A" ){
    const Jorway73A* cc = static_cast<const Jorway73A*>( crate_->getCrateController() );
    if ( pulseChannels & Ch1 ) cc->write( amplitude, A0, F16, (Station_t)slot_ );
    if ( pulseChannels & Ch2 ) cc->write( amplitude, A1, F16, (Station_t)slot_ );
  }
}

void AFEB::teststand::LE32::writeThreshold( const Channel_t thresholdChannels, const unsigned int threshold ){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  if ( crate_->getCrateController()->getName() == "Jorway73A" ){
    const Jorway73A* cc = static_cast<const Jorway73A*>( crate_->getCrateController() );
    if ( thresholdChannels & Ch1 ) cc->write( threshold, A0, F17, (Station_t)slot_ );
    if ( thresholdChannels & Ch2 ) cc->write( threshold, A1, F17, (Station_t)slot_ );
  }
}

void AFEB::teststand::LE32::enablePulses( const Channel_t pulseChannels ){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  if ( crate_->getCrateController()->getName() == "Jorway73A" ){
    const Jorway73A* cc = static_cast<const Jorway73A*>( crate_->getCrateController() );
    cc->write( pulseChannels, A4, F16, (Station_t)slot_ );
  }  
}

void AFEB::teststand::LE32::disablePulses(){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  if ( crate_->getCrateController()->getName() == "Jorway73A" ){
    const Jorway73A* cc = static_cast<const Jorway73A*>( crate_->getCrateController() );
    cc->write( NoCh, A4, F16, (Station_t)slot_ );
  }    
}

void AFEB::teststand::LE32::enablePower( const Channel_t supplyChannels ){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  if ( crate_->getCrateController()->getName() == "Jorway73A" ){
    const Jorway73A* cc = static_cast<const Jorway73A*>( crate_->getCrateController() );
    cc->write( supplyChannels, A5, F16, (Station_t)slot_ );
  }
}


void AFEB::teststand::LE32::disablePower(){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  if ( crate_->getCrateController()->getName() == "Jorway73A" ){
    const Jorway73A* cc = static_cast<const Jorway73A*>( crate_->getCrateController() );
    cc->write( NoCh, A5, F16, (Station_t)slot_ );
  }
}

void AFEB::teststand::LE32::exec(){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  if ( crate_->getCrateController()->getName() == "Jorway73A" ){
    const Jorway73A* cc = static_cast<const Jorway73A*>( crate_->getCrateController() );
    cc->execute( A0, F25, (Station_t)slot_ );
  }
}

bool AFEB::teststand::LE32::readQ(){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  if ( crate_->getCrateController()->getName() == "Jorway73A" ){
    const Jorway73A* cc = static_cast<const Jorway73A*>( crate_->getCrateController() );
    return cc->q();
  }
  else{
    exit(1);
  }
  return false;
}

short AFEB::teststand::LE32::testLAM(){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  if ( crate_->getCrateController()->getName() == "Jorway73A" ){
    const Jorway73A* cc = static_cast<const Jorway73A*>( crate_->getCrateController() );
    if ( cc->lam() == slot_ ) return slot_;
    else                      return 0;
  }
  return 0;
}

