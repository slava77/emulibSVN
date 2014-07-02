#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string>
#include <fstream>
#include <iostream>
#include "Tripdata_plotter.h"

///////////////////////////////////////////
#define MAX_LINE_LENGTH 500
#define MAX_DATA_ARRAY_SIZE 50000
///////////////////////////////////////////

#ifndef __CINT__
#include "TSystem.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TGaxis.h"
#include "TMultiGraph.h"
#endif

using namespace std;

Tripdata_plotter::Tripdata_plotter()
 : all_boards(true), all_channels(true) {

 	local_dir = gSystem->pwd();
	voltage = new Float_t[MAX_DATA_ARRAY_SIZE];
	current = new Float_t[MAX_DATA_ARRAY_SIZE];
}

Tripdata_plotter::Tripdata_plotter(int RDB)
 : all_boards(false), all_channels(true), RDB(RDB) {

 	local_dir = gSystem->pwd();
	voltage = new Float_t[MAX_DATA_ARRAY_SIZE];
	current = new Float_t[MAX_DATA_ARRAY_SIZE];
}

Tripdata_plotter::Tripdata_plotter(int RDB, int channel)
 : all_boards(false), all_channels(false), RDB(RDB), channel(channel) {

 	local_dir = gSystem->pwd();
	voltage = new Float_t[MAX_DATA_ARRAY_SIZE];
	current = new Float_t[MAX_DATA_ARRAY_SIZE];
}

  Tripdata_plotter::~Tripdata_plotter() {
 	delete[] voltage;
 	delete[] current;
}

char * Tripdata_plotter::std_path = "/home/hvuser/data/Remote_";
char * Tripdata_plotter::output_dir = "/home/hvuser/plots/";

/*
  Finds .dump files and matches them with corresponding calibration files.
  Calls read_files() and make_plots() to plot the calibrated voltage and current vs. time.
  The plots are saved in the output_dir as .png files
*/
void Tripdata_plotter::search_plot(char * dir) {

	string tmp_name, RDB_str;
 	int RDB_index, underscr_index;
 	const char *direntry;
 	void *dirp = gSystem->OpenDirectory(dir);

	while ((direntry = gSystem->GetDirEntry(dirp))) {

		if (direntry[0] == '.')
			continue;

		data_file = dir; data_file += '/'; data_file += direntry;

		//check if data_file is dir
		if (gSystem->cd(data_file.c_str())) {
			gSystem->cd(local_dir.c_str());
			//search recursively
			search_plot((char*)data_file.c_str());
		}
		else if (strstr(direntry,".dump")) {
			//determine the board #
			tmp_name = direntry;
			RDB_index = tmp_name.find("RDB#");
			if (RDB_index != -1) {
				//skip the "RDB#" characters and retrieve the actual RDB number
				RDB_index += 4;
				underscr_index = tmp_name.find_first_of("_", RDB_index);
				RDB_str = tmp_name.substr(RDB_index, underscr_index-RDB_index);

				//set the paths to the calibration files
				voltage_calibration_file = std_path + RDB_str + "/T10_M" + RDB_str + ".log";
				current_calibration_file = std_path + RDB_str + "/T09_M" + RDB_str + ".log";

				//handle the inputs from the constructor and make appropriate plots
				//if the files can be read, make plots
				if (all_boards && all_channels) {
					if(read_files())
						make_plots();
				}
				else if (!all_boards && all_channels && atoi(RDB_str.c_str()) == RDB) {
					if(read_files())
						make_plots();
				}
				else if (!all_boards && !all_channels && atoi(RDB_str.c_str()) == RDB && get_channel(tmp_name) == channel) {
					if(read_files())
						make_plots();
				}
			}
		}
 	}
}//search_plot

