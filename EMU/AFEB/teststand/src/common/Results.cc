#include "AFEB/teststand/Results.h"
#include "AFEB/teststand/utils/String.h"
#include "AFEB/teststand/utils/System.h"
#include "TF1.h"
#include "TMath.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "Math/SpecFuncMathCore.h" // for erf()

#include <sstream>

using namespace AFEB::teststand;

AFEB::teststand::Results::Results( const Measurement* const measurement, const TestedDevice* const device  ) :
  bsem_( toolbox::BSem::EMPTY ), // locked
  isFinal_( false ),
  measurement_( measurement ),
  testedDevice_( device ),
  fileName_( utils::stringFrom<int>( measurement_->getIndex() ) 
	     + "__" + measurement_->getType()
	     + "__" + testedDevice_->getId() )
{

  // pulses( channel, amplitude)  2D histogram
  stringstream name;
  name << "pulses__" << fileName_;
  stringstream title;
  title << measurement_->getName() 
	<< " with " << measurement_->getNPulses()
	<< " pulses of " << testedDevice_->getType()
	<< " of id "  << testedDevice_->getId();
  int ampSpan = 
    ( measurement_->getAmplitudeMax() - measurement_->getAmplitudeMin() ) -
    ( measurement_->getAmplitudeMax() - measurement_->getAmplitudeMin() ) %  measurement_->getAmplitudeStep();
  int nAmp = ampSpan / measurement_->getAmplitudeStep() + 1;
  pulses_ = new TH2D( name.str().c_str(),
		      title.str().c_str(),
		      testedDevice_->getNChannels(),
		      0.,
		      0. + testedDevice_->getNChannels(),
		      nAmp,
		      measurement_->getAmplitudeMin() -          0.5   * measurement_->getAmplitudeStep(),
		      measurement_->getAmplitudeMin() + ( nAmp - 0.5 ) * measurement_->getAmplitudeStep()
		      );
  pulses_->SetXTitle( "channel" );
  pulses_->SetYTitle( "amplitude [ADC units]" );
  pulses_->SetZTitle( "count" );
  pulses_->SetTitleOffset( 0.8, "x" );
  pulses_->SetTitleOffset( 0.5, "y" );
  pulses_->SetTitleOffset( 0.7, "z" );
  pulses_->SetTitleSize( 0.08, "x" );
  pulses_->SetTitleSize( 0.08, "y" );
  pulses_->SetTitleSize( 0.08, "z" );
  pulses_->SetLabelSize( 0.08, "x" );
  pulses_->SetLabelSize( 0.08, "y" );
  pulses_->SetLabelSize( 0.08, "z" );
  pulses_->SetMinimum( 0. );
  pulses_->SetMaximum( 2 * measurement_->getNPulses() );
  pulses_->SetStats( kFALSE );
  pulses_->GetXaxis()->CenterLabels( kTRUE );
  pulses_->GetXaxis()->SetNdivisions( testedDevice_->getNChannels() );

  // measured threshold( channel ) 1D histogram
  name.str("");
  name << "threshold__" << fileName_;
  title.str("");
  title << "measured threshold for " << testedDevice_->getType()
	<< " of id "  << testedDevice_->getId();
  threshold_ = new TH1D( name.str().c_str(),
			 title.str().c_str(),
			 testedDevice_->getNChannels(),
			 0.,
			 0. + testedDevice_->getNChannels() );
  threshold_->SetXTitle( "channel" );
  threshold_->SetYTitle( "threshold" );
  threshold_->SetTitleOffset( 0.5, "y" );
  threshold_->SetTitleSize( 0.08, "y" );
  threshold_->SetLabelSize( 0.08, "y" );
  threshold_->SetStats( kFALSE );
  threshold_->SetMarkerStyle( kOpenCircle );
  threshold_->SetMinimum( - 0.1 * measurement_->getAmplitudeMax() );
  threshold_->SetMaximum(   1.1 * measurement_->getAmplitudeMax() );
  threshold_->GetXaxis()->CenterLabels( kTRUE );
  threshold_->GetXaxis()->SetNdivisions( testedDevice_->getNChannels() );

  // measured noise( channel ) 1D histogram
  name.str("");
  name << "noise__" << fileName_;
  title.str("");
  title << "measured noise for " << testedDevice_->getType()
	<< " of id "  << testedDevice_->getId();
  noise_ = new TH1D( name.str().c_str(),
		     title.str().c_str(),
		     testedDevice_->getNChannels(),
		     0.,
		     0. + testedDevice_->getNChannels() );
  noise_->SetXTitle( "channel" );
  noise_->SetYTitle( "noise" );
  noise_->SetStats( kFALSE );
  noise_->SetMarkerStyle( kOpenSquare );
  noise_->SetMinimum( - 0.1 * measurement_->getAmplitudeMax() );
  noise_->SetMaximum(   1.1 * measurement_->getAmplitudeMax() );
  noise_->GetXaxis()->CenterLabels( kTRUE );
  noise_->GetXaxis()->SetNdivisions( testedDevice_->getNChannels() );

  // measured efficiency( channel ) 1D histogram
  name.str("");
  name << "efficiency__" << fileName_;
  title.str("");
  title << "measured efficiency for " << testedDevice_->getType()
	<< " of id "  << testedDevice_->getId();
  efficiency_ = new TH1D( name.str().c_str(),
			  title.str().c_str(),
			  testedDevice_->getNChannels(),
			  0.,
			  0. + testedDevice_->getNChannels() );
  efficiency_->SetXTitle( "channel" );
  efficiency_->SetYTitle( "efficiency:#bullet" );
  efficiency_->SetStats( kFALSE );
  efficiency_->SetMarkerStyle( kFullDotLarge );
  efficiency_->SetMarkerColor( kBlue );
  efficiency_->SetLineColor( kBlue );
  efficiency_->SetMinimum( -0.1 );
  efficiency_->SetMaximum(  1.1 );
  efficiency_->GetXaxis()->CenterLabels( kTRUE );
  efficiency_->GetXaxis()->SetNdivisions( testedDevice_->getNChannels() );

  // threshold S-curve fit's chi^2/ndf( channel ) 1D histogram
  name.str("");
  name << "chi2ndf__" << fileName_;
  title.str("");
  title << "S-curve fit's chi^2/ndf for " << testedDevice_->getType()
	<< " of id "  << testedDevice_->getId();
  chi2ndf_ = new TH1D( name.str().c_str(),
		       title.str().c_str(),
		       testedDevice_->getNChannels(),
		       0.,
		       0. + testedDevice_->getNChannels() );
  chi2ndf_->SetXTitle( "channel" );
  chi2ndf_->SetYTitle( "#chi^2/ndf" );
  chi2ndf_->SetTitleOffset( 0.5, "y" );
  chi2ndf_->SetTitleSize( 0.08, "y" );
  chi2ndf_->SetLabelSize( 0.08, "y" );
  chi2ndf_->SetStats( kFALSE );
  chi2ndf_->SetMarkerStyle( kOpenCircle );
  chi2ndf_->GetXaxis()->CenterLabels( kTRUE );
  chi2ndf_->GetXaxis()->SetNdivisions( testedDevice_->getNChannels() );

  // measured mean+-rms( channel ) 1D histogram
  name.str("");
  name << "timesOnPlateau__" << fileName_;
  title.str("");
  title << "mean and rms of measured times on efficiency plateau for " << testedDevice_->getType()
	<< " of id "  << testedDevice_->getId();
  timeOnPlateau_ = new TH1D( name.str().c_str(),
			  title.str().c_str(),
			  testedDevice_->getNChannels(),
			  0.,
			  0. + testedDevice_->getNChannels() );
  timeOnPlateau_->SetXTitle( "channel" );
  timeOnPlateau_->SetYTitle( "RMS of times on plateau" );
  // timeOnPlateau_->SetStats( kFALSE );
  // timeOnPlateau_->SetMarkerStyle( kFullDotLarge );
  // timeOnPlateau_->SetMarkerColor( kBlue );
  // timeOnPlateau_->SetLineColor( kBlue );
  // timeOnPlateau_->SetMinimum(  0.0000008 );
  // timeOnPlateau_->SetMaximum(  1.0 );
  // timeOnPlateau_->GetXaxis()->CenterLabels( kTRUE );
  // timeOnPlateau_->GetXaxis()->SetNdivisions( testedDevice_->getNChannels() );

  // time vs amplitude profile histogram for each channel, and
  // measured efficiency( amplitude ) for each channel
  enum lineStyles { solid=1, dotted=3, dashed=5, dottedDashed=7 };
  enum colors { black=1, red=2, blue=4, green=8 };
  const int nStyles = 4;
  const int nColors = 4;
  const unsigned long style[nStyles] = { solid, dotted, dashed, dottedDashed };
  const unsigned long color[nColors] = { black, red, blue, green  };
  legend_ = new TLegend( 0.90, 0.1, 0.99, 0.9 );
  legend_->SetHeader( "channel" );
  legend_->SetTextAlign( 22 ); // middle, middle
  legend_->SetTextSize( 0.025 );
  legend_->SetMargin( 0.6 ); // to make the line segment as long as 0.6 times the box width
  for ( int iChannel = 0; iChannel < testedDevice_->getNChannels(); ++iChannel ){

    // time vs amplitude profile histogram
    name.str("");
    name << "timeVsAmpl__" << measurement_->getIndex() << "__" << measurement_->getType() 
	 << "__" << testedDevice_->getId() << "__ch_" << iChannel;
    title.str("");
    title << "Times in channel " << iChannel << " of " << testedDevice_->getType()
	  << " of id "  << testedDevice_->getId();
    TProfile* t = new TProfile( name.str().c_str(), 
				title.str().c_str(),
				nAmp,
				measurement_->getAmplitudeMin() -          0.5   * measurement_->getAmplitudeStep(),
				measurement_->getAmplitudeMin() + ( nAmp - 0.5 ) * measurement_->getAmplitudeStep(),
				measurement_->getTDCTimeMin(),
				measurement_->getTDCTimeMax(),
				"S" ); // spread option (not error on mean)
    t->SetXTitle( "amplitude" );
    t->SetYTitle( "time [TDC units]" );
    t->SetStats( kFALSE );
    if ( iChannel / nStyles < nColors ) t->SetLineColor( color[ iChannel/nStyles ] );
    t->SetLineStyle( style[ iChannel%nStyles ] );
    timeVsAmplitude_.push_back( t );

    // measured efficiency( amplitude ) graph
    name.str("");
    name << "effVsAmpl__" << measurement_->getIndex() << "__" << measurement_->getType() 
	 << "__" << testedDevice_->getId() << "__ch_" << iChannel;
    title.str("");
    title << "Efficiency vs. pulse amplitude in channel " << iChannel << " of " << testedDevice_->getType()
	  << " of id "  << testedDevice_->getId();
    TH1D *e = new TH1D( name.str().c_str(), 
			title.str().c_str(),
			nAmp,
			measurement_->getAmplitudeMin() -          0.5   * measurement_->getAmplitudeStep(),
			measurement_->getAmplitudeMin() + ( nAmp - 0.5 ) * measurement_->getAmplitudeStep() );
    e->SetXTitle( "amplitude" );
    e->SetYTitle( "efficiency" );
    e->SetStats( kFALSE );
    if ( iChannel / nStyles < nColors ) e->SetLineColor( color[ iChannel/nStyles ] );
    e->SetLineStyle( style[ iChannel%nStyles ] );
    sCurve_.push_back( e );
    legend_->AddEntry( e,  utils::stringFrom<int>( iChannel ).c_str(), "l" );

  } // for ( int iChannel = 0; iChannel < testedDevice_->getNChannels(); ++iChannel )



  // ( channel, amplitude, time ) ntuple
  name.str("");
  name << "times__" << fileName_;
  title.str("");
  title << "TDC times of " << measurement_->getNPulses()
	<< " pulses for " << testedDevice_->getType()
	<< " of id "  << testedDevice_->getId();
  times_ = new TTree( name.str().c_str(), title.str().c_str() );
  times_->Branch("ch",&channel_,"ch/I");
  times_->Branch("a",&amplitude_,"a/I");
  times_->Branch("t",&time_,"t/I");

  bsem_.give();
}

