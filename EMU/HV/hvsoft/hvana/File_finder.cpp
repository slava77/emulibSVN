#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <fstream>
#include <iostream>
#include "File_finder.h"

#ifndef __CINT__
#include "TSystem.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGaxis.h"
#include "TMultiGraph.h"
#endif

using namespace std;

File_finder::File_finder()
 : all_boards(true), all_channels(true) {local_dir = gSystem->pwd();}

File_finder::File_finder(int RDB)
 : all_boards(false), all_channels(true), RDB(RDB) {local_dir = gSystem->pwd();}

File_finder::File_finder(int RDB, int channel)
 : all_boards(false), all_channels(false), RDB(RDB), channel(channel) {local_dir = gSystem->pwd();}

//Find .dump files and match them with corresponding calibration files
//Plot the calibrated voltage and current vs. time

char * File_finder::std_path = "/home/hvuser/data/Remote_";
char * File_finder::output_dir = "/home/hvuser/plots/";

void File_finder::search_plot(char * dir) {

 	string tmp_name, RDB_str, data_file, curr_calib_file, volt_calib_file;
 	int RDB_index, underscr_index;
 	const char *direntry;
 	void *dirp = gSystem->OpenDirectory(dir);

	while ((direntry = gSystem->GetDirEntry(dirp))) {

		if (direntry[0] == '.')
			continue;

		data_file = dir; data_file += '/'; data_file += direntry;

		if (gSystem->cd(data_file.c_str())) {
			gSystem->cd(local_dir.c_str());
			search_plot((char*)data_file.c_str());
		}
		else if (strstr(direntry,".dump")) {
			//determine the board #
			tmp_name = direntry;
			RDB_index = tmp_name.find("RDB#");
			if (RDB_index != -1) {
				RDB_index += 4;
				underscr_index = tmp_name.find_first_of("_", RDB_index);
				RDB_str = tmp_name.substr(RDB_index, underscr_index-RDB_index);

				//set the paths to the calibration files
				volt_calib_file = std_path + RDB_str + "/T10_M" + RDB_str + ".log";
				curr_calib_file = std_path + RDB_str + "/T09_M" + RDB_str + ".log";

				//handle the inputs from the constructor and make appropriate plots
				if (all_boards && all_channels) {
cout << volt_calib_file << endl;
					cout << curr_calib_file << endl;
					make_plots(data_file, volt_calib_file, curr_calib_file);

				}
				else if (!all_boards && all_channels && atoi(RDB_str.c_str()) == RDB) {
					make_plots(data_file, volt_calib_file, curr_calib_file);
				}
				else if (!all_boards && !all_channels && atoi(RDB_str.c_str()) == RDB && 								 	get_channel(tmp_name) == channel) {
					make_plots(data_file, volt_calib_file, curr_calib_file);
				}
			}
		}
 	}
}//search_plot

