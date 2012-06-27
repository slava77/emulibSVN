#ifndef _FileWriter_h_
#define _FileWriter_h_

#include <string>
#include <sstream>
#include <fstream>
#include "log4cplus/logger.h"
#include <stdio.h>

class FileWriter{

private:
  unsigned long  maxFileSize_; // bytes
  string         pathToFile_;
  string         filePrefix_;
  Logger         logger_;
  unsigned long  runNumber_;
  unsigned long  bytesInFileCounter_;
  unsigned long  eventsInFileCounter_;
  unsigned long  filesInRunCounter_;
  unsigned long  bytesInRunCounter_;
  unsigned long  eventsInRunCounter_;
  string         fileName_;
  std::fstream *fs_;

  string nameFile(){
    ostringstream fileNameStream;
    fileNameStream << pathToFile_ << "/" << filePrefix_ <<"_Run";
    fileNameStream.fill('0');
    fileNameStream.width(5);
    fileNameStream << runNumber_ << "_";
    fileNameStream.width(3);
    fileNameStream << filesInRunCounter_ << ".bin";
    return fileNameStream.str();
  }

  void open(){
    fileName_ = nameFile();
    fs_->open(fileName_.c_str(), ios::out | ios::binary);
    bytesInFileCounter_  = 0;
    eventsInFileCounter_ = 0;
    filesInRunCounter_++;
    LOG4CPLUS_INFO( logger_, "Opened " << fileName_ );
  }


  void close(){ 
    fs_->close();
    LOG4CPLUS_INFO( logger_, "Wrote "                           << 
		    eventsInFileCounter_ << " events ("         << 
		    bytesInFileCounter_  << " bytes) to "       << 
		    fileName_ 	   << "; so far "               << 
		    eventsInRunCounter_  << " events ("         << 
		    bytesInRunCounter_   << " bytes) in run "   << 
		    runNumber_ );
  }


public:
  FileWriter(const unsigned long maxFileSize, const string pathToFile, const string prefix, const Logger* logger)
    :maxFileSize_         (maxFileSize)
    ,pathToFile_          (pathToFile)
    ,filePrefix_          (prefix)
    ,logger_              (*logger)
    ,bytesInFileCounter_  (0)
    ,eventsInFileCounter_ (0)
    ,filesInRunCounter_   (0)
    ,bytesInRunCounter_   (0)
    ,eventsInRunCounter_  (0)
  {
    fs_ = new std::fstream();
    LOG4CPLUS_DEBUG( logger_, "New file writer has been created." );
  }

  ~FileWriter(){ delete fs_; }

  void startNewRun( const int runNumber ){
    runNumber_          = runNumber;
    bytesInRunCounter_  = 0;
    eventsInRunCounter_ = 0;
    open();
  }

  void writeData( const char* buf, const int nBytes ){ 
    fs_->write(buf, nBytes);
    bytesInFileCounter_ += nBytes;
    bytesInRunCounter_  += nBytes;
  }

  void startNewEvent(){
    eventsInFileCounter_++;
    eventsInRunCounter_++;
    if ( bytesInFileCounter_ > maxFileSize_ ){
      close();
      open();
    }
  }

  void endRun(){ 
    close();
    LOG4CPLUS_INFO( logger_, 
		    "End of run "       <<
		    runNumber_          << ". Wrote "  <<
		    eventsInRunCounter_ << " events (" <<
		    bytesInRunCounter_  <<" bytes) in "<<
		    filesInRunCounter_  <<" file"      <<
		    (filesInRunCounter_==1?".":"s.")      );
  }

  long getFileSize(){
    // Returns file size in bytes, or -1 if file size couldn't be obtained.
    if ( fs_->is_open() ){
      // If it's still open, synchronize buffer, then get size
      long size = (long) ( fs_->flush().seekp(0,ios::end).tellp() );
      if ( fs_->fail() ) return long(-1);
      else               return size;
    }
    else{
      fs_->open( fileName_.c_str(), ios::in | ios::binary );
      if ( fs_->fail() ){ 
	return long(-1);
      }
      else{
	long size = (long) ( fs_->seekg(0,ios::end).tellg() );
	fs_->close();
	return size;
      }
    }
    return long(-1);
  }

  void removeFile(){
    if ( fs_->is_open() ) fs_->close();
    if ( ::remove( fileName_.c_str() ) == -1 ){
      LOG4CPLUS_WARN( logger_, "Error deleting file " << fileName_ );
    }
    else{
      LOG4CPLUS_INFO( logger_, "Deleted file " << fileName_ );
    }
  }

};


#endif //#ifndef _FileWriter_h_