AFEB::teststand::Results::~Results(){
  delete times_;
  delete pulses_;
  delete threshold_;
  delete noise_;
  delete efficiency_;
  delete chi2ndf_;
  delete timeOnPlateau_;
  delete legend_;
  for ( vector<TProfile*>::iterator t=timeVsAmplitude_.begin(); t!=timeVsAmplitude_.end(); ++t ) delete *t;
  for ( vector<TH1D*    >::iterator s=         sCurve_.begin(); s!=         sCurve_.end(); ++s ) delete *s;
}

void AFEB::teststand::Results::add( const int channel, int const amplitude, const int time ){
  bsem_.take();
  channel_   = channel;
  amplitude_ = amplitude;
  time_      = time;
  times_->Fill();
  if ( measurement_->getTDCTimeMin() <= time_ && time_ <= measurement_->getTDCTimeMax() ){ 
    pulses_->Fill( channel, amplitude );
    sCurve_.at( channel )->Fill( amplitude );
    timeVsAmplitude_.at( channel )->Fill( amplitude, time );
  }
  bsem_.give();
}

//double AFEB::teststand::Results::normalCDF( double *x, double *p ) const {
double normalCDF( double *x, double *p ){
  // normal cumulative distrubtion function
  return p[2] * 0.5 * ( 1. + ROOT::Math::erf( ( x[0] - p[0] ) / ( TMath::Sqrt(2.) * p[1] ) ) );
}