bool Tripdata_plotter::read_files() {

	//extract the channel number from the data_file
	ifstream dif;
	dif.open(data_file.c_str());
	if (!dif) {
		cerr << "The file: " + data_file + " could not be opened." << endl;
		return false;
	}
	int channel_in_file = -1;
	bool keep_looking = true;
	while (keep_looking && !dif.eof()) {
		while (dif.get() != 'C' && !dif.eof()) {}
		if (dif.get() == 'h') {
			dif >> dec >> channel_in_file;
			keep_looking = false;
		}
	}
	if (channel_in_file == -1) {
		cerr << "The channel number could not be read. Check file: " << data_file << endl;
		return false;
	}
	dif.close();

	//extract the calibration constants A and B, where V = A*ADC + B,
	//from the voltage calibration file
	ifstream vcif;
	vcif.open(voltage_calibration_file.c_str());
	if (!vcif) {
		cerr << "The file: " + voltage_calibration_file + " could not be opened." << endl;
		return false;
	}

	//find the row number of the last occurance of "A:"
	//assumption: this line is in every file, if not look_for_voltcal[] should be changed approprietly
	int i, line_nr = 1, last_occur = 0;
	char tmp_line_voltcal[MAX_LINE_LENGTH], look_for_voltcal[] = "A:";

 	while (!vcif.eof()) {
		vcif.getline(tmp_line_voltcal, MAX_LINE_LENGTH, '\n');
		if (strstr(tmp_line_voltcal, look_for_voltcal) != NULL) {
			last_occur = line_nr;
		}
		line_nr++;
	}
	vcif.clear();	vcif.seekg(0);

	//skip all lines until the line where look_for_voltcal last appeared
	for (i=1; i<=last_occur; i++) {
		vcif.ignore(MAX_LINE_LENGTH, '\n');
	}

	//skip channel-1 commas after "A:"
	int num_commas_breaks = 0;
	//assumption: maximum length of A is 32 bytes.
	char tmp_A[32];
	while (num_commas_breaks < channel_in_file-1) {
		if (vcif.peek() == ',' || vcif.peek() == '\n')
			num_commas_breaks++;
		vcif.ignore(1);
	}

	i = 0;
	while ((vcif.peek() >= '0' && vcif.peek() <= '9') || vcif.peek() == '.' || vcif.peek() == '-') {
		tmp_A[i] = vcif.get();
		i++;
	}
	A = atof(tmp_A);

	//assumption: no ':' until after 'B'
	while (vcif.get() != ':') {}
	//move forward to the first number of B
	vcif.ignore(MAX_LINE_LENGTH, '\n');

	//skip channel-1 commas after "B:"
	num_commas_breaks = 0;
	//assumption: maximum length of B is 32 bytes.
	char tmp_B[32];
	while (num_commas_breaks < channel_in_file-1) {
		if (vcif.peek() == ',' || vcif.peek() == '\n' )
			num_commas_breaks++;
		vcif.ignore(1);
	}

	i = 0;
	while ((vcif.peek() >= '0' && vcif.peek() <= '9') || vcif.peek() == '.' || vcif.peek() == '-') {
		tmp_B[i] = vcif.get();
		i++;
	}
	B = atof(tmp_B);

	vcif.close();

	//extract the corrent values from the current calibration file
	ifstream ccif;
	ccif.open(current_calibration_file.c_str());
	if (!ccif) {
		cerr << "The file: " + current_calibration_file + " could not be opened." << endl;
		return false;
	}
	line_nr = 1, last_occur = 0;
	char tmp_line_currcal[MAX_LINE_LENGTH],look_for_currcal[] = "Current Tab:";

 	while (!ccif.eof()) {
		ccif.getline(tmp_line_currcal, MAX_LINE_LENGTH, '\n');
		if (strstr(tmp_line_currcal, look_for_currcal) != NULL) {
			last_occur = line_nr;
		}
		line_nr++;
	}
	ccif.clear();	ccif.seekg(0);

	//skip all lines until the line where look_for_currcal last appeared
	for (i=1; i<=last_occur; i++) {
		ccif.ignore(MAX_LINE_LENGTH, '\n');
	}

	//skip channel-1 commas
	//assumption: maximum length of C is 32 bytes.
	char tmp_C[32];

	while ( ccif.peek() != EOF ) {
		num_commas_breaks = 0;
		while (num_commas_breaks < channel_in_file-1) {
			if (ccif.peek() == ',' || ccif.peek() == '\n')
				num_commas_breaks++;
			ccif.ignore(1);
		}

		i = 0;
 		while ((ccif.peek() >= '0' && ccif.peek() <= '9') || ccif.peek() == '.' || vcif.peek() == '-') {
 			tmp_C[i] = ccif.get();
 			i++;
 		}
 		curr_cal.push_back(atof(tmp_C));
		memset(tmp_C, 0, 31);
 		ccif.ignore(MAX_LINE_LENGTH, '\n');
	}

	ccif.close();
	return true;

}//read_files

