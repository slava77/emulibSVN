#ifndef __AFEB_teststand_Results_h__
#define __AFEB_teststand_Results_h__

#include "toolbox/BSem.h"

#include "TH2D.h"
#include "TTree.h"
#include "TFile.h"
#include "TGaxis.h"

#include "AFEB/teststand/Measurement.h"

using namespace std;
using namespace AFEB::teststand;

namespace AFEB { namespace teststand {

    class Measurement;
    class TestedDevice;

    class Results{
    public:
      Results( const Measurement* const measurement, const TestedDevice* const device );
      ~Results();
      void add( const int channel, int const amplitude, const int time );
      void fitResults();
      void createFigure();
      void save();
    private:
      toolbox::BSem bsem_;
      const Measurement* const measurement_;
      const TestedDevice* const testedDevice_;
      TTree *times_; ///< ( channel, amplitude, time ) ntuple
      TH2D *pulses_; ///< pulses( channel, amplitude)  2D histogram
      TH1D *threshold_; ///< measured threshold( channel ) 1D histogram
      TH1D *noise_; ///< measured noise( channel ) 1D histogram
      TH1D *efficiency_; ///< measured efficiency( channel ) 1D histogram
      int channel_;
      int amplitude_;
      int time_;
      TGaxis* adjustToHistogram( const TH1* const h1, TH1* h2, bool isNewAxisOnRight=true );
    };
  }}

#endif