void AFEB::teststand::Results::estimateFitParameters( TH1D& hist,
						      const double from,
						      const double to,
						      double& mean,
						      double& sigma,
						      double& height ){
  // Estimate the fit parameters assuming hist is ~ a normal CDF.

  // The difference histogram:
  int fromBin = hist.FindBin( from );
  int toBin   = hist.FindBin( to   );
  double lo = hist.GetBinLowEdge( fromBin   );
  double hi = hist.GetBinLowEdge( toBin + 1 );
  int nBins = toBin - fromBin + 1;
  TH1D diff( "diff", "diff",
	     nBins - 1,
	     lo + 0.5 * ( hi - lo ) / nBins,
	     hi - 0.5 * ( hi - lo ) / nBins );
  int iBin = 1;
  // cout << " from=" << from
  //      << " to="   <<   to
  //      << " fromBin=" << fromBin
  //      << " toBin="   <<   toBin
  //      << " lo=" << lo
  //      << " hi=" << hi 
  //      << " nBins="   <<   nBins
  //      << endl;
  for ( int i=fromBin; i<=toBin-1; ++i ){
    diff.SetBinContent( iBin, hist.GetBinContent( i+1 ) - hist.GetBinContent( i ) );
    // cout << diff.GetBinContent( iBin ) << " ";
    ++iBin;
  }
  // cout << endl;


  mean   = diff.GetMean();
  sigma  = diff.GetRMS();
  // Find maximum:
  height = 0.;
  for ( int i=fromBin; i<=toBin; ++i ){
    if ( height < hist.GetBinContent( i ) ) height = hist.GetBinContent( i );
  }

  // cout << " m = " << mean 
  //      << " s = " << sigma 
  //      << " h = " << height 
  //      << endl;
}

