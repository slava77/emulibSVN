#include "AFEB/teststand/Measurement.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/crate/LE32.h"
#include "AFEB/teststand/crate/LeCroy3377.h"
#include "AFEB/teststand/crate/Jorway73A.h"
#include "xcept/Exception.h"
#include "TRandom3.h"

#include <iomanip>

using namespace std;
using namespace AFEB::teststand;

const char* const AFEB::teststand::Measurement::typeString_[] = { "count_vs_dac", "time_vs_dac" };
const char* const AFEB::teststand::Measurement::statusString_[] = { "waiting", "running", "done" };
const char* const AFEB::teststand::Measurement::capacitorString_[] = { "external", "internal" };

ostream& AFEB::teststand::operator<<( ostream& os, const Measurement& m ){

  os << "Measurement " << m.index_ << ": '" << m.name_ << "' of type " << m.getTypeString() << " (" << m.type_ << ") at position " << m.position_ << endl
     << " injectionCapacitor="  << m.capacitorString_[m.injectionCapacitor_]
     << " amplitudeMin="        << m.amplitudeMin_        
     << " amplitudeMax="        << m.amplitudeMax_        
     << " amplitudeStep="       << m.amplitudeStep_       << endl
     << " nPulses="             << m.nPulses_             
     << " thresholdValue="      << m.thresholdValue_      << endl
     << " tdcTimeMin="          << m.tdcTimeMin_          
     << " tdcTimeMax="          << m.tdcTimeMax_          << endl 
     << "  Tested devices:"                               << endl;

  map<TestedDevice*,Results*>::const_iterator r;
  for ( r = m.results_.begin(); r != m.results_.end(); ++r ){
    os << *r->first;
  }

  return os;
}

AFEB::teststand::Measurement::Measurement( const int position, const int index, const string name, const string type, const string resultDir, bool generateDummyData ) :
  bsem_( toolbox::BSem::EMPTY ), // locked
  position_( position ),
  index_( index ),
  name_( name ),
  type_( getType( type ) ),
  status_( AFEB::teststand::Measurement::waiting ),
  resultDir_( resultDir ),
  generateDummyData_( generateDummyData ),
  isToKeepRunning_( true ),
  tdcResolution_( 0.5 ) // [ns] Will set TDC to this resolution. It can be hard-coded as it's never changed.
{
  bsem_.give();
}

AFEB::teststand::Measurement::~Measurement(){
  // Delete results. (The tested devices were created by Configuration, and will also be deleted by it.)
  for ( map<TestedDevice*,Results*>::iterator r = results_.begin(); r != results_.end(); ++r ) delete r->second;
}

void AFEB::teststand::Measurement::setPulseParameters( const vector< pair<string,string> >& param ){
  bsem_.take();
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if      ( p->first.compare( "amplitudeMin"  ) == 0 ) amplitudeMin_       = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "amplitudeMax"  ) == 0 ) amplitudeMax_       = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "amplitudeStep" ) == 0 ) amplitudeStep_      = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "nPulses"       ) == 0 ) nPulses_            = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "capacitor"     ) == 0 ) injectionCapacitor_ = getInjectionCapacitor( p->second );
  } // for ( p = param.begin(); p != param.end(); ++p )

  //cout << param << endl << *this << endl;
  bsem_.give();
}

void AFEB::teststand::Measurement::setThresholdParameters( const vector< pair<string,string> >& param ){
  bsem_.take();
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if ( p->first.compare( "thresholdValue" ) == 0 ) thresholdValue_ = utils::stringTo<int>( p->second );
  }
  bsem_.give();
}

void AFEB::teststand::Measurement::setTDCParameters( const vector< pair<string,string> >& param ){
  bsem_.take();
  vector< pair<string,string> >::const_iterator p;
  for ( p = param.begin(); p != param.end(); ++p ){
    if      ( p->first.compare( "timeMin" ) == 0 ) tdcTimeMin_ = utils::stringTo<int>( p->second );
    else if ( p->first.compare( "timeMax" ) == 0 ) tdcTimeMax_ = utils::stringTo<int>( p->second );
  }
  bsem_.give();
}

