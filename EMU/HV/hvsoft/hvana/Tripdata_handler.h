#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#ifndef __CINT__
#include "TSystem.h"
#endif

using namespace std;

class Tripdata_handler {

public:
	void read_files(string df, string vcf, string ccf);
	void set_voltage_current();
	Float_t *get_voltage();
	Float_t *get_current();
	int get_channel;
	int get_num_of_meas();

private:
	string data_file, voltage_calibration_file, current_calibration_file;
	Float_t *voltage;
	Float_t *current;
	vector<double> curr_cal;
	int channel, num_of_meas;
	double A,B;
};


