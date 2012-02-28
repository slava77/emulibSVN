#include "AFEB/teststand/AnalyzedDevice.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/utils/System.h"
#include "xcept/Exception.h"

#include "TMath.h"

#include <iomanip>

const string AFEB::teststand::AnalyzedDevice::analyzedDeviceNamespace_( "http://cms.cern.ch/emu/afeb/teststand/analyzeddevice" );

ostream& AFEB::teststand::operator<<( ostream& os, const AnalyzedDevice& d ){
  os << *dynamic_cast<const TestedDevice*>( &d );
  os << "correction coefficient      injection capacitance [pF]      pulse division factor" << endl
     << right
     << setw(22) << d.correctionCoefficient_ << "     "
     << setw(26) << d.injectionCapacitance_  << "     "
     << setw(21) << d.pulseDivisionFactor_
     << endl << "Channels:" << endl;
  int i=1;
  for ( vector<AnalyzedChannel>::const_iterator c = d.channels_.begin(); c != d.channels_.end(); ++c ){
    cout << right << setw(10) << i++ << *c << endl;
  }  
  return os;
}

AFEB::teststand::AnalyzedDevice::AnalyzedDevice( const TestedDevice& device, const string& rawResultsDir, const vector<Measurement*>& measurements )
  : TestedDevice( device )
  , rawResultsDir_( rawResultsDir )
  , measurements_( measurements )
  , correctionCoefficient_( 0. )
  , injectionCapacitance_ ( 0. )
  , pulseDivisionFactor_  ( 0. )
{
  channels_.resize( nChannels_ );
}

DAC AFEB::teststand::AnalyzedDevice::getPulseDACDescriptor() const {
  return DAC( crate_->getModule( pulseGeneratorSlot_ )->getId(),
	      crate_->getModule( pulseGeneratorSlot_ )->getName(),
	      DAC::getType( crate_->getModule( pulseGeneratorSlot_ )->getType() ),
	      pulseGeneratorSocket_ );
}

DAC AFEB::teststand::AnalyzedDevice::getThresholdDACDescriptor() const {
  return DAC( crate_->getModule( signalConverterSlot_ )->getId(),
	      crate_->getModule( signalConverterSlot_ )->getName(),
	      DAC::getType( crate_->getModule( signalConverterSlot_ )->getType() ),
	      signalConverterSocket_ );
}

void AFEB::teststand::AnalyzedDevice::addThresholdMeasurement( const int iChannel, 
							       const pair<double,double> V_setThreshold ,
							       const pair<double,double> V_measuredThreshold,
							       const pair<double,double> V_measuredNoise ){
  // Fit Q(V) instead of V(Q) to better estimate the error of the fit parameters. (The error is on Q rather than on V.)

  TMatrixD    x  ( 1, 1 );
  TMatrixD    y  ( 1, 1 );
  TMatrixDSym var( 1 );

  // Convert measured threshold voltage to injected charge [fC]. Take into account the correction coefficient as well:
  x  ( 0, 0 ) = V_setThreshold.first;
  y  ( 0, 0 ) = chargeFromVoltage( V_measuredThreshold.first );
  var( 0, 0 ) = TMath::Power( chargeFromVoltage( V_measuredThreshold.second ), 2 );
  channels_[iChannel].QofVfitter_.addObservation( x, y, var );

  // Add the noise measurements as well to average them while we're at it:
  y  ( 0, 0 ) = chargeFromVoltage( V_measuredNoise.first );
  var( 0, 0 ) = TMath::Power( chargeFromVoltage( V_measuredNoise.second ), 2 );
  channels_[iChannel].noiseAverager_.addObservation( x, y, var );

  // cout << channels_[iChannel].noiseAverager_.getObservationCount() << " Added ( " << x(0,0) << ", " << y(0,0) << "+-" << TMath::Sqrt( var(0,0) ) << " )" << endl;
}

