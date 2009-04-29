
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

#define BUF_SIZE 32000        /* Rough estimate of max event size */
#define NWPAW    1000000
#define HISTO_SIZE      NWPAW*sizeof(float)

typedef struct{
        char h[HISTO_SIZE];
}PAWC;

typedef struct{
  int    num;
  int    type;
  char   name[12];
} csc_id;

PAWC pawc_;           /* A global variable */

//static unsigned short buf[BUF_SIZE];




//========================================================

// 12/07/2002
class File_attr{

protected:

  char my_CARRIAGE_RETURN[100];
  static const int MAXLINE = 1000;
  static const int MAXFILES = 1000;

  File_attr(){
    strcpy(my_CARRIAGE_RETURN,"my_CARRIAGE_RETURN");
  }

};

class File_oper;
//=======================================================

class File_oper : public File_attr{

public: 
  char *buffer;
  //  char *buffer_myCR;
  char *buffer_corr;
  bool line_mode;

  FILE *fp;
  string *file_name;
  int lines_number;



  int  readout();
  int alignment();
public:
  File_oper(char *name, bool line_mode=false, int mode=0);
};

//=======================================================
class File_edit : public  File_attr{

public:

  int number_destins;
  
  File_oper *files_destin[MAXFILES];
  File_oper *file_delete;
  File_oper *file_replace;

  File_oper *files_correct[MAXFILES];

  File_edit(){ // constructor
   number_destins=0;
  }

  int replacement();
  int destin_addition(char *name, bool line_mode=false);
  int delete_addition(char *name, bool line_mode=false);
  int replace_addition(char *name, bool line_mode=false);

};

//=======================================================

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
//=======================================================
int File_edit::destin_addition(char *name, bool line_mode){
    files_destin[number_destins] = new File_oper(name,line_mode);

    char fn[1000];
    strcpy(fn,name);

    string fn_s=string(fn);
    fn_s=fn_s.substr(0,fn_s.length()-16);

    //    strcat(fn,"1");
    files_correct[number_destins] = new File_oper((char *)fn_s.c_str(),line_mode,1);

    files_destin[number_destins]->readout();
    number_destins++;
    return 1;
}
int File_edit::delete_addition(char *name, bool line_mode){
    file_delete = new File_oper(name,line_mode);
    file_delete->readout();
    return 1;
}  
int File_edit::replace_addition(char *name, bool line_mode){
    file_replace = new File_oper(name,line_mode);
    printf("=======\n");
    file_replace->readout();
    return 1;
}

//=======================================================
int File_edit::replacement(){


  for(int i=0;i<number_destins;i++){
    int pointer_destin=0;
    int pointer_current=0;


     char *point;
     int stay_length;    

     while((point=strstr(files_destin[i]->buffer+pointer_current,file_delete->buffer))!=NULL){

       stay_length=(point-(files_destin[i]->buffer+pointer_current));
//---------------------------------------------------------------
       memcpy(files_destin[i]->buffer_corr + pointer_destin,
              files_destin[i]->buffer + pointer_current,
              stay_length);

       pointer_destin += stay_length;
//---------------------------------------------------------------
       memcpy(files_destin[i]->buffer_corr + pointer_destin,
              file_replace->buffer,
              strlen(file_replace->buffer));

       pointer_destin += strlen(file_replace->buffer);
       printf("replace_buffer_length=%d\n",strlen(file_replace->buffer));
//---------------------------------------------------------------
       pointer_current += (stay_length + strlen(file_delete->buffer));

     }
     

       stay_length=strlen(files_destin[i]->buffer+pointer_current);
       printf("*************************stay_length=%d %d %d %s\n",stay_length,pointer_current,strlen(files_destin[i]->buffer),
           files_destin[i]->buffer );
        memcpy(files_destin[i]->buffer_corr + pointer_destin,
              files_destin[i]->buffer + pointer_current,
              stay_length);  

        
	*(files_destin[i]->buffer_corr+pointer_destin+stay_length)='\0';

     printf("buffer_corr=%s %s\n",files_destin[i]->buffer_corr,files_correct[i]->file_name->c_str());

     
  fwrite((char *)files_destin[i]->buffer_corr,1,
         strlen(files_destin[i]->buffer_corr),files_correct[i]->fp); 
     
   fclose(files_correct[i]->fp);
  }

  

  return 1;

}






















