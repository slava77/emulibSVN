int hv_calib_plots(const char* file);
int plot_adc_calib(const char* file);
int plot_dac_calib(const char* file);
int plot_current_calib(const char* file);

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <algorithm>

#ifndef __CINT__
#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#include "Riostream.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TFrame.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TImage.h"

int gen_plots_browser(const char* file, int chans, TDatime* tnow);
using namespace std;

string dfile = "";
int width=1600;
int height=1200;
// TROOT root("rint", "The ROOT Interactive Interface");

void usage(char * name)
{
	cout << "Usage: " << name << " calibfile [optional parameters]" << endl;
/*	cout << "Parameters in ascending order:" << endl;
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
*/
	
};

int main(int argc, char **argv) 
{
	if (argc<2) {usage(argv[0]); return 0;};
	switch (argc) {
/*
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
*/ 
      	case 4: height = atoi(argv[3]);		// Number of channels in HV module
      	case 3: width = atoi(argv[2]);		// Maximum number of points to analyze (-1 ALL)

      	case 2: dfile = argv[1];		// HV Data file name
                break;
        }
	// TApplication theApp("HVTest", &argc, argv);
	hv_calib_plots(dfile.c_str());
	// hv_ana4(dfile.c_str(), nmax, nchans, nstart, nthresh, adcthresh, rmsthresh);
	// theApp.Run(false);
}

#endif

void padRefresh(TPad *pad,int flag=0);


int hv_calib_plots(const char* file) 
{
  stringstream st;
  char line[255];
  ifstream caldatafile(file);
  if (caldatafile) {
	cout << file << " Opened" << endl;
	while(!caldatafile.eof())
        {
          caldatafile.getline(line,255);
          if ((string(line).find("Test10") ==0) || (string(line).find("Test15") ==0)) {
		cout << "Found ADC Calibration data" << endl; 
		plot_adc_calib(file);
		break;
		
          }
          if ((string(line).find("Test11") ==0) || (string(line).find("Test16") ==0)) {
                cout << "Found DAC Calibration data" << endl;
                plot_dac_calib(file);
		break;
          }
	  if (string(line).find("Test9") ==0) {
                cout << "Found Current Calibration data" << endl;
                plot_current_calib(file);
		break;
          }
	  if (string(line).find("Test14") ==0) {
                cout << "Found Current Calibration data" << endl;
                plot_current_calib(file);
                break;
          }

        } 
	caldatafile.close();
  }
  return 0;
}

