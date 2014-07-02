int hv_ana(const char* file, int events, int chans);
int hv_ana2(const char* file, int events, int chans, int start);
int hv_ana3(const char* file, int events, int chans, int start, int nthresh, int adcthresh);
int hv_ana4(const char* file, int events, int chans, int start, int nthresh, int adcthresh, float rmsthresh);
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdlib.h>

#ifndef __CINT__
#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#include "Riostream.h"
#include "TCanvas.h"
#include "TFrame.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TImage.h"

using namespace std;

int nmax=-1;
string dfile = "";
int nchans=36;
int nstart=0;
int nthresh = 1;
int adcthresh = 10;
float rmsthresh = 1.0;
int testnum=19;
float temperature=0.0;
float humidity = 0.0;
int primaryhv = -1;
int boarddac = -1;
int runtime=0;
// TROOT root("rint", "The ROOT Interactive Interface");

void usage(char * name)
{
	cout << "Usage: " << name << " datafile [optional parameters]" << endl;
	cout << "Parameters in ascending order:" << endl;
	cout << "2 - Maximum number of points to analyze (-1 ALL)" << endl;
	cout << "3 - Number of  channels in HV module (default - " << nchans <<")" << endl;
	cout << "4 - Analysis starting point (default - " << nstart << ")" << endl;
	cout << "5 - Number of points for spike filtering (default - " << nthresh <<")" << endl;
	cout << "6 - +/- ADC range for spike detection (default - " << adcthresh << ")" << endl;
	cout << "7 - Maximal RMS limit (default - "<< rmsthresh << ")" << endl;
	cout << "8 - Test number (default - "<<testnum << ")" << endl;
	cout << "9 - Boards Voltage (default - " << boarddac <<")" << endl;
	cout << "10 - Primary Power Supply Voltage (default - " << primaryhv << ")" << endl;
	cout << "11 - Ambient temperature in C (default - " << temperature << ")" << endl;
	cout << "12 - Humidity in % (default - " << humidity<< ")" << endl;
	cout << "13 - Data saving runtime in seconds (default - " << runtime << ")" << endl;

	
};

int main(int argc, char **argv) 
{
	if (argc<2) {usage(argv[0]); return 0;};
	switch (argc) {
	case 14:  runtime = atoi(argv[13]);	// Runtime in seconds
	case 13: humidity = atof(argv[12]);	// Humidity
	case 12: temperature = atof(argv[11]);	// Temperature	
	case 11: primaryhv = atoi(argv[10]);	// Primary Power Supply Voltage
	case 10: boarddac = atoi(argv[9]);	// Boards Voltage DAC
	case 9: testnum = atoi(argv[8]);	// Test number
	case 8: rmsthresh = atof(argv[7]);	// Maximal RMS  
	case 7: adcthresh = atoi(argv[6]);	// +/- ADC range for spike detection
	case 6: nthresh = atoi(argv[5]);	// Number of points for spike filtering
	case 5: nstart = atoi(argv[4]);		// Analysis starting point  
      	case 4: nchans = atoi(argv[3]);		// Number of channels in HV module
      	case 3: nmax = atoi(argv[2]);		// Maximum number of points to analyze (-1 ALL)
      	case 2: dfile = argv[1];		// HV Data file name
                break;
        }
	// TApplication theApp("HVTest", &argc, argv);
	hv_ana4(dfile.c_str(), nmax, nchans, nstart, nthresh, adcthresh, rmsthresh);
	// theApp.Run(false);
}

#endif

void padRefresh(TPad *pad,int flag=0);

int hv_ana(const char* file, int events, int chans)
{
	return hv_ana2(file, events, chans, 0);
}

int hv_ana2(const char* file, int events, int chans, int start)
{
	return hv_ana3(file, events, chans, start,  1, 25);
}

int hv_ana3(const char* file, int events, int chans, int start, int nthresh, int adcthresh)
{
	return hv_ana4(file, events, chans, start, nthresh, adcthresh, 1.5);
}

