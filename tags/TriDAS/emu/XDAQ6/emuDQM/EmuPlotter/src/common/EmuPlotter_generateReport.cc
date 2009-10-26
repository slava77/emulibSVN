#include "EmuPlotter.h"
#include "TClass.h"

/*
  Generate text file with DQM report
*/

int EmuPlotter::loadKillBitsConfig(std::string killbitsFile)
{
/*
  if (killbitsFile == "") {
    return 1;
  }

  XMLPlatformUtils::Initialize();
  XercesDOMParser *parser = new XercesDOMParser();
  parser->setValidationScheme(XercesDOMParser::Val_Always);
  parser->setDoNamespaces(true);
  parser->setDoSchema(true);
  parser->setValidationSchemaFullChecking(false); // this is default
  parser->setCreateEntityReferenceNodes(true);  // this is default
  parser->setIncludeIgnorableWhitespace (false);

  parser->parse(killbitsFile.c_str());
  DOMDocument *doc = parser->getDocument();
  DOMNodeList *l = doc->getElementsByTagName( XMLString::transcode("KillBits") );
  if( l->getLength() != 1 ){
    return 1;
  }

  DOMNodeList *itemList = doc->getElementsByTagName( XMLString::transcode("CSC") );
  if( itemList->getLength() == 0 ){
    return 1;
  }
  for(uint32_t i=0; i<itemList->getLength(); i++){
    std::map<std::string, std::string> obj_info;
    std::map<std::string, std::string>::iterator itr;
    DOMNodeList *children = itemList->item(i)->getChildNodes();
    for(unsigned int i=0; i<children->getLength(); i++){
      std::string paramname = std::string(XMLString::transcode(children->item(i)->getNodeName()));
      if ( children->item(i)->hasChildNodes() ) {
        std::string param = std::string(XMLString::transcode(children->item(i)->getFirstChild()->getNodeValue()));
        obj_info[paramname] = param;
      }
    }
    itr = obj_info.find("CSC");
    if (itr != obj_info.end()) {
      std::cout << "Found masks for " << itr->second << std::endl;
      if (obj_info["CFEBChans"] != "") {
        tmasks[itr->second]=parseMask(obj_info["CFEBChans"]);
      }
    }
  }
  delete parser;
*/
  return 0;
}


int getNumStrips(std::string cscID)
{
  if ((cscID.find("ME+1/3") == 0) || (cscID.find("ME-1/3") ==0 )) return 64;
  else return 80;
}

int getNumCFEBs(std::string cscID)
{
  if ((cscID.find("ME+1/3") == 0) || (cscID.find("ME-1/3") ==0 )) return 4;
  else return 5;
}

int getNumWireGroups(std::string cscID) 
{
  if ( (cscID.find("ME+4/1") == 0) || (cscID.find("ME-4/1") ==0) 
       || (cscID.find("ME+3/1") == 0) || (cscID.find("ME-3/1") == 0) ) return 96;
  else if ( (cscID.find("ME+2/1") == 0) || (cscID.find("ME-2/1") ==0)) return 112;
  else if ( (cscID.find("ME+1/1") == 0) || (cscID.find("ME-1/1") ==0)) return 48;
  else if ( (cscID.find("ME+1/3") == 0) || (cscID.find("ME-1/3") ==0)) return 32;
  else return 64;
}



std::string EmuPlotter::getCSCName(std::string cscID, int& crate, int& slot, int& CSCtype, int& CSCposition ) {
  //  int crate=0, slot=0;
  std::string cscName="";
  if (sscanf(cscID.c_str(), "CSC_%03d_%02d", &crate , &slot) == 2) {
    cscName=getCSCFromMap(crate,slot, CSCtype, CSCposition );
  }
  return cscName;
}


MonitorElement* EmuPlotter::findME(std::string tag, std::string name, TDirectory* rootfolder)
{
  MonitorElement* me = 0;
  EmuMonitoringObject* mo=0;

  if (rootfolder) {

    TKey *key = rootfolder->FindKey(tag.c_str());

    if (key && key->ReadObj()->IsA()->InheritsFrom( "TDirectory" ) ) {

      rootfolder->cd(tag.c_str());
      TDirectory* hdir= gDirectory;

      std::string scope = tag;
      if (tag.find("EMU") != std::string::npos) {scope = "EMU"; tag="EMU_Summary"; }
      else if (tag.find("DDU")  != std::string::npos) {scope = "DDU";}
      else if (tag.find("CSC")  != std::string::npos) {scope = "CSC";}


      
      ME_List & listMEs = MEFactories[scope];
      

      if (isMEvalid(listMEs, name, mo)) {
	EmuMonitoringObject* moc = new EmuMonitoringObject(*mo);
        moc->setPrefix(tag);
	if ( (key = hdir->FindKeyAny(moc->getFullName().c_str())) != NULL) {

	  //	LOG4CPLUS_WARN(logger_,"Found " << mo->getName());
	  TObject *obj = key->ReadObj();
	  if ( obj->IsA()->InheritsFrom( "TH1" ) ) {
	    me = dynamic_cast<MonitorElement*> (obj);
	  }
	}
	delete moc;
      } 
    }

  } 

  return me;
  
}


std::vector<std::string> getListOfFolders(std::string filter, TDirectory* dir)
{
  std::vector<std::string> dirlist;
  if (dir) {
    TList* keyslist = dir->GetListOfKeys();
    TIter next(keyslist);
    while (TObject* key = next()) {
      if (key->IsA()->InheritsFrom( "TKey" )) {
	TObject* obj = (dynamic_cast<TKey*>(key))->ReadObj();
	if  (obj->IsA()->InheritsFrom( "TDirectory" )) {
	  std::string name = obj->GetName();
	  if (name.find(filter) != std::string::npos) {
	    dirlist.push_back(name);
	    //	    std::cout << name << std::endl;
	  }
	}
	delete obj;
      }
    }
      
  }
  return dirlist;
}


