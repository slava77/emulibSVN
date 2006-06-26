#ifndef EmuTFbookkeeper_h
#define EmuTFbookkeeper_h

#include <string>
#include <list>
#include <map>
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "EmuTFxmlParsing.h"

class EmuTFbookkeeper {
private:
	std::map<std::string,TH1*>           tfHists, spHists,     pcHists,         cscHists;
	std::map<std::string,TH1*>           tfAlias, spAlias[12], pcAlias[12][60], cscAlias[12][60][9];
	std::map<std::string,EmuTFxmlParsing::HistAttributes> attributes;
	std::list<TObject*>                                   cleenupList;
public:
	// Create set of histograms for the particular:
	// Sector Processor  (one arg), Peripheral Crate (two args), chamber (three arge)
	// return false in case of failure
	bool book(unsigned short sp=0, unsigned short mpc=0, unsigned short csc=0);

	bool isBooked(unsigned short sp=0, unsigned short mpc=0, unsigned short csc=0){
		if( sp>12 || mpc>60 || csc>9 ) return false;
		std::map<std::string,TH1*> &hists = ( sp && mpc && csc ? cscAlias[sp-1][mpc-1][csc-1] : ( sp && mpc && !csc ? pcAlias[sp-1][mpc-1] : ( sp && !mpc && !csc ? spAlias[sp-1] : tfAlias ) ) );
		return hists.size();
	}

	// Obtain final list of canvases with all histograms
	std::map<std::string,TCanvas*> wrapToCanvases(const std::map<std::string,EmuTFxmlParsing::CanvasAttributes>& canvasList);

	TH1* get(std::string hname, unsigned short sp=0, unsigned short mpc=0, unsigned short csc=0){
		if( sp>12 || mpc>60 || csc>9 ) return 0;
		std::map<std::string,TH1*> &hists = ( sp && mpc && csc ? cscAlias[sp-1][mpc-1][csc-1] : ( sp && mpc && !csc ? pcAlias[sp-1][mpc-1] : ( sp && !mpc && !csc ? spAlias[sp-1] : tfAlias ) ) );
		std::map<std::string,TH1*>::const_iterator entry = hists.find(hname);
		return ( entry==hists.end() ? 0 : entry->second );
	}

	// Default constructor
	EmuTFbookkeeper(const std::map<std::string,EmuTFxmlParsing::HistAttributes>& histList){ attributes=histList; }

	// Cleanup
	~EmuTFbookkeeper(void){
		for(std::map<std::string,TH1*>::iterator iter=tfHists.begin(); iter!=tfHists.end(); iter++)
			delete iter->second;
		for(std::map<std::string,TH1*>::iterator iter=spHists.begin(); iter!=spHists.end(); iter++)
			delete iter->second;
		for(std::map<std::string,TH1*>::iterator iter=pcHists.begin(); iter!=pcHists.end(); iter++)
			delete iter->second;
		for(std::map<std::string,TH1*>::iterator iter=cscHists.begin(); iter!=cscHists.end(); iter++)
			delete iter->second;
		for(std::list<TObject*>::iterator iter=cleenupList.begin(); iter!=cleenupList.end(); iter++)
			delete *iter;
	}
};

#endif