double AFEB::teststand::Results::mean( const TH1D& hist, const double from, const double to ){
  // Calculate the mean of histogram values between from and to.
  double sumY  = 0;
  int n = 0;
  for ( int i=1; i<=hist.GetNbinsX(); ++i ){
    double x = hist.GetBinCenter( i );
    if ( from <= x && x <= to ){
      double y = hist.GetBinContent( i );
      sumY += y;
      n++;
    }
  }
  return ( n>0 ? sumY / n : 0. );
}

double AFEB::teststand::Results::rms( const TH1D& hist, const double from, const double to ){
  // Calculate the RMS of histogram values between from and to.
  double m = mean( hist, from, to );
  double sumD2  = 0;
  int n = 0;
  for ( int i=1; i<=hist.GetNbinsX(); ++i ){
    double x = hist.GetBinCenter( i );
    if ( from <= x && x <= to ){
      double y = hist.GetBinContent( i );
      sumD2 += ( y - m ) * ( y - m );
      n++;
    }
  }
  return ( n>0 ? TMath::Sqrt( sumD2 / n ) : 0. );
}

void AFEB::teststand::Results::fit( const double from, const double to ){
  // Fit the threshold scan's S-curve with a normal CDF.
  bsem_.take();
  double lo = pulses_->GetYaxis()->GetXmin();
  double hi = pulses_->GetYaxis()->GetXmax();
  TH1D p( "pulses", "pulses", pulses_->GetNbinsY(), lo, hi );
  bsem_.give();

  // Make sure fit range doesn't extend beyond that of the histogram.
  // To be sure, restrict it to between the first and last bin's center.
  double From = TMath::Max( from, p.GetBinCenter( 1             ) );
  double To   = TMath::Min( to  , p.GetBinCenter( p.GetNbinsX() )  );   

  const char* normalCDFname = "normalCDF";
  //TF1 nCDF( "normalCDF", &AFEB::teststand::Results::normalCDF, lo, hi, 3, NULL );
  TF1 nCDF( normalCDFname, &normalCDF, lo, hi, 3, (const char*)NULL );
  nCDF.SetParNames( "mean", "sigma", "height" );
  double mean, sigma, height;
  nCDF.SetParameters( 0.5 * ( hi + lo ),
		      0.1 * ( hi - lo ),
		      measurement_->getNPulses() );
  if ( To != From ) nCDF.SetRange( From, To ); // fit only in the range measured so far

  // The array of amplitudes at which the efficiency plateau starts in each channel
  vector<double> plateauStarts;
  plateauStarts.resize( testedDevice_->getNChannels(), 0. );

  // Loop over channels and fit results
  for ( int iChannelBin = 1; iChannelBin <= testedDevice_->getNChannels(); ++iChannelBin ){
    p.Reset();
    // cout << "ch = " << iChannelBin-1 << endl;
    // cout << "counts = ( ";

    bsem_.take();
    for ( int iAmpBin = 1; iAmpBin <= pulses_->GetNbinsY(); ++iAmpBin ){
      // cout << pulses_->GetBinContent( iChannelBin, iAmpBin ) << " ";
      p.SetBinContent( iAmpBin, pulses_->GetBinContent( iChannelBin, iAmpBin ) );
    }
    bsem_.give();

    // cout << ")" << endl;

    estimateFitParameters( p, From, To, mean, sigma, height );

    bsem_.take();
    if ( isFinal_ ){
      if ( measurement_->getTypeType() == AFEB::teststand::Measurement::time_vs_dac ){
	// Fit may fail in time_vs_dac as the transition region may not be covered or resolved well. 
	// Use the estimates instead, which are less accurate but more robust.
	threshold_ ->SetBinContent( iChannelBin, mean  );
	noise_     ->SetBinContent( iChannelBin, sigma );
	efficiency_->SetBinContent( iChannelBin, height / measurement_->getNPulses() );
	chi2ndf_   ->SetBinContent( iChannelBin, 0. );
	threshold_ ->SetBinError( iChannelBin, 0. );
	noise_     ->SetBinError( iChannelBin, 0. );
	efficiency_->SetBinError( iChannelBin, 0.00001 ); // For some reason the efficiency histogram refuses to be plotted scaled to the others if the error is zero...
	// // If sigma=0 (the S curve is a step function at the given resolution), take the increment
	// // size for sigma, i.e., start the plateau 3 amplitude steps away from the transition.
	// double plateauStart = mean + 3. * TMath::Max( sigma, double( measurement_->getAmplitudeStep() ) );

	// Start it at twice the (apparent) threshold. 
	// In time_vs_dac measurements we sure go up to a high enough amplitude to have a long plateau.
	plateauStarts[iChannelBin-1] = 2 * mean;
      }
      else{
	// Calculate everything properly for the final results.
	// p.Print( "all" );
	// cout << "ch = " << iChannelBin-1
	//      << " lo = " << lo 
	//      << " hi = " << hi 
	//      << " from = " << from 
	//      << " to = " << to 
	//      << " From = " << From 
	//      << " To = " << To 
	//      << " m = " << mean 
	//      << " s = " << sigma 
	//      << " h = " << height 
	//      << endl;
	nCDF.SetParameters( mean, sigma, height );
	p.Fit( &nCDF, "QR" ); // quiet (no printing), use function range
	threshold_ ->SetBinContent( iChannelBin, nCDF.GetParameter( 0 ) );
	noise_     ->SetBinContent( iChannelBin, nCDF.GetParameter( 1 ) );
	efficiency_->SetBinContent( iChannelBin, nCDF.GetParameter( 2 ) / measurement_->getNPulses() );
	chi2ndf_   ->SetBinContent( iChannelBin, nCDF.GetChisquare() / ( pulses_->GetNbinsY() - 3 ) );
	threshold_ ->SetBinError( iChannelBin, nCDF.GetParError( 0 ) );
	noise_     ->SetBinError( iChannelBin, nCDF.GetParError( 1 ) );
	efficiency_->SetBinError( iChannelBin, nCDF.GetParError( 2 ) / measurement_->getNPulses() );    
	// Look at times beyond 3 stddev above transition.
	plateauStarts[iChannelBin-1] = nCDF.GetParameter( 0 ) + 3. * nCDF.GetParameter( 1 );
      }
    }
    else{
      // Just use estimates for the updates to show progress.
      threshold_ ->SetBinContent( iChannelBin, mean  );
      noise_     ->SetBinContent( iChannelBin, sigma );
      efficiency_->SetBinContent( iChannelBin, height / measurement_->getNPulses() );
      threshold_ ->SetBinError( iChannelBin, 0. );
      noise_     ->SetBinError( iChannelBin, 0. );
      efficiency_->SetBinError( iChannelBin, 0.00001 ); // For some reason the efficiency histogram refuses to be plotted scaled to the others if the error is zero...
    }
    bsem_.give();
  }

  bsem_.take();
  if ( isFinal_ ){
    timesOnEfficiencyPlateau( plateauStarts ); // This fills timeOnPlateau_.
  }
  bsem_.give();

}

