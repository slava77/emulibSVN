#include "emu/dqm/calibration/Test_25_ALCTTrigger.h"
#include <boost/algorithm/string.hpp>


using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;
using namespace boost::algorithm;

Test_25_ALCTTrigger::Test_25_ALCTTrigger(std::string dfile): Test_Generic(dfile)
{
  testID    = "25_ALCTTrigger";
  nExpectedEvents   = 14000;
  dduID     = 0;
  //  binCheckMask=0x16CFF3F6;
  binCheckMask  = 0x1FEBF3F6;
  //  binCheckMask=0xF7CB3BF6;
  ltc_bug   = 2;
  logger = Logger::getInstance(testID);

}


void Test_25_ALCTTrigger::initCSC(std::string cscID)
{
  //= Init per CSC events counters
  nCSCEvents[cscID]=0;

  ThresholdScanDataA thdata;
  thdata.Nbins = getNumWireGroups(cscID);
  thdata.Nlayers = 6;

  memset(thdata.content, 0, sizeof (thdata.content));

  tscan_data[cscID] = thdata;

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

  for (int i=0; i<TEST_DATA2D_NLAYERS; i++)
    for (int j=0; j<TEST_DATA2D_NBINS; j++) afebdata.content[i][j]=0.;


  cscdata["R01"]=afebdata;

  tdata[cscID] = cscdata;

  bookTestsForCSC(cscID);


}

void Test_25_ALCTTrigger::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
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

  if (dduID != (bin_checker.dduSourceID()&0xFF))
  {

    LOG4CPLUS_DEBUG(logger, "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << (bin_checker.dduSourceID()&0xFF) << " First event");
    dduID = bin_checker.dduSourceID()&0xFF;
    dduL1A[dduID]=0;
    DDUstats[dduID].evt_cntr=0;
    DDUstats[dduID].first_l1a=-1;
  }

  dduID = bin_checker.dduSourceID()&0xFF;
  DDUstats[dduID].evt_cntr++;

  if (bin_checker.errors() != 0)
  {
    // std::cout << "Evt#" << std::dec << nTotalEvents << ": Nonzero Binary Errors Status is observed: 0x"<< std::hex << bin_checker.errors()
    //  << " mask:0x" << std::hex << binCheckMask << std::dec << std::endl;
    doBinCheck();
    //    return;
  }

  CSCDDUEventData dduData((uint16_t *) data, &bin_checker);

  currL1A=(int)(dduData.header().lvl1num());
  if (DDUstats[dduID].evt_cntr ==1)
  {
    DDUstats[dduID].first_l1a = currL1A;
    LOG4CPLUS_DEBUG(logger, "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << dduID
                    << " First L1A:" << DDUstats[dduID].first_l1a);
  }
  else if (DDUstats[dduID].first_l1a==-1)
  {
    DDUstats[dduID].first_l1a = currL1A-DDUstats[dduID].evt_cntr+1;
    LOG4CPLUS_DEBUG(logger, "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << dduID
                    << " First L1A :" << DDUstats[dduID].first_l1a << " after "
                    << currL1A-DDUstats[dduID].evt_cntr << " bad events");
  }

  DDUstats[dduID].l1a_cntr=currL1A;

  if ((DDUstats[dduID].l1a_cntr-DDUstats[dduID].first_l1a) != (DDUstats[dduID].evt_cntr-1))
  {
    LOG4CPLUS_WARN(logger, "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << dduID
                   << " Desynched L1A: " << ((DDUstats[dduID].l1a_cntr-DDUstats[dduID].first_l1a) - (DDUstats[dduID].evt_cntr-1)));
  }


  std::vector<CSCEventData> chamberDatas;
  chamberDatas = dduData.cscData();
  CSCDDUHeader dduHeader  = dduData.header();


  // std::cout << nTotalEvents << " " << chamberDatas.size() << std::endl;

  if (chamberDatas.size() >0)
  {
    DDUstats[dduID].csc_evt_cntr++;
  }
  else
  {
    DDUstats[dduID].empty_evt_cntr++;
  }

  // === set ltc_bug=2 in case of LTC double L1A bug
  // TODO: automatic detection of LTC L1A bug
  //  int ltc_bug=1;
  if (DDUstats[dduID].evt_cntr == 8)
    if  (DDUstats[dduID].empty_evt_cntr==0)
    {
      LOG4CPLUS_INFO(logger, "No LTC/TTC double L1A bug in data");
      // ltc_bug=1;
    }
    else
    {
      LOG4CPLUS_WARN(logger, "Found LTC/TTC double L1A bug in data");
    }


  int nCSCs = chamberDatas.size();

  //= Loop over found chambers and fill data structures
  for (int i=0; i < nCSCs; i++)
  {
    analyzeCSC(chamberDatas[i]);
  }

  DDUstats[dduID].last_empty=chamberDatas.size();

}


void Test_25_ALCTTrigger::analyzeCSC(const CSCEventData& data)
{


  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
  if (!dmbHeader && !dmbTrailer)
  {
    return;
  }


  int csctype=0, cscposition=0;
  std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition);
  // std::string cscID(Form("CSC_%03d_%02d", data.dmbHeader().crateID(), data.dmbHeader().dmbID()));
  // == Do not process unmapped CSCs and ME1/1 chambers
  if (cscID == "") return;
  // if ((cscID.find("ME+1.1") == 0) || (cscID.find("ME-1.1") ==0) ) return;


  cscTestData::iterator td_itr = tdata.find(cscID);
  if ( (td_itr == tdata.end()) || (tdata.size() == 0) )
  {
    LOG4CPLUS_INFO(logger, "Found " << cscID);
    initCSC(cscID);
    addCSCtoMap(cscID, dmbHeader->crateID(), dmbHeader->dmbID());
  }
  nCSCEvents[cscID]++;

  // std::cout << nCSCEvents[cscID] << " " << cscID << std::endl;
  // == Define aliases to access chamber specific data

  MonHistos& cschistos = mhistos[cscID];

  test_step& tstep = htree[dduID][cscID];// test_steps[cscID];

  ThresholdScanDataA& thdata = tscan_data[cscID];

  tstep.evt_cnt++;

  if (data.nalct())
  {
    const CSCAnodeData* alctData = data.alctData();
    if (alctData)
    {
      
    }
  }
}


void Test_25_ALCTTrigger::finishCSC(std::string cscID)
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
    

  }
}

bool Test_25_ALCTTrigger::checkResults(std::string cscID)
{
  bool isValid=true;
  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end())
  {
    // TestData& cscdata= td_itr->second;
    // TestData2D& r01 = cscdata["R01"];

    // int badChannels=0;
  }

  return isValid;
}
