#include "emu/dqm/calibration/Test_16_CFEBConnectivity.h"

using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;


Test_16_CFEBConnectivity::Test_16_CFEBConnectivity(std::string dfile):
  Test_Generic(dfile)
{
  testID = "16_CFEBConnectivity";
  nExpectedEvents = 3000;
  //  binCheckMask=0x16CFF3F6;
  //  binCheckMask=0x1FEBF3F6;
  binCheckMask=0x16EBF7F6; // same value used in Lisa's test16
  //  binCheckMask=0xF7CB3BF6;
  logger = Logger::getInstance(testID);
  
  min_adc_hist = new TH1F("min adc","min_adc_hist",16,0.0,16.0);
  max_adc_hist = new TH1F("max adc","max_adc_hist",16,0.0,16.0);

}

Test_16_CFEBConnectivity::~Test_16_CFEBConnectivity()
{
	delete min_adc_hist;
	delete max_adc_hist;
}

void Test_16_CFEBConnectivity::initCSC(std::string cscID)
{

 
  //= Init per CSC events counters
  nCSCEvents[cscID]=0;

  //= Init data structures for storing SCA data and results
  if (adcSum2.find(cscID) == adcSum2.end())
  {
	  TimeBinsTestData empty;
	  adcSum2[cscID] = empty;
  }
  if (adcSum.find(cscID) == adcSum.end())
  {
  	  TimeBinsTestData empty;
  	  adcSum[cscID] = empty;
  }
  for (int t = 1; t<16; ++t) // no need to store time bin 0
  {
	  TestData2D tb_data;
	  tb_data.Nbins = getNumStrips(cscID);
	  tb_data.Nlayers = NLAYERS;
	  memset(tb_data.content, 0, sizeof (tb_data.content));
	  adcSum[cscID][t] = tb_data;
	  adcSum2[cscID][t] = tb_data;
  }

  TestData cscdata;
  TestData2D cfebdata;
  cfebdata.Nbins = getNumStrips(cscID);
  cfebdata.Nlayers = NLAYERS;
  memset(cfebdata.content, 0, sizeof (cfebdata.content));
  memset(cfebdata.cnts, 0, sizeof (cfebdata.cnts));
  
  // Channels mask
  if (tmasks.find(cscID) != tmasks.end())
    {
      cscdata["_MASK"]=tmasks[cscID];
    }
  else
    {
      cscdata["_MASK"]=cfebdata;
    }

  for (int i=0; i<TEST_DATA2D_NLAYERS; i++)
    for (int j=0; j<TEST_DATA2D_NBINS; j++) cfebdata.content[i][j]=0.;


  
  cscdata["R01"] = cfebdata;


  tdata[cscID] = cscdata;

  bookTestsForCSC(cscID);
}

void Test_16_CFEBConnectivity::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
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


void Test_16_CFEBConnectivity::analyzeCSC(const CSCEventData& data)
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
  
  //cout << "cscID: " << cscID << endl;


  cscTestData::iterator td_itr = tdata.find(cscID);
  if ( (td_itr == tdata.end()) || (tdata.size() == 0) )
    {
      LOG4CPLUS_INFO(logger, "Found " << cscID);
      initCSC(cscID);
      addCSCtoMap(cscID, dmbHeader->crateID(), dmbHeader->dmbID());
    }
  nCSCEvents[cscID]++;


  TimeBinsTestData& ch_adc_sum = adcSum[cscID];
  TimeBinsTestData& ch_adc_sum2 = adcSum2[cscID];
  
  //"PASS 0"
  if (dmbHeader->cfebAvailable())
    {
      for (int icfeb=0; icfeb<getNumStrips(cscID)/16; icfeb++)   // loop over cfebs in a given chamber
      {
        CSCCFEBData * cfebData =  data.cfebData(icfeb);
        if (!cfebData) continue;

        for (int ilayer = 1; ilayer <= NLAYERS; ilayer++)   // loop over layers in a given chamber
        {
          int nTimeSamples= cfebData->nTimeSamples();
          
          for (int istrip = 1; istrip <= 16; istrip++)   // loop over cfeb strips
          {
          
        	  int adc_max = -1;
        	  int adc_min = 4096;
        	  
        	  //if(icfeb == 1 && ilayer == 1) cout << "nTimeSamples: " << nTimeSamples << endl;
        	  
        	  // loop for calculating min&max bins later
        	  int jmin = -1, jmax = 16;
        	  for (int j=0; j<nTimeSamples; j++)
        	  {
        		  CSCCFEBDataWord* timeSample=(cfebData->timeSlice(j))->timeSample(ilayer,istrip);
        		  
        		  
        		  if(timeSample->adcCounts > adc_max)
					  {
						  adc_max = timeSample->adcCounts;
						  jmax = j;
					  }
					  if(timeSample->adcCounts < adc_min)
					  {
						  adc_min = timeSample->adcCounts;
						  jmin = j;
					  }
        	  }
        	  
        	  /*if(ilayer == 1 && istrip == 1) 
        	  {
        		  printf("jmin: %i, adc_min: %i, jmax: %i, adc_max: %i\n", jmin,adc_min,jmax,adc_max);
        	  }*/

        	  
        	  min_adc_hist->Fill(jmin);
        	  max_adc_hist->Fill(jmax);

        	  // loop for storing the sums and sums of squares of counts above pedestal 
              int pedestal = (cfebData->timeSlice(0))->timeSample(ilayer,istrip)->adcCounts;
        	  for (int j=1; j<nTimeSamples; j++)
        	  {
                  int adc_count = (cfebData->timeSlice(j))->timeSample(ilayer,istrip)->adcCounts;
                  double count_diff = (double) adc_count - (double) pedestal;
                  
                  /*if(isnan(count_diff)) {
                	  cout << "####count_diff was nan!####" << endl;
                	  count_diff = 0.0;
                  }*/
                                    
                  int strip_idx = icfeb*16 + istrip - 1;
                  //cout << "count_diff" << count_diff << endl;
                  ch_adc_sum[j].content[ilayer-1][strip_idx] += count_diff;
                  ch_adc_sum[j].cnts[ilayer-1][strip_idx] += 1;
                  ch_adc_sum2[j].content[ilayer-1][strip_idx] += count_diff*count_diff;
                  ch_adc_sum2[j].cnts[ilayer-1][strip_idx] += 1;   
        	  }          
          }
        }
      }
    }
}


