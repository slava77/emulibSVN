#include "AFEB/teststand/Results.h"
#include "TF1.h"
#include "TMath.h"
#include "Math/SpecFuncMathCore.h" // for erf()

#include <sstream>

using namespace AFEB::teststand;

AFEB::teststand::Results::Results( const Measurement* const measurement, const TestedDevice* const device  ) :
  bsem_( toolbox::BSem::EMPTY ), // locked
  measurement_( measurement ),
  testedDevice_( device ){

  // pulses( channel, amplitude)  2D histogram
  stringstream name;
  name << "pulses__" << measurement_->getType() << "__" << testedDevice_->getId();
  stringstream title;
  title << "Number of pulses out of " << measurement_->getNPulses()
	<< " in " << testedDevice_->getType()
	<< " of id "  << testedDevice_->getId()
	<< " in a '" << measurement_->getName() 
	<< "' measurement ";
  int yspan = 
    ( measurement_->getAmplitudeMax() - measurement_->getAmplitudeMin() ) -
    ( measurement_->getAmplitudeMax() - measurement_->getAmplitudeMin() ) %  measurement_->getAmplitudeStep();
  int ny = yspan / measurement_->getAmplitudeStep() + 1;
  pulses_ = new TH2D( name.str().c_str(),
		      title.str().c_str(),
		      testedDevice_->getNChannels(),
		      0.5,
		      0.5 + testedDevice_->getNChannels(),
		      ny,
		      measurement_->getAmplitudeMin() -        0.5   * measurement_->getAmplitudeStep(),
		      measurement_->getAmplitudeMin() + ( ny - 0.5 ) * measurement_->getAmplitudeStep()
		      );

  // measured threshold( channel ) 1D histogram
  name.str("");
  name << "threshold__" << measurement_->getType() << "__" << testedDevice_->getId();
  title.str("");
  title << "measured threshold for " << testedDevice_->getType()
	<< " of id "  << testedDevice_->getId();
  threshold_ = new TH1D( name.str().c_str(),
			 title.str().c_str(),
			 testedDevice_->getNChannels(),
			 0.5,
			 0.5 + testedDevice_->getNChannels() );

  // measured noise( channel ) 1D histogram
  name.str("");
  name << "noise__" << measurement_->getType() << "__" << testedDevice_->getId();
  title.str("");
  title << "measured noise for " << testedDevice_->getType()
	<< " of id "  << testedDevice_->getId();
  noise_ = new TH1D( name.str().c_str(),
		     title.str().c_str(),
		     testedDevice_->getNChannels(),
		     0.5,
		     0.5 + testedDevice_->getNChannels() );

  // measured efficiency( channel ) 1D histogram
  name.str("");
  name << "efficiency__" << measurement_->getType() << "__" << testedDevice_->getId();
  title.str("");
  title << "measured efficiency for " << testedDevice_->getType()
	<< " of id "  << testedDevice_->getId();
  efficiency_ = new TH1D( name.str().c_str(),
			  title.str().c_str(),
			  testedDevice_->getNChannels(),
			  0.5,
			  0.5 + testedDevice_->getNChannels() );

  // ( channel, amplitude, time ) ntuple
  name.str("");
  name << "times__" << measurement_->getType() << "__" << testedDevice_->getId();
  title.str("");
  title << "TDC times " << measurement_->getNPulses()
	<< " for " << testedDevice_->getType()
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

void AFEB::teststand::Results::createFigure(){
  double lo = pulses_->GetYaxis()->GetXmin();
  double hi = pulses_->GetYaxis()->GetXmax();
  TH1D p( "pulses", "pulses", pulses_->GetNbinsY(), lo, hi ); 
  //TF1 nCDF( "normalCDF", &AFEB::teststand::Results::normalCDF, lo, hi, 3, NULL );
  TF1 nCDF( "normalCDF", &normalCDF, lo, hi, 3, NULL );
  nCDF.SetParNames( "mean", "sigma", "height" );
  nCDF.SetParameters( 0.5 * ( hi + lo ),
		      0.1 * ( hi - lo ),
		      measurement_->getNPulses() );

  // Loop over channels and fit results
  for ( int iChannel = 1; iChannel <= testedDevice_->getNChannels(); ++iChannel ){
    p.Reset();
    for ( int iAmp = 1; iAmp <= pulses_->GetNbinsY(); ++iAmp ){
      p.Fill( pulses_->GetXaxis()->GetBinCenter( iChannel ), 
	      pulses_->GetBinContent( iChannel, iAmp ) );
      cout << " " << iChannel
	   << " " << iAmp
	   << " " << pulses_->GetXaxis()->GetBinCenter( iChannel )
	   << " " << pulses_->GetBinContent( iChannel, iAmp )
	   << endl;
    }
    p.Fit( &nCDF, "V" );
    threshold_ ->Fill( iChannel, nCDF.GetParameter( "mean"   ) );
    noise_     ->Fill( iChannel, nCDF.GetParameter( "sigma"  ) );
    efficiency_->Fill( iChannel, nCDF.GetParameter( "height" ) );
  }
  // TODO: pretty plot
}

void AFEB::teststand::Results::save(){
  createFigure();
  times_->Print();
  TFile f( (testedDevice_->getType()+"__"+testedDevice_->getId()+".root").c_str(), "recreate" );
  f.cd();
  times_->Write();
  pulses_->Write();
  threshold_->Write();
  noise_->Write();
  efficiency_->Write();
  f.Close();
}