int EmuPlotter::generateReport(std::string rootfile, std::string path, std::string runname) 
{


  gSystem->Load("libHistPainter");
  TFile* rootsrc = TFile::Open( rootfile.c_str());
  if (!rootsrc) {
    LOG4CPLUS_ERROR (logger_, "Unable to open " << rootfile.c_str());
    return -1;
  }
  if (!rootsrc->cd("DQMData")) {
    LOG4CPLUS_ERROR (logger_, "No histos folder in file");
    return -1;
  }

  //  loadKillBitsConfig(xmlKillBitsCfgFile);

  std::ofstream tree_items;
  std::ofstream csc_list;

  TString command = Form("mkdir -p %s",path.c_str());
  gSystem->Exec(command.Data());

  std::string runNumber = runname;
  
   TDirectory *sourcedir = gDirectory;

  std::vector<std::string> EMU_folders;
  std::vector<std::string> DDU_folders=getListOfFolders("DDU", sourcedir);
  std::vector<std::string> CSC_folders=getListOfFolders("CSC", sourcedir);

  ReportEntry entry;

  std::string hname="";

  int CSCtype = 0;
  int CSCposition = 0;


  std::map<std::string, uint32_t>::iterator stats_itr;

   // == DDUs checks and stats
  uint32_t ddu_evt_cntr = 0;
  uint32_t ddu_cntr = 0;
  uint32_t ddu_avg_events = 0;

  std::map<std::string, uint32_t> ddu_stats;
  hname = "All_DDUs_in_Readout";
  MonitorElement* me = findME("EMU", hname,  sourcedir);

  if (me) {
    TH1F* h = dynamic_cast<TH1F*>(me);

    for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
      {
	uint32_t cnt = uint32_t(h->GetBinContent(i));
	if (cnt>0) {
	  ddu_cntr++;
	  std::string dduName = Form("DDU_%02d", i);
	  ddu_stats[dduName] = cnt;
	  ddu_evt_cntr+=cnt;

	}
      }
    if (ddu_cntr) {
      int hot_ddus = 0;
      int low_ddus = 0;
      dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d DDUs in Readout", ddu_cntr),NONE));
      dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Total number of DDU events: %d ", ddu_evt_cntr),NONE));
      ddu_avg_events=ddu_evt_cntr/ddu_cntr;
      dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Average number of events per DDU: %d ", ddu_avg_events),NONE));
      if (ddu_avg_events >= 500) { // Detect different efficiency DDUs if average number of events is reasonable (>500)
	for (stats_itr=ddu_stats.begin(); stats_itr != ddu_stats.end(); ++stats_itr) {
	  double fract=((double)stats_itr->second)/ddu_avg_events;
	  std::string dduName=stats_itr->first;
	  if (fract > 1.2) {
	    std::string diag=Form("Hot readout DDU: %d events, %.f times more than average DDU events counter (avg events=%d)",
				  stats_itr->second, fract, ddu_avg_events);
	    dqm_report.addEntry(dduName, entry.fillEntry(diag, MINOR, "All_DDUs_in_Readout","EMU_Test01_DDUs_in_Readout"));
	    hot_ddus++;
	  } else if (fract < 0.8) {
	    std::string diag=Form("Low readout DDU: %d events, %f fraction of average DDU events counter (avg events=%d)",
				  stats_itr->second, fract, ddu_avg_events);
	    dqm_report.addEntry(dduName, entry.fillEntry(diag, MINOR, "All_DDUs_in_Readout","EMU_Test01_DDUs_in_Readout"));
	    low_ddus++;
	  }
	}
      }
      
    } else {
      dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d DDUs with data", ddu_cntr),CRITICAL,"All_DDUs_in_Readout","EMU_Test01_DDUs_in_Readout"));
      //      report["EMU Summary"].push_back(Form("%d DDUs with data [critical]", ddu_cntr));
    }
  } else {
    LOG4CPLUS_WARN(logger_,"Can not find " << hname);
  }


  // Check for DDU trailer errors
  uint32_t ddu_with_errs = 0;
  uint32_t ddu_err_cntr = 0;
  std::map<std::string, uint32_t> ddu_err_stats;
  hname = "All_DDUs_Trailer_Errors";
  me = findME("EMU", hname,  sourcedir);

  if (me) {
    TH2F* h = dynamic_cast<TH2F*>(me);
    for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
      {
	uint32_t cnt = uint32_t(h->GetBinContent(i,2));
	if (cnt>0) {
	  ddu_with_errs++;
	  std::string dduName = Form("DDU_%02d", i);
	  ddu_err_stats[dduName] = cnt;
	  ddu_err_cntr+=cnt;

	}
      }
    if (ddu_with_errs) {
      dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d DDUs with Trailer Error Status", ddu_with_errs), 
							  NONE, "All_DDUs_Trailer_Errors", "EMU_Test03_DDU_Reported_Errors"));
      dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Total number of DDU events with Trailer Error Status: %d ", ddu_err_cntr), 
							  NONE, "All_DDUs_Trailer_Errors", "EMU_Test03_DDU_Reported_Errors"));
      if (ddu_avg_events >= 500) { // Detect DDUs with Trailer Errors Status if average number of events is reasonable (>500)
	for (stats_itr=ddu_err_stats.begin(); stats_itr != ddu_err_stats.end(); ++stats_itr) {	  
	  std::string dduName=stats_itr->first;
	  uint32_t events = ddu_stats[dduName];
	  double fract=(((double)stats_itr->second)/events)*100;
	  DQM_SEVERITY severity=NONE;
	  if (fract >= 80.) severity=CRITICAL;
	  else if (fract >= 50.) severity=SEVERE;
	  else if (fract >20.) severity=TOLERABLE;
	  else severity=MINOR;
	  std::string diag=Form("DDU Trailer Error Status: %d events, (%f%%)",
				stats_itr->second, fract);
	  dqm_report.addEntry(dduName, entry.fillEntry(diag, severity, "All_DDUs_Trailer_Errors", "EMU_Test03_DDU_Reported_Errors"));
	}
      }
      
    } 
    delete me;

  } else {
    LOG4CPLUS_WARN(logger_,"Can not find " << hname);
  }


  // Check for DDU Format errors
  uint32_t ddu_with_fmt_errs = 0;
  uint32_t ddu_fmt_err_cntr = 0;
  std::map<std::string, uint32_t> ddu_fmt_err_stats;
  hname = "All_DDUs_Format_Errors";
  me = findME("EMU", hname,  sourcedir);

  if (me) {
    TH2F* h = dynamic_cast<TH2F*>(me);
    for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
      {
	uint32_t cnt = uint32_t(h->GetBinContent(i,2));
	if (cnt>0) {
	  ddu_with_fmt_errs++;
	  std::string dduName = Form("DDU_%02d", i);
	  ddu_fmt_err_stats[dduName] = cnt;
	  ddu_fmt_err_cntr+=cnt;

	}
      }
    if (ddu_with_fmt_errs) {
      dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d DDUs with detected Format Errors", ddu_with_fmt_errs), NONE));
      dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Total number of DDU events with detected Format Errors: %d ", ddu_fmt_err_cntr), NONE));
      if (ddu_avg_events >= 500) { // Detect DDUs with Format Errors if average number of events is reasonable (>500)
	for (stats_itr=ddu_fmt_err_stats.begin(); stats_itr != ddu_fmt_err_stats.end(); ++stats_itr) {	  
	  std::string dduName=stats_itr->first;
	   uint32_t events = ddu_stats[dduName];
	  double fract=(((double)stats_itr->second)/events)*100;
	  DQM_SEVERITY severity=NONE;
	  if (fract >= 20.) severity=CRITICAL;
	  else if (fract >= 10.) severity=SEVERE;
	  else if (fract >5.) severity=TOLERABLE;
	  else severity=MINOR;
	  std::string diag=Form("DDU Detected Format Errors: %d events, (%f%%)",
				stats_itr->second, fract);
	  dqm_report.addEntry(dduName, entry.fillEntry(diag, severity,"All_DDUs_Format_Errors", "EMU_Test04_DDU_Format_Errors" ));
	}
      }
      
    }
    delete me;
  } else {
    LOG4CPLUS_WARN(logger_,"Can not find " << hname);
  }

  // Check for DDU Live Inputs
  uint32_t ddu_live_inputs = 0;
  std::map<std::string, uint32_t> ddu_live_inp_stats;
  hname =  "All_DDUs_Live_Inputs";
  me = findME("EMU", hname,  sourcedir);
  
  if (me) {
    TH2F* h = dynamic_cast<TH2F*>(me);
    for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
      for (int j=int(h->GetYaxis()->GetXmin()); j<= int(h->GetYaxis()->GetXmax()); j++)
	{
	  uint32_t cnt = uint32_t(h->GetBinContent(i,j));
	  if (cnt>0) {
	    ddu_live_inputs++;
	    std::string dduName = Form("DDU_%02d", i);
	    ddu_live_inp_stats[dduName] |= (1<<(j-1));

	  }

	}
    dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d DDU Live Inputs detected", ddu_live_inputs)));
      
    delete me;
  } else {
    LOG4CPLUS_WARN(logger_,"Can not find " << hname);
  }

  // Check for DDU Inputs with Data
  uint32_t ddu_inp_w_data = 0;
  std::map<std::string, uint32_t> ddu_inp_data_stats;
  hname = "All_DDUs_Inputs_with_Data";
  me = findME("EMU", hname,  sourcedir);
  
  if (me) {
    TH2F* h = dynamic_cast<TH2F*>(me);
    for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
      for (int j=int(h->GetYaxis()->GetXmin()); j<= int(h->GetYaxis()->GetXmax()); j++)
	{
	  uint32_t cnt = uint32_t(h->GetBinContent(i,j));
	  if (cnt>0) {
	    ddu_inp_w_data++;
	    std::string dduName = Form("DDU_%02d", i);
	    ddu_inp_data_stats[dduName] |= (1<<(j-1));

	  }

	}
    dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d DDU Inputs with Data detected", ddu_inp_w_data)));
    if (ddu_avg_events >= 500) {
      for (stats_itr= ddu_inp_data_stats.begin(); stats_itr !=  ddu_inp_data_stats.end(); ++stats_itr) {	  
	std::string dduName=stats_itr->first;
	uint32_t live_inputs = ddu_live_inp_stats[dduName];
	uint32_t with_data = ddu_inp_data_stats[dduName];
	for (int i=0; i<16; i++) 
	  {
	    if ( ((live_inputs>>i) & 0x1) && !((with_data>>i) & 0x01))
	      {
		std::string diag=Form("DDU Input #%d: No Data",i+1);
		dqm_report.addEntry(dduName, entry.fillEntry(diag, NONE, "All_DDUs_Inputs_with_Data", "EMU_Test05_DDU_Inputs_Status"));

	      }
	  }
      }
    }
      
  
  } else {
    LOG4CPLUS_WARN(logger_,"Can not find " << hname);
  }


  
  // Check for DDU Inputs in ERROR state
  uint32_t ddu_inp_w_errors = 0;
  std::map<std::string, uint32_t> ddu_inp_w_errors_stats;
  hname = "All_DDUs_Inputs_Errors";
  me = findME("EMU", hname,  sourcedir);
  
  if (me) {
    TH2F* h = dynamic_cast<TH2F*>(me);
    for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
      for (int j=3; j<= int(h->GetYaxis()->GetXmax()); j++)
	{
	  uint32_t cnt = uint32_t(h->GetBinContent(i,j));
	  if (cnt>0) {
	    ddu_inp_w_errors++;
	    std::string dduName = Form("DDU_%02d", i);
	    ddu_inp_w_errors_stats[dduName] |= (1<<(j-3));

	  }

	}
    if (ddu_inp_w_errors) {
     dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d DDU Inputs in ERROR state detected on %d DDUs", ddu_inp_w_errors, ddu_inp_w_errors_stats.size())));
      if (ddu_avg_events >= 500) { // Detect DDUs Inputs in ERROR state if average number of events is reasonable (>500)
	for (stats_itr= ddu_inp_w_errors_stats.begin(); stats_itr !=  ddu_inp_w_errors_stats.end(); ++stats_itr) {	  
	  std::string dduName=stats_itr->first;
	  uint32_t err_inputs = ddu_inp_w_errors_stats[dduName];
	  for (int i=0; i<16; i++) 
	    {
	      if ( (err_inputs>>i) & 0x1)
		{
		  std::string diag=Form("DDU Input #%d: detected ERROR state",i+1);
		  dqm_report.addEntry(dduName, entry.fillEntry(diag, NONE, "All_DDUs_Inputs_Errors", "EMU_Test06_DDU_Inputs_in_Error_State"));

		}
	    }
	}
      }
    }
  
  } else {
    LOG4CPLUS_WARN(logger_,"Can not find " << hname);
  }

  // Check for DDU Inputs with WARNING state
  uint32_t ddu_inp_w_warn = 0;
  std::map<std::string, uint32_t> ddu_inp_w_warn_stats;
  hname = "All_DDUs_Inputs_Warnings";
  me = findME("EMU", hname,  sourcedir);
  
  if (me) {
    TH2F* h = dynamic_cast<TH2F*>(me);
    for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
      for (int j=3; j<= int(h->GetYaxis()->GetXmax()); j++)
	{
	  uint32_t cnt = uint32_t(h->GetBinContent(i,j));
	  if (cnt>0) {
	    ddu_inp_w_warn++;
	    std::string dduName = Form("DDU_%02d", i);
	    ddu_inp_w_warn_stats[dduName] |= (1<<(j-3));

	  }

	}
    if (ddu_inp_w_warn) {
      dqm_report.addEntry("EMU Summary",entry.fillEntry(Form("%d DDU Inputs in WARNING state detected on %d DDUs", ddu_inp_w_warn, ddu_inp_w_warn_stats.size())));
      if (ddu_avg_events >= 500) { // Detect DDUs Inputs in ERROR state if average number of events is reasonable (>500)
	for (stats_itr= ddu_inp_w_warn_stats.begin(); stats_itr !=  ddu_inp_w_warn_stats.end(); ++stats_itr) {	  
	  std::string dduName=stats_itr->first;
	  uint32_t warn_inputs = ddu_inp_w_warn_stats[dduName];
	  for (int i=0; i<16; i++) 
	    {
	      if ( (warn_inputs>>i) & 0x1)
		{
		  std::string diag=Form("DDU Input #%d: detected WARNING state",i+1);
		  dqm_report.addEntry(dduName, entry.fillEntry(diag));

		}
	    }
	}
      }
    }
      
  
  } else {
    LOG4CPLUS_WARN(logger_,"Can not find " << hname);
  }


  // == Chambers checks and stats
  uint32_t csc_evt_cntr = 0;
  uint32_t csc_cntr = 0;
  uint32_t csc_avg_events = 0;
  std::map<std::string, uint32_t> csc_stats;
  hname = "DMB_Reporting";
  me = findME("EMU", hname,  sourcedir);

  if (me) {
    TH2F* h = dynamic_cast<TH2F*>(me);
    for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
      for (int j=int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++) {
	uint32_t cnt = uint32_t(h->GetBinContent(i, j));
	if (cnt>0) {
	  std::string cscTag(Form("CSC_%03d_%02d", i, j));
	  std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
	  csc_stats[cscName] = cnt;
	  csc_evt_cntr+=cnt;
	  csc_cntr++;
		
	}	      
      }
    if (csc_cntr) {
      int hot_cscs = 0;
      int low_cscs = 0;
      dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with data", csc_cntr)));
      dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Total number of CSC events: %d ", csc_evt_cntr)));
      csc_avg_events=csc_evt_cntr/csc_cntr;
      dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Average number of events per CSC: %d ", csc_avg_events)));
      if (csc_avg_events >= 500) { // Detect Hot/Low eff chambers if average number of events is reasonable (>500)
	for (stats_itr=csc_stats.begin(); stats_itr != csc_stats.end(); ++stats_itr) {
	  double fract=((double)stats_itr->second)/csc_avg_events;
	  std::string cscName=stats_itr->first;
	  if (fract >= 5.) {
	    std::string diag=Form("Hot chamber: %d events, %.f times more than average events counter (avg events=%d)",
				  stats_itr->second, fract, csc_avg_events);
	    dqm_report.addEntry(cscName, entry.fillEntry(diag, SEVERE));
	    hot_cscs++;
	  } else if (fract < 0.05) {
	    std::string diag=Form("Low efficiency chamber: %d events, %f fraction of average events counter (avg events=%d)",
				  stats_itr->second, fract, csc_avg_events);
	    dqm_report.addEntry(cscName, entry.fillEntry(diag, SEVERE));
	    low_cscs++;
	  }
	}
      }
      
      if (hot_cscs) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d Hot CSCs", hot_cscs), CRITICAL));
      if (low_cscs) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d Low Efficiency CSCs", low_cscs), SEVERE));


    } else {
      dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with data", csc_cntr), CRITICAL));
    }
  } else {
    LOG4CPLUS_WARN(logger_,"Can not find " << hname);
  }

  /*
  if (ddu_evt_cntr) {
    float events_ratio = float(csc_evt_cntr)/ddu_evt_cntr;
    report["EMU Summary"].push_back(Form("CSC/DDU Events Ratio: %f", events_ratio));
  }
  */
  // == Check for chambers with Format Errors
  hname =  "DMB_Format_Errors_Fract";
  me = findME("EMU", hname,  sourcedir);
  MonitorElement* me2 = findME("EMU", "DMB_Format_Errors",  sourcedir);
  if (me && me2) 
    {
      TH2F* h1 = dynamic_cast<TH2F*>(me);
      TH2F* h2 = dynamic_cast<TH2F*>(me2);
      int csc_cntr=0;
      for (int i=int(h1->GetXaxis()->GetXmin()); i<= int(h1->GetXaxis()->GetXmax()); i++) 
	for (int j=int(h1->GetYaxis()->GetXmin()); j <= int(h1->GetYaxis()->GetXmax()); j++) 
	  {
	    double z = h1->GetBinContent(i, j);

	    if (z>0) // If chamber has format errors
	      {
		csc_cntr++;
		std::string cscTag(Form("CSC_%03d_%02d", i, j));
		std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
		uint32_t events =  uint32_t(h2->GetBinContent(i, j));

		float fract=z*100;
		DQM_SEVERITY severity=NONE;
		if (fract >= 80.) severity=CRITICAL;
		else if (fract >= 10.) severity=SEVERE;
		else if (fract > 1.) severity=TOLERABLE;
		else severity=MINOR;
		std::string diag=Form("Format Errors: %d events (%.3f%%))",events, fract);
		dqm_report.addEntry(cscName,entry.fillEntry(diag,severity));


		// --- Get Format Errors Details
		MonitorElement* me3 = findME(cscTag, "BinCheck_Errors_Frequency",  sourcedir);
		MonitorElement* me4 = findME(cscTag, "BinCheck_ErrorStat_Table",  sourcedir);
		if (me3 && me4) 
		  {
 
		    TH2F* h3 = dynamic_cast<TH2F*>(me3);
		    TH2F* h4 = dynamic_cast<TH2F*>(me4);
			 
		    for (int err=int(h3->GetYaxis()->GetXmin()); err <= int(h3->GetYaxis()->GetXmax()); err++) {
		      double z = h3->GetBinContent(1, err);
			   
		      if (z>0) 
			{
			  uint32_t events = uint32_t(h4->GetBinContent(1, err));
			  float fract=z*100;
			  std::string error_type = std::string(h3->GetYaxis()->GetBinLabel(err));
			  std::string diag=std::string(Form("\tFormat Errors: %s %d events (%.3f%%)",error_type.c_str(), events, fract));
			  // LOG4CPLUS_WARN(logger_, cscTag << ": "<< diag);
			  dqm_report.addEntry(cscName, entry.fillEntry(diag));
			}
		    }
		  }
		// --- 

	      }
	  }
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with Format Errors", csc_cntr)));
    } else {
      LOG4CPLUS_WARN(logger_,"Can not find " << hname);
    }


   // == Check for chambers with DMB-Input FIFO Full 
  me = findME("EMU", "DMB_input_fifo_full_Fract",  sourcedir);
  me2 = findME("EMU", "DMB_input_fifo_full",  sourcedir);
  if (me && me2) 
    {
      TH2F* h1 = dynamic_cast<TH2F*>(me);
      TH2F* h2 = dynamic_cast<TH2F*>(me2);
      int csc_cntr=0;
      for (int i=int(h1->GetXaxis()->GetXmin()); i<= int(h1->GetXaxis()->GetXmax()); i++) 
	for (int j=int(h1->GetYaxis()->GetXmin()); j <= int(h1->GetYaxis()->GetXmax()); j++) 
	  {
	    double z = h1->GetBinContent(i, j);

	    if (z>0) // If chamber has DMB Input FIFO Full
	      {
		csc_cntr++;
		std::string cscTag(Form("CSC_%03d_%02d", i, j));
		std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
		uint32_t events =  uint32_t(h2->GetBinContent(i, j));

		float fract=z*100;
		DQM_SEVERITY severity=NONE;
		if (fract >= 80.) severity=CRITICAL;
		else if (fract >= 10.) severity=SEVERE;
		else if (fract > 1.) severity=TOLERABLE;
		else severity=MINOR;
		std::string diag=Form("DMB-Input FIFO Full: %d events (%.3f%%)",events, fract);
		dqm_report.addEntry(cscName, entry.fillEntry(diag, severity));


		// --- Get Format Errors Details
		MonitorElement* me3 = findME(cscTag, "BinCheck_DataFlow_Problems_Frequency",  sourcedir);
		MonitorElement* me4 = findME(cscTag, "BinCheck_DataFlow_Problems_Table",  sourcedir);
		if (me3 && me4) 
		  {
 
		    TH2F* h3 = dynamic_cast<TH2F*>(me3);
		    TH2F* h4 = dynamic_cast<TH2F*>(me4);
			 
		    for (int err=int(h3->GetYaxis()->GetXmin()); err <= 7; err++) {
		      double z = h3->GetBinContent(1, err);
			   
		      if (z>0) 
			{
			  uint32_t events = uint32_t(h4->GetBinContent(1, err));
			  float fract=z*100;
			  std::string error_type = std::string(h3->GetYaxis()->GetBinLabel(err));
			  std::string diag=std::string(Form("DMB-Input FIFO Full: %s %d events (%.3f%%)",error_type.c_str(), events, fract));
			  // LOG4CPLUS_WARN(logger_, cscTag << ": "<< diag);
			  dqm_report.addEntry(cscName, entry.fillEntry(diag));
			}
		    }
		  }
		// --- 

	      }
	  }
       if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with DMB-Input FIFO Full", csc_cntr)));
    }

     // == Check for chambers with DMB-Input Timeout 
  me = findME("EMU", "DMB_input_timeout_Fract",  sourcedir);
  me2 = findME("EMU", "DMB_input_timeout",  sourcedir);
  if (me && me2) 
    {
      TH2F* h1 = dynamic_cast<TH2F*>(me);
      TH2F* h2 = dynamic_cast<TH2F*>(me2);
      int csc_cntr=0;
      for (int i=int(h1->GetXaxis()->GetXmin()); i<= int(h1->GetXaxis()->GetXmax()); i++) 
	for (int j=int(h1->GetYaxis()->GetXmin()); j <= int(h1->GetYaxis()->GetXmax()); j++) 
	  {
	    double z = h1->GetBinContent(i, j);

	    if (z>0) // If chamber has DMB Input Timeout
	      {
		csc_cntr++;
		std::string cscTag(Form("CSC_%03d_%02d", i, j));
		std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
		uint32_t events =  uint32_t(h2->GetBinContent(i, j));

		float fract=z*100;
		DQM_SEVERITY severity=NONE;
		if (fract >= 80.) severity=CRITICAL;
		else if (fract >= 10.) severity=SEVERE;
		else if (fract > 1.) severity=TOLERABLE;
		else severity=MINOR;
		std::string diag=Form("DMB-Input Timeout: %d events (%.3f%%)",events, fract);
		dqm_report.addEntry(cscName, entry.fillEntry(diag,severity));


		// --- Get Format Errors Details
		MonitorElement* me3 = findME(cscTag, "BinCheck_DataFlow_Problems_Frequency",  sourcedir);
		MonitorElement* me4 = findME(cscTag, "BinCheck_DataFlow_Problems_Table",  sourcedir);
		if (me3 && me4) 
		  {
 
		    TH2F* h3 = dynamic_cast<TH2F*>(me3);
		    TH2F* h4 = dynamic_cast<TH2F*>(me4);
			 
		    for (int err=8; err<int(h3->GetYaxis()->GetXmax())-2; err++) {
		      double z = h3->GetBinContent(1, err);
			   
		      if (z>0) 
			{
			  uint32_t events = uint32_t(h4->GetBinContent(1, err));
			  float fract=z*100;
			  std::string error_type = std::string(h3->GetYaxis()->GetBinLabel(err));
			  std::string diag=std::string(Form("DMB-Input Timeout: %s %d events (%.3f%%)",error_type.c_str(), events, fract));
			  // LOG4CPLUS_WARN(logger_, cscTag << ": "<< diag);
			  dqm_report.addEntry(cscName, entry.fillEntry(diag));
			}
		    }
		  }
		// --- 

	      }
	  }
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with DMB-Input Timeouts", csc_cntr)));
    }
	
  // == Check for missing ALCT data blocks
  me = findME("EMU", "DMB_wo_ALCT_Fract",  sourcedir);
  if (me) 
    {
      TH2F* h = dynamic_cast<TH2F*>(me);
      int csc_cntr=0;
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
	for (int j=int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++) {
	  double z = h->GetBinContent(i, j);
	  if (z>0.95) {
	    csc_cntr++;
	    std::string cscTag(Form("CSC_%03d_%02d", i, j));
	    std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
	    float fract=z*100;
	    std::string diag=Form("No ALCT Data: %.1f%%",fract);

	    dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL));
	  }

	}
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs without ALCT data", csc_cntr)));
    }

  // == Check for missing CLCT data blocks
  me = findME("EMU", "DMB_wo_CLCT_Fract",  sourcedir);
  if (me) 
    {
      TH2F* h = dynamic_cast<TH2F*>(me);
      int csc_cntr=0;
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
	for (int j=int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++) {
	  double z = h->GetBinContent(i, j);
	  if (z>0.95) {
	    csc_cntr++;
	    std::string cscTag(Form("CSC_%03d_%02d", i, j));
	    std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
	    float fract=z*100;
	    std::string diag=Form("No CLCT Data: %.1f%%",fract);

	    dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL));
	  }

	}
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs without CLCT data", csc_cntr)));
    }

  // == Check for missing CFEB data blocks
  me = findME("EMU", "DMB_wo_CFEB_Fract",  sourcedir);
  if (me) 
    {
      TH2F* h = dynamic_cast<TH2F*>(me);
      int csc_cntr=0;
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
	for (int j=int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++) {
	  double z = h->GetBinContent(i, j);
	  if (z>0.95) {
	    csc_cntr++;
	    std::string cscTag(Form("CSC_%03d_%02d", i, j));
	    std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
	    float fract=z*100;
	    std::string diag=Form("No CFEB Data: %.1f%%",fract);

	    dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL));
	  }

	}
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs without CFEB data", csc_cntr)));
    }


  // == Check for chambers with L1A out of sync
  me = findME("EMU", "DMB_L1A_out_of_sync_Fract",  sourcedir);
  me2 = findME("EMU", "DMB_L1A_out_of_sync",  sourcedir);
  if (me) 
    {
      TH2F* h = dynamic_cast<TH2F*>(me);
      TH2F* h2 = dynamic_cast<TH2F*>(me2);
      int csc_cntr=0;
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
	for (int j=int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++) {
	  double z = h->GetBinContent(i, j);
	  if (z>0) {
	    csc_cntr++;
	    uint32_t events = uint32_t(h2->GetBinContent(i, j));
	    std::string cscTag(Form("CSC_%03d_%02d", i, j));
	    std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
	    float fract=z*100;
	    DQM_SEVERITY severity=NONE;
	    if (fract >= 80.) severity=CRITICAL;
	    else if (fract >= 10.) severity=SEVERE;
	    else if (fract > 1.) severity=TOLERABLE;
	    else severity=MINOR;	   

	    std::string diag=Form("L1A out of sync: %d events (%.3f%%)",events, fract);

	    dqm_report.addEntry(cscName, entry.fillEntry(diag,severity));

	    MonitorElement* me3 = 0;

	    // Prepare list of L1A histos 
	    std::vector<std::pair<std::string, std::string> > l1a_histos;
	    for (int icfeb=0; icfeb<5; icfeb++) {
	      l1a_histos.push_back(make_pair( Form("CFEB%d", icfeb+1 ), Form("CFEB%d_DMB_L1A_diff", icfeb) ));
	    }
	    l1a_histos.push_back(make_pair( "ALCT","ALCT_DMB_L1A_diff") );
	    l1a_histos.push_back(make_pair( "CLCT","CLCT_DMB_L1A_diff") );
	    l1a_histos.push_back(make_pair( "DDU","DMB_DDU_L1A_diff") );

	    // -- Find which board has L1A out of sync
	    for (uint32_t k = 0; k < l1a_histos.size(); k++) {
	      me3 = findME(cscTag, l1a_histos[k].second,  sourcedir);
	      if (me3) {
		TH1F* h3 = dynamic_cast<TH1F*>(me3);
		if (h3->GetMean() != 0) {
		  diag=Form("L1A out of sync: %s",(l1a_histos[k].first).c_str());

		  dqm_report.addEntry(cscName, entry.fillEntry(diag));
		}
	      }

	    }

	  }

	}
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with L1A out of sync", csc_cntr)));
    }

  // == Perform per Chamber Checks
  for (uint32_t i=0; i<CSC_folders.size(); i++) {
    int crate=0, slot =0; 
    //    std::cout << getCSCName(CSC_folders[i], crate, slot, CSCtype, CSCposition) << std::endl;
    std::string cscName = getCSCName(CSC_folders[i], crate, slot, CSCtype, CSCposition);
    int nCFEBs = getNumCFEBs(cscName);
    int nStrips = getNumStrips(cscName);
    int nWireGroups = getNumWireGroups(cscName);
    int deadCFEBs[5];
    int deadALCT=0;


    // -- CFEBs DAV checks
    me = findME(CSC_folders[i], "Actual_DMB_CFEB_DAV_Frequency",  sourcedir);
    me2 = findME(CSC_folders[i], "Actual_DMB_CFEB_DAV_Rate",  sourcedir);
    if (me && me2) {
      TH1F* h = dynamic_cast<TH1F*>(me);
      TH1F* h1 = dynamic_cast<TH1F*>(me2);

      if  ( h->GetEntries() > nCFEBs) {
	for (int icfeb=0; icfeb< nCFEBs; icfeb++) {
	  double z =  h->GetBinContent(icfeb+1);
	  if (z < 5) {
	    //	    uint32_t events = uint32_t(h1->GetBinContent(icfeb+1));
	  
	    std::string diag=Form("CFEB Dead/Low efficiency: CFEB%d DAV %.3f%%", icfeb+1, z);
	    if (z==0) deadCFEBs[icfeb]=1; // Mark this CFEB as dead
	  
	    dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL));
	  }
	}
      }
      delete me;
      delete me2;
    }

    
    // -- CFEB SCA Occupancies Checks
    for (int ilayer=1; ilayer<=6; ilayer++) {
      std::string name = Form("CFEB_ActiveStrips_Ly%d",ilayer);

      me = findME(CSC_folders[i], name , sourcedir);
      if (me) {
	TH1F* h = dynamic_cast<TH1F*>(me);
	if (h->GetEntries() > nStrips) {
	  for (int icfeb=0; icfeb < nCFEBs; icfeb++) {
            if (deadCFEBs[icfeb]) continue; // Skip dead CFEBs
	    if (h->Integral(icfeb*16+1, (icfeb+1)*16) == 0) {
	  
	      std::string diag=Form("No SCA Data: CFEB%d Layer%d", icfeb+1, ilayer);
	  
	      dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL));
	    }
	    
	  }
      
	}
	delete me;
      }
    }

    // -- CFEB Comparators Occupancies Checks
    for (int ilayer=1; ilayer<=6; ilayer++) {
      std::string name = Form("CLCT_Ly%d_Efficiency",ilayer);

      me = findME(CSC_folders[i], name , sourcedir);
      if (me) {
	TH1F* h = dynamic_cast<TH1F*>(me);
	// if (h->GetEntries() > nStrips) {
	 if (h->GetMaximum() > 0.1) {
	  for (int icfeb=0; icfeb < nCFEBs; icfeb++) {
	    if (deadCFEBs[icfeb]) continue; // Skip dead CFEBs
	    if (h->Integral(icfeb*32+1, (icfeb+1)*32) == 0) {
	  
	      std::string diag=Form("No Comparators Data: CFEB%d Layer%d", icfeb+1, ilayer);
	  
	      dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL));
	    }
	    
	  }
      
	}
	 delete me;
      }
    }
    
    // -- Anode Occupancies Checks
    for (int ilayer=1; ilayer<=6; ilayer++) {
      std::string name = Form("ALCT_Ly%d_Efficiency",ilayer);

      me = findME(CSC_folders[i], name , sourcedir);
      if (me) {
	TH1F* h = dynamic_cast<TH1F*>(me);
	// if (h->GetEntries() > nWireGroups) {
	if (h->GetMaximum() > 0.1) {  
	  for (int iseg=0; iseg < nWireGroups/8; iseg++) {
	    if (h->Integral(iseg*8+1, (iseg+1)*8) == 0) {
	      int afeb = iseg*3+ilayer/2+1;
	      std::string diag=Form("No Anode Data: AFEB%d Layer%d", afeb, ilayer);

	      dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL));
	  
	    }
	    
	  }
      
	}
	delete me;
      }
    }

  }


   // == Check for chambers with format warnings (CFEB B-Words)
  me = findME("EMU", "DMB_Format_Warnings_Fract",  sourcedir);
  me2 = findME("EMU", "DMB_Format_Warnings",  sourcedir);
  if (me) 
    {
      TH2F* h = dynamic_cast<TH2F*>(me);
      TH2F* h2 = dynamic_cast<TH2F*>(me2);
      int csc_cntr=0;
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
	for (int j=int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++) {
	  double z = h->GetBinContent(i, j);
	  if (z>0) {
	    csc_cntr++;
	    uint32_t events = uint32_t(h2->GetBinContent(i, j));
	    std::string cscTag(Form("CSC_%03d_%02d", i, j));
	    std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
	    float fract=z*100;
	    DQM_SEVERITY severity=NONE;
	    if (fract >= 80.) severity=CRITICAL;
	    else if (fract >= 10.) severity=SEVERE;
	    else if (fract > 1.) severity=TOLERABLE;
	    else severity=MINOR;	   

	    std::string diag=Form("CFEB B-Words: %d events (%.3f%%)",events, fract);

	    dqm_report.addEntry(cscName, entry.fillEntry(diag, severity));
	  }

	}
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with CFEB B-Words", csc_cntr)));
    }

  int csc_w_problems=0;
  T_DQMReport& report = dqm_report.getReport();
  T_DQMReport::iterator itr;
  for (itr = report.begin(); itr != report.end(); ++itr) {
    if (itr->first.find("ME") == 0) {
      csc_w_problems++;
    }
  }
  dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Found %d CSCs with issues of different severity", csc_w_problems)));
  
  showReport();
  path = (path.size()?path+"/":"");
  saveReport(path+"dqm_report.txt");
  return 0;
}

