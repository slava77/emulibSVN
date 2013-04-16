/*****************************************************************************
 * This is the manager program which is the top-level program for FAST site
 * chamber tests. (This simple C program will probably be replaced by a
 * nicer user interface later.)
 * First  version:  March  21, 2000
 * Second version:  August 02, 2001 I changed the numeration of tests
 * 4.4 version: March 20, 2003 There were a lot of chnages but nobody
 * keeps the count of version
 * The last changes include Test_27 modification for all CSC types S.D.
 *****************************************************************************/

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include "misc.h"
#include "daq_conf.h"
#include "daq_ipc.h"
#include "daq_module.h"
#include "test_utils.h"
#include "test_params.h"


#define MAX_TEST_NUM     29
#define MIN_CSC_NUM       1 
#define MAX_CSC_NUM     300

#define MAX_LINE_LENGTH  80

typedef struct 
  {
  int    num;
  int    type;
  char   name[12];
  } csc_id;

/* Global variables local to this file */
static char testhome[] = "/test_results";
static char filelist_name_element[] = "/test_info/filelist.txt";
static char last_data_file_element[] = "/test_info/last_data_file.txt";
/*
static char testhome[] = "/home/cscme42/test_results";
static char filelist_name_element[] = "/home/cscme42/test_info/filelist.txt";
static char last_data_file_element[] = "/home/cscme42/test_info/last_data_file.txt";
*/
char  number_of_run[1000];

int show_csc_status(csc_id *csc);
void set_up_test(int test_num, csc_id *csc);

int fast_ana(char *in_file, int max_events, int nskip, int test_num,
	     csc_id *csc);
//code crashes somehow here
int readfile(char *filename);
int getcha(char *c);
int getint(int *n);
int getstr(char *s);
int screen(char *mode);
void wait_for_return(void);
//void add_test_num(char *newfname, char *fname, int testnum);

char* datadir_translation;

