
/* file_editing

1) compilation:
g++ -o file_editing file_editing.cxx

2) description:
the program examines file_destin ( first parameter (s) -- may be as many as needed) as following:
a) look for contents of file_delete : the parameter after file_destin(s)
   if the content of file_delete is found it is removed from all file_destine 
   ... and replaced by content of file_replace (last parameter)
b) the new corrected file is 
                        file_destine(s)_name1
3)
the usage: 
example ( just one file_destin)
 
  ./file_editing file_destin.dat file_delete.dat file_replace.dat  

 will create new corrected file:
             file_destin.dat1 with removed content of file_delete.dat
  (if content of file_delete.dat is located in different parts of
    file  file_destin.dat, the all pieces will be removed)
 
*/
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
//#include "daq_module.h" 
//#include "misc.h" 
//#include "daq_conf.h"
//#include "csc_event.h"        /* upevt       */
//#include "cfortran.h"
//#include "hbook.h"
//#include "test_params.h"
//#include "test_functions.h"

//================================
//================================
#include <syslog.h>
#include <signal.h>
//#include "daq_conf.h"
//#include "daq_ipc.h"
//#include "daq_histo.h"
//#include "daq_module.h"
//#include "pedestal.h"
//#include "event_ana.h"
//================================

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/wait.h>
#include <string>
#include <iostream>



#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <vector>

using std::string;
using namespace std;


//========================================================

int main(int argc, char **argv){

  char tmp[200];
  char tmp1[200];
  struct stat stat_buf;
  vector<string> files;



  if(argc < 2) {

    printf("there should be at least 1 parameter: \n ");
      exit(0);
  }


 for(int i=0;i<argc-1;i++){
   //  printf("11\n");
  stat(argv[1+i], &stat_buf);

  //  printf("1\n");
  if(stat_buf.st_mode & S_IFDIR){/*printf("S_ISDIR(stat_buf.st_mode  %s\n",argv[1+i]);*/continue;}
     if(string(argv[1+i]).find("/",0) == string::npos)continue;
     if(string(argv[1+i]).find("*",0) != string::npos)continue;
        if(string(argv[1+i]).find("_file_removed222",0) != string::npos)continue;

	files.push_back(string(argv[1+i]));
	//	printf("argv[1+i] ==== %s\n",argv[1+i]);

 }


 /// exit(0);

 for(int i=0;i<files.size();i++){

    sprintf(tmp1,"mv %s %s_file_removed222",files[i].c_str(),files[i].c_str());
    system(tmp1);
    /////////////sprintf(tmp,"%s_file_removed222",files[i].c_str());


    sprintf(tmp1,"unix2dos -n %s_file_removed222 %s",files[i].c_str(),files[i].c_str());
    system(tmp1);
    sprintf(tmp1,"rm %s_file_removed222",files[i].c_str());
    system(tmp1);   

   printf("command ******************=%s\n",tmp1);//argv[1+i]);
 }
  return 1;
}

//========================================================








