#ifndef __emu_pc_AppNavigator_h__
#define __emu_pc_AppNavigator_h__

#include <string>
#include <vector>

namespace emu { 
  namespace pc {

    class AppNavigator;

    class AppNavigator : public std::vector< std::pair<std::string,std::string> >{
      
    public:
      AppNavigator& addLink( const std::string& name, const std::string& href = emu::pc::AppNavigator::noHRef );
      std::string getHTML() const;
      static const std::string noHRef;
    };

  }
}

#endif
