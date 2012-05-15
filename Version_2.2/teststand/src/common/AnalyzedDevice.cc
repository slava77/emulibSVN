#include "AFEB/teststand/AnalyzedDevice.h"
#include "AFEB/teststand/Legend.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/utils/System.h"
#include "AFEB/teststand/utils/DOM.h"
#include "xcept/Exception.h"

#include "TMath.h"
#include "TCanvas.h"
#include "TStyle.h"

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

double AFEB::teststand::AnalyzedDevice::chargeFromVoltage( const double voltage, Measurement::Capacitor_t capacitorType ) const {
  if ( capacitorType == Measurement::external ) return injectionCapacitance_ * voltage / pulseDivisionFactor_ / correctionCoefficient_; 
  if ( capacitorType == Measurement::internal ) return nominalInternalCapacitance_ * voltage;
  return 0.;
}

pair<double,double> AFEB::teststand::AnalyzedDevice::chargeFromVoltage( const pair<double,double> voltage, Measurement::Capacitor_t capacitorType ) const {
  return make_pair( AnalyzedDevice::chargeFromVoltage( voltage.first , capacitorType ),
		    AnalyzedDevice::chargeFromVoltage( voltage.second, capacitorType ) );
}


void AFEB::teststand::AnalyzedDevice::addThresholdMeasurement( const int iChannel, 
							       const pair<double,double> V_setThreshold ,
							       const pair<double,double> Q_measuredThreshold,
							       const pair<double,double> Q_measuredNoise ){
  // Fit Q(V) instead of V(Q) to better estimate the error of the fit parameters. (The error is on Q rather than on V.)

  TMatrixD    x  ( 1, 1 );
  TMatrixD    y  ( 1, 1 );
  TMatrixDSym var( 1 );

  // Convert measured threshold voltage to injected charge [fC]. Take into account the correction coefficient as well:
  x  ( 0, 0 ) = V_setThreshold.first;
  y  ( 0, 0 ) = Q_measuredThreshold.first;
  var( 0, 0 ) = TMath::Power( Q_measuredThreshold.second, 2 );
  channels_[iChannel].QofVfitter_.addObservation( x, y, var );

  // Add the noise measurements as well to average them while we're at it:
  y  ( 0, 0 ) = Q_measuredNoise.first;
  var( 0, 0 ) = TMath::Power( Q_measuredNoise.second, 2 );
  channels_[iChannel].noiseAverager_.addObservation( x, y, var );

  // cout << channels_[iChannel].noiseAverager_.getObservationCount() << " Added ( " << x(0,0) << ", " << y(0,0) << "+-" << TMath::Sqrt( var(0,0) ) << " )" << endl;
}

void AFEB::teststand::AnalyzedDevice::calculateGains(){
  // Loop over the measurements
  for ( vector<Measurement*>::const_iterator m = measurements_.begin(); m != measurements_.end(); ++m ){
    // Pick the count_vs_dac measurements with pulses through external capacitors into this device:
    if ( (*m)->getTestedDevice( id_ ) && (*m)->getType() == Measurement::count_vs_dac && (*m)->getInjectionCapacitor() == Measurement::external ){      
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
				 chargeFromVoltage( pulseDAC_->mV_from_DACUnit( thresholdHistogram->GetBinContent( iChannel+1 ),
										thresholdHistogram->GetBinError  ( iChannel+1 )  ),
						    (*m)->getInjectionCapacitor() ),
				 chargeFromVoltage( pulseDAC_->mV_from_DACUnit( noiseHistogram    ->GetBinContent( iChannel+1 ),
										noiseHistogram    ->GetBinError  ( iChannel+1 )  ),
						    (*m)->getInjectionCapacitor() )
				 );
      } // for ( int iChannel=0; iChannel<nChannels; ++iChannel ){
      
      f.Close();
    } // if ( (*m)->getTestedDevice( id_ ) && (*m)->getType() == Measurement::count_vs_dac && (*m)->getInjectionCapacitor() == Measurement::external )
  } // for ( vector<Measurement*>::const_iterator m = measurements.begin(); m != measurements.end(); ++m ){
  
  // Now we're ready to calculate the gains
  for ( vector<AnalyzedChannel>::iterator c = channels_.begin(); c != channels_.end(); ++c ){
    c->calculateGain();
  }
}

void AFEB::teststand::AnalyzedDevice::calculateInternalCapacitances(){
  // Loop over measurements:
  for ( vector<Measurement*>::const_iterator m = measurements_.begin(); m != measurements_.end(); ++m ){
    // Pick the count_vs_dac measurements with pulses through external internal into this device:
    if ( (*m)->getTestedDevice( id_ ) && (*m)->getType() == Measurement::count_vs_dac && (*m)->getInjectionCapacitor() == Measurement::internal ){      
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
    } // if ( (*m)->getTestedDevice( id_ ) && (*m)->getType() == Measurement::count_vs_dac && (*m)->getInjectionCapacitor() == Measurement::internal )
  } // for ( vector<Measurement*>::const_iterator m = measurements.begin(); m != measurements.end(); ++m )
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
  for ( vector<AnalyzedChannel>::const_iterator c = channels_.begin(); c != channels_.end(); ++c ) v[n++] = - c->offset_; // Comply with odd convention on sign of offset.
  return v;
}

valarray<double> AFEB::teststand::AnalyzedDevice::getInternalCapacitances() const {
  valarray<double> v( channels_.size() );
  size_t n = 0;
  for ( vector<AnalyzedChannel>::const_iterator c = channels_.begin(); c != channels_.end(); ++c ) v[n++] = c->internalCapacitance_;
  return v;
}