void AFEB::teststand::Measurement::addTestedDevice( TestedDevice* device ){
  bsem_.take();
  // Check if this input has already been taken:
  map<TestedDevice*,Results*>::const_iterator r;
  for ( r = results_.begin(); r != results_.end(); ++r ){
    if ( r->first->getTDCSlot()  == device->getTDCSlot()  && 
	 r->first->getTDCSocket() == device->getTDCSocket()    ){
      stringstream ss;
      ss << "A tested device (" << r->first->getId() 
	 << ") is already configured to be read out by input " << device->getTDCSocket() 
	 << " of the TDC in slot " << device->getTDCSlot();
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
  }
  // Create results for it, too:
  Results *results = new Results( this, device );
  results_.insert( make_pair( device, results ) );
  bsem_.give();
}

const TestedDevice* AFEB::teststand::Measurement::getTestedDevice( const int tdcSocket ) const {
  map<TestedDevice*,Results*>::const_iterator r;
  for ( r = results_.begin(); r != results_.end(); ++r ){
    if ( r->first->getTDCSocket() == tdcSocket ) return r->first;
  }
  return NULL;
}

const TestedDevice* AFEB::teststand::Measurement::getTestedDevice( const string& deviceId ) const {
  map<TestedDevice*,Results*>::const_iterator r;
  for ( r = results_.begin(); r != results_.end(); ++r ){
    if ( r->first->getId() == deviceId ) return r->first;
  }
  return NULL;  
}

TestedDevice* AFEB::teststand::Measurement::findTestedDevice( const int tdcSocket ) {
  map<TestedDevice*,Results*>::iterator r;
  for ( r = results_.begin(); r != results_.end(); ++r ){
    if ( r->first->getTDCSocket() == tdcSocket ) return r->first;
  }
  return NULL;
}

Results* AFEB::teststand::Measurement::findResults( const int tdcSocket ) {
  map<TestedDevice*,Results*>::iterator r;
  for ( r = results_.begin(); r != results_.end(); ++r ){
    if ( r->first->getTDCSocket() == tdcSocket ) return r->second;
  }
  return NULL;
}

int AFEB::teststand::Measurement::getTDCSlot() const {
  if ( results_.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, "Failed to get TDC slot number for this measurement as it has no tested devices." );
  }
  // By definition, all tested devices of a measurement are read out by the same TDC. Take the first one:
  return results_.begin()->first->getTDCSlot();
}

bool AFEB::teststand::Measurement::execute(){
  bsem_.take();
  if ( ! isToKeepRunning_ ){
    status_ = AFEB::teststand::Measurement::waiting;
    bsem_.give();
    return false;
  }
  bool keepRunning = true;
  status_ = AFEB::teststand::Measurement::running;
  bsem_.give();

  switch ( type_ ){
  case count_vs_dac:
  case time_vs_dac:
    keepRunning = ( generateDummyData_ ? dummyResultGenerator() : countVsDAQ() );
    break;
  default:
    stringstream ss;
    ss << "Unknown measurement type " << type_;
    XCEPT_RAISE( xcept::Exception, ss.str() );
    break;
  }

  bsem_.take();
  // Save everything if it hasn't been aborted:
  if ( keepRunning ) for ( map<TestedDevice*,Results*>::iterator r = results_.begin(); r != results_.end(); ++r ) r->second->save( resultDir_ );

  status_ = AFEB::teststand::Measurement::done;
  bsem_.give();

  return keepRunning;
}

void AFEB::teststand::Measurement::refitSCurves(){
  if ( type_ == count_vs_dac ) for ( map<TestedDevice*,Results*>::iterator r = results_.begin(); r != results_.end(); ++r ) r->second->refitSCurves( resultDir_ );
  status_ = done;
}

