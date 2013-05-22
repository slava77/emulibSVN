#include "emu/dqm/calibration/Test_11_AFEBNoise.h"
#include <boost/algorithm/string.hpp>


using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;
using namespace boost::algorithm;

Test_11_AFEBNoise::Test_11_AFEBNoise(std::string dfile): Test_Generic(dfile)
{
  testID    = "11_AFEBNoise";
  nExpectedEvents   = 15000;
  binCheckMask  = 0x1FEBF3F6;
  logger = Logger::getInstance(testID);

}

void Test_11_AFEBNoise::initCSC(std::string cscID)
{
  //= Init per CSC events counters
  nCSCEvents[cscID]=0;

  TestData cscdata;
  TestData2D afebdata;
  afebdata.Nbins = getNumWireGroups(cscID);
  afebdata.Nlayers = NLAYERS;
  memset(afebdata.content, 0, sizeof (afebdata.content));
  memset(afebdata.cnts, 0, sizeof (afebdata.cnts));


  // Channels mask
  if (tmasks.find(cscID) != tmasks.end())
  {
    cscdata["_MASK"]=tmasks[cscID];
  }
  else
  {
    cscdata["_MASK"]=afebdata;
  }
  

  for (int i=0; i<TEST_DATA2D_NLAYERS; i++) {
    for (int j=0; j<TEST_DATA2D_NBINS; j++) {
      afebdata.content[i][j]=0.;
      afebdata.cnts[i][j]=0;
	}
  }


  cscdata["R01"]=afebdata;
  cscdata["R02"]=afebdata;

  tdata[cscID] = cscdata;

  bookTestsForCSC(cscID);
  
  
}

void Test_11_AFEBNoise::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
{

  nTotalEvents++;

  //= Examiner checks
  const uint16_t *tmp = reinterpret_cast<const uint16_t *>(data);
  bin_checker.setMask( binCheckMask);
  if ( bin_checker.check(tmp,dataSize/sizeof(short)) < 0 )
  {
    //   No ddu trailer found - force checker to summarize errors by adding artificial trailer
    const uint16_t dduTrailer[4] = { 0x8000, 0x8000, 0xFFFF, 0x8000 };
    tmp = dduTrailer;
    bin_checker.check(tmp,uint32_t(4));
  }


  if (bin_checker.errors() != 0)
  {
    doBinCheck();
    //    return;
  }

  CSCDDUEventData dduData((uint16_t *) data, &bin_checker);

  std::vector<CSCEventData> chamberDatas;
  chamberDatas = dduData.cscData();
  CSCDDUHeader dduHeader  = dduData.header();



  int nCSCs = chamberDatas.size();

  //= Loop over found chambers and fill data structures
  for (int i=0; i < nCSCs; i++)
  {
    analyzeCSC(chamberDatas[i]);
  }


}


void Test_11_AFEBNoise::analyzeCSC(const CSCEventData& data)
{


  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
  if (!dmbHeader && !dmbTrailer)
  {
    return;
  }
  int csctype=0, cscposition=0;
  std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition);
  if (cscID == "") return;

  cscTestData::iterator td_itr = tdata.find(cscID);
  if ( (td_itr == tdata.end()) || (tdata.size() == 0) )
  {
    LOG4CPLUS_INFO(logger, "Found " << cscID);
    initCSC(cscID);
    addCSCtoMap(cscID, dmbHeader->crateID(), dmbHeader->dmbID());
  }
  nCSCEvents[cscID]++;
  
  int evtNum = nCSCEvents[cscID];

        
  TestData& cscdata = tdata[cscID];
	
  TestData2D& r01 = cscdata["R01"];
  TestData2D& r02 = cscdata["R02"];

  if (data.nalct())
  {
    
  }
}


void Test_11_AFEBNoise::finishCSC(std::string cscID)
{
  if (nCSCEvents[cscID] < nExpectedEvents)
  {
    LOG4CPLUS_ERROR(logger, Form("%s: Not enough events for test analysis (%d events)", cscID.c_str(), nCSCEvents[cscID] ));
    // = Set error
    return;
  }

  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end())
  {

    TestData& cscdata= td_itr->second;
    TestData2D& r01 = cscdata["R01"];
    TestData2D& r02 = cscdata["R02"];

	for(int i = 0; i < 6; i++) {
	  for(int j = 0; j < getNumWireGroups(cscID); j++) {
		  r01.content[i][j] = i+j;
		  r02.content[i][j] = i+j;
	  }
	}
    
  }
}

bool Test_11_AFEBNoise::checkResults(std::string cscID)
{
  bool isValid=true;
  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end())
  {
  
  }

  return isValid;
}
