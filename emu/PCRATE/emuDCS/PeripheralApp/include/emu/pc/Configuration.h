#ifndef __emu_pc_Configuration_h__
#define __emu_pc_Configuration_h__

#include "toolbox/BSem.h"
#include "emu/pc/EmuEndcap.h"
#include "emu/db/TStoreReadWriter.h"

#include <string>
#include <vector>
#include <set>
#include <sstream>

namespace emu { 
  namespace pc {

    class Configuration{
      
    public:
      enum Source_t { noSource, fileSource, dbSource, stringSource, nSources };
      enum System_t { noSystem=0x00, plusSide=0x01, minusSide=0x02, bothSides=0x03 };
      static const char* const sourceString_[ nSources ];

      friend std::ostream& operator<<( std::ostream& os, const emu::pc::Configuration& configuration );

      Configuration();		///< default ctor
      Configuration( const emu::pc::Configuration& other );  ///< copy ctor
      ~Configuration();
      Configuration& setTStoreViewFile( const std::string& tStoreViewFile );
      Configuration& setParentApplication( xdaq::Application* const parentApp );
      Configuration& setFrom( const emu::pc::Configuration::Source_t source, 
			      const std::string& id,
			      const int ethPort = -1,
			      const std::set<int> crateIds = std::set<int>() );
      Configuration& unset();
      Configuration& recordTimeOfWritingFlash();
      bool isSet() const;
      Source_t getSource() const { return source_; };
      std::string getSourceString() const { return sourceString_[ source_ ]; };
      std::set<std::string> getIds() const;
      std::string getCreationDateTime() const { return creationDateTime_; }
      std::string getTStoreViewFile() const { return tStoreViewFile_; }
      std::vector<std::string> getAvailableDBIds( const emu::pc::Configuration::System_t system ) const;
      EmuEndcap* getEmuEndcap() const { return emuEndcap_; }
      xdaq::Application* getParentApplication() const { return parentApplication_; }
      std::string getXML() const;

    private:
      std::string selectCrates( const std::string& xml, const std::set<int>& crateIds ) const;
      std::string setEthPort( const std::string& xml, const int ethPort ) const;
      void createFromFile( const std::string& fileName, const int ethPort, const std::set<int> crateIds );
      void createFromDB( const std::string& id, const int ethPort, const std::set<int> crateIds );
      void createFromString( const std::string& xml, const int ethPort, const std::set<int> crateIds );

      mutable toolbox::BSem bSem_; ///< binary semaphore, exempt from constness
      Source_t source_;
      std::set<std::string> ids_;
      std::string creationDateTime_;
      std::string tStoreViewFile_;
      xdaq::Application* parentApplication_;
      EmuEndcap *emuEndcap_;
      std::string xml_;
    };

  }
}

#endif