// == Show DQM Report
void EmuPlotter::showReport()
{
  std::map<std::string, std::vector<std::string> >::iterator itr;
  std::vector<std::string>::iterator err_itr;

  LOG4CPLUS_WARN(logger_, "<=== Automatically generated DQM Report ===>");
  for (itr = report.begin(); itr != report.end(); ++itr) {
    if (itr->first.find("EMU") == 0) {
      LOG4CPLUS_WARN(logger_, "<--- " <<itr->first << " ---> ");
      for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr) {
	LOG4CPLUS_WARN(logger_, *err_itr);
      }
    }
  }

  LOG4CPLUS_WARN(logger_, "<--- DDUs Detailed Report --->");
   for (itr = report.begin(); itr != report.end(); ++itr) {
    if (itr->first.find("DDU") == 0) {
      LOG4CPLUS_WARN(logger_, "[= " <<itr->first << " =]");
      for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr) {
	LOG4CPLUS_WARN(logger_, *err_itr);
      }
    }
  }

  LOG4CPLUS_WARN(logger_, "<--- CSCs Detailed Report --->");
  for (itr = report.begin(); itr != report.end(); ++itr) {
    if (itr->first.find("ME") == 0) {
      LOG4CPLUS_WARN(logger_, "[= " <<itr->first << " =]");
      for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr) {
	LOG4CPLUS_WARN(logger_, *err_itr);
      }
    }	
  }

}


