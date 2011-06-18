#include "AFEB/teststand/Results.h"
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
  measurement_( measurement ),
  testedDevice_( device ),
  fileName_( measurement_->getType() + "_" + testedDevice_->getType() + "_" + testedDevice_->getId() )
{

  // pulses( channel, amplitude)  2D histogram
  stringstream name;
  name << "pulses__" << measurement_->getIndex() << "_" << measurement_->getType() 
       << "__" << testedDevice_->getId();
  stringstream title;
  title << measurement_->getName() 
	<< " with " << measurement_->getNPulses()
	<< " pulses of " << testedDevice_->getType()
	<< " of id "  << testedDevice_->getId();
  int yspan = 
    ( measurement_->getAmplitudeMax() - measurement_->getAmplitudeMin() ) -
    ( measurement_->getAmplitudeMax() - measurement_->getAmplitudeMin() ) %  measurement_->getAmplitudeStep();
  int ny = yspan / measurement_->getAmplitudeStep() + 1;
  pulses_ = new TH2D( name.str().c_str(),
		      title.str().c_str(),
		      testedDevice_->getNChannels(),
		      0.,
		      0. + testedDevice_->getNChannels(),
		      ny,
		      measurement_->getAmplitudeMin() -        0.5   * measurement_->getAmplitudeStep(),
		      measurement_->getAmplitudeMin() + ( ny - 0.5 ) * measurement_->getAmplitudeStep()
		      );
  pulses_->SetXTitle( "channel" );
  pulses_->SetYTitle( "amplitude" );
  pulses_->SetZTitle( "count" );
  pulses_->SetMinimum( 0. );
  pulses_->SetMaximum( measurement_->getNPulses() );
  pulses_->SetStats( kFALSE );
  pulses_->GetXaxis()->CenterLabels( kTRUE );
  pulses_->GetXaxis()->SetNdivisions( testedDevice_->getNChannels() );

  // measured threshold( channel ) 1D histogram
  name.str("");
  name << "threshold__" << measurement_->getIndex() << "_" << measurement_->getType() 
       << "__" << testedDevice_->getId();
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
  threshold_->SetStats( kFALSE );
  threshold_->SetMarkerStyle( kOpenCircle );
  threshold_->SetMinimum( - 0.1 * measurement_->getAmplitudeMax() );
  threshold_->SetMaximum(   1.1 * measurement_->getAmplitudeMax() );
  threshold_->GetXaxis()->CenterLabels( kTRUE );
  threshold_->GetXaxis()->SetNdivisions( testedDevice_->getNChannels() );

  // measured noise( channel ) 1D histogram
  name.str("");
  name << "noise__" << measurement_->getIndex() << "_" << measurement_->getType() 
       << "__" << testedDevice_->getId();
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
  name << "efficiency__" << measurement_->getIndex() << "_" << measurement_->getType() 
       << "__" << testedDevice_->getId();
  title.str("");
  title << "measured efficiency for " << testedDevice_->getType()
	<< " of id "  << testedDevice_->getId();
  efficiency_ = new TH1D( name.str().c_str(),
			  title.str().c_str(),
			  testedDevice_->getNChannels(),
			  0.,
			  0. + testedDevice_->getNChannels() );
  efficiency_->SetXTitle( "channel" );
  efficiency_->SetYTitle( "efficiency" );
  efficiency_->SetStats( kFALSE );
  efficiency_->SetMarkerStyle( kFullDotLarge );
  efficiency_->SetMarkerColor( kBlue );
  efficiency_->SetLineColor( kBlue );
  efficiency_->SetMinimum( -0.1 );
  efficiency_->SetMaximum(  1.1 );
  efficiency_->GetXaxis()->CenterLabels( kTRUE );
  efficiency_->GetXaxis()->SetNdivisions( testedDevice_->getNChannels() );

  // ( channel, amplitude, time ) ntuple
  name.str("");
  name << "times__" << measurement_->getIndex() << "_" << measurement_->getType() 
       << "__" << testedDevice_->getId();
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
}

void AFEB::teststand::Results::add( const int channel, int const amplitude, const int time ){
  bsem_.take();
  channel_   = channel;
  amplitude_ = amplitude;
  time_      = time;
  times_->Fill();
  pulses_->Fill( channel, amplitude );
  bsem_.give();
}

//double AFEB::teststand::Results::normalCDF( double *x, double *p ) const {
double normalCDF( double *x, double *p ){
  // normal cumulative distrubtion function
  return p[2] * 0.5 * ( 1. + ROOT::Math::erf( ( x[0] - p[0] ) / ( TMath::Sqrt(2.) * p[1] ) ) );
}