TH1D AFEB::teststand::AnalyzedDevice::histogramContents( const TH1D* h ) const {
  TH1D hh( "hh", h->GetTitle(), 100, h->GetMinimum(), h->GetMaximum() );
  for ( int iBin=1; iBin<=h->GetNbinsX(); ++iBin ){
    hh.Fill( h->GetBinContent( iBin ) );
  }
  hh.SetFillStyle( 1001 );
  hh.SetFillColor( kBlue );
  return hh;
}

string AFEB::teststand::AnalyzedDevice::measurementsToXMLAndPlots( const string& analyzedResultsDir, TPDF& pdf ){
  stringstream ss;
  pdf.NewPage();
  // Loop over measurements:
  for ( vector<Measurement*>::const_iterator m = measurements_.begin(); m != measurements_.end(); ++m ){
    // Pick the ones involving this device:
    if ( (*m)->getTestedDevice( id_ ) ){

    ss.str() = "";
    ss << "  <ad:measurement index=\""                  << noshowpos 
       <<                                                  (*m)->getIndex()
       <<                  "\" type=\""                 << (*m)->getTypeString()
       <<                  "\" capacitor=\""            << (*m)->getInjectionCapacitorString()
       <<                  "\" nPulses=\""              << (*m)->getNPulses()
       <<                  "\" setThreshold=\""         << (*m)->getSetThreshold()
       <<                  "\" setThresholdVoltage=\""  << showpoint << setprecision(4) << fixed << thresholdDAC_->mV_from_DACUnit( (*m)->getSetThreshold() ).first
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
      TH1D *thresholdVsChannel;
      f.GetObject( histogramName.c_str(), thresholdVsChannel );      
      for ( int iBin=1; iBin<=thresholdVsChannel->GetNbinsX(); ++iBin ){
	 // Transform the contents from DAC units to charge
	thresholdVsChannel->SetBinContent( iBin, chargeFromVoltage( pulseDAC_->mV_from_DACUnit( thresholdVsChannel->GetBinContent( iBin ) ).first,
								    (*m)->getInjectionCapacitor() ) );
	thresholdVsChannel->SetBinError( iBin, chargeFromVoltage( pulseDAC_->mV_from_DACUnit( thresholdVsChannel->GetBinError( iBin ) ).first,
								  (*m)->getInjectionCapacitor() ) ); // Actually, the error should only be multuplied by the first derivative of the transformation, but the difference is very small if any.
      }

      // ...and the noise histogram...
      histogramName = string( "noise__" ) + fileName;
      TH1D *noiseVsChannel;
      f.GetObject( histogramName.c_str(), noiseVsChannel );
      for ( int iBin=1; iBin<=noiseVsChannel->GetNbinsX(); ++iBin ){
	 // Transform the contents from DAC units to charge
	noiseVsChannel->SetBinContent( iBin, chargeFromVoltage( pulseDAC_->mV_from_DACUnit( noiseVsChannel->GetBinContent( iBin ) ).first,
								    (*m)->getInjectionCapacitor() ) );
	noiseVsChannel->SetBinError( iBin, chargeFromVoltage( pulseDAC_->mV_from_DACUnit( noiseVsChannel->GetBinError( iBin ) ).first,
								  (*m)->getInjectionCapacitor() ) ); // Actually, the error should only be multuplied by the first derivative of the transformation, but the difference is very small if any.
      }

      // ...and the chi^2/ndf histogram:
      histogramName = string( "chi2ndf__" ) + fileName;
      TH1D *chi2ndfVsChannel;
      f.GetObject( histogramName.c_str(), chi2ndfVsChannel );
      
      TCanvas canvas("canvas","canvas",1000,1400);
      canvas.Divide( 1, 4 );
      Legend legend;
      int nDead = 0;
      int nHot  = 0;
      // Valarrays for calculating the statistics
      valarray<double> threshold( nChannels_ );
      valarray<double> noise    ( nChannels_ );
      valarray<double> chi2ndf  ( nChannels_ );
      // Loop over the channels
      for ( int iChannel=0; iChannel<nChannels_; ++iChannel ){
	// The plot
	canvas.cd( 1 );
	TH1D *efficiencyHistogram;
	histogramName = string( "effVsAmpl__" ) + fileName + "__ch_" + utils::stringFrom<int>( iChannel );
	f.GetObject( histogramName.c_str(), efficiencyHistogram );
	// Copy it and transform the X axis from DAC units to charge
	TH1D effVsCharge( *efficiencyHistogram );
	effVsCharge.GetXaxis()->Set( efficiencyHistogram->GetXaxis()->GetNbins(),
				     chargeFromVoltage( pulseDAC_->mV_from_DACUnit( efficiencyHistogram->GetXaxis()->GetXmin() ).first, 
							(*m)->getInjectionCapacitor() ),
				     chargeFromVoltage( pulseDAC_->mV_from_DACUnit( efficiencyHistogram->GetXaxis()->GetXmax() ).first, 
							(*m)->getInjectionCapacitor() ) );
	effVsCharge.SetTitle( string( "Threshold curves at U=" + utils::stringFrom<double>( thresholdDAC_->mV_from_DACUnit( (*m)->getSetThreshold() ).first ) + " mV" ).c_str() );
	effVsCharge.SetXTitle( string( "input charge [fC] to " + (*m)->getInjectionCapacitorString() + " capacitor" ).c_str() );
	effVsCharge.SetYTitle( "count" );
	effVsCharge.SetLineColor( legend.getColor( iChannel ) );
	effVsCharge.SetLineStyle( legend.getStyle( iChannel ) );
	legend.AddEntry( efficiencyHistogram,  utils::stringFrom<int>( iChannel ).c_str(), "l" ); // Use efficiencyHistogram as effVsCharge will be already out of scope when legend is drawn.
	effVsCharge.DrawCopy( (iChannel==0?"l":"lsame") );

	// The XML
	int maxCount = TMath::Nint( efficiencyHistogram->GetBinContent( efficiencyHistogram->GetMaximumBin() ) );
	ss << "    <ad:channel number=\""    << noshowpos << setw(2) << iChannel
	   <<              "\" threshold=\"" << showpos << showpoint << setw(8) << setprecision(4) << fixed
	   <<                                thresholdVsChannel->GetBinContent( iChannel+1 )
	   <<              "\" noise=\""
	   <<                                noiseVsChannel    ->GetBinContent( iChannel+1 )
	   <<              "\" chi2ndf=\""
	   <<                                chi2ndfVsChannel  ->GetBinContent( iChannel+1 )
	   <<              "\" maxCount=\""  << noshowpos << noshowpoint << setw(3)
	   <<                                maxCount
	   <<     "\"/>" << endl;
	threshold[ iChannel ] = thresholdVsChannel->GetBinContent( iChannel+1 );
	noise    [ iChannel ] = noiseVsChannel    ->GetBinContent( iChannel+1 );
	chi2ndf  [ iChannel ] = chi2ndfVsChannel  ->GetBinContent( iChannel+1 );
	if ( maxCount == 0 ) nDead++;
	if ( maxCount > (*m)->getNPulses() ) nHot++;
      } // for ( int iChannel=0; iChannel<d->getNChannels(); ++iChannel )

      // Channels' statistics to XML
      map<string,double> stat = utils::statistics( threshold );
      ss << "    <ad:threshold ";
      for ( map<string,double>::const_iterator s=stat.begin(); s!=stat.end(); ++s ) ss << " " << s->first << "=\"" << noshowpos << showpoint << setprecision(4) << s->second << "\"";
      ss << "/>" << endl;
      stat = utils::statistics( noise );
      ss << "    <ad:noise ";
      for ( map<string,double>::const_iterator s=stat.begin(); s!=stat.end(); ++s ) ss << " " << s->first << "=\"" << noshowpos << showpoint << setprecision(4) << s->second << "\"";
      ss << "/>" << endl;
      stat = utils::statistics( chi2ndf );
      ss << "    <ad:chi2ndf ";
      for ( map<string,double>::const_iterator s=stat.begin(); s!=stat.end(); ++s ) ss << " " << s->first << "=\"" << noshowpos << showpoint << setprecision(4) << s->second << "\"";
      ss << "/>" << endl;
      ss << "    <ad:channels dead=\"" << nDead << "\" hot=\"" << nHot << "\"/>" << endl;

      // Plots
      canvas.cd( 1 );
      legend.Draw();

      canvas.cd( 2 )->Divide( 2, 1 );
      canvas.cd( 2 )->cd( 1 );
      thresholdVsChannel->SetMinimum( 0. );
      thresholdVsChannel->SetMaximum( 1.5 * thresholdVsChannel->GetBinContent( thresholdVsChannel->GetMaximumBin() ) );
      thresholdVsChannel->SetMarkerColor( kBlue );
      thresholdVsChannel->DrawCopy( "pe" );
      canvas.cd( 2 )->cd( 2 );
      gStyle->SetOptStat( 111110 );
      TH1D hc = histogramContents( thresholdVsChannel );
      hc.SetTitle( "Threshold" );
      hc.SetXTitle( "threshold [fC]" );
      hc.SetYTitle( "number of channels" );
      hc.DrawCopy();

      canvas.cd( 3 )->Divide( 2, 1 );
      canvas.cd( 3 )->cd( 1 );
      noiseVsChannel->SetMinimum( 0. );
      noiseVsChannel->SetMaximum( 1.5 * noiseVsChannel->GetBinContent( noiseVsChannel->GetMaximumBin() ) );
      noiseVsChannel->SetMarkerColor( kBlue );
      noiseVsChannel->DrawCopy( "pe" );
      canvas.cd( 3 )->cd( 2 );
      hc = histogramContents( noiseVsChannel );
      hc.SetTitle( "Noise" );
      hc.SetXTitle( "noise [fC]" );
      hc.SetYTitle( "number of channels" );
      hc.DrawCopy();

      canvas.cd( 4 )->Divide( 2, 1 );
      canvas.cd( 4 )->cd( 1 );
      chi2ndfVsChannel->SetMinimum( 0. );
      chi2ndfVsChannel->SetMaximum( 1.5 * chi2ndfVsChannel->GetBinContent( chi2ndfVsChannel->GetMaximumBin() ) );
      chi2ndfVsChannel->SetYTitle( "#chi^{2} / ndf" );
      chi2ndfVsChannel->SetMarkerColor( kBlue );
      chi2ndfVsChannel->DrawCopy( "p" );
      canvas.cd( 4 )->cd( 2 );
      hc = histogramContents( chi2ndfVsChannel );
      hc.SetTitle( "#chi^{2} / ndf" );
      hc.SetXTitle( "#chi^{2} / ndf" );
      hc.SetYTitle( "number of channels" );
      hc.DrawCopy();

      canvas.cd();
      canvas.Draw();

      pdf.NewPage();

    } // if ( (*m)->getType() == Measurement::count_vs_dac )

    if ( (*m)->getType() == Measurement::time_vs_dac ){

      TCanvas meanCanvas("meanCanvas","meanCanvas",1000,1400);
      TCanvas rmsCanvas ("rmsCanvas" ,"rmsCanvas" ,1000,1400);
      meanCanvas.Divide( 1, 5 );
      rmsCanvas .Divide( 1, 4 );
      Legend legend;

      // Some containers for finding the bins which the nominal input charge values would fall into, and getting the charges corresponding the center of those bins:
      vector<size_t> bins;                 // bins[iCharge]
      vector<double> realCharges;          // realCharges[iCharge]
      vector<valarray<double> > meanTimes; // meanTimes[iCharge][iChannel]
      vector<valarray<double> > rmsTimes;  // rmsTimes[iCharge][iChannel]
      // For finding the bins for the ends of range for slew calculation:
      int binInputChargeRangeStart = 0;
      int binInputChargeRangeEnd = 0;
      double inputChargeRangeStart = 0;
      double inputChargeRangeEnd = 0;
      valarray<double> meanSpan( nChannels_ );
      valarray<double> rmsSpan ( nChannels_ );

      //
      // Loop over the channels and get the mean and rms of the times at the specified input charges,
      // and the min and max of mean and rms of times in the specified input charge range
      //
      TH1D *meanTimeVsCharge = NULL;
      TH1D  *rmsTimeVsCharge = NULL;
      for ( int iChannel=0; iChannel<nChannels_; ++iChannel ){
	// Get the times histogram.
	string histogramName = string( "timeVsAmpl__" ) + fileName + "__ch_" + utils::stringFrom<int>( iChannel );
	TProfile *timesHistogram = NULL;
	f.GetObject( histogramName.c_str(), timesHistogram );
	// Profile hists need projecting:
	TH1D *meanTDCVsDAC = timesHistogram->ProjectionX( "tp", ""    ); // Keep the original errors, which are actually the RMS in this case.
	TH1D *rmsTDCVsDAC  = timesHistogram->ProjectionX( "tp", "c=e" ); // Contents will be the errors, which are actually the RMS in this case.
	// Copy and transform them:
	meanTimeVsCharge = new TH1D( *meanTDCVsDAC );
	rmsTimeVsCharge =  new TH1D( * rmsTDCVsDAC );
	meanTimeVsCharge->SetStats( kFALSE );
	rmsTimeVsCharge ->SetStats( kFALSE );
	// Transform their horizontal, pulse amplitude axis:
	meanTimeVsCharge->GetXaxis()->Set( meanTDCVsDAC->GetXaxis()->GetNbins(),
					  chargeFromVoltage( pulseDAC_->mV_from_DACUnit( meanTDCVsDAC->GetXaxis()->GetXmin() ).first, (*m)->getInjectionCapacitor() ),
					  chargeFromVoltage( pulseDAC_->mV_from_DACUnit( meanTDCVsDAC->GetXaxis()->GetXmax() ).first, (*m)->getInjectionCapacitor() ) );
	rmsTimeVsCharge->GetXaxis()->Set( rmsTDCVsDAC->GetXaxis()->GetNbins(),
					 chargeFromVoltage( pulseDAC_->mV_from_DACUnit( rmsTDCVsDAC->GetXaxis()->GetXmin() ).first, (*m)->getInjectionCapacitor() ),
					 chargeFromVoltage( pulseDAC_->mV_from_DACUnit( rmsTDCVsDAC->GetXaxis()->GetXmax() ).first, (*m)->getInjectionCapacitor() ) );
	// Transform their vertical, time axis:	
	for ( int iBin=1; iBin<=meanTDCVsDAC->GetNbinsX(); ++iBin ){
	  // Transform the contents from TDC units to time
	  meanTimeVsCharge->SetBinContent( iBin, (*m)->nanosecondsFromTDCUnits( meanTDCVsDAC->GetBinContent( iBin ) ) );
	  rmsTimeVsCharge-> SetBinContent( iBin, (*m)->nanosecondsFromTDCUnits(  rmsTDCVsDAC->GetBinContent( iBin ) ) );
	}


	meanCanvas.cd( 1 );
	gStyle->SetOptStat( 0 );
	meanTimeVsCharge->SetTitle( ("Mean time vs. input charge for "+type_+" of id "+id_).c_str() );
	meanTimeVsCharge->SetXTitle( "input charge [fC]" );
	meanTimeVsCharge->SetYTitle( "mean time [ns]" );
	meanTimeVsCharge->SetLineColor( legend.getColor( iChannel ) );
	meanTimeVsCharge->SetLineStyle( legend.getStyle( iChannel ) );
	meanTimeVsCharge->DrawCopy( (iChannel==0?"l":"lsame") );
	legend.AddEntry( meanTimeVsCharge, utils::stringFrom<int>( iChannel ).c_str(), "l" );
	rmsCanvas.cd( 1 );
	gStyle->SetOptStat( 0 );
	rmsTimeVsCharge->SetTitle( ("Time resolution vs. input charge for "+type_+" of id "+id_).c_str() );
	rmsTimeVsCharge->SetXTitle( "input charge [fC]" );
	rmsTimeVsCharge->SetYTitle( "time resolution [ns]" );
	rmsTimeVsCharge->SetLineColor( legend.getColor( iChannel ) );
	rmsTimeVsCharge->SetLineStyle( legend.getStyle( iChannel ) );
	rmsTimeVsCharge->DrawCopy( (iChannel==0?"l":"lsame") );

	// Find the bins corresponding to the charges at which the mean and RMS of time are to be reported. 
	// Do it for the first channel only; they're the same for the other channels.
	if ( iChannel == 0 ){
	  for ( size_t iCharge=0; iCharge<sizeof(nominalInputCharges_)/sizeof(double); ++iCharge ){
	    cout << "iCharge " << iCharge << "    bin " << meanTimeVsCharge->FindBin( nominalInputCharges_[iCharge] ) << endl;
	    bins.push_back( meanTimeVsCharge->FindBin( nominalInputCharges_[iCharge] ) );
	    realCharges.push_back( meanTimeVsCharge->GetBinCenter( bins.back() ) );
	    meanTimes.push_back( valarray<double>( nChannels_ ) ); // prepare container
	    rmsTimes .push_back( valarray<double>( nChannels_ ) ); // prepare container
	  }
	  // Find the bins for the ends of range for slew calculation:
	  binInputChargeRangeStart = max(                             1, meanTimeVsCharge->FindBin( nominalInputChargeRangeStart_ ) );
	  binInputChargeRangeEnd   = min( meanTimeVsCharge->GetNbinsX(), meanTimeVsCharge->FindBin( nominalInputChargeRangeEnd_   ) );
	  inputChargeRangeStart = meanTimeVsCharge->GetBinCenter( binInputChargeRangeStart );
	  inputChargeRangeEnd   = meanTimeVsCharge->GetBinCenter( binInputChargeRangeEnd   );
	  cout << "binInputChargeRangeStart " << binInputChargeRangeStart << "  binInputChargeRangeEnd " << binInputChargeRangeEnd << "  inputChargeRangeStart " << inputChargeRangeStart << "  inputChargeRangeEnd " << inputChargeRangeEnd << endl;
	}

	// Get the mean and rms of times at the specified input charges:
	for ( size_t iCharge=0; iCharge<sizeof(nominalInputCharges_)/sizeof(double); ++iCharge ){
	  meanTimes[iCharge][iChannel] = meanTimeVsCharge->GetBinContent( bins[iCharge] );
	  rmsTimes [iCharge][iChannel] = rmsTimeVsCharge-> GetBinContent( bins[iCharge] );
	}
	// Get the min and max of mean and rms of times in the specified input charge range: 
	double minMean = 999999., maxMean = -999999.;
	double minRMS  = 999999., maxRMS  = -999999.;
	for ( int iBin = binInputChargeRangeStart; iBin <= binInputChargeRangeEnd; ++iBin ){
	  if ( meanTimeVsCharge->GetBinContent( iBin ) < minMean ) minMean = meanTimeVsCharge->GetBinContent( iBin );
	  if ( meanTimeVsCharge->GetBinContent( iBin ) > maxMean ) maxMean = meanTimeVsCharge->GetBinContent( iBin );
	  if ( rmsTimeVsCharge-> GetBinContent( iBin ) < minRMS  ) minRMS  = rmsTimeVsCharge-> GetBinContent( iBin );
	  if ( rmsTimeVsCharge-> GetBinContent( iBin ) > maxRMS  ) maxRMS  = rmsTimeVsCharge-> GetBinContent( iBin );
	}
	meanSpan[iChannel] = maxMean - minMean;
	rmsSpan [iChannel] = maxRMS  - minRMS ;
	delete meanTDCVsDAC;
	delete rmsTDCVsDAC;
      } // for ( int iChannel=0; iChannel<nChannels_; ++iChannel )

      // Plot mean and rms of times for different input charges
      size_t nCharges = sizeof(nominalInputCharges_)/sizeof(double);
      for ( size_t iCharge=0; iCharge<nCharges; ++iCharge ){
	meanCanvas.cd( 2+iCharge )->Divide( 2, 1 );
	rmsCanvas .cd( 2+iCharge )->Divide( 2, 1 );
	TH1D meanTimeVsChannel( "meanTimeVsChannel", 
				("Mean propagation time vs. channel at " + utils::stringFrom<double>( nominalInputCharges_[iCharge] ) + " fC input charge").c_str(),
				nChannels_, 0., 0. + nChannels_ );
	meanTimeVsChannel.GetXaxis()->CenterLabels( kTRUE );
	meanTimeVsChannel.GetXaxis()->SetNdivisions( nChannels_ );
	meanTimeVsChannel.SetYTitle( "mean propagation time [ns]" );
	meanTimeVsChannel.SetMinimum( 20. );
	meanTimeVsChannel.SetMaximum( 120. );
	meanTimeVsChannel.SetStats( kFALSE );
	meanTimeVsChannel.SetMarkerStyle( kOpenCircle );
	meanTimeVsChannel.SetMarkerColor( kBlue );
	TH1D rmsTimeVsChannel( meanTimeVsChannel );
	rmsTimeVsChannel.SetTitle( ( "RMS of propagation time vs. channel at " + utils::stringFrom<double>( nominalInputCharges_[iCharge] ) + " fC input charge").c_str() );
	rmsTimeVsChannel.SetYTitle( "rms of propagation time [ns]" );
	rmsTimeVsChannel.SetMinimum( 0. );
	rmsTimeVsChannel.SetMaximum( 3. );
	for ( int iChannel=0; iChannel<nChannels_; ++iChannel ){
	  meanTimeVsChannel.SetBinContent( iChannel+1, meanTimes[iCharge][iChannel] );
	  rmsTimeVsChannel .SetBinContent( iChannel+1,  rmsTimes[iCharge][iChannel] );
	}
	meanCanvas.cd( 2+iCharge )->cd( 1 );
	gStyle->SetOptStat( 0 );
	meanTimeVsChannel.DrawCopy( "p" );
	meanCanvas.cd( 2+iCharge )->cd( 2 );
	gStyle->SetOptStat( 111110 );
	TH1D hc = histogramContents( &meanTimeVsChannel );
	hc.SetTitle( "Mean propagation times" );
	hc.SetXTitle( "mean time [ns]" );
	hc.SetYTitle( "number of channels" );
	gStyle->SetOptStat( 111110 );
	hc.DrawCopy();
	// Plot the slewing time, too, after the plots for the last input charge
	if ( iCharge+1 == nCharges ){
	  meanCanvas.cd( 2+iCharge+1 )->Divide( 2, 1 );
	  TH1D slewingTimeVsChannel( meanTimeVsChannel );
	  slewingTimeVsChannel.SetMinimum( 0. );
	  slewingTimeVsChannel.SetMaximum( 5. );
	  slewingTimeVsChannel.SetTitle( ( "Slewing time vs. channel in input charge range [" +  utils::stringFrom<double>( nominalInputChargeRangeStart_ ) + "," +  utils::stringFrom<double>( nominalInputChargeRangeEnd_ ) + "] fC" ).c_str() );
	  for ( int iChannel=0; iChannel<nChannels_; ++iChannel ) slewingTimeVsChannel.SetBinContent( iChannel+1, meanSpan[iChannel] );
	  meanCanvas.cd( 2+iCharge+1 )->cd( 1 );
	  gStyle->SetOptStat( 0 );
	  slewingTimeVsChannel.DrawCopy( "p" );
	  meanCanvas.cd( 2+iCharge+1 )->cd( 2 );
	  gStyle->SetOptStat( 111110 );
	  hc = histogramContents( &slewingTimeVsChannel );
	  hc.SetTitle( "Slewing times" );
	  hc.SetXTitle( "slewing time [ns]" );
	  hc.SetYTitle( "number of channels" );
	  gStyle->SetOptStat( 111110 );
	  hc.DrawCopy();
	}
	rmsCanvas.cd( 2+iCharge )->cd( 1 );
	gStyle->SetOptStat( 0 );
	rmsTimeVsChannel.DrawCopy( "p" );
	rmsCanvas.cd( 2+iCharge )->cd( 2 );
	gStyle->SetOptStat( 111110 );
	hc = histogramContents( &rmsTimeVsChannel );
	hc.SetTitle( "RMS of times" );
	hc.SetXTitle( "rms of time [ns]" );
	hc.SetYTitle( "number of channels" );
	gStyle->SetOptStat( 111110 );
	hc.DrawCopy();
      }

      meanCanvas.cd( 1 );
      legend.Draw();      
      meanCanvas.cd();
      meanCanvas.Draw();
      pdf.NewPage();
      rmsCanvas.cd( 1 );
      legend.Draw();
      rmsCanvas.cd();
      rmsCanvas.Draw();
      pdf.NewPage();
      delete meanTimeVsCharge;
      delete  rmsTimeVsCharge;

      // cout << "meanTimes" << endl << meanTimes << endl;
      // cout << "rmsTimes" << endl << rmsTimes << endl;

      //
      // Now write the values to XML;
      //
      for ( size_t iCharge=0; iCharge<sizeof(nominalInputCharges_)/sizeof(double); ++iCharge ){
	ss << "    <ad:times nominalInputCharge=\"" << showpos << showpoint << setprecision(4) << fixed << nominalInputCharges_[iCharge]
	   <<            "\" realInputCharge=\""    << showpos << showpoint << setprecision(4) << fixed << realCharges[iCharge]
	   <<    "\">" << endl;
	for ( int iChannel=0; iChannel<nChannels_; ++iChannel ){
	  ss << "      <ad:channel number=\""  << noshowpos << setw(2) << iChannel
	     <<                "\" mean=\""    << showpos << showpoint << setw(8) << setprecision(4) << fixed << meanTimes[iCharge][iChannel]
	     <<                "\" rms=\""     << showpos << showpoint << setw(8) << setprecision(4) << fixed << rmsTimes [iCharge][iChannel]
	     <<       "\"/>" << endl;
	}
	ss << "      " << statisticsToXML( "mean", meanTimes[iCharge] ) << endl;
	ss << "      " << statisticsToXML( "rms" , rmsTimes[iCharge]  ) << endl;
	ss << "    </ad:times>" << endl;
      } //

      ss << "    <ad:slew fromInputCharge=\"" << showpos << showpoint << setprecision(4) << fixed << inputChargeRangeStart
	 <<           "\" toInputCharge=\""   << showpos << showpoint << setprecision(4) << fixed << inputChargeRangeEnd
	 <<    "\">" << endl;
      for ( int iChannel=0; iChannel<nChannels_; ++iChannel ){
	ss << "      <ad:channel number=\""       << noshowpos << setw(2) << iChannel
	   <<                "\" spanOfMeans=\""  << showpos << showpoint << setw(8) << setprecision(4) << fixed << meanSpan[iChannel]
	   <<                "\" spanOfRMSs=\""   << showpos << showpoint << setw(8) << setprecision(4) << fixed << rmsSpan[iChannel]
	   <<       "\"/>" << endl;
      }
      ss << "      " << statisticsToXML( "spanOfMeans", meanSpan ) << endl;
      ss << "      " << statisticsToXML( "spanOfRMSs" , rmsSpan  ) << endl;
      ss << "    </ad:slew>" << endl;


    } // if ( (*m)->getType() == Measurement::time_vs_dac )

    f.Close();
    ss << "  </ad:measurement>" << endl;

    } // if ( (*m)->getTestedDevice( id_ ) )
  } // for ( vector<Measurement*>::const_iterator m = measurements_.begin(); m != measurements_.end(); ++m )

  return ss.str();
}

