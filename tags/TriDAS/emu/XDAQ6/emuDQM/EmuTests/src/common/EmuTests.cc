#include "EmuTests.h"

EmuTests::EmuTests():
	logger_(Logger::getInstance("EmuTests"))
{
	init();
}


EmuTests::EmuTests(Logger logger):
        logger_(logger)
{
	init();
}

void EmuTests::init() 
{
	xmlHistosBookingCfgFile = "";
        xmlCanvasesCfgFile = "";
	cscMapFile = "";
        tmap = getCSCTypeToBinMap();
	dduCheckMask = 0xFFFFFFFF;
        binCheckMask = 0xFFFFFFFF;
	reset();
}

void EmuTests::reset()
{
	fFirstEvent = true;
        nEvents = 0;
        nBadEvents = 0;
        nGoodEvents = 0;
        nCSCEvents = 0;
        L1ANumber = 0;
        BXN = 0;
        eTag = "";
	bin_checker.output1().hide();
        bin_checker.output2().hide();
        bin_checker.crcALCT(true);
        bin_checker.crcTMB (true);
        bin_checker.crcCFEB(true);
        bin_checker.modeDDU(true);
	fBusy=false;
}

void EmuTests::clearMECollection(ME_List & collection) 
{

	if (collection.size() > 0) {
		for (ME_List_iterator itr = collection.begin();itr != collection.end(); ++itr) {
                        delete itr->second;
        	}
		collection.clear();
	}

}
void EmuTests::printMECollection(ME_List & collection)
{
	int i = 0;
        for (ME_List_iterator itr = collection.begin();itr != collection.end(); ++itr) {
		LOG4CPLUS_INFO(logger_, ++i << ":" << itr->first << ":" << itr->second->getFullName());
        }

}


void EmuTests::clearCanvasesCollection(MECanvases_List & collection)
{

        if (collection.size() > 0) {
                for (MECanvases_List_iterator itr = collection.begin();itr != collection.end(); ++itr) {
                        delete itr->second;
                }
                collection.clear();
        }

}
void EmuTests::printCanvasesCollection(MECanvases_List & collection)
{
        int i = 0;
        for (MECanvases_List_iterator itr = collection.begin();itr != collection.end(); ++itr) {
                LOG4CPLUS_INFO(logger_, ++i << ":" << itr->first << ":" << itr->second->getFullName());
        }

}


EmuTests::~EmuTests() {
	std::map<std::string, ME_List >::iterator itr;	
	for (itr = MEs.begin(); itr != MEs.end(); ++itr) {
		clearMECollection(itr->second);
	}

	MEs.clear();
	clearMECollection(commonMEfactory);
	clearMECollection(chamberMEfactory);
	clearMECollection(dduMEfactory);
}

void EmuTests::book() {
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
	if (loadXMLBookingInfo(xmlHistosBookingCfgFile) == 0) 
	{
		reset();
		loadXMLCanvasesInfo(xmlCanvasesCfgFile);
	}
*/
}

void EmuTests::setCSCMapFile(std::string filename)
{
        if (filename != "") {
		cscMapFile = filename;
                cscMapping  = CSCReadoutMappingFromFile(cscMapFile);
        }


}

std::map<std::string, int> EmuTests::getCSCTypeToBinMap()
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

std::string EmuTests::getCSCTypeLabel(int endcap, int station, int ring )
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

void EmuTests::getCSCFromMap(int crate, int slot, int& csctype, int& cscposition)
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


bool EmuTests::isMEvalid(ME_List& MEs, std::string name, EmuMonitoringObject*& me)
{
	ME_List_iterator res = MEs.find(name);
	if (res != MEs.end() && (res->second != 0)) {
		me = res->second;
		// cout << "Found " << me->getName() << endl;
		return true;
	} else {
		LOG4CPLUS_WARN(logger_, "Plotter can not find ME: '" << name << "'");
		me = 0;
		return false;
	}
	
} 