void AFEB::teststand::AnalyzedDevice::calculateGains(){
  // Loop over the count_vs_dac measurements with pulses through external capacitors
  for ( vector<Measurement*>::const_iterator m = measurements_.begin(); m != measurements_.end(); ++m ){
    if ( (*m)->getTypeType() == Measurement::count_vs_dac && (*m)->getInjectionCapacitorType() == Measurement::external ){      
      // Open the root file of this measurement's results...
      string fileName = Results::getFileName( (*m)->getIndex(), (*m)->getType(), id_ );
      TFile f( ( rawResultsDir_ + "/" + fileName +".root").c_str(), "READ" );
      if ( f.IsZombie() ){
	stringstream ss;
	ss << "Failed to open ROOT file " << f.GetName();
	XCEPT_RAISE( xcept::Exception, ss.str() );
      }
      cout << "Opened " << f.GetName() << endl;
      f.cd();
      // ...and get the threshold histogram...
      string histogramName = string( "threshold__" ) + fileName;
      TH1D *thresholdHistogram;
      f.GetObject( histogramName.c_str(), thresholdHistogram );
      // ...and the noise histogram, too, while we're at it:
      histogramName = string( "noise__" ) + fileName;
      TH1D *noiseHistogram;
      f.GetObject( histogramName.c_str(), noiseHistogram );
      
      // Loop over the channels
      for ( int iChannel=0; iChannel<nChannels_; ++iChannel ){
	addThresholdMeasurement( iChannel,
				 thresholdDAC_->mV_from_DACUnit( (*m)->getSetThreshold() ),
				 pulseDAC_    ->mV_from_DACUnit( thresholdHistogram->GetBinContent( iChannel+1 ),
								 thresholdHistogram->GetBinError  ( iChannel+1 )  ),
				 pulseDAC_    ->mV_from_DACUnit( noiseHistogram    ->GetBinContent( iChannel+1 ),
								 noiseHistogram    ->GetBinError  ( iChannel+1 )  )
				 );
      } // for ( int iChannel=0; iChannel<nChannels; ++iChannel ){
      
      f.Close();
    } // if ( (*m)->getTypeType() == Measurement::count_vs_dac && (*m)->getInjectionCapacitorType() == Measurement::external )
  } // for ( vector<Measurement*>::const_iterator m = measurements.begin(); m != measurements.end(); ++m ){
  
  // Now we're ready to calculate the gains
  for ( vector<AnalyzedChannel>::iterator c = channels_.begin(); c != channels_.end(); ++c ){
    c->calculateGain();
  }
}

void AFEB::teststand::AnalyzedDevice::calculateInternalCapacitances(){
  // Find the count_vs_dac measurements with charge injection through internal capacitors
  for ( vector<Measurement*>::const_iterator m = measurements_.begin(); m != measurements_.end(); ++m ){
    if ( (*m)->getTypeType() == Measurement::count_vs_dac && (*m)->getInjectionCapacitorType() == Measurement::internal ){      
      // Open the root file of this measurement's results...
      string fileName = Results::getFileName( (*m)->getIndex(), (*m)->getType(), id_ );
      TFile f( ( rawResultsDir_ + "/" + fileName +".root").c_str(), "READ" );
      if ( f.IsZombie() ){
	stringstream ss;
	ss << "Failed to open ROOT file " << f.GetName();
	XCEPT_RAISE( xcept::Exception, ss.str() );
      }
      cout << "Opened " << f.GetName() << endl;
      f.cd();
      // ...and get the threshold histogram...
      string histogramName = string( "threshold__" ) + fileName;
      TH1D *thresholdHistogram;
      f.GetObject( histogramName.c_str(), thresholdHistogram );
      
      // Loop over the channels
      for ( int iChannel=0; iChannel<nChannels_; ++iChannel ){
	channels_[iChannel]
	  .calculateInternalCapacitance( thresholdDAC_->mV_from_DACUnit( (*m)->getSetThreshold()                         ), // set threshold 
					 pulseDAC_    ->mV_from_DACUnit( thresholdHistogram->GetBinContent( iChannel+1 ),
									 thresholdHistogram->GetBinError  ( iChannel+1 ) )  // measured threshold
					 );
      } // for ( int iChannel=0; iChannel<d->getNChannels(); ++iChannel )
      
      f.Close();
    } // if ( (*m)->getTypeType() == Measurement::count_vs_dac && (*m)->getInjectionCapacitorType() == Measurement::internal )
  } // for ( vector<Measurement*>::const_iterator m = measurements.begin(); m != measurements.end(); ++m )
}

