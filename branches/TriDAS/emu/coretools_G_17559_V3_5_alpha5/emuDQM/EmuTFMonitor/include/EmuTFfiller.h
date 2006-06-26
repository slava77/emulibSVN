#ifndef EmuTFfiller_h
#define EmuTFfiller_h 

#include "EmuTFbookkeeper.h"
#include "EmuTFxmlParsing.h"

class EmuTFfiller {
private:
	EmuTFbookkeeper tf;
	int nevents;

public:
	void fill(const unsigned short *buffer, unsigned int size, unsigned int errorFlag);

	EmuTFbookkeeper& bookkeeper(void){ return tf; }

	EmuTFfiller(std::map<std::string,EmuTFxmlParsing::HistAttributes> histList):tf(histList),nevents(0){}
	~EmuTFfiller(void){}

};

#endif