void File_finder::make_plots(string data_file, string volt_calib_file, string curr_calib_file) {

	Tripdata_handler tdh;
	cout << volt_calib_file << endl;
	cout << curr_calib_file << endl;
 	tdh.read_files(data_file, volt_calib_file, curr_calib_file);
	cout << volt_calib_file << endl;
	cout << curr_calib_file << endl;
	tdh.set_voltage_current();
	cout << volt_calib_file << endl;
	cout << curr_calib_file << endl;
	const Int_t n = tdh.get_num_of_meas();
	Float_t *voltage = tdh.get_voltage();
	Float_t *current = tdh.get_current();
	Float_t time[n];
	Float_t max_curr = 0;
	Float_t min_curr = 0;
	Float_t max_volt = 0;
	Float_t min_volt = 0;
	Float_t scale = 1;

	int pos_start = data_file.find("tripdata");
	int pos_end = data_file.find(".dump");
	string file_name = data_file.substr(pos_start, pos_end-pos_start);
	string output_file_name = output_dir; output_file_name += "/" + file_name + ".gif";
	log_buf += file_name + '\n';

	TCanvas *c1 = new TCanvas("c1", file_name.c_str(), 700, 700);

	//assign values to the arrays and check which elements in the arrays are the largest/smallest
	//assumption: no negative values
	for (int i=0; i<n; i++) {

		//time in s, 20ms between each measurement
		time[i] = (i+1)*20*0.001;

		if(max_curr < current[i]) {
			max_curr = current[i];
		}
		if(max_volt < voltage[i]) {
			max_volt = voltage[i];
		}
	}
	min_volt = max_volt; min_curr = max_curr;
	for (int i=0; i<n; i++) {
		if(min_curr > current[i]) {
			min_curr = current[i];
		}
		if(min_volt > voltage[i]) {
			min_volt = voltage[i];
		}
	}

	//the actual scaling
	if(max_curr != min_curr && max_volt != min_volt) {
 		scale = (max_curr-min_curr)/(max_volt-min_volt);

		if(max_volt > max_curr) {
			for (int i=0; i<n; i++) {
				voltage[i] = voltage[i]*scale;
				voltage[i] += max_curr - max_volt*scale;
			}
		}
		else {
			for (int i=0; i<n; i++) {
				current[i] = current[i]*scale;
				current[i] += max_volt - max_curr*scale;
			}
		}
	}
	TGraph *gr1;
	TGraph *gr2;

	TMultiGraph *mg = new TMultiGraph();
	if(max_volt > max_curr) {
		gr2 = new TGraph(n,time,voltage);
		gr2->SetLineColor(kRed);
		gr1 = new TGraph(n,time,current);

		mg->Add(gr1);
		mg->Add(gr2);
		mg->SetTitle("current (#muA) #color[2]{voltage (V)}");
		mg->Draw("LA");
		c1->Update();
	}
	else {
		gr2 = new TGraph(n,time,voltage);
		gr1 = new TGraph(n,time,current);
		gr2->SetLineColor(kRed);

		mg->Add(gr2);
		mg->Add(gr1);
		mg->SetTitle("current (#muA) #color[2]{voltage (V)}");
		mg->Draw("LA");
		c1->Update();
	}

	//scale the right axis
	Float_t scale_max = gPad->GetUymax();
	Float_t scale_min = gPad->GetUymin();
	if (scale != 0) {
		if(max_volt > max_curr) {

			scale_max -= max_curr - max_volt*scale;
			scale_min -= max_curr - max_volt*scale;

			scale_max = scale_max/scale;
			scale_min = scale_min/scale;

		}
		else {
			scale_max = scale_max/scale;
			scale_min = scale_min/scale;
		}
	}

	//make an axis for the voltage (or current...)
	TGaxis *axis = new TGaxis(gPad->GetUxmax(),gPad->GetUymin(), gPad->GetUxmax(),
	gPad->GetUymax(), scale_min, scale_max, 510, "+L");
	axis->SetLineColor(kRed);
	axis->Draw();
 	mg->GetXaxis()->SetTitle("Time (s)");
	mg->GetXaxis()->CenterTitle();

	c1 -> SaveAs(output_file_name.c_str());
	string move_file = "mv " + data_file + " " + output_dir + "/";
	system(move_file.c_str());

	delete voltage;
	delete current;

	delete axis;
	delete gr1;
	delete gr2;
	delete mg;
	delete c1;

}//make_plots

void File_finder::save_log() {

	time_t rawtime;
	time (&rawtime);
	string time_date = ctime (&rawtime);

	log_file = output_dir; log_file += "/analyze_trips.log";
	//just for debug
	cout << log_file << endl;
	ofstream log(log_file.c_str(), ios::app);
	log << "\n\n";
	log << time_date;
	log << "--------------------------------------------------\n";
	log << log_buf;
}//save_log

int File_finder::get_channel(string tmp_name) {

	int underscr_index;
	int channel_index = tmp_name.find("_ch");
	if (channel_index != -1) {
		channel_index += 3;
		underscr_index = tmp_name.find_first_of("_", channel_index);
		return atoi(tmp_name.substr(channel_index, underscr_index-channel_index).c_str());
	}
	else {
		return -1;
	}
}//get_channel