valarray<double> AFEB::teststand::AnalyzedDevice::getNoises() const {
  valarray<double> v( channels_.size() );
  size_t n = 0;
  for ( vector<AnalyzedChannel>::const_iterator c = channels_.begin(); c != channels_.end(); ++c ) v[n++] = c->noise_;
  return v;
}

valarray<double> AFEB::teststand::AnalyzedDevice::getGains() const {
  valarray<double> v( channels_.size() );
  size_t n = 0;
  for ( vector<AnalyzedChannel>::const_iterator c = channels_.begin(); c != channels_.end(); ++c ) v[n++] = c->gain_;
  return v;
}

valarray<double> AFEB::teststand::AnalyzedDevice::getOffsets() const {
  valarray<double> v( channels_.size() );
  size_t n = 0;
  for ( vector<AnalyzedChannel>::const_iterator c = channels_.begin(); c != channels_.end(); ++c ) v[n++] = c->offset_;
  return v;
}

valarray<double> AFEB::teststand::AnalyzedDevice::getInternalCapacitances() const {
  valarray<double> v( channels_.size() );
  size_t n = 0;
  for ( vector<AnalyzedChannel>::const_iterator c = channels_.begin(); c != channels_.end(); ++c ) v[n++] = c->internalCapacitance_;
  return v;
}

