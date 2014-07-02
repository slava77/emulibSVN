#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "Tripdata_handler.h"

#ifndef __CINT__
#include "TSystem.h"
#endif


using namespace std;

void Tripdata_handler::read_files(string df, string vcf, string ccf) {

	data_file = df;
	voltage_calibration_file = vcf;
	current_calibration_file = ccf;

	//extract the channel number
	ifstream dif;
	dif.open(data_file.c_str());
	if (!dif) {
		cerr << "The file: " + data_file + " could not be opened.";
	}

	channel = -1;
	bool keep_looking = true;
	while (keep_looking && !dif.eof()) {
		while (dif.get() != 'C' && !dif.eof()) {}
		if (dif.get() == 'h') {
			dif >> dec >> channel;
			keep_looking = false;
		}
	}
	if (channel == -1) {
		cerr << "The channel number could not be read. Check file: " << data_file << endl;
		exit(EXIT_FAILURE);
	}
	dif.close();

	//extract the calibration constants A and B, where V = A*ADC + B
	ifstream vcif;
	vcif.open(voltage_calibration_file.c_str());
	if (!vcif) {
		cerr << "The file: " + voltage_calibration_file + " could not be opened.";
	}

	//find the row number of the last occurance of "A:"
	//assumptions: this line is in every file, if not look_for_volt[] should be changed approprietly, no line is longer
	//than 200 characters
	const int MAX_LINE_LENGTH_VOLTCAL_FILE = 200;
	char tmp_line_voltcal[MAX_LINE_LENGTH_VOLTCAL_FILE];
	int line_nr = 1, last_occur = 0, i;
	char look_for_volt[] = "A:";

 	while (!vcif.eof()) {
		vcif.getline(tmp_line_voltcal, MAX_LINE_LENGTH_VOLTCAL_FILE, '\n');
		if (strcmp(look_for_volt, tmp_line_voltcal) == 0) {
			last_occur = line_nr;
		}
		line_nr++;
	}
	vcif.clear();
	vcif.seekg(0);

	//skip all lines until the line where look_for_volt last appeared
	for (i=1; i<=last_occur; i++) {
		vcif.ignore(MAX_LINE_LENGTH_VOLTCAL_FILE, '\n');
	}

	//skip channel-1 commas
	int num_commas_breaks = 0;
	//assumption: maximum length of A is 32 bytes.
	char tmp_A[32];
	while (num_commas_breaks < channel-1) {
		if (vcif.peek() == ',' || vcif.peek() == '\n')
			num_commas_breaks++;
		vcif.ignore(1, ',');
	}

	i = 0;
	while (vcif.peek() >= '0' && vcif.peek() <= '9' || vcif.peek() == '.' || vcif.peek() == '-') {
		tmp_A[i] = vcif.get();
		i++;
	}
	A = atof(tmp_A);

	//assumption: no ':' until after 'B'
	while (vcif.get() != ':') {}
	//move forward to the first number of B
	vcif.ignore(1);

	//skip channel-1 commas
	num_commas_breaks = 0;
	//assumption: maximum length of B is 32 bytes.
	char tmp_B[32];
	while (num_commas_breaks < channel-1) {
		if (vcif.peek() == ',' || vcif.peek() == '\n' )
			num_commas_breaks++;
		vcif.ignore(1, ',');
	}

	i = 0;
	while (vcif.peek() >= '0' && vcif.peek() <= '9' || vcif.peek() == '.' || vcif.peek() == '-') {
		tmp_B[i] = vcif.get();
		i++;
	}
	B = atof(tmp_B);

	vcif.close();

	//extract the corrent values from the current calibration file
	ifstream ccif;
	ccif.open(current_calibration_file.c_str());
	if (!ccif) {
		cerr << "The file: " + current_calibration_file + " could not be opened.";
	}

	//assumption: max_line_length is shorter than 300 bytes
	const int MAX_LINE_LENGTH_CURRCAL_FILE = 300;
	char tmp_line_currcal[MAX_LINE_LENGTH_CURRCAL_FILE];
	line_nr = 1, last_occur = 0;
	char look_for_curr[] = "Current Tab:";

 	while (!ccif.eof()) {
		ccif.getline(tmp_line_currcal, MAX_LINE_LENGTH_CURRCAL_FILE, '\n');
		if (strcmp(look_for_curr, tmp_line_currcal) == 0) {
			last_occur = line_nr;
		}
		line_nr++;
	}

	ccif.clear();
	ccif.seekg(0);

	//skip all lines until the line where look_for_curr last appeared
	for (i=1; i<=last_occur; i++) {
		ccif.ignore(MAX_LINE_LENGTH_CURRCAL_FILE, '\n');
	}

	//skip channel-1 commas
	//assumption: maximum length of C is 32 bytes.
	char tmp_C[32];

	while ( ccif.peek() != EOF ) {
		num_commas_breaks = 0;
		while (num_commas_breaks < channel-1) {
			if (ccif.peek() == ',' || ccif.peek() == '\n')
				num_commas_breaks++;
			ccif.ignore(1, ',');
		}

		i = 0;
 		while (ccif.peek() >= '0' && ccif.peek() <= '9' || ccif.peek() == '.' || vcif.peek() == '-') {
 			tmp_C[i] = ccif.get();
 			i++;

 		}
 		curr_cal.push_back(atof(tmp_C));
		memset(tmp_C, 0, 31);

 		ccif.ignore(MAX_LINE_LENGTH_CURRCAL_FILE, '\n');
	}
	ccif.close();
}//read_files