int EmuPlotter::save_ALCT_CLCT_Match_Data(std::string rootfile, std::string path, std::string runname)
{
  TFile* rootsrc = TFile::Open( rootfile.c_str());
  if (!rootsrc) {
    LOG4CPLUS_ERROR (logger_, "Unable to open " << rootfile.c_str());
    return -1;
  }
  if (!rootsrc->cd("DQMData")) {
    LOG4CPLUS_ERROR (logger_, "No histos folder in file");
    return -1;
  }


  TString command = Form("mkdir -p %s",path.c_str());
  gSystem->Exec(command.Data());

  std::string runNumber = runname;
  
  TDirectory *sourcedir = gDirectory;

  std::vector<std::string> CSC_folders=getListOfFolders("CSC", sourcedir);


  int CSCtype = 0;
  int CSCposition = 0;


  std::ostringstream strout;
  // == Perform per Chamber Checks
  for (uint32_t i=0; i<CSC_folders.size(); i++) {
    int crate=0, slot =0; 
    std::string cscName = getCSCName(CSC_folders[i], crate, slot, CSCtype, CSCposition);

    // ALCT-CLCT Match
    MonitorElement* me = findME(CSC_folders[i], "ALCT_Match_Time",  sourcedir);
    if (me) {
      TH1F* h = dynamic_cast<TH1F*>(me);
      strout << cscName << " " << fixed << setprecision(3) << h->GetMean()<< " " << setprecision(3) << h->GetRMS() << " " << setprecision(0) << h->GetEntries() << std::endl;
    }
  }
  path = (path.size()?path+"/":"");
  std::string filename = (path+"alct_clct_match.txt");
  std::ofstream fout(filename.c_str());
  LOG4CPLUS_WARN(logger_, "Saving ALCT-CLCT Match data to " << filename);
  fout << "ChamberID  Mean  RMS  Entries" << std::endl;
  

  fout << strout.str();
  fout.close();
  return 0;

}

