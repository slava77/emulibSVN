#include "EmuPlotter.h"

EmuPlotter::EmuPlotter():
	logger_(Logger::getInstance("EmuPlotter")),
	dduCheckMask(0xFFFFFFFF),
	binCheckMask(0xFFFFFFFF)
{
	xmlHistosBookingCfgFile = "";
	xmlCanvasesCfgFile = "";
	setParameters();
}


EmuPlotter::EmuPlotter(Logger logger):
        logger_(logger),
	dduCheckMask(0xFFFFFFFF),
        binCheckMask(0xFFFFFFFF)
{
	xmlHistosBookingCfgFile = "";
        xmlCanvasesCfgFile = "";
	setParameters();
}

void EmuPlotter::setParameters() 
{
	fFirstEvent = true;
	nEvents = 0;
	nBadEvents = 0;
	L1ANumber = 0;
	BXN = 0;
	// dduCheckMask = 0xFFFFFFFF;
	// binCheckMask = 0xFFFFFFFF;
        bin_checker.output1().hide();
        bin_checker.output2().hide();
        bin_checker.crcALCT(true);
        bin_checker.crcTMB (true);
        bin_checker.crcCFEB(true);
	bin_checker.modeDDU(true);
	cscMapFile = "";
        this->setCSCMapFile(cscMapFile);
        tmap = this->getCSCTypeToBinMap();
	fBusy=false;
}

void EmuPlotter::clearMECollection(ME_List & collection) 
{

	if (collection.size() > 0) {
		for (ME_List_iterator itr = collection.begin();itr != collection.end(); ++itr) {
                        delete itr->second;
        	}
		collection.clear();
	}

}
void EmuPlotter::printMECollection(ME_List & collection)
{
	int i = 0;
        for (ME_List_iterator itr = collection.begin();itr != collection.end(); ++itr) {
		LOG4CPLUS_INFO(logger_, ++i << ":" << itr->first << ":" << itr->second->getFullName());
        }

}


void EmuPlotter::clearCanvasesCollection(MECanvases_List & collection)
{

        if (collection.size() > 0) {
                for (MECanvases_List_iterator itr = collection.begin();itr != collection.end(); ++itr) {
                        delete itr->second;
                }
                collection.clear();
        }

}
void EmuPlotter::printCanvasesCollection(MECanvases_List & collection)
{
        int i = 0;
        for (MECanvases_List_iterator itr = collection.begin();itr != collection.end(); ++itr) {
                LOG4CPLUS_INFO(logger_, ++i << ":" << itr->first << ":" << itr->second->getFullName());
        }

}


EmuPlotter::~EmuPlotter() {
	std::map<std::string, ME_List >::iterator itr;	
	for (itr = MEs.begin(); itr != MEs.end(); ++itr) {
		clearMECollection(itr->second);
	}

	MEs.clear();
	clearMECollection(commonMEfactory);
	clearMECollection(chamberMEfactory);
	clearMECollection(dduMEfactory);
}

void EmuPlotter::book() {
	if (MEs.size() > 0) {
		std::map<std::string, ME_List >::iterator itr;
        	for (itr = MEs.begin(); itr != MEs.end(); ++itr) {
                	clearMECollection(itr->second);
        	}
        	MEs.clear();
	}
	if (MECanvases.size() > 0) {
                std::map<std::string, MECanvases_List >::iterator itr;
                for (itr = MECanvases.begin(); itr != MECanvases.end(); ++itr) {
                        clearCanvasesCollection(itr->second);
                }
                MECanvases.clear();
        }

/*
        clearMECollection(commonMEfactory);
        clearMECollection(chamberMEfactory);
        clearMECollection(globalMEfactory);
*/
//	LOG4CPLUS_INFO(logger_, "XML Config file: "  <<  xmlCfgFile)
//	loadXMLBookingInfo(xmlCfgFile);
//	setParameters();
        

	if (loadXMLBookingInfo(xmlHistosBookingCfgFile) == 0) 
	{
		setParameters();
/*
		string common = Form("EMU_%d",node);
		MEs[common] = bookCommon(node);
		LOG4CPLUS_INFO(logger_,"=== ME Collection for " << common);
	        printMECollection(MEs[common]);
*/
	}
	loadXMLCanvasesInfo(xmlCanvasesCfgFile);

}

void EmuPlotter::setCSCMapFile(std::string filename)
{
        if (filename != "") {
		cscMapFile = filename;
                cscMapping  = CSCReadoutMappingFromFile(cscMapFile);
        }


}

std::map<std::string, int> EmuPlotter::getCSCTypeToBinMap()
{
        std::map<std::string, int> tmap;
        tmap["ME-4/2"] = 0;
        tmap["ME-4/1"] = 1;
        tmap["ME-3/2"] = 2;
        tmap["ME-3/1"] = 3;
        tmap["ME-2/2"] = 4;
        tmap["ME-2/1"] = 5;
        tmap["ME-1/3"] = 6;
        tmap["ME-1/2"] = 7;
        tmap["ME-1/1"] = 8;
        tmap["ME+1/1"] = 9;
        tmap["ME+1/2"] = 10;
        tmap["ME+1/3"] = 11;
        tmap["ME+2/1"] = 12;
        tmap["ME+2/2"] = 13;
        tmap["ME+3/1"] = 14;
        tmap["ME+3/2"] = 15;
        tmap["ME+4/1"] = 16;
        tmap["ME+4/2"] = 17;
        return tmap;

}

std::string EmuPlotter::getCSCTypeLabel(int endcap, int station, int ring )
{
        std::string label = "Unknown";
        std::ostringstream st;
        if ((endcap > 0) && (station>0) && (ring>0)) {
                if (endcap==1) {
                        st << "ME+" << station << "/" << ring;
                        label = st.str();
                } else if (endcap==2) {
                        st << "ME-" << station << "/" << ring;
                        label = st.str();
                } else {
                        label = "Unknown";
                }
        }
        return label;
}

void EmuPlotter::getCSCFromMap(int crate, int slot, int& csctype, int& cscposition)
{
//  LOG4CPLUS_INFO(logger_, "========== get CSC from Map crate" << crate << " slot" << slot);
  int iendcap = -1;
  int istation = -1;
  int iring = -1;
  // TODO: Add actual Map conversion
  int id = cscMapping.chamber(iendcap, istation, crate, slot, -1);
  if (id==0) {
        return;
  }
  CSCDetId cid( id );
  iendcap = cid.endcap();
  istation = cid.station();
  iring = cid.ring();
  cscposition = cid.chamber();

//  std::map<std::string, int> tmap = getCSCTypeToBinMap();
  std::string tlabel = getCSCTypeLabel(iendcap, istation, iring );
  std::map<std::string,int>::const_iterator it = tmap.find( tlabel );
  if (it != tmap.end()) {
        csctype = it->second;
//      LOG4CPLUS_INFO(logger_, "========== get CSC from Map label:" << tlabel << "/" << cscposition);
  } else {
//      LOG4CPLUS_INFO(logger_, "========== can not find map");
        csctype = 0;
  }

  // return bin number which corresponds for CSC Type (ex. ME+4/2 -> bin 18)


}


bool EmuPlotter::isMEvalid(ME_List& MEs, std::string name, EmuMonitoringObject*& me)
{
	ME_List_iterator res = MEs.find(name);
	if (res != MEs.end() && (res->second != 0)) {
		me = res->second;
		// cout << "Found " << me->getName() << endl;
		return true;
	} else {
		LOG4CPLUS_WARN(logger_, "Can not find ME " << name);
		me = 0;
		return false;
	}
	
} 