int hv_ana4(const char* file, int events, int chans, int start, int nthresh, int adcthresh, float rmsthresh)
{

   	ifstream in;
   	ofstream out;
	ofstream htmlout;
	ofstream log;
	ostringstream st;
	int nmax=events;
	string dfile(file);
	string filen="dat";
	int nchans=(chans>37) ?36 :((chans<1)? 1:chans);
	int limit = 2*adcthresh;
	float rmslimit = rmsthresh;
	/*
	switch (argc) {
	case 4: nchans = atoi(argv[3]);
	case 3: nmax = atoi(argv[2]);
	case 2: dfile = argv[1];
		break;
	}	
	*/
	cout  << dfile << endl;
	// TApplication theApp("HVTest", &argc, argv);
        Int_t i, cur[36], min[36], max[36], posmin[36], posmax[36];
        TH1F* histos[36];	
	TH1F* triphisto;

   	Int_t nlines = 0;
        char name[255];
	char hname[255];
	int tripcnt[36];
	int mean[36];
	TCanvas* mainCanvas;
	TH2F *h2;
	memset(tripcnt, 0, sizeof(tripcnt));
	memset(mean, 0, sizeof(mean));

	if (dfile.find(".dat") != string::npos)
		filen = dfile.substr(0, dfile.find(".dat"));
	// cout << filen << endl;
	TDatime *now = new (TDatime);

	if (!dfile.size()) {
		cerr << "Empty input filename. Exiting..." << endl;
		return -1;
	}
	
	in.open(dfile.c_str());
	if (!in.good()) {
		cerr << "Unable to open '" << dfile << "' Exiting..." << endl;
		return -1;
	}
	st.clear();
	st << filen;
	if (primaryhv > -1) st <<  "_V" << primaryhv;
	if (boarddac > -1) st << "-" << boarddac;
	if (temperature > 0.0) st << "_T" << ((int)temperature);
	if (humidity >0.0) st << "_H" << ((int)humidity);
	/*
	sprintf(name, "%s_%08d_%06d.log",filen.c_str(), now->GetDate(), now->GetTime());
	log.open(name, ios::out | ios::app);
	sprintf(name, "%s_%08d_%06d.res",filen.c_str(), now->GetDate(), now->GetTime());
	out.open(name);
	sprintf(name, "%s_%08d_%06d.html",filen.c_str(), now->GetDate(), now->GetTime());
	htmlout.open(name);
	// printf("%s\n", name);
	*/
	nlines = 0;
	sprintf(name, "%s_%08d_%06d.root", filen.c_str(), now->GetDate(), now->GetTime());
	TFile *f = new TFile(name, "RECREATE");
	char line[255];
	bool fFirstRun = true;
	int realnchans = 0;
     while (!in.eof()) {
	in.getline(line, 255);
	nlines++;
	stringstream st(line);
	if (fFirstRun) {
		while (st >> cur[realnchans]) realnchans++;
		nchans = realnchans;
		sprintf(hname, "%s ADC Distribution", dfile.c_str());
        	for (i=0; i< nchans; i++) {
                	sprintf(name, "hCh%02d", i);
                	sprintf(hname, "%s: %s: Ch%d ADC",now->AsSQLString(), filen.c_str(), i);
                	histos[i] = new TH1F(name,hname,4192,0,4192 );
        	}
		/*
        	triphisto = new TH1F("trips","Trips on channels", nchans,0,nchans );	
        	mainCanvas = new TCanvas("HVTest","HVTest",200, 10, 1000,900);
        	// mainCanvas->SetFillColor(42);
        	// mainCanvas->GetFrame()->SetFillColor(21);
        	mainCanvas->GetFrame()->SetBorderSize(6);
        	mainCanvas->GetFrame()->SetBorderMode(-1);
		*/
        	sprintf(hname, "%s: %s ADC Distribution", now->AsSQLString(), filen.c_str());
        	h2 = new TH2F("h2",hname,4192,0,4192, nchans, 0, nchans );
        	for(i=0; i<nchans; i++) {
                	min[i]=4192;
                	max[i]=0;
        	} 
		fFirstRun=false;

	}
	if (nlines && (nlines%10000) == 0) printf("...%d\n", nlines);
	if (nlines >= start) {
  	for (i=0; i<nchans; i++) {
  		st >> cur[i];
  	}
	/*
	if (nlines && (nlines%1000) == 0)
          {
  		printf("%d points analyzed\n", nlines);
  		// for (i=0; i<5; i++) printf("%d ", cur[i]);
//		printf("%d ", cur[0]);
//  		printf("\n");
		h2->Draw();
		padRefresh(mainCanvas);
          }
	*/
	if (nlines==nmax+start+1) break;
  	for (i=0; i<nchans; i++) {
		if (nlines==1000) mean[i]=(int)histos[i]->GetMean();
		if (( (abs(cur[i] - mean[i])) > adcthresh) && (nlines > 1000)) {
			tripcnt[i]++;
			if (tripcnt[i]>=nthresh) {
  	     			histos[i]->Fill(cur[i]);
				h2->Fill(cur[i], i);
				
			}
			else continue;
		}
		else {
			/*
			if (tripcnt[i] >= nthresh) {
				cout << "ch:" << i+1 << " tripcnt:" << tripcnt[i] << endl;
				triphisto->Fill(i);
			}
			*/
			tripcnt[i]=0;
		}
		if (tripcnt[i]==0) {
			histos[i]->Fill(cur[i]);
  	     		h2->Fill(cur[i], i);
		}

  	     	if (cur[i] < min[i]) {min[i]=cur[i]; posmin[i]=nlines;};
  	     	if (cur[i] > max[i]) {max[i]=cur[i]; posmax[i]=nlines;};
  	 }
	}
		if (gROOT->IsInterrupted())
                        break;
     }
	/*
	sprintf(name, "%s_%08d_%06d.gif", filen.c_str(), now->GetDate(), now->GetTime());
	mainCanvas->Print(name);

	htmlout << "<HTML>\n<HEAD>\n<TITLE>Results for " << dfile << "</TITLE></HEAD>\n<BODY>\n<IMG SRC=" << name << "><BR><BR>\n" << endl;
	sprintf(name, "%s_%08d_%06d.root", filen.c_str(), now->GetDate(), now->GetTime());
	htmlout << "Results ROOT File <A HREF=" << name<< ">" << name<< "</A><BR><BR>\n" << endl; 
	htmlout << "<PRE>\n" << endl;
	
	log << "========================================================" << endl;
	log << "Test" << testnum << " results " << now->AsSQLString() << endl;
	log << "Data file: " << dfile << endl;
	log << "Primary Power Supply Voltage: " << primaryhv << ((primaryhv<0)? "(unknown)" : " V") << endl;
	log << "Board Voltage DAC: " << boarddac << ((boarddac<0)? "(unknown)" : " ") << endl;
	log << "Spike filtering: " << nthresh << ((nthresh>1)?" ":" (no filtering)") << " points" << endl;
	log << "ADC Variation range: +/-" << adcthresh << " counts" << endl;
	log << "Maximal RMS limit: " << rmslimit << endl;
	log << "Temperature (C): " << temperature << (temperature>0?" ": " (unknown)" )<< endl;
	log << "Humidity (%): " << humidity << (humidity>0?" ": " (unknown)")<< endl;
	log << "Runtime: " << (runtime>0?runtime:(nlines/30)) << " seconds" << endl;
	log  << "Analyzed " << (nlines - start-1) <<   " points\n";
	
	htmlout << "<HR>" << endl;
	htmlout << "Test" << testnum << " results " << now->AsSQLString() << endl;
	htmlout << "Data file: " << dfile << endl;
	htmlout << "Primary Power Supply Voltage: " << primaryhv << ((primaryhv<0)? "(unknown)" : " V") << endl;
	htmlout << "Board Voltage DAC: " << boarddac << ((boarddac<0)? "(unknown)" : " ") << endl;
	htmlout << "Spike filtering: " << nthresh << ((nthresh>1)?" ":" (no filtering)") << " points" << endl;
	htmlout << "ADC Variation range: +/-" << adcthresh << " counts" << endl;
	htmlout << "Maximal RMS limit: " << rmslimit << endl;
	htmlout << "Temperature (C): " << temperature << (temperature>0?" ": " (unknown)" )<< endl;
	htmlout << "Humidity (%): " << humidity << (humidity>0?" ": " (unknown)")<< endl;
	htmlout << "Runtime: " << (runtime>0?runtime:(nlines/30)) << " seconds" << endl;
	htmlout << "Analyzed " << (nlines - start-1) <<   " points\n";
   	
	out << "========================================================" << endl;
	out << "Test" << testnum << " results " << now->AsSQLString() << endl;
	out << "Data file: " << dfile << endl;
	out << "Primary Power Supply Voltage: " << primaryhv << ((primaryhv<0)? "(unknown)" : " V") << endl;
	out << "Board Voltage DAC: " << boarddac << ((boarddac<0)? "(unknown)" : " ") << endl;
	out << "Spike filtering: " << nthresh << ((nthresh>1)?" ":" (no filtering)") << " points" << endl;
	out << "ADC Variation range: +/-" << adcthresh << " counts" << endl;
	out << "Maximal RMS limit: " << rmslimit << endl;
	out << "Temperature (C): " << temperature << (temperature>0?" ": " (unknown)" )<< endl;
	out << "Humidity (%): " << humidity << (humidity>0?" ": " (unknown)")<< endl;
	out << "Runtime: " << (runtime>0?runtime:(nlines/30)) << " seconds" << endl;
	out << "Analyzed " << (nlines - start-1) <<   " points\n";

   	cout << "========================================================" << endl;
	cout << "Test" << testnum << " results " << now->AsSQLString() << endl;

	
	log << "\nCh#   MIN    MAX   MEAN  MEAN-MIN  MAX-MEAN   MAX-MIN          RMS" << endl;
	out << "\nCh#   MIN    MAX   MEAN  MEAN-MIN  MAX-MEAN   MAX-MIN          RMS" << endl;
	htmlout << "\nCh#   MIN    MAX   MEAN  MEAN-MIN  MAX-MEAN   MAX-MIN          RMS" << endl;
	cout << "\nCh#   MIN    MAX   MEAN  MEAN-MIN  MAX-MEAN   MAX-MIN          RMS" << endl;
	   
	for (i=0; i< nchans; i++) {	
		log << "" << setiosflags(ios_base::right) << setw(2) << setfill('0') << i+1 << setfill(' ')<< setw(7) << min[i]  <<  setw(7) << max[i]<< setw(7) << (int)(histos[i]->GetMean()) << 
			setw (10) <<  ((int)(histos[i]->GetMean())-min[i]) << setw (10) <<  (max[i]-(int)(histos[i]->GetMean())) <<
			setw (10) <<  max[i]-min[i] << setw(4) << ((max[i]-min[i]>limit)? " (!)":"") <<   setw(9)    << setprecision(3) << histos[i]->GetRMS() << ((histos[i]->GetRMS() > rmslimit)? " (!)":"") << endl;				
		out << "" << setiosflags(ios_base::right) << setw(2) << setfill('0') << i+1 << setfill(' ')<< setw(7) << min[i]  <<  setw(7) << max[i]<< setw(7) << (int)(histos[i]->GetMean()) << 
			setw (10) <<  ((int)(histos[i]->GetMean())-min[i]) << setw (10) <<  (max[i]-(int)(histos[i]->GetMean())) <<
			setw (10) <<  max[i]-min[i] << setw(4) << ((max[i]-min[i]>limit)? " (!)":"") <<   setw(9)    << setprecision(3) << histos[i]->GetRMS() << ((histos[i]->GetRMS() > rmslimit)? " (!)":"") << endl;				
		htmlout << "" << setiosflags(ios_base::right) << setw(2) << setfill('0') << i+1 << setfill(' ')<< setw(7) << min[i]  <<  setw(7) << max[i]<< setw(7) << (int)(histos[i]->GetMean()) << 
			setw (10) <<  ((int)(histos[i]->GetMean())-min[i]) << setw (10) <<  (max[i]-(int)(histos[i]->GetMean())) <<
			setw (10) <<  max[i]-min[i] << setw(4) << ((max[i]-min[i]>limit)? " (!)":"") <<   setw(9)    << setprecision(3) << histos[i]->GetRMS() << ((histos[i]->GetRMS() > rmslimit)? " (!)":"") << endl;				
		cout << "" << setiosflags(ios_base::right) << setw(2) << setfill('0') << i+1 << setfill(' ')<< setw(7) << min[i]  <<  setw(7) << max[i]<< setw(7) << (int)(histos[i]->GetMean()) << 
			setw (10) <<  ((int)(histos[i]->GetMean())-min[i]) << setw (10) <<  (max[i]-(int)(histos[i]->GetMean())) <<
			setw (10) <<  max[i]-min[i] << setw(4) << ((max[i]-min[i]>limit)? " (!)":"") <<   setw(9)    << setprecision(3) << histos[i]->GetRMS() << ((histos[i]->GetRMS() > rmslimit)? " (!)":"") << endl;				
		
	}

	out << "\nMIN:\n";
	printf("\nMIN:\n");
     	for (i=0; i< nchans; i++) {
		out << "#" << i << ":" << posmin[i]<< "=" << min[i] << " ";
		printf("#%d:%d=%d ", i, posmin[i], min[i]);
     	}
	
	out << "\nMAX:\n";
     	printf("\nMAX:\n");
     	for (i=0; i< nchans; i++) {
		out << "#" << i << ":" << posmax[i]<< "=" << max[i] << " " ;
		printf("#%d:%d=%d ", i, posmax[i], max[i]);
     	}
	out << endl;
	cout << endl << endl;
	
	cout << "Data file: " << dfile << endl;
	cout << "Primary Power Supply Voltage: " << primaryhv << ((primaryhv<0)? "(unknown)" : " V") << endl;
	cout << "Board Voltage DAC: " << boarddac << ((boarddac<0)? "(unknown)" : " ") << endl;
	cout << "Spike filtering: " << nthresh << ((nthresh>1)?" ":" (no filtering)") << " points" << endl;
	cout << "ADC Variation range: +/-" << adcthresh << " counts" << endl;
	cout << "Maximal RMS limit: " << rmslimit << endl;
	cout << "Temperature (C): " << temperature << (temperature>0?" ": " (unknown)" )<< endl;
	cout << "Humidity (%): " << humidity << (humidity>0?" ": " (unknown)")<< endl;
	cout << "Runtime: " << (runtime>0?runtime:(nlines/30)) << " seconds" << endl;
	cout << "Analyzed " << (nlines - start-1) <<   " points\n";
	
	int erradc=0;
	log << "\nFailed Channels ADC range (+/-" << adcthresh << ") check:" <<endl;
	htmlout << "\nFailed Channels ADC range (+/-" << adcthresh << ") check:" <<endl;
	out << "\nFailed Channels ADC range (+/-" << adcthresh << ") check:" <<endl;
	cout << "\nFailed Channels ADC range (+/-" << adcthresh << ") check:" <<endl;

	for (i=0; i< nchans; i++) {
		if ((max[i]-min[i])>limit) {
			erradc++;
			log << "Ch#" << i+1 <<  ": " <<  (max[i]-min[i]) << endl;
			htmlout << "Ch#" << i+1 <<  ": " <<  (max[i]-min[i]) << endl;
			out << "Ch#" << i+1 <<  ": " <<  (max[i]-min[i]) << endl;
			cout << "Ch#" << i+1 <<  ": " <<  (max[i]-min[i]) << endl;
		}
	}	
	log << erradc << " channels" << endl;
	htmlout << erradc << " channels" << endl;
	out << erradc << " channels" << endl;
	cout << erradc << " channels" << endl;

	int errrms=0;
	log << "\nFailed Channels RMS (" << rmslimit << ") check:" <<endl;
	htmlout << "\nFailed Channels RMS (" << rmslimit << ") check:" <<endl;
	out << "\nFailed Channels RMS (" << rmslimit << ") check:" <<endl;
	cout << "\nFailed Channels RMS (" << rmslimit << ") check:" <<endl;
	for (i=0; i< nchans; i++) {		
		if ((histos[i]->GetRMS())>rmslimit) {
			errrms++;
			log << "Ch#" << i+1 <<  ": " <<  histos[i]->GetRMS() << endl;
			htmlout << "Ch#" << i+1 <<  ": " <<  histos[i]->GetRMS() << endl;
			out << "Ch#" << i+1 <<  ": " <<  histos[i]->GetRMS() << endl;
			cout << "Ch#" << i+1 <<  ": " <<  histos[i]->GetRMS() << endl;
		}
	}
	log << errrms << " channels" << endl;
	htmlout << errrms << " channels" << endl;
	out << errrms << " channels" << endl;
	cout << errrms << " channels" << endl;

	
	if (erradc || errrms) {
		log << "\nTest Result: Failed" << endl;
		out << "\nTest Result: Failed" << endl;
		cout << "\nTest Result: Failed" << endl;
		htmlout << "\nTest Result: Failed" << endl;
	} else {
		log << "\nTest Result: OK" << endl;
		out << "\nTest Result: OK" << endl;
		cout << "\nTest Result: OK" << endl;
		htmlout << "\nTest Result: OK" << endl;

	}
	cout << "========================================================" << endl;
	htmlout << "<HR>\n</PRE>\n</BODY>\n</HTML>" << endl;
	
	log << "========================================================" << endl << endl;
	log.clear();
	log.close();
	in.clear();
   	in.close();
	out.clear(); 
	out.close();
	htmlout.clear();
	htmlout.close();
	*/
   	f->Write();
//	TImage *img = TImage::Create();
//   	img->FromPad(mainCanvas);
//	sprintf(name, "%s.gif", dfile.c_str());
//	mainCanvas->Print(name);
//	img->WriteImage(name);
//	delete img;
	// theApp.Run();
	// delete mainCanvas;
	// delete h2;
	for (int i=0; i< nchans; i++) delete histos[i];
	// delete f;
	
	return 0;
}

void padRefresh(TPad *pad,int flag)
{
  if (!pad) return;
  pad->Modified();
  pad->Update();
  TList *tl = pad->GetListOfPrimitives();
  if (!tl) return;
  TListIter next(tl);
  TObject *to;
  while ((to=next())) {
    if (to->InheritsFrom(TPad::Class())) padRefresh((TPad*)to,1);}
  if (flag) return;
  gSystem->ProcessEvents();
}