void Tripdata_plotter::make_plots() {

	set_voltage_current();
	calibrate_voltage();
	calibrate_current();
	const Int_t n = num_of_meas;
	Float_t time[n];
	Float_t max_curr = 0;
	Float_t min_curr = 0;
	Float_t max_volt = 0;
	Float_t min_volt = 0;
	Float_t scale = 1;

	int pos_start = data_file.find("tripdata");
	int pos_end = data_file.find(".dump");
	string file_name = data_file.substr(pos_start, pos_end-pos_start);
	string output_file_name = output_dir; output_file_name += "/" + file_name + ".png";
	log_buf += file_name + '\n';

	TCanvas *c1 = new TCanvas("c1", file_name.c_str(), 800, 700);

// 	//assign values to the arrays and check which elements in the arrays are the largest/smallest
// 	//assumption: no negative values
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
		if(max_volt > max_curr) {
		 	scale = (max_curr-min_curr)/(max_volt-min_volt);
			for (int i=0; i<n; i++) {
				voltage[i] = voltage[i]*scale;
				voltage[i] += max_curr - max_volt*scale;
			}
		}
		else {
			scale = (max_volt-min_volt)/(max_curr-min_curr);
			for (int i=0; i<n; i++) {
				current[i] = current[i]*scale;
				current[i] += max_volt - max_curr*scale;
			}
		}
	}
	TGraph *gr1;
	TGraph *gr2;
	TMultiGraph *mg = new TMultiGraph();
	gr2 = new TGraph(n,time,voltage);
	gr2->SetLineColor(kRed);
	gr1 = new TGraph(n,time,current);
	mg->Add(gr1);
	mg->Add(gr2);
	mg->SetTitle("current (#muA) #color[2]{voltage (V)}");
	mg->Draw("LA");
	c1->Update();

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
			scale_max -= max_volt - max_curr*scale;
			scale_min -= max_volt - max_curr*scale;

			scale_max = scale_max/scale;
			scale_min = scale_min/scale;
		}
	}

	//make an axis for the voltage (or current...)
	TGaxis *axis = new TGaxis(gPad->GetUxmax(),gPad->GetUymin(), gPad->GetUxmax(),
	gPad->GetUymax(), scale_min, scale_max, 510, "+L");
	axis->SetLineColor(kRed);
	axis->Draw();
	c1->Update();

 	mg->GetXaxis()->SetTitle("Time (s)");
	c1->Update();
	mg->GetXaxis()->CenterTitle();
	c1->Update();

	c1 -> Print(output_file_name.c_str(), "png");

	//move the original tripdata files to the output dir
	string rename_file = output_dir; rename_file += file_name; rename_file += ".dump";
	rename(data_file.c_str(), rename_file.c_str());

	//clear the voltage and current arrays for next file
	memset(voltage, 0, num_of_meas);
	memset(current, 0, num_of_meas);

	delete axis;
	delete gr1;
	delete gr2;
	delete mg;
	delete c1;

}//make_plots

void Tripdata_plotter::set_voltage_current() {

	ifstream dif;
	dif.open(data_file.c_str());

	int i, line_nr = 1, last_occur = 0;
	char tmp_line_datafile[MAX_LINE_LENGTH], look_for_datafile[] = "#    Vmon Imon";

 	while (!dif.eof()) {
		dif.getline(tmp_line_datafile, MAX_LINE_LENGTH, '\n');
		if (strstr(tmp_line_datafile, look_for_datafile) != NULL) {
			last_occur = line_nr;
		}
		line_nr++;
	}
	dif.clear(); dif.seekg(0);

	num_of_meas = line_nr - last_occur - 2;

	//skip all lines until the line where look_for last appeared
	for (i=1; i<=last_occur; i++) {
		dif.ignore(MAX_LINE_LENGTH, '\n');
	}

	i = 0;
	int tmp_volt, tmp_curr;
	while (dif.peek() != EOF && i < num_of_meas) {
		//move to the next integer, '.' or '-'
		while (!(dif.peek() >= '0' && dif.peek() <= '9')) {
			dif.ignore(1);
		}
		dif >> tmp_line_datafile >> tmp_volt >> tmp_curr;
		dif.ignore(MAX_LINE_LENGTH, '\n');
		voltage[i] = tmp_volt;
		current[i] = tmp_curr;
		i++;
	}

} //set_voltage_current

//reads bitvalues of voltage, converts voltage in xV
void Tripdata_plotter::calibrate_voltage() {

	for (int i=0; i<num_of_meas; i++) {
		voltage[i] = voltage[i]*A + B;
	}
}//calibrate_voltage

//reads bitvalues of current, converts current in xA
void Tripdata_plotter::calibrate_current() {

	double tmp_curr, k;
	int i;
	unsigned int j;
	for (i=0; i<num_of_meas; i++) {
		tmp_curr = current[i];

		j=0;
		//assumption: the values in array curr_carr are ordered,
		//largest element first
		while (tmp_curr <= curr_cal[j] && j < curr_cal.size()) {
			j++;
		}
		if (j > 0) {
			//make a linear interpolation between the closest curr_val elements
			if (curr_cal[j-1] > tmp_curr && curr_cal[j] < tmp_curr) {
//				k = fabs((tmp_curr - curr_cal[j])/(curr_cal[j-1] - curr_cal[j]));
				k = fabs((curr_cal[j-1] - tmp_curr)/(curr_cal[j-1] - curr_cal[j]));
			}
			else {
				k = 0;
			}
		}
		else {
			k = 0;
		}
		//current in mikroamps
		current[i] = (j+k)*0.1;
	}
	curr_cal.clear();
}//calibrate_current

int Tripdata_plotter::get_channel(string tmp_name) {
	
	//retrieve the channel number from string tmp_name
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

void Tripdata_plotter::save_log() {

	//save log file in the output dir
	time_t rawtime;
	time (&rawtime);
	string time_date = ctime (&rawtime);

	log_file = output_dir; log_file += "analyze_trips.log";
	cout << "logfile: " << log_file << endl;
	ofstream log(log_file.c_str(), ios::app);
	log << "\n\n";
	log << time_date;
	log << "--------------------------------------------------\n";
	log << log_buf;
}//save_log