int plot_adc_calib(const char* file)
{
  stringstream st;
  char line[255];
  vector<float> A(36), B(36);
  bool fA=false, fB=false;
  int i=0;
  A.clear(); B.clear();
  ifstream adcfile(file);
  if (adcfile) {
      while(!adcfile.eof())
        {
          adcfile.getline(line,255);
          if (string(line).find("Limits") ==0) {
            adcfile.getline(line,255);
          }

          if (fA) {
            int cnt=0;

            string str(line);
            replace(str.begin(), str.end(), ',', ' ');
            stringstream st(str);

            float t;
            while (st >> t ) {
              A.push_back(t);
              cout << A[i]<<":";
              i++;
              cnt++;
            }
            cout << endl;
            if (cnt==0) fA=false;
          }
          if (fB) {
            int cnt=0;
            string str(line);
            replace(str.begin(), str.end(), ',', ' ');
            stringstream st(str);
            float t;
            while (st >> t ) {
              B.push_back(t);
              cout << B[i]<<":";
              i++;
              cnt++;
            }
            cout << endl;
            if (cnt==0) fB=false;
          }
          if (string(line).find("A:") == 0) {
            i=0;
            fB=false;
            fA=true;
            A.clear();
          }
          if (string(line).find("B:") ==0) {
            i=0;
            fA=false;
            fB=true;
            B.clear();
          }
	/*
          string str = string(line);
          int pos = str.find("A=", 0);
          if (pos != string::npos) {
            stringstream st(str.substr(pos+2, str.size()));
            st >> data.vmon.A;
            // cout << data.vmon.A << endl;
          }
          pos = str.find("B=", 0);
          if ( pos != string::npos) {
            stringstream stB(str.substr(pos+2, str.size()));
            stB >> data.vmon.B;
            // cout << data.vmon.B << endl;
          }
         */

        }
      adcfile.close();
    }

  cout << A.size() << endl;
  char name[255];
  TDatime *now = new (TDatime);
  sprintf(name, "%s_%08d_%06d.root", file, now->GetDate(), now->GetTime());
  // TFile *f = new TFile(name, "RECREATE");
  if (A.size()) {
        Int_t n = A.size();
        Float_t x[n], y[n];
        for (unsigned int i=0; i<A.size(); i++) {
           x[i]=i+1;
           y[i]=A[i];
        }


	TCanvas *mainCanvas = new TCanvas("CalADC","CalibADCPlot",200, 10, width, height);
                // mainCanvas->SetFillColor(42);
                // mainCanvas->GetFrame()->SetFillColor(21);
                // mainCanvas->GetFrame()->SetBorderSize(6);
                // mainCanvas->GetFrame()->SetBorderMode(-1);
        TGraph *tg = new TGraph(n,x,y);
	mainCanvas->Divide(1,2);
        tg = new TGraph(n,x,y);
        tg->SetFillColor(45);
	sprintf(name, "%s ADC Calibration Plot Slope (A)", file);
	tg->SetTitle(name);
        mainCanvas->cd(1);
        tg->Draw("AB");

        mainCanvas->cd(2);
        for (unsigned int i=0; i<B.size(); i++) {
           x[i]=i+1;
           y[i]=B[i];
        }
        TGraph *tg1 = new TGraph(n,x,y);
        tg1->SetFillColor(45);
	sprintf(name, "%s ADC Calibration Plot Offset (B)", file);
        tg1->SetTitle(name);
        tg1->Draw("AB");
        sprintf(name, "%s_%08d_%06d.png", file, now->GetDate(), now->GetTime());
        mainCanvas->Print(name);
        // gROOT->GetListOfSpecials()->Add(tg);
        mainCanvas->Draw();
	delete mainCanvas;
  	delete tg;
  	delete tg1;
  }

  return 0;
}

int plot_dac_calib(const char* file)
{
  stringstream st;
  char line[255];
  vector<float> A(36), B(36);
    bool fA=false, fB=false;
    int i=0;
    A.clear(); B.clear();
    ifstream dacfile(file);
    if (dacfile) {
      while(!dacfile.eof())
        {   
          dacfile.getline(line,255);
          if (string(line).find("Limits") ==0) {
            dacfile.getline(line,255);
          } 
          if (fA) {
            int cnt=0;
            
            string str(line);
            replace(str.begin(), str.end(), ',', ' ');
            stringstream st(str);
            
            float t;
            while (st >> t ) {
              A.push_back(t);
              cout << A[i]<<":";
              i++;
              cnt++;
            }
            cout << endl;
            if (cnt==0) fA=false;
          }
          if (fB) {
            int cnt=0;
            string str(line);
            replace(str.begin(), str.end(), ',', ' ');
            stringstream st(str);
            float t;
            while (st >> t ) {
              B.push_back(t);
                cout << B[i]<<":";
              i++;
              cnt++;
            }
            cout << endl;
            if (cnt==0) fB=false;
          }
          if (string(line).find("A:") == 0) {
            i=0;
            fB=false;
            fA=true;
            A.clear();
          }
          if (string(line).find("B:") ==0) {
            i=0;
            fA=false;
            fB=true;
            B.clear();
          }

        }
      dacfile.close();
    }

  cout << A.size() << endl;
  char name[255]; 
  TDatime *now = new (TDatime);
  sprintf(name, "%s_%08d_%06d.root", file, now->GetDate(), now->GetTime());
  // TFile *f = new TFile(name, "RECREATE");
  if (A.size()) {
        Int_t n = A.size();
        Float_t x[n], y[n];
        for (unsigned int i=0; i<A.size(); i++) { 
           x[i]=i+1;
           y[i]=A[i];
        }   
            
        TCanvas *mainCanvas = new TCanvas("CalDAC","CalibDACPlot",200, 10, width, height);
                // mainCanvas->SetFillColor(42);
                // mainCanvas->GetFrame()->SetFillColor(21);
                // mainCanvas->GetFrame()->SetBorderSize(6);
                // mainCanvas->GetFrame()->SetBorderMode(-1);
	mainCanvas->Divide(1,2);
	TGraph *tg = new TGraph(n,x,y);
        tg->SetFillColor(45);
	sprintf(name, "%s DAC Calibration Plot Slope (A)", file);
        tg->SetTitle(name);
	mainCanvas->cd(1);
	tg->Draw("AB");
	
	mainCanvas->cd(2);
	for (unsigned int i=0; i<B.size(); i++) {
           x[i]=i+1;
           y[i]=B[i];
        }
	TGraph *tg1 = new TGraph(n,x,y);
        tg1->SetFillColor(45);
	sprintf(name, "%s DAC Calibration Plot Offset (B)", file);
	tg1->SetTitle(name);
	tg1->Draw("AB");
        sprintf(name, "%s_%08d_%06d.png", file, now->GetDate(), now->GetTime());
        mainCanvas->Print(name);
        // gROOT->GetListOfSpecials()->Add(tg);
        mainCanvas->Draw();
	delete mainCanvas;
  	delete tg;
  	delete tg1;
  }

  return 0;
}

