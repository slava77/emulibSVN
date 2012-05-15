#include "AFEB/teststand/utils/System.h"
#include "AFEB/teststand/utils/IO.h"
#include "AFEB/teststand/utils/String.h"
#include "toolbox/regex.h"

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

string AFEB::teststand::utils::performExpansions( const string toExpand ){
  string expanded;
  try{
    redi::ipstream command( ( std::string("echo ") + toExpand ).c_str() );
    string reply;
    while ( std::getline( command, reply ) ) {
      expanded += reply;
    }
  }
  catch( std::exception& e ){
    stringstream ess; ess << "Expansion failed. Failed to execute shell command \"echo " << toExpand << "\": " << e.what();
    XCEPT_RAISE( xcept::Exception, ess.str() );
  }
  return expanded;
}

string AFEB::teststand::utils::getDateTime(){
  time_t t;
  struct tm tm;

  time( &t );
  localtime_r( &t, &tm );

  stringstream ss;
  ss << setfill('0') << setw(4) << tm.tm_year+1900 << "-"
     << setfill('0') << setw(2) << tm.tm_mon+1     << "-"
     << setfill('0') << setw(2) << tm.tm_mday      << "T"
     << setfill('0') << setw(2) << tm.tm_hour      << ":"
     << setfill('0') << setw(2) << tm.tm_min       << ":"
     << setfill('0') << setw(2) << tm.tm_sec;

  return ss.str();
}

AFEB::teststand::utils::SCSI_t AFEB::teststand::utils::getSCSI( const string vendor, const string model ){
  // /proc/scsi/scsi has entries like this:
  // Host: scsi3 Channel: 00 Id: 00 Lun: 00
  //   Vendor: JORWAY   Model: 73A              Rev: 208 
  //   Type:   Unknown                          ANSI SCSI revision: 02
  AFEB::teststand::utils::SCSI_t scsi;
  string scsiInfo = readFile("/proc/scsi/scsi" );
  const string regex1( "^[[:blank:]]*Host:[[:blank:]]+scsi([[:digit:]]{1,3})[[:blank:]]+Channel:[[:blank:]]+([[:digit:]][[:digit:]])[[:blank:]]+Id:[[:blank:]]+([[:digit:]][[:digit:]])[[:blank:]]+Lun:[[:blank:]]+([[:digit:]][[:digit:]])[[:blank:]]*$" );
  const string regex2( "^[[:blank:]]*Vendor: ([[:print:]]+)Model: ([[:print:]]+)Rev: ([[:print:]]+)$" );

  vector<string> matches;
  vector<string> lines = utils::splitSting( scsiInfo, "\n" );

  try{
    for ( vector<string>::iterator l = lines.begin(); l != lines.end(); ++l ){
      if ( toolbox::regx_match( *l, regex1, matches ) ){
	scsi.host    = utils::stringTo<int>( matches[1] );
	scsi.channel = utils::stringTo<int>( matches[2] );
	scsi.id      = utils::stringTo<int>( matches[3] );
	scsi.lun     = utils::stringTo<int>( matches[4] );
	//cout << matches.size() << " regex1 matches: " << endl << matches << endl;
      }
      if ( toolbox::regx_match( *l, regex2, matches ) ){
	scsi.vendor   = utils::shaveOffBlanks( matches[1] );
	scsi.model    = utils::shaveOffBlanks( matches[2] );
	scsi.revision = utils::shaveOffBlanks( matches[3] );
	//cout << matches.size() << " regex2 matches: " << endl << matches << endl;
	// Require strict match for the vendor name. As for the model name, it's enough if it starts with the given string.
	if ( scsi.vendor.compare( vendor ) == 0 && 
	     scsi.model .find   ( model  ) == 0    ) return scsi;
      }
    }
    XCEPT_RAISE( xcept::Exception, string( "No SCSI device of vendor \"" ) + vendor + "\" and model \"" + model + "\" found.");
  }
  catch( xcept::Exception &e ){
    XCEPT_RETHROW( xcept::Exception, "Failed to get SCSI parameters.", e );
  }
  catch( std::exception &e ){
    stringstream ss;
    ss << "Failed to get SCSI parameters. " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( ... ){
    XCEPT_RAISE( xcept::Exception, "Failed to get SCSI parameters. Unknown exception." );
  }

  return scsi;
}

string AFEB::teststand::utils::findSCSIDevice( AFEB::teststand::utils::SCSI_t scsi ){

  const char *fullPathTo_devices     = "/proc/scsi/sg/devices";
  const char *fullPathTo_device_strs = "/proc/scsi/sg/device_strs";
  int host, chan, id, lun, type, opens, qdepth, busy, online; // See /proc/scsi/sg/device_hdr
  string vendor, model;
  const int nGenericDevices = 256; // There are 256 possible SCSI generic (sg) devices: /dev/sg{0..255}

  fstream devices    ( fullPathTo_devices    , fstream::in );
  fstream device_strs( fullPathTo_device_strs, fstream::in );
  int iLine = 0; // The line number corresponds to the SCSI generic device number in /dev/sg<N>
  while ( devices.good() && device_strs.good() && iLine < nGenericDevices ){
    const int lineLength = 256;
    char line_devices[lineLength];
    char line_device_strs[lineLength];
    devices    .getline( line_devices    , lineLength );
    device_strs.getline( line_device_strs, lineLength );
    if ( ! devices.fail() && ! device_strs.fail() ){
      stringstream ss_line_devices    ( line_devices     );
      ss_line_devices >> host >> chan >> id >> lun >> type >> opens >> qdepth >> busy >> online;
      if ( scsi.host    == host &&
	   scsi.channel == chan &&
	   scsi.id      == id   &&
	   scsi.lun     == lun     ){
	stringstream ss_line_device_strs( line_device_strs );
	ss_line_device_strs >> vendor >> model;
	// Require strict match for the vendor name. As for the model name, it's enough if it starts with the given string.
	if ( scsi.vendor.compare( vendor ) == 0 && scsi.model.find( model ) == 0 ){
	  stringstream scsiDevice;
	  scsiDevice << "/dev/sg" << iLine;
	  return scsiDevice.str();
	}
      }
    }
    ++iLine;
  }

  return string("");
}
