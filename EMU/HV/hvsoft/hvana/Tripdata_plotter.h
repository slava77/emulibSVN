#include <string>
#include <vector>

#ifndef __CINT__
#include "TSystem.h"
#endif

using namespace std;

class Tripdata_plotter {

public:
	Tripdata_plotter();
	Tripdata_plotter(int RDB);
	Tripdata_plotter(int RDB, int channel);
	~Tripdata_plotter();
	void search_plot(char * dir);
	void save_log();

private:
	bool read_files();
 	void make_plots();
	void set_voltage_current();
	void calibrate_voltage();
	void calibrate_current();
	int get_channel(string data_file);

	bool all_boards, all_channels;
	int RDB, channel, num_of_meas;
	double A,B;
	static char *std_path, *std_mb_path, *output_dir;
	string local_dir, log_file, log_buf;
	string data_file, voltage_calibration_file, current_calibration_file;
	Float_t *voltage;
	Float_t *current;
	vector<double> curr_cal;	
};
