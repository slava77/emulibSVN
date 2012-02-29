#include "AFEB/teststand/AnalyzedDevice.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/utils/System.h"
#include "xcept/Exception.h"

#include "TMath.h"

#include <iomanip>

const string AFEB::teststand::AnalyzedDevice::analyzedDeviceNamespace_( "http://cms.cern.ch/emu/afeb/teststand/analyzeddevice" );
const double AFEB::teststand::AnalyzedDevice::nominalInputCharges_[]  = { 50., 100., 150. };
const double AFEB::teststand::AnalyzedDevice::nominalInputChargeRangeStart_ =  50.;
const double AFEB::teststand::AnalyzedDevice::nominalInputChargeRangeEnd_   = 580.;

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
    if ( (*m)->getType() == Measurement::count_vs_dac && (*m)->getInjectionCapacitor() == Measurement::external ){      
      // Open the root file of this measurement's results...
      string fileName = Results::getFileName( (*m)->getIndex(), (*m)->getTypeString(), id_ );
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
    } // if ( (*m)->getType() == Measurement::count_vs_dac && (*m)->getInjectionCapacitor() == Measurement::external )
  } // for ( vector<Measurement*>::const_iterator m = measurements.begin(); m != measurements.end(); ++m ){
  
  // Now we're ready to calculate the gains
  for ( vector<AnalyzedChannel>::iterator c = channels_.begin(); c != channels_.end(); ++c ){
    c->calculateGain();
  }
}

