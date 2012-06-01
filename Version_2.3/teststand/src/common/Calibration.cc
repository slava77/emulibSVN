#include "AFEB/teststand/Calibration.h"
#include "AFEB/teststand/crate/Crate.h"
#include "AFEB/teststand/utils/String.h"
#include "xcept/Exception.h"

using namespace std;
using namespace AFEB::teststand;

ostream& AFEB::teststand::operator<<( ostream& os, const Calibration& c ){
  os << "Calibration of DACs at threshold level = " << c.getThresholdLevel() 
     << " and test pulse amplitude = "              << c.getPulseAmplitude() 
     << " of modules:" << endl;
  for ( vector<Module*>::const_iterator m=c.modules_.begin(); m!=c.modules_.end(); ++m ){
    os << "    " << **m;
  }
  return os;
}

AFEB::teststand::Calibration::Calibration() :
  bsem_( toolbox::BSem::EMPTY ), // locked
  thresholdLevel_( -1 ),
  pulseAmplitude_( -1 ),
  initialized_( false ),
  isToKeepRunning_( true )
{
  bsem_.give();
}

void AFEB::teststand::Calibration::addModule( Module* module ){
  bsem_.take();
  modules_.push_back( module );
  bsem_.give();
}

bool AFEB::teststand::Calibration::execute(){
  bsem_.take();

  // Check if there are modules with DACs to calibrate
  if ( modules_.size() == 0 ){
    bsem_.give();
    return false;
  }

  // Check if this a just a dummy run, in which case there's no crate controller defined
  if ( modules_.front()->getCrate()->getCrateController() == NULL ){
    cout << *this;
    bsem_.give();
    return isToKeepRunning_;
  }

  // Initialize crate if not yet initialized
  if ( ! initialized_ ){
    // cout << *modules_.front() << endl << flush;
    // cout << *modules_.front()->getCrate() << endl << flush;
    // cout << *modules_.front()->getCrate()->getCrateController() << endl << flush;
    modules_.front()->getCrate()->getCrateController()->initialize();
    initialized_ = true;
  }

  // Set threshold levels and send pulses of the requested amplitude
  const int nPulses = 1000; // Number of pulses to be sent uninterrupted.
  for ( vector<Module*>::const_iterator m=modules_.begin(); m!=modules_.end(); ++m ){
    if ( (*m)->getName().compare( "LE32" ) == 0 ){
      LE32* le32 = static_cast<LE32*>( *m );
      if ( thresholdLevel_ >= 0 ){
	le32->writeThreshold( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ), thresholdLevel_ );
      }
      if ( pulseAmplitude_ >= 0 ){
	le32->writeAmplitude( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ), pulseAmplitude_ );
	le32->enablePulses( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ) );
	for ( int iPulse = 0; iPulse < nPulses; ++iPulse ) le32->exec();
      }
      else{
	le32->enablePulses( LE32::NoCh );
      }
   }
  }
  bsem_.give();
  return isToKeepRunning_;
}

void AFEB::teststand::Calibration::setDACValues( const map<string,string> DACValues ){
  cout << "AFEB::teststand::Calibration::setDACValues" << endl;
  map<string,string>::const_iterator v;
  v = DACValues.find( "thresholdLevel" );
  if ( v != DACValues.end() ){
    cout << "thresholdLevel=" << v->second << endl;
    setThresholdLevel( utils::stringTo<int>( v->second ) );
  }
  v = DACValues.find( "pulseApmlitude" );
  if ( v != DACValues.end() ){
    cout << "pulseApmlitude=" << v->second << endl;
    setPulseAmplitude( utils::stringTo<int>( v->second ) );
  }
}