bool AFEB::teststand::Measurement::countVsDAQ(){

  time_t timeOfLastUpdate = 0;

  // All tested devices in one measurement are read out by the same TDC. Take the first device to get its.
  int nDeviceChannels = results_.begin()->first->getNChannels();
  Crate* crate = results_.begin()->first->getCrate();
  LeCroy3377*       tdc = static_cast<LeCroy3377*>( crate->getModule( results_.begin()->first->getTDCSlot() ) );
  // All tested devices's signals in one measurement are fed through the same signal converter module. Take the first device to get its.
  LE32* signalConverter = static_cast<LE32*>( crate->getModule( results_.begin()->first->getSignalConverterSlot() ) );
  // Use either the specified pulse generator for common injection (through external capacitor), 
  // or the corresponding signal converter module to inject individually (through external or internal capacitor). 
  LE32*  pulseGenerator = NULL;
  if      ( injectionCapacitor_ == external ) pulseGenerator = static_cast<LE32*>( crate->getModule( results_.begin()->first->getPulseGeneratorSlot() ) );
  else if ( injectionCapacitor_ == internal ) pulseGenerator = signalConverter;
  CrateController* controller = crate->getCrateController();

  controller->initialize();

  // TODO: remove debug
  // cout << "***AFEB::teststand::Measurement::countVsDAQ***" << endl << *crate << endl;  
  // while ( true ){
  //   controller->read( CAMAC::A0, CAMAC::F1, CAMAC::N7 );
  // }
  //

  // Zero CAMAC:
  //controller->z(); // TODO: needed? Takes very long, see why.

  // BEGIN test pulses
  // pulseGenerator->writeAmplitude( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ), amplitudeMax_ );
  // pulseGenerator->enablePulses(  (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ) );
  // pulseGenerator->writeAmplitude( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ), amplitudeMax_ );
  // cout << "Wrote amplitude " << amplitudeMax_ 
  //      << " to channels " <<  (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 )
  //      << endl;
  // for( int i=0; i<100000; i++ ){
  //   if ( i%1000 == 0 ) std::cout << "pulse " << std::dec << i << std::endl;
  //   pulseGenerator->exec();
  // }
  // END test pulses

  // Set up TDC:
  // Mode1 = M1    : Common Start , Single Word
  // Shift = 0     : 0.5 ns resolution
  // Hit   = 2     : max 2 hits per channel allowed
  // Edge  = 0     : only leading edge recorded
  // Mpi   = 0     : no Measure Pause Interval
  // TimeOut = 550 : time out at 550 ns (in multiples of 50ns, should be slightly longer than the enforced time out delay)
  // TimeEnf = 511 : time out enforced at 511 ns (10-bit data for single word, leading edge only --> max 1024 * 0.5 ns = 512 ns)
  tdc->Set( LeCroy3377::M1, 0, 2, 0, 0, 550, 511 ); // ( Mode, Shift, Hit, Edge, Mpi, TimeOut, TimeEnf )

  // Set up threshold-setting module:
  signalConverter->writeThreshold( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ), thresholdValue_ );
  signalConverter->writeAmplitude( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ), 0 );
  signalConverter->enablePulses( LE32::NoCh );

  // Set pulse generator
  pulseGenerator->writeAmplitude( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ), amplitudeMin_ );

  // Gradually crank up the pulse height:
  for ( int amplitude = amplitudeMin_; amplitude <= amplitudeMax_; amplitude += amplitudeStep_ ){

    pulseGenerator->writeAmplitude( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ), amplitude );
    pulseGenerator->enablePulses  ( (LE32::Channel_t)( LE32::Ch1 | LE32::Ch2 ) );
    pulseGenerator->exec();
    tdc->Clear();
    tdc->EnableAcq();

    // Send pulses
    for ( int iPulse = 0; iPulse < nPulses_; ++iPulse ){

      tdc->Clear();
      pulseGenerator->exec();
      
      tdc->HeadRdBlock(); // This does the actual hardware readout.

      // Check whether we've been instructed to abort in the meantime:
      if ( ! isToKeepRunning_ ) return false;

      // Read out TDC channels
      for ( int iShort = 0; iShort < LeCroy3377::nShortsData; ++iShort ){ // TODO: LeCroy3377::nShortsData / 2 ?
	// LeCroy3377::BlockRd sets LeCroy3377::TimeCh, which can be accessed by LeCroy3377::TimeChRd(),
	//                     and LeCroy3377::Channel, which can be accessed by LeCroy3377::ChannelRd()
	if ( !tdc->BlockRd( iShort ) ) break;
	int tdcSocket = tdc->ChannelRd() / nDeviceChannels + 1;
	Results* results = findResults( tdcSocket );
	if ( results ) results->add( tdc->ChannelRd() % nDeviceChannels, amplitude, tdc->TimeChRd() );
	// Check whether we've been instructed to abort in the meantime:
	if ( ! isToKeepRunning_ ) return false;
      } // for ( int iShort = 0; iShort < LeCroy3377::nShortsData; ++iShort )

    } // for ( int iPulse = 0; iPulse < nPulses_; ++iPulse )

    // Update stored results for the first and last amplitude or if they were updated too ling ago
    time_t now;
    time( &now );
    if ( now - timeOfLastUpdate > 10 || amplitude == amplitudeMin_ || amplitude == amplitudeMax_ ){
      timeOfLastUpdate = now;
      for ( map<TestedDevice*,Results*>::iterator r = results_.begin(); r != results_.end(); ++r ){
	r->second->updateOutput( resultDir_, amplitudeMin_, amplitude );
      }
    }

  } // for ( int amplitude = amplitudeMin_; amplitude <= amplitudeMax_; amplitude += amplitudeStep_ )

  return true;
}