string AFEB::teststand::AnalyzedDevice::statisticsToXML( const string& name, const valarray<double>& values ) const {
  stringstream ss;
  map<string,double> stats = utils::statistics( values );
  ss << "<ad:" << name ;
  for ( map<string,double>::const_iterator s=stats.begin(); s!=stats.end(); ++s ){
    ss << " " << s->first << "=\"" << showpos << showpoint << setprecision(4) << fixed << s->second << "\"";
  }
  ss <<   "/>";
  return ss.str();
}

double AFEB::teststand::AnalyzedDevice::getMaxMeasuredThreshold( const double setThreshold ){
  valarray<double> thresholds( nChannels_ );
  for ( int iChannel=0; iChannel<nChannels_; ++iChannel ){
    thresholds[iChannel] = channels_[iChannel].getThresholdCharge( setThreshold );
  }
  return utils::statistics( thresholds )["max"];
}

void AFEB::teststand::AnalyzedDevice::saveResults( const string& afebRootDir, const string& analyzedResultsDir ){

  
  TPDF pdf( ( analyzedResultsDir+"/"+id_+".pdf" ).c_str() );

  // Write results to XML
  stringstream ss;
  ss << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"        << endl
     << "<?xml-stylesheet type=\"text/xml\" href=\"analyzedResults_XSLT.xml\"?>" << endl;


  ss << "<ad:device xmlns:ad=\""        << analyzedDeviceNamespace_
     <<         "\" type=\""            << type_
     <<         "\" id=\""              << id_
     <<         "\" analysisDate=\""    << utils::getDateTime()
     <<         "\" measurementDate=\"" << measurementDate_
     << "\">" << endl;
  vector<Measurement*>::const_iterator m = measurements_.begin();
  if ( m != measurements_.end() && (*m)->isDummyData() ) ss << "  <ad:dummyData/>" << endl;
  ss << "  <ad:adaptor id=\""                    << adaptorId_
     <<            "\" type=\""                  << adaptorType_
     <<            "\" socket=\""                << socket_
     <<            "\" correctionCoefficient=\"" << noshowpos << showpoint << setprecision(4) << fixed << correctionCoefficient_
     <<            "\" injectionCapacitance=\""  << noshowpos << showpoint << setprecision(4) << fixed << injectionCapacitance_
     <<            "\" pulseDivisionFactor=\""   << noshowpos << showpoint << setprecision(4) << fixed << pulseDivisionFactor_
     <<   "\"/>" << endl;

  ss << measurementsToXMLAndPlots( analyzedResultsDir, pdf );

  size_t iChannel = 0;
  for ( vector<AnalyzedChannel>::const_iterator c = channels_.begin(); c != channels_.end(); ++c ){
    ss << "  <ad:channel number=\""       << noshowpos << noshowpoint << setw(2) << iChannel++
       <<            "\" offset=\""       << showpos << showpoint << setw(8) << setprecision(4) << fixed << - c->offset_ // Comply with odd convention on sign of offset.
       <<            "\" gain=\""         << showpos << showpoint << setw(8) << setprecision(4) << fixed << c->gain_
       <<            "\" C_int=\""        << showpos << showpoint << setw(8) << setprecision(4) << fixed << c->internalCapacitance_
       <<   "\"/>" << endl;
  }
  
  ss << "  " << statisticsToXML( "gain ",  getGains()                ) << endl;
  ss << "  " << statisticsToXML( "offset", getOffsets()              ) << endl;
  ss << "  " << statisticsToXML( "C_int",  getInternalCapacitances() ) << endl;
  
  ss << "  <ad:averageSetThreshold value=\"" 
     << -1. * utils::statistics( getOffsets() )["mean"] + utils::statistics( getGains() )["mean"] * 20 // Comply with odd convention on sign of offset.
     <<                        "\" atCharge=\"20\"/>" << endl;

  ss << "  <ad:maxMeasuredThreshold value=\"" << getMaxMeasuredThreshold( 0 )
     <<                         "\" atSetThreshold=\"0\"/>" << endl;

  // Include metadata:
  try{
    ss << AFEB::teststand::utils::readFile( afebRootDir + "/AFEB/teststand/xml/AnalyzedResultMetadata.xml" );
  }
  catch( xcept::Exception& ignored ){}

  ss << "</ad:device>" << endl;

  cout << ss.str();

  //
  // Page for offsets and gains
  //
  TH1D offsetVsChannel( "offsetVsChannel", "Offset vs. channel", nChannels_, 0., 0. + nChannels_ );
  offsetVsChannel.GetXaxis()->CenterLabels( kTRUE );
  offsetVsChannel.GetXaxis()->SetNdivisions( nChannels_ );
  offsetVsChannel.SetYTitle( "offset [mV]" );
  offsetVsChannel.SetStats( kFALSE );
  offsetVsChannel.SetMarkerStyle( kOpenCircle );
  offsetVsChannel.SetMarkerColor( kBlue );
  offsetVsChannel.SetMinimum( -20. );
  offsetVsChannel.SetMaximum( 180. );
  TH1D gainVsChannel( offsetVsChannel ); // Just copy an existing x(channel) histogram instead of creating one from scratch.
  gainVsChannel.SetTitle( "Gain vs. channel" );
  gainVsChannel.SetYTitle( "gain [mV/fC]" );
  gainVsChannel.SetMinimum( 0. );
  gainVsChannel.SetMaximum( 10. );
  TH1D CintVsChannel( offsetVsChannel ); // Just copy an existing x(channel) histogram instead of creating one from scratch.
  CintVsChannel.SetTitle( "Measured internal capacitance vs. channel" );
  CintVsChannel.SetYTitle( "internal capacitance [pF]" );
  CintVsChannel.SetMinimum( 0. );
  CintVsChannel.SetMaximum( 0.4 );
  TH1D QthrAt0mVVsChannel( offsetVsChannel ); // Just copy an existing x(channel) histogram instead of creating one from scratch.
  QthrAt0mVVsChannel.SetTitle( "Threshold charge at 0mV threshold voltage setting vs. channel" );
  QthrAt0mVVsChannel.SetYTitle( "Q_{threshold}(U_{set thershold}=0mV) [fC]" );
  QthrAt0mVVsChannel.SetMinimum( -10. );
  QthrAt0mVVsChannel.SetMaximum( 50. );
  TH1D UthrFor20fCVsChannel( offsetVsChannel ); // Just copy an existing x(channel) histogram instead of creating one from scratch.
  UthrFor20fCVsChannel.SetTitle( "Threshold voltage for 20 fC charge vs. channel" );
  UthrFor20fCVsChannel.SetYTitle( "U_{set thershold}(Q_{threshold}=20fC) [fC]" );
  UthrFor20fCVsChannel.SetMinimum( 0. );
  UthrFor20fCVsChannel.SetMaximum( 300. );
  // Loop over the channels
  for ( int iChannel=0; iChannel<nChannels_; ++iChannel ){
    offsetVsChannel     .SetBinContent( iChannel+1, - channels_[iChannel].offset_                     ); // Comply with odd convention on sign of offset.
    gainVsChannel       .SetBinContent( iChannel+1, channels_[iChannel].gain_                         );
    CintVsChannel       .SetBinContent( iChannel+1, channels_[iChannel].internalCapacitance_          );
    QthrAt0mVVsChannel  .SetBinContent( iChannel+1, channels_[iChannel].getThresholdCharge( 0. )      );
    UthrFor20fCVsChannel.SetBinContent( iChannel+1, channels_[iChannel].getSetThresholdVoltage( 20. ) );
  }
  TCanvas canvas("gainCanvas","canvas",1000,1400);
  canvas.Divide( 2, 5 );  
  canvas.cd( 1 );
  gStyle->SetOptStat( 0 );
  offsetVsChannel.DrawCopy( "p" );
  canvas.cd( 2 );
  gStyle->SetOptStat( 111110 );
  TH1D hc = histogramContents( &offsetVsChannel );
  hc.SetTitle( "Offsets" );
  hc.SetXTitle( "offset [mV]" );
  hc.SetYTitle( "number of channels" );
  gStyle->SetOptStat( 111110 );
  hc.DrawCopy();
  canvas.cd( 3 );
  gainVsChannel.DrawCopy( "p" );
  canvas.cd( 4 );
  hc = histogramContents( &gainVsChannel );
  hc.SetTitle( "Gains" );
  hc.SetXTitle( "gain [mV/fC]" );
  hc.SetYTitle( "number of channels" );
  hc.DrawCopy();

  canvas.cd( 5 );
  QthrAt0mVVsChannel.DrawCopy( "p" );
  canvas.cd( 6 );
  hc = histogramContents( &QthrAt0mVVsChannel );
  hc.SetTitle( "Threshold charge at at 0mV threshold voltage setting" );
  hc.SetXTitle( "Q_{threshold} [fC]" );
  hc.SetYTitle( "number of channels" );
  hc.DrawCopy();

  canvas.cd( 7 );
  CintVsChannel.DrawCopy( "p" );
  canvas.cd( 8 );
  hc = histogramContents( &CintVsChannel );
  hc.SetTitle( "Internal capacitances" );
  hc.SetXTitle( "C_{int} [pF]" );
  hc.SetYTitle( "number of channels" );
  hc.DrawCopy();

  canvas.cd( 9 );
  UthrFor20fCVsChannel.DrawCopy( "p" );
  canvas.cd( 10 );
  hc = histogramContents( &UthrFor20fCVsChannel );
  hc.SetTitle( "Threshold voltages for 20 fC charge" );
  hc.SetXTitle( "U_{threshold} [mV]" );
  hc.SetYTitle( "number of channels" );
  hc.DrawCopy();

  canvas.cd();
  canvas.Draw();
  
  pdf.Close();

  utils::writeFile( analyzedResultsDir + "/" + id_ + ".xml", ss.str() );

}

bool AFEB::teststand::AnalyzedDevice::passesSelectionCuts( const string& analyzedResultsDir, const vector<Cut>& cuts ){
  bool passes = true;
  string analyzedResultsXML( utils::readFile( analyzedResultsDir + "/" + id_ + ".xml" ) );
  for ( vector<Cut>::const_iterator c=cuts.begin(); c!=cuts.end(); ++c ){
    double value = utils::stringTo<double>( utils::getSelectedNodeValue( analyzedResultsXML, c->getXpath() ) );
    if ( ! c->accepts( value ) ){
      cout << *c << " failed. Value=" << value << endl;
      passes = false;
    }
  }
  return passes;
}