void AFEB::teststand::AnalyzedDevice::calculateInternalCapacitances(){
  // Find the count_vs_dac measurements with charge injection through internal capacitors
  for ( vector<Measurement*>::const_iterator m = measurements_.begin(); m != measurements_.end(); ++m ){
    if ( (*m)->getType() == Measurement::count_vs_dac && (*m)->getInjectionCapacitor() == Measurement::internal ){      
      // Open the root file of this measurement's results...
      string fileName = Results::getFileName( (*m)->getIndex(), (*m)->getTypeString(), id_ );
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
    } // if ( (*m)->getType() == Measurement::count_vs_dac && (*m)->getInjectionCapacitor() == Measurement::internal )
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
    ss << "  <ad:measurement index=\""                     << noshowpos 
       <<                                                     (*m)->getIndex()
       <<                  "\" type=\""                    << (*m)->getTypeString()
       <<                  "\" capacitor=\""               << (*m)->getInjectionCapacitorString()
       <<                  "\" nPulses=\""                 << (*m)->getNPulses()
       <<                  "\" setThreshold.ADC=\""        << (*m)->getSetThreshold()
       <<                  "\" setThresholdVoltage.mV=\""  << thresholdDAC_->mV_from_DACUnit( (*m)->getSetThreshold() ).first
       <<   "\">" << endl;

    // Open the root file of this measurement's results...
    string fileName = Results::getFileName( (*m)->getIndex(), (*m)->getTypeString(), id_ );
    TFile f( ( rawResultsDir_ + "/" + fileName +".root").c_str(), "READ" );
    if ( f.IsZombie() ){
      stringstream ss;
      ss << "Failed to open ROOT file " << f.GetName();
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }
    cout << "Opened " << f.GetName() << endl;
    f.cd();

    if ( (*m)->getType() == Measurement::count_vs_dac ){

      // Get the threshold histogram...
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
	ss << "    <ad:channel number=\""       << noshowpos << setw(2) << iChannel
	   <<              "\" threshold.fC=\"" << showpos << showpoint << setw(8) << setprecision(4) << fixed
	   <<                                   chargeFromVoltage( pulseDAC_->mV_from_DACUnit( thresholdHistogram->GetBinContent( iChannel+1 ) ).first )
	   <<              "\" noise.fC=\""
	   <<                                   chargeFromVoltage( pulseDAC_->mV_from_DACUnit( noiseHistogram    ->GetBinContent( iChannel+1 ) ).first )
	   <<              "\" chi2ndf=\""
	   <<                                                                                  chi2ndfHistogram  ->GetBinContent( iChannel+1 )
	   <<     "\"/>" << endl;
      } // for ( int iChannel=0; iChannel<d->getNChannels(); ++iChannel )
      
    } // if ( (*m)->getType() == Measurement::count_vs_dac )

    if ( (*m)->getType() == Measurement::time_vs_dac ){

      // Find the bins corresponding to the charges at which the mean and RMS of time are to be reported. 
      // Do it for the first channel only; they're the same for the other channels.
      string histogramName = string( "timeVsAmpl__" ) + fileName + "__ch_0";
      TProfile *timesHistogram;
      f.GetObject( histogramName.c_str(), timesHistogram );
      // Profile hist needs projecting:
      TH1D *timesVsADC = timesHistogram->ProjectionX( "tp", "e" ); // Keep the original errors, which are actually the RMS in this case.
      TH1D *timesVsCharge = new TH1D( "timesVsCharge", "timesVsCharge", timesVsADC->GetNbinsX(), 
				      chargeFromVoltage( pulseDAC_->mV_from_DACUnit( timesVsADC->GetBinLowEdge(                          1 ) ).first ),
				      chargeFromVoltage( pulseDAC_->mV_from_DACUnit( timesVsADC->GetBinLowEdge ( timesVsADC->GetNbinsX()+1 ) ).first )
				      );

      // Find the bins which the nominal input charge values would fall into, and get the charges corresponding the center of those bins:
      vector<size_t> bins;                 // bins[iCharge]
      vector<double> realCharges;          // realCharges[iCharge]
      vector<valarray<double> > meanTimes; // meanTimes[iCharge][iChannel]
      vector<valarray<double> > rmsTimes;  // rmsTimes[iCharge][iChannel]
      for ( size_t iCharge=0; iCharge<sizeof(nominalInputCharges_)/sizeof(double); ++iCharge ){
	bins.push_back( timesVsCharge->FindBin( nominalInputCharges_[iCharge] ) );
	realCharges.push_back( chargeFromVoltage( pulseDAC_->mV_from_DACUnit( timesVsADC->GetBinCenter( bins.back() ) ).first ) );
	meanTimes.push_back( valarray<double>( nChannels_ ) ); // prepare container
	rmsTimes .push_back( valarray<double>( nChannels_ ) ); // prepare container
      }
      // Find the bins for the ends of range for slew calculation:
      int binInputChargeRangeStart = max(                          1, timesVsCharge->FindBin( nominalInputChargeRangeStart_ ) );
      int binInputChargeRangeEnd   = min( timesVsCharge->GetNbinsX(), timesVsCharge->FindBin( nominalInputChargeRangeEnd_   ) );
      double inputChargeRangeStart = chargeFromVoltage( pulseDAC_->mV_from_DACUnit( timesVsADC->GetBinCenter( binInputChargeRangeStart ) ).first );
      double inputChargeRangeEnd   = chargeFromVoltage( pulseDAC_->mV_from_DACUnit( timesVsADC->GetBinCenter( binInputChargeRangeEnd   ) ).first );
      valarray<double> meanSpan( nChannels_ );
      valarray<double> rmsSpan ( nChannels_ );

      delete timesVsADC;
      delete timesVsCharge;

      // Loop over the channels and get the mean and rms of the times at the specified input charges,
      // and the min and max of mean and rms of times in the specified input charge range
      for ( int iChannel=0; iChannel<nChannels_; ++iChannel ){
	// Get the times histogram...
	histogramName = string( "timeVsAmpl__" ) + fileName + "__ch_" + utils::stringFrom<int>( iChannel );
	f.GetObject( histogramName.c_str(), timesHistogram );
	// Profile hist needs projecting:
	TH1D *timesVsADC = timesHistogram->ProjectionX( "tp", "e" ); // Keep the original errors, which are actually the RMS in this case.
	// Get the mean and rms of times at the specified input charges:
	for ( size_t iCharge=0; iCharge<sizeof(nominalInputCharges_)/sizeof(double); ++iCharge ){
	  meanTimes[iCharge][iChannel] = timesVsADC->GetBinContent( bins[iCharge] );
	  rmsTimes [iCharge][iChannel] = timesVsADC->GetBinError  ( bins[iCharge] );
	}
	// Get the min and max of mean and rms of times in the specified input charge range: 
	double minMean = 999999., maxMean = -999999.;
	double minRMS  = 999999., maxRMS  = -999999.;
	for ( int iBin = binInputChargeRangeStart; iBin <= binInputChargeRangeEnd; ++iBin ){
	  if ( timesVsADC->GetBinContent( iBin ) < minMean ) minMean = timesVsADC->GetBinContent( iBin );
	  if ( timesVsADC->GetBinContent( iBin ) > maxMean ) maxMean = timesVsADC->GetBinContent( iBin );
	  if ( timesVsADC->GetBinError  ( iBin ) < minRMS  ) minRMS  = timesVsADC->GetBinError  ( iBin );
	  if ( timesVsADC->GetBinError  ( iBin ) > maxRMS  ) maxRMS  = timesVsADC->GetBinError  ( iBin );
	}
	meanSpan[iChannel] = maxMean - minMean;
	rmsSpan [iChannel] = maxRMS  - minRMS;
	delete timesVsADC;
      } // for ( int iChannel=0; iChannel<nChannels_; ++iChannel )

      // cout << "meanTimes" << endl << meanTimes << endl;
      // cout << "rmsTimes" << endl << rmsTimes << endl;

      // Now write the values to XML;
      for ( size_t iCharge=0; iCharge<sizeof(nominalInputCharges_)/sizeof(double); ++iCharge ){
	ss << "    <ad:times nominalInputCharge.fC=\"" << showpos << showpoint << setw(8) << setprecision(4) << fixed << nominalInputCharges_[iCharge]
	   <<            "\" realInputCharge.fC=\""    << showpos << showpoint << setw(8) << setprecision(4) << fixed << realCharges[iCharge]
	   <<    "\">" << endl;
	for ( int iChannel=0; iChannel<nChannels_; ++iChannel ){
	  ss << "      <ad:channel number=\""  << noshowpos << setw(2) << iChannel
	     <<                "\" mean.ns=\"" << showpos << showpoint << setw(8) << setprecision(4) << fixed << meanTimes[iCharge][iChannel]
	     <<                "\" rms.ns=\""  << showpos << showpoint << setw(8) << setprecision(4) << fixed << rmsTimes [iCharge][iChannel]
	     <<       "\"/>" << endl;
	}
	ss << "      " << statisticsToXML( "mean.ns", meanTimes[iCharge] ) << endl;
	ss << "      " << statisticsToXML( "rms.ns" , rmsTimes[iCharge]  ) << endl;
	ss << "    </ad:times>" << endl;
      } //

      ss << "    <ad:slew fromInputCharge.fC=\"" << showpos << showpoint << setw(8) << setprecision(4) << fixed << inputChargeRangeStart
	 <<           "\" toInputCharge.fC=\""   << showpos << showpoint << setw(8) << setprecision(4) << fixed << inputChargeRangeEnd
	 <<    "\">" << endl;
      for ( int iChannel=0; iChannel<nChannels_; ++iChannel ){
	ss << "      <ad:channel number=\""          << noshowpos << setw(2) << iChannel
	   <<                "\" spanOfMeans.ns=\""  << showpos << showpoint << setw(8) << setprecision(4) << fixed << meanSpan[iChannel]
	   <<                "\" spanOfRMSs.ns=\""   << showpos << showpoint << setw(8) << setprecision(4) << fixed << rmsSpan[iChannel]
	   <<       "\"/>" << endl;
      }
      ss << "      " << statisticsToXML( "spanOfMeans.ns", meanSpan ) << endl;
      ss << "      " << statisticsToXML( "spanOfRMSs.ns" , rmsSpan  ) << endl;
      ss << "    </ad:slew>" << endl;


    } // if ( (*m)->getType() == Measurement::time_vs_dac )

    f.Close();
    ss << "  </ad:measurement>" << endl;

  } // for ( vector<Measurement*>::const_iterator m = measurements_.begin(); m != measurements_.end(); ++m )
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
    ss << "  <ad:channel number=\""          << noshowpos << noshowpoint << setw(2) << iChannel++
       <<            "\" offset.mV=\""       << showpos << showpoint << setw(8) << setprecision(4) << fixed << c->offset_
       <<            "\" gain.mV.fC-1=\""    << showpos << showpoint << setw(8) << setprecision(4) << fixed << c->gain_
       <<            "\" C_int.pF=\""        << showpos << showpoint << setw(8) << setprecision(4) << fixed << c->internalCapacitance_
       <<            "\" averageNoise.fC=\"" << showpos << showpoint << setw(8) << setprecision(4) << fixed << c->noise_
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