bool AFEB::teststand::Measurement::dummyResultGenerator(){
  // All tested devices in one measurement are read out by the same TDC. Take the first device to get its.
  int nDeviceChannels = results_.begin()->first->getNChannels();

  TRandom3 rndm;

  // Spread the threshold so that not all channels have it at the same value:
  double thresholdOffset[LeCroy3377::nShortsData / 2];
  for ( int iShort = 0; iShort < LeCroy3377::nShortsData / 2; ++iShort ){
    thresholdOffset[ iShort ] = rndm.Gaus( 0., 0.08 * ( amplitudeMax_ - amplitudeMin_ ) );
  }

  time_t timeOfLastUpdate = 0;

  // Gradually crank up the pulse height:
  for ( int amplitude = amplitudeMin_; amplitude <= amplitudeMax_; amplitude += amplitudeStep_ ){

    // cout << "amplitude=" << amplitude << endl;

    //::usleep(10); // TODO: remove

    // Send pulses
    for ( int iPulse = 0; iPulse < nPulses_; ++iPulse ){

      //cout << "   pulse=" << iPulse << endl;

      // Read out TDC channels
      for ( int iShort = 0; iShort < LeCroy3377::nShortsData / 2; ++iShort ){
	//cout << "         data word=" << iShort << endl;
	int tdcSocket = iShort / nDeviceChannels + 1;
	Results* results = findResults( tdcSocket );
	if ( results ){
	  if ( double(amplitude) > rndm.Gaus( thresholdValue_ + thresholdOffset[ iShort ],
					      0.1 * ( amplitudeMax_ - amplitudeMin_ ) ) ){
	    results->add( iShort % nDeviceChannels, 
			  amplitude,
			  rndm.Gaus( 0.5 * ( tdcTimeMax_ + tdcTimeMin_ ), 
				     0.1 * ( tdcTimeMax_ - tdcTimeMin_ ) ) );
	    // Check whether we've been instructed to abort in the meantime:
	    if ( ! isToKeepRunning_ ) return false;
	  }
	}
      } // for ( int iShort = 0; iShort < LeCroy3377::nShortsData; ++iShort )

    } // for ( int iPulse = 0; iPulse < nPulses_; ++iPulse )

    // Update stored results for the first and last amplitude or if they were updated too ling ago
    time_t now;
    time( &now );
    if ( now - timeOfLastUpdate > 1 || amplitude == amplitudeMin_ || amplitude == amplitudeMax_ ){
      timeOfLastUpdate = now;
      for ( map<TestedDevice*,Results*>::iterator r = results_.begin(); r != results_.end(); ++r ){
	r->second->updateOutput( resultDir_, amplitudeMin_, amplitude );
      }
    }

  } // for ( int amplitude = amplitudeMin_; amplitude <= amplitudeMax_; amplitude += amplitudeStep_ )

  return true;
}

