//	PROGRAM: 	EvtDump	v 1
//	Authors:	V. Barashko, A. Korytov, July 13, 2004
//=============================================================================================
// 	This program dumps a requested DDU event: 4x16 bits per line in hex format
//	It also sets tags for found Headers, Trailers, or Signature Words
//==============================================================================================

#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;


// == Prints four 16-bits words in Hex 
void printb(unsigned short* buf)
{
	for (int i=0; i<4; i++) 
	cout << " " << setw(4)<< setfill('0') << hex << buf[i];
	cout << dec;
};


// == Main =======================================================================================

int main(int argc, char **argv)
{
	bool fDDUHeader=false, fDDUTrailer=false, SampleTag=false;
	
	long EventToPrint=-1;
	long cntDDUHeaders=0, cntDDUTrailers=0, cntDMBHeaders=0;
	long DDU_L1A=0, DMB_L1A=0, ALCT_L1A=0, TMB_L1A=0;
	long SampleCount=0, BSampleCount=0;

	// == Define 16bit words buffer size of 4
	unsigned short buf_2[4], buf_1[4], buf0[4], buf1[4], buf2[4]; 

	// == Set buffer to 0's
	bzero(buf_2, sizeof(buf_2));
	bzero(buf_1, sizeof(buf_1));
	bzero(buf0, sizeof(buf0));
	bzero(buf1, sizeof(buf1));
	bzero(buf2, sizeof(buf2));
	
	string datafile="";

	// == Process command line options
	switch (argc) {
        case 2: datafile = argv[1];
                break;
  	}
	
	// == Open input data file
	// ifstream input(datafile.c_str());
	
        int input = ::open(datafile.c_str(), O_RDONLY | O_LARGEFILE);

	if (input<0) { perror(datafile.c_str()); return -1; }
	cerr << datafile << " Opened" << endl;
		
	cout << "Enter Event Number to print: ";
	cin >> EventToPrint;

//------------------------------------------------------------------------------------------------

	// == Read from datafile 4 16-bit words into buf till end-of-file is found
	// while( !input.eof() ) 
	while (::read(input,(char *)buf2, sizeof(buf2)))
	{
		
		for (int i=0; i<4; i++) {
		buf_2[i]=buf_1[i];
		buf_1[i]=buf0[i];
		buf0[i]=buf1[i];
		buf1[i]=buf2[i];
		}
		
		// == Read 8 bytes into buffer
		// input.read((char *)buf2, sizeof(buf2));
		
		
		// == Check for Format Control Words
		
		// == DDU Header found
		if ( /*(buf0[0]==0x8000) &&*/ 
		     (buf0[1]==0x8000) && 
		     (buf0[2]==0x0001) && 
		     (buf0[3]==0x8000) ) {
		      cntDDUHeaders++; // Increment DDU Header counter
		      cntDMBHeaders=0;  // Reset DMB Header counter

			if(cntDDUHeaders%1000==0) {
		        cout << "DDU Header Occurrence " << cntDDUHeaders << endl;
			}

			if(cntDDUHeaders==EventToPrint) {
			DDU_L1A = ((buf_1[2]&0xFFFF) + ((buf_1[3]&0x00FF) << 16));
			cout << endl << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl << endl;
			cout << "DDU  Header Occurrence " << cntDDUHeaders << endl;
			cout << endl << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl << endl;

		        cout << "<DDU|      L1A= " << DDU_L1A << 
				"    ( " <<  (buf1[0]&0x000F) << " DMBs in event )" << endl;
			}

			if (cntDDUHeaders == (EventToPrint+1)) {  // Passed the requested Event, Exit the program
			// input.close();
			::close(input);
			cerr << datafile << " Closed" << endl;
			return 0;
			} 
		}


		// == Print the whole event
		if(cntDDUHeaders==EventToPrint) { 

		// == DMB Header found

        if(  ((buf0[0]&0xF000)==0x9000) && 
		     ((buf0[1]&0xF000)==0x9000) && 
		     ((buf0[2]&0xF000)==0x9000) && 
		     ((buf0[3]&0xF000)==0x9000) &&
             ((buf1[0]&0xF000)==0xA000) && 
		     ((buf1[1]&0xF000)==0xA000) && 
		     ((buf1[2]&0xF000)==0xA000) && 
             ((buf1[3]&0xF000)==0xA000) )   {
		       SampleCount=0; 
		       BSampleCount=0;
		       cntDMBHeaders++;
			   DMB_L1A =  ((buf0[0]&0x0FFF) + ((buf0[1]&0x0FFF) << 12));
		       cout << " " << endl << "<DMB"<<cntDMBHeaders<<"|     L1A= " << DMB_L1A << 
		               "   ( " << ((buf0[2]&0x0400)>>10) << " ALCT, "
			               << ((buf0[2]&0x0800)>>11) << " TMB, "
				       << ((buf0[2]&0x0010)>>4) << 
				          ((buf0[2]&0x0008)>>3) << 
					  ((buf0[2]&0x0004)>>2) <<
					  ((buf0[2]&0x0002)>>1) <<
					  ((buf0[2]&0x0001)>>0) << " CFEBs in event )" << endl;
		}


//KK start
        if(  ((buf_1[0]&0xF000)!=0x9000) &&
		     ((buf_1[1]&0xF000)!=0x9000) &&
		     ((buf_1[2]&0xF000)!=0x9000) &&
		     ((buf_1[3]&0xF000)!=0x9000) &&
             ((buf0[0]&0xF000)==0xA000) &&
		     ((buf0[1]&0xF000)==0xA000) &&
		     ((buf0[2]&0xF000)==0xA000) &&
             ((buf0[3]&0xF000)==0xA000) )   {
//KK end
		       SampleCount=0;
		       BSampleCount=0;
		       cntDMBHeaders++;
			   DMB_L1A =  ((buf0[0]&0x0FFF) + ((buf0[1]&0x0FFF) << 12));
		       cout << " " << endl << "<DMB"<<cntDMBHeaders<<"|     L1A= " << DMB_L1A <<
		               "   ( " << ((buf0[0]&0x0400)>>10) << " ALCT, "
			               << ((buf0[0]&0x0800)>>11) << " TMB, "
				       << ((buf0[0]&0x0010)>>4) <<
				          ((buf0[0]&0x0008)>>3) <<
					  ((buf0[0]&0x0004)>>2) <<
					  ((buf0[0]&0x0002)>>1) <<
					  ((buf0[0]&0x0001)>>0) << " CFEBs in event )" << endl;
		}


		// == ALCT Header found right after DMB Header 
                //   (check for all currently reserved/fixed bits in ALCT first 4 words)
		if( ( ((buf0[0]&0xF800)==0x6000) &&
		      ((buf0[1]&0xFF80)==0x0080) &&
		      ((buf0[2]&0xF000)==0x0000) &&
		      ((buf0[3]&0xc000)==0x0000) )
                            && 
		    ( ((buf_1[0]&0xF000)==0xA000) && 
		      ((buf_1[1]&0xF000)==0xA000) && 
		      ((buf_1[2]&0xF000)==0xA000) && 
		      ((buf_1[3]&0xF000)==0xA000) )  )  {
		      ALCT_L1A = (buf0[0]&0x000F);
		       cout << " " << endl << "<ALCT|     L1A= " << ALCT_L1A <<  endl;
		}


		// == TMB Header found right after DMB Header or right after ALCT Trailer		      
		if( ( ((buf0[0]&0xFFFF) ==0x6B0C) &&  
		      ((buf_1[0]&0xF000)==0xA000) && 
		      ((buf_1[1]&0xF000)==0xA000) && 
		      ((buf_1[2]&0xF000)==0xA000) && 
		      ((buf_1[3]&0xF000)==0xA000) ) 
                              ||
		    ( ((buf0[0]&0xFFFF) ==0x6B0C) &&  
		      ((buf_1[0]&0xF800)==0xD000) && 
		      ((buf_1[1]&0xF800)==0xD000) && 
		      ((buf_1[2]&0xFFFF)==0xDE0D) && 
		      ((buf_1[3]&0xF000)==0xD000) )   )   {
		      TMB_L1A = (buf0[2]&0x000F);
		       cout << " " << endl << "<TMB|      L1A= " << TMB_L1A <<  endl;
		}      



		// == CFEB Sample Trailer found
		if ( ((buf0[1]&0xF000)==0x7000) && 
		     ((buf0[2]&0xF000)==0x7000) && 
		     ((buf0[3]&0xFFFF)==0x7FFF) )   {
			       SampleCount++;
			       SampleTag=true;
		}      




		// == CFEB B-word found
		if ( ((buf0[0]&0xF000)==0xB000) && 
		     ((buf0[1]&0xF000)==0xB000) && 
		     ((buf0[2]&0xF000)==0xB000) && 
		     ((buf0[3]&0xF000)==0xB000) )   {
			       SampleCount++;
			       SampleTag=true;
		 }      



		// print buf0 (and maybe buf_1) in hex
		if ( (buf0[0]==0x8000) && 
	    	(buf0[1]==0x8000) && 
	     	(buf0[2]==0x0001) && 
	     	(buf0[3]==0x8000) ) {
		printb(buf_1);
		cout << endl;
		}
		printb(buf0);
		
		if ( SampleTag ) { cout << "  |CFEB sample "<<SampleCount<<">" << endl; SampleTag=false; }



		// == ALCT Trailer found
		if ( ((buf0[0]&0xF800)==0xD000) && 
		     ((buf0[1]&0xF800)==0xD000) && 
		     ((buf0[2]&0xFFFF)==0xDE0D) && 
		     ((buf0[3]&0xF000)==0xD000) )   {
		       cout << "  |ALCT> " << endl;
		}      



		// == TMB Trailer found
		if ( ((buf0[0]&0xF000)==0xD000) && 
		     ((buf0[1]&0xF000)==0xD000) && 
		     ((buf0[2]&0xFFFF)==0xDE0F) && 
		     ((buf0[3]&0xF000)==0xD000) )   {
		       cout << "  |TMB> " << endl;
		}      




		// == DMB F- and E-Trailers found
		if ( ((buf_1[0]&0xF000)==0xF000) && 
		     ((buf_1[1]&0xF000)==0xF000) && 
		     ((buf_1[2]&0xF000)==0xF000) && 
		     ((buf_1[3]&0xF000)==0xF000) &&
		     ((buf0[0]&0xF000)==0xE000) && 
		     ((buf0[1]&0xF000)==0xE000) && 
		     ((buf0[2]&0xF000)==0xE000) && 
		     ((buf0[3]&0xF000)==0xE000) )   {
		          cout << "  |DMB"<<cntDMBHeaders<<">;" << endl;
		          SampleCount=0;
		          BSampleCount=0; 
		}

		
		cout << endl;


		} // End of dumping the bulk of the requested event
		  		

		if ( (buf0[0]==0x8000) && 
		     (buf0[1]==0x8000) && 
		     (buf0[2]==0xFFFF) && 
		     (buf0[3]==0x8000) ) {
		     cntDDUTrailers++; // Increment DDUTrailer counter
		     
		     if(cntDDUHeaders==EventToPrint) {
		     printb(buf1); cout << endl;
		     printb(buf2); cout << "  |DDU>" << endl;
		     cout << endl << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl << endl;

		     cout << "DDU Trailer Occurrence " << cntDDUTrailers << endl << endl;

		     cout << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl << endl;
		     // Exit the program
		     // input.close();
		     ::close(input);
		     cerr << datafile << " Closed" << endl << endl;
		     return 0;
		     }
		}


	
	}


	// == Close input data file	
//	input.close();
	::close(input);
	cerr << datafile << " Closed" << endl;


// == Exit from program
	return 0;
}
