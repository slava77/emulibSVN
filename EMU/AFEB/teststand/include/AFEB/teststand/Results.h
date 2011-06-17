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
      void fit( const double from, const double to );
      void createFigure( const string directory, const double fitRangeStart=0., const double fitRangeEnd=0. );
      string getFileName() const { return fileName_; }
      map<string,pair<double,double> > getParameters( const int channel ) const;
      void save( const string directory );
    private:
      toolbox::BSem bsem_;
      const Measurement* const measurement_;
      const TestedDevice* const testedDevice_;
      string fileName_; ///< result file name without the extension
      TTree *times_; ///< ( channel, amplitude, time ) ntuple
      TH2D *pulses_; ///< pulses( channel, amplitude)  2D histogram
      TH1D *threshold_; ///< measured threshold( channel ) 1D histogram
      TH1D *noise_; ///< measured noise( channel ) 1D histogram
      TH1D *efficiency_; ///< measured efficiency( channel ) 1D histogram
      int channel_;
      int amplitude_;
      int time_;
      TGaxis* adjustToHistogram( const TH1* const h1, TH1* h2, bool isNewAxisOnRight=true );
      void estimateFitParameters( const TH1D& hist, double& mean, double& sigma, double& height );
    };
  }}

#endif
