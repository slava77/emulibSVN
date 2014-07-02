void analyze_trips(int argc, char *argv[]);

#include <string>
#include <iostream>
#include <stdlib.h>
#include "Tripdata_plotter.h"
#define START_DIR "/opt/cschv/logs/"

#ifndef __CINT__

using namespace std;

int main(int argc, char *argv[]) {

	analyze_trips(argc ,argv);
	return 0;
}

#endif

using namespace std;

/* 
	Creates a Tripdata_plotter and envokes its methods...
*/
void analyze_trips(int argc, char *argv[]) {

	//Creates a Tripdata_plotter, scans recursievly  
	if (argc == 1) {
		Tripdata_plotter tdp;
		tdp.search_plot(START_DIR);
		tdp.save_log();
	}
	else if (argc == 2) {
		Tripdata_plotter tdp(atoi(argv[1]));
		tdp.search_plot(START_DIR);
		tdp.save_log();
	}
	else if (argc == 3) {
		Tripdata_plotter tdp(atoi(argv[1]), atoi(argv[2]));
		tdp.search_plot(START_DIR);
		tdp.save_log();
	}
	else {
		cout << "Enter 0, 1 or 2 input arguments" << endl;
		exit(0);
	}
}