//========================================================
File_oper::File_oper(char *name, bool line_mode, int mode){

  this->line_mode=line_mode;

  file_name = new string(name);
  lines_number=0;

  if(!mode){
   if((fp = fopen(file_name->c_str(), "r")) == NULL){ 
    cout << "file " << file_name->c_str() << " can't be open"<< endl;
    exit(0);
   }
   printf("file_name->c_str()=%s\n",file_name->c_str());
  }
  else{
   if((fp = fopen(file_name->c_str(), "w+")) == NULL){
        
    cout << "file " << file_name->c_str() << " can't be open"<< endl;
    exit(0);
   }
   printf("file_name->c_str()=%s\n",file_name->c_str());
  }

}
/*
//========================================================
File_oper::File_oper(char *name, int mode){

  file_name = new string(name);
  lines_number=0;

  if((fp = fopen(file_name->c_str(), "w+")) == NULL){
     
    cout << "file " << file_name->c_str() << " can't be open"<< endl;
    exit(0);
  }

}
*/
//========================================================
int  File_oper::readout(){
  struct stat buf;

  int ret=stat(file_name->c_str(),&buf); 
  off_t size = buf.st_size+2; // +2 for alignment
  buffer=new char[size]; // +2 for alignment
  buffer_corr=new char[size+size]; // to make file bigger: we do not know it's final size

  char linebuf[MAXLINE];

  long current_pointer=0;

printf("size=%d\n",size);
  while(fgets(linebuf,MAXLINE,fp)){

    printf("size=%d\n",size);
    memcpy(buffer+current_pointer,linebuf,strlen(linebuf)); //
    current_pointer += strlen(linebuf);
  }  
printf("size=%d\n",size);

//  if(line_mode)alignment();

  return ret;

}
//========================================================
int  File_oper::alignment(){

printf("strlen(buffer=%d\n",strlen(buffer));

 if(strlen(buffer)==0)return 1;
  
  *(buffer+strlen(buffer))='\n';
  *(buffer+strlen(buffer))='\0';
  while(*(buffer+strlen(buffer)-2) == '\n' && strlen(buffer) > 1){
    printf("================%s %d \n",buffer,strlen(buffer));

    if(strlen(buffer)>0)*(buffer+strlen(buffer)-1)='\0';
  }

 

printf("strlen(buffer=%d\n",strlen(buffer));
  return 1;

}
//========================================================

int main(int argc, char **argv){

  if(argc < 4) {

    printf("there should be at least three parameters: \n desination file(s), delete_file, replace_file \n ");
      exit(0);
  }


  if(!strcmp(argv[argc-1],"0")){ // case of we do not need to place anything on place of deleted string 
   if((fopen("0", "w+")) == NULL){     
    cout << "file " << "0" << " can't be open"<< endl;
    exit(0);
   }
  }


  File_edit *file_edit; 
  file_edit = new File_edit();

  file_edit->replace_addition(argv[argc-1],true);
  printf("%s\n",file_edit->file_replace->buffer);

  file_edit->delete_addition(argv[argc-2],true);
  printf("%s\n",file_edit->file_delete->buffer);

  char tmp[200];
  char tmp1[200];
  struct stat stat_buf;
  vector<string> files;

 for(int i=0;i<argc-3;i++){
   //  printf("11\n");
  stat(argv[1+i], &stat_buf);

  //  printf("1\n");
     if(stat_buf.st_mode & S_IFDIR){printf("S_ISDIR(stat_buf.st_mode  %s\n",argv[1+i]);continue;}
     if(string(argv[1+i]).find("/",0) == string::npos)continue;
     if(string(argv[1+i]).find("*",0) != string::npos)continue;
        if(string(argv[1+i]).find("_file_removed111",0) != string::npos)continue;

	files.push_back(string(argv[1+i]));
	printf("argv[1+i] ==== %s\n",argv[1+i]);

 }


 /// exit(0);

 for(int i=0;i<files.size();i++){

    sprintf(tmp1,"mv %s %s_file_removed111",files[i].c_str(),files[i].c_str());
    system(tmp1);
    sprintf(tmp,"%s_file_removed111",files[i].c_str());

    printf("command ******************=%s\n",tmp);//argv[1+i]);

    file_edit->destin_addition(tmp/*argv[1+i]*/,true);
    // printf("------------->%s\n",file_edit->files_destin[i]->buffer);
    //  printf("-------------------------------->%s\n",file_edit->files_correct[i]->buffer);

     printf("------------->%s\n",file_edit->files_destin[i]->file_name->c_str());
     printf("-------------------------------->%s\n",file_edit->files_correct[i]->file_name->c_str());
 }


  /*
  char tmp[200];
 char tmp1[200];
  FILE *fp;
  int file_count=0;
  string tmp_s;

  for(int i=0;i<argc-3;i++){
    sprintf(tmp,"ls -1 %s  > filelist_temp",argv[1+i]);

    printf("command ******************=%s\n",argv[1+i]);
    system(tmp);
    tmp_s=string(tmp);
     if(tmp_s.find("/",0) == string::npos)continue;
     //g++ -o file_editing_samplefilelist file_editing_samplefilelist.cxx

  fp = fopen("old_ver_temp", "r");


  if(fp){
   while(fgets(tmp,200,fp)){
    sprintf(tmp1,"mv %s %s1",tmp,tmp);
    system(tmp1);

    file_edit->destin_addition(tmp1,true);
    printf("%s\n",file_edit->files_destin[file_count]->buffer);
    printf("---------->%s\n",file_edit->files_correct[file_count]->buffer);
    file_count++;

     //  if(!count)break;

//      DebugN("rm -v "+tmp1+"");
   } // while   
  } // if fp



  } // for
  */



   file_edit->replacement();

  return 1;
}

//========================================================