void AFEB::teststand::Results::timesOnEfficiencyPlateau( vector<double>& plateauStarts ){
  // An array of auxiliary histograms just to do the statistics for us:
  vector<TH1D*> timesOfChannel;
  for ( int iChannel=0; iChannel<testedDevice_->getNChannels(); ++iChannel ){
    timesOfChannel.push_back( new TH1D( (string( "timesOfChannel" )+utils::stringFrom<int>( iChannel )).c_str(),
					(string( "timesOfChannel" )+utils::stringFrom<int>( iChannel )).c_str(), 
					measurement_->getTDCTimeMax()-measurement_->getTDCTimeMin(),
					double( measurement_->getTDCTimeMin() ),
					double( measurement_->getTDCTimeMax() ) ) );
  }
  // Loop over all entries and collect the times on the efficiency plateau of that channel:
  for( Long64_t i=0; i<times_->GetEntries(); ++i ){
    times_->GetEntry( i );
    if ( amplitude_ > plateauStarts.at( channel_ ) ) timesOfChannel.at( channel_ )->Fill( time_ );
  }
  // Get the statistics of every channel into the timeOnPlateau_ histogram:
  for ( int iChannel = 0; iChannel < testedDevice_->getNChannels(); ++iChannel ){
    timeOnPlateau_->SetBinContent( iChannel + 1, timesOfChannel.at( iChannel )->GetMean() );
    timeOnPlateau_->SetBinError  ( iChannel + 1, timesOfChannel.at( iChannel )->GetRMS () );    
  }
  // Delete the auxiliary histograms:
  for ( vector<TH1D*>::iterator h = timesOfChannel.begin(); h != timesOfChannel.end(); ++h ) delete *h;
}

