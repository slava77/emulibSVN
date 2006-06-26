#include "RegEx.cc"
#include "EmuTFbookkeeper.cc"
#include "EmuTFxmlParsing.cc"
#include "EmuTFfiller.cc"
#include "TH1.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TFile.h"
#include "TApplication.h"

#include "IORawData/CSCCommissioning/src/FileReaderDDU.cc"

//g++ -o test EmuTFtest.cpp -I/home/slice/DAQKit/v3.6/TriDAS/daq/extern/xerces/linuxx86/include/ -I../../src/common/ -I../../include/ -L/home/slice/DAQKit/v3.6/TriDAS/daq/extern/xerces/linuxx86/lib/ -lxerces-c `root-config --cflags` `root-config --glibs` -Wall -I/home/slice/kotov/CMSSW_0_8_0_pre2/src/

int main(int argc, char *argv[]){
	using namespace std;
	using namespace EmuTFxmlParsing;
	// Root specific
	extern void InitGui();
	VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
	TROOT root("ROOTSYSTEM","ROOT_", initfuncs);
	int a=2;
	char *v[]={"qwe","-b"};
	TApplication app("qwe", &a, v);

	// Histograms/Canvases description
	std::map<std::string,HistAttributes>   histList   = parseHistXML  ("../../xml/CSCTF_histograms.xml");
	std::map<std::string,CanvasAttributes> canvasList = parseCanvasXML("../../xml/CSCTF_canvases.xml");

	EmuTFfiller filler(histList);

	// DDU File Reader
	FileReaderDDU reader;
	reader.open(argv[1]);
	//reader.reject(FileReaderDDU::DDUoversize|FileReaderDDU::FFFF);
	//reader.select(FileReaderDDU::Type1);
	reader.select(0);

	// Event buffer
	size_t size, nevents=0;
	const unsigned short *buf=0;

	// Main cycle
	while( (size = reader.read(buf)) /*&& nevents<5*/ ){
		filler.fill(buf,size,reader.status());
		nevents++;
	}

	std::map<std::string,TCanvas*> rootCanvases = filler.bookkeeper().wrapToCanvases(canvasList);
	std::map<std::string,TCanvas*>::iterator iter = rootCanvases.begin();
	TFile file("qqq.root","RECREATE");
	while( iter != rootCanvases.end() ){
		//iter->second->Draw();
		iter->second->Write();
		iter++;
	}
	file.Close();

	app.Terminate();
	return 0;
}
