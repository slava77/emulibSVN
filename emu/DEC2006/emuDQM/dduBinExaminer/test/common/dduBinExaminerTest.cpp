//	PROGRAM: 	dduBinExaminer	v 1
//	Authors:	V. Barashko, A. Korytov, K. Kotov, July 13, 2004
//=============================================================================================
// 	This program checks DDU binary datafile for event format integrity.
//	It reads a requested binary file and:
//	1) searches for Control Words (Headers, Trailers or other Signatures)
//	2) checks for their proper sequence
//	3) counts words and checks for consistency with 
//		a) the expected nominal count (CFEB samples, ALCT, TMB)
//		b) the number of words counted by various boards (ALCT, TMB, DDU)
//	4) prints out structure for each event (can be suppressed by redirecting it to /dev/null)
//	5) prints out warnings for each event (can be suppressed by redirecting it to /dev/null)
//	6) print out errors for each event
//	6) prints out the finals statistics of errors and warnings for the whole file
//	7) produces a few histograms (ad hoc feature)
//==============================================================================================

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include <set>
#include <vector>

// == ROOT Section
#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TString.h>
// ==
#include <getopt.h>
#include "dduBinExaminer.hh"

using namespace std;


// == Book histograms
map<string, TH1*> book(void){
        map<string, TH1*> h;
        string hname;

	// == Define Histo name, and, then, new Histo object
        hname = "hDDUWordCount";
        h[hname] = new TH1F(hname.c_str(), "DDU Word Count", 10000 ,  0 , 10000);

        hname = "hDDUActualWordCount_NoTrailerEvents";
        h[hname] = new TH1F(hname.c_str(), "DDU Actual Word Count (NoTrailer Events)", 10000 ,  0 , 10000);

        hname = "hDDUTrailerHeaderGap_WordCount";
        h[hname] = new TH1F(hname.c_str(), "DDU Trailer-Header Gap Words", 10000 ,  0 , 10000);

	// == Return list of Histos
        return h;
}

// == Save histograms to ROOT file
void savehistos(const char* filename, map<string, TH1*> histos){
	cerr << "Saving Histograms to "<< filename << endl;
	TFile f(filename, "recreate");
	f.cd();

     	for (map<string, TH1*>::iterator h_itr = histos.begin();
             h_itr != histos.end(); ++h_itr) {
        	h_itr->second->Write();
     	}

  	f.Close();
}

// == Prints four 16bits words in Hex
void printb(unsigned short* buf){
	for (int i=0; i<4; i++)
		cout << " " << setw(4)<< setfill('0') << hex << buf[i];
	cout << dec << endl;
}

// == Main =======================================================================================