string AFEB::teststand::Measurement::resultsXML(){
  stringstream ss;
  ss << "<a:measurement index=\""  << getIndex()
     <<             "\" type=\""   << getTypeString()
     <<             "\" name=\""   << getName()
     <<             "\" status=\"" << getStatusString() 
     <<             "\">" << endl;
  for ( map<TestedDevice*,Results*>::const_iterator r = results_.begin(); r != results_.end(); ++r ){
    ss << "<a:device id=\"" << r->first->getId() << "\">" << endl
       << "<a:file name=\"" << r->second->getFileName() << "\"/>" << endl;
    // Loop over channels and get fit results
    for ( int iChannel = 0; iChannel < r->first->getNChannels(); ++iChannel ){
      ss << "<a:channel number=\"" <<  iChannel << "\">";
      map<string,pair<double,double> > parameters = r->second->getParameters( iChannel );
      for ( map<string,pair<double,double> >::const_iterator p = parameters.begin(); p != parameters.end(); ++p ){ 
	ss << "<a:parameter name=\""  << p->first
	   <<           "\" value=\"" <<   showpos << showpoint << setprecision(6) << p->second.first
	   <<           "\" error=\"" << noshowpos << showpoint << setprecision(6) << p->second.second
	   <<           "\"/>" << noshowpos << noshowpoint;
      }
      ss << "</a:channel>" << endl;
    } // for ( int iChannel = 1; iChannel <= r->first->getNChannels(); ++iChannel )
      // Add statistics to assess stability and uniformity of...
    ss << "<a:statistics>" << endl;
    map<string,double> stat;
    if ( getType() == Measurement::count_vs_dac ){
      // ...threshold,... 
      stat = r->second->getThresholdStats();
      ss << "<a:parameter name=\"threshold [DAC units]\"";
      for ( map<string,double>::const_iterator s=stat.begin(); s!=stat.end(); ++s ){
	ss << " " << s->first << "=\"" << noshowpos << showpoint << setprecision(6) << s->second << "\"";
      }
      ss << "/>" << endl;
      // ...noise,...
      stat = r->second->getNoiseStats();
      ss << "<a:parameter name=\"noise [DAC units]\"";
      for ( map<string,double>::const_iterator s=stat.begin(); s!=stat.end(); ++s ){
	ss << " " << s->first << "=\"" << noshowpos << showpoint << setprecision(6) << s->second << "\"";
      }
      ss << "/>" << endl;
      // ...chi^2/ndf,...
      stat = r->second->getChi2NDFStats();
      ss << "<a:parameter name=\"&#x03c7;&#xb2;/ndf\"";
      for ( map<string,double>::const_iterator s=stat.begin(); s!=stat.end(); ++s ){
	ss << " " << s->first << "=\"" << noshowpos << showpoint << setprecision(6) << s->second << "\"";
      }
      ss << "/>" << endl;
    } // if ( m->getType() == Measurement::count_vs_dac )
    if ( getType() == Measurement::time_vs_dac ){
      // ...mean times...
      stat = r->second->getTimeStats();
      ss << "<a:parameter name=\"mean time on plateau [TDC units]\"";
      for ( map<string,double>::const_iterator s=stat.begin(); s!=stat.end(); ++s ){
	ss << " " << s->first << "=\"" << noshowpos << showpoint << setprecision(6) << s->second << "\"";
      }
      ss << "/>" << endl;      
      // ...mean time spans at each amplitude...
      stat = r->second->getTimeSpanStats();
      ss << "<a:parameter name=\"span of channels' mean times [TDC units]\"";
      for ( map<string,double>::const_iterator s=stat.begin(); s!=stat.end(); ++s ){
	ss << " " << s->first << "=\"" << noshowpos << showpoint << setprecision(6) << s->second << "\"";
      }
      ss << "/>" << endl;      
      // ...and slewing times on the efficiency plateau.
      stat = r->second->getSlewStats();
      ss << "<a:parameter name=\"slewing time on plateau [TDC units]\"";
      for ( map<string,double>::const_iterator s=stat.begin(); s!=stat.end(); ++s ){
	ss << " " << s->first << "=\"" << noshowpos << showpoint << setprecision(6) << s->second << "\"";
      }
      ss << "/>" << endl;
    } // if ( m->getType() == Measurement::time_vs_dac )
    ss << "</a:statistics>" << endl;
    ss << "</a:device>" << endl;
  } // for ( map<TestedDevice*,Results*>::const_iterator r = results.begin(); r != results.end(); ++r )
  ss << "</a:measurement>" << endl;
  return ss.str();
}

