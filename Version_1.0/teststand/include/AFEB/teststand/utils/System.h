#ifndef __AFEB_teststand_utils_System_h__
#define __AFEB_teststand_utils_System_h__

#include "log4cplus/logger.h"
#include "xcept/Exception.h"

#include <string>
#include <vector>

using namespace std;

namespace AFEB { namespace teststand { namespace utils {

      /// 
      /// Read in a file.
      /// @param fileName Name of file to read. 
      ///
      /// @return Content of the file.
      ///
      string readFile( const string fileName ) throw ( xcept::Exception );

      /// 
      /// Create a file and write \c content into it.
      /// @param fileName Name of file to create.
      /// @param content Content of file to write to \c fileName
      ///
      void writeFile( const string fileName, const string content ) throw ( xcept::Exception );

      /// 
      /// Execute the argument in a shell.
      /// @param shellCommand 
      ///
      /// @return A vector of lines of the reply.
      ///
      vector<string> execShellCommand( const string shellCommand ) throw ( xcept::Exception );

      /// 
      /// Have the shell perform expansions (parameter, file name, etc.) on the argument.
      /// @param toExpand The string to expand.
      ///
      /// @return The (parameter, file name, etc.)-expanded string.
      ///
      string performExpansions( const string toExpand );

      /// Gets UTC date and time.
      ///
      ///
      /// @return UTC date and time in YYYYMMDD_hhmmss_UTC format
      ///
      string getDateTime();
      
      struct SCSI_t {
	int host;
	int channel;
	int id;
	int lun;
	string vendor;
	string model;
	string revision;
      };
      
      SCSI_t getSCSI( const string vendor, const string model );
      string findSCSIDevice( SCSI_t scsi );
}}}

#endif