void AFEB::teststand::Results::createFigure( const string directory, const double fitRangeStart, const double fitRangeEnd ){
  fit( fitRangeStart, fitRangeEnd );

  // Work on copies to keep original histograms intact:
  bsem_.take();
  TH2D pulses( *pulses_ );
  TH1D threshold( *threshold_ );
  TH1D efficiency( *efficiency_ );
  TH1D noise( *noise_ );
  bsem_.give();
  
  gStyle->SetPalette(1,0);

  TLatex latex;
  latex.SetTextSize( 0.025 );
  latex.SetTextAlign(23);  // h centered, v top aligned
  latex.DrawLatex( 0.5, 0.99, pulses_->GetTitle() );

  TCanvas c( fileName_.c_str(), measurement_->getName().c_str(), 500, 1500 );
  c.Divide( 1, 3, 0.00001, 0.00001 );

  TPad *efficiencyPad = (TPad*)c.GetPad(1);
  TPad *sCurvePad = (TPad*)c.GetPad(2);
  TPad *timePad = (TPad*)c.GetPad(3);

  //
  // The threshold scan (efficiencies)
  // 
  efficiencyPad->Divide( 1, 3, 0., 0. );

  efficiencyPad->cd( 0 );
  gPad->SetBottomMargin( 0.05 );

  efficiencyPad->cd( 1 );
  gPad->SetRightMargin( 0.05 );
  gPad->SetTopMargin( 0.01 );
  gPad->SetGridx();
  gPad->SetFrameFillColor( kGray + 3 );
  pulses.SetTitle("");
  pulses_->SetZTitle( "count" );
  pulses.DrawCopy("colz");

  efficiencyPad->cd( 2 );
  gPad->SetRightMargin( 0.05 );
  // gPad->SetBottomMargin( 0.15 );
  gPad->SetGridx();
  gPad->SetGridy();
  threshold.SetTitle("");
  threshold.SetYTitle("threshold:#circ, noise:#Box [ADC]");
  threshold.DrawCopy("p e");
  noise.DrawCopy("same p e");
  // cout << "Efficiency before adjustment"; efficiency.Print("all");
  TGaxis *axis = adjustToHistogram( &threshold, &efficiency );
  // cout << "Efficiency after adjustment "; efficiency.Print("all");
  axis->SetTitleOffset( 0.7 );
  axis->SetTitleSize( 0.08 );
  axis->SetLabelSize( 0.08 );
  axis->Draw();
  efficiency.DrawCopy("same p e");

  efficiencyPad->cd( 3 );
  gPad->SetRightMargin( 0.05 );
  gPad->SetTopMargin( 0.01 );
  gPad->SetBottomMargin( 0.15 );
  gPad->SetGridx();
  gPad->SetFrameFillColor( kGray + 3 );
  pulses.SetMinimum( measurement_->getNPulses() - 2 );
  pulses.SetMaximum( measurement_->getNPulses() + 2 );
  pulses.SetNdivisions( 5, "z" );
  pulses.SetTitle("");
  pulses_->SetZTitle( "count (zoomed in on 100%)" );
  pulses.DrawCopy("colz");

  //
  // S curves
  //
  sCurvePad->cd( 0 );
  gPad->SetLeftMargin( 0.12 );
  gPad->SetRightMargin( 0.10 );
  for ( int iChannel = 0; iChannel < testedDevice_->getNChannels(); ++iChannel ){
    // Create a copy so as not to change the original's tile:
    TH1D e( *sCurve_.at( iChannel ) );
    e.Scale( 1./measurement_->getNPulses() );
    e.SetTitle( (string( "S curve for device " ) + testedDevice_->getId()).c_str() );
    e.SetMinimum( 0. );
    e.SetMaximum( 2.1 );
    e.SetTitleOffset( 1.5, "Y" );
    if ( iChannel == 0 ) e.DrawCopy("l");
    else                 e.DrawCopy("lsame");
  }
  legend_->Draw();

  //
  // The time responses
  //
  timePad->cd();
  gPad->SetLeftMargin( 0.12 );
  gPad->SetRightMargin( 0.10 );
  for ( int iChannel = 0; iChannel < testedDevice_->getNChannels(); ++iChannel ){
    // Profile hist needs projecting:
    TH1D *tp = timeVsAmplitude_.at( iChannel )->ProjectionX( "t", "" );
    // Create a copy so that we can delete the original projection, just to avoid any possible interference:
    TH1D t( *tp );
    // Delete original projection:
    delete tp;
    t.SetLineStyle( timeVsAmplitude_.at( iChannel )->GetLineStyle() ); // Keep line style...
    t.SetLineColor( timeVsAmplitude_.at( iChannel )->GetLineColor() ); // ...and color
    t.SetTitle( (string( "Mean time vs. amplitude for device " ) + testedDevice_->getId()).c_str() );
    t.SetTitleOffset( 1.5, "Y" );
    t.SetStats( kFALSE );
    if ( iChannel == 0 ){
      t.SetMinimum( measurement_->getTDCTimeMin() );
      t.SetMaximum( measurement_->getTDCTimeMax() );
      t.DrawCopy("l");
    }
    else{
      t.DrawCopy("lsame");
    }
  }
  legend_->Draw();

  c.Print( ( directory + "/" + fileName_+".png").c_str() );
  delete axis;
}