int main(int argc, char ** argv) 
{
    int        i, j, k, istat, test_num = 20, size;
    logical    keep_looping = _TRUE;
    char       menu_bl[] = " ";
    char       dummy_name[12];
    char       *p;
    csc_id     csc;
    char       *temp_string, *steptest;
	char z;
	int disk, ring, num, res, max_events;
	char fname[1000];

/* fetch the datadir_translation: we don't go any farther without it. */
    temp_string = getenv("DATADIR"); 
    if (temp_string==0) 
    {
		printf("DATADIR could not be translated.\n");
		exit(1);
    }
    size = strlen(temp_string)+1;
    datadir_translation = malloc(size);
    if(datadir_translation==NULL)
    {
		printf("malloc() failed for datadir_translation.\n");
		exit(1);
    }
    memset(datadir_translation,0,size);
    strcpy(datadir_translation,temp_string);

    /* Initialize with default values */
    csc.num = 1; 
    csc.type = 5; 
    make_name(csc.name, csc.num, csc.type);


	if (argc > 1)
	{
		strcpy(fname, "/csc_data/");
		// data directory
		if (NULL == strstr(argv[1], fname)) // data dir is not included with filename
		{
			strcat(fname, argv[1]);
		}
		else
		{
			strcpy(fname, argv[1]);
		}
		

		// test number
		steptest = strstr(fname, "STEP_");
		res = sscanf(steptest, "STEP_%d", &test_num); // take test number from file name
		if (res != 1 || test_num < 11 || test_num > 28)
		{
			printf("File name does not contain valid STEP test number (11-28): %s\n", fname);
			exit(0);
		}

		// chamber name, type, number
		if (argc > 2)
		{
			// underscore in leading character means that's a pedestal run for event_display
			res = sscanf(argv[2], "%[pm_]%d.%d.%d", &z, &disk, &ring, &num);
			if 
			(
				res != 4 || 
				disk < 1 || disk > 4 ||
				ring < 1 || ring > 2
			)
			{
				printf("Unsupported chamber name: %s\n", argv[2]);
				exit(0);
			}
			else
			{
				if (num < 1  || num > ring * 18)
				{
					printf("Unsupported chamber number: %d\n", num);
					exit(0);
				}
			}

			csc.num = num;

			switch (disk * 10 + ring) // will give 11, 12, ... 42
			{
			case 11: csc.type = 2; break; // unclear a bit
			case 12: csc.type = 1; break;
			case 13: csc.type = 2; break;
			case 21: csc.type = 3; break;
			case 22: csc.type = 5; break;
			case 31: csc.type = 4; break;
			case 32: csc.type = 5; break;
			case 41: csc.type = 6; break;
			case 42: csc.type = 5; break;
			default: 
				printf("Unsupported chamber type: %s\n", argv[2]);
				exit(0);
			}

			strcpy(csc.name, argv[2]);
				
		}

		// event number to process
		if (argc > 3) max_events = atoi(argv[3]);
		else max_events = 0; // process all events

		strcpy(number_of_run, fname+10); // number_of_run propagates into plot titles

		fast_ana(fname, max_events, 0, test_num, &csc);
		return 0;
	}


    /* Infinite loop */
    i = 1;
    while (keep_looping) 
    {
		screen("clear");
		printf("\n\n");
		printf("%25s TEST MANAGER TOP MENU\n", menu_bl);
		printf("%25s -------------------------------------\n", menu_bl);
		printf("%15s Current test chamber         %s\n", menu_bl, csc.name);
		printf("\n");
		printf("%15s 0) Quit\n", menu_bl);
		printf("%15s 1) Select chamber number\n", menu_bl);
		printf("%15s 2) Show status of tests of this chamber\n", menu_bl);
		printf("%15s 3) Select a test\n", menu_bl);
		printf("\n");
		printf(" Enter [ %d ] : ", i);
		if (getint(&i) != 0) continue;
		screen("clear");
		printf("\n");
 
		/* Respond to request */
		if (i == 0) 
		{
			keep_looping = _FALSE;
		}

		else if (i == 1) 
		{
			/* select a csc type */
			do
			{
				j = csc.type;
				printf("%8s Possible chamber types are:\n", menu_bl);
				for (k = 1; k <= NUM_CSC_TYPES; k++) 
				{
					make_name(dummy_name, 0, k);
					if ((p = strstr(dummy_name, ".000"))) *p = '\0';
					else printf("couldn't find \".000\" in %s\n", dummy_name);
					printf("%12s %2d. %s\n", menu_bl, k, dummy_name);
				}
				printf("%8s Give the number of the chamber type [%d]:  ", menu_bl, j);
				if (getint(&j) != 0) continue;
				if (j < 1 || j > NUM_CSC_TYPES) 
				{
					printf("Number must be in the range 1-%d\n", NUM_CSC_TYPES); 
					continue;
				}
				csc.type = j;
	
				/* select a csc number */
				do
				{
					j = csc.num;
					printf("%8s Give the chamber number [%d]:  ", menu_bl, j);
					if (getint(&j) != 0) continue;
					if (j < MIN_CSC_NUM || j > MAX_CSC_NUM) 
					{
						printf("Chamber number must be in the range %d - %d\n", 
							   MIN_CSC_NUM, MAX_CSC_NUM);
						continue;
					}
					csc.num = j;
					make_name(csc.name, csc.num, csc.type);
					break;
				} while (1);
				break;
			} while(1);
		}
		else if (i == 2) 
		{
		  //printf("chose 2, status is shown %d\n",i);
			istat = show_csc_status(&csc);
			//printf("istat after show csc_status %d",istat);
		}
    
		else if (i == 3) 
		{
			/* select a test */
			do 
			{
				j = test_num;
				printf("%8s Available tests are:\n", menu_bl);
				printf("%8s      1.  Broken-wire test\n", menu_bl);
				printf("%8s      2.  HV Connectivity test\n", menu_bl);
				printf("%8s      3.  Gas Leak test\n", menu_bl);
				printf("%8s      4.  Long-term HV test\n", menu_bl);
				printf("%8s      5.  Gas gain test(radioactive source)\n", menu_bl);
				printf("%8s      6.  Assembly\n", menu_bl);
				printf("%8s      7.  Water leak test\n", menu_bl);
				printf("%8s      8.  Low Voltage test\n", menu_bl);
				printf("%8s      9.  HV-on current and cabling test\n", menu_bl);
				printf("%8s     10.  ALCT self-test\n", menu_bl);
				printf("%8s     11.  AFEB Noise test\n", menu_bl);
				printf("%8s     12.  AFEB Connectivity test\n", menu_bl);
				printf("%8s     13.  AFEB Threshold and analog noise test\n", menu_bl);
				printf("%8s     14.  ALCT Time-delay test\n", menu_bl);
				printf("%8s     15.  CFEB Noise test\n", menu_bl);
				printf("%8s     16.  CFEB Connectivity test\n", menu_bl);
				printf("%8s     17.  CFEB Calibration\n", menu_bl);
				printf("%8s     18.  Comparator noise test\n", menu_bl);
				printf("%8s     19.  Comparator Threshold test\n", menu_bl);
				printf("%8s     20.  Comparator Timing test\n", menu_bl);
				printf("%8s     21.  Left/Right Comparator Logic test\n", menu_bl);
				printf("%8s     22.  Left/Right Comparator Offset test\n", menu_bl);
				printf("%8s     23.  Neighbor Comparator test\n", menu_bl);
				printf("%8s     24.  Chamber gain map\n", menu_bl);
				printf("%8s     25.  ALCT self-trigger on cosmics test\n", menu_bl);
				printf("%8s     26.  CLCT self-trigger on cosmics tests\n", menu_bl);
				printf("%8s     27.  High statistics cosmic test\n", menu_bl);
				printf("%8s     28.  Performance vs HV test\n", menu_bl);
				printf("%8s     29.  High-rate test\n", menu_bl);
				printf("%8s Give the number of the test [%d]:  ", menu_bl, j);
				if (getint(&j) != 0) continue;
				if (j < 1 || j > MAX_TEST_NUM) 
				{
					printf("Test number must be in the range 1-%d\n", MAX_TEST_NUM); 
					continue;
				}
				test_num = j;
				set_up_test(test_num, &csc);
				break; /* we did a test, so exit */
			} while(1);
		}
    }
    free(datadir_translation);
    return 0;
}

