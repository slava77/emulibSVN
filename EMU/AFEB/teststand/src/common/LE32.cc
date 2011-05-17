#include "AFEB/teststand/LE32.h"
#include "AFEB/teststand/Jorway73A.h"
#include "AFEB/teststand/Crate.h"

#include <assert.h>
#include <stdlib.h> // for exit()

AFEB::teststand::LE32::LE32( const string type ) :
  Module( "LE32", type ){
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
  if ( pulseChannels & Ch1 ) crate_->getCrateController()->write( amplitude, A0, F16, (Station_t)slot_ );
  if ( pulseChannels & Ch2 ) crate_->getCrateController()->write( amplitude, A1, F16, (Station_t)slot_ );
}

void AFEB::teststand::LE32::writeThreshold( const Channel_t thresholdChannels, const unsigned int threshold ){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  if ( thresholdChannels & Ch1 ) crate_->getCrateController()->write( threshold, A0, F17, (Station_t)slot_ );
  if ( thresholdChannels & Ch2 ) crate_->getCrateController()->write( threshold, A1, F17, (Station_t)slot_ );
}

void AFEB::teststand::LE32::enablePulses( const Channel_t pulseChannels ){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  crate_->getCrateController()->write( pulseChannels, A4, F16, (Station_t)slot_ );
}

void AFEB::teststand::LE32::disablePulses(){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  crate_->getCrateController()->write( NoCh, A4, F16, (Station_t)slot_ );
}

void AFEB::teststand::LE32::enablePower( const Channel_t supplyChannels ){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  crate_->getCrateController()->write( supplyChannels, A5, F16, (Station_t)slot_ );
}


void AFEB::teststand::LE32::disablePower(){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  crate_->getCrateController()->write( NoCh, A5, F16, (Station_t)slot_ );
}

void AFEB::teststand::LE32::exec(){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  crate_->getCrateController()->execute( A0, F25, (Station_t)slot_ );
}

bool AFEB::teststand::LE32::readQ(){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  return crate_->getCrateController()->q();
  return false;
}

short AFEB::teststand::LE32::testLAM(){
  assert( crate_ != NULL );
  assert( crate_->getCrateController() != NULL );
  if ( crate_->getCrateController()->lam() == slot_ ) return slot_;
  return 0;
}