TGaxis* AFEB::teststand::Results::adjustToHistogram( const TH1* const h1, TH1* h2, bool isNewAxisOnRight ){
  // Transform h2 to h1 so they can be superimposed on the same plot, 
  // and return the axis for h2.
  // The desired min and max of both h1 and h2 should already be set when this method is invoked.

  // The abscissa
  double alo = h1->GetXaxis()->GetXmin();
  double ahi = h1->GetXaxis()->GetXmax();

  // The ordinate to which to adjust
  double h1lo = h1->GetMinimum();
  double h1hi = h1->GetMaximum();
  
  // The ordinate to be transformed
  double h2lo = h2->GetMinimum();
  double h2hi = h2->GetMaximum();

  double s = ( h1hi - h1lo ) / ( h2hi - h2lo );

  // The transformation that takes h2lo -> h1lo and h2hi -> h1hi:
  // f(x) = ( x - h2lo ) * ( h1hi - h1lo ) / ( h2hi - h2lo ) + h1lo
  //      = ( x - h2lo ) * s + h1lo

  for ( int i=1; i<=h2->GetNbinsX(); ++i ){
    h2->SetBinContent( i, s * ( h2->GetBinContent( i ) - h2lo ) + h1lo );
    h2->SetBinError  ( i, s *   h2->GetBinError( i )                   );
  }

  double x = ( isNewAxisOnRight ? ahi : alo );
  double start_y = h1lo;
  double end_y   = h1hi;

  TGaxis *axis = new TGaxis(x, start_y,
			    x, end_y  ,
			    h2lo, h2hi,
			    510, "+L" );
  axis->SetLineColor( h2->GetLineColor() );
  axis->SetLabelColor( h2->GetLineColor() );
  axis->SetTitleColor( h2->GetLineColor() );
  axis->SetTitle( h2->GetYaxis()->GetTitle() );
  return axis;
}

