#include "AFEB/teststand/Jorway73A.h"
#include "AFEB/teststand/utils/System.h"

#include "xcept/Exception.h"

AFEB::teststand::Jorway73A::Jorway73A( const int crateNumber ) :
  CrateController( "Jorway73A", crateNumber ),
  branch_( findBranch() )
{}

int AFEB::teststand::Jorway73A::findBranch(){
  // See Jorway/src/sjy_cdchn.c how to get bus and id (==>branch=SCSI id + (8*SCSI bus)) from /proc/scsi/scsi
  // cat /proc/scsi/scsi
  // Host: scsi3 Channel: 00 Id: 00 Lun: 00
  //           ^bus               ^id     ==> In this example, branch=24.
  //   Vendor: JORWAY   Model: 73A              Rev: 208 
  //   Type:   Unknown                          ANSI SCSI revision: 02
  const string vendor( "PIONEER" );
  const string model( "BD-ROM" );
  // const string vendor( "JORWAY" );
  // const string model( "73A" );
  AFEB::teststand::utils::SCSI_t scsi;
  try{
    scsi = AFEB::teststand::utils::getSCSI( vendor, model );
  }
  catch( xcept::Exception &e ){
    stringstream ss;
    ss << "Failed to find the Jorway73A SCSI device. (Looked for vendor: " << vendor 
       << ", model: " << model << ")";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  // TODO: see if /dev/sg* is read/writeable.
  cout << "Found SCSI device:"            << endl
       << " vendor:  " << scsi.vendor  << endl
       << " model:   " << scsi.model   << endl
       << " host:    " << scsi.host    << endl
       << " channel: " << scsi.channel << endl
       << " id:      " << scsi.id      << endl
       << " lun:     " << scsi.lun     << endl
       <<                                 endl
       << "'Branch' for Jorway73A: "
       << scsi.id + 8 * scsi.host      << endl;
  return ( scsi.id + 8 * scsi.host );
}

//=======================================
// Implement the generic virtual methods
//=======================================

void AFEB::teststand::Jorway73A::initialize() const{
  // cdchn( branch, channel, route )
  // SCSI device will be closed if channel=0
  // route seems to be dummy.
  // cdchn( branch_, 0, 0 ); TODO: do we need to close it?
  cdchn( branch_, 1, 0 );
}

//=====================================
// Implement the CAMAC virtual methods
//=====================================

void AFEB::teststand::Jorway73A::z() const{
  // We only need the branch and crate for cccz. Pick any valid station and subaddress to make ::cdreg happy.
  const unsigned int dummy = 1; // an arbitrary valid station and subaddress
  // Encode branch and crate in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, dummy, dummy );
  ::cccz( ext );
}

void AFEB::teststand::Jorway73A::c() const{
  // We only need the branch and crate for cccc. Pick any valid station and subaddress to make ::cdreg happy.
  const unsigned int dummy = 1; // an arbitrary valid station and subaddress
  // Encode branch and crate in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, dummy, dummy );
  ::cccc( ext );
}

bool AFEB::teststand::Jorway73A::x() const{
  return ::xrespn();
}

bool AFEB::teststand::Jorway73A::q() const{
  return ::qrespn();
}

unsigned short AFEB::teststand::Jorway73A::lam() const { // TODO: implement. (Needed?)
  return 0;
}

void AFEB::teststand::Jorway73A::write( const unsigned int data, 
					const Subaddress_t subaddress, 
					const Function_t function,
					const Station_t station ) const {
  unsigned short dataw = static_cast<unsigned short>(data);
  // Encode everything in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, station, subaddress );
  // Execute a single 16 bit data transfer function
  int qres[4];
  ::cssa( function, ext, &dataw, qres );
}

unsigned int AFEB::teststand::Jorway73A::read( const Subaddress_t subaddress, 
					       const Function_t function,
					       const Station_t station ) const{
  unsigned short dataw = 0;
  // Encode full address in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, station, subaddress );
  // cout << " ext " << hex << ext
  //      << " branch_ " << dec << branch_
  //      << " crateNumber_ " << crateNumber_
  //      << " station " << station
  //      << " subaddress " << subaddress
  //      << endl;

  // Execute a single 16 bit data transfer function
  int qres[4] = {0,0,0,0};

  // cout << " function " << function
  //      << " ext " << hex << ext
  //      << " dataw " << dataw
  //      << " qres[0] " << qres[0]
  //      << " qres[3] " << qres[3]
  //      << endl;
  ::cssa( function, ext, &dataw, qres );
  // cout << " function " << function
  //      << " ext " << hex << ext
  //      << " dataw " << dataw
  //      << " qres[0] " << qres[0]
  //      << " qres[3] " << qres[3]
  //      << endl;

  return dataw;
}

void AFEB::teststand::Jorway73A::readBlock( const Subaddress_t subaddress, // TODO: is subaddress needed here?
					    const Function_t function,
					    const Station_t station,
					    unsigned short *data,
					    const int blockSize ) const{
  // Encode full address in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, station, subaddress );
  // Execute a single slot 16 bit "DMA" CAMAC operation
  int qres[4];
  qres[0] = blockSize;
  qres[3] = 0;
  ::csubc( function, ext, &data[0], qres );
}

void AFEB::teststand::Jorway73A::execute( const Subaddress_t subaddress, 
					  const Function_t function,
					  const Station_t station ) const{
  // Encode full address in an external address
  int ext;
  ::cdreg( &ext, branch_, crateNumber_, station, subaddress );
    // Execute a single 16 bit data transfer function
  unsigned short dataw = 0;
  int qres[4];
  ::cssa( function, ext, &dataw, qres );
}