// static function
AFEB::teststand::Measurement::Type_t AFEB::teststand::Measurement::getType( const string& typeString ){
  AFEB::teststand::Measurement::Type_t type = AFEB::teststand::Measurement::nTypes;
  bool isValidType = false;
  for ( int i=0; i<AFEB::teststand::Measurement::nTypes && !isValidType; ++i ){
    isValidType |= ( typeString.compare( typeString_[i] ) == 0 );
    if ( isValidType ) type = (AFEB::teststand::Measurement::Type_t) i;
  }
  if ( !isValidType ){
    stringstream ss;
    ss << "\"" << typeString << "\" is not a valid measurement type.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return type;
}

// static function
string AFEB::teststand::Measurement::getTypeString( const Type_t type ){
  if ( type < (AFEB::teststand::Measurement::Type_t)0 || 
       type >= AFEB::teststand::Measurement::nTypes      ){
    stringstream ss;
    ss << "'" << type << "' is not a valid measurement type.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return typeString_[type];
}

// static function
AFEB::teststand::Measurement::Capacitor_t AFEB::teststand::Measurement::getInjectionCapacitor( const string& injectionCapacitorString ){
  AFEB::teststand::Measurement::Capacitor_t capacitor = AFEB::teststand::Measurement::nCapacitors;
  bool isValidType = false;
  for ( int i=0; i<AFEB::teststand::Measurement::nCapacitors && !isValidType; ++i ){
    isValidType |= ( injectionCapacitorString.compare( capacitorString_[i] ) == 0 );
    if ( isValidType ) capacitor = (AFEB::teststand::Measurement::Capacitor_t) i;
  }
  if ( !isValidType ){
    stringstream ss;
    ss << "\"" << injectionCapacitorString << "\" is not a valid injection capacitor type.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return capacitor;
}

// static function
string AFEB::teststand::Measurement::getInjectionCapacitorString( const Capacitor_t injectionCapacitor ){
  if ( injectionCapacitor < (AFEB::teststand::Measurement::Capacitor_t)0 || 
       injectionCapacitor >= AFEB::teststand::Measurement::nCapacitors      ){
    stringstream ss;
    ss << "'" << injectionCapacitor << "' is not a valid injection capacitor type.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return capacitorString_[injectionCapacitor];
}