#include "emu/dqm/calibration/Test_21_CFEBComparatorLogic.h"

using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;


Test_21_CFEBComparatorLogic::Test_21_CFEBComparatorLogic(std::string dfile):
  Test_Generic(dfile)
{
  testID = "21_CFEBComparatorLogic";

  //need to load from xml at some point
  events_per_hstrip = 250;
  hstrips_per_run = 32;
  hstrip_step = 1;
  hstrip_first =1;

  nExpectedEvents = events_per_hstrip*hstrips_per_run;

  //do we need this?  binCheckMask=0x16EBF7F6; // same value used in Lisa's test16

  logger = Logger::getInstance(testID);
  
}

Test_21_CFEBComparatorLogic::~Test_21_CFEBComparatorLogic()
{
}

void Test_21_CFEBComparatorLogic::initCSC(std::string cscID)
{
 
  //= Init per CSC events counters
  nCSCEvents[cscID]=0;

  //= Init data structures for storing data and results
  int nLayers = NLAYERS;
  
  
  //isME11() in emu::dqm::utils uses cscID.find("ME+1/1")
  // instead of cscID.find("ME+1.1") so it doesn't work
  bool isME11 = false;
  if ((cscID.find("ME+1.1") == 0) || (cscID.find("ME-1.1") ==0 )) {
    isME11 = true;
  }
 
  if(isME11) 
  {
	cout << "is ME11-type chamber" << endl;
  }

  int nBins = getNumStrips(cscID)*2;

  tdata[cscID]["R01"].Nlayers = nLayers;
  tdata[cscID]["R01"].Nbins = nBins;

  // Channels mask
  if(tmasks.find(cscID) != tmasks.end())
    {
      tdata[cscID]["_MASK"] = tmasks[cscID];
    } else 
    {
      tdata[cscID]["_MASK"].Nlayers = nLayers;
      tdata[cscID]["_MASK"].Nbins = nBins;
    }
	
	for(int i = 0; i < nLayers; i++) {
	  for(int j = 0; j < nBins; j++) {
		tdata[cscID]["_MASK"].content[i][j] = (j == 0 || j==(nBins-1)) ? 2 : 0;
	  } // use mask to only pass layer/plane 4 through
	}
      
  //initialize local arrays here as well
  TestData2D& ldata = halfStrips[cscID];
  ldata.clearArrays();
  ldata.Nlayers = nLayers;
  ldata.Nbins = nBins;

  ldata = halfStripsMultiple[cscID];
  ldata.clearArrays();
  ldata.Nlayers = nLayers;
  ldata.Nbins = nBins;

  ldata = halfStripsFake[cscID];
  ldata.clearArrays();
  ldata.Nlayers = nLayers;
  ldata.Nbins = nBins;

  
  bookTestsForCSC(cscID);
}

