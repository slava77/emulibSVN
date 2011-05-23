#ifndef __AFEB_teststand_utils_System_h__
#define __AFEB_teststand_utils_System_h__

#include "log4cplus/logger.h"
#include "xcept/Exception.h"

#include <string>
#include <vector>

using namespace std;

namespace AFEB { namespace teststand { namespace utils {

      string readFile( const string fileName ) throw ( xcept::Exception );
      
      void writeFile( const string fileName, const string content ) throw ( xcept::Exception );
      
      vector<string> execShellCommand( const string shellCommand ) throw ( xcept::Exception );
      
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
}}}

#endif