int EmuPlotter::save_CSCCounters(std::string rootfile, std::string path, std::string runname)
{
  TFile* rootsrc = TFile::Open( rootfile.c_str());
  if (!rootsrc) {
    LOG4CPLUS_ERROR (logger_, "Unable to open " << rootfile.c_str());
    return -1;
  }
  if (!rootsrc->cd("DQMData")) {
    LOG4CPLUS_ERROR (logger_, "No histos folder in file");
    return -1;
  }


  TString command = Form("mkdir -p %s",path.c_str());
  gSystem->Exec(command.Data());

  std::string runNumber = runname;
  
  TDirectory *sourcedir = gDirectory;

  std::vector<std::string> CSC_folders=getListOfFolders("CSC", sourcedir);


  int CSCtype = 0;
  int CSCposition = 0;


  std::ostringstream strout;
  cscCntrs.clear();
  MonitorElement* mes = findME("EMU", "DMB_Format_Errors",  sourcedir); 
  TH2F* hs = dynamic_cast<TH2F*>(mes);
  // == Perform per Chamber Checks
  for (uint32_t i=0; i<CSC_folders.size(); i++) {
    int crate=0, slot =0; 
    std::string cscName = getCSCName(CSC_folders[i], crate, slot, CSCtype, CSCposition);
    CSCCounters& trigCnts = cscCntrs[CSC_folders[i]];

    
    MonitorElement* me = findME(CSC_folders[i], "Actual_DMB_FEB_DAV_Rate",  sourcedir);
    if (me) {
      //      TH1F* h = dynamic_cast<TH1F*>(me);
      trigCnts["ALCT"] =  me->GetBinContent(1);
      trigCnts["CLCT"] =  me->GetBinContent(2);
      trigCnts["CFEB"] =  me->GetBinContent(3);
    }

    me = findME(CSC_folders[i], "Actual_DMB_FEB_DAV_Frequency",  sourcedir);
    if (me) {
      // TH1F* h = dynamic_cast<TH1F*>(me);
      trigCnts["DMB"] =  me->GetEntries();
    }

    trigCnts["BAD"] = hs->GetBinContent(crate, slot);
    
  }
  

  path = (path.size()?path+"/":"");
  std::string filename = (path+"csc_counters.js");
  std::ofstream fout(filename.c_str());
  LOG4CPLUS_WARN(logger_, "Saving CSC Counters " << filename);
  
  fout << "var CSC_COUNTERS=[" << std::endl;
  fout << "['Run: "<< runname <<"'," << std::endl;

  if (!cscCntrs.empty()) {
    std::map<std::string, CSCCounters>::iterator citr;
    for (citr=cscCntrs.begin(); citr != cscCntrs.end(); ++citr) {
      fout << "['" << citr->first << "',[";
      CSCCounters::iterator itr;
      for (itr=citr->second.begin(); itr != citr->second.end(); ++itr) { // == Loop and Output Counters
	fout << "['"<< itr->first << "','" << itr->second <<"'],";
      }
      fout << "]]," << std::endl;
    }
  }

  fout << "]]" << std::endl;
  fout.close();
  return 0;

}