int main(int argc, char **argv){
	dduBinExaminer examiner;

	//typedef long ERRARR[examiner.nERRORS];
	//typedef long WRNARR[examiner.nWARNINGS];

	long cntERROR[examiner.nERRORS], cntWARNING[examiner.nWARNINGS], cntBadEvent=0, unknownChamber=0;
	map< int, map<int,long> > cntChambERROR;
	map< int, map<int,long> > cntChambWARNING;

	// set ERROR and WARNING counters and flags to zero
	bzero(cntERROR,   sizeof(cntERROR)  );
	bzero(cntWARNING, sizeof(cntWARNING));

	examiner.output1().show();
	examiner.crcALCT(true);
	examiner.crcTMB (true);
	examiner.crcCFEB(true);

// For error's timelines
	const int tlbinsize=100;
	vector<int> *tlerror[examiner.nERRORS], *tlwarn[examiner.nWARNINGS];
	bzero(tlerror,sizeof(tlerror));
	for(int nerr=0; nerr<examiner.nERRORS;   nerr++) tlerror[nerr] = new vector<int>(1000000/tlbinsize);
	for(int nwrn=0; nwrn<examiner.nWARNINGS; nwrn++) tlwarn [nwrn] = new vector<int>(1000000/tlbinsize);

	string datafile="";
	string rootfile="dduBinExaminer.root";


	// == Book histograms
	map<string, TH1*> h = book();

	// == Process command line options
	if( argc<2 ) { printf("Nothing to be done\nType -h for help\n"); exit(0); } else { datafile = argv[1]; }
	ofstream log_stat;
	ofstream log_err [examiner.nERRORS], log_warn [examiner.nWARNINGS];
	set<int> errEvent[examiner.nERRORS], warnEvent[examiner.nWARNINGS], badEvent;
	static struct option options[] = {
		{"help"      ,0, 0, 'h'}, {"statistic" ,0, 0, 's'}, {"error"    ,1, 0, 'e'},
		{"warning"   ,1, 0, 'w'}, {"datafile"  ,1, 0, 'd'}, {"rootfile" ,1, 0, 'r'},
		{"noALCTCRC" ,0, 0, 'n'}, {"noCFEBCRC" ,0, 0, 'n'}, {"noTMBCRC" ,0, 0, 'n'},
		{0, 0, 0, 0}
	};
	while (1) {
		int index=0; char *tmp,*token; int errnum,warnnum; string tmpname;
		int c = getopt_long(argc, argv, "nhse:w:d:r:",options, &index);
		if (c == -1)	break;
		switch (c){
			case 'h':
				printf("Usage:\n");
				printf("-h         ,   --help              shows this message\n");
				printf("-d INPUT   ,   --datafile=INPUT    raw DDU file\n");
				printf("-r OUTPUT  ,   --rootfile=OUTPUT   output with histograms\n");
				printf("-s [NAME]  ,   --statistic=[NAME]  log information about bad events into runErrStat.all or [NAME] if specified\n");
				printf("-e E1,..,En,   --error=E1,..En     log information about particular errors into runErrStat.[NUMBER]\n");
				printf("-w W1,..,Wn,   --warning=W1,..Wn   log information about particular warnings into runWarnStat.[NUMBER]\n");
				printf("--noALCTCRC                        disable ALCT CRC code\n");
				printf("--noCFEBCRC                        disable CFEB CRC code\n");
				printf("--noTMBCRC                         disable TMB CRC code\n");
				printf("\nFirst two non-key arguments are considered to be 'datafile' and 'rootfile'");
				printf("\n");
				exit(0);
			case 's':
				tmpname = ( optarg && strlen(optarg) ? optarg : "runErrStat.all" ) ;
				log_stat.open(tmpname.c_str());
				break;
			case 'e':
				token = tmp = strdup(optarg);
				while(1){
					if((token = strrchr(tmp,','))) *token++ = '\0'; else token = tmp;
					if(!strlen(token)) continue;
					errnum = atoi(token);
					if( errnum<0 || errnum>=19 ){
						printf("Wrong error number: %s  ( 0 <= sould_be <= 19 )\n",token);
						exit(0);
					}
					tmpname = "runErrStat.";
					tmpname.append(token);
					log_err[errnum].open(tmpname.c_str());
					if(token == tmp)break;
				}
				free(tmp);
				break;
			case 'w':
				token = tmp = strdup(optarg);
				while(1){
					if((token = strrchr(tmp,','))) *token++ = '\0'; else token = tmp;
					if(!strlen(token)) continue;
					warnnum = atoi(token);
					if( warnnum<0 || warnnum>=5 ){
						printf("Wrong warning number: %s  ( 0 <= sould_be <= 5 )\n",token);
						exit(0);
					}
					tmpname = "runWarnStat.";
					tmpname.append(token);
					log_warn[warnnum].open(tmpname.c_str());
					if(token == tmp)break;
				}
				free(tmp);
				break;
			case 'd': datafile = optarg; break;
			case 'r': rootfile = optarg; break;
			case 'n': break;
			default : printf("Type -h for help\n"); return 0;
		}
	}
	for(int arg=1; arg<argc; arg++){
		if( !strcmp("--noALCTCRC",argv[arg]) )
			examiner.crcALCT(false);
		if( !strcmp("--noTMBCRC", argv[arg]) )
			examiner.crcTMB(false);
		if( !strcmp("--noCFEBCRC",argv[arg]) )
			examiner.crcCFEB(false);
	}

	// == Open input data file
	ifstream input(datafile.c_str());
	if (!input) { perror(datafile.c_str()); return -1; }
	cerr << datafile << " Opened" << endl;

	const int bufferSize = 116384;
	unsigned short buffer[bufferSize];

	unsigned long iteration=1;

//------------------------------------------------------------------------------------------------

	// == Read from datafile 4 16bit words into buf till end-of-file found
	while(!input.eof()){

		input.read((char *)(buffer), bufferSize*sizeof(short));

		const unsigned short *buf = buffer;

		long length = input.gcount()/sizeof(short);

		while( (length = examiner.check(buf,length)) >= 0 ){
			h["hDDUWordCount"]->Fill((buf-buffer)/4.);
			// increment statistics Errors and Warnings
			for(int err=0; err<examiner.nERRORS; err++){
				set<int> chamb_err = examiner.chambersWithError(err);
				set<int>::const_iterator chamber = chamb_err.begin();
				while( chamber != chamb_err.end() ) cntChambERROR[*chamber++][err]++;
				if( examiner.error(err) ){
					cntERROR[err]++;
					errEvent[err].insert(iteration);
				}
				badEvent.insert(iteration);
			}

			for(int wrn=0; wrn<examiner.nWARNINGS; wrn++){
				set<int> chamb_wrn = examiner.chambersWithWarning(wrn);
				set<int>::const_iterator chamber = chamb_wrn.begin();
				while( chamber != chamb_wrn.end() ) cntChambWARNING[*chamber++][wrn]++;
				if( examiner.warning(wrn) ){
					cntWARNING[wrn]++;
					warnEvent[wrn].insert(iteration);
				}
			}
			if( examiner.errors() ) cntBadEvent++;
			if( examiner.errorsForChamber(-2) ) unknownChamber++;

			if( iteration % tlbinsize == 0 )
				for(int err=0; err<examiner.nERRORS; err++){
					if( tlerror[err] && tlerror[err]->size() <= iteration/tlbinsize )
						tlerror[err]->resize( tlerror[err]->size() + iteration/tlbinsize );
					if( tlerror[err] ) (*tlerror[err])[iteration/tlbinsize] = cntERROR[err];
				}
			if( iteration % tlbinsize == 0 )
				for(int wrn=0; wrn<examiner.nWARNINGS; wrn++){
					if( tlwarn[wrn] && tlwarn[wrn]->size() <= iteration/tlbinsize )
						tlwarn[wrn]->resize( tlwarn[wrn]->size() + iteration/tlbinsize );
					if( tlwarn[wrn] ) (*tlwarn [wrn])[iteration/tlbinsize] = cntWARNING[wrn];
				}

			// == Print some intermediate stats after so many trailers read
			if( (iteration%5000)==0 ){
				cerr << endl << "DDU Headers: " << examiner.cntDDU_Headers << endl;
				//cerr << "Trailers:    " << cntDDU_Trailers << endl;
				cerr << "Bad Events:  " << cntBadEvent
					<< "   or " << (cntBadEvent/examiner.cntDDU_Headers) << " x Number of DDU Headers" << endl;

				for(int err=0; err<examiner.nERRORS; err++)
					if( cntERROR[err] != 0 )
						cerr << "ERROR " << err << "  " << examiner.errName(err) << "    " << cntERROR[err] << endl;

				for(int wrn=0; wrn<examiner.nWARNINGS; wrn++)
					if( cntWARNING[wrn] != 0 )
						cerr << "WARNING " << wrn << "  " << examiner.wrnName(wrn) << "    " << cntWARNING[wrn] << endl;
			}

			if( examiner.error(18) ) cout<<"ERROR 18"<<endl;
			iteration++;
		}

	}

	// == Print out the final stats
	cerr << endl << endl << "  " << datafile << endl;
	cerr << endl << endl << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
	cerr << " DDU Headers:                  " << examiner.cntDDU_Headers << endl;
	cerr << " DDUTrailers:                  " << examiner.cntDDU_Trailers << endl;
	cerr << " Bad Events:                   " << cntBadEvent
		 << "  or  " << (float(cntBadEvent)/examiner.cntDDU_Headers) << " x Number of DDU Headers" << endl;

	const char *descr;

	cerr << endl << " NUMBER OF EVENTS WITH FOLLOWING ERRORS: " << endl;
	for(int err=0; err<examiner.nERRORS; err++)
		if( (descr=examiner.errName(err)) && strlen(descr) )
			cerr << "  ERROR " << err << "  " << descr << "    " << cntERROR[err] << endl;

	cerr << endl << " NUMBER OF EVENTS WITH FOLLOWING WARNINGS: " << endl;
	for(int wrn=0; wrn<examiner.nWARNINGS; wrn++)
		if( (descr=examiner.wrnName(wrn)) && strlen(descr) )
			cerr << "  WARNING " << wrn << "  " << descr << "    " << cntWARNING[wrn] << endl;

	cerr << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl << endl;
	// == Save Histos to ROOT file
	savehistos(rootfile.c_str(), h);

	// == Close input data file
	input.close();
	cerr << datafile << " Closed" << endl;

	//hash_map< int, hash_map<int,long> >::iterator chamber = cntChambERROR.begin();
	//while( chamber != cntChambERROR.end() ){
	map<int,long>::const_iterator chamber = examiner.cntCHAMB_Headers.begin();
	while( chamber != examiner.cntCHAMB_Headers.end() ){
		cerr << endl << endl << endl;
		cerr << "-------------------------------------------------------------------------" << endl << endl;
		cerr << " Chamber: "<< chamber->first << " ( crate: " << (chamber->first>>4) << " slot: " << (chamber->first&0x0F) << " )" << endl;
		cerr << chamber->first << "DMB Headers:                  " << examiner.cntCHAMB_Headers[chamber->first] << endl;
		cerr << chamber->first << "DMBTrailers:                  " << examiner.cntCHAMB_Trailers[chamber->first] << endl;
		//cerr << " Bad Events:                   " << cntBadEvent
		//	 << "  or  " << (float(cntBadEvent)/examiner.cntDDU_Headers) << " x Number of DDU Headers" << endl;

		const char *descr;

		cerr << endl << " NUMBER OF EVENTS WITH FOLLOWING ERRORS: " << endl;
		if( (descr=examiner.errName(0)) && strlen(descr) )
			cerr << chamber->first << " ERROR " << 0 << "  " << descr << "    " << cntChambERROR[chamber->first][0] << endl;
		for(int err=5; err<24; err++){
			if( err==19 || err==20 ) continue;
			if( (descr=examiner.errName(err)) && strlen(descr) )
				cerr << chamber->first << " ERROR " << err << "  " << descr << "    " << cntChambERROR[chamber->first][err] << endl;
		}
		cerr << endl << " NUMBER OF EVENTS WITH FOLLOWING WARNINGS: " << endl;
		for(int wrn=1; wrn<examiner.nWARNINGS; wrn++)
			if( (descr=examiner.wrnName(wrn)) && strlen(descr) )
				cerr << chamber->first << " WARNING " << wrn << "  " << descr << "    " << cntChambWARNING[chamber->first][wrn] << endl;

		chamber++;
	}

		cerr << endl << endl << endl;
		cerr << "-------------------------------------------------------------------------" << endl << endl;
		cerr << " Chamber: -1" << endl;
		cerr << "-1DMB Headers:                  " << unknownChamber << endl;
		cerr << "-1DMBTrailers:                  " << unknownChamber << endl;

		//const char *descr;

		cerr << endl << " NUMBER OF EVENTS WITH FOLLOWING ERRORS: " << endl;
		if( (descr=examiner.errName(0)) && strlen(descr) )
			cerr << "-1 ERROR " << 0 << "  " << descr << "    " << cntChambERROR[-2][0] << endl;
		for(int err=5; err<24; err++){
			if( err==19 || err==20 ) continue;
			if( (descr=examiner.errName(err)) && strlen(descr) )
				cerr << "-1 ERROR " << err << "  " << descr << "    " << cntChambERROR[-2][err] << endl;
		}
		cerr << endl << " NUMBER OF EVENTS WITH FOLLOWING WARNINGS: " << endl;
		for(int wrn=1; wrn<examiner.nWARNINGS; wrn++)
			if( (descr=examiner.wrnName(wrn)) && strlen(descr) )
				cerr << "-1 WARNING " << wrn << "  " << descr << "    " << cntChambWARNING[-2][wrn] << endl;


	cerr << "-------------------------------------------------------------------------" << endl << endl;


	////////////////////////////// output files
	for(int err=0; err<examiner.nERRORS; err++){ // Errors
		if( log_err[err].is_open() ){
			set<int>::const_iterator iter = errEvent[err].begin();
			while( iter != errEvent[err].end() ){ log_err[err]<<(*iter)<<endl; iter++; }
		}
	}
	for(int wrn=0; wrn<examiner.nWARNINGS; wrn++){ // Warnings
		if( log_warn[wrn].is_open() ){
			set<int>::const_iterator iter = warnEvent[wrn].begin();
			while( iter != warnEvent[wrn].end() ){ log_warn[wrn]<<(*iter)<<endl; iter++; }
		}
	}
	if( log_stat.is_open() ){
		set<int>::const_iterator iter = badEvent.begin();
		while( iter != badEvent.end() ){
			log_stat<<(*iter)<<"  	";
			for(int err=0; err<examiner.nERRORS; err++){ // Errors
				set<int>::const_iterator occurance = errEvent[err].find((*iter));
				if( occurance != errEvent[err].end() ) log_stat<<"*"; else log_stat<<".";
			}
			log_stat<<endl;
			iter++;
		}
	}

	// For error's timelines
	if( cntBadEvent ){
		const char *descr;
		TFile f(rootfile.c_str(), "UPDATE");
		for(unsigned int i=0,bin=1; i<examiner.nERRORS; i++,bin=1){
			if( !tlerror[i] ) continue;
			char buff[128]; sprintf(buff,"error%d",i);
			TH1F err(buff,buff,iteration/tlbinsize,1,iteration);
			if( (descr=examiner.errName(i)) && strlen(descr) < sizeof(buff) ){
				sprintf(buff,"%s",descr);
				char  *pointer = buff+strlen(buff);
				while( pointer>buff && *(pointer-1) == ' ' ) pointer--;
				*pointer = '\0';
				char buff2[128];
				sprintf(buff2,"Error %d:%s",i,buff);
				err.SetTitle(buff2);
			}
			err.GetXaxis()->SetTitle("Event number");
			sprintf(buff,"Errors per bin (%d Events)",tlbinsize);
			err.GetYaxis()->SetTitle(buff);
			vector<int>::const_iterator iter = tlerror[i]->begin(), prev = iter;
			while( iter != tlerror[i]->end() && *prev <= *iter && iteration >= tlbinsize*bin ){
				int nerrors = ( iter==prev ? *iter : *iter-*prev );
				err.SetBinContent(bin,nerrors);
				prev = iter; iter++; bin++;
			}
			err.SetEntries(cntERROR[i]);
			err.Write();
		}
		for(unsigned int i=0,bin=1; i<examiner.nWARNINGS; i++,bin=1){
			if( !tlwarn[i] ) continue;
			char buff[128]; sprintf(buff,"warning%d",i);
			TH1F wrn(buff,buff,iteration/tlbinsize,1,iteration);
			if( (descr=examiner.wrnName(i)) && strlen(descr) < sizeof(buff) ){
				sprintf(buff,"%s",descr);
				char  *pointer = buff+strlen(buff);
				while( pointer>buff && *(pointer-1) == ' ' ) pointer--;
				*pointer = '\0';
				char buff2[128];
				sprintf(buff2,"Warning %d:%s",i,buff);
				wrn.SetTitle(buff2);
			}
			wrn.GetXaxis()->SetTitle("Event number");
			sprintf(buff,"Warnings per bin (%d Events)",tlbinsize);
			wrn.GetYaxis()->SetTitle(buff);
			vector<int>::const_iterator iter = tlwarn[i]->begin(), prev = iter;
			while( iter != tlwarn[i]->end() && *prev <= *iter && iteration >= tlbinsize*bin ){
				int nwarnings = ( iter==prev ? *iter : *iter-*prev );
				wrn.SetBinContent(bin,nwarnings);
				prev = iter; iter++; bin++;
			}
			wrn.SetEntries(cntWARNING[i]);
			wrn.Write();
		}
	  	f.Close();
	}

// == Exit from program
	return 0;
}
