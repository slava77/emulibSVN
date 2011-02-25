#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <signal.h>
#include <string.h>
// 11/08/2004
// this is how to implement timeout on wait
// this is how to implement timeout on connect

int g_v=0;

#define SIZE 1024
char buf[SIZE];

// ftp port 21
// ftp port 23
#define TIME_PORT 1025
int pid;
int pid_real;

  void (*was)(int), catch_function(int a);
  void (*was)(int), catch_function_empty(int a);
int dns_restart();
int proc_delete(char *process_template);

int main(int argc, 
         char *argv[])
{

  proc_delete(argv[1]);

  exit(0);
}

// ==================
int proc_delete(char *process_template){

  FILE *fp;
  char line[200];   
  char tmp1[20], tmp2[20];
  int pid_dns;
  int n_items;

  char command_ps[200];

  char ret[200];
  char *ret1;
   char tmp100[100];

   char ret2[100];
   char *ret22=ret2;

  ret1=&ret;

  strcpy(command_ps,"ps -ax | grep ");
   strcat(command_ps,process_template);   
  strcat(command_ps," > temp.txt ");

    system(command_ps);

     printf("%s \n",command_ps);

   fp=fopen("temp.txt","rt");

   while(1){

   ret1= (char *)fgets((char *)line, 200, fp);
   if(ret1==NULL)return;
   printf("1:%s\n",line);
   if(strstr(line,"delete_processes_by_name"))continue;
   if(strstr(line,"ps -ax"))continue;
   printf("2:%s\n",ret22);

   n_items=sscanf(line,"%s %s", tmp1,tmp2);

   if(n_items == 2){
   pid_dns=atoi(tmp1);
   printf("pid_dns = %d\n",pid_dns);

   //   kill(pid_dns,SIGTERM);

   sprintf(tmp100,"kill -9 %d",pid_dns);
   system(tmp100);
   }

   } // while(1);

}