// == Save DQM Report to file
void EmuPlotter::saveReport(std::string filename)
{
  T_DQMReport::iterator itr;
  vector<ReportEntry>::iterator err_itr;


  T_DQMReport& report = dqm_report.getReport();
  
  std::ofstream out(filename.c_str());
  LOG4CPLUS_INFO(logger_, " Saving DQM report to " << filename);
  out << "<=== Automatically generated at " << emu::dqm::utils::now() << " DQM Report ===>" << std::endl;


  out << std::endl;
  for (itr = report.begin(); itr != report.end(); ++itr) {
    if (itr->first.find("EMU") == 0) {
      out << "<--- " <<itr->first << " --->" << std::endl;
      for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr) {
	out <<"\t" <<err_itr->text;
	if (err_itr->severity != NONE) 
	  out << " [" << DQM_SEVERITY_STR[err_itr->severity]<< "] ";
	out << std::endl;
      }
    }
  }

  out << std::endl;
  out << "<--- DDUs Detailed Report --->" << std::endl;
  for (itr = report.begin(); itr != report.end(); ++itr) {
    if (itr->first.find("DDU") == 0) {
      out << "[= " <<itr->first << " =]" << std::endl;
      for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr) {
	out <<"\t" <<err_itr->text;
	if (err_itr->severity != NONE) 
	  out << " [" << DQM_SEVERITY_STR[err_itr->severity]<< "] ";
	out << std::endl;
      }
    }
  }

  out << std::endl;
  out << "<--- CSCs Detailed Report --->" << std::endl;
  for (itr = report.begin(); itr != report.end(); ++itr) {
    if (itr->first.find("ME") == 0) {
      out << "[= " <<itr->first << " =]" << std::endl;
      for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr) {
	out <<"\t" <<err_itr->text;
	if (err_itr->severity != NONE) 
	  out << " [" << DQM_SEVERITY_STR[err_itr->severity]<< "] ";
	out << std::endl;

      }
    }	
  }

}


