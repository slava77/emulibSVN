#ifndef _Test_25_ALCTTrigger_h_
#define _Test_25_ALCTTrigger_h_


#include <iostream>
#include <iomanip>
#include <math.h>
#include <map>
#include <sys/stat.h>

#include "emu/dqm/calibration/Test_Generic.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TProfile.h>
#include <TFile.h>


#define NMAXSCAN            2
#define MAX_CALIB_POINTS   256

typedef struct ddu_stats_afeb
{
  long first_l1a;
  int last_empty;
  long evt_cntr;
  long l1a_cntr;
  int empty_evt_cntr;
  int csc_evt_cntr;
  int pass;
  int thresh; 

} ddu_stats_afeb;

typedef struct ThresholdScanDataA
{
  int Nbins;
  int Nlayers;
  int content[NMAXSCAN][NLAYERS][MAX_WIREGROUPS][MAX_CALIB_POINTS];
} ThresholdScanDataA;

typedef std::map<std::string, ThresholdScanDataA>    cscThresholdScanDataA;


class Test_25_ALCTTrigger: public Test_Generic
{
public:
  Test_25_ALCTTrigger(std::string datafile);
  void analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber = 0);

protected:

  void initCSC(std::string cscID);
  void analyzeCSC(const CSCEventData& data);
  void finishCSC(std::string cscID);
  bool checkResults(std::string cscID);

  std::map<std::string, uint32_t> l1a_cntrs;
  std::map<int, int> dduL1A;
  int currL1A;
  int startL1A;
  int dduID;
  int ltc_bug;
  

  std::map<int, ddu_stats_afeb> DDUstats;
  std::map<int, std::map<std::string, test_step> > htree;
  cscThresholdScanDataA tscan_data;

  unsigned short alct_valid_patt[2];     /* ALCT valid pattern flag          */
  unsigned short alct_patt_quality[2];   /* ALCT pattern quality (0-3)       */
  unsigned short alct_accel_muon[2];     /* ALCT accelerator muon            */
  unsigned short alct_wire_group[2];     /* ALCT Wire-Gang ID (0-111)        */
  unsigned short alct_full_bxn;          /* ALCT Full Bunch Crossing Number  */
  
  
};

#endif