void AFEB::teststand::Results::estimateFitParameters( const TH1D& hist,
						      double& mean,
						      double& sigma,
						      double& height ){
  // Estimate the fit parameters assuming hist is ~ a normal CDF.

  // The original histogram's binning:
  double lo = hist.GetXaxis()->GetXmin();
  double hi = hist.GetXaxis()->GetXmax();
  int nBins = hist.GetNbinsX();
  
  // The difference histogram:
  TH1D diff( "diff", "diff",
	     nBins - 1,
	     lo + 0.5 * ( hi - lo ) / nBins,
	     hi - 0.5 * ( hi - lo ) / nBins );
  for ( int i=1; i<=diff.GetNbinsX(); ++i ){
    diff.SetBinContent( i, hist.GetBinContent( i+1 ) - hist.GetBinContent( i ) );
  }

  mean   = diff.GetMean();
  sigma  = diff.GetRMS();
  height = hist.GetBinContent( hist.GetMaximumBin() );

}

void AFEB::teststand::Results::fit( const double from, const double to ){
  // Fit the threshold scan's S-curve with a normal CDF.
  bsem_.take();
  double lo = pulses_->GetYaxis()->GetXmin();
  double hi = pulses_->GetYaxis()->GetXmax();
  bsem_.give();

  TH1D p( "pulses", "pulses", pulses_->GetNbinsY(), lo, hi ); 
  //TF1 nCDF( "normalCDF", &AFEB::teststand::Results::normalCDF, lo, hi, 3, NULL );
  TF1 nCDF( "normalCDF", &normalCDF, lo, hi, 3, NULL );
  nCDF.SetParNames( "mean", "sigma", "height" );
  double mean, sigma, height;
  nCDF.SetParameters( 0.5 * ( hi + lo ),
		      0.1 * ( hi - lo ),
		      measurement_->getNPulses() );
  if ( to != from ) nCDF.SetRange( from, to ); // fit only in the range measured so far

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

    estimateFitParameters( p, mean, sigma, height );
    nCDF.SetParameters( mean, sigma, height );
    p.Fit( &nCDF, "QR" ); // quiet (no printing), use function range
    // if ( iChannelBin == testedDevice_->getNChannels() ){
    //   TFile f( (testedDevice_->getType()+"__"+testedDevice_->getId()+"_test.root").c_str(), "recreate" );
    //   f.cd();
    //   for ( int i=1; i<=p.GetNbinsX(); ++i ) cout << p.GetBinContent( i ) << " ";
    //   cout << endl;
    //   p.Write();
    //   f.Close();
    // }

    bsem_.take();
    threshold_ ->SetBinContent( iChannelBin, nCDF.GetParameter( 0 ) );
    noise_     ->SetBinContent( iChannelBin, nCDF.GetParameter( 1 ) );
    efficiency_->SetBinContent( iChannelBin, nCDF.GetParameter( 2 ) / measurement_->getNPulses() );
    threshold_ ->SetBinError( iChannelBin, nCDF.GetParError( 0 ) );
    noise_     ->SetBinError( iChannelBin, nCDF.GetParError( 1 ) );
    efficiency_->SetBinError( iChannelBin, nCDF.GetParError( 2 ) / measurement_->getNPulses() );    
    bsem_.give();
  }
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

  TCanvas c( fileName_.c_str(), measurement_->getName().c_str(), 500, 500 );
  TLatex latex;
  c.Divide( 1, 2, 0., 0. );

  c.cd( 0 );
  latex.SetTextSize( 0.025 );
  latex.SetTextAlign(23);  // h centered, v top aligned
  latex.DrawLatex( 0.5, 0.99, pulses_->GetTitle() );

  c.cd( 1 );
  gPad->SetRightMargin( 0.07 );
  gPad->SetGridx();
  gPad->SetFrameFillColor( kGray + 3 );
  pulses.SetTitle("");
  pulses.Draw("colz");

  c.cd( 2 );
  gPad->SetRightMargin( 0.07 );
  gPad->SetGridx();
  gPad->SetGridy();
  threshold.SetTitle("");
  threshold.SetYTitle("threshold (circles) and noise (squares)");
  threshold.Draw();
  noise.Draw("same");
  TGaxis *axis = adjustToHistogram( &threshold, &efficiency );
  axis->Draw();
  efficiency.Draw("same");
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
  createFigure( directory );
  times_->Print();
  TFile f( (directory + "/" + testedDevice_->getType()+"__"+testedDevice_->getId()+".root").c_str(), "UPDATE", "", 5 );
  f.cd();

  bsem_.take();
  times_->Write();
  pulses_->Write();
  threshold_->Write();
  noise_->Write();
  efficiency_->Write();
  bsem_.give();

  f.Close();
}

map<string,pair<double,double> > AFEB::teststand::Results::getParameters( const int channel ) const {
  map<string,pair<double,double> > values;
  values["threshold" ] = make_pair<double,double>( threshold_ ->GetBinContent( channel+1 ),
						   threshold_ ->GetBinError  ( channel+1 ) );
  values["noise"     ] = make_pair<double,double>( noise_     ->GetBinContent( channel+1 ),
						   noise_     ->GetBinError  ( channel+1 ) );
  values["efficiency"] = make_pair<double,double>( efficiency_->GetBinContent( channel+1 ),
						   efficiency_->GetBinError  ( channel+1 ) );
  return values;
}