/*
// == Save DQM Report to file
void EmuPlotter::saveReport(std::string filename)
{
  std::map<std::string, std::vector<std::string> >::iterator itr;
  std::vector<std::string>::iterator err_itr;
  
  std::ofstream out(filename.c_str());
  LOG4CPLUS_INFO(logger_, " Saving DQM report to " << filename);
  out << "<=== Automatically generated at " << emu::dqm::utils::now() << " DQM Report ===>" << std::endl;

  out << std::endl;
  for (itr = report.begin(); itr != report.end(); ++itr) {
    if (itr->first.find("EMU") == 0) {
      out << "<--- " <<itr->first << " --->" << std::endl;
      for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr) {
	out <<"\t" <<*err_itr << std::endl;
      }
    }
  }

  out << std::endl;
  out << "<--- DDUs Detailed Report --->" << std::endl;
  for (itr = report.begin(); itr != report.end(); ++itr) {
    if (itr->first.find("DDU") == 0) {
      out << "[= " <<itr->first << " =]" << std::endl;
      for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr) {
	out <<"\t" <<*err_itr << std::endl;
      }
    }
  }

  out << std::endl;
  out << "<--- CSCs Detailed Report --->" << std::endl;
  for (itr = report.begin(); itr != report.end(); ++itr) {
    if (itr->first.find("ME") == 0) {
      out << "[= " <<itr->first << " =]" << std::endl;
      for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr) {
	out <<"\t" <<*err_itr << std::endl;
      }
    }	
  }

}
*/