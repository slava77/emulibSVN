#include "AFEB/teststand/utils/System.h"

#include <exception>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <time.h>
#include "pstream.h"

using namespace std;

string
AFEB::teststand::utils::readFile( const string fileName )
  throw ( xcept::Exception ){
  stringstream ss;
  try{
    fstream inFile( fileName.c_str(), fstream::in );
    while ( inFile.good() ){
      char c = char( inFile.get() );
      if ( inFile.good() ) ss << c;
    }
  }
  catch( std::exception& e ){
    stringstream ess; ess << "Failed to read file \"" << fileName << "\": " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }
  //cout << "AFEB::teststand::utils::readFile" << endl << ss.str() << endl; 
  return ss.str();
}

void
AFEB::teststand::utils::writeFile( const string fileName, const string content )
  throw ( xcept::Exception ){
  try{
    fstream outFile( fileName.c_str(), fstream::out );
    outFile << content;
  }
  catch( std::exception& e ){
    stringstream ess; ess << "Failed to write file \"" << fileName << "\": " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }
}

vector<string> AFEB::teststand::utils::execShellCommand( const string shellCommand )
  throw ( xcept::Exception ){
  vector<string> replyLines;
  try{
    redi::ipstream command( shellCommand.c_str() );
    string reply;
    while ( std::getline( command, reply ) ) {
      replyLines.push_back(reply);
    }
  }
  catch( std::exception& e ){
    stringstream ess; ess << "Failed to execute shell command \"" << shellCommand << "\": " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }
  return replyLines;
}

string AFEB::teststand::utils::getDateTime(){
  time_t t;
  struct tm *tm;

  time ( &t );
  tm = gmtime ( &t );

  stringstream ss;
  ss << setfill('0') << setw(4) << tm->tm_year+1900 << "-"
     << setfill('0') << setw(2) << tm->tm_mon+1     << "-"
     << setfill('0') << setw(2) << tm->tm_mday      << " "
     << setfill('0') << setw(2) << tm->tm_hour      << ":"
     << setfill('0') << setw(2) << tm->tm_min       << ":"
     << setfill('0') << setw(2) << tm->tm_sec       << " UTC";

  return ss.str();
}
