#include "emu/pc/Configuration.h"

#include "emu/pc/XMLParser.h"
#include "emu/pc/EmuEndcapConfigWrapper.h"
#include "emu/db/TStoreReadWriter.h"
#include "emu/db/PCConfigHierarchy.h"
#include "emu/db/XMLReadWriter.h"

#include "emu/utils/System.h"
#include "emu/utils/String.h"
#include "emu/utils/IO.h"
#include "emu/utils/DOM.h"

#include "xcept/tools.h"

using namespace std;

const char* const emu::pc::Configuration::sourceString_[] = { "none", "file", "db", "string" };

ostream&
emu::pc::operator<<( ostream& os, const emu::pc::Configuration& configuration ){
  using namespace emu::utils;
  os << "emu::pc::Configuration from " << configuration.getSourceString();
  if      ( configuration.source_ == emu::pc::Configuration::fileSource ) os << " " << *configuration.ids_.begin();
  else if ( configuration.source_ == emu::pc::Configuration::dbSource   ) os << " id " << configuration.ids_;
  os << endl;
  return os;
}

emu::pc::Configuration::Configuration()
  : bSem_( toolbox::BSem::EMPTY ) // born locked
  , source_( emu::pc::Configuration::noSource )
  , parentApplication_( NULL )
  , emuEndcap_( NULL ){
  bSem_.give();
}

emu::pc::Configuration::Configuration( const emu::pc::Configuration& other )
  : bSem_( toolbox::BSem::EMPTY ){
  source_            = other.getSource();
  ids_               = other.getIds();
  creationDateTime_  = other.getCreationDateTime();
  tStoreViewFile_    = other.getTStoreViewFile();
  parentApplication_ = other.getParentApplication();
  emuEndcap_         = other.getEmuEndcap();
  xml_               = other.getXML();
  bSem_.give();
}

emu::pc::Configuration::~Configuration(){
  delete emuEndcap_;
}

emu::pc::Configuration& emu::pc::Configuration::setTStoreViewFile( const string& tStoreViewFile ){
  bSem_.take();
  tStoreViewFile_ = emu::utils::performExpansions( tStoreViewFile );
  bSem_.give();
  return *this;
}

emu::pc::Configuration& emu::pc::Configuration::setParentApplication( xdaq::Application* const parentApp ){
  bSem_.take();
  parentApplication_ = parentApp;
  bSem_.give();
  return *this;
}

