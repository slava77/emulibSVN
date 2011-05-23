#include "AFEB/teststand/Jorway73A.h"
#include "AFEB/teststand/utils/System.h"

#include "xcept/Exception.h"

AFEB::teststand::Jorway73A::Jorway73A( const int crateNumber ) :
	CrateController( "Jorway73A", crateNumber ),
	CAMAC( getBranch(), crateNumber )
{}

int AFEB::teststand::Jorway73A::getBranch(){
  // See Jorway/src/sjy_cdchn.c how to get bus and id (==>branch=SCSI id + (8*SCSI bus)) from /proc/scsi/scsi
  // cat /proc/scsi/scsi
  // Host: scsi3 Channel: 00 Id: 00 Lun: 00
  //           ^bus               ^id     ==> In this example, branch=24.
  //   Vendor: JORWAY   Model: 73A              Rev: 208 
  //   Type:   Unknown                          ANSI SCSI revision: 02
  const string vendor( "JORWAY" );
  const string model( "73A" );
  AFEB::teststand::utils::SCSI_t scsi = AFEB::teststand::utils::getSCSI( vendor, model );
  if ( scsi.vendor != vendor || scsi.model != model ){
    stringstream ss;
    ss << "Failed to find the Jorway73A SCSI device. (Looked for vendor: " << vendor 
       << ", model: " << model << ")";
    cout << ss.str() << endl; // XCEPT_RAISE( xcept::Exception, ss.str() ); // TODO: throw exception
  }
  return ( scsi.id + 8 * scsi.host );
}