void Test_21_CFEBComparatorLogic::analyzeCSC(const CSCEventData& data)
{
  
  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
  if (!dmbHeader && !dmbTrailer)
    {
      return;
    }
  
  int csctype=0, cscposition=0;
  std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition);
  if (cscID == "")  return;
  


  cscTestData::iterator td_itr = tdata.find(cscID);
  if ( (td_itr == tdata.end()) || (tdata.size() == 0) )
    {
      LOG4CPLUS_INFO(logger, "Found " << cscID);
      initCSC(cscID);
      addCSCtoMap(cscID, dmbHeader->crateID(), dmbHeader->dmbID());
    }
  
  
  if(nCSCEvents[cscID] >= nExpectedEvents) 
    {
      LOG4CPLUS_WARN(logger, nCSCEvents[cscID] << " exceeded expected number of events (" << nExpectedEvents << ")");
    }
  
  nCSCEvents[cscID]++;
  

  int NCFEBs = getNumStrips(cscID)/16;
  TestData2D& chamber_halfStrips = halfStrips[cscID];
  TestData2D& chamber_halfStripsMultiple = halfStripsMultiple[cscID];
  TestData2D& chamber_halfStripsFake = halfStripsFake[cscID];
  
  //  LOG4CPLUS_INFO(logger, "got data with size "<<data.size()<<" nclct "<<data.nclct()<<" nalct "<<data.nalct());

  int nCLCTs = data.nclct();
  if (nCLCTs == 0 )
    {
      LOG4CPLUS_ERROR(logger, "nCLCTs == 0 ");
      return;
    }
  CSCCLCTData * clctData = data.clctData(); 

  //figure out the expected halfstrip within one CFEB:
  int current_hstrip = (nCSCEvents[cscID]-1)/events_per_hstrip + hstrip_first; //counts start from 1
  if (current_hstrip< hstrip_first || current_hstrip > hstrips_per_run)
    {
      LOG4CPLUS_ERROR(logger, " unexpected half-strip index (" << current_hstrip << ")");
    }
  
  for (int icfeb=0; icfeb<NCFEBs; icfeb++)   // loop over cfebs in a given chamber
    {
      //layer-index of the current halfstrip: index starts from 0
      int icurrent_hstrip = current_hstrip -1 + icfeb*32; //counts start from 0

      for(int ilayer = 1; ilayer <= 6; ilayer++)		  
	{
	  chamber_halfStrips.cnts[ilayer-1][icurrent_hstrip]++;// shifts are annoying
	  chamber_halfStripsMultiple.cnts[ilayer-1][icurrent_hstrip]++;// shifts are annoying
	  int nOnCurrentStrip = 0;

	  std::vector<CSCComparatorDigi> comparatorDigis(clctData->comparatorDigis(ilayer, icfeb));
	  int nComparatorDigis = comparatorDigis.size();
	  //	  LOG4CPLUS_INFO(logger, "nComparatorDigis " << nComparatorDigis <<" for expected halfstrip "
	  //			 <<current_hstrip<<" layer "<<ilayer<<" cfeb "<<icfeb);
	  for (int iComp = 0; iComp< nComparatorDigis; ++iComp)
	    {
	      const CSCComparatorDigi& compDigi=comparatorDigis[iComp];
	      //	      LOG4CPLUS_INFO(logger, "iComp "<<iComp<<" strip "<< compDigi.getStrip()<<" comparator "<<compDigi.getComparator());
	      int compHalfStrip = (compDigi.getStrip()-1)*2+compDigi.getComparator();
	      //	      LOG4CPLUS_INFO(logger, "compHalfStrip "<<compHalfStrip<<" for "<<icurrent_hstrip);
	      if (icurrent_hstrip == compHalfStrip )
		{
		  nOnCurrentStrip++;
		} 
	      else 
		{
		  chamber_halfStripsFake.cnts[ilayer-1][compHalfStrip]++;
		  chamber_halfStripsFake.content[ilayer-1][compHalfStrip]+=1.0;
		}
	    }//iComp< nComparatorDigis
	  if (nOnCurrentStrip==1) chamber_halfStrips.content[ilayer-1][icurrent_hstrip]+=1.0;
	  if (nOnCurrentStrip>1)
	    {
	      LOG4CPLUS_INFO(logger, "multiple ( "<< nOnCurrentStrip<<" ) comparator digis on halfstrip "
			     <<icurrent_hstrip<<" ilayer "<<ilayer);
	      chamber_halfStripsMultiple.content[ilayer-1][icurrent_hstrip]+=1.*nOnCurrentStrip;
	    }
	  
	}//ilayer <= 6
    }//icfeb<NCFEBs
}


void Test_21_CFEBComparatorLogic::finishCSC(std::string cscID)
{
		  
  cscTestData::iterator td_itr =  tdata.find(cscID);
  TestData& cscdata = tdata[cscID];
  
  TestData2D& r01 = cscdata["R01"];

  TestData2D& chamber_halfStrips = halfStrips[cscID];

  for(int i = 0; i<TEST_DATA2D_NLAYERS; i++) 
    {
      for(int j = 0; j<TEST_DATA2D_NBINS; j++) 
	{
	  //fill the efficiency
	  int cnt = chamber_halfStrips.cnts[i][j];
	  double den = cnt;
	  double num = chamber_halfStrips.content[i][j];
	  double eff = den > 0 ? num/den : num > 0 ? 42.0 : 0.0;
	  r01.content[i][j] = eff;
	  r01.cnts[i][j] = cnt;
	}//j<TEST_DATA2D_NBINS
    }//i<TEST_DATA2D_NLAYERS
  
}

bool Test_21_CFEBComparatorLogic::checkResults(std::string cscID)
{
  return true;
}
