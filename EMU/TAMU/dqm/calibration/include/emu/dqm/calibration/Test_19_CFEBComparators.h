#ifndef _Test_19_CFEBComparators_h_
#define _Test_19_CFEBComparators_h_

#include <iostream>
#include <iomanip>
#include <math.h>
#include <map>
#include <sys/stat.h>

#include "emu/dqm/calibration/Test_Generic.h"

#include "DataFormats/CSCDigi/interface/CSCComparatorDigi.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TFile.h>

#define NMAXSCANC             10 //root of problem, limits curr_pass at 5, causing segfault since not allocated to array
#define NMAXTHRESHC          40 
#define NMAXBUCKETC          31
#define NCFEB_CHIP            6
#define NCFEB                 5
#define MAX_CALIB_POINTSC   256

#define NLAYER 6
#define NSTRIP 112 //80?

// Injected pulse in fC is A0+A1*DAC_calibration_pulse
#define INJECT_PULSE_C0  0. 
#define INJECT_PULSE_C1  0.655 // for the new DAC 

typedef struct ddu_stats_cfeb_thresh
{
  long first_l1a;
  int last_empty;
  long evt_cntr;
  long l1a_cntr;
  int empty_evt_cntr;
  int csc_evt_cntr;
  int pass;
  int thresh; 

} ddu_stats_cfeb_thresh;




typedef struct ThresholdScanDataC
{
  int Nbins;
  int Nlayers;
  int content[NMAXSCANC][NLAYER][NSTRIP][MAX_CALIB_POINTSC];
} ThresholdScanDataC;

typedef std::map<std::string, ThresholdScanDataC> cscThresholdScanDataC;

///////move into test generic? -- it already has this for AFEBs
typedef struct CFEBCalibParams
{

  int Nbins;
  int Nlayers;
  double capacitances[NCFEB];
  double gains[NCFEB];
} CFEBCalibParams;

typedef std::map<std::string, CFEBCalibParams> cscCFEBCalibParams;
/////////////


class Test_19_CFEBComparators: public Test_Generic
{
  
public:
  Test_19_CFEBComparators(std::string datafile);
  ~Test_19_CFEBComparators();
  
  void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);

protected:

  void initCSC(std::string cscID);
  void analyzeCSC(const CSCEventData& data);
  void finishCSC(std::string cscID);
  bool checkResults(std::string cscID);
  bool loadCFEBCalibParams(std::string cscID);
  int calc_thresh(int npoints, int* content, float* par, float* chisq);

  std::map<std::string, uint32_t> l1a_cntrs;
  std::map<int, int> dduL1A;
  int currL1A;
  int startL1A;
  int dduID;
  int ltc_bug;
  
  int num_tpamps;
  int num_thresh;
  int first_thresh;
  int thresh_step;
  int ev_per_thresh;
  int pass;

  std::map<int, ddu_stats_cfeb_thresh> DDUstats;
  std::map<int, std::map<std::string, test_step> > htree;
  bool fSwitch;
  cscThresholdScanDataC tscan_data;
  cscCFEBCalibParams cfeb_cal_params;
  
  
private:
  
};

#endif