/******************************************************************************
 *  This is the csc status unpacking and display routine
 ******************************************************************************/


#define STATUS_OK         0
#define STATUS_FAILED     1
#define STATUS_MISSING    2
#define STATUS_UNKNOWN    3

int show_csc_status(csc_id *csc) 
  {
  int            itest, length, status, test_status[MAX_TEST_NUM+1];
  char           *command, *filename, *result_dir, *filelist_name;
  char           line[81], myline[81], result[10];
  char           fname[20];
  static char    status_name[][16] = 
  {"OK", "Failed", "Results missing", "Unknown"};
  static int     active[MAX_TEST_NUM+1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
                                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  FILE           *fp;
  
  //Find the results directory, or create it 
  result_dir = malloc(strlen(datadir_translation) + strlen(testhome) 
		      + strlen(csc->name) + 1);
  //result_dir = malloc(strlen(testhome) + strlen(csc->name) + 1);
  if (result_dir == NULL)
    {
    printf("malloc() failed for result_dir\n");
    status = -1;
    wait_for_return();
    return -1;
    }
  sprintf(result_dir, "%s%s/%s", datadir_translation, testhome, csc->name);
  //sprintf(result_dir, "%s/%s", testhome, csc->name);
  printf("\nResults for this chamber are kept in %s\n", result_dir);

  //printf("status %i",access((const char *)result_dir, R_OK));
  
  status=access((const char *)result_dir, R_OK);
  //if ((status = access((const char *)result_dir, R_OK)) != 0)  
  if (status != 0)  
    {
      //perror("show_csc_status"); 
      //printf("File %s not found (access returned %d)\n", result_dir, status); 
      
      command = malloc(strlen(result_dir) + 10);
      if (command == NULL)
	{
	  free(result_dir);
	  printf("malloc() failed for command.\n");
	  status = -1;
	  wait_for_return();
	  return -1;
	}
      sprintf(command, "mkdir %s", result_dir);
      if ((status = system(command)) != 0) perror("show_csc_status"); 
      free(command);
    }
  else{printf("Directory already exists (access returned %d)\n", status); }
  
  //printf("before test number loop, %d\n",MAX_TEST_NUM);
  //First check for the .result files // 
   for (itest = 1; itest <= MAX_TEST_NUM; itest++) 
    {
   
      //printf("test number loop");
    test_status[itest] = STATUS_UNKNOWN;
    //printf("p0");
    
    filename = malloc(strlen(result_dir) + 20);
    if (filename == NULL)
      {
      free(result_dir);
      printf("malloc() failed for filename.\n");
      status = -1;
      wait_for_return();
      return -1;
      }
    sprintf(filename, "%s/test_%02d.results", result_dir, itest);
    
    //printf("filename max_test %s%d",filename,itest);
    if ((fp = fopen(filename, "r")) != NULL) 
      {
      while (1) 
	{
	if ((fgets(line, MAX_LINE_LENGTH, fp) == NULL)) break;
	length = strcspn(line, "#\n");
	
	// Comment lines have zero length 
	if (length > 0) 
	  {
	  strncpy(myline, line, length);
	  myline[length] = '\0';
	  
	  // Check for headers 
	  if (strstr(myline,"Test result = ") != NULL) 
	    {
	    sscanf((char *)myline, "Test result = %s", result);
	    if (strcasecmp(result, "OK") == 0) test_status[itest] = STATUS_OK;
	    else if (strcasecmp(result, "Failed") == 0)
	      test_status[itest] = STATUS_FAILED;
	    }
	  }
	}

      if (fclose(fp)) 
	{
	printf("Error: fclose returned status %d\n", status);
	status = -1;
	}
      free(filename);
      }
    
    }

  // Then read the list of required files for all tests 
  filelist_name = malloc(strlen(datadir_translation)  + strlen(filelist_name_element) + 1);
 //filelist_name = malloc(strlen(filelist_name_element) + 1);
  if (filelist_name == NULL)
    {
    free(result_dir);
    printf("malloc() failed on filelist_name.\n");
    status = -1;
    wait_for_return();
    return -1;
    }
  sprintf(filelist_name,"%s%s",datadir_translation,filelist_name_element);
  //sprintf(filelist_name,"%s",filelist_name_element);
  fp = NULL;
  if ((fp = fopen(filelist_name, "r")) != NULL) 
    {
    while (1) 
      {
      if ((fgets(line, MAX_LINE_LENGTH, fp) == NULL)) break;
      length = strcspn(line, "#\n");
      
      //Comment lines have zero length 
      if (length > 0) 
	{
	strncpy(myline, line, length);
	myline[length] = '\0';
	
	// Check for headers 
	if (strstr(myline,"Files for test ") != NULL) 
	  {
	  sscanf((char *)myline, "Files for test %d", &itest);
	  }
        
	// If it's not a header, the current line should be a filename 
	else 
	  {
	  printf("%s\n", myline); 
	  if (sscanf((char *)myline, "%s", fname) != 1) 
	    {
	    printf("Error reading result filename from %s", filename);
	    status = -1;
	    }
	  
	  //Look for that file 
	  filename = malloc(strlen(result_dir) + strlen(fname) + 2);
	  if (filename == NULL)
	    {
	    free(result_dir);
	    free(filelist_name);
	    printf("malloc() failed on filename.\n");
	    status = -1;
	    fclose(fp);
	    wait_for_return();
	    return -1;
	    }
	  sprintf(filename, "%s/%s", result_dir, fname);
	  printf("Looking for file %s... ", filename);
	  if ((status = access(filename, R_OK)) != 0) 
	    {
	    if (test_status[itest] == STATUS_OK) 
	      test_status[itest] = STATUS_MISSING;
	    printf("it's missing\n");
	    }
	  else printf("found it\n");
	  free(filename);
	  }
	}
      }
    printf("Can't open %s\n", filename);
    free(result_dir);
    return -1;
    }
  else printf("Can't find %s\n", filelist_name);

  if (fp && fclose(fp)) 
    {
    printf("fclose returned status %d\n", status);
    status = -1;
    }
  free(filelist_name);

// Show status 
  printf("\n");
  printf("----------- Status of FAST tests for CSC %s ---------\n", csc->name);
  for (itest = 0; itest <= MAX_TEST_NUM; itest++) 
    {
    if (active[itest]) printf("   %2d     %s\n", itest, 
			      status_name[test_status[itest]]);
    }
  printf("-------------------------------------------------------------\n");
  
  free(result_dir);
  wait_for_return();

  return 0;
  }

/******************************************************************************
 *  This is the test set up routine
 ******************************************************************************/

#define MAX_RESULT_FILES   25

void set_up_test(int test_num, csc_id *csc) 
{
	int         i, j, itest, length, nlines, status;
	int         max_events, nfiles, nskip;
	logical     keep_looping = _TRUE;
	char        menu_bl[] = " ";
	char        line[81], myline[81];
	char        in_file[1000];  /* test 17 */
	char        *result_dir, *filename, *newfilename, last_data_file[1000];
	char        *filelist_name, *command;
	char        reply;
	char        fname[MAX_RESULT_FILES][MAX_TEST_NUM+1];
	FILE        *fp;

	/* Infinite loop */
	i = 1;
	while (keep_looping) 
    {
		screen("clear");
		printf("\n\n");
		printf("%26s TEST ACTION MENU\n", menu_bl);
		printf("%26s ----------------\n", menu_bl);
		printf("%15s Current test chamber         %s\n", menu_bl, csc->name);
		printf("%15s Current test                 %d\n", menu_bl, test_num);
		printf("\n");
		printf("%15s 0) Back to Top Menu\n", menu_bl);
		printf("%15s 1) Show setup instructions\n", menu_bl);
		printf("%15s 2) Analyze results\n", menu_bl);
		printf("%15s 3) Save results\n", menu_bl);
		printf("\n");
		printf(" Enter [ %d ] : ", i);

		if (getint(&i) != 0) continue;
		screen("clear");
		printf("\n");
		/* Respond to request */
		if (i == 0) 
		{
			keep_looping = _FALSE;
		}

		else if (i == 1) 
		{
			/* Show instructions */ 
			filename = malloc(strlen(datadir_translation) + 30);
			if (filename == NULL)
			{
				printf("malloc() failed for filename.\n");
				wait_for_return();
				return;
			}
			sprintf(filename, "%s/test_info/test_%02d.txt", 
					datadir_translation,test_num);
			nlines = readfile(filename);
			if (nlines > 0) 
			{

				printf("\n");
				printf("*******************************************************\n");
				printf("Automatic display of test_info files has been disabled.\n");
				printf("Instructions for this test are in the file:\n");
				printf("       %s\n", filename);
				printf("*******************************************************\n");
				printf("\n");
				wait_for_return();

				//      if ((mypid = fork()) < 0)
				//      perror("Error forking process to display setup instructions");
				//
				//      /* This code is executed only by the child process */
				//      if (mypid == 0) {
				//        sprintf(title, "Instructions for Test %d", test_num); 
				//        sprintf(geom, "80x%d", nlines+3);
				//
				//        execl("/usr/bin/X11/nxterm", "nxterm", "-fn", "9x15", 
				//         "-T", title, "-bg", "lightblue", "-sb", 
				//         "-geometry", geom,
				//         "-e", "/home/fast/fast_daq/lisas_test_manager/show_info", filename, 
				//         (char *)0);
				//        printf("exec to display setup instructions failed\n");
				//        exit(7);
				//      }
			}
			else printf("*** No instruction file found for this test ***\n");
			free(filename);
		}

		else if (i == 2) 
		{
			/* Call the analysis routine */
			/* Default data file is the last one analyzed; get it from last_data_file */
			//sprintf(last_data_file,"%s",last_data_file_element);
			sprintf(last_data_file,"%s%s",datadir_translation,last_data_file_element);
			if ((fp = fopen(last_data_file, "r")) != NULL) 
			{
				while ((fgets(line, MAX_LINE_LENGTH, fp) != NULL)) 
				{
					length = strcspn(line, "#\n");

					if (length > 0)                /* comment lines have zero length */
					{
						strncpy(myline, line, length);
						myline[length] = '\0';
						sscanf((char *)myline, "%s", in_file);
					}
				}
	
				if (fclose(fp)) 
				{
					printf("fclose returned status %d\n", status);
					status = -1;
				}
			}

			//  Look for most recently accessed data file in $CSC_DIR 
			do
			{
				nskip = 0;
				max_events = 0;
				printf("Give name of file to analyze [%s] :  ", in_file);
				getstr(in_file);
				printf("in file, %s\n",in_file);
				printf("Give maximum number of events (0 = all) [%d]: ", max_events);
				getint(&max_events);
				printf("max_events, %d\n",max_events);
				//max_events=0;
				// Do the analysis and print the results 
				strcpy(number_of_run, in_file);
				status = fast_ana(in_file, max_events, nskip, test_num, csc);
				printf("status afterwards %i",status);
			} while(status == -1);

			/* Save the data file name */
			if ((fp = fopen(last_data_file, "w")) != NULL) 
			{
				fprintf(fp, "%s", in_file);
				if (fclose(fp) != 0) 
				{
					printf("Error closing %s\n", last_data_file);
					wait_for_return();
				}	
			}
		}

		else if (i == 3) 
		{
			/* Copy the results to the csc's directory, after creating it if necessary */
		  //result_dir = malloc(strlen(testhome)+ strlen(csc_name) + 1 );
			result_dir = malloc( strlen(datadir_translation) + strlen(testhome) + strlen(csc_name) + 1 );
			if(result_dir == NULL)
			{
				status = -1;
				printf("malloc() failed for result_dir\n");
				wait_for_return();
				return;
			}
			//sprintf(result_dir,"%s/%s", /*datadir_translation, */ testhome, csc_name);
			sprintf(result_dir,"%s%s/%s", datadir_translation, testhome, csc_name);
			if ((status = access((const char *)result_dir, R_OK)) != 0)  
			{
				command = malloc( strlen(result_dir) + 10);
				if(command == NULL)
				{
					status = -1;
					free(result_dir);
					printf("malloc() failed for command.\n");
					wait_for_return();
					return;
				}
				sprintf(command, "mkdir %s", result_dir);
				if ((status = system(command)) != 0) perror("show_csc_status");
				free(command);
			}

			/* Read the list of required files for all tests and copy each one from
			 * /temp to the CSC's result directory.
			 */
			printf("Looking for result files for test %d ...\n", test_num);
			nfiles = 0;
			//filelist_name = malloc(/*strlen(datadir_translation) +*/strlen(filelist_name_element)+1);
			filelist_name = malloc(strlen(datadir_translation) +strlen(filelist_name_element)+1);
			if (filelist_name == NULL)
			{
				free(result_dir);
				printf("malloc() failed for filelist_name.\n");
				wait_for_return();
				return;	
			}
			//sprintf(filelist_name,"%s",/*datadir_translation,*/filelist_name_element);
			sprintf(filelist_name,"%s%s",datadir_translation,filelist_name_element);
			if ((fp = fopen(filelist_name, "r")) != NULL) 
			{
				while (1) 
				{
					if ((fgets(line, MAX_LINE_LENGTH, fp) == NULL)) break;
					length = strcspn(line, "#\n");

					/* Comment lines have zero length */
					if (length > 0) 
					{
						strncpy(myline, line, length);
						myline[length] = '\0';

						/* Check for headers */
						if (strstr(myline,"Files for test ") != NULL) 
						{
							sscanf((char *)myline, "Files for test %d", &itest);
						}
        
						/* If it's not a header, the current line should be a filename */
						else if (itest == test_num) 
						{
							if (nfiles < MAX_RESULT_FILES) 
							{
								if (sscanf((char *)myline, "%s", fname[nfiles]) != 1) 
								{
									printf("Error reading filename from %s", filename);
									status = -1;
								}
								else 
								{
								  // Look for the file in the /temp directory 
								  //filename = malloc(/*strlen(datadir_translation) + */strlen(testhome) +strlen(fname[nfiles])+10);
								  filename = malloc(strlen(datadir_translation) + strlen(testhome) +strlen(fname[nfiles])+10);
									if (filename == NULL)
									{
										status = -1;
										free(result_dir);
										fclose(fp);
										free(filelist_name);
										printf("malloc() failed for filename.\n");
										wait_for_return();
										return;			    
									}
									//sprintf(filename, "%s/temp/%s",testhome, fname[nfiles]);
									sprintf(filename, "%s%s/temp/%s", 
											datadir_translation, testhome, fname[nfiles]);
									if ((status = access(filename, R_OK)) == 0) nfiles++;
									else 
										printf("  Couldn't find %s\n", filename);
									free(filename);
								}
							}
							else 
							{
								printf("Too many result files; skipping remaining files\n");
								break;
							}
						}
					}
				}
	
				if (fclose(fp)) 
				{
					printf("fclose returned status %d\n", status);
					status = -1;
				}
				free(filelist_name);

				/* Show the list of files and ask for confirmation of copy */
				if ((nfiles) > 0) 
				{
					printf("\nCopy the following files to %s?\n", result_dir);
					for (j = 0; j < nfiles; j++) printf("  %s\n", fname[j]);
					printf("  y/n [n] : ");
					reply = 'n';
					getcha(&reply);
					printf("\n");

					/* Copy the files */
					if (reply == 'y' || reply == 'Y') 
					{
						for (j = 0; j < nfiles; j++) 
						{
						  //filename = malloc(/*strlen(datadir_translation) + */strlen(testhome)+ strlen(fname[j]) + 10);
						  filename = malloc(strlen(datadir_translation) + strlen(testhome)+ strlen(fname[j]) + 10);
							if (filename == NULL)
							{
								status = -1;
								free(result_dir);
								printf("malloc() failed for filename.\n");
								wait_for_return();
								return;
							}
							newfilename = malloc(strlen(result_dir) + strlen(fname[j]) + 2);
							if (newfilename == NULL)
							{
								status = -1;
								free(result_dir);
								free(filename);
								printf("malloc() failed for newfilename.\n");
								wait_for_return();
								return;
							}
			     			       	//sprintf(filename, "%s/temp/%s", testhome, fname[j]);
							sprintf(filename, "%s%s/temp/%s", datadir_translation,testhome, fname[j]);
							sprintf(newfilename, "%s/%s",result_dir, fname[j]);

							command = malloc(strlen(filename) + strlen(newfilename) + 10);
							if (command == NULL)
							{
								status = -1;
								free(result_dir);
								free(filename);
								free(newfilename);
								printf("malloc() failed for command.\n");
								wait_for_return();
								return;
							}

							sprintf(command, "cp %s %s", filename, newfilename);
							if ((status = system(command)) != 0) perror("copying results"); 
							else printf("  copied %s\n", fname[j]);
							free(command);
							free(newfilename);
							free(filename);
						}
					}
					else printf("No files copied\n");
				}
				else printf("Found no files to copy\n");
			}
			else 
			{
				printf("Can't find %s\n", filelist_name);
				free(filelist_name);
			}
			free(result_dir);
			wait_for_return();
		}
    }
}

/******************************************************************************
 *  This is a routine to read the test info files; for now, it just
 *  gets the number of lines.
 ******************************************************************************/

int readfile(char *filename) 
  {
  int     nlines = 0, max_chars = 80;
  char    line[81];
  FILE    *fp;

  if ((fp = fopen(filename, "r"))) 
    {
    while (fgets(line, max_chars, fp)) 
      {
      nlines++;
      }
    if (fclose(fp)) perror("readfile");
    return nlines;
    }
  else return 0;
  }

/******************************************************************************
 *  This is a routine to add the test number to the result file name.
 ******************************************************************************/
/*
void add_test_num(char *newfname, char *fname, int testnum) 
{
int     k1, k2;
char    *s;

strcpy(newfname, fname);
if ((s = strstr(fname,"test")) != NULL) 
{
 k2 = strlen(fname);
 k1 = (int)(s - fname) + 4;
 sprintf(newfname + k1, "_%02d", testnum);
 strcpy(newfname + k1 + 3, s + 4);
}
 return;
}
*/