string AFEB::teststand::AnalyzedDevice::measurementsToXML() const {
  stringstream ss;
  for ( vector<Measurement*>::const_iterator m = measurements_.begin(); m != measurements_.end(); ++m ){
    ss.str() = "";
    ss << "  <ad:measurement index=\""             << noshowpos 
       <<                                             (*m)->getIndex()
       <<                  "\" type=\""              << (*m)->getType()
       <<                  "\" capacitor=\""         << (*m)->getInjectionCapacitor()
       <<                  "\" nPulses=\""           << (*m)->getNPulses()
       <<                  "\" setThreshold.ADC=\"" << (*m)->getSetThreshold()
       <<                  "\" setThreshold.mV=\""  << thresholdDAC_->mV_from_DACUnit( (*m)->getSetThreshold() ).first
       <<   "\">" << endl;

    // Open the root file of this measurement's results...
    string fileName = Results::getFileName( (*m)->getIndex(), (*m)->getType(), id_ );
    TFile f( ( rawResultsDir_ + "/" + fileName +".root").c_str(), "READ" );
    if ( f.IsZombie() ){
      stringstream ss;
      ss << "Failed to open ROOT file " << f.GetName();
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    cout << "Opened " << f.GetName() << endl;
    f.cd();
    // ...and get the threshold histogram...
    string histogramName = string( "threshold__" ) + fileName;
    TH1D *thresholdHistogram;
    f.GetObject( histogramName.c_str(), thresholdHistogram );
    // ...and the noise histogram...
    histogramName = string( "noise__" ) + fileName;
    TH1D *noiseHistogram;
    f.GetObject( histogramName.c_str(), noiseHistogram );
    // ...and the chi^2/ndf histogram:
    histogramName = string( "chi2ndf__" ) + fileName;
    TH1D *chi2ndfHistogram;
    f.GetObject( histogramName.c_str(), chi2ndfHistogram );

    // Loop over the channels
    for ( int iChannel=0; iChannel<nChannels_; ++iChannel ){
      ss << "    <ad:channel number=\""       << noshowpos << setw(2) 
	 <<                                   iChannel
	 <<              "\" threshold.fC=\"" << showpos << showpoint << setw(8) << setprecision(4) << fixed
	 <<                                   chargeFromVoltage( pulseDAC_->mV_from_DACUnit( thresholdHistogram->GetBinContent( iChannel+1 ) ).first )
	 <<              "\" noise.fC=\""
	 <<                                   chargeFromVoltage( pulseDAC_->mV_from_DACUnit( noiseHistogram    ->GetBinContent( iChannel+1 ) ).first )
	 <<              "\" chi2ndf=\""
	 <<                                                                                  chi2ndfHistogram  ->GetBinContent( iChannel+1 )
	 <<     "\"/>" << endl;
    } // for ( int iChannel=0; iChannel<d->getNChannels(); ++iChannel )
    
    f.Close();
    ss << "  </ad:measurement>" << endl;
  }
  return ss.str();
}

string AFEB::teststand::AnalyzedDevice::statisticsToXML( const string& name, const valarray<double>& values ) const {
  stringstream ss;
  map<string,double> stats = utils::statistics( values );
  ss << "<ad:" << name ;
  for ( map<string,double>::const_iterator s=stats.begin(); s!=stats.end(); ++s ){
    ss << " " << s->first << "=\"" << showpos << showpoint << setprecision(4) << setw(8) << fixed << s->second << "\"";
  }
  ss <<   "/>";
  return ss.str();
}

double AFEB::teststand::AnalyzedDevice::getMaxMeasuredThreshold( const double setThreshold ){
  TMatrixD x( 1, 1 );
  x( 0, 0 ) = setThreshold;
  valarray<double> thresholds( nChannels_ );
  for ( int iChannel=0; iChannel<nChannels_; ++iChannel ){
    thresholds[iChannel] = channels_[iChannel].QofVfitter_.getY( x )( 0, 0 );
  }
  return utils::statistics( thresholds )["max"];
}

void AFEB::teststand::AnalyzedDevice::saveResults( const string& analyzedResultsDir ){

  utils::execShellCommand( string( "mkdir -p " ) + analyzedResultsDir );

  stringstream ss;
  ss << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"        << endl
     << "<?xml-stylesheet type=\"text/xml\" href=\"analyzedResults_XSLT.xml\"?>" << endl;


  ss << "<ad:device xmlns:ad=\""     << analyzedDeviceNamespace_
     <<         "\" type=\""         << type_
     <<         "\" id=\""           << id_
     <<         "\" analysisDate=\"" << utils::getDateTime()
     << "\">" << endl
     << "  <ad:adaptor name=\""                    << adaptorName_
     <<            "\" socket=\""                  << socket_
     <<            "\" correctionCoefficient=\""   << noshowpos << showpoint << setprecision(4) << setw(8) << fixed << correctionCoefficient_
     <<            "\" injectionCapacitance.fC=\"" << noshowpos << showpoint << setprecision(4) << setw(8) << fixed << injectionCapacitance_
     <<            "\" pulseDivisionFactor=\""     << noshowpos << showpoint << setprecision(4) << setw(8) << fixed << pulseDivisionFactor_
     <<   "\"/>" << endl;

  ss << measurementsToXML();

  size_t iChannel = 0;
  for ( vector<AnalyzedChannel>::const_iterator c = channels_.begin(); c != channels_.end(); ++c ){
    ss << "  <ad:channel noise.fC=\""     << showpos << showpoint << setw(8) << setprecision(4) << fixed << c->noise_
       <<            "\" offset.mV=\""    << showpos << showpoint << setw(8) << setprecision(4) << fixed << c->offset_
       <<            "\" gain.mV.fC-1=\"" << showpos << showpoint << setw(8) << setprecision(4) << fixed << c->gain_
       <<            "\" C_int.pF=\""     << showpos << showpoint << setw(8) << setprecision(4) << fixed << c->internalCapacitance_
       <<            "\" number=\""       << setw(2) << iChannel++
       <<   "\"/>" << endl;
  }
  
  ss << "  " << statisticsToXML( "noise.fC",      getNoises()               ) << endl;
  ss << "  " << statisticsToXML( "gain.mV.fC-1 ", getGains()                ) << endl;
  ss << "  " << statisticsToXML( "offset.mV",     getOffsets()              ) << endl;
  ss << "  " << statisticsToXML( "C_int.pF",      getInternalCapacitances() ) << endl;
  
  ss << "  <ad:averageSetThreshold value.mV=\"" 
     << utils::statistics( getOffsets() )["mean"] + utils::statistics( getGains() )["mean"] * 20
     <<                        "\" atCharge.fC=\"20\"/>" << endl;

  ss << "  <ad:maxMeasuredThreshold value.fC=\"" << getMaxMeasuredThreshold( 0 )
     <<                         "\" atSetThreshold.mV=\"0\"/>" << endl;

  ss << "</ad:device>" << endl;

  cout << ss.str();

  utils::writeFile( analyzedResultsDir + "/" + id_ + ".xml", ss.str() );

}
