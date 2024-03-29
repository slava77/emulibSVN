#ifndef __AFEB_teststand_Results_h__
#define __AFEB_teststand_Results_h__

#include "toolbox/BSem.h"

#include "TH2D.h"
#include "TProfile.h"
#include "TTree.h"
#include "TFile.h"
#include "TGaxis.h"
#include "TLegend.h"

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
      void refitSCurves( const string& resultDir );
      void updateOutput( const string directory, const double fitRangeStart, const double fitRangeEnd );
      string getFileName() const { return fileName_; }
      map<string,pair<double,double> > getParameters( const int channel ) const;
      void save( const string directory );
      void saveInOldFormat( const string directory );
      map<string,double> getThresholdStats();
      map<string,double> getNoiseStats();
      map<string,double> getChi2NDFStats();
      map<string,double> getTimeStats();
      map<string,double> getSlewStats();
      map<string,double> getTimeSpanStats();
      static string getFileName( const int measurementIndex, const string& measurementTypeString, const string& deviceId );
    private:
      toolbox::BSem bsem_;
      bool isFinal_; ///< Indicates that everything should be calculated, not just the plots.
      const Measurement* const measurement_;
      const TestedDevice* const testedDevice_;
      string fileName_; ///< result file name without the extension
      TFile *rootFile_;
      TTree *times_; ///< ( channel, amplitude, time ) ntuple
      TH2D *pulses_; ///< pulses( channel, amplitude)  2D histogram
      TH1D *threshold_; ///< measured threshold( channel ) 1D histogram
      TH1D *noise_; ///< measured noise( channel ) 1D histogram
      TH1D *efficiency_; ///< measured efficiency( channel ) 1D histogram
      TH1D *chi2ndf_; ///< threshold S-curve fit's chi^2/ndf( channel ) 1D histogram
      TH1D *timeMeanOnPlateau_;  ///< mean of the measured times on the efficiency plateau vs. channel
      TH1D *timeRMSOnPlateau_;  ///< RMS of the measured times on the efficiency plateau vs. channel
      TH1D *slewingOnPlateau_;  ///< slewing times (max difference between mean times) on the efficiency plateau vs. channel
      vector<TH1D*> sCurve_; ///< a measured efficiency( amplitude ) 1D histogram for each channel
      vector<TProfile*> timeVsAmplitudeProfile_;  ///< a measured time( amplitude ) 1D profile histogram for each channel
      vector<TH1D*> timeVsAmplitude_;  ///< a measured time( amplitude ) 1D histogram for each channel
      vector<double> timeSpans_; ///< max difference between any two channels' mean time for each amplitude on the efficiency plateau
      TLegend *legend_; ///< legend for S curves
      int channel_;		///< To access an entry in the times_ tree.
      int amplitude_;		///< To access an entry in the times_ tree.
      int time_;		///< To access an entry in the times_ tree.
      void bookHistograms();
      void deleteHistograms();
      void createFigure( const string directory, const double fitRangeStart, const double fitRangeEnd );
      void createLegend();
      TGaxis* adjustToHistogram( const TH1* const h1, TH1* h2, bool isNewAxisOnRight=true );
      void estimateFitParameters( TH1D& hist, const double from, const double to, double& mean, double& sigma, double& height );
      double mean( const TH1D& hist, const double from, const double to );
      double rms( const TH1D& hist, const double from, const double to );
      void timesOnEfficiencyPlateau( valarray<double>& plateauStarts );
      map<string,double> getStats( const TH1D* hist );
    };
  }}

#endif
