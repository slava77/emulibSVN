#include <string>
#include "Tripdata_handler.h"

using namespace std;

class File_finder {

public:
	File_finder();
	File_finder(int RDB);
	File_finder(int RDB, int channel);
	void search_plot(char * dir);
	void save_log();

private:
 	void make_plots(string data_file, string volt_calib_file, string curr_calib_file);
	int get_channel(string data_file);

	bool all_boards, all_channels;
	int RDB, channel;
	string local_dir, log_file, log_buf;
	static char *std_path, *output_dir;
};
