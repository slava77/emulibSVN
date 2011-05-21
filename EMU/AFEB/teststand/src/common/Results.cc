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
  pulse_ = new TH2D( name.str().c_str(),
		     title.str().c_str(),
		     testedDevice_->getNChannels(),
		     0.5,
		     0.5 + testedDevice_->getNChannels(),
		     ny,
		     measurement_->getAmplitudeMin() -        0.5   * measurement_->getAmplitudeStep(),
		     measurement_->getAmplitudeMin() + ( ny - 0.5 ) * measurement_->getAmplitudeStep()
		     );
  
}

void AFEB::teststand::Results::add( int const amplitude, const vector<double>& times ){
  bsem_.take();

  bsem_.give();
}