int plot_current_calib(const char* file)
{
  stringstream st;
  char line[255];
  bool fC=false;
  int i=0;
  vector< vector<float> > C;
  C.clear();
  ifstream curfile(file);
  if (curfile) {
          while(!curfile.eof())
            {
              curfile.getline(line,255);
              if (fC) { 
                int cnt=0;
                string str(line);
                replace(str.begin(), str.end(), ',', ' ');
                stringstream st(str);
                float t;
                char ch=0;
		vector<float> Cp;
                while (st >> t ) {
		  Cp.push_back(t);
		  // C[i][ch] = t;
                  // C[ch][(int)t] = i+1;
                  // C[ch][(int)t] = (i-MAIN_OFFS)<0?1:(i-MAIN_OFFS)+1;
                  ch++;
                  cnt++;
                }
                i++;
                if (cnt==0) { fC=false; }
		else { C.push_back(Cp); }
              }
              if (string(line).find("Current Tab:") ==0) {
                i=0; 
                fC=true; 
                C.clear();
              }
            }
          curfile.close();
        }
  cout << C.size() << endl;
  char name[255];
  TDatime *now = new (TDatime);
  sprintf(name, "%s_%08d_%06d.root", file, now->GetDate(), now->GetTime());
  // TCanvas * mainCanvas;
  // TGraph * tg;
  // TFile *f = new TFile(name, "RECREATE");
  vector<TGraph*> tgs;
  if (C.size() && C[0].size()) {
	Int_t n = C.size();
	Float_t x[n], y[n];
	// mainCanvas = new TCanvas("CalCurrent","CalibCurrentPlot",200, 10, 1600,1200);
	for (unsigned int ch=0; ch< C[0].size(); ch++) {
        	for (unsigned int i=0; i<C.size(); i++) {
	   	x[i]=i;
	   	y[i]=C[i][ch];
        	}
	
	//mainCanvas->cd();
	TGraph *tg = new TGraph(n,x,y);
	tgs.push_back(new TGraph(n,x,y));
        TCanvas *mainCanvas = new TCanvas("CalCurrent","CalibCurrentPlot",200, 10, width,height);
                // mainCanvas->SetFillColor(42);
                // mainCanvas->GetFrame()->SetFillColor(21);
                // mainCanvas->GetFrame()->SetBorderSize(6);
                // mainCanvas->GetFrame()->SetBorderMode(-1);
	sprintf(name, "%s Current Calibration Plot Ch#%02d", file,ch+1);
        tg->SetTitle(name);
	tg->SetLineWidth(1);
	tg->SetLineColor(2);
        // tg->SetLineColor(ch);
	tg->SetMaximum(3000);
        tg->SetMinimum(300);
	tg->Draw("AC");
	sprintf(name, "%s_ch%02d_%08d_%06d.png", file,ch+1, now->GetDate(), now->GetTime());
	mainCanvas->Print(name);
	mainCanvas->Draw();
	delete mainCanvas;
	delete tg;
	// gROOT->GetListOfSpecials()->Add(tg);
	}
	// mainCanvas->Draw();
  }
  TCanvas *allCanvas = new TCanvas("AllCalCurrent","AllCalibCurrentPlot",200, 10, 1600,1200);
  for (unsigned int i=0; i< tgs.size(); i++) {
	tgs[i]->SetLineWidth(1);
        tgs[i]->SetLineColor(i);
        tgs[i]->SetMaximum(3000);
	tgs[i]->SetMinimum(300);
        if (i==0) {
		sprintf(name, "%s All Channels Current Calibration Plot", file);
		tgs[i]->SetTitle(name);
		tgs[i]->Draw("AC"); 
	}
	else tgs[i]->Draw("C");
  }
  // allCanvas->Draw();
  sprintf(name, "%s_all_%08d_%06d.png", file, now->GetDate(), now->GetTime());
  allCanvas->Print(name);
  delete allCanvas;
  gen_plots_browser(file, C[0].size(), now);

  // f->Write();
  // f->Close();
  // delete f;
  // delete mainCanvas;

  return 0;
}

