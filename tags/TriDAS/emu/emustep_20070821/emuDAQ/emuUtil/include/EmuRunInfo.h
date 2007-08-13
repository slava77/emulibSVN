#ifndef __EmuRunInfo_h__
#define __EmuRunInfo_h__

#include "pstream.h"
#include <string>
#include <iterator>
// #include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
using namespace redi;


class EmuRunInfo{
  // EmuRunInfo is a destructible singleton.
  // Instantiate at configuration:
  // EmuRunInfo *ri = EmuRunInfo::Instance("dooobee","doobee","dooo");
  // Delete it when the run is stopped, and, just in case, when it's configured.
  // delete ri;

private:

  static EmuRunInfo* instance_;

  string  bookingCommand_;
  string  writingCommand_;
  string  dbUserFile_;
  string  dbAddress_;

  string  dbUser_;
  string  dbPassword_;
  string  user_;

  unsigned int runNumber_;
  unsigned int runSequenceNumber_;
  string runNumberString_;
  string sequenceName_;
  string runSequenceNumberString_;

  string errorMessage_;

  static const string runNumKeyword_;
  static const string seqNameKeyword_;
  static const string runSeqNumKeyword_;
  static const string bookingSuccessKeyword_;
  static const string updateSuccessKeyword_;


protected:

//   EmuRunInfo();
  EmuRunInfo( const string bookingCommand, 
	      const string writingCommand, 
	      const string dbUserFile, 
	      const string dbAddress ):
    bookingCommand_  (bookingCommand),
    writingCommand_  (writingCommand),
    dbUserFile_      (dbUserFile    ),
    dbAddress_       (dbAddress     ),
    dbUser_          (""            ),
    dbPassword_      (""            ),
    runNumber_       (0             ),
    runNumberString_ (""            ),
    errorMessage_    (""            )
  {
    findOutWhoIAm();
    getDbUserData();
  }

public:

  static EmuRunInfo* Instance( const string bookingCommand, 
			       const string writingCommand, 
			       const string dbUserFile, 
			       const string dbAddress ){
    
    if ( instance_ == 0 )  // is it the first call?
      instance_ = new EmuRunInfo( bookingCommand, writingCommand, dbUserFile, dbAddress ); // create sole instance

    return instance_; // address of sole instance

  }

  ~EmuRunInfo(){
    instance_ = 0; // make sure it can be instantiated next time
  }

  void findOutWhoIAm(){

    ipstream who("whoami");
    while (who >> user_);

  }

  void getDbUserData(){
    
    fstream fs;
    fs.open( dbUserFile_.c_str(), ios::in );
    if ( fs.is_open() ){
      string userAndPasswd;
      fs >> userAndPasswd;
      string::size_type loc = userAndPasswd.find(":");
      if ( loc == string::npos ){
	stringstream oss;
	oss << dbUserFile_ << " contains no username:password. ==> No run number database access.";
	throw oss.str();
      }
      dbUser_     = userAndPasswd.substr( 0, loc );
      dbPassword_ = userAndPasswd.substr( loc+1 );
      fs.close();
    }
    else{
      stringstream oss;
      oss << "Could not open " << dbUserFile_ << " for reading. ==> No run number database access.";
      throw oss.str();
    }

  }

  bool bookRunNumber( const string sequence ){

    errorMessage_ = "";
    bool success  = false;

    if ( runNumberString_ != "" ) {
      stringstream ss;
      ss << "Already booked run number " << runNumberString_ 
	 << ". Cannot book another until this run is stopped." << endl;
      errorMessage_ = ss.str();
      return false;
    }

    string command = bookingCommand_;
    command += " ";
    command += dbAddress_;
    command += " ";
    command += dbUser_;
    command += " ";
    command += dbPassword_;
    command += " ";
    command += user_;
    command += " ";
    command += sequence;

//     cout << command << endl;

    string reply;
    ipstream book( command.c_str() );
//     while (book >> reply){
//       cout << reply;
//     }
//     cout << endl;

    vector<string> replyLines;
    while ( std::getline(book, reply) ) {
      replyLines.push_back(reply);
    }
//     copy( replyLines.begin(), replyLines.end(), ostream_iterator<string>(cout, "\n") );

    vector<string>::const_iterator line;
    for ( line = replyLines.begin(); line != replyLines.end(); ++line ){

      string::size_type keywordPos;

      keywordPos = (*line).find(runNumKeyword_,0);
      if ( keywordPos == 0 ){
	runNumberString_ = (*line).substr( keywordPos + runNumKeyword_.length() + 1 );
	stringstream ss( runNumberString_ );
	ss >> runNumber_;
      }

      keywordPos = (*line).find(seqNameKeyword_,0);
      if ( keywordPos == 0 )
	sequenceName_ = (*line).substr( keywordPos + seqNameKeyword_.length() + 1 );

      keywordPos = (*line).find(runSeqNumKeyword_,0);
      if ( keywordPos  == 0 ){
	runSequenceNumberString_ = (*line).substr( keywordPos + runSeqNumKeyword_.length() + 1 );
	stringstream ss( runSequenceNumberString_ );
	ss >> runSequenceNumber_;
      }

      keywordPos = (*line).find(bookingSuccessKeyword_,0);
      if ( keywordPos  == 0 ) success = true;

    }

    if ( !success )
      for ( line = replyLines.begin(); line != replyLines.end(); ++line ) errorMessage_ += *line;

    return success;

  }
  
  bool writeRunInfo( const string name, const string value, const string nameSpace ){

    errorMessage_ = "";
    bool success = false;

    string command = writingCommand_;
    command += " ";
    command += dbAddress_;
    command += " ";
    command += dbUser_;
    command += " ";
    command += dbPassword_;
    command += " ";
    command += runNumberString_;
    command += " \"";
    command += name;
    command += "\" \"";
    command += value;
    command += "\" ";
    command += nameSpace;

//     cout << command << endl;

    string reply;
    ipstream write( command.c_str() );

    vector<string> replyLines;
    while ( std::getline(write, reply) ) {
      replyLines.push_back(reply);
    }

//     copy( replyLines.begin(), replyLines.end(), ostream_iterator<string>(cout, "\n") );

    vector<string>::const_iterator line;
    for ( line = replyLines.begin(); line != replyLines.end(); ++line )
      success |= ( (*line).find(updateSuccessKeyword_,0) == 0 );

    if ( !success )
      for ( line = replyLines.begin(); line != replyLines.end(); ++line ) 
	errorMessage_ += *line;

    return success;

  }
  
  unsigned int runNumber()        { return runNumber_; }
  unsigned int runSequenceNumber(){ return runSequenceNumber_; }
  string runNumberString()        { return runNumberString_; }
  string runSequenceNumberString(){ return runSequenceNumberString_; }
  string sequenceName()           { return sequenceName_; }
  string errorMessage()           { return errorMessage_; }
};

EmuRunInfo* EmuRunInfo::instance_ = 0; // init pointer

const string EmuRunInfo::runNumKeyword_   ("RUN_NUMBER"         );
const string EmuRunInfo::seqNameKeyword_  ("SEQUENCE_NAME"      );
const string EmuRunInfo::runSeqNumKeyword_("RUN_SEQUENCE_NUMBER");
const string EmuRunInfo::bookingSuccessKeyword_("Created Run Sequence Number");
const string EmuRunInfo::updateSuccessKeyword_ ("Updated parameter"          );

#endif
