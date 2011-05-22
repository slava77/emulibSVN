#include "AFEB/teststand/Results.h"

#include <sstream>

using namespace AFEB::teststand;

AFEB::teststand::Results::Results( const Measurement* const measurement, const TestedDevice* const device  ) :
  bsem_( toolbox::BSem::EMPTY ), // locked
  measurement_( measurement ),
  testedDevice_( device ){

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
}

void AFEB::teststand::Results::add( const int channel, int const amplitude, const int time ){
  bsem_.take();
  channel_   = channel;
  amplitude_ = amplitude;
  time_      = time;
  times_->Fill();
  bsem_.give();
}

void AFEB::teststand::Results::save(){
  times_->Print();
  TFile f( (testedDevice_->getType()+"__"+testedDevice_->getId()+".root").c_str(), "recreate" );
  f.cd();
  times_->Write();
  f.Close();
}
