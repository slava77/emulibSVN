#ifndef EmuTFplotter_h
#define EmuTFplotter_h

#include "TFile.h"
#include "EmuTFfiller.h"
#include "EmuMonitor/include/EmuPlotterInterface.h"

class EmuTFplotter : public EmuPlotterInterface {
private:
	std::string hfile;
public:
	virtual void SetHistoFile(string f){ hfile=f; }
	virtual void SaveImagesFromROOTFile(std::string filename, 
		std::string path,
		std::string format=DEFAULT_IMAGE_FORMAT,
		int width=DEFAULT_CANVAS_WIDTH,
		int height=DEFAULT_CANVAS_HEIGHT){}
	virtual void SaveImages(std::string path,
		std::string format=DEFAULT_IMAGE_FORMAT,
		int width=DEFAULT_CANVAS_WIDTH,
		int height=DEFAULT_CANVAS_HEIGHT){}
	virtual void SaveHistos(void){
		std::map<std::string,TCanvas*>::iterator iter = rootCanvases.begin();
		TFile file(hfile.c_str(),"RECREATE");
		while( iter != rootCanvases.end() ){
			//iter->second->Draw();
			iter->second->Write();
			iter++;
		}
	}
	virtual void SetDDUCheckMask(unsigned int){};
	virtual void SetBinCheckMask(unsigned int){};
	virtual void SetDDU2004(int){};
	virtual void book(void){}
	virtual void fill(unsigned char * data, int dataSize, unsigned short errorStat) = 0;

	virtual bool isListModified(){ return true; } //!!!
	virtual void setListModified(bool flag){}
	virtual TConsumerInfo* getConsumerInfo() = 0;

	EmuTFplotter(void):EmuPlotterInterface(){
		hfile = "qqq.root";
		histos   = ;
		canvases = ;
	}
	~EmuTFplotter(void){}
};

#endif