void Test_16_CFEBConnectivity::finishCSC(std::string cscID)
{
		  
	cscTestData::iterator td_itr =  tdata.find(cscID);
	TestData& cscdata = tdata[cscID];
	
    TestData2D& r01 = cscdata["R01"];
    	
	// hist bin starts at 1, jmin/jmax at 0.
	int i_max = max_adc_hist->GetMaximumBin() - 1;
	int i_min = min_adc_hist->GetMaximumBin() - 1;

	int tsmax[2] = {i_max - 2, i_max + 2};
    if (tsmax[0] <= 0) tsmax[0] =  1;
    if (tsmax[1] > 16) tsmax[1] = 15;
    printf("Timesample with ADC min is %d, with ADC max is %d\n", tsmax[0],
           tsmax[1]);

	cout << "avg max/peak time bin: " << i_max << endl;
	cout << "avg min time bin: " << i_min << endl;
	//printf("i_min: %d, min: %d, i_max: %d, max: %d\n", i_min,min,i_max,max);
	
	double sum[TEST_DATA2D_NLAYERS][TEST_DATA2D_NBINS];
	double sumsq[TEST_DATA2D_NLAYERS][TEST_DATA2D_NBINS];
	int nevents[TEST_DATA2D_NLAYERS][TEST_DATA2D_NBINS];

	TimeBinsTestData& ch_adc_sum = adcSum[cscID];
	TimeBinsTestData& ch_adc_sum2 = adcSum2[cscID];
	for (int ilayer=0; ilayer < TEST_DATA2D_NLAYERS; ++ilayer)
	{
		for (int istrip = 0; istrip < TEST_DATA2D_NBINS; ++istrip)
		{
			for (int i = tsmax[0]; i<=tsmax[1]; i++)
			{
				sum[ilayer][istrip] += ch_adc_sum[i].content[ilayer][istrip];
				sumsq[ilayer][istrip] += ch_adc_sum2[i].content[ilayer][istrip];
				nevents[ilayer][istrip] += ch_adc_sum[i].cnts[ilayer][istrip];
			}
		}
	}

	
	// the stuff below is ported from test_16_finish
	int     ilayer, istrip, n;
	float   avg, sigma_sq;

	int nstrips = ch_adc_sum[1].Nbins;
	double adc_diff[TEST_DATA2D_NLAYERS][TEST_DATA2D_NBINS];
	double adc_diff_err[TEST_DATA2D_NLAYERS][TEST_DATA2D_NBINS];

	
	/* Get average max response - min response */
	for (ilayer = 0; ilayer < TEST_DATA2D_NLAYERS; ilayer++)
	{
		for (istrip = 0; istrip < nstrips; istrip++)
		{

			r01.cnts[ilayer][istrip]++;
			
			if ((n = nevents[ilayer][istrip]) > 1) 
			{

				//cout << "sum[ilayer][istrip]: " << sum[ilayer][istrip] << endl;
				//cout << "n: " << n << endl;
				avg = sum[ilayer][istrip] / n;
				if(isnan(avg)) {
					cout << "###avg was nan!####" << endl;
					//sum[ilayer][istrip] is nan sometimes!
					cout << "sum[ilayer][istrip]: " << sum[ilayer][istrip] << endl;
					cout << "ilayer: " << ilayer << endl;
					cout << "istrip: " << istrip << endl;
					cout << "n: " << n << endl;
					avg = -999.;
				}
				//cout << "avg:" << avg << endl;
				sigma_sq = (n / (n - 1)) * (sumsq[ilayer][istrip] / n - avg * avg);
				adc_diff[ilayer][istrip] = avg;
				adc_diff_err[ilayer][istrip] = sqrt(sigma_sq / n);
				r01.content[ilayer][istrip] = avg;
			}
			else if (n == 1)
			{
				adc_diff[ilayer][istrip]  = sum[ilayer][istrip];
				adc_diff_err[ilayer][istrip] = 10.;
				r01.content[ilayer][istrip] = sum[ilayer][istrip];
			}
			else
			{
				adc_diff[ilayer][istrip] = -999.;
				adc_diff_err[ilayer][istrip] = 0.;
				r01.content[ilayer][istrip] = -999.;
			}
		}
	}
	
}

bool Test_16_CFEBConnectivity::checkResults(std::string cscID)
{
  return true;
}