emu::pc::Configuration& emu::pc::Configuration::setFrom( const emu::pc::Configuration::Source_t source, const string& id, const int ethPort, const std::set<int> crateIds ){
  bSem_.take();
  try{
    if      ( source == emu::pc::Configuration::fileSource   ) createFromFile  ( id, ethPort, crateIds );
    else if ( source == emu::pc::Configuration::dbSource     ) createFromDB    ( id, ethPort, crateIds );
    else if ( source == emu::pc::Configuration::stringSource ) createFromString( id, ethPort, crateIds );
    else{
      XCEPT_RAISE( xcept::Exception, 
		   string( "Invalid configuration source '" ) 
		   + emu::utils::stringFrom<emu::pc::Configuration::Source_t>( source_ ) + "'." );
    }
    source_ = source;
  }
  catch( xcept::Exception& e ){
    stringstream ss;
    ss <<  "Failed to create configuration: " << xcept::stdformat_exception_history(e);
    bSem_.give();
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( std::exception& e ){
    stringstream ss;
    ss <<  "Failed to create configuration: " << e.what();
    bSem_.give();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    stringstream ss;
    ss <<  "Failed to create configuration: unknown exception caught.";
    bSem_.give();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  bSem_.give();
  return *this;
}

emu::pc::Configuration& emu::pc::Configuration::unset(){
  bSem_.take();
  delete emuEndcap_;
  emuEndcap_ = NULL;
  source_ = emu::pc::Configuration::noSource;
  ids_.clear();
  xml_.clear();
  creationDateTime_.clear();
  bSem_.give();
  return *this;
}

emu::pc::Configuration& emu::pc::Configuration::recordTimeOfWritingFlash(){
  if ( !isSet() || source_ != emu::pc::Configuration::dbSource ) return *this;
  bSem_.take();
  try{
    emu::db::PCConfigHierarchy hierarchy( tStoreViewFile_ );
    emu::db::TStoreReadWriter tStore( &hierarchy, parentApplication_, "", 0 );
    for ( std::set<std::string>::iterator iid = ids_.begin(); iid != ids_.end(); ++ iid ){
      xdata::UnsignedInteger64 id = 0;
      id.fromString( *iid );
      tStore.writeFlashTime( id );
    }
  }
  catch( xcept::Exception& e ){
    bSem_.give();
    XCEPT_RETHROW( xcept::Exception, "Failed to record in db the time of writing FLASH.", e );
  }
  bSem_.give();
  return *this;
}

void emu::pc::Configuration::createFromFile( const string& fileName, const int ethPort, const std::set<int> crateIds ){
  ids_.clear();
  ids_.insert( fileName );
  // XMLParser should not delete EmuEndcap when destructed so we can use a local instance of it.
  XMLParser parser;
  // parser.parseFile( fileName );
  xml_ = selectCrates( emu::utils::readFile( fileName ), crateIds );
  xml_ = setEthPort( xml_, ethPort ); 
  parser.parseString( xml_ );
  emuEndcap_ = parser.GetEmuEndcap();
  creationDateTime_ = emu::utils::getDateTime();
}

void emu::pc::Configuration::createFromDB( const string& ids, const int ethPort, const std::set<int> crateIds ){
  if ( parentApplication_ == NULL ){
    XCEPT_RAISE( xcept::Exception, "No parent application specified." );
  }
  emu::db::PCConfigHierarchy hierarchy( tStoreViewFile_ );
  emu::db::TStoreReadWriter tStore( &hierarchy, parentApplication_, "", 0 );

  //
  // Get the sanitized id(s) from the argument.
  //
  set<int> sanitizedIds = emu::utils::csvTo<set<int> >( ids );
  if ( sanitizedIds.size() > 2 ){
    XCEPT_RAISE( xcept::Exception, "More than two DB configuration ids specified." );
  }
  if ( sanitizedIds.find( 0 ) != sanitizedIds.end() ){
    XCEPT_RAISE( xcept::Exception, "DB configuration id is empty." );
  }
 
  //
  // Check if we really need to create them or they're already flashed.
  //
  set<string> flashedIds;
  for ( set<int>::iterator id = sanitizedIds.begin(); id != sanitizedIds.end(); ++id ){
    if ( *id == -1 || *id == -2 ){
      flashedIds.insert( tStore.readLastConfigIdFlashed( ( *id == -1 ) ? "plus" : "minus" ).toString() );
    }
  }
  if ( source_ == emu::pc::Configuration::dbSource && ids_ == flashedIds ) return; // No need to (re)create it/them.

  //
  // Get the real ids to be used.
  //
  ids_.clear();
  for ( set<int>::iterator id = sanitizedIds.begin(); id != sanitizedIds.end(); ++id ){
    if ( *id == -1 || *id == -2 ){
      ids_.insert( tStore.readLastConfigIdFlashed( ( *id == -1 ) ? "plus" : "minus" ).toString() );
    }
    else{
      ids_.insert( emu::utils::stringFrom<int>( *id ) );
    }
  }

  //
  // Create the EmuEndcap objects with the selected crates
  //
  for ( set<string>::iterator id = ids_.begin(); id != ids_.end(); ++id ){
    tStore.read( emu::utils::stringTo<uint64_t>( *id ) );
    emu::db::ConfigTree tree( tStore.configTables() );
    xml_ = selectCrates( emu::db::XMLReadWriter( &hierarchy, string() ).makeXML( &tree ), crateIds );
    xml_ = setEthPort( xml_, ethPort ); 
    XMLParser parser;
    parser.parseString( xml_ );
    emuEndcap_ = parser.GetEmuEndcap();
    if ( emuEndcap_ == NULL ){
      ostringstream oss;
      oss << "No EmuEndcap created from id " << *id
	  << " in configuration db for crates " << crateIds << ".";
      XCEPT_RAISE( xcept::Exception, oss.str() );
    }
  }
  // TODO: Merge EmuEndcap objects if more than one

  creationDateTime_ = emu::utils::getDateTime();
}

void emu::pc::Configuration::createFromString( const string& xml, const int ethPort, const std::set<int> crateIds ){
  ids_.clear();
  ids_.insert( "see master instance" );
  xml_ = selectCrates( xml, crateIds );
  xml_ = setEthPort( xml_, ethPort ); 
  // XMLParser should not delete EmuEndcap when destructed.
  XMLParser parser;
  parser.parseString( xml_ );
  emuEndcap_ = parser.GetEmuEndcap();
  creationDateTime_ = emu::utils::getDateTime();
}

bool emu::pc::Configuration::isSet() const {
  bool isSet = false;
  bSem_.take();
  isSet = ( source_ != emu::pc::Configuration::noSource        && 
	    ids_.size() > 0                                    &&
	    find( ids_.begin(), ids_.end(), "" ) == ids_.end() && 
	    emuEndcap_ != NULL                                    );
  bSem_.give();
  return isSet;
}

set<string> emu::pc::Configuration::getIds() const {
  bSem_.take();
  set<string> id( ids_ );
  bSem_.give();
  return id;
}

vector<string> emu::pc::Configuration::getAvailableDBIds( const emu::pc::Configuration::System_t system ) const {
  if ( parentApplication_ == NULL ){
    XCEPT_RAISE( xcept::Exception, "No parent application specified." );
  }
  emu::db::PCConfigHierarchy hierarchy( tStoreViewFile_ );
  emu::db::TStoreReadWriter tStore( &hierarchy, parentApplication_, "", 0 );
  vector<string> ids;
  if ( system & emu::pc::Configuration::plusSide ){
    vector<string> i = tStore.readIDs( 1, 500 );
    ids.insert( ids.end(), i.begin(), i.end() );
  }
  if ( system & emu::pc::Configuration::minusSide ){
    vector<string> i = tStore.readIDs( 2, 500 );
    ids.insert( ids.end(), i.begin(), i.end() );
  }
  return ids;
}

string emu::pc::Configuration::getXML() const {
  string xml;
  bSem_.take();
  xml = xml_;
  bSem_.give();
  return xml;
}

string emu::pc::Configuration::selectCrates( const std::string& xml, const set<int>& crateIds ) const {
  // If no crate id specified, we keep them all.
  if ( crateIds.empty() ) return xml;
  // Build XPath expression selecting all unwanted crates
  ostringstream xpath;
  xpath << "//EmuSystem/PeripheralCrate[";
  for ( set<int>::iterator ci = crateIds.begin(); ci != crateIds.end(); ++ci ){
    xpath << "@crateID!='" << *ci << "'";
    set<int>::iterator nextci = ci;
    ++nextci;
    if ( nextci != crateIds.end() ) xpath << " and ";
  }
  xpath << "]";
  // Remove unwanted crates
  return emu::utils::removeSelectedNode( xml, xpath.str() );
}

string emu::pc::Configuration::setEthPort( const std::string& xml, const int ethPort ) const {
  // If no eth port specified, we make no changes.
  if ( ethPort < 0 ) return xml;
  // Set eth port for all crates
  return emu::utils::setSelectedNodesValues( xml, "//EmuSystem/PeripheralCrate/VCC/@eth_port", 
					     emu::utils::stringFrom<int>( ethPort ) );
}