void AFEB::teststand::Results::save( const string directory ){

  // Indicate that everything must be calculated:
  bsem_.take();
  isFinal_ = true;
  bsem_.give();

  createFigure( directory, measurement_->getAmplitudeMin(), measurement_->getAmplitudeMax() );
  times_->Print();
  TFile f( (directory + "/" + fileName_ +".root").c_str(), "UPDATE", "", 9 );
  f.cd();

  bsem_.take();
  // times_->Write();
  pulses_->Write();
  threshold_->Write();
  chi2ndf_->Write();
  noise_->Write();
  efficiency_->Write();
  timeOnPlateau_->Write();
  for ( vector<TProfile*>::iterator t=timeVsAmplitude_.begin(); t!=timeVsAmplitude_.end(); ++t ) (*t)->Write();
  for ( vector<TH1D*>::iterator s=sCurve_.begin(); s!=sCurve_.end(); ++s ) (*s)->Write();
  bsem_.give();

  f.Close();

  // For backward compatibility, save in old format as well:
  saveInOldFormat( directory );
}

void AFEB::teststand::Results::saveInOldFormat( const string directory ){
  stringstream resultInOldFormat;
  for ( int iAmpBin = 1; iAmpBin <= pulses_->GetNbinsY(); ++iAmpBin ){
    resultInOldFormat << TMath::Nint( pulses_->GetYaxis()->GetBinCenter( iAmpBin ) ) << "\t";
    for ( int iChannelBin = 1; iChannelBin <= testedDevice_->getNChannels(); ++iChannelBin ){
      resultInOldFormat << TMath::Nint( pulses_->GetBinContent( iChannelBin, iAmpBin ) );
      if ( iChannelBin == testedDevice_->getNChannels() ) resultInOldFormat << endl;
      else                                                resultInOldFormat << "\t";
    }
  }
  utils::writeFile( directory + "/" + fileName_ + ".dat", resultInOldFormat.str() );
}

map<string,pair<double,double> > AFEB::teststand::Results::getParameters( const int channel ) const {
  map<string,pair<double,double> > values;
  values["threshold [ADC]"      ] = make_pair<double,double>( threshold_    ->GetBinContent( channel+1 ),
							      threshold_    ->GetBinError  ( channel+1 ) );
  values["noise [ADC]"          ] = make_pair<double,double>( noise_        ->GetBinContent( channel+1 ),
							      noise_        ->GetBinError  ( channel+1 ) );
  values["efficiency"           ] = make_pair<double,double>( efficiency_   ->GetBinContent( channel+1 ),
							      efficiency_   ->GetBinError  ( channel+1 ) );
  values["chi2ndf"              ] = make_pair<double,double>( chi2ndf_      ->GetBinContent( channel+1 ), 
							      0.                                         );
  values["time on plateau [TDC]"] = make_pair<double,double>( timeOnPlateau_->GetBinContent( channel+1 ), 
							      timeOnPlateau_->GetBinError  ( channel+1 ) );
  return values;
}

map<string,double> AFEB::teststand::Results::getStats( const TH1D* hist ){
  map<string,double> stats;  
  bsem_.take();
  stats["mean"] = mean( *hist, hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax() );
  stats["rms" ] = rms ( *hist, hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax() );
  stats["span"] = hist->GetBinContent( hist->GetMaximumBin() ) - hist->GetBinContent( hist->GetMinimumBin() );
  bsem_.give();
  return stats;
}

map<string,double> AFEB::teststand::Results::getThresholdStats(){
  return getStats( threshold_ );
}

map<string,double> AFEB::teststand::Results::getTimeStats(){
  return getStats( timeOnPlateau_ );
}