int gen_plots_browser(const char* file, int chans, TDatime* tnow)
{
  ofstream hout;
  char name[255];
  sprintf(name, "%08d_%06d", tnow->GetDate(), tnow->GetTime());
  string timestamp(name);
  string calibfile(file);
  sprintf(name, "%s_%08d_%06d.html",file, tnow->GetDate(), tnow->GetTime());
  hout.open(name);
  hout <<  "<html><head><title>Current Calibration Plots</title><style>"
  	<< "body {color: #000000; font-weight:bold ; font-family: Tahoma, Verdana; font-size: 12pt} "
	<< "td,a, A:link, a:visited, a:active, A:hover	"
  	<< "{color: #000000; text-decoration: none; font-family: Tahoma, Verdana; font-size: 10pt}</style>"
  	<< "<script type=\"text/javascript\">"
 	<< "var timestamp = \"" << timestamp << "\";var imgext = \".png\";"
	<< "var calibfile = \"" << calibfile << "\";var chans = "<< chans<<" ;this.o_selected = null;"
	<< "function select (id, b_deselect)"
	<< "{var obj = get_element(id);if (obj && !b_deselect) {var o_olditem = this.o_selected;this.o_selected = obj;"
	<< "if (o_olditem && (o_olditem != obj)) select(o_olditem.id, true);}"
	<< "obj.style.fontWeight = b_deselect ? 'normal' : 'bold';obj.style.color = b_deselect ? '000000' : 'ff0000';return false;}"
	<< "function genlinks() {"
	<< "document.write(\"<a id='ch0' href='javascript:void(0);' onClick='setimage(0);"
	<< "select(\\\"ch0\\\",false)'>All</a><br>\");"
	<< "for (i=1; i<=chans; i++) {"
	<< "ch=i+\"\";if (ch.length==1) ch=\"0\"+ch;"
	<< "imglink = calibfile + \"_ch\" + ch + \"_\" + timestamp + imgext;"
	<< "document.write(\"<a id='ch\"+i+\"' href='javascript:void(0);' onClick='setimage(\"+i+\");"
	<< "select(\\\"ch\"+i+\"\\\",false)'>Chan#\"+i+\"</a><br>\");}}"
	<< "function setimage(chan) {ch=chan+\"\";"
	<< "if (ch.length==1) ch=\"0\"+ch;"
	<< "if (chan==0) { imglink = calibfile + \"_all_\" + timestamp + imgext;} "
        << "else { imglink = calibfile + \"_ch\" + ch + \"_\" + timestamp + imgext;};"
	<< "document['plot'].src = imglink;}"
	<< "get_element = document.all ?function (s_id) { return document.all[s_id] } :	"
	<< "function (s_id) { return document.getElementById(s_id) };</script></head><body>"
	<< "<script language=\"JavaScript\">document.write(\"<h3>\"+calibfile+\" (\"+timestamp+\"): Current Calibration Plots</h3>\");</script>"
	<< "<table><tr><td valign='top'>	<script language=\"JavaScript\">genlinks();</script></td><td>"
	<< "<script language=\"JavaScript\">document.write(\"<img name='plot' src='' alt='plot'></img>\");"
	<< "setimage(0);</script></td></tr></table></body></html>" << endl;
  hout.clear();
  hout.close();
  return 0;
}