void Tripdata_handler::set_voltage_current() {

	ifstream dif;
	dif.open(data_file.c_str());

	int i, line_nr = 1, last_occur = 0;
	const int MAX_LINE_LENGTH_DATA_FILE = 50;
	char tmp_line[MAX_LINE_LENGTH_DATA_FILE], look_for[] = "#    Vmon Imon";

 	while (!dif.eof()) {
		dif.getline(tmp_line, MAX_LINE_LENGTH_DATA_FILE, '\n');
		if (strcmp(look_for, tmp_line) == 0) {
			last_occur = line_nr;
		}
		line_nr++;
	}
	dif.clear();
	dif.seekg(0);

	num_of_meas = line_nr - last_occur-2;
	voltage = new Float_t[num_of_meas];
	current = new Float_t[num_of_meas];

	//skip all lines until the line where look_for last appeared
	for (i=1; i<=last_occur; i++) {
		dif.ignore(MAX_LINE_LENGTH_DATA_FILE, '\n');
	}

	i = 0;
	int tmp_volt, tmp_curr;
	while (dif.peek() != EOF) {
		//move to the next integer, . or -
		while (!(dif.peek() >= '0' && dif.peek() <= '9')) {
			dif.ignore(1);
		}
		dif >> tmp_line >> tmp_volt >> tmp_curr;
		dif.ignore(MAX_LINE_LENGTH_DATA_FILE, '\n');
		voltage[i] = tmp_volt;
		current[i] = tmp_curr;
		i++;
	}

} //set_voltage_current

//reads bitvalues of voltage, converts and returns voltage in xV
Float_t *Tripdata_handler::get_voltage() {
	//convert voltage vector

	for (int i=0; i<num_of_meas; i++) {
		voltage[i] = voltage[i]*A + B;
	}

	return voltage;
}//get_voltage

//reads bitvalues of current, converts and returns current in xA
Float_t *Tripdata_handler::get_current() {

	double tmp_curr, k;
	int i;
	unsigned int j;
	for (i=0; i<num_of_meas; i++) {
		tmp_curr = current[i];

		j=0;
		while (tmp_curr <= curr_cal[j] && j < curr_cal.size()) {
			j++;
		}
		if (curr_cal[j+1] != curr_cal[j]) {
			k = fabs((tmp_curr-curr_cal[j])/(curr_cal[j+1]-curr_cal[j]));
		}
		else {
			k = 0;
		}
		//current in mikroamps
		current[i] = (j-k)*0.1;
		
		curr_cal.clear();
	}
	return current;
}//get_current

int Tripdata_handler::get_num_of_meas() {

	return num_of_meas;
}//get_num_of_meas

